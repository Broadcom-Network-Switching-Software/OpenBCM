/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/cpri.h>
#define MAX_NUM_OF_DATA_AXCS    64
#ifdef CPRIMOD_SUPPORT
#include <soc/cprimod/cprimod.h>
#endif /* CPRIMOD_SUPPORT */
#include <bcm/error.h>
#include <bcm/debug.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/iproc.h>
#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>

extern int
bcmi_esw_portctrl_detach(int unit, pbmp_t pbmp, pbmp_t *detached);
int _bcm_esw_port_gport_validate(int unit, bcm_port_t port_in,
                                 bcm_port_t *port_out);

#endif /* PORTMOD_SUPPORT */


#ifdef CPRIMOD_SUPPORT
static bcm_cpri_port_speed_t
_int_to_bcm_cpri_port_speed_t (
    int speed_i)
{
    bcm_cpri_port_speed_t speed;

    switch(speed_i) {
    case 1228:
        speed = BCM_CPRI_PORT_SPEED_1228P8;
        break;
    case 2457:
        speed = BCM_CPRI_PORT_SPEED_2457P6;
        break;
    case 3072:
        speed = BCM_CPRI_PORT_SPEED_3072P0;
        break;
    case 4915:
        speed = BCM_CPRI_PORT_SPEED_4915P2;
        break;
    case 6144:
        speed = BCM_CPRI_PORT_SPEED_6144P0;
        break;
    case 9830:
        speed = BCM_CPRI_PORT_SPEED_9830P4;
        break;
    case 10137:
        speed = BCM_CPRI_PORT_SPEED_10137P6;
        break;
    case 12165:
        speed = BCM_CPRI_PORT_SPEED_12165P12;
        break;
    case 24330:
        speed = BCM_CPRI_PORT_SPEED_24330P24;
        break;
    default:
        speed = BCM_CPRI_PORT_SPEED_1228P8;
        break;
    }

    return speed;
}

static int
_bcm_cpri_port_speed_t_to_int (
    bcm_cpri_port_speed_t speed)
{
    int speed_i;

    switch(speed) {
    case BCM_CPRI_PORT_SPEED_1228P8:
        speed_i = 1228;
        break;
    case BCM_CPRI_PORT_SPEED_2457P6:
        speed_i = 2457;
        break;
    case BCM_CPRI_PORT_SPEED_3072P0:
        speed_i = 3072;
        break;
    case BCM_CPRI_PORT_SPEED_4915P2:
        speed_i = 4915;
        break;
    case BCM_CPRI_PORT_SPEED_6144P0:
        speed_i = 6144;
        break;
    case BCM_CPRI_PORT_SPEED_9830P4:
        speed_i = 9830;
        break;
    case BCM_CPRI_PORT_SPEED_10137P6:
        speed_i = 10137;
        break;
    case BCM_CPRI_PORT_SPEED_12165P12:
        speed_i = 12165;
        break;
    case BCM_CPRI_PORT_SPEED_24330P24:
        speed_i = 24330;
        break;
    default:
        speed_i = 1228;
        break;
    }

    return speed_i;
}


static int
_portmod_cpri_axc_frame_info_t_to_bcm_cpri_axc_frame_info_t(
    bcm_cpri_dir_t direction,
    portmod_cpri_axc_frame_info_t *axc_info_t,
    bcm_cpri_axc_frame_info_t *axc_info)
{
    uint32 adjusted_sample_size = 0;
    axc_info->start_bit = axc_info_t->start_bit;
    axc_info->num_bits  = axc_info_t->num_bits;
    axc_info->map_method  = axc_info_t->container_info.map_method;
    if ((axc_info_t->container_info.map_method == cprimod_cpri_frame_map_method_1) || (axc_info_t->container_info.map_method == cprimod_cpri_frame_map_method_3)) {
        axc_info->stuffing_count = axc_info_t->container_info.Nst;
    }

    axc_info->K = axc_info_t->container_info.K;
    axc_info->sample_size_M = axc_info_t->encap_data_info.sample_size;
    axc_info->Nv = axc_info_t->container_info.Nv;
    axc_info->Na = axc_info_t->container_info.Na;
    axc_info->Nc = axc_info_t->container_info.Nc;
    axc_info->truncation_enable = axc_info_t->encap_data_info.truncation_enable;
    switch (axc_info_t->container_info.frame_sync_mode) {
        case CPRIMOD_CPRI_FRAME_SYNC_MODE_HYPER:
            axc_info->frame_sync_mode = BCM_CPRI_FRAME_SYNC_MODE_HYPER;
            break;
        case CPRIMOD_CPRI_FRAME_SYNC_MODE_RADIO:
            axc_info->frame_sync_mode = BCM_CPRI_FRAME_SYNC_MODE_RADIO;
            break;
        case CPRIMOD_CPRI_FRAME_SYNC_MODE_BASIC:
            axc_info->frame_sync_mode = BCM_CPRI_FRAME_SYNC_MODE_BASIC;
            break;
        default:
            axc_info->frame_sync_mode = BCM_CPRI_FRAME_SYNC_MODE_BASIC;
            break;
    }
    axc_info->basic_frame_offset = axc_info_t->container_info.basic_frame_offset;
    axc_info->radio_frame_offset = axc_info_t->container_info.radio_frame_offset;
    axc_info->hyper_frame_offset = axc_info_t->container_info.hyper_frame_offset;
    switch (axc_info_t->encap_data_info.truncation_type) {
    case cprimod_truncation_type_15_to_16:
        axc_info->truncation_mode = bcmCpriFrameTruncMode15_to_16;
        break;
    case cprimod_truncation_type_add_0:
        axc_info->truncation_mode = bcmCpriFrameTruncModeAdd_0;
        break;
    case cprimod_truncation_type_16_to_15:
        axc_info->truncation_mode = bcmCpriFrameTruncMode16_to_15;
        break;
    case cprimod_truncation_type_add_1:
        axc_info->truncation_mode = bcmCpriFrameTruncModeAdd_1;
        break;
    default:
        axc_info->truncation_mode = bcmCpriFrameTruncMode15_to_16;
        break;
    }
    switch (axc_info_t->encap_data_info.compression_type) {
    case cprimod_compress:
        axc_info->compress_mode = bcmCpriFrameCompressModeCompress;
        break;
    case cprimod_decompress:
        axc_info->compress_mode = bcmCpriFrameCompressModeDecompress;
        break;
    case cprimod_no_compress:
        axc_info->compress_mode = bcmCpriFrameCompressModeNone;
        break;
    default:
        axc_info->compress_mode = bcmCpriFrameCompressModeCompress;
        break;
    }

    /*
     *  Sample size need to be adjusted based on combination of compression/decompression/
     *  truncation modes in different setting.
     *  If truncation type 15_to_16 or  add_0 is set, there is nothing else to check,
     *  sample size can be 16.
     *  If compression is ON, the sample size can be set to 9.  Those are two extreme cases.
     *  If any of the decompression, 16_to_15 or add_1, set the sample size to 15.
     *  Otherwise sample size is unchanged.
     */

    if ((axc_info_t->encap_data_info.truncation_type == cprimod_truncation_type_15_to_16) ||
        (axc_info_t->encap_data_info.truncation_type == cprimod_truncation_type_add_0)) {
        adjusted_sample_size = 16;
    } else if (axc_info_t->encap_data_info.compression_type == cprimod_compress) {
        adjusted_sample_size = 9;
    } else if ((axc_info_t->encap_data_info.truncation_type == cprimod_truncation_type_16_to_15) ||
        (axc_info_t->encap_data_info.truncation_type == cprimod_truncation_type_add_1) ||
        (axc_info_t->encap_data_info.compression_type == cprimod_decompress)) {
        adjusted_sample_size = 15;
    } else {
        adjusted_sample_size = axc_info_t->encap_data_info.sample_size;
    }

    if (direction == BCM_CPRI_DIR_TX) {
        axc_info->num_samples_per_roe_pkt = axc_info_t->roe_payload_info.packet_size/(2*adjusted_sample_size);
    } else {
        axc_info->num_samples_per_roe_pkt = axc_info_t->roe_payload_info.packet_size/(2*axc_info_t->encap_data_info.sample_size);
    }

    axc_info->priority = axc_info_t->encap_data_info.priority;
    axc_info->rx_queue_size = axc_info_t->encap_data_info.buffer_size;
    axc_info->cycle_size = axc_info_t->encap_data_info.cycle_size;

    return BCM_E_NONE;
}

static int
_bcm_cpri_axc_frame_info_t_to_portmod_cpri_axc_frame_info_t(
    int axc_id,
    bcm_cpri_dir_t direction,
    bcm_cpri_axc_frame_info_t *axc_info,
    portmod_cpri_axc_frame_info_t *axc_info_t)
{
    uint32 adjusted_sample_size=0;

    axc_info_t->start_bit = axc_info->start_bit;
    axc_info_t->num_bits  = axc_info->num_bits;
    axc_info_t->container_info.map_method  = axc_info->map_method;
    axc_info_t->container_info.Nst = axc_info->stuffing_count;
    axc_info_t->container_info.K = axc_info->K;
    axc_info_t->encap_data_info.sample_size = axc_info->sample_size_M;
    if (axc_info->map_method == 0) {
        axc_info_t->container_info.Naxc = axc_info->num_bits;
    } else {
        axc_info_t->container_info.Naxc = 2*axc_info->sample_size_M;
    }
    axc_info_t->container_info.Nv = axc_info->Nv;
    axc_info_t->container_info.Na = axc_info->Na;
    axc_info_t->container_info.Nc = axc_info->Nc;
    switch (axc_info->frame_sync_mode) {

    case BCM_CPRI_FRAME_SYNC_MODE_HYPER:
        axc_info_t->container_info.frame_sync_mode = CPRIMOD_CPRI_FRAME_SYNC_MODE_HYPER;
        break;
    case BCM_CPRI_FRAME_SYNC_MODE_RADIO:
        axc_info_t->container_info.frame_sync_mode = CPRIMOD_CPRI_FRAME_SYNC_MODE_RADIO;
        break;
    case BCM_CPRI_FRAME_SYNC_MODE_BASIC:
        axc_info_t->container_info.frame_sync_mode = CPRIMOD_CPRI_FRAME_SYNC_MODE_BASIC;
        break;
    default:
        axc_info_t->container_info.frame_sync_mode = CPRIMOD_CPRI_FRAME_SYNC_MODE_BASIC;
        break;
    }
    axc_info_t->container_info.basic_frame_offset = axc_info->basic_frame_offset;
    axc_info_t->container_info.radio_frame_offset = axc_info->radio_frame_offset;
    axc_info_t->container_info.hyper_frame_offset = axc_info->hyper_frame_offset;

    if (direction == BCM_CPRI_DIR_TX) {
        /*
         * Adjust the payload size based on the compression/decompression and
         * truncation combinations for transmit.
         */

        adjusted_sample_size = axc_info->sample_size_M;

        if ((axc_info->truncation_enable == 1) &&
            ((axc_info->truncation_mode == bcmCpriFrameTruncMode16_to_15)||
             (axc_info->truncation_mode == bcmCpriFrameTruncModeAdd_1))) {
            if (adjusted_sample_size == 16) {
                adjusted_sample_size = 15;
            } else {
                LOG_ERROR(BSL_LS_BCM_CPRI,
                    ("Truncation can only apply to 16 bit sample.. Sample is %d\n",
                      adjusted_sample_size));
                return BCM_E_CONFIG;
            }
        }

        if (axc_info->compress_mode == bcmCpriFrameCompressModeCompress) {
            if (adjusted_sample_size == 15) {
                adjusted_sample_size = 9;
            } else {
                LOG_ERROR(BSL_LS_BCM_CPRI,
                    ("Compression can only be done on 15 bit samples. Sample is %d\n",
                      adjusted_sample_size));
                return BCM_E_CONFIG;
            }
        } else if (axc_info->compress_mode == bcmCpriFrameCompressModeDecompress) {
            if (adjusted_sample_size == 9) {
                adjusted_sample_size = 15;
            } else {
                LOG_ERROR(BSL_LS_BCM_CPRI,
                    ("DeCompression can only be done on 9 bit samples. Sample is %d\n",
                      adjusted_sample_size));
                return BCM_E_CONFIG;
            }
        }

        if ((axc_info->truncation_enable == 1) &&
            ((axc_info->truncation_mode == bcmCpriFrameTruncMode15_to_16)||
             (axc_info->truncation_mode == bcmCpriFrameTruncModeAdd_0))) {
            if (adjusted_sample_size == 15) {
                adjusted_sample_size = 16;
            } else {
                LOG_ERROR(BSL_LS_BCM_CPRI,
                    ("Truncation can only apply to 15 bit sample.. Sample is %d\n",
                      adjusted_sample_size));
                return BCM_E_CONFIG;
            }
        }

        axc_info_t->roe_payload_info.packet_size = axc_info->num_samples_per_roe_pkt * (2*adjusted_sample_size);
        axc_info_t->roe_payload_info.last_packet_num = 0;
        axc_info_t->roe_payload_info.last_packet_size = axc_info->num_samples_per_roe_pkt * (2*adjusted_sample_size);

    } else { /* BCM_CPRI_DIR_RX */
        axc_info_t->roe_payload_info.packet_size = axc_info->num_samples_per_roe_pkt * (2*axc_info->sample_size_M);
        axc_info_t->roe_payload_info.last_packet_num = 0;
        axc_info_t->roe_payload_info.last_packet_size = axc_info->num_samples_per_roe_pkt * (2*axc_info->sample_size_M);
    }

    axc_info_t->encap_data_info.truncation_enable = axc_info->truncation_enable;
    switch (axc_info->truncation_mode) {
    case bcmCpriFrameTruncMode15_to_16:
        axc_info_t->encap_data_info.truncation_type = cprimod_truncation_type_15_to_16;
        break;
    case bcmCpriFrameTruncModeAdd_0:
        axc_info_t->encap_data_info.truncation_type = cprimod_truncation_type_add_0;
        break;
    case bcmCpriFrameTruncMode16_to_15:
        axc_info_t->encap_data_info.truncation_type = cprimod_truncation_type_16_to_15;
        break;
    case bcmCpriFrameTruncModeAdd_1:
        axc_info_t->encap_data_info.truncation_type = cprimod_truncation_type_add_1;
        break;
    default:
        axc_info_t->encap_data_info.truncation_type = cprimod_truncation_type_15_to_16;
        break;
    }
    switch (axc_info->compress_mode) {
    case bcmCpriFrameCompressModeCompress:
        axc_info_t->encap_data_info.compression_type = cprimod_compress;
        break;
    case bcmCpriFrameCompressModeDecompress:
        axc_info_t->encap_data_info.compression_type = cprimod_decompress;
        break;
    case bcmCpriFrameCompressModeNone:
        axc_info_t->encap_data_info.compression_type = cprimod_no_compress;
        break;
    default:
        axc_info_t->encap_data_info.compression_type = cprimod_compress;
        break;
    }
    if (axc_id < 64) {
        axc_info_t->encap_data_info.mux_enable = 1;
    } else {
        axc_info_t->encap_data_info.mux_enable = 0;
    }
    axc_info_t->encap_data_info.bit_reversal = 0;
    axc_info_t->encap_data_info.priority = axc_info->priority;
    axc_info_t->encap_data_info.buffer_size = axc_info->rx_queue_size;
    axc_info_t->encap_data_info.cycle_size = axc_info->cycle_size;
    axc_info_t->encap_data_info.is_agnostic = 0;

    return BCM_E_NONE;
}
#endif

int
bcm_esw_cpri_detach(
    int unit)
{
#ifdef CPRIMOD_SUPPORT
    pbmp_t detached;
    soc_info_t *si = &SOC_INFO(unit);
    BCM_PBMP_CLEAR(detached);
    BCM_IF_ERROR_RETURN(bcmi_esw_portctrl_detach(unit, si->roe.bitmap, &detached));
    BCM_PBMP_CLEAR(si->cpri.bitmap);
    BCM_PBMP_CLEAR(si->rsvd4.bitmap);
    BCM_PBMP_CLEAR(si->roe.bitmap);
    return SOC_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_init(
    int unit)
{
#ifdef CPRIMOD_SUPPORT
    int port;
    PBMP_ITER(PBMP_ROE_ALL(unit), port) {
        SOC_IF_ERROR_RETURN(portmod_cpri_init(unit, port));
    }
    return SOC_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef CPRIMOD_SUPPORT
static
int _bcm_bcm_to_cprimod_ethertype(bcm_cpri_ethertype_t bcm_type, cprimod_ethertype_t *cprimod_type)
{
    switch (bcm_type) {
        case bcmCpriEthertypeRoe:
            *cprimod_type = cprimodEthertypeRoe;
            break;

        case bcmCpriEthertypeFast:
            *cprimod_type = cprimodEthertypeFast;
            break;

        case bcmCpriEthertypeVlan:
            *cprimod_type = cprimodEthertypeVlan;
            break;

        case bcmCpriEthertypeQinQ:
            *cprimod_type = cprimodEthertypeQinQ;
            break;

        default:
            break;
    }
    return SOC_E_NONE;
}
#endif

int
bcm_esw_cpri_port_decap_ethertype_get(
    int unit,
    bcm_gport_t port,
    bcm_cpri_ethertype_t type,
    uint16 *tpid)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_ethertype_t int_type=0;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    _bcm_bcm_to_cprimod_ethertype(type, &int_type);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_ethertype_config_get(unit, port, int_type, tpid));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_ethertype_set(
    int unit,
    bcm_gport_t port,
    bcm_cpri_ethertype_t type,
    uint16 tpid)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_ethertype_t int_type=0;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    _bcm_bcm_to_cprimod_ethertype(type, &int_type);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_ethertype_config_set(unit, port, int_type, tpid));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_flow_config_get(
    int unit,
    bcm_gport_t port,
    uint8 roe_subtype,
    bcm_cpri_decap_flow_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_cls_option_t option;
    cprimod_cls_flow_type_t flow_type;
    uint32 queue_num;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_flow_config_get(unit, port, roe_subtype, &queue_num, &option, &flow_type));

    switch (option) {
        case cprimodClsOptionUseQueueNum:
            config->lookup_option = bcmCpriDecapFlowLookupQueueNum;
            break;
        case cprimodClsOptionUseFlowIdToQueue:
            config->lookup_option = bcmCpriDecapFlowLookupFlowID;
            break;
        case cprimodClsOptionUseOpcodeToQueue:
            config->lookup_option = bcmCpriDecapFlowLookupOpcode;
            break;
        default:
            config->lookup_option = bcmCpriDecapFlowLookupQueueNum;
        break;
    }

    switch (flow_type) {
        case cprimodClsFlowTypeData:
            config->flow_type = bcmCpriDecapFlowTypeIQData;
            break;
        case cprimodClsFlowTypeDataWithExt:
            config->flow_type = bcmCpriDecapFlowTypeIQDataWithTimestamp;
            break;
        case cprimodClsFlowTypeCtrlWithOpcode:
            config->flow_type = bcmCpriDecapFlowTypeControlWithOpcode;
            break;
        case cprimodClsFlowTypeCtrl:
            config->flow_type = bcmCpriDecapFlowTypeControl;
            break;
        default:
            config->flow_type = bcmCpriDecapFlowTypeIQData;
            break;
    }
    config->queue_num = queue_num;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_flow_config_set(
    int unit,
    bcm_gport_t port,
    uint8 roe_subtype,
    bcm_cpri_decap_flow_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_cls_option_t option;
    cprimod_cls_flow_type_t flow_type;
    uint32 queue_num;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (config->lookup_option) {
        case bcmCpriDecapFlowLookupQueueNum:
            option = cprimodClsOptionUseQueueNum;
            break;
        case bcmCpriDecapFlowLookupFlowID:
            option = cprimodClsOptionUseFlowIdToQueue;
            break;
        case bcmCpriDecapFlowLookupOpcode:
            option = cprimodClsOptionUseOpcodeToQueue;
            break;
        default:
            option = cprimodClsOptionUseQueueNum;
            break;
    }

    switch (config->flow_type) {
        case bcmCpriDecapFlowTypeIQData:
            flow_type = cprimodClsFlowTypeData;
            break;
        case bcmCpriDecapFlowTypeIQDataWithTimestamp:
            flow_type = cprimodClsFlowTypeDataWithExt;
            break;
        case bcmCpriDecapFlowTypeControlWithOpcode:
            flow_type = cprimodClsFlowTypeCtrlWithOpcode;
            break;
        case bcmCpriDecapFlowTypeControl:
            flow_type = cprimodClsFlowTypeCtrl;
            break;
        default:
            flow_type = cprimodClsFlowTypeData;
            break;

    }
    queue_num =  config->queue_num;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_flow_config_set(unit, port, roe_subtype, queue_num, option, flow_type));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_flow_config_clear(
    int unit,
    bcm_gport_t port,
    uint8 roe_subtype)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_flow_config_clear(unit, port, roe_subtype));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}
