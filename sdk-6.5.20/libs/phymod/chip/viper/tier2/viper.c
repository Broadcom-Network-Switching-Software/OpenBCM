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
#include <phymod/chip/bcmi_viper_xgxs_defs.h>

#include "../../viper/tier1/viper_inc.h"


#define VMOD_ID0       0x0143
#define VMOD_ID1       0xbff0

#define VXMOD_MODEL     0x8  /*XGXS10G core*/
#define VGMOD_MODEL     0xF  /*QUAD_SGMII core*/

#define VIPER_NOF_DFES (5)
#define VIPER_NOF_LANES_IN_CORE (4)
#define VIPER_LANE_SWAP_LANE_MASK (0x3)
#define VIPER_PHY_ALL_LANES (0xf)

#define VIPER_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type = (_core_access)->type; \
        (_phy_access)->access.lane_mask = VIPER_PHY_ALL_LANES; \
    }while(0)

int viper_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    int ioerr = 0 ;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t * pm_acc = &core->access;
    COMBO_ID1r_t id1;
    COMBO_ID2r_t id2;
    SERDESID0r_t serdes_id;
    uint32_t model = 0;
    *is_identified = 0;

    if (core_id == 0){
        ioerr += READ_COMBO_ID1r(pm_acc, &id1);
        ioerr += READ_COMBO_ID2r(pm_acc, &id2);
    } else {
        COMBO_ID1r_SET(id1, (core_id >> 16) & 0xffff);
        COMBO_ID2r_SET(id2, (core_id & 0xffff));
    }

    if ((COMBO_ID1r_GET(id1) == VMOD_ID0) && (COMBO_ID2r_GET(id2) == VMOD_ID1)){
        /* PHY IDs match - now check Serdes model */
        ioerr += READ_SERDESID0r(pm_acc, &serdes_id);
        model = SERDESID0r_MODEL_NUMBERf_GET(serdes_id);
        if ((model == VGMOD_MODEL) || (model == VXMOD_MODEL)){
            *is_identified = 1;
        }
    }
#if 0
    PHYMOD_VDBG(DBG_CFG, pm_acc, ("%-22s: core_id=%0x identified=%0d adr=%0x lmask=%0x rv=%d\n",
            __func__, core_id, *is_identified, pm_acc->addr, pm_acc->lane_mask, rv));
#endif
    rv =  ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;

    return rv;
}


int viper_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    uint32_t serdes_id;
    COMBO_ID1r_t id1;
    COMBO_ID2r_t id2;
    char core_name[15]="viper";
    const phymod_access_t * pm_acc = &core->access;

    PHYMOD_IF_ERR_RETURN(viper_revid_read(&core->access, &serdes_id));
    info->serdes_id = serdes_id;
    if ((serdes_id & 0x3f) == VXMOD_MODEL) {
        info->core_version = phymodCoreVersionViperXA0;
    } else {
        info->core_version = phymodCoreVersionViperGA0;
    }
    PHYMOD_IF_ERR_RETURN
        (phymod_core_name_get(core, serdes_id, core_name, info));

    PHYMOD_IF_ERR_RETURN(READ_COMBO_ID1r(pm_acc, &id1));
    PHYMOD_IF_ERR_RETURN(READ_COMBO_ID2r(pm_acc, &id2));

    info->phy_id0 = (uint16_t) id1.v[0];
    info->phy_id1 = (uint16_t) id2.v[0];

    return PHYMOD_E_NONE;
}

/*
 * The tx swap is TX Digital to Analog
 * The rx swap is RX Analog to Digital
 */
int viper_core_lane_map_set(phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    uint32_t pcs_swap = 0, pmd_swap = 0, lane;

    /* SGMIIPLUS2_X4 core doesn't support lane swap */
    if(VIPER_CORE_IS_SGMIIPLUS2_X4(&core->access)) {
        return PHYMOD_E_UNAVAIL;
    }

    if(lane_map->num_of_lanes != VIPER_NOF_LANES_IN_CORE){
        return PHYMOD_E_CONFIG;
    }

    for( lane = 0; lane < VIPER_NOF_LANES_IN_CORE; lane++){
        if(lane_map->lane_map_rx[lane] >= VIPER_NOF_LANES_IN_CORE){
            return PHYMOD_E_CONFIG;
        }
        pmd_swap += lane_map->lane_map_rx[lane]<<(lane*4);
    }

    for( lane = 0; lane < VIPER_NOF_LANES_IN_CORE; lane++){
        if(lane_map->lane_map_tx[lane] >= VIPER_NOF_LANES_IN_CORE){
            return PHYMOD_E_CONFIG;
        }
        pcs_swap += lane_map->lane_map_tx[lane]<<(lane*4);
    }
    PHYMOD_IF_ERR_RETURN(viper_tx_lane_swap(&core->access, pcs_swap));

    PHYMOD_IF_ERR_RETURN(viper_rx_lane_swap(&core->access, pmd_swap));

    return PHYMOD_E_NONE;
    
}

