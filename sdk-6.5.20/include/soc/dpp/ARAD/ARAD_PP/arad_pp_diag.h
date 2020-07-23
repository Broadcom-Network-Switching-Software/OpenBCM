
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_DIAG_INCLUDED__

#define __ARAD_PP_DIAG_INCLUDED__



#include <soc/dpp/ARAD/arad_api_general.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/PPD/ppd_api_metering.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <bcm/policer.h>

#include <soc/dpp/PPC/ppc_api_diag.h>


#if defined(INCLUDE_KBP)
#include <soc/dpp/ARAD/arad_sw_db_tcam_mgmt.h>
#endif 



#define BLOCK_NUM   4





#define ARAD_PP_DIAG_FLD_FILL(prm_fld, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb)  \
          (prm_fld)->base = (prm_addr_msb << 16) + prm_addr_lsb;  \
          (prm_fld)->msb = prm_fld_msb;  \
          (prm_fld)->lsb= prm_fld_lsb;

#define ARAD_PP_DIAG_FLD_READ(prm_fld, core_id, prm_blk, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb, prm_err_num)  \
  ARAD_PP_DIAG_FLD_FILL(prm_fld, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb);    \
  res = arad_pp_diag_dbg_val_get_unsafe(      \
          unit,      \
          core_id,   \
          prm_blk,      \
          prm_fld,      \
          regs_val      \
        );              \
  SOC_SAND_CHECK_FUNC_RESULT(res, prm_err_num, exit);


#define SOCDNX_DIAG_FLD_READ(prm_fld, core_id, prm_blk, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb)  \
  ARAD_PP_DIAG_FLD_FILL(prm_fld, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb);    \
  rv = arad_pp_diag_dbg_val_get_unsafe(      \
          unit,      \
          core_id,   \
          prm_blk,      \
          prm_fld,      \
          regs_val      \
        );              \
  SOCDNX_SAND_IF_ERR_EXIT(rv);



#define ARAD_PP_DIAG_FLD_GET(blk, core_id, addr_msb, addr_lsb, fld_msb, fld_lsb, fld_val)  \
    do { \
        ARAD_PP_DIAG_REG_FIELD fld; \
        uint32 tmp_buff[ARAD_PP_DIAG_DBG_VAL_LEN]; \
        fld.base = (addr_msb << 16) | (addr_lsb & 0xffff); \
        fld.msb = fld_msb; \
        fld.lsb = fld_lsb; \
        res = arad_pp_diag_dbg_val_get_unsafe(unit, core_id, blk, &fld, tmp_buff);  \
        SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit); \
        fld_val = tmp_buff[0]; \
    } while (0); 



#define SOCDNX_DIAG_FLD_GET(blk, core_id, addr_msb, addr_lsb, fld_msb, fld_lsb, fld_val)  \
    do { \
        ARAD_PP_DIAG_REG_FIELD fld; \
        uint32 tmp_buff[ARAD_PP_DIAG_DBG_VAL_LEN]; \
        fld.base = (addr_msb << 16) | (addr_lsb & 0xffff); \
        fld.msb = fld_msb; \
        fld.lsb = fld_lsb; \
        rv = arad_pp_diag_dbg_val_get_unsafe(unit, core_id, blk, &fld, tmp_buff);  \
        SOCDNX_SAND_IF_ERR_EXIT(rv); \
        fld_val = tmp_buff[0]; \
    } while (0); 




typedef struct {
    uint8 block_msb_max[BLOCK_NUM];
    uint8 block_lsb_max[BLOCK_NUM];
    uint8 block_id[BLOCK_NUM];
} debug_signals_t;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 base;
  
  uint32 msb;
  
  uint32 lsb;

} ARAD_PP_DIAG_REG_FIELD;

