/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_dispatch.h>
#include <phymod/acc/phymod_tsc_iblk.h>
#include <phymod/chip/osprey7_v2l8p2.h>
#include "osprey7_v2l8p2/tier1/osprey7_v2l8p2_cfg_seq.h"
#include "osprey7_v2l8p2/tier1/osprey7_v2l8p2_enum.h"
#include "osprey7_v2l8p2/tier1/osprey7_v2l8p2_common.h"
#include "osprey7_v2l8p2/tier1/osprey7_v2l8p2_interface.h"
#include "osprey7_v2l8p2/tier1/osprey7_v2l8p2_dependencies.h"
#include "osprey7_v2l8p2/tier1/osprey7_v2l8p2_internal.h"

#define OSPREY7_V2L8P2_MODEL               0x29
#define OSPREY7_V2L8P2_NOF_LANES_IN_CORE   0x8
#define OSPREY7_V2L8P2_PHY_ALL_LANES       0xff
#define OSPREY7_V2L8P2_TX_TAP_NUM          12
#define OSPREY7_V2L8P2_PMD_CRC_UCODE       1

#define OSPREY7_V2L8P2_REF_CLOCK_312P5_HZ  312500000


#define OSPREY7_V2L8P2_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type           = (_core_access)->type; \
        (_phy_access)->port_loc       = (_core_access)->port_loc; \
        (_phy_access)->device_op_mode = (_core_access)->device_op_mode; \
        (_phy_access)->access.lane_mask = OSPREY7_V2L8P2_PHY_ALL_LANES; \
    }while(0)



int osprey7_v2l8p2_core_identify(const phymod_core_access_t* core,
                                 uint32_t core_id, uint32_t* is_identified)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    info->core_version = phymodCoreVersionOsprey7_v2l8p2;
    info->serdes_id = 0;
    info->phy_id0 = 0;
    info->phy_id1 = 0;

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    phymod_core_access_t  core_copy;
    uint32_t tx_lane_map, rx_lane_map;
    int i = 0;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_pmd_lane_map_get(&core_copy.access, &tx_lane_map, &rx_lane_map));

    /*next get the lane map into serdes spi format */
    for (i = 0; i < OSPREY7_V2L8P2_NOF_LANES_IN_CORE; i++) {
        lane_map->lane_map_tx[tx_lane_map >> (4 * i) & 0xf] = i;
        lane_map->lane_map_rx[rx_lane_map >> (4 * i) & 0xf] = i;
    }

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_core_reset_set(const phymod_core_access_t* core,
                                  phymod_reset_mode_t reset_mode,
                                  phymod_reset_direction_t direction)
{
    return PHYMOD_E_NONE;

}

int osprey7_v2l8p2_core_reset_get(const phymod_core_access_t* core,
                                  phymod_reset_mode_t reset_mode,
                                  phymod_reset_direction_t* direction)
{
    return PHYMOD_E_NONE;

}

