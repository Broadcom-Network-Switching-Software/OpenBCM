/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  File:        hurricane4_mac_flexport.c
 *  Purpose:
 *  Requires:
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>
#include <soc/phyctrl.h>
#include <soc/phy/phyctrl.h>

#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#include <soc/hurricane4_tdm.h>
#include <soc/flexport/hurricane4/hurricane4_flexport.h>



/*** START SDK API COMMON CODE ***/

/*! @file hurricane4_mac_flexport.c
 *  @brief MAC flex for Trident3.
 *  Details are shown below.
 */

int
soc_hurricane4_flex_mac_rx_port_down(
        int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint64 rval64;
    int phy_port;
    int qmode;
    int inst;
    int lport;

    /*Disable TSC lanes: */

    /* Disable MAC RX_EN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[
                port_schedule_state
                ->resource[i]
                .logical_port];
            /* checking for qsgmii ports */
            if (soc_hurricane4_flex_is_pmq_port(phy_port)) {
                inst = ((phy_port - HR4_PMQ_START) / HR4_NUM_PMQ_PORTS |
                        SOC_REG_ADDR_INSTANCE_MASK);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst,
                            0, &rval64));
                qmode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64,
                        QMODEf);
            } else {
                qmode = 0;
            }
            /* GPORT and PMQ seems to have same port up/down seq. */
            if (((soc_hurricane4_flex_is_pmq_port(phy_port)) && (qmode)) ||
                (soc_hurricane4_flex_is_qgphy_port(phy_port))) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Bringing GPORT/PMQ rx port %0d down\n"), phy_port));
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, COMMAND_CONFIGr,
                            phy_port, 0, &rval64));
                soc_reg64_field32_set(unit, COMMAND_CONFIGr, &rval64,
                        RX_ENAf, 0);
                /*soc_reg64_field32_set(unit, COMMAND_CONFIGr, &rval64, IGNORE_TX_PAUSEf, 1);
                soc_reg64_field32_set(unit, COMMAND_CONFIGr, &rval64, PAUSE_IGNOREf, 1);*/
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, COMMAND_CONFIGr,
                            phy_port, 0, rval64));


                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, FLUSH_CONTROLr,
                            phy_port, 0, &rval64));
                soc_reg64_field32_set(unit, FLUSH_CONTROLr, &rval64, FLUSHf, 1);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, FLUSH_CONTROLr,
                            phy_port, 0, rval64));
                if (soc_hurricane4_flex_is_qgphy_port(phy_port) &&
                    soc_feature(unit, soc_feature_egphy_port_leverage)) {
                    lport = port_schedule_state->resource[i].logical_port;
                    if (EXT_PHY_SW_STATE(unit, lport) ||
                        INT_PHY_SW_STATE(unit, lport)) {
                        SOC_IF_ERROR_RETURN
                            (soc_phyctrl_notify(unit, lport,
                                                phyEventStop,
                                                PHY_STOP_DRAIN));
                    }
                }

            } else if (soc_hurricane4_flex_is_eagle_port(phy_port) ||
                       (soc_hurricane4_flex_is_pmq_port(phy_port) &&
                        (qmode == 0))) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Bringing Eagle rx port %0d down\n"), phy_port));

                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, XLMAC_CTRLr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, XLMAC_CTRLr, &rval64, RX_ENf, 0);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, XLMAC_CTRLr, phy_port,
                            0, rval64));

                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, XLMAC_TX_CTRLr,
                            phy_port, 0, &rval64));
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval64, DISCARDf, 1);
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval64, EP_DISCARDf, 1);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, XLMAC_TX_CTRLr,
                            phy_port, 0, rval64));
            } else if (soc_hurricane4_flex_is_falcon_port(phy_port)) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Bringing Falcon rx port %0d down\n"), phy_port));

                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval64, RX_ENf, 0);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, phy_port,
                            0, rval64));

                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_TX_CTRLr,
                            phy_port, 0, &rval64));
                soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval64, DISCARDf, 1);
                soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval64, EP_DISCARDf, 1);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_TX_CTRLr,
                            phy_port, 0, rval64));
            }
        }
    }
    return SOC_E_NONE;
}

