/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    metrics.c
 *
 * Purpose: 
 *
 * Functions:
 *      bcm_common_ptp_ctdev_alarm_callback_register
 *      bcm_common_ptp_ctdev_alarm_callback_unregister
 *      bcm_common_ptp_ctdev_alpha_get
 *      bcm_common_ptp_ctdev_alpha_set
 *      bcm_common_ptp_ctdev_enable_get
 *      bcm_common_ptp_ctdev_enable_set
 *      bcm_common_ptp_ctdev_verbose_get
 *      bcm_common_ptp_ctdev_verbose_set
 *
 *      _bcm_ptp_ctdev_init
 *      _bcm_ptp_ctdev_gateway
 *      _bcm_ptp_ctdev_phase_accumulator
 *      _bcm_ptp_ctdev_calculator
 *      _bcm_ptp_ctdev_get
 *      _bcm_ptp_llu_div
 *      _bcm_ptp_llu_isqrt
 *      _bcm_ptp_xorshift_rand
 *      _bcm_ptp_ctdev_g823_mask
 *      _bcm_ptp_circular_buffer_init
 *      _bcm_ptp_circular_buffer_free
 *      _bcm_ptp_circular_buffer_write
 *      _bcm_ptp_circular_buffer_read
 *      _bcm_ptp_circular_buffer_peekn
 */

#if defined(INCLUDE_PTP)

#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/ptp_common.h>
#include <bcm/error.h>
#include <shared/bsl.h>
/* Definitions. */
#define PTP_CTDEV_PHASE_ERROR_THRESHOLD_PSEC                   (1000000000) /* 1 msec */
#define PTP_CTDEV_FIXEDPOINT_BIT_MAX                                   (15)

#define PTP_CTDEV_TAU0_SEC                                              (1)
#define PTP_CTDEV_NUM_TIMESCALES                                       (11)

#define PTP_CTDEV_ALPHA_NUMERATOR                                      (63)
#define PTP_CTDEV_ALPHA_DENOMINATOR                                    (64)

#define PTP_CTDEV_KEFF_NUMERATOR              (PTP_CTDEV_ALPHA_DENOMINATOR)
#define PTP_CTDEV_KEFF_DENOMINATOR            (PTP_CTDEV_ALPHA_DENOMINATOR - \
                                               PTP_CTDEV_ALPHA_NUMERATOR)

#define PTP_CTDEV_MUTEX_TIMEOUT_USEC                              (1000000)

/* Macros. */
#define PTP_CTDEV_MUTEX_TAKE() \
    PTP_MUTEX_TAKE(ctdev_mutex, PTP_CTDEV_MUTEX_TIMEOUT_USEC)

#define PTP_CTDEV_MUTEX_RELEASE_RETURN(__rv__) \
    PTP_MUTEX_RELEASE_RETURN(ctdev_mutex, __rv__)

/* Types. */
typedef struct eltype_s {
    int32 value;
} eltype_t;

typedef struct _bcm_ptp_circular_buffer_s {
    int num_el;
    int num_read;
    int num_write;

    eltype_t* data;
    eltype_t* write_el;
    eltype_t* read_el;
} _bcm_ptp_circular_buffer_t;

typedef struct _bcm_ptp_ctdev_s {
    /* Timescale. */
    int tau;
    int m;

    int64 dS;
    uint64 dSsq;
    uint64 Sov;

    /* TVAR incl. fixed-point scaling. */
    uint64 tvar;
    /* TDEV incl. fixed-point scaling. */
    uint32 tdev;
    /* TDEV (picoseconds). */
    uint64 tdev_psec;

    /* Dynamic re-scaling fixed-point bit. */
    uint32 fpbit;
} _bcm_ptp_ctdev_t;

typedef struct _bcm_ptp_ctdev_array_s {
    _bcm_ptp_ctdev_t entry[PTP_CTDEV_NUM_TIMESCALES];
    _bcm_ptp_circular_buffer_t phase_buffer;
    _bcm_ptp_circular_buffer_t wrap_buffer;
} _bcm_ptp_ctdev_array_t;

typedef struct _bcm_ptp_ctdev_options_s {
    int enable;
    int dynamic_rescale;
    int verbose;
} _bcm_ptp_ctdev_options_t;

typedef struct _bcm_ptp_ctdev_parameters_s {
    uint16 alpha_numerator;
    uint16 alpha_denominator;
    uint16 keff_numerator;
    uint16 keff_denominator;
} _bcm_ptp_ctdev_parameters_t;

/* Macros. */

/*
 * 64-BIT MATH: Inplace division.
 * Unsigned 64-bit dividend, unsigned 16-bit divisor.
 * NOTE: Macro is syntactically similar to SDK's COMPILER_64_*** operations.
 */
