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
#include <phymod/chip/bcmi_eagle_xgxs_defs.h>
#include <phymod/chip/eagle_common.h>
#include <phymod/chip/eagle.h>

#include "../../eagle/tier1/eagle_tsc_enum.h" 
#include "../../eagle/tier1/eagle_cfg_seq.h"
#include "../../eagle/tier1/eagle_tsc_common.h"
#include "../../eagle/tier1/eagle_tsc_interface.h"
#include "../../eagle/tier1/eagle_tsc_dependencies.h"
/* #include "../../eagle/tier1/eagle_tsc_common_enum.h"   */

#define EAGLE_ID0        0
#define EAGLE_ID1        0
#define EAGLE_MODEL      0x1a
#define EAGLE_REV_MASK   0x0
#define TSCE_PHY_ALL_LANES 0xf
#define TSCE_PMD_CRC_UCODE 0
 
#define TSCE_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type           = (_core_access)->type; \
        (_phy_access)->port_loc       = (_core_access)->port_loc; \
        (_phy_access)->device_op_mode = (_core_access)->device_op_mode; \
        (_phy_access)->access.lane_mask = TSCE_PHY_ALL_LANES; \
    }while(0)

#define TSCE_NOF_DFES 9
#define TSCE_NOF_LANES_IN_CORE 4
extern unsigned char  tsce_ucode[];
extern unsigned short tsce_ucode_len;



int eagle_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_config)
{
    struct eagle_tsc_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_MEMSET(&serdes_firmware_core_config, 0, sizeof(serdes_firmware_core_config));
    serdes_firmware_core_config.field.core_cfg_from_pcs = fw_config.CoreConfigFromPCS;
    serdes_firmware_core_config.field.vco_rate = fw_config.VcoRate;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_set_uc_core_config(&phy->access, serdes_firmware_core_config));
    return PHYMOD_E_NONE;
}


int eagle_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_config)
{
    struct eagle_tsc_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_get_uc_core_config(&phy->access, &serdes_firmware_core_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->CoreConfigFromPCS = serdes_firmware_core_config.field.core_cfg_from_pcs;
    fw_config->VcoRate = serdes_firmware_core_config.field.vco_rate;
    return PHYMOD_E_NONE;
}


int eagle_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    struct eagle_tsc_uc_lane_config_st serdes_firmware_config;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_get_uc_lane_cfg(&phy->access, &serdes_firmware_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->LaneConfigFromPCS = serdes_firmware_config.field.lane_cfg_from_pcs;
    fw_config->AnEnabled         = serdes_firmware_config.field.an_enabled;
    fw_config->DfeOn             = serdes_firmware_config.field.dfe_on;
    fw_config->ForceBrDfe        = serdes_firmware_config.field.force_brdfe_on;
    fw_config->Cl72AutoPolEn        = serdes_firmware_config.field.cl72_auto_polarity_en;
    fw_config->Cl72RestTO      = serdes_firmware_config.field.cl72_restart_timeout_en;
    fw_config->ScramblingDisable = serdes_firmware_config.field.scrambling_dis;
    fw_config->UnreliableLos     = serdes_firmware_config.field.unreliable_los;
    fw_config->MediaType         = serdes_firmware_config.field.media_type;

    return PHYMOD_E_NONE;
}

static int _eagle_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    struct eagle_tsc_uc_lane_config_st serdes_firmware_config;
    serdes_firmware_config.field.lane_cfg_from_pcs = fw_config.LaneConfigFromPCS;
    serdes_firmware_config.field.an_enabled        = fw_config.AnEnabled;
    serdes_firmware_config.field.dfe_on            = fw_config.DfeOn; 
    serdes_firmware_config.field.force_brdfe_on    = fw_config.ForceBrDfe;
    serdes_firmware_config.field.cl72_auto_polarity_en = fw_config.Cl72AutoPolEn;
    serdes_firmware_config.field.cl72_restart_timeout_en = fw_config.Cl72RestTO;
    serdes_firmware_config.field.scrambling_dis    = fw_config.ScramblingDisable;
    serdes_firmware_config.field.unreliable_los    = fw_config.UnreliableLos;
    serdes_firmware_config.field.media_type        = fw_config.MediaType;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_set_uc_lane_cfg(&phy->access, serdes_firmware_config));
    return PHYMOD_E_NONE;
}

int eagle_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
  PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&phy->access, 0));

  PHYMOD_IF_ERR_RETURN
            (_eagle_phy_firmware_lane_config_set(phy, fw_config));

  PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&phy->access, 1));
  return PHYMOD_E_NONE;
}

