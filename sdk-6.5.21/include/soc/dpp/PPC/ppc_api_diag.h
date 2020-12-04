/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_DIAG_INCLUDED__

#define __SOC_PPC_API_DIAG_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_frwrd_bmact.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>
#include <soc/dpp/PPC/ppc_api_frwrd_fcf.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ilm.h>
#include <soc/dpp/PPC/ppc_api_lif.h>
#include <soc/dpp/PPC/ppc_api_rif.h>
#include <soc/dpp/PPC/ppc_api_llp_sa_auth.h>
#include <soc/dpp/PPC/ppc_api_llp_vid_assign.h>
#include <soc/dpp/PPC/ppc_api_llp_parse.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv6.h>
#include <soc/dpp/PPC/ppc_api_frwrd_trill.h>
#include <soc/dpp/PPC/ppc_api_lif_ing_vlan_edit.h>
#include <soc/dpp/PPC/ppc_api_eg_encap.h>
#include <soc/dpp/PPC/ppc_api_eg_vlan_edit.h>
#include <soc/dpp/PPC/ppc_api_eg_ac.h>

#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_db.h>






#define  SOC_PPC_DIAG_MAX_NOF_HDRS (7)


#define  SOC_PPC_DIAG_TCAM_KEY_NOF_UINT32S_MAX (9)


#define  SOC_PPC_DIAG_BUFF_MAX_SIZE (32)

#define SOC_PPC_DIAG_FILTER_SECOND_SET   (0x80000000)









typedef enum
{
  SOC_PPC_DIAG_OK = 0,
  SOC_PPC_DIAG_NOT_FOUND = 1
} SOC_PPC_DIAG_RESULT;

typedef enum
{
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_NONE = 0,
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT = 1,
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_BMACT = 2,
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC = 3,
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC = 4,
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN = 5,
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC = 6,
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC = 7,
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_VPN = 8,
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM = 9,
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC = 10,
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC = 11,
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_HOST = 12,
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE_FCF = 13,
  
  SOC_PPC_NOF_DIAG_FWD_LKUP_TYPES = 14
}SOC_PPC_DIAG_FWD_LKUP_TYPE;


typedef enum
{
  
  SOC_PPC_DIAG_LEARN_COUNTER_INGRESS_LKUP = 0,
  
  SOC_PPC_DIAG_LEARN_COUNTER_EGRESS_LKUP = 1,
  
  SOC_PPC_DIAG_LEARN_COUNTER_MACT_EVENTS = 2,
  
  SOC_PPC_DIAG_LEARN_COUNTER_OLP_REQUESTS = 3,
  
  SOC_PPC_DIAG_LEARN_COUNTER_OLP_TX_MSGS = 4,
  
  SOC_PPC_DIAG_LEARN_COUNTER_MACT_REQUESTS = 5,
  
  SOC_PPC_NOF_LEARN_COUNTER_TYPES = 10
}SOC_PPC_DIAG_LEARN_COUNTER_TYPE;


typedef enum
{
  
  SOC_PPC_DIAG_TCAM_USAGE_FRWRDING = 0,
  
  SOC_PPC_DIAG_TCAM_USAGE_PMF = 1,
  
  SOC_PPC_DIAG_TCAM_USAGE_EGR_ACL = 2,
  
  SOC_PPC_NOF_DIAG_TCAM_USAGES = 3
}SOC_PPC_DIAG_TCAM_USAGE;

typedef enum
{
  
  SOC_PPC_DIAG_FLAVOR_NONE = 0x0,
  
  SOC_PPC_DIAG_FLAVOR_RAW = 0x1,
  
  SOC_PPC_DIAG_FLAVOR_CLEAR_ON_GET = 0x2,
  
  SOC_PPC_NOF_DIAG_FLAVORS = 0x4
}SOC_PPC_DIAG_FLAVOR;

typedef enum
{
  
  SOC_PPC_DIAG_LEM_LKUP_TYPE_NONE = 0,
  
  SOC_PPC_DIAG_LEM_LKUP_TYPE_MACT = 1,
  
  SOC_PPC_DIAG_LEM_LKUP_TYPE_BMACT = 2,
  
  SOC_PPC_DIAG_LEM_LKUP_TYPE_HOST = 3,
  
  SOC_PPC_DIAG_LEM_LKUP_TYPE_ILM = 4,
  
  SOC_PPC_DIAG_LEM_LKUP_TYPE_SA_AUTH = 5,
  
  SOC_PPC_DIAG_LEM_LKUP_TYPE_EXTEND_P2P = 6,
  
  SOC_PPC_DIAG_LEM_LKUP_TYPE_TRILL_ADJ = 7,
  
  SOC_PPC_NOF_DIAG_LEM_LKUP_TYPES = 8
}SOC_PPC_DIAG_LEM_LKUP_TYPE;

typedef enum
{
  
  SOC_PPC_DIAG_LIF_LKUP_TYPE_NONE = 0,
  
  SOC_PPC_DIAG_LIF_LKUP_TYPE_AC = 1,
  
  SOC_PPC_DIAG_LIF_LKUP_TYPE_PWE = 2,
  
  SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL = 3,
  
  SOC_PPC_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL = 4,
  
  SOC_PPC_DIAG_LIF_LKUP_TYPE_TRILL = 5,
  
  SOC_PPC_DIAG_LIF_LKUP_TYPE_MIM_ISID = 6,

  
  SOC_PPC_DIAG_LIF_LKUP_TYPE_L2GRE = 7,
  
  SOC_PPC_DIAG_LIF_LKUP_TYPE_VXLAN = 8,
  
  SOC_PPC_DIAG_LIF_LKUP_TYPE_EXTENDER = 9,
  
  SOC_PPC_DIAG_LIF_LKUP_TYPE_DOUBLE_DATA_ENTRY = 10,
  
  SOC_PPC_NOF_DIAG_LIF_LKUP_TYPES = 11
}SOC_PPC_DIAG_LIF_LKUP_TYPE;

