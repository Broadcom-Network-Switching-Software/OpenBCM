/*
 * $Id: gdpll_interpolator.h
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __BCM_INT_GDPLL_INTERPOLATOR_H__
#define __BCM_INT_GDPLL_INTERPOLATOR_H__

#if defined(INCLUDE_GDPLL)

#include <sal/core/sync.h>


#include <bcm/gdpll.h>
#include <bcm_int/gdpll_shared.h>

typedef struct
{
    uint64 M;           /* Freq Ratio Denominator */
    uint64 N;           /* Freq Ratio Numerator */
    uint32 InvM;        /* round(2^K/M) */
    uint16 InvMShift;   /* K parameter*/
}_bcm_gdpll_interpolator_params_t;

#define BCM_GDPLL_MAX_NUM_INTERPOLATORS                     (15)
#define BCM_GDPLL_MAX_NUM_INTERPOLATOR_EVENTID              (128)
#define INTERPOLATOR_INVALID_TS                             (0x100000000000000)

typedef struct
{
    uint8 enable;
    uint64 M;             /* Freq Ratio Denominator */
    uint64 N;             /* Freq Ratio Numerator */
    uint32 InvM;          /* round(2^K/M) */
    uint16 InvMShift;     /* K parameter*/
    uint64 prevTs;        /* PrevTs*/
    uint64 Accum;         /* Accumulation */
    uint32 iaAddr;        /* GDPLL IA address*/
}_bcm_gdpll_interpolator_t;

typedef struct
{
    uint8     enable;
    uint16 num_interpolators;
    _bcm_gdpll_interpolator_t interpolator[BCM_GDPLL_MAX_NUM_INTERPOLATORS];
}_bcm_gdpll_interpolator_ctxt_t;

extern _bcm_gdpll_interpolator_params_t g_gdpll_interpolator_params[(bcmGdpllportTypeLast-1)*(bcmGdpllportTypeLast-1)];

extern int _bcm_gdpll_interpolator_setup(int unit, bcm_gdpll_chan_t *pGdpllChan, int *pChan, uint32 *piaAddr_updated);
extern void init_interpolator(void);

#endif /* INCLUDE_GDPLL */
#endif /* __BCM_INT_GDPLL_INTERPOLATOR_H__ */
