/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        wolfhound2.c
 * Purpose:
 * Requires:
 */

#include <sal/core/boot.h>
#include <soc/bradley.h>
#include <soc/wolfhound2.h>
#include <soc/hurricane3.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/devids.h>
#include <soc/defs.h>
#include <soc/mspi.h>

#include <shared/util.h>
#include <shared/bsl.h>


#ifdef BCM_HURRICANE3_SUPPORT

#define WH2_MAX_PHY_PORTS   (38)

static const int p2l_mapping_default[] = {
    0, -1,  2,  3,  4,  5,  6,  7,
    8,  9, 10, 11, 12, 13, -1, -1,
   -1, -1, 14, 15, 16, 17, 18, 19,
   20, 21, 22, 23, 24, 25, -1, -1,
   -1, -1, 26, 27, 28, 29
};

static const int port_speed_max_default[] = {
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, -1, -1,
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, -1, -1,
    -1, -1, 25, 25, 25, 25
};

static const int tdm_24p[] = {
     2, 10, 18, 34,  3, 11, 19, 35,
     4, 12, 20, 36,  5, 13, 21, 37,
     6, 26, 22, 34,  7, 27, 23, 35,
     8, 28, 24, 36,  9, 29, 25, 37,
     0, 63
};

static const int port_speed_max_op1[] = {
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, -1, -1,
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 25, 25, 25, 25, -1, -1,
    -1, -1, 25, 25, 25, 25
};

static const int port_speed_max_op2[] = {
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, -1, -1,
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, -1, -1,
    -1, -1, 25, 25, 25, 25
};

static const int port_speed_max_op3[] = {
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, -1, -1,
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, -1, -1,
    -1, -1, 25, 25, 25, 25
};

static const int port_speed_max_op4_5_6[] = {
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, -1, -1,
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, -1, -1,
    -1, -1, 10, 10, 10, 10
};

static const int port_speed_max_op7[] = {
    -1, -1, 10, 10, 10, 10, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 25, 25, 25, 25, -1, -1,
    -1, -1, 25, 25, 25, 25
};

static const int port_speed_max_op8[] = {
    -1, -1, 10, 10, 10, 10, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, -1, -1,
    -1, -1, 25, 25, 25, 25
};

static const int port_speed_max_op9[] = {
    -1, -1, 10, 10, 10, 10, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, -1, -1,
    -1, -1, 25, 25, 25, 25
};

static const int port_speed_max_op10_11_12[] = {
    -1, -1, 10, 10, 10, 10, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, -1, -1,
    -1, -1, 10, 10, 10, 10
};

static const int port_speed_max_op13[] = {
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 10, 10, 10, 10, -1, -1,
    -1, -1, 25, 25, 25, 25, -1, -1,
    -1, -1, 25, 25, 25, 25
};

static const int port_speed_max_op14[] = {
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 10, 10, 10, 10, -1, -1,
    -1, -1, 10, 10, 10, 10, -1, -1,
    -1, -1, 25, 25, 25, 25
};

static const int port_speed_max_op15[] = {
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 10, 10, 10, 10, -1, -1,
    -1, -1, 10, 10, 10, 10, -1, -1,
    -1, -1, 25, 25, 25, 25
};

static const int port_speed_max_op16_17_18[] = {
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 10, 10, 10, 10, -1, -1,
    -1, -1, 10, 10, 10, 10, -1, -1,
    -1, -1, 10, 10, 10, 10
};

static const int p2l_mapping_tdm_0[] = {
     0, -1,  2,  3,  4,  5,  6,  7,
     8,  9, 10, 11, 12, 13, -1, -1,
    -1, -1, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, -1, -1,
    -1, -1, 26, 27, 28, 29
};

static const int tdm_0[] = {
     2, 10, 18, 34,  3, 11, 19, 35,
     4, 12, 20, 36,  5, 13, 21, 37,
     6, 26, 22, 34,  7, 27, 23, 35,
     8, 28, 24, 36,  9, 29, 25, 37,
     0, 63
};

static const int p2l_mapping_tdm_1[] = {
     0, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  2,  3,  4,  5, -1, -1,
    -1, -1,  6,  7,  8,  9, -1, -1,
    -1, -1, 10, 11, 12, 13
};

static const int tdm_1[] = {
    18, 26, 34,  0, 19, 27, 35, 63,
    20, 28, 36, 63, 21, 29, 37, 63
};

static const int p2l_mapping_tdm_2[] = {
     0, -1,  2,  3,  4,  5,  6,  7,
     8,  9, 10, 11, 12, 13, -1, -1,
    -1, -1, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, -1, -1,
    -1, -1, 26, 27, 28, 29
};

static const int tdm_2[] = {
     2, 10, 18, 34,  3, 11, 19, 35,
     4, 12, 20, 36,  5, 13, 21, 37,
     6, 26, 22,  0,  7, 27, 23, 63,
     8, 28, 24, 63,  9, 29, 25, 63
};

static const int p2l_mapping_tdm_3[] = {
     0, -1,  2,  3,  4,  5,  6,  7,
     8,  9, 10, 11, 12, 13, -1, -1,
    -1, -1, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, -1, -1,
    -1, -1, 26, 27, 28, 29
};

static const int tdm_3[] = {
     2, 18, 26, 34, 10,  3, 27, 35,
    19, 11, 28, 36,  4, 20, 29, 37,
    12,  5, 26, 34, 21, 13, 27, 35,
     6, 22, 28, 36,  7, 23, 29, 37,
     8, 24,  0, 63,  9, 25, 63, 63
};

static const int p2l_mapping_tdm_4[] = {
     0, -1,  2,  3,  4,  5, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  6,  7,  8,  9, 10, 11,
    12, 13, 14, 15, 16, 17, -1, -1,
    -1, -1, 18, 19, 20, 21
};

static const int tdm_4[] = {
     2, 18, 26, 34,  3, 19, 27, 35,
     4, 20, 28, 36,  5, 21, 29, 37,
     0, 22, 26, 34, 63, 23, 27, 35,
    63, 24, 28, 36, 63, 25, 29, 37
};

static const soc_port_t lport_dport_map_0[] = {
    0, -1, 13, 12, 11, 10,  9,  8,
    7,  6,  5,  4,  3,  2, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23,
   24, 25, 26, 27, 28, 29
};

static const soc_port_t lport_dport_map_1[] = {
    0, -1, -1, -1, -1, -1, -1, -1,
   -1, -1,  5,  4,  3,  2,  6,  7,
    8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21
};

static const soc_port_t lport_dport_map_2[] = {
    0, -1, -1, -1, -1, -1, -1, -1,
   -1, -1,  5,  4,  3,  2, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23,
   24, 25, 26, 27, 28, 29
};

static const soc_port_t lport_dport_map_3[] = {
    0, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1,  2,  3,
    4,  5, -1, -1, -1, -1,  6,  7,
    8,  9, 10, 11, 12, 13
};

static int matched_devid_idx[SOC_MAX_NUM_DEVICES] = {-1};

typedef struct wh2_sku_info_s {
    uint16      dev_id;
    int         config_option;
    int         frequency;
    char        *op_str;
    const int   *p2l_mapping;
    const int   *speed_max;
    const int   *dport_mapping;
    const int   *tdm_table;
    int         tdm_table_size;
    uint32      qgphy_core_map;
    uint8       qgphy5_lane;
    uint8       sgmii_4p0_lane;
} wh2_sku_info_t;


STATIC wh2_sku_info_t wh2_sku_port_config[] = {
    /* 53540 : 24 + 4 */
    {BCM53540_DEVICE_ID, 2, 78, "op2",
     p2l_mapping_default, port_speed_max_default, lport_dport_map_0,
     tdm_24p, 34, 0x3F, 0xF, 0
    },
    {BCM53547_DEVICE_ID, 1, 104, "op1",
     p2l_mapping_tdm_3, port_speed_max_op1, lport_dport_map_0,
     tdm_3, 40, 0x1F, 0, 0xF
    },
    {BCM53547_DEVICE_ID, 2, 104, "op2",
     p2l_mapping_tdm_0, port_speed_max_op2, lport_dport_map_0,
     tdm_0, 34, 0x3F, 0xF, 0
    },
    {BCM53547_DEVICE_ID, 3, 104, "op3",
     p2l_mapping_tdm_0, port_speed_max_op3, lport_dport_map_0,
     tdm_0, 34, 0x3F, 0x3, 0xC
    },
    {BCM53547_DEVICE_ID, 4, 62, "op4",
     p2l_mapping_tdm_2, port_speed_max_op4_5_6, lport_dport_map_0,
     tdm_2, 32, 0x3F, 0x3, 0xC
    },
    {BCM53547_DEVICE_ID, 5, 62, "op5",
     p2l_mapping_tdm_2, port_speed_max_op4_5_6, lport_dport_map_0,
     tdm_2, 32, 0x1F, 0, 0xF
    },
    {BCM53547_DEVICE_ID, 6, 62, "op6",
     p2l_mapping_tdm_2, port_speed_max_op4_5_6, lport_dport_map_0,
     tdm_2, 32, 0x3F, 0xF, 0
    },
    {BCM53548_DEVICE_ID, 7, 104, "op7",
     p2l_mapping_tdm_4, port_speed_max_op7, lport_dport_map_1,
     tdm_4, 32, 0x19, 0, 0xF
    },
    {BCM53548_DEVICE_ID, 8, 104, "op8",
     p2l_mapping_tdm_0, port_speed_max_op8, lport_dport_map_2,
     tdm_0, 34, 0x39, 0xF, 0
    },
    {BCM53548_DEVICE_ID, 9, 104, "op9",
     p2l_mapping_tdm_0, port_speed_max_op9, lport_dport_map_2,
     tdm_0, 34, 0x39, 0x3, 0xC
    },
    {BCM53548_DEVICE_ID, 10, 62, "op10",
     p2l_mapping_tdm_2, port_speed_max_op10_11_12, lport_dport_map_2,
     tdm_2, 32, 0x39, 0x3, 0xC
    },
    {BCM53548_DEVICE_ID, 11, 62, "op11",
     p2l_mapping_tdm_2, port_speed_max_op10_11_12, lport_dport_map_2,
     tdm_2, 32, 0x19, 0, 0xF
    },
    {BCM53548_DEVICE_ID, 12, 62, "op12",
     p2l_mapping_tdm_2, port_speed_max_op10_11_12, lport_dport_map_2,
     tdm_2, 32, 0x39, 0xF, 0
    },
    {BCM53549_DEVICE_ID, 13, 104, "op13",
     p2l_mapping_tdm_1, port_speed_max_op13, lport_dport_map_3,
     tdm_1, 16, 0x8, 0, 0xF
    },
    {BCM53549_DEVICE_ID, 14, 104, "op14",
     p2l_mapping_tdm_1, port_speed_max_op14, lport_dport_map_3,
     tdm_1, 16, 0x28, 0xF, 0
    },
    {BCM53549_DEVICE_ID, 15, 104, "op15",
     p2l_mapping_tdm_1, port_speed_max_op15, lport_dport_map_3,
     tdm_1, 16, 0x28, 0x3, 0xC
    },
    {BCM53549_DEVICE_ID, 16, 41, "op16",
     p2l_mapping_tdm_1, port_speed_max_op16_17_18, lport_dport_map_3,
     tdm_1, 16, 0x28, 0x3, 0xC
    },
    {BCM53549_DEVICE_ID, 17, 41, "op17",
     p2l_mapping_tdm_1, port_speed_max_op16_17_18, lport_dport_map_3,
     tdm_1, 16, 0x8, 0, 0xF
    },
    {BCM53549_DEVICE_ID, 18, 41, "op18",
     p2l_mapping_tdm_1, port_speed_max_op16_17_18, lport_dport_map_3,
     tdm_1, 16, 0x28, 0xF, 0
    },
    {0}
};

int
soc_wolfhound2_port_config_init(int unit, uint16 dev_id)
{
    soc_info_t      *si;
    int             phy_port, lport;
    const int       *p2l_mapping, *port_speed_max;
    wh2_sku_info_t  *sku_info, *matched_port_config = NULL;
    char            *option_str;
    int             valid_sku_option, i;

    option_str = soc_property_get_str(unit, "init_port_config_option");

    valid_sku_option = -1;
    matched_devid_idx[unit] = -1;

    for (i = 0; wh2_sku_port_config[i].dev_id != 0; i++) {
        sku_info = &wh2_sku_port_config[i];
        if (dev_id == sku_info->dev_id) {
            if (option_str == NULL) {
                option_str = sku_info->op_str;
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "Warning: init_port_config_option is not "
                                     "specified. Take %s for %s \n"),
                          option_str, soc_dev_name(unit)));
            }
            if (matched_devid_idx[unit] == -1) {
                matched_devid_idx[unit] = i;
            }
            if (!sal_strcmp(sku_info->op_str, option_str)) {
                valid_sku_option = i;
                break;
            }
        }
    }
    assert(matched_devid_idx[unit] != -1);

    if (valid_sku_option == -1) {
        valid_sku_option =
            wh2_sku_port_config[matched_devid_idx[unit]].config_option;
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Warning: option %s is not supported in %s. "
                             "Take %s\n"),
                  option_str, soc_dev_name(unit),
                  wh2_sku_port_config[matched_devid_idx[unit]].op_str));
    }
    matched_devid_idx[unit] = valid_sku_option;

    matched_port_config = &wh2_sku_port_config[matched_devid_idx[unit]];
    p2l_mapping = matched_port_config->p2l_mapping;
    port_speed_max = matched_port_config->speed_max;

    si = &SOC_INFO(unit);
    for (phy_port = 0; phy_port < WH2_MAX_PHY_PORTS; phy_port++) {
        if ((port_speed_max[phy_port] == -1) && (phy_port != 0)) {
            si->port_p2l_mapping[phy_port] = -1;
            si->port_p2m_mapping[phy_port] = -1;
            si->max_port_p2m_mapping[phy_port] = -1;
            si->port_num_lanes[p2l_mapping[phy_port]] = -1;
        } else {
            si->port_p2l_mapping[phy_port] = p2l_mapping[phy_port];
            si->port_p2m_mapping[phy_port] = p2l_mapping[phy_port];
            si->max_port_p2m_mapping[phy_port] = p2l_mapping[phy_port];
            lport = si->port_p2l_mapping[phy_port];
            si->port_speed_max[lport] = port_speed_max[phy_port] * 100;
            si->port_num_lanes[lport] = 1;
        }
    }
    si->frequency = matched_port_config->frequency;
    return SOC_E_NONE;
}

int
soc_wolfhound2_dport_init(int unit)
{
    int             dport;
    wh2_sku_info_t  *matched_port_config = NULL;
    const int       *l2d_mapping;

    matched_port_config = &wh2_sku_port_config[matched_devid_idx[unit]];
    l2d_mapping = matched_port_config->dport_mapping;

    if (l2d_mapping == NULL) {
        return SOC_E_NONE;
    }

    for (dport = 0; dport < 30; dport++) {
        SOC_DPORT_MAP(unit)[dport] = l2d_mapping[dport];
    }

    return SOC_E_NONE;
}

int
soc_wolfhound2_gphy_get(int unit, int port, uint8 *is_gphy)
{
    wh2_sku_info_t  *matched_port_config = NULL;
    soc_info_t      *si;
    int             qgphy_id, phy_port;
    uint32          qgphy_core_map;
    uint8           qgphy5_lane;

    matched_port_config = &wh2_sku_port_config[matched_devid_idx[unit]];
    qgphy_core_map = matched_port_config->qgphy_core_map;
    qgphy5_lane = matched_port_config->qgphy5_lane;

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];

    /* get the quad gphy id */
    /* 0:2-5, 1:6-9, 2:10-13, 3:18-21, 4:22-25, 5:26-29 */
    qgphy_id = (phy_port - 2) / 4;
    if (phy_port >= 34) {
        /* SGMII_4P1 => qgphy_id = -1 */
        qgphy_id = -1;
    } else if (phy_port >= 18) {
        qgphy_id--;
    }

    if (qgphy_id != -1) {
        if (qgphy_core_map & (1 << qgphy_id)) {
            if (qgphy_id == 5) {
                if (qgphy5_lane & (1 << (phy_port - 2) % 4)) {
                    *is_gphy = 1;
                } else {
                    *is_gphy = 0;
                }

            } else {
                *is_gphy = 1;
            }
        } else {
            *is_gphy = 0;
        }
    } else {
        *is_gphy = 0;
    }
    return SOC_E_NONE;
}

int
soc_wolfhound2_features(int unit, soc_feature_t feature)
{
    switch (feature) {
        /* Wolfhound2 specific feature (different from HR3) */
        case soc_feature_field_multi_stage: /* no VFP, EFP in WH2 */
        case soc_feature_field_slice_size128:
        case soc_feature_xlmac:
        case soc_feature_uc:
        case soc_feature_rcpu_priority: /* CMIC_PKT_CTRL not in regfile */
        case soc_feature_l3_dynamic_ecmp_group:
        case soc_feature_e2ecc: /* no E2ECC */
        case soc_feature_discard_ability: /* no WRED */
        case soc_feature_wred_drop_counter_per_port: /* no WRED */
        case soc_feature_ecn_wred: /* no ECN */
        case soc_feature_miml: /* no MiML */
        case soc_feature_miml_no_l3: /* no MiML when no L3 */
        case soc_feature_higig_lookup:
        case soc_feature_proxy_port_property:
        case soc_feature_gmii_clkout: /* no SyncE in WH2 */
            return FALSE;
        case soc_feature_wh2:
        case soc_feature_gphy:  /* need to consider sku later */
        case soc_feature_time_v3_no_bs:
        case soc_feature_no_qt_gport: /* wh2 qt has no phy model in QT */
        case soc_feature_l3_no_ecmp:
        case soc_feature_l3:
        case soc_feature_l3_ip6:
        case soc_feature_l3_entry_key_type:
        case soc_feature_lpm_tcam:
        case soc_feature_ip_mcast:
        case soc_feature_ip_mcast_repl:
        case soc_feature_ipmc_unicast:
        case soc_feature_ipmc_use_configured_dest_mac:
        case soc_feature_l3mc_use_egress_next_hop:
        case soc_feature_l3_sgv:
        case soc_feature_urpf:
        case soc_feature_l3_ingress_interface:
        case soc_feature_l3_iif_zero_invalid:
        case soc_feature_l3_iif_under_4k:
        case soc_feature_hg_trunking:
        case soc_feature_hg_trunk_override:
        case soc_feature_hg_trunk_failover:
        case soc_feature_hg_trunk_group_members_max_4:
        case soc_feature_no_mirror_truncate:
            return TRUE;

        default :
            return _soc_hurricane3_features(unit, feature);
    }
}

/*
 * soc_wolfhound2_mem_config:
 * Over-ride the default table sizes (from regsfile) for any SKUs here
 */
int
soc_wolfhound2_mem_config(int unit, int dev_id)
{
    int rv = SOC_E_NONE;
    soc_persist_t *sop = SOC_PERSIST(unit);
    switch (dev_id) {
        case BCM53540_DEVICE_ID:
        case BCM53547_DEVICE_ID:
        case BCM53548_DEVICE_ID:
        case BCM53549_DEVICE_ID:
        default:
            /* No EFP and VFP in WH2 family */
            sop->memState[EFP_COUNTER_TABLEm].index_max = -1;
            sop->memState[EFP_METER_TABLEm].index_max = -1;
            sop->memState[EFP_POLICY_TABLEm].index_max = -1;
            sop->memState[EFP_TCAMm].index_max = -1;
            sop->memState[VFP_POLICY_TABLEm].index_max = -1;
            sop->memState[VFP_TCAMm].index_max = -1;
            SOC_CONTROL(unit)->l3_defip_max_tcams = 1;
            SOC_CONTROL(unit)->l3_defip_tcam_size = 64;
            /* Module ID num: 64(0 ~ 63), not 256 */
            sop->memState[MODPORT_MAPm].index_max = 383;
            break;
    }
    if (SAL_BOOT_QUICKTURN) {
        /* QuickTurn with limited TCAM entries */
        sop->memState[L2_USER_ENTRYm].index_max = 11;
        sop->memState[L2_USER_ENTRY_ONLYm].index_max = 11;
        sop->memState[L2_USER_ENTRY_DATA_ONLYm].index_max = 11;
        sop->memState[CPU_COS_MAPm].index_max = 11;
        sop->memState[CPU_COS_MAP_ONLYm].index_max = 11;
        sop->memState[FP_GLOBAL_MASK_TCAMm].index_max = 29;
        sop->memState[VLAN_SUBNETm].index_max = 11;
        sop->memState[VLAN_SUBNET_ONLYm].index_max = 11;
        sop->memState[VLAN_SUBNET_DATA_ONLYm].index_max = 11;
        sop->memState[L3_DEFIPm].index_max = 31;
        sop->memState[L3_DEFIP_ONLYm].index_max = 31;
        sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 31;
        sop->memState[FP_TCAMm].index_max = 29;
    }
    return rv;
}

#define WH2_QGPHY_CORE_AMAC             (5)
#define WH2_QGPHY_CORE_ALL              (0x3F)
#define WH2_QGPHY_CORE_PBMP_ALL         (0xFFFFFF)
#define WH2_QGPHY_CORE_AMAC_BIT_SHIFT   (20)

static const uint32 wh2_qgphy_core_pbmp [6] = {
    0xF, 0xF0, 0xF00, 0xF000, 0xF0000, 0xF00000
};

STATIC int
soc_wh2_qgphy_init(int unit, uint32 qgphy_core_map, uint8 qgphy5_lane)
{
    int     amac = 0, i;
    uint32  pbmp, core, core_temp, iddq_pwr, iddq_bias, rval;
    uint32  pbmp_temp = 0;
    int     sleep_usec = 1100;

    pbmp = 0;
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    core = soc_reg_field_get(unit, TOP_SOFT_RESET_REGr, rval, TOP_QGPHY_RST_Lf);

    /* Check if gphy5 is up and used by AMAC */
    if (core & (1 << WH2_QGPHY_CORE_AMAC)) {
        if (soc_cm_get_bus_type(unit) & SOC_AXI_DEV_TYPE) {
            amac = 1;
        }
    }

    /* power up the QGPHY */
    for (i = 0; i < 6; i++) {
        if (qgphy_core_map & (1 << i)) {
            pbmp_temp |= wh2_qgphy_core_pbmp[i];
        }
    }
    if (amac) {
        pbmp_temp &= ~wh2_qgphy_core_pbmp[WH2_QGPHY_CORE_AMAC];
    }
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_0r(unit, &rval));
    pbmp = soc_reg_field_get(unit, TOP_QGPHY_CTRL_0r, rval, EXT_PWRDOWNf);
    pbmp &= ~pbmp_temp;
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_0r, &rval, EXT_PWRDOWNf, pbmp);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_0r(unit, rval));
    sal_usleep(sleep_usec);

    /* Release iddq isolation */
    core = qgphy_core_map;
    if ((amac) || (qgphy5_lane == 0x3)) {
        core &= ~(1 << WH2_QGPHY_CORE_AMAC);
    }
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
    iddq_pwr = soc_reg_field_get(unit, TOP_QGPHY_CTRL_2r, rval,
                                 GPHY_IDDQ_GLOBAL_PWRf);
    iddq_bias = soc_reg_field_get(unit, TOP_QGPHY_CTRL_2r, rval,
                                  IDDQ_BIASf);
    iddq_pwr &= ~core;
    iddq_bias &= ~core;
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval,
                      GPHY_IDDQ_GLOBAL_PWRf, iddq_pwr);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval,
                      IDDQ_BIASf, iddq_bias);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
    sal_usleep(sleep_usec);

    /* Toggle reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    core_temp = soc_reg_field_get(unit, TOP_SOFT_RESET_REGr, rval,
                                  TOP_QGPHY_RST_Lf);
    core_temp |= core;
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval,
                      TOP_QGPHY_RST_Lf, core_temp);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(sleep_usec);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,"QGPHY 0x%x are power up. "
                            "pwrdown bitmap %x (amac=%d)\n"),
                 core_temp, pbmp, amac));

    /* Full Quad power down */
    pbmp_temp = 0;
    core_temp = WH2_QGPHY_CORE_ALL;
    core_temp &= ~qgphy_core_map;
    if (amac) {
        /* Avoid power down qgphy5 when amac is using it */
        core_temp &= ~(1 << WH2_QGPHY_CORE_AMAC);
    } else if (qgphy5_lane == 0x3) {
        /* Power down qgphy 5 first then enable some bitmap later */
        core_temp |= (1 << WH2_QGPHY_CORE_AMAC);
    }
    if (core_temp) {
        for (i = 0; i < 6; i++) {
            if (core_temp & (1 << i)) {
                pbmp_temp |= wh2_qgphy_core_pbmp[i];
            }
        }
        SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_0r(unit, &rval));
        pbmp = soc_reg_field_get(unit, TOP_QGPHY_CTRL_0r, rval, EXT_PWRDOWNf);
        pbmp |= pbmp_temp;
        soc_reg_field_set(unit, TOP_QGPHY_CTRL_0r, &rval, EXT_PWRDOWNf, pbmp);
        SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_0r(unit, rval));
        sal_usleep(sleep_usec);

        /* Toggle iddq isolation and reset */
        /* iddq_pw1 = 1,  iddq_bias = 1, reset_n = 1*/
        SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
        iddq_pwr = soc_reg_field_get(unit, TOP_QGPHY_CTRL_2r, rval,
                                     GPHY_IDDQ_GLOBAL_PWRf);
        iddq_bias = soc_reg_field_get(unit, TOP_QGPHY_CTRL_2r, rval,
                                      IDDQ_BIASf);
        iddq_pwr |= core_temp;
        iddq_bias |= core_temp;
        soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval,
                          GPHY_IDDQ_GLOBAL_PWRf, iddq_pwr);
        soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval,
                          IDDQ_BIASf, iddq_pwr);
        SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
        sal_usleep(sleep_usec);

        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
        core = soc_reg_field_get(unit, TOP_SOFT_RESET_REGr, rval,
                                 TOP_QGPHY_RST_Lf);
        core |= core_temp;
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval,
                          TOP_QGPHY_RST_Lf, core);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
        sal_usleep(sleep_usec);

        /* iddq_pw1 = 0,  iddq_bias = 0, reset_n = 0*/
        SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
        iddq_pwr = soc_reg_field_get(unit, TOP_QGPHY_CTRL_2r, rval,
                                     GPHY_IDDQ_GLOBAL_PWRf);
        iddq_bias = soc_reg_field_get(unit, TOP_QGPHY_CTRL_2r, rval,
                                  IDDQ_BIASf);
        iddq_pwr &= ~core_temp;
        iddq_bias &= ~core_temp;
        soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval,
                          GPHY_IDDQ_GLOBAL_PWRf, iddq_pwr);
        soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval,
                          IDDQ_BIASf, iddq_pwr);
        SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
        sal_usleep(sleep_usec);

        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
        core = soc_reg_field_get(unit, TOP_SOFT_RESET_REGr, rval,
                                 TOP_QGPHY_RST_Lf);
        core &= ~core_temp;
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval,
                          TOP_QGPHY_RST_Lf, core);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
        sal_usleep(sleep_usec);

        /* reset_n = 1, iddq_pw1 = 1,  iddq_bias = 1 */
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
        core = soc_reg_field_get(unit, TOP_SOFT_RESET_REGr, rval,
                                 TOP_QGPHY_RST_Lf);
        core |= core_temp;
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval,
                          TOP_QGPHY_RST_Lf, core);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
        sal_usleep(sleep_usec);

        SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
        iddq_pwr = soc_reg_field_get(unit, TOP_QGPHY_CTRL_2r, rval,
                                     GPHY_IDDQ_GLOBAL_PWRf);
        iddq_bias = soc_reg_field_get(unit, TOP_QGPHY_CTRL_2r, rval,
                                  IDDQ_BIASf);
        iddq_pwr |= core_temp;
        iddq_bias |= core_temp;
        soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval,
                          GPHY_IDDQ_GLOBAL_PWRf, iddq_pwr);
        soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval,
                          IDDQ_BIASf, iddq_pwr);
        SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
        sal_usleep(sleep_usec);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,"Disable QGPHY 0x%x "
                                "pwrdown bitmap %x\n"),
                     core_temp, pbmp));
    }

    /* Partial power up */
    if (qgphy5_lane == 0x3) {
        if (!amac) {
            /* iddq_pw1 = 0,  iddq_bias = 0*/
            SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
            iddq_pwr = soc_reg_field_get(unit, TOP_QGPHY_CTRL_2r, rval,
                                         GPHY_IDDQ_GLOBAL_PWRf);
            iddq_bias = soc_reg_field_get(unit, TOP_QGPHY_CTRL_2r, rval,
                                      IDDQ_BIASf);
            iddq_pwr &= ~(1 << WH2_QGPHY_CORE_AMAC);
            iddq_bias &= ~(1 << WH2_QGPHY_CORE_AMAC);
            soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval,
                              GPHY_IDDQ_GLOBAL_PWRf, iddq_pwr);
            soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval,
                              IDDQ_BIASf, iddq_pwr);
            SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
            sal_usleep(sleep_usec);
        }
        /* power up qghphy lane */
        SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_0r(unit, &rval));
        pbmp = soc_reg_field_get(unit, TOP_QGPHY_CTRL_0r, rval,
                                 EXT_PWRDOWNf);
        pbmp &= ~(qgphy5_lane << WH2_QGPHY_CORE_AMAC_BIT_SHIFT);
        soc_reg_field_set(unit, TOP_QGPHY_CTRL_0r, &rval, EXT_PWRDOWNf, pbmp);
        SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_0r(unit, rval));
        sal_usleep(sleep_usec);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "Partial power up qgphy5 lane 0/1\n")));
    }

    return SOC_E_NONE;
}

STATIC int
soc_wh2_gphy_pmq_init(int unit, int pmq_inst)
{
    soc_reg_t       reg;
    int             blk, pmq_port;
    uint32          rval;
    int             sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;
    soc_info_t      *si;

    if (pmq_inst > 1) {
        /* PMQ 0,1 control GPHY related configurations */
        return SOC_E_INTERNAL;
    }

    reg = GPORT_XGXS0_CTRL_REGr;
    si = &SOC_INFO(unit);
    blk = si->pmq_block[pmq_inst];
    pmq_port = blk | SOC_REG_ADDR_BLOCK_ID_MASK;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pmq_port, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, IDDQf, 0);
    /* Deassert power down */
    soc_reg_field_set(unit, reg, &rval, PWRDWNf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pmq_port, 0, rval));
    sal_usleep(sleep_usec);

    /* Reset XGXS */
    soc_reg_field_set(unit, reg, &rval, RSTB_HWf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pmq_port, 0, rval));
    sal_usleep(sleep_usec + 10000);

    /* Bring XGXS out of reset */
    soc_reg_field_set(unit, reg, &rval, RSTB_HWf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pmq_port, 0, rval));
    sal_usleep(sleep_usec);

    /* Activate MDIO on SGMII */
    soc_reg_field_set(unit, reg, &rval, RSTB_REFCLKf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pmq_port, 0, rval));
    sal_usleep(sleep_usec);

    /* Activate clocks */
    soc_reg_field_set(unit, reg, &rval, RSTB_PLLf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pmq_port, 0, rval));

    return SOC_E_NONE;
}