typedef enum
{
  
  SOC_PPC_DIAG_DB_TYPE_LIF = 0,
  
  SOC_PPC_DIAG_DB_TYPE_LEM = 1,
  
  SOC_PPC_DIAG_DB_TYPE_LPM = 2,
  
  SOC_PPC_DIAG_DB_TYPE_TCAM_FRWRD = 3,
  
  SOC_PPC_DIAG_DB_TYPE_TCAM_PMF = 4,
  
  SOC_PPC_DIAG_DB_TYPE_TCAM_EGR = 5,
  
  SOC_PPC_DIAG_DB_TYPE_EG_LIF = 6,
  
  SOC_PPC_DIAG_DB_TYPE_EG_TUNNEL = 7,
  
  SOC_PPC_DIAG_DB_TYPE_EG_LL = 8,
  
  SOC_PPC_NOF_DIAG_DB_TYPES = 9
}SOC_PPC_DIAG_DB_TYPE;

typedef enum
{
  
  SOC_PPC_DIAG_FRWRD_DECISION_PHASE_INIT_PORT = 0,
  
  SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF = 1,
  
  SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND = 2,
  
  SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF = 3,
  
  SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC = 4,
  
  SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP = 5,
  
  SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED = 6,
  
  SOC_PPC_NOF_DIAG_FRWRD_DECISION_PHASES = 7
}SOC_PPC_DIAG_FRWRD_DECISION_PHASE;

typedef enum
{
  
  SOC_PPC_DIAG_MPLS_TERM_TYPE_NONE = 0,
  
  SOC_PPC_DIAG_MPLS_TERM_TYPE_RANGE = 1,
  
  SOC_PPC_DIAG_MPLS_TERM_TYPE_LIF_LKUP = 2,
  
  SOC_PPC_NOF_DIAG_MPLS_TERM_TYPES = 3
}SOC_PPC_DIAG_MPLS_TERM_TYPE;

typedef enum
{
  
  SOC_PPC_DIAG_PKT_TRACE_LIF = 1,
  
  SOC_PPC_DIAG_PKT_TRACE_TUNNEL_RNG = 0x2,
  
  SOC_PPC_DIAG_PKT_TRACE_TRAP = 0x4,
  
  SOC_PPC_DIAG_PKT_TRACE_FEC = 0x8,
  
  SOC_PPC_DIAG_PKT_TRACE_EG_DROP_LOG = 0x10,
  
  SOC_PPC_DIAG_PKT_TRACE_FLP = 0x20,
  
  SOC_PPC_DIAG_PKT_TRACE_PMF = 0x40,
  
  SOC_PPC_DIAG_PKT_TRACE_ALL = (int)0xFFFFFFFF,
  
  SOC_PPC_NOF_DIAG_PKT_TRACES = 6
}SOC_PPC_DIAG_PKT_TRACE;

typedef enum
{
  
  SOC_PPC_DIAG_EG_DROP_REASON_NONE = 0x0,
  
  SOC_PPC_DIAG_EG_DROP_REASON_CNM = 0x1,
  
  SOC_PPC_DIAG_EG_DROP_REASON_CFM_TRAP = 0x2,
  
  SOC_PPC_DIAG_EG_DROP_REASON_NO_VSI_TRANSLATION = 0x4,
  
  SOC_PPC_DIAG_EG_DROP_REASON_DSS_STACKING = 0x8,
  
  SOC_PPC_DIAG_EG_DROP_REASON_LAG_MULTICAST = 0x10,
  
  SOC_PPC_DIAG_EG_DROP_REASON_EXCLUDE_SRC = 0x20,
  
  SOC_PPC_DIAG_EG_DROP_REASON_VLAN_MEMBERSHIP = 0x40,
  
  SOC_PPC_DIAG_EG_DROP_REASON_UNACCEPTABLE_FRAME_TYPE = 0x80,
  
  SOC_PPC_DIAG_EG_DROP_REASON_SRC_EQUAL_DEST = 0x100,
  
  SOC_PPC_DIAG_EG_DROP_REASON_UNKNOWN_DA = 0x200,
  
  SOC_PPC_DIAG_EG_DROP_REASON_SPLIT_HORIZON = 0x400,
  
  SOC_PPC_DIAG_EG_DROP_REASON_PRIVATE_VLAN = 0x800,
  
  SOC_PPC_DIAG_EG_DROP_REASON_TTL_SCOPE = 0x1000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_MTU_VIOLATION = 0x2000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_TRILL_TTL_ZERO = 0x4000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_TRILL_SAME_INTERFACE = 0x8000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_BOUNCE_BACK = 0x10000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_ILLEGAL_EEP = 0x20000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_ILLEGAL_EEI = 0x40000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_PHP_CONFLICT = 0x80000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_POP_IP_VERSION_ERR = 0x100000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET = 0x200000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IP_TUNNEL_SNOOPED_PACKET = 0x400000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_EEDB_LINK_LAYER_ENTRY_NEEDED = 0x2000000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_STP_BLOCK = 0x4000000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_OUT_LIF_WITH_DROP = 0x8000000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_EEDB_LAST_ENTRY_NOT_AC = 0x10000000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_OTM_INVALID = 0x20000000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV4_VERSION_ERR = 0x40000000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV6_VERSION_ERR = (int)0x80000001,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV4_CHECKSUM_ERR = (int)0x80000002,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV4_IHL_SHORT = (int)0x80000004,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV4_TOTAL_LEGNTH =(int) 0x80000008,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV4_TTL_1 = (int)0x80000010,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV6_TTL_1 = (int)0x80000020,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV4_WITH_OPTIONS = (int)0x80000040,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV4_TTL_0 = (int)0x80000080,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV6_TTL_0 = (int)0x80000100,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV4_SIP_EQUAL_DIP = (int)0x80000200,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV4_DIP_IS_ZERO = (int)0x80000400,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV4_SIP_IS_MC = (int)0x80000800,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SIP_IS_MC = (int)0x80001000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV6_DIP_UNSPECIFIED = (int)0x80002000,
  
  SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SIP_UNSPECIFIED = (int)0x80004000,
  
   SOC_PPC_DIAG_EG_DROP_REASON_IPV6_LOOPBACK = (int)0x80008000,
  
   SOC_PPC_DIAG_EG_DROP_REASON_IPV6_HOP_BY_HOP = (int)0x80010000,
  
   SOC_PPC_DIAG_EG_DROP_REASON_IPV6_LINK_LOCAL_DEST = (int)0x80020000,
  
   SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SITE_LOCAL_DEST = (int)0x80040000,
  
   SOC_PPC_DIAG_EG_DROP_REASON_IPV6_LINK_LOCAL_SRC = (int)0x80080000,
  
   SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SITE_LOCAL_SRC = (int)0x80100000,
  
   SOC_PPC_DIAG_EG_DROP_REASON_IPV6_IPV4_COMPATIBLE = (int)0x80200000,
  
   SOC_PPC_DIAG_EG_DROP_REASON_IPV6_IPV4_MAPPED = (int)0x80400000,
  
   SOC_PPC_DIAG_EG_DROP_REASON_IPV6_DEST_MC = (int)0x80800000,
  
  SOC_PPC_NOF_DIAG_EG_DROP_REASONS = 56
}SOC_PPC_DIAG_EG_DROP_REASON;

