/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: esmc.c
 *
 * Purpose: Support ESMC with external DPLL.
 *
 * Functions:
 *      bcm_extdpll_esmc_init
 *      bcm_extdpll_esmc_rx_state_machine
 *      bcm_extdpll_esmc_switch_event_send
 *      bcm_extdpll_esmc_ql_get
 *      bcm_extdpll_esmc_ql_set
 *      bcm_extdpll_esmc_mac_get
 *      bcm_extdpll_esmc_mac_set
 *      bcm_extdpll_esmc_rx_enable_get
 *      bcm_extdpll_esmc_rx_enable_set
 *      bcm_extdpll_esmc_tx_enable_get
 *      bcm_extdpll_esmc_tx_enable_set
 *      bcm_extdpll_esmc_rx_portbitmap_get
 *      bcm_extdpll_esmc_rx_portbitmap_set
 *      bcm_extdpll_esmc_tx_portbitmap_get
 *      bcm_extdpll_esmc_tx_portbitmap_set
 *
 *      bcm_extdpll_esmc_info_pdu_tx
 */

#if defined(INCLUDE_PTP)

#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/common/esmc.h>
#include <bcm_int/ptp_common.h>
#include <bcm/error.h>
#include <bcm/link.h>
#include <shared/util.h>

/* Definitions. */
#define ESMC_NETWORK_OPTION_DEFAULT (bcm_esmc_network_option_g781_II)
#define ESMC_QL_DEFAULT             (bcm_esmc_g781_II_ql_smc)
#define ESMC_SSM_CODE_DEFAULT       (0x0f)

#define ESMC_RX_ENABLE_DEFAULT                (0)
#define ESMC_TX_ENABLE_DEFAULT                (0)

#define ESMC_INFO_PDU_DPC_TIME_USEC_DEFAULT   (1000000)
#define ESMC_INFO_PDU_DPC_TIME_USEC_IDLE      (10000000)
#define ESMC_FAILURE_TIMEOUT_SEC              (5)

/* Constants and variables. */
static bcm_extdpll_esmc_data_t objdata;
static shr_rdpc_t extdpll_esmc_tx_rdpc;
static shr_rdpc_t extdpll_esmc_rx_rdpc;

/* Static functions. */
static sal_usecs_t bcm_extdpll_esmc_info_pdu_tx(void **arg_unit, void **arg_stack_id, void **unused1, void **unused2);

static sal_usecs_t bcm_extdpll_esmc_info_pdu_rx(void **arg_unit, void **arg_stack_id, void **unused1, void **unused2);

static int bcm_extdpll_squelch_cdr_mux_enable(int unit, bcm_port_t port, int enable);

/*
 * Function:
 *      bcm_extdpll_esmc_init()
 * Purpose:
 *      Initialization logic.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      stack_id  - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_extdpll_esmc_init(
    int unit,
    int stack_id)
{
    int i, rv;

    objdata.rx_enable = ESMC_RX_ENABLE_DEFAULT?1:0;
    objdata.tx_enable = ESMC_TX_ENABLE_DEFAULT?1:0;

    BCM_PBMP_CLEAR(objdata.tx_pbmp);
    BCM_PBMP_CLEAR(objdata.rx_pbmp);

    //objdata.network_option = ESMC_NETWORK_OPTION_DEFAULT;

    objdata.pdu_data.ql = ESMC_QL_DEFAULT;
    objdata.pdu_data.ssm_code = ESMC_SSM_CODE_DEFAULT;
    objdata.pdu_data.essm_code = bcmEsmcEssmCodeNone;
    objdata.pdu_data.essm_flag = 0x0;
    objdata.pdu_data.essm_eec_count = 0x0;
    objdata.pdu_data.essm_eeec_count= 0x0;

				objdata.best_port = -1;
				objdata.port_l1mux_pri = -1;
				objdata.port_l1mux_bkup = -1;

				for (i=0; i< (ESMC_PORT_TABLE_ENTRIES_MAX+1); i++) {
								objdata.esmc_info[i].ssm_code = ESMC_SSM_CODE_DEFAULT;
								objdata.esmc_info[i].flags = 0;
				}

    if (shr_rdpc_callback_created(&extdpll_esmc_tx_rdpc) == BCM_E_INIT) {
        /* create it only once */
        LOG_VERBOSE(BSL_LS_BCM_PTP, 
								            (BSL_META_U(0, " Creating Tx RDPC \n")));
        shr_rdpc_callback_create(&extdpll_esmc_tx_rdpc, bcm_extdpll_esmc_info_pdu_tx);
    }

    if (shr_rdpc_callback_created(&extdpll_esmc_rx_rdpc) == BCM_E_INIT) {
        /* create it only once */
        LOG_VERBOSE(BSL_LS_BCM_PTP, 
								            (BSL_META_U(0, " Creating Rx RDPC \n")));
        shr_rdpc_callback_create(&extdpll_esmc_rx_rdpc, bcm_extdpll_esmc_info_pdu_rx);
    }

    /* Nullify the notification callback handler */
    objdata.esmc_cb_hdlr = NULL;

    /* Register the ESMC handler here */
    if (BCM_FAILURE(rv = bcm_esmc_rx_callback_register(unit, 0,
																                    bcm_extdpll_esmc_rx_state_machine))) {
								PTP_ERROR_FUNC("bcm_esmc_rx_callback_register");
				}

    LOG_VERBOSE(BSL_LS_BCM_PTP,
            (BSL_META_U(0, " bcm_extdpll_esmc_init done \n")));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_cleanup()
 * Purpose:
 *      Uninitialization logic.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      stack_id  - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_extdpll_esmc_cleanup(
    int unit,
    int stack_id)
{
    objdata.rx_enable = 0;
    objdata.tx_enable = 0;

    shr_rdpc_callback_stop(&extdpll_esmc_tx_rdpc);
    shr_rdpc_callback_stop(&extdpll_esmc_rx_rdpc);

    return BCM_E_NONE;
}

