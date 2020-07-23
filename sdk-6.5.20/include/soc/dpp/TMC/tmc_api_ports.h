/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_PORTS_INCLUDED__

#define __SOC_TMC_API_PORTS_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_cnt.h>
#include <shared/port.h>






#define  SOC_TMC_PORTS_SYS_PHYS_PORT_ID_MAX 4095


#define  SOC_TMC_PORT_LAGS_MAX 1024


#define  SOC_TMC_PORTS_LAG_OUT_MEMBERS_MAX 256


#define  SOC_TMC_PORTS_LAG_MEMBER_ID_MAX    (SOC_TMC_PORTS_LAG_OUT_MEMBERS_MAX-1)


#define  SOC_TMC_PORTS_LAG_IN_MEMBERS_MAX SOC_TMC_NOF_FAP_PORTS


#define  SOC_TMC_PORTS_LAG_IN_MEMBERS_MAX_ARAD SOC_TMC_NOF_FAP_PORTS_ARAD


#define  SOC_TMC_PORTS_LAG_MEMBERS_MAX       SOC_TMC_PORTS_LAG_IN_MEMBERS_MAX
#define  SOC_TMC_PORTS_LAG_MEMBERS_MAX_ARAD  SOC_TMC_PORTS_LAG_IN_MEMBERS_MAX_ARAD


#define SOC_TMC_PORT_PP_PORT_RCY_OVERLAY_PTCH 0x1



#define SOC_TEST1_PORT_PP_PORT 0x2


#define SOC_PRESERVING_DSCP_PORT_PP_PORT 0x4

#define SOC_MIM_SPB_PORT 0x8

#define SOC_TMC_PORT_PRD_MAX_KEY_BUILD_OFFSETS 4

#define SOC_TMC_PORT_PRD_F_PORT_EXTERNDER 0x1










typedef enum
{
  
  SOC_TMC_PORT_DIRECTION_INCOMING=0,
  
  SOC_TMC_PORT_DIRECTION_OUTGOING=1,
  
  SOC_TMC_PORT_DIRECTION_BOTH=2,
  
  SOC_TMC_PORT_NOF_DIRECTIONS=3
}SOC_TMC_PORT_DIRECTION;

typedef enum
{
  
  SOC_TMC_PORT_HEADER_TYPE_NONE=0,
  
  SOC_TMC_PORT_HEADER_TYPE_ETH=1,
  
  SOC_TMC_PORT_HEADER_TYPE_RAW=2,
  
  SOC_TMC_PORT_HEADER_TYPE_TM=3,
  
  SOC_TMC_PORT_HEADER_TYPE_PROG=4,
  
  SOC_TMC_PORT_HEADER_TYPE_CPU=5,
  
  SOC_TMC_PORT_HEADER_TYPE_STACKING=6,
  
  SOC_TMC_PORT_HEADER_TYPE_TDM=7,
  
  SOC_TMC_PORT_HEADER_TYPE_TDM_RAW = 8,
  
  SOC_TMC_PORT_HEADER_TYPE_INJECTED = 9,
    
  SOC_TMC_PORT_HEADER_TYPE_INJECTED_PP = 10,
  
  SOC_TMC_PORT_HEADER_TYPE_INJECTED_2 = 11,
    
  SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP = 12,
  
  SOC_TMC_PORT_HEADER_TYPE_XGS_HQoS = 13,
  
  SOC_TMC_PORT_HEADER_TYPE_XGS_DiffServ = 14,
  
  SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT = 15,
  
  SOC_TMC_PORT_HEADER_TYPE_TDM_PMM=16,

  SOC_TMC_PORT_HEADER_TYPE_REMOTE_CPU=17,

  
  SOC_TMC_PORT_HEADER_TYPE_UDH_ETH=18,

    
  SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW=19,
  
 SOC_TMC_PORT_HEADER_TYPE_L2_ENCAP_EXTERNAL_CPU=20,

  
  SOC_TMC_PORT_HEADER_TYPE_COE,
  
  SOC_TMC_PORT_HEADER_TYPE_MIRROR_RAW,
  
  SOC_TMC_PORT_HEADER_TYPE_DSA_RAW,
  SOC_TMC_PORT_HEADER_TYPE_RAW_DSA,
  SOC_TMC_PORT_HEADER_TYPE_RCH_0,
  SOC_TMC_PORT_HEADER_TYPE_RCH_1,
  
  SOC_TMC_PORT_NOF_HEADER_TYPES
}SOC_TMC_PORT_HEADER_TYPE;

