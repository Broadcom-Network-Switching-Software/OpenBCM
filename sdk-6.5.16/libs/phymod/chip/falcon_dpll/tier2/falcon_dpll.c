/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
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
      tx->main  = 0x66;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    case phymodMediaTypeShort:
      tx->pre   = 0xc;
      tx->main  = 0x66;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    case phymodMediaTypeMid:
      tx->pre   = 0xc;
      tx->main  = 0x66;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    case phymodMediaTypeLong:
      tx->pre   = 0xc;
      tx->main  = 0x66;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    default:
      tx->pre   = 0xc;
      tx->main  = 0x66;
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

#endif /* PHYMOD_FALCON_DPLL_SUPPORT */
