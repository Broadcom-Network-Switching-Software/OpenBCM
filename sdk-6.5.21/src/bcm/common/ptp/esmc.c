/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: esmc.c
 *
 * Purpose: Ethernet Synchronization Messaging Channel (ESMC) infrastructure.
 *
 * Functions:
 *      bcm_esmc_init
 *      bcm_esmc_rx
 *      bcm_common_esmc_tx
 *      bcm_esmc_pdu_tx_dnx
 *      bcm_esmc_rx_callback_register
 *      bcm_esmc_rx_callback_unregister
 *      bcm_esmc_tunnel_get
 *      bcm_esmc_tunnel_set
 *      bcm_esmc_g781_option_get()
 *      bcm_esmc_g781_option_set()
 *      bcm_esmc_QL_SSM_map
 *      bcm_esmc_SSM_QL_map
 *      bcm_esmc_pdu_port_data_get()
 *
 *      bcm_esmc_pdu_read
 *      bcm_esmc_pdu_write
 */

#if defined(INCLUDE_PTP)

#include <shared/bsl.h>

#include <bcm/pkt.h>
#include <bcm/tx.h>

#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/ptp_common.h>
#include <bcm/error.h>

#if defined (BCM_QAX_SUPPORT) || defined (BCM_QUX_SUPPORT)
#include <bcm/stack.h>
#include <soc/dpp/headers.h>
#include <soc/dpp/error.h>
#include <soc/dpp/port_sw_db.h>
#endif

#if defined(BCM_MONTEREY_SUPPORT) & defined(INCLUDE_GDPLL)
extern int _bcm_esw_synce_phy_port_get_lane(int unit, int port, int rx_lane, int *adjusted_lane, int *adjusted_phy_port);
#endif /* defined(BCM_MONTEREY_SUPPORT) & defined(INCLUDE_GDPLL) */

/* Definitions. */
#define ESMC_PDU_SIZE_OCTETS                  (28)
#define ESMC_PDU_PAD_SIZE_OCTETS              (64)
#define ESMC_PDU_SOURCE_MAC_OFFSET            (6)
#define ESMC_PDU_ETHERTYPE_OFFSET             (12)
#define ESMC_PDU_SLOW_PROTOCOL_SUBTYPE_OFFSET (14)
#define ESMC_PDU_EVENT_FLAG_OFFSET            (20)
#define ESMC_PDU_SSMQL_OFFSET                 (27)

#define ESMC_ETHERTYPE_SLOW_PROTOCOL          (0x8809)
#define ESMC_SLOW_PROTOCOL_SUBTYPE_OSSP       (0x0a)

#define ESMC_8021Q_TPID                       (0x8100)
#define ESMC_8021Q_SIZE                       (4)

#define ESMC_NETWORK_OPTION_DEFAULT (bcm_esmc_network_option_g781_II)
#define ESMC_QL_DEFAULT             (bcm_esmc_g781_II_ql_dus)
#define ESMC_SSM_CODE_DEFAULT       (0x0f)

#define ESMC_RX_ENABLE_DEFAULT                (0)

#define ESMC_PORT_TABLE_ENTRIES_MAX _SHR_PBMP_PORT_MAX

/* G.8264 Amd2 extended QL TLV  values */
#define ESMC_PDU_ESSM_CODE_TLV_TYPE               (0x02)
#define ESMC_PDU_ESSM_CODE_TLV_LEN                (0x0014)

#define ESMC_PDU_ESSM_CODE_TLV_SIZE_OCTETS        (20)

#define ESMC_PDU_ESSM_CODE_TLV_TYPE_OFFSET        (28)
#define ESMC_PDU_ESSM_CODE_TLV_LEN_OFFSET         (29) /* 16 bit */

#define ESMC_PDU_ESSM_CODE_TLV_SSMCODE_OFFSET     (31)
#define ESMC_PDU_ESSM_CODE_TLV_FLAG_OFFSET        (40)
#define ESMC_PDU_ESSM_CODE_TLV_EEEC_CNT_OFFSET    (41)
#define ESMC_PDU_ESSM_CODE_TLV_EEC_CNT_OFFSET     (42)

/* Macros. */

/* Types. */
typedef struct bcm_esmc_port_s {
    bcm_esmc_pdu_data_t pdu_data;
    sal_time_t timestamp;
} bcm_esmc_port_t;

typedef struct bcm_esmc_data_s {
    int rx_enable;

    bcm_esmc_rx_cb rx_callback;
    bcm_esmc_port_t rx_port[ESMC_PORT_TABLE_ENTRIES_MAX];

    bcm_pkt_t *tx_pkt;

    bcm_esmc_network_option_t network_option;
} bcm_esmc_data_t;

