/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    modular.c
 *
 * Purpose: 
 *
 * Functions:
 *      bcm_common_ptp_modular_enable_get
 *      bcm_common_ptp_modular_enable_set
 *      bcm_common_ptp_modular_phyts_get
 *      bcm_common_ptp_modular_phyts_set
 *      bcm_common_ptp_modular_portbitmap_get
 *      bcm_common_ptp_modular_portbitmap_set
 *      bcm_common_ptp_modular_verbose_get
 *      bcm_common_ptp_modular_verbose_set
 *
 *      _bcm_ptp_modular_init 
 *      _bcm_ptp_modular_shutdown
 *      _bcm_ptp_modular_supervisor
 *      _bcm_ptp_modular_physync_synchronize
 *      _bcm_ptp_modular_physync_configure
 */

#if defined(INCLUDE_PTP)

#include <shared/util.h>
#include <shared/rdpc.h>
#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/ptp_common.h>

#include <bcm/port.h>

#include <bcm/error.h>

#include <soc/dport.h>

/* Definitions. */
#define PTP_MODULAR_SUPERVISOR_DPC_TIME_USEC_DEFAULT              (1000000)
#define PTP_MODULAR_SUPERVISOR_DPC_TIME_USEC_IDLE                 (3000000)
#define PTP_MODULAR_PHYSYNC_FRAMESYNC_WAIT_USEC                      (1000)

#define PTP_MODULAR_PHYSYNC_REFPHASE_OFFSET_SEC                         (1)

#define PTP_MODULAR_PHYSYNC_FREQUENCY_PIN                              (-1)
#define PTP_MODULAR_PHYSYNC_FRAMESYNC_PIN                               (3)

/* Macros. */

/* Types. */
typedef struct _bcm_ptp_modular_options_s {
    int enable;
    int verbose;
    int phyts;
    int framesync_pin;
    bcm_pbmp_t pbmp;
} _bcm_ptp_modular_options_t;

/* Constants and variables. */
static _bcm_ptp_modular_options_t modular_options = {
    0,
    0,
    0,
    0,
    {{0x00000000}}
};

static uint32 modular_flags;
static shr_rdpc_t modular_supervisor_rdpc;

/* Static functions. */
static sal_usecs_t _bcm_ptp_modular_supervisor(
    void **arg_unit, void **arg_ptp_id,
    void **arg_clock_num, void **unused);

static int _bcm_ptp_modular_physync_synchronize(
    int unit, bcm_ptp_stack_id_t ptp_id, int clock_num);

static int _bcm_ptp_modular_physync_configure(
    int unit, bcm_port_t port, uint64 ref_phase);

/*
 * Function:
 *      _bcm_ptp_modular_init
 * Purpose:
 *      Initialize the modular system.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      flags     - (IN) Modular system control flags. (UNUSED)
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_modular_init(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 flags)
{
    int rv;

    if (shr_rdpc_callback_created(&modular_supervisor_rdpc) == BCM_E_INIT) {
        /* RDPC (and associated lock) is left in place on cleanup, so only create it once */
        rv = shr_rdpc_callback_create(&modular_supervisor_rdpc, &_bcm_ptp_modular_supervisor);
    } else {
        rv = shr_rdpc_callback_stop(&modular_supervisor_rdpc);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_ptp_modular_shutdown
 * Purpose:
 *      Shut down the modular system.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_modular_shutdown(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int rv;
    /* Cancel modular system supervisor repeating delayed procedure call. */
    rv = shr_rdpc_callback_stop(&modular_supervisor_rdpc);
    return rv;
}

/*
 * Function:
 *      bcm_common_ptp_modular_enable_get
 * Purpose:
 *      Get enable/disable state of modular system functionality.
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      ptp_id    - (IN)  PTP stack ID.
 *      clock_num - (IN)  PTP clock number.
 *      enable    - (OUT) Enable Boolean.
 *      flags     - (OUT) Modular system control flags. (UNUSED)
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_modular_enable_get(
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

    *enable = modular_options.enable;
    *flags = modular_flags;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_modular_enable_set
 * Purpose:
 *      Set enable/disable state of modular system functionality.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      enable    - (IN) Enable Boolean.
 *      flags     - (IN) Modular system control flags. (UNUSED)
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_modular_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int enable,
    uint32 flags)
{
    int rv;
    int i;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_TELECOM_PROFILE_ENABLED_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    /*
     * Make payload.
     *    Octet 0...5 : Custom management message key/identifier.
     *                  BCM<null><null><null>.
     *    Octet 6     : Modular system enabled Boolean.
     *    Octet 7     : Reserved.
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i] = enable ? 1:0;

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, ptp_id,
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_MODULAR_SYSTEM_ENABLED,
            payload, PTP_MGMTMSG_PAYLOAD_MODULAR_SYSTEM_ENABLED_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /* Start modular system supervisor DPC iff modular system is enabled. */
    if (enable) {
        rv = shr_rdpc_callback_start(&modular_supervisor_rdpc, PTP_MODULAR_SUPERVISOR_DPC_TIME_USEC_DEFAULT,
                                     INT_TO_PTR(unit), INT_TO_PTR(ptp_id), INT_TO_PTR(clock_num), 0);
    } else {
        rv = shr_rdpc_callback_stop(&modular_supervisor_rdpc);
    }

    modular_options.enable = enable;
    modular_flags = flags;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_modular_verbose_get
 * Purpose:
 *      Get verbose program control option of modular system functionality.
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      ptp_id    - (IN)  PTP stack ID.
 *      clock_num - (IN)  PTP clock number.
 *      verbose   - (OUT) Verbose Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_modular_verbose_get(
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

    *verbose = modular_options.verbose;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_modular_verbose_set
 * Purpose:
 *      Set verbose program control option of modular system functionality.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      verbose   - (IN) Verbose Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_modular_verbose_set(
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

    modular_options.verbose = verbose;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_modular_portbitmap_get
 * Purpose:
 *      Get PHY port bitmap.
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      ptp_id    - (IN)  PTP stack ID.
 *      clock_num - (IN)  PTP clock number.
 *      pbmp      - (OUT) Port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_modular_portbitmap_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_pbmp_t *pbmp)
{
    int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    *pbmp = modular_options.pbmp;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_modular_portbitmap_set
 * Purpose:
 *      Set PHY port bitmap.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      pbmp      - (IN) Port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_modular_portbitmap_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_pbmp_t pbmp)
{
    int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    modular_options.pbmp = pbmp;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_modular_phyts_get
 * Purpose:
 *      Get PHY timestamping configuration state and data.
 * Parameters:
 *      unit          - (IN)  Unit number.
 *      ptp_id        - (IN)  PTP stack ID.
 *      clock_num     - (IN)  PTP clock number.
 *      phyts         - (OUT) PHY timestamping Boolean.
 *      framesync_pin - (OUT) Framesync GPIO pin.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_modular_phyts_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *phyts,
    int *framesync_pin)
{
    int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    *phyts = modular_options.phyts;
    *framesync_pin = modular_options.framesync_pin;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_modular_phyts_set
 * Purpose:
 *      Set PHY timestamping configuration state and data.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      ptp_id        - (IN) PTP stack ID.
 *      clock_num     - (IN) PTP clock number.
 *      phyts         - (IN) PHY timestamping Boolean.
 *      framesync_pin - (IN) Framesync GPIO pin.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_ptp_modular_phyts_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int phyts,
    int framesync_pin)
{
    int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    modular_options.phyts = phyts;
    modular_options.framesync_pin = framesync_pin;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_modular_supervisor
 * Purpose:
 *      Run modular system supervisor (as RDPC).
 * Parameters:
 *      arg_unit      - (IN) Unit number (as void**).
 *      arg_ptp_id    - (IN) PTP stack ID (as void**).
 *      arg_clock_num - (IN) PTP clock number (as void**).
 *      unused        - (IN) Unused.
 * Returns:
 *      Interval before next call
 * Notes:
 */
static sal_usecs_t
_bcm_ptp_modular_supervisor(
    void **arg_unit,
    void **arg_ptp_id,
    void **arg_clock_num,
    void **unused)
{
    int rv;

    int unit = PTR_TO_INT(*arg_unit);
    bcm_ptp_stack_id_t ptp_id = (bcm_ptp_stack_id_t)PTR_TO_INT(*arg_ptp_id);
    int clock_num = PTR_TO_INT(*arg_clock_num);

    if (0 == modular_options.phyts) {
        /* IDLE. PHY timestamping option is not currently set. */
        return PTP_MODULAR_SUPERVISOR_DPC_TIME_USEC_IDLE;
    } else if (BCM_FAILURE(rv = _bcm_ptp_modular_physync_synchronize(unit, ptp_id, clock_num))) {
        PTP_ERROR_FUNC("_bcm_ptp_modular_physync_synchronize()");
        return PTP_MODULAR_SUPERVISOR_DPC_TIME_USEC_IDLE;
    } else {
        return PTP_MODULAR_SUPERVISOR_DPC_TIME_USEC_DEFAULT;
    }
}

/*
 * Function:
 *      _bcm_ptp_modular_physync_synchronize
 * Purpose:
 *      Synchronize PHYs.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_modular_physync_synchronize(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int rv;
    int i;

    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    uint8 physync_reqd = 0;
    uint64 localTime_sec;
    uint32 localTime_nsec;
    uint64 physyncTime_nsec;

    soc_port_t p, dport;
    bcm_ptp_sync_phy_input_t physync_input;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, ptp_id,
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    /* Query stack to determine whether PHY synchronization is required. */
    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, ptp_id,
            clock_num, &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_PHYSYNC_STATE,
            0, 0, resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5  : Custom management message key/identifier.
     *                   BCM<null><null><null>.
     *    Octet 6      : PHY synchronization required Boolean.
     *    Octet 7      : Reserved.
     *    Octet 8...15 : Stack local time (sec).
     *    Octet 16...19: stack local time (nsec).
     */
    i = 6; /* Advance cursor past custom management message identifier. */
    physync_reqd = resp[i++];
    i++;   /* Advance cursor past reserved octet. */

    localTime_sec = _bcm_ptp_uint64_read(resp + i);
    i += 8;
    localTime_nsec = _bcm_ptp_uint32_read(resp + i);

    /* PHY synchronization. */
    if (0 == physync_reqd) {
        /* PHY synchronization is not required. */
        return rv;
    }

    /*
     * PHY synchronization.
     * Calculate reference phase to program PHYs.
     * Reference phase based on local time (i.e. when stack is queried for
     * PHY synchronization state) plus sufficient delta to guarantee stack
     * can receive and process PHY framesync at BSHB which corresponds to
     * programmed phase.
     * 
     * NOTE: Reference phase is quantized in microsecond to align with BSHB.
     *       Katana timing card PHY synchronization integrated with BSHB TS
     *       processing and has access to full-rate (4KHz) BSHB interrupts.
     *       Keystone timing card PHY synchronization integrated with BSHB
     *       DPLL and hass access to 1KHz timestamps for DPLL update.
     */
    physyncTime_nsec = localTime_sec;
    COMPILER_64_ADD_32(physyncTime_nsec, PTP_MODULAR_PHYSYNC_REFPHASE_OFFSET_SEC);
    COMPILER_64_UMUL_32(physyncTime_nsec, 1000000000);
    COMPILER_64_ADD_32(physyncTime_nsec, ((localTime_nsec+500000)/1000000)*1000000);

    /*
     * PHY synchronization.
     * Trigger/stage framesync.
     * Program reference phase.
     * Send framesync (FW initiated GPIO high/low @ prescribed time).
     */
    
    physync_input.framesync_pin = modular_options.framesync_pin > 0 ?
        modular_options.framesync_pin : PTP_MODULAR_PHYSYNC_FRAMESYNC_PIN;
    physync_input.freq_pin= PTP_MODULAR_PHYSYNC_FREQUENCY_PIN;
    physync_input.reference_time = physyncTime_nsec;

    /* Initial framesync. Pull pin low. */
    if (BCM_FAILURE(rv = bcm_common_ptp_sync_phy(unit, ptp_id, clock_num, physync_input))) {
        PTP_ERROR_FUNC("bcm_common_ptp_sync_phy()");
        return rv;
    }

    /* Wait. Allow initial framesync to occur. */
    sal_usleep(PTP_MODULAR_PHYSYNC_FRAMESYNC_WAIT_USEC);

    /* coverity[overrun-local] */
    SOC_DPORT_PBMP_ITER(unit, modular_options.pbmp, dport, p) {
        if (BCM_FAILURE(rv = _bcm_ptp_modular_physync_configure(unit, p, physyncTime_nsec))) {
            PTP_ERROR_FUNC("_bcm_ptp_modular_physync_configure()");
            return rv;
        }
    }

    /* Actual framesync. Load PHY values. */
    if (BCM_FAILURE(rv = bcm_common_ptp_sync_phy(unit, ptp_id, clock_num, physync_input))) {
        PTP_ERROR_FUNC("bcm_common_ptp_sync_phy()");
        return rv;
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_modular_physync_configure
 * Purpose:
 *      Configure PHY port to track 4KHz SYNC_IN0 with framesync on GPIO pin.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Port.
 *      ref_phase - (IN) Reference phase.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_modular_physync_configure(
    int unit,
    bcm_port_t port,
    uint64 ref_phase)
{
    int rv;
    bcm_port_phy_timesync_config_t config;
    uint64 load_control;

    uint64 localTime;

    sal_memset(&config, 0, sizeof(config));

    config.validity_mask = BCM_PORT_PHY_TIMESYNC_VALID_FLAGS | BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE | BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE | BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1 | BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2 |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3 |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_LOOP_FILTER;

    config.flags = BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_L2_ENABLE |
        BCM_PORT_PHY_TIMESYNC_IP4_ENABLE |
        BCM_PORT_PHY_TIMESYNC_IP6_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT;

    config.gmode = bcmPortPhyTimesyncModeCpu;
    config.framesync.mode = bcmPortPhyTimesyncFramesyncSyncin1;
    config.syncout.mode = bcmPortPhyTimesyncSyncoutDisable;
    config.tx_sync_mode = bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_sync_mode = bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;
    config.tx_delay_request_mode = bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_delay_request_mode = bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;
    /* for 4KHz SyncIn */
    config.phy_1588_dpll_ref_phase = ref_phase;
    config.phy_1588_dpll_ref_phase_delta = 250000;
    config.phy_1588_dpll_k1 = 0x2b;
    config.phy_1588_dpll_k2 = 0x26;
    config.phy_1588_dpll_k3 = 0;

    /* Initial value for the loop filter: 0x8000 0000 0000 0000 */
    COMPILER_64_SET(config.phy_1588_dpll_loop_filter, 0x80000000, 0);

    if (BCM_FAILURE(rv = bcm_port_phy_timesync_config_set(unit, port, &config))) {
        PTP_ERROR_FUNC("bcm_port_phy_timesync_config_set()");
        return rv;
    }

    /*
     * Set local time (use reference phase).
     * NOTE: Empirically, programmed PHY timesync local time has scale factor
     *       of 16 compared to argument specified in nanoseconds.
     *       PHY data sheet mentions registers 2a,2b,2c to set 44 MSBs of PHY
     *       local timestamper. Control of upper 44 bits of 48-bit local time
     *       likely accounts for 4-bit (16x) scaling required to set the PHY
     *       local time with nanoseconds reference phase via following API.
     */
    localTime = _bcm_ptp_llu_div(ref_phase, 16);
    if (BCM_FAILURE(rv = bcm_port_control_phy_timesync_set(unit, port,
            bcmPortControlPhyTimesyncLocalTime, localTime))) {
        PTP_ERROR_FUNC("bcm_port_control_phy_timesync_set()");
        return rv;
    }

    COMPILER_64_SET(load_control, 0,
                    BCM_PORT_PHY_TIMESYNC_TN_LOAD |
                    BCM_PORT_PHY_TIMESYNC_TIMECODE_LOAD |
                    BCM_PORT_PHY_TIMESYNC_SYNCOUT_LOAD |
                    BCM_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD |
                    BCM_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD |
                    BCM_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD |
                    BCM_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD |
                    BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD |
                    BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD |
                    BCM_PORT_PHY_TIMESYNC_DPLL_K3_LOAD |
                    BCM_PORT_PHY_TIMESYNC_DPLL_K2_LOAD |
                    BCM_PORT_PHY_TIMESYNC_DPLL_K1_LOAD );

    /* Set load control. */
    if (BCM_FAILURE(rv = bcm_port_control_phy_timesync_set(unit, port,
            bcmPortControlPhyTimesyncLoadControl, load_control))) {
        PTP_ERROR_FUNC("bcm_port_control_phy_timesync_set()");
        return rv;
    }

    return BCM_E_NONE;
}

#endif /* defined(INCLUDE_PTP) */
