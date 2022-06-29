/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:       gdpll.c
 *
 * Remarks:    Broadcom StrataSwitch Time GDPLL API
 *
 * Functions:
 *   Private Interface functions
 *      _gdpll_event_divisor_misc_set
 *      _gdpll_event_divisor_port_set
 *      _gdpll_event_dest_misc_set
 *      _gdpll_event_dest_port_set
 *      _gdpll_event_enable_misc_set
 *      _gdpll_event_enable_port_set
 *      _gdpll_event_roe_52b_set
 *      _gdpll_event_roe_52b_get
 *      _gdpll_event_config_set
 *
 *      _gdpll_capture_enable_m7_set
 *      _gdpll_capture_enable_cpu_set
 *      _gdpll_capture_enable_cpu_get
 *
 *      _gdpll_chan_update_set
 *      _gdpll_chan_enable_set
 *      _gdpll_chan_out_txpi_set
 *      _gdpll_chan_out_misc_set
 *      _gdpll_chan_out_enable_set
 *      _gdpll_chan_priority_set
 *
 *      _gdpll_chan_config_dpll_set
 *      _gdpll_chan_config_dpll_get
 *      _gdpll_chan_debug_mode_set
 *      _gdpll_chan_debug_mode_get
 *
 *      _gdpll_flush
 *      _gdpll_init
 *      _gdpll_deinit
 *      _gdpll_input_eventId_get
 *      _gdpll_input_array_alloc
 *      _gdpll_input_array_free
 *      _gdpll_chan_alloc
 *      _gdpll_chan_free
 *
 *      _tdpll_chan_create
 *      _tdpll_gdpll_chan_destroy
 *
 *   Public Interface functions
 *      bcm_common_gdpll_chan_create
 *      bcm_common_gdpll_chan_destroy
 *      bcm_common_gdpll_chan_enable
 *      bcm_common_gdpll_chan_state_get
 *      bcm_common_gdpll_chan_debug_enable
 *      bcm_common_gdpll_debug_cb_register
 *      bcm_common_gdpll_flush
 *      bcm_common_gdpll_cb_register
 *      bcm_common_gdpll_cb_unregister
 *      bcm_common_gdpll_debug
 *      bcm_common_gdpll_offset_get
 *      bcm_common_gdpll_offset_set
 */

#include <shared/util.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/iproc.h>
#include <soc/drv.h>

#if defined(INCLUDE_GDPLL)
#include <bcm/time.h>

#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm_int/esw/switch.h>

#include <sal/core/thread.h>
#include <include/sal/core/dpc.h>

#if defined(BCM_DNX_SUPPORT)
#include <shared/util.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnxc/dnxc_ha.h>
#endif

#include <bcm/gdpll.h>
#include <bcm_int/gdpll.h>
#include <bcm_int/ns_gdpll_regs.h>

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
#include <soc/uc.h>
#endif
#ifdef BCM_MONTEREY_SUPPORT
#include <soc/monterey.h>
#endif /* BCM_MONTEREY_SUPPORT */

/****************************************************************************/
/*                      MACRO definitions                                   */
/****************************************************************************/
#define BCM_GDPLL_MISC_EVENT_EN_CPU     (1<<0)
#define BCM_GDPLL_MISC_EVENT_EN_BS0HB   (1<<1)
#define BCM_GDPLL_MISC_EVENT_EN_BS1HB   (1<<2)
#define BCM_GDPLL_MISC_EVENT_EN_IPDM0   (1<<3)
#define BCM_GDPLL_MISC_EVENT_EN_IPDM1   (1<<4)
#define BCM_GDPLL_MISC_EVENT_EN_TS1     (1<<5)
#if defined(BCM_NANOSYNC_V1_SUPPORT)
#define BCM_GDPLL_MISC_EVENT_EN_PTPTOD1PPS  (1<<6)
#define BCM_GDPLL_MISC_EVENT_EN_BS0PPS  (1<<10)
#define BCM_GDPLL_MISC_EVENT_EN_BS1PPS  (1<<11)
#elif defined(BCM_MONTEREY_SUPPORT)
#define BCM_GDPLL_MISC_EVENT_EN_RSVD1IF (1<<6)
#define BCM_GDPLL_MISC_EVENT_EN_RSVD1RF (1<<7)
#endif
#define BCM_GDPLL_MISC_EVENT_EN_BS0CONV (1<<8)
#define BCM_GDPLL_MISC_EVENT_EN_BS1CONV (1<<9)

/* GDPLL debug buffer thresholds */
#if defined(BCM_NANOSYNC_V1_SUPPORT)
#define BCM_GDPLL_DEBUG_BUFFER_START    (0x646000)
#define BCM_GDPLL_DEBUG_BUFFER_SIZE     (1024*10)   /* 40KB/4 */
#define HW_IF_FIELD                     (1<<10)

/* Interrup details */
#define SOC_NS_INTR1                    119     /* Use NSYNC_TS_INT_ENABLE_0r */
#define SOC_NS_INTR2                    120     /* Use NSYNC_TS_INT_ENABLE_1r */
#define SOC_NS_DBG_INTR                 121

#elif defined(BCM_MONTEREY_SUPPORT)
#define BCM_GDPLL_DEBUG_BUFFER_START    (0x3274000)
#define BCM_GDPLL_DEBUG_BUFFER_SIZE     (40*1024/4)   /* 40KB/4 */
#define SOC_GDPLL_NS_INTR_POS           (1<<24)
#define SOC_GDPLL_NS_INTR_DEBUG_POS     (1<<25)

#define EVENT_TS_TO_CPU                 0
#define EVENT_TS_TO_M7                  1
#define EVENT_TS_TO_ALL                 2

#define INTR_RSVD1_MESSAGE_RX_STATUS    (1<<22)
#endif

#define BCM_GDPLL_DEBUG_THRESHOLD_1K    (1024/4)
#define BCM_GDPLL_DEBUG_THRESHOLD_2K    (2048/4)
#define BCM_GDPLL_DEBUG_THRESHOLD_8K    (8192/4)
#define BCM_GDPLL_DEBUG_THRESHOLD_16K   (16384/4)
#define BCM_GDPLL_DEBUG_THRESHOLD_20K   (20*1024/4)
#define BCM_GDPLL_DEBUG_THRESHOLD       (BCM_GDPLL_DEBUG_THRESHOLD_8K)

/* Debug buffer interrupts */
#define INTR_GDPLL_DEBUG_THR            (1<<0)
#define INTR_GDPLL_DEBUG_OF             (1<<1)
#define INTR_GDPLL_DEBUG_ECC0           (1<<2)
#define INTR_GDPLL_DEBUG_ECC1           (1<<3)
#define INTR_NS_DEBUG                   (INTR_GDPLL_DEBUG_THR  | \
                                         INTR_GDPLL_DEBUG_OF)
#define TS_GPIO_COUNT                   6


/****************************************************************************/
/*                      LOCAL VARIABLES DECLARATION                         */
/****************************************************************************/

#if defined(BCM_MONTEREY_SUPPORT)
#define NUM_TDPLL_INSTANCE  64  
typedef struct gdpll_tdpll_state_s {
    uint32 tdpll_instace[NUM_TDPLL_INSTANCE];   /* Hold the I/A indicator of each instance */
} gdpll_tdpll_state_t;
STATIC gdpll_tdpll_state_t gdpll_tdpll_state;

uint32 cbData[3];   /* For RSVD1 messages */
#endif

gdpll_context_t *pGdpllCtx[BCM_MAX_NUM_UNITS] = {NULL};
STATIC m7_dpll_param_t m7DpllParam;

#if defined(BCM_XGS_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)
extern int _gdpll_reinit(int unit, gdpll_context_t *pGdpllContext);
extern int _bcm_gdpll_scache_alloc(int unit);
extern int _gdpll_portmap_get(int unit, bcm_port_t port, int *pGdpllPort, int isRx);
extern int bcm_time_ts_counter_get(int unit, bcm_time_ts_counter_t *counter);
#endif

/****************************************************************************/
/*                     Internal functions implementation                    */
/****************************************************************************/
int _timestamper_get(int unit, int timestamper, uint64 *pTs_52)
{
    int rv  = BCM_E_NONE;

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    uint32 ts_upper = 0;
    uint32 ts_lower = 0;
    uint64 ts;
    soc_reg_t reg_ts_hi[2] = { NSYNC_TIMESYNC_TS_0_TIMESTAMP_UPPER_STATUSr,
                               NSYNC_TIMESYNC_TS_1_TIMESTAMP_UPPER_STATUSr };
    soc_reg_t reg_ts_lo[2] = { NSYNC_TIMESYNC_TS_0_TIMESTAMP_LOWER_STATUSr,
                               NSYNC_TIMESYNC_TS_1_TIMESTAMP_LOWER_STATUSr };
    if ((timestamper == 0) || (timestamper == 1)) {
        READ_NS_REGr(unit, reg_ts_hi[timestamper], 0, 0, &ts_upper);
        READ_NS_REGr(unit, reg_ts_lo[timestamper], 0, 0, &ts_lower);
        ts = ts_upper;
        ts = ((ts << 32) | ts_lower);
        *pTs_52 = ts;
    } else {
        bsl_printf("GDPLL: Error!! _timestamper_get: \
                    Wrong timestamper:%d\n", timestamper);
        rv = BCM_E_FAIL;
    }
#else
    uint32 acc1 = 0;
    uint32 acc2 = 0;
    uint64 ts;

    if (timestamper == 1) {
        READ_NS_REGr(unit, NS_TIMESYNC_TS1_ACCUMULATOR_2r, 0, 0, &acc2);
        READ_NS_REGr(unit, NS_TIMESYNC_TS1_ACCUMULATOR_1r, 0, 0, &acc1);
    } else if (timestamper == 0) {
        READ_NS_REGr(unit, NS_TIMESYNC_TS0_ACCUMULATOR_2r, 0, 0, &acc2);
        READ_NS_REGr(unit, NS_TIMESYNC_TS0_ACCUMULATOR_1r, 0, 0, &acc1);
    } else {
        bsl_printf("GDPLL: Error !! _timestamper_get: Wrong timestamper:%d\n", timestamper);
        rv = BCM_E_FAIL;
    }

    u64_L(ts) = acc1 >> 8 | acc2 << 24;
    u64_H(ts) = acc2 >> 8;
    *pTs_52 = ts;
#endif

     return rv;
}

int _timestamper_set(int unit, int timestamper, uint64 ts_52)
{
    int rv  = BCM_E_NONE;
#if defined(BCM_NANOSYNC_V1_SUPPORT)
    uint32 rval;

    soc_reg_t reg_acc2[2] = { NSYNC_TIMESYNC_TS_0_INIT_ACCUMULATOR_2r,
                              NSYNC_TIMESYNC_TS_1_INIT_ACCUMULATOR_2r};
    soc_reg_t reg_acc1[2] = { NSYNC_TIMESYNC_TS_0_INIT_ACCUMULATOR_1r,
                              NSYNC_TIMESYNC_TS_1_INIT_ACCUMULATOR_1r};
    soc_reg_t reg_acc0[2] = { NSYNC_TIMESYNC_TS_0_INIT_ACCUMULATOR_0r,
                              NSYNC_TIMESYNC_TS_1_INIT_ACCUMULATOR_0r};

    if ((timestamper == 0) || (timestamper == 1)) {
        READ_NS_REGr(unit, reg_acc2[timestamper], 0, 0, &rval);
        soc_reg_field_set(unit, reg_acc2[timestamper], &rval, ACC_2f, u64_H(ts_52) << 8 | u64_L(ts_52) >> 24);
        WRITE_NS_REGr(unit, reg_acc2[timestamper], 0, 0, rval);

        READ_NS_REGr(unit, reg_acc1[timestamper], 0, 0, &rval);
        soc_reg_field_set(unit, reg_acc1[timestamper], &rval, ACC_1f, u64_L(ts_52) << 8);
        WRITE_NS_REGr(unit, reg_acc1[timestamper], 0, 0, rval);

        READ_NS_REGr(unit, reg_acc0[timestamper], 0, 0, &rval);
        soc_reg_field_set(unit, reg_acc0[timestamper], &rval, ACC_0f, 0);
        WRITE_NS_REGr(unit, reg_acc0[timestamper], 0, 0, rval);
    } else {
        bsl_printf("Error !! _timestamper_set: Wrong timestamper:%d\n", timestamper);
        rv = BCM_E_FAIL;
    }

#else
    uint32 rval;
    if (timestamper == 1) {
        READ_NS_REGr(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r, 0, 0, &rval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r, &rval, ACC2f, u64_H(ts_52) << 8 | u64_L(ts_52) >> 24);
        WRITE_NS_REGr(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r, 0, 0, rval);

        READ_NS_REGr(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r, 0, 0, &rval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r, &rval, ACC1f, u64_L(ts_52) << 8);
        WRITE_NS_REGr(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r, 0, 0, rval);

        READ_NS_REGr(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r, 0, 0, &rval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
        WRITE_NS_REGr(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r, 0, 0, rval);

    } else if (timestamper == 0) {
        READ_NS_REGr(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r, 0, 0, &rval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r, &rval, ACC2f, u64_H(ts_52) << 8 | u64_L(ts_52) >> 24);
        WRITE_NS_REGr(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r, 0, 0, rval);

        READ_NS_REGr(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r, 0, 0, &rval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r, &rval, ACC1f, u64_L(ts_52) << 8);
        WRITE_NS_REGr(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r, 0, 0, rval);

        READ_NS_REGr(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r, 0, 0, &rval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
        WRITE_NS_REGr(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r, 0, 0, rval);
    } else {
        bsl_printf("Error !! _timestamper_set: Wrong timestamper:%d\n", timestamper);
        rv = BCM_E_FAIL;
    }

#endif
    return rv;
}

int _timestamper_enable(int unit, int timestamper, int enable)
{
    int rv  = BCM_E_NONE;

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    if ((timestamper == 1) || (timestamper == 0)) {
        uint32 rval;
        uint32 cnt_en;
        READ_NS_REGr(unit, NSYNC_IEEE_1588_TIME_CONTROLr, 0, 0, &rval);
        cnt_en = soc_reg_field_get(unit, NSYNC_IEEE_1588_TIME_CONTROLr, rval, COUNTER_ENABLEf);
        if (enable) {
            cnt_en |= (1<<timestamper);
        } else {
            cnt_en &= ~(1<<timestamper);
        }
        soc_reg_field_set(unit, NSYNC_IEEE_1588_TIME_CONTROLr, &rval, COUNTER_ENABLEf, cnt_en);
        WRITE_NS_REGr(unit, NSYNC_IEEE_1588_TIME_CONTROLr, 0, 0, rval);
        bsl_printf("_timestamper_enable: success timestamper:%d enable:%d\n", timestamper, enable);
    } else {
        bsl_printf("Error !! _timestamper_enable: Wrong timestamper:%d\n", timestamper);
        rv = BCM_E_FAIL;
    }
#else
    uint32 rval;
    if (timestamper == 1) {
        READ_NS_REGr(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr, 0, 0, &rval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr, &rval, ENABLEf, enable?1:0);
        WRITE_NS_REGr(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr, 0, 0, rval);
    } else if (timestamper == 0) {
        READ_NS_REGr(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, 0, 0, &rval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &rval, ENABLEf, enable?1:0);
        WRITE_NS_REGr(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, 0, 0, rval);
    } else {
        bsl_printf("Error !! _timestamper_enable: Wrong timestamper:%d\n", timestamper);
        rv = BCM_E_FAIL;
    }
#endif

    return rv;
}

STATIC int
m7_mem_write(int unit, uint32 addr, uint32 data)
{
#if defined(BCM_NANOSYNC_V1_SUPPORT)
    WRITE_NS_MEM(unit, addr, data);
#else
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        soc_cm_iproc_write(unit, addr, data);
        return BCM_E_NONE;
    }
#endif
    return BCM_E_NONE;
}

STATIC int
m7_mem_read(int unit, uint32 addr)
{
#if defined(BCM_NANOSYNC_V1_SUPPORT)
    int val;
    READ_NS_MEM(unit, addr, &val);
    return val;
#else
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        return soc_cm_iproc_read(unit, addr);
    }
#endif
    return BCM_E_NONE;
}

STATIC int
_gdpll_event_divisor_misc_set(int unit, bcm_gdpll_input_event_t event_misc,
                            uint32 divisor, uint32 divisor_hi)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;

    switch (event_misc) {
        case bcmGdpllInputEventCpu:
        case bcmGdpllInputEventBS0HB:
        case bcmGdpllInputEventBS1HB:
        case bcmGdpllInputEventIPDM0:
        case bcmGdpllInputEventIPDM1:
        case bcmGdpllInputEventTS1TS:
#if defined(BCM_MONTEREY_SUPPORT)
        case bcmGdpllInputEventRSVD1IF:
        case bcmGdpllInputEventRSVD1RF:
#endif
        case bcmGdpllInputEventBS0ConvTC:
        case bcmGdpllInputEventBS1ConvTC:
            /* No divider */
            bsl_printf("GDPLL: event_divisor_misc_set: No divider for event:%d \n", event_misc);
            break;

#if defined(BCM_NANOSYNC_V1_SUPPORT)
        case bcmGdpllInputEventGen1:
        case bcmGdpllInputEventGen2:
        case bcmGdpllInputEventGen3:
        case bcmGdpllInputEventGen4:
        case bcmGdpllInputEventGen5:
        case bcmGdpllInputEventGen6:
        case bcmGdpllInputEventGen7:
        case bcmGdpllInputEventGen8:
        case bcmGdpllInputEventGen9:
        case bcmGdpllInputEventGen10:
        case bcmGdpllInputEventGen11:
        case bcmGdpllInputEventGen12:
        case bcmGdpllInputEventGen13:
        case bcmGdpllInputEventGen14:
        case bcmGdpllInputEventGen15:
        case bcmGdpllInputEventGen16:
            idx = event_misc - bcmGdpllInputEventGen1;

            READ_NS_REGr(unit, NSYNC_TIMESYNC_GEN_EXT_EVENT_CTRLAr, 0, 4*idx, &regVal);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_GEN_EXT_EVENT_CTRLAr, &regVal, UP_PERIOD_Nf, divisor);
            WRITE_NS_REGr(unit, NSYNC_TIMESYNC_GEN_EXT_EVENT_CTRLAr, 0, 4*idx, regVal);

            READ_NS_REGr(unit, NSYNC_TIMESYNC_GEN_EXT_EVENT_CTRLBr, 0, 4*idx, &regVal);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_GEN_EXT_EVENT_CTRLBr, &regVal, DOWN_PERIOD_Nf, divisor_hi);
            WRITE_NS_REGr(unit, NSYNC_TIMESYNC_GEN_EXT_EVENT_CTRLBr, 0, 4*idx, regVal);

            break;
#endif
        case bcmGdpllInputEventGPIO0:
        case bcmGdpllInputEventGPIO1:
        case bcmGdpllInputEventGPIO2:
        case bcmGdpllInputEventGPIO3:
        case bcmGdpllInputEventGPIO4:
        case bcmGdpllInputEventGPIO5:
            idx = event_misc - bcmGdpllInputEventGPIO0;
            READ_NS_REGr(unit, gpio_0_ctrl_r, 0, 24*idx, &regVal);
            soc_reg_field_set(unit,  gpio_0_ctrl_r, &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, gpio_0_ctrl_r, 0, 24*idx, regVal);
            break;

        case bcmGdpllInputEventBS0PLL:
        case bcmGdpllInputEventBS1PLL:
            idx = event_misc - bcmGdpllInputEventBS0PLL;
            READ_NS_REGr(unit, bs_pll_reg1[idx], 0, 0, &regVal);
            soc_reg_field_set(unit, bs_pll_reg1[idx], &regVal,
#if defined(BCM_NANOSYNC_V1_SUPPORT)
                              UP_PERIODf,
#else
                              DIVISORf,
#endif
                              divisor);
            WRITE_NS_REGr(unit, bs_pll_reg1[idx], 0, 0, regVal);
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            READ_NS_REGr(unit, bs_pll_reg2[idx], 0, 0, &regVal);
            soc_reg_field_set(unit, bs_pll_reg2[idx], &regVal,
                              DOWN_PERIODf, divisor_hi);
            WRITE_NS_REGr(unit, bs_pll_reg2[idx], 0, 0, regVal);
#endif
            break;

#if defined(BCM_MONTEREY_SUPPORT)
        case bcmGdpllInputEventLCPLL0:
        case bcmGdpllInputEventLCPLL1:
        case bcmGdpllInputEventLCPLL2:
        case bcmGdpllInputEventLCPLL3:
            idx = event_misc - bcmGdpllInputEventLCPLL0;
            READ_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, 0, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, 0, regVal);
            break;
#endif

        default:
           LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "event_divisor_misc_set: Error !! wrong param\n")));
           rv = BCM_E_PARAM;
           goto exit;
    }

exit:
    return (rv);
}

STATIC int
_gdpll_event_divisor_port_set(int unit, bcm_port_t port,
    bcm_gdpll_port_event_t port_event_type, uint32 divisor, uint32 divisor_hi)

{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;

    if (port > BCM_GDPLL_NUM_PORTS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "event_divisor_port_set: Error !! wrong port \n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    idx = port;
    switch (port_event_type) {
#if defined(BCM_MONTEREY_SUPPORT)
        case bcmGdpllPortEventRXSOF:
        case bcmGdpllPortEventTXSOF:
        case bcmGdpllPortEventROE:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "event_divisor_port_set: Error !! No divisor\n")));
            rv = BCM_E_UNAVAIL;
            break;
#endif

        case bcmGdpllPortEventRXCDR:
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            READ_NS_REGr(unit, NSYNC_TIMESYNC_RX_CDR_EVENT_CTRLAr, idx, 0, &regVal);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_RX_CDR_EVENT_CTRLAr, &regVal,
                              UP_PERIODf, divisor);
            WRITE_NS_REGr(unit, NSYNC_TIMESYNC_RX_CDR_EVENT_CTRLAr, idx, 0, regVal);
            WRITE_NS_REGr(unit, NSYNC_TIMESYNC_RX_CDR_EVENT_CTRLAr, idx, 0, regVal);

            READ_NS_REGr(unit, NSYNC_TIMESYNC_RX_CDR_EVENT_CTRLBr, idx, 0, &regVal);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_RX_CDR_EVENT_CTRLBr, &regVal,
                              DOWN_PERIODf, divisor_hi);
            WRITE_NS_REGr(unit, NSYNC_TIMESYNC_RX_CDR_EVENT_CTRLBr, idx, 0, regVal);
            WRITE_NS_REGr(unit, NSYNC_TIMESYNC_RX_CDR_EVENT_CTRLBr, idx, 0, regVal);

#elif defined(BCM_MONTEREY_SUPPORT)
            READ_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, idx, 0, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, idx, 0, regVal);
            /* When the RXCDR(RX clock) is off, the toggle flop in HW requires 3-writes to
             * NS_TIMESYNC_RX_CDR_EVENT_CTRL register to ensure the write has gone through
             * and the logic works as expected when RX clock comes active
             * Ref: SDK-178135
             */
            WRITE_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, idx, 0, regVal);
#endif
            break;

        case bcmGdpllPortEventTXPI:
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            READ_NS_REGr(unit, NSYNC_TIMESYNC_TX_PI_CLK_EVENT_CTRLAr, idx, 0, &regVal);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_TX_PI_CLK_EVENT_CTRLAr, &regVal,
                              UP_PERIODf, divisor);
            WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TX_PI_CLK_EVENT_CTRLAr, idx, 0, regVal);

            READ_NS_REGr(unit, NSYNC_TIMESYNC_TX_PI_CLK_EVENT_CTRLBr, idx, 0, &regVal);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_TX_PI_CLK_EVENT_CTRLBr, &regVal,
                              DOWN_PERIODf, divisor_hi);
            WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TX_PI_CLK_EVENT_CTRLBr, idx, 0, regVal);

#elif defined(BCM_MONTEREY_SUPPORT)
            READ_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, idx, 0, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, idx, 0, regVal);
#endif
            break;

        default:
           LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "event_divisor_port_set: Error !! Wrong param\n")));
           rv = BCM_E_PARAM;
    }

exit:
    return (rv);
}

