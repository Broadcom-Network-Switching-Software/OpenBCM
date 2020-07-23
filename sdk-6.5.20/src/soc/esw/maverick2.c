/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        maverick2.c
 * Purpose:
 * Requires:
 */


#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/dpc.h>
#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>
#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#endif

#include <soc/bradley.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/i2c.h>
#include <soc/l2x.h>

#ifdef BCM_MAVERICK2_SUPPORT

#include <soc/mmu_config.h>
#include <soc/maverick2.h>
#include <soc/maverick2_tdm.h>
#include <soc/tomahawk.h>
#include <soc/esw/cancun.h>
#include <soc/soc_ser_log.h>
#include <soc/flexport/maverick2/maverick2_flexport_defines.h>
#include <soc/esw/portctrl.h>
#include <soc/bondoptions.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif

#define MV2_NUM_PHY_PORT                (82)
#define MV2_NUM_PORT                    (66)
#define MV2_NUM_MMU_PORT                (66)
#define MV2_MGMT_PHY_PORT               (81)

#define PORT_BLOCK_BASE_PORT(port) \
        (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0x3));

#define IS_MGMT_PORT(unit,port) IS_MANAGEMENT_PORT(unit, port)

#define IS_OVERSUB_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), port))

int
_soc_mv2_port_speed_cap[SOC_MAX_NUM_DEVICES][MAVERICK2_PHY_PORTS_PER_DEV];

STATIC int
_soc_mv2_post_mmu_init_update(int unit)
{
    int port;
    soc_info_t *si = &SOC_INFO(unit);

    PBMP_ALL_ITER(unit, port) {
        /* Set init speed to max speed by default */
        si->port_init_speed[port] = si->port_speed_max[port];
        if (_soc_mv2_port_speed_cap[unit][port]) {
            /* If cap speed is available then adjust max speed for further use */
            si->port_speed_max[port] = _soc_mv2_port_speed_cap[unit][port];
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_maverick2_mem_config
 * Purpose:
 *      Configure depth of UFT/UAT memories
 * Parameters:
 *      unit (IN)    - switch unit
 * Returns:
 *      SOC_E_*
 */
int
soc_maverick2_mem_config(int unit)
{
    soc_persist_t *sop;
    sop = SOC_PERSIST(unit);

    SOC_IF_ERROR_RETURN(soc_trident3_mem_config(unit));

    /* Memories with ACC_TYPE=ADDR_SPLIT_DIST needed to be
     * set to max for 66 logical ports.
     */
    sop->memState[EGR_MAP_MHm].index_max = (16 * MV2_NUM_PORT) - 1;
    sop->memState[IFP_STORM_CONTROL_METERSm].index_max = (4 * MV2_NUM_PORT) - 1;

    /* EDATABUF memories indexed by physical ports.
     * Set to MV2 max physical ports (MV2_NUM_PHY_PORT).
     */
    sop->memState[EGR_ENABLEm].index_max = MV2_NUM_PHY_PORT;
    sop->memState[EGR_MMU_CELL_CREDITm].index_max = MV2_NUM_PHY_PORT;
    sop->memState[EGR_PORT_CREDIT_RESETm].index_max = MV2_NUM_PHY_PORT;
    sop->memState[EGR_PER_PORT_BUFFER_SFT_RESETm].index_max = MV2_NUM_PHY_PORT;

    return SOC_E_NONE;
}

int
soc_maverick2_num_cosq_init_port(int unit, int port)
{
    soc_info_t *si;
    int mmu_port;

    si = &SOC_INFO(unit);

    mmu_port = SOC_TD3_MMU_PORT(unit, port);

    if (IS_CPU_PORT(unit, port)) {
        si->port_num_cosq[port] = 48;
        si->port_cosq_base[port] = SOC_MV2_CPU_MCQ_BASE;
    } else if (IS_LB_PORT(unit, port)) {
        si->port_num_cosq[port] = 10;
        si->port_cosq_base[port] = SOC_MV2_LB_MCQ_BASE;
    } else if (IS_MGMT_PORT(unit, port)) {
        si->port_num_cosq[port] = 10;
        si->port_num_uc_cosq[port] = 10;
        si->port_cosq_base[port] = SOC_MV2_MGMT_MCQ_BASE;
        si->port_uc_cosq_base[port] = SOC_MV2_MGMT_UCQ_BASE;
    } else {
        si->port_num_cosq[port] = 10;
        si->port_cosq_base[port] =
            (mmu_port % 64) * 10;
        si->port_num_uc_cosq[port] = 10;
        si->port_uc_cosq_base[port] =
            (mmu_port % 64) * 10;
        si->port_num_ext_cosq[port] = 0;
    }

    return SOC_E_NONE;
}

int
soc_maverick2_num_cosq_init(int unit)
{
    int port;

    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_maverick2_num_cosq_init_port(unit, port));
    }

    return SOC_E_NONE;
}


STATIC void
_soc_maverick2_max_frequency_get(int unit,  uint16 dev_id, uint8 rev_id,
                                 int skew_id, int *frequency)
{
    soc_info_t *si;

    si = &SOC_INFO(unit);
    *frequency = soc_property_get(unit, spn_CORE_CLOCK_FREQUENCY, CCLK_FREQ_1525MHZ);

    switch (dev_id) {
    case BCM56771_DEVICE_ID:
        si->bandwidth = 1280000;
        si->io_bandwidth = 1280000;
        break;
    default:
        si->bandwidth = 2000000;
        si->io_bandwidth = 2000000;
        break;
    }
}

/* Function to get the number of ports present in a given Port Macro */
STATIC int
_soc_mv2_ports_per_pm_get(int unit, int pm_id)
{
    soc_info_t *si;
    int phy_port, num_ports = 0, i;

    si = &SOC_INFO(unit);
    if (pm_id >= _MV2_PBLKS_PER_DEV(unit)) {
       return SOC_E_PARAM;
    }
    phy_port = 1 + (pm_id * _MV2_PORTS_PER_PBLK);

    for (i = 0; i < _MV2_PORTS_PER_PBLK; ) {
        if (si->port_p2l_mapping[phy_port + i] != -1) {
            num_ports ++;
            i += si->port_num_lanes[si->port_p2l_mapping[phy_port + i]];
        } else {
            i ++;
        }
    }
    return num_ports;
}

STATIC int
_soc_mv2_port_flex_init(int unit, int is_any_cap)
{
    soc_info_t *si;
    int pm, blk_idx, blk_type;
    int flex_en, static_ports, max_ports;

    si = &SOC_INFO(unit);
    SHR_BITCLR_RANGE(si->flexible_pm_bitmap, 0, SOC_MAX_NUM_BLKS);

    /* portmap_x=y:speed:i */
    /* portmap_x=y:speed:cap */
    if (SOC_PBMP_NOT_NULL(SOC_PORT_DISABLED_BITMAP(unit, all)) || is_any_cap) {
        SHR_BITSET_RANGE(si->flexible_pm_bitmap, 1, _MV2_PBLKS_PER_DEV(unit));
    } else {
        /* port_flex_enable{x}=<0...1> */
        /* port_flex_enable_corex=<0...1> */
        /* port_flex_enable=<0...1> */
        for (blk_idx = 0; blk_idx < SOC_MAX_NUM_BLKS; blk_idx++) {
            blk_type = SOC_BLOCK_TYPE(unit, blk_idx);
            pm = SOC_BLOCK_NUMBER(unit, blk_idx);
            if (blk_type == SOC_BLK_CLPORT) {
       /* port_flex_enable_corex=<0...1> */
                flex_en = soc_property_suffix_num_get_only_suffix(unit,
                    pm, spn_PORT_FLEX_ENABLE, "core", -1);
                /* port_flex_enable{x}=<0...1> */
                /* port_flex_enable=<0...1> */
                if (flex_en == -1) {
                    flex_en = soc_property_phys_port_get(unit,
                        (pm * _MV2_PORTS_PER_PBLK + 1),
                        spn_PORT_FLEX_ENABLE, 0);
                }
                if (flex_en) {
                    SHR_BITSET(si->flexible_pm_bitmap, blk_idx);
                }
            }
        }
    }

    si->flex_eligible =
        (!SHR_BITNULL_RANGE(si->flexible_pm_bitmap, 1, _MV2_PBLKS_PER_DEV(unit)));

    /* port_flex_max_ports */
    memset(&(si->pm_max_ports), 0xff, sizeof(si->pm_max_ports));
    for (blk_idx = 0; blk_idx < SOC_MAX_NUM_BLKS; blk_idx++) {
        blk_type = SOC_BLOCK_TYPE(unit, blk_idx);
        pm = SOC_BLOCK_NUMBER(unit, blk_idx);
        if (blk_type == SOC_BLK_CLPORT) {
            static_ports = _soc_mv2_ports_per_pm_get(unit, pm);
            if (SHR_BITGET(si->flexible_pm_bitmap, blk_idx)) {
                /* port_flex_max_ports{x}=y */
                max_ports = soc_property_phys_port_get(unit,
                    (pm * _MV2_PORTS_PER_PBLK + 1),
                    spn_PORT_FLEX_MAX_PORTS, -1);
                /* port_flex_max_ports_corex=y */
                if (max_ports == -1) {
                    max_ports = soc_property_suffix_num_get(unit, pm,
                        spn_PORT_FLEX_MAX_PORTS, "core", 4);
                }
                /* validation check */
                if ((max_ports < 0) || (max_ports > 4) || (max_ports < static_ports)) {
                    LOG_CLI((BSL_META_U(unit,
                        "Core %d: Bad port_flex_max_ports %d; static ports %d"),
                        pm, max_ports, static_ports));
                    return SOC_E_CONFIG;
                }
                si->pm_max_ports[blk_idx] = max_ports;
            } else {
                si->pm_max_ports[blk_idx] = static_ports;
            }

        }
    }

    return SOC_E_NONE;
}

int
soc_mv2_port_lanes_set(int unit, int phy_port, int *lanes, int *port_bandwidth)
{
    int rv = SOC_E_NONE;

    *lanes =   (*port_bandwidth > 53000) ? 4
             : (*port_bandwidth > 42000) ? 2
             : (*port_bandwidth > 27000) ? 2
             : (*port_bandwidth == SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(21000)) ? 2
             : 1;

    if (soc_feature(unit, soc_feature_untethered_otp)) {
        uint32 max_lane_speed = 0;

        SHR_BITCOPY_RANGE(&max_lane_speed, 0, SOC_BOND_INFO(unit)->tsc_max_speed,
                          ((phy_port - 1) / _MV2_PORTS_PER_PBLK) * 2, 2);
        if (max_lane_speed == 2) {
            if (*port_bandwidth > 11000) {
                rv = SOC_E_CONFIG;
            }
            *port_bandwidth = 11000;
       }
    }
    return rv;
}

STATIC int
_soc_mv2_mmu_idb_ports_assign(int unit)
{
    int port, phy_port;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    /* Assign idb port and handle mgmt port*/
    for (phy_port = 1; phy_port < MV2_NUM_PHY_PORT; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        } else if (port >= 33 && phy_port <= 40) {
            /* Half Pipe 0 */
            LOG_CLI((BSL_META_U(unit,
                                "Physical port %d: mapped to Half Pipe 1\n"),
                                phy_port));
            return SOC_E_CONFIG;
        } else if (port <= 32 && phy_port > 40) {
            /* Half Pipe 1 */
            LOG_CLI((BSL_META_U(unit,
                                "Physical port %d: mapped to Half Pipe 0\n"),
                                phy_port));
            return SOC_E_CONFIG;
        }

        if (phy_port == MV2_MGMT_PHY_PORT) { /* management port */
            SOC_PBMP_PORT_ADD(si->management_pbm, port);
        }
        si->port_p2m_mapping[phy_port] = port - 1;
        si->port_l2i_mapping[port] = si->port_p2m_mapping[phy_port];
        si->port_serdes[port] = (phy_port - 1) / _MV2_PORTS_PER_PBLK;
        si->port_pipe[port] = 0;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[0], port);
        if (si->oversub_mode) {
            SOC_PBMP_PORT_ADD(si->oversub_pbm, port);
        }
    }
    return SOC_E_NONE;
}

/*
 * Maverick2 port mapping
 *
 *                   physical   idb/       device   mmu
 *                   port       idb port   port     port
 *     CMIC          0          64         0        64
 *     CLPORT0-19    1-80       0-61       1-62     0-61
 *     MGMT          81         63         64       63
 *     LBPORT0       82         65         65       65
 *
 *     MMU port number for a CLPORT and MGMT ports is fixed as (device port - 1
 *     where device port depends on the config.)
 *     CPU and LBPORT mappings are fixed.
 */
int
soc_maverick2_port_config_init(int unit, uint16 dev_id)
{
    soc_info_t *si;
    char *config_str, *sub_str, *sub_str_end;
    static const char str_2p5[] = "2.5";
    char str_buf[8];
    int rv;
    int num_port, num_phy_port, num_mmu_port;
    int port, phy_port, mmu_port;
    int pipe, index, bandwidth_cap, is_any_cap = FALSE;
    int port_bandwidth, blk_idx, hpipe_bw[2] = {0};
    char option1, option2;
    static const struct {
        int port;
        int phy_port;
        int pipe;
        int idb_port;
        int mmu_port;
    } fixed_ports[] = {
        { 0,   0,   0, 64, 64 },    /* cpu port */
        { 65,  82,  0, 65, 65 },    /* loopback port 0 */
    };

    si = &SOC_INFO(unit);

    num_phy_port = MV2_NUM_PHY_PORT;
    num_port = MV2_NUM_PORT;
    num_mmu_port = MV2_NUM_MMU_PORT;

    _soc_maverick2_max_frequency_get(unit, dev_id, -1, -1, &si->frequency);
    si->oversub_mode = soc_property_get(unit, spn_OVERSUBSCRIBE_MODE, 1);
    si->os_mixed_sister_25_50_enable = soc_property_get(unit,
                         spn_OVERSUBSCRIBE_MIXED_SISTER_25_50_ENABLE, 0);
    if (si->oversub_mode != 1 ) {
        return SOC_E_CONFIG;
    }

    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        si->port_p2l_mapping[phy_port] = -1;
        si->port_p2m_mapping[phy_port] = -1;
    }
    for (port = 0; port < num_port; port++) {
        si->port_l2p_mapping[port] = -1;
        si->port_l2i_mapping[port] = -1;
        si->port_speed_max[port] = -1;
        si->port_group[port] = -1;
        si->port_serdes[port] = -1;
        si->port_pipe[port] = -1;
        si->port_num_lanes[port] = -1;
    }
    for (mmu_port = 0; mmu_port < num_mmu_port; mmu_port++) {
        si->port_m2p_mapping[mmu_port] = -1;
    }
    SOC_PBMP_CLEAR(si->eq_pbm);
    SOC_PBMP_CLEAR(si->management_pbm);
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        SOC_PBMP_CLEAR(si->pipe_pbm[pipe]);
    };
    SOC_PBMP_CLEAR(si->oversub_pbm);
    SOC_PBMP_CLEAR(si->all.disabled_bitmap);

    /* Populate the fixed mapped ports */
    for (index = 0; index < COUNTOF(fixed_ports); index++) {
        port = fixed_ports[index].port;
        phy_port = fixed_ports[index].phy_port;
        pipe = fixed_ports[index].pipe;;

        si->port_l2p_mapping[port] = phy_port;
        si->port_l2i_mapping[port] = fixed_ports[index].idb_port;
        si->port_p2l_mapping[phy_port] = port;
        si->port_p2m_mapping[phy_port] = fixed_ports[index].mmu_port;
        si->port_pipe[port] = pipe;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);
    }

    rv = SOC_E_NONE;
    for (port = 1; port < num_port; port++) {
        if (si->port_l2p_mapping[port] != -1) { /* fixed mapped port */
            continue;
        }

        config_str = soc_property_port_get_str(unit, port, spn_PORTMAP);
        if (config_str == NULL) {
            continue;
        }

        /*
         * portmap_<port>=<physical port number>:<bandwidth in Gb>[:<bandwidth cap in Gb/i(inactive)/1/2/4(num lanes)>][:<i>]
         * eg:    portmap_1=1:100
         *     or portmap_1=1:40
         *     or portmap_1=1:40:i
         *     or portmap_1=1:40:2
         *     or portmap_1=1:40:2:i
         *     or portmap_1=1:10:100
         */
        sub_str = config_str;

        /* Parse physical port number */
        phy_port = sal_ctoi(sub_str, &sub_str_end);
        if (sub_str == sub_str_end) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Missing physical port information \"%s\"\n"),
                     port, config_str));
            rv = SOC_E_FAIL;
            continue;
        }
        if (phy_port < 0 || phy_port >= num_phy_port) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Invalid physical port number %d\n"),
                     port, phy_port));
            rv = SOC_E_FAIL;
            continue;
        }
        /* coverity[check_after_sink : FALSE] */
        if (si->port_p2l_mapping[phy_port] != -1) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Physical port %d is used by port %d\n"),
                     port, phy_port, si->port_p2l_mapping[phy_port]));
            rv = SOC_E_FAIL;
            continue;
        }

        /* Skip ':' between physical port number and bandwidth */
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Bad config string \"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            sub_str++;
        }

        /* Parse bandwidth */
        for (index = 0; index < sizeof(str_2p5) - 1; index++) {
            if (sub_str[index] == '\0') {
                break;
            }
            str_buf[index] = sub_str[index];
        }
        str_buf[index] = '\0';
        if (!sal_strcmp(str_buf, str_2p5)) {
            port_bandwidth = 2500;
            sub_str_end = &sub_str[sizeof(str_2p5) - 1];
        } else {
            port_bandwidth = sal_ctoi(sub_str, &sub_str_end) * 1000;
            if (sub_str == sub_str_end) {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Missing bandwidth information \"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            switch (port_bandwidth) {
            case 1000:
            case 10000:
            case 11000:
            case 20000:
            case 21000:
            case 25000:
            case 27000:
            case 40000:
            case 42000:
            case 50000:
            case 53000:
            case 100000:
            case 106000:
                break;
            default:
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Invalid bandwidth %d Gb\n"),
                         port, port_bandwidth / 1000));
                rv = SOC_E_FAIL;
                continue;
            }
        }

        /* Check if option presents */
        option1 = 0; option2 = 0;
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            /* Skip ':' between bandwidth and options */
            /* Skip ':' between bandwidth and bandwidth cap or options */
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Bad config string \"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            sub_str++;

            if (*sub_str != '\0') {
                /* Parse bandwidth cap or options */
                bandwidth_cap = sal_ctoi(sub_str, &sub_str_end) * 1000;
                switch (bandwidth_cap) {
                case 20000:
                case 21000:
                case 25000:
                case 27000:
                case 40000:
                case 42000:
                case 50000:
                case 53000:
                case 100000:
                case 106000:
                    sub_str = sub_str_end;
                    _soc_mv2_port_speed_cap[unit][port] = bandwidth_cap;
                    /* Flex config detected, port speed cap specified */
                    is_any_cap = TRUE;
                    break;
                default:
                    if (!(*sub_str == 'i' || *sub_str == '1' ||
                          *sub_str == 'm' || *sub_str == '2' ||
                          *sub_str == '4')) {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string "
                                            "\"%s\"\n"),
                             port, config_str));
                        rv = SOC_E_FAIL;
                        continue;
                    } else {
                        option1 = *sub_str;
                        sub_str++;
                    }
                }

                /* Check if more options present */
                if (*sub_str != '\0') {
                    /* Skip ':' between options */
                    if (*sub_str != ':') {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string \"%s\"\n"),
                                 port, config_str));
                        rv = SOC_E_FAIL;
                        continue;
                    }
                    sub_str++;

                    if (*sub_str != 'i' && *sub_str != 'm') {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string \"%s\"\n"),
                                 port, config_str));
                        rv = SOC_E_FAIL;
                        continue;
                    }
                    option2 = *sub_str;
                    sub_str++;
                }
            }
        }

        /* Check trailing string */
        if (*sub_str != '\0') {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Bad config string \"%s\"\n"),
                     port, config_str));
            rv = SOC_E_FAIL;
            continue;
        }

        /* Update soc_info */
        si->port_l2p_mapping[port] = phy_port;
        si->port_p2l_mapping[phy_port] = port;

        si->port_speed_max[port] = port_bandwidth;
        if (phy_port <= MAVERICK2_GPHY_PORTS_PER_PIPE) {
            hpipe_bw[(phy_port - 1) / 40] += MV2_TDM_SPEED_ADJUST(port_bandwidth);
        }
        if (option1 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
            si->port_num_lanes[port] = -1;
        } else {
            switch (option1) {
            case '1' : si->port_num_lanes[port] = 1; break;
            case '2' : si->port_num_lanes[port] = 2; break;
            case '4' : si->port_num_lanes[port] = 4; break;
            default  :
                if (SOC_FAILURE(soc_mv2_port_lanes_set(unit, phy_port,
                                                       &si->port_num_lanes[port],
                                                       &port_bandwidth))) {
                    rv = SOC_E_FAIL;
                }
                break;

            }
        }
        if (option2 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
            si->port_num_lanes[port] = -1;
        }
        if (si->oversub_mode) {
            SOC_PBMP_PORT_ADD(si->oversub_pbm, port);
        }
    }

    if (hpipe_bw[0] > SOC_INFO(unit).io_bandwidth / 2) {
        LOG_CLI((BSL_META_U(unit,
                 "Hpipe0 Bandwidth %dGb exceeds Max Hpipe Bandwidth %dGb\n"),
                 hpipe_bw[0] / 1000, (SOC_INFO(unit).io_bandwidth / 2) / 1000));
        return SOC_E_CONFIG;
    }

    if (hpipe_bw[1] > SOC_INFO(unit).io_bandwidth / 2) {
        LOG_CLI((BSL_META_U(unit,
                 "Hpipe1 Bandwidth %dGb exceeds Max Hpipe Bandwidth %dGb\n"),
                 hpipe_bw[1] / 1000, (SOC_INFO(unit).io_bandwidth / 2) / 1000));
        return SOC_E_CONFIG;
    }

    /* get flex port properties */
    if (SOC_FAILURE(_soc_mv2_port_flex_init(unit, is_any_cap))) {
        rv = SOC_E_FAIL;
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (si->flex_eligible) {
        if (SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN(soc_mv2_flexport_scache_recovery(unit));
        } else {
            SOC_IF_ERROR_RETURN(soc_mv2_flexport_scache_allocate(unit));
        }
    }
#endif

    SOC_IF_ERROR_RETURN(_soc_mv2_mmu_idb_ports_assign(unit));

    si->xpe_ipipe_map[0] = 1;
    si->xpe_epipe_map[0] = 1;
    si->sc_ipipe_map[0] = 1;
    si->sc_epipe_map[0] = 1;
    si->ipipe_xpe_map[0] = 1;
    si->epipe_xpe_map[0] = 1;
    si->ipipe_sc_map[0] = 1;
    si->epipe_sc_map[0] = 1;

    /* Setup clport refclk master block bitmap.
     * In MV2, there are 4 port macros (clport 4,5,14,15) that
     * will driven directly from the system board and they will act as the
     * refclk master to drive the other instances.
     */
    SHR_BITCLR_RANGE(si->pm_ref_master_bitmap, 0, SOC_MAX_NUM_BLKS);
    for (blk_idx = 0; blk_idx < SOC_MAX_NUM_BLKS; blk_idx++) {
        int blknum = SOC_BLOCK_NUMBER(unit, blk_idx);
        int blktype = SOC_BLOCK_TYPE(unit, blk_idx);

        /* CLPORTs 4,5,14,15 provide master clock for other ports */
        if (blktype == SOC_BLK_CLPORT) {
            if ((blknum == 4 || blknum == 5 || blknum == 14 || blknum == 15)) {
                SHR_BITSET(si->pm_ref_master_bitmap, blk_idx);
            }
        }
    }

    return rv;
}

