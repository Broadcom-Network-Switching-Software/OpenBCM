/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ifa2.c
 * Purpose:     IFAv2.0 embedded application interface.
 */

#if defined(INCLUDE_IFA)

#include <shared/bslenum.h>
#include <shared/bsl.h>
#include <soc/uc.h>
#include <soc/shared/shr_pkt.h>
#include <bcm/error.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/ifa2_pack.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/ifa.h>
#include <bcm_int/esw/collector.h>
#include <bcm_int/esw/switch.h>

/*
 * Macro:
 *     _BCM_IFA2_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_IFA2_ALLOC(_ptr_,_ptype_,_size_,_descr_)                   \
    do {                                                                \
        if (NULL == (_ptr_)) {                                          \
            (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_));       \
        }                                                               \
        if((_ptr_) != NULL) {                                           \
            sal_memset((_ptr_), 0, (_size_));                           \
        }  else {                                                       \
            LOG_ERROR(BSL_LS_BCM_IFA,                                   \
                      (BSL_META("IFA2 Error: Allocation failure %s\n"), \
                       (_descr_)));                                     \
        }                                                               \
    } while (0)

/* Default value of max rx pkt length soc property. */
#define _BCM_XGS5_IFA2_DEF_MAX_RX_PKT_LENGTH (1500)

/* Max possible length of the Rx packet. */
#define _BCM_XGS5_IFA2_MAX_RX_PKT_LENGTH (9000)

/* Default value of the max export length soc property. */
#define _BCM_XGS5_IFA2_DEF_MAX_EXPORT_PKT_LENGTH (9000)

/* Min length of the export packet. */
#define _BCM_XGS5_IFA2_MIN_EXPORT_LENGTH (128)

/* Min length of the RX packet. */
#define _BCM_XGS5_IFA2_MIN_RX_PKT_LENGTH (128)

/* Max possible length of the export packet. */
#define _BCM_XGS5_IFA2_MAX_EXPORT_PKT_LENGTH (9000)

/* IFA Record is variable length, data size should be 0xFFFF */
#define _BCM_XGS5_IFA2_TEMPLATE_VARIABLE_LENGTH    0xFFFF

/* IFA BASE AND MD HEADER */
#define _BCM_XGS5_IFA2_TEMPLATE_HEADERS_LENGTH    0x8

/* IFA IPFIX Template ID */
#define _BCM_XGS5_IFA2_TEMPLATE_ID   (1)

/* IFA2 app SDK messaging version. */
#define IFA2_SDK_VERSION (0)

/* Minimum Eapp messaging version expected. */
#define IFA2_UC_MIN_VERSION (0)

/* Set Id used when transmiting template sets */
#define IFA2_TEMPLATE_XMIT_SET_ID 2

/* Size of the template set header */
#define _BCM_XGS_IFA2_SET_HDR_LENGTH 4

/* Size of the IPFIX header */
#define _BCM_XGS_IFA2_IPFIX_MSG_HDR_LENGTH 16

/* Length of the L2 CRC field */
#define _BCM_XGS_IFA2_L2_CRC_LENGTH 4

/* Invalid FW core */
#define _BCM_XGS5_IFA2_INVALID_FW_CORE (-1)

/* IFA global information. */
typedef struct _bcm_xgs5_ifa2_info_s {
    /* The core on which IFA is running. */
    int uC;

    /* Max length of the export pkt. */
    uint16 max_export_pkt_length;

    /* Max length of the Rx pkt. */
    uint16 max_rx_pkt_length;

    /* Maximum length of the Rx packet that needs to be exported. */
    uint16 rx_pkt_export_max_length;

    /* DMA Buffer used for messaging between SDK and UKERNEL. */
    uint8 *dma_buffer;

    /* Length of the DMA buffer */
    int dma_buffer_len;

    /* 1 - Collector is attached. */
    uint8 collector_attached;

    /* Collector ID. */
    bcm_collector_t collector_id;

    /* Export profile ID. */
    int export_profile_id;

    /* 1 - Template status. */
    uint8 template_created;

    /* Template ID. */
    bcm_ifa_export_template_t template_id;
} _bcm_xgs5_ifa2_info_t;