typedef enum
{
  
  SOC_PPC_DIAG_PKT_TM_FIELD_TC = 0x1,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_DP = 0x2,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_DEST = 0x4,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_METER1 = 0x8,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_METER2 = 0x10,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_MTR_CMD = 0x20,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER1 = 0x40,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER2 = 0x80,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_CUD = 0x100,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_ETH_METER_PTR = 0x200,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_ING_SHAPING_DA = 0x400,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_ECN_CAPABLE = 0x800,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_CNI = 0x1000,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_DA_TYPE = 0x2000,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_ST_VSQ_PTR = 0x4000,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_LAG_LB_KEY = 0x8000,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_IGNORE_CP = 0x10000,
  
  SOC_PPC_DIAG_PKT_TM_FIELD_SNOOP_ID = 0x20000,
  
  SOC_PPC_NOF_DIAG_PKT_TM_FIELDS = 18
}SOC_PPC_DIAG_PKT_TM_FIELD;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 buff[SOC_PPC_DIAG_BUFF_MAX_SIZE];
  
  uint32 buff_len;

} SOC_PPC_DIAG_BUFFER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 flavor;

} SOC_PPC_DIAG_MODE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 vsi;

} SOC_PPC_DIAG_VSI_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 lkup_num;

  
  uint32 bank_id;

} SOC_PPC_DIAG_DB_USE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 lkup_num;
  
  SOC_PPC_DIAG_TCAM_USAGE lkup_usage;

} SOC_PPC_DIAG_TCAM_USE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_DIAG_BUFFER packet_header;
  
  uint32 in_tm_port;
  
  SOC_SAND_PP_SYS_PORT_ID src_sys_port;
  
  SOC_PPC_PORT in_pp_port;
  
  SOC_TMC_PORT_HEADER_TYPE pp_context;
  
  uint32 packet_qual;


} SOC_PPC_DIAG_RECEIVED_PACKET_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_pkt_trapped;
  
  SOC_PPC_TRAP_CODE code;
  
  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO info;

} SOC_PPC_DIAG_TRAP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_TRAP_CODE code;
  
  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO info;

} SOC_PPC_DIAG_SNOOP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 trap_stack[SOC_PPC_NOF_TRAP_CODES/SOC_SAND_NOF_BITS_IN_UINT32];
  
  uint8 trap_stack_strength[SOC_PPC_NOF_TRAP_CODES];
  
  uint8 additional_trap_stack_strength[SOC_PPC_NOF_TRAP_CODES];
  
  SOC_PPC_DIAG_TRAP_INFO committed_trap;
  
  SOC_PPC_DIAG_SNOOP_INFO committed_snoop;
  
  SOC_PPC_DIAG_TRAP_INFO trap_updated_dest;

} SOC_PPC_DIAG_TRAPS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 cpu_dest;

} SOC_PPC_DIAG_TRAP_TO_CPU_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
   SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY subnet;
  
   uint32 vrf_id;

} SOC_PPC_DIAG_IPV4_VPN_ROUTE_KEY;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
   SOC_SAND_PP_SYS_PORT_ID sys_port_id;
  
   SOC_SAND_PP_MAC_ADDRESS mac_key;

} SOC_PPC_DIAG_TRILL_ADJ_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
    
  
  uint32 trill_key_type;
  uint32 nick_name;
  uint32 ing_nick;
  uint32 dist_tree;
  uint32 link_adj_id;
  uint32 native_inner_tpid;
  uint32 port;
  uint32 vsi;
  uint32 high_vid;
  uint32 low_vid;
  uint32 flags;  
} SOC_PPC_DIAG_TRILL_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 raw[6];
  
  SOC_PPC_L2_LIF_AC_KEY ac;
  
  SOC_SAND_PP_MPLS_LABEL pwe;
  
  SOC_PPC_MPLS_LABEL_RIF_KEY mpls;
  
  SOC_PPC_RIF_IP_TERM_KEY ip_tunnel;
  
  SOC_PPC_L2_LIF_ISID_KEY mim;
  
  uint32 nick_name; 
  
  SOC_PPC_DIAG_TRILL_KEY trill;
  
  SOC_PPC_L2_LIF_GRE_KEY l2gre;
  
  SOC_PPC_L2_LIF_VXLAN_KEY vxlan;
  
  SOC_PPC_L2_LIF_EXTENDER_KEY extender;

} SOC_PPC_DIAG_LIF_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
   SOC_PPC_DIAG_LIF_KEY key;
  
   SOC_PPC_DIAG_LIF_LKUP_TYPE type;

} SOC_PPC_DIAG_EXTEND_P2P_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 raw[4];
  
  SOC_PPC_FRWRD_MACT_ENTRY_KEY mact;
  
  SOC_PPC_BMACT_ENTRY_KEY bmact;
  
  SOC_PPC_DIAG_IPV4_VPN_ROUTE_KEY host;
  
  SOC_PPC_FRWRD_ILM_KEY ilm;
  
  SOC_SAND_PP_MAC_ADDRESS sa_auth;
  
  SOC_PPC_DIAG_TRILL_ADJ_KEY trill_adj;
  
  SOC_PPC_DIAG_EXTEND_P2P_KEY extend_p2p_key;

} SOC_PPC_DIAG_LEM_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 raw[4];
  
  SOC_PPC_L2_LIF_AC_INFO ac;
  
  SOC_PPC_L2_LIF_PWE_INFO pwe;
  
  SOC_PPC_MPLS_TERM_INFO mpls;
  
  SOC_PPC_RIF_IP_TERM_INFO ip;
  
  SOC_PPC_L2_LIF_ISID_INFO mim;
  
  SOC_PPC_L2_LIF_TRILL_INFO trill;
  
  SOC_PPC_L2_LIF_GRE_INFO l2gre;
  
  SOC_PPC_L2_LIF_VXLAN_INFO vxlan;
  
  SOC_PPC_L2_LIF_EXTENDER_INFO extender;

} SOC_PPC_DIAG_LIF_VALUE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_DIAG_LIF_LKUP_TYPE type;
  
  SOC_PPC_DIAG_LIF_KEY key;
  
  uint32 base_index;
  
  uint32 opcode_id;
  
  SOC_PPC_DIAG_LIF_VALUE value;
  
  uint8 found;

} SOC_PPC_DIAG_LIF_LKUP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 val[SOC_PPC_DIAG_TCAM_KEY_NOF_UINT32S_MAX];
  
  uint32 mask[SOC_PPC_DIAG_TCAM_KEY_NOF_UINT32S_MAX];
  
  uint32 length;

} SOC_PPC_DIAG_TCAM_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 fec_ptr;

} SOC_PPC_DIAG_TCAM_VALUE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 raw[2];
  
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE mact;
  
  SOC_PPC_BMACT_ENTRY_INFO bmact;
  
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO host;
  
  SOC_PPC_FRWRD_DECISION_INFO ilm;
  
  SOC_PPC_LLP_SA_AUTH_MAC_INFO sa_auth;
  
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO sa_vid_assign;

} SOC_PPC_DIAG_LEM_VALUE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_ETH_ENCAP_TYPE encap_type;
  
  SOC_PPC_LLP_PARSE_INFO tag_fromat;
  
  SOC_SAND_PP_VLAN_TAG vlan_tags[2];
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_tag_format;
  
  SOC_PPC_L2_NEXT_PRTCL_TYPE next_prtcl;

} SOC_PPC_DIAG_PARSING_L2_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 bos;

} SOC_PPC_DIAG_PARSING_MPLS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_L3_NEXT_PRTCL_TYPE next_prtcl;
  
  uint8 is_mc;
  
  uint8 is_fragmented;
  
  uint8 hdr_err;

} SOC_PPC_DIAG_PARSING_IP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_PKT_HDR_TYPE hdr_type;
  
  uint32 hdr_offset;
  
  SOC_PPC_DIAG_PARSING_L2_INFO eth;
  
  SOC_PPC_DIAG_PARSING_IP_INFO ip;
  
  SOC_PPC_DIAG_PARSING_MPLS_INFO mpls;

} SOC_PPC_DIAG_PARSING_HEADER_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  int pfc_hw;

   
  SOC_SAND_PP_VLAN_ID initial_vid;

  
  SOC_PPC_DIAG_PARSING_HEADER_INFO hdrs_stack[SOC_PPC_DIAG_MAX_NOF_HDRS];

} SOC_PPC_DIAG_PARSING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 range_index;
  
  SOC_PPC_MPLS_TERM_LABEL_RANGE range;

} SOC_PPC_DIAG_TERM_MPLS_LABEL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_PKT_TERM_TYPE term_type;
  
  SOC_PPC_PKT_FRWRD_TYPE frwrd_type;
  
  uint8 is_scnd_my_mac;

} SOC_PPC_DIAG_TERM_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 vrf;
  
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY key;

} SOC_PPC_DIAG_IPV4_VPN_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 vrf;
  
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY key;

} SOC_PPC_DIAG_IPV6_VPN_KEY;

