/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Flowtracker - Flow Tracking embedded APP interface
 * Purpose: API to configure flowtracker embedded app interface.
 */

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/tomahawk.h>
#include <shared/alloc.h>
#include <shared/idxres_fl.h>

#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/esw/flowtracker.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/maverick2.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/collector.h>
#include <bcm_int/esw/udf.h>

#if defined(INCLUDE_FLOWTRACKER)

/* Flow tracker global information */
_bcm_int_ft_info_t *ft_global_info[BCM_MAX_NUM_UNITS] = {0};

/* List of flowtracker export templates info */
extern _bcm_int_ft_export_template_info_t *ft_export_template_info_list[BCM_MAX_NUM_UNITS];

/* List of flowtracker collector info*/
extern _bcm_int_ft_collector_info_t *ft_collector_info_list[BCM_MAX_NUM_UNITS];
extern _bcm_int_ft_v2_collector_info_t *ft_v2_collector_info_list[BCM_MAX_NUM_UNITS];
extern _bcm_int_ft_v2_export_profile_info_t *ft_v2_export_profile_info_list[BCM_MAX_NUM_UNITS];

/* List of flowtracker flow_group info*/
extern _bcm_int_ft_flow_group_info_t *ft_flow_group_info_list[BCM_MAX_NUM_UNITS];

/* List of flowtracker flow_group info*/
extern _bcm_int_ft_elph_profile_info_t *ft_elph_profile_info_list[BCM_MAX_NUM_UNITS];

/* Action conflict check */
#define FT_ACTION_CONFLICT(_action_a, _action_b) \
    if ((_action_a) == (_action_b)) {            \
        return BCM_E_CONFIG;                     \
    }

/* Map export elements to internal names */
typedef struct _bcm_th_ft_template_support_mapping_s {
    bcm_flowtracker_export_element_type_t element;
    shr_ft_template_info_elem_t           int_element;
    uint16                                data_size;
} _bcm_th_ft_template_support_mapping_t;

/* Template elements API <--> app mapping */
STATIC _bcm_th_ft_template_support_mapping_t _bcm_th_ft_template_element_mapping[] = {
    {bcmFlowtrackerExportElementTypeSrcIPv4,    SHR_FT_TEMPLATE_INFO_ELEM_SRC_IPV4, 4},
    {bcmFlowtrackerExportElementTypeDstIPv4,    SHR_FT_TEMPLATE_INFO_ELEM_DST_IPV4, 4},
    {bcmFlowtrackerExportElementTypeL4SrcPort,  SHR_FT_TEMPLATE_INFO_ELEM_L4_SRC_PORT, 2},
    {bcmFlowtrackerExportElementTypeL4DstPort,  SHR_FT_TEMPLATE_INFO_ELEM_L4_DST_PORT, 2},
    {bcmFlowtrackerExportElementTypeIPProtocol, SHR_FT_TEMPLATE_INFO_ELEM_IP_PROTOCOL, 1},
    {bcmFlowtrackerExportElementTypePktCount,   SHR_FT_TEMPLATE_INFO_ELEM_PKT_TOTAL_COUNT, 4},
    {bcmFlowtrackerExportElementTypeByteCount,  SHR_FT_TEMPLATE_INFO_ELEM_BYTE_TOTAL_COUNT, 6},
    {bcmFlowtrackerExportElementTypeFlowStartTimeMsecs, SHR_FT_TEMPLATE_INFO_ELEM_FLOW_START_TS_MSEC, 8},
    {bcmFlowtrackerExportElementTypeObservationTimeMsecs, SHR_FT_TEMPLATE_INFO_ELEM_OBS_TS_MSEC, 8},
    {bcmFlowtrackerExportElementTypeFlowtrackerGroup, SHR_FT_TEMPLATE_INFO_ELEM_GROUP_ID, 1},
    {bcmFlowtrackerExportElementTypeOuterVlanTag, SHR_FT_TEMPLATE_INFO_ELEM_VNID, 2},
    {bcmFlowtrackerExportElementTypeCustom, SHR_FT_TEMPLATE_INFO_ELEM_CUSTOM, 16},
    {bcmFlowtrackerExportElementTypeInnerSrcIPv4, SHR_FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV4, 4},
    {bcmFlowtrackerExportElementTypeInnerDstIPv4, SHR_FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV4, 4},
    {bcmFlowtrackerExportElementTypeInnerSrcIPv6, SHR_FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV6, 8},
    {bcmFlowtrackerExportElementTypeInnerDstIPv6, SHR_FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV6, 8},
    {bcmFlowtrackerExportElementTypeInnerL4SrcPort, SHR_FT_TEMPLATE_INFO_ELEM_INNER_L4_SRC_PORT, 2},
    {bcmFlowtrackerExportElementTypeInnerL4DstPort, SHR_FT_TEMPLATE_INFO_ELEM_INNER_L4_DST_PORT, 2},
    {bcmFlowtrackerExportElementTypeInnerIPProtocol, SHR_FT_TEMPLATE_INFO_ELEM_INNER_IP_PROTOCOL, 1},
};

STATIC _bcm_th_ft_template_support_mapping_t _bcm_th_ft_qcm_template_element_mapping[] = {
    {bcmFlowtrackerExportElementTypeSrcIPv4,    SHR_QCM_TEMPLATE_INFO_ELEM_SRC_IP, 16},
    {bcmFlowtrackerExportElementTypeDstIPv4,    SHR_QCM_TEMPLATE_INFO_ELEM_DST_IP, 16},
    {bcmFlowtrackerExportElementTypeL4SrcPort,  SHR_QCM_TEMPLATE_INFO_ELEM_L4_SRC_PORT, 2},
    {bcmFlowtrackerExportElementTypeL4DstPort,  SHR_QCM_TEMPLATE_INFO_ELEM_L4_DST_PORT, 2},
    {bcmFlowtrackerExportElementTypeIPProtocol, SHR_QCM_TEMPLATE_INFO_ELEM_IP_PROTOCOL, 1},
    {bcmFlowtrackerExportElementTypeByteCount,  SHR_QCM_TEMPLATE_INFO_ELEM_FLOW_RX_BYTE_COUNT, 6},
    {bcmFlowtrackerExportElementTypePktCount,   SHR_QCM_TEMPLATE_INFO_ELEM_FLOW_RX_PKT_COUNT, 2},
    {bcmFlowtrackerExportElementTypeFlowStartTimeMsecs, SHR_QCM_TEMPLATE_INFO_ELEM_FLOW_VIEW_START_TS, 8},
    {bcmFlowtrackerExportElementTypeFlowViewID, SHR_QCM_TEMPLATE_INFO_ELEM_FLOW_VIEW_ID, 2},
    {bcmFlowtrackerExportElementTypeFlowCount, SHR_QCM_TEMPLATE_INFO_ELEM_FLOW_COUNT, 2},
    {bcmFlowtrackerExportElementTypeDropByteCount, SHR_QCM_TEMPLATE_INFO_ELEM_QUEUE_DROP_BYTE_COUNT, 6},
    {bcmFlowtrackerExportElementTypeDropPktCount, SHR_QCM_TEMPLATE_INFO_ELEM_QUEUE_DROP_PKT_COUNT, 2},
    {bcmFlowtrackerExportElementTypeEgrPort, SHR_QCM_TEMPLATE_INFO_ELEM_EGR_PORT, 2},
    {bcmFlowtrackerExportElementTypeQueueID, SHR_QCM_TEMPLATE_INFO_ELEM_QUEUE_ID, 1},
    {bcmFlowtrackerExportElementTypeQueueBufferUsage, SHR_QCM_TEMPLATE_INFO_ELEM_QUEUE_BUF_USAGE, 3},
    {bcmFlowtrackerExportElementTypeBufferUsage, SHR_QCM_TEMPLATE_INFO_ELEM_BUF_USAGE, 1},
    {bcmFlowtrackerExportElementTypeIPVersion, SHR_QCM_TEMPLATE_INFO_ELEM_IP_VERSION, 1},
    {bcmFlowtrackerExportElementTypeIngPort, SHR_QCM_TEMPLATE_INFO_ELEM_ING_PORT, 2},
    {bcmFlowtrackerExportElementTypeQueueRxByteCount, SHR_QCM_TEMPLATE_INFO_ELEM_QUEUE_RX_BYTE_COUNT, 6},
    {bcmFlowtrackerExportElementTypeQueueRxPktCount, SHR_QCM_TEMPLATE_INFO_ELEM_QUEUE_RX_PKT_COUNT, 2},
};

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int _bcm_th_ft_scache_alloc(int unit, int create);
STATIC int _bcm_th_ft_wb_recover(int unit);
#define _BCM_FT_WB_INVALID_COLLECTOR_ID 0xFFFF
#endif /* BCM_WARM_BOOT_SUPPORT */

/* TD3 chunk index to FT qualifier mapping */
static int _bcm_td3_ft_udf_chunk_to_qual_mapping[] = {
                                                         SHR_FT_EM_QUAL_UDF_CHUNK0,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK1,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK2,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK3,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK4,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK5,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK6,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK7,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK8,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK9,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK10,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK11,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK12,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK13,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK14,
                                                         SHR_FT_EM_QUAL_UDF_CHUNK15,
                                                     };

#define _BCM_FT_TH_MAX_UDF_CHUNKS 16
/*--------------------------------------------------------------------------------------*
 * Each chunk or chunk pair corresponds to the data qualifier as marked below:
 *
 * 15 -- 14   13 -- 12    11 -- 10    9    8     7 -- 6    5 -- 4   3 -- 2   1     0
 *{________}  {_______}   {_______} {___}{___}  {______}  {______}  {_____} {___}{___}
 *   Data9      Data8       Data7   Data6 Data5   Data4    Data3     Data2  Data1 Data0
 *  (32bit)    (32bit)    (32bit)  (16bit)(16bit) (32bit) (32bit)   (32bit)(16bit)(16bit)
 * {_________________________________________}   {___________________________________}
 *                     ||                                          ||
 *                    UDF_2                                       UDF_1
 *--------------------------------------------------------------------------------------*/

typedef struct _bcm_th_ft_udf_chunk_info_s {
    /* Internal FT qualifier */
    int qual;

    /* Size of the chunk */
    int size;
} _bcm_th_ft_udf_chunk_info_t;

/* In TH3 all the chunks are of size 2B */
#define FT_TH3_UDF_CHUNK_SIZE 2

STATIC _bcm_th_ft_udf_chunk_info_t _bcm_th_ft_udf_chunk_info[_BCM_FT_TH_MAX_UDF_CHUNKS] =
    {
           {SHR_FT_EM_QUAL_UDF_CHUNK0, 2},
           {SHR_FT_EM_QUAL_UDF_CHUNK1, 2},
           {SHR_FT_EM_QUAL_UDF_CHUNK2, 4},
           {SHR_FT_EM_QUAL_UDF_CHUNK2, 4},
           {SHR_FT_EM_QUAL_UDF_CHUNK3, 4},
           {SHR_FT_EM_QUAL_UDF_CHUNK3, 4},
           {SHR_FT_EM_QUAL_UDF_CHUNK4, 4},
           {SHR_FT_EM_QUAL_UDF_CHUNK4, 4},
           {SHR_FT_EM_QUAL_UDF_CHUNK5, 2},
           {SHR_FT_EM_QUAL_UDF_CHUNK6, 2},
           {SHR_FT_EM_QUAL_UDF_CHUNK7, 4},
           {SHR_FT_EM_QUAL_UDF_CHUNK7, 4},
           {SHR_FT_EM_QUAL_UDF_CHUNK8, 4},
           {SHR_FT_EM_QUAL_UDF_CHUNK8, 4},
           {SHR_FT_EM_QUAL_UDF_CHUNK9, 4},
           {SHR_FT_EM_QUAL_UDF_CHUNK9, 4},
    };