typedef enum
{
  
  ARAD_PP_DIAG_SAMPLE_ENABLE_SET = ARAD_PP_PROC_DESC_BASE_DIAG_FIRST,
  ARAD_PP_DIAG_SAMPLE_ENABLE_SET_UNSAFE,
  ARAD_PP_DIAG_SAMPLE_ENABLE_SET_VERIFY,
  ARAD_PP_DIAG_SAMPLE_ENABLE_GET,
  ARAD_PP_DIAG_SAMPLE_ENABLE_GET_VERIFY,
  ARAD_PP_DIAG_SAMPLE_ENABLE_GET_UNSAFE,
  SOC_PPC_DIAG_MODE_INFO_SET,
  SOC_PPC_DIAG_MODE_INFO_SET_UNSAFE,
  SOC_PPC_DIAG_MODE_INFO_SET_VERIFY,
  SOC_PPC_DIAG_MODE_INFO_GET,
  SOC_PPC_DIAG_MODE_INFO_GET_VERIFY,
  SOC_PPC_DIAG_MODE_INFO_GET_UNSAFE,
  SOC_PPC_DIAG_PKT_TRACE_CLEAR,
  SOC_PPC_DIAG_PKT_TRACE_CLEAR_UNSAFE,
  SOC_PPC_DIAG_PKT_TRACE_CLEAR_VERIFY,
  SOC_PPC_DIAG_RECEIVED_PACKET_INFO_GET,
  SOC_PPC_DIAG_RECEIVED_PACKET_INFO_GET_UNSAFE,
  SOC_PPC_DIAG_RECEIVED_PACKET_INFO_GET_VERIFY,
  SOC_PPC_DIAG_PARSING_INFO_GET,
  SOC_PPC_DIAG_PARSING_INFO_GET_UNSAFE,
  SOC_PPC_DIAG_PARSING_INFO_GET_VERIFY,
  ARAD_PP_DIAG_TERMINATION_INFO_GET,
  ARAD_PP_DIAG_TERMINATION_INFO_GET_UNSAFE,
  ARAD_PP_DIAG_TERMINATION_INFO_GET_VERIFY,
  SOC_PPC_DIAG_FRWRD_LKUP_INFO_GET,
  SOC_PPC_DIAG_FRWRD_LKUP_INFO_GET_UNSAFE,
  SOC_PPC_DIAG_FRWRD_LKUP_INFO_GET_VERIFY,
  ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET,
  ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET_UNSAFE,
  ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET_VERIFY,
  SOC_PPC_DIAG_TRAPS_INFO_GET,
  SOC_PPC_DIAG_TRAPS_INFO_GET_UNSAFE,
  SOC_PPC_DIAG_TRAPS_INFO_GET_VERIFY,
  ARAD_PP_DIAG_TRAPPED_PACKET_INFO_GET,
  ARAD_PP_DIAG_TRAPPED_PACKET_INFO_GET_PRINT,
  ARAD_PP_DIAG_TRAPPED_PACKET_INFO_GET_UNSAFE,
  ARAD_PP_DIAG_TRAPPED_PACKET_INFO_GET_VERIFY,
  ARAD_PP_DIAG_TRAPS_ALL_TO_CPU,
  ARAD_PP_DIAG_TRAPS_ALL_TO_CPU_UNSAFE,
  ARAD_PP_DIAG_TRAPS_ALL_TO_CPU_VERIFY,
  ARAD_PP_DIAG_TRAPS_STAT_RESTORE,
  ARAD_PP_DIAG_TRAPS_STAT_RESTORE_UNSAFE,
  ARAD_PP_DIAG_TRAPS_STAT_RESTORE_VERIFY,
  ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET,
  ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET_UNSAFE,
  ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET_VERIFY,
  ARAD_PP_DIAG_LEARNING_INFO_GET,
  ARAD_PP_DIAG_LEARNING_INFO_GET_UNSAFE,
  ARAD_PP_DIAG_LEARNING_INFO_GET_VERIFY,
  ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET,
  ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET_UNSAFE,
  ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET_VERIFY,
  ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET,
  ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_UNSAFE,
  ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_VERIFY,
  SOC_PPC_DIAG_ENCAP_INFO_GET,
  SOC_PPC_DIAG_ENCAP_INFO_GET_UNSAFE,
  SOC_PPC_DIAG_ENCAP_INFO_GET_VERIFY,
  ARAD_PP_DIAG_EG_DROP_LOG_GET,
  ARAD_PP_DIAG_EG_DROP_LOG_GET_UNSAFE,
  ARAD_PP_DIAG_EG_DROP_LOG_GET_VERIFY,
  ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET,
  ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET_UNSAFE,
  ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET_VERIFY,
  ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET,
  ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET_UNSAFE,
  ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET_VERIFY,
  ARAD_PP_DIAG_DB_TCAM_LKUP_INFO_GET,
  ARAD_PP_DIAG_DB_TCAM_LKUP_INFO_GET_UNSAFE,
  ARAD_PP_DIAG_DB_TCAM_LKUP_INFO_GET_VERIFY,
  ARAD_PP_DIAG_PKT_SEND,
  ARAD_PP_DIAG_PKT_SEND_UNSAFE,
  ARAD_PP_DIAG_PKT_SEND_VERIFY,
  ARAD_PP_DIAG_GET_PROCS_PTR,
  ARAD_PP_DIAG_GET_ERRS_PTR,
  ARAD_PP_DIAG_EGRESS_VLAN_EDIT_INFO_GET, 
  ARAD_PP_DIAG_EGRESS_VLAN_EDIT_INFO_GET_VERIFY, 
  ARAD_PP_DIAG_EGRESS_VLAN_EDIT_INFO_GET_UNSAFE, 
  

  ARAD_PP_DIAG_DBG_VAL_GET_UNSAFE,
  ARAD_PP_DIAG_LIF_DB_ID_TO_DB_TYPE_MAP_GET,
  ARAD_PP_DIAG_TRAPS_RANGE_INFO_GET_UNSAFE,
  ARAD_PP_DIAG_PKT_ETH_HEADER_BUILD,

  SOC_PPC_DIAG_VSI_INFO_GET,
  SOC_PPC_DIAG_VSI_INFO_GET_VERIFY,
  SOC_PPC_DIAG_VSI_INFO_GET_UNSAFE,

  ARAD_PP_DIAG_GET_EPNI_RAW_SIGNAL,

	

  ARAD_PP_DIAG_PROCEDURE_DESC_LAST
} ARAD_PP_DIAG_PROCEDURE_DESC;