static int _eagle_pll_multiplier_get(enum eagle_tsc_pll_enum pll_div, uint32_t *pll_multiplier)
{
    switch (pll_div) {
    case EAGLE_TSC_pll_div_46x:
        *pll_multiplier = 460;
        break;
    case EAGLE_TSC_pll_div_72x:
        *pll_multiplier = 720;
        break;
    case EAGLE_TSC_pll_div_40x:
        *pll_multiplier = 400;
        break;
    case EAGLE_TSC_pll_div_42x:
        *pll_multiplier = 420;
        break;
/*
    case 0x4:
        *pll_multiplier = 580;
        break;
*/
    case EAGLE_TSC_pll_div_50x:
        *pll_multiplier = 500;
        break;
    case EAGLE_TSC_pll_div_52x:
        *pll_multiplier = 520;
        break;
/*
    case 0x7:
        *pll_multiplier = 540;
        break;
*/
    case EAGLE_TSC_pll_div_60x:
        *pll_multiplier = 600;
        break;
    case EAGLE_TSC_pll_div_64x:
        *pll_multiplier = 640;
        break;
    case EAGLE_TSC_pll_div_66x:
        *pll_multiplier = 660;
        break;
    case EAGLE_TSC_pll_div_68x:
        *pll_multiplier = 680;
        break;
    case EAGLE_TSC_pll_div_70x:
        *pll_multiplier = 700;
        break;
    case EAGLE_TSC_pll_div_80x:
        *pll_multiplier = 800;
        break;
    case EAGLE_TSC_pll_div_92x:
        *pll_multiplier = 920;
        break;
    case EAGLE_TSC_pll_div_100x:
        *pll_multiplier = 1000;
        break;
    case EAGLE_TSC_pll_div_82p5x:
        *pll_multiplier = 825;
        break;
    case EAGLE_TSC_pll_div_87p5x:
        *pll_multiplier = 875;
        break;
    case EAGLE_TSC_pll_div_73p6x:
        *pll_multiplier = 736;
        break;
    case EAGLE_TSC_pll_div_36p8x:
        *pll_multiplier = 368;
        break;
/*
    case EAGLE_TSC_pll_div_199p04x:
        *pll_multiplier = 1990.4;
        break;
*/
    default:
        *pll_multiplier = 660;
        break;
    }
    return PHYMOD_E_NONE;
}

int eagle_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    const phymod_access_t *pm_acc = &core->access;
    eagle_rev_id0_t rev_id0; 
    eagle_rev_id1_t rev_id1; 

    *is_identified = 0;

    /* PHY IDs match - now check model */
    PHYMOD_IF_ERR_RETURN(eagle_tsc_identify(pm_acc, &rev_id0, &rev_id1));
    if (rev_id0.revid_model == EAGLE_MODEL)  {
        *is_identified = 1;
    }

    return PHYMOD_E_NONE;
}

int eagle_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{

     return (1);

}


/*
 * set lane swapping for core
 * The tx swap is composed of PCS swap and after that the PMD swap.
 * The rx swap is composed just by PCS swap
 */
int eagle_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
        
    phymod_phy_access_t phy_access;
    uint32_t lane, pmd_tx_map =0;

    for( lane = 0 ; lane < TSCE_NOF_LANES_IN_CORE ; lane++){
        pmd_tx_map |= ((lane_map->lane_map_tx[lane]) & 3) << (lane*4);
    }

    TSCE_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_IF_ERR_RETURN
        (eagle_pmd_lane_swap(&phy_access.access, pmd_tx_map));
        
    return PHYMOD_E_NONE;
    
}

int eagle_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
        
    int lane;
    uint32_t pmd_swap = 0;
    PHYMOD_IF_ERR_RETURN(eagle_pmd_lane_swap_tx_get(&core->access, &pmd_swap));
    for( lane = 0 ; lane < TSCE_NOF_LANES_IN_CORE ; lane++){
        /*decode each lane from four bits*/
        /*considering the pcs lane swap: tx_map[lane] = pmd_map[pcs_map[lane]]*/
        /* lane_map->lane_map_tx[lane] = (pmd_swap>>(lane_map->lane_map_rx[lane]*4)) & TSCE_LANE_SWAP_LANE_MASK; */
        lane_map->lane_map_tx[lane] = (pmd_swap>>(lane*4)) & 0x3;
        /*rx lane map is not supported*/
        lane_map->lane_map_rx[lane] = lane;
    }
    lane_map->num_of_lanes = TSCE_NOF_LANES_IN_CORE;
 
    return PHYMOD_E_NONE;
    
}


int eagle_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
        
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (eagle_core_reset_set)\n"));
        
    return PHYMOD_E_NONE;
    
}

int eagle_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
        
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (eagle_core_reset_get) \n"));

        
    return PHYMOD_E_NONE;
    
}


int eagle_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
        
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (eagle_core_firmware_info_get) \n"));
        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_firmware_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_mode)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_firmware_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_mode)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int eagle_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
        
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (eagle_core_pll_sequencer_restart)\n"));
        
    return PHYMOD_E_NONE;
    
}

