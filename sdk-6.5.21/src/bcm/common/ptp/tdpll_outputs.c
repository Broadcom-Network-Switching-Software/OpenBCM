/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tdpll_outputs.c
 *
 * Purpose: Telecom DPLL output clock (synthesizer) configuration and management.
 *
 * Functions:
 *      bcm_tdpll_output_clock_init
 *      bcm_tdpll_output_clock_enable_get
 *      bcm_tdpll_output_clock_enable_set
 *      bcm_tdpll_output_clock_synth_frequency_get
 *      bcm_tdpll_output_clock_synth_frequency_set
 *      bcm_tdpll_output_clock_deriv_frequency_get
 *      bcm_tdpll_output_clock_deriv_frequency_set
 *      bcm_tdpll_output_clock_holdover_data_get
 *      bcm_tdpll_output_clock_holdover_frequency_set
 *      bcm_tdpll_output_clock_holdover_mode_get
 *      bcm_tdpll_output_clock_holdover_mode_set
 *      bcm_tdpll_output_clock_holdover_reset
 */

#if defined(INCLUDE_PTP)

#include <shared/bsl.h>

#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/ptp_common.h>
#include <bcm/error.h>

#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
#include <bcm/gdpll.h>
#include <bcm_int/esw/gdpll.h>
#include <bcm/time.h>
#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_internal.h>
#endif
#endif

#include <soc/iproc.h>
#define READ_NS_REGr(unit, reg, idx, rvp) \
    soc_iproc_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, idx), rvp);
#define WRITE_NS_REGr(unit, reg, idx, rv) \
        soc_iproc_setreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, idx), rv)

/* Definitions. */
#define TDPLL_OUTPUT_CLOCK_STATE_ENABLE_BIT                            (0u)

#define OUTPUT_CLOCK(clock_index)       \
(objdata.output_clock[clock_index])

/* Macros. */

/* Types. */

/* Constants and variables. */
static bcm_tdpll_output_clock_data_t objdata;

/* Static functions. */
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)

#define OUTCLOCK_BS0       (0)
#define OUTCLOCK_BS1       (1)
#define OUTCLOCK_SYNCE     (2)
#define OUTCLOCK_1588      (3)
#define OUTCLOCK_GPIO      (4)

typedef struct tdpllout_gdpll_state_s {
    /* TDPLL output clock to port mapping */
    uint32 tdpll_output_synceport[BCM_TDPLL_OUTPUT_CLOCK_NUM_SYNCE][BCM_GDPLL_NUM_PORTS];

    /* GDPLL channel handle for a given port */
    uint32 gdpllChan_synceport[BCM_GDPLL_NUM_PORTS];

    /* GDPLL channel handle for a given misc event */
    uint32 gdpllChan_miscEvent[TDPLL_OUTPUT_CLOCK_NUM_MAX];
} tdpllout_gdpll_state_t;

tdpllout_gdpll_state_t tdpllout_gdpll_state;

uint32 gdpll_txpi_coeff_table[8][8] = {
/*                   Stage 0                             Stage 1
 *          K1, K1Shift, K1K2, K1K2Shift,      K1, K1Shift, K1K2, K1K2Shift */
        {720575940, 14,   590295810, 7,    910032812, 5,    24863, 0}, /* 0.01 Hz */
        {720575940, 14,   590295810, 7,    568770507, 8,    621581, 0}, /* 0.05 Hz */
        {720575940, 14,   590295810, 7,    568770507, 9,    2486322, 0}, /* 0.1 Hz */
        {720575940, 14,   590295810, 7,    710963134, 11,   3884879, 4}, /* 0.5 Hz */
        {720575940, 14,   590295810, 7,    710963134, 12,   248632230, 0}, /* 1 Hz */
        {720575940, 14,   590295810, 7,    710963134, 14,   994528922, 2}, /* 4 Hz */
        {720575940, 14,   590295810, 7,    888703918, 14,   776975720, 3}, /* 5 Hz */
        {720575940, 15,   590295810, 9,   888703918, 15,   776975720, 5} /* 10 Hz */
};

uint32 gdpll_ts_counter_coeff_table[8][8] = {
/*                   Stage 0                             Stage 1
 *          K1, K1Shift, K1K2, K1K2Shift,      K1, K1Shift, K1K2, K1K2Shift */
        {576460752, 2,  3689349, 2, 0,0, 0,0}, /* 0.01 Hz */
        {576460752, 2,  3689349, 2, 0,0, 0,0}, /* 0.05 Hz */
        {576460752, 2,  3689349, 2, 3554816, 4,     486, 0}, /* 0.1 Hz */
        {576460752, 2,  3689349, 2, 2221760, 7,     12140, 0}, /* 0.5 Hz */
        {576460752, 2,  3689349, 2, 568770507, 0,   48561, 0}, /* 1 Hz */
        {576460752, 2,  3689349, 2, 568770507, 2,   776976, 0}, /* 4 Hz */
        {576460752, 2,  3689349, 2, 710963134, 2,   1214025, 0}, /* 5 Hz */
        {576460752, 3,  3689349, 4, 710963134, 3,   4856098, 0} /* 10 Hz */
};

uint32 gdpll_bs1_coeff_table[8][8] = {
/*                   Stage 0                              Stage 1
 *          K1,    K1Shift, K1K2,  K1K2Shift,        K1,    K1Shift,  K1K2,    K1K2Shift */
        {540431955, 1,     3458765, 1,                   0,    0,          0,   0}, /* 0.01 Hz */
        {540431955, 1,     3458765, 1,                   0,    0,          0,   0}, /* 0.05 Hz */
        {540431955, 1,     3458765, 1,             3332640,    3,        228,   0}, /* 0.1 Hz */
        {540431955, 1,     3458765, 1,           133305588,    0,       5691,   0}, /* 0.5 Hz */
        {540431955, 1,     3458765, 1,           266611176,    0,      22763,   0}, /* 1 Hz */
        {540431955, 1,     3458765, 1,          1066444702,    0,     364207,   0}, /* 4 Hz */
        {540431955, 1,     3458765, 1,           666527938,    1,     569074,   0}, /* 5 Hz */
        {540431955, 2,     3458765, 3,           666527938,    2,    2276296,   0} /* 10 Hz */
};

extern int _bcm_esw_gdpll_tdpllinstance_bind(int unit, int gdpllChan, uint32 out_clock_offset);
#endif


