/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phymod_ctrl.c
 * Purpose:     Interface functions for PHYMOD
 */

#ifdef PHYMOD_SUPPORT

#include <soc/phy/phymod_port_control.h>
#include <soc/error.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PHYMOD

#define PM_MAX_LANES (8)
#define LANE_MAP_NOF_LANES (4)
#define PATTERN_MAX_SIZE (8)

typedef int (*control_set_handler_f)(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2);
typedef int (*control_get_handler_f)(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value);

int soc_phymod_phy_intf_from_port_intf(int unit, int speed, soc_port_if_t interface,
                                       phymod_interface_t *phymod_interface)
{
    SOC_INIT_FUNC_DEFS;

    switch(interface) {
        case SOC_PORT_IF_SR:
            (*phymod_interface) = phymodInterfaceSR;
            break ;
        case SOC_PORT_IF_SR2:
            (*phymod_interface) = phymodInterfaceSR2;
            break ;
        case SOC_PORT_IF_SR4:
            (*phymod_interface) = phymodInterfaceSR4;
            break ;
        case SOC_PORT_IF_SR10:
            (*phymod_interface) = phymodInterfaceSR10;
            break ;
        case SOC_PORT_IF_DNX_XAUI:
            (*phymod_interface) = phymodInterfaceXAUI;
             break;
        case SOC_PORT_IF_KR:
            (*phymod_interface) = phymodInterfaceKR;
            break ;
        case SOC_PORT_IF_KR2:
            (*phymod_interface) = phymodInterfaceKR2;
            break ;
        case SOC_PORT_IF_KR4:
            (*phymod_interface) = phymodInterfaceKR4;
            break ;
        case SOC_PORT_IF_KR10:
            (*phymod_interface) = phymodInterfaceKR10;
            break ;
        case SOC_PORT_IF_LR10:
            (*phymod_interface) = phymodInterfaceLR10;
            break ;
        case SOC_PORT_IF_CAUI:
            (*phymod_interface) = phymodInterfaceCAUI;
            break;
        case SOC_PORT_IF_XFI2:
            (*phymod_interface) = phymodInterfaceSR2;
            break;
        case SOC_PORT_IF_XFI:
            (*phymod_interface) = phymodInterfaceXFI;
            break;
        case SOC_PORT_IF_SFI:
            (*phymod_interface) = phymodInterfaceSFI;
            break;
        case SOC_PORT_IF_XGMII:
            (*phymod_interface) = phymodInterfaceXGMII;
             break;
        case SOC_PORT_IF_SGMII:
            (*phymod_interface) = phymodInterfaceSGMII;
             break;
        case SOC_PORT_IF_GMII:
            (*phymod_interface) = phymodInterface1000X;
             break;
        case SOC_PORT_IF_RXAUI:
            (*phymod_interface) = phymodInterfaceRXAUI;
             break;
        case SOC_PORT_IF_XLAUI:
            (*phymod_interface) = phymodInterfaceXLAUI;
            break;
        case SOC_PORT_IF_XLAUI2:
            (*phymod_interface) = phymodInterfaceXLAUI2;
            break;
        case SOC_PORT_IF_CR:
            (*phymod_interface) = phymodInterfaceCR;
             break;
        case SOC_PORT_IF_CR2:
            (*phymod_interface) = phymodInterfaceCR2;
             break;
        case SOC_PORT_IF_CR4:
            (*phymod_interface) = phymodInterfaceCR4;
            break;
        case SOC_PORT_IF_CR10:
            (*phymod_interface) = phymodInterfaceCR10;
             break;
        case SOC_PORT_IF_QSGMII:
            (*phymod_interface) = phymodInterfaceQSGMII;
             break;
        case SOC_PORT_IF_ZR:
            (*phymod_interface) = phymodInterfaceZR;
             break;
        case SOC_PORT_IF_LRM:
            (*phymod_interface) = phymodInterfaceLRM;
             break;
        case SOC_PORT_IF_LR:
            (*phymod_interface) = phymodInterfaceLR;
             break;
        case SOC_PORT_IF_LR2:
            (*phymod_interface) = phymodInterfaceLR2;
             break;
        case SOC_PORT_IF_LR4:
            (*phymod_interface) = phymodInterfaceLR4;
             break;
        case SOC_PORT_IF_ILKN:
            (*phymod_interface) = phymodInterfaceBypass;
             break;
        case SOC_PORT_IF_KX:
            (*phymod_interface) = phymodInterfaceKX;
             break;
        case SOC_PORT_IF_ER:
            (*phymod_interface) = phymodInterfaceER;
            break;
        case SOC_PORT_IF_ER2:
            (*phymod_interface) = phymodInterfaceER2;
            break;
        case SOC_PORT_IF_ER4:
            (*phymod_interface) = phymodInterfaceER4;
            break;
        case SOC_PORT_IF_XLPPI:
            (*phymod_interface) = phymodInterfaceXLPPI;
            break;
        case SOC_PORT_IF_VSR:
            (*phymod_interface) = phymodInterfaceVSR;
            break;
        case SOC_PORT_IF_CX:
            (*phymod_interface) = phymodInterfaceCX;
            break;
        case SOC_PORT_IF_CAUI_C2C: 
            (*phymod_interface) = phymodInterfaceCAUI4_C2C; 
            break;
        case SOC_PORT_IF_CAUI_C2M: 
            (*phymod_interface) = phymodInterfaceCAUI4_C2M; 
            break;
        case SOC_PORT_IF_CAUI4:
            (*phymod_interface) = phymodInterfaceCAUI4;
            break;
        case SOC_PORT_IF_COUNT:
            (*phymod_interface) = phymodInterfaceCount;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Interface %d not supported in portmod_intf_to"), interface));
    }

exit:
        SOC_FUNC_RETURN;

}

int soc_phymod_phy_intf_to_port_intf (int unit,
                                   phymod_interface_t phymod_interface,
                                   soc_port_if_t *interface)
{
    SOC_INIT_FUNC_DEFS;

    switch(phymod_interface) {
        case phymodInterfaceBypass:
            (*interface) = SOC_PORT_IF_ILKN;
            break;
        case phymodInterfaceSR:
            (*interface) = SOC_PORT_IF_SR;
            break;
        case phymodInterfaceSR2:
            (*interface) = SOC_PORT_IF_SR2;
            break;
        case phymodInterfaceSR4:
            (*interface) = SOC_PORT_IF_SR4;
            break;
        case phymodInterfaceSR10:
            (*interface) = SOC_PORT_IF_SR10;
            break;
        case phymodInterfaceLR10:
            (*interface) = SOC_PORT_IF_LR10;
            break;
        case phymodInterfaceKX:
            (*interface) = SOC_PORT_IF_KX;
            break;
        case phymodInterfaceKX4:
        case phymodInterfaceXAUI:
            (*interface) = SOC_PORT_IF_DNX_XAUI;
            break;
        case phymodInterfaceKR:
            (*interface) = SOC_PORT_IF_KR;
            break;
        case phymodInterfaceKR2:
            (*interface) = SOC_PORT_IF_KR2;
            break;
        case phymodInterfaceKR4:
            (*interface) = SOC_PORT_IF_KR4;
            break;
        case phymodInterfaceKR10:
            (*interface) = SOC_PORT_IF_KR10;
            break;
        case phymodInterfaceCR:
            (*interface) = SOC_PORT_IF_CR;
            break;
        case phymodInterfaceCR2:
            (*interface) = SOC_PORT_IF_CR2;
            break;
        case phymodInterfaceCR4:
            (*interface) = SOC_PORT_IF_CR4;
            break;
        case phymodInterfaceCR10:
            (*interface) = SOC_PORT_IF_CR10;
            break;
        case phymodInterfaceXFI:
            (*interface) = SOC_PORT_IF_XFI;
            break;
        case phymodInterfaceSFI:
            (*interface) = SOC_PORT_IF_SFI;
            break;
        case phymodInterfaceXGMII:
            (*interface) = SOC_PORT_IF_XGMII;
            break;
        case phymodInterfaceSGMII:
            (*interface) = SOC_PORT_IF_SGMII;
             break;
        case phymodInterface1000X:
            (*interface) = SOC_PORT_IF_GMII;
             break;
        case phymodInterfaceCX2:
            (*interface) = SOC_PORT_IF_RXAUI;
             break;
        case phymodInterfaceX2:
            (*interface) = SOC_PORT_IF_RXAUI;
             break;
        case phymodInterfaceRXAUI:
            (*interface) = SOC_PORT_IF_RXAUI;
             break;
        case phymodInterfaceXLAUI:
            (*interface) = SOC_PORT_IF_XLAUI;
            break;
        case phymodInterfaceXLAUI2:
            (*interface) = SOC_PORT_IF_XLAUI2;
            break;
        case phymodInterfaceCAUI:
            (*interface) = SOC_PORT_IF_CAUI;
            break;
        case phymodInterfaceCAUI4:
            (*interface) = SOC_PORT_IF_CAUI4;
            break;
        case phymodInterfaceQSGMII:
            (*interface) = SOC_PORT_IF_QSGMII;
             break;
        case phymodInterfaceCX4:
            (*interface) = SOC_PORT_IF_CR4;
             break;
        case phymodInterfaceZR:
            (*interface) = SOC_PORT_IF_ZR;
             break;
        case phymodInterfaceLR:
            (*interface) = SOC_PORT_IF_LR;
             break;
        case phymodInterfaceLR2:
            (*interface) = SOC_PORT_IF_LR2;
             break;
        case phymodInterfaceLR4:
            (*interface) = SOC_PORT_IF_LR4;
             break;
        case phymodInterfaceLRM:
            (*interface) = SOC_PORT_IF_LRM;
             break;
        case phymodInterfaceER:
            (*interface) = SOC_PORT_IF_ER;
            break;
        case phymodInterfaceER2:
            (*interface) = SOC_PORT_IF_ER2;
            break;
        case phymodInterfaceER4:
            (*interface) = SOC_PORT_IF_ER4;
            break;
        case phymodInterfaceXLPPI:
            (*interface) = SOC_PORT_IF_XLPPI;
             break;
        case phymodInterfaceVSR:
            (*interface) = SOC_PORT_IF_VSR;
             break;
        case phymodInterfaceCX:
            (*interface) = SOC_PORT_IF_CX;
             break;
        case phymodInterfaceCAUI4_C2C:
            (*interface) = SOC_PORT_IF_CAUI_C2C;
            break;
        case phymodInterfaceCAUI4_C2M:
            (*interface) = SOC_PORT_IF_CAUI_C2M; 
            break;
        /*
              case phymodInterfaceX2:
                    (*interface) = SOC_PORT_IF_RXAUI;
                    break;
             */
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("Phymod Interface %d not supported in soc_phymod_phy_intf_to_port_intf"),
                            phymod_interface));
    }

exit:
    SOC_FUNC_RETURN;
}

int soc_phymod_media_type_from_port_intf(int unit, soc_port_if_t interface,
                                       phymod_phy_inf_config_t *phy_interface_config)
{
    SOC_INIT_FUNC_DEFS;

    PHYMOD_INTF_MODES_FIBER_CLR(phy_interface_config);
    PHYMOD_INTF_MODES_COPPER_CLR(phy_interface_config);
    PHYMOD_INTF_MODES_BACKPLANE_CLR(phy_interface_config);

    switch(interface) {
        case SOC_PORT_IF_SFI:
        case SOC_PORT_IF_SR:
        case SOC_PORT_IF_SR2:
        case SOC_PORT_IF_SR4:
        case SOC_PORT_IF_SR10:
        case SOC_PORT_IF_LR:
        case SOC_PORT_IF_LR2:
        case SOC_PORT_IF_LR4:
        case SOC_PORT_IF_GMII:
        case SOC_PORT_IF_LRM:
        case SOC_PORT_IF_ER:
        case SOC_PORT_IF_ER2:
        case SOC_PORT_IF_ER4:
        case SOC_PORT_IF_XFI2:
            PHYMOD_INTF_MODES_FIBER_SET(phy_interface_config);
            break ;
        case SOC_PORT_IF_KR:
        case SOC_PORT_IF_KR2:
        case SOC_PORT_IF_KR4:
        case SOC_PORT_IF_KR10:
        case SOC_PORT_IF_CAUI:
        case SOC_PORT_IF_CAUI4:
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_SGMII:
        case SOC_PORT_IF_QSGMII:
        case SOC_PORT_IF_DNX_XAUI:
        case SOC_PORT_IF_RXAUI:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_XLAUI2:
        case SOC_PORT_IF_XGMII:
            PHYMOD_INTF_MODES_BACKPLANE_SET(phy_interface_config);
            break;
        case SOC_PORT_IF_CR:
        case SOC_PORT_IF_CR2:
        case SOC_PORT_IF_CR4:
        case SOC_PORT_IF_CR10:
            PHYMOD_INTF_MODES_COPPER_SET(phy_interface_config);
             break;
        default:
            PHYMOD_INTF_MODES_BACKPLANE_SET(phy_interface_config);
    }

    SOC_FUNC_RETURN;
}

