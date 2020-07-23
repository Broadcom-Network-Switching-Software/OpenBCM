/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk2.c
 * Purpose:
 * Requires:
 */
#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>

#include <soc/defs.h>
#include <soc/mem.h>
#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#include <soc/tomahawk.h>
#include <soc/tomahawk2_tdm.h>
#include <soc/pstats.h>
#include <soc/bondoptions.h>
#include <soc/scache.h>

#ifndef MIN
#define MIN(a, b)               (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a, b)               (((a)>(b))?(a):(b))
#endif

#define CEIL(x, y)              (((x) + ((y)-1)) / (y))

#define TH2_MMU_SHARED_LIMIT_CHK(val1, val2, flags) \
    (((flags) && (val1 > val2)) || ((!flags) && (val1 < val2)))

int
_soc_th2_port_speed_cap[SOC_MAX_NUM_DEVICES][_TH2_DEV_PORTS_PER_DEV];

int
soc_th2_post_mmu_init_update(int unit)
{
    int port;
    soc_info_t *si = &SOC_INFO(unit);

    PBMP_ALL_ITER(unit, port) {
        /* Set init speed to max speed by default */
        si->port_init_speed[port] = si->port_speed_max[port];
        if (_soc_th2_port_speed_cap[unit][port]) {
            /* If cap speed is available then adjust max speed for further use */
            si->port_speed_max[port] = _soc_th2_port_speed_cap[unit][port];
        }
    }
    return SOC_E_NONE;
}

STATIC int
soc_tomahawk2_frequency_get(int unit, int *max_freq, int *def_freq)
{
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        *max_freq = SOC_BOND_INFO(unit)->max_core_freq;
        *def_freq = SOC_BOND_INFO(unit)->default_core_freq;
    } else {
        *max_freq = *def_freq = 1700;
    }

    return SOC_E_NONE;
}

STATIC int
soc_tomahawk2_tsc_max_lane_speed_get(int unit, int tsc_id, int *speed)
{
    SHR_BITDCL otp;

    memset(&otp, 0, sizeof(otp));

    /*
     * Each TSC has two bit.
     * 00 = no restriction
     * 01 = 21.875G
     * 10 = 10.9375G
     * 11 = reserved
     */
    SHR_BITCOPY_RANGE(&otp, 0, SOC_BOND_INFO(unit)->tsc_max_speed, tsc_id * 2, 2);

    switch(otp) {
        case 0:
            *speed = 27000; /* No restriction*/
            break;
        case 1:
            *speed = 21000; /* 21.875G */
            break;
        case 2:
            *speed = 11000; /* 10.9375G */
            break;
        default:
            return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

int
soc_tomahawk2_tsc_otp_info_get(int unit, int tsc_id,
                               _soc_tomahawk2_tsc_otp_info_t *info)
{

    if (soc_feature(unit, soc_feature_untethered_otp)) {
        info->disable =
            SHR_BITGET(SOC_BOND_INFO(unit)->tsc_disabled, tsc_id) ? 1 : 0;
        info->force_hg =
            SHR_BITGET(SOC_BOND_INFO(unit)->force_hg, tsc_id) ? 1 : 0;
        info->force_loopback =
            SHR_BITGET(SOC_BOND_INFO(unit)->tsc_in_loop, tsc_id) ? 1 : 0;
        SOC_IF_ERROR_RETURN(
            soc_tomahawk2_tsc_max_lane_speed_get(unit, tsc_id,
                                                 &info->max_lane_speed));
    } else {
        info->disable = 0;
        info->force_hg = 0;
        info->force_loopback = 0;
        info->max_lane_speed = 27000; /* No restriction*/
    }

    return SOC_E_NONE;
}

STATIC int
_soc_th2_ports_per_pm_get(int unit, int pm_id)
{
    soc_info_t *si;
    int phy_port, i, num_ports = 0;

    si = &SOC_INFO(unit);

    if (pm_id >= _TH2_PBLKS_PER_DEV) {
       return 0;
    }

    phy_port = 1 + (pm_id * _TH2_PORTS_PER_PBLK);
    for (i = 0; i < _TH2_PORTS_PER_PBLK; ) {
        if (si->port_p2l_mapping[phy_port + i] != -1) {
            num_ports ++;
            i += si->port_num_lanes[si->port_p2l_mapping[phy_port + i]];
        }else {
            i ++;
        }
    }

    return num_ports;
}


STATIC int
_soc_tomahawk2_port_flex_init(int unit, int is_any_cap)
{
    soc_info_t *si;
    int pm, blk_idx, blk_type;
    int flex_en, static_ports, max_ports;

    si = &SOC_INFO(unit);
    SHR_BITCLR_RANGE(si->flexible_pm_bitmap, 0, SOC_MAX_NUM_BLKS);
    
    /* portmap_x=y:speed:i */
    /* portmap_x=y:speed:cap */
    if (SOC_PBMP_NOT_NULL(SOC_PORT_DISABLED_BITMAP(unit, all)) || is_any_cap) {
        SHR_BITSET_RANGE(si->flexible_pm_bitmap, 1, _TH2_PBLKS_PER_DEV);
    } else {
        /* port_flex_enable */
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
                        (pm * _TH2_PORTS_PER_PBLK + 1), 
                        spn_PORT_FLEX_ENABLE, 0);
                }
                if (flex_en) {
                    SHR_BITSET(si->flexible_pm_bitmap, blk_idx);
                }
            }
        }
    }

    si->flex_eligible = 
        (!SHR_BITNULL_RANGE(si->flexible_pm_bitmap, 1, _TH2_PBLKS_PER_DEV));

    /* port_flex_max_ports */
    memset(&(si->pm_max_ports), 0xff, sizeof(si->pm_max_ports));
    for (blk_idx = 0; blk_idx < SOC_MAX_NUM_BLKS; blk_idx++) {
        blk_type = SOC_BLOCK_TYPE(unit, blk_idx);
        pm = SOC_BLOCK_NUMBER(unit, blk_idx);
        if (blk_type == SOC_BLK_CLPORT) {
            static_ports = _soc_th2_ports_per_pm_get(unit, pm);
            if (SHR_BITGET(si->flexible_pm_bitmap, blk_idx)) {
                /* port_flex_max_ports_corex=y */
                max_ports = soc_property_suffix_num_get_only_suffix(unit, 
                    pm, spn_PORT_FLEX_MAX_PORTS, "core", -1);
                /* port_flex_max_ports{x}=y */
                /* port_flex_max_ports=y */
                if (max_ports == -1) {
                    max_ports = soc_property_phys_port_get(unit, 
                        (pm * _TH2_PORTS_PER_PBLK + 1), 
                        spn_PORT_FLEX_MAX_PORTS, 4);
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

static int
_soc_tomahawk2_phy_dev_port_check(int unit, int phy_port, int dev_port)
{
    int num_phy_port = 264;

    if (phy_port < 0 || phy_port >= num_phy_port) {
        return FALSE;
    } else if (phy_port >= 1 && phy_port <= 64) {
        return (dev_port >= 1 && dev_port <=32);
    } else if (phy_port >= 65 && phy_port <= 128) {
        return (dev_port >= 34 && dev_port <= 66);
    } else if (phy_port >= 129 && phy_port <= 192) {
        return (dev_port >= 68 && dev_port <= 100);
    } else if (phy_port >= 193 && phy_port <= 256) {
        return (dev_port >= 102 && dev_port <= 133);
    } else if (phy_port == _TH2_PHY_PORT_MNG0) {
        return (dev_port >= 34 && dev_port <= 66);
    } else if (phy_port == _TH2_PHY_PORT_MNG1) {
        return (dev_port >= 68 && dev_port <= 100);
    } else {
        /*skip LBPORT(260,261,262,263) , CPU port(0) and hole (258)*/
        return TRUE;
    }
}

STATIC int
_soc_tomahawk2_port_mapping_check(int unit)
{
    soc_info_t *si;
    int port, phy_port, lanes, i;

    si = &SOC_INFO(unit);

    for (phy_port = 1; phy_port < _TH2_PORTS_PER_DEV;) {
        port = si->port_p2l_mapping[phy_port];
        if (port < 0) {
            phy_port ++;
            continue;
        }
        if (port >= _TH2_DEV_PORTS_PER_DEV) {
            LOG_CLI((BSL_META_U(unit, "wrong logical port %d, phy %d\n"),
                     port, phy_port));
            return SOC_E_CONFIG;
        }
        lanes = si->port_num_lanes[port];
        if ((lanes < 0) || (lanes > 4)) {
            LOG_CLI((BSL_META_U(unit, "wrong lane number, port %d, lanes %d\n"),
                     port, lanes));
            return SOC_E_CONFIG;
        }
        for (i = 1; i < lanes; i ++) {
            if (si->port_p2l_mapping[phy_port + i] != -1) {
                LOG_CLI((BSL_META_U(unit, "port overlay, port %d(%d-%d) and "
                                          "port %d(%d..)\n"),
                         port, phy_port, phy_port + lanes - 1,
                         si->port_p2l_mapping[phy_port + i], phy_port + i));
                return SOC_E_CONFIG;
            }
        }
        phy_port += lanes;
    }

    return SOC_E_NONE;
}

/*
 * Tomahawk2 port mapping
 *
 *                   physical   idb/       device   mmu
 *                   port       idb port   port     port
 *     CMIC          0          0/32       0        32
 *     CLPORT0-15    1-64       0/0-31     1-32     0-31
 *     CLPORT16-31   65-128     1/0-31     34-66    64-95
 *     CLPORT32-47   129-192    2/0-31     68-100   128-159
 *     CLPORT48-63   193-256    3/0-31     102-133  192-223
 *     XLPORT0/0     257        1/32       66       96
 *     hole          258        3/32       134      224
 *     XLPORT0/2     259        2/32       100      160
 *     LBPORT0       260        0/33       33       33
 *     LBPORT1       261        1/33       67       97
 *     LBPORT2       262        2/33       101      161
 *     LBPORT3       263        3/33       135      225
 * Although MMU port number is flexible within the above range, it is
 *     configured as a fixed mapped to IDB port number
 */
int
soc_tomahawk2_port_config_init(int unit, uint16 dev_id)
{
    soc_info_t *si;
    char *config_str, *sub_str, *sub_str_end;
    static const char str_2p5[] = "2.5";
    char str_buf[8];
    int rv, oversub_mode, frequency, max_freq;
    int ratio_list_len, ratio_idx;
    int port, phy_port, mmu_port, idb_port, tsc_id;
    int pipe, xpe, sc, sed, index, bandwidth_cap, is_any_cap = FALSE;
    int port_bandwidth, freq_list_len, blk_idx;
    char option1, option2;
    uint32 pipe_map, xpe_map, sc_map, sed_map, ipipe_map, epipe_map;
    int latency;
    soc_pbmp_t eth_pbm, oversub_pbm;
    static const int freq_list[] = { 1700, 1625, 1525, 1425, 1325, 1275,
                                     1225, 1125, 1050, 950, 850 };
    const char *str_ratio[] = { "2:3", "1:2", "1:1" };
    static const int dpp_ratio_x10_list[] = {
        15, /* core clk : pp clk ratio is 3:2 (default) */
        20, /* core clk : pp clk ratio is 2:1 */
        10  /* core clk : pp clk ratio is 1:1 */
    };
    static const struct {
        int port;
        int phy_port;
        int pipe;
        int idb_port;
        int mmu_port;
    } fixed_ports[] = {
        { 0,   0,   0, 32, 32 },    /* cpu port */
        { 33,  260, 0, 33, 33 },    /* loopback port 0 */
        { 67,  261, 1, 33, 97 },    /* loopback port 1 */
        { 101, 262, 2, 33, 161 },   /* loopback port 2 */
        { 135, 263, 3, 33, 225 }    /* loopback port 3 */
    };

    si = &SOC_INFO(unit);


    SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency));

    SOC_PBMP_CLEAR(eth_pbm);
    eth_pbm = soc_property_get_pbmp(unit, spn_PBMP_XPORT_XE, 0);

    SOC_PBMP_CLEAR(oversub_pbm);
    oversub_mode = soc_property_get(unit, spn_OVERSUBSCRIBE_MODE, 1);
    oversub_pbm  = soc_property_get_pbmp(unit, spn_PBMP_OVERSUBSCRIBE, 0);
    oversub_mode = oversub_mode || SOC_PBMP_NOT_NULL(oversub_pbm);
    si->oversub_mode = oversub_mode;
    si->os_mixed_sister_25_50_enable = soc_property_get(
                        unit, spn_OVERSUBSCRIBE_MIXED_SISTER_25_50_ENABLE, 0);

    soc_tomahawk2_frequency_get(unit, &max_freq, &si->frequency);
    frequency = soc_property_get(unit, spn_CORE_CLOCK_FREQUENCY, -1);


    /* Core CLK frequency */
    if (frequency != -1 && frequency <= max_freq) {
        freq_list_len = COUNTOF(freq_list);
        for (index = 0; index < freq_list_len; index++) {
            if (freq_list[index] <= max_freq &&
                frequency == freq_list[index]) {
                break;
            }
        }

        if (index < freq_list_len) {
            si->frequency = frequency;
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "*** Input core clock frequency %dMHz is not "
                                "supported!\n"), frequency));
        }
    }

    /* DPP CLK ratio */
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        si->dpp_clk_ratio_x10 = SOC_BOND_INFO(unit)->dpp_clk_ratio;
    } else {
        si->dpp_clk_ratio_x10 = dpp_ratio_x10_list[0];
    }
    config_str = soc_property_get_str(unit, spn_DPP_CLOCK_RATIO);
    if (config_str) {
        ratio_list_len = COUNTOF(dpp_ratio_x10_list);
        for (ratio_idx = 0; ratio_idx < ratio_list_len; ratio_idx++) {
            if (!sal_strcmp(config_str, str_ratio[ratio_idx])) {
                break;
            }
        }
        if (ratio_idx < ratio_list_len &&
            !(si->frequency > 1125 && dpp_ratio_x10_list[ratio_idx] == 10)) {
            /* ratio 1:1 with core freq larger than 1125MHz is not supported */
            si->dpp_clk_ratio_x10 = dpp_ratio_x10_list[ratio_idx];
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "*** Input dpp clock frequency ratio %s is not "
                                "supported with core frequency %d!\n"),
                     config_str, si->frequency));
        }
    }

    /* Line rate mode requires DPR 1:1 and Core frequency no more than 1125MHz */
    if ((!si->oversub_mode)) {
        if (si->frequency > 1125) {
            LOG_CLI((BSL_META_U(unit,
                                "*** Input core clock frequency %dMHz is not "
                                "supported with line rate mode! "
                                "Must be no more than 1125MHz.\n"), si->frequency));
            return SOC_E_FAIL;
        }
        if (si->dpp_clk_ratio_x10 != 10) {
            LOG_CLI((BSL_META_U(unit,
                                "*** Input dpp clock frequency ratio %s is not "
                                "supported with line rate mode! "
                                "Must be 1:1.\n"), (config_str ? config_str : "")));
            return SOC_E_FAIL;
        }
    }

    for (phy_port = 0; phy_port < _TH2_PHY_PORTS_PER_DEV; phy_port++) {
        si->port_p2l_mapping[phy_port] = -1;
        si->port_p2m_mapping[phy_port] = -1;
    }
    for (port = 0; port < _TH2_DEV_PORTS_PER_DEV; port++) {
        si->port_l2p_mapping[port] = -1;
        si->port_l2i_mapping[port] = -1;
        si->port_speed_max[port] = -1;
        si->port_group[port] = -1;
        si->port_serdes[port] = -1;
        si->port_pipe[port] = -1;
        si->port_num_lanes[port] = -1;
        _soc_th2_port_speed_cap[unit][port] = 0;
    }
    for (mmu_port = 0; mmu_port < _TH2_MMU_PORTS_PER_DEV; mmu_port++) {
        si->port_m2p_mapping[mmu_port] = -1;
    }
    SOC_PBMP_CLEAR(si->eq_pbm);
    SOC_PBMP_CLEAR(si->management_pbm);
    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        SOC_PBMP_CLEAR(si->pipe_pbm[pipe]);
    }
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
    for (port = 1; port < _TH2_DEV_PORTS_PER_DEV; port++) {
        if (si->port_l2p_mapping[port] != -1) { /* fixed mapped port */
            continue;
        }
        if (port == _TH2_DEV_RSV_PORT) { /* Reserved port */
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
                                "Port %d: Missing physical port information "
                                "\"%s\"\n"),
                     port, config_str));
            rv = SOC_E_FAIL;
            continue;
        }
        if (phy_port < 0 || phy_port >= _TH2_PHY_PORTS_PER_DEV) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Invalid physical port number %d\n"),
                     port, phy_port));
            rv = SOC_E_FAIL;
            continue;
        }
        tsc_id = (phy_port - 1) / _TH2_PORTS_PER_PBLK;
        if (soc_feature(unit, soc_feature_untethered_otp)) {
            if (SHR_BITGET(SOC_BOND_INFO(unit)->tsc_disabled, tsc_id)) {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Invalid physical port number %d. Skipped\n"),
                         port, phy_port));
                continue;
            }
            if (SHR_BITGET(SOC_BOND_INFO(unit)->force_hg, tsc_id) &&
                (SOC_PBMP_MEMBER(eth_pbm, port))) {
                LOG_CLI((BSL_META_U(unit,
                           "Port %d: non-hg port in TSC(%d) which is hg-only\n"),
                           port, tsc_id));
                rv = SOC_E_FAIL;
                continue;
            }
        }
        if (!_soc_tomahawk2_phy_dev_port_check(unit, phy_port, port)) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Invalid mapping physical port number %d\n"),
                     port, phy_port));
            rv = SOC_E_FAIL;
            continue;
        }
        /* coverity[check_after_sink : FALSE] */
        if (si->port_p2l_mapping[phy_port] != -1) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Physical port %d is used by port "
                                "%d\n"),
                     port, phy_port, si->port_p2l_mapping[phy_port]));
            rv = SOC_E_FAIL;
            continue;
        }
        /* In order to reuse the mapping between logical and physical port
         * number for the mapping relationship between physical and IDB port
         * number, we have to make maping between IDB and logical port as fixed.
         * For the management ports:
         *  phy_port    IDB port  => Logical port
         *  257         32              66
         *  259         32              100
         */
        if ((phy_port == _TH2_PHY_PORT_MNG0 && port != _TH2_DEV_PORT_MNG0) ||
            (phy_port == _TH2_PHY_PORT_MNG1 && port != _TH2_DEV_PORT_MNG1)) {
            LOG_CLI((BSL_META_U(unit,
                                "Management Port %d: Invalid port number %d\n"),
                     phy_port, port));
            rv = SOC_E_FAIL;
            continue;
        }
        /* Check device port number and physical port number of general port
         * are in the same pipeline range. */
        pipe = port/_TH2_DEV_PORTS_PER_PIPE;
        if ((port != _TH2_DEV_PORT_MNG0 && port != _TH2_DEV_PORT_MNG1) &&
            ((phy_port < (pipe * _TH2_GPHY_PORTS_PER_PIPE + 1)) ||
            (phy_port > (pipe + 1) * _TH2_GPHY_PORTS_PER_PIPE))) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d, Physical port %d: "
                                "Not in same pipeline range\n"),
                     port, phy_port));
            rv = SOC_E_FAIL;
            continue;
        }
        /* Skip HG ports if device is under latency mode */
        if ((!SOC_PBMP_MEMBER(eth_pbm, port)) &&
            (latency != SOC_SWITCH_BYPASS_MODE_NONE)) {
            LOG_CLI((BSL_META_U(unit,
                    "Skip port %d: HG not supported under latency mode\n"),
                     port));
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
                                    "Port %d: Missing bandwidth information "
                                    "\"%s\"\n"),
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
            /* Skip ':' between bandwidth and cap or options */
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
                    sub_str = sub_str_end;
                    _soc_th2_port_speed_cap[unit][port] = bandwidth_cap;
                    /* Flex config detected, port speed cap specified */
                    is_any_cap = TRUE;
                    break;
                default:
                    if (!(*sub_str == 'i' || *sub_str == '1' ||
                          *sub_str == '2' || *sub_str == '4')) {
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
                                            "Port %d: Bad config string "
                                            "\"%s\"\n"),
                                 port, config_str));
                        rv = SOC_E_FAIL;
                        continue;
                    }
                    sub_str++;

                    if (*sub_str != 'i') {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string "
                                            "\"%s\"\n"),
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
        si->port_init_speed[port] = port_bandwidth;
        if (option1 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
        } else {
            switch (option1) {
            case '1': si->port_num_lanes[port] = 1; break;
            case '2': si->port_num_lanes[port] = 2; break;
            case '4': si->port_num_lanes[port] = 4; break;
            default: break;
            }
        }
        if (option2 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
        }
    }
    
    /* Setup port_num_lanes */
    for (port = 0; port < _TH2_DEV_PORTS_PER_DEV; port++) {
        if (si->port_speed_max[port] > 53000) {
            si->port_num_lanes[port] = 4;
        } else if (si->port_speed_max[port] == 40000 ||
                   si->port_speed_max[port] == 42000) {
            /* Note: 40G, HG[42] can operate either in dual or quad lane mode 
                     Check if adjacent ports exist to set dual lane mode */
            phy_port = si->port_l2p_mapping[port];
            if (phy_port >= 1 && phy_port <= _TH2_PHY_PORTS_PER_DEV) {
                int sis_port;

                if (phy_port % 4 == 1) {
                    sis_port = si->port_p2l_mapping[phy_port+2];
                    if ((si->port_speed_max[sis_port] > 0) &&
                        !SOC_PBMP_MEMBER(si->all.disabled_bitmap, sis_port)) {
                        si->port_num_lanes[port] = 2;
                    }
                } else if (phy_port % 4 == 3) {
                    si->port_num_lanes[port] = 2;
                }
            }
            /* Else set to quad lane mode by default if the user did not 
               specify anything and no sister ports existed */
            if (si->port_num_lanes[port] == -1) {
                si->port_num_lanes[port] = 4;
            }
        } else if ((si->port_speed_max[port] >= 20000) && 
                    !(si->port_speed_max[port] == 25000 ||
                      si->port_speed_max[port] == 27000)) {
            /* 50G, HG[53], 20G MLD, HG[21] use 2 lanes */
            si->port_num_lanes[port] = 2;
        } else if (si->port_speed_max[port] > 0) {
            /* 10G XFI, HG[11], 25G XFI and HG[27] use 1 lane */
            if (si->port_num_lanes[port] == -1) {
                /* But RXAUI and XAUI on mgmt ports use 2 and 4 lanes, which 
                 * can be provided via the portmap interface.
                 */
                si->port_num_lanes[port] = 1;
            }
        }
    }

    /* check portmap */
    SOC_IF_ERROR_RETURN(_soc_tomahawk2_port_mapping_check(unit));

    /* get flex port properties */
    if (SOC_FAILURE(_soc_tomahawk2_port_flex_init(unit, is_any_cap))) {
        rv = SOC_E_FAIL;
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (si->flex_eligible) {
        if (SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN(
                soc_th2_flexport_scache_recovery(unit));
        } else {
            SOC_IF_ERROR_RETURN(
                soc_th2_flexport_scache_allocate(unit));
        }
    }
#endif

    /* Setup high speed port (HSP) pbm */
    for (port = 0; port < _TH2_DEV_PORTS_PER_DEV; port++) {
        phy_port = si->port_l2p_mapping[port];
        if (phy_port <= 0 || phy_port > _TH2_PHY_PORT_MNG1) {
            continue;
        }

        pipe = port/_TH2_DEV_PORTS_PER_PIPE;
        if (phy_port == _TH2_PHY_PORT_MNG0) {
            si->port_l2i_mapping[port] = 32;
            si->port_p2m_mapping[phy_port] = 96;
            si->port_serdes[port] = 64;
            SOC_PBMP_PORT_ADD(si->management_pbm, port);
        } else if (phy_port == _TH2_PHY_PORT_MNG1) {
            si->port_l2i_mapping[port] = 32;
            si->port_p2m_mapping[phy_port] = 160;
            si->port_serdes[port] = 64;
            SOC_PBMP_PORT_ADD(si->management_pbm, port);
        } else {
            idb_port = (port % _TH2_DEV_PORTS_PER_PIPE) - (pipe == 0 ? 1 : 0);
            mmu_port = pipe * _TH2_MMU_PORTS_OFFSET_PER_PIPE + idb_port;
            si->port_l2i_mapping[port] = idb_port;
            si->port_p2m_mapping[phy_port] = mmu_port;
            si->port_serdes[port] = (phy_port - 1) / _TH2_PORTS_PER_PBLK;
        }
        si->port_pipe[port] = pipe;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);

        if (phy_port == _TH2_PHY_PORT_MNG0 || phy_port == _TH2_PHY_PORT_MNG1) {
            continue;
        }

        /* In TH2, only the front panel ports can be oversubscription
         * ports, and they are on IDB port numbers 0 to 31
         */
        if (oversub_mode) {
            SOC_PBMP_PORT_ADD(si->oversub_pbm, port);
        }

        if (si->port_num_lanes[port] > 1) {
            SOC_PBMP_PORT_ADD(si->eq_pbm, port);
        }
    }

    /* Setup pipe/xpe mapping
     * XPE0 (SC_R/XPE_A): IP 0/3 EP 0/1
     * XPE1 (SC_S/XPE_A): IP 0/3 EP 2/3
     * XPE2 (SC_R/XPE_B): IP 1/2 EP 0/1
     * XPE3 (SC_S/XPE_B): IP 1/2 EP 2/3
     */
    pipe_map = 0;
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            continue;
        }
        pipe_map |= 1 << pipe;
    }
    xpe_map = 0;
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        ipipe_map = (xpe < 2 ? 0x9 : 0x6) & pipe_map;
        epipe_map = (xpe & 1 ? 0xc : 0x3) & pipe_map;
        if (ipipe_map == 0 || epipe_map == 0) {
            continue;
        }
        xpe_map |= 1 << xpe;
        si->xpe_ipipe_map[xpe] = ipipe_map;
        si->xpe_epipe_map[xpe] = epipe_map;
    }
    sc_map = 0;
    for (sc = 0; sc < NUM_SLICE(unit); sc++) {
        ipipe_map = pipe_map;
        epipe_map = (sc & 1 ? 0xc : 0x3) & pipe_map;
        if (ipipe_map == 0 || epipe_map == 0) {
            continue;
        }
        sc_map |= 1 << sc;
        si->sc_ipipe_map[sc] = ipipe_map;
        si->sc_epipe_map[sc] = epipe_map;
    }
    sed_map = 0;
    for (sed = 0; sed < NUM_SED(unit); sed++) {
        ipipe_map = pipe_map;
        epipe_map = (sed & 1 ? 0xc : 0x3) & pipe_map;
        if (ipipe_map == 0 || epipe_map == 0) {
            continue;
        }
        sed_map |= 1 << sed;
        si->sed_ipipe_map[sed] = ipipe_map;
        si->sed_epipe_map[sed] = epipe_map;
    }
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        /* Number of pipes is 4 in Tomahawk2 */
        /* coverity[overrun-local : FALSE] */
        if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            continue;
        }
        /* coverity[overrun-local : FALSE] */
        si->ipipe_xpe_map[pipe] = (pipe == 0 || pipe == 3 ? 0x3 : 0xc) &
            xpe_map;
        /* coverity[overrun-local : FALSE] */
        si->epipe_xpe_map[pipe] = (pipe < 2 ? 0x5 : 0xa) & xpe_map;
        /* coverity[overrun-local : FALSE] */
        si->ipipe_sc_map[pipe] = sc_map;
        /* coverity[overrun-local : FALSE] */
        si->epipe_sc_map[pipe] = (pipe < 2 ? 0x1 : 0x2) & sc_map;
        /* coverity[overrun-local : FALSE] */
        si->ipipe_sed_map[pipe] = sed_map;
        /* coverity[overrun-local : FALSE] */
        si->epipe_sed_map[pipe] = (pipe < 2 ? 0x1 : 0x2) & sed_map;
    }

    /* Setup clport refclk master block bitmap.
     * In Tomahawk2, there are 8 port macros (clport 6,7,24,25,38,39,56,57 )that
     * will driven directly from the system board and they will act as the
     * refclk master to drive the other instances.
     */
    SHR_BITCLR_RANGE(si->pm_ref_master_bitmap, 0, SOC_MAX_NUM_BLKS);
    for (blk_idx = 0; blk_idx < SOC_MAX_NUM_BLKS; blk_idx++) {
        int blknum = SOC_BLOCK_NUMBER(unit, blk_idx);
        int blktype = SOC_BLOCK_TYPE(unit, blk_idx);

        if (blktype == SOC_BLK_CLPORT) {
            if ((blknum == 6 || blknum == 7 || blknum == 24 || blknum == 25 ||
              blknum == 38 ||blknum == 39 || blknum == 56 || blknum == 57)) {
                SHR_BITSET(si->pm_ref_master_bitmap, blk_idx);
            }
	}
    }

    return rv;
}

int
_soc_tomahawk2_port_mapping_init(int unit)
{
    soc_info_t *si;
    int port, phy_port, idb_port;
    int index;
    int num_port;
    soc_reg_t reg;
    soc_mem_t mem;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe;

    /*
     * 96 entries MMU_CHFC_SYSPORT_MAPPINGm.SYS_PORT
     * 256 entries SYS_PORTMAPm.DEVICE_PORT_NUMBER
     */

    si = &SOC_INFO(unit);

    /* Ingress physical to IDB port mapping */
    sal_memset(&entry, 0, sizeof(entry));

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        mem = SOC_MEM_UNIQUE_ACC(unit, ING_PHY_TO_IDB_PORT_MAPm)[pipe];
        for (index = 0; index < _TH2_GPHY_PORTS_PER_PIPE; index++) {
            phy_port = index + 1 + pipe * _TH2_GPHY_PORTS_PER_PIPE;
            port = si->port_p2l_mapping[phy_port];
            if (port != -1) {
                idb_port = si->port_l2i_mapping[port];
                soc_mem_field32_set(unit, mem, &entry, IDB_PORTf, idb_port);
                soc_mem_field32_set(unit, mem, &entry, VALIDf, 1);
            } else {
                soc_mem_field32_set(unit, mem, &entry, VALIDf, 0);
            }
            SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry));
        }
    }

    num_port = soc_mem_index_count(unit, PORT_TABm) - 1;

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

    PBMP_ALL_ITER(unit, port) {
        phy_port = si->port_l2p_mapping[port];

        /* MMU port to device port mapping */
        rval = 0;
        reg = MMU_PORT_TO_DEVICE_PORT_MAPPINGr;
        soc_reg_field_set(unit, reg, &rval, DEVICE_PORTf, port);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));

        /* MMU port to physical port mapping */
        rval = 0;
        reg = MMU_PORT_TO_PHY_PORT_MAPPINGr;
        soc_reg_field_set(unit, reg, &rval, PHY_PORTf, phy_port);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));

        /* MMU port to system port mapping */
        rval = 0;
        reg = MMU_PORT_TO_SYSTEM_PORT_MAPPINGr;
        soc_reg_field_set(unit, reg, &rval, SYSTEM_PORTf, port);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }
    return SOC_E_NONE;
}