STATIC int
soc_wh2_sgmii_init(int unit, int sgmii_inst)
{
    soc_reg_t       reg;
    int             blk, pmq_port;
    uint32          rval;
    int             sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;
    soc_info_t      *si;

    if (sgmii_inst == 0) {
        /* TOP_SGMII_CTRL_REGr */
        reg = TOP_SGMII_CTRL_REGr;
        pmq_port = REG_PORT_ANY;
    } else if (sgmii_inst == 1) {
        /* GPORT_SGMII0_CTRL_REGr in PMQ2 */
        reg = GPORT_SGMII0_CTRL_REGr;
        si = &SOC_INFO(unit);
        blk = si->pmq_block[2];
        pmq_port = blk | SOC_REG_ADDR_BLOCK_ID_MASK;
    } else {
        return SOC_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pmq_port, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, IDDQf, 0);
    /* Deassert power down */
    soc_reg_field_set(unit, reg, &rval, PWRDWNf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pmq_port, 0, rval));
    sal_usleep(sleep_usec);

    /* Reset SGMII */
    soc_reg_field_set(unit, reg, &rval, RSTB_HWf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pmq_port, 0, rval));
    sal_usleep(sleep_usec + 10000);

    /* Bring SGMII out of reset */
    soc_reg_field_set(unit, reg, &rval, RSTB_HWf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pmq_port, 0, rval));
    sal_usleep(sleep_usec);

    /* Activate MDIO on SGMII */
    soc_reg_field_set(unit, reg, &rval, RSTB_MDIOREGSf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pmq_port, 0, rval));
    sal_usleep(sleep_usec);

    /* Activate clocks */
    soc_reg_field_set(unit, reg, &rval, RSTB_PLLf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pmq_port, 0, rval));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_wolfhound2_port_reset
 * Purpose:
 *      Special reset sequencing for BCM53540
 */
int
soc_wolfhound2_port_reset(int unit)
{
    int         blk;

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_PMQ) {
        if (SOC_BLOCK_NUMBER(unit, blk) == 2) {
            /* reset SGMII_4P1 */
            SOC_IF_ERROR_RETURN(soc_wh2_sgmii_init(unit, 1));
        } else {
            SOC_IF_ERROR_RETURN(
                soc_wh2_gphy_pmq_init(unit, SOC_BLOCK_NUMBER(unit, blk)));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_wolfhound2_chip_reset
 * Purpose:
 *      Special reset sequencing for BCM53540
 */
int
soc_wolfhound2_chip_reset(int unit)
{
    uint32          to_usec, rval;
    int             option;
    soc_info_t      *si;
    uint32          strap_sts_1, value, qgphy_core_map;
    wh2_sku_info_t  *matched_port_config = NULL;
    uint8           qgphy5_lane, sgmii_4p0_lane;

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x00000000); /* block 7  - 0 */
    WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x00430000); /* block 15 - 8 */
    WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x00005064); /* block 23 - 16 */
    WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x00000000); /* block 31 - 24 */
    WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x77772222); /* block 39 - 32 */
    WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x00000111); /* block 40 - 47 */

    WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x7d0);

    sal_usleep(to_usec);


    /* Change core clock if required */
    si = &SOC_INFO(unit);
    /* set core clock to 62.5MHz */
    if (si->frequency == 62) {
        rval = 0x28;
    /* set core clock to 41.67MHz */
    } else if (si->frequency == 41) {
        rval = 0x3c;
   /* set core clock to 104.17MHz */
    } else {
        rval = 0x18;
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_CORE_PLL_CTRL4r,
                                 REG_PORT_ANY, MSTR_CH0_MDIVf, rval));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                CMIC_TO_CORE_PLL_LOADf, 1));

    /* Check option and strap pin status to do the proper initialization */
    matched_port_config = &wh2_sku_port_config[matched_devid_idx[unit]];
    option = matched_port_config->config_option;
    qgphy_core_map = matched_port_config->qgphy_core_map;
    qgphy5_lane = matched_port_config->qgphy5_lane;
    sgmii_4p0_lane = matched_port_config->sgmii_4p0_lane;
#define WH2_STRAP_GPHY_SGMII_SEL_0  (1 << 0)
#define WH2_STRAP_GPHY_SGMII_SEL_1  (1 << 1)
    strap_sts_1 = 0;
    option = option % 6;
    SOC_IF_ERROR_RETURN(READ_TOP_STRAP_STATUS_1r(unit, &strap_sts_1));
    /* check strap v.s. option */
    value = (strap_sts_1 >> 1) & 0x3;
    if (!SAL_BOOT_SIMULATION) {
        switch (option) {
            case 0:
            case 2:
                if (value != 3) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "Invalid option "
                                          "(unmatch with strap pin)")));
                    return SOC_E_INTERNAL;
                }
                break;
            case 3:
            case 4:
                if (value != 1) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "Invalid option "
                                          "(unmatch with strap pin)")));
                    return SOC_E_INTERNAL;
                }
                break;
            case 1:
            case 5:
                if (value != 0) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "Invalid option "
                                          "(unmatch with strap pin)")));
                    return SOC_E_INTERNAL;
                }
                break;
        }

        if (value & WH2_STRAP_GPHY_SGMII_SEL_0) {
            /* front ports select GPHY */
            if (!(qgphy_core_map & 0x20) || !(qgphy5_lane & 0x3)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Invalid option "
                                      "(unmatch with strap pin)")));
                return SOC_E_INTERNAL;
            }
        }
        if (value & WH2_STRAP_GPHY_SGMII_SEL_1) {
            /* front ports select GPHY, OOB port select SGMII */
            if (!(qgphy_core_map & 0x20) || !(qgphy5_lane & 0xC)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Invalid option "
                                      "(unmatch with strap pin)")));
                return SOC_E_INTERNAL;
            }
        }
    }

    /* re-ordering the port sequence if required*/

    /* Bring port block out of reset */
    /* GPHY or SGMII configuration need to be considered here */
    if (sgmii_4p0_lane) {
        SOC_IF_ERROR_RETURN(soc_wh2_sgmii_init(unit, 0));
    }
    SOC_IF_ERROR_RETURN(
        soc_wh2_qgphy_init(unit, qgphy_core_map, qgphy5_lane));

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_GXP0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_GXP1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_GXP2_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    /* Bring network sync out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TS_PLL_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    return SOC_E_NONE;
}

#define WH2_MMU_IPMC_GROUP_MAX     30
static soc_mem_t ipmc_mems[WH2_MMU_IPMC_GROUP_MAX] = {
        INVALIDm,  INVALIDm,
        MMU_IPMC_GROUP_TBL2m,  MMU_IPMC_GROUP_TBL3m,
        MMU_IPMC_GROUP_TBL4m,  MMU_IPMC_GROUP_TBL5m,
        MMU_IPMC_GROUP_TBL6m,  MMU_IPMC_GROUP_TBL7m,
        MMU_IPMC_GROUP_TBL8m,  MMU_IPMC_GROUP_TBL9m,
        MMU_IPMC_GROUP_TBL10m,  MMU_IPMC_GROUP_TBL11m,
        MMU_IPMC_GROUP_TBL12m,  MMU_IPMC_GROUP_TBL13m,
        MMU_IPMC_GROUP_TBL14m,  MMU_IPMC_GROUP_TBL15m,
        MMU_IPMC_GROUP_TBL16m,  MMU_IPMC_GROUP_TBL17m,
        MMU_IPMC_GROUP_TBL18m,  MMU_IPMC_GROUP_TBL19m,
        MMU_IPMC_GROUP_TBL20m,  MMU_IPMC_GROUP_TBL21m,
        MMU_IPMC_GROUP_TBL22m,  MMU_IPMC_GROUP_TBL23m,
        MMU_IPMC_GROUP_TBL24m,  MMU_IPMC_GROUP_TBL25m,
        MMU_IPMC_GROUP_TBL26m,  MMU_IPMC_GROUP_TBL27m,
        MMU_IPMC_GROUP_TBL28m,  MMU_IPMC_GROUP_TBL29m
};

static int
soc_wolfhound2_pipe_mem_clear(int unit)
{
    uint32              rval;
    int                 pipe_init_usec, index;
    soc_timeout_t       to;
    static const struct {
        soc_mem_t mem;
        uint32 skip_flags; /* always skip on QUICKTURN or XGSSIM */
    } cam_list[] = {
        { CPU_COS_MAPm,                     BOOT_F_PLISIM },
        { EFP_TCAMm,                        BOOT_F_PLISIM },
        { FP_GLOBAL_MASK_TCAMm,             BOOT_F_PLISIM },
        { FP_TCAMm,                         BOOT_F_PLISIM },
        { L2_USER_ENTRYm,                   BOOT_F_PLISIM },
        { L2_USER_ENTRY_ONLYm,              BOOT_F_PLISIM },
        { L3_DEFIPm,                        BOOT_F_PLISIM },
        { L3_DEFIP_ONLYm,                   BOOT_F_PLISIM },
        { VFP_TCAMm,                        BOOT_F_PLISIM },
        { VLAN_SUBNETm,                     0 }, /* VLAN API needs all 0 mask */
        { VLAN_SUBNET_ONLYm,                0 }
    };

    /*
     * Reset the IPIPE and EPIPE block
     */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    /* Set count to # entries in largest IPIPE table, L2_ENTRYm */
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, 0x4000);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    /* Set count to # entries in largest EPIPE table, EGR_GPP_ATTRIBUTESm */
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, 0x2000);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
        if (soc_reg_field_get(unit, ING_HW_RESET_CONTROL_2r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit, "unit %d : ING_HW_RESET timeout\n"),
                     unit));
            break;
        }
    } while (TRUE);

    /* Wait for EPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
        if (soc_reg_field_get(unit, EGR_HW_RESET_CONTROL_1r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit, "unit %d : EGR_HW_RESET timeout\n"),
                     unit));
            break;
        }
    } while (TRUE);

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    if (!SAL_BOOT_SIMULATION || SAL_BOOT_BCMSIM) {
        /* TCAM tables are not handled by hardware reset control */
        for (index = 0; index < sizeof(cam_list) / sizeof(cam_list[0]);
             index++) {
            if (sal_boot_flags_get() & cam_list[index].skip_flags) {
                continue;
            }
            if (((cam_list[index].mem == EFP_TCAMm) ||
                (cam_list[index].mem == VFP_TCAMm)) &&
                (!soc_feature(unit, soc_feature_field_multi_stage))) {
                continue;
            }
            SOC_IF_ERROR_RETURN(
                soc_mem_clear(unit, cam_list[index].mem, COPYNO_ALL, TRUE));
        }

        /* MMU_IPMC_VLAN_TBL */
        SOC_IF_ERROR_RETURN(
            soc_mem_clear(unit, MMU_IPMC_VLAN_TBLm, COPYNO_ALL, TRUE));

        /* MMU_IPMC_GROUP_TBLn : n is 2 ~ 31 in HR3 */
        for (index = 0; index < WH2_MMU_IPMC_GROUP_MAX; index++) {
            if (ipmc_mems[index] != INVALIDm) {
                SOC_IF_ERROR_RETURN(
                    soc_mem_clear(unit, ipmc_mems[index], COPYNO_ALL, TRUE));
            }
        }
    }

    return SOC_E_NONE;
}


STATIC int
soc_wolfhound2_init_port_mapping(int unit)
{
    soc_info_t *si;
    soc_mem_t mem;
    uint32 rval;
    ing_physical_to_logical_port_number_mapping_table_entry_t entry;
    int port, phy_port;
    int num_port, num_phy_port;

    si = &SOC_INFO(unit);

    /* Ingress physical to logical port mapping */
    mem = ING_PHYSICAL_TO_LOGICAL_PORT_NUMBER_MAPPING_TABLEm;
    num_phy_port = soc_mem_index_count(unit, mem);
    sal_memset(&entry, 0, sizeof(entry));
    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        soc_mem_field32_set(unit, mem, &entry, LOGICAL_PORT_NUMBERf,
                            port == -1 ? 0x3F : port);
        SOC_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port, &entry));
    }
    num_port = soc_mem_index_count(unit, PORT_TABm);

    /* Egress logical to physical port mapping */
    for (port = 0; port < num_port; port++) {
        phy_port = si->port_l2p_mapping[port];
        rval = 0;
        soc_reg_field_set(unit, EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr,
                          &rval, PHYSICAL_PORT_NUMBERf,
                          phy_port == -1 ? 0x3F : phy_port);
        SOC_IF_ERROR_RETURN(
            WRITE_EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr(unit, port,
                                                               rval));
    }
    /* MMU logical to physical port mapping */
    /*(Here, Same as Egress logical to physical port mapping) */
    for (port = 0; port < num_port; port++) {
        phy_port = si->port_m2p_mapping[port];
        if (phy_port != -1) {
            rval = 0;
            soc_reg_field_set(unit, LOG_TO_PHY_PORT_MAPPINGr, &rval,
                              PHY_PORTf, phy_port);
            SOC_IF_ERROR_RETURN(
                WRITE_LOG_TO_PHY_PORT_MAPPINGr(unit, port, rval));
        }
    }

    return SOC_E_NONE;
}

STATIC int
soc_wolfhound2_pgw_ge_tdm_config(int unit)
{
    int blk, instance, phy_port, port, bindex;
    uint32 rval;
    int ge0, ge1;
    soc_driver_t *d = SOC_DRIVER(unit);
    int num_blktype, i;
    soc_info_t *si = &SOC_INFO(unit);

    /* The value of the PGW_GE TDM mode depends on the active port numbers */
    num_blktype = d->port_num_blktype;
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_PGW_GE) {
        /* iter pgw_ge0 ~ pgw_ge2 */
        instance = SOC_BLOCK_PORT(unit, blk);
        ge0 = ge1 = 0;
        SOC_PBMP_ITER(si->block_bitmap[blk], port) {
            /* iter the valid port in pgw_gex */
            phy_port = si->port_l2p_mapping[port];
            for (i = 0; i < num_blktype; i++) {
                if (d->port_info[phy_port * num_blktype + i].blk == blk) {
                    /* get index of pgw_gex */
                    bindex = d->port_info[phy_port * num_blktype + i].bindex;
                    if (bindex < 8) {
                        /* index 0-7 controlled by PGW_GE0_MODE_REG */
                        ge0++;
                    } else {
                        /* index 8-15 controlled by PGW_GE1_MODE_REG */
                        ge1++;
                    }
                }
            }
        }
        SOC_IF_ERROR_RETURN(READ_PGW_GE0_MODE_REGr(unit, instance, &rval));
        soc_reg_field_set(unit, PGW_GE0_MODE_REGr, &rval, GP0_TDM_MODEf, ge0);
        SOC_IF_ERROR_RETURN(WRITE_PGW_GE0_MODE_REGr(unit, instance, rval));
        SOC_IF_ERROR_RETURN(READ_PGW_GE1_MODE_REGr(unit, instance, &rval));
        soc_reg_field_set(unit, PGW_GE1_MODE_REGr, &rval, GP0_TDM_MODEf, ge1);
        SOC_IF_ERROR_RETURN(WRITE_PGW_GE1_MODE_REGr(unit, instance, rval));
   }
   return SOC_E_NONE;
}

#define wh2_get_pval(_map_, _idx_, _max_num_) \
    ((_idx_ < _max_num_) ? _map_[_idx_]: 0)

#define WH2_PGW_COUNT_GE12P       (2)
#define WH2_PORTS_PER_PGW_GE12P   (12)
#define WH2_FIRST_PORT_PGW_GE12P0_0 (2)
#define WH2_FIRST_PORT_PGW_GE12P1_0 (18)

#define WH2_PGW_COUNT_GE4P       (1)
#define WH2_PORTS_PER_PGW_GE4P   (4)
#define WH2_FIRST_PORT_PGW_GE4P0 (34)

STATIC int
soc_wolfhound2_ledup_init(int unit)
{
    int i, j, pval0, pval1, pval2, pval3, port_idx;
    uint32 led_remap[64] = {0}, port_cnt = 0, rval = 0;

    struct led_remap_t {
       uint32 reg_addr;
       uint32 port0;
       uint32 port1;
       uint32 port2;
       uint32 port3;
    } ledup0_remap_r[] = {
        {CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r,
         REMAP_PORT_0f, REMAP_PORT_1f, REMAP_PORT_2f, REMAP_PORT_3f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r,
         REMAP_PORT_4f, REMAP_PORT_5f, REMAP_PORT_6f, REMAP_PORT_7f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r,
         REMAP_PORT_8f, REMAP_PORT_9f, REMAP_PORT_10f, REMAP_PORT_11f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r,
         REMAP_PORT_12f, REMAP_PORT_13f, REMAP_PORT_14f, REMAP_PORT_15f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r,
         REMAP_PORT_16f, REMAP_PORT_17f, REMAP_PORT_18f, REMAP_PORT_19f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r,
         REMAP_PORT_20f, REMAP_PORT_21f, REMAP_PORT_22f, REMAP_PORT_23f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r,
         REMAP_PORT_24f, REMAP_PORT_25f, REMAP_PORT_26f, REMAP_PORT_27f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r,
         REMAP_PORT_28f, REMAP_PORT_29f, REMAP_PORT_30f, REMAP_PORT_31f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r,
         REMAP_PORT_32f, REMAP_PORT_33f, REMAP_PORT_34f, REMAP_PORT_35f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r,
         REMAP_PORT_36f, REMAP_PORT_37f, REMAP_PORT_38f, REMAP_PORT_39f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r,
         REMAP_PORT_40f, REMAP_PORT_41f, REMAP_PORT_42f, REMAP_PORT_43f}
    };

    /* The LED scanning out order is
     *  PGW_GE4P(37->34) -> PGW_GE12P1(18->29) -> PGW_GE12P0(13->2)
     */

    /* PGW_GE4P(34->37) */
    for (i = WH2_PGW_COUNT_GE4P - 1; i >= 0; i--) {
        for (j = 0; j < WH2_PORTS_PER_PGW_GE4P; j++) {
            led_remap[port_cnt++] =
                WH2_FIRST_PORT_PGW_GE4P0 + WH2_PORTS_PER_PGW_GE4P * i + j;
        }
    }

    /* PGW_GE12P1 -> PGW_GE12P0 */
    for (i = WH2_PGW_COUNT_GE12P - 1; i >= 0; i--) {
        for (j = 0; j < WH2_PORTS_PER_PGW_GE12P; j++) {
            /* Not consecutive port mapping */
            if (i == 1) {
                led_remap[port_cnt++] =
                    WH2_FIRST_PORT_PGW_GE12P1_0 + j;
            } else {
                led_remap[port_cnt++] =
                    WH2_FIRST_PORT_PGW_GE12P0_0 + j;
            }
        }
    }

    /* Setup CMIC_LEDUP0_PORT_ORDER_REMAP registers */
    for (i = 0; i < sizeof(ledup0_remap_r) / sizeof(ledup0_remap_r[0]); i++) {
        port_idx = i * 4;
        pval0 = wh2_get_pval(led_remap, port_idx, port_cnt);
        pval1 = wh2_get_pval(led_remap, port_idx + 1, port_cnt);
        pval2 = wh2_get_pval(led_remap, port_idx + 2, port_cnt);
        pval3 = wh2_get_pval(led_remap, port_idx + 3, port_cnt);

        rval = 0;
        soc_reg_field_set(unit, ledup0_remap_r[i].reg_addr, &rval,
                          ledup0_remap_r[i].port0, pval0);
        soc_reg_field_set(unit, ledup0_remap_r[i].reg_addr, &rval,
                          ledup0_remap_r[i].port1, pval1);
        soc_reg_field_set(unit, ledup0_remap_r[i].reg_addr, &rval,
                          ledup0_remap_r[i].port2, pval2);
        soc_reg_field_set(unit, ledup0_remap_r[i].reg_addr, &rval,
                          ledup0_remap_r[i].port3, pval3);

        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN
            (soc_pci_write(unit,
                           soc_reg_addr(unit, ledup0_remap_r[i].reg_addr,
                           REG_PORT_ANY, 0),rval));
    }

    /* Initialize the LEDuP0 data ram */
    rval = 0;
    for (i = 0; i < 256; i++) {
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_DATA_RAMr(unit, i, rval));
    }

    return SOC_E_NONE;
}

STATIC int
soc_wolfhound2_misc_init(int unit)
{
    int         blk, parity_enable, port, l2_ovf_enable, l2_tbl_size;
    uint32      rval, fval;
    soc_field_t fields[3];
    uint32      values[3];
    uint32      entry[SOC_MAX_MEM_WORDS];
    uint64      reg64;
    int         freq, target_freq, divisor, dividend, delay;
    soc_info_t *si = &SOC_INFO(unit);

    /* Stop the mem scan task before all of the parity init takes place */
    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        /* Clear IPIPE/EPIPE Memories */
        SOC_IF_ERROR_RETURN(soc_wolfhound2_pipe_mem_clear(unit));
        /* PGW_GE TDM mode */
        SOC_IF_ERROR_RETURN(soc_wolfhound2_pgw_ge_tdm_config(unit));
    }
    SOC_IF_ERROR_RETURN(soc_wolfhound2_init_port_mapping(unit));

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    soc_hr3_ser_register(unit);
    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_hr3_tcam_ser_init(unit));
        /* Enabling the packet drop when parity error occurred */
        SOC_IF_ERROR_RETURN(READ_IPIPE_PERR_CONTROLr(unit, &rval));
        fval = 1;
        soc_reg_field_set(unit, IPIPE_PERR_CONTROLr, &rval, DROPf, fval);
        SOC_IF_ERROR_RETURN(WRITE_IPIPE_PERR_CONTROLr(unit, rval));
        SOC_IF_ERROR_RETURN(soc_hr3_ser_enable_all(unit, TRUE));
#if defined(SER_TR_TEST_SUPPORT)
        soc_hr3_ser_test_register(unit);
#endif /* SER_TR_TEST_SUPPORT */
    } else {
        SOC_IF_ERROR_RETURN(soc_hr3_ser_enable_all(unit, FALSE));
    }

    /* Enable L2 overflow bucket */
    l2_tbl_size = soc_property_get(unit, spn_L2_TABLE_SIZE, FALSE);
    if (l2_tbl_size == (soc_mem_index_count(unit, L2Xm) +
                        soc_mem_index_count(unit, L2_ENTRY_OVERFLOWm))) {
        /*
         * if l2 table size equals to l2 table size + l2 overflow table size,
         * it indicates the l2 overflow table is enabled.
         */
        l2_ovf_enable = 1;
    } else {
        l2_ovf_enable = 0;
    }
    SOC_IF_ERROR_RETURN(READ_L2_LEARN_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, L2_LEARN_CONTROLr, &rval,
                      OVERFLOW_BUCKET_ENABLEf, l2_ovf_enable);
    SOC_IF_ERROR_RETURN(WRITE_L2_LEARN_CONTROLr(unit, rval));
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->l2_overflow_bucket_enable = l2_ovf_enable;
    SOC_CONTROL_UNLOCK(unit);

    /* Enable L2 overflow interrupt */
    if (soc_property_get(unit, spn_L2_OVERFLOW_EVENT, FALSE)) {
        SOC_CONTROL_LOCK(unit);
        SOC_CONTROL(unit)->l2_overflow_enable = TRUE;
        SOC_CONTROL_UNLOCK(unit);
        SOC_IF_ERROR_RETURN(soc_hr3_l2_overflow_start(unit));
    } else {
        SOC_CONTROL_LOCK(unit);
        SOC_CONTROL(unit)->l2_overflow_enable = FALSE;
        SOC_CONTROL_UNLOCK(unit);
        SOC_IF_ERROR_RETURN(soc_hr3_l2_overflow_stop(unit));
    }

    /* Port related init */
    /* Enable GPORT */
    rval = 0;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 1);
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, GPORT_ENf, 1);
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_GPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_GPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }

    /* MMU related init */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, METERING_CLK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    /* Enable dual hash on L2 and L3 tables */
    fields[0] = ENABLEf;
    values[0] = 1;
    fields[1] = HASH_SELECTf;
    values[1] = FB_HASH_CRC32_LOWER;
    fields[2] = INSERT_LEAST_FULL_HALFf;
    values[2] = 1;
    SOC_IF_ERROR_RETURN(
        soc_reg_fields32_modify(unit, L2_AUX_HASH_CONTROLr, REG_PORT_ANY, 3,
                                fields, values));
    SOC_IF_ERROR_RETURN(
        soc_reg_fields32_modify(unit, L3_AUX_HASH_CONTROLr, REG_PORT_ANY, 3,
                                fields, values));

    /*
     * Egress Enable
     */
    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(
            WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL,
                              SOC_INFO(unit).port_l2p_mapping[port], entry));
    }

    COMPILER_64_ZERO(reg64);
    soc_reg64_field32_set(unit, EPC_LINK_BMAP_64r, &reg64, PORT_BITMAP_LOf,
                          SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAP_64r(unit, reg64));

    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &reg64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L3SRC_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L2DST_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L2f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L3f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_RARP_TO_FPf, 0x3); /* enable both ARP & RARP */
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_VALIDATION_ENf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          IGNORE_HG_HDR_LAG_FAILOVERf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, reg64));

    SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
    soc_reg_field_set(unit, EGR_CONFIG_1r, &rval, RING_MODEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIG_1r(unit, rval));

    /*
     * The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    rval = 0;
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, VT_MISS_UNTAGf, 0);

    /* Enable pri/cfi remarking on egress ports. */
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, REMARK_OUTER_DOT1Pf,
                      1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, rval));
    }

    /* Multicast range initialization */
    SOC_IF_ERROR_RETURN(
        soc_hbx_higig2_mcast_sizes_set(
            unit,
            soc_property_get(unit, spn_HIGIG2_MULTICAST_VLAN_RANGE,
                             SOC_HBX_MULTICAST_RANGE_DEFAULT),
            soc_property_get(unit, spn_HIGIG2_MULTICAST_L2_RANGE,
                             SOC_HBX_MULTICAST_RANGE_DEFAULT),
            soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                             SOC_HBX_MULTICAST_RANGE_DEFAULT)));

    /* Setup SW2_FP_DST_ACTION_CONTROL */
    fields[0] = HGTRUNK_RES_ENf;
    fields[1] = LAG_RES_ENf;
    values[0] = values[1] = 1;
    SOC_IF_ERROR_RETURN(
        soc_reg_fields32_modify(unit, SW2_FP_DST_ACTION_CONTROLr,
                                REG_PORT_ANY, 2, fields, values));

    /* Directed Mirroring ON by default except for CPU port */
    /*
     * The src_port info will be changed as egress port
     * if EM_SRCMOD_CHANGEf = 1
     */
    PBMP_PORT_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(READ_EGR_PORT_64r(unit, port, &reg64));
        soc_reg64_field32_set(unit, EGR_PORT_64r, &reg64, EM_SRCMOD_CHANGEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_PORT_64r(unit, port, reg64));
        SOC_IF_ERROR_RETURN(READ_IEGR_PORT_64r(unit, port, &reg64));
        soc_reg64_field32_set(unit, IEGR_PORT_64r, &reg64,
                              EM_SRCMOD_CHANGEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IEGR_PORT_64r(unit, port, reg64));
    }

    SOC_IF_ERROR_RETURN(_soc_hr3_l3iif_hw_mem_init(unit, TRUE));


    freq = si->frequency;

    /*
     * Set external MDIO freq to around 10MHz
     * target_freq = core_clock_freq * DIVIDEND / DIVISOR / 2
     */
    if (!SOC_WARM_BOOT(unit)) {
        target_freq = 10;
        divisor = (freq + (target_freq * 2 - 1)) / (target_freq * 2);
        divisor = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVISOR, divisor);
        dividend = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVIDEND, 1);

        rval = 0;
        soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVISORf, divisor);
        soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVIDENDf, dividend);
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUSTr(unit, rval));
    }

    /* CMIC related */
    freq = si->frequency;
    /*
     * Set internal MDIO freq to around 10MHz
     * Valid range is from 2.5MHz to 12.5MHz
     * target_freq = core_clock_freq * DIVIDEND / DIVISOR / 2
     * or
     * DIVISOR = core_clock_freq * DIVIDENT / (target_freq * 2)
     */
    target_freq = 10;
    divisor = (freq + (target_freq * 2 - 1)) / (target_freq * 2);
    divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR, divisor);
    dividend = soc_property_get(unit, spn_RATE_INT_MDIO_DIVIDEND, 1);
    rval = 0;
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVISORf,
                       divisor);
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVIDENDf,
                       dividend);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval));

    delay = soc_property_get(unit, spn_MDIO_OUTPUT_DELAY, -1);
    if (delay >= 1  && delay <= 15) {
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_MIIM_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_MIIM_CONFIGr, &rval, MDIO_OUT_DELAYf,
                          delay);
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_MIIM_CONFIGr(unit, rval));
    }

    /* LEDuP init */
    SOC_IF_ERROR_RETURN(soc_wolfhound2_ledup_init(unit));

    /* MSPI init */
    if (soc_mspi_init(unit) != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit, "MSPI Init Failed\n")));
    }

    /* I2C set to Master Mode through Override Strap */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_OVERRIDE_STRAPr(unit, &rval));
    soc_reg_field_set(unit, CMIC_OVERRIDE_STRAPr, &rval,
                      ENABLE_OVERRIDE_I2C_MASTER_SLAVE_MODEf, 1);
    soc_reg_field_set(unit, CMIC_OVERRIDE_STRAPr, &rval,
                      I2C_MASTER_SLAVE_MODEf, 1);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_OVERRIDE_STRAPr(unit, rval));

    return SOC_E_NONE;
}

STATIC int
soc_wolfhound2_tdm_init(int unit)
{
    int                         tdm_size;
    uint32                      rval;
    iarb_tdm_table_entry_t      iarb_tdm;
    mmu_arb_tdm_table_entry_t   mmu_arb_tdm;
    int                         i, port, phy_port;
    wh2_sku_info_t              *matched_sku_info;
    const int                   *arr = NULL;

    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 1);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf, 83);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    if (matched_devid_idx[unit] == -1) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit, "Warning: soc_wolfhound2_port_config_init "
                             "should be invoked first! "
                             "Choose bcm53540 port config.\n")));
        matched_devid_idx[unit] = 0;
    }
    matched_sku_info = &wh2_sku_port_config[matched_devid_idx[unit]];

    arr = matched_sku_info->tdm_table;
    tdm_size = matched_sku_info->tdm_table_size;

    if (arr == NULL) {
        return SOC_E_CONFIG;
    }

    for (i = 0; i < tdm_size; i++) {
        phy_port = arr[i];
        port = (phy_port != 63) ? matched_sku_info->p2l_mapping[phy_port] : 63;
        sal_memset(&iarb_tdm, 0, sizeof(iarb_tdm_table_entry_t));
        sal_memset(&mmu_arb_tdm, 0, sizeof(mmu_arb_tdm_table_entry_t));

        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm, PORT_NUMf,
                                        phy_port);

        soc_MMU_ARB_TDM_TABLEm_field32_set(unit, &mmu_arb_tdm, PORT_NUMf,
                                           (port != -1) ? port : 63);
        if (i == tdm_size - 1) {
            soc_MMU_ARB_TDM_TABLEm_field32_set(unit, &mmu_arb_tdm, WRAP_ENf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i,
                                                  &iarb_tdm));
        SOC_IF_ERROR_RETURN(WRITE_MMU_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i,
                                                     &mmu_arb_tdm));
    }
    rval = 0;
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf,
                      tdm_size -1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    return SOC_E_NONE;
}