int viper_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    uint32_t pmd_swap = 0, pcs_swap = 0, lane;

    /* SGMIIPLUS2_X4 core doesn't support lane swap */
    if(VIPER_CORE_IS_SGMIIPLUS2_X4(&core->access)) {
        return PHYMOD_E_UNAVAIL;
    }

    PHYMOD_IF_ERR_RETURN(viper_tx_lane_swap_get(&core->access, &pcs_swap));
    PHYMOD_IF_ERR_RETURN(viper_rx_lane_swap_get(&core->access, &pmd_swap));

    for( lane = 0 ; lane < VIPER_NOF_LANES_IN_CORE ; lane++){
        /* Deccode each lane from four bits */
        lane_map->lane_map_rx[lane] = (pmd_swap >> (lane*4)) & VIPER_LANE_SWAP_LANE_MASK;
        lane_map->lane_map_tx[lane] = (pcs_swap >> (lane*4)) & VIPER_LANE_SWAP_LANE_MASK;
    }
    lane_map->num_of_lanes = VIPER_NOF_LANES_IN_CORE;
        
    return PHYMOD_E_NONE;
}


int viper_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    
    /* Place your code here */
        
    return PHYMOD_E_NONE;
}

int viper_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
    
    /* Place your code here */
        
    return PHYMOD_E_NONE;
}


int viper_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    switch (operation) {
        case phymodSeqOpStop:
            PHYMOD_IF_ERR_RETURN
                (viper_pll_sequencer_control(&core->access, 0)); 
        break;
        case phymodSeqOpStart:
            PHYMOD_IF_ERR_RETURN
                (viper_pll_sequencer_control(&core->access, 1));
            
            /* PHYMOD_IF_ERR_RETURN
                (viper_pll_lock_wait(&core->access, 250000)); */
        break;
        case phymodSeqOpRestart:
            PHYMOD_IF_ERR_RETURN
                (viper_pll_sequencer_control(&core->access, 0));
            
            PHYMOD_IF_ERR_RETURN
                (viper_pll_sequencer_control(&core->access, 1));
            
            /* PHYMOD_IF_ERR_RETURN
                (viper_pll_lock_wait(&core->access, 250000)); */
        break;
        default:
            return PHYMOD_E_UNAVAIL;
        break;
    }

    return PHYMOD_E_NONE;
}


int viper_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{
        
    
    /* NOT NEED, REMOVE LATER  */
        
    return PHYMOD_E_NONE;
    
}


int viper_phy_rx_restart(const phymod_phy_access_t* phy)
{
    
    /* NOT NEED, REMOVE LATER  */
        
    return PHYMOD_E_NONE;
    
}


int viper_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    int rv = PHYMOD_E_NONE;
    phymod_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, &(phy->access), sizeof(phy_copy));
    rv = viper_tx_pol_set(&phy_copy, polarity->tx_polarity);
    if (rv != PHYMOD_E_NONE) return (rv);

    rv = viper_rx_pol_set(&phy_copy, polarity->rx_polarity);

    return rv;
}

int viper_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    int rv = PHYMOD_E_NONE;
    phymod_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, &(phy->access), sizeof(phy_copy));

    rv = viper_tx_pol_get(&phy_copy, &polarity->tx_polarity);
    if (rv != PHYMOD_E_NONE) return (rv);

    rv = viper_rx_pol_get(&phy_copy, &polarity->rx_polarity);

    return rv;
}