/* Constants and variables. */
static const uint8 esmc_pdu_template[ESMC_PDU_SIZE_OCTETS + ESMC_PDU_ESSM_CODE_TLV_SIZE_OCTETS] = {
    0x01, 0x80, 0xc2, 0x00, 0x00, 0x02,    /* Destination MAC. */
    0x00, 0x10, 0x18, 0x00, 0x00, 0x00,    /* Source MAC. */
    0x88, 0x09,                            /* Slow Protocol Ethertype. */
    ESMC_SLOW_PROTOCOL_SUBTYPE_OSSP,       /* Slow Protocol subtype. */
    0x00, 0x19, 0xa7,                      /* ITU Organization Unique Identifier (OUI). */
    0x00, 0x01,                            /* ITU Subtype. */
    0x10,                                  /* Version, Event Flag. */
    0x00, 0x00, 0x00,                      /* Reserved. */
    0x01,                                  /* TLV Type. */
    0x00, 0x04,                            /* TLV Length. */
    0x00,                                  /* Reserved, SSM/QL. */
    /* G8264Amd2 - ESSM code TLV -  synce clock type */
    0x02,                                               /* ESSM code TLV - Type - 0x02. */
    0x00, 0x14,                                         /* ESSM code TLV - Length. */
    bcmEsmcEssmCodeNone,                                /* ESSM code TLV - ESSMCode */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* ESSM code TLV - synce master id */
    0x00,                                               /* ESSM code TLV - flag */
    0x00,                                               /* ESSM code TLV - cascaded EEEC count */
    0x00,                                               /* ESSM code TLV - cascaded EEC count */
    0x00, 0x00, 0x00, 0x00, 0x00                        /* ESSM code TLV - Reserved */
};

static const bcm_mac_t slow_protocol_multicast_mac = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x02};

static bcm_esmc_data_t objdata;

/* Static functions. */
static int bcm_esmc_pdu_read(
    uint8 *esmc_pdu, int esmc_pdu_len,
    bcm_esmc_pdu_data_t *esmc_pdu_data);

static int bcm_esmc_pdu_write(
    bcm_esmc_pdu_data_t *esmc_pdu_data, int esmc_pdu_len_max,
    uint8 *esmc_pdu, int *esmc_pdu_len);

/*
 * Function:
 *      bcm_esmc_init()
 * Purpose:
 *      Initialize Ethernet Synchronization Messaging Channel (ESMC).
 * Parameters:
 *      unit      - (IN) Unit number.
 *      stack_id  - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esmc_init(
    int unit,
    int stack_id)
{
    int rv;
    int port_num;

    /* Initialize ESMC module data. */
    objdata.rx_enable = ESMC_RX_ENABLE_DEFAULT;
    objdata.rx_callback = NULL;
    objdata.network_option = ESMC_NETWORK_OPTION_DEFAULT;

    if (NULL == objdata.tx_pkt) {
        if (BCM_FAILURE(rv = bcm_pkt_alloc(unit, 128, BCM_TX_CRC_REGEN, &objdata.tx_pkt))) {
            return rv;
        }
    }

    for (port_num = 0; port_num < ESMC_PORT_TABLE_ENTRIES_MAX; ++port_num) {
        sal_memset(objdata.rx_port[port_num].pdu_data.source_mac, 0, sizeof(bcm_mac_t));
        objdata.rx_port[port_num].pdu_data.pdu_type = bcm_esmc_pdu_type_info;
        objdata.rx_port[port_num].pdu_data.ql = ESMC_QL_DEFAULT;
        objdata.rx_port[port_num].pdu_data.ssm_code = ESMC_SSM_CODE_DEFAULT;

        objdata.rx_port[port_num].timestamp = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_cleanup()
 * Purpose:
 *      Uninitialize Ethernet Synchronization Messaging Channel (ESMC).
 * Parameters:
 *      unit      - (IN) Unit number.
 *      stack_id  - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esmc_cleanup(
    int unit,
    int stack_id)
{
    /* Leave objdata.tx_pkt allocated, as it won't be re-allocated on re-init */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_rx()
 * Purpose:
 *      ESMC PDU receive (Rx).
 * Parameters:
 *      unit         - (IN) Unit number.
 *      stack_id     - (IN) Stack identifier index.
 *      ingress_port - (IN) Ingress port.
 *      ethertype    - (IN) Ethertype.
 *      esmc_pdu_len - (IN) ESMC PDU length (octets).
 *      esmc_pdu     - (IN) ESMC PDU.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esmc_rx(
    int unit,
    int stack_id,
    int ingress_port,
    int ethertype,
    int esmc_pdu_len,
    uint8 *esmc_pdu)
{
    int rv;
    bcm_esmc_pdu_data_t esmc_pdu_data;

    /* Argument checking and error handling. */
    if (ESMC_PDU_SIZE_OCTETS > esmc_pdu_len) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "bcm_esmc_rx(): Insufficient message length (%d).\n"), esmc_pdu_len));
        return BCM_E_PARAM;
    }

    if (_bcm_ptp_uint16_read(esmc_pdu + ESMC_PDU_ETHERTYPE_OFFSET) == ESMC_8021Q_TPID) {
        /*
         * Tagged packet.
         * Remove 802.1Q header (single tag assumed).
         * NB: Use memmove() b/c there is no corresponding sal_memmove().
         */
        memmove(esmc_pdu + ESMC_PDU_ETHERTYPE_OFFSET,
            esmc_pdu + ESMC_PDU_ETHERTYPE_OFFSET + ESMC_8021Q_SIZE,
            esmc_pdu_len - ESMC_PDU_ETHERTYPE_OFFSET - ESMC_8021Q_SIZE);
        sal_memset(esmc_pdu + esmc_pdu_len - ESMC_8021Q_SIZE, 0, ESMC_8021Q_SIZE);
        esmc_pdu_len -= ESMC_8021Q_SIZE;
    }

    if (ESMC_ETHERTYPE_SLOW_PROTOCOL != ethertype) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "bcm_esmc_rx(): Incorrect Ethertype (0x%04x).\n"), ethertype));
        return BCM_E_PARAM;
    } else if (ESMC_SLOW_PROTOCOL_SUBTYPE_OSSP !=
               esmc_pdu[ESMC_PDU_SLOW_PROTOCOL_SUBTYPE_OFFSET]) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "bcm_esmc_rx(): Unexpected Slow Protocols subtype (0x%02x).\n"),
                     esmc_pdu[ESMC_PDU_SLOW_PROTOCOL_SUBTYPE_OFFSET]));
        return BCM_E_PARAM;
    } else if (sal_memcmp(esmc_pdu, slow_protocol_multicast_mac, sizeof(bcm_mac_t))) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "bcm_esmc_rx(): Incorrect destination MAC (%02x:%02x:%02x:%02x:%02x:%02x).\n"),
                     esmc_pdu[0], esmc_pdu[1], esmc_pdu[2], esmc_pdu[3], esmc_pdu[4], esmc_pdu[5]));
        return BCM_E_PARAM;
    }

    if (BCM_FAILURE(rv = bcm_esmc_pdu_read(esmc_pdu, esmc_pdu_len,
            &esmc_pdu_data))) {
        PTP_ERROR_FUNC("bcm_esmc_pdu_read()");
        return rv;
    }