int soc_phymod_intf_mode_from_phymod_intf(int unit, phymod_interface_t phymod_interface,
                                       phymod_phy_inf_config_t *phy_interface_config)
{
    SOC_INIT_FUNC_DEFS;

    PHYMOD_INTF_MODES_FIBER_CLR(phy_interface_config);
    PHYMOD_INTF_MODES_COPPER_CLR(phy_interface_config);
    PHYMOD_INTF_MODES_BACKPLANE_CLR(phy_interface_config);

    switch(phymod_interface) {
        case phymodInterfaceSFI:
        case phymodInterfaceSR:
        case phymodInterfaceSR2:
        case phymodInterfaceSR4:
        case phymodInterfaceSR10:
        case phymodInterfaceLR:
        case phymodInterfaceLR2:
        case phymodInterfaceLR4:
        case phymodInterface1000X:
        case phymodInterfaceLRM:
        case phymodInterfaceER:
        case phymodInterfaceER2:
        case phymodInterfaceER4:
            PHYMOD_INTF_MODES_FIBER_SET(phy_interface_config);
            break ;
        case phymodInterfaceKR:
        case phymodInterfaceKR2:
        case phymodInterfaceKR4:
        case phymodInterfaceKR10:
        case phymodInterfaceCAUI:
        case phymodInterfaceCAUI4:
        case phymodInterfaceXFI:
        case phymodInterfaceSGMII:
        case phymodInterfaceQSGMII:
        case phymodInterfaceXAUI:
        case phymodInterfaceRXAUI:
        case phymodInterfaceXLAUI:
        case phymodInterfaceXLAUI2:
        case phymodInterfaceXGMII:
            PHYMOD_INTF_MODES_BACKPLANE_SET(phy_interface_config);
            break;
        case phymodInterfaceCR:
        case phymodInterfaceCR2:
        case phymodInterfaceCR4:
        case phymodInterfaceCR10:
            PHYMOD_INTF_MODES_COPPER_SET(phy_interface_config);
             break;
        default:
            PHYMOD_INTF_MODES_BACKPLANE_SET(phy_interface_config);
    }

    SOC_FUNC_RETURN;
}

int
soc_prbs_poly_to_phymod(uint32 sdk_poly, phymod_prbs_poly_t *phymod_poly){
    switch(sdk_poly){
    case SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1:
        *phymod_poly = phymodPrbsPoly7;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1:
        *phymod_poly = phymodPrbsPoly9;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1:
        *phymod_poly = phymodPrbsPoly11;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1:
        *phymod_poly = phymodPrbsPoly15;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1:
        *phymod_poly = phymodPrbsPoly23;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1:
        *phymod_poly = phymodPrbsPoly31;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1:
        *phymod_poly = phymodPrbsPoly58;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X49_X40_1:
        *phymod_poly = phymodPrbsPoly49;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X20_X3_1:
        *phymod_poly = phymodPrbsPoly20;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X13_X12_X2_1:
        *phymod_poly = phymodPrbsPoly13;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X10_X7_1:
        *phymod_poly = phymodPrbsPoly10;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_PAM4_13Q:
        *phymod_poly = phymodPrbsPolyQ13;
        break;
    default:
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}


int
phymod_prbs_poly_to_soc(phymod_prbs_poly_t phymod_poly, uint32 *sdk_poly){
    switch(phymod_poly){
    case phymodPrbsPoly7:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1;
        break;
    case phymodPrbsPoly9:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1;
        break;
    case phymodPrbsPoly11:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1;
        break;
    case phymodPrbsPoly15:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1;
        break;
    case phymodPrbsPoly23:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1;
        break;
    case phymodPrbsPoly31:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1;
        break;
    case phymodPrbsPoly58:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1;
        break;
    case phymodPrbsPoly49:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X49_X40_1;
        break;
    case phymodPrbsPoly20:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X20_X3_1;
        break;
    case phymodPrbsPoly13:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X13_X12_X2_1;
        break;
    case phymodPrbsPoly10:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_X10_X7_1;
        break;
    case phymodPrbsPolyQ13:
        *sdk_poly = SOC_PHY_PRBS_POLYNOMIAL_PAM4_13Q;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

/****************************************************************************** 
  Set controls handlers
 */
STATIC int
control_handler_preemphasis_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
       phymod_tx.pre  = value1 & 0xff;
       phymod_tx.main = (value1 >> 8) & 0xff;
       phymod_tx.post = (value1 >> 16) & 0xff;  
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.pre = phymod_tx_def.pre;

    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));

    return SOC_E_NONE;
}

STATIC int
control_handler_tx_fir_mode_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));

    if (value1 == SOC_PHY_TX_FIR_3TAP_MODE) {
        phymod_tx.tap_mode = phymodTxTapMode3Tap;
    } else if (value1 == SOC_PHY_TX_FIR_6TAP_MODE) {
        phymod_tx.tap_mode = phymodTxTapMode6Tap;
    } else {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}

STATIC int
control_handler_tx_fir_pre2_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.pre2 = value1;
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.pre2 = phymod_tx_def.pre2;

    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_pre_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.pre = value1; 
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.pre = phymod_tx_def.pre;

    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_main_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.main = value1; 
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.main = phymod_tx_def.main;

    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_post_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.post = value1; 
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.post = phymod_tx_def.post;

    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_post2_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.post2 = value1; 
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.post2 = phymod_tx_def.post2;

    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_post3_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.post3 = value1; 
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.post3 = phymod_tx_def.post3;

    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}

STATIC int
control_handler_link_training_tx_fir_post_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    uint8_t tx_post = 0;

    tx_post = (uint8_t) value1;
    SOC_IF_ERROR_RETURN(phymod_phy_training_tx_fir_post_set(phy, tx_post));
    return SOC_E_NONE;
}

STATIC int
control_handler_driver_current_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx, phymod_tx_def;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    if (value1 != 0xFFFFFFFF)
    {
        phymod_tx.amp = value1; 
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &phymod_tx_def));
        phymod_tx.amp = phymod_tx_def.amp;
    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));
    return SOC_E_NONE;
}

STATIC int
control_handler_tx_fir_drivermode_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_t phymod_tx;
    int drivermode;

    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }
    if(PHYMOD_INTF_CONFIG_TX_FIR_DRIVERMODE_ENABLE_GET(phy)) {
        drivermode = 1;
    } else {
        drivermode = 0;
        PHYMOD_INTF_CONFIG_TX_FIR_DRIVERMODE_ENABLE_SET(phy);
    }

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
   
    phymod_tx.drivermode = value1; 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy, &phymod_tx));

    if(drivermode == 0) {
        PHYMOD_INTF_CONFIG_TX_FIR_DRIVERMODE_ENABLE_CLR(phy) ;
    }

    return SOC_E_NONE;
}

STATIC int
control_handler_rx_tap_release(int unit, phymod_phy_access_t *phy, uint32 value, uint32 tap)
{
    phymod_rx_t phymod_rx;

    /* bounds check "tap" */
    if (tap >= COUNTOF(phymod_rx.dfe)) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    phymod_rx.dfe[tap].enable = FALSE;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_adaptation_resume(phy));

    return SOC_E_NONE;
}

STATIC int
control_handler_rx_adaptation_resume(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    if (value1) {
        SOC_IF_ERROR_RETURN(phymod_phy_rx_adaptation_resume(phy));
    }

    return SOC_E_NONE;
}
 
STATIC int 
control_handler_rx_tap_set(int unit, phymod_phy_access_t *phy, uint32 value, uint32 tap)
{
    phymod_rx_t phymod_rx;

    /* bounds check "tap" */
    if (tap >= COUNTOF(phymod_rx.dfe)) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    phymod_rx.dfe[tap].enable = TRUE;
    phymod_rx.dfe[tap].value = value;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(phy, &phymod_rx));

    return SOC_E_NONE;
}


STATIC int 
control_handler_pi_control_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_tx_override_t tx_override;

    phymod_tx_override_t_init(&tx_override);
    tx_override.phase_interpolator.enable = (value1 == 0) ? 0 : 1;
    tx_override.phase_interpolator.value = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_tx_override_set(phy, &tx_override));

    return SOC_E_NONE;
}


STATIC int
control_handler_tx_squelch(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_phy_tx_lane_control_t tx_control;

    if (value1 == 1) {
        tx_control = phymodTxSquelchOn;
    } else {
        tx_control = phymodTxSquelchOff;
    }
    SOC_IF_ERROR_RETURN(phymod_phy_tx_lane_control_set(phy, tx_control));
    return SOC_E_NONE;
}

STATIC int
control_handler_rx_squelch(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_phy_rx_lane_control_t rx_control;

    if (value1 == 1) {
        rx_control = phymodRxSquelchOn;
    } else {
        rx_control = phymodRxSquelchOff;
    }
    SOC_IF_ERROR_RETURN(phymod_phy_rx_lane_control_set(phy, rx_control));
    return SOC_E_NONE;
}



STATIC int
control_handler_rx_peak_filter_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_rx_t phymod_rx;


    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    phymod_rx.peaking_filter.enable = TRUE;
    phymod_rx.peaking_filter.value = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(phy, &phymod_rx));

    return SOC_E_NONE;
}

STATIC int
control_handler_rx_vga_release(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_rx_t phymod_rx;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    phymod_rx.vga.enable = FALSE;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(phy, &phymod_rx));
    return SOC_E_NONE;
}

STATIC int 
control_handler_rx_vga_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_rx_t phymod_rx;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    phymod_rx.vga.enable = TRUE;
    phymod_rx.vga.value = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(phy, &phymod_rx));

    return SOC_E_NONE;
}

STATIC int 
control_handler_pattern_length_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_pattern_t pattern;
    uint32 enable;
    uint32 pattern_arr[PATTERN_MAX_SIZE];

    pattern.pattern = pattern_arr;

    SOC_IF_ERROR_RETURN(phymod_phy_pattern_config_get(phy, &pattern));
    SOC_IF_ERROR_RETURN(phymod_phy_pattern_enable_get(phy, &enable));
    pattern.pattern_len = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_pattern_enable_set(phy, 1, &pattern));
    if (!enable) {
        SOC_IF_ERROR_RETURN(phymod_phy_pattern_enable_set(phy, enable, &pattern)); 
    }
    return SOC_E_NONE;
}

STATIC int 
control_handler_pattern_enable_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_pattern_t pattern;
    uint32 enable;
    uint32 pattern_arr[PATTERN_MAX_SIZE];

    pattern.pattern = pattern_arr;
    enable = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_pattern_config_get(phy, &pattern));
    SOC_IF_ERROR_RETURN(phymod_phy_pattern_enable_set(phy, enable, &pattern));
    return SOC_E_NONE;
}

STATIC int 
control_handler_pattern_config_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_pattern_t pattern;
	uint32 pattern_arr[PATTERN_MAX_SIZE];

    pattern.pattern = pattern_arr;

    SOC_IF_ERROR_RETURN(phymod_phy_pattern_config_get(phy, &pattern));
    pattern.pattern[value2] = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_pattern_config_set(phy, &pattern));
    return SOC_E_NONE;
}


STATIC int 
control_handler_prbs_poly_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    SOC_IF_ERROR_RETURN(soc_prbs_poly_to_phymod(value1, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(phy, flags, &prbs));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_tx_poly_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    SOC_IF_ERROR_RETURN(soc_prbs_poly_to_phymod(value1, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(phy, flags, &prbs));
    return SOC_E_NONE;
}


STATIC int 
control_handler_prbs_rx_poly_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    SOC_IF_ERROR_RETURN(soc_prbs_poly_to_phymod(value1, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(phy, flags, &prbs));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_tx_invert_data_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    prbs.invert = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(phy, flags, &prbs));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_rx_invert_data_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    prbs.invert = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(phy, flags, &prbs));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_tx_enable_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(phy, flags, value1));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_rx_enable_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(phy, flags, value1));
    return SOC_E_NONE;
}


STATIC int
control_handler_rx_reset_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_phy_reset_t reset;

    SOC_IF_ERROR_RETURN(phymod_phy_reset_get(phy, &reset));
    reset.rx = (value1 == 0)?  phymodResetDirectionOut : phymodResetDirectionIn;
    SOC_IF_ERROR_RETURN(phymod_phy_reset_set(phy, &reset));

    return SOC_E_NONE;
}


STATIC int
control_handler_tx_reset_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_phy_reset_t reset;

    SOC_IF_ERROR_RETURN(phymod_phy_reset_get(phy, &reset));
    reset.tx = (value1 == 0)?  phymodResetDirectionOut : phymodResetDirectionIn;
    SOC_IF_ERROR_RETURN(phymod_phy_reset_set(phy, &reset));

    return SOC_E_NONE;
}


STATIC int
control_handler_dsc_dump_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    int nof_phys = 1;

    SOC_IF_ERROR_RETURN(phymod_diag_dsc(phy, nof_phys));

    return SOC_E_NONE;

        

}

STATIC int
control_handler_cl72_enable_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN(phymod_phy_cl72_set(phy, value1));

    return SOC_E_NONE;
}

STATIC int
control_handler_short_chn_mode_enable_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN(phymod_phy_short_chn_mode_enable_set(phy, value1));

    return SOC_E_NONE;
}


STATIC int
control_handler_lane_map_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_core_access_t core;
    phymod_lane_map_t lane_map;
    uint32 idx;

    SOC_IF_ERROR_RETURN(phymod_core_access_t_init(&core));
    core.type = phy->type;
    sal_memcpy(&core.access, &phy->access, sizeof(core.access));
    core.access.lane_mask = 0;

    lane_map.num_of_lanes = LANE_MAP_NOF_LANES;
    for (idx=0; idx < LANE_MAP_NOF_LANES; idx++) {
        lane_map.lane_map_rx[idx] = (value1 >> (idx * 4 /*4 bit per lane*/)) & 0xf;
    }
    for (idx=0; idx < LANE_MAP_NOF_LANES; idx++) {
        lane_map.lane_map_tx[idx] = (value1 >> (16 + idx * 4 /*4 bit per lane*/)) & 0xf;
    }
    SOC_IF_ERROR_RETURN(phymod_core_lane_map_set(&core, &lane_map));
    return SOC_E_NONE;
}


STATIC int 
control_handler_loopback_internal_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(phy, phymodLoopbackGlobal, value1));

    return SOC_E_NONE;
}

STATIC int 
control_handler_loopback_pmd_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(phy, phymodLoopbackGlobalPMD, value1));

    return SOC_E_NONE;
}