int eagle_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{
        
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (eagle_core_wait_event) \n"));

        
    return PHYMOD_E_NONE;
    
}

/* reset rx sequencer
 * flags - unused parameter
 */
int eagle_phy_rx_restart(const phymod_phy_access_t* phy)
{
    PHYMOD_IF_ERR_RETURN(eagle_tsc_rx_restart(&phy->access, 1));
    return PHYMOD_E_NONE;
}

int eagle_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
        
    PHYMOD_IF_ERR_RETURN
        (eagle_tx_rx_polarity_set(&phy->access, polarity->tx_polarity, polarity->rx_polarity));
        
    return PHYMOD_E_NONE;
}

int eagle_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
        
   PHYMOD_IF_ERR_RETURN
        (eagle_tx_rx_polarity_get(&phy->access, &polarity->tx_polarity, &polarity->rx_polarity));
        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
        
   /*  first need to call the validate routine to check */
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_PRE, (int8_t)tx->pre));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_MAIN, (int8_t)tx->main));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_POST1, (int8_t)tx->post));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_POST2, (int8_t)tx->post2));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_POST3, (int8_t)tx->post3));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_AMP,  (int8_t)tx->amp));

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    int8_t value = 0;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_PRE, &value));
    tx->pre = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_MAIN, &value));
    tx->main = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_POST1, &value));
    tx->post = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_POST2, &value));
    tx->post2 = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_POST3, &value));
    tx->post3 = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_AMP, &value));
    tx->amp = value;

    return PHYMOD_E_NONE;
    
}


int eagle_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
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


int eagle_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{
        
    
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_tx_pi_freq_override(&phy->access,
                                    tx_override->phase_interpolator.enable,
                                    tx_override->phase_interpolator.value));

    return PHYMOD_E_NONE;
    
}

int eagle_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
        
    
/*
    PHYMOD_IF_ERR_RETURN
        (temod_tx_pi_control_get(&phy->access, &tx_override->phase_interpolator.value));
*/
        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
        
    uint32_t i;

    /*params check*/
    if((rx->num_of_dfe_taps == 0) || (rx->num_of_dfe_taps < TSCE_NOF_DFES)){
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal number of DFEs to set %u"), rx->num_of_dfe_taps));
    }

    /*vga set*/
    if (rx->vga.enable) {
        /* first stop the rx adaption */
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy->access, 1));
        PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_VGA, rx->vga.value));
    } else {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy->access, 0));
    }

    /*dfe set*/
    for (i = 0 ; i < rx->num_of_dfe_taps ; i++){
        if(rx->dfe[i].enable){
            PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy->access, 1));
            switch (i) {
                case 0:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE1, rx->dfe[i].value));
                    break;
                case 1:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE2, rx->dfe[i].value));
                    break;
                case 2:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE3, rx->dfe[i].value));
                    break;
                case 3:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE4, rx->dfe[i].value));
                    break;
                case 4:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE5, rx->dfe[i].value));
                    break;
                default:
                    return PHYMOD_E_PARAM;
            }
        } else {
            PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy->access, 0));
        }


    }

    /*peaking filter set*/
    if(rx->peaking_filter.enable){
        /* first stop the rx adaption */
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy->access, 1));
        PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_PF, rx->peaking_filter.value));
    } else {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy->access, 0));
    }

    if(rx->low_freq_peaking_filter.enable){
        /* first stop the rx adaption */
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy->access, 1));
        PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_PF2, rx->low_freq_peaking_filter.value));
    } else {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy->access, 0));
    }
        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    int8_t val;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_PF, &val));
    rx->peaking_filter.value = val;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_PF2, &val));
    rx->low_freq_peaking_filter.value = val;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_VGA, &val));
    rx->vga.value = val;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE1, &val));
    rx->dfe[0].value = val;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE2, &val));
    rx->dfe[1].value = val;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE3, &val));
    rx->dfe[2].value = val;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE4, &val));
    rx->dfe[3].value = val;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE5, &val));
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


