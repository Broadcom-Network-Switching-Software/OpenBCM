/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tdpll_esmc.c
 *
 * Purpose: Telecom DPLL Ethernet Synchronization Messaging Channel (ESMC) support.
 *
 * Functions:
 *      bcm_tdpll_esmc_init
 *      bcm_tdpll_esmc_rx_state_machine
 *      bcm_tdpll_esmc_holdover_event_send
 *      bcm_tdpll_esmc_switch_event_send
 *      bcm_tdpll_esmc_ql_get
 *      bcm_tdpll_esmc_ql_set
 *      bcm_tdpll_esmc_holdover_ql_get
 *      bcm_tdpll_esmc_holdover_ql_set
 *      bcm_tdpll_esmc_mac_get
 *      bcm_tdpll_esmc_mac_set
 *      bcm_tdpll_esmc_rx_enable_get
 *      bcm_tdpll_esmc_rx_enable_set
 *      bcm_tdpll_esmc_tx_enable_get
 *      bcm_tdpll_esmc_tx_enable_set
 *      bcm_tdpll_esmc_rx_portbitmap_get
 *      bcm_tdpll_esmc_rx_portbitmap_set
 *      bcm_tdpll_esmc_tx_portbitmap_get
 *      bcm_tdpll_esmc_tx_portbitmap_set
 *
 *      bcm_tdpll_esmc_info_pdu_tx
 *      bcm_tdpll_esmc_1588_clock_ql_get
 */

#if defined(INCLUDE_PTP)

#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/ptp_common.h>
#include <bcm/error.h>
#include <shared/rdpc.h>

#ifdef BCM_ESMC_EXTDPLL_SUPPORT
#include <bcm_int/common/esmc.h>
#else
/* Definitions. */
#define TDPLL_ESMC_NETWORK_OPTION_DEFAULT (bcm_esmc_network_option_g781_II)
#define TDPLL_ESMC_QL_DEFAULT             (bcm_esmc_g781_II_ql_smc)
#define TDPLL_ESMC_SSM_CODE_DEFAULT       (0x0c)

#define TDPLL_ESMC_RX_ENABLE_DEFAULT                (0)
#define TDPLL_ESMC_TX_ENABLE_DEFAULT                (0)

#define TDPLL_ESMC_INFO_PDU_DPC_TIME_USEC_DEFAULT   (1000000/TDPLL_DPLL_INSTANCE_NUM_MAX)
#define TDPLL_ESMC_INFO_PDU_DPC_TIME_USEC_IDLE      (10000000)

/* Macros. */

/* Types. */

/* Constants and variables. */
static bcm_tdpll_esmc_data_t objdata;

static shr_rdpc_t tdpll_tx_rdpc;
/* Static functions. */
static sal_usecs_t bcm_tdpll_esmc_info_pdu_tx(
    void **arg_unit, void **arg_stack_id,
    void **arg_dpll_index, void **unused);

static int bcm_tdpll_esmc_1588_clock_ql_get(
    int unit, bcm_ptp_stack_id_t ptp_id,
    bcm_esmc_network_option_t network_option,
    bcm_esmc_quality_level_t *ql);

#endif /* BCM_ESMC_EXTDPLL_SUPPORT */