void
_soc_th2_mmu_init_dev_config(int unit, _soc_mmu_device_info_t *devcfg,
                            int lossless)
{
    sal_memset(devcfg, 0, sizeof(_soc_mmu_device_info_t));

    devcfg->max_pkt_byte = _TH_MMU_MAX_PACKET_BYTES;
    devcfg->mmu_hdr_byte = _TH_MMU_PACKET_HEADER_BYTES;
    devcfg->jumbo_pkt_size = _TH_MMU_JUMBO_FRAME_BYTES;
    devcfg->default_mtu_size = _TH_MMU_DEFAULT_MTU_BYTES;
    devcfg->mmu_cell_size = _TH_MMU_BYTES_PER_CELL;
    devcfg->mmu_total_cell = _TH2_MMU_TOTAL_CELLS_PER_XPE;
    devcfg->num_pg = _TH_MMU_NUM_PG;
    devcfg->num_service_pool = _TH_MMU_NUM_POOL;
    devcfg->flags = SOC_MMU_CFG_F_PORT_MIN | SOC_MMU_CFG_F_PORT_POOL_MIN |
                    SOC_MMU_CFG_F_RQE | SOC_MMU_CFG_F_EGR_MCQ_ENTRY;
    devcfg->total_mcq_entry = _TH2_MMU_TOTAL_MCQ_ENTRY(unit);
    devcfg->rqe_queue_num = _TH_MMU_NUM_RQE_QUEUES;
}

STATIC int
_soc_th2_default_pg_headroom(int unit, soc_port_t port,
                            int lossless)
{
    int speed = 1000, hdrm = 0;
    uint8 port_oversub = 0;

    if (IS_CPU_PORT(unit, port)) {
        return 77;
    } else if (!lossless) {
        return 0;
    } else if (IS_LB_PORT(unit, port)) {
        return 150;
    }

    speed = SOC_INFO(unit).port_speed_max[port];

    if (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port)) {
        port_oversub = 1;
    }

    if ((speed >= 1000) && (speed < 20000)) {
        hdrm = port_oversub ? 
            _TH2_PG_HEADROOM_OVERSUB_10G : _TH2_PG_HEADROOM_LINERATE_10G;
    } else if ((speed >= 20000) && (speed <= 30000)) {
        hdrm = port_oversub ? 
            _TH2_PG_HEADROOM_OVERSUB_25G : _TH2_PG_HEADROOM_LINERATE_25G;
    } else if ((speed >= 30000) && (speed <= 42000)) {
        hdrm = port_oversub ? 
            _TH2_PG_HEADROOM_OVERSUB_40G : _TH2_PG_HEADROOM_LINERATE_40G;
    } else if ((speed >= 50000) && (speed < 100000)) {
        hdrm = port_oversub ? 
            _TH2_PG_HEADROOM_OVERSUB_50G : _TH2_PG_HEADROOM_LINERATE_50G;
    } else if (speed >= 100000) {
        hdrm = port_oversub ? 
            _TH2_PG_HEADROOM_OVERSUB_100G : _TH2_PG_HEADROOM_LINERATE_100G;
    } else {
        hdrm = port_oversub ? 
            _TH2_PG_HEADROOM_OVERSUB_10G : _TH2_PG_HEADROOM_LINERATE_10G;
    }
    return hdrm;
}

STATIC int
_soc_th2_default_mcq_guarantee(int unit, soc_port_t port,
                               int lossless)
{
    if (lossless) {
        return 0;
    } else if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
        return 8;
    } else {
        return 0;
    }
}

STATIC int
_soc_th2_mmu_config_buf_asf(int unit, 
    _soc_mmu_cfg_buf_t *buf, int lossless)
{
    int pipe, pipe_map, xpe, pm;
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    int oversub, num_ports, ports[3], asf, cap;
    int asf_profile;
    int asf_reserved_cells[3][3][2] =     /* 1/2/4 port, asf profile, line/oversub*/
        {
            {{ 0,  0}, {16,  27}, { 46,  56}},     /* 1-port PM */
            {{ 0,  0}, {30,  52}, { 92, 112}},     /* 2-ports PM */
            {{ 0,  0}, {60, 104}, {184, 224}},     /* 3/4-ports PM */
        };
    int asf_reserved_cells_cap[3][2] =
        {{ 0,  0}, {16, 27}, {46, 56}};


    asf_profile = soc_property_get(unit, spn_ASF_MEM_PROFILE,
        _SOC_TH_ASF_MEM_PROFILE_SIMILAR);
    if ((asf_profile > _SOC_TH_ASF_MEM_PROFILE_EXTREME)
        || (asf_profile < _SOC_TH_ASF_MEM_PROFILE_NONE)) {
        asf_profile = _SOC_TH_ASF_MEM_PROFILE_NONE;
    }

    si = &SOC_INFO(unit);
    oversub = SOC_PBMP_IS_NULL(si->oversub_pbm) ? 0 : 1;
    
    for (xpe = 0; xpe < _TH2_XPES_PER_DEV; xpe ++) {
        /* use pool 0 only */
        buf_pool = &buf->pools_xpe[xpe][0];
        asf = 0;
        ports[0] = ports[1] = ports[2] = 0;
        
        pipe_map = si->xpe_epipe_map[xpe];
        for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe ++) {
            if (pipe_map & (1 << pipe)) {
                for (pm = pipe * _TH2_PBLKS_PER_PIPE; 
                    pm < (pipe + 1) * _TH2_PBLKS_PER_PIPE; pm ++) {
                    num_ports = si->pm_max_ports[pm + 1];
                    if (num_ports == 1) {
                        ports[0] ++;
                    } else if (num_ports == 2) {
                        ports[1] ++;
                    } else if ((num_ports == 4) || (num_ports == 3)) {
                        ports[2] ++;
                    }
                }
            }
        }

        asf = asf_reserved_cells[0][asf_profile][oversub] * ports[0]
            + asf_reserved_cells[1][asf_profile][oversub] * ports[1]
            + asf_reserved_cells[2][asf_profile][oversub] * ports[2];
        cap = asf_reserved_cells_cap[asf_profile][oversub] * 64;
        asf = ((asf < cap) ? asf : cap);
        
        buf_pool->asf_reserved = asf;
    }

    return SOC_E_NONE;
}

STATIC int 
_soc_th2_mmu_config_buf_queue_guarantee(int unit, 
    _soc_mmu_cfg_buf_t *buf, _soc_mmu_device_info_t *devcfg, int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    int default_mtu_cells;
    int cpu[] = {1, 0, 0, 0};
    int lb[] = {1, 1, 1, 1};
    int xpe, pipe, pipe_map;
    int port_num;

    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    si = &SOC_INFO(unit);

    for (xpe = 0; xpe < _TH2_XPES_PER_DEV; xpe ++) {
        buf_pool = &buf->pools_xpe[xpe][0];
        buf_pool->queue_guarantee = 0;
        buf_pool->queue_group_guarantee = 0;
        buf_pool->rqe_guarantee = _TH_MMU_NUM_RQE_QUEUES * 8;
        if (lossless) {
            continue;
        }
        
        pipe_map = si->xpe_epipe_map[xpe];
        for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe ++) {
            if (pipe_map & (1 << pipe)) {
                /* cosq guarantee, cpu and lb only */
                buf_pool->queue_guarantee +=
                    (cpu[pipe] * 48 + lb[pipe] * 10) * default_mtu_cells;
                /* queue group guarantee, fp and mgmt */
                SOC_PBMP_COUNT(si->pipe_pbm[pipe], port_num);
                port_num = port_num - cpu[pipe] - lb[pipe];
                buf_pool->queue_group_guarantee +=
                    port_num * default_mtu_cells * 2;
            }
        }
    }

    return SOC_E_NONE;
}

STATIC int 
_soc_th2_mmu_config_buf_flex_port(int unit, 
    _soc_mmu_cfg_buf_t *buf, int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    int cpu[] = {1, 0, 0, 0};
    int mgmt[] = {0, 1, 1, 0};
    int lb[] = {1, 1, 1, 1};
    int xpe, pipe, pipe_map, pm;
    int num_ports, ports[3], cpu_ports, lb_ports, mgmt_ports;
    int headroom, cap;
    int headroom_cells[3] =     /* 1/2/4 port */
        {_TH2_PG_HEADROOM_OVERSUB_100G, 
         _TH2_PG_HEADROOM_OVERSUB_50G,
         _TH2_PG_HEADROOM_OVERSUB_25G};

    si = &SOC_INFO(unit);
    
    for (xpe = 0; xpe < _TH2_XPES_PER_DEV; xpe ++) {
        /* use pool 0 only */
        buf_pool = &buf->pools_xpe[xpe][0];
        
        /* ingress */
        /* count number of ports */
        pipe_map = si->xpe_ipipe_map[xpe];
        ports[0] = ports[1] = ports[2] = 0;
        cpu_ports = lb_ports = mgmt_ports = 0;
        for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe ++) {
            if (pipe_map & (1 << pipe)) {
                cpu_ports += cpu[pipe];
                lb_ports += lb[pipe];
                mgmt_ports += mgmt[pipe];
                for (pm = pipe * _TH2_PBLKS_PER_PIPE; 
                    pm < (pipe + 1) * _TH2_PBLKS_PER_PIPE; pm ++) {
                    num_ports = si->pm_max_ports[pm + 1];
                    if (num_ports == 1) {
                        ports[0] ++;
                    } else if (num_ports == 2) {
                        ports[1] ++;
                    } else if ((num_ports == 4) || (num_ports == 3)) {
                        ports[2] ++;
                    }
                }
            }
        }
        
        if (lossless) {
            headroom = headroom_cells[0] * ports[0]
                + headroom_cells[1] * ports[1] * 2
                + headroom_cells[2] * ports[2] * 4;
            cap = headroom_cells[0] * ports[0] + 
                (64 - ports[0]) * _TH2_PG_HEADROOM_OVERSUB_50G;
            /* pg headroom for fp, cpu, lb */
            buf_pool->prigroup_headroom = 
                MIN(headroom, cap) + cpu_ports * 77 + lb_ports * 150;
            /* pg min for fp, cpu, lb, mgmt, mmu_pg_min = 8 */
            buf_pool->prigroup_guarantee = 
                (MIN((ports[0] + ports[1] * 2 + ports[2] * 4), 64) 
                    + cpu_ports + lb_ports + mgmt_ports) * 8;
        } else {
            /* pg headroom for cpu */
            buf_pool->prigroup_headroom = cpu_ports * 77;
            /* mmu_pg_min = 0 */
            buf_pool->prigroup_guarantee = 0;
        }
        
        /* egress */
        /* count number of ports */
        pipe_map = si->xpe_epipe_map[xpe];
        ports[0] = ports[1] = ports[2] = 0;
        cpu_ports = lb_ports = mgmt_ports = 0;
        for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe ++) {
            if (pipe_map & (1 << pipe)) {
                cpu_ports += cpu[pipe];
                lb_ports += lb[pipe];
                mgmt_ports += mgmt[pipe];
                for (pm = pipe * _TH2_PBLKS_PER_PIPE; 
                    pm < (pipe + 1) * _TH2_PBLKS_PER_PIPE; pm ++) {
                    num_ports = si->pm_max_ports[pm + 1];
                    if (num_ports == 1) {
                        ports[0] ++;
                    } else if (num_ports == 2) {
                        ports[1] ++;
                    } else if ((num_ports == 4) || (num_ports == 3)) {
                        ports[2] ++;
                    }
                }
            }
        }
        
        if (lossless) {
            /* mmu_egr_qgrp_min = 0  */
            buf_pool->queue_group_guarantee = 0;
        } else {
            /* mmu_egr_qgrp_min = 16  */
            buf_pool->queue_group_guarantee = 
               (MIN((ports[0] + ports[1] * 2 + ports[2] * 4), 64) + mgmt_ports)
               * 16;
        }
        
        /* reserve 6 before buffer admission due to design restriction,
           queue minimal guarantee for buffer admission is 0 per v2.2 */
        buf_pool->mcq_entry_reserved = 
            (MIN((ports[0] + ports[1] * 2 + ports[2] * 4), 64) * 10
                + mgmt_ports * 10 + lb_ports * 10 + cpu_ports * 48) 
            * (6 + 0);
        
    }

    return SOC_E_NONE;
}

STATIC int 
_soc_th2_mmu_config_buf_calculate(int unit, 
    _soc_mmu_cfg_buf_t *buf, _soc_mmu_device_info_t *devcfg, int lossless)
{
    soc_info_t *si;
    int flex;
    
    si = &SOC_INFO(unit);
    flex = si->flex_eligible;

    SOC_IF_ERROR_RETURN(
        _soc_th2_mmu_config_buf_asf(unit, buf, lossless));
    
    SOC_IF_ERROR_RETURN(
        _soc_th2_mmu_config_buf_queue_guarantee(unit, buf, devcfg, lossless));
    
    if (flex) {
        SOC_IF_ERROR_RETURN(
            _soc_th2_mmu_config_buf_flex_port(unit, buf, lossless));
    }

    return SOC_E_NONE;
}

/*
 * Default class 1 MMU settings about headroom, guarantee, dynamic, color, ...
 * which depend on single port/queue/qgroup/pg...
 * Version 2.2
 */
STATIC void
_soc_th2_mmu_config_buf_class1(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless)
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
    int port, xpe, idx;
    int total_pool_size = 0;
    int rqe_entry_shared_total;

    si = &SOC_INFO(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Initializing default MMU config class 1(u=%d)\n"), unit));
    max_packet_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->max_pkt_byte +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

    total_pool_size = devcfg->mmu_total_cell; /* per XPE limit */
    
    buf->headroom = 2 * max_packet_cells;

    rqe_entry_shared_total = _TH2_MMU_TOTAL_RQE_ENTRY(unit) - 160 - 88;

    for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if (idx == 0) {  /* 100% scale up by 100 */
            buf_pool->size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->yellow_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->red_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->total_mcq_entry = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->total_rqe_entry = rqe_entry_shared_total;
        } else {
            buf_pool->size = 0;
            buf_pool->yellow_size = 0;
            buf_pool->red_size = 0;
            buf_pool->total_mcq_entry = 0;
            buf_pool->total_rqe_entry = 0;
        }
        
        for (xpe = 0; xpe < _TH2_XPES_PER_DEV; xpe ++) {
            sal_memcpy(&buf->pools_xpe[xpe][idx], buf_pool, 
                sizeof(_soc_mmu_cfg_buf_pool_t));
        }
    }

    for (idx = 0; idx < SOC_TH_MMU_CFG_QGROUP_MAX; idx++) {
        queue_grp = &buf->qgroups[idx];
        /* resume limits - accounted for 8 cell granularity */
        queue_grp->pool_resume = 16;
        queue_grp->yellow_resume = 16;
        queue_grp->red_resume = 16;
        if (lossless) {
            queue_grp->guarantee = 0;
            queue_grp->discard_enable = 0;
        } else {
            queue_grp->guarantee = 16;
            queue_grp->discard_enable = 1;
        }
    }

    PBMP_ALL_ITER(unit, port) {
        if (port >= SOC_MMU_CFG_PORT_MAX) {
            break;
        }
        buf_port = &buf->ports[port];

        /* internal priority to priority group mapping */
        for (idx = 0; idx < 16; idx++) {
            buf_port->pri_to_prigroup[idx] = 7;
        }

        /* priority group to pool mapping */
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
            buf_port->prigroups[idx].pool_idx = 0;
        }

        for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            buf_port_pool->guarantee = 0;
            buf_port_pool->pool_limit = 0;
            buf_port_pool->pool_resume = 0;
            if (idx == 0) {
                buf_port_pool->pool_limit = total_pool_size;
                buf_port_pool->pool_resume =
                            total_pool_size - 16;
            }
        }

        buf_port->pkt_size = max_packet_cells;

        /* priority group */
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
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
                    _soc_th2_default_pg_headroom(unit, port, lossless);
                if (lossless) {
                    buf_prigroup->guarantee = default_mtu_cells;
                }
            }
        }

        /* multicast queue */
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];
            buf_queue->qgroup_id = -1;
            /* accroding to TH2 MMU setting, 
               reserve 6 before buffer admission due to design restriction,
               queue minimal guarantee for buffer admission is 0 per v2.2 */
            buf_queue->mcq_entry_guarantee = 0;
            buf_queue->mcq_entry_reserved = 6;
            buf_queue->guarantee = 
                _soc_th2_default_mcq_guarantee(unit, port, lossless);
            if (lossless) {
                buf_queue->discard_enable = 0;
                buf_queue->color_discard_enable = 0;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
            } else {
                buf_queue->discard_enable = 1;
                buf_queue->color_discard_enable = 0;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
            }
        }

        /* unicast queue */
        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];
            buf_queue->qgroup_id = -1;
            if (lossless) {
                buf_queue->guarantee = 0;
                buf_queue->discard_enable = 0;
                buf_queue->color_discard_enable = 0;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;
            } else {
                buf_queue->guarantee = 0;
                buf_queue->discard_enable = 1;
                buf_queue->color_discard_enable = 0;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;
                /* Enable QGROUP in LOSSY MODE */
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
    for (idx = 0; idx < _TH_MMU_NUM_RQE_QUEUES; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];
        buf_rqe_queue->pool_idx = 0;
        if (lossless) {
            buf_rqe_queue->guarantee = 8;
            buf_rqe_queue->discard_enable = 0;
        } else {
            buf_rqe_queue->guarantee = 8;
            buf_rqe_queue->discard_enable = 1;
        }
    }

}

/*
 * Default class 2 MMU settings about shared buffer limit, which depend on 
 * guaranteed/reserved/headroom buffers on all port/queue/qgroup/pg/...
 * Version 2.2
 */
