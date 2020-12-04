/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phyconfig.h
 *
 * Purpose:     Set default configuration of the PHYs.
 */

#ifndef _PHY_CONFIG_H
#define _PHY_CONFIG_H

/************************/
/* Shared standard PHYs */
/************************/
#if defined(INCLUDE_SERDES)
#endif /* INCLUDE_SERDES */

#if defined(INCLUDE_PHY_XGXS)
#endif /* INCLUDE_PHY_XGXS */

#if defined(INCLUDE_PHY_FEGE)
#endif /* INCLUDE_PHY_FEGE */

#if defined(INCLUDE_PHY_XEHG) 
#endif /* INCLUDE_PHY_XEHG */


/*******************/
/* Internal SerDes */
/*******************/
#if defined(INCLUDE_PHY_5690)
#endif /* INCLUDE_PHY_5690 */

#if defined(INCLUDE_SERDES_ASSUMED)
#endif /* INCLUDE_SERDES_ASSUMED */

#if defined(INCLUDE_PHY_56XXX)
#endif /* INCLUDE_PHY_56XXX */

#if defined(INCLUDE_SERDES_100FX)
#endif /* INCLUDE_SERDES_100FX */

#if defined(INCLUDE_SERDES_65LP)
#endif /* INCLUDE_SERDES_65LP */

#if defined(INCLUDE_SERDES_COMBO)
#endif /* INCLUDE_SERDES_COMBO */

#if defined(INCLUDE_SERDES_COMBO65)
#endif /* INCLUDE_SERDES_COMBO65 */



/*********************/
/* Internal 10G PHYs */
/*********************/
#if defined(INCLUDE_PHY_XGXS1)
/* Workaround for Pass 0 Silicon */
#undef XGXS1_QUICK_LINK_FLAPPING_FIX
#endif /* INCLUDE_PHY_XGXS1 */

#if defined(INCLUDE_PHY_XGXS5)
#endif /* INCLUDE_PHY_XGXS5 */

#if defined(INCLUDE_PHY_XGXS6)
#endif /* INCLUDE_PHY_XGXS6 */

#if defined(INCLUDE_PHY_HL65)
#endif /* INCLUDE_PHY_HL65 */

/***********/
/* FE PHYs */
/***********/
#if defined(INCLUDE_PHY_522X)
#endif /* INCLUDE_PHY_522X */

/***********/
/* GE PHYs */
/***********/
#if defined(INCLUDE_PHY_5421S)
#endif /* INCLUDE_PHY_5421S */

#if defined(INCLUDE_PHY_54XX)
#endif /* INCLUDE_PHY_54XX */

#if defined(INCLUDE_PHY_5464_ESW)
#endif /* INCLUDE_PHY_5464_ESW */


#if defined(INCLUDE_PHY_5482)
#endif /* INCLUDE_PHY_5482 */


/*************/
/* 10GE PHYs */
/*************/
#if defined(INCLUDE_PHY_8703)
#endif /* INCLUDE_PHY_8703 */

#if defined(INCLUDE_PHY_8705)
#endif /* INCLUDE_PHY_8705 */

#if defined(INCLUDE_PHY_8706)
#endif /* INCLUDE_PHY_8706 */

#endif /* _PHY_CONFIG_H */