/*
 * Function:
 *      bcm_tdpll_esmc_init()
 * Purpose:
 *      Initialize T-DPLL Ethernet Synchronization Messaging Channel (ESMC).
 * Parameters:
 *      unit      - (IN) Unit number.
 *      stack_id  - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_esmc_init(
    int unit,
    int stack_id)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_init(unit, stack_id);
#else
    int i;

    objdata.rx_enable = TDPLL_ESMC_RX_ENABLE_DEFAULT ? 1:0;
    for (i = 0; i < TDPLL_DPLL_INSTANCE_NUM_MAX; ++i) {
        objdata.tx_enable[i] = TDPLL_ESMC_TX_ENABLE_DEFAULT ? 1:0;
        SOC_PBMP_CLEAR(objdata.tx_pbmp[i]);
        SOC_PBMP_CLEAR(objdata.rx_pbmp[i]);
    }

    objdata.network_option = TDPLL_ESMC_NETWORK_OPTION_DEFAULT;
    for (i = 0; i < TDPLL_DPLL_INSTANCE_NUM_MAX; ++i) {
        objdata.pdu_data[i].ql = TDPLL_ESMC_QL_DEFAULT;
        objdata.holdover_ql[i] = TDPLL_ESMC_QL_DEFAULT;
    }

    for (i = 0; i < TDPLL_DPLL_INSTANCE_NUM_MAX; ++i) {
        objdata.pdu_data[i].ssm_code = TDPLL_ESMC_SSM_CODE_DEFAULT;
        objdata.holdover_ssm_code[i] = TDPLL_ESMC_SSM_CODE_DEFAULT;
    }

    for (i = 0; i < TDPLL_DPLL_INSTANCE_NUM_MAX; ++i) {
        objdata.pdu_data[i].essm_code = bcmEsmcEssmCodeNone;
        objdata.pdu_data[i].essm_flag = 0x0;
        objdata.pdu_data[i].essm_eec_count = 0x0;
        objdata.pdu_data[i].essm_eeec_count= 0x0;
    }


    if (shr_rdpc_callback_created(&tdpll_tx_rdpc) == BCM_E_INIT) {
        /* This lock is left in place on cleanup, so only create it once */
        shr_rdpc_callback_create(&tdpll_tx_rdpc, bcm_tdpll_esmc_info_pdu_tx);
    }

    /* QL Change callback. */
    objdata.ql_change_callback = NULL;

    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_cleanup()
 * Purpose:
 *      Uninitialize T-DPLL Ethernet Synchronization Messaging Channel (ESMC).
 * Parameters:
 *      unit      - (IN) Unit number.
 *      stack_id  - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_esmc_cleanup(
    int unit,
    int stack_id)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_cleanup(unit, stack_id);
#else
    int i;

    objdata.rx_enable = 0;
    for (i = 0; i < TDPLL_DPLL_INSTANCE_NUM_MAX; ++i) {
        objdata.tx_enable[i] = 0;
    }

    shr_rdpc_callback_stop(&tdpll_tx_rdpc);

    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

#ifndef BCM_ESMC_EXTDPLL_SUPPORT
static int
bcm_common_tdpll_esmc_handle_rx_essm_code(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_pdu_data_t *esmc_pdu_data)
{
    if (objdata.pdu_data[dpll_index].essm_code == bcmEsmcEssmCodeNone) {
        return BCM_E_NONE;
    }
    /* incoming pdu's ESSM code is not copied */
    objdata.pdu_data[dpll_index].essm_flag = esmc_pdu_data->essm_flag;
    objdata.pdu_data[dpll_index].essm_eec_count = esmc_pdu_data->essm_eec_count;
    objdata.pdu_data[dpll_index].essm_eeec_count = esmc_pdu_data->essm_eeec_count;

    switch (objdata.pdu_data[dpll_index].essm_code) {
        case bcmEsmcEssmCodeEEC:
            objdata.pdu_data[dpll_index].essm_eec_count++;
            break;
        case bcmEsmcEssmCodeEEEC:
            objdata.pdu_data[dpll_index].essm_eeec_count++;
        default:
            break;
    }
    return BCM_E_NONE;
}
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */

/*
 * Function:
 *      bcm_tdpll_esmc_rx_state_machine()
 * Purpose:
 *      Execute state machine for Rx ESMC PDU.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      stack_id      - (IN) Stack identifier index.
 *      ingress_port  - (IN) Ingress port.
 *      esmc_pdu_data - (IN) ESMC PDU.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      State machine updates the SSM/QL of a T-DPLL input clock, transmits an
 *      ESMC event PDU upon change to SSM/QL of selected reference clock, etc.
 */
int
bcm_common_tdpll_esmc_rx_state_machine(
    int unit,
    int stack_id,
    int ingress_port,
    bcm_esmc_pdu_data_t *esmc_pdu_data)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_rx_state_machine(unit, stack_id, ingress_port, esmc_pdu_data);