STATIC void
_soc_th2_mmu_config_buf_class2(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    int port, xpe, idx;
    int total_pool_size = 0;
    int egr_shared_total, egr_shared_total_xpe;
    int ing_shared_total, ing_shared_total_xpe;
    uint32 color_limit = 0;

    si = &SOC_INFO(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Initializing default MMU config class 2(u=%d)\n"), unit));
    
    total_pool_size = devcfg->mmu_total_cell; /* per XPE limit */
    
    ing_shared_total = total_pool_size;
    for (xpe = 0; xpe < _TH2_XPES_PER_DEV; xpe ++) {
        ing_shared_total_xpe = total_pool_size
            - buf->pools_xpe[xpe][0].asf_reserved
            - buf->headroom
            - buf->pools_xpe[xpe][0].prigroup_headroom
            - buf->pools_xpe[xpe][0].prigroup_guarantee
            - buf->pools_xpe[xpe][0].port_guarantee;
        if (lossless) {
            ing_shared_total_xpe = ing_shared_total_xpe
                - buf->pools_xpe[xpe][0].queue_guarantee
                - buf->pools_xpe[xpe][0].queue_group_guarantee
                - buf->pools_xpe[xpe][0].rqe_guarantee;
        }

        if (ing_shared_total_xpe < ing_shared_total) {
            ing_shared_total = ing_shared_total_xpe;
        }
    }
    egr_shared_total = total_pool_size;
    for (xpe = 0; xpe < _TH2_XPES_PER_DEV; xpe ++) {
        egr_shared_total_xpe = total_pool_size
            - buf->pools_xpe[xpe][0].asf_reserved
            - buf->pools_xpe[xpe][0].queue_guarantee
            - buf->pools_xpe[xpe][0].queue_group_guarantee
            - buf->pools_xpe[xpe][0].rqe_guarantee;

        if (egr_shared_total_xpe < egr_shared_total) {
            egr_shared_total = egr_shared_total_xpe;
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "MMU config: Total Shared size: ingress %d egress %d\n"),
                            ing_shared_total, egr_shared_total));

    color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;
    
    for (idx = 0; idx < SOC_TH_MMU_CFG_QGROUP_MAX; idx++) {
        queue_grp = &buf->qgroups[idx];
        if (lossless) {
            queue_grp->pool_limit = egr_shared_total;
            queue_grp->pool_scale = -1;
            queue_grp->red_limit = egr_shared_total;
            queue_grp->yellow_limit = egr_shared_total;
        } else {
            queue_grp->pool_limit = 0;
            queue_grp->pool_scale = 8;
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
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
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
            if (lossless) {
                buf_queue->pool_scale = -1;
                buf_queue->pool_limit = egr_shared_total;
                buf_queue->yellow_limit = egr_shared_total;
                buf_queue->red_limit = egr_shared_total;
            } else {
                buf_queue->pool_scale = 8;
                buf_queue->pool_limit = 0;
                buf_queue->yellow_limit = color_limit
                                          | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
                buf_queue->red_limit = color_limit
                                       | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
            }
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
                buf_queue->red_limit = color_limit
                                       | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
                buf_queue->yellow_limit = color_limit
                                          | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
            }
        }
    }

    /* RQE */
    for (idx = 0; idx < _TH_MMU_NUM_RQE_QUEUES; idx++) {
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
_soc_th2_mmu_config_buf_set_hw_global(int unit, _soc_mmu_cfg_buf_t *buf,
                              _soc_mmu_device_info_t *devcfg, int lossless)
{
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    mmu_thdo_config_qgroup_entry_t cfg_qgrp;
    mmu_thdo_offset_qgroup_entry_t offset_qgrp;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    soc_mem_t mem0, mem1;
    uint32 fval, rval, rval2, rval3;
    int rqlen;
    int idx, pval;
    int max_packet_cells, jumbo_frame_cells, default_mtu_cells, limit, limit_xpe;
    int pipe, xpe;
    int pool_resume = 0;
    int pool_pg_headroom = 0;
    static const soc_mem_t mmu_thdo_qgrp_uc_mem[] = {
        MMU_THDU_CONFIG_QGROUPm,
        MMU_THDU_OFFSET_QGROUPm
    };

    max_packet_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->max_pkt_byte +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    jumbo_frame_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->jumbo_pkt_size +
                                                    devcfg->mmu_hdr_byte,
                                                    devcfg->mmu_cell_size);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    pool_resume = 2 * jumbo_frame_cells;

    rval = 0;
    fval = _TH2_MMU_PHYSICAL_CELLS_PER_XPE - _TH2_MMU_RSVD_CELLS_CFAP_PER_XPE;
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDSETr, &rval, CFAPFULLSETPOINTf,
                      fval);
    SOC_IF_ERROR_RETURN(
        soc_tomahawk_xpe_reg32_set(unit, CFAPFULLTHRESHOLDSETr, -1, -1,
                                   -1, rval));
    
    rval = 0;
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDRESETr, &rval,
                      CFAPFULLRESETPOINTf, fval - (2 * jumbo_frame_cells));
    SOC_IF_ERROR_RETURN(
        soc_tomahawk_xpe_reg32_set(unit, CFAPFULLTHRESHOLDRESETr, -1, -1,
                                   -1, rval));
    
    rval = 0;
    soc_reg_field_set(unit, CFAPBANKFULLr, &rval, LIMITf, 
        _TH2_CFAP_BANK_FULL_LIMIT);
    for (idx = 0; idx < SOC_REG_NUMELS(unit, CFAPBANKFULLr); idx++) {
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_xpe_reg32_set(unit, CFAPBANKFULLr, -1, -1,
                                       idx, rval));
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_MMU_SCFG_TOQ_MC_CFG0r(unit, &rval));
    soc_reg_field_set(unit, MMU_SCFG_TOQ_MC_CFG0r, &rval, 
        MCQE_FULL_THRESHOLDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MMU_SCFG_TOQ_MC_CFG0r(unit, rval));
    
    /* Input thresholds */
    rval = 0;
    soc_reg_field_set(unit, THDI_GLOBAL_HDRM_LIMITr,
                      &rval, GLOBAL_HDRM_LIMITf, buf->headroom / 2);
    /* Per XPE register */
    SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
        THDI_GLOBAL_HDRM_LIMITr, -1, -1, 0, rval));

    for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        pool_pg_headroom = 0;
        limit = buf_pool->total;
        for (xpe = 0; xpe < _TH2_XPES_PER_DEV; xpe ++) {
            limit_xpe = buf_pool->total
                - buf->pools_xpe[xpe][idx].asf_reserved
                - buf->headroom
                - buf->pools_xpe[xpe][idx].prigroup_headroom
                - buf->pools_xpe[xpe][idx].prigroup_guarantee
                - buf->pools_xpe[xpe][idx].port_guarantee;
            if (lossless) {
                limit_xpe = limit_xpe
                    - buf->pools_xpe[xpe][idx].queue_guarantee
                    - buf->pools_xpe[xpe][idx].queue_group_guarantee
                    - buf->pools_xpe[xpe][idx].rqe_guarantee;
            }

            if (limit_xpe < limit) {
                limit = limit_xpe;
            }
            if (buf->pools_xpe[xpe][idx].prigroup_headroom > pool_pg_headroom) {
                pool_pg_headroom = buf->pools_xpe[xpe][idx].prigroup_headroom;
            }
        }

        if (limit > 0) {
            rval = 0;
            soc_reg_field_set(unit, THDI_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
                              limit);
            SOC_IF_ERROR_RETURN(
                soc_tomahawk_xpe_reg32_set(unit, THDI_BUFFER_CELL_LIMIT_SPr,
                                           -1, -1, idx, rval));
        }

        rval = 0;
        soc_reg_field_set(unit, THDI_CELL_RESET_LIMIT_OFFSET_SPr, &rval,
                          OFFSETf, pool_resume);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_xpe_reg32_set(unit, THDI_CELL_RESET_LIMIT_OFFSET_SPr,
                                       -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, THDI_HDRM_BUFFER_CELL_LIMIT_HPr, &rval,
                          LIMITf, pool_pg_headroom);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_xpe_reg32_set(unit, THDI_HDRM_BUFFER_CELL_LIMIT_HPr,
                                       -1, -1, idx, rval));
    }
    
    rval = 0;
    SOC_IF_ERROR_RETURN(
        soc_tomahawk_xpe_reg32_set(unit, THDI_BUFFER_CELL_LIMIT_PUBLIC_POOLr,
                                   -1, -1, 0, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(
        soc_tomahawk_xpe_reg32_set(unit, THDI_HDRM_POOL_CFGr,
                                   -1, -1, 0, rval));

    rval = 0;
    soc_reg_field_set(unit, THDI_PORT_MAX_PKT_SIZEr, &rval,
                        PORT_MAX_PKT_SIZEf, max_packet_cells);
    SOC_IF_ERROR_RETURN(WRITE_THDI_PORT_MAX_PKT_SIZEr(unit, rval));

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

    SOC_IF_ERROR_RETURN(
        WRITE_MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_YELLOWr(unit, 0, 2));

    SOC_IF_ERROR_RETURN(
        WRITE_MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_REDr(unit, 0, 2));

    SOC_IF_ERROR_RETURN(
        WRITE_MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_YELLOWr(unit, 0, 1));

    SOC_IF_ERROR_RETURN(
        WRITE_MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_REDr(unit, 0, 1));

    /* per service pool settings */
    for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        limit = buf_pool->total;
        for (xpe = 0; xpe < _TH2_XPES_PER_DEV; xpe ++) {
            limit_xpe = buf_pool->total
                - buf->pools_xpe[xpe][idx].asf_reserved
                - buf->pools_xpe[xpe][idx].queue_guarantee
                - buf->pools_xpe[xpe][idx].queue_group_guarantee
                - buf->pools_xpe[xpe][idx].rqe_guarantee;
            
            if (limit_xpe < limit) {
                limit = limit_xpe;
            }
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_SHARED_LIMITr, &rval,
                          SHARED_LIMITf, limit);
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_DB_POOL_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr,
                          &rval, YELLOW_SHARED_LIMITf, CEIL(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RED_SHARED_LIMITr,
                          &rval, RED_SHARED_LIMITf, CEIL(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_DB_POOL_RED_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RESUME_LIMITr,
                          &rval, RESUME_LIMITf, CEIL(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_DB_POOL_RESUME_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr,
                          &rval, YELLOW_RESUME_LIMITf, CEIL(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RED_RESUME_LIMITr,
                          &rval, RED_RESUME_LIMITf, CEIL(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_DB_POOL_RED_RESUME_LIMITr, -1, -1, idx, rval));

        /* mcq entries, each XPE carries half the queues */
        limit = buf_pool->total_mcq_entry;
        for (xpe = 0; xpe < _TH2_XPES_PER_DEV; xpe ++) {
            limit_xpe = buf_pool->total_mcq_entry
                - buf->pools_xpe[xpe][idx].mcq_entry_reserved
                - SOC_TH2_MMU_MCQ_ENTRY_RESERVED_TDM_OVERSHOOT;
            
            if (limit_xpe < limit) {
                limit = limit_xpe;
            }
        }
        if (limit <= 0) {
            continue;
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_SHARED_LIMITr,
                          &rval, SHARED_LIMITf, limit / 4);
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_MCQE_POOL_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr,
                          &rval, YELLOW_SHARED_LIMITf, CEIL(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr,
                          &rval, RED_SHARED_LIMITf, CEIL(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RESUME_LIMITr,
                          &rval, RESUME_LIMITf, CEIL(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_MCQE_POOL_RESUME_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr,
                          &rval, YELLOW_RESUME_LIMITf, CEIL(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr,
                                       &rval, RED_RESUME_LIMITf, CEIL(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr, -1, -1, idx, rval));
    }

    rval = 0;
    soc_reg_field_set(unit, MMU_THDR_DB_CONFIGr, &rval, 
                                    CLEAR_DROP_STATE_ON_CONFIG_UPDATEf, 1);
    soc_reg_field_set(unit, MMU_THDR_DB_CONFIGr, &rval, MOP_POLICY_1Bf, 1);
    soc_reg_field_set(unit, MMU_THDR_DB_CONFIGr, &rval, MOP_POLICY_1Af, 0);
    SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIGr(unit, rval));

    /* configure Q-groups */
    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        for (idx = 0; idx < SOC_TH_MMU_CFG_QGROUP_MAX; idx++) {
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
                pval = _TH2_POLL_SCALE_TO_LIMIT(queue_grp->pool_limit,
                          queue_grp->red_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_RED_CELLf, CEIL(pval, 8));

            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                        LIMIT_RED_CELLf,
                                        CEIL(queue_grp->red_limit, 8));
            }

            if (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                pval = _TH2_POLL_SCALE_TO_LIMIT(queue_grp->pool_limit,
                          queue_grp->yellow_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_YELLOW_CELLf, CEIL(pval,8));

            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_YELLOW_CELLf,
                                    CEIL(queue_grp->yellow_limit, 8));
            }

            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_CELLf,
                                queue_grp->pool_resume / 8);
            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_YELLOW_CELLf,
                                queue_grp->pool_resume / 8);
            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_RED_CELLf,
                                queue_grp->pool_resume / 8);
            SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, idx, &cfg_qgrp));
            SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, idx, &offset_qgrp));
        }
    }

    /* RQE */
    for (idx = 0; idx < _TH_MMU_NUM_RQE_QUEUES; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_MIN_PRIQr,
                          &rval, MIN_LIMITf, buf_rqe_queue->guarantee);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_LIMIT_MIN_PRIQr(unit, idx, rval));

        rval = 0;
        rval2 = 0;
        rval3 = 0;
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
                SHARED_RED_LIMITf, CEIL(buf_rqe_queue->red_limit, 8));

            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                SHARED_YELLOW_LIMITf, CEIL(buf_rqe_queue->yellow_limit, 8));
        }

        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr, &rval, LIMIT_ENABLEf,
                          (buf_rqe_queue->discard_enable ? 1 : 0));

        if (buf_rqe_queue->pool_scale != -1) {
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                              &rval, DYNAMIC_ENABLEf, 1);
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                              SHARED_ALPHAf, buf_rqe_queue->pool_scale);
        } else {
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                              SHARED_LIMITf, buf_rqe_queue->pool_limit);
        }
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                          RESET_OFFSETf, 2);

        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG1_PRIQr(unit, idx, rval));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG_PRIQr_REG32(unit, idx, rval2));
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
        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_MIN_PRIQr,
                          &rval, MIN_LIMITf, buf_rqe_queue->guarantee/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_LIMIT_MIN_PRIQr(unit, idx, rval));

        rval = 0;
        rval2 = 0;
        rval3 = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                          &rval, SPIDf, buf_rqe_queue->pool_idx);

        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr, &rval,
                          LIMIT_ENABLEf, (!lossless));
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                          &rval, DYNAMIC_ENABLEf, (!lossless));
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr, &rval,
                          COLOR_ENABLEf, 0);
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                          &rval, COLOR_LIMIT_DYNAMICf, (!lossless));

        limit = (lossless ?
            CEIL(buf->pools[buf_rqe_queue->pool_idx].total_rqe_entry, 8*11) :
            0);
        soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                          &rval3, SHARED_RED_LIMITf, limit);
        soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                          &rval3, SHARED_YELLOW_LIMITf, limit);

        limit = (lossless ?
            CEIL(buf->pools[buf_rqe_queue->pool_idx].total_rqe_entry, 8*11) :
            8);
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_PRIQr, &rval2,
                          SHARED_LIMITf, limit);
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_PRIQr, &rval2,
                          RESET_OFFSETf, 1);

        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG1_PRIQr(unit, idx, rval));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG_PRIQr(unit, idx, rval2));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_LIMIT_COLOR_PRIQr(unit, idx, rval3));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_REDf, default_mtu_cells/8);
        soc_reg_field_set(unit, MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_YELLOWf, default_mtu_cells/8);
        SOC_IF_ERROR_RETURN(
                  WRITE_MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr(unit, idx, rval));

    }

    /* per pool RQE settings */
    for (idx = 0; idx < 4; idx++) {
        buf_pool = &buf->pools[idx];
        if (((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) ||
            (buf_pool->total == 0)) {
            continue;
        }

        limit = buf_pool->total;
        for (xpe = 0; xpe < _TH2_XPES_PER_DEV; xpe ++) {
            limit_xpe = buf_pool->total
                - buf->pools_xpe[xpe][idx].asf_reserved
                - buf->pools_xpe[xpe][idx].queue_guarantee
                - buf->pools_xpe[xpe][idx].queue_group_guarantee
                - buf->pools_xpe[xpe][idx].rqe_guarantee;

            if (limit_xpe < limit) {
                limit = limit_xpe;
            }
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_SPr,
                          &rval, SHARED_LIMITf, limit);
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_SPr, &rval, RESUME_LIMITf,
                          CEIL((limit - default_mtu_cells * 2), 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDR_DB_CONFIG_SPr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_SP_SHARED_LIMITr, &rval,
                          SHARED_RED_LIMITf,
                          CEIL(limit, 8));
        soc_reg_field_set(unit, MMU_THDR_DB_SP_SHARED_LIMITr, &rval,
                          SHARED_YELLOW_LIMITf,
                          CEIL(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDR_DB_SP_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_RED_LIMITf,
                          CEIL((limit - default_mtu_cells * 2), 8));
        soc_reg_field_set(unit, MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_YELLOW_LIMITf,
                          CEIL((limit - default_mtu_cells * 2), 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, -1, -1, idx, rval));

        rqlen = CEIL(buf_pool->total_rqe_entry, 8);
        if (rqlen <= 0) {
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
                                                                     idx, rval));

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
_soc_th2_mmu_config_buf_set_hw_port(int unit, int port, _soc_mmu_cfg_buf_t *buf,
                              _soc_mmu_device_info_t *devcfg, int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    thdi_port_sp_config_entry_t thdi_sp_config;
    thdi_port_pg_config_entry_t pg_config_mem;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    uint32 entry0[SOC_MAX_MEM_WORDS], entry1[SOC_MAX_MEM_WORDS];
    uint32 ent_sz = sizeof(uint32) * SOC_MAX_MEM_WORDS;
    soc_reg_t reg = INVALIDr;
    soc_mem_t mem, mem0, mem1, mem2, mem3;
    uint32 fval, rval;
    int base, numq;
    int idx, midx, pri;
    int default_mtu_cells, limit, limit_xpe, rlimit;
    int pipe, xpe;
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

    si = &SOC_INFO(unit);
    pipe = si->port_pipe[port];
    buf_port = &buf->ports[port];
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

    /*==================== THDI ====================*/

    /********************************
     * Input port per port settings *
 */

    /* Internal priority to priority group mapping */
    for (idx = 0; idx < 16; idx++) {
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

    /* Ingress priority group to ingress service pool mapping */
    rval = 0;
    for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
        soc_reg_field_set(unit, THDI_PORT_PG_SPIDr, &rval,
                          prigroup_spid_field[idx],
                          buf_port->prigroups[idx].pool_idx);
    }
    SOC_IF_ERROR_RETURN(WRITE_THDI_PORT_PG_SPIDr(unit, port, rval));

    /* Enables for port receive, pause, PFC, SAFC */
    fval = 0;
    rval = 0;
    for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
        if (buf_port->prigroups[idx].flow_control_enable != 0) {
            for (pri=0; pri < 16; pri++) {
                if (buf_port->pri_to_prigroup[pri] == idx) {
                    fval |= 1 << pri;
                }
            }
        }
    }
    soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                      INPUT_PORT_RX_ENABLEf, 1);
    soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                      PORT_PRI_XON_ENABLEf, fval);
    soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                      PORT_PAUSE_ENABLEf, fval ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_THDI_INPUT_PORT_XON_ENABLESr(unit, port, rval));

    /*****************************************
     * Input port per port per pool settings *
 */
    mem = SOC_MEM_UNIQUE_ACC(unit, THDI_PORT_SP_CONFIGm)[pipe];
    if (mem != INVALIDm) {
        for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            midx = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port, THDI_PORT_SP_CONFIGm,
                                              idx);
            sal_memset(&thdi_sp_config, 0, sizeof(thdi_sp_config));
            soc_mem_field32_set(unit, mem, &thdi_sp_config,
                                PORT_SP_MIN_LIMITf, buf_port_pool->guarantee);
            soc_mem_field32_set(unit, mem, &thdi_sp_config,
                           PORT_SP_RESUME_LIMITf, buf_port_pool->pool_resume);
            soc_mem_field32_set(unit, mem, &thdi_sp_config,
                               PORT_SP_MAX_LIMITf, buf_port_pool->pool_limit);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                               midx, &thdi_sp_config));
        }
    }

    /***************************************************
     * Input port per port per priority group settings *
 */
    mem = SOC_MEM_UNIQUE_ACC(unit, THDI_PORT_PG_CONFIGm)[pipe];
    if (mem != INVALIDm) {
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf->ports[port].prigroups[idx];

            midx = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port, THDI_PORT_PG_CONFIGm,
                                              idx);
            sal_memset(&pg_config_mem, 0, sizeof(pg_config_mem));
            soc_mem_field32_set(unit, mem, &pg_config_mem,
                                PG_MIN_LIMITf, buf_prigroup->guarantee);

            if (buf_prigroup->pool_scale != -1) {
                soc_mem_field32_set(unit, mem, &pg_config_mem,
                                    PG_SHARED_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem, &pg_config_mem,
                                    PG_SHARED_LIMITf,
                                    buf_prigroup->pool_scale);
            } else {
                soc_mem_field32_set(unit, mem, &pg_config_mem,
                                    PG_SHARED_LIMITf,
                                    buf_prigroup->pool_limit);
            }

            soc_mem_field32_set(unit, mem, &pg_config_mem,
                                PG_GBL_HDRM_ENf,
                                buf_prigroup->device_headroom_enable);
            soc_mem_field32_set(unit, mem, &pg_config_mem,
                                PG_HDRM_LIMITf, buf_prigroup->headroom);

            soc_mem_field32_set(unit, mem, &pg_config_mem,
                                PG_RESET_OFFSETf, buf_prigroup->pool_resume / 8);

            soc_mem_field32_set(unit, mem, &pg_config_mem,
                                PG_RESET_FLOORf, buf_prigroup->pool_floor);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem,
                                              MEM_BLOCK_ALL, midx,
                                              &pg_config_mem));
        }
    }

    /*==================== THDO ====================*/

    /*******************************************
     *  Output port per port per queue setting *
 */

    /* MC */
    numq = si->port_num_cosq[port];
    base = si->port_cosq_base[port];

    mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[0])[pipe];
    mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[1])[pipe];
    if ((numq != 0) && (mem0 != INVALIDm) && (mem1 != INVALIDm)) {
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[idx];

            /* mmu_thdm_db_queue_config_0_entry_t */
            sal_memset(entry0, 0, ent_sz);

            soc_mem_field32_set(unit, mem0, entry0,
                                Q_MIN_LIMITf, buf_queue->guarantee);
            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem0, entry0, Q_COLOR_LIMIT_ENABLEf,
                                    1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem0, entry0,
                           Q_SHARED_ALPHAf, buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_SHARED_LIMITf, buf_queue->pool_limit);
            }
            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_COLOR_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem0, entry0, YELLOW_SHARED_LIMITf,
                     buf_queue->yellow_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
                soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                     buf_queue->red_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0, YELLOW_SHARED_LIMITf,
                     CEIL(buf_queue->yellow_limit, 8));
                soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                     CEIL(buf_queue->red_limit, 8));
            }

            soc_mem_field32_set(unit, mem0, entry0,
                                Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, base + idx,
                               entry0));

            /* mmu_thdm_db_queue_offset_entry_t */
            sal_memset(entry0, 0, ent_sz);

            soc_mem_field32_set(unit, mem1, entry0,
                                RESUME_OFFSETf, (default_mtu_cells * 2)/8);
            soc_mem_field32_set(unit, mem1, entry0,
                                YELLOW_RESUME_OFFSET_PROFILE_SELf, 0);
            soc_mem_field32_set(unit, mem1, entry0,
                                RED_RESUME_OFFSET_PROFILE_SELf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, base + idx,
                               entry0));

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

            /* mmu_thdm_mcqe_queue_config_entry_t */
            sal_memset(entry0, 0, ent_sz);

            soc_mem_field32_set(unit, mem2, entry0,
                                Q_MIN_LIMITf,
                                buf_queue->mcq_entry_guarantee/4);
            
            /* each XPE carries half the queues */
            limit = buf_pool->total_mcq_entry;
            for (xpe = 0; xpe < _TH2_XPES_PER_DEV; xpe ++) {
                limit_xpe = buf_pool->total_mcq_entry
                    - buf->pools_xpe[xpe][buf_queue->pool_idx].mcq_entry_reserved
                    - SOC_TH2_MMU_MCQ_ENTRY_RESERVED_TDM_OVERSHOOT;
                
                if (limit_xpe < limit) {
                    limit = limit_xpe;
                }
            }
            if (limit <= 0) {
                continue;
            }

            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0, Q_COLOR_LIMIT_ENABLEf,
                                    1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_SHARED_ALPHAf,
                                    buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_SHARED_LIMITf, CEIL(limit, 4));
            }
            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_COLOR_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem2, entry0, YELLOW_SHARED_LIMITf,
                     buf_queue->yellow_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
                soc_mem_field32_set(unit, mem2, entry0, RED_SHARED_LIMITf,
                     buf_queue->red_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem2, entry0, YELLOW_SHARED_LIMITf,
                                    CEIL(limit, 8));
                soc_mem_field32_set(unit, mem2, entry0, RED_SHARED_LIMITf,
                                    CEIL(limit, 8));

            }

            soc_mem_field32_set(unit, mem2, entry0,
                                Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem2, MEM_BLOCK_ALL, base + idx,
                               entry0));

            /* mmu_thdm_mcqe_queue_offset_entry_t */
            sal_memset(entry0, 0, ent_sz);

            soc_mem_field32_set(unit, mem3, entry0, RESUME_OFFSETf, 1);
            soc_mem_field32_set(unit, mem3, entry0,
                                YELLOW_RESUME_OFFSET_PROFILE_SELf, 0);
            soc_mem_field32_set(unit, mem3, entry0,
                                RED_RESUME_OFFSET_PROFILE_SELf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem3, MEM_BLOCK_ALL, base + idx,
                               entry0));

        }
    }

    /* Per port per pool */
    for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
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

        midx = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port,
                                            mmu_thdo_port_mc_mem[0],
                                            idx);

        /* mmu_thdm_db_portsp_config_entry_t */
        sal_memset(entry0, 0, ent_sz);

        soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMITf, limit);
        soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                            CEIL(limit, 8));
        soc_mem_field32_set(unit, mem0, entry0, YELLOW_SHARED_LIMITf,
                            CEIL(limit, 8));

        soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMIT_ENABLEf,
                            !lossless);

        soc_mem_field32_set(unit, mem0, entry0, SHARED_RESUME_LIMITf,
                            CEIL(rlimit, 8));
        soc_mem_field32_set(unit, mem0, entry0, YELLOW_RESUME_LIMITf,
                            CEIL(rlimit, 8));
        soc_mem_field32_set(unit, mem0, entry0, RED_RESUME_LIMITf,
                            CEIL(rlimit, 8));

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem0, MEM_BLOCK_ALL,
                            midx, entry0));

        mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_mc_mem[1])[pipe];
        if (mem1 == INVALIDm) {
            continue;
        }
        buf_pool = &buf->pools[idx];

        /* mmu_thdm_mcqe_portsp_config_entry_t */
        sal_memset(entry0, 0, ent_sz);

        limit = buf_pool->total_mcq_entry;
        if (limit <= 0) {
            continue;
        }

        soc_mem_field32_set(unit, mem1, entry0, SHARED_LIMITf,
                            CEIL(limit, 4) - 1);
        soc_mem_field32_set(unit, mem1, entry0, YELLOW_SHARED_LIMITf,
                            CEIL(limit, 8) - 1);
        soc_mem_field32_set(unit, mem1, entry0, RED_SHARED_LIMITf,
                            CEIL(limit, 8) - 1);

        soc_mem_field32_set(unit, mem1, entry0, SHARED_LIMIT_ENABLEf,
                            !lossless);

        soc_mem_field32_set(unit, mem1, entry0, SHARED_RESUME_LIMITf,
                            CEIL(limit, 8) - 2);
        soc_mem_field32_set(unit, mem1, entry0, YELLOW_RESUME_LIMITf,
                            CEIL(limit, 8) - 2);
        soc_mem_field32_set(unit, mem1, entry0, RED_RESUME_LIMITf,
                            CEIL(limit, 8) - 2);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL,
                            midx, entry0));
    }

    /* UC */
    numq = si->port_num_uc_cosq[port];
    base = si->port_uc_cosq_base[port];

    mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[0])[pipe];
    mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[1])[pipe];
    mem2 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[2])[pipe];
    if ((numq != 0) &&
        (mem0 != INVALIDm) && (mem1 != INVALIDm) && (mem2 != INVALIDm)) {
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[si->port_num_cosq[port] + idx];

            /* mmu_thdu_config_queue_entry_t */
            sal_memset(entry0, 0, ent_sz);
            /* mmu_thdu_offset_queue_entry_t */
            sal_memset(entry1, 0, ent_sz);

            soc_mem_field32_set(unit, mem0, entry0,
                                Q_MIN_LIMIT_CELLf, buf_queue->guarantee);
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_DYNAMIC_CELLf,
                                    1);
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_SHARED_ALPHA_CELLf,
                                    buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_SHARED_LIMIT_CELLf,
                                    buf_queue->pool_limit);
            }
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_CELLf,
                                buf_queue->pool_resume / 8);

            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_YELLOW_CELLf,
                     buf_queue->yellow_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_RED_CELLf,
                     buf_queue->red_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_YELLOW_CELLf,
                                    CEIL(buf_queue->yellow_limit, 8));
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_RED_CELLf,
                                    CEIL(buf_queue->red_limit, 8));
            }
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_YELLOW_CELLf,
                                buf_queue->yellow_resume / 8);
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_RED_CELLf,
                                buf_queue->red_resume / 8);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, base + idx, entry0));

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, base + idx, entry1));

            /* mmu_thdo_q_to_qgrp_map_entry_t */
            sal_memset(entry0, 0, ent_sz);
            soc_mem_field32_set(unit, mem2, entry0,
                                Q_SPIDf, buf_queue->pool_idx);

            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_COLOR_ENABLE_CELLf, 1);
            }


            if (buf_queue->qgroup_id >= 0) {
                soc_mem_field32_set(unit, mem2, entry0, QGROUP_VALIDf, 1);
                if (buf_queue->qgroup_min_enable) {
                    soc_mem_field32_set(unit, mem2, entry0, USE_QGROUP_MINf, 1);
                }
            }

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL,
                                              base + idx, entry0));
        }
    }

    /* Per  port per pool unicast */
    for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];

        if (buf_pool->total == 0) {
            continue;
        }

        limit = buf_pool->total;

        mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_uc_mem[0])[pipe];
        mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_uc_mem[1])[pipe];
        if ((mem0 == INVALIDm) || (mem1 == INVALIDm)) {
            continue;
        }
        midx = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port,
                                          mmu_thdo_port_uc_mem[0],
                                          idx);
        /* mmu_thdu_config_port_entry_t */
        sal_memset(entry0, 0, ent_sz);
        /* mmu_thdu_resume_port_entry_t */
        sal_memset(entry1, 0, ent_sz);

        soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMITf, limit);
        soc_mem_field32_set(unit, mem1, entry1,
                            SHARED_RESUMEf,
                            CEIL((limit - default_mtu_cells*2), 8));

        soc_mem_field32_set(unit, mem0, entry0, YELLOW_LIMITf, 
                            CEIL(limit, 8));
        soc_mem_field32_set(unit, mem1, entry1,
                            YELLOW_RESUMEf,
                            CEIL((limit - default_mtu_cells*2), 8));

        soc_mem_field32_set(unit, mem0, entry0, RED_LIMITf, 
                            CEIL(limit, 8));
        soc_mem_field32_set(unit, mem1, entry1,
                            RED_RESUMEf,
                            CEIL((limit - default_mtu_cells*2), 8));

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem0, MEM_BLOCK_ALL,
                            midx, entry0));
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL,
                            midx, entry1));
    }

   return SOC_E_NONE;
}

STATIC int
_soc_th2_mmu_config_buf_set_hw(int unit, _soc_mmu_cfg_buf_t *buf,
                              _soc_mmu_device_info_t *devcfg, int lossless)
{
    int port;

    SOC_IF_ERROR_RETURN
        (_soc_th2_mmu_config_buf_set_hw_global(unit, buf, devcfg, lossless));

    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (_soc_th2_mmu_config_buf_set_hw_port(unit, port, buf, devcfg,
                                                 lossless));
    }
    return SOC_E_NONE;
}

int
soc_th2_mmu_config_init_port(int unit, int port)
{
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_device_info_t devcfg;
    int lossless = 1;
    int rv;

    buf = soc_mmu_cfg_alloc(unit);
    if (!buf) {
        return SOC_E_MEMORY;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    _soc_th2_mmu_init_dev_config(unit, &devcfg, lossless);
    /* Calculate the buf - global as well as per port
     * but _soc_th2_mmu_config_buf_set_hw_port is only made
     * for that port - since it is flex operation - we don't
     * touch any other port */
    /* Class 1 config */
    _soc_th2_mmu_config_buf_class1(unit, buf, &devcfg, lossless);
    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        /* Override default class 1 config */
        _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
    }
    rv = _soc_mmu_cfg_buf_check(unit, buf, &devcfg);
    if (SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU config: Use default setting\n")));
        _soc_th2_mmu_config_buf_class1(unit, buf, &devcfg, lossless);
        _soc_mmu_cfg_buf_calculate(unit, buf, &devcfg);
    }

    /* TH2 buffer special calculation */
    _soc_th2_mmu_config_buf_calculate(unit, buf, &devcfg, lossless);
    /* Default class 2 config */
    _soc_th2_mmu_config_buf_class2(unit, buf, &devcfg, lossless);
    /* Override default class 2 config */
    if (SOC_SUCCESS(rv)) {
        if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
            _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
        }
    }
    
    rv = _soc_th2_mmu_config_buf_set_hw_port(unit, port, buf, &devcfg, lossless);

    soc_mmu_cfg_free(unit, buf);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));
    return rv;
}

int
soc_th2_mmu_config_init(int unit, int test_only)
{
    int rv;
    int lossless = 1;
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_device_info_t devcfg;

    buf = soc_mmu_cfg_alloc(unit);
    if (!buf) {
        return SOC_E_MEMORY;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    /* Update the soc_info mmu_lossless to the updated value */
    SOC_INFO(unit).mmu_lossless = lossless;

    _soc_th2_mmu_init_dev_config(unit, &devcfg, lossless);
    
    /* Default class 1 config */
    _soc_th2_mmu_config_buf_class1(unit, buf, &devcfg, lossless);
    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        /* Override default class 1 config */
        _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
    }
    rv = _soc_mmu_cfg_buf_check(unit, buf, &devcfg);
    if (!test_only) {
        if (SOC_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "MMU config: Use default setting\n")));
            _soc_th2_mmu_config_buf_class1(unit, buf, &devcfg, lossless);
            _soc_mmu_cfg_buf_calculate(unit, buf, &devcfg);
        }
        /* TH2 buffer special calculation */
        _soc_th2_mmu_config_buf_calculate(unit, buf, &devcfg, lossless);
        /* Default class 2 config */
        _soc_th2_mmu_config_buf_class2(unit, buf, &devcfg, lossless);
        /* Override default class 2 config */
        if (SOC_SUCCESS(rv)) {
            if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
                _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
            }
        }

        rv = _soc_th2_mmu_config_buf_set_hw(unit, buf, &devcfg, lossless);
    }

    soc_mmu_cfg_free(unit, buf);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));
    return rv;
}

/*
 * Check and set threshold to register
 * 
 * Parameter:
 *   reg/mem    - reg or mem
 *   field      - threshold field
 *   unique     - if the acc_type is unique or duplicate
 *   xpe        - if unique, inidicate the xpe #
 *   val        - threshold value
 *   decrease   - 1: only new val < current val will be set to reg
 *                0: only new val > current val will be set to reg
 *   chk_zero   - only non-zero val could be set to reg
 */
int
_soc_th2_mmu_config_shared_limit_chk_set(int unit, 
        soc_reg_t reg, soc_mem_t mem, soc_field_t field, int index, 
        int xpe, uint32 val, int decrease, int chk_zero)
{
    uint32 rval, entry[SOC_MAX_MEM_WORDS];
    uint32 cur_val;

    if (reg != INVALIDr) {
        rval = 0;
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_xpe_reg32_get(unit, reg, xpe, -1, index, &rval));
        
        cur_val = soc_reg_field_get(unit, reg, rval, field);
        if (((!chk_zero) || (chk_zero && (cur_val != 0))) &&
            TH2_MMU_SHARED_LIMIT_CHK(cur_val, val, decrease)) {
            soc_reg_field_set(unit, reg, &rval, field, val);
            SOC_IF_ERROR_RETURN
                (soc_tomahawk_xpe_reg32_set(unit, reg, xpe, -1, index, rval));
        }
    } else if (mem != INVALIDm) {
        sal_memset(entry, 0, sizeof(entry));
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_xpe_mem_read(unit, mem, xpe, -1, 
                MEM_BLOCK_ALL, index, entry));
        
        cur_val = soc_mem_field32_get(unit, mem, entry, field);
        if (((!chk_zero) || (chk_zero && (cur_val != 0))) &&
            TH2_MMU_SHARED_LIMIT_CHK(cur_val, val, decrease)) {
            soc_mem_field32_set(unit, mem, entry, field, val);
            SOC_IF_ERROR_RETURN
                (soc_tomahawk_xpe_mem_write(unit, mem, xpe, -1, 
                    MEM_BLOCK_ALL, index, entry));
        }
    } else {
        return SOC_E_INTERNAL;
    }
    
    return SOC_E_NONE;
}