#if defined(BCM_NANOSYNC_V1_SUPPORT)
/* Get the event-id from the input event types */
STATIC int _gdpll_event_id_get(int unit, bcm_gdpll_input_event_t input_event,
        int port, bcm_gdpll_port_event_t port_event_type, int *pId)
{
    int rv  = BCM_E_NONE;

    /* EventId 0 to 34 are Misc events
     * EventId 35 to 546 for port events
     */
    switch (input_event) {
        case bcmGdpllInputEventGen1:
        case bcmGdpllInputEventGen2:
        case bcmGdpllInputEventGen3:
        case bcmGdpllInputEventGen4:
        case bcmGdpllInputEventGen5:
        case bcmGdpllInputEventGen6:
        case bcmGdpllInputEventGen7:
        case bcmGdpllInputEventGen8:
        case bcmGdpllInputEventGen9:
        case bcmGdpllInputEventGen10:
        case bcmGdpllInputEventGen11:
        case bcmGdpllInputEventGen12:
        case bcmGdpllInputEventGen13:
        case bcmGdpllInputEventGen14:
        case bcmGdpllInputEventGen15:
        case bcmGdpllInputEventGen16:
            *pId = 0 + (input_event - bcmGdpllInputEventGen1);
            break;
        case bcmGdpllInputEventCpu:
            *pId = 16;
            break;
        case bcmGdpllInputEventTS1TS:
            *pId = 17;
            break;
        case bcmGdpllInputEventIPDM0:
        case bcmGdpllInputEventIPDM1:
            *pId = 18 + (input_event - bcmGdpllInputEventIPDM0);
            break;
        case bcmGdpllInputEventGPIO0:
        case bcmGdpllInputEventGPIO1:
        case bcmGdpllInputEventGPIO2:
        case bcmGdpllInputEventGPIO3:
        case bcmGdpllInputEventGPIO4:
        case bcmGdpllInputEventGPIO5:
            *pId = 20 + (input_event - bcmGdpllInputEventGPIO0);
            break;
        case bcmGdpllInputEventBS0HB:
        case bcmGdpllInputEventBS1HB:
            *pId = 26 + (input_event - bcmGdpllInputEventBS0HB);
            break;
        case bcmGdpllInputEventBS0PLL:
        case bcmGdpllInputEventBS1PLL:
            *pId = 28 + (input_event - bcmGdpllInputEventBS0PLL);
            break;
        case bcmGdpllInputEventBS0ConvTC:
        case bcmGdpllInputEventBS1ConvTC:
            *pId = 30 + (input_event - bcmGdpllInputEventBS0ConvTC);
            break;
        case bcmGdpllInputEventPTPTOD1PPS:
            *pId = 32;
            break;
        case bcmGdpllInputEventBS0PPS:
        case bcmGdpllInputEventBS1PPS:
            *pId = 33 + (input_event - bcmGdpllInputEventBS0PPS);
            break;
        case bcmGdpllInputEventPORT:
            if (port_event_type == bcmGdpllPortEventRXCDR) {
                *pId = 35 + port*2;
            } else if (port_event_type == bcmGdpllPortEventTXPI) {
                *pId = 35 + (port*2) + 1;
            } else {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "_gdpll_event_id_get: Error !! wrong port_event_type:%d\n"), port_event_type));
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "_gdpll_event_id_get: Error !! wrong event:%d\n"), input_event));
            rv  = BCM_E_PARAM;
    }
    return rv;
}

/* Get the input event based on event-id */
STATIC int _gdpll_event_get(int unit, int eventId, bcm_gdpll_input_event_t *pInput_event,
           int *gdpll_port, bcm_gdpll_port_event_t *port_event_type)
{
    int rv  = BCM_E_NONE;

    if (eventId < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "event_divisor_port_set: Error !! wrong port \n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    if(eventId <= 15) {
        *pInput_event = bcmGdpllInputEventGen1 + eventId;       /* 0-15 */
    } else if (eventId == 16) {
        *pInput_event = bcmGdpllInputEventCpu;                  /* 16 */
    } else if (eventId == 17) {
        *pInput_event = bcmGdpllInputEventTS1TS;                /* 17 */
    } else if (eventId <= 19) {
        *pInput_event = bcmGdpllInputEventIPDM0 + (eventId-18); /* 18-19*/
    } else if (eventId <= 25) {
        *pInput_event = bcmGdpllInputEventGPIO0 + (eventId-20); /* 20-25*/
    } else if (eventId <= 27) {
        *pInput_event = bcmGdpllInputEventBS0HB + (eventId-26); /* 26-27 */
    } else if (eventId <= 29) {
        *pInput_event = bcmGdpllInputEventBS0PLL + (eventId-28);/* 28-29 */
    } else if (eventId <= 31) {
        *pInput_event = bcmGdpllInputEventBS0ConvTC + (eventId-30); /* 30-31 */
    } else if (eventId == 32) {
        *pInput_event = bcmGdpllInputEventPTPTOD1PPS;           /* 32 */
    } else if (eventId <= 34) {
        *pInput_event = bcmGdpllInputEventBS0PPS + (eventId-26);/* 33-34 */
    } else if (eventId <= 546) {                                /* 35-546*/
        *pInput_event = bcmGdpllInputEventPORT;
        *gdpll_port = (eventId-35)/2;
        *port_event_type = eventId%2 ? bcmGdpllPortEventRXCDR:bcmGdpllPortEventTXPI;
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "_gdpll_event_get: Error !! wrong event_id:%d\n"), eventId));
        rv  = BCM_E_PARAM;
    }

exit:
    return rv;
}
#endif

/* Mapping of input events to eventId will be device specific */
STATIC int
_gdpll_input_eventId_get(int unit,
                    bcm_gdpll_chan_input_t *pInputEvent,
                    uint32 *pEventId)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if ((pEventId==NULL) || (pGdpllContext==NULL) ||
        (pInputEvent->input_event < bcmGdpllInputEventCpu) ||
        (pInputEvent->input_event >= bcmGdpllInputEventMax)) {
         LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "input_eventId_get: Error !! Wrong event\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    SOC_IF_ERROR_RETURN(_gdpll_event_id_get(unit, pInputEvent->input_event,
			                    pInputEvent->port, pInputEvent->port_event_type, &idx));
#elif defined(BCM_MONTEREY_SUPPORT)
    /* EventId 0 to 191 for port events,
     * EventId 192 to 213 for EventCpu to EventLCPLL3
     * EventId 214 for R5
     */
    switch ((int)pInputEvent->input_event) {
        case bcmGdpllInputEventPORT:
            if (pInputEvent->port < 0 || pInputEvent->port > BCM_GDPLL_NUM_PORTS){
                LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "input_eventId_get: Error !! Wrong port\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }

            if ((pInputEvent->port_event_type == bcmGdpllPortEventRXCDR) ||
                (pInputEvent->port_event_type == bcmGdpllPortEventRXSOF)){
                /* Event could be TXSOF or TXPI based on port type */
                idx = pInputEvent->port*2;
            } else if (pInputEvent->port_event_type == bcmGdpllPortEventROE) {
                idx = pInputEvent->port*2 + 1;
            } else if ((pInputEvent->port_event_type == bcmGdpllPortEventTXSOF) ||
                       (pInputEvent->port_event_type == bcmGdpllPortEventTXPI)){
                /* Event could be TXSOF or TXPI based on port type */
                idx = BCM_GDPLL_IA_START_TXPI_TXSOF + pInputEvent->port;
            } else {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "input_eventId_get: Error !! Wrong param\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }
            break;

        case bcmGdpllInputEventR5:
            idx = BCM_GDPLL_IA_START_R5;
            break;

        default:
            idx = BCM_GDPLL_IA_START_MISC + pInputEvent->input_event;
    }
#endif

    *pEventId = idx;

exit:
    return rv;
}

/* Get the event based on the eventId */
STATIC int
_gdpll_input_event_get(int unit,
                    int chan, int isFb,
                    bcm_gdpll_input_event_t *pEvent_misc,
                    bcm_gdpll_port_event_t *pPort_event_type,
                    bcm_port_t *pPort)
{
    int rv  = BCM_E_NONE;
#if defined(BCM_MONTEREY_SUPPORT)
    int idx = 0;
    bcm_port_t port;
#endif
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    uint32 eventId = isFb ? pGdpllContext->dpll_chan[chan].eventId_fb:
                pGdpllContext->dpll_chan[chan].eventId_ref;

    if (eventId >= BCM_GDPLL_NUM_INPUT_EVENTS){
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "input_event_get: Error !! Wrong param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    *pPort = isFb ? pGdpllContext->dpll_chan[chan].gdpllport_fb:pGdpllContext->dpll_chan[chan].gdpllport_ref;
    SOC_IF_ERROR_RETURN(_gdpll_event_get(unit, eventId, pEvent_misc, pPort, pPort_event_type));

#elif defined(BCM_MONTEREY_SUPPORT)
    port = isFb ? pGdpllContext->dpll_chan[chan].lport_fb:pGdpllContext->dpll_chan[chan].lport_ref;
    *pPort = isFb ? pGdpllContext->dpll_chan[chan].gdpllport_fb:pGdpllContext->dpll_chan[chan].gdpllport_ref;

    if (eventId >= BCM_GDPLL_IA_START_R5) {
        /* Event from R5 */
        *pEvent_misc = bcmGdpllInputEventR5;

    } else if (eventId >= BCM_GDPLL_IA_START_MISC) {
        /* Miscellaneous events */
        *pEvent_misc = eventId - BCM_GDPLL_IA_START_MISC;

    } else if (eventId >= BCM_GDPLL_IA_START_TXPI_TXSOF) {
        /* Its a TXPI or TXSOF event based on the port */
        *pEvent_misc = bcmGdpllInputEventPORT;
        if (IS_ROE_PORT(unit, port)) {
            *pPort_event_type = bcmGdpllPortEventTXSOF;
        } else {
            *pPort_event_type = bcmGdpllPortEventTXPI;
        }
    } else {
        /* Its a TXPI or TXSOF event based on the port */
        *pEvent_misc = bcmGdpllInputEventPORT;
        idx = eventId%2;

        if (IS_ROE_PORT(unit, port)) {
            *pPort_event_type = idx ? bcmGdpllPortEventROE : bcmGdpllPortEventRXSOF;
         } else {
            if (idx) {
                /* Unused event ID */
                LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "input_event_get: Error !! Unused event ID\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }
            *pPort_event_type = bcmGdpllPortEventRXCDR;
         }
    }
#endif

exit:
    return rv;
}

STATIC int
_gdpll_input_eventId_inUse(int unit,
                    uint32 eventId, int *pIsUse)
{
    int rv  = BCM_E_NONE;
    int chan;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

#if defined(BCM_MONTEREY_SUPPORT)
    /* Since the event from R5 can be for multiple outpts, skip the check */
    if (eventId == BCM_GDPLL_IA_START_R5) {
        *pIsUse = 0;
        goto exit;
    }
#endif

    /* Check if this event is ref or fb for any of the active channels */
    for (chan=0; chan<BCM_GDPLL_NUM_CHANNELS; chan++) {
        if (pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_CHAN_ALLOC) {
            if ((pGdpllContext->dpll_chan[chan].eventId_ref == eventId) ||
                (pGdpllContext->dpll_chan[chan].eventId_fb == eventId)) {
                /* This event is already used by chan */
                *pIsUse = 1;
                bsl_printf("GDPLL: eventId:%d is under use by chan:%d\n", eventId, chan);
                goto exit;
            }
        }
    }

    *pIsUse = 0;

exit:
    return rv;
}

STATIC int
_gdpll_input_array_alloc(int unit, uint32 eventId, uint32 *pIaAddr)
{
    int rv  = BCM_E_NONE;

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    *pIaAddr = eventId;
#elif defined(BCM_MONTEREY_SUPPORT)
    int ia_loc;

    /*
     * Findout if this event is already under use by active channel
     * If not, findout the free ia address from the pool and allocate,
     *
     * Monterey have event id's less than the input array size.
     * Hence let the event id be the input array address for now
     * HW team recommends the eventId to be same as iaAddr
     */

    if (eventId == 214/*BCM_GDPLL_IA_START_R5, R5-event */) {
        /* For now, the IA locations assigned is from 214 */
        int ia_found = 0;
        int instance_count;
        for (ia_loc = eventId; ia_loc < BCM_GDPLL_NUM_IA_ENTRIES; ia_loc++) {
            for (instance_count = 0; instance_count < NUM_TDPLL_INSTANCE; instance_count++) {
                if (gdpll_tdpll_state.tdpll_instace[instance_count] != ia_loc)
                    continue;
            }
            if (instance_count == NUM_TDPLL_INSTANCE) {
                /* Found the ia location */
                ia_found = 1;
                break;
            }

            continue;
        }

        if (!ia_found) {
            bsl_printf("_gdpll_input_array_alloc: Error!! IA location for R5 is not found\n");
            rv = BCM_E_FULL;
        } else {
            *pIaAddr = ia_loc;
            bsl_printf("_gdpll_input_array_alloc: ia location for R5 is:%d\n", ia_loc);
        }

    } else {
        *pIaAddr = eventId;
    }
#endif

    return rv;
}

STATIC int
_gdpll_input_array_free(int unit, uint32 eventId)
{
    int rv  = BCM_E_NONE;
    /*
     * Figure out if this eventId is under use by any active channel
     * If not, free the input array address assocuated with this event
     */
    return rv;
}


STATIC int
_gdpll_event_dest_misc_set(int unit, bcm_gdpll_input_event_t event_misc,
                            gdpll_event_dest_cfg_t *pEventDest,
                            uint32 eventId, uint32 *pIaAddr)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;
    uint32 dest;
    uint32 chkProfile;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if (event_misc == bcmGdpllInputEventPORT ||
        event_misc == bcmGdpllInputEventR5 ||
        event_misc >= bcmGdpllInputEventMax ||
        event_misc < bcmGdpllInputEventCpu) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_misc_set: Error !! Wrong param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Check if the event is already enabled, that may indicate
     * that it will be used by some active GDPLL channel.
     * if so, return error
     */
    /* Check if this event is forwared to any of IA location */
    if ((pGdpllContext->ia_loc[eventId] >= 0) &&
        (pGdpllContext->ia_loc[eventId] < BCM_GDPLL_NUM_IA_ENTRIES)) {
        /* This event is already forwarded to IA location and could
         * be in use by an active GDPLL channel.
         * Hence it cant be forwarded
         */
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_misc_set: Error !! Event busy eventId:%d IA:%d\n"), eventId, pGdpllContext->ia_loc[eventId]));
        rv = BCM_E_BUSY;
        goto exit;
    }

    /* Get the free Input array address */
    BCM_IF_ERROR_RETURN(_gdpll_input_array_alloc(unit,
        eventId, pIaAddr));

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    /* Get the eventId, that itself acts as the event forwarding database index */
    SOC_IF_ERROR_RETURN(_gdpll_event_id_get(unit, event_misc, 0, 0, &idx));
    bsl_printf("\n dest misc_set: event_misc:%d idx:%d eventId:%d\n", event_misc, idx, eventId);

    if (pEventDest->event_dest != bcmGdpllEventDestBitmap) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_misc_set: Error !! Event with unsupported event dest:%d\n"), pEventDest->event_dest));
        rv = BCM_E_PARAM;
        goto exit;
    }
    dest = pEventDest->event_dest_bitmap;
#else
    /* Since we have 150 numels of NS_TIMESYNC_TS_EVENT_FWD_CFG,
     * the miscellaneous events starts from index 128 per table 6.4 in uArch
     */
    idx = 128 + event_misc;

    if (pEventDest->event_dest == bcmGdpllEventDestCPU) {
        dest = EVENT_TS_TO_CPU;
    }else if (pEventDest->event_dest == bcmGdpllEventDestM7) {
        dest = EVENT_TS_TO_M7;
    }else if (pEventDest->event_dest == bcmGdpllEventDestALL) {
        dest = EVENT_TS_TO_ALL;
    }else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_misc_set: Error !! wrong event dest\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }
#endif

    
    chkProfile = 0;

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    
    READ_NS_REGr(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_MSBr, 0, (idx*8), &regVal);
    soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_MSBr, &regVal,
                              EVENT_IDf, eventId);
    WRITE_NS_REGr(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_MSBr, 0, (idx*8), regVal);
    bsl_printf("\n dest misc_set: fwd_cfg_%d_msb :%x\n",idx, regVal);

    READ_NS_REGr(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, 0, (idx*8), &regVal);
    dest |= soc_reg_field_get(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr,
                            regVal, DESTf);
    soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, &regVal,
                              DESTf, dest);
    soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, &regVal,
                              DPLL_IA_ADDRf, *pIaAddr);
    soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, &regVal,
                              GDPLL_TSf, pEventDest->ts_counter ? 1:0);
    soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, &regVal,
                              PPM_CHECK_ENABLEf, pEventDest->ppm_check_enable ? 1:0);
    soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, &regVal,
                              SRC_TS_COUNTERf, pEventDest->ts_counter ? 1:0);
    soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, &regVal,
                              CHK_PROFILEf, chkProfile);
    WRITE_NS_REGr(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, 0, (idx*8), regVal);
    bsl_printf("\n dest misc_set: fwd_cfg_%d_lsb :%x dest:%d ia_addr:%d \n",idx, regVal, dest, *pIaAddr);

#elif defined(BCM_MONTEREY_SUPPORT)
    READ_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, 0, &regVal);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              EVENT_IDf, eventId);
    dest |= soc_reg_field_get(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr,
                            regVal, DESTf);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DESTf, dest);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DPLL_ADDRf, *pIaAddr);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              GDPLL_TSf, pEventDest->ts_counter ? 1:0);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              PPM_CHECK_ENABLEf, pEventDest->ppm_check_enable ? 1:0);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              SRC_TS_COUNTERf, pEventDest->ts_counter ? 1:0);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              CHK_PROFILEf, chkProfile);
    WRITE_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, 0, regVal);
#endif

exit:
    return (rv);
}

STATIC int
_gdpll_event_dest_port_set (int unit, bcm_port_t port,
                bcm_gdpll_port_event_t port_event_type,
                gdpll_event_dest_cfg_t *pEventDest,
                uint32 eventId, uint32 *pIaAddr)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;
    uint32 dest;
    uint32 chkProfile;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if (port >= BCM_GDPLL_NUM_PORTS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_port_set: Error !! wrong param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Check if the event is already enabled, that may indicate
     * it will be used by some active GDPLL channel.
     * if so, return error
     */
    if ((pGdpllContext->ia_loc[eventId] >= 0) &&
        (pGdpllContext->ia_loc[eventId] < BCM_GDPLL_NUM_IA_ENTRIES)) {
        /* This event is already forwarded IA location and could be in use
         * by an active GDPLL channel. Hence it cant be forwarded
         */
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_port_set: Error !! event busy\n")));
        rv = BCM_E_BUSY;
        goto exit;
    }

    /* Get the free Input array address */
    BCM_IF_ERROR_RETURN(_gdpll_input_array_alloc(unit,
        eventId, pIaAddr));

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    SOC_IF_ERROR_RETURN(_gdpll_event_id_get(unit, port_event_type, port, port_event_type, &idx));

    if (pEventDest->event_dest != bcmGdpllEventDestBitmap) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_port_set: Error !! Event with unsupported event dest:%d\n"), pEventDest->event_dest));
        rv = BCM_E_PARAM;
        goto exit;
    }
    dest = pEventDest->event_dest_bitmap;
#elif defined(BCM_MONTEREY_SUPPORT)
    if (pEventDest->event_dest == bcmGdpllEventDestCPU) {
        dest = EVENT_TS_TO_CPU;
    }else if (pEventDest->event_dest == bcmGdpllEventDestM7) {
        dest = EVENT_TS_TO_M7;
    }else if (pEventDest->event_dest == bcmGdpllEventDestALL) {
        dest = EVENT_TS_TO_ALL;
    }else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_port_set: Error !! wrong event dest\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }
#endif

    
    chkProfile = 0;

    switch(port_event_type) {
        case bcmGdpllPortEventRXCDR:
        case bcmGdpllPortEventTXPI:
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            
            READ_NS_REGr(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_MSBr, 0, (idx*8), &regVal);
            soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_MSBr, &regVal,
                              EVENT_IDf, eventId);
            WRITE_NS_REGr(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_MSBr, 0, (idx*8), regVal);
            bsl_printf("\n dest port_set: fwd_cfg_%d_msb :%x\n",idx, regVal);

            READ_NS_REGr(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, 0, (idx*8), &regVal);
            dest |= soc_reg_field_get(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr,
                            regVal, DESTf);
            soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, &regVal,
                              DESTf, dest);
            soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, &regVal,
                              DPLL_IA_ADDRf, *pIaAddr);
            soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, &regVal,
                              GDPLL_TSf, pEventDest->ts_counter ? 1:0);
            soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, &regVal,
                              PPM_CHECK_ENABLEf, pEventDest->ppm_check_enable ? 1:0);
            soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, &regVal,
                              SRC_TS_COUNTERf, pEventDest->ts_counter ? 1:0);
            soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, &regVal,
                              CHK_PROFILEf, chkProfile);
            WRITE_NS_REGr(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, 0, (idx*8), regVal);
            bsl_printf("\n dest port_set: fwd_cfg_%d_lsb :%x\n",idx, regVal);
            break;

#elif defined(BCM_MONTEREY_SUPPORT)
            if (port_event_type == bcmGdpllPortEventRXCDR){
                idx = port;
            }else if (port_event_type == bcmGdpllPortEventTXPI) {
                idx = port + BCM_GDPLL_NUM_PORTS;
            }

            bsl_printf("GDPLL: Setting NS_TIMESYNC_TS_EVENT_FWD_CFG idx:%d IA-Addr:%d\n",
                        idx, *pIaAddr);

            READ_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, 0, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              EVENT_IDf, eventId);
            dest |= soc_reg_field_get(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr,
                                    regVal, DESTf);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DESTf, dest);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DPLL_ADDRf, *pIaAddr);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              GDPLL_TSf, pEventDest->ts_counter ? 1:0);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              PPM_CHECK_ENABLEf, pEventDest->ppm_check_enable ? 1:0);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              SRC_TS_COUNTERf, pEventDest->ts_counter ? 1:0);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              CHK_PROFILEf, chkProfile);
            WRITE_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, 0, regVal);

            break;

        case bcmGdpllPortEventRXSOF:    /* Type-0 */
        case bcmGdpllPortEventTXSOF:    /* Type-1 */
        case bcmGdpllPortEventROE:      /* Type-2 */
            if (SOC_IS_MONTEREY(unit)) {
                /* Nanosync support 32-CPRI ports for timestamping
                 * Monterey implements below 24-CPRI port maps to Nanosync
                 *   Phys-Port(phys)   GDPLL_PORT   CPRI_GDPLL_Port(TXSOF,RXSOF,ROE)
                 *       1-12            0-11          0-11
                 *      53-64            52-63        12-23
                 */
                if (port <= 11) {
                } else if ((port >= 52) && (port <= 63)){
                    port = port - 52 + 12;
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                          "event_dest_port_set: Error !! Invalid CPRI ref-port:%d\n"),
                              port));
                    rv = BCM_E_PARAM;
                    goto exit;
                }
            }

            /*
             * Having 128 Numels, register map is
             * Port-0: type-0, type-1, type-2, unused
             * Port-1: type-0, type-1, type-2, unused
             */
            if ((port >= BCM_GDPLL_NUM_CPRI_PORTS) || (port < 0)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                      "event_dest_port_set: Error !! Invalid CPRI port associated to gdpllPort:%d\n"), port));
                rv = BCM_E_PARAM;
                goto exit;
            }

            idx = port*4;
            if (port_event_type == bcmGdpllPortEventTXSOF) {
                idx = idx + 1;
            }else if (port_event_type == bcmGdpllPortEventROE) {
                idx = idx + 2;
            }

            bsl_printf("GDPLL: Setting NS_TIMESYNC_MAPPER_FWD_CFG idx:%d, IA-Addr:%d\n",
                        idx, *pIaAddr);
            READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, idx, 0, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              EVENT_IDf, eventId);
            dest |= soc_reg_field_get(unit, NS_TIMESYNC_MAPPER_FWD_CFGr,
                                    regVal, DESTf);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              DESTf, dest);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              DPLL_ADDRf, *pIaAddr);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              PPM_CHECK_ENABLEf, pEventDest->ppm_check_enable ? 1:0);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              SRC_TS_COUNTERf, 1);  /* It is TS1 counter */
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              CHK_PROFILEf, chkProfile);
            WRITE_NS_REGr(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, idx, 0, regVal);
            break;
#endif

        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                      "event_dest_port_set: Error !! wrong event\n")));
            rv = BCM_E_PARAM;
            goto exit;
    }

    /* eventId got set the forwarding to iaAddr */
    bsl_printf("GDPLL: Event Id populated :%d\n",*pIaAddr);
    pGdpllContext->ia_loc[eventId] = *pIaAddr;

exit:
    return (rv);
}

STATIC int
_gdpll_event_enable_misc_set(int unit, bcm_gdpll_input_event_t  event_misc,
                               int enable)
{
    int rv = BCM_E_NONE;
    uint32 idx = 0;
    uint32 enable_field = 0;
    uint32 regVal = 0;
    uint32 reg_enable;

    /*
    NS_MISC_CLK_EVENT_CTRL
    NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRL
    NS_BROADSYNC0_CLK_EVENT_CTRL
    NS_BROADSYNC1_CLK_EVENT_CTRL
    NS_TIMESYNC_GPIO_x_CTRL
     */
    switch (event_misc){
        case bcmGdpllInputEventCpu:
            if (event_misc == bcmGdpllInputEventCpu) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_CPU;
            }
        case bcmGdpllInputEventBS0HB:
            if (event_misc == bcmGdpllInputEventBS0HB) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS0HB;
            }
        case bcmGdpllInputEventBS1HB:
            if (event_misc == bcmGdpllInputEventBS1HB) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS1HB;
            }
        case bcmGdpllInputEventIPDM0:
            if (event_misc == bcmGdpllInputEventIPDM0) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_IPDM0;
            }
        case bcmGdpllInputEventIPDM1:
            if (event_misc == bcmGdpllInputEventIPDM1) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_IPDM1;
            }
        case bcmGdpllInputEventTS1TS:
            if (event_misc == bcmGdpllInputEventTS1TS) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_TS1;
            }