#else
    int rv;

    int dpll_index = 0;
    int input_clock_index;
    int reference_index;

    bcm_esmc_quality_level_t input_clock_ql;
    bcm_esmc_quality_level_t esmc_pdu_ql;
    bcm_tdpll_input_clock_ql_change_cb_data_t cb_data;

    bcm_esmc_pdu_data_t pdu_data;

    /* Update networking option. */
    bcm_esmc_g781_option_get(unit, stack_id, &objdata.network_option);

    /* Look up ESMC PDU source in input clock set. */
    if (BCM_SUCCESS(rv = bcm_tdpll_input_clock_port_lookup(unit, stack_id,
            ingress_port, &input_clock_index))) {
        /* Update input clock MAC address. ESMC PDU from a known SyncE input clock. */
        bcm_tdpll_input_clock_mac_set(unit, stack_id, input_clock_index,
            &esmc_pdu_data->source_mac);
    } else if (BCM_FAILURE(rv = bcm_tdpll_input_clock_mac_lookup(unit, stack_id,
            &esmc_pdu_data->source_mac, &input_clock_index))) {
        /* Do nothing. ESMC PDU not from a known source / input clock. */
        return BCM_E_NONE;
    }

    /* Input clock SSM/QL change. */
    if (BCM_FAILURE(rv = bcm_tdpll_input_clock_ql_get(unit, stack_id,
            input_clock_index, &input_clock_ql))) {
        PTP_ERROR_FUNC("bcm_tdpll_input_clock_ql_get()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_esmc_SSM_QL_map(unit, objdata.network_option,
            esmc_pdu_data->ssm_code, &esmc_pdu_ql))) {
        PTP_ERROR_FUNC("bcm_esmc_SSM_QL_map()");
        return rv;
    }

    if (input_clock_ql != esmc_pdu_ql) {
        if (BCM_FAILURE(rv = bcm_tdpll_input_clock_ql_set(unit, stack_id,
                input_clock_index, esmc_pdu_ql))) {
                PTP_ERROR_FUNC("bcm_tdpll_input_clock_ql_set()");
            return rv;
        }
        cb_data.input_clk_index = input_clock_index;
        cb_data.prior_ql = input_clock_ql;
        cb_data.current_ql = esmc_pdu_ql;
        if (objdata.ql_change_callback) {
            objdata.ql_change_callback(unit, stack_id, &cb_data);
        }
    }

    /* Selected reference clock SSM/QL change. */
    for (dpll_index = 0; dpll_index < TDPLL_DPLL_INSTANCE_NUM_MAX; ++dpll_index) {
        if ((0 == objdata.tx_enable[dpll_index]) ||
                    SOC_PBMP_IS_NULL(objdata.tx_pbmp[dpll_index])) {
            /* ESMC Tx not enabled for DPLL. */
            continue;
        }


        if (BCM_FAILURE(rv = bcm_tdpll_input_clock_dpll_reference_get(unit, stack_id,
                dpll_index, &reference_index))) {
            /*
             * Do nothing. T-DPLL reference clock lookup failure.
             * Holdover logic shall initiate update to local node ESMC attributes and issue
             * an ESMC event PDU upon entry to holdover.
             */
            continue;
        }

        if (reference_index != input_clock_index) {
            /* Do nothing. ESMC PDU is not from T-DPLL current / active reference clock. */
            continue;
        }

        bcm_common_tdpll_esmc_handle_rx_essm_code(unit, stack_id, dpll_index, esmc_pdu_data);

        if (input_clock_ql == esmc_pdu_ql) {
            /* Do nothing. ESMC PDU from known source / input clock but SSM/QL is unchanged. */
            continue;
        }


        if (objdata.pdu_data[dpll_index].ssm_code != esmc_pdu_data->ssm_code) {
            /* Update local node ESMC attributes. */
            objdata.pdu_data[dpll_index].ssm_code = esmc_pdu_data->ssm_code;
            if (BCM_FAILURE(rv = bcm_esmc_SSM_QL_map(unit, objdata.network_option,
                    objdata.pdu_data[dpll_index].ssm_code, &objdata.pdu_data[dpll_index].ql))) {
                PTP_ERROR_FUNC("bcm_esmc_SSM_QL_map()");
                continue;
            }

            /* Transmit ESMC event PDU. */
            pdu_data = objdata.pdu_data[dpll_index];
            pdu_data.pdu_type = bcm_esmc_pdu_type_event;
            if (BCM_FAILURE(rv = bcm_esmc_tx(unit, stack_id, objdata.tx_pbmp[dpll_index],
                    &pdu_data))) {
                PTP_ERROR_FUNC("bcm_esmc_tx()");
            }
        }
    }

    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_holdover_event_send()
 * Purpose:
 *      Update ESMC and issue event PDU as result of holdover entry.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_esmc_holdover_event_send(
    int unit,
    int stack_id,
    int dpll_index)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return BCM_E_UNAVAIL;
#else
    int rv;
    bcm_esmc_pdu_data_t pdu_data;

    /* Argument checking and error handling. */
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    if (objdata.pdu_data[dpll_index].ql == objdata.holdover_ql[dpll_index]) {
        /* Do nothing. ESMC QL is unchanged. */
        return BCM_E_NONE;
    }

    objdata.pdu_data[dpll_index].ql = objdata.holdover_ql[dpll_index];
    objdata.pdu_data[dpll_index].ssm_code = objdata.holdover_ssm_code[dpll_index];

    if (objdata.tx_enable[dpll_index]
        && SOC_PBMP_NOT_NULL(objdata.tx_pbmp[dpll_index])) {
        /* Transmit ESMC event PDU. */
        pdu_data = objdata.pdu_data[dpll_index];
        pdu_data.pdu_type = bcm_esmc_pdu_type_event;
        if (BCM_FAILURE(rv = bcm_esmc_tx(unit, stack_id, objdata.tx_pbmp[dpll_index],
                &pdu_data))) {
            PTP_ERROR_FUNC("bcm_esmc_tx()");
        }
    }

    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_switch_event_send()
 * Purpose:
 *      Update ESMC and issue event PDU as result of reference switch.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      ql         - (IN) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_tdpll_esmc_switch_event_send(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t ql)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return BCM_E_UNAVAIL;
#else
    int rv;
    bcm_esmc_pdu_data_t pdu_data;

    /* Argument checking and error handling. */
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    if (ql == objdata.pdu_data[dpll_index].ql) {
        /* Do nothing. ESMC QL is unchanged. */
        return BCM_E_NONE;
    }

    /* Update networking option. */
    bcm_esmc_g781_option_get(unit, stack_id, &objdata.network_option);

    objdata.pdu_data[dpll_index].ql = ql;
    if (BCM_FAILURE(rv = bcm_esmc_QL_SSM_map(unit, objdata.network_option,
            objdata.pdu_data[dpll_index].ql, &objdata.pdu_data[dpll_index].ssm_code))) {
        PTP_ERROR_FUNC("bcm_esmc_QL_SSM_map");
        return rv;
    }

    if (objdata.tx_enable[dpll_index]
        && SOC_PBMP_NOT_NULL(objdata.tx_pbmp[dpll_index])) {
        /* Transmit ESMC event PDU. */
        pdu_data = objdata.pdu_data[dpll_index];
        pdu_data.pdu_type = bcm_esmc_pdu_type_event;
        if (BCM_FAILURE(rv = bcm_esmc_tx(unit, stack_id, objdata.tx_pbmp[dpll_index],
                &pdu_data))) {
            PTP_ERROR_FUNC("bcm_esmc_tx()");
        }
    }

    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_ql_get()
 * Purpose:
 *      Get quality level (QL) for ESMC.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      ql         - (OUT) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_esmc_ql_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t *ql)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
     return bcm_extdpll_esmc_ql_get(unit, stack_id, ql);
#else
    /* Argument checking and error handling. */
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    *ql = objdata.pdu_data[dpll_index].ql;
    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_ql_set()
 * Purpose:
 *      Set quality level (QL) for ESMC.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      ql         - (IN) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Assignment is transient. QL shall be overwritten by subsequent
 *      ESMC PDUs received from a T-DPLL selected reference clock.
 */
int
bcm_common_tdpll_esmc_ql_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t ql)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
     return bcm_extdpll_esmc_ql_set(unit, stack_id, dpll_index, ql);
