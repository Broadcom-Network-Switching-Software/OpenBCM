/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_dispatch.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/merlin16.h>
#include <phymod/phymod_acc.h>
#include <phymod/chip/bcmi_tsce16_xgxs_defs.h>
#include "../../merlin16/tier1/merlin16_cfg_seq.h"
#include "../../merlin16/tier1/merlin16_enum.h"
#include "../../merlin16/tier1/merlin16_common.h"
#include "../../merlin16/tier1/merlin16_interface.h"
#include "../../merlin16/tier1/merlin16_dependencies.h"
#include "../../merlin16/tier1/merlin16_debug_functions.h"
#include "../../merlin16/tier1/merlin16_internal.h"

#define MERLIN16_MODEL         0x23
#define MERLIN16_PHY_ALL_LANES 0xf
#define MERLIN16_NOF_LANES_IN_CORE 4

#define MERLIN16_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type           = (_core_access)->type; \
        (_phy_access)->port_loc       = (_core_access)->port_loc; \
        (_phy_access)->device_op_mode = (_core_access)->device_op_mode; \
        (_phy_access)->access.lane_mask = MERLIN16_PHY_ALL_LANES; \
    }while(0)

#define MERLIN16_NOF_DFES 5

#define MERILIN16_PMD_CRC_UCODE  1
/* uController's firmware */
extern unsigned char merlin16_ucode[];
extern unsigned short merlin16_ucode_ver;
extern unsigned short merlin16_ucode_crc;
extern unsigned short merlin16_ucode_len;


/*Initialize phymod module*/
int merlin16_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    merlin16_rev_id0_t rev_id0;
    merlin16_rev_id1_t rev_id1;
    const phymod_access_t *pm_acc = &core->access;
    *is_identified = 0;

    PHYMOD_IF_ERR_RETURN(merlin16_identify(pm_acc, &rev_id0, &rev_id1));
    if (rev_id0.revid_model == MERLIN16_MODEL) {
        *is_identified = 1;
    }

    return PHYMOD_E_NONE;
}

/*Retrive core information*/
int merlin16_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    info->core_version = phymodCoreVersionMerlin16;
    PHYMOD_STRNCPY(info->name, "Merlin16", PHYMOD_STRLEN("Merlin16") + 1);
    info->phy_id0 = 0;
    info->phy_id1= 0;

    return PHYMOD_E_NONE;
}

/*Set\get lane mapping*/
int merlin16_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    /*Just suport lane address mapping, not support lane swapping*/
    return PHYMOD_E_NONE;
}
int merlin16_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    /*Just suport lane address mapping, not support lane swapping*/
    return PHYMOD_E_NONE;
}

/*Reset Core*/
int merlin16_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    return PHYMOD_E_NONE;
}
int merlin16_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
    return PHYMOD_E_NONE;
}

/*Retrive firmware information*/
int merlin16_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    return PHYMOD_E_NONE;
}

/*Start\Stop the sequencer*/
int merlin16_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    return PHYMOD_E_NONE;
}

/*Wait for core event*/
int merlin16_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{
    return PHYMOD_E_NONE;
}

/* re-tune rx path*/
int merlin16_phy_rx_restart(const phymod_phy_access_t* phy)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);

        PHYMOD_IF_ERR_RETURN(merlin16_rx_restart(&phy_copy.access, 1));
    }
    return PHYMOD_E_NONE;
}

/*Set phy polarity*/
int merlin16_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);

        PHYMOD_IF_ERR_RETURN
            (merlin16_polarity_set(&phy_copy.access, polarity->tx_polarity, polarity->rx_polarity));
    }

    return PHYMOD_E_NONE;
}
int merlin16_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    PHYMOD_IF_ERR_RETURN
        (merlin16_polarity_get(&phy->access, &polarity->tx_polarity, &polarity->rx_polarity));

    return PHYMOD_E_NONE;
}

/*Set\Get TX Parameters*/
int merlin16_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);

        PHYMOD_IF_ERR_RETURN
            (merlin16_apply_txfir_cfg(&phy_copy.access, (int8_t)tx->pre, (int8_t)tx->main, (int8_t)tx->post, (int8_t)tx->post2));
    }

    return PHYMOD_E_NONE;
}
int merlin16_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    int8_t value = 0;

    PHYMOD_IF_ERR_RETURN
        (merlin16_read_tx_afe(&phy->access, TX_AFE_PRE, &value));
    tx->pre = value;
    PHYMOD_IF_ERR_RETURN
        (merlin16_read_tx_afe(&phy->access, TX_AFE_MAIN, &value));
    tx->main = value;
    PHYMOD_IF_ERR_RETURN
        (merlin16_read_tx_afe(&phy->access, TX_AFE_POST1, &value));
    tx->post = value;
    PHYMOD_IF_ERR_RETURN
        (merlin16_read_tx_afe(&phy->access, TX_AFE_POST2, &value));
    tx->post2 = value;

    return PHYMOD_E_NONE;
}

/*Request for default TX parameters configuration per media type*/
int merlin16_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{
    switch (media) {
    case phymodMediaTypeChipToChip:
        tx->pre   = 0x0;
        tx->main  = 0x32;
        tx->post  = 0xa;
        tx->post2 = 0x0;
        break;
    case phymodMediaTypeShort:
        tx->pre   = 0x0;
        tx->main  = 0x32;
        tx->post  = 0xa;
        tx->post2 = 0x0;
        break;
    case phymodMediaTypeMid:
        tx->pre   = 0x0;
        tx->main  = 0x32;
        tx->post  = 0xa;
        tx->post2 = 0x0;
        break;
    case phymodMediaTypeLong:
        tx->pre   = 0x0;
        tx->main  = 0x32;
        tx->post  = 0xa;
        tx->post2 = 0x0;
        break;
    default:
        tx->pre   = 0x0;
        tx->main  = 0x32;
        tx->post  = 0xa;
        tx->post2 = 0x0;
        break;
    }

    return PHYMOD_E_NONE;
}