#define COMPILER_64_UDIV_16(dst, src)                                              \
    do {                                                                           \
        int __el__;                                                                \
        uint16 __numh__[4];  /* Numerator as 16-bit half words. */                 \
        uint16 __qh__[4];    /* Quotient as 16-bit half-words. */                  \
        uint16 __rhat__ = 0; /* Remainder. */                                      \
                                                                                   \
        uint32 __Ql__;                                                             \
        uint32 __Qh__;                                                             \
                                                                                   \
        __numh__[0] = ((uint16)(COMPILER_64_LO(dst) & 0xffff));                    \
        __numh__[1] = ((uint16)(COMPILER_64_LO(dst) >> 16));                       \
        __numh__[2] = ((uint16)(COMPILER_64_HI(dst) & 0xffff));                    \
        __numh__[3] = ((uint16)(COMPILER_64_HI(dst) >> 16));                       \
                                                                                   \
        for (__el__ = 3; __el__ >= 0; --__el__) {                                  \
            __qh__[__el__] = (__rhat__*65536 + __numh__[__el__])/(src);            \
            __rhat__ = (__rhat__*65536 + __numh__[__el__]) - __qh__[__el__]*(src); \
        }                                                                          \
                                                                                   \
        __Ql__ = __qh__[0] + (__qh__[1] << 16);                                    \
        __Qh__ = __qh__[2] + (__qh__[3] << 16);                                    \
        COMPILER_64_SET(dst, __Qh__, __Ql__);                                      \
    } while (0)

/*
 * 64-BIT MATH: Integer square root.
 * Unsigned 64-bit argument, unsigned 32-bit result.
 * NOTE: Macro is syntactically similar to SDK's COMPILER_64_*** operations.
 */
#define COMPILER_64_ISQRT(dst, src)                                                \
    do {                                                                           \
        uint64 __root__;                                                           \
        uint64 __remainder__;                                                      \
        uint64 __place__;                                                          \
        uint64 __condval__;                                                        \
                                                                                   \
        __remainder__ = src;                                                       \
        COMPILER_64_ZERO(__root__);                                                \
        COMPILER_64_SET(__place__, 0x40000000, 0);                                 \
                                                                                   \
        while (COMPILER_64_GT(__place__, __remainder__)) {                         \
            COMPILER_64_UDIV_16(__place__, 4);                                     \
        }                                                                          \
                                                                                   \
        while (!COMPILER_64_IS_ZERO(__place__)) {                                  \
            __condval__ = __place__;                                               \
            COMPILER_64_ADD_64(__condval__, __root__);                             \
            if (COMPILER_64_GE(__remainder__, __condval__)) {                      \
                COMPILER_64_SUB_64(__remainder__, __condval__);                    \
                COMPILER_64_ADD_64(__root__, __place__);                           \
                COMPILER_64_ADD_64(__root__, __place__);                           \
            }                                                                      \
            COMPILER_64_UDIV_16(__root__, 2);                                      \
            COMPILER_64_UDIV_16(__place__, 4);                                     \
        }                                                                          \
        dst = COMPILER_64_LO(__root__);                                            \
    } while (0)

/* Constants and variables. */
static _bcm_ptp_mutex_t ctdev_mutex = 0x0;

static int32 phase_err_psec;
static int32 wrap_counter;

static const int64 zero64 = COMPILER_64_INIT(0,0);

static const _bcm_ptp_ctdev_t ctdev_default;
static _bcm_ptp_ctdev_array_t ctdev_array;

static _bcm_ptp_ctdev_options_t ctdev_options = {
    0,
    1,
    0
};

static uint32 ctdev_flags;

static _bcm_ptp_ctdev_parameters_t ctdev_parameters = {
    PTP_CTDEV_ALPHA_NUMERATOR,
    PTP_CTDEV_ALPHA_DENOMINATOR,
    PTP_CTDEV_KEFF_NUMERATOR,
    PTP_CTDEV_KEFF_DENOMINATOR
};

static int ctdev_memory_init;

static bcm_ptp_ctdev_alarm_cb ctdev_alarm_fcn = NULL;
static bcm_ptp_ctdev_alarm_data_t ctdev_alarm_data;

/* Static functions. */
static void _bcm_ptp_circular_buffer_init(_bcm_ptp_circular_buffer_t *buffer, int num_el);
static void _bcm_ptp_circular_buffer_free(_bcm_ptp_circular_buffer_t *buffer);

static void _bcm_ptp_circular_buffer_write(_bcm_ptp_circular_buffer_t *buffer, eltype_t *value);
static void _bcm_ptp_circular_buffer_read(_bcm_ptp_circular_buffer_t *buffer, eltype_t *value);

#if 0  /* Unused. Preserve for future use. */
static void _bcm_ptp_circular_buffer_peek(_bcm_ptp_circular_buffer_t *buffer, eltype_t *value);
#endif /* 0 */
static void _bcm_ptp_circular_buffer_peekn(_bcm_ptp_circular_buffer_t *buffer, int n, eltype_t *value);