int
soc_th2_mmu_config_shared_buf_set_hw(int unit, int res, 
                        int *thdi_shd, int *thdo_db_shd, int *thdo_qe_shd,
                        int post_update)
{
    soc_info_t *si;
    _soc_mmu_device_info_t devcfg;
    int pipe, xpe, xpe_map;
    uint32 entry0[SOC_MAX_MEM_WORDS], new_limit, rval = 0;
    thdi_port_pg_config_entry_t pg_config_mem;
    mmu_thdo_config_qgroup_entry_t cfg_qgrp;
    int port, base, numq, idx, midx;
    soc_mem_t base_mem = INVALIDm;
    soc_mem_t mem = INVALIDm;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    int granularity;
    int thdi_shd_min, thdo_db_shd_min, thdo_qe_shd_min;
    int default_mtu_cells;

    si = &SOC_INFO(unit);

    xpe_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
    
    _soc_th2_mmu_init_dev_config(unit, &devcfg, 0);
    
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg.default_mtu_size +
                                                   devcfg.mmu_hdr_byte,
                                                   devcfg.mmu_cell_size);
    
    _TH_ARRAY_MIN(thdi_shd, NUM_XPE(unit), xpe_map, thdi_shd_min);
    _TH_ARRAY_MIN(thdo_db_shd, NUM_XPE(unit), xpe_map, thdo_db_shd_min);
    _TH_ARRAY_MIN(thdo_qe_shd, NUM_XPE(unit), xpe_map, thdo_qe_shd_min);
    
    /***********************************
     * THDI
 */
    if (res & 0x1) { /* if (res & THDI) { */

        /* THDI SP entries */
        rval = 0;
        reg = THDI_BUFFER_CELL_LIMIT_SPr;
        field = LIMITf;
        granularity = 1;
        for (xpe = 0; xpe < NUM_XPE(unit); xpe ++) {
            if (!(xpe_map & (1 << xpe))) {
                continue;
            }
            
            new_limit = CEIL(thdi_shd[xpe], granularity);
            for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            }
        }

        /* Input per port settings */
        base_mem = THDI_PORT_PG_CONFIGm;
        PBMP_ALL_ITER(unit, port) {
            pipe = si->port_pipe[port];
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            field = PG_SHARED_LIMITf;
            granularity = 1;
            new_limit = CEIL(thdi_shd_min, granularity);
            /* Input port per Port-PG settings */
            for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
                midx = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port, base_mem, idx);
                sal_memset(&pg_config_mem, 0, sizeof(pg_config_mem));
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                  midx, &pg_config_mem));
                if (!soc_mem_field32_get(unit, mem, &pg_config_mem,
                                         PG_SHARED_DYNAMICf)) {
                    SOC_IF_ERROR_RETURN
                        (_soc_th2_mmu_config_shared_limit_chk_set
                            (unit, INVALIDr, mem, field, midx, -1, new_limit, 
                             post_update, 1));
                }
            }
        }
    }

    /***********************************
     * THDO
 */
    if (res & 0x2) { /* if (res & THDO) { */
        /* Per SP settings */
        for (xpe = 0; xpe < NUM_XPE(unit); xpe ++) {
            if (!(xpe_map & (1 << xpe))) {
                continue;
            }
            
            for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
                /* Data Buffer */
                rval = 0;
                reg = MMU_THDM_DB_POOL_SHARED_LIMITr;
                field = SHARED_LIMITf;
                granularity = 1;
                new_limit = CEIL(thdo_db_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            
                rval = 0;
                reg = MMU_THDM_DB_POOL_RESUME_LIMITr;
                field = RESUME_LIMITf;
                granularity = 8;
                new_limit = CEIL(thdo_db_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            
                rval = 0;
                reg = MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr;
                field = YELLOW_SHARED_LIMITf;
                granularity = 8;
                new_limit = CEIL(thdo_db_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            
                rval = 0;
                reg = MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr;
                field = YELLOW_RESUME_LIMITf;
                granularity = 8;
                new_limit = CEIL(thdo_db_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            
                rval = 0;
                reg = MMU_THDM_DB_POOL_RED_SHARED_LIMITr;
                field = RED_SHARED_LIMITf;
                granularity = 8;
                new_limit = CEIL(thdo_db_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            
                rval = 0;
                reg = MMU_THDM_DB_POOL_RED_RESUME_LIMITr;
                field = RED_RESUME_LIMITf;
                granularity = 8;
                new_limit = CEIL(thdo_db_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            
                /* MCQE */
                rval = 0;
                reg = MMU_THDM_MCQE_POOL_SHARED_LIMITr;
                field = SHARED_LIMITf;
                granularity = 4;
                new_limit = CEIL(thdo_qe_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            
                rval = 0;
                reg = MMU_THDM_MCQE_POOL_RESUME_LIMITr;
                field = RESUME_LIMITf;
                granularity = 8;
                new_limit = CEIL(thdo_qe_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            
                rval = 0;
                reg = MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr;
                field = YELLOW_SHARED_LIMITf;
                granularity = 8;
                new_limit = CEIL(thdo_qe_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            
                rval = 0;
                reg = MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr;
                field = YELLOW_RESUME_LIMITf;
                granularity = 8;
                new_limit = CEIL(thdo_qe_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            
                rval = 0;
                reg = MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr;
                field = RED_SHARED_LIMITf;
                granularity = 8;
                new_limit = CEIL(thdo_qe_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            
                rval = 0;
                reg = MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr;
                field = RED_RESUME_LIMITf;
                granularity = 8;
                new_limit = CEIL(thdo_qe_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            }
        }

        /* configure Q-groups */
        base_mem = MMU_THDU_CONFIG_QGROUPm;
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            for (idx = 0; idx < SOC_TH_MMU_CFG_QGROUP_MAX; idx++) {
                sal_memset(&cfg_qgrp, 0, sizeof(cfg_qgrp));

                SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, idx, &cfg_qgrp));

                field = Q_SHARED_LIMIT_CELLf;
                granularity = 1;
                if (!soc_mem_field32_get(unit, mem, &cfg_qgrp, Q_LIMIT_DYNAMIC_CELLf)) {
                    new_limit = CEIL(thdo_db_shd_min, granularity);
                    SOC_IF_ERROR_RETURN
                        (_soc_th2_mmu_config_shared_limit_chk_set
                            (unit, INVALIDr, mem, field, idx, -1, new_limit, 
                             post_update, 0));
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, &cfg_qgrp,
                                         Q_COLOR_LIMIT_DYNAMIC_CELLf)) {
                    field = LIMIT_RED_CELLf;
                    new_limit = CEIL(thdo_db_shd_min, granularity);
                    SOC_IF_ERROR_RETURN
                        (_soc_th2_mmu_config_shared_limit_chk_set
                            (unit, INVALIDr, mem, field, idx, -1, new_limit, 
                             post_update, 0));

                    field = LIMIT_YELLOW_CELLf;
                    new_limit = CEIL(thdo_db_shd_min, granularity);
                    SOC_IF_ERROR_RETURN
                        (_soc_th2_mmu_config_shared_limit_chk_set
                            (unit, INVALIDr, mem, field, idx, -1, new_limit, 
                             post_update, 0));
                }
            }
        }

        /* Output port per port per queue setting for UC queue */
        PBMP_PORT_ITER(unit, port) {
            base_mem = MMU_THDU_CONFIG_QUEUEm;
            pipe = si->port_pipe[port];

            /* per port UC queue */
            numq = si->port_num_uc_cosq[port];
            base = si->port_uc_cosq_base[port];

            if (numq == 0) {
                continue;
            }
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            for (idx = 0; idx < numq; idx++) {
                sal_memset(entry0, 0, sizeof(mmu_thdu_config_queue_entry_t));

                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL, base + idx, entry0));

                field = Q_SHARED_LIMIT_CELLf;
                granularity = 1;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_LIMIT_DYNAMIC_CELLf)) {
                    new_limit = CEIL(thdo_db_shd_min, granularity);
                    SOC_IF_ERROR_RETURN
                        (_soc_th2_mmu_config_shared_limit_chk_set
                            (unit, INVALIDr, mem, field, base + idx, -1, new_limit, 
                             post_update, 0));
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMIC_CELLf)) {
                    field = LIMIT_YELLOW_CELLf;
                    new_limit = CEIL(thdo_db_shd_min, granularity);
                    SOC_IF_ERROR_RETURN
                        (_soc_th2_mmu_config_shared_limit_chk_set
                            (unit, INVALIDr, mem, field, base + idx, -1, new_limit, 
                             post_update, 0));
                    
                    field = LIMIT_RED_CELLf;
                    new_limit = CEIL(thdo_db_shd_min, granularity);
                    SOC_IF_ERROR_RETURN
                        (_soc_th2_mmu_config_shared_limit_chk_set
                            (unit, INVALIDr, mem, field, base + idx, -1, new_limit, 
                             post_update, 0));
                }
            }
        }

        /* Output port per port per queue setting for MC queue */
        PBMP_PORT_ITER(unit, port) {
            /* DB entries - Queue */
            numq = si->port_num_cosq[port];
            base = si->port_cosq_base[port];
            pipe = si->port_pipe[port];
            if (numq == 0) {
                continue;
            }

            base_mem = MMU_THDM_DB_QUEUE_CONFIGm;
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];

            for (idx = 0; idx < numq; idx++) {
                sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_config_entry_t));

                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL, base + idx, entry0));

                field = Q_SHARED_LIMITf;
                granularity = 1;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_LIMIT_DYNAMICf)) {
                    new_limit = CEIL(thdo_db_shd_min, granularity);
                    SOC_IF_ERROR_RETURN
                        (_soc_th2_mmu_config_shared_limit_chk_set
                            (unit, INVALIDr, mem, field, base + idx, -1, new_limit, 
                             post_update, 0));
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMICf)) {
                    field = YELLOW_SHARED_LIMITf;
                    new_limit = CEIL(thdo_db_shd_min, granularity);
                    SOC_IF_ERROR_RETURN
                        (_soc_th2_mmu_config_shared_limit_chk_set
                            (unit, INVALIDr, mem, field, base + idx, -1, new_limit, 
                             post_update, 0));
                    
                    field = RED_SHARED_LIMITf;
                    new_limit = CEIL(thdo_db_shd_min, granularity);
                    SOC_IF_ERROR_RETURN
                        (_soc_th2_mmu_config_shared_limit_chk_set
                            (unit, INVALIDr, mem, field, base + idx, -1, new_limit, 
                             post_update, 0));
                }
            }

            /* MCQE - Queue */
            base_mem = MMU_THDM_MCQE_QUEUE_CONFIGm;
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            for (idx = 0; idx < numq; idx++) {
                sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_queue_config_entry_t));

                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL, base + idx, entry0));

                field = Q_SHARED_LIMITf;
                granularity = 4;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_LIMIT_DYNAMICf)) {
                    new_limit = CEIL(thdo_qe_shd_min, granularity);
                    SOC_IF_ERROR_RETURN
                        (_soc_th2_mmu_config_shared_limit_chk_set
                            (unit, INVALIDr, mem, field, base + idx, -1, new_limit, 
                             post_update, 0));
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMICf)) {
                    field = YELLOW_SHARED_LIMITf;
                    new_limit = CEIL(thdo_qe_shd_min, granularity);
                    SOC_IF_ERROR_RETURN
                        (_soc_th2_mmu_config_shared_limit_chk_set
                            (unit, INVALIDr, mem, field, base + idx, -1, new_limit, 
                             post_update, 0));
                    
                    field = RED_SHARED_LIMITf;
                    new_limit = CEIL(thdo_qe_shd_min, granularity);
                    SOC_IF_ERROR_RETURN
                        (_soc_th2_mmu_config_shared_limit_chk_set
                            (unit, INVALIDr, mem, field, base + idx, -1, new_limit, 
                             post_update, 0));
                }
            }
        }

        /* RQE */
        for (idx = 0; idx < _TH_MMU_NUM_RQE_QUEUES; idx++) {
            rval = 0;
            SOC_IF_ERROR_RETURN
                (READ_MMU_THDR_DB_CONFIG1_PRIQr(unit, idx, &rval));

            reg = MMU_THDR_DB_CONFIG_PRIQr;
            field = SHARED_LIMITf;
            granularity = 1;
            if (!soc_reg_field_get(unit, MMU_THDR_DB_CONFIG1_PRIQr, rval,
                                   DYNAMIC_ENABLEf)) {
                new_limit = CEIL(thdo_db_shd_min, granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, -1, new_limit, 
                         post_update, 0));
            }

            reg = MMU_THDR_DB_LIMIT_COLOR_PRIQr;
            granularity = 8;
            if (!soc_reg_field_get(unit, MMU_THDR_DB_CONFIG1_PRIQr, rval,
                                   COLOR_LIMIT_DYNAMICf)) {
                field = SHARED_RED_LIMITf;
                new_limit = CEIL(thdo_db_shd_min, granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, -1, new_limit, 
                         post_update, 0));

                field = SHARED_YELLOW_LIMITf;
                new_limit = CEIL(thdo_db_shd_min, granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, -1, new_limit, 
                         post_update, 0));
            }
        }
        
        /* per pool RQE settings */
        for (xpe = 0; xpe < NUM_XPE(unit); xpe ++) {
            if (!(xpe_map & (1 << xpe))) {
                continue;
            }
            
            for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
                reg = MMU_THDR_DB_CONFIG_SPr;
                field = SHARED_LIMITf;
                granularity = 1;
                new_limit = CEIL(thdo_db_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
                
                field = RESUME_LIMITf;
                new_limit = CEIL((thdo_db_shd[xpe] - default_mtu_cells * 2), 8);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            
                reg = MMU_THDR_DB_SP_SHARED_LIMITr;
                field = SHARED_YELLOW_LIMITf;
                granularity = 8;
                new_limit = CEIL(thdo_db_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
                
                field = SHARED_RED_LIMITf;
                granularity = 8;
                new_limit = CEIL(thdo_db_shd[xpe], granularity);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            
                reg = MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr;
                field = RESUME_YELLOW_LIMITf;
                new_limit = CEIL((thdo_db_shd[xpe] - default_mtu_cells * 2), 8);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
                
                field = RESUME_RED_LIMITf;
                new_limit = CEIL((thdo_db_shd[xpe] - default_mtu_cells * 2), 8);
                SOC_IF_ERROR_RETURN
                    (_soc_th2_mmu_config_shared_limit_chk_set
                        (unit, reg, INVALIDm, field, idx, xpe, new_limit, 
                         post_update, 1));
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th2_mmu_config_shared_buf_set_hw_port
 * Purpose:
 *      Update shared limit setting of port.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      port        - (IN) Logical port.
 *      thdi_shd    - (IN) Ingress shared limit
 *      thdo_db_shd - (IN) Egress shared limit
 *      thdo_qe_shd - (IN) Egress shared limit for MCQE
 * Returns:
 *      SOC_E_XXX
 */
int
soc_th2_mmu_config_shared_buf_set_hw_port(int unit, int port, int *thdi_shd,
                                          int *thdo_db_shd, int *thdo_qe_shd)
{
    soc_info_t *si;
    int pipe, xpe_map, base, numq, idx, midx, dynamic_q, update, granularity;
    int thdi_shd_min, thdo_db_shd_min, thdo_qe_shd_min;
    uint32  new_limit, cur_limit, cur_limit1;
    thdi_port_pg_config_entry_t pg_cfg;
    mmu_thdu_config_queue_entry_t cfg_queue;
    mmu_thdm_db_queue_config_entry_t dbq_cfg;
    mmu_thdm_mcqe_queue_config_entry_t mcqe_cfg;
    soc_mem_t mem = INVALIDm;
    soc_field_t field[3];

    si = &SOC_INFO(unit);
    pipe = si->port_pipe[port];
    xpe_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
    _TH_ARRAY_MIN(thdi_shd, NUM_XPE(unit), xpe_map, thdi_shd_min);
    _TH_ARRAY_MIN(thdo_db_shd, NUM_XPE(unit), xpe_map, thdo_db_shd_min);
    _TH_ARRAY_MIN(thdo_qe_shd, NUM_XPE(unit), xpe_map, thdo_qe_shd_min);


    /***********************************
     * THDI
 */

    /* Input per port settings */
    mem = SOC_MEM_UNIQUE_ACC(unit, THDI_PORT_PG_CONFIGm)[pipe];
    field[0] = PG_SHARED_LIMITf;
    granularity = 1;
    new_limit = CEIL(thdi_shd_min, granularity);
    /* Input port per Port-PG settings */
    for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
        midx = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port, THDI_PORT_PG_CONFIGm,
                                          idx);
        sal_memset(&pg_cfg, 0, sizeof(pg_cfg));
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, &pg_cfg));
        if (!soc_mem_field32_get(unit, mem, &pg_cfg, PG_SHARED_DYNAMICf)) {
            cur_limit = soc_mem_field32_get(unit, mem, &pg_cfg, field[0]);
            if ((cur_limit != 0) && (cur_limit != new_limit)) {
                soc_mem_field32_set(unit, mem, &pg_cfg, field[0], new_limit);
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_xpe_mem_write(unit, mem, -1, -1,
                                                MEM_BLOCK_ALL, midx, &pg_cfg));
            }
        }
    }

    /***********************************
     * THDO
 */

    /* per port UC queue */
    numq = si->port_num_uc_cosq[port];
    base = si->port_uc_cosq_base[port];
    if (numq != 0) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDU_CONFIG_QUEUEm)[pipe];
        field[0] = Q_SHARED_LIMIT_CELLf;
        field[1] = LIMIT_YELLOW_CELLf;
        field[2] = LIMIT_RED_CELLf;

        for (idx = 0; idx < numq; idx++) {
            midx = base + idx;
            sal_memset(&cfg_queue, 0, sizeof(cfg_queue));
            update = 0;

            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, &cfg_queue));

            granularity = 1;
            new_limit = CEIL(thdo_db_shd_min, granularity);
            cur_limit = soc_mem_field32_get(unit, mem, &cfg_queue, field[0]);
            dynamic_q = soc_mem_field32_get(unit, mem, &cfg_queue,
                                            Q_LIMIT_DYNAMIC_CELLf);
            if ((new_limit != cur_limit) && !dynamic_q) {
                soc_mem_field32_set(unit, mem, &cfg_queue, field[0], new_limit);
                update = 1;
            }

            granularity = 8;
            new_limit = CEIL(thdo_db_shd_min, granularity);
            cur_limit = soc_mem_field32_get(unit, mem, &cfg_queue, field[1]);
            cur_limit1 = soc_mem_field32_get(unit, mem, &cfg_queue, field[2]);
            dynamic_q = soc_mem_field32_get(unit, mem, &cfg_queue,
                                            Q_COLOR_LIMIT_DYNAMIC_CELLf);
            if (((new_limit != cur_limit) || (new_limit != cur_limit1)) &&
                !dynamic_q) {
                soc_mem_field32_set(unit, mem, &cfg_queue, field[1], new_limit);
                soc_mem_field32_set(unit, mem, &cfg_queue, field[2], new_limit);
                update = 1;
            }

            if (update) {
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_xpe_mem_write(unit, mem, -1, -1,
                                                MEM_BLOCK_ALL, midx,
                                                &cfg_queue));
            }
        }
    }

    /* per port per MC queue */
    numq = si->port_num_cosq[port];
    base = si->port_cosq_base[port];
    if (numq != 0) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDM_DB_QUEUE_CONFIGm)[pipe];
        field[0] = Q_SHARED_LIMITf;
        field[1] = YELLOW_SHARED_LIMITf;
        field[2] = RED_SHARED_LIMITf;

        for (idx = 0; idx < numq; idx++) {
            midx = base + idx;
            sal_memset(&dbq_cfg, 0, sizeof(dbq_cfg));
            update = 0;

            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, &dbq_cfg));

            granularity = 1;
            new_limit = CEIL(thdo_db_shd_min, granularity);
            cur_limit = soc_mem_field32_get(unit, mem, &dbq_cfg, field[0]);
            dynamic_q = soc_mem_field32_get(unit, mem, &dbq_cfg,
                                            Q_LIMIT_DYNAMICf);
            if ((new_limit != cur_limit) && !dynamic_q) {
                soc_mem_field32_set(unit, mem, &dbq_cfg, field[0], new_limit);
                update = 1;
            }

            granularity = 8;
            new_limit = CEIL(thdo_db_shd_min, granularity);
            cur_limit = soc_mem_field32_get(unit, mem, &dbq_cfg, field[1]);
            cur_limit1 = soc_mem_field32_get(unit, mem, &dbq_cfg, field[2]);
            dynamic_q = soc_mem_field32_get(unit, mem, &dbq_cfg,
                                            Q_COLOR_LIMIT_DYNAMICf);
            if (((new_limit != cur_limit) || (new_limit != cur_limit1)) &&
                !dynamic_q) {
                soc_mem_field32_set(unit, mem, &dbq_cfg, field[1], new_limit);
                soc_mem_field32_set(unit, mem, &dbq_cfg, field[2], new_limit);
                update = 1;
            }

            if (update) {
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_xpe_mem_write(unit, mem, -1, -1,
                                                MEM_BLOCK_ALL, midx,
                                                &dbq_cfg));
            }
        }

        /* MCQE - Queue */
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDM_MCQE_QUEUE_CONFIGm)[pipe];
        field[0] = Q_SHARED_LIMITf;
        field[1] = YELLOW_SHARED_LIMITf;
        field[2] = RED_SHARED_LIMITf;
        for (idx = 0; idx < numq; idx++) {
            midx = base + idx;
            sal_memset(&mcqe_cfg, 0, sizeof(mcqe_cfg));
            update = 0;

            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, &mcqe_cfg));

            granularity = 4;
            new_limit = CEIL(thdo_qe_shd_min, granularity);
            cur_limit = soc_mem_field32_get(unit, mem, &mcqe_cfg, field[0]);
            dynamic_q = soc_mem_field32_get(unit, mem, &mcqe_cfg,
                                            Q_LIMIT_DYNAMICf);
            if ((new_limit != cur_limit) && !dynamic_q) {
                soc_mem_field32_set(unit, mem, &mcqe_cfg, field[0], new_limit);
                update = 1;
            }

            granularity = 8;
            new_limit = CEIL(thdo_qe_shd_min, granularity);
            cur_limit = soc_mem_field32_get(unit, mem, &mcqe_cfg, field[1]);
            cur_limit1 = soc_mem_field32_get(unit, mem, &mcqe_cfg, field[2]);
            dynamic_q = soc_mem_field32_get(unit, mem, &mcqe_cfg,
                                            Q_COLOR_LIMIT_DYNAMICf);
            if (((new_limit != cur_limit) || (new_limit != cur_limit1)) &&
                !dynamic_q) {
                soc_mem_field32_set(unit, mem, &mcqe_cfg, field[1], new_limit);
                soc_mem_field32_set(unit, mem, &mcqe_cfg, field[2], new_limit);
                update = 1;
            }

            if (update) {
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_xpe_mem_write(unit, mem, -1, -1,
                                                MEM_BLOCK_ALL, midx,
                                                &mcqe_cfg));
            }
        }
    }

    return SOC_E_NONE;
}

static int
_soc_th2_dpp_ratio_sel(int ratiox10)
{
    switch (ratiox10) {
    case 15:
        /* core : dpp is 3:2 */
        return 1;
    case 20:
        /* core : dpp is 2:1 */
        return 2;
    case 10:
        /* core : dpp is 1:1 */
        return 3;
    default:
        return 1;
    }
}

/*
 * Recommended Min. Slot Pipeline Latency Value
 * CORE/DPP Ratio (R) |  Modes
 * --------------------------------------------
 *	              |  FULL | L2+L3 | L2
 * --------------------------------------------
 *          1         |  189  | 144   |  85
 * --------------------------------------------
 *          1.5       |  309  | 241   | 153
 * --------------------------------------------
 *          2         |  423  | 333   | 215
 * --------------------------------------------
 */
int
soc_th2_slot_pipeline_latency_get(int dpp_ratio_x10, int lmode)
{
    int dpr_id;
    int latency[3][3] = {
        {189, 144,  85},
        {309, 241, 153},
        {423, 333, 215},
    };

    dpr_id = (dpp_ratio_x10 == 20 ? 2 : 
             (dpp_ratio_x10 == 15 ? 1 : 0));

    if ((lmode > SOC_SWITCH_BYPASS_MODE_LOW) || 
        (lmode < SOC_SWITCH_BYPASS_MODE_NONE)) {
         lmode = SOC_SWITCH_BYPASS_MODE_NONE;
    }

    return latency[dpr_id][lmode];
}

STATIC int
_soc_tomahawk2_init_ipep_memory(int unit)
{
    uint32 pipe_map;
    soc_reg_t reg;
    int pipe, count;
    uint32 rval, in_progress;
    int pipe_init_usec;
    soc_timeout_t to;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    /* Initial IPIPE memory */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, REGIONf, 0);
    /* Set count to # entries of largest IPIPE table */
    count = soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
    if (count < soc_mem_index_count(unit, L2Xm)) {
        count = soc_mem_index_count(unit, L2Xm);
    }
    if (count < soc_mem_index_count(unit, L3_ENTRY_ONLYm)) {
        count = soc_mem_index_count(unit, L3_ENTRY_ONLYm);
    }
    if (count < soc_mem_index_count(unit, FPEM_ECCm)) {
        count = soc_mem_index_count(unit, FPEM_ECCm);
    }
    if (count < soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m)) {
        count = soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m);
    }
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, count);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    /* Initial EPIPE memory */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    /* Set count to # entries in largest EPIPE table (EGR_L3_NEXT_HOP) */
    count = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, count);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IPIPE memory initialization done */
    in_progress = pipe_map;
    do {
        for (pipe = 0; pipe < _TH2_PIPES_PER_DEV && in_progress; pipe++) {
            reg = SOC_REG_UNIQUE_ACC(unit, ING_HW_RESET_CONTROL_2r)[pipe];
            if (in_progress & (1 << pipe)) { /* not done yet */
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                    in_progress ^= (1 << pipe);
                }
            }
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET timeout\n"), unit));
            break;
        }
    } while (in_progress != 0);

    /* Wait for EPIPE memory initialization done */
    in_progress = pipe_map;
    do {
        for (pipe = 0; pipe < _TH2_PIPES_PER_DEV && in_progress; pipe++) {
            reg = SOC_REG_UNIQUE_ACC(unit, EGR_HW_RESET_CONTROL_1r)[pipe];
            if (in_progress & (1 << pipe)) { /* not done yet */
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                    in_progress ^= (1 << pipe);
                }
            }
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : EGR_HW_RESET timeout\n"), unit));
            break;
        }
    } while (in_progress != 0);

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    rval = SOC_REG_INFO(unit, EGR_HW_RESET_CONTROL_1r).rst_val_lo;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    return SOC_E_NONE;
}

static int
_soc_tomahawk2_clear_l2_mod_fifo(int unit) {
    uint32 rval;
    sal_usecs_t clear_l2mod_usec, sleep_usec = 10000;
    soc_timeout_t to;
    soc_reg_t reg;
    int count;
    /* Initial L2_MGT memories */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_L2_MGMT_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, L2_MGMT_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, L2_MGMT_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    count = soc_mem_index_count(unit, L2_MOD_FIFOm);
    if (count < soc_mem_index_count(unit, L2_MGMT_SER_FIFOm)) {
        count = soc_mem_index_count(unit, L2_MGMT_SER_FIFOm);
    }
    soc_reg_field_set(unit, L2_MGMT_HW_RESET_CONTROL_1r, &rval, COUNTf, count);
    SOC_IF_ERROR_RETURN(WRITE_L2_MGMT_HW_RESET_CONTROL_1r(unit, rval));

    if (SAL_BOOT_SIMULATION) {
        clear_l2mod_usec = 1000000;
    } else {
        clear_l2mod_usec = 500000;
    }
    soc_timeout_init(&to, clear_l2mod_usec, 0);
    do {
        sal_usleep(sleep_usec);
        /* Make sure L2_MGMT memories are all clean */
        reg = L2_MGMT_HW_RESET_CONTROL_1r;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (soc_reg_field_get(unit, reg, rval, DONEf)) {
            return WRITE_L2_MGMT_HW_RESET_CONTROL_1r(unit, 0);
        }

        if (soc_timeout_check(&to)) {
            break;
        }
    } while (1);

    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
        "unit %d : L2_MGMT_HW_RESET not done (reg val: 0x%x) !! \n"),
        unit, rval));

    return SOC_E_TIMEOUT;
}

STATIC int
_soc_tomahawk2_init_idb_memory(int unit)
{
    uint32 pipe_map;
    soc_reg_t reg;
    int pipe;
    uint32 rval, in_progress;
    int pipe_init_usec;
    soc_timeout_t to;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    /* Initialize IDB memory */
    rval = 0;
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, REGIONf, 1);
    
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, 0x100);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IDB memory initialization done */
    in_progress = pipe_map;
    do {
        for (pipe = 0; pipe < _TH2_PIPES_PER_DEV && in_progress; pipe++) {
            reg = SOC_REG_UNIQUE_ACC(unit, ING_HW_RESET_CONTROL_2r)[pipe];
            if (in_progress & (1 << pipe)) { /* not done yet */
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                    in_progress ^= (1 << pipe);
                }
            }
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET timeout\n"), unit));
            break;
        }
    } while (in_progress != 0);
    rval = 0;
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, REGIONf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    return SOC_E_NONE;
}

int
soc_tomahawk2_init_mmu_memory(int unit)
{
    uint32 rval = 0;

    /* Initialize MMU memory */
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, 0));
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, PARITY_ENf, 1);
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, INIT_MEMf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, INIT_MEMf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
    sal_usleep(20);
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, PARITY_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));

    return SOC_E_NONE;
}

int
soc_tomahawk2_clear_mmu_memory(int unit)
{
#define SOC_MMU_BASE_TYPE_IPORT 0
#define SOC_MMU_BASE_TYPE_EPORT 1
#define SOC_MMU_BASE_TYPE_IPIPE 2
#define SOC_MMU_BASE_TYPE_EPIPE 3
#define SOC_MMU_BASE_TYPE_CHIP 4
#define SOC_MMU_BASE_TYPE_XPE 5
#define SOC_MMU_BASE_TYPE_SC 6
#define SOC_MMU_BASE_TYPE_LAYER 7
    int i, j, use_base_type_views, base_type, index, num_views;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    static const struct {
        soc_mem_t mem; /* base view */
        int use_base_type_views; /* 1 means use xpe/sc/etc views */
        int index; /* -1 means clear entire mem */
    } mem_list[] = {
        { MMU_CBPDATA0m, 1, 0 },
        { MMU_CBPDATA1m, 1, 0 },
        { MMU_CBPDATA2m, 1, 0 },
        { MMU_CBPDATA3m, 1, 0 },
        { MMU_CBPDATA4m, 1, 0 },
        { MMU_CBPDATA5m, 1, 0 },
        { MMU_CBPDATA6m, 1, 0 },
        { MMU_CBPDATA7m, 1, 0 },
        { INVALIDm, 0, -1 }
    };
    for (i = 0; mem_list[i].mem != INVALIDm; i++) {
        mem = mem_list[i].mem;
        index = mem_list[i].index;
        use_base_type_views = mem_list[i].use_base_type_views;
        if (use_base_type_views) {
            base_type = soc_tomahawk_mem_basetype_get(unit, mem);
            switch (base_type) {
            case SOC_MMU_BASE_TYPE_XPE:
                num_views = NUM_XPE(unit);
                break;
            case SOC_MMU_BASE_TYPE_SC:
                num_views = NUM_SLICE(unit);
                break;
            case SOC_MMU_BASE_TYPE_LAYER:
                num_views = NUM_LAYER(unit);
                break;
            case SOC_MMU_BASE_TYPE_IPIPE:
            case SOC_MMU_BASE_TYPE_EPIPE:
                num_views = NUM_PIPE(unit);
                break;
            default:
                num_views = -1; /* not supported */
                break;
            }
            if (num_views < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "mmu_mem %s, base_type %d will not be"
                                      "cleared \n"),
                           SOC_MEM_NAME(unit,mem), base_type));
                continue;
            }
        } else {
            num_views = 0; /* dont_care */
        }

        sal_memset(entry, 0x0,
                   soc_mem_entry_words(unit, mem) * sizeof(uint32));
        if (use_base_type_views) {
            for (j = 0; j < num_views; j++) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "mmu_mem %s, index %d will be "
                                        "cleared \n"),
                            SOC_MEM_NAME(unit, SOC_MEM_UNIQUE_ACC(unit, mem)[j]),
                            index));
                if (index >= 0) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit,
                                       SOC_MEM_UNIQUE_ACC(unit, mem)[j],
                                       MEM_BLOCK_ALL, index, entry));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_clear(unit,
                                       SOC_MEM_UNIQUE_ACC(unit, mem)[j],
                                       COPYNO_ALL, TRUE));
                }
            }
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "mmu_mem %s, index %d will be "
                                    "cleared \n"),
                        SOC_MEM_NAME(unit, mem), index));
            if (index >= 0) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                                   entry));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, mem, COPYNO_ALL, TRUE));
            }
        }
    }
    return SOC_E_NONE;
}

int
soc_tomahawk2_clear_enabled_port_data (int unit)
{
    uint64 rval64;
    int    port;

    /* Some registers are implemented in memory, need to clear them in order
     * to have correct parity value */
    COMPILER_64_ZERO(rval64);
    PBMP_ALL_ITER(unit, port) {
        if (SOC_REG_IS_VALID(unit, EGR_1588_LINK_DELAY_64r)) {
            SOC_IF_ERROR_RETURN(WRITE_EGR_1588_LINK_DELAY_64r(unit, port, rval64));
        }
    }

    return SOC_E_NONE;
}

/*
 * FUNCTION : soc_th2_bypass_unused_pm
 * ARGS:
 *         unit    -    IN, unit number
 * Description :
 * Bypass unused port blocks, by programming TOP_TSC_ENABLEr and TOP_TSC_ENABLE_1r
 * appropriately.
 */
