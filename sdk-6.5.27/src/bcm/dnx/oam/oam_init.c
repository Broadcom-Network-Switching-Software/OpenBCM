
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/oam/oam_counter.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <include/soc/dnx/swstate/auto_generated/access/oam_access.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/switch/switch.h>
#include <src/bcm/dnx/oam/oamp_v1/oam_oamp_v1.h>
#ifdef DNX_EMULATION_1_CORE
#include <soc/sand/sand_aux_access.h>
#endif

#define MIP_ABOVE_MDL_NUM_BITS 1
#define PACKET_IS_BFD_NUM_BITS 1
#define NOF_MEP_BELOW_MDL_NUM_BITS 3
#define MDL_MP_TYPE_NUM_BITS 2
#define NOF_MEP_ABOVE_MDL_NUM_BITS 3

#define DNX_INVALID_PROFILE_ID -1

#define ACTIVE_INDEX 0

#define PASSIVE_INDEX 1

#define MIN_MEPS_ABOVE_FOR_MATCH_LOW 2

#define INGRESS_AND_EGRESS 2

#define IS_1588  0
#define IS_NTP   1

typedef struct
{
    int oam_opcode;
    dbal_enum_value_field_oam_internal_opcode_e internal_opcode;
} oam_opcode_mapping_table_table_entry_t;

typedef struct
{

    int oam_port_profile;

    int key_mep_type;

    dbal_enum_value_field_oam_sub_type_e key_subtype;

    dbal_enum_value_field_oam_process_action_e value_process_action;
} oam_process_map_table_entry_t;

#define NOF_OAM_OPCODE_DEFAULT_MAPPED_ENTRIES 15

#define OAM_EGRESS_PACKET_INJECTED_BITMAP_INIT_VAL     0x80

#define OAM_INJECT_PP_PORT_INIT_VAL                1

#define OAM_ETHERNET_CFG_INIT_VAL                0x4000

#define OAM_LIF_INVALID_HEADER_INIT_VAL           0

#define OAM_MPLS_TP_OR_BFD_ENABLE_MAP_INIT_VAL 0xFF

#define OAM_BFD_VALIDITY_CHECK_INIT_VAL           1

#define OAM_NO_BFD_AUTHENTICATION_INIT_VAL        0

#define MAX_OAM_OFFSET_INIT_VAL        0x100

#define OAM_MPLS_TP_FORCE_OPCODE_INIT_VAL         0

#define OAM_INVALID_LIF_INIT_VAL             0xFFFF

#define NUM_OAM_TS_REQ_SUB_TYPES 14
#define NUM_OAM_TS_REQ_SUB_TYPES_V2 15

#define OAM_TCAM_IDENTIFICATION_APPDB_0_ID  0

#define OAM_TCAM_IDENTIFICATION_APPDB_1_ID  1

#define OAM_PORT_PROFILE_TYPE_INJECT                0
#define OAM_PORT_PROFILE_TYPE_TRAP                  1

const uint8 match_high[] = {
    DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_HIGH,
    DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_HIGH
};

const uint8 match_low[] = {
    DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_LOW,
    DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_LOW
};

const uint8 match_middle[] = {
    DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_MIDDLE,
    DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_MIDDLE
};

const uint8 match_no_counter[] = {
    DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_NO_COUNTER,
    DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_NO_COUNTER
};


/* *INDENT-OFF* */

static const oam_opcode_mapping_table_table_entry_t oam_opcode_map[NOF_OAM_OPCODE_DEFAULT_MAPPED_ENTRIES] = {
        
        {DBAL_DEFINE_OAM_OPCODE_BFD_OR_OAM_DATA,  DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_BFD},
        {DBAL_DEFINE_OAM_OPCODE_CCM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_CCM},
        {DBAL_DEFINE_OAM_OPCODE_LBR,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LBR},
        {DBAL_DEFINE_OAM_OPCODE_LBM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LBM},
        {DBAL_DEFINE_OAM_OPCODE_LTR,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LTR},
        {DBAL_DEFINE_OAM_OPCODE_LTM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LTM},
        {DBAL_DEFINE_OAM_OPCODE_AIS,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_AIS},
        {DBAL_DEFINE_OAM_OPCODE_LINEAR_APS,       DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LINEAR_APS},
        {DBAL_DEFINE_OAM_OPCODE_LMR,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMR},
        {DBAL_DEFINE_OAM_OPCODE_LMM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMM},
        {DBAL_DEFINE_OAM_OPCODE_1DM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_1DM},
        {DBAL_DEFINE_OAM_OPCODE_DMR,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMR},
        {DBAL_DEFINE_OAM_OPCODE_DMM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMM},
        {DBAL_DEFINE_OAM_OPCODE_SLR,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLR},
        {DBAL_DEFINE_OAM_OPCODE_SLM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLM}
};



