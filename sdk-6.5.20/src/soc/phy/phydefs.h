/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:     phydefs.h
 * Purpose:  Basic defines for PHYs.
 *           This should be included after all other SOC include files and
 *           before all other PHY include files.
 */
#ifndef _PHY_CONFIG_H
#define _PHY_CONFIG_H 

#include <soc/defs.h>

/* PHY/Switch dependicies */
#if defined(BCM_DRACO_SUPPORT)
#define INCLUDE_PHY_5690
#endif /* BCM_DRACO_SUPPORT */

#if defined(BCM_XGS_SUPPORT)
#define INCLUDE_PHY_XGXS1
#endif /* BCM_XGS_SUPPORT */

#if defined(BCM_EASYRIDER_SUPPORT)
#define INCLUDE_PHY_56XXX
#define INCLUDE_PHY_XGXS5
#endif /* BCM_EASYRIDER_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT)
#define INCLUDE_PHY_56XXX
#define INCLUDE_PHY_XGXS5
#endif /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_HUMV_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) 
#define INCLUDE_PHY_XGXS6
#endif /* BCM_HUMV_SUPPORT || BCM_BRADLEY_SUPPORT */

#if defined(BCM_GOLDWING_SUPPORT)
#define INCLUDE_PHY_56XXX
#define INCLUDE_PHY_XGXS6
#endif /* BCM_GOLDWING_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT)
#define INCLUDE_SERDES_100FX   
#define INCLUDE_PHY_XGXS5
#endif /* BCM_FIREBOLT2_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT)
#define INCLUDE_PHY_56XXX
#define INCLUDE_SERDES_COMBO
#endif /* BCM_RAPTOR_SUPPORT */

#if defined(BCM_RAVEN_SUPPORT)
#define INCLUDE_SERDES_65LP
#define INCLUDE_SERDES_COMBO65
#endif /* BCM_RAVEN_SUPPORT */

#if defined(BCM_HAWKEYE_SUPPORT)
#define INCLUDE_XGXS_QSGMII65
#endif /* BCM_HAWKEYE_SUPPORT */

#if defined(BCM_HURRICANE_SUPPORT)
#define INCLUDE_XGXS_QSGMII65
#define INCLUDE_XGXS_WCMOD
#define INCLUDE_XGXS_TSCMOD
#endif /* BCM_HURRICANE_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_SHADOW_SUPPORT)
/* #define INCLUDE_XGXS_WCMOD  :debug WL driver with trident WC B0 board -ravick*/
#define INCLUDE_XGXS_WC40
#endif

#if defined(BCM_DFE_SUPPORT) || defined(BCM_ARAD_SUPPORT)
#define INCLUDE_XGXS_WCMOD
#endif

#if defined(BCM_TRIUMPH_SUPPORT)
#define INCLUDE_SERDES_65LP
#define INCLUDE_XGXS_HL65
#define INCLUDE_XGXS_16G
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_SCORPION_SUPPORT)
#define INCLUDE_SERDES_65LP
#define INCLUDE_XGXS_HL65
#define INCLUDE_XGXS_16G
#endif /* BCM_SCORPION_SUPPORT */

#if defined(BCM_KATANA_SUPPORT)
#define INCLUDE_XGXS_16G
#endif /* BCM_KATANA_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT)
#define INCLUDE_XGXS_16G
#define INCLUDE_XGXS_WCMOD
#define INCLUDE_XGXS_WC40    /*added for Helix 4 warpCore B0 */
#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT)
#define INCLUDE_XGXS_TSCMOD
#endif

#if defined(BCM_GREYHOUND_SUPPORT)
#if defined(PHYMOD_SUPPORT)
#define INCLUDE_XGXS_TSCE
#define INCLUDE_SERDES_QSGMIIE
#endif
#endif /* BCM_GREYHOUND_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
#if defined(PHYMOD_SUPPORT)
#define INCLUDE_XGXS_VIPER
#define INCLUDE_XGXS_TSCE
#define INCLUDE_XGXS_TSCE_DPLL
#define INCLUDE_SERDES_QSGMIIE
#define INCLUDE_XGXS_TSCF
#endif
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_HURRICANE3_SUPPORT)
#ifndef INCLUDE_PHY_EGPHY28
#define INCLUDE_PHY_EGPHY28
#endif
#if defined(PHYMOD_SUPPORT)
#define INCLUDE_SERDES_QTCE
#endif
#endif /* BCM_HURRICANE3_SUPPORT */

#if defined(BCM_53540_A0)
#ifndef INCLUDE_PHY_EGPHY28
#define INCLUDE_PHY_EGPHY28
#endif
#if defined(PHYMOD_SUPPORT)
#define INCLUDE_XGXS_VIPER
#endif
#endif /* BCM_53540_A0 (Wolfhound2) */

#if defined(BCM_HURRICANE4_SUPPORT)
#ifndef INCLUDE_PHY_EGPHY16
#define INCLUDE_PHY_EGPHY16
#endif
#endif /* BCM_HURRICANE4_SUPPORT */

#if defined(BCM_88202_A0)
#if defined(PHYMOD_SUPPORT)
#define INCLUDE_XGXS_TSCE
#endif
#endif /* BCM_88202_A0 */

#if defined(BCM_TOMAHAWK_SUPPORT)
#if defined(PHYMOD_SUPPORT)
#define INCLUDE_XGXS_TSCE
#define INCLUDE_XGXS_TSCF
#endif
#endif /* BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_SABER2_SUPPORT)
#if defined(PHYMOD_SUPPORT)
#define INCLUDE_XGXS_TSCE
#define INCLUDE_XGXS_VIPER
#define INCLUDE_SERDES_QSGMIIE
#endif
#endif /* BCM_SABER2_SUPPORT */


/* PHY/PHY dependencies */


#if defined(INCLUDE_PHY_5464) && defined(BCM_ESW_SUPPORT)
#define INCLUDE_PHY_5464_ESW
#endif /* BCM_ESW_SUPPORT */

#if defined(INCLUDE_PHY_5482) && defined(BCM_ESW_SUPPORT)
#define INCLUDE_PHY_5482_ESW
#endif /* BCM_ESW_SUPPORT */

/* definition for the SW cablediag supporting on phy54684 */
#if defined(INCLUDE_PHY_54684)
#define INCLUDE_PHY_54684_ESW
#endif /* INCLUDE_PHY_54684 */


#if defined(INCLUDE_PHY_XGXS1) || defined(INCLUDE_PHY_XGXS5) || \
    defined(INCLUDE_PHY_XGXS6)
#define INCLUDE_PHY_XGXS
#endif 

#if defined(INCLUDE_SERDES_100FX)  || defined(INCLUDE_SERDES_65LP) || \
    defined (INCLUDE_SERDES_COMBO) || defined(INCLUDE_PHY_56XXX) || \
    defined(INCLUDE_PHY_53XXX)
#define INCLUDE_SERDES
#endif

#if defined(INCLUDE_PHY_8703) || defined(INCLUDE_PHY_8705) || \
    defined(INCLUDE_PHY_8706) || defined(INCLUDE_PHY_8750) || \
    defined(INCLUDE_PHY_84334)|| defined(INCLUDE_PHY_8072) || \
    defined(INCLUDE_PHY_8481)
#define INCLUDE_PHY_XEHG
#endif
#endif /* _PHY_DEFS_H */