/*
 * Function:
 *      _bcm_th_ft_template_validate_convert
 * Purpose:
 *      Validate the template and convert to app format
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      set_id        - (IN)  Set Id
 *      num_elements  - (IN)  Number of template elements
 *      elements      - (IN)  Elements list
 *      template      - (OUT) Template info in internal format
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_template_validate_convert(int unit,
                                     uint16 set_id,
                                     int num_elements,
                                     bcm_flowtracker_export_element_info_t *elements,
                                     _bcm_int_ft_export_template_info_t *template)
{
    int i, j;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    sal_memset(template, 0, sizeof(_bcm_int_ft_export_template_info_t));

    /* Validate set id. Should be between 256 and 65535 */
    if (!BCM_INT_FT_TEMPLATE_SET_ID_VALID(set_id)) {
        return BCM_E_PARAM;
    }
    template->set_id = set_id;

    if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
        if (num_elements != SHR_QCM_TEMPLATE_MAX_INFO_ELEMENTS) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                      "FT QCM(unit %d) Error: Invalid num of export params\n"),
                       unit));
            return BCM_E_PARAM;
        }
    } else {
        if (num_elements > SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS) {
            return BCM_E_PARAM;
        }
    }
    template->num_export_elements = num_elements;

    for (i = 0; i < num_elements; i++) {
        if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
            int tmp_ele = -1;
            if (elements[i].element == bcmFlowtrackerExportElementTypeSrcIPv6) {
                tmp_ele = bcmFlowtrackerExportElementTypeSrcIPv4;
            } else if (elements[i].element == bcmFlowtrackerExportElementTypeDstIPv6) {
                tmp_ele = bcmFlowtrackerExportElementTypeDstIPv4;
            } else {
                tmp_ele = elements[i].element;
            }
            for (j = 0; j < SHR_QCM_TEMPLATE_MAX_INFO_ELEMENTS; j++) {
                if ((tmp_ele == _bcm_th_ft_qcm_template_element_mapping[j].element)) {
                    if ((elements[i].element != bcmFlowtrackerExportElementTypeFlowStartTimeMsecs) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeFlowViewID) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeFlowCount) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeByteCount) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypePktCount) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeDropByteCount) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeDropPktCount) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeQueueRxByteCount) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeQueueRxPktCount) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeEgrPort) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeIngPort) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeQueueID) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeSrcIPv4) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeDstIPv4) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeSrcIPv6) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeDstIPv6) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeL4SrcPort) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeL4DstPort) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeIPProtocol) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeIPVersion) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeQueueBufferUsage) &&
                        (elements[i].element != bcmFlowtrackerExportElementTypeBufferUsage)) {
                       LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                 "FT QCM(unit %d) Error: Invalid export param : "
                                 "%d %d\n"),
                                  unit, elements[i].element,
                                  _bcm_th_ft_qcm_template_element_mapping[j].element));
                        return BCM_E_PARAM;
                    }

                    if ((elements[i].data_size != 0) &&
                        (elements[i].data_size !=
                                 _bcm_th_ft_qcm_template_element_mapping[j].data_size)) {
                       LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                 "FT QCM(unit %d) Error: Invalid export params size : " "%d %d %d\n"),
                                  unit, elements[i].element, elements[i].data_size,
                                  _bcm_th_ft_qcm_template_element_mapping[j].element));
                        return BCM_E_PARAM;
                    }

                    template->elements[i].element =
                               _bcm_th_ft_qcm_template_element_mapping[j].int_element;
                    template->elements[i].data_size =
                               _bcm_th_ft_qcm_template_element_mapping[j].data_size;
                    template->elements[i].id =
                               elements[i].info_elem_id;
                    if (elements[i].flags &
                                   BCM_FLOWTRACKER_EXPORT_ELEMENT_FLAGS_ENTERPRISE) {
                        template->elements[i].enterprise = 1;
                    }
                    break;
                }
            }
            if (j >= SHR_QCM_TEMPLATE_MAX_INFO_ELEMENTS) {
                /* No match found */
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                          "FT QCM(unit %d) Error: Invalid export param : " "%d\n"),
                           unit, elements[i].element));
                return BCM_E_PARAM;
            }
        } else {
            for (j = 0; j < SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS; j++) {
                if (elements[i].element == _bcm_th_ft_template_element_mapping[j].element) {
                    if (((elements[i].element == bcmFlowtrackerExportElementTypeOuterVlanTag) ||
                         (elements[i].element == bcmFlowtrackerExportElementTypeCustom) ||
                         (elements[i].element == bcmFlowtrackerExportElementTypeInnerSrcIPv4) ||
                         (elements[i].element == bcmFlowtrackerExportElementTypeInnerDstIPv4) ||
                         (elements[i].element == bcmFlowtrackerExportElementTypeInnerSrcIPv6) ||
                         (elements[i].element == bcmFlowtrackerExportElementTypeInnerDstIPv6) ||
                         (elements[i].element == bcmFlowtrackerExportElementTypeInnerL4SrcPort) ||
                         (elements[i].element == bcmFlowtrackerExportElementTypeInnerL4DstPort) ||
                         (elements[i].element == bcmFlowtrackerExportElementTypeInnerIPProtocol)) &&
                        (ft_info->cfg_mode < SHR_FT_CFG_MODE_V2)) {
                        return BCM_E_PARAM;
                    }

                    if ((elements[i].data_size != 0) &&
                        (elements[i].data_size !=
                                 _bcm_th_ft_template_element_mapping[j].data_size)) {
                        return BCM_E_PARAM;
                    }

                    if (elements[i].element ==
                                 bcmFlowtrackerExportElementTypeFlowStartTimeMsecs) {
                        if (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_FLOW_START_TS)) {
                            return BCM_E_PARAM;
                        }
                    }

                    template->elements[i].element =
                               _bcm_th_ft_template_element_mapping[j].int_element;
                    template->elements[i].data_size =
                               _bcm_th_ft_template_element_mapping[j].data_size;
                    template->elements[i].id =
                               elements[i].info_elem_id;
                    if (elements[i].flags &
                                   BCM_FLOWTRACKER_EXPORT_ELEMENT_FLAGS_ENTERPRISE) {
                        template->elements[i].enterprise = 1;
                    }
                    break;
                }
            }
            if (j >= SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS) {
                /* No match found */
                return BCM_E_PARAM;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_collector_info_validate
 * Purpose:
 *      Validate the collector params
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      collector  - (IN)  Collector data structure
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_collector_info_validate(int unit,
                                   bcm_flowtracker_collector_info_t *collector)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    /* Only IPv4 over UDP is supported */
    if (collector->transport_type !=
                       bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp) {
        return BCM_E_UNAVAIL;
    }

    /* Validate the export length */
    if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
        if (collector->max_packet_length < SHR_FT_QCM_MIN_EXPORT_LENGTH ||
            collector->max_packet_length > ft_info->max_export_pkt_length) {
            return BCM_E_PARAM;
        }
    } else {
        if (collector->max_packet_length < SHR_FT_MIN_EXPORT_LENGTH ||
            collector->max_packet_length > ft_info->max_export_pkt_length) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_collector_export_profile_validate
 * Purpose:
 *      Validate the collector and export profile params
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      collector       - (IN)  Collector data structure
 *      export_profile  - (IN)  Export profile data structure
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_collector_export_profile_validate(
                               int unit,
                               bcm_collector_info_t *collector,
                               bcm_collector_export_profile_t *export_profile)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    /* Check if the wire format can be supported */
    switch (export_profile->wire_format) {
        case bcmCollectorWireFormatIpfix:
            if (!(ft_info->uc_features & SHR_FT_UC_FEATURE_IPFIX_EXPORT)) {
                return BCM_E_UNAVAIL;
            }
            break;

        case bcmCollectorWireFormatProtoBuf:
            if (!(ft_info->uc_features & SHR_FT_UC_FEATURE_PROTOBUF_EXPORT)) {
                return BCM_E_UNAVAIL;
            }
            break;

        default:
            return BCM_E_UNAVAIL;
    }

    /* Only IPv4 and IPv6 over UDP is supported */
    if ((collector->transport_type != bcmCollectorTransportTypeIpv4Udp) &&
        (collector->transport_type != bcmCollectorTransportTypeIpv6Udp)) {
        return BCM_E_UNAVAIL;
    }

    if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
        if (export_profile->wire_format != bcmCollectorWireFormatIpfix) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                      "FT QCM(unit %d) Error: Invalid export wire format : " "%d\n"),
                      unit, export_profile->wire_format));
            return BCM_E_PARAM;
        }
        /* Validate the export length */
        if (export_profile->max_packet_length < SHR_FT_QCM_MIN_EXPORT_LENGTH) {
            return BCM_E_PARAM;
        }
    } else {
        /* Validate the export length */
        if (export_profile->max_packet_length < SHR_FT_MIN_EXPORT_LENGTH ||
            export_profile->max_packet_length > ft_info->max_export_pkt_length) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_validate_aging_timer_interval
 * Purpose:
 *      Validate the aging interval
 * Parameters:
 *      unit               - (IN) BCM device number
 *      aging_interval_ms  - (IN)  Aging interval in msec
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_validate_aging_timer_interval(int unit, uint32 aging_interval_ms)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (aging_interval_ms == 0) {
        return BCM_E_PARAM;
    }

    if ((aging_interval_ms % BCM_INT_FT_AGING_INTERVAL_STEP_MSECS) != 0) {
        return BCM_E_PARAM;
    }

    if (aging_interval_ms >
        BCM_INT_FT_AGING_INTERVAL_SCAN_MULTIPLE * (ft_info->scan_interval_usecs / 1000)) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th_ft_qcm_field_qual_convert
 * Purpose:
 *      Convert the field qualifier to app notation
 * Parameters:
 *      field_qual      - (IN)  Field qualifier
 *      ft_qual         - (OUT) Qualifier in app notation
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_qcm_field_qual_convert(int field_qual,
                                  shr_qcm_em_qual_t *ft_qual)
{
    switch (field_qual) {
        case bcmFieldQualifySrcIp:
            *ft_qual = SHR_QCM_EM_QUAL_SRC_IP;
            break;

        case bcmFieldQualifyDstIp:
            *ft_qual = SHR_QCM_EM_QUAL_DST_IP;
            break;

        case bcmFieldQualifyIpProtocol:
            *ft_qual = SHR_QCM_EM_QUAL_IP_PROTOCOL;
            break;

        case bcmFieldQualifyL4SrcPort:
            *ft_qual = SHR_QCM_EM_QUAL_L4_SRC_PORT;
            break;

        case bcmFieldQualifyL4DstPort:
            *ft_qual = SHR_QCM_EM_QUAL_L4_DST_PORT;
            break;

        case bcmFieldQualifyInPort:
            *ft_qual = SHR_QCM_EM_QUAL_ING_PORT;
            break;

        case bcmFieldQualifySrcIp6:
            *ft_qual = SHR_QCM_EM_QUAL_SRC_IP;
            break;

        case bcmFieldQualifyDstIp6:
            *ft_qual = SHR_QCM_EM_QUAL_DST_IP;
            break;

        case _bcmFieldQualifyPreLogicalTableId:
            *ft_qual = SHR_QCM_EM_QUAL_LT_ID;
            break;

        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th_ft_qcm_fill_em_group_create_msg
 * Purpose:
 *      Convert the EM key format from field data structure to
 *      App message format
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      msg           - (OUT) Key format message
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_th_ft_qcm_fill_em_group_create_msg(int unit,
                                        bcm_field_group_t *em_group,
                                        uint8 ft_em_group,
                                        int num_tracking_params,
                                        bcm_flowtracker_tracking_param_info_t *tracking_params,
                                        shr_ft_qcm_msg_ctrl_em_key_format_t *msg)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_field_group_qual_t **qual_arr;
    uint16 *qid_arr, qid;
    _bcm_field_qual_offset_t *offset_arr;
    int num_parts_count, max_parts_count;
    int i, j, k, qidx, offset_idx;
    uint16 offset, next_offset, width;
    uint16 word_boundary;
    int rv;
    int qual_count, em_qual_count;
    shr_qcm_em_qual_t ft_qual;
    shr_ft_qcm_msg_ctrl_qual_parts_t *qual_info;
    _field_group_t *fg, *tmp_fg;
    int key_boundary = 79;

    if (soc_feature(unit, soc_feature_th3_style_fp)) {
        key_boundary = 51;
    }

    BCM_IF_ERROR_RETURN(_field_group_get(unit, em_group[0], &fg));

    if (fg->stage_id != _BCM_FIELD_STAGE_EXACTMATCH) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT QCM(unit %d) Error: Group Stage is not ExactMatch \n"),
                   unit));
        return BCM_E_PARAM;
    }

    if (fg->em_mode != _FieldExactMatchMode320) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT QCM(unit %d) Error: Only 320b EM view is supported \n"),
                   unit));
        return BCM_E_PARAM;
    }

    qual_arr = sal_alloc(_FP_MAX_ENTRY_TYPES * sizeof(_bcm_field_group_qual_t*),
                         "FT QCM EM qual_arr");
    if (qual_arr == NULL) {
        return BCM_E_MEMORY;
    }
    for (i = 0; i < _FP_MAX_ENTRY_TYPES; i++) {
        qual_arr[i] = sal_alloc(_FP_MAX_ENTRY_WIDTH * sizeof(_bcm_field_group_qual_t),
                                "FT QCM EM qual_arr");
        if (qual_arr[i] == NULL) {
            for (j = 0; j < i; j++) {
                sal_free(qual_arr[j]);
            }
            sal_free(qual_arr);
            qual_arr = NULL;
            return BCM_E_MEMORY;
        }
    }

    /* Get the EM key format */
    if (fg->em_mode == _FieldExactMatchMode128) {
        max_parts_count = 1;
    } else {
        max_parts_count = 2;
    }
    rv = _bcm_esw_field_group_info_key_get(unit, em_group[0],
                                           max_parts_count, qual_arr,
                                           &num_parts_count);
    if (BCM_FAILURE(rv)) {
        for (i = 0; i < _FP_MAX_ENTRY_TYPES; i++) {
            sal_free(qual_arr[i]);
        }
        sal_free(qual_arr);
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT QCM(unit %d) Error: _bcm_esw_field_group_info_key_get "
                              "failed rv=%d\n"),
                   unit, rv));
        return rv;
    }

    /* Get the total number of qualifiers across all the parts. This may include
     * duplicates if a single qualifier is split across multiple parts
     */
    em_qual_count = 0;
    for (i  = 0; i < num_parts_count; i++) {
        em_qual_count += qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size;
    }

    qid_arr = sal_alloc(em_qual_count * sizeof(uint16), "FT QCM EM qid arr");
    if (qid_arr == NULL) {
        for (i = 0; i < _FP_MAX_ENTRY_TYPES; i++) {
            sal_free(qual_arr[i]);
        }
        sal_free(qual_arr);
        return BCM_E_MEMORY;
    }
    sal_memset(qid_arr, 0, em_qual_count * sizeof(uint16));

    offset_arr = sal_alloc(em_qual_count * sizeof(_bcm_field_qual_offset_t),
                           "FT QCM EM qid arr");
    if (offset_arr == NULL) {
        sal_free(qid_arr);
        for (i = 0; i < _FP_MAX_ENTRY_TYPES; i++) {
            sal_free(qual_arr[i]);
        }
        sal_free(qual_arr);
        return BCM_E_MEMORY;
    }
    sal_memset(offset_arr, 0, em_qual_count * sizeof(_bcm_field_qual_offset_t));

    /* Copy over the first part */
    sal_memcpy(qid_arr, qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].qid_arr,
               qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].size * sizeof(uint16));
    sal_memcpy(offset_arr, qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].offset_arr,
               qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].size * sizeof(_bcm_field_qual_offset_t));
    qual_count = qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].size;

    /* Copy over the remaining parts, adjust the offset and filter duplicates */
    for (i = 1; i < num_parts_count; i++) {
        for (j = 0; j < qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size; j++) {
            qid = qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr[j];

            /* Check if it is a case of a single qualifier split across
             * multiple parts
             */
            for (k = 0; k < qual_count; k++) {
                if (qid_arr[k] == qid) {
                    qidx = k;
                    break;
                }
            }
            if (k >= qual_count) {
                /* Not a duplicate */
                qidx = qual_count;
                qual_count++;
            }
            qid_arr[qidx] = qid;
            /* Insert to the list */
            for (k = 0, offset_idx = offset_arr[qidx].num_offsets;
                 k < qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[j].num_offsets;
                 k++, offset_idx++) {
                offset_arr[qidx].offset[offset_idx] =
                        160 * i + qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[j].offset[k];
                offset_arr[qidx].width[offset_idx] =
                           qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[j].width[k];
            }
            offset_arr[qidx].num_offsets = offset_idx;
        }
    }

    for (i = 0; i < _FP_MAX_ENTRY_TYPES; i++) {
        sal_free(qual_arr[i]);
    }
    sal_free(qual_arr);

    /* Check if any qualifier has more parts that the app can handle */
    for (qidx = 0; qidx < qual_count; qidx++) {
        if (offset_arr[qidx].num_offsets > SHR_FT_QCM_EM_KEY_MAX_QUAL_PARTS) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                      (BSL_META_U(unit,
                              "FT QCM(unit %d) Error: Excess num_offsets than expected "
                              "num_offsets=%u\n"),
                   unit, offset_arr[qidx].num_offsets));
            sal_free(qid_arr);
            sal_free(offset_arr);
            return BCM_E_PARAM;
        }
    }

    if (qual_count > SHR_QCM_EM_MAX_QUAL) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT QCM(unit %d) Error: Excess qualifiers than expected "
                              "qual_count=%u\n"),
                   unit, qual_count));
        sal_free(qid_arr);
        sal_free(offset_arr);
        return BCM_E_PARAM;
    }

    if (fg->em_mode == _FieldExactMatchMode128) {
        /* In Exact Match Mode 128 - whole key is not presented
         * for lookup. Instead Bit 0-47 and Bit 80-160 are provided
         * to UFT. Hence qualifier offsets at position greater than
         * 80 bit are shifted by 32 bit for key.
         */
        for (i = 0; i < qual_count; i++) {
            for (j = 0; j < offset_arr[i].num_offsets; j++) {
                if (offset_arr[i].offset[j] > key_boundary) {
                    offset_arr[i].offset[j] -= 32;
                }
            }
        }
    }
    /* Some qual parts are contiguous but field stores them as separate parts,
     * combine them to reduce the number of bitops the FW has to perform on each
     * flow
     */
    for (i = 0; i < qual_count; i++) {
        j = 0;
        while(j < (offset_arr[i].num_offsets - 1)) {
            offset      = offset_arr[i].offset[j];
            width       = offset_arr[i].width[j];
            next_offset = offset_arr[i].offset[j + 1];

            if ((offset + width) == next_offset) {
                /* Increase the width  */
                offset_arr[i].width[j] += offset_arr[i].width[j + 1];

                /* Remove the next element */
                for (k = j + 1; k < (offset_arr[i].num_offsets - 1); k++) {
                    offset_arr[i].offset[k] = offset_arr[i].offset[k + 1];
                    offset_arr[i].width[k]  = offset_arr[i].width[k + 1];
                }
                offset_arr[i].num_offsets--;
            } else {
                j++;
            }
        }
    }

    sal_memset(msg, 0, sizeof(*msg));
    msg->group_idx = ft_em_group;
    msg->key_size  = (fg->em_mode == _FieldExactMatchMode128) ? 128 : 320;
    msg->num_pipes = ft_info->num_pipes;
    msg->lt_id[0]  = fg->lt_id;
    for (i = 1; i < ft_info->num_pipes; i++) {
        if (!SOC_IS_TOMAHAWK3(unit)) {
            BCM_IF_ERROR_RETURN(_field_group_get(unit, em_group[i], &tmp_fg));
            msg->lt_id[i]  = tmp_fg->lt_id;
        } else {
            /* In TH3, EM is in global mode. So fill up the same LT ID
             * in all pipe information.
             */
            msg->lt_id[i]  = fg->lt_id;
        }
    }


    /* Fill up the tracking params and part of qualifier info  */
    qidx = 0;
    for (i = 0; i < num_tracking_params; i++) {
        switch (tracking_params[i].param) {
            case bcmFlowtrackerTrackingParamTypeSrcIPv4:
                msg->qual_info[qidx].qual = SHR_QCM_EM_QUAL_SRC_IP;
                qidx++;
                break;

            case bcmFlowtrackerTrackingParamTypeDstIPv4:
                msg->qual_info[qidx].qual = SHR_QCM_EM_QUAL_DST_IP;
                qidx++;
                break;

            case bcmFlowtrackerTrackingParamTypeL4SrcPort:
                msg->qual_info[qidx].qual = SHR_QCM_EM_QUAL_L4_SRC_PORT;
                qidx++;
                break;

            case bcmFlowtrackerTrackingParamTypeL4DstPort:
                msg->qual_info[qidx].qual = SHR_QCM_EM_QUAL_L4_DST_PORT;
                qidx++;
                break;

            case bcmFlowtrackerTrackingParamTypeIPProtocol:
                msg->qual_info[qidx].qual = SHR_QCM_EM_QUAL_IP_PROTOCOL;
                qidx++;
                break;

            case bcmFlowtrackerTrackingParamTypeIngPort:
                msg->qual_info[qidx].qual = SHR_QCM_EM_QUAL_ING_PORT;
                qidx++;
                break;

            case bcmFlowtrackerTrackingParamTypeSrcIPv6:
                msg->qual_info[qidx].qual = SHR_QCM_EM_QUAL_SRC_IP;
                qidx++;
                break;

            case bcmFlowtrackerTrackingParamTypeDstIPv6:
                msg->qual_info[qidx].qual = SHR_QCM_EM_QUAL_DST_IP;
                qidx++;
                break;

            default:
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                          (BSL_META_U(unit,
                                      "FT QCM(unit %d) Error: Invalid tracking param: "
                                      "%d\n"),
                   unit, tracking_params[i].param));
                sal_free(qid_arr);
                sal_free(offset_arr);
                return BCM_E_PARAM;
        }
    }

    /* Check if LT ID is an implicit qualifier and add it to the list */
    for (i = 0; i < qual_count; i++) {
        if (qid_arr[i] == _bcmFieldQualifyPreLogicalTableId) {
            rv = _bcm_th_ft_qcm_field_qual_convert(qid_arr[i], &ft_qual);
            BCM_IF_ERROR_RETURN(rv);

            msg->qual_info[qidx].qual = SHR_QCM_EM_QUAL_LT_ID;
            qidx++;
            break;
        }
    }

    msg->num_qual = qidx;
    if (msg->num_qual != qual_count) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT QCM(unit %d) Error: Unexpected number of qualifiers: "
                              "(%u, %u)\n"),
                   unit, msg->num_qual, qual_count));
        sal_free(qid_arr);
        sal_free(offset_arr);
        return BCM_E_PARAM;
    }

    /* Fill the rest of qualifier information */
    for (i = 0; i < qual_count; i++) {
        rv = _bcm_th_ft_qcm_field_qual_convert(qid_arr[i], &ft_qual);
        if (rv != BCM_E_NONE) {
            sal_free(qid_arr);
            sal_free(offset_arr);
            return rv;
        }

        /* Find the info structure corresponding to this qualifier */
        for (j = 0; j < qual_count; j++) {
            if (msg->qual_info[j].qual == ft_qual) {
                qual_info = &(msg->qual_info[j]);
                break;
            }
        }
        if (j >= qual_count) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                      (BSL_META_U(unit,
                                  "FT QCM(unit %d) Error: %u qualifier not found\n"),
                       unit, ft_qual));
            sal_free(qid_arr);
            sal_free(offset_arr);
            return BCM_E_PARAM;
        }

        for (j = 0, k = 0; j < offset_arr[i].num_offsets; j++) {
            offset = offset_arr[i].offset[j];
            width  = offset_arr[i].width[j];

            if (k >= SHR_FT_QCM_EM_KEY_MAX_QUAL_PARTS) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                          (BSL_META_U(unit,
                                      "FT QCM(unit %d) Error: Excess num_offsets than expected "
                                      "num_offsets=%u\n"),
                           unit, k));
                sal_free(qid_arr);
                sal_free(offset_arr);
                return BCM_E_RESOURCE;
            }

            word_boundary = offset + (SHR_FT_EM_KEY_DWORD_SIZE -
                                           (offset % SHR_FT_EM_KEY_DWORD_SIZE));
            if ((offset + width) > word_boundary) {
                /* The key is present in multiple words, split it into different
                 * offsets to help the FW
                 */
                qual_info->offset[k] = offset;
                qual_info->width[k]  = word_boundary - offset;

                do {
                    k++;
                    if (k >= SHR_FT_QCM_EM_KEY_MAX_QUAL_PARTS) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                                  (BSL_META_U(unit,
                                              "FT QCM(unit %d) Error: Excess num_offsets than expected "
                                              "num_offsets=%u\n"),
                                   unit, k));
                        sal_free(qid_arr);
                        sal_free(offset_arr);
                        return BCM_E_RESOURCE;
                    }
                    qual_info->offset[k] = word_boundary;
                    qual_info->width[k]  = width - (word_boundary - offset);
                    word_boundary += SHR_FT_EM_KEY_DWORD_SIZE;
                } while(qual_info->width[k] > SHR_FT_EM_KEY_DWORD_SIZE);

                k++;
                if (k > SHR_FT_QCM_EM_KEY_MAX_QUAL_PARTS) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                              (BSL_META_U(unit,
                                          "FT QCM(unit %d) Error: Excess num_offsets than expected "
                                          "num_offsets=%u\n"),
                               unit, k));
                    sal_free(qid_arr);
                    sal_free(offset_arr);
                    return BCM_E_RESOURCE;
                }
            } else {
                qual_info->offset[k] = offset;
                qual_info->width[k]  = width;
                k++;
                if (k > SHR_FT_QCM_EM_KEY_MAX_QUAL_PARTS) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                              (BSL_META_U(unit,
                                          "FT QCM(unit %d) Error: Excess num_offsets than expected "
                                          "num_offsets=%u\n"),
                               unit, k));
                    sal_free(qid_arr);
                    sal_free(offset_arr);
                    return BCM_E_RESOURCE;
                }
            }
        }
        qual_info->num_parts = k;
    }

    sal_free(qid_arr);
    sal_free(offset_arr);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_fill_em_key_format_msg
 * Purpose:
 *      Convert the EM 5-tuple key format from field data structure to
 *      App message format
 * Parameters:
 *      qual  - (IN)  Qualifier info
 *      msg   - (OUT) Key format message
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_fill_em_key_format_msg(_bcm_field_group_qual_t qual,
                                  shr_ft_msg_ctrl_em_key_format_t *msg)
{
    int    i, j, k;
    uint16 offset, next_offset, width;
    uint8  word_boundary;

    if (qual.size != 5) { /* IPv4 5-tuple */
        return BCM_E_PARAM;
    }

    for (i = 0; i < qual.size; i++) {
        for (j = 0; j < qual.offset_arr[i].num_offsets; j++) {
            if (qual.offset_arr[i].offset[j] > 79) {
                /* In Exact Match Mode 128 - whole key is not presented
                 * for lookup. Instead Bit 0-47 and Bit 80-160 are provided
                 * to UFT. Hence qualifier offsets at position greater than
                 * 80 bit are shifted by 32 bit for key.
                 */
                qual.offset_arr[i].offset[j] -= 32;
            }
        }
    }

    /* Some qual parts are contiguous but field stores them as separate parts,
     * combine them to reduce the number of bitops the FW has to perform on each
     * flow
     */
    for (i = 0; i < qual.size; i++) {
        j = 0;
        while(j < (qual.offset_arr[i].num_offsets - 1)) {
            offset      = qual.offset_arr[i].offset[j];
            width       = qual.offset_arr[i].width[j];
            next_offset = qual.offset_arr[i].offset[j + 1];

            if ((offset + width) == next_offset) {
                /* Increase the width  */
                qual.offset_arr[i].width[j] += qual.offset_arr[i].width[j + 1];

                /* Remove the next element */
                for (k = j + 1; k < (qual.offset_arr[i].num_offsets - 1); k++) {
                    qual.offset_arr[i].offset[k] = qual.offset_arr[i].offset[k + 1];
                    qual.offset_arr[i].width[k]  = qual.offset_arr[i].width[k + 1];
                }
                qual.offset_arr[i].num_offsets--;
            } else {
                j++;
            }
        }
    }

    sal_memset(msg, 0, sizeof(*msg));
    msg->key_size = 128; /* 128 bit view of the EM table is used */
    msg->num_qual = qual.size;

    for (i = 0; i < qual.size; i++) {
        switch (qual.qid_arr[i]) {
            case bcmFieldQualifySrcIp:
                msg->qual[i] = SHR_FT_EM_QUAL_SRC_IPV4;
                break;

            case bcmFieldQualifyDstIp:
                msg->qual[i] = SHR_FT_EM_QUAL_DST_IPV4;
                break;

            case bcmFieldQualifyIpProtocol:
                msg->qual[i] = SHR_FT_EM_QUAL_IP_PROTOCOL;
                break;

            case bcmFieldQualifyL4SrcPort:
                msg->qual[i] = SHR_FT_EM_QUAL_L4_SRC_PORT;
                break;

            case bcmFieldQualifyL4DstPort:
                msg->qual[i] = SHR_FT_EM_QUAL_L4_DST_PORT;
                break;

            default:
                return BCM_E_PARAM;
        }

        for (j = 0, k = 0; j < qual.offset_arr[i].num_offsets; j++) {
            offset = qual.offset_arr[i].offset[j];
            width  = qual.offset_arr[i].width[j];

            word_boundary = offset + (SHR_FT_EM_KEY_DWORD_SIZE -
                                           (offset % SHR_FT_EM_KEY_DWORD_SIZE));
            if ((offset + width) > word_boundary) {
                /* The key is present in multiple words, split it into different
                 * offsets to help the FW
                 */
                msg->qual_parts[i].offset[k] = offset;
                msg->qual_parts[i].width[k]  = word_boundary - offset;

                do {
                    k++;
                    msg->qual_parts[i].offset[k] = word_boundary;
                    msg->qual_parts[i].width[k]  = width - (word_boundary - offset);
                    word_boundary += SHR_FT_EM_KEY_DWORD_SIZE;
                } while(msg->qual_parts[i].width[k] > SHR_FT_EM_KEY_DWORD_SIZE);

                k++;

            } else {
                msg->qual_parts[i].offset[k] = offset;
                msg->qual_parts[i].width[k]  = width;
                k++;
            }
        }
        msg->qual_parts[i].num_parts = k;

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_field_qual_convert
 * Purpose:
 *      Convert the field qualifier to app notation
 * Parameters:
 *      field_qual      - (IN)  Field qualifier
 *      ft_qual         - (OUT) Qualifier in app notation
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_field_qual_convert(int field_qual, shr_ft_em_qual_t *ft_qual)
{
    switch (field_qual) {
        case bcmFieldQualifySrcIp:
            *ft_qual = SHR_FT_EM_QUAL_SRC_IPV4;
            break;

        case bcmFieldQualifyDstIp:
            *ft_qual = SHR_FT_EM_QUAL_DST_IPV4;
            break;

        case bcmFieldQualifyIpProtocol:
            *ft_qual = SHR_FT_EM_QUAL_IP_PROTOCOL;
            break;

        case bcmFieldQualifyL4SrcPort:
            *ft_qual = SHR_FT_EM_QUAL_L4_SRC_PORT;
            break;

        case bcmFieldQualifyL4DstPort:
            *ft_qual = SHR_FT_EM_QUAL_L4_DST_PORT;
            break;

        case bcmFieldQualifyVxlanNetworkId:
            *ft_qual = SHR_FT_EM_QUAL_VNID;
            break;

        case _bcmFieldQualifyData0:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK0;
            break;

        case _bcmFieldQualifyData1:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK1;
            break;

        case _bcmFieldQualifyData2:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK2;
            break;

        case _bcmFieldQualifyData3:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK3;
            break;

        case _bcmFieldQualifyData4:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK4;
            break;

        case _bcmFieldQualifyData5:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK5;
            break;

        case _bcmFieldQualifyData6:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK6;
            break;

        case _bcmFieldQualifyData7:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK7;
            break;

        case _bcmFieldQualifyData8:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK8;
            break;

        case _bcmFieldQualifyData9:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK9;
            break;

        case _bcmFieldQualifyData10:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK10;
            break;

        case _bcmFieldQualifyData11:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK11;
            break;

        case _bcmFieldQualifyData12:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK12;
            break;

        case _bcmFieldQualifyData13:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK13;
            break;

        case _bcmFieldQualifyData14:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK14;
            break;

        case _bcmFieldQualifyData15:
            *ft_qual = SHR_FT_EM_QUAL_UDF_CHUNK15;
            break;

        case _bcmFieldQualifyPreLogicalTableId:
            *ft_qual = SHR_FT_EM_QUAL_LT_ID;
            break;

        case bcmFieldQualifyInPort:
            *ft_qual = SHR_FT_EM_QUAL_IN_PORT;
            break;

        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_ft_em_group_create_msg_udf_get
 * Purpose:
 *      Convert the UDF Id to the format, that app can understand
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      udf_id        - (IN)  UDF Id
 *      hint_id       - (IN)  Hint Id of the group
 *      tp_info       - (OUT) Tracking param info
 *      qual_info     - (OUT) UDF info split into multiple qualifiers
 *      num_qual      - (OUT) Number of qualifers required to hold the UDF info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_td3_ft_em_group_create_msg_udf_get(int unit,
                                        bcm_udf_id_t udf_id,
                                        bcm_field_hintid_t hint_id,
                                        shr_ft_msg_ctrl_tp_info_t *tp_info,
                                        shr_ft_msg_ctrl_qual_info_t *qual_info,
                                        int *num_qual)
{
    bcm_udf_chunk_info_t udf_chunk_info;
    bcm_udf_abstract_pkt_format_info_t pkt_format_info;
    uint16 base_offset;
    int chunk, num_chunks;
    _field_hints_t *f_ht = NULL;
    _field_hint_t *hint_entry = NULL;
    int hint, hint_start = 0, hint_width = 0;
    int rv;

    /* Get the UDF chunk info */
    bcm_udf_chunk_info_t_init(&udf_chunk_info);
    rv = bcm_esw_udf_chunk_info_get(unit, udf_id, &udf_chunk_info);
    BCM_IF_ERROR_RETURN(rv);

    /* Get the base offset used by all the UDF chunks */
    rv = bcm_esw_udf_abstract_pkt_format_info_get(unit,
                                                  udf_chunk_info.abstract_pkt_format,
                                                  &pkt_format_info);
    BCM_IF_ERROR_RETURN(rv);

    switch (pkt_format_info.base_offset) {
        case bcmUdfPktBaseOffsetStartOfOuterL2:
            base_offset = SHR_FT_UDF_BASE_OFFSET_OUTER_L2;
            break;

        case bcmUdfPktBaseOffsetStartOfOuterL3:
            base_offset = SHR_FT_UDF_BASE_OFFSET_OUTER_L3;
            break;

        case bcmUdfPktBaseOffsetStartOfOuterL4:
            base_offset = SHR_FT_UDF_BASE_OFFSET_OUTER_L4;
            break;

        case bcmUdfPktBaseOffsetStartOfOuterL5:
            base_offset = SHR_FT_UDF_BASE_OFFSET_OUTER_L5;
            break;

        default:
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                      (BSL_META_U(unit,
                                  "FT(unit %d) Error: Invalid base offset: "
                                  "%d\n"),
                       unit, pkt_format_info.base_offset));
            return BCM_E_PARAM;
    }

    /* See if there are any hints associated with this Udf Id */
    hint = 0;
    BCM_IF_ERROR_RETURN(_field_hints_control_get (unit, hint_id, &f_ht));
    if (f_ht != NULL) {
        hint_entry = f_ht->hints;
        while (hint_entry != NULL) {
            if ((hint_entry->hint->hint_type == bcmFieldHintTypeExtraction) &&
                (hint_entry->hint->qual      == bcmFieldQualifyUdf)         &&
                (hint_entry->hint->udf_id    == udf_id)) {

                if (((hint_entry->hint->start_bit % 8) != 0) ||
                     (((hint_entry->hint->end_bit + 1) % 8) != 0)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                              (BSL_META_U(unit,
                                          "FT(unit %d) Error: Hints not byte aligned \n"),
                               unit));
                    return BCM_E_PARAM;
                }

                /* UDFs are extracted from the MSB, but hints are specified from the LSB */
                hint = 1;
                hint_start = udf_chunk_info.width - ((hint_entry->hint->end_bit + 1) / 8);
                hint_width = (hint_entry->hint->end_bit - hint_entry->hint->start_bit + 1) / 8;

                if ((udf_chunk_info.width  - hint_width) >
                                 _BCM_UDF_CTRL_OFFSET_GRAN(unit)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                              (BSL_META_U(unit,
                                          "FT(unit %d) Error: Hint removes more "
                                          "than 1 chunk  \n"),
                               unit));
                    return BCM_E_PARAM;
                }

                break;
            }
            hint_entry = hint_entry->next;
        }
    }

    num_chunks = 0;
    *num_qual = 0;
    for (chunk = 0; chunk < _BCM_UDF_CTRL_MAX_UDF_CHUNKS(unit); chunk++) {
        if(udf_chunk_info.chunk_bmap & (1 << chunk)) {
            /* Populate the qualifier info */
            qual_info[*num_qual].qual = _bcm_td3_ft_udf_chunk_to_qual_mapping[chunk];
            qual_info[*num_qual].base_offset = base_offset;
            tp_info->qual[num_chunks] =
                _bcm_td3_ft_udf_chunk_to_qual_mapping[chunk];
            tp_info->udf = 1;


            if (hint) {
                if (num_chunks == 0) {
                    qual_info[*num_qual].relative_offset =
                                      udf_chunk_info.offset + hint_start;
                } else {
                    qual_info[*num_qual].relative_offset =
                        qual_info[*num_qual - 1].relative_offset + _BCM_UDF_CTRL_OFFSET_GRAN(unit);
                }

                if (hint_width <= _BCM_UDF_CTRL_OFFSET_GRAN(unit)) {
                    qual_info[*num_qual].width = hint_width;
                } else {
                    hint_width -= _BCM_UDF_CTRL_OFFSET_GRAN(unit);
                    qual_info[*num_qual].width = _BCM_UDF_CTRL_OFFSET_GRAN(unit);
                }
            } else {
                qual_info[*num_qual].width = _BCM_UDF_CTRL_OFFSET_GRAN(unit);
                qual_info[*num_qual].relative_offset =
                                 (_BCM_UDF_CTRL_OFFSET_GRAN(unit) * num_chunks) +
                                   udf_chunk_info.offset;
            }

            num_chunks++;
            (*num_qual)++;
        }
    }
    tp_info->num_qual = num_chunks;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_em_group_create_msg_udf_get
 * Purpose:
 *      Convert the UDF Id to the format, that app can understand
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      udf_id        - (IN)  UDF Id
 *      hint_id       - (IN)  Hint Id of the group
 *      tp_info       - (OUT) Tracking param info
 *      qual_info     - (OUT) UDF info split into multiple qualifiers
 *      num_qual      - (OUT) Number of qualifers required to hold the UDF info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_th_ft_em_group_create_msg_udf_get(int unit,
                                       bcm_udf_id_t udf_id,
                                        bcm_field_hintid_t hint_id,
                                       shr_ft_msg_ctrl_tp_info_t *tp_info,
                                       shr_ft_msg_ctrl_qual_info_t *qual_info,
                                       int *num_qual)
{
    bcmi_xgs4_udf_offset_info_t *offset_info = NULL;
    uint16 base_offset;
    int chunk, num_chunks;
    int chunk_size;
    _field_hints_t *f_ht = NULL;
    _field_hint_t *hint_entry = NULL;
    int hint, hint_start = 0, hint_width = 0;
    int rv;

    rv = bcmi_xgs4_udf_offset_node_get(unit, udf_id, &offset_info);
    BCM_IF_ERROR_RETURN(rv);

    if (((offset_info->start % 2) != 0) || ((offset_info->width % 2) != 0)) {
        /* UDF width & start must be aligned to 2B boundary. Hints need to be
         * used to mask off the unneeded bits
         */
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: UDF start (%u) / width (%u)"
                              "not aligned to 2B \n"),
                   unit, offset_info->start, offset_info->width));
        return BCM_E_PARAM;
    }


    switch (offset_info->layer) {
        case bcmUdfLayerL2Header:
            base_offset = SHR_FT_UDF_BASE_OFFSET_OUTER_L2;
            break;

        case bcmUdfLayerL3OuterHeader:
            base_offset = SHR_FT_UDF_BASE_OFFSET_OUTER_L3;
            break;

        case bcmUdfLayerL4OuterHeader:
            base_offset = SHR_FT_UDF_BASE_OFFSET_OUTER_L4;
            break;

        default:
            return BCM_E_UNAVAIL;
    }

    /* See if there are any hints associated with this Udf Id */
    hint = 0;
    BCM_IF_ERROR_RETURN(_field_hints_control_get (unit, hint_id, &f_ht));
    if (f_ht != NULL) {
        hint_entry = f_ht->hints;
        while (hint_entry != NULL) {
            if ((hint_entry->hint->hint_type == bcmFieldHintTypeExtraction) &&
                (hint_entry->hint->qual      == bcmFieldQualifyUdf)         &&
                (hint_entry->hint->udf_id    == udf_id)) {

                if (((hint_entry->hint->start_bit % 8) != 0) ||
                     (((hint_entry->hint->end_bit + 1) % 8) != 0)) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                              (BSL_META_U(unit,
                                          "FT(unit %d) Error: Hints not byte aligned \n"),
                               unit));
                    return BCM_E_PARAM;
                }

                /* UDFs are extracted from the MSB, but hints are specified from the LSB */
                hint = 1;
                hint_start = offset_info->width - ((hint_entry->hint->end_bit + 1) / 8);
                hint_width = (hint_entry->hint->end_bit - hint_entry->hint->start_bit + 1) / 8;

                if ((offset_info->width  - hint_width) > 2) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                              (BSL_META_U(unit,
                                          "FT(unit %d) Error: Hint removes more "
                                          "than 2B  \n"),
                               unit));
                    return BCM_E_PARAM;
                }

                break;
            }
            hint_entry = hint_entry->next;
        }
    }

    if (!hint) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Cannot find hints for "
                              "UDF Id (%d) \n"),
                   unit, udf_id));
        return BCM_E_NOT_FOUND;
    }

    chunk          = 0;
    num_chunks     = 0;
    *num_qual      = 0;

    while (chunk < _BCM_FT_TH_MAX_UDF_CHUNKS) {
        /* Get the chunk size in bytes */
        if (SOC_IS_TOMAHAWK3(unit)) {
            chunk_size = FT_TH3_UDF_CHUNK_SIZE;
        } else {
            chunk_size = _bcm_th_ft_udf_chunk_info[chunk].size;
        }

        if(offset_info->hw_bmap & (1 << chunk)) {
            /* Fill in the qual details in the message and TP info */
            if (SOC_IS_TOMAHAWK3(unit)) {
                qual_info[*num_qual].qual = _bcm_td3_ft_udf_chunk_to_qual_mapping[chunk];
                tp_info->qual[num_chunks] = _bcm_td3_ft_udf_chunk_to_qual_mapping[chunk];
            } else {
                qual_info[*num_qual].qual = _bcm_th_ft_udf_chunk_info[chunk].qual;
                tp_info->qual[num_chunks] = _bcm_th_ft_udf_chunk_info[chunk].qual;
            }
            qual_info[*num_qual].base_offset = base_offset;

            tp_info->udf = 1;

            if (num_chunks == 0) {
                qual_info[*num_qual].relative_offset = offset_info->start + hint_start;
            } else {
                qual_info[*num_qual].relative_offset =
                    qual_info[*num_qual - 1].relative_offset + qual_info[*num_qual - 1].width;
            }

            if (hint_width <= chunk_size) {
                qual_info[*num_qual].width = hint_width;
            } else {
                hint_width -= chunk_size;
                qual_info[*num_qual].width = chunk_size;
            }


            num_chunks++;
            (*num_qual)++;
        }
        chunk += (chunk_size / 2);
    }

    tp_info->num_qual = num_chunks;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_fill_em_group_create_msg
 * Purpose:
 *      Convert the EM key format from field data structure to
 *      App message format
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      msg           - (OUT) Key format message
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_th_ft_fill_em_group_create_msg(int unit,
                                    bcm_field_group_t *em_group,
                                    uint8 ft_em_group,
                                    int num_tracking_params,
                                    bcm_flowtracker_tracking_param_info_t *tracking_params,
                                    shr_ft_msg_ctrl_em_group_create_t *msg)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_field_group_qual_t **qual_arr;
    uint16 *qid_arr, qid;
    _bcm_field_qual_offset_t *offset_arr;
    int num_parts_count, max_parts_count;
    int i, j, k, qidx, offset_idx;
    uint16 offset, next_offset, width;
    uint16 word_boundary;
    int rv;
    int udf;
    int num_qual, qual_count, em_qual_count;
    shr_ft_em_qual_t ft_qual;
    shr_ft_msg_ctrl_qual_info_t *qual_info;
    _field_group_t *fg, *tmp_fg;
    int action;
    int key_boundary = 79;

    if (soc_feature(unit, soc_feature_th3_style_fp)) {
        key_boundary = 51;
    }

    BCM_IF_ERROR_RETURN(_field_group_get(unit, em_group[0], &fg));

    if (fg->stage_id != _BCM_FIELD_STAGE_EXACTMATCH) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group Stage is not ExactMatch \n"),
                   unit));
        return BCM_E_PARAM;
    }

    if ((fg->em_mode != _FieldExactMatchMode128) &&
        (fg->em_mode != _FieldExactMatchMode320)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Only 128/320b EM view is supported \n"),
                   unit));
        return BCM_E_PARAM;
    }

    qual_arr = sal_alloc(_FP_MAX_ENTRY_TYPES * sizeof(_bcm_field_group_qual_t*),
                         "FT EM qual_arr");
    if (qual_arr == NULL) {
        return BCM_E_MEMORY;
    }
    for (i = 0; i < _FP_MAX_ENTRY_TYPES; i++) {
        qual_arr[i] = sal_alloc(_FP_MAX_ENTRY_WIDTH * sizeof(_bcm_field_group_qual_t),
                                "FT EM qual_arr");
        if (qual_arr[i] == NULL) {
            for (j = 0; j < i; j++) {
                sal_free(qual_arr[j]);
            }
            return BCM_E_MEMORY;
        }
    }

    /* Get the EM key format */
    if (fg->em_mode == _FieldExactMatchMode128) {
        max_parts_count = 1;
    } else {
        max_parts_count = 2;
    }
    rv = _bcm_esw_field_group_info_key_get(unit, em_group[0],
                                           max_parts_count, qual_arr,
                                           &num_parts_count);
    if (BCM_FAILURE(rv)) {
        for (i = 0; i < _FP_MAX_ENTRY_TYPES; i++) {
            sal_free(qual_arr[i]);
        }
        sal_free(qual_arr);
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: _bcm_esw_field_group_info_key_get "
                              "failed rv=%d\n"),
                   unit, rv));
        return rv;
    }

    /* Get the total number of qualifiers across all the parts. This may include
     * duplicates if a single qualifier is split across multiple parts
     */
    em_qual_count = 0;
    for (i  = 0; i < num_parts_count; i++) {
        em_qual_count += qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size;
    }

    qid_arr = sal_alloc(em_qual_count * sizeof(uint16), "FT EM qid arr");
    if (qid_arr == NULL) {
        for (i = 0; i < _FP_MAX_ENTRY_TYPES; i++) {
            sal_free(qual_arr[i]);
        }
        sal_free(qual_arr);
        return BCM_E_MEMORY;
    }
    sal_memset(qid_arr, 0, em_qual_count * sizeof(uint16));

    offset_arr = sal_alloc(em_qual_count * sizeof(_bcm_field_qual_offset_t),
                           "FT EM qid arr");
    if (offset_arr == NULL) {
        sal_free(qid_arr);
        for (i = 0; i < _FP_MAX_ENTRY_TYPES; i++) {
            sal_free(qual_arr[i]);
        }
        sal_free(qual_arr);
        return BCM_E_MEMORY;
    }
    sal_memset(offset_arr, 0, em_qual_count * sizeof(_bcm_field_qual_offset_t));

    /* Copy over the first part */
    sal_memcpy(qid_arr, qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].qid_arr,
               qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].size * sizeof(uint16));
    sal_memcpy(offset_arr, qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].offset_arr,
               qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].size * sizeof(_bcm_field_qual_offset_t));
    qual_count = qual_arr[_FP_ENTRY_TYPE_DEFAULT][0].size;

    /* Copy over the remaining parts, adjust the offset and filter duplicates */
    for (i = 1; i < num_parts_count; i++) {
        for (j = 0; j < qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size; j++) {
            qid = qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr[j];

            /* Check if it is a case of a single qualifier split across
             * multiple parts
             */
            for (k = 0; k < qual_count; k++) {
                if (qid_arr[k] == qid) {
                    qidx = k;
                    break;
                }
            }
            if (k >= qual_count) {
                /* Not a duplicate */
                qidx = qual_count;
                qual_count++;
            }
            qid_arr[qidx] = qid;
            /* Insert to the list */
            for (k = 0, offset_idx = offset_arr[qidx].num_offsets;
                 k < qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[j].num_offsets;
                 k++, offset_idx++) {
                offset_arr[qidx].offset[offset_idx] =
                        160 * i + qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[j].offset[k];
                offset_arr[qidx].width[offset_idx] =
                           qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[j].width[k];
            }
            offset_arr[qidx].num_offsets = offset_idx;
        }
    }

    for (i = 0; i < _FP_MAX_ENTRY_TYPES; i++) {
        sal_free(qual_arr[i]);
    }
    sal_free(qual_arr);

    /* Check if any qualifier has more parts that the app can handle */
    for (qidx = 0; qidx < qual_count; qidx++) {
        if (offset_arr[qidx].num_offsets > SHR_FT_EM_KEY_MAX_QUAL_PARTS) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                      (BSL_META_U(unit,
                              "FT(unit %d) Error: Excess num_offsets than expected "
                              "num_offsets=%u\n"),
                   unit, offset_arr[qidx].num_offsets));
            return BCM_E_PARAM;
        }
    }

    if (qual_count >= SHR_FT_EM_MAX_QUAL) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Excess qualifiers than expected "
                              "qual_count=%u\n"),
                   unit, qual_count));
        sal_free(qid_arr);
        sal_free(offset_arr);
        return BCM_E_PARAM;
    }

    if (fg->em_mode == _FieldExactMatchMode128) {
        /* In Exact Match Mode 128 - whole key is not presented
         * for lookup. Instead Bit 0-47 and Bit 80-160 are provided
         * to UFT. Hence qualifier offsets at position greater than
         * 80 bit are shifted by 32 bit for key.
         */
        for (i = 0; i < qual_count; i++) {
            for (j = 0; j < offset_arr[i].num_offsets; j++) {
                if (offset_arr[i].offset[j] > key_boundary) {
                    offset_arr[i].offset[j] -= 32;
                }
            }
        }
    }
    /* Some qual parts are contiguous but field stores them as separate parts,
     * combine them to reduce the number of bitops the FW has to perform on each
     * flow
     */
    for (i = 0; i < qual_count; i++) {
        j = 0;
        while(j < (offset_arr[i].num_offsets - 1)) {
            offset      = offset_arr[i].offset[j];
            width       = offset_arr[i].width[j];
            next_offset = offset_arr[i].offset[j + 1];

            if ((offset + width) == next_offset) {
                /* Increase the width  */
                offset_arr[i].width[j] += offset_arr[i].width[j + 1];

                /* Remove the next element */
                for (k = j + 1; k < (offset_arr[i].num_offsets - 1); k++) {
                    offset_arr[i].offset[k] = offset_arr[i].offset[k + 1];
                    offset_arr[i].width[k]  = offset_arr[i].width[k + 1];
                }
                offset_arr[i].num_offsets--;
            } else {
                j++;
            }
        }
    }

    sal_memset(msg, 0, sizeof(*msg));
    msg->group_idx = ft_em_group;
    msg->num_pipes = ft_info->num_pipes;
    msg->lt_id[0]  = fg->lt_id;
    for (i = 1; i < ft_info->num_pipes; i++) {
        if (!SOC_IS_TOMAHAWK3(unit)) {
            BCM_IF_ERROR_RETURN(_field_group_get(unit, em_group[i], &tmp_fg));
            msg->lt_id[i]  = tmp_fg->lt_id;
        } else {
            /* In TH3, EM is in global mode. So fill up the same LT ID
             * in all pipe information.
             */
            msg->lt_id[i]  = fg->lt_id;
        }
    }
    msg->key_size  = (fg->em_mode == _FieldExactMatchMode128) ? 128 : 320;

    /* Fill up the tracking params and part of qualifier info  */
    qidx = 0;
    msg->num_tp = num_tracking_params;
    for (i = 0; i < num_tracking_params; i++) {
        udf  = 0;
        switch (tracking_params[i].param) {
            case bcmFlowtrackerTrackingParamTypeSrcIPv4:
                msg->tp_info[i].param     = SHR_FT_TP_SRC_IPV4;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = SHR_FT_EM_QUAL_SRC_IPV4;
                msg->qual_info[qidx].qual = SHR_FT_EM_QUAL_SRC_IPV4;
                qidx++;
                break;

            case bcmFlowtrackerTrackingParamTypeDstIPv4:
                msg->tp_info[i].param     = SHR_FT_TP_DST_IPV4;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = SHR_FT_EM_QUAL_DST_IPV4;
                msg->qual_info[qidx].qual = SHR_FT_EM_QUAL_DST_IPV4;
                qidx++;
                break;

            case bcmFlowtrackerTrackingParamTypeL4SrcPort:
                msg->tp_info[i].param     = SHR_FT_TP_L4_SRC_PORT;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = SHR_FT_EM_QUAL_L4_SRC_PORT;
                msg->qual_info[qidx].qual = SHR_FT_EM_QUAL_L4_SRC_PORT;
                qidx++;
                break;

            case bcmFlowtrackerTrackingParamTypeL4DstPort:
                msg->tp_info[i].param     = SHR_FT_TP_L4_DST_PORT;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = SHR_FT_EM_QUAL_L4_DST_PORT;
                msg->qual_info[qidx].qual = SHR_FT_EM_QUAL_L4_DST_PORT;
                qidx++;
                break;

            case bcmFlowtrackerTrackingParamTypeIPProtocol:
                msg->tp_info[i].param     = SHR_FT_TP_IP_PROTOCOL;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = SHR_FT_EM_QUAL_IP_PROTOCOL;
                msg->qual_info[qidx].qual = SHR_FT_EM_QUAL_IP_PROTOCOL;
                qidx++;
                break;

            case bcmFlowtrackerTrackingParamTypeVxlanNetworkId:
                msg->tp_info[i].param     = SHR_FT_TP_VNID;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = SHR_FT_EM_QUAL_VNID;
                msg->qual_info[qidx].qual = SHR_FT_EM_QUAL_VNID;
                qidx++;
                break;

            case bcmFlowtrackerTrackingParamTypeInnerSrcIPv4:
                msg->tp_info[i].param = SHR_FT_TP_INNER_SRC_IPV4;
                udf                   = 1;
                break;

            case bcmFlowtrackerTrackingParamTypeInnerDstIPv4:
                msg->tp_info[i].param = SHR_FT_TP_INNER_DST_IPV4;
                udf                   = 1;
                break;

            case bcmFlowtrackerTrackingParamTypeInnerSrcIPv6:
                msg->tp_info[i].param = SHR_FT_TP_INNER_SRC_IPV6;
                udf                   = 1;
                break;

            case bcmFlowtrackerTrackingParamTypeInnerDstIPv6:
                msg->tp_info[i].param = SHR_FT_TP_INNER_DST_IPV6;
                udf                   = 1;
                break;

            case bcmFlowtrackerTrackingParamTypeInnerL4SrcPort:
                msg->tp_info[i].param = SHR_FT_TP_INNER_L4_SRC_PORT;
                udf                   = 1;
                break;

            case bcmFlowtrackerTrackingParamTypeInnerL4DstPort:
                msg->tp_info[i].param = SHR_FT_TP_INNER_L4_DST_PORT;
                udf                   = 1;
                break;

            case bcmFlowtrackerTrackingParamTypeInnerIPProtocol:
                msg->tp_info[i].param = SHR_FT_TP_INNER_IP_PROTOCOL;
                udf                   = 1;
                break;

            case bcmFlowtrackerTrackingParamTypeCustom:
                msg->tp_info[i].param = SHR_FT_TP_CUSTOM;
                udf                   = 1;
                break;

            case bcmFlowtrackerTrackingParamTypeIngPort:
                msg->tp_info[i].param = SHR_FT_TP_IN_PORT;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = SHR_FT_EM_QUAL_IN_PORT;
                msg->qual_info[qidx].qual = SHR_FT_EM_QUAL_IN_PORT;
                qidx++;
                break;

            default:
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                          (BSL_META_U(unit,
                                      "FT(unit %d) Error: Invalid tracking param: "
                                      "%d\n"),
                   unit, tracking_params[i].param));
                sal_free(qid_arr);
                sal_free(offset_arr);
                return BCM_E_PARAM;
        }

        if (udf == 1) {
            if (soc_feature(unit, soc_feature_udf_td3x_support)) {
                rv = _bcm_td3_ft_em_group_create_msg_udf_get(unit,
                                                             tracking_params[i].udf_id,
                                                             fg->hintid,
                                                             &(msg->tp_info[i]),
                                                             &(msg->qual_info[qidx]),
                                                             &num_qual);
            } else if (soc_feature(unit, soc_feature_udf_support)) {
                rv = _bcm_th_ft_em_group_create_msg_udf_get(unit,
                                                            tracking_params[i].udf_id,
                                                            fg->hintid,
                                                            &(msg->tp_info[i]),
                                                            &(msg->qual_info[qidx]),
                                                            &num_qual);
            } else {
                rv =  BCM_E_UNAVAIL;
            }
            if (rv != BCM_E_NONE) {
                sal_free(qid_arr);
                sal_free(offset_arr);
                return rv;
            }
            qidx += num_qual;
        }
    }

    /* Check if LT ID is an implicit qualifier and add it to the list */
    for (i = 0; i < qual_count; i++) {
        if (qid_arr[i] == _bcmFieldQualifyPreLogicalTableId) {
            rv = _bcm_th_ft_field_qual_convert(qid_arr[i], &ft_qual);
            BCM_IF_ERROR_RETURN(rv);

            msg->qual_info[qidx].qual = SHR_FT_EM_QUAL_LT_ID;
            qidx++;
            break;
        }
    }

    msg->num_qual = qidx;

    if (msg->num_qual != qual_count) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Unexpected number of qualifiers: "
                              "(%u, %u)\n"),
                   unit, msg->num_qual, qual_count));
        sal_free(qid_arr);
        sal_free(offset_arr);
        return BCM_E_PARAM;
    }

    /* Fill the rest of qualifier information */
    for (i = 0; i < qual_count; i++) {
        rv = _bcm_th_ft_field_qual_convert(qid_arr[i], &ft_qual);
        if (rv != BCM_E_NONE) {
            sal_free(qid_arr);
            sal_free(offset_arr);
            return rv;
        }

        /* Find the info structure corresponding to this qualifier */
        for (j = 0; j < qual_count; j++) {
            if (msg->qual_info[j].qual == ft_qual) {
                qual_info = &(msg->qual_info[j]);
                break;
            }
        }
        if (j >= qual_count) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                      (BSL_META_U(unit,
                                  "FT(unit %d) Error: %u qualifier not found\n"),
                       unit, ft_qual));
            sal_free(qid_arr);
            sal_free(offset_arr);
            return BCM_E_PARAM;
        }

        for (j = 0, k = 0; j < offset_arr[i].num_offsets; j++) {
            offset = offset_arr[i].offset[j];
            width  = offset_arr[i].width[j];

            if (k >= SHR_FT_EM_KEY_MAX_QUAL_PARTS) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                          (BSL_META_U(unit,
                                      "FT(unit %d) Error: Excess num_offsets than expected "
                                      "num_offsets=%u\n"),
                           unit, k));
                return BCM_E_RESOURCE;
            }

            word_boundary = offset + (SHR_FT_EM_KEY_DWORD_SIZE -
                                           (offset % SHR_FT_EM_KEY_DWORD_SIZE));
            if ((offset + width) > word_boundary) {
                /* The key is present in multiple words, split it into different
                 * offsets to help the FW
                 */
                qual_info->part_offset[k] = offset;
                qual_info->part_width[k]  = word_boundary - offset;

                do {
                    k++;
                    if (k >= SHR_FT_EM_KEY_MAX_QUAL_PARTS) {
                        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                                  (BSL_META_U(unit,
                                              "FT(unit %d) Error: Excess num_offsets than expected "
                                              "num_offsets=%u\n"),
                                   unit, k));
                        return BCM_E_RESOURCE;
                    }
                    qual_info->part_offset[k] = word_boundary;
                    qual_info->part_width[k]  = width - (word_boundary - offset);
                    word_boundary += SHR_FT_EM_KEY_DWORD_SIZE;
                } while(qual_info->part_width[k] > SHR_FT_EM_KEY_DWORD_SIZE);

                k++;
                if (k > SHR_FT_EM_KEY_MAX_QUAL_PARTS) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                              (BSL_META_U(unit,
                                          "FT(unit %d) Error: Excess num_offsets than expected "
                                          "num_offsets=%u\n"),
                               unit, k));
                    return BCM_E_RESOURCE;
                }
            } else {
                qual_info->part_offset[k] = offset;
                qual_info->part_width[k]  = width;
                k++;
                if (k > SHR_FT_EM_KEY_MAX_QUAL_PARTS) {
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                              (BSL_META_U(unit,
                                          "FT(unit %d) Error: Excess num_offsets than expected "
                                          "num_offsets=%u\n"),
                               unit, k));
                    return BCM_E_RESOURCE;
                }
            }
        }
        qual_info->num_parts = k;

    }

    /* Populate the ASET */
    for (action = 0; action < bcmFieldActionCount; action++) {
        if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionPolicerGroup)) {
            msg->aset |= SHR_FT_EM_ACTION_METER;
        } else if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionStatGroup)) {
            msg->aset |= SHR_FT_EM_ACTION_CTR;
        } else {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                      (BSL_META_U(unit,
                                  "FT(unit %d) Error: Invalid action: %u \n"),
                       unit, action));
            sal_free(qid_arr);
            sal_free(offset_arr);
            return BCM_E_PARAM;
        }
    }

    sal_free(qid_arr);
    sal_free(offset_arr);
    return BCM_E_NONE;
}