const oam_process_map_table_entry_t process_map_entries_shared[] =
{
    { OAM_PORT_PROFILE_TYPE_INJECT, DBAL_ENUM_FVAL_OAM_DIRECTION_UP_MEP,   DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_STAMP_COUNTER                   },
    { OAM_PORT_PROFILE_TYPE_INJECT, DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP, DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_READ_COUNTER_AND_STAMP          },
    { OAM_PORT_PROFILE_TYPE_INJECT, DBAL_ENUM_FVAL_OAM_DIRECTION_UP_MEP,   DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_INGRESS_STAMP_NTP               },
    { OAM_PORT_PROFILE_TYPE_INJECT, DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP, DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_NTP                },
    { OAM_PORT_PROFILE_TYPE_INJECT, DBAL_ENUM_FVAL_OAM_DIRECTION_UP_MEP,   DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_INGRESS_STAMP_1588              },
    { OAM_PORT_PROFILE_TYPE_INJECT, DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP, DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_1588               },
    { OAM_PORT_PROFILE_TYPE_INJECT, DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP, DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_REFLECTOR_1ST,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_INGRESS_STAMP_NTP               },
    { OAM_PORT_PROFILE_TYPE_INJECT, DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP, DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_REFLECTOR_2ND,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_NTP                },
    { OAM_PORT_PROFILE_TYPE_TRAP, DBAL_ENUM_FVAL_OAM_DIRECTION_UP_MEP,   DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_NTP                },
    { OAM_PORT_PROFILE_TYPE_TRAP, DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP, DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_NTP               },
    { OAM_PORT_PROFILE_TYPE_TRAP, DBAL_ENUM_FVAL_OAM_DIRECTION_UP_MEP,   DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_1588               },
    { OAM_PORT_PROFILE_TYPE_TRAP, DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP, DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_1588              },
    { OAM_PORT_PROFILE_TYPE_INJECT, DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP,   DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_TX,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_NTP               }
};


const uint8 OAM_TS_REQ_SUB_TYPES[NUM_OAM_TS_REQ_SUB_TYPES] = {
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_REFLECTOR_1ST,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_REFLECTOR_2ND,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_RFC8321,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_IPV6_RFC8321_ON_FLOW_ID,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_SFLOW_SEQUENCE_RESET,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOOPBACK,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_CCM,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_PASSIVE_ERROR,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_LEVEL_ERROR,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_IPFIX_TX_COMMAND,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_TX
};





shr_error_e
dnx_oam_port_profile_set(
    int unit)
{
    uint32 entry_handle_id;
    bcm_port_t port_i;
    bcm_pbmp_t pp_ports;
    int is_lag;
    int header_type = BCM_SWITCH_PORT_HEADER_TYPE_NONE;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_PP, 0, &pp_ports));
    BCM_PBMP_ITER(pp_ports, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port_i, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info.local_port, &port_info));
        SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, gport_info.local_port, &is_lag));
        if (!DNX_ALGO_PORT_TYPE_IS_EGR_PP(unit, port_info, is_lag))
        {
            continue;
        }

        
        SHR_IF_ERR_EXIT(dnx_switch_header_type_get(unit, port_i, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT, &header_type));
        if (header_type == BCM_SWITCH_PORT_HEADER_TYPE_CPU
            || header_type == BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                       gport_info.internal_port_pp_info.pp_port[0]);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_info.internal_port_pp_info.core_id[0]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PORT_PROFILE, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static int
dnx_oam_opcode_counter_offset(
    int opcode,
    dbal_enum_value_field_oam_internal_opcode_e internal_opcode)
{
    int offset;
    switch (internal_opcode)
    {
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMM:
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_1DM:
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMM:
        {
            offset = 4;
            break;
        }
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMR:
        {
            offset = 12;
            break;
        }
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLM:
        {
            offset = 12;
            break;
        }
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLR:
        {
            offset = 16;
            break;
        }
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMR:
        {
            offset = 20;
            break;
        }
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_CCM:
        {
            offset = 58;
            break;
        }
        default:
        {
            offset = 0;
            break;
        }
    }

    return offset;
}