typedef enum
{
  
  SOC_PPC_DIAG_PKT_TRACE_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_DIAG_FIRST,
  ARAD_PP_DIAG_MAX_SIZE_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_VALID_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_BUFF_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_BUFF_LEN_OUT_OF_RANGE_ERR,
  SOC_PPC_DIAG_FLAVOR_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_LKUP_NUM_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_BANK_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_LKUP_USAGE_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_IN_TM_PORT_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_PARSER_PROGRAM_POINTER_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_PACKET_QUAL_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_CODE_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_CPU_DEST_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_IP_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_BASE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_OPCODE_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_LENGTH_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_FEC_PTR_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_ENCAP_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_VLAN_TAG_FORMAT_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_NEXT_PRTCL_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_HDR_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_HDR_OFFSET_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_RANGE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_TERM_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_LABEL_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_RANGE_BIT_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_FRWRD_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_VRF_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_TRILL_UC_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_FRWRD_HDR_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_VALID_FIELDS_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_METER1_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_METER2_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_DP_METER_CMD_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_COUNTER1_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_COUNTER2_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_CUD_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_EEP_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_DROP_LOG_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_ETHER_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_TOTAL_SIZE_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_NOF_PACKETS_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_INVALID_TM_PORT_ERR,
  


  ARAD_PP_DIAG_RESTORE_NOT_SAVED_ERR,
  ARAD_PP_DIAG_LIF_DB_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_DIAG_INVALID_LAST_PACKET_ERR,
  
  ARAD_PP_DIAG_ERR_LAST
} ARAD_PP_DIAG_ERR;

typedef struct
{
  uint32 trap_dest[SOC_PPC_NOF_TRAP_CODES * 4];

  uint8 already_saved;

  SOC_PPC_DIAG_MODE_INFO mode_info;
} ARAD_PP_SW_DB_DIAG;










#define ARAD_DIAG_BLK_NOF_BITS (256)

#define ARAD_DIAG_DBG_VAL_LEN  (ARAD_DIAG_BLK_NOF_BITS/32)
#define ARAD_PP_DIAG_DBG_VAL_LEN        ARAD_DIAG_DBG_VAL_LEN

uint32
  arad_pp_diag_is_valid(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32          func_name,
    SOC_SAND_OUT uint32          *ret_val
);

uint32
  arad_pp_diag_init_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core_id
);

uint32
  arad_pp_diag_dbg_val_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  int              core_id,
    SOC_SAND_IN  uint32               blk,
    SOC_SAND_IN  ARAD_PP_DIAG_REG_FIELD   *fld,
    SOC_SAND_OUT uint32               val[ARAD_PP_DIAG_DBG_VAL_LEN]
);