int osprey7_v2l8p2_phy_tx_lane_control_set(const phymod_phy_access_t* phy,
                                           phymod_phy_tx_lane_control_t tx_control)
{
    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_tx_lane_control_get(const phymod_phy_access_t* phy,
                                           phymod_phy_tx_lane_control_t *tx_control)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_rx_lane_control_set(const phymod_phy_access_t* phy,
                                           phymod_phy_rx_lane_control_t rx_control)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_rx_lane_control_get(const phymod_phy_access_t* phy,
                                           phymod_phy_rx_lane_control_t* rx_control)
{
    return PHYMOD_E_NONE;

}

int osprey7_v2l8p2_phy_firmware_lane_config_get(const phymod_phy_access_t* phy,
                                                phymod_firmware_lane_config_t* fw_config)
{
    struct osprey7_v2l8p2_uc_lane_config_st lane_config;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_MEMSET(&lane_config, 0x0, sizeof(lane_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));

    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_get_uc_lane_cfg(&pm_phy_copy.access, &lane_config));

    fw_config->LaneConfigFromPCS     = lane_config.field.lane_cfg_from_pcs;
    fw_config->AnEnabled             = lane_config.field.an_enabled;
    fw_config->DfeOn                 = lane_config.field.dfe_on;
    fw_config->LpDfeOn               = lane_config.field.dfe_lp_mode;
    fw_config->ForceBrDfe            = lane_config.field.force_brdfe_on;
    fw_config->MediaType             = lane_config.field.media_type;
    fw_config->UnreliableLos         = lane_config.field.unreliable_los;
    fw_config->Cl72AutoPolEn         = lane_config.field.cl72_auto_polarity_en;
    fw_config->ScramblingDisable     = lane_config.field.scrambling_dis;
    fw_config->Cl72RestTO            = lane_config.field.cl72_restart_timeout_en;
    fw_config->ForceExtenedReach     = lane_config.field.force_er;
    fw_config->ForceNormalReach      = lane_config.field.force_nr;
    fw_config->LpPrecoderEnabled     = lane_config.field.lp_has_prec_en;
    fw_config->ForcePAM4Mode         = lane_config.field.force_pam4_mode;
    fw_config->ForceNRZMode          = lane_config.field.force_nrz_mode;

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    enum osprey7_v2l8p2_txfir_tap_enable_enum enable_taps = OSPREY7_V2L8P2_NRZ_6TAP;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_lane_soft_reset(&phy_copy.access, 1));

    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        /*next check 3 tap mode or 6 tap mode */
        if (tx->tap_mode == phymodTxTapMode3Tap) {
            if (tx->sig_method == phymodSignallingMethodNRZ) {
                enable_taps= OSPREY7_V2L8P2_NRZ_LP_3TAP;
            } else {
                enable_taps= OSPREY7_V2L8P2_PAM4_LP_3TAP;
            }
            PHYMOD_PMD_IF_ERR_RETURN
                (osprey7_v2l8p2_apply_txfir_cfg(&phy_copy.access,
                                                 enable_taps,
                                                 tx->pre3,
                                                 0,
                                                 tx->pre,
                                                 tx->main,
                                                 tx->post,
                                                 0));
        } else {
            if (tx->sig_method == phymodSignallingMethodNRZ) {
                enable_taps= OSPREY7_V2L8P2_NRZ_6TAP;
            } else {
                enable_taps= OSPREY7_V2L8P2_PAM4_6TAP;
            }
            PHYMOD_PMD_IF_ERR_RETURN
                (osprey7_v2l8p2_apply_txfir_cfg(&phy_copy.access,
                                                 enable_taps,
                                                 tx->pre3,
                                                 tx->pre2,
                                                 tx->pre,
                                                 tx->main,
                                                 tx->post,
                                                 tx->post2));
        }
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_lane_soft_reset(&phy_copy.access, 0));

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    phymod_core_access_t  core_copy;
    uint8_t pmd_tx_addr[8], pmd_rx_addr[8];
    int i = 0;
    uint8_t tmp_phy_lane;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    /*next get the lane map into serdes spi format */
    for (i = 0; i < OSPREY7_V2L8P2_NOF_LANES_IN_CORE; i++) {
        tmp_phy_lane = (uint8_t) lane_map->lane_map_tx[i];
        pmd_tx_addr[tmp_phy_lane] = i;
        tmp_phy_lane = (uint8_t) lane_map->lane_map_rx[i];
        pmd_rx_addr[tmp_phy_lane] = i;
    }

    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_map_lanes(&core_copy.access,
                                   OSPREY7_V2L8P2_NOF_LANES_IN_CORE,
                                   pmd_tx_addr,
                                   pmd_rx_addr));

    return PHYMOD_E_NONE;
}