static _bcm_xgs5_ifa2_info_t *ifa2_global_info[BCM_MAX_NUM_UNITS] = {0};

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int _bcm_xgs5_ifa2_scache_alloc(int unit, int create);
STATIC int _bcm_xgs5_ifa2_wb_init_status_recover(int unit, int *init);
STATIC int _bcm_xgs5_ifa2_cfg_wb_recover(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#define _BCM_IFA_INVALID_COLLECTOR_ID 0xFFFF
#define _BCM_IFA_INVALID_EXPORT_PROIFILE_ID 0xFFFF

/*
 * Function:
 *      _bcm_xgs5_ifa2_convert_uc_error
 * Purpose:
 *      Converts uController error code to corresponding BCM error code.
 * Parameters:
 *      uc_rv  - (IN) uController error code to convert.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes:
 */
STATIC int
_bcm_xgs5_ifa2_convert_uc_error(uint32 uc_rv)
{
    int rv = BCM_E_NONE;

    switch(uc_rv) {
    case SHR_IFA2_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_IFA2_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_IFA2_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_IFA2_UC_E_UNIT:
        rv = BCM_E_UNIT;
        break;
    case SHR_IFA2_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_IFA2_UC_E_EMPTY:
        rv = BCM_E_EMPTY;
        break;
    case SHR_IFA2_UC_E_FULL:
        rv = BCM_E_FULL;
        break;
    case SHR_IFA2_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_IFA2_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_IFA2_UC_E_TIMEOUT:
        rv = BCM_E_TIMEOUT;
        break;
    case SHR_IFA2_UC_E_BUSY:
        rv = BCM_E_BUSY;
        break;
    case SHR_IFA2_UC_E_FAIL:
        rv = BCM_E_FAIL;
        break;
    case SHR_IFA2_UC_E_DISABLED:
        rv = BCM_E_DISABLED;
        break;
    case SHR_IFA2_UC_E_BADID:
        rv = BCM_E_BADID;
        break;
    case SHR_IFA2_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_IFA2_UC_E_CONFIG:
        rv = BCM_E_CONFIG;
        break;
    case SHR_IFA2_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    case SHR_IFA2_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    case SHR_IFA2_UC_E_PORT:
        rv = BCM_E_PORT;
        break;
    default:
        rv = BCM_E_INTERNAL;
        break;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_msg_send_receive
 * Purpose:
 *      Sends given IFA2 control message to the uController.
 *      Receives and verifies expected reply.
 *      Performs DMA operation if required.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      s_subclass  - (IN) BFD message subclass.
 *      s_len       - (IN) Value for 'len' field in message struct.
 *                         Length of buffer to flush if DMA send is required.
 *      s_data      - (IN) Value for 'data' field in message struct.
 *                         Ignored if message requires a DMA send/receive
 *                         operation.
 *      r_subclass  - (IN) Expected reply message subclass.
 *      r_len       - (OUT) Returns value in 'len' reply message field.
 *      timeout     - (INT) Time to wait for message ack from uC.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_xgs5_ifa2_msg_send_receive(int unit, uint8 s_subclass,
                                uint16 s_len, uint32 s_data,
                                uint8 r_subclass, uint16 *r_len,
                                sal_usecs_t timeout)
{
    int rv;
    mos_msg_data_t send, reply;
    uint32 uc_rv;
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    uint8 *dma_buffer;
    int dma_buffer_len;

    dma_buffer = ifa2_info->dma_buffer;
    dma_buffer_len = ifa2_info->dma_buffer_len;

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_IFA2;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, ifa2_info->uC,
                                      &send, &reply,
                                      timeout);
    if (rv != SOC_E_NONE){
        return BCM_E_INTERNAL;
    }

    /* Convert IFA2 uController error code to BCM */
    uc_rv  = bcm_ntohl(reply.s.data);
    rv     = _bcm_xgs5_ifa2_convert_uc_error(uc_rv);

    *r_len = bcm_ntohs(reply.s.len);

    /*Check reply class and subclass*/
    if((rv == SOC_E_NONE) && ((reply.s.mclass != MOS_MSG_CLASS_IFA2) ||
        (reply.s.subclass != r_subclass)))
    {
        return BCM_E_INTERNAL;
    }
    return rv;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_uc_appl_init
 * Purpose:
 *      Send the application init message to the IFA app.
 * Parameters:
 *      unit       - (IN)  BCM device number
 * Returns:
 *      1 - Application init success.
 *      0 - Not successful
 */
STATIC
int _bcm_xgs5_ifa2_uc_appl_init(int unit)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    int uC;
    int rv;

    for (uC = 0; uC < SOC_INFO(unit).num_ucs; uC++) {
        if (!soc_uc_in_reset(unit, uC)) {
            rv = soc_cmic_uc_appl_init(unit, uC, MOS_MSG_CLASS_IFA2,
                                       SHR_IFA2_MAX_UC_MSG_TIMEOUT,
                                       IFA2_SDK_VERSION, IFA2_UC_MIN_VERSION,
                                       _bcm_ifa2_appl_callback, NULL);
            if (SOC_E_NONE == rv) {
                ifa2_info->uC = uC;
                return 1;
            }
        }
    }
    return 0;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_uc_msg_init
 * Purpose:
 *      Send the init message to the IFA app.
 * Parameters:
 *      unit       - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_xgs5_ifa2_uc_msg_init(int unit)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    shr_ifa2_msg_ctrl_init_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    uint64 init_time_64;
    uint32 init_time_32;

    sal_memset(&msg, 0, sizeof(msg));

    /* Get the current time in seconds and convert to msecs. */
    init_time_32 = sal_time();
    COMPILER_64_SET(init_time_64, 0, init_time_32);
    COMPILER_64_UMUL_32(init_time_64, 1000);

    COMPILER_64_TO_32_HI(msg.init_time_hi, init_time_64);
    COMPILER_64_TO_32_LO(msg.init_time_lo, init_time_64);

    msg.max_export_pkt_length = ifa2_info->max_export_pkt_length;
    msg.max_rx_pkt_length     = ifa2_info->max_rx_pkt_length;

    buffer_req = ifa2_info->dma_buffer;
    buffer_ptr = _bcm_xgs5_ifa2_msg_ctrl_init_pack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_IFA2_INIT,
                                         buffer_len, 0,
                                         MOS_MSG_SUBCLASS_IFA2_INIT_REPLY,
                                         &reply_len,
                                         SHR_IFA2_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_uc_msg_shutdown
 * Purpose:
 *      Send the shutdown message to the IFA app.
 * Parameters:
 *      unit       - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_xgs5_ifa2_uc_msg_shutdown(int unit)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    int status = 0;
    uint16 reply_len = 0;

    SOC_IF_ERROR_RETURN(soc_uc_status(unit, ifa2_info->uC, &status));
    if (status) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_ifa2_msg_send_receive(unit,
                                             MOS_MSG_SUBCLASS_IFA2_SHUTDOWN,
                                             0, 0,
                                             MOS_MSG_SUBCLASS_IFA2_SHUTDOWN_REPLY,
                                             &reply_len,
                                             SHR_IFA2_MAX_UC_MSG_TIMEOUT));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_uc_msg_md_fmt_set
 * Purpose:
 *      Send the metadata format set message to the app
 * Parameters:
 *      unit       - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC
int _bcm_xgs5_ifa2_uc_msg_md_fmt_set(int unit)
{
    shr_ifa2_msg_ctrl_md_format_set_t msg;
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int i;

    sal_memset(&msg, 0, sizeof(msg));

    /* Only a single MD format is supported currently, hardcode it. */
    msg.flags |= SHR_IFA2_MSG_CTRL_MD_FORMAT_SET_FLAGS_LNS;
    msg.namespace = 1;

    i = 0;

    msg.elements[i] = SHR_IFA2_MD_ELEMENT_LNS_DEVICE_ID;
    msg.element_widths[i] = 24;
    i++;

    msg.elements[i] = SHR_IFA2_MD_ELEMENT_IP_TTL;
    msg.element_widths[i] = 8;
    i++;

    msg.elements[i] = SHR_IFA2_MD_ELEMENT_EGRESS_PORT_SPEED;
    msg.element_widths[i] = 4;
    i++;

    msg.elements[i] = SHR_IFA2_MD_ELEMENT_CONGESTION;
    msg.element_widths[i] = 2;
    i++;

    msg.elements[i] = SHR_IFA2_MD_ELEMENT_QUEUE_ID;
    msg.element_widths[i] = 6;
    i++;

    msg.elements[i] = SHR_IFA2_MD_ELEMENT_RX_TIMESTAMP_SEC;
    msg.element_widths[i] = 20;
    i++;

    msg.elements[i] = SHR_IFA2_MD_ELEMENT_EGRESS_SYSTEM_PORT;
    msg.element_widths[i] = 16;
    i++;

    msg.elements[i] = SHR_IFA2_MD_ELEMENT_INGRESS_SYSTEM_PORT;
    msg.element_widths[i] = 16;
    i++;

    msg.elements[i] = SHR_IFA2_MD_ELEMENT_RX_TIMESTAMP_NANO_SEC;
    msg.element_widths[i] = 32;
    i++;

    msg.elements[i] = SHR_IFA2_MD_ELEMENT_RESIDENCE_TIME_NANO_SEC;
    msg.element_widths[i] = 32;
    i++;

    msg.elements[i] = SHR_IFA2_MD_ELEMENT_OPAQUE_DATA_0;
    msg.element_widths[i] = 32;
    i++;

    msg.elements[i] = SHR_IFA2_MD_ELEMENT_OPAQUE_DATA_1;
    msg.element_widths[i] = 32;
    i++;

    msg.elements[i] = SHR_IFA2_MD_ELEMENT_OPAQUE_DATA_2;
    msg.element_widths[i] = 32;
    i++;

    msg.num_elements = i;

    buffer_req = ifa2_info->dma_buffer;
    buffer_ptr = _bcm_xgs5_ifa2_msg_ctrl_md_format_set_pack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_IFA2_MD_FORMAT_SET,
                                         buffer_len, 0,
                                         MOS_MSG_SUBCLASS_IFA2_MD_FORMAT_SET_REPLY,
                                         &reply_len,
                                         SHR_IFA2_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_uc_msg_cfg_update
 * Purpose:
 *      Update IFA run time configuration.
 * Parameters:
 *      unit         - (IN) BCM device number.
 *      num_params   - (IN) Number of parameters to update.
 *      params       - (IN) Parameters to update.
 *      num_values   - (IN) Number of values per parameter.
 *      values       - (IN) Values per parameter.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC
int _bcm_xgs5_ifa2_uc_msg_cfg_update(int unit, uint16 num_params,
                                     uint32 *params, uint16 *num_values,
                                     uint32 values[][SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_MAX_VALUES])
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    shr_ifa2_msg_ctrl_config_update_t msg;
    int i, j;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(&msg, 0, sizeof(msg));

    msg.num_params = num_params;
    for (i = 0; i < num_params; i++) {
        msg.params[i] = params[i];
        msg.num_values[i] = num_values[i];
        for (j = 0; j < num_values[i]; j++) {
            msg.values[i][j] = values[i][j];
        }
    }

    buffer_req = ifa2_info->dma_buffer;
    buffer_ptr = _bcm_xgs5_ifa2_msg_ctrl_config_update_pack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_IFA2_CONFIG_UPDATE,
                                         buffer_len, 0,
                                         MOS_MSG_SUBCLASS_IFA2_CONFIG_UPDATE_REPLY,
                                         &reply_len,
                                         SHR_IFA2_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ifa2_udp_header_get(bcm_collector_info_t *collector,
                              shr_udp_header_t *udp)
{
    /* Set UDP */
    sal_memset(udp, 0, sizeof(*udp));
    udp->src = collector->udp.src_port;
    udp->dst = collector->udp.dst_port;
    /* Do not set UDP length since this will be filled by
     * EAPP.
     */
    udp->length = 0;
    udp->sum = 0;    /* Calculated later */

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ifa2_ipv6_headers_get(bcm_collector_info_t *collector,
                               uint16 *etype,
                               shr_ipv6_header_t *ipv6)
{
    sal_memset(ipv6, 0, sizeof(*ipv6));

    *etype = SHR_L2_ETYPE_IPV6;

    ipv6->version  = SHR_IPV6_VERSION;
    ipv6->next_header = SHR_IP_PROTOCOL_UDP;
    ipv6->t_class = collector->ipv6.traffic_class;
    ipv6->hop_limit = collector->ipv6.hop_limit;
    ipv6->f_label = collector->ipv6.flow_label;
    /* Dont fill in length since length will be calculated by
     * EAPP.
     */
    ipv6->p_length   = 0;

    /* Destination and Source IP address */
    sal_memcpy(ipv6->src, collector->ipv6.src_ip, sizeof(bcm_ip6_t));
    sal_memcpy(ipv6->dst, collector->ipv6.dst_ip, sizeof(bcm_ip6_t));

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ifa2_ipv4_headers_get(bcm_collector_info_t *collector,
                               uint16 *etype,
                               shr_ipv4_header_t *ipv4)
{
    sal_memset(ipv4, 0, sizeof(*ipv4));

    *etype = SHR_L2_ETYPE_IPV4;

    ipv4->version  = SHR_IPV4_VERSION;
    ipv4->h_length = SHR_IPV4_HEADER_LENGTH_WORDS;
    ipv4->dscp     = collector->ipv4.dscp;
    /* Dont fill in length since length will be calculated by
     * EAPP.
     */
    ipv4->ttl      = collector->ipv4.ttl;
    ipv4->protocol = SHR_IP_PROTOCOL_UDP;
    ipv4->src      = collector->ipv4.src_ip;
    ipv4->dst      = collector->ipv4.dst_ip;

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ifa2_l2_header_get(bcm_collector_info_t *collector,
                              uint16 etype, shr_l2_header_t *l2)
{
    sal_memset(l2, 0, sizeof(*l2));

    /* Destination and Source MAC */
    sal_memcpy(l2->dst_mac, collector->eth.dst_mac, SHR_MAC_ADDR_LENGTH);
    sal_memcpy(l2->src_mac, collector->eth.src_mac, SHR_MAC_ADDR_LENGTH);

    /*
     * VLAN Tag(s)
     */
    if (collector->eth.vlan_tag_structure == BCM_COLLECTOR_ETH_HDR_UNTAGGED) {
        /* Set to 0 to indicate untagged */
        l2->outer_vlan_tag.tpid = 0;
        l2->inner_vlan_tag.tpid = 0;
    } else {
        /* Fill outer vlan */
        if ((collector->eth.vlan_tag_structure == BCM_COLLECTOR_ETH_HDR_SINGLE_TAGGED) ||
            (collector->eth.vlan_tag_structure == BCM_COLLECTOR_ETH_HDR_DOUBLE_TAGGED))
        {
            l2->outer_vlan_tag.tpid     = collector->eth.outer_tpid;
            l2->outer_vlan_tag.tci.prio = BCM_VLAN_CTRL_PRIO(collector->eth.outer_vlan_tag);
            l2->outer_vlan_tag.tci.cfi  = BCM_VLAN_CTRL_CFI(collector->eth.outer_vlan_tag);
            l2->outer_vlan_tag.tci.vid  = BCM_VLAN_CTRL_ID(collector->eth.outer_vlan_tag);
        } else {
            return BCM_E_PARAM;
        }
        /* Fill inner vlan if double tagged */
        if (collector->eth.vlan_tag_structure == BCM_COLLECTOR_ETH_HDR_DOUBLE_TAGGED) {
            l2->inner_vlan_tag.tpid     = collector->eth.inner_tpid;
            l2->inner_vlan_tag.tci.prio = BCM_VLAN_CTRL_PRIO(collector->eth.inner_vlan_tag);
            l2->inner_vlan_tag.tci.cfi  = BCM_VLAN_CTRL_CFI(collector->eth.inner_vlan_tag);
            l2->inner_vlan_tag.tci.vid  = BCM_VLAN_CTRL_ID(collector->eth.inner_vlan_tag);
        } else {
            l2->inner_vlan_tag.tpid = 0;  /* Set to 0 to indicate not inner tagged */
        }
    }

    /* Ether Type */
    l2->etype = etype;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_uc_msg_collector_create_build
 * Purpose:
 *      Construct the collector create message.
 * Parameters:
 *      unit              - (IN)  BCM device number.
 *      collector         - (IN)  Collector Info.
 *      export_profile    - (IN)  Export profile Info.
 *      msg               - (OUT) Collector create message.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_xgs5_ifa2_uc_msg_collector_create_build(int unit,
                                             bcm_collector_info_t *collector,
                                             bcm_collector_export_profile_t *export_profile,
                                             shr_ifa2_msg_ctrl_collector_create_t *msg)
{
    uint8 *buf = msg->encap;
    uint8 *cur_ptr;
    uint16 etype = 0;
    shr_udp_header_t udp;
    shr_ipv4_header_t ipv4;
    shr_ipv6_header_t ipv6;
    shr_l2_header_t l2;
    uint16 ip_offset = 0;
    uint16 udp_offset = 0;

    sal_memset(msg, 0, sizeof(shr_ifa2_msg_ctrl_collector_create_t));

    sal_memset(&udp, 0, sizeof(udp));
    sal_memset(&ipv4, 0, sizeof(ipv4));
    sal_memset(&ipv6, 0, sizeof(ipv6));
    sal_memset(&l2, 0, sizeof(l2));

    /*
     * Get necessary headers/labels information.
     *
     * Following order is important since some headers/labels
     * may depend on previous header/label information.
     */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_udp_header_get(collector, &udp));

    if (collector->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_ifa2_ipv4_headers_get(collector, &etype, &ipv4));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_ifa2_ipv6_headers_get(collector, &etype, &ipv6));
    }

    /* Always build L2 Header */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_l2_header_get(collector, etype, &l2));

    /*
     * Pack header/labels into given buffer (network packet format).
     *
     * Following packing order must be followed to correctly
     * build the packet encapsulation.
     */
    cur_ptr = buf;

    /* L2 Header is always present */
    cur_ptr = shr_l2_header_pack(cur_ptr, &l2);

    /*
     * IP offset
     */
    ip_offset = cur_ptr - buf;
    if (collector->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        cur_ptr = shr_ipv4_header_pack(cur_ptr, &ipv4);
    } else {
        cur_ptr = shr_ipv6_header_pack(cur_ptr, &ipv6);
    }

    /*
     * UDP offset
     */
    udp_offset = cur_ptr - buf;
    cur_ptr = shr_udp_header_pack(cur_ptr, &udp);

    /* Set export pkt encapsulation length */
    msg->encap_length = cur_ptr - buf;

    /* Set export pkt IP base offset */
    msg->ip_offset = ip_offset;

    /* Set export pkt IP Base checksum */
    if (collector->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        msg->ip_base_checksum =
            shr_initial_chksum_get(SHR_IPV4_HEADER_LENGTH, buf + ip_offset);
    }

    /* Set export pkt UDP Base checksum */
    if (collector->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        msg->udp_base_checksum =
            shr_udp_initial_checksum_get(0, &ipv4, NULL, NULL, &udp);
    } else {
        msg->udp_base_checksum =
            shr_udp_initial_checksum_get(1, NULL, &ipv6, NULL, &udp);
    }

    if (collector->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        msg->flags |= SHR_IFA2_MSG_CTRL_COLLECTOR_FLAGS_L3_TYPE_IPV4;
    }
    if (collector->udp.flags & BCM_COLLECTOR_UDP_FLAGS_CHECKSUM_ENABLE) {
        msg->flags |= SHR_IFA2_MSG_CTRL_COLLECTOR_FLAGS_UDP_CHECKSUM_ENABLE;
    }

    /* Set export pkt UDP base offset */
    msg->udp_offset = udp_offset;

    /* Copy over the export profile fields. */
    msg->mtu = export_profile->max_packet_length;
    if (export_profile->flags & BCM_COLLECTOR_EXPORT_PROFILE_FLAGS_USE_NUM_RECORDS) {
        msg->flags |= SHR_IFA2_MSG_CTRL_COLLECTOR_FLAGS_USE_NUM_RECORDS;
        msg->num_records = export_profile->num_records;
    }

    /* Observation domain ID */
    msg->observation_domain_id = collector->ipfix.observation_domain_id;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_uc_msg_template_set
 * Purpose:
 *      Send the template message to uC.
 * Parameters:
 *      unit        - (IN) BCM device number.
 *      msg         - (IN) Template create message.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_xgs5_ifa2_uc_msg_template_set(int unit,
                                   shr_ifa2_msg_export_template_info_t *msg)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    buffer_req = ifa2_info->dma_buffer;
    buffer_ptr = _bcm_xgs5_ifa2_msg_ctrl_template_pack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_IFA2_TEMPLATE_CREATE,
                                         buffer_len, 0,
                                         MOS_MSG_SUBCLASS_IFA2_TEMPLATE_CREATE_REPLY,
                                         &reply_len,
                                         SHR_IFA2_MAX_UC_MSG_TIMEOUT));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_uc_msg_template_get
 * Purpose:
 *      Get template message from uC.
 * Parameters:
 *      unit        - (IN)  BCM device number.
 *      msg         - (OUT) Template get message.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_xgs5_ifa2_uc_msg_template_get(int unit,
                                   shr_ifa2_msg_export_template_info_t *msg)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_IFA2_TEMPLATE_GET,
                                         0, 0,
                                         MOS_MSG_SUBCLASS_IFA2_TEMPLATE_GET_REPLY,
                                         &reply_len,
                                         SHR_IFA2_MAX_UC_MSG_TIMEOUT));

    buffer_req = ifa2_info->dma_buffer;
    buffer_ptr = _bcm_xgs5_ifa2_msg_ctrl_template_unpack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_uc_msg_template_delete
 * Purpose:
 *      Send the template delete message to uC.
 * Parameters:
 *      unit        - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_xgs5_ifa2_uc_msg_template_delete(int unit)
{
    uint16 reply_len;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_IFA2_TEMPLATE_DELETE,
                                         0, 0,
                                         MOS_MSG_SUBCLASS_IFA2_TEMPLATE_DELETE_REPLY,
                                         &reply_len,
                                         SHR_IFA2_MAX_UC_MSG_TIMEOUT));

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_xgs5_ifa2_uc_msg_collector_create
 * Purpose:
 *      Send the collector create message to uC.
 * Parameters:
 *      unit        - (IN) BCM device number.
 *      msg         - (IN) Collector create message.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_xgs5_ifa2_uc_msg_collector_create(int unit,
                                       shr_ifa2_msg_ctrl_collector_create_t *msg)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    buffer_req = ifa2_info->dma_buffer;
    buffer_ptr = _bcm_xgs5_ifa2_msg_ctrl_collector_create_pack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_IFA2_COLLECTOR_CREATE,
                                         buffer_len, 0,
                                         MOS_MSG_SUBCLASS_IFA2_COLLECTOR_CREATE_REPLY,
                                         &reply_len,
                                         SHR_IFA2_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_uc_msg_collector_delete
 * Purpose:
 *      Send the collector delete message to uC.
 * Parameters:
 *      unit        - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_xgs5_ifa2_uc_msg_collector_delete(int unit)
{
    uint16 reply_len;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_IFA2_COLLECTOR_DELETE,
                                         0, 0,
                                         MOS_MSG_SUBCLASS_IFA2_COLLECTOR_DELETE_REPLY,
                                         &reply_len,
                                         SHR_IFA2_MAX_UC_MSG_TIMEOUT));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_eapp_template_encap_build_pack
 * Purpose:
 *      Build the template set encap
 * Parameters:
 *      enterprise_number - (IN)   IPFIX enterprise number.
 *      template_info     - (IN)   Template Information
 *      encap_length      - (OUT)  Encap length
 *      encap             - (OUT)  Encap
 * Returns:
 *      None
 */