STATIC int 
control_handler_loopback_remote_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(phy, phymodLoopbackRemotePMD, value1));

    return SOC_E_NONE;
}

#if 0
STATIC int 
control_handler_loopback_remote_pcs_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(phy, phymodLoopbackRemotePCS, value1));

    return SOC_E_NONE;
}
#endif

STATIC int
control_handler_power_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_phy_power_t      power;

    phymod_phy_power_t_init(&power);
    if (value1) {
        power.tx = phymodPowerOn;
        power.rx = phymodPowerOn;
    } 
    else {
        power.tx = phymodPowerOff;
        power.rx = phymodPowerOff;
    }
     SOC_IF_ERROR_RETURN(phymod_phy_power_set(phy, &power));

    return SOC_E_NONE;
}

STATIC int
control_handler_medium_type_set(int unit, phymod_phy_access_t *phy, uint32 medium_type, uint32 ref_clk)
{
    phymod_phy_inf_config_t config;
    phymod_firmware_lane_config_t fw_config;

#ifdef PHYMOD_FALCON_DPLL_SUPPORT
    if (phy->type == phymodDispatchTypeFalcon_dpll) {
        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(phy, &fw_config));
        switch (medium_type){
            case SOC_PORT_MEDIUM_FIBER:
                fw_config.MediaType = phymodFirmwareMediaTypeOptics;
                break;
            case SOC_PORT_MEDIUM_COPPER:
                fw_config.MediaType = phymodFirmwareMediaTypeCopperCable;
                break;
            case SOC_PORT_MEDIUM_BACKPLANE:
                fw_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                break;
            default:
                fw_config.MediaType = phymodFirmwareMediaTypeOptics;
                break;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_set(phy, fw_config));
        return SOC_E_NONE;
    }
#endif
    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(phy, 0 /* flags */, ref_clk, &config));
    PHYMOD_INTF_MODES_FIBER_CLR(&config); 
    PHYMOD_INTF_MODES_COPPER_CLR(&config); 
    if (medium_type == SOC_PORT_MEDIUM_FIBER) {
        PHYMOD_INTF_MODES_FIBER_SET(&config); 
    } else if (medium_type == SOC_PORT_MEDIUM_COPPER){
        PHYMOD_INTF_MODES_COPPER_SET(&config); 
    }

    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_set(phy, 0 /* flags */,&config));
    return SOC_E_NONE;
}

STATIC int
control_handler_rx_low_freq_filter_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_rx_t phymod_rx;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    phymod_rx.low_freq_peaking_filter.enable = TRUE;
    phymod_rx.low_freq_peaking_filter.value = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(phy, &phymod_rx));

    return SOC_E_NONE;
}

STATIC int
control_handler_rx_high_freq_filter_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_rx_t phymod_rx;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    phymod_rx.high_freq_peaking_filter.enable = TRUE;
    phymod_rx.high_freq_peaking_filter.value = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(phy, &phymod_rx));

    return SOC_E_NONE;
}



STATIC int
control_handler_firmware_mode_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_firmware_lane_config_t fw_config;
    int mode = value1;
    int control = value2;

    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(phy, &fw_config));

    switch (control) {
        case SOC_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE: 
            if (mode) {
                fw_config.ForceBrDfe = 1;
                fw_config.DfeOn = 1;
            } else {
                fw_config.ForceBrDfe = 0;
            }
            break;
        case SOC_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE:
            if (mode) {
                fw_config.LpDfeOn = 1;
                fw_config.DfeOn = 1;
            } else {
                fw_config.LpDfeOn = 0;
            }
            break;
        case SOC_PHY_CONTROL_FIRMWARE_CL72_RESTART_TIMEOUT_ENABLE:
            fw_config.Cl72RestTO = mode ? 1 : 0;
            break;
        case SOC_PHY_CONTROL_FIRMWARE_CL72_AUTO_POLARITY_ENABLE:
            fw_config.Cl72AutoPolEn = mode ? 1 : 0;
            break;
        case SOC_PHY_CONTROL_FIRMWARE_DFE_ENABLE:
            fw_config.DfeOn = mode ? 1 : 0;
            break;
        case SOC_PHY_CONTROL_FIRMWARE_AN_CL72_TX_INIT_SKIP_ON_RESTART:
            fw_config.AnCl72TxInitSkipOnRestart = mode ? 1 : 0;
            break;
        case SOC_PHY_CONTROL_FIRMWARE_MODE:
            /* Select FW parameters according to mode */
            switch (mode) {
                case SOC_PHY_FIRMWARE_LP_DFE_ENABLE: 
                    fw_config.LpDfeOn = 1;
                    fw_config.DfeOn = 1;
                    break;
                case SOC_PHY_FIRMWARE_FORCE_BRDFE:
                    fw_config.ForceBrDfe = 1;
                    fw_config.DfeOn = 1;
                    break;
                case SOC_PHY_FIRMWARE_FORCE_OSDFE:
                case SOC_PHY_FIRMWARE_DFE_ENABLE:
                    fw_config.DfeOn = 1;
                    fw_config.ForceBrDfe = 0;
                    break;
                case SOC_PHY_FIRMWARE_SFP_DAC:
                    fw_config.MediaType = phymodFirmwareMediaTypeCopperCable;
                    break;
                case SOC_PHY_FIRMWARE_XLAUI:
                    fw_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                    break;
                case SOC_PHY_FIRMWARE_SFP_OPT_SR4:
                    fw_config.MediaType = phymodFirmwareMediaTypeOptics;
                    break;
                case SOC_PHY_FIRMWARE_DEFAULT:
                default:
                    fw_config.LaneConfigFromPCS = 0;
                    fw_config.DfeOn = 0;
                    fw_config.LpDfeOn = 0;
                    fw_config.ForceBrDfe = 0;
                    break;
            }
            break;
        case SOC_PHY_CONTROL_FIRMWARE_RX_FORCE_EXTENDED_REACH_ENABLE:
            fw_config.ForceExtenedReach = mode ? 1 : 0;
            break;
        case SOC_PHY_CONTROL_FIRMWARE_RX_FORCE_NORMAL_REACH_ENABLE:
            fw_config.ForceNormalReach = mode ? 1 : 0;
            break;
        default:
            break;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_set(phy, fw_config));

    return SOC_E_NONE;
}


STATIC int
control_handler_rx_seq_toggle_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN(phymod_phy_rx_restart(phy));
    return SOC_E_NONE;
}


STATIC int
control_handler_scrambler_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 ref_clk)
{
    phymod_phy_inf_config_t config;

    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(phy, 0 /* flags */, ref_clk, &config));
    PHYMOD_INTF_MODES_SCR_SET(&config);
    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_set(phy, PHYMOD_INTF_F_DONT_OVERIDE_TX_PARAMS, &config));

    return SOC_E_NONE;
}

STATIC int
control_handler_fec_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 ref_clk)
{
    
    SOC_IF_ERROR_RETURN(phymod_phy_fec_enable_set(phy, value1));
    return SOC_E_NONE;
}

STATIC int 
control_handler_fec_override_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    
    SOC_IF_ERROR_RETURN(phymod_phy_fec_override_set(phy, value1));
    return SOC_E_NONE;
}

STATIC int 
control_handler_tx_polarity_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_polarity_t    polarity;

    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(phy, &polarity));
    polarity.tx_polarity = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_set(phy, &polarity));

    return SOC_E_NONE;
}


STATIC int 
control_handler_rx_polarity_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    phymod_polarity_t    polarity;

    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(phy, &polarity));
    polarity.rx_polarity = value1;
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_set(phy, &polarity));

    return SOC_E_NONE;
}

STATIC int
control_handler_pam4_tx_pattern_set(int unit, phymod_phy_access_t *phy,  uint32 tx_pattern, uint32 param2)
{
    if (tx_pattern == SOC_PHY_PAM4_TX_PATTERN_OFF) {
        SOC_IF_ERROR_RETURN(phymod_phy_PAM4_tx_pattern_enable_set(phy, phymod_PAM4TxPattern_JP03B, 0));
        SOC_IF_ERROR_RETURN(phymod_phy_PAM4_tx_pattern_enable_set(phy, phymod_PAM4TxPattern_Linear, 0));
    } else if (tx_pattern == SOC_PHY_PAM4_TX_PATTERN_JP03B) {
        SOC_IF_ERROR_RETURN(phymod_phy_PAM4_tx_pattern_enable_set(phy, phymod_PAM4TxPattern_JP03B, 1));
    } else if (tx_pattern == SOC_PHY_PAM4_TX_PATTERN_LINEAR) {
        SOC_IF_ERROR_RETURN(phymod_phy_PAM4_tx_pattern_enable_set(phy, phymod_PAM4TxPattern_Linear, 1));
    } else {
      return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

STATIC int
control_handler_pcs_mode_set (int unit, phymod_phy_access_t *phy,  uint32 pcs_mode, uint32 param2)
{
    phymod_phy_inf_config_t config;

    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(phy, 0 /* flags */, param2, &config));
#ifdef PHYMOD_TSCE_SUPPORT
    if (phy->type == phymodDispatchTypeTsce) {
        if (PHYMOD_INTF_MODES_HIGIG_GET(&config) && 
            ((config.data_rate == 40000) || (config.data_rate == 42000))) {
            if (pcs_mode == SOC_PHY_CONTROL_PCS_MODE_BRCM) {
                config.interface_type = phymodInterfaceXGMII;
            } else {
                if (config.interface_type == phymodInterfaceXGMII) {
                    config.interface_type = phymodInterfaceKR4;
                }
            }
        } else {
            return SOC_E_UNAVAIL;
        }
    } else {
        return SOC_E_UNAVAIL;
    }
#else
        return SOC_E_UNAVAIL;
#endif
    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_set(phy, 0, &config));

    return SOC_E_NONE;
}

STATIC int
control_handler_tx_pam4_precoder_enable_set(int unit, phymod_phy_access_t *phy,  uint32 value1, uint32 value2)
{
    uint32_t  cl72_en = 0;

    /* first if link training is enabled */
    SOC_IF_ERROR_RETURN(phymod_phy_cl72_get(phy, &cl72_en));

    if (cl72_en) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: feature not supported with link training on \n")));
        return SOC_E_FAIL;

    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_tx_pam4_precoder_enable_set(phy, (int) value1));
    }
    return SOC_E_NONE;
}

STATIC int
control_handler_lp_tx_precoder_enable_set(int unit, phymod_phy_access_t *phy,  uint32 value1, uint32 value2)
{
    phymod_firmware_lane_config_t fw_config;
    uint32_t  cl72_en = 0;

    /* first if link training is enabled */
    SOC_IF_ERROR_RETURN(phymod_phy_cl72_get(phy, &cl72_en));

    if (cl72_en) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: feature not supported with link training on \n")));
        return SOC_E_FAIL;
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(phy, &fw_config));
        fw_config.LpPrecoderEnabled = value1;
        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_set(phy, fw_config));
    }
    return SOC_E_NONE;
}

STATIC int
control_handler_channel_loss_hint_set(int unit, phymod_phy_access_t *phy,  uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN(phymod_phy_channel_loss_hint_set(phy, value1));
    return SOC_E_NONE;
}


STATIC int
control_handler_flexe_50g_nofec_20k_am_spacing_enable_set(int unit, phymod_phy_access_t *phy,
                                                          uint32 value1, uint32 value2)
{
    SOC_IF_ERROR_RETURN(phymod_phy_50g_nofec_20k_am_spacing_enable_set(phy, (int)value1));
    return SOC_E_NONE;
}



/****************************************************************************** 
 Get controls handlers
 */
STATIC int
control_handler_preemphasis_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    /* the shift is chip dependent.  Need to check later for other chips  */
    *value = phymod_tx.pre  | (phymod_tx.main << 8) | (phymod_tx.post << 16);

    return SOC_E_NONE;
}
STATIC int
control_handler_tx_fir_mode_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));

    if (phymod_tx.tap_mode == phymodTxTapMode3Tap) {
        *value = SOC_PHY_TX_FIR_3TAP_MODE;
    } else if (phymod_tx.tap_mode == phymodTxTapMode6Tap) {
        *value = SOC_PHY_TX_FIR_6TAP_MODE;
    } else {
      return SOC_E_PARAM;
    }


    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_pre2_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.pre2;
    return SOC_E_NONE;
}

STATIC int
control_handler_tx_fir_pre_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.pre;
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_main_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.main;
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_post_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.post;
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_post2_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.post2;
    return SOC_E_NONE;
}


STATIC int
control_handler_tx_fir_post3_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.post3;
    return SOC_E_NONE;
}

STATIC int
control_handler_link_training_tx_fir_post_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32* value)
{
    uint8_t tx_post = 0;

    SOC_IF_ERROR_RETURN(phymod_phy_training_tx_fir_post_get(phy, &tx_post));
    *value = (uint32) tx_post;

    return SOC_E_NONE;
}

STATIC int
control_handler_driver_current_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.amp;
    return SOC_E_NONE;
}

STATIC int
control_handler_tx_fir_drivermode_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_t phymod_tx;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy, &phymod_tx));
    *value = phymod_tx.drivermode;
    return SOC_E_NONE;
}

STATIC int 
control_handler_rx_tap_get(int unit, phymod_phy_access_t *phy, uint32 tap, uint32 *value)
{
    phymod_rx_t phymod_rx;

    /* bounds check "tap" */
    if (tap >= COUNTOF(phymod_rx.dfe)) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    /* use phymod_rx.dfe[tap].enable as the indicator for PAM4 mode for BH*/
    if ((tap == 0) && !phymod_rx.dfe[tap].enable) {
        PHYMOD_DEBUG_ERROR(("ERROR :: DFE1 is not supported on PAM4 mode \n"));
        return SOC_E_PARAM;
    }
    *value = phymod_rx.dfe[tap].value;

    return SOC_E_NONE;
}


