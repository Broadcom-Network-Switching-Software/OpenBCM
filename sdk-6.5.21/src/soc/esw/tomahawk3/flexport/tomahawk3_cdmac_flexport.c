/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk3_cdmac_flexport.c
 * Purpose:
 * Requires:
 */

#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>

#include <soc/defs.h>
#include <soc/mem.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <soc/tomahawk.h>
#include <soc/init/tomahawk3_port_init.h>
#include <soc/flexport/tomahawk3_cdmac_flexport.h>

/*** START SDK API COMMON CODE ***/


/*! @file tomahawk3_cdmac_flexport.c
 *  @brief MAC flex for Tomahawk3
 *  Details are shown below.
 */

int
soc_tomahawk3_flex_mac_rx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint32 rval32;
    int phy_port;

    /*Disable TSC lanes: */

    /* Disable MAC RX_EN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i]
                    .logical_port];

            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, CDMAC_CTRLr, phy_port,
                                                    0, &rval32));
            soc_reg_field_set(unit, CDMAC_CTRLr, &rval32, RX_ENf, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, CDMAC_CTRLr, phy_port,
                                                    0, rval32));

            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, CDMAC_TX_CTRLr,
                                                    phy_port, 0, &rval32));
            soc_reg_field_set(
                      unit, CDMAC_TX_CTRLr, &rval32, DISCARDf, 1);
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, CDMAC_TX_CTRLr,
                                                    phy_port, 0, rval32));
        }
    }
    return SOC_E_NONE;
}

int
soc_tomahawk3_flex_mac_tx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint32 rval32;
    int phy_port;

    /* Disable MAC TX_EN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[
                    port_schedule_state->resource[i].logical_port];

            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, CDMAC_CTRLr, phy_port,
                                                    0, &rval32));
            soc_reg_field_set(unit, CDMAC_CTRLr, &rval32, TX_ENf, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, CDMAC_CTRLr, phy_port,
                                                    0, rval32));
        }
    }

    /* Apply soft reset */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i]
                    .logical_port];

            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, CDMAC_CTRLr, phy_port,
                                                    0, &rval32));
            soc_reg_field_set(unit, CDMAC_CTRLr, &rval32, SOFT_RESETf, 1);
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, CDMAC_CTRLr, phy_port,
                                                    0, rval32));
        }
    }

    return SOC_E_NONE;
}


int
soc_tomahawk3_is_pm_flexing_up(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pm_num,
    int *pm_going_up)
{
    int i, phy_port;
    int l_pm_num;

    *pm_going_up = 0;
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            phy_port = port_schedule_state->resource[i].physical_port;
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                l_pm_num = (phy_port - 1) / _TH3_PORTS_PER_PBLK;
                if (pm_num == l_pm_num) {
                    *pm_going_up = 1;
                }
            }
        }
    }

    return SOC_E_NONE;
}


int
soc_tomahawk3_set_cdmac_tx_ctrl(
    int unit, 
    soc_port_schedule_state_t *port_schedule_state,
    int phy_port)
{
    uint32 rval32;

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, CDMAC_TX_CTRLr,
                                              phy_port, 0, &rval32));
    soc_reg_field_set(
              unit, CDMAC_TX_CTRLr, &rval32, DISCARDf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, CDMAC_TX_CTRLr,
              phy_port, 0, rval32));

    return SOC_E_NONE;
}


int
soc_tomahawk3_flex_mac_port_up(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int pm_num, pm_going_up, reset;

    /* APIs that have to be called per PM when PM is flexing UP */

    for (pm_num = 0; pm_num < _TH3_NUM_OF_TSCBH ; pm_num++) {
        SOC_IF_ERROR_RETURN(soc_tomahawk3_is_pm_flexing_up(
                            unit, port_schedule_state, pm_num, &pm_going_up));
        if (pm_going_up == 1) {
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_flex_pm8x50() pm_num=%0d pm_going_up=%0d Call soc_tomahawk3_cdmac_reset\n"),
                  pm_num, pm_going_up));
            reset = 0;
            SOC_IF_ERROR_RETURN(soc_tomahawk3_cdmac_reset(
                  unit, &port_schedule_state->out_port_map, pm_num, reset));
        }
    }

    for (pm_num = 0; pm_num < _TH3_NUM_OF_TSCBH ; pm_num++) {
        SOC_IF_ERROR_RETURN(soc_tomahawk3_is_pm_flexing_up(
                            unit, port_schedule_state, pm_num, &pm_going_up));
        if (pm_going_up == 1) {
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_flex_pm8x50() pm_num=%0d pm_going_up=%0d Call soc_tomahawk3_set_cdmac_port_mode\n"),
                  pm_num, pm_going_up));
            reset = 0;
            SOC_IF_ERROR_RETURN(soc_tomahawk3_set_cdmac_port_mode(
                  unit, &port_schedule_state->out_port_map, pm_num));
        }
    }

    return SOC_E_NONE;
}