#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
void gdpll_chan_bandwidth_dependent_config(int unit, bcm_gdpll_chan_t *pGdpllChan, int outClock, bcm_tdpll_dpll_bandwidth_t *bw)
{
    uint8 bw_index = 255;

    /* Stage 0 */
    pGdpllChan->chan_dpll_config.k1[0] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[0] = 0;
    pGdpllChan->chan_dpll_config.k1k2[0] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[0] = 0;

    /* Stage 1 */
    pGdpllChan->chan_dpll_config.k1[1] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2[1] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[1] = 0;

    /* Stage 2 */
    pGdpllChan->chan_dpll_config.k1[2] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[2] = 0;
    pGdpllChan->chan_dpll_config.k1k2[2] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[2] = 0;


    switch (bw->units) {
    case bcm_tdpll_dpll_bandwidth_mHz:
        switch (bw->value) {
        case 10:
            /* 0.01 Hz. */
            bw_index = 0;
            break;
        case 50:
            /* 0.05 Hz. */
            bw_index = 1;
            break;
        case 100:
            /* 0.10 Hz. */
            bw_index = 2;
            break;
        case 500:
            /* 0.50 Hz. */
            bw_index = 3;
            break;
        default:
            break;
        }
        break;
    case bcm_tdpll_dpll_bandwidth_Hz:
        switch (bw->value) {
        case 1:
            /* 1 Hz. */
            bw_index = 4;
            break;
        case 4:
            /* 1 Hz. */
            bw_index = 5;
            break;
        case 5:
            /* 5 Hz. */
            bw_index = 6;
            break;
        case 10:
            /* 10 Hz. */
            bw_index = 7;
            break;
        default:
            break;
        }
        break;
    case bcm_tdpll_dpll_bandwidth_kHz:
        /* Fall through. */
    default:
        break;
    }
    if (bw_index > 7){
        cli_out("Error setting bandwidth dependent parameters: gdpll_chan_bandwidth_dependent_config()!!!\n");
        return;
    }

    cli_out("outClock=%d, bw_index = %d\n", outClock, bw_index);

    if (outClock == OUTCLOCK_BS0) {
        /* Stage 0 */
        pGdpllChan->chan_dpll_config.k1[0] = 666527938;
        pGdpllChan->chan_dpll_config.k1Shift[0] = 2;
        pGdpllChan->chan_dpll_config.k1k2[0] = 11115;
        pGdpllChan->chan_dpll_config.k1k2Shift[0] = 5;

        /* Stage 1 */
        pGdpllChan->chan_dpll_config.k1[1] = 0;
        pGdpllChan->chan_dpll_config.k1Shift[1] = 0;
        pGdpllChan->chan_dpll_config.k1k2[1] = 0;
        pGdpllChan->chan_dpll_config.k1k2Shift[1] = 0;

        /* Stage 2 */
        pGdpllChan->chan_dpll_config.k1[2] = 0;
        pGdpllChan->chan_dpll_config.k1Shift[2] = 0;
        pGdpllChan->chan_dpll_config.k1k2[2] = 0;
        pGdpllChan->chan_dpll_config.k1k2Shift[2] = 0;
    } else if ((outClock == OUTCLOCK_BS1)){
        /* Stage 0 */
        pGdpllChan->chan_dpll_config.k1[0] = gdpll_bs1_coeff_table[bw_index][0];
        pGdpllChan->chan_dpll_config.k1Shift[0] = gdpll_bs1_coeff_table[bw_index][1];
        pGdpllChan->chan_dpll_config.k1k2[0] = gdpll_bs1_coeff_table[bw_index][2];
        pGdpllChan->chan_dpll_config.k1k2Shift[0] = gdpll_bs1_coeff_table[bw_index][3];

        /* Stage 1 */
        pGdpllChan->chan_dpll_config.k1[1] = gdpll_bs1_coeff_table[bw_index][4];
        pGdpllChan->chan_dpll_config.k1Shift[1] = gdpll_bs1_coeff_table[bw_index][5];
        pGdpllChan->chan_dpll_config.k1k2[1] = gdpll_bs1_coeff_table[bw_index][6];
        pGdpllChan->chan_dpll_config.k1k2Shift[1] = gdpll_bs1_coeff_table[bw_index][7];
    } else if (outClock == OUTCLOCK_SYNCE) {
        /* Stage 0 */
        pGdpllChan->chan_dpll_config.k1[0] = gdpll_txpi_coeff_table[bw_index][0];
        pGdpllChan->chan_dpll_config.k1Shift[0] = gdpll_txpi_coeff_table[bw_index][1];
        pGdpllChan->chan_dpll_config.k1k2[0] = gdpll_txpi_coeff_table[bw_index][2];
        pGdpllChan->chan_dpll_config.k1k2Shift[0] = gdpll_txpi_coeff_table[bw_index][3];

        /* Stage 1 */
        pGdpllChan->chan_dpll_config.k1[1] = gdpll_txpi_coeff_table[bw_index][4];
        pGdpllChan->chan_dpll_config.k1Shift[1] = gdpll_txpi_coeff_table[bw_index][5];
        pGdpllChan->chan_dpll_config.k1k2[1] = gdpll_txpi_coeff_table[bw_index][6];
        pGdpllChan->chan_dpll_config.k1k2Shift[1] = gdpll_txpi_coeff_table[bw_index][7];
    } else if ((outClock == OUTCLOCK_1588)){
        /* Stage 0 */
        pGdpllChan->chan_dpll_config.k1[0] = gdpll_ts_counter_coeff_table[bw_index][0];
        pGdpllChan->chan_dpll_config.k1Shift[0] = gdpll_ts_counter_coeff_table[bw_index][1];
        pGdpllChan->chan_dpll_config.k1k2[0] = gdpll_ts_counter_coeff_table[bw_index][2];
        pGdpllChan->chan_dpll_config.k1k2Shift[0] = gdpll_ts_counter_coeff_table[bw_index][3];

        /* Stage 1 */
        pGdpllChan->chan_dpll_config.k1[1] = gdpll_ts_counter_coeff_table[bw_index][4];
        pGdpllChan->chan_dpll_config.k1Shift[1] = gdpll_ts_counter_coeff_table[bw_index][5];
        pGdpllChan->chan_dpll_config.k1k2[1] = gdpll_ts_counter_coeff_table[bw_index][6];
        pGdpllChan->chan_dpll_config.k1k2Shift[1] = gdpll_ts_counter_coeff_table[bw_index][7];
    }


    cli_out("k1[0]: %d, k1Shift[0]: %d, k1k2[0]: %d, k1k2Shift[0]: %d\n",
            pGdpllChan->chan_dpll_config.k1[0], pGdpllChan->chan_dpll_config.k1Shift[0],
            pGdpllChan->chan_dpll_config.k1k2[0], pGdpllChan->chan_dpll_config.k1k2Shift[0]);

    cli_out("k1[1]: %d, k1Shift[1]: %d, k1k2[1]: %d, k1k2Shift[1]: %d\n",
            pGdpllChan->chan_dpll_config.k1[1], pGdpllChan->chan_dpll_config.k1Shift[1],
            pGdpllChan->chan_dpll_config.k1k2[1], pGdpllChan->chan_dpll_config.k1k2Shift[1]);

    cli_out("k1[2]: %d, k1Shift[2]: %d, k1k2[2]: %d, k1k2Shift[2]: %d\n",
            pGdpllChan->chan_dpll_config.k1[2], pGdpllChan->chan_dpll_config.k1Shift[2],
            pGdpllChan->chan_dpll_config.k1k2[2], pGdpllChan->chan_dpll_config.k1k2Shift[2]);

}