STATIC void
_bcm_xgs5_ifa2_eapp_template_encap_build_pack(int unit,
                                              uint32  enterprise_number,
                                              shr_ifa2_msg_export_template_info_t *template_info,
                                              uint16 *encap_length, uint8 *buf)
{
    uint8  *cur_ptr = buf;
    uint16  element_id;
    int     i;

    /*
     * +----------------------+------------------+
     * |     Bits 0...15      |   Bits 16...31   |
     * +----------------------+------------------+
     * | Set ID = 2(Template) |    Set Length    |
     * +----------------------+------------------+
     * |     Template ID      | Number of fields |
     * +----------------------+------------------+
     * |        Type          |   Field Length   |
     * +----------------------+------------------+
     * |        Type          |   Field Length   |
     * +----------------------+------------------+
     * |        Type          |   Field Length   |
     * +----------------------+------------------+
     */

    SHR_PKT_PACK_U16(cur_ptr, IFA2_TEMPLATE_XMIT_SET_ID);

    /* Skip over length */
    cur_ptr += 2;

    SHR_PKT_PACK_U16(cur_ptr, template_info->set_id);
    SHR_PKT_PACK_U16(cur_ptr, template_info->num_export_elements);

    for (i = 0; i < template_info->num_export_elements; i++) {
        if (template_info->elements[i].enterprise == 0) {
            /* Non enterprise element, format below */
            /*
             * +-+--------------------+------------------+
             * |0|      1...15        |     16...31      |
             * +-+--------------------+------------------+
             * |0|      Type          |   Field Length   |
             * +----------------------+------------------+
             */
            SHR_PKT_PACK_U16(cur_ptr, template_info->elements[i].id);
            SHR_PKT_PACK_U16(cur_ptr, template_info->elements[i].data_size);
        } else {
            /* Enterprise specific */
            /*
             * +-+--------------------+------------------+
             * |0|      1...15        |     16...31      |
             * +-+--------------------+------------------+
             * |1|      Type          |   Field Length   |
             * +----------------------+------------------+
             * |             Enterprise number           |
             * +----------------------+------------------+
             */
            /* Set the enterprise bit */
            element_id = (1 << 15) | template_info->elements[i].id;
            SHR_PKT_PACK_U16(cur_ptr, element_id);
            SHR_PKT_PACK_U16(cur_ptr, template_info->elements[i].data_size);
            SHR_PKT_PACK_U32(cur_ptr, enterprise_number);
        }
    }

    *encap_length = cur_ptr - buf;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_uc_msg_stats_set
 * Purpose:
 *      Send the stats set message to uC.
 * Parameters:
 *      unit        - (IN) BCM device number.
 *      stats       - (IN) Statistics.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_xgs5_ifa2_uc_msg_stats_set(int unit, bcm_ifa_stat_info_t *stats)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    shr_ifa2_msg_ctrl_stats_set_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(&msg, 0, sizeof(msg));

    msg.rx_pkt_cnt_lo = stats->rx_pkt_cnt;
    msg.rx_pkt_excess_rate_discard_lo = stats->rx_pkt_excess_rate_drop_count;
    msg.rx_pkt_no_export_config_discard_lo = stats->ifa_collector_not_present_drop;
    msg.rx_pkt_current_length_exceed_discard_lo = stats->ifa_int_hdr_len_invalid_drop;
    msg.rx_pkt_length_exceed_max_discard_lo = stats->rx_pkt_length_exceed_max_drop_count;
    msg.rx_pkt_parse_error_discard_lo = stats->rx_pkt_parse_error_drop_count;
    msg.rx_pkt_unknown_namespace_discard_lo = stats->rx_pkt_unknown_namespace_drop_count;
    msg.tx_pkt_cnt_lo = stats->tx_pkt_cnt;
    msg.tx_record_cnt_lo = stats->tx_record_count;
    msg.tx_pkt_failure_cnt_lo = stats->tx_pkt_failure_count;
    msg.rx_pkt_no_template_config_discard_lo = stats->ifa_template_not_present_drop;

    buffer_req = ifa2_info->dma_buffer;
    buffer_ptr = _bcm_xgs5_ifa2_msg_ctrl_stats_set_pack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_IFA2_STATS_SET,
                                         buffer_len, 0,
                                         MOS_MSG_SUBCLASS_IFA2_STATS_SET_REPLY,
                                         &reply_len,
                                         SHR_IFA2_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_uc_msg_stats_get
 * Purpose:
 *      Send the stats get message to uC.
 * Parameters:
 *      unit        - (IN)  BCM device number.
 *      stats       - (OUT) Statistics.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_xgs5_ifa2_uc_msg_stats_get(int unit, bcm_ifa_stat_info_t *stats)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    shr_ifa2_msg_ctrl_stats_get_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(&msg, 0, sizeof(msg));

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_IFA2_STATS_GET,
                                         0, 0,
                                         MOS_MSG_SUBCLASS_IFA2_STATS_GET_REPLY,
                                         &reply_len,
                                         SHR_IFA2_MAX_UC_MSG_TIMEOUT));

    buffer_req = ifa2_info->dma_buffer;
    buffer_ptr = _bcm_xgs5_ifa2_msg_ctrl_stats_get_unpack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        return BCM_E_INTERNAL;
    }

    stats->rx_pkt_cnt = msg.rx_pkt_cnt_lo;
    stats->rx_pkt_excess_rate_drop_count = msg.rx_pkt_excess_rate_discard_lo;
    stats->ifa_collector_not_present_drop = msg.rx_pkt_no_export_config_discard_lo;
    stats->ifa_int_hdr_len_invalid_drop = msg.rx_pkt_current_length_exceed_discard_lo;
    stats->rx_pkt_length_exceed_max_drop_count = msg.rx_pkt_length_exceed_max_discard_lo;
    stats->rx_pkt_parse_error_drop_count = msg.rx_pkt_parse_error_discard_lo;
    stats->rx_pkt_unknown_namespace_drop_count = msg.rx_pkt_unknown_namespace_discard_lo;
    stats->tx_pkt_cnt = msg.tx_pkt_cnt_lo;
    stats->tx_record_count = msg.tx_record_cnt_lo;
    stats->tx_pkt_failure_count = msg.tx_pkt_failure_cnt_lo;
    stats->ifa_template_not_present_drop = msg.rx_pkt_no_template_config_discard_lo;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_collector_attach
 * Purpose:
 *      Attach collector and export profile.
 * Parameters:
 *      unit              - (IN) BCM device number
 *      collector_id      - (IN) Collector ID
 *      export_profile_id - (IN) Export profile ID.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_ifa2_collector_attach(int unit,
                                    bcm_collector_t collector_id,
                                    int export_profile_id,
                                    bcm_ifa_export_template_t template_id)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    bcm_collector_info_t collector;
    bcm_collector_export_profile_t export_profile;
    shr_ifa2_msg_ctrl_collector_create_t msg;

    if (ifa2_info == NULL) {
        return BCM_E_INIT;
    }

    if (ifa2_info->collector_attached != 0) {
        return BCM_E_EXISTS;
    }

    /* Verify if template exists */
    if ((!ifa2_info->template_created) ||
        ifa2_info->template_id != template_id) {
        return BCM_E_PARAM;
    }

    if (_bcm_esw_collector_check_user_is_other(unit,
                                               collector_id,
                                               _BCM_COLLECTOR_EXPORT_APP_IFA2_IPFIX)) {
        return (BCM_E_CONFIG);
    }

    /* Get and validate collector info */
    bcm_collector_info_t_init(&collector);
    BCM_IF_ERROR_RETURN(bcm_esw_collector_get(unit, collector_id, &collector));

    if ((collector.transport_type != bcmCollectorTransportTypeIpv4Udp) &&
        (collector.transport_type != bcmCollectorTransportTypeIpv6Udp)) {
        return BCM_E_UNAVAIL;
    }

    /* Get and validate export profile info */
    bcm_collector_export_profile_t_init(&export_profile);
    BCM_IF_ERROR_RETURN
        (bcm_esw_collector_export_profile_get(unit,
                                              export_profile_id,
                                              &export_profile));

    if (export_profile.wire_format != bcmCollectorWireFormatIpfix) {
        return BCM_E_PARAM;
    }
    if (!export_profile.flags & BCM_COLLECTOR_EXPORT_PROFILE_FLAGS_USE_NUM_RECORDS) {
        if (export_profile.max_packet_length < _BCM_XGS5_IFA2_MIN_EXPORT_LENGTH) {
            return BCM_E_PARAM;
        }
        if (export_profile.max_packet_length > ifa2_info->max_export_pkt_length) {
            return BCM_E_PARAM;
        }
    }
    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_uc_msg_collector_create_build(unit, &collector,
                                                      &export_profile,
                                                      &msg));

    if (!(export_profile.flags &
          BCM_COLLECTOR_EXPORT_PROFILE_FLAGS_USE_NUM_RECORDS)) {
        /* Check if there is space to insert at least the headers  */
        if ((msg.encap_length +
                    _BCM_XGS_IFA2_IPFIX_MSG_HDR_LENGTH   +
                    _BCM_XGS_IFA2_SET_HDR_LENGTH         +
                    _BCM_XGS_IFA2_L2_CRC_LENGTH)  > msg.mtu) {
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_uc_msg_collector_create(unit, &msg));

    /*
     * Update the user and increment the ref counts for the colector and
     * export profile.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_collector_user_update(unit,
                                        collector_id,
                                        _BCM_COLLECTOR_EXPORT_APP_IFA2_IPFIX));
    BCM_IF_ERROR_RETURN
        (_bcm_esw_collector_ref_count_update(unit, collector_id, 1));
    BCM_IF_ERROR_RETURN
        (_bcm_esw_collector_export_profile_ref_count_update(unit,
                                                            export_profile_id,
                                                            1));
    ifa2_info->collector_attached = 1;
    ifa2_info->collector_id       = collector_id;
    ifa2_info->export_profile_id  = export_profile_id;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_collector_attach_get
 * Purpose:
 *      Get the attached collector and export profile.
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      collector_id      - (OUT) Collector ID
 *      export_profile_id - (OUT) Export profile ID.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_ifa2_collector_attach_get(int unit,
                                        bcm_collector_t *collector_id,
                                        int *export_profile_id,
                                        bcm_ifa_export_template_t *template_id)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];

    if (ifa2_info == NULL) {
        return BCM_E_INIT;
    }

    if (ifa2_info->collector_attached == 0) {
        return BCM_E_NOT_FOUND;
    }

    *collector_id      = ifa2_info->collector_id;
    *export_profile_id = ifa2_info->export_profile_id;
    *template_id       = ifa2_info->template_id;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_collector_detach
 * Purpose:
 *      Detach collector and export profile.
 * Parameters:
 *      unit              - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_ifa2_collector_detach(int unit,
                                    bcm_collector_t collector_id,
                                    int export_profile_id,
                                    bcm_ifa_export_template_t template_id)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];

    if (ifa2_info == NULL) {
        return BCM_E_INIT;
    }

    if (ifa2_info->collector_attached == 0) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_uc_msg_collector_delete(unit));

    /* Decrement the ref counts */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_collector_ref_count_update(unit, ifa2_info->collector_id, -1));
    BCM_IF_ERROR_RETURN
        (_bcm_esw_collector_export_profile_ref_count_update(unit,
                                                            ifa2_info->export_profile_id,
                                                            -1));
    ifa2_info->collector_attached = 0;
    ifa2_info->collector_id = _BCM_IFA_INVALID_COLLECTOR_ID;
    ifa2_info->export_profile_id = _BCM_IFA_INVALID_EXPORT_PROIFILE_ID;

    return BCM_E_NONE;
}