/*Set\Get TX override Parameters*/
int merlin16_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);

        PHYMOD_IF_ERR_RETURN
            (merlin16_tx_pi_freq_override(&phy_copy.access,
                                        tx_override->phase_interpolator.enable,
                                        tx_override->phase_interpolator.value));
    }

    return PHYMOD_E_NONE;
}
int merlin16_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
    return PHYMOD_E_NONE;
}

/*Set\Get RX Parameters*/
int merlin16_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    uint32_t i;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*params check*/
    if((rx->num_of_dfe_taps == 0) || (rx->num_of_dfe_taps < MERLIN16_NOF_DFES)){
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal number of DFEs to set %u"), rx->num_of_dfe_taps));
    }

    for (lane = 0; lane < num_lane; lane++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + lane)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + lane);

        /*vga set*/
        if (rx->vga.enable) {
            /* first stop the rx adaption */
            PHYMOD_IF_ERR_RETURN(merlin16_stop_rx_adaptation(&phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN(merlin16_write_rx_afe(&phy_copy.access, RX_AFE_VGA, rx->vga.value));
        } else {
            PHYMOD_IF_ERR_RETURN(merlin16_stop_rx_adaptation(&phy_copy.access, 0));
        }

        /*dfe set*/
        for (i = 0 ; i < rx->num_of_dfe_taps ; i++){
            if(rx->dfe[i].enable){
                PHYMOD_IF_ERR_RETURN(merlin16_stop_rx_adaptation(&phy_copy.access, 1));
                switch (i) {
                    case 0:
                        PHYMOD_IF_ERR_RETURN(merlin16_write_rx_afe(&phy_copy.access, RX_AFE_DFE1, rx->dfe[i].value));
                        break;
                    case 1:
                        PHYMOD_IF_ERR_RETURN(merlin16_write_rx_afe(&phy_copy.access, RX_AFE_DFE2, rx->dfe[i].value));
                        break;
                    case 2:
                        PHYMOD_IF_ERR_RETURN(merlin16_write_rx_afe(&phy_copy.access, RX_AFE_DFE3, rx->dfe[i].value));
                        break;
                    case 3:
                        PHYMOD_IF_ERR_RETURN(merlin16_write_rx_afe(&phy_copy.access, RX_AFE_DFE4, rx->dfe[i].value));
                        break;
                    case 4:
                        PHYMOD_IF_ERR_RETURN(merlin16_write_rx_afe(&phy_copy.access, RX_AFE_DFE5, rx->dfe[i].value));
                        break;
                    default:
                        return PHYMOD_E_PARAM;
                }
            } else {
                PHYMOD_IF_ERR_RETURN(merlin16_stop_rx_adaptation(&phy_copy.access, 0));
            }


        }

        /*peaking filter set*/
        if(rx->peaking_filter.enable){
            /* first stop the rx adaption */
            PHYMOD_IF_ERR_RETURN(merlin16_stop_rx_adaptation(&phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN(merlin16_write_rx_afe(&phy_copy.access, RX_AFE_PF, rx->peaking_filter.value));
        } else {
            PHYMOD_IF_ERR_RETURN(merlin16_stop_rx_adaptation(&phy_copy.access, 0));
        }

        if(rx->low_freq_peaking_filter.enable){
            /* first stop the rx adaption */
            PHYMOD_IF_ERR_RETURN(merlin16_stop_rx_adaptation(&phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN(merlin16_write_rx_afe(&phy_copy.access, RX_AFE_PF2, rx->low_freq_peaking_filter.value));
        } else {
            PHYMOD_IF_ERR_RETURN(merlin16_stop_rx_adaptation(&phy_copy.access, 0));
        }
    }

    return PHYMOD_E_NONE;
}

int merlin16_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    int8_t val;

    PHYMOD_IF_ERR_RETURN
        (merlin16_read_rx_afe(&phy->access, RX_AFE_PF, &val));
    rx->peaking_filter.value = val;

    PHYMOD_IF_ERR_RETURN
        (merlin16_read_rx_afe(&phy->access, RX_AFE_PF2, &val));
    rx->low_freq_peaking_filter.value = val;

    PHYMOD_IF_ERR_RETURN
        (merlin16_read_rx_afe(&phy->access, RX_AFE_VGA, &val));
    rx->vga.value = val;

    PHYMOD_IF_ERR_RETURN
        (merlin16_read_rx_afe(&phy->access, RX_AFE_DFE1, &val));
    rx->dfe[0].value = val;

    PHYMOD_IF_ERR_RETURN
        (merlin16_read_rx_afe(&phy->access, RX_AFE_DFE2, &val));
    rx->dfe[1].value = val;

    PHYMOD_IF_ERR_RETURN
        (merlin16_read_rx_afe(&phy->access, RX_AFE_DFE3, &val));
    rx->dfe[2].value = val;

    PHYMOD_IF_ERR_RETURN
        (merlin16_read_rx_afe(&phy->access, RX_AFE_DFE4, &val));
    rx->dfe[3].value = val;

    PHYMOD_IF_ERR_RETURN
        (merlin16_read_rx_afe(&phy->access, RX_AFE_DFE5, &val));
    rx->dfe[4].value = val;

    rx->num_of_dfe_taps = 5;
    rx->dfe[0].enable = 1;
    rx->dfe[1].enable = 1;
    rx->dfe[2].enable = 1;
    rx->dfe[3].enable = 1;
    rx->dfe[4].enable = 1;
    rx->vga.enable = 1;
    rx->low_freq_peaking_filter.enable = 1;
    rx->peaking_filter.enable = 1;

    return PHYMOD_E_NONE;
}


/*Reset phy*/
int merlin16_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
    uint32_t rst, rst_rx, rst_tx;
    int start_lane, num_lane, i;
    phymod_phy_access_t phy_copy;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    if (reset->rx == reset->tx) {
        PHYMOD_IF_ERR_RETURN(merlin16_tx_lane_soft_reset_release_get(&phy_copy.access, &rst_tx));
        PHYMOD_IF_ERR_RETURN(merlin16_rx_lane_soft_reset_release_get(&phy_copy.access, &rst_rx));
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 1 << (start_lane + i);
            /*If tx is in reset, release tx lane*/
            if (rst_tx == 0) {
                PHYMOD_IF_ERR_RETURN(merlin16_tx_lane_soft_reset_release(&phy_copy.access, 1));
                PHYMOD_USLEEP(10);
            }
            /*If rx is in reset, release rx lane*/
            if (rst_rx == 0) {
                PHYMOD_IF_ERR_RETURN(merlin16_rx_lane_soft_reset_release(&phy_copy.access, 1));
                PHYMOD_USLEEP(10);
            }
            switch (reset->tx) {
                case phymodResetDirectionIn:
                    PHYMOD_IF_ERR_RETURN(merlin16_lane_soft_reset_release(&phy_copy.access, 0));
                    break;
                case phymodResetDirectionOut:
                    PHYMOD_IF_ERR_RETURN(merlin16_lane_soft_reset_release(&phy_copy.access, 1));
                    break;
                case phymodResetDirectionInOut:
                    PHYMOD_IF_ERR_RETURN(merlin16_lane_soft_reset_release(&phy_copy.access, 0));
                    PHYMOD_USLEEP(10);
                    PHYMOD_IF_ERR_RETURN(merlin16_lane_soft_reset_release(&phy_copy.access, 1));
                    break;
                default:
                    break;
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN(merlin16_lane_soft_reset_release_get(&phy->access, &rst));
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 1 << (start_lane + i);
            /*If ln_rst is is in reset , release ln_rst first*/
            if (rst == 0) {
                PHYMOD_IF_ERR_RETURN(merlin16_lane_soft_reset_release(&phy_copy.access, 1));
                PHYMOD_USLEEP(10);
            }
            switch (reset->tx) {
                case phymodResetDirectionIn:
                    PHYMOD_IF_ERR_RETURN(merlin16_tx_lane_soft_reset_release(&phy_copy.access, 0));
                    break;
                case phymodResetDirectionOut:
                    PHYMOD_IF_ERR_RETURN(merlin16_tx_lane_soft_reset_release(&phy_copy.access, 1));
                    break;
                case phymodResetDirectionInOut:
                    PHYMOD_IF_ERR_RETURN(merlin16_tx_lane_soft_reset_release(&phy_copy.access, 0));
                    PHYMOD_USLEEP(10);
                    PHYMOD_IF_ERR_RETURN(merlin16_tx_lane_soft_reset_release(&phy_copy.access, 1));
                    break;
                default:
                    break;
            }
            switch (reset ->rx) {
                case phymodResetDirectionIn:
                    PHYMOD_IF_ERR_RETURN(merlin16_rx_lane_soft_reset_release(&phy_copy.access, 0));
                    break;
                case phymodResetDirectionOut:
                    PHYMOD_IF_ERR_RETURN(merlin16_rx_lane_soft_reset_release(&phy_copy.access, 1));
                    break;
                case phymodResetDirectionInOut:
                    PHYMOD_IF_ERR_RETURN(merlin16_rx_lane_soft_reset_release(&phy_copy.access, 0));
                    PHYMOD_USLEEP(10);
                    PHYMOD_IF_ERR_RETURN(merlin16_rx_lane_soft_reset_release(&phy_copy.access, 1));
                    break;
                default:
                    break;
            }
        }
    }

    return PHYMOD_E_NONE;
}


int merlin16_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
    uint32_t rst, rst_rx, rst_tx;
    PHYMOD_IF_ERR_RETURN(merlin16_lane_soft_reset_release_get(&phy->access, &rst));
    if(rst == 0) {
       reset->tx = phymodResetDirectionIn;
       reset->rx = phymodResetDirectionIn;
    } else {
       PHYMOD_IF_ERR_RETURN(merlin16_tx_lane_soft_reset_release_get(&phy->access, &rst_tx));
       PHYMOD_IF_ERR_RETURN(merlin16_rx_lane_soft_reset_release_get(&phy->access, &rst_rx));
       if (rst_tx == 0) {
           reset->tx = phymodResetDirectionIn;
       } else {
           reset->tx = phymodResetDirectionOut;
       }

       if(rst_rx == 0) {
           reset->rx = phymodResetDirectionIn;
       } else {
           reset->rx = phymodResetDirectionOut;
       }
    }

    return PHYMOD_E_NONE;
}


/*Control phy power*/
int merlin16_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    enum srds_core_pwrdn_mode_enum mode;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);

        if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerNoChange)) {
                /*disable tx on the PMD side */
                mode = PWRDN_TX;
                PHYMOD_IF_ERR_RETURN(merlin16_lane_pwrdn(&phy_copy.access, mode));
        }
        if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerNoChange)) {
                /*enable tx on the PMD side */
                PHYMOD_IF_ERR_RETURN(merlin16_pwrdn_set(&phy_copy.access, 1, 0));
        }
        if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOff)) {
                /*disable rx on the PMD side */
                mode = PWRDN_RX;
                PHYMOD_IF_ERR_RETURN(merlin16_lane_pwrdn(&phy_copy.access, mode));
        }
        if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOn)) {
                PHYMOD_IF_ERR_RETURN(merlin16_pwrdn_set(&phy_copy.access, 0, 0));
                /*enable rx on the PMD side */
        }
        if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerOn)) {
                mode = PWR_ON;
                PHYMOD_IF_ERR_RETURN(merlin16_lane_pwrdn(&phy_copy.access, mode));
        }
        if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerOff)) {
                /* Both Tx and Rx power down */
                mode = PWRDN;
                PHYMOD_IF_ERR_RETURN(merlin16_lane_pwrdn(&phy_copy.access, mode));
        }
    }
    return PHYMOD_E_NONE;
}


int merlin16_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    power_status_t pwrdn;
    PHYMOD_IF_ERR_RETURN(merlin16_pwrdn_get(&phy->access, &pwrdn));
    power->rx = (pwrdn.rx_s_pwrdn == 0)? phymodPowerOn: phymodPowerOff;
    power->tx = (pwrdn.tx_s_pwrdn == 0)? phymodPowerOn: phymodPowerOff;
    return PHYMOD_E_NONE;
}

/*TX transmission control*/
int merlin16_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    switch (tx_control)
    {
        case phymodTxSquelchOn:
            for (i = 0; i < num_lane; i++) {
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
                phy_copy.access.lane_mask = 1 << (start_lane + i);
                PHYMOD_IF_ERR_RETURN(merlin16_tx_disable(&phy_copy.access, 1));
            }
            break;
        case phymodTxSquelchOff:
            for (i = 0; i < num_lane; i++) {
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
                phy_copy.access.lane_mask = 1 << (start_lane + i);
                PHYMOD_IF_ERR_RETURN(merlin16_tx_disable(&phy_copy.access, 0));
            }
            break;
        default:
            PHYMOD_DEBUG_ERROR(("This control is NOT SUPPORTED!! (eagle_phy_tx_lane_control_set) \n"));
            break;
    }

    return PHYMOD_E_NONE;

}