static
int _osprey7_v2l8p2_phy_firmware_lane_config_set(const phymod_phy_access_t* phy,
                                                phymod_firmware_lane_config_t fw_config)
{
    uint32_t is_warm_boot;
    struct osprey7_v2l8p2_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_MEMSET(&serdes_firmware_config, 0x0, sizeof(serdes_firmware_config));
    serdes_firmware_config.field.lane_cfg_from_pcs      = fw_config.LaneConfigFromPCS;
    serdes_firmware_config.field.an_enabled             = fw_config.AnEnabled;
    serdes_firmware_config.field.dfe_on                 = fw_config.DfeOn;
    serdes_firmware_config.field.force_brdfe_on         = fw_config.ForceBrDfe;
    /* serdes_firmware_config.field.cl72_emulation_en = fw_config.Cl72Enable; */
    serdes_firmware_config.field.scrambling_dis         = fw_config.ScramblingDisable;
    serdes_firmware_config.field.unreliable_los         = fw_config.UnreliableLos;
    serdes_firmware_config.field.media_type             = fw_config.MediaType;
    serdes_firmware_config.field.dfe_lp_mode            = fw_config.LpDfeOn;
    serdes_firmware_config.field.cl72_auto_polarity_en  = fw_config.Cl72AutoPolEn;
    serdes_firmware_config.field.cl72_restart_timeout_en = fw_config.Cl72RestTO;
    serdes_firmware_config.field.force_er               = fw_config.ForceExtenedReach;
    serdes_firmware_config.field.force_nr               = fw_config.ForceNormalReach;
    serdes_firmware_config.field.force_nrz_mode         = fw_config.ForceNRZMode;
    serdes_firmware_config.field.force_pam4_mode        = fw_config.ForcePAM4Mode;
    serdes_firmware_config.field.lp_has_prec_en         = fw_config.LpPrecoderEnabled;

    PHYMOD_IF_ERR_RETURN(PHYMOD_IS_WRITE_DISABLED(&phy->access, &is_warm_boot));

    if(!is_warm_boot) {
        PHYMOD_IF_ERR_RETURN(osprey7_v2l8p2_set_uc_lane_cfg(&phy_copy.access, serdes_firmware_config));
    }
    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_firmware_lane_config_set(const phymod_phy_access_t* phy,
                                                phymod_firmware_lane_config_t fw_config)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_lane_soft_reset(&phy_copy.access, 1));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (_osprey7_v2l8p2_phy_firmware_lane_config_set(&phy_copy, fw_config));
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_lane_soft_reset(&phy_copy.access, 0));

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_rx_restart(const phymod_phy_access_t* phy)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_tx_rx_polarity_set(&phy_copy.access, polarity->tx_polarity, polarity->rx_polarity));

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_tx_rx_polarity_get(&phy_copy.access, &polarity->tx_polarity, &polarity->rx_polarity));
    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    uint8_t pmd_tx_tap_mode;
    int16_t val;
    uint16_t tx_tap_nrz_mode = 0;
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));


    /* read current tx tap mode */
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_tx_tap_mode_get(&phy_copy.access, &pmd_tx_tap_mode));

    /*read current tx NRZ mode control info */
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_tx_nrz_mode_get(&phy_copy.access, &tx_tap_nrz_mode));

    if (pmd_tx_tap_mode == 0) {
        /* 3 tap mode */
        tx->tap_mode = phymodTxTapMode3Tap;
    } else {
        tx->tap_mode = phymodTxTapMode6Tap;
    }

    if (tx_tap_nrz_mode) {
        tx->sig_method = phymodSignallingMethodNRZ;
    } else {
        tx->sig_method = phymodSignallingMethodPAM4;
    }

    /*next check 3 tap mode or 6 tap mode */
    if (tx->tap_mode == phymodTxTapMode3Tap) {
        PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_read_tx_afe(&phy_copy.access, OSPREY7_V2L8P2_TX_AFE_TAP0, &val));
        tx->pre = val;
        PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_read_tx_afe(&phy_copy.access, OSPREY7_V2L8P2_TX_AFE_TAP1, &val));
        tx->main = val;
        PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_read_tx_afe(&phy_copy.access, OSPREY7_V2L8P2_TX_AFE_TAP2, &val));
        tx->post = val;
        tx->pre2 = 0;
        tx->post2 = 0;
        tx->post3 = 0;
        tx->pre3 = 0;
    } else {
        PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_read_tx_afe(&phy_copy.access, OSPREY7_V2L8P2_TX_AFE_TAP0, &val));
        tx->pre3 = val;
        PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_read_tx_afe(&phy_copy.access, OSPREY7_V2L8P2_TX_AFE_TAP1, &val));
        tx->pre2 = val;
        PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_read_tx_afe(&phy_copy.access, OSPREY7_V2L8P2_TX_AFE_TAP2, &val));
        tx->pre = val;
        PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_read_tx_afe(&phy_copy.access, OSPREY7_V2L8P2_TX_AFE_TAP3, &val));
        tx->main = val;
        PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_read_tx_afe(&phy_copy.access, OSPREY7_V2L8P2_TX_AFE_TAP4, &val));
        tx->post = val;
        PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_read_tx_afe(&phy_copy.access, OSPREY7_V2L8P2_TX_AFE_TAP5, &val));
        tx->post2 = val;
        tx->post3 = 0;
    }

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_lane_soft_reset(&phy_copy.access, 1));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
             (osprey7_v2l8p2_tx_pi_freq_override(&phy_copy.access,
                                                 tx_override->phase_interpolator.enable,
                                                 tx_override->phase_interpolator.value));
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_lane_soft_reset(&phy_copy.access, 0));

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
    phymod_phy_access_t phy_copy;
    int16_t value;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_tx_pi_control_get(&phy_copy.access, &value));

    tx_override->phase_interpolator.value = (int32_t) value;

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    uint8_t uc_lane_stopped;
    phymod_phy_signalling_method_t signalling_mode;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next read the PAM4 mode or  not */
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_signalling_mode_status_get(&phy_copy.access, &signalling_mode));

    for (i = 0; i < num_lane; i++) {
        uint32_t j = 0;
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        /* first check if uc lane is stopped already */
        PHYMOD_IF_ERR_RETURN(osprey7_v2l8p2_stop_uc_lane_status(&phy_copy.access, &uc_lane_stopped));
        if (!uc_lane_stopped) {
            PHYMOD_IF_ERR_RETURN(osprey7_v2l8p2_stop_rx_adaptation(&phy_copy.access, 1));
        }

        PHYMOD_PMD_IF_ERR_RETURN
            (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_VGA, (int8_t) rx->vga.value));

        PHYMOD_PMD_IF_ERR_RETURN
            (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_PF, (int8_t) rx->peaking_filter.value));

        PHYMOD_PMD_IF_ERR_RETURN
            (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_PF2, (int8_t) rx->low_freq_peaking_filter.value));

        /* next check the peaking value */
        PHYMOD_PMD_IF_ERR_RETURN
            (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_PF3, (int8_t) rx->high_freq_peaking_filter.value));

        for (j = 0 ; j < rx->num_of_dfe_taps ; j++){
            switch (j) {
                case 0:
                    if (signalling_mode == phymodSignallingMethodNRZ) {
                        PHYMOD_PMD_IF_ERR_RETURN
                            (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE1, (int8_t) rx->dfe[j].value));
                    } else {
                        if (rx->dfe[0].enable) {
                            PHYMOD_DEBUG_ERROR(("ERROR :: DFE1 is not supported on PAM4 mode \n"));
                            return PHYMOD_E_PARAM;
                        }
                    }
                    break;
                case 1:
                    PHYMOD_PMD_IF_ERR_RETURN
                        (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE2, (int8_t) rx->dfe[j].value));
                    break;
                case 2:
                    PHYMOD_PMD_IF_ERR_RETURN
                        (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE3, (int8_t) rx->dfe[j].value));
                    break;
                case 3:
                    PHYMOD_PMD_IF_ERR_RETURN
                        (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE4, (int8_t) rx->dfe[j].value));
                    break;
                case 4:
                    PHYMOD_PMD_IF_ERR_RETURN
                        (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE5, (int8_t) rx->dfe[j].value));
                    break;
                case 5:
                    PHYMOD_PMD_IF_ERR_RETURN
                        (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE6, (int8_t) rx->dfe[j].value));
                    break;
                case 6:
                    PHYMOD_PMD_IF_ERR_RETURN
                        (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE7, (int8_t) rx->dfe[j].value));
                    break;
                case 7:
                    PHYMOD_PMD_IF_ERR_RETURN
                        (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE8, (int8_t) rx->dfe[j].value));
                    break;
                case 8:
                    PHYMOD_PMD_IF_ERR_RETURN
                        (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE9, (int8_t) rx->dfe[j].value));
                    break;
                case 9:
                    PHYMOD_PMD_IF_ERR_RETURN
                        (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE10, (int8_t) rx->dfe[j].value));
                    break;
                case 10:
                    PHYMOD_PMD_IF_ERR_RETURN
                        (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE11,(int8_t)  rx->dfe[j].value));
                    break;
                case 11:
                    PHYMOD_PMD_IF_ERR_RETURN
                        (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE12, (int8_t) rx->dfe[j].value));
                    break;
                case 12:
                    PHYMOD_PMD_IF_ERR_RETURN
                        (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE13, (int8_t) rx->dfe[j].value));
                    break;
                case 13:
                    PHYMOD_PMD_IF_ERR_RETURN
                        (osprey7_v2l8p2_write_rx_afe(&phy_copy.access, RX_AFE_DFE14, (int8_t) rx->dfe[j].value));
                    break;
                default:
                    return PHYMOD_E_PARAM;
            }
        }
    }

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    uint32_t j;
    int8_t val;
    phymod_phy_access_t phy_copy;
    phymod_phy_signalling_method_t signalling_mode;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next read the PAM4 mode or  not */
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_signalling_mode_status_get(&phy_copy.access, &signalling_mode));

    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_VGA, &val));
    rx->vga.value = val;
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_PF, &val));
    rx->peaking_filter.value = val;
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_PF2, &val));
    rx->low_freq_peaking_filter.value = val;
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_PF3, &val));
    rx->high_freq_peaking_filter.value = val;

    rx->num_of_dfe_taps = 14;

    for (j = 0 ; j < rx->num_of_dfe_taps ; j++){
        switch (j) {
            case 0:
                if (signalling_mode == phymodSignallingMethodNRZ) {
                    PHYMOD_IF_ERR_RETURN
                        (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE1, &val));
                    rx->dfe[0].enable = 1;
                } else {
                    /* for PAM4 mode, DFE1 tap is not supported */
                    rx->dfe[0].enable = 0;
                }
                break;
            case 1:
                PHYMOD_IF_ERR_RETURN
                    (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE2, &val));
                break;
            case 2:
                PHYMOD_IF_ERR_RETURN
                    (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE3, &val));
                break;
            case 3:
                PHYMOD_IF_ERR_RETURN
                    (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE4, &val));
                break;
            case 4:
                PHYMOD_IF_ERR_RETURN
                    (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE5, &val));
                break;
            case 5:
                PHYMOD_IF_ERR_RETURN
                    (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE6, &val));
                break;
            case 6:
                PHYMOD_IF_ERR_RETURN
                    (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE7, &val));
                break;
            case 7:
                PHYMOD_IF_ERR_RETURN
                    (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE8, &val));
                break;
            case 8:
                PHYMOD_IF_ERR_RETURN
                    (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE9, &val));
                break;
            case 9:
                PHYMOD_IF_ERR_RETURN
                    (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE10, &val));
                break;
            case 10:
                PHYMOD_IF_ERR_RETURN
                    (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE11, &val));
                break;
            case 11:
                PHYMOD_IF_ERR_RETURN
                    (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE12, &val));
                break;
            case 12:
                PHYMOD_IF_ERR_RETURN
                    (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE13, &val));
                break;
            case 13:
                PHYMOD_IF_ERR_RETURN
                    (osprey7_v2l8p2_read_rx_afe(&phy_copy.access, RX_AFE_DFE14, &val));
                break;
            default:
                return PHYMOD_E_PARAM;
        }
        rx->dfe[j].value = val;
    }

    for (j = 1 ; j < rx->num_of_dfe_taps ; j++){
        rx->dfe[j].enable = 1;
    }
    rx->vga.enable = 1;
    rx->peaking_filter.enable = 1;
    rx->low_freq_peaking_filter.enable = 1;
    rx->high_freq_peaking_filter.enable = 1;

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_speed_config_set(const phymod_phy_access_t* phy,
                                   const phymod_phy_speed_config_t* speed_config,
                                   const phymod_phy_pll_state_t* old_pll_state,
                                   phymod_phy_pll_state_t* new_pll_state)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_speed_config_get(const phymod_phy_access_t* phy, phymod_phy_speed_config_t* speed_config)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_loopback_set(const phymod_phy_access_t* phy,
                                    phymod_loopback_mode_t loopback,
                                    uint32_t enable)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_loopback_get(const phymod_phy_access_t* phy,
                                    phymod_loopback_mode_t loopback,
                                    uint32_t* enable)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_core_init(const phymod_core_access_t* core,
                             const phymod_core_init_config_t* init_config,
                             const phymod_core_status_t* core_status)
{
    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* pmd_lock)
{
    int start_lane, num_lane, i;
    phymod_phy_access_t pm_phy_copy;
    uint8_t tmp_lock;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    *pmd_lock = 1;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_pmd_lock_status(&pm_phy_copy.access, &tmp_lock));
        *pmd_lock &= (uint32_t) tmp_lock;
    }
    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_rx_pmd_lock_status_get(const phymod_phy_access_t* phy,
                                         uint32_t* pmd_lock,
                                         uint32_t* pmd_lock_change)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_rx_pmd_lock_status_get(&pm_phy_copy.access,
                                          pmd_lock,
                                          pmd_lock_change));
    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* signal_detect)
{
    int start_lane, num_lane, i;
    phymod_phy_access_t pm_phy_copy;
    uint32_t tmp_detect;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    *signal_detect = 1;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_signal_detect(&pm_phy_copy.access, &tmp_detect));
        *signal_detect &= tmp_detect;
    }
    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* core_vco_pll_multiplier)
{
    phymod_phy_access_t pm_phy_copy;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_INTERNAL_read_pll_div(&pm_phy_copy.access, core_vco_pll_multiplier));

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_firmware_load_info_get(const phymod_phy_access_t* phy,
                                              phymod_firmware_load_info_t* info)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_rx_adaptation_resume(const phymod_phy_access_t* phy)
{
    phymod_phy_access_t phy_copy;
    uint8_t uc_lane_stopped;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN(osprey7_v2l8p2_stop_uc_lane_status(&phy_copy.access, &uc_lane_stopped));
        if (uc_lane_stopped) {
            PHYMOD_IF_ERR_RETURN(osprey7_v2l8p2_stop_rx_adaptation(&phy_copy.access, 0));
        }
    }
    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_tx_pam4_precoder_enable_set(const phymod_phy_access_t* phy, int enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_lane_soft_reset(&phy_copy.access, 1));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_tx_pam4_precoder_enable_set(&phy_copy.access, enable));
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_lane_soft_reset(&phy_copy.access, 0));

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_pll_pwrdn(const phymod_phy_access_t* phy, uint32_t pll_index, uint32_t pwrdn)
{
    phymod_phy_access_t phy_copy;
    uint32_t is_pwrdn = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << 0;
    phy_copy.access.pll_idx = pll_index;

    PHYMOD_IF_ERR_RETURN(osprey7_v2l8p2_pll_pwrdn_get(&phy_copy.access, &is_pwrdn));
    if (is_pwrdn != pwrdn) {
        if (pwrdn) {
            PHYMOD_IF_ERR_RETURN(osprey7_v2l8p2_core_pwrdn(&phy_copy.access, PWRDN));
        } else {
            PHYMOD_IF_ERR_RETURN(osprey7_v2l8p2_core_pwrdn(&phy_copy.access, PWR_ON));
        }
    }

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_tx_pam4_precoder_enable_get(const phymod_phy_access_t* phy, int *enable)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_tx_pam4_precoder_enable_get(&pm_phy_copy.access, enable));
    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_eye_margin_est_get(const phymod_phy_access_t* phy,
                                          phymod_eye_margin_mode_t eye_margin_mode,
                                          uint32_t* value)
{
    int start_lane, num_lane;
    uint16_t hz_l, hz_r, vt_u, vt_d;
    phymod_phy_access_t phy_copy;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_INTERNAL_get_eye_margin_est(&phy_copy.access, &hz_l, &hz_r, &vt_u, &vt_d));

    switch (eye_margin_mode) {
    case phymod_eye_marign_HZ_L:
        *value = hz_l;
        break;
    case phymod_eye_marign_HZ_R:
        *value = hz_r;
        break;
    case phymod_eye_marign_VT_U:
        *value = vt_u;
        break;
    case phymod_eye_marign_VT_D:
        *value = vt_d;
        break;
    default:
        *value = 0;
        break;
    }

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_channel_loss_hint_set(const phymod_phy_access_t* phy,  uint32_t channel_loss)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_channel_loss_hint_get(const phymod_phy_access_t* phy,  uint32_t *channel_loss)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_pmd_info_init(const phymod_phy_access_t* phy)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1;

    /* call PMD api to init the global PMD data structure */
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_init_osprey7_v2l8p2_info(&phy_copy.access));

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_rx_ppm_get(const phymod_phy_access_t* phy, int16_t* rx_ppm)
{

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_pll_powerdown_get(const phymod_phy_access_t* phy,
                                         uint32_t pll_index, uint32_t* powerdown)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    phy_copy.access.pll_idx = pll_index;
    phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (osprey7_v2l8p2_pll_pwrdn_get(&phy_copy.access, powerdown));

    return PHYMOD_E_NONE;
}