#else
    int rv;

    /* Argument checking and error handling. */
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    /* Update networking option. */
    bcm_esmc_g781_option_get(unit, stack_id, &objdata.network_option);

    objdata.pdu_data[dpll_index].ql = ql;
    if (BCM_FAILURE(rv = bcm_esmc_QL_SSM_map(unit, objdata.network_option,
            objdata.pdu_data[dpll_index].ql, &objdata.pdu_data[dpll_index].ssm_code))) {
        PTP_ERROR_FUNC("bcm_esmc_QL_SSM_map");
        return rv;
    }

    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_holdover_ql_get()
 * Purpose:
 *      Get quality level (QL) for ESMC during holdover.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      ql         - (OUT) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_esmc_holdover_ql_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t *ql)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return BCM_E_UNAVAIL;
#else
    /* Argument checking and error handling. */
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    *ql = objdata.holdover_ql[dpll_index];
    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_holdover_ql_set()
 * Purpose:
 *      Set quality level (QL) for ESMC during holdover.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      ql         - (IN) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_esmc_holdover_ql_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t ql)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return BCM_E_UNAVAIL;
#else
    int rv;

    /* Argument checking and error handling. */
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    /* Update networking option. */
    bcm_esmc_g781_option_get(unit, stack_id, &objdata.network_option);

    objdata.holdover_ql[dpll_index] = ql;
    if (BCM_FAILURE(rv = bcm_esmc_QL_SSM_map(unit, objdata.network_option,
            objdata.holdover_ql[dpll_index], &objdata.holdover_ssm_code[dpll_index]))) {
        PTP_ERROR_FUNC("bcm_esmc_QL_SSM_map");
        return rv;
    }

    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_mac_get()
 * Purpose:
 *      Get MAC address for ESMC.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      mac        - (OUT) Local port MAC address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      MAC address is used as source MAC in Tx'd ESMC info and event PDUs.
 *      MAC address is used in discard-from-self logic for reflected PDUs.
 */
int
bcm_common_tdpll_esmc_mac_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_mac_t *mac)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_mac_get(unit, stack_id, mac);
#else
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_PAYLOAD_ESMC_MAC_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_ESMC_MAC_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    /* Argument checking and error handling. */
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

    /* Make indexed payload to get MAC address for specified DPLL instance. */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    payload[6] = (uint8)dpll_index;
    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_ESMC_MAC,
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
     *    Octet 7      : Reserved.
     *    Octet 8...13 : ESMC MAC address.
     */
    sal_memcpy(mac, resp + 8, sizeof(bcm_mac_t));

    /* Set host-maintained ESMC MAC address. */
    sal_memcpy(objdata.pdu_data[dpll_index].source_mac, resp + 8, sizeof(bcm_mac_t));

    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_mac_set()
 * Purpose:
 *      Set MAC address for ESMC.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      mac        - (IN) Local port MAC address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      MAC address is used as source MAC in Tx'd ESMC info and event PDUs.
 *      MAC address is used in discard-from-self logic for reflected PDUs.
 */
int
bcm_common_tdpll_esmc_mac_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_mac_t *mac)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_mac_set(unit, stack_id, mac);
#else
    int rv;
    int i;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_ESMC_MAC_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    /* Argument checking and error handling. */
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
     *    Octet 7      : Reserved.
     *    Octet 8...13 : ESMC MAC address.
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i++] = (uint8)dpll_index;
    payload[i++] = 0;
    sal_memcpy(payload + i, mac, sizeof(bcm_mac_t));

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_ESMC_MAC,
            payload, PTP_MGMTMSG_PAYLOAD_ESMC_MAC_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /* Set host-maintained ESMC MAC address. */
    sal_memcpy(objdata.pdu_data[dpll_index].source_mac, mac, sizeof(bcm_mac_t));

    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_rx_enable_get()
 * Purpose:
 *      Get ESMC PDU receive (Rx) enable state.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 *      enable   - (OUT) ESMC PDU receive enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_esmc_rx_enable_get(
    int unit,
    int stack_id,
    int *enable)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_rx_enable_get(unit, stack_id, enable);