STATIC int 
control_handler_pi_control_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_tx_override_t tx_override;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_override_get(phy, &tx_override));
    *value = tx_override.phase_interpolator.value;

    return SOC_E_NONE;
}


STATIC int
control_handler_tx_squelch_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_phy_tx_lane_control_t  tx_control;

    tx_control = phymodTxSquelchOn;
    SOC_IF_ERROR_RETURN(phymod_phy_tx_lane_control_get(phy, &tx_control));
    *value = (tx_control == phymodTxSquelchOn)? 1 : 0;

    return SOC_E_NONE;
}

STATIC int
control_handler_rx_squelch_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_phy_rx_lane_control_t  rx_control;

    rx_control = phymodRxSquelchOn;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_lane_control_get(phy, &rx_control));
    *value = (rx_control == phymodRxSquelchOn)? 1 : 0;

    return SOC_E_NONE;
}

STATIC int
control_handler_power_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_phy_power_t      power;

    phymod_phy_power_t_init(&power);
    SOC_IF_ERROR_RETURN(phymod_phy_power_get(phy, &power));

    if(power.tx == phymodPowerOn && power.rx == phymodPowerOn) {
        *value = 1;
    } else {
        *value = 0;
    }

    return SOC_E_NONE;
}

STATIC int
control_handler_medium_type_get(int unit, phymod_phy_access_t *phy, uint32 ref_clk, uint32 *value)
{
    phymod_phy_inf_config_t config;
    phymod_firmware_lane_config_t fw_config;

#ifdef PHYMOD_FALCON_DPLL_SUPPORT
    if (phy->type == phymodDispatchTypeFalcon_dpll) {
        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(phy, &fw_config));

       if (fw_config.MediaType == phymodFirmwareMediaTypeOptics) {
            *value = SOC_PORT_MEDIUM_FIBER;
        } else if (fw_config.MediaType == phymodFirmwareMediaTypeCopperCable){
            *value = SOC_PORT_MEDIUM_COPPER;
        } else if (fw_config.MediaType == phymodFirmwareMediaTypePcbTraceBackPlane) {
            *value = SOC_PORT_MEDIUM_BACKPLANE;
        } else {
            *value = SOC_PORT_MEDIUM_COUNT;
        }
        return SOC_E_NONE;
    }
#endif

    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(phy, 0 /* flags */, ref_clk, &config));
    if (PHYMOD_INTF_MODES_FIBER_GET(&config)) {
        *value = SOC_PORT_MEDIUM_FIBER; 
    } else if (PHYMOD_INTF_MODES_COPPER_GET(&config)) {
        *value = SOC_PORT_MEDIUM_COPPER; 
    } else {
        *value = SOC_PORT_MEDIUM_NONE;
    }
    return SOC_E_NONE;
}



STATIC int
control_handler_rx_peak_filter_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_rx_t phymod_rx;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    *value = phymod_rx.peaking_filter.value;

    return SOC_E_NONE;
}


STATIC int 
control_handler_rx_vga_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_rx_t phymod_rx;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    *value = phymod_rx.vga.value;

    return SOC_E_NONE;
}

STATIC int 
control_handler_pattern_length_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_pattern_t pattern;
    uint32 pattern_arr[PATTERN_MAX_SIZE];

    pattern.pattern = pattern_arr;

    SOC_IF_ERROR_RETURN(phymod_phy_pattern_config_get(phy, &pattern));
    *value = pattern.pattern_len;
    return SOC_E_NONE;
}

STATIC int 
control_handler_pattern_enable_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_pattern_t pattern;
    uint32 enable;
    uint32 pattern_arr[PATTERN_MAX_SIZE];

    pattern.pattern = pattern_arr;
    SOC_IF_ERROR_RETURN(phymod_phy_pattern_config_get(phy, &pattern));
    SOC_IF_ERROR_RETURN(phymod_phy_pattern_enable_get(phy, &enable));
    *value = enable;
    return SOC_E_NONE;
}

STATIC int 
control_handler_pattern_config_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_pattern_t pattern;
	uint32 pattern_arr[PATTERN_MAX_SIZE];

    pattern.pattern = pattern_arr;

    SOC_IF_ERROR_RETURN(phymod_phy_pattern_config_get(phy, &pattern));
    *value = pattern.pattern[param];
    return SOC_E_NONE;
}

STATIC int 
control_handler_prbs_poly_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    SOC_IF_ERROR_RETURN(phymod_prbs_poly_to_soc(prbs.poly, value));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_tx_poly_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    SOC_IF_ERROR_RETURN(phymod_prbs_poly_to_soc(prbs.poly, value));
    return SOC_E_NONE;
}


STATIC int 
control_handler_prbs_rx_poly_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    SOC_IF_ERROR_RETURN(phymod_prbs_poly_to_soc(prbs.poly, value));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_tx_invert_data_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    *value = prbs.invert;
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_rx_invert_data_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_prbs_t prbs;
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(phy, flags, &prbs));
    *value = prbs.invert;
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_tx_enable_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_get(phy, flags, value));
    return SOC_E_NONE;
}


STATIC int
control_handler_prbs_rx_enable_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    uint32_t flags = 0;

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_get(phy, flags, value));
    return SOC_E_NONE;
}



STATIC int
control_handler_prbs_rx_status_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    uint32_t flags = PHYMOD_PRBS_STATUS_F_CLEAR_ON_READ;
    phymod_prbs_status_t status;

    SOC_IF_ERROR_RETURN(phymod_phy_prbs_status_get(phy, flags, &status));
    if (status.prbs_lock==0){
        *value = -1;
    } else if (status.prbs_lock_loss) {
        *value = -2;
    } else{
        *value = status.error_count;
    }
    return SOC_E_NONE;
}

STATIC int
control_handler_cl72_enable_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    SOC_IF_ERROR_RETURN(phymod_phy_cl72_get(phy, value));

    return SOC_E_NONE;
}

STATIC int
control_handler_short_chn_mode_enable_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    uint32_t status = 0;

    SOC_IF_ERROR_RETURN(phymod_phy_short_chn_mode_enable_get(phy, value, &status));

    return SOC_E_NONE;
}

STATIC int
control_handler_short_chn_mode_status_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    uint32_t enable = 0;

    SOC_IF_ERROR_RETURN(phymod_phy_short_chn_mode_enable_get(phy, &enable, value));

    return SOC_E_NONE;
}

STATIC int
control_handler_rx_signal_detect_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    SOC_IF_ERROR_RETURN(phymod_phy_rx_signal_detect_get(phy, value));

    return SOC_E_NONE;
}



STATIC int
control_handler_cl72_status_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_cl72_status_t status;
    SOC_IF_ERROR_RETURN(phymod_phy_cl72_status_get(phy, &status));
    *value = status.locked;

    return SOC_E_NONE;
}


STATIC int
control_handler_lane_map_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_core_access_t core;
    phymod_lane_map_t lane_map;
    uint32 idx;

    SOC_IF_ERROR_RETURN(phymod_core_access_t_init(&core));
    core.type = phy->type;
    sal_memcpy(&core.access, &phy->access, sizeof(core.access));
    core.access.lane_mask = 0;
    *value = 0;
    sal_memset(&lane_map, 0, sizeof(phymod_lane_map_t));

    SOC_IF_ERROR_RETURN(phymod_core_lane_map_get(&core, &lane_map));
    if(lane_map.num_of_lanes != LANE_MAP_NOF_LANES){
        return SOC_E_INTERNAL;
    }
    for (idx=0; idx < LANE_MAP_NOF_LANES; idx++) {
        *value += ((lane_map.lane_map_rx[idx] & 0xf) << (idx * 4));
    }
    for (idx=0; idx < LANE_MAP_NOF_LANES; idx++) {
        *value += ((lane_map.lane_map_tx[idx] & 0xf)<< (idx * 4 + 16));
    }
    
    return SOC_E_NONE;
}



STATIC int 
control_handler_loopback_internal_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_get(phy, phymodLoopbackGlobal, value));

    return SOC_E_NONE;
}

STATIC int 
control_handler_loopback_pmd_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_get(phy, phymodLoopbackGlobalPMD, value));

    return SOC_E_NONE;
}

STATIC int 
control_handler_loopback_remote_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_get(phy, phymodLoopbackRemotePMD, value));

    return SOC_E_NONE;
}

#if 0
STATIC int 
control_handler_loopback_remote_pcs_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_get(phy, phymodLoopbackRemotePCS, value));

    return SOC_E_NONE;
}
#endif

STATIC int
control_handler_rx_low_freq_filter_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_rx_t phymod_rx;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    *value = phymod_rx.low_freq_peaking_filter.value;

    return SOC_E_NONE;
}

STATIC int
control_handler_rx_high_freq_filter_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_rx_t phymod_rx;

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(phy, &phymod_rx));
    *value = phymod_rx.high_freq_peaking_filter.value;

    return SOC_E_NONE;
}


STATIC int
control_handler_scrambler_get(int unit, phymod_phy_access_t *phy, uint32 ref_clk, uint32 *value)
{
    phymod_phy_inf_config_t config;

    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(phy, 0 /* flags */, ref_clk, &config));
    *value = PHYMOD_INTF_MODES_SCR_GET(&config); 

    return SOC_E_NONE;
}

STATIC int
control_handler_fec_get(int unit, phymod_phy_access_t *phy, uint32 ref_clk, uint32 *value)
{
    uint32_t               enable = *value;

    SOC_IF_ERROR_RETURN(phymod_phy_fec_enable_get(phy,  &enable));
    *value = enable; 

    return SOC_E_NONE;
}

STATIC int
control_handler_fec_override_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{

    SOC_IF_ERROR_RETURN(phymod_phy_fec_override_get(phy, value));

    return SOC_E_NONE;
}

STATIC int
control_handler_firmware_mode_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_firmware_lane_config_t fw_config;
    int control = param;

    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(phy, &fw_config));

    switch (control) {
    case SOC_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE:
        *value = fw_config.DfeOn && fw_config.ForceBrDfe ? 1 : 0;
        break;
    case SOC_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE:
        *value = fw_config.DfeOn && fw_config.LpDfeOn ? 1 : 0;
        break;
    case SOC_PHY_CONTROL_FIRMWARE_DFE_ENABLE:
        *value = fw_config.DfeOn ? 1 : 0;
        break;
    case SOC_PHY_CONTROL_FIRMWARE_CL72_RESTART_TIMEOUT_ENABLE:
        *value = fw_config.Cl72RestTO ? 1 : 0;
        break;
    case SOC_PHY_CONTROL_FIRMWARE_CL72_AUTO_POLARITY_ENABLE:
        *value = fw_config.Cl72AutoPolEn ? 1 : 0;
        break;
    case SOC_PHY_CONTROL_FIRMWARE_AN_CL72_TX_INIT_SKIP_ON_RESTART:
        *value = fw_config.AnCl72TxInitSkipOnRestart ? 1 : 0;
        break;
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
        if ((fw_config.LpDfeOn) && (fw_config.DfeOn)) {
            *value = SOC_PHY_FIRMWARE_LP_DFE_ENABLE;
        } else if ((fw_config.ForceBrDfe) && (fw_config.DfeOn)) {
            *value = SOC_PHY_FIRMWARE_FORCE_BRDFE;
        } else if (fw_config.DfeOn) {
            *value = SOC_PHY_FIRMWARE_DFE_ENABLE;
        } else {
            *value = SOC_PHY_FIRMWARE_DEFAULT;
        }
        break;
    case SOC_PHY_CONTROL_FIRMWARE_RX_FORCE_EXTENDED_REACH_ENABLE:
        *value = fw_config.ForceExtenedReach ? 1 : 0;
        break;
    case SOC_PHY_CONTROL_FIRMWARE_RX_FORCE_NORMAL_REACH_ENABLE:
        *value = fw_config.ForceNormalReach ? 1 : 0;
        break;
    }
                    

    return SOC_E_NONE;

}
STATIC int
control_handler_rx_seq_done_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{

    SOC_IF_ERROR_RETURN(phymod_phy_rx_pmd_locked_get(phy, value));

    return SOC_E_NONE;
}

STATIC int 
control_handler_tx_polarity_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_polarity_t    polarity;

    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(phy, &polarity));
    *value = polarity.tx_polarity;

    return SOC_E_NONE;
}


STATIC int 
control_handler_rx_polarity_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    phymod_polarity_t    polarity;

    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(phy, &polarity));
    *value = polarity.rx_polarity;

    return SOC_E_NONE;
}


STATIC int
control_handler_rx_reset_get(int unit, phymod_phy_access_t *phy,  uint32 param, uint32 *value)
{
    phymod_phy_reset_t reset;

    SOC_IF_ERROR_RETURN(phymod_phy_reset_get(phy, &reset));
    *value = (reset.rx  == phymodResetDirectionIn) ?  1 : 0;

    return SOC_E_NONE;
}


STATIC int
control_handler_tx_reset_get(int unit, phymod_phy_access_t *phy,  uint32 param, uint32 *value)
{
    phymod_phy_reset_t reset;

    SOC_IF_ERROR_RETURN(phymod_phy_reset_get(phy, &reset));
    *value = (reset.tx  == phymodResetDirectionIn) ?  1 : 0;

    return SOC_E_NONE;
}

STATIC int
control_handler_interface_set (int unit, phymod_phy_access_t *phy,  uint32 intf_type, uint32 param2)
{
    phymod_phy_inf_config_t config;

    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(phy, 0 /* flags */, param2, &config));
    SOC_IF_ERROR_RETURN(soc_phymod_phy_intf_from_port_intf(unit,
                    config.data_rate, intf_type, &(config.interface_type)));
    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_set(phy, 0, &config));

    return SOC_E_NONE;
}

STATIC int
control_handler_interface_get (int unit, phymod_phy_access_t *phy, uint32_t param2,  uint32_t *intf_type )
{
    phymod_phy_inf_config_t config;

    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(phy, 0 /* flags */, param2, &config));
    SOC_IF_ERROR_RETURN(soc_phymod_phy_intf_to_port_intf(unit, config.interface_type, intf_type));

    return SOC_E_NONE;
}

STATIC int
control_handler_gpio_config_set(int unit, phymod_phy_access_t *phy,  uint32 value, uint32 param2) {
    int gpio_pin_no = 0 ;
    phymod_gpio_mode_t gpio_mode= (value >= phymodGpioModeCount)? phymodGpioModeDisabled : value;

    for (gpio_pin_no = 0; gpio_pin_no < 4; gpio_pin_no++) {
        gpio_mode = (value >> (4 * gpio_pin_no)) & 0xF;
        SOC_IF_ERROR_RETURN(phymod_phy_gpio_config_set(phy, gpio_pin_no, gpio_mode));
    }

    return SOC_E_NONE;
}

STATIC int
control_handler_gpio_config_get(int unit, phymod_phy_access_t *phy,  uint32 param2, uint32 *value) {
    int gpio_pin_no = 0 ;
    phymod_gpio_mode_t gpio_mode= phymodGpioModeDisabled;


    for (gpio_pin_no = 0; gpio_pin_no < 4; gpio_pin_no++) {
        SOC_IF_ERROR_RETURN(phymod_phy_gpio_config_get(phy, gpio_pin_no, &gpio_mode));
        *value |= (gpio_mode << (4 * gpio_pin_no));
    }

    return SOC_E_NONE;
}

STATIC int
control_handler_gpio_pin_value_set(int unit, phymod_phy_access_t *phy,  uint32 value, uint32 param2) {
    int gpio_pin_no = 0 ;
    int gpio_pin_value = 0 ;

    for (gpio_pin_no = 0; gpio_pin_no < 4; gpio_pin_no++) {
        gpio_pin_value = (value >> (4 * gpio_pin_no)) & 0xF;
        SOC_IF_ERROR_RETURN(phymod_phy_gpio_pin_value_set(phy, gpio_pin_no, gpio_pin_value));
    }

    return SOC_E_NONE;
}

STATIC int
control_handler_gpio_pin_value_get(int unit, phymod_phy_access_t *phy,  uint32 param2, uint32 *value) {
    int gpio_pin_no = 0 ;
    int gpio_pin_value = 0 ;


    for (gpio_pin_no = 0; gpio_pin_no < 4; gpio_pin_no++) {
        SOC_IF_ERROR_RETURN(phymod_phy_gpio_pin_value_get(phy, gpio_pin_no, &gpio_pin_value));
        *value |= (gpio_pin_value << (4 * gpio_pin_no));
    }

    return SOC_E_NONE;
}

STATIC int
control_handler_intr_status_get(int unit, phymod_phy_access_t *phy,  uint32 param2, uint32 *value) {

    *value = 0;
    SOC_IF_ERROR_RETURN(phymod_phy_intr_status_get(phy, value));

    return(phymod_phy_intr_status_clear(phy, *value));
}

STATIC int
control_handler_linkdown_transmit_set(int unit, phymod_phy_access_t *phy,  uint32 value, uint32 param2) {

    return SOC_E_NONE;
}

STATIC int
control_handler_linkdown_transmit_get(int unit, phymod_phy_access_t *phy,  uint32 param2, uint32 *value) {

    return SOC_E_NONE;
}

STATIC int
control_handler_intr_enable_set(int unit, phymod_phy_access_t *phy,  uint32 value, uint32 param2) {

    return(phymod_phy_intr_enable_set(phy, value));
}

STATIC int
control_handler_intr_enable_get(int unit, phymod_phy_access_t *phy,  uint32 param2, uint32 *value) {

    return(phymod_phy_intr_enable_get(phy, value));
}

STATIC int
control_handler_unreliable_los_enable_set(int unit, phymod_phy_access_t *phy,  uint32 value, uint32 param2) {

    phymod_firmware_lane_config_t config;
    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(phy, &config));
    config.UnreliableLos = value;
    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_set(phy, config));

    return SOC_E_NONE;
}

STATIC int
control_handler_unreliable_los_enable_get(int unit, phymod_phy_access_t *phy,  uint32 param2, uint32 *value) {

    phymod_firmware_lane_config_t config;
    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(phy, &config));
    *value = config.UnreliableLos;

    return SOC_E_NONE;
}

STATIC int
control_handler_eye_margin_hz_l_get(int unit, phymod_phy_access_t *phy,  uint32 param2, uint32 *value) {

    SOC_IF_ERROR_RETURN(phymod_phy_eye_margin_est_get(phy, phymod_eye_marign_HZ_L, value));

    return SOC_E_NONE;
}

STATIC int
control_handler_eye_margin_hz_r_get(int unit, phymod_phy_access_t *phy,  uint32 param2, uint32 *value) {

    SOC_IF_ERROR_RETURN(phymod_phy_eye_margin_est_get(phy, phymod_eye_marign_HZ_R, value));

    return SOC_E_NONE;
}

STATIC int
control_handler_eye_margin_vt_u_get(int unit, phymod_phy_access_t *phy,  uint32 param2, uint32 *value) {

    SOC_IF_ERROR_RETURN(phymod_phy_eye_margin_est_get(phy, phymod_eye_marign_VT_U, value));

    return SOC_E_NONE;
}

STATIC int
control_handler_eye_margin_vt_d_get(int unit, phymod_phy_access_t *phy,  uint32 param2, uint32 *value) {

    SOC_IF_ERROR_RETURN(phymod_phy_eye_margin_est_get(phy, phymod_eye_marign_VT_D, value));

    return SOC_E_NONE;
}

STATIC int
control_handler_autoneg_get (int unit, phymod_phy_access_t *phy, uint32_t param2,  uint32_t *value )
{
    phymod_autoneg_control_t an;
    int an_done;
    SOC_IF_ERROR_RETURN(phymod_phy_autoneg_get(phy, &an, (uint32 *)&an_done));
    switch(an.an_mode) {
        case phymod_AN_MODE_CL37:
            *value= SOC_PHY_CONTROL_AUTONEG_MODE_CL37;
            break;
        case phymod_AN_MODE_CL37BAM:
            *value = SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM;
            break;
        case phymod_AN_MODE_CL73:
            *value = SOC_PHY_CONTROL_AUTONEG_MODE_CL73;
            break;
        case phymod_AN_MODE_CL73BAM:
            *value = SOC_PHY_CONTROL_AUTONEG_MODE_CL73_BAM;
            break;
        default:
            return SOC_E_PARAM;
            break;
    }
    return SOC_E_NONE;
}

STATIC int
control_handler_pm_codec_enable_set(int unit,
                                   phymod_phy_access_t *phy,
                                   uint32_t value,
                                   uint32_t param2)
{
    phymod_phy_hg2_codec_t codec_mode;

    if (phy == NULL) return SOC_E_INTERNAL;

    switch (value) {
        case SOC_PHY_CONTROL_HG2_BCM_CODEC_ENABLE_OFF:
            codec_mode = phymodBcmHG2CodecOff;
            break;
        case SOC_PHY_CONTROL_HG2_BCM_CODEC_ENABLE_ON_WITH_8BYTE_IPG:
            codec_mode = phymodBcmHG2CodecOnWith8ByteIPG;
            break;
        case SOC_PHY_CONTROL_HG2_BCM_CODEC_ENABLE_ON_WITH_9BYTE_IPG:
            codec_mode = phymodBcmHG2CodecOnWith9ByteIPG;
            break;
        default:
            codec_mode = phymodBcmHG2CodecOff;
            break;
    }

    return (phymod_phy_hg2_codec_control_set(phy, codec_mode));
}

STATIC int
control_handler_pm_codec_enable_get(int unit,
                                   phymod_phy_access_t *phy,
                                   uint32_t param2,
                                   uint32_t *value)
{
    phymod_phy_hg2_codec_t codec_mode;

    if (phy == NULL) return SOC_E_INTERNAL;

    SOC_IF_ERROR_RETURN(phymod_phy_hg2_codec_control_get(phy, &codec_mode));

    switch (codec_mode) {
        case phymodBcmHG2CodecOff:
            *value = SOC_PHY_CONTROL_HG2_BCM_CODEC_ENABLE_OFF;
            break;
        case phymodBcmHG2CodecOnWith8ByteIPG:
            *value = SOC_PHY_CONTROL_HG2_BCM_CODEC_ENABLE_ON_WITH_8BYTE_IPG;
            break;
        case phymodBcmHG2CodecOnWith9ByteIPG:
            *value = SOC_PHY_CONTROL_HG2_BCM_CODEC_ENABLE_ON_WITH_9BYTE_IPG;
            break;
        default:
            *value = SOC_PHY_CONTROL_HG2_BCM_CODEC_ENABLE_OFF;
            break;

    }
    return SOC_E_NONE;
}

STATIC int
control_handler_eee_set(int unit, phymod_phy_access_t *phy,  uint32 value, uint32 param2) {
    SOC_IF_ERROR_RETURN(phymod_phy_eee_set(phy, value));
    return SOC_E_NONE;
}

STATIC int
control_handler_eee_get (int unit, phymod_phy_access_t *phy, uint32_t param2,  uint32_t *value )
{
    SOC_IF_ERROR_RETURN(phymod_phy_eee_get(phy, value));
    return SOC_E_NONE;
}

STATIC int
control_handler_pam4_tx_pattern_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    uint32_t linear_enable = 0;
    uint32_t jp03b_enable = 0;

    SOC_IF_ERROR_RETURN(phymod_phy_PAM4_tx_pattern_enable_get(phy, phymod_PAM4TxPattern_JP03B, &jp03b_enable));
    SOC_IF_ERROR_RETURN(phymod_phy_PAM4_tx_pattern_enable_get(phy, phymod_PAM4TxPattern_Linear, &linear_enable));
    if (jp03b_enable) {
        *value = SOC_PHY_PAM4_TX_PATTERN_JP03B;
    } else if (linear_enable) {
        *value = SOC_PHY_PAM4_TX_PATTERN_LINEAR;
    } else {
        *value = SOC_PHY_PAM4_TX_PATTERN_OFF;
    }
    return SOC_E_NONE;
}

STATIC int
control_handler_pcs_mode_get (int unit, phymod_phy_access_t *phy, uint32 param2, uint32 *value)
{
    phymod_phy_inf_config_t config;

    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(phy, 0 /* flags */, param2, &config));
#ifdef PHYMOD_TSCE_SUPPORT
    if (phy->type == phymodDispatchTypeTsce) {
        if (PHYMOD_INTF_MODES_HIGIG_GET(&config)) {
            if (config.interface_type == phymodInterfaceXGMII) {
                *value = SOC_PHY_CONTROL_PCS_MODE_BRCM;
            } else {
                *value = SOC_PHY_CONTROL_PCS_MODE_IEEE;
            }
            return SOC_E_NONE;
        }
    }
#endif

    *value = SOC_PHY_CONTROL_PCS_MODE_IEEE;
    return SOC_E_NONE;
}

STATIC int
control_handler_fec_counter_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    if (param) {
        SOC_IF_ERROR_RETURN(phymod_phy_fec_uncorrectable_counter_get(phy, value));
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_fec_correctable_counter_get(phy, value));
    }
    return SOC_E_NONE;
}

STATIC int
control_handler_fec_cl91_counter_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    if (param) {
        SOC_IF_ERROR_RETURN(phymod_phy_fec_cl91_uncorrectable_counter_get(phy, value));
    } else {
        SOC_IF_ERROR_RETURN(phymod_phy_fec_cl91_correctable_counter_get(phy, value));
    }
    return SOC_E_NONE;
}

STATIC int
control_handler_rsfec_symb_err_counter_get(int unit, phymod_phy_access_t *phy, uint32 param, uint32 *value)
{
    uint32_t i, count = 0, error_count[PM_MAX_LANES];
    soc_port_phy_rsfec_symb_errcnt_t symbol_error_count;

    symbol_error_count.max_count = PM_MAX_LANES;
    symbol_error_count.symbol_errcnt = error_count;

    SOC_IF_ERROR_RETURN(phymod_phy_rsfec_symbol_error_counter_get(phy,
                                                                 symbol_error_count.max_count,
                                                                 &symbol_error_count.actual_count,
                                                                 symbol_error_count.symbol_errcnt));

    for (i = 0; i < symbol_error_count.actual_count; i++) {
         if ((0xffffffff - count) < error_count[i]) {
             count = 0xffffffff;
         } else {
             count += error_count[i];
         }
    }
    *value = count;

    return SOC_E_NONE;
}

STATIC int
control_handler_fec_bypass_indication_set(int unit, phymod_phy_access_t *phy,  uint32_t value, uint32_t param2) {
    SOC_IF_ERROR_RETURN(phymod_phy_fec_bypass_indication_set(phy, value));
    return SOC_E_NONE;
}

STATIC int
control_handler_fec_bypass_indication_get (int unit, phymod_phy_access_t *phy, uint32_t param2,  uint32_t *value )
{
    SOC_IF_ERROR_RETURN(phymod_phy_fec_bypass_indication_get(phy, value));
    return SOC_E_NONE;
}

STATIC int
control_handler_fec_corruption_period_set(int unit, phymod_phy_access_t *phy,  uint32_t value, uint32_t param2) {
    SOC_IF_ERROR_RETURN(phymod_phy_fec_corruption_period_set(phy, value));
    return SOC_E_NONE;
}

STATIC int
control_handler_fec_corruption_period_get (int unit, phymod_phy_access_t *phy, uint32_t param2,  uint32_t *value )
{
    SOC_IF_ERROR_RETURN(phymod_phy_fec_corruption_period_get(phy, value));
    return SOC_E_NONE;
}