uint32
  arad_pp_diag_get_raw_signal(
      int core_id,
      ARAD_MODULE_ID prm_blk,
      int prm_addr_msb,
      int prm_addr_lsb,
      int prm_fld_msb,
      int prm_fld_lsb,
      uint32 *regs_val
);


uint32
  arad_pp_diag_sample_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                 enable
  );

uint32
  arad_pp_diag_sample_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                 enable
  );

uint32
  arad_pp_diag_sample_enable_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_diag_sample_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                 *enable
  );


uint32
  arad_pp_diag_mode_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_DIAG_MODE_INFO                      *mode_info
  );

uint32
  arad_pp_diag_mode_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_DIAG_MODE_INFO                      *mode_info
  );

uint32
  arad_pp_diag_mode_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_diag_mode_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_DIAG_MODE_INFO                      *mode_info
  );

uint32
  arad_pp_diag_vsi_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_diag_vsi_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_VSI_INFO                      *vsi_info
  );



uint32
  arad_pp_diag_pkt_trace_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  uint32                                  pkt_trace
  );

uint32
  arad_pp_diag_pkt_trace_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pkt_trace
  );


uint32
  arad_pp_diag_received_packet_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_RECEIVED_PACKET_INFO           *rcvd_pkt_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val
  );

uint32
  arad_pp_diag_received_packet_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_diag_parsing_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_INFO              *pars_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val

  );

uint32
  arad_pp_diag_parsing_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_diag_termination_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_TERM_INFO              *term_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                 *ret_val

  );

uint32
  arad_pp_diag_termination_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

#if defined(INCLUDE_KBP)
uint32
  arad_pp_diag_get_request(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  int                     core_id,
    SOC_SAND_IN  uint32                     nof_bytes,
    SOC_SAND_OUT uint32                     *buffer
  );

uint32
    arad_pp_diag_get_frwrd_type_size(
        SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID type,
        SOC_SAND_OUT uint32                   *size
    );

ARAD_KBP_FRWRD_IP_TBL_ID
    arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(
        SOC_SAND_IN SOC_PPC_DIAG_FWD_LKUP_TYPE type
    );

void
  SOC_PPC_DIAG_IPV4_UNICAST_RPF_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_IPV4_UNICAST_RPF *info,
    SOC_SAND_IN uint32 offset
  );

void
  SOC_PPC_DIAG_IPV4_MULTICAST_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_IPV4_MULTICAST *info,
    SOC_SAND_IN uint32 offset
  );

void
  SOC_PPC_DIAG_IPV6_UNICAST_RPF_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_IPV6_UNICAST_RPF *info,
    SOC_SAND_IN uint32 offset
  );

void
  SOC_PPC_DIAG_IPV6_MULTICAST_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_IPV6_MULTICAST *info,
    SOC_SAND_IN uint32 offset
  );

void
  SOC_PPC_DIAG_MPLS_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_MPLS *info,
    SOC_SAND_IN uint32 offset
  );

void
  SOC_PPC_DIAG_TRILL_UNICAST_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_TRILL_UNICAST *info,
    SOC_SAND_IN uint32 offset
  );

void
  SOC_PPC_DIAG_TRILL_MULTICAST_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_TRILL_MULTICAST *info,
    SOC_SAND_IN uint32 offset
  );

#endif 

void
  ARAD_PP_DIAG_MTR_INFO_print(
	SOC_SAND_IN			int unit,
	SOC_SAND_IN			bcm_policer_t policer_id, 
	SOC_SAND_IN			bcm_policer_config_t *policer_cfg, 
	SOC_SAND_IN			int cbl, 
	SOC_SAND_IN			int ebl);

void
  ARAD_PP_DIAG_ETH_POLICER_INFO_print(
	SOC_SAND_IN		int 						              unit,
	SOC_SAND_IN		bcm_port_t 					          port,
	SOC_SAND_IN		SOC_PPC_MTR_BW_PROFILE_INFO   *policer_cfg, 
	SOC_SAND_IN		int 						              *bucket_lvl,
  SOC_SAND_IN		uint32 						            agg_policer_valid,
  SOC_SAND_IN		uint32 						            agg_policer_id);

void
  ARAD_PP_DIAG_AGGREGATE_ETH_POLICER_INFO_print(
	SOC_SAND_IN		int 						              unit,
	SOC_SAND_IN		bcm_policer_t 	              policer_id,
  SOC_SAND_IN		int 	                        nom_of_policers,
  SOC_SAND_IN		SOC_PPC_MTR_BW_PROFILE_INFO   *policer_cfg, 
  SOC_SAND_IN		int 						              *bucket_lvl);