int eagle_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
        
    switch (reset->rx) {
          case phymodResetDirectionIn:
              PHYMOD_IF_ERR_RETURN(eagle_force_rx_set_rst(&phy->access, 0x1));
          break;
          case phymodResetDirectionOut:
              PHYMOD_IF_ERR_RETURN(eagle_force_rx_set_rst(&phy->access, 0x0));
          break;
          case phymodResetDirectionInOut:
              PHYMOD_IF_ERR_RETURN(eagle_force_rx_set_rst(&phy->access, 0x1));
              PHYMOD_USLEEP(10);
              PHYMOD_IF_ERR_RETURN(eagle_force_rx_set_rst(&phy->access, 0x0));
          break;
          default:
          break;
    }
    switch (reset->tx) {
          case phymodResetDirectionIn:
              PHYMOD_IF_ERR_RETURN(eagle_force_tx_set_rst(&phy->access, 0x1));
          break;
          case phymodResetDirectionOut:
              PHYMOD_IF_ERR_RETURN(eagle_force_tx_set_rst(&phy->access, 0x0));
          break;
          case phymodResetDirectionInOut:
              PHYMOD_IF_ERR_RETURN(eagle_force_tx_set_rst(&phy->access, 0x1));
              PHYMOD_USLEEP(10);
              PHYMOD_IF_ERR_RETURN(eagle_force_tx_set_rst(&phy->access, 0x0));
          break;
          default:
          break;
    }
        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
        
    
    uint32_t rst;
    PHYMOD_IF_ERR_RETURN(eagle_force_tx_get_rst(&phy->access, &rst));
    if(rst == 0) {
       reset->tx = phymodResetDirectionOut;
    } else {
       reset->tx = phymodResetDirectionIn;
    }
    PHYMOD_IF_ERR_RETURN(eagle_force_rx_get_rst(&phy->access, &rst));
    if(rst == 0) {
       reset->rx = phymodResetDirectionOut;
    } else {
       reset->rx = phymodResetDirectionIn;
    }
        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
        
    enum srds_core_pwrdn_mode_enum mode;
    mode = PWRDN;
    /* printf("MODE: %x\n", mode); */
    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerNoChange)) {
            /*disable tx on the PMD side */
            mode = PWRDN_TX;
            PHYMOD_IF_ERR_RETURN(eagle_tsc_lane_pwrdn(&phy->access, mode));
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerNoChange)) {
            /*enable tx on the PMD side */
            PHYMOD_IF_ERR_RETURN(eagle_tsc_pwrdn_set(&phy->access, 1, 0));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOff)) {
            /*disable rx on the PMD side */
            mode = PWRDN_RX;
            PHYMOD_IF_ERR_RETURN(eagle_tsc_lane_pwrdn(&phy->access, mode));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOn)) {
            PHYMOD_IF_ERR_RETURN(eagle_tsc_pwrdn_set(&phy->access, 0, 0));
            /*enable rx on the PMD side */
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerOn)) {
            mode = PWR_ON;
            PHYMOD_IF_ERR_RETURN(eagle_tsc_lane_pwrdn(&phy->access, mode));
    }
    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerOff)) {
            /* Both Tx and Rx power down */
            mode = PWRDN;
            PHYMOD_IF_ERR_RETURN(eagle_tsc_lane_pwrdn(&phy->access, mode));
    }
        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    
    power_status_t pwrdn;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_pwrdn_get(&phy->access, &pwrdn));
    power->rx = (pwrdn.rx_s_pwrdn == 0)? phymodPowerOn: phymodPowerOff;
    power->tx = (pwrdn.tx_s_pwrdn == 0)? phymodPowerOn: phymodPowerOff;
        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_tx_disable_set(const phymod_phy_access_t* phy, uint32_t tx_disable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_tx_disable_get(const phymod_phy_access_t* phy, uint32_t* tx_disable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
        
    switch (tx_control)
    {
        case phymodTxSquelchOn:
            PHYMOD_IF_ERR_RETURN(eagle_tx_lane_control_set(&phy->access, 1));
            break;
        case phymodTxSquelchOff:
            PHYMOD_IF_ERR_RETURN(eagle_tx_lane_control_set(&phy->access, 0));
            break;
        default:
            PHYMOD_DEBUG_ERROR(("This control is NOT SUPPORTED!! (eagle_phy_tx_lane_control_set) \n"));
            break;
    }

    return PHYMOD_E_NONE;

}

int eagle_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
        
    uint32_t enable;

    PHYMOD_IF_ERR_RETURN(eagle_tx_lane_control_get(&phy->access, &enable));

    if(enable){
      *tx_control = phymodTxSquelchOn;
    } else {
      *tx_control = phymodTxSquelchOff;
    }

    return PHYMOD_E_NONE;

}

/*Rx control*/
int eagle_phy_rx_lane_control_set(const phymod_phy_access_t* phy,  phymod_phy_rx_lane_control_t rx_control)
{

    switch (rx_control)
    {
        case phymodRxSquelchOn:
            PHYMOD_IF_ERR_RETURN(eagle_rx_lane_control_set(&phy->access, 1));
            break;
        case phymodRxSquelchOff:
            PHYMOD_IF_ERR_RETURN(eagle_rx_lane_control_set(&phy->access, 0));
            break;
        default:
            PHYMOD_DEBUG_ERROR(("This control is NOT SUPPORTED!! (eagle_phy_rx_lane_control_set) \n"));
            break;
    }

    return PHYMOD_E_NONE;

    
}

