/** \file soc/dev_macros.h
 * Slim SoC module to allow bcm actions.
 * 
 * This file contains soc macros for every device.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much. 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef SOC_DEV_MACROS_H_INCLUDE
#define SOC_DEV_MACROS_H_INCLUDE

#include <soc/drv_chip.h>
#include <soc/defs.h>
#include <soc/cm.h>

/* Not supported in this version of SDK */
#define SOC_IS_DRACO1(unit)     (0)
#define SOC_IS_DRACO15(unit)    (0)
#define SOC_IS_DRACO(unit)      (0)
#define SOC_IS_HERCULES1(unit)  (0)
#define SOC_IS_LYNX(unit)       (0)
#define SOC_IS_TUCANA(unit)     (0)
#define SOC_IS_XGS12_SWITCH(unit) (0)

#ifdef  BCM_HERCULES_SUPPORT
#ifdef  BCM_HERCULES15_SUPPORT
#define SOC_IS_HERCULES15(unit) ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_HERCULES15))
#else
#define SOC_IS_HERCULES15(unit) (0)
#endif
#define SOC_IS_HERCULES(unit)   ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_HERCULES))
#else
#define SOC_IS_HERCULES15(unit) (0)
#define SOC_IS_HERCULES(unit)   (0)
#endif

#ifdef  BCM_FIREBOLT_SUPPORT
#define SOC_IS_FIREBOLT(unit)   ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_FIREBOLT))
#define SOC_IS_FB(unit)         ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_FB))
#define SOC_IS_FB_FX_HX(unit)   ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_FB_FX_HX))
#define SOC_IS_HB_GW(unit)      ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_HB_GW))
#define SOC_IS_HBX(unit)        ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_HBX))
#define SOC_IS_FBX(unit)        ((!SOC_INFO(unit).spi_device) && \
    ((SOC_INFO(unit).chip & SOC_INFO_CHIP_FBX) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_KATANA2) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_HELIX4) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_HURRICANE2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITAN2PLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_APACHE) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MONTEREY) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE4) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWKPLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TRIDENT3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK2) ||\
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX5) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FIREBOLT6) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MAVERICK2)))
#else
#define SOC_IS_FIREBOLT(unit)   (0)
#define SOC_IS_FB(unit)         (0)
#define SOC_IS_FB_FX_HX(unit)   (0)
#define SOC_IS_HB_GW(unit)      (0)
#define SOC_IS_HBX(unit)        (0)
#define SOC_IS_FBX(unit)        (0)
#endif

#ifdef  BCM_HURRICANE2_SUPPORT
#define SOC_IS_HURRICANE2(unit)  ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_HURRICANE2))
#else
#define SOC_IS_HURRICANE2(unit)    (0)
#endif

#ifdef  BCM_GREYHOUND_SUPPORT
#define SOC_IS_GREYHOUND(unit)  ((!SOC_INFO(unit).spi_device) && \
        (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND))
#else
#define SOC_IS_GREYHOUND(unit)    (0)
#endif

#ifdef  BCM_HURRICANE3_SUPPORT
#define SOC_IS_HURRICANE3(unit)  ((!SOC_INFO(unit).spi_device) && \
        (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE3))
#else
#define SOC_IS_HURRICANE3(unit)    (0)
#endif

#ifdef  BCM_HURRICANE4_SUPPORT
#define SOC_IS_HURRICANE4(unit)  ((!SOC_INFO(unit).spi_device) && \
        (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE4))
#else
#define SOC_IS_HURRICANE4(unit)    (0)
#endif

#ifdef  BCM_TRIDENT3_SUPPORT
#define SOC_IS_TRIDENT3(unit)  ((!SOC_INFO(unit).spi_device) && \
        (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TRIDENT3))
#define SOC_IS_TRIDENT3X(unit)  ((!SOC_INFO(unit).spi_device) && \
        ((SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TRIDENT3) || \
         (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE4) || \
         (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX5) || \
         (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FIREBOLT6) || \
		 (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MAVERICK2)))
#else
#define SOC_IS_TRIDENT3(unit)    (0)
#define SOC_IS_TRIDENT3X(unit)   (0)
#endif
#ifdef  BCM_GREYHOUND2_SUPPORT
#define SOC_IS_GREYHOUND2(unit)  ((!SOC_INFO(unit).spi_device) && \
        (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND2))
#else
#define SOC_IS_GREYHOUND2(unit)    (0)
#endif

#ifdef BCM_FIRELIGHT_SUPPORT
#define SOC_IS_FIRELIGHT(unit)     (SOC_IS_GREYHOUND2(unit) && \
        soc_feature(unit, soc_feature_fl))
#else
#define SOC_IS_FIRELIGHT(unit)    (0)
#endif

#ifdef  BCM_HELIX5_SUPPORT
#define SOC_IS_HELIX5(unit)  ((!SOC_INFO(unit).spi_device) && \
        (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX5))

#define SOC_IS_HELIX5X(unit)  ((!SOC_INFO(unit).spi_device) && \
        ((SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX5) ||\
        (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE4) || \
        (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FIREBOLT6)))

#else
#define SOC_IS_HELIX5(unit)    (0)
#define SOC_IS_HELIX5X(unit)    (0)
#endif

#ifdef  BCM_MAVERICK2_SUPPORT
   #define SOC_IS_MAVERICK2(unit)  ((!SOC_INFO(unit).spi_device) && \
           (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MAVERICK2))
