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
#include <phymod/phymod_util.h>
#include <phymod/phymod_dispatch.h>
#include "falcon_dpll/tier1/falcon2_monterey_interface.h"
#include "falcon_dpll/tier1/falcon_dpll_cfg_seq.h"

#ifdef PHYMOD_FALCON_DPLL_SUPPORT


int falcon_dpll_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_PRE, (int8_t)tx->pre));
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_MAIN, (int8_t)tx->main));
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_POST1, (int8_t)tx->post));
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_POST2, (int8_t)tx->post2));
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_POST3, (int8_t)tx->post3));
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_AMP,  (int8_t)tx->amp));
    if((tx->drivermode != -1) &&
       (phy->device_op_mode & PHYMOD_INTF_CONFIG_TX_FIR_DRIVERMODE_ENABLE)) {
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_DRIVERMODE,  (int8_t)tx->drivermode));
    }

    return PHYMOD_E_NONE;
}

int falcon_dpll_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    int8_t value = 0;

    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_PRE, &value));
    tx->pre = value;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_MAIN, &value));
    tx->main = value;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_POST1, &value));
    tx->post = value;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_POST2, &value));
    tx->post2 = value;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_POST3, &value));
    tx->post3 = value;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_AMP, &value));
    tx->amp = value;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_DRIVERMODE, &value));
    tx->drivermode = value;

    return PHYMOD_E_NONE;
}


/* NEED TO REVISIT: need to update these settings */
int falcon_dpll_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{        
    switch (media) {
    case phymodMediaTypeChipToChip:
      tx->pre   = 0xc;
      tx->main  = 0x64;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    case phymodMediaTypeShort:
      tx->pre   = 0xc;
      tx->main  = 0x64;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    case phymodMediaTypeMid:
      tx->pre   = 0xc;
      tx->main  = 0x64;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    case phymodMediaTypeLong:
      tx->pre   = 0xc;
      tx->main  = 0x64;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    default:
      tx->pre   = 0xc;
      tx->main  = 0x64;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    }

    return PHYMOD_E_NONE;
}

int falcon_dpll_phy_txpi_config_set(const phymod_phy_access_t* phy, const phymod_txpi_config_t* config)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
         (falcon2_monterey_tx_pi_enable_set(&phy_copy.access, config->enable));

    PHYMOD_IF_ERR_RETURN
         (falcon2_monterey_tx_pi_ext_pd_select_set(&phy_copy.access, config->mode == PHYMOD_TXPI_EXT_PD_MODE ? 1 : 0));

    return PHYMOD_E_NONE;
}

int falcon_dpll_phy_txpi_config_get(const phymod_phy_access_t* phy, phymod_txpi_config_t* config)
{
    phymod_phy_access_t phy_copy;
    uint32_t ext_pd;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
         (falcon2_monterey_tx_pi_enable_get(&phy_copy.access, &config->enable));

    PHYMOD_IF_ERR_RETURN
         (falcon2_monterey_tx_pi_ext_pd_select_get(&phy_copy.access, &ext_pd));

    config->mode = ext_pd ? PHYMOD_TXPI_EXT_PD_MODE : PHYMOD_TXPI_NORMAL_MODE;

    return PHYMOD_E_NONE;
}

int falcon_dpll_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{

    switch (tx_control) {
    case phymodTxSquelchOn:
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_tx_squelch_set(&phy->access, 1));
        break;
    case phymodTxSquelchOff:
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_tx_squelch_set(&phy->access, 0));
        break;
    case phymodTxElectricalIdleEnable:
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_electrical_idle_set(&phy->access, 1));
        break;
    case phymodTxElectricalIdleDisable:
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_electrical_idle_set(&phy->access, 0));
        break;
    default:
        break;
    }

    return PHYMOD_E_NONE;
}

int falcon_dpll_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
    int enable;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN(falcon2_monterey_tx_squelch_get(&pm_phy_copy.access, &enable));

    /* Check if PMD loopback is on */
    if (enable) {
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) {
            enable = 0;
        }
    }
    if (enable) {
        *tx_control = phymodTxSquelchOn;
    } else {
        *tx_control = phymodTxSquelchOff;
    }

    return PHYMOD_E_NONE;
}

int falcon_dpll_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    switch (rx_control) {
    case phymodRxSquelchOn:
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_rx_squelch_set(&pm_phy_copy.access, 1));
        break;
    case phymodRxSquelchOff:
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_rx_squelch_set(&pm_phy_copy.access, 0));
        break;
    default:
        break;
    }

    return PHYMOD_E_NONE;
}