typedef enum
{
  
  SOC_TMC_PORTS_SNOOP_SIZE_BYTES_64=0,
  
  SOC_TMC_PORTS_SNOOP_SIZE_BYTES_192=2,
  
  SOC_TMC_PORTS_SNOOP_SIZE_ALL=3,
  
  SOC_TMC_PORTS_NOF_SNOOP_SIZES=4
}SOC_TMC_PORTS_SNOOP_SIZE;

typedef enum
{
  
  SOC_TMC_PORTS_FTMH_EXT_OUTLIF_NEVER=0,
  
  SOC_TMC_PORTS_FTMH_EXT_OUTLIF_IF_MC=1,
  
  SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ALWAYS=2,
  
  SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ENLARGE=4,
  
  SOC_TMC_PORTS_FTMH_EXT_OUTLIF_DOUBLE_TAG=3,
  
  SOC_TMC_PORTS_FTMH_NOF_EXT_OUTLIFS
}SOC_TMC_PORTS_FTMH_EXT_OUTLIF;

typedef enum
{
  
  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A=0,
  
  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE_B=1,
  
  SOC_TMC_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES=2
}SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE;

typedef enum
{
  
  SOC_TMC_PORTS_VT_PROFILE_NONE=0,
  
  SOC_TMC_PORTS_VT_PROFILE_OVERLAY_RCY=1,
  
  SOC_TMC_PORTS_VT_PROFILE_CUSTOM_PP=2,
  
  SOC_TMC_PORTS_NOF_VT_PROFILES
} SOC_TMC_PORTS_VT_PROFILE;

typedef enum
{
  
  SOC_TMC_PORTS_TT_PROFILE_NONE=0,
  
  SOC_TMC_PORTS_TT_PROFILE_OVERLAY_RCY=1,
  
  SOC_TMC_PORTS_TT_PROFILE_PON=2,
  
  SOC_TMC_PORTS_TT_PROFILE_FORCE_BRIDGE_FWD=3,
  
  SOC_TMC_PORTS_NOF_TT_PROFILES
} SOC_TMC_PORTS_TT_PROFILE;