int eagle_phy_rx_lane_control_get(const phymod_phy_access_t* phy,  phymod_phy_rx_lane_control_t* rx_control)
{
    
    uint32_t enable;

    PHYMOD_IF_ERR_RETURN(eagle_rx_lane_control_get(&phy->access, &enable));

    if(enable){
      *rx_control = phymodRxSquelchOn;
    } else {
      *rx_control = phymodRxSquelchOff;
    }

    return PHYMOD_E_NONE;
    
}

int eagle_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
        
    /* phymod_tx_t tx_params; */
    enum eagle_tsc_pll_enum current_pll_div=EAGLE_TSC_pll_div_66x;
    enum eagle_tsc_pll_enum new_pll_div=EAGLE_TSC_pll_div_66x;
    uint32_t vco_rate;
    int16_t  new_os_mode =-1;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;

    /* sc_table_entry exp_entry; RAVI */
    phymod_firmware_lane_config_t firmware_lane_config;
    phymod_firmware_core_config_t firmware_core_config;

    firmware_lane_config.MediaType = 0;

    /*next program the tx fir taps and driver current based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&pm_phy_copy.access, 0));
    }

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (eagle_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config));

    /*make sure that an and config from pcs is off*/
    firmware_core_config.CoreConfigFromPCS = 0;
    firmware_lane_config.AnEnabled = 0;
    firmware_lane_config.LaneConfigFromPCS = 0;
    firmware_lane_config.DfeOn = 0;
    firmware_lane_config.Cl72RestTO = 1; 

    /* No PCS
    PHYMOD_IF_ERR_RETURN
        (temod_update_port_mode(&phy->access, (int *) &pll_switch));
    */
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_get_vco(config, &vco_rate, &new_pll_div, &new_os_mode));

    if(config->data_rate >= 10312) {
      firmware_lane_config.DfeOn = 1;
    }

/*
    PHYMOD_IF_ERR_RETURN
        (eagle_pll_mode_get(&pm_phy_copy.access, &current_pll_div));
*/
    PHYMOD_IF_ERR_RETURN
        (eagle_pll_config_get(&pm_phy_copy.access, &current_pll_div));

    /* new_pll_div already found
    PHYMOD_IF_ERR_RETURN
        (temod_plldiv_lkup_get(&phy->access, spd_intf, &new_pll_div));
    */

    /*
    if(config->device_aux_modes !=NULL){
        PHYMOD_IF_ERR_RETURN
            (_tsce_st_hto_interface_config_set(config->device_aux_modes, start_lane, new_speed_vec, &new_pll_div, &new_os_mode)) ;
    }
    */

    /*  clean the interface, no need for interface */



    /*if pll change is enabled*/
    if((current_pll_div != new_pll_div) && (PHYMOD_INTF_F_DONT_TURN_OFF_PLL & flags)){
        if(config->interface_type != phymodInterfaceBypass) {
            /*Terminate the execution of the function*/
            PHYMOD_DEBUG_WARN(("PLL has to change for speed_set from %u to %u but DONT_TURN_OFF_PLL flag is enabled \n",
                               (unsigned int)current_pll_div, (unsigned int)new_pll_div));
            return PHYMOD_E_NONE;
        }
    }
    /*pll switch is required and expected */
    if((current_pll_div != new_pll_div) && !(PHYMOD_INTF_F_DONT_TURN_OFF_PLL & flags)) {
        /* phymod_access_t tmp_phy_access; */
        PHYMOD_IF_ERR_RETURN
            (eagle_core_soft_reset_release(&pm_phy_copy.access, 0));

        /*release the uc reset */
#if 0
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_uc_reset(&pm_phy_copy.access ,0));
#endif
        /*set the PLL divider */
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_configure_pll(&pm_phy_copy.access, new_pll_div));
/*
        PHYMOD_IF_ERR_RETURN
            (eagle_pll_mode_set(&pm_phy_copy.access, new_pll_div));
*/

        /* firmware_core_config.VcoRate = (vco_rate * 16 + 500) / 1000 - 224; */
        firmware_core_config.VcoRate = (vco_rate - 5750) / 250 + 1;

        /*change the  master port num to the current caller port */
        /*, need a function to get the starting lane number based on the lane_map*/
        /*
        PHYMOD_IF_ERR_RETURN
            (temod_master_port_num_set(&phy->access, start_lane));
        ////////////////// Do we need PLL reset ///////////////
        */
#if 0
        PHYMOD_IF_ERR_RETURN
            (eagle_pll_reset_enable_set(&pm_phy_copy.access, 1));