int viper_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    phymod_access_t phy_copy;
    int start_lane = 0, num_lane = 0, i;

    PHYMOD_MEMCPY(&phy_copy, &(phy->access), sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy_copy, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
         phy_copy.lane_mask = 0x1 << (i + start_lane);

         PHYMOD_IF_ERR_RETURN(viper_tx_post_set(&phy_copy, 1, (tx->post & 0xff)));
         PHYMOD_IF_ERR_RETURN(viper_tx_main_set(&phy_copy, (tx->main  & 0xff)));
    }

    return PHYMOD_E_NONE;

}

int viper_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    uint32_t tx_value, enable;
    phymod_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, &(phy->access), sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN(viper_tx_post_get(&phy_copy, &enable, &tx_value));
    tx->post = tx_value & 0xff;

    PHYMOD_IF_ERR_RETURN(viper_tx_main_get(&phy_copy, &tx_value));
    tx->main = tx_value & 0xff;

    return PHYMOD_E_NONE;
}


int viper_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    phymod_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, &(phy->access), sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN(viper_rx_pf_set(&phy_copy, rx->peaking_filter.value));

    return PHYMOD_E_NONE;

}

int viper_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    uint32_t rx_value;
    phymod_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, &(phy->access), sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN(viper_rx_pf_get(&phy_copy, &rx_value));
    rx->peaking_filter.value = rx_value;

    return PHYMOD_E_NONE;

}


int viper_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int viper_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int viper_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    int rv = PHYMOD_E_NONE;
    int combo = 0;
    viper_actual_speed_id_t speed_id;
    uint16_t sgmii_mode = 0;
    phymod_phy_access_t phy_copy;
    uint32_t rx_swing, rx_los_en, rx_los_invert_en;
    uint32_t lpbk, sd_thre;
    uint32_t tx_polarity, rx_polarity;
    uint32_t tx_post, tx_main, tx_post_enable;
    uint32_t lpi_enable;
    viper_duplex_mode_t duplex_mode;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN(viper_actual_speed_get(&phy->access, &speed_id));
    if (speed_id == VIPER_SPD_10G_CX4)  {
        combo = 1;
    }

    if (power->tx | power->rx){
        rv = viper_phy_enable_set(&phy->access, 1, combo);
        /*RST the pipeline and re-configure the viper after power on for 100-FX*/
        if(VIPER_CORE_IS_SGMIIPLUS2_X4(&phy->access)) {
            PHYMOD_IF_ERR_RETURN(viper_sgmii_mode_get(&phy->access, &sgmii_mode));
            if (speed_id == VIPER_SPD_100M && !sgmii_mode) {
                /*Store the configuration*/
                PHYMOD_IF_ERR_RETURN
                    (viper_rx_large_swing_get(&phy_copy.access, &rx_swing));
                PHYMOD_IF_ERR_RETURN
                    (viper_tx_pol_get(&phy_copy.access, &tx_polarity));
                PHYMOD_IF_ERR_RETURN
                    (viper_rx_pol_get(&phy_copy.access, &rx_polarity));
                PHYMOD_IF_ERR_RETURN
                    (viper_tx_post_get(&phy_copy.access, &tx_post_enable, &tx_post));
                PHYMOD_IF_ERR_RETURN
                    (viper_tx_main_get(&phy_copy.access, &tx_main));
                PHYMOD_IF_ERR_RETURN
                    (viper_signal_detect_get(&phy_copy.access, &rx_los_en));
                PHYMOD_IF_ERR_RETURN
                    (viper_signal_invert_get(&phy_copy.access, &rx_los_invert_en));
                PHYMOD_IF_ERR_RETURN
                    (viper_100FX_global_loopback_get(&phy->access, &lpbk));
                PHYMOD_IF_ERR_RETURN
                    (viper_sigdet_threshold_get(&phy->access, &sd_thre));
                PHYMOD_IF_ERR_RETURN
                    (viper_lpi_enable_get(&phy->access, &lpi_enable));
                PHYMOD_IF_ERR_RETURN
                    (viper_duplex_get(&phy->access, &duplex_mode));

                /*rstb_hw*/
                PHYMOD_IF_ERR_RETURN
                    (viper_phy_hw_reset(&phy->access));
                PHYMOD_USLEEP(100*1000);

                /* rstb_hw resets both registers and datapath.
                 * Recover the configration after rstb_hw.
                 */
                PHYMOD_IF_ERR_RETURN
                   (viper_rx_large_swing_set(&phy_copy.access, rx_swing));
                PHYMOD_IF_ERR_RETURN
                    (viper_tx_pol_set(&phy_copy.access, tx_polarity));
                PHYMOD_IF_ERR_RETURN
                    (viper_rx_pol_set(&phy_copy.access, rx_polarity));
                if (tx_post_enable) {
                    PHYMOD_IF_ERR_RETURN
                        (viper_tx_post_set(&phy_copy.access, 1, (tx_post & 0xff)));
                }
                PHYMOD_IF_ERR_RETURN
                    (viper_tx_main_set(&phy_copy.access, (tx_main & 0xff)));
                if (rx_los_en) {
                    PHYMOD_IF_ERR_RETURN
                       (viper_signal_detect_set(&phy_copy.access));
                }
                if (rx_los_invert_en) {
                    PHYMOD_IF_ERR_RETURN
                       (viper_signal_invert_set(&phy_copy.access));
                }
                PHYMOD_IF_ERR_RETURN
                    (viper_sigdet_threshold_set(&phy->access, sd_thre));
                PHYMOD_IF_ERR_RETURN
                    (viper_lpi_enable_set(&phy->access,lpi_enable));
                PHYMOD_IF_ERR_RETURN
                    (viper_duplex_set(&phy->access, duplex_mode));
                PHYMOD_IF_ERR_RETURN
                    (viper_set_spd_intf(&phy_copy.access, VIPER_SPD_100_FX));
                PHYMOD_USLEEP(100*1000);
                if (lpbk) {
                    PHYMOD_IF_ERR_RETURN
                        (viper_100FX_global_loopback_set(&phy->access, lpbk));
                }
            }
        }
    } else {
        rv = viper_phy_enable_set(&phy->access, 0, combo);
    }

    return rv;
}

