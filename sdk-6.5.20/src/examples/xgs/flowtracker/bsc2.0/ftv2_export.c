/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * C file to put FT export related api calls
 */

/* Export Template Variables */
uint8 ftv2_group_export_template_test_config = 0;
uint16 fte_set_id = 0x800a;
const int FTE_MAX_INFO_ELEMENTS = 24;
bcm_flowtracker_export_template_t template_id1 = -1;

/* Export Trigger Variables */
uint8 ftv2_group_export_trigger_test_config = 0;

/* Expected export reason */
uint16 expected_export_reason = 0;

/* Expected export flags */
uint16 expected_export_flags = 0;

/* Retreive export element and its size from provided tracking param */
void fte_export_element_maps(bcm_flowtracker_tracking_param_type_t tparam,
                             bcm_flowtracker_export_element_type_t *exp_ele,
                             uint32 *data_size)
{
    switch(tparam) {
        /* 0-4 */
        case bcmFlowtrackerTrackingParamTypeSrcIPv4:
            *exp_ele = bcmFlowtrackerExportElementTypeSrcIPv4;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeDstIPv4:
            *exp_ele = bcmFlowtrackerExportElementTypeDstIPv4;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeSrcIPv6:
            *exp_ele = bcmFlowtrackerExportElementTypeSrcIPv6;
            *data_size = 16;
            break;
        case bcmFlowtrackerTrackingParamTypeDstIPv6:
            *exp_ele = bcmFlowtrackerExportElementTypeDstIPv6;
            *data_size = 16;
            break;
        /* 5-9 */
        case bcmFlowtrackerTrackingParamTypeL4SrcPort:
            *exp_ele = bcmFlowtrackerExportElementTypeL4SrcPort;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeL4DstPort:
            *exp_ele = bcmFlowtrackerExportElementTypeL4DstPort;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeIPProtocol:
            *exp_ele = bcmFlowtrackerExportElementTypeIPProtocol;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypePktCount:
            *exp_ele = bcmFlowtrackerExportElementTypePktCount;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeByteCount:
            *exp_ele = bcmFlowtrackerExportElementTypeByteCount;
            *data_size = 4;
            break;
        /* 10-14 */
        case bcmFlowtrackerTrackingParamTypeVRF:
            *exp_ele = bcmFlowtrackerExportElementTypeVRF;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeTTL:
            *exp_ele = bcmFlowtrackerExportElementTypeTTL;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIPLength:
            *exp_ele = bcmFlowtrackerExportElementTypeIPLength;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeTcpWindowSize:
            *exp_ele = bcmFlowtrackerExportElementTypeTcpWindowSize;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeDosAttack:
            *exp_ele = bcmFlowtrackerExportElementTypeDosAttack;
            *data_size = 4;
            break;
        /* 15-19 */
        case bcmFlowtrackerTrackingParamTypeVxlanNetworkId:
            *exp_ele = bcmFlowtrackerExportElementTypeVxlanNetworkId;
            *data_size = 3;
            break;
        case bcmFlowtrackerTrackingParamTypeNextHeader:
            *exp_ele = bcmFlowtrackerExportElementTypeNextHeader;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeHopLimit:
            *exp_ele = bcmFlowtrackerExportElementTypeHopLimit;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerSrcIPv4:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerSrcIPv4;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerDstIPv4:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerDstIPv4;
            *data_size = 4;
            break;
        /* 20-24 */
        case bcmFlowtrackerTrackingParamTypeInnerSrcIPv6:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerSrcIPv6;
            *data_size = 16;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerDstIPv6:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerDstIPv6;
            *data_size = 16;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerIPProtocol:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerIPProtocol;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerTTL:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerTTL;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerNextHeader:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerNextHeader;
            *data_size = 1;
            break;
        /* 25-29 */
        case bcmFlowtrackerTrackingParamTypeInnerHopLimit:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerHopLimit;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerL4SrcPort:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerL4SrcPort;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerL4DstPort:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerL4DstPort;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeTcpFlags:
            *exp_ele = bcmFlowtrackerExportElementTypeTcpFlags;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeOuterVlanTag:
            *exp_ele = bcmFlowtrackerExportElementTypeOuterVlanTag;
            *data_size = 2;
            break;
        /* 30-34 */
        case bcmFlowtrackerTrackingParamTypeIP6Length:
            *exp_ele = bcmFlowtrackerExportElementTypeIP6Length;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerIPLength:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerIPLength;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerIP6Length:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerIP6Length;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampNewLearn:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampNewLearn;
            *data_size = 6;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampFlowStart:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampFlowStart;
            *data_size = 6;
            break;
        /* 35-39 */
        case bcmFlowtrackerTrackingParamTypeTimestampFlowEnd:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampFlowEnd;
            *data_size = 6;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampCheckEvent1;
            *data_size = 6;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampCheckEvent2;
            *data_size = 6;
            break;
        case bcmFlowtrackerTrackingParamTypeInnerDosAttack:
            *exp_ele = bcmFlowtrackerExportElementTypeInnerDosAttack;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeTunnelClass:
            *exp_ele = bcmFlowtrackerExportElementTypeTunnelClass;
            *data_size = 3;
            break;
        /* 40-44 */
        case bcmFlowtrackerTrackingParamTypeFlowtrackerCheck:
            *exp_ele = bcmFlowtrackerExportElementTypeFlowtrackerCheck;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeCustom:
            *exp_ele = bcmFlowtrackerExportElementTypeCustom;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeChipDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeChipDelay;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeE2EDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeE2EDelay;
            *data_size = 4;
            break;
        /* 45-49 */
        case bcmFlowtrackerTrackingParamTypeIPATDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeIPATDelay;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeIPDTDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeIPDTDelay;
            *data_size = 4;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPGMinCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPGMinCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPGSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPGSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPoolMinCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPoolMinCongestionLevel;
            *data_size = 1;
            break;
        /* 50-54 */
        case bcmFlowtrackerTrackingParamTypeIngPortPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPoolSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPoolSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPGHeadroomCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPGHeadroomCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPortPoolHeadroomCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPortPoolHeadroomCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngGlobalHeadroomCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeIngGlobalHeadroomCongestionLevel;
            *data_size = 1;
            break;
        /* 55-59 */
        case bcmFlowtrackerTrackingParamTypeEgrPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrPoolSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrQueueMinCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrQueueMinCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrQueueSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrQueueSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrQueueGroupMinCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrQueueGroupMinCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrQueueGroupSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrQueueGroupSharedCongestionLevel;
            *data_size = 1;
            break;
        /* 60-64 */
        case bcmFlowtrackerTrackingParamTypeEgrPortPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrPortPoolSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrRQEPoolSharedCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrRQEPoolSharedCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeCFAPCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeCFAPCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeMMUCongestionLevel:
            *exp_ele = bcmFlowtrackerExportElementTypeMMUCongestionLevel;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeMMUQueueId:
            *exp_ele = bcmFlowtrackerExportElementTypeMMUQueueId;
            *data_size = 2;
            break;
        /* 65-69 */
        case bcmFlowtrackerTrackingParamTypeECMPGroupIdLevel1:
            *exp_ele = bcmFlowtrackerExportElementTypeECMPGroupIdLevel1;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeECMPGroupIdLevel2:
            *exp_ele = bcmFlowtrackerExportElementTypeECMPGroupIdLevel2;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeTrunkMemberId:
            *exp_ele = bcmFlowtrackerExportElementTypeTrunkMemberId;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeTrunkGroupId:
            *exp_ele = bcmFlowtrackerExportElementTypeTrunkGroupId;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeIngPort:
            *exp_ele = bcmFlowtrackerExportElementTypeIngPort;
            *data_size = 1;
            break;
        /* 70-74 */
        case bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector:
            *exp_ele = bcmFlowtrackerExportElementTypeIngDropReasonGroupIdVector;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeNextHopB:
            *exp_ele = bcmFlowtrackerExportElementTypeNextHopB;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeNextHopA:
            *exp_ele = bcmFlowtrackerExportElementTypeNextHopA;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeIPPayloadLength:
            *exp_ele = bcmFlowtrackerExportElementTypeIPPayloadLength;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeQosAttr:
            *exp_ele = bcmFlowtrackerExportElementTypeQosAttr;
            *data_size = 2;
            break;
        /* 75-79 */
        case bcmFlowtrackerTrackingParamTypeEgrPort:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrPort;
            *data_size = 1;
            break;
        case bcmFlowtrackerTrackingParamTypeMMUCos:
            *exp_ele = bcmFlowtrackerExportElementTypeMMUCos;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector:
            *exp_ele = bcmFlowtrackerExportElementTypeEgrDropReasonGroupIdVector;
            *data_size = 2;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampCheckEvent3;
            *data_size = 6;
            break;
        case bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4:
            *exp_ele = bcmFlowtrackerExportElementTypeTimestampCheckEvent4;
            *data_size = 6;
            break;
        case bcmFlowtrackerTrackingParamTypeIngressDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeIngressDelay;
            *data_size = 4;
            break;
        /* 80 */
        case bcmFlowtrackerTrackingParamTypeEgressDelay:
            *exp_ele = bcmFlowtrackerExportElementTypeEgressDelay;
            *data_size = 4;
            break;
        default:
            *data_size = 0;
    }
}