int osprey7_v2l8p2_phy_uc_ram_var_set(const phymod_phy_access_t* phy, phymod_uc_ram_variable_t ram_var, int16_t value)
{
    int rv = PHYMOD_E_NONE;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_lane_soft_reset(&phy_copy.access, 1));

    switch(ram_var){
        case phymodUcRAMVar_PAM4_LT_NR_ER_LMT_BIAS_OSX1:
            PHYMOD_IF_ERR_RETURN
                (osprey7_v2l8p2_pam4_nr_er_lmt_bias_osx1_set(&phy_copy.access, value));
            break;
        case phymodUcRAMVar_PAM4_LT_NR_ER_LMT_BIAS_OSX2:
            PHYMOD_IF_ERR_RETURN
                 (osprey7_v2l8p2_pam4_nr_er_lmt_bias_osx2_set(&phy_copy.access, value));
            break;
        case phymodUcRAMVar_PAM4_LT_NR_ER_LMT2_BIAS_OSX1:
            PHYMOD_IF_ERR_RETURN
                (osprey7_v2l8p2_pam4_nr_er_lmt2_bias_osx1_set(&phy_copy.access, value));
            break;
        case phymodUcRAMVar_PAM4_LT_NR_ER_LMT2_BIAS_OSX2:
            PHYMOD_IF_ERR_RETURN
                 (osprey7_v2l8p2_pam4_nr_er_lmt2_bias_osx2_set(&phy_copy.access, value));
            break;
        case phymodUcRAMVar_SIGNAL_DETECT_THRESHOLD:
            PHYMOD_IF_ERR_RETURN
                 (osprey7_v2l8p2_usr_signal_detect_threshold_set(&phy_copy.access, value));
            break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported uc RAM variable %u"), ram_var));
        rv =  PHYMOD_E_PARAM;
    }

    PHYMOD_IF_ERR_RETURN
            (osprey7_v2l8p2_lane_soft_reset(&phy_copy.access, 0));

    return rv;
}