int viper_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{

    int rv = PHYMOD_E_NONE; 
    int enable = 0;
            
    rv = viper_phy_enable_get(&phy->access, &enable);
    if (rv != PHYMOD_E_NONE) return (rv);
    power->tx = enable;
    power->rx = enable;
        
    return rv;
    
}


int viper_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
        
    
    /*NOT NEED, REMOVE LATER */
        
    return PHYMOD_E_NONE;
    
}

int viper_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
        
    
    /*NOT NEED, REMOVE LATER */
        
    return PHYMOD_E_NONE;
    
}


int viper_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
        
    
    /*NOT NEED, REMOVE LATER */
        
    return PHYMOD_E_NONE;
    
}

int viper_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
        
    
    /*NOT NEED, REMOVE LATER */
        
    return PHYMOD_E_NONE;
    
}


int viper_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
        
    
    /*NOT NEED, REMOVE LATER */
        
    return PHYMOD_E_NONE;
    
}

int viper_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
        
    
    /*NOT NEED, REMOVE LATER */
        
    return PHYMOD_E_NONE;
    
}


int viper_phy_eee_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    uint32_t lpi_bypass;
    int rv = PHYMOD_E_NONE ;
    lpi_bypass = PHYMOD_LPI_BYPASS_GET(enable);
    enable &= 0x1;
    if (lpi_bypass) {
        rv = viper_lpi_enable_set(&phy->access,enable);
    } else {
        return PHYMOD_E_UNAVAIL;
    }

    return rv;
}

int viper_phy_eee_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    if (PHYMOD_LPI_BYPASS_GET(*enable)) {
        PHYMOD_IF_ERR_RETURN(viper_lpi_enable_get(&phy->access, enable));
        PHYMOD_LPI_BYPASS_SET(*enable);
    } else {
        return PHYMOD_E_UNAVAIL;
    }
    return PHYMOD_E_NONE;
}

int viper_phy_linkdown_transmit_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    PHYMOD_IF_ERR_RETURN(viper_linkdown_transmit_set(&phy->access, enable));

    return PHYMOD_E_NONE;
}

int viper_phy_linkdown_transmit_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    PHYMOD_IF_ERR_RETURN(viper_linkdown_transmit_get(&phy->access, enable));

    return PHYMOD_E_NONE;
}

int viper_phy_interface_config_set(phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    
    viper_spd_intfc_type_t spd_intf = VIPER_SPD_ILLEGAL;

    if  (config->interface_type == phymodInterfaceSGMII) {
        switch (config->data_rate) {
            case 10:
                spd_intf = VIPER_SPD_10_SGMII;
                break;
            case 100:
                spd_intf = VIPER_SPD_100_SGMII;
                break;
            case 1000:
                spd_intf = VIPER_SPD_1000_SGMII;
                break;
            default:
                spd_intf = VIPER_SPD_1000_SGMII;
                break;
        }
   } else if (config->interface_type == phymodInterface1000X) {
        switch (config->data_rate) {
            case 100:
                spd_intf = VIPER_SPD_100_FX;
                break;
            case 1000:
                spd_intf = VIPER_SPD_1000_X1;
                break;
            case 2500:
                spd_intf = VIPER_SPD_2500;
                break;
            default:
                spd_intf = VIPER_SPD_1000_X1;
                break;
        }
    } else if (config->interface_type == phymodInterfaceXAUI) {
        /* SGMIIPLUS2_X4 core doesn't support 10G */
        if(VIPER_CORE_IS_SGMIIPLUS2_X4(&phy->access)) {
            return PHYMOD_E_UNAVAIL;
        }

        if (config->data_rate == 10000) {
            spd_intf = VIPER_SPD_10000_CX4;
        }
    }
    PHYMOD_IF_ERR_RETURN
        (viper_set_spd_intf(&phy->access, spd_intf));

    if (config->interface_type == phymodInterfaceXAUI){
        if(VIPER_CORE_IS_SGMIIPLUS2_X4(&phy->access)) {
            return PHYMOD_E_UNAVAIL;
        }
        PHYMOD_IF_ERR_RETURN
            (viper_10G_CX4_compliance_set(&phy->access, 0xe));
   }

   return PHYMOD_E_NONE;
    
}


int viper_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{
    viper_actual_speed_id_t speed_id;
    
    PHYMOD_IF_ERR_RETURN(viper_actual_speed_get(&phy->access, &speed_id));
    PHYMOD_IF_ERR_RETURN(viper_speed_id_interface_config_get(phy, speed_id, config));

    return PHYMOD_E_NONE;
    
}