char* ftv2_convert_export_element_to_string(bcm_flowtracker_export_element_type_t export_element)
{
    switch(export_element) {
        /* 0-4 */
        case bcmFlowtrackerExportElementTypeSrcIPv4:
            return "SrcIPv4";
        case bcmFlowtrackerExportElementTypeDstIPv4:
            return "DstIPv4";
        case bcmFlowtrackerExportElementTypeSrcIPv6:
            return "SrcIPv6";
        case bcmFlowtrackerExportElementTypeDstIPv6:
            return "DstIPv6";
        case bcmFlowtrackerExportElementTypeL4SrcPort:
            return "L4SrcPort";
        /* 5-9 */
        case bcmFlowtrackerExportElementTypeL4DstPort:
            return "L4DstPort";
        case bcmFlowtrackerExportElementTypeIPProtocol:
            return "IPProtocol";
        case bcmFlowtrackerExportElementTypePktCount:
            return "PktCount";
        case bcmFlowtrackerExportElementTypeByteCount:
            return "ByteCount";
        case bcmFlowtrackerExportElementTypePktDeltaCount:
            return "PktDeltaCount";
        /* 10-14 */
        case bcmFlowtrackerExportElementTypeByteDeltaCount:
            return "ByteDeltaCount";
        case bcmFlowtrackerExportElementTypeVRF:
            return "VRF";
        case bcmFlowtrackerExportElementTypeObservationTimeMsecs:
            return "ObservationTimeMsecs";
        case bcmFlowtrackerExportElementTypeFlowStartTimeMsecs:
            return "FlowStartTimeMsecs";
        case bcmFlowtrackerExportElementTypeFlowtrackerGroup:
            return "FlowtrackerGroup";
        /* 15-19 */
        case bcmFlowtrackerExportElementTypeReserved:
            return "Reserved";
        case bcmFlowtrackerExportElementTypeTTL:
            return "TTL";
        case bcmFlowtrackerExportElementTypeIPLength:
            return "IPLength";
        case bcmFlowtrackerExportElementTypeTcpWindowSize:
            return "TcpWindowSize";
        case bcmFlowtrackerExportElementTypeDosAttack:
            return "DosAttack";
        /* 20-24 */
        case bcmFlowtrackerExportElementTypeVxlanNetworkId:
            return "VxlanNetworkId";
        case bcmFlowtrackerExportElementTypeNextHeader:
            return "NextHeader";
        case bcmFlowtrackerExportElementTypeHopLimit:
            return "HopLimit";
        case bcmFlowtrackerExportElementTypeInnerSrcIPv4:
            return "InnerSrcIPv4";
        case bcmFlowtrackerExportElementTypeInnerDstIPv4:
            return "InnerDstIPv4";
        /* 25-29 */
        case bcmFlowtrackerExportElementTypeInnerSrcIPv6:
            return "InnerSrcIPv6";
        case bcmFlowtrackerExportElementTypeInnerDstIPv6:
            return "InnerDstIPv6";
        case bcmFlowtrackerExportElementTypeInnerIPProtocol:
            return "InnerIPProtocol";
        case bcmFlowtrackerExportElementTypeInnerTTL:
            return "InnerTTL";
        case bcmFlowtrackerExportElementTypeInnerNextHeader:
            return "InnerNextHeader";
        /* 30-34 */
        case bcmFlowtrackerExportElementTypeInnerHopLimit:
            return "InnerHopLimit";
        case bcmFlowtrackerExportElementTypeInnerL4SrcPort:
            return "InnerL4SrcPort";
        case bcmFlowtrackerExportElementTypeInnerL4DstPort:
            return "InnerL4DstPort";
        case bcmFlowtrackerExportElementTypeExportReasons:
            return "ExportReasons";
        case bcmFlowtrackerExportElementTypeExportFlags:
            return "ExportFlags";
        /* 35-39 */
        case bcmFlowtrackerExportElementTypeTcpFlags:
            return "TcpFlags";
        case bcmFlowtrackerExportElementTypeOuterVlanTag:
            return "OuterVlanTag";
        case bcmFlowtrackerExportElementTypeIP6Length:
            return "IP6Length";
        case bcmFlowtrackerExportElementTypeInnerIPLength:
            return "InnerIPLength";
        case bcmFlowtrackerExportElementTypeInnerIP6Length:
            return "InnerIP6Length";
        /* 40-44 */
        case bcmFlowtrackerExportElementTypeTimestampNewLearn:
            return "TimestampNewLearn";
        case bcmFlowtrackerExportElementTypeTimestampFlowStart:
            return "FlowStart";
        case bcmFlowtrackerExportElementTypeTimestampFlowEnd:
            return "FlowEnd";
        case bcmFlowtrackerExportElementTypeTimestampCheckEvent1:
            return "Timestamp Check Event1";
        case bcmFlowtrackerExportElementTypeTimestampCheckEvent2:
            return "Timestamp Check Event2";
        /* 45-49 */
        case bcmFlowtrackerExportElementTypeInnerDosAttack:
            return "InnerDosAttack";
        case bcmFlowtrackerExportElementTypeTunnelClass:
            return "TunnelClass";
        case bcmFlowtrackerExportElementTypeFlowtrackerCheck:
            return "FlowtrackerCheck";
        case bcmFlowtrackerExportElementTypeCustom:
            return "Custom";
        case bcmFlowtrackerExportElementTypeFlowViewID:
            return "FlowViewID";
        /* 50-54 */
        case bcmFlowtrackerExportElementTypeFlowViewSampleCount:
            return "FlowViewSampleCount";
        case bcmFlowtrackerExportElementTypeFlowCount:
            return "FlowCount";
        case bcmFlowtrackerExportElementTypeDropPktCount:
            return "DropPktCount";
        case bcmFlowtrackerExportElementTypeDropByteCount:
            return "DropByteCount";
        case bcmFlowtrackerExportElementTypeEgrPort:
            return "EgrPort";
        /* 55-59 */
        case bcmFlowtrackerExportElementTypeQueueID:
            return "QueueID";
        case bcmFlowtrackerExportElementTypeQueueBufferUsage:
            return "QueueBufferUsage";
        case bcmFlowtrackerExportElementTypeChipDelay:
            return "ChipDelay";
        case bcmFlowtrackerExportElementTypeE2EDelay:
            return "E2EDelay";
        case bcmFlowtrackerExportElementTypeIPATDelay:
            return "IPATDelay";
        /* 60-64 */
        case bcmFlowtrackerExportElementTypeIPDTDelay:
            return "IPDTDelay";
        case bcmFlowtrackerExportElementTypeIngPortPGMinCongestionLevel:
            return "IngPortPGMinCongestionLevel";
        case bcmFlowtrackerExportElementTypeIngPortPGSharedCongestionLevel:
            return "IngPortPGSharedCongestionLevel";
        case bcmFlowtrackerExportElementTypeIngPortPoolMinCongestionLevel:
            return "IngPortPoolMinCongestionLevel";
        case bcmFlowtrackerExportElementTypeIngPortPoolSharedCongestionLevel:
            return "IngPortPoolSharedCongestionLevel";
        /* 65-69 */
        case bcmFlowtrackerExportElementTypeIngPoolSharedCongestionLevel:
            return "IngPoolSharedCongestionLevel";
        case bcmFlowtrackerExportElementTypeIngPortPGHeadroomCongestionLevel:
            return "IngPortPGHeadroomCongestionLevel";
        case bcmFlowtrackerExportElementTypeIngPortPoolHeadroomCongestionLevel:
            return "IngPortPoolHeadroomCongestionLevel";
        case bcmFlowtrackerExportElementTypeIngGlobalHeadroomCongestionLevel:
            return "IngGlobalHeadroomCongestionLevel";
        case bcmFlowtrackerExportElementTypeEgrPoolSharedCongestionLevel:
            return "EgrPoolSharedCongestionLevel";
        /* 70-74 */
        case bcmFlowtrackerExportElementTypeEgrQueueMinCongestionLevel:
            return "EgrQueueMinCongestionLevel";
        case bcmFlowtrackerExportElementTypeEgrQueueSharedCongestionLevel:
            return "EgrQueueSharedCongestionLevel";
        case bcmFlowtrackerExportElementTypeEgrQueueGroupMinCongestionLevel:
            return "EgrQueueGroupMinCongestionLevel";
        case bcmFlowtrackerExportElementTypeEgrQueueGroupSharedCongestionLevel:
            return "EgrQueueGroupSharedCongestionLevel";
        case bcmFlowtrackerExportElementTypeEgrPortPoolSharedCongestionLevel:
            return "EgrPortPoolSharedCongestionLevel";
        /* 75-79 */
        case bcmFlowtrackerExportElementTypeEgrRQEPoolSharedCongestionLevel:
            return "EgrRQEPoolSharedCongestionLevel";
        case bcmFlowtrackerExportElementTypeCFAPCongestionLevel:
            return "CFAPCongestionLevel";
        case bcmFlowtrackerExportElementTypeMMUCongestionLevel:
            return "MMUCongestionLevel";
        case bcmFlowtrackerExportElementTypeMMUQueueId:
            return "MMU Queue Id";
        case bcmFlowtrackerExportElementTypeECMPGroupIdLevel1:
            return "ECMPGroupIdLevel1";
        /* 80-84 */
        case bcmFlowtrackerExportElementTypeECMPGroupIdLevel2:
            return "ECMPGroupIdLevel2";
        case bcmFlowtrackerExportElementTypeTrunkMemberId:
            return "TrunkMemberId";
        case bcmFlowtrackerExportElementTypeTrunkGroupId:
            return "TrunkGroupId";
        case bcmFlowtrackerExportElementTypeIngPort:
            return "IngPort";
        case bcmFlowtrackerExportElementTypeIngDropReasonGroupIdVector:
            return "IngDropReasonGroupIdVector";
        /* 85-89 */
        case bcmFlowtrackerExportElementTypeNextHopB:
            return "NextHopB";
        case bcmFlowtrackerExportElementTypeNextHopA:
            return "NextHopA";
        case bcmFlowtrackerExportElementTypeIPPayloadLength:
            return "IPPayloadLength";
        case bcmFlowtrackerExportElementTypeQosAttr:
            return "QosAttr";
        case bcmFlowtrackerExportElementTypeMMUCos:
            return "MMUCos";
        /* 90-92 */
        case bcmFlowtrackerExportElementTypeEgrDropReasonGroupIdVector:
            return "EgrDropReasonGroupIdVector";
        case bcmFlowtrackerExportElementTypeTimestampCheckEvent3:
            return "TimestampCheckEvent3";
        case bcmFlowtrackerExportElementTypeTimestampCheckEvent4:
            return "TimestampCheckEvent4";
        default:
            return "Unknown";
    }
}