soc_error_t
soc_th2_bypass_unused_pm(int unit)
{
    int port, phy_port, pm, block, bn, i, port_macro;
    uint32 rval = 0, orval, xrval;
    soc_info_t *si = &SOC_INFO(unit);
    uint8 used_pm_map[_TH2_PBLKS_PER_DEV + 1]; /* #CLPORT + #XLPORT */
    soc_reg_t reg[2] = {TOP_TSC_ENABLEr, TOP_TSC_ENABLE_1r};
    soc_field_t reg_field[_TH2_PBLKS_PER_DEV] = {
        TSC_0_ENABLEf, TSC_1_ENABLEf, TSC_2_ENABLEf, TSC_3_ENABLEf,
        TSC_4_ENABLEf, TSC_5_ENABLEf, TSC_6_ENABLEf, TSC_7_ENABLEf,
        TSC_8_ENABLEf, TSC_9_ENABLEf, TSC_10_ENABLEf, TSC_11_ENABLEf,
        TSC_12_ENABLEf, TSC_13_ENABLEf, TSC_14_ENABLEf, TSC_15_ENABLEf,
        TSC_16_ENABLEf, TSC_17_ENABLEf, TSC_18_ENABLEf, TSC_19_ENABLEf,
        TSC_20_ENABLEf, TSC_21_ENABLEf, TSC_22_ENABLEf, TSC_23_ENABLEf,
        TSC_24_ENABLEf, TSC_25_ENABLEf, TSC_26_ENABLEf, TSC_27_ENABLEf,
        TSC_28_ENABLEf, TSC_29_ENABLEf, TSC_30_ENABLEf, TSC_31_ENABLEf,
        TSC_32_ENABLEf, TSC_33_ENABLEf, TSC_34_ENABLEf, TSC_35_ENABLEf,
        TSC_36_ENABLEf, TSC_37_ENABLEf, TSC_38_ENABLEf, TSC_39_ENABLEf,
        TSC_40_ENABLEf, TSC_41_ENABLEf, TSC_42_ENABLEf, TSC_43_ENABLEf,
        TSC_44_ENABLEf, TSC_45_ENABLEf, TSC_46_ENABLEf, TSC_47_ENABLEf,
        TSC_48_ENABLEf, TSC_49_ENABLEf, TSC_50_ENABLEf, TSC_51_ENABLEf,
        TSC_52_ENABLEf, TSC_53_ENABLEf, TSC_54_ENABLEf, TSC_55_ENABLEf,
        TSC_56_ENABLEf, TSC_57_ENABLEf, TSC_58_ENABLEf, TSC_59_ENABLEf,
        TSC_60_ENABLEf, TSC_61_ENABLEf, TSC_62_ENABLEf, TSC_63_ENABLEf
    };

    sal_memset(used_pm_map, 0, sizeof(used_pm_map));
    for (phy_port = 1; phy_port <= _TH2_PORTS_PER_DEV; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port > 0) {
            pm = si->port_serdes[port];
            if ((-1 != pm) && (0 == used_pm_map[pm])) {
                used_pm_map[pm] = 1;
            }
        }
    }

    for (i = 0; i < 2; i++) {
        SOC_IF_ERROR_RETURN(
            soc_reg32_get(unit, reg[i], REG_PORT_ANY, 0, &rval));
        orval = rval;
        /* Upper loop bound omits XLPORT block intentionally */
        for (port_macro = 0; port_macro < _TH2_PBLKS_PER_DEV / 2; port_macro++) {
            pm = port_macro + 32 * i;
            /* In Tomahawk2, there are 8 port macros (clport 6,7,24,25,38,39,56,57 )that
             * will driven directly from the system board and they will act as the
             * refclk master to drive the other instances.
             */
            if ((pm == 6 || pm == 7 || pm == 24 || pm == 25 ||
              pm == 38 ||pm == 39 || pm == 56 || pm == 57)) {
                continue;
            }
            /* If PM is not used, bypass it */
            if (0 == used_pm_map[pm]) {
                soc_reg_field_set(unit, reg[i], &rval, reg_field[pm], 0);
                phy_port = 1 + (pm * _TH_PORTS_PER_PBLK);
                block = SOC_PORT_BLOCK(unit, phy_port);
                /* Invalidate port block in soc info */
                if (SOC_BLK_CLPORT == SOC_BLOCK_INFO(unit, block).type) {
                    si->block_valid[block] = 0;
                }
            }
        }
        if (rval != orval) {
            /*Calculate different bits number between old and new*/
            xrval = rval ^ orval;
            for (bn = 0; xrval != 0; bn++) {
                xrval &= (xrval - 1);
            }

            /*Do two steps configuration change for the case of different bits number beyond 10*/
            if (bn > 10) {
                /*Change half of those different bits to new at first*/
                bn /= 2;
                xrval = rval ^ orval;
                do {
                    xrval &= (xrval-1);
                } while (bn-- > 0);
                xrval ^= rval;
                SOC_IF_ERROR_RETURN(
                    soc_reg32_set(unit, reg[i], REG_PORT_ANY, 0, xrval));
                sal_usleep(100000);
            }
            SOC_IF_ERROR_RETURN(
                    soc_reg32_set(unit, reg[i], REG_PORT_ANY, 0, rval));
            sal_usleep(100000);
        }
    }

    return SOC_E_NONE;
}

void
soc_tomahawk2_sbus_ring_map_config(int unit)
{
    /*
     * SBUS ring and block number:
     * ring 0: IP(1), LBPORT0(79), LBPORT1(80), LBPORT2(81), LBPORT3(82)
     * ring 1: EP(2)
     * ring 2: MMU_XPE(3), MMU_SC(4), MMU_GLB(5), MMU_SED(12)
     * ring 3: PM64XLPORT0(11), PM0(15), PM1(16), PM2(17), PM3(18), PM4(19), PM5(20), 
     *         PM6(21), PM7(22), PM8(23), PM9(24), PM10(25), PM11(26), PM12(27),
     *         PM13(28), PM14(29), PM15(30), PM16(31), PM17(32), PM18(33), PM19(34), 
     *         PM20(35), PM21(36), PM22(37), PM23(38), PM24(39), PM25(40), PM26(41), 
     *         PM27(42), PM28(43), PM29(44), PM30(45), PM31(46), CLPORT64(83)
     * ring 4: PM32(47), PM33(48), PM34(49), PM35(50), PM36(51),
     *         PM37(52), PM38(53), PM39(54), PM40(55), PM41(56), PM42(57), PM43(58),
     *         PM44(59), PM45(60), PM46(61), PM47(62), PM48(63), PM49(64), PM50(65),
     *         PM51(66), PM52(67), PM53(68), PM54(69), PM55(70), PM56(71), PM57(72),
     *         PM58(73), PM59(74), PM60(75), PM61(76), PM62(77), PM63(78)
     * ring 5: OTPC(6), OTPC1(13), AVS(87), TOP(7), SER(8)
     */
    WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x55222100);
    WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x30523005);
    WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x33333333);
    WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x33333333);
    WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x33333333);
    WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x43333333);
    WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0X44444444);
    WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x44444444);
    WRITE_CMIC_SBUS_RING_MAP_64_71r(unit, 0x44444444);
    WRITE_CMIC_SBUS_RING_MAP_72_79r(unit, 0x04444444);
    WRITE_CMIC_SBUS_RING_MAP_80_87r(unit, 0x50003000);
#ifdef FW_BCAST_DOWNLOAD
    /*
     * program SBUS agent with SBUS ID 126 to SBUS ring 3, SBUS ring 3 use 126 as its broadcast ID
     * program SBUS agent with SBUS ID 125 to SBUS ring 4, SBUS ring 4 use 125 as its broadcast ID
     */
    WRITE_CMIC_SBUS_RING_MAP_120_127r(unit, 0x03400000);
#endif
    if (!SAL_BOOT_QUICKTURN) {
        WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x1000);
    }

    return;
}

int
soc_tomahawk2_chip_reset(int unit)
{
#define _SOC_TH2_PLL_CFG_FIELDS 4
#define _SOC_TH2_BSPLL_COUNT    2
#define _SOC_TH2_HW_TEMP_MAX       125
#define _SOC_TH2_DEF_SW_TEMP_MAX   110

    soc_info_t *si;
    soc_reg_t reg;
    int freq_sel, max_freq_sel, def_freq_sel, freq_list_len, max_freq, def_freq;
    int def_dpp_ratio_x10, ratio_sel, ratio_list_len, field_count, pipe;
    uint32 to_usec, rval;
    uint32 sw_temp_mask, hw_temp_mask, temp_thr, sw_temp_thr, hw_temp_thr;
    soc_field_t fields[_SOC_TH2_PLL_CFG_FIELDS];
    uint32 values[_SOC_TH2_PLL_CFG_FIELDS];
    int num_banks, index, shared_bank, shared_bank_offset = 0;
    int num_shared_l2_banks, num_shared_l3_banks, num_shared_fpem_banks;
    uint32 config = 0, map = 0, fpmap = 0;
    uint32 pipe_map;
    int parity_enable;
    int num_share_banks_enabled = 4;
    int share_banks_bitmap_enabled = 0xf;    
    static soc_field_t l2_fields[] = {
        L2_ENTRY_BANK_2f, L2_ENTRY_BANK_3f, L2_ENTRY_BANK_4f,
        L2_ENTRY_BANK_5f
    };
    static soc_field_t l3_fields[] = {
        L3_ENTRY_BANK_4f, L3_ENTRY_BANK_5f, L3_ENTRY_BANK_6f,
        L3_ENTRY_BANK_7f
    };
    static soc_field_t fpem_fields[] = {
        FPEM_ENTRY_BANK_0f, FPEM_ENTRY_BANK_1f, FPEM_ENTRY_BANK_2f,
        FPEM_ENTRY_BANK_3f
    };
    static const soc_reg_t bspll_status_reg[] = {
        TOP_BS_PLL0_STATUSr, TOP_BS_PLL1_STATUSr,
    };
    static const soc_reg_t temp_thr_reg[] = {
        TOP_PVTMON_0_INTR_THRESHOLDr, TOP_PVTMON_1_INTR_THRESHOLDr,
        TOP_PVTMON_2_INTR_THRESHOLDr, TOP_PVTMON_3_INTR_THRESHOLDr,
        TOP_PVTMON_4_INTR_THRESHOLDr, TOP_PVTMON_5_INTR_THRESHOLDr,
        TOP_PVTMON_6_INTR_THRESHOLDr, TOP_PVTMON_7_INTR_THRESHOLDr
    };
    static const char *temp_thr_prop[] = {
        "temp0_threshold", "temp1_threshold", "temp2_threshold",
        "temp3_threshold", "temp4_threshold", "temp5_threshold",
        "temp6_threshold", "temp7_threshold"
    };
    static const int freq_list[] = { 1700, 1625, 1525, 1425, 1325, 1275,
                                     1225, 1125, 1050, 950, 850 };
    static const int dpp_ratio_x10_list[] = {
        15, /* core clk : pp clk ratio is 3:2 (default) */
        20, /* core clk : pp clk ratio is 2:1 */
        10  /* core clk : pp clk ratio is 1:1 */
    };
    const char *ratio_str[] = {"2:3", "1:2", "1:1"};
    uint16 dev_id;
    uint8 rev_id;
    

   /* Settings for 500 MHz TSPLL output clock. */
    unsigned ts_ref_freq = 0;
    unsigned ts_idx = 0;

    static const soc_pll_param_t ts_pll[] = {
    /*     Fref, Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
      {25000000,      100,         0,     1,    5, 10,  3, 10,       1},
      {50000000,      100,         0,     2,    5, 10,  3, 10,       1}, /* 50 MHz external reference. */
      {       0,      100,         0,     2,    5, 10,  3, 10,       1} /* 50 MHz internal reference. */
    };

    /* Settings for 20 MHz BSPLL output clock. */
    unsigned bs_ref_freq = 0;
    unsigned bs_idx = 0;

    static const soc_pll_param_t bs_pll[] = {
    /*     Fref, Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
      {12800000,      195,   0x50000,     1,  125, 10,  3, 10,      1},
      {20000000,      125,         0,     1,  125, 10,  3, 10,      1},
      {25000000,      100,         0,     1,  125, 10,  3, 10,      1},
      {32000000,       78,   0x20000,     1,  125, 10,  3, 10,      1},
      {50000000,      100,         0,     2,  125, 10,  3, 10,      1},
      {       0,      100,         0,     2,  125, 10,  3, 10,      1} /* 50 MHz internal reference. */
    };

    si = &SOC_INFO(unit);
    soc_cm_get_id(unit, &dev_id, &rev_id);
    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    soc_tomahawk2_sbus_ring_map_config(unit);

    sal_usleep(to_usec);

    field_count = 0;

    /* Core frequency */
    SOC_IF_ERROR_RETURN
        (soc_tomahawk2_frequency_get(unit, &max_freq, &def_freq));
    max_freq_sel = 0;
    def_freq_sel = 0;
    freq_list_len = COUNTOF(freq_list);
    for (freq_sel = 0; freq_sel < freq_list_len; freq_sel++) {
        if (max_freq == freq_list[freq_sel]) {
            max_freq_sel = freq_sel;
        }
        if (def_freq == freq_list[freq_sel]) {
            def_freq_sel = freq_sel;
        }
    }
    if ((si->frequency != def_freq) && (si->frequency < max_freq)) {
        for (freq_sel = max_freq_sel + 1; freq_sel < freq_list_len;
             freq_sel++) {
            if (si->frequency == freq_list[freq_sel]) {
                break;
            }
        }
        if (freq_sel < freq_list_len) {
            LOG_CLI((BSL_META_U(unit,
                                "*** change core clock frequency to %dMHz\n"),
                                si->frequency));
            fields[field_count] = CORE_CLK_0_FREQ_SELf;
            values[field_count] = freq_sel;
            field_count++;
        }
    }

    if (soc_feature(unit, soc_feature_untethered_otp)) {
        def_dpp_ratio_x10 = SOC_BOND_INFO(unit)->dpp_clk_ratio;
    } else {
        def_dpp_ratio_x10 = dpp_ratio_x10_list[0];
    }

    /* DPP ratio */
    if (si->dpp_clk_ratio_x10 != def_dpp_ratio_x10) {
        ratio_list_len = COUNTOF(dpp_ratio_x10_list);
        for (ratio_sel = 1; ratio_sel < ratio_list_len; ratio_sel++) {
            if (si->dpp_clk_ratio_x10 == dpp_ratio_x10_list[ratio_sel]) {
                break;
            }
        }
        if (ratio_sel < ratio_list_len) {
            LOG_CLI((BSL_META_U(unit,
                                "*** change dpp_clk : core_clk ratio to %s\n"),
                                ratio_str[ratio_sel]));
            ratio_sel = _soc_th2_dpp_ratio_sel(si->dpp_clk_ratio_x10);
            fields[field_count] = DPP_CLK_RATIO_SELf;
            values[field_count] = ratio_sel;
            field_count++;
        }
    }
    if (field_count) {
        fields[field_count] = SW_CORE_CLK_0_SEL_ENf;
        values[field_count] = 1;
        field_count++;
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, TOP_CORE_CLK_FREQ_SELr, REG_PORT_ANY,
                                    field_count, fields, values));
        sal_usleep(to_usec);
    } else {
        /* Set correct frequency in non OTP'd SKUs */
        SOC_IF_ERROR_RETURN(READ_TOP_DEV_REV_IDr(unit, &rval));
        if (!(soc_reg_field_get(unit, TOP_DEV_REV_IDr, rval, DEVICE_SKEWf) & 0x8)) {
            LOG_CLI((BSL_META_U(unit,
                                "*** change core clock frequency to %dMHz, "
                                "change dpp ratio to %s\n"),
                                def_freq, "2:3"));
            field_count = 0;
            fields[field_count] = CORE_CLK_0_FREQ_SELf;
            values[field_count] = def_freq_sel;
            field_count++;

            ratio_sel = _soc_th2_dpp_ratio_sel(dpp_ratio_x10_list[0]);
            fields[field_count] = DPP_CLK_RATIO_SELf;
            values[field_count] = ratio_sel;
            field_count++;

            fields[field_count] = SW_CORE_CLK_0_SEL_ENf;
            values[field_count] = 1;
            field_count++;
            SOC_IF_ERROR_RETURN
                (soc_reg_fields32_modify(unit, TOP_CORE_CLK_FREQ_SELr,
                                    REG_PORT_ANY, field_count, fields, values));
            sal_usleep(to_usec);
        }
    }

    /* Configure TS PLL */
    ts_ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0);  /* 0->internal reference */

    for (ts_idx = 0; ts_idx < sizeof(ts_pll)/sizeof(ts_pll[0]); ++ts_idx) {
        if (ts_pll[ts_idx].ref_freq == ts_ref_freq) {
            break;
        }
    }

    if (ts_idx == sizeof(ts_pll)/sizeof(ts_pll[0])) {
        /* Valid configuration lookup failed. Use internal reference. */
        ts_idx = 5;
        ts_ref_freq = 0;
    }

    /* Do not change PLL settings for internal reference */
    if(ts_ref_freq) {
        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, REFCLK_SOURCE_SELf, (ts_ref_freq != 0));
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_3r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_0r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_0r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_MNDIV, ts_pll[ts_idx].mdiv));
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_0r(unit, rval));


        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_1r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_PDIV, ts_pll[ts_idx].pdiv));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_1r, &rval, NDIV_INTf,
                          soc_property_get(unit, spn_PTP_TS_PLL_N, ts_pll[ts_idx].ndiv_int));
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_1r(unit, rval));


        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_2r(unit,&rval));
        /* TBD */
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_2r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_4r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_TS_KA, ts_pll[ts_idx].ka));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_TS_KI, ts_pll[ts_idx].ki));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_TS_KP, ts_pll[ts_idx].kp));
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_4r(unit, rval));
    }

    /* Set 250Mhz (implies 4ns resolution) default timesync clock to
       calculate assymentric delays */
    SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nanoseconds */

    /* Configure BroadSync PLLs. */
    bs_ref_freq = soc_property_get(unit, spn_PTP_BS_FREF, 0);  /* 0->internal reference */
    
    for (bs_idx = 0; bs_idx < sizeof(bs_pll)/sizeof(bs_pll[0]); ++bs_idx) {
        if (bs_pll[bs_idx].ref_freq == bs_ref_freq) {
            break;
        }
    }
    if (bs_idx == sizeof(bs_pll)/sizeof(bs_pll[0])) {
        /* Valid configuration lookup failed. Use internal reference. */
        bs_idx = 5;
        bs_ref_freq = 0;
    }

    if (bs_ref_freq) {
        /* PLL settings for external reference */
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, REFCLK_SOURCE_SELf, (bs_ref_freq != 0));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_3r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_0r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_0r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_BS_MNDIV, bs_pll[bs_idx].mdiv));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_0r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_BS_PDIV, bs_pll[bs_idx].pdiv));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, NDIV_INTf,
                          soc_property_get(unit, spn_PTP_BS_NDIV_INT, bs_pll[bs_idx].ndiv_int));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_1r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_2r(unit, &rval));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_2r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_4r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_BS_KA, bs_pll[bs_idx].ka));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_BS_KI, bs_pll[bs_idx].ki));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_BS_KP, bs_pll[bs_idx].kp));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_4r(unit, rval));

    /* Configure BS PLL1 */
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_3r, &rval, REFCLK_SOURCE_SELf, (bs_ref_freq != 0));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_3r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_0r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_0r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_BS_MNDIV, bs_pll[bs_idx].mdiv));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_0r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_BS_PDIV, bs_pll[bs_idx].pdiv));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, NDIV_INTf,
                          soc_property_get(unit, spn_PTP_BS_NDIV_INT, bs_pll[bs_idx].ndiv_int));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_1r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_2r(unit, &rval));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_2r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_4r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_4r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_BS_KA, bs_pll[bs_idx].ka));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_4r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_BS_KI, bs_pll[bs_idx].ki));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_4r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_BS_KP, bs_pll[bs_idx].kp));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_4r(unit, rval));

    }
 /* Bring XG PLL0, TS PLL, BS PLL0/1, AVS out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_5r(unit, &rval));
    soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, ROUTE_CTRLf, 0);
    soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, EN_CTRLf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL0_CTRL_5r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL0_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_RST_Lf,
                      1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    
    sal_usleep(to_usec);
    
    if (!SAL_BOOT_SIMULATION) {
        /* Check XG PLL0 lock status */
        reg = TOP_XG_PLL0_STATUSr;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (!soc_reg_field_get(unit, reg, rval, TOP_XGPLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "XG_PLL0 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
        
        /* Check TS PLL lock status */
        reg = TOP_TS_PLL_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (!soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TS_PLL not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
        
        /* Check BS PLL lock status */
        for (index = 0; index < _SOC_TH2_BSPLL_COUNT; index++) {
            reg = bspll_status_reg[index];
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            if (!soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "BS_PLL%d not locked on unit %d "
                                      "status = 0x%08x\n"), index, unit, rval));
            }
        }

        /* Check Core PLL lock status, only valid for TH2 B0 revision */
        if (((BCM56970_DEVICE_ID & 0xFFF0) == (dev_id & 0xFFF0)) &&
            (BCM56970_B0_REV_ID == rev_id)) {
            reg = TOP_CORE_PLL0_STATUSr;
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            if (!soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "CORE_PLL not locked on unit %d "
                                    "status = 0x%08x\n"), unit, rval));
            }
        }
    }
    
    /* De-assert XG PLL0, TS PLL, BS PLL0/1 post reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_XG_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TS_PLL_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL1_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    
    sal_usleep(to_usec);

    if (rev_id == BCM56970_A0_REV_ID) {
        /* TH2 A0 need to set correct value 0 for BG_ADJ, 
         * TH2 B0 default value has been changed to 0.
         */
        SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_0r(unit, &rval));
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_0r, &rval, BG_ADJf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_0r(unit, rval));
    }

    /* Check the the sticky bit whether HW PVTMON High Temperature Protection 
    * happened in system's previous running or not. Halt the current bootup, print error  
    * message, and ask for a system reboot if the sticky bit was set. */
    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_RESULT_0r(unit, &rval));
    if (soc_reg_field_get(unit, TOP_PVTMON_RESULT_0r, 
                          rval, PVTMON_HIGHTEMP_STATUSf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "HW PVTMON High Temperature Protection "
                              "was invoked during last system run. "
                              "Please correct the temperature problem "
                              "and cycle power to continue.\n")));
        (void)soc_event_generate(unit, SOC_SWITCH_EVENT_ALARM,
                                 SOC_SWITCH_EVENT_ALARM_125C_TEMP, 
                                 -1, -1);
        return SOC_E_FAIL;
    }

    /* Enable high temperature interrupt monitoring.
     * Default on: pvtmon5 (close to core_plls at center of die).
     * Default threshold for pvtmon5: 110C.     
     * Enable HW PVTMON High Temperature Protection.
     * Default on: pvtmon[0-4, 6-7] .
     * Default threshold : 125C.   */
    sw_temp_mask = soc_property_get(unit, "temp_monitor_select", 1<<5);
    sw_temp_mask = sw_temp_mask & ((1 << 8) - 1);
    hw_temp_mask = soc_property_get(unit, "hw_temp_monitor_select", 
                                    ((1 << 8) - 1));
    hw_temp_mask = hw_temp_mask & ((1 << 8) - 1);
    hw_temp_mask = hw_temp_mask & (~ sw_temp_mask);
    /* sw_temp_mask is a 8 bit mask to indicate which temp sensor to hook up to
     * the interrupt. */

    sw_temp_thr = soc_property_get(unit, spn_SW_TEMP_THRESHOLD, 
                                   _SOC_TH2_DEF_SW_TEMP_MAX);
    if (sw_temp_thr > _SOC_TH2_DEF_SW_TEMP_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Unsupported temp value %d !! Max %d. Using %d.\n"),
                  sw_temp_thr, _SOC_TH2_DEF_SW_TEMP_MAX, _SOC_TH2_DEF_SW_TEMP_MAX));
        sw_temp_thr = _SOC_TH2_DEF_SW_TEMP_MAX;
    }
    
    rval = 0;
    for (index = 0; index <COUNTOF(temp_thr_reg); index++) {
        uint32 trval;

        /* Temp = 434.10 - o_ADC_data * 0.53504
         * data = (434.10 - temp)/0.53504 = (434100-(temp*1000))/535
         * Note: Since this is a high temp value we can safely assume it to
         * always be a +ive number. This is in degrees celcius.
         */
        if (hw_temp_mask & (1 << index)) {
            hw_temp_thr = soc_property_get(unit, temp_thr_prop[index], 
                                           _SOC_TH2_HW_TEMP_MAX);
            if (hw_temp_thr > _SOC_TH2_HW_TEMP_MAX) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Unsupported temp value %d !! Max %d. "
                                      "Using %d.\n"), hw_temp_thr, 
                                      _SOC_TH2_HW_TEMP_MAX, 
                                      _SOC_TH2_HW_TEMP_MAX));
                hw_temp_thr = _SOC_TH2_HW_TEMP_MAX;
            }
            temp_thr = hw_temp_thr;
        } else {
            temp_thr = sw_temp_thr;
        }
        /* Convert temperature to config data */
        temp_thr = (434100-(temp_thr*1000))/535;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, temp_thr_reg[index], REG_PORT_ANY, 0, &trval));
        soc_reg_field_set(unit, temp_thr_reg[index], &trval, MIN_THRESHOLDf,
                          temp_thr);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, temp_thr_reg[index], REG_PORT_ANY, 0, trval));
        if (sw_temp_mask & (1 << index)) {
            rval |= (1 << ((index * 2) + 1)); /* 2 bits per pvtmon, using min */
        }
    }
    _soc_th_temp_mon_mask[unit] = sw_temp_mask;

    /* Enable high temperature interrupt monitoring.*/
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_INTR_MASKr(unit, rval));

    /* Enable HW PVTMON High Temperature Protection */
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    rval = (rval & (~((1 << 8) - 1))) | hw_temp_mask;
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));


    /* Enable temp mon interrupt */
    (void)soc_cmicm_intr3_enable(unit, 0x4); /* PVTMON_INTR bit 2 */

    /* Bring port blocks out of reset */
    rval = 0;
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_PM64_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, 0xffffffff));
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_4r(unit, 0xffffffff));
    sal_usleep(to_usec);

    /* Per DE, need set PSG/PCG to 0 before de-assert IP/EP/MMU SOFT reset */
    SOC_IF_ERROR_RETURN(WRITE_TOP_CLOCKING_ENFORCE_PSGr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_TOP_CLOCKING_ENFORCE_PCGr(unit, 0));

    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    /* Enable IS_TDM_CALENDAR0,1 SER protection. 
    * They can't be auto-generated by regFile.*/
    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    if (parity_enable) {	
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, IDB_SER_CONTROLr,
                                                   REG_PORT_ANY,
                                                   IS_TDM_ECC_ENf,1));
    }

    SOC_IF_ERROR_RETURN(_soc_tomahawk2_init_idb_memory(unit));

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    /* Enable scheduler for SBUS accesses to PP IPIPE */
    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        reg = SOC_REG_UNIQUE_ACC(unit, IS_TDM_CONFIGr)[pipe];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

        reg = SOC_REG_UNIQUE_ACC(unit, IS_OPP_SCHED_CFGr)[pipe];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, OPP1_PORT_ENf, 1);
        soc_reg_field_set(unit, reg, &rval, OPP1_SPACINGf, 6);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    /* Must do this before setting low-latency mode */
    SOC_IF_ERROR_RETURN(_soc_tomahawk2_init_ipep_memory(unit));
    SOC_IF_ERROR_RETURN(_soc_tomahawk2_clear_l2_mod_fifo(unit));
    /* Configure Switch Latency Bypass Mode */
    SOC_IF_ERROR_RETURN(soc_th_latency_init(unit));
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        soc_th2_uft_otp_info_get(unit, &num_share_banks_enabled,
            &share_banks_bitmap_enabled);  
    }

    if (soc_feature(unit, soc_feature_untethered_otp)
        && num_share_banks_enabled < 4) {
        SOC_IF_ERROR_RETURN
            (soc_th2_set_uft_bank_map(unit));    
    } else {

        SOC_IF_ERROR_RETURN
            (soc_tomahawk_hash_bank_count_get(unit, L2Xm, &num_banks));
        num_shared_l2_banks = num_banks - 2; /* minus 2 dedicated L2 banks */
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_hash_bank_count_get(unit, L3_ENTRY_ONLYm, &num_banks));
        num_shared_l3_banks = num_banks - 4; /* minus 4 dedicated L3 banks */
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_hash_bank_count_get(unit, EXACT_MATCH_2m,
                                              &num_shared_fpem_banks));

        if (soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m)) {
            int non_alpm = num_shared_fpem_banks + num_shared_l3_banks +
                num_shared_l2_banks;
            if ((non_alpm) && (non_alpm <= 2)) {
                /* If Shared banks are used between ALPM and non-ALPM memories,
                 * then ALPM uses Shared Bank B2, B3 and non-ALPM uses B4, B5 banks
                 */
                soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                                  ALPM_ENTRY_BANK_CONFIGf, 0x3);
                soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                                  ALPM_BANK_MODEf, 1);
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, ISS_LOG_TO_PHY_BANK_MAP_2r,
                                            REG_PORT_ANY, ALPM_BANK_MODEf, 1));
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, ISS_MEMORY_CONTROL_84r,
                                            REG_PORT_ANY, BYPASS_ISS_MEMORY_LPf, 0x3));
                soc_th_set_alpm_banks(unit, 2);
                shared_bank_offset = 2;
            } else {
                soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                                  ALPM_ENTRY_BANK_CONFIGf, 0xf);
                soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                                  ALPM_BANK_MODEf, 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, ISS_LOG_TO_PHY_BANK_MAP_2r,
                                            REG_PORT_ANY, ALPM_BANK_MODEf, 0));
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, ISS_MEMORY_CONTROL_84r,
                                            REG_PORT_ANY, BYPASS_ISS_MEMORY_LPf, 0xf));
                soc_th_set_alpm_banks(unit, 4);
            }
        }

        soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config, L2_ENTRY_BANK_CONFIGf,
                          ((1 << num_shared_l2_banks) - 1) << shared_bank_offset);
        for (index = 0; index < num_shared_l2_banks; index++) {
            shared_bank = index + shared_bank_offset;
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                              l2_fields[index], shared_bank);
        }
        shared_bank_offset += num_shared_l2_banks;

        soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config, L3_ENTRY_BANK_CONFIGf,
                          ((1 << num_shared_l3_banks) - 1) << shared_bank_offset);
        for (index = 0; index < num_shared_l3_banks; index++) {
            shared_bank = index + shared_bank_offset;;
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                              l3_fields[index], shared_bank);
        }
        shared_bank_offset += num_shared_l3_banks;

        soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                          FPEM_ENTRY_BANK_CONFIGf,
                          ((1 << num_shared_fpem_banks) - 1) << shared_bank_offset);
        for (index = 0; index < num_shared_fpem_banks; index++) {
            shared_bank = index + shared_bank_offset;
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                              fpem_fields[index], shared_bank);
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAP_3r, &fpmap,
                              fpem_fields[index], shared_bank);
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, ISS_BANK_CONFIGr, REG_PORT_ANY, 0, config));
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, REG_PORT_ANY, 0, map));
        SOC_IF_ERROR_RETURN
            (soc_th_iss_log_to_phy_bank_map_shadow_set(unit, map));
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, ISS_LOG_TO_PHY_BANK_MAP_3r, REG_PORT_ANY, 0, fpmap));
    }
    /* Bypass unused Port Blocks */
    if (soc_property_get(unit, "disable_unused_port_blocks", TRUE)) {
        SOC_IF_ERROR_RETURN(soc_th2_bypass_unused_pm(unit));
    }

    if (soc_feature(unit, soc_feature_turbo_boot)) {
        if (soc_property_get(unit, "clear_on_hw_resetting", 
                             (!(SOC_CONTROL(unit)->soc_flags &
                                SOC_F_ALL_MODULES_INITED) ||
                              SAL_BOOT_BCMSIM))) {
            SOC_HW_RESET_START(unit);
        }
    }

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      soc_tomahawk2_chip_warmboot_reset
 * Purpose:
 *      Special re-init sequencing for BCM56970 during warmboot
 */