/*
  * Function:
 *      _bcm_th_ft_create_profile_mem
 * Purpose:
 *      Create profile memory for the Exact Match QoS profile
 * Parameters:
 *      unit          - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int _bcm_th_ft_create_profile_mem(int unit)
{
    int                                      rv;
    _bcm_int_ft_info_t                      *ft_info = FT_INFO_GET(unit);
    int                                      pipe;
    soc_profile_mem_t                       *profile_mem = NULL;
    soc_mem_t                                pipe_mem;
    exact_match_qos_actions_profile_entry_t  qos_prof_entry = {{0}};
    void                                    *profile_entry[1];
    int                                      entry_words;
    uint32                                   qos_index;

    /* Create profile memory for the Exact Match QoS profile memory */
    for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
        entry_words = soc_mem_entry_words(unit,
                                          EXACT_MATCH_QOS_ACTIONS_PROFILE_PIPE0m);
        pipe_mem = SOC_MEM_UNIQUE_ACC(unit,
                                      EXACT_MATCH_QOS_ACTIONS_PROFILEm)[pipe];

        profile_mem = sal_alloc(sizeof(soc_profile_mem_t), " FT QoS profile");
        if (profile_mem == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(profile_mem);

        rv = soc_profile_mem_create(unit, &pipe_mem, &entry_words, 1,
                                    profile_mem);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }

        if (SOC_WARM_BOOT(unit)) {
            /* Increment the ref count of the reserved entry (0) */
            rv = soc_profile_mem_reference(unit, profile_mem, 0, 1);
        } else {
            /* Reserve the 0th entry for default action (no QoS actions) */
            profile_entry[0] = &qos_prof_entry;
            rv = soc_profile_mem_add(unit, profile_mem, (void *) &profile_entry,
                                     1, &qos_index);
        }
        if (BCM_FAILURE(rv)) {
            return (rv);
        }

        ft_info->pipe_info[pipe].qos_profile_mem = profile_mem;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_field_config_init
 * Purpose:
 *      Create EM group and export to R5
 * Parameters:
 *      unit          - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int _bcm_th_ft_field_config_init(int unit)
{
    int                                      rv;
    bcm_field_group_config_t                 group_config;
    _bcm_field_group_qual_t                **qual_arr;
    int                                      num_parts_count;
    int                                      pipe;
    int                                      i, j;
    shr_ft_msg_ctrl_em_key_format_t          em_key_format;
    bcm_port_config_t                        pc;
    _bcm_int_ft_info_t                      *ft_info = FT_INFO_GET(unit);

    if (!SOC_WARM_BOOT(unit)) {
        /* Create the EM group, with 5-tuple as qual and counter as action */
        if (!soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
            BCM_IF_ERROR_RETURN(
                    bcm_esw_field_group_oper_mode_set(unit,
                                        bcmFieldQualifyStageIngressExactMatch,
                                             bcmFieldGroupOperModePipeLocal));
        }
        bcm_port_config_t_init(&pc);
        BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &pc));


        for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
            bcm_field_group_config_t_init(&group_config);

            if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
                group_config.flags = BCM_FIELD_GROUP_CREATE_WITH_ID   |
                                     BCM_FIELD_GROUP_CREATE_WITH_ASET;
                group_config.priority = 10;
            } else {
                group_config.flags = BCM_FIELD_GROUP_CREATE_WITH_ID   |
                                     BCM_FIELD_GROUP_CREATE_WITH_ASET |
                                     BCM_FIELD_GROUP_CREATE_WITH_PORT;
            }

            group_config.group = _FP_FLOWTRACKER_RESERVED_EM_GID_MIN + pipe;
            group_config.mode  = bcmFieldGroupModeAuto;
            if (BCM_PBMP_IS_NULL(pc.per_pipe[pipe])) {
                /* No ports in this pipe */
                continue;
            } else {
                BCM_PBMP_ASSIGN(group_config.ports, pc.per_pipe[pipe]);
            }

            /* Set EM qset to 5-tuple */
            BCM_FIELD_QSET_INIT(group_config.qset);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngressExactMatch);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifySrcIp);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyDstIp);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIpProtocol);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyL4SrcPort);
            BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyL4DstPort);

            /* Set EM aset */
            BCM_FIELD_ASET_INIT(group_config.aset);

            /* Enable counter set in action profile for flex counter update */
            if (!soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
                BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionStatGroup);
            }

            /* Enable METER_SET in action_profile for EM to see incoming color
             * for ETRAP Color Aware actions
             */
            BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionPolicerGroup);

            rv = _bcm_esw_field_group_config_create(unit, &group_config,
                                                    _FP_GROUP_CONFIG_FLOWTRACKER_SPECIFIC);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
                group_config.group = _FP_FLOWTRACKER_RESERVED_EM_GID_MIN + ft_info->num_pipes;
                group_config.priority = 10 + ft_info->num_pipes;
                rv = _bcm_esw_field_group_config_create(unit, &group_config,
                                                        _FP_GROUP_CONFIG_FLOWTRACKER_SPECIFIC);
                if (BCM_FAILURE(rv)) {
                    return rv;
                }
            }
        }

        /* Key format export to app */
        qual_arr = sal_alloc(_FP_MAX_ENTRY_TYPES * sizeof(_bcm_field_group_qual_t*),
                             "FT EM qual_arr");
        if (qual_arr == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < _FP_MAX_ENTRY_TYPES; i++) {
            qual_arr[i] = sal_alloc(_FP_MAX_ENTRY_WIDTH * sizeof(_bcm_field_group_qual_t),
                                    "FT EM qual_arr");
            if (qual_arr[i] == NULL) {
                for (j = 0; j < i; j++) {
                    sal_free(qual_arr[j]);
                }
                return BCM_E_MEMORY;
            }
        }


        /* Get the EM key format */
        rv = _bcm_esw_field_group_info_key_get(unit,
                                               _FP_FLOWTRACKER_RESERVED_EM_GID_MIN,
                                               1, qual_arr, &num_parts_count);
        if (BCM_FAILURE(rv)) {
            for (i = 0; i < _FP_MAX_ENTRY_TYPES; i++) {
                sal_free(qual_arr[i]);
            }
            sal_free(qual_arr);
            return rv;
        }

        /* Fill up the EM KEY, key has only one part */
        rv = _bcm_th_ft_fill_em_key_format_msg(qual_arr[_FP_ENTRY_TYPE_DEFAULT][0],
                                               &em_key_format);

        for (i = 0; i < _FP_MAX_ENTRY_TYPES; i++) {
            sal_free(qual_arr[i]);
        }
        sal_free(qual_arr);

        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /* Send the EM Key to EAPP */
        rv = _bcm_xgs5_ft_eapp_send_em_key_format_msg(unit, &em_key_format);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_field_config_detach
 * Purpose:
 *      Delete EM group
 * Parameters:
 *      unit          - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int _bcm_th_ft_field_config_detach(int unit)
{
    int rv;
    int pipe;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (!SOC_WARM_BOOT(unit)) {
        for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
            rv = _bcm_esw_field_group_config_destroy(
                                                     unit,
                                                     _FP_FLOWTRACKER_RESERVED_EM_GID_MIN + pipe,
                                                     _FP_GROUP_CONFIG_FLOWTRACKER_SPECIFIC);

            if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                /* Ignore BCM_E_NOT_FOUND as the group may not have been
                 * created yet
                 */
                return rv;
            }
            if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
                rv = _bcm_esw_field_group_config_destroy(
                                                         unit,
                                                         _FP_FLOWTRACKER_RESERVED_EM_GID_MIN + ft_info->num_pipes,
                                                         _FP_GROUP_CONFIG_FLOWTRACKER_SPECIFIC);
                if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
                    /* Ignore BCM_E_NOT_FOUND as the group may not have been
                     * created yet
                     */
                    return rv;
                }
            }
        }

        /* Set the EM group mode back to global (default) */
        BCM_IF_ERROR_RETURN(
            bcm_esw_field_group_oper_mode_set(unit,
                                              bcmFieldQualifyStageIngressExactMatch,
                                              bcmFieldGroupOperModeGlobal));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _soc_th_ft_em_ser_handler
 * Purpose:
 *      Handle SER error on EM table
 * Parameters:
 *      unit      - (IN) BCM device number
 *      pipe      - (IN) Pipe index
 *      mem       - (IN) Memory (EM)
 *      index     - (IN) Index
 * Returns:
 *      SOC_E_XXX     - BCM error code.
 */
int
_soc_th_ft_em_ser_handler(int unit, int pipe, soc_mem_t mem, int index)
{
    int        rv         = SOC_E_NONE;
    void      *null_entry = NULL;
    soc_mem_t  pipe_mem;

    if ((mem != FPEM_ECCm) && (mem != EXACT_MATCH_ECCm)) {
        return SOC_E_PARAM;
    }
    /* Convert to pipe unique view */
    pipe_mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];

    /* Clear out the entry */
    null_entry = soc_mem_entry_null(unit, pipe_mem);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, pipe_mem, MEM_BLOCK_ALL, index, null_entry));

    /* Flowtracker module uses the double wide EXACT_MATCH_2 view, clear out
     * the other half also
     */
    if ((index % 2) == 0) {
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, pipe_mem, MEM_BLOCK_ALL, index + 1, null_entry));
    } else {
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, pipe_mem, MEM_BLOCK_ALL, index - 1, null_entry));
    }

    /* Send the SER event to EAPP */
    rv = _bcm_xgs5_ft_eapp_send_ser_event_msg(unit, pipe, mem, (index / 2));
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_export_interval_validate
 * Purpose:
 *      Common validations for export interval.
 * Parameters:
 *      unit            - (IN) BCM device number
 *      export_interval - (IN) Export interval configured.
 * Returns:
 *      BCM_E_XXX     - BCM error code.
 */
int
_bcm_th_ft_export_interval_validate(int unit, uint32 export_interval)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (ft_info->elph_mode) {
        if (export_interval < BCM_INT_FT_ELPH_DEF_EXPORT_INTERVAL_MSECS) {
            return BCM_E_CONFIG;
        }
    } else {
        if (export_interval < BCM_INT_FT_DEF_EXPORT_INTERVAL_MSECS) {
            return BCM_E_CONFIG;
        }
    }

    if ((export_interval * 1000) < ft_info->scan_interval_usecs) {
        /* Export interval cannot be smaller than scan interval */
        return BCM_E_CONFIG;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th_ft_cfg_mode_v1_supported
 * Purpose:
 *      Is API cfg mode v1 supported or not.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      1 - Supported, 0 - Not supported.
 */

uint8 _bcm_th_ft_cfg_mode_v1_supported(int unit)
{
    if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MAVERICK2(unit)) {
        return 0;
    }
    return 1;
}

/*
 * Function:
 *      bcm_th_ft_init
 * Purpose:
 *      Initialize the Flowtracker subsystem.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_init(int unit)
{
    int flowtracker_enable = 0;
    int rv = BCM_E_NONE, pipe, pool_idx;
    bcm_stat_flex_flowtracker_counter_info_t flex_ctr_info;
    _bcm_int_ft_pipe_info_t *pipe_info = NULL;
    bcm_stat_flex_flowtracker_pool_info_t pool_info;
    char max_flows_pipe_soc_prop_str[50];
    int pipe_flow_limit;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    uint8 cfg_mode;
    int i, j;
    int qcm_enable = 0;

    flowtracker_enable = soc_property_get(unit, spn_FLOWTRACKER_ENABLE, 0);
    switch (flowtracker_enable) {
        case 0:
            /* Flowtracker not enabled, return */
            return BCM_E_NONE;

        case 1:
            cfg_mode = SHR_FT_CFG_MODE_V1;
            if (!_bcm_th_ft_cfg_mode_v1_supported(unit)) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                        (BSL_META_U(unit,
                                    "FT(unit %d) Error: flowtracker_enable value = 1 is not supported\n"), unit));
                return BCM_E_CONFIG;
            }
            break;

        case 2:
            cfg_mode = SHR_FT_CFG_MODE_V2;
            break;

        default:
            /* Unsupported value */
            return BCM_E_PARAM;
    }

    if ((SOC_WARM_BOOT(unit)) &&
        (soc_property_get(unit, spn_FLOWTRACKER_HOSTMEM_ENABLE, 0))) {
         LOG_VERBOSE(BSL_LS_BCM_FLOWTRACKER,
                     (BSL_META_U(unit,"Warm boot is not supported for "
                                 "Flowtracker Application with host memory "
                                 "access \n")));
        return BCM_E_NONE;
    }

    if (ft_info != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
        ft_info = NULL;
    }

    _BCM_FT_ALLOC(ft_info, _bcm_int_ft_info_t,
                  sizeof(_bcm_int_ft_info_t), "FT_INFO");
    if (ft_info == NULL) {
        return BCM_E_MEMORY;
    }
    ft_global_info[unit] = ft_info;

    /* Set config mode */
    ft_info->cfg_mode = cfg_mode;

    /* Initialize core number to invalid */
    ft_info->uc_num = BCM_INT_FT_INVALID_FW_CORE;

    /* Fill the number of pipes info. */
    ft_info->num_pipes = NUM_PIPE(unit);

    /* Initialise MV2 Flow Tracker in hardware */
#if defined(BCM_MAVERICK2_SUPPORT)
    if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        rv = _bcm_mv2_flowtracker_config_init(unit, ft_info);
        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
            return rv;
        }
    }