#define CELL_SIZE 128

/* Maximum device supported number of COSQ. */
#define WH2_QUEUE_MAX_NUM     8
/* Default configured number of COSQ. */
#define WH2_QUEUE_DEFAULT_NUM 4

typedef struct soc_wh2_mmu_params_s {
    /*_traffic_attributes_*/
    uint32 min_packet_size_traffic_attr;              /* c7 */
    uint32 ethernet_mtu_traffic_attr;                 /* c8 */
    uint32 max_packet_size_traffic_attr;              /* c9 */
    uint32 standard_jumbo_frame_traffic_attr;         /* c10 */
    uint32 lossless_traffic_mtu_size_traffic_attr;    /* c11 */

    /*_sw_config_property_*/
    uint32 total_physical_memory_sw_cfg;              /* c14 */
    uint32 cfapfullsetpoint_sw_cfg;                   /* c15 */
    uint32 total_cell_memory_for_admission_sw_cfg;    /* c16 */
    uint32 num_1g_ports_uplink_ports_sw_cfg;          /* c17 configurable */
    uint32 num_2p5g_ports_uplink_ports_sw_cfg;        /* c18 configurable */
    /* for lossy or lossles(pause) mode */
    uint32 num_1g_ports_downlink_ports_sw_cfg;        /* c19 configurable */
    uint32 num_2p5g_ports_downlink_ports_sw_cfg;      /* c20 configurable */
    /* for lossy+lossless(pfc) mode */
    uint32 num_lossy_downlink_ports_sw_cfg;           /* c19 configurable */
    uint32 num_lossless_downlink_ports_sw_cfg;        /* c20 configurable */

    uint32 number_of_uplink_ports_sw_cfg;             /* c21 */
    uint32 number_of_downlink_ports_sw_cfg;           /* c22 */
    uint32 flow_control_enabled_sw_cfg;               /* c23 lossy, lossless */
    uint32 flow_control_type_sw_cfg;                  /* c24 pause=0, pfc=1 */
    uint32 queue_port_limit_ratio_sw_cfg;             /* c25 */

    /*_sw_config_parameter_*/
    uint32 egress_queue_min_reserve_uplink_ports_lossy;            /* c29 */
    uint32 egress_queue_min_reserve_downlink_ports_lossy;          /* c30 */
    uint32 egress_queue_min_reserve_uplink_ports_lossless;         /* c31 */
    uint32 egress_queue_min_reserve_downlink_ports_lossless;       /* c32 */
    uint32 egress_queue_min_reserve_cpu_ports;                     /* c33 */
    uint32 egress_xq_min_reserve_lossy_uplink_and_downlink_ports;  /* c34 */
    uint32 egress_xq_min_reserve_lossless_uplink_ports;            /* c35 */
    uint32 egress_xq_min_reserve_lossless_downlink_ports;          /* c36 */
    uint32 num_total_pri_groups;                                   /* c37 */
    uint32 num_active_pri_group_lossless;                          /* c38 */
    uint32 num_lossless_queues;                                    /* c39 */
    uint32 num_lossy_queues;                                       /* c40 */
    uint32 mmu_xoff_pkt_threshold_uplink_ports;                    /* c41 */
    uint32 mmu_xoff_pkt_threshold_downlink_ports;                  /* c42 */
    uint32 mmu_xoff_cell_threshold_1g_port_uplink_ports;           /* c43 */
    uint32 mmu_xoff_cell_threshold_2p5g_port_uplink_ports;         /* c44 */

    /* for lossy or lossles(pause) mode */
    uint32 mmu_xoff_cell_threshold_1g_downlink_ports;              /* c45 */
    uint32 mmu_xoff_cell_threshold_2p5g_downlink_ports;            /* c46 */
    /* for lossy+lossless(pfc) mode */
    uint32 mmu_xoff_cell_threshold_downlink_lossy_ports;           /* c45 */
    uint32 mmu_xoff_cell_threshold_downlink_lossless_ports;        /* c46 */

    /*_device_attributes_*/
    uint32 cell_size;                                              /* c49 */
    uint32 num_egress_queues_per_port;                             /* c50 */
    uint32 num_cpu_queues;                                         /* c51 */
    uint32 num_1g_ports_uplink_ports;                              /* c52 */
    uint32 num_2p5g_ports_uplink_ports;                            /* c53 */

    /* for lossy or lossles(pause) mode */
    uint32 num_1g_ports_downlink_ports;                            /* c54 */
    uint32 num_2p5g_ports_downlink_ports;                          /* c55 */
    /* for lossy+lossless(pfc) mode */
    uint32 num_lossy_downlink_ports;                               /* c54 */
    uint32 num_lossless_downlink_ports;                            /* c55 */

    uint32 num_cpu_ports;                                          /* c56 */
    uint32 number_of_uplink_ports;                                 /* c57 */
    uint32 number_of_downlink_ports;                               /* c58 */
    uint32 total_physical_memory;                                  /* c59 */
    uint32 total_cell_memory;                                      /* c60 */
    uint32 total_advertised_cell_memory;                           /* c61 */
    uint32 numxq_per_uplink_ports;                                 /* c62 */
    uint32 numxq_per_downlink_ports_and_cpu_port;                  /* c63 */
    uint32 the_number_of_pipelines_per_device;                     /* c64 */

    uint32 n_egress_queues_active_per_xpe;                         /* c66 */
    uint32 n_ingress_ports_active_per_xpe;                         /* c67 */

    /*_reservations_*/
    uint32 wrr_reserve;                                            /* c70 */
    uint32 reserved_for_cfap;                                      /* c71 */
    uint32 skidmarker;                                             /* c72 */
    uint32 skidmarker_option;                                      /* c73 */
    uint32 prefetch;                                               /* c74 */

    /*_intermediate_results_*/
    /*_general_information_*/
    uint32 min_packet_size;                                        /* c79 */
    uint32 ethernet_mtu;                                           /* c80 */
    uint32 max_packet_size;                                        /* c81 */
    uint32 standard_jumbo_frame;                                   /* c82 */
    uint32 lossless_traffic_mtu_size;                              /* c83 */
    /*_ingress_thresholds_*/
    /* for lossy or lossles(pause) mode */
    uint32 headroom_for_1g_port;                                   /* c85 */
    uint32 headroom_for_2p5g_port;                                 /* c86 */
    /* for lossy+lossless(pfc) mode */
    /* Skipped c85 in this mode */
    uint32 headroom_for_lossless_downlink_port;                    /* c86 */

    /* for lossy or lossles(pause) mode */
    uint32 xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;   /* c87 */
    uint32 xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports; /* c88 */
    /* for lossy+lossless(pfc) mode */
    uint32 xoff_cell_thresholds_per_port_pg_downlink_lossy_ports;     /* c87 */
    uint32 xoff_cell_thresholds_per_port_pg_downlink_lossless_ports;  /* c88 */

    uint32 xoff_cell_threshold_1g_uplink_ports;                    /* c89 */
    uint32 xoff_cell_threshold_2p5g_uplink_ports;                  /* c90 */
    uint32 xoff_packet_thresholds_per_port_uplink_port;            /* c91 */
    uint32 xoff_packet_thresholds_per_port_downlink_port;          /* c92 */

    /* for lossy or lossles(pause) mode */
    uint32 discard_limit_per_port_pg_downlink_1g_port;             /* c93 */
    /* for lossy+lossless(pfc) mode */
    uint32 discard_limit_per_port_pg_lossy_downlink_port;          /* c93 */

    uint32 discard_limit_per_port_pg_downlink_2p5g_port;           /* c94 */
    uint32 discard_limit_per_port_pg_uplink_port;                  /* c95 */
    /*_egress_thresholds_*/
    uint32 total_reserved_cells_for_uplink_ports;                  /* c97 */
    uint32 total_reserved_cells_for_downlink_ports;                /* c98 */
    uint32 total_reserved_cells_for_cpu_port;                      /* c99 */
    uint32 total_reserved;                                         /* c100 */
    uint32 shared_space_cells;                                     /* c101 */
    uint32 reserved_xqs_per_uplink_port;                           /* c102 */
    uint32 shared_xqs_per_uplink_port;                             /* c103 */
    uint32 reserved_xqs_per_downlink_port;                         /* c104 */
    uint32 shared_xqs_per_downlink_port;                           /* c105 */
    uint32 reserved_xqs_at_cpu_port;                               /* c106 */
    uint32 shared_xqs_at_cpu_port;                                 /* c107 */
} soc_wh2_mmu_params_t;
soc_wh2_mmu_params_t mmu_params;

STATIC
uint32 _ceil_func(uint32 numerators , uint32 denominator)
{
    uint32  result;
    if (denominator == 0) {
        return 0xFFFFFFFF;
    }
    result = numerators / denominator;
    if (numerators % denominator != 0) {
        result++;
    }
    return result;
}

STATIC int
soc_wolfhound2_mmu_lossy_lossless_pfc_config_helper(int unit)
{
    uint32 val = 0;
    uint32 fld_val = 0;
    uint32 tmp = 0;
    int port; /* logical port id */
    int pport; /* physical port id */
    int speed;
    int cos;
    int i;
    int lossless_port = 0;
    uint32 ibppktsetlimit_pktsetlimit;
    uint32 ibppktsetlimit_resetlimitsel;
    uint32 mmu_fc_rx_en_mmu_fc_rx_enable;
    uint32 mmu_fc_tx_en_mmu_fc_tx_enable;
    uint32 pgcelllimit_0_2__cellsetlimit;
    uint32 pgcelllimit_3__cellsetlimit;
    uint32 pgcelllimit_4_7__cellsetlimit;
    uint32 pgcelllimit_0_2__cellresetlimit;
    uint32 pgcelllimit_3__cellresetlimit;
    uint32 pgcelllimit_4_7__cellresetlimit;
    uint32 pgdiscardsetlimit_0_2__discardsetlimit;
    uint32 pgdiscardsetlimit_3__discardsetlimit;
    uint32 pgdiscardsetlimit_4_7__discardsetlimit;
    uint32 holcosminxqcnt_0_2__holcosminxqcnt;
    uint32 holcosminxqcnt_3__holcosminxqcnt;
    uint32 holcosminxqcnt_4_7__holcosminxqcnt;
    uint32 holcospktsetlimit_0_2__pktsetlimit;
    uint32 holcospktsetlimit_3__pktsetlimit;
    uint32 holcospktsetlimit_4_7__pktsetlimit;
    uint32 holcospktresetlimit_0_2__pktresetlimit;
    uint32 holcospktresetlimit_3__pktresetlimit;
    uint32 holcospktresetlimit_4_7__pktresetlimit;
    uint32 cngcospktlimit0_0_7__cngpktsetlimit0;
    uint32 cngcospktlimit1_0_7__cngpktsetlimit1;
    uint32 cngportpktlimit0_cngportpktlimit0;
    uint32 cngportpktlimit1_cngportpktlimit1;
    uint32 dynxqcntport_dynxqcntport;
    uint32 dynresetlimport_dynresetlimport;
    uint32 lwmcoscellsetlimit_0_2__cellsetlimit;
    uint32 lwmcoscellsetlimit_3__cellsetlimit;
    uint32 lwmcoscellsetlimit_4_7__cellsetlimit;
    uint32 lwmcoscellsetlimit_0_2__cellresetlimit;
    uint32 lwmcoscellsetlimit_3__cellresetlimit;
    uint32 lwmcoscellsetlimit_4_7__cellresetlimit;
    uint32 holcoscellmaxlimit_0_2__cellmaxlimit;
    uint32 holcoscellmaxlimit_3__cellmaxlimit;
    uint32 holcoscellmaxlimit_4_7__cellmaxlimit;
    uint32 holcoscellmaxlimit_0_2__cellmaxresumelimit;
    uint32 holcoscellmaxlimit_3__cellmaxresumelimit;
    uint32 holcoscellmaxlimit_4_7__cellmaxresumelimit;
    uint32 dyncelllimit_dyncellsetlimit;
    uint32 dyncelllimit_dyncellresetlimit;
    uint32 color_drop_en_color_drop_en;
    uint32 shared_pool_ctrl_dynamic_cos_drop_en;
    uint32 shared_pool_ctrl_shared_pool_discard_en;
    uint32 shared_pool_ctrl_sharded_pool_xoff_en;

    soc_pbmp_t pbmp_lossless;

    SOC_PBMP_CLEAR(pbmp_lossless);
    pbmp_lossless = soc_property_get_pbmp_default(unit, spn_MMU_LOSSLESS_PBMP,
                                                  pbmp_lossless);

    SOC_IF_ERROR_RETURN(READ_CFAPFULLTHRESHOLDr(unit, &val));
    /* i6 = c15 */
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDr, &val,
                      CFAPFULLSETPOINTf,
                      mmu_params.cfapfullsetpoint_sw_cfg);
    /* i7 = i6 - c82 */
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDr, &val,
                      CFAPFULLRESETPOINTf,
                      mmu_params.cfapfullsetpoint_sw_cfg - \
                      mmu_params.standard_jumbo_frame);
    SOC_IF_ERROR_RETURN(WRITE_CFAPFULLTHRESHOLDr(unit, val));

    /* i10 = c16 */
    SOC_IF_ERROR_RETURN(READ_GBLLIMITSETLIMITr(unit, &val));
    soc_reg_field_set(unit, GBLLIMITSETLIMITr, &val,
                      GBLCELLSETLIMITf,
                      mmu_params.total_cell_memory_for_admission_sw_cfg);
    SOC_IF_ERROR_RETURN(WRITE_GBLLIMITSETLIMITr(unit, val));

    /* i11 = i10 */
    SOC_IF_ERROR_RETURN(READ_GBLLIMITRESETLIMITr(unit, &val));
    soc_reg_field_set(unit, GBLLIMITRESETLIMITr, &val,
                      GBLCELLRESETLIMITf,
                      mmu_params.total_cell_memory_for_admission_sw_cfg);
    SOC_IF_ERROR_RETURN(WRITE_GBLLIMITRESETLIMITr(unit, val));

    /* Device Wide - Egress Thresholds */
    val = 0;
    /* i15 = c101 */
    soc_reg_field_set(unit, TOTALDYNCELLSETLIMITr,
                      &val, TOTALDYNCELLSETLIMITf,
                      mmu_params.shared_space_cells);
    SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLSETLIMITr(unit, val));
    val = 0;
    /* i16 =i15-c82*2*/
    soc_reg_field_set(unit, TOTALDYNCELLRESETLIMITr,
                      &val, TOTALDYNCELLRESETLIMITf,
                      mmu_params.shared_space_cells - \
                      (mmu_params.standard_jumbo_frame * 2));
    SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLRESETLIMITr(unit, val));

    /* MICCONFIGr */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    soc_reg_field_set(unit, MISCCONFIGr, &val, MULTIPLE_ACCOUNTING_FIX_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &val, CNG_DROP_ENf, 0);
    soc_reg_field_set(unit, MISCCONFIGr, &val, DYN_XQ_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &val, HOL_CELL_SOP_DROP_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &val, DYNAMIC_MEMORY_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &val, SKIDMARKERf, 3);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));

    PBMP_ALL_ITER(unit, port) {
        pport = SOC_INFO(unit).port_l2p_mapping[port];
        speed = SOC_INFO(unit).port_speed_max[port];

        /* Settings based on port type */
        if (IS_CPU_PORT(unit, port)) {
            val = 0;
            if (mmu_params.flow_control_type_sw_cfg) {
                /* PFC */
                soc_reg_field_set(unit, PG_CTRL0r, &val, PPFC_PG_ENf, 0);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI0_GRPf, 0);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI1_GRPf, 1);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI2_GRPf, 2);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI3_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI4_GRPf, 4);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI5_GRPf, 5);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI6_GRPf, 6);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI7_GRPf, 7);
            } else {
                /* PAUSE */
                soc_reg_field_set(unit, PG_CTRL0r, &val, PPFC_PG_ENf, 0);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI0_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI1_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI2_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI3_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI4_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI5_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI6_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI7_GRPf, 3);
            }
            SOC_IF_ERROR_RETURN(WRITE_PG_CTRL0r(unit, port, val));
            val = 0;
            if (mmu_params.flow_control_type_sw_cfg) {
                /* PFC */
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI8_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI9_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI10_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI11_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI12_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI13_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI14_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI15_GRPf, 7);
            } else {
                /* PAUSE */
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI8_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI9_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI10_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI11_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI12_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI13_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI14_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI15_GRPf, 3);
            }
            SOC_IF_ERROR_RETURN(WRITE_PG_CTRL1r(unit, port, val));

            val = 0;
            soc_reg_field_set(unit, PG2TCr, &val, PG_BMPf, 0);
            for (i = 0; i < 8; i++) {
                SOC_IF_ERROR_RETURN(WRITE_PG2TCr(unit, port, i, val));
            }

            ibppktsetlimit_pktsetlimit = \
                mmu_params.\
                xoff_packet_thresholds_per_port_downlink_port; /* c92 */
            ibppktsetlimit_resetlimitsel = 3;
            mmu_fc_rx_en_mmu_fc_rx_enable = 0;
            mmu_fc_tx_en_mmu_fc_tx_enable = 0;
            pgcelllimit_0_2__cellsetlimit = \
                mmu_params.\
                xoff_cell_thresholds_per_port_pg_downlink_lossy_ports; /*c87*/
            pgcelllimit_3__cellsetlimit = \
                mmu_params.\
                xoff_cell_thresholds_per_port_pg_downlink_lossy_ports; /*c87*/
            pgcelllimit_4_7__cellsetlimit = 0;
            pgcelllimit_0_2__cellresetlimit = \
                mmu_params.\
                xoff_cell_thresholds_per_port_pg_downlink_lossy_ports; /*c87*/
            pgcelllimit_3__cellresetlimit = \
                mmu_params.\
                xoff_cell_thresholds_per_port_pg_downlink_lossy_ports; /*c87*/
            pgcelllimit_4_7__cellresetlimit = 0;
            pgdiscardsetlimit_0_2__discardsetlimit = \
                mmu_params.\
                discard_limit_per_port_pg_lossy_downlink_port; /* c93 */
            pgdiscardsetlimit_3__discardsetlimit = \
                mmu_params.\
                discard_limit_per_port_pg_lossy_downlink_port; /* c93 */
            pgdiscardsetlimit_4_7__discardsetlimit = 0;

            holcosminxqcnt_0_2__holcosminxqcnt = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            holcosminxqcnt_3__holcosminxqcnt = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            holcosminxqcnt_4_7__holcosminxqcnt = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            holcospktsetlimit_0_2__pktsetlimit = \
                mmu_params.shared_xqs_at_cpu_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_0_2__holcosminxqcnt; /* C107-C72-C74+Q70 */
            holcospktsetlimit_3__pktsetlimit = \
                mmu_params.shared_xqs_at_cpu_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_3__holcosminxqcnt; /* C107-C72-C74+Q71 */
            holcospktsetlimit_4_7__pktsetlimit = \
                mmu_params.shared_xqs_at_cpu_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_4_7__holcosminxqcnt; /* C107-C72-C74+Q72 */
            holcospktresetlimit_0_2__pktresetlimit = \
                holcospktsetlimit_0_2__pktsetlimit - 1; /* q73 - 1 */
            holcospktresetlimit_3__pktresetlimit = \
                holcospktsetlimit_3__pktsetlimit - 1; /* q74 - 1 */
            holcospktresetlimit_4_7__pktresetlimit = \
                holcospktsetlimit_4_7__pktsetlimit - 1; /* q75 - 1 */
            cngcospktlimit0_0_7__cngpktsetlimit0 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1; /*c63-1*/
            cngcospktlimit1_0_7__cngpktsetlimit1 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1; /*c63-1*/
            cngportpktlimit0_cngportpktlimit0 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1; /*c63-1*/
            cngportpktlimit1_cngportpktlimit1 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1; /*c63-1*/
            dynxqcntport_dynxqcntport = \
                mmu_params.shared_xqs_at_cpu_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch; /* c107-c72-c74 */
            dynresetlimport_dynresetlimport = \
                dynxqcntport_dynxqcntport - 2; /* q83 - 2 */
            lwmcoscellsetlimit_0_2__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_3__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_4_7__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_0_2__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_3__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_4_7__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            /* CEILING(c101*C25+Q85, 1) */
            tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
            holcoscellmaxlimit_0_2__cellmaxlimit = \
                tmp + lwmcoscellsetlimit_0_2__cellsetlimit;
            /* CEILING(c101*C25+Q86, 1) */
            tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
            holcoscellmaxlimit_3__cellmaxlimit = \
                tmp + lwmcoscellsetlimit_3__cellsetlimit;
            /* CEILING(c101*C25+Q87, 1) */
            tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
            holcoscellmaxlimit_4_7__cellmaxlimit = \
                tmp + lwmcoscellsetlimit_4_7__cellsetlimit;
            holcoscellmaxlimit_0_2__cellmaxresumelimit = \
                holcoscellmaxlimit_0_2__cellmaxlimit - \
                mmu_params.ethernet_mtu; /* q91 - c83 */
            holcoscellmaxlimit_3__cellmaxresumelimit = \
                holcoscellmaxlimit_3__cellmaxlimit - \
                mmu_params.ethernet_mtu; /* q92 - c83 */
            holcoscellmaxlimit_4_7__cellmaxresumelimit = \
                holcoscellmaxlimit_4_7__cellmaxlimit - \
                mmu_params.ethernet_mtu; /* q93 - c83 */
            dyncelllimit_dyncellsetlimit = \
                mmu_params.shared_space_cells; /* c101 */
            dyncelllimit_dyncellresetlimit = \
                dyncelllimit_dyncellsetlimit - \
                (mmu_params.ethernet_mtu * 2); /* q97-c83*2 */
            color_drop_en_color_drop_en = 0;
            shared_pool_ctrl_dynamic_cos_drop_en = 255;
            shared_pool_ctrl_shared_pool_discard_en = 255;
            shared_pool_ctrl_sharded_pool_xoff_en = 0;
        } else if (pport >= 34) { /* uplink port 1G, 2.5G*/
            if (SOC_PBMP_MEMBER(pbmp_lossless, port)) {
                /* no lossless config allowed on uplink ports */
                LOG_WARN(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "unit %d : MMU lossless configuration is "
                                    "not available on uplink port %d. "
                                    "No lossless setting will be applied "
                                    "to the port.\n"),
                                    unit, port));
            }

            val = 0;
            if (mmu_params.flow_control_type_sw_cfg) {
                /* PFC */
                soc_reg_field_set(unit, PG_CTRL0r, &val, PPFC_PG_ENf, 0);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI0_GRPf, 0);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI1_GRPf, 1);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI2_GRPf, 2);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI3_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI4_GRPf, 4);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI5_GRPf, 5);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI6_GRPf, 6);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI7_GRPf, 7);
            } else {
                /* PAUSE */
                soc_reg_field_set(unit, PG_CTRL0r, &val, PPFC_PG_ENf, 0);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI0_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI1_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI2_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI3_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI4_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI5_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI6_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI7_GRPf, 3);
            }
            SOC_IF_ERROR_RETURN(WRITE_PG_CTRL0r(unit, port, val));
            val = 0;
            if (mmu_params.flow_control_type_sw_cfg) {
                /* PFC */
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI8_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI9_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI10_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI11_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI12_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI13_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI14_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI15_GRPf, 7);
            } else {
                /* PAUSE */
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI8_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI9_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI10_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI11_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI12_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI13_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI14_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI15_GRPf, 3);
            }
            SOC_IF_ERROR_RETURN(WRITE_PG_CTRL1r(unit, port, val));
            val = 0;
            soc_reg_field_set(unit, PG2TCr, &val, PG_BMPf, 0);
            for (i = 0; i < 8; i++) {
                SOC_IF_ERROR_RETURN(WRITE_PG2TCr(unit, port, i, val));
            }

            if (speed == 10) { /* 1G */
                /* c89 */
                pgcelllimit_0_2__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_threshold_1g_uplink_ports;
                /* c89 */
                pgcelllimit_3__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_threshold_1g_uplink_ports;
                pgcelllimit_4_7__cellsetlimit = 0;
                /* c89 */
                pgcelllimit_0_2__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_threshold_1g_uplink_ports;
                /* c89 */
                pgcelllimit_3__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_threshold_1g_uplink_ports;
                pgcelllimit_4_7__cellresetlimit = 0;
            } else { /* 2.5 G */
                /* c90 */
                pgcelllimit_0_2__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_threshold_2p5g_uplink_ports;
                /* c90 */
                pgcelllimit_3__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_threshold_2p5g_uplink_ports;
                pgcelllimit_4_7__cellsetlimit = 0;
                /* c90 */
                pgcelllimit_0_2__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_threshold_2p5g_uplink_ports;
                /* c90 */
                pgcelllimit_3__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_threshold_2p5g_uplink_ports;
                pgcelllimit_4_7__cellresetlimit = 0;
            }
            /* c91 */
            ibppktsetlimit_pktsetlimit = \
                mmu_params.\
                xoff_packet_thresholds_per_port_uplink_port;
            ibppktsetlimit_resetlimitsel = 3;
            mmu_fc_rx_en_mmu_fc_rx_enable = 0;
            mmu_fc_tx_en_mmu_fc_tx_enable = 0;
            /* c95 */
            pgdiscardsetlimit_0_2__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_uplink_port;
            /* c95 */
            pgdiscardsetlimit_3__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_uplink_port;
            pgdiscardsetlimit_4_7__discardsetlimit = 0;

            /* c34 */
            holcosminxqcnt_0_2__holcosminxqcnt = \
                mmu_params.\
                egress_xq_min_reserve_lossy_uplink_and_downlink_ports;
            /* c35 */
            holcosminxqcnt_3__holcosminxqcnt = \
                mmu_params.\
                egress_xq_min_reserve_lossless_uplink_ports;
            holcosminxqcnt_4_7__holcosminxqcnt = 0;
            /* C103-C72-C74+C34 */
            holcospktsetlimit_0_2__pktsetlimit = \
                mmu_params.shared_xqs_per_uplink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_0_2__holcosminxqcnt;
            /* C103-C72-C74+c35 */
            holcospktsetlimit_3__pktsetlimit = \
                mmu_params.shared_xqs_per_uplink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_3__holcosminxqcnt;
            holcospktsetlimit_4_7__pktsetlimit = 0;
            holcospktresetlimit_0_2__pktresetlimit = \
            /* m73 - 1 */
                holcospktsetlimit_0_2__pktsetlimit - 1;
            /* m74 - 1 */
            holcospktresetlimit_3__pktresetlimit = \
                holcospktsetlimit_3__pktsetlimit - 1;
            holcospktresetlimit_4_7__pktresetlimit = 0;
            /* c62 - 1 */
            cngcospktlimit0_0_7__cngpktsetlimit0 = \
                mmu_params.numxq_per_uplink_ports - 1;
            /* c62 - 1 */
            cngcospktlimit1_0_7__cngpktsetlimit1 = \
                mmu_params.numxq_per_uplink_ports - 1;
            /* c62 - 1 */
            cngportpktlimit0_cngportpktlimit0 = \
                mmu_params.numxq_per_uplink_ports - 1;
            /* c62 - 1 */
            cngportpktlimit1_cngportpktlimit1 = \
                mmu_params.numxq_per_uplink_ports - 1;
            /* c103-c72-c74 */
            dynxqcntport_dynxqcntport = \
                mmu_params.shared_xqs_per_uplink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch;
            /* m83 - 2 */
            dynresetlimport_dynresetlimport = \
                dynxqcntport_dynxqcntport - 2;
            /* c29 */
            lwmcoscellsetlimit_0_2__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_uplink_ports_lossy;
            /* c31 */
            lwmcoscellsetlimit_3__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_uplink_ports_lossless;
            lwmcoscellsetlimit_4_7__cellsetlimit = 0;
            /* c29 */
            lwmcoscellsetlimit_0_2__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_uplink_ports_lossy;
            /* c31 */
            lwmcoscellsetlimit_3__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_uplink_ports_lossless;
            lwmcoscellsetlimit_4_7__cellresetlimit = 0;
            /* CEILING(c101*C25+o85, 1) */
            tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
            holcoscellmaxlimit_0_2__cellmaxlimit = \
                tmp + lwmcoscellsetlimit_0_2__cellsetlimit;
            /* CEILING(c101*C25+o85, 1) */
            tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
            holcoscellmaxlimit_3__cellmaxlimit = \
                tmp + lwmcoscellsetlimit_0_2__cellsetlimit;
            holcoscellmaxlimit_4_7__cellmaxlimit = 0;
            /* o91 - c80 */
            holcoscellmaxlimit_0_2__cellmaxresumelimit = \
                holcoscellmaxlimit_0_2__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            /* o91 - c80 */
            holcoscellmaxlimit_3__cellmaxresumelimit = \
                holcoscellmaxlimit_0_2__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            holcoscellmaxlimit_4_7__cellmaxresumelimit = 0;
            /* c101 */
            dyncelllimit_dyncellsetlimit = \
                mmu_params.shared_space_cells;
            /* o97-c80*2 */
            dyncelllimit_dyncellresetlimit = \
                dyncelllimit_dyncellsetlimit - \
                (mmu_params.ethernet_mtu * 2);
            color_drop_en_color_drop_en = 0;
            shared_pool_ctrl_dynamic_cos_drop_en = 255;
            shared_pool_ctrl_shared_pool_discard_en = 255;
            shared_pool_ctrl_sharded_pool_xoff_en = 0;
        } else { /* downlink port lossy, lossless */
            lossless_port = 0;
            if (SOC_PBMP_MEMBER(pbmp_lossless, port)) {
                lossless_port = 1;
            }

            val = 0;
            if (lossless_port) {
                soc_reg_field_set(unit, PG_CTRL0r, &val, PPFC_PG_ENf, 0x8);
            } else {
                soc_reg_field_set(unit, PG_CTRL0r, &val, PPFC_PG_ENf, 0);
            }
            if (mmu_params.flow_control_type_sw_cfg) {
                /* PFC */
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI0_GRPf, 0);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI1_GRPf, 1);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI2_GRPf, 2);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI3_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI4_GRPf, 4);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI5_GRPf, 5);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI6_GRPf, 6);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI7_GRPf, 7);
            } else {
                /* PAUSE */
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI0_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI1_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI2_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI3_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI4_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI5_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI6_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL0r, &val, PRI7_GRPf, 3);
            }
            SOC_IF_ERROR_RETURN(WRITE_PG_CTRL0r(unit, port, val));
            val = 0;
            if (mmu_params.flow_control_type_sw_cfg) {
                /* PFC */
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI8_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI9_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI10_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI11_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI12_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI13_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI14_GRPf, 7);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI15_GRPf, 7);
            } else {
                /* PAUSE */
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI8_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI9_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI10_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI11_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI12_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI13_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI14_GRPf, 3);
                soc_reg_field_set(unit, PG_CTRL1r, &val, PRI15_GRPf, 3);
            }
            SOC_IF_ERROR_RETURN(WRITE_PG_CTRL1r(unit, port, val));
            val = 0;
            soc_reg_field_set(unit, PG2TCr, &val, PG_BMPf, 0);
            for (i = 0; i < 8; i++) {
                SOC_IF_ERROR_RETURN(WRITE_PG2TCr(unit, port, i, val));
            }
            if (lossless_port) {
                /* PG2TC(3) */
                val = 0x8;
                SOC_IF_ERROR_RETURN(WRITE_PG2TCr(unit, port, 3, val));
            }

            if (lossless_port) { /* lossless ports */
                mmu_fc_rx_en_mmu_fc_rx_enable = 0x8;
                mmu_fc_tx_en_mmu_fc_tx_enable = 0x8;

                /* c87 */
                pgcelllimit_0_2__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_downlink_lossy_ports;
                /* c88 */
                pgcelllimit_3__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_downlink_lossless_ports;
                pgcelllimit_4_7__cellsetlimit = 0;
                /* c87 */
                pgcelllimit_0_2__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_downlink_lossy_ports;
                /* c88 */
                pgcelllimit_3__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_downlink_lossless_ports;
                pgcelllimit_4_7__cellresetlimit = 0;
                /* c93 */
                pgdiscardsetlimit_0_2__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_lossy_downlink_port;
                /* c94 */
                pgdiscardsetlimit_3__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                pgdiscardsetlimit_4_7__discardsetlimit = 0;
            } else { /* lossy ports */
                mmu_fc_rx_en_mmu_fc_rx_enable = 0;
                mmu_fc_tx_en_mmu_fc_tx_enable = 0;

                /* c87 */
                pgcelllimit_0_2__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_downlink_lossy_ports;
                /* c87 */
                pgcelllimit_3__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_downlink_lossy_ports;
                pgcelllimit_4_7__cellsetlimit = 0;
                /* c87 */
                pgcelllimit_0_2__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_downlink_lossy_ports;
                /* c87 */
                pgcelllimit_3__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_downlink_lossy_ports;
                pgcelllimit_4_7__cellresetlimit = 0;
                /* c93 */
                pgdiscardsetlimit_0_2__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_lossy_downlink_port;
                /* c93 */
                pgdiscardsetlimit_3__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_lossy_downlink_port;
                pgdiscardsetlimit_4_7__discardsetlimit = 0;
            }
            /* c92 */
            ibppktsetlimit_pktsetlimit = \
                mmu_params.\
                xoff_packet_thresholds_per_port_downlink_port;
            ibppktsetlimit_resetlimitsel = 3;

            /* c34 */
            holcosminxqcnt_0_2__holcosminxqcnt = \
                mmu_params.\
                egress_xq_min_reserve_lossy_uplink_and_downlink_ports;
            /* c36 */
            holcosminxqcnt_3__holcosminxqcnt = \
                mmu_params.\
                egress_xq_min_reserve_lossless_downlink_ports;
            holcosminxqcnt_4_7__holcosminxqcnt = 0;
            /* c105-C72-C74+c34 */
            holcospktsetlimit_0_2__pktsetlimit = \
                mmu_params.shared_xqs_per_downlink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_0_2__holcosminxqcnt;
            /* c105-C72-C74+c36 */
            holcospktsetlimit_3__pktsetlimit = \
                mmu_params.shared_xqs_per_downlink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_3__holcosminxqcnt;
            holcospktsetlimit_4_7__pktsetlimit = 0;
            holcospktresetlimit_0_2__pktresetlimit = \
                holcospktsetlimit_0_2__pktsetlimit - 1;
            holcospktresetlimit_3__pktresetlimit = \
                holcospktsetlimit_3__pktsetlimit - 1;
            holcospktresetlimit_4_7__pktresetlimit = 0;
            /* c63 - 1 */
            cngcospktlimit0_0_7__cngpktsetlimit0 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1;
            /* c63 - 1 */
            cngcospktlimit1_0_7__cngpktsetlimit1 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1;
            /* c63 - 1 */
            cngportpktlimit0_cngportpktlimit0 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1;
            /* c63 - 1 */
            cngportpktlimit1_cngportpktlimit1 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1;
            /* c105-c72-c74 */
            dynxqcntport_dynxqcntport = \
                mmu_params.shared_xqs_per_downlink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch;
            dynresetlimport_dynresetlimport = \
                dynxqcntport_dynxqcntport - 2;
            /* c30 */
            lwmcoscellsetlimit_0_2__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_downlink_ports_lossy;
            /* c32 */
            lwmcoscellsetlimit_3__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_downlink_ports_lossless;
            lwmcoscellsetlimit_4_7__cellsetlimit = 0;
            /* c30 */
            lwmcoscellsetlimit_0_2__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_downlink_ports_lossy;
            /* c32 */
            lwmcoscellsetlimit_3__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_downlink_ports_lossless;
            lwmcoscellsetlimit_4_7__cellresetlimit = 0;
            /* CEILING(c101*C25+o85, 1) */
            tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
            holcoscellmaxlimit_0_2__cellmaxlimit = \
                tmp + lwmcoscellsetlimit_0_2__cellsetlimit;
            /* CEILING(c101*C25+o85, 1) */
            tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
            holcoscellmaxlimit_3__cellmaxlimit = \
                tmp + lwmcoscellsetlimit_0_2__cellsetlimit;
            holcoscellmaxlimit_4_7__cellmaxlimit = 0;
            /* I91-C80 */
            holcoscellmaxlimit_0_2__cellmaxresumelimit = \
                holcoscellmaxlimit_0_2__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            /* I91-C80 */
            holcoscellmaxlimit_3__cellmaxresumelimit = \
                holcoscellmaxlimit_0_2__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            holcoscellmaxlimit_4_7__cellmaxresumelimit = 0;
            /* c101 */
            dyncelllimit_dyncellsetlimit = \
                mmu_params.shared_space_cells;
            /* o97-c80*2 */
            dyncelllimit_dyncellresetlimit = \
                dyncelllimit_dyncellsetlimit - \
                (mmu_params.ethernet_mtu * 2);
            color_drop_en_color_drop_en = 0;
            shared_pool_ctrl_dynamic_cos_drop_en = 255;
            shared_pool_ctrl_shared_pool_discard_en = 255;
            shared_pool_ctrl_sharded_pool_xoff_en = 0;
        }

        /* Program the setting */
        val = 0;
        /* i52 = c92 */
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &val, PKTSETLIMITf,
                          ibppktsetlimit_pktsetlimit);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &val, RESETLIMITSELf,
                          ibppktsetlimit_resetlimitsel);
        SOC_IF_ERROR_RETURN(WRITE_IBPPKTSETLIMITr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &val, MMU_FC_RX_ENABLEf,
                          mmu_fc_rx_en_mmu_fc_rx_enable);
        SOC_IF_ERROR_RETURN(WRITE_MMU_FC_RX_ENr(unit, port, val));
        val = 0;
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &val, MMU_FC_TX_ENABLEf,
                          mmu_fc_tx_en_mmu_fc_tx_enable);
        SOC_IF_ERROR_RETURN(WRITE_MMU_FC_TX_ENr(unit, port, val));

        for (cos = 0; cos < WH2_QUEUE_MAX_NUM; cos++) {
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcospktsetlimit_0_2__pktsetlimit;
            } else if (cos == 3) {
                fld_val = holcospktsetlimit_3__pktsetlimit;
            } else {
                fld_val = holcospktsetlimit_4_7__pktsetlimit;
            }
            SOC_IF_ERROR_RETURN
                (READ_HOLCOSPKTSETLIMITr(unit, port, cos, &val));
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &val,
                              PKTSETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSPKTSETLIMITr(unit, port, cos, val));

            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcospktresetlimit_0_2__pktresetlimit;
            } else if (cos == 3) {
                fld_val = holcospktresetlimit_3__pktresetlimit;
            } else {
                fld_val = holcospktresetlimit_4_7__pktresetlimit;
            }
            SOC_IF_ERROR_RETURN
                (READ_HOLCOSPKTRESETLIMITr(unit, port, cos, &val));
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &val,
                              PKTRESETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSPKTRESETLIMITr(unit, port, cos, val));

            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcosminxqcnt_0_2__holcosminxqcnt;
            } else if (cos == 3) {
                fld_val = holcosminxqcnt_3__holcosminxqcnt;
            } else {
                fld_val = holcosminxqcnt_4_7__holcosminxqcnt;
            }
            val = 0;
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &val,
                              HOLCOSMINXQCNTf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSMINXQCNTr(unit, port, cos, val));

            val = 0;
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = lwmcoscellsetlimit_0_2__cellsetlimit;
            } else if (cos == 3) {
                fld_val = lwmcoscellsetlimit_3__cellsetlimit;
            } else {
                fld_val = lwmcoscellsetlimit_4_7__cellsetlimit;
            }
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &val,
                              CELLSETLIMITf, fld_val);
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = lwmcoscellsetlimit_0_2__cellresetlimit;
            } else if (cos == 3) {
                fld_val = lwmcoscellsetlimit_3__cellresetlimit;
            } else {
                fld_val = lwmcoscellsetlimit_4_7__cellresetlimit;
            }
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &val,
                              CELLRESETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_LWMCOSCELLSETLIMITr(unit, port, cos, val));

            val = 0;
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcoscellmaxlimit_0_2__cellmaxlimit;
            } else if (cos == 3) {
                fld_val = holcoscellmaxlimit_3__cellmaxlimit;
            } else {
                fld_val = holcoscellmaxlimit_4_7__cellmaxlimit;
            }
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &val,
                              CELLMAXLIMITf, fld_val);
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcoscellmaxlimit_0_2__cellmaxresumelimit;
            } else if (cos == 3) {
                fld_val = holcoscellmaxlimit_3__cellmaxresumelimit;
            } else {
                fld_val = holcoscellmaxlimit_4_7__cellmaxresumelimit;
            }
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &val,
                              CELLMAXRESUMELIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSCELLMAXLIMITr(unit, port, cos, val));

            val = 0;
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = pgcelllimit_0_2__cellsetlimit;
            } else if (cos == 3) {
                fld_val = pgcelllimit_3__cellsetlimit;
            } else {
                fld_val = pgcelllimit_4_7__cellsetlimit;
            }
            soc_reg_field_set(unit, PGCELLLIMITr, &val,
                              CELLSETLIMITf, fld_val);
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = pgcelllimit_0_2__cellresetlimit;
            } else if (cos == 3) {
                fld_val = pgcelllimit_3__cellresetlimit;
            } else {
                fld_val = pgcelllimit_4_7__cellresetlimit;
            }
            soc_reg_field_set(unit, PGCELLLIMITr, &val,
                              CELLRESETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_PGCELLLIMITr(unit, port, cos, val));

            val = 0;
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = pgdiscardsetlimit_0_2__discardsetlimit;
            } else if (cos == 3) {
                fld_val = pgdiscardsetlimit_3__discardsetlimit;
            } else {
                fld_val = pgdiscardsetlimit_4_7__discardsetlimit;
            }
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &val,
                              DISCARDSETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_PGDISCARDSETLIMITr(unit, port, cos, val));

            val = 0;
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &val,
                              CNGPKTSETLIMIT0f,
                              cngcospktlimit0_0_7__cngpktsetlimit0);
            SOC_IF_ERROR_RETURN
                (WRITE_CNGCOSPKTLIMIT0r(unit, port, cos, val));
            val = 0;
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &val,
                              CNGPKTSETLIMIT1f,
                              cngcospktlimit1_0_7__cngpktsetlimit1);
            SOC_IF_ERROR_RETURN
                (WRITE_CNGCOSPKTLIMIT1r(unit, port, cos, val));
        }

        val = 0;
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &val,
                          CNGPORTPKTLIMIT0f,
                          cngportpktlimit0_cngportpktlimit0);
        SOC_IF_ERROR_RETURN(WRITE_CNGPORTPKTLIMIT0r(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &val,
                          CNGPORTPKTLIMIT1f,
                          cngportpktlimit1_cngportpktlimit1);
        SOC_IF_ERROR_RETURN(WRITE_CNGPORTPKTLIMIT1r(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &val,
                          DYNXQCNTPORTf,
                          dynxqcntport_dynxqcntport);
        SOC_IF_ERROR_RETURN(WRITE_DYNXQCNTPORTr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &val,
                          DYNRESETLIMPORTf,
                          dynresetlimport_dynresetlimport);
        SOC_IF_ERROR_RETURN(WRITE_DYNRESETLIMPORTr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                          DYNCELLSETLIMITf,
                          dyncelllimit_dyncellsetlimit);
        soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                          DYNCELLRESETLIMITf,
                          dyncelllimit_dyncellresetlimit);
        SOC_IF_ERROR_RETURN(WRITE_DYNCELLLIMITr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, COLOR_DROP_ENr, &val,
                          COLOR_DROP_ENf,
                          color_drop_en_color_drop_en);
        SOC_IF_ERROR_RETURN(WRITE_COLOR_DROP_ENr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, SHARED_POOL_CTRLr, &val,
                          DYNAMIC_COS_DROP_ENf,
                          shared_pool_ctrl_dynamic_cos_drop_en);
        soc_reg_field_set(unit, SHARED_POOL_CTRLr, &val,
                          SHARED_POOL_DISCARD_ENf,
                          shared_pool_ctrl_shared_pool_discard_en);
        soc_reg_field_set(unit, SHARED_POOL_CTRLr, &val,
                          SHARED_POOL_XOFF_ENf,
                          shared_pool_ctrl_sharded_pool_xoff_en);
        SOC_IF_ERROR_RETURN(WRITE_SHARED_POOL_CTRLr(unit, port, val));
   }

   return SOC_E_NONE;
}

STATIC int
soc_wolfhound2_mmu_lossy_lossless_pfc_init_helper(int unit)
{
    uint32 tmp;
    wh2_sku_info_t *matched_port_config = NULL;
    int option;
    int downlink_port_1g;
    int downlink_port_2p5g;
    int uplink_port_1g;
    int uplink_port_2p5g;
    int lossy_downlink_ports;
    int lossless_downlink_ports;
    soc_pbmp_t pbmp_lossless;

    SOC_PBMP_CLEAR(pbmp_lossless);
    pbmp_lossless = soc_property_get_pbmp_default(unit, spn_MMU_LOSSLESS_PBMP,
                                                  pbmp_lossless);

    /*
     * Lossless port count should between 1~4 and already validated by
     * soc_wolfhound2_mmu_init().
     */
    SOC_PBMP_COUNT(pbmp_lossless, lossless_downlink_ports);

    matched_port_config = &wh2_sku_port_config[matched_devid_idx[unit]];
    option = matched_port_config->config_option;
    /* MMU settings */
    switch (option) {
        case 1:
            downlink_port_1g = 20;
            downlink_port_2p5g = 4;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 2:
        case 3:
            downlink_port_1g = 24;
            downlink_port_2p5g = 0;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 4:
        case 5:
        case 6:
            downlink_port_1g = 24;
            downlink_port_2p5g = 0;
            uplink_port_1g = 4;
            uplink_port_2p5g = 0;
            break;
        case 7:
            downlink_port_1g = 12;
            downlink_port_2p5g = 4;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 8:
        case 9:
            downlink_port_1g = 16;
            downlink_port_2p5g = 0;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 10:
        case 11:
        case 12:
            downlink_port_1g = 16;
            downlink_port_2p5g = 0;
            uplink_port_1g = 4;
            uplink_port_2p5g = 0;
            break;
        case 13:
            downlink_port_1g = 4;
            downlink_port_2p5g = 4;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 14:
        case 15:
            downlink_port_1g = 8;
            downlink_port_2p5g = 0;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 16:
        case 17:
        case 18:
            downlink_port_1g = 8;
            downlink_port_2p5g = 0;
            uplink_port_1g = 4;
            uplink_port_2p5g = 0;
            break;
        default:
            /* Default matched wh2_sku_port_config[0] */
            downlink_port_1g = 20;
            downlink_port_2p5g = 4;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
    }
    lossy_downlink_ports = (downlink_port_1g + downlink_port_2p5g) - \
                           lossless_downlink_ports;
    /* Prepare MMU settings */
    /*
     * Traffic Attributes
     */
    /* c7 */
    mmu_params.min_packet_size_traffic_attr = 64;
    /*
     * c8
     * the value is 1.5 in doc, save 15 instead in sw.
     * any later usage should take care of this.
     */
    mmu_params.ethernet_mtu_traffic_attr = 15;
    /* c9 */
    mmu_params.max_packet_size_traffic_attr = 12288;
    /* c10 */
    mmu_params.standard_jumbo_frame_traffic_attr = 9216;
    /* c11 */
    mmu_params.lossless_traffic_mtu_size_traffic_attr = 2048;

    /*
     * General Information
     * moved to earlier place as these are needed by others
     */
    /* c79 = CEILING(C7/c49, 1)*/
    tmp = mmu_params.min_packet_size_traffic_attr / CELL_SIZE;
    mmu_params.min_packet_size = _ceil_func(tmp, 1);
    /* c80 = CEILING(C8*1024/c49, 1)*/
    tmp = (mmu_params.ethernet_mtu_traffic_attr * 1024) / CELL_SIZE;
    tmp /= 10; /* The value is 1.5 in MMU setting data, save as 15 in SW*/
    mmu_params.ethernet_mtu = _ceil_func(tmp, 1);
    /* c81 = CEILING(C9/c49, 1)*/
    tmp = mmu_params.max_packet_size_traffic_attr / CELL_SIZE;
    mmu_params.max_packet_size = _ceil_func(tmp, 1);
    /* c82 = CEILING(C10/c49, 1)*/
    tmp = mmu_params.standard_jumbo_frame_traffic_attr / CELL_SIZE;
    mmu_params.standard_jumbo_frame = _ceil_func(tmp, 1);
    /* c83 = CEILING(C11/c49, 1)*/
    tmp = mmu_params.lossless_traffic_mtu_size_traffic_attr / CELL_SIZE;
    mmu_params.lossless_traffic_mtu_size = _ceil_func(tmp, 1);

    /*
     * Reservations
     * moved to earlier place as these are needed by others
     */
    /* c70 = c80*4*/
    mmu_params.wrr_reserve = mmu_params.ethernet_mtu * 4;
    /* c71 = (31)*2 +7+29*/
    mmu_params.reserved_for_cfap = 98;
    /* c72 */
    mmu_params.skidmarker = 7;
    /* c73 */
    mmu_params.skidmarker_option = 3;
    /* c74 */
    mmu_params.prefetch = 9;

    /*
     * SW Config Property
     */
    /* c14 */
    mmu_params.total_physical_memory_sw_cfg = 4096;
    /* c15 = c14-c71 */
    mmu_params.cfapfullsetpoint_sw_cfg = \
        mmu_params.total_physical_memory_sw_cfg - mmu_params.reserved_for_cfap;
    /* c16 = c15 */
    mmu_params.total_cell_memory_for_admission_sw_cfg = \
        mmu_params.cfapfullsetpoint_sw_cfg;

    /* c17 configurable */
    mmu_params.num_1g_ports_uplink_ports_sw_cfg = uplink_port_1g;
    /* c18 configurable */
    mmu_params.num_2p5g_ports_uplink_ports_sw_cfg = uplink_port_2p5g;
    /* c19 configurable */
    mmu_params.num_lossy_downlink_ports_sw_cfg = lossy_downlink_ports;
    /* c20 configurable */
    mmu_params.num_lossless_downlink_ports_sw_cfg = lossless_downlink_ports;
    /* c21 = SUM(C17:C18) */
    mmu_params.number_of_uplink_ports_sw_cfg = \
        mmu_params.num_1g_ports_uplink_ports_sw_cfg + \
        mmu_params.num_2p5g_ports_uplink_ports_sw_cfg;
    /* c22 = SUM(C19:C20)*/
    mmu_params.number_of_downlink_ports_sw_cfg = \
        mmu_params.num_lossy_downlink_ports_sw_cfg + \
        mmu_params.num_lossless_downlink_ports_sw_cfg;
    /* c23 lossless = 1 */
    mmu_params.flow_control_enabled_sw_cfg = 1;
    /* c24 PFC = 1 */
    mmu_params.flow_control_type_sw_cfg = 1;
    /* c25 = 0.25, not used */
    /* mmu_params.queue_port_limit_ratio_sw_cfg = 0.25; */

    /*
     * Device Attributes
     * moved to earlier place as these are needed by others
     */
    /* c49 */
    mmu_params.cell_size = CELL_SIZE;
    /* c50 */
    mmu_params.num_egress_queues_per_port = 4;
    /* c51 */
    mmu_params.num_cpu_queues = 8;
    /* c52 = c17*/
    mmu_params.num_1g_ports_uplink_ports = \
        mmu_params.num_1g_ports_uplink_ports_sw_cfg;
    /* c53 = c18 */
    mmu_params.num_2p5g_ports_uplink_ports = \
        mmu_params.num_2p5g_ports_uplink_ports_sw_cfg;
    /* c54 = c19 */
    mmu_params.num_lossy_downlink_ports = \
        mmu_params.num_lossy_downlink_ports_sw_cfg;
    /* c55 = c20 */
    mmu_params.num_lossless_downlink_ports = \
        mmu_params.num_lossless_downlink_ports_sw_cfg;
    /* c56 */
    mmu_params.num_cpu_ports = 1;
    /* c57 = c21*/
    mmu_params.number_of_uplink_ports = \
        mmu_params.number_of_uplink_ports_sw_cfg;
    /* c58 = c22 */
    mmu_params.number_of_downlink_ports = \
        mmu_params.number_of_downlink_ports_sw_cfg;
    /* c59 = c14 */
    mmu_params.total_physical_memory = \
        mmu_params.total_physical_memory_sw_cfg;
    /* c60 = c16 */
    mmu_params.total_cell_memory = \
        mmu_params.total_cell_memory_for_admission_sw_cfg;
    /* c61 = c60 */
    mmu_params.total_advertised_cell_memory = \
        mmu_params.total_cell_memory;
    /* c62 */
    mmu_params.numxq_per_uplink_ports = 512;
    /* c63 */
    mmu_params.numxq_per_downlink_ports_and_cpu_port = 512;
    /* c64 */
    mmu_params.the_number_of_pipelines_per_device = 1;
    /* c66 */
    mmu_params.n_egress_queues_active_per_xpe = 1;
    /* c67 */
    mmu_params.n_ingress_ports_active_per_xpe = 1;

    /*
     * SW Config Parameter : may different from lossy to lossless
     */
    /* c46, used by (c88) */
    mmu_params.mmu_xoff_cell_threshold_downlink_lossless_ports = 36;
    /* c88 = c46, used by (c35) */
    mmu_params.xoff_cell_thresholds_per_port_pg_downlink_lossless_ports = \
        mmu_params.mmu_xoff_cell_threshold_downlink_lossless_ports;
    /* c35 =(c88+c46)*c20, used by later (c31) */
    mmu_params.egress_xq_min_reserve_lossless_uplink_ports = \
        (mmu_params.xoff_cell_thresholds_per_port_pg_downlink_lossless_ports + \
         mmu_params.mmu_xoff_cell_threshold_downlink_lossless_ports) * \
        mmu_params.num_lossless_downlink_ports_sw_cfg;

    /* c29 = c80 */
    mmu_params.egress_queue_min_reserve_uplink_ports_lossy = \
        mmu_params.ethernet_mtu;
    /* c30 = c80 */
    mmu_params.egress_queue_min_reserve_downlink_ports_lossy = \
        mmu_params.ethernet_mtu;
    /* c31 = c35 */
    mmu_params.egress_queue_min_reserve_uplink_ports_lossless = \
        mmu_params.egress_xq_min_reserve_lossless_uplink_ports;
    /* c32 */
    mmu_params.egress_queue_min_reserve_downlink_ports_lossless = 0;
    /* c33 = c80 */
    mmu_params.egress_queue_min_reserve_cpu_ports = \
        mmu_params.ethernet_mtu;
    /* c34 = c80 */
    mmu_params.egress_xq_min_reserve_lossy_uplink_and_downlink_ports = \
        mmu_params.ethernet_mtu;
    /* c36 */
    mmu_params.egress_xq_min_reserve_lossless_downlink_ports = 0;
    /* c37 */
    mmu_params.num_total_pri_groups = 4;
    /* c38 */
    mmu_params.num_active_pri_group_lossless = 1;
    /* c39 */
    mmu_params.num_lossless_queues = 1;
    /* c40 */
    mmu_params.num_lossy_queues = 3;
    /* c41 = c61 */
    mmu_params.mmu_xoff_pkt_threshold_uplink_ports = \
        mmu_params.total_advertised_cell_memory;
    /* c42 = c61 */
    mmu_params.mmu_xoff_pkt_threshold_downlink_ports = \
        mmu_params.total_advertised_cell_memory;
    /* c43 = c61 */
    mmu_params.mmu_xoff_cell_threshold_1g_port_uplink_ports = \
        mmu_params.total_advertised_cell_memory;
    /* c44 = c61 */
    mmu_params.mmu_xoff_cell_threshold_2p5g_port_uplink_ports = \
        mmu_params.total_advertised_cell_memory;
    /* c45 = c61 */
    mmu_params.mmu_xoff_cell_threshold_downlink_lossy_ports = \
        mmu_params.total_advertised_cell_memory;

    /*
     * Ingress Thresholds : may different from lossy to lossless
     */
    /* c86 = c46 */
    mmu_params.headroom_for_lossless_downlink_port = \
        mmu_params.mmu_xoff_cell_threshold_downlink_lossless_ports;
    /* c87 = c45 */
    mmu_params.xoff_cell_thresholds_per_port_pg_downlink_lossy_ports = \
        mmu_params.mmu_xoff_cell_threshold_downlink_lossy_ports;
    /* c89 = c43 */
    mmu_params.xoff_cell_threshold_1g_uplink_ports = \
        mmu_params.mmu_xoff_cell_threshold_1g_port_uplink_ports;
    /* c90 = c44 */
    mmu_params.xoff_cell_threshold_2p5g_uplink_ports = \
        mmu_params.mmu_xoff_cell_threshold_2p5g_port_uplink_ports;
    /* c91 = c41 */
    mmu_params.xoff_packet_thresholds_per_port_uplink_port = \
        mmu_params.mmu_xoff_pkt_threshold_uplink_ports;
    /* c92 = c42 */
    mmu_params.xoff_packet_thresholds_per_port_downlink_port = \
        mmu_params.mmu_xoff_pkt_threshold_downlink_ports;
    /* c93 = c61 */
    mmu_params.discard_limit_per_port_pg_lossy_downlink_port = \
        mmu_params.total_advertised_cell_memory;
    /* c94 = c88+c86 */
    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port = \
        mmu_params.xoff_cell_thresholds_per_port_pg_downlink_lossless_ports + \
        mmu_params.headroom_for_lossless_downlink_port;
    /* c95 = c61 */
    mmu_params.discard_limit_per_port_pg_uplink_port = \
        mmu_params.total_advertised_cell_memory;

    /*
     * Egress Thresholds
     */
    /* c97 = c29*c57*c40+c57*c31*c39 */
    mmu_params.total_reserved_cells_for_uplink_ports = \
        (mmu_params.egress_queue_min_reserve_uplink_ports_lossy * \
         mmu_params.number_of_uplink_ports * \
         mmu_params.num_lossy_queues) + \
        (mmu_params.number_of_uplink_ports * \
         mmu_params.egress_queue_min_reserve_uplink_ports_lossless * \
         mmu_params.num_lossless_queues);
    /* c98 = c58*c30*(c40)+c58*c32*c39 */
    mmu_params.total_reserved_cells_for_downlink_ports = \
        (mmu_params.number_of_downlink_ports * \
         mmu_params.egress_queue_min_reserve_downlink_ports_lossy * \
         mmu_params.num_lossy_queues) + \
        (mmu_params.number_of_downlink_ports * \
         mmu_params.egress_queue_min_reserve_downlink_ports_lossless * \
         mmu_params.num_lossless_queues);
    /* c99 = c56*c33*c51 */
    mmu_params.total_reserved_cells_for_cpu_port = \
        mmu_params.num_cpu_ports * \
        mmu_params.egress_queue_min_reserve_cpu_ports * \
        mmu_params.num_cpu_queues;
    /* c100 = SUM(c97:c99) */
    mmu_params.total_reserved = \
        mmu_params.total_reserved_cells_for_uplink_ports + \
        mmu_params.total_reserved_cells_for_downlink_ports + \
        mmu_params.total_reserved_cells_for_cpu_port;
    /* c101 = c61-c100 */
    mmu_params.shared_space_cells = \
        mmu_params.total_advertised_cell_memory - \
        mmu_params.total_reserved;
    /* c102 = c34*c40+c35*c39 */
    mmu_params.reserved_xqs_per_uplink_port = \
        (mmu_params.egress_xq_min_reserve_lossy_uplink_and_downlink_ports * \
         mmu_params.num_lossy_queues) + \
        (mmu_params.egress_xq_min_reserve_lossless_uplink_ports * \
         mmu_params.num_lossless_queues);
    /* c103 = c62-c102 */
    mmu_params.shared_xqs_per_uplink_port = \
        mmu_params.numxq_per_uplink_ports - \
        mmu_params.reserved_xqs_per_uplink_port;
    /* c104 = c34*c40+c36*c39 */
    mmu_params.reserved_xqs_per_downlink_port = \
        (mmu_params.egress_xq_min_reserve_lossy_uplink_and_downlink_ports * \
         mmu_params.num_lossy_queues) + \
        (mmu_params.egress_xq_min_reserve_lossless_downlink_ports * \
         mmu_params.num_lossless_queues);
    /* c105 = c63-c104 */
    mmu_params.shared_xqs_per_downlink_port = \
        mmu_params.numxq_per_downlink_ports_and_cpu_port - \
        mmu_params.reserved_xqs_per_downlink_port;
    /* c106 = c33 * c51 */
    mmu_params.reserved_xqs_at_cpu_port = \
        mmu_params.egress_queue_min_reserve_cpu_ports * \
        mmu_params.num_cpu_queues;
    /* c107 = c63 - c106 */
    mmu_params.shared_xqs_at_cpu_port = \
        mmu_params.numxq_per_downlink_ports_and_cpu_port - \
        mmu_params.reserved_xqs_at_cpu_port;

    /* Program MMU settings */
    SOC_IF_ERROR_RETURN(
        soc_wolfhound2_mmu_lossy_lossless_pfc_config_helper(unit));

    return SOC_E_NONE;
}