#else
    int rv;

    rv = bcm_esmc_tunnel_get(unit, stack_id, enable);
    objdata.rx_enable = *enable ? 1:0;

    return rv;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_rx_enable_set()
 * Purpose:
 *      Set ESMC PDU receive (Rx) enable state.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 *      enable   - (IN) ESMC PDU receive enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_esmc_rx_enable_set(
    int unit,
    int stack_id,
    int enable)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_rx_enable_set(unit, stack_id, enable);
#else
    int rv;

    rv = bcm_esmc_tunnel_set(unit, stack_id, enable);
    objdata.rx_enable = enable ? 1:0;

    return rv;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_tx_enable_get()
 * Purpose:
 *      Get ESMC PDU transmit (Tx) enable state.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      enable     - (OUT) ESMC PDU transmit enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_esmc_tx_enable_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *enable)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_tx_enable_get(unit, stack_id, enable);
#else
    /* Argument checking and error handling. */
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    *enable = objdata.tx_enable[dpll_index] ? 1:0;
    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_tx_enable_set()
 * Purpose:
 *      Set ESMC PDU transmit (Tx) enable state.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      enable     - (IN) ESMC PDU transmit enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_esmc_tx_enable_set(
    int unit,
    int stack_id,
    int dpll_index,
    int enable)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_tx_enable_set(unit, stack_id, enable);
#else
    int i;
    int tx_esmc;

    /* Argument checking and error handling. */
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    objdata.tx_enable[dpll_index] = enable ? 1:0;

    /*
     * Conditionally start DPC for periodic ESMC information PDUs.
     * NB: Cancel prior ESMC information DPC process (if any).
     */
    tx_esmc = 0;
    for (i = 0; i < TDPLL_DPLL_INSTANCE_NUM_MAX; ++i) {
        if (objdata.tx_enable[i]) {
            tx_esmc = 1;
            break;
        }
    }

    if (tx_esmc) {
        shr_rdpc_callback_start(&tdpll_tx_rdpc, TDPLL_ESMC_INFO_PDU_DPC_TIME_USEC_DEFAULT,
                                INT_TO_PTR(unit), INT_TO_PTR(stack_id), 0, 0);
    } else {
        shr_rdpc_callback_stop(&tdpll_tx_rdpc);
    }

    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_rx_portbitmap_get()
 * Purpose:
 *      Get port bitmap for ESMC Rx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      pbmp       - (OUT) Rx port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_esmc_rx_portbitmap_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t *pbmp)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_rx_portbitmap_get(unit, stack_id, pbmp);
#else
    *pbmp = objdata.rx_pbmp[dpll_index];
    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_rx_portbitmap_set()
 * Purpose:
 *      Set port bitmap for ESMC Rx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      pbmp       - (IN) Rx port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_esmc_rx_portbitmap_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t pbmp)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_rx_portbitmap_set(unit, stack_id, pbmp);
#else
    objdata.rx_pbmp[dpll_index] = pbmp;
    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_tx_portbitmap_get()
 * Purpose:
 *      Get port bitmap for ESMC Tx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      pbmp       - (OUT) Tx port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_esmc_tx_portbitmap_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t *pbmp)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_tx_portbitmap_get(unit, stack_id, pbmp);
#else
    *pbmp = objdata.tx_pbmp[dpll_index];
    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_tdpll_esmc_tx_portbitmap_set()
 * Purpose:
 *      Set port bitmap for ESMC Tx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      pbmp       - (IN) Tx port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_esmc_tx_portbitmap_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t pbmp)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_tx_portbitmap_set(unit, stack_id, pbmp);
#else
    objdata.tx_pbmp[dpll_index] = pbmp;
    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