#else
   #define SOC_IS_MAVERICK2(unit)    (0)
#endif

#ifdef  BCM_FIREBOLT6_SUPPORT
   #define SOC_IS_FIREBOLT6(unit)  ((!SOC_INFO(unit).spi_device) && \
           (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FIREBOLT6))
#else
   #define SOC_IS_FIREBOLT6(unit)    (0)
#endif


#ifdef  BCM_HELIX_SUPPORT
#define SOC_IS_HELIX1(unit)     ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_FHX) && \
                (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FELIX))
#define SOC_IS_HELIX15(unit)    ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_FHX) && \
                (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX15))
#define SOC_IS_HELIX(unit)      ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_FHX) && \
                ((SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX) || \
                 (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX15)))
#else
#define SOC_IS_HELIX(unit)      (0)
#define SOC_IS_HELIX1(unit)     (0)
#define SOC_IS_HELIX15(unit)    (0)
#endif
#ifdef  BCM_FELIX_SUPPORT
#define SOC_IS_FELIX1(unit)     ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_FHX) && \
                (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FELIX))
#define SOC_IS_FELIX15(unit)    ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_FHX) && \
                (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FELIX15))
#define SOC_IS_FELIX(unit)      ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_FHX) && \
                ((SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FELIX) || \
                 (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FELIX15)))
#else
#define SOC_IS_FELIX(unit)      (0)
#define SOC_IS_FELIX1(unit)     (0)
#define SOC_IS_FELIX15(unit)    (0)
#endif
#ifdef  BCM_RAPTOR_SUPPORT
#define SOC_IS_RAPTOR(unit)     ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_RAPTOR))
#define SOC_IS_RAVEN(unit)      ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_RAVEN))
#define SOC_IS_HAWKEYE(unit)    ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_HAWKEYE))
#else
#define SOC_IS_RAPTOR(unit)     (0)
#define SOC_IS_RAVEN(unit)      (0)
#define SOC_IS_HAWKEYE(unit)    (0)
#endif
#if defined(BCM_HELIX_SUPPORT) || defined(BCM_FELIX_SUPPORT)
#define SOC_IS_FX_HX(unit)      ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_FX_HX))
#else
#define SOC_IS_FX_HX(unit)      (0)
#endif

#ifdef  BCM_GOLDWING_SUPPORT
#define SOC_IS_GOLDWING(unit)   ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_GOLDWING))
#else
#define SOC_IS_GOLDWING(unit)   (0)
#endif

#ifdef  BCM_HUMV_SUPPORT
#define SOC_IS_HUMV(unit)       ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_HUMV))
#else
#define SOC_IS_HUMV(unit)       (0)
#endif

#ifdef  BCM_BRADLEY_SUPPORT
#define SOC_IS_BRADLEY(unit)    ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_BRADLEY))
#else
#define SOC_IS_BRADLEY(unit)    (0)
#endif

#ifdef  BCM_FIREBOLT2_SUPPORT
#define SOC_IS_FIREBOLT2(unit)  ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_FIREBOLT2))
#else
#define SOC_IS_FIREBOLT2(unit)  (0)
#endif

#ifdef  BCM_TRIUMPH_SUPPORT
#define SOC_IS_TRIUMPH(unit)    ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_TRIUMPH))
#define SOC_IS_ENDURO(unit)    ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_ENDURO))
#define SOC_IS_HURRICANE(unit)  ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_HURRICANE))
#define SOC_IS_HURRICANEX(unit)   ((!SOC_INFO(unit).spi_device) && \
                ((SOC_INFO(unit).chip & SOC_INFO_CHIP_HURRICANE) || \
                 (SOC_INFO(unit).chip & SOC_INFO_CHIP_HURRICANE2) || \
                 (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE3)))
#define SOC_IS_TR_VL(unit)      ((!SOC_INFO(unit).spi_device) && \
    ((SOC_INFO(unit).chip & SOC_INFO_CHIP_TR_VL) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_KATANA2) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_HELIX4) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_HURRICANE2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITAN2PLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_APACHE) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MONTEREY) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE4) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWKPLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TRIDENT3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK2) ||\
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX5) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FIREBOLT6) || \
	 (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MAVERICK2)))
#else
#define SOC_IS_TRIUMPH(unit)    (0)
#define SOC_IS_ENDURO(unit)     (0)
#define SOC_IS_HURRICANE(unit)  (0)
#define SOC_IS_HURRICANEX(unit) (0)
#define SOC_IS_TR_VL(unit)      (0)
#endif

#ifdef  BCM_VALKYRIE_SUPPORT
#define SOC_IS_VALKYRIE(unit)   ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_VALKYRIE))
#else
#define SOC_IS_VALKYRIE(unit)   (0)
#endif

#ifdef  BCM_SCORPION_SUPPORT
#define SOC_IS_SCORPION(unit)   ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_SCORPION))
#define SOC_IS_SC_CQ(unit)      ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_SC_CQ))
#else
#define SOC_IS_SCORPION(unit)   (0)
#define SOC_IS_SC_CQ(unit)      (0)
#endif

#ifdef  BCM_CONQUEROR_SUPPORT
#define SOC_IS_CONQUEROR(unit)  ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_CONQUEROR))
#else
#define SOC_IS_CONQUEROR(unit)  (0)
#endif

