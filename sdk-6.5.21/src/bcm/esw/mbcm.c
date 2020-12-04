/*

 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mbcm.c
 * Purpose:     Implementation of bcm multiplexing
 */


#include <soc/defs.h>
#include <shared/bsl.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw_dispatch.h>

#include <bcm_int/common/family.h>

mbcm_functions_t	*mbcm_driver[BCM_MAX_NUM_UNITS];

#if defined(BCM_TOMAHAWK_SUPPORT)
static mbcm_functions_t *mbcm_l2_latency = NULL;
static mbcm_functions_t *mbcm_l3_latency = NULL;
static mbcm_functions_t *mbcm_normal_latency = NULL;
static uint32 mbcm_ref[3] = {0}; /* ref cnt */

STATIC int
_bcm_esw_mbcm_init(int unit, mbcm_functions_t **mbcm);
#endif


/****************************************************************
 *
 * Function:        mbcm_init
 * Parameters:      unit   --   unit to setup
 * 
 * Initialize the mbcm driver for the indicated unit.
 */
int
mbcm_init(int unit)
{
#ifdef	BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mbcm_driver[unit] = &mbcm_hercules_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_HERCULES); /* Should become H15 */
	return BCM_E_NONE;
    }
#endif	/* BCM_HERCULES15_SUPPORT */
#ifdef	BCM_ALLLAYER_SUPPORT
    if (SOC_IS_ALLLAYER(unit)) {
        mbcm_driver[unit] = &mbcm_alllayer_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_ALLLAYER);         
        return BCM_E_NONE;
    }
#endif	/* BCM_ALLLAYER_SUPPORT */
#ifdef	BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FB_FX_HX(unit)) {
        mbcm_driver[unit] = &mbcm_firebolt_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_FIREBOLT); 
	return BCM_E_NONE;
    }
#endif	/* BCM_FIREBOLT_SUPPORT */
#ifdef	BCM_HB_GW_SUPPORT
    if (SOC_IS_BRADLEY(unit)) {
        mbcm_driver[unit] = &mbcm_bradley_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_BRADLEY); 
	return BCM_E_NONE;
    }
#endif	/* BCM_BRADLEY_SUPPORT */
#ifdef	BCM_GOLDWING_SUPPORT
    if (SOC_IS_GOLDWING(unit)) {
        mbcm_driver[unit] = &mbcm_bradley_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_BRADLEY); 
        return BCM_E_NONE;
    }
#endif	/* BCM_GOLDWING_SUPPORT */
#ifdef	BCM_HUMV_SUPPORT
    if (SOC_IS_HUMV(unit)) {
        mbcm_driver[unit] = &mbcm_humv_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_HUMV); 
        return BCM_E_NONE;
    }
#endif	/* BCM_HUMV_SUPPORT */
#ifdef	BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH(unit)) {
        mbcm_driver[unit] = &mbcm_triumph_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIUMPH); 
        return BCM_E_NONE;
    }
#endif	/* BCM_TRIUMPH_SUPPORT */
#ifdef	BCM_VALKYRIE_SUPPORT
    if (SOC_IS_VALKYRIE(unit)) {
        mbcm_driver[unit] = &mbcm_triumph_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIUMPH); 
        return BCM_E_NONE;
    }
#endif	/* BCM_VALKYRIE_SUPPORT */
#ifdef	BCM_SCORPION_SUPPORT
    if (SOC_IS_SCORPION(unit)) {
        mbcm_driver[unit] = &mbcm_scorpion_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_SCORPION); 
	return BCM_E_NONE;
    }
#endif	/* BCM_SCORPION_SUPPORT */
#ifdef	BCM_CONQUEROR_SUPPORT
    if (SOC_IS_CONQUEROR(unit)) {
        mbcm_driver[unit] = &mbcm_conqueror_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_CONQUEROR); 
	return BCM_E_NONE;
    }
#endif	/* BCM_CONQUEROR_SUPPORT */
#ifdef	BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit)) {
        mbcm_driver[unit] = &mbcm_triumph2_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIUMPH2); 
        return BCM_E_NONE;
    }
#endif	/* BCM_TRIUMPH2_SUPPORT */
#ifdef	BCM_APOLLO_SUPPORT
    if (SOC_IS_APOLLO(unit)) {
        mbcm_driver[unit] = &mbcm_triumph2_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIUMPH2); 
        return BCM_E_NONE;
    }