STATIC int
control_handler_pdet_set(int unit, phymod_phy_access_t *phy,  uint32 value, uint32 param2)
{
    phymod_autoneg_control_t an_config;
    uint32      an_done;
    int           rv = SOC_E_NONE, num_lane = 0;
    uint32_t lane_mask;
    int control = param2;

    sal_memset(&an_config, 0, sizeof(phymod_autoneg_control_t));

    SOC_IF_ERROR_RETURN(phymod_phy_autoneg_get(phy, &an_config, &an_done));
    lane_mask = phy->access.lane_mask;
    while (lane_mask) {
        if (lane_mask & 0x1) num_lane++;
        lane_mask >>= 1;
    }
    an_config.num_lane_adv = num_lane;

    switch (control) {
        case SOC_PHY_CONTROL_PARALLEL_DETECTION:
            if (value) {
                PHYMOD_AN_F_SET_CL73_PDET_KX_ENABLE_SET(&an_config);
            } else {
                PHYMOD_AN_F_SET_CL73_PDET_KX_ENABLE_CLR(&an_config);
            }
            break;
        case SOC_PHY_CONTROL_PARALLEL_DETECTION_10G:
            if (value) {
                PHYMOD_AN_F_SET_CL73_PDET_KX4_ENABLE_SET(&an_config);
            } else {
                PHYMOD_AN_F_SET_CL73_PDET_KX4_ENABLE_CLR(&an_config);
            }
            break;
    }

    rv = phymod_phy_autoneg_set(phy, &an_config);
    /* Because the returned value PHYMOD_E_UNAVAIL is -12 from tscf phymod driver */
    if (rv == PHYMOD_E_UNAVAIL) {
        rv = SOC_E_UNAVAIL;
    }

    return rv;

}

STATIC int
control_handler_pdet_get (int unit, phymod_phy_access_t *phy, uint32_t param,  uint32_t *value )
{
    phymod_autoneg_control_t an_config;
    uint32      an_done;
    int control = param;

    sal_memset(&an_config, 0, sizeof(phymod_autoneg_control_t));

    SOC_IF_ERROR_RETURN(phymod_phy_autoneg_get(phy, &an_config, &an_done));
    switch (control) {
        case SOC_PHY_CONTROL_PARALLEL_DETECTION:
            if (PHYMOD_AN_F_SET_CL73_PDET_KX_ENABLE_GET(&an_config)) {
                *value = 1;
            } else {
                *value = 0;
            }
            break;
        case SOC_PHY_CONTROL_PARALLEL_DETECTION_10G:
            if (PHYMOD_AN_F_SET_CL73_PDET_KX4_ENABLE_GET(&an_config)) {
                *value = 1;
            } else {
                *value = 0;
            }
            break;
    }

    return SOC_E_NONE;
}

STATIC int
control_handler_tx_pam4_precoder_enable_get(int unit, phymod_phy_access_t *phy,  uint32 param,  uint32 *value)
{
    int temp_value; 
    SOC_IF_ERROR_RETURN(phymod_phy_tx_pam4_precoder_enable_get(phy,&temp_value));
    *value = (uint32_t) temp_value;
    return SOC_E_NONE;
}

STATIC int
control_handler_lp_tx_precoder_enable_get(int unit, phymod_phy_access_t *phy,  uint32 param,  uint32 *value)
{
    phymod_firmware_lane_config_t fw_config;

    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(phy, &fw_config));
    *value = fw_config.LpPrecoderEnabled;
    return SOC_E_NONE;
}

STATIC int
control_handler_rx_ppm_get(int unit, phymod_phy_access_t *phy, uint32_t param2, uint32_t *value)
{
    int rv = SOC_E_NONE;
    int16 rx_ppm = 0;

    rv = phymod_phy_rx_ppm_get(phy, &rx_ppm);
    /* Because the returned value PHYMOD_E_UNAVAIL is -12 from viper phymod driver */
    if (rv == PHYMOD_E_UNAVAIL) {
        rv = SOC_E_UNAVAIL;
    } else {
        *value = (uint32_t)rx_ppm;
    }

    return rv;
}

STATIC int
control_handler_fast_ber_proj_get(int unit, phymod_phy_access_t *phy,  uint32 param2, uint32 *value)
{

    SOC_IF_ERROR_RETURN(phymod_phy_fast_ber_proj_get(phy, value));

    return SOC_E_NONE;
}


STATIC int
control_handler_channel_loss_hint_get(int unit, phymod_phy_access_t *phy,  uint32 param2, uint32 *value)
{

    SOC_IF_ERROR_RETURN(phymod_phy_channel_loss_hint_get(phy, value));

    return SOC_E_NONE;
}

STATIC int
control_handler_pcs_reset(int unit, phymod_phy_access_t *phy, uint32 value, uint32 param2)
{
    if (value) {
        SOC_IF_ERROR_RETURN(phymod_phy_pcs_reset(phy));
    }

    return SOC_E_NONE;
}


STATIC int
control_handler_base_r_fec_intr_enable_set(int unit, phymod_phy_access_t *phy,
                                           uint32 value, uint32 param2)
{
    SOC_IF_ERROR_RETURN(phymod_phy_interrupt_enable_set(phy,
                                                        phymodIntrEccBaseRFEC,
                                                        value));
    return SOC_E_NONE;
}

STATIC int
control_handler_base_r_fec_intr_enable_get(int unit, phymod_phy_access_t *phy,
                                           uint32 param2, uint32 *value)
{
    SOC_IF_ERROR_RETURN(phymod_phy_interrupt_enable_get(phy,
                                                        phymodIntrEccBaseRFEC,
                                                        value));
    return SOC_E_NONE;
}


STATIC int
control_handler_pmd_lane_diag_debug_level_set(int unit, phymod_phy_access_t *phy, uint32 value1, uint32 value2)
{
    if (value1 > 6) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN
        (phymod_phy_pmd_lane_diag_debug_level_set(phy, value1));

    return SOC_E_NONE;
}

STATIC int
control_handler_pmd_lane_diag_debug_level_get(int unit, phymod_phy_access_t *phy,  uint32 param2, uint32 *value)
{

    SOC_IF_ERROR_RETURN(phymod_phy_pmd_lane_diag_debug_level_get(phy, value));

    return SOC_E_NONE;
}

STATIC int
control_handler_flexe_50g_nofec_20k_am_spacing_enable_get(int unit, phymod_phy_access_t *phy,
                                                          uint32 param2, uint32 *value)
{
    int temp_value;

    SOC_IF_ERROR_RETURN(phymod_phy_50g_nofec_20k_am_spacing_enable_get(phy, &temp_value));
    *value = (uint32_t)temp_value;

    return SOC_E_NONE;
}

STATIC int
control_handler_pmd_usr_misc_ctrl_set(int unit, phymod_phy_access_t *phy,
                                      uint32 value1, uint32 value2)
{
    int control = value2;
    phymod_phy_pmd_usr_misc_ctrl_t misc_ctrl = phymodMiscCtrlAutoNegForceER;

    switch (control) {
        case SOC_PHY_CONTROL_FW_AN_FORCE_EXTENDED_REACH_ENABLE:
            misc_ctrl = phymodMiscCtrlAutoNegForceER;
            break;
        case SOC_PHY_CONTROL_FW_AN_FORCE_NORMAL_REACH_ENABLE:
            misc_ctrl = phymodMiscCtrlAutoNegForceNR;
            break;
        default:
            break;
    }
    SOC_IF_ERROR_RETURN
        (phymod_phy_pmd_usr_misc_ctrl_set(phy, misc_ctrl, value1));

    return SOC_E_NONE;
}

STATIC int
control_handler_pmd_usr_misc_ctrl_get(int unit, phymod_phy_access_t *phy,
                                      uint32 param, uint32 *value)
{
    int control = param;
    phymod_phy_pmd_usr_misc_ctrl_t misc_ctrl = phymodMiscCtrlAutoNegForceER;

    switch (control) {
        case SOC_PHY_CONTROL_FW_AN_FORCE_EXTENDED_REACH_ENABLE:
            misc_ctrl = phymodMiscCtrlAutoNegForceER;
            break;
        case SOC_PHY_CONTROL_FW_AN_FORCE_NORMAL_REACH_ENABLE:
            misc_ctrl = phymodMiscCtrlAutoNegForceNR;
            break;
        default:
            break;
    }

    SOC_IF_ERROR_RETURN
        (phymod_phy_pmd_usr_misc_ctrl_get(phy, misc_ctrl, value));

    return SOC_E_NONE;
}

/****************************************************************************** 
  port and phy control set wrapers
 */