#ifdef  BCM_TRIUMPH2_SUPPORT
#define SOC_IS_TRIUMPH2(unit)    ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_TRIUMPH2))
#else
#define SOC_IS_TRIUMPH2(unit)   (0)
#endif

#ifdef  BCM_APOLLO_SUPPORT
#define SOC_IS_APOLLO(unit)    ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_APOLLO))
#else
#define SOC_IS_APOLLO(unit)     (0)
#endif

#ifdef  BCM_VALKYRIE2_SUPPORT
#define SOC_IS_VALKYRIE2(unit)    ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_VALKYRIE2))
#else
#define SOC_IS_VALKYRIE2(unit)  (0)
#endif

#ifdef  BCM_TRIDENT_SUPPORT
#define SOC_IS_TRIDENT(unit)    ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_TRIDENT))
#define SOC_IS_TITAN(unit)      ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_TITAN))
#define SOC_IS_TD_TT(unit)      ((!SOC_INFO(unit).spi_device) && \
    ((SOC_INFO(unit).chip & SOC_INFO_CHIP_TD_TT) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITAN2PLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MONTEREY) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_APACHE) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWKPLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK2) || \
	 (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TRIDENT3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE4) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX5) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FIREBOLT6) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MAVERICK2)))
#else
#define SOC_IS_TRIDENT(unit)    (0)
#define SOC_IS_TITAN(unit)      (0)
#define SOC_IS_TD_TT(unit)      (0)
#endif

#ifdef  BCM_TRIUMPH3_SUPPORT
#define SOC_IS_TRIUMPH3(unit)  ((!SOC_INFO(unit).spi_device) && \
                ((SOC_INFO(unit).chip & SOC_INFO_CHIP_TRIUMPH3) || \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_HELIX4)))
#else
#define SOC_IS_TRIUMPH3(unit)   (0)
#endif

#ifdef  BCM_HELIX4_SUPPORT
#define SOC_IS_HELIX4(unit) (SOC_INFO(unit).chip & SOC_INFO_CHIP_HELIX4)
#else
#define SOC_IS_HELIX4(unit)     (0)
#endif

#ifdef  BCM_KATANA_SUPPORT
#define SOC_IS_KATANA(unit)    ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_KATANA))
#define SOC_IS_KATANAX(unit)   ((!SOC_INFO(unit).spi_device) && \
                    ((SOC_INFO(unit).chip & SOC_INFO_CHIP_KATANA) || \
                     (SOC_INFO(unit).chip & SOC_INFO_CHIP_KATANA2)))
#else
#define SOC_IS_KATANA(unit)     (0)
#define SOC_IS_KATANAX(unit)    (0)
#endif

#ifdef  BCM_KATANA2_SUPPORT
#define SOC_IS_KATANA2(unit)  ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_KATANA2))
#else
#define SOC_IS_KATANA2(unit)    (0)
#endif

#ifdef  BCM_SABER2_SUPPORT
#define SOC_IS_SABER2(unit)  ((!SOC_INFO(unit).spi_device) && \
                ((SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_SABER2) || \
                 (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_METROLITE)))
#else
#define SOC_IS_SABER2(unit)    (0)
#endif

#ifdef  BCM_METROLITE_SUPPORT
#define SOC_IS_METROLITE(unit)  ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_METROLITE))
#define SOC_PORT_PP_BITMAP(unit, port)   (SOC_INFO(unit).pp_port_bitmap[port])
#else
#define SOC_IS_METROLITE(unit)    (0)
#endif

#ifdef  BCM_TRIDENT2_SUPPORT
#define SOC_IS_TRIDENT2(unit)   ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_TRIDENT2))
#define SOC_IS_TITAN2(unit)     ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_TITAN2))
#define SOC_IS_TD2_TT2(unit)    ((!SOC_INFO(unit).spi_device) && \
    ((SOC_INFO(unit).chip & SOC_INFO_CHIP_TD2_TT2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITAN2PLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MONTEREY) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_APACHE) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWKPLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TRIDENT3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE4) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX5) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FIREBOLT6) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MAVERICK2)))
#define SOC_IS_TRIDENT2X(unit)  ((!SOC_INFO(unit).spi_device) && \
            ((SOC_INFO(unit).chip & SOC_INFO_CHIP_TRIDENT2X) || \
            (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MONTEREY) || \
             (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_APACHE)))
#define SOC_IS_TITAN2X(unit)    ((!SOC_INFO(unit).spi_device) && \
        ((SOC_INFO(unit).chip & SOC_INFO_CHIP_TITAN2) || \
         (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITAN2PLUS)))
#else
#define SOC_IS_TRIDENT2(unit)   (0)
#define SOC_IS_TITAN2(unit)     (0)
#define SOC_IS_TD2_TT2(unit)    (0)
#define SOC_IS_TRIDENT2X(unit)  (0)
#define SOC_IS_TITAN2X(unit)    (0)
#endif

#ifdef BCM_TRIDENT2PLUS_SUPPORT
#define SOC_IS_TRIDENT2PLUS(unit)   ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_TRIDENT2PLUS))
#define SOC_IS_TITAN2PLUS(unit)     ((!SOC_INFO(unit).spi_device) && \
        (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITAN2PLUS))
#define SOC_IS_TD2P_TT2P(unit)      ((!SOC_INFO(unit).spi_device) && \
        ((SOC_INFO(unit).chip & SOC_INFO_CHIP_TRIDENT2PLUS) || \
         (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITAN2PLUS)))