#endif
        /*update the firmware config properly*/
        PHYMOD_IF_ERR_RETURN
            (eagle_phy_firmware_core_config_set(&pm_phy_copy, firmware_core_config));
        PHYMOD_IF_ERR_RETURN
            (eagle_core_soft_reset_release(&pm_phy_copy.access, 1));
    }

    /*
    PHYMOD_IF_ERR_RETURN
        (temod_set_spd_intf(&phy->access, spd_intf));
    */

    /*change TX parameters if enabled*/
    /*
    if((PHYMOD_IF_F_DONT_OVERIDE_TX_PARAMS & flags) == 0) {
        PHYMOD_IF_ERR_RETURN
            (tsce_phy_media_type_tx_get(phy, phymodMediaTypeMid, &tx_params));
    }
    */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
             (_eagle_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }

    /*release the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }

    PHYMOD_IF_ERR_RETURN
        (eagle_osr_mode_set(&phy->access, new_os_mode));

        
    return PHYMOD_E_NONE;
    
}

/*flags- unused parameter*/
int eagle_phy_interface_config_get(const phymod_phy_access_t* phy, 
                                   uint32_t flags, 
                                   phymod_ref_clk_t ref_clock, 
                                   phymod_phy_inf_config_t* config)
{
    int osr_mode;
    /* uint32_t pll_div; */
    enum eagle_tsc_pll_enum pll_div;
    uint32_t pll_multiplier, refclk;
    phymod_phy_access_t phy_copy;
    uint32_t actual_osr, actual_osr_rem;
    phymod_osr_mode_t osr_mode_enum;
    int start_lane, num_lane;

    config->ref_clock = ref_clock;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (eagle_osr_mode_get(&phy->access, &osr_mode));
/*
    PHYMOD_IF_ERR_RETURN
        (eagle_pll_mode_get(&phy_copy.access, &pll_div));
*/
    PHYMOD_IF_ERR_RETURN
        (eagle_pll_config_get(&phy_copy.access, &pll_div));
    PHYMOD_IF_ERR_RETURN
        (_eagle_pll_multiplier_get(pll_div, &pll_multiplier));


    if (ref_clock == phymodRefClk156Mhz) {
        refclk = 15625;
    } else if (ref_clock == phymodRefClk125Mhz) {
        refclk = 12500;
    } else {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL,  (_PHYMOD_MSG("Unknown refclk")));
    }


    PHYMOD_IF_ERR_RETURN(eagle_osr_mode_to_enum(osr_mode, &osr_mode_enum));
    PHYMOD_IF_ERR_RETURN(phymod_osr_mode_to_actual_os(osr_mode_enum, &actual_osr, &actual_osr_rem));

  /*  Divide by 1000 is take care of pll_mulitple(by 10) and ref_clk (by 100) */
    config->data_rate = (refclk*pll_multiplier)/(1000*actual_osr);
    config->interface_type = phymodInterfaceBypass;

    if(osr_mode_enum == phymodOversampleMode2) {
        PHYMOD_INTF_MODES_OS2_SET(config);
    }

    return PHYMOD_E_NONE;
}

        
int eagle_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    PHYMOD_IF_ERR_RETURN
        (eagle_lane_soft_reset_release(&phy->access, 0));
    PHYMOD_IF_ERR_RETURN
        (eagle_clause72_control(&phy->access, cl72_en));
    PHYMOD_IF_ERR_RETURN
        (eagle_lane_soft_reset_release(&phy->access, 1));
        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
        
    PHYMOD_IF_ERR_RETURN
        (eagle_clause72_control_get(&phy->access, cl72_en));
        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
        
    
    uint32_t local_status;
    PHYMOD_IF_ERR_RETURN
        (eagle_pmd_cl72_receiver_status(&phy->access, &local_status));
    status->locked = local_status;
    return PHYMOD_E_NONE;
    
}


int eagle_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability_set_type)
{
    
    /* Not supported */
    /* PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (eagle_phy_autoneg_ability_set) \n"));  */

    return PHYMOD_E_NONE;

}

int eagle_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
        
   /* Not supported */
   PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (eagle_phy_autoneg_ability_get) \n"));

    return PHYMOD_E_NONE;
    
}


int eagle_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (eagle_phy_autoneg_set) \n"));
        
    return PHYMOD_E_NONE;
    
}

int eagle_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (eagle_phy_autoneg_get) \n"));

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("This function is NOT SUPPORTED!! (eagle_phy_autoneg_status_get) \n"));
        
    return PHYMOD_E_NONE;
    
}

/* load tsce fw. the fw_loader parameter is valid just for external fw load*/
STATIC
int _eagle_core_firmware_load(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config)
{
    /* int wait; */

    switch(init_config->firmware_load_method){
    case phymodFirmwareLoadMethodInternal:
        PHYMOD_IF_ERR_RETURN(eagle_tsc_ucode_mdio_load(&core->access, tsce_ucode, tsce_ucode_len));
        break;
    case phymodFirmwareLoadMethodExternal:
        if(!PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_GET(init_config)) {
            PHYMOD_IF_ERR_RETURN(eagle_pram_flop_set(&core->access, 0x0));

            PHYMOD_IF_ERR_RETURN(eagle_tsc_ucode_init(&core->access));
            
            /* 
            wait = (PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_GET(init_config)) ? 0 : 1;
            PHYMOD_IF_ERR_RETURN(eagle_pram_firmware_enable(&core->access, 1, wait)); 
            */

            PHYMOD_IF_ERR_RETURN(eagle_pram_firmware_enable(&core->access, 1)); 

            if(PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_GET(init_config)) {
                return PHYMOD_E_NONE;
            }

            PHYMOD_NULL_CHECK(init_config->firmware_loader);

            PHYMOD_IF_ERR_RETURN(init_config->firmware_loader(core, tsce_ucode_len, tsce_ucode));
        }
        PHYMOD_IF_ERR_RETURN(eagle_pram_firmware_enable(&core->access, 0));
        break;
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), init_config->firmware_load_method));
    }
    if(init_config->firmware_load_method != phymodFirmwareLoadMethodNone){
        /*PHYMOD_IF_ERR_RETURN(tsce_core_firmware_info_get(core, &actual_fw));
        if((tsce_ucode_crc != actual_fw.fw_crc) || (tsce_ucode_ver != actual_fw.fw_version)){
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("fw load validation was failed")));
        } */
    }
    return PHYMOD_E_NONE;
}

int eagle_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    int rv;
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    phymod_firmware_core_config_t  firmware_core_config_tmp;

    TSCE_CORE_TO_PHY_ACCESS(&phy_access, core);
    phy_access_copy = phy_access;
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;
    phy_access_copy = phy_access;
    phy_access_copy.access = core->access;
    phy_access_copy.access.lane_mask = 0x1;
    phy_access_copy.type = core->type;


    if(!PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (eagle_pmd_reset_seq(&core_copy.access, core_status->pmd_active));
    }
    rv = _eagle_core_firmware_load(&core_copy, init_config);
    if (rv != PHYMOD_E_NONE) {
        PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC firmware-load failed\n", core->access.addr, core->access.lane_mask));
        PHYMOD_IF_ERR_RETURN(rv);
    }

    if(PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_GET(init_config)) {
        return PHYMOD_E_NONE;
    }

    /*next we need to check if the load is correct or not */
    if(init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {

       /* This moved earlier as compared to falcon, to sync up with tsce.c */
        PHYMOD_IF_ERR_RETURN
            (eagle_pmd_ln_h_rstb_pkill_override( &phy_access_copy.access, 0x1));

        /*next we need to set the uc active and release uc */
        PHYMOD_IF_ERR_RETURN
            (eagle_uc_active_set(&core_copy.access ,1));

        /*release the uc reset */
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_uc_reset(&core_copy.access ,0));

#ifndef TSCE_PMD_CRC_UCODE
        if(PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
            rv = eagle_tsc_ucode_load_verify(&core_copy.access, (uint8_t *) &tsce_ucode, tsce_ucode_len);
            if (rv != PHYMOD_E_NONE) {
                PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC load-verify failed\n", core->access.addr, core->access.lane_mask));
                PHYMOD_IF_ERR_RETURN(rv);
            }
        }
#endif

        /* we need to wait at least 10ms for the uc to settle */
        PHYMOD_USLEEP(10000);

       /* poll the ready bit in 10 ms */
#ifndef TSCE_PMD_CRC_UCODE
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_poll_uc_dsc_ready_for_cmd_equals_1(&phy_access_copy.access, 1));
#else
/*
        PHYMOD_IF_ERR_RETURN(
             eagle_tsc_ucode_crc_verify( &core_copy.access, tsce_ucode_len,tsce_ucode_crc));
*/
#endif


        PHYMOD_IF_ERR_RETURN
            (eagle_pmd_ln_h_rstb_pkill_override( &phy_access_copy.access, 0x0));
    }

    /* plldiv CONFIG */
    PHYMOD_IF_ERR_RETURN
        (eagle_pll_mode_set(&core_copy.access, 0xa));

    /*now config the lane mapping and polarity */
    PHYMOD_IF_ERR_RETURN
        (eagle_core_lane_map_set(core, &init_config->lane_map));
    /*
    PHYMOD_IF_ERR_RETURN
        (tsce_core_lane_map_set(core, &init_config->lane_map));
    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_timer_init(&core->access));
    PHYMOD_IF_ERR_RETURN
        (temod_master_port_num_set(&core->access, 0));
    */
    /*don't overide the fw that set in config set if not specified*/
    firmware_core_config_tmp = init_config->firmware_core_config;
    firmware_core_config_tmp.CoreConfigFromPCS = 0;
    /*set the vco rate to be default at 10.3125G */
    firmware_core_config_tmp.VcoRate = 0x13;

    PHYMOD_IF_ERR_RETURN
        (eagle_phy_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp));

    /* release core soft reset */
    PHYMOD_IF_ERR_RETURN
        (eagle_core_soft_reset_release(&core_copy.access, 1));

        
    return PHYMOD_E_NONE;
    
}