int
soc_maverick2_chip_reset(int unit)
{
#define _SOC_MV2_BSPLL_COUNT    2
#define _SOC_MV2_PLL_CFG_FIELDS 4
#define _SOC_MV2_TEMP_MAX       130
#define _SOC_MV2_DEF_TEMP_MAX   90
    int index;
    soc_reg_t reg;
    uint32 to_usec, temp_mask;
    uint32 rval, temp_thr;
        uint32 dco_code, dco_code_target;
    int ref_freq;
    char blank = 0;
    static const soc_pll_16_param_t ts_pll_params[] = {
        /* 25MHz external reference clock */
        {25000000, 25, 2, 10, 3, 10, 1, 200, {10, 20, 32, 10, 200, 40}},
        /* 50MHz external reference clock */
        {50000000, 50, 2, 10, 3, 10, 1, 100, {10,  0,  0,  0, 200, 40}},
        /* 50MHz internal reference clock from core PLL */
        {       0, 50, 2, 10, 3, 10, 1, 100, {10,  0,  0,  0, 200, 40}}
    };
    static const soc_pll_16_param_t bs_pll_params[] = {
        /* 12.8MHz external reference clock */
        {12800000, 1, 2, 10, 3, 10, 1, 200, {128, 0, 0, 0, 0, 0}},
        /* 20MHz external reference clock */
        {20000000, 1, 2, 10, 3, 10, 1, 125, {125, 0, 0, 0, 0, 0}},
        /* 25MHz external reference clock */
        {25000000, 1, 2, 10, 3, 10, 1, 100, {125, 0, 0, 0, 0, 0}},
        /* 32MHz external reference clock */
        {32000000, 1, 2, 10, 3, 10, 1,  80, {128, 0, 0, 0, 0, 0}},
        /* 50MHz external reference clock */
        {50000000, 1, 2, 10, 3, 10, 1,  50, {125, 0, 0, 0, 0, 0}},
        /* 50MHz internal reference clock from core PLL */
        {       0, 0, 2, 10, 3, 10, 1,  50, {125, 0, 0, 0, 0, 0}}
    };
    static const int auto_ref = -1;
    static const uint32 dco_poison = 0xffff;

    static const soc_reg_t temp_thr_reg[] = {
        TOP_PVTMON_0_INTR_THRESHOLDr, TOP_PVTMON_1_INTR_THRESHOLDr,
        TOP_PVTMON_2_INTR_THRESHOLDr, TOP_PVTMON_3_INTR_THRESHOLDr,
        TOP_PVTMON_4_INTR_THRESHOLDr, TOP_PVTMON_5_INTR_THRESHOLDr,
        TOP_PVTMON_6_INTR_THRESHOLDr, TOP_PVTMON_7_INTR_THRESHOLDr,
        TOP_PVTMON_8_INTR_THRESHOLDr
    };
    static const char *temp_thr_prop[] = {
        "temp0_threshold", "temp1_threshold", "temp2_threshold",
        "temp3_threshold", "temp4_threshold", "temp5_threshold",
        "temp6_threshold", "temp7_threshold", "temp8_threshold"
    };

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);


    soc_maverick2_sbus_ring_map_config(unit);

    sal_usleep(to_usec);


    /* Bring up TS PLL */
    ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, auto_ref);
    index = (ref_freq == auto_ref) ? (COUNTOF(ts_pll_params) - 1) : 0;
    dco_code_target = dco_poison;

    for (; index < COUNTOF(ts_pll_params); index++) {
        if (ref_freq != auto_ref &&
                ts_pll_params[index].ref_freq != ref_freq) {
            continue;
        }

        /* Place in reset */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REG_2r, REG_PORT_ANY,
                                    TOP_TS_PLL_RST_Lf, 0));

        /* Set N-divider value */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_TS_PLL_CTRL_0r, REG_PORT_ANY,
                                    NDIV_INTf, ts_pll_params[index].ndiv));

        /* Select internal or external clock source */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_TS_PLL_CTRL_2r, REG_PORT_ANY,
                            REFCLK_SOURCE_SELf,
                            ts_pll_params[index].ref_freq == 0 ? 0 : 1));

        /* Set new reference frequency info */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_TS_PLL_CTRL_3r, REG_PORT_ANY,
                            FREFEFF_INFOf,
                            ts_pll_params[index].ref_freq_info));

        /* Take out of reset */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REG_2r, REG_PORT_ANY,
                                    TOP_TS_PLL_RST_Lf, 1));

        /* Give time to lock */
        sal_usleep(to_usec);

        /* Check PLL lock status */
        reg = TOP_TS_PLL_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (SAL_BOOT_SIMULATION ||
                soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
            if (ref_freq == auto_ref && dco_code_target == dco_poison) {
                /* If auto freq, get DCO value for later comparison */
                SOC_IF_ERROR_RETURN(
                    _soc_trident3_pll_dco_code_get(unit, SOC_TD3_TS_PLL,
                                                    &dco_code_target));
                _soc_td3_pll_dco_normalize(&dco_code_target,
                                            ts_pll_params[index].ref_freq,
                                            ts_pll_params[index].ndiv);

                /* Start iterating over all of the available frequencies */
                index = -1;
            } else if (ref_freq == auto_ref) {
                /* Make sure the current DCO code is close to the target */
                SOC_IF_ERROR_RETURN(
                    _soc_trident3_pll_dco_code_get(unit, SOC_TD3_TS_PLL,
                                                    &dco_code));

                _soc_td3_pll_dco_normalize(&dco_code,
                                            ts_pll_params[index].ref_freq,
                                            ts_pll_params[index].ndiv);

                if (dco_code >= (dco_code_target - 400) &&
                        dco_code <= (dco_code_target + 400)) {
                    if (ts_pll_params[index].ref_freq) {
                        LOG_WARN(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                "TS_PLL locked on unit %d using "
                                "%d.%dMHz external reference clock\n"
                                "Recommended to add \"%s=%u\" "
                                "config variable\n"),
                                unit,
                                ts_pll_params[index].ref_freq / 1000000,
                                ts_pll_params[index].ref_freq / 100000 % 10,
                                spn_PTP_TS_PLL_FREF,
                                ts_pll_params[index].ref_freq));
                    }
                    break;
                } else {
                    continue;
                }
            } else {
                /* PLL locked at desired frequency */
                break;
            }
        } else if (ref_freq != auto_ref) {
            /* Not able to lock at desired frequency */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                    "TS_PLL failed to lock on unit %d "
                    "status = 0x%08x\n"), unit, rval));
            return SOC_E_INIT;
        }
    }

    /* Bring up BS0 PLL */
    ref_freq = soc_property_suffix_num_get(unit, 0, spn_PTP_BS_FREF,
                                           &blank, auto_ref);
    index = (ref_freq == auto_ref) ? (COUNTOF(bs_pll_params) - 1) : 0;
    dco_code_target = dco_poison;

    for (; index < COUNTOF(bs_pll_params); index++) {
        if (ref_freq != auto_ref &&
                bs_pll_params[index].ref_freq != ref_freq) {
            continue;
        }

        /* Place in reset */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REG_2r, REG_PORT_ANY,
                                    TOP_BS_PLL0_RST_Lf, 0));

        /* Set N-divider value */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_BS_PLL0_CTRL_0r, REG_PORT_ANY,
                                    NDIV_INTf, bs_pll_params[index].ndiv));

        /* Select internal or external clock source */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_BS_PLL0_CTRL_2r, REG_PORT_ANY,
                            REFCLK_SOURCE_SELf,
                            bs_pll_params[index].ref_freq == 0 ? 0 : 1));

        /* Set new reference frequency info */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_BS_PLL0_CTRL_3r, REG_PORT_ANY,
                            FREFEFF_INFOf,
                            bs_pll_params[index].ref_freq_info));

        /* Take out of reset */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REG_2r, REG_PORT_ANY,
                                    TOP_BS_PLL0_RST_Lf, 1));

        /* Give time to lock */
        sal_usleep(to_usec);

        /* Check PLL lock status */
        reg = TOP_TS_PLL_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (SAL_BOOT_SIMULATION ||
                soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
            if (ref_freq == auto_ref && dco_code_target == dco_poison) {
                /* If auto freq, get DCO value for later comparison */
                SOC_IF_ERROR_RETURN(
                    _soc_trident3_pll_dco_code_get(unit, SOC_TD3_BS_PLL0,
                                                    &dco_code_target));
                _soc_td3_pll_dco_normalize(&dco_code_target,
                                            bs_pll_params[index].ref_freq,
                                            bs_pll_params[index].ndiv);

                /* Start iterating over all of the available frequencies */
                index = -1;
            } else if (ref_freq == auto_ref) {
                /* Make sure the current DCO code is close to the target */
                SOC_IF_ERROR_RETURN(
                    _soc_trident3_pll_dco_code_get(unit, SOC_TD3_BS_PLL0,
                                                    &dco_code));
                _soc_td3_pll_dco_normalize(&dco_code,
                                            bs_pll_params[index].ref_freq,
                                            bs_pll_params[index].ndiv);

                /* LOG_WARN(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                        "BS_PLL0: %d %d %d\n"),
                        bs_pll_params[index].ref_freq,
                        dco_code,
                        dco_code_target)); */

                if (dco_code >= (dco_code_target - 400) &&
                        dco_code <= (dco_code_target + 400)) {
                    if (bs_pll_params[index].ref_freq) {
                        LOG_WARN(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                "BS_PLL0 locked on unit %d using "
                                "%d.%dMHz external reference clock\n"
                                "Recommended to add \"%s_0=%u\" "
                                "config variable\n"),
                                unit,
                                bs_pll_params[index].ref_freq / 1000000,
                                bs_pll_params[index].ref_freq / 100000 % 10,
                                spn_PTP_BS_FREF,
                                bs_pll_params[index].ref_freq));
                    }
                    break;
                } else {
                    continue;
                }
            } else {
                /* PLL locked at desired frequency */
                break;
            }
        } else if (ref_freq != auto_ref) {
            /* Not able to lock at desired frequency */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                    "BS_PLL0 failed to lock on unit %d "
                    "status = 0x%08x\n"), unit, rval));
            return SOC_E_INIT;
        }
    }

    /* Bring up BS1 PLL */
    ref_freq = soc_property_suffix_num_get(unit, 1, spn_PTP_BS_FREF,
                                           &blank, auto_ref);
    index = (ref_freq == auto_ref) ? (COUNTOF(bs_pll_params) - 1) : 0;
    dco_code_target = dco_poison;

    for (; index < COUNTOF(bs_pll_params); index++) {
        if (ref_freq != auto_ref &&
                bs_pll_params[index].ref_freq != ref_freq) {
            continue;
        }

        /* Place in reset */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REG_2r, REG_PORT_ANY,
                                    TOP_BS_PLL1_RST_Lf, 0));

        /* Set N-divider value */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_BS_PLL1_CTRL_0r, REG_PORT_ANY,
                                    NDIV_INTf, bs_pll_params[index].ndiv));

        /* Select internal or external clock source */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_BS_PLL1_CTRL_2r, REG_PORT_ANY,
                            REFCLK_SOURCE_SELf,
                            bs_pll_params[index].ref_freq == 0 ? 0 : 1));

        /* Set new reference frequency info */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_BS_PLL1_CTRL_3r, REG_PORT_ANY,
                            FREFEFF_INFOf,
                            bs_pll_params[index].ref_freq_info));

        /* Take out of reset */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REG_2r, REG_PORT_ANY,
                                    TOP_BS_PLL1_RST_Lf, 1));

        /* Give time to lock */
        sal_usleep(to_usec);

        /* Check PLL lock status */
        reg = TOP_TS_PLL_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (SAL_BOOT_SIMULATION ||
                soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
            if (ref_freq == auto_ref && dco_code_target == dco_poison) {
                /* If auto freq, get DCO value for later comparison */
                SOC_IF_ERROR_RETURN(
                    _soc_trident3_pll_dco_code_get(unit, SOC_TD3_BS_PLL1,
                                                    &dco_code_target));
                _soc_td3_pll_dco_normalize(&dco_code_target,
                                            bs_pll_params[index].ref_freq,
                                            bs_pll_params[index].ndiv);

                /* Start iterating over all of the available frequencies */
                index = -1;
            } else if (ref_freq == auto_ref) {
                /* Make sure the current DCO code is close to the target */
                SOC_IF_ERROR_RETURN(
                    _soc_trident3_pll_dco_code_get(unit, SOC_TD3_BS_PLL1,
                                                    &dco_code));
                _soc_td3_pll_dco_normalize(&dco_code,
                                            bs_pll_params[index].ref_freq,
                                            bs_pll_params[index].ndiv);

                if (dco_code >= (dco_code_target - 400) &&
                        dco_code <= (dco_code_target + 400)) {
                    if (bs_pll_params[index].ref_freq) {
                        LOG_WARN(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                "BS_PLL1 locked on unit %d using "
                                "%d.%dMHz external reference clock\n"
                                "Recommended to add \"%s_1=%u\" "
                                "config variable\n"),
                                unit,
                                bs_pll_params[index].ref_freq / 1000000,
                                bs_pll_params[index].ref_freq / 100000 % 10,
                                spn_PTP_BS_FREF,
                                bs_pll_params[index].ref_freq));
                    }
                    break;
                } else {
                    continue;
                }
            } else {
                /* PLL locked at desired frequency */
                break;
            }
        } else if (ref_freq != auto_ref) {
            /* Not able to lock at desired frequency */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                    "BS_PLL1 failed to lock on unit %d "
                    "status = 0x%08x\n"), unit, rval));
            return SOC_E_INIT;
        }
    }
    
    /* De-assert TS PLL, BS PLL0/1 post reset and bring AVS out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                       TOP_TS_PLL_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                       TOP_BS_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                       TOP_BS_PLL1_POST_RST_Lf, 1);
    soc_reg_field_set(unit,TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);

    /* SW WAR - SDK-215192 abnormal small pulses from max_temp_clk output pin */
    rval = 0;
    soc_reg_field_set(unit, AVS_REG_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr, &rval,
                      SEQ_MASK_PVT_MNTRf, 0x7e);
    SOC_IF_ERROR_RETURN(WRITE_AVS_REG_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr(unit, rval));

    /* Enable high temperature interrupt monitoring.
     * Default on: pvtmon[0-7] (enable all pvtmons). */
    temp_mask = soc_property_get(unit, "temp_monitor_select", 
                                 ((1 << COUNTOF(temp_thr_reg)) - 1));
    /* The above is a 8 bit mask to indicate which temp sensor to hook up to
     * the interrupt. */
    rval = 0;
    for (index = 0; index <COUNTOF(temp_thr_reg); index++) {
        uint32 trval;

        /* Temp = 434.10 - o_ADC_data * 0.53504
         * data = (434.10 - temp)/0.53504 = (434100-(temp*1000))/535
         * Note: Since this is a high temp value we can safely assume it to 
         * always be a +ive number. This is in degrees celcius.
         */
        temp_thr = soc_property_get(unit, temp_thr_prop[index], 
                                    _SOC_MV2_DEF_TEMP_MAX);
        if (temp_thr > _SOC_MV2_TEMP_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                          "Unsupported temp value %d !! Max %d. Using %d.\n"),
                          temp_thr, _SOC_MV2_TEMP_MAX, _SOC_MV2_DEF_TEMP_MAX));
            temp_thr = _SOC_MV2_DEF_TEMP_MAX;
        }
        /* Convert temperature to config data */
        temp_thr = (434100-(temp_thr*1000))/535;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, temp_thr_reg[index], REG_PORT_ANY, 0, &trval));
        soc_reg_field_set(unit, temp_thr_reg[index], &trval, MIN_THRESHOLDf,
                          temp_thr);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, temp_thr_reg[index], REG_PORT_ANY, 0, trval));
        if (temp_mask & (1 << index)) {
            rval |= (1 << ((index * 2) + 1)); /*2 bits per pvtmon, using min*/
        }
    }
    soc_td3_temp_mon_mask[unit] = temp_mask;

    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_INTR_MASKr(unit, rval));
    /* Enable temp mon interrupt */
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        
        /* (void)soc_cmicm_intr3_enable(unit, 0x4); PVTMON_INTR bit 2 */
    }
#endif
    /* Bring port blocks out of reset */
    rval = 0;
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, 0x1fffff));
    sal_usleep(to_usec);

   /* Enable IPEP clock gating by default for saving IDLE power.
   *  IPEP clock gating should be disabled if visibility feature support is needed. */
   if (!soc_property_get(unit, spn_IPEP_CLOCK_GATING_DISABLE, 0)) {
        /* Need to set PSG/PCG to 0 before de-assert IP/EP/MMU SOFT reset  */
        SOC_IF_ERROR_RETURN(WRITE_TOP_CLOCKING_ENFORCE_PSGr(unit, 0));
        SOC_IF_ERROR_RETURN(WRITE_TOP_CLOCKING_ENFORCE_PCGr(unit, 0));
    }

    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);

    if (soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, IDB_SER_CONTROLr,
                                                   REG_PORT_ANY,
                                                   IS_TDM_ECC_ENf,1));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, IDB_SER_CONTROLr,
                                                   REG_PORT_ANY,
                                                   CA_CPU_ECC_ENABLEf,1));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, IDB_SER_CONTROLr,
                                                   REG_PORT_ANY,
                                                   CA_LPBK_ECC_ENABLEf,1));
    }

    SOC_IF_ERROR_RETURN(soc_trident3_init_idb_memory(unit));
    SOC_IF_ERROR_RETURN(_soc_trident3_init_hash_control_reset(unit));
    SOC_IF_ERROR_RETURN(soc_trident3_uft_uat_config(unit));

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(soc_trident3_clear_all_memory(unit));
    }

    if (soc_feature(unit, soc_feature_turbo_boot)) {
        uint32 def_val = 0;

        if (!(SOC_CONTROL(unit)->soc_flags & SOC_F_ALL_MODULES_INITED)) {
            def_val = 1;
        }
        if (soc_property_get(unit, "clear_on_hw_resetting", def_val)) {
            SOC_HW_RESET_START(unit);
        }
    }

    return SOC_E_NONE;
}

int
soc_maverick2_port_reset(int unit)
{
    return soc_trident3_port_reset(unit);
}

STATIC int
_soc_maverick2_port_mapping_init(int unit)
{
    soc_info_t *si;
    int port, phy_port, i;
    int num_port;
    soc_reg_t reg;
    soc_mem_t mem;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];

    /*
     * 96 entries MMU_CHFC_SYSPORT_MAPPINGm.SYS_PORT
     * 256 entries SYS_PORTMAPm.DEVICE_PORT_NUMBER
     */

    si = &SOC_INFO(unit);

    num_port = soc_mem_index_count(unit, ING_DEVICE_PORTm) - 1;
    mem = SOC_MEM_UNIQUE_ACC
            (unit, ING_PHY_TO_IDB_PORT_MAPm)[0];
    sal_memset(&entry, 0,
           sizeof(ing_phy_to_idb_port_map_entry_t));
    /* Invalidate all entries of ING_PHY_TO_IDB_PORT_MAP table first.
    *  Set all entries IDB_PORT  to 0x3f and VALID to 0 */
    soc_mem_field32_set(unit, mem, &entry, IDB_PORTf, 0x3f);
    soc_mem_field32_set(unit, mem, &entry, VALIDf, 0);
    for (i = 0; i < MV2_NUM_PHY_PORT - 2; i++) {
        SOC_IF_ERROR_RETURN
           (soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, &entry));
    }

    /* Ingress physical to device port mapping */
    for (i = 0; i < MV2_NUM_PHY_PORT; i++) {
        phy_port = i + 1;
        port = si->port_p2l_mapping[phy_port];

        sal_memset(&entry, 0,
               sizeof(ing_phy_to_idb_port_map_entry_t));

        /* Skip MGMT port*/  
        if(phy_port == MV2_MGMT_PHY_PORT){
                continue;
        }
        if (port != -1) {
            if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) { 
                continue;
            }
            soc_mem_field32_set(unit, mem, &entry, VALIDf, 1);
            soc_mem_field32_set(unit, mem, &entry, IDB_PORTf, si->port_l2i_mapping[port]);
        } else {
            soc_mem_field32_set(unit, mem, &entry, VALIDf, 0);
            soc_mem_field32_set(unit, mem, &entry, IDB_PORTf, 0x3f);
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, &entry));
    }

    /* Ingress GPP port to device port mapping */
    mem = SYS_PORTMAPm;
    sal_memset(&entry, 0, sizeof(sys_portmap_entry_t));
    for (port = 0; port < num_port; port++) {
        soc_mem_field32_set(unit, mem, &entry, DEVICE_PORT_NUMBERf, port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &entry));
    }

    /* Ingress device port to GPP port mapping
     * PORT_TAB.SRC_SYS_PORT_ID is programmed in the general port config
     * init routine _bcm_fb_port_cfg_init()
     */

    /* Egress device port to physical port mapping */
    rval = 0;
    reg = EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr;
    PBMP_ALL_ITER(unit, port) {
        soc_reg_field_set(unit, reg, &rval, PHYSICAL_PORT_NUMBERf,
                          si->port_l2p_mapping[port]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }

    /* MMU port to physical port mapping */
    rval = 0;
    reg = MMU_PORT_TO_PHY_PORT_MAPPINGr;
    PBMP_ALL_ITER(unit, port) {
        soc_reg_field_set(unit, reg, &rval, PHY_PORTf,
                          si->port_l2p_mapping[port]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }

    /* MMU port to system port mapping */
    rval = 0;
    reg = MMU_PORT_TO_SYSTEM_PORT_MAPPINGr;
    PBMP_ALL_ITER(unit, port) {
        soc_reg_field_set(unit, reg, &rval, SYSTEM_PORTf, port);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_maverick2_tdm_init(int unit)
{
	soc_control_t *soc = SOC_CONTROL(unit);
	soc_info_t *si = &SOC_INFO(unit);
	soc_port_schedule_state_t *port_schedule_state;
	soc_port_map_type_t *in_portmap;
	int port, speed;
	int rv;
	if (soc->tdm_info == NULL) {
		soc->tdm_info = sal_alloc(sizeof(_soc_maverick2_tdm_temp_t),
									"Maverick2 TDM info");
		if (soc->tdm_info == NULL) {
			return SOC_E_MEMORY;
		}
		sal_memset(soc->tdm_info, 0, sizeof(_soc_maverick2_tdm_temp_t));
	}

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(soc_mv2_tdm_scache_recovery(unit));
        return SOC_E_NONE;
    } else {
        SOC_IF_ERROR_RETURN(soc_mv2_tdm_scache_allocate(unit));
    }
#endif

	port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                    "Maverick2 soc_port_schedule_state_t");
	if (port_schedule_state == NULL) {
        return SOC_E_MEMORY;
    }
	sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));
	/* Core clock frequency */
	port_schedule_state->frequency = si->frequency;

	/* Construct in_port_map */
	in_portmap = &port_schedule_state->in_port_map;
	PBMP_PORT_ITER(unit, port) {
		if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
			continue;
		}
		if (IS_HG_PORT(unit, port)) {
            speed = soc_port_speed_higig2eth(si->port_speed_max[port]);
        } else {
            speed = si->port_speed_max[port];
        }
        in_portmap->log_port_speed[port] = speed;
        in_portmap->port_num_lanes[port] = IS_MGMT_PORT(unit, port) ? 1 : si->port_num_lanes[port];
	}
	sal_memcpy(in_portmap->port_p2l_mapping, si->port_p2l_mapping,
                sizeof(int)*MAVERICK2_TDM_PHY_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_l2p_mapping, si->port_l2p_mapping,
                sizeof(int)*_MV2_TDM_DEV_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_p2m_mapping, si->port_p2m_mapping,
                sizeof(int)*MAVERICK2_TDM_PHY_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_m2p_mapping, si->port_m2p_mapping,
                sizeof(int)*_MV2_TDM_MMU_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_l2i_mapping, si->port_l2i_mapping,
                sizeof(int)*_MV2_TDM_DEV_PORTS_PER_DEV);
    sal_memcpy(&in_portmap->physical_pbm, &si->physical_pbm, sizeof(pbmp_t));
    sal_memcpy(&in_portmap->hg2_pbm, &si->hg.bitmap, sizeof(pbmp_t));
    sal_memcpy(&in_portmap->management_pbm, &si->management_pbm,
                sizeof(pbmp_t));
    sal_memcpy(&in_portmap->oversub_pbm, &si->oversub_pbm, sizeof(pbmp_t));

	port_schedule_state->is_flexport = 0;
	rv = soc_mv2_port_schedule_tdm_init(unit, port_schedule_state);

    rv = soc_mv2_tdm_init(unit, port_schedule_state);
	if (rv != SOC_E_NONE) {
        LOG_CLI((BSL_META_U(unit,
            "Unable to configure TDM\n")));
        sal_free(port_schedule_state);
        return rv;
    }
	rv = soc_mv2_soc_tdm_update(unit, port_schedule_state);

    sal_free(port_schedule_state);

    return rv;
}