typedef enum
{
  
  SOC_TMC_PORTS_FLP_PROFILE_NONE=0,
  
  SOC_TMC_PORTS_FLP_PROFILE_OVERLAY_RCY=1,
  
  SOC_TMC_PORTS_FLP_PROFILE_PON=2,
  
  SOC_TMC_PORTS_FLP_PROFILE_NON_BFD_FRWD=2,
  
  SOC_TMC_PORTS_FLP_PROFILE_OAMP=3,
  
  SOC_TMC_PORTS_FLP_PROFILE_IBC=3,
  
  SOC_TMC_PORTS_NOF_FLP_PROFILES
} SOC_TMC_PORTS_FLP_PROFILE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_INTERFACE_ID if_id;
  
  uint32 channel_id;
}SOC_TMC_PORT2IF_MAPPING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 sys_port;
  
  uint32 member_id;

    
  uint32 flags;
  
}  SOC_TMC_PORTS_LAG_MEMBER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 nof_entries;
  
  SOC_TMC_PORTS_LAG_MEMBER lag_member_sys_ports[SOC_TMC_PORTS_LAG_MEMBERS_MAX];

}  SOC_TMC_PORTS_LAG_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 nof_entries;
  
  SOC_TMC_PORTS_LAG_MEMBER lag_member_sys_ports[SOC_TMC_PORTS_LAG_MEMBERS_MAX_ARAD];

}  SOC_TMC_PORTS_LAG_INFO_ARAD;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 
  uint8 in_member;
 
  uint32 in_lag;
 
  uint8 nof_of_out_lags;
 
  uint32 out_lags[SOC_TMC_MAX_NOF_LAG_MEMBER_IN_GROUP];
}SOC_TMC_PORT_LAG_SYS_PORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  uint32 override_val;
}SOC_TMC_PORTS_OVERRIDE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  SOC_TMC_DEST_INFO destination;
  
  SOC_TMC_PORTS_OVERRIDE_INFO dp_override;
  
  SOC_TMC_PORTS_OVERRIDE_INFO tc_override;
}SOC_TMC_PORT_INBOUND_MIRROR_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  uint32 ifp_id;  
  
  uint8  skip_port_deafult_enable;
}SOC_TMC_PORT_OUTBOUND_MIRROR_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  SOC_TMC_PORTS_SNOOP_SIZE size_bytes;
  
  SOC_TMC_DEST_INFO destination;
  
  SOC_TMC_PORTS_OVERRIDE_INFO dp_override;
  
  SOC_TMC_PORTS_OVERRIDE_INFO tc_override;
}SOC_TMC_PORT_SNOOP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 pp_header_present;
  
  uint8 out_mirror_dis;
  
  uint32 snoop_cmd_ndx;
  
  uint8 exclude_src;
  
  uint32 tr_cls;
  
  uint32 dp;
  
  SOC_TMC_DEST_INFO destination;
}SOC_TMC_PORTS_ITMH_BASE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  uint8 is_src_sys_port;
  
  SOC_TMC_DEST_SYS_PORT_INFO src_port;
   
  SOC_TMC_DEST_INFO destination;
}SOC_TMC_PORTS_ITMH_EXT_SRC_PORT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_PORTS_ITMH_BASE base;
  
  SOC_TMC_PORTS_ITMH_EXT_SRC_PORT extension;
}SOC_TMC_PORTS_ITMH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  uint32 queue_id;
}SOC_TMC_PORTS_ISP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 cid;
  
  uint8 ifp;
  
  uint8 tr_cls;
  
  uint8 dp;
  
  uint8 data_type;
}SOC_TMC_PORTS_STAG_FIELDS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_PORTS_FTMH_EXT_OUTLIF outlif_ext_en;
  
  uint8 src_ext_en;
  
  uint8 dest_ext_en;
}SOC_TMC_PORTS_OTMH_EXTENSIONS_EN;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  int32 uc_credit_discount;
  
  int32 mc_credit_discount;
}SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO;

typedef enum
{
  
  SOC_TMC_PORTS_FC_TYPE_NONE = 0,
  
  SOC_TMC_PORTS_FC_TYPE_LL = 1,
  
  SOC_TMC_PORTS_FC_TYPE_CB2 = 2,
  
  SOC_TMC_PORTS_FC_TYPE_CB8 = 3,
  
  SOC_TMC_PORTS_NOF_FC_TYPES = 4
}SOC_TMC_PORTS_FC_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_stag_enabled;
  
  uint32 first_header_size;
  
  SOC_TMC_PORTS_FC_TYPE fc_type;
  
  SOC_TMC_PORT_HEADER_TYPE header_type;
  
  SOC_TMC_PORT_HEADER_TYPE header_type_out;
  
  uint8 is_snoop_enabled;
  
  uint32 mirror_profile;
  
  uint8 is_tm_ing_shaping_enabled;
  
  uint8 is_tm_pph_present_enabled;
  
  uint8 is_tm_src_syst_port_ext_present;
  
  uint32 flags;

} SOC_TMC_PORT_PP_PORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_CNT_PROCESSOR_ID processor_id;

  
  uint32 id;

} SOC_TMC_PORT_COUNTER_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_DEST_INFO destination;
  
  uint32 tr_cls;
  
  uint32 dp;
  
  uint32 snoop_cmd_ndx;
  
  SOC_TMC_PORT_COUNTER_INFO counter;

} SOC_TMC_PORTS_FORWARDING_HEADER_INFO;

typedef enum
{
  
  SOC_TMC_PORT_INJECTED_HDR_FORMAT_NONE = 0,
  
  SOC_TMC_PORT_INJECTED_HDR_FORMAT_PP_PORT = 1,
  
  SOC_TMC_PORT_NOF_INJECTED_HDR_FORMATS = 2
}SOC_TMC_PORT_INJECTED_HDR_FORMAT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_PORT_INJECTED_HDR_FORMAT hdr_format;
  
  uint32 pp_port_for_tm_traffic;

} SOC_TMC_PORT_INJECTED_CPU_HEADER_INFO;