/*
 * Function:
 *      _bcm_ptp_ctdev_init
 * Purpose:
 *      Initialize the continuous time deviation (C-TDEV) metric.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      p         - (IN) C-TDEV algorithm control parameters. (UNUSED)
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_ctdev_init(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 p)
{
    int i;

    if (!ctdev_mutex) {
        ctdev_mutex = _bcm_ptp_mutex_create("ctdev_mutex");
    }

    PTP_CTDEV_MUTEX_TAKE();

    /* Memory management. */
    if (ctdev_memory_init) {
        _bcm_ptp_circular_buffer_free(&ctdev_array.phase_buffer);
        _bcm_ptp_circular_buffer_free(&ctdev_array.wrap_buffer);
    }
    ctdev_memory_init = 1;

    _bcm_ptp_circular_buffer_init(&ctdev_array.phase_buffer,
        1 + 3*(1 << (PTP_CTDEV_NUM_TIMESCALES -1)));
    phase_err_psec = 0;

    _bcm_ptp_circular_buffer_init(&ctdev_array.wrap_buffer,
        1 + 3*(1 << (PTP_CTDEV_NUM_TIMESCALES -1)));
    wrap_counter = 0;

    for (i = 0; i < PTP_CTDEV_NUM_TIMESCALES; ++i) {
        ctdev_array.entry[i] = ctdev_default;

        /* Set timescale. */
        ctdev_array.entry[i].m = 1 << i;
        ctdev_array.entry[i].tau = ctdev_array.entry[i].m * PTP_CTDEV_TAU0_SEC;

        /* Initialize dynamic re-scaling fixed-point bit. */
        ctdev_array.entry[i].fpbit = 0;
    }

    /* Restore default algorithm parameters. */
    ctdev_parameters.alpha_numerator = PTP_CTDEV_ALPHA_NUMERATOR;
    ctdev_parameters.alpha_denominator = PTP_CTDEV_ALPHA_DENOMINATOR;
    ctdev_parameters.keff_numerator = PTP_CTDEV_KEFF_NUMERATOR;
    ctdev_parameters.keff_denominator = PTP_CTDEV_KEFF_DENOMINATOR;

    /* Reset elapsed data time (duration of frequency correction data processed). */
    ctdev_alarm_data.elapsed_sec = 0;

    PTP_CTDEV_MUTEX_RELEASE_RETURN(BCM_E_NONE);
}

/*
 * Function:
 *      bcm_common_ptp_ctdev_alarm_callback_register
 * Purpose:
 *      Register a C-TDEV alarm callback function.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      alarm_cb  - (IN) C-TDEV alarm callback function pointer.
 * Returns:
 *      BCM_E_XXX - Function status.
 */