#else
#define SOC_IS_TRIDENT2PLUS(unit)   (0)
#define SOC_IS_TITAN2PLUS(unit)     (0)
#define SOC_IS_TD2P_TT2P(unit)      (0)
#endif

#ifdef  BCM_TOMAHAWK_SUPPORT
#define SOC_IS_TOMAHAWK(unit)   ((!SOC_INFO(unit).spi_device) && \
                ((SOC_INFO(unit).chip & SOC_INFO_CHIP_TOMAHAWK) || \
                 (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWKPLUS)))
#define SOC_IS_TOMAHAWKX(unit)   ((!SOC_INFO(unit).spi_device) && \
                ((SOC_INFO(unit).chip & SOC_INFO_CHIP_TOMAHAWK) || \
                 (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWKPLUS) || \
                 (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK2) || \
                 (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK2) || \
                 (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK3)))
#else
#define SOC_IS_TOMAHAWK(unit)   (0)
#define SOC_IS_TOMAHAWKX(unit)  (0)
#endif

#ifdef  BCM_TOMAHAWKPLUS_SUPPORT
#define SOC_IS_TOMAHAWKPLUS(unit)   ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWKPLUS))
#else
#define SOC_IS_TOMAHAWKPLUS(unit)   (0)
#endif

#ifdef  BCM_TOMAHAWK2_SUPPORT
#define SOC_IS_TOMAHAWK2(unit)   ((!SOC_INFO(unit).spi_device) && \
                ((SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK2) || \
                 (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK2)))
#else
#define SOC_IS_TOMAHAWK2(unit)   (0)
#endif

#ifdef  BCM_TOMAHAWK3_SUPPORT
#define SOC_IS_TOMAHAWK3(unit)   ((!SOC_INFO(unit).spi_device) && \
                ((SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK3)))
#else
#define SOC_IS_TOMAHAWK3(unit)   (0)
#endif

#ifdef BCM_APACHE_SUPPORT
#define SOC_IS_APACHE(unit)  ((!SOC_INFO(unit).spi_device) && \
                ((SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_APACHE) || \
                (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MONTEREY)))
#define SOC_IS_MONTEREY(unit)  ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MONTEREY))
#else
#define SOC_IS_APACHE(unit)    (0)
#define SOC_IS_MONTEREY(unit)    (0)
#endif

#ifdef BCM_TRX_SUPPORT
#define SOC_IS_TRX(unit)    ((!SOC_INFO(unit).spi_device) && \
    ((SOC_INFO(unit).chip & SOC_INFO_CHIP_TRX) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_KATANA2) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_HELIX4) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_HURRICANE2)||\
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITAN2PLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_APACHE) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MONTEREY) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWKPLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TRIDENT3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK2) ||\
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE4) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX5) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FIREBOLT6) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MAVERICK2)))
#else
#define SOC_IS_TRX(unit)        (0)
#endif

#ifdef  BCM_XGS_SUPPORT
#define SOC_IS_XGS(unit)    ((!SOC_INFO(unit).spi_device) && \
    ((SOC_INFO(unit).chip & SOC_INFO_CHIP_XGS) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_KATANA2) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_HELIX4) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_HURRICANE2) ||\
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITAN2PLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_APACHE) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MONTEREY) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE4) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK2) ||\
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK2) ||\
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TRIDENT3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX5) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FIREBOLT6) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MAVERICK2)))
#else
#define SOC_IS_XGS(unit)        (0)
#endif
#ifdef  BCM_XGS_FABRIC_SUPPORT
#define SOC_IS_XGS_FABRIC(unit) ((!SOC_INFO(unit).spi_device) && \
        ((SOC_INFO(unit).chip & SOC_INFO_CHIP_XGS_FABRIC) || \
         (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITAN2PLUS) ||\
         (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK2)))
#define SOC_IS_XGS_FABRIC_TITAN(unit) ((!SOC_INFO(unit).spi_device) && \
        ((SOC_INFO(unit).chip & SOC_INFO_CHIP_XGS_FABRIC) || \
         (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK) ||\
         (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK2)))
#else
#define SOC_IS_XGS_FABRIC(unit) (0)
#define SOC_IS_XGS_FABRIC_TITAN(unit) (0)
#endif
#ifdef  BCM_XGS_SWITCH_SUPPORT
#define SOC_IS_XGS_SWITCH(unit) ((!SOC_INFO(unit).spi_device) && \
    ((SOC_INFO(unit).chip & SOC_INFO_CHIP_XGS_SWITCH) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_KATANA2) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_HELIX4) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_HURRICANE2)||\
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITAN2PLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_APACHE) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MONTEREY) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE4) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TRIDENT3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK2) ||\
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK2) ||\
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX5) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FIREBOLT6) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MAVERICK2)))
#else
#define SOC_IS_XGS_SWITCH(unit) (0)
#endif
#ifdef  BCM_XGS12_FABRIC_SUPPORT
#define SOC_IS_XGS12_FABRIC(unit)   ((!SOC_INFO(unit).spi_device) && \
                (SOC_INFO(unit).chip & SOC_INFO_CHIP_XGS12_FABRIC))