#if defined(INCLUDE_KBP)
typedef struct{
    SOC_SAND_MAGIC_NUM_VAR

    uint16 vrf;

    uint32 sip;

    uint32 dip;
}SOC_PPC_DIAG_IPV4_UNICAST_RPF;

typedef struct{
    SOC_SAND_MAGIC_NUM_VAR

    uint16 vrf;

    uint16 in_rif;

    uint32 sip;

    uint32 dip;
}SOC_PPC_DIAG_IPV4_MULTICAST;

typedef struct{
    SOC_SAND_MAGIC_NUM_VAR

    uint16 vrf;

    SOC_SAND_PP_IPV6_ADDRESS sip;

    SOC_SAND_PP_IPV6_ADDRESS dip;
}SOC_PPC_DIAG_IPV6_UNICAST_RPF;

typedef struct{
    SOC_SAND_MAGIC_NUM_VAR

    uint16 vrf;

    uint16 in_rif;

    SOC_SAND_PP_IPV6_ADDRESS sip;

    SOC_SAND_PP_IPV6_ADDRESS dip;
}SOC_PPC_DIAG_IPV6_MULTICAST;

typedef struct{
    SOC_SAND_MAGIC_NUM_VAR

    uint16 in_rif;

    uint8 in_port;

    uint8 exp;

    uint32 mpls_label;
}SOC_PPC_DIAG_MPLS;