typedef enum
{
    
    
  
  SOC_TMC_PORT_LAG_MODE_1K_16 = 0,
  
  SOC_TMC_PORT_LAG_MODE_512_32 = 1,
  
  SOC_TMC_PORT_LAG_MODE_256_64 = 2,
  
  SOC_TMC_PORT_LAG_MODE_128_128 = 3,
  
  SOC_TMC_PORT_LAG_MODE_64_256 = 4,
  
  SOC_TMC_PORT_LAG_MODE_32_64 = 2,
  
  SOC_TMC_PORT_LAG_MODE_16_128 = 3,
  
  SOC_TMC_PORT_LAG_MODE_8_256 = 4,
  
  SOC_TMC_NOF_PORT_LAG_MODES = 5
}SOC_TMC_PORT_LAG_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 pp_port;
}SOC_TMC_PORTS_PON_TUNNEL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint16 tpid;
  uint16 vlan;
  uint16 eth_type;
  uint8 sa[6];
  uint8 da[6];
}SOC_TMC_L2_ENCAP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
} SOC_TMC_PORTS_SWAP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_PORTS_VT_PROFILE ptc_vt_profile;
  
  SOC_TMC_PORTS_TT_PROFILE ptc_tt_profile;
  
  SOC_TMC_PORTS_FLP_PROFILE ptc_flp_profile;
  
  uint32 en_trill_mc_in_two_path;
} SOC_TMC_PORTS_PROGRAMS_INFO;


typedef struct {
    uint32 enable_rx;
    uint32 enable_tx;
    uint32 nof_seq_number_repetitions_rx;
    uint32 nof_seq_number_repetitions_tx;
    uint32 nof_requests_resent;
    uint32 rx_timeout_words;
    uint32 rx_timeout_sn;
    uint32 rx_ignore;
    uint32 rx_watchdog;
    uint32 buffer_size_entries;
    uint32 tx_wait_for_seq_num_change;
    uint32 tx_ignore_requests_when_fifo_almost_empty;
    uint32 rx_reset_when_error;
    uint32 rx_reset_when_watchdog_err;
    uint32 rx_reset_when_alligned_error;
    uint32 rx_reset_when_retry_error;
    uint32 rx_reset_when_wrap_after_disc_error;
    uint32 rx_reset_when_wrap_before_disc_error;
    uint32 rx_reset_when_timout_error;
    uint32 reserved_channel_id_rx;                 
    uint32 reserved_channel_id_tx;                 
    uint32 seq_number_bits_rx;                     
    uint32 seq_number_bits_tx;                     
    uint32 rx_discontinuity_event_timeout;         
    uint32 peer_tx_buffer_size;                    
} SOC_TMC_PORTS_ILKN_RETRANSMIT_CONFIG;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  uint32 metaframe_sync_period;
  uint32 interfcae_status_ignore;
  uint32 interfcae_status_oob_ignore;
  uint32 interleaved;
  uint32 mubits_tx_polarity;
  uint32 mubits_rx_polarity;
  uint32 fc_tx_polarity;
  uint32 fc_rx_polarity;
  uint32 dedicated_tdm_context;
  SOC_TMC_PORTS_ILKN_RETRANSMIT_CONFIG retransmit;

} SOC_TMC_PORTS_ILKN_CONFIG;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  uint8 rx_recovery_lane;

} SOC_TMC_PORTS_CAUI_CONFIG;


typedef enum
{
  SOC_TMC_PORTS_APPLICATION_MAPPING_TYPE_XGS_MAC_EXTENDER = 0,
  SOC_TMC_PORTS_APPLICATION_MAPPING_TYPE_PP_PORT_EXTENDER = 1,
  SOC_TMC_PORTS_APPLICATION_MAPPING_NOF_TYPES
} SOC_TMC_PORTS_APPLICATION_MAPPING_TYPE;

typedef union
{
  
  struct
  {
    
    uint32 hg_modid;
    
    uint32 hg_port;
    
    uint32 pp_port;
  } xgs_mac_ext;
  
  struct
  {
    
    uint32 index;
    
    uint32 pp_port;
    
    uint32 sys_port;
  } pp_port_ext;
} SOC_TMC_PORTS_APPLICATION_MAPPING_VALUE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  SOC_TMC_PORTS_APPLICATION_MAPPING_TYPE type;

  
  SOC_TMC_PORTS_APPLICATION_MAPPING_VALUE value;
   

} SOC_TMC_PORTS_APPLICATION_MAPPING_INFO;