#endif /* BCM_MAVERICK2_SUPPORT */

    /* Call flex counter API to get information regarding pools */
    sal_memset(&flex_ctr_info, 0, sizeof(flex_ctr_info));
    rv = _bcm_stat_flex_flowtracker_counter_info_get(unit, &flex_ctr_info);
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
        return rv;
    }

    qcm_enable = soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0);
    if (qcm_enable) {
        /* Get SOC properties */
        ft_info->max_flow_groups = soc_property_get(unit,
                                                    spn_FLOWTRACKER_MAX_FLOW_GROUPS,
                                                    BCM_INT_QCM_FT_MAX_FLOW_GROUPS);

        if (ft_info->max_flow_groups > BCM_INT_QCM_FT_MAX_FLOW_GROUPS) {
            BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
            return BCM_E_CONFIG;
        }
    } else {
        /* Get SOC properties */
        ft_info->max_flow_groups = soc_property_get(unit,
                                                    spn_FLOWTRACKER_MAX_FLOW_GROUPS,
                                                    BCM_INT_FT_MAX_FLOW_GROUPS);

        if (ft_info->max_flow_groups > BCM_INT_FT_MAX_FLOW_GROUPS) {
            BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
            return BCM_E_CONFIG;
        }
    }

    if (ft_info->max_flow_groups == 0) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
        return BCM_E_CONFIG;
    }

    ft_info->max_flows_total = 0;
    ft_info->enterprise_number =
            soc_property_get(unit, spn_FLOWTRACKER_ENTERPRISE_NUMBER, 0);
    ft_info->max_export_pkt_length =
            soc_property_get(unit, spn_FLOWTRACKER_MAX_EXPORT_PKT_LENGTH,
                                    BCM_INT_FT_DEF_MAX_EXPORT_LENGTH);

    if (qcm_enable) {
        /*
         * Per pipe limit is not configured, check for global and divide
         * equally
         */
        ft_info->max_flows_total = soc_property_get(unit,
                                           spn_QCM_MAX_FLOWS,
                                           BCM_INT_QCM_FT_MAX_FLOWS);

        if (ft_info->max_flows_total == 0) {
            BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
            return BCM_E_CONFIG;
        }

        for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
            ft_info->pipe_info[pipe].max_flows = ft_info->max_flows_total;
        }

        if ((ft_info->max_export_pkt_length < SHR_FT_QCM_MIN_EXPORT_LENGTH) ||
            (ft_info->max_export_pkt_length > SHR_FT_MAX_EXPORT_LENGTH)) {
            BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
            return BCM_E_CONFIG;
        }

    } else {
        for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
            sal_memset(max_flows_pipe_soc_prop_str, 0, 50);
            sal_snprintf(max_flows_pipe_soc_prop_str, 50, "%s%d",
                         "flowtracker_max_flows_pipe", pipe);
            ft_info->pipe_info[pipe].max_flows =
                                        soc_property_get(unit,
                                                         max_flows_pipe_soc_prop_str,
                                                         0);
            ft_info->max_flows_total += ft_info->pipe_info[pipe].max_flows;
        }
        if (ft_info->max_flows_total == 0) {
            /* Per pipe limit is not configured, check for global and divide
             * equally
             */
            pipe_flow_limit = soc_property_get(unit,
                                               spn_FLOWTRACKER_MAX_FLOWS,
                                               BCM_INT_FT_DEF_MAX_FLOWS) /
                                                              ft_info->num_pipes;

            for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
                ft_info->pipe_info[pipe].max_flows = pipe_flow_limit;
            }
            ft_info->max_flows_total = pipe_flow_limit * ft_info->num_pipes;
        }

        if (ft_info->max_flows_total == 0) {
            BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
            return BCM_E_CONFIG;
        }

        if ((ft_info->max_export_pkt_length < SHR_FT_MIN_EXPORT_LENGTH) ||
            (ft_info->max_export_pkt_length > SHR_FT_MAX_EXPORT_LENGTH)) {
            BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
            return BCM_E_CONFIG;
        }

        if (soc_property_get(unit, spn_FLOWTRACKER_FLOW_START_TIMESTAMP_IE_ENABLE, 0)) {
            ft_info->flags |= BCM_INT_FT_INFO_FLAGS_FLOW_START_TS;
        }

        if (soc_property_get(unit, spn_FLOWTRACKER_ELEPHANT_ENABLE, 0)) {
            if (!soc_feature(unit, soc_feature_hw_etrap)) {
                ft_info->elph_mode = 1;
            } else {
                return BCM_E_CONFIG;
            }
        }

        ft_info->export_interval =
            (soc_property_get(unit, spn_FLOWTRACKER_EXPORT_INTERVAL_USECS,
                              BCM_INT_FT_DEF_EXPORT_INTERVAL_MSECS * 1000)) / 1000;

        if (ft_info->elph_mode) {
            ft_info->scan_interval_usecs =
                       soc_property_get(unit,
                                  spn_FLOWTRACKER_ELEPHANT_SCAN_INTERVAL_USECS,
                                        BCM_INT_FT_ELPH_DEF_SCAN_INTERVAL_USECS);
            if (ft_info->scan_interval_usecs < BCM_INT_FT_ELPH_MIN_SCAN_INTERVAL_USECS) {
                return BCM_E_CONFIG;
            }
            if ((ft_info->scan_interval_usecs % BCM_INT_FT_ELPH_SCAN_INTERVAL_STEP_USECS) != 0) {
                return BCM_E_CONFIG;
            }
        } else {
            if (soc_property_get_str(unit, spn_FLOWTRACKER_SCAN_INTERVAL_USECS) == NULL) {
                /* If scan interval soc property is not used, fallback to default behavior
                 * of deriving scan interval from export interval.
                 */
                if (ft_info->export_interval > (BCM_INT_FT_MAX_SCAN_INTERVAL_USECS / 1000)) {
                    ft_info->scan_interval_usecs = BCM_INT_FT_MAX_SCAN_INTERVAL_USECS;
                } else {
                    ft_info->scan_interval_usecs = ft_info->export_interval * 1000;
                }
            } else {
                /* With common collector infrastructure, export interval can be dynamically
                 * changed. Hence scan interval will not be made a function of export interval.
                 * Instead, it will be obtained from user through config property.
                 */
                ft_info->scan_interval_usecs = soc_property_get(unit,
                                               spn_FLOWTRACKER_SCAN_INTERVAL_USECS,
                                               BCM_INT_FT_DEF_SCAN_INTERVAL_USECS);
                if (ft_info->scan_interval_usecs < BCM_INT_FT_DEF_SCAN_INTERVAL_USECS) {
                    return BCM_E_CONFIG;
                }
                if ((ft_info->scan_interval_usecs % BCM_INT_FT_SCAN_INTERVAL_STEP_USECS) != 0) {
                    return BCM_E_CONFIG;
                }
            }
        }
        BCM_IF_ERROR_RETURN(_bcm_th_ft_export_interval_validate(unit, ft_info->export_interval));
    }

    /* Fill the per pipe info */
    for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
        pipe_info = &(ft_info->pipe_info[pipe]);
        if (ft_info->pipe_info[pipe].max_flows == 0) {
            pipe_info->num_ctr_pools = 0;
        } else {
            pipe_info->num_ctr_pools = flex_ctr_info.pool_num;
        }

        for (pool_idx = 0; pool_idx < pipe_info->num_ctr_pools; pool_idx++) {
            sal_memset(&pool_info, 0, sizeof(pool_info));
            pool_info = flex_ctr_info.pool_info[pool_idx];
            pipe_info->ctr_pools[pool_idx] = pool_info.pool_id;
            pipe_info->ctr_pool_size[pool_idx] = pool_info.end_idx + 1;
        }
    }

    if (qcm_enable) {
        ft_info->flags |= BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR;
    } else {
        if (soc_property_get(unit, spn_FLOWTRACKER_DROP_MONITOR_ENABLE, 0)) {
            ft_info->flags |= BCM_INT_FT_INFO_FLAGS_DROP_MONITOR;
        }

        if (soc_property_get(unit, spn_FLOWTRACKER_HOSTMEM_ENABLE, 0)) {
            ft_info->flags |= BCM_INT_FT_INFO_FLAGS_HOSTMEM_ENABLE;
        }

        ft_info->fsp_reinject_max_length = soc_property_get(unit,
                                           spn_FLOWTRACKER_FSP_REINJECT_MAX_LENGTH,
                                           128);
        if ((ft_info->fsp_reinject_max_length <
                                SHR_FT_MIN_EXPORT_LENGTH) ||
            (ft_info->fsp_reinject_max_length >
                                SHR_FT_MAX_EXPORT_LENGTH)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit,
                                "FT(unit %d) Error: Invalid "
                                " reinjected FSP packet\n"), unit));
            return BCM_E_CONFIG;
        }
    }

    /* Initialize the embedded app */
    rv = bcm_xgs5_ft_eapp_init(unit);
    if (BCM_FAILURE(rv) ||
        (ft_info->uc_num == BCM_INT_FT_INVALID_FW_CORE)) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
        return rv;
    }

    SOC_CONTROL(unit)->uc_flowtracker_enable = 1;

    /* Create EM group and export to app in V1 */
    if (ft_info->cfg_mode == SHR_FT_CFG_MODE_V1) {
        rv = _bcm_th_ft_field_config_init(unit);
        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
            return rv;
        }
    }

    /* Create profile memory for the Exact Match QoS profile */
    rv = _bcm_th_ft_create_profile_mem(unit);
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
        return rv;
    }

    /* Initalize the data structures */
    rv = shr_idxres_list_create(&(ft_info->template_pool),
                                BCM_INT_FT_TEMPLATE_START_ID,
                                BCM_INT_FT_TEMPLATE_END_ID,
                                BCM_INT_FT_TEMPLATE_START_ID,
                                BCM_INT_FT_TEMPLATE_END_ID,
                                "flowtracker template pool");
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
        return rv;
    }

    rv = shr_idxres_list_create(&(ft_info->collector_pool),
                                BCM_INT_FT_COLLECTOR_START_ID,
                                BCM_INT_FT_COLLECTOR_END_ID,
                                BCM_INT_FT_COLLECTOR_START_ID,
                                BCM_INT_FT_COLLECTOR_END_ID,
                                "flowtracker collector pool");
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
        return rv;
    }

    if (ft_info->cfg_mode >= SHR_FT_CFG_MODE_V2) {
        rv = shr_idxres_list_create(&(ft_info->export_profile_pool),
                                    BCM_INT_FT_EXPORT_PROFILE_START_ID,
                                    BCM_INT_FT_EXPORT_PROFILE_END_ID,
                                    BCM_INT_FT_EXPORT_PROFILE_START_ID,
                                    BCM_INT_FT_EXPORT_PROFILE_END_ID,
                                    "flowtracker export_profile pool");
        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
            return rv;
        }
    }

    rv = shr_idxres_list_create(&(ft_info->flow_group_pool),
                                BCM_INT_FT_GROUP_START_ID,
                                ft_info->max_flow_groups + BCM_INT_FT_GROUP_START_ID - 1,
                                BCM_INT_FT_GROUP_START_ID,
                                ft_info->max_flow_groups + BCM_INT_FT_GROUP_START_ID - 1,
                                "flowtracker flow group pool");
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
        return rv;
    }

    rv = shr_idxres_list_create(&(ft_info->elph_profile_pool),
                                BCM_INT_FT_ELPH_PROFILE_START_ID,
                                BCM_INT_FT_ELPH_PROFILE_END_ID,
                                BCM_INT_FT_ELPH_PROFILE_START_ID,
                                BCM_INT_FT_ELPH_PROFILE_END_ID,
                                "flowtracker elephant profile  pool");
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_detach(unit));
        return rv;
    }

    soc_th_ft_em_ser_handler_register(unit, _soc_th_ft_em_ser_handler);

    ft_info->default_grp_id = 0;
    for (i = 0; i < SHR_FT_MAX_EM_GROUPS; ++i) {
        for (j = 0; j < ft_info->num_pipes; ++j) {
            ft_info->em_group_id_map[i][j] = BCM_INT_FT_INVALID_EM_GROUP_ID;
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_th_ft_wb_recover(unit);
    } else {
        rv = _bcm_th_ft_scache_alloc(unit, 1);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (rv == BCM_E_NOT_FOUND) {
        /* Continue with initialization if scache not found */
        rv = BCM_E_NONE;
    }
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_hw_clear
 * Purpose:
 *      Clear the H/w resources EM group & Flex counter
 * Parameters:
 *      unit          - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int _bcm_th_ft_hw_clear(int unit)
{
    int rv;
    int pipe, ctr_pool, pool_idx;
    soc_mem_t ctr_mem[] = {
                           ING_FLEX_CTR_COUNTER_TABLE_0m,
                           ING_FLEX_CTR_COUNTER_TABLE_1m,
                           ING_FLEX_CTR_COUNTER_TABLE_2m,
                           ING_FLEX_CTR_COUNTER_TABLE_3m,
                           ING_FLEX_CTR_COUNTER_TABLE_4m,
                           ING_FLEX_CTR_COUNTER_TABLE_5m,
                           ING_FLEX_CTR_COUNTER_TABLE_6m,
                           ING_FLEX_CTR_COUNTER_TABLE_7m,
                           ING_FLEX_CTR_COUNTER_TABLE_8m,
                           ING_FLEX_CTR_COUNTER_TABLE_9m,
                           ING_FLEX_CTR_COUNTER_TABLE_10m,
                           ING_FLEX_CTR_COUNTER_TABLE_11m,
                           ING_FLEX_CTR_COUNTER_TABLE_12m,
                           ING_FLEX_CTR_COUNTER_TABLE_13m,
                           ING_FLEX_CTR_COUNTER_TABLE_14m,
                           ING_FLEX_CTR_COUNTER_TABLE_15m,
                           ING_FLEX_CTR_COUNTER_TABLE_16m,
                           ING_FLEX_CTR_COUNTER_TABLE_17m,
                           ING_FLEX_CTR_COUNTER_TABLE_18m,
                           ING_FLEX_CTR_COUNTER_TABLE_19m,
                        };
    soc_mem_t mem;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int th3 = 0;

    th3 = SOC_IS_TOMAHAWK3(unit);
    if (th3) {
        soc_mem_t th3_mem;
        uint32 *bufp = NULL, key;
        bcm_field_entry_t eid = _FP_INTERNAL_RESERVED_EM_ID;
        int key_size, hw_index, index_min, index_max, entry_words;

        rv = _bcm_field_em_entry_info_get(unit, eid, &th3_mem, &key,
                                          &key_size, &hw_index);
        if (SOC_FAILURE(rv)) {
            return rv;
        }

        index_min      =  soc_mem_index_min(unit, th3_mem);
        index_max      =  soc_mem_index_max(unit, th3_mem);
        entry_words    =  soc_mem_entry_words(unit, th3_mem);;
        _BCM_FT_ALLOC(bufp, uint32, sizeof(uint32) * entry_words, "mem_clear_buf");
        if (bufp == NULL) {
            return SOC_E_MEMORY;
        }

        while (index_min <= index_max) {
            if (index_min == hw_index) {
                ++index_min;
                continue;
            }

            rv = soc_mem_write(unit, th3_mem, MEM_BLOCK_ALL, index_min, bufp);
            if (SOC_FAILURE(rv)) {
                sal_free(bufp);
                return rv;
            }
            ++index_min;
        }
        sal_free(bufp);
    }

    for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
        if (!th3) {
            mem = SOC_MEM_UNIQUE_ACC(unit, EXACT_MATCH_2m)[pipe];
            rv = soc_mem_clear(unit, mem, MEM_BLOCK_ALL, TRUE);
            if (SOC_FAILURE(rv)) {
                return rv;
            }
        }

        for (pool_idx = 0;
             pool_idx < ft_info->pipe_info[pipe].num_ctr_pools;
             pool_idx++) {

            ctr_pool = ft_info->pipe_info[pipe].ctr_pools[pool_idx];
            mem = SOC_MEM_UNIQUE_ACC(unit, ctr_mem[ctr_pool])[pipe];
            rv = soc_mem_clear(unit, mem, MEM_BLOCK_ALL, TRUE);
            if (SOC_FAILURE(rv)) {
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_detach
 * Purpose:
 *      Detach the Flowtracker subsystem.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_detach(int unit)
{
    int rv = BCM_E_NONE;
    int pipe;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (ft_info == NULL) {
        return BCM_E_NONE;
    }

    if ((SOC_WARM_BOOT(unit)) &&
        (ft_info->flags & BCM_INT_FT_INFO_FLAGS_HOSTMEM_ENABLE)) {
        return BCM_E_INTERNAL;
    }

    /* De-init Maverick2 Flow Tracker in hardware */
#if defined(BCM_MAVERICK2_SUPPORT)
    if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        rv = _bcm_mv2_flowtracker_config_detach(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
#endif /* BCM_MAVERICK2_SUPPORT */

    /* Delete the EM group */
    if (ft_info->cfg_mode == SHR_FT_CFG_MODE_V1) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_field_config_detach(unit));
    }

    for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
        if (ft_info->pipe_info[pipe].qos_profile_mem != NULL) {
            rv = soc_profile_mem_destroy(unit,
                                         ft_info->pipe_info[pipe].qos_profile_mem);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            ft_info->pipe_info[pipe].qos_profile_mem = NULL;
        }
    }

    /* Destroy the allocated data structures */
    if (ft_info->template_pool != NULL) {
        rv = shr_idxres_list_destroy(ft_info->template_pool);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        ft_info->template_pool = NULL;
    }

    if (ft_info->collector_pool != NULL) {
        rv = shr_idxres_list_destroy(ft_info->collector_pool);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        ft_info->collector_pool = NULL;
    }

    if (ft_info->export_profile_pool != NULL) {
        rv = shr_idxres_list_destroy(ft_info->export_profile_pool);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        ft_info->export_profile_pool = NULL;
    }

    if (ft_info->flow_group_pool != NULL) {
        rv = shr_idxres_list_destroy(ft_info->flow_group_pool);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        ft_info->flow_group_pool = NULL;
    }

    if (ft_info->elph_profile_pool != NULL) {
        rv = shr_idxres_list_destroy(ft_info->elph_profile_pool);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        ft_info->elph_profile_pool = NULL;
    }

    /* De-init the embedded app */
    rv = bcm_xgs5_ft_eapp_detach(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Clear the H/w resources */
        BCM_IF_ERROR_RETURN(_bcm_th_ft_hw_clear(unit));
    }

    SOC_CONTROL(unit)->uc_flowtracker_enable = 0;

    soc_th_ft_em_ser_handler_register(unit, NULL);

    sal_free(ft_info);
    ft_global_info[unit] = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_export_template_create
 * Purpose:
 *      Create an export template
 * Parameters:
 *      unit                    - (IN)    BCM device number
 *      options                 - (IN)    Template create options
 *      id                      - (INOUT) Template Id
 *      set_id                  - (IN)    set_id to be used for the template
 *      num_export_elements     - (IN)    Number of elements in the template
 *      list_of_export_elements - (IN)    Export element list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_th_ft_export_template_create(
                  int unit,
                  uint32 options,
                  bcm_flowtracker_export_template_t *id,
                  uint16 set_id,
                  int num_export_elements,
                  bcm_flowtracker_export_element_info_t *list_of_export_elements)
{
    _bcm_int_ft_export_template_info_t template_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    /* Check if template is supported templates and convert to internal
     * representation.
     */
    rv = _bcm_th_ft_template_validate_convert(unit,
                                              set_id,
                                              num_export_elements,
                                              list_of_export_elements,
                                              &template_int_info);
    BCM_IF_ERROR_RETURN(rv);

    /* If WITH_ID, reserve the ID */
    if (options & BCM_FLOWTRACKER_EXPORT_TEMPLATE_WITH_ID) {
        rv = shr_idxres_list_reserve(ft_info->template_pool,
                                     *id, *id);
        if (BCM_FAILURE(rv)) {
           return ((rv == BCM_E_RESOURCE) ? BCM_E_EXISTS : rv);
        }
    } else {
        /* Else allocate the ID */
        rv = shr_idxres_list_alloc(ft_info->template_pool,
                                   (shr_idxres_element_t *)id);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* Add it to the list of templates */
    rv = _bcm_xgs5_ft_template_list_template_add(unit, *id, &template_int_info);
    if (BCM_FAILURE(rv)) {
        /* Free up the template id allocated */
        BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->template_pool, *id));
        return rv;
    }

    /* Inform FT EAPP of the change/addition in/of export template information */
    rv = _bcm_xgs5_ft_eapp_send_template_create_msg(unit, *id);
    if (BCM_FAILURE(rv)) {
        /* Free up the template id allocated */
        BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->template_pool, *id));
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_template_list_template_delete(unit, *id));
        return rv;
    }

    ft_info->num_templates +=1;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_export_template_get
 * Purpose:
 *      Get a flowtracker export template with ID.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      id                      - (IN)  Template Id
 *      set_id                  - (OUT) Set Id of the template
 *      max_size                - (IN)  Size of the export element list array
 *      list_of_export_elements - (OUT) Export element list
 *      list_size               - (OUT) Number of elements in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_export_template_get(
                  int unit,
                  bcm_flowtracker_export_template_t id,
                  uint16 *set_id,
                  int max_size,
                  bcm_flowtracker_export_element_info_t *list_of_export_elements,
                  int *list_size)
{
    _bcm_int_ft_export_template_info_t template_int_info;
    int num_elements_to_copy = 0;
    int i, j;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(&template_int_info, 0, sizeof(template_int_info));

    /* If the max_size is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if (max_size != 0 && list_of_export_elements == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the template info based on template id */
    rv = _bcm_xgs5_ft_template_list_template_get(unit, id, &template_int_info);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* If max_size == 0 and list_of_export_elements == NULL,
     * return the number of export elements in list_size
     * variable.
     */
    if (max_size == 0 && list_of_export_elements == NULL) {
        *list_size = template_int_info.num_export_elements;
        return BCM_E_NONE;
    }

    *set_id = template_int_info.set_id;

    /* Else, if number of export elements less than max_size,
     * fill the list with the elements and fill list_size with
     * num_export_elements.
     */
    if (template_int_info.num_export_elements < max_size) {
        num_elements_to_copy = template_int_info.num_export_elements;
    } else {
        /* Else, if number of export elements greater than or equal to max_size,
         * fill the list with the elements upto max_size and fill list_size
         * with max_size.
         */
        num_elements_to_copy = max_size;
    }
    for (i = 0; i < num_elements_to_copy; i++) {
        /* Convert internal representation to API enum */
        if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
            for (j = 0; j < SHR_QCM_TEMPLATE_MAX_INFO_ELEMENTS; j++) {
                if (template_int_info.elements[i].element ==
                                _bcm_th_ft_qcm_template_element_mapping[j].int_element) {
                    list_of_export_elements[i].element =
                                         _bcm_th_ft_qcm_template_element_mapping[j].element;
                    list_of_export_elements[i].data_size =
                                      _bcm_th_ft_qcm_template_element_mapping[j].data_size;
                    break;
                }
            }
        } else {
            for (j = 0; j < SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS; j++) {
                if (template_int_info.elements[i].element ==
                                _bcm_th_ft_template_element_mapping[j].int_element) {
                    list_of_export_elements[i].element =
                                         _bcm_th_ft_template_element_mapping[j].element;
                    list_of_export_elements[i].data_size =
                                      _bcm_th_ft_template_element_mapping[j].data_size;
                    break;
                }
            }
        }
        list_of_export_elements[i].info_elem_id =
                       template_int_info.elements[i].id;
        if (template_int_info.elements[i].enterprise) {
            list_of_export_elements[i].flags |=
                         BCM_FLOWTRACKER_EXPORT_ELEMENT_FLAGS_ENTERPRISE;
        }
    }
    *list_size = num_elements_to_copy;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_export_template_destroy
 * Purpose:
 *      Destroy a flowtracker export template
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      id                      - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_export_template_destroy(int unit,
                                       bcm_flowtracker_export_template_t id)
{
    int rv = BCM_E_NONE;
    _bcm_int_ft_export_template_info_t template_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    /* Check if the template is in use by some flow group */
    rv = _bcm_xgs5_ft_flow_group_list_template_in_use_check(unit, id);
    if (rv == BCM_E_EXISTS) {
        /* One cannot delete a template without deleting the
         * association of the template to the flow group.
         */
        return BCM_E_CONFIG;
    }

    sal_memset(&template_int_info, 0, sizeof(template_int_info));
    rv = _bcm_xgs5_ft_template_list_template_get(unit, id, &template_int_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (template_int_info.interval_secs != 0) {
        /* We are still periodically exporting this template, reject
         * the delete
         */
        return BCM_E_CONFIG;
    }

    /* Delete the template from the list with the given template Id */
    rv = _bcm_xgs5_ft_template_list_template_delete(unit, id);

    /* If not found, return not found */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Else inform FT EAPP of deleted export template */
    rv = _bcm_xgs5_ft_eapp_send_template_delete_msg(unit, id);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Free up the ID from the pool */
    BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->template_pool, id));

    ft_info->num_templates -=1;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_collector_create
 * Purpose:
 *      Create a flowtracker collector with given collector info.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      options         - (IN)  Collector create options
 *      id              - (INOUT)  Collector Id
 *      collector_info  - (IN)  Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_collector_create(int unit,
                                uint32 options,
                                bcm_flowtracker_collector_t *id,
                                bcm_flowtracker_collector_info_t *collector_info)
{
    int rv = BCM_E_NONE;
    _bcm_int_ft_collector_info_t old_collector_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode != SHR_FT_CFG_MODE_V1) {
        /* API only supported in v1 flowtracker */
        return BCM_E_UNAVAIL;
    }

    /* Validate collector_info for any unsupported encap or
     * export length etc.
     */
    rv = _bcm_th_ft_collector_info_validate(unit, collector_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* 1. IF replace, WITH_ID is mandatory. If not provided, return
     * BCM_E_PARAM.
     *
     * 2. Also verify that a collector with such an ID
     * exists, else return BCM_E_NOT_FOUND
     */
    if ((options & BCM_FLOWTRACKER_COLLECTOR_REPLACE) &&
       !(options & BCM_FLOWTRACKER_COLLECTOR_WITH_ID)) {
        return BCM_E_PARAM;
    }

    if ((options & BCM_FLOWTRACKER_COLLECTOR_WITH_ID) &&
        (options & BCM_FLOWTRACKER_COLLECTOR_REPLACE)) {
        sal_memset(&old_collector_int_info, 0, sizeof(old_collector_int_info));
        rv = _bcm_xgs5_ft_collector_list_collector_get(unit, *id,
                                                       &old_collector_int_info);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* WITH_ID */
    if (options & BCM_FLOWTRACKER_COLLECTOR_WITH_ID) {
        if (!(options & BCM_FLOWTRACKER_COLLECTOR_REPLACE)) {
            /* If WITH_ID and not replace, reserve the ID */
            rv = shr_idxres_list_reserve(ft_info->collector_pool,
                                         *id, *id);
            if (BCM_FAILURE(rv)) {
                return ((rv == BCM_E_RESOURCE) ? BCM_E_EXISTS : rv);
            }
        }
    } else {
        /* Else allocate the ID */
        rv = shr_idxres_list_alloc(ft_info->collector_pool,
                                   (shr_idxres_element_t *)id);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* WITH_ID and replace. */
    if ((options & BCM_FLOWTRACKER_COLLECTOR_WITH_ID)  &&
        (options & BCM_FLOWTRACKER_COLLECTOR_REPLACE)) {
        /* Modify the collector info in the list with ID. */
        rv = _bcm_xgs5_ft_collector_list_collector_modify(unit, *id,
                                                          collector_info);
    }
    else {
        /* Add it to the list of collectors */
        rv = _bcm_xgs5_ft_collector_list_collector_add(unit, *id, collector_info);
    }
    if (BCM_FAILURE(rv)) {
        /* Free up the collector id allocated */
        if (!(options & BCM_FLOWTRACKER_COLLECTOR_REPLACE)) {
            BCM_IF_ERROR_RETURN(
                     shr_idxres_list_free(ft_info->collector_pool,*id));
        }
        return rv;
    }

    /* Inform FT EAPP of the change/addition in/of collector information */
    rv = _bcm_xgs5_ft_eapp_send_collector_create_msg(
                                    unit, *id,
                                    BCM_INT_COLLECTOR_INVALID_EXPORT_PROFILE_ID);
    if (BCM_FAILURE(rv)) {
        if (!(options & BCM_FLOWTRACKER_COLLECTOR_REPLACE)) {
            BCM_IF_ERROR_RETURN(
                        shr_idxres_list_free(ft_info->collector_pool, *id));
            BCM_IF_ERROR_RETURN(
                       _bcm_xgs5_ft_collector_list_collector_delete(unit, *id));
        }
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_collector_get
 * Purpose:
 *      Get flowtracker collector information
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      id              - (IN)  Collector Id
 *      collector_info  - (OUT) Collector info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_collector_get(int unit,
                             bcm_flowtracker_collector_t id,
                             bcm_flowtracker_collector_info_t *collector_info)
{
    _bcm_int_ft_collector_info_t collector_int_info;
    int rv = BCM_E_NONE;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode != SHR_FT_CFG_MODE_V1) {
        /* API only supported in v1 flowtracker */
        return BCM_E_UNAVAIL;
    }

    /* Ensure collector_info is not NULL */
    if (collector_info == NULL) {
        return BCM_E_PARAM;
    }

    sal_memset(&collector_int_info, 0, sizeof(collector_int_info));
    /* Get the collector info from the list
     * using ID.
     */
    rv = _bcm_xgs5_ft_collector_list_collector_get(unit, id,
                                                   &collector_int_info);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Else return SUCCESS */
    sal_memcpy(collector_info, &(collector_int_info.collector_info),
                sizeof(bcm_flowtracker_collector_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_collector_get_all
 * Purpose:
 *      Get the list of all flowtracker collectors configured.
 * Parameters:
 *      unit                 - (IN)  BCM device number
 *      max_size             - (IN)  Size of the collector list array
 *      collector_list       - (OUT) Collector id list
 *      list_szie            - (OUT) NUmber of elements in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_collector_get_all(
                           int unit,
                           int max_size,
                           bcm_flowtracker_collector_t *collector_list,
                           int *list_size)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int i;
    int num_collectors;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode != SHR_FT_CFG_MODE_V1) {
        /* API only supported in v1 flowtracker */
        return BCM_E_UNAVAIL;
    }

    /* If the max_size is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if (max_size != 0 && collector_list == NULL) {
        return BCM_E_PARAM;
    }

    num_collectors = 0;
    *list_size = 0;
    for (i = BCM_INT_FT_COLLECTOR_START_ID; i <= BCM_INT_FT_COLLECTOR_END_ID; i++) {
        if (ft_collector_info_list[unit][i].collector_id == i) {
            /* Collector exists */
            num_collectors++;
            if (*list_size < max_size) {
                collector_list[*list_size] = i;
                (*list_size)++;
            }
        }
    }

    /* If the max_size is 0 then return the number of collectors configured in
     * list_size
     */
    if (max_size == 0) {
        *list_size = num_collectors;
        return BCM_E_NONE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_collector_destroy
 * Purpose:
 *      Destroy a flowtracker collector
 * Parameters:
 *      unit           - (IN)  BCM device number
 *      collector_id   - (IN) Collector Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_collector_destroy(int unit,
                                 bcm_flowtracker_collector_t id)
{
    int rv = BCM_E_NONE;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode != SHR_FT_CFG_MODE_V1) {
        /* API only supported in v1 flowtracker */
        return BCM_E_UNAVAIL;
    }

    /* Check if the collector is in use by some flow group */
    rv = _bcm_xgs5_ft_flow_group_list_collector_in_use_check(unit, id);
    if (rv == BCM_E_EXISTS) {
        /* One cannot delete a collector without deleting the
         * association of the collector to the flow group.
         */
        return BCM_E_CONFIG;
    }

    /* Delete the collector from the collector list with
     * given ID.
     */
    rv = _bcm_xgs5_ft_collector_list_collector_delete(unit, id);
    /* If not found in the list, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Else inform FT EAPP of deleted collector */
    rv = _bcm_xgs5_ft_eapp_send_collector_delete_msg(unit, id);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Free up the ID from the pool */
    BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->collector_pool, id));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_template_transmit_config_set
 * Purpose:
 *      Set the template set transmit configuration
 * Parameters:
 *      unit         - (IN) BCM device number
 *      template_id  - (IN) Template Id
 *      collector_id - (IN) Collector Id
 *      config       - (IN) Template transmit config
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_th_ft_template_transmit_config_set(
                             int unit,
                             bcm_flowtracker_export_template_t template_id,
                             bcm_flowtracker_collector_t collector_id,
                             bcm_flowtracker_template_transmit_config_t *config)
{
    _bcm_int_ft_export_template_info_t template_int_info;
    _bcm_int_ft_collector_info_t collector_int_info;
    int coll_int_id = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }


    /* Check if template and collector are configured */
    sal_memset(&template_int_info, 0, sizeof(template_int_info));
    rv = _bcm_xgs5_ft_template_list_template_get(unit, template_id,
                                                 &template_int_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (ft_info->cfg_mode == SHR_FT_CFG_MODE_V1) {
        sal_memset(&collector_int_info, 0, sizeof(collector_int_info));
        rv = _bcm_xgs5_ft_collector_list_collector_get(unit, collector_id,
                &collector_int_info);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        rv = _bcm_xgs5_ft_v2_collector_list_collector_get(unit, collector_id,
                &coll_int_id);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* Copy over the information */
    template_int_info.interval_secs = config->retransmit_interval_secs;
    template_int_info.initial_burst = config->initial_burst;

    rv = _bcm_xgs5_ft_template_list_template_modify(unit, template_id,
                                                    &template_int_info);
    BCM_IF_ERROR_RETURN(rv);

    /* Send message to EAPP */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_eapp_send_template_xmit_start(unit,
                                                                   template_id,
                                                                   collector_id));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_flowtracker_template_transmit_config_get
 * Purpose:
 *      Get the template set transmit configuration
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      template_id  - (IN)  Template Id
 *      collector_id - (IN)  Collector Id
 *      config       - (OUT) Template transmit config
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_th_ft_template_transmit_config_get(
                             int unit,
                             bcm_flowtracker_export_template_t template_id,
                             bcm_flowtracker_collector_t collector_id,
                             bcm_flowtracker_template_transmit_config_t *config)
{
    _bcm_int_ft_export_template_info_t template_int_info;
    _bcm_int_ft_collector_info_t collector_int_info;
    int coll_int_id = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    /* Check if template and collector are configured */
    sal_memset(&template_int_info, 0, sizeof(template_int_info));
    rv = _bcm_xgs5_ft_template_list_template_get(unit, template_id,
                                                 &template_int_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (ft_info->cfg_mode == SHR_FT_CFG_MODE_V1) {
        sal_memset(&collector_int_info, 0, sizeof(collector_int_info));
        rv = _bcm_xgs5_ft_collector_list_collector_get(unit, collector_id,
                &collector_int_info);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        rv = _bcm_xgs5_ft_v2_collector_list_collector_get(unit, collector_id,
                &coll_int_id);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* Copy over the information */
    config->retransmit_interval_secs = template_int_info.interval_secs;
    config->initial_burst            = template_int_info.initial_burst;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_create
 * Purpose:
 *      Create a flowtracker flow group
 * Parameters:
 *      unit              - (IN)    BCM device number
 *      options           - (IN)    Group create options
 *      id                - (INOUT) Group Id
 *      flow_group_info   - (IN)    Group Info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_flow_group_create(int unit,
                                 uint32 options,
                                 bcm_flowtracker_group_t *id,
                                 bcm_flowtracker_group_info_t *flow_group_info)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int i, rv = BCM_E_NONE;
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    uint32 flow_group_alloc_count = 0;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    /* 1. IF replace, WITH_ID is mandatory. If not provided, return
     * BCM_E_PARAM.
     *
     * 2. Also verify that a flow group with such an ID
     * exists, else return BCM_E_NOT_FOUND
     */
    if ((options & BCM_FLOWTRACKER_GROUP_REPLACE) &&
       !(options & BCM_FLOWTRACKER_GROUP_WITH_ID)) {
        return BCM_E_PARAM;
    }

    /* Observation domain Id is a global paramter. Setting it for one group
     * changes it for all groups.
     * Observation domain Id should be same for all flow groups.
     */
    shr_idxres_list_state(ft_info->flow_group_pool, NULL, NULL,
                            NULL, NULL, NULL, &flow_group_alloc_count);

    if (!flow_group_alloc_count)
        ft_info->observation_domain_id = flow_group_info->observation_domain_id;
    else if (ft_info->observation_domain_id !=
             flow_group_info->observation_domain_id) {
        return BCM_E_PARAM;
    }

    if ((options & BCM_FLOWTRACKER_GROUP_REPLACE) &&
        (options & BCM_FLOWTRACKER_GROUP_WITH_ID)) {
        if ((flow_group_info->group_flags & BCM_FLOWTRACKER_GROUP_DEFAULT) &&
            (ft_info->default_grp_id != *id)) {
            return (BCM_E_CONFIG);
        }

        if ((!(flow_group_info->group_flags & BCM_FLOWTRACKER_GROUP_DEFAULT)) &&
            (ft_info->default_grp_id == *id)) {
            return (BCM_E_PARAM);
        }

        rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, *id);
        if (BCM_E_EXISTS != rv) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                      (BSL_META_U(unit,
                                  "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                       unit, *id));
            return (BCM_E_NOT_FOUND);
        }
        sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
        /* Get the flow group info for this group id */
        rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, *id,
                                                         &flow_group_int_info);

        /* Return BCM_E_NOT_FOUND if not found */
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        for (i = 0; i < ft_info->num_pipes; ++i) {
            if (flow_group_info->field_group[i] != flow_group_int_info.field_group[i]) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                          (BSL_META_U(unit,
                                      "FT(unit %d) Error: Group (ID:%d) field group can not modify.\n"),
                           unit, *id));
                return (BCM_E_CONFIG);
            }
        }
    }

    /* WITH_ID */
    if (options & BCM_FLOWTRACKER_GROUP_WITH_ID) {
        if ((flow_group_info->group_flags & BCM_FLOWTRACKER_GROUP_DEFAULT) &&
            ft_info->default_grp_id) {
            return (BCM_E_CONFIG);
        }

        if (!(options & BCM_FLOWTRACKER_GROUP_REPLACE)) {
            /* If WITH_ID and not replace, reserve the ID */
            rv = shr_idxres_list_reserve(ft_info->flow_group_pool,
                                         *id, *id);
            if (BCM_FAILURE(rv)) {
                return ((rv == BCM_E_RESOURCE) ? BCM_E_EXISTS : rv);
            }
        }
    } else {
        /* Else allocate the ID */
        rv = shr_idxres_list_alloc(ft_info->flow_group_pool,
                                   (shr_idxres_element_t *)id);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    if (!(options & BCM_FLOWTRACKER_GROUP_REPLACE)) {
        /* Add it to the list of flow_groups if it is not a replace */
        rv = _bcm_xgs5_ft_flow_group_list_flow_group_add(unit, *id);
        if (BCM_FAILURE(rv)) {
            /* Free up the flow_group id allocated */
            if (!(options & BCM_FLOWTRACKER_GROUP_REPLACE)) {
                BCM_IF_ERROR_RETURN(
                            shr_idxres_list_free(ft_info->flow_group_pool, *id));
            }
            return rv;
        }
    }

    /* Observation domain Id is a global paramter. Setting it for one group
     * changes it for all groups
     */
    ft_info->observation_domain_id = flow_group_info->observation_domain_id;

    /* Program Flowtracker group with default properties */
#if defined(BCM_MAVERICK2_SUPPORT)
    if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        if (!(options & BCM_FLOWTRACKER_GROUP_REPLACE)) {
            rv = _bcm_mv2_flowtracker_flow_group_default_config_set(unit, *id);
            if (BCM_FAILURE(rv)) {
                BCM_IF_ERROR_RETURN(
                        shr_idxres_list_free(ft_info->flow_group_pool, *id));
                return rv;
            }
        }
    }
#endif /* BCM_MAVERICK2_SUPPORT */

    /* Inform FT EAPP of the change/addition in/of flow_group information */
    if (!(options & BCM_FLOWTRACKER_GROUP_REPLACE)) {
        rv = _bcm_xgs5_ft_eapp_send_flow_group_create_msg(unit, *id);
    }

    if (BCM_FAILURE(rv)) {
        /* Free up the flow_group id allocated */
        if (!(options & BCM_FLOWTRACKER_GROUP_REPLACE)) {
            BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->flow_group_pool,
                                                     *id));
            BCM_IF_ERROR_RETURN(
                      _bcm_xgs5_ft_flow_group_list_flow_group_delete(unit, *id));
#if defined(BCM_MAVERICK2_SUPPORT)
            if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
                BCM_IF_ERROR_RETURN(
                      _bcm_mv2_flowtracker_flow_group_config_clear(unit, *id));
            }
#endif /* BCM_MAVERICK2_SUPPORT */
        }
        return rv;
    }

    if ((flow_group_info->group_flags & BCM_FLOWTRACKER_GROUP_DEFAULT) &&
        (!ft_info->default_grp_id)) {
        ft_info->default_grp_id = *id;
        /*
         * Inform FT EAPP of the change in the flow group
         * info using flow group default group id.
         */
        rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(unit, *id,
                                                          SHR_FT_GROUP_UPDATE_DEFAULT_GROUP,
                                                          1);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, *id,
                                                     &flow_group_int_info);
    BCM_IF_ERROR_RETURN(rv);

    for (i = 0; i < ft_info->num_pipes; ++i) {
        flow_group_int_info.field_group[i] = flow_group_info->field_group[i];
    }
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_modify(unit, *id,
                                                        &flow_group_int_info);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_get
 * Purpose:
 *      Get flowtracker flow group information
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      id                - (IN)  Group Id
 *      flow_group_info   - (OUT) Group Info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_flow_group_get(int unit,
                              bcm_flowtracker_group_t id,
                              bcm_flowtracker_group_info_t *flow_group_info)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int i, rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    /* Ensure flow_Group_info is not NULL */
    if (flow_group_info == NULL) {
        return BCM_E_PARAM;
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, id));
        return (BCM_E_NOT_FOUND);
    }

    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    /* Get the flow group info for this group id */
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, id,
                                                     &flow_group_int_info);

    /* Return BCM_E_NOT_FOUND if not found */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Else return SUCCESS */
    flow_group_info->observation_domain_id = ft_info->observation_domain_id;
    for (i = 0; i < ft_info->num_pipes; ++i) {
        flow_group_info->field_group[i] = flow_group_int_info.field_group[i];
    }
    if (ft_info->default_grp_id == id) {
        flow_group_info->group_flags |= BCM_FLOWTRACKER_GROUP_DEFAULT;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_get_all
 * Purpose:
 *      Get list of all flow groups created
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      max_size          - (IN)  Size of the flow group list array
 *      flow_group_list   - (OUT) List of flow groups created
 *      list_size         - (OUT) Number of flow grups in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_flow_group_get_all(int unit,
                                  int max_size,
                                  bcm_flowtracker_group_t *flow_group_list,
                                  int *list_size)
{
    bcm_flowtracker_group_t temp_flow_group_list[BCM_INT_FT_MAX_FLOW_GROUPS];
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE, num_flow_groups = 0, num_flow_groups_to_copy = 0;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    /* If the max_size is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if (max_size != 0 && flow_group_list == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the list of flow groups */
    rv = _bcm_xgs5_ft_flow_group_list_get_all_ids (unit, temp_flow_group_list,
                                                   &num_flow_groups);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* If the max_size is 0 and flow_group_info_list is
     * NULL, then return the number of flow_groups in
     * list_size
     */
    if (max_size == 0) {
        *list_size = num_flow_groups;
        return BCM_E_NONE;
    }

    /* If the number of flow_groups is less than max_size,
     * fill the list for number of flow_groups and fill
     * list_size with number_of_flow_groups.
     */
    if (num_flow_groups < max_size) {
        num_flow_groups_to_copy = num_flow_groups;
    } else {

        /* If the number of flow_groups is greater than or equal to
         * max_size, fill the list for max_size and fill list_size
         * with max_size.
         */
        num_flow_groups_to_copy = max_size;
    }

    sal_memcpy(flow_group_list, temp_flow_group_list,
               num_flow_groups_to_copy * sizeof(bcm_flowtracker_group_t));
    *list_size = num_flow_groups_to_copy;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_flow_limit_set
 * Purpose:
 *      Set flow limit on the flow group
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      id           - (IN)  Flow group Id
 *      flow_limit   - (IN) Max number of flows that can be learnt on the group
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes: In case of TH/TH2/TH3/TD3A0 flow limit is set in emb app.
 *        Whereas in case of maverick2 flow limit is set in hardware.
 */
int _bcm_th_ft_flow_group_flow_limit_set(int unit,
                                         bcm_flowtracker_group_t id,
                                         uint32 flow_limit)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    /* Validate flow limit against max flow limit
     */
    if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
        if (flow_limit > BCM_INT_QCM_FT_MAX_FLOWS) {
            return BCM_E_PARAM;
        }
    } else {
        if (flow_limit > ft_info->max_flows_total) {
            return BCM_E_PARAM;
        }
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, id));
        return (BCM_E_NOT_FOUND);
    }

    /* Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, id,
                                                     &flow_group_int_info);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* If found, modify the flow group info in the
     * list with this flow limit information.
     */
    flow_group_int_info.flow_limit = flow_limit;
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_modify(unit, id,
         &flow_group_int_info);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Inform FT EAPP of the change in the flow group
     * info using flow group update msg.
     */
    rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(
                                             unit, id,
                                             SHR_FT_GROUP_UPDATE_FLOW_THRESHOLD,
                                             flow_limit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

#if defined(BCM_MAVERICK2_SUPPORT)
    if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        rv = _bcm_mv2_flowtracker_flow_group_flow_limit_set(unit, id,
                                                            flow_limit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
#endif /* BCM_MAVERICK2_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_flow_limit_get
 * Purpose:
 *      Get flow limit of the flow group
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      id           - (IN)  Flow group Id
 *      flow_limit   - (OUT) Flow limit
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes: In case of TH/TH2/TH3/TD3A0 flow limit is retreived from emb app.
 *        Whereas in case of maverick2 flow limit is retreived from hardware.
 */
int _bcm_th_ft_flow_group_flow_limit_get(int unit,
                                         bcm_flowtracker_group_t id,
                                         uint32 *flow_limit)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, id));
        return (BCM_E_NOT_FOUND);
    }

    /* Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, id,
                                                     &flow_group_int_info);
    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* If found, return the flow limit info in
     * flow_limit param.
     */
    *flow_limit = flow_group_int_info.flow_limit;

#if defined(BCM_MAVERICK2_SUPPORT)
    if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        rv = _bcm_mv2_flowtracker_flow_group_flow_limit_get(unit, id,
                                                            flow_limit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
#endif /* BCM_MAVERICK2_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_th_ft_flow_group_stat_modeid_set
 * Purpose:
 *  Set stat modeid on the flow group
 * Parameters:
 *  unit         - (IN) BCM device number
 *  id           - (IN) Flow group Id
 *  stat_modeid  - (IN) Stat Modeid
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 * Notes: In case of TH/TH2/TH3/TD3A0 default modeid is reserved in init.
 *   Whereas in case of maverick2 stat mode is created first by user
 *   and set in hardware
 */
int
_bcm_th_ft_flow_group_stat_modeid_set(int unit,
                                      bcm_flowtracker_group_t id,
                                      uint32 stat_modeid)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_UNAVAIL;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

#if defined(BCM_MAVERICK2_SUPPORT)
    if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        rv = _bcm_mv2_flowtracker_flow_group_stat_modeid_set(unit, id,
                                                             stat_modeid);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
#endif /* BCM_MAVERICK2_SUPPORT */

    return rv;
}

/*
 * Function:
 *  _bcm_th_ft_flow_group_stat_modeid_get
 * Purpose:
 *  Get stat modeid associated to flow group
 * Parameters:
 *  unit         - (IN)  BCM device number
 *  id           - (IN)  Flow group Id
 *  stat_modeid  - (OUT) Stat Modeid
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 * Notes: In case of TH/TH2/TH3/TD3A0 default modeid is reserved in init.
 *   Whereas in case of maverick2 stat mode is created first by user
 *   and get from hardware.
 */
int
_bcm_th_ft_flow_group_stat_modeid_get(int unit,
                                      bcm_flowtracker_group_t id,
                                      uint32 *stat_modeid)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_UNAVAIL;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

#if defined(BCM_MAVERICK2_SUPPORT)
    if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        rv = _bcm_mv2_flowtracker_flow_group_stat_modeid_get(unit, id,
                                                             stat_modeid);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
