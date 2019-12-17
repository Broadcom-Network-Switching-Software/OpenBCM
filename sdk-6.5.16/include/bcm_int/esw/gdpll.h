/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        gdpll.h
 * Purpose:     Internal GDPLL header
 */

#ifndef __BCM_INT_GDPLL_H__
#define __BCM_INT_GDPLL_H__

#if defined(INCLUDE_GDPLL)

#include <bcm/gdpll.h>
#include <bcm_int/esw/gdpll_shared.h>

/* Number of GDPLL input array locations */
#define BCM_GDPLL_NUM_IA_ENTRIES    256

/* TBD - As per section 6.6, port events are from 0 to 191 and misc events
 * start from 192
 */
#define BCM_GDPLL_NUM_INPUT_EVENTS       256
#define BCM_GDPLL_IA_START_TXPI_TXSOF    128
#define BCM_GDPLL_IA_START_MISC          192
#define BCM_GDPLL_IA_START_R5            214

#define GDPLL_DEBUGBUFF_LOCAL_COPY
#ifdef GDPLL_DEBUGBUFF_LOCAL_COPY
#define GDPLL_NUM_DEBUG_BUFFERS     2
#endif


#define bcmGdpllInputEventR5    23
#define bcmGdpllInputEventMax   24

/* M7 TCM region references
 */
#define BCM_M7_ITCM_BASE                (0x3260000)
#define BCM_M7_DTCM_BASE                (0x3264000)

/* Internal datastructure for event destination config */
typedef struct gdpll_event_dest_cfg_s {
    bcm_gdpll_event_dest_t event_dest;
    int                 ts_counter;       /* 0: TS0     1:TS1*/
    int                 ppm_check_enable; /* PPM check enable */
} gdpll_event_dest_cfg_t;

/* Internal datastructure for gdpll channel */
typedef struct dpll_chan_s {
    uint32 flag;        /* Channel flags */
    uint32 eventId_ref; /* Channel reference event */
    uint32 eventId_fb;  /* Channel feedback event */
    int    phaseConterRef;
}dpll_chan_t;

/* Internal datastructure for gdpll context */
typedef struct gdpll_context_s {
    /* Per dpll channel parameters */
    dpll_chan_t dpll_chan[BCM_GDPLL_NUM_CHANNELS];

    /* event_id array holds the Input array location for that event */
    int          eventId[BCM_GDPLL_NUM_INPUT_EVENTS];
    sal_mutex_t  mutex;

    /* User callback for debug registry */
    bcm_gdpll_debug_cb debug_cb;
    void         *pUserData;
    bcm_gdpll_cb gdpll_cb;
    void         *pGdpll_cb_data;
    int          gdpll_cb_type;
    sal_sem_t    debug_sem;
    uint32       debugRdIdx;

}gdpll_context_t;

#if defined(DPLL_VERSION_02)
/* Per DPLL Configuration variables stored in state memory */
typedef struct COMPILER_ATTRIBUTE((__packed__)) m7_gdpll_chan_dpll_config_s {
    int32   K1[BCM_GDPLL_NUM_STATES];
    int8    K1Shift[BCM_GDPLL_NUM_STATES];
    int32   K1K2[BCM_GDPLL_NUM_STATES];
    int8    K1K2Shift[BCM_GDPLL_NUM_STATES];
    uint32  LockDetThres[BCM_GDPLL_NUM_STATES];
    uint32  LockIndicatorThresholdLo;
    uint32  LockIndicatorThresholdHi;
    uint16  DwellCount[BCM_GDPLL_NUM_STATES];
    uint64  PhaseErrorLimiterThres[BCM_GDPLL_NUM_STATES];
    uint8   NCObits;                    /* How many bits in NCO */
    uint8   PhaseCounterRef;            /* use the phase counter instead of timestamp as reference input */
    uint8   DPLLNumOfStates;
    uint16  IdumpMod[BCM_GDPLL_NUM_STATES];
    uint8   PhaseErrorShift;
    int64   NominalLoopFilter;          /* Nominal value of loop filter */
    uint8   InvertPhaseError;
    uint8   NormalizePhaseError;        /* remove full cycles of phase error */
    int32   NormPhaseErrorThres0;
    int32   NormPhaseErrorThres1;
} m7_gdpll_chan_dpll_config_t;

/* Per DPLL state variables stored in state memory */
typedef struct COMPILER_ATTRIBUTE((__packed__)) m7_gdpll_chan_dpll_state_s {
    int64       Offset;
    uint8       LockIndicator;              /* Initialize to FALSE */
    uint8       LossOfLockIndicator;        /* Initialize to FALSE */
    uint8       LargeErrorIndicator;        /* Initialize to FALSE */
    uint8       PadByte1;                   /* Pad for aligned 32b access */
    uint8       HoldoverFlag;               /* Initialize to FALSE. Used to get the DPLL into holdover */
    uint8       InitPhaseCounterFlag;           /* Initialize to FALSE */
    uint8       InitOffsetFlag;                 /* Initialize to FALSE */
    uint8       PadByte2;                   /* Pad for aligned 32b access */
    uint8       DPLLState;
    int64       LoopFilter;
    uint32      DwellCounter;
    uint32      LockDetFilter;
    uint64      PhaseCounter;
    uint64      PhaseCounterDelta;
    uint16      PhaseCounterN;              /* Initialize to 1 */
    uint16      PhaseCounterM;              /* Initialize to 0 */
    uint16      PhaseCounterFrac;           /* Initialize to 0 */
    uint16      IdumpCounter;               /* Initialize to 0 */
    int64       AccumPhaseError;            /* Initialize to 0 */
    int64       PrevIn0[2];                    /* Initialize to 0 */
    int64       PrevIn1[2];                    /* Initialize to 0 */
    uint8       LargeErrorCounter;
} m7_gdpll_chan_dpll_state_t;

/* Internal datastructure for dpll config */
typedef struct COMPILER_ATTRIBUTE((__packed__)) m7_dpll_param_s {
    int debugMode;
    m7_gdpll_chan_dpll_state_t  m7_dpll_state;
    m7_gdpll_chan_dpll_config_t m7_dpll_config;
} m7_dpll_param_t;
#endif  /* DPLL_VERSION_02 */

/* Internal datastructure for dpll config */
typedef struct dpll_param_s {
    int debugMode;
    bcm_gdpll_chan_dpll_config_t    dpll_config;
    bcm_gdpll_chan_dpll_state_t     dpll_state;
} dpll_param_t;

#endif /* INCLUDE_GDPLL */
#endif /* __BCM_INT_GDPLL_H__ */