void gdpll_chan_config(int unit, bcm_gdpll_chan_t *pGdpllChan, int outClock, bcm_tdpll_dpll_bandwidth_t *bw)
{
    gdpll_chan_bandwidth_dependent_config(unit, pGdpllChan, outClock, bw);
    if (outClock == OUTCLOCK_SYNCE) {

        pGdpllChan->chan_dpll_config.lockDetThres[0]=25600;
        pGdpllChan->chan_dpll_config.lockDetThres[1]=80000;
        pGdpllChan->chan_dpll_config.lockDetThres[2]=0;

        pGdpllChan->chan_dpll_config.dwell_count[0]=1000;
        pGdpllChan->chan_dpll_config.dwell_count[1]=1000;
        pGdpllChan->chan_dpll_config.dwell_count[2]=0;
        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0])=0;
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0])=200000;
        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1])=0;
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1])=195000;
        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2])=0;
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2])=0;

        pGdpllChan->chan_dpll_config.nco_bits=48;
        pGdpllChan->chan_dpll_config.dpll_num_states=2;

        pGdpllChan->chan_dpll_config.idump_mod[0]=0;
        pGdpllChan->chan_dpll_config.idump_mod[1]=0;
        pGdpllChan->chan_dpll_config.idump_mod[2]=0;

        pGdpllChan->chan_dpll_config.phase_error_shift=12;
        u64_H(pGdpllChan->chan_dpll_config.nominal_loop_filter)=0;
        u64_L(pGdpllChan->chan_dpll_config.nominal_loop_filter)=0;
        pGdpllChan->chan_dpll_config.invert_phase_error=0;
        pGdpllChan->chan_dpll_config.lockIndicatorThresholdLHi=80000;
        pGdpllChan->chan_dpll_config.lockIndicatorThresholdLo=25600;
        pGdpllChan->chan_dpll_config.norm_phase_error=1;
        /* Phase error thresholds are in ns */
        pGdpllChan->chan_dpll_config.phase_error_thres0=1000000;
        pGdpllChan->chan_dpll_config.phase_error_thres1=507;
        pGdpllChan->chan_dpll_config.holdover_filter_coeff=512;

        /* DPLL State */
        pGdpllChan->chan_dpll_state.dpll_state=0;
        u64_H(pGdpllChan->chan_dpll_state.loop_filter)=0;
        u64_L(pGdpllChan->chan_dpll_state.loop_filter)=0;
        pGdpllChan->chan_dpll_state.dwell_counter=1000;
        pGdpllChan->chan_dpll_state.lockDetFilter=120000;
        u64_H(pGdpllChan->chan_dpll_state.offset)=0;
        u64_L(pGdpllChan->chan_dpll_state.offset)=0;
        pGdpllChan->chan_dpll_state.init_flag=0;
        pGdpllChan->chan_dpll_state.init_offset_flag=1;
        u64_H(pGdpllChan->chan_dpll_state.phase_counter)=0;
        u64_L(pGdpllChan->chan_dpll_state.phase_counter)=0;
        u64_H(pGdpllChan->chan_dpll_state.phaseCounterDelta)=0;
        u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta)=0;
        pGdpllChan->chan_dpll_state.phaseCounterM=0;
        pGdpllChan->chan_dpll_state.phaseCounterN=1;
        pGdpllChan->chan_dpll_state.phaseCounterFrac=0;
        pGdpllChan->chan_dpll_state.idumpCounter=0;
        u64_H(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
        u64_L(pGdpllChan->chan_dpll_state.accumPhaseError)=0;

    } else if (outClock == OUTCLOCK_1588) {
        /* DPLL configs */
        pGdpllChan->chan_dpll_config.lockDetThres[0]=25600;
        pGdpllChan->chan_dpll_config.lockDetThres[1]=80000;
        pGdpllChan->chan_dpll_config.lockDetThres[2]=0;

        pGdpllChan->chan_dpll_config.dwell_count[0]=500;
        pGdpllChan->chan_dpll_config.dwell_count[1]=500;
        pGdpllChan->chan_dpll_config.dwell_count[2]=0;
        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0])=0;
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0])=100000000;
        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1])=0;
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1])=100000000;
        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2])=0;
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2])=0;

        pGdpllChan->chan_dpll_config.nco_bits=48;
        pGdpllChan->chan_dpll_config.dpll_num_states=2;

        pGdpllChan->chan_dpll_config.idump_mod[0]=0;
        pGdpllChan->chan_dpll_config.idump_mod[1]=0;
        pGdpllChan->chan_dpll_config.idump_mod[2]=0;

        pGdpllChan->chan_dpll_config.phase_error_shift=12;
        u64_H(pGdpllChan->chan_dpll_config.nominal_loop_filter)=0x20000000;
        u64_L(pGdpllChan->chan_dpll_config.nominal_loop_filter)=0;
        pGdpllChan->chan_dpll_config.invert_phase_error=0;
        pGdpllChan->chan_dpll_config.lockIndicatorThresholdLHi=80000;
        pGdpllChan->chan_dpll_config.lockIndicatorThresholdLo=25600;
        pGdpllChan->chan_dpll_config.norm_phase_error=1;
        /* Phase error thresholds are in ns. For TS1 channel it is 800us */
        pGdpllChan->chan_dpll_config.phase_error_thres0=800000;
        pGdpllChan->chan_dpll_config.phase_error_thres1=800000;
        pGdpllChan->chan_dpll_config.holdover_filter_coeff=512;

        /* DPLL State */
        pGdpllChan->chan_dpll_state.dpll_state=0;
        u64_H(pGdpllChan->chan_dpll_state.loop_filter)=0x20000000;
        u64_L(pGdpllChan->chan_dpll_state.loop_filter)=0;
        pGdpllChan->chan_dpll_state.dwell_counter=500;
        pGdpllChan->chan_dpll_state.lockDetFilter=120000;
        u64_H(pGdpllChan->chan_dpll_state.offset)=0;
        u64_L(pGdpllChan->chan_dpll_state.offset)=0;
        pGdpllChan->chan_dpll_state.init_flag=1;
        pGdpllChan->chan_dpll_state.init_offset_flag=0;
        u64_H(pGdpllChan->chan_dpll_state.phase_counter)=0;
        u64_L(pGdpllChan->chan_dpll_state.phase_counter)=0;
        u64_H(pGdpllChan->chan_dpll_state.phaseCounterDelta)=0;
        u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta)=25600000;
        pGdpllChan->chan_dpll_state.phaseCounterM=0;
        pGdpllChan->chan_dpll_state.phaseCounterN=1;
        pGdpllChan->chan_dpll_state.phaseCounterFrac=0;
        pGdpllChan->chan_dpll_state.idumpCounter=0;
        u64_H(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
        u64_L(pGdpllChan->chan_dpll_state.accumPhaseError)=0;

    } else if (outClock == OUTCLOCK_BS0) {
        /* DPLL configs */
        pGdpllChan->chan_dpll_config.lockDetThres[0]=38000;
        pGdpllChan->chan_dpll_config.lockDetThres[1]=0;
        pGdpllChan->chan_dpll_config.lockDetThres[2]=0;

        pGdpllChan->chan_dpll_config.dwell_count[0]=3000;
        pGdpllChan->chan_dpll_config.dwell_count[1]=0;
        pGdpllChan->chan_dpll_config.dwell_count[2]=0;

        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0])=0;
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0])=200000000;
        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1])=0;
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1])=0;
        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2])=0;
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2])=0;

        pGdpllChan->chan_dpll_config.idump_mod[0]=1;
        pGdpllChan->chan_dpll_config.idump_mod[1]=0;
        pGdpllChan->chan_dpll_config.idump_mod[2]=0;

        pGdpllChan->chan_dpll_config.nco_bits=48;
        pGdpllChan->chan_dpll_config.dpll_num_states=1;

        pGdpllChan->chan_dpll_config.phase_error_shift=12;

        u64_H(pGdpllChan->chan_dpll_config.nominal_loop_filter)=0x0F000000;
        u64_L(pGdpllChan->chan_dpll_config.nominal_loop_filter)=0;
        pGdpllChan->chan_dpll_config.invert_phase_error=1;
        pGdpllChan->chan_dpll_config.lockIndicatorThresholdLHi=38000;
        pGdpllChan->chan_dpll_config.lockIndicatorThresholdLo=25000;
        pGdpllChan->chan_dpll_config.norm_phase_error=0;
        /* Phase error thresholds are in ns */
        pGdpllChan->chan_dpll_config.phase_error_thres0=500000000;
        pGdpllChan->chan_dpll_config.phase_error_thres1=500000000;
        pGdpllChan->chan_dpll_config.holdover_filter_coeff=512;

        /* DPLL State */
        pGdpllChan->chan_dpll_state.dpll_state=0;
        u64_H(pGdpllChan->chan_dpll_state.loop_filter)=0x0F000000;
        u64_L(pGdpllChan->chan_dpll_state.loop_filter)=0;
        pGdpllChan->chan_dpll_state.dwell_counter=0;
        pGdpllChan->chan_dpll_state.lockDetFilter=64000;
        u64_H(pGdpllChan->chan_dpll_state.offset)=0;
        u64_L(pGdpllChan->chan_dpll_state.offset)=0;
        pGdpllChan->chan_dpll_state.init_flag=0;
        u64_H(pGdpllChan->chan_dpll_state.phase_counter)=0;
        u64_L(pGdpllChan->chan_dpll_state.phase_counter)=0;
        u64_H(pGdpllChan->chan_dpll_state.phaseCounterDelta)=0;
        u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta)=4000000;
        pGdpllChan->chan_dpll_state.phaseCounterM=0;
        pGdpllChan->chan_dpll_state.phaseCounterN=1;
        pGdpllChan->chan_dpll_state.phaseCounterFrac=0;
        pGdpllChan->chan_dpll_state.idumpCounter=0;
        u64_H(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
        u64_L(pGdpllChan->chan_dpll_state.accumPhaseError)=0;

    } else if (outClock == OUTCLOCK_BS1) {
        /* DPLL configs */
        pGdpllChan->chan_dpll_config.lockDetThres[0]=38000;
        pGdpllChan->chan_dpll_config.lockDetThres[1]=25000;
        pGdpllChan->chan_dpll_config.lockDetThres[2]=0;

        pGdpllChan->chan_dpll_config.dwell_count[0]=250;
        pGdpllChan->chan_dpll_config.dwell_count[1]=250;
        pGdpllChan->chan_dpll_config.dwell_count[2]=0;

        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0])=0;
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0])=400000000;
        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1])=0;
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1])=400000000;
        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2])=0;
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2])=0;

        pGdpllChan->chan_dpll_config.idump_mod[0]=1;
        pGdpllChan->chan_dpll_config.idump_mod[1]=0;
        pGdpllChan->chan_dpll_config.idump_mod[2]=0;

        pGdpllChan->chan_dpll_config.nco_bits=48;
        pGdpllChan->chan_dpll_config.dpll_num_states=2;

        pGdpllChan->chan_dpll_config.phase_error_shift=12;

        u64_H(pGdpllChan->chan_dpll_config.nominal_loop_filter)=0x0F000000;
        u64_L(pGdpllChan->chan_dpll_config.nominal_loop_filter)=0;
        pGdpllChan->chan_dpll_config.invert_phase_error=1;
        pGdpllChan->chan_dpll_config.lockIndicatorThresholdLHi=38000;
        pGdpllChan->chan_dpll_config.lockIndicatorThresholdLo=25000;
        pGdpllChan->chan_dpll_config.norm_phase_error=1;
        /* Phase error thresholds are in ns */
        pGdpllChan->chan_dpll_config.phase_error_thres0=800000;
        pGdpllChan->chan_dpll_config.phase_error_thres1=800000;
        pGdpllChan->chan_dpll_config.holdover_filter_coeff=512;

        /* DPLL State */
        pGdpllChan->chan_dpll_state.dpll_state=0;
        u64_H(pGdpllChan->chan_dpll_state.loop_filter)=0x0F000000;
        u64_L(pGdpllChan->chan_dpll_state.loop_filter)=0;
        pGdpllChan->chan_dpll_state.dwell_counter=250;
        pGdpllChan->chan_dpll_state.lockDetFilter=64000;
        u64_H(pGdpllChan->chan_dpll_state.offset)=0;
        u64_L(pGdpllChan->chan_dpll_state.offset)=0;
        pGdpllChan->chan_dpll_state.init_offset_flag=1;
        pGdpllChan->chan_dpll_state.init_flag=0;
        u64_H(pGdpllChan->chan_dpll_state.phase_counter)=0;
        u64_L(pGdpllChan->chan_dpll_state.phase_counter)=0;
        u64_H(pGdpllChan->chan_dpll_state.phaseCounterDelta)=0;
        u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta)=4000000;
        pGdpllChan->chan_dpll_state.phaseCounterM=0;
        pGdpllChan->chan_dpll_state.phaseCounterN=1;
        pGdpllChan->chan_dpll_state.phaseCounterFrac=0;
        pGdpllChan->chan_dpll_state.idumpCounter=0;
        u64_H(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
        u64_L(pGdpllChan->chan_dpll_state.accumPhaseError)=0;

    }

    return;
}
#endif