#endif /* BCM_MAVERICK2_SUPPORT */

    return rv;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_age_timer_set
 * Purpose:
 *      Set aging timer interval in ms on the flow group. Aging interval has to
 *      be configured in steps of 1sec with a minimum of 1sec. Default value of
 *      1 minute
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) Flow group Id
 *      aging_interval_ms   - (IN) Aging interval in msec
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_flow_group_age_timer_set(int unit,
                                        bcm_flowtracker_group_t id,
                                        uint32 aging_interval_ms)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL || ft_info->elph_mode == TRUE) {
        return BCM_E_INIT;
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, id));
        return (BCM_E_NOT_FOUND);
    }

    /* Validate if aging timer interval is a multiple of 100ms
     * and greater than 100ms.
     */
    if (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR)) {
        rv = _bcm_th_ft_validate_aging_timer_interval(unit, aging_interval_ms);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, id,
            &flow_group_int_info);


    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* If found, modify the flow group info in the
     * list with this aging interval information.
     */
    flow_group_int_info.aging_interval_msecs = aging_interval_ms;
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_modify(unit, id,
         &flow_group_int_info);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Inform FT EAPP of the change in the flow group
     * info using flow group update msg.
     */
    rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(unit, id,
            SHR_FT_GROUP_UPDATE_AGING_INTERVAL, aging_interval_ms);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_age_timer_get
 * Purpose:
 *      Get aging timer interval in ms set on the flow group.
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      id                  - (IN)  Flow group Id
 *      aging_interval_ms   - (OUT) Aging interval in msec
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_flow_group_age_timer_get(int unit,
                                        bcm_flowtracker_group_t id,
                                        uint32 *aging_interval_ms)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL || ft_info->elph_mode == TRUE) {
        return BCM_E_INIT;
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, id));
        return (BCM_E_NOT_FOUND);
    }

    /* Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, id,
                                                     &flow_group_int_info);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* If found, return the aging info in
     * aging_interval_ms param.
     */
    *aging_interval_ms = flow_group_int_info.aging_interval_msecs;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_tracking_params_validate
 * Purpose:
 *      Validate Tracking params
 * Parameters:
 *      unit                  - (IN) BCM device number
 *      id                    - (IN) Flow group Id
 *      num_tracking_params   - (IN) Number of tracking params in the list.
 *      tracking_params_list  - (IN) Tracking Params list.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_th_ft_flow_group_tracking_params_validate(
                                    int unit,
                                   bcm_flowtracker_group_t id,
                                   int num_tracking_params,
                                   bcm_flowtracker_tracking_param_info_t *tracking_params_list)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int i;
    uint8 in_port_qual_present = 0;

    if (soc_feature(unit, soc_feature_th3_style_fp)) {
        for (i = 0; i < num_tracking_params; i++) {
            if (tracking_params_list[i].param == bcmFlowtrackerTrackingParamTypeIngPort) {
                    in_port_qual_present = 1;
                    break;
            }
        }
        if (!in_port_qual_present) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit,
                    "FT(unit %d) Error: Tracking param type in port is mandatory for TH3 style platforms.\n"),
                     unit));
            return BCM_E_PARAM;
        }
    }

    if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
        uint8 ipv4_cnt = 0, ipv6_cnt = 0;
        if (num_tracking_params > SHR_QCM_MAX_TP) {
             LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "FT QCM(unit %d) Error: Invalid number of tracking param: "
                       "%d %d\n"), unit, num_tracking_params, SHR_QCM_MAX_TP));
             return BCM_E_PARAM;
        }
        for (i = 0; i < num_tracking_params; i++) {
            switch (tracking_params_list[i].param) {
                case bcmFlowtrackerTrackingParamTypeSrcIPv4:
                    ++ipv4_cnt;
                    break;

                case bcmFlowtrackerTrackingParamTypeDstIPv4:
                    ++ipv4_cnt;
                    break;

                case bcmFlowtrackerTrackingParamTypeL4SrcPort:
                case bcmFlowtrackerTrackingParamTypeL4DstPort:
                case bcmFlowtrackerTrackingParamTypeIPProtocol:
                case bcmFlowtrackerTrackingParamTypeIngPort:
                    break;

                case bcmFlowtrackerTrackingParamTypeSrcIPv6:
                    ++ipv6_cnt;
                    break;

                case bcmFlowtrackerTrackingParamTypeDstIPv6:
                    ++ipv6_cnt;
                    break;

                default:
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                              "FT QCM(unit %d) Error: Invalid tracking param: "
                              "%d\n"), unit, tracking_params_list[i].param));
                    return BCM_E_PARAM;
            }
        }

        if (ipv4_cnt && ipv6_cnt) {
             LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                       "FT QCM(unit %d) Error: Invalid tracking param combination\n"), unit));
             return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_tracking_params_set
 * Purpose:
 *      Set Tracking params
 *      Create EM group table for new groups
 * Parameters:
 *      unit                  - (IN) BCM device number
 *      id                    - (IN) Flow group Id
 *      num_tracking_params   - (IN) Number of tracking params in the list.
 *      tracking_params_list  - (IN) Tracking Params list.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_flow_group_tracking_params_set(
                                   int unit,
                                   bcm_flowtracker_group_t id,
                                   int num_tracking_params,
                                   bcm_flowtracker_tracking_param_info_t *tracking_params_list)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int i, j, rv = BCM_E_NONE;
    int em_group_id_map_status = 0;
    int em_group_idx = 0;
    shr_ft_msg_ctrl_em_group_create_t em_group_msg;
    shr_ft_qcm_msg_ctrl_em_key_format_t qcm_em_key_format;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) {
        return BCM_E_CONFIG;
    }

    if (((num_tracking_params == 0)      ||
         (tracking_params_list == NULL)) ||
        (ft_info->default_grp_id == id)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_th_ft_flow_group_tracking_params_validate(unit,
                                                                       id,
                                                                       num_tracking_params,
                                                                       tracking_params_list));

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, id));
        return (BCM_E_NOT_FOUND);
    }

    /*
     * Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, id,
                                                     &flow_group_int_info);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (flow_group_int_info.num_tracking_params != 0) {
        return BCM_E_EXISTS;
    }

    for (i = 0; i < SHR_FT_MAX_EM_GROUPS; ++i) {
        for (j = 0; j < ft_info->num_pipes; ++j) {
            if (ft_info->em_group_id_map[i][j] == flow_group_int_info.field_group[j]) {
                em_group_id_map_status = 1;
                em_group_idx = i;
            } else {
                em_group_id_map_status = 0;
                break;
            }
        }
        if (em_group_id_map_status) {
            break;
        }
    }

    if (em_group_id_map_status) {
        BCM_IF_ERROR_RETURN(
            _bcm_xgs5_ft_eapp_send_flow_group_update_msg(unit, id,
                                                         SHR_FT_GROUP_UPDATE_EM_GROUP_IDX,
                                                         em_group_idx));
    } else {
        for (i = 0; i < SHR_FT_MAX_EM_GROUPS; ++i) {
            if (ft_info->em_group_id_map[i][0] == BCM_INT_FT_INVALID_EM_GROUP_ID) {
                for (j = 0; j < ft_info->num_pipes; ++j) {
                    ft_info->em_group_id_map[i][j] = flow_group_int_info.field_group[j];
                }
                if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
                    BCM_IF_ERROR_RETURN(_bcm_th_ft_qcm_fill_em_group_create_msg(unit,
                                                                            flow_group_int_info.field_group,
                                                                            i,
                                                                            num_tracking_params,
                                                                            tracking_params_list,
                                                                            &qcm_em_key_format));

                    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_qcm_eapp_send_em_group_create_msg(unit,
                                                                     &qcm_em_key_format));
                } else {
                    BCM_IF_ERROR_RETURN(_bcm_th_ft_fill_em_group_create_msg(unit,
                                                                            flow_group_int_info.field_group,
                                                                            i,
                                                                            num_tracking_params,
                                                                            tracking_params_list,
                                                                            &em_group_msg));

                    BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_eapp_send_em_group_create_msg(unit,
                                                                     &em_group_msg));
                }
                BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_eapp_send_flow_group_update_msg(unit, id,
                                                      SHR_FT_GROUP_UPDATE_EM_GROUP_IDX,
                                                      i));
                break;
            }
        }
        if (i >= SHR_FT_MAX_EM_GROUPS) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                      (BSL_META_U(unit,
                       "FT(unit %d) Error: Maximum number of EM Groups created.\n"),
                      unit));
            return BCM_E_RESOURCE;
        }
    }

    flow_group_int_info.num_tracking_params = num_tracking_params;
    _BCM_FT_ALLOC(flow_group_int_info.tracking_params,
                  bcm_flowtracker_tracking_param_info_t,
                  num_tracking_params * sizeof(bcm_flowtracker_tracking_param_info_t),
                  "Group Tracking Params list");
    if (flow_group_int_info.tracking_params == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memcpy(flow_group_int_info.tracking_params, tracking_params_list,
               sizeof(bcm_flowtracker_tracking_param_info_t) * num_tracking_params);

    rv = _bcm_xgs5_ft_flow_group_list_flow_group_modify(unit, id,
                                                        &flow_group_int_info);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_tracking_params_get
 * Purpose:
 *      Set Tracking params
 *      Create EM group table for new groups
 * Parameters:
 *      unit                  - (IN)  BCM device number
 *      id                    - (IN)  Flow group Id
 *      max_size              - (IN)  Maximum number of tracking params that
 *                                    can be accomodated in the list.
 *      list_size             - (OUT) Number of tracking params in the list.
 *      tracking_params_list  - (OUT) Tracking Params list.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_flow_group_tracking_params_get(
                                   int unit,
                                   bcm_flowtracker_group_t id,
                                   int max_size,
                                   int *list_size,
                                   bcm_flowtracker_tracking_param_info_t *tracking_params_list)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) {
        return BCM_E_CONFIG;
    }

    /* If the max_size is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if (max_size != 0 && tracking_params_list == NULL) {
        return BCM_E_PARAM;
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, id));
        return (BCM_E_NOT_FOUND);
    }

    /*
     * Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, id,
                                                     &flow_group_int_info);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (max_size == 0) {
        *list_size = flow_group_int_info.num_tracking_params;
        return BCM_E_NONE;
    }

    if (flow_group_int_info.num_tracking_params > max_size) {
        *list_size = max_size;
    } else {
        *list_size = flow_group_int_info.num_tracking_params;
    }

    sal_memcpy(tracking_params_list, flow_group_int_info.tracking_params,
               sizeof(bcm_flowtracker_tracking_param_info_t) * (*list_size));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_export_trigger_set
 * Purpose:
 *      Set Export trigger and Inform FT EAPP of the change
 *      flow group using export trigger set.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      id                  - (IN) Flow group Id
 *      export_trigger_info - (IN) Export trigger
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_flow_group_export_trigger_set(int unit,
                         bcm_flowtracker_group_t id,
                         bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) {
        return BCM_E_CONFIG;
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, id));
        return (BCM_E_NOT_FOUND);
    }

    /*
     * Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, id,
                                                     &flow_group_int_info);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    flow_group_int_info.export_trigger = 0;

    if (BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                                    bcmFlowtrackerExportTriggerTimer)) {
        flow_group_int_info.export_trigger |= SHR_FT_EXPORT_TRIGGER_PERIODIC;
    }

    if (BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                                    bcmFlowtrackerExportTriggerNewLearn)) {
        if (ft_info->uc_features & SHR_FT_UC_FEATURE_EXPORT_TRIGGER_NEW_LEARN) {
            flow_group_int_info.export_trigger |= SHR_FT_EXPORT_TRIGGER_NEW_LEARN;
        } else {
            return BCM_E_UNAVAIL;
        }
    }

    if (BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                                    bcmFlowtrackerExportTriggerAgeOut)) {
        if (ft_info->uc_features & SHR_FT_UC_FEATURE_EXPORT_TRIGGER_AGE_OUT) {
            flow_group_int_info.export_trigger |= SHR_FT_EXPORT_TRIGGER_AGE_OUT;
        } else {
            return BCM_E_UNAVAIL;
        }
    }

    if (BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                                    bcmFlowtrackerExportTriggerDrop)) {
        if ((ft_info->uc_features & SHR_FT_UC_FEATURE_EXPORT_TRIGGER_DROP) &&
            (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_MONITOR)) {
            flow_group_int_info.export_trigger |= SHR_FT_EXPORT_TRIGGER_DROP;
        } else {
            return BCM_E_UNAVAIL;
        }
    }

    if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
        if (ft_info->uc_features & SHR_FT_UC_FEATURE_CONGESTION_MONITOR) {
            if (BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                                            bcmFlowtrackerExportTriggerCongestion)) {
                flow_group_int_info.export_trigger = SHR_FT_EXPORT_TRIGGER_CONGESTION_MONITOR;
            } else if ((BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                                                    bcmFlowtrackerExportTriggerTimer)) ||
                       (BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                                                    bcmFlowtrackerExportTriggerNewLearn)) ||
                       (BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                                                    bcmFlowtrackerExportTriggerAgeOut)) ||
                       (BCM_FLOWTRACKER_TRIGGER_GET(*export_trigger_info,
                                                    bcmFlowtrackerExportTriggerDrop)))
            {
                return BCM_E_PARAM;
            }
        } else {
            return BCM_E_UNAVAIL;
        }
    }

    /*
     * Inform FT EAPP of the change in the flow group
     * info using flow group export trigger set.
     */
    rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(unit, id,
                                                      SHR_FT_GROUP_UPDATE_EXPORT_TRIGGERS,
                                                      flow_group_int_info.export_trigger);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = _bcm_xgs5_ft_flow_group_list_flow_group_modify(unit, id,
                                                        &flow_group_int_info);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_export_trigger_get
 * Purpose:
 *      Set Export trigger and Inform FT EAPP of the change
 *      flow group using export trigger set.
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      id                  - (IN)  Flow group Id
 *      export_trigger_info - (OUT) Export trigger
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_flow_group_export_trigger_get(int unit,
                         bcm_flowtracker_group_t id,
                         bcm_flowtracker_export_trigger_info_t *export_trigger_info)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) {
        return BCM_E_CONFIG;
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, id));
        return (BCM_E_NOT_FOUND);
    }

    /*
     * Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, id,
                                                     &flow_group_int_info);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (flow_group_int_info.export_trigger & SHR_FT_EXPORT_TRIGGER_PERIODIC) {
        BCM_FLOWTRACKER_TRIGGER_SET(*export_trigger_info,
                                    bcmFlowtrackerExportTriggerTimer);
    }

    if (flow_group_int_info.export_trigger & SHR_FT_EXPORT_TRIGGER_NEW_LEARN) {
        BCM_FLOWTRACKER_TRIGGER_SET(*export_trigger_info,
                                    bcmFlowtrackerExportTriggerNewLearn);
    }

    if (flow_group_int_info.export_trigger & SHR_FT_EXPORT_TRIGGER_AGE_OUT) {
        BCM_FLOWTRACKER_TRIGGER_SET(*export_trigger_info,
                                    bcmFlowtrackerExportTriggerAgeOut);
    }

    if (flow_group_int_info.export_trigger & SHR_FT_EXPORT_TRIGGER_DROP) {
        BCM_FLOWTRACKER_TRIGGER_SET(*export_trigger_info,
                                    bcmFlowtrackerExportTriggerDrop);
    }

    if (flow_group_int_info.export_trigger & SHR_FT_EXPORT_TRIGGER_CONGESTION_MONITOR) {
        BCM_FLOWTRACKER_TRIGGER_SET(*export_trigger_info,
                                    bcmFlowtrackerExportTriggerCongestion);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_flow_count_get
 * Purpose:
 *      Get the number of flows learnt in the flow group
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      id               - (IN)  Flow group Id
 *      flow_count       - (OUT) Number of flows learnt
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes: In Case of TH/TH2/TH3/TD3A0 flow count is retreived from emb app.
 *        where as in case of maverick2 flow count comes from hardware.
 */
int _bcm_th_ft_flow_group_flow_count_get(int unit,
                                         bcm_flowtracker_group_t id,
                                         uint32 *flow_count)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, id));
        return (BCM_E_NOT_FOUND);
    }

    /* Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, id,
                                                     &flow_group_int_info);


    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Get the flow count from FT EAPP using
     * flow_group get msg.
     */
    rv = _bcm_xgs5_ft_eapp_send_flow_group_get_msg(unit, id, flow_count);

    /* Return the failure if any failure */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

#if defined(BCM_MAVERICK2_SUPPORT)
    if (soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        rv = _bcm_mv2_flowtracker_flow_group_flow_count_get(unit, id,
                                                            flow_count);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
#endif /* BCM_MAVERICK2_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_destroy
 * Purpose:
 *      Destroy a flowtracker flow group
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      id               - (IN)  Flow group Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_flow_group_destroy(int unit,
                                  bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t *flow_group_list = ft_flow_group_info_list[unit];
    _bcm_int_ft_flow_group_info_t *group = NULL;
    int i, j, em_group_id_map_status = 0;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, id));
        return (BCM_E_NOT_FOUND);
    }


    /* Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, id,
                                                     &flow_group_int_info);
    BCM_IF_ERROR_RETURN(rv);

    if (ft_info->elph_mode == TRUE && flow_group_int_info.elph_profile_id != 0) {
        /* Elephant profile must be detached prior to group delete */
        return BCM_E_CONFIG;
    }
    if (flow_group_int_info.num_actions != 0) {
        /* Actions must be unset prior to group delete */
        return BCM_E_CONFIG;
    }

    /* Collector and export profile must be detached prior to destroy */
    if (ft_info->cfg_mode >= SHR_FT_CFG_MODE_V2) {
        if ((flow_group_int_info.collector_id != BCM_INT_COLLECTOR_INVALID_COLLECTOR_ID) ||
            (flow_group_int_info.export_profile_id != BCM_INT_COLLECTOR_INVALID_EXPORT_PROFILE_ID)) {
            return BCM_E_CONFIG;
        }
    }

    /* Delete the flow group from the flow group list with
     * given ID.
     */
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_delete(unit, id);
    /* If not found in the list, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Else inform FT EAPP of deleted flow group */
    rv = _bcm_xgs5_ft_eapp_send_flow_group_delete_msg(unit, id);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Free up the ID from the pool */
    BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->flow_group_pool, id));

    if (ft_info->default_grp_id == id) {
        ft_info->default_grp_id = 0;
    }

    for (i = BCM_INT_FT_GROUP_START_ID;
         i < (ft_info->max_flow_groups + BCM_INT_FT_GROUP_START_ID);
         i++) {
        group = &(flow_group_list[i]);
        if ((group->flow_group_id != i) || (group->flow_group_id == id)) {
            continue;
        }
        for (j = 0; j < ft_info->num_pipes; ++j) {
            if (flow_group_int_info.field_group[j] == group->field_group[j]) {
                em_group_id_map_status = 1;
            } else {
                em_group_id_map_status = 0;
            }
        }
        if (em_group_id_map_status) {
            break;
        }
    }

    if (!em_group_id_map_status) {
        for (i = 0; i < SHR_FT_MAX_EM_GROUPS; ++i) {
            if (ft_info->em_group_id_map[i][0] == flow_group_int_info.field_group[0]) {
                for (j = 0; j < ft_info->num_pipes; ++j) {
                    ft_info->em_group_id_map[i][j] = -1;
                }
                BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_eapp_send_em_group_delete_msg(unit,
                                                                               i));
                break;
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_group_clear
 * Purpose:
 *      Clear a flow group's flow entries.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      id            - (IN)  Flow group Id
 *      flags         - (IN)  Clear params
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_group_clear(int unit,
                           bcm_flowtracker_group_t id,
                           uint32 flags)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t           *ft_info = FT_INFO_GET(unit);
    uint32                                 int_flags;
    int                                    rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, id));
        return (BCM_E_NOT_FOUND);
    }

    /* Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, id,
                                                     &flow_group_int_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (flags == BCM_FLOWTRACKER_GROUP_CLEAR_ALL) {
        int_flags = SHR_FT_GROUP_UPDATE_FLUSH;
    } else if (flags == BCM_FLOWTRACKER_GROUP_CLEAR_FLOW_DATA_ONLY) {
        int_flags = SHR_FT_GROUP_UPDATE_COUNTER_CLEAR;
    } else {
        return BCM_E_PARAM;
    }

    /* Send the group clear msg to the app */
    rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(unit, id, int_flags, 0);
    return rv;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_collector_add
 * Purpose:
 *      Associate flow group to a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_th_ft_flow_group_collector_add(int unit,
                                    bcm_flowtracker_group_t flow_group_id,
                                    bcm_flowtracker_collector_t collector_id,
                                    bcm_flowtracker_export_template_t template_id)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode != SHR_FT_CFG_MODE_V1) {
        /* API only supported in v1 flowtracker */
        return BCM_E_UNAVAIL;
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, flow_group_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, flow_group_id));
        return (BCM_E_NOT_FOUND);
    }

    /* Verify if such a collector and template exists */
    rv = shr_idxres_list_elem_state(ft_info->collector_pool, collector_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Collector (ID:%d) does not exist.\n"),
                 unit, collector_id));
        return (BCM_E_PARAM);
    }

    rv = shr_idxres_list_elem_state(ft_info->template_pool, template_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Template (ID:%d) does not exist.\n"),
                 unit, template_id));
        return (BCM_E_PARAM);
    }

    /* Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id,
                                                     &flow_group_int_info);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (ft_info->uc_features & SHR_FT_UC_FEATURE_MULTIPLE_IPFIX_TEMPLATES) {
        /* Send the collector ID and Template ID set msg to the FT EAPP */
        rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(unit, flow_group_id,
                                             SHR_FT_GROUP_UPDATE_COLLECTOR_SET,
                                                          0);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(unit, flow_group_id,
                                             SHR_FT_GROUP_UPDATE_TEMPLATE_SET,
                                                          template_id - 1);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(unit, flow_group_id,
                                            SHR_FT_GROUP_UPDATE_COLLECTOR_TEMPLATE_SET,
                                            0);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    /* Update the group info with the collector ID and
     * template ID.
     */
    flow_group_int_info.collector_id = collector_id;
    flow_group_int_info.template_id = template_id;
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_modify(unit,
            flow_group_id,
            &flow_group_int_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_collector_delete
 * Purpose:
 *      Dis-associate flow group from a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_flow_group_collector_delete(
                                   int unit,
                                   bcm_flowtracker_group_t flow_group_id,
                                   bcm_flowtracker_collector_t collector_id,
                                   bcm_flowtracker_export_template_t template_id)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode != SHR_FT_CFG_MODE_V1) {
        /* API only supported in v1 flowtracker */
        return BCM_E_UNAVAIL;
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, flow_group_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, flow_group_id));
        return (BCM_E_NOT_FOUND);
    }

    /* Verify if such a collector and template exists */
    rv = shr_idxres_list_elem_state(ft_info->collector_pool, collector_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Collector (ID:%d) does not exist.\n"),
                 unit, collector_id));
        return (BCM_E_PARAM);
    }

    rv = shr_idxres_list_elem_state(ft_info->template_pool, template_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Template (ID:%d) does not exist.\n"),
                 unit, template_id));
        return (BCM_E_PARAM);
    }

    /* Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id,
                                                     &flow_group_int_info);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Update the group info with collector ID and template ID
     * to invalid value (0).
     */
    flow_group_int_info.collector_id = 0;
    flow_group_int_info.template_id = 0;
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_modify(unit,
                                                        flow_group_id,
                                                        &flow_group_int_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Send the collector ID and Template ID unset msg to the FT EAPP */
    if (ft_info->uc_features & SHR_FT_UC_FEATURE_MULTIPLE_IPFIX_TEMPLATES) {
        rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(
                                       unit, flow_group_id,
                                       SHR_FT_GROUP_UPDATE_COLLECTOR_UNSET,
                                       0);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(
                                       unit, flow_group_id,
                                       SHR_FT_GROUP_UPDATE_TEMPLATE_UNSET,
                                       template_id);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(
                                       unit, flow_group_id,
                                       SHR_FT_GROUP_UPDATE_COLLECTOR_TEMPLATE_UNSET,
                                       0);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_collector_get_all
 * Purpose:
 *      Get list of all collectors, templates  attached to a flow group
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      flow_group_id       - (IN)  Flow group Id
 *      max_list_size       - (IN)  Size of the list arrays
 *      list_of_collectors  - (OUT) Collector list
 *      list_of_templates   - (OUT) Template list
 *      list_size           - (OUT) Number of elements in the lists
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_flow_group_collector_get_all(
                            int unit,
                            bcm_flowtracker_group_t flow_group_id,
                            int max_list_size,
                            bcm_flowtracker_collector_t *list_of_collectors,
                            bcm_flowtracker_export_template_t *list_of_templates,
                            int *list_size)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode != SHR_FT_CFG_MODE_V1) {
        /* API only supported in v1 flowtracker */
        return BCM_E_UNAVAIL;
    }

    /* If the max_size is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if ((max_list_size != 0) &&
        (list_of_collectors == NULL ||
         list_of_templates == NULL)) {
        return BCM_E_PARAM;
    }

    /* Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id,
                                                     &flow_group_int_info);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* If the max_size is 0,
     * then return the number of collectors and templates in
     * list_size. Currently we support only one collector and one template.
     */
    if (max_list_size == 0) {
        if (flow_group_int_info.collector_id != 0 &&
            flow_group_int_info.template_id != 0) {
            *list_size = 1;
        } else {
            *list_size = 0;
        }
        return BCM_E_NONE;
    }


    list_of_collectors[0] = flow_group_int_info.collector_id;
    list_of_templates[0] = flow_group_int_info.template_id;

    *list_size = 1;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_collector_attach
 * Purpose:
 *      Associate flow group to a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_th_ft_group_collector_attach(int unit,
                                  bcm_flowtracker_group_t flow_group_id,
                                  bcm_collector_t collector_id,
                                  int export_profile_id,
                                  bcm_flowtracker_export_template_t template_id)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    bcm_collector_info_t collector_info;
    bcm_collector_export_profile_t export_profile_info;
    int coll_int_id, export_profile_int_id;
    int coll_ref_cnt = 0;
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) {
        /* API not supported in v1 flowtracker */
        return BCM_E_UNAVAIL;
    }

    /* Verify if flow group exists */
    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, flow_group_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, flow_group_id));
        return (BCM_E_NOT_FOUND);
    }

    /* Get flow group info with the ID from the flow group list */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id,
                                                     &flow_group_int_info);
    BCM_IF_ERROR_RETURN(rv);

    /* Check if the flow group is already attached to a collector */
    if (flow_group_int_info.collector_id !=
                             BCM_INT_COLLECTOR_INVALID_COLLECTOR_ID) {
        /* Only one colector can be attached to a group */
        return BCM_E_RESOURCE;
    }

    /* Get collector info */
    bcm_collector_info_t_init(&collector_info);
    BCM_IF_ERROR_RETURN(bcm_esw_collector_get(unit, collector_id, &collector_info));

    /* Get export profile info */
    bcm_collector_export_profile_t_init(&export_profile_info);
    BCM_IF_ERROR_RETURN(
                bcm_esw_collector_export_profile_get(unit,
                                                     export_profile_id,
                                                     &export_profile_info));

    /* Verify if template exists */
    if (export_profile_info.wire_format == bcmCollectorWireFormatIpfix) {
        rv = shr_idxres_list_elem_state(ft_info->template_pool, template_id);
        if (BCM_E_EXISTS != rv) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit,
                                "FT(unit %d) Error: Template (ID:%d) does not exist.\n"),
                     unit, template_id));
            return (BCM_E_NOT_FOUND);
        }
    }

    /* Validate the collector and export profile */
    rv = _bcm_th_ft_collector_export_profile_validate(unit, &collector_info,
                                                      &export_profile_info);
    BCM_IF_ERROR_RETURN(rv);

    if (export_profile_info.wire_format == bcmCollectorWireFormatIpfix) {
        if (_bcm_esw_collector_check_user_is_other(unit, collector_id, _BCM_COLLECTOR_EXPORT_APP_FT_IPFIX)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit,
                                "FT(unit %d) Error: Collector (ID:%d) is associated to another wire format.\n"),
                     unit, collector_id));
            return (BCM_E_CONFIG);
        } 
    } else {
        if (_bcm_esw_collector_check_user_is_other(unit, collector_id, _BCM_COLLECTOR_EXPORT_APP_FT_PROTOBUF)) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit,
                                "FT(unit %d) Error: Collector (ID:%d) is associated to another wire format.\n"),
                     unit, collector_id));
            return (BCM_E_CONFIG);
        } 
    }

    if ((ft_info->uc_features & SHR_FT_UC_FEATURE_EXPORT_INTERVAL_CHANGE) && 
        (export_profile_info.export_interval != BCM_COLLECTOR_EXPORT_INTERVAL_INVALID)) {
        coll_ref_cnt = _bcm_esw_collector_ref_count_get(unit, collector_id);
        if (coll_ref_cnt > 0) {
            /* If flowtracker is already using this collector,
             * verify if the export profile's interval is matching
             * already configured value. If not, throw error.
             * Export interval can be changed only when all collectors are detached.
             */
            /* Multiply by 1000 to convert usecs to msecs */
            if (export_profile_info.export_interval != (ft_info->export_interval * 1000)) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                        (BSL_META_U(unit,
                                    "FT(unit %d) Error: Only one export interval is supported.\n"),
                         unit));
                return BCM_E_CONFIG;
            }
        }
        /* 0 is a special case in API. Do not validate. */
        if (export_profile_info.export_interval != 0) {
            BCM_IF_ERROR_RETURN(_bcm_th_ft_export_interval_validate(unit, export_profile_info.export_interval));
        }
    }

    /* Check if this collector is already created */
    rv = _bcm_xgs5_ft_v2_collector_list_collector_get(unit, collector_id,
                                                      &coll_int_id);
    if (rv == BCM_E_NOT_FOUND) {

        /* Allocate an internal ID */
        rv = shr_idxres_list_alloc(ft_info->collector_pool,
                                   (shr_idxres_element_t *)(&coll_int_id));
        BCM_IF_ERROR_RETURN(rv);

        /* Add it to the list */
        rv = _bcm_xgs5_ft_v2_collector_list_collector_add(unit, collector_id,
                                                          coll_int_id);
        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->collector_pool,
                                                     coll_int_id));
            return rv;
        }

        /* Check if this export profile is already created */
        rv = _bcm_xgs5_ft_v2_collector_list_export_profile_get(unit, export_profile_id,
                                                               &export_profile_int_id);
        if (rv == BCM_E_NOT_FOUND) {
            /* Allocate an internal ID */
            rv = shr_idxres_list_alloc(ft_info->export_profile_pool,
                                       (shr_idxres_element_t *)(&export_profile_int_id));
            if (rv !=  BCM_E_NONE) {
                rv = _bcm_xgs5_ft_v2_collector_list_collector_delete(unit,
                                                                     collector_id);
                BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->collector_pool,
                                                         coll_int_id));
            }

            /* Add it to the list */
            rv = _bcm_xgs5_ft_v2_collector_list_export_profile_add(unit,
                                                                   export_profile_id,
                                                                   export_profile_int_id);
            if (rv !=  BCM_E_NONE) {
                rv = _bcm_xgs5_ft_v2_collector_list_collector_delete(unit,
                                                                     collector_id);
                BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->collector_pool,
                                                         coll_int_id));
            }
        } else if (rv != BCM_E_NONE) {
            rv = _bcm_xgs5_ft_v2_collector_list_collector_delete(unit,
                                                                 collector_id);
            BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->collector_pool,
                                                     coll_int_id));
        }

        /* Send collector create message to E-App */
        rv = _bcm_xgs5_ft_eapp_send_collector_create_msg(unit, collector_id,
                                                         export_profile_id);
        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(
                 _bcm_xgs5_ft_v2_collector_list_collector_delete(unit,
                                                                 collector_id));
            BCM_IF_ERROR_RETURN(
             _bcm_xgs5_ft_v2_collector_list_export_profile_delete(unit,
                                                                  export_profile_id));

            BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->collector_pool,
                                                     coll_int_id));
            BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->export_profile_pool,
                                                     export_profile_int_id));
            return rv;
        }
    } else if (rv == BCM_E_NONE) {
        /* Check if this export profile is already created */
        rv = _bcm_xgs5_ft_v2_collector_list_export_profile_get(unit, export_profile_id,
                                                               &export_profile_int_id);
        if (rv == BCM_E_NOT_FOUND) {
            /* Allocate an internal ID */
            rv = shr_idxres_list_alloc(ft_info->export_profile_pool,
                                       (shr_idxres_element_t *)(&export_profile_int_id));
            BCM_IF_ERROR_RETURN(rv);

            /* Add it to the list */
            rv = _bcm_xgs5_ft_v2_collector_list_export_profile_add(unit,
                                                                   export_profile_id,
                                                                   export_profile_int_id);
            BCM_IF_ERROR_RETURN(rv);

        } else if (rv != BCM_E_NONE) {
            return rv;
        }
    } else {
        return rv;
    }

    if (ft_info->uc_features & SHR_FT_UC_FEATURE_MULTIPLE_IPFIX_TEMPLATES) {
        rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(
                                     unit, flow_group_id,
                                     SHR_FT_GROUP_UPDATE_COLLECTOR_SET, 0);
        BCM_IF_ERROR_RETURN(rv);

        if (export_profile_info.wire_format == bcmCollectorWireFormatIpfix) {
            rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(
                                         unit, flow_group_id,
                                         SHR_FT_GROUP_UPDATE_TEMPLATE_SET, template_id - 1);
            BCM_IF_ERROR_RETURN(rv);
        }
    } else {
        rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(
                                    unit, flow_group_id,
                                    SHR_FT_GROUP_UPDATE_COLLECTOR_TEMPLATE_SET, 0);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Update the group info with the collector ID and
     * template ID.
     */
    flow_group_int_info.collector_id      = collector_id;
    flow_group_int_info.export_profile_id = export_profile_id;
    flow_group_int_info.template_id       = template_id;
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_modify(unit,
                                                        flow_group_id,
                                                        &flow_group_int_info);
    BCM_IF_ERROR_RETURN(rv);

    /* Increment the ref counts */
    BCM_IF_ERROR_RETURN(
           _bcm_esw_collector_export_profile_ref_count_update(unit,
                                                              export_profile_id,
                                                              1));
    BCM_IF_ERROR_RETURN(_bcm_esw_collector_ref_count_update(unit,
                                                            collector_id, 1));

    if (export_profile_info.wire_format == bcmCollectorWireFormatIpfix) {
        BCM_IF_ERROR_RETURN(_bcm_esw_collector_user_update(unit,
                    collector_id, _BCM_COLLECTOR_EXPORT_APP_FT_IPFIX));
        if (soc_property_get(unit, spn_FLOWTRACKER_ENTERPRISE_NUMBER, 0) == 0) {
            ft_info->enterprise_number = collector_info.ipfix.enterprise_number;
        }
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_collector_user_update(unit,
                    collector_id, _BCM_COLLECTOR_EXPORT_APP_FT_PROTOBUF));
    }

    /* Multiply by 1000 to convert usecs to msecs */
    if ((ft_info->uc_features & SHR_FT_UC_FEATURE_EXPORT_INTERVAL_CHANGE) 
     && (export_profile_info.export_interval != (ft_info->export_interval * 1000))
     && (export_profile_info.export_interval != BCM_COLLECTOR_EXPORT_INTERVAL_INVALID)) {
        /* Update the export interval and send a msg */
        ft_info->export_interval = (export_profile_info.export_interval/1000);
        /* Send the export interval update msg to the FT EAPP */
        rv = _bcm_xgs5_ft_eapp_send_export_interval_update_msg(
                unit, ft_info->export_interval);
        BCM_IF_ERROR_RETURN(rv);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_flow_group_collector_detach
 * Purpose:
 *      Disassociate flow group from a collector and an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_th_ft_group_collector_detach(int unit,
                                  bcm_flowtracker_group_t flow_group_id,
                                  bcm_collector_t collector_id,
                                  int export_profile_id,
                                  bcm_flowtracker_export_template_t template_id)
{
    bcm_collector_export_profile_t export_profile_info;
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int coll_int_id, export_profile_int_id;
    int in_use;
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) {
        /* API not supported in v1 flowtracker */
        return BCM_E_UNAVAIL;
    }

    /* Verify if flow group exists */
    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, flow_group_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, flow_group_id));
        return (BCM_E_NOT_FOUND);
    }

    /* Check if the collector is created */
    rv = _bcm_xgs5_ft_v2_collector_list_collector_get(unit, collector_id,
                                                      &coll_int_id);
    BCM_IF_ERROR_RETURN(rv);

    /* Check if the export profile is created */
    rv = _bcm_xgs5_ft_v2_collector_list_export_profile_get(unit, export_profile_id,
                                                           &export_profile_int_id);
    BCM_IF_ERROR_RETURN(rv);

    /* Get export profile info */
    bcm_collector_export_profile_t_init(&export_profile_info);
    BCM_IF_ERROR_RETURN(
                bcm_esw_collector_export_profile_get(unit,
                                                     export_profile_id,
                                                     &export_profile_info));

    /* Verify if template exists */
    if (export_profile_info.wire_format == bcmCollectorWireFormatIpfix) {
        rv = shr_idxres_list_elem_state(ft_info->template_pool, template_id);
        if (BCM_E_EXISTS != rv) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                    (BSL_META_U(unit,
                                "FT(unit %d) Error: Template (ID:%d) does not exist.\n"),
                     unit, template_id));
            return (BCM_E_NOT_FOUND);
        }
    }

    /* Get flow group info with the ID from the flow group list */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id,
                                                     &flow_group_int_info);
    BCM_IF_ERROR_RETURN(rv);

    /* Check if this collector & export_profile is attached to the flow group */
    if ((flow_group_int_info.collector_id != collector_id) ||
        (flow_group_int_info.export_profile_id != export_profile_id)) {
        return BCM_E_PARAM;
    }

    /* Send the collector ID and Template ID unset msg to the FT EAPP */
    if (ft_info->uc_features & SHR_FT_UC_FEATURE_MULTIPLE_IPFIX_TEMPLATES) {
        rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(
                                       unit, flow_group_id,
                                       SHR_FT_GROUP_UPDATE_COLLECTOR_UNSET,
                                       0);
        BCM_IF_ERROR_RETURN(rv);

        if (export_profile_info.wire_format == bcmCollectorWireFormatIpfix) {
            rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(
                    unit, flow_group_id,
                    SHR_FT_GROUP_UPDATE_TEMPLATE_UNSET,
                    template_id - 1);

            BCM_IF_ERROR_RETURN(rv);
        }
    } else {
        rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(
                                       unit, flow_group_id,
                                       SHR_FT_GROUP_UPDATE_COLLECTOR_TEMPLATE_UNSET,
                                       0);
    }

    /* Update the group info with the collector ID and
     * template ID.
     */
    flow_group_int_info.collector_id      = BCM_INT_COLLECTOR_INVALID_COLLECTOR_ID;
    flow_group_int_info.export_profile_id = BCM_INT_COLLECTOR_INVALID_EXPORT_PROFILE_ID;
    flow_group_int_info.template_id       = 0;
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_modify(unit,
                                                        flow_group_id,
                                                        &flow_group_int_info);
    BCM_IF_ERROR_RETURN(rv);

    /* Check if the collector is in use */
    _bcm_xgs5_ft_v2_collector_list_collector_in_use(unit, collector_id, &in_use);
    if (!in_use) {
        /* Collector is not in use, delete it and decrement the ref_count */
        rv = _bcm_xgs5_ft_eapp_send_collector_delete_msg(unit, collector_id);
        BCM_IF_ERROR_RETURN(rv);

        rv = _bcm_xgs5_ft_v2_collector_list_collector_delete(unit, collector_id);
        BCM_IF_ERROR_RETURN(rv);

        BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->collector_pool,
                                                 coll_int_id));
    }

    /* Check if the collector is in use */
    _bcm_xgs5_ft_v2_collector_list_export_profile_in_use(unit, export_profile_id,
                                                         &in_use);
    if (!in_use) {
        rv = _bcm_xgs5_ft_v2_collector_list_export_profile_delete(unit,
                                                                  export_profile_id);
        BCM_IF_ERROR_RETURN(rv);

        BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->export_profile_pool,
                                                 export_profile_int_id));
    }

    /* Decrement the ref counts */
    rv = _bcm_esw_collector_export_profile_ref_count_update(unit,
                                                            export_profile_id,
                                                            -1);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_esw_collector_ref_count_update(unit,
                                             collector_id,
                                             -1);

    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_group_collector_attach_get_all
 * Purpose:
 *      Get the list of collectors and templates associated with the flow group
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      flow_group_id       - (IN)  Flow group Id
 *      max_list_size       - (IN)  Size of the list arrays
 *      list_of_collectors  - (OUT) Collector list
 *      list_of_templates   - (OUT) Template list
 *      list_size           - (OUT) Number of elements in the lists
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_group_collector_attach_get_all(
                            int unit,
                            bcm_flowtracker_group_t flow_group_id,
                            int max_list_size,
                            bcm_collector_t *list_of_collectors,
                            int *list_of_export_profile_ids,
                            bcm_flowtracker_export_template_t *list_of_templates,
                            int *list_size)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->cfg_mode < SHR_FT_CFG_MODE_V2) {
        /* API not supported in v1 flowtracker */
        return BCM_E_UNAVAIL;
    }

    /* Verify if flow group exists */
    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, flow_group_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, flow_group_id));
        return (BCM_E_NOT_FOUND);
    }

    /* Get flow group info with the ID from the flow group list */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id,
                                                     &flow_group_int_info);
    BCM_IF_ERROR_RETURN(rv);

    /* max_list_size == 0, just populate list_size and return */
    if (max_list_size == 0) {
        if (flow_group_int_info.collector_id ==
                                BCM_INT_COLLECTOR_INVALID_COLLECTOR_ID) {
            *list_size = 0;
        } else {
            /* Only one collector and template can be attached to a group */
            *list_size = 1;
        }
        return BCM_E_NONE;
    }

    /* Get the collector info */

    *list_size                    = 1;
    list_of_templates[0]          = flow_group_int_info.template_id;
    list_of_collectors[0]         = flow_group_int_info.collector_id;
    list_of_export_profile_ids[0] = flow_group_int_info.export_profile_id;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_flowtracker_group_data_get
 * Purpose:
 *      Get flow data for a given flow key within the given flow group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      flow_group_id - (IN)  Flow group Id
 *      flow_key      - (IN)  Five tuple that constitutes a flow
 *      flow_data     - (OUT) Data associated with the flow key
 * Returns:
 *      BCM_E_XXX     - BCM error code.
 */