/*
 * Create an IFA export template.
 * Parameters:
 * unit                     - (IN)   BCM device number
 * options                  - (IN)   Template create options
 * id                       - (IN)   Template Id
 * set_id                   - (IN)   set_id to be used for the template
 * num_export_elements      - (IN)   Number of elements in the template
 * list_of_export_elements  - (IN)   Export element list
 */
int _bcm_xgs5_ifa2_export_template_create(int unit,
                                          uint32 options,
                                          bcm_ifa_export_template_t *id,
                                          uint16 set_id,
                                          int num_export_elements,
                                          bcm_ifa_export_element_info_t *list_of_export_elements)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    shr_ifa2_msg_export_template_info_t msg;
    int i;
    uint8  ifa_headers_present = 0, ifa_md_stack_present = 0, ifa_pkt_data_present = 0;

    if (ifa2_info == NULL) {
        return BCM_E_INIT;
    }

    /* Validate set id. Should be between 256 and 65535 */
    if (set_id < 256) {
        return BCM_E_PARAM;
    }

    /* All the three elements are needed for proper functionality of
     * IFA2.0 application and hence this check is mandatory
     */
    if (num_export_elements < bcmIfaExportElementTypeCount ||
        num_export_elements > bcmIfaExportElementTypeCount) {
        return BCM_E_PARAM;
    }

    if (options & BCM_IFA_EXPORT_TEMPLATE_WITH_ID) {
        if (*id != _BCM_XGS5_IFA2_TEMPLATE_ID) {
            return BCM_E_PARAM;
        }

        ifa2_info->template_id = *id;
    } else {
        ifa2_info->template_id = *id = _BCM_XGS5_IFA2_TEMPLATE_ID;
    }

    sal_memset(&msg, 0, sizeof(shr_ifa2_msg_export_template_info_t));
    msg.template_id                  = *id;
    msg.set_id                       = set_id;
    msg.num_export_elements          = num_export_elements;

    for (i = 0; i < num_export_elements; i++) {
        if (list_of_export_elements[i].element == bcmIfaExportElementTypeIFAHeaders) {
            msg.elements[i].element     = SHR_IFA2_TEMPLATE_INFO_ELEM_IFA_HEADERS;
            msg.elements[i].data_size   = _BCM_XGS5_IFA2_TEMPLATE_HEADERS_LENGTH;
            ifa_headers_present = 1;
        } else if (list_of_export_elements[i].element == bcmIfaExportElementTypeMetadataStack) {
            msg.elements[i].element     = SHR_IFA2_TEMPLATE_INFO_ELEM_IFA_MD_STACK;
            msg.elements[i].data_size   = _BCM_XGS5_IFA2_TEMPLATE_VARIABLE_LENGTH;
            ifa_md_stack_present = 1;
        } else if (list_of_export_elements[i].element == bcmIfaExportElementTypePktData) {
            msg.elements[i].element     = SHR_IFA2_TEMPLATE_INFO_ELEM_IFA_PKT_DATA;
            msg.elements[i].data_size   = _BCM_XGS5_IFA2_TEMPLATE_VARIABLE_LENGTH;
            ifa_pkt_data_present = 1;
        } else {
            return BCM_E_PARAM;
        }
        if (list_of_export_elements[i].flags &
            BCM_IFA_EXPORT_ELEMENT_FLAGS_ENTERPRISE) {
            msg.elements[i].enterprise          = 1;
            msg.elements[i].id                  =
                       list_of_export_elements[i].info_elem_id;
        } else {
            msg.elements[i].enterprise = 0;
            msg.elements[i].id    = list_of_export_elements[i].info_elem_id;
        }
    }

    /* If any of the IFA2.0 export element is not present, return error */
    if (!ifa_headers_present || !ifa_md_stack_present || !ifa_pkt_data_present) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_uc_msg_template_set(unit, &msg));
    ifa2_info->template_created = 1;
    return BCM_E_NONE;
}

/*
 * Get an IFA export template.
 * unit                     - (IN)   BCM device number
 * id                       - (IN)   Template Id
 * set_id                   - (OUT)  Set Id of the template
 * max_size                 - (IN)   Size of the export element list array
 * list_of_export_elements  - (OUT)  Export element list
 * list_size                - (OUT)  Number of elements in the list
 */
int _bcm_xgs5_ifa2_export_template_get(int unit,
                                       bcm_ifa_export_template_t id,
                                       uint16 *set_id,
                                       int max_size,
                                       bcm_ifa_export_element_info_t *list_of_export_elements,
                                       int *list_size)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    shr_ifa2_msg_export_template_info_t msg;
    int i;

    if (ifa2_info == NULL) {
        return BCM_E_INIT;
    }

    /* Verify if template exists */
    if (!ifa2_info->template_created || ifa2_info->template_id != id) {
        return BCM_E_NOT_FOUND;
    }

    sal_memset(&msg, 0, sizeof(shr_ifa2_msg_export_template_info_t));
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_uc_msg_template_get(unit, &msg));

    *set_id                      = msg.set_id;
    if (msg.num_export_elements < max_size) {
        *list_size                   = msg.num_export_elements;
    } else {
        *list_size                   = max_size;
    }

    for (i = 0; i < *list_size; i++) {
        if (msg.elements[i].element == SHR_IFA2_TEMPLATE_INFO_ELEM_IFA_HEADERS) {
            list_of_export_elements[i].element = bcmIfaExportElementTypeIFAHeaders;
        } else if (msg.elements[i].element == SHR_IFA2_TEMPLATE_INFO_ELEM_IFA_MD_STACK) {
            list_of_export_elements[i].element = bcmIfaExportElementTypeMetadataStack;
        } else if (msg.elements[i].element == SHR_IFA2_TEMPLATE_INFO_ELEM_IFA_PKT_DATA) {
            list_of_export_elements[i].element = bcmIfaExportElementTypePktData;
        }
        list_of_export_elements[i].data_size = msg.elements[i].data_size;
        list_of_export_elements[i].info_elem_id = msg.elements[i].id;
        if (msg.elements[i].enterprise == 1) {
            list_of_export_elements[i].flags |= BCM_IFA_EXPORT_ELEMENT_FLAGS_ENTERPRISE;
        }
    }

    return BCM_E_NONE;
}

