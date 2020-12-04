/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tdpll.c
 *
 * Purpose: Telecom DPLL logical DPLL instance configuration and management.
 *
 * Functions:
 *      bcm_tdpll_dpll_instance_init
 *      bcm_tdpll_dpll_bindings_get
 *      bcm_tdpll_dpll_bindings_set
 *      bcm_tdpll_dpll_reference_get
 *      bcm_tdpll_dpll_reference_set
 *      bcm_tdpll_dpll_bandwidth_get
 *      bcm_tdpll_dpll_bandwidth_set
 *      bcm_tdpll_dpll_phase_control_get
 *      bcm_tdpll_dpll_phase_control_set
 */

#if defined(INCLUDE_PTP)

#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/ptp_common.h>
#include <bcm/error.h>

/* Definitions. */
#define TDPLL_DPLL_BANDWIDTH_DEFAULT_VALUE  (1)
#define TDPLL_DPLL_BANDWIDTH_DEFAULT_UNITS  (bcm_tdpll_dpll_bandwidth_Hz)
#define TDPLL_DPLL_PHASEMODE_DEFAULT        (bcm_tdpll_dpll_phase_mode_pbo)

#define DPLL(dpll_index)       \
(objdata.dpll_instance[dpll_index])

/* Macros. */

/* Types. */

/* Constants and variables. */
static bcm_tdpll_dpll_instance_data_t objdata;

/* Static functions. */


/*
 * Function:
 *      bcm_tdpll_dpll_instance_init()
 * Purpose:
 *      Initialize T-DPLL logical DPLL instance configuration and management data.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) PTP stack ID.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_dpll_instance_init(
    int unit,
    int stack_id)
{
    int i;

    /*Initialize DPLL instances. */
    for (i = 0; i < TDPLL_DPLL_INSTANCE_NUM_MAX; ++i) {
        /* Identification attributes. */
        DPLL(i).index = i;

        /* DPLL input bindings. */
        SHR_BITSET_RANGE(DPLL(i).bindings.input_clocks, 0, TDPLL_INPUT_CLOCK_NUM_MAX);

        /* DPLL output bindings. */
        SHR_BITCLR_RANGE(DPLL(i).bindings.output_clocks, 0, TDPLL_OUTPUT_CLOCK_NUM_MAX);
        SHR_BITSET(DPLL(i).bindings.output_clocks, i);

        /*
         * DPLL configuration properties.
         * - bandwidth     : DPLL bandwidth.
         * - phase_control : Output phase control properties.
         */
        DPLL(i).properties.bandwidth.value = TDPLL_DPLL_BANDWIDTH_DEFAULT_VALUE;
        DPLL(i).properties.bandwidth.units = TDPLL_DPLL_BANDWIDTH_DEFAULT_UNITS;

        DPLL(i).properties.phase_control.mode = TDPLL_DPLL_PHASEMODE_DEFAULT;
        DPLL(i).properties.phase_control.offset_ns = 0;

        /* DPLL reference. */
        DPLL(i).reference = -1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_dpll_instance_cleanup()
 * Purpose:
 *      Uninitialize T-DPLL logical DPLL instance configuration and management data.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) PTP stack ID.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_dpll_instance_cleanup(
    int unit,
    int stack_id)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_dpll_bindings_get()
 * Purpose:
 *      Get logical DPLL instance input/output bindings.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      bindings   - (OUT) DPLL instance bindings.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_dpll_bindings_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bindings_t *bindings)
{
    int i;
    int el;
    int rv;

    uint8 resp[PTP_MGMTMSG_PAYLOAD_TDPLL_BINDINGS_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_TDPLL_BINDINGS_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
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

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_TDPLL_BINDINGS,
            0, 0, resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : Index, DPLL instance 0.
     *    Octet 7       : Reserved.
     *    Octet 8...11  : Input clocks, DPLL instance 0.
     *    Octet 12...15 : Output clocks, DPLL instance 0.
     *    Octet 16      : Index, DPLL instance 1.
     *    Octet 17      : Reserved.
     *    Octet 18...21 : Input clocks, DPLL instance 1.
     *    Octet 22...25 : Output clocks, DPLL instance 1.
     *    Octet 26      : Index, DPLL instance 2.
     *    Octet 27      : Reserved.
     *    Octet 28...31 : Input clocks, DPLL instance 2.
     *    Octet 32...35 : Output clocks, DPLL instance 2.
     *    Octet 36      : Index, DPLL instance 3.
     *    Octet 37      : Reserved.
     *    Octet 38...41 : Input clocks, DPLL instance 3.
     *    Octet 42...45 : Output clocks, DPLL instance 3.
     *    Octet 46      : Index, DPLL instance 4.
     *    Octet 47      : Reserved.
     *    Octet 48...51 : Input clocks, DPLL instance 4.
     *    Octet 52...55 : Output clocks, DPLL instance 4.
     *    Octet 56      : Index, DPLL instance 5.
     *    Octet 57      : Reserved.
     *    Octet 58...61 : Input clocks, DPLL instance 5.
     *    Octet 62...65 : Output clocks, DPLL instance 5.
     *    Octet 66      : Index, DPLL instance 5.
     *    Octet 67      : Reserved.
     *    Octet 68...71 : Input clocks, DPLL instance 6.
     *    Octet 72...75 : Output clocks, DPLL instance 6.
     */
    i = 6;
    for (el = 0; el < TDPLL_DPLL_INSTANCE_NUM_MAX; ++el) {
        DPLL(el).index = resp[i++];
        i++;

        
        DPLL(el).bindings.input_clocks[0] = _bcm_ptp_uint32_read(resp + i);
        i += 4;
        
        DPLL(el).bindings.output_clocks[0] = _bcm_ptp_uint32_read(resp + i);
        i += 4;
    }

    *bindings = DPLL(dpll_index).bindings;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_dpll_bindings_set()
 * Purpose:
 *      Set logical DPLL instance input/output bindings.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      bindings   - (IN) DPLL instance bindings.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_dpll_bindings_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bindings_t *bindings)
{
    int i;
    int el;
    int rv;
#if defined (BCM_MONTEREY_SUPPORT)
    int j;
    int k;
    uint8 num_instance_per_msg = 10;
    uint8 *num_instance_ptr = NULL;
#endif

    uint8 payload[PTP_MGMTMSG_PAYLOAD_TDPLL_BINDINGS_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;
    bcm_ptp_port_identity_t portid;
    uint32 output_mask;

    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }

#if defined (BCM_MONTEREY_SUPPORT)
    num_instance_per_msg = TDPLL_DPLL_INSTANCE_NUM_MAX > 10 ? 10: TDPLL_DPLL_INSTANCE_NUM_MAX;
#endif

    DPLL(dpll_index).bindings = *bindings;

#if defined (BCM_MONTEREY_SUPPORT)
    /* Limit input/output clock assignments to valid masks. */
    DPLL(dpll_index).bindings.input_clocks[_SHR_BITDCLSIZE(TDPLL_INPUT_CLOCK_NUM_MAX)-1] &=
            ((1 << (TDPLL_INPUT_CLOCK_NUM_MAX % SHR_BITWID)) - 1);

    DPLL(dpll_index).bindings.output_clocks[_SHR_BITDCLSIZE(TDPLL_OUTPUT_CLOCK_NUM_MAX)-1] &=
            ((1 << (TDPLL_OUTPUT_CLOCK_NUM_MAX % SHR_BITWID)) - 1);

#else
    /* Limit input/output clock assignments to valid masks. */
    
    DPLL(dpll_index).bindings.input_clocks[0] &= ((1 << TDPLL_INPUT_CLOCK_NUM_MAX) - 1);
    
    DPLL(dpll_index).bindings.output_clocks[0] &= ((1 << TDPLL_OUTPUT_CLOCK_NUM_MAX) - 1);

    /* Reject output clock bindings for XGPLL2/PMHPLL and XGPLL3 if not supported by h/w */
    if (!soc_feature(unit, soc_feature_tdpll_outputclk_synce3)) {
         if (TDPLL_OUTPUT_CLOCK_SYNCE3_PLL_ENABLED( DPLL(dpll_index).bindings.output_clocks[0])) {
                return BCM_E_PARAM;
            }
    }
    
    if (!soc_feature(unit, soc_feature_tdpll_outputclk_xgpll3)) {
       if (TDPLL_OUTPUT_CLOCK_SYNCE_XGPLL3_ENABLED( DPLL(dpll_index).bindings.output_clocks[0]) ) {
            return BCM_E_PARAM;
       }
    }
#endif

    /* Update reference-selection and switching logic with input-DPLL bindings. */
    for (el = 0; el < TDPLL_INPUT_CLOCK_NUM_MAX; ++el) {
        if (BCM_FAILURE(rv = bcm_tdpll_input_clock_dpll_use_set(unit, stack_id, el,
                dpll_index, SHR_BITGET(DPLL(dpll_index).bindings.input_clocks, el)))) {
            PTP_ERROR_FUNC("bcm_tdpll_dpll_instance_set()");
            continue;
        }
    }

    /*
     * Reconcile output clock assignments of DPLL instances.
     * NOTE: Each synthesizer (output clock) only bindable to one DPLL instance.
     */

#if defined(BCM_MONTEREY_SUPPORT)
    for (el = 0; el < TDPLL_DPLL_INSTANCE_NUM_MAX; ++el) {
        output_mask = DPLL(dpll_index).bindings.output_clocks[el/SHR_BITWID];
        if (dpll_index == el) {
            continue;
        }
        DPLL(el).bindings.output_clocks[el/SHR_BITWID] ^= (output_mask & DPLL(el).bindings.output_clocks[el/SHR_BITWID]);
        output_mask |= DPLL(el).bindings.output_clocks[el/SHR_BITWID];
    }
#else
    
    output_mask = DPLL(dpll_index).bindings.output_clocks[0];
    for (el = 0; el < TDPLL_DPLL_INSTANCE_NUM_MAX; ++el) {
        if (dpll_index == el) {
            continue;
        }
        DPLL(el).bindings.output_clocks[0] ^= (output_mask & DPLL(el).bindings.output_clocks[0]);
        output_mask |= DPLL(el).bindings.output_clocks[0];
    }
#endif

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
     *    Octet 6       : Index, DPLL instance 0.
     *    Octet 7       : Reserved.
     *    Octet 8...11  : Input clocks, DPLL instance 0.
     *    Octet 12...15 : Output clocks, DPLL instance 0.
     *    Octet 16      : Index, DPLL instance 1.
     *    Octet 17      : Reserved.
     *    Octet 18...21 : Input clocks, DPLL instance 1.
     *    Octet 22...25 : Output clocks, DPLL instance 1.
     *    Octet 26      : Index, DPLL instance 2.
     *    Octet 27      : Reserved.
     *    Octet 28...31 : Input clocks, DPLL instance 2.
     *    Octet 32...35 : Output clocks, DPLL instance 2.
     *    Octet 36      : Index, DPLL instance 3.
     *    Octet 37      : Reserved.
     *    Octet 38...41 : Input clocks, DPLL instance 3.
     *    Octet 42...45 : Output clocks, DPLL instance 3.
     *    Octet 46      : Index, DPLL instance 4.
     *    Octet 47      : Reserved.
     *    Octet 48...51 : Input clocks, DPLL instance 4.
     *    Octet 52...55 : Output clocks, DPLL instance 4.
     *    Octet 56      : Index, DPLL instance 5.
     *    Octet 57      : Reserved.
     *    Octet 58...61 : Input clocks, DPLL instance 5.
     *    Octet 62...65 : Output clocks, DPLL instance 5.
     *    Octet 66      : Index, DPLL instance 5.
     *    Octet 67      : Reserved.
     *    Octet 68...71 : Input clocks, DPLL instance 6.
     *    Octet 72...75 : Output clocks, DPLL instance 6.
     */
#if defined (BCM_MONTEREY_SUPPORT)
    for (k = 0; k < TDPLL_DPLL_INSTANCE_NUM_MAX; k+=num_instance_per_msg) {
        sal_memcpy(payload, "BCM\0\0\0", 6);
        i = 6;
        num_instance_ptr = &(payload[i]);
        i++;
        payload[i++] = 0;

        for (el = k; (el < (k + num_instance_per_msg) && (el < TDPLL_DPLL_INSTANCE_NUM_MAX)); ++el) {
            payload[i++] = (uint8)DPLL(el).index;
            payload[i++] = 0;

            for (j = 0; j < _SHR_BITDCLSIZE(BCM_TDPLL_INPUT_CLOCK_NUM_MAX); j++) {
                _bcm_ptp_uint32_write(payload+i, DPLL(el).bindings.input_clocks[j]);
                i += 4;
            }

            for (j = 0; j < _SHR_BITDCLSIZE(BCM_TDPLL_OUTPUT_CLOCK_NUM_MAX); j++) {
                _bcm_ptp_uint32_write(payload+i, DPLL(el).bindings.output_clocks[j]);
                i += 4;
            }
        }

        *num_instance_ptr = (uint8)(el - k);

        if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
                &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_TDPLL_BINDINGS,
                payload, PTP_MGMTMSG_PAYLOAD_TDPLL_BINDINGS_SIZE_OCTETS,
                resp, &resp_len))) {
            PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
            return rv;
        }

    }
#else
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    for (el = 0; el < TDPLL_DPLL_INSTANCE_NUM_MAX; ++el) {
        payload[i++] = (uint8)DPLL(el).index;
        payload[i++] = 0;

        
        _bcm_ptp_uint32_write(payload+i, DPLL(el).bindings.input_clocks[0]);
        i += 4;
        
        _bcm_ptp_uint32_write(payload+i, DPLL(el).bindings.output_clocks[0]);
        i += 4;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_TDPLL_BINDINGS,
            payload, PTP_MGMTMSG_PAYLOAD_TDPLL_BINDINGS_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_dpll_reference_get()
 * Purpose:
 *      Get reference clocks for DPLL instances.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      stack_id     - (IN) Stack identifier index.
 *      max_num_dpll - (IN) Maximum number of DPLL instances.
 *      dpll_ref     - (OUT) DPLL instance reference clocks.
 *      num_dpll     - (OUT) Number of DPLL instances.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Reference switching. Gets current reference clocks of DPLL instances.
 */
int
bcm_common_tdpll_dpll_reference_get(
    int unit,
    int stack_id,
    int max_num_dpll,
    int *dpll_ref,
    int *num_dpll)
{
    int rv;
    int i;
    int el;

    uint8 resp[PTP_MGMTMSG_PAYLOAD_TDPLL_REFERENCE_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_TDPLL_REFERENCE_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    /* Argument checking and error handling. */
    if (max_num_dpll < TDPLL_DPLL_INSTANCE_NUM_MAX) {
        *num_dpll = 0;
        return BCM_E_RESOURCE;
    }
    *num_dpll = TDPLL_DPLL_INSTANCE_NUM_MAX;

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
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_TDPLL_REFERENCE,
            0, 0, resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5 : Custom management message key/identifier.
     *                  BCM<null><null><null>.
     *    Octet 6     : Reference clock for DPLL instance 0.
     *    Octet 7     : Reference clock for DPLL instance 1.
     *    Octet 8     : Reference clock for DPLL instance 2.
     *    Octet 9     : Reference clock for DPLL instance 3.
     *    Octet 10    : Reference clock for DPLL instance 4.
     *    Octet 11    : Reference clock for DPLL instance 5.
     *    Octet 12    : Reference clock for DPLL instance 6.
     */
    i = 6;
    for (el = 0 ; el < *num_dpll; ++el) {
        dpll_ref[el] = (int)((int8)resp[i++]);
        /* Update host-side DPLL instance attributes. */
        DPLL(el).reference = dpll_ref[el];
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_dpll_reference_set()
 * Purpose:
 *      Set reference clocks for DPLL instances.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 *      num_dpll - (IN) Number of DPLL instances.
 *      dpll_ref - (IN) DPLL instance reference clocks.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Reference switching. Sets current reference clocks of DPLL instances.
 */
int
bcm_tdpll_dpll_reference_set(
    int unit,
    int stack_id,
    int num_dpll,
    int *dpll_ref)
{
    int rv;
    int i;
    int el;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_TDPLL_REFERENCE_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    /* Argument checking and error handling. */
    if (num_dpll > TDPLL_DPLL_INSTANCE_NUM_MAX) {
        num_dpll = TDPLL_DPLL_INSTANCE_NUM_MAX;
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
     *    Octet 6     : Reference clock for DPLL instance (DPLL instance 0).
     *    Octet 7     : Reference clock for DPLL instance (DPLL instance 1).
     *    Octet 8     : Reference clock for DPLL instance (DPLL instance 2).
     *    Octet 9     : Reference clock for DPLL instance (DPLL instance 3).
     *    Octet 10    : Reference clock for DPLL instance 4.
     *    Octet 11    : Reference clock for DPLL instance 5.
     *    Octet 12    : Reference clock for DPLL instance 6.
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    for (el = 0 ; el < num_dpll; ++el) {
        payload[i++] = (uint8)dpll_ref[el];
    }

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_TDPLL_REFERENCE,
            payload, PTP_MGMTMSG_PAYLOAD_TDPLL_REFERENCE_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /* Update host-side DPLL instance attributes. */
    for (el = 0; el < num_dpll; ++el) {
        DPLL(el).reference = dpll_ref[el];
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_dpll_bandwidth_get()
 * Purpose:
 *      Get DPLL bandwidth.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      bandwidth  - (OUT) DPLL bandwidth.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_dpll_bandwidth_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bandwidth_t *bandwidth)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_PAYLOAD_TDPLL_BANDWIDTH_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_TDPLL_BANDWIDTH_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
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

    /* Make indexed payload to get DPLL bandwidth for specified DPLL instance. */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    payload[6] = (uint8)dpll_index;
    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_TDPLL_BANDWIDTH,
            payload, PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5  : Custom management message key/identifier.
     *                   BCM<null><null><null>.
     *    Octet 6      : DPLL instance number.
     *    Octet 7...10 : DPLL bandwidth value.
     *    Octet 11     : DPLL bandwidth units.
     */
    i = 6; /* Advance cursor past custom management message identifier. */
    ++i;   /* Advance past DPLL instance number. */

    bandwidth->value = _bcm_ptp_uint32_read(resp+i);
    i += 4;
    bandwidth->units = (bcm_tdpll_dpll_bandwidth_units_t)resp[i];

    /* Set host-maintained DPLL instance properties. */
    DPLL(dpll_index).properties.bandwidth = *bandwidth;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_dpll_bandwidth_set()
 * Purpose:
 *      Set DPLL bandwidth.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      bandwidth  - (IN) DPLL bandwidth.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_dpll_bandwidth_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bandwidth_t *bandwidth)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_TDPLL_BANDWIDTH_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
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

    /*
     * Make payload.
     *    Octet 0...5  : Custom management message key/identifier.
     *                   BCM<null><null><null>.
     *    Octet 6      : DPLL instance number.
     *    Octet 7...10 : DPLL bandwidth value.
     *    Octet 11     : DPLL bandwidth units.
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i++] = (uint8)dpll_index;
    _bcm_ptp_uint32_write(payload+i, (uint32)bandwidth->value);
    i += 4;
    payload[i++] = (uint8)bandwidth->units;

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_TDPLL_BANDWIDTH,
            payload, PTP_MGMTMSG_PAYLOAD_TDPLL_BANDWIDTH_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /* Set host-maintained DPLL instance properties. */
    DPLL(dpll_index).properties.bandwidth = *bandwidth;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_dpll_phase_control_get()
 * Purpose:
 *      Get DPLL instance's phase control configuration.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      stack_id      - (IN) Stack identifier index.
 *      dpll_index    - (IN) DPLL instance number.
 *      phase_control - (OUT) Phase control configuration.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_dpll_phase_control_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_phase_control_t *phase_control)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_PAYLOAD_TDPLL_PHASE_CONTROL_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_TDPLL_PHASE_CONTROL_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
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

    /* Make indexed payload to get phase control configuration for specified DPLL instance. */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    payload[6] = (uint8)dpll_index;
    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_TDPLL_PHASE_CONTROL,
            payload, PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : DPLL instance number.
     *    Octet 7       : Phase control mode.
     *    Octet 8...11  : Phase offset (ns).
     */
    i = 6; /* Advance cursor past custom management message identifier. */
    ++i;   /* Advance past DPLL instance number. */

    phase_control->mode = (bcm_tdpll_dpll_phase_mode_t)resp[i++];
    phase_control->offset_ns = (int32)_bcm_ptp_uint32_read(resp+i);

    /* Set host-maintained DPLL instance properties. */
    DPLL(dpll_index).properties.phase_control = *phase_control;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tdpll_dpll_phase_control_set()
 * Purpose:
 *      Set DPLL instance's phase control configuration.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      stack_id      - (IN) Stack identifier index.
 *      dpll_index    - (IN) DPLL instance number.
 *      phase_control - (IN) Phase control configuration.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_dpll_phase_control_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_phase_control_t *phase_control)
{
    int i;
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_TDPLL_PHASE_CONTROL_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
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

    /*
     * Make payload.
     *    Octet 0...5   : Custom management message key/identifier.
     *                    BCM<null><null><null>.
     *    Octet 6       : DPLL instance number.
     *    Octet 7       : Phase control mode.
     *    Octet 8...11  : Phase offset (ns).
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i++] = (uint8)dpll_index;
    payload[i++] = (uint8)phase_control->mode;

    _bcm_ptp_uint32_write(payload+i, (uint32)phase_control->offset_ns);

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_TDPLL_PHASE_CONTROL,
            payload, PTP_MGMTMSG_PAYLOAD_TDPLL_PHASE_CONTROL_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /* Set host-maintained DPLL instance properties. */
    DPLL(dpll_index).properties.phase_control = *phase_control;

    return BCM_E_NONE;
}

#if defined (BCM_MONTEREY_SUPPORT) && defined (INCLUDE_GDPLL)
extern void _bcm_common_tdpll_output_holdover_set(int tdpll_outClk_index, int enable_ho);
extern void _bcm_common_tdpll_output_pbo_set(int tdpll_outClk_index);
#define BCM_TDPLL_OUTPUT_CLOCK_1588 (BCM_TDPLL_OUTPUT_CLOCK_NUM_BROADSYNC+BCM_TDPLL_OUTPUT_CLOCK_NUM_SYNCE)

void
_bcm_common_tdpll_dpll_holdover_set(
    int unit,
    uint16 dpll_index,
    int enable_ho)
{
    int outClk_index;
    LOG_INFO(BSL_LS_BCM_PTP,
                (BSL_META_U(unit, "_bcm_common_tdpll_dpll_holdover_set: dpll[%d] enable_ho[%d]\n"),
                dpll_index, enable_ho));
    /* Enable holdover for all outputs corresponding to the tdpll instance */
    for (outClk_index = 0; outClk_index < BCM_TDPLL_OUTPUT_CLOCK_NUM_MAX; outClk_index++) {
        if(DPLL(dpll_index).bindings.output_clocks[0] & (1 << outClk_index))
            _bcm_common_tdpll_output_holdover_set(outClk_index, enable_ho);
    }
}

void
_bcm_common_tdpll_dpll_pbo_set(uint16 input_clk_idx)
{
    int dpll_index,outClk_index;
    for (dpll_index = 0; dpll_index < TDPLL_DPLL_INSTANCE_NUM_MAX; dpll_index++) {
        if (DPLL(dpll_index).bindings.input_clocks[0] & (1 << input_clk_idx)) {
            /* Current instance has this input. Enable holdover*/
            for (outClk_index = 0; outClk_index < BCM_TDPLL_OUTPUT_CLOCK_NUM_MAX; outClk_index++) {
                if ((outClk_index != BCM_TDPLL_OUTPUT_CLOCK_1588) &&
                    (DPLL(dpll_index).bindings.output_clocks[0] & (1 << outClk_index))) {
                    _bcm_common_tdpll_output_pbo_set(outClk_index);
                }
            }
        }
    }
}

void
_bcm_common_tdpll_dpll_reference_lock(int unit, int stack_id, int out_clk_idx, int lock) {
    int dpll_index, in_clk_idx;
    for (dpll_index = 0; dpll_index < TDPLL_DPLL_INSTANCE_NUM_MAX; dpll_index++) {
        if (DPLL(dpll_index).bindings.output_clocks[0] & (1 << out_clk_idx)) {
            for (in_clk_idx = 0; in_clk_idx < BCM_TDPLL_INPUT_CLOCK_NUM_GPIO; in_clk_idx++) {
                if (DPLL(dpll_index).bindings.input_clocks[0] & (1 << in_clk_idx)) {
                    LOG_INFO(BSL_LS_BCM_PTP,
                            (BSL_META_U(unit, "DPLL[%d] Lock[%d] InpClk[%d]\n"),
                             dpll_index, lock, in_clk_idx));
                    bcm_tdpll_input_clock_lockout_set(unit, stack_id, in_clk_idx, lock);
                }
            }
        }
    }
}

int
_bcm_common_tdpll_dpll_handle_1pps_event(int unit, int stack_id, int pps_in_state) {
    int output_clk_idx = BCM_TDPLL_OUTPUT_CLOCK_1588;
    int rv = BCM_E_NONE;

    switch(pps_in_state) {
        case _bcm_ptp_pps_in_state_missing:
        case _bcm_ptp_pps_in_state_active_missing_tod:
            _bcm_common_tdpll_dpll_reference_lock(unit, stack_id, output_clk_idx, 1);
            break;
        case _bcm_ptp_pps_in_state_valid:
            _bcm_common_tdpll_dpll_reference_lock(unit, stack_id, output_clk_idx, 0);
            break;
        default :
            break;
    }
    return rv;
}
#endif /*defined (BCM_MONTEREY_SUPPORT) && defined (INCLUDE_GDPLL)*/

#endif /* defined(INCLUDE_PTP) */