typedef struct{
    SOC_SAND_MAGIC_NUM_VAR

    uint16 egress_nick;
}SOC_PPC_DIAG_TRILL_UNICAST;

typedef struct{
    SOC_SAND_MAGIC_NUM_VAR

    uint8 esdai;

    uint16 fid_vsi;

    uint16 dist_tree_nick;
}SOC_PPC_DIAG_TRILL_MULTICAST;
#endif 

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_FRWRD_MACT_ENTRY_KEY mact;
  
  SOC_PPC_BMACT_ENTRY_KEY bmact;
  
  SOC_PPC_DIAG_IPV4_VPN_KEY ipv4_uc;
  
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY ipv4_mc;
  
  SOC_PPC_DIAG_IPV6_VPN_KEY ipv6_uc;
  
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY ipv6_mc;
  
  SOC_PPC_FRWRD_ILM_KEY ilm;
  
  uint32 trill_uc;
  
  SOC_PPC_TRILL_MC_ROUTE_KEY trill_mc;
  
  SOC_PPC_FRWRD_FCF_ROUTE_KEY fcf;
  
  uint32 raw[2];
#if defined(INCLUDE_KBP)
  
  SOC_PPC_DIAG_IPV4_UNICAST_RPF kbp_ipv4_unicast_rpf;
  
  SOC_PPC_DIAG_IPV4_MULTICAST kbp_ipv4_multicast;
  
  SOC_PPC_DIAG_IPV6_UNICAST_RPF kbp_ipv6_unicast_rpf;
  
  SOC_PPC_DIAG_IPV6_MULTICAST kbp_ipv6_multicast;
  
  SOC_PPC_DIAG_MPLS kbp_mpls;
  
  SOC_PPC_DIAG_TRILL_UNICAST kbp_trill_unicast;
  
  SOC_PPC_DIAG_TRILL_MULTICAST kbp_trill_multicast;
#endif 

} SOC_PPC_DIAG_FRWRD_LKUP_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE mact;
  
  SOC_PPC_BMACT_ENTRY_INFO bmact;
  
  SOC_PPC_FRWRD_DECISION_INFO frwrd_decision;
  
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO host_info;
  
  uint32 raw[2];

} SOC_PPC_DIAG_FRWRD_LKUP_VALUE;

#if defined(INCLUDE_KBP)
typedef struct{
    SOC_SAND_MAGIC_NUM_VAR

    uint8 match_status;

    uint8 is_synamic;

    uint8 p2p_service;

    uint8 identifier;

    uint8 out_lif_valid;

    uint16 out_lif;

    SOC_PPC_FRWRD_DECISION_INFO destination;

    uint32 eei;

    uint16 fec_ptr;
}SOC_PPC_DIAG_IP_REPLY_RECORD;

typedef struct{
    SOC_SAND_MAGIC_NUM_VAR

    uint8 match_status;

    uint16 dest_id;
} SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD;

#endif 

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_DIAG_FWD_LKUP_TYPE frwrd_type;
  
  uint8 frwrd_hdr_index;
  
  SOC_PPC_DIAG_FRWRD_LKUP_KEY lkup_key;
  
  uint8 key_found;
  
  SOC_PPC_DIAG_FRWRD_LKUP_VALUE lkup_res;
#if defined(INCLUDE_KBP)

  uint8 is_kbp;

  SOC_PPC_DIAG_IP_REPLY_RECORD ip_reply_record;

  SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD second_ip_reply_result;