/*
 * Destroy an IFA export template.
 * unit   - (IN)  BCM device number
 * id     - (IN)  Template Id
 */
int _bcm_xgs5_ifa2_export_template_destroy(int unit,
                                           bcm_ifa_export_template_t id)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];

    if (ifa2_info == NULL) {
        return BCM_E_INIT;
    }

    /* Verify if template exists */
    if (!ifa2_info->template_created || ifa2_info->template_id != id) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_uc_msg_template_delete(unit));

    ifa2_info->template_created = 0;
    return BCM_E_NONE;
}


/*
 * Set the template transmit configuration.
 * unit              - (IN)  BCM device number
 * template_id       - (IN)  Template Id
 * collector_id      - (IN)  Collector Id
 * config            - (IN)  Template transmit config
 */
int _bcm_xgs5_ifa2_template_transmit_config_set(int unit,
                                                bcm_ifa_export_template_t template_id,
                                                bcm_collector_t collector_id,
                                                bcm_ifa_template_transmit_config_t *config)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    shr_ifa2_msg_ctrl_template_xmit_t   msg;
    shr_ifa2_msg_export_template_info_t template_info;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    bcm_collector_info_t collector;

    if (ifa2_info == NULL) {
        return BCM_E_INIT;
    }

    /* Verify if template exists */
    if ((!ifa2_info->template_created) ||
        ifa2_info->template_id != template_id) {
        return BCM_E_CONFIG;
    }

    if ((!ifa2_info->collector_attached) ||
        ifa2_info->collector_id != collector_id) {
        return BCM_E_CONFIG;
    }

    /* Get and validate collector info */
    bcm_collector_info_t_init(&collector);
    BCM_IF_ERROR_RETURN(bcm_esw_collector_get(unit, collector_id, &collector));

    sal_memset(&msg, 0, sizeof(shr_ifa2_msg_ctrl_template_xmit_t));
    sal_memset(&template_info, 0, sizeof(shr_ifa2_msg_export_template_info_t));
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_uc_msg_template_get(unit, &template_info));

    /* Build the template encap */
    _bcm_xgs5_ifa2_eapp_template_encap_build_pack(unit,
                                              collector.ipfix.enterprise_number,
                                              &template_info,
                                              &(msg.encap_length),
                                              msg.encap);

    msg.template_id   = template_id;
    msg.collector_id  = collector_id;
    msg.interval_secs = config->retransmit_interval_secs;
    msg.initial_burst = config->initial_burst;

    buffer_req = ifa2_info->dma_buffer;
    buffer_ptr = _bcm_xgs5_ifa2_msg_ctrl_template_xmit_pack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_IFA2_TEMPLATE_XMIT_SET,
                                         buffer_len, 0,
                                         MOS_MSG_SUBCLASS_IFA2_TEMPLATE_XMIT_SET_REPLY,
                                         &reply_len,
                                         SHR_IFA2_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

/*
 * Get the template transmit configuration.
 * unit              - (IN)  BCM device number
 * template_id       - (IN)  Template Id
 * collector_id      - (IN)  Collector Id
 * config            - (OUT) Template transmit config
 */
int _bcm_xgs5_ifa2_template_transmit_config_get(int unit,
                                                bcm_ifa_export_template_t template_id,
                                                bcm_collector_t collector_id,
                                                bcm_ifa_template_transmit_config_t *config)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    shr_ifa2_msg_ctrl_template_xmit_t   msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    if (ifa2_info == NULL) {
        return BCM_E_INIT;
    }

    /* Verify if template exists */
    if ((!ifa2_info->template_created) ||
        ifa2_info->template_id != template_id) {
        return BCM_E_CONFIG;
    }

    if ((!ifa2_info->collector_attached) ||
        ifa2_info->collector_id != collector_id) {
        return BCM_E_CONFIG;
    }

    sal_memset(&msg, 0, sizeof(shr_ifa2_msg_ctrl_template_xmit_t));

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_IFA2_TEMPLATE_XMIT_GET,
                                         0, 0,
                                         MOS_MSG_SUBCLASS_IFA2_TEMPLATE_XMIT_GET_REPLY,
                                         &reply_len,
                                         SHR_IFA2_MAX_UC_MSG_TIMEOUT));
    buffer_req = ifa2_info->dma_buffer;
    buffer_ptr = _bcm_xgs5_ifa2_msg_ctrl_template_xmit_unpack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        return BCM_E_INTERNAL;
    }
    config->retransmit_interval_secs  = msg.interval_secs;
    config->initial_burst             = msg.initial_burst;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_stat_info_set
 * Purpose:
 *      Set the IFA2 stats.
 * Parameters:
 *      unit        - (IN)  BCM device number.
 *      stats       - (IN)  Statistics.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_ifa2_stat_info_set(int unit, bcm_ifa_stat_info_t *stats)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];

    if (ifa2_info == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_uc_msg_stats_set(unit, stats));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_stat_info_get
 * Purpose:
 *      Get the IFA2 stats.
 * Parameters:
 *      unit        - (IN)  BCM device number.
 *      stats       - (OUT) Statistics.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_ifa2_stat_info_get(int unit, bcm_ifa_stat_info_t *stats)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];

    if (ifa2_info == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_uc_msg_stats_get(unit, stats));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_config_info_set
 * Purpose:
 *      Set the IFA2 configuration.
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      cfg_info   - (IN)  Config Info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_ifa2_config_info_set(int unit, bcm_ifa_config_info_t *cfg_info)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    uint32 params[SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT];
    uint32 values[SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT][SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_MAX_VALUES];
    uint16 num_values[SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT];

    if (ifa2_info == NULL) {
        return BCM_E_INIT;
    }

    if ((cfg_info->rx_packet_export_max_length > ifa2_info->max_rx_pkt_length) ||
        (cfg_info->rx_packet_export_max_length > ifa2_info->max_export_pkt_length)) {
        return BCM_E_PARAM;
    }

    params[0] = SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_PARAM_RX_PKT_EXPORT_MAX_LENGTH;
    values[0][0] = cfg_info->rx_packet_export_max_length;
    num_values[0] = 1;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa2_uc_msg_cfg_update(unit, 1, params, num_values, values));

    ifa2_info->rx_pkt_export_max_length = cfg_info->rx_packet_export_max_length;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_config_info_get
 * Purpose:
 *      Get the IFA2 configuration.
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      cfg_info   - (IN)  Config Info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_ifa2_config_info_get(int unit, bcm_ifa_config_info_t *cfg_info)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];

    if (ifa2_info == NULL) {
        return BCM_E_INIT;
    }

    cfg_info->rx_packet_export_max_length = ifa2_info->rx_pkt_export_max_length;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_detach
 * Purpose:
 *      Detach the IFAv2 app
 * Parameters:
 *      unit       - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_ifa2_detach(int unit)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];

    if (ifa2_info == NULL) {
        return BCM_E_NONE;
    }

    /* Shutdown the embedded app */
    if (!SOC_WARM_BOOT(unit) &&
        (ifa2_info->uC != _BCM_XGS5_IFA2_INVALID_FW_CORE)) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_uc_msg_shutdown(unit));
    }

    sal_free(ifa2_info);
    ifa2_global_info[unit] = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_init
 * Purpose:
 *      Initialize the IFAv2 app
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      success    - (IN)  0 - App not loaded
 *                         1 - App loaded successfully
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_ifa2_init(int unit, uint8 *success)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    int appl_init_success;
    int rv;