STATIC int
_soc_maverick2_idb_init(int unit)
{
    soc_info_t *si;
    _soc_maverick2_tdm_temp_t *tdm;
    soc_reg_t reg;
    soc_field_t field;
    int block_info_idx, pipe, clport, obm, lane;
    int port, phy_port_base, lport;
    int lossless, i, count;
    uint32 rval0, rval1, fval;
    uint32 rval_port_cfg = 0, rval_max_usage_cfg = 0;
    uint64 rval64;
    int num_lanes, oversub_ratio_idx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;

    static const struct _obm_setting_s {
        int discard_limit;
        int lossless_xoff;
        int lossy_only_lossy_low_pri[3];
        int port_xoff[3];          /* for oversub ratio 1.5, 1.8, and above */
        int lossy_low_pri[3];      /* for oversub ratio 1.5, 1.8, and above */
        int lossy_discard[3];      /* for oversub ratio 1.5, 1.8, and above */
        int buffer_start[4];
        int buffer_end[4];
    } obm_settings[] = { /* indexed by number of lanes */
        { /* 0 - invalid  */ 0 },
        { /* 1 lane */
            1148, 136, 
            { 777, 777, 756 },
            { 363, 363, 318 }, /* port_xoff */
            { 179, 179, 189 }, /* lossy_low_pri */
            { 546, 546, 577 },  /* lossy_discard */
            { 0,    1152, 2304, 3456}, /* buffer_start */
            { 1151, 2303, 3455, 4607}  /* buffer_end */
        },
        { /* 2 lanes */
            2300, 324, 
            { 1929, 1929, 1908 },
            { 1221, 1221, 1158 }, /* port_xoff */
            { 255, 255, 270 }, /* lossy_low_pri */
            { 622, 622, 658 },  /* lossy_discard */
            { 0,    0, 2304, 2304},  /* buffer_start */
            { 2303, 2303, 4607, 4607}   /* buffer_end */
        },
        { /* 3 - invalid  */ 0 },
        { /* 4 lanes */
            4604, 774, 
            { 4233, 4233, 4212 },
            { 2844, 2844, 2742 }, /* port_xoff */
            { 255, 255, 270 }, /* lossy_low_pri */
            { 546, 546, 577 },  /* lossy_discard */
            { 0,    0,    0,    0   }, /* buffer_start */
            { 4607, 4607, 4607, 4607}  /* buffer_end */
        }
    };

    static const soc_reg_t obm_ctrl_regs[] = {
        IDB_OBM0_CONTROLr, IDB_OBM1_CONTROLr,
        IDB_OBM2_CONTROLr, IDB_OBM3_CONTROLr,
        IDB_OBM4_CONTROLr, IDB_OBM5_CONTROLr,
        IDB_OBM6_CONTROLr, IDB_OBM7_CONTROLr,
        IDB_OBM8_CONTROLr, IDB_OBM9_CONTROLr,
        IDB_OBM10_CONTROLr, IDB_OBM11_CONTROLr,
        IDB_OBM12_CONTROLr, IDB_OBM13_CONTROLr,
        IDB_OBM14_CONTROLr, IDB_OBM15_CONTROLr,
        IDB_OBM16_CONTROLr, IDB_OBM17_CONTROLr,
        IDB_OBM18_CONTROLr, IDB_OBM19_CONTROLr
    };
    static const soc_reg_t obm_ca_ctrl_regs[] = {
        IDB_OBM0_CA_CONTROLr, IDB_OBM1_CA_CONTROLr,
        IDB_OBM2_CA_CONTROLr, IDB_OBM3_CA_CONTROLr,
        IDB_OBM4_CA_CONTROLr, IDB_OBM5_CA_CONTROLr,
        IDB_OBM6_CA_CONTROLr, IDB_OBM7_CA_CONTROLr,
        IDB_OBM8_CA_CONTROLr, IDB_OBM9_CA_CONTROLr,
        IDB_OBM10_CA_CONTROLr, IDB_OBM11_CA_CONTROLr,
        IDB_OBM12_CA_CONTROLr, IDB_OBM13_CA_CONTROLr,
        IDB_OBM14_CA_CONTROLr, IDB_OBM15_CA_CONTROLr,
        IDB_OBM16_CA_CONTROLr, IDB_OBM17_CA_CONTROLr,
        IDB_OBM18_CA_CONTROLr, IDB_OBM19_CA_CONTROLr
    };
    static const soc_reg_t obm_thresh_regs[] = {
        IDB_OBM0_THRESHOLDr, IDB_OBM1_THRESHOLDr,
        IDB_OBM2_THRESHOLDr, IDB_OBM3_THRESHOLDr,
        IDB_OBM4_THRESHOLDr, IDB_OBM5_THRESHOLDr,
        IDB_OBM6_THRESHOLDr, IDB_OBM7_THRESHOLDr,
        IDB_OBM8_THRESHOLDr, IDB_OBM9_THRESHOLDr,
        IDB_OBM10_THRESHOLDr, IDB_OBM11_THRESHOLDr,
        IDB_OBM12_THRESHOLDr, IDB_OBM13_THRESHOLDr,
        IDB_OBM14_THRESHOLDr, IDB_OBM15_THRESHOLDr,
        IDB_OBM16_THRESHOLDr, IDB_OBM17_THRESHOLDr,
        IDB_OBM18_THRESHOLDr, IDB_OBM19_THRESHOLDr
    };
    static const soc_reg_t obm_thresh_1_regs[] = {
        IDB_OBM0_THRESHOLD_1r, IDB_OBM1_THRESHOLD_1r,
        IDB_OBM2_THRESHOLD_1r, IDB_OBM3_THRESHOLD_1r,
        IDB_OBM4_THRESHOLD_1r, IDB_OBM5_THRESHOLD_1r,
        IDB_OBM6_THRESHOLD_1r, IDB_OBM7_THRESHOLD_1r,
        IDB_OBM8_THRESHOLD_1r, IDB_OBM9_THRESHOLD_1r,
        IDB_OBM10_THRESHOLD_1r, IDB_OBM11_THRESHOLD_1r,
        IDB_OBM12_THRESHOLD_1r, IDB_OBM13_THRESHOLD_1r,
        IDB_OBM14_THRESHOLD_1r, IDB_OBM15_THRESHOLD_1r,
        IDB_OBM16_THRESHOLD_1r, IDB_OBM17_THRESHOLD_1r,
        IDB_OBM18_THRESHOLD_1r, IDB_OBM19_THRESHOLD_1r
    };
    static const soc_reg_t obm_fc_thresh_regs[] = {
        IDB_OBM0_FC_THRESHOLDr, IDB_OBM1_FC_THRESHOLDr,
        IDB_OBM2_FC_THRESHOLDr, IDB_OBM3_FC_THRESHOLDr,
        IDB_OBM4_FC_THRESHOLDr, IDB_OBM5_FC_THRESHOLDr,
        IDB_OBM6_FC_THRESHOLDr, IDB_OBM7_FC_THRESHOLDr,
        IDB_OBM8_FC_THRESHOLDr, IDB_OBM9_FC_THRESHOLDr,
        IDB_OBM10_FC_THRESHOLDr, IDB_OBM11_FC_THRESHOLDr,
        IDB_OBM12_FC_THRESHOLDr, IDB_OBM13_FC_THRESHOLDr,
        IDB_OBM14_FC_THRESHOLDr, IDB_OBM15_FC_THRESHOLDr,
        IDB_OBM16_FC_THRESHOLDr, IDB_OBM17_FC_THRESHOLDr,
        IDB_OBM18_FC_THRESHOLDr, IDB_OBM19_FC_THRESHOLDr
    };
    static const soc_reg_t obm_fc_thresh_1_regs[] = {
        IDB_OBM0_FC_THRESHOLD_1r, IDB_OBM1_FC_THRESHOLD_1r,
        IDB_OBM2_FC_THRESHOLD_1r, IDB_OBM3_FC_THRESHOLD_1r,
        IDB_OBM4_FC_THRESHOLD_1r, IDB_OBM5_FC_THRESHOLD_1r,
        IDB_OBM6_FC_THRESHOLD_1r, IDB_OBM7_FC_THRESHOLD_1r,
        IDB_OBM8_FC_THRESHOLD_1r, IDB_OBM9_FC_THRESHOLD_1r,
        IDB_OBM10_FC_THRESHOLD_1r, IDB_OBM11_FC_THRESHOLD_1r,
        IDB_OBM12_FC_THRESHOLD_1r, IDB_OBM13_FC_THRESHOLD_1r,
        IDB_OBM14_FC_THRESHOLD_1r, IDB_OBM15_FC_THRESHOLD_1r,
        IDB_OBM16_FC_THRESHOLD_1r, IDB_OBM17_FC_THRESHOLD_1r,
        IDB_OBM18_FC_THRESHOLD_1r, IDB_OBM19_FC_THRESHOLD_1r
    };
    static const soc_reg_t obm_shared_regs[] = {
        IDB_OBM0_SHARED_CONFIGr, IDB_OBM1_SHARED_CONFIGr,
        IDB_OBM2_SHARED_CONFIGr, IDB_OBM3_SHARED_CONFIGr,
        IDB_OBM4_SHARED_CONFIGr, IDB_OBM5_SHARED_CONFIGr,
        IDB_OBM6_SHARED_CONFIGr, IDB_OBM7_SHARED_CONFIGr,
        IDB_OBM8_SHARED_CONFIGr, IDB_OBM9_SHARED_CONFIGr,
        IDB_OBM10_SHARED_CONFIGr, IDB_OBM11_SHARED_CONFIGr,
        IDB_OBM12_SHARED_CONFIGr, IDB_OBM13_SHARED_CONFIGr,
        IDB_OBM14_SHARED_CONFIGr, IDB_OBM15_SHARED_CONFIGr,
        IDB_OBM16_SHARED_CONFIGr, IDB_OBM17_SHARED_CONFIGr,
        IDB_OBM18_SHARED_CONFIGr, IDB_OBM19_SHARED_CONFIGr
    };
    static const soc_reg_t obm_max_usage_regs[] = {
        IDB_OBM0_MAX_USAGE_SELECTr, IDB_OBM1_MAX_USAGE_SELECTr,
        IDB_OBM2_MAX_USAGE_SELECTr, IDB_OBM3_MAX_USAGE_SELECTr,
        IDB_OBM4_MAX_USAGE_SELECTr, IDB_OBM5_MAX_USAGE_SELECTr,
        IDB_OBM6_MAX_USAGE_SELECTr, IDB_OBM7_MAX_USAGE_SELECTr,
        IDB_OBM8_MAX_USAGE_SELECTr, IDB_OBM9_MAX_USAGE_SELECTr,
        IDB_OBM10_MAX_USAGE_SELECTr, IDB_OBM11_MAX_USAGE_SELECTr,
        IDB_OBM12_MAX_USAGE_SELECTr, IDB_OBM13_MAX_USAGE_SELECTr,
        IDB_OBM14_MAX_USAGE_SELECTr, IDB_OBM15_MAX_USAGE_SELECTr,
        IDB_OBM16_MAX_USAGE_SELECTr, IDB_OBM17_MAX_USAGE_SELECTr,
        IDB_OBM18_MAX_USAGE_SELECTr, IDB_OBM19_MAX_USAGE_SELECTr
    };
    static const soc_field_t obm_bypass_fields[] = {
        PORT0_BYPASS_ENABLEf, PORT1_BYPASS_ENABLEf,
        PORT2_BYPASS_ENABLEf, PORT3_BYPASS_ENABLEf
    };
    static const soc_field_t obm_oversub_fields[] = {
        PORT0_OVERSUB_ENABLEf, PORT1_OVERSUB_ENABLEf,
        PORT2_OVERSUB_ENABLEf, PORT3_OVERSUB_ENABLEf
    };
    static const soc_field_t obm_ca_sop_fields[] = {
        PORT0_CA_SOPf, PORT1_CA_SOPf, PORT2_CA_SOPf, PORT3_CA_SOPf
    };
    static const soc_reg_t obm_port_config_regs[] = {
        IDB_OBM0_PORT_CONFIGr, IDB_OBM1_PORT_CONFIGr,
        IDB_OBM2_PORT_CONFIGr, IDB_OBM3_PORT_CONFIGr,
        IDB_OBM4_PORT_CONFIGr, IDB_OBM5_PORT_CONFIGr,
        IDB_OBM6_PORT_CONFIGr, IDB_OBM7_PORT_CONFIGr,
        IDB_OBM8_PORT_CONFIGr, IDB_OBM9_PORT_CONFIGr,
        IDB_OBM10_PORT_CONFIGr, IDB_OBM11_PORT_CONFIGr,
        IDB_OBM12_PORT_CONFIGr, IDB_OBM13_PORT_CONFIGr,
        IDB_OBM14_PORT_CONFIGr, IDB_OBM15_PORT_CONFIGr,
        IDB_OBM16_PORT_CONFIGr, IDB_OBM17_PORT_CONFIGr,
        IDB_OBM18_PORT_CONFIGr, IDB_OBM19_PORT_CONFIGr
    };
    static const soc_reg_t obm_fc_config_regs[] = {
        IDB_OBM0_FLOW_CONTROL_CONFIGr, IDB_OBM1_FLOW_CONTROL_CONFIGr,
        IDB_OBM2_FLOW_CONTROL_CONFIGr, IDB_OBM3_FLOW_CONTROL_CONFIGr,
        IDB_OBM4_FLOW_CONTROL_CONFIGr, IDB_OBM5_FLOW_CONTROL_CONFIGr,
        IDB_OBM6_FLOW_CONTROL_CONFIGr, IDB_OBM7_FLOW_CONTROL_CONFIGr,
        IDB_OBM8_FLOW_CONTROL_CONFIGr, IDB_OBM9_FLOW_CONTROL_CONFIGr,
        IDB_OBM10_FLOW_CONTROL_CONFIGr, IDB_OBM11_FLOW_CONTROL_CONFIGr,
        IDB_OBM12_FLOW_CONTROL_CONFIGr, IDB_OBM13_FLOW_CONTROL_CONFIGr,
        IDB_OBM14_FLOW_CONTROL_CONFIGr, IDB_OBM15_FLOW_CONTROL_CONFIGr,
        IDB_OBM16_FLOW_CONTROL_CONFIGr, IDB_OBM17_FLOW_CONTROL_CONFIGr,
        IDB_OBM18_FLOW_CONTROL_CONFIGr, IDB_OBM19_FLOW_CONTROL_CONFIGr
    };
    static const soc_reg_t obm_buffer_config_regs[] = {
        IDB_OBM0_BUFFER_CONFIGr, IDB_OBM1_BUFFER_CONFIGr,
        IDB_OBM2_BUFFER_CONFIGr, IDB_OBM3_BUFFER_CONFIGr,
        IDB_OBM4_BUFFER_CONFIGr, IDB_OBM5_BUFFER_CONFIGr,
        IDB_OBM6_BUFFER_CONFIGr, IDB_OBM7_BUFFER_CONFIGr,
        IDB_OBM8_BUFFER_CONFIGr, IDB_OBM9_BUFFER_CONFIGr,
        IDB_OBM10_BUFFER_CONFIGr, IDB_OBM11_BUFFER_CONFIGr,
        IDB_OBM12_BUFFER_CONFIGr, IDB_OBM13_BUFFER_CONFIGr,
        IDB_OBM14_BUFFER_CONFIGr, IDB_OBM15_BUFFER_CONFIGr,
        IDB_OBM16_BUFFER_CONFIGr, IDB_OBM17_BUFFER_CONFIGr,
        IDB_OBM18_BUFFER_CONFIGr, IDB_OBM19_BUFFER_CONFIGr
    };
    static const soc_mem_t obm_pri_map_mem[][4] = {
        {IDB_OBM0_PRI_MAP_PORT0m, IDB_OBM0_PRI_MAP_PORT1m,
         IDB_OBM0_PRI_MAP_PORT2m, IDB_OBM0_PRI_MAP_PORT3m
        },
        {IDB_OBM1_PRI_MAP_PORT0m, IDB_OBM1_PRI_MAP_PORT1m,
         IDB_OBM1_PRI_MAP_PORT2m, IDB_OBM1_PRI_MAP_PORT3m
        },
        {IDB_OBM2_PRI_MAP_PORT0m, IDB_OBM2_PRI_MAP_PORT1m,
         IDB_OBM2_PRI_MAP_PORT2m, IDB_OBM2_PRI_MAP_PORT3m
        },
        {IDB_OBM3_PRI_MAP_PORT0m, IDB_OBM3_PRI_MAP_PORT1m,
         IDB_OBM3_PRI_MAP_PORT2m, IDB_OBM3_PRI_MAP_PORT3m
        },
        {IDB_OBM4_PRI_MAP_PORT0m, IDB_OBM4_PRI_MAP_PORT1m,
         IDB_OBM4_PRI_MAP_PORT2m, IDB_OBM4_PRI_MAP_PORT3m
        },
        {IDB_OBM5_PRI_MAP_PORT0m, IDB_OBM5_PRI_MAP_PORT1m,
         IDB_OBM5_PRI_MAP_PORT2m, IDB_OBM5_PRI_MAP_PORT3m
        },
        {IDB_OBM6_PRI_MAP_PORT0m, IDB_OBM6_PRI_MAP_PORT1m,
         IDB_OBM6_PRI_MAP_PORT2m, IDB_OBM6_PRI_MAP_PORT3m
        },
        {IDB_OBM7_PRI_MAP_PORT0m, IDB_OBM7_PRI_MAP_PORT1m,
         IDB_OBM7_PRI_MAP_PORT2m, IDB_OBM7_PRI_MAP_PORT3m
        },
        {IDB_OBM8_PRI_MAP_PORT0m, IDB_OBM8_PRI_MAP_PORT1m,
         IDB_OBM8_PRI_MAP_PORT2m, IDB_OBM8_PRI_MAP_PORT3m
        },
        {IDB_OBM9_PRI_MAP_PORT0m, IDB_OBM9_PRI_MAP_PORT1m,
         IDB_OBM9_PRI_MAP_PORT2m, IDB_OBM9_PRI_MAP_PORT3m
        },
        {IDB_OBM10_PRI_MAP_PORT0m, IDB_OBM10_PRI_MAP_PORT1m,
         IDB_OBM10_PRI_MAP_PORT2m, IDB_OBM10_PRI_MAP_PORT3m
        },
        {IDB_OBM11_PRI_MAP_PORT0m, IDB_OBM11_PRI_MAP_PORT1m,
         IDB_OBM11_PRI_MAP_PORT2m, IDB_OBM11_PRI_MAP_PORT3m
        },
        {IDB_OBM12_PRI_MAP_PORT0m, IDB_OBM12_PRI_MAP_PORT1m,
         IDB_OBM12_PRI_MAP_PORT2m, IDB_OBM12_PRI_MAP_PORT3m
        },
        {IDB_OBM13_PRI_MAP_PORT0m, IDB_OBM13_PRI_MAP_PORT1m,
         IDB_OBM13_PRI_MAP_PORT2m, IDB_OBM13_PRI_MAP_PORT3m
        },
        {IDB_OBM14_PRI_MAP_PORT0m, IDB_OBM14_PRI_MAP_PORT1m,
         IDB_OBM14_PRI_MAP_PORT2m, IDB_OBM14_PRI_MAP_PORT3m
        },
        {IDB_OBM15_PRI_MAP_PORT0m, IDB_OBM15_PRI_MAP_PORT1m,
         IDB_OBM15_PRI_MAP_PORT2m, IDB_OBM15_PRI_MAP_PORT3m
        },
        {IDB_OBM16_PRI_MAP_PORT0m, IDB_OBM16_PRI_MAP_PORT1m,
         IDB_OBM16_PRI_MAP_PORT2m, IDB_OBM16_PRI_MAP_PORT3m
        },
        {IDB_OBM17_PRI_MAP_PORT0m, IDB_OBM17_PRI_MAP_PORT1m,
         IDB_OBM17_PRI_MAP_PORT2m, IDB_OBM17_PRI_MAP_PORT3m
        },
        {IDB_OBM18_PRI_MAP_PORT0m, IDB_OBM18_PRI_MAP_PORT1m,
         IDB_OBM18_PRI_MAP_PORT2m, IDB_OBM18_PRI_MAP_PORT3m
        },
        {IDB_OBM19_PRI_MAP_PORT0m, IDB_OBM19_PRI_MAP_PORT1m,
         IDB_OBM19_PRI_MAP_PORT2m, IDB_OBM19_PRI_MAP_PORT3m
        },
    };

    static const soc_field_t obm_ob_pri_fields[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf, OFFSET8_OB_PRIORITYf,
        OFFSET9_OB_PRIORITYf, OFFSET10_OB_PRIORITYf, OFFSET11_OB_PRIORITYf,
        OFFSET12_OB_PRIORITYf, OFFSET13_OB_PRIORITYf, OFFSET14_OB_PRIORITYf,
        OFFSET15_OB_PRIORITYf
    };
    static const int hw_mode_values[SOC_MV2_PORT_RATIO_COUNT] = {
        0, 1, 1, 1, 3, 5, 4, 6, 2
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;
    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    rval_port_cfg = 0;
    reg = SOC_REG_UNIQUE_ACC(unit, obm_port_config_regs[0])[0];
    soc_reg_field_set(unit, reg, &rval_port_cfg, PORT_PRIf, 2);

    rval_max_usage_cfg = 0;
    reg = SOC_REG_UNIQUE_ACC(unit, obm_max_usage_regs[0])[0];
    soc_reg_field_set(unit, reg, &rval_max_usage_cfg, PRIORITY_SELECTf, 2);

    sal_memset(entry, 0, sizeof(idb_obm0_pri_map_port0_entry_t));
    mem = SOC_MEM_UNIQUE_ACC(unit, obm_pri_map_mem[0][0])[0];

    count = COUNTOF(obm_ob_pri_fields);
    for (i = 0; i < count; i++) {
        soc_mem_field32_set(unit, mem, entry, obm_ob_pri_fields[i], 2);
    }

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        phy_port_base = PORT_BLOCK_BASE_PORT(port);
        pipe = si->port_pipe[port];
        /* MV2: OBM number is a function of phyport or TSC no. */

        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);
        if (clport > 19) {
           continue;
        } else {
           obm = clport;
        }

        /* Set cell assembly mode then toggle reset to send initial credit
         * to EP */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_ca_ctrl_regs[obm])[pipe];
        rval0 = 0;
        soc_reg_field_set(unit, reg, &rval0, PORT_MODEf,
                          hw_mode_values[tdm->port_ratio[obm]]);

        rval1 = rval0;
        /* PORT_RESETf is a 4 bit field, set to all one's in the next step */
        soc_reg_field_set(unit, reg, &rval0, PORT0_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT1_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT2_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT3_RESETf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval1));

        if (!SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
            continue;
        }

        /* Enable oversub */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_ctrl_regs[obm])[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval0));
        for (lane = 0; lane < _MV2_PORTS_PER_PBLK; lane++) {
            if (si->port_p2l_mapping[phy_port_base + lane] == -1) {
                continue;
            }
            field = obm_bypass_fields[lane];
            soc_reg_field_set(unit, reg, &rval0, field, 1);
            field = obm_oversub_fields[lane];
            soc_reg_field_set(unit, reg, &rval0, field, 1);
            lport = si->port_p2l_mapping[phy_port_base + lane];
            /* Update CA_SOP settings */
            if (si->port_speed_max[lport] < 100000) {
                field = obm_ca_sop_fields[lane];
                soc_reg_field_set(unit, reg, &rval0, field, 0);
            }
        }

        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        if (!SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
            continue;
        }

        /* Configure buffer */
        for (lane = 0; lane < _MV2_PORTS_PER_PBLK; lane++) {
            port = si->port_p2l_mapping[phy_port_base + lane];
            if (port == -1) {
                continue;
            }
            num_lanes = si->port_num_lanes[port];
            reg = SOC_REG_UNIQUE_ACC(unit, obm_buffer_config_regs[obm])[pipe];
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, lane, &rval0));
            fval = obm_settings[num_lanes].buffer_start[lane];
            soc_reg_field_set(unit, reg, &rval0, BUFFER_STARTf, fval);
            fval = obm_settings[num_lanes].buffer_end[lane];
            soc_reg_field_set(unit, reg, &rval0, BUFFER_ENDf, fval);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, lane, rval0));
        }

        /* Configure shared config */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_shared_regs[obm])[pipe];
        SOC_IF_ERROR_RETURN
            (soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, SOP_DISCARD_MODEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, REG_PORT_ANY, 0, rval64));
        if ((tdm->ovs_ratio_x1000[pipe][0] + tdm->ovs_ratio_x1000[pipe][1]) > 1800) { /* ratio > 1.8 */
            oversub_ratio_idx = 2;
        } else if ((tdm->ovs_ratio_x1000[pipe][0] + tdm->ovs_ratio_x1000[pipe][1]) <= 1800) { /* ratio <= 1.8 */
            oversub_ratio_idx = 1;
        } else {
            oversub_ratio_idx = 0;
        }

        if (lossless) {
            for (lane = 0; lane < _MV2_PORTS_PER_PBLK; lane++) {
                port = si->port_p2l_mapping[phy_port_base + lane];
                if (port == -1) {
                    continue;
                }
                num_lanes = si->port_num_lanes[port];

                /* Configure threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                                      4604);
                fval = obm_settings[num_lanes].lossy_low_pri[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_LOW_PRIf, fval);
                fval = obm_settings[num_lanes].lossy_discard[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_DISCARDf, fval);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_1_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[num_lanes].discard_limit;
                soc_reg64_field32_set(unit, reg, &rval64, DISCARD_LIMITf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_DISCARDf,
                                      4604);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[num_lanes].lossless_xoff;
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                                      fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XONf,
                                      fval - 30);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XOFFf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XONf,
                                      fval - 30);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control threshold_1 */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_1_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[num_lanes].port_xoff[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XOFFf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XONf, fval - 30);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control config */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_config_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, PORT_FC_ENf, 1);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_FC_ENf, 1);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_FC_ENf, 0);
                soc_reg64_field32_set(unit, reg, &rval64,
                                      LOSSLESS0_PRIORITY_PROFILEf, 0xffff);
                soc_reg64_field32_set(unit, reg, &rval64,
                                      LOSSLESS1_PRIORITY_PROFILEf, 0);
                soc_reg64_field32_set(unit, reg, &rval64,
                                      XOFF_REFRESH_TIMERf, 0x100);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* OBM Port config */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_port_config_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, lane,
                                   rval_port_cfg));
                mem = SOC_MEM_UNIQUE_ACC
                        (unit, obm_pri_map_mem[obm][lane])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));
            }
            reg = SOC_REG_UNIQUE_ACC(unit, obm_max_usage_regs[obm])[pipe];
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval_max_usage_cfg));
        } else { /* lossy */
            for (lane = 0; lane < _MV2_PORTS_PER_PBLK; lane++) {
                port = si->port_p2l_mapping[phy_port_base + lane];
                if (port == -1) {
                    continue;
                }
                num_lanes = si->port_num_lanes[port];

                /* Configure threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[num_lanes].discard_limit;
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_DISCARDf,
                                      fval - 4);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                                      4604);
                fval = obm_settings[num_lanes].lossy_only_lossy_low_pri[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_LOW_PRIf, fval);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure threshold_1 */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_1_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[num_lanes].discard_limit;
                soc_reg64_field32_set(unit, reg, &rval64, DISCARD_LIMITf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_DISCARDf,
                                      4604);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_1_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XOFFf, 4604);
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XONf, 4604);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                                      4604);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XONf, 4604);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XONf, 4604);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XOFFf,
                                      4604);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control config */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_config_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, PORT_FC_ENf, 0);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_FC_ENf, 0);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_FC_ENf, 0);
                soc_reg64_field32_set(unit, reg, &rval64,
                                      LOSSLESS0_PRIORITY_PROFILEf, 0);
                soc_reg64_field32_set(unit, reg, &rval64,
                                      LOSSLESS1_PRIORITY_PROFILEf, 0);
                soc_reg64_field32_set(unit, reg, &rval64,
                                      XOFF_REFRESH_TIMERf, 0x100);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

            }
        }
    }

    /* Toggle cpu port cell assembly reset to send initial credit to EP */
    reg = IDB_CA_CPU_CONTROLr;
    rval0 = 0;
    soc_reg_field_set(unit, reg, &rval0, PORT_RESETf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
    sal_msleep(5);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));

    /* Toggle loopback cell assembly reset to send initial credit to EP */
    reg = IDB_CA_LPBK_CONTROLr;
    rval0 = 0;
    soc_reg_field_set(unit, reg, &rval0, PORT_RESETf, 1);
    PBMP_LB_ITER(unit, port) {
        reg = SOC_REG_UNIQUE_ACC(unit, IDB_CA_LPBK_CONTROLr)
                                            [si->port_pipe[port]] ;
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));
    }

    soc_td3_set_idb_dpp_ctrl(unit);

    return SOC_E_NONE;
}