#else
#define SOC_IS_XGS12_FABRIC(unit) (0)
#endif
#ifdef  BCM_XGS3_SWITCH_SUPPORT
#define SOC_IS_XGS3_SWITCH(unit)    ((!SOC_INFO(unit).spi_device) && \
    ((SOC_INFO(unit).chip & SOC_INFO_CHIP_XGS3_SWITCH) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_KATANA2) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_HELIX4) || \
     (SOC_INFO(unit).chip & SOC_INFO_CHIP_HURRICANE2)|| \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITAN2PLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_APACHE) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MONTEREY) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE4) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWKPLUS) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TRIDENT3) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK2) ||\
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK2) ||\
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TOMAHAWK3) ||\
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_GREYHOUND2) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HELIX5) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_FIREBOLT6) || \
     (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_MAVERICK2)))
#else
#define SOC_IS_XGS3_SWITCH(unit) (0)
#endif
#ifdef  BCM_XGS3_FABRIC_SUPPORT
#define SOC_IS_XGS3_FABRIC(unit)  ((!SOC_INFO(unit).spi_device) && \
        ((SOC_INFO(unit).chip & SOC_INFO_CHIP_XGS3_FABRIC) || \
         (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITAN2PLUS) ||\
         (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_TITANHAWK2)))

#else
#define SOC_IS_XGS3_FABRIC(unit) (0)
#endif

#ifdef BCM_PETRA_SUPPORT
#define SOC_IS_ARADPLUS(unit)     (CMDEV(unit).dev.chip_enum >= SOC_INFO_CHIP_TYPE_ARADPLUS && CMDEV(unit).dev.chip_enum <= SOC_INFO_CHIP_TYPE_DNX1_LAST)
#define SOC_IS_ARAD(unit)         (SOC_IS_ARADPLUS(unit)) /* SOC_IS_ARADPLUS already
                                                                                                                includes all chips above it. */
#define SOC_IS_ARADPLUS_A0(unit)       (CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_ARADPLUS)
#define SOC_IS_ARAD_B0_AND_ABOVE(unit)  (SOC_IS_ARAD(unit)  )
#define SOC_IS_ARAD_B1_AND_BELOW(unit)  (0)
#define SOC_IS_ARADPLUS_AND_BELOW(unit) (CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_ARADPLUS)
#else
#define SOC_IS_ARAD(unit)               (0)
#define SOC_IS_ARADPLUS(unit)           (0)
#define SOC_IS_ARAD_A0(unit)            (0)
#define SOC_IS_ARAD_B0(unit)            (0)
#define SOC_IS_ARAD_B1(unit)            (0)
#define SOC_IS_ARADPLUS_A0(unit)        (0)
#define SOC_IS_ARAD_B0_AND_ABOVE(unit)  (0)
#define SOC_IS_ARAD_B1_AND_BELOW(unit)  (0)
#define SOC_IS_ARADPLUS_AND_BELOW(unit) (0)
#endif

#ifdef BCM_JERICHO_SUPPORT
#define SOC_IS_QMX(unit)          (CMDEV(unit).dev.chip_enum   == SOC_INFO_CHIP_TYPE_QMX)
#define SOC_IS_JERICHO(unit)      (CMDEV(unit).dev.chip_enum  >= SOC_INFO_CHIP_TYPE_JERICHO_FIRST && CMDEV(unit).dev.chip_enum  <= SOC_INFO_CHIP_TYPE_DNX1_LAST)
#define SOC_IS_QMX_A0(unit)       ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_QMX) && CMDEV(unit).dev.rev_id < QMX_B0_REV_ID)
#define SOC_IS_QMX_B0(unit)       ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_QMX) && CMDEV(unit).dev.rev_id >= QMX_B0_REV_ID)
#define SOC_IS_JERICHO_A0(unit)   ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_JERICHO) && CMDEV(unit).dev.rev_id < JERICHO_B0_REV_ID)
#define SOC_IS_JERICHO_B0(unit)       ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_JERICHO) && CMDEV(unit).dev.rev_id >= JERICHO_B0_REV_ID)
#define SOC_IS_JERICHO_AND_BELOW(unit)  (CMDEV(unit).dev.chip_enum  >= SOC_INFO_CHIP_TYPE_ARADPLUS && CMDEV(unit).dev.chip_enum  <= SOC_INFO_CHIP_TYPE_QMX)
#define SOC_IS_JERICHO_B0_AND_ABOVE(unit) ((CMDEV(unit).dev.chip_enum  >= SOC_INFO_CHIP_TYPE_JERICHO && CMDEV(unit).dev.chip_enum  <= SOC_INFO_CHIP_TYPE_QMX && CMDEV(unit).dev.rev_id >= JERICHO_B0_REV_ID) || (CMDEV(unit).dev.chip_enum  >= SOC_INFO_CHIP_TYPE_JERICHO_PLUS && CMDEV(unit).dev.chip_enum  <= SOC_INFO_CHIP_TYPE_DNX1_LAST))

#else
#define SOC_IS_QMX(unit)          (0)
#define SOC_IS_JERICHO(unit)      (0)
#define SOC_IS_QMX_A0(unit)       (0)
#define SOC_IS_QMX_B0(unit)       (0)
#define SOC_IS_JERICHO_A0(unit)   (0)
#define SOC_IS_JERICHO_B0(unit)       (0)
#define SOC_IS_JERICHO_AND_BELOW(unit)  (SOC_IS_ARAD(unit) && !SOC_IS_QAX(unit) && !SOC_IS_JERICHO_PLUS(unit))
#define SOC_IS_JERICHO_B0_AND_ABOVE(unit) (0)
#endif