int
soc_tomahawk2_chip_warmboot_reset(int unit)
{
    uint32 map = 0;

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, ISS_LOG_TO_PHY_BANK_MAPr, REG_PORT_ANY, 0, &map));
    SOC_IF_ERROR_RETURN
        (soc_th_iss_log_to_phy_bank_map_shadow_set(unit, map));

    return SOC_E_NONE;
}
#endif

int
soc_tomahawk2_sed_base_index_check(int unit, int base_type, int sed,
                                 int base_index, char *msg)
{
    soc_info_t *si;
    int pipe;
    uint32 map;
    char *base_name;

    si = &SOC_INFO(unit);

    if (sed == -1 || base_index == -1) {
        return SOC_E_NONE;
    }

    if (sed < NUM_SED(unit) && si->sed_ipipe_map[sed] == 0) {
        if (msg) {
            LOG_CLI((BSL_META_U(unit,
                                "%s: SED%d is not in config\n"),
                     msg, sed));
        }
        return SOC_E_PARAM;
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        base_name = base_type == 0 ? "IPORT" : "EPORT";
        pipe = si->port_pipe[base_index];
        if (pipe == -1) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (sed < NUM_SED(unit)) { /* Unique access type */
            map = base_type == 0 ?
                si->ipipe_sed_map[pipe] : si->epipe_sed_map[pipe];
            if (map & (1 << sed)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in SED%d\n"),
                         msg, base_name, base_index, sed));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (base_type == 2) {
            base_name = "IPIPE";
            map = si->ipipe_sed_map[base_index];
        } else {
            base_name = "EPIPE";
            map = si->epipe_sed_map[base_index]; 
        }
        if (map == 0) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (sed < NUM_SED(unit)) { /* Unique access type */
            if (map & (1 << sed)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in SED%d\n"),
                         msg, base_name, base_index, sed));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 4: /* CHIP */
        break;
    case 5: /* XPE */
        if (si->xpe_ipipe_map[base_index] == 0) {
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: XPE%d is not in config\n"),
                         msg, base_index));
            }
        } else if (sed < NUM_SED(unit)) { /* Unique access type */
            if (sed == 0) { /* XPE 0 and 2 are in slice 0 */
                if (base_index == 0 || base_index == 2) {
                    break;
                }
            } else { /* XPE 1 and 3 are in slice 1 */
                if (base_index == 1 || base_index == 3) {
                    break;
                }
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: XPE%d is not in SLICE%d\n"),
                         msg, base_index, sed));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 6: /* SLICE */
    case 7: /* LAYER, not used */
        break;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk2_sed_reg_check(int unit, soc_reg_t reg, int sed, int base_index)
{
    int acc_type, base_type;

    if (!SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg).block, SOC_BLK_MMU_SED)) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not SED register\n"), SOC_REG_NAME(unit, reg)));
        return SOC_E_PARAM;
    }

    if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL) { /* UNIQUE base register */
        if (sed == -1 || sed >= NUM_SED(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad SED value %d\n"),
                     SOC_REG_NAME(unit, reg), sed));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_REG_ACC_TYPE(unit, reg);
        if (acc_type < NUM_SED(unit)) { /* UNIQUE per SED register */
            if (sed != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Override SED value %d with ACC_TYPE of %s\n"),
                         sed, SOC_REG_NAME(unit, reg)));
            }
            sed = acc_type;
        } else { /* non-UNIQUE register */
            return SOC_E_NONE;
        }
    }
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;

    return soc_tomahawk2_sed_base_index_check(unit, base_type, sed, base_index,
                                            SOC_REG_NAME(unit, reg));
}

STATIC int
_soc_tomahawk2_sed_reg_access(int unit, soc_reg_t reg, int sed, int base_index,
                            int index, uint64 *data, int write)
{
    soc_info_t *si;
    int port;
    int base_type;
    uint32 base_index_map;
    soc_pbmp_t base_index_pbmp;
    uint32 inst;

    si = &SOC_INFO(unit);
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;

    if (sed >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk2_sed_reg_check(unit, reg, sed, base_index));
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            SOC_PBMP_ASSIGN(base_index_pbmp, PBMP_ALL(unit));
        } else {
            /* This argument is logical port, same as soc_reg_get/set */
            SOC_PBMP_PORT_SET(base_index_pbmp, base_index);
        }

        SOC_PBMP_ITER(base_index_pbmp, port) {
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, index, data));
            }
        }
        break;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        /* No unique access type for such base_type */
        soc_tomahawk_pipe_map_get(unit, &base_index_map);
        if (base_index != -1) {
            base_index_map &= 1 << base_index;
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        for (base_index = 0; base_index < NUM_PIPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    case 4: /* CHIP */
        if (write) {
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, 0, index, *data));
        } else {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, 0, index, data));
        }
        break;
    case 5: /* XPE */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        } else {
            base_index_map = 1 << base_index;
        }

        for (base_index = 0; base_index < NUM_XPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    case 6: /* SLICE */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = si->ipipe_sed_map[0];;
        } else {
            base_index_map = 1 << base_index;
        }
        for (base_index = 0; base_index < NUM_SED(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    case 7: /* LAYER */
    default:
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

int
soc_tomahawk2_sed_reg32_set(int unit, soc_reg_t reg, int sed, int base_index,
                          int index, uint32 data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk2_sed_reg_access(unit, reg, sed, base_index,
                                                    index, &data64, TRUE));

    return SOC_E_NONE;
}

int
soc_tomahawk2_sed_reg32_get(int unit, soc_reg_t reg, int sed, int base_index,
                          int index, uint32 *data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, *data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk2_sed_reg_access(unit, reg, sed, base_index,
                                                    index, &data64, FALSE));
    *data = COMPILER_64_LO(data64);

    return SOC_E_NONE;
}

int
soc_tomahawk2_sed_reg_set(int unit, soc_reg_t reg, int sed, int base_index,
                        int index, uint64 data)
{
    return _soc_tomahawk2_sed_reg_access(unit, reg, sed, base_index,
                                       index, &data, TRUE);
}

int
soc_tomahawk2_sed_reg_get(int unit, soc_reg_t reg, int sed, int base_index,
                        int index, uint64 *data)
{
    return _soc_tomahawk2_sed_reg_access(unit, reg, sed, base_index,
                                       index, data, FALSE);
}

soc_pstats_tbl_desc_t
_soc_th2_pstats_tbl_desc_all = {SOC_BLK_NONE, INVALIDm, FALSE, FALSE};
soc_pstats_tbl_desc_t _soc_th2_pstats_tbl_desc[] = {
    /* List all the potential memories */
#ifdef BCM_PSTATS_MEASURE
    {
        SOC_BLK_MMU_GLB,
        INVALIDm,
        FALSE, FALSE,
        {
            {MMU_INTFO_TIMESTAMPm},
            {INVALIDm}
        }
    },
#endif
    {
        SOC_BLK_MMU_GLB,
        INVALIDm,
        FALSE, FALSE,
        {
            {MMU_INTFO_UTC_TIMESTAMPm},
            {INVALIDm}
        }
    },
    {
        SOC_BLK_MMU_XPE,
        MMU_THDU_UCQ_STATS_TABLEm,
        TRUE, TRUE
    },
    {
        SOC_BLK_MMU_XPE,
        THDI_PKT_STAT_SP_SHARED_COUNTm,
        FALSE, FALSE
    },
    {
        SOC_BLK_MMU_XPE,
        MMU_THDM_DB_POOL_MCUC_PKSTATm,
        FALSE, FALSE
#ifdef BCM_PSTATS_MEASURE
    },
    {
        SOC_BLK_MMU_GLB,
        INVALIDm,
        FALSE, FALSE,
        {
            {MMU_INTFO_TIMESTAMPm},
            {INVALIDm}
        }
#endif
    }
};

void
soc_tomahawk2_mmu_pstats_tbl_config_all(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_mem_desc_t *desc, *desc_all;
    int ti, mi, di = 0;

    desc_all = _soc_th2_pstats_tbl_desc_all.desc;
    /* Check and strip invalid memories */
    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        mi = 0;
        desc = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].desc;
        while (desc[mi].mem != INVALIDm) {
            desc_all[di] = desc[mi];
            di++;
            mi++;
        }
    }
    desc_all[di].mem = INVALIDm;

    soc->pstats_tbl_desc = &_soc_th2_pstats_tbl_desc_all;
    soc->pstats_tbl_desc_count = 1;
}

void
soc_tomahawk2_mmu_pstats_tbl_config(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_mem_desc_t *desc;
    soc_mem_t bmem;
    int pe, mor_dma;
    int ti, mi, xpe, pipe;

    soc->pstats_tbl_desc = _soc_th2_pstats_tbl_desc;
    soc->pstats_tbl_desc_count = COUNTOF(_soc_th2_pstats_tbl_desc);
    soc->pstats_mode = PSTATS_MODE_NULL;

    /* Check and strip invalid memories */
    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        mi = 0;
        desc = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].desc;
        bmem = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].bmem;
        if (bmem == INVALIDm) {
            while (desc[mi].mem != INVALIDm) {
                desc[mi].width = soc_mem_entry_words(unit, desc[mi].mem);
                desc[mi].entries = soc_mem_index_count(unit, desc[mi].mem);
                mi++;
            }
            continue;
        }
        pe = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].pipe_enum;
        mor_dma = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].mor_dma;
        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                desc[mi].mem = pe ? SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, bmem, xpe, pipe) :
                                    SOC_MEM_UNIQUE_ACC(unit, bmem)[xpe];
                if (desc[mi].mem == INVALIDm) {
                    continue;
                }
                desc[mi].width = soc_mem_entry_words(unit, desc[mi].mem);
                desc[mi].entries = soc_mem_index_count(unit, desc[mi].mem);
                if (mor_dma) {
                    desc[mi].mor_dma = TRUE;
                }
                mi++;
                if (!pe) {
                    break;
                }
            }
        }
        desc[mi].mem = INVALIDm;
    }

    if (soc_property_get(unit, "pstats_desc_all", 1)) {
        /* Use single desc chain */
        soc_tomahawk2_mmu_pstats_tbl_config_all(unit);
    }
}

int
soc_tomahawk2_mmu_pstats_mode_set(int unit, uint32 flags)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int pf = 0;
    uint32 rval = 0;
    int rv;

    SOC_PSTATS_LOCK(unit);

    if (flags & _TH2_MMU_PSTATS_HWM_MOD) {
        soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval,
                          HWM_MODEf, 1);
        pf |= _TH2_MMU_PSTATS_HWM_MOD;
        if (flags & _TH2_MMU_PSTATS_RESET_ON_READ) {
            soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval,
                              HWM_RESET_ON_READf, 1);
            pf |= _TH2_MMU_PSTATS_RESET_ON_READ;
        }
    }

    if (flags & _TH2_MMU_PSTATS_PKT_MOD) {
        soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval,
                          SELECTf, 1);
        pf |= _TH2_MMU_PSTATS_PKT_MOD;
        if (soc_feature(unit, soc_feature_mor_dma) &&
            (flags & _TH2_MMU_PSTATS_MOR_EN)) {
            soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval,
                              MOR_ENf, 1);
        }
        if (flags & _TH2_MMU_PSTATS_MOR_EN) {
            pf |= _TH2_MMU_PSTATS_MOR_EN;
        }
    }

    if (flags & _TH2_MMU_PSTATS_ENABLE) {
        soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval,
                          ENABLEf, 1);
        pf |= _TH2_MMU_PSTATS_ENABLE;
        if (flags & _TH2_MMU_PSTATS_PKT_MOD) {
            if ((flags & _TH2_MMU_PSTATS_SYNC) ||
                (flags & _TH2_MMU_PSTATS_HWM_MOD &&
                 flags & _TH2_MMU_PSTATS_RESET_ON_READ)) {
                soc->pstats_mode = PSTATS_MODE_PKT_SYNC;
                pf |= _TH2_MMU_PSTATS_SYNC;
            } else {
                soc->pstats_mode = PSTATS_MODE_PKT_BUFF;
            }
        } else {
            soc->pstats_mode = PSTATS_MODE_OOB;
        }
    } else {
        soc->pstats_mode = PSTATS_MODE_NULL;
    }

    soc->pstats_flags = pf;

    rv = WRITE_MMU_GCFG_PKTSTAT_OOBSTATr(unit, rval);

    SOC_PSTATS_UNLOCK(unit);

    return rv;
}

int
soc_tomahawk2_mmu_pstats_mode_get(int unit, uint32 *flags)
{
    int pf = 0;
    uint32 rval = 0;
    int rv;
    soc_reg_t reg = MMU_GCFG_PKTSTAT_OOBSTATr;

    SOC_PSTATS_LOCK(unit);

    rv = READ_MMU_GCFG_PKTSTAT_OOBSTATr(unit, &rval);
    if (SOC_FAILURE(rv)) {
        SOC_PSTATS_UNLOCK(unit);
        return rv;
    }

    if (soc_reg_field_get(unit, reg, rval, ENABLEf)) {
        pf |= _TH2_MMU_PSTATS_ENABLE;
        if (soc_reg_field_get(unit, reg, rval, SELECTf)) {
            pf |= _TH2_MMU_PSTATS_PKT_MOD;
        }
    }
    if (soc_reg_field_get(unit, reg, rval, HWM_MODEf)) {
        pf |= _TH2_MMU_PSTATS_HWM_MOD;
        if (soc_reg_field_get(unit, reg, rval, HWM_RESET_ON_READf)) {
            pf |= _TH2_MMU_PSTATS_RESET_ON_READ;
        }
    }

    SOC_PSTATS_UNLOCK(unit);

    *flags = pf;

    return SOC_E_NONE;
}

int
soc_tomahawk2_mmu_pstats_mor_enable(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 rval;
    sal_usecs_t stime = sal_time_usecs();


    if (soc->pstats_flags & _TH2_MMU_PSTATS_MOR_EN) {
        SOC_IF_ERROR_RETURN(READ_MMU_GCFG_PKTSTAT_OOBSTATr(unit, &rval));
        if ( ! soc_reg_field_get(unit, 
                                 MMU_GCFG_PKTSTAT_OOBSTATr, rval, MOR_ENf)) {
            soc_reg_field_set(unit, 
                              MMU_GCFG_PKTSTAT_OOBSTATr, &rval, MOR_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_PKTSTAT_OOBSTATr(unit, rval));
        }
    }

    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "pstats dma pc done in %d usecs\n"),
              SAL_USECS_SUB(sal_time_usecs(), stime)));

    return SOC_E_NONE;
}

int
soc_tomahawk2_mmu_pstats_mor_disable(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 rval;
    sal_usecs_t stime = sal_time_usecs();

    if (!soc_feature(unit, soc_feature_mor_dma) &&
        (soc->pstats_flags & _TH2_MMU_PSTATS_MOR_EN)) {
        SOC_IF_ERROR_RETURN(READ_MMU_GCFG_PKTSTAT_OOBSTATr(unit, &rval));
        soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval, MOR_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_PKTSTAT_OOBSTATr(unit, rval));
    }

    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "pstats dma cb done in %d usecs\n"),
              SAL_USECS_SUB(sal_time_usecs(), stime)));

    return SOC_E_NONE;
}

int
soc_tomahawk2_mmu_splitter_reset(int unit)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_MMU_GCFG_SPLITTERr(unit, &rval));
    soc_reg_field_set(unit, MMU_GCFG_SPLITTERr, &rval, RESET_SBUSf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_SPLITTERr(unit, rval));

    return SOC_E_NONE;
}

#define TSC_REG_ADDR_TSCID_SET(_phy_reg, _phyad) \
                            ((_phy_reg) |= ((_phyad) & 0x1f) << 19)

int
soc_tomahawk2_mdio_addr_to_port(uint32 phy_addr)
{
    int bus, offset;

    /* Must be internal MDIO address */
    if ((phy_addr & 0x80) == 0) {
        return 0;
    }

    /*
     * Internal phy address:
     * bus 0 phy 1 to 28 are mapped to Physical port 1 to 28
     * bus 1 phy 1 to 28 are mapped to Physical port 29 to 56
     * bus 2 phy 1 to 16 are mapped to Physical port 57 to 72
     * bus 3 phy 1 to 28 are mapped to Physical port 73 to 100
     * bus 4 phy 1 to 28 are mapped to Physical port 101 to 128
     * bus 5 phy 1 to 28 are mapped to Physical port 129 to 156
     * bus 6 phy 1 to 28 are mapped to Physical port 157 to 184
     * bus 7 phy 1 to 16 are mapped to Physical port 185 to 200
     * bus 8 phy 1 to 28 are mapped to Physical port 201 to 228
     * bus 9 phy 1 to 28 are mapped to Physical port 229 to 256
     * bus 10 phy 1 is mapped to Physical port 257 and 
     *        phy 3 is mapped to Physical port 259
     */
    bus = PHY_ID_BUS_NUM(phy_addr);
    if (bus > 10) {
        return 0;
    }

    switch (bus) {
    case 0: offset = 0;
        break;
    case 1: offset = 28;
        break;
    case 2: offset = 56;
        break;
    case 3: offset = 72;
        break;
    case 4: offset = 100;
        break;
    case 5: offset = 128;
        break;
    case 6: offset = 156;
        break;
    case 7: offset = 184;
        break;
    case 8: offset = 200;
        break;
    case 9: offset = 228;
        break;
    case 10: offset = 256;
        if ((phy_addr & 0x1f) == 2 || (phy_addr & 0x1f) > 3) {
            return 0;
        }
        break;
    default:
        return 0;
    }

    return (phy_addr & 0x1f) + offset;
}

int
soc_th2_ledup_init(int unit)
{
    soc_reg_t led_reg[][16] = {
        {
            CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r,
            CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r
        },
        {
            CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_24_27r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_28_31r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_32_35r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_36_39r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_40_43r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_44_47r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_48_51r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_52_55r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_56_59r,
            CMIC_LEDUP1_PORT_ORDER_REMAP_60_63r
        },
        {
            CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_24_27r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_28_31r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_32_35r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_36_39r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_40_43r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_44_47r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_48_51r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_52_55r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_56_59r,
            CMIC_LEDUP2_PORT_ORDER_REMAP_60_63r
        },
        {
            CMIC_LEDUP3_PORT_ORDER_REMAP_0_3r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_4_7r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_8_11r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_12_15r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_16_19r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_20_23r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_24_27r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_28_31r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_32_35r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_36_39r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_40_43r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_44_47r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_48_51r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_52_55r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_56_59r,
            CMIC_LEDUP3_PORT_ORDER_REMAP_60_63r
        },
        {
            CMIC_LEDUP4_PORT_ORDER_REMAP_0_3r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_4_7r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_8_11r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_12_15r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_16_19r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_20_23r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_24_27r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_28_31r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_32_35r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_36_39r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_40_43r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_44_47r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_48_51r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_52_55r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_56_59r,
            CMIC_LEDUP4_PORT_ORDER_REMAP_60_63r
        }
    };
    soc_field_t led_port[] = {
         REMAP_PORT_0f, REMAP_PORT_1f, REMAP_PORT_2f, REMAP_PORT_3f,
         REMAP_PORT_4f, REMAP_PORT_5f, REMAP_PORT_6f, REMAP_PORT_7f,
         REMAP_PORT_8f, REMAP_PORT_9f, REMAP_PORT_10f, REMAP_PORT_11f,
         REMAP_PORT_12f, REMAP_PORT_13f, REMAP_PORT_14f, REMAP_PORT_15f,
         REMAP_PORT_16f, REMAP_PORT_17f, REMAP_PORT_18f, REMAP_PORT_19f,
         REMAP_PORT_20f, REMAP_PORT_21f, REMAP_PORT_22f, REMAP_PORT_23f,
         REMAP_PORT_24f, REMAP_PORT_25f, REMAP_PORT_26f, REMAP_PORT_27f,
         REMAP_PORT_28f, REMAP_PORT_29f, REMAP_PORT_30f, REMAP_PORT_31f,
         REMAP_PORT_32f, REMAP_PORT_33f, REMAP_PORT_34f, REMAP_PORT_35f,
         REMAP_PORT_36f, REMAP_PORT_37f, REMAP_PORT_38f, REMAP_PORT_39f,
         REMAP_PORT_40f, REMAP_PORT_41f, REMAP_PORT_42f, REMAP_PORT_43f,
         REMAP_PORT_44f, REMAP_PORT_45f, REMAP_PORT_46f, REMAP_PORT_47f,
         REMAP_PORT_48f, REMAP_PORT_49f, REMAP_PORT_50f, REMAP_PORT_51f,
         REMAP_PORT_52f, REMAP_PORT_53f, REMAP_PORT_54f, REMAP_PORT_55f,
         REMAP_PORT_56f, REMAP_PORT_57f, REMAP_PORT_58f, REMAP_PORT_59f,
         REMAP_PORT_60f, REMAP_PORT_61f, REMAP_PORT_62f, REMAP_PORT_63f
    };
    soc_reg_t led_clk_div_reg[] = {
        CMIC_LEDUP0_CLK_DIVr, CMIC_LEDUP1_CLK_DIVr,
        CMIC_LEDUP2_CLK_DIVr, CMIC_LEDUP3_CLK_DIVr,
        CMIC_LEDUP4_CLK_DIVr
    };
    int reg_num = COUNTOF(led_reg[0]);
    uint32 rval;
    int i, j, p;
    int freq, cycles;

    /* Program the LED clock output frequency based on core clock */
    freq = SOC_INFO(unit).frequency;

    /* For LEDCLK_HALF_PERIOD. TH2 uses 2MHz LED clock. */
    /* Formula : cycles = (freq * 10^6) * (250 * 10^-9) = freq/4 */
    /* Round up the value for non-integer clocks */
    cycles = (freq + 3) / 4;

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_CLK_DIVr, &rval,
                      LEDCLK_HALF_PERIODf, cycles);
    for (i = 0; i < COUNTOF(led_clk_div_reg); i++) {
        soc_pci_write(unit, soc_reg_addr(unit, led_clk_div_reg[i],
                                         REG_PORT_ANY, 0), rval);
    }

    /*
     * Configure LED0 remap register settings.
     * Scan chain order: 64, 63, ...., 1
     */
    for (i = 0, rval = 0, p = 0; i < reg_num; i++) {
        for (j = 0; j < 4; j++, p++) {
            soc_reg_field_set(unit, led_reg[0][i], &rval, led_port[p], 63 - p);
        }
        soc_pci_write(unit,
                      soc_reg_addr(unit, led_reg[0][i], REG_PORT_ANY, 0),
                      rval);
    }

    /*
     * Configure LED1 remap register settings.
     * Scan chain order: 65, 66, ...., 128
     */
    for (i = 0, rval = 0; i < reg_num; i++) {
        for (j = 0, p = i << 2; j < 4; j++) {
            soc_reg_field_set(unit, led_reg[1][i], &rval, led_port[p + j], p + 3 - j);
        }
        soc_pci_write(unit,
                      soc_reg_addr(unit, led_reg[1][i], REG_PORT_ANY, 0),
                      rval);
    }

    /*
     * Configure LED2 remap register settings.
     * Scan chain order: 192, 191, ...., 129
     */
    for (i = 0, rval = 0, p = 0; i < reg_num; i++) {
        for (j = 0; j < 4; j++, p++) {
            soc_reg_field_set(unit, led_reg[2][i], &rval, led_port[p], 63 - p);
        }
        soc_pci_write(unit,
                      soc_reg_addr(unit, led_reg[2][i], REG_PORT_ANY, 0),
                      rval);
    }

    /*
     * Configure LED3 remap register settings.
     * Scan chain order: 193, 194, ...., 256
     */
    for (i = 0, rval = 0; i < reg_num; i++) {
        for (j = 0, p = i << 2; j < 4; j++) {
            soc_reg_field_set(unit, led_reg[3][i], &rval, led_port[p + j], p + 3 - j);
        }
        soc_pci_write(unit,
                      soc_reg_addr(unit, led_reg[3][i], REG_PORT_ANY, 0),
                      rval);
    }

    /*
     * Configure LED4 remap register settings.
     * Scan chain order: 260, 259, 258, 257
     */
    for (i = 0, rval = 0, p = 0; i < reg_num; i++) {
        for (j = 0; j < 4; j++, p++) {
            soc_reg_field_set(unit, led_reg[4][i], &rval, led_port[p], 0x3f);
        }
        if (i == 0) {
            soc_reg_field_set(unit, led_reg[4][i], &rval, led_port[1], 1);
            soc_reg_field_set(unit, led_reg[4][i], &rval, led_port[3], 0);
        }
        soc_pci_write(unit,
                      soc_reg_addr(unit, led_reg[4][i], REG_PORT_ANY, 0),
                      rval);
    }

    /* initialize the LED processors data ram */
    rval = 0;
    for (i = 0; i < 256; i++) {
        WRITE_CMIC_LEDUP0_DATA_RAMr(unit, i, rval);
        WRITE_CMIC_LEDUP1_DATA_RAMr(unit, i, rval);
        soc_pci_write(unit,
                      soc_reg_addr(unit, CMIC_LEDUP2_DATA_RAMr, REG_PORT_ANY, i),
                      rval);
        soc_pci_write(unit,
                      soc_reg_addr(unit, CMIC_LEDUP3_DATA_RAMr, REG_PORT_ANY, i),
                      rval);
        soc_pci_write(unit,
                      soc_reg_addr(unit, CMIC_LEDUP4_DATA_RAMr, REG_PORT_ANY, i),
                      rval);
    }

    return SOC_E_NONE;
}

/* For a given logical port, return the OBM id and the lane number */
int
soc_tomahawk2_port_obm_info_get(int unit, soc_port_t port,
                               int *obm_id, int *lane)
{
    return soc_tomahawk_port_obm_info_get(unit, port, obm_id, lane);
}