extern int
(*_phy_tsce_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);
extern int
(*_phy_tscf_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);

#define TSC_REG_ADDR_TSCID_SET(_phy_reg, _phyad)    \
                            ((_phy_reg) |= ((_phyad) & 0x1f) << 19)

STATIC int
_soc_maverick2_mdio_addr_to_port(uint32 phy_addr)
{
    int bus, offset;

    /* Must be internal MDIO address */
    if ((phy_addr & 0x80) == 0) {
        return 0;
    }

    /*
     * Internal phy address:
     * bus 0 phy 1 to 20 are mapped to Physical port 1 to 20
     * bus 1 phy 21 to 40 are mapped to Physical port 21 to 40
     * bus 2 phy 41 to 60 are mapped to Physical port 41 to 60
     * bus 3 phy 1 to 24 are mapped to Physical port 85 to 88
     * bus 4 phy 1 to 16 are mapped to Physical port 89 to 104
     * bus 5 phy 1 to 24 are mapped to Physical port 105 to 128
     * bus 6 phy 1 is mapped to Physical port 129 and
     *       phy 3 is mapped to Physical port 131
     */
    bus = PHY_ID_BUS_NUM(phy_addr);
    if (bus > 6) {
        return 0;
    }
    switch (bus) {
    case 0: offset = 0;
        break;
    case 1: offset = 20;
        break;
    case 2: offset = 40;
        break;
    case 3: offset = 60;
        break;
    case 6: offset = 80;
        break;
    default:
        return 0;
    }

    return (phy_addr & 0x1f) + offset;
}

STATIC int
_soc_maverick2_tscx_reg_read(int unit, uint32 phy_addr,
                            uint32 phy_reg, uint32 *phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_maverick2_mdio_addr_to_port(phy_addr);

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_maverick2_tscx_reg_read[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
    rv = soc_sbus_tsc_reg_read(unit, port, blk, phy_addr,
                               phy_reg, phy_data);
    return rv;
}

STATIC int
_soc_maverick2_tscx_reg_write(int unit, uint32 phy_addr,
                             uint32 phy_reg, uint32 phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_maverick2_mdio_addr_to_port(phy_addr);

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_maverick2_tscx_reg_write[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
    rv = soc_sbus_tsc_reg_write(unit, port, blk, phy_addr,
                                phy_reg, phy_data);
    return rv;
}

#ifdef BCM_CMICX_SUPPORT
STATIC int
_soc_mv2_ledup_init(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int freq;
    uint32 rval, clk_half_period, refresh_period;

    /* freq in KHz */
    freq = (si->frequency) * 1000;

    /* For LED refresh period = 33 ms (about 30Hz)  */

    /* refresh period
     * = (required refresh period in sec)*(switch clock frequency in Hz)
     */
    refresh_period = (freq * 33);

    rval = 0;
    soc_reg_field_set(unit, U0_LED_REFRESH_CTRLr, &rval,
                      REFRESH_CYCLE_PERIODf, refresh_period);

    SOC_IF_ERROR_RETURN(WRITE_U0_LED_REFRESH_CTRLr(unit, rval));

    /* For LED clock period */
    /* LEDCLK_HALF_PERIOD
     *  = [(required LED clock period in sec)/2]*(M0SS clock frequency in Hz)
     *
     *  Where M0SS freqency is 858MHz and
     *  Typical LED clock period is 200ns(5MHz) = 2*10^-7
     */

    freq = 858 * 1000000;
    clk_half_period = (freq + 2500000) / 5000000;
    clk_half_period = clk_half_period / 2;

    rval = 0;
    soc_reg_field_set(unit, U0_LED_CLK_DIV_CTRLr, &rval,
                      LEDCLK_HALF_PERIODf, clk_half_period);

    SOC_IF_ERROR_RETURN(WRITE_U0_LED_CLK_DIV_CTRLr(unit, rval));


    SOC_IF_ERROR_RETURN(READ_U0_LED_ACCU_CTRLr(unit, &rval));
    soc_reg_field_set(unit, U0_LED_ACCU_CTRLr, &rval,
                      LAST_PORTf, 76);
    SOC_IF_ERROR_RETURN(WRITE_U0_LED_ACCU_CTRLr(unit, rval));

    /* To initialize M0s for LED or Firmware Link Scan*/
    if (soc_feature(unit, soc_feature_cmicx) && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
        SOC_IF_ERROR_RETURN(soc_iproc_m0_init(unit));
    }

    return SOC_E_NONE;
}
#endif

STATIC int
_soc_maverick2_misc_init(int unit)
{
    soc_control_t *soc;
    soc_info_t *si;
    soc_mem_t mem;
    uint32 rval;
#if !defined(BCM_MV2_ASF_EXCLUDE)
    uint32 fval;
#endif
    uint64 rval64;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_pbmp_t pbmp;
    int port, phy_port;
    uint32 parity_enable;
    modport_map_subport_entry_t map_entry;
    ing_dest_port_enable_entry_t ingr_entry;
    uint32 mc_ctrl = 0;
    int fabric_port_enable = 1;
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    int num_overlay_ecmp_rh_flowset_entries;
    int ecmp_levels = 1;
#endif
    int num_lag_rh_flowset_entries;
    soc_field_t fields[2];
    uint32 values[2];

    soc = SOC_CONTROL(unit);
    si = &SOC_INFO(unit);

    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));
    SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_stop(unit));

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify
        (unit, AUX_ARB_CONTROLr, REG_PORT_ANY, FP_REFRESH_ENABLEf, 1));

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify
        (unit, AUX_ARB_CONTROLr, REG_PORT_ANY, FP_REFRESH_ENABLEf, parity_enable));

    if (parity_enable) {
        if (!SOC_WARM_BOOT(unit)) {
            /* Certain mems/regs need to be cleared before enabling SER */
            SOC_IF_ERROR_RETURN(soc_trident3_clear_mmu_memory(unit));
        }
        soc_ser_log_init(unit, NULL, 0);

        SOC_IF_ERROR_RETURN(soc_mv2_ser_enable_all(unit, TRUE));
        soc_mv2_ser_register(unit);
        soc_mv2_ser_test_register(unit);
    }

    SOC_IF_ERROR_RETURN(_soc_trident3_init_mmu_memory(unit));
    SOC_IF_ERROR_RETURN(READ_MMU_GCFG_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, PARITY_ENf, parity_enable ? 1 : 0);
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, REFRESH_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));

    /* SDK-174533 Certain memories do have 1 bit error so the below function enables
       parity and disable 1 bit error correction reporting by default for these
       memories in order for the customers to determine if the part is bad or not.
    */
    SOC_IF_ERROR_RETURN(soc_trident3_ser_enable_special_table(unit));
    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(
            _soc_cancun_load_status_clear(unit, CANCUN_SOC_FILE_TYPE_CIH));
        /* Dummy call to clear garbage h/w values */
        SOC_IF_ERROR_RETURN
            (soc_trident3_temperature_monitor_get(unit,
                                                  10,
                                                  NULL,
                                                  NULL));
    }

    SOC_IF_ERROR_RETURN(soc_trident3_uft_uat_config(unit));

    soc_cancun_init(unit);
    SOC_IF_ERROR_RETURN(soc_trident3_latency_config(unit));
    SOC_IF_ERROR_RETURN(_soc_maverick2_port_mapping_init(unit));

    /*
     * Parse config at init time and cache property value for use at
     * run time
     */
    if (SOC_PBMP_NOT_NULL(PBMP_HG_ALL(unit))) {
        fabric_port_enable = 1;
    } else {
        fabric_port_enable = soc_property_get(unit, spn_FABRIC_PORT_ENABLE, 1);
    }
    SOC_CONTROL_LOCK(unit);
    si->fabric_port_enable = fabric_port_enable;
    SOC_CONTROL_UNLOCK(unit);


    if (!SAL_BOOT_XGSSIM) {
        if (soc->tdm_info == NULL) {
            soc->tdm_info = sal_alloc(sizeof(_soc_maverick2_tdm_temp_t),
                                      "Maverick2 TDM info");
            if (soc->tdm_info == NULL) {
                return SOC_E_MEMORY;
            }
        }
        _soc_maverick2_tdm_init(unit);
        _soc_maverick2_idb_init(unit);
    }

    sal_memset(entry, 0, sizeof(cpu_pbm_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBMm, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBMm, MEM_BLOCK_ALL, 0, entry));

    sal_memset(entry, 0, sizeof(cpu_pbm_2_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBM_2m, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBM_2m, MEM_BLOCK_ALL, 0, entry));

#if __GNUC__ > 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmemset-elt-size"
#endif
    /*
     * We are NOT doing a sal_memset(entry,0,sizeof(entry)) for
     * performance reasons, because the actual soc_mem_field_set()
     * will only write sizeof(device_loopback_ports_bitmap_entry_t)=17
     * bytes while sizeof(entry)=SOC_MAX_MEM_BYTES can be up to 651
     * depending on which devices are compiled in.
     */
    sal_memset(entry, 0, sizeof(device_loopback_ports_bitmap_entry_t));
#if __GNUC__ > 6
#pragma GCC diagnostic pop
#endif
    soc_mem_pbmp_field_set(unit, DEVICE_LOOPBACK_PORTS_BITMAPm, entry, BITMAPf,
                           &PBMP_LB(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, DEVICE_LOOPBACK_PORTS_BITMAPm, MEM_BLOCK_ALL, 0,
                       entry));

    PBMP_LB_ITER(unit, port) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MULTIPASS_LOOPBACK_BITMAPm)
            [si->port_pipe[port]];
        sal_memset(entry, 0, sizeof(multipass_loopback_bitmap_entry_t));
        SOC_PBMP_PORT_SET(pbmp, port);
        soc_mem_pbmp_field_set(unit, mem, &entry, BITMAPf, &pbmp);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));
    }

    sal_memset(entry, 0, sizeof(egr_ing_port_entry_t));
    soc_mem_field32_set(unit, EGR_ING_PORTm, entry, PORT_TYPEf, 1);
    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, port, entry));
    }
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, si->cpu_hg_index,
                       entry));
    soc_mem_field32_set(unit, EGR_ING_PORTm, entry, PORT_TYPEf, 2);
    PBMP_LB_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, port, entry));
    }

    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROLr(unit,&rval));
    if (SOC_PBMP_NOT_NULL(si->management_pbm)) {
        soc_reg_field_set(unit, TOP_MISC_CONTROLr, &rval, ENABLE_MGMT_PORTf, 1);
    } else {
        soc_reg_field_set(unit, TOP_MISC_CONTROLr, &rval, ENABLE_MGMT_PORTf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROLr(unit, rval));

    /* Enable dual hash tables */
    SOC_IF_ERROR_RETURN(soc_trident3_hash_init(unit));

#if !defined(BCM_MV2_ASF_EXCLUDE)
    /* Configure EP credit */
    sal_memset(entry, 0, sizeof(egr_mmu_cell_credit_entry_t));
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(
            soc_mv2_port_asf_speed_to_mmu_cell_credit(unit, port,
                si->port_speed_max[port], &fval));
        SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ANY,
                            si->port_l2p_mapping[port], &entry));
        soc_EGR_MMU_CELL_CREDITm_field32_set(unit, &entry, CREDITf, fval);
        SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ALL,
                            si->port_l2p_mapping[port], &entry));
    }

    /* Configure egress transmit start count */
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(
            soc_mv2_port_asf_xmit_start_count_set(unit, port,
                si->port_speed_max[port], _SOC_MV2_ASF_MODE_SAF, 0));
    }
#endif

    /* Enable egress */
    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        phy_port = si->port_l2p_mapping[port];
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
    }

    sal_memset(entry, 0, sizeof(epc_link_bmap_entry_t));
    soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                           &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0, entry));

    /* enable all ports */
    sal_memset(&ingr_entry, 0, sizeof(ingr_entry));
    soc_mem_pbmp_field_set(unit, ING_DEST_PORT_ENABLEm, &ingr_entry,
                           PORT_BITMAPf, &PBMP_ALL(unit));
    SOC_IF_ERROR_RETURN(
        WRITE_ING_DEST_PORT_ENABLEm(unit, MEM_BLOCK_ALL, 0, &ingr_entry));

    sal_memset(&map_entry, 0, sizeof(map_entry));
    soc_mem_field32_set(unit, MODPORT_MAP_SUBPORTm, &map_entry, ENABLEf, 1);

    /* my_modid and other modid related initialization */
    PBMP_ALL_ITER(unit, port) {
        /* configure logical port numbers */
        soc_mem_field32_set(unit, MODPORT_MAP_SUBPORTm, &map_entry,
                            DESTf, port);
        SOC_IF_ERROR_RETURN(WRITE_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ALL,
                                                        port, &map_entry));
    }

    /* setting up my_modid */
    SOC_IF_ERROR_RETURN(READ_MY_MODID_SET_2_64r(unit, &rval64));

    soc_reg64_field32_set(unit, MY_MODID_SET_2_64r, &rval64,
                          MODID_0_VALIDf, 1);
    soc_reg64_field32_set(unit, MY_MODID_SET_2_64r, &rval64,
                          MODID_0f, SOC_BASE_MODID(unit));

    SOC_IF_ERROR_RETURN(WRITE_MY_MODID_SET_2_64r(unit, rval64));

    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          L3SRC_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          L2DST_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          APPLY_EGR_MASK_ON_L2f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          APPLY_EGR_MASK_ON_L3f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          ARP_RARP_TO_FPf, 0x3); /* enable both ARP & RARP */
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          ARP_VALIDATION_ENf, 1);
    if (soc_feature(unit, soc_feature_port_lag_failover)) {
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                              IGNORE_HG_HDR_LAG_FAILOVERf, 0);
    } else {
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                              IGNORE_HG_HDR_LAG_FAILOVERf, 1);
    }
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          APPLY_HG_MGID_ADJUSTf, 0xFF);

    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, rval64));

    SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    SOC_PBMP_REMOVE(pbmp, PBMP_LB(unit));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_EN_EFILTER_BITMAPm,
                                     MEM_BLOCK_ANY, 0, &entry));
    soc_mem_pbmp_field_set(unit, ING_EN_EFILTER_BITMAPm, &entry, BITMAPf,
                           &pbmp);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_EN_EFILTER_BITMAPm,
                                      MEM_BLOCK_ANY, 0, &entry));

    /* Multicast range initialization */
    SOC_IF_ERROR_RETURN
        (soc_hbx_higig2_mcast_sizes_set(unit,
             soc_property_get(unit, spn_HIGIG2_MULTICAST_VLAN_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L2_RANGE,
                              soc_mem_index_count(unit, L2MCm)),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                              soc_mem_index_count(unit, L3_IPMCm))));

    soc->mcast_size = soc_property_get(unit, spn_MULTICAST_L2_RANGE,
                                        soc_mem_index_count(unit, L2MCm));
    soc->ipmc_size =  soc_property_get(unit, spn_MULTICAST_L3_RANGE,
                                        soc_mem_index_count(unit, L3_IPMCm));

    SOC_IF_ERROR_RETURN(READ_MC_CONTROL_4r(unit, &mc_ctrl));

    soc_reg_field_set(unit, MC_CONTROL_4r, &mc_ctrl,
                      ALLOW_IPMC_INDEX_WRAP_AROUNDf, 1);
    SOC_IF_ERROR_RETURN
        (WRITE_MC_CONTROL_4r(unit, mc_ctrl));

    /* Setup MDIO divider */
    soc_trident3_mdio_rate_divisor_set(unit);

    _phy_tsce_firmware_set_helper[unit] = soc_trident3_tscx_firmware_set;
    _phy_tscf_firmware_set_helper[unit] = soc_trident3_tscx_firmware_set;

    /* Check if Hierarchical ECMP mode is set */
    if (soc_property_get(unit, spn_L3_ECMP_LEVELS, 1) == 2) {
        uint32 rval = 0;
        uint32 ecmp_mode = 1;
        rval = 0;
        soc_reg_field_set(unit, ECMP_CONFIGr, &rval, ECMP_MODEf, ecmp_mode);
        SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIGr(unit, rval));

    } else {
        uint32 rval = 0;
        uint32 ecmp_mode = 0;
        soc_reg_field_set(unit, ECMP_CONFIGr, &rval, ECMP_MODEf, ecmp_mode);
        SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIGr(unit, rval));

    }

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROLr,
                        REG_PORT_ANY, ECMP_FLOWSET_TABLE_CONFIGf, 1));
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
        ecmp_levels = soc_property_get(unit, spn_L3_ECMP_LEVELS, 1);
        if ((soc_feature(unit, soc_feature_riot) ||
            soc_feature(unit, soc_feature_multi_level_ecmp)) &&
            ecmp_levels > 1 && soc_mem_index_count(unit,RH_ECMP_FLOWSETm)) {

            num_overlay_ecmp_rh_flowset_entries = soc_property_get(unit,
                   spn_RIOT_OVERLAY_ECMP_RESILIENT_HASH_SIZE, 0);

            if (SOC_FIELD_RANGE_CHECK(
                 num_overlay_ecmp_rh_flowset_entries, 0, 32768)) {
                 num_overlay_ecmp_rh_flowset_entries = 32768;
            }

            switch (num_overlay_ecmp_rh_flowset_entries) {
            case 0:
                break;
            case 32768:
                if (soc_mem_index_count(unit,RH_ECMP_FLOWSETm) >= 32768) {
                    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                 ENHANCED_HASHING_CONTROLr,
                                 REG_PORT_ANY, ECMP_FLOWSET_TABLE_CONFIGf, 2));
                } else {
                    LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "%s:Total ECMP entries %d are less than RH entries :%d \n"),
                    FUNCTION_NAME(),  soc_mem_index_count(unit,RH_ECMP_FLOWSETm),
                    num_overlay_ecmp_rh_flowset_entries));
                    return SOC_E_CONFIG;
               }
                break;
            default:
                LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "%s: Value for overlay RH entries is not correct : %d \n"),
                    FUNCTION_NAME(),  num_overlay_ecmp_rh_flowset_entries));
               return SOC_E_CONFIG;
            }
        }
#endif

    /* Setup SW2_IFP_DST_ACTION_CONTROLr */
    fields[0] = HGTRUNK_RES_ENf;
    values[0] = 1;
    fields[1] = LAG_RES_ENf;
    values[1] = 1;
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit,
                SW2_IFP_DST_ACTION_CONTROLr, REG_PORT_ANY, 2, fields, values));

    /* Populate and enable RTAG7 Macro flow offset table */
    SOC_IF_ERROR_RETURN(_soc_trident3_macro_flow_offset_init(unit));

/* Enhanced hashing  LAG/TRUNK config
    * LAG/TRUNK share the same flow set table used in RH
    * 64k is shared between LAG and HGT for
    * - 32k each - default
    * - 64K dedicated to HGT RH
    * - 64K dedicated to LAG RH
    */
    num_lag_rh_flowset_entries = soc_property_get(unit,
                spn_TRUNK_RESILIENT_HASH_TABLE_SIZE, 32768);

    switch (num_lag_rh_flowset_entries) {
        case 0:
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROL_2r,
                        REG_PORT_ANY, HGT_LAG_FLOWSET_TABLE_CONFIGf, 1));
            break;
        case 65536:
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROL_2r,
                        REG_PORT_ANY, HGT_LAG_FLOWSET_TABLE_CONFIGf, 0));
            break;
        case 32768:
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROL_2r,
                        REG_PORT_ANY, HGT_LAG_FLOWSET_TABLE_CONFIGf, 2));
            break;
        default:
            return SOC_E_CONFIG;
    }

    SOC_IF_ERROR_RETURN
        (soc_counter_tomahawk_status_enable(unit, TRUE));

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) &&
        !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM))
    {
        SOC_IF_ERROR_RETURN(_soc_mv2_ledup_init(unit));
    }
#endif /* BCM_CMICX_SUPPORT */

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MISC Init completed (u=%d)\n"), unit));

    /* Mem Scan thread start should be the last step in Misc init */
    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));
        SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_start(unit));
    }

    return SOC_E_NONE;
}


#if !defined(BCM_MV2_ASF_EXCLUDE)
/*
 * Funtion to caltulate global buffer reserve for ASF.
 */
STATIC int
_soc_mv2_mmu_config_buf_asf(int unit, int lossless, int *cnt)
{
    soc_info_t *si;
    int pm, oversub, num_ports, ports[3], asf = 0;

    static int asf_profile[SOC_MAX_NUM_DEVICES] = {-2};
    int asf_reserved_cells[3][3][2] =     /* 1/2/4 port, asf profile, line/oversub*/
        {
            {{ 0,  0}, {14, 25}, { 39, 48}},     /* 1-port PM */
            {{ 0,  0}, {28, 50}, { 78, 96}},     /* 2-ports PM */
            {{ 0,  0}, {52, 96}, {156, 192}},    /* 3/4-ports PM */
        };

    /* To init static array to -1 */
    if (asf_profile[0] == -2) {
        sal_memset(asf_profile, -1, sizeof(asf_profile));
    }

    if (asf_profile[unit] == -1) {
        asf_profile[unit] = soc_property_get(unit, spn_ASF_MEM_PROFILE,
            _SOC_MV2_ASF_MEM_PROFILE_SIMILAR);
        if ((asf_profile[unit] > _SOC_MV2_ASF_MEM_PROFILE_EXTREME)
            || (asf_profile[unit] < _SOC_MV2_ASF_MEM_PROFILE_NONE)) {
            asf_profile[unit] = _SOC_MV2_ASF_MEM_PROFILE_NONE;
        }
    }

    si = &SOC_INFO(unit);
    oversub = SOC_PBMP_IS_NULL(si->oversub_pbm)? 0 : 1;

    ports[0] = ports[1] = ports[2] = 0;
    for (pm = 0; pm < _MV2_PBLKS_PER_DEV(unit); pm++) {
        num_ports = _soc_mv2_ports_per_pm_get(unit, pm);
        if (num_ports == 1) {
            ports[0]++;
        } else if (num_ports == 2) {
            ports[1]++;
        } else if ((num_ports == 4) || (num_ports == 3)) {
            ports[2]++;
        }
    }

    asf = asf_reserved_cells[0][asf_profile[unit]][oversub] * ports[0]
        + asf_reserved_cells[1][asf_profile[unit]][oversub] * ports[1]
        + asf_reserved_cells[2][asf_profile[unit]][oversub] * ports[2];

    if (cnt != NULL) {
        *cnt = asf;
    }

    return SOC_E_NONE;
}
#endif

/*
 * Function used for global resource reservation (MCQE / RQE / Buf cell).
 * Input state: Total hardware resource.
 * Output state: Resource available for admission control.
 */
STATIC void
_soc_mv2_mmu_config_dev_reserve(int unit, _soc_mmu_device_info_t *devcfg,
                                int lossless)
{
    int port, pm, asf_rsvd = 0;
    uint32 total_mcq = 0, num_ports = 0;
    uint32 cpu_cnt = 1, lb_cnt = 1, mgmt_cnt = 0;
    soc_info_t *si = &SOC_INFO(unit);

    SOC_PBMP_COUNT(si->management_pbm, mgmt_cnt);

    /* Device reservation for RQE Entry */
    devcfg->total_rqe_queue_entry -= _MV2_MMU_RQE_ENTRY_RSVD_PER_XPE;

    /* Device reservation for MCQ Entry - 6 entries for each MC queue */
    if (si->flex_eligible) {
        for (pm = 0; pm < _MV2_PBLKS_PER_DEV(unit); pm++) {
            num_ports += _soc_mv2_ports_per_pm_get(unit, pm);
        }
        total_mcq += (num_ports * SOC_MV2_NUM_MCAST_QUEUE_PER_PORT);
    } else {
        PBMP_ALL_ITER(unit, port) {
            if (IS_CPU_PORT(unit, port) ||
                IS_LB_PORT(unit,port) ||
                IS_MGMT_PORT(unit,port)) {
                continue;
            }
            total_mcq += si->port_num_cosq[port];
        }
    }
    total_mcq += ((cpu_cnt * SOC_MV2_NUM_CPU_QUEUES) +
                  (lb_cnt + mgmt_cnt) * SOC_MV2_NUM_MCAST_QUEUE_PER_PORT);

    devcfg->total_mcq_entry -= ((total_mcq * _MV2_MCQE_RSVD_PER_MCQ) +
                                _MV2_TDM_MCQE_RSVD_OVERSHOOT);

#if !defined(BCM_MV2_ASF_EXCLUDE)
    /* Device reservation for buffer cell */
    (void)_soc_mv2_mmu_config_buf_asf(unit, lossless, &asf_rsvd);
#endif

    devcfg->mmu_total_cell -= asf_rsvd;
}

STATIC void
_soc_mv2_mmu_init_dev_config(int unit, _soc_mmu_device_info_t *devcfg,
                             int lossless)
{
    if (devcfg == NULL) {
        LOG_FATAL(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                            "MMU config data error\
                            (u=%d)\n"), unit));
        return;
    }
    sal_memset(devcfg, 0, sizeof(_soc_mmu_device_info_t));

    devcfg->max_pkt_byte = _MV2_MMU_MAX_PACKET_BYTES;
    devcfg->mmu_hdr_byte = _MV2_MMU_PACKET_HEADER_BYTES;
    devcfg->jumbo_pkt_size = _MV2_MMU_JUMBO_FRAME_BYTES;
    devcfg->default_mtu_size = _MV2_MMU_DEFAULT_MTU_BYTES;
    devcfg->mmu_cell_size = _MV2_MMU_BYTES_PER_CELL;
    devcfg->mmu_total_cell = _MV2_MMU_TOTAL_CELLS_PER_XPE;
    devcfg->num_pg = _MV2_MMU_NUM_PG;
    devcfg->num_service_pool = _MV2_MMU_NUM_POOL;
    devcfg->flags = SOC_MMU_CFG_F_PORT_MIN | SOC_MMU_CFG_F_PORT_POOL_MIN |
                    SOC_MMU_CFG_F_RQE | SOC_MMU_CFG_F_EGR_MCQ_ENTRY;
    devcfg->total_mcq_entry = _MV2_MMU_TOTAL_MCQ_ENTRY(unit);
    devcfg->rqe_queue_num = 11;
    devcfg->total_rqe_queue_entry = _MV2_MMU_TOTAL_RQE_ENTRY(unit);

    _soc_mv2_mmu_config_dev_reserve(unit, devcfg, lossless);
}

STATIC void
_soc_mv2_mmu_sw_info_config (int unit, _soc_mv2_mmu_sw_config_info_t * swcfg,
                             int lossless)
{
    /* Default settings is lossless */
    swcfg->mmu_egr_queue_min = 0;
    swcfg->mmu_egr_qgrp_min = 0;
    swcfg->mmu_total_pri_groups = 8;
    swcfg->mmu_active_pri_groups = 1;
    swcfg->mmu_pg_min = 7;
    swcfg->mmu_port_min = 0;
    swcfg->mmu_mc_egr_qentry_min = 0;
    swcfg->mmu_rqe_qentry_min = 8;
    swcfg->mmu_rqe_queue_min = 7;
    if (lossless == 0)
    {
        swcfg->mmu_egr_queue_min = 7;
        swcfg->mmu_egr_qgrp_min = 14;
        swcfg->mmu_pg_min = 0;
    }
}

STATIC int
_soc_mv2_default_pg_headroom(int unit, soc_port_t port,
                            int lossless)
{
    int speed = 1000, hdrm = 0;
    uint8 port_oversub = 0;

    if (IS_CPU_PORT(unit, port)) {
        if (lossless) {
           return 69;
        }
        return 77;
    } else if (!lossless) {
        return 0;
    } else if (IS_LB_PORT(unit, port)) {
        return 160;
    }

    speed = SOC_INFO(unit).port_speed_max[port];

    if (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port)) {
        port_oversub = 1;
    }
    if (speed <= 11000) {
        hdrm = port_oversub ? 194 : 160;
    } else if (speed <= 21000) {
        hdrm = port_oversub ? 231 : 197;
    } else if (speed <= 27000) {
        hdrm = port_oversub ? 288 : 254;
    } else if (speed <= 42000) {
        hdrm = port_oversub ? 354 : 273;
    } else if (speed <= 53000) {
        hdrm = port_oversub ? 431 : 350;
    } else if (speed >= 100000) {
        hdrm = port_oversub ? 766 : 572;
    } else {
        hdrm = port_oversub ? 196 : 163;
    }
    return hdrm;
}

/*
 * Default phase_1 MMU settings about headroom, guarantee, dynamic, color, ...
 * which depend on single port/queue/qgroup/pg...
 * Input state: Available for admission in devcfg, configured swcfg.
 * Output state: Independent data filled in buf.
 * Version 1.1
 */
STATIC void
_soc_mv2_mmu_config_buf_phase1(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg,
                               _soc_mv2_mmu_sw_config_info_t *swcfg,
                               int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    int max_packet_cells, default_mtu_cells;
    int port, idx;
    int total_pool_size = 0, asf_rsvd = 0;
    int mcq_entry_shared_total;
    int rqe_entry_shared_total, qmin;

    si = &SOC_INFO(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Initializing default MMU config phase 1(u=%d)\n"), unit));
    max_packet_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->max_pkt_byte +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

#if !defined(BCM_MV2_ASF_EXCLUDE)
    (void)_soc_mv2_mmu_config_buf_asf(unit, lossless, &asf_rsvd);
#endif
    total_pool_size = devcfg->mmu_total_cell + asf_rsvd; /* Add back ASF reserved. */

    buf->headroom =  max_packet_cells;   /* 1 packet per pipe. */

    mcq_entry_shared_total = devcfg->total_mcq_entry;

    rqe_entry_shared_total = devcfg->total_rqe_queue_entry -
        (_MV2_MMU_NUM_RQE_QUEUES * swcfg->mmu_rqe_qentry_min);

    for (idx = 0; idx < _MV2_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if (idx == 0) {  /* 100% scale up by 100 */
            buf_pool->size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->yellow_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->red_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->total_mcq_entry = mcq_entry_shared_total;
            buf_pool->total_rqe_entry = rqe_entry_shared_total;
        } else {
            buf_pool->size = 0;
            buf_pool->yellow_size = 0;
            buf_pool->red_size = 0;
            buf_pool->total_mcq_entry = 0;
            buf_pool->total_rqe_entry = 0;
        }
    }

    for (idx = 0; idx < SOC_MV2_MMU_CFG_QGROUP_MAX; idx++) {
        queue_grp = &buf->qgroups[idx];
        /* resume limits - accounted for 8 cell granularity */
        queue_grp->pool_resume = 16;
        queue_grp->yellow_resume = 16;
        queue_grp->red_resume = 16;
        queue_grp->guarantee = swcfg->mmu_egr_qgrp_min;
        if (lossless) {
            queue_grp->discard_enable = 0;
        } else {
            queue_grp->discard_enable = 1;
        }
    }

    PBMP_ALL_ITER(unit, port) {
        if (port >= SOC_MMU_CFG_PORT_MAX) {
            break;
        }
        buf_port = &buf->ports[port];

        /* internal priority to priority group mapping */
        for (idx = 0; idx < _MV2_MMU_NUM_INT_PRI; idx++) {
            buf_port->pri_to_prigroup[idx] = 7;
        }

        /* priority group to pool mapping */
        for (idx = 0; idx < _MV2_MMU_NUM_PG; idx++) {
            buf_port->prigroups[idx].pool_idx = 0;
        }

        for (idx = 0; idx < _MV2_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            buf_port_pool->guarantee = 0;
            /* Port based accouting is always enabled
             * Set it to max for all service pools so that
             * it does not trigger drops by default */
            buf_port_pool->pool_limit = total_pool_size;
            buf_port_pool->pool_resume = total_pool_size -
                                         (default_mtu_cells * 2);
        }

        buf_port->pkt_size = max_packet_cells;

        /* priority group */
        for (idx = 0; idx < _MV2_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            buf_prigroup->guarantee = 0;
            buf_prigroup->user_delay = -1;
            buf_prigroup->switch_delay = -1;
            buf_prigroup->pkt_size = max_packet_cells;
            buf_prigroup->device_headroom_enable = 0;
            buf_prigroup->pool_floor = 0;
            buf_prigroup->headroom = 0;
            buf_prigroup->pool_resume = 0;
            buf_prigroup->flow_control_enable = 0;
            if (idx == 7) {
                buf_prigroup->device_headroom_enable = 1;
                buf_prigroup->flow_control_enable = lossless;
                buf_prigroup->headroom =
                    _soc_mv2_default_pg_headroom(unit, port, lossless);
                if (lossless) {
                    buf_prigroup->guarantee = swcfg->mmu_pg_min;
                }
            }
        }

        /* multicast queue */
        if (IS_CPU_PORT(unit, port) ||
            IS_LB_PORT(unit,port)) {
            qmin = swcfg->mmu_egr_queue_min;
        } else {
            qmin = 0;
        }
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];
            buf_queue->qgroup_id = -1;
            buf_queue->guarantee = qmin;
            buf_queue->mcq_entry_guarantee = swcfg->mmu_mc_egr_qentry_min;
            buf_queue->color_discard_enable = 1;
            buf_queue->pool_scale = 8;
            buf_queue->discard_enable = 1;
            /* resume limits - accounted for 8 cell granularity */
            buf_queue->pool_resume = 16;
        }

        /* unicast queue */
        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];
            buf_queue->qgroup_id = -1;
            buf_queue->guarantee = 0;
            buf_queue->color_discard_enable = 0;
            if (lossless) {
                buf_queue->discard_enable = 0;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;
            } else {
                buf_queue->discard_enable = 1;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;
                buf_queue->qgroup_id = 0;
                buf_queue->qgroup_min_enable = 1;
            }
        }

        /* queue to pool mapping */
        for (idx = 0;
             idx < si->port_num_cosq[port] + si->port_num_uc_cosq[port]; idx++) {
            buf_port->queues[idx].pool_idx = 0;
        }
    }

    /* RQE */
    for (idx = 0; idx < _MV2_MMU_NUM_RQE_QUEUES; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];
        buf_rqe_queue->pool_idx = 0;
        buf_rqe_queue->guarantee = swcfg->mmu_rqe_queue_min;
        if (lossless) {
            buf_rqe_queue->discard_enable = 0;
        } else {
            buf_rqe_queue->discard_enable = 1;
        }
    }
}

/*
 * Default phase_2 MMU settings about shared buffer limit, which depends on
 * guaranteed/headroom buffers on all port/queue/qgroup/pg/...
 * Input state: _soc_mmu_cfg_buf_calculate processed buf.
 * Output state: All necessary data filled in buf.
 * Version 1.1
 */