#ifdef BCM_QAX_SUPPORT
#define SOC_IS_QAX(unit)          (CMDEV(unit).dev.chip_enum >= SOC_INFO_CHIP_TYPE_QAX_FIRST && CMDEV(unit).dev.chip_enum <= SOC_INFO_CHIP_TYPE_QAX_LAST)
#define SOC_IS_QAX_A0(unit)       (((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_QAX) || (CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_KALIA)) && (CMDEV(unit).dev.rev_id < QAX_B0_REV_ID))
#define SOC_IS_QAX_B0(unit)       (((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_QAX) || (CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_KALIA)) && (CMDEV(unit).dev.rev_id >= QAX_B0_REV_ID))
#define SOC_IS_KALIA(unit)        (CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_KALIA)
#define SOC_IS_QAX_WITH_FABRIC_ENABLED(unit) (SOC_IS_KALIA(unit) || (SOC_IS_QAX(unit) && SOC_DPP_IS_MESH(unit)) || soc_feature(unit, soc_feature_packet_tdm_marking))
#else
#define SOC_IS_QAX(unit)          (0)
#define SOC_IS_QAX_A0(unit)       (0)
#define SOC_IS_QAX_B0(unit)       (0)
#define SOC_IS_KALIA(unit)          (0)
#define SOC_IS_QAX_WITH_FABRIC_ENABLED(unit)    (0)
#endif


#ifdef BCM_QUX_SUPPORT
#define SOC_IS_QUX(unit)          ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_QUX))
#define SOC_IS_QUX_A0(unit)       ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_QUX) && (CMDEV(unit).dev.rev_id >= QUX_A0_REV_ID))
#else
#define SOC_IS_QUX(unit)          (0)
#define SOC_IS_QUX_A0(unit)       (0)
#endif

#ifdef BCM_JERICHO_PLUS_SUPPORT
#define SOC_IS_JERICHO_PLUS(unit)          ((CMDEV(unit).dev.chip_enum >= SOC_INFO_CHIP_TYPE_JERICHO_PLUS) && (CMDEV(unit).dev.chip_enum <= SOC_INFO_CHIP_TYPE_QAX_LAST))
#define SOC_IS_JERICHO_PLUS_A0(unit)       ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_JERICHO_PLUS))
#define SOC_IS_JERICHO_PLUS_AND_BELOW(unit)  ((CMDEV(unit).dev.chip_enum >= SOC_INFO_CHIP_TYPE_ARADPLUS) && (CMDEV(unit).dev.chip_enum <= SOC_INFO_CHIP_TYPE_JERICHO_LAST))
#define SOC_IS_JERICHO_PLUS_ONLY(unit)       (CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_JERICHO_PLUS)
#else
#define SOC_IS_JERICHO_PLUS(unit)          (0)
#define SOC_IS_JERICHO_PLUS_A0(unit)       (0)
#define SOC_IS_JERICHO_PLUS_AND_BELOW(unit) (SOC_IS_ARAD(unit) && !SOC_IS_QAX(unit))
#define SOC_IS_JERICHO_PLUS_ONLY(unit)  (0)
#endif

#ifdef BCM_DNX_SUPPORT
#define SOC_IS_DNX(unit)                ((CMDEV(unit).dev.chip_enum >= SOC_INFO_CHIP_TYPE_DNX2_FIRST) && (CMDEV(unit).dev.chip_enum <= SOC_INFO_CHIP_TYPE_DNX2_LAST))
#define SOC_IS_DNX_TYPE(dev_type)       (SOC_IS_DNX2_TYPE(dev_type))
#define SOC_IS_DNX2(unit)               ((CMDEV(unit).dev.chip_enum >= SOC_INFO_CHIP_TYPE_DNX2_FIRST) && (CMDEV(unit).dev.chip_enum <= SOC_INFO_CHIP_TYPE_DNX2_LAST))
#define SOC_IS_DNX2_TYPE(dev_type)      (SOC_IS_JERICHO2_TYPE(dev_type) || SOC_IS_J2C_TYPE(dev_type) || SOC_IS_J2P_TYPE(dev_type) || SOC_IS_J2X_TYPE(dev_type) || SOC_IS_Q2A_TYPE(dev_type))
#define SOC_IS_JERICHO2_A0(unit)       ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_JERICHO2) && CMDEV(unit).dev.rev_id == BCM88690_A0_REV_ID)
#define SOC_IS_JERICHO2_B(unit)        ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_JERICHO2) && CMDEV(unit).dev.rev_id >= BCM88690_B0_REV_ID)
#define SOC_IS_JERICHO2_B1(unit)       ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_JERICHO2) && CMDEV(unit).dev.rev_id >= BCM88690_B1_REV_ID)
#define SOC_IS_JERICHO2_B0(unit)       ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_JERICHO2) && CMDEV(unit).dev.rev_id == BCM88690_B0_REV_ID)
#define SOC_IS_JERICHO2(unit)          (CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_JERICHO2)
/* SKU Support*/
/* Returns TRUE for all devices with DEV_ID from 0x8690 to 0x869F*/
#define SOC_IS_JERICHO2_TYPE(dev_type) (((dev_type) & DNXC_DEVID_FAMILY_MASK) == (BCM88690_DEVICE_ID & DNXC_DEVID_FAMILY_MASK))
#define SOC_IS_J2C(unit)                (CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_J2C)
#define SOC_IS_J2C_A(unit)              ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_J2C) && CMDEV(unit).dev.rev_id >= BCM88800_A0_REV_ID)
#define SOC_IS_J2C_A0(unit)             ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_J2C) && CMDEV(unit).dev.rev_id == BCM88800_A0_REV_ID)
#define SOC_IS_J2C_A1(unit)             ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_J2C) && CMDEV(unit).dev.rev_id >= BCM88800_A1_REV_ID)
#define SOC_IS_J2C_TYPE(dev_type)       (((dev_type) & J2C_DEVID_FAMILY_MASK) == J2C_DEVICE_ID)