/*
 * Function:
 *      bcm_tdpll_output_clock_init()
 * Purpose:
 *      Initialize T-DPLL output clock configuration and management data.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      stack_id  - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_output_clock_init(
    int unit,
    int stack_id)
{
    int i;

    /*Initialize output clock attributes. */
    for (i = 0; i < TDPLL_OUTPUT_CLOCK_NUM_MAX; ++i) {
        /* Identification attributes. */
        OUTPUT_CLOCK(i).index = i;

        /* Synthesizer frequency and TS EVENT (timestamp) frequency. */
        OUTPUT_CLOCK(i).frequency.synth = 0;
        OUTPUT_CLOCK(i).frequency.tsevent = 0;
        OUTPUT_CLOCK(i).frequency.tsevent_quotient = -1;

        /* Derivative clock frequency. */
        OUTPUT_CLOCK(i).frequency.deriv = 0;
        OUTPUT_CLOCK(i).frequency.deriv_quotient = -1;

        /* State. */
        OUTPUT_CLOCK(i).state = 0;
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
        tdpllout_gdpll_state.gdpllChan_miscEvent[i] = -1;
#endif
    }

#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
    for (i = 0; i < BCM_GDPLL_NUM_PORTS; ++i) {
        tdpllout_gdpll_state.gdpllChan_synceport[i] = -1;
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_cleanup()
 * Purpose:
 *      Uninitialize T-DPLL output clock configuration and management data.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      stack_id  - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_output_clock_cleanup(
    int unit,
    int stack_id)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_common_tdpll_output_clock_index_validate()
 * Purpose:
 *      Validate the TDPLL output clock index.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      clock_index - (IN) Output clock index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */

static int
_bcm_common_tdpll_output_clock_index_validate (
    int unit,
    int clock_index)
{
    int rv = BCM_E_NONE;

    if (clock_index < 0 || clock_index >= TDPLL_OUTPUT_CLOCK_NUM_MAX) {
        return BCM_E_PARAM;
    }

    if (!soc_feature(unit, soc_feature_tdpll_outputclk_synce3) &&
       (clock_index == TDPLL_OUTPUT_CLOCK_IDX_SYNCE3) ) {
        return BCM_E_PARAM;
    }

    if (!soc_feature(unit, soc_feature_tdpll_outputclk_xgpll3) &&
	    (clock_index == TDPLL_OUTPUT_CLOCK_IDX_XGPLL3)) {
        return BCM_E_PARAM;
    }

#if defined (BCM_QAX_SUPPORT)
    if ((SOC_IS_QUX(unit) || SOC_IS_QAX(unit)) &&
        ((clock_index == TDPLL_OUTPUT_CLOCK_IDX_GPIO4) ||
         (clock_index == TDPLL_OUTPUT_CLOCK_IDX_GPIO5)) ) {
        return BCM_E_PARAM;
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_enable_get()
 * Purpose:
 *      Get output clock enable Boolean.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 *      enable      - (OUT) Output clock enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_output_clock_enable_get(
    int unit,
    int stack_id,
    int clock_index,
    int *enable)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_PAYLOAD_OUTPUT_CLOCK_ENABLED_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_OUTPUT_CLOCK_ENABLED_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (clock_index < 0 || clock_index >= TDPLL_OUTPUT_CLOCK_NUM_MAX) {
        return BCM_E_PARAM;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    /* Make indexed payload to get frequency for specified output clock. */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    payload[6] = (uint8)clock_index;
    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_OUTPUT_CLOCK_ENABLED,
            payload, PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : Output clock index.
     *    Octet 7       : Output clock enable Boolean.
     */
    i = 6; /* Advance cursor past custom management message identifier. */
    ++i;   /* Advance past output clock index. */

    *enable = resp[i] ? 1:0;

    /* Set host-maintained output clock enable Boolean. */
    if (*enable) {
        OUTPUT_CLOCK(clock_index).state |= (1 << TDPLL_OUTPUT_CLOCK_STATE_ENABLE_BIT);
    } else {
        OUTPUT_CLOCK(clock_index).state &= ~(1 << TDPLL_OUTPUT_CLOCK_STATE_ENABLE_BIT);
    }

    return BCM_E_NONE;
}

#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
int _bcm_tdpll_output_clock_synce_gdpll_create(int unit,
        int clock_index,
        bcm_port_t port,
        uint32 event_div,
        bcm_tdpll_dpll_bandwidth_t *bandwidth)
{
    bcm_gdpll_chan_t    gdpll_chan;
    int gdpllChan = 0;
    int rv = BCM_E_NONE;

    uint32 flags = BCM_GDPLL_CONF_DPLL |
                   BCM_GDPLL_EVENT_CONFIG_REF | BCM_GDPLL_EVENT_CONFIG_FB |
                   BCM_GDPLL_CHAN_ALLOC | BCM_GDPLL_CHAN_SET_PRIORITY |
                   BCM_GDPLL_CHAN_OUTPUT_CONF;

    sal_memset(&gdpll_chan, 0, sizeof(bcm_gdpll_chan_t));

    /* Feedback config */
    gdpll_chan.event_fb.input_event     = bcmGdpllInputEventPORT;
    gdpll_chan.event_fb.port_event_type = bcmGdpllPortEventTXPI;
    gdpll_chan.event_fb.event_dest      = bcmGdpllEventDestM7;
    gdpll_chan.event_fb.ts_counter      = 0; /* TS0 */
    gdpll_chan.event_fb.port            = port;
    gdpll_chan.event_fb.event_divisor   = event_div;

    /* Reference config */
    gdpll_chan.event_ref.input_event    = bcmGdpllInputEventR5;
    gdpll_chan.event_ref.event_dest     = bcmGdpllEventDestCPU;
    gdpll_chan.event_ref.event_divisor  = 1;

    /* Output and channel configs */
    gdpll_chan.chan_prio    = 0;
    gdpll_chan.out_event    = bcmGdpllOutputEventTXPI;
    gdpll_chan.ts_pair_dest = bcmGdpllDebugDestM7;

    /* Set phase counter reference */
    gdpll_chan.chan_dpll_config.phase_counter_ref = 0;

    /* Output port config */
    gdpll_chan.port         = gdpll_chan.event_fb.port;

    /* Channel bw related config */
    gdpll_chan_config(unit, &gdpll_chan, OUTCLOCK_SYNCE, bandwidth);

    rv = bcm_gdpll_chan_create (unit, flags, &gdpll_chan, &gdpllChan);
    if (BCM_FAILURE(rv)) {
        PTP_ERROR_FUNC("bcm_gdpll_chan_create()");
    } else {
        LOG_INFO(BSL_LS_BCM_PTP,
                    (BSL_META_U(0, "gdpll_chan_created: OutClk:%d port:%d div:%d gdpll_ch:%d \n"),
                    clock_index, port, event_div, gdpllChan));
        tdpllout_gdpll_state.gdpllChan_synceport[port-1] = gdpllChan;

        rv = _bcm_esw_gdpll_tdpllinstance_bind(unit, gdpllChan, clock_index);
        if (BCM_FAILURE(rv)) {
            PTP_ERROR_FUNC("_bcm_esw_gdpll_tdpllinstance_bind()");
        }
    }
    return rv;
}

int _bcm_common_tdpll_output_clock_portattach(int unit,
         int clock_index,
         bcm_port_t port,
         bcm_tdpll_dpll_bandwidth_t *bandwidth)
{
    int gdpllChan = -1;
    int synce_idx = 0;
    uint32 txpi_clk = 0;
    uint32 rxcdr_clk = 0;
    uint32 prev_event_div = 0;
    uint32 event_div = 0;
    int rv = BCM_E_NONE;

    synce_idx = clock_index - BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC;

    portmod_port_txpi_rxcdr_clk_get(unit, port, &txpi_clk, &rxcdr_clk);

    if (txpi_clk && OUTPUT_CLOCK(clock_index).frequency.tsevent) {
        event_div = txpi_clk/(OUTPUT_CLOCK(clock_index).frequency.tsevent * 2);
    }

    if (tdpllout_gdpll_state.tdpll_output_synceport[synce_idx][port-1] == 1) {
        /* The port was already part of the output clock */
        gdpllChan = tdpllout_gdpll_state.gdpllChan_synceport[port-1];

        if (gdpllChan != -1) {
            prev_event_div = _bcm_esw_gdpll_event_fb_divisor_get(unit, gdpllChan);

            LOG_VERBOSE(BSL_LS_BCM_PTP,
              (BSL_META_U(0, "attach: clock:%d, port:%d txpiclk:%d div:%d/%d gdpll_ch:%d\n"),
              clock_index, port, txpi_clk, event_div, prev_event_div, gdpllChan));

            if( prev_event_div == event_div) {
                return rv;
            } else {
                /* port speed changed. Recreate the channel */
                LOG_VERBOSE(BSL_LS_BCM_PTP,
                  (BSL_META_U(0, "gdpll_chan_destroy: clock:%d port:%d  gdpll_ch:%d \n"),
                  clock_index, port, gdpllChan));

                rv = bcm_gdpll_chan_destroy(unit, gdpllChan);
                if (rv != BCM_E_NONE) {
                    PTP_ERROR_FUNC("bcm_gdpll_chan_destroy()");
                }
                tdpllout_gdpll_state.gdpllChan_synceport[port-1] = -1;
                tdpllout_gdpll_state.tdpll_output_synceport[synce_idx][port-1] = 0;
            }
        }
    }

    /* create GDPLL channel for the port */
    rv = _bcm_tdpll_output_clock_synce_gdpll_create(unit, clock_index, port, event_div, bandwidth);
    if (rv != BCM_E_NONE) {
        PTP_ERROR_FUNC("_bcm_tdpll_output_clock_synce_gdpll_create()");
        return rv;
    }
    tdpllout_gdpll_state.tdpll_output_synceport[synce_idx][port-1] = 1;
    return rv;
}

int _bcm_tdpll_output_clock_synce_pbm(int unit,
         int clock_index,
         bcm_pbmp_t pbmp)
{
    bcm_port_t port;
    int gdpllChan=-1;
    int synce_idx;
    int iter=0;
    int rv = BCM_E_NONE;

    synce_idx = clock_index-BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC;

    for (port = 1; port <= BCM_GDPLL_NUM_PORTS; port++) {
        gdpllChan = tdpllout_gdpll_state.gdpllChan_synceport[port-1];
        if (gdpllChan == -1) {
            /* gdpll ch doesn't exist for the port */
            continue;
        }

        /* The port was already configured for output */
        if (tdpllout_gdpll_state.tdpll_output_synceport[synce_idx][port-1] == 1) {
            if (!BCM_PBMP_MEMBER(pbmp, port)) {
                /* port is not part of outclock any more */
                rv = bcm_gdpll_chan_destroy(unit, gdpllChan);
                if (rv != BCM_E_NONE) {
                    PTP_ERROR_FUNC("bcm_gdpll_chan_destroy()");
                }
                tdpllout_gdpll_state.gdpllChan_synceport[port-1] = -1;
                tdpllout_gdpll_state.tdpll_output_synceport[synce_idx][port-1] = 0;
                LOG_VERBOSE(BSL_LS_BCM_PTP,
                  (BSL_META_U(0, "gdpll_chan_destroy: out_clk:%d port:%d gdpll_ch:%d \n"),
                  clock_index, port, gdpllChan));
            }
        } else {
            for (iter=0; iter < BCM_TDPLL_OUTPUT_CLOCK_NUM_SYNCE; iter++) {
                if ((iter != synce_idx) &&
                    (tdpllout_gdpll_state.tdpll_output_synceport[iter][port-1] == 1) &&
                    (BCM_PBMP_MEMBER(pbmp, port)) ) {
                    /* port was part of other outclock */
                    rv = bcm_gdpll_chan_destroy(unit, gdpllChan);
                    if (rv != BCM_E_NONE) {
                        PTP_ERROR_FUNC("bcm_gdpll_chan_destroy()");
                    }
                    tdpllout_gdpll_state.gdpllChan_synceport[port-1] = -1;
                    tdpllout_gdpll_state.tdpll_output_synceport[iter][port-1] = 0;
                    LOG_VERBOSE(BSL_LS_BCM_PTP,
                      (BSL_META_U(0, "gdpll_chan_destroy: out_clk:%d port:%d gdpll_ch:%d \n"),
                      clock_index, port, gdpllChan));
                }
            }
        }
    } /* for */
    return rv;
}

int _bcm_tdpll_output_clock_synce_configure(int unit,
         int clock_index,
         bcm_pbmp_t pbmp,
         bcm_tdpll_dpll_bandwidth_t *bandwidth)
{
    bcm_port_t port;
    int rv = BCM_E_NONE;

    rv = _bcm_tdpll_output_clock_synce_pbm(unit, clock_index, pbmp);
    if (rv != BCM_E_NONE) {
        PTP_ERROR_FUNC("_bcm_tdpll_output_clock_synce_pbm()");
        return rv;
    }

    BCM_PBMP_ITER(pbmp, port) {
        /* Set for non-cpri ports */
        if(!IS_CPRI_PORT(unit, port)) {
            rv = _bcm_common_tdpll_output_clock_portattach(unit, clock_index, port, bandwidth);
            if (rv != BCM_E_NONE) {
                PTP_ERROR_FUNC("_bcm_common_tdpll_output_clock_portattach()");
                return rv;
            }
        }
    }
    return rv;
}
#endif

/*
 * Function:
 *      bcm_tdpll_output_clock_create()
 * Purpose:
 *      Create output clock.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      pbmp        - (IN) Port bitmap.
 *      bandwidth   - (IN) DPLL Bandwidth.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_output_clock_create(
    int unit,
    int stack_id,
    int clock_index,
    bcm_pbmp_t pbmp,
    bcm_tdpll_dpll_bandwidth_t *bandwidth)
{
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
    uint32 flags = BCM_GDPLL_CONF_DPLL |
                   BCM_GDPLL_EVENT_CONFIG_REF | BCM_GDPLL_EVENT_CONFIG_FB |
                   BCM_GDPLL_CHAN_ALLOC | BCM_GDPLL_CHAN_SET_PRIORITY |
                   BCM_GDPLL_CHAN_OUTPUT_CONF;
    int result=0, gdpllChan=0;
    bcm_gdpll_chan_t    gdpll_chan;
    _bcm_ptp_stack_info_t *stack_p;
    _bcm_ptp_info_t *ptp_info_p;

    bcm_time_ts_counter_t counter;
    uint64 T, V, val_64b;
    uint64 mask_bit;

    int rv = BCM_E_NONE;

    uint32 start_delay=0;
    uint32 regVal=0;

    SET_PTP_INFO;
    stack_p = &ptp_info_p->stack_info[stack_id];

    sal_memset(&gdpll_chan, 0, sizeof(bcm_gdpll_chan_t));

    if (clock_index < BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC) {
        /* Set the GDPLL channel config for BS */
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                        (BSL_META_U(unit,
                            " ### tdpll_output_clock: Set BS%d \n"), clock_index));

        /* Feedback config */
        gdpll_chan.event_fb.event_dest      = bcmGdpllEventDestALL;

        gdpll_chan.chan_prio    = 0;
        gdpll_chan.ts_pair_dest = bcmGdpllDebugDestM7;

        if (clock_index == 0) {
            gdpll_chan.event_fb.input_event     = bcmGdpllInputEventBS0PLL;
            gdpll_chan.event_fb.ts_counter      = 1; /* TS1 */
            gdpll_chan.event_fb.event_divisor   = 2500; /* Rate supported for BS synth is 4KHz */
            gdpll_chan.out_event                = bcmGdpllOutputEventBSPLL0;

           /* Set phase counter reference */
           gdpll_chan.chan_dpll_config.phase_counter_ref = 1;

           gdpll_chan_config(unit, &gdpll_chan, OUTCLOCK_BS0, bandwidth);
        } else {
            gdpll_chan.event_fb.input_event     = bcmGdpllInputEventBS1PLL;
            gdpll_chan.event_fb.ts_counter      = 0 /* TS0 */;
            gdpll_chan.event_fb.event_divisor   = 16000; /* Rate supported for BS synth is 625Hz */
            gdpll_chan.out_event                = bcmGdpllOutputEventBSPLL1;

            /* Reference config */
            gdpll_chan.event_ref.input_event    = bcmGdpllInputEventR5;
            gdpll_chan.event_ref.event_dest     = bcmGdpllEventDestCPU;
            gdpll_chan.event_ref.event_divisor  = 1;

            /* Clear phase counter reference */
            gdpll_chan.chan_dpll_config.phase_counter_ref = 0;

            gdpll_chan_config(unit, &gdpll_chan, OUTCLOCK_BS1, bandwidth);
        }

        if (clock_index == 0) {
            /* Get the current 52b timestamper value */
            counter.ts_counter=gdpll_chan.event_fb.ts_counter;
            rv = bcm_time_ts_counter_get(unit, &counter);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_PTP,
                        (BSL_META_U(0, "### Error in bcm_time_ts_counter_get ### \n")));
                return rv;
            }
            COMPILER_64_COPY(T, counter.ts_counter_ns);

            /* V = floor(T / 16,000,000,000) */
            COMPILER_64_COPY(V, T);
            COMPILER_64_SET(val_64b, 0x3, 0xB9ACA000);  /* 16,000,000,000 = 0x3_B9AC_A000 */
            COMPILER_64_UDIV_64(V, val_64b);

            /* ensure headroom of (START_DELAY + 1ms) before 1sec TS1 boundary */
            COMPILER_64_ADD_32(V, 2); /* align with (X+2)th 1sec-Boundary */
            COMPILER_64_UMUL_32(V, 1000000000);
            COMPILER_64_SHL(V, 4);
            COMPILER_64_SUB_32(V, 4000000);/* For 4Khz DPLL rate for BS0 PLL */

            /* 52b mask for Mod 2^52 */
            u64_H(mask_bit) = 0xFFFFF;
            u64_L(mask_bit) = 0xFFFFFFFF;

            COMPILER_64_AND(V, mask_bit);
            COMPILER_64_COPY(gdpll_chan.chan_dpll_state.phase_counter, V);

            READ_NS_REGr(unit, NS_BROADSYNC_SYNC_MODEr, 0, &regVal );
            start_delay = soc_reg_field_get(unit, NS_BROADSYNC_SYNC_MODEr, regVal, START_DELAYf);

            LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                                " BS synth chan create : CurrentTS[%llu/0x%llx] phase_counter[%llu/0x%llx] start_delay[%u]\n"),
                                (long long unsigned int)counter.ts_counter_ns, (long long unsigned int)counter.ts_counter_ns,
                                (long long unsigned int)gdpll_chan.chan_dpll_state.phase_counter, (long long unsigned int)gdpll_chan.chan_dpll_state.phase_counter, start_delay));
            /* Check if the headroom is more than (Start_Delay+1ms)*/
            COMPILER_64_SET(val_64b, 0, 1000000);   /* 1mSec = 1000000nSec */
            COMPILER_64_ADD_32(val_64b, (start_delay*50));
            COMPILER_64_SHR(V, 4);
            COMPILER_64_SHR(T, 4);
            COMPILER_64_SUB_64(V, T);

            if (COMPILER_64_GT(V, val_64b)) {
                LOG_VERBOSE(BSL_LS_BCM_PTP,
                        (BSL_META_U(unit,
                                    "Available headroom[%llu nsec] \n"), (long long unsigned int)u64_L(V)));
                COMPILER_64_SUB_64(V, val_64b);
                COMPILER_64_UDIV_32(V, 1000);
                LOG_VERBOSE(BSL_LS_BCM_PTP,
                        (BSL_META_U(unit,
                                    "wait for %u sec \n"), (unsigned)u64_L(V)));
                sal_usleep(u64_L(V));
            } else {
                LOG_VERBOSE(BSL_LS_BCM_PTP,
                        (BSL_META_U(unit,
                                    "Insufficient headroom[%llu / 0x%llx] \n"), (long long unsigned int)u64_L(V), (long long unsigned int)u64_L(V)));
            }
        }

        result = bcm_gdpll_chan_create (unit, flags, &gdpll_chan, &gdpllChan);
        if (BCM_FAILURE(result)) {
            cli_out(" tdpll_output_clock_create: Error!! gdpll chan create failed\n");
        } else {

            rv = _bcm_esw_gdpll_tdpllinstance_bind(unit, gdpllChan, clock_index);
            tdpllout_gdpll_state.gdpllChan_miscEvent[clock_index] = gdpllChan;
            cli_out(" gdpll_chan:%d\n", gdpllChan);
        }

    } else if (clock_index < (BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC + BCM_TDPLL_OUTPUT_CLOCK_NUM_SYNCE)) {

        /* handle pbm changes and create/recreate the GDPLL channels according to pbmp */
        _bcm_tdpll_output_clock_synce_configure(unit, clock_index, pbmp, bandwidth);

    } else if (clock_index == (BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC + BCM_TDPLL_OUTPUT_CLOCK_NUM_SYNCE)) {
        cli_out(" tdpll_output_clock_enable_set: Output clock is 1588-Hybrid\n");

        /* Reference config */
        gdpll_chan.event_fb.input_event    = bcmGdpllInputEventR5;
        gdpll_chan.event_fb.event_dest     = bcmGdpllEventDestCPU;
        gdpll_chan.event_fb.event_divisor  = 1;

        /* Channel output configurations */
        gdpll_chan.chan_prio    = 0;
        if ((stack_p->flags & BCM_PTP_STACK_TIMESTAMPER_COMBINED_MODE)) {
            gdpll_chan.out_event    = bcmGdpllOutputEventTS0;   /* combined mode */
        } else {
            gdpll_chan.out_event    = bcmGdpllOutputEventTS1;   /* Non-combined mode */
        }

        gdpll_chan.ts_pair_dest = bcmGdpllDebugDestM7;
        gdpll_chan.event_fb.ts_counter = 1; /* TS1 */

        /* Set phase counter reference */
        gdpll_chan.chan_dpll_config.phase_counter_ref = 1;

        /* DPLL config for 1588 */
        gdpll_chan_config(unit, &gdpll_chan, OUTCLOCK_1588, bandwidth);

        result = bcm_gdpll_chan_create (unit, flags, &gdpll_chan, &gdpllChan);
        if (BCM_FAILURE(result)) {
            cli_out(" tdpll_output_clock_enable_set: Error!! gdpll chan create failed\n");
        } else {

            rv = _bcm_esw_gdpll_tdpllinstance_bind(unit, gdpllChan, clock_index);
            tdpllout_gdpll_state.gdpllChan_miscEvent[clock_index] = gdpllChan;
            cli_out(" gdpll_chan:%d\n", gdpllChan);
        }
    } else if (clock_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) {
        cli_out(" tdpll_output_clock_enable_set: Output clock is GPIO\n");
    }

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_tdpll_output_clock_enable_set()
 * Purpose:
 *      Set output clock enable Boolean.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 *      enable      - (IN) Output clock enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_output_clock_enable_set(
    int unit,
    int stack_id,
    int clock_index,
    int enable)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_OUTPUT_CLOCK_ENABLED_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
    int result=0, gdpllChan=0;
    bcm_port_t port;