#endif 

} SOC_PPC_DIAG_FRWRD_LKUP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 valid;
  
  uint8 ingress;
  
  uint8 is_new_key;
  
  SOC_PPC_FRWRD_MACT_ENTRY_KEY key;
  
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE value;

} SOC_PPC_DIAG_LEARN_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 cmd_id;
  
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO cmd_info;
  
  SOC_SAND_PP_VLAN_TAG ac_tag;
  
  SOC_SAND_PP_VLAN_TAG ac_tag2;

  
  uint32 adv_mode;

} SOC_PPC_DIAG_VLAN_EDIT_RES;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_FRWRD_DECISION_INFO frwrd_decision;

 
  uint32 outlif;

} SOC_PPC_DIAG_FRWRD_DECISION_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_DIAG_FRWRD_DECISION_INFO frwrd[SOC_PPC_NOF_DIAG_FRWRD_DECISION_PHASES];

  
  SOC_PPC_TRAP_INFO trap[SOC_PPC_NOF_DIAG_FRWRD_DECISION_PHASES];

  
  uint32 trap_qual[SOC_PPC_NOF_DIAG_FRWRD_DECISION_PHASES];

} SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 valid_fields;
  
  SOC_PPC_FRWRD_DECISION_INFO frwrd_decision;
  
  SOC_SAND_PP_TC tc;
  
  SOC_SAND_PP_DP dp;
  
  uint32 meter1;
  
  uint32 meter2;
  
  uint32 dp_meter_cmd;
  
  uint32 counter1;
  
  uint32 counter2;
  
  uint32 cud;
  
  uint32 eth_meter_ptr;
  
  uint32 ingress_shaping_da;
  
  uint32 ecn_capable;
  
  uint32 cni;
  
  SOC_SAND_PP_ETHERNET_DA_TYPE da_type;
  
  uint32 st_vsq_ptr;
  
  uint32 lag_lb_key;
  
  uint32 ignore_cp;
  
  uint32 snoop_id;
} SOC_PPC_DIAG_PKT_TM_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_MPLS_COMMAND mpls_cmd;
  
  SOC_PPC_EG_ENCAP_ENTRY_INFO encap_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
  
  uint32 eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
  
  uint32 nof_eeps;
  
  SOC_PPC_VSI_ID ll_vsi;
  
  SOC_PPC_AC_ID out_ac;
  
  SOC_PPC_TM_PORT tm_port;
  
  SOC_PPC_PORT pp_port;
  
  uint8 rif_is_valid;
  
  uint32 out_rif;
  
  uint32 lif_profile;
  
  SOC_PPC_EEI eei;

} SOC_PPC_DIAG_ENCAP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 drop_log[2];

} SOC_PPC_DIAG_EG_DROP_LOG_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 ether_type;
  
  SOC_SAND_PP_MAC_ADDRESS da;
  
  SOC_SAND_PP_MAC_ADDRESS sa;
  
  uint8 is_tagged;
  
  SOC_SAND_PP_VLAN_TAG tag;

} SOC_PPC_DIAG_ETH_PACKET_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY key;  
    
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO cmd_info;  
   
  SOC_SAND_PP_VLAN_TAG ac_tag;  
   
   SOC_SAND_PP_VLAN_TAG ac_tag2;
} SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO;



typedef enum soc_ppc_diag_glem_outlif_source_e {
    soc_ppc_diag_glem_outlif_source_ftmh,
    soc_ppc_diag_glem_outlif_source_eei,
    soc_ppc_diag_glem_outlif_source_cud1,
    soc_ppc_diag_glem_outlif_source_cud2,
    soc_ppc_diag_glem_outlif_source_user,
    soc_ppc_diag_glem_outlif_source_count
} soc_ppc_diag_glem_outlif_source_t;

typedef struct soc_ppc_diag_glem_outlif_s {
    int global_outlif;
    int local_outlif;
    soc_ppc_diag_glem_outlif_source_t source;
    uint8 found;
    uint8 accessed;
    uint8 mapped;
} soc_ppc_diag_glem_outlif_t;

typedef struct soc_ppc_diag_glem_signals_s {
    soc_ppc_diag_glem_outlif_t  outlifs[2];
} soc_ppc_diag_glem_signals_t;










void
  SOC_PPC_DIAG_BUFFER_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_BUFFER *info
  );

void
  SOC_PPC_DIAG_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_MODE_INFO *info
  );

void
  SOC_PPC_DIAG_VSI_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_VSI_INFO *info
  );

void
  SOC_PPC_DIAG_DB_USE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_DB_USE_INFO *info
  );

void
  SOC_PPC_DIAG_TCAM_USE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TCAM_USE_INFO *info
  );

void
  SOC_PPC_DIAG_RECEIVED_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_RECEIVED_PACKET_INFO *info
  );

void
  SOC_PPC_DIAG_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TRAP_INFO *info
  );

void
  SOC_PPC_DIAG_SNOOP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_SNOOP_INFO *info
  );

void
  SOC_PPC_DIAG_TRAPS_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TRAPS_INFO *info
  );

void
  SOC_PPC_DIAG_TRAP_TO_CPU_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TRAP_TO_CPU_INFO *info
  );

void
  SOC_PPC_DIAG_IPV4_VPN_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV4_VPN_ROUTE_KEY *info
  );

void
  SOC_PPC_DIAG_LEM_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_KEY *info
  );

void
  SOC_PPC_DIAG_LIF_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_LIF_KEY *info
  );

void
  SOC_PPC_DIAG_LIF_VALUE_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_LIF_VALUE *info
  );

void
  SOC_PPC_DIAG_LIF_LKUP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_LIF_LKUP_INFO *info
  );

void
  SOC_PPC_DIAG_TCAM_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TCAM_KEY *info
  );

void
  SOC_PPC_DIAG_TCAM_VALUE_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TCAM_VALUE *info
  );

void
  SOC_PPC_DIAG_LEM_VALUE_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_VALUE *info
  );

void
  SOC_PPC_DIAG_PARSING_L2_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_L2_INFO *info
  );

void
  SOC_PPC_DIAG_PARSING_MPLS_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_MPLS_INFO *info
  );

void
  SOC_PPC_DIAG_PARSING_IP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_IP_INFO *info
  );

void
  SOC_PPC_DIAG_PARSING_HEADER_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_HEADER_INFO *info
  );

void
  SOC_PPC_DIAG_PARSING_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_INFO *info
  );

void
  SOC_PPC_DIAG_TERM_MPLS_LABEL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TERM_MPLS_LABEL_INFO *info
  );

void
  SOC_PPC_DIAG_TERM_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TERM_INFO *info
  );

void
  SOC_PPC_DIAG_IPV4_VPN_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV4_VPN_KEY *info
  );

void
  SOC_PPC_DIAG_IPV6_VPN_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV6_VPN_KEY *info
  );

#if defined(INCLUDE_KBP)
void
  SOC_PPC_DIAG_IPV4_UNICAST_RPF_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV4_UNICAST_RPF *info
  );

void
  SOC_PPC_DIAG_IPV4_MULTICAST_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV4_MULTICAST *info
  );

void
  SOC_PPC_DIAG_IPV6_UNICAST_RPF_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV6_UNICAST_RPF *info
  );

void
  SOC_PPC_DIAG_IPV6_MULTICAST_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV6_MULTICAST *info
  );

void
  SOC_PPC_DIAG_MPLS_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_MPLS *info
  );

void
  SOC_PPC_DIAG_TRILL_UNICAST_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TRILL_UNICAST *info
  );

void
  SOC_PPC_DIAG_TRILL_MULTICAST_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TRILL_MULTICAST *info
  );

void
  SOC_PPC_DIAG_IP_REPLY_RECORD_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IP_REPLY_RECORD *info
  );

void
  SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD *info
  );
#endif 

void
  SOC_PPC_DIAG_FRWRD_LKUP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_LKUP_KEY *info
  );

void
  SOC_PPC_DIAG_FRWRD_LKUP_VALUE_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_LKUP_VALUE *info
  );

void
  SOC_PPC_DIAG_FRWRD_LKUP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_LKUP_INFO *info
  );

void
  SOC_PPC_DIAG_LEARN_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_LEARN_INFO *info
  );

void
  SOC_PPC_DIAG_VLAN_EDIT_RES_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_VLAN_EDIT_RES *info
  );

void
  SOC_PPC_DIAG_FRWRD_DECISION_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_DECISION_INFO *info
  );

void
  SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO *info
  );

void
  SOC_PPC_DIAG_PKT_TM_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_PKT_TM_INFO *info
  );

void
  SOC_PPC_DIAG_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_ENCAP_INFO *info
  );

void
  SOC_PPC_DIAG_EG_DROP_LOG_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_EG_DROP_LOG_INFO *info
  );

void
  SOC_PPC_DIAG_ETH_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_ETH_PACKET_INFO *info
  );

void 
  SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO *prm_vec_res
  );

void
  SOC_PPC_DIAG_TRILL_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TRILL_KEY *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_DIAG_FWD_LKUP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_FWD_LKUP_TYPE enum_val
  );

const char*
  SOC_PPC_DIAG_TCAM_USAGE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_TCAM_USAGE enum_val
  );

const char*
  SOC_PPC_DIAG_FLAVOR_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_FLAVOR enum_val
  );

const char*
  SOC_PPC_DIAG_LEM_LKUP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_LEM_LKUP_TYPE enum_val
  );

const char*
  SOC_PPC_DIAG_LIF_LKUP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_LIF_LKUP_TYPE enum_val
  );

const char*
  SOC_PPC_DIAG_DB_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_DB_TYPE enum_val
  );

const char*
  SOC_PPC_DIAG_FRWRD_DECISION_PHASE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_FRWRD_DECISION_PHASE enum_val
  );

const char*
  SOC_PPC_DIAG_MPLS_TERM_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_MPLS_TERM_TYPE enum_val
  );

const char*
  SOC_PPC_DIAG_PKT_TRACE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_PKT_TRACE enum_val
  );

const char*
  SOC_PPC_DIAG_EG_DROP_REASON_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_EG_DROP_REASON enum_val
  );

const char*
  SOC_PPC_DIAG_PKT_TM_FIELD_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_PKT_TM_FIELD enum_val
  );

void
  SOC_PPC_DIAG_BUFFER_print(
    SOC_SAND_IN  SOC_PPC_DIAG_BUFFER *info
  );

void
  SOC_PPC_DIAG_MODE_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_MODE_INFO *info
  );

void
  SOC_PPC_DIAG_VSI_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_VSI_INFO *info
  );

void
  SOC_PPC_DIAG_DB_USE_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO *info
  );

void
  SOC_PPC_DIAG_TCAM_USE_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TCAM_USE_INFO *info
  );

void
  SOC_PPC_DIAG_RECEIVED_PACKET_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_RECEIVED_PACKET_INFO *info
  );

void
  SOC_PPC_DIAG_TRAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TRAP_INFO *info
  );

void
  SOC_PPC_DIAG_SNOOP_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_SNOOP_INFO *info
  );

void
  SOC_PPC_DIAG_TRAPS_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TRAPS_INFO *info
  );

void
  SOC_PPC_DIAG_TRAP_TO_CPU_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TRAP_TO_CPU_INFO *info
  );

void
  SOC_PPC_DIAG_IPV4_VPN_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_IPV4_VPN_ROUTE_KEY *info
  );

void
  SOC_PPC_DIAG_EXTEND_P2P_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_EXTEND_P2P_KEY *info
  );


void
  SOC_PPC_DIAG_LEM_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_LEM_KEY *info,
    SOC_SAND_IN  SOC_PPC_DIAG_LEM_LKUP_TYPE type
  );

void
  SOC_PPC_DIAG_LIF_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_LIF_KEY *info,
    SOC_SAND_IN  SOC_PPC_DIAG_LIF_LKUP_TYPE lkup_type
  );