#ifdef BCM_WARM_BOOT_SUPPORT
    int init = 0;
#endif /* BCM_WARM_BOOT_SUPPORT */

    *success = 0;

    if (soc_property_get(unit, spn_IFA_ENABLE, 0) == 0) {
        /* Silently return since ifa is not enabled */
        return BCM_E_NONE;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_wb_init_status_recover(unit, &init));

        if (init == 0) {
            /* Module was not initialized prior to WB, skip initializing it now */
            return BCM_E_NONE;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (ifa2_info != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_detach(unit));
        ifa2_info = NULL;
    }

    _BCM_IFA2_ALLOC(ifa2_info, _bcm_xgs5_ifa2_info_t,
                    sizeof(_bcm_xgs5_ifa2_info_t), "IFA2_INFO");
    if (ifa2_info == NULL) {
        return BCM_E_MEMORY;
    }
    ifa2_global_info[unit] = ifa2_info;

    ifa2_info->uC = _BCM_XGS5_IFA2_INVALID_FW_CORE;
    ifa2_info->max_rx_pkt_length =
        soc_property_get(unit, spn_IFA_RX_PKT_MAX_LENGTH,
                         _BCM_XGS5_IFA2_DEF_MAX_RX_PKT_LENGTH);
    if ((ifa2_info->max_rx_pkt_length < _BCM_XGS5_IFA2_MIN_RX_PKT_LENGTH) ||
        (ifa2_info->max_rx_pkt_length > _BCM_XGS5_IFA2_MAX_RX_PKT_LENGTH)) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_detach(unit));
        return BCM_E_PARAM;
    }

    ifa2_info->max_export_pkt_length =
        soc_property_get(unit, spn_IFA_MAX_EXPORT_PKT_LENGTH,
                         _BCM_XGS5_IFA2_DEF_MAX_EXPORT_PKT_LENGTH);
    if ((ifa2_info->max_export_pkt_length < _BCM_XGS5_IFA2_MIN_EXPORT_LENGTH) ||
        (ifa2_info->max_export_pkt_length > _BCM_XGS5_IFA2_MAX_EXPORT_PKT_LENGTH)) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_detach(unit));
        return BCM_E_PARAM;
    }

    ifa2_info->collector_id = _BCM_IFA_INVALID_COLLECTOR_ID;
    ifa2_info->export_profile_id = _BCM_IFA_INVALID_EXPORT_PROIFILE_ID;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_xgs5_ifa2_cfg_wb_recover(unit);
    } else {
        rv = _bcm_xgs5_ifa2_scache_alloc(unit, 1);
    }
    if (rv != BCM_E_NONE) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_detach(unit));
        return rv;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    ifa2_info->dma_buffer_len = sizeof(shr_ifa2_msg_ctrl_t);
    ifa2_info->dma_buffer = soc_cm_salloc(unit, ifa2_info->dma_buffer_len,
                                         "IFA2 DMA buffer");
    if (!ifa2_info->dma_buffer) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_detach(unit));
        return BCM_E_MEMORY;
    }
    sal_memset(ifa2_info->dma_buffer, 0, ifa2_info->dma_buffer_len);

    if (!SOC_WARM_BOOT(unit)) {
        /* Send the application init message to the uC. */
        appl_init_success = _bcm_xgs5_ifa2_uc_appl_init(unit);
        if (appl_init_success == 0) {
            /* App not loaded. */
            BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_detach(unit));
            return BCM_E_NONE;
        }

        /* Send the init message to the app. */
        rv = _bcm_xgs5_ifa2_uc_msg_init(unit);
        if (rv != BCM_E_NONE) {
            BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_detach(unit));
            return rv;
        }

        /* Send the MD format message to the app. */
        rv = _bcm_xgs5_ifa2_uc_msg_md_fmt_set(unit);
        if (rv != BCM_E_NONE) {
            BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_detach(unit));
            return rv;
        }
    }

    *success = 1;

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_IFA2_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_IFA2_DEFAULT_VERSION  BCM_WB_IFA2_VERSION_1_0

#define BCM_WB_IFA2_INIT_STATUS_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_IFA2_INIT_STATUS_DEFAULT_VERSION  BCM_WB_IFA2_INIT_STATUS_VERSION_1_0

#define IFA2_SCACHE_WRITE(_scache, _val, _type)                   \
    do {                                                          \
        _type _tmp = (_type) (_val);                              \
        sal_memcpy((_scache), &(_tmp), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
    } while(0)

#define IFA2_SCACHE_READ(_scache, _var, _type)                    \
    do {                                                          \
        _type _tmp;                                               \
        sal_memcpy(&(_tmp), (_scache), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
        (_var)     = (_tmp);                                      \
    } while(0)

/*
 * Function:
 *     _bcm_xgs5_ifa2_cfg_scache_v0_size_get
 * Purpose:
 *     Get IFA2 config scache v0 size
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Size required
 */
STATIC uint32
_bcm_xgs5_ifa2_cfg_scache_v0_size_get(int unit)
{
    uint32 size = 0;

    /* ifa2_info->uC */
    size += sizeof(uint8);

    /* ifa2_info->rx_pkt_export_max_length */
    size += sizeof(uint16);

    /* ifa2_info->collector_attached */
    size += sizeof(uint8);

    /* ifa2_info->collector_id */
    size += sizeof(int);

    /* ifa2_info->export_profile_id */
    size += sizeof(int);

    /* ifa2_info->template_created */
    size += sizeof(uint8);

    /* ifa2_info->template_id */
    size += sizeof(int);

    return size;
}

/*
 * Function:
 *     _bcm_xgs5_ifa2_cfg_scache_v0_sync
 * Purpose:
 *     Sync IFA2 config scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     None
 */
STATIC void
_bcm_xgs5_ifa2_cfg_scache_v0_sync(int unit, uint8 **scache)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];

    IFA2_SCACHE_WRITE(*scache, ifa2_info->uC, uint8);
    IFA2_SCACHE_WRITE(*scache, ifa2_info->rx_pkt_export_max_length, uint16);
    IFA2_SCACHE_WRITE(*scache, ifa2_info->collector_attached, uint8);
    IFA2_SCACHE_WRITE(*scache, ifa2_info->collector_id, int);
    IFA2_SCACHE_WRITE(*scache, ifa2_info->export_profile_id, int);
    IFA2_SCACHE_WRITE(*scache, ifa2_info->template_created, uint8);
    IFA2_SCACHE_WRITE(*scache, ifa2_info->template_id, int);
}

