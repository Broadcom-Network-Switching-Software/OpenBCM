/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <sal/types.h>
#ifndef _WCMOD_DIAGNOSTICS_H_
#define _WCMOD_DIAGNOSTICS_H_

#include "wcmod_defines.h"
#ifdef _SDK_WCMOD_
#include <sal/types.h>
#endif

/*#define SPEED_CALC_FREQ       156.25 */
/*#define SPEED_CALC_FREQ_UNITS "MHz" */

#define MAX_TAP_SUM_CONSTRAINT 63
#define MIN_TAP_EYE_CONSTRAINT 17

/* #define BER_EXTRAP_SPD_UP */
#define MAX_LOOPS 47
#define MIN_RUNTIME  1
#define INDEX_UNINITIALIZED  (-1)
#define SPD_UP_MAX_RUNTIME 64
#ifdef BER_EXTRAP_SPD_UP
#define MAX_RUNTIME SPD_UP_MAX_RUNTIME
#else
#define MAX_RUNTIME 256
#endif
#define HI_CONFIDENCE_ERR_CNT 100    /* bit errors to determine high confidence */
#define LO_CONFIDENCE_MIN_ERR_CNT 10 /* bit errors, exit condition for low confidence */
#define HI_CONFIDENCE_MIN_ERR_CNT 20 /* bit errors exit condition for high confidence */
#define VEYE_UNIT 1.75
#define HEYE_UNIT 3.125
#define WC_UTIL_MAX_ROUND_DIGITS (8)

/* Do not change the value, used as index */
#define WC_UTIL_VEYE           0  /* vertical eye */
#define WC_UTIL_VEYE_U         1  /* vertical eye up */
#define WC_UTIL_VEYE_D         2  /* vertical eye down */
#define WC_UTIL_HEYE_R         3  /* horizontal right eye */
#define WC_UTIL_HEYE_L         4  /* horizontal left eye */

typedef struct {
    int total_errs[MAX_LOOPS];
    int total_elapsed_time[MAX_LOOPS];
    int mono_flags[MAX_LOOPS];
    int max_loops;
    int offset_max;
    int veye_cnt;
    uint32 rate;      /* frequency in KHZ */
    int first_good_ber_idx;
    int first_small_errcnt_idx;
    int target_ber;    /* this is the target BER example, -18, representing 10^(-18) */
    int tolerance;     /* this is the percentage  specify +/- % opening/margin of the target_ber */
} WCMOD_EYE_DIAG_INFOt;

extern int wcmod_diag_autoneg(wcmod_st *ws);
extern int wcmod_diag_general(wcmod_st *ws);
extern int wcmod_diag_internal_traffic(wcmod_st *ws);
extern int wcmod_diag_link(wcmod_st *ws);
extern int wcmod_diag_topology(wcmod_st *ws);
extern int wcmod_diag_speed(wcmod_st *ws);
extern int _wcmod_eye_margin_data_get(wcmod_st* ws, WCMOD_EYE_DIAG_INFOt *pInfo,int type);
extern int _wcmod_eye_margin_ber_cal(WCMOD_EYE_DIAG_INFOt *pInfo, int type);
extern int _wcmod_eye_margin_diagram_cal(WCMOD_EYE_DIAG_INFOt *pInfo, int type);

#endif