int
soc_tomahawk3_flex_mac_enable(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
   int i, phy_port;


    /* Per port registers need to be programmed for each port going up */

    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            phy_port = port_schedule_state->resource[i].physical_port;
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)){
                SOC_IF_ERROR_RETURN(soc_tomahawk3_set_cdmac_tx_ctrl(
                                         unit, port_schedule_state, phy_port));
                SOC_IF_ERROR_RETURN(soc_tomahawk3_cdmac_enable(
                                         unit, phy_port));
            }
        }
    }

    return SOC_E_NONE;
}


int
soc_tomahawk3_flex_get_pm_state(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pm_num,
    int *pm_is_up_before_flex,
    int *pm_is_up_after_flex)
{
    int phy_port, log_port;
    int l_pm_num;
    
    *pm_is_up_before_flex = 0;
    for (log_port = 0; log_port < _TH3_DEV_PORTS_PER_DEV; log_port++) {
        if (port_schedule_state->in_port_map.log_port_speed[log_port] > 0 ) {
            phy_port = port_schedule_state->in_port_map.port_l2p_mapping[log_port];
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                l_pm_num = (phy_port - 1) / _TH3_PORTS_PER_PBLK;
                if (l_pm_num == pm_num) {
                    *pm_is_up_before_flex = 1;
                }
            }
        }
    }

    *pm_is_up_after_flex = 0;
    for (log_port = 0; log_port < _TH3_DEV_PORTS_PER_DEV; log_port++) {
        if (port_schedule_state->out_port_map.log_port_speed[log_port] > 0 ) {
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[log_port];
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                l_pm_num = (phy_port - 1) / _TH3_PORTS_PER_PBLK;
                if (l_pm_num == pm_num) {
                    *pm_is_up_after_flex = 1;
                }
            }
        }
    }

    return SOC_E_NONE;
}


int
soc_tomahawk3_flex_tsc(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int is_down)
{
    int pm_num, reset;
    int pm_is_up_before_flex, pm_is_up_after_flex;

    for (pm_num = 0; pm_num < _TH3_NUM_OF_TSCBH ; pm_num++) {
        SOC_IF_ERROR_RETURN(soc_tomahawk3_flex_get_pm_state(
                            unit, port_schedule_state, pm_num, 
                            &pm_is_up_before_flex,
                            &pm_is_up_after_flex));
        if ((is_down == 1) &&               /* this call is when whole PM goes DOWN  && */
            (pm_is_up_before_flex == 1) &&  /* PM was UP   before FLEX && */
            (pm_is_up_after_flex == 0) ) {   /* PM is  DOWN after  FLEX    */
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_flex_tsc() pm_num=%0d going DOWN assert rst TSC"
                  "pm_is_up_before_flex=%0d pm_is_up_after_flex=%0d\n"),
                  pm_num, pm_is_up_before_flex, pm_is_up_after_flex));
            reset = 1;
            SOC_IF_ERROR_RETURN(soc_tomahawk3_cdmac_reset(
                  unit, &port_schedule_state->out_port_map, pm_num, reset));
        }

        if ((is_down == 0) &&               /* this call is when whole PM comes UP  && */
            (pm_is_up_before_flex == 0) &&  /* PM was DOWN before FLEX && */
            (pm_is_up_after_flex == 1) ) {  /* PM is  UP   after  FLEX    */
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_flex_tsc() pm_num=%0d going UP release rst TSC"
                  "pm_is_up_before_flex=%0d pm_is_up_after_flex=%0d\n"),
                  pm_num, pm_is_up_before_flex, pm_is_up_after_flex));
            reset = 0;
            SOC_IF_ERROR_RETURN(soc_tomahawk3_cdmac_reset(
                  unit, &port_schedule_state->out_port_map, pm_num, reset));
        }
    }

    return SOC_E_NONE;
}


int
soc_tomahawk3_flex_tsc_down(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int is_down;

    is_down = 1;
    SOC_IF_ERROR_RETURN(soc_tomahawk3_flex_tsc(
                        unit, port_schedule_state, is_down));

    return SOC_E_NONE;
}


int
soc_tomahawk3_flex_tsc_up(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int is_down;

    is_down = 0;
    SOC_IF_ERROR_RETURN(soc_tomahawk3_flex_tsc(
                        unit, port_schedule_state, is_down));

    return SOC_E_NONE;
}

/*** END SDK API COMMON CODE ***/

#endif /* BCM_TOMAHAWK3_SUPPORT */
