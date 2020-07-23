/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tdpll_inputs.c
 *
 * Purpose: Telecom DPLL input clock monitoring, reference selection, and switching.
 *
 * Functions:
 *      bcm_tdpll_input_clock_init
 *      bcm_tdpll_input_clock_shutdown
 *      bcm_tdpll_input_clock_control
 *      bcm_tdpll_input_clock_port_lookup
 *      bcm_tdpll_input_clock_mac_lookup
 *      bcm_tdpll_input_clock_mac_get
 *      bcm_tdpll_input_clock_mac_set
 *      bcm_tdpll_input_clock_reference_mac_get
 *      bcm_tdpll_input_clock_frequency_error_get
 *      bcm_tdpll_input_clock_threshold_state_get
 *      bcm_tdpll_input_clock_enable_get
 *      bcm_tdpll_input_clock_enable_set
 *      bcm_tdpll_input_clock_l1mux_get
 *      bcm_tdpll_input_clock_l1mux_set
 *      bcm_tdpll_input_clock_valid_get
 *      bcm_tdpll_input_clock_valid_set
 *      bcm_tdpll_input_clock_dpll_use_get
 *      bcm_tdpll_input_clock_dpll_use_set
 *      bcm_tdpll_input_clock_frequency_get
 *      bcm_tdpll_input_clock_frequency_set
 *      bcm_tdpll_input_clock_ql_get
 *      bcm_tdpll_input_clock_ql_set
 *      bcm_tdpll_input_clock_priority_get
 *      bcm_tdpll_input_clock_priority_set
 *      bcm_tdpll_input_clock_lockout_get
 *      bcm_tdpll_input_clock_lockout_set
 *      bcm_tdpll_input_clock_monitor_interval_get
 *      bcm_tdpll_input_clock_monitor_interval_set
 *      bcm_tdpll_input_clock_monitor_threshold_get
 *      bcm_tdpll_input_clock_monitor_threshold_set
 *      bcm_tdpll_input_clock_ql_enabled_get
 *      bcm_tdpll_input_clock_ql_enabled_set
 *      bcm_tdpll_input_clock_revertive_get
 *      bcm_tdpll_input_clock_revertive_set
 *      bcm_tdpll_input_clock_best_get
 *      bcm_tdpll_input_clock_dpll_reference_get
 *      bcm_tdpll_input_clock_monitor_callback_register
 *      bcm_tdpll_input_clock_monitor_callback_unregister
 *      bcm_tdpll_input_clock_selector_callback_register
 *      bcm_tdpll_input_clock_selector_callback_unregister
 *      bcm_common_tdpll_input_clock_callback_register
*       bcm_common_tdpll_input_clock_callback_unregister
 *
 *      bcm_tdpll_input_clock_state_machine
 *      bcm_tdpll_input_clock_reference_selector
 *      bcm_tdpll_input_clock_monitor_gateway
 *      bcm_tdpll_input_clock_monitor_data_get
 *      bcm_tdpll_input_clock_monitor_calc
 *      bcm_tdpll_input_clock_monitor_eval
 */

#if defined(INCLUDE_PTP)

#include <shared/bsl.h>
#include <shared/rdpc.h>

#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/ptp_common.h>
#include <bcm/error.h>

/* Definitions. */
#define TDPLL_USEC_PER_SEC                                        (1000000)
#define TDPLL_NSEC_PER_SEC                                     (1000000000)

#define TDPLL_ESMC_FAILURE_TIMEOUT_SEC                                  (5)

#define TDPLL_MONITOR_INTERVAL_SEC_MIN                                  (4)
#define TDPLL_MONITOR_INTERVAL_SEC_MAX                               (2048)
#define TDPLL_MONITOR_INTERVAL_SEC_DEFAULT                             (32)

#define TDPLL_ALARM_SOFT_WARN_THRESHOLD_PPB                          (8000)
#define TDPLL_ALARM_HARD_ACCEPT_THRESHOLD_PPB                        (9000)
#define TDPLL_ALARM_HARD_REJECT_THRESHOLD_PPB                       (12000)

#define TDPLL_FREQUENCY_ERROR_MAX_PPB                          (1000000000)

#define TDPLL_STATE_MACHINE_DPC_TIME_USEC_DEFAULT                 (1000000)
#define TDPLL_STATE_MACHINE_DPC_TIME_USEC_IDLE                   (10000000)

#define TDPLL_INPUT_L1MUX_NUM_MAX                                      (2)

#define TDPLL_INPUT_CLOCK_STATE_ENABLE_BIT                             (0u)
#define TDPLL_INPUT_CLOCK_STATE_TSAVAIL_BIT                            (1u)
#define TDPLL_INPUT_CLOCK_STATE_VALID_BIT                              (2u)
#define TDPLL_INPUT_CLOCK_STATE_QL_DNU_BIT                             (3u)

#define INPUT_CLOCK(clock_index)       \
(objdata.input_clock[clock_index])

#define INPUT_CLOCK_BEST(dpll_index)  \
(objdata.input_clock[objdata.selector_state.selected_clock[dpll_index]])

#define INPUT_CLOCK_ACTIVE(dpll_index) \
(objdata.input_clock[objdata.selector_state.reference_clock[dpll_index]])

#define INPUT_CLOCK_DPLL_REF(dpll_index) \
(objdata.selector_state.reference_clock[dpll_index])

#if defined(BCM_SABER2_SUPPORT)
#define SB2_EPHY_CLKSEL                                                (30)
#define EPHY_PORT_MAP(i,L1MuxPort,phy_port,port) \
/* ext.PHY 25MHz recovered clock,check SB2 top spec for lane info */\
if(SB2_EPHY_CLKSEL==L1MuxPort) {\
    port = phy_port; /*PHY port used for L1 clk recovery*/\
} else { /* SB2 without ext.PHY L1 recovery support */\
    port = L1MuxPort+1;\
}
#endif /*BCM_SABER2_SUPPORT*/

/* Macros. */

/* Types. */

/* Constants and variables. */
static bcm_tdpll_input_clock_data_t objdata;  
static shr_rdpc_t tdpll_input_clock_state_machine_rdpc; 

/* Static functions. */
static sal_usecs_t bcm_tdpll_input_clock_state_machine(
    void **arg_unit, void **arg_stack_id,
    void **unused0, void **unused1);

static int bcm_tdpll_input_clock_esmc_timeout(
    int unit, int stack_id);

static int bcm_tdpll_input_clock_reference_selector(
    int dpll_index);

static int bcm_tdpll_input_clock_monitor_gateway(
    int unit, int stack_id);

static int bcm_tdpll_input_clock_monitor_data_get(
    int unit, int stack_id);

static int bcm_tdpll_input_clock_monitor_calc(
    int unit, int stack_id,
    bcm_tdpll_input_clock_t *input_clock);

static int bcm_tdpll_input_clock_monitor_eval(
    int unit, int stack_id,
    bcm_tdpll_input_clock_t *input_clock);

#if defined(BCM_MONTEREY_SUPPORT) & defined(INCLUDE_GDPLL)
#include <bcm/gdpll.h>
#include <soc/portmod/portmod.h>
#include <bcm/link.h>

#define TS1_OFFSET_INVALID     (0x10000000)

static int64 uc_2a_ts1_offset = 0;

typedef enum bcm_uc_2a_manager_state_e {
    bcmUc2aStateUninitialized,
    bcmUc2aStateChanNoTrigg,
    bcmUc2aStateReconfig,
    bcmUc2aStateActive,
    bcmUc2aStateLast
} bcm_uc_2a_manager_state_t;

typedef struct bcm_uc_2a_manager_context_s {
    bcm_uc_2a_manager_state_t state;
    uint8 hw_err_count;
    uint8  no_hw_err_count;
} bcm_uc_2a_manager_context_t;

bcm_uc_2a_manager_context_t bcm_uc_2a_manager_ctxt[BCM_GDPLL_NUM_CHANNELS];

extern int _bcm_esw_synce_phy_port_get_lane(int unit, int port, int rx_lane, int *adjusted_lane, int *adjusted_phy_port);
extern int _bcm_esw_synce_phy_lane_get_port(int unit, int lanePort, int isRx, uint32 *lport);
extern void _bcm_common_tdpll_dpll_holdover_set(int unit, uint16 dpll_index, int enable_ho);
extern void _bcm_common_tdpll_dpll_pbo_set(uint16 input_clk_idx);
extern int _bcm_esw_gdpll_chan_get(int unit, int chan, bcm_gdpll_chan_t *pGdpllChan);
static void _bcm_usecase_manager_state_machine(int unit);
#endif /* defined(BCM_MONTEREY_SUPPORT) & defined(INCLUDE_GDPLL) */

/*
 * Function:
 *      bcm_tdpll_input_clock_init()
 * Purpose:
 *      Initialize T-DPLL input clock functionality.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_input_clock_init(
    int unit,
    int stack_id)
{
    int i;
    int dpll_index;

    if (shr_rdpc_callback_created(&tdpll_input_clock_state_machine_rdpc) == BCM_E_INIT) {
        /* RDPC (and associated lock) is left in place on cleanup, so only create it once */
        shr_rdpc_callback_create(&tdpll_input_clock_state_machine_rdpc, &bcm_tdpll_input_clock_state_machine);
    }

    /* INPUT CLOCK MONITORING. */

    /* Set default monitoring parameters and thresholds. */
    objdata.monitor_options.interval = TDPLL_MONITOR_INTERVAL_SEC_DEFAULT;
    objdata.monitor_options.soft_warn_threshold_ppb = TDPLL_ALARM_SOFT_WARN_THRESHOLD_PPB;
    objdata.monitor_options.hard_accept_threshold_ppb = TDPLL_ALARM_HARD_ACCEPT_THRESHOLD_PPB;
    objdata.monitor_options.hard_reject_threshold_ppb = TDPLL_ALARM_HARD_REJECT_THRESHOLD_PPB;

    /* Monitor callback. */
    objdata.monitor_callback = NULL;

    objdata.callback[bcmTdpllCallbackTypeMonitor] = NULL;
    objdata.callback[bcmTdpllCallbackTypeSelector] = NULL;
    objdata.callback[bcmTdpllCallbackTypeNotification] = NULL;

    /*Initialize input clock attributes. */
    for (i = 0; i < TDPLL_INPUT_CLOCK_NUM_MAX; ++i) {
        /* Identification attributes. */
        INPUT_CLOCK(i).index = i;
        sal_memset(INPUT_CLOCK(i).mac, 0, sizeof(bcm_mac_t));

        /*
         * L1 mux port assignment.
         * First SyncE clock mapped to primary L1 clock recovery mux.
         * Other SyncE clocks mapped to backup L1 clock recovery mux.
         */
        INPUT_CLOCK(i).l1mux.index = (i <= TDPLL_INPUT_CLOCK_NUM_GPIO) ? 0:1;
        INPUT_CLOCK(i).l1mux.port = 0;

        /* DPLL instance assignments. */
        for (dpll_index = 0; dpll_index < TDPLL_DPLL_INSTANCE_NUM_MAX; ++dpll_index) {
            INPUT_CLOCK(i).dpll_use[dpll_index] = 0;
        }

        /* Clock frequency and TS EVENT (timestamp) frequency. */
        INPUT_CLOCK(i).frequency.clock = 0;
        INPUT_CLOCK(i).frequency.tsevent = 0;
        INPUT_CLOCK(i).frequency.tsevent_quotient = -1;

        /* State. */
        INPUT_CLOCK(i).state = 0;

        /* Monitor. */
        INPUT_CLOCK(i).monitor.over_soft_warn_threshold = 0;
        INPUT_CLOCK(i).monitor.under_hard_accept_threshold = 0;
        INPUT_CLOCK(i).monitor.over_hard_reject_threshold = 0;

        COMPILER_64_SET(INPUT_CLOCK(i).monitor.dt_ns, 0, TDPLL_MONITOR_INTERVAL_SEC_DEFAULT);
        COMPILER_64_UMUL_32(INPUT_CLOCK(i).monitor.dt_ns, TDPLL_NSEC_PER_SEC);
        INPUT_CLOCK(i).monitor.dtref_ns = INPUT_CLOCK(i).monitor.dt_ns;

        COMPILER_64_ZERO(INPUT_CLOCK(i).monitor.dt_sum_ns);
        INPUT_CLOCK(i).monitor.dtref_sum_ns = INPUT_CLOCK(i).monitor.dt_sum_ns;
        COMPILER_64_ZERO(INPUT_CLOCK(i).monitor.prior_evnum);
        INPUT_CLOCK(i).monitor.numev_sum = 0;

        INPUT_CLOCK(i).monitor.num_missing_tsevent = 0;
        
        /* Reference selection. */
        INPUT_CLOCK(i).select.ql = bcm_esmc_g781_II_ql_dus;
        INPUT_CLOCK(i).select.priority = i;
        INPUT_CLOCK(i).select.lockout = 0;
    }

    
    /* REFERENCE SELECTION. */
    for (dpll_index = 0; dpll_index < TDPLL_DPLL_INSTANCE_NUM_MAX; ++dpll_index) {
        /* Set default reference selection algorithm options per DPLL instance. */
        objdata.selector_options.ql_enabled[dpll_index] = 0;

        /* Set selected input clocks for each DPLL instance. */
        objdata.selector_state.prior_selected_clock[dpll_index] = -1;
        objdata.selector_state.selected_clock[dpll_index] = -1;
        objdata.selector_state.reference_clock[dpll_index] = -1;
    }

    /* Reference selection callback. */
    objdata.selector_callback = NULL;

    /* REFERENCE SWITCHING. */
    for (dpll_index = 0; dpll_index < TDPLL_DPLL_INSTANCE_NUM_MAX; ++dpll_index) {
        /* Set default reference switching cotrol parameters. */
        objdata.switching_options.revertive[dpll_index] = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_cleanup()
 * Purpose:
 *      Uninitialize T-DPLL input clock functionality.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_input_clock_cleanup(
    int unit)
{
    return shr_rdpc_callback_stop(&tdpll_input_clock_state_machine_rdpc);
}

/*
 * Function:
 *      bcm_tdpll_input_clock_control()
 * Purpose:
 *      Start/stop T-DPLL input clock monitoring, reference selection, and switching
 *      state machine.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 *      enable   - (IN) Enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_control(
    int unit,
    int stack_id,
    int enable)
{
    int rv;
    if (enable) {
        rv = shr_rdpc_callback_start(&tdpll_input_clock_state_machine_rdpc, TDPLL_STATE_MACHINE_DPC_TIME_USEC_DEFAULT,
                                INT_TO_PTR(unit), INT_TO_PTR(stack_id), 0, 0);
    } else {
        rv = shr_rdpc_callback_stop(&tdpll_input_clock_state_machine_rdpc);
    }

    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_port_lookup()
 * Purpose:
 *      Look up a SyncE input clock by port number.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      port_num    - (IN) Physical port number.
 *      clock_index - (OUT) Input clock index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_input_clock_port_lookup(
    int unit,
    int stack_id,
    int port_num,
    int *clock_index)
{
    int el;
    int logical_port_num = port_num - 1;

    if (SOC_IS_QUX(unit) || SOC_IS_QAX(unit) || SOC_IS_QMX(unit)|| SOC_IS_GREYHOUND2(unit) || SOC_IS_MONTEREY(unit)) {
        logical_port_num = port_num;
    }

    for (el = 0; el < TDPLL_INPUT_CLOCK_NUM_SYNCE; ++el) {
        LOG_DEBUG(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                        "bcm_tdpll_input_clock_port_lookup: ClkIdx:%d lport_num:%d port:%d l1m.port[%d])\n"),
                        el, logical_port_num, port_num, INPUT_CLOCK(el+TDPLL_INPUT_CLOCK_NUM_GPIO).l1mux.port));
#if defined(BCM_SABER2_SUPPORT)
        if(SB2_EPHY_CLKSEL==INPUT_CLOCK(el+TDPLL_INPUT_CLOCK_NUM_GPIO).l1mux.port) {/* ext.PHY 25MHz recovered clock,check SB2 top spec for lane info */
            *clock_index = el+TDPLL_INPUT_CLOCK_NUM_GPIO;
            objdata.phy_port[el+TDPLL_INPUT_CLOCK_NUM_GPIO] = port_num; /*ext.PHY port mapped to SB2 port num */
            return BCM_E_NONE;
        }