int osprey7_v2l8p2_phy_uc_ram_var_get(const phymod_phy_access_t* phy, phymod_uc_ram_variable_t ram_var, int16_t* value)
{

    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));


    switch(ram_var){
        case phymodUcRAMVar_PAM4_LT_NR_ER_LMT_BIAS_OSX1:
            PHYMOD_IF_ERR_RETURN
                (osprey7_v2l8p2_pam4_nr_er_lmt_bias_osx1_get(&phy_copy.access, value));
            break;
        case phymodUcRAMVar_PAM4_LT_NR_ER_LMT_BIAS_OSX2:
            PHYMOD_IF_ERR_RETURN
                 (osprey7_v2l8p2_pam4_nr_er_lmt_bias_osx2_get(&phy_copy.access, value));
            break;
        case phymodUcRAMVar_PAM4_LT_NR_ER_LMT2_BIAS_OSX1:
            PHYMOD_IF_ERR_RETURN
                (osprey7_v2l8p2_pam4_nr_er_lmt2_bias_osx1_get(&phy_copy.access, value));
            break;
        case phymodUcRAMVar_PAM4_LT_NR_ER_LMT2_BIAS_OSX2:
            PHYMOD_IF_ERR_RETURN
                 (osprey7_v2l8p2_pam4_nr_er_lmt2_bias_osx2_get(&phy_copy.access, value));
            break;
        case phymodUcRAMVar_SIGNAL_DETECT_THRESHOLD:
            PHYMOD_IF_ERR_RETURN
                 (osprey7_v2l8p2_usr_signal_detect_threshold_get(&phy_copy.access, value));
            break;
        default:
             PHYMOD_DEBUG_ERROR(("ERROR :: %d uC RAM varaible is not supported.\n", ram_var));
             return PHYMOD_E_PARAM;
    }


    return PHYMOD_E_NONE;
}