/*
 * Function:
 *     _bcm_xgs5_ifa2_cfg_scache_v0_recover
 * Purpose:
 *     Recover IFA2 config scache v0 data
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_xgs5_ifa2_cfg_scache_v0_recover(int unit, uint8 **scache)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];


    IFA2_SCACHE_READ(*scache, ifa2_info->uC, uint8);
    IFA2_SCACHE_READ(*scache, ifa2_info->rx_pkt_export_max_length, uint16);
    IFA2_SCACHE_READ(*scache, ifa2_info->collector_attached, uint8);
    IFA2_SCACHE_READ(*scache, ifa2_info->collector_id, int);
    IFA2_SCACHE_READ(*scache, ifa2_info->export_profile_id, int);
    IFA2_SCACHE_READ(*scache, ifa2_info->template_created, uint8);
    IFA2_SCACHE_READ(*scache, ifa2_info->template_id, int);

    if (ifa2_info->collector_id != _BCM_IFA_INVALID_COLLECTOR_ID) {
        BCM_IF_ERROR_RETURN(
                _bcm_esw_collector_ref_count_update(unit,
                    ifa2_info->collector_id, 1));
    }
    if (ifa2_info->export_profile_id !=
        _BCM_IFA_INVALID_EXPORT_PROIFILE_ID) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_collector_export_profile_ref_count_update(unit,
                ifa2_info->export_profile_id, 1));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_cfg_scache_alloc
 * Purpose:
 *      WB scache alloc for IFA2 configuration.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      create  - (IN) 1 - Create, 0 - Realloc
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_xgs5_ifa2_cfg_scache_alloc(int unit, int create)
{
    soc_scache_handle_t scache_handle;
    uint32 cur_size = 0;
    uint32 rqd_size = 0;
    int rv = BCM_E_NONE;
    uint8 *scache = NULL;

    rqd_size += _bcm_xgs5_ifa2_cfg_scache_v0_size_get(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_IFA,
                          _bcmIfa2WbSequenceCfg);

    if (create) {
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 1, rqd_size,
                                     &scache, BCM_WB_IFA2_DEFAULT_VERSION, NULL);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* Get current size */
        rv = soc_scache_ptr_get(unit, scache_handle, &scache, &cur_size);
        SOC_IF_ERROR_RETURN(rv);

        if (rqd_size > cur_size) {
            /* Request the extra size */
            rv = soc_scache_realloc(unit, scache_handle, rqd_size - cur_size);
            SOC_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_cfg_sync
 * Purpose:
 *      Warmboot scache sync of IFA configuration.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
int _bcm_xgs5_ifa2_cfg_sync(int unit)
{
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];
    soc_scache_handle_t scache_handle;
    int stable_size;
    uint8 *scache = NULL;

    if (ifa2_info == NULL) {
        return BCM_E_NONE;
    }

    /* Get IFA2 module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_IFA,
                          _bcmIfa2WbSequenceCfg);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                 &scache, BCM_WB_IFA2_DEFAULT_VERSION,
                                 NULL));
    if (NULL == scache) {
        return BCM_E_INTERNAL;
    }

    /* Sync IFA2 scache */
    _bcm_xgs5_ifa2_cfg_scache_v0_sync(unit, &scache);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_cfg_wb_recover
 * Purpose:
 *      IFA2 config WB recovery
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
int _bcm_xgs5_ifa2_cfg_wb_recover(int unit)
{
    soc_scache_handle_t scache_handle;
    int stable_size;       /* Secondary storage size.   */
    uint8 *scache = NULL;
    int rv = BCM_E_NONE;
    uint16 recovered_ver = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_IFA,
                              _bcmIfa2WbSequenceCfg);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                     &scache, BCM_WB_IFA2_DEFAULT_VERSION,
                                     &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have warmboot state */
            rv = _bcm_xgs5_ifa2_cfg_scache_alloc(unit, 1);
            return rv;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (NULL == scache) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_NONE;
    }

    /* Scache recovery */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_cfg_scache_v0_recover(unit, &scache));

    /* Realloc any extra scache that may be needed */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_cfg_scache_alloc(unit, 0));

    return rv;
}

/*
 * Function:
 *     _bcm_xgs5_ifa2_init_status_sync
 * Purpose:
 *     Syncs IFA2 init status to scache
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_xgs5_ifa2_init_status_sync(int unit)
{

    int stable_size;
    soc_scache_handle_t scache_handle;
    uint8 *scache = NULL;
    _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];

    /* Get Telemetry module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_IFA, _bcmIfa2WbSequenceInitStatus);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                                &scache, 0, NULL));
    if (NULL == scache) {
        return BCM_E_INTERNAL;
    }

    if (ifa2_info == NULL) {
        /* Module not running */
        IFA2_SCACHE_WRITE(scache, 0, uint8);
    } else {
        /* Module in running state */
        IFA2_SCACHE_WRITE(scache, 1, uint8);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ifa2_init_status_scache_size_get
 * Purpose:
 *     Get the size required to sync telemetry module status to scache
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_ifa2_init_status_scache_size_get(int unit)
{
    return (sizeof(uint8));
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_init_status_scache_alloc
 * Purpose:
 *      Telemetry WB scache alloc for storing module_status
 * Parameters:
 *      unit    - (IN) Unit number.
 *      create  - (IN) 1 - Create, 0 - Realloc
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
int _bcm_xgs5_ifa2_init_status_scache_alloc(int unit, int create)
{
    uint32                  cur_size = 0;
    uint32                  rqd_size = 0;
    int                     rv = BCM_E_NONE;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;

    rqd_size += _bcm_ifa2_init_status_scache_size_get(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_IFA,
                          _bcmIfa2WbSequenceInitStatus);

    if (create) {
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 1, rqd_size,
                               &scache, BCM_WB_IFA2_INIT_STATUS_DEFAULT_VERSION, NULL);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* Get current size */
        rv = soc_scache_ptr_get(unit, scache_handle, &scache, &cur_size);
        SOC_IF_ERROR_RETURN(rv);

        if (rqd_size > cur_size) {
            /* Request the extra size */
            rv = soc_scache_realloc(unit, scache_handle, rqd_size - cur_size);
            SOC_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_wb_init_status_recover
 * Purpose:
 *      Find out if the IFA2 module was running prior to warmboot
 * Parameters:
 *      unit        - (IN)  BCM device number
 *      init        - (OUT) 1 - Module initialized,
 *                          0 - Module detached
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC
int _bcm_xgs5_ifa2_wb_init_status_recover(int unit, int *init)
{
    int                 stable_size;       /* Secondary storage size.   */
    uint8               *scache = NULL;
    soc_scache_handle_t scache_handle;
    int                 rv = BCM_E_NONE;
    uint16              recovered_ver = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                              BCM_MODULE_IFA,
                              _bcmIfa2WbSequenceInitStatus);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                     &scache,
                                     BCM_WB_IFA2_INIT_STATUS_DEFAULT_VERSION,
                                     &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have
             * warmboot state for module status
             */
            rv = _bcm_xgs5_ifa2_init_status_scache_alloc(unit, 1);

            /* Set init = 1 to avoid breaking upgrade from older release */
            *init = 1;
            return rv;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (NULL == scache) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_NONE;
    }

    /* Scache recovery */
    IFA2_SCACHE_READ(scache, *init, uint8);

    /* Realloc any extra scache that may be needed */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_init_status_scache_alloc(unit, 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_scache_alloc
 * Purpose:
 *      IFA2 WB scache alloc.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      create  - (IN) 1 - Create, 0 - Realloc
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC
int _bcm_xgs5_ifa2_scache_alloc(int unit, int create)
{
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_init_status_scache_alloc(unit, create));
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_cfg_scache_alloc(unit, create));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa2_sync
 * Purpose:
 *      IFA2 warmboot scache sync.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_xgs5_ifa2_sync(int unit)
{
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_init_status_sync(unit));
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa2_cfg_sync(unit));

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_xgs5_ifa2_appl_callback
 * Purpose:
 *      Update IFA2 FW appl state
 * Parameters:
 *      unit  - (IN) Unit number.
 *      uC    - (IN) core number.
 *      stage - (IN) core reset stage.
 *      user_data - (IN) data pointer.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int _bcm_xgs5_ifa2_appl_callback(int unit,
                                    int uC,
                                    soc_cmic_uc_shutdown_stage_t stage,
                                    void *user_data) {
   _bcm_xgs5_ifa2_info_t *ifa2_info = ifa2_global_info[unit];

    if (ifa2_info == NULL) {
        return BCM_E_INIT;
    }

    ifa2_info->uC = _BCM_XGS5_IFA2_INVALID_FW_CORE;

    return BCM_E_NONE;
}
#else
typedef int bcm_make_iso_compilers_happy;
#endif /* INCLUDE_IFA */