#if defined(BCM_MONTEREY_SUPPORT) & defined(INCLUDE_GDPLL)
    if (SOC_IS_MONTEREY(unit)) {
        _bcm_esw_synce_phy_port_get_lane(0, ingress_port, 1, NULL, &ingress_port);
    }
#endif

    /* Update ESMC port table entry. */
    if (ingress_port > 0 && ingress_port <= ESMC_PORT_TABLE_ENTRIES_MAX) {
        objdata.rx_port[ingress_port-1].pdu_data = esmc_pdu_data;
        objdata.rx_port[ingress_port-1].timestamp = _bcm_ptp_monotonic_time();
    }

    if (objdata.rx_callback) {
        LOG_VERBOSE(BSL_LS_BCM_PTP,
            (BSL_META_U(0, " ESMC PDU Rcvd: port[%d] Type[%d] ssm[%d] src_mac[%02x.%02x.%02x.%02x.%02x.%02x]\n"),
                ingress_port, esmc_pdu_data.pdu_type, esmc_pdu_data.ssm_code,
                esmc_pdu_data.source_mac[0], esmc_pdu_data.source_mac[1],esmc_pdu_data.source_mac[2],
                esmc_pdu_data.source_mac[3],esmc_pdu_data.source_mac[4],esmc_pdu_data.source_mac[5]));
        if (BCM_FAILURE(rv = objdata.rx_callback(unit, stack_id, ingress_port, &esmc_pdu_data))) {
            PTP_ERROR_FUNC("ESMC Rx Callback f()");
            return rv;
        }
    }

    return BCM_E_NONE;
}

#if defined (BCM_QAX_SUPPORT) || defined (BCM_QUX_SUPPORT)
/*
 * Function:
 *      bcm_esmc_pdu_tx_dnx()
 * Purpose:
 *      Add required DNX headers to ESMC PDU and transmit (Tx).
 * Parameters:
 *      unit          - (IN) Unit number.
 *      esmc_pdu      - (IN) ESMC PDU.
 *      esmc_pdu_len  - (IN) ESMC PDU length.
 * Returns:
 *      None.
 * Notes:
 */