#endif
        if (logical_port_num == INPUT_CLOCK(el+TDPLL_INPUT_CLOCK_NUM_GPIO).l1mux.port) {
            /*
             * Select SyncE input clock.
             * ESMC PDU ingressed on physical port corresponding to L1 mux
             * port for a SyncE input clock.
             */
            *clock_index = el + TDPLL_INPUT_CLOCK_NUM_GPIO;
            return BCM_E_NONE;
        }
    }
    LOG_DEBUG(BSL_LS_BCM_PTP,
                (BSL_META_U(unit, "bcm_tdpll_input_clock_port_lookup: Error. port not found\n")));

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_mac_lookup()
 * Purpose:
 *      Look up an input clock by MAC address.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      mac         - (IN) MAC address.
 *      clock_index - (OUT) Input clock index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_input_clock_mac_lookup(
    int unit,
    int stack_id,
    bcm_mac_t *mac,
    int *clock_index)
{
    int el;

    for (el = 0; el < TDPLL_INPUT_CLOCK_NUM_MAX; ++el) {
        /* Scan input clock array for entry with matching MAC address. */
        if (0 == sal_memcmp(mac, INPUT_CLOCK(el).mac, sizeof(bcm_mac_t))) {
            /* Select corresponding input clock. */
            *clock_index = el;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_common_tdpll_input_clock_index_validate()
 * Purpose:
 *      Validate the TDPLL input clock index.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      clock_index - (IN) Input clock index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */

static int
_bcm_common_tdpll_input_clock_index_validate (
    int unit,
    int clock_index)
{
    int rv = BCM_E_NONE;

    if (clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
        return BCM_E_PARAM;
    }

#if defined (BCM_QAX_SUPPORT)
    if ((SOC_IS_QUX(unit) || SOC_IS_QAX(unit)) &&
        ((clock_index == TDPLL_INPUT_CLOCK_IDX_GPIO4) ||
         (clock_index == TDPLL_INPUT_CLOCK_IDX_GPIO5)) ) {
        return BCM_E_PARAM;
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_mac_get()
 * Purpose:
 *      Get MAC address of input clock.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      mac         - (OUT) Input clock MAC address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_mac_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_mac_t *mac)
{
    if (clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
        return BCM_E_PARAM;
    }

    sal_memcpy(mac, INPUT_CLOCK(clock_index).mac, sizeof(bcm_mac_t));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_mac_set()
 * Purpose:
 *      Set MAC address of input clock.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      mac         - (IN) Input clock MAC address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_mac_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_mac_t *mac)
{
    int rv = BCM_E_NONE;
    if (BCM_FAILURE(rv = _bcm_common_tdpll_input_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_input_clock_index_validate()");
        return rv;
    }

    sal_memcpy(INPUT_CLOCK(clock_index).mac, mac, sizeof(bcm_mac_t));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_reference_mac_get()
 * Purpose:
 *      Get MAC address of current selected reference clock of DPLL instance.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      mac        - (OUT) Input clock MAC address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_input_clock_reference_mac_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_mac_t *mac)
{
    int reference_index;

    /* Argument checking and error handling. */
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    reference_index = objdata.selector_state.reference_clock[dpll_index];

    if (reference_index < 0 || reference_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
        sal_memset(mac, 0, sizeof(bcm_mac_t));
        return BCM_E_PARAM;
    }

    sal_memcpy(mac, INPUT_CLOCK(reference_index).mac, sizeof(bcm_mac_t));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_frequency_error_get()
 * Purpose:
 *      Get fractional frequency error of an input clock from input-clock
 *      monitoring process.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      stack_id       - (IN) Stack identifier index.
 *      clock_index    - (IN) Input clock index.
 *      freq_error_ppb - (OUT) Input clock fractional frequency error (ppb).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_frequency_error_get(
    int unit,
    int stack_id,
    int clock_index,
    int *freq_error_ppb)
{
    if (clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
        return BCM_E_PARAM;
    }
    *freq_error_ppb = INPUT_CLOCK(clock_index).monitor.freq_error_ppb;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_threshold_state_get()
 * Purpose:
 *      Get monitor threshold state of an input clock from input-clock
 *      monitoring process.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      stack_id        - (IN) Stack identifier index.
 *      clock_index     - (IN) Input clock index.
 *      threshold_type  - (IN) Input-clock monitoring threshold type.
 *      threshold_state - (OUT) Input-clock monitoring threshold state Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_threshold_state_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_input_clock_monitor_type_t threshold_type,
    int *threshold_state)
{
    if (clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
        return BCM_E_PARAM;
    }

    switch (threshold_type) {
    case bcm_tdpll_input_clock_monitor_type_soft_warn:
        *threshold_state = INPUT_CLOCK(clock_index).monitor.over_soft_warn_threshold ? 1:0;
        break;
    case bcm_tdpll_input_clock_monitor_type_hard_accept:
        *threshold_state = INPUT_CLOCK(clock_index).monitor.under_hard_accept_threshold ? 1:0;
        break;
    case bcm_tdpll_input_clock_monitor_type_hard_reject:
        *threshold_state = INPUT_CLOCK(clock_index).monitor.over_hard_reject_threshold ? 1:0;
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_enable_get()
 * Purpose:
 *      Get input clock enable Boolean.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      enable      - (OUT) Input clock enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_enable_get(
    int unit,
    int stack_id,
    int clock_index,
    int *enable)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_ENABLED_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_ENABLED_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
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

    /* Make indexed payload to get enable Boolean for specified input clock. */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    payload[6] = (uint8)clock_index;
    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_INPUT_CLOCK_ENABLED,
            payload, PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : Input clock index.
     *    Octet 7       : Input clock enable Boolean.
     */
    i = 6; /* Advance cursor past custom management message identifier. */
    ++i;   /* Advance past input clock index. */
    
    *enable = resp[i] ? 1:0;

    /* Set host-maintained input clock enable Boolean. */
    if (*enable) {
        INPUT_CLOCK(clock_index).state |= (1 << TDPLL_INPUT_CLOCK_STATE_ENABLE_BIT);
    } else {
        INPUT_CLOCK(clock_index).state &= ~(1 << TDPLL_INPUT_CLOCK_STATE_ENABLE_BIT);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_enable_set()
 * Purpose:
 *      Set input-clock enable Boolean.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      enable      - (IN) Input clock enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_enable_set(
    int unit,
    int stack_id,
    int clock_index,
    int enable)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_ENABLED_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_common_tdpll_input_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_input_clock_index_validate()");
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
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : Input clock index.
     *    Octet 7       : Input clock enable Boolean.
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i++] = (uint8)clock_index;
    payload[i] = enable ? 1:0;

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_INPUT_CLOCK_ENABLED,
            payload, PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_ENABLED_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /* Set host-maintained input clock enable Boolean. */
    if (enable) {
        INPUT_CLOCK(clock_index).state |= (1 << TDPLL_INPUT_CLOCK_STATE_ENABLE_BIT);
    } else {
        INPUT_CLOCK(clock_index).state &= ~(1 << TDPLL_INPUT_CLOCK_STATE_ENABLE_BIT);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_l1mux_get()
 * Purpose:
 *      Get L1 mux mapping (mux index and port number) of input clock.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      l1mux       - (OUT) L1 mux mapping.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_l1mux_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_input_clock_l1mux_t *l1mux)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_L1MUX_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_L1MUX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
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

#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
    if (SOC_IS_MONTEREY(unit)) {
        uint32 lport=0;

        l1mux->index = INPUT_CLOCK(clock_index).l1mux.index;
        if (BCM_FAILURE(rv = _bcm_esw_synce_phy_lane_get_port(0, INPUT_CLOCK(clock_index).l1mux.port, 1, &lport))) {
            PTP_ERROR_FUNC("_bcm_esw_synce_phy_lane_get_port()");
            return rv;
        } else {
            LOG_DEBUG(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit, "bcm_common_tdpll_input_clock_l1mux_get: laneadjusted[%d] logical_port[%d] rv[%d]\n"), 
                    INPUT_CLOCK(clock_index).l1mux.port, lport, rv));
            l1mux->port = lport;
            return rv;
        }
    }
#endif

    /* Make indexed payload to get L1 mux port number for specified input clock. */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    payload[6] = (uint8)clock_index;
    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_INPUT_CLOCK_L1MUX,
            payload, PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5 : Custom management message key/identifier.
     *                  BCM<null><null><null>.
     *    Octet 6     : Input clock index.
     *    Octet 7     : L1 mux index.
     *    Octet 8     : L1 mux port number.
     *    Octet 9     : Reserved.
     */
    i = 6; /* Advance cursor past custom management message identifier. */
    ++i;   /* Advance past input clock index. */
    
    l1mux->index = (int)((int8)resp[i++]);
    l1mux->port = (int)((int8)resp[i]);

    /* Set host-maintained L1 mux and mux port number. */
    INPUT_CLOCK(clock_index).l1mux.index = l1mux->index;
    INPUT_CLOCK(clock_index).l1mux.port = l1mux->port;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_l1mux_set()
 * Purpose:
 *      Set L1 mux mapping (mux index and port number) of input clock.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      l1mux       - (IN) L1 mux mapping.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_l1mux_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_input_clock_l1mux_t *l1mux)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_L1MUX_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX ||
        l1mux->index < 0 || l1mux->index >= TDPLL_INPUT_L1MUX_NUM_MAX) {
        return BCM_E_PARAM;
    }

    if (BCM_FAILURE(rv = _bcm_common_tdpll_input_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_input_clock_index_validate()");
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
     *    Octet 6     : Input clock index.
     *    Octet 7     : L1 mux index.
     *    Octet 8     : L1 mux port number.
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i++] = (uint8)clock_index;
    payload[i++] = (uint8)l1mux->index;

#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
    if (SOC_IS_MONTEREY(unit)) {
        /* notify f/w with number adjusted for lane-swap*/
        int port = l1mux->port;
        if (BCM_FAILURE(rv = _bcm_esw_synce_phy_port_get_lane(0, port, 1, NULL, &port))) {
            PTP_ERROR_FUNC("_bcm_esw_synce_phy_port_get_lane()");
            return rv;
        } else {
            LOG_DEBUG(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit, "bcm_tdpll_input_clock_l1mux_set: logicalport[%d] laneadjusted[%d] rv[%d]\n"), l1mux->port, port, rv));
            l1mux->port = port;
        }
    }
#endif

    payload[i++] = (uint8)l1mux->port;
    payload[i] = 0;

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_INPUT_CLOCK_L1MUX,
            payload, PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_L1MUX_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /* Set host-maintained L1 mux and muport number. */
    INPUT_CLOCK(clock_index).l1mux.index = l1mux->index;
    INPUT_CLOCK(clock_index).l1mux.port = l1mux->port;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_valid_get()
 * Purpose:
 *      Get valid Boolean of an input clock from input-clock monitoring process.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      valid       - (OUT) Input clock valid Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_valid_get(
    int unit,
    int stack_id,
    int clock_index,
    int *valid)
{
    if (clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
        return BCM_E_PARAM;
    }
    *valid = (INPUT_CLOCK(clock_index).state & (1 << TDPLL_INPUT_CLOCK_STATE_VALID_BIT));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_valid_set()
 * Purpose:
 *      Set input-clock valid Boolean from monitoring process.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      valid       - (IN) Input clock valid Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Assignment is transient. Valid Boolean shall be reset by subsequent
 *      input-clock monitoring decision.
 */
int
bcm_common_tdpll_input_clock_valid_set(
    int unit,
    int stack_id,
    int clock_index,
    int valid)
{
    if (clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
        return BCM_E_PARAM;
    }

    if (valid) {
        INPUT_CLOCK(clock_index).state |= (1 << TDPLL_INPUT_CLOCK_STATE_VALID_BIT);
    } else {
        INPUT_CLOCK(clock_index).state &= ~(1 << TDPLL_INPUT_CLOCK_STATE_VALID_BIT);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_dpll_use_get()
 * Purpose:
 *      Get input-clock DPLL-use/assignment Boolean for reference selection.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      dpll_index  - (IN) DPLL instance number.
 *      dpll_use    - (OUT) DPLL-use Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      DPLL-use Boolean controls whether an input clock is used in reference
 *      selection logic for logical DPLL instance.
 */
int
bcm_tdpll_input_clock_dpll_use_get(
    int unit,
    int stack_id,
    int clock_index,
    int dpll_index,
    int *dpll_use)
{
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX ||
        clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
        return BCM_E_PARAM;
    }
    *dpll_use = INPUT_CLOCK(clock_index).dpll_use[dpll_index] ? 1:0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_dpll_use_set()
 * Purpose:
 *      Set input-clock DPLL-use/assignment Boolean for reference selection.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      dpll_index  - (IN) DPLL instance number.
 *      dpll_use    - (IN) DPLL-use Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      DPLL-use Boolean controls whether an input clock is used in reference
 *      selection logic for logical DPLL instance.
 */
int
bcm_tdpll_input_clock_dpll_use_set(
    int unit,
    int stack_id,
    int clock_index,
    int dpll_index,
    int dpll_use)
{
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX ||
        clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
        return BCM_E_PARAM;
    }
    INPUT_CLOCK(clock_index).dpll_use[dpll_index] = dpll_use ? 1:0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_frequency_get()
 * Purpose:
 *      Get input clock frequency.
 * Parameters:
 *      unit              - (IN) Unit number.
 *      stack_id          - (IN) Stack identifier index.
 *      clock_index       - (IN) Input clock index.
 *      clock_frequency   - (OUT) Frequency (Hz).
 *      tsevent_frequency - (OUT) TS event frequency (Hz).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_frequency_get(
    int unit,
    int stack_id,
    int clock_index,
    uint32 *clock_frequency,
    uint32 *tsevent_frequency)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_FREQUENCY_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_FREQUENCY_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
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

    /* Make indexed payload to get frequency for specified input clock. */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    payload[6] = (uint8)clock_index;
    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_INPUT_CLOCK_FREQUENCY,
            payload, PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : Input clock index.
     *    Octet 7       : Reserved.
     *    Octet 8...11  : Frequency (Hz).
     *    Octet 12...15 : TS event frequency (Hz).
     */
    i = 6; /* Advance cursor past custom management message identifier. */
    ++i;   /* Advance past input clock index. */
    ++i;   /* Advance past reserved octet. */
    
    *clock_frequency = _bcm_ptp_uint32_read(resp + i);
    i += 4;
    *tsevent_frequency = _bcm_ptp_uint32_read(resp + i);

    /* Set host-maintained input clock frequencies and ratio. */
    INPUT_CLOCK(clock_index).frequency.clock = *clock_frequency;
    INPUT_CLOCK(clock_index).frequency.tsevent = *tsevent_frequency;
    INPUT_CLOCK(clock_index).frequency.tsevent_quotient = *tsevent_frequency ?
        (*clock_frequency + (*tsevent_frequency >> 1))/(*tsevent_frequency) : -1;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_frequency_set()
 * Purpose:
 *      Set input clock frequency.
 * Parameters:
 *      unit              - (IN) Unit number.
 *      stack_id          - (IN) Stack identifier index.
 *      clock_index       - (IN) Input clock index.
 *      clock_frequency   - (IN) Frequency (Hz).
 *      tsevent_frequency - (IN) TS event frequency (Hz).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    uint32 clock_frequency,
    uint32 tsevent_frequency)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_FREQUENCY_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_common_tdpll_input_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_input_clock_index_validate()");
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
    if ((0 == tsevent_frequency)) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "TS EVENT frequency is not a multiple of %u Hz. fTS: %u\n"),
                     (unsigned)BCM_TDPLL_FREQUENCY, (unsigned)tsevent_frequency));
        return BCM_E_PARAM;
    }

    /*
     * CONSTRAINT: TS event frequency and input clock frequency are integrally
     *             related, N = f_clock / f_tsevent, such that TS events occur
     *             at every Nth input clock edge. 
     */
    if ((0 == tsevent_frequency) || (clock_frequency % tsevent_frequency)) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "CLOCK and TS EVENT frequencies are not integrally related. fCLK: %u fTS: %u"),
                     (unsigned)clock_frequency, (unsigned)tsevent_frequency));
        return BCM_E_PARAM;
    }

    /*
     * Make payload.
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : Input clock index.
     *    Octet 7       : Reserved.
     *    Octet 8...11  : Frequency (Hz).
     *    Octet 12...15 : TS event frequency (Hz).
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i++] = (uint8)clock_index;
    payload[i++] = 0;

    _bcm_ptp_uint32_write(payload+i, clock_frequency);
    i += 4;
    _bcm_ptp_uint32_write(payload+i, tsevent_frequency);

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_INPUT_CLOCK_FREQUENCY,
            payload, PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_FREQUENCY_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /* Set host-maintained input clock frequencies and ratio. */
    INPUT_CLOCK(clock_index).frequency.clock = clock_frequency;
    INPUT_CLOCK(clock_index).frequency.tsevent = tsevent_frequency;
    INPUT_CLOCK(clock_index).frequency.tsevent_quotient = clock_frequency/tsevent_frequency;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_ql_get()
 * Purpose:
 *      Get input clock quality level (QL).
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      ql          - (OUT) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_ql_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_esmc_quality_level_t *ql)
{
    if (clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
        return BCM_E_PARAM;
    }
    *ql = INPUT_CLOCK(clock_index).select.ql;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_ql_set()
 * Purpose:
 *      Set input clock quality level (QL).
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      ql          - (IN) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_ql_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_esmc_quality_level_t ql)
{
    int dpll_index;
    int rv = BCM_E_NONE;

    if (BCM_FAILURE(rv = _bcm_common_tdpll_input_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_input_clock_index_validate()");
        return rv;
    }

    INPUT_CLOCK(clock_index).select.ql = ql;

    /* Transmit ESMC event PDU for DPLLs that use input clock as reference. */
    for (dpll_index = 0; dpll_index < TDPLL_DPLL_INSTANCE_NUM_MAX; ++dpll_index) {
        if (clock_index == INPUT_CLOCK_DPLL_REF(dpll_index)) {
            bcm_tdpll_esmc_switch_event_send(unit, stack_id, dpll_index, ql);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_priority_get()
 * Purpose:
 *      Get input clock priority for reference selection.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      priority    - (OUT) Input clock priority.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_priority_get(
    int unit,
    int stack_id,
    int clock_index,
    int *priority)
{
    if (clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
        return BCM_E_PARAM;
    }
    *priority = INPUT_CLOCK(clock_index).select.priority;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_priority_set()
 * Purpose:
 *      Set input clock priority for reference selection.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      priority    - (IN) Input clock priority.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_priority_set(
    int unit,
    int stack_id,
    int clock_index,
    int priority)
{
    int rv = BCM_E_NONE;
    if (BCM_FAILURE(rv = _bcm_common_tdpll_input_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_input_clock_index_validate()");
        return rv;
    }

    INPUT_CLOCK(clock_index).select.priority = priority;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_lockout_get()
 * Purpose:
 *      Get input clock lockout Boolean for reference selection.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      lockout     - (OUT) Input clock lockout Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_lockout_get(
    int unit,
    int stack_id,
    int clock_index,
    int *lockout)
{
    if (clock_index < 0 || clock_index >= TDPLL_INPUT_CLOCK_NUM_MAX) {
        return BCM_E_PARAM;
    }
    *lockout = INPUT_CLOCK(clock_index).select.lockout;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_lockout_set()
 * Purpose:
 *      Set input clock lockout Boolean for reference selection.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      clock_index - (IN) Input clock index.
 *      lockout     - (IN) Input clock lockout Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_lockout_set(
    int unit,
    int stack_id,
    int clock_index,
    int lockout)
{
    int rv = BCM_E_NONE;
    if (BCM_FAILURE(rv = _bcm_common_tdpll_input_clock_index_validate(unit, clock_index))) {
        PTP_ERROR_FUNC("_bcm_common_tdpll_input_clock_index_validate()");
        return rv;
    }

    INPUT_CLOCK(clock_index).select.lockout = lockout ? 1:0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_monitor_interval_get()
 * Purpose:
 *      Get input clock monitoring interval.
 * Parameters:
 *      unit             - (IN) Unit number.
 *      stack_id         - (IN) Stack identifier index.
 *      monitor_interval - (OUT) Input clock monitoring interval (sec).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Monitoring interval defines the period over which fractional frequency error
 *      of an input clock is calculated for purposes of threshold-based comparison /
 *      validation.
 */
int
bcm_common_tdpll_input_clock_monitor_interval_get(
    int unit,
    int stack_id,
    uint32 *monitor_interval)
{
    *monitor_interval = objdata.monitor_options.interval;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_monitor_interval_set()
 * Purpose:
 *      Set input clock monitoring interval.
 * Parameters:
 *      unit             - (IN) Unit number.
 *      stack_id         - (IN) Stack identifier index.
 *      monitor_interval - (IN) Input clock monitoring interval (sec).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Monitoring interval defines the period over which fractional frequency error
 *      of an input clock is calculated for purposes of threshold-based comparison /
 *      validation.
 */
int
bcm_common_tdpll_input_clock_monitor_interval_set(
    int unit,
    int stack_id,
    uint32 monitor_interval)
{
    monitor_interval = ((monitor_interval < TDPLL_MONITOR_INTERVAL_SEC_MIN) ?
                        TDPLL_MONITOR_INTERVAL_SEC_MIN :
                        (monitor_interval > TDPLL_MONITOR_INTERVAL_SEC_MAX) ?
                        TDPLL_MONITOR_INTERVAL_SEC_MAX : monitor_interval);
    objdata.monitor_options.interval = monitor_interval;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_monitor_threshold_get()
 * Purpose:
 *      Get monitor threshold for input-clock valid classification required
 *      in reference selection.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      stack_id       - (IN) Stack identifier index.
 *      threshold_type - (IN) Input clock monitoring threshold type.
 *      threshold      - (OUT) Input clock monitoring threshold (ppb).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_monitor_threshold_get(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_type_t threshold_type,
    uint32 *threshold)
{
    switch (threshold_type) {
    case bcm_tdpll_input_clock_monitor_type_soft_warn:
        *threshold = objdata.monitor_options.soft_warn_threshold_ppb;
        break;
    case bcm_tdpll_input_clock_monitor_type_hard_accept:
        *threshold = objdata.monitor_options.hard_accept_threshold_ppb;
        break;
    case bcm_tdpll_input_clock_monitor_type_hard_reject:
        *threshold = objdata.monitor_options.hard_reject_threshold_ppb;
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_monitor_threshold_set()
 * Purpose:
 *      Set monitor threshold for input-clock valid classification required
 *      in reference selection.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      stack_id       - (IN) Stack identifier index.
 *      threshold_type - (IN) Input-clock monitoring threshold type.
 *      threshold      - (IN) Input-clock monitoring threshold (ppb).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_monitor_threshold_set(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_type_t threshold_type,
    uint32 threshold)
{
    switch (threshold_type) {
    case bcm_tdpll_input_clock_monitor_type_soft_warn:
        objdata.monitor_options.soft_warn_threshold_ppb = threshold;
        break;
    case bcm_tdpll_input_clock_monitor_type_hard_accept:
        objdata.monitor_options.hard_accept_threshold_ppb = threshold;
        break;
    case bcm_tdpll_input_clock_monitor_type_hard_reject:
        objdata.monitor_options.hard_reject_threshold_ppb = threshold;
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_ql_enabled_get()
 * Purpose:
 *      Get QL-enabled Boolean for reference selection.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      ql_enabled - (OUT) QL-enabled Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_ql_enabled_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *ql_enabled)
{
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }
    *ql_enabled = objdata.selector_options.ql_enabled[dpll_index] ? 1:0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_ql_enabled_set()
 * Purpose:
 *      Set QL-enabled Boolean for reference selection.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      ql_enabled - (IN) QL-enabled Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_ql_enabled_set(
    int unit,
    int stack_id,
    int dpll_index,
    int ql_enabled)
{
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }
    objdata.selector_options.ql_enabled[dpll_index] = ql_enabled ? 1:0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_revertive_get()
 * Purpose:
 *      Get revertive mode Boolean for reference selection and switching.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      revertive  - (OUT) Revertive mode Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_revertive_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *revertive)
{
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }
    *revertive = objdata.switching_options.revertive[dpll_index] ? 1:0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_revertive_set()
 * Purpose:
 *      Set revertive mode Boolean for reference selection and switching.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      revertive  - (IN) Revertive mode Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_input_clock_revertive_set(
    int unit,
    int stack_id,
    int dpll_index,
    int revertive)
{
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }
    objdata.switching_options.revertive[dpll_index] = revertive ? 1:0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_best_get()
 * Purpose:
 *      Get best (i.e. selected) reference for a DPLL instance.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      best_clock - (OUT) Best / preferred input clock index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Best / preferred input clock might not be active reference of DPLL
 *      instance, e.g. if revertive option is not set.
 */
int
bcm_common_tdpll_input_clock_best_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *best_clock)
{
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }
    *best_clock = objdata.selector_state.selected_clock[dpll_index];

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_dpll_reference_get()
 * Purpose:
 *      Get active reference for a DPLL instance.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      reference  - (OUT) Active reference input clock index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_input_clock_dpll_reference_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *reference)
{
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }
    *reference = INPUT_CLOCK_DPLL_REF(dpll_index);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_monitor_callback_register()
 * Purpose:
 *      Register input clock monitoring callback.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      monitor_cb - (IN) Input clock monitoring callback function pointer.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Input clock monitoring callback generates an event to notify user if
 *      state has changed w.r.t. a threshold criterion.
 */
int
bcm_common_tdpll_input_clock_monitor_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_cb monitor_cb)
{
    objdata.monitor_callback = monitor_cb;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_monitor_callback_unregister()
 * Purpose:
 *      Unregister input clock monitoring callback.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Input clock monitoring callback generates an event to notify user if
 *      state has changed w.r.t. a threshold criterion.
 */
int
bcm_common_tdpll_input_clock_monitor_callback_unregister(
    int unit,
    int stack_id)
{
    objdata.monitor_callback = NULL;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_selector_callback_register()
 * Purpose:
 *      Register input clock reference selection callback.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      selector_cb - (IN) Input clock reference selection callback function pointer.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Reference selection callback generates an event to notify user if a
 *      new reference is selected but automatic switching to it is deferred,
 *      because revertive option is not enabled.
 */
int
bcm_common_tdpll_input_clock_selector_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_selector_cb selector_cb)
{
    objdata.selector_callback = selector_cb;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_selector_callback_unregister()
 * Purpose:
 *      Unregister input clock reference selection callback.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Reference selection callback generates an event to notify user if a
 *      new reference is selected but automatic switching to it is deferred,
 *      because revertive option is not enabled.
 */
int
bcm_common_tdpll_input_clock_selector_callback_unregister(
    int unit,
    int stack_id)
{
    objdata.selector_callback = NULL;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_callback_register()
 * Purpose:
 *      Register input clock callback for various cb types.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      type       - (IN) callback type - monitor/selector/notification 
 *      callback   - (IN) Input clock callback function pointer.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *       This is a unified callback handler register mechanism
 *      that allows application to register callback functions
 *      for different callback types i.e., monitor/selector/notification.
 */
int
bcm_common_tdpll_input_clock_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_callback_type_t type,
    bcm_tdpll_input_clock_cb callback)
{
    objdata.callback[type] = callback;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_callback_unregister()
 * Purpose:
 *      Unregister input clock callback.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 *      callback - (IN) Input clock callback function pointer.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Unregister a particular callback handler say
 *      which is registered for monitor/selector/notification.
 */
int
bcm_common_tdpll_input_clock_callback_unregister(
    int unit,
    int stack_id,
    bcm_tdpll_callback_type_t type)
{
    objdata.callback[type] = NULL;
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_tdpll_input_clock_state_machine()
 * Purpose:
 *      T-DPLL reference selection (as DPC).
 * Parameters:
 *      arg_unit     - (IN) Unit number (as void*).
 *      arg_stack_id - (IN) Stack identifier index (as void*).
 *      unused0      - (IN) Unused.
 *      unused1      - (IN) Unused.
 * Returns:
 *      Time until next call
 * Notes:
 */
static sal_usecs_t
bcm_tdpll_input_clock_state_machine(
    void **arg_unit,
    void **arg_stack_id,
    void **unused0,
    void **unused1)
{
    int rv;
    int i;

    int unit = PTR_TO_INT(*arg_unit);
    int stack_id = PTR_TO_INT(*arg_stack_id);

    bcm_tdpll_input_clock_selector_cb_data_t cb_data;
    bcm_tdpll_input_clock_ref_change_cb_data_t cb_noti_data;
    bcm_tdpll_input_clock_cb_data_t cb_data_new;

    int rxen = 0;
    int update_fw_reqd = 0;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return TDPLL_STATE_MACHINE_DPC_TIME_USEC_IDLE;
    }

    if (BCM_FAILURE(rv = bcm_tdpll_esmc_rx_enable_get(unit, stack_id, &rxen))) {
        PTP_ERROR_FUNC("bcm_tdpll_esmc_rx_enable_get()");
        return TDPLL_STATE_MACHINE_DPC_TIME_USEC_IDLE;
    }

    /* SyncE input QL timeout processing iff ESMC PDU Rx is enabled. */
    if (rxen && BCM_FAILURE(rv = bcm_tdpll_input_clock_esmc_timeout(unit, stack_id))) {
        PTP_ERROR_FUNC("bcm_tdpll_input_clock_esmc_timeout()");
        return TDPLL_STATE_MACHINE_DPC_TIME_USEC_IDLE;
    }

    if (BCM_FAILURE(rv = bcm_tdpll_input_clock_monitor_gateway(unit, stack_id))) {
        PTP_ERROR_FUNC("bcm_tdpll_input_clock_monitor_gateway()");
        return TDPLL_STATE_MACHINE_DPC_TIME_USEC_IDLE;
    }

    for (i = 0; i < TDPLL_DPLL_INSTANCE_NUM_MAX; ++i) {
        /* Reference selection for a DPLL instance. */
        if (BCM_FAILURE(rv = bcm_tdpll_input_clock_reference_selector(i))) {
            /*
             * UNKNOWN best clock/no usable input clock yielded by reference
             * selection procedure.
             *
             * Force update of DPLL instance's active reference regardless
             * of revertive setting.
             */
            if (objdata.selector_state.reference_clock[i] >= 0 &&
                objdata.selector_state.reference_clock[i] < TDPLL_INPUT_CLOCK_NUM_MAX) {
                update_fw_reqd = 1;
                objdata.selector_state.reference_clock[i] = -1;

                if (objdata.callback[bcmTdpllCallbackTypeNotification]) {
                  cb_noti_data.noti_type = bcmTdpllNotificationTypeRefchange;
                  cb_noti_data.dpll_index = i;
                  cb_noti_data.prior_selected_clock = objdata.selector_state.prior_selected_clock[i];
                  cb_noti_data.selected_clock = objdata.selector_state.selected_clock[i];

                  cb_data_new.callback_type = bcmTdpllCallbackTypeNotification;
                  cb_data_new.cb_info = (void *)&cb_noti_data;
                  objdata.callback[bcmTdpllCallbackTypeNotification](unit, stack_id, &cb_data_new);
                }
#if defined (BCM_MONTEREY_SUPPORT) && defined (INCLUDE_GDPLL)
                _bcm_common_tdpll_dpll_holdover_set(unit, i, 1);
#endif
                /* Holdover event. */
                bcm_tdpll_esmc_holdover_event_send(unit, stack_id, i);
            } else if (objdata.selector_state.reference_clock[i] == -1) {
                /* (CONDITIONAL) Holdover event. */
                bcm_tdpll_esmc_holdover_event_send(unit, stack_id, i);
            }
        } else if (objdata.switching_options.revertive[i] ||
                   objdata.selector_state.reference_clock[i] == -1 ||
                   (INPUT_CLOCK_ACTIVE(i).state & (1 << TDPLL_INPUT_CLOCK_STATE_QL_DNU_BIT)) ||
                   (INPUT_CLOCK_ACTIVE(i).dpll_use[i] == 0) ||
                   (INPUT_CLOCK_ACTIVE(i).state & (1 << TDPLL_INPUT_CLOCK_STATE_VALID_BIT)) == 0) {
            /*
             * Best clock yielded by reference selection procedure.
             *
             * Conditionally update DPLL instance's active reference if
             *    - revertive mode is set,
             *    - no prior active reference,
             *    - QL-DNU/DUS of active reference clock in QL-enabled mode,
             *    - prior active reference is no longer a member of set
             *      of input clocks bound to DPLL instance,
             *    - prior active reference is invalid/unusable reference.
             */
            if (objdata.selector_state.reference_clock[i] != objdata.selector_state.selected_clock[i]) {
                update_fw_reqd = 1;
                objdata.selector_state.reference_clock[i] = objdata.selector_state.selected_clock[i];

                if (objdata.callback[bcmTdpllCallbackTypeNotification]) {
                  cb_noti_data.noti_type = bcmTdpllNotificationTypeRefchange;
                  cb_noti_data.dpll_index = i;
                  cb_noti_data.prior_selected_clock = objdata.selector_state.prior_selected_clock[i];
                  cb_noti_data.selected_clock = objdata.selector_state.selected_clock[i];

                  cb_data_new.callback_type = bcmTdpllCallbackTypeNotification;
                  cb_data_new.cb_info = (void *)&cb_noti_data;
                  objdata.callback[bcmTdpllCallbackTypeNotification](unit, stack_id, &cb_data_new);
                }

                /* Reference switch event. */
                bcm_tdpll_esmc_switch_event_send(unit, stack_id, i, INPUT_CLOCK_BEST(i).select.ql);
#if defined (BCM_MONTEREY_SUPPORT) && defined (INCLUDE_GDPLL)
                if ((objdata.selector_state.prior_selected_clock[i] == -1) &&
                        (objdata.selector_state.selected_clock[i] != -1)){
                    /* Exit from Holdover if required*/
                    _bcm_common_tdpll_dpll_holdover_set(unit, i, 0);
                } else if (objdata.selector_state.prior_selected_clock[i] != objdata.selector_state.selected_clock[i]){
                    /* Input switch has occured. perform PBO*/
                    _bcm_common_tdpll_dpll_pbo_set(objdata.selector_state.selected_clock[i]);
                }
#endif
            }
        } else {
            if (objdata.selector_state.selected_clock[i] != objdata.selector_state.prior_selected_clock[i]) {
                cb_data.dpll_index = i;
                cb_data.prior_selected_clock = objdata.selector_state.prior_selected_clock[i];
                cb_data.selected_clock = objdata.selector_state.selected_clock[i];

                if (objdata.selector_callback) { 
                    objdata.selector_callback(unit, stack_id, &cb_data);
                }

                if (objdata.callback[bcmTdpllCallbackTypeSelector]) {
                    cb_data_new.callback_type = bcmTdpllCallbackTypeSelector;
                    cb_data_new.cb_info = (void *)&cb_data;
                    objdata.callback[bcmTdpllCallbackTypeSelector](unit, stack_id, &cb_data_new);
                }

                if (objdata.callback[bcmTdpllCallbackTypeNotification]) {
                  cb_noti_data.noti_type = bcmTdpllNotificationTypeRefchange;
                  cb_noti_data.dpll_index = i;
                  cb_noti_data.prior_selected_clock = objdata.selector_state.prior_selected_clock[i];
                  cb_noti_data.selected_clock = objdata.selector_state.selected_clock[i];

                  cb_data_new.callback_type = bcmTdpllCallbackTypeNotification;
                  cb_data_new.cb_info = (void *)&cb_noti_data;
                  objdata.callback[bcmTdpllCallbackTypeNotification](unit, stack_id, &cb_data_new);
                }
            }
        }
    }

    if (1 == update_fw_reqd) {
        /* Reference switch. */
        bcm_tdpll_dpll_reference_set(unit, stack_id,
            TDPLL_DPLL_INSTANCE_NUM_MAX,
            objdata.selector_state.reference_clock);
    }

#if defined(BCM_MONTEREY_SUPPORT) && defined (INCLUDE_GDPLL)
    _bcm_usecase_manager_state_machine(unit);
#endif
    return TDPLL_STATE_MACHINE_DPC_TIME_USEC_DEFAULT;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_esmc_timeout()
 * Purpose:
 *      Check ESMC availability / timeout for SyncE input clocks.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
bcm_tdpll_input_clock_esmc_timeout(
    int unit,
    int stack_id)
{
    int i;
    int rv;
    int dpll_index;
    int port;
    bcm_esmc_network_option_t g781_option;
    bcm_esmc_quality_level_t ql_fail;

    bcm_esmc_pdu_data_t pdu_data_port;
    sal_time_t pdu_timestamp_port;

    /* Get ITU-T G.781 networking option. */
    bcm_esmc_g781_option_get(unit, stack_id, &g781_option);

    for (i = 0; i < TDPLL_INPUT_CLOCK_NUM_SYNCE; ++i) {
#if defined(BCM_SABER2_SUPPORT) /* SB2 ext.PHY L1 recovery support on portmapped to logical port*/
        EPHY_PORT_MAP(i,INPUT_CLOCK(i+TDPLL_INPUT_CLOCK_NUM_GPIO).l1mux.port,
                  objdata.phy_port[i+TDPLL_INPUT_CLOCK_NUM_GPIO],port);
#else
        if (SOC_IS_QUX(unit) || SOC_IS_QAX(unit) || SOC_IS_QMX(unit) || SOC_IS_GREYHOUND2(unit) || SOC_IS_MONTEREY(unit)) {
            port = INPUT_CLOCK(i+TDPLL_INPUT_CLOCK_NUM_GPIO).l1mux.port;
        } else {
            port = INPUT_CLOCK(i+TDPLL_INPUT_CLOCK_NUM_GPIO).l1mux.port+1;
        }
#endif /* BCM_SABER2_SUPPORT */
        if (BCM_FAILURE(rv = bcm_esmc_pdu_port_data_get(unit, stack_id,
                port,&pdu_data_port, &pdu_timestamp_port))) {
            continue;
        }

        if ((_bcm_ptp_monotonic_time() - pdu_timestamp_port) > TDPLL_ESMC_FAILURE_TIMEOUT_SEC) {
            /*
             * Elapsed time since prior ESMC PDU exceeds 5 sec. timeout per ITU-T G.8264.
             * Set QL to do not use (DNU) to signal a failure.
             */
            switch (g781_option) {
            case bcm_esmc_network_option_g781_I:
                ql_fail = bcm_esmc_g781_I_ql_dnu;
                break;
            case bcm_esmc_network_option_g781_II:
                ql_fail = bcm_esmc_g781_II_ql_dus;
                break;
            case bcm_esmc_network_option_g781_III:
                ql_fail = bcm_esmc_g781_III_ql_sec; /* ? */
                break;
            default:
                return BCM_E_PARAM;
            }

            for (dpll_index = 0; dpll_index < TDPLL_DPLL_INSTANCE_NUM_MAX; ++dpll_index) {
                if ((i+TDPLL_INPUT_CLOCK_NUM_GPIO) == INPUT_CLOCK_DPLL_REF(dpll_index)) {
                    /* DPLL selected reference failure. Transmit ESMC event PDU. */
                    LOG_DEBUG(BSL_LS_BCM_PTP,
                        (BSL_META_U(unit, "ESMC Timeout detected for dpll_idx: %d \n"), dpll_index));
                    bcm_tdpll_esmc_switch_event_send(unit, stack_id, dpll_index, ql_fail);
                }
            }

            /* Update SyncE input clock QL. */
            INPUT_CLOCK(i+TDPLL_INPUT_CLOCK_NUM_GPIO).select.ql = ql_fail;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_reference_selector()
 * Purpose:
 *      Identify best input clock to serve as a reference for DPLL instance.
 * Parameters:
 *      dpll_index - (IN) DPLL instance number.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
bcm_tdpll_input_clock_reference_selector(
    int dpll_index)
{
    int el;
    int unknown_best_clock = 0;

    int ql;
    int qlbest;
    int ql_dnu; /* QL do-not-use. */

    /* Argument checking and error handling. */
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    objdata.selector_state.prior_selected_clock[dpll_index] = 
        objdata.selector_state.selected_clock[dpll_index];

    ql_dnu = (objdata.selector_options.ql_enabled[dpll_index] &&
              (INPUT_CLOCK_BEST(dpll_index).select.ql == bcm_esmc_g781_I_ql_dnu ||
               INPUT_CLOCK_BEST(dpll_index).select.ql == bcm_esmc_g781_II_ql_dus)) ? 1:0;

    if (0 == INPUT_CLOCK_BEST(dpll_index).dpll_use[dpll_index] ||
        0 == (INPUT_CLOCK_BEST(dpll_index).state & (1 << TDPLL_INPUT_CLOCK_STATE_ENABLE_BIT)) ||
        0 == (INPUT_CLOCK_BEST(dpll_index).state & (1 << TDPLL_INPUT_CLOCK_STATE_TSAVAIL_BIT)) ||
        0 == (INPUT_CLOCK_BEST(dpll_index).state & (1 << TDPLL_INPUT_CLOCK_STATE_VALID_BIT)) ||
        1 == INPUT_CLOCK_BEST(dpll_index).select.lockout ||
        1 == ql_dnu) {
        /*
         * Prior best input clock is not associated with this DPLL instance.
         * OR prior best input clock is either not enabled or not valid.
         */
        unknown_best_clock = 1;

        /* Re-initialize preferred clock to unknown DPLL logical instance. */
        for (el = 0; el < TDPLL_INPUT_CLOCK_NUM_MAX; ++el) {
            if (INPUT_CLOCK(el).dpll_use[dpll_index]) {
                objdata.selector_state.selected_clock[dpll_index] = -1;
                break;
            }
        }
    }

    for (el = 0; el < TDPLL_INPUT_CLOCK_NUM_MAX; ++el) {
        ql_dnu = (objdata.selector_options.ql_enabled[dpll_index] &&
                  (INPUT_CLOCK(el).select.ql == bcm_esmc_g781_I_ql_dnu ||
                   INPUT_CLOCK(el).select.ql == bcm_esmc_g781_II_ql_dus)) ? 1:0;

        /* Set QL-DNU/DUS flag. */
        if (ql_dnu) {
            INPUT_CLOCK(el).state |= (1 << TDPLL_INPUT_CLOCK_STATE_QL_DNU_BIT);
        } else {
            INPUT_CLOCK(el).state &= ~(1 << TDPLL_INPUT_CLOCK_STATE_QL_DNU_BIT);
        }

        if (0 == INPUT_CLOCK(el).dpll_use[dpll_index] ||
            0 == (INPUT_CLOCK(el).state & (1 << TDPLL_INPUT_CLOCK_STATE_ENABLE_BIT)) ||
            0 == (INPUT_CLOCK(el).state & (1 << TDPLL_INPUT_CLOCK_STATE_TSAVAIL_BIT)) ||
            0 == (INPUT_CLOCK(el).state & (1 << TDPLL_INPUT_CLOCK_STATE_VALID_BIT)) ||
            1 == INPUT_CLOCK(el).select.lockout ||
            1 == ql_dnu) {
            /*
             * Input clock is not associated with this DPLL instance.
             * OR input clock is either not enabled or not valid.
             */
            continue;
        }

        if (1 == unknown_best_clock) {
            unknown_best_clock = 0;
            objdata.selector_state.selected_clock[dpll_index] = el;
        }

        if (1 == objdata.selector_options.ql_enabled[dpll_index]) {
            /* QL-enabled selection. */
            ql = INPUT_CLOCK(el).select.ql & 0xf;
            qlbest = INPUT_CLOCK_BEST(dpll_index).select.ql & 0xf;

            if (ql < qlbest) {
                /*
                 * PRIMARY SELECTION CRITERION.
                 * Higher quality level (lower numerical value) than selected
                 * reference clock.
                 */
                objdata.selector_state.selected_clock[dpll_index] = el;
            } else if (ql == qlbest &&
                       INPUT_CLOCK(el).select.priority < INPUT_CLOCK_BEST(dpll_index).select.priority) {
                /*
                 * SECONDARY SELECTION CRITERION.
                 * Equal QL, higher priority (lower numerical value) than selected
                 * reference clock.
                 */
                objdata.selector_state.selected_clock[dpll_index] = el;
            } else if (ql == qlbest &&
                       INPUT_CLOCK(el).select.priority == INPUT_CLOCK_BEST(dpll_index).select.priority &&
                       INPUT_CLOCK(el).index < INPUT_CLOCK_BEST(dpll_index).index) {
                /*
                 * TERTIARY SELECTION CRITERION.
                 * Equal (QL, priority), lesser clock index than selected
                 * reference clock.
                 */
                objdata.selector_state.selected_clock[dpll_index] = el;
            }       
        } else {
            /* QL-disabled selection. */
            if (INPUT_CLOCK(el).select.priority < INPUT_CLOCK_BEST(dpll_index).select.priority) {
                objdata.selector_state.selected_clock[dpll_index] = el;
            } else if (INPUT_CLOCK(el).select.priority == INPUT_CLOCK_BEST(dpll_index).select.priority &&
                       INPUT_CLOCK(el).index < INPUT_CLOCK_BEST(dpll_index).index) {
                /*
                 * Equal priority, lesser clock index than selected
                 * reference clock.
                 */
                objdata.selector_state.selected_clock[dpll_index] = el;
            }
        }
    }

    if (g_apts_enabled && (g_apts_usr_cfgd_source_state & bcmPtpClockAptsSourceSynceInternalDpll)){
        if ((1 == unknown_best_clock) ||
                ((objdata.selector_state.selected_clock[dpll_index] <= BCM_TDPLL_INPUT_CLOCK_NUM_GPIO) &&
                        (objdata.selector_state.selected_clock[dpll_index] >= BCM_TDPLL_INPUT_CLOCK_NUM_1588))) {
            /* Synce is not available from internal TDPLL */
            g_apts_current_source_state &= _bcm_ptp_apts_avail_ptp_gps;
        } else {
            g_apts_current_source_state |= _bcm_ptp_apts_avail_synce;
        }
        _bcm_ptp_apts_update_current_mode();
    }

    if (1 == unknown_best_clock) {
        return BCM_E_NOT_FOUND;
    } else {
        return BCM_E_NONE;
    }
}

static int
bcm_tdpll_input_clock_monitor_gateway(
    int unit,
    int stack_id)
{
    int rv;
    int i;

    /* Get input clock monitoring data. */
    if (BCM_FAILURE(rv = bcm_tdpll_input_clock_monitor_data_get(unit, stack_id))) {
        PTP_ERROR_FUNC("bcm_tdpll_input_clock_monitor_data_get()");
        return rv;
    }

    for (i = 0; i < TDPLL_INPUT_CLOCK_NUM_MAX; ++i) {
        if (BCM_FAILURE(rv = bcm_tdpll_input_clock_monitor_calc(unit, stack_id,
                &INPUT_CLOCK(i)))) {
            PTP_ERROR_FUNC("bcm_tdpll_input_clock_monitor_calc()");
            return rv;
        }
        if (BCM_FAILURE(rv = bcm_tdpll_input_clock_monitor_eval(unit, stack_id,
                &INPUT_CLOCK(i)))) {
            PTP_ERROR_FUNC("bcm_tdpll_input_clock_monitor_eval()");
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_input_clock_monitor_data_get()
 * Purpose:
 *      Get input clock prescreen state and timestamp event data.
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      stack_id - (IN)  Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Timestamps form basis for fractional frequency error measurements
 *      used in input clock monitoring.
 */
static int
bcm_tdpll_input_clock_monitor_data_get(
    int unit,
    int stack_id)
{
    int rv;
    int i;
    int index;

    uint8 resp[PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_MONITOR_DATA_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_INPUT_CLOCK_MONITOR_DATA_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

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

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_INPUT_CLOCK_MONITOR_DATA,
            0, 0, resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5     : Custom management message key/identifier.
     *                      BCM<null><null><null>.
     *    Octet 6...9     : Input clock 0 usable reference (FW classification).
     *    Octet 10...17   : Input clock 0 elapsed time (ns).
     *    Octet 18...25   : Input clock 0 TS event time (ns).
     *    Octet 26...33   : Input clock 0 TS event number.
     *    Octet 34...37   : Input clock 1 usable reference (FW classification).
     *    Octet 38...45   : Input clock 1 elapsed time (ns).
     *    Octet 46...53   : Input clock 1 TS event time (ns).
     *    Octet 54...61   : Input clock 1 TS event number.
     *    Octet 62...65   : Input clock 2 usable reference (FW classification).
     *    Octet 66...73   : Input clock 2 elapsed time (ns).
     *    Octet 74...81   : Input clock 2 TS event time (ns).
     *    Octet 82...89   : Input clock 2 TS event number.
     *    Octet 90...93   : Input clock 3 usable reference (FW classification).
     *    Octet 94...101  : Input clock 3 elapsed time (ns).
     *    Octet 102...109 : Input clock 3 TS event time (ns).
     *    Octet 110...117 : Input clock 3 TS event number.
     *    Octet 118...121 : Input clock 4 usable reference (FW classification).
     *    Octet 122...129 : Input clock 4 elapsed time (ns).
     *    Octet 130...137 : Input clock 4 TS event time (ns).
     *    Octet 138...145 : Input clock 4 TS event number.
     *    Octet 146...149 : Input clock 5 usable reference (FW classification).
     *    Octet 150...157 : Input clock 5 elapsed time (ns).
     *    Octet 158...165 : Input clock 5 TS event time (ns).
     *    Octet 166...173 : Input clock 5 TS event number.
     *    Octet 174...177 : Input clock 6 usable reference (FW classification).
     *    Octet 178...185 : Input clock 6 elapsed time (ns).
     *    Octet 186...193 : Input clock 6 TS event time (ns).
     *    Octet 194...201 : Input clock 6 TS event number.
     *    Octet 202...205 : Input clock 7 usable reference (FW classification).
     *    Octet 206...213 : Input clock 7 elapsed time (ns).
     *    Octet 214...221 : Input clock 7 TS event time (ns).
     *    Octet 222...229 : Input clock 7 TS event number.
     *    Octet 230...233 : Input clock 8 usable reference (FW classification).
     *    Octet 234...241 : Input clock 8 elapsed time (ns).
     *    Octet 242...249 : Input clock 8 TS event time (ns).
     *    Octet 250...257 : Input clock 8 TS event number.
     *    Octet 257...261 : Input clock 9 usable reference (FW classification).
     *    Octet 262...269 : Input clock 9 elapsed time (ns).
     *    Octet 270...277 : Input clock 9 TS event time (ns).
     *    Octet 278...285 : Input clock 9 TS event number.
     *
     * NOTES:
     *    Input clock TS event times are most recent timestamps.
     *
     *    Input clock elapsed times are nanonseconds since the prior TS event.
     *    If clocks are "perfect" and timestamper is tracking, elapsed times
     *    will equal 1B nanoseconds, i.e. one second.
 */
    i = 6; /* Advance cursor past custom management message identifier. */

    for (index = 0; index < TDPLL_INPUT_CLOCK_NUM_MAX; ++index) {
        objdata.prescreen_valid[index] = _bcm_ptp_uint32_read(resp + i) ? 1:0;
        i += 4;

        INPUT_CLOCK(index).monitor.tsevent_dt = _bcm_ptp_uint64_read(resp + i);
        i += 8;
        INPUT_CLOCK(index).monitor.tsevent_time = _bcm_ptp_uint64_read(resp + i);
        i += 8;
        INPUT_CLOCK(index).monitor.tsevent_num = _bcm_ptp_uint64_read(resp + i);
        i += 8;
    }

    return BCM_E_NONE;
}

static int
bcm_tdpll_input_clock_monitor_calc(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_t *input_clock)
{

    /* Argument checking and error handling. */
    if (NULL == input_clock) {
        return BCM_E_NOT_FOUND;
    }

    /*
     * Update elapsed time measurement iff new TS event data are available.
     *
     * NOTE: Frequency f over a monitoring interval [t0,tN] is given by:
     *
     *               mN
     *       f = --------- , where m equals number of input clock edges
     *           (tN - t0)   per second and N equals number of seconds.
     *
     *       Or, in terms of N subintervals [t0,t1], [t1,t2], ... [tN-1,tN]
     *
     *                                      mN
     *       f = ---------------------------------------------------------
     *           (tN - tN-1) + (tN-1 - tN-2) + ... + (t2 - t1) + (t1 - t0)
     *
     *                                       N
     *       f = ---------------------------------------------------------
     *           (tN - tN-1) + (tN-1 - tN-2) + ... + (t2 - t1) + (t1 - t0)
     *           -----------   -------------         ---------   ---------
     *               m               m                   m           m
     *
     *       Subinterval frequency fi equals (by definition):
     *                 m
     *       fi = -------------
     *            t(i) - t(i-1)
     *
     *       Effective frequency f over monitoring interval, which includes
     *       N subintervals, is a function of N subinterval frequencies:
     *
     *                     N
     *       f = ---------------------
     *            1     1           1
     *           --- + --- + ... + ---
     *           f1    f2          fN
     *
     *       1   1   1     1           1
     *       - = - (--- + --- + ... + ---)
     *       f   N  f1    f2          fN
     *
     *       Normalized subinterval frequency (fni) is obtained by dividing
     *       by the number of input clock edges per second, (m). Normalized
     *       frequency during one-second subinterval i is the reciprocal of
     *       elapsed time t(i) - t(i-1).
     *
     *       1    1    1     1           1
     *       - = --- (--- + --- + ... + ---)
     *       f   mN   fn1   fn2         fnN
     *
     *       1    1  |                                                         |
     *       - = --- |(tN - tN-1) + (tN-1 - tN-2) + ... + (t2 - t1) + (t1 - t0)|
     *       f   mN  |                                                         |
     *
     *       NB: t0, t1, ..., tN are local OCXO reference times (open-loop)
     *           and are unobservable if the timestamp counter is steered.
     *
     *           ts0, ts1, ..., tsN are local OCXO reference times (closed-
     *           loop) and include the effects of timestamper increments Xi
     *           as the timestamper frequency control is dynamically varied
     *           to track a selected reference clock. 
     *
     *           ts(i) - ts(i-1) = (1 + Xi) ( t(i) - t(i-1) )
     *
     *                             ts(i) - ts(i-1)
     *             t(i) - t(i-1) = --------------- 
     *                                 1 + Xi
     *
     *  For input clock monitoring purposes, objective is to undo effects of
     *  active, closed-loop steering of timestamper, and thus decouple input
     *  clock frequency estimates from the selected reference. The resultant
     *  frequency monitoring results are w.r.t. open-loop local OCXO, i.e.
     *  sans timestamper increment control influences.
     *
     *  1    1  |(tsN - tsN-1)   (tsN-1 - tsN-2)         (ts2 - ts1)   (ts1 - ts0)|
     *  - = --- |------------- + --------------- + ... + ----------- + -----------|
     *  f   mN  |   1 + XN            1 + XN-2              1 + X2        1 + X1  |
     *
     * Without loss of generality, the soln can omit mN scaling by factoring
     * it out of the reference clock frequency in fractional frequency error.
     * Calculation done in terms of accumulated, open-loop elapsed time, i.e.
     * with corrections to remove effects of timestamper increment control.
     *
     * NOTE: Solution currently has free-running timestamp counter for T-DPLL.
     *       The TS frequency corrections are zero by definition, and the open
     *       loop timestamps are directly measurable.
     */

    if (COMPILER_64_LE(input_clock->monitor.tsevent_num, input_clock->monitor.prior_evnum)) {
        /* Increment number of consecutive missing events. Do not rollover. */
        if (input_clock->monitor.num_missing_tsevent < ((uint32)-1)) {
            input_clock->monitor.num_missing_tsevent++;
        }

        if (input_clock->monitor.num_missing_tsevent > 3) {
            input_clock->state &= ~(1 << TDPLL_INPUT_CLOCK_STATE_TSAVAIL_BIT);
            input_clock->monitor.prior_evnum = input_clock->monitor.tsevent_num;
            /* Missing timestamps - Frequency equals zero; period is infinite.*/
            COMPILER_64_ALLONES(input_clock->monitor.dt_ns);
            COMPILER_64_SET(input_clock->monitor.dtref_ns, 0, 1000000000);
        }

        return BCM_E_NONE;
    }

    input_clock->monitor.prior_evnum = input_clock->monitor.tsevent_num;

    /* Reset number of consecutive missing events. */
    input_clock->monitor.num_missing_tsevent = 0;
    input_clock->state |= (1 << TDPLL_INPUT_CLOCK_STATE_TSAVAIL_BIT);

    COMPILER_64_ADD_64(input_clock->monitor.dt_sum_ns, input_clock->monitor.tsevent_dt);
    COMPILER_64_ADD_32(input_clock->monitor.dtref_sum_ns, 1000000000);

    input_clock->monitor.numev_sum++;

    if (input_clock->monitor.numev_sum >= objdata.monitor_options.interval) {
        input_clock->monitor.dt_ns = input_clock->monitor.dt_sum_ns;
        input_clock->monitor.dtref_ns = input_clock->monitor.dtref_sum_ns;

        COMPILER_64_ZERO(input_clock->monitor.dt_sum_ns);
        COMPILER_64_ZERO(input_clock->monitor.dtref_sum_ns);
        input_clock->monitor.numev_sum = 0;
    }

    return BCM_E_NONE;
}

static int
bcm_tdpll_input_clock_monitor_eval(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_t *input_clock)
{
    uint64 ocxodt_us;
    uint32 ocxodt_uslo;

    uint64 errabs;
    uint64 errval, errlim;

    uint16 q0, q1;
    int freqerr_sign;

    int prior_monitor_state;
    bcm_tdpll_input_clock_monitor_cb_data_t cb_data;
    bcm_tdpll_input_clock_cb_data_t cb_data_new;

    int64 servo_freq_correction_pbb;

    if (NULL == input_clock) {
        return BCM_E_NOT_FOUND;
    }

    /*
     * Calculate fractional frequency error, X, of a telecom DPLL input clock.
     *
     *       X = (f - fR)/fR, where fR equals reference frequency.
     *
     * OCXO timestamps (ti0,ti1) acquired from firmware are times in system's
     * local OCXO timeframe corresponding to TS EVENTS of input clock.
     *
     *       f = mN/(ti1 - ti0), where mN is number of T-DPLL input clock edges.
     *                           m = Number of T-DPLL input clock edges per sec.
     *                           N = Number of (one-second) periods, monitoring
     *                               window duration.
     *
     * OCXO timestamps (to0,to1) correspond to prescribed monitoring interval.
     *
     *      fR = mN/(to1 - to0), where mN is number of T-DPLL input clock edges.
     *                           m = Number of T-DPLL input clock edges per sec.
     *                           N = Number of (one-second) periods, monitoring
     *                               window duration.
     *
     * -------------------------------------------------------------------------
     *                mN            mN
     *           ----------- - -----------
     *           (ti1 - ti0)   (to1 - to0)
     *      X =  -------------------------
     *                       mN
     *                  -----------
     *                  (to1 - to0)
     *
     *           (to1 - to0)
     *       X = ----------- - 1
     *           (ti1 - ti0)
     *
     *           (to1 - to0) - (ti1 - ti0)
     *       X = -------------------------
     *                  (ti1 - ti0)
     *
     *           (REF dt - CLK_i dt)
     *       X = ----------------------
     *                  CLK_i dt
     *
     *           |REF dt - CLK_i dt|   |TS Interval Error|
     *     |X| = ------------------- = -------------------
     *                CLK_i dt              CLK_i dt
     */

    if (COMPILER_64_GE(input_clock->monitor.dtref_ns, input_clock->monitor.dt_ns)) {
        freqerr_sign = 1;
        errabs = input_clock->monitor.dtref_ns;
        COMPILER_64_SUB_64(errabs, input_clock->monitor.dt_ns);
    } else {
        freqerr_sign = -1;
        errabs = input_clock->monitor.dt_ns;
        COMPILER_64_SUB_64(errabs, input_clock->monitor.dtref_ns);
    }

    /*
     * CLASSIFICATION.
     * Alarm threshold exceedance criteria.
     *
     *  |TS Interval Error| (ns)   Threshold (ppb)
     *  ------------------------ > --------------- ?
     *         CLK_i dt     (ns)        10^9
     *
     *  or equivalently with SDK 64-bit math compliant multiplicands and divisors.
     *
     *                                    Threshold (ppb) x CLK_i dt (ns)
     *  |TS Interval Error| (ns) x 10^6 > ------------------------------- ?
     *                                                    10^3
 */
    errval = errabs;
    COMPILER_64_UMUL_32(errval, (uint32)TDPLL_USEC_PER_SEC);

    /* Soft-limit WARN threshold criterion. */
    prior_monitor_state = input_clock->monitor.over_soft_warn_threshold ? 1:0;

    errlim = input_clock->monitor.dt_ns;
    COMPILER_64_UMUL_32(errlim, objdata.monitor_options.soft_warn_threshold_ppb);
    errlim = _bcm_ptp_llu_div(errlim, 1000);
    input_clock->monitor.over_soft_warn_threshold = COMPILER_64_GE(errval, errlim) ? 1:0;

    if (input_clock->monitor.over_soft_warn_threshold != prior_monitor_state) {
       /* Input clock monitoring state change (FALSE --> TRUE or TRUE --> FALSE). */
       cb_data.index = input_clock->index;
       cb_data.monitor_type = bcm_tdpll_input_clock_monitor_type_soft_warn;
       cb_data.monitor_value = input_clock->monitor.over_soft_warn_threshold;
        if (objdata.monitor_callback) {
            objdata.monitor_callback(unit, stack_id, &cb_data);
        }

        if (objdata.callback[bcmTdpllCallbackTypeMonitor]) { 
            cb_data_new.callback_type = bcmTdpllCallbackTypeMonitor;
            cb_data_new.cb_info = (void *)&cb_data;
            objdata.callback[bcmTdpllCallbackTypeMonitor](unit, stack_id, &cb_data_new);
        }
    }

    /* Hard-limit ACCEPT threshold criterion. */
    prior_monitor_state = input_clock->monitor.under_hard_accept_threshold ? 1:0;

    errlim = input_clock->monitor.dt_ns;
    COMPILER_64_UMUL_32(errlim, objdata.monitor_options.hard_accept_threshold_ppb);
    errlim = _bcm_ptp_llu_div(errlim, 1000);
    input_clock->monitor.under_hard_accept_threshold = COMPILER_64_LT(errval, errlim) ? 1:0;

    if (input_clock->monitor.under_hard_accept_threshold != prior_monitor_state) {
        /* Input clock monitoring state change (FALSE --> TRUE or TRUE --> FALSE). */
        cb_data.index = input_clock->index;
        cb_data.monitor_type = bcm_tdpll_input_clock_monitor_type_hard_accept;
        cb_data.monitor_value = input_clock->monitor.under_hard_accept_threshold;
        if (objdata.monitor_callback) { 
            objdata.monitor_callback(unit, stack_id, &cb_data);
        }

        if (objdata.callback[bcmTdpllCallbackTypeMonitor]) { 
            cb_data_new.callback_type = bcmTdpllCallbackTypeMonitor;
            cb_data_new.cb_info = (void *)&cb_data;
            objdata.callback[bcmTdpllCallbackTypeMonitor](unit, stack_id, &cb_data_new);
        }
    }

    /* Hard-limit REJECT threshold criterion. */
    prior_monitor_state = input_clock->monitor.over_hard_reject_threshold ? 1:0;

    errlim = input_clock->monitor.dt_ns;
    COMPILER_64_UMUL_32(errlim, objdata.monitor_options.hard_reject_threshold_ppb);
    errlim = _bcm_ptp_llu_div(errlim, 1000);
    input_clock->monitor.over_hard_reject_threshold = COMPILER_64_GE(errval, errlim) ? 1:0;

    if (input_clock->monitor.over_hard_reject_threshold != prior_monitor_state) {
        /* Input clock monitoring state change (FALSE --> TRUE or TRUE --> FALSE). */
        cb_data.index = input_clock->index;
        cb_data.monitor_type = bcm_tdpll_input_clock_monitor_type_hard_reject;
        cb_data.monitor_value = input_clock->monitor.over_hard_reject_threshold;
        if (objdata.monitor_callback) { 
            objdata.monitor_callback(unit, stack_id, &cb_data);
        }

        if (objdata.callback[bcmTdpllCallbackTypeMonitor]) { 
            cb_data_new.callback_type = bcmTdpllCallbackTypeMonitor;
            cb_data_new.cb_info = (void *)&cb_data;
            objdata.callback[bcmTdpllCallbackTypeMonitor](unit, stack_id, &cb_data_new);
        }
    }

    /* Classify input clock (valid/invalid). */
    if (input_clock->monitor.over_hard_reject_threshold ||
        (0 == objdata.prescreen_valid[input_clock->index])) {
        input_clock->state &= ~(1 << TDPLL_INPUT_CLOCK_STATE_VALID_BIT);
    } else if (input_clock->monitor.under_hard_accept_threshold) {
        input_clock->state |= (1 << TDPLL_INPUT_CLOCK_STATE_VALID_BIT);
    }

    /* Estimate input clock fractional frequency error. */
    ocxodt_us = _bcm_ptp_llu_div(input_clock->monitor.dt_ns, 1000);
    ocxodt_uslo = COMPILER_64_LO(ocxodt_us);

    if (COMPILER_64_HI(ocxodt_us) || ocxodt_uslo == 0) {
        input_clock->monitor.freq_error_ppb = TDPLL_FREQUENCY_ERROR_MAX_PPB*freqerr_sign;
    } else {
        q0 = (ocxodt_uslo > 65536) ? ((ocxodt_uslo/65536) + 1):1;
        q1 = (ocxodt_uslo > q0) ? (ocxodt_uslo/q0):1;

        errval = _bcm_ptp_llu_div(errval, q0);
        errval = _bcm_ptp_llu_div(errval, q1);
        input_clock->monitor.freq_error_ppb = COMPILER_64_LO(errval)*freqerr_sign;
    }

    /* As per disucssion with Architect, in case of TS0/TS1 combined mode with PTP hybrid mode
     * where TS1 frequency corrected with synce recovered frequency, actual frequency of any input
     * will be signed addition of computed frequency offset by TDPLL monitoring algorithm and applied
     * frequency correction of currently selected synce reference. TDPLL frequency monitoring algorithm
     * will be modified for same. There will no change in existing phase monitoring computation. For example,
     * if synce recovered clock applies +100 pbb frequency offset, and for any particular input, TDPLL computes
     * frequency offset of +200pbb then, actual frequency with respect to unsteered clock with be
     *  (+100pbb +  200pbb) = 300pbb.
     */
    if(_bcm_ptp_bcm_servo_synce_freq_corr_get_in_combined_mode(unit, stack_id, &servo_freq_correction_pbb)
        == BCM_E_NONE) {
        input_clock->monitor.freq_error_ppb += servo_freq_correction_pbb;
    }



    return BCM_E_NONE;
}

#if defined(BCM_MONTEREY_SUPPORT) && (INCLUDE_GDPLL)
extern int portmod_cpri_port_encap_presentation_time_config_set(int unit, int port,
        portmod_presentation_time_attribute_t attribute,
        const portmod_presentation_time_t* time);

int _bcm_uc_2a_handle_ts1_offset_change_event(int unit, int64 new_ts1_offset)
{
    int rv = BCM_E_UNAVAIL;
    int chan = 0;
    uint32 cpri_offset = 0;
    bcm_gdpll_chan_t current_gdpll_chan;



    if (new_ts1_offset != TS1_OFFSET_INVALID) {
        uc_2a_ts1_offset = new_ts1_offset;
    }

    /*for all 2A channels, update the offset in both directions*/
    for (chan=0; chan<BCM_GDPLL_NUM_CHANNELS; chan++) {
        _bcm_esw_gdpll_chan_get(unit, chan, &current_gdpll_chan);
        if ((current_gdpll_chan.event_ref.input_event == bcmGdpllInputEventPORT) &&
                (current_gdpll_chan.event_fb.input_event == bcmGdpllInputEventPORT) &&
                (current_gdpll_chan.event_ref.port_event_type == bcmGdpllPortEventROE) &&
                (current_gdpll_chan.event_fb.port_event_type == bcmGdpllPortEventTXSOF)) {
            /*This is a 2A channel*/
            cpri_offset = ((uint32)((uint64)uc_2a_ts1_offset)) & 0x1FFFFFFF;
            LOG_INFO(BSL_LS_BCM_PTP,
                        (BSL_META_U(unit, " Applying new offset %ld channel %d port: %d 29-bit offset: %u\n"),
                                (long int)uc_2a_ts1_offset, chan, current_gdpll_chan.event_ref.port, cpri_offset));
            bcm_gdpll_offset_set(unit, chan, (uint64)uc_2a_ts1_offset);
            portmod_cpri_port_encap_presentation_time_config_set(unit, current_gdpll_chan.event_ref.port,
                    PORTMOD_ENCAP_PRESENTATION_PTP_TIME_ADJUST, (portmod_presentation_time_t *)&cpri_offset);
        }
    }

    rv = BCM_E_NONE;

    return rv;
}

void _bcm_uc_2a_update_ts1_offset(int unit, bcm_gdpll_chan_t *current_gdpll_chan, int chan)
{
    uint32 cpri_offset = 0;

    cpri_offset = ((uint32)((uint64)uc_2a_ts1_offset)) & 0x1FFFFFFF;
    LOG_INFO(BSL_LS_BCM_PTP,
                (BSL_META_U(unit, " Applying new offset %ld channel %d port: %d 29-bit offset: %u\n"),
                        (long int)uc_2a_ts1_offset, chan, current_gdpll_chan->event_ref.port, cpri_offset));
    bcm_gdpll_offset_set(unit, chan, (uint64)uc_2a_ts1_offset);
    portmod_cpri_port_encap_presentation_time_config_set(unit, current_gdpll_chan->event_ref.port,
            PORTMOD_ENCAP_PRESENTATION_PTP_TIME_ADJUST, (portmod_presentation_time_t *)&cpri_offset);

    return;
}


#define GDPLL_CHAN_STAT_HW_ERR_MASK  (0x80000000)
#define GDPLL_CHAN_STAT_LARGE_OFFSET_MASK  (0x40000000)

static void _bcm_usecase_manager_state_machine(int unit)
{
    int chan = 0;
    bcm_gdpll_chan_t current_gdpll_chan;
    bcm_uc_2a_manager_state_t old_state;
    uint32 flags = BCM_GDPLL_CONF_DPLL |
                           BCM_GDPLL_EVENT_CONFIG_REF | BCM_GDPLL_EVENT_CONFIG_FB |
                           BCM_GDPLL_CHAN_ALLOC | BCM_GDPLL_CHAN_SET_PRIORITY |
                           BCM_GDPLL_CHAN_OUTPUT_CONF;
    uint32 chan_status;

    for (chan=0; chan<BCM_GDPLL_NUM_CHANNELS; chan++) {
            _bcm_esw_gdpll_chan_get(unit, chan, &current_gdpll_chan);
            if ((current_gdpll_chan.event_ref.input_event == bcmGdpllInputEventPORT) &&
                    (current_gdpll_chan.event_fb.input_event == bcmGdpllInputEventPORT) &&
                    (current_gdpll_chan.event_ref.port_event_type == bcmGdpllPortEventROE) &&
                    (current_gdpll_chan.event_fb.port_event_type == bcmGdpllPortEventTXSOF)) {
                /*This is a 2A channel*/
                old_state = bcm_uc_2a_manager_ctxt[chan].state;
                bcm_gdpll_chan_state_get(unit, chan, &chan_status);
                LOG_DEBUG(BSL_LS_BCM_PTP, (BSL_META_U(unit, "curr_state: %d, chan state: %x\n"), old_state, chan_status));
                switch (old_state){
                case bcmUc2aStateUninitialized:
                    /*Found new 2A channel. Update state as Active (optimistic) and offset*/
                    bcm_uc_2a_manager_ctxt[chan].state = bcmUc2aStateActive;
                    bcm_uc_2a_manager_ctxt[chan].hw_err_count = 0;
                    bcm_uc_2a_manager_ctxt[chan].no_hw_err_count = 0;
                    _bcm_uc_2a_update_ts1_offset(unit, &current_gdpll_chan, chan);
                    break;

                case bcmUc2aStateChanNoTrigg:
                    /*Check for channel trigger status and update state if required */

                    if ((chan_status & GDPLL_CHAN_STAT_HW_ERR_MASK)){
                        break;
                    } else if ((chan_status & GDPLL_CHAN_STAT_LARGE_OFFSET_MASK)){
                        /*Error detected. Most likely due to link down*/
                        bcm_uc_2a_manager_ctxt[chan].no_hw_err_count += 1;
                    } else {
                        bcm_uc_2a_manager_ctxt[chan].hw_err_count = 0;
                        bcm_uc_2a_manager_ctxt[chan].state = bcmUc2aStateActive;
                    }

                    if (bcm_uc_2a_manager_ctxt[chan].no_hw_err_count > 120) {
                        bcm_uc_2a_manager_ctxt[chan].state = bcmUc2aStateReconfig;
                    }
                    break;

                case bcmUc2aStateReconfig:
                    /* Destroy and recreate the channel. Update state if successful */
                    bcm_gdpll_chan_destroy(unit, chan);
                    sal_usleep(1000000);
                    bcm_gdpll_chan_create(unit, flags, &current_gdpll_chan, &chan);
                    sal_usleep(1000000);
                    bcm_gdpll_chan_enable(unit, chan, 1);
                    bcm_uc_2a_manager_ctxt[chan].state = bcmUc2aStateActive;
                    bcm_uc_2a_manager_ctxt[chan].hw_err_count = 0;
                    bcm_uc_2a_manager_ctxt[chan].no_hw_err_count = 0;
                    sal_usleep(1000000);
                    _bcm_uc_2a_update_ts1_offset(unit, &current_gdpll_chan, chan);
                    break;

                case bcmUc2aStateActive:
                    /* Check for link status and update state if required */
                    if (chan_status & GDPLL_CHAN_STAT_HW_ERR_MASK){
                        /*Error detected. Most likely due to link down*/
                        bcm_uc_2a_manager_ctxt[chan].hw_err_count += 1;
                    }

                    if (bcm_uc_2a_manager_ctxt[chan].hw_err_count >= 2){
                        bcm_uc_2a_manager_ctxt[chan].state = bcmUc2aStateChanNoTrigg;
                    }

                    bcm_uc_2a_manager_ctxt[chan].no_hw_err_count = 0;

                    break;

                default:
                    cli_out("Error. Incorrect state detected: %d\n", old_state);
                    break;
                }
            }
    }
}
#endif

#endif /* defined(INCLUDE_PTP) */