#if defined(BCM_NANOSYNC_V1_SUPPORT)
        case bcmGdpllInputEventPTPTOD1PPS:
            if (event_misc == bcmGdpllInputEventPTPTOD1PPS) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_PTPTOD1PPS;
            }
        case bcmGdpllInputEventBS0PPS:
            if (event_misc == bcmGdpllInputEventBS0PPS) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS0PPS;
            }
        case bcmGdpllInputEventBS1PPS:
            if (event_misc == bcmGdpllInputEventBS1PPS) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS1PPS;
            }
#elif defined(BCM_MONTEREY_SUPPORT)
        case bcmGdpllInputEventRSVD1IF:
            if (event_misc == bcmGdpllInputEventRSVD1IF) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_RSVD1IF;
            }
        case bcmGdpllInputEventRSVD1RF:
            if (event_misc == bcmGdpllInputEventRSVD1RF) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_RSVD1RF;
            }
#endif
        case bcmGdpllInputEventBS0ConvTC:
            if (event_misc == bcmGdpllInputEventBS0ConvTC) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS0CONV;
            }
        case bcmGdpllInputEventBS1ConvTC:
            if (event_misc == bcmGdpllInputEventBS1ConvTC) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS1CONV;
            }

            /* Write the enable flag */
            READ_NS_REGr(unit, misc_clk_event_ctrl_r, 0, 0, &regVal);
            reg_enable = soc_reg_field_get(unit, misc_clk_event_ctrl_r, regVal, ENABLEf);

            reg_enable = enable ? (reg_enable | enable_field) : (reg_enable & (~enable_field));

            soc_reg_field_set(unit, misc_clk_event_ctrl_r, &regVal,
                              ENABLEf, reg_enable);
            WRITE_NS_REGr(unit, misc_clk_event_ctrl_r, 0, 0, regVal);
            break;

#if defined(BCM_NANOSYNC_V1_SUPPORT)
        case bcmGdpllInputEventGen1:
        case bcmGdpllInputEventGen2:
        case bcmGdpllInputEventGen3:
        case bcmGdpllInputEventGen4:
        case bcmGdpllInputEventGen5:
        case bcmGdpllInputEventGen6:
        case bcmGdpllInputEventGen7:
        case bcmGdpllInputEventGen8:
        case bcmGdpllInputEventGen9:
        case bcmGdpllInputEventGen10:
        case bcmGdpllInputEventGen11:
        case bcmGdpllInputEventGen12:
        case bcmGdpllInputEventGen13:
        case bcmGdpllInputEventGen14:
        case bcmGdpllInputEventGen15:
        case bcmGdpllInputEventGen16:
            idx = event_misc - bcmGdpllInputEventGen1;
            READ_NS_REGr(unit, NSYNC_TIMESYNC_GEN_EXT_EVENT_CTRLAr, 0, 4*idx, &regVal);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_GEN_EXT_EVENT_CTRLAr, &regVal,
                              ENABLE_Nf, enable ? 1:0);
            WRITE_NS_REGr(unit, NSYNC_TIMESYNC_GEN_EXT_EVENT_CTRLAr, 0, 4*idx, regVal);
            break;
#endif
        case bcmGdpllInputEventGPIO0:
        case bcmGdpllInputEventGPIO1:
        case bcmGdpllInputEventGPIO2:
        case bcmGdpllInputEventGPIO3:
        case bcmGdpllInputEventGPIO4:
        case bcmGdpllInputEventGPIO5:
            idx = event_misc - bcmGdpllInputEventGPIO0;
            READ_NS_REGr(unit, gpio_0_ctrl_r, 0, 24*idx, &regVal);
            /*  soc_reg_field_set(unit,  gpio_0_ctrl_r, &regVal,
                               CAPTURE_ENABLEf, enable ? 1:0); */
            WRITE_NS_REGr(unit, gpio_0_ctrl_r, 0, 24*idx, regVal);
            break;

        case bcmGdpllInputEventBS0PLL:
        case bcmGdpllInputEventBS1PLL:
            idx = event_misc - bcmGdpllInputEventBS0PLL;
            READ_NS_REGr(unit, bs_pll_reg1[idx], 0, 0, &regVal);
            soc_reg_field_set(unit, bs_pll_reg1[idx], &regVal,
                              ENABLEf, enable ? 1:0);
            WRITE_NS_REGr(unit, bs_pll_reg1[idx], 0, 0, regVal);

#if defined(BCM_NANOSYNC_V1_SUPPORT)
            
#elif defined(BCM_MONTEREY_SUPPORT)
            if (event_misc == bcmGdpllInputEventBS0PLL) {
                READ_NS_REGr(unit, NS_BROADSYNC_SYNC_MODEr, 0, 0, &regVal);
                reg_enable = soc_reg_field_get(unit, NS_BROADSYNC_SYNC_MODEr, regVal, SYNC_MODEf);
                if (reg_enable) {
                    bcm_time_ts_counter_t counter;

                    counter.ts_counter= 1; /* TS1 */
                    rv = bcm_time_ts_counter_get(unit, &counter);
                    if (rv != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_PTP,
                                    (BSL_META_U(unit,
                                    "event_enable_misc_set: Error !! bcm_time_ts_counter_get\n")));
                    }

                    READ_NS_REGr(unit, NS_BS0_BS_CLK_CTRLr, 0, 0, &regVal);
                    soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regVal, ENABLEf, enable ? 1:0);
                    WRITE_NS_REGr(unit, NS_BS0_BS_CLK_CTRLr, 0, 0, regVal);

                    LOG_VERBOSE(BSL_LS_BCM_PTP,
                                    (BSL_META_U(unit,
                                    " event_enable_misc_set:[%d] CurTime[%llu/0x%llx] \n"), event_misc,
                                    (long long unsigned int)counter.ts_counter_ns,
                                    (long long unsigned int)counter.ts_counter_ns));
                }
            } else if (event_misc == bcmGdpllInputEventBS1PLL) {
                READ_NS_REGr(unit, common_ctrl_r, 0, 0, &regVal);
                reg_enable = soc_reg_field_get(unit, common_ctrl_r, regVal, RESERVEDf);
                reg_enable = ((reg_enable>>26) & 0x1);
                if (reg_enable) {
                    bcm_time_ts_counter_t counter;

                    counter.ts_counter= 1; /* TS1 */
                    rv = bcm_time_ts_counter_get(unit, &counter);
                    if (rv != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_PTP,
                                    (BSL_META_U(unit,
                                    "event_enable_misc_set: Error !! bcm_time_ts_counter_get\n")));
                    }

                    READ_NS_REGr(unit, NS_BS1_BS_CLK_CTRLr, 0, 0, &regVal);
                    soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regVal, ENABLEf, enable ? 1:0);
                    WRITE_NS_REGr(unit, NS_BS1_BS_CLK_CTRLr, 0, 0, regVal);

                    LOG_VERBOSE(BSL_LS_BCM_PTP,
                                    (BSL_META_U(unit,
                                    " event_enable_misc_set:[%d] CurTime[%llu/0x%llx] \n"), event_misc,
                                    (long long unsigned int)counter.ts_counter_ns,
                                    (long long unsigned int)counter.ts_counter_ns));
                }
            }
#endif

            break;

#if defined(BCM_MONTEREY_SUPPORT)
        case bcmGdpllInputEventLCPLL0:
        case bcmGdpllInputEventLCPLL1:
        case bcmGdpllInputEventLCPLL2:
        case bcmGdpllInputEventLCPLL3:
            idx = event_misc - bcmGdpllInputEventLCPLL0;
            READ_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, 0, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, &regVal,
                              ENABLEf, enable ? 1:0);
            WRITE_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, 0, regVal);
            break;
#endif

        case bcmGdpllInputEventPORT:
        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                      "event_enable_misc_set: Error !! wrong event\n")));
            rv = BCM_E_PARAM;
            goto exit;
    }

exit:
    return (rv);
}

STATIC int
_gdpll_event_enable_port_set(int unit, bcm_port_t port, bcm_gdpll_port_event_t port_event_type,
                                 int enable)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
#if defined(BCM_MONTEREY_SUPPORT)
    uint32 mapper_reg;
    uint32 enable_field = 0;
    uint32 reg_enable = 0;
#endif

    if (port >= BCM_GDPLL_NUM_PORTS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "event_enable_port_set: Error !! wrong port\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    switch (port_event_type) {
#if defined(BCM_MONTEREY_SUPPORT)
        case bcmGdpllPortEventRXSOF:
        case bcmGdpllPortEventTXSOF:
        case bcmGdpllPortEventROE:

            if (SOC_IS_MONTEREY(unit)) {
                /* Nanosync support 32-CPRI ports for timestamping
                 * Monterey implements below 24-CPRI port maps to Nanosync
                 *   Phys-Port(phys)   GDPLL_PORT   CPRI_GDPLL_Port(TXSOF,RXSOF,ROE)
                 *       1-12            0-11          0-11
                 *      53-64            52-63        12-23
                 */
                if (port <= 11) {
                } else if ((port >= 52) && (port <= 63)){
                    port = port - 52 + 12;
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                          "event_enable_port_set: Error !! Invalid CPRI ref-port:%d\n"),
                              port));
                    rv = BCM_E_PARAM;
                    goto exit;
                }
            }

            /* GDPLL has support to 32 CPRI ports */
            if ((port >= BCM_GDPLL_NUM_CPRI_PORTS) || (port < 0)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                      "event_enable_port_set: Error !! wrong port or non-cpri\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }

            /* Program NS_TIMESYNC_MAPPER_PORT_ENABLE_x */
            enable_field = 1 << (port_event_type - bcmGdpllPortEventRXSOF);
            mapper_reg = port/8;

            READ_NS_REGr(unit, mapper_port_enable_regs[mapper_reg], 0, 0, &regVal);
            reg_enable = soc_reg_field_get(unit, mapper_port_enable_regs[mapper_reg], regVal, mapper_port_enable[port]);

            reg_enable = enable ? (reg_enable | enable_field) : (reg_enable & (~enable_field));

            soc_reg_field_set(unit, mapper_port_enable_regs[mapper_reg], &regVal, mapper_port_enable[port], reg_enable);
            WRITE_NS_REGr(unit, mapper_port_enable_regs[mapper_reg], 0, 0, regVal);

            break;
#endif

        case bcmGdpllPortEventRXCDR:
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            /* Program NSYNC_TIMESYNC_RX_CDR_EVENT_CTRLA */
            READ_NS_REGr(unit, NSYNC_TIMESYNC_RX_CDR_EVENT_CTRLAr, port, 0, &regVal);
            if (soc_reg_field_get(unit, NSYNC_TIMESYNC_RX_CDR_EVENT_CTRLAr, regVal, ENABLEf) != enable) {
                soc_reg_field_set(unit, NSYNC_TIMESYNC_RX_CDR_EVENT_CTRLAr, &regVal, ENABLEf, enable ? 1:0);
                WRITE_NS_REGr(unit, NSYNC_TIMESYNC_RX_CDR_EVENT_CTRLAr, port, 0, regVal);
            }
#elif defined(BCM_MONTEREY_SUPPORT)
            /* Program NS_TIMESYNC_RX_CDR_EVENT_CTRL */
            READ_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, port, 0, &regVal);
            if (soc_reg_field_get(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, regVal, ENABLEf) != enable) {
                soc_reg_field_set(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, &regVal, ENABLEf, enable ? 1:0);
                WRITE_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, port, 0, regVal);
            }
#endif
            break;

        case bcmGdpllPortEventTXPI:
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            /* Program NSYNC_TIMESYNC_TX_PI_CLK_EVENT_CTRLA */
            READ_NS_REGr(unit, NSYNC_TIMESYNC_TX_PI_CLK_EVENT_CTRLAr, port, 0, &regVal);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_TX_PI_CLK_EVENT_CTRLAr, &regVal, ENABLEf, enable ? 1:0);
            WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TX_PI_CLK_EVENT_CTRLAr, port, 0, regVal);
#elif defined(BCM_MONTEREY_SUPPORT)
            /* Program NS_TIMESYNC_TX_PI_CLK_EVENT_CTRL */
            READ_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, port, 0, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, &regVal, ENABLEf, enable ? 1:0);
            WRITE_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, port, 0, regVal);
#endif
            break;

        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "event_enable_port_set: Error !! wrong port event\n")));
            rv = BCM_E_PARAM;
          goto exit;
    }

exit:
    return (rv);
}

#ifdef BCM_MONTEREY_SUPPORT
STATIC int
_gdpll_event_roe_52b_set(int unit, bcm_port_t port, int enable)
{
    int rv = BCM_E_NONE;
    uint32 enable_field = 0;
    uint32 regVal = 0;
    uint32 reg_enable = 0;

    /* Program NS_TIMESYNC_MAPPER_TYPE2_FORMAT */

    enable_field = 1 << port;

    READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, port, 0, &regVal);
    reg_enable = soc_reg_field_get(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, regVal, CONV_ENf);

    reg_enable = enable ? (reg_enable | enable_field) : (reg_enable & (~enable_field));

    soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, &regVal, CONV_ENf, reg_enable);
    WRITE_NS_REGr(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, port, 0, regVal);

    return (rv);
}
#endif

STATIC int
_gdpll_chan_update_set(int unit, int chan)
{
    /* Update NS_GDPLL_IA_UPDATE_CONFIG */
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    uint32 eventId_fb;
    uint32 eventId_ref;

    if (chan >= BCM_GDPLL_NUM_CHANNELS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_update_set: Error !! wrong channel\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    eventId_fb = pGdpllContext->dpll_chan[chan].eventId_fb;
    eventId_ref = pGdpllContext->dpll_chan[chan].eventId_ref;

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, 0, (chan*4), &regVal);
#else
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, 0, &regVal);
#endif

    /* Set the feedback for the channel */
    if ((eventId_fb < BCM_GDPLL_NUM_INPUT_EVENTS) &&
        (pGdpllContext->ia_loc[eventId_fb] < BCM_GDPLL_NUM_IA_ENTRIES)){
        bsl_printf("\nGDPLL: chan_update: feedback Id:0x%x\n", pGdpllContext->ia_loc[eventId_fb]);

        soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ia_update_fb_id_f,
                    pGdpllContext->ia_loc[eventId_fb]);
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_update_set: Error !! fb event not found\n")));
        rv = BCM_E_NOT_FOUND;
        goto exit;
    }

    /* Set the reference for the channel */
    if (pGdpllContext->dpll_chan[chan].phaseConterRef == 0){
        if ((eventId_ref < BCM_GDPLL_NUM_INPUT_EVENTS) &&
            (pGdpllContext->ia_loc[eventId_ref] < BCM_GDPLL_NUM_IA_ENTRIES)){
            bsl_printf("\nGDPLL: chan_update: ref Id:0x%x\n", pGdpllContext->ia_loc[eventId_ref]);

            soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ia_update_ref_id_f,
                    pGdpllContext->ia_loc[eventId_ref]);

        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_update_set: Error !! ref event not found\n")));
            rv = BCM_E_NOT_FOUND;
            goto exit;
        }
    }

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, 0, (chan*4), regVal);
#else
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, 0, regVal);
#endif

exit:
    return (rv);
}

int
_bcm_common_gdpll_chan_get(int unit, int chan,
                           bcm_gdpll_chan_t *pGdpllChan)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    *pGdpllChan = pGdpllContext->dpll_chan[chan].gdpll_chan_config;

    return (rv);
}

STATIC int
_gdpll_chan_enable_set(int unit, int chan, int enable)
{
   /* NS_GDPLL_IA_UPDATE_CONFIG */
    int rv = BCM_E_NONE;
    uint32 regVal = 0;

    if (chan >= BCM_GDPLL_NUM_CHANNELS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_enable_set: Error !! invalid channel\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    if (enable) {
#if defined(BCM_NANOSYNC_V1_SUPPORT)
        READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, 0, (chan*4), &regVal);
#else
        READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, 0, &regVal);
#endif
        if (soc_reg_field_get(unit, NS_GDPLL_IA_UPDATE_CONFIGr, regVal, ia_update_enable_f)) {
            /* Already in enable state */
            return rv;
        }

        /* Clear the status flags if any */
        WRITE_NS_REGr(unit, ia_ts_pair_available0_r, 0, 4*chan/32, 1<<(chan%32));
        WRITE_NS_REGr(unit, ia_in_status0_r, 0, 4*chan/32, 1<<(chan%32));
        /* Enable the intr for the gdpll channel */
        WRITE_NS_REGr(unit, ia_in_enable0_r, 0, 4*chan/32, (1<<(chan%32)));
    }

    bsl_printf("GDPLL: Set enable:%d on GDPLL channel:%d\n", enable, chan);
#if defined(BCM_NANOSYNC_V1_SUPPORT)
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, 0, (chan*4), &regVal);
    soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ia_update_enable_f, enable ? 1:0);
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, 0, (chan*4), regVal);
#else
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, 0, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ia_update_enable_f, enable ? 1:0);
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, 0, regVal);
#endif

exit:
    return (rv);
}

STATIC int
_gdpll_capture_enable_m7_set(int unit, int enable)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    /* NS_MISC_CLK_EVENT_CTRL */
    bsl_printf("GDPLL: gdpll_capture_enable_m7_set enable:%d\n", enable);

    READ_NS_REGr(unit, misc_clk_event_ctrl_r, 0, 0, &regVal);
    soc_reg_field_set(unit, misc_clk_event_ctrl_r, &regVal, GDPLL_CAPTURE_ENABLEf, enable ? 1:0);
    WRITE_NS_REGr(unit, misc_clk_event_ctrl_r, 0, 0, regVal);

    return (rv);
}

#if defined(BCM_MONTEREY_SUPPORT)
int
_gdpll_capture_enable_cpu_set(int unit, int enable)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;

    READ_NS_REGr(unit, misc_clk_event_ctrl_r, 0, 0, &regVal);
    soc_reg_field_set(unit, misc_clk_event_ctrl_r, &regVal, TIME_CAPTURE_ENABLEf, enable ? 1:0);
    WRITE_NS_REGr(unit, misc_clk_event_ctrl_r, 0, 0, regVal);

    return (rv);
}
#endif

STATIC int
_gdpll_flush(int unit)
{
    int rv = BCM_E_NONE;
    /* NS_GDPLL_GDPLL_COMMON_CTRL */

    WRITE_NS_REGr(unit, NS_GDPLL_GDPLL_COMMON_CTRLr, 0, 0, 1);
    sal_usleep(100);
    WRITE_NS_REGr(unit, NS_GDPLL_GDPLL_COMMON_CTRLr, 0, 0, 0);

    return (rv);
}

STATIC int
_gdpll_chan_out_txpi_set(int unit, int chan, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
#if defined(BCM_NANOSYNC_V1_SUPPORT)
    uint32 dest_ctrl = 0;
#endif

    if ((chan >= BCM_GDPLL_NUM_CHANNELS) &&
        (port >= BCM_GDPLL_NUM_PORTS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_out_txpi_set: Error !! invalid param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    dest_ctrl &= ~HW_IF_FIELD;  /* Set HW_IF=0 */
    dest_ctrl |= (16 + port);     /* Set HW_EVENT_ID to be port starting from 16 */

    /* NS_GDPLL_NCO_UPDATE_CONTROL */
    READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, 0, (chan*4), &regVal);
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_TYPE_Nf, 1);  /* HW_FC */
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_CTRL_Nf, dest_ctrl);
    WRITE_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, 0, (chan*4), regVal);

#elif defined(BCM_MONTEREY_SUPPORT)
    /* NS_GDPLL_NCO_UPDATE_CONTROL */
    READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, chan, 0, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_IDf, port);
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_TYPEf, 1);
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_TYPEf, 0);
    WRITE_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, chan, 0, regVal);
#endif

exit:
    return (rv);
}

STATIC int
_gdpll_chan_out_misc_set(int unit, int chan,
                        bcm_gdpll_output_event_t event)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
#if defined(BCM_NANOSYNC_V1_SUPPORT)
    uint32 dest_ctrl = 0;
#elif defined(BCM_MONTEREY_SUPPORT)
    uint32 dest_hw_id = 0;
    uint32 dest_hw_type = 0;
    uint32 dpllAddr, addrOffset;
    uint32 enableReg = 0;
    int count;
#endif

    if (chan >= BCM_GDPLL_NUM_CHANNELS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_out_misc_set: Error !! invalid param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    switch ((int)event) {
        case bcmGdpllOutputEventBSPLL0:
        case bcmGdpllOutputEventBSPLL1:
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            dest_ctrl &= ~HW_IF_FIELD;  /* Set HW_IF=0 */
            dest_ctrl |= (0 + (event-bcmGdpllOutputEventBSPLL0)); /* 0/1 */
#elif defined(BCM_MONTEREY_SUPPORT)
        case bcmGdpllOutputEventLCPLL0:
        case bcmGdpllOutputEventLCPLL1:
        case bcmGdpllOutputEventLCPLL2:
        case bcmGdpllOutputEventLCPLL3:
            bsl_printf("GDPLL: Setting NCO out control for BSPLL/LCPLL\n");
            dest_hw_id = BCM_GDPLL_NUM_PORTS +
                    (event - bcmGdpllOutputEventBSPLL0);
            dest_hw_type = 0;
#endif
            break;

        case bcmGdpllOutputEventTS0:
        case bcmGdpllOutputEventTS1:
            bsl_printf("GDPLL: Setting NCO out control for TS0/TS1\n");
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            dest_ctrl |= HW_IF_FIELD;                          /* Set HW_IF=1 */
            dest_ctrl |= (0 + (event-bcmGdpllOutputEventTS0)); /* 0/1 */
#elif defined(BCM_MONTEREY_SUPPORT)
            dest_hw_id = event - bcmGdpllOutputEventTS0;
            dest_hw_type = 1;
#endif
            break;

#if defined(BCM_MONTEREY_SUPPORT)
        /* This is supposed to be BCN counter which is not there,
         * but now R5 reads the NCO and updates the BCN counter
         */
        case bcmGdpllOutputEventBCN:
            bsl_printf("GDPLL: Setting NCO out control for BCN\n");
            dest_hw_id = 1;
            dest_hw_type = 1;
            break;
#endif

        case bcmGdpllOutputEventNTPTOD:
            bsl_printf("GDPLL: Setting NCO out control for NTP-TOD\n");
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            dest_ctrl &= ~HW_IF_FIELD;  /* Set HW_IF=0 */
            dest_ctrl |= 3;             /* 3 */
#elif defined(BCM_MONTEREY_SUPPORT)
            dest_hw_id = 2;
            dest_hw_type = 1;
#endif
            break;

        case bcmGdpllOutputEventPTPTOD:
            bsl_printf("GDPLL: Setting NCO out control for PTP-TOD\n");
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            dest_ctrl &= ~HW_IF_FIELD;  /* Set HW_IF=0 */
            dest_ctrl |= 2;             /* 3 */
#elif defined(BCM_MONTEREY_SUPPORT)
            dest_hw_id = 3;
            dest_hw_type = 1;
#endif
            break;

#if defined(BCM_NANOSYNC_V1_SUPPORT)
        case bcmGdpllOutputEventPTPTODRef:
            bsl_printf("GDPLL: Setting NCO out control for PTPTODRef \n");
            dest_ctrl |= HW_IF_FIELD;   /* Set HW_IF=1 */
            dest_ctrl |= 2;             /* 2 */
            break;

        case bcmGdpllOutputEventGen1:
        case bcmGdpllOutputEventGen2:
        case bcmGdpllOutputEventGen3:
        case bcmGdpllOutputEventGen4:
        case bcmGdpllOutputEventGen5:
        case bcmGdpllOutputEventGen6:
        case bcmGdpllOutputEventGen7:
        case bcmGdpllOutputEventGen8:
        case bcmGdpllOutputEventGen9:
        case bcmGdpllOutputEventGen10:
        case bcmGdpllOutputEventGen11:
        case bcmGdpllOutputEventGen12:
            dest_ctrl &= ~HW_IF_FIELD;  /* Set HW_IF=0 */
            dest_ctrl |= 4 + (event - bcmGdpllOutputEventGen1);  /* 4/5/.../15 */
            break;
#endif

        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_out_misc_set: Error !! invalid event\n")));
            rv = BCM_E_PARAM;
            goto exit;
    }

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, 0, (chan*4), &regVal);

    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_TYPE_Nf, 1);  /* HW_FC */
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_CTRL_Nf, dest_ctrl);

    WRITE_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, 0, (chan*4), regVal);
#elif defined(BCM_MONTEREY_SUPPORT)
    READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, chan, 0, &regVal);


    if ((event == bcmGdpllOutputEventBCN) || (event == bcmGdpllOutputEventNTPTOD) ||
        (event == bcmGdpllOutputEventPTPTOD)) {
        

        dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
        /* Set the Offset location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);

        soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_TYPEf, 0);
        if (event == bcmGdpllOutputEventBCN) {
            m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BCN_UPDATE_LOC, chan);
        } else if (event == bcmGdpllOutputEventNTPTOD) {
            m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_UPDATE_LOC, chan);
            m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_DPLL_OFFSET_LOC, dpllAddr+addrOffset);
            LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                                " Writing DpllOffset_addr:0x%08x@0x%08x for NTP-TOD \n"),
                     (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_NTPTOD_DPLL_OFFSET_LOC)));
        } else if (event == bcmGdpllOutputEventPTPTOD) {
            m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_UPDATE_LOC, chan);
            m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_DPLL_OFFSET_LOC, dpllAddr+addrOffset);
            LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                                " Writing DpllOffset_addr:0x%08x@0x%08x for PTP-TOD \n"),
                     (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_PTPTOD_DPLL_OFFSET_LOC)));
        }

        /* Enable the Channel Output array status reporting to R5 */
        READ_NS_REGr(unit, nco_out_enable0_r, 0, 4*chan/32, &enableReg);
        enableReg |= 1<<(chan%32);
        soc_reg_field_set(unit, nco_out_enable0_r, &enableReg, INTENf, enableReg);
        WRITE_NS_REGr(unit, nco_out_enable0_r, 0, 4*chan/32, enableReg);

        if (chan < BCM_GDPLL_NUM_CHANNELS) {
            /* Enable in the NS interrupt line */
            sal_usleep(100);
            READ_NS_REGr(unit, ts_int_enable0_r, 0, 0, &enableReg);
            for (count = 0; count<(BCM_GDPLL_NUM_CHANNELS/32); count++) {
                soc_reg_field_set(unit, ts_int_status_r, &enableReg, gdpll_oa_status[count], 1);
            }
            WRITE_NS_REGr(unit, ts_int_enable0_r, 0, 0, enableReg);
        }

    } else {
        soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_IDf, dest_hw_id);
        soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_TYPEf, 1);
        soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_TYPEf, dest_hw_type);
    }
    WRITE_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, chan, 0, regVal);
#endif  /* BCM_NANOSYNC_V1_SUPPORT */

exit:
    return (rv);
}

#if 0  
STATIC int
_gdpll_chan_out_enable_set(int unit, int chan, int enable)
{
    int rv = BCM_E_NONE;
    /* NS_GDPLL_NCO_UPDATE_COMMON_CONTROL */

    return (rv);
}
#endif

int
_gdpll_m7_status (int unit, uint8 *pBuff)
{
    int rv = BCM_E_NONE;
    int count;
    int version_size = DPLL_FW_VERSION_SIZE;
    uint32 *pVersion;

    uint32 m7AliveIndi = BCM_M7_DTCM_BASE + DPLL_ALIVE_OFFSET;
    uint32 versionAddr = BCM_M7_DTCM_BASE + DPLL_FW_VERSION_OFFSET;

    pVersion = (uint32 *)pBuff;
    if (pBuff == NULL) {
        return BCM_E_PARAM;
    }

    version_size = version_size / 4;
    m7_mem_write(unit, m7AliveIndi, 0);
    sal_usleep(100);
    if(m7_mem_read(unit, m7AliveIndi)) {
        /* M7 is alive */
        for (count=0; count < version_size; count++) {
            *pVersion = swap32(m7_mem_read(unit, versionAddr));
            versionAddr = versionAddr + 4;
            pVersion++;
        }
    } else {
        return BCM_E_FAIL;
    }

    return rv;
}

STATIC int
_gdpll_chan_config_dpll_set(int unit, int chan,
                        dpll_param_t *pDpllParam)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    uint32 stateNum;
    uint32 count;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 *pData;

    if ((chan >= BCM_GDPLL_NUM_CHANNELS) ||
        (pDpllParam == NULL)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_config_dpll_set: Error !! invalid param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Clear the param memory */
    sal_memset(&m7DpllParam, 0, sizeof(m7DpllParam));

    /*  m7DpllParam.debugMode = pDpllParam->debugMode; */

    m7DpllParam.m7_dpll_config.DPLLNumOfStates = pDpllParam->dpll_config.dpll_num_states;
    for (stateNum = 0; stateNum < m7DpllParam.m7_dpll_config.DPLLNumOfStates; stateNum++) {
        m7DpllParam.m7_dpll_config.K1Shift[stateNum] = pDpllParam->dpll_config.k1Shift[stateNum];
        m7DpllParam.m7_dpll_config.K1K2Shift[stateNum] = pDpllParam->dpll_config.k1k2Shift[stateNum];

        m7DpllParam.m7_dpll_config.K1[stateNum] = swap32(pDpllParam->dpll_config.k1[stateNum]);
        m7DpllParam.m7_dpll_config.K1K2[stateNum] = swap32(pDpllParam->dpll_config.k1k2[stateNum]);
        m7DpllParam.m7_dpll_config.LockDetThres[stateNum] = swap32(pDpllParam->dpll_config.lockDetThres[stateNum]);
        m7DpllParam.m7_dpll_config.DwellCount[stateNum] = swap16(pDpllParam->dpll_config.dwell_count[stateNum]);
        u64_H(m7DpllParam.m7_dpll_config.PhaseErrorLimiterThres[stateNum]) =
                    swap64_h(pDpllParam->dpll_config.phase_error_limiter_thres[stateNum]);
        u64_L(m7DpllParam.m7_dpll_config.PhaseErrorLimiterThres[stateNum]) =
                    swap64_l(pDpllParam->dpll_config.phase_error_limiter_thres[stateNum]);
        m7DpllParam.m7_dpll_config.IdumpMod[stateNum] = swap16(pDpllParam->dpll_config.idump_mod[stateNum]);
    }

    m7DpllParam.m7_dpll_config.LockIndicatorThresholdLo = swap32(pDpllParam->dpll_config.lockIndicatorThresholdLo);
    m7DpllParam.m7_dpll_config.LockIndicatorThresholdHi = swap32(pDpllParam->dpll_config.lockIndicatorThresholdLHi);
    m7DpllParam.m7_dpll_config.NCObits = pDpllParam->dpll_config.nco_bits;
    m7DpllParam.m7_dpll_config.PhaseCounterRef = pDpllParam->dpll_config.phase_counter_ref;
    m7DpllParam.m7_dpll_config.PhaseErrorShift = pDpllParam->dpll_config.phase_error_shift;
    u64_H(m7DpllParam.m7_dpll_config.NominalLoopFilter) = swap64_h(pDpllParam->dpll_config.nominal_loop_filter);
    u64_L(m7DpllParam.m7_dpll_config.NominalLoopFilter) = swap64_l(pDpllParam->dpll_config.nominal_loop_filter);
    m7DpllParam.m7_dpll_config.InvertPhaseError = pDpllParam->dpll_config.invert_phase_error;

    m7DpllParam.m7_dpll_config.NormalizePhaseError = pDpllParam->dpll_config.norm_phase_error;
    m7DpllParam.m7_dpll_config.PhaseErrorThres0 = swap32(pDpllParam->dpll_config.phase_error_thres0);
    m7DpllParam.m7_dpll_config.PhaseErrorThres1 = swap32(pDpllParam->dpll_config.phase_error_thres1);
    m7DpllParam.m7_dpll_config.HoldoverFilterCoeff = swap16(pDpllParam->dpll_config.holdover_filter_coeff);

    /* DPLL State setting */
    m7DpllParam.m7_dpll_state.DPLLState = pDpllParam->dpll_state.dpll_state;
    u64_H(m7DpllParam.m7_dpll_state.LoopFilter) = swap64_h(pDpllParam->dpll_state.loop_filter);
    u64_L(m7DpllParam.m7_dpll_state.LoopFilter) = swap64_l(pDpllParam->dpll_state.loop_filter);

    u64_H(m7DpllParam.m7_dpll_state.Offset) = swap64_h(pDpllParam->dpll_state.offset);
    u64_L(m7DpllParam.m7_dpll_state.Offset) = swap64_l(pDpllParam->dpll_state.offset);

    u64_H(m7DpllParam.m7_dpll_state.PhaseCounter) = swap64_h(pDpllParam->dpll_state.phase_counter);
    u64_L(m7DpllParam.m7_dpll_state.PhaseCounter) = swap64_l(pDpllParam->dpll_state.phase_counter);

    u64_H(m7DpllParam.m7_dpll_state.PhaseCounterDelta) = swap64_h(pDpllParam->dpll_state.phaseCounterDelta);
    u64_L(m7DpllParam.m7_dpll_state.PhaseCounterDelta) = swap64_l(pDpllParam->dpll_state.phaseCounterDelta);

    u64_H(m7DpllParam.m7_dpll_state.AccumPhaseError) = swap64_h(pDpllParam->dpll_state.accumPhaseError);
    u64_L(m7DpllParam.m7_dpll_state.AccumPhaseError) = swap64_l(pDpllParam->dpll_state.accumPhaseError);

    m7DpllParam.m7_dpll_state.DwellCounter = swap32(pDpllParam->dpll_state.dwell_counter);
    m7DpllParam.m7_dpll_state.LockDetFilter = swap32(pDpllParam->dpll_state.lockDetFilter);
    m7DpllParam.m7_dpll_state.InitPhaseCounterFlag = pDpllParam->dpll_state.init_flag;
    m7DpllParam.m7_dpll_state.InitOffsetFlag = pDpllParam->dpll_state.init_offset_flag;
    m7DpllParam.m7_dpll_state.PhaseCounterN = swap16(pDpllParam->dpll_state.phaseCounterN);
    m7DpllParam.m7_dpll_state.PhaseCounterM = swap16(pDpllParam->dpll_state.phaseCounterM);
    m7DpllParam.m7_dpll_state.PhaseCounterFrac = swap16(pDpllParam->dpll_state.phaseCounterFrac);
    m7DpllParam.m7_dpll_state.IdumpCounter = swap16(pDpllParam->dpll_state.idumpCounter);

    m7DpllParam.m7_dpll_state.LockIndicator = 0;
    m7DpllParam.m7_dpll_state.LossOfLockIndicator = 0;
    m7DpllParam.m7_dpll_state.LargeErrorIndicator = 0;

    /* Exclude the debugMode location as it will be programmed in debugMode API */
    pData = (uint32 *)&m7DpllParam; pData++;
    dpllAddr += sizeof(uint32);

    for (count = 0; count < sizeof(m7DpllParam)-4;) {
        m7_mem_write(unit, dpllAddr, swap32(*pData));
        pData++;
        count += sizeof(uint32);
        dpllAddr += sizeof(uint32);
    }

    /* Update the channel for phase counter reference */
        bsl_printf("GDPLL: Set the phase counter ref\n");
#if defined(BCM_NANOSYNC_V1_SUPPORT)
        READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, 0, (chan*4), &regVal);
        soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ia_update_phaseref_f,
            pDpllParam->dpll_config.phase_counter_ref ? 1:0);
        WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, 0, (chan*4), regVal);
#else
        READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, 0, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ia_update_phaseref_f,
            pDpllParam->dpll_config.phase_counter_ref ? 1:0);
        WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, 0, regVal);
#endif

exit:
    return (rv);
}

STATIC int
_gdpll_chan_config_dpll_get(int unit, int chan,
                        dpll_param_t *pDpllParam)
{
    int rv = BCM_E_NONE;
    uint32 count;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 *pData;
    uint32 stateNum;

    pData = (uint32 *)&m7DpllParam;
    if ((chan >= BCM_GDPLL_NUM_CHANNELS) ||
        (pDpllParam == NULL)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_config_dpll_get: Error !! invalid param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Read the DTCM locations for DPLL config */
    for (count = 0; count < sizeof(m7_dpll_param_t);) {
        *pData = swap32(m7_mem_read(unit, dpllAddr));
        pData++;
        count += sizeof(uint32);
        dpllAddr += sizeof(uint32);
    }

    pDpllParam->debugMode = m7DpllParam.debugMode;
    pDpllParam->dpll_config.dpll_num_states = m7DpllParam.m7_dpll_config.DPLLNumOfStates;

    for (stateNum = 0; stateNum < m7DpllParam.m7_dpll_config.DPLLNumOfStates; stateNum++) {
        pDpllParam->dpll_config.k1Shift[stateNum] = m7DpllParam.m7_dpll_config.K1Shift[stateNum];
        pDpllParam->dpll_config.k1k2Shift[stateNum] = m7DpllParam.m7_dpll_config.K1K2Shift[stateNum];

        pDpllParam->dpll_config.k1[stateNum] = swap32(m7DpllParam.m7_dpll_config.K1[stateNum]);
        pDpllParam->dpll_config.k1k2[stateNum] = swap32(m7DpllParam.m7_dpll_config.K1K2[stateNum]);
        pDpllParam->dpll_config.lockDetThres[stateNum] = swap32(m7DpllParam.m7_dpll_config.LockDetThres[stateNum]);
        pDpllParam->dpll_config.dwell_count[stateNum] = swap16(m7DpllParam.m7_dpll_config.DwellCount[stateNum]);
        u64_H(pDpllParam->dpll_config.phase_error_limiter_thres[stateNum]) =
            swap64_h(m7DpllParam.m7_dpll_config.PhaseErrorLimiterThres[stateNum]);
        u64_L(pDpllParam->dpll_config.phase_error_limiter_thres[stateNum]) =
            swap64_l(m7DpllParam.m7_dpll_config.PhaseErrorLimiterThres[stateNum]);
        pDpllParam->dpll_config.idump_mod[stateNum] = swap16(m7DpllParam.m7_dpll_config.IdumpMod[stateNum]);
    }

    pDpllParam->dpll_config.lockIndicatorThresholdLo = swap32(m7DpllParam.m7_dpll_config.LockIndicatorThresholdLo);
    pDpllParam->dpll_config.lockIndicatorThresholdLHi = swap32(m7DpllParam.m7_dpll_config.LockIndicatorThresholdHi);

    pDpllParam->dpll_config.nco_bits = m7DpllParam.m7_dpll_config.NCObits;
    pDpllParam->dpll_config.phase_counter_ref = m7DpllParam.m7_dpll_config.PhaseCounterRef;
    pDpllParam->dpll_config.phase_error_shift = m7DpllParam.m7_dpll_config.PhaseErrorShift;
    u64_H(pDpllParam->dpll_config.nominal_loop_filter) = swap64_h(m7DpllParam.m7_dpll_config.NominalLoopFilter);
    u64_L(pDpllParam->dpll_config.nominal_loop_filter) = swap64_l(m7DpllParam.m7_dpll_config.NominalLoopFilter);
    pDpllParam->dpll_config.invert_phase_error = m7DpllParam.m7_dpll_config.InvertPhaseError;

    pDpllParam->dpll_config.norm_phase_error = m7DpllParam.m7_dpll_config.NormalizePhaseError;
    pDpllParam->dpll_config.phase_error_thres0 = swap32(m7DpllParam.m7_dpll_config.PhaseErrorThres0);
    pDpllParam->dpll_config.phase_error_thres1 = swap32(m7DpllParam.m7_dpll_config.PhaseErrorThres1);
    pDpllParam->dpll_config.holdover_filter_coeff = swap16(m7DpllParam.m7_dpll_config.HoldoverFilterCoeff);

    /* DPLL State setting */
    pDpllParam->dpll_state.dpll_state = m7DpllParam.m7_dpll_state.DPLLState;

    pDpllParam->dpll_state.dwell_counter = swap32(m7DpllParam.m7_dpll_state.DwellCounter);
    pDpllParam->dpll_state.lockDetFilter = swap32(m7DpllParam.m7_dpll_state.LockDetFilter);
    pDpllParam->dpll_state.init_flag = m7DpllParam.m7_dpll_state.InitPhaseCounterFlag;
    pDpllParam->dpll_state.init_offset_flag = m7DpllParam.m7_dpll_state.InitOffsetFlag;
    u64_H(pDpllParam->dpll_state.loop_filter) = swap64_h(m7DpllParam.m7_dpll_state.LoopFilter);
    u64_L(pDpllParam->dpll_state.loop_filter) = swap64_l(m7DpllParam.m7_dpll_state.LoopFilter);

    u64_H(pDpllParam->dpll_state.offset) = swap64_h(m7DpllParam.m7_dpll_state.Offset);
    u64_L(pDpllParam->dpll_state.offset) = swap64_l(m7DpllParam.m7_dpll_state.Offset);

    u64_H(pDpllParam->dpll_state.phase_counter) = swap64_h(m7DpllParam.m7_dpll_state.PhaseCounter);
    u64_L(pDpllParam->dpll_state.phase_counter) = swap64_l(m7DpllParam.m7_dpll_state.PhaseCounter);

    u64_H(pDpllParam->dpll_state.phaseCounterDelta) = swap64_h(m7DpllParam.m7_dpll_state.PhaseCounterDelta);
    u64_L(pDpllParam->dpll_state.phaseCounterDelta) = swap64_l(m7DpllParam.m7_dpll_state.PhaseCounterDelta);

    u64_H(pDpllParam->dpll_state.accumPhaseError) = swap64_h(m7DpllParam.m7_dpll_state.AccumPhaseError);
    u64_L(pDpllParam->dpll_state.accumPhaseError) = swap64_l(m7DpllParam.m7_dpll_state.AccumPhaseError);
    pDpllParam->dpll_state.phaseCounterN = swap16(m7DpllParam.m7_dpll_state.PhaseCounterN);
    pDpllParam->dpll_state.phaseCounterM = swap16(m7DpllParam.m7_dpll_state.PhaseCounterM);
    pDpllParam->dpll_state.phaseCounterFrac = swap16(m7DpllParam.m7_dpll_state.PhaseCounterFrac);
    pDpllParam->dpll_state.idumpCounter = swap16(m7DpllParam.m7_dpll_state.IdumpCounter);

    pDpllParam->dpll_state.lockIndicator = m7DpllParam.m7_dpll_state.LockIndicator;
    pDpllParam->dpll_state.lossOfLockIndicator = m7DpllParam.m7_dpll_state.LossOfLockIndicator;
    pDpllParam->dpll_state.largeErrorIndicator = m7DpllParam.m7_dpll_state.LargeErrorIndicator;

exit:
    return (rv);
}

STATIC int
_gdpll_chan_debug_mode_set(int unit, int chan,
            bcm_gdpll_debug_mode_t debugMode)
{
    int rv = BCM_E_NONE;
    uint32 dpll_debugMode;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);

    /* Info - debugEnable debugMode addresses visible by M7 */
    dpll_debugMode = dpllAddr + 0/*DPLL_DEBUG_MODE*/;

    /* M7 ITCM configuration, for the desired debug mode */
    m7_mem_write(unit, dpll_debugMode, debugMode);

    return (rv);
}

int
_bcm_common_gdpll_holdover_set(int unit, int chan, uint8 enable)
{
    int rv = BCM_E_NONE;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 addrOff_holdover;
    uint32 addrOff_InitOffset;
    uint32 regVal;
    int fb_id=0;
    gdpll_context_t *pGdpllContext = NULL;

    addrOff_holdover = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.HoldoverFlag);
    addrOff_InitOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.InitOffsetFlag);

    /* Set the holdover flag */
    regVal = m7_mem_read(unit, dpllAddr+addrOff_holdover);
    regVal = enable ? regVal | 0x1 : regVal & (~0x1);
    m7_mem_write(unit, dpllAddr+addrOff_holdover, regVal);

    pGdpllContext = pGdpllCtx[unit];
    if (pGdpllContext->dpll_chan[chan].phaseConterRef == 0){
        /* Set the phasecounter flag to enable the HW to trigger M7 on
         * feedback OR the fb/ref pair
         */
        READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, 0, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ia_update_phaseref_f,
             enable ? 1:0);
        WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, 0, regVal);
    }

    /* Set the initOffset flag if required*/
    if (enable == 0)
    {
        regVal = m7_mem_read(unit, dpllAddr+addrOff_InitOffset);
        regVal |= 0x10000;
        m7_mem_write(unit, dpllAddr+addrOff_InitOffset, regVal);
    } else {
        if (pGdpllContext->dpll_chan[chan].phaseConterRef == 1){

            uint32 reg_hi=0, reg_low=0;
            READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, 0, &regVal);
            fb_id = soc_reg_field_get(unit, NS_GDPLL_IA_UPDATE_CONFIGr, regVal,ia_update_fb_id_f);

#define NS_GDPLL_INPUT_ARRAY_TS_000_LSB (0x03271004)
#define NS_GDPLL_INPUT_ARRAY_TS_000_MSB (0x03271000)

            reg_hi = (NS_GDPLL_INPUT_ARRAY_TS_000_MSB + (fb_id*8));
            reg_low = (NS_GDPLL_INPUT_ARRAY_TS_000_LSB + (fb_id*8));

            soc_iproc_setreg(unit, reg_hi, 0xc0de);
            soc_iproc_setreg(unit, reg_low, 0xdeadbeaf);
        }
    }

    return rv;
}

int
_bcm_common_gdpll_holdover_get(int unit, int chan, uint8 *enable)
{
    int rv = BCM_E_NONE;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 addrOff_holdover;
    uint32 regVal;

    addrOff_holdover = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.HoldoverFlag);

    /* Set the holdover flag */
    regVal = m7_mem_read(unit, dpllAddr+addrOff_holdover);
    *enable = regVal & 0x1;

    return rv;
}

int
_bcm_common_gdpll_pbo_set(int unit, int chan)
{
    int rv = BCM_E_NONE;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 addrOff_InitOffset;
    uint32 regVal;

    addrOff_InitOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.InitOffsetFlag);

    /* Set the initOffset flag */
    regVal = m7_mem_read(unit, dpllAddr+addrOff_InitOffset);
    regVal |= 0x10000;
    m7_mem_write(unit, dpllAddr+addrOff_InitOffset, regVal);

    return rv;
}

int
_bcm_common_gdpll_LoopFilter_set(int unit, int chan, int64 LoopFilter)
{
    int rv = BCM_E_NONE;

    dpll_param_t dpllParam;

    /* Check if the InitOffsetFlag is set */
    if(_gdpll_chan_config_dpll_get(unit, chan,
                &dpllParam) != BCM_E_NONE) {
        rv = BCM_E_INTERNAL;
        goto exit;
    }
    bsl_printf("\n _gdpll_LoopFilter_set: PrevLF[0x%llx]\n",
                (long long unsigned int)dpllParam.dpll_state.loop_filter);

    u64_H(dpllParam.dpll_state.loop_filter) = u64_H(LoopFilter);
    u64_L(dpllParam.dpll_state.loop_filter) = u64_L(LoopFilter);

    if(_gdpll_chan_config_dpll_set(unit, chan,
                &dpllParam) != BCM_E_NONE) {
        rv = BCM_E_INTERNAL;
        goto exit;
    }

    bsl_printf("\n _gdpll_LoopFilter_set: RevLF[0x%llx]\n",
                (long long unsigned int)dpllParam.dpll_state.loop_filter);

exit:
    return rv;
}

int
bcm_common_gdpll_offset_set(int unit, int chan,
            uint64 Offset)
{
    int rv = BCM_E_NONE;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 addrOffset;
    dpll_param_t dpllParam;

    /* Check if the InitOffsetFlag is set */
    if(_gdpll_chan_config_dpll_get(unit, chan,
                &dpllParam) != BCM_E_NONE) {
        rv = BCM_E_INTERNAL;
        goto exit;
    }

    if (dpllParam.dpll_state.init_offset_flag) {
        bsl_printf("GDPLL: offset_set: Error !! InitOffsetFlag is set\n");
        rv = BCM_E_INTERNAL;
        goto exit;
    }

    addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);

    /* M7 DTCM configuration, for the Offset update */
    m7_mem_write(unit, dpllAddr+addrOffset, u64_L(Offset));
    m7_mem_write(unit, dpllAddr+addrOffset+4, u64_H(Offset));

exit:
    return (rv);
}

int
bcm_common_gdpll_offset_get(int unit, int chan,
            uint64 *pOffset)
{
    int rv = BCM_E_NONE;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 addrOffset;

    addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);

    /* M7 DTCM configuration, for the Offset update */
    u64_L(*pOffset) = m7_mem_read(unit, dpllAddr+addrOffset);
    u64_H(*pOffset) = m7_mem_read(unit, dpllAddr+addrOffset+4);

    return (rv);
}

/*STATIC*/ int
_gdpll_chan_debug_mode_get(int unit, int chan,
            bcm_gdpll_debug_mode_t *pDebugMode)
{
    int rv = BCM_E_NONE;
    uint32 dpll_debugMode;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);

    if (!pDebugMode || (chan >= BCM_GDPLL_NUM_CHANNELS)) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Info - debugEnable debugMode addresses visible by M7 */
    dpll_debugMode = dpllAddr + 0/*DPLL_DEBUG_MODE*/;

    /* M7 ITCM configuration, for the desired debug mode */
    *pDebugMode = m7_mem_read(unit, dpll_debugMode);

exit:
    return (rv);
}

STATIC int
_gdpll_deinit(int unit)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    /* Destroy protection mutex. */
    if (NULL != pGdpllContext->mutex) {
        sal_mutex_destroy(pGdpllContext->mutex);
        pGdpllContext->mutex = NULL;
    }

    return (rv);
}