int merlin16_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
    uint32_t enable;

    PHYMOD_IF_ERR_RETURN(merlin16_tx_disable_get(&phy->access, &enable));

    if(enable){
      *tx_control = phymodTxSquelchOn;
    } else {
      *tx_control = phymodTxSquelchOff;
    }

    return PHYMOD_E_NONE;

}

/*Rx control*/
int merlin16_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    switch (rx_control)
    {
        case phymodRxSquelchOn:
            for (i = 0; i < num_lane; i++) {
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
                phy_copy.access.lane_mask = 1 << (start_lane + i);
                PHYMOD_IF_ERR_RETURN(merlin16_rx_lane_control_set(&phy_copy.access, 1));
            }
            break;
        case phymodRxSquelchOff:
            for (i = 0; i < num_lane; i++) {
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
                phy_copy.access.lane_mask = 1 << (start_lane + i);
                PHYMOD_IF_ERR_RETURN(merlin16_rx_lane_control_set(&phy_copy.access, 0));
            }
            break;
        default:
            PHYMOD_DEBUG_ERROR(("This control is NOT SUPPORTED!! (eagle_phy_rx_lane_control_set) \n"));
            break;
    }

    return PHYMOD_E_NONE;
}


int merlin16_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{

    uint32_t enable;

    PHYMOD_IF_ERR_RETURN(merlin16_rx_lane_control_get(&phy->access, &enable));

    if(enable){
      *rx_control = phymodRxSquelchOn;
    } else {
      *rx_control = phymodRxSquelchOff;
    }

    return PHYMOD_E_NONE;

}

int merlin16_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_config)
{
    struct merlin16_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_MEMSET(&serdes_firmware_core_config, 0, sizeof(serdes_firmware_core_config));
    serdes_firmware_core_config.field.core_cfg_from_pcs = fw_config.CoreConfigFromPCS;
    serdes_firmware_core_config.field.vco_rate = fw_config.VcoRate;

    PHYMOD_IF_ERR_RETURN(merlin16_INTERNAL_set_uc_core_config(&phy->access, serdes_firmware_core_config));
    return PHYMOD_E_NONE;
}