STATIC void
_soc_mv2_mmu_config_buf_phase2(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg,
                               _soc_mv2_mmu_sw_config_info_t *swcfg,
                               int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    int port, idx, pm, ports[3] = {0};
    int pg_hdrm_ob[] = {690, 372, 220}; /* PG headroom of 1/2/4-port PortMacro */
    int total_pool_size = 0;
    int ing_rsvd_total = 0, egr_rsvd_total = 0;
    int ing_shared_total, egr_shared_total;
    int total_rsvd_qmin = 0;
    uint32 color_limit = 0;
    uint32 num_ports = 0, mgmt_cnt = 0, cpu_cnt = 0, lb_cnt = 0;
    uint32 cpu_hdrm = 0, lb_hdrm = 0, mgmt_hdrm = 0;

    si = &SOC_INFO(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Initializing default MMU config phase 2(u=%d)\n"), unit));

    buf_pool = &buf->pools[0];
    total_pool_size = devcfg->mmu_total_cell; /* per XPE limit */
    total_rsvd_qmin = swcfg->mmu_egr_queue_min * (SOC_MV2_NUM_CPU_QUEUES +
                        (SOC_MV2_NUM_MCAST_QUEUE_PER_PORT));

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            cpu_cnt++;
            cpu_hdrm = _soc_mv2_default_pg_headroom(unit, port, lossless);
        } else if (IS_LB_PORT(unit,port)) {
            lb_cnt++;
            lb_hdrm = _soc_mv2_default_pg_headroom(unit, port, lossless);
        } else if (IS_MGMT_PORT(unit,port)) {
            mgmt_cnt++;
            mgmt_hdrm = _soc_mv2_default_pg_headroom(unit, port, lossless);
        } else {
            num_ports++;
        }
    }
    if (si->flex_eligible) {
        num_ports = 0;
        for (pm = 0; pm < _MV2_PBLKS_PER_DEV(unit); pm++) {
            port = _soc_mv2_ports_per_pm_get(unit, pm);
            if (port == 1) {
                ports[0]++;
            } else if (port == 2) {
                ports[1]++;
            } else if ((port == 4) || (port == 3)) {
                ports[2]++;
                port = 4;
            }
            num_ports += port;
        }

        if (lossless) {
            ing_rsvd_total += (buf->headroom +
                swcfg->mmu_active_pri_groups*(
                ports[0]*pg_hdrm_ob[0] + 2*ports[1]*pg_hdrm_ob[1] +
                4*ports[2]*pg_hdrm_ob[2] + cpu_cnt*cpu_hdrm + lb_cnt*lb_hdrm) +
                swcfg->mmu_pg_min*swcfg->mmu_active_pri_groups*(
                num_ports + cpu_cnt + lb_cnt + mgmt_cnt) +
                swcfg->mmu_port_min*swcfg->mmu_active_pri_groups*(
                num_ports + cpu_cnt + lb_cnt + mgmt_cnt));
            /* Management port is not included during Total PG
             * headroom calculation */
            if (buf_pool->prigroup_headroom >= mgmt_hdrm) {
                buf_pool->prigroup_headroom -= mgmt_hdrm;
            }
        } else {
            ing_rsvd_total += buf->headroom + cpu_hdrm;
        }
    } else {
        if (lossless && buf_pool->prigroup_headroom >= mgmt_hdrm) {
            /* Management port is not included during Total PG
             * headroom calculation */
            buf_pool->prigroup_headroom -= mgmt_hdrm;
        }
        ing_rsvd_total += (buf->headroom + buf_pool->prigroup_headroom +
                           buf_pool->prigroup_guarantee);
    }

    egr_rsvd_total += total_rsvd_qmin +
                       ((num_ports + mgmt_cnt) * swcfg->mmu_egr_qgrp_min) +
                       (swcfg->mmu_rqe_queue_min * _MV2_MMU_NUM_RQE_QUEUES);

    if (lossless) {
        ing_rsvd_total += egr_rsvd_total;
    }

    ing_shared_total = total_pool_size - ing_rsvd_total;
    egr_shared_total = total_pool_size - egr_rsvd_total;

    swcfg->ing_shared_total = ing_shared_total;
    swcfg->egr_shared_total = egr_shared_total;

    LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "MMU config: Total Shared size: ingress %d egress %d\n"),
                            ing_shared_total, egr_shared_total));

    color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;

    for (idx = 0; idx < SOC_MV2_MMU_CFG_QGROUP_MAX; idx++) {
        queue_grp = &buf->qgroups[idx];
        if (lossless) {
            queue_grp->pool_scale = -1;
            queue_grp->pool_limit = egr_shared_total;
            queue_grp->red_limit = egr_shared_total;
            queue_grp->yellow_limit = egr_shared_total;
        } else {
            queue_grp->pool_scale = 8;
            queue_grp->pool_limit = 0;
            queue_grp->red_limit = color_limit;
            queue_grp->yellow_limit = color_limit;
        }
    }

    PBMP_ALL_ITER(unit, port) {
        if (port >= SOC_MMU_CFG_PORT_MAX) {
            break;
        }
        buf_port = &buf->ports[port];

        /* priority group */
        for (idx = 0; idx < _MV2_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            buf_prigroup->pool_limit = 0;
            buf_prigroup->pool_scale = -1;
            if (idx == 7) {
                if (lossless) {
                    buf_prigroup->pool_scale = 8;
                } else {
                    buf_prigroup->pool_limit = ing_shared_total;
                }
            }
        }

        /* multicast queue */
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];
            buf_queue->pool_scale = 8;
            buf_queue->pool_limit = 0;
            buf_queue->yellow_limit = color_limit;
            buf_queue->red_limit = color_limit;
        }

        /* unicast queue */
        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];
            if (lossless) {
                buf_queue->pool_scale = -1;
                buf_queue->pool_limit = egr_shared_total;
                buf_queue->red_limit = egr_shared_total;
                buf_queue->yellow_limit = egr_shared_total;
            } else {
                buf_queue->pool_scale = 8;
                buf_queue->pool_limit = 0;
                buf_queue->red_limit = color_limit;
                buf_queue->yellow_limit = color_limit;
            }
        }
    }

    /* RQE */
    for (idx = 0; idx < _MV2_MMU_NUM_RQE_QUEUES; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];
        if (lossless) {
            buf_rqe_queue->pool_scale = -1;
            buf_rqe_queue->pool_limit = egr_shared_total;
            buf_rqe_queue->red_limit = egr_shared_total;
            buf_rqe_queue->yellow_limit = egr_shared_total;
        } else {
            buf_rqe_queue->pool_scale = 8;
            buf_rqe_queue->pool_limit = 0;
            buf_rqe_queue->red_limit = color_limit;
            buf_rqe_queue->yellow_limit = color_limit;
        }
    }
}

STATIC int
_soc_mv2_pool_scale_to_limit(int size, int scale)
{
    int factor = 1000;

    switch (scale) {
        case 7: factor = 875; break;
        case 6: factor = 750; break;
        case 5: factor = 625; break;
        case 4: factor = 500; break;
        case 3: factor = 375; break;
        case 2: factor = 250; break;
        case 1: factor = 125; break;
        case 0:
        default:
            factor = 1000; break;
    }
    return (size * factor)/1000;
}

STATIC int
_soc_mv2_mmu_config_buf_set_hw_port(int unit, int port, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless,
                               _soc_mv2_mmu_sw_config_info_t *swcfg)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    thdi_port_sp_config_entry_t thdi_sp_config;
    thdi_port_pg_config_entry_t pg_config_mem;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    uint32 entry0[SOC_MAX_MEM_WORDS], entry1[SOC_MAX_MEM_WORDS];
    soc_reg_t reg = INVALIDr;
    soc_mem_t mem0, mem1, mem2, mem3;
    uint32 fval, rval;
    int base, numq;
    int idx, midx, pri, index1;
    int default_mtu_cells, limit, rlimit;
    int pipe;

    static const soc_mem_t mmu_thdi_port_mem[] = {
        THDI_PORT_SP_CONFIGm,
        THDI_PORT_PG_CONFIGm
    };
    static const soc_mem_t mmu_thdo_q_uc_mem[] = {
        MMU_THDU_CONFIG_QUEUEm,
        MMU_THDU_OFFSET_QUEUEm,
        MMU_THDU_Q_TO_QGRP_MAPm
    };
    static const soc_mem_t mmu_thdo_port_uc_mem[] = {
        MMU_THDU_CONFIG_PORTm,
        MMU_THDU_RESUME_PORTm
    };
    static const soc_mem_t mmu_thdo_q_mc_mem[] = {
        MMU_THDM_DB_QUEUE_CONFIGm,
        MMU_THDM_DB_QUEUE_OFFSETm,
        MMU_THDM_MCQE_QUEUE_CONFIGm,
        MMU_THDM_MCQE_QUEUE_OFFSETm
    };

    static const soc_mem_t mmu_thdo_port_mc_mem[] = {
        MMU_THDM_DB_PORTSP_CONFIGm,
        MMU_THDM_MCQE_PORTSP_CONFIGm
    };

    static const soc_field_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };
    static const soc_field_t prigroup_hpid_field[] = {
        PG0_HPIDf, PG1_HPIDf, PG2_HPIDf, PG3_HPIDf,
        PG4_HPIDf, PG5_HPIDf, PG6_HPIDf, PG7_HPIDf
    };

    si = &SOC_INFO(unit);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

    /* internal priority to priority group mapping */
    buf_port = &buf->ports[port];

    for (idx = 0; idx < COUNTOF(prigroup_field); idx++) {
        if (idx % 8 == 0) { /* 8 fields per register */
            reg = prigroup_reg[idx / 8];
            rval = 0;
        }
        soc_reg_field_set(unit, reg, &rval, prigroup_field[idx],
                buf_port->pri_to_prigroup[idx]);
        if (idx % 8 == 7) { /* 8 fields per register */
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
        }
    }

    /* Input port per port settings */
    pipe = si->port_pipe[port];
    buf_port = &buf->ports[port];

    rval = 0;
    for (idx = 0; idx < _MV2_MMU_NUM_PG; idx++) {
        soc_reg_field_set(unit, THDI_PORT_PG_SPIDr, &rval,
                          prigroup_spid_field[idx],
                          buf_port->prigroups[idx].pool_idx);
    }
    SOC_IF_ERROR_RETURN(WRITE_THDI_PORT_PG_SPIDr(unit, port, rval));

    
    rval = 0;
    for (idx = 0; idx < _MV2_MMU_NUM_PG; idx++) {
        soc_reg_field_set(unit, THDI_HDRM_PORT_PG_HPIDr, &rval,
                          prigroup_hpid_field[idx],
                          buf_port->prigroups[idx].pool_idx);
    }
    SOC_IF_ERROR_RETURN(WRITE_THDI_HDRM_PORT_PG_HPIDr(unit, port, rval));

    /* Per port per pool settings */
    mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdi_port_mem[0])[pipe];
    if (mem0 != INVALIDm) {
        for (idx = 0; idx < _MV2_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            midx = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, port, mmu_thdi_port_mem[0],
                                               idx);
            sal_memset(&thdi_sp_config, 0, sizeof(thdi_sp_config));
            soc_mem_field32_set(unit, mem0, &thdi_sp_config,
                                PORT_SP_MIN_LIMITf, buf_port_pool->guarantee);
            soc_mem_field32_set(unit, mem0, &thdi_sp_config,
                            PORT_SP_RESUME_LIMITf, buf_port_pool->pool_resume);
            soc_mem_field32_set(unit, mem0, &thdi_sp_config,
                                PORT_SP_MAX_LIMITf, buf_port_pool->pool_limit);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL,
                               midx, &thdi_sp_config));
        }
    }

    fval = 0;
    for (idx = 0; idx < _MV2_MMU_NUM_PG; idx++) {
        if (buf_port->prigroups[idx].flow_control_enable != 0) {
            for (pri=0; pri < 16; pri++) {
                if (buf_port->pri_to_prigroup[pri] == idx) {
                    fval |= 1 << pri;
                }
            }
        }
    }

    rval = 0;
    soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                      INPUT_PORT_RX_ENABLEf, 1);
    if (port == CMIC_PORT (unit)) {
        fval = 0;
    }
    soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                      PORT_PRI_XON_ENABLEf, fval);
    soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                      PORT_PAUSE_ENABLEf, fval ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_THDI_INPUT_PORT_XON_ENABLESr(unit,
                        port, rval));

    rval = 0;
    soc_reg_field_set(unit, THDI_PORT_MAX_PKT_SIZEr, &rval,
                      PORT_MAX_PKT_SIZEf, buf_port->pkt_size);
    SOC_IF_ERROR_RETURN(WRITE_THDI_PORT_MAX_PKT_SIZEr(unit, rval));

    /* Input port per port per priority group settings */
    mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdi_port_mem[1])[pipe];
    if (mem1 != INVALIDm) {
        for (idx = 0; idx < _MV2_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf->ports[port].prigroups[idx];

            midx = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, port, mmu_thdi_port_mem[1],
                                               idx);
            sal_memset(&pg_config_mem, 0, sizeof(pg_config_mem));
            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_MIN_LIMITf, buf_prigroup->guarantee);

            if (buf_prigroup->pool_scale != -1) {
                soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                    PG_SHARED_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                    PG_SHARED_LIMITf,
                                    buf_prigroup->pool_scale);
            } else {
                soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                    PG_SHARED_LIMITf,
                                    buf_prigroup->pool_limit);
            }

            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_GBL_HDRM_ENf,
                                buf_prigroup->device_headroom_enable);
            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_HDRM_LIMITf, buf_prigroup->headroom);

            soc_mem_field32_set(unit, mem1, &pg_config_mem, PG_RESET_OFFSETf,
                                0);

            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_RESET_FLOORf, buf_prigroup->pool_floor);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL, midx,
                                &pg_config_mem));
        }
    }

    /***********************************
     * THDO
 */
    /* Output port per port per queue setting for regular multicast queue */
    pipe = si->port_pipe[port];
    numq = si->port_num_cosq[port];
    base = si->port_cosq_base[port];

    mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[0])[pipe];
    mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[1])[pipe];
    if ((numq != 0) && (mem0 != INVALIDm) && (mem1 != INVALIDm)) {
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[idx];

            sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_config_0_entry_t));

            soc_mem_field32_set(unit, mem0, entry0, Q_MIN_LIMITf,
                                buf_queue->guarantee);
            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem0, entry0, Q_COLOR_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem0, entry0, Q_SHARED_ALPHAf,
                                    buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0, Q_SHARED_LIMITf,
                                    buf_queue->pool_limit);
            }
            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                    (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem0, entry0, Q_COLOR_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem0, entry0, YELLOW_SHARED_LIMITf,
                        buf_queue->yellow_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                        buf_queue->red_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0, YELLOW_SHARED_LIMITf,
                                    sal_ceil_func(buf_queue->yellow_limit, 8));
                soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                                    sal_ceil_func(buf_queue->red_limit, 8));
            }

            soc_mem_field32_set(unit, mem0, entry0, Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, base + idx, entry0));

            sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_offset_entry_t));

            soc_mem_field32_set(unit, mem1, entry0, RESUME_OFFSETf,
                                (default_mtu_cells * 2)/8);
            soc_mem_field32_set(unit, mem1, entry0,
                                YELLOW_RESUME_OFFSET_PROFILE_SELf, 0);
            soc_mem_field32_set(unit, mem1, entry0,
                                RED_RESUME_OFFSET_PROFILE_SELf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, base + idx, entry0));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                    WRITE_MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_YELLOWr(unit, 0,
                        2));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                    WRITE_MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_REDr(unit, 0,
                        2));
        }
    }

    mem2 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[2])[pipe];
    mem3 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[3])[pipe];
    if ((numq != 0) && (mem2 != INVALIDm) && (mem3 != INVALIDm)) {
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[idx];
            buf_pool = &buf->pools[buf_queue->pool_idx];
            if (buf_pool->total == 0) {
                continue;
            }
            sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_queue_config_entry_t));

            soc_mem_field32_set(unit, mem2, entry0, Q_MIN_LIMITf,
                                sal_ceil_func(buf_queue->mcq_entry_guarantee, 4));
            limit = buf_pool->total_mcq_entry - buf_pool->mcq_entry_reserved;

            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0, Q_COLOR_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem2, entry0, Q_SHARED_ALPHAf,
                                    buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem2, entry0, Q_SHARED_LIMITf,
                                    sal_ceil_func(limit, 4));
            }
            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                    (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem2, entry0, Q_COLOR_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem2, entry0, YELLOW_SHARED_LIMITf,
                        buf_queue->yellow_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem2, entry0, RED_SHARED_LIMITf,
                        buf_queue->red_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem2, entry0, YELLOW_SHARED_LIMITf,
                                    sal_ceil_func(limit, 8));
                soc_mem_field32_set(unit, mem2, entry0, RED_SHARED_LIMITf,
                                    sal_ceil_func(limit, 8));
            }

            soc_mem_field32_set(unit, mem2, entry0, Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem2, MEM_BLOCK_ALL, base + idx, entry0));

            sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_queue_offset_entry_t));

            soc_mem_field32_set(unit, mem3, entry0, RESUME_OFFSETf, 1);
            soc_mem_field32_set(unit, mem3, entry0,
                                YELLOW_RESUME_OFFSET_PROFILE_SELf, 0);
            soc_mem_field32_set(unit, mem3, entry0,
                                RED_RESUME_OFFSET_PROFILE_SELf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem3, MEM_BLOCK_ALL, base + idx, entry0));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                    WRITE_MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_YELLOWr(unit, 0,
                        1));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                    WRITE_MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_REDr(unit, 0,
                        1));
        }
    }

    /* Per  port per pool */
    for (idx = 0; idx < _MV2_MMU_NUM_POOL; idx++) {
        buf_port_pool = &buf->ports[port].pools[idx];
        if (buf_port_pool->pool_limit == 0) {
            continue;
        }

        limit = buf_port_pool->pool_limit;
        rlimit = limit - (default_mtu_cells * 2);

        mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_mc_mem[0])[pipe];
        if (mem0 == INVALIDm) {
            continue;
        }

        index1 = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, port,
                                             mmu_thdo_port_mc_mem[0], idx);
        sal_memset(entry0, 0, sizeof(mmu_thdm_db_portsp_config_entry_t));

        soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMITf, limit);
        soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                            sal_ceil_func(limit, 8));
        soc_mem_field32_set(unit, mem0, entry0, YELLOW_SHARED_LIMITf,
                            sal_ceil_func(limit, 8));

        soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMIT_ENABLEf,
                            !lossless);

        soc_mem_field32_set(unit, mem0, entry0, SHARED_RESUME_LIMITf,
                            sal_ceil_func(rlimit, 8));
        soc_mem_field32_set(unit, mem0, entry0, YELLOW_RESUME_LIMITf,
                            sal_ceil_func(rlimit, 8));
        soc_mem_field32_set(unit, mem0, entry0, RED_RESUME_LIMITf,
                            sal_ceil_func(rlimit, 8));

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem0, MEM_BLOCK_ALL,
                            index1, entry0));

        mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_mc_mem[1])[pipe];
        if (mem1 == INVALIDm) {
            continue;
        }
        buf_pool = &buf->pools[idx];
        sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_portsp_config_entry_t));

        limit = _MV2_MMU_TOTAL_MCQ_ENTRY(unit);

        soc_mem_field32_set(unit, mem1, entry0, SHARED_LIMITf,
                            (sal_ceil_func(limit, 4)) - 1);
        soc_mem_field32_set(unit, mem1, entry0, YELLOW_SHARED_LIMITf,
                            (sal_ceil_func(limit, 8)) - 1);
        soc_mem_field32_set(unit, mem1, entry0, RED_SHARED_LIMITf,
                            (sal_ceil_func(limit, 8)) - 1);

        soc_mem_field32_set(unit, mem1, entry0, SHARED_LIMIT_ENABLEf,
                            !lossless);

        soc_mem_field32_set(unit, mem1, entry0, SHARED_RESUME_LIMITf,
                            (sal_ceil_func(limit, 8)) - 2);
        soc_mem_field32_set(unit, mem1, entry0, YELLOW_RESUME_LIMITf,
                            (sal_ceil_func(limit, 8)) - 2);
        soc_mem_field32_set(unit, mem1, entry0, RED_RESUME_LIMITf,
                            (sal_ceil_func(limit, 8)) - 2);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL,
                            index1, entry0));
    }

    /* Output port per port per queue setting for regular unicast queue */
    pipe = si->port_pipe[port];
    /* per port regular unicast queue */
    numq = si->port_num_uc_cosq[port];
    base = si->port_uc_cosq_base[port];

    mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[0])[pipe];
    mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[1])[pipe];
    mem2 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[2])[pipe];
    if ((numq != 0) &&
        (mem0 != INVALIDm) && (mem1 != INVALIDm) && (mem2 != INVALIDm)) {
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[si->port_num_cosq[port] + idx];

            sal_memset(entry0, 0, sizeof(mmu_thdu_config_queue_entry_t));
            sal_memset(entry1, 0, sizeof(mmu_thdu_offset_queue_entry_t));

            soc_mem_field32_set(unit, mem0, entry0,
                                Q_MIN_LIMIT_CELLf, buf_queue->guarantee);
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_DYNAMIC_CELLf, 1);
                soc_mem_field32_set(unit, mem0, entry0, Q_SHARED_ALPHA_CELLf,
                                    buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0, Q_SHARED_LIMIT_CELLf,
                                    buf_queue->pool_limit);
            }
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_CELLf,
                                sal_ceil_func(buf_queue->pool_resume, 8));

            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                    (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_YELLOW_CELLf,
                        buf_queue->yellow_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_RED_CELLf,
                        buf_queue->red_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_YELLOW_CELLf,
                                    sal_ceil_func(buf_queue->yellow_limit, 8));
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_RED_CELLf,
                                    sal_ceil_func(buf_queue->red_limit, 8));
            }
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_YELLOW_CELLf,
                                sal_ceil_func(buf_queue->yellow_resume, 8));
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_RED_CELLf,
                                sal_ceil_func(buf_queue->red_resume, 8));
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, base + idx, entry0));

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, base + idx, entry1));

            sal_memset(entry0, 0, sizeof(mmu_thdo_q_to_qgrp_map_entry_t));

            if (buf_queue->qgroup_id >= 0) {
                soc_mem_field32_set(unit, mem2, entry0, QGROUP_VALIDf, 1);
                if (buf_queue->qgroup_min_enable) {
                    soc_mem_field32_set(unit, mem2, entry0, USE_QGROUP_MINf, 1);
                }
            }
            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_ENABLEf, 1);
            }

            soc_mem_field32_set(unit, mem2, entry0, Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL, base + idx,
                                entry0));
        }
    }

    /* Per  port per pool unicast */
    for (idx = 0; idx < _MV2_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];

        if (buf_pool->total == 0) {
            continue;
        }

        limit = buf->ports[port].pools[idx].pool_limit;

        mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_uc_mem[0])[pipe];
        mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_uc_mem[1])[pipe];
        if ((mem0 == INVALIDm) || (mem1 == INVALIDm)) {
            continue;
        }
        index1 = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, port,
                                             mmu_thdo_port_uc_mem[0], idx);
        sal_memset(entry0, 0, sizeof(mmu_thdu_config_port_entry_t));
        sal_memset(entry1, 0, sizeof(mmu_thdu_resume_port_entry_t));

        soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMITf, limit);
        soc_mem_field32_set(unit, mem0, entry0, YELLOW_LIMITf,
                            sal_ceil_func(limit, 8) / 8);
        soc_mem_field32_set(unit, mem0, entry0, RED_LIMITf,
                            sal_ceil_func(limit, 8) / 8);
        if (limit >= default_mtu_cells * 2) {
            soc_mem_field32_set(unit, mem1, entry1, SHARED_RESUMEf,
                            sal_ceil_func((limit - (default_mtu_cells * 2)),
                            8) / 8);

            soc_mem_field32_set(unit, mem1, entry1, YELLOW_RESUMEf,
                            sal_ceil_func((limit - (default_mtu_cells * 2)),
                            8) / 8);

            soc_mem_field32_set(unit, mem1, entry1, RED_RESUMEf,
                            sal_ceil_func((limit - (default_mtu_cells * 2)),
                            8) / 8);
        }

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem0, MEM_BLOCK_ALL,
                            index1, entry0));
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL,
                            index1, entry1));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_mv2_mmu_config_buf_set_hw_global(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless,
                               _soc_mv2_mmu_sw_config_info_t *swcfg)
{
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    mmu_thdo_config_qgroup_entry_t cfg_qgrp;
    mmu_thdo_offset_qgroup_entry_t offset_qgrp;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    soc_mem_t mem0, mem1;
    uint32 fval, rval, rval2, rval3;
    int rqlen, idx, pval;
    int jumbo_frame_cells, default_mtu_cells, limit;
    int pipe;
    uint64 rval64;
    int pool_resume = 0;

     static const soc_mem_t mmu_thdo_qgrp_uc_mem[] = {
         MMU_THDU_CONFIG_QGROUPm,
         MMU_THDU_OFFSET_QGROUPm
    };

    jumbo_frame_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->jumbo_pkt_size +
                                                    devcfg->mmu_hdr_byte,
                                                    devcfg->mmu_cell_size);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    pool_resume = 2 * jumbo_frame_cells;

    rval = 0;
    fval = _MV2_MMU_PHYSICAL_CELLS_PER_XPE - _MV2_MMU_RSVD_CELLS_CFAP_PER_XPE;
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDSETr, &rval, CFAPFULLSETPOINTf,
                      fval);
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg32_set(unit, CFAPFULLTHRESHOLDSETr, -1, -1,
                                   -1, rval));

    rval = 0;
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDRESETr, &rval,
                      CFAPFULLRESETPOINTf, fval - (2 * jumbo_frame_cells));
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg32_set(unit, CFAPFULLTHRESHOLDRESETr, -1, -1,
                                   -1, rval));

    rval = 0;
    soc_reg_field_set(unit, CFAPBANKFULLr, &rval, LIMITf,
                      _MV2_MMU_CFAP_BANK_FULL_LIMIT);
    for (idx = 0; idx < SOC_REG_NUMELS(unit, CFAPBANKFULLr); idx++) {
        SOC_IF_ERROR_RETURN(
            soc_trident3_xpe_reg32_set(unit, CFAPBANKFULLr, -1, -1,
                                       idx, rval));
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_MMU_SCFG_TOQ_MC_CFG0r(unit, &rval));
    soc_reg_field_set(unit, MMU_SCFG_TOQ_MC_CFG0r, &rval,
                      MCQE_FULL_THRESHOLDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MMU_SCFG_TOQ_MC_CFG0r(unit, rval));

    rval = 0;
    for (idx = 0; idx < 2; idx++) {
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, THDI_HDRM_POOL_CFGr, idx, 0, &rval));
        soc_reg_field_set(unit, THDI_HDRM_POOL_CFGr, &rval,
                          PEAK_COUNT_UPDATE_EN_HPf, 0);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, THDI_HDRM_POOL_CFGr, idx, 0, rval));
    }

    /* Input thresholds */
    rval = 0;
    soc_reg_field_set(unit, THDI_GLOBAL_HDRM_LIMITr,
                      &rval, GLOBAL_HDRM_LIMITf, buf->headroom );
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, THDI_GLOBAL_HDRM_LIMITr,
                                      REG_PORT_ANY, 0, rval));

    for (idx = 0; idx < _MV2_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        limit = buf_pool->total -
                buf_pool->port_guarantee -
                buf_pool->prigroup_guarantee -
                buf_pool->prigroup_headroom;

        rval = 0;
        soc_reg_field_set(unit, THDI_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
                          limit);
        SOC_IF_ERROR_RETURN(
            soc_trident3_xpe_reg32_set(unit, THDI_BUFFER_CELL_LIMIT_SPr,
                                       -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, THDI_CELL_RESET_LIMIT_OFFSET_SPr, &rval,
                          OFFSETf, pool_resume);
        SOC_IF_ERROR_RETURN(
            soc_trident3_xpe_reg32_set(unit, THDI_CELL_RESET_LIMIT_OFFSET_SPr,
                                       -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, THDI_HDRM_BUFFER_CELL_LIMIT_HPr, &rval,
                          LIMITf, buf_pool->prigroup_headroom);
        SOC_IF_ERROR_RETURN(
            soc_trident3_xpe_reg32_set(unit, THDI_HDRM_BUFFER_CELL_LIMIT_HPr,
                                       -1, -1, idx, rval));
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg32_set(unit, THDI_BUFFER_CELL_LIMIT_PUBLIC_POOLr,
                                   -1, -1, 0, rval));

    /* output thresholds */
    SOC_IF_ERROR_RETURN(READ_OP_THDU_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval,
                      ENABLE_QUEUE_AND_GROUP_TICKETf, 1);
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval,
                      ENABLE_UPDATE_COLOR_RESUMEf, 0);
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval, MOP_POLICY_1Bf, 1);
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval, MOP_POLICY_1Af, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_THDU_CONFIGr(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, MMU_THDM_DB_DEVICE_THR_CONFIGr, &rval,
                      MOP_POLICYf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_MMU_THDR_DB_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, MMU_THDR_DB_CONFIGr, &rval,
                      MOP_POLICY_1Bf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIGr(unit, rval));

    /* per service pool settings */
    for (idx = 0; idx < _MV2_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        limit = buf_pool->total -
                buf_pool->queue_group_guarantee -
                buf_pool->queue_guarantee -
                buf_pool->rqe_guarantee;

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_SHARED_LIMITr, &rval,
                          SHARED_LIMITf, limit);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_SHARED_LIMITr(unit,
                                                                 idx,
                                                                 rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr,
                          &rval, YELLOW_SHARED_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr(unit,
                                                                        idx,
                                                                        rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RED_SHARED_LIMITr,
                          &rval, RED_SHARED_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_RED_SHARED_LIMITr(unit,
                                                                     idx,
                                                                     rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RESUME_LIMITr,
                          &rval, RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_RESUME_LIMITr(unit,
                                                                 idx,
                                                                 rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr,
                          &rval, YELLOW_RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr(unit,
                                                                        idx,
                                                                        rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RED_RESUME_LIMITr,
                          &rval, RED_RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_RED_RESUME_LIMITr(unit,
                                                                     idx,
                                                                     rval));

        /* mcq entries */
        limit = buf_pool->total_mcq_entry - buf_pool->mcq_entry_reserved;

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_SHARED_LIMITr,
                          &rval, SHARED_LIMITf, sal_ceil_func(limit, 4));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_SHARED_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr,
                          &rval, YELLOW_SHARED_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr,
                          &rval, RED_SHARED_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RESUME_LIMITr,
                          &rval, RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_RESUME_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr,
                          &rval, YELLOW_RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr,
                          &rval, RED_RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr(unit,
                                                                   idx, rval));
    }

    /* configure Q-groups */
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        for (idx = 0; idx < SOC_MV2_MMU_CFG_QGROUP_MAX; idx++) {
            mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_qgrp_uc_mem[0])[pipe];
            mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_qgrp_uc_mem[1])[pipe];
            if ((mem0 == INVALIDm) || (mem1 == INVALIDm)) {
                continue;
            }
            queue_grp = &buf->qgroups[idx];


            sal_memset(&cfg_qgrp, 0, sizeof(cfg_qgrp));
            sal_memset(&offset_qgrp, 0, sizeof(offset_qgrp));

            soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                Q_MIN_LIMIT_CELLf, queue_grp->guarantee);

            if (queue_grp->pool_scale != -1) {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    Q_SHARED_ALPHA_CELLf,
                                    queue_grp->pool_scale);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    Q_LIMIT_DYNAMIC_CELLf, 1);
            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    Q_SHARED_LIMIT_CELLf,
                                    queue_grp->pool_limit);
            }

            if ((queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
            }

            if (queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                pval = _soc_mv2_pool_scale_to_limit(queue_grp->pool_limit,
                          queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_RED_CELLf, sal_ceil_func(pval, 8));

            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_RED_CELLf,
                                    sal_ceil_func(queue_grp->red_limit, 8));
            }

            if (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                pval = _soc_mv2_pool_scale_to_limit(queue_grp->pool_limit,
                          queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_YELLOW_CELLf, sal_ceil_func(pval, 8));

            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_YELLOW_CELLf,
                                    sal_ceil_func(queue_grp->yellow_limit, 8));
            }

            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_CELLf,
                                sal_ceil_func(queue_grp->pool_resume, 8));
            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_YELLOW_CELLf,
                                sal_ceil_func(queue_grp->pool_resume, 8));
            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_RED_CELLf,
                                sal_ceil_func(queue_grp->pool_resume, 8));
            SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, idx, &cfg_qgrp));
            SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, idx, &offset_qgrp));
        }
    }

    /* RQE */
    for (idx = 0; idx < _MV2_MMU_NUM_RQE_QUEUES; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_MIN_PRIQr,
                          &rval, MIN_LIMITf, buf_rqe_queue->guarantee);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_LIMIT_MIN_PRIQr(unit, idx, rval));

        rval = 0;
        rval2 = 0;
        rval3 = 0;
        COMPILER_64_SET(rval64, 0, 0);
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                            &rval, SPIDf, buf_rqe_queue->pool_idx);

        if ((buf_rqe_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
            (buf_rqe_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                              &rval, COLOR_LIMIT_DYNAMICf, 1);
            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                SHARED_RED_LIMITf,
                buf_rqe_queue->red_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                SHARED_YELLOW_LIMITf,
                buf_rqe_queue->yellow_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
        } else {
            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                              SHARED_RED_LIMITf,
                              sal_ceil_func(buf_rqe_queue->red_limit, 8));

            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                              SHARED_YELLOW_LIMITf,
                              sal_ceil_func(buf_rqe_queue->yellow_limit, 8));
        }

        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr, &rval, LIMIT_ENABLEf,
                          (buf_rqe_queue->discard_enable ? 1 : 0));

        if (buf_rqe_queue->pool_scale != -1) {
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                              &rval, DYNAMIC_ENABLEf, 1);
            soc_reg64_field32_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval64,
                              SHARED_ALPHAf, buf_rqe_queue->pool_scale);
        } else {
            soc_reg64_field32_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval64,
                              SHARED_LIMITf, buf_rqe_queue->pool_limit);
        }
        soc_reg64_field32_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval64,
                          RESET_OFFSETf, 2);

        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG1_PRIQr(unit, idx, rval));
        SOC_IF_ERROR_RETURN(soc_reg64_set(unit, MMU_THDR_DB_CONFIG_PRIQr, REG_PORT_ANY, idx, rval64));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_LIMIT_COLOR_PRIQr(unit,
                                                                idx, rval3));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_REDf,
                          (default_mtu_cells * 2)/8);
        soc_reg_field_set(unit, MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_YELLOWf,
                          (default_mtu_cells * 2)/8);
        SOC_IF_ERROR_RETURN(
                  WRITE_MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr(unit, idx, rval));

        /* queue entry */
        buf_pool = &buf->pools[buf_rqe_queue->pool_idx];
        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_MIN_PRIQr,
                          &rval, MIN_LIMITf,
                          sal_ceil_func(buf_rqe_queue->guarantee, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_LIMIT_MIN_PRIQr(unit, idx, rval));

        rval = 0;
        rval2 = 0;
        rval3 = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                          &rval, SPIDf, buf_rqe_queue->pool_idx);

        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                          &rval, COLOR_LIMIT_DYNAMICf, (!lossless));
        soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                          &rval3, SHARED_RED_LIMITf,
                          lossless? (sal_ceil_func(buf_pool->total_rqe_entry,
                          8)): 0);
        soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                          &rval3, SHARED_YELLOW_LIMITf,
                          lossless? (sal_ceil_func(buf_pool->total_rqe_entry,
                          8)): 0);

        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr, &rval,
                          LIMIT_ENABLEf, !lossless);
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                          &rval, DYNAMIC_ENABLEf, (!lossless));

        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_PRIQr, &rval2,
                          SHARED_LIMITf,
                          lossless? (sal_ceil_func(buf_pool->total_rqe_entry,
                          8)): 8);
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_PRIQr, &rval2,
                          RESET_OFFSETf, 1);

        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG1_PRIQr(unit, idx, rval));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG_PRIQr(unit, idx, rval2));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_LIMIT_COLOR_PRIQr(unit, idx, rval3));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_REDf,
                          sal_ceil_func(default_mtu_cells, 8));
        soc_reg_field_set(unit, MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_YELLOWf,
                          sal_ceil_func(default_mtu_cells, 8));
        SOC_IF_ERROR_RETURN(
                  WRITE_MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr(unit, idx, rval));

    }

    /* per pool RQE settings */
    for (idx = 0; idx < _MV2_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if (((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) ||
            (buf_pool->total == 0)) {
            continue;
        }

        limit = swcfg->egr_shared_total;

        COMPILER_64_SET(rval64, 0, 0);
        soc_reg64_field32_set(unit, MMU_THDR_DB_CONFIG_SPr,
                              &rval64, SHARED_LIMITf, limit);
        soc_reg64_field32_set(unit, MMU_THDR_DB_CONFIG_SPr, &rval64,
            RESUME_LIMITf, sal_ceil_func((limit - (default_mtu_cells * 2)), 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG_SPr(unit, idx, rval64));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_SP_SHARED_LIMITr, &rval,
                          SHARED_RED_LIMITf,
                          sal_ceil_func(limit, 8));
        soc_reg_field_set(unit, MMU_THDR_DB_SP_SHARED_LIMITr, &rval,
                          SHARED_YELLOW_LIMITf,
                          sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_SP_SHARED_LIMITr(unit, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_RED_LIMITf,
                          sal_ceil_func((limit - (default_mtu_cells * 2)), 8));
        soc_reg_field_set(unit, MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_YELLOW_LIMITf,
                          sal_ceil_func((limit - (default_mtu_cells * 2)), 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr(unit,
                                                                     idx, rval));

        rqlen = sal_ceil_func(buf_pool->total_rqe_entry, 8);
        if (rqlen == 0) {
            continue;
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_SPr, &rval,
                          SHARED_LIMITf, rqlen);
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_SPr, &rval,
                          RESUME_LIMITf, rqlen - 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG_SPr(unit, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr, &rval,
                          SHARED_RED_LIMITf, rqlen);
        soc_reg_field_set(unit, MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr, &rval,
                          SHARED_YELLOW_LIMITf, rqlen);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr(unit,
                                                                     idx,
                                                                     rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_RED_LIMITf, rqlen - 1);
        soc_reg_field_set(unit, MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_YELLOW_LIMITf, rqlen - 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr(unit,
                                                                     idx, rval));
    }


    return SOC_E_NONE;
}

STATIC int
_soc_mv2_mmu_config_buf_set_hw(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless,
                               _soc_mv2_mmu_sw_config_info_t *swcfg)
{
    int port;

    SOC_IF_ERROR_RETURN
        (_soc_mv2_mmu_config_buf_set_hw_global(unit, buf, devcfg, lossless,
                                               swcfg));

    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (_soc_mv2_mmu_config_buf_set_hw_port(unit, port, buf, devcfg,
                                                 lossless, swcfg));
    }

    return SOC_E_NONE;
}

int
soc_mv2_mmu_config_init_port(int unit, int port)
{
    int rv;
    int lossless = 1;
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_device_info_t devcfg;
    _soc_mv2_mmu_sw_config_info_t swcfg;

    buf = soc_mmu_cfg_alloc(unit);
    if (!buf) {
        return SOC_E_MEMORY;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    _soc_mv2_mmu_init_dev_config(unit, &devcfg, lossless);
    _soc_mv2_mmu_sw_info_config (unit, &swcfg, lossless);


    /* Calculate the buf - global as well as per port
     * but _soc_mv2_mmu_config_buf_set_hw_port is only called
     * for that port - since it is flex operation - we don't
     * touch any other port */
    _soc_mv2_mmu_config_buf_phase1(unit, buf, &devcfg, &swcfg, lossless);

    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        /* Override default config */
        _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
    }
    rv = _soc_mmu_cfg_buf_check(unit, buf, &devcfg);
    if (SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU config: Use default setting\n")));
        _soc_mv2_mmu_config_buf_phase1(unit, buf, &devcfg, &swcfg, lossless);
        SOC_IF_ERROR_RETURN
            (_soc_mmu_cfg_buf_calculate(unit, buf, &devcfg));
    }

    _soc_mv2_mmu_config_buf_phase2(unit, buf, &devcfg, &swcfg, lossless);
    /* Override default phase 2 config */
    if (SOC_SUCCESS(rv)) {
        if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
            _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
        }
    }
    rv = _soc_mv2_mmu_config_buf_set_hw_port(unit, port, buf, &devcfg,
                                             lossless, &swcfg);

    soc_mmu_cfg_free(unit, buf);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));
    return rv;
}