void
gdpll_cb_thread (void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    int chan;
    int flag = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if (pGdpllContext == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                   "gdpll_cb_thread: Error !! context not setup\n")));
        return;
    }

    while(1) {
        /* Wait on the signal */
        (void)sal_sem_take(pGdpllContext->cb_sem, sal_sem_FOREVER);

        for (chan = 0; chan < BCM_GDPLL_NUM_CHANNELS; chan++) {
            /* Notify, if there is a "state change" OR "if Error flag is
             * set with different error code" */
            flag = 0;
            if (pGdpllContext->dpll_chan[chan].curr_state.state !=
                pGdpllContext->dpll_chan[chan].prev_state.state) {
                flag = 1;
            } else if ((pGdpllContext->dpll_chan[chan].curr_state.state & BCM_GDPLL_CHAN_STATE_ERROR) &&
                       (pGdpllContext->dpll_chan[chan].prev_state.error_code !=
                        pGdpllContext->dpll_chan[chan].curr_state.error_code)) {
                flag = 1;
            }

            if (flag) {
                /* Callback with chan status info */
                pGdpllContext->dpll_chan[chan].prev_state = pGdpllContext->dpll_chan[chan].curr_state;
                if (pGdpllContext->gdpll_cb[bcmGdpllCbChanState]) {
                    pGdpllContext->dpll_chan[chan].curr_state.chan = chan;
                    pGdpllContext->gdpll_cb[bcmGdpllCbChanState](unit,
                        pGdpllContext->pGdpll_cb_data[bcmGdpllCbChanState],
                        (uint32 *)&pGdpllContext->dpll_chan[chan].curr_state, bcmGdpllCbChanState);
                }
            }
        }
    }
}

void
gdpll_debug_buffer_send_thread(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    uint32 regVal;
    uint32 dpllDebugFlag = BCM_M7_DTCM_BASE + DPLL_DEBUG_ENABLE_OFFSET;

    if (pGdpllContext == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                  "gdpll_debug_buffer_send_thread: Error !! context not setup\n")));
        return;
    }

    while(1) {
     /* Wait on the signal from the ISR */
        (void)sal_sem_take(pGdpllContext->debug_sem, sal_sem_FOREVER);
        if (pGdpllContext->debug_cb && m7_mem_read (unit, dpllDebugFlag)) {
            /* Return of this cb confirms the buffer consumption */
            pGdpllContext->debug_cb(unit, pGdpllContext->pUserData,
                    (BCM_GDPLL_DEBUG_BUFFER_START + (pGdpllContext->debugRdIdx * 4)),
                    BCM_GDPLL_DEBUG_THRESHOLD);
        }

        READ_NS_REGr(unit, debug_m7dg_rstate_r, 0, 0, &regVal);
        pGdpllContext->debugRdIdx = regVal;
    }
}

#if 0
STATIC int
_bcm_update_tod_ptpntp(int unit, int chan_num, bcm_gdpll_output_event_t event)
{
    uint32 nco_hi, nco_lo;

    if (chan_num >= BCM_GDPLL_NUM_CHANNELS || chan_num < 0) {
        return BCM_E_INTERNAL;
    }

    /* Read the OA for the correction value */
    READ_NS_REGr(unit, NS_GDPLL_OUTPUT_ARRAY_000_MSBr, chan_num, 0, &nco_hi);
    READ_NS_REGr(unit, NS_GDPLL_OUTPUT_ARRAY_000_LSBr, chan_num, 0, &nco_lo);

    if (event == bcmGdpllOutputEventNTPTOD) {
        WRITE_NTP_TIME_FREQ_CONTROLr(unit, nco_lo);
    } else if (event == bcmGdpllOutputEventPTPTOD) {
        WRITE_IEEE1588_TIME_FREQ_CONTROLr(unit, nco_lo);
    }

    return BCM_E_NONE;
}
#endif

STATIC void
gdpll_state_change_chan_capture(int unit, int chan)
{
    gdpll_context_t *pGdpllContext;
    dpll_param_t dpllParam;

    pGdpllContext = pGdpllCtx[unit];
    if (NULL == pGdpllContext) {
        return;
    }

    if (pGdpllContext->dpll_chan[chan].enable) {
        /* If chan is enabled, poll on channel status */

        /* Construct the current chan SW state (Lock, Large-Error) */
        if (_gdpll_chan_config_dpll_get(unit, chan, &dpllParam) != BCM_E_NONE) {
            return;
        }

        pGdpllContext->dpll_chan[chan].curr_state.state  = BCM_GDPLL_CHAN_STATE_ENABLE;
        pGdpllContext->dpll_chan[chan].curr_state.state |=
            dpllParam.dpll_state.largeErrorIndicator ? BCM_GDPLL_CHAN_STATE_ERROR:0;
        pGdpllContext->dpll_chan[chan].curr_state.error_code =
            dpllParam.dpll_state.largeErrorIndicator ? bcmGdpllChanErrorDpllLError:bcmGdpllChanErrorNone;

        /* If the DPLL is not tracking large error, get the lock indication */
        if (!dpllParam.dpll_state.largeErrorIndicator) {
            pGdpllContext->dpll_chan[chan].curr_state.state |=
                dpllParam.dpll_state.lockIndicator ? BCM_GDPLL_CHAN_STATE_LOCK:0;
        }
    } else {
        pGdpllContext->dpll_chan[chan].curr_state.state  = 0;
        pGdpllContext->dpll_chan[chan].curr_state.error_code = 0;
    }

}

STATIC void
gdpll_state_change_capture(int unit)
{
    int chan;
    gdpll_context_t *pGdpllContext;

    pGdpllContext = pGdpllCtx[unit];
    if (NULL == pGdpllContext) {
        return;
    }

    /* Retrieve state of all the enabled channels */
    for (chan = 0; chan < BCM_GDPLL_NUM_CHANNELS; chan++) {
        gdpll_state_change_chan_capture(unit, chan);
    }
}

#if defined(BCM_MONTEREY_SUPPORT)
/* Deferred call to get the RSVD1 message to user over callback */
void gdpll_intr_dpc(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);
    uint32 *pBuff = (uint32 *)d1;

    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if (pGdpllContext && (pGdpllContext->gdpll_cb[bcmGdpllCbRsvd1])) {
        /* Return of this cb confirms the data consumption */
        pGdpllContext->gdpll_cb[bcmGdpllCbRsvd1](unit, pGdpllContext->pGdpll_cb_data[bcmGdpllCbRsvd1],
                    pBuff, bcmGdpllCbRsvd1);
    }
}
#endif

/* Nanosync generic interrupt */
#if defined(BCM_CMICX_SUPPORT)
void gdpll_intr(int unit, void *data)
#else
void gdpll_intr(int unit, uint32 int_status, uint32 int_enable)
#endif
{
    uint32 int_status_clear = 0;  /* Interrupt status clear */
    uint32 stat, stat0, stat1, stat2;  /* Interrupt status */
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    int chan_num, chan_idx;
    uint32 regVal = 0;
    int post_cb_sem = 0;
    uint32 intEnable = 0;
    uint32 count = 0;
    uint32 in_enable=0;
#if defined(BCM_CMICX_SUPPORT)
    uint32 int_status = 0;

    /* Read interrupt status */
    READ_NS_REGr(unit, ts_int_status_r, 0, 0, &int_status);
#endif

#if 1   
    /* TS0_CNT_OFFSET_UPDATED interrupt */
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, int_ts0_off_update_f)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "soc_nanosync_intr: int_ts0_off_update_f \n")));
        /* Set W1TC to clear the flag */
        soc_reg_field_set(unit, ts_int_status_r, &int_status_clear, int_ts0_off_update_f, 1);
    }

    /* TS1_CNT_OFFSET_UPDATED interrupt */
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, int_ts1_off_update_f)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "soc_nanosync_intr: int_ts1_off_update_f \n")));
        /* Set W1TC to clear the flag */
        soc_reg_field_set(unit, ts_int_status_r, &int_status_clear, int_ts1_off_update_f, 1);
    }
#endif

    /* M7_CORE_INT_STATUS interrupt */
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, int_m7_int_status_f)) {

        /* Check the status in NS_GDPLL_M7_CORE_INT_STATUS for LOCKUP,
         * SYSRESET_REQ, TXEV, ERRORf interrupts
         */
        READ_NS_REGr(unit, m7_core_int_status_r, 0, 0, &regVal);
        READ_NS_REGr(unit, m7_core_int_enable_r, 0, 0, &intEnable);
        intEnable &= regVal;

        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
            "soc_nanosync_intr: M7_CORE_INT_STATUS-0x%x\n"), regVal));

        /* Process ERROR interrupt */
        if (soc_reg_field_get(unit, m7_core_int_status_r, intEnable, ERRORf)) {
            /* Read ERROR_INFO and ack the interrupt */
            READ_NS_REGr(unit, m7_error_info_r, 0, 0, &regVal);
            WRITE_NS_REGr(unit, m7_error_info_int_r, 0, 0, 0);

            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "soc_nanosync_intr: ERROR INFO:0x%x\n"), regVal));

            /* Process M7 interrupt for state change */
            if (regVal & (INTR_STATE_CHANGE << 8)) {
                /* This is a state change SW interrupt on one of the GDPLL channels */
                gdpll_state_change_capture(unit);
                post_cb_sem = 1;    /* Notify the State change */
            }
        }

        /* Process LOCKUP interrupt */
        if (soc_reg_field_get(unit, m7_core_int_status_r, intEnable, LOCKUPf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: LOCKUP\n")));
            WRITE_NS_REGr(unit, m7_core_int_status_r, 0, 0, 0x1);
        }

        /* Process SYSRESET_REQ interrupt */
        if (soc_reg_field_get(unit, m7_core_int_status_r, intEnable, SYSRESET_REQf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: SYSRESET_REQ\n")));
            WRITE_NS_REGr(unit, m7_core_int_status_r, 0, 0, 0x2);
        }

        /* Process TXEV interrupt */
        if (soc_reg_field_get(unit, m7_core_int_status_r, intEnable, TXEVf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: TXEV\n")));
            WRITE_NS_REGr(unit, m7_core_int_status_r, 0, 0, 0x4);
        }

        /* Set W1TC to clear the flag */
        soc_reg_field_set(unit, ts_int_status_r, &int_status_clear, int_m7_int_status_f, 1);
    }

    /* Write W1TC to clear the flags in NS_TS_INT_STATUS */
    WRITE_NS_REGr(unit, ts_int_status_r, 0, 0, int_status_clear);

    /*
     * In HW error intr, clear the M7-SW state change indications if any,
     * captured in M7_CORE_INT_STATUS.
     */
    /* GDPLL_IA_0/1/2/3_STATUS interrupt */
    count = 0;
    while (count < (BCM_GDPLL_NUM_CHANNELS/32)) {
#if defined(BCM_NANOSYNC_V1_SUPPORT)
        if (soc_reg_field_get(unit, ts_int_status_r, int_status, GDPLL_IA_STATUSf)) {
#else
        if (soc_reg_field_get(unit, ts_int_status_r, int_status, gdpll_ia_status[count])) {
#endif
            READ_NS_REGr(unit, ia_in_status0_r, 0, 4*count, &stat);
            READ_NS_REGr(unit, ia_in_enable0_r, 0, 4*count, &in_enable);

            chan_num = count*32; chan_idx = 0;
            while (chan_idx < 32) {
                if ( pGdpllContext->dpll_chan[chan_num+chan_idx].enable &&
                     ((stat >> chan_idx) & 0x1) &&
                     ((in_enable >> chan_idx) & 0x1)) {

                    /* Clear the intr for the channel */
                    in_enable &= ~(1<<chan_idx);
                    WRITE_NS_REGr(unit, ia_in_enable0_r, 0, 4*count, in_enable);

                    /* Capture the error code and post sem for notifying the application */
#if defined(BCM_NANOSYNC_V1_SUPPORT)
                    READ_NS_REGr(unit, NS_GDPLL_IA_ERROR_CODEr, 0, chan_num+chan_idx, &regVal);
#else
                    READ_NS_REGr(unit, NS_GDPLL_IA_ERROR_CODEr, chan_num+chan_idx, 0, &regVal);
#endif
                    pGdpllContext->dpll_chan[chan_num+chan_idx].curr_state.state =
                            BCM_GDPLL_CHAN_STATE_ENABLE | BCM_GDPLL_CHAN_STATE_ERROR;
                    if (regVal==0) {
                        pGdpllContext->dpll_chan[chan_num+chan_idx].curr_state.error_code = bcmGdpllChanErrorTSOverload;
                    } else {
                        pGdpllContext->dpll_chan[chan_num+chan_idx].curr_state.error_code = regVal & 0x3;
                    }

                    post_cb_sem = 1;

                    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                           " Error: chan-%d error_code-0x%x\n"), chan_num+chan_idx, regVal));
                }
                chan_idx++;
            }

            WRITE_NS_REGr(unit, ia_in_status0_r, 0, 4*count, stat);
        }
        count++;
    }

    /* Post callback sema for state change notification */
    if (post_cb_sem) {
        sal_sem_give(pGdpllContext->cb_sem);
    }

#if defined(BCM_MONTEREY_SUPPORT)
    /* RSVD1_MESSAGE_RX_STATUS interrupt */
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, BRCM_RESERVED_IPROC_3_1f/*RSVD1_MESSAGE_RX_STATUSf*/)) {
        if (pGdpllContext) {
            if (pGdpllContext->gdpll_cb[bcmGdpllCbRsvd1]) {
                /* Read the RSVD1 status registers
                 * MESSAGE_CAPTURE_0_STATUS - BRCM_RESERVED_IPROC_6
                 * MESSAGE_CAPTURE_1_STATUS - BRCM_RESERVED_IPROC_7
                 * MESSAGE_CAPTURE_2_STATUS - BRCM_RESERVED_IPROC_8
                 */
                 READ_NS_REGr(unit, BRCM_RESERVED_IPROC_6r, 0, 0, &cbData[0]);
                 READ_NS_REGr(unit, BRCM_RESERVED_IPROC_7r, 0, 0, &cbData[1]);
                 READ_NS_REGr(unit, BRCM_RESERVED_IPROC_8r, 0, 0, &cbData[2]);
#if 0
                /* Return of this cb confirms the data consumption */
                pGdpllContext->gdpll_cb[bcmGdpllCbRsvd1](unit, pGdpllContext->pGdpll_cb_data[bcmGdpllCbRsvd1],
                    cbData, bcmGdpllCbRsvd1);
#else
                sal_dpc(gdpll_intr_dpc,
                        INT_TO_PTR(unit), cbData, NULL, NULL, NULL);
#endif
            }
        }

        /* Clear the leaf level status at BRCM_RESERVED_IPROC_9 */
        WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_9r, 0, 0, 0x3);
    }
#endif

#if 0   /* This was enabled to test the NTP/PTP TOD corrections from host */
    /* GDPLL_OA_x_STATUS interrupt */
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, GDPLL_OA_0_STATUSf) ||
        soc_reg_field_get(unit, ts_int_status_r, int_status, GDPLL_OA_1_STATUSf) ||
        soc_reg_field_get(unit, ts_int_status_r, int_status, GDPLL_OA_2_STATUSf) ||
        soc_reg_field_get(unit, ts_int_status_r, int_status, GDPLL_OA_3_STATUSf)) {

        READ_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS0r, 0, 0, &stat0);
        READ_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS1r, 0, 0, &stat1);
        READ_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS2r, 0, 0, &stat2);
        READ_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS3r, 0, 0, &stat3);

        
        if (1 == soc_property_get(unit,"tod_ptpntp_host_update", -1)) {

            /* Check if this NCO update is for NTPTOD */
            chan_num = m7_mem_read(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_UPDATE_LOC);
            if (chan_num < 32 && (stat0 & (1<<chan_num))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventNTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS0r, 0, 0, (1<<chan_num));
            } else if (chan_num < 64 && (stat1 & (1<<(chan_num-32)))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventNTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS1r, 0, 0, (1<<(chan_num-32)));
            } else if (chan_num < 96 && (stat2 & (1<<(chan_num-64)))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventNTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS2r, 0, 0, (1<<(chan_num-64)));
            } else if (chan_num < 128 && (stat3 & (1<<(chan_num-96)))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventNTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS3r, 0, 0, (1<<(chan_num-96)));
            }
        }

        if (1 == soc_property_get(unit,"tod_ptpntp_host_update", -1)) {

            /* Check if this NCO update is for PTPTOD */
            chan_num = m7_mem_read(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_UPDATE_LOC);
            if (chan_num < 32 && (stat0 & (1<<chan_num))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventPTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS0r, 0, 0, (1<<chan_num));
            } else if (chan_num < 64 && (stat1 & (1<<(chan_num-32)))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventPTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS1r, 0, 0, (1<<(chan_num-32)));
            } else if (chan_num < 96 && (stat2 & (1<<(chan_num-64)))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventPTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS2r, 0, 0, (1<<(chan_num-64)));
            } else if (chan_num < 128 && (stat3 & (1<<(chan_num-96)))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventPTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS3r, 0, 0, (1<<(chan_num-96)));
            }
        }
    }
#endif

    /* GDPLL_NCO_OVERLOAD_x_STATUS interrupt */
#if defined(BCM_NANOSYNC_V1_SUPPORT)
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, GDPLL_NCO_OVERLOAD_STATUSf)) {
#else
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, GDPLL_NCO_OVERLOAD_0_STATUSf) ||
        soc_reg_field_get(unit, ts_int_status_r, int_status, GDPLL_NCO_OVERLOAD_1_STATUSf) ||
        soc_reg_field_get(unit, ts_int_status_r, int_status, GDPLL_NCO_OVERLOAD_2_STATUSf) ||
        soc_reg_field_get(unit, ts_int_status_r, int_status, GDPLL_NCO_OVERLOAD_3_STATUSf)) {
#endif

        for (count = 0; count < (BCM_GDPLL_NUM_CHANNELS/32); count++) {
            READ_NS_REGr(unit, nco_overload_status0_r, 0, 4*count, &stat0);
            WRITE_NS_REGr(unit, nco_overload_status0_r, 0, 4*count, stat0);

            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NCO_OVERLOAD(%d:%d): 0x%x\n"), count*32, 31+count*32, stat0));
        }
    }

    /* GDPLL_COMMON_STATUS interrupt */
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, int_common_status_f)) {
        READ_NS_REGr(unit, NS_GDPLL_GDPLL_COMMON_INT_STATUSr, 0, 0, &stat0);
        WRITE_NS_REGr(unit, NS_GDPLL_GDPLL_COMMON_INT_STATUSr, 0, 0, stat0);

        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: COMMON_INT_STATUS(0:3): 0x%x\n"), stat0));
    }

    /* TS_CPU_FIFO_ECC_ERR_STATUS interrupt */
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, int_fifo1_ecc_err_f)) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: int_fifo1_ecc_err_f\n")));
    }
#if defined(BCM_NANOSYNC_V1_SUPPORT)
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, int_fifo2_ecc_err_f)) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: int_fifo2_ecc_err_f\n")));
    }
#endif

    /* GDPLL_MEM_ECC_ERR_STATUS interrupt */
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, int_mem_ecc_err_f)) {
        READ_NS_REGr(unit, NS_GDPLL_NCO_MEM_ECC_STATUSr, 0, 0, &stat0);
        READ_NS_REGr(unit, NS_GDPLL_WTCHDG_MEM_ECC_STATUSr, 0, 0, &stat1);
        READ_NS_REGr(unit, NS_GDPLL_IA_MEM_ECC_STATUSr, 0, 0, &stat2);

        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NCO_MEM_ECC_STATUS: 0x%x\n"), stat0));
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_GDPLL_WTCHDG_MEM_ECC_STATUS: 0x%x\n"), stat1));
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_GDPLL_IA_MEM_ECC_STATUS: 0x%x\n"), stat2));
    }

    /* M7_TCM_ECC_ERR_STATUS interrupt */
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, int_tcm_ecc_err_f)) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: M7_TCM_ECC_ERR_STATUS\n")));
    }

#if defined(BCM_MONTEREY_SUPPORT)
    /* RoE_TS_ERR interrupt */
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, ROE_TS_ERRf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                  " Error: RoE_TS_ERRf \n")));
    }
#endif

    /* TS_CAPTURE_OVRD interrupt */
    if (soc_reg_field_get(unit, ts_int_status_r, int_status, int_ts_capture_ovrd_f)) {

        for (count = 0; count < (BCM_GDPLL_NUM_PORTS/32); count++) {
            READ_NS_REGr(unit, rx_cdr_event_ovrd_status_r, 0, 4*count, &stat0);
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_RX_CDR_EVENT_OVRD_STATUS%d:0x%x\n"), count, stat0));

            READ_NS_REGr(unit, tx_pi_event_ovrd_status_r, 0, 4*count, &stat0);
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_TX_PI_EVENT_OVRD_STATUS%d:0x%x\n"), count, stat0));
        }

        READ_NS_REGr(unit, misc_event_ovrd_status_r, 0, 0, &stat0);
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: MISC_EVENT_OVRD_STATUS:0x%x\n"), stat0));

#if defined(BCM_MONTEREY_SUPPORT)
        READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_RX_TYPE0_OVRD_STATUSr, 0, 0, &stat0);
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_TIMESYNC_MAPPER_RX_TYPE0_OVRD_STATUS:0x%x\n"), stat0));

        READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_RX_TYPE1_OVRD_STATUSr, 0, 0, &stat0);
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_TIMESYNC_MAPPER_RX_TYPE1_OVRD_STATUS:0x%x\n"), stat0));

        READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_RX_TYPE2_OVRD_STATUSr, 0, 0, &stat0);
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_TIMESYNC_MAPPER_RX_TYPE2_OVRD_STATUS:0x%x\n"), stat0));
#endif
    }

#if defined(BCM_CMICX_SUPPORT) && defined(BCM_NANOSYNC_V1_SUPPORT)
    soc_cmic_intr_enable(unit, SOC_NS_INTR1);
#else
#if 0 /* Interrupt is re-enabled in time.c */
    /* Re-enable GDPLL NS IRQ */
    soc_cmicm_intr2_enable(unit, SOC_GDPLL_NS_INTR_POS);
#endif
#endif
}


/* Nanosync debug buffer interrupt */
#if defined(BCM_CMICX_SUPPORT) && defined(BCM_NANOSYNC_V1_SUPPORT)
void soc_nanosync_debug_intr(int unit, void *data)
{
#else
void soc_nanosync_debug_intr(void *unit_vp, void *d1, void *d2,
                           void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);
#endif
    uint32 regVal = 0;
    uint32 int_status = 0;  /* Interrupt status */
    uint32 int_enable = 0;  /* Interrupt enable */
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    /* Read the GDPLL interrupt status */
    READ_NS_REGr(unit, debug_m7dg_int_status_r, 0, 0, &int_status);

    /* Process threshold interrupt */
    if (soc_reg_field_get(unit, debug_m7dg_int_status_r, int_status, THRESHOLD_REACHEDf)) {
        READ_NS_REGr(unit, debug_m7dg_rstate_r, 0, 0, &regVal);
        regVal = (pGdpllContext->debugRdIdx + BCM_GDPLL_DEBUG_THRESHOLD) % BCM_GDPLL_DEBUG_BUFFER_SIZE;
        WRITE_NS_REGr(unit, debug_m7dg_rstate_r, 0, 0, regVal);

        int_enable |= INTR_GDPLL_DEBUG_THR;
        /* Signal the thread for buffer read */
        if (pGdpllContext->debug_sem) {
            sal_sem_give(pGdpllContext->debug_sem);
        } else {
            uint32 dpllDebugFlag = BCM_M7_DTCM_BASE + DPLL_DEBUG_ENABLE_OFFSET;

            if (pGdpllContext->debug_cb && m7_mem_read (unit, dpllDebugFlag)) {
                /* Return of this cb confirms the buffer consumption */
                pGdpllContext->debug_cb(unit, pGdpllContext->pUserData,
                    (BCM_GDPLL_DEBUG_BUFFER_START + (pGdpllContext->debugRdIdx * 4)),
                    BCM_GDPLL_DEBUG_THRESHOLD);
            }

            READ_NS_REGr(unit, debug_m7dg_rstate_r, 0, 0, &regVal);
            pGdpllContext->debugRdIdx = regVal;
        }
    }

    if (soc_reg_field_get(unit, debug_m7dg_int_status_r, int_status, m7dg_int_status_ovflw_f)) {
        int_enable |= INTR_GDPLL_DEBUG_OF;
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "OVERFLOW !!\n")));
        m7_mem_write(unit, (BCM_M7_DTCM_BASE + DPLL_DEBUG_ENABLE_OFFSET), 0);
    }

    if (soc_reg_field_get(unit, debug_m7dg_int_status_r, int_status, m7dg_int_status_ecc_err_mem0f)) {
        int_enable |= INTR_GDPLL_DEBUG_ECC0;
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "ECC_ERR_MEM0 !!\n")));
    }

    if (soc_reg_field_get(unit, debug_m7dg_int_status_r, int_status, m7dg_int_status_ecc_err_mem1f)) {
        int_enable |= INTR_GDPLL_DEBUG_ECC1;
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "ECC_ERR_MEM1 !!\n")));
    }

    /* Clear the interrupt status, which is W1TC */
    WRITE_NS_REGr(unit, debug_m7dg_int_status_r, 0, 0, int_status);

    /* Re-enable the interrupts */
    READ_NS_REGr(unit, debug_m7dg_int_enable_r, 0, 0, &regVal);
    int_enable |= soc_reg_field_get(unit, debug_m7dg_int_enable_r, regVal, INTENf);
    soc_reg_field_set(unit, debug_m7dg_int_enable_r, &regVal, INTENf, int_enable);
    WRITE_NS_REGr(unit, debug_m7dg_int_enable_r, 0, 0, int_enable);