int viper_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability_set_type)
{
    viper_an_ability_t value;

    PHYMOD_MEMSET(&value, 0x0, sizeof(value));

    /* check if sgmii  or not */
    if (PHYMOD_AN_CAP_SGMII_GET(an_ability_set_type)) {
        switch (an_ability_set_type->sgmii_speed) {
            case phymod_CL37_SGMII_10M:
                value.cl37_sgmii_speed = VIPER_CL37_SGMII_10M;
                break;
            case phymod_CL37_SGMII_100M:
                value.cl37_sgmii_speed = VIPER_CL37_SGMII_100M;
                break;
            case phymod_CL37_SGMII_1000M:
                value.cl37_sgmii_speed = VIPER_CL37_SGMII_1000M;
                break;
            default:
                value.cl37_sgmii_speed = VIPER_CL37_SGMII_1000M;
                break;
        }
    }

    /* next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability_set_type) && !PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability_set_type)) {
        value.an_pause = VIPER_SYMM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability_set_type) && !PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability_set_type)) {
        value.an_pause = VIPER_ASYM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability_set_type) && PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability_set_type)) {
        value.an_pause = VIPER_ASYM_SYMM_PAUSE;
    }

    PHYMOD_IF_ERR_RETURN(viper_autoneg_local_ability_set(&phy->access, &value));

    return PHYMOD_E_NONE;
}

int viper_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    viper_an_adv_ability_t value;

    PHYMOD_MEMSET(&value, 0x0, sizeof(value));

    PHYMOD_IF_ERR_RETURN(viper_autoneg_local_ability_get(&phy->access, &value));
    switch (value.cl37_sgmii_speed){
    case VIPER_CL37_SGMII_10M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_10M;
        break;
    case VIPER_CL37_SGMII_100M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_100M;
        break;
    case VIPER_CL37_SGMII_1000M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_1000M;
        break;
    default:
        break;
    }

    switch(value.an_pause) {
    case VIPER_ASYM_PAUSE:
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        break;
    case VIPER_SYMM_PAUSE:
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        break;
    case VIPER_ASYM_SYMM_PAUSE:
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        break;
    default:
        break;
    }

    return PHYMOD_E_NONE;
    
}


int viper_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    viper_an_adv_ability_t value;

    PHYMOD_MEMSET(&value, 0x0, sizeof(value));

    PHYMOD_IF_ERR_RETURN(viper_autoneg_remote_ability_get(&phy->access, &value));
    switch (value.cl37_sgmii_speed){
    case VIPER_CL37_SGMII_10M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_10M;
        break;
    case VIPER_CL37_SGMII_100M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_100M;
        break;
    case VIPER_CL37_SGMII_1000M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_1000M;
        break;
    default:
        break;
    }

    switch(value.an_pause) {
    case VIPER_ASYM_PAUSE:
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        break;
    case VIPER_SYMM_PAUSE:
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        break;
    case VIPER_ASYM_SYMM_PAUSE:
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        break;
    default:
        break;
    }

    return PHYMOD_E_NONE;
}


int viper_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    PHYMOD_IF_ERR_RETURN(viper_autoneg_set(&phy->access, an));

    return PHYMOD_E_NONE;
    
}

int viper_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
       
    PHYMOD_IF_ERR_RETURN(viper_autoneg_get(&phy->access, an, an_done));
       
    return PHYMOD_E_NONE;
    
}


int viper_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
   return (viper_autoneg_status_get(&phy->access, status)); 
}

int viper_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    phymod_core_access_t  core_copy;
    phymod_core_access_t  core_copy2;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;
    PHYMOD_MEMCPY(&core_copy2, core, sizeof(core_copy2));

    /* Enable multiple MMD */
    PHYMOD_IF_ERR_RETURN(viper_multimmds_set(&core_copy.access, 1));

    if (init_config->interface.interface_type == phymodInterfaceXAUI){
        /* SGMIIPLUS2_X4 core doesn't support 10G */
        if(VIPER_CORE_IS_SGMIIPLUS2_X4(&core->access)) {
            return PHYMOD_E_UNAVAIL;
        }
        PHYMOD_IF_ERR_RETURN(viper_multimmds_set(&core_copy.access, 0));
        PHYMOD_IF_ERR_RETURN(viper_pll_disable_forced_10G(&core_copy.access));
        PHYMOD_IF_ERR_RETURN
            (viper_xgxs_sel_enable(&core_copy.access, 1));

        if ((core_copy2.access.lane_mask == 0x1) || (core_copy2.access.lane_mask == 0xf)){
            PHYMOD_IF_ERR_RETURN
                (viper_core_lane_map_set(&core_copy2, &init_config->lane_map));
        }
        PHYMOD_IF_ERR_RETURN(viper_pll_enable_forced_10G(&core_copy.access));
    } else {
        /* Some cores' TXPLL are not stable when reset chip with default cal_th value 0x6.
         * To get TXPLL stable, change SGMIIPlus2 PLL calibration threshold from 0x6 to 0xc before PLL disable/enable.
         * To be fixed after ASIC/DEV team confirm the rootcause.
        */
        if(VIPER_CORE_IS_SGMIIPLUS2_X4(&core->access)) {
            PHYMOD_IF_ERR_RETURN(viper_cal_th_set(&core_copy.access, 0xc));
        }

        PHYMOD_IF_ERR_RETURN(viper_pll_disable(&core_copy.access));
        PHYMOD_IF_ERR_RETURN
            (viper_xgxs_sel_enable(&core_copy.access, 1));
        /* SGMIIPLUS2_X4 doesn't support lane swap */
        if (VIPER_CORE_IS_XGXS_10G_028(&core->access)) {
            if ((core_copy2.access.lane_mask == 0x1) || (core_copy2.access.lane_mask == 0xf)){
                PHYMOD_IF_ERR_RETURN
                    (viper_core_lane_map_set(&core_copy2, &init_config->lane_map));
            }
        }
        PHYMOD_IF_ERR_RETURN(viper_pll_enable(&core_copy.access));

    }
    return PHYMOD_E_NONE;
}


int viper_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* core_vco_pll_multiplier)
{
    
    /* NOT NEED REMOVE LATER  */

    return PHYMOD_E_NONE;
    
}