static int
bcm_extdpll_esmc_handle_rx_essm_code(
    int unit,
    int stack_id,
    bcm_esmc_pdu_data_t *esmc_pdu_data)
{
    if (objdata.pdu_data.essm_code == bcmEsmcEssmCodeNone) {
        return BCM_E_NONE;
    }
    /* incoming pdu's ESSM code is not copied */
    objdata.pdu_data.essm_flag = esmc_pdu_data->essm_flag;
    objdata.pdu_data.essm_eec_count = esmc_pdu_data->essm_eec_count;
    objdata.pdu_data.essm_eeec_count = esmc_pdu_data->essm_eeec_count;

    switch (objdata.pdu_data.essm_code) {
        case bcmEsmcEssmCodeEEC:
            objdata.pdu_data.essm_eec_count++;
            break;
        case bcmEsmcEssmCodeEEEC:
            objdata.pdu_data.essm_eeec_count++;
        default:
            break;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_rx_state_machine()
 * Purpose:
 *      State machine for handling of ESMC PDU Rx for external DPLL.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      stack_id      - (IN) Stack identifier index.
 *      ingress_port  - (IN) Ingress port.
 *      esmc_pdu_data - (IN) ESMC PDU.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      State machine updates the SSM/QL of each SyncE enabled port, 
	*      notifies the application if it detects that the QL changed.
 */
int
bcm_extdpll_esmc_rx_state_machine(
    int unit,
    int stack_id,
    int ingress_port,
    bcm_esmc_pdu_data_t *esmc_pdu_data)
{
				int rv = BCM_E_NONE;
    bcm_tdpll_ql_change_event_cb_data_t cb_data;
    bcm_esmc_network_option_t network_option;

    /* Update networking option. */
    bcm_esmc_g781_option_get(unit, stack_id, &network_option);

				if (objdata.best_port == ingress_port) {
        bcm_extdpll_esmc_handle_rx_essm_code(unit, stack_id, esmc_pdu_data);
				}

    if ( objdata.esmc_info[ingress_port].ssm_code == esmc_pdu_data->ssm_code) {
        /* Do nothing. ESMC PDU with SSM/QL unchanged. */
        return BCM_E_NONE;
    } else {
        /* Squelch the CDR mux output if the incoming QL is DNU/DUS */
        if ( ((network_option == bcm_esmc_network_option_g781_I) && (esmc_pdu_data->ql == bcm_esmc_g781_I_ql_dnu)) ||
            ((network_option == bcm_esmc_network_option_g781_II) && (esmc_pdu_data->ql == bcm_esmc_g781_II_ql_dus)) ) {
            if (BCM_FAILURE(rv = bcm_extdpll_squelch_cdr_mux_enable(unit, ingress_port, 1))) {
                PTP_ERROR_FUNC("bcm_extdpll_squelch_cdr_mux_enable");
            }
        } else if ( (objdata.esmc_info[ingress_port].flags & (1<<ESMC_FLAG_BIT_SYNCE_TS_MISS_INDICATION)) ||
								            (objdata.esmc_info[ingress_port].flags & (1<<ESMC_FLAG_BIT_PORT_LINK_DOWN_INDICATION)) ||
								            (objdata.esmc_info[ingress_port].flags & (1<<ESMC_FLAG_BIT_ESMC_TIMEOUT_INDICATION)) ||
																				((network_option == bcm_esmc_network_option_g781_I) && (objdata.esmc_info[ingress_port].ssm_code == ESMC_SSM_CODE_DEFAULT)) ||
																				((network_option == bcm_esmc_network_option_g781_II) && (objdata.esmc_info[ingress_port].ssm_code == ESMC_SSM_CODE_DEFAULT)))
				    {
            if (BCM_FAILURE(rv = bcm_extdpll_squelch_cdr_mux_enable(unit, ingress_port, 0))) {
    								    PTP_ERROR_FUNC("bcm_extdpll_squelch_cdr_mux_enable");
    				    }
				        objdata.esmc_info[ingress_port].flags &= ~(1<<ESMC_FLAG_BIT_ESMC_TIMEOUT_INDICATION | 
												                                           1<<ESMC_FLAG_BIT_SYNCE_TS_MISS_INDICATION | 
																																																							1<<ESMC_FLAG_BIT_PORT_LINK_DOWN_INDICATION);
        }
        if (objdata.esmc_cb_hdlr) 
								{
						      cb_data.event_type = bcmTdpllEventCbTypeQlChange; 
								    cb_data.port = ingress_port;
								    cb_data.ql_old = objdata.esmc_info[ingress_port].ssm_code;
							     cb_data.ql_new = esmc_pdu_data->ssm_code;

            objdata.esmc_cb_hdlr(unit, stack_id, (bcm_tdpll_event_cb_data_t *)&cb_data);
        }
								else {
            LOG_VERBOSE(BSL_LS_BCM_PTP,
                (BSL_META_U(0, " RxSM: Callback handler not registered \n")));
        }
						  /* update stored value for the port */
        objdata.esmc_info[ingress_port].ssm_code = esmc_pdu_data->ssm_code;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_switch_event_send
 * Purpose:
 *      Update ESMC and issue event PDU as result of reference switch.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      ql         - (IN) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_extdpll_esmc_switch_event_send(
    int unit,
    int stack_id,
    bcm_esmc_quality_level_t ql)
{
				int rv;
    bcm_esmc_pdu_data_t pdu_data;
    bcm_esmc_network_option_t network_option;

    /* Update networking option. */
    bcm_esmc_g781_option_get(unit, stack_id, &network_option);

    objdata.pdu_data.ql = ql;

    if (BCM_FAILURE(rv = bcm_esmc_QL_SSM_map(unit, network_option,
            objdata.pdu_data.ql, &objdata.pdu_data.ssm_code))) {
        PTP_ERROR_FUNC("bcm_esmc_QL_SSM_map");
        return rv;
    }

    if (objdata.tx_enable && BCM_PBMP_NOT_NULL(objdata.tx_pbmp)) {
        /* Transmit ESMC event PDU. */
        pdu_data = objdata.pdu_data;
        pdu_data.pdu_type = bcm_esmc_pdu_type_event;
        if (BCM_FAILURE(rv = bcm_esmc_tx(unit, stack_id, objdata.tx_pbmp, &pdu_data))) {
            PTP_ERROR_FUNC("bcm_esmc_tx()");
        }
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_extdpll_esmc_ql_get()
 * Purpose:
 *      Get current configured ESMC QL.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      ql         - (OUT) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_extdpll_esmc_ql_get(
    int unit,
    int stack_id,
    bcm_esmc_quality_level_t *ql)
{
    *ql = objdata.pdu_data.ql;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_ql_set()
 * Purpose:
 *      Set ESMC QL
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      best_port  - (IN) Best ESMC Rx port.
 *      ql         - (IN) QL.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Interpret dpll_index as the best ESMC Rx port selected by application.
	*      Reusing the field.
 */
int
bcm_extdpll_esmc_ql_set(
    int unit,
    int stack_id,
    int best_port,
    bcm_esmc_quality_level_t ql)
{
    int rv;

			 if (objdata.pdu_data.ql != ql) {
								/* Tx ESMC-Event PDU with new QL configured */
        objdata.pdu_data.ql = ql;
        if (BCM_FAILURE(rv = bcm_extdpll_esmc_switch_event_send(unit, stack_id, ql))) {
            PTP_ERROR_FUNC("bcm_extdpll_esmc_switch_event_send");
				    }
				}

				objdata.best_port = best_port;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_mac_get()
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
 *      MAC address is used as source MAC in ESMC info/event PDU Tx.
 */
int
bcm_extdpll_esmc_mac_get(
    int unit,
    int stack_id,
    bcm_mac_t *mac)
{
    int rv;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_INDEXED_PROPRIETARY_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_PAYLOAD_ESMC_MAC_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_ESMC_MAC_SIZE_OCTETS;

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

    sal_memcpy(payload, "BCM\0\0\0", 6);
    payload[6] = 0;
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
     *    Octet 6      : DPLL instance number(0).
     *    Octet 7      : Reserved.
     *    Octet 8...13 : ESMC MAC address.
     */
    sal_memcpy(mac, resp + 8, sizeof(bcm_mac_t));

    /* Set host-maintained ESMC MAC address. */
    sal_memcpy(objdata.pdu_data.source_mac, resp + 8, sizeof(bcm_mac_t));

    LOG_VERBOSE(BSL_LS_BCM_PTP,
                  (BSL_META_U(unit, "\n MAC get[%02x:%02x:%02x:%02x:%02x:%02x]\n "),
				      objdata.pdu_data.source_mac[0], objdata.pdu_data.source_mac[1], objdata.pdu_data.source_mac[2], 
										objdata.pdu_data.source_mac[3], objdata.pdu_data.source_mac[4], objdata.pdu_data.source_mac[5]));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_mac_set()
 * Purpose:
 *      Set MAC address for ESMC.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      mac        - (IN) Local port MAC address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      MAC address is used as source MAC in Tx'd ESMC info and event PDUs.
 *      MAC address is used in discard-from-self logic for reflected PDUs.
 */
int
bcm_extdpll_esmc_mac_set(
    int unit,
    int stack_id,
    bcm_mac_t *mac)
{
    int rv;
    int i;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_ESMC_MAC_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

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
    payload[i++] = 0;
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
    sal_memcpy(objdata.pdu_data.source_mac, mac, sizeof(bcm_mac_t));

    LOG_VERBOSE(BSL_LS_BCM_PTP,
                  (BSL_META_U(unit, "\n MAC set[%02x:%02x:%02x:%02x:%02x:%02x]\n "),
				      objdata.pdu_data.source_mac[0], objdata.pdu_data.source_mac[1], objdata.pdu_data.source_mac[2], 
										objdata.pdu_data.source_mac[3], objdata.pdu_data.source_mac[4], objdata.pdu_data.source_mac[5]));


    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_rx_enable_get()
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
bcm_extdpll_esmc_rx_enable_get(
    int unit,
    int stack_id,
    int *enable)
{
    int rv;

    rv = bcm_esmc_tunnel_get(unit, stack_id, enable);
    objdata.rx_enable = *enable ? 1:0;

    return rv;
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
bcm_extdpll_esmc_rx_enable_set(
    int unit,
    int stack_id,
    int enable)
{
    int rv;

    rv = bcm_esmc_tunnel_set(unit, stack_id, enable);
    objdata.rx_enable = enable ? 1:0;
    LOG_VERBOSE(BSL_LS_BCM_PTP,
            (BSL_META_U(0, "esmc rx enable[%d] rv[%d]\n"), enable, rv));

    if (objdata.rx_enable) {
        if (BCM_FAILURE(rv = bcm_extdpll_register_linkscan_callback(unit))) {
            LOG_ERROR(BSL_LS_BCM_PTP,
                (BSL_META_U(0, " Failed to register linkscan handler \n")));
        }
        LOG_VERBOSE(BSL_LS_BCM_PTP, 
								            (BSL_META_U(0, " Starting Rx RDPC \n")));
        shr_rdpc_callback_start(&extdpll_esmc_rx_rdpc, ESMC_INFO_PDU_DPC_TIME_USEC_DEFAULT,
                                INT_TO_PTR(unit), INT_TO_PTR(stack_id), 0, 0);
    } else {
        if (BCM_FAILURE(rv = bcm_extdpll_unregister_linkscan_callback(unit))) {
            LOG_ERROR(BSL_LS_BCM_PTP,
                (BSL_META_U(0, " Failed to unregister linkscan handler \n")));
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_extdpll_esmc_tx_enable_get()
 * Purpose:
 *      Get ESMC PDU transmit (Tx) enable state.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      enable     - (OUT) ESMC PDU transmit enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_extdpll_esmc_tx_enable_get(
    int unit,
    int stack_id,
    int *enable)
{
    *enable = objdata.tx_enable ? 1:0;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_tx_enable_set()
 * Purpose:
 *      Set ESMC PDU transmit (Tx) enable state.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      enable     - (IN) ESMC PDU transmit enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_extdpll_esmc_tx_enable_set(
    int unit,
    int stack_id,
    int enable)
{

    objdata.tx_enable = enable ? 1:0;

    if (objdata.tx_enable) {
        LOG_VERBOSE(BSL_LS_BCM_PTP, 
								            (BSL_META_U(0, " Starting Tx RDPC \n")));
        shr_rdpc_callback_start(&extdpll_esmc_tx_rdpc, ESMC_INFO_PDU_DPC_TIME_USEC_DEFAULT,
                                INT_TO_PTR(unit), INT_TO_PTR(stack_id), 0, 0);
    } else {
        shr_rdpc_callback_stop(&extdpll_esmc_tx_rdpc);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_rx_portbitmap_get()
 * Purpose:
 *      Get port bitmap for ESMC Rx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      pbmp       - (OUT) Rx port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_extdpll_esmc_rx_portbitmap_get(
    int unit,
    int stack_id,
    bcm_pbmp_t *pbmp)
{
    *pbmp = objdata.rx_pbmp;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_rx_portbitmap_set()
 * Purpose:
 *      Set port bitmap for ESMC Rx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      pbmp       - (IN) Rx port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_extdpll_esmc_rx_portbitmap_set(
    int unit,
    int stack_id,
    bcm_pbmp_t pbmp)
{
				/* Check for Tx,Rx pbmp mutual exclusion if the other is already configured */
				if ( BCM_PBMP_NOT_NULL(objdata.tx_pbmp)) 
		  { 
								bcm_pbmp_t temp = objdata.tx_pbmp;
								BCM_PBMP_AND(temp, pbmp);
								if (BCM_PBMP_NOT_NULL(pbmp) &&	BCM_PBMP_NOT_NULL(temp) ) {
            LOG_ERROR(BSL_LS_BCM_PTP,
                (BSL_META_U(0, " Rx pbmp shall be mutually exclusive to Tx pbmp \n")));
								    return BCM_E_PARAM;
				    }
				}
    objdata.rx_pbmp = pbmp;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_tx_portbitmap_get()
 * Purpose:
 *      Get port bitmap for ESMC Tx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      pbmp       - (OUT) Tx port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_extdpll_esmc_tx_portbitmap_get(
    int unit,
    int stack_id,
    bcm_pbmp_t *pbmp)
{
    *pbmp = objdata.tx_pbmp;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_tx_portbitmap_set()
 * Purpose:
 *      Set port bitmap for ESMC Tx.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      pbmp       - (IN) Tx port bitmap.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_extdpll_esmc_tx_portbitmap_set(
    int unit,
    int stack_id,
    bcm_pbmp_t pbmp)
{
				/* Check for Tx,Rx pbmp mutual exclusion if the other is already configured */
				if ( BCM_PBMP_NOT_NULL(objdata.rx_pbmp)) 
		  { 
								bcm_pbmp_t temp = objdata.rx_pbmp;
								BCM_PBMP_AND(temp, pbmp);
								if (BCM_PBMP_NOT_NULL(pbmp) &&	BCM_PBMP_NOT_NULL(temp) ) {
            LOG_ERROR(BSL_LS_BCM_PTP,
                (BSL_META_U(0, " Tx pbmp shall be mutually exclusive to Rx pbmp \n")));
								    return BCM_E_PARAM;
				    }
				}
    objdata.tx_pbmp = pbmp;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_info_pdu_tx()
 * Purpose:
 *      ESMC information PDU (as DPC).
 * Parameters:
 *      arg_unit       - (IN) Pointer to Unit number (as void*).
 *      arg_stack_id   - (IN) Pointer to Stack identifier index (as void*).
 *      unused         - (IN) Unused.
 * Returns:
 *      Time interval before next call
 * Notes:
 */
static sal_usecs_t
bcm_extdpll_esmc_info_pdu_tx(
    void **arg_unit,
    void **arg_stack_id,
    void **unused1,
    void **unused2)
{
    int rv;

    int unit = PTR_TO_INT(*arg_unit);
    int stack_id = PTR_TO_INT(*arg_stack_id);

    bcm_esmc_pdu_data_t pdu_data;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return ESMC_INFO_PDU_DPC_TIME_USEC_IDLE;
    }

    if (objdata.tx_enable && BCM_PBMP_NOT_NULL(objdata.tx_pbmp)) {
        /* Transmit ESMC information PDU. */
        pdu_data = objdata.pdu_data;
        pdu_data.pdu_type = bcm_esmc_pdu_type_info;

        if (BCM_FAILURE(rv = bcm_esmc_tx(unit, stack_id, objdata.tx_pbmp, &pdu_data))) {
            PTP_ERROR_FUNC("bcm_esmc_tx()");
        }
    }

    return ESMC_INFO_PDU_DPC_TIME_USEC_DEFAULT;
}

static int bcm_extdpll_squelch_cdr_mux_enable(int unit, bcm_port_t port, int enable)
{
				int rv = BCM_E_NONE;

				bcm_time_synce_clock_source_config_t config;
				bcm_time_synce_clock_source_control_t control = bcmTimeSynceClockSourceControlSquelch;

//				bsl_printf(" ===== bcm_extdpll_squelch_cdr_mux_enable: port[%d] enable[%d] ===== \n", port, enable);
    
				if (objdata.port_l1mux_pri == port) {
								config.clk_src = bcmTimeSynceClockSourcePrimary;
				    LOG_INFO(BSL_LS_BCM_PTP,
            (BSL_META_U(0, " Squelching CDR clock. port[%d] mux[%d] enable[%d] \n"), port, config.clk_src, enable));

								if (BCM_FAILURE(rv = bcm_time_synce_clock_source_control_set(unit, &config, control, enable))) {
												PTP_ERROR_FUNC("bcm_time_synce_clock_source_control_set(squelch)");
								}
				}

				if (objdata.port_l1mux_bkup == port) {
								config.clk_src = bcmTimeSynceClockSourceSecondary;
				    LOG_INFO(BSL_LS_BCM_PTP,
            (BSL_META_U(0, " Squelching CDR clock. port[%d] mux[%d] enable[%d] \n"), port, config.clk_src, enable));
								if (BCM_FAILURE(rv = bcm_time_synce_clock_source_control_set(unit, &config, control, enable))) {
												PTP_ERROR_FUNC("bcm_time_synce_clock_source_control_set(squelch)");
								}
				}
    return rv;
}

/*
 * Function:
 *      bcm_extdpll_esmc_info_pdu_rx()
 * Purpose:
 *      ESMC processing and timeout detection (as DPC).
 * Parameters:
 *      arg_unit       - (IN) Pointer to Unit number (as void*).
 *      arg_stack_id   - (IN) Pointer to Stack identifier index (as void*).
 *      unused         - (IN) Unused.
 * Returns:
 *      Time interval before next call
 * Notes:
 */
static sal_usecs_t
bcm_extdpll_esmc_info_pdu_rx(
    void **arg_unit,
    void **arg_stack_id,
    void **unused1,
    void **unused2)
{
    int rv;

    int unit = PTR_TO_INT(*arg_unit);
    int stack_id = PTR_TO_INT(*arg_stack_id);
				bcm_port_t port;

    bcm_esmc_pdu_data_t pdu_data_port;
				sal_time_t pdu_timestamp_port;

				bcm_tdpll_esmc_timeout_event_cb_data_t cb_data;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return ESMC_INFO_PDU_DPC_TIME_USEC_IDLE;
    }

    if (!objdata.esmc_cb_hdlr) {
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                (BSL_META_U(0, " Rx_DPC: Callback handler not registered.Rx_en[%d] \n"), objdata.rx_enable));
        return BCM_E_NONE;
				}

    if (objdata.rx_enable && BCM_PBMP_NOT_NULL(objdata.rx_pbmp)) {
        BCM_PBMP_ITER(objdata.rx_pbmp, port)
				    {
            if (BCM_FAILURE(rv = bcm_esmc_pdu_port_data_get(unit, stack_id, port, &pdu_data_port, &pdu_timestamp_port))) {
                continue;
            }

            if (objdata.esmc_info[port].flags & (1<<ESMC_FLAG_BIT_ESMC_TIMEOUT_INDICATION)) {
																continue;
            }

												if ((pdu_timestamp_port != 0) &&
																((_bcm_ptp_monotonic_time() - pdu_timestamp_port) >= ESMC_FAILURE_TIMEOUT_SEC) ) {
            
												    LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(0, " Rx_DPC: port PDU timestamp[%d] CurrentTime[%d]\n"), (int)pdu_timestamp_port, (int)_bcm_ptp_monotonic_time()));

                if (BCM_FAILURE(rv = bcm_extdpll_squelch_cdr_mux_enable(unit, port, 1))) {
        								    PTP_ERROR_FUNC("bcm_extdpll_squelch_cdr_mux_enable");
        				    }

    											/* Notify application of the ESMC Rx timeout */
    								    cb_data.event_type = bcmTdpllEventCbTypeEsmcTimeout;
    								    cb_data.port = port;
                objdata.esmc_cb_hdlr(unit, stack_id, (bcm_tdpll_event_cb_data_t *)&cb_data);

                /* Reset ssm code to default, so that QL change notification goes to the 
												       application when port is up again */
                objdata.esmc_info[port].flags |= (1<<ESMC_FLAG_BIT_ESMC_TIMEOUT_INDICATION);
                objdata.esmc_info[port].ssm_code = ESMC_SSM_CODE_DEFAULT;

            }
        } /* BCM_PBMP_ITER */
    } /* if (objdata.rx_enable.. */

    return ESMC_INFO_PDU_DPC_TIME_USEC_DEFAULT;
}
/*
 * Function:
 *      bcm_extdpll_esmc_essm_code_get()
 * Purpose:
 *      Get synce clock type for ESMC.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      essm_code - (OUT) essm code as per G.8264 amd2.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_extdpll_esmc_essm_code_get(
    int unit,
    int stack_id,
    bcm_esmc_essm_code_t *essm_code)
{
    *essm_code= objdata.pdu_data.essm_code;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_essm_code_set()
 * Purpose:
 *      Get synce clock type for ESMC.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      essm_code - (IN) essm oode type as per G.8264 amd 2.
 * Returns:
 *      BCM_E_XXX - Function status.
 */
int
bcm_extdpll_esmc_essm_code_set(
    int unit,
    int stack_id,
    bcm_esmc_essm_code_t essm_code)
{

    objdata.pdu_data.essm_code = essm_code;

    switch (essm_code) {
        case bcmEsmcEssmCodeEEC:
           objdata.pdu_data.essm_eec_count = 1;
           break;
        default:
            break;
    }        

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_event_callback_register()
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
bcm_extdpll_event_callback_register(
    int unit, 
    int stack_id, 
    bcm_tdpll_event_cb_f callback)
{

    int rv;
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    objdata.esmc_cb_hdlr = callback;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_event_callback_unregister()
 * Purpose:
 *      Unregister the event callback handler with ESMC stack.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 */
int
bcm_extdpll_event_callback_unregister(
    int unit, 
    int stack_id)
{
				int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    objdata.esmc_cb_hdlr = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_extdpll_esmc_select_port_set()
 * Purpose:
 *      Keep ESMC stack up to date with selected CDR MUX reference.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) Stack identifier index.
 *      port       - (IN) selected port number.
 *      clk_src    - (IN) L1 MUX - primary/backup.
 * Returns:
 *      BCM_E_XXX - Function status.
	*  To inform ESMC stack whenever the CDR MUX reference changes
 */
#define TDPLL_INPUT_CLK_SYNCE1	(BCM_TDPLL_INPUT_CLOCK_NUM_GPIO)
#define TDPLL_INPUT_CLK_SYNCE2	(BCM_TDPLL_INPUT_CLOCK_NUM_GPIO+1)

int
bcm_extdpll_esmc_select_port_set(
    int unit, 
				bcm_port_t port,
				bcm_time_synce_clock_src_type_t clk_src)
{
				if (clk_src == bcmTimeSynceClockSourcePrimary) {
				    objdata.port_l1mux_pri = port;
				}

				if (clk_src == bcmTimeSynceClockSourceSecondary) {
				    objdata.port_l1mux_bkup = port;
				}

    return BCM_E_NONE;
}

int
bcm_extdpll_esmc_synce_event_handler(
    int unit,
				int stack_id,
				int input_clk_index)
{
				int rv = BCM_E_NONE;

				bcm_tdpll_synce_ts_miss_event_cb_data_t cb_data;

    if (!objdata.esmc_cb_hdlr) {
        LOG_INFO(BSL_LS_BCM_PTP,
                (BSL_META_U(0, " SyncE_TS_Miss: Callback handler not registered \n")));
        return BCM_E_NONE;
				}

				cb_data.event_type = bcmTdpllEventCbTypeSynceTsMiss; 

				if (input_clk_index == TDPLL_INPUT_CLK_SYNCE1) {
				    cb_data.port = objdata.port_l1mux_pri;
				} else if (input_clk_index == TDPLL_INPUT_CLK_SYNCE2) {
				    cb_data.port = objdata.port_l1mux_bkup;
				} else {
        return BCM_E_PARAM;
				}

				if (objdata.esmc_info[cb_data.port].flags & (1<<ESMC_FLAG_BIT_SYNCE_TS_MISS_INDICATION)) {
    				return rv;
    } else {
        objdata.esmc_info[cb_data.port].flags |= (1<<ESMC_FLAG_BIT_SYNCE_TS_MISS_INDICATION);
        objdata.esmc_info[cb_data.port].ssm_code = ESMC_SSM_CODE_DEFAULT;
    }

				/* Squelch the MUX output as SyncE loss is detected for the best port configured for CDR MUX */
    if (BCM_FAILURE(rv = bcm_extdpll_squelch_cdr_mux_enable(unit, cb_data.port, 1))) {
								PTP_ERROR_FUNC("bcm_extdpll_squelch_cdr_mux_enable");
				}

    objdata.esmc_cb_hdlr(unit, stack_id, (bcm_tdpll_event_cb_data_t *)&cb_data);
				
    return rv;
}

void bcm_extdpll_linkscan_handler(int unit, bcm_port_t port, bcm_port_info_t * info)
{
				int rv = BCM_E_NONE;

				bcm_tdpll_port_linkdown_event_cb_data_t cb_data;

#if 0
    bsl_printf("Link change on port %d\n", port);
    bsl_printf("    enable     : %d\n", info->enable);
    bsl_printf("    linkstatus : %d\n", info->linkstatus);
    bsl_printf("    autoneg    : %d\n", info->autoneg);
    bsl_printf("    speed      : %d\n", info->speed);
    bsl_printf("    linkscan   : %d\n", info->linkscan);
    bsl_printf("    encap_mode : %d\n", info->encap_mode);
    bsl_printf("    fault      : %d\n", info->fault);
#endif

    LOG_VERBOSE(BSL_LS_BCM_PTP,
            (BSL_META_U(0, "Linkscan notification for port[%d] status[%d] \n"), port, info->linkstatus));

    if (!objdata.esmc_cb_hdlr) {
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                (BSL_META_U(0, " Linkscan_Hdlr : Callback handler not registered \n")));
        return;
				}
								
				if (!BCM_PBMP_MEMBER(objdata.rx_pbmp, port)) {
        LOG_VERBOSE(BSL_LS_BCM_PTP,
            (BSL_META_U(0, " Port[%d] is not a member of Rx_pbmp \n"), port));
								return;
				}

    if (info->linkstatus == 0) {
								if (objdata.esmc_info[port].flags & (1<<ESMC_FLAG_BIT_PORT_LINK_DOWN_INDICATION)) {
    								return;
        } else {
            objdata.esmc_info[port].flags |= (1<<ESMC_FLAG_BIT_PORT_LINK_DOWN_INDICATION);
            objdata.esmc_info[port].ssm_code = ESMC_SSM_CODE_DEFAULT;
        }

        if (BCM_FAILURE(rv = bcm_extdpll_squelch_cdr_mux_enable(unit, port, 1))) {
												PTP_ERROR_FUNC("bcm_extdpll_squelch_cdr_mux_enable");
								}

								cb_data.event_type = bcmTdpllEventCbTypeLinkDown;
								cb_data.port = port;
        objdata.esmc_cb_hdlr(unit, 0, (bcm_tdpll_event_cb_data_t *)&cb_data);
				}
}

bcm_error_t bcm_extdpll_register_linkscan_callback(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    BCM_IF_ERROR_RETURN(bcm_linkscan_register(unit, bcm_extdpll_linkscan_handler));
    return rv;
}

bcm_error_t bcm_extdpll_unregister_linkscan_callback(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    BCM_IF_ERROR_RETURN(bcm_linkscan_unregister(unit, bcm_extdpll_linkscan_handler));
    return rv;
}

int bcm_esmc_extdpll_event_handler( int unit, int stack_id, bcm_tdpll_event_cb_data_t *data)
{
    cli_out("\n esmc_extdpll_event_handler:\n");
    switch(data->cb_event_type) {
        case bcmTdpllEventCbTypeQlChange:
								{
            bcm_tdpll_ql_change_event_cb_data_t *ptr = (bcm_tdpll_ql_change_event_cb_data_t *)data;
            cli_out("\t Event type : ESMC QL Changed\n");
            cli_out("\t port       : [%d]\n", ptr->port);
            cli_out("\t QL_old     : [%d]\n", ptr->ql_old);
            cli_out("\t QL_new     : [%d]\n", ptr->ql_new);
            break;
								}
        case bcmTdpllEventCbTypeEsmcTimeout:
								{
            bcm_tdpll_esmc_timeout_event_cb_data_t *ptr = (bcm_tdpll_esmc_timeout_event_cb_data_t *)data;
            cli_out("\t Event type : ESMC Timeout\n");
            cli_out("\t port       : [%d]\n", ptr->port);
            break;
								}
        case bcmTdpllEventCbTypeSynceTsMiss:
								{
            bcm_tdpll_synce_ts_miss_event_cb_data_t *ptr = (bcm_tdpll_synce_ts_miss_event_cb_data_t *)data;
            cli_out("\t Event type : SyncE TS Missing\n");
            cli_out("\t port       : [%d]\n", ptr->port);
            break;
								}
        case bcmTdpllEventCbTypeLinkDown:
								{
            bcm_tdpll_port_linkdown_event_cb_data_t *ptr = (bcm_tdpll_port_linkdown_event_cb_data_t *)data;
            cli_out("\t Event type : Port Link down\n");
            cli_out("\t port       : [%d]\n", ptr->port);
            break;
								}
        default:
            cli_out("\t Unsupported Event type \n");
    }
    return 0;
}

int bcm_extdpll_esmc_event_cb_register(int unit, int stack_id)
{
    bcm_error_t rv = BCM_E_NONE;
    BCM_IF_ERROR_RETURN(bcm_tdpll_event_callback_register(unit, stack_id, bcm_esmc_extdpll_event_handler));
				return rv;
}

int bcm_extdpll_esmc_event_cb_unregister(int unit, int stack_id)
{
    bcm_error_t rv = BCM_E_NONE;
    BCM_IF_ERROR_RETURN(bcm_tdpll_event_callback_unregister(unit, stack_id));
				return rv;
}

#endif /* defined(INCLUDE_PTP) */