int eagle_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
        
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;
    int lane_bkup;
    phymod_polarity_t tmp_pol;

    PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /*next program the tx fir taps and driver current based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));
    /*per lane based reset release */
    /* PHYMOD_IF_ERR_RETURN
        (temod_pmd_x4_reset(pm_acc)); */
    PHYMOD_IF_ERR_RETURN(eagle_lane_hard_soft_reset_release(&pm_phy_copy.access, 0));
    PHYMOD_IF_ERR_RETURN(eagle_lane_hard_soft_reset_release(&pm_phy_copy.access, 1));
    PHYMOD_IF_ERR_RETURN(eagle_lane_soft_reset_release(&pm_phy_copy.access, 0));
    PHYMOD_IF_ERR_RETURN(eagle_lane_soft_reset_release(&pm_phy_copy.access, 1));

    lane_bkup = pm_phy_copy.access.lane_mask;
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }
    pm_phy_copy.access.lane_mask = lane_bkup;

    /* program the rx/tx polarity */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        tmp_pol.tx_polarity = (init_config->polarity.tx_polarity) >> i & 0x1;
        tmp_pol.rx_polarity = (init_config->polarity.rx_polarity) >> i & 0x1;
        PHYMOD_IF_ERR_RETURN
            (eagle_phy_polarity_set(&pm_phy_copy, &tmp_pol));
    }

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (eagle_phy_tx_set(&pm_phy_copy, &init_config->tx[i]));
    }

    PHYMOD_IF_ERR_RETURN
        (eagle_phy_cl72_set(&pm_phy_copy, init_config->cl72_en));

    /* ONLY for PCS
    PHYMOD_IF_ERR_RETURN
        (temod_update_port_mode(pm_acc, &pll_restart));

    PHYMOD_IF_ERR_RETURN
        (temod_rx_lane_control_set(pm_acc, 1));
    PHYMOD_IF_ERR_RETURN
        (temod_tx_lane_control_set(pm_acc, TEMOD_TX_LANE_RESET_TRAFFIC_ENABLE));
    */

    return PHYMOD_E_NONE;

}

int eagle_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
        
    int i;
    int start_lane, num_lane;
    int rv = PHYMOD_E_NONE;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
        /* PHYMOD_IF_ERR_RETURN(temod_tx_loopback_control(&phy->access, enable, start_lane, num_lane)); */
        break;
    case phymodLoopbackGlobalPMD :
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(eagle_tsc_dig_lpbk(&phy_copy.access, (uint8_t) enable));
            PHYMOD_IF_ERR_RETURN(eagle_pmd_force_signal_detect(&phy_copy.access, (int) enable));
        }
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(eagle_tsc_rmt_lpbk(&phy->access, (uint8_t)enable));
        break;
    case phymodLoopbackRemotePCS :
        /* PHYMOD_IF_ERR_RETURN(temod_rx_loopback_control(&phy->access, enable)); */
        break;
    default :
        break;
    }
    return rv;
    
}

int eagle_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
        
    int start_lane, num_lane;

    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
        /* PHYMOD_IF_ERR_RETURN(temod_tx_loopback_get(&phy->access, &enable_core)); */
        /* *enable = (enable_core >> start_lane) & 0x1; */
        break;
    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN(eagle_tsc_dig_lpbk_get(&phy->access, enable));
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(eagle_tsc_rmt_lpbk_get(&phy->access, enable));
        break;
    case phymodLoopbackRemotePCS :
        /* PHYMOD_IF_ERR_RETURN(temod_rx_loopback_control(&phy->access, enable, enable, enable)); */
        break;
    default :
        break;
    }
    return PHYMOD_E_NONE;
    
}


/* this function gives the PMD_RX_LOCK_STATUS */
int eagle_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* signal_detect)
{

    PHYMOD_IF_ERR_RETURN(eagle_tsc_signal_detect(&phy->access, signal_detect));

    return PHYMOD_E_NONE;

}


int eagle_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
        
    unsigned char rx_lock;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_pmd_lock_status(&phy->access, &rx_lock));
    *link_status = (uint32_t ) rx_lock;

    return PHYMOD_E_NONE;
    
}


int eagle_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* pmd_lock)
{

    PHYMOD_IF_ERR_RETURN(eagle_phy_link_status_get(phy, pmd_lock));
    return PHYMOD_E_NONE;

}

int eagle_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
        
    
    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_read(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;
    
}


int eagle_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
        
   PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_write(&phy->access, reg_addr, val));
   return PHYMOD_E_NONE;
    
}