#endif

    if (BCM_FAILURE(rv = _bcm_common_tdpll_output_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_output_clock_index_validate()");
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
    if (clock_index < BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC) {
        /* Broadsync PLL channels */
        gdpllChan = tdpllout_gdpll_state.gdpllChan_miscEvent[clock_index];
        if (gdpllChan != -1) {
            result = bcm_gdpll_chan_enable(unit, gdpllChan, enable);
            if (BCM_FAILURE(result)) {
                cli_out(" tdpll_output_clock_enable_set: Error!! gdpll chan enable failed\n");
                return result;
            }
        }
    } else if (clock_index < (BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC + BCM_TDPLL_OUTPUT_CLOCK_NUM_SYNCE)) {
        /* Channel associated to syncE */
        port = 1;
        while (port <= BCM_GDPLL_NUM_PORTS) {
            /* Check if the port is associated to this clock */
            if (tdpllout_gdpll_state.tdpll_output_synceport[clock_index - BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC][port-1] == 1) {
                /* Check if the channel associated to port is valid */
                gdpllChan = tdpllout_gdpll_state.gdpllChan_synceport[port-1];
                if(gdpllChan != -1) {
                    result = bcm_gdpll_chan_enable(unit, gdpllChan, enable);
                    if (BCM_FAILURE(result)) {
                        cli_out(" tdpll_output_clock_enable_set: Error!! gdpll chan enable failed\n");
                        return result;
                    }
                }
            }
            port++;
        }
    } else if (clock_index == (BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC + BCM_TDPLL_OUTPUT_CLOCK_NUM_SYNCE)) {
        /* 1588 channel */
        gdpllChan = tdpllout_gdpll_state.gdpllChan_miscEvent[clock_index];
        if (gdpllChan != -1) {
            result = bcm_gdpll_chan_enable(unit, gdpllChan, enable);
            if (BCM_FAILURE(result)) {
                cli_out(" tdpll_output_clock_enable_set: Error!! gdpll chan enable failed\n");
                return result;
            }
        }
    } else if (clock_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) {
        /* GPIO channels */
        cli_out(" tdpll_output_clock_enable_set: Output clock is GPIO\n");
    }

#endif

    /*
     * Make payload.
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : Output clock index.
     *    Octet 7       : Output clock enable Boolean.
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i++] = (uint8)clock_index;
    payload[i] = enable ? 1:0;

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_OUTPUT_CLOCK_ENABLED,
            payload, PTP_MGMTMSG_PAYLOAD_OUTPUT_CLOCK_ENABLED_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /* Set host-maintained output clock enable Boolean. */
    if (enable) {
        OUTPUT_CLOCK(clock_index).state |= (1 << TDPLL_OUTPUT_CLOCK_STATE_ENABLE_BIT);
    } else {
        OUTPUT_CLOCK(clock_index).state &= ~(1 << TDPLL_OUTPUT_CLOCK_STATE_ENABLE_BIT);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_synth_frequency_get()
 * Purpose:
 *      Get output-clock (synthesizer) frequency.
 * Parameters:
 *      unit              - (IN) Unit number.
 *      stack_id          - (IN) Stack identifier index.
 *      clock_index       - (IN) Output clock index.
 *      synth_frequency   - (OUT) Synthesizer frequency (Hz).
 *      tsevent_frequency - (OUT) TS event frequency (Hz).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_output_clock_synth_frequency_get(
    int unit,
    int stack_id,
    int clock_index,
    uint32 *synth_frequency,
    uint32 *tsevent_frequency)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_PAYLOAD_OUTPUT_CLOCK_SYNTH_FREQUENCY_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_OUTPUT_CLOCK_SYNTH_FREQUENCY_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_common_tdpll_output_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_output_clock_index_validate()");
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    /* Make indexed payload to get synthesizer frequency for specified output clock. */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    payload[6] = (uint8)clock_index;
    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_OUTPUT_CLOCK_SYNTH_FREQUENCY,
            payload, PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : Output clock index.
     *    Octet 7       : Reserved.
     *    Octet 8...11  : Synthesizer frequency (Hz).
     *    Octet 12...15 : TS event frequency (Hz).
     */
    i = 6; /* Advance cursor past custom management message identifier. */
    ++i;   /* Advance past output clock index. */
    ++i;   /* Advance past reserved octet. */

    *synth_frequency = _bcm_ptp_uint32_read(resp + i);
    i += 4;
    *tsevent_frequency = _bcm_ptp_uint32_read(resp + i);

    /* Set host-maintained output clock frequencies. */
    OUTPUT_CLOCK(clock_index).frequency.synth = *synth_frequency;
    OUTPUT_CLOCK(clock_index).frequency.tsevent = *tsevent_frequency;
    OUTPUT_CLOCK(clock_index).frequency.tsevent_quotient = *tsevent_frequency ?
        (*synth_frequency + (*tsevent_frequency >> 1))/(*tsevent_frequency) : -1;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_synth_frequency_set()
 * Purpose:
 *      Set output-clock (synthesizer) frequency.
 * Parameters:
 *      unit              - (IN) Unit number.
 *      stack_id          - (IN) Stack identifier index.
 *      clock_index       - (IN) Output clock index.
 *      synth_frequency   - (IN) Synthesizer frequency (Hz).
 *      tsevent_frequency - (IN) TS event frequency (Hz).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_output_clock_synth_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    uint32 synth_frequency,
    uint32 tsevent_frequency)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_OUTPUT_CLOCK_SYNTH_FREQUENCY_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_common_tdpll_output_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_output_clock_index_validate()");
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    /*
     * CONSTRAINT: TS event frequency is a multiple of 1 kHz / 100 Hz.
     *             DPLL instances (and physical synthesizers bound to them)
     *             operate at 1 kHz / 100 Hz.
     */
    if ((0 == tsevent_frequency) || (tsevent_frequency % BCM_TDPLL_FREQUENCY)) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "TS EVENT frequency is not a multiple of %u Hz. fTS: %u\n"),
                     (unsigned)BCM_TDPLL_FREQUENCY, (unsigned)tsevent_frequency));
        return BCM_E_PARAM;
    }

    /*
     * CONSTRAINT: TS event frequency and synthesizer frequency are integrally
     *             related, N = f_synth / f_tsevent, such that TS events occur
     *             at every Nth synthesizer clock edge.
     */
    if ((0 == synth_frequency) || (synth_frequency % tsevent_frequency)) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "SYNTH and TS EVENT frequencies are not integrally related. fSYNTH: %u fTS: %u"),
                     (unsigned)synth_frequency, (unsigned)tsevent_frequency));
        return BCM_E_PARAM;
    }

    /*
     * Make payload.
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : Output clock index.
     *    Octet 7       : Reserved.
     *    Octet 8...11  : Synthesizer frequency (Hz).
     *    Octet 12...15 : TS event frequency (Hz).
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i++] = (uint8)clock_index;
    payload[i++] = 0;

    _bcm_ptp_uint32_write(payload+i, synth_frequency);
    i += 4;
    _bcm_ptp_uint32_write(payload+i, tsevent_frequency);

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_OUTPUT_CLOCK_SYNTH_FREQUENCY,
            payload, PTP_MGMTMSG_PAYLOAD_OUTPUT_CLOCK_SYNTH_FREQUENCY_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /* Set host-maintained output clock frequencies. */
    OUTPUT_CLOCK(clock_index).frequency.synth = synth_frequency;
    OUTPUT_CLOCK(clock_index).frequency.tsevent = tsevent_frequency;
    OUTPUT_CLOCK(clock_index).frequency.tsevent_quotient = synth_frequency/tsevent_frequency;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_deriv_frequency_get()
 * Purpose:
 *      Get synthesizer derivative-clock frequency.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      stack_id        - (IN) Stack identifier index.
 *      clock_index     - (IN) Output clock index.
 *      deriv_frequency - (OUT) Derivative clock frequency (Hz).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_output_clock_deriv_frequency_get(
    int unit,
    int stack_id,
    int clock_index,
    uint32 *deriv_frequency)
{
    int i;
    int rv;

    uint32 synth_frequency;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_PAYLOAD_OUTPUT_CLOCK_DERIV_FREQUENCY_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_OUTPUT_CLOCK_DERIV_FREQUENCY_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_common_tdpll_output_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_output_clock_index_validate()");
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    /* Make indexed payload to get derivative-clock frequency(ies) for specified output clock. */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    payload[6] = (uint8)clock_index;
    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_OUTPUT_CLOCK_DERIV_FREQUENCY,
            payload, PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : Output clock index.
     *    Octet 7       : Reserved.
     *    Octet 8...11  : Derivative clock frequency (Hz).
     */
    i = 6; /* Advance cursor past custom management message identifier. */
    ++i;   /* Advance past output clock index. */
    ++i;   /* Advance past reserved octet. */

    *deriv_frequency = _bcm_ptp_uint32_read(resp + i);

    /* Set host-maintained output clock frequencies and ratios. */
    OUTPUT_CLOCK(clock_index).frequency.deriv = *deriv_frequency;

    synth_frequency = OUTPUT_CLOCK(clock_index).frequency.synth;
    OUTPUT_CLOCK(clock_index).frequency.deriv_quotient = *deriv_frequency ?
        (synth_frequency + (*deriv_frequency >> 1))/(*deriv_frequency) : -1;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_deriv_frequency_set()
 * Purpose:
 *      Set synthesizer derivative clock frequency.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      stack_id        - (IN) Stack identifier index.
 *      clock_index     - (IN) Output clock index.
 *      deriv_frequency - (IN) Derivative clock frequency (Hz).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_output_clock_deriv_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    uint32 deriv_frequency)
{
    int i;
    int rv;

    uint32 synth_frequency;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_OUTPUT_CLOCK_DERIV_FREQUENCY_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_common_tdpll_output_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_output_clock_index_validate()");
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    /*
     * CONSTRAINT: Derivative clock frequency and synthesizer frequency are
     *             integrally related, i.e. M = f_synth / f_deriv, such that
     *             derivative clock pulses occur every Mth synthesizer clock
     *             pulse.
     */
    synth_frequency = OUTPUT_CLOCK(clock_index).frequency.synth;
    if (deriv_frequency) {
        if ((0 == synth_frequency) || (synth_frequency % deriv_frequency)) {
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit,
                                    "SYNTH and DERIV frequencies are not integrally related. fSYNTH: %u fDERIV: %u"),
                         (unsigned)synth_frequency, (unsigned)deriv_frequency));
            return BCM_E_PARAM;
        }
    }

    /*
     * Make payload.
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : Output clock index.
     *    Octet 7       : Reserved.
     *    Octet 8...11  : Derivative-clock frequency (Hz).
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i++] = (uint8)clock_index;
    payload[i++] = 0;

    _bcm_ptp_uint32_write(payload+i, deriv_frequency);

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_OUTPUT_CLOCK_DERIV_FREQUENCY,
            payload, PTP_MGMTMSG_PAYLOAD_OUTPUT_CLOCK_DERIV_FREQUENCY_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /* Set host-maintained output clock frequencies. */
    OUTPUT_CLOCK(clock_index).frequency.deriv = deriv_frequency;

    OUTPUT_CLOCK(clock_index).frequency.deriv_quotient = deriv_frequency ?
        synth_frequency/deriv_frequency : -1;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_holdover_data_get()
 * Purpose:
 *      Get holdover configuration data.
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 *      hdata       - (OUT) Holdover configuration data.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_output_clock_holdover_data_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_holdover_data_t *hdata)
{
    int rv;
    int i;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_PAYLOAD_HOLDOVER_DATA_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_HOLDOVER_DATA_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_common_tdpll_output_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_output_clock_index_validate()");
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    /* Make indexed payload to get holdover data for specified output clock. */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    payload[6] = (uint8)clock_index;
    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_HOLDOVER_DATA,
            payload, PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : Output clock index.
     *    Octet 7       : Reserved.
     *    Octet 8...11  : Instantaneous holdover frequency (ppt).
     *    Octet 12...15 : 1s average holdover frequency (ppt).
     *    Octet 16...19 : Manual holdover frequency (ppt).
     *    Octet 20...23 : Fast-average holdover frequency (ppt).
     *    Octet 24...27 : Slow-average holdover frequency (ppt).
     *    Octet 28      : Fast-average valid Boolean.
     *    Octet 29      : Slow-average valid Boolean.
     *    Octet 30      : Holdover mode.
     *    Octet 31      : Reserved.
     */
    i = 6; /* Advance cursor past custom management message identifier. */
    ++i;   /* Advance past output clock index. */
    ++i;   /* Advance past reserved octet. */

    hdata->freq_instantaneous = (bcm_tdpll_frequency_correction_t)_bcm_ptp_uint32_read(resp + i);
    i += 4;
    hdata->freq_avg1s = (bcm_tdpll_frequency_correction_t)_bcm_ptp_uint32_read(resp + i);
    i += 4;
    hdata->freq_manual = (bcm_tdpll_frequency_correction_t)_bcm_ptp_uint32_read(resp + i);
    i += 4;
    hdata->freq_fast_average = (bcm_tdpll_frequency_correction_t)_bcm_ptp_uint32_read(resp + i);
    i += 4;
    hdata->freq_slow_average = (bcm_tdpll_frequency_correction_t)_bcm_ptp_uint32_read(resp + i);
    i += 4;
    hdata->freq_fast_average_valid = resp[i++] ? 1:0;
    hdata->freq_slow_average_valid = resp[i++] ? 1:0;
    hdata->mode = resp[i++];

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_holdover_frequency_set()
 * Purpose:
 *      Set manual holdover frequency correction.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 *      hfreq       - (IN) Holdover frequency correction (ppt).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Manual frequency correction is used if holdover mode is manual.
 */
int
bcm_common_tdpll_output_clock_holdover_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_frequency_correction_t hfreq)
{
    int rv;
    int i;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_HOLDOVER_FREQUENCY_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_common_tdpll_output_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_output_clock_index_validate()");
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    /*
     * Make payload.
     *    Octet 0...5  : Custom management message key/identifier.
     *                   BCM<null><null><null>.
     *    Octet 6      : Output clock index.
     *    Octet 7      : Reserved.
     *    Octet 8...11 : Holdover frequency (ppt).
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i++] = (uint8)clock_index;
    payload[i++] = 0;
    _bcm_ptp_uint32_write(payload + i, (uint32)hfreq);

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_HOLDOVER_FREQUENCY,
            payload, PTP_MGMTMSG_PAYLOAD_HOLDOVER_FREQUENCY_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_holdover_mode_get()
 * Purpose:
 *      Get holdover mode.
 *      |Manual|Instantaneous|One-Second|Fast Average|Slow Average|
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 *      hmode       - (OUT) Holdover mode.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_output_clock_holdover_mode_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_holdover_mode_t *hmode)
{
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_PAYLOAD_HOLDOVER_MODE_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_HOLDOVER_MODE_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_common_tdpll_output_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_output_clock_index_validate()");
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    /* Make indexed payload to get holdover mode for specified output clock. */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    payload[6] = (uint8)clock_index;
    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_HOLDOVER_MODE,
            payload, PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5 : Custom management message key/identifier.
     *                  BCM<null><null><null>.
     *    Octet 6     : Output clock index.
     *    Octet 7     : Holdover mode.
     */
    *hmode = resp[7];

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_holdover_mode_set()
 * Purpose:
 *      Set holdover mode.
 *      |Manual|Instantaneous|Fast Average|Slow Average|
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 *      hmode       - (IN) Holdover mode.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_output_clock_holdover_mode_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_holdover_mode_t hmode)
{
    int rv;
    int i;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_HOLDOVER_MODE_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_common_tdpll_output_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_output_clock_index_validate()");
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    /*
     * Make payload.
     *    Octet 0...5 : Custom management message key/identifier.
     *                  BCM<null><null><null>.
     *    Octet 6     : Output clock index.
     *    Octet 7     : Holdover mode.
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i++] = (uint8)clock_index;
    payload[i++] = (uint8)hmode;

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_HOLDOVER_MODE,
            payload, PTP_MGMTMSG_PAYLOAD_HOLDOVER_MODE_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_output_clock_holdover_reset()
 * Purpose:
 *      Reset holdover frequency calculations.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Output clock index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_output_clock_holdover_reset(
    int unit,
    int stack_id,
    int clock_index)
{
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_HOLDOVER_RESET_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_common_tdpll_output_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_output_clock_index_validate()");
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    /*
     * Make payload.
     *    Octet 0...5 : Custom management message key/identifier.
     *                  BCM<null><null><null>.
     *    Octet 6     : Output clock index.
     *    Octet 7     : Reserved.
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    payload[6] = (uint8)clock_index;

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_CMD, PTP_MGMTMSG_ID_HOLDOVER_RESET,
            payload, PTP_MGMTMSG_PAYLOAD_HOLDOVER_RESET_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    return BCM_E_NONE;
}

#if defined (BCM_MONTEREY_SUPPORT) && defined (INCLUDE_GDPLL)
extern int _bcm_esw_gdpll_holdover_set(int unit, int chan, uint8 enable);
extern int _bcm_esw_gdpll_holdover_get(int unit, int chan, uint8 *enable);
extern int _bcm_esw_gdpll_pbo_set(int unit, int chan);

void _bcm_common_tdpll_gdpll_holdover_get_set(int enable_ho, int gdpll_chan) {
    uint8 enabled = ~0;

    if (gdpll_chan == -1) {
        /*No gdpll channel. Nothing to do here*/
        return;
    }

    _bcm_esw_gdpll_holdover_get(0,gdpll_chan,&enabled);

    if (enable_ho != enabled) {
        cli_out("_bcm_common_tdpll_gdpll_holdover_get_set: enable_HO: %d, gdpll_chan: %d, enabled: %d\n", enable_ho, gdpll_chan, enabled);
        _bcm_esw_gdpll_holdover_set(0,gdpll_chan, enable_ho);
    }
}