void
  SOC_TMC_PORT2IF_MAPPING_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT2IF_MAPPING_INFO *info
  );

void
SOC_TMC_PORTS_LAG_MEMBER_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_LAG_MEMBER *info
  );

void
  SOC_TMC_PORTS_LAG_INFO_ARAD_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_LAG_INFO_ARAD *info
  );

void
  SOC_TMC_PORTS_LAG_INFO_PETRAB_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_LAG_INFO *info
  );

void
  SOC_TMC_PORTS_OVERRIDE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_OVERRIDE_INFO *info
  );

void
  SOC_TMC_PORT_INBOUND_MIRROR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_INBOUND_MIRROR_INFO *info
  );

void
  SOC_TMC_PORT_OUTBOUND_MIRROR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_OUTBOUND_MIRROR_INFO *info
  );

void
  SOC_TMC_PORT_SNOOP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_SNOOP_INFO *info
  );

void
  SOC_TMC_PORTS_ITMH_BASE_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_ITMH_BASE *info
  );

void
  SOC_TMC_PORTS_ITMH_EXT_SRC_PORT_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_ITMH_EXT_SRC_PORT *info
  );

void
  SOC_TMC_PORTS_ITMH_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_ITMH *info
  );

void
  SOC_TMC_PORTS_ISP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_ISP_INFO *info
  );

void
  SOC_TMC_PORT_LAG_SYS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_LAG_SYS_PORT_INFO *info
  );

void
  SOC_TMC_PORTS_STAG_FIELDS_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_STAG_FIELDS   *info
  );

void
  SOC_TMC_PORTS_OTMH_EXTENSIONS_EN_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_OTMH_EXTENSIONS_EN *info
  );

void
  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  );

void
  SOC_TMC_PORT_PP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_PP_PORT_INFO *info
  );

void
  SOC_TMC_PORT_COUNTER_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_COUNTER_INFO *info
  );

void
  SOC_TMC_PORTS_FORWARDING_HEADER_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_FORWARDING_HEADER_INFO *info
  );

void
  SOC_TMC_PORT_INJECTED_CPU_HEADER_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_INJECTED_CPU_HEADER_INFO *info
  );

void
  SOC_TMC_PORTS_SWAP_INFO_clear(
    SOC_TMC_PORTS_SWAP_INFO *info
  );

void
  SOC_TMC_PORTS_PON_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_PON_TUNNEL_INFO *info
  );

void
  SOC_TMC_PORTS_PROGRAMS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_PROGRAMS_INFO *info
  );

void
  SOC_TMC_L2_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_L2_ENCAP_INFO *info
  );

void
  SOC_TMC_PORTS_APPLICATION_MAPPING_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_APPLICATION_MAPPING_INFO *info
  );


#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_PORT_DIRECTION_to_string(
    SOC_SAND_IN SOC_TMC_PORT_DIRECTION enum_val
  );

const char*
  SOC_TMC_PORT_HEADER_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_PORT_HEADER_TYPE enum_val
  );

const char*
  SOC_TMC_PORTS_SNOOP_SIZE_to_string(
    SOC_SAND_IN SOC_TMC_PORTS_SNOOP_SIZE enum_val
  );

const char*
  SOC_TMC_PORTS_FTMH_EXT_OUTLIF_to_string(
    SOC_SAND_IN SOC_TMC_PORTS_FTMH_EXT_OUTLIF enum_val
  );

const char*
  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE enum_val
  );

const char*
  SOC_TMC_PORT_INJECTED_HDR_FORMAT_to_string(
    SOC_SAND_IN  SOC_TMC_PORT_INJECTED_HDR_FORMAT enum_val
  );

void
  SOC_TMC_PORT2IF_MAPPING_INFO_print(
    SOC_SAND_IN SOC_TMC_PORT2IF_MAPPING_INFO *info
  );

void
  SOC_TMC_PORTS_LAG_MEMBER_print(
    SOC_SAND_IN  SOC_TMC_PORTS_LAG_MEMBER *info
  );

void
  SOC_TMC_PORTS_LAG_INFO_print(
    SOC_SAND_IN SOC_TMC_PORTS_LAG_INFO *info
  );