static shr_error_e
dnx_oam_mpls_tp_mep_is_exist(
    int unit,
    uint8 *is_exist)
{
    int is_ep_end;
    uint32 entry_handle_id;
    bcm_oam_endpoint_type_t ep_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *is_exist = FALSE;

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_ENDPOINT_INFO_SW, &entry_handle_id));
    
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_ep_end));

    while (!is_ep_end)
    {
        
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_OAM_ENDPOINT_SW_STATE_TYPE, INST_SINGLE, &ep_type));

        if (ep_type == bcmOAMEndpointTypeBhhSection ||
            ep_type == bcmOAMEndpointTypeBHHMPLS || ep_type == bcmOAMEndpointTypeBHHPwe)
        {
            *is_exist = TRUE;
            break;
        }

        
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_ep_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_opcode_map_set(
    int unit,
    int opcode,
    dbal_enum_value_field_oam_internal_opcode_e internal_opcode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_ETHERNET_OAM_OPCODE_MAP, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, opcode);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INTERNAL_OPCODE, INST_SINGLE, internal_opcode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_STAMP_OFFSET, INST_SINGLE,
                                 dnx_oam_opcode_counter_offset(opcode, internal_opcode));

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_ETHERNET_OAM_OPCODE_MAP, entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, opcode);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INTERNAL_OPCODE, INST_SINGLE, internal_opcode);

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_opcode_map_init_set(
    int unit)
{
    uint32 entry_handle_id;
    uint32 index;
    int rv = _SHR_E_NONE;
    const oam_opcode_mapping_table_table_entry_t *tmp_oam_opcode_map;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_ETHERNET_OAM_OPCODE_MAP, &entry_handle_id));

    
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INTERNAL_OPCODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_OTHER);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_STAMP_OFFSET, INST_SINGLE, 0);

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_ETHERNET_OAM_OPCODE_MAP, entry_handle_id));

    

    
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INTERNAL_OPCODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_OTHER);

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    tmp_oam_opcode_map = oam_opcode_map;
    for (index = 0; index < NOF_OAM_OPCODE_DEFAULT_MAPPED_ENTRIES; index++, tmp_oam_opcode_map++)
    {
        rv = dnx_oam_opcode_map_set(unit, tmp_oam_opcode_map->oam_opcode, tmp_oam_opcode_map->internal_opcode);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_mpls_tp_mdl_ignore_tcam_entry_add(
    int unit,
    int opcode,
    dbal_enum_value_field_oam_internal_opcode_e internal_opcode)
{
    uint32 entry_handle_id;
    uint32 pkt_data[10] = { 0x0 };
    uint32 pkt_data_mask[10] = { 0x0 };
    uint32 zero_padding[3] = { 0x0 };
    
    int opcode_position = dnx_data_oam.general.oam_tcam_key_pkt_data_at_fwd_header_get(unit) - 16;
    int opcode_width = 8;
    uint32 bits_mask;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    bits_mask = (1 << (32 - opcode_position % 32)) - 1;
    pkt_data[opcode_position / 32] = (opcode & bits_mask) << (opcode_position % 32);
    pkt_data_mask[opcode_position / 32] = bits_mask << (opcode_position % 32);
    if (32 - opcode_position % 32 < opcode_width)
    {
        bits_mask = (1 << (opcode_width - (32 - opcode_position % 32))) - 1;
        pkt_data[opcode_position / 32 + 1] = (opcode >> (32 - opcode_position % 32)) & bits_mask;
        pkt_data_mask[opcode_position / 32 + 1] = bits_mask;
    }

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_TCAM_IDENTIFICATION_DB, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_attribute_set(unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, 3));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER_MINUS_1,
                               DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER,
                               DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_OAM_LIFS, 0x0, 0x0);
    dbal_entry_key_field_arr32_masked_set(unit, entry_handle_id, DBAL_FIELD_PKT_DATA_AT_FWD_HEADER,
                                          pkt_data, pkt_data_mask);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCAM_RESULT_TYPE, INST_SINGLE, 0x0);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_ZERO_PADDING_TCAM_IDENT, INST_SINGLE,
                                     zero_padding);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_OAM, INST_SINGLE, 0x1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_MD_LEVEL, INST_SINGLE, 0x7);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, INST_SINGLE, internal_opcode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_OFFSET, INST_SINGLE, 0x0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_STAMP_OFFSET, INST_SINGLE,
                                 dnx_oam_opcode_counter_offset(opcode, internal_opcode));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PCP, INST_SINGLE, 0x0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MY_CFM_MAC, INST_SINGLE, 0x0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_TCAM_INSTRUCTION, INST_SINGLE, 0x0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_IS_BFD, INST_SINGLE, 0x0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_YOUR_DISCR, INST_SINGLE, 0x0);

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_mpls_tp_mdl_ignore_tcam_entry_delete(
    int unit,
    int opcode,
    dbal_enum_value_field_oam_internal_opcode_e internal_opcode)
{
    uint32 entry_handle_id;
    uint32 entry_access_id;
    uint32 pkt_data[10] = { 0x0 };
    uint32 pkt_data_mask[10] = { 0x0 };
    
    int opcode_position = dnx_data_oam.general.oam_tcam_key_pkt_data_at_fwd_header_get(unit) - 16;
    int opcode_width = 8;
    uint32 bits_mask;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    bits_mask = (1 << (32 - opcode_position % 32)) - 1;
    pkt_data[opcode_position / 32] = (opcode & bits_mask) << (opcode_position % 32);
    pkt_data_mask[opcode_position / 32] = bits_mask << (opcode_position % 32);
    if (32 - opcode_position % 32 < opcode_width)
    {
        bits_mask = (1 << (opcode_width - (32 - opcode_position % 32))) - 1;
        pkt_data[opcode_position / 32 + 1] = (opcode >> (32 - opcode_position % 32)) & bits_mask;
        pkt_data_mask[opcode_position / 32 + 1] = bits_mask;
    }

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_TCAM_IDENTIFICATION_DB, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER_MINUS_1,
                               DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER,
                               DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_OAM_LIFS, 0x0, 0x0);

    dbal_entry_key_field_arr32_masked_set(unit, entry_handle_id, DBAL_FIELD_PKT_DATA_AT_FWD_HEADER,
                                          pkt_data, pkt_data_mask);

    
    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_mpls_tp_mdl_ignore_set(
    int unit,
    uint32 control_value)
{
    int index = 0;
    uint8 is_exist = 0;
    uint32 current_value = 0;
    const oam_opcode_mapping_table_table_entry_t *tmp_oam_opcode_map = oam_opcode_map;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_mpls_tp_mep_is_exist(unit, &is_exist));
    if (is_exist)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ignoring mpls-tp OAM MDL can not be set when MPLS MEP exists");
    }

    SHR_IF_ERR_EXIT(algo_oam_db.mpls_tp_mdl_ignore.get(unit, &current_value));
    if (control_value > 0 && current_value > 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ignoring mpls-tp OAM MDL is already enabled");
    }
    if (control_value == 0 && current_value == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ignoring mpls-tp OAM MDL is already disbled");
    }

    if (control_value)
    {
        
        for (index = 0; index < NOF_OAM_OPCODE_DEFAULT_MAPPED_ENTRIES; index++, tmp_oam_opcode_map++)
        {
            SHR_IF_ERR_EXIT(dnx_oam_mpls_tp_mdl_ignore_tcam_entry_add(unit, tmp_oam_opcode_map->oam_opcode,
                                                                      tmp_oam_opcode_map->internal_opcode));
        }
    }
    else
    {
        
        for (index = 0; index < NOF_OAM_OPCODE_DEFAULT_MAPPED_ENTRIES; index++, tmp_oam_opcode_map++)
        {
            SHR_IF_ERR_EXIT(dnx_oam_mpls_tp_mdl_ignore_tcam_entry_delete(unit, tmp_oam_opcode_map->oam_opcode,
                                                                         tmp_oam_opcode_map->internal_opcode));
        }
    }

    SHR_IF_ERR_EXIT(algo_oam_db.mpls_tp_mdl_ignore.set(unit, control_value));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_mpls_tp_mdl_ignore_get(
    int unit,
    uint32 *control_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_oam_db.mpls_tp_mdl_ignore.get(unit, control_value));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_twamp_stateless_mode_set(
    int unit,
    int value)
{
    uint32 entry_handle_id;
    uint32 context_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_TWAMP, &entry_handle_id));
    if (value)
    {
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_TERM_CONTEXT_ID,
                                                      DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMP_STATELESS_2ND_PASS,
                                                      &context_val));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_TERM_CONTEXT_ID,
                                                      DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMP_2ND_PASS,
                                                      &context_val));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TERM_CONTEXT_REFLECTORS_TWAMP_2ND_PASS, INST_SINGLE,
                                 context_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_twamp_stateless_mode_get(
    int unit,
    int *value)
{
    uint32 entry_handle_id;
    uint32 context_val = 0, stateless_context_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *value = 0;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_TWAMP, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TERM_CONTEXT_REFLECTORS_TWAMP_2ND_PASS, INST_SINGLE,
                               &context_val);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_TERM_CONTEXT_ID,
                                                  DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMP_STATELESS_2ND_PASS,
                                                  &stateless_context_val));

    if (context_val == stateless_context_val)
    {
        *value = 1;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



static int
dnx_oam_calculate_mp_type_from_key(
    int mip_above_mdl,
    int packet_is_bfd,
    int nof_mep_below_mdl,
    int mdl_mp_type,
    int nof_mep_above_mdl)
{
    
    uint8 active_or_passive;
    if (packet_is_bfd)
    {
        if ((mip_above_mdl) && (nof_mep_above_mdl == DNX_OAM_MAX_MDL) && (nof_mep_below_mdl == DNX_OAM_MAX_MDL))
        {
            
            return DBAL_ENUM_FVAL_MP_TYPE_ABOVE_UPPER_MEP;
        }
        
        return DBAL_ENUM_FVAL_MP_TYPE_BFD;
    }
    else
    {
        
        if (mdl_mp_type == DBAL_ENUM_FVAL_MDL_MP_TYPE_MIP)
        {
            
            return DBAL_ENUM_FVAL_MP_TYPE_MIP_MATCH;
        }
        else
        {
            
            if (mip_above_mdl && (nof_mep_above_mdl == 0) && (mdl_mp_type == DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP))
            {
                
                if (nof_mep_below_mdl == 0)
                {
                    
                    return DBAL_ENUM_FVAL_MP_TYPE_BELOW_MIP;
                }
                else
                {
                    
                    return DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIP_AND_MEP;
                }
            }
            else
            {
                
                if (mdl_mp_type == DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP)
                {
                    
                    if (nof_mep_above_mdl == 0)
                    {
                        
                        return DBAL_ENUM_FVAL_MP_TYPE_ABOVE_UPPER_MEP;
                    }
                    else
                    {
                        if (nof_mep_below_mdl == 0)
                        {
                            
                            return DBAL_ENUM_FVAL_MP_TYPE_BELOW_LOWER_MEP;
                        }
                        else
                        {

                            
                            if (nof_mep_above_mdl == 1)
                            {
                                
                                return DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_HIGH_AND_MIDDLE;
                            }
                            else
                            {
                                if (nof_mep_below_mdl == 1)
                                {
                                    
                                    return DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIDDLE_AND_LOW;
                                }
                                else
                                {
                                    
                                    return DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_HIGH_AND_MIDDLE;
                                }
                            }

                        }
                    }
                }
                else
                {
                    
                    active_or_passive = (mdl_mp_type == DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP) ?
                        ACTIVE_INDEX : PASSIVE_INDEX;
                    if ((nof_mep_below_mdl == 0) && (nof_mep_above_mdl >= MIN_MEPS_ABOVE_FOR_MATCH_LOW))
                    {
                        
                        return match_low[active_or_passive];
                    }
                    else
                    {
                        if (nof_mep_above_mdl == 0)
                        {
                            
                            return match_high[active_or_passive];
                        }
                        else
                        {
                            
                            if ((nof_mep_below_mdl == 1) || ((nof_mep_below_mdl == 0) && (nof_mep_above_mdl == 1)))
                            {
                                return match_middle[active_or_passive];
                            }
                            else
                            {
                                
                                return match_no_counter[active_or_passive];
                            }
                        }
                    }
                }
            }
        }
    }
}


static shr_error_e
dnx_oam_mp_type_table_init(
    int unit)
{
    int mip_above_mdl, packet_is_bfd, nof_mep_below_mdl;
    int table_index, mdl_mp_type, nof_mep_above_mdl, mp_type;
    uint32 entry_handle_id[2];
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_OAM_MP_TYPE_MAP, &entry_handle_id[0]));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_MP_TYPE_MAP, &entry_handle_id[1]));

    
    for (mip_above_mdl = 0; mip_above_mdl < SAL_BIT(MIP_ABOVE_MDL_NUM_BITS); mip_above_mdl++)
    {
        for (packet_is_bfd = 0; packet_is_bfd < SAL_BIT(PACKET_IS_BFD_NUM_BITS); packet_is_bfd++)
        {
            for (nof_mep_below_mdl = 0; nof_mep_below_mdl < SAL_BIT(NOF_MEP_BELOW_MDL_NUM_BITS); nof_mep_below_mdl++)
            {
                for (mdl_mp_type = 0; mdl_mp_type < SAL_BIT(MDL_MP_TYPE_NUM_BITS); mdl_mp_type++)
                {
                    for (nof_mep_above_mdl = 0; nof_mep_above_mdl < SAL_BIT(NOF_MEP_ABOVE_MDL_NUM_BITS);
                         nof_mep_above_mdl++)
                    {
                        
                        mp_type = dnx_oam_calculate_mp_type_from_key(mip_above_mdl, packet_is_bfd, nof_mep_below_mdl,
                                                                     mdl_mp_type, nof_mep_above_mdl);

                        
                        for (table_index = 0; table_index < INGRESS_AND_EGRESS; table_index++)
                        {
                            
                            dbal_entry_key_field32_set(unit, entry_handle_id[table_index],
                                                       DBAL_FIELD_MIP_ABOVE_MDL, mip_above_mdl);
                            dbal_entry_key_field32_set(unit, entry_handle_id[table_index],
                                                       DBAL_FIELD_PACKET_IS_BFD, packet_is_bfd);
                            dbal_entry_key_field32_set(unit, entry_handle_id[table_index],
                                                       DBAL_FIELD_NOF_MEP_BELOW_MDL, nof_mep_below_mdl);
                            dbal_entry_key_field32_set(unit, entry_handle_id[table_index],
                                                       DBAL_FIELD_MDL_MP_TYPE, mdl_mp_type);
                            dbal_entry_key_field32_set(unit, entry_handle_id[table_index],
                                                       DBAL_FIELD_NOF_MEP_ABOVE_MDL, nof_mep_above_mdl);

                            
                            dbal_entry_value_field32_set(unit, entry_handle_id[table_index],
                                                         DBAL_FIELD_MP_TYPE, INST_SINGLE, mp_type);

                            
                            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id[table_index], DBAL_COMMIT));
                        }
                    }
                }
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_sub_type_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_OAM, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_OAM_SUB_TYPE_DM_1588_INDICATION, INST_SINGLE, 1);
        
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_udh_size_init(
    int unit)
{
    uint32 entry_handle_id;
    int system_headers_mode;
    uint32 udh_size = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
    {
        udh_size = BITS2BYTES(dnx_data_field.udh.field_class_id_size_0_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_1_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_2_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_3_get(unit));

        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_OAM, &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_UDH_SIZE_JR1, INST_SINGLE, udh_size);
        
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_sub_type_properties_init(
    int unit)
{
    uint32 entry_handle_id;
    int index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_TS_HEADER_PER_SUBTYPE, &entry_handle_id));

    
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUILD_OAM_TS_HEADER, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    for (index = 0; index < NUM_OAM_TS_REQ_SUB_TYPES; index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY, OAM_TS_REQ_SUB_TYPES[index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUILD_OAM_TS_HEADER, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_OAM_SUB_TYPE_TO_DATA_TYPE_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_TOD_1588);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_TOD_NTP);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY, DBAL_ENUM_FVAL_OAM_SUB_TYPE_RFC8321);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_COUNTER_VALUE);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_SFLOW_SEQUENCE_RESET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_COUNTER_VALUE);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_IPFIX_TX_COMMAND);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_COUNTER_VALUE);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY, DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOOPBACK);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_NONE);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY, DBAL_ENUM_FVAL_OAM_SUB_TYPE_CCM);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_NONE);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_PASSIVE_ERROR);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_NONE);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_LEVEL_ERROR);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_NONE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_SUB_TYPE_TO_DATA_TYPE_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_TOD_1588);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_TOD_NTP);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_COUNTER_VALUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