#ifndef BCM_ESMC_EXTDPLL_SUPPORT
/*
 * Function:
 *      bcm_tdpll_esmc_1588_clock_ql_get
 * Purpose:
 *      Gets clock quality of a PTP clock default dataset.
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      ptp_id    - (IN)  PTP stack ID.
 *      clock_quality - (OUT) PTP clock quality
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tdpll_esmc_1588_clock_ql_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    bcm_esmc_network_option_t network_option,
    bcm_esmc_quality_level_t *ql)
{
    int rv = BCM_E_PARAM;
    bcm_ptp_parent_dataset_t par_data_set;

    if (BCM_FAILURE(rv = bcm_ptp_clock_parent_dataset_get(unit, ptp_id, 0, &par_data_set))) {
        return rv;
    }
    *ql = bcm_esmc_ql_unresolvable;
    switch (par_data_set.grandmaster_clock_quality.clock_class)
    {
        case 6:
        case 7:
        case 135:
        case 140:
            if (network_option == bcm_esmc_network_option_g781_I)
                *ql = bcm_esmc_g781_I_ql_prc;
            else if (network_option == bcm_esmc_network_option_g781_II)
                *ql = bcm_esmc_g781_II_ql_prs;
            break;
        case 150:
            if (network_option == bcm_esmc_network_option_g781_I)
                *ql = bcm_esmc_g781_I_ql_ssua;
            else if (network_option == bcm_esmc_network_option_g781_II)
                *ql = bcm_esmc_g781_II_ql_st2;
            break;
        case 160:
            if (network_option == bcm_esmc_network_option_g781_I)
                *ql = bcm_esmc_g781_I_ql_ssub;
            else if (network_option == bcm_esmc_network_option_g781_II)
                *ql = bcm_esmc_g781_II_ql_st3e;
            break;
        case 165:
        case 248:
        case 255:
        default:
            if (network_option == bcm_esmc_network_option_g781_I)
                *ql = bcm_esmc_g781_I_ql_sec;
            else if (network_option == bcm_esmc_network_option_g781_II)
                *ql = bcm_esmc_g781_II_ql_st3;
            break;
    }

    return rv;
}


/*
 * Function:
 *      bcm_tdpll_esmc_info_pdu_tx()
 * Purpose:
 *      ESMC information PDU (as DPC).
 * Parameters:
 *      arg_unit       - (IN) Pointer to Unit number (as void*).
 *      arg_stack_id   - (IN) Pointer to Stack identifier index (as void*).
 *      arg_dpll_index - (IN) Pointer to DPLL instance number (as void*).
 *      unused         - (IN) Unused.
 * Returns:
 *      Time interval before next call
 * Notes:
 */
static sal_usecs_t
bcm_tdpll_esmc_info_pdu_tx(
    void **arg_unit,
    void **arg_stack_id,
    void **arg_dpll_index,
    void **unused)
{
    int rv;
    int unit = PTR_TO_INT(*arg_unit);
    int stack_id = PTR_TO_INT(*arg_stack_id);
    bcm_esmc_pdu_data_t pdu_data;
    size_t *dpll_index = (size_t*)arg_dpll_index; /* use as ptr so value can be updated for next call */
    bcm_esmc_quality_level_t ql = bcm_esmc_ql_unresolvable;
    bcm_ptp_clock_info_t *clock_info;
    clock_info = &_bcm_common_ptp_unit_array[0].stack_array[0].clock_array[0].clock_info;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return TDPLL_ESMC_INFO_PDU_DPC_TIME_USEC_IDLE;
    }
    if (objdata.tx_enable[*dpll_index]
        && SOC_PBMP_NOT_NULL(objdata.tx_pbmp[*dpll_index])) {
        if (PTP_CLOCK_IS_G8275P2_PROF(clock_info)) {
            if (BCM_FAILURE(rv = bcm_tdpll_esmc_1588_clock_ql_get(unit,stack_id,objdata.network_option,&ql))) {
                PTP_ERROR_FUNC("bcm_tdpll_esmc_1588_clock_ql_get");
            } else {
                if (objdata.pdu_data[*dpll_index].ql != ql)
                {
                    bcm_tdpll_esmc_switch_event_send(unit, stack_id, *dpll_index, ql);
                } else {
                    /* Transmit ESMC information PDU. */
                    objdata.pdu_data[*dpll_index].ql = ql;
                    if (BCM_FAILURE(rv = bcm_esmc_QL_SSM_map(unit, objdata.network_option,
                        objdata.pdu_data[*dpll_index].ql, &objdata.pdu_data[*dpll_index].ssm_code))) {
                        PTP_ERROR_FUNC("bcm_esmc_QL_SSM_map");
                    }
                }
            }
        }
        /* Transmit ESMC information PDU. */
        pdu_data = objdata.pdu_data[*dpll_index];
        pdu_data.pdu_type = bcm_esmc_pdu_type_info;

        if (BCM_FAILURE(rv = bcm_esmc_tx(unit, stack_id, objdata.tx_pbmp[*dpll_index],
                                        &pdu_data))) {
            PTP_ERROR_FUNC("bcm_esmc_tx()");
        }
    }

    ++(*dpll_index);
    (*dpll_index) %= TDPLL_DPLL_INSTANCE_NUM_MAX;

    return TDPLL_ESMC_INFO_PDU_DPC_TIME_USEC_DEFAULT;
}
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */

/*
 * Function:
 *      bcm_common_tdpll_input_clock_ql_change_callback_register()
 * Purpose:
 *      Register input clock esmc quality change callback.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      ql_change_cb - (IN) Input clock esmc QL change callback function pointer.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      QL change callback generates an event to notify user if a
 *      QL level change happens to take necessary action if required.
 */
int
bcm_common_tdpll_input_clock_ql_change_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_ql_change_cb ql_change_cb)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return BCM_E_UNAVAIL; 
#else
    objdata.ql_change_callback = ql_change_cb;
    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_common_tdpll_input_clock_ql_change_callback_unregister()
 * Purpose:
 *      Un-register input clock esmc quality change callback.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      QL change callback generates an event to notify user if a
 *      QL level change happens to take necessary action if required.
 */

int
bcm_common_tdpll_input_clock_ql_change_callback_unregister(
    int unit,
    int stack_id)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return BCM_E_UNAVAIL; 
#else
    objdata.ql_change_callback = NULL;
    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_common_tdpll_esmc_essm_code_get()
 * Purpose:
 *      Get synce clock type for ESMC.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      essm_code - (OUT) essm code as per G.8264 amd2.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_tdpll_esmc_essm_code_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_essm_code_t *essm_code)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_essm_code_get(unit, stack_id, essm_code);
#else
    /* Argument checking and error handling. */
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }

    *essm_code= objdata.pdu_data[dpll_index].essm_code;
    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_common_tdpll_esmc_essm_code_set()
 * Purpose:
 *      Get synce clock type for ESMC.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      dpll_index - (IN) DPLL instance number.
 *      essm_code - (IN) essm oode type as per G.8264 amd 2.
 * Returns:
 *      BCM_E_XXX - Function status.
 */
int
bcm_common_tdpll_esmc_essm_code_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_essm_code_t essm_code)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_esmc_essm_code_set(unit, stack_id, essm_code);
#else

    /* Argument checking and error handling. */
    if (dpll_index < 0 || dpll_index >= TDPLL_DPLL_INSTANCE_NUM_MAX) {
        return BCM_E_PARAM;
    }


    objdata.pdu_data[dpll_index].essm_code = essm_code;

    switch (essm_code) {
        case bcmEsmcEssmCodeEEC:
           objdata.pdu_data[dpll_index].essm_eec_count = 1;
           break;
        default:
            break;
    }        

    return BCM_E_NONE;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_common_tdpll_event_callback_register()
 * Purpose:
 *      Register the event callback handler with ESMC stack.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      callback   - (IN) callback handler func.
 * Returns:
 *      BCM_E_XXX - Function status.
 */
int
bcm_common_tdpll_event_callback_register(
    int unit, 
    int stack_id, 
    bcm_tdpll_event_cb_f callback)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_event_callback_register(unit, stack_id, callback);
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}

/*
 * Function:
 *      bcm_common_tdpll_event_callback_unregister()
 * Purpose:
 *      Unregister the event callback handler with ESMC stack.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 */
int
bcm_common_tdpll_event_callback_unregister(
    int unit, 
    int stack_id)
{
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    return bcm_extdpll_event_callback_unregister(unit, stack_id);
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
}



#endif /* defined(INCLUDE_PTP) */