STATIC int
soc_wolfhound2_mmu_lossy_or_lossless_config_helper(int unit, int lossless)
{
    uint32 val = 0;
    uint32 fld_val = 0;
    uint32 tmp = 0;
    int port; /* logical port id */
    int pport; /* physical port id */
    int speed;
    int cos;
    int i;
    uint32 ibppktsetlimit_pktsetlimit;
    uint32 ibppktsetlimit_resetlimitsel;
    uint32 mmu_fc_rx_en_mmu_fc_rx_enable;
    uint32 mmu_fc_tx_en_mmu_fc_tx_enable;
    uint32 pgcelllimit_0_2__cellsetlimit;
    uint32 pgcelllimit_3__cellsetlimit;
    uint32 pgcelllimit_4_6__cellsetlimit;
    uint32 pgcelllimit_7__cellsetlimit;
    uint32 pgcelllimit_0_2__cellresetlimit;
    uint32 pgcelllimit_3__cellresetlimit;
    uint32 pgcelllimit_4_6__cellresetlimit;
    uint32 pgcelllimit_7__cellresetlimit;
    uint32 pgdiscardsetlimit_0_2__discardsetlimit;
    uint32 pgdiscardsetlimit_3__discardsetlimit;
    uint32 pgdiscardsetlimit_4_6__discardsetlimit;
    uint32 pgdiscardsetlimit_7__discardsetlimit;
    uint32 holcosminxqcnt_0_2__holcosminxqcnt;
    uint32 holcosminxqcnt_3__holcosminxqcnt;
    uint32 holcosminxqcnt_4_7__holcosminxqcnt;
    uint32 holcospktsetlimit_0_2__pktsetlimit;
    uint32 holcospktsetlimit_3__pktsetlimit;
    uint32 holcospktsetlimit_4_7__pktsetlimit;
    uint32 holcospktresetlimit_0_2__pktresetlimit;
    uint32 holcospktresetlimit_3__pktresetlimit;
    uint32 holcospktresetlimit_4_7__pktresetlimit;
    uint32 cngcospktlimit0_0_7__cngpktsetlimit0;
    uint32 cngcospktlimit1_0_7__cngpktsetlimit1;
    uint32 cngportpktlimit0_cngportpktlimit0;
    uint32 cngportpktlimit1_cngportpktlimit1;
    uint32 dynxqcntport_dynxqcntport;
    uint32 dynresetlimport_dynresetlimport;
    uint32 lwmcoscellsetlimit_0_2__cellsetlimit;
    uint32 lwmcoscellsetlimit_3__cellsetlimit;
    uint32 lwmcoscellsetlimit_4_7__cellsetlimit;
    uint32 lwmcoscellsetlimit_0_2__cellresetlimit;
    uint32 lwmcoscellsetlimit_3__cellresetlimit;
    uint32 lwmcoscellsetlimit_4_7__cellresetlimit;
    uint32 holcoscellmaxlimit_0_2__cellmaxlimit;
    uint32 holcoscellmaxlimit_3__cellmaxlimit;
    uint32 holcoscellmaxlimit_4_7__cellmaxlimit;
    uint32 holcoscellmaxlimit_0_2__cellmaxresumelimit;
    uint32 holcoscellmaxlimit_3__cellmaxresumelimit;
    uint32 holcoscellmaxlimit_4_7__cellmaxresumelimit;
    uint32 dyncelllimit_dyncellsetlimit;
    uint32 dyncelllimit_dyncellresetlimit;
    uint32 color_drop_en_color_drop_en;
    uint32 shared_pool_ctrl_dynamic_cos_drop_en;
    uint32 shared_pool_ctrl_shared_pool_discard_en;
    uint32 shared_pool_ctrl_sharded_pool_xoff_en;

    SOC_IF_ERROR_RETURN(READ_CFAPFULLTHRESHOLDr(unit, &val));
    /* i6 = c15 */
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDr, &val,
                      CFAPFULLSETPOINTf,
                      mmu_params.cfapfullsetpoint_sw_cfg);
    /* i7 = i6 - c82 */
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDr, &val,
                      CFAPFULLRESETPOINTf,
                      mmu_params.cfapfullsetpoint_sw_cfg - \
                      mmu_params.standard_jumbo_frame);
    SOC_IF_ERROR_RETURN(WRITE_CFAPFULLTHRESHOLDr(unit, val));

    /* i10 = c16 */
    SOC_IF_ERROR_RETURN(READ_GBLLIMITSETLIMITr(unit, &val));
    soc_reg_field_set(unit, GBLLIMITSETLIMITr, &val,
                      GBLCELLSETLIMITf,
                      mmu_params.total_cell_memory_for_admission_sw_cfg);
    SOC_IF_ERROR_RETURN(WRITE_GBLLIMITSETLIMITr(unit, val));

    /* i11 = i10 */
    SOC_IF_ERROR_RETURN(READ_GBLLIMITRESETLIMITr(unit, &val));
    soc_reg_field_set(unit, GBLLIMITRESETLIMITr, &val,
                      GBLCELLRESETLIMITf,
                      mmu_params.total_cell_memory_for_admission_sw_cfg);
    SOC_IF_ERROR_RETURN(WRITE_GBLLIMITRESETLIMITr(unit, val));

    /* Device Wide - Egress Thresholds */
    val = 0;
    /* i15 = c101 */
    soc_reg_field_set(unit, TOTALDYNCELLSETLIMITr,
                      &val, TOTALDYNCELLSETLIMITf,
                      mmu_params.shared_space_cells);
    SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLSETLIMITr(unit, val));
    val = 0;
    /* i16 =i15-c82*2*/
    soc_reg_field_set(unit, TOTALDYNCELLRESETLIMITr,
                      &val, TOTALDYNCELLRESETLIMITf,
                      mmu_params.shared_space_cells - \
                      (mmu_params.standard_jumbo_frame * 2));
    SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLRESETLIMITr(unit, val));

    /* MICCONFIGr */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    soc_reg_field_set(unit, MISCCONFIGr, &val, MULTIPLE_ACCOUNTING_FIX_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &val, CNG_DROP_ENf, 0);
    soc_reg_field_set(unit, MISCCONFIGr, &val, DYN_XQ_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &val, HOL_CELL_SOP_DROP_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &val, DYNAMIC_MEMORY_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &val, SKIDMARKERf, 3);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));

    PBMP_ALL_ITER(unit, port) {
        pport = SOC_INFO(unit).port_l2p_mapping[port];
        speed = SOC_INFO(unit).port_speed_max[port];

        val = 0;
        if (lossless) {
            if (IS_CPU_PORT(unit, port)) {
                soc_reg_field_set(unit, PG_CTRL0r, &val, PPFC_PG_ENf, 0);
            } else {
                soc_reg_field_set(unit, PG_CTRL0r, &val, PPFC_PG_ENf, 0x80);
            }
        } else {
            soc_reg_field_set(unit, PG_CTRL0r, &val, PPFC_PG_ENf, 0);
        }
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI0_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI1_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI2_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI3_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI4_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI5_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI6_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI7_GRPf, 7);
        SOC_IF_ERROR_RETURN(WRITE_PG_CTRL0r(unit, port, val));
        val = 0;
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI8_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI9_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI10_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI11_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI12_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI13_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI14_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI15_GRPf, 7);
        SOC_IF_ERROR_RETURN(WRITE_PG_CTRL1r(unit, port, val));
        val = 0;
        soc_reg_field_set(unit, PG2TCr, &val, PG_BMPf, 0);
        for (i = 0; i < 8; i++) {
            SOC_IF_ERROR_RETURN(WRITE_PG2TCr(unit, port, i, val));
        }

        /* Settings based on port type */
        if (IS_CPU_PORT(unit, port)) {
            ibppktsetlimit_pktsetlimit = \
                mmu_params.\
                xoff_packet_thresholds_per_port_downlink_port; /* c92 */
            ibppktsetlimit_resetlimitsel = 3;
            mmu_fc_rx_en_mmu_fc_rx_enable = 0;
            mmu_fc_tx_en_mmu_fc_tx_enable = 0;
            if (lossless) {
                /* c93 */
                pgcelllimit_0_2__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c93 */
                pgcelllimit_3__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c93 */
                pgcelllimit_4_6__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c87 */
                pgcelllimit_7__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                /* c93 */
                pgcelllimit_0_2__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c93 */
                pgcelllimit_3__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c93 */
                pgcelllimit_4_6__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c87 */
                pgcelllimit_7__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
            } else {
                /* c87 */
                pgcelllimit_0_2__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                /* c87 */
                pgcelllimit_3__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                /* c87 */
                pgcelllimit_4_6__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                /* c87 */
                pgcelllimit_7__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                /* c87 */
                pgcelllimit_0_2__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                /* c87 */
                pgcelllimit_3__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                /* c87 */
                pgcelllimit_4_6__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                /* c87 */
                pgcelllimit_7__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
            }
            pgdiscardsetlimit_0_2__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port; /* c93 */
            pgdiscardsetlimit_3__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port; /* c93 */
            pgdiscardsetlimit_4_6__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port; /* c93 */
            pgdiscardsetlimit_7__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port; /* c93 */

            holcosminxqcnt_0_2__holcosminxqcnt = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            holcosminxqcnt_3__holcosminxqcnt = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            holcosminxqcnt_4_7__holcosminxqcnt = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            holcospktsetlimit_0_2__pktsetlimit = \
                mmu_params.shared_xqs_at_cpu_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_0_2__holcosminxqcnt; /* C107-C72-C74+Q70 */
            holcospktsetlimit_3__pktsetlimit = \
                mmu_params.shared_xqs_at_cpu_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_3__holcosminxqcnt; /* C107-C72-C74+Q71 */
            holcospktsetlimit_4_7__pktsetlimit = \
                mmu_params.shared_xqs_at_cpu_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_4_7__holcosminxqcnt; /* C107-C72-C74+Q72 */
            holcospktresetlimit_0_2__pktresetlimit = \
                holcospktsetlimit_0_2__pktsetlimit - 1; /* q73 - 1 */
            holcospktresetlimit_3__pktresetlimit = \
                holcospktsetlimit_3__pktsetlimit - 1; /* q74 - 1 */
            holcospktresetlimit_4_7__pktresetlimit = \
                holcospktsetlimit_4_7__pktsetlimit - 1; /* q75 - 1 */
            cngcospktlimit0_0_7__cngpktsetlimit0 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1; /*c63-1*/
            cngcospktlimit1_0_7__cngpktsetlimit1 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1; /*c63-1*/
            cngportpktlimit0_cngportpktlimit0 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1; /*c63-1*/
            cngportpktlimit1_cngportpktlimit1 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1; /*c63-1*/
            dynxqcntport_dynxqcntport = \
                mmu_params.shared_xqs_at_cpu_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch; /* c107-c72-c74 */
            dynresetlimport_dynresetlimport = \
                dynxqcntport_dynxqcntport - 2; /* q83 - 2 */
            lwmcoscellsetlimit_0_2__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_3__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_4_7__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_0_2__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_3__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_4_7__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            /* CEILING(c101*C25+Q85, 1) */
            tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
            holcoscellmaxlimit_0_2__cellmaxlimit = \
                tmp + lwmcoscellsetlimit_0_2__cellsetlimit;
            /* CEILING(c101*C25+Q86, 1) */
            tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
            holcoscellmaxlimit_3__cellmaxlimit = \
                tmp + lwmcoscellsetlimit_3__cellsetlimit;
            /* CEILING(c101*C25+Q87, 1) */
            tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
            holcoscellmaxlimit_4_7__cellmaxlimit = \
                tmp + lwmcoscellsetlimit_4_7__cellsetlimit;
            holcoscellmaxlimit_0_2__cellmaxresumelimit = \
                holcoscellmaxlimit_0_2__cellmaxlimit - \
                mmu_params.ethernet_mtu; /* q91 - c80 */
            holcoscellmaxlimit_3__cellmaxresumelimit = \
                holcoscellmaxlimit_3__cellmaxlimit - \
                mmu_params.ethernet_mtu; /* q92 - c80 */
            holcoscellmaxlimit_4_7__cellmaxresumelimit = \
                holcoscellmaxlimit_4_7__cellmaxlimit - \
                mmu_params.ethernet_mtu; /* q93 - c80 */
            dyncelllimit_dyncellsetlimit = \
                mmu_params.shared_space_cells; /* c101 */
            dyncelllimit_dyncellresetlimit = \
                dyncelllimit_dyncellsetlimit - \
                (mmu_params.ethernet_mtu * 2); /* q97-c80*2 */
            color_drop_en_color_drop_en = 0;
            shared_pool_ctrl_dynamic_cos_drop_en = 255;
            shared_pool_ctrl_shared_pool_discard_en = 255;
            shared_pool_ctrl_sharded_pool_xoff_en = 0;
        } else if (pport >= 34) { /* uplink port 1G, 2.5G */
            if (lossless) {
                SOC_IF_ERROR_RETURN(WRITE_PG2TCr(unit, port, 7, 0x80));
            }

            if (lossless) {
                if (speed == 10) { /* 1G */
                    /* c95 */
                    pgcelllimit_0_2__cellsetlimit = \
                        mmu_params.discard_limit_per_port_pg_uplink_port;
                    /* c95 */
                    pgcelllimit_3__cellsetlimit = \
                        mmu_params.discard_limit_per_port_pg_uplink_port;
                    /* c95 */
                    pgcelllimit_4_6__cellsetlimit = \
                        mmu_params.discard_limit_per_port_pg_uplink_port;
                    /* c89 */
                    pgcelllimit_7__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_1g_uplink_ports;
                    /* c95 */
                    pgcelllimit_0_2__cellresetlimit = \
                        mmu_params.discard_limit_per_port_pg_uplink_port;
                    /* c95 */
                    pgcelllimit_3__cellresetlimit = \
                        mmu_params.discard_limit_per_port_pg_uplink_port;
                    /* c95 */
                    pgcelllimit_4_6__cellresetlimit = \
                        mmu_params.discard_limit_per_port_pg_uplink_port;
                    /* c89 */
                    pgcelllimit_7__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_1g_uplink_ports;
                } else { /* 2.5 G */
                    /* c95 */
                    pgcelllimit_0_2__cellsetlimit = \
                        mmu_params.discard_limit_per_port_pg_uplink_port;
                    /* c95 */
                    pgcelllimit_3__cellsetlimit = \
                        mmu_params.discard_limit_per_port_pg_uplink_port;
                    /* c95 */
                    pgcelllimit_4_6__cellsetlimit = \
                        mmu_params.discard_limit_per_port_pg_uplink_port;
                    /* c90 */
                    pgcelllimit_7__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_2p5g_uplink_ports;
                    /* c95 */
                    pgcelllimit_0_2__cellresetlimit = \
                        mmu_params.discard_limit_per_port_pg_uplink_port;
                    /* c95 */
                    pgcelllimit_3__cellresetlimit = \
                        mmu_params.discard_limit_per_port_pg_uplink_port;
                    /* c95 */
                    pgcelllimit_4_6__cellresetlimit = \
                        mmu_params.discard_limit_per_port_pg_uplink_port;
                    /* c90 */
                    pgcelllimit_7__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_2p5g_uplink_ports;
                }
            } else {
                if (speed == 10) { /* 1G */
                    /* c89 */
                    pgcelllimit_0_2__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_1g_uplink_ports;
                    /* c89 */
                    pgcelllimit_3__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_1g_uplink_ports;
                    /* c89 */
                    pgcelllimit_4_6__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_1g_uplink_ports;
                    /* c89 */
                    pgcelllimit_7__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_1g_uplink_ports;
                    /* c89 */
                    pgcelllimit_0_2__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_1g_uplink_ports;
                    /* c89 */
                    pgcelllimit_3__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_1g_uplink_ports;
                    /* c89 */
                    pgcelllimit_4_6__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_1g_uplink_ports;
                    /* c89 */
                    pgcelllimit_7__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_1g_uplink_ports;
                } else { /* 2.5 G */
                    /* c90 */
                    pgcelllimit_0_2__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_2p5g_uplink_ports;
                    /* c90 */
                    pgcelllimit_3__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_2p5g_uplink_ports;
                    /* c90 */
                    pgcelllimit_4_6__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_2p5g_uplink_ports;
                    /* c90 */
                    pgcelllimit_7__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_2p5g_uplink_ports;
                    /* c90 */
                    pgcelllimit_0_2__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_2p5g_uplink_ports;
                    /* c90 */
                    pgcelllimit_3__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_2p5g_uplink_ports;
                    /* c90 */
                    pgcelllimit_4_6__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_2p5g_uplink_ports;
                    /* c90 */
                    pgcelllimit_7__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_threshold_2p5g_uplink_ports;
                }
            }
            /* c91 */
            ibppktsetlimit_pktsetlimit = \
                mmu_params.\
                xoff_packet_thresholds_per_port_uplink_port;
            ibppktsetlimit_resetlimitsel = 3;
            mmu_fc_rx_en_mmu_fc_rx_enable = 0;
            if (lossless) {
                mmu_fc_tx_en_mmu_fc_tx_enable = 0x80;
            } else {
                mmu_fc_tx_en_mmu_fc_tx_enable = 0;
            }
            /* c95 */
            pgdiscardsetlimit_0_2__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_uplink_port;
            /* c95 */
            pgdiscardsetlimit_3__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_uplink_port;
            /* c95 */
            pgdiscardsetlimit_4_6__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_uplink_port;
            /* c95 */
            pgdiscardsetlimit_7__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_uplink_port;

            if (lossless) {
                /* c36 */
                holcosminxqcnt_0_2__holcosminxqcnt = \
                    mmu_params.egress_xq_min_reserve_lossless_downlink_ports;
                /* c36 */
                holcosminxqcnt_3__holcosminxqcnt = \
                    mmu_params.egress_xq_min_reserve_lossless_downlink_ports;
                holcosminxqcnt_4_7__holcosminxqcnt = 0;
                /* C103-C72-C74+M70 */
                holcospktsetlimit_0_2__pktsetlimit = \
                    mmu_params.shared_xqs_per_uplink_port - \
                    mmu_params.skidmarker - \
                    mmu_params.prefetch + \
                    holcosminxqcnt_0_2__holcosminxqcnt;
                /* C103-C72-C74+M71 */
                holcospktsetlimit_3__pktsetlimit = \
                    mmu_params.shared_xqs_per_uplink_port - \
                    mmu_params.skidmarker - \
                    mmu_params.prefetch + \
                    holcosminxqcnt_3__holcosminxqcnt;
                holcospktsetlimit_4_7__pktsetlimit = 0;
            } else {
                /* c34 */
                holcosminxqcnt_0_2__holcosminxqcnt = \
                    mmu_params.\
                    egress_xq_min_reserve_lossy_uplink_and_downlink_ports;
                /* c34 */
                holcosminxqcnt_3__holcosminxqcnt = \
                    mmu_params.\
                    egress_xq_min_reserve_lossy_uplink_and_downlink_ports;
                holcosminxqcnt_4_7__holcosminxqcnt = 0;
                /* C103-C72-C74+C34 */
                holcospktsetlimit_0_2__pktsetlimit = \
                    mmu_params.shared_xqs_per_uplink_port - \
                    mmu_params.skidmarker - \
                    mmu_params.prefetch + \
                    mmu_params.\
                    egress_xq_min_reserve_lossy_uplink_and_downlink_ports;
                /* C103-C72-C74+C34 */
                holcospktsetlimit_3__pktsetlimit = \
                    mmu_params.shared_xqs_per_uplink_port - \
                    mmu_params.skidmarker - \
                    mmu_params.prefetch + \
                    mmu_params.\
                    egress_xq_min_reserve_lossy_uplink_and_downlink_ports;
                holcospktsetlimit_4_7__pktsetlimit = 0;
            }
            /* m73 - 1 */
            holcospktresetlimit_0_2__pktresetlimit = \
                holcospktsetlimit_0_2__pktsetlimit - 1;
            /* m74 - 1 */
            holcospktresetlimit_3__pktresetlimit = \
                holcospktsetlimit_3__pktsetlimit - 1;
            holcospktresetlimit_4_7__pktresetlimit = 0;
            /* c62 - 1 */
            cngcospktlimit0_0_7__cngpktsetlimit0 = \
                mmu_params.numxq_per_uplink_ports - 1;
            /* c62 - 1 */
            cngcospktlimit1_0_7__cngpktsetlimit1 = \
                mmu_params.numxq_per_uplink_ports - 1;
            /* c62 - 1 */
            cngportpktlimit0_cngportpktlimit0 = \
                mmu_params.numxq_per_uplink_ports - 1;
            /* c62 - 1 */
            cngportpktlimit1_cngportpktlimit1 = \
                mmu_params.numxq_per_uplink_ports - 1;
            /* c103-c72-c74 */
            dynxqcntport_dynxqcntport = \
                mmu_params.shared_xqs_per_uplink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch;
            /* m83 - 2 */
            dynresetlimport_dynresetlimport = \
                dynxqcntport_dynxqcntport - 2;
            if (lossless) {
                /* c31 */
                lwmcoscellsetlimit_0_2__cellsetlimit = \
                    mmu_params.egress_queue_min_reserve_uplink_ports_lossless;
                /* c31 */
                lwmcoscellsetlimit_3__cellsetlimit = \
                    mmu_params.egress_queue_min_reserve_uplink_ports_lossless;
                lwmcoscellsetlimit_4_7__cellsetlimit = 0;
                /* c31 */
                lwmcoscellsetlimit_0_2__cellresetlimit = \
                    mmu_params.egress_queue_min_reserve_uplink_ports_lossless;
                /* c31 */
                lwmcoscellsetlimit_3__cellresetlimit = \
                    mmu_params.egress_queue_min_reserve_uplink_ports_lossless;
                lwmcoscellsetlimit_4_7__cellresetlimit = 0;
                /* CEILING((C87+C85)*C54+(C88+C86)*C55+(C89+C85)*C53+(C90+C86)*C52+I85,1) */
                tmp = (mmu_params.\
                       xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports +
                       mmu_params.headroom_for_1g_port) * \
                      mmu_params.num_1g_ports_downlink_ports + \
                      (mmu_params.\
                       xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports +
                       mmu_params.headroom_for_2p5g_port) * \
                      mmu_params.num_2p5g_ports_downlink_ports + \
                      (mmu_params.\
                       xoff_cell_threshold_1g_uplink_ports + \
                       mmu_params.headroom_for_1g_port) * \
                      mmu_params.num_2p5g_ports_uplink_ports + \
                      (mmu_params.xoff_cell_threshold_2p5g_uplink_ports + \
                       mmu_params.headroom_for_2p5g_port) * \
                      mmu_params.num_1g_ports_uplink_ports + \
                      lwmcoscellsetlimit_0_2__cellsetlimit;
                holcoscellmaxlimit_0_2__cellmaxlimit = _ceil_func(tmp, 1);
                /* CEILING((C87+C85)*C54+(C88+C86)*C55+(C89+C85)*C53+(C90+C86)*C52+I86, 1) */
                tmp = (mmu_params.\
                       xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports +
                       mmu_params.headroom_for_1g_port) * \
                      mmu_params.num_1g_ports_downlink_ports + \
                      (mmu_params.\
                       xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports +
                       mmu_params.headroom_for_2p5g_port) * \
                      mmu_params.num_2p5g_ports_downlink_ports + \
                      (mmu_params.\
                       xoff_cell_threshold_1g_uplink_ports + \
                       mmu_params.headroom_for_1g_port) * \
                      mmu_params.num_2p5g_ports_uplink_ports + \
                      (mmu_params.xoff_cell_threshold_2p5g_uplink_ports + \
                       mmu_params.headroom_for_2p5g_port) * \
                      mmu_params.num_1g_ports_uplink_ports + \
                      lwmcoscellsetlimit_3__cellsetlimit;
                holcoscellmaxlimit_3__cellmaxlimit = _ceil_func(tmp, 1);;
                holcoscellmaxlimit_4_7__cellmaxlimit = 0;
            } else {
                /* c29 */
                lwmcoscellsetlimit_0_2__cellsetlimit = \
                    mmu_params.egress_queue_min_reserve_uplink_ports_lossy;
                /* c29 */
                lwmcoscellsetlimit_3__cellsetlimit = \
                    mmu_params.egress_queue_min_reserve_uplink_ports_lossy;
                lwmcoscellsetlimit_4_7__cellsetlimit = 0;
                /* c29 */
                lwmcoscellsetlimit_0_2__cellresetlimit = \
                    mmu_params.egress_queue_min_reserve_uplink_ports_lossy;
                /* c29 */
                lwmcoscellsetlimit_3__cellresetlimit = \
                    mmu_params.egress_queue_min_reserve_uplink_ports_lossy;
                lwmcoscellsetlimit_4_7__cellresetlimit = 0;
                /* CEILING(c101*C25+o85, 1) */
                tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
                holcoscellmaxlimit_0_2__cellmaxlimit = \
                    tmp + lwmcoscellsetlimit_0_2__cellsetlimit;
                /* CEILING(c101*C25+o86, 1) */
                tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
                holcoscellmaxlimit_3__cellmaxlimit = \
                    tmp + lwmcoscellsetlimit_3__cellsetlimit;
                holcoscellmaxlimit_4_7__cellmaxlimit = 0;
            }
            /* o91 - c80 */
            holcoscellmaxlimit_0_2__cellmaxresumelimit = \
                holcoscellmaxlimit_0_2__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            /* o92 - c80 */
            holcoscellmaxlimit_3__cellmaxresumelimit = \
                holcoscellmaxlimit_3__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            holcoscellmaxlimit_4_7__cellmaxresumelimit = 0;
            /* c101 */
            dyncelllimit_dyncellsetlimit = \
                mmu_params.shared_space_cells;
            /* o97-c80*2 */
            dyncelllimit_dyncellresetlimit = \
                dyncelllimit_dyncellsetlimit - \
                (mmu_params.ethernet_mtu * 2);
            color_drop_en_color_drop_en = 0;
            shared_pool_ctrl_dynamic_cos_drop_en = 255;
            shared_pool_ctrl_shared_pool_discard_en = 255;
            shared_pool_ctrl_sharded_pool_xoff_en = 0;
        } else { /* downlink port 1G, 2.5G */
            if (lossless) {
                SOC_IF_ERROR_RETURN(WRITE_PG2TCr(unit, port, 7, 0x80));
            }

            if (lossless) {
                if (speed == 10) { /* 1G */
                    /* c93 */
                    pgcelllimit_0_2__cellsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                    /* c93 */
                    pgcelllimit_3__cellsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                    /* c93 */
                    pgcelllimit_4_6__cellsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                    /* c87 */
                    pgcelllimit_7__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                    /* c93 */
                    pgcelllimit_0_2__cellresetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                    /* c93 */
                    pgcelllimit_3__cellresetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                    /* c93 */
                    pgcelllimit_4_6__cellresetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                    /* c87 */
                    pgcelllimit_7__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                    /* c93 */
                    pgdiscardsetlimit_0_2__discardsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                    /* c93 */
                    pgdiscardsetlimit_3__discardsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                    /* c93 */
                    pgdiscardsetlimit_4_6__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                    /* c93 */
                    pgdiscardsetlimit_7__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                } else { /* 2.5 G */
                    /* c94 */
                    pgcelllimit_0_2__cellsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                    /* c94 */
                    pgcelllimit_3__cellsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                    /* c94 */
                    pgcelllimit_4_6__cellsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                    /* c88 */
                    pgcelllimit_7__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports;
                    /* c94 */
                    pgcelllimit_0_2__cellresetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                    /* c94 */
                    pgcelllimit_3__cellresetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                    /* c94 */
                    pgcelllimit_4_6__cellresetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                    /* c88 */
                    pgcelllimit_7__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports;
                    /* c94 */
                    pgdiscardsetlimit_0_2__discardsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                    /* c94 */
                    pgdiscardsetlimit_3__discardsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                    /* c94 */
                    pgdiscardsetlimit_4_6__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                    /* c94 */
                    pgdiscardsetlimit_7__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                }
            } else {
                if (speed == 10) { /* 1G */
                    /* c87 */
                    pgcelllimit_0_2__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                    /* c87 */
                    pgcelllimit_3__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                    /* c87 */
                    pgcelllimit_4_6__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                    /* c87 */
                    pgcelllimit_7__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                    /* c87 */
                    pgcelllimit_0_2__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                    /* c87 */
                    pgcelllimit_3__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                    /* c87 */
                    pgcelllimit_4_6__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                    /* c87 */
                    pgcelllimit_7__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                    /* c93 */
                    pgdiscardsetlimit_0_2__discardsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                    /* c93 */
                    pgdiscardsetlimit_3__discardsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                    /* c93 */
                    pgdiscardsetlimit_4_6__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                    /* c93 */
                    pgdiscardsetlimit_7__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                } else { /* 2.5 G */
                    /* c88 */
                    pgcelllimit_0_2__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports;
                    /* c88 */
                    pgcelllimit_3__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports;
                    /* c88 */
                    pgcelllimit_4_6__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports;
                    /* c88 */
                    pgcelllimit_7__cellsetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports;
                    /* c88 */
                    pgcelllimit_0_2__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports;
                    /* c88 */
                    pgcelllimit_3__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports;
                    /* c88 */
                    pgcelllimit_4_6__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports;
                    /* c88 */
                    pgcelllimit_7__cellresetlimit = \
                        mmu_params.\
                        xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports;
                    /* c94 */
                    pgdiscardsetlimit_0_2__discardsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                    /* c94 */
                    pgdiscardsetlimit_3__discardsetlimit = \
                        mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                    /* c94 */
                    pgdiscardsetlimit_4_6__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                    /* c94 */
                    pgdiscardsetlimit_7__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                }
            }
            /* c92 */
            ibppktsetlimit_pktsetlimit = \
                mmu_params.\
                xoff_packet_thresholds_per_port_downlink_port;
            ibppktsetlimit_resetlimitsel = 3;
            mmu_fc_rx_en_mmu_fc_rx_enable = 0;
            if (lossless) {
                mmu_fc_tx_en_mmu_fc_tx_enable = 0x80;
            } else {
                mmu_fc_tx_en_mmu_fc_tx_enable = 0;
            }

            if (lossless) {
                /* c35 */
                holcosminxqcnt_0_2__holcosminxqcnt = \
                    mmu_params.egress_xq_min_reserve_lossless_uplink_ports;
                /* c35 */
                holcosminxqcnt_3__holcosminxqcnt = \
                    mmu_params.egress_xq_min_reserve_lossless_uplink_ports;
                holcosminxqcnt_4_7__holcosminxqcnt = 0;
                /* C105-C72-C74+I70 */
                holcospktsetlimit_0_2__pktsetlimit = \
                    mmu_params.shared_xqs_per_downlink_port - \
                    mmu_params.skidmarker - \
                    mmu_params.prefetch + \
                    holcosminxqcnt_0_2__holcosminxqcnt;
                /* C105-C72-C74+I71 */
                holcospktsetlimit_3__pktsetlimit = \
                    mmu_params.shared_xqs_per_downlink_port - \
                    mmu_params.skidmarker - \
                    mmu_params.prefetch + \
                    holcosminxqcnt_3__holcosminxqcnt;
                holcospktsetlimit_4_7__pktsetlimit = 0;
            } else {
                /* c34 */
                holcosminxqcnt_0_2__holcosminxqcnt = \
                    mmu_params.\
                    egress_xq_min_reserve_lossy_uplink_and_downlink_ports;
                /* c34 */
                holcosminxqcnt_3__holcosminxqcnt = \
                    mmu_params.\
                    egress_xq_min_reserve_lossy_uplink_and_downlink_ports;
                holcosminxqcnt_4_7__holcosminxqcnt = 0;
                /* C105-C72-C74+C34 */
                holcospktsetlimit_0_2__pktsetlimit = \
                    mmu_params.shared_xqs_per_downlink_port - \
                    mmu_params.skidmarker - \
                    mmu_params.prefetch + \
                    holcosminxqcnt_0_2__holcosminxqcnt;
                /* C105-C72-C74+C34 */
                holcospktsetlimit_3__pktsetlimit = \
                    mmu_params.shared_xqs_per_downlink_port - \
                    mmu_params.skidmarker - \
                    mmu_params.prefetch + \
                    holcosminxqcnt_0_2__holcosminxqcnt;
                holcospktsetlimit_4_7__pktsetlimit = 0;
            }
            holcospktresetlimit_0_2__pktresetlimit = \
                holcospktsetlimit_0_2__pktsetlimit - 1;
            holcospktresetlimit_3__pktresetlimit = \
                holcospktsetlimit_3__pktsetlimit - 1;
            holcospktresetlimit_4_7__pktresetlimit = 0;
            /* c63 - 1 */
            cngcospktlimit0_0_7__cngpktsetlimit0 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1;
            /* c63 - 1 */
            cngcospktlimit1_0_7__cngpktsetlimit1 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1;
            /* c63 - 1 */
            cngportpktlimit0_cngportpktlimit0 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1;
            /* c63 - 1 */
            cngportpktlimit1_cngportpktlimit1 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1;
            /* c105-c72-c74 */
            dynxqcntport_dynxqcntport = \
                mmu_params.shared_xqs_per_downlink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch;
            /* I83-2 */
            dynresetlimport_dynresetlimport = \
                dynxqcntport_dynxqcntport - 2;
            if (lossless) {
                /* c32 */
                lwmcoscellsetlimit_0_2__cellsetlimit = \
                    mmu_params.egress_queue_min_reserve_downlink_ports_lossless;
                /* c32 */
                lwmcoscellsetlimit_3__cellsetlimit = \
                    mmu_params.egress_queue_min_reserve_downlink_ports_lossless;
                lwmcoscellsetlimit_4_7__cellsetlimit = 0;
                /* c32 */
                lwmcoscellsetlimit_0_2__cellresetlimit = \
                    mmu_params.egress_queue_min_reserve_downlink_ports_lossless;
                /* c32 */
                lwmcoscellsetlimit_3__cellresetlimit = \
                    mmu_params.egress_queue_min_reserve_downlink_ports_lossless;
                lwmcoscellsetlimit_4_7__cellresetlimit = 0;
                /* CEILING((C87+C85)*C54+(C88+C86)*C55+(C89+C85)*C53+(C90+C86)*C52+I85, 1) */
                tmp = (mmu_params.\
                       xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports +
                       mmu_params.headroom_for_1g_port) * \
                      mmu_params.num_1g_ports_downlink_ports + \
                      (mmu_params.\
                       xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports +
                       mmu_params.headroom_for_2p5g_port) * \
                      mmu_params.num_2p5g_ports_downlink_ports + \
                      (mmu_params.\
                       xoff_cell_threshold_1g_uplink_ports + \
                       mmu_params.headroom_for_1g_port) * \
                      mmu_params.num_2p5g_ports_uplink_ports + \
                      (mmu_params.xoff_cell_threshold_2p5g_uplink_ports + \
                       mmu_params.headroom_for_2p5g_port) * \
                      mmu_params.num_1g_ports_uplink_ports + \
                      lwmcoscellsetlimit_0_2__cellsetlimit;
                holcoscellmaxlimit_0_2__cellmaxlimit = _ceil_func(tmp, 1);
                /* CEILING((C87+C85)*C54+(C88+C86)*C55+(C89+C85)*C53+(C90+C86)*C52+I86, 1) */
                tmp = (mmu_params.\
                       xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports +
                       mmu_params.headroom_for_1g_port) * \
                      mmu_params.num_1g_ports_downlink_ports + \
                      (mmu_params.\
                       xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports +
                       mmu_params.headroom_for_2p5g_port) * \
                      mmu_params.num_2p5g_ports_downlink_ports + \
                      (mmu_params.\
                       xoff_cell_threshold_1g_uplink_ports + \
                       mmu_params.headroom_for_1g_port) * \
                      mmu_params.num_2p5g_ports_uplink_ports + \
                      (mmu_params.xoff_cell_threshold_2p5g_uplink_ports + \
                       mmu_params.headroom_for_2p5g_port) * \
                      mmu_params.num_1g_ports_uplink_ports + \
                      lwmcoscellsetlimit_3__cellsetlimit;
                holcoscellmaxlimit_3__cellmaxlimit = _ceil_func(tmp, 1);
                holcoscellmaxlimit_4_7__cellmaxlimit = 0;
            } else {
                /* c30 */
                lwmcoscellsetlimit_0_2__cellsetlimit = \
                    mmu_params.egress_queue_min_reserve_downlink_ports_lossy;
                /* c30 */
                lwmcoscellsetlimit_3__cellsetlimit = \
                    mmu_params.egress_queue_min_reserve_downlink_ports_lossy;
                lwmcoscellsetlimit_4_7__cellsetlimit = 0;
                /* c30 */
                lwmcoscellsetlimit_0_2__cellresetlimit = \
                    mmu_params.egress_queue_min_reserve_downlink_ports_lossy;
                /* c30 */
                lwmcoscellsetlimit_3__cellresetlimit = \
                    mmu_params.egress_queue_min_reserve_downlink_ports_lossy;
                lwmcoscellsetlimit_4_7__cellresetlimit = 0;
                /* CEILING(c101*C25+o85, 1) */
                tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
                holcoscellmaxlimit_0_2__cellmaxlimit = \
                    tmp + lwmcoscellsetlimit_0_2__cellsetlimit;
                /* CEILING(c101*C25+o87, 1) */
                tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
                holcoscellmaxlimit_3__cellmaxlimit = \
                    tmp + lwmcoscellsetlimit_3__cellsetlimit;
                holcoscellmaxlimit_4_7__cellmaxlimit = 0;
            }
            /* I91-C80 */
            holcoscellmaxlimit_0_2__cellmaxresumelimit = \
                holcoscellmaxlimit_0_2__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            /* I92-C80 */
            holcoscellmaxlimit_3__cellmaxresumelimit = \
                holcoscellmaxlimit_3__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            holcoscellmaxlimit_4_7__cellmaxresumelimit = 0;
            /* c101 */
            dyncelllimit_dyncellsetlimit = \
                mmu_params.shared_space_cells;
            /* o97-c80*2 */
            dyncelllimit_dyncellresetlimit = \
                dyncelllimit_dyncellsetlimit - \
                (mmu_params.ethernet_mtu * 2);
            color_drop_en_color_drop_en = 0;
            shared_pool_ctrl_dynamic_cos_drop_en = 255;
            shared_pool_ctrl_shared_pool_discard_en = 255;
            shared_pool_ctrl_sharded_pool_xoff_en = 0;
        }

        /* Program the setting */
        val = 0;
        /* i52 = c92 */
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &val, PKTSETLIMITf,
                          ibppktsetlimit_pktsetlimit);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &val, RESETLIMITSELf,
                          ibppktsetlimit_resetlimitsel);
        SOC_IF_ERROR_RETURN(WRITE_IBPPKTSETLIMITr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &val, MMU_FC_RX_ENABLEf,
                          mmu_fc_rx_en_mmu_fc_rx_enable);
        SOC_IF_ERROR_RETURN(WRITE_MMU_FC_RX_ENr(unit, port, val));
        val = 0;
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &val, MMU_FC_TX_ENABLEf,
                          mmu_fc_tx_en_mmu_fc_tx_enable);
        SOC_IF_ERROR_RETURN(WRITE_MMU_FC_TX_ENr(unit, port, val));

        for (cos = 0; cos < WH2_QUEUE_MAX_NUM; cos++) {
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcospktsetlimit_0_2__pktsetlimit;
            } else if (cos == 3) {
                fld_val = holcospktsetlimit_3__pktsetlimit;
            } else {
                fld_val = holcospktsetlimit_4_7__pktsetlimit;
            }
            SOC_IF_ERROR_RETURN
                (READ_HOLCOSPKTSETLIMITr(unit, port, cos, &val));
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &val,
                              PKTSETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSPKTSETLIMITr(unit, port, cos, val));

            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcospktresetlimit_0_2__pktresetlimit;
            } else if (cos == 3) {
                fld_val = holcospktresetlimit_3__pktresetlimit;
            } else {
                fld_val = holcospktresetlimit_4_7__pktresetlimit;
            }
            SOC_IF_ERROR_RETURN
                (READ_HOLCOSPKTRESETLIMITr(unit, port, cos, &val));
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &val,
                              PKTRESETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSPKTRESETLIMITr(unit, port, cos, val));

            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcosminxqcnt_0_2__holcosminxqcnt;
            } else if (cos == 3) {
                fld_val = holcosminxqcnt_3__holcosminxqcnt;
            } else {
                fld_val = holcosminxqcnt_4_7__holcosminxqcnt;
            }
            val = 0;
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &val,
                              HOLCOSMINXQCNTf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSMINXQCNTr(unit, port, cos, val));

            val = 0;
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = lwmcoscellsetlimit_0_2__cellsetlimit;
            } else if (cos == 3) {
                fld_val = lwmcoscellsetlimit_3__cellsetlimit;
            } else {
                fld_val = lwmcoscellsetlimit_4_7__cellsetlimit;
            }
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &val,
                              CELLSETLIMITf, fld_val);
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = lwmcoscellsetlimit_0_2__cellresetlimit;
            } else if (cos == 3) {
                fld_val = lwmcoscellsetlimit_3__cellresetlimit;
            } else {
                fld_val = lwmcoscellsetlimit_4_7__cellresetlimit;
            }
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &val,
                              CELLRESETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_LWMCOSCELLSETLIMITr(unit, port, cos, val));

            val = 0;
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcoscellmaxlimit_0_2__cellmaxlimit;
            } else if (cos == 3) {
                fld_val = holcoscellmaxlimit_3__cellmaxlimit;
            } else {
                fld_val = holcoscellmaxlimit_4_7__cellmaxlimit;
            }
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &val,
                              CELLMAXLIMITf, fld_val);
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcoscellmaxlimit_0_2__cellmaxresumelimit;
            } else if (cos == 3) {
                fld_val = holcoscellmaxlimit_3__cellmaxresumelimit;
            } else {
                fld_val = holcoscellmaxlimit_4_7__cellmaxresumelimit;
            }
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &val,
                              CELLMAXRESUMELIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSCELLMAXLIMITr(unit, port, cos, val));

            val = 0;
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = pgcelllimit_0_2__cellsetlimit;
            } else if (cos == 3) {
                fld_val = pgcelllimit_3__cellsetlimit;
            } else if ((cos >= 4) && (cos <= 6)) {
                fld_val = pgcelllimit_4_6__cellsetlimit;
            } else {
                fld_val = pgcelllimit_7__cellsetlimit;
            }
            soc_reg_field_set(unit, PGCELLLIMITr, &val,
                              CELLSETLIMITf, fld_val);
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = pgcelllimit_0_2__cellresetlimit;
            } else if (cos == 3) {
                fld_val = pgcelllimit_3__cellresetlimit;
            } else if ((cos >= 4) && (cos <= 6)) {
                fld_val = pgcelllimit_4_6__cellresetlimit;
            } else {
                fld_val = pgcelllimit_7__cellresetlimit;
            }
            soc_reg_field_set(unit, PGCELLLIMITr, &val,
                              CELLRESETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_PGCELLLIMITr(unit, port, cos, val));

            val = 0;
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = pgdiscardsetlimit_0_2__discardsetlimit;
            } else if (cos == 3) {
                fld_val = pgdiscardsetlimit_3__discardsetlimit;
            } else if ((cos >= 4) && (cos <= 6)) {
                fld_val = pgdiscardsetlimit_4_6__discardsetlimit;
            } else {
                fld_val = pgdiscardsetlimit_7__discardsetlimit;
            }
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &val,
                              DISCARDSETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_PGDISCARDSETLIMITr(unit, port, cos, val));

            val = 0;
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &val,
                              CNGPKTSETLIMIT0f,
                              cngcospktlimit0_0_7__cngpktsetlimit0);
            SOC_IF_ERROR_RETURN
                (WRITE_CNGCOSPKTLIMIT0r(unit, port, cos, val));
            val = 0;
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &val,
                              CNGPKTSETLIMIT1f,
                              cngcospktlimit1_0_7__cngpktsetlimit1);
            SOC_IF_ERROR_RETURN
                (WRITE_CNGCOSPKTLIMIT1r(unit, port, cos, val));
        }

        val = 0;
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &val,
                          CNGPORTPKTLIMIT0f,
                          cngportpktlimit0_cngportpktlimit0);
        SOC_IF_ERROR_RETURN(WRITE_CNGPORTPKTLIMIT0r(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &val,
                          CNGPORTPKTLIMIT1f,
                          cngportpktlimit1_cngportpktlimit1);
        SOC_IF_ERROR_RETURN(WRITE_CNGPORTPKTLIMIT1r(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &val,
                          DYNXQCNTPORTf,
                          dynxqcntport_dynxqcntport);
        SOC_IF_ERROR_RETURN(WRITE_DYNXQCNTPORTr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &val,
                          DYNRESETLIMPORTf,
                          dynresetlimport_dynresetlimport);
        SOC_IF_ERROR_RETURN(WRITE_DYNRESETLIMPORTr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                          DYNCELLSETLIMITf,
                          dyncelllimit_dyncellsetlimit);
        soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                          DYNCELLRESETLIMITf,
                          dyncelllimit_dyncellresetlimit);
        SOC_IF_ERROR_RETURN(WRITE_DYNCELLLIMITr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, COLOR_DROP_ENr, &val,
                          COLOR_DROP_ENf,
                          color_drop_en_color_drop_en);
        SOC_IF_ERROR_RETURN(WRITE_COLOR_DROP_ENr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, SHARED_POOL_CTRLr, &val,
                          DYNAMIC_COS_DROP_ENf,
                          shared_pool_ctrl_dynamic_cos_drop_en);
        soc_reg_field_set(unit, SHARED_POOL_CTRLr, &val,
                          SHARED_POOL_DISCARD_ENf,
                          shared_pool_ctrl_shared_pool_discard_en);
        soc_reg_field_set(unit, SHARED_POOL_CTRLr, &val,
                          SHARED_POOL_XOFF_ENf,
                          shared_pool_ctrl_sharded_pool_xoff_en);
        SOC_IF_ERROR_RETURN(WRITE_SHARED_POOL_CTRLr(unit, port, val));
   }

   return SOC_E_NONE;
}

