/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk3_port_init.c
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


/*** START SDK API COMMON CODE ***/


/*! @file tomahawk3_port_init.c
 *  @brief PORT init for Tomahawk3. Used only when TSC stubbed out.
 *  Details are shown below.
 */


/*! @fn int soc_tomahawk3_tscbh_reset(int unit, int pmnum)
 *  @param unit Chip unit number.
 *  @param pmnum PM_8x50 number. Value can be 0 to 31
 *  @param reset TSC reset. 1 - Apply reset,
                            0 - Release reset.
 *  @brief Helper function to apply/ or release TSC reset.
 *  @returns SOC_E_NONE
 */

int
soc_tomahawk3_tscbh_reset(
    int unit, int pmnum, int reset)
{
    uint32 rval32;
    int phy_port;
    int rst_l, pll_select;

    /* always use PLL1 for the tsc clock source for PM8x50 */
    pll_select = 1;
    /* Write CDPORT_XGXS0_CTRL_REG */
    phy_port = 1 + (pmnum * _TH3_PORTS_PER_PBLK);
    rst_l = reset ? 0 : 1;
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
          CDPORT_XGXS0_CTRL_REGr, phy_port, 0, &rval32));
    soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr,
                                 &rval32, TSC_RSTBf, rst_l);

    /*config the tsc power dwon bit */
    soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr,
                                 &rval32, TSC_PWRDWNf, reset);

    /* Choose the PLL1 as the tsc clock for PM8x50 */
    if (!reset) {
        soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr,
                                     &rval32, TSC_CLK_SELf, pll_select);
    }

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit,
       CDPORT_XGXS0_CTRL_REGr, phy_port, 0, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_cdmac_reset(int unit,
            soc_port_map_type_t *port_map, int pmnum, int reset)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param pmnum PM_8x50 number. Value can be 0 to 31
 *  @param reset IDB CA write clock domain reset. 1 - Apply reset,
                                                  0 - Release reset.
 *  @brief Helper function to apply/ or release CDMAC reset.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_cdmac_reset(
    int unit, soc_port_map_type_t *port_map, int pmnum, int reset)
{
    uint32 rval32;
    int phy_port;
    int lport;
    int cdmac1_reset;

    /* Write CDPORT_MAC_CONTROLr */
    /* if using 400G MAC mode (needed for speeds >= 300G),
       keep CDMAC1 in reset.*/
    phy_port = 1 + (pmnum * _TH3_PORTS_PER_PBLK);
    lport = port_map->port_p2l_mapping[phy_port];
    if (port_map->log_port_speed[lport] >= 300000) {
       cdmac1_reset = 1;
    } else {
       cdmac1_reset = reset;
    }
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
          CDPORT_MAC_CONTROLr, phy_port, 0, &rval32));
    soc_reg_field_set(unit, CDPORT_MAC_CONTROLr,
                &rval32, CDMAC0_RESETf, reset);
    soc_reg_field_set(unit, CDPORT_MAC_CONTROLr,
                &rval32, CDMAC1_RESETf, cdmac1_reset);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit,
       CDPORT_MAC_CONTROLr, phy_port, 0, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_set_cdmac_port_mode(int unit,
            soc_port_map_type_t *port_map, int pmnum)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param pmnum PM_8x50 number. Value can be 0 to 31
 *  @brief Helper function to set CDMAC port mode.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_set_cdmac_port_mode(
    int unit, soc_port_map_type_t *port_map, int pmnum)
{
    uint32 rval32;
    int phy_port;
    int cdmac0_port_mode;
    int cdmac1_port_mode;
    int speed_400g;
    /* Write CDPORT_MODE_REGr */
    phy_port = 1 + (pmnum * _TH3_PORTS_PER_PBLK);
    soc_tomahawk3_port_mode_get(port_map,
                     pmnum, &cdmac0_port_mode,
                     &cdmac1_port_mode, &speed_400g);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
          CDPORT_MODE_REGr, phy_port, 0, &rval32));
    soc_reg_field_set(unit, CDPORT_MODE_REGr,
                &rval32, SINGLE_PORT_MODE_SPEED_400Gf, speed_400g);
    soc_reg_field_set(unit, CDPORT_MODE_REGr,
                &rval32, MAC0_PORT_MODEf, cdmac0_port_mode);
    soc_reg_field_set(unit, CDPORT_MODE_REGr,
                &rval32, MAC1_PORT_MODEf, cdmac1_port_mode);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit,
       CDPORT_MODE_REGr, phy_port, 0, rval32));
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_cdmac_enable(int unit, int phy_port)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param pmnum Physical port number. Value can be 1 to 256.
 *  @brief Helper function to enable CDMAC.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_cdmac_enable(
    int unit, int phy_port)
{
    uint32 rval32;
    /* Write CDMAC_CTRLr */
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
          CDMAC_CTRLr, phy_port, 0, &rval32));
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval32, RX_ENf, 1);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval32, TX_ENf, 1);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval32, SOFT_RESETf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit,
       CDMAC_CTRLr, phy_port, 0, rval32));
    return SOC_E_NONE;
}

/*! @fn void soc_tomahawk3_port_mode_get(soc_port_map_type_t
 *               *port_map, int pmnum, int *cdmac0_port_mode,
 *               int *cdmac1_port_mode, int *speed_400g)
 *  @param port_map Pointer to a soc_port_map_type_t struct variable
 *  @param pmnum PM number (0 to 31)
 *  @param *cdmac0_port_mode Port mode for CDMAC0
 *  @param *cdmac1_port_mode Port mode for CDMAC1
 *  @param *speed_400g Speed 400G indication
 *  @brief Help function to get port ratio
 * Description:
 *     Help function to get port ratio
 */