void
bcm_esmc_pdu_tx_dnx(
    int    unit,
    uint8  *esmc_pdu,
    int    esmc_pdu_len)
{
    int rv;
    int gport;
    int port_ndx;
    bcm_gport_t    sysport;
    soc_dpp_itmh_t itmh; /* itmh header contents */

    bcm_pkt_t      *esmc_packet = objdata.tx_pkt;

    if (SOC_IS_JERICHO(unit)) {

        esmc_packet->flags = 0x21;

        BCM_PBMP_ITER(esmc_packet->tx_pbmp, port_ndx){
            /* Fetch the system port */
            rv = bcm_stk_gport_sysport_get(unit, port_ndx, &sysport);
            if(BCM_E_NONE != rv) {
                LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META("bcm_stk_gport_sysport_get failed. TX Unit: %d port: %d Error(%d): %s\n"), \
                        unit, port_ndx, rv, bcm_errmsg(rv)));
            }

            /* Get gport for the system port */
            gport = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);

            sal_memset(&itmh, 0, sizeof(soc_dpp_itmh_t));

            itmh.base.jer_dest_info.dest_info_hi = 0x1;
            itmh.base.jer_dest_info.dest_info_mi = (gport >> 8) & 0xff;
            itmh.base.jer_dest_info.dest_info_lo = (gport & 0xff);

            /* Update PTCH header */
            esmc_packet->pkt_data[0].data[0] = 0x50;
            esmc_packet->pkt_data[0].data[1] = 0x00;

            /* Update ITMH header */
            sal_memcpy(BCM_PKT_IEEE(esmc_packet) + DPP_HDR_PTCH_TYPE2_LEN, &itmh.base.raw.bytes[0], DPP_HDR_ITMH_BASE_LEN);

            /* Update ESMC packet */
            sal_memcpy(BCM_PKT_IEEE(esmc_packet) + DPP_HDR_ITMH_BASE_LEN + DPP_HDR_PTCH_TYPE2_LEN, esmc_pdu, esmc_pdu_len);

            /* Update length */
            esmc_packet->pkt_data[0].len = DPP_HDR_PTCH_TYPE2_LEN + DPP_HDR_ITMH_BASE_LEN + esmc_pdu_len;

            LOG_VERBOSE(BSL_LS_BCM_PTP,
                (BSL_META("port_ndx[%d] sysport[0x%08x] gport[0x%08x] Header[%02x:%02x:%02x:%02x:%02x:%02x] pkt_len[%d] \n"), port_ndx, sysport, gport,
                    esmc_packet->pkt_data[0].data[0], esmc_packet->pkt_data[0].data[1], esmc_packet->pkt_data[0].data[2],
                    esmc_packet->pkt_data[0].data[3], esmc_packet->pkt_data[0].data[4], esmc_packet->pkt_data[0].data[5],
                    esmc_packet->pkt_data[0].len));

            rv = bcm_tx(unit, esmc_packet, NULL);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META("bcm_tx failed: TX Unit %d: %s\n"), \
                        unit, bcm_errmsg(rv)));
                break;
            }
        } /* BCM_PBMP_ITER */

    } /* SOC_IS_JERICHO*/

}
#endif

/*
 * Function:
 *      bcm_common_esmc_tx()
 * Purpose:
 *      ESMC PDU transmit (Tx).
 * Parameters:
 *      unit          - (IN) Unit number.
 *      stack_id      - (IN) Stack identifier index.
 *      pbmp          - (IN) Tx port bitmap.
 *      esmc_pdu_data - (IN) ESMC PDU data.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_esmc_tx(
    int unit,
    int stack_id,
    bcm_pbmp_t pbmp,
    bcm_esmc_pdu_data_t *esmc_pdu_data)
{
    int rv;

    int esmc_pdu_len;
    int esmc_pdu_len_max = ESMC_PDU_PAD_SIZE_OCTETS + ESMC_PDU_ESSM_CODE_TLV_SIZE_OCTETS ;
    uint8 esmc_pdu[ESMC_PDU_PAD_SIZE_OCTETS + ESMC_PDU_ESSM_CODE_TLV_SIZE_OCTETS] = {0};

    bcm_pkt_t *esmc_packet = objdata.tx_pkt;

    if (BCM_FAILURE(rv = bcm_esmc_pdu_write(esmc_pdu_data,
            esmc_pdu_len_max, esmc_pdu, &esmc_pdu_len))) {
        PTP_ERROR_FUNC("bcm_esmc_pdu_write()");
        return rv;
    }

    BCM_PBMP_ASSIGN(esmc_packet->tx_pbmp, pbmp);

#if defined (BCM_QAX_SUPPORT) || defined (BCM_QUX_SUPPORT)
    bcm_esmc_pdu_tx_dnx(unit, esmc_pdu, esmc_pdu_len);
#else

    sal_memcpy(BCM_PKT_IEEE(esmc_packet), esmc_pdu, esmc_pdu_len);
    esmc_packet->pkt_data[0].len = esmc_pdu_len + 4;

    if (BCM_FAILURE(rv = bcm_tx(unit, esmc_packet, NULL))) {
        PTP_ERROR_FUNC("bcm_tx()");
        return rv;
    }

#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_rx_callback_register()
 * Purpose:
 *      Register ESMC PDU Rx callback.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 *      rx_cb    - (IN) ESMC PDU Rx callback function pointer.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_esmc_rx_callback_register(
    int unit,
    int stack_id,
    bcm_esmc_rx_cb rx_cb)
{
    objdata.rx_callback = rx_cb;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_rx_callback_unregister()
 * Purpose:
 *      Unregister ESMC PDU Rx callback.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_esmc_rx_callback_unregister(
    int unit,
    int stack_id)
{
    objdata.rx_callback = NULL;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_tunnel_get()
 * Purpose:
 *      Get ESMC PDU tunneling-enabled Boolean.
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      stack_id - (IN)  PTP stack ID.
 *      enable   - (OUT) Enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Boolean controls whether ToP tunnels slow-protocol Ethertype packets
 *      - which include ESMC PDUs - to host for processing.
 */