#define SOC_IS_J2P(unit)                (CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_J2P)
#define SOC_IS_J2P_A0(unit)             ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_J2P) && CMDEV(unit).dev.rev_id == BCM88850_A0_REV_ID)
#define SOC_IS_J2P_A1(unit)             ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_J2P) && CMDEV(unit).dev.rev_id == BCM88850_A1_REV_ID)
#define SOC_IS_J2P_A2(unit)             ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_J2P) && CMDEV(unit).dev.rev_id >= BCM88850_A2_REV_ID)
#define SOC_IS_J2P_TYPE(dev_type)       ((((dev_type) & DNXC_DEVID_FAMILY_MASK) == BCM88850_DEVICE_ID) || (((dev_type) & DNXC_DEVID_FAMILY_MASK) == BCM88840_DEVICE_ID))
#define SOC_IS_J2X(unit)                (CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_J2X)
#define SOC_IS_J2X_TYPE(dev_type)       (((dev_type) & DNXC_DEVID_FAMILY_MASK) == BCM88830_DEVICE_ID)
#define SOC_IS_Q2A(unit)                (CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_Q2A)
#define SOC_IS_Q2A_A0(unit)             ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_Q2A) && CMDEV(unit).dev.rev_id == BCM88480_A0_REV_ID)
#define SOC_IS_Q2A_TYPE(dev_type)       ((((dev_type) & DNXC_DEVID_FAMILY_MASK) == Q2A_DEVICE_ID) || (((dev_type) & DNXC_DEVID_FAMILY_MASK) == Q2U_DEVICE_ID) || (((dev_type) & DNXC_DEVID_FAMILY_MASK) == Q2N_DEVICE_ID))
#define SOC_IS_Q2A_B(unit)             ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_Q2A) && CMDEV(unit).dev.rev_id >= BCM88480_B0_REV_ID)
#define SOC_IS_Q2A_B0(unit)             ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_Q2A) && CMDEV(unit).dev.rev_id == BCM88480_B0_REV_ID)
#define SOC_IS_Q2A_B1(unit)             ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_Q2A) && CMDEV(unit).dev.rev_id >= BCM88480_B1_REV_ID)
#define SOC_IS_Q2N(unit)                ((CMDEV(unit).dev.dev_id & DNXC_DEVID_FAMILY_MASK) == Q2N_DEVICE_ID)

#else
#define SOC_IS_DNX(unit)                (0)
#define SOC_IS_DNX_TYPE(dev_type)       (0)
#define SOC_IS_DNX2(unit)               (0)
#define SOC_IS_DNX2_TYPE(dev_type)      (0)
#define SOC_IS_JERICHO2_A0(unit)       (0)
#define SOC_IS_JERICHO2_B(unit)        (0)
#define SOC_IS_JERICHO2_B1(unit)       (0)
#define SOC_IS_JERICHO2_B0(unit)       (0)
#define SOC_IS_JERICHO2(unit)          (0)
#define SOC_IS_JERICHO2_TYPE(dev_type) (0)
#define SOC_IS_J2C(unit)                (0)
#define SOC_IS_J2C_A(unit)              (0)
#define SOC_IS_J2C_A0(unit)             (0)
#define SOC_IS_J2C_A1(unit)             (0)
#define SOC_IS_J2C_TYPE(unit)           (0)
#define SOC_IS_J2P(unit)                (0)
#define SOC_IS_J2P_A0(unit)             (0)
#define SOC_IS_J2P_A1(unit)             (0)
#define SOC_IS_J2P_A2(unit)             (0)
#define SOC_IS_J2P_TYPE(unit)           (0)
#define SOC_IS_J2X(unit)                (0)
#define SOC_IS_J2X_TYPE(unit)           (0)
#define SOC_IS_Q2A(unit)                (0)
#define SOC_IS_Q2A_A0(unit)             (0)
#define SOC_IS_Q2A_TYPE(dev_type)       (0)
#define SOC_IS_Q2A_B(unit)             (0)
#define SOC_IS_Q2A_B0(unit)             (0)
#define SOC_IS_Q2A_B1(unit)             (0)
#define SOC_IS_Q2N(unit)                0
#endif