int
bcm_esw_cpri_port_decap_flow_to_queue_mapping_set(
    int unit,
    bcm_gport_t port,
    uint32 flow_id,
    uint32 queue_num)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_flow_to_queue_mapping_set(unit, port, flow_id, queue_num));
    return SOC_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_flow_to_queue_mapping_get(
    int unit,
    bcm_gport_t port,
    uint32 flow_id,
    uint32 *queue_num)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_flow_to_queue_mapping_get(unit, port, flow_id, queue_num));
    return SOC_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_decap_flow_to_queue_mapping_clear(
    int unit,
    bcm_gport_t port,
    uint32 flow_id)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_flow_to_queue_mapping_clear(unit, port, flow_id));
    return SOC_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}


int
bcm_esw_cpri_port_decap_queue_to_ordering_info_index_set(
    int unit,
    bcm_gport_t port,
    uint32 queue_num,
    uint32 ordering_info_index)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_queue_to_ordering_info_index_set(unit, port, queue_num, ordering_info_index));

    return SOC_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_queue_to_ordering_info_index_get(
    int unit,
    bcm_gport_t port,
    uint32 queue_num,
    uint32 *ordering_info_index)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_queue_to_ordering_info_index_get(unit, port, queue_num, ordering_info_index));

    return SOC_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef CPRIMOD_SUPPORT
static
int _bcm_bcm_to_cprimod_ordering_info_type (bcm_cpri_roe_info_type_t bcm_type, cprimod_ordering_info_type_t *cprimod_type)
{
    switch (bcm_type) {
        case bcmCpriRoEInfoTypeUseSeq:
            *cprimod_type = cprimod_ordering_info_type_roe_sequence;
            break;

        case bcmCpriRoEInfoTypeBfnForQcnt:
            *cprimod_type = cprimod_ordering_info_type_bfn_for_qcnt;
            break;

        case bcmCpriRoEInfoTypePInfo:
            *cprimod_type = cprimod_ordering_info_type_use_pinfo;
            break;
        default:
            break;
    }
    return SOC_E_NONE;
}


static
int _bcm_cprimod_to_bcm_ordering_info_type (cprimod_ordering_info_type_t cprimod_type, bcm_cpri_roe_info_type_t *bcm_type)
{
    switch (cprimod_type) {
        case cprimod_ordering_info_type_roe_sequence:
            *bcm_type = bcmCpriRoEInfoTypeUseSeq;
            break;

        case cprimod_ordering_info_type_bfn_for_qcnt:
            *bcm_type = bcmCpriRoEInfoTypeBfnForQcnt;
            break;

        case cprimod_ordering_info_type_use_pinfo:
            *bcm_type = bcmCpriRoEInfoTypePInfo;
            break;
        default:
            break;
    }
    return SOC_E_NONE;
}

static
int _is_power_of_two (uint32 number, uint32* two_power )
{
    uint32 check_val=0x1;
    int loop;

    if (number == 0) {
        return FALSE;
    } else {
        for (loop=0; loop<32 ; loop++) {
            if (number == check_val) {
                *two_power = loop;
                return TRUE;
            } else {
                if (((number & check_val) == check_val) &&
                    ((number & ~check_val)!= 0)) {
                    return FALSE;
                } else {
                    check_val <<= 1;
                }
            }
        }
        return FALSE;
    }
}

static
int _convert_fraction_to_binary_coded_decimal ( uint32 denominator, uint32 numerator,  uint32 num_bits, uint32* bcd_val )
{
    uint32 remainder = 0;
    int loop;

    if (numerator >= denominator) {
        LOG_ERROR(BSL_LS_BCM_CPRI, (BSL_META("Only Support Fraction.\n")));
        return SOC_E_PARAM;
    }

    *bcd_val    = 0;
    remainder   = numerator;

    for (loop=0; (loop < num_bits) && (remainder != 0) ; loop++) {
        numerator = remainder * 2;
        if( denominator  >  numerator ){
            remainder = numerator;
        } else {
            *bcd_val |=  0x01 << (num_bits-loop-1);
            remainder = numerator - denominator;
        }
    }
    return SOC_E_NONE;
}
#endif

int
bcm_esw_cpri_port_decap_roe_ordering_info_get(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_cpri_decap_roe_oi_t *ordering_info)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_decap_ordering_info_entry_t entry;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_decap_ordering_info_entry_t_init(&entry);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_roe_ordering_info_get(unit, port, index, &entry));

    _bcm_cprimod_to_bcm_ordering_info_type(entry.type, &ordering_info->info_type);

    ordering_info->p_ext                = entry.pcnt_extended;
    ordering_info->pcnt_pkt_count       = entry.pcnt_pkt_count;
    ordering_info->pcnt_size            = entry.pcnt_size;
    ordering_info->pcnt_increment       = entry.pcnt_increment;
    ordering_info->pcnt_max             = entry.pcnt_max;
    ordering_info->qcnt_size            = entry.qcnt_size;
    ordering_info->qcnt_increment       = entry.qcnt_increment;
    ordering_info->qcnt_max             = entry.qcnt_max;
    ordering_info->modulo_2             = entry.modulo_2;
    ordering_info->gsm_tsn_bitmap       = entry.gsm_tsn_bitmap;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_roe_ordering_info_set(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_cpri_decap_roe_oi_t *ordering_info)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_decap_ordering_info_entry_t entry;
    uint32 bias = 0;
    uint32 two_power = 0;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_decap_ordering_info_entry_t_init(&entry);

    _bcm_bcm_to_cprimod_ordering_info_type(ordering_info->info_type, &entry.type);
    entry.pcnt_extended         = ordering_info->p_ext;
    entry.pcnt_pkt_count        = ordering_info->pcnt_pkt_count;
    entry.pcnt_size             = ordering_info->pcnt_size;
    entry.pcnt_increment        = ordering_info->pcnt_increment;
    entry.pcnt_max              = ordering_info->pcnt_max;

    entry.pcnt_increment_p2     = _is_power_of_two(ordering_info->pcnt_increment, &two_power);
    if (entry.pcnt_increment_p2) {
        entry.pcnt_bias         = two_power;
    } else {
        _convert_fraction_to_binary_coded_decimal(ordering_info->pcnt_increment, 1, ordering_info->pcnt_size, &bias);
        entry.pcnt_bias         = bias;
    }

    entry.qcnt_size             = ordering_info->qcnt_size;
    entry.qcnt_increment        = ordering_info->qcnt_increment;
    entry.qcnt_max              = ordering_info->qcnt_max;

    entry.qcnt_increment_p2     = _is_power_of_two(ordering_info->qcnt_increment, &two_power);
    if (entry.qcnt_increment_p2) {
        entry.qcnt_bias         = two_power;
    } else {
        _convert_fraction_to_binary_coded_decimal(ordering_info->qcnt_increment, 1, ordering_info->qcnt_size, &bias);
        entry.qcnt_bias         = bias;
    }

    entry.modulo_2              = ordering_info->modulo_2;
    entry.gsm_tsn_bitmap        = ordering_info->gsm_tsn_bitmap;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_roe_ordering_info_set(unit, port, index, &entry));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_roe_ordering_sequence_offset_get(
    int unit,
    bcm_gport_t port,
    int queue,
    bcm_cpri_roe_oi_seq_offset_t *seq_offset)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_cpri_ordering_info_offset_t offset;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    /* user need to provide the sizes. */
    offset.p_size   = seq_offset->pcnt_size;
    offset.q_size   = seq_offset->qcnt_size;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_roe_ordering_sequence_offset_get(unit, port, queue, &offset));

    seq_offset->pcnt_offset = offset.p_offset;
    seq_offset->qcnt_offset = offset.q_offset;
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_roe_ordering_sequence_offset_set(
    int unit,
    bcm_gport_t port,
    int queue,
    bcm_cpri_roe_oi_seq_offset_t *seq_offset)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_cpri_ordering_info_offset_t offset;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    offset.p_size   = seq_offset->pcnt_size;
    offset.p_offset = seq_offset->pcnt_offset;
    offset.q_size   = seq_offset->qcnt_size;
    offset.q_offset = seq_offset->qcnt_offset;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_roe_ordering_sequence_offset_set(unit, port, queue, &offset));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef CPRIMOD_SUPPORT
static
int _bcm_cprimod_to_bcm_hdr_vlan_type(cprimod_hdr_vlan_type_t cprimod_type, bcm_cpri_vlan_type_t *bcm_type)
{
    switch (cprimod_type) {
        case cprimodHdrVlanTypeUntagged:
            *bcm_type = bcmCpriVlanTypeNone;
            break;

        case cprimodHdrVlanTypeTaggedVlan0:
            *bcm_type = bcmCpriVlanTypeSingle;
            break;

        case cprimodHdrVlanTypeQinQ:
            *bcm_type = bcmCpriVlanTypeQinQ;
            break;
        case cprimodHdrVlanTypeTaggedVlan1:
            *bcm_type = bcmCpriVlanTypeSingleTable1;
            break;
        default:
            *bcm_type = bcmCpriVlanTypeNone;
        break;
    }
    return SOC_E_NONE;

}
#endif

int
bcm_esw_cpri_port_encap_get(
    int unit,
    bcm_gport_t port,
    int queue_num,
    bcm_cpri_encap_info_t *encap_config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_encap_header_config_t config;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    cprimod_encap_header_config_t_init(&config);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_get(unit, port, queue_num, &config));

    _bcm_cprimod_to_bcm_hdr_vlan_type(config.vlan_type, &encap_config->vlan_type);

    encap_config->hdr_type                  = config.header_type;
    encap_config->flow_id                   = config.flow_id ;
    encap_config->subtype                   = config.roe_subtype;
    encap_config->roe_ordering_info_index   = config.ordering_info_index;
    encap_config->macda_index               = config.mac_da_index;
    encap_config->macsa_index               = config.mac_sa_index;
    encap_config->vlan0_idx                 = config.vlan_id_0_index;
    encap_config->vlan1_idx                 = config.vlan_id_1_index;
    encap_config->vlan0_priority            = config.vlan_0_priority;
    encap_config->vlan1_priority            = config.vlan_1_priority;
    encap_config->vlan_ethertype_idx        = config.vlan_eth_type_index;
    encap_config->opcode                    = config.roe_opcode;
    encap_config->version                   = config.version;
    encap_config->type                      = config.roe_frame_format;
    encap_config->ecpri12_header_byte0      = config.ecpri12_header_byte0;

    if (config.use_tagid_for_vlan) {
        encap_config->flags |= BCM_CPRI_ENCAP_USE_TAG_ID_FOR_VLAN;
    }

    if (config.use_tagid_for_flowid) {
        encap_config->flags |= BCM_CPRI_ENCAP_USE_TAG_ID_FOR_FLOWID;
    }

    if (config.use_opcode) {
        encap_config->flags |= BCM_CPRI_ENCAP_USE_OPCODE;
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_entry_macda_get(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_mac_t macaddr)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_roe_encap_macda_entry_get(unit, port, index, macaddr));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_entry_macda_set(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_mac_t macaddr)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_roe_encap_macda_entry_set(unit, port, index, macaddr));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_entry_macsa_get(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_mac_t macaddr)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_roe_encap_macsa_entry_get(unit, port, index, macaddr));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_entry_macsa_set(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_mac_t macaddr)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_roe_encap_macsa_entry_set(unit, port, index, macaddr));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_entry_vlan_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    uint32 index,
    bcm_vlan_t *vlan_id)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_vlan_table_id_t table_id = cprimodVlanTable0;
    uint32 int_vlan_id;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if (flags == BCM_CPRI_VLAN_FLAG_VLAN_0) {
        table_id = cprimodVlanTable0;
    } else {
        table_id = cprimodVlanTable1;
    }

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_roe_encap_vlan_entry_get(unit, port, index, table_id, &int_vlan_id));

    *vlan_id = int_vlan_id;
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_entry_vlan_set(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    uint32 index,
    bcm_vlan_t vlan_id)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_vlan_table_id_t table_id = cprimodVlanTable0;
    uint32 int_vlan_id;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if (flags == BCM_CPRI_VLAN_FLAG_VLAN_0) {
        table_id = cprimodVlanTable0;
    } else {
        table_id = cprimodVlanTable1;
    }
    int_vlan_id = vlan_id;
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_roe_encap_vlan_entry_set(unit, port, index, table_id, int_vlan_id));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef CPRIMOD_SUPPORT
static
int _bcm_bcm_to_cprimod_ordering_info_incr_type(bcm_cpri_roe_incr_type_t bcm_type, cprimod_ordering_info_prop_t *cprimod_type)
{
    switch (bcm_type) {
        case bcmCpriRoEIncrTypeNone:
            *cprimod_type = cprimod_ordering_info_prop_no_increment;
            break;

        case bcmCpriRoEIncrTypeConstant:
            *cprimod_type = cprimod_ordering_info_prop_increment_by_constant;
            break;

        case bcmCpriRoEIncrTypePayloadSize:
            *cprimod_type = cprimod_ordering_info_prop_increment_by_payload;
            break;
        default:
            break;
    }
    return SOC_E_NONE;
}

static
int _bcm_cprimod_to_bcm_ordering_info_incr_type(cprimod_ordering_info_prop_t cprimod_type, bcm_cpri_roe_incr_type_t* bcm_type)
{
    switch (cprimod_type) {
        case cprimod_ordering_info_prop_no_increment:
            *bcm_type = bcmCpriRoEIncrTypeNone;
            break;

        case cprimod_ordering_info_prop_increment_by_constant:
            *bcm_type = bcmCpriRoEIncrTypeConstant;
            break;

        case cprimod_ordering_info_prop_increment_by_payload:
            *bcm_type = bcmCpriRoEIncrTypePayloadSize;
            break;
        default:
            break;
    }
    return SOC_E_NONE;
}
#endif
int
bcm_esw_cpri_port_encap_roe_ordering_info_get(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_cpri_encap_roe_oi_t *ordering_info)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_encap_ordering_info_entry_t entry;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_encap_ordering_info_entry_t_init(&entry);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_roe_ordering_info_get(unit, port, index, &entry));

    _bcm_cprimod_to_bcm_ordering_info_type(entry.type, &ordering_info->info_type);
    _bcm_cprimod_to_bcm_ordering_info_incr_type(entry.pcnt_prop, &ordering_info->p_seq_incr);
    _bcm_cprimod_to_bcm_ordering_info_incr_type(entry.qcnt_prop, &ordering_info->q_seq_incr);
    ordering_info->pcnt_size        = entry.pcnt_size;
    ordering_info->pcnt_increment   = entry.pcnt_increment;
    ordering_info->pcnt_max         = entry.pcnt_max;
    ordering_info->qcnt_size        = entry.qcnt_size;
    ordering_info->qcnt_increment   = entry.qcnt_increment;
    ordering_info->qcnt_max         = entry.qcnt_max;
    ordering_info->seq_rsvd         = entry.seq_reserve_value;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_ordering_info_set(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_cpri_encap_roe_oi_t *ordering_info)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_encap_ordering_info_entry_t entry;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_encap_ordering_info_entry_t_init(&entry);

    _bcm_bcm_to_cprimod_ordering_info_type(ordering_info->info_type, &entry.type);
    _bcm_bcm_to_cprimod_ordering_info_incr_type(ordering_info->p_seq_incr, &entry.pcnt_prop);
    _bcm_bcm_to_cprimod_ordering_info_incr_type(ordering_info->q_seq_incr, &entry.qcnt_prop);
    entry.pcnt_size             = ordering_info->pcnt_size;
    entry.pcnt_increment        = ordering_info->pcnt_increment;
    entry.pcnt_max              = ordering_info->pcnt_max;
    entry.qcnt_size             = ordering_info->qcnt_size;
    entry.qcnt_increment        = ordering_info->qcnt_increment;
    entry.qcnt_max              = ordering_info->qcnt_max;
    entry.seq_reserve_value     = ordering_info->seq_rsvd;


    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_roe_ordering_info_set(unit, port, index, &entry));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif

}

int
bcm_esw_cpri_port_encap_roe_ordering_sequence_offset_get(
    int unit,
    bcm_gport_t port,
    int queue,
    int control,
    bcm_cpri_roe_oi_seq_offset_t *seq_offset)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_cpri_ordering_info_offset_t offset;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    /* user need to provide the sizes. */
    offset.p_size   = seq_offset->pcnt_size;
    offset.q_size   = seq_offset->qcnt_size;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_roe_ordering_sequence_offset_get(unit, port, queue, control, &offset));

    seq_offset->pcnt_offset = offset.p_offset;
    seq_offset->qcnt_offset = offset.q_offset;
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_ordering_sequence_offset_set(
    int unit,
    bcm_gport_t port,
    int queue,
    int control,
    bcm_cpri_roe_oi_seq_offset_t *seq_offset)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_cpri_ordering_info_offset_t offset;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    offset.p_size   = seq_offset->pcnt_size;
    offset.p_offset = seq_offset->pcnt_offset;
    offset.q_size   = seq_offset->qcnt_size;
    offset.q_offset = seq_offset->qcnt_offset;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_roe_ordering_sequence_offset_set(unit, port, queue, control, &offset));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef CPRIMOD_SUPPORT