int
soc_hurricane4_flex_mac_tx_port_down(
        int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint64 rval64;
    int phy_port;
    int qmode;
    int inst;
    int lport;

    /* Disable MAC TX_EN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[
                port_schedule_state
                ->resource[i]
                .logical_port];

            if (soc_hurricane4_flex_is_pmq_port(phy_port)) {
                inst = ((phy_port - HR4_PMQ_START) / HR4_NUM_PMQ_PORTS | SOC_REG_ADDR_INSTANCE_MASK);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst,
                            0, &rval64));
                qmode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64,
                        QMODEf);
            } else {
                qmode = 0;

            }
            /* GPORT and PMQ seems to have same port up/down seq. */
            if (((soc_hurricane4_flex_is_pmq_port(phy_port)) && (qmode)) ||
                (soc_hurricane4_flex_is_qgphy_port(phy_port))) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Bringing GPORT tx port %0d down\n"), phy_port));
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, COMMAND_CONFIGr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, COMMAND_CONFIGr, &rval64, TX_ENAf, 0);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, COMMAND_CONFIGr, phy_port,
                            0, rval64));
            } else if (soc_hurricane4_flex_is_eagle_port(phy_port) ||
                       (soc_hurricane4_flex_is_pmq_port(phy_port) &&
                        (qmode == 0))) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Bringing Eagle tx port %0d down\n"), phy_port));
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, XLMAC_CTRLr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, XLMAC_CTRLr, &rval64, TX_ENf, 0);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, XLMAC_CTRLr, phy_port,
                            0, rval64));
            }
            else if (soc_hurricane4_flex_is_falcon_port(phy_port)) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Bringing Falcon tx port %0d down\n"), phy_port));
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval64, TX_ENf, 0);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, phy_port,
                            0, rval64));
            }
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

            if (soc_hurricane4_flex_is_pmq_port(phy_port)) {
                inst = ((phy_port - HR4_PMQ_START) / HR4_NUM_PMQ_PORTS | SOC_REG_ADDR_INSTANCE_MASK);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst,
                            0, &rval64));
                qmode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64,
                        QMODEf);
            } else {
                qmode = 0;

            }
            /* GPORT and PMQ seems to have same port up/down seq. */
            if (((soc_hurricane4_flex_is_pmq_port(phy_port)) && (qmode)) ||
                (soc_hurricane4_flex_is_qgphy_port(phy_port))) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Applying soft reset to gport %0d \n"), phy_port));
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, COMMAND_CONFIGr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, COMMAND_CONFIGr, &rval64, SW_RESETf, 1);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, COMMAND_CONFIGr, phy_port,
                            0, rval64));
                if (soc_hurricane4_flex_is_qgphy_port(phy_port) &&
                    soc_feature(unit, soc_feature_egphy_port_leverage)) {
                    lport = port_schedule_state->resource[i].logical_port;
                    if (EXT_PHY_SW_STATE(unit, lport) ||
                        INT_PHY_SW_STATE(unit, lport)) {
                        SOC_IF_ERROR_RETURN
                            (soc_phyctrl_notify(unit, lport,
                                                phyEventStop,
                                                PHY_STOP_MAC_DIS));
                    }
                }
                /* adding delay for port drain 
                 * as per david there will be max 24 entries
 */
                sal_usleep(410);

            } else if (soc_hurricane4_flex_is_eagle_port(phy_port) ||
                       (soc_hurricane4_flex_is_pmq_port(phy_port) &&
                        (qmode == 0))) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Applying soft reset to port %0d \n"), phy_port));
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, XLMAC_CTRLr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, XLMAC_CTRLr, &rval64, SOFT_RESETf, 1);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, XLMAC_CTRLr, phy_port,
                            0, rval64));
            } else if (soc_hurricane4_flex_is_falcon_port(phy_port)) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Applying soft reset to port %0d \n"), phy_port));
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval64, SOFT_RESETf, 1);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, phy_port,
                            0, rval64));
            }
        }
    }
    return SOC_E_NONE;
}