#ifdef BCM_DFE_SUPPORT
#define SOC_IS_BCM88773(unit) (CMDEV(unit).dev.dev_id == BCM88773_DEVICE_ID || CMDEV(unit).dev.dev_id == BCM88953_DEVICE_ID)
#define SOC_IS_BCM88773_A1(unit) ((CMDEV(unit).dev.dev_id == BCM88773_DEVICE_ID || CMDEV(unit).dev.dev_id == BCM88953_DEVICE_ID) && CMDEV(unit).dev.rev_id == BCM88773_A1_REV_ID)
#define SOC_IS_BCM88774(unit) (CMDEV(unit).dev.dev_id == BCM88774_DEVICE_ID || CMDEV(unit).dev.dev_id == BCM88954_DEVICE_ID)
#define SOC_IS_BCM88774_A1(unit) ((CMDEV(unit).dev.dev_id == BCM88774_DEVICE_ID || CMDEV(unit).dev.dev_id == BCM88954_DEVICE_ID) && CMDEV(unit).dev.rev_id == BCM88774_A1_REV_ID)
#define SOC_IS_BCM88775(unit) (CMDEV(unit).dev.dev_id == BCM88775_DEVICE_ID || CMDEV(unit).dev.dev_id == BCM88955_DEVICE_ID)
#define SOC_IS_BCM88775_A1(unit) ((CMDEV(unit).dev.dev_id == BCM88775_DEVICE_ID || CMDEV(unit).dev.dev_id == BCM88955_DEVICE_ID) && CMDEV(unit).dev.rev_id == BCM88775_A1_REV_ID)
#define SOC_IS_BCM88776(unit) (CMDEV(unit).dev.dev_id == BCM88776_DEVICE_ID || CMDEV(unit).dev.dev_id == BCM88956_DEVICE_ID)
#define SOC_IS_BCM88776_A1(unit) ((CMDEV(unit).dev.dev_id == BCM88776_DEVICE_ID || CMDEV(unit).dev.dev_id == BCM88956_DEVICE_ID) && CMDEV(unit).dev.rev_id == BCM88776_A1_REV_ID)
#define SOC_IS_FE3200(unit) (CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_FE3200)
#define SOC_IS_FE3200_A0(unit) ((CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_FE3200) && CMDEV(unit).dev.rev_id == BCM88950_A0_REV_ID)
#define SOC_IS_DFE_TYPE(dev_type) ( (dev_type) == BCM88772_DEVICE_ID || (dev_type) == BCM88952_DEVICE_ID || (dev_type) == BCM88770_DEVICE_ID || (dev_type) == BCM88773_DEVICE_ID || (dev_type) == BCM88774_DEVICE_ID || (dev_type) == BCM88775_DEVICE_ID || (dev_type) == BCM88776_DEVICE_ID || (dev_type) == BCM88777_DEVICE_ID || (dev_type) == BCM88950_DEVICE_ID || (dev_type) == BCM88953_DEVICE_ID || (dev_type) == BCM88954_DEVICE_ID || (dev_type) == BCM88955_DEVICE_ID || (dev_type) == BCM88956_DEVICE_ID)

#else /*!BCM_DFE_SUPPORT*/
#define SOC_IS_FE3200(unit)             (0)
#define SOC_IS_FE3200_A0(unit)          (0)
#define SOC_IS_DFE_TYPE(dev_type)       (0)
#endif /*BCM_DFE_SUPPORT*/

#ifdef BCM_DNXF_SUPPORT
#define SOC_IS_RAMON(unit) (CMDEV(unit).dev.chip_enum == SOC_INFO_CHIP_TYPE_RAMON)
#define SOC_IS_RAMON_A0(unit) (SOC_IS_RAMON(unit) && CMDEV(unit).dev.rev_id == BCM88790_A0_REV_ID)
#define SOC_IS_RAMON_A1(unit) (SOC_IS_RAMON(unit) && CMDEV(unit).dev.rev_id == DNXC_A1_REV_ID)
#define SOC_IS_RAMON_B0(unit) (SOC_IS_RAMON(unit) && CMDEV(unit).dev.rev_id >= BCM88790_B0_REV_ID)
/* SKU Support*/
/* Returns TRUE for all devices with DEV_ID from 0x8790 to 0x879F*/
#define SOC_IS_RAMON_TYPE(dev_type) (((dev_type) & DNXC_DEVID_FAMILY_MASK) == BCM88790_DEVICE_ID)

#define SOC_IS_DNXF(unit)           (SOC_IS_RAMON(unit))
#define SOC_IS_DNXF_TYPE(dev_type)  (SOC_IS_RAMON_TYPE(dev_type))
#else
#define SOC_IS_DNXF(unit)           (0)
#define SOC_IS_DNXF_TYPE(dev_type)  (0)
#define SOC_IS_RAMON(unit)          (0)
#define SOC_IS_RAMON_A0(unit)       (0)
#define SOC_IS_RAMON_A1(unit)       (0)
#define SOC_IS_RAMON_B0(unit)       (0)
#endif /*BCM_DNXF_SUPPORT*/

#define SOC_IS_DPP(unit) (SOC_IS_ARAD(unit) || SOC_IS_QAX(unit)  || SOC_IS_JERICHO_PLUS(unit))
#define SOC_IS_DFE(unit) (SOC_IS_FE3200(unit))
#define SOC_IS_SAND(unit) (SOC_IS_DPP(unit) || SOC_IS_DFE(unit) || SOC_IS_DNX(unit) || SOC_IS_DNXF(unit))

#define SOC_IS_DPP_DRC_COMBO28(unit) (SOC_IS_JERICHO(unit))


#if defined(BCM_LTSW_SUPPORT) || defined(BCM_CTSW_SUPPORT)
#define SOC_IS_LTSW(unit) \
        ((SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_LT_SWITCHES))
#else
#define SOC_IS_LTSW(unit) (0)
#endif

#define SOC_IS_ESW(unit) (!SOC_IS_SAND(unit) && !SOC_IS_LTSW(unit))


#endif