static
int _bcm_to_cprimod_hdr_vlan_type(bcm_cpri_vlan_type_t bcm_type, cprimod_hdr_vlan_type_t *cprimod_type)
{
    switch (bcm_type) {
        case bcmCpriVlanTypeNone:
            *cprimod_type = cprimodHdrVlanTypeUntagged;
            break;

        case bcmCpriVlanTypeSingle:
            *cprimod_type = cprimodHdrVlanTypeTaggedVlan0;
            break;

        case bcmCpriVlanTypeQinQ:
            *cprimod_type = cprimodHdrVlanTypeQinQ;
            break;
        case bcmCpriVlanTypeSingleTable1:
            *cprimod_type = cprimodHdrVlanTypeTaggedVlan1;
            break;
        default:
            *cprimod_type = cprimodHdrVlanTypeUntagged;
        break;
    }
    return SOC_E_NONE;
}
#endif

int
bcm_esw_cpri_port_encap_set(
    int unit,
    bcm_gport_t port,
    int queue_num,
    bcm_cpri_encap_info_t *encap_config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_encap_header_config_t config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_encap_header_config_t_init(&config);

    _bcm_to_cprimod_hdr_vlan_type(encap_config->vlan_type, &config.vlan_type);

    config.header_type = encap_config->hdr_type ;

    config.flow_id              = encap_config->flow_id ;
    config.roe_subtype          = encap_config->subtype ;
    config.ordering_info_index  = encap_config->roe_ordering_info_index ;
    config.mac_da_index         = encap_config->macda_index ;
    config.mac_sa_index         = encap_config->macsa_index ;
    config.vlan_id_0_index      = encap_config->vlan0_idx ;
    config.vlan_id_1_index      = encap_config->vlan1_idx ;
    config.vlan_0_priority      = encap_config->vlan0_priority ;
    config.vlan_1_priority      = encap_config->vlan1_priority ;
    config.vlan_eth_type_index  = encap_config->vlan_ethertype_idx;
    config.roe_opcode           = encap_config->opcode;
    config.version              = encap_config->version;
    config.roe_frame_format     = encap_config->type;
    config.ecpri_pc_id          = encap_config->ecpri_pc_id;
    config.ecpri_msg_type       = encap_config->ecpri_msg_type;
    config.ecpri12_header_byte0 = encap_config->ecpri12_header_byte0;

    if(encap_config->flags & BCM_CPRI_ENCAP_USE_TAG_ID_FOR_VLAN){
        config.use_tagid_for_vlan = 1;
    } else {
        config.use_tagid_for_vlan = 0;
    }

    if(encap_config->flags & BCM_CPRI_ENCAP_USE_TAG_ID_FOR_FLOWID){
        config.use_tagid_for_flowid = 1;
    } else {
        config.use_tagid_for_flowid = 0;
    }

    if(encap_config->flags & BCM_CPRI_ENCAP_USE_OPCODE){
        config.use_opcode = 1;
    } else {
        config.use_opcode = 0;
    }

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_set(unit, port, queue_num, &config));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_vlan_config_get(
    int unit,
    bcm_gport_t port,
    bcm_cpri_encap_vlan_config_t *vlan_config)
{
#ifdef CPRIMOD_SUPPORT
    uint16 ethertype = 0;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_get(unit, port, cprimodEthertypeRoe, &ethertype));
    vlan_config->roe_ethertype_0 = ethertype;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_get(unit, port, cprimodEthertypeRoe1, &ethertype));
    vlan_config->roe_ethertype_1= ethertype;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_get(unit, port, cprimodEthertypeVlan, &ethertype));
    vlan_config->ethertype_vlan_tagged= ethertype;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_get(unit, port, cprimodEthertypeQinQ, &ethertype));
    vlan_config->ethertype_vlan_qinq= ethertype;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_vlan_config_set(
    int unit,
    bcm_gport_t port,
    bcm_cpri_encap_vlan_config_t *vlan_config)
{
#ifdef CPRIMOD_SUPPORT
    uint16 ethertype;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    ethertype = vlan_config->roe_ethertype_0;
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_set(unit, port, cprimodEthertypeRoe, ethertype));

    ethertype = vlan_config->roe_ethertype_1;
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_set(unit, port, cprimodEthertypeRoe1, ethertype));

    ethertype = vlan_config->ethertype_vlan_tagged;
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_set(unit, port, cprimodEthertypeVlan, ethertype));

    ethertype = vlan_config->ethertype_vlan_qinq;
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_set(unit, port, cprimodEthertypeQinQ, ethertype));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_dbm_entry_clear(
    int unit,
    bcm_gport_t port,
    int index)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_dbm_rule_entry_t profile;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_dbm_rule_entry_t_init(&profile);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_dbm_entry_set(unit, port, index, &profile));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef CPRIMOD_SUPPORT
static
int _bcm_to_cprimod_position_start(uint8 bcm_pos_index, cprimod_dbm_rule_pos_index_t *pos_grp_index)
{
    switch (bcm_pos_index) {
        case 0:
            *pos_grp_index = cprimodDbmRulePosIndexStart0;
            break;
        case 10:
            *pos_grp_index = cprimodDbmRulePosIndexStart10;
            break;
        case 20:
            *pos_grp_index = cprimodDbmRulePosIndexStart20;
            break;
        case 30:
            *pos_grp_index = cprimodDbmRulePosIndexStart30;
            break;
        case 40:
            *pos_grp_index = cprimodDbmRulePosIndexStart40;
            break;
        case 50:
            *pos_grp_index = cprimodDbmRulePosIndexStart50;
            break;
        case 60:
            *pos_grp_index = cprimodDbmRulePosIndexStart60;
            break;
        case 70:
            *pos_grp_index = cprimodDbmRulePosIndexStart70;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_CPRI,
                      (BSL_META("DBM Profile Position Table cannot start at %d\n"), bcm_pos_index));
            return BCM_E_PARAM;
            break;
    }
    return BCM_E_NONE;
}

int _cprimod_to_bcm_position_start(cprimod_dbm_rule_pos_index_t pos_grp_index, uint8 *bcm_pos_index)
{
    switch (pos_grp_index) {
        case cprimodDbmRulePosIndexStart0:
            *bcm_pos_index = 0;
            break;
        case cprimodDbmRulePosIndexStart10:
            *bcm_pos_index = 10;
            break;
        case cprimodDbmRulePosIndexStart20:
            *bcm_pos_index = 20;
            break;
        case cprimodDbmRulePosIndexStart30:
            *bcm_pos_index = 30;
            break;
        case cprimodDbmRulePosIndexStart40:
            *bcm_pos_index = 40;
            break;
        case cprimodDbmRulePosIndexStart50:
            *bcm_pos_index = 50;
            break;
        case cprimodDbmRulePosIndexStart60:
            *bcm_pos_index = 60;
            break;
        case cprimodDbmRulePosIndexStart70:
            *bcm_pos_index = 70;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_CPRI,
                      (BSL_META( "DBM Profile Position Table cannot start at %d\n"), pos_grp_index));
            return BCM_E_PARAM;
            break;
    }
    return BCM_E_NONE;
}

static
int _bcm_to_cprimod_position_table_size(uint8 pos_table_size, cprimod_dbm_rule_pos_grp_size_t *pos_grp_size)
{
    switch (pos_table_size) {
        case 10:
            *pos_grp_size = cprimodDbmRulePosGrpSize10;
            break;
        case 20:
            *pos_grp_size = cprimodDbmRulePosGrpSize20;
            break;
        case 40:
            *pos_grp_size = cprimodDbmRulePosGrpSize40;
            break;
        case 80:
            *pos_grp_size = cprimodDbmRulePosGrpSize80;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_CPRI,
                      (BSL_META( "DBM Profile Num Position Table cannot be  %d\n"), pos_table_size));
            return BCM_E_PARAM;
            break;
    }
    return BCM_E_NONE;
}

static
int _cprimod_to_bcm_position_table_size(cprimod_dbm_rule_pos_grp_size_t pos_grp_size, uint8 *pos_table_size)
{
    switch (pos_grp_size) {
        case cprimodDbmRulePosGrpSize10:
            *pos_table_size = 10;
            break;
        case cprimodDbmRulePosGrpSize20:
            *pos_table_size = 20;
            break;
        case cprimodDbmRulePosGrpSize40:
            *pos_table_size = 40;
            break;
        case cprimodDbmRulePosGrpSize80:
            *pos_table_size = 80;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_CPRI,
                      (BSL_META( "DBM Profile Num Position Table cannot be  %d\n"), pos_grp_size));
            return BCM_E_PARAM;
            break;
    }
    return BCM_E_NONE;
}
#endif

int
bcm_esw_cpri_port_rsvd4_dbm_entry_get(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_rsvd4_dbm_entry_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_dbm_rule_entry_t profile;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_dbm_rule_entry_t_init(&profile);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_dbm_entry_get(unit, port, index, &profile));
    entry->bm1_multiple = profile.bm1_mult;
    entry->bm1[0]       = profile.bm1[0];
    entry->bm1[1]       = profile.bm1[1];
    entry->bm1[2]       = profile.bm1[2];
    entry->bm1_size     = profile.bm1_size;
    entry->bm2[0]       = profile.bm2[0];
    entry->bm2[1]       = profile.bm2[1];
    entry->bm2_size     = profile.bm2_size;
    entry->num_slots    = profile.num_slots;

    SOC_IF_ERROR_RETURN
        (_cprimod_to_bcm_position_start(profile.pos_grp_index, &entry->pos_index));

    SOC_IF_ERROR_RETURN
        (_cprimod_to_bcm_position_table_size(profile.pos_grp_size,&entry->pos_entries));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif

}

int
bcm_esw_cpri_port_rsvd4_dbm_entry_set(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_rsvd4_dbm_entry_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_dbm_rule_entry_t profile;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_dbm_rule_entry_t_init(&profile);

    profile.bm1_mult    = entry->bm1_multiple;
    profile.bm1[0]      = entry->bm1[0];
    profile.bm1[1]      = entry->bm1[1];
    profile.bm1[2]      = entry->bm1[2];
    profile.bm1_size    = entry->bm1_size;
    profile.bm2[0]      = entry->bm2[0];
    profile.bm2[1]      = entry->bm2[1];
    profile.bm2_size    = entry->bm2_size;
    profile.num_slots   = entry->num_slots;

    SOC_IF_ERROR_RETURN
        (_bcm_to_cprimod_position_start(entry->pos_index, &profile.pos_grp_index));

    SOC_IF_ERROR_RETURN
        (_bcm_to_cprimod_position_table_size(entry->pos_entries, &profile.pos_grp_size));

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_dbm_entry_set(unit, port, index, &profile));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_rx_bfn_sync_config_set(
    int unit,
    bcm_gport_t port,
    uint16 master_frame_count,
    uint64 master_frame_start)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_rx_master_frame_sync_config_set(unit, port, master_frame_count,master_frame_start));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_rx_flow_add(
    int unit,
    bcm_gport_t port,
    int flow_id,
    bcm_rsvd4_rx_flow_info_t *flow_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_rsvd4_rx_flow_info_t int_flow_info;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    int_flow_info.mask                      = flow_info->mask ;
    int_flow_info.match_data                = flow_info->match_data ;

    int_flow_info.packet_size               = flow_info->payload_size ;
    int_flow_info.last_packet_size          = flow_info->last_payload_size ;
    int_flow_info.last_packet_num           = flow_info->last_payload_index ;

    int_flow_info.axc_id                 = flow_info->axc_id;
    int_flow_info.stuffing_cnt              = flow_info->stuffing_count ;
    int_flow_info.msg_ts_mode               = flow_info->msg_ts_mode;
    int_flow_info.msg_ts_count              = flow_info-> msg_ts_count;

    int_flow_info.master_frame_offset       = flow_info->master_frame_offset;
    int_flow_info.message_number_offset     = flow_info->message_number_offset ;
    int_flow_info.container_block_count     = flow_info->container_blk_count;
    int_flow_info.queue_size                = flow_info->queue_size ;
    int_flow_info.priority                  = flow_info->priority ;
    int_flow_info.pad_size                  = flow_info->gsm_pad_size ;
    int_flow_info.extra_pad_size            = flow_info->gsm_extra_pad_size ;
    int_flow_info.pad_enable                = flow_info->gsm_pad_en;
    int_flow_info.control_location          = flow_info->gsm_ctrl_loc ;
    int_flow_info.tsn_bitmap                = flow_info->gsm_tsn_bitmap ;
    int_flow_info.frame_sync_mode           = flow_info->sync_mode;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_rx_flow_add(unit, port, flow_id, &int_flow_info));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_rx_flow_delete(
    int unit,
    bcm_gport_t port,
    int flow_id)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_rx_flow_delete(unit, port, flow_id));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}


int
bcm_esw_cpri_port_rsvd4_rx_frame_config_debug_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_config_t config_type,
    uint32 value)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_rsvd4_rx_config_field_t field;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (config_type) {
    case bcmRsvd4RxConfigDescrambleEnable:
        field = CPRIMOD_RSVD4_RX_CONFIG_DESCRAMBLE_ENABLE;
        break;
    case bcmRsvd4RxConfigSyncThreshold:
        field = CPRIMOD_RSVD4_RX_CONFIG_SYNC_T;
        break;
    case bcmRsvd4RxConfigUnsyncThreshold:
        field = CPRIMOD_RSVD4_RX_CONFIG_UNSYNC_T;
        break;
    case bcmRsvd4RxConfigFrameSyncThreshold:
        field = CPRIMOD_RSVD4_RX_CONFIG_FRAME_SYNC_T;
        break;
    case bcmRsvd4RxConfigFrameUnsyncThreshold:
        field = CPRIMOD_RSVD4_RX_CONFIG_FRAME_UNSYNC_T;
        break;
    case bcmRsvd4RxConfigBlockSize:
        field = CPRIMOD_RSVD4_RX_CONFIG_BLOCK_SIZE;
        break;
    default:
        return BCM_E_PARAM;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_rx_frame_optional_config_set(unit,
                                                                       port,
                                                                       field,
                                                                       value));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Set RSVD4 Rx FSM Control and/or Configuration. */
int
bcm_esw_cpri_port_rsvd4_rx_fsm_control_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_fsm_control_t control_type,
    uint32 value) {
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (control_type) {
    case BCM_RSVD4_RX_FSM_CONTROL_DESCRAMBLE_ENABLE:
    case BCM_RSVD4_RX_FSM_CONTROL_SYNC_T:
    case BCM_RSVD4_RX_FSM_CONTROL_UNSYNC_T:
    case BCM_RSVD4_RX_FSM_CONTROL_FRAME_SYNC_T:
    case BCM_RSVD4_RX_FSM_CONTROL_FRAME_UNSYNC_T:
        BCM_IF_ERROR_RETURN
            (portmod_port_rsvd4_rx_frame_optional_config_set(unit,
                                                             port,
                                                             control_type,
                                                             value));
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_CPRI,
                 (BSL_META("Unsupported ControlType %d.\n"), control_type));
        return BCM_E_PARAM;
        break;
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Get RSVD4 Rx FSM Control and/or Configuration. */
int
bcm_esw_cpri_port_rsvd4_rx_fsm_control_get(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_fsm_control_t control_type,
    uint32 *value) {
#ifdef CPRIMOD_SUPPORT
    int data = 0;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (control_type) {
    case BCM_RSVD4_RX_FSM_CONTROL_DESCRAMBLE_ENABLE:
    case BCM_RSVD4_RX_FSM_CONTROL_SYNC_T:
    case BCM_RSVD4_RX_FSM_CONTROL_UNSYNC_T:
    case BCM_RSVD4_RX_FSM_CONTROL_FRAME_SYNC_T:
    case BCM_RSVD4_RX_FSM_CONTROL_FRAME_UNSYNC_T:
        BCM_IF_ERROR_RETURN
            (portmod_port_rsvd4_rx_frame_optional_config_get(unit,
                                                             port,
                                                             control_type,
                                                             &data));
        *value = data;
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_CPRI,
                 (BSL_META("Unsupported ControlType %d.\n"), control_type));
        return BCM_E_PARAM;
        break;
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif

}


int
bcm_esw_cpri_port_rsvd4_rx_frame_config_get(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_frame_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_port_rsvd4_speed_mult_t speed;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_rx_speed_get(unit, port, &speed));
    if(speed == cprimodRsvd4SpdMult4X) {
        config->speed_multiple = 4;
    } else if (speed == cprimodRsvd4SpdMult8X) {
        config->speed_multiple = 8;
    } else {
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_rx_frame_config_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_frame_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_port_rsvd4_speed_mult_t speed = cprimodRsvd4SpdMultCount;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if(config->speed_multiple == 4) {
        speed = cprimodRsvd4SpdMult4X;
    } else if(config->speed_multiple == 8) {
        speed = cprimodRsvd4SpdMult8X;
    } else {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_rx_speed_set(unit, port, speed));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_rx_override_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_params_t parameter,
    int enable,
    int value)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_rsvd4_rx_overide_t parameter_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (parameter) {
    case bcmRsvd4RxParams_Frame_Unsync_T_Invalid_MG_Rcvd:
        parameter_t = cprimodRsvd4RxOverideFrameUnsyncTInvldMgRecvd;
        break;
    case bcmRsvd4RxParams_Frame_Sync_T_Valid_MG_Rcvd:
        parameter_t = cprimodRsvd4RxOverideFrameSyncTVldMgRecvd;
        break;
    case bcmRsvd4RxParams_K_MG_Idles_Rcvd:
        parameter_t = cprimodRsvd4RxOverideKMgIdlesRecvd;
        break;
    case bcmRsvd4RxParams_Idle_Req_Rcvd:
        parameter_t = cprimodRsvd4RxOverideIdleReqRecvd;
        break;
    case bcmRsvd4RxParams_Idle_Ack_Rcvd:
        parameter_t = cprimodRsvd4RxOverideIdleAckRecvd;
        break;
    case bcmRsvd4RxParams_SeedCapAndVerifyDone:
        parameter_t = cprimodRsvd4RxOverideSeedCapAndVerifyDone;
        break;
    default:
        parameter_t = cprimodRsvd4RxOverideFrameUnsyncTInvldMgRecvd;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_cpri_port_rsvd4_rx_override_set(unit, port,
                                                                parameter_t,
                                                                enable,
                                                                value));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_rx_fsm_state_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_fsm_state_t state)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_rsvd4_rx_fsm_state_t state_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (state) {
    case bcmRsvd4RxFsmStateUnsync:
        state_t = cprimodRsvd4RxFsmStateUnsync;
        break;
    case bcmRsvd4RxFsmStateWaitForSeed:
        state_t = cprimodRsvd4RxFsmStateWaitForSeed;
        break;
    case bcmRsvd4RxFsmStateWaitForAck:
        state_t = cprimodRsvd4RxFsmStateWaitForAck;
        break;
    case bcmRsvd4RxFsmStateWaitForK28p7Idles:
        state_t = cprimodRsvd4RxFsmStateWaitForK28p7Idles;
        break;
    case bcmRsvd4RxFsmStateWaitForFrameSync:
        state_t = cprimodRsvd4RxFsmStateWaitForFrameSync;
        break;
    case bcmRsvd4RxFsmStateFrameSync:
        state_t = cprimodRsvd4RxFsmStateFrameSync;
        break;
    default:
        state_t = cprimodRsvd4RxFsmStateUnsync;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_cpri_port_rsvd4_rx_fsm_state_set(unit, port,
                                                                 state_t));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_secondary_dbm_entry_clear(
    int unit,
    bcm_gport_t port,
    int index)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_secondary_dbm_rule_entry_t profile;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_secondary_dbm_rule_entry_t_init(&profile);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_secondary_dbm_entry_set(unit, port, index, &profile));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_secondary_dbm_entry_get(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_rsvd4_secondary_dbm_entry_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_secondary_dbm_rule_entry_t profile;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_secondary_dbm_rule_entry_t_init(&profile);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_secondary_dbm_entry_get(unit, port, index, &profile));

    entry->bm1_multiple     = profile.bm1_mult;
    entry->bm1              = profile.bm1;
    entry->bm1_size         = profile.bm1_size;
    entry->bm2              = profile.bm2;
    entry->bm2_size         = profile.bm2_size;
    entry->num_slots        = profile.num_slots;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif

}