#if defined(BCM_CMICX_SUPPORT) && defined(BCM_NANOSYNC_V1_SUPPORT)
    soc_cmic_intr_enable(unit, SOC_NS_INTR1);
    soc_cmic_intr_enable(unit, SOC_NS_DBG_INTR);
#else
    /* Re-enable GDPLL debug buffer interrupt */
    soc_cmicm_intr2_enable(unit, SOC_GDPLL_NS_INTR_POS | SOC_GDPLL_NS_INTR_DEBUG_POS);
#endif
}

void clear_m7_memory(int unit)
{
    uint32 m7_itcmaddr = BCM_M7_ITCM_BASE;
    uint32 m7_dtcmaddr = BCM_M7_DTCM_BASE;
    uint32 i;
    uint32 itcm_size = BCM_M7_ITCM_SIZE;
    uint32 dtcm_size = BCM_M7_DTCM_SIZE;
    uint32 rval;

    /* Pull M7 out of reset for TCM initialization */
    rval = 0;
#if defined(BCM_NANOSYNC_V1_SUPPORT)
    READ_NS_REGr(unit, M_7_SS_CTRLr, 0, 0, &rval);
    soc_reg_field_set(unit, M_7_SS_CTRLr, &rval, SYS_RESETf, 0x0);
    soc_reg_field_set(unit, M_7_SS_CTRLr, &rval, POWERUP_RESETf, 0x0);
    WRITE_NS_REGr(unit, M_7_SS_CTRLr, 0, 0, rval);
#else
    READ_M7SS_CTRLr(unit, &rval);
    soc_reg_field_set(unit, M7SS_CTRLr, &rval, SYS_RESETf, 0x0);
    soc_reg_field_set(unit, M7SS_CTRLr, &rval, POWERUP_RESETf, 0x0);
    WRITE_M7SS_CTRLr(unit, rval);
#endif

    /* Clear ITCM */
    for (i=0; i<itcm_size;) {
        WRITE_NS_MEM(unit, m7_itcmaddr, 0);
        i = i+4;
        m7_itcmaddr = m7_itcmaddr+4;
    }

    /* Clear DTCM */
    for (i=0; i<dtcm_size;) {
        WRITE_NS_MEM(unit, m7_dtcmaddr, 0);
        i = i+4;
        m7_dtcmaddr = m7_dtcmaddr+4;
    }
}

int
_bcm_common_gdpll_init(int unit)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    uint32 alloc_sz = 0;
    uint32 count = 0;
    gdpll_context_t *pGdpllContext = NULL;
    sal_thread_t thread_id;
    uint32 reg;
    uint32 regCount;
    int debug_thread_pri = 0;
#if defined(BCM_CMICX_SUPPORT) && defined(BCM_NANOSYNC_V1_SUPPORT)
    soc_cmic_intr_handler_t handle;
#endif

#if defined(BCM_MONTEREY_SUPPORT)
    /* Clear the tdpll state structure */
    for (count = 0; count < NUM_TDPLL_INSTANCE; count++) {
        gdpll_tdpll_state.tdpll_instace[count] = BCM_GDPLL_NUM_IA_ENTRIES;
    }
#endif

#if defined(BCM_XGS_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)
    _bcm_gdpll_scache_alloc(unit);
#endif

    /* Allocate time config structure. */
    alloc_sz = sizeof(gdpll_context_t);
    pGdpllContext = sal_alloc(alloc_sz, "GDPLL module");
    if (NULL == pGdpllContext) {
        rv = BCM_E_MEMORY;
        goto exit;
    }

#if defined(BCM_DNX_SUPPORT)
    sal_memset(pGdpllContext, 0, alloc_sz);

    /* Set all events available */
    for (count = 0; count < BCM_GDPLL_NUM_INPUT_EVENTS; count++) {
        pGdpllContext->ia_loc[count] = BCM_GDPLL_NUM_IA_ENTRIES;
    }
#else
    if (SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(_gdpll_reinit(unit, pGdpllContext));
    } else {
        sal_memset(pGdpllContext, 0, alloc_sz);

        /* Set all events available */
        for (count = 0; count < BCM_GDPLL_NUM_INPUT_EVENTS; count++) {
            pGdpllContext->ia_loc[count] = BCM_GDPLL_NUM_IA_ENTRIES;
        }
    }   /* SOC_WARM_BOOT */
#endif

    /* Create protection mutex. */
    pGdpllContext->mutex = NULL;
    pGdpllContext->mutex = sal_mutex_create("GDPLL mutex");
    if (NULL == pGdpllContext->mutex) {
        _gdpll_deinit(unit);
        return (BCM_E_MEMORY);
    }

    pGdpllContext->debug_cb = NULL;
    if (!SOC_WARM_BOOT(unit)) {
        clear_m7_memory(unit);

        

        /* Configure debug threshold level */
        READ_NS_REGr(unit, debug_m7dg_threshold_r, 0, 0, &regVal);
        soc_reg_field_set(unit, debug_m7dg_threshold_r, &regVal, THRESHOLDf, BCM_GDPLL_DEBUG_THRESHOLD);
        WRITE_NS_REGr(unit, debug_m7dg_threshold_r, 0, 0, regVal);

        /* Enable the GDPLL debug feature */
        /* 1->0 transition to clear the buffer pointers */
        READ_NS_REGr(unit, debug_m7dg_enable_r, 0, 0, &regVal);
        soc_reg_field_set(unit, debug_m7dg_enable_r, &regVal, ENABLEf, 1);
        WRITE_NS_REGr(unit, debug_m7dg_enable_r, 0, 0, regVal);
        soc_reg_field_set(unit, debug_m7dg_enable_r, &regVal, ENABLEf, 0);
        WRITE_NS_REGr(unit, debug_m7dg_enable_r, 0, 0, regVal);
        soc_reg_field_set(unit, debug_m7dg_enable_r, &regVal, ENABLEf, 1);
        WRITE_NS_REGr(unit, debug_m7dg_enable_r, 0, 0, regVal);
    }   /* SOC_WARM_BOOT */

    pGdpllCtx[unit] = pGdpllContext;

    pGdpllContext->debugRdIdx = 0;

    debug_thread_pri = soc_property_get(unit,"gdpll_debug_thread_pri", -1);
    pGdpllContext->debug_sem = NULL;
    if ((debug_thread_pri >= 0) &&  (debug_thread_pri < 255)) {
        /* Create the debug buffer semaphore */
        pGdpllContext->debug_sem = sal_sem_create("gdpll_debug_buff", sal_sem_COUNTING, 0);
        if (pGdpllContext->debug_sem == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "_bcm_common_gdpll_init: Error !! failed to create the sem\n")));
              return (BCM_E_MEMORY);
        }

        /* Create the debug thread */
        thread_id = sal_thread_create("GDPLL Debug thread", SAL_THREAD_STKSZ, debug_thread_pri,
                        gdpll_debug_buffer_send_thread, INT_TO_PTR(unit));
        if (thread_id == SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "GDPLL Error !! debug thread create failed\n")));
            return BCM_E_INTERNAL;
        }
    }

    /* Create the GDPLL CB thread */
    pGdpllContext->cb_sem = sal_sem_create("gdpll_cb", sal_sem_COUNTING, 0);
    if (pGdpllContext->cb_sem == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "_bcm_common_gdpll_init: Error !! failed to create the cb_sem\n")));
          return (BCM_E_MEMORY);
    }

    /* Create the debug thread */
    thread_id = sal_thread_create("GDPLL CB thread", SAL_THREAD_STKSZ, 100/*thread_pri*/,
                    gdpll_cb_thread, INT_TO_PTR(unit));
    if (thread_id == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "GDPLL Error !! CB thread create failed\n")));
        return BCM_E_INTERNAL;
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Disable the ECC protection before Input-Array clearing */
        READ_NS_REGr(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, 0, 0, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, &regVal,
                          DISABLE_ECCf, 1);
#if defined(BCM_NANOSYNC_V1_SUPPORT)
        soc_reg_field_set(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, &regVal,
                          DISABLE_ECC_RDf, 1);
#endif
        WRITE_NS_REGr(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, 0, 0, regVal);

        /* Clear the Input Array */
        reg = BCM_GDPLL_IA_START;
        for (regCount = 0; regCount < (BCM_GDPLL_NUM_IA_ENTRIES*2); regCount++) {
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            WRITE_NS_MEM(unit, reg+regCount*4, 0);
#else
            soc_iproc_setreg(unit, reg+regCount*4, 0);
#endif
        }

        /* Clear the Output Array */
        reg = BCM_GDPLL_OA_START;
        for (regCount = 0; regCount < (BCM_GDPLL_NUM_OA_ENTRIES*2); regCount++) {
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            WRITE_NS_MEM(unit, reg+regCount*4, 0);
#else
            soc_iproc_setreg(unit, reg+regCount*4, 0);
#endif
        }

        /* Enable the ECC protection on Input-Array */
        READ_NS_REGr(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, 0, 0, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, &regVal,
                          DISABLE_ECCf, 0);
#if defined(BCM_NANOSYNC_V1_SUPPORT)
        soc_reg_field_set(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, &regVal,
                          DISABLE_ECC_RDf, 0);
#endif
        WRITE_NS_REGr(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, 0, 0, regVal);

        /* Clear the out status flags */
        for (count = 0; count < (BCM_GDPLL_NUM_CHANNELS/32); count++) {
            WRITE_NS_REGr(unit, nco_out_status0_r, 0, 4*count, 0xFFFFFFFF); /* 0:31  */
        }

#if 0
        /* Enable the Channel Output array status reporting */
        for (count = 0; count < (BCM_GDPLL_NUM_CHANNELS/32); count++) {
            WRITE_NS_REGr(unit, nco_out_enable0_r, 0, 4*count, 0xFFFFFFFF);
        }
#endif

        /* Enable NCO overload detection is global to all the channeld ?? */
        READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_COMMON_CONTROLr, 0, 0, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_COMMON_CONTROLr, &regVal, NCO_WERROR_DISABLEf, 1);
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_COMMON_CONTROLr, 0, 0, regVal);

        /* ECC error detection is set by default, ECC_CONTROL register */

        /* Enable GDPLL common interrupt status reporting */
        WRITE_NS_REGr(unit, NS_GDPLL_GDPLL_COMMON_INT_ENABLEr, 0, 0, 0xF);

        /* Enable the M7 core-interrupt SW status reporting */
        WRITE_NS_REGr(unit, m7_core_int_enable_r, 0, 0, 0x8/*0xF*/);

        /* There are no enabling control for TS_CAPTURE_OVRD */

        /* Set the watchdog on every channel
         * The watchdog status will get reflected in GDPLL_IA_IN_STATUS* and
         * corresponding error code in NS_GDPLL_IA_ERROR_CODE
         */
        for (count = 0; count < BCM_GDPLL_NUM_CHANNELS; count++) {
            /* Set enable and Interval field NS_GDPLL_IA_WDOG_CONFIG */
            READ_NS_REGr(unit, NS_GDPLL_IA_WDOG_CONFIGr, count, 0, &regVal);
            soc_reg_field_set(unit, NS_GDPLL_IA_WDOG_CONFIGr, &regVal,
                              ia_wdog_interval_f, 0xFFFFF);
            soc_reg_field_set(unit, NS_GDPLL_IA_WDOG_CONFIGr, &regVal,
                              ia_wdog_enable_f, 1);
            WRITE_NS_REGr(unit, NS_GDPLL_IA_WDOG_CONFIGr, count, 0, regVal);
        }

#if defined(BCM_MONTEREY_SUPPORT)
        if (soc_feature(unit, soc_feature_rsvd1_intf)) {
            /* Pull RSVD1 PLL out of reset */
            SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &regVal));
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &regVal, BRCM_RESERVED_TOP_1_1f, 1);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &regVal, BRCM_RESERVED_TOP_1_2f, 1);
            SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, regVal));

            /* Init the BCN counter */
            READ_NS_REGr(unit, BRCM_RESERVED_IPROC_12r, 0, 0, &regVal);
            soc_reg_field_set(unit, BRCM_RESERVED_IPROC_12r, &regVal, BRCM_RESERVED_IPROC_12_1f, 0x400);
            WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_12r, 0, 0, regVal);

            READ_NS_REGr(unit, BRCM_RESERVED_IPROC_13r, 0, 0, &regVal);
            soc_reg_field_set(unit, BRCM_RESERVED_IPROC_13r, &regVal, BRCM_RESERVED_IPROC_13_1f, 0);
            WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_13r, 0, 0, regVal);

            READ_NS_REGr(unit, BRCM_RESERVED_IPROC_11r, 0, 0, &regVal);
            soc_reg_field_set(unit, BRCM_RESERVED_IPROC_11r, &regVal, BRCM_RESERVED_IPROC_11_1f, 0x1);
            WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_11r, 0, 0, regVal);
        }
#endif

        /* Set capture_enable for all the TS GPIO's */
        for (count = 0; count < TS_GPIO_COUNT; count++) {
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            int idx = 0;
#endif
            READ_NS_REGr(unit, gpio_0_ctrl_r, 0, 24*count, &regVal);
            soc_reg_field_set(unit,  gpio_0_ctrl_r, &regVal,
                              CAPTURE_ENABLEf, 1);
            WRITE_NS_REGr(unit, gpio_0_ctrl_r, 0, 24*count, regVal);

            /* Set forward configuration for GPIO events */
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            SOC_IF_ERROR_RETURN(_gdpll_event_id_get(unit, bcmGdpllInputEventGPIO0 + count, 0, 0, &idx));

            READ_NS_REGr(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_MSBr, 0, (idx*8), &regVal);
            soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_MSBr, &regVal,
                              EVENT_IDf, idx);
            WRITE_NS_REGr(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_MSBr, 0, (idx*8), regVal);

            READ_NS_REGr(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, 0, ((idx*8)), &regVal);
            soc_reg_field_set(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, &regVal,
                              DESTf, BCM_GDPLL_EVENT_DEST_GDPLL);
            WRITE_NS_REGr(unit, NS_FWD_TIMESYNC_TS_EVENT_FWD_CFG_0_LSBr, 0, ((idx*8)), regVal);
#elif defined(BCM_MONTEREY_SUPPORT)
            READ_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, (131+count), 0, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal, EVENT_IDf, (195+count));
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal, DESTf, 0x1);
            WRITE_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, (131+count), 0, regVal);
#endif
        }

        if (!SOC_WARM_BOOT(unit)) {
            /* Set broadsync setting to reset state */
            WRITE_NS_REGr(unit, bs0_bs_tc_ctrl_r, 0, 0, 0);
            WRITE_NS_REGr(unit, bs1_bs_tc_ctrl_r, 0, 0, 0);
        }

        /* Check if the Offset is non-word aligned */
        reg = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);
        if (reg%4) {
            bsl_printf("GDPLL: WARN !! Offset field at offset:%d\n", reg);
        }

        /* Check if HoldoverFlag and NominalLoopFiletr are non-word aligned */
        reg = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.HoldoverFlag);
        if (reg%4) {
            bsl_printf("GDPLL: WARN !! HoldoverFlag field at offset:%d\n", reg);
        }

        reg = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_config.NominalLoopFilter);
        if (reg%4) {
            bsl_printf("GDPLL: WARN !! NominalLoopFilter field at offset:%d\n", reg);
        }

        /* Check the size of DPLL param */
        count = sizeof(m7DpllParam);
        if (count > DPLL_PARAM_SIZE) {
            bsl_printf("GDPLL: ERROR !! DPLL param size:%d is more than allocated:%d\n",
                       count, DPLL_PARAM_SIZE);
            return BCM_E_NONE;
        }

#if defined(BCM_MONTEREY_SUPPORT)
        /* Invalidate the NCO out configuration registers */
        if (SOC_IS_MONTEREY(unit)) {
            /* Monterey uArch spec defines hw_type:0 and hw_id:70 as invalid */
            for (count = 0; count < BCM_GDPLL_NUM_CHANNELS; count++) {
                READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, count, 0, &regVal);
                soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_IDf, 70);
                soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_TYPEf, 0);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, count, 0, regVal);
            }
        }
#endif

        /* Mark the locations as free untill 1588 channel is not configured */
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_1588_FREQ_LOCK_LOC, 0);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_1588_DPLL_OFFSET_LOC, 0);

        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0PLL_FREQ_LOCK_LOC, 0);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0PLL_DPLL_OFFSET_LOC, 0);

        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1PLL_FREQ_LOCK_LOC, 0);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1PLL_DPLL_OFFSET_LOC, 0);

        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_DPLL_OFFSET_LOC, 0);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_DPLL_OFFSET_LOC, 0);

        /* Comand M7 not to generate dump */
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_CHAN_DUMP_LOC +
                           SAL_OFFSETOF(dpll_dump_t, chan), 0xDEAD);

        /* Mask the GDPLL Debug IRQ interrupts and NS_TS IRQ interrupts */
        WRITE_NS_REGr(unit, debug_m7dg_int_enable_r, 0, 0, INTR_NS_DEBUG);

#if defined(BCM_MONTEREY_SUPPORT)
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BCN_UPDATE_LOC, 0xFF);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_UPDATE_LOC, 0xFF);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_UPDATE_LOC, 0xFF);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0_UPDATE_LOC, 0xFF);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1_UPDATE_LOC, 0xFF);
#endif
    }   /* !SOC_WARM_BOOT */

#if defined(BCM_CMICX_SUPPORT) && defined(BCM_NANOSYNC_V1_SUPPORT)
    /* Register Nanosync1 int */
    handle.num = SOC_NS_INTR1;
    handle.intr_fn = gdpll_intr;
    handle.intr_data = NULL;
    soc_cmic_intr_register(unit, &handle, 1);

    /* Register Debug-buffer int */
    handle.num = SOC_NS_DBG_INTR;
    handle.intr_fn = soc_nanosync_debug_intr;
    handle.intr_data = NULL;
    soc_cmic_intr_register(unit, &handle, 1);

    soc_cmic_intr_enable(unit, SOC_NS_INTR1);
    soc_cmic_intr_enable(unit, SOC_NS_DBG_INTR);

#else
    /* Enable both(GDPLL debug buffer and Error) interrupts */
    soc_cmicm_intr2_enable(unit, SOC_GDPLL_NS_INTR_POS | SOC_GDPLL_NS_INTR_DEBUG_POS);
#endif

exit:
    return (rv);
}

STATIC int
_gdpll_chan_alloc(int unit, bcm_gdpll_chan_t *pGdpllChan, int *pChan)
{
    int rv = BCM_E_NONE;
    uint32 idx = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if (pGdpllContext == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "_gdpll_chan_alloc: Error !! No context\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    switch (pGdpllChan->out_event) {
        case bcmGdpllOutputEventBSPLL0:
        case bcmGdpllOutputEventBSPLL1:
            idx = 0 + (pGdpllChan->out_event - bcmGdpllOutputEventBSPLL0);
            break;
        case bcmGdpllOutputEventPTPTOD:
            idx = 2;
            break;
        case bcmGdpllOutputEventNTPTOD:
            idx = 3;
            break;
        case bcmGdpllOutputEventGen1:
        case bcmGdpllOutputEventGen2:
        case bcmGdpllOutputEventGen3:
        case bcmGdpllOutputEventGen4:
        case bcmGdpllOutputEventGen5:
        case bcmGdpllOutputEventGen6:
        case bcmGdpllOutputEventGen7:
        case bcmGdpllOutputEventGen8:
        case bcmGdpllOutputEventGen9:
        case bcmGdpllOutputEventGen10:
        case bcmGdpllOutputEventGen11:
        case bcmGdpllOutputEventGen12:
            idx = 4 + (pGdpllChan->out_event - bcmGdpllOutputEventGen1);
            break;
        case bcmGdpllOutputEventTXPI:
            /* GDPLL port is already computed outside of this */
            idx = pGdpllChan->port;
            idx += 16;  /* Out TXPI starts from 16..271 */
            break;
        case bcmGdpllOutputEventTS0:
        case bcmGdpllOutputEventTS1:
            idx = 272 + (pGdpllChan->out_event - bcmGdpllOutputEventTS0);
            break;
        case bcmGdpllOutputEventPTPTODRef:
            idx = 274;
            break;

        default:
            rv = BCM_E_FULL;
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "_gdpll_chan_alloc: Error !! Invalid channel out_event:%d\n"), pGdpllChan->out_event));
            break;
    }

    if ((rv == BCM_E_NONE) && !(pGdpllContext->dpll_chan[idx].flag & BCM_GDPLL_CHAN_ALLOC)) {
        *pChan = idx;
    }

#elif defined(BCM_MONTEREY_SUPPORT)
    for (idx = 0; idx < BCM_GDPLL_NUM_CHANNELS; idx++) {
        if (!(pGdpllContext->dpll_chan[idx].flag & BCM_GDPLL_CHAN_ALLOC)) {
            *pChan = idx;
            goto exit;
        }
    }

    rv = BCM_E_FULL;
#endif

exit:
    return (rv);
}

#if 0
STATIC int
_gdpll_chan_free(int unit, int chan)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if ((pGdpllContext == NULL) ||
        (chan >= BCM_GDPLL_NUM_CHANNELS)){
        rv = BCM_E_PARAM;
        goto exit;
    }

    pGdpllContext->dpll_chan[chan].flag &= ~BCM_GDPLL_CHAN_ALLOC;

exit:
    return (rv);
}
#endif

STATIC int
_gdpll_event_config_set(int unit, bcm_gdpll_chan_input_t *pChanInput, uint32 eventId, uint32 *pIaAddr)
{
    int rv = BCM_E_NONE;
    gdpll_event_dest_cfg_t eventDest;

    /* Configure the event */
    eventDest.event_dest = pChanInput->event_dest;
    eventDest.ts_counter = pChanInput->ts_counter;
    eventDest.ppm_check_enable = pChanInput->ppm_check_enable;
    eventDest.event_dest_bitmap = pChanInput->event_dest_bitmap;

    if (pChanInput->input_event == bcmGdpllInputEventPORT) {
        if ((pChanInput->port_event_type == bcmGdpllPortEventRXCDR) ||
            (pChanInput->port_event_type == bcmGdpllPortEventTXPI)) {
            BCM_IF_ERROR_RETURN(_gdpll_event_divisor_port_set(unit,
                            pChanInput->port, pChanInput->port_event_type,
                            pChanInput->event_divisor, pChanInput->event_divisor_hi));
        }
        BCM_IF_ERROR_RETURN(_gdpll_event_dest_port_set(unit, pChanInput->port,
                            pChanInput->port_event_type,
                            &eventDest, eventId, pIaAddr));

#if defined(BCM_MONTEREY_SUPPORT)
        /* ROE event, disable the 52b conversion SWIPROC-591 */
        if (pChanInput->port_event_type == bcmGdpllPortEventROE)
            BCM_IF_ERROR_RETURN(_gdpll_event_roe_52b_set(unit,
                            pChanInput->port, 0));

    } else if(pChanInput->input_event == bcmGdpllInputEventR5) {
        
         *pIaAddr = BCM_GDPLL_IA_START_R5;

        /* Get the free Input array address */
        /*BCM_IF_ERROR_RETURN(_gdpll_input_array_alloc(unit,
            eventId, pIaAddr));*/
#endif
    } else {
        BCM_IF_ERROR_RETURN(_gdpll_event_divisor_misc_set(unit,
                            pChanInput->input_event,
                            pChanInput->event_divisor, pChanInput->event_divisor_hi));
        BCM_IF_ERROR_RETURN(_gdpll_event_dest_misc_set(unit,
                            pChanInput->input_event,
                            &eventDest, eventId, pIaAddr));
    }

    return (rv);
}

STATIC int
_gdpll_chan_priority_set(int unit, int chan, bcm_gdpll_chan_priority_t chan_prio)
{
    /* NS_GDPLL_IA_UPDATE_CONFIG */
    int rv = BCM_E_NONE;
    uint32 regVal = 0;

    if (chan >= BCM_GDPLL_NUM_CHANNELS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_priority_set: Error !! invalid chan\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, 0, (chan*4), &regVal);
    soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ia_update_speed_f, chan_prio);
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, 0, (chan*4), regVal);
#else
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, 0, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ia_update_speed_f, chan_prio);
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, 0, regVal);
#endif

exit:
    return (rv);
}

/****************************************************************************/
/*                      API functions implementation                        */
/****************************************************************************/

