/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
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

#if defined(BCM_MONTEREY_SUPPORT) & defined(INCLUDE_GDPLL)
#include <bcm/gdpll.h>
#endif

/* Definitions. */
#define TDPLL_OUTPUT_CLOCK_STATE_ENABLE_BIT                            (0u)

#define OUTPUT_CLOCK(clock_index)       \
(objdata.output_clock[clock_index])

/* Macros. */

/* Types. */

/* Constants and variables. */
static bcm_tdpll_output_clock_data_t objdata;

/* Static functions. */
#if defined(BCM_MONTEREY_SUPPORT)
#define OUTCLOCK_BS        (1)
#define OUTCLOCK_SYNCE     (2)
#define OUTCLOCK_1588      (3)
#define OUTCLOCK_GPIO      (4)

typedef struct tdpllout_gdpll_state_s {
    uint32 outClk_gdpllChan[TDPLL_OUTPUT_CLOCK_NUM_MAX];   /* Hold the I/A indicator of each instance */
    uint32 outClk_port[TDPLL_OUTPUT_CLOCK_NUM_MAX];
} tdpllout_gdpll_state_t;

tdpllout_gdpll_state_t tdpllout_gdpll_state;

uint32 gdpll_txpi_coeff_table[8][8] = {
/*                   Stage 0                             Stage 1
 *          K1, K1Shift, K1K2, K1K2Shift,      K1, K1Shift, K1K2, K1K2Shift */
        {720575940, 14,   737869763, 8,    910032812, 5,    15540, 2}, /* 0.01 Hz */
        {720575940, 14,   737869763, 8,    568770507, 8,    12140, 7}, /* 0.05 Hz */
        {688285981, 14,   831137788, 8,    568770507, 9,    3107903, 1}, /* 0.1 Hz */
        {720575940, 14,   737869763, 8,    710963134, 11,   2428049, 6}, /* 0.5 Hz */
        {720575940, 14,   737869763, 8,    710963134, 12,   621580576, 0}, /* 1 Hz */
        {688285981, 14,   831137788, 8,    710963134, 14,   621580576, 4}, /* 4 Hz */
        {720575940, 14,   737869763, 8,    888703918, 14,   971219650, 4}, /* 5 Hz */
        {720575940, 15,   737869763, 10,   888703918, 15,   971219650, 6} /* 10 Hz */
};

uint32 gdpll_ts_counter_coeff_table[8][8] = {
/*                   Stage 0                             Stage 1
 *          K1, K1Shift, K1K2, K1K2Shift,      K1, K1Shift, K1K2, K1K2Shift */
        {576460752, 2,  2305843, 4, 0,0, 0,0}, /* 0.01 Hz */
        {576460752, 2,  2305843, 4, 0,0, 0,0}, /* 0.05 Hz */
        {3441430, 9,  3324551, 3, 2221760, 10,   15175, 7}, /* 0.1 Hz */
        {576460752, 2,  2305843, 4, 2221760, 7, 15175, 1}, /* 0.5 Hz */
        {3441430, 9,  3324551, 3, 2221760, 10,   15175, 7}, /* 1 Hz */
        {3441430, 9,  3324551, 3, 2221760, 10,   15175, 7}, /* 4 Hz */
        {576460752, 2,  2305843, 4, 710963134, 2,   3035061, 0}, /* 5 Hz */
        {576460752, 3,  2305843, 6, 710963134, 3,   3035061, 2} /* 10 Hz */
};

#define OUTPUT_CLOCK_GDPLLCHAN(clock_index)       \
(tdpllout_gdpll_state.outClk_gdpllChan[clock_index])

int _bcm_esw_tdpllOutinstance_bind(int unit, int outclock_index, uint32 tdpll_instance_num);
extern int _bcm_esw_gdpll_tdpllinstance_bind(int unit, int gdpllChan, uint32 tdpll_instance_num);
extern int _bcm_common_tdpll_dpll_outclock_bandwidth_get(int clock_index, bcm_tdpll_dpll_bandwidth_t *bandwidth);
#endif