int
bcm_common_esmc_tunnel_get(
    int unit,
    int stack_id,
    int *enable)
{
    int rv;

    uint8 resp[PTP_MGMTMSG_PAYLOAD_ESMC_TUNNEL_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_ESMC_TUNNEL_SIZE_OCTETS;

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
            &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_ESMC_TUNNEL,
            0, 0, resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Parse response.
     *    Octet 0...5 : Custom management message key/identifier.
     *                  BCM<null><null><null>.
     *    Octet 6     : ESMC PDU tunneling-enabled Boolean.
     *    Octet 7     : Reserved.
     */
    *enable = resp[6] ? 1:0; /* Advance cursor past custom management message identifier. */

    /* Set local ESMC PDU Rx enable Boolean. */
    objdata.rx_enable = *enable;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_tunnel_set()
 * Purpose:
 *      Set ESMC PDU tunneling-enabled Boolean.
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      stack_id - (IN)  PTP stack ID.
 *      enable   - (OUT) Enable Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Boolean controls whether ToP tunnels slow-protocol Ethertype packets
 *      - which include ESMC PDUs - to host for processing.
 */
int
bcm_common_esmc_tunnel_set(
    int unit,
    int stack_id,
    int enable)
{
    int rv;
    int i;
    
    uint8 payload[PTP_MGMTMSG_PAYLOAD_ESMC_TUNNEL_SIZE_OCTETS] = {0};
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
     *    Octet 0...5 : Custom management message key/identifier.
     *                  BCM<null><null><null>.
     *    Octet 6     : ESMC tunneling-enabled Boolean.
     *    Octet 7     : Reserved.
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i++] = enable ? 1:0;
    
    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, stack_id, PTP_CLOCK_NUMBER_DEFAULT,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_ESMC_TUNNEL,
            payload, PTP_MGMTMSG_PAYLOAD_ESMC_TUNNEL_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /* Set local ESMC PDU Rx enable Boolean. */
    objdata.rx_enable = enable ? 1:0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_g781_option_get()
 * Purpose:
 *      Get ITU-T G.781 networking option for SyncE.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      g781_option - (OUT) ITU-T G.781 networking option.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_esmc_g781_option_get(
    int unit,
    int stack_id,
    bcm_esmc_network_option_t *g781_option)
{
    *g781_option = objdata.network_option;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_g781_option_set()
 * Purpose:
 *      Set ITU-T G.781 networking option for SyncE.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      stack_id    - (IN) Stack identifier index.
 *      g781_option - (IN) ITU-T G.781 networking option.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_esmc_g781_option_set(
    int unit,
    int stack_id,
    bcm_esmc_network_option_t g781_option)
{
    switch (g781_option) {
    case bcm_esmc_network_option_g781_I:
    case bcm_esmc_network_option_g781_II:
    case bcm_esmc_network_option_g781_III:
        objdata.network_option = g781_option;
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_QL_SSM_map()
 * Purpose:
 *      Get synchronization status message (SSM) code corresponding to
 *      ITU-T G.781 quality level (QL).
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      opt      - (IN)  ITU-T G.781 networking option.
 *      ql       - (IN)  ITU-T G.781 quality level.
 *      ssm_code - (OUT) SSM code.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_esmc_QL_SSM_map(
    int unit,
    bcm_esmc_network_option_t opt,
    bcm_esmc_quality_level_t ql,
    uint8 *ssm_code)
{
    switch (opt) {
    case bcm_esmc_network_option_g781_I:
        switch (ql) {
        case bcm_esmc_g781_I_ql_prc:
            *ssm_code = 0x02;
            break;
        case bcm_esmc_g781_I_ql_ssua:
            *ssm_code = 0x04;
            break;
        case bcm_esmc_g781_I_ql_ssub:
            *ssm_code = 0x08;
            break;
        case bcm_esmc_g781_I_ql_sec:
            *ssm_code = 0x0b;
            break;
        case bcm_esmc_g781_I_ql_dnu:
            /* Fall through. */
        default:
            *ssm_code = 0x0f;
        }
        break;
    case bcm_esmc_network_option_g781_II:
        switch (ql) {
        case bcm_esmc_g781_II_ql_stu:
            *ssm_code = 0x00;
            break;
        case bcm_esmc_g781_II_ql_prs:
            *ssm_code = 0x01;
            break;
        case bcm_esmc_g781_II_ql_tnc:
            *ssm_code = 0x04;
            break;
        case bcm_esmc_g781_II_ql_st2:
            *ssm_code = 0x07;
            break;
        case bcm_esmc_g781_II_ql_st3:
            *ssm_code = 0x0a;
            break;
        case bcm_esmc_g781_II_ql_smc:
            *ssm_code = 0x0c;
            break;
        case bcm_esmc_g781_II_ql_st3e:
            *ssm_code = 0x0d;
            break;
        case bcm_esmc_g781_II_ql_prov:
            *ssm_code = 0x0e;
            break;
        case bcm_esmc_g781_II_ql_dus:
            /* Fall through. */
        default:
            *ssm_code = 0x0f;
        }
        break;
    case bcm_esmc_network_option_g781_III:
        switch (ql) {
        case bcm_esmc_g781_III_ql_unk:
            *ssm_code = 0x00;
            break;
        case bcm_esmc_g781_III_ql_sec:
            *ssm_code = 0x0b;
            break;
        default:
            *ssm_code = 0x0f;
        }
        break;
    default:
        *ssm_code = 0x0f;
        return BCM_E_CONFIG;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_SSM_QL_map()
 * Purpose:
 *      Get ITU-T G.781 quality level (QL) corresponding to synchronization
 *      status message (SSM) code.
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      opt      - (IN)  ITU-T G.781 networking option.
 *      ssm_code - (IN)  SSM code.
 *      ql       - (OUT) ITU-T G.781 quality level.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_common_esmc_SSM_QL_map(
    int unit,
    bcm_esmc_network_option_t opt,
    uint8 ssm_code,
    bcm_esmc_quality_level_t *ql)
{
    switch (opt) {
    case bcm_esmc_network_option_g781_I:
        switch (ssm_code) {
        case 0x00:
            *ql = bcm_esmc_g781_I_ql_inv0;
            break;
        case 0x01:
            *ql = bcm_esmc_g781_I_ql_inv1;
            break;
        case 0x02:
            *ql = bcm_esmc_g781_I_ql_prc;
            break;
        case 0x03:
            *ql = bcm_esmc_g781_I_ql_inv3;
            break;
        case 0x04:
            *ql = bcm_esmc_g781_I_ql_ssua;
            break;
        case 0x05:
            *ql = bcm_esmc_g781_I_ql_inv5;
            break;
        case 0x06:
            *ql = bcm_esmc_g781_I_ql_inv6;
            break;
        case 0x07:
            *ql = bcm_esmc_g781_I_ql_inv7;
            break;
        case 0x08:
            *ql = bcm_esmc_g781_I_ql_ssub;
            break;
        case 0x09:
            *ql = bcm_esmc_g781_I_ql_inv9;
            break;
        case 0x0a:
            *ql = bcm_esmc_g781_I_ql_inv10;
            break;
        case 0x0b:
            *ql = bcm_esmc_g781_I_ql_sec;
            break;
        case 0x0c:
            *ql = bcm_esmc_g781_I_ql_inv12;
            break;
        case 0x0d:
            *ql = bcm_esmc_g781_I_ql_inv13;
            break;
        case 0x0e:
            *ql = bcm_esmc_g781_I_ql_inv14;
            break;
        case 0x0f:
            *ql = bcm_esmc_g781_I_ql_dnu;
            break;
        default:
            *ql = bcm_esmc_g781_I_ql_nsupp;
            return BCM_E_CONFIG;
        }
        break;
    case bcm_esmc_network_option_g781_II:
        switch (ssm_code) {
        case 0x00:
            *ql = bcm_esmc_g781_II_ql_stu;
            break;
        case 0x01:
             *ql = bcm_esmc_g781_II_ql_prs;
           break;
        case 0x02:
            *ql = bcm_esmc_g781_II_ql_inv2;
            break;
        case 0x03:
            *ql = bcm_esmc_g781_II_ql_inv3;
            break;
        case 0x04:
            *ql = bcm_esmc_g781_II_ql_tnc;
            break;
        case 0x05:
            *ql = bcm_esmc_g781_II_ql_inv5;
            break;
        case 0x06:
            *ql = bcm_esmc_g781_II_ql_inv6;
            break;
        case 0x07:
            *ql = bcm_esmc_g781_II_ql_st2;
            break;
        case 0x08:
            *ql = bcm_esmc_g781_II_ql_inv8;
            break;
        case 0x09:
            *ql = bcm_esmc_g781_II_ql_inv9;
            break;
        case 0x0a:
            *ql = bcm_esmc_g781_II_ql_st3;
            break;
        case 0x0b:
            *ql = bcm_esmc_g781_II_ql_inv11;
            break;
        case 0x0c:
            *ql = bcm_esmc_g781_II_ql_smc;
            break;
        case 0x0d:
            *ql = bcm_esmc_g781_II_ql_st3e;
            break;
        case 0x0e:
            *ql = bcm_esmc_g781_II_ql_prov;
            break;
        case 0x0f:
            *ql = bcm_esmc_g781_II_ql_dus;
            break;
        default:
            *ql = bcm_esmc_g781_II_ql_nsupp;
            return BCM_E_CONFIG;
        }
        break;
    case bcm_esmc_network_option_g781_III:
        switch (ssm_code) {
        case 0x00:
            *ql = bcm_esmc_g781_III_ql_unk;
            break;
        case 0x01:
            *ql = bcm_esmc_g781_III_ql_inv1;
            break;
        case 0x02:
            *ql = bcm_esmc_g781_III_ql_inv2;
            break;
        case 0x03:
            *ql = bcm_esmc_g781_III_ql_inv3;
            break;
        case 0x04:
            *ql = bcm_esmc_g781_III_ql_inv4;
            break;
        case 0x05:
            *ql = bcm_esmc_g781_III_ql_inv5;
            break;
        case 0x06:
            *ql = bcm_esmc_g781_III_ql_inv6;
            break;
        case 0x07:
            *ql = bcm_esmc_g781_III_ql_inv7;
            break;
        case 0x08:
            *ql = bcm_esmc_g781_III_ql_inv8;
            break;
        case 0x09:
            *ql = bcm_esmc_g781_III_ql_inv9;
            break;
        case 0x0a:
            *ql = bcm_esmc_g781_III_ql_inv10;
            break;
        case 0x0b:
            *ql = bcm_esmc_g781_III_ql_sec;
            break;
        case 0x0c:
            *ql = bcm_esmc_g781_III_ql_inv12;
            break;
        case 0x0d:
            *ql = bcm_esmc_g781_III_ql_inv13;
            break;
        case 0x0e:
            *ql = bcm_esmc_g781_III_ql_inv14;
            break;
        case 0x0f:
            *ql = bcm_esmc_g781_III_ql_inv15;
            break;
        default:
            *ql = bcm_esmc_g781_III_ql_nsupp;
            return BCM_E_CONFIG;
        }
        break;
    default:
        *ql = bcm_esmc_ql_unresolvable;
        return BCM_E_CONFIG;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_pdu_port_data_get()
 * Purpose:
 *      Get data for most recent ESMC PDU of a port.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      stack_id  - (IN) Stack identifier index.
 *      port_num  - (IN) Physical port number.
 *      pdu_data  - (OUT) ESMC PDU data.
 *      timestamp - (OUT) ESMC PDU timestamp.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esmc_pdu_port_data_get(
    int unit,
    int stack_id,
    int port_num,
    bcm_esmc_pdu_data_t *pdu_data,
    sal_time_t *timestamp)
{
    if (port_num < 1 || port_num > ESMC_PORT_TABLE_ENTRIES_MAX) {
        return BCM_E_PARAM;
    }

    *pdu_data = objdata.rx_port[port_num-1].pdu_data;
    *timestamp = objdata.rx_port[port_num-1].timestamp;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_pdu_essm_code_tlv_read()
 * Purpose:
 *      Read ESMC PDU and extract relevant data.
 * Parameters:
 *      esmc_pdu      - (IN)  ESMC PDU.
 *      esmc_pdu_len  - (IN)  ESMC PDU length (octets).
 *      esmc_pdu_data - (OUT) ESMC PDU data.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
bcm_esmc_pdu_essm_code_tlv_read(
    uint8 *esmc_pdu,
    int esmc_pdu_len,
    bcm_esmc_pdu_data_t *esmc_pdu_data)
{
    esmc_pdu_data->essm_code = bcmEsmcEssmCodeNone;
    esmc_pdu_data->essm_flag = 0;
    esmc_pdu_data->essm_eec_count = 0;
    esmc_pdu_data->essm_eeec_count = 0;

    /* Argument checking and error handling. */
    if (esmc_pdu_len < (ESMC_PDU_SIZE_OCTETS + ESMC_PDU_ESSM_CODE_TLV_LEN)) {
        return BCM_E_UNAVAIL;
    }

    /* check synce clokc type ql tlv type */
    if ( ESMC_PDU_ESSM_CODE_TLV_TYPE  != esmc_pdu[ESMC_PDU_ESSM_CODE_TLV_TYPE_OFFSET]) {
        return BCM_E_UNAVAIL;
    }

    /* parse synce clock type tlv */
    esmc_pdu_data->essm_code = (bcm_esmc_essm_code_t)esmc_pdu[ESMC_PDU_ESSM_CODE_TLV_SSMCODE_OFFSET];
    esmc_pdu_data->essm_flag = esmc_pdu[ESMC_PDU_ESSM_CODE_TLV_FLAG_OFFSET];
    esmc_pdu_data->essm_eeec_count = esmc_pdu[ESMC_PDU_ESSM_CODE_TLV_EEEC_CNT_OFFSET];
    esmc_pdu_data->essm_eec_count = esmc_pdu[ESMC_PDU_ESSM_CODE_TLV_EEC_CNT_OFFSET];

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_pdu_read()
 * Purpose:
 *      Read ESMC PDU and extract relevant data.
 * Parameters:
 *      esmc_pdu      - (IN)  ESMC PDU.
 *      esmc_pdu_len  - (IN)  ESMC PDU length (octets).
 *      esmc_pdu_data - (OUT) ESMC PDU data.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
bcm_esmc_pdu_read(
    uint8 *esmc_pdu,
    int esmc_pdu_len,
    bcm_esmc_pdu_data_t *esmc_pdu_data)
{
    int rv;

    /* Argument checking and error handling. */
    if (esmc_pdu_len < ESMC_PDU_SIZE_OCTETS) {
        return BCM_E_PARAM;
    }

    /* Get source MAC address. */
    sal_memcpy(esmc_pdu_data->source_mac, esmc_pdu + ESMC_PDU_SOURCE_MAC_OFFSET, sizeof(bcm_mac_t));
    
    /* Get ESMC PDU type. */
    esmc_pdu_data->pdu_type = ((esmc_pdu[ESMC_PDU_EVENT_FLAG_OFFSET] & 0x08) >> 3);
    
    /* Get ESMC SSM/QL. */
    esmc_pdu_data->ssm_code = (esmc_pdu[ESMC_PDU_SSMQL_OFFSET]);
    if (BCM_FAILURE(rv = bcm_esmc_SSM_QL_map(0, objdata.network_option,
            esmc_pdu_data->ssm_code, &esmc_pdu_data->ql))) {
        PTP_ERROR_FUNC("bcm_esmc_SSM_QL_map");
        return rv;
    }

    /* parse G.8264 amd2 ql tlv if present */
    bcm_esmc_pdu_essm_code_tlv_read(esmc_pdu, esmc_pdu_len, esmc_pdu_data);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esmc_pdu_write()
 * Purpose:
 *      Write ESMC PDU.
 * Parameters:
 *      esmc_pdu_data    - (IN)  ESMC PDU data.
 *      esmc_pdu_len_max - (IN)  ESMC PDU maximum length (octets).
 *      esmc_pdu         - (OUT) ESMC PDU.
 *      esmc_pdu_len     - (OUT) ESMC PDU length (octets).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
bcm_esmc_pdu_write(
    bcm_esmc_pdu_data_t *esmc_pdu_data,
    int esmc_pdu_len_max,
    uint8 *esmc_pdu,
    int *esmc_pdu_len)
{
    /* Argument checking and error handling. */
    if (esmc_pdu_len_max < ESMC_PDU_PAD_SIZE_OCTETS) {
        return BCM_E_PARAM;
    }
    *esmc_pdu_len = ESMC_PDU_PAD_SIZE_OCTETS;

    /* Initialize PDU with ESMC PDU template. */
    sal_memset(esmc_pdu, 0, esmc_pdu_len_max);
    sal_memcpy(esmc_pdu, esmc_pdu_template, ESMC_PDU_SIZE_OCTETS);

    /* Set source MAC address. */
    sal_memcpy(esmc_pdu + ESMC_PDU_SOURCE_MAC_OFFSET, esmc_pdu_data->source_mac,
        sizeof(bcm_mac_t));
    
    /* Set ESMC PDU type. */
    esmc_pdu[ESMC_PDU_EVENT_FLAG_OFFSET] |= (esmc_pdu_data->pdu_type << 3);
    
    /* Set ESMC SSM/QL. */
    esmc_pdu[ESMC_PDU_SSMQL_OFFSET] = (esmc_pdu_data->ssm_code & 0x0f);

    /* if required , write g.8264 amd2 synce clock type tlv */
    if (esmc_pdu_data->essm_code != bcmEsmcEssmCodeNone) {
        sal_memcpy(esmc_pdu + ESMC_PDU_SIZE_OCTETS, esmc_pdu_template + ESMC_PDU_SIZE_OCTETS, ESMC_PDU_ESSM_CODE_TLV_LEN);
        /* TBD different sync clokc type handling */
        esmc_pdu[ESMC_PDU_ESSM_CODE_TLV_SSMCODE_OFFSET] = esmc_pdu_data->essm_code;
        esmc_pdu[ESMC_PDU_ESSM_CODE_TLV_FLAG_OFFSET] = esmc_pdu_data->essm_flag;
        esmc_pdu[ESMC_PDU_ESSM_CODE_TLV_EEEC_CNT_OFFSET] = esmc_pdu_data->essm_eeec_count;
        esmc_pdu[ESMC_PDU_ESSM_CODE_TLV_EEC_CNT_OFFSET] = esmc_pdu_data->essm_eec_count;
        *esmc_pdu_len += ESMC_PDU_ESSM_CODE_TLV_SIZE_OCTETS;
    }

    return BCM_E_NONE;
}

#endif /* defined(INCLUDE_PTP) */