int
bcm_common_gdpll_input_event_poll(int unit, bcm_gdpll_input_event_t input_event,
    uint32 timeout)
{
    uint32 rval = 0;
    uint32 poll_interval_uSec = 5;  /* 1uSec polling interval */
    uint32 poll_period_uSec = timeout;
    int rv = BCM_E_NONE;
    soc_field_t field;

    switch (input_event) {
        case bcmGdpllInputEventBS0HB:
        case bcmGdpllInputEventBS1HB:
            /* Set BS Input-Mode and Enable-Capture */
            if (input_event == bcmGdpllInputEventBS0HB) {
                READ_NS_REGr(unit, bs0_bs_config_r, 0, 0, &rval);
                soc_reg_field_set(unit, bs0_bs_config_r, &rval, MODEf, 0);
                WRITE_NS_REGr(unit, bs0_bs_config_r, 0, 0, rval);

                READ_NS_REGr(unit, misc_clk_event_ctrl_r, 0, 0, &rval);
                rval = rval | 0x2;
                WRITE_NS_REGr(unit, misc_clk_event_ctrl_r, 0, 0, rval);

            } else {
                READ_NS_REGr(unit, bs1_bs_config_r, 0, 0, &rval);
                soc_reg_field_set(unit, bs1_bs_config_r, &rval, MODEf, 0);
                WRITE_NS_REGr(unit, bs1_bs_config_r, 0, 0, rval);

                READ_NS_REGr(unit, misc_clk_event_ctrl_r, 0, 0, &rval);
                rval = rval | 0x4;
                WRITE_NS_REGr(unit, misc_clk_event_ctrl_r, 0, 0, rval);
            }

            field = (input_event == bcmGdpllInputEventBS0HB) ? event_status_bs0_hb_f : event_status_bs1_hb_f;
            break;

        case bcmGdpllInputEventGPIO0:
        case bcmGdpllInputEventGPIO1:
        case bcmGdpllInputEventGPIO2:
        case bcmGdpllInputEventGPIO3:
        case bcmGdpllInputEventGPIO4:
        case bcmGdpllInputEventGPIO5:
            field = (input_event == bcmGdpllInputEventGPIO0) ? TS_GPIO_0f:
                (input_event == bcmGdpllInputEventGPIO1) ? TS_GPIO_1f:
                (input_event == bcmGdpllInputEventGPIO2) ? TS_GPIO_2f:
                (input_event == bcmGdpllInputEventGPIO3) ? TS_GPIO_3f:
                (input_event == bcmGdpllInputEventGPIO4) ? TS_GPIO_4f: TS_GPIO_5f;
            break;

        default:
            return BCM_E_UNAVAIL;
    }

    /* Clear if any flag is already set */
    soc_reg_field_set(unit, misc_event_status_r, &rval, field, 1);
    WRITE_NS_REGr(unit, misc_event_status_r, 0, 0, rval);

    /* Poll on the next event */
    rv = BCM_E_TIMEOUT;
    while (poll_period_uSec > 0) {
        READ_NS_REGr(unit, misc_event_status_r, 0, 0, &rval);
        if (soc_reg_field_get(unit, misc_event_status_r, rval, field)) {
            rv = BCM_E_NONE;
            break;
        }
        sal_usleep(poll_interval_uSec);
        poll_period_uSec -= poll_interval_uSec;
    }

    return rv;
}

/*
 * Function:
 *      bcm_common_gdpll_chan_enable
 * Purpose:
 *      Channel enable
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      chan - (IN) Channel Number
 *      enable   - (IN) Enable flag
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_common_gdpll_chan_enable(int unit, int chan, int enable)
{
    int rv = BCM_E_NONE;
    bcm_gdpll_input_event_t event_misc;
    bcm_gdpll_port_event_t port_event_type;
    bcm_port_t port;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    uint8 buffer[DPLL_FW_VERSION_SIZE];

    if((chan >= BCM_GDPLL_NUM_CHANNELS) ||
       (pGdpllContext == NULL)) {
        rv = BCM_E_PARAM;
        return rv;
    }

    /* Channel is not allocated and feedbak is configured */
    if (!(pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_CHAN_ALLOC) ||
        !(pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_EVENT_CONFIG_FB)) {
        rv = BCM_E_NOT_FOUND;
        return rv;
    }

    /* Check if the M7 is alive to service the channel events */
    rv = _gdpll_m7_status(unit, buffer);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "gdpll_chan_enable: M7-FW not running\n")));
        return rv;
    }

    sal_mutex_take(pGdpllContext->mutex, sal_mutex_FOREVER);

    /* Check if phaseCounterRef and event-ref is not configured */
    if (!pGdpllContext->dpll_chan[chan].phaseConterRef &&
        !(pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_EVENT_CONFIG_REF)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "gdpll_chan_enable: Error !! phase ref error\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    if (enable == 0) {
        /* For disable case, first disable the channel */
        _gdpll_chan_enable_set(unit, chan, enable);
    }

    /* enable=1, Enable the events in sequence */
        bsl_printf("GDPLL: Set enable for feedback event\n");
        /* set enable for feedback event */
        _gdpll_input_event_get(unit, chan, 1,
                    &event_misc, &port_event_type, &port);
        bsl_printf("GDPLL: chan_enable: FB event:%d port:%d event_type:%d\n", event_misc, port, port_event_type);
        if (event_misc == bcmGdpllInputEventPORT) {
            _gdpll_event_enable_port_set(unit, port, port_event_type, enable);
        } else if (event_misc == bcmGdpllInputEventR5) {
            
        } else {
            _gdpll_event_enable_misc_set(unit, event_misc, enable);
        }

        /* set enable for reference event */
        if (!pGdpllContext->dpll_chan[chan].phaseConterRef) {
            if (!(pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_EVENT_CONFIG_REF)) {
                /* Phase reference is not set, return error */
                LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                      "gdpll_chan_enable: Error !! phase ref is not set\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }

            bsl_printf("GDPLL: Set enable for reference event\n");
            _gdpll_input_event_get(unit, chan, 0,
                        &event_misc, &port_event_type, &port);
            bsl_printf("GDPLL: chan_enable: REF event:%d port:%d event_type:%d\n", event_misc, port, port_event_type);
            if (event_misc == bcmGdpllInputEventPORT) {
                _gdpll_event_enable_port_set(unit, port, port_event_type, enable);
            } else if (event_misc == bcmGdpllInputEventR5) {
                
            } else {
                _gdpll_event_enable_misc_set(unit, event_misc, enable);
            }
        }

    if (enable) {
        /*
         * For enable:1, first enable the IA capture and then enable the
         * GDPLL channel
         */
        /* Enable GDPLL input array capture */
        _gdpll_capture_enable_m7_set(unit, enable);

        /* Enable the GDPLL channel */
        _gdpll_chan_enable_set(unit, chan, enable);
    }

    /* For channel disable, clear the PAIR_AVAILABLE flag which is just
     * a debug hook
     */
    if (enable == 0) {
        /* Clear the status flags if any */
        WRITE_NS_REGr(unit, ia_ts_pair_available0_r, 0, 4*chan/32, 1<<(chan%32));
        WRITE_NS_REGr(unit, ia_in_status0_r, 0, 4*chan/32, 1<<(chan%32));
    }

    /* Set enable in channel context */
    if (rv == BCM_E_NONE) {
        pGdpllContext->dpll_chan[chan].enable = (uint8)enable;
    }

    /* Update the current state and trigger cb for notification */
    gdpll_state_change_chan_capture(unit, chan);
    sal_sem_give(pGdpllContext->cb_sem);

exit:
    sal_mutex_give(pGdpllContext->mutex);
    return (rv);
}

/*
 * Function:
 *      bcm_common_gdpll_chan_create
 * Purpose:
 *      Initialize time module
 * Parameters:
 *      unit  - (IN) StrataSwitch Unit #.
 *      flags - (IN) Flags for channel create
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_common_gdpll_chan_create(int unit, uint32 flags,
                          bcm_gdpll_chan_t *pGdpllChan, int *pChan)
{
    int rv = BCM_E_NONE;
    uint32 iaAddr_fb    = BCM_GDPLL_NUM_IA_ENTRIES;   /* Indicating free */
    uint32 iaAddr_ref   = BCM_GDPLL_NUM_IA_ENTRIES;   /* Indicating free */
    uint32 eventId_fb   = BCM_GDPLL_NUM_INPUT_EVENTS; /* Indicating invalid event */
    uint32 eventId_ref  = BCM_GDPLL_NUM_INPUT_EVENTS; /* Indicating invalid event */
    int phaseCounterRef = 0;
    int  gdpll_port;
    uint32 dpllAddr, addrOffset;

    int chan;
    int inUse = 0;
    dpll_param_t   dpllParam;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    bcm_gdpll_chan_t *pCurrentGdpllChan = NULL;

    if ((pChan == NULL) || (pGdpllChan == NULL) ||
        (pGdpllContext == NULL)) {
        return BCM_E_PARAM;
    }

    /* Lock the channel access */
    sal_mutex_take(pGdpllContext->mutex, sal_mutex_FOREVER);

    /* Get the free channel
     * If the channel is allocated earlier, then pChan carries the
     * allocated channel number, otherwise flag should need to be
     * with BCM_GDPLL_CHAN_ALLOC
     */
    if(flags & BCM_GDPLL_CHAN_ALLOC) {
        /* Alloc new channel */
        if(_gdpll_chan_alloc(unit, pGdpllChan, &chan) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        *pChan = chan;
    } else if((*pChan < BCM_GDPLL_NUM_CHANNELS) &&
              (pGdpllContext->dpll_chan[*pChan].flag & BCM_GDPLL_CHAN_ALLOC)){
        /* Channel is already allocated earlier and channel number
         * passed by the user
         */
        chan = *pChan;
    } else {
        /* Channel is not allocated earlier either */
        rv = BCM_E_PARAM;
        goto exit;
    }

    pGdpllContext->dpll_chan[chan].gdpll_chan_config = *pGdpllChan;
    pCurrentGdpllChan = &(pGdpllContext->dpll_chan[chan].gdpll_chan_config);
    /* Feedback event forward configuration */
    if(flags & BCM_GDPLL_EVENT_CONFIG_FB) {
        if (pCurrentGdpllChan->event_fb.input_event == bcmGdpllInputEventPORT) {
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            /* GDPLL port is already computed from the caller */
            gdpll_port = pCurrentGdpllChan->event_fb.port;
            bsl_printf("GDPLL: FB lPort:%d  gdpll_port:%d\n",
                pCurrentGdpllChan->event_fb.port, gdpll_port);
#elif defined(BCM_MONTEREY_SUPPORT)
            int use_rx_lanemap=0;
            if (pCurrentGdpllChan->event_fb.port_event_type == bcmGdpllPortEventTXPI) {
                if (IS_CL_PORT(unit, pCurrentGdpllChan->event_fb.port)) {
                    use_rx_lanemap = 0;
                } else if (IS_XL_PORT(unit, pCurrentGdpllChan->event_fb.port)) {
                    use_rx_lanemap = 1;
                }
                rv = _gdpll_portmap_get(unit, pCurrentGdpllChan->event_fb.port, &gdpll_port, use_rx_lanemap);
                if (rv != BCM_E_NONE) {
                    goto exit;
                }

            } else if ((pCurrentGdpllChan->event_fb.port_event_type == bcmGdpllPortEventTXSOF) ||
                       (pCurrentGdpllChan->event_fb.port_event_type == bcmGdpllPortEventRXSOF) ||
                       (pCurrentGdpllChan->event_fb.port_event_type == bcmGdpllPortEventROE)) {
                gdpll_port = SOC_INFO(unit).port_l2p_mapping[pCurrentGdpllChan->event_fb.port];
                gdpll_port -= 1;
            } else {
                rv = _gdpll_portmap_get(unit, pCurrentGdpllChan->event_fb.port, &gdpll_port, 1);
                if (rv != BCM_E_NONE) {
                    goto exit;
                }
            }
            bsl_printf("GDPLL: FB lPort:%d phys_port:%d gdpll_port:%d\n",
                pCurrentGdpllChan->event_fb.port, SOC_INFO(unit).port_l2p_mapping[pCurrentGdpllChan->event_fb.port], gdpll_port);
#endif

            pGdpllContext->dpll_chan[chan].lport_fb = pCurrentGdpllChan->event_fb.port;
            pGdpllContext->dpll_chan[chan].gdpllport_fb = gdpll_port;
            pCurrentGdpllChan->event_fb.port = gdpll_port;
        }

        if(_gdpll_input_eventId_get(unit,
                    &pCurrentGdpllChan->event_fb,
                    &eventId_fb) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }

        bsl_printf("GDPLL: Feedback eventId:%d\n", eventId_fb);
        bsl_printf("GDPLL: Feedback dest:%d\n", pCurrentGdpllChan->event_fb.event_dest_bitmap);
        /* Check if the event is already under use */
        if(_gdpll_input_eventId_inUse(unit,
                    eventId_fb, &inUse) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        if (inUse) {
            /* Event is under use by other channel */
            rv = BCM_E_PARAM;
            goto exit;
        }

        /* Configure the input event and get iaAddr_fb */
        if(_gdpll_event_config_set(unit, &(pCurrentGdpllChan->event_fb),
                    eventId_fb, &iaAddr_fb) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        pGdpllContext->ia_loc[eventId_fb] = iaAddr_fb;
    }

    /* Reference event forward configuration */
    if(flags & BCM_GDPLL_EVENT_CONFIG_REF) {
        if (pCurrentGdpllChan->event_ref.input_event == bcmGdpllInputEventPORT) {
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            /* GDPLL port is already computed from the caller */
            gdpll_port = pCurrentGdpllChan->event_ref.port;
            bsl_printf("GDPLL: REF lPort:%d gdpll_port:%d\n",
                pCurrentGdpllChan->event_ref.port, gdpll_port);
#elif defined(BCM_MONTEREY_SUPPORT)
            if (pCurrentGdpllChan->event_ref.port_event_type == bcmGdpllPortEventTXPI) {
                rv = _gdpll_portmap_get(unit, pCurrentGdpllChan->event_ref.port, &gdpll_port, 1);
                if (rv != BCM_E_NONE) {
                    goto exit;
                }
            } else if ((pCurrentGdpllChan->event_ref.port_event_type == bcmGdpllPortEventTXSOF) ||
                       (pCurrentGdpllChan->event_ref.port_event_type == bcmGdpllPortEventRXSOF) ||
                       (pCurrentGdpllChan->event_ref.port_event_type == bcmGdpllPortEventROE)) {
                gdpll_port = SOC_INFO(unit).port_l2p_mapping[pCurrentGdpllChan->event_ref.port];
                gdpll_port -= 1;
            } else {
                rv = _gdpll_portmap_get(unit, pCurrentGdpllChan->event_ref.port, &gdpll_port, 1);
                if (rv != BCM_E_NONE) {
                    goto exit;
                }
            }
            bsl_printf("GDPLL: REF lPort:%d phys_port:%d gdpll_port:%d\n",
                pCurrentGdpllChan->event_ref.port, SOC_INFO(unit).port_l2p_mapping[pCurrentGdpllChan->event_ref.port], gdpll_port);
#endif

            pGdpllContext->dpll_chan[chan].lport_ref = pCurrentGdpllChan->event_ref.port;
            pGdpllContext->dpll_chan[chan].gdpllport_ref = gdpll_port;
            pCurrentGdpllChan->event_ref.port = gdpll_port;
        }

        if (_gdpll_input_eventId_get(unit,
                    &pCurrentGdpllChan->event_ref,
                    &eventId_ref) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }

        bsl_printf("GDPLL: Reference eventId:%d\n", eventId_ref);
        bsl_printf("GDPLL: Reference dest:%d\n", pCurrentGdpllChan->event_ref.event_dest_bitmap);
        /* Check if the event is already under use */
        if(_gdpll_input_eventId_inUse(unit,
                    eventId_ref, &inUse) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }

        if (!inUse) {
            /* Configure the Reference event and get iaAddr_ref */
            if(_gdpll_event_config_set(unit, &(pCurrentGdpllChan->event_ref), eventId_ref, &iaAddr_ref) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto exit;
            }
            pGdpllContext->ia_loc[eventId_ref] = iaAddr_ref;
        }
    }

    /* Channel priority set */
    if(flags & BCM_GDPLL_CHAN_SET_PRIORITY) {
        if(_gdpll_chan_priority_set(unit, chan,
                                pCurrentGdpllChan->chan_prio) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
    }

    /* Channel DPLL configuration */
    if(flags & BCM_GDPLL_CONF_DPLL) {
        dpllParam.dpll_config = pCurrentGdpllChan->chan_dpll_config;
        dpllParam.dpll_state  = pCurrentGdpllChan->chan_dpll_state;
        if(_gdpll_chan_config_dpll_set(unit, chan,
                    &dpllParam) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        if (pCurrentGdpllChan->chan_dpll_config.phase_counter_ref) {
            phaseCounterRef = 1;
        }
    }

    /* Channel output configurations */
    if(flags & BCM_GDPLL_CHAN_OUTPUT_CONF) {
        if (pCurrentGdpllChan->out_event == bcmGdpllOutputEventTXPI) {
#if defined(BCM_NANOSYNC_V1_SUPPORT)
            /* port is already computed by the caller of this function */
            gdpll_port = pCurrentGdpllChan->port;
#elif defined(BCM_MONTEREY_SUPPORT)
            /* Do not consider lane-map settings for NCO UPDATE hw_id */
            gdpll_port = SOC_INFO(unit).port_l2p_mapping[pCurrentGdpllChan->port] - 1;
#endif
            pCurrentGdpllChan->port = gdpll_port;
            bsl_printf("\n_gdpll_chan_out_txpi_set : nco_hwid[%d]\n", pCurrentGdpllChan->port);
            if(_gdpll_chan_out_txpi_set(unit, chan, pCurrentGdpllChan->port) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto exit;
            }
        } else {
            if(_gdpll_chan_out_misc_set(unit, chan, pCurrentGdpllChan->out_event) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto exit;
            }
        }
    }

    /* Set the channel soft state data structures */
    if((flags & BCM_GDPLL_EVENT_CONFIG_REF) &&
       (pGdpllContext->dpll_chan[chan].phaseConterRef == 0)) {
        pGdpllContext->dpll_chan[chan].eventId_ref = eventId_ref;
        pGdpllContext->dpll_chan[chan].iA_ref = eventId_ref;
    }

    if(flags & BCM_GDPLL_EVENT_CONFIG_FB) {
        pGdpllContext->dpll_chan[chan].eventId_fb = eventId_fb;
        pGdpllContext->dpll_chan[chan].iA_fb = eventId_fb;
    }

    /* Set the channel soft state data structures */
    if (phaseCounterRef) {

        /* If the ref event is not in use by any channels, then free */
        if (pGdpllContext->dpll_chan[chan].eventId_ref < BCM_GDPLL_NUM_INPUT_EVENTS) {
            if(_gdpll_input_eventId_inUse(unit,
               pGdpllContext->dpll_chan[chan].eventId_ref, &inUse) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto exit;
            }
            if (!inUse) {
                pGdpllContext->ia_loc[pGdpllContext->dpll_chan[chan].eventId_ref] = BCM_GDPLL_NUM_IA_ENTRIES;
            } else {
                bsl_printf("GDPLL: This channel has phaseCounterRef but the ref settings are used by other channel\n");
            }
        }

        pGdpllContext->dpll_chan[chan].phaseConterRef = 1;
        pGdpllContext->dpll_chan[chan].eventId_ref = BCM_GDPLL_NUM_INPUT_EVENTS; /* Indicating invalid event */;
    }

    /* Now update the flags for the channel */
    pGdpllContext->dpll_chan[chan].flag |= flags;

    /* Upadet the channel with ref and fb event */
    _gdpll_chan_update_set(unit, chan);

    /* Program the lockIndicator and offset of 1588 counter to the location under for R5 to consume for PTP */
    if ((pCurrentGdpllChan->out_event == bcmGdpllOutputEventTS0) ||
        (pCurrentGdpllChan->out_event == bcmGdpllOutputEventTS1)) {

        dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);

        /* Set the LockIndicator location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.LockIndicator);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_1588_FREQ_LOCK_LOC, dpllAddr+addrOffset);
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                        " Writing FreqLockInd_addr:0x%08x@0x%08x 1588 TS0/TS1 out event \n"),
                        (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_1588_FREQ_LOCK_LOC)));

        /* Set the Offset location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_1588_DPLL_OFFSET_LOC, dpllAddr+addrOffset);
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                        " Writing DpllOffset_addr:0x%08x@0x%08x for 1588 TS0/TS1 out event \n"),
                        (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_1588_DPLL_OFFSET_LOC)));
        if (pCurrentGdpllChan->event_fb.input_event == bcmGdpllInputEventBS0HB) {
            m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0_UPDATE_LOC, chan);
        }
    } else if ( (pCurrentGdpllChan->event_fb.input_event == bcmGdpllInputEventBS0PLL) &&
                (pCurrentGdpllChan->out_event == bcmGdpllOutputEventBSPLL0)) {

        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0_UPDATE_LOC, chan);
        dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);

        /* Set the LockIndicator location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.LockIndicator);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0PLL_FREQ_LOCK_LOC, dpllAddr+addrOffset);
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                        " Writing FreqLockInd_addr:0x%08x@0x%08x BS0PLL out event \n"),
                        (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_BS0PLL_FREQ_LOCK_LOC)));

        /* Set the Offset location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0PLL_DPLL_OFFSET_LOC, dpllAddr+addrOffset);
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                        " Writing DpllOffset_addr:0x%08x@0x%08x for BS0PLL out event \n"),
                        (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_BS0PLL_DPLL_OFFSET_LOC)));
    } else if ( (pCurrentGdpllChan->event_fb.input_event == bcmGdpllInputEventBS1PLL) &&
                (pCurrentGdpllChan->out_event == bcmGdpllOutputEventBSPLL1)) {

        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1_UPDATE_LOC, chan);

        dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);

        /* Set the LockIndicator location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.LockIndicator);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1PLL_FREQ_LOCK_LOC, dpllAddr+addrOffset);
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                        " Writing FreqLockInd_addr:0x%08x@0x%08x BS1PLL out event \n"),
                        (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_BS1PLL_FREQ_LOCK_LOC)));

        /* Set the Offset location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1PLL_DPLL_OFFSET_LOC, dpllAddr+addrOffset);
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                        " Writing DpllOffset_addr:0x%08x@0x%08x for BS0PLL out event \n"),
                        (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_BS1PLL_DPLL_OFFSET_LOC)));
    }
    pGdpllContext->dpll_chan[chan].gdpll_chan_config = *pGdpllChan;

exit:
    sal_mutex_give(pGdpllContext->mutex);
    return (rv);
}

/*
 * Function:
 *      bcm_common_gdpll_chan_destroy
 * Purpose:
 *      Destroys the GDPLL channel
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      chan - (IN) Channel number
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_common_gdpll_chan_destroy(int unit, int chan)
{
    int rv = BCM_E_NONE;
    int inUse = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
#if defined(BCM_MONTEREY_SUPPORT)
    uint32 enableReg = 0;
    uint32 disable_chan_nco = 0;
#endif

    if((chan >= BCM_GDPLL_NUM_CHANNELS) ||
       (pGdpllContext == NULL)) {
        return BCM_E_PARAM;
    }

    sal_mutex_take(pGdpllContext->mutex, sal_mutex_FOREVER);

    /* If channel is not created, then return BCM_E_NOT_FOUND */
    if (pGdpllContext->dpll_chan[chan].flag == 0) {
        return BCM_E_NOT_FOUND;
    }

    /* Disable the channel */
    bcm_common_gdpll_chan_enable(unit, chan, 0);
    pGdpllContext->dpll_chan[chan].flag = 0;

    /* If the ref event is not in use by any channels, then free */
    if (pGdpllContext->dpll_chan[chan].eventId_ref < BCM_GDPLL_NUM_INPUT_EVENTS) {
        if(_gdpll_input_eventId_inUse(unit,
           pGdpllContext->dpll_chan[chan].eventId_ref, &inUse) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        if (!inUse) {
            pGdpllContext->ia_loc[pGdpllContext->dpll_chan[chan].eventId_ref] = BCM_GDPLL_NUM_IA_ENTRIES;
        } else {
            bsl_printf("GDPLL: Error !! Ref event is in use by other channel \n");
        }
    }

    /* If the fb event is not in use by any channels, then free */
    if (pGdpllContext->dpll_chan[chan].eventId_fb < BCM_GDPLL_NUM_INPUT_EVENTS) {
        if(_gdpll_input_eventId_inUse(unit,
            pGdpllContext->dpll_chan[chan].eventId_fb, &inUse) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        if (!inUse) {
            pGdpllContext->ia_loc[pGdpllContext->dpll_chan[chan].eventId_fb] = BCM_GDPLL_NUM_IA_ENTRIES;
        } else {
            bsl_printf("GDPLL: Error !! FB event is IN use by other channel\n");
        }
    }

    _gdpll_input_array_free(unit,
        pGdpllContext->dpll_chan[chan].eventId_ref);
    pGdpllContext->dpll_chan[chan].eventId_ref = BCM_GDPLL_NUM_INPUT_EVENTS;

    _gdpll_input_array_free(unit,
        pGdpllContext->dpll_chan[chan].eventId_fb);
    pGdpllContext->dpll_chan[chan].eventId_fb = BCM_GDPLL_NUM_INPUT_EVENTS;
    pGdpllContext->dpll_chan[chan].phaseConterRef = 0; /*BCM_GDPLL_NUM_INPUT_EVENTS; */

#if defined(BCM_MONTEREY_SUPPORT)
    /*
     * If the channel is for BCN output, then clear location to indicate R5,
     * not to process the NCO and update BCN counter
     */
    if(chan == m7_mem_read(unit, BCM_M7_DTCM_BASE + DPLL_BCN_UPDATE_LOC)) {
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BCN_UPDATE_LOC, 0xFF);
        disable_chan_nco = 1;
    } else if (chan == m7_mem_read(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_UPDATE_LOC)) {
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_UPDATE_LOC, 0xFF);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_DPLL_OFFSET_LOC, 0x0);
        disable_chan_nco = 1;
    } else if (chan == m7_mem_read(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_UPDATE_LOC)) {
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_UPDATE_LOC, 0xFF);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_DPLL_OFFSET_LOC, 0x0);
        disable_chan_nco = 1;
    }
    if (chan == m7_mem_read(unit, BCM_M7_DTCM_BASE + DPLL_BS0_UPDATE_LOC)) {
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0_UPDATE_LOC, 0xFF);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0PLL_DPLL_OFFSET_LOC, 0x0);
    } else if (chan == m7_mem_read(unit, BCM_M7_DTCM_BASE + DPLL_BS1_UPDATE_LOC)){
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1_UPDATE_LOC, 0xFF);
        m7_mem_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1PLL_DPLL_OFFSET_LOC, 0x0);
    }

    if (disable_chan_nco) {
        /* Disable the Channel Output array status reporting over interrupt */
        READ_NS_REGr(unit, nco_out_enable0_r, 0, 4*chan/32, &enableReg);
        enableReg &= ~(1<<(chan%32));
        soc_reg_field_set(unit, nco_out_enable0_r, &enableReg, INTENf, enableReg);
        WRITE_NS_REGr(unit, nco_out_enable0_r, 0, 4*chan/32, enableReg);
    }
