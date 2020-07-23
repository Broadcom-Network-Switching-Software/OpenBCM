

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MIRROR


#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stack.h>
#include <bcm/types.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm_int/dnx/mirror/mirror.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_mirror_access.h>
#include "mirror_profile.h"
#include "mirror_header.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <src/bcm/dnx/init/init_pp.h>





#define DNX_MIRROR_SYSTEM_HEADER_MAX_LEN_IN_DWORDS   (14)


#define DNX_MIRROR_HEADER_OUT_LIF_EXT_TYPE   (4)


#define DNX_MIRROR_SYSTEM_HEADER_DATA_GET(_unit, _type, _field)  (dnx_data_headers._type._field##_get(_unit))


#define DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(_unit, _type, _field, _val) \
    { \
        uint32 __field_offset__; \
        uint32 __field_bits__; \
        __field_offset__ = DNX_MIRROR_SYSTEM_HEADER_DATA_GET(_unit, _type, _field##_offset); \
        __field_bits__ = DNX_MIRROR_SYSTEM_HEADER_DATA_GET(_unit, _type, _field##_bits); \
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(system_header, (offset + __field_offset__), __field_bits__, _val)); \
    }


#define DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(_unit, _type, _field, _val) \
    { \
        uint32 __field_offset__; \
        uint32 __field_bits__; \
        __field_offset__ = DNX_MIRROR_SYSTEM_HEADER_DATA_GET(_unit, _type, _field##_offset); \
        __field_bits__ = DNX_MIRROR_SYSTEM_HEADER_DATA_GET(_unit, _type, _field##_bits); \
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, (offset + __field_offset__), __field_bits__, &_val)); \
    }


#define DNX_MIRROR_LEGACY_SYSTEM_HEADER_DATA_GET(_unit, _type, _field)  (dnx_data_headers._type._field##_get(_unit))


#define DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_SET(_unit, _type, _field, _val) \
    { \
        uint32 __field_offset__; \
        uint32 __field_bits__; \
        __field_offset__ = DNX_MIRROR_LEGACY_SYSTEM_HEADER_DATA_GET(_unit, _type, _field##_offset); \
        __field_bits__ = DNX_MIRROR_LEGACY_SYSTEM_HEADER_DATA_GET(_unit, _type, _field##_bits); \
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(system_header, (offset + __field_offset__), __field_bits__, _val)); \
    }


#define DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_GET(_unit, _type, _field, _val) \
    { \
        uint32 __field_offset__; \
        uint32 __field_bits__; \
        __field_offset__ = DNX_MIRROR_LEGACY_SYSTEM_HEADER_DATA_GET(_unit, _type, _field##_offset); \
        __field_bits__ = DNX_MIRROR_LEGACY_SYSTEM_HEADER_DATA_GET(_unit, _type, _field##_bits); \
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, (offset + __field_offset__), __field_bits__, (uint32 *)&_val)); \
    }


#define DNX_MIRROR_UDH_SIZE_TO_UDH_TYPE(_size) \
    ((_size)>2)?3:(_size)


#define DNX_MIRROR_UDH_TYPE_TO_UDH_SIZE(_size) \
        ((_size)>2)?4:(_size)




typedef enum
{
    
    DNX_MIRROR_FTMH_ASE_TYPE_OAM = 0x0,
    
    DNX_MIRROR_FTMH_ASE_TYPE_1588v2 = 0x1,
    
    DNX_MIRROR_FTMH_ASE_TYPE_ERSPAN_OR_SFLOW = 0x3,
    
    DNX_MIRROR_FTMH_ASE_TYPE_TRAJECTORY_TRACE = 0x5,
    
    DNX_MIRROR_FTMH_ASE_TYPE_INT = 0x9
} dnx_mirror_ftmh_ase_type_encode_t;


static shr_error_e
dnx_mirror_header_ase_header_set(
    int unit,
    uint32 offset,
    bcm_pkt_dnx_ftmh_ase_extension_t * ase_ext,
    uint32 *system_header)
{
    bcm_pkt_dnx_erspan_info_t *erspan_info = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ase_ext, _SHR_E_PARAM, "ase_ext");
    SHR_NULL_CHECK(system_header, _SHR_E_PARAM, "system_header");

    switch (ase_ext->ase_type)
    {
        case bcmPktDnxAseTypeNone:
        case bcmPktDnxAseTypeMirrorOnDrop:
            break;
        case bcmPktDnxAseTypeTrajectoryTrace:
            DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, trajectory_trace_type,
                                               DNX_MIRROR_FTMH_ASE_TYPE_TRAJECTORY_TRACE);

            break;
        case bcmPktDnxAseTypeErspan:
            erspan_info = &ase_ext->ase_info.erspan_info;

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, direction, erspan_info->direction);

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, truncated, erspan_info->truncated_flag);

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, en, erspan_info->en);

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, cos, erspan_info->cos);

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, vlan, erspan_info->vlan);

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, erspan_type, DNX_MIRROR_FTMH_ASE_TYPE_ERSPAN_OR_SFLOW);

            break;
        case bcmPktDnxAseTypeSFlow:
            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, erspan_type, DNX_MIRROR_FTMH_ASE_TYPE_ERSPAN_OR_SFLOW);

            break;
        case bcmPktDnxAseTypeInt:

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, int_profile, ase_ext->ase_info.int_profile);

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, int_type, DNX_MIRROR_FTMH_ASE_TYPE_INT);

            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcmPktDnxAseType: %d is not supported.\n", ase_ext->ase_type);

            break;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_mirror_header_system_header_buffer_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_mirror_header_info_t * mirror_header_info,
    uint32 total_len_in_bits,
    uint32 *header_size,
    uint32 *system_header,
    uint32 *stacking_ext_offset,
    uint32 *ase_ext_offset,
    uint32 *tsh_offset,
    uint8 fhei_stamp_en,
    uint32 *fhei_offset)
{
    int snif_type, action_profile_id;
    uint32 offset, cur_offset_in_bytes;
    uint32 lif_index = 0, lif_ext_type = 0, lif_ext_size;
    uint32 entry_handle_id, lif_ext_index, udh_index = 0;
    uint32 lb_key_ext_size, stacking_ext_size, inter_hdr_base_size;
    uint32 all_lif_ext_size[DNX_MIRROR_HEADER_OUT_LIF_EXT_TYPE] = { 0 };
    uint32 stamp_dp_en = 0;
    uint8 add_dsp_ext, udh_en;
    bcm_mirror_pkt_dnx_ftmh_header_t *tm;
    bcm_mirror_pkt_dnx_pp_header_t *pp;
    bcm_pkt_dnx_udh_t *udh;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dbal_enum_value_field_tm_action_type_e tm_action_type = DBAL_ENUM_FVAL_TM_ACTION_TYPE_FORWARD;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    snif_type = DNX_MIRROR_DEST_TYPE_GET(flags);
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_LB_KEY_EXT_SIZE, INST_SINGLE, &lb_key_ext_size);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_STACKING_EXT_SIZE, INST_SINGLE,
                               &stacking_ext_size);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_SIZE, INST_SINGLE, &inter_hdr_base_size);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_USER_HEADER_ENABLE, INST_SINGLE, &udh_en);

    
    for (lif_ext_index = 0; lif_ext_index < (DNX_MIRROR_HEADER_OUT_LIF_EXT_TYPE - 1); lif_ext_index++)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, lif_ext_index,
                                   &all_lif_ext_size[lif_ext_index + 1]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    
    offset = total_len_in_bits;

    
    offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE);
    cur_offset_in_bytes = DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE;

    tm = &mirror_header_info->tm;
    pp = &mirror_header_info->pp;
    udh = &mirror_header_info->udh[0];

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, traffic_class, tm->tc);

    
    if (tm->src_sysport == BCM_GPORT_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tm->src_sysport is invalid.\n");
    }

    if (BCM_GPORT_IS_SET(tm->src_sysport))
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, tm->src_sysport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info));
        if (gport_info.sys_port == BCM_GPORT_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tm->src_sysport 0x%x: can not be converted to system port.\n", tm->src_sysport);
        }
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, src_sys_port_aggregate, gport_info.sys_port);
    }
    else
    {
        
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, src_sys_port_aggregate, tm->src_sysport);
    }

    if (tm->dp != 0)
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_CONG_INIT_SETTINGS, entry_handle_id));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SRAM_STAMP_FTMH_DP_EN, INST_SINGLE, &stamp_dp_en);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        
        if (stamp_dp_en)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "Mirror : DP set in mirror_header_info which will be overridden by hardware %s%s%s%s\n",
                        EMPTY, EMPTY, EMPTY, EMPTY);
        }
    }

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, drop_precedence, tm->dp);

    
    switch (snif_type)
    {
        case DBAL_ENUM_FVAL_SNIF_TYPE_SNOOP:
            tm_action_type = DBAL_ENUM_FVAL_TM_ACTION_TYPE_SNOOP;
            break;
        case DBAL_ENUM_FVAL_SNIF_TYPE_MIRROR:
            tm_action_type = DBAL_ENUM_FVAL_TM_ACTION_TYPE_MIRROR;
            break;
        case DBAL_ENUM_FVAL_SNIF_TYPE_STATISTICAL_SAMPLE:
            tm_action_type = DBAL_ENUM_FVAL_TM_ACTION_TYPE_STAT_SAMPLE;
            break;
        default:
            break;
    }

    DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, tm_action_type, tm_action_type);

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, tm_action_is_mc, tm->is_mc_traffic);

    
    if (tm->is_mc_traffic == TRUE)
    {
        if (!_BCM_MULTICAST_IS_SET(tm->mc_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tm->mc_id 0x%x is not multicast.\n", tm->mc_id);
        }

        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, outlif, _BCM_MULTICAST_ID_GET(tm->mc_id));
    }
    else
    {
        if (tm->out_vport == BCM_GPORT_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tm->out_vport is invalid.\n");
        }
        else if (tm->out_vport != 0)
        {
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                            (unit, tm->out_vport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS,
                             &gport_hw_resources));
            if (gport_hw_resources.global_out_lif == BCM_GPORT_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "tm->out_vport 0x%x: cant not be converted to global outlif.\n", tm->out_vport);
            }
            DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, outlif, gport_hw_resources.global_out_lif);
        }
    }

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, tm_profile, 2);

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, visibility, 0);

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, internal_header_en, 1);

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, tsh_en, 1);

    
    add_dsp_ext = dnx_data_stack.general.stacking_enable_get(unit) || dnx_data_headers.ftmh.add_dsp_ext_get(unit);

    if (add_dsp_ext)
    {
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, tm_dest_ext_present, 1);
    }

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, ase_present, tm->ase_ext.valid);

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, flow_id_ext_present, tm->flow_id_ext.valid);

    
    if (lb_key_ext_size > 0)
    {
        
        offset -= BYTES2BITS(lb_key_ext_size);
        cur_offset_in_bytes += lb_key_ext_size;
    }

    
    if (stacking_ext_size > 0)
    {
        
        *stacking_ext_offset = cur_offset_in_bytes;

        
        offset -= BYTES2BITS(stacking_ext_size);
        cur_offset_in_bytes += stacking_ext_size;
    }

    
    if (add_dsp_ext)
    {
        uint32 dest;

        
        offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_FTMH_TM_DST_SIZE);
        cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_FTMH_TM_DST_SIZE;

        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SNIF_COMMAND_TABLE, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &dest);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(system_header, offset,
                                                   DNX_MIRROR_SYSTEM_HEADER_DATA_GET(unit, ftmh, tm_dst_bits), dest));
    }

    
    if (tm->ase_ext.valid == TRUE)
    {
        
        *ase_ext_offset = cur_offset_in_bytes;

        
        offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_FTMH_ASE_HEADER_SIZE);
        cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_FTMH_ASE_HEADER_SIZE;

        SHR_IF_ERR_EXIT(dnx_mirror_header_ase_header_set(unit, offset, &tm->ase_ext, system_header));
    }

    
    if (tm->flow_id_ext.valid == TRUE)
    {
        
        offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_FTMH_FLOW_ID_HEADER_SIZE);
        cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_FTMH_FLOW_ID_HEADER_SIZE;

        
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, flow_id, tm->flow_id_ext.flow_id);
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, ftmh, flow_profile, tm->flow_id_ext.flow_profile);
    }

    
    *tsh_offset = cur_offset_in_bytes;

    
    offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_TSH_BASE_HEADER_SIZE);
    cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_TSH_BASE_HEADER_SIZE;

    
    offset -= BYTES2BITS(inter_hdr_base_size);
    cur_offset_in_bytes += inter_hdr_base_size;

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, internal, tail_edit, (pp->tail_edit_profile));

    
    if (fhei_stamp_en)
    {
        
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, internal, fhei_size, 2);
    }

    while (pp->out_vport_ext[lif_index] != 0)
    {
        ++lif_ext_type;
        ++lif_index;

        if (lif_index >= (sizeof(pp->out_vport_ext) / sizeof(pp->out_vport_ext[0])))
        {
            break;
        }
    }

    DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, internal, lif_ext_type, lif_ext_type);
    DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, internal, ttl, 255);

    if (tm->ase_ext.ase_type == bcmPktDnxAseTypeSFlow)
    {
        
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, internal, parsing_start_type,
                                           (dnx_init_pp_layer_types_value_get
                                            (unit, DBAL_ENUM_FVAL_LAYER_TYPES_INGRESS_SCTP_EGRESS_FTMH)));
    }
    else
    {
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, internal, parsing_start_type,
                                           (dnx_init_pp_layer_types_value_get
                                            (unit, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET)));
    }

    
    if (mirror_header_info->pp.eth_header_remove)
    {
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, internal, eth_rm_fli, 1);
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, internal, parsing_start_type,
                                           (dnx_init_pp_layer_types_value_get
                                            (unit, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET)));
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, internal, eth_rm_pso, 0);
    }

    
    if (fhei_stamp_en)
    {
        
        offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE);
        cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE;

        
        *fhei_offset = cur_offset_in_bytes;
    }

    if (lif_ext_type > 0)
    {
        
        lif_ext_size = BYTES2BITS(all_lif_ext_size[1]);

        
        offset -= BYTES2BITS(all_lif_ext_size[lif_ext_type]);
        cur_offset_in_bytes += all_lif_ext_size[lif_ext_type];

        
        switch (lif_ext_type)
        {
            case 3:
                
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, pp->out_vport_ext[2], DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS,
                                 &gport_hw_resources));
                if (gport_hw_resources.global_out_lif == BCM_GPORT_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "pp->out_vport_ext[2] 0x%x: cant not be converted to global outlif.\n",
                                 pp->out_vport_ext[2]);
                }
                SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                                (system_header, offset + (lif_ext_type - 3) * lif_ext_size, lif_ext_size,
                                 gport_hw_resources.global_out_lif));
                 
            case 2:
                
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, pp->out_vport_ext[1], DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS,
                                 &gport_hw_resources));
                if (gport_hw_resources.global_out_lif == BCM_GPORT_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "pp->out_vport_ext[1] 0x%x: cant not be converted to global outlif.\n",
                                 pp->out_vport_ext[1]);
                }
                SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                                (system_header, offset + (lif_ext_type - 2) * lif_ext_size, lif_ext_size,
                                 gport_hw_resources.global_out_lif));
                 
            case 1:
                
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, pp->out_vport_ext[0], DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS,
                                 &gport_hw_resources));
                if (gport_hw_resources.global_out_lif == BCM_GPORT_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "pp->out_vport_ext[0] 0x%x: cant not be converted to global outlif.\n",
                                 pp->out_vport_ext[0]);
                }
                SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                                (system_header, offset + (lif_ext_type - 1) * lif_ext_size, lif_ext_size,
                                 gport_hw_resources.global_out_lif));
                break;
            default:
                break;
        }
    }

    
    offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_UDH_BASE_SIZE);
    cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_UDH_BASE_SIZE;

    if (udh_en)
    {
        
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, udh, data_type0, DNX_MIRROR_UDH_SIZE_TO_UDH_TYPE(udh[0].size));
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, udh, data_type1, DNX_MIRROR_UDH_SIZE_TO_UDH_TYPE(udh[1].size));
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, udh, data_type2, DNX_MIRROR_UDH_SIZE_TO_UDH_TYPE(udh[2].size));
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, udh, data_type3, DNX_MIRROR_UDH_SIZE_TO_UDH_TYPE(udh[3].size));

        for (udh_index = 0; udh_index < sizeof(mirror_header_info->udh) / sizeof(mirror_header_info->udh[0]);
             ++udh_index)
        {
            switch (udh[udh_index].size)
            {
                case 4:
                    sal_memcpy((uint8 *) system_header + cur_offset_in_bytes, (uint8 *) &udh[udh_index].data, 4);
                    break;
                case 2:
                    system_header[cur_offset_in_bytes + 1] = (udh[udh_index].data >> 8) & 0xFF;
                     
                case 1:
                    system_header[cur_offset_in_bytes] = udh[udh_index].data & 0xFF;
                    break;
                default:
                    break;
            }
            cur_offset_in_bytes += udh[udh_index].size;
        }
    }
    else
    {
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, udh, data_type0, 0);
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, udh, data_type1, 0);
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, udh, data_type2, 0);
        DNX_MIRROR_SYSTEM_HEADER_FIELD_SET(unit, udh, data_type3, 0);
    }

    *header_size = cur_offset_in_bytes;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_mirror_header_legacy_system_header_buffer_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_mirror_header_info_t * mirror_header_info,
    uint32 total_len_in_bits,
    uint32 *header_size,
    uint32 *system_header,
    uint32 *stacking_ext_offset,
    uint8 fhei_stamp_en,
    uint32 *fhei_offset)
{
    int snif_type, action_profile_id;
    uint32 offset, cur_offset_in_bytes;
    uint32 add_dsp_ext, lb_key_ext_size, stacking_ext_size;
    uint32 entry_handle_id;
    bcm_mirror_pkt_dnx_ftmh_header_t *tm;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dbal_enum_value_field_tm_action_type_e tm_action_type = DBAL_ENUM_FVAL_TM_ACTION_TYPE_FORWARD;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (flags & BCM_MIRROR_DEST_IS_SNOOP)
    {
        snif_type = DBAL_ENUM_FVAL_SNIF_TYPE_SNOOP;
    }
    else
    {
        snif_type = DBAL_ENUM_FVAL_SNIF_TYPE_MIRROR;
    }

    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_LB_KEY_EXT_SIZE, INST_SINGLE, &lb_key_ext_size);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_STACKING_EXT_SIZE, INST_SINGLE,
                               &stacking_ext_size);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    
    offset = total_len_in_bits;

    
    offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_FTMH_LEGACY_BASE_HEADER_SIZE);
    cur_offset_in_bytes = DNX_DATA_MAX_HEADERS_FTMH_LEGACY_BASE_HEADER_SIZE;

    tm = &mirror_header_info->tm;

    
    DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_SET(unit, ftmh_legacy, traffic_class, tm->tc);

    
    if (tm->src_sysport == BCM_GPORT_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tm->src_sysport is invalid.\n");
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, tm->src_sysport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info));
    if (gport_info.sys_port == BCM_GPORT_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tm->src_sysport 0x%x: can not be converted to system port.\n", tm->src_sysport);
    }
    DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_SET(unit, ftmh_legacy, src_sys_port_aggregate, gport_info.sys_port);

    
    DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_SET(unit, ftmh_legacy, drop_precedence, tm->dp);

    
    switch (snif_type)
    {
        case DBAL_ENUM_FVAL_SNIF_TYPE_SNOOP:
            tm_action_type = DBAL_ENUM_FVAL_TM_ACTION_TYPE_SNOOP;
            break;
        case DBAL_ENUM_FVAL_SNIF_TYPE_MIRROR:
            tm_action_type = DBAL_ENUM_FVAL_TM_ACTION_TYPE_MIRROR;
            break;
        
    }

    DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_SET(unit, ftmh_legacy, tm_action_type, tm_action_type);

    
    DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_SET(unit, ftmh_legacy, internal_header_en, 1);

    
    DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_SET(unit, ftmh_legacy, outbound_mirr_disable, 1);

    
    DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_SET(unit, ftmh_legacy, tm_action_is_mc, tm->is_mc_traffic);

    
    if (tm->is_mc_traffic == TRUE)
    {
        if (!_BCM_MULTICAST_IS_SET(tm->mc_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tm->mc_id 0x%x: is not multicast.\n", tm->mc_id);
        }

        DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_SET(unit, ftmh_legacy, outlif, _BCM_MULTICAST_ID_GET(tm->mc_id));
    }
    else
    {
        if (tm->out_vport == BCM_GPORT_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tm->out_vport is invalid.\n");
        }
        else if (tm->out_vport != 0)
        {
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                            (unit, tm->out_vport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS,
                             &gport_hw_resources));
            if (gport_hw_resources.global_out_lif == BCM_GPORT_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "tm->out_vport 0x%x: can not be converted to global outlif.\n", tm->out_vport);
            }
            DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_SET(unit, ftmh_legacy, outlif, gport_hw_resources.global_out_lif);
        }
    }

    
    add_dsp_ext = dnx_data_stack.general.stacking_enable_get(unit) || dnx_data_headers.ftmh.add_dsp_ext_get(unit);

    if (add_dsp_ext)
    {
        DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_SET(unit, ftmh_legacy, dsp_ext_present, 1);
    }

    
    if (lb_key_ext_size > 0)
    {
        
        offset -= BYTES2BITS(lb_key_ext_size);
        cur_offset_in_bytes += lb_key_ext_size;
    }

    
    if (add_dsp_ext)
    {
        uint32 dest;
        dbal_fields_e dbal_dest_type;
        uint32 dest_val = 0;

        
        offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_FTMH_LEGACY_DSP_EXT_HEADER_SIZE);
        cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_FTMH_LEGACY_DSP_EXT_HEADER_SIZE;

        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SNIF_COMMAND_TABLE, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &dest);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_DESTINATION, dest, &dbal_dest_type, &dest_val));

        
        if (dbal_dest_type == DBAL_FIELD_PORT_ID)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_set_field(system_header, offset,
                                                       DNX_MIRROR_SYSTEM_HEADER_DATA_GET(unit, ftmh_legacy,
                                                                                         dsp_ext_header_bits), dest));
        }
    }

    
    if (stacking_ext_size > 0)
    {
        
        *stacking_ext_offset = cur_offset_in_bytes;

        
        offset -= BYTES2BITS(stacking_ext_size);
        cur_offset_in_bytes += stacking_ext_size;
    }

    
    
    offset -= BYTES2BITS(7);
    cur_offset_in_bytes += 7;

    
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(system_header, (offset + 48), 4, 11));

    
    if (fhei_stamp_en)
    {
        
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(system_header, (offset + 52), 2, 1));

        
        offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE_SZ0);
        cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE_SZ0;

        
        *fhei_offset = cur_offset_in_bytes;
    }

    *header_size = cur_offset_in_bytes;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_mirror_header_system_header_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_mirror_header_info_t * mirror_header_info)
{
    uint8 is_keep_incoming_headers = FALSE, fhei_stamp_en = FALSE;
    int snif_type, action_profile_id, system_headers_mode;
    uint32 entry_handle_id, total_len_in_bits, header_size = 0;
    uint32 itpp_delta = 0;
    uint32 stacking_ext_offset = 0, ase_ext_offset = 0, tsh_ext_offset = 0, fhei_offset = 0;
    uint32 system_header[DNX_MIRROR_SYSTEM_HEADER_MAX_LEN_IN_DWORDS] = { 0 };
    dbal_enum_value_field_ftmh_app_specific_ext_type_e ase_type = DBAL_ENUM_FVAL_FTMH_APP_SPECIFIC_EXT_TYPE_NONE_OR_MOD;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    snif_type = DNX_MIRROR_DEST_TYPE_GET(flags);
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    
    system_headers_mode = DNX_MIRROR_SYSTEM_HEADER_DATA_GET(unit, system_headers, system_headers_mode);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_SYSTEM_HEADER_TABLE, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIFF_APPEND_MODE, INST_SINGLE, TRUE);

    
    if (flags & BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER)
    {
        is_keep_incoming_headers = TRUE;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMOVE_SYSTEM_HEADERS, INST_SINGLE,
                                 !is_keep_incoming_headers);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEEP_INCOMING_SYSTEM_HEADRS, INST_SINGLE,
                                 is_keep_incoming_headers);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SNIF_NEW_SYSTEM_HEADER_TABLE, entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

    
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_STAMP_EN, INST_SINGLE, &fhei_stamp_en);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_SNIF_NEW_SYSTEM_HEADER_TABLE, DBAL_FIELD_NEW_SYSTEM_HEADER, FALSE, 0, 0,
                     (int *) &total_len_in_bits));

    if (system_headers_mode == DNX_MIRROR_SYSTEM_HEADER_DATA_GET(unit, system_headers, system_headers_mode_jericho2))
    {
        
        SHR_IF_ERR_EXIT(dnx_mirror_header_system_header_buffer_set
                        (unit, flags, mirror_dest_id, mirror_header_info, total_len_in_bits,
                         &header_size, system_header, &stacking_ext_offset,
                         &ase_ext_offset, &tsh_ext_offset, fhei_stamp_en, &fhei_offset));

        itpp_delta = header_size;

        if (ase_ext_offset > 0)
        {
            dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_EXT_STAMP_EN, INST_SINGLE, TRUE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_EXT_OFFSET, INST_SINGLE,
                                         ase_ext_offset);
            if (mirror_header_info->tm.ase_ext.ase_type == bcmPktDnxAseTypeTrajectoryTrace)
            {
                ase_type = DBAL_ENUM_FVAL_FTMH_APP_SPECIFIC_EXT_TYPE_TRAJECTORY_TRACE;
            }
            else if (mirror_header_info->tm.ase_ext.ase_type == bcmPktDnxAseTypeErspan)
            {
                ase_type = DBAL_ENUM_FVAL_FTMH_APP_SPECIFIC_EXT_TYPE_ERSPAN;
            }
            else if (mirror_header_info->tm.ase_ext.ase_type == bcmPktDnxAseTypeSFlow)
            {
                ase_type = DBAL_ENUM_FVAL_FTMH_APP_SPECIFIC_EXT_TYPE_SFLOW;
            }
            else if (mirror_header_info->tm.ase_ext.ase_type == bcmPktDnxAseTypeMirrorOnDrop)
            {
                ase_type = DBAL_ENUM_FVAL_FTMH_APP_SPECIFIC_EXT_TYPE_NONE_OR_MOD;
            }
            else if (mirror_header_info->tm.ase_ext.ase_type == bcmPktDnxAseTypeInt)
            {
                ase_type = DBAL_ENUM_FVAL_FTMH_APP_SPECIFIC_EXT_TYPE_NONE_OR_MOD;
                
                dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_APPLICATION_EXT_STAMP_EN, INST_SINGLE,
                                            FALSE);
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APP_EXT_TYPE, INST_SINGLE, ase_type);

        }

        
        if (tsh_ext_offset > 0)
        {
            dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_TSH_STAMP_EN, INST_SINGLE, TRUE);
            dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_TSH_HDR_OFFSET, INST_SINGLE, tsh_ext_offset);
        }
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_mirror_header_legacy_system_header_buffer_set
                        (unit, flags, mirror_dest_id, mirror_header_info, total_len_in_bits,
                         &header_size, system_header, &stacking_ext_offset, fhei_stamp_en, &fhei_offset));

        
        itpp_delta = header_size
            + BITS2BYTES(dnx_data_headers.system_headers.udh_base_size_get(unit)
                         + dnx_data_field.udh.field_class_id_total_size_get(unit));
    }

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_NEW_SYSTEM_HEADER, INST_SINGLE, system_header);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_SIZE, INST_SINGLE, header_size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITPP_DELTA, INST_SINGLE, itpp_delta);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITPP_DELTA_OW, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_STACKING_EXT_OFFSET, INST_SINGLE,
                                 stacking_ext_offset);

    
    if (fhei_offset > 0)
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_STAMP_EN, INST_SINGLE, TRUE);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_EXT_OFFSET, INST_SINGLE, fhei_offset);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    {
        int profile;

        int mod_profile_index;

        for (mod_profile_index = 0; mod_profile_index < dnx_data_snif.ingress.nof_mirror_on_drop_profiles_get(unit);
             mod_profile_index++)
        {
            SHR_IF_ERR_EXIT(dnx_mirror_db.mirror_on_drop_profile.get(unit, mod_profile_index, &profile));
            if (profile == action_profile_id)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_MIRROR_ON_DROP_COMMAND, &entry_handle_id));

                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MOD_PROFILE_ID, mod_profile_index);

                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMOVE_SYSTEM_HEADERS, INST_SINGLE,
                                             !is_keep_incoming_headers);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITPP_DELTA, INST_SINGLE, itpp_delta);

                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_mirror_header_ase_header_get(
    int unit,
    uint32 offset,
    uint32 *system_header,
    dbal_enum_value_field_ftmh_app_specific_ext_type_e ase_type,
    bcm_pkt_dnx_ftmh_ase_extension_t * ase_ext)
{
    uint32 val;
    bcm_pkt_dnx_erspan_info_t *erspan_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    
    if (ase_type == DBAL_ENUM_FVAL_FTMH_APP_SPECIFIC_EXT_TYPE_TRAJECTORY_TRACE)
    {
        ase_ext->ase_type = bcmPktDnxAseTypeTrajectoryTrace;
    }
    else if (ase_type == DBAL_ENUM_FVAL_FTMH_APP_SPECIFIC_EXT_TYPE_ERSPAN)
    {
        ase_ext->ase_type = bcmPktDnxAseTypeErspan;
    }
    else if (ase_type == DBAL_ENUM_FVAL_FTMH_APP_SPECIFIC_EXT_TYPE_SFLOW)
    {
        ase_ext->ase_type = bcmPktDnxAseTypeSFlow;
    }
    else if (ase_type == DBAL_ENUM_FVAL_FTMH_APP_SPECIFIC_EXT_TYPE_NONE_OR_MOD)
    {
        if (dnx_data_ingr_congestion.mirror_on_drop.feature_get(unit,
                                                                dnx_data_ingr_congestion_mirror_on_drop_is_supported))
        {
            ase_ext->ase_type = bcmPktDnxAseTypeMirrorOnDrop;
        }
        else
        {
            ase_ext->ase_type = bcmPktDnxAseTypeNone;
        }
    }

    switch (ase_ext->ase_type)
    {
        case bcmPktDnxAseTypeNone:
        case bcmPktDnxAseTypeMirrorOnDrop:
            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, int_type, val);
            if (val == DNX_MIRROR_FTMH_ASE_TYPE_INT)
            {
                ase_ext->ase_type = bcmPktDnxAseTypeInt;
                DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, int_profile, val);
                ase_ext->ase_info.int_profile = val;
            }
            break;
        case bcmPktDnxAseTypeTrajectoryTrace:
        {

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, trajectory_trace_type, val);
            if (val != DNX_MIRROR_FTMH_ASE_TYPE_TRAJECTORY_TRACE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "The decode ASE type %d is not trajectory trace.\n", val);
            }

            break;
        }
        case bcmPktDnxAseTypeErspan:
        {
            
            erspan_info = &ase_ext->ase_info.erspan_info;

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, erspan_type, val);
            if (val != DNX_MIRROR_FTMH_ASE_TYPE_ERSPAN_OR_SFLOW)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "The decoded ASE type %d is not ERSPAN.\n", val);
            }

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, direction, val);
            erspan_info->direction = val;

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, truncated, val);
            erspan_info->truncated_flag = val;

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, en, val);
            erspan_info->en = val;

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, cos, val);
            erspan_info->cos = val;

            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, vlan, val);
            erspan_info->vlan = val;

            break;
        }
        case bcmPktDnxAseTypeSFlow:
        {
            
            DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, erspan_type, val);
            if (val != DNX_MIRROR_FTMH_ASE_TYPE_ERSPAN_OR_SFLOW)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "The decoded ASE type %d is not ERSPAN.\n", val);
            }

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcmPktDnxAseType: %d is not supported.\n", ase_ext->ase_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_mirror_header_system_header_parser(
    int unit,
    uint32 flags,
    uint8 ase_stamp_en,
    uint8 tsh_stamp_en,
    uint8 fhei_stamp_en,
    uint32 fhei_offset,
    uint32 header_size,
    uint32 stacking_ext_offset,
    uint32 tsh_hdr_offset,
    uint32 total_len_in_bits,
    uint32 *system_header,
    dbal_enum_value_field_ftmh_app_specific_ext_type_e ase_type,
    bcm_mirror_header_info_t * mirror_header_info)
{
    uint32 offset, cur_offset_in_bytes;
    uint32 lif_ext_type = 0, lif_ext_size, fhei_size;
    uint32 entry_handle_id, lif_ext_index, udh_index = 0;
    uint32 lb_key_ext_size, stacking_ext_size, inter_hdr_base_size;
    uint32 all_lif_ext_size[DNX_MIRROR_HEADER_OUT_LIF_EXT_TYPE] = { 0 };
    uint8 add_dsp_ext, udh_en;
    uint32 val = 0;
    bcm_mirror_pkt_dnx_ftmh_header_t *tm;
    bcm_mirror_pkt_dnx_pp_header_t *pp;
    bcm_pkt_dnx_udh_t *udh;
    dbal_enum_value_field_tm_action_type_e tm_action;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_gport_t gport_tmp;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    offset = total_len_in_bits;

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_LB_KEY_EXT_SIZE, INST_SINGLE, &lb_key_ext_size);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_STACKING_EXT_SIZE, INST_SINGLE,
                               &stacking_ext_size);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_SIZE, INST_SINGLE, &inter_hdr_base_size);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_USER_HEADER_ENABLE, INST_SINGLE, &udh_en);

    
    for (lif_ext_index = 0; lif_ext_index < (DNX_MIRROR_HEADER_OUT_LIF_EXT_TYPE - 1); lif_ext_index++)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, lif_ext_index,
                                   &all_lif_ext_size[lif_ext_index + 1]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    
    offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE);
    cur_offset_in_bytes = DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE;

    tm = &mirror_header_info->tm;
    pp = &mirror_header_info->pp;
    udh = &mirror_header_info->udh[0];

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, traffic_class, val);
    tm->tc = (uint8) val;

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, src_sys_port_aggregate, val);
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_system_port(unit, (int) val, &gport_tmp));
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport_tmp, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info));
    if (BCM_GPORT_IS_TRUNK(gport_tmp))
    {
        tm->src_sysport = gport_tmp;
    }
    else
    {
        tm->src_sysport = gport_info.sys_port;
    }

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, drop_precedence, val);
    tm->dp = (uint8) val;

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, tm_action_type, tm_action);
    if (((flags & BCM_MIRROR_DEST_IS_SNOOP) && (tm_action != DBAL_ENUM_FVAL_TM_ACTION_TYPE_SNOOP)) ||
        ((flags & BCM_MIRROR_DEST_IS_STAT_SAMPLE) && (tm_action != DBAL_ENUM_FVAL_TM_ACTION_TYPE_STAT_SAMPLE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The TM action type %d has conflict with flags.\n", tm_action);
    }

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, tm_action_is_mc, val);
    tm->is_mc_traffic = (uint8) val;

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, outlif, val);
    if (tm->is_mc_traffic == TRUE)
    {
        tm->mc_id = (bcm_multicast_t) val;
    }
    else
    {
        if (val != 0)
        {
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                            (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, _SHR_CORE_ALL, val,
                             &tm->out_vport));
        }
    }

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, ase_present, val);
    tm->ase_ext.valid = (uint8) val;

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, flow_id_ext_present, val);
    tm->flow_id_ext.valid = (uint8) val;

    
    if (lb_key_ext_size > 0)
    {
        
        offset -= BYTES2BITS(lb_key_ext_size);
        cur_offset_in_bytes += lb_key_ext_size;
    }

    
    if (stacking_ext_size > 0)
    {
        if (cur_offset_in_bytes != stacking_ext_offset)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The FTMH stacking header offset mismatch: is 0x%x expected 0x%x.\n",
                         stacking_ext_offset, cur_offset_in_bytes);
        }

        
        offset -= BYTES2BITS(stacking_ext_size);
        cur_offset_in_bytes += stacking_ext_size;
    }

    
    add_dsp_ext = dnx_data_stack.general.stacking_enable_get(unit) || dnx_data_headers.ftmh.add_dsp_ext_get(unit);

    if (add_dsp_ext)
    {
        
        offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_FTMH_TM_DST_SIZE);
        cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_FTMH_TM_DST_SIZE;
    }

    
    if (tm->ase_ext.valid && (ase_stamp_en == TRUE))
    {
        
        offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_FTMH_ASE_HEADER_SIZE);
        cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_FTMH_ASE_HEADER_SIZE;

        SHR_IF_ERR_EXIT(dnx_mirror_header_ase_header_get(unit, offset, system_header, ase_type, &tm->ase_ext));
    }

    
    if (tm->flow_id_ext.valid == TRUE)
    {
        
        offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_FTMH_FLOW_ID_HEADER_SIZE);
        cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_FTMH_FLOW_ID_HEADER_SIZE;

        DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, flow_id, tm->flow_id_ext.flow_id);
        DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, ftmh, flow_profile, val);
        tm->flow_id_ext.flow_profile = (uint8) val;
    }

    
    offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_TSH_BASE_HEADER_SIZE);
    cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_TSH_BASE_HEADER_SIZE;

    
    offset -= BYTES2BITS(inter_hdr_base_size);
    cur_offset_in_bytes += inter_hdr_base_size;

    
    DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, internal, tail_edit, val);
    pp->tail_edit_profile = (uint8) val;

    
    if (fhei_stamp_en)
    {
        
        DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, internal, fhei_size, fhei_size);

        if (fhei_size != 2)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FHEI SZ mismatch: is 0x%x expected 2.\n", fhei_size);
        }
    }

    DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, internal, lif_ext_type, lif_ext_type);

    
    if (fhei_stamp_en)
    {
        
        offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE_SZ1);
        cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE_SZ1;

        
        if (fhei_offset != cur_offset_in_bytes)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FHEI offset mismatch: is %d expected %d.\n", fhei_offset, cur_offset_in_bytes);
        }
    }

    if (lif_ext_type > 0)
    {
        
        lif_ext_size = BYTES2BITS(all_lif_ext_size[1]);

        
        offset -= BYTES2BITS(all_lif_ext_size[lif_ext_type]);
        cur_offset_in_bytes += all_lif_ext_size[lif_ext_type];

        
        switch (lif_ext_type)
        {
            case 3:
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                                (system_header, offset + (lif_ext_type - 3) * lif_ext_size, lif_ext_size, &val));
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, _SHR_CORE_ALL, val,
                                 &pp->out_vport_ext[2]));
                 
            case 2:
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                                (system_header, offset + (lif_ext_type - 2) * lif_ext_size, lif_ext_size, &val));
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, _SHR_CORE_ALL, val,
                                 &pp->out_vport_ext[1]));
                 
            case 1:
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                                (system_header, offset + (lif_ext_type - 1) * lif_ext_size, lif_ext_size, &val));
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, _SHR_CORE_ALL, val,
                                 &pp->out_vport_ext[0]));
                break;
            default:
                break;
        }
    }

    
    if (udh_en)
    {
        
        offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_UDH_BASE_SIZE);
        cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_UDH_BASE_SIZE;

        DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, udh, data_type0, val);
        udh[0].size = DNX_MIRROR_UDH_TYPE_TO_UDH_SIZE(val);
        DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, udh, data_type1, val);
        udh[1].size = DNX_MIRROR_UDH_TYPE_TO_UDH_SIZE(val);
        DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, udh, data_type2, val);
        udh[2].size = DNX_MIRROR_UDH_TYPE_TO_UDH_SIZE(val);
        DNX_MIRROR_SYSTEM_HEADER_FIELD_GET(unit, udh, data_type3, val);
        udh[3].size = DNX_MIRROR_UDH_TYPE_TO_UDH_SIZE(val);

        for (udh_index = 0; udh_index < sizeof(mirror_header_info->udh) / sizeof(mirror_header_info->udh[0]);
             ++udh_index)
        {
            switch (udh[udh_index].size)
            {
                case 4:
                    sal_memcpy((uint8 *) &udh[udh_index].data, (uint8 *) system_header + cur_offset_in_bytes, 4);
                    break;
                case 2:
                    udh[udh_index].data = system_header[cur_offset_in_bytes + 1] << 8;
                     
                case 1:
                    udh[udh_index].data |= system_header[cur_offset_in_bytes];
                    break;
                default:
                    break;
            }
            cur_offset_in_bytes += udh[udh_index].size;
        }
    }

    if (cur_offset_in_bytes != header_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Total header size mismatch: is 0x%x expected 0x%x.\n",
                     header_size, cur_offset_in_bytes);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_mirror_header_legacy_system_header_parser(
    int unit,
    uint32 flags,
    uint8 fhei_stamp_en,
    uint32 fhei_offset,
    uint32 stacking_ext_offset,
    uint32 header_size,
    uint32 total_len,
    uint32 *system_header,
    bcm_mirror_header_info_t * mirror_header_info)
{
    uint32 offset, cur_offset_in_bytes;
    uint32 add_dsp_ext, lb_key_ext_size, stacking_ext_size;
    uint32 entry_handle_id;
    uint32 val = 0;
    bcm_mirror_pkt_dnx_ftmh_header_t *tm;
    dbal_enum_value_field_tm_action_type_e tm_action;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_LB_KEY_EXT_SIZE, INST_SINGLE, &lb_key_ext_size);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_STACKING_EXT_SIZE, INST_SINGLE,
                               &stacking_ext_size);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    
    offset = total_len;

    
    offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_FTMH_LEGACY_BASE_HEADER_SIZE);
    cur_offset_in_bytes = DNX_DATA_MAX_HEADERS_FTMH_LEGACY_BASE_HEADER_SIZE;

    tm = &mirror_header_info->tm;

    
    DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_GET(unit, ftmh_legacy, traffic_class, val);
    tm->tc = (uint8) val;

    
    DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_GET(unit, ftmh_legacy, src_sys_port_aggregate, val);
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, val, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info));
    tm->src_sysport = gport_info.sys_port;

    
    DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_GET(unit, ftmh_legacy, drop_precedence, val);
    tm->dp = (uint8) val;

    
    DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_GET(unit, ftmh_legacy, tm_action_type, tm_action);
    if ((flags & BCM_MIRROR_DEST_IS_SNOOP) && (tm_action != DBAL_ENUM_FVAL_TM_ACTION_TYPE_SNOOP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The TM action type %d has conflict with flags.\n", tm_action);
    }

    
    DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_GET(unit, ftmh_legacy, tm_action_is_mc, val);
    tm->is_mc_traffic = val;

    
    DNX_MIRROR_LEGACY_SYSTEM_HEADER_FIELD_GET(unit, ftmh_legacy, outlif, val);
    if (tm->is_mc_traffic == TRUE)
    {
        tm->mc_id = val;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                        (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, _SHR_CORE_ALL, val, &tm->out_vport));
    }

    
    if (lb_key_ext_size > 0)
    {
        
        offset -= BYTES2BITS(lb_key_ext_size);
        cur_offset_in_bytes += lb_key_ext_size;
    }

    
    add_dsp_ext = dnx_data_stack.general.stacking_enable_get(unit) || dnx_data_headers.ftmh.add_dsp_ext_get(unit);

    if (add_dsp_ext)
    {
        
        offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_FTMH_LEGACY_DSP_EXT_HEADER_SIZE);
        cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_FTMH_LEGACY_DSP_EXT_HEADER_SIZE;
    }

    
    if (stacking_ext_size > 0)
    {
        if (cur_offset_in_bytes != stacking_ext_offset)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The FTMH stacking header offset mismatch: is %d expected %d.\n",
                         stacking_ext_offset, cur_offset_in_bytes);
        }

        
        offset -= BYTES2BITS(stacking_ext_size);
        cur_offset_in_bytes += stacking_ext_size;
    }

    
    offset -= BYTES2BITS(7);
    cur_offset_in_bytes += 7;

    
    if (fhei_stamp_en)
    {
        
        offset -= BYTES2BITS(DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE_SZ0);
        cur_offset_in_bytes += DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE_SZ0;

        
        if (fhei_offset != cur_offset_in_bytes)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FHEI offset mismatch: is %d expected %d.\n", fhei_offset, cur_offset_in_bytes);
        }
    }

    

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_mirror_header_system_header_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_mirror_header_info_t * mirror_header_info)
{
    int snif_type, action_profile_id, system_headers_mode;
    uint8 is_keep_incoming_headers = FALSE, append_mode = FALSE;
    uint8 ase_stamp_en = FALSE, tsh_stamp_en = FALSE, fhei_stamp_en = FALSE;
    uint32 entry_handle_id;
    uint32 total_len_in_bits, header_size;
    uint32 stacking_ext_offset, tsh_hdr_offset, fhei_offset;
    uint32 system_header[DNX_MIRROR_SYSTEM_HEADER_MAX_LEN_IN_DWORDS] = { 0 };
    dbal_enum_value_field_ftmh_app_specific_ext_type_e ase_type = DBAL_ENUM_FVAL_FTMH_APP_SPECIFIC_EXT_TYPE_NONE_OR_MOD;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");
    SHR_NULL_CHECK(mirror_header_info, _SHR_E_PARAM, "mirror_header_info");

    snif_type = (BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest_id)) ? DBAL_ENUM_FVAL_SNIF_TYPE_SNOOP :
        (BCM_GPORT_IS_MIRROR_STAT_SAMPLE(mirror_dest_id)) ? DBAL_ENUM_FVAL_SNIF_TYPE_STATISTICAL_SAMPLE :
        DBAL_ENUM_FVAL_SNIF_TYPE_MIRROR;
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    *flags = (snif_type == DBAL_ENUM_FVAL_SNIF_TYPE_SNOOP) ? BCM_MIRROR_DEST_IS_SNOOP :
        (snif_type == DBAL_ENUM_FVAL_SNIF_TYPE_STATISTICAL_SAMPLE) ? BCM_MIRROR_DEST_IS_STAT_SAMPLE : 0;

    sal_memset(mirror_header_info, 0, sizeof(bcm_mirror_header_info_t));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_SYSTEM_HEADER_TABLE, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

    
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_SNIFF_APPEND_MODE, INST_SINGLE, &append_mode);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_KEEP_INCOMING_SYSTEM_HEADRS, INST_SINGLE,
                              &is_keep_incoming_headers);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (append_mode == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This sniff profile %d is not worked in append mode.\n", action_profile_id);
    }

    
    if (is_keep_incoming_headers == TRUE)
    {
        *flags |= BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SNIF_NEW_SYSTEM_HEADER_TABLE, entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

    
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_NEW_SYSTEM_HEADER, INST_SINGLE, system_header);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HEADER_SIZE, INST_SINGLE, &header_size);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_APP_EXT_TYPE, INST_SINGLE, &ase_type);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_APPLICATION_EXT_STAMP_EN, INST_SINGLE, &ase_stamp_en);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_STACKING_EXT_OFFSET, INST_SINGLE,
                               &stacking_ext_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_TSH_STAMP_EN, INST_SINGLE, &tsh_stamp_en);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TSH_HDR_OFFSET, INST_SINGLE, &tsh_hdr_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_STAMP_EN, INST_SINGLE, &fhei_stamp_en);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_EXT_OFFSET, INST_SINGLE, &fhei_offset);
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_SNIF_NEW_SYSTEM_HEADER_TABLE, DBAL_FIELD_NEW_SYSTEM_HEADER, FALSE, 0, 0,
                     (int *) &total_len_in_bits));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    
    system_headers_mode = DNX_MIRROR_SYSTEM_HEADER_DATA_GET(unit, system_headers, system_headers_mode);
    if (system_headers_mode == DNX_MIRROR_SYSTEM_HEADER_DATA_GET(unit, system_headers, system_headers_mode_jericho2))
    {
        
        SHR_IF_ERR_EXIT(dnx_mirror_header_system_header_parser
                        (unit, *flags, ase_stamp_en, tsh_stamp_en, fhei_stamp_en, fhei_offset,
                         header_size, stacking_ext_offset, tsh_hdr_offset,
                         total_len_in_bits, system_header, ase_type, mirror_header_info));
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_mirror_header_legacy_system_header_parser
                        (unit, *flags, fhei_stamp_en, fhei_offset, stacking_ext_offset,
                         header_size, total_len_in_bits, system_header, mirror_header_info));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