int merlin16_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_config)
{
    struct merlin16_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_IF_ERR_RETURN(merlin16_get_uc_core_config(&phy->access, &serdes_firmware_core_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->CoreConfigFromPCS = serdes_firmware_core_config.field.core_cfg_from_pcs;
    fw_config->VcoRate = serdes_firmware_core_config.field.vco_rate;
    return PHYMOD_E_NONE;
}

int merlin16_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    struct merlin16_uc_lane_config_st serdes_firmware_config;
    PHYMOD_MEMSET(&serdes_firmware_config, 0x0, sizeof(serdes_firmware_config));

    PHYMOD_IF_ERR_RETURN(merlin16_get_uc_lane_cfg(&phy->access, &serdes_firmware_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->LaneConfigFromPCS = serdes_firmware_config.field.lane_cfg_from_pcs;
    fw_config->AnEnabled         = serdes_firmware_config.field.an_enabled;
    fw_config->DfeOn             = serdes_firmware_config.field.dfe_on;
    fw_config->ForceBrDfe        = serdes_firmware_config.field.force_brdfe_on;
    fw_config->Cl72AutoPolEn     = serdes_firmware_config.field.cl72_auto_polarity_en;
    fw_config->Cl72RestTO        = serdes_firmware_config.field.cl72_restart_timeout_en;
    fw_config->ScramblingDisable = serdes_firmware_config.field.scrambling_dis;
    fw_config->UnreliableLos     = serdes_firmware_config.field.unreliable_los;
    fw_config->MediaType         = serdes_firmware_config.field.media_type;
    fw_config->Cl72AutoPolEn     = serdes_firmware_config.field.cl72_auto_polarity_en;
    fw_config->Cl72RestTO        = serdes_firmware_config.field.cl72_restart_timeout_en;

    return PHYMOD_E_NONE;
}

STATIC
int _merlin16_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    struct merlin16_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    /* uint32_t rst_status; */
    uint32_t is_warm_boot;

    PHYMOD_MEMSET(&serdes_firmware_config, 0x0, sizeof(serdes_firmware_config));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        serdes_firmware_config.field.lane_cfg_from_pcs = fw_config.LaneConfigFromPCS;
        serdes_firmware_config.field.an_enabled        = fw_config.AnEnabled;
        serdes_firmware_config.field.dfe_on            = fw_config.DfeOn;
        serdes_firmware_config.field.force_brdfe_on    = fw_config.ForceBrDfe;
        /* serdes_firmware_config.field.cl72_emulation_en = fw_config.Cl72Enable; */
        serdes_firmware_config.field.scrambling_dis    = fw_config.ScramblingDisable;
        serdes_firmware_config.field.unreliable_los    = fw_config.UnreliableLos;
        serdes_firmware_config.field.media_type        = fw_config.MediaType;
        serdes_firmware_config.field.cl72_auto_polarity_en   = fw_config.Cl72AutoPolEn;
        serdes_firmware_config.field.cl72_restart_timeout_en = fw_config.Cl72RestTO;

        PHYMOD_IF_ERR_RETURN(PHYMOD_IS_WRITE_DISABLED(&phy_copy.access, &is_warm_boot));

        if (!is_warm_boot) {
            /* PHYMOD_IF_ERR_RETURN(merlin16_lane_soft_reset_read(&phy_copy.access, &rst_status)); */
            /* if (rst_status) */ PHYMOD_IF_ERR_RETURN (merlin16_lane_soft_reset_release(&phy_copy.access, 0));
            PHYMOD_IF_ERR_RETURN(merlin16_set_uc_lane_cfg(&phy_copy.access, serdes_firmware_config));
            /* if (rst_status) */ PHYMOD_IF_ERR_RETURN (merlin16_lane_soft_reset_release(&phy_copy.access, 1));
        }
    }

    return PHYMOD_E_NONE;
}

int merlin16_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&phy_copy.access, 0));
    }

    PHYMOD_IF_ERR_RETURN
         (_merlin16_phy_firmware_lane_config_set(phy, fw_config));
    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&phy_copy.access, 1));
    }

    return PHYMOD_E_NONE;
}


/*Set the interface mode, speed and other configuration related to interface.*/
int merlin16_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    uint32_t new_pll_div;
    int16_t  new_os_mode = 0;
    phymod_phy_access_t pm_phy_copy;
    int start_lane=0, num_lane, i;
    uint32_t current_pll_div;
    phymod_firmware_lane_config_t firmware_lane_config;
    enum merlin16_pll_refclk_enum refclk = MERLIN16_PLL_REFCLK_156P25MHZ;

#ifdef PHYMOD_DIAG
    const phymod_access_t *pm_acc;
    pm_acc = &phy->access;
    PHYMOD_VDBG(DBG_SPD, pm_acc, ("%-22s: p=%p adr=%0"PRIx32" lmask=%0"PRIx32" speed=%0d intf=%0d(%s) flags=%0x\n",
      __func__, (void *)pm_acc, pm_acc->addr, pm_acc->lane_mask, config->data_rate, config->interface_type,
                                  phymod_interface_t_mapping[config->interface_type].key, flags));
#endif

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));

    /* end of special mode */
    firmware_lane_config.MediaType = 0;

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));


    /* Hold the per lne soft reset bit */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&pm_phy_copy.access, 0));
    }

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (merlin16_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config));

    firmware_lane_config.AnEnabled = 0;
    firmware_lane_config.LaneConfigFromPCS = 0;
    firmware_lane_config.DfeOn = 1;
    firmware_lane_config.LpDfeOn = 0;
    firmware_lane_config.Cl72RestTO = 1;
    if (config->data_rate < 10000) {
        firmware_lane_config.DfeOn = 0;
    }
    if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
    } else if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
    } else {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
    }

    /* get the current PLL setting */
    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (merlin16_INTERNAL_read_pll_div(&pm_phy_copy.access, &current_pll_div));

    /*Get new pll div through ref_clk and data rate*/
    PHYMOD_IF_ERR_RETURN
        (merlin16_get_pll_vco_osmode(config, &new_pll_div, &new_os_mode));

    /* if pll change is enabled. new_pll_div is the reg vector value */
    if ((current_pll_div != new_pll_div) && (PHYMOD_INTF_F_DONT_TURN_OFF_PLL & flags)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                               (_PHYMOD_MSG("pll has to change for speed_set from %u to %u but DONT_TURN_OFF_PLL flag is enabled"),
                                 (unsigned int)current_pll_div, (unsigned int)new_pll_div));
    }

    /* pll switch is required and expected */
    if((current_pll_div != new_pll_div) && !(PHYMOD_INTF_F_DONT_TURN_OFF_PLL & flags)) {
        PHYMOD_IF_ERR_RETURN
            (merlin16_core_soft_reset_release(&pm_phy_copy.access, 0));

        switch (config->ref_clock) {
            case phymodRefClk156Mhz:
                 refclk = MERLIN16_PLL_REFCLK_156P25MHZ;
                 break;
            case phymodRefClk125Mhz:
                 refclk = MERLIN16_PLL_REFCLK_125MHZ;
                 break;
            case phymodRefClk161Mhz:
                 refclk = MERLIN16_PLL_REFCLK_161P1328125MHZ;
                 break;
            default:
                 refclk = MERLIN16_PLL_REFCLK_156P25MHZ;
                 break;
        }
        /* set the PLL divider */
        PHYMOD_IF_ERR_RETURN
            (merlin16_configure_pll_refclk_div(&pm_phy_copy.access, refclk, new_pll_div));

        PHYMOD_IF_ERR_RETURN
            (merlin16_core_soft_reset_release(&pm_phy_copy.access, 1));
    }

    /*Set os mode*/
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
             (merlin16_osmode_set(&pm_phy_copy.access, new_os_mode));
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
             (_merlin16_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }

    /* release the per lne soft reset bit */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }

    return PHYMOD_E_NONE;
}