int falcon_dpll_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    int enable;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN(falcon2_monterey_rx_squelch_get(&pm_phy_copy.access, &enable));
    /* Check if PMD loopback is on */
    if (enable) {
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) {
            enable = 0;
        }
    }
    if (enable) {
        *rx_control = phymodRxSquelchOn;
    } else {
        *rx_control = phymodRxSquelchOff;
    }

    return PHYMOD_E_NONE;
}

/*
 * falcon2_monterey_lane_soft_reset_release() need to be call before and after calling this routine.
 */

STATIC
int _falcon_dpll_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    struct falcon2_monterey_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;
    int start_lane;
    int num_lane;
    int lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_MEMSET(&serdes_firmware_config, 0x0, sizeof(serdes_firmware_config));

    serdes_firmware_config.field.lane_cfg_from_pcs = fw_config.LaneConfigFromPCS;
    serdes_firmware_config.field.an_enabled        = fw_config.AnEnabled;
    serdes_firmware_config.field.dfe_on            = fw_config.DfeOn;
    serdes_firmware_config.field.dfe_lp_mode       = fw_config.LpDfeOn;
    serdes_firmware_config.field.force_brdfe_on    = fw_config.ForceBrDfe;
    serdes_firmware_config.field.media_type        = fw_config.MediaType;
    serdes_firmware_config.field.unreliable_los    = fw_config.UnreliableLos;
    serdes_firmware_config.field.scrambling_dis    = fw_config.ScramblingDisable;
    serdes_firmware_config.field.cl72_auto_polarity_en   = fw_config.Cl72AutoPolEn;
    serdes_firmware_config.field.cl72_restart_timeout_en = fw_config.Cl72RestTO;

    for (lane = 0; lane < num_lane; lane++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + lane)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + lane);
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_set_uc_lane_cfg(&phy_copy.access, serdes_firmware_config));
    }
    return PHYMOD_E_NONE;
}

int falcon_dpll_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_lane_config)
{
    phymod_phy_access_t phy_copy;
    int start_lane;
    int num_lane;
    int lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* Hold the per lane soft reset bit */
    for (lane = 0; lane < num_lane; lane++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + lane)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + lane);
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_lane_soft_reset_release(&phy_copy.access, 0));
    }

    PHYMOD_IF_ERR_RETURN
         (_falcon_dpll_phy_firmware_lane_config_set(phy, fw_lane_config));

    /* Release the per lane soft reset bit */
    for (lane = 0; lane < num_lane; lane++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + lane)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + lane);
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_lane_soft_reset_release(&phy_copy.access, 1));
    }

    return PHYMOD_E_NONE;
}

int falcon_dpll_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_lane_config)
{
    struct falcon2_monterey_uc_lane_config_st serdes_firmware_config;

    PHYMOD_MEMSET(&serdes_firmware_config, 0x0, sizeof(serdes_firmware_config));
    PHYMOD_IF_ERR_RETURN(falcon2_monterey_get_uc_lane_cfg(&phy->access, &serdes_firmware_config));
    PHYMOD_MEMSET(fw_lane_config, 0, sizeof(*fw_lane_config));
    fw_lane_config->LaneConfigFromPCS = serdes_firmware_config.field.lane_cfg_from_pcs;
    fw_lane_config->AnEnabled         = serdes_firmware_config.field.an_enabled;
    fw_lane_config->DfeOn             = serdes_firmware_config.field.dfe_on;
    fw_lane_config->LpDfeOn           = serdes_firmware_config.field.dfe_lp_mode;
    fw_lane_config->ForceBrDfe        = serdes_firmware_config.field.force_brdfe_on;
    fw_lane_config->ScramblingDisable = serdes_firmware_config.field.scrambling_dis;
    fw_lane_config->UnreliableLos     = serdes_firmware_config.field.unreliable_los;
    fw_lane_config->MediaType         = serdes_firmware_config.field.media_type;
    fw_lane_config->Cl72AutoPolEn     = serdes_firmware_config.field.cl72_auto_polarity_en;
    fw_lane_config->Cl72RestTO        = serdes_firmware_config.field.cl72_restart_timeout_en;

    return PHYMOD_E_NONE;
}
#endif /* PHYMOD_FALCON_DPLL_SUPPORT */