static shr_error_e
dnx_oam_init_general_configuration(
    int unit)
{
    uint32 entry_handle_id;
    int index, nof_process_map_entries = 0;
    uint32 nof_rifs = 4096;
    uint32 val_arr[2] = { 0 };
    dbal_enum_value_field_oam_process_action_e value_process_action;
    int oam_y1711_enable = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    oam_y1711_enable = dnx_data_oam.general.feature_get(unit, dnx_data_oam_general_oam_y1711_enable) ? 1 : 0;

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_INJECT_ENABLE,
                                 INST_SINGLE, OAM_INJECT_PP_PORT_INIT_VAL);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_PRT_QUALIFIER_INJECTED_INDICATION, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER,
                               DBAL_ENUM_FVAL_PRT_QUALIFIER_OAMP_INJECTION);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_PACKET_INJECTED_INDICATION, INST_SINGLE,
                                 TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_GENERAL_CONFIGURATION, &entry_handle_id));
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_PACKET_INJECTED_BITMAP,
                                 INST_SINGLE, OAM_EGRESS_PACKET_INJECTED_BITMAP_INIT_VAL);

    
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_MAP_STRENGTH_TO_OAM_INJECTION,
                                 INST_SINGLE, OAM_EGRESS_PACKET_INJECTED_BITMAP_INIT_VAL);
    
    val_arr[0] = OAM_ETHERNET_CFG_INIT_VAL;
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_ETHERNET_NEXT_PROTOCOL_OAM, INST_SINGLE,
                                     val_arr);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_INVALID_HEADER,
                                 INST_SINGLE, OAM_LIF_INVALID_HEADER_INIT_VAL);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_BFD_VALIDITY_CHECK,
                                 INST_SINGLE, OAM_BFD_VALIDITY_CHECK_INIT_VAL);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_NO_BFD_AUTHENTICATION,
                                 INST_SINGLE, OAM_NO_BFD_AUTHENTICATION_INIT_VAL);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_MPLS_TP_FORCE_OPCODE,
                                 INST_SINGLE, OAM_MPLS_TP_FORCE_OPCODE_INIT_VAL);

    if (dnx_data_oam.general.feature_get(unit, dnx_data_oam_general_max_oam_offset_support))
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_OAM_OFFSET,
                                     INST_SINGLE, MAX_OAM_OFFSET_INIT_VAL);
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_MAX_OAM_OFFSET_DROP, INST_SINGLE, 0);
    }

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INVALID_LIF, INST_SINGLE,
                                 OAM_INVALID_LIF_INIT_VAL);

    {
        if (dnx_data_oam.general.oam_mpls_tp_or_bfd_enable_map_exists_get(unit))
        {
        
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_MPLS_TP_OR_BFD_ENABLE_MAP,
                                         INST_SINGLE, OAM_MPLS_TP_OR_BFD_ENABLE_MAP_INIT_VAL);
        }
    }

    
    nof_rifs = dnx_data_l3.rif.nof_rifs_get(unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RIF_ID_THRESHOLD, INST_SINGLE, nof_rifs - 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_PROCESS_MAP, &entry_handle_id));

        
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PORT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_MEP_TYPE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

        
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PROCESS_ACTION,
                                 INST_SINGLE, DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_NONE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    nof_process_map_entries = sizeof(process_map_entries_shared) / sizeof(oam_process_map_table_entry_t);

    
    
    for (index = 0; index < nof_process_map_entries; index++)
    {
        
        if ((process_map_entries_shared[index].key_subtype == DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_TX) &&
            !dnx_data_oam.general.feature_get(unit, dnx_data_oam_general_oam_sub_type_twamp_tx_support))
        {
            continue;
        }

        value_process_action = process_map_entries_shared[index].value_process_action;

        
        if (oam_y1711_enable)
        {
            if ((process_map_entries_shared[index].oam_port_profile == OAM_PORT_PROFILE_TYPE_INJECT) &&
                (process_map_entries_shared[index].key_mep_type == DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP) &&
                (process_map_entries_shared[index].key_subtype == DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP))
            {
                value_process_action = DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588;
            }
        }

        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_PROFILE,
                                   process_map_entries_shared[index].oam_port_profile);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_MEP_TYPE,
                                   process_map_entries_shared[index].key_mep_type);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE,
                                   process_map_entries_shared[index].key_subtype);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PROCESS_ACTION,
                                     INST_SINGLE, value_process_action);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    if (DNX_OAMP_IS_V2(unit))
    {
    }

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_LM_READ_PER_SUB_TYPE_DISABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT);
    if (dnx_data_oam.general.oam_lm_read_per_sub_disable_field_exists_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LM_READ_INDEX_DISABLE, INST_SINGLE, 0);
    }
    if (dnx_data_oam.general.oam_lm_read_per_sub_enable_field_exists_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LM_READ_INDEX_ENABLE, INST_SINGLE, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));


    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAM_TCAM_IDENTIFICATION_APPDB, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IDENTIFICATION_APPDB_0, INST_SINGLE,
                                 OAM_TCAM_IDENTIFICATION_APPDB_0_ID);
    if (dnx_data_oam.general.oam_identification_appdb_1_field_exists_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IDENTIFICATION_APPDB_1, INST_SINGLE,
                                     OAM_TCAM_IDENTIFICATION_APPDB_1_ID);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    {
        SHR_IF_ERR_EXIT(dnx_oam_sub_type_properties_init(unit));
    }


    if (dnx_data_oam.general.feature_get(unit, dnx_data_oam_general_oam_twamp_stateless_mode_support))
    {
        SHR_IF_ERR_EXIT(dnx_oam_twamp_stateless_mode_set(unit, 0));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_init_tod(
    int unit)
{
    uint32 entry_handle_id;
    int tod_config_in_eci;
    int tod_config_in_ns;
    int tod_config_in_oam;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    tod_config_in_eci = dnx_data_oam.general.oam_tod_config_in_eci_get(unit);
    if (tod_config_in_eci)
    {

        
        SHR_IF_ERR_EXIT(dnx_oam_tod_set(unit, IS_1588, 0x1ffff0013576543));

        
        SHR_IF_ERR_EXIT(dnx_oam_tod_set(unit, IS_NTP, 0x1ffff0013576543));

        
    }

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_TOD_GENERAL_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECI_NSE_RESET, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAM_TOD_GENERAL_CONFIGURATION, entry_handle_id));
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECI_TOD_MODE, INST_SINGLE, 3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_CMIC_TOD_MODE, INST_SINGLE, 3);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAM_TOD_GENERAL_CONFIGURATION, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECI_NSE_RESET, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAM_TOD_GENERAL_CONFIGURATION, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TODW_SELECT_IEEE_1588_TIMER, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TODW_SELECT_NTP_TIMER, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECI_NSE_NCO_FREQ_CONTROL, INST_SINGLE,
                                 dnx_data_oam.general.oam_nse_nco_freq_control_get(unit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    tod_config_in_oam = dnx_data_oam.general.tod_init_in_oam_get(unit);
    tod_config_in_ns = dnx_data_oam.general.oam_tod_config_in_ns_get(unit);
    if ((tod_config_in_oam) && (tod_config_in_ns)
        && (dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_ts_freq_lock) != 0))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_1588_TOD_CONFIGURATION_NS, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NS_IEEE1588_TIME_FREQ_CONTROL_LOWER, INST_SINGLE,
                                     0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NS_IEEE1588_TIME_FREQ_CONTROL_UPPER, INST_SINGLE,
                                     dnx_data_oam.general.oam_ns_freq_control_upper_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NS_IEEE1588_TIME_CONTROL_LOAD_ENABLE,
                                     INST_SINGLE, 0x21);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NS_IEEE1588_TIME_CONTROL_LOAD_TYPE, INST_SINGLE,
                                     0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NS_IEEE1588_TIME_CONTROL_COUNTER_ENABLE,
                                     INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_init_acc_egr_profile(
    int unit)
{
    shr_error_e rv;
    int oam_lif = 0;
    int mdl = 0;
    int mep_id = 0;
    int reserved_acc_egr_profile_id = dnx_data_oam.general.oam_egr_acc_action_profile_id_for_inject_mpls_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (reserved_acc_egr_profile_id != DNX_INVALID_PROFILE_ID)
    {
        rv = dnx_oam_acc_mep_add(unit, _SHR_CORE_ALL, 0, oam_lif, mdl, mep_id, reserved_acc_egr_profile_id);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_init(
    int unit)
{
    int rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    
    rv = dnx_oam_key_select_ingress_init(unit);
    SHR_IF_ERR_EXIT(rv);

    rv = dnx_oam_key_select_egress_init(unit);
    SHR_IF_ERR_EXIT(rv);

    
    rv = dnx_oam_opcode_map_init_set(unit);
    SHR_IF_ERR_EXIT(rv);

    
    rv = dnx_oam_counters_init(unit);
    SHR_IF_ERR_EXIT(rv);

    
    rv = dnx_oam_init_acc_egr_profile(unit);
    SHR_IF_ERR_EXIT(rv);

    
    SHR_IF_ERR_EXIT(dnx_oam_mp_type_table_init(unit));

    
    rv = dnx_oam_init_general_configuration(unit);
    SHR_IF_ERR_EXIT(rv);

    {
        
        SHR_IF_ERR_EXIT(dnx_oam_sub_type_init(unit));

        
        SHR_IF_ERR_EXIT(dnx_oam_udh_size_init(unit));
    }

    rv = dnx_oam_sw_db_init(unit);
    SHR_IF_ERR_EXIT(rv);

    {
        
        
        rv = dnx_oam_init_tod(unit);
        SHR_IF_ERR_EXIT(rv);
    }

    
    rv = dnx_oam_oamp_init(unit);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oam_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_deinit(unit));

    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_deinit(unit));

exit:SHR_FUNC_EXIT;
}