int viper_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{

    /*int pll_restart = 0;*/
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    phymod_polarity_t tmp_pol;
    int start_lane = 0, num_lane = 0, i; 

    PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));

    /* set the receive input voltage (differential pk-pk) to 1200 mVppd */
    for ( i = 0; i < num_lane; i++){
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);

        PHYMOD_IF_ERR_RETURN
            (viper_rx_large_swing_set(&pm_phy_copy.access, init_config->rx_swing));
    }

    /* program the rx/tx polarity */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        tmp_pol.tx_polarity = (init_config->polarity.tx_polarity) >> i & 0x1;
        tmp_pol.rx_polarity = (init_config->polarity.rx_polarity) >> i & 0x1;
        PHYMOD_IF_ERR_RETURN
            (viper_phy_polarity_set(&pm_phy_copy, &tmp_pol));
    }

    for(i = 0; i < num_lane; i++)
    {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        /*init hw rx_los:enable signal detect*/
        if(init_config->rx_los.rx_los_en)
        {
            PHYMOD_IF_ERR_RETURN
                (viper_signal_detect_set(&pm_phy_copy.access));
        }

         /*init hw rx_los:invert signal detect*/
        if(init_config->rx_los.rx_los_invert_en)
        {
            PHYMOD_IF_ERR_RETURN
                (viper_signal_invert_set(&pm_phy_copy.access));
        }
    }
#if 0
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (viper_phy_tx_set(&pm_phy_copy, &init_config->tx[i]));
    }
#endif
    for(i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (viper_sigdet_threshold_set(&pm_phy_copy.access, 0xd));
    }
    pm_phy_copy.access.lane_mask = 0x1;

    /* PHYMOD_IF_ERR_RETURN
        (viper_update_port_mode(pm_acc, &pll_restart));

    PHYMOD_IF_ERR_RETURN
        (viper_rx_lane_control_set(pm_acc, 1));
    PHYMOD_IF_ERR_RETURN
        (viper_tx_lane_control_set(pm_acc, TEMOD_TX_LANE_RESET_TRAFFIC_ENABLE));       
     */

    return PHYMOD_E_NONE;
 
}


int viper_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    /*PHYMOD_IF_ERR_RETURN(viper_global_loopback_set(&phy->access, (uint8_t) enable));*/
    viper_actual_speed_id_t speed_id;
    uint32_t fx_mode = 0;

    PHYMOD_IF_ERR_RETURN(viper_actual_speed_get(&phy->access, &speed_id));

    switch (loopback) {
    case phymodLoopbackGlobal :
        if (speed_id == VIPER_SPD_10G_CX4) {
            /* SGMIIPLUS2_X4 core doesn't support 10G */
            if(VIPER_CORE_IS_SGMIIPLUS2_X4(&phy->access)) {
                return PHYMOD_E_UNAVAIL; 
            }
            PHYMOD_IF_ERR_RETURN(viper_10g_global_loopback_set(&phy->access, (uint8_t) enable));
        } else { 
            PHYMOD_IF_ERR_RETURN(viper_fiber_force_100FX_get(&phy->access, &fx_mode));
            if ((speed_id == VIPER_SPD_100M) && fx_mode ){
                PHYMOD_IF_ERR_RETURN(viper_100FX_global_loopback_set(&phy->access, (uint8_t) enable));
            } else {
                PHYMOD_IF_ERR_RETURN(viper_global_loopback_set(&phy->access, (uint8_t) enable));
            }
        }
        break;
    case phymodLoopbackGlobalPMD :
         return PHYMOD_E_UNAVAIL;
    case phymodLoopbackRemotePCS :
         break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(viper_remote_loopback_set(&phy->access, speed_id, (uint8_t)enable));
        break;
    default :
        break;
    }    

    return  PHYMOD_E_NONE;
}

int viper_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    viper_actual_speed_id_t speed_id;
    uint32_t fx_mode = 0;

    PHYMOD_IF_ERR_RETURN(viper_actual_speed_get(&phy->access, &speed_id));

    switch (loopback) {
    case phymodLoopbackGlobal :
        if (speed_id == VIPER_SPD_10G_CX4)  {
            /* SGMIIPLUS2_X4 core doesn't support 10G */
            if(VIPER_CORE_IS_SGMIIPLUS2_X4(&phy->access)) {
                return PHYMOD_E_UNAVAIL;
            }
            PHYMOD_IF_ERR_RETURN(viper_10g_global_loopback_get(&phy->access, enable));
        } else {
            PHYMOD_IF_ERR_RETURN(viper_fiber_force_100FX_get(&phy->access, &fx_mode));
            if ((speed_id == VIPER_SPD_100M) && fx_mode ){
                PHYMOD_IF_ERR_RETURN(viper_100FX_global_loopback_get(&phy->access, enable));
            } else {
                PHYMOD_IF_ERR_RETURN(viper_global_loopback_get(&phy->access, enable));
            }
        }
        break;
    case phymodLoopbackGlobalPMD :
        return PHYMOD_E_UNAVAIL;
    case phymodLoopbackRemotePCS :
         break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(viper_remote_loopback_get(&phy->access, speed_id, enable));
        break;
    default :
        break;
    }
    return PHYMOD_E_NONE;
}