void
_bcm_common_tdpll_output_holdover_set(int tdpll_outClk_index, int enable_ho)
{
    int gdpll_chan = -1;
    if (tdpll_outClk_index < BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC) {
        gdpll_chan = tdpllout_gdpll_state.gdpllChan_miscEvent[tdpll_outClk_index];
        _bcm_common_tdpll_gdpll_holdover_get_set(enable_ho, gdpll_chan);
    }  else if (tdpll_outClk_index < (BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC + BCM_TDPLL_OUTPUT_CLOCK_NUM_SYNCE)) {
        int port;
        for (port = 1; port <= BCM_GDPLL_NUM_PORTS; port++) {
            if(tdpllout_gdpll_state.tdpll_output_synceport[tdpll_outClk_index - BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC][port-1] == 1){
                gdpll_chan = tdpllout_gdpll_state.gdpllChan_synceport[port-1];
                _bcm_common_tdpll_gdpll_holdover_get_set(enable_ho, gdpll_chan);
            }
        }
    } else if (tdpll_outClk_index == (BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC + BCM_TDPLL_OUTPUT_CLOCK_NUM_SYNCE)) {
        gdpll_chan = tdpllout_gdpll_state.gdpllChan_miscEvent[tdpll_outClk_index];
        _bcm_common_tdpll_gdpll_holdover_get_set(enable_ho, gdpll_chan);
    } else if (tdpll_outClk_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) {
        /*GPIO. No gdpll channel configured*/
    }

}