/* when calling this function,  make sure pack enough array of phymod_phy_access_t if nof_phys != 1, and phys points to the first object */
/* assume the phymod_phy_access object is just a working copy, no need to make a copy again or reset the object content */
int soc_port_control_get_wrapper(int unit, phymod_ref_clk_t ref_clock, int is_lane_control, phymod_phy_access_t *phys, int nof_phys, soc_phy_control_t control, uint32 *value)
{
    control_get_handler_f handler = NULL;
    uint32 param = 0;
    int i;
    int lane_control_support = FALSE;

    if ((control < 0) || (control >= SOC_PHY_CONTROL_COUNT)) {
        return (SOC_E_PARAM);
    }

    *value = 0;
    switch(control) {
    case SOC_PHY_CONTROL_TX_FIR_MODE:
        handler = control_handler_tx_fir_mode_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_PRE2:
        handler = control_handler_tx_fir_pre2_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_PRE:
        handler = control_handler_tx_fir_pre_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
        handler = control_handler_tx_fir_main_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST:
        handler = control_handler_tx_fir_post_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        handler = control_handler_tx_fir_post2_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST3:
        handler = control_handler_tx_fir_post3_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_LINK_TRAINING_INIT_TX_FIR_POST:
        handler = control_handler_link_training_tx_fir_post_get;
        lane_control_support = TRUE;
        break;
    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS:
        handler = control_handler_preemphasis_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        phys->access.lane_mask = 0x1 ;
        handler = control_handler_preemphasis_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        phys->access.lane_mask = 0x2 ;
        handler = control_handler_preemphasis_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        phys->access.lane_mask = 0x4 ;
        handler = control_handler_preemphasis_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        phys->access.lane_mask = 0x8 ;
        handler = control_handler_preemphasis_get;
        lane_control_support = TRUE;
        break;
    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        phys->access.lane_mask = 0x1 ;
        handler = control_handler_driver_current_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        phys->access.lane_mask = 0x2 ;
        handler = control_handler_driver_current_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        phys->access.lane_mask = 0x4 ;
        handler = control_handler_driver_current_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        phys->access.lane_mask = 0x8 ;
        handler = control_handler_driver_current_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        handler = control_handler_driver_current_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_DRIVERMODE:
        handler = control_handler_tx_fir_drivermode_get;
        lane_control_support = TRUE;
        break;
     /* TX LANE SQUELCH */
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        handler = control_handler_tx_squelch_get;
        lane_control_support = TRUE;
        break;
     /* RX LANE SQUELCH */
    case SOC_PHY_CONTROL_RX_LANE_SQUELCH:
        handler = control_handler_rx_squelch_get;
        lane_control_support = TRUE;
        break;
    /* RX PEAK FILTER */
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        handler = control_handler_rx_peak_filter_get;
        lane_control_support = TRUE;
        break;
    /* RX VGA */
    case SOC_PHY_CONTROL_RX_VGA:
        handler = control_handler_rx_vga_get;
        lane_control_support = TRUE;
        break;
    /* RX TAP */
    case SOC_PHY_CONTROL_RX_TAP1:
        handler = control_handler_rx_tap_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        handler = control_handler_rx_tap_get;
        lane_control_support = TRUE;
        param = 1;
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        handler = control_handler_rx_tap_get;
        lane_control_support = TRUE;
        param = 2;
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        handler = control_handler_rx_tap_get;
        lane_control_support = TRUE;
        param = 3;
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        handler = control_handler_rx_tap_get;
        lane_control_support = TRUE;
        param = 4;
        break;
    case SOC_PHY_CONTROL_RX_TAP6:
        handler = control_handler_rx_tap_get;
        lane_control_support = TRUE;
        param = 5;
        break;
    /* PHASE INTERPOLATOR */
    case SOC_PHY_CONTROL_PHASE_INTERP:
    case SOC_PHY_CONTROL_TX_PPM_ADJUST:
        handler = control_handler_pi_control_get;
        lane_control_support = TRUE;
        break;

    /* RX_SIGNAL_DETECT  */
    case SOC_PHY_CONTROL_RX_SIGNAL_DETECT :
        handler = control_handler_rx_signal_detect_get;
        lane_control_support = TRUE;
        break;

    /* CL72 ENABLE */
    case SOC_PHY_CONTROL_CL72:
        handler = control_handler_cl72_enable_get;
        lane_control_support = TRUE;
        break;

    /* SHORT_CHANNEL_MODE ENABLE */
    case SOC_PHY_CONTROL_SHORT_CHANNEL_MODE:
        handler = control_handler_short_chn_mode_enable_get;
        lane_control_support = TRUE;
        break;

    /* SHORT_CHANNEL_MODE ENABLE STATUS*/
    case SOC_PHY_CONTROL_SHORT_CHANNEL_MODE_STATUS:
        handler = control_handler_short_chn_mode_status_get;
        lane_control_support = TRUE;
        break;

    /* CL72 STATUS */
    case SOC_PHY_CONTROL_CL72_STATUS:
        handler = control_handler_cl72_status_get;
        lane_control_support = TRUE;
        break;

    /* LANE SWAP */
    case SOC_PHY_CONTROL_LANE_SWAP:
        handler = control_handler_lane_map_get;
        break;
    /* TX PATTERN */
    case SOC_PHY_CONTROL_TX_PATTERN_LENGTH:
        handler = control_handler_pattern_length_get;
        lane_control_support = TRUE;
        break ;
    case SOC_PHY_CONTROL_TX_PATTERN_GEN_ENABLE:
        handler = control_handler_pattern_enable_get;
        lane_control_support = TRUE;
        break ;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA0:
        handler = control_handler_pattern_config_get;
        lane_control_support = TRUE;
        param = 0;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA1:
        handler = control_handler_pattern_config_get;
        lane_control_support = TRUE;
        param = 1;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA2:
        handler = control_handler_pattern_config_get;
        lane_control_support = TRUE;
        param = 2;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA3:
        handler = control_handler_pattern_config_get;
        lane_control_support = TRUE;
        param = 3;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA4:
        handler = control_handler_pattern_config_get;
        lane_control_support = TRUE;
        param = 4;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA5:
        handler = control_handler_pattern_config_get;
        lane_control_support = TRUE;
        param = 5;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA6:
        handler = control_handler_pattern_config_get;
        lane_control_support = TRUE;
        param = 6;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA7:
        handler = control_handler_pattern_config_get;
        lane_control_support = TRUE;
        param = 7;
        break;
    /* PRBS */
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        handler = control_handler_prbs_poly_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        handler = control_handler_prbs_tx_invert_data_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        handler = control_handler_prbs_tx_enable_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        handler = control_handler_prbs_rx_invert_data_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        handler = control_handler_prbs_rx_enable_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        handler = control_handler_prbs_tx_poly_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        handler = control_handler_prbs_tx_invert_data_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        handler = control_handler_prbs_tx_enable_get;
        lane_control_support = TRUE;
        break; 
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        handler = control_handler_prbs_rx_poly_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        handler = control_handler_prbs_rx_invert_data_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        handler = control_handler_prbs_rx_enable_get;
        lane_control_support = TRUE;
        break; 
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
        handler = control_handler_prbs_rx_status_get;
        lane_control_support = TRUE;
        break;
    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_INTERNAL:
        handler = control_handler_loopback_internal_get;
        lane_control_support = TRUE;
       break;
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
        handler = control_handler_loopback_pmd_get;
        lane_control_support = TRUE;
       break;

    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
        handler = control_handler_loopback_remote_get;
        lane_control_support = TRUE;
        break;

    /* SCRAMBLER */
    case SOC_PHY_CONTROL_SCRAMBLER:
        handler = control_handler_scrambler_get;
        param = ref_clock;
        break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        handler = control_handler_fec_get;
        param = ref_clock;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91:
        handler = control_handler_fec_get;
        param = ref_clock;
        lane_control_support = TRUE;
        PHYMOD_FEC_CL108_CLR(*value);
        PHYMOD_FEC_CL91_SET(*value);
        break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL108:
        handler = control_handler_fec_get;
        param = ref_clock;
        lane_control_support = TRUE;
        PHYMOD_FEC_CL91_CLR(*value);
        PHYMOD_FEC_CL108_SET(*value);
        break;
    case SOC_PHY_CONTROL_AUTONEG_FEC_OVERRIDE:
        handler = control_handler_fec_override_get;
        lane_control_support = TRUE;
        break;        
    /* RX_LOW_FREQ_PEAK_FILTER */
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        handler = control_handler_rx_low_freq_filter_get;
        lane_control_support = TRUE;
        break;
    /* RX_HIGH_FREQ_PEAK_FILTER */
    case SOC_PHY_CONTROL_RX_HIGH_FREQ_PEAK_FILTER:
        handler = control_handler_rx_high_freq_filter_get;
        lane_control_support = TRUE;
        break;
    /* RX_SEQ_DONE */
    case SOC_PHY_CONTROL_RX_SEQ_DONE:
       handler = control_handler_rx_seq_done_get;
       lane_control_support = TRUE;
       break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        handler = control_handler_tx_polarity_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_RX_POLARITY:
        handler = control_handler_rx_polarity_get;
        lane_control_support = TRUE;
        break;
     /* RESET */
    case SOC_PHY_CONTROL_RX_RESET:
        handler = control_handler_rx_reset_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_RESET:
        handler = control_handler_tx_reset_get;
        lane_control_support = TRUE;
        break;
    /* POWER */
    case SOC_PHY_CONTROL_POWER:
        handler = control_handler_power_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
    case SOC_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_DFE_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_CL72_RESTART_TIMEOUT_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_CL72_AUTO_POLARITY_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_RX_FORCE_EXTENDED_REACH_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_RX_FORCE_NORMAL_REACH_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_AN_CL72_TX_INIT_SKIP_ON_RESTART:
        handler = control_handler_firmware_mode_get;
        lane_control_support = TRUE;
        param = control;
        break; 
    case SOC_PHY_CONTROL_MEDIUM_TYPE:
        handler = control_handler_medium_type_get;
        lane_control_support = TRUE;
        param = ref_clock;
        break;

    case SOC_PHY_CONTROL_INTERFACE:
        /* value=interface type, control=switch param */ 
        handler = control_handler_interface_get;
        param = ref_clock;
        lane_control_support = TRUE;
        break;

    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        /* handler = control_handler_loopback_remote_pcs_get; */
        handler = control_handler_loopback_remote_get;
        lane_control_support = TRUE;
        break;

    case SOC_PHY_CONTROL_GPIO_CONFIG:
        handler = control_handler_gpio_config_get;
        lane_control_support = TRUE;
        break;

    case SOC_PHY_CONTROL_GPIO_VALUE:
        handler = control_handler_gpio_pin_value_get;
        lane_control_support = TRUE;
        break;


    case SOC_PHY_CONTROL_INTR_STATUS:
        handler = control_handler_intr_status_get;
        lane_control_support = TRUE;
        break;

    case SOC_PHY_CONTROL_INTR_MASK:
        handler = control_handler_intr_enable_get;
        lane_control_support = TRUE;
        break;

    case SOC_PHY_CONTROL_UNRELIABLE_LOS:
        handler = control_handler_unreliable_los_enable_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_EYE_VAL_HZ_L:
        handler = control_handler_eye_margin_hz_l_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_EYE_VAL_HZ_R:
        handler = control_handler_eye_margin_hz_r_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_EYE_VAL_VT_U:
        handler = control_handler_eye_margin_vt_u_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_EYE_VAL_VT_D:
        handler = control_handler_eye_margin_vt_d_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_AUTONEG_MODE:
        handler = control_handler_autoneg_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
	handler = control_handler_linkdown_transmit_get;
	lane_control_support = TRUE;
	break;
    case SOC_PHY_CONTROL_HG2_BCM_CODEC_ENABLE:
        handler = control_handler_pm_codec_enable_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_EEE:
        handler = control_handler_eee_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PAM4_TX_PATTERN_ENABLE:
        handler = control_handler_pam4_tx_pattern_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PCS_MODE:
        handler = control_handler_pcs_mode_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_FEC_SYMBOL_ERROR_COUNT:
        handler = control_handler_rsfec_symb_err_counter_get;
        lane_control_support = TRUE;
        param = is_lane_control;
        break;
    case SOC_PHY_CONTROL_FEC_CORRECTED_BLOCK_COUNT:
        handler = control_handler_fec_counter_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_FEC_UNCORRECTED_BLOCK_COUNT:
        handler = control_handler_fec_counter_get;
        lane_control_support = TRUE;
        param = 1;
        break;
    case SOC_PHY_CONTROL_FEC_CORRECTED_CODEWORD_COUNT:
        handler = control_handler_fec_cl91_counter_get;
        break;
    case SOC_PHY_CONTROL_FEC_UNCORRECTED_CODEWORD_COUNT:
        handler = control_handler_fec_cl91_counter_get;
        param = 1;
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
    case SOC_PHY_CONTROL_PARALLEL_DETECTION_10G:
        handler = control_handler_pdet_get;
        lane_control_support = TRUE;
        param = control;
        break;
    case SOC_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE:
        handler = control_handler_tx_pam4_precoder_enable_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_LP_TX_PRECODER_ENABLE:
        handler = control_handler_lp_tx_precoder_enable_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_RX_PPM:
        handler = control_handler_rx_ppm_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_FAST_BER_PROJ:
        handler = control_handler_fast_ber_proj_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_CHANNEL_LOSS_HINT:
        handler = control_handler_channel_loss_hint_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_FEC_BYPASS_INDICATION_ENABLE:
        handler = control_handler_fec_bypass_indication_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_FEC_CORRUPTION_PERIOD:
        handler = control_handler_fec_corruption_period_get;
        break;
    case SOC_PHY_CONTROL_BASE_R_FEC_ECC_INTR_ENABLE:
        handler = control_handler_base_r_fec_intr_enable_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PMD_DIAG_DEBUG_LANE_EVENT_LOG_LEVEL:
        handler = control_handler_pmd_lane_diag_debug_level_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_FLEXE_50G_NOFEC_20K_AM_SPACING_ENABLE:
        handler = control_handler_flexe_50g_nofec_20k_am_spacing_enable_get;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_FW_AN_FORCE_EXTENDED_REACH_ENABLE:
    case SOC_PHY_CONTROL_FW_AN_FORCE_NORMAL_REACH_ENABLE:
        handler = control_handler_pmd_usr_misc_ctrl_get;
        lane_control_support = TRUE;
        param = control;
        break;
    default: 
        return SOC_E_UNAVAIL;
        break; 
    }
    if(is_lane_control && !lane_control_support){
        return SOC_E_UNAVAIL;
    }
    if(handler == NULL){
        return SOC_E_INTERNAL;
    }

    if(phys[0].access.lane_mask){
        SOC_IF_ERROR_RETURN(handler(unit, &phys[0], param, value));
    }

    if(control == SOC_PHY_CONTROL_PRBS_RX_STATUS){
        uint32 value2 = 0;
        for(i = 1 ; i < nof_phys ; i++){

            /* if lane mask is zero, skip it. */
            if(phys[i].access.lane_mask){
                SOC_IF_ERROR_RETURN(handler(unit, &phys[i], param, &value2));

                /* -1 !prbs_lock ,  -2 prbs_lock_loss, 0 1 2 ... prbs err cnt.
                   -1 supersede everybody.
                   -2 is next in line
                    0 ..   treated equally. */

            if( ((int)(*value)) != -1 ) {
                if((int)value2 < 0){
                        *value = value2;
                } else if( ((int)(*value)) >=0 ) {
                        *value += value2;
                    }
                }
            }
        }
    } else {
        for(i = 1 ; i < nof_phys ; i++){
            if(phys[i].access.lane_mask){
                SOC_IF_ERROR_RETURN(handler(unit, &phys[i], param, value));
            }
        }
    }

    return SOC_E_NONE;
}