int fte_tracking_param_xlate(int unit,
        bcm_flowtracker_tracking_param_type_t tparam,
        uint32 *size,
        bcm_flowtracker_export_element_type_t *exp_elem)
{
    fte_export_element_maps(tparam, exp_elem, size);

    if (*size == 0) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * d chg BSC_DG_GROUP_ALU32_PROFILE_0
 * d chg BSC_DG_GROUP_ALU32_PROFILE_1
 * d chg BSC_DG_GROUP_ALU32_PROFILE_2
 * d chg BSC_DG_GROUP_ALU32_PROFILE_3
 * d chg BSC_DG_GROUP_ALU32_PROFILE_4
 * d chg BSC_DG_GROUP_ALU32_PROFILE_5
 * d chg BSC_DG_GROUP_ALU32_PROFILE_6
 * d chg BSC_DG_GROUP_ALU32_PROFILE_7
 * d chg BSC_DG_GROUP_ALU32_PROFILE_8
 * d chg BSC_DG_GROUP_ALU32_PROFILE_9
 * d chg BSC_DG_GROUP_ALU32_PROFILE_10
 * d chg BSC_DG_GROUP_ALU32_PROFILE_11
 * d chg BSC_DG_GROUP_ALU16_PROFILE_0
 * d chg BSC_DG_GROUP_ALU16_PROFILE_1
 * d chg BSC_DG_GROUP_ALU16_PROFILE_2
 * d chg BSC_DG_GROUP_ALU16_PROFILE_3
 * d chg BSC_DG_GROUP_ALU16_PROFILE_4
 * d chg BSC_DG_GROUP_ALU16_PROFILE_5
 * d chg BSC_DG_GROUP_ALU16_PROFILE_6
 * d chg BSC_DG_GROUP_ALU16_PROFILE_7
 * d chg BSC_DG_GROUP_TIMESTAMP_PROFILE
 * d chg BSC_DG_GROUP_LOAD8_PROFILE
 * d chg BSC_DG_GROUP_LOAD16_PROFILE
 * d chg BSC_DG_GROUP_TABLE
 * d chg BSD_POLICY_ACTION_PROFILE
 * d chg BSC_KG_GROUP_TABLE
 * d chg BSC_DT_PDE_PROFILE
 */
int
ftv2_group_export_template_collector_add(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    int ix = 0, elem = 0;
    int tparam_print = 0;
    uint32 options = 0x0;
    int actual_tracking_params = 0;
    int actual_export_elements = 0;
    char *export_element_string = NULL;
    int max_export_elements = FTE_MAX_INFO_ELEMENTS;
    int max_tracking_params = max_num_tracking_params;
    bcm_flowtracker_tracking_param_info_t tracking_params[FTE_MAX_INFO_ELEMENTS];
    bcm_flowtracker_export_element_info_t export_elems_ip[FTE_MAX_INFO_ELEMENTS];
    bcm_flowtracker_export_element_info_t export_elems_op[FTE_MAX_INFO_ELEMENTS];

    /* Skip this export template create if no test config set */
    if (!ftv2_group_export_template_test_config) {
        if (!skip_log) {
            printf("[CINT] Step15 - Skip Template Validate & Create ......\n");
        }
        return rv;
    }

    /* Create FT Export Template as per test config set */
    switch(ftv2_group_export_template_test_config) {
        case 1:
            /* ftv2_group_export_template_test_config = 1 */
            /*
             * Get tracking params first. This step is done to get
             * export elements from tracking params automatically
             */
            rv = bcm_flowtracker_group_tracking_params_get(unit,
                                                           flow_group_id1,
                                                           max_tracking_params,
                                                           tracking_params,
                                                          &actual_tracking_params);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_group_tracking_params_get => rv %d(%s)\n",
                                                                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            /* Initialise export elements ip/op structure */
            for (ix = 0; ix < FTE_MAX_INFO_ELEMENTS; ix++) {
                export_elems_ip[ix].data_size = 0;
                export_elems_ip[ix].element = bcmFlowtrackerExportElementTypeReserved;
                export_elems_op[ix].data_size = 0;
                export_elems_op[ix].element = bcmFlowtrackerExportElementTypeReserved;
            }

            /* Covert tracking params into export elements */
            for (ix = 0; ix < actual_tracking_params; ix++) {
                rv = fte_tracking_param_xlate(unit,
                                              tracking_params[ix].param,
                                            &(export_elems_ip[ix].data_size),
                                            &(export_elems_ip[ix].element));

                if (BCM_FAILURE(rv)) {
                    tparam_print = tracking_params[ix].param;
                    printf("[CINT] TC %2d  - translating tracking param %d to export element
                            failed with rv %d(%s)\n",tc_id, tparam_print, rv, bcm_errmsg(rv));
                    return rv;
                }
                if (tracking_params[ix].param == bcmFlowtrackerTrackingParamTypeFlowtrackerCheck) {
                    export_elems_ip[ix].check_id = tracking_params[ix].check_id;
                }
            }

            /* Add FlowGroup to info elements */
            export_elems_ip[ix].element = bcmFlowtrackerExportElementTypeFlowtrackerGroup;
            export_elems_ip[ix].data_size = 2;
            actual_tracking_params++;

            if (!skip_log) {
                printf("*************************************************************************************************\n");
                printf("[CINT] Input information elements (total %d): \n", actual_tracking_params);
                for (ix = 0; ix < actual_tracking_params; ix++) {
                    elem = (export_elems_ip[ix].element);
                    export_element_string = ftv2_convert_export_element_to_string(elem);
                    printf("\tinput info element (%2d): elem = %3d(%35s) and size = %3d\n", ix, elem,
                            export_element_string, export_elems_ip[ix].data_size);
                }
                printf("*************************************************************************************************\n");
            }

            /* Validate export template once */
            rv = bcm_flowtracker_export_template_validate(unit,
                                                          flow_group_id1,
                                                          actual_tracking_params,
                                                          export_elems_ip,
                                                          max_export_elements,
                                                          export_elems_op,
                                                         &actual_export_elements);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_export_template_validate => rv %d(%s)\n",
                                                                       tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            if (!skip_log) {
                printf("*************************************************************************************************\n");
                printf("[CINT] Output information elements (total %d): \n", actual_export_elements);
                for (ix = 0; ix < actual_export_elements; ix++) {
                    elem = (export_elems_op[ix].element);
                    export_element_string = ftv2_convert_export_element_to_string(elem);
                    printf("\toutput info element (%2d): elem = %3d(%35s) and size = %3d\n", ix, elem,
                            export_element_string, export_elems_op[ix].data_size);
                }
                printf("*************************************************************************************************\n");
            }

            /* Create Template First */
            rv = bcm_flowtracker_export_template_create(unit, options,
                                                       &template_id1,
                                                        fte_set_id,
                                                        actual_export_elements,
                                                        export_elems_op);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_export_template_create => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }

            /* Attach template and collector to FT Group */
            rv = bcm_flowtracker_group_collector_attach(unit, flow_group_id1, collector_id1, export_profile_id1, template_id1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_group_collector_attach => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            if (!skip_log) {
                printf("[CINT] Step15 - Export Template Created with id %d, Validated and Added to flow tracker group 0x%x\n",
                                                                                                  template_id1,flow_group_id1);
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid export template test config %d\n",tc_id,ftv2_group_export_template_test_config);
            return BCM_E_FAIL;
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step15 - Export Template Created Succesfully.\n");
    }

    return BCM_E_NONE;
}

/*
 * Remove Collector & Template from group.
 * Delete Template Created
 */
int
ftv2_group_export_template_collector_destroy(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    /* Skip export template destroy if no test config set */
    if (!ftv2_group_export_template_test_config) {
        if (!skip_log) {
            printf("[CINT] Cleanup - Skip Destroy Export Template ......\n");
        }
        return rv;
    }

    /* Destroy Export template as per test config set */
    if (template_id1  != -1) {

        rv = bcm_flowtracker_export_template_destroy(unit, template_id1);
        if (BCM_FAILURE(rv)) {
            printf("[CINT] TC %2d - bcm_flowtracker_export_template_destroy => rv %d(%s)\n",
                                                                  tc_id, rv, bcm_errmsg(rv));
            return rv;
        }
        if (!skip_log) {
            printf("[CINT] Cleanup - Destroy export template id: %d\n",template_id1);
        }
        template_id1 = -1;
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Cleanup - Deattach collector & template from group and export template deleted successfully.\n");
    }

    return BCM_E_NONE;
}

/*
 * Create export triggers as per ftv2_group_export_trigger_test_config
 * BSC_KG_GROUP_TABLEm - EXPORT_LEARNf
 */
int
ftv2_export_trigger_main(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_export_trigger_info_t trig_info1;

    /* Skip this trigger set if no test config set */
    if (!ftv2_group_export_trigger_test_config) {
        if (!skip_log) {
            printf("[CINT] Step16 - Skip Export Trigger Set ......\n");
        }
        return rv;
    }

    /* Set export trigger as per test config set */
    switch(ftv2_group_export_trigger_test_config) {
        case 1:
            /* ftv2_group_export_trigger_test_config = 1 */
            /* Set Export Trigger as new learn */
            BCM_FLOWTRACKER_TRIGGER_SET(trig_info1, bcmFlowtrackerExportTriggerNewLearn);
            rv = bcm_flowtracker_group_export_trigger_set(unit, flow_group_id1, &trig_info1);
            if (BCM_FAILURE(rv)) {
                printf("[CINT] TC %2d - bcm_flowtracker_group_export_trigger_set => rv %d(%s)\n",
                        tc_id, rv, bcm_errmsg(rv));
                return rv;
            }
            /* Set Expected export reason as New Learn */
            expected_export_reason |= 0x0008;

            if (!skip_log) {
                printf("[CINT] Step16 - Set Export Trigger to NewLearn.\n");
            }
            break;

        default:
            printf("[CINT] TC %2d - invalid export trigger test config %d\n",tc_id, ftv2_group_export_trigger_test_config);
            return BCM_E_FAIL;
    }

    /* Dump Memory & Registers */
    if (dump_mem_reg) {
        bshell(unit, "d chg BSC_KG_GROUP_TABLE");
    }

    /* Final Step Log */
    if (!skip_log) {
        printf ("[CINT] Step16 - Export Trigger Set Succesfully.\n");
    }

    return BCM_E_NONE;
}