#endif

exit:
    sal_mutex_give(pGdpllContext->mutex);
    return (rv);
}

/*
 * Function:
 *      bcm_common_gdpll_chan_state_get
 * Purpose:
 *      Get Channel status
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      chan  - (IN) Channel Number
 *      dpllState - (OUT) DPLL State
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_common_gdpll_chan_state_get(int unit, int chan, uint32 *dpllState)
{
    int rv = BCM_E_NONE;
    dpll_param_t dpllParam;
    uint32 regVal = 0;
    uint8 chan_enable = 0;
    uint8 chan_err = 0;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 addrOffset;
    uint8 lError = 0;
    uint8 lock;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if ((chan >= BCM_GDPLL_NUM_CHANNELS) || (chan < 0) || (dpllState == NULL) ||
        (!pGdpllContext)) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Check the channel enable status */
#if defined(BCM_NANOSYNC_V1_SUPPORT)
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, 0, (chan*4), &regVal);
#else
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, 0, &regVal);
#endif
    chan_enable = soc_reg_field_get(unit, NS_GDPLL_IA_UPDATE_CONFIGr, regVal, ia_update_enable_f);
    if (!chan_enable) {
        /* Channel is not enabled */
        *dpllState = 0;
        goto exit;
    }

    /* If state change callback is already registered, then get the state from SW state
     * maintained per async notification
     */
    if (pGdpllContext->gdpll_cb[bcmGdpllCbChanState]) {
        if (pGdpllContext->dpll_chan[chan].prev_state.state & BCM_GDPLL_CHAN_STATE_ERROR) {
            /* Error can be either a DPLL error or the HW error */
            if (pGdpllContext->dpll_chan[chan].prev_state.error_code != bcmGdpllChanErrorDpllLError) {
                /* Its a HW error */
                chan_err = 1;
            } else {
                /* Its a DPLL large error */
                lError = 1;
            }
        }

        /* Get the DPLL lock */
        lock = (pGdpllContext->dpll_chan[chan].prev_state.state & BCM_GDPLL_CHAN_STATE_LOCK) ? 1:0;
        *dpllState = lock | chan_enable << 8 | lError << 30 | chan_err << 31;
        goto exit;
    }

    /* Following state retrieval is when the callback is not registered */
    READ_NS_REGr(unit, ia_in_status0_r, 0, 4*chan/32, &regVal);
    chan_err = (regVal & (1<<(chan%32))) ? 1:0;

    if(_gdpll_chan_config_dpll_get(unit, chan,
                &dpllParam) != BCM_E_NONE) {
        rv = BCM_E_INTERNAL;
        goto exit;
    }

    /* Clear the M7-DPLL indications */
    if (chan_err) {
        dpllParam.dpll_state.lockIndicator = 0;
        dpllParam.dpll_state.lossOfLockIndicator = 0;
        dpllParam.dpll_state.largeErrorIndicator = 0;

        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.LockIndicator);
        m7_mem_write(unit, dpllAddr+addrOffset, 0);

        /* Clear the channel error, if any */
        WRITE_NS_REGr(unit, ia_in_status0_r, 0, 4*chan/32, 1<<(chan%32));
    }
    *dpllState = dpllParam.dpll_state.lockIndicator |
                 (dpllParam.dpll_state.lossOfLockIndicator & 0x7F) << 1 |
                 chan_enable << 8 | chan_err << 31 |
                 (dpllParam.dpll_state.largeErrorIndicator & 0x1) << 30;
exit:
    return (rv);
}

/*
 * Function:
 *      bcm_common_gdpll_chan_debug_enable
 * Purpose:
 *      Enable Channel debugging
 * Parameters:
 *      unit     - (IN) StrataSwitch Unit #.
 *      chan - (IN) Channel Number
 *      debug_mode  - (IN) Debug mode
 *      enable      - (IN) Enable flag
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_common_gdpll_chan_debug_enable(int unit, int chan,
                    bcm_gdpll_debug_mode_t debug_mode, int enable)
{
    int rv = BCM_E_NONE;

    if(chan >= BCM_GDPLL_NUM_CHANNELS) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    BCM_IF_ERROR_RETURN(_gdpll_chan_debug_mode_set(unit, chan,
                        debug_mode));
exit:
    return (rv);
}

/*
 * Function:
 *      bcm_common_gdpll_debug_cb_register
 * Purpose:
 *      Register Channel debug callback
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      user_data - (IN) Users private data
 *      cb      - (IN) Callback handler
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_common_gdpll_debug_cb_register(int unit, void *user_data,
                            bcm_gdpll_debug_cb cb)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if(pGdpllContext == NULL) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    pGdpllContext->debug_cb = cb;
    pGdpllContext->pUserData = user_data;

exit:
    return (rv);
}

/*
 * Function:
 *      bcm_common_gdpll_cb_register
 * Purpose:
 *      Register Generic GDPLL callback
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      cb_type - (IN) Callback type
 *      cb      - (IN) Callback handler
 *      user_data - (IN) Users private data
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int bcm_common_gdpll_cb_register(int unit, bcm_gdpll_cb_reg_t cb_type,
                          bcm_gdpll_cb cb, void *user_data)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    uint32 regVal = 0;
    uint32 intEnable = 0;
#if defined(BCM_MONTEREY_SUPPORT)
    int count;
#endif

    /* Param Validation */
    if (cb_type >= bcmGdpllCbMax) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Check if cb is already registered */
    if ((pGdpllContext == NULL) ||
       (cb && (pGdpllContext->gdpll_cb[cb_type] != NULL))) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Lock as we update the interrupt enable register */
    sal_mutex_take(pGdpllContext->mutex, sal_mutex_FOREVER);

    pGdpllContext->gdpll_cb[cb_type] = cb;
    pGdpllContext->pGdpll_cb_data[cb_type] = user_data;

    /* Read the main int enable */
    READ_NS_REGr(unit, ts_int_enable0_r, 0, 0, &intEnable);

    /* Callback specific controls */
    switch (cb_type) {
#if defined(BCM_MONTEREY_SUPPORT)
    case bcmGdpllCbRsvd1:
        /* Enable the RSVD1 message arrival status leaf-level interrupt(both RX and ERR interrupts)
         * in MESSAGE_INT_ENABLE(BRCM_RESERVED_IPROC_10)
         * Enable RSVD1 interrupt on Nanosync interrupt line in NS_TS_INT_ENABLE(bit-22)
         */
        if (soc_feature(unit, soc_feature_rsvd1_intf)) {

            /* Clear the earlier status if any */
            WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_9r, 0, 0, 0x3);

            /* Enable the RSVD1 leaf-level interupt, both RX and ERR */
            READ_NS_REGr(unit, BRCM_RESERVED_IPROC_10r, 0, 0, &regVal);
            regVal |= 0x3;
            WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_10r, 0, 0, regVal);

            /* Enable RSVD1 status interrupt on Nanosync intr line */
            intEnable |= INTR_RSVD1_MESSAGE_RX_STATUS;
        }
        break;
#endif

    case bcmGdpllCbChanState:
        /* Enable the M7 core-interrupt ERROR status reporting */
        READ_NS_REGr(unit, m7_core_int_enable_r, 0, 0, &regVal);
        regVal |= 0x8;
        WRITE_NS_REGr(unit, m7_core_int_enable_r, 0, 0, regVal);

        /* Capture the DPLL state and post sema for cb before enabling interrupts */
        gdpll_state_change_capture(unit);
        sal_sem_give(pGdpllContext->cb_sem);

        /* Enable M7 Core and IA error interrupts */
        soc_reg_field_set(unit, ts_int_status_r, &intEnable, int_m7_int_status_f, 1);
#if defined(BCM_NANOSYNC_V1_SUPPORT)
        soc_reg_field_set(unit, ts_int_status_r, &intEnable, GDPLL_IA_STATUSf, 1);
#else
        for (count = 0; count<(BCM_GDPLL_NUM_CHANNELS/32); count++) {
            soc_reg_field_set(unit, ts_int_status_r, &intEnable, gdpll_ia_status[count], 1);
        }
#endif
        break;

    case bcmGdpllCbMax:
    default:
        break;
    }

    /* Set the int enable bits */
    WRITE_NS_REGr(unit, ts_int_enable0_r, 0, 0, intEnable);
    sal_mutex_give(pGdpllContext->mutex);

exit:
    return (rv);
}

/*
 * Function:
 *      bcm_common_gdpll_cb_unregister
 * Purpose:
 *      Un-register Generic GDPLL callback
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      cb_type - (IN) Callback type
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int bcm_common_gdpll_cb_unregister(int unit, bcm_gdpll_cb_reg_t cb_type)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
#if defined(BCM_MONTEREY_SUPPORT)
    int count;
#endif
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    /* Param Validation */
    if (cb_type >= bcmGdpllCbMax) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    if(pGdpllContext == NULL) {
        rv = BCM_E_PARAM;
        goto exit;
    }
    sal_mutex_take(pGdpllContext->mutex, sal_mutex_FOREVER);

    /* Callback specific controls */
    switch (cb_type) {
#if defined(BCM_MONTEREY_SUPPORT)
    case bcmGdpllCbRsvd1:
        if (soc_feature(unit, soc_feature_rsvd1_intf)) {
            /* Disable RSVD1 status interrupt on Nanosync intr line */
            READ_NS_REGr(unit, ts_int_enable0_r, 0, 0, &regVal);
            regVal &= ~INTR_RSVD1_MESSAGE_RX_STATUS;
            WRITE_NS_REGr(unit, ts_int_enable0_r, 0, 0, regVal);

            /* Disable the RSVD1 leaf-level interupt, both RX and ERR */
            regVal = 0;
            WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_10r, 0, 0, regVal);
        }
        break;
#endif

    case bcmGdpllCbChanState:
        /* Disable the M7 core ERROR interrupt */

        /* Enable the M7 core-interrupt ERROR status reporting */
        READ_NS_REGr(unit, m7_core_int_enable_r, 0, 0, &regVal);
        regVal &= ~0x8;
        WRITE_NS_REGr(unit, m7_core_int_enable_r, 0, 0, regVal);

        /* Disable M7 Core and IA error interrupts */
        READ_NS_REGr(unit, ts_int_enable0_r, 0, 0, &regVal);
        soc_reg_field_set(unit, ts_int_status_r, &regVal, int_m7_int_status_f, 0);
#if defined(BCM_NANOSYNC_V1_SUPPORT)
        soc_reg_field_set(unit, ts_int_status_r, &regVal, GDPLL_IA_STATUSf, 0);
#else
        for (count = 0; count<(BCM_GDPLL_NUM_CHANNELS/32); count++) {
            soc_reg_field_set(unit, ts_int_status_r, &regVal, gdpll_ia_status[count], 0);
        }
#endif
        WRITE_NS_REGr(unit, ts_int_enable0_r, 0, 0, regVal);

        break;

    case bcmGdpllCbMax:
    default:
        break;
    }

    pGdpllContext->gdpll_cb[cb_type] = NULL;
    pGdpllContext->pGdpll_cb_data[cb_type] = NULL;
    sal_mutex_give(pGdpllContext->mutex);

exit:
    return (rv);
}

/*
 * Function:
 *      bcm_common_gdpll_flush
 * Purpose:
 *      Flush the GDPLL pipeline
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_common_gdpll_flush (int unit)
{
    int rv = BCM_E_NONE;
    int chan = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if(pGdpllContext == NULL) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    for(chan = 0; chan < BCM_GDPLL_NUM_CHANNELS; chan++) {
        bcm_common_gdpll_chan_destroy(unit, chan);
    }

    /* Disable the GDPLL capture */
    _gdpll_capture_enable_m7_set(unit, 0);

    pGdpllContext->debug_cb = NULL;
    pGdpllContext->pUserData = NULL;

    _gdpll_flush(unit);

exit:
    return (rv);
}

/*
 * Function:
 *      bcm_common_gdpll_debug
 * Purpose:
 *      Enable the GDPLL global debug feature
 *      This will set the flag for M7 to start pushing the debug
 *      content to debug buffers
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_common_gdpll_debug (int unit, int enable)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    uint32 dpllDebugFlag = BCM_M7_DTCM_BASE + DPLL_DEBUG_ENABLE_OFFSET;

    if (enable) {
        /* Check if its already enabled */
        if(m7_mem_read (unit, dpllDebugFlag)) {
            /* Already enabled. Return error */
            rv = BCM_E_PARAM;
            goto exit;
        }

        /* Reset the debug pointers and re-enable */
        READ_NS_REGr(unit, debug_m7dg_enable_r, 0, 0, &regVal);
        soc_reg_field_set(unit, debug_m7dg_enable_r, &regVal, ENABLEf, 0);
        WRITE_NS_REGr(unit, debug_m7dg_enable_r, 0, 0, regVal);

        soc_reg_field_set(unit, debug_m7dg_enable_r, &regVal, ENABLEf, 1);
        WRITE_NS_REGr(unit, debug_m7dg_enable_r, 0, 0, regVal);
    }

    m7_mem_write(unit, dpllDebugFlag, enable);

exit:
    return (rv);
}

int
_gdpll_debug_get (int unit, int *pEnable)
{
    int rv = BCM_E_NONE;
    uint32 dpllDebugFlag = BCM_M7_DTCM_BASE + DPLL_DEBUG_ENABLE_OFFSET;

    if (!pEnable) {
        rv = BCM_E_PARAM;
        goto exit;
    }
    *pEnable = m7_mem_read (unit, dpllDebugFlag);

exit:
    return rv;
}

/* Retrieve debug dump on a given channel
 * unit: (IN)
 * pChanDump: (IN/OUT)
 */
int
_gdpll_debug_dump (int unit, dpll_dump_t *pChanDump)
{
    int rv = BCM_E_NONE;
    uint32 *pData;
    uint32 chan;
    uint32 poll_interval_uSec = 100;  /* 1uSec polling interval */
    int count;
    uint32 dumpAddr = BCM_M7_DTCM_BASE + DPLL_CHAN_DUMP_LOC;
    uint32 addr;

    /* polling interval is set to 4-secs, assuming that 1G channel
     * rate at 1Hz */
    uint32 poll_period_interval = 4*1000000; /* 4*1000000uSec=4sec */

    if (!pChanDump) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    chan = m7_mem_read (unit, dumpAddr + SAL_OFFSETOF(dpll_dump_t, chan));
    if (chan != 0xDEAD) {
        bsl_printf("ERROR: M7FW Busy 0x%x\n", chan);

        /* Comand M7 not to generate dump */
        m7_mem_write(unit, dumpAddr + SAL_OFFSETOF(dpll_dump_t, chan),
                       0xDEAD);
        rv = BCM_E_BUSY;
        goto exit;
    }

    /* Write the channel number and poll for M7 to respond */
    rv = BCM_E_TIMEOUT;
    chan = pChanDump->chan;
    m7_mem_write(unit, dumpAddr + SAL_OFFSETOF(dpll_dump_t, chan), chan);
    while (poll_period_interval > 0) {
        /* Check response from M7 */
        if (0xDEAD == m7_mem_read (unit, dumpAddr + SAL_OFFSETOF(dpll_dump_t, chan))) {
            /* Debug dump is captured, read the DTCM locations for DPLL config */
            addr = dumpAddr;
            pData = (uint32 *)pChanDump;
            for (count = 0; count < sizeof(dpll_dump_t);) {
                *pData = swap32(m7_mem_read(unit, addr));
                pData++;
                count += sizeof(uint32);
                addr += sizeof(uint32);
            }
            pChanDump->chan = chan;

            /* Compensate the endianness chage for 64b */
            count = 0;
            while (count < 2) {
                uint32 temp;

                temp = swap64_h(pChanDump->debug_dump[count].feedback);
                u64_L(pChanDump->debug_dump[count].feedback) = swap64_l(pChanDump->debug_dump[count].feedback);
                u64_H(pChanDump->debug_dump[count].feedback) = temp;

                temp = swap64_h(pChanDump->debug_dump[count].reference);
                u64_L(pChanDump->debug_dump[count].reference) = swap64_l(pChanDump->debug_dump[count].reference);
                u64_H(pChanDump->debug_dump[count].reference) = temp;

                temp = swap64_h(pChanDump->debug_dump[count].perror);
                u64_L(pChanDump->debug_dump[count].perror) = swap64_l(pChanDump->debug_dump[count].perror);
                u64_H(pChanDump->debug_dump[count].perror) = temp;

                count++;
            }

            rv = BCM_E_NONE;
            break;
        }
        sal_usleep(poll_interval_uSec);
        poll_period_interval -= poll_interval_uSec;
    }

    /* Comand M7 not to generate dump */
    m7_mem_write(unit, dumpAddr + SAL_OFFSETOF(dpll_dump_t, chan),
                       0xDEAD);
exit:
    return rv;
}

int
_bcm_common_gdpll_tdpllinstance_bind(int unit, int gdpllChan, uint32 out_clock_offset)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
/*    uint32  eventId_ref;
    bcm_gdpll_chan_input_t inputEvent;*/
    uint32 iaAddr;
    uint32 dest_hw_id = 0;
    uint32 dest_hw_type = 0;

    if ((gdpllChan < 0 || gdpllChan >= BCM_GDPLL_NUM_CHANNELS)) {
        bsl_printf("_bcm_common_gdpll_tdpllinstance_bind: Error!! parameters\n");
        rv = BCM_E_PARAM;
        goto exit;
    }
#if 0

    if (BCM_GDPLL_NUM_IA_ENTRIES == gdpll_tdpll_state.tdpll_instace[tdpll_instance_num]) {
        /* Alloc the IA entry */
        inputEvent.input_event = bcmGdpllInputEventR5;
        if(_gdpll_input_eventId_get(unit,
                    &inputEvent,
                    &eventId_ref) != BCM_E_NONE) {
            bsl_printf("_bcm_common_gdpll_tdpllinstance_bind: Error!! in input_eventId_get \n");
            rv = BCM_E_INTERNAL;
            goto exit;
        }

        rv = _gdpll_input_array_alloc(unit, eventId_ref, &iaAddr);
        if (rv != BCM_E_NONE) {
            bsl_printf("_bcm_common_gdpll_tdpllinstance_bind: Error!! input_array_alloc failed. %s\n", bcm_errmsg(rv));
            goto exit;
        }

        gdpll_tdpll_state.tdpll_instace[tdpll_instance_num] = iaAddr;
    }

    iaAddr = gdpll_tdpll_state.tdpll_instace[tdpll_instance_num];
    bsl_printf("_bcm_common_gdpll_tdpllinstance_bind Channel: %d ref:%d/%d\n", gdpllChan, gdpll_tdpll_state.tdpll_instace[tdpll_instance_num], gdpll_tdpll_state.tdpll_instace[tdpll_instance_num]+1);
#else
    iaAddr = 214 + out_clock_offset;
#endif

    
    /* Channel update with reference indix */
#if defined(BCM_NANOSYNC_V1_SUPPORT)
    READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, 0, (gdpllChan*4), &regVal);
#else
    READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, gdpllChan, 0, &regVal);
#endif
    dest_hw_id =  soc_reg_field_get(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, regVal, DEST_HW_IDf);
    dest_hw_type = soc_reg_field_get(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, regVal, DEST_HW_TYPEf);

    /* If the channel is 1588, set feedback or set reference */
#if defined(BCM_NANOSYNC_V1_SUPPORT)
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, 0, (gdpllChan*4), &regVal);
#else
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, gdpllChan, 0, &regVal);
#endif
    if (((dest_hw_id == 1) || (dest_hw_id == 0)) && (dest_hw_type == 1)) {
        soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ia_update_fb_id_f, iaAddr);
    } else {
        soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ia_update_ref_id_f, iaAddr);
    }

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, 0, (gdpllChan*4), regVal);
#else
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, gdpllChan, 0, regVal);
#endif

exit:
    return rv;
}

uint32 _gdpll_event_fb_divisor_get(int unit, int chan) {
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    return (pGdpllContext->dpll_chan[chan].gdpll_chan_config.event_fb.event_divisor);
}

#if 0
int
_tdpll_chan_create(int unit, uint32 flags,
                          bcm_gdpll_chan_t *pGdpllChan, int *pChan,
                          uint32 tdpll_instance_num)
{
    int rv = BCM_E_NONE;
    uint32 iaAddr;
    uint32 eventId_ref;
    uint32 regVal = 0;

    /* Checks if reference is set to be from R5 and check instance number */
    if ((bcmGdpllInputEventR5 != pGdpllChan->event_ref.input_event) ||
        (tdpll_instance_num < 0 || tdpll_instance_num >= NUM_TDPLL_INSTANCE)) {
        bsl_printf("_tdpll_chan_create: Error!! parameters\n");
        rv = BCM_E_PARAM;
        goto exit;
    }

    rv = bcm_common_gdpll_chan_create (unit, flags, pGdpllChan, pChan);
    if (BCM_FAILURE(rv)) {
        bsl_printf("_tdpll_chan_create: Error!! gdpll_chan_create failed. %s\n", bcm_errmsg(rv));
        goto exit;
    }

    if (BCM_GDPLL_NUM_IA_ENTRIES == gdpll_tdpll_state.tdpll_instace[tdpll_instance_num]) {
        /* Alloc the IA entry */
        if(_gdpll_input_eventId_get(unit,
                    &pGdpllChan->event_ref,
                    &eventId_ref) != BCM_E_NONE) {
            bsl_printf("_tdpll_chan_create: Error!! in input_eventId_get \n");
            rv = BCM_E_INTERNAL;
            goto deleteChan;
        }

        rv = _gdpll_input_array_alloc(unit, eventId_ref, &iaAddr);
        if (rv != BCM_E_NONE) {
            bsl_printf("_tdpll_chan_create: Error!! input_array_alloc failed. %s\n", bcm_errmsg(rv));
            goto deleteChan;
        }

        gdpll_tdpll_state.tdpll_instace[tdpll_instance_num] = iaAddr;
    }

    bsl_printf("_tdpll_chan_create Channel: %d ref:%d\n", *pChan, gdpll_tdpll_state.tdpll_instace[tdpll_instance_num]);

    /* Channel update with reference indix */
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, *pChan, 0, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ia_update_ref_id_f,
                    gdpll_tdpll_state.tdpll_instace[tdpll_instance_num]);
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, *pChan, 0, regVal);
    return rv;

deleteChan:
    rv = bcm_common_gdpll_chan_destroy(unit, *pChan);
    if (BCM_FAILURE(rv)) {
        bsl_printf("_tdpll_chan_create: chan destroy failed\n");
    }
exit:
    return rv;
}

/*
 * Call this API if user want to perform one of the bellow or both
 * 1. disassociate instance with all outputs with valid instance number
 * 2. Free the channel allocated
 */
int
_tdpll_gdpll_chan_destroy(int unit, int chan, uint32 tdpll_instance_num)
{
    int rv = BCM_E_NONE;

    rv = bcm_common_gdpll_chan_destroy(unit, chan);
    if (BCM_FAILURE(rv)) {
        bsl_printf("_tdpll_gdpll_chan_destroy: chan destroy failed\n");
    }

    /* Free the instance with associated outputs */
    if ((tdpll_instance_num >= 0) && (tdpll_instance_num < NUM_TDPLL_INSTANCE)) {
        gdpll_tdpll_state.tdpll_instace[tdpll_instance_num] = BCM_GDPLL_NUM_IA_ENTRIES;
    }

    return rv;
}
#endif

int bcm_common_gdpll_chan_get(
    int unit,
    int chan,
    bcm_gdpll_chan_t *gdpll_chan)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if ((chan < 0) || (chan > BCM_GDPLL_NUM_CHANNELS) || (gdpll_chan == NULL)){
        rv = BCM_E_PARAM;
        goto exit;
    }

    sal_memcpy(gdpll_chan, &(pGdpllContext->dpll_chan[chan].gdpll_chan_config),
                       sizeof(bcm_gdpll_chan_t));
exit:
    return rv;
}
#endif