int
soc_hurricane4_flex_mac_port_up(int                        unit,
        soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint64 rval64;
    uint32 rval32;
    int phy_port;
    int port;
    int subp;
    int mode;
    int speed_100g;
    int clport;
    int higig2_mode;
    int strict_preamble=0;
    int higig_mode;
    int qmode;
    int inst;
    int speed_mode;
    int hdr_mode;
    int lport;
    static const int clport_mode_values[SOC_HR4_PORT_RATIO_COUNT] = {
        4, 3, 3, 3, 2, 2, 1, 1, 0
    };


    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "START soc_hurricane4_flex_mac_port_up\n")));
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
            /*strict_preamble = (higig2_mode||higig_mode)?0:1;*/
            if (!(higig2_mode||higig_mode) && (port_schedule_state->resource[i].speed >= 10000)) {
              strict_preamble = 1;
            }
            if (soc_hurricane4_flex_is_pmq_port(phy_port)) {
                inst = ((phy_port - HR4_PMQ_START) / HR4_NUM_PMQ_PORTS | SOC_REG_ADDR_INSTANCE_MASK);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst,
                            0, &rval64));
                qmode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64,
                        QMODEf);
            } else {
                qmode = 0;

            }
            /* GPORT and PMQ seems to have same port up/down seq. */
            if (((soc_hurricane4_flex_is_pmq_port(phy_port)) && (qmode))
                 || (soc_hurricane4_flex_is_qgphy_port(phy_port)) ) {
                /* configured thru API:  bcm_port_frame_max_set */

                /* SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, FRM_LENGTHr, phy_port,
                 *           0, &rval64));
                 * soc_reg64_field32_set(unit, FRM_LENGTHr, &rval64, MAXFRf, 12288);
                 * SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, FRM_LENGTHr, phy_port,
                 *           0, rval64));
                 */
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, GPORT_RSV_MASKr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit,GPORT_RSV_MASKr, &rval64, MASKf, 120);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, GPORT_RSV_MASKr, phy_port,
                            0, rval64));
                /* SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, TX_IPG_LENGTHr, phy_port,
                 *             0, &rval64));
                 * soc_reg64_field32_set(unit, TX_IPG_LENGTHr, &rval64, TX_IPG_LENGTHf, 12);
                 * SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, TX_IPG_LENGTHr, phy_port,
                 *             0, rval64));
                 */
            } else if (soc_hurricane4_flex_is_eagle_port(phy_port) ||
                       (soc_hurricane4_flex_is_pmq_port(phy_port) &&
                        (qmode == 0))) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Bringing Eagle mac rx port %0d up\n"), phy_port));
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, XLMAC_RX_CTRLr,
                            phy_port, 0, &rval64));
                soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rval64, RX_ANY_STARTf, 0);
                soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rval64, STRIP_CRCf, 0);
                soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rval64, STRICT_PREAMBLEf,
                        strict_preamble);
                soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rval64, RUNT_THRESHOLDf,
                        64);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, XLMAC_RX_CTRLr,
                            phy_port, 0, rval64));
            } else if (soc_hurricane4_flex_is_falcon_port(phy_port)) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Bringing Falcon mac rx port %0d up\n"), phy_port));
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_RX_CTRLr,
                            phy_port, 0, &rval64));
                soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &rval64, RX_ANY_STARTf, 0);
                soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &rval64, STRIP_CRCf, 0);
                soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &rval64, STRICT_PREAMBLEf,
                        strict_preamble);
                soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &rval64, RUNT_THRESHOLDf,
                        64);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_RX_CTRLr,
                            phy_port, 0, rval64));
            }
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

            if (soc_hurricane4_flex_is_pmq_port(phy_port)) {
                inst = ((phy_port - HR4_PMQ_START) / HR4_NUM_PMQ_PORTS | SOC_REG_ADDR_INSTANCE_MASK);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst,
                            0, &rval64));
                qmode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64,
                        QMODEf);
            } else {
                qmode = 0;
            }
            if (soc_hurricane4_flex_is_falcon_port(phy_port) == 1) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Setting Falcon mac cl port %0d up\n"), phy_port));
                SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, CLPORT_CONFIGr,
                            phy_port, 0, &rval32));
                soc_reg_field_set(unit, CLPORT_CONFIGr, &rval32, HIGIG2_MODEf,
                        higig2_mode);
                soc_reg_field_set(unit, CLPORT_CONFIGr, &rval32, HIGIG_MODEf,
                        higig_mode);
                SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, CLPORT_CONFIGr,
                            phy_port, 0, rval32));
            } else if (soc_hurricane4_flex_is_eagle_port(phy_port) ||
                       (soc_hurricane4_flex_is_pmq_port(phy_port) &&
                        (qmode == 0))) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Setting Eagle mac xl port %0d up\n"), phy_port));
                SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, XLPORT_CONFIGr,
                            phy_port, 0, &rval32));
                soc_reg_field_set(unit, XLPORT_CONFIGr, &rval32, HIGIG2_MODEf,
                        higig2_mode);
                soc_reg_field_set(unit, XLPORT_CONFIGr, &rval32, HIGIG_MODEf,
                        higig_mode);
                SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, XLPORT_CONFIGr,
                            phy_port, 0, rval32));
            }
        }
    }

    /*CLPORT Enable: */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            subp = soc_hurricane4_get_subp_from_phy_pnum(phy_port);

            if (soc_hurricane4_flex_is_pmq_port(phy_port)) {
                inst = ((phy_port - HR4_PMQ_START) / HR4_NUM_PMQ_PORTS | SOC_REG_ADDR_INSTANCE_MASK);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst,
                            0, &rval64));
                qmode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64,
                        QMODEf);
            } else {
                qmode = 0;
            }

            if (soc_hurricane4_flex_is_falcon_port(phy_port) == 1) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Setting Falcon enable port %0d up\n"), phy_port));
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
            } else if (soc_hurricane4_flex_is_eagle_port(phy_port) ||
                       (soc_hurricane4_flex_is_pmq_port(phy_port) &&
                        (qmode == 0))) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Setting Eagle enable port %0d up\n"), phy_port));
                SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, XLPORT_ENABLE_REGr,
                            phy_port, 0, &rval32));
                if (subp == 0) {
                    soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval32, PORT0f, 1);
                } else if (subp==1) {
                    soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval32, PORT1f, 1);
                } else if (subp==2) {
                    soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval32, PORT2f, 1);
                } else {
                    soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval32, PORT3f, 1);
                }
                SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, XLPORT_ENABLE_REGr,
                            phy_port, 0, rval32));
            }
        }
    }
    sal_usleep(100 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);

    /*CLPORT mode: */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            subp = soc_hurricane4_get_subp_from_phy_pnum(phy_port);
            /* clport = ((phy_port - 1)>>2); */
            clport = soc_hurricane4_get_pm_from_phy_pnum(phy_port);
            soc_hr4_tdm_get_port_ratio(unit, port_schedule_state, clport,
                    &mode,
                    0);
            if (port_schedule_state->out_port_map.log_port_speed[port]>=
                    100000) {
                speed_100g = 1;
            } else {
                speed_100g = 0;
            }

            if (soc_hurricane4_flex_is_pmq_port(phy_port)) {
                inst = ((phy_port - HR4_PMQ_START) / HR4_NUM_PMQ_PORTS | SOC_REG_ADDR_INSTANCE_MASK);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst,
                            0, &rval64));
                qmode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64,
                        QMODEf);
            } else {
                qmode = 0;
            }

            if (soc_hurricane4_flex_is_falcon_port(phy_port) == 1) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Setting mode port %0d %0d %d\n"), phy_port, mode, clport_mode_values[mode]));
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
            } else if (soc_hurricane4_flex_is_eagle_port(phy_port) ||
                       (soc_hurricane4_flex_is_pmq_port(phy_port) &&
                        (qmode == 0))) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Setting mode port %0d %0d %d\n"), phy_port, mode, clport_mode_values[mode]));
                SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, XLPORT_MODE_REGr,
                            phy_port, 0, &rval32));
                /*  soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval32,
                 *                   SINGLE_PORT_MODE_SPEED_100Gf,
                 *                   speed_100g);
                 */
                soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval32,
                        XPORT0_CORE_PORT_MODEf,
                        clport_mode_values[mode]);
                soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval32,
                        XPORT0_PHY_PORT_MODEf,
                        clport_mode_values[mode]);
                SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, XLPORT_MODE_REGr,
                            phy_port, 0, rval32));
            }
        }
    }
    sal_usleep(200 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);

    /* Release soft reset */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            if (soc_hurricane4_flex_is_pmq_port(phy_port)) {
                inst = ((phy_port - HR4_PMQ_START) / HR4_NUM_PMQ_PORTS | SOC_REG_ADDR_INSTANCE_MASK);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst,
                            0, &rval64));
                qmode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64,
                        QMODEf);
            } else {
                qmode = 0;

            }
            /* GPORT and PMQ seems to have same port up/down seq. */
            if (((soc_hurricane4_flex_is_pmq_port(phy_port)) && (qmode)) ||
                (soc_hurricane4_flex_is_qgphy_port(phy_port))) {
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, COMMAND_CONFIGr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, COMMAND_CONFIGr, &rval64, SW_RESETf, 0);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, COMMAND_CONFIGr, phy_port,
                            0, rval64));

            } else if (soc_hurricane4_flex_is_eagle_port(phy_port) ||
                       (soc_hurricane4_flex_is_pmq_port(phy_port) &&
                        (qmode == 0))) {
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, XLMAC_CTRLr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval64, SOFT_RESETf, 0);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, XLMAC_CTRLr, phy_port,
                            0, rval64));
            } else if (soc_hurricane4_flex_is_falcon_port(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval64, SOFT_RESETf, 0);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, phy_port,
                            0, rval64));
                /*
                 *    Special handling for new mac version. Internally MAC loopback
                 *    looks for rising edge on MAC loopback configuration to enter
                 *    loopback state.
                 *    Do only if loopback is enabled on the port.
 */
                if (soc_reg64_field32_get(unit, CLMAC_CTRLr, rval64, LOCAL_LPBKf)) {
                    soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval64, LOCAL_LPBKf, 0);
                    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, phy_port,
                                0, rval64));
                    /* Wait 10usec as suggested by MAC designer */
                    sal_udelay(10);
                    soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval64, LOCAL_LPBKf, 1);
                    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, phy_port,
                                0, rval64));
                }
            }
        }
    }

    sal_usleep(100 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
    /* Enable MAC RX_EN & TX_EN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            if (soc_hurricane4_flex_is_pmq_port(phy_port)) {
                inst = ((phy_port - HR4_PMQ_START) / HR4_NUM_PMQ_PORTS | SOC_REG_ADDR_INSTANCE_MASK);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst,
                            0, &rval64));
                qmode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64,
                        QMODEf);
            } else {
                qmode = 0;

            }
            /* GPORT and PMQ seems to have same port up/down seq. */
            if (((soc_hurricane4_flex_is_pmq_port(phy_port)) && (qmode)) ||
                (soc_hurricane4_flex_is_qgphy_port(phy_port))) {
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, COMMAND_CONFIGr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, COMMAND_CONFIGr, &rval64, RX_ENAf, 1);
                soc_reg64_field32_set(unit, COMMAND_CONFIGr, &rval64, TX_ENAf, 1);
                /*soc_reg64_field32_set(unit, COMMAND_CONFIGr, &rval64, IGNORE_TX_PAUSEf, 0);
                soc_reg64_field32_set(unit, COMMAND_CONFIGr, &rval64, PAUSE_IGNOREf, 0);*/
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, COMMAND_CONFIGr, phy_port,
                            0, rval64));

                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, FLUSH_CONTROLr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, FLUSH_CONTROLr, &rval64, FLUSHf, 0);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, FLUSH_CONTROLr, phy_port,
                            0, rval64));

                if (soc_hurricane4_flex_is_qgphy_port(phy_port) &&
                    soc_feature(unit, soc_feature_egphy_port_leverage)) {
                    lport = port_schedule_state->resource[i].logical_port;
                    if (EXT_PHY_SW_STATE(unit, lport) ||
                        INT_PHY_SW_STATE(unit, lport)) {
                        SOC_IF_ERROR_RETURN
                            (soc_phyctrl_notify(unit, lport,
                                                phyEventResume,
                                                PHY_STOP_DRAIN));
                    }
                }
            } else if (soc_hurricane4_flex_is_eagle_port(phy_port) ||
                       (soc_hurricane4_flex_is_pmq_port(phy_port) &&
                        (qmode == 0))) {
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, XLMAC_TX_CTRLr,
                            phy_port, 0, &rval64));
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval64, DISCARDf, 0);
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval64, EP_DISCARDf, 0);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, XLMAC_TX_CTRLr,
                            phy_port, 0, rval64));

                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, XLMAC_CTRLr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, XLMAC_CTRLr, &rval64, RX_ENf, 1);
                soc_reg64_field32_set(unit, XLMAC_CTRLr, &rval64, TX_ENf, 1);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, XLMAC_CTRLr, phy_port,
                            0, rval64));
            } else if (soc_hurricane4_flex_is_falcon_port(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_TX_CTRLr,
                            phy_port, 0, &rval64));
                soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval64, DISCARDf, 0);
                soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval64, EP_DISCARDf, 0);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_TX_CTRLr,
                            phy_port, 0, rval64));

                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval64, RX_ENf, 1);
                soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval64, TX_ENf, 1);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, phy_port,
                            0, rval64));
            }
        }
    }

    /* CLMAC MODE */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port != -1) {
            port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];

            switch(port_schedule_state->out_port_map.log_port_speed[port]) {
                case 10 	: speed_mode = 0; break;
                case 100 	: speed_mode = 1; break;
                case 1000 : speed_mode = 2; break;
                case 2500 : speed_mode = 3; break;
                default 	: speed_mode = 4;
            }

            switch(port_schedule_state->resource[i].encap) {
                case _SHR_PORT_ENCAP_IEEE 	: hdr_mode = 0; break;
                case _SHR_PORT_ENCAP_HIGIG	: hdr_mode = 1; break;
                case _SHR_PORT_ENCAP_HIGIG2	: hdr_mode = 2; break;
                default						: hdr_mode = 0;
            }

            if (soc_hurricane4_flex_is_pmq_port(phy_port)) {
                inst = ((phy_port - HR4_PMQ_START) / HR4_NUM_PMQ_PORTS | SOC_REG_ADDR_INSTANCE_MASK);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst,
                            0, &rval64));
                qmode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64,
                        QMODEf);
            } else {
                qmode = 0;

            }
            /* GPORT and PMQ seems to have same port up/down seq. */
            if (((soc_hurricane4_flex_is_pmq_port(phy_port)) && (qmode)) ||
                (soc_hurricane4_flex_is_qgphy_port(phy_port))) {
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, COMMAND_CONFIGr, phy_port,
                            0, &rval64));
                soc_reg64_field32_set(unit, COMMAND_CONFIGr, &rval64, ETH_SPEEDf, speed_mode);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, COMMAND_CONFIGr, phy_port,
                            0, rval64));
                if (soc_hurricane4_flex_is_qgphy_port(phy_port) &&
                    soc_feature(unit, soc_feature_egphy_port_leverage)) {
                    lport = port_schedule_state->resource[i].logical_port;
                    if (EXT_PHY_SW_STATE(unit, lport) ||
                        INT_PHY_SW_STATE(unit, lport)) {
                        SOC_IF_ERROR_RETURN
                            (soc_phyctrl_notify(unit, lport,
                                                phyEventResume,
                                                PHY_STOP_MAC_DIS));
                    }
                }
            } else if (soc_hurricane4_flex_is_eagle_port(phy_port) ||
                       (soc_hurricane4_flex_is_pmq_port(phy_port) &&
                        (qmode == 0))) {
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, XLMAC_MODEr,
                            phy_port, 0, &rval64));
                soc_reg64_field32_set(unit, XLMAC_MODEr, &rval64, HDR_MODEf, hdr_mode);
                soc_reg64_field32_set(unit, XLMAC_MODEr, &rval64, SPEED_MODEf, speed_mode);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, XLMAC_MODEr,
                            phy_port, 0, rval64));
            } else if (soc_hurricane4_flex_is_falcon_port(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_MODEr,
                            phy_port, 0, &rval64));
                soc_reg64_field32_set(unit, CLMAC_MODEr, &rval64, HDR_MODEf, hdr_mode);
                soc_reg64_field32_set(unit, CLMAC_MODEr, &rval64, SPEED_MODEf, speed_mode);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_MODEr,
                            phy_port, 0, rval64));
            }
        }
    }

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "END soc_hurricane4_flex_mac_port_up\n")));
    return SOC_E_NONE;
}

#endif