uint32
  arad_pp_diag_frwrd_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_LKUP_INFO           *frwrd_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val
  );

uint32
  arad_pp_diag_frwrd_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_diag_frwrd_lpm_lkup_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_DIAG_IPV4_VPN_KEY              *lpm_key,
    SOC_SAND_OUT uint32                             *fec_ptr,
    SOC_SAND_OUT uint8                            *found,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT              *ret_val
  );

uint32
  arad_pp_diag_frwrd_lpm_lkup_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_DIAG_IPV4_VPN_KEY              *lpm_key
  );


uint32
  arad_pp_diag_traps_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_TRAPS_INFO                *traps_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val

  );

uint32
  arad_pp_diag_traps_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_diag_frwrd_decision_trace_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO      *frwrd_trace_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val

  );

uint32
  arad_pp_diag_frwrd_decision_trace_get_verify(
    SOC_SAND_IN  int                                 unit
  );



uint32
  arad_pp_diag_learning_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_LEARN_INFO                *learn_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val

  );

uint32
  arad_pp_diag_learning_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_diag_ing_vlan_edit_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_VLAN_EDIT_RES             *vec_res,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val

  );

uint32
  arad_pp_diag_ing_vlan_edit_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_diag_pkt_associated_tm_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_PKT_TM_INFO               *pkt_tm_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val
  );

uint32
  arad_pp_diag_pkt_associated_tm_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_diag_encap_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_ENCAP_INFO             *encap_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                 *ret_val
  );

uint32
  arad_pp_diag_encap_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_diag_out_rif_get_unsafe(
    SOC_SAND_IN     int     unit,
    SOC_SAND_IN     int     core_id,
    SOC_SAND_OUT    uint8   *rif_is_valid,
    SOC_SAND_OUT    uint32  *out_rif
  );


uint32
  arad_pp_diag_eg_drop_log_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_DIAG_EG_DROP_LOG_INFO               *eg_drop_log
  );

uint32
  arad_pp_diag_eg_drop_log_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_diag_db_lif_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO               *db_info,
    SOC_SAND_OUT SOC_PPC_DIAG_LIF_LKUP_INFO             *lkup_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val

  );

uint32
  arad_pp_diag_db_lif_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO                    *db_info
  );


uint32
  arad_pp_diag_db_lem_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO                    *db_info,
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_LKUP_TYPE                  *type,
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_KEY                        *key,
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_VALUE                      *val,
    SOC_SAND_OUT uint8                                 *valid
  );

uint32
  arad_pp_diag_db_lem_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO                    *db_info
  );




uint32
  arad_pp_diag_egress_vlan_edit_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO      *prm_vec_res,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val
  );

uint32
  arad_pp_diag_egress_vlan_edit_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_diag_cos_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id
  );

uint32
  SOC_PPC_DIAG_MODE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_DIAG_MODE_INFO *info
  );

uint32
  SOC_PPC_DIAG_DB_USE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO *info
  );
uint32
  SOC_PPC_DIAG_IPV4_VPN_KEY_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_DIAG_IPV4_VPN_KEY *info
  );

uint32
  SOC_PPC_DIAG_IPV6_VPN_KEY_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_DIAG_IPV6_VPN_KEY *info
  );





uint32
  arad_pp_diag_prge_first_instr_get(int unit, int core_id, uint32 *first_instruction);





int
arad_pp_diag_ftmh_cfg_get(int unit, int * p_cfg_ftmh_lb_key_ext_en, int * p_cfg_ftmh_stacking_ext_enable);





uint32
arad_pp_diag_epni_prge_program_tbl_get(int unit, uint32 offset, ARAD_PP_EPNI_PRGE_PROGRAM_TBL_DATA *tbl_data);



int
arad_pp_diag_mem_read(int unit,
                        soc_mem_t mem,
                        int copyno,
                        int in_line,
                        void* val);

uint32
  arad_pp_diag_fwd_decision_in_buffer_parse(
    SOC_SAND_IN  int                                  unit,    
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE signal_type,
    SOC_SAND_IN  uint32                                  dest_buffer,
    SOC_SAND_IN  uint32                                  asd_buffer,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO             *fwd_decision
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif
