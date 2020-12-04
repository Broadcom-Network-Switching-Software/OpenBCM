/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  trident3_mac_flexport.c
*/


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#include <soc/trident3.h>
#include <soc/trident3_tdm.h>
#include <soc/flexport/trident3/trident3_flexport.h>


/*** START SDK API COMMON CODE ***/


/*! @file trident3_mac_flexport.c
 *  @brief MAC flex for Trident3.
 *  Details are shown below.
 */

int
soc_trident3_flex_mac_rx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint64 rval64;
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
			LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Bringing rx port %0d down\n"), phy_port));

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
    return SOC_E_NONE;
}

int
soc_trident3_flex_mac_tx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    uint64 rval64;
    int phy_port;

    /* Disable MAC TX_EN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            phy_port =
                port_schedule_state->in_port_map.port_l2p_mapping[
                    port_schedule_state
                    ->resource[i]
                    .logical_port];

			LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Bringing tx port %0d down\n"), phy_port));
            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, phy_port,
                                                    0, &rval64));
            soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval64, TX_ENf, 0);
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

			LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Applying soft reset to port %0d \n"), phy_port));
            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_CTRLr, phy_port,
                                                    0, &rval64));
            soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval64, SOFT_RESETf, 1);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_CTRLr, phy_port,
                                                    0, rval64));
        }
    }
    return SOC_E_NONE;
}

int
soc_trident3_flex_mac_port_up(int                        unit,
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
    int strict_preamble;
    int higig_mode;
	int speed_mode;
	int hdr_mode;
    static const int clport_mode_values[SOC_TD3_PORT_RATIO_COUNT] = {
        4, 3, 3, 3, 2, 2, 1, 1, 0
    };


	LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "START soc_trident3_flex_mac_port_up\n")));
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
			LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Bringing mac rx port %0d up\n"), phy_port));
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
			LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Setting mac cl port %0d up\n"), phy_port));
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
            subp = soc_trident3_get_subp_from_phy_pnum(phy_port);

			LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Setting enable port %0d up\n"), phy_port));
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
            subp = soc_trident3_get_subp_from_phy_pnum(phy_port);
            clport = ((phy_port-1)>>2);
            soc_td3_tdm_get_port_ratio(unit, port_schedule_state, clport,
                                         &mode,
                                         0);
            if (port_schedule_state->out_port_map.log_port_speed[port]>=
                100000) {
                speed_100g = 1;
            } else {
                speed_100g = 0;
            }

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
            soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval64, SOFT_RESETf, 0);
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

            SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, CLMAC_MODEr,
                                                    phy_port, 0, &rval64));
            soc_reg64_field32_set(unit, CLMAC_MODEr, &rval64, HDR_MODEf, hdr_mode);
            soc_reg64_field32_set(unit, CLMAC_MODEr, &rval64, SPEED_MODEf, speed_mode);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLMAC_MODEr,
                                                    phy_port, 0, rval64));
        }
    }

	LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "END soc_trident3_flex_mac_port_up\n")));
    return SOC_E_NONE;
}


#endif /* BCM_TRIDENT3_SUPPORT */