int
bcm_esw_cpri_port_rsvd4_secondary_dbm_entry_set(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_rsvd4_secondary_dbm_entry_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_secondary_dbm_rule_entry_t profile;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_secondary_dbm_rule_entry_t_init(&profile);

    profile.bm1_mult    = entry->bm1_multiple;
    profile.bm1         = entry->bm1;
    profile.bm1_size    = entry->bm1_size;
    profile.bm2         = entry->bm2;
    profile.bm2_size    = entry->bm2_size;
    profile.num_slots   = entry->num_slots;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_secondary_dbm_entry_set(unit, port, index, &profile));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_flow_add(
    int unit,
    bcm_gport_t port,
    int flow_id,
    bcm_rsvd4_tx_flow_info_t *flow_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_rsvd4_tx_flow_info_t int_flow_info;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    portmod_rsvd4_tx_flow_info_t_init(unit, &int_flow_info);

    int_flow_info.packet_size               = flow_info->payload_size;
    int_flow_info.last_packet_size          = flow_info->last_payload_size;
    int_flow_info.last_packet_num           = flow_info->last_index ;

    int_flow_info.axc_id                    = flow_info->axc_id;
    int_flow_info.stuffing_cnt              = flow_info->stuffing_count ;
    int_flow_info.msg_ts_mode               = flow_info->msg_ts_mode;
    int_flow_info.msg_ts_count              = flow_info-> msg_ts_count;
    int_flow_info.master_frame_offset       = flow_info->master_frame_offset;
    int_flow_info.message_number_offset     = flow_info->message_number_offset;
    int_flow_info.container_block_count     = flow_info->container_blk_count;
    int_flow_info.buffer_size                = flow_info->buffer_size ;
    int_flow_info.cycle_size                = flow_info->cycle_size ;
    int_flow_info.use_ts_dbm                = flow_info->secondary_dbm_enable;
    int_flow_info.ts_dbm_profile_num        = flow_info->index_to_secondary_dbm; /* secondary dbm profile num. */
    int_flow_info.num_active_slot           = flow_info->num_active_slot;
    int_flow_info.msg_addr                  = flow_info->message_addr;
    int_flow_info.msg_type                  = flow_info->message_type;
    int_flow_info.msg_ts_offset             = flow_info->msg_ts_offset;
    int_flow_info.pad_size                  = flow_info->pad_size ;
    int_flow_info.extra_pad_size            = flow_info->extra_pad_size ;
    int_flow_info.pad_enable                = flow_info->pad_enable;
    int_flow_info.control_location          = flow_info->gsm_ctrl_loc ;
    int_flow_info.frame_sync_mode           = flow_info->sync_mode;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_tx_flow_add(unit, port, flow_id, &int_flow_info));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_flow_delete(
    int unit,
    bcm_gport_t port,
    int flow_id)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_tx_flow_delete(unit, port, flow_id));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif

}

int
bcm_esw_cpri_port_rsvd4_tx_modulo_rule_entry_set(
    int unit,
    bcm_gport_t port,
    uint32 modulo_rule_num,
    bcm_cpri_tx_rule_type_t modulo_rule_type,
    bcm_cpri_modulo_rule_entry_t* modulo_rule)

{
#ifdef CPRIMOD_SUPPORT
    cprimod_modulo_rule_entry_t mod_rule;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_modulo_rule_entry_t_init(&mod_rule);

    mod_rule.active         = modulo_rule->active;
    mod_rule.modulo_value   = modulo_rule->modulo_value;
    mod_rule.modulo_index   = modulo_rule->modulo_index;
    mod_rule.dbm_enable     = modulo_rule->dbm_enable;
    mod_rule.flow_dbm_id    = modulo_rule->dbm_dbm_id;
    mod_rule.flow_type      = modulo_rule->flow_type;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_modulo_rule_entry_set(unit, port, modulo_rule_num, modulo_rule_type, &mod_rule));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_modulo_rule_entry_get(
    int unit,
    bcm_gport_t port,
    uint32 modulo_rule_num,
    bcm_cpri_tx_rule_type_t modulo_rule_type,
    bcm_cpri_modulo_rule_entry_t* modulo_rule)

{
#ifdef CPRIMOD_SUPPORT
    cprimod_modulo_rule_entry_t mod_rule;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_modulo_rule_entry_t_init(&mod_rule);
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_modulo_rule_entry_get(unit, port, modulo_rule_num, modulo_rule_type, &mod_rule));
    modulo_rule->active         = mod_rule.active;
    modulo_rule->modulo_value   = mod_rule.modulo_value;
    modulo_rule->modulo_index   = mod_rule.modulo_index;
    modulo_rule->dbm_enable     = mod_rule.dbm_enable;
    modulo_rule->dbm_dbm_id     = mod_rule.flow_dbm_id ;
    modulo_rule->flow_type      = mod_rule.flow_type;
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_position_entry_set(
    int unit,
    bcm_gport_t port,
    uint32 pos_index,
    bcm_cpri_dbm_position_entry_t* pos_entry)

{
#ifdef CPRIMOD_SUPPORT
    cprimod_dbm_pos_table_entry_t position_entry;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_dbm_pos_table_entry_t_init(&position_entry);

    position_entry.valid        = pos_entry->valid;
    position_entry.flow_id      = pos_entry->flow_id;
    position_entry.flow_type    = pos_entry->flow_type;
    position_entry.index        = pos_entry->index;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_dbm_position_entry_set(unit, port, pos_index, &position_entry));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_position_entry_get(
    int unit,
    bcm_gport_t port,
    uint32 pos_index,
    bcm_cpri_dbm_position_entry_t* pos_entry)

{
#ifdef CPRIMOD_SUPPORT
    cprimod_dbm_pos_table_entry_t position_entry;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_dbm_pos_table_entry_t_init(&position_entry);

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_dbm_position_entry_get(unit, port, pos_index, &position_entry));

    pos_entry->valid        = position_entry.valid;
    pos_entry->flow_id      = position_entry.flow_id;
    pos_entry->flow_type    = position_entry.flow_type;
    pos_entry->index        = position_entry.index;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}


int
bcm_esw_cpri_port_rsvd4_tx_frame_config_debug_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_tx_config_t config_type,
    uint32 value)
{
#ifdef CPRIMOD_SUPPORT

    cprimod_rsvd4_tx_config_field_t field;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (config_type) {
    case bcmRsvd4TxConfigForceOffState:
        field = CPRIMOD_RSVD4_TX_CONFIG_FORCE_OFF_STATE;
        break;
    case bcmRsvd4TxConfigAckT:
        field = CPRIMOD_RSVD4_TX_CONFIG_ACK_T;
        break;
    case bcmRsvd4TxConfigForceIdleAck:
        field = CPRIMOD_RSVD4_TX_CONFIG_FORCE_IDLE_ACK;
        break;
    case bcmRsvd4TxConfigLosEnable:
        field = CPRIMOD_RSVD4_TX_CONFIG_LOS_ENABLE;
        break;
    case bcmRsvd4TxConfigScrambleEnable:
        field = CPRIMOD_RSVD4_TX_CONFIG_SCRAMBLE_ENABLE;
        break;
    case bcmRsvd4TxConfigScrambleSeed:
        field = CPRIMOD_RSVD4_TX_CONFIG_SCRAMBLE_SEED;
        break;
    case bcmRsvd4TxConfigTransmitterEnable:
        field = CPRIMOD_RSVD4_TX_CONFIG_TRANSMITTER_ENABLE;
        break;
    default:
        field = CPRIMOD_RSVD4_TX_CONFIG_COUNT;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_tx_frame_optional_config_set(unit,
                                                                       port,
                                                                       field,
                                                                       value));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Set RSVD4 Tx FSM Control and/or Configuration. */
int
bcm_esw_cpri_port_rsvd4_tx_fsm_control_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_fsm_control_t control_type,
    uint32 value) {
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (control_type) {
    case BCM_RSVD4_TX_FSM_CONTROL_FORCE_OFF_STATE:
    case BCM_RSVD4_TX_FSM_CONTROL_ACK_T:
    case BCM_RSVD4_TX_FSM_CONTROL_FORCE_IDLE_ACK:
    case BCM_RSVD4_TX_FSM_CONTROL_LOS_ENABLE:
    case BCM_RSVD4_TX_FSM_CONTROL_SCRAMBLE_ENABLE:
    case BCM_RSVD4_TX_FSM_CONTROL_SCRAMBLE_SEED:
    case BCM_RSVD4_TX_FSM_CONTROL_TRANSMITTER_EN:
        BCM_IF_ERROR_RETURN
            (portmod_port_rsvd4_tx_frame_optional_config_set(unit,
                                                             port,
                                                             control_type,
                                                             value));
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_CPRI,
                 (BSL_META("Unsupported ControlType %d.\n"), control_type));
        return BCM_E_PARAM;
        break;
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Set RSVD4 Tx FSM Control and/or Configuration. */
int
bcm_esw_cpri_port_rsvd4_tx_fsm_control_get(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_fsm_control_t control_type,
    uint32* value) {
#ifdef CPRIMOD_SUPPORT
    int data=0;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (control_type) {
    case BCM_RSVD4_TX_FSM_CONTROL_FORCE_OFF_STATE:
    case BCM_RSVD4_TX_FSM_CONTROL_ACK_T:
    case BCM_RSVD4_TX_FSM_CONTROL_FORCE_IDLE_ACK:
    case BCM_RSVD4_TX_FSM_CONTROL_LOS_ENABLE:
    case BCM_RSVD4_TX_FSM_CONTROL_SCRAMBLE_ENABLE:
    case BCM_RSVD4_TX_FSM_CONTROL_SCRAMBLE_SEED:
    case BCM_RSVD4_TX_FSM_CONTROL_TRANSMITTER_EN:
        BCM_IF_ERROR_RETURN
            (portmod_port_rsvd4_tx_frame_optional_config_get(unit,
                                                             port,
                                                             control_type,
                                                             &data));
        *value = data;
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_CPRI,
                 (BSL_META("Unsupported ControlType %d.\n"), control_type));
        return BCM_E_PARAM;
        break;
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_frame_config_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_tx_frame_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_port_rsvd4_speed_mult_t speed = cprimodRsvd4SpdMultCount;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if(config->speed_multiple == 4) {
        speed = cprimodRsvd4SpdMult4X;
    } else if(config->speed_multiple == 8) {
        speed = cprimodRsvd4SpdMult8X;
    }

    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_tx_speed_set(unit, port, speed));
    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_tx_frame_optional_config_set(unit,
                                                                       port,
                                                                       CPRIMOD_RSVD4_TX_CONFIG_SCRAMBLE_SEED,
                                                                       config->rsvd4_scrambler_seed));
    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_tx_frame_optional_config_set(unit,
                                                                       port,
                                                                       CPRIMOD_RSVD4_TX_CONFIG_LOS_ENABLE,
                                                                       config->los_enable));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_frame_config_get(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_tx_frame_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_port_rsvd4_speed_mult_t speed = cprimodRsvd4SpdMultCount;
    int scrambler_seed =0;
    int los_enable =0;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_tx_speed_get(unit, port, &speed));

    if (speed == cprimodRsvd4SpdMult4X) {
        config->speed_multiple = 4;
    } else if (speed == cprimodRsvd4SpdMult8X) {
        config->speed_multiple = 8;
    }

    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_tx_frame_optional_config_get(unit,
                                                                       port,
                                                                       CPRIMOD_RSVD4_TX_CONFIG_SCRAMBLE_SEED,
                                                                       &scrambler_seed));
    config->rsvd4_scrambler_seed = scrambler_seed;
    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_tx_frame_optional_config_get(unit,
                                                                       port,
                                                                       CPRIMOD_RSVD4_TX_CONFIG_LOS_ENABLE,
                                                                       &los_enable));
    config->los_enable = los_enable;
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_override_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_tx_params_t parameter,
    int enable,
    int value)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_rsvd4_tx_overide_t parameter_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (parameter) {
    case bcmRsvd4TxParams_StartTx:
        parameter_t = cprimodRsvd4TxOverideStartTx;
        break;
    case bcmRsvd4TxParams_PCS_Ack_CAP:
        parameter_t = cprimodRsvd4TxOverideRxPcsAckCap;
        break;
    case bcmRsvd4TxParams_PCS_Idle_REQ:
        parameter_t = cprimodRsvd4TxOverideRxPcsIdleReq;
        break;
    case bcmRsvd4TxParams_PCS_SCR_Lock:
        parameter_t = cprimodRsvd4TxOverideRxPcsScrLock;
        break;
    case bcmRsvd4TxParams_LOS_Status:
        parameter_t = cprimodRsvd4TxOverideLosStauts;
        break;
    default:
        parameter_t = cprimodRsvd4TxOverideStartTx;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_cpri_port_rsvd4_tx_override_set(unit, port,
                                                                parameter_t,
                                                                enable,
                                                                value));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_fsm_state_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_tx_fsm_state_t state)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_rsvd4_tx_fsm_state_t state_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (state) {
    case bcmRsvd4TxFsmStateOff:
        state_t = cprimodRsvd4TxFsmStateOff;
        break;
    case bcmRsvd4TxFsmStateIdle:
        state_t = cprimodRsvd4TxFsmStateIdle;
        break;
    case bcmRsvd4TxFsmStateIdleReq:
        state_t = cprimodRsvd4TxFsmStateIdleReq;
        break;
    case bcmRsvd4TxFsmStateIdleAck:
        state_t = cprimodRsvd4TxFsmStateIdleAck;
        break;
    case bcmRsvd4TxFsmStateFrameTx:
        state_t = cprimodRsvd4TxFsmStateFrameTx;
        break;
    default:
        state_t = cprimodRsvd4TxFsmStateOff;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_cpri_port_rsvd4_tx_fsm_state_set(unit, port,
                                                                 state_t));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rx_axc_frame_add(
    int unit,
    bcm_gport_t port,
    int axc_id,
    bcm_cpri_axc_frame_info_t *axc_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_axc_frame_info_t axc_info_t;
    portmod_cpri_rx_agnostic_config_info_t agnostic_config;
    int agnostic_enable;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_agnostic_config_get(unit, port,
                                                  &agnostic_enable,
                                                  &agnostic_config));

    if (agnostic_enable) {
        LOG_ERROR(BSL_LS_BCM_CPRI,
                  (BSL_META("AxC cannot added while Agnostic Mode is Enabled.\n")));
        return BCM_E_CONFIG;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_cpri_axc_frame_info_t_to_portmod_cpri_axc_frame_info_t(axc_id,
                                                                     BCM_CPRI_DIR_RX,
                                                                     axc_info,
                                                                     &axc_info_t));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_rx_axc_frame_add(unit, port, axc_id,
                                                           &axc_info_t));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rx_axc_frame_delete(
    int unit,
    bcm_gport_t port,
    int axc_id)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_rx_axc_frame_delete(unit, port,
                                                              axc_id));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rx_axc_frame_get(
    int unit,
    bcm_gport_t port,
    int axc_id,
    bcm_cpri_axc_frame_info_t *axc_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_axc_frame_info_t axc_info_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_rx_axc_frame_get(unit, port, axc_id,
                                                           0, &axc_info_t));
    _portmod_cpri_axc_frame_info_t_to_bcm_cpri_axc_frame_info_t(BCM_CPRI_DIR_RX, &axc_info_t, axc_info);
    axc_info->axc_id = axc_id;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rx_axc_frame_get_all(
    int unit,
    bcm_gport_t port,
    int max_count,
    bcm_cpri_axc_frame_info_t *axc_info_list,
    int *axc_count)
{
#ifdef CPRIMOD_SUPPORT
    int i;
    int tbl_entry;
    portmod_cpri_axc_frame_info_t axc_info_t;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    *axc_count = 0;
    tbl_entry=0;
    for (i=0; i<max_count && tbl_entry<MAX_NUM_OF_DATA_AXCS;) {
        BCM_IF_ERROR_RETURN(portmod_cpri_port_rx_axc_frame_get(unit, port,
                                                               tbl_entry,
                                                               1,
                                                               &axc_info_t));
        if (axc_info_t.start_bit != 0 && axc_info_t.num_bits != 0) {
            _portmod_cpri_axc_frame_info_t_to_bcm_cpri_axc_frame_info_t(BCM_CPRI_DIR_RX, &axc_info_t,
                                                                        axc_info_list);
            axc_info_list->axc_id = tbl_entry;
            axc_info_list++;
            ++(*axc_count);
            i++;
        }
        tbl_entry++;
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rx_speed_get(
    int unit,
    bcm_gport_t port,
    bcm_cpri_port_type_t *port_type,
    bcm_cpri_port_speed_t *speed)
{
#ifdef CPRIMOD_SUPPORT
    portmod_port_interface_config_t config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_port_interface_config_get(unit, port, &config,
                                                          PORTMOD_INIT_F_RX_ONLY));
    *speed = _int_to_bcm_cpri_port_speed_t(config.speed);
    *port_type = bcmCpriPortTypeCpri;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rx_speed_set(
    int unit,
    bcm_gport_t port,
    bcm_cpri_port_type_t port_type,
    bcm_cpri_port_speed_t speed)
{
#ifdef CPRIMOD_SUPPORT
    portmod_port_interface_config_t config;

    if (port_type != bcmCpriPortTypeCpri) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    config.speed = _bcm_cpri_port_speed_t_to_int(speed);
    config.encap_mode = _SHR_PORT_ENCAP_CPRI;
    BCM_IF_ERROR_RETURN(portmod_port_interface_config_set(unit, port, &config,
                                                          PORTMOD_INIT_F_RX_ONLY));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_stage_activate(
    int unit,
    bcm_gport_t port,
    bcm_cpri_stage_t stage)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_stage_t stage_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (stage) {
    case bcmCpriStageRxParser:
        stage_t = portmodCpriStageRx;
        break;
    case bcmCpriStageTxAssembler:
        stage_t = portmodCpriStageTx;
        break;
    default:
        stage_t = portmodCpriStageRx;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_cpri_port_stage_activate(unit, port, stage_t));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_axc_frame_add(
    int unit,
    bcm_gport_t port,
    int axc_id,
    bcm_cpri_axc_frame_info_t *axc_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_axc_frame_info_t axc_info_t;
    portmod_cpri_tx_agnostic_config_info_t agnostic_config;
    int agnostic_enable;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_agnostic_config_get(unit, port,
                                                  &agnostic_enable,
                                                  &agnostic_config));

    if (agnostic_enable) {
        LOG_ERROR(BSL_LS_BCM_CPRI,
                  (BSL_META("AxC cannot added while Agnostic Mode is Enabled.\n")));
        return BCM_E_CONFIG;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_cpri_axc_frame_info_t_to_portmod_cpri_axc_frame_info_t(axc_id,
                                                                     BCM_CPRI_DIR_TX,
                                                                     axc_info,
                                                                     &axc_info_t));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_tx_axc_frame_add(unit, port, axc_id,
                                                           &axc_info_t));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_axc_frame_delete(
    int unit,
    bcm_gport_t port,
    int axc_id)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_tx_axc_frame_delete(unit, port,
                                                              axc_id));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_axc_frame_get(
    int unit,
    bcm_gport_t port,
    int axc_id,
    bcm_cpri_axc_frame_info_t *axc_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_axc_frame_info_t axc_info_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_tx_axc_frame_get(unit, port, axc_id,
                                                           0, &axc_info_t));
    _portmod_cpri_axc_frame_info_t_to_bcm_cpri_axc_frame_info_t(BCM_CPRI_DIR_TX, &axc_info_t, axc_info);
    axc_info->axc_id = axc_id;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_axc_frame_get_all(
    int unit,
    bcm_gport_t port,
    int max_count,
    bcm_cpri_axc_frame_info_t *axc_info_list,
    int *axc_count)
{
#ifdef CPRIMOD_SUPPORT
    int i;
    int tbl_entry;
    portmod_cpri_axc_frame_info_t axc_info_t;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    *axc_count = 0;
    tbl_entry=0;
    for (i=0; i<max_count && tbl_entry<MAX_NUM_OF_DATA_AXCS;) {
        BCM_IF_ERROR_RETURN(portmod_cpri_port_tx_axc_frame_get(unit, port,
                                                               tbl_entry,
                                                               1,
                                                               &axc_info_t));
        if (axc_info_t.start_bit != 0 && axc_info_t.num_bits != 0) {
            _portmod_cpri_axc_frame_info_t_to_bcm_cpri_axc_frame_info_t(BCM_CPRI_DIR_TX, &axc_info_t,
                                                                        axc_info_list);
            axc_info_list->axc_id = tbl_entry;
            axc_info_list++;
            ++(*axc_count);
            i++;
        }
        tbl_entry++;
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_frame_tgen_config_set(
    int unit,
    bcm_gport_t port,
    int enable,
    bcm_cpri_tx_frame_tgen_config_t *tgen_config)
{
#ifdef CPRIMOD_SUPPORT

    portmod_tx_frame_tgen_config_t tgen_cfg_t;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if (enable) {
        tgen_cfg_t.tx_tgen_hfn = tgen_config->tx_tgen_hfn ;
        tgen_cfg_t.tx_tgen_bfn = tgen_config->tx_tgen_bfn;
        tgen_cfg_t.tx_gen_bfn_hfn_sel = tgen_config->tx_tgen_bfn_sel;
        COMPILER_64_COPY(tgen_cfg_t.tx_tgen_ts_offset, tgen_config->tx_tgen_ts_offset);
        BCM_IF_ERROR_RETURN
            (portmod_cpri_port_tx_frame_tgen_config_set(unit, port, &tgen_cfg_t));
    }

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_frame_tgen_enable(unit, port, enable));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_speed_get(
    int unit,
    bcm_gport_t port,
    bcm_cpri_port_type_t *port_type,
    bcm_cpri_port_speed_t *speed)
{
#ifdef CPRIMOD_SUPPORT
    portmod_port_interface_config_t config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_port_interface_config_get(unit, port, &config,
                                                          PORTMOD_INIT_F_TX_ONLY));
    *speed = _int_to_bcm_cpri_port_speed_t(config.speed);
    *port_type = bcmCpriPortTypeCpri;
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_speed_set(
    int unit,
    bcm_gport_t port,
    bcm_cpri_port_type_t port_type,
    bcm_cpri_port_speed_t speed)
{
#ifdef CPRIMOD_SUPPORT
    portmod_port_interface_config_t config;

    if (port_type != bcmCpriPortTypeCpri) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    config.speed = _bcm_cpri_port_speed_t_to_int(speed);
    config.encap_mode = _SHR_PORT_ENCAP_CPRI;
    BCM_IF_ERROR_RETURN(portmod_port_interface_config_set(unit, port, &config,
                                                          PORTMOD_INIT_F_TX_ONLY));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Add control message flow. */
int bcm_esw_cpri_port_rsvd4_rx_control_flow_add(
        int unit,
        int port,
        int control_flow_id,
        bcm_rsvd4_control_flow_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_control_flow_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_control_flow_config_t_init(unit, &portmod_config);

    portmod_config.match_mask   = config->match_mask ;
    portmod_config.match_data   = config->match_data ;
    portmod_config.proc_type    = config->type ;
    portmod_config.queue_num    = config->queue_num ;
    portmod_config.sync_profile = config->sync_profile ;
    portmod_config.sync_enable  = config->sync_en ;
    portmod_config.priority     = config->priority ;
    portmod_config.queue_size   = config->queue_size ;

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_rx_control_flow_add(unit, port, control_flow_id, &portmod_config));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Delete control message flow. */
int bcm_esw_cpri_port_rsvd4_rx_control_flow_delete(
    int unit,
    int port,
    int control_flow_id,
    bcm_rsvd4_control_flow_config_t *control_flow_cfg)
{

#ifdef CPRIMOD_SUPPORT
    portmod_control_flow_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_control_flow_config_t_init(unit, &portmod_config);

    portmod_config.match_mask   = control_flow_cfg->match_mask ;
    portmod_config.match_data   = control_flow_cfg->match_data ;
    portmod_config.proc_type    = control_flow_cfg->type ;
    portmod_config.queue_num    = control_flow_cfg->queue_num ;

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_rx_control_flow_delete(unit, port, control_flow_id, &portmod_config));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Setup the fast ethernet control flow. */
int bcm_esw_cpri_port_rsvd4_cm_fast_eth_config_set(
    int unit,
    int port,
    int control_flow_id,
    bcm_rsvd4_fast_eth_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_fast_eth_config_t int_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    int_config.queue_num        = config->queue_num;
    int_config.no_fcs_err_check = config->no_fcs_err_check;
    int_config.min_packet_size  = config->min_packet_size;
    int_config.max_packet_size  = config->max_packet_size;
    int_config.min_packet_drop  = config->min_packet_drop;
    int_config.max_packet_drop  = config->max_packet_drop;
    int_config.strip_crc        = config->strip_crc;

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_fast_eth_config_set(unit, port, &int_config));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Configure sync profile entry. */
int bcm_esw_cpri_port_rsvd4_sync_profile_entry_set(
    int unit,
    int port,
    int index,
    bcm_rsvd4_sync_profile_entry_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_sync_profile_entry_t entry;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_sync_profile_entry_t_init(unit, &entry);
    entry.count_cycle           = config->count_cycle;
    entry.message_offset        = config->message_offset;
    entry.master_frame_offset   = config->master_frame_offset;

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_rx_sync_profile_entry_set(unit, port, index, &entry));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Retrieve sync profile entry. */
int bcm_esw_cpri_port_rsvd4_sync_profile_entry_get(
    int unit,
    int port,
    int index,
    bcm_rsvd4_sync_profile_entry_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_sync_profile_entry_t entry;

    portmod_sync_profile_entry_t_init(unit, &entry);

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_rx_sync_profile_entry_get(unit, port, index, &entry));

    config->count_cycle         = entry.count_cycle;
    config->message_offset      = entry.message_offset;
    config->master_frame_offset = entry.master_frame_offset;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_control_flow_group_member_add(
    int unit,
    int port,
    bcm_rsvd4_control_group_id_t group_num,
    bcm_rsvd4_tx_control_flow_group_config_t *config)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_control_flow_group_member_add(unit, port, group_num, config->priority, config->queue_num, config->type));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_control_flow_group_member_delete (
    int unit,
    int port,
    bcm_rsvd4_control_group_id_t group_num,
    bcm_rsvd4_priority_t priority)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_control_flow_group_member_delete(unit, port, group_num, priority));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_cm_eth_config_set(
    int unit,
    int port,
    bcm_rsvd4_tx_cm_eth_config_t *config)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_eth_message_config_set(unit, port, config->msg_node, config->msg_subnode, config->msg_type, config->msg_padding));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_cm_raw_config_set(
    int unit,
    int port,
    bcm_rsvd4_tx_cm_raw_config_t *config)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_single_raw_message_config_set(unit, port,  config->msg_id, config->msg_type));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_control_flow_add(
    int unit,
    int port,
    int control_flow_id,
    bcm_rsvd4_tx_control_flow_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_rsvd4_tx_config_info_t config_info;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_rsvd4_tx_config_info_t_init(unit, &config_info);

    config_info.proc_type = config->type;
    config_info.queue_num = config->queue_num;
    config_info.queue_size = config->queue_size;
    config_info.cycle_size = config->cycle_size;
    config_info.crc_mode = config->crc_mode;


    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_control_flow_config_set(unit, port, control_flow_id, &config_info));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/*  */
