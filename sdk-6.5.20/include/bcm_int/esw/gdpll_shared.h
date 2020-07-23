/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        gdpll_shared.h
 * Purpose:     Shared GDPLL header file between M7 and the host
 */

#ifndef __BCM_INT_GDPLL_SHARED_H__
#define __BCM_INT_GDPLL_SHARED_H__

#if defined(INCLUDE_GDPLL)

/* M7 DTCM region references */
#define DPLL_ALIVE_SIZE                 4   /* This is the M7 Alive Indication */
#define DPLL_FW_VERSION_SIZE            40  /* This is the M7 version size */
#define DPLL_DEBUG_ENABLE_SIZE          4   /* This is the M7 switch for debug push */
#define DPLL_PARAM_SIZE                 236 /* This size is given to accomodate max 3 states */
#define DPLL_GENERIC_SIZE               (1024*2)

/* For any offset that gets added for versioning, etc */
#define DPLL_ALIVE_OFFSET               0   /* This is the M7 Alive Indication */
#define DPLL_FW_VERSION_OFFSET          (DPLL_ALIVE_OFFSET + DPLL_ALIVE_SIZE)  /* This is the M7 version start */
#define DPLL_BCN_UPDATE_LOC             (DPLL_FW_VERSION_OFFSET + DPLL_FW_VERSION_SIZE)  /* Location used by R5 and Host for BCN counter */
#define DPLL_NTPTOD_UPDATE_LOC          (DPLL_BCN_UPDATE_LOC + 4)     /* Location used by R5 and Host for NTPTOD counter updates */
#define DPLL_PTPTOD_UPDATE_LOC          (DPLL_NTPTOD_UPDATE_LOC + 4)  /* Location used by R5 and Host for PTPTOD counter updates */
#define DPLL_1588_FREQ_LOCK_LOC         (DPLL_PTPTOD_UPDATE_LOC + 4)
#define DPLL_1588_DPLL_OFFSET_LOC       (DPLL_1588_FREQ_LOCK_LOC + 4)

#define DPLL_BS0PLL_FREQ_LOCK_LOC       (DPLL_1588_DPLL_OFFSET_LOC + 4)
#define DPLL_BS0PLL_DPLL_OFFSET_LOC     (DPLL_BS0PLL_FREQ_LOCK_LOC + 4)

#define DPLL_BS1PLL_FREQ_LOCK_LOC       (DPLL_BS0PLL_DPLL_OFFSET_LOC + 4)
#define DPLL_BS1PLL_DPLL_OFFSET_LOC     (DPLL_BS1PLL_FREQ_LOCK_LOC + 4)
#define DPLL_BS0_UPDATE_LOC             (DPLL_BS1PLL_DPLL_OFFSET_LOC + 4)
#define DPLL_BS1_UPDATE_LOC             (DPLL_BS0_UPDATE_LOC + 4)
#define DPLL_NTPTOD_DPLL_OFFSET_LOC     (DPLL_BS1_UPDATE_LOC + 4)
#define DPLL_PTPTOD_DPLL_OFFSET_LOC     (DPLL_NTPTOD_DPLL_OFFSET_LOC + 4)
#define DPLL_CHAN_DUMP_LOC              (DPLL_PTPTOD_DPLL_OFFSET_LOC + 4)   /* size: sizeof(dpll_dump_t)*/


#define DPLL_DEBUG_ENABLE_OFFSET        (DPLL_GENERIC_SIZE)/*200*/   /* This is the M7 switch for debug push */
#define DPLL_PARAM_START_OFFSET         (DPLL_DEBUG_ENABLE_OFFSET + DPLL_DEBUG_ENABLE_SIZE)
#define DPLL_CHAN_PARAM_ADDR(base, chan) \
    (base + DPLL_PARAM_START_OFFSET + (DPLL_PARAM_SIZE * (chan)))

/* M7 SW Interrupt types */
#define INTR_STATE_CHANGE   0x1

/* Channel debug data */
typedef struct COMPILER_ATTRIBUTE((__packed__)) gdpll_chan_debug_dump_s {
    int64   feedback;
    int64   reference;
    int64   perror;
} gdpll_chan_debug_dump_t;

typedef struct  COMPILER_ATTRIBUTE((__packed__)) dpll_dump_s {
    uint32      chan;
    gdpll_chan_debug_dump_t debug_dump[2];
} dpll_dump_t;

#define BCM_GDPLL_NUM_STATES 3

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
    int32   PhaseErrorThres0;
    int32   PhaseErrorThres1;
    uint16  HoldoverFilterCoeff;
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

#endif /* INCLUDE_GDPLL */
#endif /* __BCM_INT_GDPLL_SHARED_H__ */