STATIC int
soc_wolfhound2_mmu_lossy_or_lossless_init_helper(int unit, int lossless)
{
    uint32 tmp;
    wh2_sku_info_t *matched_port_config = NULL;
    int option;
    int downlink_port_1g;
    int downlink_port_2p5g;
    int uplink_port_1g;
    int uplink_port_2p5g;

    matched_port_config = &wh2_sku_port_config[matched_devid_idx[unit]];
    option = matched_port_config->config_option;
    /* MMU settings */
    switch (option) {
        case 1:
            downlink_port_1g = 20;
            downlink_port_2p5g = 4;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 2:
        case 3:
            downlink_port_1g = 24;
            downlink_port_2p5g = 0;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 4:
        case 5:
        case 6:
            downlink_port_1g = 24;
            downlink_port_2p5g = 0;
            uplink_port_1g = 4;
            uplink_port_2p5g = 0;
            break;
        case 7:
            downlink_port_1g = 12;
            downlink_port_2p5g = 4;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 8:
        case 9:
            downlink_port_1g = 16;
            downlink_port_2p5g = 0;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 10:
        case 11:
        case 12:
            downlink_port_1g = 16;
            downlink_port_2p5g = 0;
            uplink_port_1g = 4;
            uplink_port_2p5g = 0;
            break;
        case 13:
            downlink_port_1g = 4;
            downlink_port_2p5g = 4;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 14:
        case 15:
            downlink_port_1g = 8;
            downlink_port_2p5g = 0;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 16:
        case 17:
        case 18:
            downlink_port_1g = 8;
            downlink_port_2p5g = 0;
            uplink_port_1g = 4;
            uplink_port_2p5g = 0;
            break;
        default:
            /* Default matched wh2_sku_port_config[0] */
            downlink_port_1g = 20;
            downlink_port_2p5g = 4;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
    }

    /* Prepare MMU settings */
    /*
     * Traffic Attributes
     */
    /* c7 */
    mmu_params.min_packet_size_traffic_attr = 64;
    /*
     * c8
     * the value is 1.5 in doc, save 15 instead in sw.
     * any later usage should take care of this.
     */
    mmu_params.ethernet_mtu_traffic_attr = 15;
    /* c9 */
    mmu_params.max_packet_size_traffic_attr = 12288;
    /* c10 */
    mmu_params.standard_jumbo_frame_traffic_attr = 9216;
    /* c11 */
    mmu_params.lossless_traffic_mtu_size_traffic_attr = 2048;

    /*
     * General Information
     * moved to earlier place as these are needed by others
     */
    /* c79 = CEILING(C7/c49, 1)*/
    tmp = mmu_params.min_packet_size_traffic_attr / CELL_SIZE;
    mmu_params.min_packet_size = _ceil_func(tmp, 1);
    /* c80 = CEILING(C8*1024/c49, 1)*/
    tmp = (mmu_params.ethernet_mtu_traffic_attr * 1024) / CELL_SIZE;
    tmp /= 10; /* The value is 1.5 in MMU setting data, save as 15 in SW */
    mmu_params.ethernet_mtu = _ceil_func(tmp, 1);
    /* c81 = CEILING(C9/c49, 1)*/
    tmp = mmu_params.max_packet_size_traffic_attr / CELL_SIZE;
    mmu_params.max_packet_size = _ceil_func(tmp, 1);
    /* c82 = CEILING(C10/c49, 1)*/
    tmp = mmu_params.standard_jumbo_frame_traffic_attr / CELL_SIZE;
    mmu_params.standard_jumbo_frame = _ceil_func(tmp, 1);
    /* c83 = CEILING(C11/c49, 1)*/
    tmp = mmu_params.lossless_traffic_mtu_size_traffic_attr / CELL_SIZE;
    mmu_params.lossless_traffic_mtu_size = _ceil_func(tmp, 1);

    /*
     * Reservations
     * moved to earlier place as these are needed by others
     */
    /* c70 = c80*4*/
    mmu_params.wrr_reserve = mmu_params.ethernet_mtu * 4;
    /* c71 = (31)*2 +7+29*/
    mmu_params.reserved_for_cfap = 98;
    /* c72 */
    mmu_params.skidmarker = 7;
    /* c73 */
    mmu_params.skidmarker_option = 3;
    /* c74 */
    mmu_params.prefetch = 9;

    /*
     * SW Config Property
     */
    /* c14 */
    mmu_params.total_physical_memory_sw_cfg = 4096;
    /* c15 = c14-c71 */
    mmu_params.cfapfullsetpoint_sw_cfg = \
        mmu_params.total_physical_memory_sw_cfg - mmu_params.reserved_for_cfap;
    /* c16 = c15 */
    mmu_params.total_cell_memory_for_admission_sw_cfg = \
        mmu_params.cfapfullsetpoint_sw_cfg;

    /* c17 configurable */
    mmu_params.num_1g_ports_uplink_ports_sw_cfg = uplink_port_1g;
    /* c18 configurable */
    mmu_params.num_2p5g_ports_uplink_ports_sw_cfg = uplink_port_2p5g;
    /* c19 configurable */
    mmu_params.num_1g_ports_downlink_ports_sw_cfg = downlink_port_1g;
    /* c20 configurable */
    mmu_params.num_2p5g_ports_downlink_ports_sw_cfg = downlink_port_2p5g;
    /* c21 = SUM(C17:C18) */
    mmu_params.number_of_uplink_ports_sw_cfg = \
        mmu_params.num_1g_ports_uplink_ports_sw_cfg + \
        mmu_params.num_2p5g_ports_uplink_ports_sw_cfg;
    /* c22 = SUM(C19:C20)*/
    mmu_params.number_of_downlink_ports_sw_cfg = \
        mmu_params.num_1g_ports_downlink_ports_sw_cfg + \
        mmu_params.num_2p5g_ports_downlink_ports_sw_cfg;
    /* c23 lossless */
    if (lossless) {
        mmu_params.flow_control_enabled_sw_cfg = 1;
    } else {
        mmu_params.flow_control_enabled_sw_cfg = 0;
    }
    /* c24 PFC = 0 */
    mmu_params.flow_control_type_sw_cfg = 0;
    /* c25 = 0.25, not used */
    /* mmu_params.queue_port_limit_ratio_sw_cfg = 0.25; */

    /*
     * Device Attributes
     * moved to earlier place as these are needed by others
     */
    /* c49 */
    mmu_params.cell_size = CELL_SIZE;
    /* c50 */
    mmu_params.num_egress_queues_per_port = 4;
    /* c51 */
    mmu_params.num_cpu_queues = 8;
    /* c52 = c17*/
    mmu_params.num_1g_ports_uplink_ports = \
        mmu_params.num_1g_ports_uplink_ports_sw_cfg;
    /* c53 = c18 */
    mmu_params.num_2p5g_ports_uplink_ports = \
        mmu_params.num_2p5g_ports_uplink_ports_sw_cfg;
    /* c54 = c19 */
    mmu_params.num_1g_ports_downlink_ports = \
        mmu_params.num_1g_ports_downlink_ports_sw_cfg;
    /* c55 = c20 */
    mmu_params.num_2p5g_ports_downlink_ports = \
        mmu_params.num_2p5g_ports_downlink_ports_sw_cfg;
    /* c56 */
    mmu_params.num_cpu_ports = 1;
    /* c57 = c21*/
    mmu_params.number_of_uplink_ports = \
        mmu_params.number_of_uplink_ports_sw_cfg;
    /* c58 = c22 */
    mmu_params.number_of_downlink_ports = \
        mmu_params.number_of_downlink_ports_sw_cfg;
    /* c59 = c14 */
    mmu_params.total_physical_memory = \
        mmu_params.total_physical_memory_sw_cfg;
    /* c60 = c16 */
    mmu_params.total_cell_memory = \
        mmu_params.total_cell_memory_for_admission_sw_cfg;
    /* c61 = c60 */
    mmu_params.total_advertised_cell_memory = \
        mmu_params.total_cell_memory;
    /* c62 */
    mmu_params.numxq_per_uplink_ports = 512;
    /* c63 */
    mmu_params.numxq_per_downlink_ports_and_cpu_port = 512;
    /* c64 */
    mmu_params.the_number_of_pipelines_per_device = 1;
    /* c66 */
    mmu_params.n_egress_queues_active_per_xpe = 1;
    /* c67 */
    mmu_params.n_ingress_ports_active_per_xpe = 1;

    /*
     * SW Config Parameter : may different from lossy to lossless
     */
    if (lossless) {
        /* c29 */
        mmu_params.egress_queue_min_reserve_uplink_ports_lossy = 0;
        /* c30 */
        mmu_params.egress_queue_min_reserve_downlink_ports_lossy = 0;
        /* c31 */
        mmu_params.egress_queue_min_reserve_uplink_ports_lossless = 12;
        /* c32 */
        mmu_params.egress_queue_min_reserve_downlink_ports_lossless = 12;
    } else {
        /* c29 = c80 */
        mmu_params.egress_queue_min_reserve_uplink_ports_lossy = \
            mmu_params.ethernet_mtu;
        /* c30 = c80 */
        mmu_params.egress_queue_min_reserve_downlink_ports_lossy = \
            mmu_params.ethernet_mtu;
        /* c31 */
        mmu_params.egress_queue_min_reserve_uplink_ports_lossless = 0;
        /* c32 */
        mmu_params.egress_queue_min_reserve_downlink_ports_lossless = 0;
    }
    /* c33 = c80 */
    mmu_params.egress_queue_min_reserve_cpu_ports = \
        mmu_params.ethernet_mtu;
    if (lossless) {
        /* c34 */
        mmu_params.egress_xq_min_reserve_lossy_uplink_and_downlink_ports = 0;
        /* c35 */
        mmu_params.egress_xq_min_reserve_lossless_uplink_ports = 12;
        /* c36 */
        mmu_params.egress_xq_min_reserve_lossless_downlink_ports = 12;
    } else {
        /* c34 = c80 */
        mmu_params.egress_xq_min_reserve_lossy_uplink_and_downlink_ports = \
            mmu_params.ethernet_mtu;
        /* c35 */
        mmu_params.egress_xq_min_reserve_lossless_uplink_ports = 0;
        /* c36 */
        mmu_params.egress_xq_min_reserve_lossless_downlink_ports = 0;
    }
    /* c37 */
    mmu_params.num_total_pri_groups = 4;
    if (lossless) {
        /* c38 */
        mmu_params.num_active_pri_group_lossless = 1;
        /* c39 */
        mmu_params.num_lossless_queues = 4;
        /* c40 */
        mmu_params.num_lossy_queues = 0;
        /* c41 = c61 */
        mmu_params.mmu_xoff_pkt_threshold_uplink_ports = 12;
        /* c42 = c61 */
        mmu_params.mmu_xoff_pkt_threshold_downlink_ports = 12;
        /* c43 = c61 */
        mmu_params.mmu_xoff_cell_threshold_1g_port_uplink_ports = 36;
        /* c44 = c61 */
        mmu_params.mmu_xoff_cell_threshold_2p5g_port_uplink_ports = 36;
        /* c45 = c61 */
        mmu_params.mmu_xoff_cell_threshold_1g_downlink_ports = 36;
        /* c46 = c61 */
        mmu_params.mmu_xoff_cell_threshold_2p5g_downlink_ports = 36;
    } else {
        /* c38 */
        mmu_params.num_active_pri_group_lossless = 0;
        /* c39 */
        mmu_params.num_lossless_queues = 0;
        /* c40 */
        mmu_params.num_lossy_queues = 4;
        /* c41 = c61 */
        mmu_params.mmu_xoff_pkt_threshold_uplink_ports = \
            mmu_params.total_advertised_cell_memory;
        /* c42 = c61 */
        mmu_params.mmu_xoff_pkt_threshold_downlink_ports = \
            mmu_params.total_advertised_cell_memory;
        /* c43 = c61 */
        mmu_params.mmu_xoff_cell_threshold_1g_port_uplink_ports = \
            mmu_params.total_advertised_cell_memory;
        /* c44 = c61 */
        mmu_params.mmu_xoff_cell_threshold_2p5g_port_uplink_ports = \
            mmu_params.total_advertised_cell_memory;
        /* c45 = c61 */
        mmu_params.mmu_xoff_cell_threshold_1g_downlink_ports = \
            mmu_params.total_advertised_cell_memory;
        /* c46 = c61 */
        mmu_params.mmu_xoff_cell_threshold_2p5g_downlink_ports = \
            mmu_params.total_advertised_cell_memory;
    }

    /*
     * Ingress Thresholds : may different from lossy to lossless
     */
    if (lossless) {
        /* c85 */
        mmu_params.headroom_for_1g_port = 36;
        /* c86 */
        mmu_params.headroom_for_2p5g_port = 36;
    } else {
        /* c85 */
        mmu_params.headroom_for_1g_port = 0;
        /* c86 */
        mmu_params.headroom_for_2p5g_port = 0;
    }
    /* c87 = c45 */
    mmu_params.xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports = \
        mmu_params.mmu_xoff_cell_threshold_1g_downlink_ports;
    /* c88 = c46 */
    mmu_params.xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports = \
        mmu_params.mmu_xoff_cell_threshold_2p5g_downlink_ports;
    /* c89 = c43 */
    mmu_params.xoff_cell_threshold_1g_uplink_ports = \
        mmu_params.mmu_xoff_cell_threshold_1g_port_uplink_ports;
    /* c90 = c44 */
    mmu_params.xoff_cell_threshold_2p5g_uplink_ports = \
        mmu_params.mmu_xoff_cell_threshold_2p5g_port_uplink_ports;
    /* c91 = c41 */
    mmu_params.xoff_packet_thresholds_per_port_uplink_port = \
        mmu_params.mmu_xoff_pkt_threshold_uplink_ports;
    /* c92 = c42 */
    mmu_params.xoff_packet_thresholds_per_port_downlink_port = \
        mmu_params.mmu_xoff_pkt_threshold_downlink_ports;
    if (lossless) {
        /* c93 = c61 */
        mmu_params.discard_limit_per_port_pg_downlink_1g_port = \
        mmu_params.total_advertised_cell_memory;
        /* c94 = c61 */
        mmu_params.discard_limit_per_port_pg_downlink_2p5g_port = \
        mmu_params.total_advertised_cell_memory;
    } else {
        /* c93 = c87+c85 */
        mmu_params.discard_limit_per_port_pg_downlink_1g_port = \
            mmu_params.xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports + \
            mmu_params.headroom_for_1g_port;
        /* c94 = c88+c86 */
        mmu_params.discard_limit_per_port_pg_downlink_2p5g_port = \
            mmu_params.xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports + \
            mmu_params.headroom_for_lossless_downlink_port;
    }
    /* c95 = c61 */
    mmu_params.discard_limit_per_port_pg_uplink_port = \
        mmu_params.total_advertised_cell_memory;

    /*
     * Egress Thresholds
     */
    /* c97 = c29*c57*c40+c57*c31*c39 */
    mmu_params.total_reserved_cells_for_uplink_ports = \
        (mmu_params.egress_queue_min_reserve_uplink_ports_lossy * \
         mmu_params.number_of_uplink_ports * \
         mmu_params.num_lossy_queues) + \
        (mmu_params.number_of_uplink_ports * \
         mmu_params.egress_queue_min_reserve_uplink_ports_lossless * \
         mmu_params.num_lossless_queues);
    /* c98 = c58*c30*(c40)+c58*c32*c39 */
    mmu_params.total_reserved_cells_for_downlink_ports = \
        (mmu_params.number_of_downlink_ports * \
         mmu_params.egress_queue_min_reserve_downlink_ports_lossy * \
         mmu_params.num_lossy_queues) + \
        (mmu_params.number_of_downlink_ports * \
         mmu_params.egress_queue_min_reserve_downlink_ports_lossless * \
         mmu_params.num_lossless_queues);
    /* c99 = c56*c33*c51 */
    mmu_params.total_reserved_cells_for_cpu_port = \
        mmu_params.num_cpu_ports * \
        mmu_params.egress_queue_min_reserve_cpu_ports * \
        mmu_params.num_cpu_queues;
    /* c100 = SUM(c97:c99) */
    mmu_params.total_reserved = \
        mmu_params.total_reserved_cells_for_uplink_ports + \
        mmu_params.total_reserved_cells_for_downlink_ports + \
        mmu_params.total_reserved_cells_for_cpu_port;
    /* c101 = c61-c100 */
    mmu_params.shared_space_cells = \
        mmu_params.total_advertised_cell_memory - \
        mmu_params.total_reserved;
    /* c102 = c34*c40+c35*c39 */
    mmu_params.reserved_xqs_per_uplink_port = \
        (mmu_params.egress_xq_min_reserve_lossy_uplink_and_downlink_ports * \
         mmu_params.num_lossy_queues) + \
        (mmu_params.egress_xq_min_reserve_lossless_uplink_ports * \
         mmu_params.num_lossless_queues);
    /* c103 = c62-c102 */
    mmu_params.shared_xqs_per_uplink_port = \
        mmu_params.numxq_per_uplink_ports - \
        mmu_params.reserved_xqs_per_uplink_port;
    /* c104 = c34*c40+c36*c39 */
    mmu_params.reserved_xqs_per_downlink_port = \
        (mmu_params.egress_xq_min_reserve_lossy_uplink_and_downlink_ports * \
         mmu_params.num_lossy_queues) + \
        (mmu_params.egress_xq_min_reserve_lossless_downlink_ports * \
         mmu_params.num_lossless_queues);
    /* c105 = c63-c104 */
    mmu_params.shared_xqs_per_downlink_port = \
        mmu_params.numxq_per_downlink_ports_and_cpu_port - \
        mmu_params.reserved_xqs_per_downlink_port;
    /* c106 = c33 * c51 */
    mmu_params.reserved_xqs_at_cpu_port = \
        mmu_params.egress_queue_min_reserve_cpu_ports * \
        mmu_params.num_cpu_queues;
    /* c107 = c63 - c106 */
    mmu_params.shared_xqs_at_cpu_port = \
        mmu_params.numxq_per_downlink_ports_and_cpu_port - \
        mmu_params.reserved_xqs_at_cpu_port;

    /* Program MMU settings */
    SOC_IF_ERROR_RETURN(
        soc_wolfhound2_mmu_lossy_or_lossless_config_helper(unit, lossless));

    return SOC_E_NONE;
}