STATIC
int _merlin16_pll_multiplier_get(uint32_t pll_div, uint32_t *pll_multiplier)
{
    switch (pll_div) {
        case MERLIN16_PLL_DIV_70: *pll_multiplier = 70;
            break;
        case MERLIN16_PLL_DIV_66: *pll_multiplier = 66;
            break;
        case MERLIN16_PLL_DIV_64: *pll_multiplier = 64;
            break;
        case MERLIN16_PLL_DIV_60: *pll_multiplier = 60;
            break;
        case MERLIN16_PLL_DIV_92: *pll_multiplier = 92;
            break;
        case MERLIN16_PLL_DIV_80: *pll_multiplier = 80;
            break;
        case MERLIN16_PLL_DIV_54P4: *pll_multiplier = 54.4;
            break;
        default: return PHYMOD_E_NONE;
            break;
    }
    return PHYMOD_E_NONE;
}

STATIC
int _merlin16_actual_osr_get(int16_t osr_mode, int* actual_os)
{
    switch(osr_mode) {
        case 0: *actual_os = 1; break;
        case 1: *actual_os = 2; break;
        case 2: *actual_os = 3; break;
        case 4: *actual_os = 4; break;
        case 5: *actual_os = 5; break;
        case 7: *actual_os = 8; break;
        case 8: *actual_os = 8 + 1/4; break;
        case 9: *actual_os = 10; break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("unsupported OS mode %d"), osr_mode));
    }
    return PHYMOD_E_NONE;
}


int merlin16_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{
    int osr_mode;
    uint32_t pll_div;
    uint32_t pll_multiplier = 1, refclk;
    phymod_phy_access_t phy_copy;
    int actual_osr;

    config->ref_clock = ref_clock;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (merlin16_osr_mode_get(&phy->access, &osr_mode));
    PHYMOD_IF_ERR_RETURN
        (merlin16_INTERNAL_read_pll_div(&phy_copy.access, &pll_div));
    PHYMOD_IF_ERR_RETURN
        (_merlin16_pll_multiplier_get(pll_div, &pll_multiplier));

    if (ref_clock == phymodRefClk156Mhz) {
        refclk = 15625;
    } else if (ref_clock == phymodRefClk125Mhz) {
        refclk = 12500;
    } else {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL,  (_PHYMOD_MSG("Unknown refclk")));
    }

    PHYMOD_IF_ERR_RETURN(_merlin16_actual_osr_get(osr_mode, &actual_osr));

    config->data_rate = (refclk*pll_multiplier)/(100*actual_osr);
    config->interface_type = phymodInterfaceBypass;

    if(osr_mode == 1) {
        PHYMOD_INTF_MODES_OS2_SET(config);
    }

    return PHYMOD_E_NONE;
}


/*Set\Get CL72*/
int merlin16_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    struct merlin16_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
#ifdef PHYMOD_DIAG
    const phymod_access_t *pm_acc;
#endif    

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

#ifdef PHYMOD_DIAG
    pm_acc = &phy->access;
    PHYMOD_VDBG(DBG_CL72, pm_acc, ("%-22s: p=%p adr=%0"PRIx32" lmask=%0"PRIx32" cl72_en=%d\n",
                __func__, (void *)pm_acc, pm_acc->addr, pm_acc->lane_mask, (int)cl72_en));
#endif

    PHYMOD_IF_ERR_RETURN(merlin16_get_uc_lane_cfg(&phy->access, &serdes_firmware_config));

    if (serdes_firmware_config.field.dfe_on == 0) {
      PHYMOD_DEBUG_ERROR(("ERROR :: DFE is off : Can not start CL72 with no DFE\n"));
      return PHYMOD_E_CONFIG;
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (merlin16_clause72_control(&phy_copy.access, cl72_en));
    }
    return PHYMOD_E_NONE;
}

int merlin16_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    uint32_t local_en;;
    PHYMOD_IF_ERR_RETURN
        (merlin16_pmd_cl72_enable_get(&phy->access, &local_en));
    *cl72_en = local_en;
    return PHYMOD_E_NONE;
}


/*Get CL72 status*/
int merlin16_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    uint32_t local_status;
    PHYMOD_IF_ERR_RETURN
        (merlin16_pmd_cl72_receiver_status(&phy->access, &local_status));
    status->locked = local_status;
    return PHYMOD_E_NONE;
}