STATIC int
soc_mv2_mmu_config_init(int unit, int test_only)
{
    int rv;
    int lossless = 1;
    _soc_mmu_cfg_buf_t *buf = NULL;
    _soc_mmu_cfg_buf_t *buf_canned = NULL;
    _soc_mmu_cfg_buf_t *buf_user = NULL;
    _soc_mmu_device_info_t devcfg;
    _soc_mv2_mmu_sw_config_info_t swcfg;

    buf_canned = soc_mmu_cfg_alloc(unit);
    if (!buf_canned) {
        return SOC_E_MEMORY;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    SOC_INFO(unit).mmu_lossless = lossless;
    _soc_mv2_mmu_init_dev_config(unit, &devcfg, lossless);
    _soc_mv2_mmu_sw_info_config (unit, &swcfg, lossless);

    _soc_mv2_mmu_config_buf_phase1(unit, buf_canned, &devcfg, &swcfg, lossless);

    /* coverity[CHECKED_RETURN: FALSE] */
    rv  =_soc_mmu_cfg_buf_check(unit, buf_canned, &devcfg);
    _soc_mv2_mmu_config_buf_phase2(unit, buf_canned, &devcfg, &swcfg, lossless);
    buf = buf_canned;

    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        buf_user = soc_mmu_cfg_alloc(unit);
        if (!buf_user) {
            soc_mmu_cfg_free(unit, buf_canned);
            return SOC_E_MEMORY;
        }

        sal_memcpy(buf_user, buf_canned, sizeof(_soc_mmu_cfg_buf_t));

        /* Pickup user-defined parameters */
        _soc_mmu_cfg_buf_read(unit, buf_user, &devcfg);

        /* Validate the new parameters */
        rv = _soc_mmu_cfg_buf_check(unit, buf_user, &devcfg);
        if (SOC_SUCCESS(rv)) {
            rv = _soc_mmu_cfg_buf_calculate(unit, buf_user, &devcfg);
        }
        if (SOC_SUCCESS(rv)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU config: Using custom settings\n")));
            buf = buf_user;
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU config: Bad MMU settings.  Using defaults\n")));
        }
    }

    rv = _soc_mv2_mmu_config_buf_set_hw(unit, buf, &devcfg, lossless, &swcfg);

    soc_mmu_cfg_free(unit, buf_canned);
    if (buf_user != NULL) {
        soc_mmu_cfg_free(unit, buf_user);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));
    return rv;
}

STATIC int
_soc_mv2_thdo_hw_set(int unit, soc_port_t port, int enable)
{
    uint64 rval64, rval64_tmp;
    int pipe, i;
    int split, pos;
    soc_reg_t reg[3][2] = {
        {
            THDU_OUTPUT_PORT_RX_ENABLE_SPLIT0r,
            THDU_OUTPUT_PORT_RX_ENABLE_SPLIT1r
        },
        {
            MMU_THDM_DB_PORT_RX_ENABLE_64_SPLIT0r,
            MMU_THDM_DB_PORT_RX_ENABLE_64_SPLIT1r
        },
        {
            MMU_THDM_MCQE_PORT_RX_ENABLE_64_SPLIT0r,
            MMU_THDM_MCQE_PORT_RX_ENABLE_64_SPLIT1r
        }
    };

    SOC_IF_ERROR_RETURN(
        soc_td3_mmu_bmp_reg_pos_get(unit, port, &pipe, &split, &pos));

    for (i = 0; i < 3; i++) {
        COMPILER_64_ZERO(rval64);
        COMPILER_64_ZERO(rval64_tmp);

        if (pos < 32) {
            COMPILER_64_SET(rval64_tmp, 0, (1 << pos));
        } else {
            COMPILER_64_SET(rval64_tmp, (1 << (pos - 32)), 0);
        }

        SOC_IF_ERROR_RETURN
            (soc_trident3_xpe_reg_get(unit, reg[i][split],
                                      -1, pipe, 0, &rval64));

        if (enable) {
            COMPILER_64_OR(rval64, rval64_tmp);
        } else {
            COMPILER_64_NOT(rval64_tmp);
            COMPILER_64_AND(rval64, rval64_tmp);
        }

        SOC_IF_ERROR_RETURN
            (soc_trident3_xpe_reg_set(unit, reg[i][split],
                                      -1, pipe, 0, rval64));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_maverick2_mmu_init(int unit)
{
    int port;
    int enable = 1;
    int test_only;
    uint32 rval = 0;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
    } else
#endif
    {
        test_only = (SAL_BOOT_XGSSIM) ? TRUE : FALSE;
        SOC_IF_ERROR_RETURN(soc_mv2_mmu_config_init(unit, test_only));

        PBMP_ALL_ITER(unit, port) {
            _soc_mv2_thdo_hw_set(unit, port, enable);
        }

    }

    /* enable WRED refresh */
    SOC_IF_ERROR_RETURN(READ_WRED_REFRESH_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, WRED_REFRESH_CONTROLr, &rval,
            REFRESH_DISABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_WRED_REFRESH_CONTROLr(unit, rval));

    SOC_IF_ERROR_RETURN(_soc_mv2_post_mmu_init_update(unit));
    return SOC_E_NONE;
}

void
soc_maverick2_sbus_ring_map_config(int unit)
{
    /*
     * SBUS ring and block number:
     * ring 0: IP(1), LBPORT0(54), LBPORT1(51), LBPORT2(52), LBPORT3(53)
     * ring 1: EP(2)
     * ring 2: MMU_XPE(3), MMU_SC(4), MMU_GLB(5)
     * ring 3: PM7(22), PM6(21), PM5(20), PM4(19), PM3(18), PM2(17), PM1(16),
     *         PM0(15), PM31(46), PM30(45), PM29(44), PM28(43), PM27(42),
     *         PM26(41), PM25(40), PM24(39), CLPORT32(55)
     * ring 4: PM32(11), PM8(23), PM9(24), PM10(25), PM11(26), PM12(27),
     *         PM13(28), PM14(29), PM15(30), PM16(31), PM17(32), PM18(33),
     *         PM19(34), PM20(35), PM21(36), PM22(37), PM23(38)
     * ring 5: OTPC(6), AVS(59), TOP(7), SER(8)
     */

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_0_7_OFFSET,0x52222100);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_8_15_OFFSET,0x30053005);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_16_23_OFFSET,0x33333333);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_24_31_OFFSET,0x44444443);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_32_39_OFFSET,0x00000444);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_40_47_OFFSET,0x00000005);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_48_55_OFFSET,0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_56_63_OFFSET,0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_TIMEOUT_OFFSET,0x5000);
    }
