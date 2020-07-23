/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  tomahawk2_mac_flexport.c
*/


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#include <soc/tomahawk.h>
#include <soc/tomahawk2_tdm.h>
#include <soc/flexport/tomahawk2_flexport.h>


/*** START SDK API COMMON CODE ***/


/*! @file tomahawk2_mac_flexport.c
 *  @brief MAC flex for Tomahawk2.
 *  Details are shown below.
 */

int
soc_tomahawk2_flex_mac_rx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint64 rval64, fldval64;
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

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, phy_port,
                                                    0, &rval64));
            COMPILER_64_ZERO(fldval64);
            soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, RX_ENf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, phy_port,
                                                    0, rval64));

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_TX_CTRLr,
                                                    phy_port, 0, &rval64));
            COMPILER_64_SET(fldval64, 0, 1);
            soc_reg64_field_set(
                      unit, CLMAC_TX_CTRLr, &rval64, DISCARDf, fldval64);
            soc_reg64_field_set(
                      unit, CLMAC_TX_CTRLr, &rval64, EP_DISCARDf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_TX_CTRLr,
                                                    phy_port, 0, rval64));
        }
    }
    return SOC_E_NONE;
}

int
soc_tomahawk2_flex_mac_tx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint64 rval64, fldval64;
    int phy_port;

    /* Disable MAC TX_EN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i]
                    .logical_port];

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, phy_port,
                                                    0, &rval64));
            COMPILER_64_ZERO(fldval64);
            soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, TX_ENf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, phy_port,
                                                    0, rval64));
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

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, phy_port,
                                                    0, &rval64));
            COMPILER_64_SET(fldval64, 0, 1);
            soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, SOFT_RESETf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, phy_port,
                                                    0, rval64));
        }
    }
    return SOC_E_NONE;
}

int
soc_tomahawk2_flex_mac_port_up(int                        unit,
                               soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint64 rval64, fldval64;
    uint32 rval32;
    int phy_port;
    int port;
    int subp;
    int mode;
    int speed_100g;
    int clport;
    int higig2_mode;
    int strict_preamble;
    int higig_mode;
    uint64 average_ipg;
    static const int clport_mode_values[SOC_TH_PORT_RATIO_COUNT] = {
        4, 3, 3, 3, 2, 2, 1, 1, 0
    };


    /*Disable TSC lanes: */

    /*CLMAC_RX_CTRL */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            higig2_mode =
                ((port_schedule_state->resource[i].encap ==
                  _SHR_PORT_ENCAP_HIGIG2)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_LITE)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_L2)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_IP_GRE));
            higig_mode =
                (port_schedule_state->resource[i].encap ==
                 _SHR_PORT_ENCAP_HIGIG);
            strict_preamble = (higig2_mode||higig_mode)?0:1;
            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_RX_CTRLr,
                                                    phy_port, 0, &rval64));
            COMPILER_64_ZERO(fldval64);
            soc_reg64_field_set(
                      unit, CLMAC_RX_CTRLr, &rval64, RX_ANY_STARTf, fldval64);
            soc_reg64_field_set(
                      unit, CLMAC_RX_CTRLr, &rval64, STRIP_CRCf, fldval64);
            COMPILER_64_SET(fldval64, 0, strict_preamble);
            soc_reg64_field_set(unit, CLMAC_RX_CTRLr, &rval64, STRICT_PREAMBLEf,
                                fldval64);
            COMPILER_64_SET(fldval64, 0, 64);
            soc_reg64_field_set(unit, CLMAC_RX_CTRLr, &rval64, RUNT_THRESHOLDf,
                                fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_RX_CTRLr,
                                                    phy_port, 0, rval64));
        }
    }

    /*CLPORT_CONFIG */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            higig2_mode =
                ((port_schedule_state->resource[i].encap ==
                  _SHR_PORT_ENCAP_HIGIG2)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_LITE)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_L2)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_IP_GRE));
            higig_mode =
                (port_schedule_state->resource[i].encap ==
                 _SHR_PORT_ENCAP_HIGIG);
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, CLPORT_CONFIGr,
                                                      phy_port, 0, &rval32));
            soc_reg_field_set(unit, CLPORT_CONFIGr, &rval32, HIGIG2_MODEf,
                              higig2_mode);
            soc_reg_field_set(unit, CLPORT_CONFIGr, &rval32, HIGIG_MODEf,
                              higig_mode);
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, CLPORT_CONFIGr,
                                                      phy_port, 0, rval32));
        }
    }

    /*CLPORT Enable: */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            subp = soc_tomahawk2_get_subp_from_phy_pnum(phy_port);

            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, CLPORT_ENABLE_REGr,
                                                      phy_port, 0, &rval32));
            if (subp == 0) {
                soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &rval32, PORT0f, 1);
            } else if (subp==1) {
                soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &rval32, PORT1f, 1);
            } else if (subp==2) {
                soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &rval32, PORT2f, 1);
            } else {
                soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &rval32, PORT3f, 1);
            }
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, CLPORT_ENABLE_REGr,
                                                      phy_port, 0, rval32));
        }
    }
    sal_usleep(100 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);

    /*CLPORT mode: */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            subp = soc_tomahawk2_get_subp_from_phy_pnum(phy_port);
            clport = ((phy_port-1)>>2);
            soc_tomahawk2_port_ratio_get(unit, port_schedule_state, clport,
                                         &mode,
                                         0);
            if (port_schedule_state->out_port_map.log_port_speed[port]>=
                100000) {
                speed_100g = 1;
            } else {
                speed_100g = 0;
            }

            SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, CLPORT_MODE_REGr,
                                                      phy_port, 0, &rval32));
            soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval32,
                              SINGLE_PORT_MODE_SPEED_100Gf,
                              speed_100g);
            soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval32,
                              XPORT0_CORE_PORT_MODEf,
                              clport_mode_values[mode]);
            soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval32,
                              XPORT0_PHY_PORT_MODEf,
                              clport_mode_values[mode]);
            SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, CLPORT_MODE_REGr,
                                                      phy_port, 0, rval32));
        }
    }
    sal_usleep(200 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);

    /* Release soft reset */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, phy_port,
                                                    0, &rval64));
            COMPILER_64_ZERO(fldval64);
            soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, SOFT_RESETf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, phy_port,
                                                    0, rval64));
        }
    }

    sal_usleep(100 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
    /* Enable MAC RX_EN & TX_EN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            higig2_mode =
                ((port_schedule_state->resource[i].encap ==
                  _SHR_PORT_ENCAP_HIGIG2)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_LITE)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_L2)||
                 (port_schedule_state->resource[i].encap ==
             _SHR_PORT_ENCAP_HIGIG2_IP_GRE));
            if (higig2_mode) {
               COMPILER_64_SET(average_ipg, 0, CLMAC_AVERAGE_IPG_HIGIG);
            } else {
               COMPILER_64_SET(average_ipg, 0, CLMAC_AVERAGE_IPG_DEFAULT);
            }

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_TX_CTRLr,
                                                    phy_port, 0, &rval64));
            COMPILER_64_ZERO(fldval64);
            soc_reg64_field_set(unit, CLMAC_TX_CTRLr, &rval64, DISCARDf, fldval64);
            soc_reg64_field_set(unit, CLMAC_TX_CTRLr, &rval64, EP_DISCARDf, fldval64);
            soc_reg64_field_set(unit, CLMAC_TX_CTRLr, &rval64, AVERAGE_IPGf, average_ipg);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_TX_CTRLr,
                                                    phy_port, 0, rval64));

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, phy_port,
                                                    0, &rval64));
            COMPILER_64_SET(fldval64, 0, 1);
            soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, RX_ENf, fldval64);
            soc_reg64_field_set(unit, CLMAC_CTRLr, &rval64, TX_ENf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, phy_port,
                                                    0, rval64));

        }
    }

    return SOC_E_NONE;
}


#endif /* BCM_TOMAHAWK2_SUPPORT */
