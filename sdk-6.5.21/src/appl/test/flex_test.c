/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This is the top level Flex port test, which will select the
 * chip-specific flexport test based on chip info.
 */
#include <appl/diag/system.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#include <soc/cm.h>
#include <soc/dma.h>
#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/cmicm.h>
#include <soc/cmic.h>

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_L3) && \
           (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)) && \
           !defined(NO_SAL_APPL) && (!defined(__KERNEL__))

/* flexport.c : TH2, TD2+ */
extern  int     flexport_test_init(int,args_t *, void **);
extern  int     flexport_test_cleanup(int, void *);
extern  int     flexport_test(int, args_t *, void *);

/* flexport_td3.c : TD3 */
extern  int     flexport_td3_test_init(int,args_t *, void **);
extern  int     flexport_td3_test_cleanup(int, void *);
extern  int     flexport_td3_test(int, args_t *, void *);

/* flexport_th3.c : TH3 */
#if defined(BCM_TOMAHAWK3_SUPPORT)
extern  int     flexport_th3_test_init(int,args_t *, void **);
extern  int     flexport_th3_test_cleanup(int, void *);
extern  int     flexport_th3_test(int, args_t *, void *);
#endif

/*
 * Function:
 *      flex_test_init
 * Purpose:
 *      Test init function. Parse CLI params and do necessary init.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     SOC_E_XXXX
 */
int
flex_test_init(int unit, args_t *a, void **pa)
{
    int rv = BCM_E_NONE;

    if (SOC_IS_TOMAHAWK3(unit)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
        rv = flexport_th3_test_init(unit, a, pa);
#endif
    } else if (SOC_IS_TRIDENT3X(unit)) {
        rv = flexport_td3_test_init(unit, a, pa);
    } else if (SOC_IS_TRIDENT2X(unit) ||
               SOC_IS_TOMAHAWK2(unit)) {
        rv = flexport_test_init(unit, a, pa);
    }

    return rv;
}

/*
 * Function:
 *      flex_test
 * Purpose:
 *      Set up ports/streams and send packets.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     SOC_E_XXXX
 */
int
flex_test(int unit, args_t *a, void *pa)
{
    int rv = BCM_E_NONE;

    if (SOC_IS_TOMAHAWK3(unit)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
        rv = flexport_th3_test(unit, a, pa);
#endif
    } else if (SOC_IS_TRIDENT3X(unit)) {
        rv = flexport_td3_test(unit, a, pa);
    } else if (SOC_IS_TRIDENT2X(unit) ||
               SOC_IS_TOMAHAWK2(unit)) {
        rv = flexport_test(unit, a, pa);
    }

    return rv;
}

/*
 * Function:
 *      flex_test_cleanup
 * Purpose:
 *      Do test end checks and free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      a - Pointer to arguments
 *
 * Returns:
 *     SOC_E_XXXX
 */
int
flex_test_cleanup(int unit, void *pa)
{
    int rv = BCM_E_NONE;

    if (SOC_IS_TOMAHAWK3(unit)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
        rv = flexport_th3_test_cleanup(unit, pa);
#endif
    } else if (SOC_IS_TRIDENT3X(unit)) {
        rv = flexport_td3_test_cleanup(unit, pa);
    } else if (SOC_IS_TRIDENT2X(unit) ||
               SOC_IS_TOMAHAWK2(unit)) {
        rv = flexport_test_cleanup(unit, pa);
    }
    return rv;
}


#endif