int bcm_esw_cpri_port_rsvd4_tx_cm_tunnel_crc_config_set(
    int unit,
    int port,
    bcm_rsvd4_cm_tunnel_crc_option_t crc_option)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_single_tunnel_message_config_set(unit, port, crc_option));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_cm_hdr_config_entry_set(
    int unit,
    int port,
    int index,
    bcm_rsvd4_tx_cm_hdr_entry_t* entry)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_control_header_entry_set(unit, port, index, entry->rsvd4_header_node,
                                                   entry->rsvd4_header_subnode, entry->rsvd4_control_payload_node));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_cm_hdr_lkup_entry_set(
    int unit,
    int port,
    int flow_id,
    int header_index)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_control_flow_header_index_set(unit, port, flow_id, header_index));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rx_cw_sync_info_get(
    int unit,
    int port,
    uint32* hyper_frame_num,
    uint32* radio_frame_num)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_cw_sync_info_get(unit, port, hyper_frame_num, radio_frame_num));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_l1_inband_info_get(
    int unit,
    int port,
    bcm_cpri_cw_l1_inband_info_t *l1_inband_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_cw_l1_inband_info_t l1_data;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_cw_l1_inband_info_get(unit, port, &l1_data));
    l1_inband_info->protocol_ver       = l1_data.protocol_ver;
    l1_inband_info->hdlc_rate          = l1_data.hdlc_rate;
    l1_inband_info->layer1_function    = l1_data.layer1_function;
    l1_inband_info->eth_ptr            = l1_data.eth_ptr;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_l1_signal_protection_set(
    int unit,
    int port,
    uint32 signal_map,
    uint32 enable)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_cw_l1_signal_signal_protection_set(unit, port, signal_map, enable));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_l1_signal_protection_get(
    int unit,
    int port,
    uint32 signal_map,
    uint32 *enable)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_cw_l1_signal_signal_protection_get(unit, port, signal_map, enable));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_cw_slow_hdlc_config_set(
    int unit,
    int port,
    bcm_cpri_slow_hdlc_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_slow_hdlc_config_info_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_slow_hdlc_config_info_t_init(unit, &portmod_config);

    portmod_config.tx_crc_mode      = config->hdlc_crc_mode;
    portmod_config.tx_flag_size     = config->tx_flag_size;
    portmod_config.crc_init_val     = config->crc_init_val;
    portmod_config.use_fe_mac       = config->use_fe_mac;
    portmod_config.crc_byte_swap    = config->crc_byte_swap;
    portmod_config.no_fcs_err_check = config->no_fcs_err_check;
    portmod_config.cw_sel           = config->hdlc_cw_sel;
    portmod_config.cw_size          = config->hdlc_cw_size;
    portmod_config.fcs_size         = config->hdlc_fcs_size;
    portmod_config.runt_frame_drop  = config->hdlc_run_drop;
    portmod_config.long_frame_drop  = config->hdlc_max_drop;
    portmod_config.min_frame_size   = config->hdlc_min_size;
    portmod_config.max_frame_size   = config->hdlc_max_size;
    portmod_config.queue_num        = config->hdlc_queue_num;
    portmod_config.priority         = config->priority;
    portmod_config.queue_size       = config->queue_size;
    portmod_config.cycle_size       = config->cycle_size;
    portmod_config.buffer_size      = config->buffer_size;

    portmod_config.tx_filling_flag_pattern  = config->tx_filling_pattern;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_cw_slow_hdlc_config_set(unit, port, &portmod_config));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_cw_slow_hdlc_config_get(
    int unit,
    int port,
    bcm_cpri_slow_hdlc_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_slow_hdlc_config_info_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_slow_hdlc_config_info_t_init(unit, &portmod_config);
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_cw_slow_hdlc_config_get(unit, port, &portmod_config));

    config->hdlc_crc_mode       = portmod_config.tx_crc_mode;
    config->tx_flag_size        = portmod_config.tx_flag_size;
    config->crc_init_val        = portmod_config.crc_init_val;
    config->use_fe_mac          = portmod_config.use_fe_mac;
    config->crc_byte_swap       = portmod_config.crc_byte_swap;
    config->no_fcs_err_check    = portmod_config.no_fcs_err_check;
    config->hdlc_cw_sel         = portmod_config.cw_sel;
    config->hdlc_cw_size        = portmod_config.cw_size;
    config->hdlc_fcs_size       = portmod_config.fcs_size;
    config->hdlc_run_drop       = portmod_config.runt_frame_drop;
    config->hdlc_max_drop       = portmod_config.long_frame_drop;
    config->hdlc_min_size       = portmod_config.min_frame_size;
    config->hdlc_max_size       = portmod_config.max_frame_size;
    config->hdlc_queue_num      = portmod_config.queue_num;
    config->priority            = portmod_config.priority;
    config->queue_size          = portmod_config.queue_size;
    config->cycle_size          = portmod_config.cycle_size;
    config->buffer_size         = portmod_config.buffer_size;

    config->tx_filling_pattern  = portmod_config.tx_filling_flag_pattern;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_cw_fast_eth_config_set(
    int unit,
    int port,
    bcm_cpri_fast_eth_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_fast_eth_config_info_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_fast_eth_config_info_t_init(unit, &portmod_config);

    portmod_config.tx_crc_mode      = config->crc_mode;
    portmod_config.schan_start      = config->sub_channel_start;
    portmod_config.schan_size       = config->sub_channel_size;
    portmod_config.cw_sel           = config->cw_sel;
    portmod_config.cw_size          = config->cw_size;
    portmod_config.min_packet_size  = config->min_pkt_size;
    portmod_config.max_packet_size  = config->max_pkt_size;
    portmod_config.min_packet_drop  = config->drop_undersize_pkt;
    portmod_config.max_packet_drop  = config->drop_oversize_pkt;
    portmod_config.strip_crc        = config->strip_crc;
    portmod_config.min_ipg          = config->min_ipg;
    portmod_config.queue_num        = config->queue_num;
    portmod_config.priority         = config->priority;
    portmod_config.queue_size       = config->queue_size;
    portmod_config.cycle_size       = config->cycle_size;
    portmod_config.buffer_size      = config->buffer_size;
    portmod_config.no_fcs_err_check = config->ignore_fcs_err;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_cw_fast_eth_config_set(unit, port, &portmod_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_cw_fast_eth_config_get(
    int unit,
    int port,
    bcm_cpri_fast_eth_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_fast_eth_config_info_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_fast_eth_config_info_t_init(unit, &portmod_config);

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_cw_fast_eth_config_get(unit, port, &portmod_config));

    config->crc_mode            = portmod_config.tx_crc_mode;
    config->sub_channel_start   = portmod_config.schan_start;
    config->sub_channel_size    = portmod_config.schan_size;
    config->cw_sel              = portmod_config.cw_sel;
    config->cw_size             = portmod_config.cw_size;
    config->min_pkt_size        = portmod_config.min_packet_size;
    config->max_pkt_size        = portmod_config.max_packet_size;
    config->drop_undersize_pkt  = portmod_config.min_packet_drop;
    config->drop_oversize_pkt  = portmod_config.max_packet_drop;
    config->strip_crc           = portmod_config.strip_crc;
    config->min_ipg             = portmod_config.min_ipg;
    config->queue_num           = portmod_config.queue_num;
    config->priority            = portmod_config.priority;
    config->queue_size          = portmod_config.queue_size;
    config->cycle_size          = portmod_config.cycle_size;
    config->buffer_size         = portmod_config.buffer_size;
    config->ignore_fcs_err      = portmod_config.no_fcs_err_check;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_vsd_config_set(
    int unit,
    int port,
    bcm_cpri_rx_vsd_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_vsd_config_info_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_rx_vsd_config_info_t_init(unit,&portmod_config);
    portmod_config.subchan_start          = config->subchan_start;
    portmod_config.subchan_size           = config->subchan_size;
    portmod_config.subchan_bytes          = config->subchan_bytes;
    portmod_config.subchan_steps          = config->subchan_step;
    portmod_config.flow_bytes           = config->flow_bytes;
    portmod_config.queue_num            = config->queue_num;
    portmod_config.rsvd_sector_mask[0]  = config->vsd_valid_sector_mask[0];
    portmod_config.rsvd_sector_mask[1]  = config->vsd_valid_sector_mask[1];
    portmod_config.rsvd_sector_mask[2]  = config->vsd_valid_sector_mask[2];
    portmod_config.rsvd_sector_mask[3]  = config->vsd_valid_sector_mask[3];
    portmod_config.queue_size           = config->queue_size;
    portmod_config.priority             = config->priority;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_vsd_config_set(unit, port, &portmod_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}


int bcm_esw_cpri_port_rx_cw_vsd_config_get(
    int unit,
    int port,
    bcm_cpri_rx_vsd_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_vsd_config_info_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_rx_vsd_config_info_t_init(unit,&portmod_config);

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_vsd_config_get(unit, port, &portmod_config));

    config->subchan_start   = portmod_config.subchan_start;
    config->subchan_size    = portmod_config.subchan_size;
    config->subchan_bytes   = portmod_config.subchan_bytes;
    config->subchan_step    = portmod_config.subchan_steps;
    config->flow_bytes      = portmod_config.flow_bytes;
    config->queue_num       = portmod_config.queue_num;
    config->queue_size      = portmod_config.queue_size;
    config->priority        = portmod_config.priority;

    config->vsd_valid_sector_mask[0] = portmod_config.rsvd_sector_mask[0];
    config->vsd_valid_sector_mask[1] = portmod_config.rsvd_sector_mask[1];
    config->vsd_valid_sector_mask[2] = portmod_config.rsvd_sector_mask[2];
    config->vsd_valid_sector_mask[3] = portmod_config.rsvd_sector_mask[3];

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_vsd_ctrl_flow_add(
    int unit,
    int port,
    uint16 group_id,
    bcm_cpri_rx_vsd_flow_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_vsd_flow_info_t flow_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    flow_config.hyper_frame_number  = config->hyper_frame_number;
    flow_config.hyper_frame_modulo  = config->hyper_frame_modulo;
    flow_config.filter_zero_data    = config->filter_zero_data;
    flow_config.section_num[0]      = config->section_num[0];
    flow_config.section_num[1]      = config->section_num[1];
    flow_config.section_num[2]      = config->section_num[2];
    flow_config.section_num[3]      = config->section_num[3];
    flow_config.num_sector          = config->num_sector;
    flow_config.tag_id              = config->tag_id;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_vsd_flow_add(unit, port, group_id, &flow_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_vsd_ctrl_flow_delete(
    int unit,
    int port,
    uint16 group_id) {
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_vsd_flow_delete(unit, port, group_id));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_vsd_ctrl_flow_get(
    int unit,
    int port,
    uint16 group_id,
    bcm_cpri_rx_vsd_flow_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
   portmod_cpri_rx_vsd_flow_info_t flow_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_vsd_flow_get(unit, port, group_id, &flow_config));

    config->hyper_frame_number  = flow_config.hyper_frame_number;
    config->hyper_frame_modulo  = flow_config.hyper_frame_modulo;
    config->filter_zero_data    = flow_config.filter_zero_data;
    config->section_num[0]      = flow_config.section_num[0];
    config->section_num[1]      = flow_config.section_num[1];
    config->section_num[2]      = flow_config.section_num[2];
    config->section_num[3]      = flow_config.section_num[3];
    config->num_sector          = flow_config.num_sector;
    config->tag_id              = flow_config.tag_id;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_encap_control_queue_tag_to_flow_id_map_set(
    int unit,
    int port,
    uint32 tag_id,
    uint32 flow_id)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_encap_queue_control_tag_to_flow_map_set(unit, port, tag_id, flow_id));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_vsd_raw_config_set(
    int unit,
    int port,
    uint8 vsd_raw_id,
    bcm_cpri_rx_vsd_raw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_vsd_raw_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_rx_vsd_raw_config_t_init(unit,&portmod_config);

    portmod_config.schan_start          = config->schan_start;
    portmod_config.schan_size           = config->schan_size;
    portmod_config.cw_sel               = config->cw_sel;
    portmod_config.cw_size              = config->cw_size;
    portmod_config.filter_mode          = config->filter_mode;
    portmod_config.hyper_frame_index    = config->hfn_offset;
    portmod_config.hyper_frame_modulo   = config->hfn_modulo;
    portmod_config.match_offset         = config->match_offset;
    portmod_config.match_value          = config->match_value;
    portmod_config.match_mask           = config->match_mask;
    portmod_config.queue_num            = config->queue_num;
    portmod_config.priority             = config->priority;
    portmod_config.queue_size           = config->queue_size;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_vsd_raw_config_set(unit, port, vsd_raw_id, &portmod_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_vsd_raw_config_get(
    int unit,
    int port,
    uint8 vsd_raw_id,
    bcm_cpri_rx_vsd_raw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_vsd_raw_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_rx_vsd_raw_config_t_init(unit,&portmod_config);

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_vsd_raw_config_get(unit, port, vsd_raw_id, &portmod_config));

    config->schan_start = portmod_config.schan_start;
    config->schan_size  = portmod_config.schan_size;
    config->cw_sel      = portmod_config.cw_sel;
    config->cw_size     = portmod_config.cw_size;
    config->filter_mode = portmod_config.filter_mode;
    config->hfn_offset  = portmod_config.hyper_frame_index;
    config->hfn_modulo  = portmod_config.hyper_frame_modulo;
    config->match_offset= portmod_config.match_offset;
    config->match_value = portmod_config.match_value;
    config->match_mask  = portmod_config.match_mask;
    config->queue_num   = portmod_config.queue_num;
    config->priority    = portmod_config.priority;
    config->queue_size  = portmod_config.queue_size;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_rsvd5_config_set(
    int unit,
    int port,
    bcm_cpri_rx_rsvd5_config_info_t* config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_brcm_rsvd5_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_rx_brcm_rsvd5_config_t_init(unit,&portmod_config);
    portmod_config.schan_start      = config->schan_start ;
    portmod_config.schan_size       = config->schan_size ;
    portmod_config.parity_disable   = config->disable_parity_check ;
    portmod_config.queue_num        = config->queue_num;
    portmod_config.priority         = config->priority;
    portmod_config.queue_size       = config->queue_size;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_brcm_rsvd5_config_set(unit, port, &portmod_config));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_rsvd5_config_get(
    int unit,
    int port,
    bcm_cpri_rx_rsvd5_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_brcm_rsvd5_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_rx_brcm_rsvd5_config_t_init(unit,&portmod_config);

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_brcm_rsvd5_config_get(unit, port, &portmod_config));

    config->schan_start = portmod_config.schan_start;
    config->schan_size  = portmod_config.schan_size;
    config->disable_parity_check = portmod_config.parity_disable;
    config->queue_num   = portmod_config.queue_num;
    config->priority    = portmod_config.priority;
    config->queue_size  = portmod_config.queue_size;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_tag_entry_add(
    int unit,
    int port,
    bcm_cpri_tag_gen_entry_t *config)

{
#ifdef CPRIMOD_SUPPORT
    portmod_tag_gen_entry_t int_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    int_config.tag_id       = config->tag_id;
    int_config.mask         = config->mask;
    int_config.header       = config->header;
    int_config.word_count   = config->word_count;

    BCM_IF_ERROR_RETURN
        (portmod_rx_tag_gen_entry_add(unit, port, &int_config));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_tag_entry_delete(
    int unit,
    int port,
    bcm_cpri_tag_gen_entry_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_tag_gen_entry_t int_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    int_config.tag_id       = config->tag_id;
    int_config.mask         = config->mask;
    int_config.header       = config->header;
    int_config.word_count   = config->word_count;

    BCM_IF_ERROR_RETURN
        (portmod_rx_tag_gen_entry_delete(unit, port, &int_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_gcw_config_set(
    int unit,
    int port,
    uint8 index,
    bcm_cpri_rx_gcw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_gcw_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_rx_gcw_config_t_init(unit,&portmod_config);
    portmod_config.Ns       = config->Ns ;
    portmod_config.Xs       = config->Xs ;
    portmod_config.Y        = config->Y ;
    portmod_config.mask     = config->mask ;
    portmod_config.filter_mode          = config->filter_mode ;
    portmod_config.hyper_frame_index    = config->hfn_index ;
    portmod_config.hyper_frame_modulo   = config->hfn_modulo ;
    portmod_config.match_mask           = config->match_mask ;
    portmod_config.match_value          = config->match_value;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_gcw_config_set(unit, port, index, &portmod_config));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_gcw_config_get(
    int unit,
    int port,
    uint8 index,
    bcm_cpri_rx_gcw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_gcw_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_rx_gcw_config_t_init(unit,&portmod_config);

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_gcw_config_get(unit, port, index, &portmod_config));

    config->Ns      = portmod_config.Ns;
    config->Xs      = portmod_config.Xs;
    config->Y       = portmod_config.Y;
    config->mask    = portmod_config.mask;

    config->filter_mode = portmod_config.filter_mode;
    config->hfn_index   = portmod_config.hyper_frame_index;
    config->hfn_modulo  = portmod_config.hyper_frame_modulo;
    config->match_mask  = portmod_config.match_mask;
    config->match_value = portmod_config.match_value;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_sync_info_set(
    int unit,
    int port,
    bcm_cpri_cw_sync_info_t *entry)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_cw_sync_info_set(unit, port, entry->hfn, entry->bfn));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_sync_info_get(
    int unit,
    int port,
    bcm_cpri_cw_sync_info_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_cw_sync_info_get(unit, port, &entry->hfn, &entry->bfn));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}


int bcm_esw_cpri_port_tx_cw_l1_inband_info_set(
    int unit,
    int port,
    bcm_cpri_cw_l1_inband_info_t *inband_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_cw_l1_config_info_t int_entry;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (((inband_info->eth_ptr < 20) || (inband_info->eth_ptr > 63)) &&
        (inband_info->eth_ptr != 0)) {
        LOG_ERROR(BSL_LS_BCM_CPRI,
                 (BSL_META("eth_ptr p= %d Valid Eth Channel should be p= 0, 20..63\n"), inband_info->eth_ptr));
        return BCM_E_PARAM;
    }

    int_entry.eth_ptr           = inband_info->eth_ptr;
    int_entry.layer1_function   = inband_info->layer1_function;
    int_entry.hdlc_rate         = inband_info->hdlc_rate;
    int_entry.protocol_ver      = inband_info->protocol_ver;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_cw_l1_inband_info_set(unit, port, &int_entry));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_l1_inband_info_get(
    int unit,
    int port,
    bcm_cpri_cw_l1_inband_info_t *inband_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_cw_l1_inband_info_t int_entry;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_cw_l1_inband_info_get(unit, port, &int_entry));

    inband_info->eth_ptr           = int_entry.eth_ptr;
    inband_info->layer1_function   = int_entry.layer1_function;
    inband_info->hdlc_rate         = int_entry.hdlc_rate;
    inband_info->protocol_ver      = int_entry.protocol_ver;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_config_set(
    int unit,
    int port,
    bcm_cpri_tx_vsd_config_info_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_tx_vsd_config_info_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_tx_vsd_config_info_t_init(unit,&portmod_config);
    portmod_config.subchan_start        = entry->subchan_start;
    portmod_config.subchan_size         = entry->subchan_size;
    portmod_config.subchan_bytes        = entry->subchan_bytes;
    portmod_config.subchan_steps        = entry->subchan_step;
    portmod_config.flow_bytes           = entry->flow_bytes;
    portmod_config.queue_num            = entry->queue;
    portmod_config.rsvd_sector_mask[0]  = entry->vsd_valid_sector_mask[0];
    portmod_config.rsvd_sector_mask[1]  = entry->vsd_valid_sector_mask[1];
    portmod_config.rsvd_sector_mask[2]  = entry->vsd_valid_sector_mask[2];
    portmod_config.rsvd_sector_mask[3]  = entry->vsd_valid_sector_mask[3];
    portmod_config.buffer_size          = entry->buffer_size;
    portmod_config.cycle_size           = entry->cycle_size;
    portmod_config.byte_order_swap      = entry->byte_order_swap;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_vsd_config_set(unit, port, &portmod_config));


    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_config_get(
    int unit,
    int port,
    bcm_cpri_tx_vsd_config_info_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_tx_vsd_config_info_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_tx_vsd_config_info_t_init(unit,&portmod_config);

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_vsd_config_get(unit, port, &portmod_config));

    entry->subchan_start            = portmod_config.subchan_start;
    entry->subchan_size             = portmod_config.subchan_size;
    entry->subchan_bytes            = portmod_config.subchan_bytes;
    entry->subchan_step             = portmod_config.subchan_steps;
    entry->flow_bytes               = portmod_config.flow_bytes;
    entry->queue                    = portmod_config.queue_num;
    entry->vsd_valid_sector_mask[0] = portmod_config.rsvd_sector_mask[0];
    entry->vsd_valid_sector_mask[1] = portmod_config.rsvd_sector_mask[1];
    entry->vsd_valid_sector_mask[2] = portmod_config.rsvd_sector_mask[2];
    entry->vsd_valid_sector_mask[3] = portmod_config.rsvd_sector_mask[3];
    entry->buffer_size              = portmod_config.buffer_size;
    entry->cycle_size               = portmod_config.cycle_size;
    entry->byte_order_swap          = portmod_config.byte_order_swap;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_raw_flow_config_set(
    int unit,
    int port,
    uint8 index,
    bcm_cpri_tx_vsd_raw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT

    portmod_cpri_tx_vsd_raw_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_tx_vsd_raw_config_t_init(unit,&portmod_config);

    portmod_config.schan_start          = config->schan_start;
    portmod_config.schan_size           = config->schan_size;
    portmod_config.cw_sel               = config->cw_sel;
    portmod_config.cw_size              = config->cw_size;
    portmod_config.map_mode             = config->map_mode;
    portmod_config.repeat_enable        = config->repeat_mode;
    portmod_config.hyper_frame_index    = config->hfn_index;
    portmod_config.hyper_frame_modulo   = config->hfn_modulo;
    portmod_config.bfn0_filter_enable   = config->bfn0_filter_enable;
    portmod_config.bfn1_filter_enable   = config->bfn1_filter_enable;
    portmod_config.idle_value           = config->idle_value;
    portmod_config.queue_num            = config->queue_num;
    portmod_config.cycle_size           = config->cycle_size;
    portmod_config.buffer_size          = config->buffer_size;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_vsd_raw_config_set(unit, port, index, &portmod_config)) ;


    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_raw_flow_config_get(
    int unit,
    int port,
    uint8 index,
    bcm_cpri_tx_vsd_raw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_tx_vsd_raw_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_tx_vsd_raw_config_t_init(unit,&portmod_config);

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_vsd_raw_config_get(unit, port, index, &portmod_config)) ;


    config->schan_start        = portmod_config.schan_start;
    config->schan_size         = portmod_config.schan_size;
    config->cw_sel             = portmod_config.cw_sel;
    config->cw_size            = portmod_config.cw_size;
    config->map_mode           = portmod_config.map_mode;
    config->repeat_mode        = portmod_config.repeat_enable;
    config->hfn_index          = portmod_config.hyper_frame_index;
    config->hfn_modulo         = portmod_config.hyper_frame_modulo;
    config->bfn0_filter_enable = portmod_config.bfn0_filter_enable;
    config->bfn1_filter_enable = portmod_config.bfn1_filter_enable;
    config->idle_value         = portmod_config.idle_value;
    config->queue_num          = portmod_config.queue_num;
    config->cycle_size         = portmod_config.cycle_size;
    config->buffer_size        = portmod_config.buffer_size;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_raw_filter_set(
    int unit,
    int port,
    bcm_cpri_tx_cw_vsd_raw_filter_info_t *config)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_vsd_raw_filter_set(unit, port, config->bfn0_value,config->bfn0_mask, config->bfn1_value, config->bfn1_mask));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_raw_filter_get(
    int unit,
    int port,
    bcm_cpri_tx_cw_vsd_raw_filter_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_vsd_raw_filter_get(unit, port,
                                                 &config->bfn0_value, &config->bfn0_mask,
                                                 &config->bfn1_value, &config->bfn1_mask));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_rsvd5_config_set(
    int unit,
    int port,
    bcm_cpri_tx_cw_rsvd5_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_tx_brcm_rsvd5_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_tx_brcm_rsvd5_config_t_init(unit,&portmod_config);

    portmod_config.schan_start  = config->schan_start ;
    portmod_config.schan_size   = config->schan_size ;
    portmod_config.crc_enable   = config->crc_check_enable;
    portmod_config.queue_num    = config->queue_num;
    portmod_config.buffer_size  = config->buffer_size;
    portmod_config.cycle_size   = config->cycle_size;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_brcm_rsvd5_config_set(unit, port, &portmod_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_rsvd5_config_get(
    int unit,
    int port,
    bcm_cpri_tx_cw_rsvd5_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_tx_brcm_rsvd5_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_tx_brcm_rsvd5_config_t_init(unit,&portmod_config);

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_brcm_rsvd5_config_get(unit, port, &portmod_config));

    config->schan_start      =  portmod_config.schan_start;
    config->schan_size       =  portmod_config.schan_size;
    config->crc_check_enable =  portmod_config.crc_enable;
    config->queue_num        =  portmod_config.queue_num;
    config->buffer_size      =  portmod_config.buffer_size;
    config->cycle_size       =  portmod_config.cycle_size;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_gcw_config_set(
    int unit,
    int port,
    uint8 index,
    bcm_cpri_tx_gcw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_tx_gcw_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_tx_gcw_config_t_init(unit,&portmod_config);
    portmod_config.Ns       = config->Ns ;
    portmod_config.Xs       = config->Xs ;
    portmod_config.Y        = config->Y ;
    portmod_config.mask     = config->mask ;
    portmod_config.repeat_enable        = config->repeat_mode ;
    portmod_config.hyper_frame_index    = config->hfn_index ;
    portmod_config.hyper_frame_modulo   = config->hfn_modulo ;
    portmod_config.bfn0_filter_enable   = config->bfn0_filter_enable;
    portmod_config.bfn1_filter_enable   = config->bfn1_filter_enable;


    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_gcw_config_set(unit, port, index, &portmod_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_gcw_config_get(
    int unit,
    int port,
    uint8 index,
    bcm_cpri_tx_gcw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_tx_gcw_config_t portmod_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    portmod_cpri_tx_gcw_config_t_init(unit,&portmod_config);
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_gcw_config_get(unit, port, index, &portmod_config));

    config->Ns      = portmod_config.Ns;
    config->Xs      = portmod_config.Xs;
    config->Y       = portmod_config.Y;
    config->mask    = portmod_config.mask;

    config->repeat_mode         = portmod_config.repeat_enable;
    config->hfn_index           = portmod_config.hyper_frame_index;
    config->hfn_modulo          = portmod_config.hyper_frame_modulo;
    config->bfn0_filter_enable  = portmod_config.bfn0_filter_enable;
    config->bfn1_filter_enable  = portmod_config.bfn1_filter_enable;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_gcw_filter_set(
    int unit,
    int port,
    bcm_cpri_tx_gcw_tx_filter_info_t *config)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_gcw_filter_set(unit, port, config->bfn0_value,config->bfn0_mask, config->bfn1_value, config->bfn1_mask));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_gcw_filter_get(
    int unit,
    int port,
    bcm_cpri_tx_gcw_tx_filter_info_t *config)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_gcw_filter_get( unit, port,
                                              &config->bfn0_value, &config->bfn0_mask,
                                              &config->bfn1_value, &config->bfn1_mask));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_ctrl_flow_add(
    int unit,
    int port,
    uint16 group_id,
    bcm_cpri_tx_vsd_flow_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_tx_vsd_flow_info_t flow_config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    flow_config.hyper_frame_number  = config->hyper_frame_number;
    flow_config.hyper_frame_modulo  = config->hyper_frame_modulo;
    flow_config.repeat_mode         = config->repeat_mode;
    flow_config.section_num[0]      = config->section_num[0];
    flow_config.section_num[1]      = config->section_num[1];
    flow_config.section_num[2]      = config->section_num[2];
    flow_config.section_num[3]      = config->section_num[3];
    flow_config.num_sector          = config->num_sector;
    flow_config.roe_flow_id         = config->roe_flow_id;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_vsd_flow_add(unit, port, group_id, &flow_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_ctrl_flow_delete(
    int unit,
    int port,
    uint16 group_id) {
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_vsd_flow_delete(unit, port, group_id));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_ctrl_flow_get(
    int unit,
    int port,
    uint16 group_id,
    bcm_cpri_tx_vsd_flow_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_tx_vsd_flow_info_t flow_config;


    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_vsd_flow_get(unit, port, group_id, &flow_config));

    config->hyper_frame_number  = flow_config.hyper_frame_number;
    config->hyper_frame_modulo  = flow_config.hyper_frame_modulo;
    config->repeat_mode         = flow_config.repeat_mode;
    config->section_num[0]      = flow_config.section_num[0];
    config->section_num[1]      = flow_config.section_num[1];
    config->section_num[2]      = flow_config.section_num[2];
    config->section_num[3]      = flow_config.section_num[3];
    config->num_sector          = flow_config.num_sector;
    config->roe_flow_id         = flow_config.roe_flow_id;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_tag_config_set(
    int unit,
    int port,
    bcm_cpri_control_tag_config_t *config)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_rx_tag_config_set(unit, port, config->default_tag, config->no_match_tag));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Register a callback routine for CPRI port interrupts. */
int bcm_esw_cpri_port_interrupt_callback_register(
    int unit,
    bcm_cpri_interrupt_type_t cpri_intr_type,
    bcm_cpri_port_interrupt_callback_t callback,
    void *cb_data)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_port_interrupt_callback_register(unit,
                                                  cpri_intr_type,
                                                  callback,
                                                  cb_data);
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Unregister a callback routine registered for CPRI port interrupts. */
int bcm_esw_cpri_port_interrupt_callback_unregister(
    int unit,
    bcm_cpri_interrupt_type_t cpri_intr_type,
    bcm_cpri_port_interrupt_callback_t callback)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_port_interrupt_callback_unregister(unit,
                                                    cpri_intr_type,
                                                    callback);;
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Set Enable/Disable CPRI port interrupts base on type and index.. */
int bcm_esw_cpri_port_interrupt_enable_set(
    int unit,
    bcm_port_t port,
    bcm_cpri_interrupt_type_t cpri_intr_type, int data,
    int enable)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    return (portmod_cpri_port_interrupt_enable_set(unit, port, cpri_intr_type,
                                                   data, enable));
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Get Enable/Disable  CPRI port interrupts base on type and index. */
int bcm_esw_cpri_port_interrupt_enable_get(
    int unit,
    bcm_port_t port,
    bcm_cpri_interrupt_type_t cpri_intr_type,
    int data,
    int *enable)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    return (portmod_cpri_port_interrupt_enable_get(unit, port, cpri_intr_type,
                                                   data, enable));
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Set CPRI port 1588 timestamp capture configuration. */
int bcm_esw_cpri_port_1588_timestamp_capture_config_set(
    int unit,
    bcm_port_t port,
    int direction,
    bcm_cpri_1588_capture_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_1588_capture_config_t cfg;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    cfg.mode = config->mode;
    cfg.capture_bfn_num = config->capture_bfn_num;
    cfg.capture_hfn_num = config->capture_hfn_num;

    BCM_IF_ERROR_RETURN(
       portmod_1588_timestamp_capture_config_set(unit, port, direction, &cfg));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Get CPRI port 1588 timestamp capture configuration. */
int bcm_esw_cpri_port_1588_timestamp_capture_config_get(
    int unit,
    bcm_port_t port,
    int direction,
    bcm_cpri_1588_capture_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_1588_capture_config_t cfg;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN(
       portmod_1588_timestamp_capture_config_get(unit, port, direction, &cfg));

    config->mode = cfg.mode;
    config->capture_bfn_num = cfg.capture_bfn_num;
    config->capture_hfn_num = cfg.capture_hfn_num;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* API to setup the agnostic mode and its configuration. */
int bcm_esw_cpri_port_agnostic_config_set(int unit, bcm_port_t port,
                                      int dir,
                                      int enable,
                                      bcm_cpri_port_agnostic_config_t *config)
{
#ifdef CPRIMOD_SUPPORT

    portmod_cpri_rx_agnostic_config_info_t rx_config;
    portmod_cpri_tx_agnostic_config_info_t tx_config;

    /*
     * convert the gport to local port.
     */
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (dir == BCM_CPRI_DIR_RX ) {
        portmod_cpri_rx_agnostic_config_info_t_init(unit, &rx_config);
        if (enable) {
            rx_config.sync_mode             = config->frame_sync_mode;
            rx_config.basic_frame_offset    = config->basic_frame_offset;
            rx_config.hyper_frame_offset    = config->hyper_frame_offset;
            rx_config.radio_frame_offset    = config->radio_frame_offset;
            rx_config.payload_multiple      = config->payload_multiple;
            rx_config.pres_mod_count        = config->presentation_mod_count;
            rx_config.ordering_info_option  = config->ordering_info_option;
        }
        BCM_IF_ERROR_RETURN
            (portmod_cpri_port_rx_agnostic_config_set(unit, port, enable, &rx_config));

    } else { /* BCM_CPRI_DIR_TX */
        portmod_cpri_tx_agnostic_config_info_t_init(unit, &tx_config);
        if (enable) {
            tx_config.sync_mode             = config->frame_sync_mode;
            tx_config.basic_frame_offset    = config->basic_frame_offset;
            tx_config.hyper_frame_offset    = config->hyper_frame_offset;
            tx_config.radio_frame_offset    = config->radio_frame_offset;
            tx_config.payload_multiple      = config->payload_multiple;
            tx_config.queue_mod_count       = config->tx_queue_mod_cnt;
            tx_config.pres_mod_count        = config->presentation_mod_count;
            tx_config.pres_mod_offset       = config->presentation_mod_offset;
            tx_config.ordering_info_option  = config->ordering_info_option;
        }
        BCM_IF_ERROR_RETURN
            (portmod_cpri_port_tx_agnostic_config_set(unit, port, enable, &tx_config));
    }

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* API to get the agnostic mode and its configuration. */
int bcm_esw_cpri_port_agnostic_config_get(int unit, bcm_port_t port,
                                      bcm_cpri_dir_t dir,
                                      int *enable,
                                      bcm_cpri_port_agnostic_config_t *config)
{
#ifdef CPRIMOD_SUPPORT

    portmod_cpri_rx_agnostic_config_info_t rx_config;
    portmod_cpri_tx_agnostic_config_info_t tx_config;
    /*
     * convert the gport to local port.
     */
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));


    if (dir == BCM_CPRI_DIR_RX ) {
        portmod_cpri_rx_agnostic_config_info_t_init(unit, &rx_config);
        BCM_IF_ERROR_RETURN
            (portmod_cpri_port_rx_agnostic_config_get(unit, port, enable, &rx_config));
        config->frame_sync_mode         = rx_config.sync_mode;
        config->basic_frame_offset      = rx_config.basic_frame_offset;
        config->hyper_frame_offset      = rx_config.hyper_frame_offset;
        config->radio_frame_offset      = rx_config.radio_frame_offset;
        config->payload_multiple        = rx_config.payload_multiple  ;
        config->presentation_mod_count  = rx_config.pres_mod_count;
        config->ordering_info_option    = rx_config.ordering_info_option;


    } else { /* BCM_CPRI_DIR_TX */
        portmod_cpri_tx_agnostic_config_info_t_init(unit, &tx_config);
        BCM_IF_ERROR_RETURN
            (portmod_cpri_port_tx_agnostic_config_get(unit, port, enable, &tx_config));

        config->frame_sync_mode         = tx_config.sync_mode;
        config->basic_frame_offset      = tx_config.basic_frame_offset;
        config->hyper_frame_offset      = tx_config.hyper_frame_offset;
        config->radio_frame_offset      = tx_config.radio_frame_offset;
        config->payload_multiple        = tx_config.payload_multiple  ;
        config->presentation_mod_count  = tx_config.pres_mod_count;

        config->tx_queue_mod_cnt        = tx_config.queue_mod_count;
        config->presentation_mod_offset = tx_config.pres_mod_offset;
        config->ordering_info_option    = tx_config.ordering_info_option;
    }

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* API to set the presentation attribute. */
int bcm_esw_cpri_encap_presentation_time_config_set(int unit, bcm_port_t port,
                     bcm_cpri_presentation_ts_attribute_t attribute,
                     bcm_cpri_presentation_time_t *time)
{
#ifdef CPRIMOD_SUPPORT
    /*
     * convert the gport to local port.
     */
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_encap_presentation_time_config_set(unit, port, attribute, time));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* API to get the presentation attribute. */
int bcm_esw_cpri_encap_presentation_time_config_get(int unit, bcm_port_t port,
                     bcm_cpri_presentation_ts_attribute_t attribute,
                     bcm_cpri_presentation_time_t *time)
{
#ifdef CPRIMOD_SUPPORT
    /*
     * convert the gport to local port.
     */
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_encap_presentation_time_config_get(unit, port, attribute, time));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}


/* API to configure the decap presentation time. */
int bcm_esw_cpri_decap_presentation_time_config_set(int unit, bcm_port_t port,
                             bcm_cpri_decap_presentation_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    /*
     * convert the gport to local port.
     */
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_decap_presentation_time_config_set(unit, port, config));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* API to retrieve the decap presentation time. */
int bcm_esw_cpri_decap_presentation_time_config_get(int unit, bcm_port_t port,
                             bcm_cpri_decap_presentation_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    /*
     * convert the gport to local port.
     */
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_decap_presentation_time_config_get(unit, port, config));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Set CPRI port 1588 timestamp capture configuration for FIFO and HW. */
int bcm_esw_cpri_port_1588_timestamp_config_set(int unit, bcm_port_t port,
                                            bcm_cpri_dir_t direction,
                                            bcm_cpri_1588_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_1588_time_t adj_time;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    adj_time.nano_sec = config->nano_sec;
    adj_time.sub_nano_sec = config->sub_nano_sec;
    adj_time.sign_bit = config->sign_bit;
    BCM_IF_ERROR_RETURN(
    portmod_1588_timestamp_adjust_set(unit, port, direction, &adj_time));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Get CPRI port 1588 timestamp capture configuration for FIFO and HW. */
int bcm_esw_cpri_port_1588_timestamp_config_get(int unit, bcm_port_t port,
                                            bcm_cpri_dir_t direction,
                                            bcm_cpri_1588_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_1588_time_t adj_time;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN(
    portmod_1588_timestamp_adjust_get(unit, port, direction, &adj_time));
    config->nano_sec = adj_time.nano_sec;
    config->sub_nano_sec = adj_time.sub_nano_sec;
    config->sign_bit = adj_time.sign_bit;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Set CPRI port 1588 timestamp parameter configuration. */
int bcm_esw_cpri_port_1588_timestamp_ts_config_set(int unit, bcm_port_t port,
                                bcm_cpri_dir_t direction,
                                bcm_cpri_1588_ts_type_t type,
                                bcm_cpri_1588_timestamp_ts_config_t *config)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if(type == BCM_CPRI_1588_TS_TYPE_FIFO) {
        if(config->enable) {
            BCM_IF_ERROR_RETURN(
            portmod_1588_timestamp_fifo_config_set(unit, port, direction,
            config->modulo_count));
        } else {
            BCM_IF_ERROR_RETURN(
            portmod_1588_timestamp_fifo_config_set(unit, port, direction, 0));
        }
    } else if(type == BCM_CPRI_1588_TS_TYPE_CMIC) {
        if(config->enable) {
            BCM_IF_ERROR_RETURN(
            portmod_1588_timestamp_cmic_config_set(unit, port, direction,
            config->modulo_count));
        } else {
            BCM_IF_ERROR_RETURN(
            portmod_1588_timestamp_cmic_config_set(unit, port, direction, 0));
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_CPRI,
                 (BSL_META("Bad timestamp type %d.\n"), port));
        return BCM_E_PARAM;
    }
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Get CPRI port 1588 timestamp parameter configuration. */
int bcm_esw_cpri_port_1588_timestamp_ts_config_get(int unit, bcm_port_t port,
                                bcm_cpri_dir_t direction,
                                bcm_cpri_1588_ts_type_t type,
                                bcm_cpri_1588_timestamp_ts_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    uint16 mod0;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if(type == BCM_CPRI_1588_TS_TYPE_FIFO) {
        BCM_IF_ERROR_RETURN(
        portmod_1588_timestamp_fifo_config_get(unit, port, direction,
        &mod0));
    } else if(type == BCM_CPRI_1588_TS_TYPE_CMIC) {
        BCM_IF_ERROR_RETURN(
        portmod_1588_timestamp_cmic_config_get(unit, port, direction,
        &mod0));
    } else {
        LOG_ERROR(BSL_LS_BCM_CPRI,
                 (BSL_META("Bad timestamp type %d.\n"), port));
        return BCM_E_PARAM;
    }
    config->modulo_count = mod0;
    if(mod0 != 0) {
        config->enable = 1;
    } else {
        config->enable = 0;
    }
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Set CPRI port 1588 timestamp parameter configuration. */
int bcm_esw_cpri_port_1588_timestamp_get(int unit, bcm_port_t port,
                                        bcm_cpri_dir_t direction,
                                        bcm_cpri_1588_ts_type_t type,
                                        int max_count,
                                        int *count,
                                        bcm_cpri_1588_timestamp_t *config)
{
#ifdef CPRIMOD_SUPPORT
    uint8 cap_cnt;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if((type != BCM_CPRI_1588_TS_TYPE_SW) &&
       (type != BCM_CPRI_1588_TS_TYPE_FIFO)) {
        LOG_ERROR(BSL_LS_BCM_CPRI,
                 (BSL_META("TS read only availabel for SW/FIFO type port %d.\n"), port));
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(
        portmod_1588_captured_timestamp_get(unit, port, direction, type,
        max_count, &cap_cnt, config));
    *count = cap_cnt;

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* API to retrieve the CPRI port stats. */
int bcm_esw_cpri_port_stat_get(int unit, bcm_gport_t port,
                               bcm_cpri_stat_type_t type, uint64 *val)
{
#ifdef CPRIMOD_SUPPORT
    soc_reg_t counter;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));

    switch (type) {
        case bcmCpriStatRxSymbolErr:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_SYMBOL_ERR;
            break;
        case bcmCpriStatRxSeedMismatch:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_SEED_VECTOR_MISMATCH;
            break;
        case bcmCpriStatRxStateErrTransition:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_STATE_TRANSITION_ERR;
            break;
        case bcmCpriStatTxVsdCtrlFlowIdErr:
            counter = SOC_COUNTER_NON_DMA_CPRI_TX_VSD_CTRL_PKT_FLOW_ID_ERR;
            break;
        case bcmCpriStatTxVsdCtrlPktSizeErr:
            counter = SOC_COUNTER_NON_DMA_CPRI_TX_VSD_CTRL_PKT_SIZE_ERR;
            break;
        case bcmCpriStatTxVsdRawPktSizeErr:
            counter = SOC_COUNTER_NON_DMA_CPRI_TX_VSD_RAW_PKT_SIZE_ERR;
            break;
        case bcmCpriStatTxRsvd5CrcErr:
            counter = SOC_COUNTER_NON_DMA_RSVD5_TX_DROP_CRC_ERR;
            break;
        case bcmCpriStatRxRsvd4SingleMsgCrcErr:
            counter = SOC_COUNTER_NON_DMA_RSVD4_RX_SINGLE_MSG_DROP_CRC_ERR;
            break;
        case bcmCpriStatRxTagLookupFailed:
            counter = SOC_COUNTER_NON_DMA_RSVD4_RX_TAG_LKUP_FAIL;
            break;
        case bcmCpriStatRxRsvd4SingleMsgCnt:
            counter = SOC_COUNTER_NON_DMA_RSVD4_RX_SINGLE_MSG_CTRL_STREAMS;
            break;
        case bcmCpriStatRxRsvd4MultiMsgCnt:
            counter = SOC_COUNTER_NON_DMA_RSVD4_RX_MULPILE_MSG_CTRL_STREAMS;
            break;
        case bcmCpriStatRxRsvd4MultiMsgSeqErr:
            counter = SOC_COUNTER_NON_DMA_RSVD4_RX_INCORRECT_TIME_STAMP_SEQ;
            break;
        case bcmCpriStatRxDataByteErr:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_DATA_BYTES_ERR;
            break;
        case bcmCpriStatRxRsvd4HeaderErr:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_RSVD4_HDR_MSG_ERR;
            break;
        case bcmCpriStatRxRsvd4HeaderNoMatch:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_RSVD4_MSG_HDR_LKUP_FAIL;
            break;
        case bcmCpriStatRxRsvd4MsgCnt:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_RSVD4_TOTAL_MSG_COUNT;
            break;
        case bcmCpriStatRxHfnErr:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_INCORRECT_HFN_EXTRACT ;
            break;
        case bcmCpriStatRxBfnErr:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_INCORRECT_BFN_EXTRACT;
            break;
        case bcmCpriStatRxCtrlWordErr:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_CTRL_WORD_ERR ;
            break;
        case bcmCpriStatRxGcwErr:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_GENERIC_CTRL_WORD_ERR;
            break;
        case bcmCpriStatRxVsdRawErr:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_VSD_RAW_PKT_DROP;
            break;
        case bcmCpriStatRxVsdCtrlErr:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_VSD_CTRL_PKT_DROP;
            break;
        case bcmCpriStatTxRsvd4DataMsgCnt:
            counter = SOC_COUNTER_NON_DMA_CPRI_TX_RSVD4_DATA_SLOT_DATA_MSG;
            break;
        case bcmCpriStatTxRsvd4DataEmptyMsgCnt:
            counter = SOC_COUNTER_NON_DMA_CPRI_TX_RSVD4_DATA_SLOT_EMPTY_MSG;
            break;
        case bcmCpriStatTxRsvd4CtrlMsgCnt:
            counter = SOC_COUNTER_NON_DMA_CPRI_TX_RSVD4_CTRL_SLOT_CTRL_MSG;
            break;
        case bcmCpriStatTxRsvd4CtrlEmptyMsgCnt:
            counter = SOC_COUNTER_NON_DMA_CPRI_TX_RSVD4_CTRL_SLOT_EMPTY_MSG;
            break;
        case bcmCpriStatRxRsvd5ParityErr:
            counter = SOC_COUNTER_NON_DMA_RSVD5_RX_MSG_PARITY_ERR;
            break;
        case bcmCpriStatRxHdlcErrFrame:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_HDLC_FRAME_ERR ;
            break;
        case bcmCpriStatRxHdlcGoodFrame:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_HDLC_FRAME_OK;
            break;
        case bcmCpriStatRxFastEthDroppedPkt:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_FAST_ETH_PKT_MAC_DROP;
            break;
        case bcmCpriStatRxFastEthGoodPkt:
            counter = SOC_COUNTER_NON_DMA_CPRI_RX_FAST_ETH_PKT_MAC_OK;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_CPRI,
                 (BSL_META("Bad Counter type %d.\n"), type));
            return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (soc_counter_get(unit, port, counter,0,val));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* API to enable/disable FEC Clause 91. */
int bcm_esw_cpri_port_fec_enable_set(int unit, bcm_gport_t port,
                                     bcm_cpri_dir_t dir, int enable)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_fec_enable_set(unit, port, dir, enable));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* API to get status if FEC Clause 91 is enabled. */
int bcm_esw_cpri_port_fec_enable_get(int unit, bcm_gport_t port,
                                     bcm_cpri_dir_t dir, int *enable)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_fec_enable_get(unit, port, dir, enable));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* API to set FEC timestamp config. */
int bcm_esw_cpri_port_fec_timestamp_config_set(int unit, bcm_gport_t port,
                                    bcm_cpri_dir_t dir,
                                    bcm_cpri_fec_timestamp_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_fec_timestamp_config_t cfg;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cfg.enable                    = config->enable;
    cfg.control_bit_offset_0      = config->control_bit_offset_0;
    cfg.control_bit_offset_1      = config->control_bit_offset_1;
    cfg.control_bit_offset_2      = config->control_bit_offset_2;
    cfg.control_bit_offset_3      = config->control_bit_offset_3;
    cfg.fec_fifo_ts_source_select = config->fec_fifo_ts_source_select;
    cfg.fec_cmic_ts_source_select = config->fec_cmic_ts_source_select;
    BCM_IF_ERROR_RETURN(portmod_cpri_port_fec_timestamp_config_set(unit, port, dir, &cfg));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* API to retrieve FEC timestamp config. */
int bcm_esw_cpri_port_fec_timestamp_config_get(int unit, bcm_gport_t port,
                                    bcm_cpri_dir_t dir,
                                    bcm_cpri_fec_timestamp_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_fec_timestamp_config_t cfg;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_fec_timestamp_config_get(unit, port, dir, &cfg));
    config->enable                    = cfg.enable;
    config->control_bit_offset_0      = cfg.control_bit_offset_0;
    config->control_bit_offset_1      = cfg.control_bit_offset_1;
    config->control_bit_offset_2      = cfg.control_bit_offset_2;
    config->control_bit_offset_3      = cfg.control_bit_offset_3;
    config->fec_fifo_ts_source_select = cfg.fec_fifo_ts_source_select;
    config->fec_cmic_ts_source_select = cfg.fec_cmic_ts_source_select;
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* API to set FEC aux config. */
int bcm_esw_cpri_port_fec_aux_config_set(int unit, bcm_gport_t port,
                                    bcm_cpri_fec_aux_config_t param_id,
                                    uint32 value)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_fec_aux_config_set(unit, port, param_id, value));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* API to retrieve FEC aux config. */
int bcm_esw_cpri_port_fec_aux_config_get(int unit, bcm_gport_t port,
                                    bcm_cpri_fec_aux_config_t param_id,
                                    uint32 *value)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_fec_aux_config_get(unit, port, param_id, value));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Get the FEC Statistic Counter */
int bcm_esw_cpri_port_fec_stat_get( int unit, bcm_gport_t port, 
                                    bcm_cpri_fec_stat_type_t type, 
                                    uint32 *counter_value)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_fec_stat_get(unit, port, type, counter_value));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Set and Program Compression Table. */
int bcm_esw_cpri_port_compression_lookup_table_set(
    int unit,
    bcm_gport_t port,
    int table_depth,
    uint32 *comp_table)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_compression_lookup_table_set(unit, port, table_depth, comp_table));
    return (BCM_E_NONE);
#else
     return (BCM_E_UNAVAIL);
#endif
}

/* Get Compression Table. */
int bcm_esw_cpri_port_compression_lookup_table_get(
    int unit,
    bcm_gport_t port,
    int max_depth,
    int *table_depth,
    uint32 *comp_table)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_compression_lookup_table_get(unit, port, max_depth, table_depth, comp_table));
    return (BCM_E_NONE);
#else
     return (BCM_E_UNAVAIL);
#endif
}

/* Set and Program DeCompression Table. */
int bcm_esw_cpri_port_decompression_lookup_table_set(
    int unit,
    bcm_gport_t port,
    int table_depth,
    uint32 *comp_table)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_decompression_lookup_table_set(unit, port, table_depth, comp_table));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Get DeCompression Table. */
int bcm_esw_cpri_port_decompression_lookup_table_get(
    int unit,
    bcm_gport_t port,
    int max_depth,
    int *table_depth,
    uint32 *comp_table)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_decompression_lookup_table_get(unit, port, max_depth, table_depth, comp_table));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Set Decapsulation Queue Flow Control configuration. */
int bcm_esw_cpri_port_decap_queue_flow_control_set(
    int unit,
    bcm_gport_t port,
    uint32 queue_num,
    uint32 xon_threshold,
    uint32 xoff_threshold,
    int enable) {
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_decap_queue_flow_control_set(unit, port,
                                                        queue_num,
                                                        enable,
                                                        xon_threshold,
                                                        xoff_threshold));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Get Decapsulation Queue Flow Control configuration. */
int bcm_esw_cpri_port_decap_queue_flow_control_get(
    int unit,
    bcm_gport_t port,
    uint32 queue_num,
    uint32 *xon_threshold,
    uint32 *xoff_threshold,
    int *enable){
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_decap_queue_flow_control_get(unit, port,
                                                        queue_num,
                                                        enable,
                                                        xon_threshold,
                                                        xoff_threshold));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Get Rx GCW word. */
int bcm_esw_cpri_port_rx_cw_gcw_word_get(
    int unit,
    bcm_port_t port,
    int group_index,
    uint16 *gcw_word) {
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_gcw_word_get(unit, port, group_index, gcw_word));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif

}

/* Set Tx GCW word. */
int bcm_esw_cpri_port_tx_cw_gcw_word_set(
    int unit,
    bcm_port_t port,
    int group_index,
    uint16 gcw_word) {

#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_gcw_word_set(unit, port, group_index, gcw_word));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif

}

/* Set tunnel Mode configuration. */
int bcm_esw_cpri_port_tunnel_mode_config_set(
    int unit,
    bcm_port_t port,
    bcm_cpri_dir_t direction,
    int enable,
    bcm_cpri_port_tunnel_config_t *config) {

#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_tunnel_mode_config_t rx_config;
    portmod_cpri_tx_tunnel_mode_config_t tx_config;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));

    if (direction == BCM_CPRI_DIR_RX ) {
        portmod_cpri_rx_tunnel_mode_config_t_init(unit, &rx_config);
        if(enable) {
            rx_config.payload_size          = config->payload_size;
            rx_config.pres_mod_count        = config->presentation_mod_count;
            rx_config.ordering_info_option  = config->ordering_info_option;
        }
        BCM_IF_ERROR_RETURN
            (portmod_cpri_port_rx_tunnel_mode_config_set(unit, port, enable, &rx_config));
    } else { /* BCM_CPRI_DIR_TX */
        portmod_cpri_tx_tunnel_mode_config_t_init(unit, &tx_config);
        if(enable) {
            tx_config.payload_size          = config->payload_size;
            tx_config.pres_mod_count        = config->presentation_mod_count;
            tx_config.pres_mod_offset       = config->presentation_mod_offset;
            tx_config.queue_mod_count       = config->tx_queue_mod_count;
            tx_config.ordering_info_option  = config->ordering_info_option;
        }
        BCM_IF_ERROR_RETURN
            (portmod_cpri_port_tx_tunnel_mode_config_set(unit, port, enable, &tx_config));
    }
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif

}

/* Get tunnel Mode configuration. */
int bcm_esw_cpri_port_tunnel_mode_config_get(
    int unit,
    bcm_port_t port,
    bcm_cpri_dir_t direction,
    int *enable,
    bcm_cpri_port_tunnel_config_t *config) {

#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_tunnel_mode_config_t rx_config;
    portmod_cpri_tx_tunnel_mode_config_t tx_config;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));

    if (direction == BCM_CPRI_DIR_RX ) {
        portmod_cpri_rx_tunnel_mode_config_t_init(unit, &rx_config);
        rx_config.ordering_info_option = 0;

        BCM_IF_ERROR_RETURN
            (portmod_cpri_port_rx_tunnel_mode_config_get(unit, port, enable, &rx_config));
        config->payload_size            = rx_config.payload_size;
        config->presentation_mod_count  = rx_config.pres_mod_count;
        config->ordering_info_option    = rx_config.ordering_info_option;
    } else { /* BCM_CPRI_DIR_TX */
        portmod_cpri_tx_tunnel_mode_config_t_init(unit, &tx_config);
        tx_config.ordering_info_option = 0;
        BCM_IF_ERROR_RETURN
            (portmod_cpri_port_tx_tunnel_mode_config_get(unit, port, enable, &tx_config));
        config->payload_size            = tx_config.payload_size;
        config->presentation_mod_count  = tx_config.pres_mod_count;
        config->presentation_mod_offset = tx_config.pres_mod_offset;
        config->tx_queue_mod_count      = tx_config.queue_mod_count;
        config->ordering_info_option    = tx_config.ordering_info_option;
    }
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif

}