#endif

    return;
}


/*
 * Function:
 *      soc_mv2_port_schedule_tdm_init
 * Purpose:
 *      Initialize TDM information in port_schedule_state that will be passed
 *      to DV FlexPort API
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (OUT) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      If called during Scheduler Initialization, only below values need to be
 *      properly set:
 *        soc_tdm_schedule_pipe_t::num_slices
 *        soc_tdm_schedule_t:: cal_len
 *        soc_tdm_schedule_t:: num_ovs_groups
 *        soc_tdm_schedule_t:: ovs_group_len
 *        soc_tdm_schedule_t:: num_pkt_sch_or_ovs_space
 *        soc_tdm_schedule_t:: pkt_sch_or_ovs_space_len
 */
int
soc_mv2_port_schedule_tdm_init(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    _soc_maverick2_tdm_temp_t *tdm = soc->tdm_info;
    /*soc_info_t *si = &SOC_INFO(unit);*/
    soc_tdm_schedule_pipe_t *tdm_ischd, *tdm_eschd;
    soc_tdm_schedule_t *sched;
    int pipe, hpipe, phy_port, is_flexport, group, slot;
    int *port_p2PBLK_inst_mapping;

    if (tdm == NULL) {
        return SOC_E_INIT;
    }

    is_flexport = port_schedule_state->is_flexport;
	/*
    port_schedule_state->calendar_type = si->fabric_port_enable ?
                                        _MV2_TDM_CALENDAR_UNIVERSAL :
                                        _MV2_TDM_CALENDAR_ETHERNET_OPTIMIZED;
	*/
    /* Construct tdm_ingress_schedule_pipe and tdm_egress_schedule_pipe */
    for (pipe = 0; pipe <  MAVERICK2_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        tdm_eschd = &port_schedule_state->tdm_egress_schedule_pipe[pipe];

        tdm_ischd->num_slices = MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE;
        tdm_eschd->num_slices = MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE;

        if (is_flexport) {
            /* TDM Calendar is always in slice 0 */
            sal_memcpy(tdm_ischd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].idb_tdm,
                        sizeof(int)*_MV2_TDM_LENGTH);
            sal_memcpy(tdm_eschd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].mmu_tdm,
                        sizeof(int)*_MV2_TDM_LENGTH);
        }

        /* OVS */
        for (hpipe = 0; hpipe < MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            /* IDB OVERSUB*/
            sched = &tdm_ischd->tdm_schedule_slice[hpipe];
            sched->cal_len = _MV2_TDM_LENGTH;
            sched->num_ovs_groups = MAVERICK2_TDM_OVS_GROUPS_PER_HPIPE;
            sched->ovs_group_len = MAVERICK2_TDM_OVS_GROUP_LENGTH;
            sched->num_pkt_sch_or_ovs_space = 1;
            sched->pkt_sch_or_ovs_space_len = _MV2_PKT_SCH_LENGTH;

            if (is_flexport) {
                for (group = 0; group < MAVERICK2_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                    for (slot = 0; slot < MAVERICK2_TDM_OVS_GROUP_LENGTH; slot++) {
                        sched->oversub_schedule[group][slot] =
                            tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot];
                    }
                }
                for (slot = 0; slot < _MV2_PKT_SCH_LENGTH; slot++) {
                    sched->pkt_sch_or_ovs_space[0][slot] =
                        tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot];
                }
            }

            /* MMU OVERSUB */
            sched = &tdm_eschd->tdm_schedule_slice[hpipe];
            sched->cal_len = _MV2_TDM_LENGTH;
            sched->num_ovs_groups = MAVERICK2_TDM_OVS_GROUPS_PER_HPIPE;
            sched->ovs_group_len = MAVERICK2_TDM_OVS_GROUP_LENGTH;
            sched->num_pkt_sch_or_ovs_space = 1;
            sched->pkt_sch_or_ovs_space_len = _MV2_PKT_SCH_LENGTH;

            if (is_flexport) {
                for (group = 0; group < MAVERICK2_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                    for (slot = 0; slot < MAVERICK2_TDM_OVS_GROUP_LENGTH; slot++) {
                        sched->oversub_schedule[group][slot] =
                            tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot];
                        }
                    }
                for (slot = 0; slot < _MV2_PKT_SCH_LENGTH; slot++) {
                    sched->pkt_sch_or_ovs_space[0][slot] =
                        tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot];
                }
            }
        }
    }

    if (is_flexport) {
        port_p2PBLK_inst_mapping =
            port_schedule_state->in_port_map.port_p2PBLK_inst_mapping;
        /* pblk info for phy_port */
        for (phy_port = 1; phy_port < MV2_NUM_EXT_PORTS; phy_port++) {
            if (tdm->pblk_info[phy_port].pblk_cal_idx == -1) {
                continue;
            }
            port_p2PBLK_inst_mapping[phy_port] =
                                        tdm->pblk_info[phy_port].pblk_cal_idx;
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_mv2_soc_tdm_update
 * Purpose:
 *      Update TDM information in SOC with TDM state return from DV FlexPort API
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (OUT) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
soc_mv2_soc_tdm_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_info_t *si = &SOC_INFO(unit);
    _soc_maverick2_tdm_temp_t *tdm = soc->tdm_info;
    soc_tdm_schedule_pipe_t *tdm_ischd, *tdm_eschd;
    soc_tdm_schedule_t *sched;
    int pipe, hpipe, port, phy_port, clport, slot;
    int length, ovs_core_slot_count, ovs_io_slot_count, port_slot_count;
    int index, group;

    if (tdm == NULL) {
        return SOC_E_INIT;
    }

    /* Copy info from soc_port_schedule_state_t to _soc_maverick2_tdm_temp_t */
    for (pipe = 0; pipe <  MAVERICK2_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        tdm_eschd = &port_schedule_state->tdm_egress_schedule_pipe[pipe];

        /* TDM Calendar is always in slice 0 */
        sal_memcpy(tdm->tdm_pipe[pipe].idb_tdm,
                    tdm_ischd->tdm_schedule_slice[0].linerate_schedule,
                    sizeof(int)*_MV2_TDM_LENGTH);
        sal_memcpy(tdm->tdm_pipe[pipe].mmu_tdm,
                    tdm_eschd->tdm_schedule_slice[0].linerate_schedule,
                    sizeof(int)*_MV2_TDM_LENGTH);

        for (hpipe = 0; hpipe < MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            sched = &tdm_ischd->tdm_schedule_slice[hpipe];
            for (group = 0; group < MAVERICK2_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                for (slot = 0; slot < MAVERICK2_TDM_OVS_GROUP_LENGTH; slot++) {
                    tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot] =
                        sched->oversub_schedule[group][slot];
                }
            }
            for (slot = 0; slot < _MV2_PKT_SCH_LENGTH; slot++) {
                tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot] =
                    sched->pkt_sch_or_ovs_space[0][slot];

            }
        }
    }

    /* pblk info init  */
    for (phy_port = 1; phy_port < MV2_NUM_EXT_PORTS; phy_port++) {
        tdm->pblk_info[phy_port].pblk_cal_idx = -1;
        tdm->pblk_info[phy_port].pblk_hpipe_num = -1;
    }

    /* pblk info for phy_port */
    for (pipe = 0; pipe < MAVERICK2_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        for (hpipe = 0; hpipe < MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            for (group = 0; group < MAVERICK2_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                for (index = 0; index < MAVERICK2_TDM_OVS_GROUP_LENGTH; index++) {
                    phy_port =
                        tdm_ischd->tdm_schedule_slice[hpipe].oversub_schedule[group][index];
                    if (phy_port < MV2_NUM_EXT_PORTS) {
                        tdm->pblk_info[phy_port].pblk_hpipe_num = hpipe;
                        tdm->pblk_info[phy_port].pblk_cal_idx =
                            port_schedule_state->out_port_map.port_p2PBLK_inst_mapping[phy_port];
                    }
                }
            }
        }
    }

    /* CLPORT port ratio */
    for (clport = 0; clport < MAVERICK2_TDM_PBLKS_PER_DEV; clport++) {
        soc_mv2_tdm_get_port_ratio(unit, port_schedule_state, clport,
            &tdm->port_ratio[clport], 1);
    }

    /* Calculate the oversub ratio */
    for (pipe = 0; pipe < MAVERICK2_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        /* Count number of OVSB_TOKEN assigned by the TDM code */
        for (length = _MV2_TDM_LENGTH; length > 0; length--) {
            if (tdm_ischd->tdm_schedule_slice[0].linerate_schedule[length - 1]
                    != MV2_NUM_EXT_PORTS) {
                break;
            }
        }
        ovs_core_slot_count = 0;
        for (index = 0; index < length; index++) {
            if (tdm_ischd->tdm_schedule_slice[0].linerate_schedule[index]
                    == MV2_OVSB_TOKEN) {
                ovs_core_slot_count++;
            }
        }

        /* Count number of slot needed for half-pipe's oversub I/O bandwidth */
        for (hpipe = 0; hpipe < MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            ovs_io_slot_count = 0;
            for (group = 0; group < MAVERICK2_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                for (index = 0; index < MAVERICK2_TDM_OVS_GROUP_LENGTH; index++) {
                    phy_port = tdm_ischd->tdm_schedule_slice[hpipe].oversub_schedule[group][index];
                    if (phy_port == MV2_NUM_EXT_PORTS) {
                        continue;
                    }
                    port = si->port_p2l_mapping[phy_port];
					
                    if ((port == -1) || IS_CPU_PORT(unit, port) ||
                        IS_LB_PORT(unit, port) ||
                        IS_MGMT_PORT(unit, port)) {
                        LOG_ERROR(BSL_LS_SOC_PORT,
                                  (BSL_META_U(unit,
                                              "Flexport: Invalid physical "
                                              "port %d in OverSub table.\n"),
                                   phy_port));
                        continue;
                    }
                    port_slot_count = si->port_speed_max[port] / 2500;
                    ovs_io_slot_count += port_slot_count;
                }
            }
            if (ovs_core_slot_count != 0) {
                tdm->ovs_ratio_x1000[pipe][hpipe] =
                    ovs_io_slot_count * 1000 / (ovs_core_slot_count / 2);
            }
        }
    }

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define SOC_FLEXPORT_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_FLEXPORT_WB_DEFAULT_VERSION            SOC_FLEXPORT_WB_VERSION_1_0

int
soc_mv2_flexport_scache_allocate(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int stable_size;
    int default_ver = SOC_FLEXPORT_WB_DEFAULT_VERSION;

    alloc_size =  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* phy to logical*/
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* phy to mmu */
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* logical to phy */
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* max port speed */
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* init port speed */
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* num of lanes */
                  (sizeof(pbmp_t))                       + /* HG bitmap */
                  (sizeof(pbmp_t));                        /* Disabled bitmap */

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

/*
 * Function:
 *      soc_mv2_flexport_scache_sync
 * Purpose:
 *      Record Port info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
 *    port_speed_max
 *    port_init_speed
 *    port_num_lanes
 *    disabled_bitmap
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_mv2_flexport_scache_sync(int unit)
{
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    uint32 var_size = 0;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 scache_offset=0;
    int rv = 0;

    alloc_size =  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* phy to logical*/
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* phy to mmu */
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* logical to phy */
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* max port speed */
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* init port speed */
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* num of lanes */
                  (sizeof(pbmp_t))                       + /* HG bitmap */
                  (sizeof(pbmp_t));                        /* Disabled bitmap */

    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_get,
                        &flexport_scache_ptr,
                        SOC_FLEXPORT_WB_DEFAULT_VERSION,
                        NULL);
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
    var_size = sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV;
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2l_mapping, var_size);
    scache_offset += var_size;


    /* Physical to MMU port mapping */
    var_size = sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV;
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2m_mapping, var_size);
    scache_offset += var_size;

    /* Logical to Physical port mapping */
    var_size = sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV;
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_l2p_mapping, var_size);
    scache_offset += var_size;

    /* Max port speed */
    var_size = sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV;

    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_speed_max, var_size);
    scache_offset += var_size;

    /* Init port speed */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_init_speed, var_size);
    scache_offset += var_size;

    /* Num of lanes */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_num_lanes, var_size);
    scache_offset += var_size;

    /* HG Bitmap */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               &si->hg.bitmap,
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

/*
 * Function:
 *      soc_mv2_flexport_scache_recovery
 * Purpose:
 *      Recover Port info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
 *    port_p2l_mapping
 *    port_l2p_mapping
 *    port_speed_max
 *    port_init_speed
 *    port_num_lanes
 *    disabled_bitmap
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_mv2_flexport_scache_recovery(int unit)
{
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int rv = SOC_E_NONE;
    uint8 *flexport_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    uint32 scache_offset=0;
    uint32 var_size = 0;
    uint16 recovered_ver = 0;
    soc_info_t *si = &SOC_INFO(unit);

    alloc_size =  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* phy to logical*/
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* phy to mmu */
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* logical to phy */
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* max port speed */
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* init port speed */
                  (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV) + /* num of lanes */
                  (sizeof(pbmp_t))                         + /* HG bitmap */
                  (sizeof(pbmp_t));                          /* Disabled bitmap */

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
    var_size = (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV);
    sal_memcpy(si->port_p2l_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /*Physical to MMU port mapping */
    var_size = (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV);
    sal_memcpy(si->port_p2m_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /*Logical to Physical port mapping*/
    var_size = (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV);
    sal_memcpy(si->port_l2p_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Max port speed */
    var_size = (sizeof(int) * MAVERICK2_PHY_PORTS_PER_DEV);

    sal_memcpy(_soc_mv2_port_speed_cap[unit],
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Init port speed */
    sal_memcpy(si->port_speed_max,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Num of lanes */
    sal_memcpy(si->port_num_lanes,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* HG Bitmap */
    sal_memcpy(&si->hg.bitmap,
           &flexport_scache_ptr[scache_offset],
           sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    /* Disabled Port Bitmap */
    sal_memcpy(&si->all.disabled_bitmap,
           &flexport_scache_ptr[scache_offset],
           sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    SOC_PBMP_CLEAR(si->pipe_pbm[0]);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

#define SOC_TDM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_TDM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define SOC_TDM_WB_DEFAULT_VERSION            SOC_TDM_WB_VERSION_1_1

int soc_mv2_tdm_scache_allocate(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *tdm_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int stable_size;
    int default_ver = SOC_TDM_WB_DEFAULT_VERSION;
    int ilen, ovs_size, pkt_shp_size, hpipes;

    ilen = sizeof(int);
    ovs_size = MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE *
               MAVERICK2_OVS_GROUP_COUNT_PER_HPIPE *
               MAVERICK2_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE *
                   _MV2_PKT_SCH_LENGTH;
    hpipes = MAVERICK2_PIPES_PER_DEV * MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * MAVERICK2_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * MAVERICK2_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * MAVERICK2_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * MAVERICK2_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * MAVERICK2_PBLKS_PER_DEV)     + /* port ratio */
                 (ilen * hpipes);                  /* oversub ratio */
    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_TDM_HANDLE, 0);
    /* check to see if an scache table has been configured */
    rv = soc_stable_size_get(unit, &stable_size);
    if (SOC_FAILURE(rv) || stable_size <= 0) {
        return rv;
    }

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      TRUE, &alloc_get,
                                      &tdm_scache_ptr,
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
                  "ERROR: scache memory for tdm size mismatch"
                  "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }

    if (NULL == tdm_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: scache memory not allocated for tdm"
                  "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_mv2_tdm_scache_sync
 * Purpose:
 *      Record TDM info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
 *    IDB TDM info on pipe0
 *    MMU TDM info on pipe0
 *    Oversub group info on pipe0
 *    pkt shaper info on pipe0
 *    pblk id of phy-port
 *    port ratio
 *    oversub ratio
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_mv2_tdm_scache_sync(int unit)
{
    uint8 *tdm_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    uint32 var_size = 0;
    uint32 scache_offset=0;
    int rv = 0;
    int ilen, ovs_size, pkt_shp_size, hpipes, phy_port;
    _soc_maverick2_tdm_temp_t  *tdm = SOC_CONTROL(unit)->tdm_info;

    ilen = sizeof(int);
    ovs_size = MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE *
               MAVERICK2_OVS_GROUP_COUNT_PER_HPIPE *
               MAVERICK2_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE *
                   _MV2_PKT_SCH_LENGTH;
    hpipes = MAVERICK2_PIPES_PER_DEV * MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * MAVERICK2_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * MAVERICK2_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * MAVERICK2_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * MAVERICK2_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * MAVERICK2_PBLKS_PER_DEV)     + /* port ratio */
                 (ilen * hpipes);                  /* oversub ratio */
    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_TDM_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      FALSE, &alloc_get,
                                      &tdm_scache_ptr,
                                      SOC_TDM_WB_DEFAULT_VERSION,
                                      NULL);
    if (rv == SOC_E_NOT_FOUND) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: scache memory for tdm size mismatch"
                  "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }
    if (NULL == tdm_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: scache memory not allocated for tdm"
                  "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /* IDB TDM info on pipe0 */
    var_size = ilen * MAVERICK2_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[0].idb_tdm,
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe0 */
    var_size = ilen * MAVERICK2_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[0].mmu_tdm,
               var_size);
    scache_offset += var_size;

    /* Oversub group info on pipe0 */
    var_size = ilen * ovs_size;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[0].ovs_tdm,
               var_size);
    scache_offset += var_size;

    /* pkt shaper info on pipe0 */
    var_size = ilen * pkt_shp_size;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[0].pkt_shaper_tdm,
               var_size);
    scache_offset += var_size;

    /* hpipe id of phy-port */
    for (phy_port = 0; phy_port < MAVERICK2_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm_scache_ptr[scache_offset],
                   &tdm->pblk_info[phy_port].pblk_hpipe_num,
                   var_size);
        scache_offset += var_size;
    }

    /* pblk id of phy-port */
    for (phy_port = 0; phy_port < MAVERICK2_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm_scache_ptr[scache_offset],
                   &tdm->pblk_info[phy_port].pblk_cal_idx,
                   var_size);
        scache_offset += var_size;
    }

    /* port ratio */
    var_size = ilen * MAVERICK2_PBLKS_PER_DEV;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->port_ratio,
               var_size);
    scache_offset += var_size;

    /* oversub ratio */
    var_size = ilen * hpipes;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->ovs_ratio_x1000,
               var_size);
    scache_offset += var_size;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_mv2_tdm_scache_recovery
 * Purpose:
 *      Recover TDM info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
 *    IDB TDM info on pipe0
 *    MMU TDM info on pipe0
 *    Oversub group info on pipe0
 *    pkt shaper info on pipe0
 *    hpipe id of phy-port
 *    pblk id of phy-port
 *    port ratio
 *    oversub ratio
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_mv2_tdm_scache_recovery(int unit)
{
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int rv = SOC_E_NONE;
    uint8 *tdm_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    uint32 scache_offset=0;
    uint32 var_size = 0;
    uint16 recovered_ver = 0;
    int additional_scache_sz = 0;
    int ilen, ovs_size, pkt_shp_size, hpipes, phy_port;
    _soc_maverick2_tdm_temp_t *tdm = SOC_CONTROL(unit)->tdm_info;

    ilen = sizeof(int);
    ovs_size = MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE *
               MAVERICK2_OVS_GROUP_COUNT_PER_HPIPE *
               MAVERICK2_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE *
                   _MV2_PKT_SCH_LENGTH;
    hpipes = MAVERICK2_PIPES_PER_DEV * MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * MAVERICK2_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * MAVERICK2_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * MAVERICK2_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * MAVERICK2_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * MAVERICK2_PBLKS_PER_DEV)     + /* port ratio */
                 (ilen * hpipes);                  /* oversub ratio */
    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_TDM_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      FALSE, &alloc_get,
                                      &tdm_scache_ptr,
                                      SOC_TDM_WB_DEFAULT_VERSION,
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
                  "ERROR: scache recovery for tdm"
                  "%s()[LINE:%d] DONE \n"),
                  FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }

    if (NULL == tdm_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: scache recovery for tdm"
                  "%s()[LINE:%d] DONE \n"),
                  FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /* IDB TDM info on pipe0 */
    var_size = ilen * MAVERICK2_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[0].idb_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe0 */
    var_size = ilen * MAVERICK2_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[0].mmu_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Oversub group info on pipe0 */
    var_size = ilen * ovs_size;
    sal_memcpy(tdm->tdm_pipe[0].ovs_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* pkt shaper info on pipe0 */
    if (recovered_ver < SOC_TDM_WB_VERSION_1_1) {
        /* Pre 6.5.15, the value of _MV2_PKT_SCH_LENGTH was defined as 160.
         * After this was corrected and fixed to 200, the corresponding increase
         * in array size after an upgrade to 6.5.15 needs to be handled.
         * Hence keeping it as 160 when we sync from an older WB version
         * and also taking care of additional bytes allocation */
        pkt_shp_size = MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE * 160;
        additional_scache_sz = ilen * MAVERICK2_OVS_HPIPE_COUNT_PER_PIPE * 40 ;
    }
    var_size = ilen * pkt_shp_size;
    sal_memcpy(tdm->tdm_pipe[0].pkt_shaper_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* hpipe id of phy-port */
    for (phy_port = 0; phy_port < MAVERICK2_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm->pblk_info[phy_port].pblk_hpipe_num,
                   &tdm_scache_ptr[scache_offset],
                   var_size);
        scache_offset += var_size;
    }

    /* pblk id of phy-port */
    for (phy_port = 0; phy_port < MAVERICK2_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm->pblk_info[phy_port].pblk_cal_idx,
                   &tdm_scache_ptr[scache_offset],
                   var_size);
        scache_offset += var_size;
    }

    /* port ratio */
    var_size = ilen * MAVERICK2_PBLKS_PER_DEV;
    sal_memcpy(tdm->port_ratio,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* oversub ratio */
    var_size = ilen * hpipes;
    sal_memcpy(tdm->ovs_ratio_x1000,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    if (additional_scache_sz > 0) {
        SOC_IF_ERROR_RETURN(soc_scache_realloc(unit,
                    scache_handle, additional_scache_sz));
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));


    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Maverick2 chip driver functions.
 * Pls keep at the end of this file for easy access.
 */
soc_functions_t soc_maverick2_drv_funs = {
    _soc_maverick2_misc_init,
    _soc_maverick2_mmu_init,
    soc_trident3_age_timer_get,
    soc_trident3_age_timer_max_get,
    soc_trident3_age_timer_set,
    soc_trident3_tscx_firmware_set,
    _soc_maverick2_tscx_reg_read,
    _soc_maverick2_tscx_reg_write,
    soc_maverick2_bond_info_init,
};
#endif /* BCM_MAVERICK2_SUPPORT */