STATIC int
_soc_tomahawk2_idb_ca_reset(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    _soc_tomahawk2_tdm_t *tdm = SOC_CONTROL(unit)->tdm_info;
    int block_info_idx, pipe, clport, obm, port;
    uint32 rval0, rval1;
    soc_reg_t reg;
    static const soc_reg_t obm_ca_ctrl_regs[] = {
        IDB_OBM0_CA_CONTROLr, IDB_OBM1_CA_CONTROLr,
        IDB_OBM2_CA_CONTROLr, IDB_OBM3_CA_CONTROLr,
        IDB_OBM4_CA_CONTROLr, IDB_OBM5_CA_CONTROLr,
        IDB_OBM6_CA_CONTROLr, IDB_OBM7_CA_CONTROLr,
        IDB_OBM8_CA_CONTROLr, IDB_OBM9_CA_CONTROLr,
        IDB_OBM10_CA_CONTROLr, IDB_OBM11_CA_CONTROLr,
        IDB_OBM12_CA_CONTROLr, IDB_OBM13_CA_CONTROLr,
        IDB_OBM14_CA_CONTROLr, IDB_OBM15_CA_CONTROLr
    };
    static const int hw_mode_values[SOC_TH_PORT_RATIO_COUNT] = {
        0, 1, 1, 1, 3, 5, 4, 6, 2
    };

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        pipe = si->port_pipe[port];
        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);
        obm = clport & 0xF;

        /* Set cell assembly mode then toggle reset to send initial credit
         * to EP */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_ca_ctrl_regs[obm])[pipe];
        rval0 = 0;
        soc_reg_field_set(unit, reg, &rval0, PORT_MODEf,
                          hw_mode_values[tdm->port_ratio[clport]]);
        rval1 = rval0;
        soc_reg_field_set(unit, reg, &rval0, PORT0_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT1_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT2_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT3_RESETf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval1));
    }

    /* Toggle cpu port cell assembly reset */
    reg = IDB_CA_CPU_CONTROLr;
    rval0 = 0;
    soc_reg_field_set(unit, reg, &rval0, PORT_RESETf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));

    /* Toggle loopback port cell assembly reset */
    reg = IDB_CA_LPBK_CONTROLr;
    rval0 = 0;
    soc_reg_field_set(unit, reg, &rval0, PORT_RESETf, 1);
    PBMP_LB_ITER(unit, port) {
        reg = SOC_REG_UNIQUE_ACC(unit, IDB_CA_LPBK_CONTROLr)
            [si->port_pipe[port]];
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_tomahawk2_idb_init
 * Purpose:
 *      IDB initialization, including Cell Assembly reset, Credit init, OBM init
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      SOC_E_xxx
 * Notes:
 *      The init sequences follow the guideline of TH2_SW_Init.docx
 *          3.8	Cell Assembly Soft Reset
 *          3.9	Enable Credit Propagation
 *          4.1	Oversubscription Buffer Manager
 *      The OBM setting follow the guideline of TH2_mmu_setting_v2.2.xlsx
 */
int
_soc_tomahawk2_idb_init(int unit)
{
#define _TH2_CELLS_PER_OBM        4608 
    soc_info_t *si;
    soc_reg_t reg;
    soc_mem_t mem;
    soc_field_t field;
    int block_info_idx, pipe, clport, obm, lane, port, lport, phy_port_base;
    int lossless, prio, i, count, num_lanes, pipe_init_usec;
    uint32 rval0, in_progress, pipe_map;
    soc_timeout_t to;
    uint64 rval64;
    uint32 entry[SOC_MAX_MEM_WORDS];
    static const struct obm_setting_s {
        int discard_limit;
        int lossless_discard;
        int port_xoff;
        int port_xon;
        int lossless_xoff;
        int lossless_xon;
        int lossy_low_pri;
        int lossy_discard;
    } obm_settings[2][5] = {
        /* LOSSY Settings*/
        { /* indexed by number of lanes used in the port */
            { /* 0 - invalid */ 0 },
            { /* 1 lane */
                _TH2_CELLS_PER_OBM/4,           /* discard_limit */
                _TH2_CELLS_PER_OBM,             /* lossless_discard */
                _TH2_CELLS_PER_OBM,             /* port_xoff */
                _TH2_CELLS_PER_OBM,             /* port_xon */
                _TH2_CELLS_PER_OBM,             /* lossless_xoff */
                _TH2_CELLS_PER_OBM,             /* lossless_xon */
                768,                            /* lossy_low_pri */
                1148                            /* lossy_discard */
            },
            { /* 2 lanes */
                _TH2_CELLS_PER_OBM/2,           /* discard_limit */
                _TH2_CELLS_PER_OBM,             /* lossless_discard */
                _TH2_CELLS_PER_OBM,             /* port_xoff */
                _TH2_CELLS_PER_OBM,             /* port_xon */
                _TH2_CELLS_PER_OBM,             /* lossless_xoff */
                _TH2_CELLS_PER_OBM,             /* lossless_xon */
                1920,                           /* lossy_low_pri */
                2300                            /* lossy_discard */
            },
            { /* 3 - invalid  */ 0 },
            { /* 4 lanes */
                _TH2_CELLS_PER_OBM,             /* discard_limit */
                _TH2_CELLS_PER_OBM,             /* lossless_discard */
                _TH2_CELLS_PER_OBM,             /* port_xoff */
                _TH2_CELLS_PER_OBM,             /* port_xon */
                _TH2_CELLS_PER_OBM,             /* lossless_xoff */
                _TH2_CELLS_PER_OBM,             /* lossless_xon */
                4224,                           /* lossy_low_pri */
                4604                            /* lossy_discard */
            }
        },
        /* LOSSLESS Settings*/
        { /* indexed by number of lanes used in the port */
            { /* 0 - invalid */ 0 },
            { /* 1 lane */
                _TH2_CELLS_PER_OBM/4,           /* discard_limit */
                _TH2_CELLS_PER_OBM,             /* lossless_discard */
                559,                            /* port_xoff */
                529,                            /* port_xon */
                129,                            /* lossless_xoff */
                99,                             /* lossless_xon */
                270,                            /* lossy_low_pri */
                654                             /* lossy_discard */
            },
            { /* 2 lanes */
                _TH2_CELLS_PER_OBM/2,           /* discard_limit */
                _TH2_CELLS_PER_OBM,             /* lossless_discard */
                1523,                           /* port_xoff */
                1493,                           /* port_xon */
                321,                            /* lossless_xoff */
                291,                            /* lossless_xon */
                270,                            /* lossy_low_pri */
                654                             /* lossy_discard */
            },
            { /* 3 - invalid  */ 0 },
            { /* 4 lanes */
                _TH2_CELLS_PER_OBM,             /* discard_limit */
                _TH2_CELLS_PER_OBM,             /* lossless_discard */
                3827,                           /* port_xoff */
                3797,                           /* port_xon */
                768,                            /* lossless_xoff */
                738,                            /* lossless_xon */
                270,                            /* lossy_low_pri */
                654                             /* lossy_discard */
            }
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
        IDB_OBM14_CONTROLr, IDB_OBM15_CONTROLr
    };
    static const soc_reg_t obm_thresh_regs[] = {
        IDB_OBM0_THRESHOLDr, IDB_OBM1_THRESHOLDr,
        IDB_OBM2_THRESHOLDr, IDB_OBM3_THRESHOLDr,
        IDB_OBM4_THRESHOLDr, IDB_OBM5_THRESHOLDr,
        IDB_OBM6_THRESHOLDr, IDB_OBM7_THRESHOLDr,
        IDB_OBM8_THRESHOLDr, IDB_OBM9_THRESHOLDr,
        IDB_OBM10_THRESHOLDr, IDB_OBM11_THRESHOLDr,
        IDB_OBM12_THRESHOLDr, IDB_OBM13_THRESHOLDr,
        IDB_OBM14_THRESHOLDr, IDB_OBM15_THRESHOLDr
    };
    static const soc_reg_t obm_thresh1_regs[] = {
        IDB_OBM0_THRESHOLD_1r, IDB_OBM1_THRESHOLD_1r,
        IDB_OBM2_THRESHOLD_1r, IDB_OBM3_THRESHOLD_1r,
        IDB_OBM4_THRESHOLD_1r, IDB_OBM5_THRESHOLD_1r,
        IDB_OBM6_THRESHOLD_1r, IDB_OBM7_THRESHOLD_1r,
        IDB_OBM8_THRESHOLD_1r, IDB_OBM9_THRESHOLD_1r,
        IDB_OBM10_THRESHOLD_1r, IDB_OBM11_THRESHOLD_1r,
        IDB_OBM12_THRESHOLD_1r, IDB_OBM13_THRESHOLD_1r,
        IDB_OBM14_THRESHOLD_1r, IDB_OBM15_THRESHOLD_1r
    };
    static const soc_reg_t tpid_regs[] = {
        IDB_OBM0_OUTER_TPIDr, IDB_OBM1_OUTER_TPIDr,
        IDB_OBM2_OUTER_TPIDr, IDB_OBM3_OUTER_TPIDr,
        IDB_OBM4_OUTER_TPIDr, IDB_OBM5_OUTER_TPIDr,
        IDB_OBM6_OUTER_TPIDr, IDB_OBM7_OUTER_TPIDr,
        IDB_OBM8_OUTER_TPIDr, IDB_OBM9_OUTER_TPIDr,
        IDB_OBM10_OUTER_TPIDr, IDB_OBM11_OUTER_TPIDr,
        IDB_OBM12_OUTER_TPIDr, IDB_OBM13_OUTER_TPIDr,
        IDB_OBM14_OUTER_TPIDr, IDB_OBM15_OUTER_TPIDr
    };
    static const soc_reg_t obm_fc_thresh_regs[] = {
        IDB_OBM0_FC_THRESHOLDr, IDB_OBM1_FC_THRESHOLDr,
        IDB_OBM2_FC_THRESHOLDr, IDB_OBM3_FC_THRESHOLDr,
        IDB_OBM4_FC_THRESHOLDr, IDB_OBM5_FC_THRESHOLDr,
        IDB_OBM6_FC_THRESHOLDr, IDB_OBM7_FC_THRESHOLDr,
        IDB_OBM8_FC_THRESHOLDr, IDB_OBM9_FC_THRESHOLDr,
        IDB_OBM10_FC_THRESHOLDr, IDB_OBM11_FC_THRESHOLDr,
        IDB_OBM12_FC_THRESHOLDr, IDB_OBM13_FC_THRESHOLDr,
        IDB_OBM14_FC_THRESHOLDr, IDB_OBM15_FC_THRESHOLDr
    };
    static const soc_reg_t obm_fc_thresh1_regs[] = {
        IDB_OBM0_FC_THRESHOLD_1r, IDB_OBM1_FC_THRESHOLD_1r,
        IDB_OBM2_FC_THRESHOLD_1r, IDB_OBM3_FC_THRESHOLD_1r,
        IDB_OBM4_FC_THRESHOLD_1r, IDB_OBM5_FC_THRESHOLD_1r,
        IDB_OBM6_FC_THRESHOLD_1r, IDB_OBM7_FC_THRESHOLD_1r,
        IDB_OBM8_FC_THRESHOLD_1r, IDB_OBM9_FC_THRESHOLD_1r,
        IDB_OBM10_FC_THRESHOLD_1r, IDB_OBM11_FC_THRESHOLD_1r,
        IDB_OBM12_FC_THRESHOLD_1r, IDB_OBM13_FC_THRESHOLD_1r,
        IDB_OBM14_FC_THRESHOLD_1r, IDB_OBM15_FC_THRESHOLD_1r
    };
    static const soc_reg_t obm_shared_regs[] = {
        IDB_OBM0_SHARED_CONFIGr, IDB_OBM1_SHARED_CONFIGr,
        IDB_OBM2_SHARED_CONFIGr, IDB_OBM3_SHARED_CONFIGr,
        IDB_OBM4_SHARED_CONFIGr, IDB_OBM5_SHARED_CONFIGr,
        IDB_OBM6_SHARED_CONFIGr, IDB_OBM7_SHARED_CONFIGr,
        IDB_OBM8_SHARED_CONFIGr, IDB_OBM9_SHARED_CONFIGr,
        IDB_OBM10_SHARED_CONFIGr, IDB_OBM11_SHARED_CONFIGr,
        IDB_OBM12_SHARED_CONFIGr, IDB_OBM13_SHARED_CONFIGr,
        IDB_OBM14_SHARED_CONFIGr, IDB_OBM15_SHARED_CONFIGr
    };
    static const soc_reg_t obm_max_usage_regs[] = {
        IDB_OBM0_MAX_USAGE_SELECTr, IDB_OBM1_MAX_USAGE_SELECTr,
        IDB_OBM2_MAX_USAGE_SELECTr, IDB_OBM3_MAX_USAGE_SELECTr,
        IDB_OBM4_MAX_USAGE_SELECTr, IDB_OBM5_MAX_USAGE_SELECTr,
        IDB_OBM6_MAX_USAGE_SELECTr, IDB_OBM7_MAX_USAGE_SELECTr,
        IDB_OBM8_MAX_USAGE_SELECTr, IDB_OBM9_MAX_USAGE_SELECTr,
        IDB_OBM10_MAX_USAGE_SELECTr, IDB_OBM11_MAX_USAGE_SELECTr,
        IDB_OBM12_MAX_USAGE_SELECTr, IDB_OBM13_MAX_USAGE_SELECTr,
        IDB_OBM14_MAX_USAGE_SELECTr, IDB_OBM15_MAX_USAGE_SELECTr
    };
    static const soc_reg_t obm_port_config_regs[] = {
        IDB_OBM0_PORT_CONFIGr, IDB_OBM1_PORT_CONFIGr,
        IDB_OBM2_PORT_CONFIGr, IDB_OBM3_PORT_CONFIGr,
        IDB_OBM4_PORT_CONFIGr, IDB_OBM5_PORT_CONFIGr,
        IDB_OBM6_PORT_CONFIGr, IDB_OBM7_PORT_CONFIGr,
        IDB_OBM8_PORT_CONFIGr, IDB_OBM9_PORT_CONFIGr,
        IDB_OBM10_PORT_CONFIGr, IDB_OBM11_PORT_CONFIGr,
        IDB_OBM12_PORT_CONFIGr, IDB_OBM13_PORT_CONFIGr,
        IDB_OBM14_PORT_CONFIGr, IDB_OBM15_PORT_CONFIGr
    };
    static const soc_reg_t obm_fc_config_regs[] = {
        IDB_OBM0_FLOW_CONTROL_CONFIGr, IDB_OBM1_FLOW_CONTROL_CONFIGr,
        IDB_OBM2_FLOW_CONTROL_CONFIGr, IDB_OBM3_FLOW_CONTROL_CONFIGr,
        IDB_OBM4_FLOW_CONTROL_CONFIGr, IDB_OBM5_FLOW_CONTROL_CONFIGr,
        IDB_OBM6_FLOW_CONTROL_CONFIGr, IDB_OBM7_FLOW_CONTROL_CONFIGr,
        IDB_OBM8_FLOW_CONTROL_CONFIGr, IDB_OBM9_FLOW_CONTROL_CONFIGr,
        IDB_OBM10_FLOW_CONTROL_CONFIGr, IDB_OBM11_FLOW_CONTROL_CONFIGr,
        IDB_OBM12_FLOW_CONTROL_CONFIGr, IDB_OBM13_FLOW_CONTROL_CONFIGr,
        IDB_OBM14_FLOW_CONTROL_CONFIGr, IDB_OBM15_FLOW_CONTROL_CONFIGr
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
    };
    static const soc_field_t obm_ob_pri_fields[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf, OFFSET8_OB_PRIORITYf,
        OFFSET9_OB_PRIORITYf, OFFSET10_OB_PRIORITYf, OFFSET11_OB_PRIORITYf,
        OFFSET12_OB_PRIORITYf, OFFSET13_OB_PRIORITYf, OFFSET14_OB_PRIORITYf,
        OFFSET15_OB_PRIORITYf
    };

    si = &SOC_INFO(unit);
    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    /* Cell Assembly Soft Reset */
    SOC_IF_ERROR_RETURN(_soc_tomahawk2_idb_ca_reset(unit));

    /* Prepare pri_map entry value for the loop below */
    sal_memset(entry, 0, sizeof(idb_obm0_pri_map_port0_entry_t));
    mem = SOC_MEM_UNIQUE_ACC(unit, obm_pri_map_mem[0][0])[0];
    count = COUNTOF(obm_ob_pri_fields);
    prio = (lossless ? 2 : 0);
    for (i = 0; i < count; i++) {
        soc_mem_field32_set(unit, mem, entry, obm_ob_pri_fields[i], prio);
    }

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        phy_port_base = PORT_BLOCK_BASE_PORT(port);
        pipe = si->port_pipe[port];
        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);
        obm = clport & 0xF;

        /*4.1.1	Aggregate Thresholds */
        /* Enable oversub */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_ctrl_regs[obm])[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval0));
        for (lane = 0; lane < _TH2_PORTS_PER_PBLK; lane++) {
            if (si->port_p2l_mapping[phy_port_base + lane] == -1) {
                continue;
            }
            field = obm_bypass_fields[lane];
            soc_reg_field_set(unit, reg, &rval0, field, 1);
            field = obm_oversub_fields[lane];
            soc_reg_field_set(unit, reg, &rval0, field, 1);
            lport = si->port_p2l_mapping[phy_port_base + lane];
            if (si->port_init_speed[lport]< 100000) {
                field = obm_ca_sop_fields[lane];
                soc_reg_field_set(unit, reg, &rval0, field, 0);
            }
        }

        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        if (!SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
            continue;
        }

        /* Configure shared config */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_shared_regs[obm])[pipe];
        SOC_IF_ERROR_RETURN
            (soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, DISCARD_THRESHOLDf,
                              _TH2_CELLS_PER_OBM);
        soc_reg64_field32_set(unit, reg, &rval64, SOP_THRESHOLDf,
                              _TH2_CELLS_PER_OBM);
        soc_reg64_field32_set(unit, reg, &rval64, SOP_DISCARD_MODEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, REG_PORT_ANY, 0, rval64));

        /* IDB OBM Max Usage Counter Configuration */
        rval0 = 0;
        reg = SOC_REG_UNIQUE_ACC(unit, obm_max_usage_regs[obm])[pipe];
        soc_reg_field_set(unit, reg, &rval0, PRIORITY_SELECTf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));

        for (lane = 0; lane < _TH2_PORTS_PER_PBLK; lane++) {
            port = si->port_p2l_mapping[phy_port_base + lane];
            if (port == -1) {
                continue;
            }
            num_lanes = si->port_num_lanes[port];

            /* 4.1.2    Per Port Thresholds */
            /* Configure threshold */
            reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_regs[obm])[pipe];
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                            _TH2_CELLS_PER_OBM);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSY_DISCARDf,
                            obm_settings[lossless][num_lanes].lossy_discard);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSY_LOW_PRIf,
                            obm_settings[lossless][num_lanes].lossy_low_pri);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSY_MINf, 0);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

            /* Configure threshold_1 */
            reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh1_regs[obm])[pipe];
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
            soc_reg64_field32_set(unit, reg, &rval64, DISCARD_LIMITf,
                            obm_settings[lossless][num_lanes].discard_limit);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_DISCARDf,
                            _TH2_CELLS_PER_OBM);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

            /* Configure pri_map table */
            mem = SOC_MEM_UNIQUE_ACC
                    (unit, obm_pri_map_mem[obm][lane])[pipe];
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));

            /* Configure OBM Port config */
            rval0 = 0;
            reg = SOC_REG_UNIQUE_ACC(unit, obm_port_config_regs[obm])[pipe];
            soc_reg_field_set(unit, reg, &rval0, PORT_PRIf, 2);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, lane, rval0));

            /* Configure Outer TPID */
            rval0 = 0;
            reg = SOC_REG_UNIQUE_ACC(unit, tpid_regs[obm])[pipe];
            soc_reg_field_set(unit, reg, &rval0, ENABLEf, 1);
            soc_reg_field_set(unit, reg, &rval0, TPIDf, 0x8100);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, lane, rval0));

            /* 4.1.3	Flow Control Thresholds */
            /* Configure flow control threshold */
            reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_regs[obm])[pipe];
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XONf,
                            obm_settings[lossless][num_lanes].lossless_xon);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XOFFf,
                            obm_settings[lossless][num_lanes].lossless_xoff);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XONf,
                            obm_settings[lossless][num_lanes].lossless_xon);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                            obm_settings[lossless][num_lanes].lossless_xoff);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

            /* Configure flow control threshold_1 */
            reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh1_regs[obm])[pipe];
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
            soc_reg64_field32_set(unit, reg, &rval64, PORT_XONf,
                            obm_settings[lossless][num_lanes].port_xon);
            soc_reg64_field32_set(unit, reg, &rval64, PORT_XOFFf,
                            obm_settings[lossless][num_lanes].port_xoff);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

            /* Configure flow control config */
            reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_config_regs[obm])[pipe];
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
            soc_reg64_field32_set(unit, reg, &rval64, PORT_FC_ENf, lossless);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_FC_ENf,
                                  lossless);
            soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_FC_ENf, 0);
            soc_reg64_field32_set(unit, reg, &rval64,
                                  LOSSLESS0_PRIORITY_PROFILEf,
                                  (lossless ? 0xffff : 0));
            soc_reg64_field32_set(unit, reg, &rval64,
                                  LOSSLESS1_PRIORITY_PROFILEf, 0);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));
        }
    }

    /* Set IDB DPP configuration control register to release credits to IS */
    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        reg = SOC_REG_UNIQUE_ACC(unit, IDB_DPP_CTRLr)[pipe];
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval0));        
        soc_reg_field_set(unit, reg, &rval0, CREDITSf, 32);
        soc_reg_field_set(unit, reg, &rval0, STARTf, 1);
        soc_reg_field_set(unit, reg, &rval0, DONEf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
    }

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for DPP credit initialization done */
    in_progress = pipe_map;
    do {
        for (pipe = 0; pipe < _TH2_PIPES_PER_DEV && in_progress; pipe++) {
            reg = SOC_REG_UNIQUE_ACC(unit, IDB_DPP_CTRLr)[pipe];
            if (in_progress & (1 << pipe)) { /* not done yet */
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval0));
                if (soc_reg_field_get(unit, reg, rval0, DONEf)) {
                    in_progress ^= (1 << pipe);
                }
            }
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : IDB_DPP_CTRL timeout\n"), unit));
            break;
        }
    } while (in_progress != 0);

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define SOC_TDM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_TDM_WB_DEFAULT_VERSION            SOC_TDM_WB_VERSION_1_0

int soc_th2_tdm_scache_allocate(int unit)
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
    ovs_size = _TH2_OVS_HPIPE_COUNT_PER_PIPE *
               _TH2_OVS_GROUP_COUNT_PER_HPIPE *
               _TH2_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = _TH2_OVS_HPIPE_COUNT_PER_PIPE *
                   _TH2_PKT_SCH_LENGTH;
    hpipes = _TH2_PIPES_PER_DEV * _TH2_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * _TH2_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * _TH2_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * _TH2_TDM_LENGTH) + /* IDB TDM info on pipe1 */
                 (ilen * _TH2_TDM_LENGTH) + /* MMU TDM info on pipe1 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe1 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe1     */
                 (ilen * _TH2_TDM_LENGTH) + /* IDB TDM info on pipe2 */
                 (ilen * _TH2_TDM_LENGTH) + /* MMU TDM info on pipe2 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe2 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe2     */
                 (ilen * _TH2_TDM_LENGTH) + /* IDB TDM info on pipe3 */
                 (ilen * _TH2_TDM_LENGTH) + /* MMU TDM info on pipe3 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe3 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe3     */
                 (ilen * _TH2_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * _TH2_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * _TH2_PBLKS_PER_DEV)     + /* port ratio */
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
 *      soc_th2_tdm_scache_sync
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
 *    IDB TDM info on pipe1
 *    MMU TDM info on pipe1
 *    Oversub group info on pipe1
 *    pkt shaper info on pipe1
 *    IDB TDM info on pipe2
 *    MMU TDM info on pipe2
 *    Oversub group info on pipe2
 *    pkt shaper info on pipe2
 *    IDB TDM info on pipe3
 *    MMU TDM info on pipe3
 *    Oversub group info on pipe3
 *    pkt shaper info on pipe3
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
int soc_th2_tdm_scache_sync(int unit)
{
    uint8 *tdm_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    uint32 var_size = 0;
    uint32 scache_offset=0;
    int rv = 0;
    int ilen, ovs_size, pkt_shp_size, hpipes, phy_port;
    _soc_tomahawk2_tdm_t *tdm = SOC_CONTROL(unit)->tdm_info;

    ilen = sizeof(int);
    ovs_size = _TH2_OVS_HPIPE_COUNT_PER_PIPE *
               _TH2_OVS_GROUP_COUNT_PER_HPIPE *
               _TH2_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = _TH2_OVS_HPIPE_COUNT_PER_PIPE *
                   _TH2_PKT_SCH_LENGTH;
    hpipes = _TH2_PIPES_PER_DEV * _TH2_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * _TH2_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * _TH2_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * _TH2_TDM_LENGTH) + /* IDB TDM info on pipe1 */
                 (ilen * _TH2_TDM_LENGTH) + /* MMU TDM info on pipe1 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe1 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe1     */
                 (ilen * _TH2_TDM_LENGTH) + /* IDB TDM info on pipe2 */
                 (ilen * _TH2_TDM_LENGTH) + /* MMU TDM info on pipe2 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe2 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe2     */
                 (ilen * _TH2_TDM_LENGTH) + /* IDB TDM info on pipe3 */
                 (ilen * _TH2_TDM_LENGTH) + /* MMU TDM info on pipe3 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe3 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe3     */
                 (ilen * _TH2_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * _TH2_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * _TH2_PBLKS_PER_DEV)     + /* port ratio */
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

    if (NULL == tdm_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: scache memory not allocated for tdm"
                  "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /* IDB TDM info on pipe0 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[0].idb_tdm,
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe0 */
    var_size = ilen * _TH2_TDM_LENGTH;
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

    /* IDB TDM info on pipe1 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[1].idb_tdm,
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe1 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[1].mmu_tdm,
               var_size);
    scache_offset += var_size;

    /* Oversub group info on pipe1 */
    var_size = ilen * ovs_size;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[1].ovs_tdm,
               var_size);
    scache_offset += var_size;

    /* pkt shaper info on pipe1 */
    var_size = ilen * pkt_shp_size;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[1].pkt_shaper_tdm,
               var_size);
    scache_offset += var_size;

    /* IDB TDM info on pipe2 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[2].idb_tdm,
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe2 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[2].mmu_tdm,
               var_size);
    scache_offset += var_size;

    /* Oversub group info on pipe2 */
    var_size = ilen * ovs_size;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[2].ovs_tdm,
               var_size);
    scache_offset += var_size;

    /* pkt shaper info on pipe2 */
    var_size = ilen * pkt_shp_size;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[2].pkt_shaper_tdm,
               var_size);
    scache_offset += var_size;

    /* IDB TDM info on pipe3 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[3].idb_tdm,
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe3 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[3].mmu_tdm,
               var_size);
    scache_offset += var_size;

    /* Oversub group info on pipe3 */
    var_size = ilen * ovs_size;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[3].ovs_tdm,
               var_size);
    scache_offset += var_size;

    /* pkt shaper info on pipe3 */
    var_size = ilen * pkt_shp_size;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[3].pkt_shaper_tdm,
               var_size);
    scache_offset += var_size;

    /* hpipe id of phy-port */
    for (phy_port = 0; phy_port < _TH2_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm_scache_ptr[scache_offset],
                   &tdm->pblk_info[phy_port].pblk_hpipe_num,
                   var_size);
        scache_offset += var_size;
    }

    /* pblk id of phy-port */
    for (phy_port = 0; phy_port < _TH2_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm_scache_ptr[scache_offset],
                   &tdm->pblk_info[phy_port].pblk_cal_idx,
                   var_size);
        scache_offset += var_size;
    }

    /* port ratio */
    var_size = ilen * _TH2_PBLKS_PER_DEV;
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
 *      soc_th2_tdm_scache_recovery
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
 *    IDB TDM info on pipe1
 *    MMU TDM info on pipe1
 *    Oversub group info on pipe1
 *    pkt shaper info on pipe1
 *    IDB TDM info on pipe2
 *    MMU TDM info on pipe2
 *    Oversub group info on pipe2
 *    pkt shaper info on pipe2
 *    IDB TDM info on pipe3
 *    MMU TDM info on pipe3
 *    Oversub group info on pipe3
 *    pkt shaper info on pipe3
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
int soc_th2_tdm_scache_recovery(int unit)
{
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int rv = SOC_E_NONE;
    uint8 *tdm_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    uint32 scache_offset=0;
    uint32 var_size = 0;
    uint16 recovered_ver = 0;
    int ilen, ovs_size, pkt_shp_size, hpipes, phy_port;
    _soc_tomahawk2_tdm_t *tdm = SOC_CONTROL(unit)->tdm_info;

    ilen = sizeof(int);
    ovs_size = _TH2_OVS_HPIPE_COUNT_PER_PIPE *
               _TH2_OVS_GROUP_COUNT_PER_HPIPE *
               _TH2_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = _TH2_OVS_HPIPE_COUNT_PER_PIPE *
                   _TH2_PKT_SCH_LENGTH;
    hpipes = _TH2_PIPES_PER_DEV * _TH2_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * _TH2_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * _TH2_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * _TH2_TDM_LENGTH) + /* IDB TDM info on pipe1 */
                 (ilen * _TH2_TDM_LENGTH) + /* MMU TDM info on pipe1 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe1 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe1     */
                 (ilen * _TH2_TDM_LENGTH) + /* IDB TDM info on pipe2 */
                 (ilen * _TH2_TDM_LENGTH) + /* MMU TDM info on pipe2 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe2 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe2     */
                 (ilen * _TH2_TDM_LENGTH) + /* IDB TDM info on pipe3 */
                 (ilen * _TH2_TDM_LENGTH) + /* MMU TDM info on pipe3 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe3 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe3     */
                 (ilen * _TH2_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * _TH2_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * _TH2_PBLKS_PER_DEV)     + /* port ratio */
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

    if (NULL == tdm_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: scache recovery for tdm"
                  "%s()[LINE:%d] DONE \n"),
                  FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /* IDB TDM info on pipe0 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[0].idb_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe0 */
    var_size = ilen * _TH2_TDM_LENGTH;
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
    var_size = ilen * pkt_shp_size;
    sal_memcpy(tdm->tdm_pipe[0].pkt_shaper_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* IDB TDM info on pipe1 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[1].idb_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe1 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[1].mmu_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Oversub group info on pipe1 */
    var_size = ilen * ovs_size;
    sal_memcpy(tdm->tdm_pipe[1].ovs_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* pkt shaper info on pipe1 */
    var_size = ilen * pkt_shp_size;
    sal_memcpy(tdm->tdm_pipe[1].pkt_shaper_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* IDB TDM info on pipe2 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[2].idb_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe2 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[2].mmu_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Oversub group info on pipe2 */
    var_size = ilen * ovs_size;
    sal_memcpy(tdm->tdm_pipe[2].ovs_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* pkt shaper info on pipe2 */
    var_size = ilen * pkt_shp_size;
    sal_memcpy(tdm->tdm_pipe[2].pkt_shaper_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* IDB TDM info on pipe3 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[3].idb_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe3 */
    var_size = ilen * _TH2_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[3].mmu_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Oversub group info on pipe3 */
    var_size = ilen * ovs_size;
    sal_memcpy(tdm->tdm_pipe[3].ovs_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* pkt shaper info on pipe3 */
    var_size = ilen * pkt_shp_size;
    sal_memcpy(tdm->tdm_pipe[3].pkt_shaper_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* hpipe id of phy-port */
    for (phy_port = 0; phy_port < _TH2_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm->pblk_info[phy_port].pblk_hpipe_num,
                   &tdm_scache_ptr[scache_offset],
                   var_size);
        scache_offset += var_size;
    }

    /* pblk id of phy-port */
    for (phy_port = 0; phy_port < _TH2_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm->pblk_info[phy_port].pblk_cal_idx,
                   &tdm_scache_ptr[scache_offset],
                   var_size);
        scache_offset += var_size;
    }

    /* port ratio */
    var_size = ilen * _TH2_PBLKS_PER_DEV;
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

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

int
_soc_tomahawk2_tdm_init(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_schedule_state_t *port_schedule_state;
    soc_port_map_type_t *in_portmap;
    int port;
    int rv;
    soc_pbmp_t pbmp_mixed_sister_25_50;

    if (soc->tdm_info == NULL) {
        soc->tdm_info = sal_alloc(sizeof(_soc_tomahawk2_tdm_t),
                                    "Tomahawk2 TDM info");
        if (soc->tdm_info == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(soc->tdm_info, 0, sizeof(_soc_tomahawk2_tdm_t));
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_th2_tdm_scache_recovery(unit));
        return SOC_E_NONE;
    } else {
        SOC_IF_ERROR_RETURN(
            soc_th2_tdm_scache_allocate(unit));
    }
#endif

    port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                    "Tomahawk2 soc_port_schedule_state_t");
    if (port_schedule_state == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));

    /* Core clock frequency */
    port_schedule_state->frequency = si->frequency;

    /* Construct in_port_map */
    in_portmap = &port_schedule_state->in_port_map;

    pbmp_mixed_sister_25_50 = soc_property_get_pbmp(
                        unit, spn_PBMP_OVERSUBSCRIBE_MIXED_SISTER_25_50_INIT, 0);
    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            continue;
        }
        in_portmap->log_port_speed[port] = MAX(si->port_speed_max[port],
                                               SPEED_10G);
        in_portmap->port_num_lanes[port] = si->port_num_lanes[port];
        /* in_portmap->physical_pbm used to pass the info about the port bitmap of 25G
               * ports using 50G TDM calendar to TDM code.
               */
        if (SOC_PBMP_MEMBER(pbmp_mixed_sister_25_50, port)) {
            SOC_PBMP_PORT_ADD(in_portmap->physical_pbm, port);
        }
    }
    /* DV API requires LB speed non-zero */
    PBMP_LB_ITER(unit, port) {
        in_portmap->log_port_speed[port] = SPEED_100G;
        in_portmap->port_num_lanes[port] = si->port_num_lanes[port];
    }

    sal_memcpy(in_portmap->port_p2l_mapping, si->port_p2l_mapping,
                sizeof(int)*_TH2_PHY_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_l2p_mapping, si->port_l2p_mapping,
                sizeof(int)*_TH2_DEV_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_p2m_mapping, si->port_p2m_mapping,
                sizeof(int)*_TH2_PHY_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_m2p_mapping, si->port_m2p_mapping,
                sizeof(int)*_TH2_MMU_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_l2i_mapping, si->port_l2i_mapping,
                sizeof(int)*_TH2_DEV_PORTS_PER_DEV);
    sal_memcpy(&in_portmap->hg2_pbm, &si->hg.bitmap, sizeof(pbmp_t));
    sal_memcpy(&in_portmap->management_pbm, &si->management_pbm,
                sizeof(pbmp_t));
    sal_memcpy(&in_portmap->oversub_pbm, &si->oversub_pbm, sizeof(pbmp_t));
    /* DV API requires management ports always be in portmap,
       regardless they exist or not */
    in_portmap->port_p2l_mapping[_TH2_PHY_PORT_MNG0] = _TH2_DEV_PORT_MNG0;
    in_portmap->port_p2l_mapping[_TH2_PHY_PORT_MNG1] = _TH2_DEV_PORT_MNG1;
    in_portmap->port_l2p_mapping[_TH2_DEV_PORT_MNG0] = _TH2_PHY_PORT_MNG0;
    in_portmap->port_l2p_mapping[_TH2_DEV_PORT_MNG1] = _TH2_PHY_PORT_MNG1;
    in_portmap->port_p2m_mapping[_TH2_PHY_PORT_MNG0] = _TH2_MMU_PORT_MNG0;
    in_portmap->port_p2m_mapping[_TH2_PHY_PORT_MNG1] = _TH2_MMU_PORT_MNG1;
    in_portmap->port_m2p_mapping[_TH2_MMU_PORT_MNG0] = _TH2_PHY_PORT_MNG0;
    in_portmap->port_m2p_mapping[_TH2_MMU_PORT_MNG1] = _TH2_PHY_PORT_MNG1;
    in_portmap->port_l2i_mapping[_TH2_DEV_PORT_MNG0] = _TH2_IDB_PORT_CPU_MNG;
    in_portmap->port_l2i_mapping[_TH2_DEV_PORT_MNG1] = _TH2_IDB_PORT_CPU_MNG;

    /* Construct tdm_ingress_schedule_pipe and tdm_egress_schedule_pipe */
    port_schedule_state->is_flexport = 0;
    rv = soc_th2_port_schedule_tdm_init(unit, port_schedule_state);

    rv = soc_tomahawk2_tdm_init(unit, port_schedule_state);
    if (rv != SOC_E_NONE) {
        LOG_CLI((BSL_META_U(unit,
            "Unsupported config for TDM calendar generation\n")));
        sal_free(port_schedule_state);
        return rv;
    }

    /* Update SOC TDM info */
    rv = soc_th2_soc_tdm_update(unit, port_schedule_state);

    sal_free(port_schedule_state);

    return rv;
}

int
_soc_tomahawk2_ing_fsaf_init(int unit)
{
    int port, pipe, obm, lane;
    int send_en, receive_en, thd, duration;
    uint64 rval64;
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t reg;
    static const soc_reg_t idb_obm_fsaf_cfg[] = {
        IDB_OBM0_DBG_Ar,  IDB_OBM1_DBG_Ar,
        IDB_OBM2_DBG_Ar,  IDB_OBM3_DBG_Ar,
        IDB_OBM4_DBG_Ar,  IDB_OBM5_DBG_Ar,
        IDB_OBM6_DBG_Ar,  IDB_OBM7_DBG_Ar,
        IDB_OBM8_DBG_Ar,  IDB_OBM9_DBG_Ar,
        IDB_OBM10_DBG_Ar, IDB_OBM11_DBG_Ar,
        IDB_OBM12_DBG_Ar, IDB_OBM13_DBG_Ar,
        IDB_OBM14_DBG_Ar, IDB_OBM15_DBG_Ar,
    };
    static const soc_reg_t idb_fsaf_cfg = IDB_DBG_Br;

    send_en = (si->oversub_mode) ? 1 : 0;
    receive_en = (si->oversub_mode) ? 1 : 0;
    PBMP_ALL_ITER(unit, port) {
        if (SOC_FAILURE(soc_tomahawk2_port_obm_info_get
                (unit, port, &obm, &lane))) {
            continue;
        }
        thd = ((si->port_speed_max[port] == 10000) ||
               (si->port_speed_max[port] == 11000) ||
               (si->port_speed_max[port] == 20000) ||
               (si->port_speed_max[port] == 21000) ||
               (si->port_speed_max[port] == 40000) ||
               (si->port_speed_max[port] == 42000)) ? 12 : 18;
        pipe = si->port_pipe[port];
        reg = SOC_REG_UNIQUE_ACC(unit, idb_obm_fsaf_cfg[obm])[pipe];
        SOC_IF_ERROR_RETURN
            (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, FIELD_Af, receive_en);
        soc_reg64_field32_set(unit, reg, &rval64, FIELD_Bf, send_en);
        soc_reg64_field32_set(unit, reg, &rval64, FIELD_Cf, thd);
        SOC_IF_ERROR_RETURN
            (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));
    }

    duration = si->frequency * 2;
    for (pipe = 0; pipe < si->num_pipe; pipe ++) {
        reg = SOC_REG_UNIQUE_ACC(unit, idb_fsaf_cfg)[pipe];
        SOC_IF_ERROR_RETURN
            (soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, FIELD_Af, duration);
        SOC_IF_ERROR_RETURN
            (soc_reg_set(unit, reg, REG_PORT_ANY, 0, rval64));
    }

    return SOC_E_NONE;
}