/*Set\Get autoneg*/
int merlin16_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability_set_type)
{
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (merlin16_phy_autoneg_ability_set) \n"));
    return PHYMOD_E_NONE;

}
int merlin16_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (merlin16_phy_autoneg_ability_get) \n"));

    return PHYMOD_E_NONE;
}

/*Set\Get autoneg*/
int merlin16_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (merlin16_phy_autoneg_set) \n"));

    return PHYMOD_E_NONE;
}
int merlin16_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (merlin16_phy_autoneg_get) \n"));

    return PHYMOD_E_NONE;
}

/*Get Autoneg status*/
int merlin16_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (merlin16_phy_autoneg_status_get) \n"));

    return PHYMOD_E_NONE;
}

/* load tsce fw. the fw_loader parameter is valid just for external fw load */
STATIC
int _merlin16_core_firmware_load(const phymod_core_access_t* core, phymod_firmware_load_method_t load_method, phymod_firmware_loader_f fw_loader)
{
#ifdef PHYMOD_DIAG
    const phymod_access_t *pm_acc;
    pm_acc = &core->access;
    PHYMOD_VDBG(DBG_CFG, pm_acc,
       ("%-22s: p=%p adr=%0"PRIx32" lmask=%0"PRIx32" load_meth=%0d",
        __func__, (void *)pm_acc, pm_acc->addr, pm_acc->lane_mask, (int)load_method));
#endif

    switch (load_method) {
    case phymodFirmwareLoadMethodInternal:
        PHYMOD_IF_ERR_RETURN(merlin16_ucode_mdio_load(&core->access, merlin16_ucode, merlin16_ucode_len));
        break;
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_NULL_CHECK(fw_loader);
        PHYMOD_IF_ERR_RETURN(merlin16_ucode_pram_load_pre(&core->access));
        PHYMOD_IF_ERR_RETURN(fw_loader(core, merlin16_ucode_len, merlin16_ucode));
        PHYMOD_IF_ERR_RETURN(merlin16_ucode_pram_load_post(&core->access));
        break;
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), load_method));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _merlin16_core_init_pass1(const phymod_core_access_t* core,
                          const phymod_core_init_config_t* init_config,
                          const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    uint32_t  uc_active = 0;
    int i, start_lane, num_lane;

    MERLIN16_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    phy_access_copy = phy_access;
    phy_access_copy.access = core->access;
    phy_access_copy.access.lane_mask = 0x1;
    phy_access_copy.type = core->type;

    /*
     * First step to program merlin16 is to do power reset. That is to
     * program field POR_H_RSTB and CORE_DP_H_RSTB in register PMD_X1_CTL
     */
    PHYMOD_IF_ERR_RETURN
        (merlin16_pmd_reset_seq(&core_copy.access, core_status->pmd_active));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy_access.access, &start_lane, &num_lane));

    /*
     * Before programming the PMD lane address map register, the PMD lanes
     * have to be reset. Without do this, writing the PMD lane address map
     * regsiter will not take effect, meaning the reading value != writing
     * value.
     */
    for (i = 0; i < MERLIN16_NOF_LANES_IN_CORE; i++) {
        phy_access.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_hard_soft_reset_release(&phy_access.access, 0));
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&phy_access.access, 0));

        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_hard_soft_reset_release(&phy_access.access, 1));
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&phy_access.access, 1));
    }

    PHYMOD_IF_ERR_RETURN(merlin16_uc_active_get(&core_copy.access, &uc_active));
    if (uc_active) {
        return(PHYMOD_E_NONE);
    }

    /* need to set the heart beat default is for 156.25M */
    PHYMOD_IF_ERR_RETURN (merlin16_refclk_set(&core_copy.access, init_config->interface.ref_clock));

    /*
     * In 28nm TSCE code the lane map programming was done in core_init_pass2.
     * Now it is moved to pass1 because merlin16 requires that PMD lane address
     * map be done before loading the uCode. Notice that before programming the
     * PMD lane address map, lane and lane DP has to be reset by calling
     * temod16_pmd_x4_reset().
     */
    PHYMOD_IF_ERR_RETURN
        (merlin16_core_lane_map_set(&core_copy, &init_config->lane_map));

    if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        if (_merlin16_core_firmware_load(&core_copy, init_config->firmware_load_method, init_config->firmware_loader)) {
            PHYMOD_DEBUG_ERROR(("devad 0x%"PRIx32" lane 0x%"PRIx32": UC firmware-load failed\n", core->access.addr, core->access.lane_mask));
            PHYMOD_IF_ERR_RETURN (PHYMOD_E_INIT);
        }

    }

    PHYMOD_IF_ERR_RETURN
        (merlin16_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x1));
    PHYMOD_IF_ERR_RETURN
        (merlin16_uc_reset(&phy_access_copy.access, 0));
    PHYMOD_IF_ERR_RETURN
        (merlin16_wait_uc_active(&phy_access_copy.access));

    /* Initialize software information table for the micro */
    PHYMOD_IF_ERR_RETURN
        (merlin16_init_merlin16_info(&core_copy.access));

    if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        if (PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
            PHYMOD_IF_ERR_RETURN
                (merlin16_start_ucode_crc_calc(&core_copy.access, merlin16_ucode_len));
        }
    }

    return (PHYMOD_E_NONE);
}