#endif	/* BCM_APOLLO_SUPPORT */
#ifdef	BCM_VALKYRIE2_SUPPORT
    if (SOC_IS_VALKYRIE2(unit)) {
        mbcm_driver[unit] = &mbcm_triumph2_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIUMPH2); 
        return BCM_E_NONE;
    }
#endif	/* BCM_VALKYRIE2_SUPPORT */
#ifdef	BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit)) {
        mbcm_driver[unit] = &mbcm_enduro_driver;
        
        bcm_chip_family_set(unit, BCM_FAMILY_TRIUMPH); 
        return BCM_E_NONE;
    }
#endif	/* BCM_ENDURO_SUPPORT */
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        mbcm_driver[unit] = &mbcm_conqueror_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_CONQUEROR);
       return BCM_E_NONE;
    }
#endif /* BCM_SHADOW_SUPPORT */
#ifdef	BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
        mbcm_driver[unit] = &mbcm_trident_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
    if (SOC_IS_TITAN(unit)) {
        mbcm_driver[unit] = &mbcm_titan_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TITAN);
        return BCM_E_NONE;
    }
#endif	/* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (SOC_IS_TRIDENT2PLUS(unit)) {
        mbcm_driver[unit] = &mbcm_trident2plus_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
    if (SOC_IS_TITAN2PLUS(unit)) {
        mbcm_driver[unit] = &mbcm_titan2plus_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TITAN);
        return BCM_E_NONE;
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY (unit))
    {
        mbcm_driver[unit] = &mbcm_monterey_driver;
        bcm_chip_family_set (unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE (unit))
    {
        mbcm_driver[unit] = &mbcm_apache_driver;
        bcm_chip_family_set (unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT 
    if (SOC_IS_TRIDENT2(unit)) {
        mbcm_driver[unit] = &mbcm_trident2_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
    if (SOC_IS_TITAN2(unit)) {
        mbcm_driver[unit] = &mbcm_titan2_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TITAN);
        return BCM_E_NONE;
    }
#endif	/* BCM_TRIDENT2_SUPPORT */
#ifdef	BCM_HURRICANE1_SUPPORT
    if (SOC_IS_HURRICANE(unit)) {
        mbcm_driver[unit] = &mbcm_hurricane_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIUMPH); 
        return BCM_E_NONE;
    }
#endif	/* BCM_HURRICANE1_SUPPORT */
#ifdef	BCM_HURRICANE_SUPPORT
    if (SOC_IS_HURRICANE2(unit)) {
        mbcm_driver[unit] = &mbcm_hurricane2_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIUMPH);
        return BCM_E_NONE;
    }
#endif	/* BCM_HURRICANE_SUPPORT */
#ifdef  BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        mbcm_driver[unit] = &mbcm_triumph3_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef	BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        mbcm_driver[unit] = &mbcm_katana_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
#endif	/* BCM_KATANA_SUPPORT */
#ifdef	BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        mbcm_driver[unit] = &mbcm_katana2_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
#endif	/* BCM_KATANA2_SUPPORT */
#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit)) {
        mbcm_driver[unit] = &mbcm_greyhound_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIUMPH);
        return BCM_E_NONE;
    }
#endif /* BCM_GREYHOUND_SUPPORT */
#ifdef	BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWK(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_mbcm_init(unit, &mbcm_driver[unit]));
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
#endif	/* BCM_TOMAHAWK_SUPPORT */

#ifdef  BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        mbcm_driver[unit] = &mbcm_firebolt6_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
#endif  /* BCM_FIREBOLT6_SUPPORT */

#ifdef  BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        mbcm_driver[unit] = &mbcm_hurricane4_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
#endif  /* BCM_HURRICANE4_SUPPORT */

#ifdef  BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        mbcm_driver[unit] = &mbcm_helix5_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
#endif  /* BCM_HELIX5_SUPPORT */

#ifdef	BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        mbcm_driver[unit] = &mbcm_tomahawk3_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

#ifdef  BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3(unit)) {
        mbcm_driver[unit] = &mbcm_trident3_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
#endif  /* BCM_TRIDENT3_SUPPORT */


#ifdef  BCM_MAVERICK2_SUPPORT
    if (SOC_IS_MAVERICK2(unit)) {
        mbcm_driver[unit] = &mbcm_trident3_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
#endif  /* BCM_MAVERICK2_SUPPORT */

#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_IS_HURRICANE3(unit)) {
        mbcm_driver[unit] = &mbcm_hurricane3_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIUMPH);
        return BCM_E_NONE;
    }