STATIC int
soc_wolfhound2_mmu_8_cosq_config_helper(int unit, int lossless)
{
    uint32 val = 0;
    uint32 fld_val = 0;
    uint32 tmp = 0;
    int port; /* logical port id */
    int pport; /* physical port id */
    int speed;
    int cos;
    int i;
    uint32 ibppktsetlimit_pktsetlimit;
    uint32 ibppktsetlimit_resetlimitsel;
    uint32 mmu_fc_rx_en_mmu_fc_rx_enable;
    uint32 mmu_fc_tx_en_mmu_fc_tx_enable;
    uint32 pgcelllimit_0_2__cellsetlimit;
    uint32 pgcelllimit_3__cellsetlimit;
    uint32 pgcelllimit_4_6__cellsetlimit;
    uint32 pgcelllimit_7__cellsetlimit;
    uint32 pgcelllimit_0_2__cellresetlimit;
    uint32 pgcelllimit_3__cellresetlimit;
    uint32 pgcelllimit_4_6__cellresetlimit;
    uint32 pgcelllimit_7__cellresetlimit;
    uint32 pgdiscardsetlimit_0_2__discardsetlimit;
    uint32 pgdiscardsetlimit_3__discardsetlimit;
    uint32 pgdiscardsetlimit_4_6__discardsetlimit;
    uint32 pgdiscardsetlimit_7__discardsetlimit;
    uint32 holcosminxqcnt_0_2__holcosminxqcnt;
    uint32 holcosminxqcnt_3__holcosminxqcnt;
    uint32 holcosminxqcnt_4_7__holcosminxqcnt;
    uint32 holcospktsetlimit_0_2__pktsetlimit;
    uint32 holcospktsetlimit_3__pktsetlimit;
    uint32 holcospktsetlimit_4_7__pktsetlimit;
    uint32 holcospktresetlimit_0_2__pktresetlimit;
    uint32 holcospktresetlimit_3__pktresetlimit;
    uint32 holcospktresetlimit_4_7__pktresetlimit;
    uint32 cngcospktlimit0_0_7__cngpktsetlimit0;
    uint32 cngcospktlimit1_0_7__cngpktsetlimit1;
    uint32 cngportpktlimit0_cngportpktlimit0;
    uint32 cngportpktlimit1_cngportpktlimit1;
    uint32 dynxqcntport_dynxqcntport;
    uint32 dynresetlimport_dynresetlimport;
    uint32 lwmcoscellsetlimit_0_2__cellsetlimit;
    uint32 lwmcoscellsetlimit_3__cellsetlimit;
    uint32 lwmcoscellsetlimit_4_7__cellsetlimit;
    uint32 lwmcoscellsetlimit_0_2__cellresetlimit;
    uint32 lwmcoscellsetlimit_3__cellresetlimit;
    uint32 lwmcoscellsetlimit_4_7__cellresetlimit;
    uint32 holcoscellmaxlimit_0_2__cellmaxlimit;
    uint32 holcoscellmaxlimit_3__cellmaxlimit;
    uint32 holcoscellmaxlimit_4_7__cellmaxlimit;
    uint32 holcoscellmaxlimit_0_2__cellmaxresumelimit;
    uint32 holcoscellmaxlimit_3__cellmaxresumelimit;
    uint32 holcoscellmaxlimit_4_7__cellmaxresumelimit;
    uint32 dyncelllimit_dyncellsetlimit;
    uint32 dyncelllimit_dyncellresetlimit;
    uint32 color_drop_en_color_drop_en;
    uint32 shared_pool_ctrl_dynamic_cos_drop_en;
    uint32 shared_pool_ctrl_shared_pool_discard_en;
    uint32 shared_pool_ctrl_sharded_pool_xoff_en;

    SOC_IF_ERROR_RETURN(READ_CFAPFULLTHRESHOLDr(unit, &val));
    /* i6 = c15 */
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDr, &val,
                      CFAPFULLSETPOINTf,
                      mmu_params.cfapfullsetpoint_sw_cfg);
    /* i7 = i6 - c82 */
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDr, &val,
                      CFAPFULLRESETPOINTf,
                      mmu_params.cfapfullsetpoint_sw_cfg - \
                      mmu_params.standard_jumbo_frame);
    SOC_IF_ERROR_RETURN(WRITE_CFAPFULLTHRESHOLDr(unit, val));

    /* i10 = c16 */
    SOC_IF_ERROR_RETURN(READ_GBLLIMITSETLIMITr(unit, &val));
    soc_reg_field_set(unit, GBLLIMITSETLIMITr, &val,
                      GBLCELLSETLIMITf,
                      mmu_params.total_cell_memory_for_admission_sw_cfg);
    SOC_IF_ERROR_RETURN(WRITE_GBLLIMITSETLIMITr(unit, val));

    /* i11 = i10 */
    SOC_IF_ERROR_RETURN(READ_GBLLIMITRESETLIMITr(unit, &val));
    soc_reg_field_set(unit, GBLLIMITRESETLIMITr, &val,
                      GBLCELLRESETLIMITf,
                      mmu_params.total_cell_memory_for_admission_sw_cfg);
    SOC_IF_ERROR_RETURN(WRITE_GBLLIMITRESETLIMITr(unit, val));

    /* Device Wide - Egress Thresholds */
    val = 0;
    /* i15 = c101 */
    soc_reg_field_set(unit, TOTALDYNCELLSETLIMITr,
                      &val, TOTALDYNCELLSETLIMITf,
                      mmu_params.shared_space_cells);
    SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLSETLIMITr(unit, val));
    val = 0;
    /* i16 =i15-c82*2*/
    soc_reg_field_set(unit, TOTALDYNCELLRESETLIMITr,
                      &val, TOTALDYNCELLRESETLIMITf,
                      mmu_params.shared_space_cells - \
                      (mmu_params.standard_jumbo_frame * 2));
    SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLRESETLIMITr(unit, val));

    /* MICCONFIGr */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    soc_reg_field_set(unit, MISCCONFIGr, &val, MULTIPLE_ACCOUNTING_FIX_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &val, CNG_DROP_ENf, 0);
    soc_reg_field_set(unit, MISCCONFIGr, &val, DYN_XQ_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &val, HOL_CELL_SOP_DROP_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &val, DYNAMIC_MEMORY_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &val, SKIDMARKERf, 3);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));

    PBMP_ALL_ITER(unit, port) {
        pport = SOC_INFO(unit).port_l2p_mapping[port];
        speed = SOC_INFO(unit).port_speed_max[port];

        val = 0;
        if (IS_CPU_PORT(unit, port)) {
            soc_reg_field_set(unit, PG_CTRL0r, &val, PPFC_PG_ENf, 0);
        } else {
            if (lossless) {
                soc_reg_field_set(unit, PG_CTRL0r, &val, PPFC_PG_ENf, 0x80);
            } else {
                soc_reg_field_set(unit, PG_CTRL0r, &val, PPFC_PG_ENf, 0);
            }
        }
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI0_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI1_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI2_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI3_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI4_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI5_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI6_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &val, PRI7_GRPf, 7);
        SOC_IF_ERROR_RETURN(WRITE_PG_CTRL0r(unit, port, val));
        val = 0;
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI8_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI9_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI10_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI11_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI12_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI13_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI14_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &val, PRI15_GRPf, 7);
        SOC_IF_ERROR_RETURN(WRITE_PG_CTRL1r(unit, port, val));
        val = 0;
        soc_reg_field_set(unit, PG2TCr, &val, PG_BMPf, 0);
        for (i = 0; i < 8; i++) {
            SOC_IF_ERROR_RETURN(WRITE_PG2TCr(unit, port, i, val));
        }

        /* Settings based on port type */
        if (IS_CPU_PORT(unit, port)) {
            ibppktsetlimit_pktsetlimit = \
                mmu_params.\
                xoff_packet_thresholds_per_port_downlink_port; /* c92 */
            ibppktsetlimit_resetlimitsel = 3;
            mmu_fc_rx_en_mmu_fc_rx_enable = 0;
            mmu_fc_tx_en_mmu_fc_tx_enable = 0;
            /* c93 */
            pgcelllimit_0_2__cellsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port;
            /* c93 */
            pgcelllimit_3__cellsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port;
            /* c93 */
            pgcelllimit_4_6__cellsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port;
            /* c87 */
            pgcelllimit_7__cellsetlimit = \
                mmu_params.\
                xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
            /* c93 */
            pgcelllimit_0_2__cellresetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port;
            /* c93 */
            pgcelllimit_3__cellresetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port;
            /* c93 */
            pgcelllimit_4_6__cellresetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port;
            /* c87 */
            pgcelllimit_7__cellresetlimit = \
                mmu_params.\
                xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
            pgdiscardsetlimit_0_2__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port; /* c93 */
            pgdiscardsetlimit_3__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port; /* c93 */
            pgdiscardsetlimit_4_6__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port; /* c93 */
            pgdiscardsetlimit_7__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port; /* c93 */

            holcosminxqcnt_0_2__holcosminxqcnt = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            holcosminxqcnt_3__holcosminxqcnt = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            holcosminxqcnt_4_7__holcosminxqcnt = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            holcospktsetlimit_0_2__pktsetlimit = \
                mmu_params.shared_xqs_at_cpu_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_0_2__holcosminxqcnt; /* C107-C72-C74+Q70 */
            holcospktsetlimit_3__pktsetlimit = \
                mmu_params.shared_xqs_at_cpu_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_3__holcosminxqcnt; /* C107-C72-C74+Q71 */
            holcospktsetlimit_4_7__pktsetlimit = \
                mmu_params.shared_xqs_at_cpu_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_4_7__holcosminxqcnt; /* C107-C72-C74+Q72 */
            holcospktresetlimit_0_2__pktresetlimit = \
                holcospktsetlimit_0_2__pktsetlimit - 1; /* q73 - 1 */
            holcospktresetlimit_3__pktresetlimit = \
                holcospktsetlimit_3__pktsetlimit - 1; /* q74 - 1 */
            holcospktresetlimit_4_7__pktresetlimit = \
                holcospktsetlimit_4_7__pktsetlimit - 1; /* q75 - 1 */
            cngcospktlimit0_0_7__cngpktsetlimit0 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1; /*c63-1*/
            cngcospktlimit1_0_7__cngpktsetlimit1 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1; /*c63-1*/
            cngportpktlimit0_cngportpktlimit0 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1; /*c63-1*/
            cngportpktlimit1_cngportpktlimit1 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1; /*c63-1*/
            dynxqcntport_dynxqcntport = \
                mmu_params.shared_xqs_at_cpu_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch; /* c107-c72-c74 */
            dynresetlimport_dynresetlimport = \
                dynxqcntport_dynxqcntport - 2; /* q83 - 2 */
            lwmcoscellsetlimit_0_2__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_3__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_4_7__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_0_2__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_3__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            lwmcoscellsetlimit_4_7__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_cpu_ports; /* c33 */
            /* CEILING(c101*C25+Q85, 1) */
            tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
            holcoscellmaxlimit_0_2__cellmaxlimit = \
                tmp + lwmcoscellsetlimit_0_2__cellsetlimit;
            /* CEILING(c101*C25+Q86, 1) */
            tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
            holcoscellmaxlimit_3__cellmaxlimit = \
                tmp + lwmcoscellsetlimit_3__cellsetlimit;
            /* CEILING(c101*C25+Q87, 1) */
            tmp = _ceil_func((mmu_params.shared_space_cells * 10 / 4), 10);
            holcoscellmaxlimit_4_7__cellmaxlimit = \
                tmp + lwmcoscellsetlimit_4_7__cellsetlimit;
            holcoscellmaxlimit_0_2__cellmaxresumelimit = \
                holcoscellmaxlimit_0_2__cellmaxlimit - \
                mmu_params.ethernet_mtu; /* q91 - c80 */
            holcoscellmaxlimit_3__cellmaxresumelimit = \
                holcoscellmaxlimit_3__cellmaxlimit - \
                mmu_params.ethernet_mtu; /* q92 - c80 */
            holcoscellmaxlimit_4_7__cellmaxresumelimit = \
                holcoscellmaxlimit_4_7__cellmaxlimit - \
                mmu_params.ethernet_mtu; /* q93 - c80 */
            dyncelllimit_dyncellsetlimit = \
                mmu_params.shared_space_cells; /* c101 */
            dyncelllimit_dyncellresetlimit = \
                dyncelllimit_dyncellsetlimit - \
                (mmu_params.ethernet_mtu * 2); /* q97-c80*2 */
            color_drop_en_color_drop_en = 0;
            shared_pool_ctrl_dynamic_cos_drop_en = 255;
            shared_pool_ctrl_shared_pool_discard_en = 255;
            shared_pool_ctrl_sharded_pool_xoff_en = 0;
        } else if (pport >= 34) { /* uplink port 1G, 2.5G */
            SOC_IF_ERROR_RETURN(WRITE_PG2TCr(unit, port, 7, 0x80));

            if (speed == 10) { /* 1G */
                /* c95 */
                pgcelllimit_0_2__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_uplink_port;
                /* c95 */
                pgcelllimit_3__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_uplink_port;
                /* c95 */
                pgcelllimit_4_6__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_uplink_port;
                /* c89 */
                pgcelllimit_7__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_threshold_1g_uplink_ports;
                /* c95 */
                pgcelllimit_0_2__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_uplink_port;
                /* c95 */
                pgcelllimit_3__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_uplink_port;
                /* c95 */
                pgcelllimit_4_6__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_uplink_port;
                /* c89 */
                pgcelllimit_7__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_threshold_1g_uplink_ports;
            } else { /* 2.5 G */
                /* c95 */
                pgcelllimit_0_2__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_uplink_port;
                /* c95 */
                pgcelllimit_3__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_uplink_port;
                /* c95 */
                pgcelllimit_4_6__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_uplink_port;
                /* c90 */
                pgcelllimit_7__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_threshold_2p5g_uplink_ports;
                /* c95 */
                pgcelllimit_0_2__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_uplink_port;
                /* c95 */
                pgcelllimit_3__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_uplink_port;
                /* c95 */
                pgcelllimit_4_6__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_uplink_port;
                /* c90 */
                pgcelllimit_7__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_threshold_2p5g_uplink_ports;
            }
            /* c91 */
            ibppktsetlimit_pktsetlimit = \
                mmu_params.\
                xoff_packet_thresholds_per_port_uplink_port;
            ibppktsetlimit_resetlimitsel = 3;
            mmu_fc_rx_en_mmu_fc_rx_enable = 0;
            if (lossless) {
                mmu_fc_tx_en_mmu_fc_tx_enable = 0x80;
            } else {
                mmu_fc_tx_en_mmu_fc_tx_enable = 0;
            }
            /* c95 */
            pgdiscardsetlimit_0_2__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_uplink_port;
            /* c95 */
            pgdiscardsetlimit_3__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_uplink_port;
            /* c95 */
            pgdiscardsetlimit_4_6__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_uplink_port;
            /* c95 */
            pgdiscardsetlimit_7__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_uplink_port;

            /* 0 */
            holcosminxqcnt_0_2__holcosminxqcnt = 0;
            /* 0 */
            holcosminxqcnt_3__holcosminxqcnt = 0;
            /* c36 */
            holcosminxqcnt_4_7__holcosminxqcnt = \
                mmu_params.egress_xq_min_reserve_lossless_downlink_ports;
            /* C103-C72-C74+M70 */
            holcospktsetlimit_0_2__pktsetlimit = \
                mmu_params.shared_xqs_per_uplink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_0_2__holcosminxqcnt;
            /* C103-C72-C74+M71 */
            holcospktsetlimit_3__pktsetlimit = \
                mmu_params.shared_xqs_per_uplink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_3__holcosminxqcnt;
            /* C103-C72-C74+M72 */
            holcospktsetlimit_4_7__pktsetlimit = \
                mmu_params.shared_xqs_per_uplink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_4_7__holcosminxqcnt;
            /* m73 - 1 */
            holcospktresetlimit_0_2__pktresetlimit = \
                holcospktsetlimit_0_2__pktsetlimit - 1;
            /* m74 - 1 */
            holcospktresetlimit_3__pktresetlimit = \
                holcospktsetlimit_3__pktsetlimit - 1;
            /* m75 - 1 */
            holcospktresetlimit_4_7__pktresetlimit = \
                holcospktsetlimit_4_7__pktsetlimit - 1;
            /* c62 - 1 */
            cngcospktlimit0_0_7__cngpktsetlimit0 = \
                mmu_params.numxq_per_uplink_ports - 1;
            /* c62 - 1 */
            cngcospktlimit1_0_7__cngpktsetlimit1 = \
                mmu_params.numxq_per_uplink_ports - 1;
            /* c62 - 1 */
            cngportpktlimit0_cngportpktlimit0 = \
                mmu_params.numxq_per_uplink_ports - 1;
            /* c62 - 1 */
            cngportpktlimit1_cngportpktlimit1 = \
                mmu_params.numxq_per_uplink_ports - 1;
            /* c103-c72-c74 */
            dynxqcntport_dynxqcntport = \
                mmu_params.shared_xqs_per_uplink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch;
            /* m83 - 2 */
            dynresetlimport_dynresetlimport = \
                dynxqcntport_dynxqcntport - 2;
            /* 0 */
            lwmcoscellsetlimit_0_2__cellsetlimit = 0;
            /* 0 */
            lwmcoscellsetlimit_3__cellsetlimit = 0;
            /* c31 */
            lwmcoscellsetlimit_4_7__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_uplink_ports_lossless;
            /* 0 */
            lwmcoscellsetlimit_0_2__cellresetlimit = 0;
            /* 0 */
            lwmcoscellsetlimit_3__cellresetlimit = 0;
            /* c31 */
            lwmcoscellsetlimit_4_7__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_uplink_ports_lossless;
            /* CEILING((C87+C85)*C54+(C88+C86)*C55+(C89+C85)*C53+(C90+C86)*C52+I85,1) */
            tmp = (mmu_params.\
                   xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports +
                   mmu_params.headroom_for_1g_port) * \
                  mmu_params.num_1g_ports_downlink_ports + \
                  (mmu_params.\
                   xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports +
                   mmu_params.headroom_for_2p5g_port) * \
                  mmu_params.num_2p5g_ports_downlink_ports + \
                  (mmu_params.\
                   xoff_cell_threshold_1g_uplink_ports + \
                   mmu_params.headroom_for_1g_port) * \
                  mmu_params.num_2p5g_ports_uplink_ports + \
                  (mmu_params.xoff_cell_threshold_2p5g_uplink_ports + \
                   mmu_params.headroom_for_2p5g_port) * \
                  mmu_params.num_1g_ports_uplink_ports + \
                  lwmcoscellsetlimit_0_2__cellsetlimit;
            holcoscellmaxlimit_0_2__cellmaxlimit = _ceil_func(tmp, 1);
            /* CEILING((C87+C85)*C54+(C88+C86)*C55+(C89+C85)*C53+(C90+C86)*C52+I86, 1) */
            tmp = (mmu_params.\
                   xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports +
                   mmu_params.headroom_for_1g_port) * \
                  mmu_params.num_1g_ports_downlink_ports + \
                  (mmu_params.\
                   xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports +
                   mmu_params.headroom_for_2p5g_port) * \
                  mmu_params.num_2p5g_ports_downlink_ports + \
                  (mmu_params.\
                   xoff_cell_threshold_1g_uplink_ports + \
                   mmu_params.headroom_for_1g_port) * \
                  mmu_params.num_2p5g_ports_uplink_ports + \
                  (mmu_params.xoff_cell_threshold_2p5g_uplink_ports + \
                   mmu_params.headroom_for_2p5g_port) * \
                  mmu_params.num_1g_ports_uplink_ports + \
                  lwmcoscellsetlimit_3__cellsetlimit;
            holcoscellmaxlimit_3__cellmaxlimit = _ceil_func(tmp, 1);;
            /* CEILING((C87+C85)*C54+(C88+C86)*C55+(C89+C85)*C53+(C90+C86)*C52+I87, 1) */
            tmp = (mmu_params.\
                   xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports +
                   mmu_params.headroom_for_1g_port) * \
                  mmu_params.num_1g_ports_downlink_ports + \
                  (mmu_params.\
                   xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports +
                   mmu_params.headroom_for_2p5g_port) * \
                  mmu_params.num_2p5g_ports_downlink_ports + \
                  (mmu_params.\
                   xoff_cell_threshold_1g_uplink_ports + \
                   mmu_params.headroom_for_1g_port) * \
                  mmu_params.num_2p5g_ports_uplink_ports + \
                  (mmu_params.xoff_cell_threshold_2p5g_uplink_ports + \
                   mmu_params.headroom_for_2p5g_port) * \
                  mmu_params.num_1g_ports_uplink_ports + \
                  lwmcoscellsetlimit_4_7__cellsetlimit;
            holcoscellmaxlimit_4_7__cellmaxlimit = _ceil_func(tmp, 1);;
            /* o91 - c80 */
            holcoscellmaxlimit_0_2__cellmaxresumelimit = \
                holcoscellmaxlimit_0_2__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            /* o92 - c80 */
            holcoscellmaxlimit_3__cellmaxresumelimit = \
                holcoscellmaxlimit_3__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            /* o93 - c80 */
            holcoscellmaxlimit_4_7__cellmaxresumelimit = \
                holcoscellmaxlimit_4_7__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            /* c101 */
            dyncelllimit_dyncellsetlimit = \
                mmu_params.shared_space_cells;
            /* o97-c80*2 */
            dyncelllimit_dyncellresetlimit = \
                dyncelllimit_dyncellsetlimit - \
                (mmu_params.ethernet_mtu * 2);
            color_drop_en_color_drop_en = 0;
            shared_pool_ctrl_dynamic_cos_drop_en = 255;
            shared_pool_ctrl_shared_pool_discard_en = 255;
            shared_pool_ctrl_sharded_pool_xoff_en = 0;
        } else { /* downlink port 1G, 2.5G */
            SOC_IF_ERROR_RETURN(WRITE_PG2TCr(unit, port, 7, 0x80));

            if (speed == 10) { /* 1G */
                /* c93 */
                pgcelllimit_0_2__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c93 */
                pgcelllimit_3__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c93 */
                pgcelllimit_4_6__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c87 */
                pgcelllimit_7__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                /* c93 */
                pgcelllimit_0_2__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c93 */
                pgcelllimit_3__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c93 */
                pgcelllimit_4_6__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c87 */
                pgcelllimit_7__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports;
                /* c93 */
                pgdiscardsetlimit_0_2__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c93 */
                pgdiscardsetlimit_3__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c93 */
                pgdiscardsetlimit_4_6__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port;
                /* c93 */
                pgdiscardsetlimit_7__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_1g_port;
            } else { /* 2.5 G */
                /* c94 */
                pgcelllimit_0_2__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                /* c94 */
                pgcelllimit_3__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                /* c94 */
                pgcelllimit_4_6__cellsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                /* c88 */
                pgcelllimit_7__cellsetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports;
                /* c94 */
                pgcelllimit_0_2__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                /* c94 */
                pgcelllimit_3__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                /* c94 */
                pgcelllimit_4_6__cellresetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                /* c88 */
                pgcelllimit_7__cellresetlimit = \
                    mmu_params.\
                    xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports;
                /* c94 */
                pgdiscardsetlimit_0_2__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                /* c94 */
                pgdiscardsetlimit_3__discardsetlimit = \
                    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                /* c94 */
                pgdiscardsetlimit_4_6__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
                /* c94 */
                pgdiscardsetlimit_7__discardsetlimit = \
                mmu_params.discard_limit_per_port_pg_downlink_2p5g_port;
            }
            /* c92 */
            ibppktsetlimit_pktsetlimit = \
                mmu_params.\
                xoff_packet_thresholds_per_port_downlink_port;
            ibppktsetlimit_resetlimitsel = 3;
            mmu_fc_rx_en_mmu_fc_rx_enable = 0;
            if (lossless) {
                mmu_fc_tx_en_mmu_fc_tx_enable = 0x80;
            } else {
                mmu_fc_tx_en_mmu_fc_tx_enable = 0;
            }

            /* 0 */
            holcosminxqcnt_0_2__holcosminxqcnt = 0;
            /* 0 */
            holcosminxqcnt_3__holcosminxqcnt = 0;
            /* c35 */
            holcosminxqcnt_4_7__holcosminxqcnt = \
                mmu_params.egress_xq_min_reserve_lossless_uplink_ports;
            /* C105-C72-C74+I70 */
            holcospktsetlimit_0_2__pktsetlimit = \
                mmu_params.shared_xqs_per_downlink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_0_2__holcosminxqcnt;
            /* C105-C72-C74+I71 */
            holcospktsetlimit_3__pktsetlimit = \
                mmu_params.shared_xqs_per_downlink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_3__holcosminxqcnt;
            /* C105-C72-C74+I72 */
            holcospktsetlimit_4_7__pktsetlimit = \
                mmu_params.shared_xqs_per_downlink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch + \
                holcosminxqcnt_4_7__holcosminxqcnt;
            /* I73 -1 */
            holcospktresetlimit_0_2__pktresetlimit = \
                holcospktsetlimit_0_2__pktsetlimit - 1;
            /* I74 -1 */
            holcospktresetlimit_3__pktresetlimit = \
                holcospktsetlimit_3__pktsetlimit - 1;
            /* I75 -1 */
            holcospktresetlimit_4_7__pktresetlimit = \
                holcospktsetlimit_4_7__pktsetlimit - 1;
            /* c63 - 1 */
            cngcospktlimit0_0_7__cngpktsetlimit0 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1;
            /* c63 - 1 */
            cngcospktlimit1_0_7__cngpktsetlimit1 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1;
            /* c63 - 1 */
            cngportpktlimit0_cngportpktlimit0 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1;
            /* c63 - 1 */
            cngportpktlimit1_cngportpktlimit1 = \
                mmu_params.numxq_per_downlink_ports_and_cpu_port - 1;
            /* c105-c72-c74 */
            dynxqcntport_dynxqcntport = \
                mmu_params.shared_xqs_per_downlink_port - \
                mmu_params.skidmarker - \
                mmu_params.prefetch;
            /* I83-2 */
            dynresetlimport_dynresetlimport = \
                dynxqcntport_dynxqcntport - 2;
            /* 0 */
            lwmcoscellsetlimit_0_2__cellsetlimit = 0;
            /* 0 */
            lwmcoscellsetlimit_3__cellsetlimit = 0;
            /* c32 */
            lwmcoscellsetlimit_4_7__cellsetlimit = \
                mmu_params.egress_queue_min_reserve_downlink_ports_lossless;
            /* 0 */
            lwmcoscellsetlimit_0_2__cellresetlimit = 0;
            /* 0 */
            lwmcoscellsetlimit_3__cellresetlimit = 0;
            /* c32 */
            lwmcoscellsetlimit_4_7__cellresetlimit = \
                mmu_params.egress_queue_min_reserve_downlink_ports_lossless;
            /* CEILING((C87+C85)*C54+(C88+C86)*C55+(C89+C85)*C53+(C90+C86)*C52+I85, 1) */
            tmp = (mmu_params.\
                   xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports +
                   mmu_params.headroom_for_1g_port) * \
                  mmu_params.num_1g_ports_downlink_ports + \
                  (mmu_params.\
                   xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports +
                   mmu_params.headroom_for_2p5g_port) * \
                  mmu_params.num_2p5g_ports_downlink_ports + \
                  (mmu_params.\
                   xoff_cell_threshold_1g_uplink_ports + \
                   mmu_params.headroom_for_1g_port) * \
                  mmu_params.num_2p5g_ports_uplink_ports + \
                  (mmu_params.xoff_cell_threshold_2p5g_uplink_ports + \
                   mmu_params.headroom_for_2p5g_port) * \
                  mmu_params.num_1g_ports_uplink_ports + \
                  lwmcoscellsetlimit_0_2__cellsetlimit;
            holcoscellmaxlimit_0_2__cellmaxlimit = _ceil_func(tmp, 1);
            /* CEILING((C87+C85)*C54+(C88+C86)*C55+(C89+C85)*C53+(C90+C86)*C52+I86, 1) */
            tmp = (mmu_params.\
                   xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports +
                   mmu_params.headroom_for_1g_port) * \
                  mmu_params.num_1g_ports_downlink_ports + \
                  (mmu_params.\
                   xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports +
                   mmu_params.headroom_for_2p5g_port) * \
                  mmu_params.num_2p5g_ports_downlink_ports + \
                  (mmu_params.\
                   xoff_cell_threshold_1g_uplink_ports + \
                   mmu_params.headroom_for_1g_port) * \
                  mmu_params.num_2p5g_ports_uplink_ports + \
                  (mmu_params.xoff_cell_threshold_2p5g_uplink_ports + \
                   mmu_params.headroom_for_2p5g_port) * \
                  mmu_params.num_1g_ports_uplink_ports + \
                  lwmcoscellsetlimit_3__cellsetlimit;
            holcoscellmaxlimit_3__cellmaxlimit = _ceil_func(tmp, 1);
            /* CEILING((C87+C85)*C54+(C88+C86)*C55+(C89+C85)*C53+(C90+C86)*C52+I87, 1) */
            tmp = (mmu_params.\
                   xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports +
                   mmu_params.headroom_for_1g_port) * \
                  mmu_params.num_1g_ports_downlink_ports + \
                  (mmu_params.\
                   xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports +
                   mmu_params.headroom_for_2p5g_port) * \
                  mmu_params.num_2p5g_ports_downlink_ports + \
                  (mmu_params.\
                   xoff_cell_threshold_1g_uplink_ports + \
                   mmu_params.headroom_for_1g_port) * \
                  mmu_params.num_2p5g_ports_uplink_ports + \
                  (mmu_params.xoff_cell_threshold_2p5g_uplink_ports + \
                   mmu_params.headroom_for_2p5g_port) * \
                  mmu_params.num_1g_ports_uplink_ports + \
                  lwmcoscellsetlimit_4_7__cellsetlimit;
            holcoscellmaxlimit_4_7__cellmaxlimit = _ceil_func(tmp, 1);
            /* I91-C80 */
            holcoscellmaxlimit_0_2__cellmaxresumelimit = \
                holcoscellmaxlimit_0_2__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            /* I92-C80 */
            holcoscellmaxlimit_3__cellmaxresumelimit = \
                holcoscellmaxlimit_3__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            /* I93-C80 */
            holcoscellmaxlimit_4_7__cellmaxresumelimit = \
                holcoscellmaxlimit_4_7__cellmaxlimit - \
                mmu_params.ethernet_mtu;
            /* c101 */
            dyncelllimit_dyncellsetlimit = \
                mmu_params.shared_space_cells;
            /* o97-c80*2 */
            dyncelllimit_dyncellresetlimit = \
                dyncelllimit_dyncellsetlimit - \
                (mmu_params.ethernet_mtu * 2);
            color_drop_en_color_drop_en = 0;
            shared_pool_ctrl_dynamic_cos_drop_en = 255;
            shared_pool_ctrl_shared_pool_discard_en = 255;
            shared_pool_ctrl_sharded_pool_xoff_en = 0;
        }

        /* Program the setting */
        val = 0;
        /* i52 = c92 */
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &val, PKTSETLIMITf,
                          ibppktsetlimit_pktsetlimit);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &val, RESETLIMITSELf,
                          ibppktsetlimit_resetlimitsel);
        SOC_IF_ERROR_RETURN(WRITE_IBPPKTSETLIMITr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &val, MMU_FC_RX_ENABLEf,
                          mmu_fc_rx_en_mmu_fc_rx_enable);
        SOC_IF_ERROR_RETURN(WRITE_MMU_FC_RX_ENr(unit, port, val));
        val = 0;
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &val, MMU_FC_TX_ENABLEf,
                          mmu_fc_tx_en_mmu_fc_tx_enable);
        SOC_IF_ERROR_RETURN(WRITE_MMU_FC_TX_ENr(unit, port, val));

        for (cos = 0; cos < WH2_QUEUE_MAX_NUM; cos++) {
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcospktsetlimit_0_2__pktsetlimit;
            } else if (cos == 3) {
                fld_val = holcospktsetlimit_3__pktsetlimit;
            } else {
                fld_val = holcospktsetlimit_4_7__pktsetlimit;
            }
            SOC_IF_ERROR_RETURN
                (READ_HOLCOSPKTSETLIMITr(unit, port, cos, &val));
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &val,
                              PKTSETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSPKTSETLIMITr(unit, port, cos, val));

            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcospktresetlimit_0_2__pktresetlimit;
            } else if (cos == 3) {
                fld_val = holcospktresetlimit_3__pktresetlimit;
            } else {
                fld_val = holcospktresetlimit_4_7__pktresetlimit;
            }
            SOC_IF_ERROR_RETURN
                (READ_HOLCOSPKTRESETLIMITr(unit, port, cos, &val));
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &val,
                              PKTRESETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSPKTRESETLIMITr(unit, port, cos, val));

            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcosminxqcnt_0_2__holcosminxqcnt;
            } else if (cos == 3) {
                fld_val = holcosminxqcnt_3__holcosminxqcnt;
            } else {
                fld_val = holcosminxqcnt_4_7__holcosminxqcnt;
            }
            val = 0;
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &val,
                              HOLCOSMINXQCNTf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSMINXQCNTr(unit, port, cos, val));

            val = 0;
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = lwmcoscellsetlimit_0_2__cellsetlimit;
            } else if (cos == 3) {
                fld_val = lwmcoscellsetlimit_3__cellsetlimit;
            } else {
                fld_val = lwmcoscellsetlimit_4_7__cellsetlimit;
            }
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &val,
                              CELLSETLIMITf, fld_val);
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = lwmcoscellsetlimit_0_2__cellresetlimit;
            } else if (cos == 3) {
                fld_val = lwmcoscellsetlimit_3__cellresetlimit;
            } else {
                fld_val = lwmcoscellsetlimit_4_7__cellresetlimit;
            }
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &val,
                              CELLRESETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_LWMCOSCELLSETLIMITr(unit, port, cos, val));

            val = 0;
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcoscellmaxlimit_0_2__cellmaxlimit;
            } else if (cos == 3) {
                fld_val = holcoscellmaxlimit_3__cellmaxlimit;
            } else {
                fld_val = holcoscellmaxlimit_4_7__cellmaxlimit;
            }
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &val,
                              CELLMAXLIMITf, fld_val);
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = holcoscellmaxlimit_0_2__cellmaxresumelimit;
            } else if (cos == 3) {
                fld_val = holcoscellmaxlimit_3__cellmaxresumelimit;
            } else {
                fld_val = holcoscellmaxlimit_4_7__cellmaxresumelimit;
            }
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &val,
                              CELLMAXRESUMELIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSCELLMAXLIMITr(unit, port, cos, val));

            val = 0;
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = pgcelllimit_0_2__cellsetlimit;
            } else if (cos == 3) {
                fld_val = pgcelllimit_3__cellsetlimit;
            } else if ((cos >= 4) && (cos <= 6)) {
                fld_val = pgcelllimit_4_6__cellsetlimit;
            } else {
                fld_val = pgcelllimit_7__cellsetlimit;
            }
            soc_reg_field_set(unit, PGCELLLIMITr, &val,
                              CELLSETLIMITf, fld_val);
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = pgcelllimit_0_2__cellresetlimit;
            } else if (cos == 3) {
                fld_val = pgcelllimit_3__cellresetlimit;
            } else if ((cos >= 4) && (cos <= 6)) {
                fld_val = pgcelllimit_4_6__cellresetlimit;
            } else {
                fld_val = pgcelllimit_7__cellresetlimit;
            }
            soc_reg_field_set(unit, PGCELLLIMITr, &val,
                              CELLRESETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_PGCELLLIMITr(unit, port, cos, val));

            val = 0;
            if ((cos >= 0) && (cos <= 2)) {
                fld_val = pgdiscardsetlimit_0_2__discardsetlimit;
            } else if (cos == 3) {
                fld_val = pgdiscardsetlimit_3__discardsetlimit;
            } else if ((cos >= 4) && (cos <= 6)) {
                fld_val = pgdiscardsetlimit_4_6__discardsetlimit;
            } else {
                fld_val = pgdiscardsetlimit_7__discardsetlimit;
            }
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &val,
                              DISCARDSETLIMITf, fld_val);
            SOC_IF_ERROR_RETURN
                (WRITE_PGDISCARDSETLIMITr(unit, port, cos, val));

            val = 0;
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &val,
                              CNGPKTSETLIMIT0f,
                              cngcospktlimit0_0_7__cngpktsetlimit0);
            SOC_IF_ERROR_RETURN
                (WRITE_CNGCOSPKTLIMIT0r(unit, port, cos, val));
            val = 0;
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &val,
                              CNGPKTSETLIMIT1f,
                              cngcospktlimit1_0_7__cngpktsetlimit1);
            SOC_IF_ERROR_RETURN
                (WRITE_CNGCOSPKTLIMIT1r(unit, port, cos, val));
        }

        val = 0;
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &val,
                          CNGPORTPKTLIMIT0f,
                          cngportpktlimit0_cngportpktlimit0);
        SOC_IF_ERROR_RETURN(WRITE_CNGPORTPKTLIMIT0r(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &val,
                          CNGPORTPKTLIMIT1f,
                          cngportpktlimit1_cngportpktlimit1);
        SOC_IF_ERROR_RETURN(WRITE_CNGPORTPKTLIMIT1r(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &val,
                          DYNXQCNTPORTf,
                          dynxqcntport_dynxqcntport);
        SOC_IF_ERROR_RETURN(WRITE_DYNXQCNTPORTr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &val,
                          DYNRESETLIMPORTf,
                          dynresetlimport_dynresetlimport);
        SOC_IF_ERROR_RETURN(WRITE_DYNRESETLIMPORTr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                          DYNCELLSETLIMITf,
                          dyncelllimit_dyncellsetlimit);
        soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                          DYNCELLRESETLIMITf,
                          dyncelllimit_dyncellresetlimit);
        SOC_IF_ERROR_RETURN(WRITE_DYNCELLLIMITr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, COLOR_DROP_ENr, &val,
                          COLOR_DROP_ENf,
                          color_drop_en_color_drop_en);
        SOC_IF_ERROR_RETURN(WRITE_COLOR_DROP_ENr(unit, port, val));

        val = 0;
        soc_reg_field_set(unit, SHARED_POOL_CTRLr, &val,
                          DYNAMIC_COS_DROP_ENf,
                          shared_pool_ctrl_dynamic_cos_drop_en);
        soc_reg_field_set(unit, SHARED_POOL_CTRLr, &val,
                          SHARED_POOL_DISCARD_ENf,
                          shared_pool_ctrl_shared_pool_discard_en);
        soc_reg_field_set(unit, SHARED_POOL_CTRLr, &val,
                          SHARED_POOL_XOFF_ENf,
                          shared_pool_ctrl_sharded_pool_xoff_en);
        SOC_IF_ERROR_RETURN(WRITE_SHARED_POOL_CTRLr(unit, port, val));
   }

   return SOC_E_NONE;
}