STATIC
int _merlin16_core_init_pass2(const phymod_core_access_t* core,
                            const phymod_core_init_config_t* init_config,
                            const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    phymod_firmware_core_config_t  firmware_core_config_tmp;

    MERLIN16_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    phy_access_copy = phy_access;
    phy_access_copy.access = core->access;
    phy_access_copy.access.lane_mask = 0x1;
    phy_access_copy.type = core->type;

    if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        if (PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
            PHYMOD_IF_ERR_RETURN
                (merlin16_check_ucode_crc(&core_copy.access, merlin16_ucode_crc, 250));
        }
    }

    PHYMOD_IF_ERR_RETURN(
        merlin16_pmd_ln_h_rstb_pkill_override( &phy_access_copy.access, 0x0));

    phy_access_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (merlin16_core_soft_reset_release(&core_copy.access, 0));

    /* After loading the PMD uCode, PLL needs to be configured. */
    PHYMOD_IF_ERR_RETURN
        (merlin16_configure_pll_refclk_div(&phy_access_copy.access, MERLIN16_PLL_REFCLK_156P25MHZ,
                                               MERLIN16_PLL_DIV_66));

    PHYMOD_IF_ERR_RETURN
        (merlin16_phy_firmware_core_config_get(&phy_access_copy, &firmware_core_config_tmp));
    firmware_core_config_tmp.CoreConfigFromPCS = 0;

    PHYMOD_IF_ERR_RETURN
        (merlin16_phy_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp));

    /*
     * After programming the PMD PLL, we need to release the core reset and lane reset.
     * Core reset is done below, and lane reset is done in tsce16_phy_init() by calling
     * temod16_pmd_x4_reset().
     */
    PHYMOD_IF_ERR_RETURN
        (merlin16_core_soft_reset_release(&core_copy.access, 1));

    return PHYMOD_E_NONE;
}

/*Core Initialization*/
int merlin16_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_merlin16_core_init_pass1(core, init_config, core_status));

        if (PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
            return PHYMOD_E_NONE;
        }
    }

    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_merlin16_core_init_pass2(core, init_config, core_status));
    }

    return PHYMOD_E_NONE;

}


/*Phy Initialization*/
int merlin16_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;
    phymod_polarity_t tmp_pol;
#ifdef PHYMOD_DIAG
    PHYMOD_VDBG(DBG_CFG, pm_acc, ("%-22s: p=%p adr=%0"PRIx32" lmask=%0"PRIx32"\n",
                __func__, (void *)pm_acc, pm_acc->addr, pm_acc->lane_mask));
#endif
    PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));

    /* per lane based reset release */
    for (i = 0; i < num_lane; i++) {
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_hard_soft_reset_release(&pm_phy_copy.access, 0));
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_hard_soft_reset_release(&pm_phy_copy.access, 1));

        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&pm_phy_copy.access, 0));
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }

    /* program the rx/tx polarity */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        tmp_pol.tx_polarity = (init_config->polarity.tx_polarity) >> i & 0x1;
        tmp_pol.rx_polarity = (init_config->polarity.rx_polarity) >> i & 0x1;
        PHYMOD_IF_ERR_RETURN
            (merlin16_phy_polarity_set(&pm_phy_copy, &tmp_pol));
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (merlin16_phy_tx_set(&pm_phy_copy, &init_config->tx[i]));
    }

    /* next check if pcs-bypass mode  */
    if (PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        PHYMOD_IF_ERR_RETURN
            (merlin16_pmd_tx_disable_pin_dis_set(&phy->access, 1));
    }

    return PHYMOD_E_NONE;
}


/*Set\get loopback mode*/
int merlin16_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    int start_lane, num_lane, i;
    phymod_phy_access_t phy_copy;
    const phymod_access_t *pm_acc;

    pm_acc = &phy->access;
#ifdef PHYMOD_DIAG
    PHYMOD_VDBG(DBG_LPK, pm_acc, ("%-22s: p=%p adr=%0"PRIx32" lmask=%0"PRIx32" lpbk=%0d(%s) en=%0d\n",
              __func__, (void *)pm_acc, pm_acc->addr, pm_acc->lane_mask, loopback,
                            phymod_loopback_mode_t_mapping[loopback].key, enable));
#endif
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal:
        break;
    case phymodLoopbackGlobalPMD:
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(merlin16_pmd_loopback_set(&phy->access, enable));
        }
        break;
    case phymodLoopbackRemotePMD:
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            PHYMOD_IF_ERR_RETURN(merlin16_rmt_lpbk(&phy->access, enable));
        }
        break;
    case phymodLoopbackRemotePCS:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                               (_PHYMOD_MSG("PCS Remote LoopBack not supported")));
        break;
    default:
        break;
    }

    return PHYMOD_E_NONE;
}

int merlin16_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    *enable = 0;

    switch (loopback) {
    case phymodLoopbackGlobal :
        break;
    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN(merlin16_pmd_loopback_get(&phy->access, enable));
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(merlin16_rmt_lpbk_get(&phy->access, enable));
        break;
    case phymodLoopbackRemotePCS :
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                               (_PHYMOD_MSG("PCS Remote LoopBack not supported")));
        break;
    default :
        break;
    }
    return PHYMOD_E_NONE;
}


/*Get rx pmd locked indication*/
int merlin16_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{
    uint8_t rx_lock;
    PHYMOD_IF_ERR_RETURN(merlin16_pmd_lock_status(&phy->access, &rx_lock));
    *rx_pmd_locked = (uint32_t)rx_lock;

    return PHYMOD_E_NONE;
}

/*Get link up status indication*/
int merlin16_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    unsigned char rx_lock;
    PHYMOD_IF_ERR_RETURN(merlin16_pmd_lock_status(&phy->access, &rx_lock));
    *link_status = (uint32_t ) rx_lock;

    return PHYMOD_E_NONE;
}

/*Read phymod register*/
int merlin16_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_read(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;
}


/*Write phymod register*/
int merlin16_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_write(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;
}