int viper_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{

    /* Viper has no hardware to get rx_pmd_locked status */
    *rx_pmd_locked = 1;

    return PHYMOD_E_NONE;
    
}


int viper_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    viper_actual_speed_id_t speed_id;

    PHYMOD_IF_ERR_RETURN(viper_actual_speed_get(&phy->access, &speed_id));
    
    if (speed_id == VIPER_SPD_10G_CX4 ) {
        /* SGMIIPLUS2_X4 core doesn't support 10G */
        if(VIPER_CORE_IS_SGMIIPLUS2_X4(&phy->access)) {
            return PHYMOD_E_UNAVAIL;
        }
        PHYMOD_IF_ERR_RETURN (viper_get_link_status_10G(&phy->access, link_status));
    } else {
        PHYMOD_IF_ERR_RETURN (viper_get_link_status(&phy->access, link_status));
    }
    return PHYMOD_E_NONE;

    
}


int viper_phy_pcs_userspeed_set(const phymod_phy_access_t* phy, const phymod_pcs_userspeed_config_t* config)
{
        
    
    /* NOT NEED REMOVE LATER */
        
    return PHYMOD_E_NONE;
    
}

int viper_phy_pcs_userspeed_get(const phymod_phy_access_t* phy, phymod_pcs_userspeed_config_t* config)
{
    /* NOT NEED REMOVE LATER */
        
    return PHYMOD_E_NONE;
    
}

int viper_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    phymod_access_t phy_copy;
    int start_lane = 0, num_lane = 0;

    PHYMOD_MEMCPY(&phy_copy, &(phy->access), sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy_copy, &start_lane, &num_lane));

     phy_copy.lane_mask = 0x1 << start_lane;

     PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_read(&phy_copy, reg_addr, val));

    return(phymod_tsc_iblk_read(&phy->access, reg_addr, val));
}


int viper_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    phymod_access_t phy_copy;
    int start_lane = 0, num_lane = 0, i;

    PHYMOD_MEMCPY(&phy_copy, &(phy->access), sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy_copy, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
         phy_copy.lane_mask = 0x1 << (i + start_lane);

         PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_write(&phy_copy, reg_addr, val));
    }

    return PHYMOD_E_NONE;
}

int viper_phy_rx_ppm_get(const phymod_phy_access_t* phy, uint32_t* rx_ppm)
{
    /* Viper doesn't support rx_ppm which is from DSC. */
    return PHYMOD_E_UNAVAIL;
}

int viper_phy_duplex_set(const phymod_phy_access_t* phy, phymod_duplex_mode_t duplex)
{
    viper_duplex_mode_t duplex_mode;

    switch (duplex) {
        case phymodDuplexHalf:
            duplex_mode = VIPER_DUPLEX_HALF;
            break;
        case phymodDuplexFull:
            duplex_mode = VIPER_DUPLEX_FULL;
            break;
        default:
            duplex_mode = VIPER_DUPLEX_FULL;
            break;
    }
    PHYMOD_IF_ERR_RETURN(viper_duplex_set(&phy->access, duplex_mode));

    return PHYMOD_E_NONE;
}

int viper_phy_duplex_get(const phymod_phy_access_t* phy, phymod_duplex_mode_t* duplex)
{
    viper_duplex_mode_t duplex_mode;

    PHYMOD_IF_ERR_RETURN(viper_duplex_get(&phy->access, &duplex_mode));
    switch (duplex_mode) {
        case VIPER_DUPLEX_HALF:
            *duplex = phymodDuplexHalf;
            break;
        case VIPER_DUPLEX_FULL:
            *duplex = phymodDuplexFull;
            break;
        default:
            *duplex = phymodDuplexFull;
            break;
    }

    return PHYMOD_E_NONE;
}