int
bcm_common_ptp_ctdev_alarm_callback_register(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_ctdev_alarm_cb alarm_cb)
{
    int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    ctdev_alarm_fcn = alarm_cb;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_ctdev_alarm_callback_unregister
 * Purpose:
 *      Unregister a C-TDEV alarm callback function.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 * Returns:
 *      BCM_E_XXX - Function status.
 */
int
bcm_common_ptp_ctdev_alarm_callback_unregister(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    ctdev_alarm_fcn = NULL;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_ctdev_enable_get
 * Purpose:
 *      Get enable/disable state of C-TDEV processing.
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      ptp_id      - (IN)  PTP stack ID.
 *      clock_num   - (IN)  PTP clock number.
 *      enable      - (OUT) Enable Boolean.
 *      flags       - (OUT) C-TDEV control flags. (UNUSED)
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_ctdev_enable_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *enable,
    uint32 *flags)
{
    int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    *enable = ctdev_options.enable;
    *flags = ctdev_flags;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_ctdev_enable_set
 * Purpose:
 *      Set enable/disable state of C-TDEV processing.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      ptp_id      - (IN) PTP stack ID.
 *      clock_num   - (IN) PTP clock number.
 *      enable      - (IN) Enable Boolean.
 *      flags       - (IN) C-TDEV control flags. (UNUSED)
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_ctdev_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int enable,
    uint32 flags)
{
    int rv;
    uint32 p = 0;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    /* Reset C-TDEV algorithm on disable.*/
    if (0 == enable) {
        if (BCM_FAILURE(rv = _bcm_ptp_ctdev_init(unit, ptp_id, clock_num, p))) {
            return rv;
        }
    }

    ctdev_options.enable = enable;
    ctdev_flags = flags;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_ctdev_verbose_get
 * Purpose:
 *      Get verbose program control option of C-TDEV processing.
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      ptp_id      - (IN)  PTP stack ID.
 *      clock_num   - (IN)  PTP clock number.
 *      verbose     - (OUT) Verbose Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_ctdev_verbose_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *verbose)
{
    int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    *verbose = ctdev_options.verbose;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_ctdev_verbose_set
 * Purpose:
 *      Set verbose program control option of C-TDEV processing.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      ptp_id      - (IN) PTP stack ID.
 *      clock_num   - (IN) PTP clock number.
 *      verbose     - (IN) Verbose Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_ctdev_verbose_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int verbose)
{
    int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    ctdev_options.verbose = verbose;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_ctdev_alpha_get
 * Purpose:
 *      Get C-TDEV recursive algorithm forgetting factor (alpha).
 * Parameters:
 *      unit              - (IN)  Unit number.
 *      ptp_id            - (IN)  PTP stack ID.
 *      clock_num         - (IN)  PTP clock number.
 *      alpha_numerator   - (OUT) Forgetting factor numerator.
 *      alpha_denominator - (OUT) Forgetting factor denominator.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_ctdev_alpha_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 *alpha_numerator,
    uint16 *alpha_denominator)
{
    int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    *alpha_numerator = ctdev_parameters.alpha_numerator;
    *alpha_denominator = ctdev_parameters.alpha_denominator;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_ctdev_alpha_set
 * Purpose:
 *      Set C-TDEV recursive algorithm forgetting factor (alpha).
 * Parameters:
 *      unit              - (IN) Unit number.
 *      ptp_id            - (IN) PTP stack ID.
 *      clock_num         - (IN) PTP clock number.
 *      alpha_numerator   - (IN) Forgetting factor numerator.
 *      alpha_denominator - (IN) Forgetting factor denominator.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_ctdev_alpha_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 alpha_numerator,
    uint16 alpha_denominator)
{
    int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (alpha_numerator >= alpha_denominator) {
        /* alpha >= 1 numerically unstable by definition. */
        return BCM_E_PARAM;
    }

    ctdev_parameters.alpha_numerator = alpha_numerator;
    ctdev_parameters.alpha_denominator = alpha_denominator;

    ctdev_parameters.keff_numerator = ctdev_parameters.alpha_denominator;
    ctdev_parameters.keff_denominator =
        (ctdev_parameters.alpha_denominator - ctdev_parameters.alpha_numerator);

    LOG_CLI((BSL_META_U(unit,
                        "bcm_common_ptp_ctdev_alpha_set(): ALPHA = %u/%u\n"),
             (unsigned)ctdev_parameters.alpha_numerator,
             (unsigned)ctdev_parameters.alpha_denominator));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_ctdev_gateway
 * Purpose:
 *      Gateway to update C-TDEV metric with event message data from PTP stack
 *      that includes 1s-average frequency correction measurement(s).
 * Parameters:
 *      unit        - (IN) Unit number.
 *      ptp_id      - (IN) PTP stack ID.
 *      clock_num   - (IN) PTP clock number.
 *      ev_data_len - (IN) TDEV event message data length (octets).
 *      ev_data     - (IN) TDEV event message data.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_ctdev_gateway(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int ev_data_len,
    uint8 *ev_data)
{
    int rv;
    int i;

    int cursor = 0;
    int numel;
    int64 freq_corr_ppt;

    bcm_ptp_ctdev_alarm_data_t ctdev_alarm_data_copy;

    if (0 == ctdev_options.enable) {
        /* C-TDEV disabled. */
        if (ctdev_options.verbose) {
            LOG_CLI((BSL_META_U(unit,
                                "_bcm_ptp_ctdev_gateway(): C-TDEV disabled.\n")));
        }
        return BCM_E_NONE;
    }

    /*
     * Advance cursor to data.
     * Higher-level callers vetted event type.
     * Only one TDEV event subtype at present.
     *
     * MESSAGE FORMAT:
     *     Octet 0:1 : Event type (TDEV event).
     *     Octet 2   : TDEV event subtype (frequency correction data).
     *     Octet 3   : Number of frequency correction data elements.
     *     Octet 4+8(N-1):4+8*N: Frequency correction entry N, parts per trillion.
     */
    cursor = 3;
    numel = ev_data[cursor++];

    while (numel--) {
        freq_corr_ppt = _bcm_ptp_int64_read(ev_data+cursor);
        cursor += sizeof(int64);

        if (BCM_FAILURE(rv = _bcm_ptp_ctdev_phase_accumulator(unit, ptp_id,
                clock_num, freq_corr_ppt))) {
            PTP_ERROR_FUNC("_bcm_ptp_ctdev_phase_accumulator()");
            return rv;
        }
        if (BCM_FAILURE(rv = _bcm_ptp_ctdev_calculator(unit, ptp_id, clock_num))) {
            PTP_ERROR_FUNC("_bcm_ptp_ctdev_calculator()");
            return rv;
        }

        /* C-TDEV alarm. */
        ctdev_alarm_data.elapsed_sec += PTP_CTDEV_TAU0_SEC;
        if (ctdev_alarm_fcn) {
            COMPILER_64_SET(ctdev_alarm_data.freq_corr_ppt, COMPILER_64_HI(freq_corr_ppt), COMPILER_64_LO(freq_corr_ppt));
            /* ctdev_alarm_data.freq_corr_ppt = freq_corr_ppt; */
            ctdev_alarm_data.num_tau = PTP_CTDEV_NUM_TIMESCALES;
            for (i = 0; i < PTP_CTDEV_NUM_TIMESCALES; ++i) {
                ctdev_alarm_data.tau_sec[i] = ctdev_array.entry[i].tau;
                ctdev_alarm_data.tdev_psec[i] = ctdev_array.entry[i].tdev_psec;
            }
            /* Pass copy of alarm data to protect core data reused in recursive calculations. */
            ctdev_alarm_data_copy = ctdev_alarm_data;
            ctdev_alarm_fcn(unit, ptp_id, clock_num, &ctdev_alarm_data_copy);
        }

        if (ctdev_options.verbose) {
            /*
             * Display comma-delimited frequency correction and C-TDEV(tau).
             * NOTE: Printout assumes 12 timescales  are enabled/calculated.
             */
            /*
            LOG_CLI((BSL_META_U(unit,
                                "%u,%lld,%llu,%llu,%llu,%llu,%llu,%llu,"
                                "%llu,%llu,%llu,%llu,%llu,%llu\n"),
                     (unsigned)sal_time(), freq_corr_ppt,
                     (long long unsigned)ctdev_array.entry[0].tdev_psec,
                     (long long unsigned)ctdev_array.entry[1].tdev_psec,
                     (long long unsigned)ctdev_array.entry[2].tdev_psec,
                     (long long unsigned)ctdev_array.entry[3].tdev_psec,
                     (long long unsigned)ctdev_array.entry[4].tdev_psec,
                     (long long unsigned)ctdev_array.entry[5].tdev_psec,
                     (long long unsigned)ctdev_array.entry[6].tdev_psec,
                     (long long unsigned)ctdev_array.entry[7].tdev_psec,
                     (long long unsigned)ctdev_array.entry[8].tdev_psec,
                     (long long unsigned)ctdev_array.entry[9].tdev_psec,
                     (long long unsigned)ctdev_array.entry[10].tdev_psec,
                     (long long unsigned)ctdev_array.entry[11].tdev_psec));
             */
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_ctdev_phase_accumulator
 * Purpose:
 *      Update C-TDEV phase accumulator with frequency correction data.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      ptp_id        - (IN) PTP stack ID.
 *      clock_num     - (IN) PTP clock number.
 *      freq_corr_ppt - (IN) Frequency correction (parts per trillion).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_ctdev_phase_accumulator(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    const int64 freq_corr_ppt)
{
    eltype_t qval;

    if (0 == ctdev_options.enable) {
        /* C-TDEV disabled. */
        if (ctdev_options.verbose) {
            LOG_CLI((BSL_META_U(unit,
                                "_bcm_ptp_ctdev_phase_accumulator(): "
                                "C-TDEV disabled.\n")));
        }
        return BCM_E_NONE;
    }

    PTP_CTDEV_MUTEX_TAKE();

    /* Phase accumulator incl. wrapping. */
    phase_err_psec += COMPILER_64_LO(freq_corr_ppt)*PTP_CTDEV_TAU0_SEC;
    if (phase_err_psec > PTP_CTDEV_PHASE_ERROR_THRESHOLD_PSEC) {
        phase_err_psec -= (PTP_CTDEV_PHASE_ERROR_THRESHOLD_PSEC << 1);
        ++wrap_counter;
    }
    if (phase_err_psec < -PTP_CTDEV_PHASE_ERROR_THRESHOLD_PSEC) {
        phase_err_psec += (PTP_CTDEV_PHASE_ERROR_THRESHOLD_PSEC << 1);
        --wrap_counter;
    }

    /* Enqueue. */
    qval.value = phase_err_psec;
    _bcm_ptp_circular_buffer_write(&ctdev_array.phase_buffer, &qval);

    qval.value = wrap_counter;
    _bcm_ptp_circular_buffer_write(&ctdev_array.wrap_buffer, &qval);

    PTP_CTDEV_MUTEX_RELEASE_RETURN(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_ptp_ctdev_calculator
 * Purpose:
 *      Calculate C-TDEV metric over relevant timescales.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_ctdev_calculator(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int i;

    eltype_t pval;
    eltype_t qval;
    eltype_t wval;
    int32 dA;
    int32 dB;
    int32 dw;

    uint32 Sovh, dSl, dSh;
    uint32 fpbit_prior, fpbit_maxSov;

    _bcm_ptp_ctdev_t *ctdev;

    pval.value = 0;

    if (0 == ctdev_options.enable) {
        /* C-TDEV disabled. */
        if (ctdev_options.verbose) {
            LOG_CLI((BSL_META_U(unit,
                                "_bcm_ptp_ctdev_calculator(): C-TDEV disabled.\n")));
        }
        return BCM_E_NONE;
    }

    PTP_CTDEV_MUTEX_TAKE();

    /* Dequeue. */
    _bcm_ptp_circular_buffer_read(&ctdev_array.phase_buffer, &qval);
    _bcm_ptp_circular_buffer_read(&ctdev_array.wrap_buffer, &wval);

    for (i = 0; i < PTP_CTDEV_NUM_TIMESCALES; ++i) {
        /* Set active C-TDEV(tau). */
        ctdev = &ctdev_array.entry[i];

        /*
         * Calculate S.
         * S = S + 3 dA + dB.
         *
         * NB: S is a second derivative of the phase deviations. C-TDEV calculates
         *     S via accumulation of O(1) backward finite difference approximations
         *     of the third derivative.
         */

        /* Assemble difference terms (dA,dB). */
        _bcm_ptp_circular_buffer_peekn(&ctdev_array.phase_buffer, 2*ctdev->m, &pval);
        dA = pval.value;
        _bcm_ptp_circular_buffer_peekn(&ctdev_array.phase_buffer, ctdev->m, &pval);
        dA -= pval.value;

        _bcm_ptp_circular_buffer_peekn(&ctdev_array.phase_buffer, 3*ctdev->m, &pval);
        dB = qval.value - pval.value;

        COMPILER_64_ADD_32(ctdev->dS, dA);
        COMPILER_64_ADD_32(ctdev->dS, dA);
        COMPILER_64_ADD_32(ctdev->dS, dA);
        COMPILER_64_ADD_32(ctdev->dS, dB);

        /* Account for phase wrapping effects in (dA,dB) terms. */
        dw = wval.value;
        _bcm_ptp_circular_buffer_peekn(&ctdev_array.wrap_buffer, ctdev->m, &pval);
        dw -= (3*pval.value);
        _bcm_ptp_circular_buffer_peekn(&ctdev_array.wrap_buffer, 2*ctdev->m, &pval);
        dw += (3*pval.value);
        _bcm_ptp_circular_buffer_peekn(&ctdev_array.wrap_buffer, 3*ctdev->m, &pval);
        dw -= pval.value;

        if (dw > 0) {
            while (dw--) {
                COMPILER_64_ADD_32(ctdev->dS, (PTP_CTDEV_PHASE_ERROR_THRESHOLD_PSEC << 1));
            }
        } else if (dw < 0) {
            while (dw++) {
                COMPILER_64_SUB_32(ctdev->dS, (PTP_CTDEV_PHASE_ERROR_THRESHOLD_PSEC << 1));
            }
        }

        /* Calculate S^2. */
        COMPILER_64_ZERO(ctdev->dSsq);
        if (COMPILER_64_GE(ctdev->dS, zero64)) {
            COMPILER_64_ADD_64(ctdev->dSsq, ctdev->dS);
        } else {
            COMPILER_64_SUB_64(ctdev->dSsq, ctdev->dS);
        }
        /* NB: S^2 equals (S/tau)^2 in reference algorithm. */
        COMPILER_64_UDIV_16(ctdev->dSsq, ctdev->tau);

        /* Dynamic re-scaling. */
        if (1 == ctdev_options.dynamic_rescale) {
            fpbit_prior = ctdev->fpbit;
            ctdev->fpbit = 0;
            fpbit_maxSov = 31 - (fpbit_prior << 1);
            Sovh = COMPILER_64_HI(ctdev->Sov);
            dSl = COMPILER_64_LO(ctdev->dSsq);
            dSh = COMPILER_64_HI(ctdev->dSsq);
            while (Sovh > (1 << fpbit_maxSov) || (dSl > (1 << 31)) || (dSh > 0)) {
                if (++ctdev->fpbit >= PTP_CTDEV_FIXEDPOINT_BIT_MAX) {
                    break;
                }
                if (dSh == 0) {
                    dSl >>= 1;
                }
                dSh >>= 1;
                Sovh >>= 2;
            }

            if (ctdev->fpbit) {
                COMPILER_64_UDIV_16(ctdev->dSsq, (1 << ctdev->fpbit));
            }
            if (ctdev->fpbit) {
                COMPILER_64_UDIV_16(ctdev->Sov, (1 << ctdev->fpbit));
            }
            if (fpbit_prior) {
                COMPILER_64_UMUL_32(ctdev->Sov, (1 << fpbit_prior));
            }
            if (ctdev->fpbit) {
                COMPILER_64_UDIV_16(ctdev->Sov, (1 << ctdev->fpbit));
            }
            if (fpbit_prior) {
                COMPILER_64_UMUL_32(ctdev->Sov, (1 << fpbit_prior));
            }
        }

        if (COMPILER_64_HI(ctdev->dSsq)) {
            /* Overflow. */
            PTP_CTDEV_MUTEX_RELEASE_RETURN(BCM_E_PARAM);
        }
        COMPILER_64_UMUL_32(ctdev->dSsq, COMPILER_64_LO(ctdev->dSsq));

        /*
         * Calculate Sov.
         * Sov = alpha Sov + S^2.
         */
        COMPILER_64_UDIV_16(ctdev->Sov, ctdev_parameters.alpha_denominator);
        COMPILER_64_UMUL_32(ctdev->Sov, ctdev_parameters.alpha_numerator);
        COMPILER_64_ADD_64(ctdev->Sov, ctdev->dSsq);

        /* Calculate time variance (TVAR). */
        ctdev->tvar = ctdev->Sov;
        COMPILER_64_UDIV_16(ctdev->tvar, 6);
        COMPILER_64_UDIV_16(ctdev->tvar, ctdev_parameters.keff_numerator);
        COMPILER_64_UMUL_32(ctdev->tvar, ctdev_parameters.keff_denominator);

        /* Calculate time deviation (TDEV). */
        COMPILER_64_ISQRT(ctdev->tdev, ctdev->tvar);
        COMPILER_64_SET(ctdev->tdev_psec, 0, ctdev->tdev);
        if ((1 == ctdev_options.dynamic_rescale) && ctdev->fpbit) {
            COMPILER_64_UMUL_32(ctdev->tdev_psec, (1 << ctdev->fpbit));
        }
    }
    PTP_CTDEV_MUTEX_RELEASE_RETURN(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_ptp_ctdev_get
 * Purpose:
 *      Get the C-TDEV metric for the specified timescale, if available.
 * Parameters:
 *      unit          - (IN)  Unit number.
 *      ptp_id        - (IN)  PTP stack ID.
 *      clock_num     - (IN)  PTP clock number.
 *      timescale_sec - (IN)  Timescale in seconds.
 *      tdev_psec     - (OUT) TDEV value.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      If the exact timescale is not available, the next larger one
 *      is returned.
 */
int
_bcm_ptp_ctdev_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    unsigned timescale_sec,
    uint64 *tdev_psec)
{
    unsigned timescale_idx = 0;

    if (0 == ctdev_options.enable) {
        /* C-TDEV disabled. */
        if (ctdev_options.verbose) {
            LOG_CLI((BSL_META_U(unit,
                                "_bcm_ptp_ctdev_get(): C-TDEV disabled.\n")));
        }

        return BCM_E_UNAVAIL;
    }

    PTP_CTDEV_MUTEX_TAKE();

    /* Find lg_2(timescale) */
    while (timescale_sec > PTP_CTDEV_TAU0_SEC) {
        ++timescale_idx;
        timescale_sec = timescale_sec / 2;
    }

    *tdev_psec = ctdev_array.entry[timescale_idx].tdev_psec;

    PTP_CTDEV_MUTEX_RELEASE_RETURN(BCM_E_NONE);
}


/*
 * Function:
 *      _bcm_ptp_llu_div
 * Purpose:
 *      Division of unsigned 64-bit quantity by unsigned 16-bit quantity.
 * Parameters:
 *      num - (IN) Numerator.
 *      den - (IN) Denominator.
 * Returns:
 *      Result, num / den.
 * Notes:
 */
uint64
_bcm_ptp_llu_div(
    uint64 num,
    uint16 den)
{
    int el;
    uint16 numh[4];  /* Numerator as 16-bit half words. */
    uint16 qh[4];    /* Quotient as 16-bit half-words. */
    uint16 rhat = 0; /* Remainder. */

    uint32 Ql, Qh;
    uint64 Q;

    numh[0] = (COMPILER_64_LO(num)) & 0xffff;
    numh[1] = (COMPILER_64_LO(num) >> 16) & 0xffff;
    numh[2] = (COMPILER_64_HI(num)) & 0xffff;
    numh[3] = (COMPILER_64_HI(num) >> 16) & 0xffff;

    for (el = 3; el >= 0; --el) {
        qh[el] = (rhat*65536 + numh[el])/(den);
        rhat = (rhat*65536 + numh[el]) - qh[el]*(den);
    }

    Ql = qh[0] + (qh[1] << 16);
    Qh = qh[2] + (qh[3] << 16);
    COMPILER_64_SET(Q, Qh, Ql);

    return Q;
}

/*
 * Function:
 *      _bcm_ptp_llu_isqrt
 * Purpose:
 *      Integer square root of unsigned 64-bit quantity.
 * Parameters:
 *      x - (IN) Argument.
 * Returns:
 *      Result, isqrt(x).
 * Notes:
 */
uint32
_bcm_ptp_llu_isqrt(
    uint64 x)
{
    uint64 root, remainder, place, condval;

    remainder = x;
    COMPILER_64_ZERO(root);
    COMPILER_64_SET(place, 0x40000000, 0);

    while (COMPILER_64_GT(place, remainder)) {
        COMPILER_64_UDIV_16(place, 4);
    }

    while (!COMPILER_64_IS_ZERO(place)) {
        condval = place;
        COMPILER_64_ADD_64(condval, root);
        if (COMPILER_64_GE(remainder, condval)) {
            COMPILER_64_SUB_64(remainder, condval);
            COMPILER_64_ADD_64(root, place);
            COMPILER_64_ADD_64(root, place);
        }
        COMPILER_64_UDIV_16(root, 2);
        COMPILER_64_UDIV_16(place, 4);
    }
    return COMPILER_64_LO(root);
}

/*
 * Function:
 *      _bcm_ptp_xorshift_rand()
 * Purpose:
 *      Pseudorandom number generator based on xorshift algorithm with period 2^128 -1.
 * Parameters:
 *      None.
 * Returns:
 *      Result.
 * Notes:
 *      Ref. Marsaglia, G. (2003), Xorshift RNGs, Journal Statistical Software, 8, 1-9.
 */
uint32
_bcm_ptp_xorshift_rand(
    void)
{
    static uint8 isseed;

    static uint32 x = 123456789;
    static uint32 y = 362436069;
    static uint32 z = 521288629;
    static uint32 w = 88675123;

    uint32 t;

    /* Seed. */
    if (0 == isseed) {
        x = sal_time_usecs();
        if (0 == x) {
            x = 123456789;
        }
        isseed = 1;
    }

    t = (x ^ (x << 11));
    x = y; y = z; z = w;

    return ( w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)) );
}

/*
 * Function:
 *      _bcm_ptp_ctdev_g823_mask
 * Purpose:
 *      Calculate ITU-T G.823 SEC TDEV mask.
 * Parameters:
 *      tau - (IN) Averaging time (sec).
 * Returns:
 *      ITU-T G.823 SEC TDEV mask value.
 * Notes:
 */
uint32
_bcm_ptp_ctdev_g823_mask(
    int tau)
{
    uint32 mask_psec = 0;
    uint64 tau64;

    if (tau <= 17) {
        /* NOTE: Actual cutoff is 17.14 sec. */
        mask_psec = 12000;
    } else if (tau <= 100) {
        mask_psec = 700*tau;
    } else {
        /* NOTE: Equivalent scaling to increase precision of square-root result. */
        COMPILER_64_SET(tau64, 0, 10000*tau);
        COMPILER_64_ISQRT(mask_psec, tau64);
        mask_psec *= 12;
        mask_psec += 58000;
        mask_psec += (3*tau)/10;
    }

    return mask_psec;
}

/*
 * Function:
 *      _bcm_ptp_circular_buffer_init
 * Purpose:
 *      Create and initialize a circular buffer.
 * Parameters:
 *      buffer - (IN) Circular buffer.
 *      num_el - (IN) Number of buffer elements.
 * Returns:
 *      None.
 * Notes:
 */
static void
_bcm_ptp_circular_buffer_init(
    _bcm_ptp_circular_buffer_t *buffer,
    int num_el)
{
    buffer->num_el = num_el;
    buffer->num_write = 0;
    buffer->num_read = 0;

    buffer->data = sal_alloc(buffer->num_el*sizeof(eltype_t),"ptp-circular-buffer");
    sal_memset(buffer->data, 0, buffer->num_el*sizeof(eltype_t));

    buffer->read_el = buffer->data;
    buffer->write_el = buffer->data;
}

/*
 * Function:
 *      _bcm_ptp_circular_buffer_free
 * Purpose:
 *      Free circular buffer data memory.
 * Parameters:
 *      buffer - (IN) Circular buffer.
 * Returns:
 *      None.
 * Notes:
 */
static void
_bcm_ptp_circular_buffer_free(
    _bcm_ptp_circular_buffer_t *buffer)
{
    sal_free(buffer->data);
}

/*
 * Function:
 *      _bcm_ptp_circular_buffer_write
 * Purpose:
 *      Enqueue; write data to circular buffer.
 * Parameters:
 *      buffer - (IN) Circular buffer.
 *      value  - (IN) Data.
 * Returns:
 *      None.
 * Notes:
 */
static void
_bcm_ptp_circular_buffer_write(
    _bcm_ptp_circular_buffer_t *buffer,
    eltype_t *value)
{
    *(buffer->write_el++) = *value;
    if (buffer->write_el == buffer->data + buffer->num_el) {
        buffer->write_el = buffer->data;
    }
    buffer->num_write++;
}

/*
 * Function:
 *      _bcm_ptp_circular_buffer_read
 * Purpose:
 *      Dequeue; read data from circular buffer.
 * Parameters:
 *      buffer - (IN)  Circular buffer.
 *      value  - (OUT) Data.
 * Returns:
 *      None.
 * Notes:
 */
static void
_bcm_ptp_circular_buffer_read(
    _bcm_ptp_circular_buffer_t *buffer,
    eltype_t *value)
{
    *value = *(buffer->read_el++);
    if (buffer->read_el == buffer->data + buffer->num_el) {
        buffer->read_el = buffer->data;
    }
    buffer->num_read++;
}

#if 0  /* Unused. Preserve for future use. */
/*
 * Function:
 *      _bcm_ptp_circular_buffer_peek
 * Purpose:
 *      Peek at most recent value written to buffer.
 * Parameters:
 *      buffer - (IN)  Circular buffer.
 *      value  - (OUT) Data.
 * Returns:
 *      None.
 * Notes:
 */
static void
_bcm_ptp_circular_buffer_peek(
    _bcm_ptp_circular_buffer_t *buffer,
    eltype_t *value)
{
    *value = (buffer->write_el == buffer->data) ?
             *(buffer->write_el + buffer->num_el - 1) : *(buffer->write_el - 1);
}
#endif /* 0 */

/*
 * Function:
 *      _bcm_ptp_circular_buffer_peekn
 * Purpose:
 *      Peek at prior value written to buffer.
 * Parameters:
 *      buffer - (IN)  Circular buffer.
 *      n      - (IN)  Relative position of prior value (as positive index).
 *      value  - (OUT) Data.
 * Returns:
 *      None.
 * Notes:
 */
static void
_bcm_ptp_circular_buffer_peekn(
    _bcm_ptp_circular_buffer_t *buffer,
    int n,
    eltype_t *value)
{
    if (n >= buffer->num_el) {
        /* Peek index exceeds maximum allowed index based on buffer size. */
        return;
    } else if (n >= buffer->write_el - buffer->data) {
        *value = *(buffer->write_el + buffer->num_el - n - 1);
    } else {
        *value = *(buffer->write_el - n - 1);
    }
}

#endif /* defined(INCLUDE_PTP) */