int soc_port_control_set_wrapper(int unit, phymod_ref_clk_t ref_clock, int is_lane_control, phymod_phy_access_t *phys, int nof_phys, soc_phy_control_t control, uint32 value)
{
    control_set_handler_f handler = NULL;
    uint32 param2 = 0;
    int lane_control_support = FALSE;
    int i;

    if ((control < 0) || (control >= SOC_PHY_CONTROL_COUNT)) {
        return (SOC_E_PARAM);
    }

    switch(control) {
    case SOC_PHY_CONTROL_TX_FIR_MODE:
        handler = control_handler_tx_fir_mode_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_PRE2:
        handler = control_handler_tx_fir_pre2_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_PRE:
        handler = control_handler_tx_fir_pre_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
        handler = control_handler_tx_fir_main_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST:
        handler = control_handler_tx_fir_post_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        handler = control_handler_tx_fir_post2_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST3:
        handler = control_handler_tx_fir_post3_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_LINK_TRAINING_INIT_TX_FIR_POST:
        handler = control_handler_link_training_tx_fir_post_set;
        lane_control_support = TRUE;
        break;
    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS:
        handler = control_handler_preemphasis_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        phys->access.lane_mask = 0x1 ;
        handler = control_handler_preemphasis_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        phys->access.lane_mask = 0x2 ;
        handler = control_handler_preemphasis_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        phys->access.lane_mask = 0x4 ;
        handler = control_handler_preemphasis_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        phys->access.lane_mask = 0x8 ;
        handler = control_handler_preemphasis_set;
        lane_control_support = TRUE;
        break;
    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
         phys->access.lane_mask = 0x1 ;
        handler = control_handler_driver_current_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
         phys->access.lane_mask = 0x2 ;
        handler = control_handler_driver_current_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
         phys->access.lane_mask = 0x4 ;
        handler = control_handler_driver_current_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
         phys->access.lane_mask = 0x8 ;
        handler = control_handler_driver_current_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        handler = control_handler_driver_current_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_FIR_DRIVERMODE:
        handler = control_handler_tx_fir_drivermode_set;
        lane_control_support = TRUE;
        break;
     /* TX LANE SQUELCH */
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        handler = control_handler_tx_squelch;
        lane_control_support = TRUE;
        break;
     /* RX LANE SQUELCH */
    case SOC_PHY_CONTROL_RX_LANE_SQUELCH:
        handler = control_handler_rx_squelch;
        lane_control_support = TRUE;
        break;
    /* RX PEAK FILTER */
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        handler = control_handler_rx_peak_filter_set;
        lane_control_support = TRUE;
        break;
    /* RX VGA */
    case SOC_PHY_CONTROL_RX_VGA:
        handler = control_handler_rx_vga_set;
        lane_control_support = TRUE;
        break;
    /* RX VGA RELEASE */
    case SOC_PHY_CONTROL_RX_VGA_RELEASE:
        handler = control_handler_rx_vga_release;
        lane_control_support = TRUE;
        break;
    /* RX TAP */
    case SOC_PHY_CONTROL_RX_TAP1:
        handler = control_handler_rx_tap_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        handler = control_handler_rx_tap_set;
        lane_control_support = TRUE;
        param2 = 1;
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        handler = control_handler_rx_tap_set;
        lane_control_support = TRUE;
        param2 = 2;
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        handler = control_handler_rx_tap_set;
        lane_control_support = TRUE;
        param2 = 3;
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        handler = control_handler_rx_tap_set;
        lane_control_support = TRUE;
        param2 = 4;
        break;
    case SOC_PHY_CONTROL_RX_TAP6:
        handler = control_handler_rx_tap_set;
        lane_control_support = TRUE;
        param2 = 5;
        break;
    case SOC_PHY_CONTROL_RX_TAP1_RELEASE:
        handler = control_handler_rx_tap_release;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_RX_TAP2_RELEASE:
        handler = control_handler_rx_tap_release;
        lane_control_support = TRUE;
        param2 = 1;
        break;
    case SOC_PHY_CONTROL_RX_TAP3_RELEASE:
        handler = control_handler_rx_tap_release;
        lane_control_support = TRUE;
        param2 = 2;
        break;
    case SOC_PHY_CONTROL_RX_TAP4_RELEASE:
        handler = control_handler_rx_tap_release;
        lane_control_support = TRUE;
        param2 = 3;
        break;
    case SOC_PHY_CONTROL_RX_TAP5_RELEASE:
        handler = control_handler_rx_tap_release;
        lane_control_support = TRUE;
        param2 = 4;
        break;
    case SOC_PHY_CONTROL_RX_TAP6_RELEASE:
        handler = control_handler_rx_tap_release;
        lane_control_support = TRUE;
        param2 = 5;
        break;
    case SOC_PHY_CONTROL_RX_ADAPTATION_RESUME:
        handler = control_handler_rx_adaptation_resume;
        lane_control_support = TRUE;
        break;
    /* PHASE INTERPOLATOR */
    case SOC_PHY_CONTROL_PHASE_INTERP:
    case SOC_PHY_CONTROL_TX_PPM_ADJUST:
        handler = control_handler_pi_control_set;
        lane_control_support = TRUE;
        break;
    /* POLARITY */
    case SOC_PHY_CONTROL_RX_POLARITY:
        handler = control_handler_rx_polarity_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        handler = control_handler_tx_polarity_set;
        lane_control_support = TRUE;
        break;
    /* RESET */
    case SOC_PHY_CONTROL_RX_RESET:
        handler = control_handler_rx_reset_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_TX_RESET:
        handler = control_handler_tx_reset_set;
        lane_control_support = TRUE;
        break;

    case SOC_PHY_CONTROL_DUMP:
        handler = control_handler_dsc_dump_set;
        lane_control_support = TRUE;
        break;

    /* CL72 ENABLE */
    case SOC_PHY_CONTROL_CL72:
        handler = control_handler_cl72_enable_set;
        lane_control_support = TRUE;
        break;

    /* SHORT_CHANNEL_MODE ENABLE */
    case SOC_PHY_CONTROL_SHORT_CHANNEL_MODE:
        handler = control_handler_short_chn_mode_enable_set;
        lane_control_support = TRUE;
        break;

    /* LANE SWAP */
    case SOC_PHY_CONTROL_LANE_SWAP:
        handler = control_handler_lane_map_set;
        break;

    /* TX PATTERN */
        
    case SOC_PHY_CONTROL_TX_PATTERN_LENGTH:
        handler = control_handler_pattern_length_set;
        lane_control_support = TRUE;
        break ;
    case SOC_PHY_CONTROL_TX_PATTERN_GEN_ENABLE:
        handler = control_handler_pattern_enable_set;
        lane_control_support = TRUE;
        break ;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA0:
        handler = control_handler_pattern_config_set;
        lane_control_support = TRUE;
        param2 = 0;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA1:
        handler = control_handler_pattern_config_set;
        lane_control_support = TRUE;
        param2 = 1;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA2:
        handler = control_handler_pattern_config_set;
        lane_control_support = TRUE;
        param2 = 2;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA3:
        handler = control_handler_pattern_config_set;
        lane_control_support = TRUE;
        param2 = 3;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA4:
        handler = control_handler_pattern_config_set;
        lane_control_support = TRUE;
        param2 = 4;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA5:
        handler = control_handler_pattern_config_set;
        lane_control_support = TRUE;
        param2 = 5;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA6:
        handler = control_handler_pattern_config_set;
        lane_control_support = TRUE;
        param2 = 6;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA7:
        handler = control_handler_pattern_config_set;
        lane_control_support = TRUE;
        param2 = 7;
        break;
    /* PRBS */
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        handler = control_handler_prbs_poly_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        handler = control_handler_prbs_tx_invert_data_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        handler = control_handler_prbs_tx_enable_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        handler = control_handler_prbs_rx_invert_data_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        handler = control_handler_prbs_rx_enable_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        handler = control_handler_prbs_tx_poly_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        handler = control_handler_prbs_tx_invert_data_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        handler = control_handler_prbs_tx_enable_set;
        lane_control_support = TRUE;
        break; 
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        handler = control_handler_prbs_rx_poly_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        handler = control_handler_prbs_rx_invert_data_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        handler = control_handler_prbs_rx_enable_set;
        lane_control_support = TRUE;
        break; 

    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_INTERNAL:
        handler = control_handler_loopback_internal_set;
        lane_control_support = TRUE;
       break;
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
        handler = control_handler_loopback_pmd_set;
        lane_control_support = TRUE;
       break;

    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        /* handler = control_handler_loopback_remote_pcs_set; */
        handler = control_handler_loopback_remote_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
        handler = control_handler_loopback_remote_set;
        lane_control_support = TRUE;
        break;

    /* SCRAMBLER */
    case SOC_PHY_CONTROL_SCRAMBLER:
        handler = control_handler_scrambler_set;
        lane_control_support = TRUE;
        param2 = ref_clock;
        break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        handler = control_handler_fec_set;
        lane_control_support = TRUE;
        param2 = ref_clock;
        break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91:
        handler = control_handler_fec_set;
        lane_control_support = TRUE;
        param2 = ref_clock;
        PHYMOD_FEC_CL108_CLR(value);
        PHYMOD_FEC_CL91_SET(value);
        break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL108:
        handler = control_handler_fec_set;
        lane_control_support = TRUE;
        param2 = ref_clock;
        PHYMOD_FEC_CL91_CLR(value);
        PHYMOD_FEC_CL108_SET(value);
        break;
    case SOC_PHY_CONTROL_AUTONEG_FEC_OVERRIDE:
        handler = control_handler_fec_override_set;
        lane_control_support = TRUE;
        break;        
    case SOC_PHY_CONTROL_CHANNEL_LOSS_HINT:
        handler = control_handler_channel_loss_hint_set;
        lane_control_support = TRUE;
        break;
#if defined(TODO_CONTROLS) /*not implemented yet*/
    /* 8B10B */
    case SOC_PHY_CONTROL_8B10B:
        rv = tsce_8b10b_set(pmc, value);
        break;

    /* 64B65B */
    case SOC_PHY_CONTROL_64B66B:
        rv = tsce_64b65b_set(pmc, value);
        break;
#endif /*not implemented yet*/

    /* POWER */
    case SOC_PHY_CONTROL_POWER:
        handler = control_handler_power_set;
        lane_control_support = TRUE;
        break;

    /* RX_LOW_FREQ_PEAK_FILTER */
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        handler = control_handler_rx_low_freq_filter_set;
        lane_control_support = TRUE;
        break;
    /* RX_HIGH_FREQ_PEAK_FILTER */
    case SOC_PHY_CONTROL_RX_HIGH_FREQ_PEAK_FILTER:
        handler = control_handler_rx_high_freq_filter_set;
        lane_control_support = TRUE;
        break;

#if defined(TODO_CONTROLS) /*not implemented yet*/
    /* TX_PPM_ADJUST */
    case SOC_PHY_CONTROL_TX_PPM_ADJUST:
       handler = ;
       break;
    /* VCO_FREQ */
    case SOC_PHY_CONTROL_VCO_FREQ:
       rv = tsce_vco_freq_set(pmc, value);
       break;

    /* PLL_DIVIDER */
    case SOC_PHY_CONTROL_PLL_DIVIDER:
       rv = tsce_pll_divider_set(pmc, value);
       break;

    /* OVERSAMPLE_MODE */
    case SOC_PHY_CONTROL_OVERSAMPLE_MODE:
       rv = tsce_oversample_mode_set(pmc, value);
       break;

    /* REF_CLK */
    case SOC_PHY_CONTROL_REF_CLK:
       rv = tsce_ref_clk_set(pmc, value);
       break;
    /* DRIVER_SUPPLY */
    case SOC_PHY_CONTROL_DRIVER_SUPPLY:
       rv = tsce_driver_supply_set(pmc, value);
       break;
#endif /*not implemented yet*/
    /* RX_SEQ_TOGGLE */
    case SOC_PHY_CONTROL_RX_SEQ_TOGGLE:
       handler = control_handler_rx_seq_toggle_set;
       lane_control_support = TRUE;
       break;

    case SOC_PHY_CONTROL_FIRMWARE_MODE:
    case SOC_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_DFE_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_CL72_RESTART_TIMEOUT_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_CL72_AUTO_POLARITY_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_RX_FORCE_EXTENDED_REACH_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_RX_FORCE_NORMAL_REACH_ENABLE:
    case SOC_PHY_CONTROL_FIRMWARE_AN_CL72_TX_INIT_SKIP_ON_RESTART:
        handler = control_handler_firmware_mode_set;
        lane_control_support = TRUE;
        param2 = control;
        break; 
    case SOC_PHY_CONTROL_MEDIUM_TYPE:
        handler = control_handler_medium_type_set;
        lane_control_support = TRUE;
        param2 = ref_clock;
        break;
    case SOC_PHY_CONTROL_INTERFACE:
        /* value=interface type, control=switch param */ 
        handler = control_handler_interface_set;
        param2 = ref_clock;
        lane_control_support = TRUE;
        break;

    case SOC_PHY_CONTROL_GPIO_CONFIG:
        handler = control_handler_gpio_config_set;
        lane_control_support = TRUE;
        break;

    case SOC_PHY_CONTROL_GPIO_VALUE:
        handler = control_handler_gpio_pin_value_set;
        lane_control_support = TRUE;
        break;

    case SOC_PHY_CONTROL_INTR_MASK:
        handler = control_handler_intr_enable_set;
        lane_control_support = TRUE;
        break;

    case SOC_PHY_CONTROL_UNRELIABLE_LOS:
        handler = control_handler_unreliable_los_enable_set;
        lane_control_support = TRUE;
        break;

    /* LINKDOWN TRANSMIT */
    /* Non-legacy phy no need to do register setting */
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        handler = control_handler_linkdown_transmit_set;
	lane_control_support = TRUE;
	break;

    case SOC_PHY_CONTROL_HG2_BCM_CODEC_ENABLE:
        handler = control_handler_pm_codec_enable_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_EEE:
        handler = control_handler_eee_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PAM4_TX_PATTERN_ENABLE:
        handler = control_handler_pam4_tx_pattern_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PCS_MODE:
        handler = control_handler_pcs_mode_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
    case SOC_PHY_CONTROL_PARALLEL_DETECTION_10G:
        handler = control_handler_pdet_set;
        lane_control_support = TRUE;
        param2 = control;
        break;
    case SOC_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE:
        handler = control_handler_tx_pam4_precoder_enable_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_LP_TX_PRECODER_ENABLE:
        handler = control_handler_lp_tx_precoder_enable_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_FEC_BYPASS_INDICATION_ENABLE:
        handler = control_handler_fec_bypass_indication_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_FEC_CORRUPTION_PERIOD:
        handler = control_handler_fec_corruption_period_set;
        break;
    case SOC_PHY_CONTROL_PCS_RESET:
        handler = control_handler_pcs_reset;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_BASE_R_FEC_ECC_INTR_ENABLE:
        handler = control_handler_base_r_fec_intr_enable_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_PMD_DIAG_DEBUG_LANE_EVENT_LOG_LEVEL:
        handler = control_handler_pmd_lane_diag_debug_level_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_FLEXE_50G_NOFEC_20K_AM_SPACING_ENABLE:
        handler = control_handler_flexe_50g_nofec_20k_am_spacing_enable_set;
        lane_control_support = TRUE;
        break;
    case SOC_PHY_CONTROL_FW_AN_FORCE_EXTENDED_REACH_ENABLE:
    case SOC_PHY_CONTROL_FW_AN_FORCE_NORMAL_REACH_ENABLE:
        handler = control_handler_pmd_usr_misc_ctrl_set;
        lane_control_support = TRUE;
        param2 = control;
        break;
    default:
        return SOC_E_UNAVAIL;
        break; 
    }

    if(is_lane_control && !lane_control_support){
        return SOC_E_UNAVAIL;
    }

    if(handler == NULL){
        return SOC_E_INTERNAL;
    }

    for(i = 0 ; i < nof_phys ; i++){
        /* if lane mask is 0, skip it. */
        if( phys[i].access.lane_mask) {    
            SOC_IF_ERROR_RETURN(handler(unit, &phys[i], value, param2));
        }
    }

    return SOC_E_NONE;
}

int soc_portctrl_phy_tx_set(int unit, phymod_phy_access_t *phy_access, phymod_tx_t *phy_tx)
{

    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(phy_access, phy_tx));

    return SOC_E_NONE;
}

int soc_portctrl_phy_tx_get(int unit, phymod_phy_access_t *phy_access, phymod_tx_t *phy_tx)
{

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(phy_access, phy_tx));

    return SOC_E_NONE;
}
#undef _ERR_MSG_MODULE_NAME
#else
typedef int phymod_port_control_not_empty; /* Make ISO compilers happy. */
#endif 