void
soc_tomahawk3_port_mode_get(soc_port_map_type_t *port_map,
                             int pmnum, int *cdmac0_port_mode,
                             int *cdmac1_port_mode, int *speed_400g)
{
    int port, phy_port_base, lane;
    int num_lanes[_TH3_PORTS_PER_PBLK];

    phy_port_base = 1 + (pmnum * _TH3_PORTS_PER_PBLK);
    for (lane = 0; lane < _TH3_PORTS_PER_PBLK; lane += 2) {
        port = port_map->port_p2l_mapping[phy_port_base + lane];
        if ( (port == -1) || (port_map->log_port_speed[port] == 0) ) {
            num_lanes[lane] = -1;
        } else {
            num_lanes[lane] = port_map->port_num_lanes[port];
        }
    }
    *speed_400g = (num_lanes[0] == 8) ? 1 : 0;
    *cdmac0_port_mode =0;
    *cdmac1_port_mode =0;
    *cdmac0_port_mode |= (((num_lanes[0] == 4) ||
                        (num_lanes[0] == 8)) ? 1 : 0) << 2; /*Bit2*/
    *cdmac0_port_mode |= ((num_lanes[0] == 2) ? 1 : 0) << 1; /*Bit1*/
    *cdmac0_port_mode |= (num_lanes[2] == 2) ? 1 : 0; /*Bit0*/
    *cdmac1_port_mode |= (((num_lanes[4] == 4) ||
                        (num_lanes[0] == 8)) ? 1 : 0) << 2; /*Bit2*/
    *cdmac1_port_mode |= ((num_lanes[4] == 2) ? 1 : 0) << 1; /*Bit1*/
    *cdmac1_port_mode |= (num_lanes[6] == 2) ? 1 : 0; /*Bit0*/
}

/*! @fn int soc_tomahawk3_tsc_clock_enable(int unit,
                       soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @brief Helper function to release TSCBH reset during init.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_tsc_clock_enable(int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int phy_port;
    int pmnum;
    int pm_going_up;
    int lport;
    int reset;

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "Call soc_tomahawk3_tsc_clock_enable \n")));

    /* If multiple ports going up in same CDPORT, write only once. */
    for (pmnum = 0; pmnum < _TH3_NUM_OF_TSCBH ; pmnum++) {
        pm_going_up=0;
        for (lport = 0; lport < _TH3_DEV_PORTS_PER_DEV; lport++) {
            if (port_schedule_state->out_port_map.log_port_speed[lport] > 0 ) {
                phy_port =
                    port_schedule_state->out_port_map.port_l2p_mapping[lport];
                if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port) && (((phy_port-1)>>3) == pmnum)) {
                    pm_going_up|= 1;
                }
            }
        }
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "pmnum %0d pm_going_up=%0d"),
                  pmnum, pm_going_up));
        reset=0;
        if (pm_going_up){
            SOC_IF_ERROR_RETURN(soc_tomahawk3_tscbh_reset(
                                unit, pmnum,reset));
        }
    }
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_pm8x50_port_init(int unit,
                       soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
                               struct variable
 *  @brief Helper function for pm8x50 port init during bringup.
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_pm8x50_port_init(int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int phy_port;
    int pmnum;
    int pm_going_up;
    int lport;
    int reset;

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "Call soc_tomahawk3_pm8x50_port_init \n")));

    /* If multiple ports going up in same CDPORT, write only once. */
    for (pmnum = 0; pmnum < _TH3_NUM_OF_TSCBH ; pmnum++) {
        pm_going_up=0;
        for (lport = 0; lport < _TH3_DEV_PORTS_PER_DEV; lport++) {
            if (port_schedule_state->out_port_map.log_port_speed[lport] > 0 ) {
                phy_port =
                    port_schedule_state->out_port_map.port_l2p_mapping[lport];
                if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)&&
                    (((phy_port-1)>>3) == pmnum)) {
                    pm_going_up|= 1;
                }
            }
        }
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "pmnum %0d pm_going_up=%0d"),
                  pmnum, pm_going_up));
        reset=0;
        if (pm_going_up){
            SOC_IF_ERROR_RETURN(soc_tomahawk3_cdmac_reset(
                  unit, &port_schedule_state->out_port_map,pmnum,reset));
        }
    }

    /* If multiple ports going up in same CDPORT, write only once. */
    for (pmnum = 0; pmnum < _TH3_NUM_OF_TSCBH ; pmnum++) {
        pm_going_up=0;
        for (lport = 0; lport < _TH3_DEV_PORTS_PER_DEV; lport++) {
            if (port_schedule_state->out_port_map.log_port_speed[lport] > 0 ) {
                phy_port =
                    port_schedule_state->out_port_map.port_l2p_mapping[lport];
                if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)&&
                    (((phy_port-1)>>3) == pmnum)) {
                    pm_going_up|= 1;
                }
            }
        }
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "pmnum %0d pm_going_up=%0d"),
                  pmnum, pm_going_up));
        reset=0;
        if (pm_going_up){
            SOC_IF_ERROR_RETURN(soc_tomahawk3_set_cdmac_port_mode(
                  unit, &port_schedule_state->out_port_map,pmnum));
        }
    }

    /* Per port registers need to be written for each port */
    for (lport = 1; lport < _TH3_DEV_PORTS_PER_DEV; lport++) {
        if (port_schedule_state->out_port_map.log_port_speed[lport] > 0 ) {
            phy_port =
                port_schedule_state->out_port_map.port_l2p_mapping[lport];
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)){
                SOC_IF_ERROR_RETURN(soc_tomahawk3_cdmac_enable(
                                         unit, phy_port));
            }
        }
    }

    return SOC_E_NONE;
}

/*** END SDK API COMMON CODE ***/

#endif /* BCM_TOMAHAWK3_SUPPORT */