/* Set RSVD4 Agnostic  Mode configuration. */
int bcm_esw_cpri_port_rsvd4_agnostic_mode_config_set(
    int unit,
    bcm_port_t port,
    bcm_cpri_dir_t direction,
    int enable,
    bcm_cpri_port_rsvd4_agnostic_config_t *config) {

#ifdef CPRIMOD_SUPPORT
    portmod_rsvd4_rx_agnostic_config_t rx_config;
    portmod_rsvd4_tx_agnostic_config_t tx_config;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));

    if (direction == BCM_CPRI_DIR_RX ) {
        portmod_rsvd4_rx_agnostic_config_t_init(unit, &rx_config);
        if(enable) {
            rx_config.frame_sync_mode       = config->sync_mode;
            rx_config.master_frame_offset   = config->master_frame_offset;
            rx_config.message_group_number  = config->message_group_offset;
            rx_config.payload_multiple      = config->payload_multiple;
            rx_config.pres_mod_count        = config->presentation_mod_count;
            rx_config.ordering_info_option  = config->ordering_info_option;
        }
        BCM_IF_ERROR_RETURN
            (portmod_rsvd4_port_rx_agnostic_config_set(unit, port, enable, &rx_config));
    } else { /* BCM_CPRI_DIR_TX */
        portmod_rsvd4_tx_agnostic_config_t_init(unit, &tx_config);
        if(enable) {
            tx_config.frame_sync_mode       = config->sync_mode;
            tx_config.master_frame_offset   = config->master_frame_offset;
            tx_config.message_group_number  = config->message_group_offset;
            tx_config.payload_multiple      = config->payload_multiple;
            tx_config.pres_mod_count        = config->presentation_mod_count;
            tx_config.pres_mod_offset       = config->presentation_mod_offset;
            tx_config.queue_mod_count       = config->tx_queue_mod_count;
            tx_config.ordering_info_option  = config->ordering_info_option;
        }
        BCM_IF_ERROR_RETURN
            (portmod_rsvd4_port_tx_agnostic_config_set(unit, port, enable, &tx_config));
    }
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif

}