void
_bcm_common_tdpll_output_pbo_set(int tdpll_outClk_index, int enable_ho)
{
    int gdpll_chan = -1;
    if (tdpll_outClk_index < BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC) {
        gdpll_chan = tdpllout_gdpll_state.gdpllChan_miscEvent[tdpll_outClk_index];
        _bcm_esw_gdpll_pbo_set(0, gdpll_chan);
    }  else if (tdpll_outClk_index < (BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC + BCM_TDPLL_OUTPUT_CLOCK_NUM_SYNCE)) {
        int port;
        for (port = 1; port <= BCM_GDPLL_NUM_PORTS; port++) {
            if(tdpllout_gdpll_state.tdpll_output_synceport[tdpll_outClk_index - BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC][port-1] == 1){
                gdpll_chan = tdpllout_gdpll_state.gdpllChan_synceport[port-1];
                _bcm_esw_gdpll_pbo_set(0, gdpll_chan);
            }
        }
    } else if (tdpll_outClk_index == (BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC + BCM_TDPLL_OUTPUT_CLOCK_NUM_SYNCE)) {
        gdpll_chan = tdpllout_gdpll_state.gdpllChan_miscEvent[tdpll_outClk_index];
        _bcm_esw_gdpll_pbo_set(0, gdpll_chan);
    } else if (tdpll_outClk_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) {
        /*GPIO. No gdpll channel configured*/
    }
}
#endif /*defined (BCM_MONTEREY_SUPPORT) && defined (INCLUDE_GDPLL)*/

#endif /* defined(INCLUDE_PTP) */