STATIC int
soc_wolfhound2_mmu_8_cosq_init_helper(int unit, int lossless)
{
    uint32 tmp;
    wh2_sku_info_t *matched_port_config = NULL;
    int option;
    int downlink_port_1g;
    int downlink_port_2p5g;
    int uplink_port_1g;
    int uplink_port_2p5g;

    matched_port_config = &wh2_sku_port_config[matched_devid_idx[unit]];
    option = matched_port_config->config_option;
    /* MMU settings */
    switch (option) {
        case 1:
            downlink_port_1g = 20;
            downlink_port_2p5g = 4;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 2:
        case 3:
            downlink_port_1g = 24;
            downlink_port_2p5g = 0;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 4:
        case 5:
        case 6:
            downlink_port_1g = 24;
            downlink_port_2p5g = 0;
            uplink_port_1g = 4;
            uplink_port_2p5g = 0;
            break;
        case 7:
            downlink_port_1g = 12;
            downlink_port_2p5g = 4;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 8:
        case 9:
            downlink_port_1g = 16;
            downlink_port_2p5g = 0;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 10:
        case 11:
        case 12:
            downlink_port_1g = 16;
            downlink_port_2p5g = 0;
            uplink_port_1g = 4;
            uplink_port_2p5g = 0;
            break;
        case 13:
            downlink_port_1g = 4;
            downlink_port_2p5g = 4;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 14:
        case 15:
            downlink_port_1g = 8;
            downlink_port_2p5g = 0;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
        case 16:
        case 17:
        case 18:
            downlink_port_1g = 8;
            downlink_port_2p5g = 0;
            uplink_port_1g = 4;
            uplink_port_2p5g = 0;
            break;
        default:
            /* Default matched wh2_sku_port_config[0] */
            downlink_port_1g = 20;
            downlink_port_2p5g = 4;
            uplink_port_1g = 0;
            uplink_port_2p5g = 4;
            break;
    }

    /* Prepare MMU settings */
    /*
     * Traffic Attributes
     */
    /* c7 */
    mmu_params.min_packet_size_traffic_attr = 64;
    /*
     * c8
     * the value is 1.5 in doc, save 15 instead in sw.
     * any later usage should take care of this.
     */
    mmu_params.ethernet_mtu_traffic_attr = 15;
    /* c9 */
    mmu_params.max_packet_size_traffic_attr = 12288;
    /* c10 */
    mmu_params.standard_jumbo_frame_traffic_attr = 9216;
    /* c11 */
    mmu_params.lossless_traffic_mtu_size_traffic_attr = 2048;

    /*
     * General Information
     * moved to earlier place as these are needed by others
     */
    /* c79 = CEILING(C7/c49, 1)*/
    tmp = mmu_params.min_packet_size_traffic_attr / CELL_SIZE;
    mmu_params.min_packet_size = _ceil_func(tmp, 1);
    /* c80 = CEILING(C8*1024/c49, 1)*/
    tmp = (mmu_params.ethernet_mtu_traffic_attr * 1024) / CELL_SIZE;
    tmp /= 10; /* The value is 1.5 in MMU setting data, save as 15 in SW */
    mmu_params.ethernet_mtu = _ceil_func(tmp, 1);
    /* c81 = CEILING(C9/c49, 1)*/
    tmp = mmu_params.max_packet_size_traffic_attr / CELL_SIZE;
    mmu_params.max_packet_size = _ceil_func(tmp, 1);
    /* c82 = CEILING(C10/c49, 1)*/
    tmp = mmu_params.standard_jumbo_frame_traffic_attr / CELL_SIZE;
    mmu_params.standard_jumbo_frame = _ceil_func(tmp, 1);
    /* c83 = CEILING(C11/c49, 1)*/
    tmp = mmu_params.lossless_traffic_mtu_size_traffic_attr / CELL_SIZE;
    mmu_params.lossless_traffic_mtu_size = _ceil_func(tmp, 1);

    /*
     * Reservations
     * moved to earlier place as these are needed by others
     */
    /* c70 = c80*4*/
    mmu_params.wrr_reserve = mmu_params.ethernet_mtu * 4;
    /* c71 = (31)*2 +7+29*/
    mmu_params.reserved_for_cfap = 98;
    /* c72 */
    mmu_params.skidmarker = 7;
    /* c73 */
    mmu_params.skidmarker_option = 3;
    /* c74 */
    mmu_params.prefetch = 9;

    /*
     * SW Config Property
     */
    /* c14 */
    mmu_params.total_physical_memory_sw_cfg = 4096;
    /* c15 = c14-c71 */
    mmu_params.cfapfullsetpoint_sw_cfg = \
        mmu_params.total_physical_memory_sw_cfg - mmu_params.reserved_for_cfap;
    /* c16 = c15 */
    mmu_params.total_cell_memory_for_admission_sw_cfg = \
        mmu_params.cfapfullsetpoint_sw_cfg;

    /* c17 configurable */
    mmu_params.num_1g_ports_uplink_ports_sw_cfg = uplink_port_1g;
    /* c18 configurable */
    mmu_params.num_2p5g_ports_uplink_ports_sw_cfg = uplink_port_2p5g;
    /* c19 configurable */
    mmu_params.num_1g_ports_downlink_ports_sw_cfg = downlink_port_1g;
    /* c20 configurable */
    mmu_params.num_2p5g_ports_downlink_ports_sw_cfg = downlink_port_2p5g;
    /* c21 = SUM(C17:C18) */
    mmu_params.number_of_uplink_ports_sw_cfg = \
        mmu_params.num_1g_ports_uplink_ports_sw_cfg + \
        mmu_params.num_2p5g_ports_uplink_ports_sw_cfg;
    /* c22 = SUM(C19:C20)*/
    mmu_params.number_of_downlink_ports_sw_cfg = \
        mmu_params.num_1g_ports_downlink_ports_sw_cfg + \
        mmu_params.num_2p5g_ports_downlink_ports_sw_cfg;
    /* c23 lossless */
    if (lossless) {
        mmu_params.flow_control_enabled_sw_cfg = 1;
    } else {
        mmu_params.flow_control_enabled_sw_cfg = 0;
    }
    /* c24 PFC = 0 */
    mmu_params.flow_control_type_sw_cfg = 0;
    /* c25 = 0.25, not used */
    /* mmu_params.queue_port_limit_ratio_sw_cfg = 0.25; */

    /*
     * Device Attributes
     * moved to earlier place as these are needed by others
     */
    /* c49 */
    mmu_params.cell_size = CELL_SIZE;
    /* c50 */
    mmu_params.num_egress_queues_per_port = WH2_QUEUE_MAX_NUM; /* 8 */
    /* c51 */
    mmu_params.num_cpu_queues = 8;
    /* c52 = c17*/
    mmu_params.num_1g_ports_uplink_ports = \
        mmu_params.num_1g_ports_uplink_ports_sw_cfg;
    /* c53 = c18 */
    mmu_params.num_2p5g_ports_uplink_ports = \
        mmu_params.num_2p5g_ports_uplink_ports_sw_cfg;
    /* c54 = c19 */
    mmu_params.num_1g_ports_downlink_ports = \
        mmu_params.num_1g_ports_downlink_ports_sw_cfg;
    /* c55 = c20 */
    mmu_params.num_2p5g_ports_downlink_ports = \
        mmu_params.num_2p5g_ports_downlink_ports_sw_cfg;
    /* c56 */
    mmu_params.num_cpu_ports = 1;
    /* c57 = c21*/
    mmu_params.number_of_uplink_ports = \
        mmu_params.number_of_uplink_ports_sw_cfg;
    /* c58 = c22 */
    mmu_params.number_of_downlink_ports = \
        mmu_params.number_of_downlink_ports_sw_cfg;
    /* c59 = c14 */
    mmu_params.total_physical_memory = \
        mmu_params.total_physical_memory_sw_cfg;
    /* c60 = c16 */
    mmu_params.total_cell_memory = \
        mmu_params.total_cell_memory_for_admission_sw_cfg;
    /* c61 = c60 */
    mmu_params.total_advertised_cell_memory = \
        mmu_params.total_cell_memory;
    /* c62 */
    mmu_params.numxq_per_uplink_ports = 512;
    /* c63 */
    mmu_params.numxq_per_downlink_ports_and_cpu_port = 512;
    /* c64 */
    mmu_params.the_number_of_pipelines_per_device = 1;
    /* c66 */
    mmu_params.n_egress_queues_active_per_xpe = 1;
    /* c67 */
    mmu_params.n_ingress_ports_active_per_xpe = 1;

    /*
     * SW Config Parameter
     */
    /* c29 */
    mmu_params.egress_queue_min_reserve_uplink_ports_lossy = 0;
    /* c30 */
    mmu_params.egress_queue_min_reserve_downlink_ports_lossy = 0;
    /* c31 */
    mmu_params.egress_queue_min_reserve_uplink_ports_lossless = 12;
    /* c32 */
    mmu_params.egress_queue_min_reserve_downlink_ports_lossless = 12;
    /* c33 = c80 */
    mmu_params.egress_queue_min_reserve_cpu_ports = \
        mmu_params.ethernet_mtu;
    /* c34 */
    mmu_params.egress_xq_min_reserve_lossy_uplink_and_downlink_ports = 0;
    /* c35 */
    mmu_params.egress_xq_min_reserve_lossless_uplink_ports = 12;
    /* c36 */
    mmu_params.egress_xq_min_reserve_lossless_downlink_ports = 12;
    /* c37 */
    mmu_params.num_total_pri_groups = 4;
    /* c38 */
    mmu_params.num_active_pri_group_lossless = 1;
    /* c39 */
    mmu_params.num_lossless_queues = 4;
    /* c40 */
    mmu_params.num_lossy_queues = 0;
    /* c41 = c61 */
    mmu_params.mmu_xoff_pkt_threshold_uplink_ports = 12;
    /* c42 = c61 */
    mmu_params.mmu_xoff_pkt_threshold_downlink_ports = 12;
    /* c43 = c61 */
    mmu_params.mmu_xoff_cell_threshold_1g_port_uplink_ports = 36;
    /* c44 = c61 */
    mmu_params.mmu_xoff_cell_threshold_2p5g_port_uplink_ports = 36;
    /* c45 = c61 */
    mmu_params.mmu_xoff_cell_threshold_1g_downlink_ports = 36;
    /* c46 = c61 */
    mmu_params.mmu_xoff_cell_threshold_2p5g_downlink_ports = 36;

    /*
     * Ingress Thresholds
     */
    /* c85 */
    mmu_params.headroom_for_1g_port = 36;
    /* c86 */
    mmu_params.headroom_for_2p5g_port = 36;
    /* c87 = c45 */
    mmu_params.xoff_cell_thresholds_per_port_pg_1g_port_downlink_ports = \
        mmu_params.mmu_xoff_cell_threshold_1g_downlink_ports;
    /* c88 = c46 */
    mmu_params.xoff_cell_thresholds_per_port_pg_2p5g_port_downlink_ports = \
        mmu_params.mmu_xoff_cell_threshold_2p5g_downlink_ports;
    /* c89 = c43 */
    mmu_params.xoff_cell_threshold_1g_uplink_ports = \
        mmu_params.mmu_xoff_cell_threshold_1g_port_uplink_ports;
    /* c90 = c44 */
    mmu_params.xoff_cell_threshold_2p5g_uplink_ports = \
        mmu_params.mmu_xoff_cell_threshold_2p5g_port_uplink_ports;
    /* c91 = c41 */
    mmu_params.xoff_packet_thresholds_per_port_uplink_port = \
        mmu_params.mmu_xoff_pkt_threshold_uplink_ports;
    /* c92 = c42 */
    mmu_params.xoff_packet_thresholds_per_port_downlink_port = \
        mmu_params.mmu_xoff_pkt_threshold_downlink_ports;
    /* c93 = c61 */
    mmu_params.discard_limit_per_port_pg_downlink_1g_port = \
    mmu_params.total_advertised_cell_memory;
    /* c94 = c61 */
    mmu_params.discard_limit_per_port_pg_downlink_2p5g_port = \
    mmu_params.total_advertised_cell_memory;
    /* c95 = c61 */
    mmu_params.discard_limit_per_port_pg_uplink_port = \
        mmu_params.total_advertised_cell_memory;

    /*
     * Egress Thresholds
     */
    /* c97 = c29*c57*c40+c57*c31*c39 */
    mmu_params.total_reserved_cells_for_uplink_ports = \
        (mmu_params.egress_queue_min_reserve_uplink_ports_lossy * \
         mmu_params.number_of_uplink_ports * \
         mmu_params.num_lossy_queues) + \
        (mmu_params.number_of_uplink_ports * \
         mmu_params.egress_queue_min_reserve_uplink_ports_lossless * \
         mmu_params.num_lossless_queues);
    /* c98 = c58*c30*(c40)+c58*c32*c39 */
    mmu_params.total_reserved_cells_for_downlink_ports = \
        (mmu_params.number_of_downlink_ports * \
         mmu_params.egress_queue_min_reserve_downlink_ports_lossy * \
         mmu_params.num_lossy_queues) + \
        (mmu_params.number_of_downlink_ports * \
         mmu_params.egress_queue_min_reserve_downlink_ports_lossless * \
         mmu_params.num_lossless_queues);
    /* c99 = c56*c33*c51 */
    mmu_params.total_reserved_cells_for_cpu_port = \
        mmu_params.num_cpu_ports * \
        mmu_params.egress_queue_min_reserve_cpu_ports * \
        mmu_params.num_cpu_queues;
    /* c100 = SUM(c97:c99) */
    mmu_params.total_reserved = \
        mmu_params.total_reserved_cells_for_uplink_ports + \
        mmu_params.total_reserved_cells_for_downlink_ports + \
        mmu_params.total_reserved_cells_for_cpu_port;
    /* c101 = c61-c100 */
    mmu_params.shared_space_cells = \
        mmu_params.total_advertised_cell_memory - \
        mmu_params.total_reserved;
    /* c102 = c34*c40+c35*c39 */
    mmu_params.reserved_xqs_per_uplink_port = \
        (mmu_params.egress_xq_min_reserve_lossy_uplink_and_downlink_ports * \
         mmu_params.num_lossy_queues) + \
        (mmu_params.egress_xq_min_reserve_lossless_uplink_ports * \
         mmu_params.num_lossless_queues);
    /* c103 = c62-c102 */
    mmu_params.shared_xqs_per_uplink_port = \
        mmu_params.numxq_per_uplink_ports - \
        mmu_params.reserved_xqs_per_uplink_port;
    /* c104 = c34*c40+c36*c39 */
    mmu_params.reserved_xqs_per_downlink_port = \
        (mmu_params.egress_xq_min_reserve_lossy_uplink_and_downlink_ports * \
         mmu_params.num_lossy_queues) + \
        (mmu_params.egress_xq_min_reserve_lossless_downlink_ports * \
         mmu_params.num_lossless_queues);
    /* c105 = c63-c104 */
    mmu_params.shared_xqs_per_downlink_port = \
        mmu_params.numxq_per_downlink_ports_and_cpu_port - \
        mmu_params.reserved_xqs_per_downlink_port;
    /* c106 = c33 * c51 */
    mmu_params.reserved_xqs_at_cpu_port = \
        mmu_params.egress_queue_min_reserve_cpu_ports * \
        mmu_params.num_cpu_queues;
    /* c107 = c63 - c106 */
    mmu_params.shared_xqs_at_cpu_port = \
        mmu_params.numxq_per_downlink_ports_and_cpu_port - \
        mmu_params.reserved_xqs_at_cpu_port;

    /* Program MMU settings */
    SOC_IF_ERROR_RETURN(
        soc_wolfhound2_mmu_8_cosq_config_helper(unit, lossless));

    return SOC_E_NONE;
}

STATIC int
soc_wolfhound2_mmu_init(int unit)
{
    uint32 val, oval, cfap_max_idx;
    int lossless_mode = 0;
    soc_pbmp_t pbmp_lossless;
    int count = 0;
    int num_cos;
    int cfg_8_queues = 0; /* Default configuration for 4 COSQ.*/

    SOC_IF_ERROR_RETURN(soc_wolfhound2_tdm_init(unit));

    cfap_max_idx = soc_mem_index_max(unit, MMU_CFAPm);
    SOC_IF_ERROR_RETURN(READ_CFAPCONFIGr(unit, &val));
    oval = val;
    soc_reg_field_set(unit, CFAPCONFIGr, &val, CFAPPOOLSIZEf, cfap_max_idx);
    if (oval != val) {
        SOC_IF_ERROR_RETURN(WRITE_CFAPCONFIGr(unit, val));
    }

    /* Default lossy mode */
    lossless_mode = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    SOC_PBMP_CLEAR(pbmp_lossless);
    pbmp_lossless = soc_property_get_pbmp_default(unit, spn_MMU_LOSSLESS_PBMP,
                                                  pbmp_lossless);
    SOC_PBMP_COUNT(pbmp_lossless, count);

    /* Default 4 COSQ if config file not assigned */
    num_cos = soc_property_get(unit, spn_BCM_NUM_COS, WH2_QUEUE_DEFAULT_NUM);
    if ((num_cos < 1) || (num_cos > WH2_QUEUE_MAX_NUM)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Warning: invalid COSQ number %d specified. "
                             "Use default 4 COSQ. \n"), num_cos));
        num_cos = WH2_QUEUE_DEFAULT_NUM;
    }
    if (num_cos > WH2_QUEUE_DEFAULT_NUM) {
        cfg_8_queues = 1;
    }

    if (cfg_8_queues) {
        /* 8 COSQ setting.*/
        if (lossless_mode) {
            SOC_IF_ERROR_RETURN(
                soc_wolfhound2_mmu_8_cosq_init_helper(unit, 1));
        } else {
            SOC_IF_ERROR_RETURN(
                soc_wolfhound2_mmu_8_cosq_init_helper(unit, 0));
        }
    } else {
        /* 4 COSQ setting (default). */
        if (lossless_mode) {
            if ((count > 0) && (count <= 4)) {
                /*
                 * Lossy+Lossless(PFC) mode. Maximum 4 support ports lossless.
                 */
                SOC_IF_ERROR_RETURN(
                    soc_wolfhound2_mmu_lossy_lossless_pfc_init_helper(unit));
            } else {
                /* Lossless(Pause) mode. Legacy lossless mode.*/
                SOC_IF_ERROR_RETURN(
                    soc_wolfhound2_mmu_lossy_or_lossless_init_helper(unit, 1));
            }
        } else {
            /* Legacy loss mode.*/
            SOC_IF_ERROR_RETURN(
                soc_wolfhound2_mmu_lossy_or_lossless_init_helper(unit, 0));
        }
    }

    /* Enable IP to CMICM credit transfer */
    val = 0;
    soc_reg_field_set(unit, IP_TO_CMICM_CREDIT_TRANSFERr, &val,
                      TRANSFER_ENABLEf, 1);
    soc_reg_field_set(unit, IP_TO_CMICM_CREDIT_TRANSFERr, &val,
                      NUM_OF_CREDITSf, 32);
    SOC_IF_ERROR_RETURN(WRITE_IP_TO_CMICM_CREDIT_TRANSFERr(unit, val));

    /*
     * Port enable
     */
    val = 0;
    soc_reg_field_set(unit, MMUPORTENABLEr, &val, MMUPORTENABLEf,
                      SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_MMUPORTENABLEr(unit, val));

    return SOC_E_NONE;
}

STATIC int
soc_wolfhound2_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 value;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &value));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
soc_wolfhound2_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
soc_wolfhound2_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 value;

    value = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, value));

    return SOC_E_NONE;
}

/*
 * Wolfhound2 chip driver functions.
 */
soc_functions_t soc_wolfhound2_drv_funs = {
    soc_wolfhound2_misc_init,
    soc_wolfhound2_mmu_init,
    soc_wolfhound2_age_timer_get,
    soc_wolfhound2_age_timer_max_get,
    soc_wolfhound2_age_timer_set,
    NULL,
    NULL,
    NULL,
};

#endif /* BCM_HURRICANE3_SUPPORT */