/* Get RSVD4 Agnostic  Mode configuration. */
int bcm_esw_cpri_port_rsvd4_agnostic_mode_config_get(
    int unit,
    bcm_port_t port,
    bcm_cpri_dir_t direction,
    int *enable,
    bcm_cpri_port_rsvd4_agnostic_config_t *config) {

#ifdef CPRIMOD_SUPPORT
    portmod_rsvd4_rx_agnostic_config_t rx_config;
    portmod_rsvd4_tx_agnostic_config_t tx_config;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));

    if (direction == BCM_CPRI_DIR_RX ) {
        portmod_rsvd4_rx_agnostic_config_t_init(unit, &rx_config);
        rx_config.ordering_info_option = 0;
        BCM_IF_ERROR_RETURN
            (portmod_rsvd4_port_rx_agnostic_config_get(unit, port, enable, &rx_config));
        config->sync_mode               = rx_config.frame_sync_mode;
        config->master_frame_offset     = rx_config.master_frame_offset;
        config->message_group_offset    = rx_config.message_group_number;
        config->payload_multiple        = rx_config.payload_multiple;
        config->presentation_mod_count  = rx_config.pres_mod_count;
        config->ordering_info_option    = rx_config.ordering_info_option;
    } else { /* BCM_CPRI_DIR_TX */
        portmod_rsvd4_tx_agnostic_config_t_init(unit, &tx_config);
        BCM_IF_ERROR_RETURN
            (portmod_rsvd4_port_tx_agnostic_config_get(unit, port, enable, &tx_config));
        config->sync_mode               = tx_config.frame_sync_mode;
        config->master_frame_offset     = tx_config.master_frame_offset;
        config->message_group_offset    = tx_config.message_group_number;
        config->payload_multiple        = tx_config.payload_multiple;
        config->presentation_mod_count  = tx_config.pres_mod_count;
        config->presentation_mod_offset = tx_config.pres_mod_offset;
        config->tx_queue_mod_count      = tx_config.queue_mod_count;
        config->ordering_info_option    = tx_config.ordering_info_option;
    }
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif

}