#if defined(BCM_MONTEREY_SUPPORT) & defined(INCLUDE_GDPLL)
int _bcm_esw_tdpllOutinstance_bind(int unit, int outclock_index, uint32 tdpll_instance_num)
{
    int rv = BCM_E_NONE;

    cli_out("### _bcm_esw_tdpllOutinstance_bind: outclock_index:%d, chan:%d, instance:%d\n",
        outclock_index, OUTPUT_CLOCK_GDPLLCHAN(outclock_index), tdpll_instance_num);

    rv = _bcm_esw_gdpll_tdpllinstance_bind(unit, OUTPUT_CLOCK_GDPLLCHAN(outclock_index), tdpll_instance_num);
    return rv;
}

int _bcm_esw_tdpll_outClk_portSet(int unit, int outclock_index, uint32 port)
{
    int rv = BCM_E_NONE;
    tdpllout_gdpll_state.outClk_port[outclock_index] = port;

    return rv;
}

void gdpll_chan_bandwidth_dependent_config(bcm_gdpll_chan_t *pGdpllChan, int outClock, bcm_tdpll_dpll_bandwidth_t bw)
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


    switch (bw.units) {
    case bcm_tdpll_dpll_bandwidth_mHz:
        switch (bw.value) {
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
        switch (bw.value) {
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

    cli_out("bw_index = %d\n", bw_index);
    if (outClock == OUTCLOCK_SYNCE) {
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
void gdpll_chan_config(int unit, bcm_gdpll_chan_t *pGdpllChan, int outClock, bcm_tdpll_dpll_bandwidth_t bw)
{
    gdpll_chan_bandwidth_dependent_config(pGdpllChan, outClock, bw);
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
        pGdpllChan->chan_dpll_config.norm_phase_error_thres0=16000000;
        pGdpllChan->chan_dpll_config.norm_phase_error_thres1=8100;

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
        pGdpllChan->chan_dpll_config.norm_phase_error=0;
        pGdpllChan->chan_dpll_config.norm_phase_error_thres0=1600000000;
        pGdpllChan->chan_dpll_config.norm_phase_error_thres1=1600000000;

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
        u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta)=64000000;
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
#if defined(BCM_MONTEREY_SUPPORT)
        OUTPUT_CLOCK_GDPLLCHAN(i) = -1;
        tdpllout_gdpll_state.outClk_port[i] = -1;
#endif
    }

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

#if defined(BCM_MONTEREY_SUPPORT) & defined(INCLUDE_GDPLL)
    uint32 flags = BCM_GDPLL_CONF_DPLL |
                   BCM_GDPLL_EVENT_CONFIG_REF | BCM_GDPLL_EVENT_CONFIG_FB |
                   BCM_GDPLL_CHAN_ALLOC | BCM_GDPLL_CHAN_SET_PRIORITY |
                   BCM_GDPLL_CHAN_OUTPUT_CONF;
    int result=0, gdpllChan=0;
    bcm_gdpll_chan_t    gdpll_chan;
    bcm_tdpll_dpll_bandwidth_t bandwidth;

    sal_memset(&gdpll_chan, 0, sizeof(bcm_gdpll_chan_t));
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

#if defined(BCM_MONTEREY_SUPPORT) & defined(INCLUDE_GDPLL)
    if (clock_index < BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC) {
        
        cli_out("### tdpll_output_clock: Set BS%d\n", clock_index);

    } else if (clock_index < (BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC + BCM_TDPLL_OUTPUT_CLOCK_NUM_SYNCE)) {
        cli_out("### tdpll_output_clock: Output clock is SyncE: clock_index:%d, port:%d divisor:%d\n",
           clock_index, tdpllout_gdpll_state.outClk_port[clock_index],
           OUTPUT_CLOCK(clock_index).frequency.tsevent_quotient/2);

        /* Feedback config */
        gdpll_chan.event_fb.input_event     = bcmGdpllInputEventPORT;
        gdpll_chan.event_fb.port            = tdpllout_gdpll_state.outClk_port[clock_index];
        gdpll_chan.event_fb.port_event_type = bcmGdpllPortEventTXPI;
        gdpll_chan.event_fb.event_divisor   = OUTPUT_CLOCK(clock_index).frequency.tsevent_quotient/2;
        gdpll_chan.event_fb.event_dest      = bcmGdpllEventDestM7;
        gdpll_chan.event_fb.ts_counter      = 0; /* TS0 */

        /* Reference config */
        gdpll_chan.event_ref.input_event    = bcmGdpllInputEventR5;
        gdpll_chan.event_ref.event_dest     = bcmGdpllEventDestCPU;
        gdpll_chan.event_ref.event_divisor  = 1;

        gdpll_chan.chan_prio    = 0;
        gdpll_chan.out_event    = bcmGdpllOutputEventTXPI;
        gdpll_chan.port         = gdpll_chan.event_fb.port;
        gdpll_chan.ts_pair_dest = bcmGdpllDebugDestM7;

        /* Set phase counter reference */
        gdpll_chan.chan_dpll_config.phase_counter_ref = 0;

        if (BCM_FAILURE(rv = _bcm_common_tdpll_dpll_outclock_bandwidth_get(clock_index, &bandwidth))) {
            PTP_ERROR_FUNC("_bcm_common_tdpll_dpll_outclock_bandwidth_get()");
            return rv;
        }
        gdpll_chan_config(unit, &gdpll_chan, OUTCLOCK_SYNCE, bandwidth);

    } else if (clock_index == (BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC + BCM_TDPLL_OUTPUT_CLOCK_NUM_SYNCE)) {
        cli_out("### tdpll_output_clock_enable_set: Output clock is 1588-Hybrid\n");

        /* Reference config */
        gdpll_chan.event_fb.input_event    = bcmGdpllInputEventR5;
        gdpll_chan.event_fb.event_dest     = bcmGdpllEventDestCPU;
        gdpll_chan.event_fb.event_divisor  = 1;

        /* Channel output configurations */
        gdpll_chan.chan_prio    = 0;
        gdpll_chan.out_event    = bcmGdpllOutputEventTS0;
        gdpll_chan.ts_pair_dest = bcmGdpllDebugDestM7;
        gdpll_chan.event_fb.ts_counter = 0; /* TS0 */

        /* Set phase counter reference */
        gdpll_chan.chan_dpll_config.phase_counter_ref = 1;

        if (BCM_FAILURE(rv = _bcm_common_tdpll_dpll_outclock_bandwidth_get(clock_index, &bandwidth))) {
            PTP_ERROR_FUNC("_bcm_common_tdpll_dpll_outclock_bandwidth_get()");
            return rv;
        }

        /* DPLL config for 1588 */
        gdpll_chan_config(unit, &gdpll_chan, OUTCLOCK_1588, bandwidth);


    } else if (clock_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) {
        cli_out("### tdpll_output_clock_enable_set: Output clock is GPIO\n");
    }

    result = bcm_gdpll_chan_create (unit, flags, &gdpll_chan, &gdpllChan);
    if (BCM_FAILURE(result)) {
        cli_out("### tdpll_output_clock_enable_set: Error!! gdpll chan create failed\n");
    }

    if (result == BCM_E_NONE) {
        result = bcm_gdpll_chan_enable(unit, gdpllChan, 1);
        if (BCM_FAILURE(result)) {
            cli_out("### tdpll_output_clock_enable_set: Error!! gdpll chan enable failed\n");
            return result;
        }
        OUTPUT_CLOCK_GDPLLCHAN(clock_index) = gdpllChan;
    }

    cli_out("### gdpll_chan:%d\n", gdpllChan);
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

#endif /* defined(INCLUDE_PTP) */