void
  SOC_TMC_PORTS_OVERRIDE_INFO_print(
    SOC_SAND_IN SOC_TMC_PORTS_OVERRIDE_INFO *info
  );

void
  SOC_TMC_PORT_INBOUND_MIRROR_INFO_print(
    SOC_SAND_IN SOC_TMC_PORT_INBOUND_MIRROR_INFO *info
  );

void
  SOC_TMC_PORT_OUTBOUND_MIRROR_INFO_print(
    SOC_SAND_IN SOC_TMC_PORT_OUTBOUND_MIRROR_INFO *info
  );

void
  SOC_TMC_PORT_SNOOP_INFO_print(
    SOC_SAND_IN SOC_TMC_PORT_SNOOP_INFO *info
  );

void
  SOC_TMC_PORTS_ITMH_BASE_print(
    SOC_SAND_IN SOC_TMC_PORTS_ITMH_BASE *info
  );

void
  SOC_TMC_PORT_LAG_SYS_PORT_INFO_print(
    SOC_SAND_IN SOC_TMC_PORT_LAG_SYS_PORT_INFO *info
  );

void
  SOC_TMC_PORTS_ITMH_EXT_SRC_PORT_print(
    SOC_SAND_IN SOC_TMC_PORTS_ITMH_EXT_SRC_PORT *info
  );

void
  SOC_TMC_PORTS_ITMH_print(
    SOC_SAND_IN SOC_TMC_PORTS_ITMH *info
  );

void
  SOC_TMC_PORTS_ISP_INFO_print(
    SOC_SAND_IN SOC_TMC_PORTS_ISP_INFO *info
  );

void
  SOC_TMC_PORTS_STAG_FIELDS_print(
    SOC_SAND_IN SOC_TMC_PORTS_STAG_FIELDS   *info
  );

void
  SOC_TMC_PORTS_OTMH_EXTENSIONS_EN_print(
    SOC_SAND_IN SOC_TMC_PORTS_OTMH_EXTENSIONS_EN *info
  );

void
  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO_print(
    SOC_SAND_IN SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  );

const char*
  SOC_TMC_PORTS_FC_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PORTS_FC_TYPE enum_val
  );

void
  SOC_TMC_PORT_PP_PORT_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORT_PP_PORT_INFO *info
  );

void
  SOC_TMC_PORT_COUNTER_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORT_COUNTER_INFO *info
  );

void
  SOC_TMC_PORTS_FORWARDING_HEADER_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORTS_FORWARDING_HEADER_INFO *info
  );

void
  SOC_TMC_PORT_INJECTED_CPU_HEADER_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORT_INJECTED_CPU_HEADER_INFO *info
  );

void
  SOC_TMC_PORTS_SWAP_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORTS_SWAP_INFO *info
  );

void
  SOC_TMC_PORTS_PON_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORTS_PON_TUNNEL_INFO *info
  );

void
  SOC_TMC_PORTS_PROGRAMS_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORTS_PROGRAMS_INFO *info
  );



typedef struct {
    
    uint8 global_tag_swap_n_size;
    
    uint8 tpid_1_tag_swap_n_size;
    uint8 tpid_0_tag_swap_n_size;
    
    uint16 tag_swap_n_tpid_1;
    uint16 tag_swap_n_tpid_0;
    
    uint8 tag_swap_n_offset_1;
    uint8 tag_swap_n_offset_0;
} SOC_TMC_PORT_SWAP_GLOBAL_INFO;


typedef struct {
    uint64 mac_da_val; 
    uint64 mac_da_mask; 
    uint32 ether_type_code; 
    uint32 ether_type_code_mask; 
} SOC_TMC_PORT_PRD_CONTROL_PLANE;


typedef struct {
    uint32 ether_code; 
    uint32 ether_code_mask; 
    uint32 num_key_fields;              
    uint32 key_fields_values[SOC_TMC_PORT_PRD_MAX_KEY_BUILD_OFFSETS]; 
    uint32 key_fields_masks[SOC_TMC_PORT_PRD_MAX_KEY_BUILD_OFFSETS]; 
    uint32 priority;                   
} SOC_TMC_PORT_PRD_FLEX_KEY_ENTRY;

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