int
soc_tomahawk2_edb_buf_reset(int unit, soc_port_t port, int reset)
{
    if (SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM) {
        return SOC_E_NONE;
    }
    return soc_tomahawk_edb_buf_reset(unit, port, reset);
}

int
soc_tomahawk2_idb_buf_reset(int unit, soc_port_t port, int reset)
{
    soc_reg_t reg, reg1;
    int port_block_base, phy_port, lane;
    int pipe, obm, clport, obm_usage_count = ~0, ca_empty = 0;
    uint32 rval;
    uint64 rval1, rval2;
    soc_info_t *si = &SOC_INFO(unit);
    static const soc_reg_t idb_obm_usage_regs[] = {
        IDB_OBM0_USAGEr, IDB_OBM1_USAGEr,
        IDB_OBM2_USAGEr, IDB_OBM3_USAGEr,
        IDB_OBM4_USAGEr, IDB_OBM5_USAGEr,
        IDB_OBM6_USAGEr, IDB_OBM7_USAGEr,
        IDB_OBM8_USAGEr, IDB_OBM9_USAGEr,
        IDB_OBM10_USAGEr, IDB_OBM11_USAGEr,
        IDB_OBM12_USAGEr, IDB_OBM13_USAGEr,
        IDB_OBM14_USAGEr, IDB_OBM15_USAGEr
    };
    static const soc_reg_t idb_obm_ca_status_regs[] = {
        IDB_OBM0_CA_HW_STATUSr, IDB_OBM1_CA_HW_STATUSr,
        IDB_OBM2_CA_HW_STATUSr, IDB_OBM3_CA_HW_STATUSr,
        IDB_OBM4_CA_HW_STATUSr, IDB_OBM5_CA_HW_STATUSr,
        IDB_OBM6_CA_HW_STATUSr, IDB_OBM7_CA_HW_STATUSr,
        IDB_OBM8_CA_HW_STATUSr, IDB_OBM9_CA_HW_STATUSr,
        IDB_OBM10_CA_HW_STATUSr, IDB_OBM11_CA_HW_STATUSr,
        IDB_OBM12_CA_HW_STATUSr, IDB_OBM13_CA_HW_STATUSr,
        IDB_OBM14_CA_HW_STATUSr, IDB_OBM15_CA_HW_STATUSr
    };
    static const soc_reg_t obm_ctrl_regs[] = {
        IDB_OBM0_CONTROLr, IDB_OBM1_CONTROLr,
        IDB_OBM2_CONTROLr, IDB_OBM3_CONTROLr,
        IDB_OBM4_CONTROLr, IDB_OBM5_CONTROLr,
        IDB_OBM6_CONTROLr, IDB_OBM7_CONTROLr,
        IDB_OBM8_CONTROLr, IDB_OBM9_CONTROLr,
        IDB_OBM10_CONTROLr, IDB_OBM11_CONTROLr,
        IDB_OBM12_CONTROLr, IDB_OBM13_CONTROLr,
        IDB_OBM14_CONTROLr, IDB_OBM15_CONTROLr
    };
    static const soc_reg_t obm_ca_ctrl_regs[] = {
        IDB_OBM0_CA_CONTROLr, IDB_OBM1_CA_CONTROLr,
        IDB_OBM2_CA_CONTROLr, IDB_OBM3_CA_CONTROLr,
        IDB_OBM4_CA_CONTROLr, IDB_OBM5_CA_CONTROLr,
        IDB_OBM6_CA_CONTROLr, IDB_OBM7_CA_CONTROLr,
        IDB_OBM8_CA_CONTROLr, IDB_OBM9_CA_CONTROLr,
        IDB_OBM10_CA_CONTROLr, IDB_OBM11_CA_CONTROLr,
        IDB_OBM12_CA_CONTROLr, IDB_OBM13_CA_CONTROLr,
        IDB_OBM14_CA_CONTROLr, IDB_OBM15_CA_CONTROLr
    };
    static const soc_field_t fifo_empty_port_fields[] = {
        FIFO_EMPTY_PORT0f, FIFO_EMPTY_PORT1f, FIFO_EMPTY_PORT2f,
        FIFO_EMPTY_PORT3f
    };
    static const soc_field_t port_reset_fields[] = {
        PORT0_RESETf, PORT1_RESETf, PORT2_RESETf, PORT3_RESETf
    };

    if (SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM) {
        return SOC_E_NONE;
    }

    phy_port = si->port_l2p_mapping[port];
    port_block_base = PORT_BLOCK_BASE_PORT(port);
    lane = phy_port - port_block_base;
    pipe = si->port_pipe[port];
    clport = si->port_serdes[port];
    obm = clport & 0xF;

    if (reset && !SAL_BOOT_SIMULATION) {
        soc_timeout_t to;

        reg = SOC_REG_UNIQUE_ACC(unit, idb_obm_usage_regs[obm])[pipe];
        reg1 = SOC_REG_UNIQUE_ACC(unit, idb_obm_ca_status_regs[obm])[pipe];

        soc_timeout_init(&to, 250000, 0);
        /* Poll until IDB buffer is empty */
        for (;;) {
            if (0 != obm_usage_count) {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, REG_PORT_ANY, lane,
                                                &rval1));
                obm_usage_count = soc_reg64_field32_get(unit, reg, rval1,
                                                        TOTAL_COUNTf);
            }
            if (0 == ca_empty) {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg1, REG_PORT_ANY, 0,
                                                &rval2));
                ca_empty = soc_reg64_field32_get(unit, reg1, rval2,
                                                 fifo_empty_port_fields[lane]);
            }
            if ((0 == obm_usage_count) && (1 == ca_empty)) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "IDBBufferDrainTimeOut:port %d,%s, timeout"
                                      "(idb_obm_usage: %d) "
                                      "(ca_fifo_empty: %d)\n"),
                           unit, SOC_PORT_NAME(unit, port), obm_usage_count,
                           ca_empty));
                return SOC_E_INTERNAL;
            }
        }
    }

    reg = SOC_REG_UNIQUE_ACC(unit, obm_ctrl_regs[obm])[pipe];
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, port_reset_fields[lane], reset);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    reg = SOC_REG_UNIQUE_ACC(unit, obm_ca_ctrl_regs[obm])[pipe];
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, port_reset_fields[lane], reset);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

_soc_th2_tcb_hw_cb th2_tcb_cb;

int
soc_th2_process_mmu_tcb(int unit, int xpe, soc_field_t field)
{
    int rv = SOC_E_NONE;
    if (th2_tcb_cb != NULL) {
        rv = th2_tcb_cb(unit);
    }
    return rv;
}

int soc_th2_set_tcb_callback(int unit, _soc_th2_tcb_hw_cb cb)
{
    th2_tcb_cb = cb;
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th2_port_speed_supported
 * Purpose:
 *      Check if the speed of port is supported
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      port            - (IN)  Logical port number.
 *      speed           - (IN)  Speed (Mbps).
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */
soc_error_t
soc_th2_port_speed_supported(int unit, soc_port_t port, int speed)
{
    int num_lanes, tsc_id, lane_speed;
    uint32 speed_mask, speed_valid;
    soc_info_t *si = &SOC_INFO(unit);

    /*
     * If Universal calendar was not selected at init time then
     * disallow speed change to HG2 speeds.
     */
    if (FALSE == si->fabric_port_enable) {
        if (IS_HG2_SPEED(speed)) {
            /*
             * Intentionally returning SOC_E_CONFIG as this is a case
             * of incorrect configuration
             */
            return SOC_E_CONFIG;
        }
    }

    /*
     * Management ports only support 1G, 2.5G and 10G. Flexport
     * on management ports is not supported. Management ports
     * are guaranteed 10G bandwidth.
     */
    if (SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port)) {
        if ((1000 == speed) || (2500 == speed) || (10000 == speed)) {
            return SOC_E_NONE;
        } else {
            return SOC_E_PARAM;
        }
    }

    num_lanes = si->port_num_lanes[port];

    if(soc_feature(unit, soc_feature_untethered_otp)) {
        tsc_id = (si->port_l2p_mapping[port] - 1) / _TH2_PORTS_PER_PBLK;
        SOC_IF_ERROR_RETURN
            (soc_tomahawk2_tsc_max_lane_speed_get(unit, tsc_id, &lane_speed));
        if (speed > (num_lanes * lane_speed)) {
            LOG_VERBOSE(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Invalid speed configuration for port=%d, "
                                  "speed=%d: Max lane speed %d\n"),
                       port, speed, lane_speed));
            return SOC_E_PARAM;
        }
    }

    speed_mask = SOC_PA_SPEED(speed);
    SOC_IF_ERROR_RETURN
        (soc_th2_support_speeds(unit, num_lanes, &speed_valid));
    if (!(speed_valid & speed_mask)) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Invalid speed configuration for "
                              "port=%d, speed=%d\n"),
                   port, speed));
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

/*
 * All non-parity related interrupts must be handled in this function.
 * All unserviced intr_status bits must either be serviced or disabled.
 *
 * Note:
 * 1. MEM_PAR_ERR_STATf is handled by tomahawk/ser.c, so ignore this
 * 2. blocktype (in) can only be SOC_BLK_MMU_XPE/SC/SED
 */
int
soc_th2_mmu_non_ser_intr_handler(int unit, soc_block_t blocktype,
                                 int mmu_base_index,
                                 uint32 rval_intr_status_reg)
{
    int i;
    uint32 rval = 0;
    soc_reg_t intr_stat_reg, intr_clr_reg;
    static soc_field_t xpe_intr_field_list[] = {
        MEM_PAR_ERR_STATf, /* handled by tomahawk/ser.c, so ignore this */
        BST_THDI_INT_STATf,
        BST_THDO_INT_STATf,
        DEQ0_NOT_IP_ERR_STATf,
        DEQ1_NOT_IP_ERR_STATf,
        ENQX_TO_CFG_MAX_CELL_INTR_0_STATf,
        ENQX_TO_CFG_MAX_CELL_INTR_1_STATf,
        TCB_FREEZE_INT_STATf,
        INVALIDf
    };
    static soc_field_t sc_intr_field_list[] = {
        MEM_PAR_ERR_STATf, /* handled by tomahawk/ser.c, so ignore this */
        ES_X_DEADLOCK_DET_INT_STATf,
        ES_Y_DEADLOCK_DET_INT_STATf,
        ES_X_1IN6_ERR_INT_STATf,
        ES_Y_1IN6_ERR_INT_STATf,
        INVALIDf
    };
    static soc_field_t sed_intr_field_list[] = {
        CFAP_A_MEM_FAIL_STATf,
        CFAP_B_MEM_FAIL_STATf,
        MEM_PAR_ERR_STATf, /* handled by tomahawk/ser.c, so ignore this */
        BST_CFAP_A_INT_STATf,
        BST_CFAP_B_INT_STATf,
        START_BY_START_ERR_STATf,
        INVALIDf
    };

    if (blocktype == SOC_BLK_MMU_XPE) {
        intr_stat_reg = MMU_XCFG_XPE_CPU_INT_STATr;
        intr_clr_reg = MMU_XCFG_XPE_CPU_INT_CLEARr;
        for (i = 0; xpe_intr_field_list[i] != INVALIDf; i++) {
            if (!soc_reg_field_get(unit, intr_stat_reg,
                                   rval_intr_status_reg,
                                   xpe_intr_field_list[i])) {
                continue;
            }
            switch (xpe_intr_field_list[i]) {
            case BST_THDI_INT_STATf:
            case BST_THDO_INT_STATf:
                /* interrupt processing */
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Serviced %s intr "
                                              "from xpe = %0d \n"),
                             unit, SOC_FIELD_NAME(unit, xpe_intr_field_list[i]),
                             mmu_base_index));

                /* clear the interrupt */
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_xpe_reg32_get(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, &rval));
                rval |= (1 << i);
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_xpe_reg32_set(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, rval));
                SOC_IF_ERROR_RETURN
                    (_soc_th_process_mmu_bst(unit, mmu_base_index,
                                             xpe_intr_field_list[i]));
                break;
            case TCB_FREEZE_INT_STATf:
                /* interrupt processing */
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Serviced %s intr "
                                              "from xpe = %0d \n"),
                             unit, SOC_FIELD_NAME(unit, xpe_intr_field_list[i]),
                             mmu_base_index));

                /* clear the interrupt */
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_xpe_reg32_get(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, &rval));
                rval |= (1 << i);
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_xpe_reg32_set(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, rval));
                SOC_IF_ERROR_RETURN
                    (soc_th2_process_mmu_tcb(unit, mmu_base_index,
                                             xpe_intr_field_list[i]));
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "Unit: %0d -- Could not service %s "
                                            "intr from xpe = %0d \n"),
                           unit, SOC_FIELD_NAME(unit, xpe_intr_field_list[i]),
                           mmu_base_index));
                break;
            }
        }
        return SOC_E_NONE;
    }

    if (blocktype == SOC_BLK_MMU_SC) {
        intr_stat_reg = MMU_SCFG_SC_CPU_INT_STATr;
        intr_clr_reg = MMU_SCFG_SC_CPU_INT_CLEARr;
        for (i = 0; sc_intr_field_list[i] != INVALIDf; i++) {
            if (!soc_reg_field_get(unit, intr_stat_reg,
                                   rval_intr_status_reg,
                                   sc_intr_field_list[i])) {
                continue;
            }
            switch (sc_intr_field_list[i]) {
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "Unit: %0d -- Could not service %s "
                                            "intr from sc = %0d \n"),
                           unit, SOC_FIELD_NAME(unit, sc_intr_field_list[i]),
                           mmu_base_index));
                break;
            }
        }
        return SOC_E_NONE;
    }

    if (blocktype == SOC_BLK_MMU_SED) {
        intr_stat_reg = MMU_SEDCFG_SED_CPU_INT_STATr;
        intr_clr_reg = MMU_SEDCFG_SED_CPU_INT_CLEARr;
        for (i = 0; sed_intr_field_list[i] != INVALIDf; i++) {
            if (!soc_reg_field_get(unit, intr_stat_reg,
                                   rval_intr_status_reg,
                                   sed_intr_field_list[i])) {
                continue;
            }
            switch (sed_intr_field_list[i]) {
            case BST_CFAP_A_INT_STATf:
            case BST_CFAP_B_INT_STATf:
                /* interrupt processing */
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Serviced %s intr "
                                              "from sed = %0d \n"),
                             unit, SOC_FIELD_NAME(unit, sed_intr_field_list[i]),
                             mmu_base_index));


                /* clear the interrupt */
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk2_sed_reg32_get(unit, intr_clr_reg,
                                                 mmu_base_index, mmu_base_index,
                                                 0, &rval));
                rval |= (1 << i);
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk2_sed_reg32_set(unit, intr_clr_reg,
                                                 mmu_base_index, mmu_base_index,
                                                 0, rval));
                SOC_IF_ERROR_RETURN
                    (_soc_th_process_mmu_bst(unit, mmu_base_index,
                                             sed_intr_field_list[i]));
                break;
            case CFAP_A_MEM_FAIL_STATf:
            case CFAP_B_MEM_FAIL_STATf:
                /* interrupt processing */
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Serviced %s intr "
                                              "from sed = %0d \n"),
                             unit, SOC_FIELD_NAME(unit, sed_intr_field_list[i]),
                             mmu_base_index));

                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_CFAP_MEM_FAIL, -1, -1);

                /* clear the interrupt */
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk2_sed_reg32_get(unit, intr_clr_reg,
                                                 mmu_base_index, mmu_base_index,
                                                 0, &rval));
                rval |= (1 << i);
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk2_sed_reg32_set(unit, intr_clr_reg,
                                                 mmu_base_index, mmu_base_index,
                                                 0, rval));
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "Unit: %0d -- Could not service %s "
                                            "intr from sed = %0d \n"),
                           unit, SOC_FIELD_NAME(unit, sed_intr_field_list[i]),
                           mmu_base_index));
                break;
            }
        }
        return SOC_E_NONE;
    }

    /* cannot be here for any other blocktype */
    return SOC_E_FAIL;
}

/*
 * Function:
 *      soc_th2_max_lr_bandwidth_validate
 * Purpose:
 *      Check line rate bandwidth for each pipe
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      This function is called at early stage of initialization for
 *      port configuration check.
 */
int
soc_th2_max_lr_bandwidth_validate(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int port, pipe;
    uint32 max_lr_bw, lr_bw[_TH2_PIPES_PER_DEV] = {0};
    int rv;

    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->oversub_pbm, port) ||
            SOC_PBMP_MEMBER(si->all.disabled_bitmap, port) ||
            IS_MANAGEMENT_PORT(unit, port)) {
            continue;
        }
        pipe = si->port_pipe[port];
        if (pipe < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Failed to get pipe for port %d\n."),
                      port));
            return SOC_E_INTERNAL;
        }
        lr_bw[pipe] += si->port_speed_max[port];
    }

    rv = soc_th2_max_lr_bandwidth(unit, &max_lr_bw);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                            "Failed to get the Max Line Rate Bandwidth.\n")));
        return rv;
    }
    for (pipe = 0; pipe <  NUM_PIPE(unit); pipe++) {
        if (lr_bw[pipe] > max_lr_bw * 1000) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Bandwidth (%d Mbps) exceeds the Max Line "
                                  "Rate Bandwidth (%d Mbps) on pipe %d.\n"),
                                  lr_bw[pipe], max_lr_bw*1000, pipe));
            return SOC_E_CONFIG;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th2_speed_class_validate
 * Purpose:
 *      Check speed classes
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      This function is called at early stage of initialization for
 *      port configuration check.
 */
int
soc_th2_speed_class_validate(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int port, speed;
    uint32 speed_mask, count, speed_class_max_num;

    speed_mask = 0;
    speed_class_max_num = si->oversub_mode ?
                    _TH2_TDM_OS_SPEED_CLASS_MAX : _TH2_TDM_LR_SPEED_CLASS_MAX;
    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port) ||
            IS_MANAGEMENT_PORT(unit, port) || IS_CPU_PORT(unit, port) ||
            IS_LB_PORT(unit, port)) {
            continue;
        }
        speed = soc_port_speed_higig2eth(si->port_speed_max[port]);
        speed_mask |= SOC_PA_SPEED(speed);
    }

    count = _shr_popcount(speed_mask);
    if (count > speed_class_max_num) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "No port configurations with more than %d port "
                              "speed classes are supported.\n"),
                              speed_class_max_num));
        return SOC_E_CONFIG;
    }

    if(SOC_FAILURE(soc_th2_speed_mix_validate(unit, speed_mask))) {
        return SOC_E_CONFIG;
    }

    return SOC_E_NONE;
}

void soc_th2_uft_otp_info_get(int unit, int *num_share_banks_enabled,
    int *share_banks_bitmap_enabled)
{
    *num_share_banks_enabled = 
        (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNoUftBank0) ? 0 : 1) 
        + (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNoUftBank1) ? 0 : 1) 
        + (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNoUftBank2) ? 0 : 1) 
        + (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNoUftBank3) ? 0 : 1);
    
    *share_banks_bitmap_enabled = 
        (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNoUftBank0) ? 0 : 0x1) 
        | (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNoUftBank1) ? 0 : 0x2) 
        | (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNoUftBank2) ? 0 : 0x4) 
        | (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNoUftBank3) ? 0 : 0x8);

    return;
}

int
soc_th2_set_uft_bank_map(int unit)
{
    int num_banks, index;
    int num_shared_l2_banks, num_shared_l3_banks, num_shared_fpem_banks;
    int num_share_banks_enabled = 4;
    int share_banks_bitmap_enabled;    
    uint32 bank_counter = 0;
    uint32 unused_bank_bitmap = 0;    
    uint32 l2_bank_bitmap = 0;
    uint32 l3_bank_bitmap = 0;
    uint32 fpem_bank_bitmap = 0;
    uint32 config = 0, map = 0, fpmap = 0;    
    static soc_field_t l2_fields[] = {
        L2_ENTRY_BANK_2f, L2_ENTRY_BANK_3f, L2_ENTRY_BANK_4f,
        L2_ENTRY_BANK_5f
    };
    static soc_field_t l3_fields[] = {
        L3_ENTRY_BANK_4f, L3_ENTRY_BANK_5f, L3_ENTRY_BANK_6f,
        L3_ENTRY_BANK_7f
    };
    static soc_field_t fpem_fields[] = {
        FPEM_ENTRY_BANK_0f, FPEM_ENTRY_BANK_1f, FPEM_ENTRY_BANK_2f,
        FPEM_ENTRY_BANK_3f
    };

    SOC_IF_ERROR_RETURN
        (soc_tomahawk_hash_bank_count_get(unit, L2Xm, &num_banks));
    num_shared_l2_banks = num_banks - 2; /* minus 2 dedicated L2 banks */
    SOC_IF_ERROR_RETURN
        (soc_tomahawk_hash_bank_count_get(unit, L3_ENTRY_ONLYm, &num_banks));
    num_shared_l3_banks = num_banks - 4; /* minus 4 dedicated L3 banks */
    SOC_IF_ERROR_RETURN
        (soc_tomahawk_hash_bank_count_get(unit, EXACT_MATCH_2m,
                                          &num_shared_fpem_banks));

    soc_th2_uft_otp_info_get(unit, &num_share_banks_enabled,
        &share_banks_bitmap_enabled);

    if (soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m)) {
        int non_alpm = num_shared_fpem_banks + num_shared_l3_banks +
            num_shared_l2_banks;
        if (((non_alpm) && (non_alpm <= (num_share_banks_enabled - 2)))
            /* If any of B4, B5 banks is disabled, ALPM can only use two banks*/
            ||(~share_banks_bitmap_enabled & 0xc)) {
            /* If Shared banks are used between ALPM and non-ALPM memories,
             * then ALPM uses Shared Bank B2, B3 and non-ALPM uses B4, B5 banks
             */
            soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                              ALPM_ENTRY_BANK_CONFIGf, 0x3);
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                              ALPM_BANK_MODEf, 1);
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_LOG_TO_PHY_BANK_MAP_2r,
                                        REG_PORT_ANY, ALPM_BANK_MODEf, 1));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_MEMORY_CONTROL_84r,
                                        REG_PORT_ANY, BYPASS_ISS_MEMORY_LPf, 0x3));
            soc_th_set_alpm_banks(unit, 2);
        } else {
            soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                              ALPM_ENTRY_BANK_CONFIGf, 0xf);
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                              ALPM_BANK_MODEf, 0);
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_LOG_TO_PHY_BANK_MAP_2r,
                                        REG_PORT_ANY, ALPM_BANK_MODEf, 0));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_MEMORY_CONTROL_84r,
                                        REG_PORT_ANY, BYPASS_ISS_MEMORY_LPf, 0xf));
            soc_th_set_alpm_banks(unit, 4);
        }
        
        /* get the unused enabled bank bitmap */    
        unused_bank_bitmap = (~((1 << soc_th_get_alpm_banks(unit)) - 1)) 
                                & share_banks_bitmap_enabled;        
    }    else {
        unused_bank_bitmap = share_banks_bitmap_enabled;     
    }

    bank_counter = 0;
    /* get l2_bank_bitmap */
    for (index = 0; index < 4; index++) {
        if (bank_counter == num_shared_l2_banks) {
            break;
        }
        if (unused_bank_bitmap & (1 << index)) {
            l2_bank_bitmap |= 1 << index;
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                          l2_fields[bank_counter], index);
            bank_counter++;
        }
    }   
    soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                      L2_ENTRY_BANK_CONFIGf, l2_bank_bitmap);
    
    unused_bank_bitmap &= (~l2_bank_bitmap);
    /* get l3_bank_bitmap */
    bank_counter = 0;
    for (index = 0; index < 4; index++) {
        if (bank_counter == num_shared_l3_banks) {
            break;
        }
        if (unused_bank_bitmap & (1 << index)) {
            l3_bank_bitmap |= 1 << index;
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                          l3_fields[bank_counter], index);
            bank_counter++;
        }
    }     
    soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                      L3_ENTRY_BANK_CONFIGf, l3_bank_bitmap);
    
    unused_bank_bitmap &= (~l3_bank_bitmap);
    /* get fpem_bank_bitmap */
    bank_counter = 0;
    for (index = 0; index < 4; index++) {
        if (bank_counter == num_shared_fpem_banks) {
            break;
        }
        if (unused_bank_bitmap & (1 << index)) {
            fpem_bank_bitmap |= 1 << index;
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                              fpem_fields[bank_counter], index);
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAP_3r, &fpmap,
                              fpem_fields[bank_counter], index);            
            bank_counter++;
        }
    }    
    soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                      FPEM_ENTRY_BANK_CONFIGf, fpem_bank_bitmap);

    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, ISS_BANK_CONFIGr, REG_PORT_ANY, 0, config));
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, REG_PORT_ANY, 0, map));
    SOC_IF_ERROR_RETURN
        (soc_th_iss_log_to_phy_bank_map_shadow_set(unit, map));
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, ISS_LOG_TO_PHY_BANK_MAP_3r, REG_PORT_ANY, 0, fpmap));

    return SOC_E_NONE;
}
#endif /* BCM_TOMAHAWK2_SUPPORT */