#endif /* BCM_HURRICANE3_SUPPORT */
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        mbcm_driver[unit] = &mbcm_greyhound2_driver;
        bcm_chip_family_set(unit, BCM_FAMILY_TRIUMPH);
        return BCM_E_NONE;
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
#ifdef  BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_mbcm_init(unit, &mbcm_driver[unit]));
        bcm_chip_family_set(unit, BCM_FAMILY_TRIDENT);
        return BCM_E_NONE;
    }
#endif  /* BCM_TOMAHAWK2_SUPPORT */

    LOG_CLI((BSL_META_U(unit,
                        "ERROR: mbcm_init unit %d: unsupported chip type\n"), unit));
    return BCM_E_INTERNAL;
}

int mbcm_deinit(int unit)
{
#if defined(BCM_TOMAHAWK_SUPPORT)
    int latency = SOC_SWITCH_BYPASS_MODE(unit);
    if (SOC_IS_TOMAHAWKX(unit) && latency && mbcm_driver[unit]) {
        if (!(--mbcm_ref[latency])) { /* ref cnt */
            sal_free(mbcm_driver[unit]);
            (latency%2) ? (mbcm_l3_latency = NULL) : (mbcm_l2_latency = NULL);
        }
    }
    mbcm_driver[unit] = NULL;
#else
    mbcm_driver[unit] = NULL;
#endif

    return BCM_E_NONE;
}

#if defined(BCM_TOMAHAWK_SUPPORT)
STATIC int
_bcm_esw_mbcm_init(int unit, mbcm_functions_t **mbcm)
{
    if (mbcm == NULL) {
        return BCM_E_INTERNAL;
    }
    if (mbcm_normal_latency == NULL) {
        if (SOC_IS_TOMAHAWK(unit)) {
            mbcm_normal_latency = &mbcm_tomahawk_driver;
        } else
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            mbcm_normal_latency = &mbcm_tomahawk2_driver;
        } else
#endif
        {
            return BCM_E_INTERNAL;
        }
    }
    /*
     * lower latency units share their mbcm variants, while
     * normal latency units share theirs.
     * this is reqd to plug different entry points for features
     * subject to different latencies (at later stages of init)
     */
    switch(SOC_SWITCH_BYPASS_MODE(unit)) {
        case SOC_SWITCH_BYPASS_MODE_NONE:
            *mbcm = mbcm_normal_latency; /* mbcm_tomahawk_driver */
            break;

        case SOC_SWITCH_BYPASS_MODE_BALANCED:
            if (!mbcm_l3_latency) {
                /* one time */
                if (!(mbcm_l3_latency =
                           sal_alloc(sizeof(mbcm_functions_t),
                                     "l3 latency mbcm variant"))) {
                    return BCM_E_MEMORY;
                }
                sal_memcpy(mbcm_l3_latency, mbcm_normal_latency,
                           sizeof(mbcm_functions_t));

                /*
                 *  overrides for this latency mode start from here
                 *  this covers both the plugging and re-plugging for
                 *  warmboot scenario as well
                 */
            }

            *mbcm = mbcm_l3_latency;
            break;

        case SOC_SWITCH_BYPASS_MODE_LOW:
            if (!mbcm_l2_latency) {
                /* one time */
                if (!(mbcm_l2_latency =
                           sal_alloc(sizeof(mbcm_functions_t),
                                     "l2 latency mbcm variant"))) {
                    return BCM_E_MEMORY;
                }
                sal_memcpy(mbcm_l2_latency, mbcm_normal_latency,
                           sizeof(mbcm_functions_t));

                /*
                 *  overrides for this latency mode start from here
                 *  this covers both the plugging and re-plugging for
                 *  warmboot scenario as well
                 */

                /* reroute trunk functionality to xgs3 for Fast LAG */
                mbcm_l2_latency->mbcm_trunk_modify     = bcm_xgs3_trunk_modify;
                mbcm_l2_latency->mbcm_trunk_get        = bcm_xgs3_trunk_get;
                mbcm_l2_latency->mbcm_trunk_destroy    = bcm_xgs3_trunk_destroy;
                mbcm_l2_latency->mbcm_trunk_mcast_join = bcm_xgs3_trunk_mcast_join;
            }

            *mbcm = mbcm_l2_latency;
            break;
    }

    ++mbcm_ref[SOC_SWITCH_BYPASS_MODE(unit)]; /* ref cnt */

    return BCM_E_NONE;
}
#endif