int _bcm_th_ft_group_data_get(int unit,
                              bcm_flowtracker_group_t flow_group_id,
                              bcm_flowtracker_flow_key_t *flow_key,
                              bcm_flowtracker_flow_data_t *flow_data)
{
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, flow_group_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, flow_group_id));
        return (BCM_E_NOT_FOUND);
    }

    /* Get flow group info with the ID from the
     * flow group list
     */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id,
                                                     &flow_group_int_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Get the flow data from the EApp */
    if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
        rv = _bcm_xgs5_ft_qcm_eapp_send_flow_group_flow_data_get_msg(unit, flow_group_id,
                                                                     flow_key);
    } else {
        rv = _bcm_xgs5_ft_eapp_send_flow_group_flow_data_get_msg(unit, flow_group_id,
                                                                 flow_key, flow_data);
    }
    return rv;
}

/*
 * Function:
 *      bcm_th_ft_group_actions_conflict_check
 * Purpose:
 *      Set list of actions on a flow group.
 * Parameters:
 *      num_actions   - (IN) Number of actions in the list.
 *      action_list   - (IN) Action list.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_group_actions_conflict_check(
                               int num_actions,
                               bcm_flowtracker_group_action_info_t *action_list)
{
    int                            i, j;
    bcm_flowtracker_group_action_t action_a, action_b;

    for (i = 0; i < num_actions; i++) {
        action_a = action_list[i].action;

        for (j = (i + 1); j < num_actions; j++) {
            action_b = action_list[j].action;

            switch(action_a) {
                case bcmFlowtrackerGroupActionRpDropPrecedence:
                    FT_ACTION_CONFLICT(action_b, bcmFlowtrackerGroupActionRpDropPrecedence);
                    break;

                case bcmFlowtrackerGroupActionYpDropPrecedence:
                    FT_ACTION_CONFLICT(action_b, bcmFlowtrackerGroupActionYpDropPrecedence);
                    break;

                case bcmFlowtrackerGroupActionGpDropPrecedence:
                    FT_ACTION_CONFLICT(action_b, bcmFlowtrackerGroupActionGpDropPrecedence);
                    break;

                case bcmFlowtrackerGroupActionUcastCosQNew:
                    FT_ACTION_CONFLICT(action_b, bcmFlowtrackerGroupActionUcastCosQNew);
                    FT_ACTION_CONFLICT(action_b, bcmFlowtrackerGroupActionPrioIntNew);
                    break;

                case bcmFlowtrackerGroupActionMcastCosQNew:
                    FT_ACTION_CONFLICT(action_b, bcmFlowtrackerGroupActionMcastCosQNew);
                    FT_ACTION_CONFLICT(action_b, bcmFlowtrackerGroupActionPrioIntNew);
                    break;

                case bcmFlowtrackerGroupActionPrioIntNew:
                    FT_ACTION_CONFLICT(action_b, bcmFlowtrackerGroupActionPrioIntNew);
                    FT_ACTION_CONFLICT(action_b, bcmFlowtrackerGroupActionUcastCosQNew);
                    FT_ACTION_CONFLICT(action_b, bcmFlowtrackerGroupActionMcastCosQNew);
                    break;

                default:
                    break;
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_group_actions_convert
 * Purpose:
 *      Convert actions to internal representation
 * Parameters:
 *      num_actions    - (IN)  Number of actions in the list.
 *      action_list    - (IN)  Action list.
 *      inum_actions   - (OUT) Number of actions in the iaction_list.
 *      iaction_list   - (OUT) Action list in internal representation
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_group_actions_convert(int unit, int num_actions,
                                 bcm_flowtracker_group_action_info_t *action_list,
                                 int *inum_actions,
                                 _bcm_int_ft_action_info_t *iaction_list,
                                 int *inum_qos_actions)
{
    int  i, j, k;
    int  rv;
    int  ucast_mcast = 0;
    int  cosq;
    int  non_qos_action = 0;
    bcm_module_t module;
    bcm_port_t port = 0;
    bcm_trunk_t trunk;
    int id;

    for (i = 0, j = 0; i < num_actions; i++) {
        switch(action_list[i].action) {
            case bcmFlowtrackerGroupActionRpDropPrecedence:
                iaction_list[j].action = _bcmIntFtActionRpDropPrecedence;
                iaction_list[j].param0 = action_list[i].params.param0;
                j++;
                break;

            case bcmFlowtrackerGroupActionYpDropPrecedence:
                iaction_list[j].action = _bcmIntFtActionYpDropPrecedence;
                iaction_list[j].param0 = action_list[i].params.param0;
                j++;
                break;

            case bcmFlowtrackerGroupActionGpDropPrecedence:
                iaction_list[j].action = _bcmIntFtActionGpDropPrecedence;
                iaction_list[j].param0 = action_list[i].params.param0;
                j++;
                break;

            case bcmFlowtrackerGroupActionUcastCosQNew:
                rv = _bcm_esw_flowtracker_ucast_cosq_resolve(unit,
                                                             action_list[i].params.param0,
                                                             &cosq);
                BCM_IF_ERROR_RETURN(rv);

                /* Actions need to be programmed diffferently if both unicast
                 * and multicast actions are present. Overwrite the multicast
                 * action if it is present.
                 */
                for (k = 0; k < j; k++) {
                    if (iaction_list[k].action == _bcmIntFtActionGpMcastCosQNew) {
                        iaction_list[k].action = _bcmIntFtActionGpCosQNew;
                        iaction_list[k].param0 = (iaction_list[k].param0 << 4) | cosq;
                        ucast_mcast = 1;
                    }

                    if (iaction_list[k].action == _bcmIntFtActionYpMcastCosQNew) {
                        iaction_list[k].action = _bcmIntFtActionYpCosQNew;
                        iaction_list[k].param0 = (iaction_list[k].param0 << 4) | cosq;
                        ucast_mcast = 1;
                    }

                    if (iaction_list[k].action == _bcmIntFtActionRpMcastCosQNew) {
                        iaction_list[k].action = _bcmIntFtActionRpCosQNew;
                        iaction_list[k].param0 = (iaction_list[k].param0 << 4) | cosq;
                        ucast_mcast = 1;
                    }
                }

                if (ucast_mcast == 0) { /* Only unicast */
                    iaction_list[j].action = _bcmIntFtActionGpUcastCosQNew;
                    iaction_list[j].param0 = cosq;
                    j++;

                    iaction_list[j].action = _bcmIntFtActionYpUcastCosQNew;
                    iaction_list[j].param0 = cosq;
                    j++;

                    iaction_list[j].action = _bcmIntFtActionRpUcastCosQNew;
                    iaction_list[j].param0 = cosq;
                    j++;
                }
                break;

            case bcmFlowtrackerGroupActionMcastCosQNew:
                rv = _bcm_esw_flowtracker_mcast_cosq_resolve(unit,
                                                             action_list[i].params.param0,
                                                             &cosq);
                BCM_IF_ERROR_RETURN(rv);

                /* Actions need to be programmed diffferently if both unicast
                 * and multicast actions are present. Overwrite the multicast
                 * action if it is present.
                 */
                for (k = 0; k < j; k++) {
                    if (iaction_list[k].action == _bcmIntFtActionGpUcastCosQNew) {
                        iaction_list[k].action = _bcmIntFtActionGpCosQNew;
                        iaction_list[k].param0 = (cosq << 4) | iaction_list[k].param0;
                        ucast_mcast = 1;
                    }

                    if (iaction_list[k].action == _bcmIntFtActionYpUcastCosQNew) {
                        iaction_list[k].action = _bcmIntFtActionYpCosQNew;
                        iaction_list[k].param0 = (cosq << 4) | iaction_list[k].param0;
                        ucast_mcast = 1;
                    }

                    if (iaction_list[k].action == _bcmIntFtActionRpUcastCosQNew) {
                        iaction_list[k].action = _bcmIntFtActionRpCosQNew;
                        iaction_list[k].param0 = (cosq << 4) | iaction_list[k].param0;
                        ucast_mcast = 1;
                    }
                }

                if (ucast_mcast == 0) { /* Only multicast */
                    iaction_list[j].action = _bcmIntFtActionGpMcastCosQNew;
                    iaction_list[j].param0 = cosq;
                    j++;

                    iaction_list[j].action = _bcmIntFtActionYpMcastCosQNew;
                    iaction_list[j].param0 = cosq;
                    j++;

                    iaction_list[j].action = _bcmIntFtActionRpMcastCosQNew;
                    iaction_list[j].param0 = cosq;
                    j++;
                }
                break;

            case bcmFlowtrackerGroupActionPrioIntNew:
                iaction_list[j].action = _bcmIntFtActionGpPrioIntNew;
                iaction_list[j].param0 = action_list[i].params.param0;
                j++;

                iaction_list[j].action = _bcmIntFtActionYpPrioIntNew;
                iaction_list[j].param0 = action_list[i].params.param0;
                j++;

                iaction_list[j].action = _bcmIntFtActionRpPrioIntNew;
                iaction_list[j].param0 = action_list[i].params.param0;
                j++;
                break;

            case bcmFlowtrackerGroupActionFspReinject:
                if (BCM_GPORT_IS_COSQ(action_list[i].params.param0)) {
                    rv = _bcm_esw_flowtracker_mcast_cosq_resolve(unit,
                                                                 action_list[i].params.param0,
                                                                 &cosq);
                    BCM_IF_ERROR_RETURN(rv);
                    iaction_list[j].param0 = cosq;
                } else {
                    if (action_list[i].params.param0) {
                        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit,
                                                     action_list[i].params.param0,
                                                     &module, &port, &trunk, &id));
                    }
                    if (((action_list[i].params.param0 == 0) || (port == 0)) &&
                        (action_list[i].params.param1 < NUM_CPU_COSQ(unit))) {
                        iaction_list[j].param0 = action_list[i].params.param1;
                    } else {
                        return BCM_E_PARAM;
                    }
                }

                iaction_list[j].action = _bcmIntFtFspReinjectActionIntNew;
                j++;
                non_qos_action++;
                break;

            case bcmFlowtrackerGroupActionDropMonitor:
                iaction_list[j].action = _bcmIntFtDropMonitorActionIntNew;
                j++;
                non_qos_action++;
                break;

            case bcmFlowtrackerGroupActionCongestion:
                iaction_list[j].action = _bcmIntFtCongestionMonitorActionIntNew;
                j++;
                non_qos_action++;
                break;

            default:
                return BCM_E_PARAM;
        }
    }

    *inum_actions = j;
    *inum_qos_actions = j - non_qos_action;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_val_set
 * Purpose:
 *     Write the width size of data from data to buf at offset.
 * Parameters:
 *     data           - (IN)  Data to be written into buf
 *     offset         - (IN)  Offset at which data has to be copied to p_fn_data
 *     width          - (IN)  Size of the data to be written into p_fn_data
 *     buf            - (OUT) Out buffer
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_th_ft_val_set(uint32 *data, uint32 offset, uint32 width, uint32 *buf)
{
    uint32 u32_mask;
    int idx, wp, bp, len;

    wp = offset / 32;
    bp = offset & (32 - 1);
    idx = 0;

    for (len = width; len > 0; len -= 32) {
        if (bp) {
            if (len < 32) {
                u32_mask = (1 << len) - 1;
                if ((data[idx] & ~u32_mask) != 0) {
                    return (BCM_E_PARAM);
                }
            } else {
                u32_mask = 0xffffffff;
            }

            buf[wp] &= ~(u32_mask << bp);
            buf[wp++] |= data[idx] << bp;
            buf[wp] &= ~(u32_mask >> (32 - bp));
            buf[wp] |= data[idx] >> (32 - bp) & ((1 << bp) - 1);

        } else {
            if (len < 32) {
                u32_mask = (1 << len) - 1;
                if ((data[idx] & ~u32_mask) != 0) {
                    return (BCM_E_PARAM);
                }
                buf[wp] &= ~u32_mask;
                buf[wp++] |= data[idx];
            } else {
                buf[wp++] = data[idx];
            }
        }

        idx++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_actions_buf_set
 * Purpose:
 *      Set action list to the specifid offsets in the buffer
 * Parameters:
 *      num_actions   - (IN)  Number of actions in a_offset[]
 *      a_offset      - (IN)  Action offset list
 *      bufp          - (OUT) Out buffer
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_actions_buf_set(int num_actions,
                           _bcm_int_ft_action_offset_t *a_offset, uint32 *buf)
{
    int    i, j;
    uint32 offset;
    uint32 width;
    uint32 value;

    for (i = 0; i < num_actions; i++) {
        for (j = 0; j < BCM_INT_FT_ACTION_MAX_PARTS; j++) {
            if (a_offset[i].width[j] > 0) {
                offset = a_offset[i].offset[j];
                width  = a_offset[i].width[j];
                value  = a_offset[i].value[j];
                /*
                 * COVERITY
                 *
                 * This flow takes care of the  Out-of-bounds access issue
                 * for data and mask.
                 */
                /* coverity[callee_ptr_arith : FALSE] */
                BCM_IF_ERROR_RETURN(_bcm_th_ft_val_set(&value, offset,
                                                       width, buf));
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_group_qos_actions_install
 * Purpose:
 *      Install list of action to the H/w
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      num_actions   - (IN)  Number of actions in the action_list.
 *      action_list   - (IN)  Action list to install
 *      qp_idx        - (OUT) QoS profile Index
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_group_qos_actions_install(int unit, int num_actions,
                                 _bcm_int_ft_action_info_t *action_list,
                                 uint32 *qp_idx)
{
    int                                      i;
    _bcm_int_ft_info_t             *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_action_offset_t              a_offset[_bcmIntFtActionCount];
    soc_mem_t                                mem;
    exact_match_qos_actions_profile_entry_t  qos_prof_entry;
    void                                    *entries[1];
    uint32                                  *bufp = NULL;
    int                                      pipe;
    int                                      rv;

    sal_memset(a_offset, 0, _bcmIntFtActionCount *
                                       sizeof(_bcm_int_ft_action_offset_t));

    /* Convert actions to H/w format */
    for (i = 0; i < num_actions; i++) {
        switch(action_list[i].action) {
            case _bcmIntFtActionGpDropPrecedence:
                a_offset[i].offset[0] = 4;
                a_offset[i].width[0]  = 2;
                a_offset[i].value[0]  = action_list[i].param0;
                break;

            case _bcmIntFtActionYpDropPrecedence:
                a_offset[i].offset[0] = 2;
                a_offset[i].width[0]  = 2;
                a_offset[i].value[0]  = action_list[i].param0;
                break;

            case _bcmIntFtActionRpDropPrecedence:
                a_offset[i].offset[0] = 0;
                a_offset[i].width[0]  = 2;
                a_offset[i].value[0]  = action_list[i].param0;
                break;

            case _bcmIntFtActionGpUcastCosQNew:
                a_offset[i].offset[0] = 38;
                a_offset[i].width[0]  = 4;
                a_offset[i].value[0]  = 8;

                a_offset[i].offset[1] = 22;
                a_offset[i].width[1]  = 4;
                a_offset[i].value[1]  = action_list[i].param0;
                break;

            case _bcmIntFtActionYpUcastCosQNew:
                a_offset[i].offset[0] = 34;
                a_offset[i].width[0]  = 4;
                a_offset[i].value[0]  = 8;

                a_offset[i].offset[1] = 14;
                a_offset[i].width[1]  = 4;
                a_offset[i].value[1]  = action_list[i].param0;
                break;

            case _bcmIntFtActionRpUcastCosQNew:
                a_offset[i].offset[0] = 30;
                a_offset[i].width[0]  = 4;
                a_offset[i].value[0]  = 8;

                a_offset[i].offset[1] = 6;
                a_offset[i].width[1]  = 4;
                a_offset[i].value[1]  = action_list[i].param0;
                break;

            case _bcmIntFtActionGpMcastCosQNew:
                a_offset[i].offset[0] = 38;
                a_offset[i].width[0]  = 4;
                a_offset[i].value[0]  = 9;

                a_offset[i].offset[1] = 26;
                a_offset[i].width[1]  = 4;
                a_offset[i].value[1]  = action_list[i].param0;
                break;

            case _bcmIntFtActionYpMcastCosQNew:
                a_offset[i].offset[0] = 34;
                a_offset[i].width[0]  = 4;
                a_offset[i].value[0]  = 9;

                a_offset[i].offset[1] = 18;
                a_offset[i].width[1]  = 4;
                a_offset[i].value[1]  = action_list[i].param0;
                break;

            case _bcmIntFtActionRpMcastCosQNew:
                a_offset[i].offset[0] = 30;
                a_offset[i].width[0]  = 4;
                a_offset[i].value[0]  = 9;

                a_offset[i].offset[1] = 10;
                a_offset[i].width[1]  = 4;
                a_offset[i].value[1]  = action_list[i].param0;
                break;

            case _bcmIntFtActionGpCosQNew:
                a_offset[i].offset[0] = 38;
                a_offset[i].width[0]  = 4;
                a_offset[i].value[0]  = 1;

                a_offset[i].offset[1] = 22;
                a_offset[i].width[1]  = 8;
                a_offset[i].value[1]  = action_list[i].param0;
                break;

            case _bcmIntFtActionYpCosQNew:
                a_offset[i].offset[0] = 34;
                a_offset[i].width[0]  = 4;
                a_offset[i].value[0]  = 1;

                a_offset[i].offset[1] = 14;
                a_offset[i].width[1]  = 8;
                a_offset[i].value[1]  = action_list[i].param0;
                break;

            case _bcmIntFtActionRpCosQNew:
                a_offset[i].offset[0] = 30;
                a_offset[i].width[0]  = 4;
                a_offset[i].value[0]  = 1;

                a_offset[i].offset[1] = 6;
                a_offset[i].width[1]  = 8;
                a_offset[i].value[1]  = action_list[i].param0;
                break;

            case _bcmIntFtActionGpPrioIntNew:
                a_offset[i].offset[0] = 38;
                a_offset[i].width[0]  = 4;
                a_offset[i].value[0]  = 5;

                a_offset[i].offset[1] = 22;
                a_offset[i].width[1]  = 3;
                a_offset[i].value[1]  = action_list[i].param0;
                break;

            case _bcmIntFtActionYpPrioIntNew:
                a_offset[i].offset[0] = 34;
                a_offset[i].width[0]  = 4;
                a_offset[i].value[0]  = 5;

                a_offset[i].offset[1] = 14;
                a_offset[i].width[1]  = 3;
                a_offset[i].value[1]  = action_list[i].param0;
                break;

            case _bcmIntFtActionRpPrioIntNew:
                a_offset[i].offset[0] = 30;
                a_offset[i].width[0]  = 4;
                a_offset[i].value[0]  = 5;

                a_offset[i].offset[1] = 6;
                a_offset[i].width[1]  = 3;
                a_offset[i].value[1]  = action_list[i].param0;
                break;

            default:
                return BCM_E_PARAM;
        }
    }

    /* Write to H/w, num_actions == 0 implies a delete, don't write as there is
     * already a default empty entry
     */
    if (num_actions != 0) {
        /* Initialize Qos Profile Actions Entry */
        mem = EXACT_MATCH_QOS_ACTIONS_PROFILEm;
        bufp = (uint32 *)&qos_prof_entry;
        sal_memcpy(bufp, soc_mem_entry_null(unit, mem),
                   soc_mem_entry_words(unit, mem) * sizeof(uint32));

        /* Set the actions in the buffer */
        rv = _bcm_th_ft_actions_buf_set(num_actions, a_offset, bufp);
        BCM_IF_ERROR_RETURN(rv);


        entries[0] = bufp;
        for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
            /* Add entry to Qos Actions Profile Tables In Hardware in all pipes */
            rv = soc_profile_mem_add(unit,
                                     ft_info->pipe_info[pipe].qos_profile_mem,
                                     entries, 1, qp_idx);
            BCM_IF_ERROR_RETURN(rv);
        }
    } else {
        /* Set the qos profile Id to the default entry */
        *qp_idx = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_group_qos_actions_remove
 * Purpose:
 *      Remove the qos actions pointed by the profile ID
 * Parameters:
 *      unit          - (IN) BCM device number
 *      qp_idx        - (IN) QoS profile Index
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_group_qos_actions_remove(int unit, uint32 qp_idx)
{
    _bcm_int_ft_info_t   *ft_info = FT_INFO_GET(unit);
    int                   pipe;
    int                   rv;

    for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
        rv = soc_profile_mem_delete(unit,
                                    ft_info->pipe_info[pipe].qos_profile_mem,
                                    qp_idx);
        BCM_IF_ERROR_RETURN(rv);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_group_num_qos_actions_get
 * Purpose:
 *      Number of existing qos actions pointed by the profile ID
 * Parameters:
 *      unit          - (IN) BCM device number
 *      action_list   - (IN) Action list.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_group_num_qos_actions_get(int unit,
                                     _bcm_int_ft_flow_group_info_t  *flow_group_int_info)
{
    int i, num_qos_actions = 0;
    for (i = 0; i < flow_group_int_info->num_actions; i++) {
        switch(flow_group_int_info->action_list[i].action) {
            case bcmFlowtrackerGroupActionRpDropPrecedence:
            case bcmFlowtrackerGroupActionYpDropPrecedence:
            case bcmFlowtrackerGroupActionGpDropPrecedence:
            case bcmFlowtrackerGroupActionUcastCosQNew:
            case bcmFlowtrackerGroupActionMcastCosQNew:
            case bcmFlowtrackerGroupActionPrioIntNew:
                num_qos_actions++;
                break;

            case bcmFlowtrackerGroupActionFspReinject:
            case bcmFlowtrackerGroupActionDropMonitor:
            case bcmFlowtrackerGroupActionCongestion:
                break;
            default:
                return BCM_E_PARAM;
        }
    }
    return (num_qos_actions);
}

/*
 * Function:
 *      _bcm_th_ft_group_actions_set
 * Purpose:
 *      Set list of actions on a flow group.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 *      flags         - (IN) Flags
 *      num_actions   - (IN) Number of actions in the list.
 *      action_list   - (IN) Action list.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_th_ft_group_actions_set(int unit,
                             bcm_flowtracker_group_t flow_group_id,
                             uint32 flags,
                             int num_actions,
                             bcm_flowtracker_group_action_info_t *action_list)
{
    _bcm_int_ft_info_t            *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t  flow_group_int_info;
    int                            inum_actions, inum_qos_actions;
    _bcm_int_ft_action_info_t      iaction_list[_bcmIntFtActionCount];
    uint32                         qp_idx = 0;
    int                            i;
    int                            prev_num_qos_actions = 0;
    shr_ft_msg_ctrl_group_actions_set_t action_msg;

    int rv = BCM_E_NONE;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    for (i = 0; i < num_actions; i++) {
        switch(action_list[i].action) {
            case bcmFlowtrackerGroupActionRpDropPrecedence:
            case bcmFlowtrackerGroupActionYpDropPrecedence:
            case bcmFlowtrackerGroupActionGpDropPrecedence:
            case bcmFlowtrackerGroupActionUcastCosQNew:
            case bcmFlowtrackerGroupActionMcastCosQNew:
            case bcmFlowtrackerGroupActionPrioIntNew:
                if (ft_info->elph_mode != TRUE) {
                    return BCM_E_INIT;
                }
                if (!(flags & BCM_FLOWTRACKER_GROUP_ACTIONS_SET_FLAGS_ELEPHANT_FLOWS)) {
                    /* Actions can be applied only on elephant flows */
                    return BCM_E_PARAM;
                }
                break;
            case bcmFlowtrackerGroupActionFspReinject:
                if (!(ft_info->uc_features & SHR_FT_UC_FEATURE_FSP_REINJECT)) {
                    /* Action applied only on FSP feature is enable */
                    return BCM_E_UNAVAIL;
                }
                break;
            case bcmFlowtrackerGroupActionDropMonitor:
                if (!(ft_info->uc_features & SHR_FT_UC_FEATURE_DROP_MONITOR) ||
                    !(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_MONITOR)) {
                    /* Action applied only on Drop Monitor feature is enable */
                    return BCM_E_UNAVAIL;
                }
                break;
            case bcmFlowtrackerGroupActionCongestion:
                if (!(ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) ||
                    (num_actions > 1)) {
                    /* Action applied only on Drop Congestion Monitor feature is enable */
                    return BCM_E_UNAVAIL;
                }
                break;
            default:
                return BCM_E_PARAM;
        }
    }

    if ((num_actions != 0) && (action_list == NULL)) {
        return BCM_E_PARAM;
    }

    /* Check if group exists */
    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, flow_group_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, flow_group_id));
        return (BCM_E_NOT_FOUND);
    }

    /* Get flow group info with the ID from the flow group list */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id,
                                                     &flow_group_int_info);
    BCM_IF_ERROR_RETURN(rv);

    if ((flow_group_int_info.num_actions == 0) && (num_actions == 0)) {
        /* Nothing installed, and a delete is requested num_actions == 0,
         * implies delete
         */
        return BCM_E_PARAM;
    }

    /* Check if the actions conflict */
    rv = _bcm_th_ft_group_actions_conflict_check(num_actions, action_list);
    BCM_IF_ERROR_RETURN(rv);

    /* Convert actions to internal representation */
    rv = _bcm_th_ft_group_actions_convert(unit, num_actions, action_list,
                                          &inum_actions, iaction_list,
                                          &inum_qos_actions);
    BCM_IF_ERROR_RETURN(rv);

    /* Install the actions to H/w */
    if (inum_qos_actions) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_group_qos_actions_install(unit, inum_actions,
                                                                 iaction_list, &qp_idx));
    }

    /* Delete the old action if present */
    prev_num_qos_actions = _bcm_th_ft_group_num_qos_actions_get(unit,
                                                           &flow_group_int_info);
    if (flow_group_int_info.num_actions != 0) {
        if (prev_num_qos_actions) {
            rv = _bcm_th_ft_group_qos_actions_remove(unit,
                                                 flow_group_int_info.qos_profile_id);
            BCM_IF_ERROR_RETURN(rv);
            flow_group_int_info.qos_profile_id = 0;
        }
        if (flow_group_int_info.num_actions > prev_num_qos_actions) {
            sal_memset(&action_msg, 0,
                       sizeof(shr_ft_msg_ctrl_group_actions_set_t));
            action_msg.group_idx = flow_group_id;
            rv = _bcm_xgs5_ft_eapp_send_group_actions_set_msg(unit,
                                                              &action_msg);
            BCM_IF_ERROR_RETURN(rv);
        }
        sal_free(flow_group_int_info.action_list);
        flow_group_int_info.action_list = NULL;
        flow_group_int_info.num_actions = 0;
    }

    /* Send the new qos profile id to Eapp */
    if (inum_qos_actions) {
        rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(unit, flow_group_id,
                                                          SHR_FT_GROUP_UPDATE_ELPH_QOS_PROFILE_ID,
                                                          qp_idx);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Send FSP, Drop and Congestion action to Eapp */
    if (inum_actions != inum_qos_actions) {
        sal_memset(&action_msg, 0,
                   sizeof(shr_ft_msg_ctrl_group_actions_set_t));
        for (i = 0; i < num_actions; i++) {
            if (action_list[i].action == bcmFlowtrackerGroupActionDropMonitor) {
                action_msg.actions  |= SHR_FT_GROUP_ACTION_DROP_MONITOR;
            }
            if (action_list[i].action == bcmFlowtrackerGroupActionFspReinject) {
                action_msg.actions  |= SHR_FT_GROUP_ACTION_FSP_REINJECT;
                action_msg.fsp_cosq  = iaction_list[i].param0;
            }
            if (action_list[i].action == bcmFlowtrackerGroupActionCongestion) {
                action_msg.actions  |= SHR_FT_GROUP_ACTION_CONGESTION_MONITOR;
            }
        }
        action_msg.group_idx = flow_group_id;
        rv = _bcm_xgs5_ft_eapp_send_group_actions_set_msg(unit,
                                                          &action_msg);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (num_actions != 0) {
        flow_group_int_info.num_actions = num_actions;
        _BCM_FT_ALLOC(flow_group_int_info.action_list,
                      _bcm_int_ft_group_action_info_t,
                      num_actions * sizeof(_bcm_int_ft_group_action_info_t),
                      "Group action list");
        if (flow_group_int_info.action_list == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < num_actions; i++) {
            flow_group_int_info.action_list[i].action = action_list[i].action;
            if ((action_list[i].action == bcmFlowtrackerGroupActionDropMonitor) ||
                (action_list[i].action == bcmFlowtrackerGroupActionFspReinject)) {
                flow_group_int_info.action_list[i].num_params = 2;
                flow_group_int_info.action_list[i].params[0] =
                                                       action_list[i].params.param0;
                flow_group_int_info.action_list[i].params[1] =
                                                       action_list[i].params.param1;
            } else {
                flow_group_int_info.action_list[i].num_params = 1;
                flow_group_int_info.action_list[i].params[0] =
                                                       action_list[i].params.param0;
            }
        }
        flow_group_int_info.qos_profile_id = qp_idx;
    }
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_modify(unit, flow_group_id,
                                                        &flow_group_int_info);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_group_actions_get
 * Purpose:
 *      Get list of actions applied on a flow group.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 *      flags         - (IN) Flags
 *      max_actions   - (IN) Maximum number of actions that can be
 *                           accomodated in the list.
 *      action_list   - (OUT) Action list.
 *      num_actions   - (OUT) Actual number of actions in the list
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_th_ft_group_actions_get(int unit,
                             bcm_flowtracker_group_t flow_group_id,
                             uint32 flags,
                             int max_actions,
                             bcm_flowtracker_group_action_info_t *action_list,
                             int *num_actions)
{
    _bcm_int_ft_info_t           *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    int                           rv = BCM_E_NONE;
    int                           i;

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    /* If the max_actions is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if (max_actions != 0 && action_list == NULL) {
        return BCM_E_PARAM;
    }

    /* Check if group exists */
    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, flow_group_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, flow_group_id));
        return (BCM_E_NOT_FOUND);
    }

    /* Get flow group info with the ID from the flow group list */
    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id,
                                                     &flow_group_int_info);
    BCM_IF_ERROR_RETURN(rv);

    if (max_actions == 0) {
        *num_actions = flow_group_int_info.num_actions;
        return BCM_E_NONE;
    }

    if (flow_group_int_info.num_actions > max_actions) {
        *num_actions = max_actions;
    } else {
        *num_actions = flow_group_int_info.num_actions;
    }

    for (i = 0; i < *num_actions; i++) {
        action_list[i].action = flow_group_int_info.action_list[i].action;
        if ((action_list[i].action == bcmFlowtrackerGroupActionDropMonitor) ||
            (action_list[i].action == bcmFlowtrackerGroupActionFspReinject)) {
            action_list[i].params.param0 =
                flow_group_int_info.action_list[i].params[0];
            action_list[i].params.param1 =
                flow_group_int_info.action_list[i].params[1];
        } else {
            action_list[i].params.param0 =
                flow_group_int_info.action_list[i].params[0];
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_th_ft_elephant_filter_validate
 * Purpose:
 *      Validate elephant filter
 * Parameters:
 *      demotion   - (IN)     1 - Demotion filter,
 *                            0 - Promotion filter
 *      filter     - (IN/OUT) Elephant filter
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th_ft_elephant_filter_validate(int unit,
                                    uint8 demotion,
                                    bcm_flowtracker_elephant_filter_t *filter)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (filter->monitor_interval_usecs == 0) {
        /* No filter configured, add default filter if it is demotion,
         * promotion filters are optional
         */
        if (demotion) {
            filter->monitor_interval_usecs = ft_info->scan_interval_usecs;
            COMPILER_64_SET(filter->size_threshold_bytes, 0, 1);
        }
        return BCM_E_NONE;
    }

    if ((filter->monitor_interval_usecs % (ft_info->scan_interval_usecs)) != 0) {
        return BCM_E_PARAM;
    }

    if ((filter->rate_low_threshold_kbits_sec %
                       BCM_INT_FT_ELPH_RATE_THRESHOLD_STEP) != 0) {
        return BCM_E_PARAM;
    }

    if ((filter->rate_high_threshold_kbits_sec %
                       BCM_INT_FT_ELPH_RATE_THRESHOLD_STEP) != 0) {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_elephant_profile_create
 * Purpose:
 *      Create flowtracker elephant profile
 * Parameters:
 *      unit         - (IN)     BCM device number
 *      options      - (IN)     Elephant profile creation options.
 *      profile      - (IN)     Elephant profile information
 *      profile_id   - (IN/OUT) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_elephant_profile_create(
                                int unit,
                                uint32 options,
                                bcm_flowtracker_elephant_profile_info_t *profile,
                                bcm_flowtracker_elephant_profile_t *profile_id)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;
    int i;

    if (ft_info == NULL || ft_info->elph_mode != TRUE) {
        return BCM_E_INIT;
    }

    /* Validate the profile */
    if (profile->num_promotion_filters > BCM_FLOWTRACKER_ELEPHANT_MAX_PROMOTION_FILTERS) {
        return BCM_E_PARAM;
    }
    for (i = 0; i < profile->num_promotion_filters; i++) {
        BCM_IF_ERROR_RETURN(
            _bcm_th_ft_elephant_filter_validate(unit, 0,
                                                &(profile->promotion_filters[i])));
    }
    BCM_IF_ERROR_RETURN(
            _bcm_th_ft_elephant_filter_validate(unit, 1,
                                         &(profile->demotion_filter)));

    if (options & BCM_FLOWTRACKER_ELEPHANT_PROFILE_WITH_ID) {
        if (*profile_id > BCM_INT_FT_MAX_ELEPHANT_PROFILES) {
            return BCM_E_PARAM;
        }
        /* Reserve the requested Id */
        rv = shr_idxres_list_reserve(ft_info->elph_profile_pool,
                                     *profile_id, *profile_id);
        if (BCM_FAILURE(rv)) {
            return ((rv == BCM_E_RESOURCE) ? BCM_E_EXISTS : rv);
        }
    } else {
        /* Allocate a free ID */
        rv = shr_idxres_list_alloc(ft_info->elph_profile_pool,
                                   (shr_idxres_element_t *)profile_id);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* Add it to the list of elephant profiles */
    rv = _bcm_xgs5_ft_elph_profile_list_add(unit, *profile_id, profile);
    if (BCM_FAILURE(rv)) {
        /* Free up the profile id allocated */
        BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->elph_profile_pool,
                                                 *profile_id));
        return rv;
    }

    /* Send elephant profile create msg */
    rv = _bcm_xgs5_ft_eapp_send_elph_profile_create_msg(unit, *profile_id);
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->elph_profile_pool,
                                                 *profile_id));
        BCM_IF_ERROR_RETURN(_bcm_xgs5_ft_elph_profile_list_delete(unit,
                                                                  *profile_id));
        return rv;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th_ft_elephant_profile_destroy
 * Purpose:
 *       Destroy a flowtracker elephant profile.
 * Parameters:
 *      unit         - (IN)     BCM device number
 *      profile_id   - (IN/OUT) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_elephant_profile_destroy(
                                  int unit,
                                  bcm_flowtracker_elephant_profile_t profile_id)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (ft_info == NULL || ft_info->elph_mode != TRUE) {
        return BCM_E_INIT;
    }

    rv = shr_idxres_list_elem_state(ft_info->elph_profile_pool, profile_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Profile (ID:%d) does not exist.\n"),
                   unit, profile_id));
        return (BCM_E_NOT_FOUND);
    }


    /* Check if the elephant profile is in use by some flow group */
    rv = _bcm_xgs5_ft_flow_group_list_elph_profile_in_use_check(unit, profile_id);
    if (rv == BCM_E_EXISTS) {
        /* One cannot delete an elephant profile  without deleting the
         * association of the profile to the flow group.
         */
        return BCM_E_CONFIG;
    }


    /* Delete the elephant profile from the list with
     * given ID.
     */
    rv = _bcm_xgs5_ft_elph_profile_list_delete(unit, profile_id);
    /* If not found in the list, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Inform FT EAPP of deleted profile */
    rv = _bcm_xgs5_ft_eapp_send_elph_profile_delete_msg(unit, profile_id);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Free up the ID from the pool */
    BCM_IF_ERROR_RETURN(shr_idxres_list_free(ft_info->elph_profile_pool,
                                             profile_id));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_elephant_profile_get
 * Purpose:
 *       Get flowtracker elephant profile information.
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      profile_id   - (IN)  Elephant profile id
 *      profile      - (OUT) Elephant profile information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_elephant_profile_get(
                                int unit,
                                bcm_flowtracker_elephant_profile_t profile_id,
                                bcm_flowtracker_elephant_profile_info_t *profile)
{
    _bcm_int_ft_info_t              *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_elph_profile_info_t  profile_int_info;
    int rv = BCM_E_NONE;

    if (ft_info == NULL || ft_info->elph_mode != TRUE) {
        return BCM_E_INIT;
    }

    if (profile == NULL) {
        return BCM_E_PARAM;
    }

    rv = shr_idxres_list_elem_state(ft_info->elph_profile_pool, profile_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Profile (ID:%d) does not exist.\n"),
                   unit, profile_id));
        return (BCM_E_NOT_FOUND);
    }


    BCM_IF_ERROR_RETURN(
                  _bcm_xgs5_ft_elph_profile_list_get(unit, profile_id,
                                                     &profile_int_info));

    sal_memcpy(profile, &(profile_int_info.profile),
               sizeof(bcm_flowtracker_elephant_profile_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_elephant_profile_get_all
 * Purpose:
 *       Get the list of all flowtracker elephant profiles configured.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      max           - (IN)  Max number of profile information that can be
 *                            accomodated within profile_list.
 *      profile_list  - (OUT) Elephant profile information
 *      count         - (OUT) Actual number of elephant profiles
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_elephant_profile_get_all(
                           int unit,
                           int max,
                           bcm_flowtracker_elephant_profile_t *profile_list,
                           int *count)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int num_profiles, i;

    if (ft_info == NULL || ft_info->elph_mode != TRUE) {
        return BCM_E_INIT;
    }

    /* If the max_size is non zero and list == NULL, return
     * BCM_E_PARAM.
     */
    if (max != 0 && profile_list == NULL) {
        return BCM_E_PARAM;
    }

    num_profiles = 0;
    *count       = 0;
    for (i = BCM_INT_FT_ELPH_PROFILE_START_ID;
         i <= BCM_INT_FT_ELPH_PROFILE_END_ID;
         i++) {
        if (ft_elph_profile_info_list[unit][i].id == i) {
            /* Profile exists */
            num_profiles++;
            if (*count < max) {
                profile_list[*count] = i;
                (*count)++;
            }
        }
    }

    if (max == 0) {
        /* Special case, just return the number of profiles */
        *count = num_profiles;
        return BCM_E_NONE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_group_elephant_profile_attach
 * Purpose:
 *       Attach a flow group with an elephant profile.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 *      profile_id    - (IN) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_group_elephant_profile_attach(
                                   int unit,
                                   bcm_flowtracker_group_t flow_group_id,
                                   bcm_flowtracker_elephant_profile_t profile_id)
{
    _bcm_int_ft_info_t           *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    int                           rv = BCM_E_NONE;

    if (ft_info == NULL || ft_info->elph_mode != TRUE) {
        return BCM_E_INIT;
    }

    /* Check if profile ID exists */
    rv = shr_idxres_list_elem_state(ft_info->elph_profile_pool, profile_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Profile (ID:%d) does not exist.\n"),
                   unit, profile_id));
        return (BCM_E_NOT_FOUND);
    }

    /* Check if the flow group exists */
    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, flow_group_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, flow_group_id));
        return (BCM_E_NOT_FOUND);
    }

    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    /* Get the flow group info for this group id */
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id,
                                                     &flow_group_int_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(
                                 unit, flow_group_id,
                                 SHR_FT_GROUP_UPDATE_ELPH_PROFILE,
                                 profile_id);
    if (BCM_FAILURE(rv)) {
        return rv;
    }


    flow_group_int_info.elph_profile_id = profile_id;
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_modify(unit, flow_group_id,
                                                        &flow_group_int_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_group_elephant_profile_attach_get
 * Purpose:
 *       Get the elephant profile Id attached with a flow group.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      flow_group_id - (IN)  Flow group Id
 *      profile_id    - (OUT) Elephant profile id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th_ft_group_elephant_profile_attach_get(
                                  int unit,
                                  bcm_flowtracker_group_t flow_group_id,
                                  bcm_flowtracker_elephant_profile_t *profile_id)
{
    _bcm_int_ft_info_t           *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    int rv = BCM_E_NONE;

    if (ft_info == NULL || ft_info->elph_mode != TRUE) {
        return BCM_E_INIT;
    }

    /* Check if the flow group exists */
    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, flow_group_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, flow_group_id));
        return (BCM_E_NOT_FOUND);
    }

    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    /* Get the flow group info for this group id */
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id,
                                                     &flow_group_int_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (flow_group_int_info.elph_profile_id == 0) {
        return BCM_E_NOT_FOUND;
    }
    *profile_id = flow_group_int_info.elph_profile_id;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_group_elephant_profile_detach
 * Purpose:
 *       Detach a flow group with an elephant profile.
 * Parameters:
 *      unit          - (IN) BCM device number
 *      flow_group_id - (IN) Flow group Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_th_ft_group_elephant_profile_detach(int unit,
                                         bcm_flowtracker_group_t flow_group_id)
{
    _bcm_int_ft_info_t     *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t flow_group_int_info;
    int rv = BCM_E_NONE;

    if (ft_info == NULL || ft_info->elph_mode != TRUE) {
        return BCM_E_INIT;
    }

    /* Check if the flow group exists */
    rv = shr_idxres_list_elem_state(ft_info->flow_group_pool, flow_group_id);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Group (ID:%d) does not exist.\n"),
                   unit, flow_group_id));
        return (BCM_E_NOT_FOUND);
    }

    sal_memset(&flow_group_int_info, 0, sizeof(flow_group_int_info));
    /* Get the flow group info for this group id */
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_get(unit, flow_group_id,
                                                     &flow_group_int_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (flow_group_int_info.elph_profile_id == 0) {
        /* Profile not attached */
        return BCM_E_PARAM;
    }

    if (flow_group_int_info.flow_limit != 0) {
        /* Flow limit has to be set to 0 before detaching elephant profile */
        return BCM_E_INTERNAL;
    }

    rv = _bcm_xgs5_ft_eapp_send_flow_group_update_msg(
                                 unit, flow_group_id,
                                 SHR_FT_GROUP_UPDATE_ELPH_PROFILE,
                                 0);
    if (BCM_FAILURE(rv)) {
        return rv;
    }


    flow_group_int_info.elph_profile_id = 0;
    rv = _bcm_xgs5_ft_flow_group_list_flow_group_modify(unit, flow_group_id,
                                                        &flow_group_int_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    return BCM_E_NONE;
}


#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_FT_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_FT_VERSION_1_1      SOC_SCACHE_VERSION(1,1)
#define BCM_WB_FT_VERSION_1_2      SOC_SCACHE_VERSION(1,2)
#define BCM_WB_FT_DEFAULT_VERSION  BCM_WB_FT_VERSION_1_2

#define FT_SCACHE_WRITE(_scache, _val, _type)                     \
    do {                                                          \
        _type _tmp = (_type) (_val);                              \
        sal_memcpy((_scache), &(_tmp), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
    } while(0)

#define FT_SCACHE_READ(_scache, _var, _type)                      \
    do {                                                          \
        _type _tmp;                                               \
        sal_memcpy(&(_tmp), (_scache), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
        (_var)     = (_tmp);                                      \
    } while(0)

/*
 * Function:
 *     _bcm_th_ft_scache_v0_global_size_get
 * Purpose:
 *     Get the size required to sync Flowtracker global data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th_ft_scache_v0_global_size_get(int unit)
{
    uint32 size = 0;

    /* Reserved bits */
    size += sizeof(uint32);

    /* Number of uC */
    size += sizeof(uint8);

    /* Mode */
    size += sizeof(uint8);

    /* Observation domain Id */
    size += sizeof(uint32);

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_global_sync
 * Purpose:
 *     Sync Flowtracker global data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th_ft_scache_v0_global_sync(int unit, uint8 **scache)
{
    _bcm_int_ft_info_t     *ft_info = FT_INFO_GET(unit);

    /* Reserve first 32 bits */
    FT_SCACHE_WRITE(*scache, 0, uint32);

    /* Number of uC */
    FT_SCACHE_WRITE(*scache, 1, uint8);

    if (ft_info->elph_mode) {
        /* Elephant mode */
        FT_SCACHE_WRITE(*scache, 1, uint8);
    } else {
        /* Standard mode */
        FT_SCACHE_WRITE(*scache, 0, uint8);
    }

    /* Observation Domain ID */
    FT_SCACHE_WRITE(*scache, ft_info->observation_domain_id, uint32);
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_global_recover
 * Purpose:
 *     Recover Flowtracker global data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v0_global_recover(int unit, uint8 **scache)
{
    uint8 num_uc;
    uint8 mode;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    /* Reserved bits */
    *scache += sizeof(uint32);

    /* Number of uC */
    FT_SCACHE_READ(*scache, num_uc, uint8);
    if (num_uc != 1) {
        return BCM_E_PARAM;
    }

    /* Mode */
    FT_SCACHE_READ(*scache, mode, uint8);
    /* Only possible mode is elephant/standard. Elephant mode is recovered
     * from soc property, validate if they are in agreement.
     */
    if (mode > 1) {
        return BCM_E_PARAM;
    } else if (mode == 1 && !ft_info->elph_mode) {
        return BCM_E_PARAM;
    } else if (mode == 0 && ft_info->elph_mode) {
        return BCM_E_PARAM;
    }

    /* Observation Domain ID */
    FT_SCACHE_READ(*scache, ft_info->observation_domain_id, uint32);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_group_size_get
 * Purpose:
 *     Get the size required to sync Flowtracker group data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th_ft_scache_v0_group_size_get(int unit)
{
    uint32 size = 0;
    int i, j;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    /* group Id */
    size += sizeof(uint16);

    /* in_use */
    size += sizeof(uint8);

    /* Collector Id */
    size += sizeof(uint16);

    /* Template Id */
    size += sizeof(uint16);

    /* Aging interval */
    size += sizeof(uint32);

    /* Flow limit */
    size += sizeof(uint32);

    /* Elephant profile Id */
    size += sizeof(uint16);

    /* QoS profile Id */
    size += sizeof(uint16);

    /* Number of actions */
    size += sizeof(uint16);

    for (i = 0; i < BCM_INT_FT_MAX_GROUP_ACTIONS; i++) {
        /* Action */
        size += sizeof(uint16);

        /* Number of params */
        size += sizeof(uint8);

        for (j = 0; j < BCM_INT_FT_MAX_GROUP_ACTION_PARAMS; j++) {
            /* Action params */
            size += sizeof(uint32);
        }
    }

    /* Multiply by number of groups */
    size *= ft_info->max_flow_groups;

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_group_sync
 * Purpose:
 *     Sync Flowtracker group data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th_ft_scache_v0_group_sync(int unit, uint8 **scache)
{
    int i, j, k;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t *flow_group_list = ft_flow_group_info_list[unit];
    _bcm_int_ft_flow_group_info_t *group = NULL;

    for (i = BCM_INT_FT_GROUP_START_ID;
         i < (ft_info->max_flow_groups + BCM_INT_FT_GROUP_START_ID);
         i++) {
        group = &(flow_group_list[i]);

        /* group Id */
        FT_SCACHE_WRITE(*scache, i, uint16);

        if (group->flow_group_id == i) {
            /* Group exists, set in_use = 1 */
            FT_SCACHE_WRITE(*scache, 1, uint8);
        } else {
            /* Group does not exist, set in_use = 0 */
            FT_SCACHE_WRITE(*scache, 0, uint8);
            continue;
        }

        /* Collector Id */
        FT_SCACHE_WRITE(*scache, group->collector_id, uint16);

        /* Template Id */
        FT_SCACHE_WRITE(*scache, group->template_id, uint16);

        /* Aging interval */
        FT_SCACHE_WRITE(*scache, group->aging_interval_msecs, uint32);

        /* Flow limit */
        FT_SCACHE_WRITE(*scache, group->flow_limit, uint32);

        /* Elephant profile Id */
        FT_SCACHE_WRITE(*scache, group->elph_profile_id, uint16);

        /* QoS profile Id */
        FT_SCACHE_WRITE(*scache, group->qos_profile_id, uint16);

        /* Number of actions */
        FT_SCACHE_WRITE(*scache, group->num_actions, uint16);

        for (j = 0; j < group->num_actions; j++) {
            /* Action */
            FT_SCACHE_WRITE(*scache, group->action_list[j].action, uint16);

            /* Number of params */
            FT_SCACHE_WRITE(*scache, group->action_list[j].num_params, uint8);

            for (k = 0; k < group->action_list[j].num_params; k++) {
                /* Action params */
                FT_SCACHE_WRITE(*scache, group->action_list[j].params[k], uint32);
            }
        }
    }
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_group_recover
 * Purpose:
 *     Recover Flowtracker group data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v0_group_recover(int unit, uint8 **scache)
{
    int i, j, k;
    int rv;
    int pipe;
    uint8 num_params;
    uint8 in_use;
    uint16 group_id;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t *flow_group_list = ft_flow_group_info_list[unit];
    _bcm_int_ft_flow_group_info_t *group = NULL;

    for (i = BCM_INT_FT_GROUP_START_ID;
         i < (ft_info->max_flow_groups + BCM_INT_FT_GROUP_START_ID);
         i++) {
        group = &(flow_group_list[i]);

        /* Group ID*/
        FT_SCACHE_READ(*scache, group_id, uint16);
        if (group_id != i) {
            return BCM_E_INTERNAL;
        }

        /* in_use */
        FT_SCACHE_READ(*scache, in_use, uint8);
        if (in_use == 0) {
            continue;
        }
        group->flow_group_id = i;

        rv = shr_idxres_list_reserve(ft_info->flow_group_pool, i, i);
        BCM_IF_ERROR_RETURN(rv);

        /* Collector Id */
        FT_SCACHE_READ(*scache, group->collector_id, uint16);
        if (ft_info->cfg_mode >= SHR_FT_CFG_MODE_V2 &&
            (group->collector_id != _BCM_FT_WB_INVALID_COLLECTOR_ID)) {
            BCM_IF_ERROR_RETURN(
                   _bcm_esw_collector_ref_count_update(unit,
                                                       group->collector_id, 1));
        }


        /* Template Id */
        FT_SCACHE_READ(*scache, group->template_id, uint16);

        /* Aging interval */
        FT_SCACHE_READ(*scache, group->aging_interval_msecs, uint32);

        /* Flow limit */
        FT_SCACHE_READ(*scache, group->flow_limit, uint32);

        /* Elephant profile Id */
        FT_SCACHE_READ(*scache, group->elph_profile_id, uint16);

        /* QoS profile Id */
        FT_SCACHE_READ(*scache, group->qos_profile_id, uint16);
        if (group->qos_profile_id != 0) {
            for (pipe = 0; pipe < ft_info->num_pipes; pipe++) {
                rv = soc_profile_mem_reference(unit,
                                               ft_info->pipe_info[pipe].qos_profile_mem,
                                               group->qos_profile_id, 1);
                BCM_IF_ERROR_RETURN(rv);
            }
        }

        /* Number of actions */
        FT_SCACHE_READ(*scache, group->num_actions, uint16);
        if (group->num_actions > BCM_INT_FT_MAX_GROUP_ACTIONS) {
            return BCM_E_INTERNAL;
        }

        /* Allocate memory for the action list */
        if (group->num_actions > 0) {
            _BCM_FT_ALLOC(group->action_list,
                          _bcm_int_ft_group_action_info_t,
                          group->num_actions * sizeof(_bcm_int_ft_group_action_info_t),
                          "Group action list");
            if (group->action_list == NULL) {
                return BCM_E_MEMORY;
            }
        }

        for (j = 0; j < group->num_actions; j++) {
            /* Action */
            FT_SCACHE_READ(*scache, group->action_list[j].action, uint16);

            if (group->action_list[j].action >= bcmFlowtrackerGroupActionCount) {
                /* Unrecognized action */
                return BCM_E_INTERNAL;
            }

            /* Number of params */
            FT_SCACHE_READ(*scache, num_params, uint8);
            if (num_params > BCM_INT_FT_MAX_GROUP_ACTION_PARAMS) {
                return BCM_E_INTERNAL;
            }

            /* Action params */
            for (k = 0; k < num_params; k++) {
                FT_SCACHE_READ(*scache, group->action_list[j].params[k], uint32);
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_collector_size_get
 * Purpose:
 *     Get the size required to sync Flowtracker collector data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th_ft_scache_v0_collector_size_get(int unit)
{
    uint32 size = 0;

    /* Collector Id */
    size += sizeof(uint16);

    /* in_use */
    size += sizeof(uint8);

    /* Transport type */
    size += sizeof(uint16);

    /* Dst mac addr */
    size += (sizeof(uint8) * 6);

    /* Src mac addr */
    size += (sizeof(uint8) * 6);

    /* Vlan tag structure */
    size += sizeof(uint8);

    /* Outer TPID */
    size += sizeof(uint16);

    /* Inner TPID */
    size += sizeof(uint16);

    /* Outer vlan tag */
    size += sizeof(uint32);

    /* Inner vlan tag */
    size += sizeof(uint32);

    /* Sync IP header (only IPv4 is supported) */
    /* Src IP */
    size += sizeof(uint32);

    /* Dst IP */
    size += sizeof(uint32);

    /* DSCP */
    size += sizeof(uint8);

    /* TTL */
    size += sizeof(uint8);

    /* Src ports */
    size += sizeof(uint16);

    /* Dst port */
    size += sizeof(uint16);

    /* Max pkt length */
    size += sizeof(uint16);

    /* Multiply by number of collectors */
    size *= BCM_INT_FT_MAX_COLLECTORS;

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_collector_sync
 * Purpose:
 *     Sync Flowtracker collector data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th_ft_scache_v0_collector_sync(int unit, uint8 **scache)
{
    int i, j;
    _bcm_int_ft_collector_info_t *collector_list = ft_collector_info_list[unit];
    _bcm_int_ft_collector_info_t *collector = NULL;

    for (i = BCM_INT_FT_COLLECTOR_START_ID; i <= BCM_INT_FT_COLLECTOR_END_ID; i++) {

        collector = &(collector_list[i]);

        /* Collector Id */
        FT_SCACHE_WRITE(*scache, i, uint16);

        if (collector->collector_id == i) {
            /* Collector exists, set in_use = 1 */
            FT_SCACHE_WRITE(*scache, 1, uint8);
        } else {
            /* Collector does not exist, set in_use = 0 */
            FT_SCACHE_WRITE(*scache, 0, uint8);
            continue;
        }


        /* Transport type */
        FT_SCACHE_WRITE(*scache, collector->collector_info.transport_type, uint16);

        /* Dst mac addr */
        for (j = 0; j < 6; j++) {
            FT_SCACHE_WRITE(*scache, collector->collector_info.eth.dst_mac[j],
                            uint8);
        }

        /* Src mac addr */
        for (j = 0; j < 6; j++) {
            FT_SCACHE_WRITE(*scache, collector->collector_info.eth.src_mac[j],
                            uint8);
        }

        /* Vlan tag structure */
        FT_SCACHE_WRITE(*scache, collector->collector_info.eth.vlan_tag_structure,
                        uint8);

        /* Outer TPID */
        FT_SCACHE_WRITE(*scache, collector->collector_info.eth.outer_tpid, uint16);

        /* Inner TPID */
        FT_SCACHE_WRITE(*scache, collector->collector_info.eth.inner_tpid, uint16);

        /* Outer vlan tag */
        FT_SCACHE_WRITE(*scache, collector->collector_info.eth.outer_vlan_tag,
                        uint32);

        /* Inner vlan tag */
        FT_SCACHE_WRITE(*scache, collector->collector_info.eth.inner_vlan_tag,
                        uint32);

        /* Sync IP header (only IPv4 is supported) */
        /* Src IP */
        FT_SCACHE_WRITE(*scache, collector->collector_info.ipv4.src_ip, uint32);

        /* Dst IP */
        FT_SCACHE_WRITE(*scache, collector->collector_info.ipv4.dst_ip, uint32);

        /* DSCP */
        FT_SCACHE_WRITE(*scache, collector->collector_info.ipv4.dscp, uint8);

        /* TTL */
        FT_SCACHE_WRITE(*scache, collector->collector_info.ipv4.ttl, uint8);

        /* Src port */
        FT_SCACHE_WRITE(*scache, collector->collector_info.udp.src_port, uint16);

        /* Dst port */
        FT_SCACHE_WRITE(*scache, collector->collector_info.udp.dst_port, uint16);

        /* Max pkt length */
        FT_SCACHE_WRITE(*scache, collector->collector_info.max_packet_length, uint16);
    }
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_collector_recover
 * Purpose:
 *     Recover Flowtracker collector data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v0_collector_recover(int unit, uint8 **scache)
{
    int i, j;
    int rv;
    uint8 in_use;
    uint16 collector_id;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_collector_info_t *collector_list = ft_collector_info_list[unit];
    _bcm_int_ft_collector_info_t *collector = NULL;

    for (i = BCM_INT_FT_COLLECTOR_START_ID; i <= BCM_INT_FT_COLLECTOR_END_ID; i++) {

        collector = &(collector_list[i]);

        /* Collector Id */
        FT_SCACHE_READ(*scache, collector_id, uint16);
        if (collector_id != i) {
            return BCM_E_INTERNAL;
        }

        /* in_use */
        FT_SCACHE_READ(*scache, in_use, uint8);
        if (in_use == 0) {
            continue;
        }

        collector->collector_id = i;

        rv = shr_idxres_list_reserve(ft_info->collector_pool, i, i);
        BCM_IF_ERROR_RETURN(rv);

        /* Transport type */
        FT_SCACHE_READ(*scache, collector->collector_info.transport_type, uint16);
        if (collector->collector_info.transport_type !=
                              bcmFlowtrackerCollectorTransportTypeIpfixIpv4Udp) {
            return BCM_E_INTERNAL;
        }

        /* Dst mac addr */
        for (j = 0; j < 6; j++) {
            FT_SCACHE_READ(*scache, collector->collector_info.eth.dst_mac[j],
                            uint8);
        }

        /* Src mac addr */
        for (j = 0; j < 6; j++) {
            FT_SCACHE_READ(*scache, collector->collector_info.eth.src_mac[j],
                            uint8);
        }

        /* Vlan tag structure */
        FT_SCACHE_READ(*scache, collector->collector_info.eth.vlan_tag_structure,
                        uint8);

        /* Outer TPID */
        FT_SCACHE_READ(*scache, collector->collector_info.eth.outer_tpid, uint16);

        /* Inner TPID */
        FT_SCACHE_READ(*scache, collector->collector_info.eth.inner_tpid, uint16);

        /* Outer vlan tag */
        FT_SCACHE_READ(*scache, collector->collector_info.eth.outer_vlan_tag,
                        uint32);

        /* Inner vlan tag */
        FT_SCACHE_READ(*scache, collector->collector_info.eth.inner_vlan_tag,
                        uint32);

        /* IP header (only IPv4 is supported) */
        /* Src IP */
        FT_SCACHE_READ(*scache, collector->collector_info.ipv4.src_ip, uint32);

        /* Dst IP */
        FT_SCACHE_READ(*scache, collector->collector_info.ipv4.dst_ip, uint32);

        /* DSCP */
        FT_SCACHE_READ(*scache, collector->collector_info.ipv4.dscp, uint8);

        /* TTL */
        FT_SCACHE_READ(*scache, collector->collector_info.ipv4.ttl, uint8);

        /* Src port */
        FT_SCACHE_READ(*scache, collector->collector_info.udp.src_port, uint16);

        /* Dst port */
        FT_SCACHE_READ(*scache, collector->collector_info.udp.dst_port, uint16);

        /* Max pkt length */
        FT_SCACHE_READ(*scache, collector->collector_info.max_packet_length, uint16);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_template_size_get
 * Purpose:
 *     Get the size required to sync Flowtracker template data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th_ft_scache_v0_template_size_get(int unit)
{
    int i;
    uint32 size = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    /* Template Id */
    size += sizeof(uint16);

    /* in_use */
    size += sizeof(uint8);

    /* Set ID */
    size += sizeof(uint16);

    /* Number of export elements */
    size += sizeof(uint16);

    if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
        for (i = 0; i < SHR_QCM_TEMPLATE_MAX_INFO_ELEMENTS; i++) {
            /* Element Id */
            size += sizeof(uint16);

            /* Data size */
            size += sizeof(uint8);

            /* Information Element Id */
            size += sizeof(uint16);

            /* Enterprise flag */
            size += sizeof(uint8);
        }
    } else {
        for (i = 0; i < SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS; i++) {
            /* Element Id */
            size += sizeof(uint16);

            /* Data size */
            size += sizeof(uint8);

            /* Information Element Id */
            size += sizeof(uint16);

            /* Enterprise flag */
            size += sizeof(uint8);
        }
    }

    /* Only first template in scache v0 */
    size *= 1;

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_template_sync
 * Purpose:
 *     Sync Flowtracker template data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     None
 */
STATIC void
_bcm_th_ft_scache_v0_template_sync(int unit, uint8 **scache)
{
    int j;
    _bcm_int_ft_export_template_info_t *template_list = ft_export_template_info_list[unit];
    _bcm_int_ft_export_template_info_t *template = NULL;

    /* Only first template id written to scache v0 */
    template = &(template_list[BCM_INT_FT_TEMPLATE_START_ID]);

    /* Template Id */
    FT_SCACHE_WRITE(*scache, BCM_INT_FT_TEMPLATE_START_ID, uint16);

    if(template->template_id == BCM_INT_FT_TEMPLATE_START_ID) {
        /* Template exists, set in_use = 1 */
        FT_SCACHE_WRITE(*scache, 1, uint8);
    } else {
        /* Template does not exist, set in_use = 0 */
        FT_SCACHE_WRITE(*scache, 0, uint8);
        return;
    }


    /* Set ID */
    FT_SCACHE_WRITE(*scache, template->set_id, uint16);

    /* Number of export elements */
    FT_SCACHE_WRITE(*scache, template->num_export_elements, uint16);

    for (j = 0; j < template->num_export_elements; j++) {
        /* Element Id */
        FT_SCACHE_WRITE(*scache, template->elements[j].element, uint16);

        /* Data size */
        FT_SCACHE_WRITE(*scache, template->elements[j].data_size, uint16);

        /* Information Element Id */
        FT_SCACHE_WRITE(*scache, template->elements[j].id, uint16);

        /* Enterprise flag */
        FT_SCACHE_WRITE(*scache, template->elements[j].enterprise, uint8);
    }
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_template_recover
 * Purpose:
 *     Recover Flowtracker template data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v0_template_recover(int unit, uint8 **scache)
{
    int j;
    int rv;
    uint8 in_use;
    uint16 template_id;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_export_template_info_t *template = NULL;

    ft_info->num_templates = 0;

    /* Only first template id to be recovered from scache v0 */
    template = &(ft_export_template_info_list[unit][BCM_INT_FT_TEMPLATE_START_ID]);

    /* Template Id */
    FT_SCACHE_READ(*scache, template_id, uint16);
    if (template_id != BCM_INT_FT_TEMPLATE_START_ID) {
        return BCM_E_INTERNAL;
    }

    /* in_use */
    FT_SCACHE_READ(*scache, in_use, uint8);
    if (in_use == 0) {
        return BCM_E_NONE;
    }

    template->template_id = BCM_INT_FT_TEMPLATE_START_ID;

    rv = shr_idxres_list_reserve(ft_info->template_pool, 1, 1);
    BCM_IF_ERROR_RETURN(rv);

    /* Set ID */
    FT_SCACHE_READ(*scache, template->set_id, uint16);

    /* Number of export elements */
    FT_SCACHE_READ(*scache, template->num_export_elements, uint16);

    for (j = 0; j < template->num_export_elements; j++) {
        /* Element Id */
        FT_SCACHE_READ(*scache, template->elements[j].element, uint16);
        if (template->elements[j].element >= SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS) {
            /* Unrecognized element */
            return BCM_E_INTERNAL;
        }

        /* Data size */
        FT_SCACHE_READ(*scache, template->elements[j].data_size, uint16);

        /* Information Element Id */
        FT_SCACHE_READ(*scache, template->elements[j].id, uint16);

        /* Enterprise flag */
        FT_SCACHE_READ(*scache, template->elements[j].enterprise, uint8);
    }

    ft_info->num_templates += 1;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_template_xmit_size_get
 * Purpose:
 *     Get the size required to sync Flowtracker template xmit data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th_ft_scache_v0_template_xmit_size_get(int unit)
{
    uint32 size = 0;

    /* Template ID */
    size += sizeof(uint16);

    /* Collector Id */
    size += sizeof(uint16);

    /* Interval secs */
    size += sizeof(uint16);

    /* Initial burst */
    size += sizeof(uint16);

    /* Only first template in scache v0 */
    size *= (BCM_INT_FT_MAX_COLLECTORS * 1);

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_template_xmit_sync
 * Purpose:
 *     Sync Flowtracker template xmit data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th_ft_scache_v0_template_xmit_sync(int unit, uint8 **scache)
{
    _bcm_int_ft_export_template_info_t *template = NULL;

    /* Template xmit is stored 'template x collector' number of time.
     * only one collector and template written to scache v0, so only a single xmit config is
     * writen to scache v0.
     */
    template  = &(ft_export_template_info_list[unit][BCM_INT_FT_TEMPLATE_START_ID]);

    /* Template ID */
    FT_SCACHE_WRITE(*scache, 1, uint16);

    /* Collector Id */
    FT_SCACHE_WRITE(*scache, 1, uint16);

    /* Interval secs */
    FT_SCACHE_WRITE(*scache, template->interval_secs, uint16);

    /* Initial burst */
    FT_SCACHE_WRITE(*scache, template->initial_burst, uint16);

}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_template_xmit_recover
 * Purpose:
 *     Recover Flowtracker template xmit data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v0_template_xmit_recover(int unit, uint8 **scache)
{
    _bcm_int_ft_export_template_info_t *template = NULL;

    /* Template xmit is stored 'template x collector' number of time.
     * Only first template id gets recovred from scache v0 , so the template xmit too.
     */
    template  = &(ft_export_template_info_list[unit][BCM_INT_FT_TEMPLATE_START_ID]);

    /* Skip template ID */
    *scache += sizeof(uint16);

    /* Skip Collector Id */
    *scache += sizeof(uint16);

    /* Interval secs */
    FT_SCACHE_READ(*scache, template->interval_secs, uint16);

    /* Initial burst */
    FT_SCACHE_READ(*scache, template->initial_burst, uint16);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_elph_prof_size_get
 * Purpose:
 *     Get the size required to sync Flowtracker elephant profile
 *     data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th_ft_scache_v0_elph_prof_size_get(int unit)
{
    int    i;
    uint32 size = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (!ft_info->elph_mode) {
        return 0;
    }

    /* Profile Id */
    size += sizeof(uint8);

    /* in_use */
    size += sizeof(uint8);

    /* Num promotion filters */
    size += sizeof(uint8);

    for (i = 0; i < BCM_FLOWTRACKER_ELEPHANT_MAX_PROMOTION_FILTERS; i++) {
        /* Flags */
        size += sizeof(uint32);

        /* Monitoring Interval */
        size += sizeof(uint32);

        /* Low rate threshold */
        size += sizeof(uint32);

        /* High rate threshold */
        size += sizeof(uint32);

        /* Size threshold upper */
        size += sizeof(uint32);

        /* Size threshold lower */
        size += sizeof(uint32);
    }

    /* Flags */
    size += sizeof(uint32);

    /* Monitoring Interval */
    size += sizeof(uint32);

    /* Low rate threshold */
    size += sizeof(uint32);

    /* High rate threshold */
    size += sizeof(uint32);

    /* Size threshold upper */
    size += sizeof(uint32);

    /* Size threshold lower */
    size += sizeof(uint32);

    /* Multiply by number of elephant profiles */
    size *= BCM_INT_FT_MAX_ELEPHANT_PROFILES;

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_elph_prof_sync
 * Purpose:
 *     Sync Flowtracker elephant profile data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th_ft_scache_v0_elph_prof_sync(int unit, uint8 **scache)
{
    int i, j;
    uint32 u32_h, u32_l;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_elph_profile_info_t *elph_profile_list = ft_elph_profile_info_list[unit];
    _bcm_int_ft_elph_profile_info_t *elph = NULL;
    bcm_flowtracker_elephant_filter_t *filter = NULL;

    if (!ft_info->elph_mode) {
        return;
    }

    for (i = BCM_INT_FT_ELPH_PROFILE_START_ID; i <= BCM_INT_FT_ELPH_PROFILE_END_ID; i++) {
        elph = &(elph_profile_list[i]);

        /* Profile Id */
        FT_SCACHE_WRITE(*scache, i, uint8);

        if (elph->id == i) {
            /* Profile exists, set in_use = 1 */
            FT_SCACHE_WRITE(*scache, 1, uint8);
        } else {
            /* Profile does not exist, set in_use = 0 */
            FT_SCACHE_WRITE(*scache, 0, uint8);
            continue;
        }

        /* Num promotion filters */
        FT_SCACHE_WRITE(*scache, elph->profile.num_promotion_filters, uint8);

        for (j = 0; j < elph->profile.num_promotion_filters; j++) {
            filter = &(elph->profile.promotion_filters[j]);

            /* Flags */
            FT_SCACHE_WRITE(*scache, filter->flags, uint32);

            /* Monitoring Interval */
            FT_SCACHE_WRITE(*scache, filter->monitor_interval_usecs, uint32);

            /* Low rate threshold */
            FT_SCACHE_WRITE(*scache, filter->rate_low_threshold_kbits_sec, uint32);

            /* High rate threshold */
            FT_SCACHE_WRITE(*scache, filter->rate_high_threshold_kbits_sec, uint32);

            /* Size threshold */
            COMPILER_64_TO_32_HI(u32_h, filter->size_threshold_bytes);
            COMPILER_64_TO_32_LO(u32_l, filter->size_threshold_bytes);
            FT_SCACHE_WRITE(*scache, u32_h, uint32);
            FT_SCACHE_WRITE(*scache, u32_l, uint32);
        }

        filter = &(elph->profile.demotion_filter);

        /* Flags */
        FT_SCACHE_WRITE(*scache, filter->flags, uint32);

        /* Monitoring Interval */
        FT_SCACHE_WRITE(*scache, filter->monitor_interval_usecs, uint32);

        /* Low rate threshold */
        FT_SCACHE_WRITE(*scache, filter->rate_low_threshold_kbits_sec, uint32);

        /* High rate threshold */
        FT_SCACHE_WRITE(*scache, filter->rate_high_threshold_kbits_sec, uint32);

        /* Size threshold */
        COMPILER_64_TO_32_HI(u32_h, filter->size_threshold_bytes);
        COMPILER_64_TO_32_LO(u32_l, filter->size_threshold_bytes);
        FT_SCACHE_WRITE(*scache, u32_h, uint32);
        FT_SCACHE_WRITE(*scache, u32_l, uint32);

    }
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_elph_prof_recover
 * Purpose:
 *     Recover Flowtracker elephant profile data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v0_elph_prof_recover(int unit, uint8 **scache)
{
    int i, j;
    int rv;
    uint8 in_use;
    uint32 u32_h, u32_l;
    uint16 profile_id;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_elph_profile_info_t *elph_profile_list = ft_elph_profile_info_list[unit];
    _bcm_int_ft_elph_profile_info_t *elph = NULL;
    bcm_flowtracker_elephant_filter_t *filter = NULL;

    if (!ft_info->elph_mode) {
        return BCM_E_NONE;
    }

    for (i = BCM_INT_FT_ELPH_PROFILE_START_ID; i <= BCM_INT_FT_ELPH_PROFILE_END_ID; i++) {
        elph = &(elph_profile_list[i]);

        /* Profile Id */
        FT_SCACHE_READ(*scache, profile_id, uint8);
        if (profile_id != i) {
            return BCM_E_INTERNAL;
        }

        /* in_use */
        FT_SCACHE_READ(*scache, in_use, uint8);
        if (in_use == 0) {
            continue;
        }

        elph->id = i;

        rv = shr_idxres_list_reserve(ft_info->elph_profile_pool, i, i);
        BCM_IF_ERROR_RETURN(rv);

        /* Num promotion filters */
        FT_SCACHE_READ(*scache, elph->profile.num_promotion_filters, uint8);
        if (elph->profile.num_promotion_filters >
                             BCM_FLOWTRACKER_ELEPHANT_MAX_PROMOTION_FILTERS) {
            return BCM_E_INTERNAL;
        }

        for (j = 0; j < elph->profile.num_promotion_filters; j++) {
            filter = &(elph->profile.promotion_filters[j]);

            /* Flags */
            FT_SCACHE_READ(*scache, filter->flags, uint32);

            /* Monitoring Interval */
            FT_SCACHE_READ(*scache, filter->monitor_interval_usecs, uint32);

            /* Low rate threshold */
            FT_SCACHE_READ(*scache, filter->rate_low_threshold_kbits_sec, uint32);

            /* High rate threshold */
            FT_SCACHE_READ(*scache, filter->rate_high_threshold_kbits_sec, uint32);

            /* Size threshold */
            FT_SCACHE_READ(*scache, u32_h, uint32);
            FT_SCACHE_READ(*scache, u32_l, uint32);
            COMPILER_64_SET(filter->size_threshold_bytes, u32_h, u32_l);
        }

        filter = &(elph->profile.demotion_filter);

        /* Flags */
        FT_SCACHE_READ(*scache, filter->flags, uint32);

        /* Monitoring Interval */
        FT_SCACHE_READ(*scache, filter->monitor_interval_usecs, uint32);

        /* Low rate threshold */
        FT_SCACHE_READ(*scache, filter->rate_low_threshold_kbits_sec, uint32);

        /* High rate threshold */
        FT_SCACHE_READ(*scache, filter->rate_high_threshold_kbits_sec, uint32);

        /* Size threshold */
        FT_SCACHE_READ(*scache, u32_h, uint32);
        FT_SCACHE_READ(*scache, u32_l, uint32);
        COMPILER_64_SET(filter->size_threshold_bytes, u32_h, u32_l);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_size_get
 * Purpose:
 *     Get Flowtracker scache v0 size
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Size required
 */
STATIC uint32
_bcm_th_ft_scache_v0_size_get(int unit)
{
    uint32 size = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    /* Get global data size */
    size += _bcm_th_ft_scache_v0_global_size_get(unit);

    /* Get group data size */
    size += _bcm_th_ft_scache_v0_group_size_get(unit);

    /* Get collector data size */
    if (ft_info->cfg_mode == SHR_FT_CFG_MODE_V1) {
        size += _bcm_th_ft_scache_v0_collector_size_get(unit);
    }

    /* Get template data size */
    size += _bcm_th_ft_scache_v0_template_size_get(unit);

    /* Get template xmit data size */
    size += _bcm_th_ft_scache_v0_template_xmit_size_get(unit);

    /* Get elephant profile data size */
    size += _bcm_th_ft_scache_v0_elph_prof_size_get(unit);

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_sync
 * Purpose:
 *     Sync Flowtracker scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     None
 */
STATIC void
_bcm_th_ft_scache_v0_sync(int unit, uint8 **scache)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    /* Sync global data */
    _bcm_th_ft_scache_v0_global_sync(unit, scache);

    /* Sync group data */
    _bcm_th_ft_scache_v0_group_sync(unit, scache);

    /* Sync collector data */
    if (ft_info->cfg_mode == SHR_FT_CFG_MODE_V1) {
        _bcm_th_ft_scache_v0_collector_sync(unit, scache);
    }

    /* Sync template data */
    _bcm_th_ft_scache_v0_template_sync(unit, scache);

    /* Sync template xmit data */
    _bcm_th_ft_scache_v0_template_xmit_sync(unit, scache);

    /* Sync elephant profile data */
    _bcm_th_ft_scache_v0_elph_prof_sync(unit, scache);
}

/*
 * Function:
 *     _bcm_th_ft_scache_v0_recover
 * Purpose:
 *     Recover Flowtracker scache v0 data
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v0_recover(int unit, uint8 **scache)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    /* Recover global data */
    BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v0_global_recover(unit, scache));

    /* Recover group data */
    BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v0_group_recover(unit, scache));

    /* Recover collector data */
    if (ft_info->cfg_mode == SHR_FT_CFG_MODE_V1) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v0_collector_recover(unit, scache));
    }

    /* Recover template data */
    BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v0_template_recover(unit, scache));

    /* Recover template xmit data */
    BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v0_template_xmit_recover(unit, scache));

    /* Recover elephant profile data */
    BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v0_elph_prof_recover(unit, scache));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_global_size_get
 * Purpose:
 *     Get the size required to sync Flowtracker global data to scache v1
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th_ft_scache_v1_global_size_get(int unit)
{
    _bcm_int_ft_info_t     *ft_info = FT_INFO_GET(unit);
    uint32 size = 0;
    int i, j;

    /* Default group Id */
    size += sizeof(uint16);

    /* Maximum EM group */
    size += sizeof(uint16);

    /* EM group Mapping */
    for (i = 0; i < SHR_FT_MAX_EM_GROUPS; i++) {
        for (j = 0; j < ft_info->num_pipes; ++j) {
            size += sizeof(int);
        }
    }

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_global_sync
 * Purpose:
 *     Sync Flowtracker global data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th_ft_scache_v1_global_sync(int unit, uint8 **scache)
{
    _bcm_int_ft_info_t     *ft_info = FT_INFO_GET(unit);
    int i, j;

    /* Default group ID */
    FT_SCACHE_WRITE(*scache, ft_info->default_grp_id, uint16);

    /* Maximum EM group */
    FT_SCACHE_WRITE(*scache, SHR_FT_MAX_EM_GROUPS, uint16);

    /* EM group Mapping */
    for (i = 0; i < SHR_FT_MAX_EM_GROUPS; i++) {
        for (j = 0; j < ft_info->num_pipes; ++j) {
            FT_SCACHE_WRITE(*scache, ft_info->em_group_id_map[i][j], int);
        }
    }
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_global_recover
 * Purpose:
 *     Recover Flowtracker global data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v1_global_recover(int unit, uint8 **scache)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    int i, j;
    uint16 max_em_grp = 0;

    /* Default group ID */
    FT_SCACHE_READ(*scache, ft_info->default_grp_id, uint16);

    /* Maximum EM group */
    FT_SCACHE_READ(*scache, max_em_grp, uint16);

    /* EM group Mapping */
    for (i = 0; i < max_em_grp; i++) {
        for (j = 0; j < ft_info->num_pipes; ++j) {
            FT_SCACHE_READ(*scache, ft_info->em_group_id_map[i][j], int);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_group_size_get
 * Purpose:
 *     Get the size required to sync flow group data to scache v1
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th_ft_scache_v1_group_size_get(int unit)
{
    uint32 size = 0;
    int i;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    /* Group in_use */
    size += sizeof(uint8);

    /* Export profile Id */
    size += sizeof(int);

    /* Export triggers */
    size += sizeof(uint32);

    for (i = 0; i <  ft_info->num_pipes; i++) {
        /* Field group */
        size += sizeof(bcm_field_group_t);
    }

    /* Number of Tracking Params */
    size += sizeof(int);

    for (i = 0; i < SHR_FT_MAX_TP; i++) {
        /* Flags for tracking parameters. */
        size += sizeof(uint32);

        /* Type of tracking parameter. */
        size += sizeof(uint16);

        /* UDF Id */
        size += sizeof(int);
    }

    /* Multiply by number of groups */
    size *= ft_info->max_flow_groups;

    return (size);
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_group_sync
 * Purpose:
 *     Sync group data with scache v1
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th_ft_scache_v1_group_sync(int unit, uint8 **scache)
{
    int i, j;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t *flow_group_list = ft_flow_group_info_list[unit];
    _bcm_int_ft_flow_group_info_t *group = NULL;

    for (i = BCM_INT_FT_GROUP_START_ID;
         i < (ft_info->max_flow_groups + BCM_INT_FT_GROUP_START_ID);
         i++) {
        group = &(flow_group_list[i]);

        if (group->flow_group_id == i) {
            /* Group exists, set in_use = 1 */
            FT_SCACHE_WRITE(*scache, 1, uint8);
        } else {
            /* Group does not exist, set in_use = 0 */
            FT_SCACHE_WRITE(*scache, 0, uint8);
            continue;
        }

        /* Export profile Id */
        FT_SCACHE_WRITE(*scache, group->export_profile_id, int);

        /* Export triggers */
        FT_SCACHE_WRITE(*scache, group->export_trigger, uint32);

        /* Field group */
        for (j = 0; j < ft_info->num_pipes; ++j) {
            FT_SCACHE_WRITE(*scache, group->field_group[j], int);
        }

        /* Number of Tracking Params */
        FT_SCACHE_WRITE(*scache, group->num_tracking_params, int);

        for (j = 0; j < group->num_tracking_params; j++) {
            /* Flags for tracking parameters. */
            FT_SCACHE_WRITE(*scache, group->tracking_params[j].flags, uint32);

            /* Type of tracking parameter. */
            FT_SCACHE_WRITE(*scache, group->tracking_params[j].param, uint16);

            /* UDF Id */
            FT_SCACHE_WRITE(*scache, group->tracking_params[j].udf_id, int);
        }
    }
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_group_recover
 * Purpose:
 *     Recover Flowtracker group data from scache v1
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v1_group_recover(int unit, uint8 **scache)
{
    int i, j;
    uint8 in_use;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_flow_group_info_t *flow_group_list = ft_flow_group_info_list[unit];
    _bcm_int_ft_flow_group_info_t *group = NULL;
    bcm_collector_export_profile_t export_profile_info;

    for (i = BCM_INT_FT_GROUP_START_ID;
         i < (ft_info->max_flow_groups + BCM_INT_FT_GROUP_START_ID);
         i++) {
        group = &(flow_group_list[i]);

        /* in_use */
        FT_SCACHE_READ(*scache, in_use, uint8);
        if (in_use == 0) {
            continue;
        }

        /* Export profile Id */
        FT_SCACHE_READ(*scache, group->export_profile_id, int);
        if (group->export_profile_id !=
            BCM_INT_COLLECTOR_INVALID_EXPORT_PROFILE_ID) {
            BCM_IF_ERROR_RETURN(
                    _bcm_esw_collector_export_profile_ref_count_update(unit,
                        group->export_profile_id,
                        1));
        }

        if (ft_info->cfg_mode >= SHR_FT_CFG_MODE_V2) {
            bcm_collector_export_profile_t_init(&export_profile_info);
            /* Get the export profile to get the wire format */
            BCM_IF_ERROR_RETURN(
                    bcm_esw_collector_export_profile_get(unit,
                        group->export_profile_id,
                        &export_profile_info));

            if (group->collector_id != _BCM_FT_WB_INVALID_COLLECTOR_ID) {
                if (export_profile_info.wire_format ==
                    bcmCollectorWireFormatIpfix) {
                    BCM_IF_ERROR_RETURN(
                      _bcm_esw_collector_user_update(unit,
                      group->collector_id, _BCM_COLLECTOR_EXPORT_APP_FT_IPFIX));
                } else {
                    BCM_IF_ERROR_RETURN(
                    _bcm_esw_collector_user_update(unit,
                    group->collector_id,
                    _BCM_COLLECTOR_EXPORT_APP_FT_PROTOBUF));
                }
            }
        }

        /* Export triggers */
        FT_SCACHE_READ(*scache, group->export_trigger, uint32);

        /* Field group */
        for (j = 0; j < ft_info->num_pipes; ++j) {
            FT_SCACHE_READ(*scache, group->field_group[j], int);
        }

        /* Number of Tracking Params */
        FT_SCACHE_READ(*scache, group->num_tracking_params, int);

        /* Allocate memory for the Tracking Params list */
        if (group->num_tracking_params > 0) {
            _BCM_FT_ALLOC(group->tracking_params,
                          bcm_flowtracker_tracking_param_info_t,
                          (group->num_tracking_params *
                           sizeof(bcm_flowtracker_tracking_param_info_t)),
                          "Group Tracking Params list");
            if (group->tracking_params == NULL) {
                return BCM_E_MEMORY;
            }
        }

        for (j = 0; j < group->num_tracking_params; j++) {
            /* Flags for tracking parameters. */
            FT_SCACHE_READ(*scache, group->tracking_params[j].flags, uint32);

            /* Type of tracking parameter. */
            FT_SCACHE_READ(*scache, group->tracking_params[j].param, uint16);

            /* UDF Id */
            FT_SCACHE_READ(*scache, group->tracking_params[j].udf_id, int);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_collector_size_get
 * Purpose:
 *     Get the size required to sync collector data to scache v1
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th_ft_scache_v1_collector_size_get(int unit)
{
    uint32 size = 0;

    /* Collector Internal Id */
    size += sizeof(int);

    size *= BCM_INT_COLLECTOR_MAX_COLLECTORS;

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_collector_sync
 * Purpose:
 *     Sync collector data with scache v1
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th_ft_scache_v1_collector_sync(int unit, uint8 **scache)
{
    int i;
    _bcm_int_ft_v2_collector_info_t *collector_list = ft_v2_collector_info_list[unit];
    _bcm_int_ft_v2_collector_info_t *collector = NULL;

    /* Start & End Id */
    FT_SCACHE_WRITE(*scache, BCM_INT_COLLECTOR_COLLECTOR_START_ID, uint16);
    FT_SCACHE_WRITE(*scache, BCM_INT_COLLECTOR_COLLECTOR_END_ID, uint16);

    for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID;
         i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID;
         i++) {

        collector = &(collector_list[i]);

        /* Collector Id */
        FT_SCACHE_WRITE(*scache, i, int);

        if (collector->int_id == 0) {
            /* Collector exists, set in_use = 0 */
            FT_SCACHE_WRITE(*scache, 0, uint8);
            continue;
        }
        /* Collector exists, set in_use = 1 */
        FT_SCACHE_WRITE(*scache, 1, uint8);

        /* Collector Internal id */
        FT_SCACHE_WRITE(*scache, collector->int_id, int);
    }
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_collector_recover
 * Purpose:
 *     Recover Flowtracker group data from scache v1
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v1_collector_recover(int unit, uint8 **scache)
{
    int i;
    uint8 in_use;
    uint16 start_id, end_id;
    int collector_id;
    int rv = BCM_E_NONE;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_v2_collector_info_t *collector_list = ft_v2_collector_info_list[unit];
    _bcm_int_ft_v2_collector_info_t *collector = NULL;

    /* Start & end Id */
    FT_SCACHE_READ(*scache, start_id, uint16);
    FT_SCACHE_READ(*scache, end_id, uint16);
    if ((start_id != BCM_INT_COLLECTOR_COLLECTOR_START_ID) ||
        (end_id != BCM_INT_COLLECTOR_COLLECTOR_END_ID)) {
        return BCM_E_PARAM;
    }

    for (i = BCM_INT_COLLECTOR_COLLECTOR_START_ID;
         i <= BCM_INT_COLLECTOR_COLLECTOR_END_ID;
         i++) {

        collector = &(collector_list[i]);

        /* Collector Id */
        FT_SCACHE_READ(*scache, collector_id, int);
        if (collector_id != i) {
            return BCM_E_INTERNAL;
        }

        /* in_use */
        FT_SCACHE_READ(*scache, in_use, uint8);
        if (in_use == 0) {
            continue;
        }

        /* Collector Internal id */
        FT_SCACHE_READ(*scache, collector->int_id, int);

        rv = shr_idxres_list_reserve(ft_info->collector_pool,
                                     collector->int_id, collector->int_id);
        BCM_IF_ERROR_RETURN(rv);

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_export_profile_size_get
 * Purpose:
 *     Get the size required to sync export_profile data to scache v1
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th_ft_scache_v1_export_profile_size_get(int unit)
{
    uint32 size = 0;

    /* Export_Profile Internal Id */
    size += sizeof(int);

    size *= BCM_INT_COLLECTOR_MAX_EXPORT_PROFILES;

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_export_profile_sync
 * Purpose:
 *     Sync export_profile data with scache v1
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th_ft_scache_v1_export_profile_sync(int unit, uint8 **scache)
{
    int i;
    _bcm_int_ft_v2_export_profile_info_t *export_profile_list = ft_v2_export_profile_info_list[unit];
    _bcm_int_ft_v2_export_profile_info_t *export_profile = NULL;

    /* Start & End Id */
    FT_SCACHE_WRITE(*scache, BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID, uint16);
    FT_SCACHE_WRITE(*scache, BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID, uint16);

    for (i = BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID;
         i <= BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID;
         i++) {

        export_profile = &(export_profile_list[i]);

        /* Export_Profile Id */
        FT_SCACHE_WRITE(*scache, i, int);

        if (export_profile->int_id == 0) {
            /* Export_Profile exists, set in_use = 0 */
            FT_SCACHE_WRITE(*scache, 0, uint8);
            continue;
        }
        /* Export_Profile exists, set in_use = 1 */
        FT_SCACHE_WRITE(*scache, 1, uint8);

        /* Export_Profile Internal id */
        FT_SCACHE_WRITE(*scache, export_profile->int_id, int);
    }
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_export_profile_recover
 * Purpose:
 *     Recover Flowtracker group data from scache v1
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v1_export_profile_recover(int unit, uint8 **scache)
{
    int i;
    uint8 in_use;
    int rv = BCM_E_NONE;
    uint16 start_id, end_id;
    int export_profile_id;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_v2_export_profile_info_t *export_profile_list = ft_v2_export_profile_info_list[unit];
    _bcm_int_ft_v2_export_profile_info_t *export_profile = NULL;

    /* Start & end Id */
    FT_SCACHE_READ(*scache, start_id, uint16);
    FT_SCACHE_READ(*scache, end_id, uint16);
    if ((start_id != BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID) ||
        (end_id != BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID)) {
        return BCM_E_PARAM;
    }

    for (i = BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID;
         i <= BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID;
         i++) {

        export_profile = &(export_profile_list[i]);

        /* Export_Profile Id */
        FT_SCACHE_READ(*scache, export_profile_id, int);
        if (export_profile_id != i) {
            return BCM_E_INTERNAL;
        }

        /* in_use */
        FT_SCACHE_READ(*scache, in_use, uint8);
        if (in_use == 0) {
            continue;
        }

        /* Export_Profile Internal id */
        FT_SCACHE_READ(*scache, export_profile->int_id, int);

        rv = shr_idxres_list_reserve(ft_info->export_profile_pool,
                                     export_profile->int_id, export_profile->int_id);
        BCM_IF_ERROR_RETURN(rv);

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_size_get
 * Purpose:
 *     Get Flowtracker scache v1 size
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Size required
 */
STATIC uint32
_bcm_th_ft_scache_v1_size_get(int unit)
{
    uint32 size = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (ft_info->cfg_mode == SHR_FT_CFG_MODE_V1) {
        return 0;
    }

    /* Get global data size */
    size += _bcm_th_ft_scache_v1_global_size_get(unit);

    /* Group data */
    size += _bcm_th_ft_scache_v1_group_size_get(unit);

    /* Collector data */
    size += _bcm_th_ft_scache_v1_collector_size_get(unit);

    /* Export profile data */
    size += _bcm_th_ft_scache_v1_export_profile_size_get(unit);

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_sync
 * Purpose:
 *     Sync Flowtracker scache v1
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     None
 */
STATIC void
_bcm_th_ft_scache_v1_sync(int unit, uint8 **scache)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (ft_info->cfg_mode == SHR_FT_CFG_MODE_V1) {
        return;
    }

    /* Sync global data */
    _bcm_th_ft_scache_v1_global_sync(unit, scache);

    /* Sync group data */
    _bcm_th_ft_scache_v1_group_sync(unit, scache);

    /* Sync collector data */
    _bcm_th_ft_scache_v1_collector_sync(unit, scache);

    /* Sync export_profile data */
    _bcm_th_ft_scache_v1_export_profile_sync(unit, scache);
}

/*
 * Function:
 *     _bcm_th_ft_scache_v1_recover
 * Purpose:
 *     Recover Flowtracker scache v1 data
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v1_recover(int unit, uint8 **scache)
{
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (ft_info->cfg_mode == SHR_FT_CFG_MODE_V1) {
        return BCM_E_NONE;
    }

    /* Recover global data */
    BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v1_global_recover(unit, scache));

    /* Recover group data */
    BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v1_group_recover(unit, scache));

    /* Recover collector data */
    BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v1_collector_recover(unit, scache));

    /* Recover export_profile data */
    BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v1_export_profile_recover(unit, scache));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v2_template_size_get
 * Purpose:
 *     Get the size required to sync Flowtracker template data to scache v2
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th_ft_scache_v2_template_size_get(int unit)
{
    int i;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    uint32 size = 0;

    /* num_templates */
    size += sizeof(uint16);

    /* Template Id */
    size += sizeof(uint16);

    /* Set ID */
    size += sizeof(uint16);

    /* Number of export elements */
    size += sizeof(uint16);

    if (ft_info->flags & BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR) {
        for (i = 0; i < SHR_QCM_TEMPLATE_MAX_INFO_ELEMENTS; i++) {
            /* Element Id */
            size += sizeof(uint16);

            /* Data size */
            size += sizeof(uint8);

            /* Information Element Id */
            size += sizeof(uint16);

            /* Enterprise flag */
            size += sizeof(uint8);
        }
    } else {
        for (i = 0; i < SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS; i++) {
            /* Element Id */
            size += sizeof(uint16);

            /* Data size */
            size += sizeof(uint8);

            /* Information Element Id */
            size += sizeof(uint16);

            /* Enterprise flag */
            size += sizeof(uint8);
        }
    }

    /* Multiply by number of templates */
    /* Template id 1 already synched to scache v0 */
    size *= (BCM_INT_FT_MAX_TEMPLATES - 1);

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v2_template_sync
 * Purpose:
 *     Sync Flowtracker remaining template data to scache v2
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     None
 */
STATIC void
_bcm_th_ft_scache_v2_template_sync(int unit, uint8 **scache)
{
    int i, j;
    int num_templates = 0;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_export_template_info_t *template = NULL;

    /* Template id 1 already synched to scache v0 */
    if (ft_info->num_templates != 0) {
        template = &(ft_export_template_info_list[unit][BCM_INT_FT_TEMPLATE_START_ID]);
        if (template->template_id != 0) {
            num_templates = (ft_info->num_templates - 1);
        } else {
            num_templates = (ft_info->num_templates);
        }
    }

    /* Number of Templates */
    FT_SCACHE_WRITE(*scache, num_templates, uint16);

    if (num_templates == 0) {
        return;
    }

    for (i = (BCM_INT_FT_TEMPLATE_START_ID + 1); i <= BCM_INT_FT_TEMPLATE_END_ID; i++) {

        template = &(ft_export_template_info_list[unit][i]);

        if (template->template_id != 0) {
            /* Template Id */
            FT_SCACHE_WRITE(*scache, i, uint16);
        } else {
            /* Template not in use */
            continue;
        }

        /* Set ID */
        FT_SCACHE_WRITE(*scache, template->set_id, uint16);

        /* Number of export elements */
        FT_SCACHE_WRITE(*scache, template->num_export_elements, uint16);

        for (j = 0; j < template->num_export_elements; j++) {
            /* Element Id */
            FT_SCACHE_WRITE(*scache, template->elements[j].element, uint16);

            /* Data size */
            FT_SCACHE_WRITE(*scache, template->elements[j].data_size, uint16);

            /* Information Element Id */
            FT_SCACHE_WRITE(*scache, template->elements[j].id, uint16);

            /* Enterprise flag */
            FT_SCACHE_WRITE(*scache, template->elements[j].enterprise, uint8);
        }
    }
}

/*
 * Function:
 *     _bcm_th_ft_scache_v2_template_recover
 * Purpose:
 *     Recover Flowtracker template data from scache v2
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v2_template_recover(int unit, uint8 **scache)
{
    int i, j;
    int rv;
    uint16 template_id;
    uint16 num_templates;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_export_template_info_t *template = NULL;

    FT_SCACHE_READ(*scache, num_templates, uint16);

    if (num_templates == 0) {
        return BCM_E_NONE;
    }

    if ((num_templates + 1) > BCM_INT_FT_TEMPLATE_END_ID) {
        return BCM_E_INTERNAL;
    }

    for (i = 0; i < num_templates; i++) {

        /* Template Id */
        /* Template Id 1 always recovered by v0 recovery api */
        FT_SCACHE_READ(*scache, template_id, uint16);
        if ((template_id <= BCM_INT_FT_TEMPLATE_START_ID)
            || (template_id > BCM_INT_FT_TEMPLATE_END_ID)) {
            return BCM_E_INTERNAL;
        }

        template = &(ft_export_template_info_list[unit][template_id]);
        template->template_id = template_id;

        rv = shr_idxres_list_reserve(ft_info->template_pool, template_id, template_id);
        BCM_IF_ERROR_RETURN(rv);

        /* Set ID */
        FT_SCACHE_READ(*scache, template->set_id, uint16);

        /* Number of export elements */
        FT_SCACHE_READ(*scache, template->num_export_elements, uint16);

        for (j = 0; j < template->num_export_elements; j++) {
            /* Element Id */
            FT_SCACHE_READ(*scache, template->elements[j].element, uint16);
            if (template->elements[j].element >= SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS) {
                /* Unrecognized element */
                return BCM_E_INTERNAL;
            }

            /* Data size */
            FT_SCACHE_READ(*scache, template->elements[j].data_size, uint16);

            /* Information Element Id */
            FT_SCACHE_READ(*scache, template->elements[j].id, uint16);

            /* Enterprise flag */
            FT_SCACHE_READ(*scache, template->elements[j].enterprise, uint8);
        }
        ft_info->num_templates += 1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v2_template_xmit_size_get
 * Purpose:
 *     Get the size required to sync Flowtracker template xmit data to scache v2
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_th_ft_scache_v2_template_xmit_size_get(int unit)
{
    uint32 size = 0;

    /* Template ID */
    size += sizeof(uint16);

    /* Number of Collectors */
    size += sizeof(uint16);

    /* Collector Id */
    size += sizeof(uint16);

    /* Interval secs */
    size += sizeof(uint16);

    /* Initial burst */
    size += sizeof(uint16);

    /* Multiply by number of collector and templates */
    /* Template id 1 already synched to scache v0 */
    size *= (BCM_INT_FT_MAX_COLLECTORS * (BCM_INT_FT_MAX_TEMPLATES- 1));

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v2_template_xmit_sync
 * Purpose:
 *     Sync Flowtracker remaining template xmit data to scache v2
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th_ft_scache_v2_template_xmit_sync(int unit, uint8 **scache)
{
    int i;
    _bcm_int_ft_export_template_info_t *template = NULL;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);

    if (ft_info->num_templates == 0) {
        return;
    }

    /* Template id 1 already synched to scache v0 */
    if (ft_info->num_templates == 1) {
        template = &(ft_export_template_info_list[unit][BCM_INT_FT_TEMPLATE_START_ID]);
        if (template->template_id != 0) {
           return;
        }
    }

    for (i = (BCM_INT_FT_TEMPLATE_START_ID + 1); i <= BCM_INT_FT_TEMPLATE_END_ID; i++) {
        template  = &(ft_export_template_info_list[unit][i]);

        if (template->template_id == 0) {
            continue;
        }

        /* Template ID */
        FT_SCACHE_WRITE(*scache, template->template_id, uint16);

        /* Number of Collectors */
        FT_SCACHE_WRITE(*scache, 1, uint16);

        /* Collector Id */
        FT_SCACHE_WRITE(*scache, 1, uint16);

        /* Interval secs */
        FT_SCACHE_WRITE(*scache, template->interval_secs, uint16);

        /* Initial burst */
        FT_SCACHE_WRITE(*scache, template->initial_burst, uint16);
    }

}

/*
 * Function:
 *     _bcm_th_ft_scache_v2_template_xmit_recover
 * Purpose:
 *     Recover Flowtracker template xmit data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v2_template_xmit_recover(int unit, uint8 **scache)
{
    int i;
    uint16 template_id;
    uint16 num_templates;
    uint16 num_collectors;
    _bcm_int_ft_info_t *ft_info = FT_INFO_GET(unit);
    _bcm_int_ft_export_template_info_t *template = NULL;

    if (ft_info->num_templates == 0) {
        return BCM_E_NONE;
    }

    /* Template Id 1 always recovered by v0 recovery api */
    if (ft_info->num_templates != 0) {
        template = &(ft_export_template_info_list[unit][BCM_INT_FT_TEMPLATE_START_ID]);

        if (template->template_id != 0) {
            num_templates = (ft_info->num_templates - 1);
        } else {
            num_templates = (ft_info->num_templates);
        }

        if (num_templates == 0) {
            return BCM_E_NONE;
        }
    }

    for (i = 0; i < num_templates; i++) {

        /* Template ID */
        FT_SCACHE_READ(*scache, template_id, uint16);

        template  = &(ft_export_template_info_list[unit][template_id]);
        if (template_id != template->template_id) {
            return BCM_E_INTERNAL;
        }
        /* Number of Collectors */
        FT_SCACHE_READ(*scache, num_collectors, uint16);

        if (num_collectors != 1) {
            return BCM_E_INTERNAL;
        }

        /* Skip Collector Id */
        *scache += sizeof(uint16);

        /* Interval secs */
        FT_SCACHE_READ(*scache, template->interval_secs, uint16);

        /* Initial burst */
        FT_SCACHE_READ(*scache, template->initial_burst, uint16);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v2_size_get
 * Purpose:
 *     Get Flowtracker scache v2 size
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Size required
 */
STATIC uint32
_bcm_th_ft_scache_v2_size_get(int unit)
{
    uint32 size = 0;

    /* Get template data size */
    size += _bcm_th_ft_scache_v2_template_size_get(unit);

    /* Get template xmit data size */
    size += _bcm_th_ft_scache_v2_template_xmit_size_get(unit);

    return size;
}

/*
 * Function:
 *     _bcm_th_ft_scache_v2_sync
 * Purpose:
 *     Sync Flowtracker scache v2
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     None
 */
STATIC void
_bcm_th_ft_scache_v2_sync(int unit, uint8 **scache)
{
    /* Sync template data */
    _bcm_th_ft_scache_v2_template_sync(unit, scache);

    /* Sync template xmit data */
    _bcm_th_ft_scache_v2_template_xmit_sync(unit, scache);
}

/*
 * Function:
 *     _bcm_th_ft_scache_v2_recover
 * Purpose:
 *     Recover Flowtracker scache v2 data
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_ft_scache_v2_recover(int unit, uint8 **scache)
{
    /* Recover remaining template data */
    BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v2_template_recover(unit, scache));

    /* Recover remianing template xmit data */
    BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v2_template_xmit_recover(unit, scache));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_scache_alloc
 * Purpose:
 *      Flowtracker WB scache alloc
 * Parameters:
 *      unit    - (IN) Unit number.
 *      create  - (IN) 1 - Create, 0 - Realloc
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_th_ft_scache_alloc(int unit, int create)
{
    uint32                  cur_size = 0;
    uint32                  rqd_size = 0;
    int                     rv = BCM_E_NONE;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;

    rqd_size += _bcm_th_ft_scache_v0_size_get(unit);
    rqd_size += _bcm_th_ft_scache_v1_size_get(unit);
    rqd_size += _bcm_th_ft_scache_v2_size_get(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOWTRACKER, 0);

    if (create) {
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 1, rqd_size,
                                     &scache, BCM_WB_FT_DEFAULT_VERSION, NULL);
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
 *      _bcm_th_ft_sync
 * Purpose:
 *      Warmboot scache sync
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_th_ft_sync(int unit)
{
    int                     stable_size;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;
    _bcm_int_ft_info_t     *ft_info = FT_INFO_GET(unit);

    if (ft_info == NULL) {
        return BCM_E_NONE;
    }

    /* Get FT module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOWTRACKER, 0);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                                &scache, BCM_WB_FT_DEFAULT_VERSION,
                                                NULL));
    if (NULL == scache) {
        return BCM_E_INTERNAL;
    }

    /* Sync FT scache */
    _bcm_th_ft_scache_v0_sync(unit, &scache);
    _bcm_th_ft_scache_v1_sync(unit, &scache);
    _bcm_th_ft_scache_v2_sync(unit, &scache);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_ft_wb_recover
 * Purpose:
 *      Flowtracker WB recovery
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_th_ft_wb_recover(int unit)
{
    int                 stable_size;       /* Secondary storage size.   */
    uint8               *scache = NULL;
    soc_scache_handle_t scache_handle;
    int                 rv = BCM_E_NONE;
    uint16              recovered_ver = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOWTRACKER, 0);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                     &scache, BCM_WB_FT_DEFAULT_VERSION,
                                     &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have warmboot state */
            rv = _bcm_th_ft_scache_alloc(unit, 1);
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
    BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v0_recover(unit, &scache));
    if (recovered_ver >= BCM_WB_FT_VERSION_1_1) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v1_recover(unit, &scache));
    }
    if (recovered_ver >= BCM_WB_FT_VERSION_1_2) {
        BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_v2_recover(unit, &scache));
    }

    /* Realloc any extra scache that may be needed */
    BCM_IF_ERROR_RETURN(_bcm_th_ft_scache_alloc(unit, 0));

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
#else /* INCLUDE_FLOWTRACKER */
typedef int _bcm_th_ft_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_FLOWTRACKER */