/* Set RoE frame format for the port. */
int bcm_esw_cpri_port_frame_config_set(
    int unit,
    bcm_port_t port,
    bcm_cpri_port_frame_config_t *config) {

#ifdef CPRIMOD_SUPPORT
    portmod_cpri_port_frame_config_t pm_config;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));

    pm_config.frame_type            = config->port_frame_format;
    pm_config.decap_primary_key     = config->decap_primary_key;
    pm_config.decap_secondary_key   = config->decap_secondary_key;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_frame_config_set(unit, port, &pm_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif

}

/* Configure decap secondary key to queue mapping. */
int bcm_esw_cpri_port_decap_secondary_key_to_queue_mapping_set(
    int unit,
    bcm_port_t port,
    uint32 secondary_key,
    uint32 queue_num) {

#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_flow_to_queue_mapping_set(unit, port, secondary_key, queue_num));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif

}

/* Retrieve decap secondary key to queue mapping. */
int bcm_esw_cpri_port_decap_secondary_key_to_queue_mapping_get(
    int unit,
    bcm_port_t port,
    uint32 secondary_key,
    uint32 *queue_num) {
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_flow_to_queue_mapping_get(unit, port, secondary_key, queue_num));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif

}

/* Set Compression Saturation Value. */
int bcm_esw_cpri_port_compression_saturation_config_set(
    int unit,
    bcm_port_t port,
    uint32 threshold_value) {
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_compression_saturation_config_set(unit, port, threshold_value));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif

}

/* Set Compression Saturation Value. */
int bcm_esw_cpri_port_compression_saturation_config_get(
    int unit,
    bcm_port_t port,
    uint32* threshold_value) {
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_compression_saturation_config_get(unit, port, threshold_value));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif

}

/* Set DeCompression Saturation Value. */
int bcm_esw_cpri_port_decompression_saturation_config_set(
    int unit,
    bcm_port_t port,
    uint32 sat_value) {
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_decompression_saturation_config_set(unit, port, sat_value));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif

}

/* Set DeCompression Saturation Value. */
int bcm_esw_cpri_port_decompression_saturation_config_get(
    int unit,
    bcm_port_t port,
    uint32* sat_value) {

#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_decompression_saturation_config_get(unit, port, sat_value));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif

}

/* Get RSVD4 Rx current frame info */
int bcm_esw_cpri_port_rsvd4_rx_current_frame_sync_info_get(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_frame_sync_info_t *frame_sync_info) {
#ifdef CPRIMOD_SUPPORT
    uint32 hyper_frame_number = 0;
    uint32 master_frame_number = 0;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_cw_sync_info_get(unit, port, &hyper_frame_number, &master_frame_number));

    frame_sync_info->master_frame_number = master_frame_number;

    return (BCM_E_NONE);
#else
   return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_current_frame_sync_info_get(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_frame_sync_info_t *frame_sync_info) {

#ifdef CPRIMOD_SUPPORT
    uint32_t master_frame_number =0;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_frame_next_bfn_get(unit, port, &master_frame_number));

    frame_sync_info->master_frame_number = master_frame_number;
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Clear Rx Data Path in Framer and Encapsulation. */
int bcm_esw_cpri_port_rx_pipeline_clear(
    int unit,
    bcm_gport_t port){

#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_pipeline_clear(unit, port));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Clear Tx Data Path in Framer and Decapsulation. */
int bcm_esw_cpri_port_tx_pipeline_clear(
    int unit,
    bcm_gport_t port){

#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_pipeline_clear(unit, port));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Get tx enable status */
int bcm_esw_cpri_port_tx_enable_get(
    int unit,
    bcm_gport_t port,
    int *enable){

#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_enable_get(unit, port, enable));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Set tx enable */
int bcm_esw_cpri_port_tx_enable_set(
    int unit,
    bcm_gport_t port,
    int enable){

#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_enable_set(unit, port, enable));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Get the value for the debug attribute. */
int bcm_esw_cpri_port_debug_get(
    int unit,
    bcm_gport_t port,
    bcm_cpri_debug_attr_t type,
    int index,
    int buffer_size,
    uint32 *data) {
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_debug_get(unit, port, type, index, buffer_size, data));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Set the value for the debug attribute. */
int bcm_esw_cpri_port_debug_set(
    int unit,
    bcm_gport_t port,
    bcm_cpri_debug_attr_t type,
    int index,
    int buffer_size,
    uint32 *data){
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_debug_set(unit, port, type, index, buffer_size, data));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/* Clear the value for the debug attribute. */
int bcm_esw_cpri_port_debug_clear(
    int unit,
    bcm_gport_t port,
    bcm_cpri_debug_attr_t type,
    int index) {
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_debug_clear(unit, port, type, index));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int
bcm_esw_cpri_rsvd1_msg_get(
    int unit,
    bcm_cpri_rsvd1_msg_t *rsvd1_msg)
{
#ifdef CPRIMOD_SUPPORT
    if (!soc_feature(unit, soc_feature_rsvd1_intf)) {
        return BCM_E_UNAVAIL;
    }
    if(!rsvd1_msg) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(READ_BRCM_RESERVED_IPROC_6r(unit, &rsvd1_msg->rsvd1_msg0));
    BCM_IF_ERROR_RETURN(READ_BRCM_RESERVED_IPROC_7r(unit, &rsvd1_msg->rsvd1_msg1));
    BCM_IF_ERROR_RETURN(READ_BRCM_RESERVED_IPROC_8r(unit, &rsvd1_msg->rsvd1_msg2));
    return SOC_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