void
  SOC_PPC_DIAG_LIF_VALUE_print(
    SOC_SAND_IN  SOC_PPC_DIAG_LIF_VALUE *info,
    SOC_SAND_IN  SOC_PPC_DIAG_LIF_LKUP_TYPE lkup_type
  );

void
  SOC_PPC_DIAG_LIF_LKUP_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_LIF_LKUP_INFO *info
  );

void
  SOC_PPC_DIAG_TCAM_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TCAM_KEY *info
  );

void
  SOC_PPC_DIAG_TCAM_VALUE_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TCAM_VALUE *info
  );

void
  SOC_PPC_DIAG_LEM_VALUE_print(
    SOC_SAND_IN  SOC_PPC_DIAG_LEM_VALUE *info,
    SOC_SAND_IN  SOC_PPC_DIAG_LEM_LKUP_TYPE type
  );

void
  SOC_PPC_DIAG_PARSING_L2_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_PARSING_L2_INFO *info
  );

void
  SOC_PPC_DIAG_PARSING_MPLS_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_PARSING_MPLS_INFO *info
  );

void
  SOC_PPC_DIAG_PARSING_IP_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_PARSING_IP_INFO *info
  );

void
  SOC_PPC_DIAG_PARSING_HEADER_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_PARSING_HEADER_INFO *info
  );

void
  SOC_PPC_DIAG_PARSING_INFO_print(
    int unit,
    SOC_SAND_IN  SOC_PPC_DIAG_PARSING_INFO *info
  );

void
  SOC_PPC_DIAG_TERM_MPLS_LABEL_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TERM_MPLS_LABEL_INFO *info
  );

void
  SOC_PPC_DIAG_TERM_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TERM_INFO *info
  );

void
  SOC_PPC_DIAG_IPV4_VPN_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_IPV4_VPN_KEY *info
  );

void
  SOC_PPC_DIAG_IPV6_VPN_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_IPV6_VPN_KEY *info
  );

#if defined(INCLUDE_KBP)
void
  SOC_PPC_DIAG_IPV4_UNICAST_RPF_print(
    SOC_SAND_IN SOC_PPC_DIAG_IPV4_UNICAST_RPF *info
  );

void
  SOC_PPC_DIAG_IPV4_MULTICAST_print(
    SOC_SAND_IN SOC_PPC_DIAG_IPV4_MULTICAST *info
  );

void
  SOC_PPC_DIAG_IPV6_UNICAST_RPF_print(
    SOC_SAND_IN SOC_PPC_DIAG_IPV6_UNICAST_RPF *info
  );

void
  SOC_PPC_DIAG_IPV6_MULTICAST_print(
    SOC_SAND_IN SOC_PPC_DIAG_IPV6_MULTICAST *info
  );

void
  SOC_PPC_DIAG_MPLS_print(
    SOC_SAND_IN SOC_PPC_DIAG_MPLS *info
  );

void
  SOC_PPC_DIAG_TRILL_UNICAST_print(
    SOC_SAND_IN SOC_PPC_DIAG_TRILL_UNICAST *info
  );

void
  SOC_PPC_DIAG_TRILL_MULTICAST_print(
    SOC_SAND_IN SOC_PPC_DIAG_TRILL_MULTICAST *info
  );

void
  SOC_PPC_DIAG_IP_REPLY_RECORD_print(
    SOC_SAND_IN SOC_PPC_DIAG_IP_REPLY_RECORD *info
  );

void
  SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD_print(
    SOC_SAND_IN SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD *info
  );
#endif 

void
  SOC_PPC_DIAG_FRWRD_LKUP_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_FRWRD_LKUP_KEY *info,
    SOC_SAND_IN  SOC_PPC_DIAG_FWD_LKUP_TYPE frwrd_type,
    SOC_SAND_IN  uint8                      is_kbp
  );

void
  SOC_PPC_DIAG_FRWRD_LKUP_VALUE_print(
    SOC_SAND_IN  SOC_PPC_DIAG_FRWRD_LKUP_VALUE *info,
    SOC_SAND_IN  SOC_PPC_DIAG_FWD_LKUP_TYPE frwrd_type
  );

void
  SOC_PPC_DIAG_FRWRD_LKUP_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_FRWRD_LKUP_INFO *info
  );

void
  SOC_PPC_DIAG_LEARN_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_LEARN_INFO *info
  );

void
  SOC_PPC_DIAG_VLAN_EDIT_RES_print(
    SOC_SAND_IN  SOC_PPC_DIAG_VLAN_EDIT_RES *info
  );

void
  SOC_PPC_DIAG_FRWRD_DECISION_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_FRWRD_DECISION_INFO *info
  );

void
  SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO *info
  );

void
  SOC_PPC_DIAG_PKT_TM_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_PKT_TM_INFO *info
  );

void
  SOC_PPC_DIAG_ENCAP_INFO_print(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_DIAG_ENCAP_INFO *info
  );

void
  SOC_PPC_DIAG_EG_DROP_LOG_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_EG_DROP_LOG_INFO *info
  );

void
  SOC_PPC_DIAG_ETH_PACKET_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_ETH_PACKET_INFO *info
  );

void 
  SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_print(
     SOC_SAND_IN SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO *prm_vec_res
  );

void
  SOC_PPC_DIAG_TRILL_KEY_print(
    SOC_SAND_IN SOC_PPC_DIAG_TRILL_KEY *info
  );


void
 soc_ppc_diag_glem_outlif_print(
    soc_ppc_diag_glem_outlif_t *info
    );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

