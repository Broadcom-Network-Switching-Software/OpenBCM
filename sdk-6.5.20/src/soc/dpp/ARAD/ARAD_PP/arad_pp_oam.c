/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_OAM
#include <shared/bsl.h>



#include <shared/swstate/access/sw_state_access.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_cnt.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_tcam_mgmt.h>
#include <soc/dpp/ARAD/arad_header_parsing_utils.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/PPC/ppc_api_oam.h> 
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_mpls_term.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif 


#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oamp_pe.h>


#include <soc/dpp/PPD/ppd_api_diag.h>
#ifdef PLISIM
  #include <sim/dpp/ChipSim/chip_sim_em.h>
#else
  #include <soc/dpp/ARAD/arad_sim_em.h>
#endif
#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_oam.h>
#include <soc/dpp/JER/JER_PP/jer_pp_lif.h>

#include <bcm_int/common/debug.h>






#define ARAD_PP_OAM_PROFILE_DEFAULT              0
#define ARAD_PP_OAM_PROFILE_PASSIVE              3


#define _ARAD_PP_OAM_NON_ACC_PROFILES_NUM_ARAD 4




#define _ARAD_PP_OAM_MAX_MD_LEVEL                7

#define _ARAD_PP_OAM_RMEP_EMC_SHIFT_BFD (SOC_IS_JERICHO(unit)?13: 16)
#define _ARAD_PP_OAM_RMEP_EMC_SHIFT_ETH 13

#define _ARAD_PP_OAM_TC_NOF_BITS 3
#define _ARAD_PP_OAM_TC_MAX ((1<<_ARAD_PP_OAM_TC_NOF_BITS)-1)
#define _ARAD_PP_OAM_DP_NOF_BITS 2
#define _ARAD_PP_OAM_DP_MAX ((1<<_ARAD_PP_OAM_DP_NOF_BITS)-1)
#define _ARAD_PP_OAM_TX_RATE_INDEX_MAX 7
#define _ARAD_PP_OAM_REQ_INTERVAL_POINTER_MAX 7
#define _ARAD_PP_OAM_DIP_INDEX_MAX 15

#define _ARAD_PP_OAM_BFD_VERS_MAX 7
#define _ARAD_PP_OAM_BFD_DIAG_MAX 63
#define _ARAD_PP_OAM_BFD_STA_MAX 3
#define _ARAD_PP_OAM_BFD_YOUR_DISC_MAX   SOC_PPC_BFD_DISCRIMINATOR_TO_LIF_START_MASK

#define _ARAD_PP_OAM_EXP_MAX 7
#define _ARAD_PP_OAM_TTL_MAX 255
#define _ARAD_PP_OAM_TOS_MAX 255

#define _ARAD_PP_OAM_PTCH_OPAQUE_VALUE 7


#define _ARAD_PP_OAM_MIN_PKT_SIZE_TXM 44



#define _ARAD_PP_OAM_NUMBER_OF_OAMP_ERROR_TYPES (15 + SOC_IS_JERICHO(unit) + (2*SOC_IS_QAX(unit)))
#define _ARAD_PP_OAM_ITMH_DEST_INFO_WIDTH 20

#define _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO 20


#define _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA 80


#define ARAD_OAM_DMA_CHANNEL_USED_EVENT SOC_MEM_FIFO_DMA_CHANNEL_3
#define ARAD_OAM_DMA_CHANNEL_USED_STAT_EVENT SOC_MEM_FIFO_DMA_CHANNEL_2
#define DMA_SRC_OAM_REPORT_FIFO 4
#define DMA_SRC_OAM_EVENT_FIFO 5

#define SMALLEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY 0x20

#define SOC_DPP_INVALID_GLOBAL_LIF(unit)    (SOC_DPP_DEFS_GET(unit, nof_global_lifs) - 1)

#define ARAD_PP_OAM_CLASSIFIER_OAM_ID_MASK ((1 << SOC_DPP_IMP_DEFS_GET(unit, oam_id_nof_bit)) - 1)

#define ARAD_PP_OAM_BFD_IPv4_FLEX_IPTOS_FEATURE_SET(unit) ((SOC_DPP_CONFIG(unit))->pp.bfd_ipv4_flex_iptos != 0)

typedef enum {
    _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD = 0,
    _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD_100,
    _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD_10000
} _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS;

typedef enum {
    _ARAD_PP_OAM_ACTION_TRAP_ERROR_LEVEL = 0,
    _ARAD_PP_OAM_ACTION_TRAP_ERROR_PASSIVE,
    _ARAD_PP_OAM_ACTION_RECYCLE,
    _ARAD_PP_OAM_ACTION_TRAP_CPU,
    _ARAD_PP_OAM_ACTION_TRAP_OAMP,
    _ARAD_PP_OAM_ACTION_FRWRD,
    _ARAD_PP_OAM_ACTION_SNOOP
} _ARAD_PP_OAM_ACTION;


#define _ARAD_PP_OAM_FLP_TCAM_TRAP_TYPE_OAM 0
#define _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE ARAD_TCAM_BANK_KEY_SIZE_IN_BITS_320


#define _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_Y1731 0
#define _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_BFD_RAW 1
#define _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_RFC6374 4
#define _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_OAM_UNKNOWN 3
#define _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_BFD_IPV4 2

#define _ARAD_PP_OAM_FWD_CODE_MPLS 0x5
#define _ARAD_PP_OAM_FWD_CODE_CPU 0x7
#define _ARAD_PP_OAM_FWD_CODE_ETHERNET 0

#define _ARAD_PP_OAM_FWD_CODE_IPV4_UC 1
#define _ARAD_PP_OAM_IDENTIFICATION_TCAM_MIN_BANKS 0
#define _ARAD_PP_OAM_IDENTIFICATION_PREFIX_SIZE 0

#define _ARAD_PP_OAM_Y1731_DEFAULT_MDLEVEL 7


#define _BCM_OAM_LIF_PROFILE_MAX 3


#define _ARAD_PP_OAM_FTMH_BASE_SIZE 9 
#define _ARAD_PP_OAM_FTMH_STACKING_EXTENSION_SIZE 2 
#define _ARAD_PP_OAM_FTMH_LB_EXTENSION_SIZE 1 
#define _ARAD_PP_OAM_FTMH_DSP_EXTENSION_SIZE 2 
#define _ARAD_PP_OAM_OAM_TS_SIZE 6 
#define _ARAD_PP_OAM_PPH_BASE_SIZE 7 
#define _ARAD_PP_OAM_FHEI_3B_SIZE 3 
#define _ARAD_PP_OAM_PACKET_OFFSET_SET(header_size, offset_byte, offset_word, offset_bit) \
    offset_byte -= header_size; \
    offset_word = offset_byte / 4; \
    offset_bit = (offset_byte % 4) * 8; 
    


#define _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_CONTROL                    1
#define _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_FREQ_CONTROL               2
#define _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_FRAC_SEC_LOWER             3
#define _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_FRAC_SEC_UPPER             4
#define _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_SEC                        5
#define _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_CONTROL               6
#define _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_FREQ_CONTROL          7
#define _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_FRAC_SEC_LOWER        8
#define _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_FRAC_SEC_UPPER        9
#define _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_SEC                  10






#define _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(internal_opcode,external_opcode)                                                                \
    (((internal_opcode==SOC_PPC_OAM_OPCODE_MAP_LMM) ||                                                          \
      (internal_opcode==SOC_PPC_OAM_OPCODE_MAP_1DM) ||                                                          \
      (internal_opcode==SOC_PPC_OAM_OPCODE_MAP_DMM)) ? 4 : (internal_opcode==SOC_PPC_OAM_OPCODE_MAP_LMR) ||     \
      (internal_opcode==SOC_PPC_OAM_OPCODE_MAP_SLM_SLR) ? ((external_opcode==SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM) ? 12 : (external_opcode==SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLR) ? 16:12) : \
      (internal_opcode==SOC_PPC_OAM_OPCODE_MAP_DMR) ? 20 : (internal_opcode==SOC_PPC_OAM_OPCODE_MAP_CCM) ?      \
	   58 :  0)

#define _ARAD_PP_OAM_MPLS_TCAM_SUPPORTED_OPCODE(opcode) \
  ((opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM) || \
   (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMR) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMM) || \
   (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMR) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBM) || \
   (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBR) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LINEAR_APS) || \
   (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_1DM))

#define _ARAD_PP_OAM_MIM_TCAM_SUPPORTED_OPCODE(opcode) \
  ((opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM) || \
   (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMR) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMM) || \
   (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMR))


#define _ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT_INDEX(table_index, dst_mac_address) \
  (table_index+(dst_mac_address->address[0]<<8))


#define IS_INGRESS_WHEN_ACTIVE(is_upmep)  (is_upmep == 0)

#define _ARAD_PP_OAM_IS_SHORT_FORMAT(icc_ndx)          (icc_ndx == 15)

#define _ARAD_PP_OAM_IS_OEM1_ENTRY(oam_id)          (oam_id & 0x10000)

#define _ARAD_PP_OAM_RMEP_DB_ENTRY_KEY_GET_FROM_MEP_INDEX_AND_RMEP_ID(local_mep_db_index, rmep_id, shift) \
    (((local_mep_db_index) << shift) + (rmep_id))




#define _ARAD_PP_OAM_OEM1_KEY_STRUCT_TO_KEY(key_struct, key) \
    key = (key_struct.ingress) + (key_struct.oam_lif << 1) + SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit) * (key_struct.your_discr <<19)


#define OAM_PROCESS_ACTION_INGRESS_STAMP_1588 1
#define OAM_PROCESS_ACTION_EGRESS_STAMP_1588 2
#define OAM_PROCESS_ACTION_INGRESS_STAMP_NTP 3
#define OAM_PROCESS_ACTION_EGRESS_STAMP_NTP 4
#define OAM_PROCESS_ACTION_STAMP_COUNTER 5
#define OAM_PROCESS_ACTION_READ_COUNTER_AND_STAMP 6


#define OAM_PROCESS_ADD(subtype, up_1_down_0, action_type, reg_to_write_on ) \
 do {\
    int index = subtype <<2 | up_1_down_0 << 1;\
    uint32 what_to_write = action_type;\
    SHR_BITCOPY_RANGE(reg_to_write_on, index*3, &what_to_write, 0, 3 );\
} while (0);


#define _ARAD_PP_OAM_OEM2_KEY_STRUCT_TO_KEY(key_struct, key) \
     key = (key_struct.your_disc << (20 + 2 * SOC_IS_JERICHO(unit))) + (key_struct.oam_lif << 4) + (key_struct.mdl << 1) + (key_struct.ingress)                     



#define _ARAD_PP_OAM_OAM2_KEY_STRUCT_TO_KEY(key_struct, key)                                                         \
    key = (key_struct.inject << 11) + (key_struct.my_cfm_mac << 10) + (key_struct.is_bfd << 9) + (key_struct.opcode << 5) + \
          (key_struct.ingress << 4) + (key_struct.mp_profile)


#define _ARAD_PP_OAM_MAC_ADD_MSB_GET(mac_add_U32, msb_U64)       \
  do {                                                        \
    COMPILER_64_SET(msb_U64, mac_add_U32[1], mac_add_U32[0]); \
    COMPILER_64_SHR(msb_U64, 8);                              \
  } while (0)

#define _ARAD_PP_OAM_MAC_ADD_MSB_SET(msb_U64, mac_add_U32)                           \
  do {                                                                                \
    uint64 msb_U64_temp;                                                             \
    COMPILER_64_SET(msb_U64_temp, COMPILER_64_HI(msb_U64), COMPILER_64_LO(msb_U64)); \
    COMPILER_64_SHL(msb_U64_temp, 8);                                                 \
    mac_add_U32[0] = COMPILER_64_LO(msb_U64_temp);                                     \
    mac_add_U32[1] = COMPILER_64_HI(msb_U64_temp);                                     \
  } while (0)

#define _ARAD_PP_OAM_MAC_ADD_LSB_SET(lsb, mac_add_U32) \
  do { \
    mac_add_U32[0] &= 0xffffff00; \
    mac_add_U32[0] |= (lsb & 0xff);  \
  } while (0)


#define _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(mep_type)  \
  ((mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) ||       \
   (mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) ||       \
   (mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS && SOC_IS_JERICHO(unit)) )

#define _ARAD_PP_OAM_MEP_TYPE_IS_RFC6374(mep_db_mep_type) \
            ((mep_db_mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE) || (mep_db_mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP) \
             || (mep_db_mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION))

#define _ARAD_PP_OAM_IS_BFD_O_MPLS_SPECIAL_TTL(mep_type)         \
  ((mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS) &&              \
   (SOC_DPP_CONFIG(unit)->pp.bfd_mpls_special_ttl_support == 1))

#define _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(data, addr)        \
  do {                                                                                                   \
      uint32 tod_reg = 0;                                                                                \
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 9999, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_TOD_COMMAND_WR_DATAr, REG_PORT_ANY, 0, TOD_COMMAND_WR_DATAf,  data));                    \
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 9999, exit, READ_ECI_TOD_COMMANDr(unit, &tod_reg));        \
      soc_reg_field_set(unit, ECI_TOD_COMMANDr, &tod_reg, TOD_COMMAND_TRIGGERf, 1);                \
      soc_reg_field_set(unit, ECI_TOD_COMMANDr, &tod_reg, TOD_COMMAND_OP_CODEf, 1);                \
      soc_reg_field_set(unit, ECI_TOD_COMMANDr, &tod_reg, TOD_COMMAND_ADDRESSf, addr);             \
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 9999, exit, WRITE_ECI_TOD_COMMANDr(unit, tod_reg));        \
  } while (0)


#define ARAD_PP_OAM_NUM_CLOCKS_IN_MEP_SCAN SOC_IS_JERICHO(unit) ? (167*arad_chip_kilo_ticks_per_sec_get(unit)/100) : (333*arad_chip_kilo_ticks_per_sec_get(unit)/100)

#define ARAD_PP_OAM_NUM_CLOCKS_MIN_NEEDED (16*1024*40)

 
#define _BCM_DPP_OAM_OAMP_MEP_DB_BANKS_NOF(_unit) (SOC_IS_QAX(_unit) ? _QAX_PP_OAM_OAMP_MEP_DB_NOF_BANKS : 1)

#define ARAD_PP_OAM_NUM_STAGES_ACTIVE_IN_MEP_SCAN 2
#define ARAD_PP_OAM_NUM_STAGES_IDLE_IN_MEP_SCAN   1
#define ARAD_PP_OAM_MAX_NUM_STAGES_IN_MEP_SCAN    15


#define ARAD_PP_OAMP_TXM_1_STAGE_CLOCKS 40


#define ARAD_PP_OAMP_MAX_EXT_DATA_ENTRY ((3*1024)/2)


#define SET_MIP_BITMEP_FROM_MEP_BITMEP(mip_bitmep, mep_bitmep)\
    do{\
    uint32 i=2,mep_entered =0,level ;\
    mip_bitmep = 0;\
    for (level=1; level<=7 ;i<<=1 , ++level) {\
        if (mep_bitmep & i) {\
            mip_bitmep |= level << (3 * mep_entered);\
            ++mep_entered;\
        }\
    }\
    if (mep_entered >2) {\
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Only 2 endpoints may be entered per direction per LIF.")));\
    }\
}while (0)

#define _NUMBER_OF_COUNTER_PROCESSORS(_u) (4)




#define ARAD_PP_OAM_OEM1_OEM2_FIND_ERRORS_AND_PRINT(fail_key_on_reg,inserted_key ,number_on_fail_reason_reg, oem_table) \
    if (fail_key_on_reg!=inserted_key) { \
        LOG_WARN(BSL_LS_SOC_OAM, \
                 (BSL_META_U(unit, \
                             "Warning: write to OEM%d failed with different key\n"), \
                  oem_table)); \
        goto exit; \
    }\
    if (number_on_fail_reason_reg & 1) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Change non-exist from self."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<1)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Change non-exist from other."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<2)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Change request over static."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<3)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Change fail non existent."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<4)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Learn over existing ."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<5)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Learn request over static."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<6)) {\
          LOG_DEBUG(BSL_LS_SOC_OAM, \
                            (BSL_META_U(unit, \
                                        "OEM%d warning: replacing existing entry, key: 0x%x\n"),oem_table, inserted_key)); \
    }\
    if (number_on_fail_reason_reg & (1<<7)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Reach max entry limit."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<8)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: delete unknown key."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<9)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Table coherency."),oem_table));\
    }\
    if (number_on_fail_reason_reg & (1<<10)) {\
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_NO_UNIT("OEM%d: Cam table full."),oem_table));\
    }



#define INSERT_INTO_REG(pkt_header_reg, bytes_before_cur_header, header_size , bit_no_in_header, field_size, value) \
 do {\
    uint32 field = value;\
    INSERT_INTO_REG_BUFFER(pkt_header_reg, bytes_before_cur_header, header_size , bit_no_in_header, field_size, &field);\
} while (0)



#define INSERT_INTO_REG_BUFFER(pkt_header_reg, bytes_before_cur_header, header_size, bit_no_in_header, field_size, value_buffer) \
 do {\
    uint32 _offset = 640 - (bytes_before_cur_header *8   + header_size *8) + bit_no_in_header ;\
    SHR_BITCOPY_RANGE(pkt_header_reg, _offset,(uint32*) value_buffer, 0,field_size );\
} while (0)



#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS

#define READ_OAMP_IPV4_SRC_ADDR_SELECTr_WITH_FAST_ACCESS(unit,regval) \
    ARAD_FAST_REGISER_GET(ARAD_FAST_REG_OAMP_IPV4_SRC_ADDR_SELECT,regval)

#define WRITE_OAMP_IPV4_SRC_ADDR_SELECTr_WITH_FAST_ACCESS(unit, regval) \
    ARAD_FAST_REGISER_SET(ARAD_FAST_REG_OAMP_IPV4_SRC_ADDR_SELECT, regval)

#define READ_OAMP_IPV4_TOS_TTL_SELECTr_WITH_FAST_ACCESS(unit,regval) \
    ARAD_FAST_REGISER_GET(ARAD_FAST_REG_OAMP_IPV4_TOS_TTL_SELECT,regval)

#define WRITE_OAMP_IPV4_TOS_TTL_SELECTr_WITH_FAST_ACCESS(unit, regval) \
    ARAD_FAST_REGISER_SET(ARAD_FAST_REG_OAMP_IPV4_TOS_TTL_SELECT, regval)

#define READ_OAMP_BFD_TX_RATEr_WITH_FAST_ACCESS(unit,regval) \
    ARAD_FAST_REGISER_GET(ARAD_FAST_REG_OAMP_BFD_TX_RATE,regval)

#define WRITE_OAMP_BFD_TX_RATEr_WITH_FAST_ACCESS(unit, regval) \
    ARAD_FAST_REGISER_SET(ARAD_FAST_REG_OAMP_BFD_TX_RATE, regval)

#define READ_OAMP_BFD_REQ_INTERVAL_POINTERr_WITH_FAST_ACCESS(unit,regval) \
    ARAD_FAST_REGISER_GET(ARAD_FAST_REG_OAMP_BFD_REQ_INTERVAL_POINTER,regval)

#define WRITE_OAMP_BFD_REQ_INTERVAL_POINTERr_WITH_FAST_ACCESS(unit, regval) \
    ARAD_FAST_REGISER_SET(ARAD_FAST_REG_OAMP_BFD_REQ_INTERVAL_POINTER, regval)

#define READ_OAMP_MPLS_PWE_PROFILEr_WITH_FAST_ACCESS(unit,regval) \
    ARAD_FAST_REGISER_GET(ARAD_FAST_REG_OAMP_MPLS_PWE_PROFILE,regval)

#define WRITE_OAMP_MPLS_PWE_PROFILEr_WITH_FAST_ACCESS(unit, regval) \
    ARAD_FAST_REGISER_SET(ARAD_FAST_REG_OAMP_MPLS_PWE_PROFILE, regval)

#define READ_OAMP_BFD_TX_IPV4_MULTI_HOPr_WITH_FAST_ACCESS(unit,regval) \
    ARAD_FAST_REGISER_GET(ARAD_FAST_REG_OAMP_BFD_TX_IPV4_MULTI_HOP,regval)

#define WRITE_OAMP_BFD_TX_IPV4_MULTI_HOPr_WITH_FAST_ACCESS(unit, regval) \
    ARAD_FAST_REGISER_SET(ARAD_FAST_REG_OAMP_BFD_TX_IPV4_MULTI_HOP, regval)

#define READ_OAMP_PR_2_FW_DTCr_WITH_FAST_ACCESS(unit,regval) \
    ARAD_FAST_REGISER_GET(ARAD_FAST_REG_OAMP_PR_2_FW_DTC,regval)

#define WRITE_OAMP_PR_2_FW_DTCr_WITH_FAST_ACCESS(unit, regval) \
    ARAD_FAST_REGISER_SET(ARAD_FAST_REG_OAMP_PR_2_FW_DTC, regval)


#else 

#define READ_OAMP_IPV4_SRC_ADDR_SELECTr_WITH_FAST_ACCESS(unit,regval) \
    READ_OAMP_IPV4_SRC_ADDR_SELECTr(unit, regval)

#define WRITE_OAMP_IPV4_SRC_ADDR_SELECTr_WITH_FAST_ACCESS(unit, regval) \
    WRITE_OAMP_IPV4_SRC_ADDR_SELECTr(unit,regval)

#define READ_OAMP_IPV4_TOS_TTL_SELECTr_WITH_FAST_ACCESS(unit,regval) \
    READ_OAMP_IPV4_TOS_TTL_SELECTr(unit, regval)

#define WRITE_OAMP_IPV4_TOS_TTL_SELECTr_WITH_FAST_ACCESS(unit, regval) \
    WRITE_OAMP_IPV4_TOS_TTL_SELECTr(unit,regval)

#define READ_OAMP_BFD_TX_RATEr_WITH_FAST_ACCESS(unit,regval) \
    READ_OAMP_BFD_TX_RATEr(unit, regval)

#define WRITE_OAMP_BFD_TX_RATEr_WITH_FAST_ACCESS(unit, regval) \
    WRITE_OAMP_BFD_TX_RATEr(unit,regval)

#define READ_OAMP_BFD_REQ_INTERVAL_POINTERr_WITH_FAST_ACCESS(unit,regval) \
    READ_OAMP_BFD_REQ_INTERVAL_POINTERr(unit, regval)

#define WRITE_OAMP_BFD_REQ_INTERVAL_POINTERr_WITH_FAST_ACCESS(unit, regval) \
    WRITE_OAMP_BFD_REQ_INTERVAL_POINTERr(unit,regval)

#define READ_OAMP_MPLS_PWE_PROFILEr_WITH_FAST_ACCESS(unit,regval) \
    READ_OAMP_MPLS_PWE_PROFILEr(unit, regval)

#define WRITE_OAMP_MPLS_PWE_PROFILEr_WITH_FAST_ACCESS(unit, regval) \
    WRITE_OAMP_MPLS_PWE_PROFILEr(unit,regval)

#define READ_OAMP_BFD_TX_IPV4_MULTI_HOPr_WITH_FAST_ACCESS(unit,regval) \
    READ_OAMP_BFD_TX_IPV4_MULTI_HOPr(unit, regval)

#define WRITE_OAMP_BFD_TX_IPV4_MULTI_HOPr_WITH_FAST_ACCESS(unit, regval) \
    WRITE_OAMP_BFD_TX_IPV4_MULTI_HOPr(unit,regval)

#define READ_OAMP_PR_2_FW_DTCr_WITH_FAST_ACCESS(unit,regval) \
    READ_OAMP_PR_2_FW_DTCr(unit, regval)

#define WRITE_OAMP_PR_2_FW_DTCr_WITH_FAST_ACCESS(unit, regval) \
    WRITE_OAMP_PR_2_FW_DTCr(unit,regval)



#endif











#define GET_TYPE_OF_OAMP_MEP_DB_ENTRY(__type, __entry_id, __reg_above_64)\
        SOC_REG_ABOVE_64_CLEAR(__reg_above_64);\
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,__entry_id, __reg_above_64));\
        __type = soc_OAMP_MEP_DBm_field32_get(unit, __reg_above_64, MEP_TYPEf)


#define CLEAR_OAMP_MEP_DB_ENTRY(__entry_id, _reg_above_64) \
        SOC_REG_ABOVE_64_CLEAR(_reg_above_64);\
        do{\
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ALL, __entry_id, _reg_above_64));\
            SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);\
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ALL, __entry_id, entry_above_64_get));\
        }while (sal_memcmp(_reg_above_64, entry_above_64_get, (soc_mem_entry_bytes(unit,OAMP_MEP_DBm))))

#define GET_MEP_AND_DA_NIC_PROFILES_FROM_OAMP_MEP_DB_ENTRY(__profile, da_nic_profile,  __mep_type ,__endpoint_id, __reg_data) \
    do{\
        soc_mem_t mem;\
         switch (__mep_type) {\
        case SOC_PPC_OAM_MEP_TYPE_ETH_OAM:\
            mem = OAMP_MEP_DBm;\
            break;\
        case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP:\
        case SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS:\
            mem = OAMP_MEP_DB_Y_1731_ON_MPLSTPm;\
            break;\
        case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE:\
        case SOC_PPC_OAM_MEP_TYPE_Y1711_PWE:\
            mem = OAMP_MEP_DB_Y_1731_ON_PWEm;\
            break;\
        case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION:\
            mem = OAMP_MEP_DB_Y_1731_ON_PWEm;\
            break;\
        case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL:\
            mem = OAMP_MEP_DB_Y_1731_ON_MPLSTPm;\
            break;\
        case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:\
        case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:\
        case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE:\
        case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE_GAL:\
        case SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS:\
        case SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP:\
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Accelerated LM/DM in HW usupported for BFD.")));\
            break;\
        default:\
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Internal error: CCM entry type invalid"))); break;\
        }\
        SOC_REG_ABOVE_64_CLEAR(__reg_data); \
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, __endpoint_id, __reg_data));        \
       __profile =   soc_mem_field32_get(unit, mem, __reg_data, MEP_PROFILEf); \
    da_nic_profile = (__mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM)?soc_OAMP_MEP_DBm_field32_get(unit, __reg_data, LMM_DA_NIC_PROFILEf):-1;\
    }while (0); 

          
#define GET_OAMP_ETH_1731_FIELD_BY_INDEX_LMM_RATE(_field, _index) \
do {\
        soc_reg_t lmm_rate_fields[] = {PROF_0_LMM_RATEf, PROF_1_LMM_RATEf, PROF_2_LMM_RATEf, PROF_3_LMM_RATEf, PROF_4_LMM_RATEf, PROF_5_LMM_RATEf, PROF_6_LMM_RATEf, \
        PROF_7_LMM_RATEf, PROF_8_LMM_RATEf, PROF_9_LMM_RATEf, PROF_10_LMM_RATEf, PROF_11_LMM_RATEf, PROF_12_LMM_RATEf, PROF_13_LMM_RATEf, PROF_14_LMM_RATEf, \
        PROF_15_LMM_RATEf};\
        _field = lmm_rate_fields[_index];\
} while (0); 


#define GET_OAMP_ETH_1731_FIELD_BY_INDEX_DMM_RATE(_field, _index) \
do {\
    soc_reg_t dmm_rate_fields[] = {PROF_0_DMM_RATEf,PROF_1_DMM_RATEf,PROF_2_DMM_RATEf,PROF_3_DMM_RATEf,PROF_4_DMM_RATEf,PROF_5_DMM_RATEf,\
        PROF_6_DMM_RATEf,PROF_7_DMM_RATEf,PROF_8_DMM_RATEf,PROF_9_DMM_RATEf,PROF_10_DMM_RATEf,PROF_11_DMM_RATEf,PROF_12_DMM_RATEf,PROF_13_DMM_RATEf,\
        PROF_14_DMM_RATEf,PROF_15_DMM_RATEf};\
    _field = dmm_rate_fields[_index];\
} while (0); 

#define GET_OAMP_ETH_1731_FIELD_BY_INDEX_PIGGY_BACK(_field, _index) \
do {\
    soc_reg_t piggy_back_lm_fields[] = {PROF_0_PIGGYBACK_LMf, PROF_1_PIGGYBACK_LMf, PROF_2_PIGGYBACK_LMf, PROF_3_PIGGYBACK_LMf, PROF_4_PIGGYBACK_LMf, PROF_5_PIGGYBACK_LMf, \
        PROF_6_PIGGYBACK_LMf, PROF_7_PIGGYBACK_LMf, PROF_8_PIGGYBACK_LMf, PROF_9_PIGGYBACK_LMf, PROF_10_PIGGYBACK_LMf, PROF_11_PIGGYBACK_LMf, PROF_12_PIGGYBACK_LMf, \
        PROF_13_PIGGYBACK_LMf, PROF_14_PIGGYBACK_LMf,PROF_15_PIGGYBACK_LMf };\
    _field = piggy_back_lm_fields[_index];\
} while (0); 

#define GET_OAMP_ETH_1731_FIELD_BY_INDEX_DA_OUI_PROF(_field, _index) \
do {\
    soc_reg_t lmm_da_oui_prof_fields[] = {PROF_0_LMM_DA_OUI_PROFILEf, PROF_1_LMM_DA_OUI_PROFILEf, PROF_2_LMM_DA_OUI_PROFILEf, PROF_3_LMM_DA_OUI_PROFILEf, \
        PROF_4_LMM_DA_OUI_PROFILEf, PROF_5_LMM_DA_OUI_PROFILEf, PROF_6_LMM_DA_OUI_PROFILEf, PROF_7_LMM_DA_OUI_PROFILEf, PROF_8_LMM_DA_OUI_PROFILEf, \
        PROF_9_LMM_DA_OUI_PROFILEf, PROF_10_LMM_DA_OUI_PROFILEf, PROF_11_LMM_DA_OUI_PROFILEf, PROF_12_LMM_DA_OUI_PROFILEf, PROF_13_LMM_DA_OUI_PROFILEf, \
        PROF_14_LMM_DA_OUI_PROFILEf, PROF_15_LMM_DA_OUI_PROFILEf};\
     _field = lmm_da_oui_prof_fields[_index];\
} while (0); 

#define GET_OAMP_ETH_1731_FIELD_BY_INDEX_RDI_GEN_METHOD(_field, _index) \
do {\
    soc_reg_t rdi_fields_fields[]  = { PROF_0_RDI_GEN_METHODf,PROF_1_RDI_GEN_METHODf,PROF_2_RDI_GEN_METHODf,PROF_3_RDI_GEN_METHODf,PROF_4_RDI_GEN_METHODf,\
    PROF_5_RDI_GEN_METHODf,PROF_6_RDI_GEN_METHODf,PROF_7_RDI_GEN_METHODf,PROF_8_RDI_GEN_METHODf,PROF_9_RDI_GEN_METHODf,PROF_10_RDI_GEN_METHODf,\
        PROF_11_RDI_GEN_METHODf,PROF_12_RDI_GEN_METHODf,PROF_13_RDI_GEN_METHODf,PROF_14_RDI_GEN_METHODf,PROF_15_RDI_GEN_METHODf,};\
     _field = rdi_fields_fields[_index];\
} while (0); 


    
#define GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_LMM_OFFSET(field,index) \
do {\
    soc_reg_t lmm_fields[] = {LMM_OFFSET_PROFILE_0f,LMM_OFFSET_PROFILE_1f,LMM_OFFSET_PROFILE_2f,LMM_OFFSET_PROFILE_3f,\
    LMM_OFFSET_PROFILE_4f,LMM_OFFSET_PROFILE_5f,LMM_OFFSET_PROFILE_6f,LMM_OFFSET_PROFILE_7f,LMM_OFFSET_PROFILE_7f,\
    LMM_OFFSET_PROFILE_8f,LMM_OFFSET_PROFILE_9f,LMM_OFFSET_PROFILE_10f,LMM_OFFSET_PROFILE_11f,LMM_OFFSET_PROFILE_12f,\
    LMM_OFFSET_PROFILE_13f,LMM_OFFSET_PROFILE_14f,LMM_OFFSET_PROFILE_15f};\
     field = lmm_fields[index];\
} while (0); 


#define GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_DMM_OFFSET(field, index) \
do {\
    soc_reg_t dmm_fields[] = {DMM_OFFSET_PROFILE_0f,DMM_OFFSET_PROFILE_1f,DMM_OFFSET_PROFILE_2f,DMM_OFFSET_PROFILE_3f,\
    DMM_OFFSET_PROFILE_4f,DMM_OFFSET_PROFILE_5f,DMM_OFFSET_PROFILE_6f,DMM_OFFSET_PROFILE_7f,DMM_OFFSET_PROFILE_7f,\
    DMM_OFFSET_PROFILE_8f,DMM_OFFSET_PROFILE_9f,DMM_OFFSET_PROFILE_10f,DMM_OFFSET_PROFILE_11f,DMM_OFFSET_PROFILE_12f,\
    DMM_OFFSET_PROFILE_13f,DMM_OFFSET_PROFILE_14f,DMM_OFFSET_PROFILE_15f};\
     field = dmm_fields[index];\
} while (0); 

#define GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_DMR_OFFSET(field, index) \
do {\
    soc_reg_t dmr_fields[] = {DMR_OFFSET_PROFILE_0f,DMR_OFFSET_PROFILE_1f,DMR_OFFSET_PROFILE_2f,DMR_OFFSET_PROFILE_3f,\
    DMR_OFFSET_PROFILE_4f,DMR_OFFSET_PROFILE_5f,DMR_OFFSET_PROFILE_6f,DMR_OFFSET_PROFILE_7f,DMR_OFFSET_PROFILE_7f,\
    DMR_OFFSET_PROFILE_8f,DMR_OFFSET_PROFILE_9f,DMR_OFFSET_PROFILE_10f,DMR_OFFSET_PROFILE_11f,DMR_OFFSET_PROFILE_12f,\
    DMR_OFFSET_PROFILE_13f,DMR_OFFSET_PROFILE_14f,DMR_OFFSET_PROFILE_15f};\
     field = dmr_fields[index];\
} while (0); 

#define GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_LMR_OFFSET(field, index) \
do {\
    soc_reg_t lmr_fields[] = {LMR_OFFSET_PROFILE_0f,LMR_OFFSET_PROFILE_1f,LMR_OFFSET_PROFILE_2f,LMR_OFFSET_PROFILE_3f,\
    LMR_OFFSET_PROFILE_4f,LMR_OFFSET_PROFILE_5f,LMR_OFFSET_PROFILE_6f,LMR_OFFSET_PROFILE_7f,LMR_OFFSET_PROFILE_7f,\
    LMR_OFFSET_PROFILE_8f,LMR_OFFSET_PROFILE_9f,LMR_OFFSET_PROFILE_10f,LMR_OFFSET_PROFILE_11f,LMR_OFFSET_PROFILE_12f,\
    LMR_OFFSET_PROFILE_13f,LMR_OFFSET_PROFILE_14f,LMR_OFFSET_PROFILE_15f};\
     field = lmr_fields[index];\
} while (0); 




#define PRINT_FOUND(found) \
     if (COMPILER_64_LO(found)) { \
         LOG_CLI((BSL_META_U(unit, \
                             ".\n"))); \
     } else { \
         LOG_CLI((BSL_META_U(unit, \
                             " (not found).\n"))); \
     }    



#define ARAD_PP_DIAG_FLD_FILL(prm_fld, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb)  \
          (prm_fld)->base = (prm_addr_msb << 16) + prm_addr_lsb;  \
          (prm_fld)->msb = prm_fld_msb;  \
          (prm_fld)->lsb= prm_fld_lsb;


#define ARAD_PP_OAM_DIAG_FLD_READ(prm_fld, core_id, prm_blk, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb,start_bit, end_bit)  \
    do {\
    uint32 __zero=0;\
    ARAD_PP_DIAG_FLD_FILL(prm_fld, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb);    \
      res = arad_pp_diag_dbg_val_get_unsafe(unit, core_id, prm_blk, prm_fld, regs_val);  \
      SOCDNX_IF_ERR_EXIT(res);\
     SHR_BITCOPY_RANGE(regs_val,end_bit+1,&__zero,0,31-end_bit-start_bit);\
         if (start_bit !=0) {\
             SHR_BITCOPY_RANGE(regs_val,0,&__zero,0,start_bit);\
         }\
    } while (0);


#define ROUND_UP_TO_NEAREST_POWER_OF_2(N) \
    --N;  N |= N>>1 ; N |= N>>2 ; N|=N>>4  ; \
     N |= N>>8 ; N |= N>>16; ++N;











VOLATILE sal_sem_t     jer_oam_event_fifo_sem[SOC_SAND_MAX_DEVICE];
VOLATILE sal_thread_t  jer_oam_event_fifo_tid[SOC_SAND_MAX_DEVICE];
VOLATILE uint8         jer_oam_event_fifo_terminate[SOC_SAND_MAX_DEVICE];

VOLATILE sal_sem_t     jer_oam_stat_fifo_sem[SOC_SAND_MAX_DEVICE];
VOLATILE sal_thread_t  jer_oam_stat_fifo_tid[SOC_SAND_MAX_DEVICE];
VOLATILE uint8         jer_oam_stat_fifo_terminate[SOC_SAND_MAX_DEVICE];

ARAD_PP_INTERNAL_OPCODE_INIT   internal_opcode_init[SOC_PPC_OAM_OPCODE_MAP_COUNT] = {{0}};

CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_oam[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_INIT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_DEINIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_DEINIT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_ICC_MAP_REGISTER_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_ICC_MAP_REGISTER_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_ICC_MAP_REGISTER_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_ICC_MAP_REGISTER_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_ICC_MAP_REGISTER_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_ICC_MAP_REGISTER_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MY_CFM_MAC_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MY_CFM_MAC_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET_UNSAFE),  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET_UNSAFE), 
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM2_ENTRY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM2_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_MEP_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_MEP_DELETE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_MEP_DELETE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_MEP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_MEP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_MEP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_DELETE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_DELETE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_DELETE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_INDEX_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_INDEX_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_RMEP_INDEX_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_INIT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_INIT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_COUNTER_RANGE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_COUNTER_RANGE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_COUNTER_RANGE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_COUNTER_RANGE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_COUNTER_RANGE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_COUNTER_RANGE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_RATE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_RATE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_RATE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_RATE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_RATE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_RATE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MPLS_PWE_PROFILE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MPLS_PWE_PROFILE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MPLS_PWE_PROFILE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MPLS_PWE_PROFILE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MPLS_PWE_PROFILE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_MPLS_PWE_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_MPLS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_MPLS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_MPLS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_MPLS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_MPLS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFD_TX_MPLS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCC_TX_MPLS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCC_TX_MPLS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCC_TX_MPLS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCC_TX_MPLS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCC_TX_MPLS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCC_TX_MPLS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCV_TX_MPLS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCV_TX_MPLS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCV_TX_MPLS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCV_TX_MPLS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCV_TX_MPLS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_BFDCV_TX_MPLS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_EVENT_FIFO_READ_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_EVENT_FIFO_READ_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_EVENT_FIFO_READ),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_PP_PCT_PROFILE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_PP_PCT_PROFILE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_PP_PCT_PROFILE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_PP_PCT_PROFILE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_PP_PCT_PROFILE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_OAM_PP_PCT_PROFILE_GET),

  

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_oam[] =
{
  
  {
    ARAD_PP_OAM_CLASSIFIER_DA_SUFFIX_CONFLICT_ERR,
    "ARAD_PP_OAM_CLASSIFIER_DA_SUFFIX_CONFLICT_ERR",
    "All MEPs on the same PP Port must share same DA[47:8] suffix.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_OAMP_SA_SUFFIX_CONFLICT_ERR,
    "ARAD_PP_OAM_OAMP_SA_SUFFIX_CONFLICT_ERR",
    " All MEPs in a device must share same SA[47:8] suffix.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_CLASSIFIER_INCONSISTENT_LIF_DATA_ERR,
    "ARAD_PP_OAM_CLASSIFIER_INCONSISTENT_LIF_DATA_ERR",
    "Different MEPs on same LIF must have same direction and counter index.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_CLASSIFIER_MDLEVEL_CHECK_FAIL_ERR,
    "ARAD_PP_OAM_CLASSIFIER_MDLEVEL_CHECK_FAIL_ERR",
    "All mips should be above maps. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_CLASSIFIER_ENTRY_EXISTS_ERR,
    "ARAD_PP_OAM_CLASSIFIER_ENTRY_EXISTS_ERR",
    "Adding existing entry to the classifier. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR,
    "ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR",
    "Entry of OEM1 doesn't exist. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'mep_index' is out of range. \n\r "
    "The range is: 0 - 8k.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_RMEP_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_RMEP_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'rmep_index' is out of range. \n\r "
    "The range is: 0 - 8k.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_EM_FULL_ERR,
    "ARAD_PP_OAM_EM_FULL_ERR",
    "Exact match is full. \n\r "
    ".\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_EM_INTERNAL_ERR,
    "ARAD_PP_OAM_EM_INTERNAL_ERR",
    "Exact match internal error. \n\r "
    ".\n\r ",
    SOC_SAND_SVR_FTL,
    FALSE
  },
  {
    ARAD_PP_OAM_EM_INSERTED_EXISTING_ERR,
    "ARAD_PP_OAM_EM_INSERTED_EXISTING_ERR",
    "Exact match error: Key exists. \n\r "
    ".\n\r ",
    SOC_SAND_SVR_FTL,
    FALSE
  },
  {
    ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR",
    "The parameter 'md_level' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_FORWARDING_SRTENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_FORWARDING_SRTENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'forwarding_strength' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_MA_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_MA_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'ma_index' is out of range. \n\r "
    "The range is: 0 - 8k.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_MEP_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_MEP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'mep_type' is out of range. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_PORT_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_PORT_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port' is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_MEP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'non_acc_profile' is out of range. \n\r "
    "The range is: 0 - 4 for ARAD and 1-15 for ARAD+.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_ACC_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_MEP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'non_acc_profile' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_PROFILE_OPCODE_MAP_ERROR,
    "ARAD_PP_OAM_PROFILE_OPCODE_MAP_ERROR",
    "ARAD_PP_OAM opcode to trap code map value is out of range. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_TRAP_TO_MIRROR_PROFILE_MAP_ERROR,
    "ARAD_PP_OAM_TRAP_TO_MIRROR_PROFILE_MAP_ERROR",
    "ARAD_PP_OAM trap code to mirror profile map value is out of range. \n\r "
    "The range is: 0 - 16.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_TX_RATE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_TX_RATE_INDEX_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM tx rate index is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_REQ_INTERVAL_POINTER_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_REQ_INTERVAL_POINTER_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM req interval pointer is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_DIP_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_DIP_INDEX_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM DIP index is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_EXP_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_EXP_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM EXP is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_TOS_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_TOS_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM TOS is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_TTL_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_TTL_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM TTL is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_DP_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_DP_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM DP is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_TC_OUT_OF_RANGE_ERR,
    "ARAD_PP_OAM_TC_OUT_OF_RANGE_ERR",
    "ARAD_PP_OAM TC is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR,
    "ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR",
    "ARAD_PP_OAM some of the values of bfd pdu static register are out of range. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_YOUR_DISC_ABOVE_MAX_ERROR,
    "ARAD_PP_OAM_YOUR_DISC_ABOVE_MAX_ERROR",
    "ARAD_PP_OAM your discriminator value is out of range. \n\r "
    "The range is: 0 - 32k.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_OAM_INTERNAL_ERROR,
    "ARAD_PP_OAM_INTERNAL_ERROR",
    "ARAD_PP_OAM internal error \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },


  


  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





uint32 _arad_pp_oam_mpls_tp_channel_type_enum_to_channel_type(int unit, _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM channel_type_ndx) {
   uint32 channel_type;

   switch (channel_type_ndx) {
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_BFD_CONTROL_ENUM:
      channel_type = soc_property_get(unit, spn_MPLSTP_BFD_CONTROL_CHANNEL_TYPE, 0x0007);
      break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_PW_ACH_ENUM:
      channel_type = soc_property_get(unit, spn_MPLSTP_PW_ACH_CHANNEL_TYPE, 0x0007);
      break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_DLM_ENUM:
      channel_type = soc_property_get(unit, spn_MPLSTP_DLM_CHANNEL_TYPE, 0x000A);
      break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_ILM_ENUM:
      channel_type = soc_property_get(unit, spn_MPLSTP_ILM_CHANNEL_TYPE, 0x000B);
      break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_DM_ENUM:
      channel_type = soc_property_get(unit, spn_MPLSTP_DM_CHANNEL_TYPE, 0x000C);
      break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_IPV4_ENUM:
      channel_type = soc_property_get(unit, spn_MPLSTP_IPV4_CHANNEL_TYPE, 0);
      break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_IPV6_ENUM:
      channel_type = soc_property_get(unit, spn_MPLSTP_IPV6_CHANNEL_TYPE, 0);
      break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_BFD_VCCV:
      channel_type = soc_property_get(unit, spn_MPLSTP_CV_CHANNEL_TYPE, 0x0021);
      break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_CC_ENUM:
      channel_type = soc_property_get(unit, spn_MPLSTP_CC_CHANNEL_TYPE, 0x0022);
      break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_CV_ENUM:
      channel_type = soc_property_get(unit, spn_MPLSTP_CV_CHANNEL_TYPE, 0x0023);
      break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_ON_DEMAND_CV_ENUM:
      channel_type = soc_property_get(unit, spn_MPLSTP_ON_DEMAND_CV_CHANNEL_TYPE, 0x0025);
      break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_PWE_ON_OAM_ENUM:
      channel_type = soc_property_get(unit, spn_MPLSTP_PWE_OAM_CHANNEL_TYPE, 0x0027);
      break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_Y1731_ENUM:
      channel_type = soc_property_get(unit, spn_MPLSTP_G8113_CHANNEL_TYPE, 0x8902);
      break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_FAULT_OAM:
      channel_type = soc_property_get(unit, spn_MPLSTP_FAULT_OAM_CHANNEL_TYPE, 0x8902);
      break;
   default:
      channel_type = 0;
   }
   return (channel_type);
}




uint32    _arad_pp_oam_mpls_tp_channel_type_enum_to_format(uint32 channel_type) {
   uint32 channel_format;
   switch (channel_type) {
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_Y1731_ENUM:
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_PWE_ON_OAM_ENUM:
      channel_format = _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_Y1731; break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_BFD_VCCV:
      channel_format = _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_BFD_IPV4; break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_CC_ENUM:
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_CV_ENUM:
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_BFD_CONTROL_ENUM:
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_IPV4_ENUM:
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_IPV6_ENUM:
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_ON_DEMAND_CV_ENUM:
      channel_format = _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_BFD_RAW; break;
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_DLM_ENUM:
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_ILM_ENUM:
   case _ARAD_PP_OAM_MPLS_TP_CHANNEL_DM_ENUM:
      channel_format = _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_RFC6374; break;
   default:
      channel_format = _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_OAM_UNKNOWN;
   }
   return (channel_format);
}




uint8 _arad_pp_oam_mpls_tp_channel_type_enum_to_internal_opcode(int unit, uint32 channel_type) {
  uint32 res;
  uint8 internal_opcode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (channel_type) {
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_DLM_ENUM:
      res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM, &internal_opcode);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      break;
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_ILM_ENUM:
      res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM, &internal_opcode);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      break;
    case _ARAD_PP_OAM_MPLS_TP_CHANNEL_DM_ENUM:
      res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMM, &internal_opcode);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      break;
    default:
      internal_opcode = 0;
  }

  return internal_opcode;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_oam_mpls_tp_channel_type_enum_to_internal_opcode()", 0, 0);
}

STATIC
uint32
  arad_pp_oam_tcam_entry_to_action(
      SOC_SAND_IN  SOC_PPC_OAM_TCAM_ENTRY_ACTION  *entry,
      SOC_SAND_OUT ARAD_TCAM_ACTION               *action
  );
STATIC
  void
    arad_pp_oam_y1711_tcam_entry_build(
      SOC_SAND_IN     int                                  unit,
      SOC_SAND_IN  SOC_PPC_OAM_TCAM_ENTRY_KEY *key,

      SOC_SAND_OUT ARAD_TCAM_ENTRY           *entry
  );

STATIC
  void
    arad_pp_oam_tcam_entry_build(
      SOC_SAND_IN     int                                  unit,
      SOC_SAND_IN  SOC_PPC_OAM_TCAM_ENTRY_KEY *key,

      SOC_SAND_OUT ARAD_TCAM_ENTRY           *entry
  );

STATIC
  void
    arad_pp_oam_tcam_entry_build_mpls_ignore_mdl_jericho(
      SOC_SAND_IN     int                                  unit,
      SOC_SAND_IN  SOC_PPC_OAM_TCAM_ENTRY_KEY *key,

      SOC_SAND_OUT ARAD_TCAM_ENTRY           *entry
  );

STATIC
  void
    arad_pp_oam_bfd_sbfd_tcam_entry_build(
      SOC_SAND_IN     int                                  unit,
      SOC_SAND_IN  SOC_PPC_OAM_TCAM_ENTRY_KEY *key,
      SOC_SAND_IN  uint32 flags,
      SOC_SAND_OUT ARAD_TCAM_ENTRY           *entry
  )
{
    uint32 mask = 0;
    uint32 val = 0;
    uint32 offset = 0;
    uint32 field_size = 0;
    int packet_data_start_offset = 0;
    int ip_protocol_offset_in_bits = 0;
    int dip_addr_offset_in_bits = 0;
    int ip_ttl_offset_in_bits = 0;
    int udp_src_port_offset_in_bits = 0;
    int udp_des_port_offset_in_bits = 0;
    ARAD_TCAM_ENTRY_clear(entry);

    entry->valid = TRUE;
    entry->is_for_update = 0;
    entry->is_inserted_top = 1;
    

    
    mask=0;
    field_size = 4;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&key->ttc, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask, 	_ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    
    mask = 0xFFFFFFFF; 
    field_size = 4;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&key->fwd_code, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask, 		 _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    
    mask = 0;
    val = 0; 
    field_size = SOC_IS_JERICHO_PLUS(unit) ? 4:3;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size , entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    
    mask = 0;
    val = 0;
    field_size = 1;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    if(SOC_IS_JERICHO_PLUS(unit)){
    	
    	mask = 0;
    	val = 0;
    	field_size = 1;
    	offset += field_size;
    	soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    	soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);
        offset = offset + 2; 
    }

    
    packet_data_start_offset = offset;

    
    ip_protocol_offset_in_bits = 9 * SOC_SAND_NOF_BITS_IN_BYTE; 
    dip_addr_offset_in_bits = 16 * SOC_SAND_NOF_BITS_IN_BYTE; 
    udp_src_port_offset_in_bits = SOC_SAND_PP_IP_NO_OPTION_NOF_BITS;
    udp_des_port_offset_in_bits = udp_src_port_offset_in_bits + 16; 
    ip_ttl_offset_in_bits = 8 * SOC_SAND_NOF_BITS_IN_BYTE; 

    
    
    mask = 0xFFFFFFFF;
    val = SOC_SAND_PP_IP_PROTOCOL_UDP; 
    field_size = SOC_SAND_PP_IP_PROTOCOL_NOF_BITS; 
    offset =  packet_data_start_offset + ip_protocol_offset_in_bits + field_size;
    soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    if(flags == _ARAD_PP_OAM_TCAM_FLAGS_SBFD_REFLECTOR_OVER_MPLS)
    {
        
        mask = 0xFF;
        val =1;
        field_size = SOC_SAND_PP_IP_TTL_NOF_BITS; 
        offset = packet_data_start_offset + ip_ttl_offset_in_bits + field_size;
        soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
        soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);
    }
    else{
        
        mask = 0xFFFFFFFF;
        val = key->your_disc; 
        field_size = SOC_SAND_PP_IPV4_ADDRESS_NOF_BITS; 
        offset = packet_data_start_offset + dip_addr_offset_in_bits + field_size;
        soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
        soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);
    }
   
    mask = 0xFFFFFFFF;
    val = 7784;
    field_size = 16; 
    if(flags == _ARAD_PP_OAM_TCAM_FLAGS_SBFD_INITIATOR){
        offset = packet_data_start_offset + udp_src_port_offset_in_bits + field_size;
    }
    else if((flags == _ARAD_PP_OAM_TCAM_FLAGS_SBFD_REFLECTOR)||(flags == _ARAD_PP_OAM_TCAM_FLAGS_SBFD_REFLECTOR_OVER_MPLS)){
        offset = packet_data_start_offset + udp_des_port_offset_in_bits + field_size;
    }
    soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

}

STATIC
  void
    arad_pp_oam_bfd_mpls_special_ttl_tcam_entry_build(
      SOC_SAND_IN  int                        unit,
      SOC_SAND_IN  SOC_PPC_OAM_TCAM_ENTRY_KEY *key,
      SOC_SAND_IN  uint32 flags,
      SOC_SAND_OUT ARAD_TCAM_ENTRY           *entry
  )
{
    uint32 mask = 0;
    uint32 val = 0;
    uint32 offset = 0;
    uint32 field_size = 0;
    uint32 ip_begin_bit = 0;
    int ip_offset = 0; 

    ARAD_TCAM_ENTRY_clear(entry);

    entry->valid = TRUE;
    entry->is_for_update = 0;
    entry->is_inserted_top = 1;
    

    
    mask = 0xFFFFFFFE;  
    field_size = 4;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&key->ttc, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask,     _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    mask = 0xFFFFFFFF; 
    field_size = 4;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&key->fwd_code, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask,          _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    offset += 8;
    ip_begin_bit = offset; 

    
    mask = 0xFFFFFFFF;
    val = 4; 
    ip_offset += 4; 
    soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 4, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 4, entry->mask);

    mask = 0xFFFFFFFF;
    val = 5; 
    ip_offset += 4; 
    soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 4, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 4, entry->mask);

    mask = 0xFFFFFFFF;
    val = key->ip_ttl; 
    ip_offset += 64; 
    soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 8, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 8, entry->mask);

    mask = 0xFFFFFFFF;
    val = 17; 
    ip_offset += 8; 
    soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 8, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 8, entry->mask);

    mask = 0xffffffff;
    val = 127; 
    ip_offset += 56;  
    soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 8, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 8, entry->mask);

    mask = 0xFFFFFFFF;
    val = 3; 
    ip_offset += 24 + 2;  
    soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 2, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 2, entry->mask);

    mask = 0xFFFFFFFF;
    val = 0x0ec8; 
    ip_offset += 30; 
    soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 16, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 16, entry->mask);
}

int _arad_pp_oam_tcam_new_entry(int                            unit,
                                SOC_PPC_OAM_TCAM_ENTRY_KEY     *oam_tcam_entry_key,
                                SOC_PPC_OAM_TCAM_ENTRY_ACTION  *oam_tcam_entry_action,
                                uint32                         tcam_entry_id,
                                uint32                         flags)
{
   ARAD_TCAM_ENTRY   _tcam_entry;
   ARAD_TCAM_ACTION  _tcam_action;
   uint32            _success;
   int               res;

   SOCDNX_INIT_FUNC_DEFS;

   ARAD_TCAM_ENTRY_clear(&_tcam_entry);
   ARAD_TCAM_ACTION_clear(&_tcam_action);
   
   res = arad_pp_oam_tcam_entry_to_action(oam_tcam_entry_action, &_tcam_action);
   SOCDNX_SAND_IF_ERR_RETURN(res);
   
   if (flags == _ARAD_PP_OAM_TCAM_FLAGS_MPLS_IGNORE_MDL_JER) {
       arad_pp_oam_tcam_entry_build_mpls_ignore_mdl_jericho(unit, oam_tcam_entry_key, &_tcam_entry);
   } else if (flags == _ARAD_PP_OAM_TCAM_FLAGS_Y1711) {
       arad_pp_oam_y1711_tcam_entry_build(unit, oam_tcam_entry_key, &_tcam_entry);
   } else if ((flags == _ARAD_PP_OAM_TCAM_FLAGS_SBFD_INITIATOR) || (flags == _ARAD_PP_OAM_TCAM_FLAGS_SBFD_REFLECTOR) || (flags == _ARAD_PP_OAM_TCAM_FLAGS_SBFD_REFLECTOR_OVER_MPLS)) {
       arad_pp_oam_bfd_sbfd_tcam_entry_build(unit, oam_tcam_entry_key,flags, &_tcam_entry);
   } else if (flags == _ARAD_PP_OAM_TCAM_FLAGS_BFD_OVER_MPLS_SPECIAL_TTL) {
       arad_pp_oam_bfd_mpls_special_ttl_tcam_entry_build(unit, oam_tcam_entry_key, flags, &_tcam_entry);
   } else {
       arad_pp_oam_tcam_entry_build(unit, oam_tcam_entry_key, &_tcam_entry);
   }

   res = arad_tcam_managed_db_entry_add_unsafe(
      unit,
      ARAD_PP_TCAM_STATIC_ACCESS_ID_OAM_IDENTIFICATION,
      tcam_entry_id,
      FALSE,
      1,
      &_tcam_entry,
      &_tcam_action,
      &_success
      );
   
   SOCDNX_SAND_IF_ERR_RETURN(res);
   if (_success != SOC_SAND_SUCCESS) {
      LOG_ERROR(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "   Error in OAM database entry add. \n\r")));
      LOG_ERROR(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "   Tcam_last_entry_id:%d\n\r"), tcam_entry_id));
      res = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
      SOCDNX_SAND_IF_ERR_RETURN(res);
   }

   SOC_EXIT;
exit:
   SOCDNX_FUNC_RETURN;
}

int      _arad_pp_oam_rmep_db_entry_key_get(int                     unit,
                                            SOC_SAND_IN    SOC_PPC_OAM_MEP_TYPE    mep_type,
                                            SOC_SAND_IN    uint32                  mep_index,
                                            uint32                  *rmep_key,
                                            SOC_SAND_IN    uint16                  rmep_id) {
   uint64      reg_val64;
   soc_field_t field_name;
   uint16      field;
   uint64      field64;

   SOCDNX_INIT_FUNC_DEFS;

   COMPILER_64_ZERO(field64);

   if ((mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) ||
       (mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
       (mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
       (mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
       (mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
       (mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) ||
       (mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL)) {
      *rmep_key = _ARAD_PP_OAM_RMEP_DB_ENTRY_KEY_GET_FROM_MEP_INDEX_AND_RMEP_ID(mep_index, rmep_id, _ARAD_PP_OAM_RMEP_EMC_SHIFT_ETH);
   } else {
      SOCDNX_SAND_IF_ERR_RETURN(READ_OAMP_BFD_EMC_CONSTr(unit, &reg_val64));
      switch (mep_type) {
      case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:
      case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
         field64      = soc_reg64_field_get(unit, OAMP_BFD_EMC_CONSTr, reg_val64, BFD_IPV4_EMC_CONSTf);
         break;
      case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE:
         field64      = soc_reg64_field_get(unit, OAMP_BFD_EMC_CONSTr, reg_val64, BFD_PWE_EMC_CONSTf);
         break;
      case SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS:
         field64      = soc_reg64_field_get(unit, OAMP_BFD_EMC_CONSTr, reg_val64, BFD_MPLS_EMC_CONSTf);
         break;
      case SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP:
         field_name   = SOC_IS_ARADPLUS(unit) ? BFD_PWE_EMC_CONSTf : BFDCC_EMC_CONSTf; 
         field64      = soc_reg64_field_get(unit, OAMP_BFD_EMC_CONSTr, reg_val64, field_name);
         break;
      default:
         SOCDNX_SAND_IF_ERR_RETURN(ARAD_PP_OAM_INTERNAL_ERROR);
      }
      field      = COMPILER_64_LO(field64);
      *rmep_key  = _ARAD_PP_OAM_RMEP_DB_ENTRY_KEY_GET_FROM_MEP_INDEX_AND_RMEP_ID(mep_index, field, _ARAD_PP_OAM_RMEP_EMC_SHIFT_BFD);
   }
   SOC_EXIT;
exit:
   SOCDNX_FUNC_RETURN;
}


void _arad_pp_oam_oem1_payload_struct_to_payload(              int                                       unit,
                                                 SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD payload_struct,
                                                               uint32                                    *payload)
{
   if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
       payload[0] = (payload_struct.mp_type_vector) | (payload_struct.counter_ndx << 16) | ((payload_struct.mp_profile &1) <<31);
       payload[1] = payload_struct.mp_profile >>1;
   }
   else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)){
       payload[0] = (((payload_struct.mp_profile & 0x3) << 29) + (payload_struct.counter_ndx << 16) + (payload_struct.mep_bitmap << 8) +
                  (payload_struct.mip_bitmap << 2) + ((payload_struct.mp_profile & 0xc) >> 2));
   }
        else {
           payload[0] = (payload_struct.mp_profile << 29) + (payload_struct.counter_ndx << 16) + (payload_struct.mep_bitmap << 8) + (payload_struct.mip_bitmap);
   }
}


void _arad_pp_oam_oam1_key_struct_to_key(int                                    unit,
                                         SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY  key_struct,
                                         uint32                                 *key) {
   if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
      *key = (key_struct.mp_profile) + ((key_struct.mp_type_jr & 0x3) << 4) + (key_struct.ingress << 6) +  (key_struct.opcode << 7) + (((key_struct.mp_type_jr & 0x4) >> 2) << 11) +
         (key_struct.inject << 13) + (key_struct.my_cfm_mac << 12);
   } else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
      *key = (key_struct.inject << 13) + (key_struct.my_cfm_mac << 12) + (key_struct.is_bfd << 11) + (key_struct.opcode << 7) +
         (key_struct.ingress << 6) + (key_struct.mp_type << 4) + (key_struct.mp_profile);
   } else {
      *key = (key_struct.inject << 13) + (key_struct.my_cfm_mac << 12) + (key_struct.is_bfd << 11) + (key_struct.opcode << 7) +
         (key_struct.ingress << 6) + (key_struct.mip_type << 4) + (key_struct.mep_type << 2) + (key_struct.mp_profile);
   }
}

void _arad_pp_oam_oam1_key_to_key_struct(int unit, uint32 key, SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY *key_struct) {
   if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
      key_struct->mp_profile = key & 0xf;
      key_struct->mp_type_jr = ((key >> 4) & 0x3) | (((key >> 11) & 0x1) << 2);
      key_struct->ingress = (key >> 6) & 0x1;
      key_struct->opcode = (key >> 7) & 0xf;
      key_struct->my_cfm_mac = (key >> 12) & 0x1;
      key_struct->inject = (key >> 13) & 0x1;
   } else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
      key_struct->mp_profile = key & 0xf;
      key_struct->mp_type = ((key >> 4) & 0x3);
      key_struct->ingress = (key >> 6) & 0x1;
      key_struct->opcode = (key >> 7) & 0xf;
      key_struct->is_bfd = (key >> 11) & 0x1;
      key_struct->my_cfm_mac = (key >> 12) & 0x1;
      key_struct->inject = (key >> 13) & 0x1;
   } else {
      key_struct->mp_profile = key & 0x3;
      key_struct->mep_type = ((key >> 2) & 0x3);
      key_struct->mp_type = ((key >> 4) & 0x3);
      key_struct->ingress = (key >> 6) & 0x1;
      key_struct->opcode = (key >> 7) & 0xf;
      key_struct->is_bfd = (key >> 11) & 0x1;
      key_struct->my_cfm_mac = (key >> 12) & 0x1;
      key_struct->inject = (key >> 13) & 0x1;
   }
}

void _arad_pp_default_mip_behavior_get(uint32 internal_opcode, uint32 is_my_cfm_mac, uint32 *action) {
   if (internal_opcode == SOC_PPC_OAM_OPCODE_MAP_LBM) {
      if (is_my_cfm_mac) {
         *action = _ARAD_PP_OAM_ACTION_RECYCLE;
      } else {
         *action = _ARAD_PP_OAM_ACTION_FRWRD;
      }
   } else if (internal_opcode == SOC_PPC_OAM_OPCODE_MAP_LTM) {
      if (is_my_cfm_mac) {
         *action = _ARAD_PP_OAM_ACTION_TRAP_CPU;
      } else {
         *action = _ARAD_PP_OAM_ACTION_SNOOP;
      }
   } else {
      *action = _ARAD_PP_OAM_ACTION_FRWRD;
   }
}


int _add_tcam_entry_and_set_wb_var(int                            unit,
                                   SOC_PPC_OAM_TCAM_ENTRY_KEY     oam_tcam_entry_key,
                                   SOC_PPC_OAM_TCAM_ENTRY_ACTION  oam_tcam_entry_action,
                                   uint32                         is_Mpls_ignore_MDL_Jericho)
{
uint32 tcam_last_id;
int    res;

   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   res = sw_state_access[unit].dpp.soc.arad.pp.oam.tcam_last_entry_id.get(unit, &tcam_last_id);
   SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);
   
   res =  _arad_pp_oam_tcam_new_entry(unit,
                                      &oam_tcam_entry_key,
                                      &oam_tcam_entry_action,
                                      tcam_last_id,
                                      (is_Mpls_ignore_MDL_Jericho ? _ARAD_PP_OAM_TCAM_FLAGS_MPLS_IGNORE_MDL_JER : 0));
   SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
   ++tcam_last_id;
   res = sw_state_access[unit].dpp.soc.arad.pp.oam.tcam_last_entry_id.set(unit, tcam_last_id);
   SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 13, exit);

   SOC_EXIT;
exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in _add_tcam_entry_and_set_wb_var()", 0, 0);
}


int set_mip_bitmep_from_mep_bitmep(int unit, uint8 *mip_bitmep, uint8 mep_bitmep) {
   uint32   i           = 2;
   uint32   mep_entered = 0;
   uint32   level;

   SOCDNX_INIT_FUNC_DEFS;

   *mip_bitmep = 0;
   for (level = 1; level <= 7; i <<= 1, ++level) {
      if (mep_bitmep & i) {
         (*mip_bitmep) |= level << (3 * mep_entered);
         ++mep_entered;
      }
   }
   if (mep_entered > 2) {
       SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS,(_BSL_SOCDNX_MSG("Only 2 endpoints may be entered per direction per LIF.")));
   }
exit:
   SOCDNX_FUNC_RETURN;
}


int get_mep_and_da_nic_profiles_from_oamp_mep_db_entry(int                    unit,
                                                       SOC_SAND_OUT  uint32   *__profile,
                                                       int                    *da_nic_profile,
                                                       uint32                 __mep_type,
                                                       SOC_SAND_IN   uint32   __endpoint_id,
                                                       soc_reg_above_64_val_t *__reg_data) {
   soc_mem_t mem;

   SOCDNX_INIT_FUNC_DEFS;

   
   if (SOC_IS_JERICHO(unit) && __mep_type == 7)
   {
      __mep_type = SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE;
   }

   switch (__mep_type) {
   case SOC_PPC_OAM_MEP_TYPE_ETH_OAM:
      mem = OAMP_MEP_DBm;
      break;
   case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP:
   case SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS:
      mem = OAMP_MEP_DB_Y_1731_ON_MPLSTPm;
      break;
   case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE:
   case SOC_PPC_OAM_MEP_TYPE_Y1711_PWE:
      mem = OAMP_MEP_DB_Y_1731_ON_PWEm;
      break;
   case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL:
      mem = OAMP_MEP_DB_Y_1731_ON_MPLSTPm;
      break;
   case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION:
      mem = OAMP_MEP_DB_Y_1731_ON_PWEm;
      break;
   case SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE:
      mem = OAMP_MEP_DB_RFC_6374_ON_MPLSTPm;
      break;
   case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:
   case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
   case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE:
   case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE_GAL:
   case SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS:
   case SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP:
      SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Accelerated LM/DM in HW usupported for BFD.")));
      break;
   default:
      SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOCDNX_MSG("Internal error: CCM entry type invalid"))); break;
   }
   SOC_REG_ABOVE_64_CLEAR(*__reg_data);
   SOCDNX_IF_ERR_EXIT(_ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, __endpoint_id, *__reg_data));
   *__profile =   soc_mem_field32_get(unit, mem, *__reg_data, MEP_PROFILEf);
   *da_nic_profile = (__mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) ? soc_OAMP_MEP_DBm_field32_get(unit, *__reg_data, LMM_DA_NIC_PROFILEf) : -1;

exit:
   SOCDNX_FUNC_RETURN;
}



int get_signal_for_direction(int unit, ARAD_PP_DIAG_REG_FIELD *fld, SOC_SAND_IN int core_id, uint32 *regs_val) {
   uint32   res;

   SOCDNX_INIT_FUNC_DEFS;

   if (SOC_IS_QAX(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID, 4, 6, 179, 179, 0, 0);
   } else if (SOC_IS_JERICHO_PLUS(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID, 4, 6, 183, 183, 0, 0);
   } else if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID, 4, 6, 113, 113, 0, 0);
   } else if (SOC_IS_ARADPLUS(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID, 0, 6, 18, 18, 0, 0);
   } else {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID, 0, 5, 136, 136, 0, 0);
   }
exit:
   SOCDNX_FUNC_RETURN;
}


int get_signal_for_oam_id_up(int unit, ARAD_PP_DIAG_REG_FIELD *fld, SOC_SAND_IN int core_id, uint32 *regs_val) {
   uint32   res;

   SOCDNX_INIT_FUNC_DEFS;

   if (SOC_IS_QAX(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_EPNI_ID, 9, 1, 211, 195, 0, 16);
   } else if (SOC_IS_JERICHO_PLUS(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_EPNI_ID, 9, 1, 255, 253, 0, 13);
   } else if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_EPNI_ID, 9, 1, 180, 164, 0, 16);
   } else {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_EPNI_ID, 9, 1, 25, 9, 0, 16);
   }
exit:
   SOCDNX_FUNC_RETURN;
}


int get_signal_for_oam_id_down(int unit, ARAD_PP_DIAG_REG_FIELD *fld, SOC_SAND_IN int core_id, uint32 *regs_val) {
   uint32   res;

   SOCDNX_INIT_FUNC_DEFS;

   if (SOC_IS_QAX(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID, 4, 6, 214, 198, 0, 16);
   } else if (SOC_IS_JERICHO_PLUS(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID, 4, 6, 218, 202, 0, 16);
   } else if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID, 4, 6, 148, 132, 0, 16);
   } else if (SOC_IS_ARADPLUS(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID, 0, 6, 53, 37, 0, 16);
   } else {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID, 0, 5, 171, 155, 0, 16);
   }
exit:
   SOCDNX_FUNC_RETURN;
}


int get_signal_for_subtype_up(int unit, ARAD_PP_DIAG_REG_FIELD *fld, SOC_SAND_IN int core_id, uint32 *regs_val) {
   uint32   res;

   SOCDNX_INIT_FUNC_DEFS;

   if (SOC_IS_QAX(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_EPNI_ID, 9, 1, 194, 192, 0, 2);
   } else if (SOC_IS_JERICHO_PLUS(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_EPNI_ID, 9, 1, 242, 240, 0, 2);
   } else if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_EPNI_ID, 9, 1, 163, 161, 0, 2);
   } else {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_EPNI_ID, 9, 1, 8, 6, 0, 2);
   }
exit:
   SOCDNX_FUNC_RETURN;
}


int get_signal_for_subtype_down(int unit, ARAD_PP_DIAG_REG_FIELD *fld, SOC_SAND_IN int core_id, uint32 *regs_val) {
   uint32   res;

   SOCDNX_INIT_FUNC_DEFS;

   if (SOC_IS_JERICHO_PLUS(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID, 0, 12, 138, 136, 0, 2);
   } else if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID, 0, 9, 224, 222, 0, 2);
   } else if (SOC_IS_ARADPLUS(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID, 9, 9, 203, 201, 0, 2);
   } else {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID, 0, 5, 139, 137, 0, 2);
   }
exit:
   SOCDNX_FUNC_RETURN;
}


int get_signal_for_trap_code(int unit, ARAD_PP_DIAG_REG_FIELD *fld, SOC_SAND_IN int core_id, uint32 *regs_val) {
   uint32   res;

   SOCDNX_INIT_FUNC_DEFS;

   if (SOC_IS_QAX(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID, 4, 8, 216, 209, 0, 7);
   } else if (SOC_IS_JERICHO_PLUS(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID, 4, 8, 221, 214, 0, 7);
   } else if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID, 4, 8, 151, 144, 0, 7);
   } else if (SOC_IS_ARADPLUS(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID, 0, 7, 208, 201, 0, 7);
   } else {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID, 0, 7, 59, 52, 0, 7);
   }
exit:
   SOCDNX_FUNC_RETURN;
}


int get_signal_for_oam_snoop_strength(int unit, ARAD_PP_DIAG_REG_FIELD *fld, SOC_SAND_IN int core_id, uint32 *regs_val) {
   uint32   res;

   SOCDNX_INIT_FUNC_DEFS;

   if (SOC_IS_QAX(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID, 4, 1, 247, 246, 0, 1);
   } else if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID, 4, 1, 251, 250, 0, 1);
   } else if (SOC_IS_ARADPLUS(unit)) {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID, 0, 1, 172, 171, 0, 1);
   } else {
      ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID, 0, 1, 34, 33, 0, 1);
   }
exit:
   SOCDNX_FUNC_RETURN;
}


int get_signal_for_oam_tcam_key_all(int unit, ARAD_PP_DIAG_REG_FIELD *fld, SOC_SAND_IN int core_id, uint32 *tcam_key_val) {
   uint32   res;
   uint32   regs_val[ARAD_TCAM_ENTRY_MAX_LEN] = {0};

   SOCDNX_INIT_FUNC_DEFS;

   if (SOC_IS_QAX(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,30,0,255,166,0,89);
   } else if (SOC_IS_JERICHO(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,28,0,255,166,0,89);
   } else {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID,15,0,255,166,0,89);
   }
   SHR_BITCOPY_RANGE(tcam_key_val,0,regs_val,0,90);

   if (SOC_IS_QAX(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,30,1,69,0,0,69);
   } else if (SOC_IS_JERICHO(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,28,1,69,0,0,69);
   } else {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID,15,1,69,0,0,69);
   }
   SHR_BITCOPY_RANGE(tcam_key_val,90,regs_val,0,70);

   if (SOC_IS_QAX(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,30,0,159,0,0,159);
   } else if (SOC_IS_JERICHO(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,28,0,159,0,0,159);
   } else {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID,15,0,159,0,0,159);
   }
   SHR_BITCOPY_RANGE(tcam_key_val,160,regs_val,0,160);

exit:
   SOCDNX_FUNC_RETURN;
}


int get_signal_for_oam_tcam_action_0(int unit, ARAD_PP_DIAG_REG_FIELD *fld, SOC_SAND_IN int core_id, uint32 *regs_val) {
   uint32   res;

   SOCDNX_INIT_FUNC_DEFS;

   if (SOC_IS_QAX(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,31,0,98,51,0,47);
   } else if (SOC_IS_JERICHO(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,29,0,98,51,0,47);
   } else {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID,16,0,82,43,0,39);
   }

exit:
   SOCDNX_FUNC_RETURN;
}


int get_signal_for_oam_tcam_action_match_0(int unit, ARAD_PP_DIAG_REG_FIELD *fld, SOC_SAND_IN int core_id, uint32 *regs_val) {
   uint32   res;

   SOCDNX_INIT_FUNC_DEFS;

   if (SOC_IS_QAX(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,31,0,99,99,0,0);
   } else if (SOC_IS_JERICHO(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,29,0,99,99,0,0);
   } else {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID,16,0,83,83,0,0);
   }

exit:
   SOCDNX_FUNC_RETURN;
}


int get_signal_for_oam_tcam_action_1(int unit, ARAD_PP_DIAG_REG_FIELD *fld, SOC_SAND_IN int core_id, uint32 *regs_val) {
   uint32   res;

   SOCDNX_INIT_FUNC_DEFS;

   if (SOC_IS_QAX(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,31,0,48,1,0,47);
   } else if (SOC_IS_JERICHO(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,29,0,48,1,0,47);
   } else {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID,16,0,40,1,0,39);
   }

exit:
   SOCDNX_FUNC_RETURN;
}


int get_signal_for_oam_tcam_action_match_1(int unit, ARAD_PP_DIAG_REG_FIELD *fld, SOC_SAND_IN int core_id, uint32 *regs_val) {
   uint32   res;

   SOCDNX_INIT_FUNC_DEFS;

   if (SOC_IS_QAX(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,31,0,49,49,0,0);
   } else if (SOC_IS_JERICHO(unit)) {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHP_ID,29,0,49,49,0,0);
   } else {
       ARAD_PP_OAM_DIAG_FLD_READ(fld, core_id, ARAD_IHB_ID,16,0,41,41,0,0);
   }

exit:
   SOCDNX_FUNC_RETURN;
}

void sub_type_msg_from_code(int unit, char **str, uint32 code) {
   switch (code) {
   case (0):
      *str = "none"; break;
   case (1):
      *str = "LM"; break;
   case (2):
      *str = "DM 1588"; break;
   case (3):
      *str = "DM NTP"; break;
   case (4):
      *str = "CCM"; break;
   case (5):
      *str = "LB"; break;
   default:
      *str = "";
      LOG_ERROR(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "Illegal subtype retreived\n")));
      break;
   }
}


int get_ccm_entry(int unit, uint32 entry_index, soc_reg_above_64_val_t *reg_data, soc_mem_t *entry_type) {
   uint32   ccm_type;
   uint32   res;

   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   GET_TYPE_OF_OAMP_MEP_DB_ENTRY(ccm_type, entry_index, *reg_data);

   
   if (SOC_IS_JERICHO(unit) && ccm_type == 7)
   {
      ccm_type = SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE;
   }   

   switch (ccm_type) {
   case SOC_PPC_OAM_MEP_TYPE_ETH_OAM:
      *entry_type = OAMP_MEP_DBm;
      break;
   case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP:
   case SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS:
      *entry_type = OAMP_MEP_DB_Y_1731_ON_MPLSTPm;
      break;
   case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE:
   case SOC_PPC_OAM_MEP_TYPE_Y1711_PWE:
      *entry_type = OAMP_MEP_DB_Y_1731_ON_PWEm;
      break;
   case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL:
      *entry_type = OAMP_MEP_DB_Y_1731_ON_MPLSTPm;
      break;
   case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION:
      *entry_type = OAMP_MEP_DB_Y_1731_ON_PWEm;
      break;
      case SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE:
      *entry_type = OAMP_MEP_DB_RFC_6374_ON_MPLSTPm;
      break;
   default:
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Unsopported MEP DB entry type."))); break;
   }

   SOC_REG_ABOVE_64_CLEAR(*reg_data);
   SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, entry_index, *reg_data));
exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in get_ccm_entry()", 0, 0);
}




soc_error_t _arad_pp_oam_trap_code_to_internal(int unit, uint32 trap_code, uint32 *internal_trap_code) {
    ARAD_SOC_REG_FIELD                          strength_fld_fwd;                                                               
    ARAD_SOC_REG_FIELD                          strength_fld_snp;     
    SOC_PPC_TRAP_CODE_INTERNAL   trap_code_enum;                                                          
    uint32 soc_sand_rv;

    SOCDNX_INIT_FUNC_DEFS; 

    soc_sand_rv = arad_pp_trap_mgmt_trap_code_to_internal(unit, trap_code, &trap_code_enum, &strength_fld_fwd, &strength_fld_snp);  
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    *internal_trap_code = trap_code_enum;
exit:
    SOCDNX_FUNC_RETURN;
}


static dma_event_handler_cb_t dma_event_handler_cb[SOC_MAX_NUM_DEVICES];  


static SOC_PPC_OAM_REPORT_MODE rx_report_event_mode[SOC_MAX_NUM_DEVICES]; 

 
static int last_dma_interrupt_message_num[SOC_MAX_NUM_DEVICES] ; 
static uint32    *dma_host_memory[SOC_MAX_NUM_DEVICES] ;  
static int arad_pp_num_entries_in_dma_host_memory[SOC_MAX_NUM_DEVICES] ; 



 
static int last_report_dma_interrupt_message_num[SOC_MAX_NUM_DEVICES] ; 
static uint32    *report_dma_host_memory[SOC_MAX_NUM_DEVICES] ;  
static int arad_pp_num_entries_in_report_dma_host_memory[SOC_MAX_NUM_DEVICES] ; 








int _insert_pph(soc_reg_above_64_val_t pkt_header_reg,const PPH_base * pph_fields, int offset);
int _insert_itmh_ing_dest_info_extension(soc_reg_above_64_val_t pkt_header_reg,int fwd_dst_info);
int _insert_itmh(int unit, soc_reg_above_64_val_t pkt_header_reg, int fwd_dst_info , int fwd_dp, int fwd_traffic_class, int snoop_cmd,int  in_mirr_flag,int pph_type) ;
int _insert_ptch2(soc_reg_above_64_val_t pkt_header_reg, int next_header_is_itmh, int opaque_value, int pp_ssp);
int _insert_fhei(soc_reg_above_64_val_t pkt_header_reg, int cpu_trap_code_qualifier, int cpu_trap_code, int offset);
int _insert_udh(soc_reg_above_64_val_t pkt_header_reg, int udg_size);

uint32 arad_pp_oam_tcam_bfd_lsp_ipv4_entry_add_unsafe(SOC_SAND_IN int);

uint32 arad_pp_oam_tcam_bfd_lsp_special_ttl_ipv4_entry_add_unsafe(SOC_SAND_IN int);



uint32
  SOC_PPC_OAM_MEP_PROFILE_DATA_verify(
    SOC_SAND_IN  SOC_PPC_OAM_MEP_PROFILE_DATA *profile_data
  )
{
  uint32 i;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(profile_data);

  for (i=0; i<SOC_PPC_OAM_OPCODE_MAP_COUNT; i++) {
      SOC_SAND_ERR_IF_ABOVE_MAX(profile_data->opcode_to_trap_code_unicast_map[i], SOC_PPC_NOF_TRAP_CODES-1, ARAD_PP_OAM_PROFILE_OPCODE_MAP_ERROR, 10, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(profile_data->opcode_to_trap_code_multicast_map[i], SOC_PPC_NOF_TRAP_CODES-1, ARAD_PP_OAM_PROFILE_OPCODE_MAP_ERROR, 10, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(profile_data);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_OAM_MEP_PROFILE_DATA_verify()",0,0);
}

uint32
  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(classifier_mep_entry);

  SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->md_level, _ARAD_PP_OAM_MAX_MD_LEVEL, ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->mep_type, SOC_PPC_OAM_MEP_TYPE_COUNT, ARAD_PP_OAM_MEP_TYPE_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->port, (ARAD_PORT_NOF_PP_PORTS-1), ARAD_PP_OAM_PORT_NDX_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->acc_profile, SOC_PPC_OAM_ACC_PROFILES_NUM, ARAD_PP_OAM_ACC_PROFILE_OUT_OF_RANGE_ERR, 70, exit);
  if (classifier_mep_entry->mep_type != SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP && classifier_mep_entry->mep_type !=SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS &&
      classifier_mep_entry->mep_type != SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP  && classifier_mep_entry->mep_type != SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE_GAL  &&
      classifier_mep_entry->mep_type !=SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP  && classifier_mep_entry->mep_type !=SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE  ) { 
      SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->ma_index, SOC_PPC_OAM_MAX_NUMBER_OF_MAS(unit), ARAD_PP_OAM_MA_INDEX_OUT_OF_RANGE_ERR, 80, exit); 
  }
  SOC_SAND_ERR_IF_ABOVE_NOF(classifier_mep_entry->counter, SOC_DPP_DEFS_GET(unit, nof_counters), ARAD_COUNTER_NDX_OUT_OF_RANGE_ERR, 90, exit);
  if (_ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type)) {
      SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->your_discriminator, _ARAD_PP_OAM_BFD_YOUR_DISC_MAX, ARAD_PP_OAM_YOUR_DISC_ABOVE_MAX_ERROR, 100, exit);
  }
  else {
      
      if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
           ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
          SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->lif, OAM_LIF_MAX_VALUE(unit), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 110, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_verify()",0,0);
}

uint32
  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_verify(
    SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES *tx_mpls_att
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(tx_mpls_att);

  SOC_SAND_ERR_IF_ABOVE_MAX(tx_mpls_att->dp, _ARAD_PP_OAM_DP_MAX, ARAD_PP_OAM_DP_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tx_mpls_att->tc, _ARAD_PP_OAM_TC_MAX, ARAD_PP_OAM_TC_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_MAGIC_NUM_VERIFY(tx_mpls_att);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_verify()",0,0);
}

uint32
  SOC_PPC_BFD_PDU_STATIC_REGISTER_verify(
    SOC_SAND_IN  SOC_PPC_BFD_PDU_STATIC_REGISTER *bfd_pdu
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(bfd_pdu);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_pdu->bfd_vers, _ARAD_PP_OAM_BFD_VERS_MAX, ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_pdu->bfd_diag, _ARAD_PP_OAM_BFD_DIAG_MAX, ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_pdu->bfd_sta, _ARAD_PP_OAM_BFD_STA_MAX, ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR, 30, exit);
  SOC_SAND_MAGIC_NUM_VERIFY(bfd_pdu);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_BFD_PDU_STATIC_REGISTER_verify()",0,0);
}

uint32
  SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER_verify(
    SOC_SAND_IN  SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER *bfd_cc_packet
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(bfd_cc_packet);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_cc_packet->bfd_static_reg_fields.bfd_vers, _ARAD_PP_OAM_BFD_VERS_MAX, ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_cc_packet->bfd_static_reg_fields.bfd_diag, _ARAD_PP_OAM_BFD_DIAG_MAX, ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_cc_packet->bfd_static_reg_fields.bfd_sta, _ARAD_PP_OAM_BFD_STA_MAX, ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR, 30, exit);
  SOC_SAND_MAGIC_NUM_VERIFY(bfd_cc_packet);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_BFD_PDU_STATIC_REGISTER_verify()",0,0);
}

uint32
  SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA_verify(
    SOC_SAND_IN  SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA *tos_ttl_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(tos_ttl_data);
  
  SOC_SAND_MAGIC_NUM_VERIFY(tos_ttl_data);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA_verify()",0,0);
}

uint32
  SOC_PPC_MPLS_PWE_PROFILE_DATA_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_PWE_PROFILE_DATA *mpls_pwe_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(mpls_pwe_profile);
  SOC_SAND_ERR_IF_ABOVE_MAX(mpls_pwe_profile->exp, _ARAD_PP_OAM_EXP_MAX, ARAD_PP_OAM_EXP_OUT_OF_RANGE_ERR, 10, exit);
  

  SOC_SAND_MAGIC_NUM_VERIFY(mpls_pwe_profile);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MPLS_PWE_PROFILE_DATA_verify()",0,0);
}


void _arad_pp_oam_ma_name_divide(
    SOC_SAND_IN   SOC_PPC_OAM_MA_NAME      name, 
    SOC_SAND_IN   uint8                    is_ma_short,
    SOC_SAND_OUT  soc_reg_above_64_val_t   umc_name,
    SOC_SAND_OUT  SOC_PPC_OAM_ICC_MAP_DATA *icc_map_data, 
    SOC_SAND_OUT  uint32                   *short_name
    )
{
    COMPILER_64_ZERO(*icc_map_data);

    if (is_ma_short) {
        SOC_REG_ABOVE_64_CLEAR(umc_name);
        *short_name = name[1] + (name[0] << 8);
    }
    else
    {
        SOC_REG_ABOVE_64_CLEAR(umc_name);
        umc_name[0]=name[12]+(name[11]<<8)+(name[10]<<16)+(name[9]<<24);
        umc_name[1]=name[8];
        *short_name = name[7] + (name[6] << 8);
        SOC_PPC_OAM_GROUP_NAME_TO_ICC_MAP_DATA(name, *icc_map_data);
    }
}

STATIC
  uint32
    arad_pp_oam_tcam_callback(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32  user_data
    )
{
  uint32
    res = SOC_SAND_OK,
    tcam_db_id,
    access_profile_id_0,
    access_profile_id_1,
    flp_program;
  uint8  prog_index;
  uint32 small_banks_used = (SOC_DPP_CONFIG(unit)->pp.use_small_banks_mode_vrrp == 1) ||
                            (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "use_small_banks_mode_vrrp",0) == 1) ||
                            (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "use_small_banks_mode_efp",0) == 1)  ||
                            (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "use_small_banks_mode_null",0) == 1);  

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  tcam_db_id = user_data;

  if (!small_banks_used)
  {
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(
          unit,
          tcam_db_id,
          0, 
          &access_profile_id_0
        );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  }
  else
  {
      access_profile_id_0=0x3F;
  }


  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(
      unit,
      tcam_db_id,
      1, 
      &access_profile_id_1
    );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);

  
  if (SOC_IS_JERICHO(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0)) {
      flp_program = 0;
  } else {
      flp_program = 1;
  }

  for (; flp_program <= PROG_FLP_LAST; flp_program ++) {
      int indx=0, found=0;
      
      for (indx = 0; indx < SOC_DPP_DEFS_GET(unit, nof_flp_programs); ++indx) {
          uint8 prog_usage;
          res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, indx, &prog_usage);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
          if (flp_program == prog_usage) {
              found = 1;
              prog_index = indx;
              break;
          }
      }
      if (found==1) {
          res = arad_pp_flp_lookups_oam(
                unit,
                access_profile_id_0,
                access_profile_id_1,
                prog_index 
          );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_tcam_callback()", 0, 0);
}


uint32 
  arad_pp_oam_tcam_database_create(
       SOC_SAND_IN int unit
  ) 
{
  ARAD_TCAM_ACCESS_INFO tcam_access_info;
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SUCCESS_FAILURE success;
  uint32 tcam_last_entry_id;

  uint32 small_banks_used = (SOC_DPP_CONFIG(unit)->pp.use_small_banks_mode_vrrp == 1) ||
                            (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "use_small_banks_mode_vrrp",0) == 1) ||
                            (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "use_small_banks_mode_efp",0) == 1)  ||
                            (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "use_small_banks_mode_null",0) == 1);

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  arad_ARAD_TCAM_ACCESS_INFO_clear(&tcam_access_info);

  tcam_last_entry_id = 0;
  res = sw_state_access[unit].dpp.soc.arad.pp.oam.tcam_last_entry_id.set(unit, tcam_last_entry_id);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  tcam_access_info.action_bitmap_ndx                   = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS | ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS |
                                                         ARAD_TCAM_ACTION_SIZE_THIRD_20_BITS | ARAD_TCAM_ACTION_SIZE_FORTH_20_BITS; 
  tcam_access_info.bank_owner                          = ARAD_TCAM_BANK_OWNER_FLP_TRAPS;    
  tcam_access_info.callback                            = arad_pp_oam_tcam_callback;
  tcam_access_info.entry_size                          = small_banks_used ? ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS : ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS;
  tcam_access_info.is_direct                           = FALSE; 
  tcam_access_info.min_banks                           = _ARAD_PP_OAM_IDENTIFICATION_TCAM_MIN_BANKS; 
  tcam_access_info.prefix_size                         = _ARAD_PP_OAM_IDENTIFICATION_PREFIX_SIZE; 
  tcam_access_info.user_data                           = ARAD_PP_TCAM_STATIC_ACCESS_ID_OAM_IDENTIFICATION;
  tcam_access_info.use_small_banks                     = ARAD_TCAM_SMALL_BANKS_FORCE;

  res = arad_tcam_access_create_unsafe(
            unit,
            ARAD_PP_TCAM_STATIC_ACCESS_ID_OAM_IDENTIFICATION,
            &tcam_access_info,
            &success
  );

  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  if(success != SOC_SAND_SUCCESS) {
      LOG_ERROR(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "   Error in OAM database create. \n\r")));
      LOG_ERROR(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "   Parameters: Prefix size %d, entry size %s, bank owner (0-IngPMF, 3-EgrPMF) %d, Action bitmap %d \n\r"), tcam_access_info.prefix_size, 
                 SOC_TMC_TCAM_BANK_ENTRY_SIZE_to_string(tcam_access_info.entry_size), tcam_access_info.bank_owner, tcam_access_info.action_bitmap_ndx));
      LOG_ERROR(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "\n\r")));
        res = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_tcam_database_create()", 0, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_oam_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_oam;
}


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_oam_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_oam;
}





uint32
  arad_pp_oam_classifier_oam1_allocate_sw_buffer(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t        *oama_buffer
  )
{
    uint32  res = 0;
    int mem_size;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    oama_buffer->use_dma = 
#ifdef PLISIM
0;
#else 
         soc_mem_dmaable(unit,IHP_OAMAm, SOC_MEM_BLOCK_ANY(unit, IHP_OAMAm));
#endif
    mem_size =  SOC_MEM_TABLE_BYTES(unit, IHP_OAMAm);

    oama_buffer->is_allocated=0;
    oama_buffer->buffer=NULL;
    if (oama_buffer->use_dma) {
        oama_buffer->buffer = soc_cm_salloc(unit,mem_size, "IHB_OAMA buffer" );
    } else {
        ARAD_ALLOC_ANY_SIZE(oama_buffer->buffer, uint32, mem_size, "IHB_OAMA buffer");
    }

    if (oama_buffer->buffer == NULL) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 10, exit);
    }

    res = soc_mem_read_range(unit, IHP_OAMAm, MEM_BLOCK_ANY, 0, soc_mem_index_max(unit, IHP_OAMAm), oama_buffer->buffer);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, res); 

    oama_buffer->is_allocated = 1;


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_classifier_oam1_entry_set_on_buffer(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY      *oam1_key,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD  *oam_payload,
    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t                           *oama_buffer
  )
{
  uint32  key = 0;
  uint32  field_val = 0;
  uint32* where_to_write; 
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRY_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oam1_key);
  SOC_SAND_CHECK_NULL_INPUT(oam_payload);
  SOC_SAND_CHECK_NULL_INPUT(oama_buffer);

  _arad_pp_oam_oam1_key_struct_to_key(unit,(*oam1_key), &key);
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " Inserting entry to OAM1: key: %d\n\r"), key));

  where_to_write = oama_buffer->buffer + key * (soc_mem_entry_words(unit, IHP_OAMAm)  );

  if (oam1_key->ingress) {
      field_val = (uint32)oam_payload->snoop_strength;
      soc_IHP_OAMAm_field_set(unit, where_to_write, SNOOP_STRENGTHf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " snoop_strength: %d\n\r"), oam_payload->snoop_strength));
      field_val = oam_payload->cpu_trap_code;
      soc_IHP_OAMAm_field_set(unit, where_to_write, CPU_TRAP_CODEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " cpu_trap_code: %d\n\r"), oam_payload->cpu_trap_code));
      field_val = (uint32)oam_payload->forwarding_strength;
      soc_IHP_OAMAm_field_set(unit, where_to_write, FORWARDING_STRENGTHf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " forwarding_strength: %d\n\r"), oam_payload->forwarding_strength));
      field_val = oam_payload->up_map;
      soc_IHP_OAMAm_field_set(unit, where_to_write, UP_MEPf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " up_map: %d\n\r"), oam_payload->up_map));
      field_val = (uint32)oam_payload->meter_disable;
      soc_IHP_OAMAm_field_set(unit, where_to_write, METER_DISABLEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " meter_disable: %d\n\r"), oam_payload->meter_disable));
  }
  else {
      if (SOC_IS_JERICHO(unit)) {
          field_val = !oam_payload->forward_disable;
          soc_IHP_OAMAm_field_set(unit, where_to_write, FORWARD_ENABLEf, &field_val);

          
          field_val =oam_payload->mirror_enable; 
          soc_IHP_OAMAm_field_set(unit,where_to_write, MIRROR_ENABLEf, &field_val);

          field_val =  oam_payload->mirror_profile ? _JER_PP_OAM_MIRROR_STRENGTH : 0;
          soc_IHP_OAMAm_field_set(unit, where_to_write, MIRROR_STRENGTHf, &field_val);

          field_val = oam_payload->mirror_profile ? _JER_PP_OAM_FORWARD_STRENGTH : 0;
          soc_IHP_OAMAm_field_set(unit, where_to_write, FWD_STRENGTHf, &field_val);


      } else {
          field_val = (uint32)oam_payload->forward_disable;
          soc_IHP_OAMAm_field_set(unit, where_to_write, FORWARD_DISABLEf, &field_val);
      }
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " forward_disable: %d\n\r"), oam_payload->forward_disable));
      field_val = (uint32)oam_payload->mirror_profile;
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " mirror_profile: %d\n\r"), oam_payload->mirror_profile));
      soc_IHP_OAMAm_field_set(unit, where_to_write, MIRROR_COMMANDf, &field_val);
  }

  if (SOC_IS_QUX(unit) && oam_payload->counter_disable == 1) {
      
      if ((oam_payload->sub_type == _ARAD_PP_OAM_SUBTYPE_DEFAULT_OAM_MESSAGE) &&
          (oam1_key->opcode != ARAD_PP_OAM_OPCODE_CCM)) {
          field_val = 6;
      } else {
          field_val = (uint32)oam_payload->sub_type;
      }
      soc_IHP_OAMAm_field_set(unit, where_to_write, SUB_TYPEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          " sub_type: %d\n\r"), oam_payload->sub_type));
  } else {
      field_val = (uint32)oam_payload->sub_type;
      soc_IHP_OAMAm_field_set(unit, where_to_write, SUB_TYPEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          " sub_type: %d\n\r"), oam_payload->sub_type));
      field_val = (uint32)oam_payload->counter_disable;
      soc_IHP_OAMAm_field_set(unit, where_to_write, COUNTER_DISABLEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          " counter_disable: %d\n\r"), oam_payload->counter_disable));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_entry_set_on_buffer()", 0, 0);
}



uint32
  arad_pp_oam_classifier_oam1_set_hw_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t          *oama_buffer
  )
{
    uint32  res = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = soc_mem_write_range(unit, IHP_OAMAm, MEM_BLOCK_ANY, 0, soc_mem_index_max(unit, IHP_OAMAm), oama_buffer->buffer);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, res); 


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));

}



void arad_pp_oam_classifier_oam1_2_buffer_free(
   SOC_SAND_IN int                                           unit, 
   SOC_SAND_INOUT _oam_oam_a_b_table_buffer_t * buffer
   )
{
    if (buffer->is_allocated) {
        if (buffer->use_dma) {
            soc_cm_sfree(unit, buffer->buffer);
        } else {
            ARAD_FREE(buffer->buffer);
        }

    }
}






uint32
  arad_pp_oam_classifier_oam2_allocate_sw_buffer(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t           *oamb_buffer
  )
{
    uint32  res = 0;
    int mem_size;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    oamb_buffer->use_dma =
#ifdef PLISIM
0;
#else 
         soc_mem_dmaable(unit,IHP_OAMBm, SOC_MEM_BLOCK_ANY(unit, IHP_OAMBm));
#endif
    mem_size =  SOC_MEM_TABLE_BYTES(unit, IHP_OAMBm);

    oamb_buffer->buffer=NULL;
    oamb_buffer->is_allocated=0;
    if (oamb_buffer->use_dma) {
        oamb_buffer->buffer = soc_cm_salloc(unit,mem_size, "IHB_OAMB buffer" );
    } else {
        ARAD_ALLOC_ANY_SIZE(oamb_buffer->buffer, uint32, mem_size, "IHB_OAMB buffer");
    }

    if (oamb_buffer->buffer == NULL) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 10, exit);
    }

    res = soc_mem_read_range(unit, IHP_OAMBm, MEM_BLOCK_ANY, 0,soc_mem_index_max(unit, IHP_OAMBm), oamb_buffer->buffer);
    SOC_SAND_SOC_IF_ERROR_RETURN(res,10,exit,res); 

    oamb_buffer->is_allocated = 1; 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));

}



uint32
  arad_pp_oam_classifier_oam2_entry_set_on_buffer(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY      *oam2_key,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD  *oam2_payload,
    SOC_SAND_INOUT _oam_oam_a_b_table_buffer_t                      *oamb_buffer
  )
{
  uint32 * where_to_write; 
  uint32  key;
  uint32  field_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM2_ENTRY_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oam2_key);
  SOC_SAND_CHECK_NULL_INPUT(oam2_payload);
  SOC_SAND_CHECK_NULL_INPUT(oamb_buffer);

  _ARAD_PP_OAM_OAM2_KEY_STRUCT_TO_KEY((*oam2_key), key);
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " Inserting entry to OAM2: key:%d\n\r"), key));
  where_to_write = oamb_buffer->buffer + key * (soc_mem_entry_words(unit, IHP_OAMBm)  );

  if (oam2_key->ingress) {
      field_val = (uint32)oam2_payload->snoop_strength;
      soc_IHP_OAMBm_field_set(unit, where_to_write, SNOOP_STRENGTHf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " snoop_strength: %d\n\r"), oam2_payload->snoop_strength));
      field_val = (uint32)oam2_payload->cpu_trap_code;
      soc_IHP_OAMBm_field_set(unit, where_to_write, CPU_TRAP_CODEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " cpu_trap_code: %d\n\r"), oam2_payload->cpu_trap_code));
      field_val = (uint32)oam2_payload->forwarding_strength;
      soc_IHP_OAMBm_field_set(unit, where_to_write, FORWARDING_STRENGTHf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
          (BSL_META_U(unit,
                      " forwarding_strength: %d\n\r"), oam2_payload->forwarding_strength));
      field_val = (uint32)oam2_payload->up_map;
      soc_IHP_OAMBm_field_set(unit,where_to_write, UP_MEPf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " up_map: %d\n\r"), oam2_payload->up_map));
      field_val = (uint32)oam2_payload->meter_disable;
      soc_IHP_OAMBm_field_set(unit, where_to_write, METER_DISABLEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " meter_disable: %d\n\r"), oam2_payload->meter_disable));
  }
  else {
      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          field_val = oam2_payload->forward_disable;
          soc_IHP_OAMBm_field_set(unit, where_to_write, FORWARD_DISABLEf, &field_val);
      } else {
          field_val = !oam2_payload->forward_disable;
          soc_IHP_OAMBm_field_set(unit, where_to_write, FORWARD_ENABLEf, &field_val);

          
          field_val = 1; 
          soc_IHP_OAMBm_field_set(unit, where_to_write, MIRROR_ENABLEf, &field_val);

          field_val =  _JER_PP_OAM_MIRROR_STRENGTH;
          soc_IHP_OAMBm_field_set(unit, where_to_write, MIRROR_STRENGTHf, &field_val); 

          field_val =  _JER_PP_OAM_FORWARD_STRENGTH;
          soc_IHP_OAMBm_field_set(unit, where_to_write, FWD_STRENGTHf, &field_val);

      }
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " forward_disable: %d\n\r"), oam2_payload->forward_disable));
      field_val = (uint32)oam2_payload->mirror_profile;
      soc_IHP_OAMBm_field_set(unit, where_to_write, MIRROR_COMMANDf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " mirror_profile: %d\n\r"), oam2_payload->mirror_profile));
  }

  if (SOC_IS_QUX(unit) && oam2_payload->counter_disable == 1) {
      
      if ((oam2_payload->sub_type == _ARAD_PP_OAM_SUBTYPE_DEFAULT_OAM_MESSAGE) &&
          (oam2_key->opcode != ARAD_PP_OAM_OPCODE_CCM)) {
          field_val = 6;
      } else {
          field_val = (uint32)oam2_payload->sub_type;
      }
      soc_IHP_OAMBm_field_set(unit, where_to_write, SUB_TYPEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          " sub_type: %d\n\r"), oam2_payload->sub_type));
  } else {
      field_val = (uint32)oam2_payload->sub_type;
      soc_IHP_OAMBm_field_set(unit, where_to_write, SUB_TYPEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          " sub_type: %d\n\r"), oam2_payload->sub_type));
      field_val = (uint32)oam2_payload->counter_disable;
      soc_IHP_OAMBm_field_set(unit,where_to_write, COUNTER_DISABLEf, &field_val);
      LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          " counter_disable: %d\n\r"), oam2_payload->counter_disable));
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam2_entry_set_on_buffer()", 0, 0);
}



uint32
  arad_pp_oam_classifier_oam2_set_hw_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t                           *oamb_buffer
  )
{
    uint32  res = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = soc_mem_write_range(unit, IHP_OAMBm, MEM_BLOCK_ANY, 0, soc_mem_index_max(unit, IHP_OAMBm), oamb_buffer->buffer);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, res); 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_classifier_oam1_entry_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY      *oam1_key,
    SOC_SAND_OUT  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD  *oam_payload
  )
{
  uint32  res;
  uint32  entry;
  uint32  key;
  uint32  field_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRY_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oam1_key);
  SOC_SAND_CHECK_NULL_INPUT(oam_payload);

  _arad_pp_oam_oam1_key_struct_to_key(unit, (*oam1_key), &key);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_OAMAm(unit, MEM_BLOCK_ANY, key, &entry));
  if (oam1_key->ingress) {
      field_val = 0;
      soc_IHP_OAMAm_field_get(unit, &entry, SNOOP_STRENGTHf, &field_val);
      oam_payload->snoop_strength = (uint8)field_val;
      soc_IHP_OAMAm_field_get(unit, &entry, CPU_TRAP_CODEf, &oam_payload->cpu_trap_code);
      field_val = 0;
      soc_IHP_OAMAm_field_get(unit, &entry, FORWARDING_STRENGTHf, &field_val);
      oam_payload->forwarding_strength = (uint8)field_val;
      field_val = 0;
      soc_IHP_OAMAm_field_get(unit, &entry, UP_MEPf, &field_val);
      oam_payload->up_map = (uint8)field_val;
      field_val = 0;
      soc_IHP_OAMAm_field_get(unit, &entry, METER_DISABLEf, &field_val);
      oam_payload->meter_disable = (uint8)field_val;
  }
  else {
      field_val = 0;
      if (SOC_IS_JERICHO(unit)) {
          soc_IHP_OAMAm_field_get(unit, &entry, MIRROR_STRENGTHf, &field_val);
          oam_payload->mirror_strength = field_val;
          field_val=0;

          soc_IHP_OAMAm_field_get(unit, &entry, MIRROR_ENABLEf, &field_val);
          oam_payload->mirror_enable = field_val;
          field_val=0;

          soc_IHP_OAMAm_field_get(unit, &entry, FORWARD_ENABLEf, &field_val);
          field_val = !field_val;
      } else {
          soc_IHP_OAMAm_field_get(unit, &entry, FORWARD_DISABLEf, &field_val);
      }
      oam_payload->forward_disable = (uint8)field_val;
      field_val = 0;
      soc_IHP_OAMAm_field_get(unit, &entry, MIRROR_COMMANDf, &field_val);
      oam_payload->mirror_profile = (uint8)field_val;
  }
  field_val = 0;
  soc_IHP_OAMAm_field_get(unit, &entry, SUB_TYPEf, &field_val);
  oam_payload->sub_type = (uint8)field_val;
  field_val = 0;
  soc_IHP_OAMAm_field_get(unit, &entry, COUNTER_DISABLEf, &field_val);
  oam_payload->counter_disable = (uint8)field_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_entry_get_unsafe()", 0, 0);
}


uint32
  arad_pp_oam_classifier_oam2_entry_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY      *oam2_key,
    SOC_SAND_OUT  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD  *oam2_payload
  )
{
    uint32  res;
    uint32  entry;
    uint32  key;
    uint32  field_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM2_ENTRY_GET_UNSAFE);
    SOC_SAND_CHECK_NULL_INPUT(oam2_key);
    SOC_SAND_CHECK_NULL_INPUT(oam2_payload);

    _ARAD_PP_OAM_OAM2_KEY_STRUCT_TO_KEY((*oam2_key), key);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_OAMBm(unit, MEM_BLOCK_ANY, key, &entry));
    if (oam2_key->ingress) {
        field_val = 0;
        soc_IHP_OAMBm_field_get(unit, &entry, SNOOP_STRENGTHf, &field_val);
        oam2_payload->snoop_strength = (uint8)field_val;
        soc_IHP_OAMBm_field_get(unit, &entry, CPU_TRAP_CODEf, &oam2_payload->cpu_trap_code);
        field_val = 0;
        soc_IHP_OAMBm_field_get(unit, &entry, FORWARDING_STRENGTHf, &field_val);
        oam2_payload->forwarding_strength = (uint8)field_val;
        field_val = 0;
        soc_IHP_OAMBm_field_get(unit, &entry, UP_MEPf, &field_val);
        oam2_payload->up_map = (uint8)field_val;
        field_val = 0;
        soc_IHP_OAMBm_field_get(unit, &entry, METER_DISABLEf, &field_val);
        oam2_payload->meter_disable = (uint8)field_val;
    }
    else {
        field_val = 0;
        soc_IHP_OAMBm_field_get(unit, &entry, FORWARD_DISABLEf, &field_val);
        oam2_payload->forward_disable = (uint8)field_val;
        field_val = 0;
        soc_IHP_OAMBm_field_get(unit, &entry, MIRROR_COMMANDf, &field_val);
        oam2_payload->mirror_profile = (uint8)field_val;
    }
    field_val = 0;
    soc_IHP_OAMBm_field_get(unit, &entry, SUB_TYPEf, &field_val);
    oam2_payload->sub_type = (uint8)field_val;
    field_val = 0;
    soc_IHP_OAMBm_field_get(unit, &entry, COUNTER_DISABLEf, &field_val);
    oam2_payload->counter_disable = (uint8)field_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam2_entry_get_unsafe()", 0, 0);
}

    

STATIC
  void
    jer_oam_event_fifo_thread(void *param)
{
    int             unit = PTR_TO_INT(param);
    int             channel_number = -1;
    int             ch = 0;
    int             cmc = 0;
    uint32          ret;
    char            thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t    thread;
    uint32 reg;
    uint32 overflow;
    uint32 timeout;
    int interrupt_condition = 0;
    int event_type = SOC_PPC_OAM_DMA_EVENT_TYPE_EVENT;
    sal_usecs_t start_time;
    int time_diff;
    int evict_interval = 1000;

    SOCDNX_INIT_FUNC_DEFS;

    bsl_printf("%s - Entered, unit=%d \n", __func__, unit);

    
    evict_interval = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_event_fifo_thread_evict_interval", 1000);

    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof (thread_name));

    ret = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_get, (unit, dma_fifo_channel_src_oam_event, &channel_number));
    if ((ret != BCM_E_NONE) ||(channel_number == -1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("%s - Cannot get FIFO Channel Number, unit=%d - rc=%d \n"), __func__, unit, ret));
    }
    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        cmc = channel_number / NOF_DMA_FIFO_PER_CMC;
        ch = channel_number % NOF_DMA_FIFO_PER_CMC;
    }

    while (jer_oam_event_fifo_terminate[unit] == 0)
    {
        (void) sal_sem_take(jer_oam_event_fifo_sem[unit], sal_sem_FOREVER);

        start_time = sal_time_usecs();
        interrupt_condition = 1;
        while ((jer_oam_event_fifo_terminate[unit] == 0) && interrupt_condition) {
            reg = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));
            timeout = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, reg, HOSTMEM_TIMEOUTf);
            overflow = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, reg, HOSTMEM_OVERFLOWf);
            if (timeout || overflow) {
                soc_ppd_oam_dma_event_handler(INT_TO_PTR(unit), INT_TO_PTR(event_type), INT_TO_PTR(cmc), INT_TO_PTR(ch), 0);

                time_diff = SAL_USECS_SUB(sal_time_usecs(), start_time);
                if (time_diff > evict_interval) {
                    
                    sal_thread_yield();

                    start_time = sal_time_usecs();
                }
            }
            else {
                interrupt_condition = 0;
            }
        }
    }

exit:
    sal_sem_destroy(jer_oam_event_fifo_sem[unit]);
    jer_oam_event_fifo_sem[unit] = NULL;
    jer_oam_event_fifo_terminate[unit] = 0;
    jer_oam_event_fifo_tid[unit] = SAL_THREAD_ERROR;
    bsl_printf("%s - Thread Exiting ... \n", __func__);
    sal_thread_exit(0);
    SOCDNX_FUNC_RETURN_VOID; 

    return;
}

STATIC
  uint32
    jer_oam_event_fifo_cleanup(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    bsl_printf("%s - Entered, unit=%d \n", __func__, unit);
    
    if ((jer_oam_event_fifo_tid[unit] != NULL) && (jer_oam_event_fifo_tid[unit] != SAL_THREAD_ERROR)) {
        jer_oam_event_fifo_terminate[unit] = 1;
        sal_sem_give(jer_oam_event_fifo_sem[unit]);
    } else {
       if (jer_oam_event_fifo_sem[unit]) {
             sal_sem_destroy(jer_oam_event_fifo_sem[unit]);
            jer_oam_event_fifo_sem[unit] = NULL;
       }
    }

    SOCDNX_FUNC_RETURN;
}

STATIC
  uint32
    jer_oam_event_fifo_init(int unit)
{
  char                        thread_name[SAL_THREAD_NAME_MAX_LEN];

  SOCDNX_INIT_FUNC_DEFS;

  sal_snprintf(thread_name, sizeof(thread_name), "OAM_event_fifo_thread.%d", unit);
  jer_oam_event_fifo_sem[unit] = sal_sem_create("OAM_event_fifo_sem", sal_sem_BINARY, 1);

  if(jer_oam_event_fifo_sem[unit] == NULL) {
    jer_oam_event_fifo_cleanup(unit);
    SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Sempahore allocation failure (OAM_fifo_sem.%d"), unit));
  }
  jer_oam_event_fifo_terminate[unit] = 0;

  jer_oam_event_fifo_tid[unit] = sal_thread_create(thread_name,
                                              SAL_THREAD_STKSZ,
                                              50 ,
                                              jer_oam_event_fifo_thread,
                                              INT_TO_PTR(unit));

  if ((jer_oam_event_fifo_tid[unit] == NULL) || (jer_oam_event_fifo_tid[unit] == SAL_THREAD_ERROR)) {
    jer_oam_event_fifo_cleanup(unit);
    SOCDNX_EXIT_WITH_ERR(SOC_SAND_GEN_ERR, (_BSL_SOCDNX_MSG("%s create failed. \n"), thread_name));
  }

  bsl_printf("%s - Task %s Created - %p \n", __func__, thread_name, (void *)jer_oam_event_fifo_tid[unit]);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
  void
    jer_oam_stat_fifo_thread(void *param)
{
    int             unit = PTR_TO_INT(param);
    int             channel_number = -1;
    int             ch = 0;
    int             cmc = 0;
    uint32          ret;
    char            thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t    thread;
    uint32 reg;
    uint32 overflow;
    uint32 timeout;
    int interrupt_condition = 0;
    int event_type = SOC_PPC_OAM_DMA_EVENT_TYPE_STAT_EVENT;
    sal_usecs_t start_time;
    int time_diff;
    int evict_interval = 1000;

    SOCDNX_INIT_FUNC_DEFS;

    bsl_printf("%s - Entered, unit=%d \n", __func__, unit);

    
    evict_interval = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_stat_fifo_thread_evict_interval", 1000);

    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof (thread_name));

    ret = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_get, (unit, dma_fifo_channel_src_oam_status, &channel_number));
    if ((ret != BCM_E_NONE) ||(channel_number == -1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("%s - Cannot get FIFO Channel Number, unit=%d - rc=%d \n"), __func__, unit, ret));
    }
    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        cmc = channel_number / NOF_DMA_FIFO_PER_CMC;
        ch = channel_number % NOF_DMA_FIFO_PER_CMC;
    }

    while (jer_oam_stat_fifo_terminate[unit] == 0)
    {
        (void) sal_sem_take(jer_oam_stat_fifo_sem[unit], sal_sem_FOREVER);

        start_time = sal_time_usecs();
        interrupt_condition = 1;
        while ((jer_oam_stat_fifo_terminate[unit] == 0) && interrupt_condition) {
            reg = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));
            timeout = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, reg, HOSTMEM_TIMEOUTf);
            overflow = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, reg, HOSTMEM_OVERFLOWf);
            if (timeout || overflow) {
                soc_ppd_oam_dma_event_handler(INT_TO_PTR(unit), INT_TO_PTR(event_type), INT_TO_PTR(cmc), INT_TO_PTR(ch), 0);

                time_diff = SAL_USECS_SUB(sal_time_usecs(), start_time);
                if (time_diff > evict_interval) {
                    
                    sal_thread_yield();

                    start_time = sal_time_usecs();
                }
            }
            else {
                interrupt_condition = 0;
            }
        }
    }

exit:
    sal_sem_destroy(jer_oam_stat_fifo_sem[unit]);
    jer_oam_stat_fifo_sem[unit] = NULL;
    jer_oam_stat_fifo_terminate[unit] = 0;
    jer_oam_stat_fifo_tid[unit] = SAL_THREAD_ERROR;
    bsl_printf("%s - Thread Exiting ... \n", __func__);
    sal_thread_exit(0);
    SOCDNX_FUNC_RETURN_VOID; 

    return;
}

STATIC
  uint32
    jer_oam_stat_fifo_cleanup(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    bsl_printf("%s - Entered, unit=%d \n", __func__, unit);
    
    if ((jer_oam_stat_fifo_tid[unit] != NULL) && (jer_oam_stat_fifo_tid[unit] != SAL_THREAD_ERROR)) {
        jer_oam_stat_fifo_terminate[unit] = 1;
        sal_sem_give(jer_oam_stat_fifo_sem[unit]);
    } else {
       if (jer_oam_stat_fifo_sem[unit]) {
             sal_sem_destroy(jer_oam_stat_fifo_sem[unit]);
            jer_oam_stat_fifo_sem[unit] = NULL;
       }
    }

    SOCDNX_FUNC_RETURN;
}

STATIC
  uint32
    jer_oam_stat_fifo_init(int unit)
{
  char                        thread_name[SAL_THREAD_NAME_MAX_LEN];

  SOCDNX_INIT_FUNC_DEFS;

  sal_snprintf(thread_name, sizeof(thread_name), "OAM_stat_fifo_thread.%d", unit);
  jer_oam_stat_fifo_sem[unit] = sal_sem_create("OAM_stat_fifo_sem", sal_sem_BINARY, 1);

  if(jer_oam_stat_fifo_sem[unit] == NULL) {
    jer_oam_stat_fifo_cleanup(unit);
    SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Sempahore allocation failure (OAM_fifo_sem.%d"), unit));
  }
  jer_oam_stat_fifo_terminate[unit] = 0;

  jer_oam_stat_fifo_tid[unit] = sal_thread_create(thread_name,
                                              SAL_THREAD_STKSZ,
                                              50 ,
                                              jer_oam_stat_fifo_thread,
                                              INT_TO_PTR(unit));

  if ((jer_oam_stat_fifo_tid[unit] == NULL) || (jer_oam_stat_fifo_tid[unit] == SAL_THREAD_ERROR)) {
    jer_oam_stat_fifo_cleanup(unit);
    SOCDNX_EXIT_WITH_ERR(SOC_SAND_GEN_ERR, (_BSL_SOCDNX_MSG("%s create failed. \n"), thread_name));
  }

  bsl_printf("%s - Task %s Created - %p \n", __func__, thread_name, (void *)jer_oam_stat_fifo_tid[unit]);

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_oam_dma_set_unsafe(
     SOC_SAND_IN  int                                 unit
     )
{
    uint32 res;
    uint32 reg, dma_threshold;
    uint32 host_mem_num_entries , timeout;
    uint32 host_mem_size_in_bytes;
    int dont, care,endian; 
    int cmc, ch;
    int channel_number = -1;
    int rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    host_mem_size_in_bytes = SOC_DPP_CONFIG(unit)->pp.oam_dma_event_buffer_size;        
    dma_threshold = SOC_DPP_CONFIG(unit)->pp.oam_dma_event_threshold;
    
    timeout = SOC_DPP_CONFIG(unit)->pp.oam_dma_event_timeout;

    
    
    host_mem_num_entries = (host_mem_size_in_bytes+1) / _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA;

    host_mem_num_entries =  (host_mem_num_entries < dma_threshold) ? dma_threshold  :host_mem_num_entries; 

    ROUND_UP_TO_NEAREST_POWER_OF_2(host_mem_num_entries);
    host_mem_num_entries = (host_mem_num_entries <SMALLEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY )? SMALLEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY : host_mem_num_entries;
    host_mem_num_entries = ( host_mem_num_entries >ARAD_PP_OAM_LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY)? ARAD_PP_OAM_LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY: host_mem_num_entries;

    arad_pp_num_entries_in_dma_host_memory[unit] = host_mem_num_entries;

    host_mem_size_in_bytes = host_mem_num_entries * _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA;


    
    if (SOC_IS_JERICHO(unit)) 
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_free_find, (unit, FALSE, &channel_number))); 
        if (channel_number == -1) {
            
            LOG_ERROR(BSL_LS_SOC_OAM,(BSL_META_U(unit, "unit %d Cannot find free DMA-channel."),unit));
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 31, exit);            
        }                  
        SOC_ALLOW_WB_WRITE(unit, jer_mgmt_dma_fifo_channel_set(unit, channel_number, dma_fifo_channel_src_oam_event), rv);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, rv);
    }
    else
    {        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, READ_ECI_GENERAL_CONFIGURATION_2r(unit, &reg));
        soc_reg_field_set(unit, ECI_GENERAL_CONFIGURATION_2r, &reg, FIFO_DMA_3_SELf, 1);    
        soc_reg_field_set(unit, ECI_GENERAL_CONFIGURATION_2r, &reg, FIFO_DMA_7_SELf, 0);    
        soc_reg_field_set(unit, ECI_GENERAL_CONFIGURATION_2r, &reg, FIFO_DMA_11_SELf, 0); 
        soc_reg_field_set(unit, ECI_GENERAL_CONFIGURATION_2r, &reg, CMICM_BISR_BYPASS_ENABLEf, 0); 
        SOC_ALLOW_WB_WRITE(unit, WRITE_ECI_GENERAL_CONFIGURATION_2r(unit, reg),rv);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, rv);
        channel_number = ARAD_OAM_DMA_CHANNEL_USED_EVENT;
    }
    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        cmc = channel_number / NOF_DMA_FIFO_PER_CMC;
        ch = channel_number % NOF_DMA_FIFO_PER_CMC;    
    } else {
        cmc = SOC_PCI_CMC(unit);
        ch = channel_number % NOF_DMA_FIFO_PER_CMC;  
    }


    dma_host_memory[unit] = soc_cm_salloc(unit, host_mem_size_in_bytes, "oam DMA"); 
    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "Allocating %d memory \n"),host_mem_size_in_bytes ));
    if (!dma_host_memory[unit]) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Memory allocation failed!")));
    }

    sal_memset(dma_host_memory[unit], 0,  host_mem_size_in_bytes);
    

    
    res = _soc_mem_sbus_fifo_dma_stop(unit, channel_number);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

    res = _soc_mem_sbus_fifo_dma_start_memreg(unit, channel_number,
                                             FALSE , 0, OAMP_INTERRUPT_MESSAGEr, MEM_BLOCK_ALL,
                                             0, host_mem_num_entries, dma_host_memory[unit]);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);


    
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(cmc, ch),dma_threshold);    

    reg=0;
    reg = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));

    MICROSECONDS_TO_DMA_CFG__TIMEOUT_COUNT(timeout);
    timeout &= 0x3fff;

    soc_endian_get(unit, &dont,&care,&endian);

    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &reg, TIMEOUT_COUNTf, timeout);
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &reg, ENDIANESSf, endian !=0);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), reg);

    if (SOC_IS_JERICHO(unit)) {
        res = jer_oam_event_fifo_init(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
    }

    
    soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_report_dma_set_unsafe(
	 SOC_SAND_IN  int                                 unit
	 )
{
	uint32 res;
	uint32 reg, dma_threshold;
    uint32 host_mem_num_entries , timeout;
    uint32 host_mem_size_in_bytes;
    int cmc, ch;
    int rv;
    int dont, care,endian; 
    int channel_number = -1;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);    

    if (!SOC_IS_JERICHO(unit)) 
    {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Oam report DMA is enabled only from Jericho")));
    }

    
    if (SOC_DPP_CONFIG(unit)->pp.oam_use_report_fifo_dma == 0) {
        SOC_SAND_EXIT_NO_ERROR;
    }

    dma_threshold = SOC_DPP_CONFIG(unit)->pp.oam_dma_report_threshold;

    host_mem_size_in_bytes = SOC_DPP_CONFIG(unit)->pp.oam_dma_report_buffer_size;
    timeout = SOC_DPP_CONFIG(unit)->pp.oam_dma_report_timeout;
    

    
    
    host_mem_num_entries = (host_mem_size_in_bytes+1) / _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA;

    host_mem_num_entries =  (host_mem_num_entries < dma_threshold) ? dma_threshold  :host_mem_num_entries; 

    ROUND_UP_TO_NEAREST_POWER_OF_2(host_mem_num_entries);
    host_mem_num_entries = (host_mem_num_entries <SMALLEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY )? SMALLEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY : host_mem_num_entries;
    host_mem_num_entries = ( host_mem_num_entries >ARAD_PP_OAM_LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY)? ARAD_PP_OAM_LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY: host_mem_num_entries;

    arad_pp_num_entries_in_report_dma_host_memory[unit] = host_mem_num_entries;

    host_mem_size_in_bytes = host_mem_num_entries * _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA;

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_free_find, (unit, FALSE, &channel_number))); 
    if (channel_number == -1) {
        
        LOG_ERROR(BSL_LS_SOC_OAM,(BSL_META_U(unit, "unit %d Cannot find free DMA-channel."),unit));
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 31, exit);
    }              
    SOC_ALLOW_WB_WRITE(unit, jer_mgmt_dma_fifo_channel_set(unit, channel_number, dma_fifo_channel_src_oam_status), rv);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, rv);
    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        cmc = channel_number / NOF_DMA_FIFO_PER_CMC;
        ch = channel_number % NOF_DMA_FIFO_PER_CMC;    
    } else {
        cmc = SOC_PCI_CMC(unit);
        ch = channel_number % NOF_DMA_FIFO_PER_CMC;  
    }

	report_dma_host_memory[unit] = soc_cm_salloc(unit, host_mem_size_in_bytes, "oam report DMA"); 
    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "Allocating %d memory \n"),host_mem_size_in_bytes ));
    if (!report_dma_host_memory[unit]) {
		SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Memory allocation failed!")));
    }

	sal_memset(report_dma_host_memory[unit], 0,  host_mem_size_in_bytes);
    

	
    res = _soc_mem_sbus_fifo_dma_stop(unit, channel_number);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

    res = _soc_mem_sbus_fifo_dma_start_memreg(unit, channel_number,
                                             FALSE , 0, OAMP_STAT_INTERRUPT_MESSAGEr, MEM_BLOCK_ALL,
                                             0, host_mem_num_entries, report_dma_host_memory[unit]);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

    
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(cmc, ch),dma_threshold);

    reg=0;
    reg = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));

    MICROSECONDS_TO_DMA_CFG__TIMEOUT_COUNT(timeout);
    timeout &= 0x3fff;

    soc_endian_get(unit, &dont,&care,&endian);

    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &reg, TIMEOUT_COUNTf, timeout);
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &reg, ENDIANESSf, endian !=0);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), reg);

    if (SOC_IS_JERICHO(unit)) {
        res = jer_oam_stat_fifo_init(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
    }

    
    soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_dma_unset_unsafe(
     SOC_SAND_IN  int                                 unit
     )
{
    uint32 res, reg;
    uint32 use_event_dma = 0;
    int channel_number = -1;
    int ch,cmc;
    int rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    use_event_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_event_fifo_dma;

    
    if (!use_event_dma) 
    {
        SOC_SAND_EXIT_NO_ERROR;
    }

    if (dma_host_memory[unit] != NULL) {
        soc_cm_sfree(unit, dma_host_memory[unit]);
        dma_host_memory[unit] = NULL;
    }

    if (SOC_IS_JERICHO(unit))
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, jer_mgmt_dma_fifo_channel_get(unit, dma_fifo_channel_src_oam_event, &channel_number));         
        
        if(channel_number != -1)
        {
            if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
                cmc = channel_number / NOF_DMA_FIFO_PER_CMC;
                ch = channel_number % NOF_DMA_FIFO_PER_CMC;    
            } else {
                cmc = SOC_PCI_CMC(unit);
                ch = channel_number % NOF_DMA_FIFO_PER_CMC;  
            }  
            res = _soc_mem_sbus_fifo_dma_stop(unit, channel_number);
            SOC_SAND_CHECK_FUNC_RESULT(res, 234 ,exit);           
            SOC_ALLOW_WB_WRITE(unit, jer_mgmt_dma_fifo_channel_set(unit, channel_number, dma_fifo_channel_src_reserved), rv);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, rv);
            soc_cmicm_cmcx_intr0_disable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));                    

            res = jer_oam_event_fifo_cleanup(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
        }
    }
    else
    {
        res = _soc_mem_sbus_fifo_dma_stop(unit, ARAD_OAM_DMA_CHANNEL_USED_EVENT);
        SOC_SAND_CHECK_FUNC_RESULT(res, 234 ,exit);
        
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, READ_ECI_GENERAL_CONFIGURATION_2r(unit, &reg));
        soc_reg_field_set(unit, ECI_GENERAL_CONFIGURATION_2r, &reg, FIFO_DMA_3_SELf, 0);    
        SOC_ALLOW_WB_WRITE(unit, WRITE_ECI_GENERAL_CONFIGURATION_2r(unit, reg),rv);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, rv);
    } 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 arad_pp_oam_report_dma_unset_unsafe(SOC_SAND_IN  int unit)
{
	uint32 res, reg;
    uint32 use_report_dma = 0;
    int channel_number = -1;
    int cmc, ch;
	SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    use_report_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_report_fifo_dma;
    if (!use_report_dma) 
    {
        SOC_SAND_EXIT_NO_ERROR;
    }

    if (report_dma_host_memory[unit] != NULL) {
        soc_cm_sfree(unit, report_dma_host_memory[unit]);
        report_dma_host_memory[unit] = NULL;
    }


    if (SOC_IS_JERICHO(unit))
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, jer_mgmt_dma_fifo_channel_get(unit, dma_fifo_channel_src_oam_status, &channel_number));         
        
        if(channel_number != -1)
        {
            if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
                cmc = channel_number / NOF_DMA_FIFO_PER_CMC;
                ch = channel_number % NOF_DMA_FIFO_PER_CMC;    
            } else {
                cmc = SOC_PCI_CMC(unit);
                ch = channel_number % NOF_DMA_FIFO_PER_CMC;  
            }  
            res = _soc_mem_sbus_fifo_dma_stop(unit, channel_number);
            SOC_SAND_CHECK_FUNC_RESULT(res, 234 ,exit);           
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, jer_mgmt_dma_fifo_channel_set(unit, channel_number, dma_fifo_channel_src_reserved));    
            soc_cmicm_cmcx_intr0_disable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));  

            res = jer_oam_stat_fifo_cleanup(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
        }
    }
    else
    {
        res = _soc_mem_sbus_fifo_dma_stop(unit, ARAD_OAM_DMA_CHANNEL_USED_STAT_EVENT);
        SOC_SAND_CHECK_FUNC_RESULT(res, 234 ,exit);
        
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, READ_ECI_GENERAL_CONFIGURATION_2r(unit, &reg));
        soc_reg_field_set(unit, ECI_GENERAL_CONFIGURATION_2r, &reg, FIFO_DMA_2_SELf, 0);    
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, WRITE_ECI_GENERAL_CONFIGURATION_2r(unit, reg));

    } 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


#define _ARAD_PP_OAMP_SET_FIRST_THREE_OCTETS_IN_TLV(tlv_value, type, length) \
    tlv_value = (( type & 0xff)<<16) | (length   & 0xffff)


uint32
  arad_pp_oam_oamp_init_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  uint8                                  is_bfd,
      SOC_SAND_IN  uint32                                 cpu_system_port,
      SOC_SAND_IN  uint8                                  is_second_init
  )
{
    uint32 res;
    uint32 reg = 0;
    uint64 reg64;
    soc_reg_above_64_val_t  reg_above_64;
    soc_reg_t reg_name;
    soc_field_t field;
    uint8 interrupt_message_event[SOC_PPC_OAM_EVENT_COUNT] = {0};
    SOC_PPC_BFD_PDU_STATIC_REGISTER              bfd_pdu;
    SOC_TMC_DEST_INFO            destination;
    uint32 destination_for_itmh;
    uint32 i, bfd_gach_cc, bfd_gach_cv, y1731_gach, bfd_pwe_cw, oam_pwe_cw, oam_pwe_channel;
    uint32 num_clocks_per_mep_scan;
    uint32 bfd_cc_channel, bfd_cv_channel, y1731_channel, bfd_pwe_channel;    
    uint32 use_event_dma=0;
    uint32 use_report_dma=0;
    uint32 event_ndx;
    int core;
    uint32 pp_port;
    int count_oamp=0;
    int bfd_rate_index;
    uint32   oamp_arbiter_weight;
    uint8 oam_pe_is_init = 0;
    bcm_gport_t oamp_port[SOC_DPP_DEFS_MAX(NOF_CORES)];
    _bcm_dpp_gport_info_t gport_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_INIT_UNSAFE);

    
    
    for (event_ndx=0; event_ndx<SOC_PPC_OAM_EVENT_COUNT; event_ndx++) {
        interrupt_message_event[event_ndx]=1;
    }
    res = arad_pp_oam_oamp_enable_interrupt_message_event_set_unsafe(unit, interrupt_message_event); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (SOC_IS_JERICHO(unit)) 
    {        
        use_report_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_report_fifo_dma;        
    }

    use_event_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_event_fifo_dma;

    
    if  (use_event_dma && !is_second_init) {
        res = arad_pp_oam_dma_set_unsafe(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

    if  (use_report_dma && !is_second_init) {
        res = arad_pp_oam_report_dma_set_unsafe(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);

    
    if (soc_property_get(unit, spn_ITMH_ARAD_MODE_ENABLE, 0) && (SOC_IS_JERICHO(unit))) {
        
        reg_name = (SOC_IS_QAX_A0(unit) || SOC_IS_QUX(unit)) ? OAMP_REG_0130r : OAMP_TX_PPHr;
        field = (SOC_IS_QAX_A0(unit) || SOC_IS_QUX(unit)) ? FIELD_5_5f : TX_USE_JER_ITMHf;
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, (soc_reg_field32_modify(unit, reg_name, REG_PORT_ANY,field, 0)));
    }

    reg = 0;
    soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, TRANSMIT_ENABLEf, 1);
    soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, SCANNER_ENABLEf, 1);
    soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, RX_2_CPUf, 0);
    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        
        soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, UPDATE_STATE_EVEN_IF_EVENT_FIFO_FULLf, 1);
    } else if (SOC_IS_ARADPLUS(unit)) {
        soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, DMR_LMR_RESPONSE_ENABLEf, 1);
        if (SOC_IS_JERICHO(unit)) {
            soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, REPORT_MODEf, SOC_PPC_OAM_REPORT_MODE_NORMAL); 
            rx_report_event_mode[unit] = SOC_PPC_OAM_REPORT_MODE_NORMAL;
        }
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_OAMP_MODE_REGISTERr(unit, reg)); 

    reg = 0;
    soc_reg_field_set(unit, OAMP_NUMBER_OF_RMEPSr, &reg, NUMBER_OF_RMEPSf, SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 41, exit, WRITE_OAMP_NUMBER_OF_RMEPSr(unit, reg));

    reg = 0;

    res = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), oamp_port, &count_oamp);
    if(res != BCM_E_NONE || count_oamp < 1) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

    res = _bcm_dpp_gport_to_phy_port(unit, oamp_port[0], _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
    if(res != BCM_E_NONE) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

    res = soc_port_sw_db_local_to_pp_port_get(unit, gport_info.local_port, &pp_port, &core);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 42, exit);

    soc_reg_field_set(unit, OAMP_DOWN_PTCHr, &reg, DOWN_PTCH_PP_SSPf, pp_port);
    soc_reg_field_set(unit, OAMP_DOWN_PTCHr, &reg, DOWN_PTCH_OPAQUE_PT_ATTRf, _ARAD_PP_OAM_PTCH_OPAQUE_VALUE);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_DOWN_PTCHr(unit, reg)); 

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  51,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, REMOTE_MEP_EXACT_MATCH_MNGMNT_UNIT_ENABLEf,  1));

    
    
    bfd_cc_channel = _arad_pp_oam_mpls_tp_channel_type_enum_to_channel_type(unit, _ARAD_PP_OAM_MPLS_TP_CHANNEL_CC_ENUM);
    bfd_gach_cc = 0x10000000 |  bfd_cc_channel;
    bfd_cv_channel = _arad_pp_oam_mpls_tp_channel_type_enum_to_channel_type(unit, _ARAD_PP_OAM_MPLS_TP_CHANNEL_CV_ENUM);
    bfd_gach_cv = 0x10000000 |  bfd_cv_channel;
    bfd_pwe_channel = _arad_pp_oam_mpls_tp_channel_type_enum_to_channel_type(unit, _ARAD_PP_OAM_MPLS_TP_CHANNEL_PW_ACH_ENUM);
    bfd_pwe_cw = 0x10000000 |  bfd_pwe_channel;
    oam_pwe_channel = _arad_pp_oam_mpls_tp_channel_type_enum_to_channel_type(unit, _ARAD_PP_OAM_MPLS_TP_CHANNEL_PWE_ON_OAM_ENUM);
    oam_pwe_cw = 0x10000000 |  oam_pwe_channel;

    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, OAMP_BFD_CC_MPLSTP_GALr, REG_PORT_ANY, 0, BFD_CC_MPLSTP_GALf,  0x0000D140)); 
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, OAMP_BFD_CC_MPLSTP_GACHr, REG_PORT_ANY, 0, BFD_CC_MPLSTP_GACHf,  bfd_gach_cc));

        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, OAMP_BFD_CV_MPLSTP_GALr, REG_PORT_ANY, 0, BFD_CV_MPLSTP_GALf,  0x0000D140)); 
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, OAMP_BFD_CV_MPLSTP_GACHr, REG_PORT_ANY, 0, BFD_CV_MPLSTP_GACHf,  bfd_gach_cv));

    }

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, OAMP_Y_1731O_PWE_GACHr, REG_PORT_ANY, 0, Y_1731O_PWE_GACHf,  oam_pwe_cw)); 

    soc_reg_field_set(unit, OAMP_Y_1731O_MPLSTP_GALr, &reg, Y_1731O_MPLSTP_GALf, 0x0000D140); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 140, exit, WRITE_OAMP_Y_1731O_MPLSTP_GALr(unit, reg));
    y1731_channel = _arad_pp_oam_mpls_tp_channel_type_enum_to_channel_type(unit, _ARAD_PP_OAM_MPLS_TP_CHANNEL_Y1731_ENUM);
    y1731_gach = 0x10000000 |  y1731_channel; 
    soc_reg_field_set(unit, OAMP_Y_1731O_MPLSTP_GACHr, &reg, Y_1731O_MPLSTP_GACHf, y1731_gach); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit, WRITE_OAMP_Y_1731O_MPLSTP_GACHr(unit, reg));


    if (is_bfd) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  52,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_MY_DISCRIMINATOR_RANGE_STARTr, REG_PORT_ANY, 0, BFD_MY_DISCRIMINATOR_RANGE_STARTf,  0));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  55,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_IPV4_UDP_SPORTr, REG_PORT_ANY, 0, BFD_IPV4_UDP_SPORTf,  0));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_MPLS_UDP_SPORTr, REG_PORT_ANY, 0, BFD_MPLS_UDP_SPORTf,  0));

        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_discriminator_type_update", 0)) {
            if (!SOC_IS_QAX(unit)) {
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_IPV4_CONTROLr, REG_PORT_ANY, 0, BFD_IPV4_CHECK_YOUR_DISCf, 0));
            } else {
                
                soc_reg_above_64_val_t profile_entry;
                uint8  profile_indx = 0;
                res = READ_OAMP_MEP_PROFILEm(unit, MEM_BLOCK_ANY,profile_indx, profile_entry );
                SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
                soc_OAMP_MEP_PROFILEm_field32_set(unit, profile_entry, YOUR_DISC_CHECK_DISf, 1);
                res = WRITE_OAMP_MEP_PROFILEm(unit, MEM_BLOCK_ANY,profile_indx, profile_entry );
                SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
            }
        }

        
        if (SOC_IS_JERICHO(unit) && _BCM_BFD_SEAMLESS_BFD_FEATURE_ENABLED(unit)) {
            res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_bfd_oamp_udp_src_port_check_enable,
                    (unit,0));
            SOC_SAND_IF_ERR_EXIT(res);
        }

        
        if (SOC_IS_ARADPLUS_A0(unit) && soc_property_get(unit, spn_BFD_ENCAPSULATION_MODE, 0)) {
            
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_PWE_CW_0r, REG_PORT_ANY, 0, BFD_PWE_CW_0f,  bfd_gach_cc));
        }
        else if (SOC_IS_JERICHO(unit)) {
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  105,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_PWE_CW_0r, REG_PORT_ANY, 0, BFD_PWE_CW_0f,  bfd_pwe_cw));
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_PWE_CW_1r, REG_PORT_ANY, 0, BFD_PWE_CW_1f,  bfd_gach_cc));
        }
        else {
            
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  105,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_PWE_CW_0r, REG_PORT_ANY, 0, BFD_PWE_CW_0f,  bfd_pwe_cw));
        }

        
        
        SOC_REG_ABOVE_64_CLEAR(reg_above_64);

        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, reg_above_64, CCM_CLASS_0f, 0);
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, reg_above_64, CCM_CLASS_1f, 1); 
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, reg_above_64, CCM_CLASS_2f, 2);
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, reg_above_64, CCM_CLASS_3f, 3);
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, reg_above_64, CCM_CLASS_4f, 4);
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, reg_above_64, CCM_CLASS_5f, 5);
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, reg_above_64, CCM_CLASS_6f, 6);
        soc_reg_field_set(unit, OAMP_PR_2_FW_DTCr, reg_above_64, CCM_CLASS_7f, 7);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_OAMP_PR_2_FW_DTCr_WITH_FAST_ACCESS(unit, reg_above_64)); 

        SOC_PPC_BFD_PDU_STATIC_REGISTER_clear(&bfd_pdu);
        bfd_pdu.bfd_vers = SOC_PPC_BFD_PDU_VERSION;
        bfd_pdu.bfd_sta = 3;
        bfd_pdu.bfd_flags = 1; 
        bfd_pdu.bfd_length = SOC_PPC_BFD_PDU_LENGTH;
        res = arad_pp_oam_bfd_pdu_static_register_set_unsafe(unit, &bfd_pdu);
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

        if (SOC_IS_ARADPLUS(unit)) {
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_PWE_ROUTER_ALERTr, REG_PORT_ANY, 0, BFD_PWE_ROUTER_ALERTf,  0x1040)); 
        }

        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            
            
            uint32 dip = (soc_sand_os_rand() % 0x800000) + (0x7f <<24);
            soc_reg_field_set(unit, OAMP_BFD_MPLS_DST_IPr, &reg, BFD_MPLS_DST_IPf,dip);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_OAMP_BFD_MPLS_DST_IPr(unit, reg)); 
        }

        
        for (bfd_rate_index=0 ; bfd_rate_index <= _ARAD_PP_OAM_TX_RATE_INDEX_MAX ; ++bfd_rate_index) {
            res = arad_pp_oam_bfd_tx_rate_set_unsafe(unit,bfd_rate_index,0);  
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        }
    } else {
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0) && !(SOC_IS_QAX(unit)) ) {
            
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, READ_OAMP_ETH_OAM_CONTROLr(unit, &reg));
            soc_reg_field_set(unit, OAMP_ETH_OAM_CONTROLr, &reg, ETH_OAM_CHECK_MDLf, 0);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_OAMP_ETH_OAM_CONTROLr(unit, reg));

        }
        
        if (!SOC_IS_QAX(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, READ_OAMP_ETH_OAM_CONTROLr(unit, &reg));
            soc_reg_field_set(unit, OAMP_ETH_OAM_CONTROLr, &reg, ETH_OAM_CHECK_MAIDf, 0);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_OAMP_ETH_OAM_CONTROLr(unit, reg));
        }

    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr(unit, &reg));
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_LOC_SET_EVENTf,1);
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_LOC_CLEAR_EVENTf,1);
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_RDI_SET_EVENTf, 1);
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_RDI_CLEAR_EVENTf,1);
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_ALMOST_LOC_SET_EVENTf, 0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr(unit, reg)); 

    if (is_second_init) {
        goto exit;
    }

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);

    
    num_clocks_per_mep_scan = ARAD_PP_OAM_NUM_CLOCKS_IN_MEP_SCAN;
    if (SOC_IS_QAX(unit)) {
        uint32 active_stages;
        uint32 idle_stages;
        COMPILER_64_ZERO(reg64);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 2111, exit, READ_OAMP_TIMER_CONFIGr(unit, &reg64));
        soc_reg64_field32_set(unit,OAMP_TIMER_CONFIGr,&reg64,NUM_CLOCKS_SECf, arad_chip_kilo_ticks_per_sec_get(unit)*1000);
        soc_reg64_field32_set(unit,OAMP_TIMER_CONFIGr,&reg64,CMIC_TOD_MODEf, 3); 
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 2111, exit, WRITE_OAMP_TIMER_CONFIGr(unit, reg64));

        
        for (i = ARAD_PP_OAM_NUM_STAGES_ACTIVE_IN_MEP_SCAN; i <= ARAD_PP_OAM_MAX_NUM_STAGES_IN_MEP_SCAN; i++) {
            if (num_clocks_per_mep_scan > (((SOC_DPP_DEFS_GET(unit, oamp_number_mep_db_entries)) * ARAD_PP_OAMP_TXM_1_STAGE_CLOCKS +
                                             ARAD_PP_OAMP_MAX_EXT_DATA_ENTRY * ARAD_PP_OAMP_TXM_1_STAGE_CLOCKS)*
                                           (ARAD_PP_OAM_NUM_STAGES_IDLE_IN_MEP_SCAN + i)) / i) {
                break;
            }
        }

        if(i<=ARAD_PP_OAM_MAX_NUM_STAGES_IN_MEP_SCAN) {
            active_stages =  i;
            idle_stages = ARAD_PP_OAM_NUM_STAGES_IDLE_IN_MEP_SCAN;
        } else {
            active_stages = ARAD_PP_OAM_MAX_NUM_STAGES_IN_MEP_SCAN;
            idle_stages   = 0;
        }

        for (i = 0; i < _BCM_DPP_OAM_OAMP_MEP_DB_BANKS_NOF(unit); i++) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 2111, exit, READ_OAMP_TXM_BANK_SCAN_CONFIGr_REG32(unit, i, &reg));
            soc_reg_field_set(unit, OAMP_TXM_BANK_SCAN_CONFIGr, &reg, NUM_CLOCKS_SCAN_BANK_Nf, num_clocks_per_mep_scan);
            if (i <= _QAX_PP_OAM_OAMP_MEP_DB_LAST_ACTIVE_BANK) {
                soc_reg_field_set(unit, OAMP_TXM_BANK_SCAN_CONFIGr, &reg, NUM_STAGES_ACTIVE_BANK_Nf, active_stages);
                soc_reg_field_set(unit, OAMP_TXM_BANK_SCAN_CONFIGr, &reg, NUM_STAGES_IDLE_BANK_Nf, idle_stages);
            } else {
                soc_reg_field_set(unit, OAMP_TXM_BANK_SCAN_CONFIGr, &reg, NUM_STAGES_ACTIVE_BANK_Nf, 0);
                soc_reg_field_set(unit, OAMP_TXM_BANK_SCAN_CONFIGr, &reg, NUM_STAGES_IDLE_BANK_Nf, 0);
            }
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 2111, exit, WRITE_OAMP_TXM_BANK_SCAN_CONFIGr_REG32(unit, i ,reg));
        }
    } else if (SOC_IS_JERICHO(unit)) {
	uint32 active_stages = 2; 
        COMPILER_64_ZERO(reg64);

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 2111, exit, READ_OAMP_TIMER_CONFIGr(unit, &reg64)); 

	if(num_clocks_per_mep_scan > ARAD_PP_OAM_NUM_CLOCKS_MIN_NEEDED){
            
	    active_stages = ((ARAD_PP_OAM_NUM_CLOCKS_MIN_NEEDED)/(num_clocks_per_mep_scan - ARAD_PP_OAM_NUM_CLOCKS_MIN_NEEDED)) + 1;
            
            active_stages = (active_stages>ARAD_PP_OAM_MAX_NUM_STAGES_IN_MEP_SCAN) ? ARAD_PP_OAM_MAX_NUM_STAGES_IN_MEP_SCAN : active_stages;
	} else {
		
	    active_stages = ARAD_PP_OAM_MAX_NUM_STAGES_IN_MEP_SCAN; 
	}

        soc_reg64_field32_set(unit,OAMP_TIMER_CONFIGr,&reg64,NUM_STAGES_ACTIVEf, active_stages);
        soc_reg64_field32_set(unit,OAMP_TIMER_CONFIGr,&reg64,NUM_CLOCKS_SCANf, num_clocks_per_mep_scan);
        soc_reg64_field32_set(unit,OAMP_TIMER_CONFIGr,&reg64,NUM_CLOCKS_SECf, arad_chip_kilo_ticks_per_sec_get(unit)*1000);
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 2111, exit, WRITE_OAMP_TIMER_CONFIGr(unit, reg64)); 


    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 12, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_TIMER_CONFIGr, REG_PORT_ANY, 0, NUM_CLOCKS_SCANf, num_clocks_per_mep_scan));
    }

    SOC_TMC_DEST_INFO_clear(&destination);
    destination.id = cpu_system_port;
    destination.type = ARAD_DEST_TYPE_SYS_PHY_PORT;

    res = arad_hpu_itmh_fwd_dest_info_build(unit, destination, soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, 0), FALSE, &destination_for_itmh);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_OAMP_CPUPORTr(unit, reg_above_64));
    for (i=0; i < _ARAD_PP_OAM_NUMBER_OF_OAMP_ERROR_TYPES; i++) {
        SHR_BITCOPY_RANGE(reg_above_64, _ARAD_PP_OAM_ITMH_DEST_INFO_WIDTH*i, &destination_for_itmh, 0, _ARAD_PP_OAM_ITMH_DEST_INFO_WIDTH); 
    }

    soc_reg_above_64_field32_set(unit, OAMP_CPUPORTr, reg_above_64, CPU_PTCH_PP_SSPf, ARAD_OAMP_PORT_ID);   

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_OAMP_CPUPORTr(unit, reg_above_64)); 
     
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  51,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, REMOTE_MEP_EXACT_MATCH_MNGMNT_UNIT_ENABLEf,  1));

    if (SOC_IS_ARADPLUS(unit)) {
        uint32 tlv, reg_index, field_name;

        sw_state_access[unit].dpp.soc.arad.pp.oamp_pe.oamp_pe_init.get(unit, &oam_pe_is_init);
        if(!oam_pe_is_init){
            res = arad_pp_oamp_pe_unsafe(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
            sw_state_access[unit].dpp.soc.arad.pp.oamp_pe.oamp_pe_init.set(unit, TRUE);
        }
        
        _ARAD_PP_OAMP_SET_FIRST_THREE_OCTETS_IN_TLV(tlv,2,1);
        soc_reg_field_set(unit,OAMP_PORT_STAT_TLVr, &reg, PORT_STAT_TLVf, tlv);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_PORT_STAT_TLVr(unit, reg)); 

        _ARAD_PP_OAMP_SET_FIRST_THREE_OCTETS_IN_TLV(tlv,4,1);
        soc_reg_field_set(unit,OAMP_INTERFACE_STAT_TLVr, &reg, INTERFACE_STAT_TLVf, tlv);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_INTERFACE_STAT_TLVr(unit, reg)); 

        
        
        reg = 0;

        if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit)) {
            
            for (field_name = TRAP_N_1731O_MPLSTPf; field_name < TRAP_N_1731O_MPLSTPf + 8; field_name++) {
                for (reg_index=1; reg_index<=3; reg_index++) {
                    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 234, exit,
                                                     ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_RX_TRAP_CODESr, REG_PORT_ANY, reg_index, field_name, reg));
                }
            }
        }

        
        res = READ_OAMP_ARBITER_WEIGHTr(unit, &oamp_arbiter_weight);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 250, exit);
        soc_reg_field_set(unit, OAMP_ARBITER_WEIGHTr, &oamp_arbiter_weight, TXM_ARB_WEIGHTf, 1 );   
        if(SOC_IS_JERICHO(unit)){
           soc_reg_field_set(unit, OAMP_ARBITER_WEIGHTr, &oamp_arbiter_weight, SAT_ARB_WEIGHTf, 0 );   
        }
        else{
           soc_reg_field_set(unit, OAMP_ARBITER_WEIGHTr, &oamp_arbiter_weight, TST_ARB_WEIGHTf, 0 );   
        }
        res = WRITE_OAMP_ARBITER_WEIGHTr(unit, oamp_arbiter_weight);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 251, exit);

        if(SOC_IS_JERICHO(unit)){
            reg = 0;
            
            soc_reg_field_set(unit, OAMP_MIN_NET_PKT_SIZEr, &reg, MIN_NET_PKT_SIZEf, _ARAD_PP_OAM_MIN_PKT_SIZE_TXM);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_OAMP_MIN_NET_PKT_SIZEr(unit, reg));
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_global_init_unsafe()", 0, 0);
}


#define _ARAD_PP_OAM_TCAM_ENTRY_BUILD_SET_MASK_FOR_VERSION_BY_OPCODE(opcode) ((opcode==SOC_PPC_OAM_ETHERNET_PDU_OPCODE_1DM || \
                                                                                                  opcode==SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMR || opcode==SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM || \
                                                                                                  opcode==SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMR || opcode==SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMM)?\
                                                                                                   0xfffffffe :0xffffffff)

STATIC
  void
    arad_pp_oam_tcam_entry_build_mpls_ignore_mdl_jericho(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  SOC_PPC_OAM_TCAM_ENTRY_KEY   *key,
      SOC_SAND_OUT ARAD_TCAM_ENTRY              *entry
  )
{
    uint32
        mask,
        val;
    uint32 offset = 0;
    uint32 field_size = 0;
    ARAD_TCAM_ENTRY_clear(entry);

    entry->valid = TRUE;
    entry->is_for_update = 0;
    entry->is_inserted_top = 1;

    
    mask = 0xFFFFFFFF;
    field_size = 4;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&key->ttc, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask,     _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    
    mask = 0xFFFFFFFF; 
    field_size = 4;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&key->fwd_code, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask,          _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    
    mask = 0;
    val = 0;
    field_size = SOC_IS_JERICHO_PLUS(unit) ? 4 : 3;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    
    mask = 0;
    val = 0;
    field_size = 1;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    if (SOC_IS_JERICHO_PLUS(unit)) {
        
        mask = 0;
        val = 0;
        field_size = 1;
        offset += field_size;
        soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
        soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);
        offset = offset + 2; 
    }


    if (key->mdl == _ARAD_PP_OAM_Y1731_DEFAULT_MDLEVEL) {

        mask = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0) ? 0 : 0xFFFFFFFF; 
        
        field_size = 3;
        offset += field_size;
        soc_sand_bitstream_set_any_field(&key->mdl, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE  - offset, field_size, entry->value);
        soc_sand_bitstream_set_any_field(&mask,     _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE  - offset, field_size, entry->mask);
        mask = _ARAD_PP_OAM_TCAM_ENTRY_BUILD_SET_MASK_FOR_VERSION_BY_OPCODE(key->opcode);
        val = 0; 
        field_size = 5;
        offset += field_size;
        soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
        soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

        if (key->opcode == (SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT - 1)) {
            mask = 0; 
            val = 0;
            field_size = 8;
            offset += field_size;
            soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
            soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);
        } else {
            mask = 0xFFFFFFFF; 
            field_size = 8;
            offset += field_size;
            soc_sand_bitstream_set_any_field(&key->opcode, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
            soc_sand_bitstream_set_any_field(&mask,        _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);
        }
    }
}

STATIC
  void
    arad_pp_oam_y1711_tcam_entry_build(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  SOC_PPC_OAM_TCAM_ENTRY_KEY   *key,
      SOC_SAND_OUT ARAD_TCAM_ENTRY              *entry
  )
{
    uint32 mask = 0;
    uint32 val = 0;
    uint32 offset = 0;
    uint32 field_size = 0;
    ARAD_TCAM_ENTRY_clear(entry);

    entry->valid = TRUE;
    entry->is_for_update = 0;
    entry->is_inserted_top = 1;
    

    
    mask=0;
    field_size = 4;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&key->ttc, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask, 	_ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    
    mask = 0xFFFFFFFF; 
    field_size = 4;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&key->fwd_code, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask, 		 _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    
    mask = 0;
    val = 0; 
    field_size = SOC_IS_JERICHO_PLUS(unit) ? 4:3;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size , entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    
    mask = 0xFFFFFFFF;
    val = SOC_IS_JERICHO_PLUS(unit) ? 0:1;
    field_size = 1;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    if(SOC_IS_JERICHO_PLUS(unit)){
    	
    	mask = 0xFFFFFFFF;
    	val = 1;
    	field_size = 1;
    	offset += field_size;
    	soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    	soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);
    }

    
    mask = 0xFFFFFFFF;
    val = 0xe;
    field_size = 20;
    offset =  offset + (SOC_IS_JERICHO_PLUS(unit) ? (field_size+2) : field_size);
    soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);
    
    mask = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable",0) == 1) ? 0x100 : 0xFFFFFFFF;
    val = 0x101;
    field_size = 12;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

    
    mask = 0xFFFFFFFF;
    if (key->your_disc) {
    	val = 0x0c;
    } else {
    	val = 0x0b; 
    }
    field_size = 8;
    offset += field_size;
    soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
    soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);


}



STATIC
  void
    arad_pp_oam_tcam_entry_build(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  SOC_PPC_OAM_TCAM_ENTRY_KEY   *key,
      SOC_SAND_OUT ARAD_TCAM_ENTRY              *entry
  )
{
    uint32
       mask,
       val;

    uint32 gal_begin_bit = 0;
    uint32 cfm_begin_bit = 0;
    uint32 ip_begin_bit = 0;
    uint32 tcam_entry_key_header_start;
    uint32 ether_type_begin_bit = 0;
    uint32 offset = 0;
    uint32 field_size = 0;
    
    ARAD_TCAM_ENTRY_clear(entry);

    entry->valid = TRUE;
    entry->is_for_update = 0;
    entry->is_inserted_top = 1;

   
    
    if ((((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_downmep_pwe_classification", 0) == 1) && (SOC_IS_ARADPLUS_AND_BELOW(unit))))
        &&  key->fwd_code == _ARAD_PP_OAM_FWD_CODE_ETHERNET && key->ttc >= SOC_PPC_PKT_TERM_TYPE_MPLS_ETH && key->ttc <= SOC_PPC_PKT_TERM_TYPE_CW_MPLS3_ETH) {
		uint32 ether_type_begin_bit;
        
        
        
        mask = 0xFFFFFFFE;  
        field_size = 4;
        offset += field_size;
        soc_sand_bitstream_set_any_field(&key->ttc, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
        soc_sand_bitstream_set_any_field(&mask,     _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

        
        mask = 0xFFFFFFFF; 
        field_size = 4;
        offset += field_size;
        soc_sand_bitstream_set_any_field(&key->fwd_code, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
        soc_sand_bitstream_set_any_field(&mask,          _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

        mask = 0;
        val = 0; 
        field_size = SOC_IS_JERICHO_PLUS(unit) ? 4:3;
        offset += field_size;
        soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
        soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

        mask = 0;
        val = 0; 
        field_size = 1;
        offset += field_size;
        soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
        soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);
        
        if(SOC_IS_JERICHO_PLUS(unit)){
          
          mask = 0;
          val = 0;
          field_size = 1;
          offset += field_size;
          soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
          soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);
        }
        tcam_entry_key_header_start = offset;
        ether_type_begin_bit = tcam_entry_key_header_start + (12 * 8) + (key->channel_type * 4 * 8);
        cfm_begin_bit =  ether_type_begin_bit + 2*8 ;
        field_size = 16;
        offset = ether_type_begin_bit + (SOC_IS_JERICHO_PLUS(unit) ? 18:16); 
	
        mask = 0xFFFFFFFF; 
        val = 0x8902;
        soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
        soc_sand_bitstream_set_any_field(&mask,     _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);

        mask = (SOC_IS_ARADPLUS_AND_BELOW(unit)) ? 0xFFFFFFFF : 0; 
		
        field_size = 3;
        offset = cfm_begin_bit + 3;
        soc_sand_bitstream_set_any_field(&key->mdl, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->value);
        soc_sand_bitstream_set_any_field(&mask,     _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - offset, field_size, entry->mask);
    } else {
		if (key->ttc == SOC_PPC_NOF_PKT_TERM_TYPES) { 
			mask = 0;
			val = 0; 
			soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - 4, 4, entry->value);
			soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - 4, 4, entry->mask);
		} else {
			
			mask = 0xFFFFFFFE;  
			soc_sand_bitstream_set_any_field(&key->ttc, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - 4, 4, entry->value);
			soc_sand_bitstream_set_any_field(&mask,     _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - 4, 4, entry->mask);
		}

		mask = 0xFFFFFFFF; 
		soc_sand_bitstream_set_any_field(&key->fwd_code, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - 8, 4, entry->value);
		soc_sand_bitstream_set_any_field(&mask,          _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - 8, 4, entry->mask);

		mask = 0;
		val = 0; 
		soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - 11, 3, entry->value);
		soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - 11, 3, entry->mask);

		mask = 0;
		val = 0; 
		soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - 12, 1, entry->value);
		soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - 12, 1, entry->mask);

		tcam_entry_key_header_start = 12;

		if (key->fwd_code == _ARAD_PP_OAM_FWD_CODE_IPV4_UC) {
			ip_begin_bit = tcam_entry_key_header_start; 
		} else {
			switch (key->ttc) {
			case SOC_PPC_NOF_PKT_TERM_TYPES:
				ether_type_begin_bit = tcam_entry_key_header_start + 12 * 8;
				cfm_begin_bit = tcam_entry_key_header_start + 14 * 8;
				break;
			case SOC_PPC_PKT_TERM_TYPE_MPLS_ETH:
			case SOC_PPC_PKT_TERM_TYPE_CW_MPLS_ETH:
				gal_begin_bit = tcam_entry_key_header_start + 4 * 8; 
				break;
			case SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH:
			case SOC_PPC_PKT_TERM_TYPE_CW_MPLS2_ETH:
				gal_begin_bit = tcam_entry_key_header_start + 2 * 4 * 8;
				break;
			default:
				gal_begin_bit = tcam_entry_key_header_start;
			}
		}

		if (ip_begin_bit > 0) {
                int ip_offset = 0; 
                
			mask = 0xFFFFFFFF;
			val = 4; 
                ip_offset += 4; 
			soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 4, entry->value);
			soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 4, entry->mask);

			mask =   0xFFFFFFFF;
			val = 5; 
                ip_offset += 4; 
			soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 4, entry->value);
			soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 4, entry->mask);

			mask = 0xFFFFFFFF;
			val = 1; 
                ip_offset += 64; 
			soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 8, entry->value);
			soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 8, entry->mask);

			mask = 0xFFFFFFFF;
			val = 17; 
                ip_offset += 8; 
			soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 8, entry->value);
			soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 8, entry->mask);

                mask = 0xffffffff;
                val = 127; 
                ip_offset += 56;  
			soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 8, entry->value);
			soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 8, entry->mask);

			mask = 0xFFFFFFFF;
			val = 3; 
                ip_offset += 24 + 2;  
			soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 2, entry->value);
			soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 2, entry->mask);

			mask = 0xFFFFFFFF;
			val = 0x0ec8; 
                ip_offset += 30; 
			soc_sand_bitstream_set_any_field(&val,  _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 16, entry->value);
			soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ip_begin_bit - ip_offset, 16, entry->mask);



		} else if (cfm_begin_bit > 0) { 
			mask = 0xFFFFFFFF; 
			val = 0x8902;
			soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ether_type_begin_bit - 16, 16, entry->value);
			soc_sand_bitstream_set_any_field(&mask,     _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - ether_type_begin_bit - 16, 16, entry->mask);

			mask = 0xFFFFFFFF; 
			soc_sand_bitstream_set_any_field(&key->mdl, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - cfm_begin_bit - 3, 3, entry->value);
			soc_sand_bitstream_set_any_field(&mask,     _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - cfm_begin_bit - 3, 3, entry->mask);

			mask = _ARAD_PP_OAM_TCAM_ENTRY_BUILD_SET_MASK_FOR_VERSION_BY_OPCODE(key->opcode);
			val = 0; 
			soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - cfm_begin_bit - 8, 5, entry->value);
			soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - cfm_begin_bit - 8, 5, entry->mask);

			if (key->opcode == (SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT - 1)) {
				mask = 0; 
				val = 0;
				soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - cfm_begin_bit - 16, 8, entry->value);
				soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - cfm_begin_bit - 16, 8, entry->mask);
			} else {
				mask = 0xFFFFFFFF; 
				soc_sand_bitstream_set_any_field(&key->opcode, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - cfm_begin_bit - 16, 8, entry->value);
				soc_sand_bitstream_set_any_field(&mask,        _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - cfm_begin_bit - 16, 8, entry->mask);
			}
        } else { 
			mask = 0xFFFFFFFF; 
			soc_sand_bitstream_set_any_field(&key->mpls_label, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 20, 20, entry->value);
			soc_sand_bitstream_set_any_field(&mask,            _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 20, 20, entry->mask);

			mask = 0; 
			val = 0;
			soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 32, 8, entry->value);
			soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 32, 8, entry->mask);

			mask = 0xFFFFFFFF;
			val = 0x1000; 
			soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 48, 16, entry->value);
			soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 48, 16, entry->mask);

			
			soc_sand_bitstream_set_any_field(&key->channel_type, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 64, 16, entry->value);
			soc_sand_bitstream_set_any_field(&mask,              _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 64, 16, entry->mask);

			if (key->mdl == _ARAD_PP_OAM_Y1731_DEFAULT_MDLEVEL) {
				mask = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0)? 0 :0xFFFFFFFF; 
                    
				soc_sand_bitstream_set_any_field(&key->mdl, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 67, 3, entry->value);
				soc_sand_bitstream_set_any_field(&mask,     _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 67, 3, entry->mask);

				mask = _ARAD_PP_OAM_TCAM_ENTRY_BUILD_SET_MASK_FOR_VERSION_BY_OPCODE(key->opcode);
				val = 0; 
				soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 72, 5, entry->value);
				soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 72, 5, entry->mask);

				if (key->opcode == (SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT - 1)) {
					mask = 0; 
					val = 0;
					soc_sand_bitstream_set_any_field(&val, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 80, 8, entry->value);
					soc_sand_bitstream_set_any_field(&mask, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 80, 8, entry->mask);
				} else {
					mask = 0xFFFFFFFF; 
					soc_sand_bitstream_set_any_field(&key->opcode, _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 80, 8, entry->value);
					soc_sand_bitstream_set_any_field(&mask,        _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE - gal_begin_bit - 80, 8, entry->mask);
				}
			}
		}
	}
}





STATIC
uint32
  arad_pp_oam_tcam_entry_to_action(
      SOC_SAND_IN  SOC_PPC_OAM_TCAM_ENTRY_ACTION  *entry,
      SOC_SAND_OUT ARAD_TCAM_ACTION               *action
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);


  ARAD_TCAM_ACTION_clear(action);
  SHR_BITCOPY_RANGE(action->value, 0, &entry->type, 0, 2); 
  SHR_BITCOPY_RANGE(action->value, 2, &entry->is_oam, 0, 1); 
  SHR_BITCOPY_RANGE(action->value, 3, &entry->is_bfd, 0, 1); 
  SHR_BITCOPY_RANGE(action->value, 4, &entry->opcode, 0, 4); 
  SHR_BITCOPY_RANGE(action->value, 8, &entry->mdl, 0, 3); 
  SHR_BITCOPY_RANGE(action->value, 11, &entry->your_discr, 0, 1); 
  SHR_BITCOPY_RANGE(action->value, 12, &entry->oam_lif_tcam_result, 0, 16); 
  SHR_BITCOPY_RANGE(action->value, 28, &entry->oam_lif_tcam_result_valid, 0, 1); 
  SHR_BITCOPY_RANGE(action->value, 29, &entry->my_cfm_mac, 0, 1); 
  SHR_BITCOPY_RANGE(action->value, 30, &entry->oam_stamp_offset, 0, 7); 
  SHR_BITCOPY_RANGE(action->value, 37, &entry->oam_offset, 0, 7); 
  SHR_BITCOPY_RANGE(action->value, 44, &entry->oam_pcp, 0, 3); 
  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_entry_to_action()",0,0);
}

uint32
  arad_pp_oam_tcam_action_to_entry(
      SOC_SAND_IN  ARAD_TCAM_ACTION               *action,
      SOC_SAND_OUT SOC_PPC_OAM_TCAM_ENTRY_ACTION  *entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SHR_BITCOPY_RANGE(&entry->type, 0, action->value, 0, 2); 
  SHR_BITCOPY_RANGE(&entry->is_oam, 0, action->value, 2, 1); 
  SHR_BITCOPY_RANGE(&entry->is_bfd, 0, action->value, 3, 1); 
  SHR_BITCOPY_RANGE(&entry->opcode, 0, action->value, 4, 4); 
  SHR_BITCOPY_RANGE(&entry->mdl, 0, action->value, 8, 3); 
  SHR_BITCOPY_RANGE(&entry->your_discr, 0, action->value, 11, 1); 
  SHR_BITCOPY_RANGE(&entry->oam_lif_tcam_result, 0, action->value, 12, 16); 
  SHR_BITCOPY_RANGE(&entry->oam_lif_tcam_result_valid, 0, action->value, 28, 1); 
  SHR_BITCOPY_RANGE(&entry->my_cfm_mac, 0, action->value, 29, 1); 
  SHR_BITCOPY_RANGE(&entry->oam_stamp_offset, 0, action->value, 30, 7); 
  SHR_BITCOPY_RANGE(&entry->oam_offset, 0, action->value, 37, 7); 
  SHR_BITCOPY_RANGE(&entry->oam_pcp, 0, action->value, 44, 3); 
  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_action_to_entry()",0,0);
}


uint32
  arad_pp_oam_classifier_profile_reset(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile,
    SOC_SAND_IN  uint8                                    is_bfd
  )
{
  SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY      oam1_key;
  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD   oam_payload;
  uint32 is_ingress, mep_type, mip_type, is_my_cfm_mac, action;
  uint32 oam_mirror_profile;
  uint32 oam_trap_code;
  uint32 res;
  uint8 internal_opcode;
  _oam_oam_a_b_table_buffer_t oama_buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  sal_memset(&oama_buffer,0, sizeof(_oam_oam_a_b_table_buffer_t) );

  SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);
  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam_payload);

  res = arad_pp_oam_classifier_oam1_allocate_sw_buffer(unit,&oama_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);

  oam1_key.inject = 0;
  oam1_key.is_bfd = is_bfd;
  oam_payload.counter_disable = 1;
  oam_payload.sub_type = 0; 
  
  oam_payload.meter_disable = 1;
  
  oam_payload.mirror_profile = 0;
  oam_payload.forward_disable = 1;

  
  for (is_ingress = 0; is_ingress <=1; is_ingress++) {
      oam1_key.ingress = is_ingress;
      for (mep_type = 0; mep_type < SOC_PPC_OAM_MP_TYPE_COUNT; mep_type++) {
          oam1_key.mep_type = mep_type;
          for (is_my_cfm_mac = 0; is_my_cfm_mac <=1; is_my_cfm_mac++) {
              oam1_key.my_cfm_mac = is_my_cfm_mac;
              for (mip_type = 0; mip_type < SOC_PPC_OAM_MP_TYPE_COUNT; mip_type++) {
                  oam1_key.mip_type = mip_type;
                  for (internal_opcode = 1; internal_opcode < SOC_PPC_OAM_OPCODE_MAP_COUNT; internal_opcode++) {
                      oam1_key.opcode = internal_opcode;
                      if (profile == ARAD_PP_OAM_PROFILE_PASSIVE) { 
                          if (mep_type == SOC_PPC_OAM_MP_TYPE_ABOVE) {
                              if (mip_type == SOC_PPC_OAM_MP_TYPE_MATCH) { 
                                  
                                  _arad_pp_default_mip_behavior_get (internal_opcode, is_my_cfm_mac, &action);
                              }
                              else { 
                                  action = _ARAD_PP_OAM_ACTION_FRWRD;
                              }
                          }
                          else { 
                              action = _ARAD_PP_OAM_ACTION_TRAP_ERROR_PASSIVE;
                          }
                      }
                      else { 
                          if (mep_type == SOC_PPC_OAM_MP_TYPE_ABOVE) {
                              if (mip_type == SOC_PPC_OAM_MP_TYPE_MATCH) {
                                  
                                  _arad_pp_default_mip_behavior_get (internal_opcode, is_my_cfm_mac, &action);
                              }
                              else { 
                                  action = _ARAD_PP_OAM_ACTION_FRWRD;
                              }
                          }
                          else { 
                              action = _ARAD_PP_OAM_ACTION_TRAP_ERROR_LEVEL;
                          }
                      }
                      oam_payload.forwarding_strength = _ARAD_PP_OAM_TRAP_STRENGTH;
                      switch (action) {
                             case _ARAD_PP_OAM_ACTION_TRAP_ERROR_PASSIVE: {
                              oam_payload.forward_disable = 1;
                              oam_payload.snoop_strength = 0;
                              res = sw_state_access[unit].dpp.soc.arad.pp.oam.mirror_profile_err_passive.get(unit, &oam_mirror_profile);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);
                              oam_payload.mirror_profile = oam_mirror_profile & 0xff;
                              res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_trap_to_cpu_passive.get(unit, &oam_trap_code);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);
                              oam_payload.cpu_trap_code = oam_trap_code;
                              oam_payload.forwarding_strength = _ARAD_PP_OAM_PASSIVE_TRAP_STRENGTH;
                              break;
                          }
                          case _ARAD_PP_OAM_ACTION_TRAP_ERROR_LEVEL: {
                              oam_payload.forward_disable = 1;
                              oam_payload.snoop_strength = 0;
                              res = sw_state_access[unit].dpp.soc.arad.pp.oam.mirror_profile_err_level.get(unit, &oam_mirror_profile);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);
                              oam_payload.mirror_profile = oam_mirror_profile & 0xff;
                              res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_trap_to_cpu_level.get(unit, &oam_trap_code);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);
                              oam_payload.cpu_trap_code = oam_trap_code;
                              oam_payload.forwarding_strength = _ARAD_PP_OAM_LEVEL_TRAP_STRENGTH;
                              break;
                          }

                          
                          
                          case _ARAD_PP_OAM_ACTION_RECYCLE: {
                              oam_payload.forward_disable = 1;
                              oam_payload.snoop_strength = 0;
                              res = sw_state_access[unit].dpp.soc.arad.pp.oam.mirror_profile_recycle.get(unit, &oam_mirror_profile);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);
                              oam_payload.mirror_profile = oam_mirror_profile & 0xff;
                              res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_recycle.get(unit, &oam_trap_code);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);
                              oam_payload.cpu_trap_code = oam_trap_code;
                              break;
                          }
                          case _ARAD_PP_OAM_ACTION_FRWRD: {
                              oam_payload.forward_disable = 0;
                              oam_payload.snoop_strength = 0;
                              oam_payload.mirror_profile = 0;
                              res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_frwrd.get(unit, &oam_trap_code);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);
                              oam_payload.cpu_trap_code = oam_trap_code;
                              oam_payload.forwarding_strength = _ARAD_PP_OAM_FORWARD_TRAP_STRENGTH;
                              break;
                          }
                          case _ARAD_PP_OAM_ACTION_SNOOP: {
                              oam_payload.forward_disable = 0;
                              oam_payload.snoop_strength = _ARAD_PP_OAM_SNOOP_STRENGTH;
                              res = sw_state_access[unit].dpp.soc.arad.pp.oam.mirror_profile_snoop_to_cpu.get(unit, &oam_mirror_profile);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);
                              oam_payload.mirror_profile = oam_mirror_profile & 0xff;
                              res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_snoop.get(unit, &oam_trap_code);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 249, exit);
                              oam_payload.cpu_trap_code = oam_trap_code;
                              break;
                          }
                          case _ARAD_PP_OAM_ACTION_TRAP_CPU:
                          case _ARAD_PP_OAM_ACTION_TRAP_OAMP: {
                              continue;
                          }
                      }
                      oam1_key.mp_profile = profile;

                      res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                      SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
                  }
              }
          }
      }
  }

  res = arad_pp_oam_classifier_oam1_set_hw_unsafe(unit,&oama_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);


exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oama_buffer);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_classifier_profile_reset()",0,0);
}


uint32
  arad_pp_oam_tcam_init_inner_eth(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32 res;
  uint32 mdl = 0, tags = 0, max_tags = 0;
  SOC_PPC_OAM_TCAM_ENTRY_ACTION oam_tcam_entry_action;
  SOC_PPC_OAM_TCAM_ENTRY_KEY oam_tcam_entry_key;
  SOC_PPC_PKT_TERM_TYPE tunnel_termination_code;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  max_tags = 3;
  
  if(SOC_IS_ARADPLUS_AND_BELOW(unit)){
      max_tags = 2;
  }
  
  for (tunnel_termination_code = 0; tunnel_termination_code < SOC_PPC_NOF_PKT_TERM_TYPES; tunnel_termination_code++) {
      if ((tunnel_termination_code == SOC_PPC_PKT_TERM_TYPE_MPLS_ETH) ||
          (tunnel_termination_code == SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH) ||
          (tunnel_termination_code == SOC_PPC_PKT_TERM_TYPE_MPLS3_ETH)) {
         for (tags = 0; tags < max_tags; tags++) {
          
            if(SOC_IS_ARADPLUS_AND_BELOW(unit)){
              for (mdl = 0; mdl <= _ARAD_PP_OAM_MAX_MD_LEVEL; mdl++) {
                
                SOC_PPC_OAM_TCAM_ENTRY_KEY_clear(&oam_tcam_entry_key);
                SOC_PPC_OAM_TCAM_ENTRY_ACTION_clear(&oam_tcam_entry_action);

                
                oam_tcam_entry_key.fwd_code = _ARAD_PP_OAM_FWD_CODE_ETHERNET; 
                oam_tcam_entry_key.ttc = tunnel_termination_code;
                oam_tcam_entry_key.mdl = mdl;
                oam_tcam_entry_key.channel_type = tags;   

                
                oam_tcam_entry_action.is_oam = 1;
                oam_tcam_entry_action.opcode = 1;     
                oam_tcam_entry_action.mdl = mdl;
                oam_tcam_entry_action.type = _ARAD_PP_OAM_FLP_TCAM_TRAP_TYPE_OAM;
                oam_tcam_entry_action.oam_offset = 14 + (tags*4); 
                res = _add_tcam_entry_and_set_wb_var(unit, oam_tcam_entry_key, oam_tcam_entry_action, 0);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
              }
          }
       }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_init_inner_eth()",0,0);
}


uint32
  arad_pp_oam_tcam_init_mpls(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint8                                       is_bfd
  )
{
  uint32 res;
  uint32 opcode;
  SOC_PPC_OAM_TCAM_ENTRY_ACTION oam_tcam_entry_action;
  SOC_PPC_OAM_TCAM_ENTRY_KEY oam_tcam_entry_key;
  uint32 channel_type_ndx, channel_format;
  SOC_PPC_PKT_TERM_TYPE tunnel_termination_code, nof_tunnels;
  uint8 internal_opcode;
  uint8 is_Mpls_ignore_MDL_Jericho = 0;
  SOC_PPC_PKT_TERM_TYPE tunnel_termination_code_for_MDL_Jericho = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  for (tunnel_termination_code = 0; tunnel_termination_code < SOC_PPC_NOF_PKT_TERM_TYPES; tunnel_termination_code++) {
      if ((tunnel_termination_code == SOC_PPC_PKT_TERM_TYPE_MPLS_ETH) ||
          (tunnel_termination_code == SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH) ||
          (tunnel_termination_code == SOC_PPC_PKT_TERM_TYPE_MPLS3_ETH)) {

          nof_tunnels = (tunnel_termination_code == SOC_PPC_PKT_TERM_TYPE_MPLS_ETH) ? 1 :
                        (tunnel_termination_code == SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH) ? 2 : 3;

          for (channel_type_ndx = 0; channel_type_ndx < _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_NOF; channel_type_ndx++) {
              
              SOC_PPC_OAM_TCAM_ENTRY_KEY_clear(&oam_tcam_entry_key);
              SOC_PPC_OAM_TCAM_ENTRY_ACTION_clear(&oam_tcam_entry_action);
              oam_tcam_entry_action.is_oam = 1;
              oam_tcam_entry_action.type = _ARAD_PP_OAM_FLP_TCAM_TRAP_TYPE_OAM;
              oam_tcam_entry_key.mpls_label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_GAL;
              oam_tcam_entry_key.ttc = tunnel_termination_code;
              if (SOC_IS_JERICHO(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0)) {
                  tunnel_termination_code_for_MDL_Jericho = (tunnel_termination_code == SOC_PPC_PKT_TERM_TYPE_MPLS_ETH) ? SOC_PPC_PKT_TERM_TYPE_CW_MPLS_ETH :
                                                            (tunnel_termination_code == SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH) ? SOC_PPC_PKT_TERM_TYPE_CW_MPLS2_ETH : SOC_PPC_PKT_TERM_TYPE_CW_MPLS3_ETH;
                  oam_tcam_entry_key.fwd_code = ARAD_PP_FWD_CODE_TM; 
                  oam_tcam_entry_key.ttc = tunnel_termination_code_for_MDL_Jericho; 
                  is_Mpls_ignore_MDL_Jericho = 1;
                  oam_tcam_entry_action.oam_offset = 0;
                  oam_tcam_entry_action.oam_stamp_offset = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(SOC_PPC_OAM_OPCODE_MAP_CCM, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM);

              } else {
                  oam_tcam_entry_key.fwd_code = _ARAD_PP_OAM_FWD_CODE_MPLS; 
                  if (nof_tunnels == 3) {
                      oam_tcam_entry_action.oam_offset = 8; 
                    oam_tcam_entry_action.oam_stamp_offset = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(SOC_PPC_OAM_OPCODE_MAP_CCM, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM) + 8; 
                  } else {
                      oam_tcam_entry_action.oam_offset = 8 + nof_tunnels * 4; 
                    oam_tcam_entry_action.oam_stamp_offset = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(SOC_PPC_OAM_OPCODE_MAP_CCM, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM) + 8 + nof_tunnels * 4;
                  }
              }
              oam_tcam_entry_key.channel_type = _arad_pp_oam_mpls_tp_channel_type_enum_to_channel_type(unit, channel_type_ndx);
              if (oam_tcam_entry_key.channel_type != 0) {
                  channel_format = _arad_pp_oam_mpls_tp_channel_type_enum_to_format(channel_type_ndx);
                  if (is_bfd) {
                      if (channel_format == _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_BFD_RAW) {
                          oam_tcam_entry_key.mdl = 0;
                          oam_tcam_entry_key.opcode = 0;
                          oam_tcam_entry_action.is_bfd = 1;
                          oam_tcam_entry_action.opcode = 0; 
                          oam_tcam_entry_action.mdl = 0;
                          res = _add_tcam_entry_and_set_wb_var(unit, oam_tcam_entry_key, oam_tcam_entry_action, is_Mpls_ignore_MDL_Jericho);
                          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
                      }
                  } else { 
                      if (channel_format == _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_Y1731 || channel_format == _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_OAM_UNKNOWN) {
                          oam_tcam_entry_key.mdl = _ARAD_PP_OAM_Y1731_DEFAULT_MDLEVEL;
                          oam_tcam_entry_action.is_bfd = 0;
                          oam_tcam_entry_action.mdl = _ARAD_PP_OAM_Y1731_DEFAULT_MDLEVEL;
                          if (channel_format == _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_Y1731) {
                              for (opcode = 0; opcode < SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
                                  if ((_ARAD_PP_OAM_MPLS_TCAM_SUPPORTED_OPCODE(opcode)) || (opcode == (SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT - 1))) {
                                      oam_tcam_entry_key.opcode = opcode;
                                      res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, opcode, &internal_opcode);
                                      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
                                      oam_tcam_entry_action.opcode = (uint32)internal_opcode;
                                      if (SOC_IS_JERICHO(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0)) {
                                          oam_tcam_entry_action.oam_stamp_offset = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(internal_opcode, opcode);
                                      } else {
                                          if (nof_tunnels == 3) {
                                              oam_tcam_entry_action.oam_stamp_offset = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(internal_opcode, opcode) + 8;
                                          } else {
                                              oam_tcam_entry_action.oam_stamp_offset = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(internal_opcode, opcode) + 8 + nof_tunnels * 4;
                                          }
                                      }
                                      res = _add_tcam_entry_and_set_wb_var(unit, oam_tcam_entry_key, oam_tcam_entry_action, is_Mpls_ignore_MDL_Jericho);
                                      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
                                  }
                              }
                          } else {
                             
                              res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM, &internal_opcode);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
                              oam_tcam_entry_key.opcode = SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM;
                              oam_tcam_entry_action.opcode = (uint32)internal_opcode;
                              res = _add_tcam_entry_and_set_wb_var(unit, oam_tcam_entry_key, oam_tcam_entry_action, is_Mpls_ignore_MDL_Jericho);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
                          }
                      }
                  }
              }
          }
      }
  }

  if (is_bfd && SOC_DPP_CONFIG(unit)->pp.bfd_mpls_lsp_support ) {
      res = arad_pp_oam_tcam_bfd_lsp_ipv4_entry_add_unsafe(unit);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_init_mpls()",0,0);
}


uint32
  arad_pp_oam_tcam_init_mim(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32 res;
  uint8  mdl;
  uint32 opcode;
  SOC_PPC_OAM_TCAM_ENTRY_ACTION oam_tcam_entry_action;
  SOC_PPC_OAM_TCAM_ENTRY_KEY oam_tcam_entry_key;
  uint8 internal_opcode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  for (mdl = 0; mdl < _ARAD_PP_OAM_MAX_MD_LEVEL; mdl++) {
      for (opcode=0; opcode < SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
          if ((_ARAD_PP_OAM_MIM_TCAM_SUPPORTED_OPCODE(opcode)) || (opcode==(SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT-1))) {
              
              SOC_PPC_OAM_TCAM_ENTRY_KEY_clear(&oam_tcam_entry_key);
              oam_tcam_entry_key.ttc = SOC_PPC_NOF_PKT_TERM_TYPES;
              oam_tcam_entry_key.fwd_code = _ARAD_PP_OAM_FWD_CODE_ETHERNET; 
              oam_tcam_entry_key.mdl = mdl;
              oam_tcam_entry_key.opcode = opcode;

              SOC_PPC_OAM_TCAM_ENTRY_ACTION_clear(&oam_tcam_entry_action);
              oam_tcam_entry_action.is_oam = 1;
              oam_tcam_entry_action.type = _ARAD_PP_OAM_FLP_TCAM_TRAP_TYPE_OAM;
              res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, opcode, &internal_opcode);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
              oam_tcam_entry_action.opcode = (uint32)internal_opcode;
              oam_tcam_entry_action.mdl = mdl;
              oam_tcam_entry_action.oam_lif_tcam_result = 0;
              oam_tcam_entry_action.oam_lif_tcam_result_valid = 0;
              oam_tcam_entry_action.oam_offset = 14;
              oam_tcam_entry_action.oam_pcp = 0;
              oam_tcam_entry_action.oam_stamp_offset = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(internal_opcode, opcode);
              if ((opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LTM) ||
                 (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBM) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBR) ||
                 (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LTR)) {

                  oam_tcam_entry_action.my_cfm_mac = 0;
              }
              else {
                  oam_tcam_entry_action.my_cfm_mac = 1;
              }

              res = _add_tcam_entry_and_set_wb_var(unit, oam_tcam_entry_key, oam_tcam_entry_action, 0);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
          }
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_init_mim()",0,0);
}




uint32
  arad_pp_oam_tcam_bfd_lsp_ipv4_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32 res;
  SOC_PPC_OAM_TCAM_ENTRY_ACTION oam_tcam_entry_action;
  SOC_PPC_OAM_TCAM_ENTRY_KEY oam_tcam_entry_key;
  uint32 tcam_entry_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  SOC_PPC_OAM_TCAM_ENTRY_KEY_clear(&oam_tcam_entry_key);
  oam_tcam_entry_key.ttc = SOC_PPC_PKT_TERM_TYPE_MPLS_ETH;
  oam_tcam_entry_key.fwd_code = _ARAD_PP_OAM_FWD_CODE_IPV4_UC;
  oam_tcam_entry_key.mdl = 0;
  oam_tcam_entry_key.opcode = 0;
  oam_tcam_entry_key.your_disc = 0;

  SOC_PPC_OAM_TCAM_ENTRY_ACTION_clear(&oam_tcam_entry_action);
  oam_tcam_entry_action.is_oam = 1;
  oam_tcam_entry_action.is_bfd = 1;
  oam_tcam_entry_action.your_discr = 0;
  oam_tcam_entry_action.type = _ARAD_PP_OAM_FLP_TCAM_TRAP_TYPE_OAM;
  oam_tcam_entry_action.opcode = 0;
  oam_tcam_entry_action.mdl = 0;
  oam_tcam_entry_action.oam_lif_tcam_result = 0;
  oam_tcam_entry_action.oam_lif_tcam_result_valid = 0;
  oam_tcam_entry_action.oam_offset = 20+8; 
  oam_tcam_entry_action.oam_pcp = 0;
  oam_tcam_entry_action.oam_stamp_offset = 20+8 + _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(oam_tcam_entry_action.opcode,  oam_tcam_entry_action.opcode); 
  oam_tcam_entry_action.my_cfm_mac = 0;

  res = sw_state_access[unit].dpp.soc.arad.pp.oam.tcam_last_entry_id.get(unit, &tcam_entry_id);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);


  if (tcam_entry_id > _ARAD_PP_OAM_TCAM_MAX_ENTRY_ID) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("Maximum number of TCAM entries exceeded.")));
  }

  res =  _arad_pp_oam_tcam_new_entry(unit,
                                     &oam_tcam_entry_key,
                                     &oam_tcam_entry_action,
                                     tcam_entry_id,
                                     0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);

  ++tcam_entry_id;
  res = sw_state_access[unit].dpp.soc.arad.pp.oam.tcam_last_entry_id.set(unit, tcam_entry_id);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 13, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_bfd_entry_add()",0,0);
}


uint32
  arad_pp_oam_tcam_bfd_lsp_ipv4_entry_delete_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 tcam_entry_index

  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  res = arad_tcam_managed_db_entry_remove_unsafe(unit, ARAD_PP_TCAM_STATIC_ACCESS_ID_OAM_IDENTIFICATION, tcam_entry_index);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_bfd_entry_delete()",0,0);
}



uint32
  arad_pp_oam_tcam_bfd_lsp_special_ttl_ipv4_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32 res;
  SOC_PPC_OAM_TCAM_ENTRY_ACTION oam_tcam_entry_action;
  SOC_PPC_OAM_TCAM_ENTRY_KEY oam_tcam_entry_key;
  uint32 tcam_entry_id;
  int ttl_handle[1] = {255};
  int i = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (i = 0; i < 1; i++) {
    
    SOC_PPC_OAM_TCAM_ENTRY_KEY_clear(&oam_tcam_entry_key);
    oam_tcam_entry_key.ttc = SOC_PPC_PKT_TERM_TYPE_MPLS_ETH;
    oam_tcam_entry_key.fwd_code = _ARAD_PP_OAM_FWD_CODE_IPV4_UC;
    oam_tcam_entry_key.mdl = 0;
    oam_tcam_entry_key.opcode = 0;
    oam_tcam_entry_key.your_disc = 0;
    oam_tcam_entry_key.ip_ttl = ttl_handle[i];

    SOC_PPC_OAM_TCAM_ENTRY_ACTION_clear(&oam_tcam_entry_action);
    oam_tcam_entry_action.is_oam = 1;
    oam_tcam_entry_action.is_bfd = 1;
    oam_tcam_entry_action.your_discr = 0;
    oam_tcam_entry_action.type = _ARAD_PP_OAM_FLP_TCAM_TRAP_TYPE_OAM;
    oam_tcam_entry_action.opcode = 0;
    oam_tcam_entry_action.mdl = 0;
    oam_tcam_entry_action.oam_lif_tcam_result = 0;
    oam_tcam_entry_action.oam_lif_tcam_result_valid = 0;
    oam_tcam_entry_action.oam_offset = 20+8; 
    oam_tcam_entry_action.oam_pcp = 0;
    oam_tcam_entry_action.oam_stamp_offset = 20+8 + _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(oam_tcam_entry_action.opcode,  oam_tcam_entry_action.opcode); 
    oam_tcam_entry_action.my_cfm_mac = 0;

    res = sw_state_access[unit].dpp.soc.arad.pp.oam.tcam_last_entry_id.get(unit, &tcam_entry_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);


    if (tcam_entry_id > _ARAD_PP_OAM_TCAM_MAX_ENTRY_ID) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("Maximum number of TCAM entries exceeded.")));
    }

    res =  _arad_pp_oam_tcam_new_entry(unit,
                                       &oam_tcam_entry_key,
                                       &oam_tcam_entry_action,
                                       tcam_entry_id,
                                       _ARAD_PP_OAM_TCAM_FLAGS_BFD_OVER_MPLS_SPECIAL_TTL);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);

    ++tcam_entry_id;
    res = sw_state_access[unit].dpp.soc.arad.pp.oam.tcam_last_entry_id.set(unit, tcam_entry_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 13, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_bfd_entry_add()",0,0);
}


uint32
  arad_pp_oam_tcam_bfd_lsp_special_ttl_ipv4_entry_delete_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 tcam_entry_index

  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  res = arad_tcam_managed_db_entry_remove_unsafe(unit, ARAD_PP_TCAM_STATIC_ACCESS_ID_OAM_IDENTIFICATION, tcam_entry_index);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_bfd_entry_delete()",0,0);
}


uint32
  arad_pp_oam_tcam_y1711_entry_add_unsafe(
    SOC_SAND_IN  int    unit
  )
{
  uint32 res;
  SOC_PPC_OAM_TCAM_ENTRY_ACTION oam_tcam_entry_action;
  SOC_PPC_OAM_TCAM_ENTRY_KEY oam_tcam_entry_key;
  uint32 tcam_entry_id;
  uint8 internal_opcode;
  int i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  
  for (i=0; i < 2; i++){

      
      if (i==0) {
          res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM, &internal_opcode); 
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
      } else {
          res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMR, &internal_opcode); 
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
      }
      
      
      SOC_PPC_OAM_TCAM_ENTRY_KEY_clear(&oam_tcam_entry_key);
      oam_tcam_entry_key.fwd_code = _ARAD_PP_OAM_FWD_CODE_MPLS;
      oam_tcam_entry_key.mdl = 0;
      oam_tcam_entry_key.opcode = 0;
      oam_tcam_entry_key.your_disc = i; 


      SOC_PPC_OAM_TCAM_ENTRY_ACTION_clear(&oam_tcam_entry_action);
      oam_tcam_entry_action.is_oam = 1;
      oam_tcam_entry_action.is_bfd = 0;
      oam_tcam_entry_action.your_discr = 0; 
      oam_tcam_entry_action.type = _ARAD_PP_OAM_FLP_TCAM_TRAP_TYPE_OAM;
      oam_tcam_entry_action.opcode = internal_opcode; 
      oam_tcam_entry_action.mdl = 7;
      oam_tcam_entry_action.oam_lif_tcam_result = 0; 
      oam_tcam_entry_action.oam_lif_tcam_result_valid = 0;
      oam_tcam_entry_action.oam_offset = 4;
      oam_tcam_entry_action.oam_pcp = 0;
      oam_tcam_entry_action.oam_stamp_offset = 1;
      oam_tcam_entry_action.my_cfm_mac = 0;

      res = sw_state_access[unit].dpp.soc.arad.pp.oam.tcam_last_entry_id.get(unit, &tcam_entry_id);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);


      if (tcam_entry_id > _ARAD_PP_OAM_TCAM_MAX_ENTRY_ID) {
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("Maximum number of TCAM entries exceeded.")));
      }

      res =  _arad_pp_oam_tcam_new_entry(unit,
                                         &oam_tcam_entry_key,
                                         &oam_tcam_entry_action,
                                         tcam_entry_id,
                                         _ARAD_PP_OAM_TCAM_FLAGS_Y1711);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);

      ++tcam_entry_id;
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.tcam_last_entry_id.set(unit, tcam_entry_id);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 13, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_y1711_entry_add_unsafe",0,0);
}


uint32
  arad_pp_oam_tcam_y1711_lm_entry_add_unsafe(
    SOC_SAND_IN  int    unit
  )
{
  uint32 res;
  SOC_PPC_OAM_TCAM_ENTRY_ACTION oam_tcam_entry_action;
  SOC_PPC_OAM_TCAM_ENTRY_KEY oam_tcam_entry_key;
  uint32 tcam_entry_id;
  uint8 internal_opcode;
  int type_index;
  int lm_index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  for(type_index=0; type_index < 2; type_index++){
    for (lm_index=0; lm_index < 2; lm_index++){   

      
      if (lm_index==0) {
          res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM, &internal_opcode); 
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
      } else {
          res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMR, &internal_opcode); 
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
      }
      
      
      SOC_PPC_OAM_TCAM_ENTRY_KEY_clear(&oam_tcam_entry_key);
      oam_tcam_entry_key.fwd_code = (type_index == 0) ?_ARAD_PP_OAM_FWD_CODE_MPLS : _ARAD_PP_OAM_FWD_CODE_ETHERNET;
      oam_tcam_entry_key.mdl = 0;
      oam_tcam_entry_key.opcode = 0;
      oam_tcam_entry_key.your_disc = lm_index; 


      SOC_PPC_OAM_TCAM_ENTRY_ACTION_clear(&oam_tcam_entry_action);
      oam_tcam_entry_action.is_oam = 1;
      oam_tcam_entry_action.is_bfd = 0;
      oam_tcam_entry_action.your_discr = 0; 
      oam_tcam_entry_action.type = _ARAD_PP_OAM_FLP_TCAM_TRAP_TYPE_OAM;
      oam_tcam_entry_action.opcode = internal_opcode; 
      oam_tcam_entry_action.mdl = 0;
      oam_tcam_entry_action.oam_lif_tcam_result = 0; 
      oam_tcam_entry_action.oam_lif_tcam_result_valid = 0;
      oam_tcam_entry_action.oam_offset = 4;
      oam_tcam_entry_action.oam_pcp = 0;
      oam_tcam_entry_action.oam_stamp_offset = 1;
      oam_tcam_entry_action.my_cfm_mac = 0;

      res = sw_state_access[unit].dpp.soc.arad.pp.oam.tcam_last_entry_id.get(unit, &tcam_entry_id);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);


      if (tcam_entry_id > _ARAD_PP_OAM_TCAM_MAX_ENTRY_ID) {
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("Maximum number of TCAM entries exceeded.")));
      }

      res =  _arad_pp_oam_tcam_new_entry(unit,
                                         &oam_tcam_entry_key,
                                         &oam_tcam_entry_action,
                                         tcam_entry_id,
                                         _ARAD_PP_OAM_TCAM_FLAGS_Y1711);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);

      ++tcam_entry_id;
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.tcam_last_entry_id.set(unit, tcam_entry_id);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 13, exit);
  }
}
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_tcam_y1711_entry_add_unsafe",0,0);
}


uint32
  arad_pp_oam_default_simple_in_lif_profile_mapping_set_unsafe(
    SOC_SAND_IN  int                                 unit
    ) {
  uint32 res;
  uint32 inlif_profile;
  uint32 oam_trap_profile = 0;
  uint32 inlif_oam_map = 0;
  int core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (inlif_profile = 0; inlif_profile < (1<<SOC_OCC_MGMT_INLIF_SIZE); inlif_profile++) {
      
      res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_OAM,
                                 &inlif_profile, &oam_trap_profile));
      SOC_SAND_IF_ERR_EXIT(res);
      inlif_oam_map |= (oam_trap_profile << (2*inlif_profile));
  }

  ARAD_DEVICE_CHECK(unit, exit);
  
  if (SOC_IS_JERICHO(unit)) {
      SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_IHP_OAM_IN_LIF_PROFILE_MAPr(unit, core, inlif_oam_map));
      }
  }
  else {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_IHB_OAM_IN_LIF_PROFILE_MAPr(unit, inlif_oam_map));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_oam_default_simple_in_lif_profile_mapping_set_unsafe()",0,0);
}





STATIC uint32
  arad_pp_oam_classifier_init_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8               is_bfd,
    SOC_SAND_IN  uint8               is_second_init
  )
{
  uint32 res, reg, entry, field, port_index, profile, mirror_trap_code, mep_index;
  uint32 mapped_fap_id, mapped_fap_port_id, channel_type_ndx, channel_format, oam_map_channel_cam_entry;
  uint32 oam_trap_code, channel_type, opcode, egress_count=0, ingress_count=0, counter_proc;
  uint64 reg_64, field_64;
  soc_reg_above_64_val_t reg_above_64;
  uint8 internal_opcode, tdm_bugfix;
  bcm_gport_t recycle_port;
  int core, block;
  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO profile_info;
  SOC_PPC_TRAP_CODE trap_code;
  soc_mem_t
      pinfo_flp_mem = SOC_IS_JERICHO(unit) ? IHP_PINFO_FLP_0m : IHB_PINFO_FLPm,
      epni_pp_pct_mem = SOC_IS_JERICHO_PLUS(unit) ? EPNI_PACKETPROCESSING_PORT_CONFIGURATION_TABLEm : EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_INIT_UNSAFE);
  COMPILER_64_ZERO(reg_64);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHP_OAM_IDENTIFICATION_ENABLEr(unit, 0, &reg));
  if (is_bfd) {
      soc_reg_field_set(unit, IHP_OAM_IDENTIFICATION_ENABLEr, &reg, OAM_MULTI_HOP_BFD_ENABLEf, 1);
      if (SOC_IS_JERICHO(unit)) {
                soc_reg_field_set(unit, IHP_OAM_IDENTIFICATION_ENABLEr, &reg, OAM_ONE_HOP_BFD_ENABLEf, 1);
      } else {
          soc_reg_field_set(unit, IHP_OAM_IDENTIFICATION_ENABLEr, &reg, OAM_ONE_HOP_BFD_ENABLEf,
                            !SOC_DPP_CONFIG(unit)->pp.bfd_mpls_lsp_support);
      }

      soc_reg_field_set(unit, IHP_OAM_IDENTIFICATION_ENABLEr, &reg, OAM_MPLS_TP_OR_BFD_ENABLE_MAPf, 0xff);
      soc_reg_field_set(unit, IHP_OAM_IDENTIFICATION_ENABLEr, &reg, OAM_BFD_VALIDITY_CHECKf, 1);
  }
  else { 
      soc_reg_field_set(unit, IHP_OAM_IDENTIFICATION_ENABLEr, &reg, OAM_ETHERNET_ENABLEf, 1);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_IHP_OAM_IDENTIFICATION_ENABLEr(unit, SOC_CORE_ALL, reg));

  recycle_port = soc_property_get(unit, spn_OAM_RCY_PORT, -1);

  
  if (is_bfd) {
      res = _arad_pp_oam_trap_code_to_internal(unit,SOC_PPC_TRAP_CODE_USER_DEFINED_DROP_TRAP, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_drop.set(unit, oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      res = _arad_pp_oam_trap_code_to_internal(unit,SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_frwrd.set(unit, oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      res = sw_state_access[unit].dpp.bcm.bfd.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_CPU, &trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.bfd.trap_code_trap_to_cpu.set(unit, oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      res = sw_state_access[unit].dpp.bcm.bfd.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_OAMP_CC_MPLS_TP, &trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.bfd.trap_code_oamp_bfd_cc_mpls_tp.set(unit, oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      res = sw_state_access[unit].dpp.bcm.bfd.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_OAMP_IPV4, &trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.bfd.trap_code_oamp_bfd_ipv4.set(unit, oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      res = sw_state_access[unit].dpp.bcm.bfd.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_OAMP_MPLS, &trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.bfd.trap_code_oamp_bfd_mpls.set(unit, oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      res = sw_state_access[unit].dpp.bcm.bfd.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_OAMP_PWE, &trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.bfd.trap_code_oamp_bfd_pwe.set(unit, oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 214, exit);

      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 160, exit, READ_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, 0, &reg_64));
      COMPILER_64_SET(field_64,0,0xFFFF);
      soc_reg64_field_set(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, &reg_64, BFD_INVALID_YOUR_DISCR_LIFf, field_64);
      if(SOC_IS_JERICHO_PLUS(unit)) {
          COMPILER_64_SET(field_64,0,0x0);
          soc_reg64_field_set(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, &reg_64, BFD_NULL_YOUR_DISCR_LIFf, field_64);
      }
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 170, exit, WRITE_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, SOC_CORE_ALL, reg_64));

      if (ARAD_PP_OAM_BFD_SEAMLESS_BFD_FEATURE_ENABLED(unit)) {
          
          COMPILER_64_SET(field_64,0,7784);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, IHP_OAM_BFD_OVER_IP_CFGr, REG_PORT_ANY, 0, UDP_SRC_PORT_BFD_RANGE_MINf, field_64));
      } 

      COMPILER_64_SET(field_64,0,0xec8);
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, IHP_OAM_BFD_OVER_IP_CFGr, REG_PORT_ANY, 0, UDP_DST_PORT_BFD_ONE_HOPf, field_64));

  }
  else { 
      if(recycle_port != -1) {
          
          SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&profile_info);
          res = sw_state_access[unit].dpp.bcm.oam.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_RECYCLE, &trap_code);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
          res = soc_ppd_trap_frwrd_profile_info_get(unit,  trap_code, &profile_info);
          SOC_SAND_CHECK_FUNC_RESULT(res, 215, exit);
          if (profile_info.dest_info.frwrd_dest.type != SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT) {
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 220, exit);
          }
          res = arad_sys_phys_to_local_port_map_get_unsafe(unit, profile_info.dest_info.frwrd_dest.dest_id, &mapped_fap_id, &mapped_fap_port_id);
          SOC_SAND_CHECK_FUNC_RESULT(res, 225, exit);
          SOC_REG_ABOVE_64_CLEAR(reg_above_64);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit,
                                       soc_mem_read(unit, epni_pp_pct_mem, MEM_BLOCK_ANY, mapped_fap_port_id, &reg_above_64));
          reg = ARAD_PRGE_PP_SELECT_ETH_OAM_LOOPBACK; 
          soc_mem_field_set(unit, epni_pp_pct_mem, (uint32 *)&reg_above_64, PRGE_PROFILEf, &reg);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 235, exit,
                                       soc_mem_write(unit, epni_pp_pct_mem, MEM_BLOCK_ANY, mapped_fap_port_id, &reg_above_64));

          res = sw_state_access[unit].dpp.bcm.oam.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_RECYCLE, &trap_code);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
          res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &oam_trap_code);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
          res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_recycle.set(unit, oam_trap_code);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      }

      res = _arad_pp_oam_trap_code_to_internal(unit,SOC_PPC_TRAP_CODE_USER_DEFINED_DROP_TRAP, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_drop.set(unit, oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      res = _arad_pp_oam_trap_code_to_internal(unit,SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_frwrd.set(unit, oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      res = sw_state_access[unit].dpp.bcm.oam.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_SNOOP, &trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_snoop.set(unit, oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      res = sw_state_access[unit].dpp.bcm.oam.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_ERR_LEVEL, &trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_trap_to_cpu_level.set(unit, oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      res = sw_state_access[unit].dpp.bcm.oam.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_ERR_PASSIVE, &trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_trap_to_cpu_passive.set(unit, oam_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);

      
      res = sw_state_access[unit].dpp.bcm.oam.trap_info.upmep_trap_ids.get(unit, SOC_PPC_OAM_UPMEP_TRAP_ID_CPU, &trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &mirror_trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

      
      res = sw_state_access[unit].dpp.bcm.oam.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_RECYCLE, &trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.mirror_profile_recycle.set(unit, trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);

      res = sw_state_access[unit].dpp.bcm.oam.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_SNOOP, &trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.mirror_profile_snoop_to_cpu.set(unit, trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);

      res = sw_state_access[unit].dpp.bcm.oam.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_ERR_LEVEL, &trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.mirror_profile_err_level.set(unit, trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);

      res = sw_state_access[unit].dpp.bcm.oam.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_ERR_PASSIVE, &trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.mirror_profile_err_passive.set(unit, trap_code);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 247, exit);

      if (SOC_IS_ARADPLUS(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, READ_EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r(unit, REG_PORT_ANY, &reg));
          tdm_bugfix = !(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_tdm_bugfix", 0));
          soc_reg_field_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r, &reg, CFG_BUG_FIX_87_DISABLEf, tdm_bugfix);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_1r(unit, REG_PORT_ANY, reg));
      }
  }

  if (!is_second_init) {
      
      for (opcode = 0; opcode < SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT;  opcode++) {
          switch (opcode) {
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_CCM;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBR:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_LBR;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBM:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_LBM;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LTR:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_LTR;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LTM:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_LTM;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_AIS:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_AIS;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LCK:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_LCK;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LINEAR_APS:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_LINEAR_APS;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMR:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_LMR;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_LMM;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_1DM:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_1DM;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMR:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_DMR;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMM:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_DMM;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_SLM;
              break;
          case SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLR:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_SLR;
              break;
          case SOC_PPC_BFD_PDU_OPCODE:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_BFD;
              break;
          default:
              internal_opcode = SOC_PPC_OAM_OPCODE_MAP_DEFAULT;
          }
          res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.set(unit, opcode, internal_opcode);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      }

     if((SOC_IS_JERICHO(unit) && (!SOC_IS_QAX(unit))) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_ccm_pkt_counting", 0) == 1)){
         res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.set(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_PSEUDO_CCM, SOC_PPC_OAM_OPCODE_MAP_LINEAR_APS);
         SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 236, exit);
      }
      
      res = arad_pp_oam_eth_oam_opcode_map_set_unsafe(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 236, exit);

      
      oam_map_channel_cam_entry = 0;
      for (channel_type_ndx = 0; channel_type_ndx < _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_NOF; channel_type_ndx++) {
          channel_type = _arad_pp_oam_mpls_tp_channel_type_enum_to_channel_type(unit, channel_type_ndx);
          channel_format = _arad_pp_oam_mpls_tp_channel_type_enum_to_format(channel_type_ndx);
          switch (channel_format) {
          case _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_Y1731:
          case _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_BFD_RAW:
          case _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_BFD_IPV4:
              entry = 0;
              soc_IHP_OAM_CHANNEL_TYPEm_field_set(unit, &entry, CHANNEL_TYPEf, &channel_type);
              field = 1;
              soc_IHP_OAM_CHANNEL_TYPEm_field_set(unit, &entry, VALIDf, &field);
              SHR_BITCOPY_RANGE(&entry, 17, &channel_format, 0, 3); 
              
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_IHP_OAM_CHANNEL_TYPEm(unit, MEM_BLOCK_ANY, oam_map_channel_cam_entry, &entry));
              oam_map_channel_cam_entry++;
              break;
          case _ARAD_PP_OAM_MPLS_TP_FORMAT_TYPE_RFC6374:
              entry = 0;
              soc_IHP_OAM_CHANNEL_TYPEm_field_set(unit, &entry, CHANNEL_TYPEf, &channel_type);
              field = 1;
              soc_IHP_OAM_CHANNEL_TYPEm_field_set(unit, &entry, VALIDf, &field);
              
              SHR_BITCOPY_RANGE(&entry, 17, &channel_format, 0, 3);
              
              field = _arad_pp_oam_mpls_tp_channel_type_enum_to_internal_opcode(unit, channel_type_ndx);
              SHR_BITCOPY_RANGE(&entry, 20, &field, 0, 4);
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_IHP_OAM_CHANNEL_TYPEm(unit, MEM_BLOCK_ANY, oam_map_channel_cam_entry, &entry));
              oam_map_channel_cam_entry++;
              break;
          default:
              break;
          }
      }

      
      res = arad_pp_oam_tcam_database_create(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = arad_pp_oam_counter_range_set_unsafe(unit, 0, 0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      
      if (SOC_IS_ARADPLUS_A0(unit)) { 
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHP_FLP_GENERAL_CFGr_REG64(unit, 0, &reg_64));
          COMPILER_64_SET(field_64, 0, SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit));
          soc_reg64_field_set(unit, IHP_FLP_GENERAL_CFGr, &reg_64, OAM_MP_TYPE_DECODING_ENABLEf, field_64);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IHP_FLP_GENERAL_CFGr_REG64(unit, SOC_CORE_ALL, reg_64));
      }

      
      SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
          block = SOC_IS_JERICHO(unit) ? IHP_BLOCK(unit, core) : MEM_BLOCK_ANY;
          for (port_index = 0; port_index < ARAD_PORT_NOF_PP_PORTS; port_index++) {
              res = soc_mem_read(unit, pinfo_flp_mem, block, port_index, &entry);
              SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);
              soc_mem_field32_set(unit, pinfo_flp_mem, &entry, ENABLE_PP_INJECTf, 1);
              res = soc_mem_write(unit, pinfo_flp_mem, block, port_index, &entry);
              SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
          }
      }

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, EPNI_OAM_INVALID_LIFr, SOC_CORE_ALL, 0, OAM_INVALID_LIFf,  0xffff));

      COMPILER_64_ZERO(reg_64);
      COMPILER_64_SET(field_64, 0, 11);
      soc_reg64_field_set(unit, IHP_OAM_ETHERNET_CFGr, &reg_64, ETHERNET_NEXT_PROTOCOL_OAM_0f, field_64);
      if (SOC_IS_JERICHO(unit)) { 
          soc_reg64_field_set(unit, IHP_OAM_ETHERNET_CFGr, &reg_64, ETHERNET_NEXT_PROTOCOL_OAM_1f, field_64);
          soc_reg64_field_set(unit, IHP_OAM_ETHERNET_CFGr, &reg_64, ETHERNET_NEXT_PROTOCOL_OAM_2f, field_64);
          soc_reg64_field_set(unit, IHP_OAM_ETHERNET_CFGr, &reg_64, ETHERNET_NEXT_PROTOCOL_OAM_3f, field_64);
      }
      COMPILER_64_SET(field_64, 0, SOC_DPP_INVALID_GLOBAL_LIF(unit));
      soc_reg64_field_set(unit, IHP_OAM_ETHERNET_CFGr, &reg_64, OAM_LIF_INVALID_HEADERf, field_64);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_IHP_OAM_ETHERNET_CFGr(unit,SOC_CORE_ALL, reg_64));

      reg = 0;
      soc_reg_field_set(unit, IHP_OAM_COUNTER_IN_RANGE_CFGr, &reg, OAM_COUNTER_IN_RANGE_CFG_BASEf, 0);
      soc_reg_field_set(unit, IHP_OAM_COUNTER_IN_RANGE_CFGr, &reg, OAM_COUNTER_IN_RANGE_CFG_SHIFTf, 0);
      soc_reg_field_set(unit, IHP_OAM_COUNTER_IN_RANGE_CFGr, &reg, OAM_COUNTER_IN_RANGE_CFG_ADD_PCPf, 0);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_IHP_OAM_COUNTER_IN_RANGE_CFGr(unit, SOC_CORE_ALL, reg));

      if (SOC_IS_JERICHO(unit)) {
          
          reg = 0;
          soc_reg_field_set(unit, IHP_OAM_COUNTER_NOT_IN_RANGE_CFGr, &reg, OAM_COUNTER_NOT_IN_RANGE_CFG_BASEf, 0);
          
          soc_reg_field_set(unit, IHP_OAM_COUNTER_NOT_IN_RANGE_CFGr, &reg, OAM_COUNTER_NOT_IN_RANGE_CFG_SHIFTf, 0);
          soc_reg_field_set(unit, IHP_OAM_COUNTER_NOT_IN_RANGE_CFGr, &reg, OAM_COUNTER_NOT_IN_RANGE_CFG_ADD_PCPf, 1);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_IHP_OAM_COUNTER_NOT_IN_RANGE_CFGr(unit, SOC_CORE_ALL, reg));
      }

      COMPILER_64_ZERO(reg_64);
      
      if (SOC_IS_JERICHO_PLUS(unit)) {
         soc_reg64_field32_set(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, &reg_64, OAM_COUNTER_INCREMENTf, 0xbd);
      } else {
         soc_reg64_field32_set(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, &reg_64, OAM_COUNTER_INCREMENTf, 0x3d);
      }
      
      if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable",0) == 1){
		  soc_reg64_field32_set(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, &reg_64, OAM_COUNTER_INCREMENTf, 0x3f);
      }

      
      if((SOC_IS_JERICHO(unit) && (!SOC_IS_QAX(unit))) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_ccm_pkt_counting",0) == 1)){
          soc_reg64_field32_set(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, &reg_64, OAM_COUNTER_INCREMENTf, 0xbd);
      }

      
      if (SOC_IS_JERICHO_AND_BELOW(unit)) {
          soc_reg64_field32_set(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, &reg_64, OAM_EGRESS_SUB_TYPEf,
                                SOC_IS_JERICHO(unit) ? _ARAD_PP_OAM_SUBTYPE_DEFAULT_EGRESS_OAM : 0);
      }

      if (SOC_IS_QUX(unit)) {
#define DISABLE_COUNT_FOR_SUBTYPE(counter_bmp,subtype) (counter_bmp &= ~(1 << subtype))
          uint32 counter_incr_bmp_val = 0;
          soc_dpp_config_pp_t *dpp_pp;
          uint8 qux_slm_lm_mode; 
          uint8 qux_dm_1588_counting_enable;
          uint8 qux_dm_ntp_counting_enable;
          uint8 qux_ccm_counting_enable; 
          uint8 qux_lb_counting_enable; 


          
          counter_incr_bmp_val = 0xbd;
          dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

          qux_slm_lm_mode = dpp_pp->qux_slm_lm_mode;
          qux_dm_1588_counting_enable = dpp_pp->qux_dm_1588_counting_enable;
          qux_dm_ntp_counting_enable = dpp_pp->qux_dm_ntp_counting_enable;
          qux_ccm_counting_enable = dpp_pp->qux_ccm_counting_enable;
          qux_lb_counting_enable = dpp_pp->qux_lb_counting_enable;


          
          if (qux_slm_lm_mode == 1) {
              DISABLE_COUNT_FOR_SUBTYPE(counter_incr_bmp_val,_ARAD_PP_OAM_SUBTYPE_DEFAULT);
          }
          
          if (!qux_ccm_counting_enable) {
              DISABLE_COUNT_FOR_SUBTYPE(counter_incr_bmp_val,_ARAD_PP_OAM_SUBTYPE_CCM);
          }
          
          if (!qux_dm_1588_counting_enable) {
              DISABLE_COUNT_FOR_SUBTYPE(counter_incr_bmp_val,_ARAD_PP_OAM_SUBTYPE_DM_1588);
          }
          
          if (!qux_dm_ntp_counting_enable) {
              DISABLE_COUNT_FOR_SUBTYPE(counter_incr_bmp_val,_ARAD_PP_OAM_SUBTYPE_DM_NTP);
          }
          
          if (!qux_lb_counting_enable) {
              DISABLE_COUNT_FOR_SUBTYPE(counter_incr_bmp_val,_ARAD_PP_OAM_SUBTYPE_ETH_LOOPBACK);
          }
          soc_reg64_field32_set(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, &reg_64, 
                  OAM_COUNTER_INCREMENTf, counter_incr_bmp_val);
      }

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, soc_reg_set(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, SOC_CORE_ALL, 0, reg_64));

      if (SOC_IS_QUX(unit)) {
          SOC_REG_ABOVE_64_CLEAR(reg_above_64);

          SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, READ_IHP_OAM_DEFAULT_COUNTERSr(unit, SOC_CORE_ALL, reg_above_64));

          
          for (mep_index = ARAD_PP_OAM_DEFAULT_EP_INGRESS_0; mep_index <= ARAD_PP_OAM_DEFAULT_EP_INGRESS_3; mep_index++) {
              soc_reg_above_64_field32_set(unit, IHP_OAM_DEFAULT_COUNTERSr, reg_above_64, COUNTER_FOR_INGRESS_PROFILE_0f + mep_index, 0x4000);
          }

          SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, WRITE_IHP_OAM_DEFAULT_COUNTERSr(unit, SOC_CORE_ALL, reg_above_64));
      }

      
      for (counter_proc = 0; counter_proc < _NUMBER_OF_COUNTER_PROCESSORS(unit); ++counter_proc) {
          char *propval = soc_property_port_get_str(unit, counter_proc, spn_COUNTER_ENGINE_SOURCE);
          
          if (propval && (sal_strcmp(propval, "INGRESS_OAM") == 0  || sal_strcmp(propval, "EGRESS_OAM") == 0)) {

              if (SOC_IS_JERICHO(unit)) {
                  SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("Counter engine source soc properties not supported for Jericho. Use API instead.")));
              }
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY,  counter_proc, CRPS_N_COUNT_MODEf,  4));
              if (sal_strcmp(propval, "INGRESS_OAM") == 0) {
                  LOG_DEBUG(BSL_LS_SOC_OAM,
                                            (BSL_META_U(unit,
                                                        "CRPS %d to be used for ingress OAM counter.\n"), counter_proc));
                  ++ingress_count;
              } else {
                  LOG_DEBUG(BSL_LS_SOC_OAM,
                                            (BSL_META_U(unit,
                                                        "CRPS %d to be used for egress OAM counter.\n"), counter_proc));
                  ++egress_count;
              }
          }
      }
      if (!is_bfd && ((egress_count * ingress_count) == 0) && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          LOG_WARN(BSL_LS_SOC_OAM,
                           (BSL_META_U(unit,
                                       "Warning: a counter must be allocated for egress OAM and ingress OAM each.\n")));
      }

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IHP_OAM_IN_LIF_PROFILE_MAPr, REG_PORT_ANY, 0, OAM_IN_LIF_PROFILEf,  0));
      if (SOC_IS_JERICHO_PLUS(unit)) {
         SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IHB_BUILD_OAM_TS_HEADERr, SOC_CORE_ALL, 0, BUILD_OAM_TS_HEADERf,  0x8e)); 
      } else {
         SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IHB_BUILD_OAM_TS_HEADERr, SOC_CORE_ALL, 0, BUILD_OAM_TS_HEADERf,  0xe)); 
      }

      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR, WRITE_IHB_OAM_MIP_BITMAPr(unit,  0));
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR, WRITE_IHB_OAM_MEP_BITMAPr(unit,  0));
      }
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR, WRITE_IHB_OAM_SUB_TYPE_MAPr(unit, SOC_CORE_ALL,  0x393)); 

      
      SOC_REG_ABOVE_64_CLEAR(reg_above_64);
      OAM_PROCESS_ADD(1, 1, OAM_PROCESS_ACTION_STAMP_COUNTER, reg_above_64);
      OAM_PROCESS_ADD(1, 0, OAM_PROCESS_ACTION_READ_COUNTER_AND_STAMP, reg_above_64);
      OAM_PROCESS_ADD(3, 1, OAM_PROCESS_ACTION_INGRESS_STAMP_NTP, reg_above_64);
      OAM_PROCESS_ADD(3, 0, OAM_PROCESS_ACTION_EGRESS_STAMP_NTP, reg_above_64);
      OAM_PROCESS_ADD(2, 1, OAM_PROCESS_ACTION_INGRESS_STAMP_1588, reg_above_64);
      OAM_PROCESS_ADD(2, 0, OAM_PROCESS_ACTION_EGRESS_STAMP_1588, reg_above_64);
      if (SOC_IS_JERICHO_PLUS(unit)) {
         OAM_PROCESS_ADD(7, 0, OAM_PROCESS_ACTION_READ_COUNTER_AND_STAMP, reg_above_64);
         OAM_PROCESS_ADD(7, 1, OAM_PROCESS_ACTION_STAMP_COUNTER, reg_above_64);
      }

      
      if((SOC_IS_JERICHO(unit) && (!SOC_IS_QAX(unit)))
         && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_ccm_pkt_counting", 0) == 1)
         && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "map_port_extr_enabled", 0) == 2)){
         
         OAM_PROCESS_ADD(4, 0, OAM_PROCESS_ACTION_READ_COUNTER_AND_STAMP, reg_above_64);
      }

      
      if (SOC_IS_QAX(unit)){
          OAM_PROCESS_ADD(4, 0, OAM_PROCESS_ACTION_STAMP_COUNTER, reg_above_64);
      }

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 190, exit, WRITE_EPNI_OAM_PROCESS_MAPr(unit, REG_PORT_ANY, reg_above_64));

      
      reg = 1 << _ARAD_PP_OAM_PTCH_OPAQUE_VALUE;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  191,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IHP_OAM_PACKET_INJECTED_BITMAPr, REG_PORT_ANY, 0, PACKET_INJECTED_BITMAPf,  reg));

      
      if(SOC_IS_QAX(unit)){
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  191,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, EPNI_CFG_EGRESS_OAM_INJECTION_MAPr, REG_PORT_ANY, 0, CFG_EGRESS_OAM_INJECTION_MAPf,  reg));
      }

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  195,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, OEMA_MNGMNT_UNIT_ENABLEf,  1));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  200,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, OEMB_MNGMNT_UNIT_ENABLEf,  1));

      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  205,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, ECI_GLOBALFr, REG_PORT_ANY, 0, TOD_MODEf,  3));
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  210,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, ECI_TOD_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, TOD_SYNC_ENABLEf,  1));
          
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0,               _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_CONTROL);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x13576543,      _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_FRAC_SEC_LOWER);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1ffff00,       _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_FRAC_SEC_UPPER);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x44b82fa1,      _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_FREQ_CONTROL);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x80804560,      _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_SEC);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1f,            _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_CONTROL);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x0,             _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_CONTROL);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x20,            _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_CONTROL);

          
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0,               _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_CONTROL);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x13576543,      _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_FRAC_SEC_LOWER);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1ffff00,       _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_FRAC_SEC_UPPER);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x10000000,      _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_FREQ_CONTROL);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x80804560,      _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_SEC);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1f,            _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_CONTROL);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x0,             _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_CONTROL);
          
          _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x20,            _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_CONTROL);
      }


      if (SOC_IS_ARADPLUS_A0(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 32, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IHB_REG_0092r, SOC_CORE_ALL, 0, ITEM_2_2f,  1));
      }

      if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_dm_tod_msb_add_enable", 1) == 0) {
          LOG_WARN(BSL_LS_SOC_OAM,
                           (BSL_META_U(unit,
                                       "Warning: OAMP acceleration of DM packets will not work correctly when custom_feature_oam_dm_tod_msb_add_enable is 0\n")));
      }

      if (soc_property_get(unit, spn_BCM886XX_OAM_DEFAULT_PROFILE, 0) > 0) { 
          
          res = arad_pp_oam_default_simple_in_lif_profile_mapping_set_unsafe(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      }
  }

  if (!is_bfd && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_mim", 0) == 1)) {
      
      res = arad_pp_oam_tcam_init_mim(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  if (!is_bfd && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "y1711_enabled", 0) == 1)) {
      
      res = arad_pp_oam_tcam_y1711_entry_add_unsafe(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      
      res = arad_pp_oam_tcam_init_mpls(unit, is_bfd);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  if (SOC_IS_JERICHO(unit) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0) == 1) && is_bfd == 0) {
      
      res = arad_pp_oam_tcam_init_mpls(unit, is_bfd);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  if (is_bfd && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_mpls_special_ttl_support", 0) == 1)) {
     res = arad_pp_oam_tcam_bfd_lsp_special_ttl_ipv4_entry_add_unsafe(unit);
     SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  
  if (!is_bfd && (((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_downmep_pwe_classification", 0) == 1) && (SOC_IS_ARADPLUS_AND_BELOW(unit))))){
      
      res = arad_pp_oam_tcam_init_inner_eth(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  

  if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 1))) {
      
      COMPILER_64_ZERO(reg_64);
      if (SOC_IS_QAX(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit, READ_ITE_STAMPING_FABRIC_HEADER_ENABLEr(unit, &reg_64));
          soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &reg_64, STAMP_DSP_EXT_ENf, 0);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit,WRITE_ITE_STAMPING_FABRIC_HEADER_ENABLEr(unit, reg_64));
      }
      else {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit, READ_IPT_STAMPING_FABRIC_HEADER_ENABLEr(unit, &reg_64));
          soc_reg64_field32_set(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, &reg_64, STAMP_DSP_EXTf, 0);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit,WRITE_IPT_STAMPING_FABRIC_HEADER_ENABLEr(unit, reg_64));
      }
  }

  
  if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
      for (profile = 0; profile < _ARAD_PP_OAM_NON_ACC_PROFILES_NUM_ARAD; profile++) {
          res = arad_pp_oam_classifier_profile_reset(unit, profile, is_bfd);
          SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_init_unsafe()", 0, 0);
}

int arad_pp_oam_channel_type_rx_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 channel_type_ndx,
    SOC_SAND_IN  int                                 num_values,
    SOC_SAND_IN  int                                 *list_of_values
    )
{
    uint32 oam_map_channel_cam_eidx, entry, field, channel_format;
    int val_index = 0, index_count = 0;
    int rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;


    index_count = soc_mem_index_count(unit, IHP_OAM_CHANNEL_TYPEm);

    for (oam_map_channel_cam_eidx = 0; oam_map_channel_cam_eidx < index_count; oam_map_channel_cam_eidx++) {
        field = 0;
        entry = 0;
        if (val_index >= num_values) {
            
            break;
        }
        channel_format = _arad_pp_oam_mpls_tp_channel_type_enum_to_format(channel_type_ndx);
        rv = READ_IHP_OAM_CHANNEL_TYPEm(unit, MEM_BLOCK_ANY, oam_map_channel_cam_eidx, &entry);
        if (rv < 0) {
            SOCDNX_EXIT_WITH_ERR_NO_MSG(rv);
        }
        soc_IHP_OAM_CHANNEL_TYPEm_field_get(unit, &entry, VALIDf, &field);
        if (field == 1) {
            
            continue;
        }
        entry = 0;
        
        soc_IHP_OAM_CHANNEL_TYPEm_field_set(unit, &entry, CHANNEL_TYPEf, (uint32 *)(&(list_of_values[val_index++])));
        field = 1;
        soc_IHP_OAM_CHANNEL_TYPEm_field_set(unit, &entry, VALIDf, &field);
        SHR_BITCOPY_RANGE(&entry, 17, &channel_format, 0, 3); 

        rv = WRITE_IHP_OAM_CHANNEL_TYPEm(unit, MEM_BLOCK_ANY, oam_map_channel_cam_eidx, &entry);
        if (rv < 0) {
            SOCDNX_EXIT_WITH_ERR_NO_MSG(rv);
        }
    }

    if (val_index < num_values-1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE,(_BSL_SOCDNX_MSG("Not enough free entries. Number of values written %d"), val_index+1));
    }

exit:
    SOCDNX_FUNC_RETURN;
    
}

int arad_pp_oam_channel_type_rx_delete(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 channel_type_ndx,
    SOC_SAND_IN  int                                 num_values,
    SOC_SAND_IN  int                                 *list_of_values
    )
{
    uint32 oam_map_channel_cam_eidx, entry, field, channel_format = 0, valid = 0, temp_field = 0;
    int index_count = 0, returned_values[16], no_of_returned_values = 0, match = 0, i=0;
    int rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;


    index_count = soc_mem_index_count(unit, IHP_OAM_CHANNEL_TYPEm);
    
    rv = arad_pp_oam_channel_type_rx_get(unit, channel_type_ndx, index_count, returned_values, &no_of_returned_values);
    SOCDNX_IF_ERR_EXIT(rv);

    for (oam_map_channel_cam_eidx = 0; oam_map_channel_cam_eidx < num_values; oam_map_channel_cam_eidx++) {
        for (i=0; i< no_of_returned_values; i++)
        {
           if(returned_values[i] == list_of_values[oam_map_channel_cam_eidx]) {
               match = 1;
               break;
           } 
        }
        if(!match) {
              SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("OAM channel type to delete does not exist in HW ")));
        }
        match = 0; 
    }

    for (oam_map_channel_cam_eidx = 0; oam_map_channel_cam_eidx < index_count; oam_map_channel_cam_eidx++) {
        field = 0;
        entry = 0;
        channel_format = _arad_pp_oam_mpls_tp_channel_type_enum_to_format(channel_type_ndx);
        rv = READ_IHP_OAM_CHANNEL_TYPEm(unit, MEM_BLOCK_ANY, oam_map_channel_cam_eidx, &entry);
        if (rv < 0) {
            SOCDNX_EXIT_WITH_ERR_NO_MSG(rv);
        }
        soc_IHP_OAM_CHANNEL_TYPEm_field_get(unit, &entry, VALIDf, &valid);
        field = 0;
        soc_IHP_OAM_CHANNEL_TYPEm_field_get(unit, &entry, CHANNEL_TYPEf, &field);
        if (valid) {
            match = 0; 
            for (i=0; i< num_values; i++)
            {
                if(field == list_of_values[i]) {
                    match = 1;
                    break;
                } 
            }
            if (match) {
                temp_field = 0;
                
                soc_IHP_OAM_CHANNEL_TYPEm_field_set(unit, &entry, CHANNEL_TYPEf, &temp_field);
                soc_IHP_OAM_CHANNEL_TYPEm_field_set(unit, &entry, VALIDf, &temp_field);
                SHR_BITCOPY_RANGE(&entry, 17, &channel_format, 0, 3); 
                rv = WRITE_IHP_OAM_CHANNEL_TYPEm(unit, MEM_BLOCK_ANY, oam_map_channel_cam_eidx, &entry);
                if (rv < 0) {
                    SOCDNX_EXIT_WITH_ERR_NO_MSG(rv);
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
    
}

int arad_pp_oam_channel_type_rx_get(
    SOC_SAND_IN    int                                 unit,
    SOC_SAND_IN    int                                 channel_type_ndx,
    SOC_SAND_IN    int                                 num_values,
    SOC_SAND_INOUT int                                 *list_of_values,
    SOC_SAND_OUT   int                                 *value_count
    )
{
    uint32 oam_map_channel_cam_eidx, entry, field, channel_format, exp_channel_format;
    int val_index = 0, index_count = 0;
    int rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;


    index_count = soc_mem_index_count(unit, IHP_OAM_CHANNEL_TYPEm);

    for (oam_map_channel_cam_eidx = 0; oam_map_channel_cam_eidx < index_count; oam_map_channel_cam_eidx++) {
        field = 0;
        entry = 0;
        channel_format = 0;
        if (val_index >= num_values) {
            
            break;
        }
        exp_channel_format = _arad_pp_oam_mpls_tp_channel_type_enum_to_format(channel_type_ndx);
        rv = READ_IHP_OAM_CHANNEL_TYPEm(unit, MEM_BLOCK_ANY, oam_map_channel_cam_eidx, &entry);
        if (rv < 0) {
            SOCDNX_EXIT_WITH_ERR_NO_MSG(rv);
        }
        soc_IHP_OAM_CHANNEL_TYPEm_field_get(unit, &entry, VALIDf, &field);
        if (field == 1) { 
            SHR_BITCOPY_RANGE(&channel_format, 0, &entry, 17, 3); 
            if (channel_format == exp_channel_format) {
                
                soc_IHP_OAM_CHANNEL_TYPEm_field_get(unit, &entry, CHANNEL_TYPEf, (uint32 *)(&(list_of_values[val_index++])));
            }
        }

    }

    *value_count = val_index;
exit:
    SOCDNX_FUNC_RETURN;
}

int arad_pp_oam_channel_type_tx_set(
    SOC_SAND_IN    int                                 unit,
    SOC_SAND_IN    int                                 channel_type_ndx,
    SOC_SAND_IN    int                                 value
    )
{
    uint32 cw = 0, reg = 0;
    int rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;
    
    
    cw = 0x10000000 |  value; 

    switch (channel_type_ndx) {
        case _ARAD_PP_OAM_MPLS_TP_CHANNEL_BFD_CONTROL_ENUM:
            soc_reg_field_set(unit, OAMP_BFD_PWE_CW_0r, &reg, BFD_PWE_CW_0f, cw);
            rv = WRITE_OAMP_BFD_PWE_CW_0r(unit, reg);
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        case _ARAD_PP_OAM_MPLS_TP_CHANNEL_PWE_ON_OAM_ENUM:
            soc_reg_field_set(unit, OAMP_Y_1731O_PWE_GACHr, &reg, Y_1731O_PWE_GACHf, cw);
            rv = WRITE_OAMP_Y_1731O_PWE_GACHr(unit, reg);
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        case _ARAD_PP_OAM_MPLS_TP_CHANNEL_Y1731_ENUM:
            soc_reg_field_set(unit, OAMP_Y_1731O_MPLSTP_GACHr, &reg, Y_1731O_MPLSTP_GACHf, cw);
            rv = WRITE_OAMP_Y_1731O_MPLSTP_GACHr(unit, reg);
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_PARAM);
            break;
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int arad_pp_oam_channel_type_tx_delete(
    SOC_SAND_IN    int                                 unit,
    SOC_SAND_IN    int                                 channel_type_ndx,
    SOC_SAND_IN    int                                 value
    )
{
    uint32 cw = 0, reg = 0, mpls_tp_channel_type = 0;
    int rv = SOC_E_NONE, get_value = 0;
    SOCDNX_INIT_FUNC_DEFS;
    
    rv = arad_pp_oam_channel_type_tx_get(unit, channel_type_ndx, &get_value);
    SOCDNX_IF_ERR_EXIT(rv);

    if (get_value != value) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("Non configured channel type to delete.")));
    }
    
    mpls_tp_channel_type = _arad_pp_oam_mpls_tp_channel_type_enum_to_channel_type(unit, channel_type_ndx); 
    cw = 0x10000000 |  mpls_tp_channel_type; 
    switch (channel_type_ndx) {
        case _ARAD_PP_OAM_MPLS_TP_CHANNEL_BFD_CONTROL_ENUM:
            soc_reg_field_set(unit, OAMP_BFD_PWE_CW_0r, &reg, BFD_PWE_CW_0f, cw);
            rv = WRITE_OAMP_BFD_PWE_CW_0r(unit, reg);
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        case _ARAD_PP_OAM_MPLS_TP_CHANNEL_PWE_ON_OAM_ENUM:
            soc_reg_field_set(unit, OAMP_Y_1731O_PWE_GACHr, &reg, Y_1731O_PWE_GACHf, cw);
            rv = WRITE_OAMP_Y_1731O_PWE_GACHr(unit, reg);
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        case _ARAD_PP_OAM_MPLS_TP_CHANNEL_Y1731_ENUM:
            soc_reg_field_set(unit, OAMP_Y_1731O_MPLSTP_GACHr, &reg, Y_1731O_MPLSTP_GACHf, cw);
            rv = WRITE_OAMP_Y_1731O_MPLSTP_GACHr(unit, reg);
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_PARAM);
            break;
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int arad_pp_oam_channel_type_tx_get(
    SOC_SAND_IN    int                                 unit,
    SOC_SAND_IN    int                                 channel_type_ndx,
    SOC_SAND_OUT   int                                 *value
    )
{
    uint32 cw = 0, reg = 0;
    int rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    switch (channel_type_ndx) {
        case _ARAD_PP_OAM_MPLS_TP_CHANNEL_BFD_CONTROL_ENUM:
            rv = READ_OAMP_BFD_PWE_CW_0r(unit, &reg);
            SOCDNX_IF_ERR_EXIT(rv);
            cw = soc_reg_field_get(unit, OAMP_BFD_PWE_CW_0r, reg, BFD_PWE_CW_0f);
            break;
        case _ARAD_PP_OAM_MPLS_TP_CHANNEL_PWE_ON_OAM_ENUM:
            rv = READ_OAMP_Y_1731O_PWE_GACHr(unit, &reg);
            SOCDNX_IF_ERR_EXIT(rv);
            cw = soc_reg_field_get(unit, OAMP_Y_1731O_PWE_GACHr, reg, Y_1731O_PWE_GACHf);
            break;
        case _ARAD_PP_OAM_MPLS_TP_CHANNEL_Y1731_ENUM:
            rv = READ_OAMP_Y_1731O_MPLSTP_GACHr(unit, &reg);
            SOCDNX_IF_ERR_EXIT(rv);
            cw = soc_reg_field_get(unit, OAMP_Y_1731O_MPLSTP_GACHr, reg, Y_1731O_MPLSTP_GACHf);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_PARAM);
            break;
    }
    *value = cw & 0xffff; 
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_oam_init_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  is_bfd
  )
{
    uint32 res = 0;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO profile_info;
    uint32 cpu_trap_code;
    uint8 is_second_init=0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_INIT_UNSAFE);

    
    if (is_bfd) {
       
        res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.oam_enable.get(unit, &is_second_init);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else {
       
        res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.bfd_enable.get(unit, &is_second_init);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    }

    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&profile_info);
    if(is_bfd) {
        res = sw_state_access[unit].dpp.bcm.bfd.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_CPU, &cpu_trap_code);
    } else {
        res = sw_state_access[unit].dpp.bcm.oam.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_CPU, &cpu_trap_code);
    }
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    res = soc_ppd_trap_frwrd_profile_info_get(unit, cpu_trap_code, &profile_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_oam_oamp_init_unsafe(unit, is_bfd, profile_info.dest_info.frwrd_dest.dest_id, is_second_init);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = arad_pp_oam_classifier_init_unsafe(unit, is_bfd, is_second_init);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (SOC_IS_JERICHO(unit)) {
        if (!is_second_init) {
            
            res = soc_jer_pp_oam_classifier_init(unit);
            SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res,222,exit,res);

            res = soc_jer_pp_oam_init_eci_tod(unit, 1, 1);
            SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res,222,exit,res);

            
            res = soc_jer_pp_oam_oamp_init(unit);
            SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res,222,exit,res);

            if (SOC_IS_JERICHO_PLUS(unit)) {
                
                res = soc_qax_pp_oam_classifier_init(unit);
                SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res,222,exit,res);

                if(SOC_IS_QAX(unit)) {
                    
                    res = soc_qax_pp_oam_oamp_init(unit);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res,222,exit,res);
                }
            }
        }
        if (!is_bfd) { 
            res = soc_jer_pp_oam_classifier_oam1_entries_add(unit);
            SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res,222,exit,res);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_init_unsafe()", 0, 0);
}


uint32
  arad_pp_oam_deinit_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  uint8                                  is_bfd,
    SOC_SAND_IN  uint8                                  tcam_db_destroy
  )
{
    uint32 res = 0;
    uint32 entry_id;
    uint32 tcam_last_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_DEINIT_UNSAFE);

    
    if (tcam_db_destroy) {
        res = sw_state_access[unit].dpp.soc.arad.pp.oam.tcam_last_entry_id.get(unit, &tcam_last_id);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
        for (entry_id = 0; entry_id <= tcam_last_id; entry_id++) {
            res = arad_tcam_managed_db_entry_remove_unsafe(unit, ARAD_PP_TCAM_STATIC_ACCESS_ID_OAM_IDENTIFICATION, entry_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        }
        res = arad_tcam_access_destroy_unsafe(unit, ARAD_PP_TCAM_STATIC_ACCESS_ID_OAM_IDENTIFICATION);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_deinit_unsafe()", 0, 0);
}



uint32
  arad_pp_oam_icc_map_register_set_unsafe(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN int                      icc_ndx,
    SOC_SAND_IN SOC_PPC_OAM_ICC_MAP_DATA *data
  )
{
  uint32 res;
  soc_reg_above_64_val_t  reg_val;
  uint32 field_32[2] = {0};
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_ICC_MAP_REGISTER_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  field_32[1] = COMPILER_64_HI(*data);
  field_32[0] = COMPILER_64_LO(*data);

  if (icc_ndx < 10) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_ICC_MAP_REG_1r(unit, reg_val));
      SHR_BITCOPY_RANGE(reg_val, SOC_PPC_OAM_ICC_MAP_DATA_NOF_BITS*icc_ndx, field_32, 0, SOC_PPC_OAM_ICC_MAP_DATA_NOF_BITS);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_ICC_MAP_REG_1r(unit, reg_val));
  }
  else {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_OAMP_ICC_MAP_REG_2r(unit, reg_val));
      SHR_BITCOPY_RANGE(reg_val, (SOC_PPC_OAM_ICC_MAP_DATA_NOF_BITS*(icc_ndx-10)), field_32, 0, SOC_PPC_OAM_ICC_MAP_DATA_NOF_BITS);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_ICC_MAP_REG_2r(unit, reg_val));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_icc_map_register_set_unsafe()", icc_ndx, 0);
}

uint32
  arad_pp_oam_icc_map_register_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                       icc_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_ICC_MAP_DATA   *data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_ICC_MAP_REGISTER_SET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_icc_map_register_set_verify()", 0, 0);

}

uint32
  arad_pp_oam_icc_map_register_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                      icc_ndx,
    SOC_SAND_OUT SOC_PPC_OAM_ICC_MAP_DATA * data
  )
{
  uint32 res;
  soc_reg_above_64_val_t  reg_val;
  uint32 field_32[2] = {0};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_ICC_MAP_REGISTER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  if (icc_ndx < 10) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_ICC_MAP_REG_1r(unit, reg_val));
      SHR_BITCOPY_RANGE(field_32, 0, reg_val, SOC_PPC_OAM_ICC_MAP_DATA_NOF_BITS*icc_ndx, SOC_PPC_OAM_ICC_MAP_DATA_NOF_BITS);
  }
  else {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_OAMP_ICC_MAP_REG_2r(unit, reg_val));
      SHR_BITCOPY_RANGE(field_32, 0, reg_val, (SOC_PPC_OAM_ICC_MAP_DATA_NOF_BITS*(icc_ndx-10)), SOC_PPC_OAM_ICC_MAP_DATA_NOF_BITS);
  }

  COMPILER_64_SET(*data, field_32[1], field_32[0]);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_icc_map_register_get_unsafe()", icc_ndx, 0);
}


#define SOC_SAND_CHECK_MAC_IS_UNICAST(mac_address) ((mac_address->address[5] & 0x1) != 1)



STATIC uint32
  arad_pp_oam_my_cfm_mac_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core, 
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *dst_mac_address,
    SOC_SAND_IN  uint32                     table_index
  )
{
  uint32 res = SOC_SAND_OK;
  soc_reg_above_64_val_t entry;
  soc_reg_above_64_val_t lsb_bitmap, lsb_bitmap_new;
  soc_reg_above_64_val_t msb;
  soc_reg_above_64_val_t msb_prev;
  int is_first_mep;
  uint64 msb_64;
  uint32 mac_add_U32[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
  int ref_count_idx;
  uint16 ref_count_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MY_CFM_MAC_SET);
  SOC_SAND_CHECK_NULL_INPUT(dst_mac_address);
  SOC_REG_ABOVE_64_CLEAR(lsb_bitmap);
  SOC_REG_ABOVE_64_CLEAR(lsb_bitmap_new);
  SOC_REG_ABOVE_64_CLEAR(entry);
  SOC_REG_ABOVE_64_CLEAR(msb);
  COMPILER_64_ZERO(msb_64);

  
  if (SOC_SAND_CHECK_MAC_IS_UNICAST(dst_mac_address)) {

      
         
      res = soc_sand_pp_mac_address_struct_to_long(dst_mac_address, mac_add_U32);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      _ARAD_PP_OAM_MAC_ADD_MSB_GET(mac_add_U32, msb_64); 
      msb[0] = COMPILER_64_LO(msb_64);
      msb[1] = COMPILER_64_HI(msb_64);

      
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_IHP_OAM_MY_CFM_MACm(unit, SOC_BLOCK_BY_DEVICE(unit,IHP_BLOCK(unit, core)), table_index, &entry));
      soc_IHP_OAM_MY_CFM_MACm_field_get(unit, &entry, LSB_BITMAPf, lsb_bitmap);
      if (SOC_REG_ABOVE_64_IS_ZERO(lsb_bitmap)) {
          is_first_mep = TRUE;
          soc_IHP_OAM_MY_CFM_MACm_field_set(unit, &entry, MSBf, msb);
      }
      else {
          is_first_mep = FALSE;
          
          SOC_REG_ABOVE_64_CLEAR(msb_prev);
          soc_IHP_OAM_MY_CFM_MACm_field_get(unit, &entry, MSBf, msb_prev);
          if (!SOC_REG_ABOVE_64_IS_EQUAL(msb_prev,msb)) {
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_DA_SUFFIX_CONFLICT_ERR, 20, exit);
          }
      }
      
      ref_count_idx = _ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT_INDEX(table_index, dst_mac_address);
      res = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.ref_count.get(unit, core, ref_count_idx, &ref_count_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
      ref_count_val++;
      res = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.ref_count.set(unit, core, ref_count_idx, ref_count_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

      SOC_REG_ABOVE_64_CREATE_MASK(lsb_bitmap_new, 1, dst_mac_address->address[0]);
      SOC_REG_ABOVE_64_OR(lsb_bitmap, lsb_bitmap_new);
      soc_IHP_OAM_MY_CFM_MACm_field_set(unit, &entry, LSB_BITMAPf, lsb_bitmap);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_IHP_OAM_MY_CFM_MACm(unit, SOC_BLOCK_BY_DEVICE(unit,IHP_BLOCK(unit, core)), table_index, entry));

      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_EPNI_MY_CFM_MAC_TABLEm(unit, SOC_BLOCK_BY_DEVICE(unit,EPNI_BLOCK(unit, core)), table_index, &entry));
      if (is_first_mep) {
          soc_EPNI_MY_CFM_MAC_TABLEm_field_set(unit, &entry, MAC_MSBSf, msb);
      } 
      soc_EPNI_MY_CFM_MAC_TABLEm_field_set(unit, &entry, MAC_LSBS_BITMAPf, lsb_bitmap);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_EPNI_MY_CFM_MAC_TABLEm(unit, SOC_BLOCK_BY_DEVICE(unit,EPNI_BLOCK(unit, core)), table_index, entry));
  } else {
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, SOC_E_PARAM);
  }
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_my_cfm_mac_set_unsafe()", 0, table_index);
}

soc_error_t 
    arad_pp_oam_my_cfm_mac_set(int unit, int core, SOC_SAND_PP_MAC_ADDRESS  *dst_mac_address, uint32 table_index){
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_pp_oam_my_cfm_mac_set_unsafe(unit, core, dst_mac_address, table_index);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t arad_pp_oam_slm_set(int unit, int is_slm)
{
    int rv;
    uint64 reg;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO(unit)) {
        rv = READ_IHP_OAM_COUNTER_INCREMENT_BITMAPr(unit, SOC_CORE_ALL, &reg);
        SOCDNX_IF_ERR_EXIT(rv);

        soc_reg64_field32_set(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, &reg, OAM_EGRESS_SUB_TYPEf, _ARAD_PP_OAM_SUBTYPE_DEFAULT_EGRESS_OAM);

        rv = WRITE_IHP_OAM_COUNTER_INCREMENT_BITMAPr(unit, SOC_CORE_ALL, reg);
        SOCDNX_IF_ERR_EXIT(rv);

    } else {
        
        rv = soc_reg_above_64_field32_modify(unit, IHB_REG_0092r, SOC_CORE_ALL, 0, ITEM_2_2f, is_slm ==0);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t arad_pp_oam_counter_increment_bitmap_set(int unit, int enable_counting) {
    int rv;
    uint32 counter_inc_reg;
    SOCDNX_INIT_FUNC_DEFS;
    
    rv =  soc_reg_above_64_field32_read(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, REG_PORT_ANY, 0, OAM_COUNTER_INCREMENTf, &counter_inc_reg);
    SOCDNX_IF_ERR_EXIT(rv); 
    if (!enable_counting) {
        counter_inc_reg &= (0xff & (~(1 << _ARAD_PP_OAM_SUBTYPE_LM))); 
        if (SOC_IS_JERICHO(unit)) {
            counter_inc_reg &= (0xff & (~(1 << _ARAD_PP_OAM_SUBTYPE_DEFAULT_EGRESS_OAM))); 
        }
    } else {
        counter_inc_reg |= (1 << _ARAD_PP_OAM_SUBTYPE_LM); 
        if (SOC_IS_JERICHO(unit)) {
           counter_inc_reg |= (1 << _ARAD_PP_OAM_SUBTYPE_DEFAULT_EGRESS_OAM); 
        }
    }
    rv = soc_reg_above_64_field32_modify(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, REG_PORT_ANY, 0, OAM_COUNTER_INCREMENTf,  counter_inc_reg);
    SOCDNX_IF_ERR_EXIT(rv); 

exit:
    SOCDNX_FUNC_RETURN;
}


                    
soc_error_t
_arad_pp_oam_set_counter_disable(int unit,
                                 int  internal_opcode, 
                                 SOC_PPC_OAM_MEP_TYPE mep_type,
                                 SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD * oam_payload, 
                                 const SOC_PPC_OAM_MEP_PROFILE_DATA *profile_data, 
                                 int is_piggy_backed) {
    int oam_default_piggyback_lm_enable = 0;

    SOCDNX_INIT_FUNC_DEFS;

    oam_default_piggyback_lm_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_default_piggyback_lm_enable", 0);

    if (internal_opcode_init[internal_opcode].additional_info == ARAD_PP_OAM_OPCODE_LM) {
        oam_payload->counter_disable = 0; 
      }  else if ((is_piggy_backed && internal_opcode_init[internal_opcode].additional_info == ARAD_PP_OAM_OPCODE_CCM)) {
        oam_payload->counter_disable = 0; 
    } else if ((oam_default_piggyback_lm_enable && internal_opcode_init[internal_opcode].additional_info == ARAD_PP_OAM_OPCODE_CCM)) {
        oam_payload->counter_disable = 0;
    } else {
        oam_payload->counter_disable = (SHR_BITGET(profile_data->counter_disable, internal_opcode) != 0);
    }

    SOC_EXIT;
exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
  arad_pp_oam_my_cfm_mac_get(int unit, int core, SOC_SAND_PP_MAC_ADDRESS *dst_mac_address, uint32 table_index){
    int rv, i;
    uint8 lsb;
    uint32 mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S] = {0};
    soc_reg_above_64_val_t ihb_entry;
    int ref_count_idx;
    uint16 ref_count_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(dst_mac_address);
    SOC_REG_ABOVE_64_CLEAR(ihb_entry);

    
    ref_count_idx = _ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT_INDEX(table_index, dst_mac_address);
    rv = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.ref_count.get(unit, core, ref_count_idx, &ref_count_val);
    SOCDNX_IF_ERR_EXIT(rv);

    
    if (ref_count_val == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("Entry doesn't exist.")));
    }

    
    rv = READ_IHP_OAM_MY_CFM_MACm(unit, SOC_BLOCK_BY_DEVICE(unit,IHP_BLOCK(unit, core)), table_index, &ihb_entry);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    soc_IHP_OAM_MY_CFM_MACm_field_get(unit, &ihb_entry, MSBf, mac_in_longs);

    
    lsb = dst_mac_address->address[0];
    rv = soc_sand_pp_mac_address_long_to_struct(mac_in_longs, dst_mac_address);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    for (i = SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - 1 ; i > 0 ; i--) {
        dst_mac_address->address[i] = dst_mac_address->address[i - 1];
    }
    
    dst_mac_address->address[0] = lsb;

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_oam_my_cfm_mac_delete_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS  *dst_mac_address,
    SOC_SAND_IN  uint32                    table_index
  )
{
  uint32 res = SOC_SAND_OK;
  soc_reg_above_64_val_t msb;
  soc_reg_above_64_val_t ihb_entry;
  soc_reg_above_64_val_t epni_entry;
  soc_reg_above_64_val_t lsb_bitmap;
  soc_reg_above_64_val_t lsb_bitmap_mask;
  int ref_count_idx;
  uint16 ref_count_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MY_CFM_MAC_DELETE);
  SOC_SAND_CHECK_NULL_INPUT(dst_mac_address);
  SOC_REG_ABOVE_64_CLEAR(lsb_bitmap);
  SOC_REG_ABOVE_64_CLEAR(ihb_entry);
  SOC_REG_ABOVE_64_CLEAR(epni_entry);
  SOC_REG_ABOVE_64_CLEAR(lsb_bitmap_mask);
  SOC_REG_ABOVE_64_CLEAR(msb);
  
    
  if (SOC_SAND_CHECK_MAC_IS_UNICAST(dst_mac_address)) {
      ref_count_idx = _ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT_INDEX(table_index, dst_mac_address);
      res = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.ref_count.get(unit, core, ref_count_idx, &ref_count_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      ref_count_val--;
      res = sw_state_access[unit].dpp.soc.arad.pp.oam_my_mac_lsb.ref_count.set(unit, core, ref_count_idx, ref_count_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      if (ref_count_val==0) {
          
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHP_OAM_MY_CFM_MACm(unit, SOC_BLOCK_BY_DEVICE(unit,IHP_BLOCK(unit, core)), table_index, &ihb_entry));
          soc_IHP_OAM_MY_CFM_MACm_field_get(unit, &ihb_entry, LSB_BITMAPf, lsb_bitmap);
          SOC_REG_ABOVE_64_CREATE_MASK(lsb_bitmap_mask, 1, dst_mac_address->address[0]);
          SOC_REG_ABOVE_64_NOT(lsb_bitmap_mask);
          SOC_REG_ABOVE_64_AND(lsb_bitmap, lsb_bitmap_mask);
          soc_IHP_OAM_MY_CFM_MACm_field_set(unit, &ihb_entry, LSB_BITMAPf, lsb_bitmap);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_EPNI_MY_CFM_MAC_TABLEm(unit, SOC_BLOCK_BY_DEVICE(unit,EPNI_BLOCK(unit, core)), table_index, &epni_entry));
          soc_EPNI_MY_CFM_MAC_TABLEm_field_set(unit, &epni_entry, MAC_LSBS_BITMAPf, lsb_bitmap);
          if (SOC_REG_ABOVE_64_IS_ZERO(lsb_bitmap)) {
              
              soc_IHP_OAM_MY_CFM_MACm_field_set(unit, &ihb_entry, MSBf, msb);
              soc_EPNI_MY_CFM_MAC_TABLEm_field_set(unit, &epni_entry, MAC_MSBSf, msb);
          }
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_IHP_OAM_MY_CFM_MACm(unit, SOC_BLOCK_BY_DEVICE(unit,IHP_BLOCK(unit, core)), table_index, ihb_entry));
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_EPNI_MY_CFM_MAC_TABLEm(unit, SOC_BLOCK_BY_DEVICE(unit,EPNI_BLOCK(unit, core)), table_index, epni_entry));
      }
  } else {
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, SOC_E_PARAM);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_my_cfm_mac_delete()", 0, table_index);
}

soc_error_t
  arad_pp_oam_my_cfm_mac_delete(int unit, int core, SOC_SAND_PP_MAC_ADDRESS *dst_mac_address, uint32 table_index)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_pp_oam_my_cfm_mac_delete_unsafe(unit, core, dst_mac_address, table_index);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}




uint32
  arad_pp_oam_classifier_oem1_entry_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload
  )
{
  uint32  res;
  uint32  reg_val; 
  uint32  entry_buffer[3];
  uint32  key[1];
  uint32  payload[2] = {0};
  uint8  found;
  
#if SOC_DPP_IS_EM_HW_ENABLE
  uint32  failure, fail_valid_val, fail_key_val, fail_reason_val;
#endif
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oem1_key);
  SOC_SAND_CHECK_NULL_INPUT(oem1_payload);

  _ARAD_PP_OAM_OEM1_KEY_STRUCT_TO_KEY((*oem1_key), *key);
  _arad_pp_oam_oem1_payload_struct_to_payload(unit, (*oem1_payload),payload);
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " Inserting entry to OEM1: key %d payload: %d\n\r"), *key, payload[0]));

  ARAD_PP_CLEAR(entry_buffer, uint32, 3);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_PPDB_A_OEMA_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0, entry_buffer));
  reg_val = 1; 
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_TYPEf, &reg_val);
  reg_val = 0; 
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_STAMPf, &reg_val);
  reg_val = 0; 
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_SELFf, &reg_val);
  reg_val = 0;
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_RESERVED_LOWf, &reg_val);
  reg_val = 3;
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_AGE_STATUSf, &reg_val);
  reg_val = *key;
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_KEYf, &reg_val);
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_PAYLOADf, payload);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_PPDB_A_OEMA_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0, entry_buffer));

  if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
      res = chip_sim_exact_match_entry_add_unsafe(
              unit,
              ARAD_CHIP_SIM_OEMA_BASE,
              key,
              ARAD_CHIP_SIM_OEMA_KEY,
              payload,
              ARAD_CHIP_SIM_OEMA_PAYLOAD,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }
#if SOC_DPP_IS_EM_HW_ENABLE
  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          PPDB_A_OEMA_INTERRUPT_REGISTER_ONEr,
          REG_PORT_ANY,
          0,
          OEMA_MANAGEMENT_COMPLETEDf,
          1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, READ_PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr(unit, &failure));
  fail_valid_val = soc_reg_field_get(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr, failure, OEMA_MNGMNT_UNIT_FAILURE_VALIDf);

  if (fail_valid_val){

             
      fail_key_val = soc_reg_field_get(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr, failure, OEMA_MNGMNT_UNIT_FAILURE_KEYf);
      fail_reason_val = soc_reg_field_get(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr, failure, OEMA_MNGMNT_UNIT_FAILURE_REASONf);
      ARAD_PP_OAM_OEM1_OEM2_FIND_ERRORS_AND_PRINT(fail_key_val, *key, fail_reason_val, 1);

  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem1_entry_set_unsafe()", 0, 0);
}


uint32 arad_pp_oam_classifier_oem1_init( SOC_SAND_IN  int        unit)
{
    uint32  res;
    uint64 reg;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET_UNSAFE);
    
      
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  195,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, OEMA_MNGMNT_UNIT_ENABLEf,  1));

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, READ_IHP_OAM_COUNTER_INCREMENT_BITMAPr(unit, SOC_CORE_ALL, &reg));
    soc_reg64_field32_set(unit, IHP_OAM_COUNTER_INCREMENT_BITMAPr, &reg, OAM_COUNTER_INCREMENTf, 0x1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_IHP_OAM_COUNTER_INCREMENT_BITMAPr(unit, SOC_CORE_ALL, reg));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem1_init()", 0, 0);
}


uint32
  arad_pp_oam_classifier_oem1_entry_get_unsafe(
    SOC_SAND_IN   int                                     unit,
    SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
    SOC_SAND_OUT  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload,
    SOC_SAND_OUT  uint8                                      *is_found
  )
{

  uint32  res;
  uint32  key[1];

  uint32  data_out[2];
#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
  uint64  reg_buffer_64;
  uint64  fld_val;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oem1_key);
  SOC_SAND_CHECK_NULL_INPUT(oem1_payload);
  SOC_SAND_CHECK_NULL_INPUT(is_found);
#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
  COMPILER_64_ZERO(reg_buffer_64);
#endif
  _ARAD_PP_OAM_OEM1_KEY_STRUCT_TO_KEY((*oem1_key), *key);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_A_OEMA_DIAGNOSTICS_KEYr, REG_PORT_ANY, 0, OEMA_DIAGNOSTICS_KEYf, *key));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_A_OEMA_DIAGNOSTICSr, REG_PORT_ANY, 0, OEMA_DIAGNOSTICS_LOOKUPf,  1));

#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
  if (SOC_DPP_IS_EM_SIM_ENABLE(unit)
#ifdef CRASH_RECOVERY_SUPPORT 
      || ((SOC_IS_DONE_INIT(unit)) && (BCM_UNIT_DO_HW_READ_WRITE(unit)))
#endif 
      )
  {
  
      res = chip_sim_exact_match_entry_get_unsafe(
              unit,
              ARAD_CHIP_SIM_OEMA_BASE,
              key,
              ARAD_CHIP_SIM_OEMA_KEY,
              data_out,
              ARAD_CHIP_SIM_OEMA_PAYLOAD,
              is_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

      if (*is_found) {
          _ARAD_PP_OAM_OEM1_PAYLOAD_TO_PAYLOAD_STRUCT(data_out, (*oem1_payload));
          LOG_DEBUG(BSL_LS_SOC_OAM,
                    (BSL_META_U(unit,
                                " Getting entry to OEM1: key %d payload: %d\n\r"), *key, data_out[0]));
      }
      else {
          LOG_DEBUG(BSL_LS_SOC_OAM,
                    (BSL_META_U(unit,
                                " Getting entry to OEM1: key %d not found\n\r"), *key));
      }


      ARAD_PP_DO_NOTHING_AND_EXIT;
  }
#endif
  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          PPDB_A_OEMA_DIAGNOSTICSr,
          REG_PORT_ANY,
          0,
          OEMA_DIAGNOSTICS_LOOKUPf,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_PPDB_A_OEMA_DIAGNOSTICS_LOOKUP_RESULTr(unit, &reg_buffer_64)); 

  ARAD_FLD_FROM_REG64(PPDB_A_OEMA_DIAGNOSTICS_LOOKUP_RESULTr, OEMA_ENTRY_FOUNDf, fld_val, reg_buffer_64, 50, exit);
  *is_found = SOC_SAND_NUM2BOOL(COMPILER_64_LO(fld_val));

  if (*is_found) {
      ARAD_FLD_FROM_REG64(PPDB_A_OEMA_DIAGNOSTICS_LOOKUP_RESULTr, OEMA_ENTRY_PAYLOADf, fld_val, reg_buffer_64, 60, exit);
      data_out[0] = COMPILER_64_LO(fld_val);
      data_out[1] = COMPILER_64_HI(fld_val);
      _ARAD_PP_OAM_OEM1_PAYLOAD_TO_PAYLOAD_STRUCT(data_out, (*oem1_payload));
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " Getting entry to OEM1: key %d payload: %d\n\r"), *key, COMPILER_64_LO(fld_val)));
  }
  else {
      LOG_DEBUG(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            " Getting entry to OEM1: key %d not found\n\r"), *key));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem1_entry_get_unsafe()", 0, 0);
}


uint32
  arad_pp_oam_classifier_oem1_entry_delete_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY *oem1_key
  )
{
  uint32  res;
  uint32  reg_val;
  uint32  entry_buffer[3];
  uint32  key[1];
#if SOC_DPP_IS_EM_HW_ENABLE
  uint32  failure, fail_valid_val, fail_key_val, fail_reason_val;
#endif
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oem1_key);
  ARAD_PP_CLEAR(entry_buffer, uint32, 2);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_PPDB_A_OEMA_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0, entry_buffer));
  reg_val = 0; 
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_TYPEf, &reg_val);
  reg_val = 0; 
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_STAMPf, &reg_val);
  _ARAD_PP_OAM_OEM1_KEY_STRUCT_TO_KEY((*oem1_key), *key);
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " Deleting entry to OEM1: key %d \n\r"), *key));
  soc_PPDB_A_OEMA_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMA_KEYf, key);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_PPDB_A_OEMA_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0, entry_buffer));

  if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
      res = chip_sim_exact_match_entry_remove_unsafe(
              unit,
              ARAD_CHIP_SIM_OEMA_BASE,
              key,
              ARAD_CHIP_SIM_OEMA_KEY
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }

#if SOC_DPP_IS_EM_HW_ENABLE

  res = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, PPDB_A_OEMA_INTERRUPT_REGISTER_ONEr,
                     REG_PORT_ANY, 0, OEMA_MANAGEMENT_COMPLETEDf, 1);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, READ_PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr(unit, &failure));
  fail_valid_val = soc_reg_field_get(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr, failure, OEMA_MNGMNT_UNIT_FAILURE_VALIDf);

  if (fail_valid_val) {
      
      fail_key_val = soc_reg_field_get(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr, failure, OEMA_MNGMNT_UNIT_FAILURE_KEYf);
      fail_reason_val = soc_reg_field_get(unit, PPDB_A_OEMA_MANAGEMENT_UNIT_FAILUREr, failure, OEMA_MNGMNT_UNIT_FAILURE_REASONf);
      ARAD_PP_OAM_OEM1_OEM2_FIND_ERRORS_AND_PRINT(fail_key_val, *key, fail_reason_val, 1);

  }

#endif
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem1_entry_delete_unsafe()", 0, 0);
}


uint32
  arad_pp_oam_classifier_oem2_entry_set_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY     * oem2_key,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD * oem2_payload
  )
{
  uint32  res;
  uint32  entry_buffer[2];
  uint32  reg_val; 
  uint32  key[1];
  uint32  payload[1];
  uint8  found;
#if SOC_DPP_IS_EM_HW_ENABLE
  uint64  failure, fail_valid_val, fail_reason_val64 , fail_key_val64 ;
  uint32  fail_reason_val , fail_key_val;

#endif
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oem2_key);
  SOC_SAND_CHECK_NULL_INPUT(oem2_payload);

  _ARAD_PP_OAM_OEM2_KEY_STRUCT_TO_KEY((*oem2_key), *key);
  _ARAD_PP_OAM_OEM2_PAYLOAD_STRUCT_TO_PAYLOAD((*oem2_payload), *payload);
  LOG_DEBUG(BSL_LS_SOC_OAM,
            (BSL_META_U(unit,
                        " Inserting entry to OEM2: key %d payload: %d\n\r"), *key, *payload));

  ARAD_PP_CLEAR(entry_buffer, uint32, 2);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_PPDB_A_OEMB_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0, entry_buffer));
  reg_val = 1; 
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_TYPEf, &reg_val);
  reg_val = 0; 
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_STAMPf, &reg_val);
  reg_val = 0; 
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_SELFf, &reg_val);
  reg_val = 0;
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_RESERVED_LOWf, &reg_val);
  reg_val = 3;
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_AGE_STATUSf, &reg_val);
  reg_val = *key;
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_KEYf, &reg_val);
  reg_val = *payload; 
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_PAYLOADf, &reg_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_PPDB_A_OEMB_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0, entry_buffer));

  if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
      res = chip_sim_exact_match_entry_add_unsafe(
              unit,
              ARAD_CHIP_SIM_OEMB_BASE,
              key,
              ARAD_CHIP_SIM_OEMB_KEY,
              payload,
              ARAD_CHIP_SIM_OEMB_PAYLOAD,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }

#if SOC_DPP_IS_EM_HW_ENABLE

  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          PPDB_A_OEMB_INTERRUPT_REGISTER_ONEr,
          REG_PORT_ANY,
          0,
          OEMB_MANAGEMENT_COMPLETEDf,
          1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, READ_PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr(unit, &failure));
  fail_valid_val = soc_reg64_field_get(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr, failure, OEMB_MNGMNT_UNIT_FAILURE_VALIDf);

  if (COMPILER_64_LO(fail_valid_val)){
             
      fail_key_val64 = soc_reg64_field_get(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr, failure, OEMB_MNGMNT_UNIT_FAILURE_KEYf);
      fail_reason_val64 = soc_reg64_field_get(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr, failure, OEMB_MNGMNT_UNIT_FAILURE_REASONf);
      fail_key_val = COMPILER_64_LO(fail_key_val64);
      fail_reason_val = COMPILER_64_LO(fail_reason_val64);
      ARAD_PP_OAM_OEM1_OEM2_FIND_ERRORS_AND_PRINT(fail_key_val, *key, fail_reason_val, 2);
  }
#endif
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem2_entry_set_unsafe()", 0, 0);
}


uint32
  arad_pp_oam_classifier_oem2_entry_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
    SOC_SAND_OUT  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  *oem2_payload,
    SOC_SAND_OUT  uint8                                      *is_found
  )
{

  uint32  res;
  uint32  key[1];
#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
  uint32  data_out[1];
#endif
  uint32  reg_buffer;
  uint32  fld_val;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oem2_key);
  SOC_SAND_CHECK_NULL_INPUT(oem2_payload);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  _ARAD_PP_OAM_OEM2_KEY_STRUCT_TO_KEY((*oem2_key), *key);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_A_OEMB_DIAGNOSTICS_KEYr, REG_PORT_ANY, 0, OEMB_DIAGNOSTICS_KEYf, *key));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_A_OEMB_DIAGNOSTICSr, REG_PORT_ANY, 0, OEMB_DIAGNOSTICS_LOOKUPf,  1));

#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
  if (SOC_DPP_IS_EM_SIM_ENABLE(unit)
#ifdef CRASH_RECOVERY_SUPPORT 
      || ((SOC_IS_DONE_INIT(unit)) && (BCM_UNIT_DO_HW_READ_WRITE(unit)))
#endif 
      )
  {
  

      res = chip_sim_exact_match_entry_get_unsafe(
              unit,
              ARAD_CHIP_SIM_OEMB_BASE,
              key,
              ARAD_CHIP_SIM_OEMB_KEY,
              data_out,
              ARAD_CHIP_SIM_OEMB_PAYLOAD,
              is_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

      if (*is_found) {
          _ARAD_PP_OAM_OEM2_PAYLOAD_TO_PAYLOAD_STRUCT(*data_out, (*oem2_payload));
          LOG_DEBUG(BSL_LS_SOC_OAM,
                    (BSL_META_U(unit,
                                " Getting entry to OEMq: key %d payload: %d\n\r"), *key, *data_out));
      }
      else {
          LOG_DEBUG(BSL_LS_SOC_OAM,
                    (BSL_META_U(unit,
                                " Getting entry to OEM2: key %d not found\n\r"), *key));
      }

      ARAD_PP_DO_NOTHING_AND_EXIT;
  }
#endif

   res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          PPDB_A_OEMB_DIAGNOSTICSr,
          REG_PORT_ANY,
          0,
          OEMB_DIAGNOSTICS_LOOKUPf,
          0
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_PPDB_A_OEMB_DIAGNOSTICS_LOOKUP_RESULTr(unit, &reg_buffer)); 

  ARAD_FLD_FROM_REG(PPDB_A_OEMB_DIAGNOSTICS_LOOKUP_RESULTr, OEMB_ENTRY_FOUNDf, fld_val, reg_buffer, 50, exit);
  *is_found = SOC_SAND_NUM2BOOL(fld_val);

  if (*is_found) {
      ARAD_FLD_FROM_REG(PPDB_A_OEMB_DIAGNOSTICS_LOOKUP_RESULTr, OEMB_ENTRY_PAYLOADf, fld_val, reg_buffer, 60, exit);
      _ARAD_PP_OAM_OEM2_PAYLOAD_TO_PAYLOAD_STRUCT(fld_val, (*oem2_payload));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem2_entry_get_unsafe()", 0, 0);
}


uint32
  arad_pp_oam_classifier_oem2_entry_delete_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key
  )
{

  uint32  res;
  uint32  reg_val;
  uint32  entry_buffer[2];
  uint32  key[1];
#if SOC_DPP_IS_EM_HW_ENABLE
  uint64  fail_valid_val, fail_key_val64, fail_reason_val64;
  uint64  failure;
  uint32  fail_reason_val ,fail_key_val;

#endif
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(oem2_key);
  ARAD_PP_CLEAR(entry_buffer, uint32, 2);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_PPDB_A_OEMB_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0, entry_buffer));
  reg_val = 0; 
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_TYPEf, &reg_val);
  reg_val = 0; 
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_STAMPf, &reg_val);
  _ARAD_PP_OAM_OEM2_KEY_STRUCT_TO_KEY((*oem2_key), *key);
  soc_PPDB_A_OEMB_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, OEMB_KEYf, key);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_PPDB_A_OEMB_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0, entry_buffer));

  if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
      res = chip_sim_exact_match_entry_remove_unsafe(
              unit,
              ARAD_CHIP_SIM_OEMB_BASE,
              key,
              ARAD_CHIP_SIM_OEMB_KEY
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }

#if SOC_DPP_IS_EM_HW_ENABLE

  res = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, PPDB_A_OEMB_INTERRUPT_REGISTER_ONEr,
                     REG_PORT_ANY, 0, OEMB_MANAGEMENT_COMPLETEDf, 1);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, READ_PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr(unit, &failure));
  fail_valid_val = soc_reg64_field_get(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr, failure, OEMB_MNGMNT_UNIT_FAILURE_VALIDf);

  if (COMPILER_64_LO(fail_valid_val)) {
      
 
      fail_key_val64 = soc_reg64_field_get(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr, failure, OEMB_MNGMNT_UNIT_FAILURE_KEYf);
      fail_reason_val64 = soc_reg64_field_get(unit, PPDB_A_OEMB_MANAGEMENT_UNIT_FAILUREr, failure, OEMB_MNGMNT_UNIT_FAILURE_REASONf);
      fail_key_val = COMPILER_64_LO(fail_key_val64);
      fail_reason_val = COMPILER_64_LO(fail_reason_val64);
      ARAD_PP_OAM_OEM1_OEM2_FIND_ERRORS_AND_PRINT(fail_key_val, *key, fail_reason_val, 2); 
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem2_entry_delete_unsafe()", 0, 0);
}


uint32
  arad_pp_oam_classifier_oam1_entries_insert_default_profile_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint8                            is_bfd
  )
{
    uint32 i;
    uint8  cpu_trap_code_to_mirror_profile_map;
    uint32  res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE_VERIFY);

    ARAD_STRUCT_VERIFY(SOC_PPC_OAM_MEP_PROFILE_DATA, &profile_data->mep_profile_data, 10, exit);
    ARAD_STRUCT_VERIFY(SOC_PPC_OAM_MEP_PROFILE_DATA, &profile_data->mip_profile_data, 20, exit);
    if (!is_bfd) {
        for (i=0; i<SOC_PPC_NOF_TRAP_CODES; i++) {
            res = sw_state_access[unit].dpp.bcm.oam.cpu_trap_code_to_mirror_profile_map.get(unit, i, &cpu_trap_code_to_mirror_profile_map);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);
            SOC_SAND_ERR_IF_ABOVE_MAX(cpu_trap_code_to_mirror_profile_map, DPP_MIRROR_ACTION_NDX_MAX, ARAD_PP_OAM_TRAP_TO_MIRROR_PROFILE_MAP_ERROR, 30, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_entries_insert_default_profile_verify()", 0, 0);
}

uint32
  arad_pp_oam_classifier_oam1_entries_insert_default_profile_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint8                            is_bfd
  )
{
  uint32 res;
  uint32 mep_type;
  uint32 is_my_cfm_mac;
  uint32 mip_type;
  uint8  internal_opcode;
  SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY       oam1_key;
  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD   oam_payload;
  uint32 oam_trap_code;
  uint8 fwd_strength = 0;
  _oam_oam_a_b_table_buffer_t oama_buffer;

  uint32 nof_mp_types_to_set = 1;

  SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO jr_mp_types[2];
  uint32 jr_mp_type_idx;
  uint32 mp_type_bfd;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE_UNSAFE);
  
  sal_memset(&oama_buffer,0, sizeof(_oam_oam_a_b_table_buffer_t) );
  
  res = arad_pp_oam_classifier_oam1_allocate_sw_buffer(unit,&oama_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);

  
  if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) && (!profile_data->is_default)) {
      res = arad_pp_oam_classifier_profile_reset(unit, ARAD_PP_OAM_PROFILE_DEFAULT, 0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);

      SOC_SAND_EXIT_NO_ERROR;
  }

  SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);
  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam_payload);

  

  
  oam1_key.inject = 0;
  oam1_key.is_bfd = is_bfd;
  oam1_key.mp_profile = classifier_mep_entry->non_acc_profile;
  oam1_key.ingress = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP)==0);
  jr_mp_types[0] = (oam1_key.ingress ?
                    SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO_INGRESS_MATCH :
                    SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO_EGRESS_MATCH);
  jr_mp_types[1] = SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO_INGRESS_EGRESS_MATCH;
  mp_type_bfd = SOC_IS_JERICHO_PLUS(unit) ? SOC_PPC_OAM_MP_TYPE_QAX_BFD : SOC_PPC_OAM_MP_TYPE_JERICHO_BFD;

  oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT_OAM_MESSAGE;
  oam_payload.snoop_strength = 0;
  oam_payload.forwarding_strength = _ARAD_PP_OAM_TRAP_STRENGTH;
  oam_payload.forward_disable = 1;
  oam_payload.up_map = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);
  if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
      oam_payload.mirror_enable = 1;
  }

  if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
      oam1_key.mp_type = SOC_PPC_OAM_MP_TYPE_MEP_OR_ACTIVE_MATCH_PLUS;
  }
  else if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)){ 
      oam1_key.mp_profile = ARAD_PP_OAM_PROFILE_DEFAULT;
      oam1_key.ingress = 1; 
      nof_mp_types_to_set = SOC_PPC_OAM_MP_TYPE_COUNT;
  }

  for (jr_mp_type_idx = 0; jr_mp_type_idx<=(SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)&&(!is_bfd)); jr_mp_type_idx++) {
      oam1_key.mp_type_jr = (is_bfd ? mp_type_bfd : jr_mp_types[jr_mp_type_idx]);
      for (mep_type = 0; mep_type < nof_mp_types_to_set; mep_type++) {
          oam1_key.mep_type = mep_type;
          for (mip_type = 0; mip_type <nof_mp_types_to_set; mip_type++) {
              oam1_key.mip_type = mip_type;
              if (is_bfd) {
                  oam1_key.my_cfm_mac = 0;
                  oam1_key.opcode = SOC_PPC_OAM_OPCODE_MAP_BFD;
                  res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM, &internal_opcode);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
                  res = _arad_pp_oam_set_counter_disable(unit ,internal_opcode, classifier_mep_entry->mep_type,(&oam_payload), &profile_data->mep_profile_data, 0);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
                  oam_payload.meter_disable = (SHR_BITGET(profile_data->mip_profile_data.meter_disable, internal_opcode) != 0);
                  res = _arad_pp_oam_trap_code_to_internal(unit,profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode], &(oam_payload.cpu_trap_code));
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
                  fwd_strength = profile_data->mep_profile_data.opcode_to_trap_strength_multicast_map[internal_opcode];
                  oam_payload.forwarding_strength = fwd_strength?fwd_strength:_ARAD_PP_OAM_TRAP_STRENGTH;
                  oam_payload.mirror_profile = 0;
                  res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
              }
              else {
                  for (is_my_cfm_mac = 0; is_my_cfm_mac <=1; is_my_cfm_mac++) {
                      oam1_key.my_cfm_mac = is_my_cfm_mac;
                      for (internal_opcode = 1; internal_opcode < SOC_PPC_OAM_OPCODE_MAP_COUNT;  internal_opcode++) {
                          oam1_key.opcode = internal_opcode;
                          res = _arad_pp_oam_set_counter_disable(unit ,internal_opcode, classifier_mep_entry->mep_type,(&oam_payload), &profile_data->mep_profile_data, 0);
                          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
                          oam_payload.meter_disable = (SHR_BITGET(profile_data->mip_profile_data.meter_disable, internal_opcode) != 0);
                          if (is_my_cfm_mac) {
                              res = _arad_pp_oam_trap_code_to_internal(unit,profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode], &oam_payload.cpu_trap_code);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
                              fwd_strength = profile_data->mep_profile_data.opcode_to_trap_strength_unicast_map[internal_opcode];
                              oam_payload.forwarding_strength = fwd_strength?fwd_strength:_ARAD_PP_OAM_TRAP_STRENGTH;
                              res = sw_state_access[unit].dpp.bcm.oam.cpu_trap_code_to_mirror_profile_map.get(unit,
                                    profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode],
                                    &oam_payload.mirror_profile);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);
                          }
                          else {
                              res = _arad_pp_oam_trap_code_to_internal(unit,profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode], &oam_payload.cpu_trap_code);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
                              fwd_strength = profile_data->mep_profile_data.opcode_to_trap_strength_multicast_map[internal_opcode];
                              oam_payload.forwarding_strength = fwd_strength?fwd_strength:_ARAD_PP_OAM_TRAP_STRENGTH;
                              res = sw_state_access[unit].dpp.bcm.oam.cpu_trap_code_to_mirror_profile_map.get(unit,
                                    profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode],
                                    &oam_payload.mirror_profile);
                              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);
                          }
                          res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
                      }
                  }
              }
          }
      }
  }

  
  jr_mp_types[0] = SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO_PASS;
  jr_mp_types[1] = SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO_ABOVE_ALL;
  oam1_key.mp_type = SOC_PPC_OAM_MP_TYPE_ABOVE_PLUS;
  oam1_key.mep_type = SOC_PPC_OAM_MP_TYPE_ABOVE;
  oam1_key.mip_type = SOC_PPC_OAM_MP_TYPE_ABOVE;

  res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_frwrd.get(unit, &oam_trap_code);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 281, exit);
  oam_payload.cpu_trap_code = oam_trap_code;
  oam_payload.forward_disable = 0;
  oam_payload.mirror_profile = 0;
  oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT;
  oam_payload.forwarding_strength = _ARAD_PP_OAM_FORWARD_TRAP_STRENGTH;

  for (jr_mp_type_idx = 0; jr_mp_type_idx<=SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit); jr_mp_type_idx++) {
      oam1_key.mp_type_jr = jr_mp_types[jr_mp_type_idx];
      for (is_my_cfm_mac = 0; is_my_cfm_mac <= 1; is_my_cfm_mac++) {
          oam1_key.my_cfm_mac = is_my_cfm_mac;
          for (internal_opcode = 1; internal_opcode < SOC_PPC_OAM_OPCODE_MAP_COUNT;  internal_opcode++) {
              oam1_key.opcode = internal_opcode;
              res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
              SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
          }
      }
  }

  res = arad_pp_oam_classifier_oam1_set_hw_unsafe(unit, &oama_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit); 

exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oama_buffer);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_entries_insert_default_profile_unsafe()", 0, 0);
}



STATIC uint32
  arad_pp_oam_classifier_oam1_2_entries_build_payload_unsafe(
    SOC_SAND_IN  int                                              unit,
    SOC_SAND_IN  uint32                                           internal_opcode,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY                 *classifier_mep_entry,
    SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA                     *profile_data,
    SOC_SAND_IN  uint8                                            is_unicast,
    SOC_SAND_INOUT  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD      *oam_payload
  )
{
    uint32 res;
    uint32 oam_trap_code;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO snoop_profile_info;
    uint32 oam_trap_code_level_err;
    uint8 fwd_strength = 0;
    uint8 opcode_action_snoop_strength = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    fwd_strength = is_unicast ? profile_data->mep_profile_data.opcode_to_trap_strength_unicast_map[internal_opcode] :
                                 profile_data->mep_profile_data.opcode_to_trap_strength_multicast_map[internal_opcode];
    oam_payload->meter_disable = (SHR_BITGET(profile_data->mep_profile_data.meter_disable, internal_opcode) != 0);
    oam_payload->forwarding_strength = fwd_strength?fwd_strength:_ARAD_PP_OAM_TRAP_STRENGTH;
    oam_payload->up_map = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);

    oam_payload->sub_type = internal_opcode_init[internal_opcode].sub_type;

    
    oam_trap_code = is_unicast ? profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] :
                                 profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode];
    if (oam_trap_code > 0) {
        res = _arad_pp_oam_trap_code_to_internal(unit,oam_trap_code, &(oam_payload->cpu_trap_code)); 
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
    }
    else {
        res = sw_state_access[unit].dpp.soc.arad.pp.bfd.trap_code_trap_to_cpu.get(unit, &oam_trap_code);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
        oam_payload->cpu_trap_code = oam_trap_code;
        
        oam_payload->forwarding_strength = _ARAD_PP_OAM_TRAP_STRENGTH;
    }

    if(((internal_opcode_init[internal_opcode].additional_info == ARAD_PP_OAM_OPCODE_LBM) || 
        
        ((internal_opcode_init[internal_opcode].additional_info == ARAD_PP_OAM_OPCODE_LBR) && !SOC_IS_JERICHO(unit))) &&
            (oam_trap_code == SOC_PPC_TRAP_CODE_OAM_OAMP_MIRROR)) {
        internal_opcode_init[internal_opcode].sub_type = _ARAD_PP_OAM_SUBTYPE_CCM;
        
        oam_payload->sub_type = _ARAD_PP_OAM_SUBTYPE_CCM;
    }
    res = sw_state_access[unit].dpp.bcm.oam.cpu_trap_code_to_mirror_profile_map.get(unit, oam_trap_code, &oam_payload->mirror_profile);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
    res = soc_ppd_trap_snoop_profile_info_get(unit, oam_trap_code, &snoop_profile_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

    opcode_action_snoop_strength = is_unicast ? profile_data->mep_profile_data.opcode_to_snoop_strength_unicast_map[internal_opcode] :
        profile_data->mep_profile_data.opcode_to_snoop_strength_multicast_map[internal_opcode];

    oam_payload->snoop_strength = opcode_action_snoop_strength;
    
    if (!oam_payload->up_map && (snoop_profile_info.snoop_cmnd > 0) && (opcode_action_snoop_strength) ) { 
        oam_payload->snoop_strength = _ARAD_PP_OAM_SNOOP_STRENGTH;
        oam_payload->sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT;
    }

    
    if (( SOC_DPP_CONFIG(unit)->pp.upmep_lbm_is_configured )&&  
        (is_unicast && oam_payload->up_map) && 
        (internal_opcode == SOC_PPC_OAM_OPCODE_MAP_LBM)){
	    oam_payload->sub_type = _ARAD_PP_OAM_SUBTYPE_ETH_LOOPBACK;	
    }

    res = _arad_pp_oam_set_counter_disable(unit,internal_opcode, classifier_mep_entry->mep_type,oam_payload, &profile_data->mep_profile_data, profile_data->is_piggybacked);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

    res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_trap_to_cpu_level.get(unit, &oam_trap_code_level_err);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 248, exit);

    
    if (oam_trap_code == oam_trap_code_level_err) {
        oam_payload->counter_disable = 1;
    }

    if ((oam_trap_code == SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP) || ((oam_payload->snoop_strength > 0) && !(oam_payload->up_map))) {
        
        oam_payload->forward_disable = 0;
    }
    else {
        oam_payload->forward_disable = 1;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_2_entries_insert_according_to_profile_mep_match_unsafe()", 0, 0);
}


STATIC uint32 arad_pp_oam_classifier_oam1_2_rfc6374_entries_build_payload(SOC_SAND_IN  int                                              unit,
                                                                          SOC_SAND_IN  uint32                                           internal_opcode,
                                                                          SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA                     *profile_data,
                                                                          SOC_SAND_INOUT  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD      *oam_payload)
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(profile_data);
  SOC_SAND_CHECK_NULL_INPUT(oam_payload);

  
  res = _arad_pp_oam_trap_code_to_internal(unit, profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode],
                                           &(oam_payload->cpu_trap_code));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

  oam_payload->forwarding_strength = _ARAD_PP_OAM_TRAP_STRENGTH; 
  oam_payload->snoop_strength = 0;

  oam_payload->up_map = 0; 
  oam_payload->sub_type = internal_opcode_init[internal_opcode].sub_type;
  oam_payload->meter_disable = (SHR_BITGET(profile_data->mep_profile_data.meter_disable, internal_opcode) != 0);
  oam_payload->counter_disable = (SHR_BITGET(profile_data->mep_profile_data.counter_disable, internal_opcode) != 0);

  if ((internal_opcode_init[internal_opcode].additional_info == ARAD_PP_OAM_OPCODE_LM) ||
      (SOC_IS_JERICHO_PLUS(unit) && (internal_opcode_init[internal_opcode].additional_info == ARAD_PP_OAM_OPCODE_SLM))) {
        oam_payload->counter_disable = 0; 
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_2_rfc6374_entries_build_payload()", 0, 0);
}


uint32 arad_pp_oam_classifier_insert_according_to_profile_verify( SOC_SAND_IN  int                             unit,
                                                                  SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                  SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data )
{
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_STRUCT_VERIFY(SOC_PPC_OAM_MEP_PROFILE_DATA, &profile_data->mep_profile_data, 10, exit);
    ARAD_STRUCT_VERIFY(SOC_PPC_OAM_MEP_PROFILE_DATA, &profile_data->mip_profile_data, 15, exit);
    if (((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) == 0)) {
        ARAD_STRUCT_VERIFY(SOC_PPC_OAM_MEP_PROFILE_DATA, &profile_data->mip_profile_data, 20, exit);
    }
    res = SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_verify(unit, classifier_mep_entry);
    SOC_SAND_CHECK_FUNC_RESULT(res , 30, exit);

    if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile_passive, SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
    }
    else {
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPC_OAM_NON_ACC_PROFILES_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 65, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oam1_2_entries_insert_according_to_profile_verify()", 0, 0);
}


uint32 arad_pp_oam_classifier_oam1_entries_insert_bfd_according_to_profile( SOC_SAND_IN int                           unit,
                                                                            SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                            SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data)
{
    uint32 res;
    uint8 internal_opcode;
    uint32 oam_trap_code;
    uint8 fwd_strength = 0;
    uint32 mp_type;
    _oam_oam_a_b_table_buffer_t oam_buffer ;

    SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY       oam1_key       ;
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD    oam_payload    ;

    SOCDNX_INIT_FUNC_DEFS;
    sal_memset(&oam_buffer,0, sizeof(_oam_oam_a_b_table_buffer_t) );

    res = arad_pp_oam_classifier_insert_according_to_profile_verify(unit, classifier_mep_entry, profile_data);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam_payload);

    
    res = arad_pp_oam_classifier_oam1_allocate_sw_buffer(unit, &oam_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    oam1_key.inject = 0;
    oam1_key.is_bfd = 1;
    oam1_key.ingress = 1;
    oam1_key.mp_profile = classifier_mep_entry->non_acc_profile;
    oam1_key.my_cfm_mac = 0;

    res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM, &internal_opcode);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    oam1_key.opcode = 0;
    oam_payload.up_map = 0;
    oam_payload.meter_disable = 0;

    oam_trap_code = profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode];
    res = _arad_pp_oam_trap_code_to_internal(unit, oam_trap_code, &(oam_payload.cpu_trap_code));
    SOCDNX_SAND_IF_ERR_EXIT(res);
    fwd_strength = profile_data->mep_profile_data.opcode_to_trap_strength_multicast_map[internal_opcode];
    oam_payload.forwarding_strength = fwd_strength?fwd_strength:_ARAD_PP_OAM_TRAP_STRENGTH;

    oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_CCM;
    oam1_key.mip_type = SOC_PPC_OAM_MP_TYPE_ABOVE;

    if (SOC_IS_JERICHO(unit)) {
        oam1_key.mp_type_jr = SOC_IS_JERICHO_PLUS(unit) ? SOC_PPC_OAM_MP_TYPE_QAX_BFD : SOC_PPC_OAM_MP_TYPE_JERICHO_BFD;
        res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oam_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(res);
    } else {
        
        for (mp_type = 0; mp_type < SOC_PPC_OAM_MP_TYPE_COUNT_PLUS; mp_type++) {
            if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                oam1_key.mep_type = mp_type; 
            } else {
                oam1_key.mp_type = mp_type;
            }

            res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oam_buffer);
            SOCDNX_SAND_IF_ERR_EXIT(res);
        }
    }

    res = arad_pp_oam_classifier_oam1_set_hw_unsafe(unit, &oam_buffer);

exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oam_buffer);
    SOCDNX_FUNC_RETURN;
}




soc_error_t arad_pp_oam_classifier1_mip_passive_entries(int unit, const  SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data) {
    int res;
    int is_my_cfm_mac, internal_opcode;
    int mep_type;
    SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY       oam1_key;
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD    oam_payload; 
    uint32 oam_trap_code ;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO snoop_profile_info;
    _oam_oam_a_b_table_buffer_t  oam_buffer={0};
    uint32 trap_code_level;
   uint8 egress_snooping_advanced_mode = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0);
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam_payload);

    res = arad_pp_oam_classifier_oam1_allocate_sw_buffer(unit, &oam_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    oam1_key.inject = 0;
    oam1_key.is_bfd = 0;
    oam1_key.ingress = 0;
    oam1_key.mp_profile = ARAD_PP_OAM_PROFILE_PASSIVE;
    oam1_key.mip_type = SOC_PPC_OAM_MP_TYPE_MATCH;


     
    res = sw_state_access[unit].dpp.bcm.oam.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_ERR_LEVEL, &trap_code_level);
    SOCDNX_SAND_IF_ERR_EXIT(res);


    for (is_my_cfm_mac =0 ; is_my_cfm_mac<=1; ++is_my_cfm_mac) {
        for (internal_opcode = 1; internal_opcode < SOC_PPC_OAM_OPCODE_MAP_COUNT; internal_opcode++) {
            for (mep_type = 0 ; mep_type < SOC_PPC_OAM_MP_TYPE_COUNT ; ++mep_type) {
                oam1_key.my_cfm_mac = is_my_cfm_mac;
                oam1_key.mep_type = mep_type;
                oam1_key.opcode = internal_opcode;
                res = _arad_pp_oam_set_counter_disable(unit,internal_opcode, 0,(&oam_payload), &profile_data->mip_profile_data, 0);
                SOCDNX_SAND_IF_ERR_EXIT(res);
                oam_payload.meter_disable =0;
                oam_payload.forward_disable = 1; 
                oam_payload.snoop_strength = 0; 
                oam_trap_code = is_my_cfm_mac ? profile_data->mip_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] :
                                                profile_data->mip_profile_data.opcode_to_trap_code_multicast_map[internal_opcode];
                
                oam_payload.sub_type = (oam_trap_code == trap_code_level || oam_trap_code == SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP) ?
                    _ARAD_PP_OAM_SUBTYPE_DEFAULT : _ARAD_PP_OAM_SUBTYPE_DEFAULT_OAM_MESSAGE; 

                res = sw_state_access[unit].dpp.bcm.oam.cpu_trap_code_to_mirror_profile_map.get(unit, oam_trap_code, &oam_payload.mirror_profile);
                SOCDNX_SAND_IF_ERR_EXIT(res);

                res = soc_ppd_trap_snoop_profile_info_get(unit, oam_trap_code, &snoop_profile_info);
                SOCDNX_SAND_IF_ERR_EXIT(res);
                if (snoop_profile_info.snoop_cmnd > 0) {
                    oam_payload.snoop_strength = _ARAD_PP_OAM_SNOOP_STRENGTH;
                    oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT;
                }
                if (oam_trap_code == SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP ||
                    (oam_payload.snoop_strength && !egress_snooping_advanced_mode)) {
                    
                    oam_payload.forward_disable = 0;
                }
                if (egress_snooping_advanced_mode &&
                    !oam1_key.ingress &&
                    oam_payload.snoop_strength) {
                    
                    oam_payload.sub_type = ARAD_PP_OAM_SUBTYPE_EGRESS_SNOOP;
                }
                
                res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oam_buffer);
                SOCDNX_SAND_IF_ERR_EXIT(res); 

            }
        }
    }

    res = arad_pp_oam_classifier_oam1_set_hw_unsafe(unit, &oam_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res); 

exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oam_buffer);
    SOCDNX_FUNC_RETURN;
}



uint32 arad_pp_oam_classifier_oam1_entries_insert_oam_non_acc_according_to_profile( SOC_SAND_IN  int                           unit,
                                                                                    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                                    SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
                                                                                    SOC_SAND_IN  uint8 is_passive,
                                                                                    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t * oam_buffer)
{
    uint32 res;
    uint32 mp_type;
    uint32 is_my_cfm_mac;
    uint32 oam_trap_code;
    uint8 fwd_strength = 0, snp_strength = 0;
    uint32 oam_mirror_profile;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO snoop_profile_info;
    uint8 internal_opcode;
    uint32 trap_code_level;
    uint8 is_upmep;
    uint8 egress_snooping_advanced_mode = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0);

    SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY       oam1_key       ;
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD    oam_payload    ;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam_payload);

    is_upmep = (is_passive^((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0));

    oam1_key.inject = 0;
    oam1_key.is_bfd = 0;
    oam1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
    oam1_key.mp_profile = is_passive ? classifier_mep_entry->non_acc_profile_passive : classifier_mep_entry->non_acc_profile;

    if (IS_INGRESS_WHEN_ACTIVE(is_upmep)) { 
        oam_payload.snoop_strength = 0;
    }

    
    res = sw_state_access[unit].dpp.bcm.oam.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_ERR_LEVEL, &trap_code_level);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    for (is_my_cfm_mac = 0; is_my_cfm_mac <= 1; is_my_cfm_mac++) {
        oam1_key.my_cfm_mac = is_my_cfm_mac;

        for (internal_opcode = 1; internal_opcode < SOC_PPC_OAM_OPCODE_MAP_COUNT; internal_opcode++) {
            oam1_key.opcode = internal_opcode;
            for (mp_type = 0; mp_type < SOC_PPC_OAM_MP_TYPE_COUNT_PLUS; mp_type++) {
                oam_payload.mirror_enable = 1; 
                oam_payload.forward_disable = 1; 
                oam_payload.counter_disable = 1; 
                if (IS_INGRESS_WHEN_ACTIVE(is_upmep)) { 
                    oam_payload.forwarding_strength = _ARAD_PP_OAM_TRAP_STRENGTH;
                }
                
                oam_payload.up_map = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);
                
                switch (mp_type) {
                case SOC_PPC_OAM_MP_TYPE_MEP_OR_ACTIVE_MATCH_PLUS: 
                    if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) { 
                        
                        oam1_key.mep_type = SOC_PPC_OAM_MP_TYPE_MATCH;
                        for (oam1_key.mip_type = SOC_PPC_OAM_MP_TYPE_ABOVE; oam1_key.mip_type <= SOC_PPC_OAM_MP_TYPE_BELOW; oam1_key.mip_type++) {
                            res = arad_pp_oam_classifier_oam1_2_entries_build_payload_unsafe(unit,
                                                                                             internal_opcode,
                                                                                             classifier_mep_entry,
                                                                                             profile_data,
                                                                                             is_my_cfm_mac,
                                                                                             &oam_payload
                                                                                            );
                            SOCDNX_SAND_IF_ERR_EXIT(res);
                            res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oam_buffer);
                            SOCDNX_SAND_IF_ERR_EXIT(res);
                        }
                      }
                      else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
                        
                        oam1_key.mp_type  = SOC_PPC_OAM_MP_TYPE_MEP_OR_ACTIVE_MATCH_PLUS;
                        oam_payload.sub_type = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
                                                                                "oam_additional_FTMH_on_error_packets", 0) ?
                                                        _ARAD_PP_OAM_SUBTYPE_CCM : _ARAD_PP_OAM_SUBTYPE_DEFAULT;
                        oam_payload.snoop_strength = 0;
                        res = sw_state_access[unit].dpp.soc.arad.pp.oam.mirror_profile_err_passive.get(unit, &oam_mirror_profile);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                        oam_payload.mirror_profile = oam_mirror_profile;
                        res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_trap_to_cpu_passive.get(unit, &oam_trap_code);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                        oam_payload.cpu_trap_code = oam_trap_code;
                        oam_payload.forwarding_strength = _ARAD_PP_OAM_PASSIVE_TRAP_STRENGTH;
                        res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oam_buffer);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                      }
                      else { 
                        oam1_key.mp_type  = SOC_PPC_OAM_MP_TYPE_MEP_OR_ACTIVE_MATCH_PLUS;
                        oam1_key.mp_type_jr = SOC_PPC_OAM_MP_TYPE_JERICHO_ACTIVE_MATCH;
                        res = arad_pp_oam_classifier_oam1_2_entries_build_payload_unsafe(unit,
                                                                                         internal_opcode,
                                                                                         classifier_mep_entry,
                                                                                         profile_data,
                                                                                         is_my_cfm_mac,
                                                                                         &oam_payload
                                                                                        );
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                        
                        oam_payload.up_map = 0;
                        res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oam_buffer);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                    }

                    break;
                case SOC_PPC_OAM_MP_TYPE_MIP_OR_PASSIVE_MATCH_PLUS:
                    if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                        
                        oam1_key.mip_type = SOC_PPC_OAM_MP_TYPE_MATCH;
                        oam1_key.mep_type  = SOC_PPC_OAM_MP_TYPE_ABOVE;
                        if (is_passive) {
                            
                            oam1_key.mp_profile = ARAD_PP_OAM_PROFILE_PASSIVE;
                        }
                    }
                      else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
                        oam1_key.mp_type  = SOC_PPC_OAM_MP_TYPE_MIP_OR_PASSIVE_MATCH_PLUS;
                    }

                    if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) || (!profile_data->mp_type_passive_active_mix) || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)
                        || SOC_DPP_CONFIG(unit)->pp.oam_mip_level_filtering) { 
                        
                        oam1_key.mp_type_jr = SOC_PPC_OAM_MP_TYPE_JERICHO_MIP_MATCH; 
                        res = _arad_pp_oam_set_counter_disable(unit, internal_opcode, classifier_mep_entry->mep_type,(&oam_payload), &profile_data->mip_profile_data, 0);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                        oam_payload.meter_disable = (SHR_BITGET(profile_data->mip_profile_data.meter_disable, internal_opcode) != 0);
                        oam_trap_code = is_my_cfm_mac ? profile_data->mip_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] :
                                                        profile_data->mip_profile_data.opcode_to_trap_code_multicast_map[internal_opcode];
                        fwd_strength = is_my_cfm_mac ? profile_data->mip_profile_data.opcode_to_trap_strength_unicast_map[internal_opcode] :
                                                        profile_data->mip_profile_data.opcode_to_trap_strength_multicast_map[internal_opcode];
                        oam_payload.forwarding_strength = fwd_strength?fwd_strength:_ARAD_PP_OAM_TRAP_STRENGTH;
                        
                        oam_payload.sub_type = (oam_trap_code == trap_code_level || oam_trap_code == SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP) ?
                                                        _ARAD_PP_OAM_SUBTYPE_DEFAULT : _ARAD_PP_OAM_SUBTYPE_DEFAULT_OAM_MESSAGE;
                        res = _arad_pp_oam_trap_code_to_internal(unit, oam_trap_code, &oam_payload.cpu_trap_code);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                        res = sw_state_access[unit].dpp.bcm.oam.cpu_trap_code_to_mirror_profile_map.get(unit, oam_trap_code, &oam_payload.mirror_profile);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                        if (SOC_IS_JERICHO(unit) && (!egress_snooping_advanced_mode) && (oam_trap_code == SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP)) {
                            
                            res = sw_state_access[unit].dpp.bcm.oam.cpu_trap_code_to_mirror_profile_map.get(unit, SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP_UP, &oam_payload.mirror_profile);
                            SOCDNX_SAND_IF_ERR_EXIT(res);
                        }
                        snp_strength = is_my_cfm_mac ? profile_data->mip_profile_data.opcode_to_snoop_strength_unicast_map[internal_opcode] :
                                                       profile_data->mip_profile_data.opcode_to_snoop_strength_multicast_map[internal_opcode];
                        res = soc_ppd_trap_snoop_profile_info_get(unit, oam_trap_code, &snoop_profile_info);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                        if (snoop_profile_info.snoop_cmnd > 0 || snp_strength) {
                            oam_payload.snoop_strength = snp_strength ? snp_strength : _ARAD_PP_OAM_SNOOP_STRENGTH;
                            if (snoop_profile_info.snoop_cmnd > 0) {
                                oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT;
                            }
                        }
                        if ((oam_trap_code == SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP) ||
                            ((oam_payload.snoop_strength) && !egress_snooping_advanced_mode)) {
                            
                            oam_payload.forward_disable = 0;
                        }
                        if (egress_snooping_advanced_mode &&
                            !oam1_key.ingress &&
                            oam_payload.snoop_strength) {
                            
                            oam_payload.sub_type = ARAD_PP_OAM_SUBTYPE_EGRESS_SNOOP;
                        }
                        res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oam_buffer);
                        SOCDNX_SAND_IF_ERR_EXIT(res);

                        
                        if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
                            oam1_key.mp_profile =  classifier_mep_entry->non_acc_profile_passive; 
                            oam1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
                            if (egress_snooping_advanced_mode && !oam1_key.ingress && oam_payload.snoop_strength) {
                                
                                oam_payload.sub_type = ARAD_PP_OAM_SUBTYPE_EGRESS_SNOOP;
                            }
                            res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oam_buffer);
                            SOCDNX_SAND_IF_ERR_EXIT(res);
                        }
                        
                        oam1_key.mp_profile = is_passive ? classifier_mep_entry->non_acc_profile_passive : classifier_mep_entry->non_acc_profile;
                        oam1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep); 
                    } else if ((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && (profile_data->mp_type_passive_active_mix))) {
                        
                        oam_payload.sub_type = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
                                                                           "oam_additional_FTMH_on_error_packets", 0) ?
                           _ARAD_PP_OAM_SUBTYPE_CCM : _ARAD_PP_OAM_SUBTYPE_DEFAULT;
                        oam_payload.snoop_strength = 0;
                        oam1_key.mp_type_jr = SOC_PPC_OAM_MP_TYPE_JERICHO_PASSIVE_MATCH; 
                        res = sw_state_access[unit].dpp.soc.arad.pp.oam.mirror_profile_err_passive.get(unit, &oam_mirror_profile);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                        oam_payload.mirror_profile = oam_mirror_profile;
                        res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_trap_to_cpu_passive.get(unit, &oam_trap_code);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                        oam_payload.cpu_trap_code = oam_trap_code;
                        oam_payload.forwarding_strength = _ARAD_PP_OAM_PASSIVE_TRAP_STRENGTH;
                        
                        if (oam1_key.ingress) {
                            oam_payload.up_map = 1;
                        } else {
                            oam_payload.up_map = 0;
                        }

                        res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oam_buffer);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                    }
                    break;
                case SOC_PPC_OAM_MP_TYPE_BELOW_PLUS: 
                    oam_payload.sub_type = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
                                                                            "oam_additional_FTMH_on_error_packets", 0) ?
                                                        _ARAD_PP_OAM_SUBTYPE_CCM : _ARAD_PP_OAM_SUBTYPE_DEFAULT;
                    
                    oam_payload.up_map = 0;
                    if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                        
                        for (oam1_key.mep_type = SOC_PPC_OAM_MP_TYPE_BELOW;
                              oam1_key.mep_type < SOC_PPC_OAM_MP_TYPE_COUNT;
                              oam1_key.mep_type++) {
                            for (oam1_key.mip_type = SOC_PPC_OAM_MP_TYPE_MATCH;
                                  oam1_key.mip_type < SOC_PPC_OAM_MP_TYPE_COUNT;
                                  oam1_key.mip_type++) {
                                oam_payload.snoop_strength = 0;
                                res = sw_state_access[unit].dpp.soc.arad.pp.oam.mirror_profile_err_level.get(unit, &oam_mirror_profile);
                                SOCDNX_SAND_IF_ERR_EXIT(res);
                                oam_payload.mirror_profile = oam_mirror_profile;
                                res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_trap_to_cpu_level.get(unit, &oam_trap_code);
                                SOCDNX_SAND_IF_ERR_EXIT(res);
                                oam_payload.cpu_trap_code = oam_trap_code;
                                oam_payload.forwarding_strength = _ARAD_PP_OAM_LEVEL_TRAP_STRENGTH;
                                res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oam_buffer);
                                SOCDNX_SAND_IF_ERR_EXIT(res);
                            }
                        }
                      }
                      else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) { 
                        oam1_key.mp_type  = SOC_PPC_OAM_MP_TYPE_BELOW_PLUS; 
                        oam_payload.snoop_strength = 0;
                        res = sw_state_access[unit].dpp.soc.arad.pp.oam.mirror_profile_err_level.get(unit, &oam_mirror_profile);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                        oam_payload.mirror_profile = oam_mirror_profile;
                        res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_trap_to_cpu_level.get(unit, &oam_trap_code);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                        oam_payload.cpu_trap_code = oam_trap_code;
                        oam_payload.forwarding_strength = _ARAD_PP_OAM_LEVEL_TRAP_STRENGTH;
                        res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oam_buffer);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                    }
                    break;
                case SOC_PPC_OAM_MP_TYPE_ABOVE_PLUS: 
                    if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                        
                        oam1_key.mep_type = SOC_PPC_OAM_MP_TYPE_ABOVE;
                        for (oam1_key.mip_type = SOC_PPC_OAM_MP_TYPE_ABOVE; oam1_key.mip_type < SOC_PPC_OAM_MP_TYPE_COUNT; oam1_key.mip_type++) {
                            
                            res = arad_pp_oam_classifier_oam1_entry_get_unsafe(unit, &oam1_key, &oam_payload);
                            SOCDNX_SAND_IF_ERR_EXIT(res);
                            oam_payload.counter_disable = (SHR_BITGET(profile_data->mep_profile_data.counter_disable, internal_opcode) != 0);
                            oam_payload.meter_disable = (SHR_BITGET(profile_data->mep_profile_data.meter_disable, internal_opcode) != 0);
                            res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oam_buffer);
                            SOCDNX_SAND_IF_ERR_EXIT(res);
                        }
                      }
                      else {  
                        oam1_key.mp_type  = SOC_PPC_OAM_MP_TYPE_ABOVE_PLUS;
                        oam1_key.mp_type_jr = SOC_IS_JERICHO_PLUS(unit) ?
                            SOC_PPC_OAM_MP_TYPE_QAX_ABOVE_ALL :
                            SOC_PPC_OAM_MP_TYPE_JERICHO_ABOVE_ALL;
                        
                        oam_payload.forward_disable = 0;
                        oam_payload.mirror_enable = 0;
                        oam_payload.snoop_strength = 0;
                        oam_payload.mirror_profile = 0;
                        oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT;
                        res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_frwrd.get(unit, &oam_trap_code);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                        oam_payload.cpu_trap_code = oam_trap_code;
                        oam_payload.forwarding_strength = _ARAD_PP_OAM_FORWARD_TRAP_STRENGTH;
                        oam_payload.counter_disable = 0;
                        oam_payload.meter_disable = (SHR_BITGET(profile_data->mep_profile_data.meter_disable, internal_opcode) != 0);
                        res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oam_buffer);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                    }
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 arad_pp_oam_classifier_oam1_rfc6374_entries_insert_oam_non_acc_according_to_profile(SOC_SAND_IN  int                                unit,
                                                                                           SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                                           SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
                                                                                           SOC_SAND_OUT _oam_oam_a_b_table_buffer_t        *oama_buffer)
{
    int rv;
    uint32 res;
    uint8 internal_opcode;
    int opcode_index;
    int opcode_count = 3;
    uint32 opcodes[] = {SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMM};
    SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY oam1_key;
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD oam1_payload;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam1_payload);

    oam1_key.ingress = 1;
    oam1_key.is_bfd = 1; 
    oam1_key.my_cfm_mac = 0; 
    oam1_key.inject = 0;
    oam1_key.mp_profile = classifier_mep_entry->non_acc_profile;
    oam1_key.mp_type_jr = SOC_IS_JERICHO_PLUS(unit) ? SOC_PPC_OAM_MP_TYPE_QAX_BFD : SOC_PPC_OAM_MP_TYPE_JERICHO_BFD;

    for (opcode_index = 0; opcode_index < opcode_count; ++opcode_index) {
        rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, opcodes[opcode_index], &internal_opcode);
        BCMDNX_IF_ERR_EXIT(rv);

        oam1_key.opcode = internal_opcode;

        res = arad_pp_oam_classifier_oam1_2_rfc6374_entries_build_payload(unit,
                                                                          internal_opcode,
                                                                          profile_data,
                                                                          &oam1_payload);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam1_payload, oama_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 arad_pp_oam_classifier_oam2_entries_insert_accelerated_according_to_profile( SOC_SAND_IN int                                          unit,
                                                                                    SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY            *classifier_mep_entry,
                                                                                    SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA                *profile_data,
                                                                                    SOC_SAND_INOUT  _oam_oam_a_b_table_buffer_t *oam_buffer)
{
    uint32 res;
    uint8 internal_opcode;
    uint32 oam_trap_code;
    uint8 fwd_strength = 0;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO snoop_profile_info;
    uint32 is_my_cfm_mac;

    SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY       oam2_key       ;
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD    oam_payload    ;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY_clear(&oam2_key);
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam_payload);

    
    oam2_key.inject = 0;
    oam2_key.is_bfd = SOC_PPC_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type);
    oam2_key.ingress = !(classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP);
    oam2_key.mp_profile = classifier_mep_entry->acc_profile;

    if (oam2_key.is_bfd) { 
        oam2_key.my_cfm_mac = 0;
        res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM, &internal_opcode);
        SOCDNX_SAND_IF_ERR_EXIT(res);
        fwd_strength = profile_data->mep_profile_data.opcode_to_trap_strength_multicast_map[internal_opcode];
        oam2_key.opcode = SOC_PPC_OAM_OPCODE_MAP_BFD;
        oam_payload.forwarding_strength = fwd_strength?fwd_strength:_ARAD_PP_OAM_TRAP_STRENGTH;
        oam_payload.up_map = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0 );
        oam_payload.meter_disable = 0;

        oam_trap_code = profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode];
        res = _arad_pp_oam_trap_code_to_internal(unit, oam_trap_code, &(oam_payload.cpu_trap_code));
        SOCDNX_SAND_IF_ERR_EXIT(res);
        oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_CCM;

        res = soc_ppd_trap_snoop_profile_info_get(unit, oam_trap_code, &snoop_profile_info);
        SOCDNX_SAND_IF_ERR_EXIT(res);
        if (snoop_profile_info.snoop_cmnd > 0) {
            oam_payload.snoop_strength = _ARAD_PP_OAM_SNOOP_STRENGTH;
        }

        res = arad_pp_oam_classifier_oam2_entry_set_on_buffer(unit, &oam2_key, &oam_payload, oam_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }
    else { 
        for (is_my_cfm_mac = 0; is_my_cfm_mac <=1; is_my_cfm_mac++) {
            oam2_key.my_cfm_mac = is_my_cfm_mac;
            for (internal_opcode = 1; internal_opcode < SOC_PPC_OAM_OPCODE_MAP_COUNT; internal_opcode++) {
                oam2_key.opcode = internal_opcode;

                res = arad_pp_oam_classifier_oam1_2_entries_build_payload_unsafe(unit,
                                                                                 internal_opcode,
                                                                                 classifier_mep_entry,
                                                                                 profile_data,
                                                                                 is_my_cfm_mac,
                                                                                 &oam_payload
                                                                                 );

                res = arad_pp_oam_classifier_oam2_entry_set_on_buffer(unit, &oam2_key, &oam_payload, oam_buffer);
                SOCDNX_SAND_IF_ERR_EXIT(res);
            }
        }
    }

exit:

    SOCDNX_FUNC_RETURN;
}


uint32 arad_pp_oam_classifier_oam2_rfc6374_entries_insert_accelerated_according_to_profile(SOC_SAND_IN int                                 unit,
                                                                                           SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY    *classifier_mep_entry,
                                                                                           SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA        *profile_data,
                                                                                           SOC_SAND_INOUT  _oam_oam_a_b_table_buffer_t     *oamb_buffer)
{
    int rv;
    uint32 res;
    uint8 internal_opcode;
    int opcode_index;
    int opcode_count = 3;
    uint32 opcodes[] = {SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMM};
    SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY oam2_key;
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD oam2_payload;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY_clear(&oam2_key);
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam2_payload);

    oam2_key.ingress = 1;
    oam2_key.is_bfd = 1; 
    oam2_key.my_cfm_mac = 0; 
    oam2_key.inject = 0;
    oam2_key.mp_profile = classifier_mep_entry->acc_profile;

    for (opcode_index = 0; opcode_index < opcode_count; ++opcode_index) {
        rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, opcodes[opcode_index], &internal_opcode);
        BCMDNX_IF_ERR_EXIT(rv);

        oam2_key.opcode = internal_opcode;

        res = arad_pp_oam_classifier_oam1_2_rfc6374_entries_build_payload(unit,
                                                                          internal_opcode,
                                                                          profile_data,
                                                                          &oam2_payload);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        res = arad_pp_oam_classifier_oam2_entry_set_on_buffer(unit, &oam2_key, &oam2_payload, oamb_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 arad_pp_oam_classifier_oam1_2_entries_insert_injected_according_to_profile( SOC_SAND_IN    int                                unit,
                                                                                   SOC_SAND_IN    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                                   SOC_SAND_IN    SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
                                                                                   SOC_SAND_INOUT  _oam_oam_a_b_table_buffer_t       *oama_buffer,
                                                                                   SOC_SAND_INOUT  _oam_oam_a_b_table_buffer_t       *oamb_buffer)
{
    uint32 res;
    int mep_prof;
    uint32 mep_type;
    uint32 is_my_cfm_mac;
    uint32 mip_type;
    uint32 oam_trap_code;
    uint8 internal_opcode;

    SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY    oam1_key;
    SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY    oam2_key;
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD oam_payload;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);
    SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY_clear(&oam2_key);
    SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam_payload);

    oam1_key.inject = 1;
    oam1_key.is_bfd = 0;
    oam1_key.ingress = TRUE; 

    oam_payload.forward_disable = 0;
    res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_frwrd.get(unit, &oam_trap_code);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    oam_payload.cpu_trap_code = oam_trap_code;
    oam_payload.mirror_profile = 0;
    oam_payload.meter_disable = 1;
    oam_payload.forwarding_strength = _ARAD_PP_OAM_INJECTED_TRAP_STRENGTH;
    oam_payload.snoop_strength = 0;
    oam_payload.up_map = 1;

    for (is_my_cfm_mac = 0; is_my_cfm_mac <=1; is_my_cfm_mac++) {
        oam1_key.my_cfm_mac = is_my_cfm_mac;
        for (internal_opcode = 1; internal_opcode < SOC_PPC_OAM_OPCODE_MAP_COUNT; internal_opcode++) {
            oam1_key.opcode = internal_opcode;
            if ((internal_opcode == SOC_PPC_OAM_OPCODE_MAP_CCM) &&
            (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_default_piggyback_lm_enable", 0) == 1)) {
                oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_LM;
            } else {
                oam_payload.sub_type = internal_opcode_init[internal_opcode].sub_type;
            }
            
            if ((oam_payload.sub_type == _ARAD_PP_OAM_SUBTYPE_LM ||
                 oam_payload.sub_type == _ARAD_PP_OAM_SUBTYPE_SLM) &&
                !(profile_data->flags & SOC_PPC_OAM_LIF_PROFILE_FLAG_COUNTED) &&
                (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_lm_enable", 0) == 1)) {
                oam_payload.sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT_OAM_MESSAGE;
            }
            res =_arad_pp_oam_set_counter_disable(unit, internal_opcode,classifier_mep_entry->mep_type, (&oam_payload), &profile_data->mep_profile_data, profile_data->is_piggybacked);
            SOCDNX_SAND_IF_ERR_EXIT(res);

            if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                

                for (mep_type = 0; mep_type < SOC_PPC_OAM_MP_TYPE_COUNT; mep_type++) {
                    for (mip_type = 0; mip_type < SOC_PPC_OAM_MP_TYPE_COUNT; mip_type++) {
                        oam1_key.mep_type = mep_type;
                        oam1_key.mip_type = mip_type;
                        
                        oam1_key.mp_profile = classifier_mep_entry->non_acc_profile_passive;
                        res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oama_buffer);
                        SOCDNX_SAND_IF_ERR_EXIT(res);

                        oam1_key.mp_profile =  ARAD_PP_OAM_PROFILE_PASSIVE;
                        res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oama_buffer);
                        SOCDNX_SAND_IF_ERR_EXIT(res);

                    }
                }
            } else {
                
                uint32 possible_mp_types_jer[] = { SOC_PPC_OAM_MP_TYPE_JERICHO_ACTIVE_MATCH, SOC_PPC_OAM_MP_TYPE_JERICHO_PASSIVE_MATCH };
                
                uint32 possible_mp_types_qax[] = { SOC_PPC_OAM_MP_TYPE_QAX_ACTIVE_MATCH, SOC_PPC_OAM_MP_TYPE_QAX_PASSIVE_MATCH };
                
                uint32 possible_mp_types_plus[] = { SOC_PPC_OAM_MP_TYPE_MEP_OR_ACTIVE_MATCH_PLUS, SOC_PPC_OAM_MP_TYPE_MIP_OR_PASSIVE_MATCH_PLUS };
                uint32 *possible_mp_types = SOC_IS_JERICHO_PLUS(unit) ? possible_mp_types_qax : possible_mp_types_jer;
                int cur_mp_type;

                for (cur_mp_type = 0; cur_mp_type < sizeof(possible_mp_types_qax) / sizeof(uint32); ++cur_mp_type) {
                    oam1_key.mp_type_jr = possible_mp_types[cur_mp_type];
                    oam1_key.mp_type = possible_mp_types_plus[cur_mp_type];
                    
                    for (mep_prof = 0; mep_prof < SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM; ++mep_prof) {
                        oam1_key.mp_profile = mep_prof;
                        res = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, oama_buffer);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                    }
                }
            }
            
            for (mep_prof = 0; mep_prof < SOC_PPC_OAM_ACC_PROFILES_NUM; ++mep_prof) {
                oam2_key.mp_profile = mep_prof;
                oam2_key.ingress = 1;
                oam2_key.inject = 1;
                oam2_key.is_bfd = 0;
                oam2_key.my_cfm_mac = oam1_key.my_cfm_mac;
                oam2_key.opcode = oam1_key.opcode;
                res = arad_pp_oam_classifier_oam2_entry_set_on_buffer(unit, &oam2_key, &oam_payload, oamb_buffer);
                SOCDNX_SAND_IF_ERR_EXIT(res);
            }
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

void arad_pp_oam_classifier_internal_opcode_init(SOC_SAND_IN  int                           unit,
                                                 SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA  *profile_data)
{
    uint8 internal_opcode;
	
    for(internal_opcode=0; internal_opcode < SOC_PPC_OAM_OPCODE_MAP_COUNT; internal_opcode++)
    {
        switch(internal_opcode_init[internal_opcode].additional_info){
        case ARAD_PP_OAM_OPCODE_CCM:
                internal_opcode_init[internal_opcode].sub_type = (profile_data->is_piggybacked) ? _ARAD_PP_OAM_SUBTYPE_LM : _ARAD_PP_OAM_SUBTYPE_CCM;
            break;
        case ARAD_PP_OAM_OPCODE_LM:
            internal_opcode_init[internal_opcode].sub_type = _ARAD_PP_OAM_SUBTYPE_LM;
            break;
        case ARAD_PP_OAM_OPCODE_SLM:
           if (SOC_IS_JERICHO_PLUS(unit)) {
              internal_opcode_init[internal_opcode].sub_type = _ARAD_PP_OAM_SUBTYPE_SLM;
           } else{
              internal_opcode_init[internal_opcode].sub_type = _ARAD_PP_OAM_SUBTYPE_DEFAULT_OAM_MESSAGE;
           }
           break;
        case ARAD_PP_OAM_OPCODE_DM:
            internal_opcode_init[internal_opcode].sub_type = (profile_data->is_1588 ? _ARAD_PP_OAM_SUBTYPE_DM_1588 : _ARAD_PP_OAM_SUBTYPE_DM_NTP);
            break;
        case ARAD_PP_OAM_OPCODE_LBR:
            internal_opcode_init[internal_opcode].sub_type = (profile_data->flags & SOC_PPC_OAM_LIF_PROFILE_FLAG_SAT_LOOPBACK) ? _ARAD_PP_OAM_SUBTYPE_ETH_LOOPBACK : _ARAD_PP_OAM_SUBTYPE_CCM;
            break;
        default:
            
            internal_opcode_init[internal_opcode].sub_type = (profile_data->flags & SOC_PPC_OAM_LIF_PROFILE_FLAG_SAT_LOOPBACK) ? _ARAD_PP_OAM_SUBTYPE_ETH_LOOPBACK :_ARAD_PP_OAM_SUBTYPE_DEFAULT_OAM_MESSAGE;
            break;
        }
    }
}


uint32 arad_pp_oam_classifier_oam1_entries_insert_bfd_acccelerated_to_profile( SOC_SAND_IN int                           unit,
                                                                               SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                               SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data)
{
    uint32 res;
    _oam_oam_a_b_table_buffer_t oamb_buffer ;

    SOCDNX_INIT_FUNC_DEFS;
    sal_memset(&oamb_buffer,0, sizeof(_oam_oam_a_b_table_buffer_t) );

    res = arad_pp_oam_classifier_insert_according_to_profile_verify(unit, classifier_mep_entry, profile_data);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    arad_pp_oam_classifier_internal_opcode_init(unit, profile_data);

    res = arad_pp_oam_classifier_oam2_allocate_sw_buffer(unit, &oamb_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_pp_oam_classifier_oam2_entries_insert_accelerated_according_to_profile( unit,
                                                                                       classifier_mep_entry,
                                                                                       profile_data,
                                                                                       &oamb_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);


    res = arad_pp_oam_classifier_oam2_set_hw_unsafe(unit, &oamb_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oamb_buffer);
    SOCDNX_FUNC_RETURN;
}

uint32 arad_pp_oam_classifier_entries_insert_passive_non_accelerated(SOC_SAND_IN int unit,
                                                                     SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                     SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data)
{
    uint32 res;
    _oam_oam_a_b_table_buffer_t oam_buffer ;

    SOCDNX_INIT_FUNC_DEFS;
    sal_memset(&oam_buffer,0, sizeof(_oam_oam_a_b_table_buffer_t) );

    res = arad_pp_oam_classifier_insert_according_to_profile_verify(unit, classifier_mep_entry, profile_data);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    arad_pp_oam_classifier_internal_opcode_init(unit, profile_data);

    res = arad_pp_oam_classifier_oam1_allocate_sw_buffer(unit, &oam_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_pp_oam_classifier_oam1_entries_insert_oam_non_acc_according_to_profile( unit,
                                                                                       classifier_mep_entry,
                                                                                       profile_data,
                                                                                       1, 
                                                                                       &oam_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_pp_oam_classifier_oam1_set_hw_unsafe(unit, &oam_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oam_buffer);
    SOCDNX_FUNC_RETURN;
}

uint32 arad_pp_oam_classifier_entries_insert_active_non_accelerated(SOC_SAND_IN int unit,
                                                                    SOC_SAND_IN int is_server,
                                                                    SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                    SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
                                                                    SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data_acc)
{
    uint32 res;
    _oam_oam_a_b_table_buffer_t oama_buffer ;
    _oam_oam_a_b_table_buffer_t oamb_buffer ;

    SOCDNX_INIT_FUNC_DEFS;
    sal_memset(&oama_buffer,0, sizeof(_oam_oam_a_b_table_buffer_t) );
    sal_memset(&oamb_buffer,0, sizeof(_oam_oam_a_b_table_buffer_t) );

    res = arad_pp_oam_classifier_insert_according_to_profile_verify(unit, classifier_mep_entry, profile_data);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    arad_pp_oam_classifier_internal_opcode_init(unit, profile_data);

    res = arad_pp_oam_classifier_oam1_allocate_sw_buffer(unit, &oama_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    res = arad_pp_oam_classifier_oam2_allocate_sw_buffer(unit, &oamb_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);


    res = arad_pp_oam_classifier_oam1_entries_insert_oam_non_acc_according_to_profile(unit, 
                                                                                       classifier_mep_entry,
                                                                                       profile_data,
                                                                                       0, 
                                                                                       &oama_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    if ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT) &&
        (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP)) {
        
        res = arad_pp_oam_classifier_oam1_2_entries_insert_injected_according_to_profile(unit, 
                classifier_mep_entry,
                ((is_server) ? profile_data_acc : profile_data),
                &oama_buffer,
                &oamb_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }

    if(SOC_IS_QAX(unit) && (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT ) &&
                          !(classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) &&
                           (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM )){

        res = soc_qax_pp_oam_classifier_oam1_2_entries_insert_egress_inject(unit,
                                                                            ((is_server) ? profile_data_acc : profile_data),
                                                                            &oama_buffer,
                                                                            &oamb_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(res);

    }
    if((SOC_IS_JERICHO(unit) && (!SOC_IS_QAX(unit))) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_ccm_pkt_counting", 0) == 1)
         && (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT )
          &&!(classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP)
          && (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM )){
        res = soc_jer_pp_oam_classifier_oam1_2_entries_insert_egress_inject(unit,
                                                                            ((is_server) ? profile_data_acc : profile_data),
                                                                            &oama_buffer,
                                                                            &oamb_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }

    res = arad_pp_oam_classifier_oam2_set_hw_unsafe(unit, &oamb_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);


    res = arad_pp_oam_classifier_oam1_set_hw_unsafe(unit, &oama_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oama_buffer);
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oamb_buffer);

    SOCDNX_FUNC_RETURN;
}


uint32 arad_pp_oam_classifier_rfc6374_entries_insert_active_non_accelerated(SOC_SAND_IN int                                unit,
                                                                            SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                            SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data)
{
    uint32 res;
    _oam_oam_a_b_table_buffer_t oama_buffer;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(&oama_buffer, 0, sizeof(_oam_oam_a_b_table_buffer_t));

    res = arad_pp_oam_classifier_insert_according_to_profile_verify(unit, classifier_mep_entry, profile_data);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    arad_pp_oam_classifier_internal_opcode_init(unit, profile_data);

    
    res = arad_pp_oam_classifier_oam1_allocate_sw_buffer(unit, &oama_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    res = arad_pp_oam_classifier_oam1_rfc6374_entries_insert_oam_non_acc_according_to_profile(unit,
                                                                                              classifier_mep_entry,
                                                                                              profile_data,
                                                                                              &oama_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    res = arad_pp_oam_classifier_oam1_set_hw_unsafe(unit, &oama_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit, &oama_buffer);

    SOCDNX_FUNC_RETURN;
}

uint32 arad_pp_oam_classifier_entries_insert_accelerated(SOC_SAND_IN int unit,
                                                         SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                         SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data)
{
    uint32 res;
    _oam_oam_a_b_table_buffer_t oama_buffer ;
    _oam_oam_a_b_table_buffer_t oamb_buffer ;

    SOCDNX_INIT_FUNC_DEFS;
    sal_memset(&oama_buffer,0, sizeof(_oam_oam_a_b_table_buffer_t) );
    sal_memset(&oamb_buffer,0, sizeof(_oam_oam_a_b_table_buffer_t) );
    
	res = arad_pp_oam_classifier_insert_according_to_profile_verify(unit, classifier_mep_entry, profile_data);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    arad_pp_oam_classifier_internal_opcode_init(unit, profile_data);

    res = arad_pp_oam_classifier_oam1_allocate_sw_buffer(unit, &oama_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_pp_oam_classifier_oam2_allocate_sw_buffer(unit, &oamb_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_pp_oam_classifier_oam2_entries_insert_accelerated_according_to_profile( unit,
                                                                                       classifier_mep_entry,
                                                                                       profile_data,
                                                                                       &oamb_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_pp_oam_classifier_oam1_2_entries_insert_injected_according_to_profile( unit,
                                                                                      classifier_mep_entry,
                                                                                      profile_data,
                                                                                      &oama_buffer,
                                                                                      &oamb_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    if(SOC_IS_QAX(unit) && (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT ) &&
                          !(classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) &&
                           (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM )){

        res = soc_qax_pp_oam_classifier_oam1_2_entries_insert_egress_inject(unit,
                                                                            profile_data,
                                                                            &oama_buffer,
                                                                            &oamb_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(res);

    }

    if((SOC_IS_JERICHO(unit) && (!SOC_IS_QAX(unit))) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_ccm_pkt_counting", 0) == 1)
         && (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT )
          &&!(classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP)
          && (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM )){
        res = soc_jer_pp_oam_classifier_oam1_2_entries_insert_egress_inject(unit,
                                                                            profile_data,
                                                                            &oama_buffer,
                                                                            &oamb_buffer);
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }

    res = arad_pp_oam_classifier_oam1_set_hw_unsafe(unit, &oama_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);


    res = arad_pp_oam_classifier_oam2_set_hw_unsafe(unit, &oamb_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oamb_buffer);
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oama_buffer);

    SOCDNX_FUNC_RETURN;
}


uint32 arad_pp_oam_classifier_rfc6374_entries_insert_accelerated(SOC_SAND_IN int                                unit,
                                                                 SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                 SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data)
{
    uint32 res;
    _oam_oam_a_b_table_buffer_t oamb_buffer;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(&oamb_buffer, 0, sizeof(_oam_oam_a_b_table_buffer_t));

    res = arad_pp_oam_classifier_insert_according_to_profile_verify(unit, classifier_mep_entry, profile_data);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    arad_pp_oam_classifier_internal_opcode_init(unit, profile_data);

    
    res = arad_pp_oam_classifier_oam2_allocate_sw_buffer(unit, &oamb_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    res = arad_pp_oam_classifier_oam2_rfc6374_entries_insert_accelerated_according_to_profile(unit,
                                                                                              classifier_mep_entry,
                                                                                              profile_data,
                                                                                              &oamb_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    res = arad_pp_oam_classifier_oam2_set_hw_unsafe(unit, &oamb_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit, &oamb_buffer);

    SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_oam_classifier_oem_mep_profile_replace_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint32                           update_mp_type
  )
{
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE_VERIFY);

    res = SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_verify(unit, classifier_mep_entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile_passive, SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
    }
    else { 
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPC_OAM_NON_ACC_PROFILES_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 65, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem_mep_profile_replace_verify()", 0, 0);
}


uint32
  arad_pp_oam_classifier_oem_mep_profile_replace_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry,
    SOC_SAND_IN  uint32                           update_mp_type
  )
{
  uint32 res;
  uint8  found;
  uint8  passive_active_enable, passive_active_enable_passive;
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY       oem1_key;
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD   oem1_payload;
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD   oem1_prev_payload, oem1_payload_active, oem1_payload_passive;
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY       oem2_key;
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD   oem2_payload;
  uint8 is_upmep, is_mip;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(classifier_mep_entry);

  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(&oem1_key);
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload);
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_prev_payload);
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload_active);
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload_passive);
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY_clear(&oem2_key);
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_clear(&oem2_payload);

  if (SOC_PPC_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type)) {
    LOG_ERROR(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          " Only Ethernet and Y1731 type supported.\n\r")));
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);
  }

  is_upmep = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);
  is_mip = _BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(classifier_mep_entry);

  
  oem1_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
  oem1_key.your_discr = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) * SOC_IS_JERICHO(unit); 

  if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)){
      res = arad_pp_oam_mep_passive_active_enable_get_unsafe(unit, classifier_mep_entry->non_acc_profile, &passive_active_enable);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      res = arad_pp_oam_mep_passive_active_enable_get_unsafe(unit, classifier_mep_entry->non_acc_profile_passive, &passive_active_enable_passive);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      
      if (classifier_mep_entry->passive_side_lif != _BCM_OAM_INVALID_LIF) {
         
         if (passive_active_enable != passive_active_enable_passive) {
                LOG_ERROR(BSL_LS_SOC_OAM,
                          (BSL_META_U(unit,
                                      " Error: passive_active_enable should be the same for passive and active sides of same endpoint.\n\r")));
                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
          }
      }
       
      if (passive_active_enable) {
          
          
          if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
               ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
              
              oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
              res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_prev_payload, &found);
              SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
              if (!found) {
              LOG_ERROR(BSL_LS_SOC_OAM,
                          (BSL_META_U(unit,
                                      " OEM1 entry lif %d not found.\n\r"), oem1_key.oam_lif));
                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 20, exit);
              }
              oem1_payload_active.mp_profile =  classifier_mep_entry->non_acc_profile;
              oem1_payload_active.mep_bitmap = oem1_prev_payload.mep_bitmap;
              oem1_payload_active.mip_bitmap = oem1_prev_payload.mip_bitmap;
              oem1_payload_active.counter_ndx = oem1_prev_payload.counter_ndx;
          }
          
          if (classifier_mep_entry->passive_side_lif != _BCM_OAM_INVALID_LIF) {
              
              oem1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
              oem1_key.oam_lif = classifier_mep_entry->passive_side_lif;
              res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_prev_payload, &found);
              SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
              
              if (!found && !update_mp_type) {
                LOG_ERROR(BSL_LS_SOC_OAM,
                          (BSL_META_U(unit,
                                      " OEM1 entry lif %d not found.\n\r"), oem1_key.oam_lif));
                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 20, exit);
              }
              oem1_payload_passive.mp_profile =  classifier_mep_entry->non_acc_profile_passive;
              oem1_payload_passive.mep_bitmap = found ? oem1_prev_payload.mep_bitmap : 0;
              if (update_mp_type) {
                  
                  res = set_mip_bitmep_from_mep_bitmep(unit, &(oem1_payload_passive.mip_bitmap), oem1_payload_active.mep_bitmap);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
              } else {
                  oem1_payload_passive.mip_bitmap = oem1_prev_payload.mip_bitmap;
              }
              oem1_payload_passive.counter_ndx = found ? oem1_prev_payload.counter_ndx : oem1_payload_active.counter_ndx;
          }
          
          if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
               ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
              
              oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
              res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload_active);
              SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
          }
          
          if (classifier_mep_entry->passive_side_lif != _BCM_OAM_INVALID_LIF) {
              oem1_key.oam_lif = classifier_mep_entry->passive_side_lif;
              oem1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
              res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload_passive);
              SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
          }
      }
      else {
          
          
          if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
               ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
          
          oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
          res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_prev_payload, &found);
          SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
          if (!found) {
            LOG_ERROR(BSL_LS_SOC_OAM,
                      (BSL_META_U(unit,
                                  " OEM1 entry lif %d not found.\n\r"), oem1_key.oam_lif));
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 20, exit);
          }
          oem1_payload_active.mp_profile = classifier_mep_entry->non_acc_profile;
          oem1_payload_active.mip_bitmap = update_mp_type ? 0 : oem1_prev_payload.mip_bitmap;
          oem1_payload_active.mep_bitmap = oem1_prev_payload.mep_bitmap;
          oem1_payload_active.counter_ndx = oem1_prev_payload.counter_ndx;
          }
          
          if (classifier_mep_entry->passive_side_lif != _BCM_OAM_INVALID_LIF) {
              
              oem1_key.oam_lif = classifier_mep_entry->passive_side_lif;
              oem1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
              res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_prev_payload, &found);
              SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
              if (!found) {
                LOG_ERROR(BSL_LS_SOC_OAM,
                          (BSL_META_U(unit,
                                      " OEM1 entry lif %d not found.\n\r"), oem1_key.oam_lif));
                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 20, exit);
              }
              oem1_payload_passive.mp_profile = classifier_mep_entry->non_acc_profile_passive;
              oem1_payload_passive.mep_bitmap = oem1_prev_payload.mep_bitmap;
              oem1_payload_passive.mip_bitmap = update_mp_type? 0 : oem1_prev_payload.mip_bitmap;
              oem1_payload_passive.counter_ndx = oem1_prev_payload.counter_ndx;
          }
          
          if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
               ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
          
          oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
          res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload_active);
          SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
          }
          
          if (classifier_mep_entry->passive_side_lif != _BCM_OAM_INVALID_LIF) {
              oem1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
              oem1_key.oam_lif = classifier_mep_entry->passive_side_lif;
              res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload_passive);
              SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
          }
      }
  }
  else {

      
      if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
           ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
      
      oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);

      res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

      if (!found) {
          LOG_ERROR(BSL_LS_SOC_OAM,
                    (BSL_META_U(unit,
                                " OEM1 entry lif %d not found.\n\r"), oem1_key.oam_lif));
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 20, exit);
      }

      if (oem1_payload.mp_profile != classifier_mep_entry->non_acc_profile) {
          oem1_payload.mp_profile = classifier_mep_entry->non_acc_profile;
          res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      }
     }
      
      if (is_mip) {
          oem1_key.ingress = 0;
          oem1_key.oam_lif = classifier_mep_entry->passive_side_lif;

          res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, &found);
          SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

          if (!found) {
              LOG_ERROR(BSL_LS_SOC_OAM,
                        (BSL_META_U(unit,
                                    " OEM1 entry lif %d not found.\n\r"), oem1_key.oam_lif));
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 20, exit);
          }

          if (oem1_payload.mp_profile != classifier_mep_entry->non_acc_profile_passive) {
              oem1_payload.mp_profile = classifier_mep_entry->non_acc_profile_passive;
              res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
              SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          }
      }
     if (SOC_IS_JERICHO_PLUS(unit)) {
        
        
         if (classifier_mep_entry->passive_side_lif != _BCM_OAM_INVALID_LIF) {
            if (classifier_mep_entry->non_acc_profile_passive == ARAD_PP_SLM_PASSIVE_PROFILE) {
               oem1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep); 
               oem1_key.oam_lif = classifier_mep_entry->passive_side_lif;
               res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, &found);
               if (!found) {
                  LOG_ERROR(BSL_LS_SOC_OAM, (BSL_META_U(unit, " OEM1 entry lif %d not found.\n\r"), oem1_key.oam_lif));
                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 20, exit);
               }
               SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
               oem1_payload.mp_profile = classifier_mep_entry->non_acc_profile_passive;
               res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
               SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
            }
        }
     }
  }

  
  if (((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && 
        (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT) != 0) && (!SOC_PPC_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type))) ||
        ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0)) {

        
        if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
             ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
              (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
              (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
              (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
              (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
              (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
            oem2_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
            oem2_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
            oem2_key.mdl = classifier_mep_entry->md_level;
            oem2_key.your_disc = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type); 

            res = arad_pp_oam_classifier_oem2_entry_get_unsafe(unit, &oem2_key, &oem2_payload, &found);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
            if (!found) {
                LOG_ERROR(BSL_LS_SOC_OAM,
                          (BSL_META_U(unit,
                                      " OEM entry not found.\n\r")));
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 60, exit);
            }

            if (oem2_payload.mp_profile != classifier_mep_entry->acc_profile) {
                oem2_payload.mp_profile = classifier_mep_entry->acc_profile;
                res = arad_pp_oam_classifier_oem2_entry_set_unsafe(unit, &oem2_key, &oem2_payload);
                SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
            }
        }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem_mep_profile_replace_unsafe()", 0, 0);
}


soc_error_t
soc_arad_pp_oam_classifier_oem_mep_mip_conflict_check(SOC_SAND_IN  int       unit,
                                                      SOC_SAND_IN  uint32    oam_lif,
                                                      SOC_SAND_IN  uint8     is_mip)
{
    uint32                                      res;
    uint8                                       ingress;
    uint32                                      mep_bitmap;
    uint32                                      mip_bitmap;
    uint8                                       found;
    uint8                                       entry_found;
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY       oem1_key;
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD   oem1_payload;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
        
        SOC_EXIT;
    }

    if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
        
        SOC_EXIT;
    }

    mep_bitmap  = 0;
    mip_bitmap  = 1;
    entry_found = FALSE;

    
    for (ingress = 0; ingress <= 1; ingress++) {
        SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(&oem1_key);
        SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload);

        oem1_key.oam_lif = oam_lif;
        oem1_key.ingress = ingress;

        res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, &found);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if (found) {
            
            mep_bitmap += oem1_payload.mep_bitmap;
            mip_bitmap *= oem1_payload.mip_bitmap;
            entry_found = TRUE;
        }
    }

    if (!entry_found) {
        
        mip_bitmap = 0;
    }

    if (mep_bitmap && is_mip) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                             (_BSL_SOCDNX_MSG("Error: MEP and MIP cannot co-exist on same LIF "
                                              " when oam_classifier_advanced_mode=1")));
    }

    if (mip_bitmap && !is_mip) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                             (_BSL_SOCDNX_MSG("Error: MEP and MIP cannot co-exist on same LIF "
                                              " when oam_classifier_advanced_mode=1")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_pp_oam_classifier_oem_mep_add_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                           mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  )
{
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD_VERIFY);

    if ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0) {
        SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
    }
    res = SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_verify(unit, classifier_mep_entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile_passive, SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
    }
    else { 
        SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPC_OAM_NON_ACC_PROFILES_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 65, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem_mep_add_verify()", mep_index, 0);
}

uint32
  arad_pp_oam_classifier_oem_mep_add_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                           mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry,
    SOC_SAND_IN  uint8                            update, 
    SOC_SAND_IN  uint8                            is_arm
  )
{
  uint32 res;
  uint8  new_mip_bitmap = 0;
  uint8  new_mep_bitmap = 0;
  uint8  found = 0;
  uint32 highest_mep;
  uint8 passive_active_enable;
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY       oem1_key;
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD   oem1_payload;
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD   oem1_prev_payload;
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY       oem2_key;
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD   oem2_payload;
  uint8 is_upmep, is_mep;
  const SOC_SAND_PP_MAC_ADDRESS    *dst_mac_address_ptr = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(classifier_mep_entry);

  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(&oem1_key);
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload);
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_prev_payload);
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY_clear(&oem2_key);
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_clear(&oem2_payload);

  is_upmep = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);
  is_mep = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)!= 0);

  if ((classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) && update != 1) {
      dst_mac_address_ptr = &(classifier_mep_entry->dst_mac_address);
      
      if (SOC_SAND_CHECK_MAC_IS_UNICAST(dst_mac_address_ptr)) {
          res = arad_pp_oam_my_cfm_mac_set_unsafe(unit, classifier_mep_entry->port_core, dst_mac_address_ptr, classifier_mep_entry->port);
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      }
  }

  
  if (((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && is_mep && (!SOC_PPC_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type))) ||
        ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0) ||
       SOC_PPD_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(mep_index)) && update != 1) {
      
      oem2_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
      if (_ARAD_PP_OAM_IS_BFD_O_MPLS_SPECIAL_TTL(classifier_mep_entry->mep_type)) {
          oem2_key.oam_lif = classifier_mep_entry->your_discriminator;
      }
      else {
          oem2_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
      }
      oem2_key.mdl = classifier_mep_entry->md_level;
      oem2_key.your_disc = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type);
      if((classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS && SOC_IS_JERICHO(unit)) && (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_TP_CC_CV)){
          oem2_key.oam_lif = classifier_mep_entry->lif;
          oem2_key.your_disc = 0;
      }
      if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) == 0)) {
          
          oem2_payload.oam_id = classifier_mep_entry->lif & ARAD_PP_OAM_CLASSIFIER_OAM_ID_MASK;
      }
      else {
          if (is_arm == 0) {
              oem2_payload.oam_id = _ARAD_PP_OAM_INDEX_TO_HW_LEGAL_INDEX(mep_index & ARAD_PP_OAM_CLASSIFIER_OAM_ID_MASK);
          } else {
              oem2_payload.oam_id = mep_index & ARAD_PP_OAM_CLASSIFIER_OAM_ID_MASK;
          }
      }
      oem2_payload.mp_profile = classifier_mep_entry->acc_profile;
      
      if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
           ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
          res = arad_pp_oam_classifier_oem2_entry_set_unsafe(unit, &oem2_key, &oem2_payload);
          SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
          if (_ARAD_PP_OAM_IS_BFD_O_MPLS_SPECIAL_TTL(classifier_mep_entry->mep_type)) {
              
              oem2_key.oam_lif = classifier_mep_entry->lif;
              oem2_key.your_disc = 0;
              res = arad_pp_oam_classifier_oem2_entry_set_unsafe(unit, &oem2_key, &oem2_payload);
              SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
          }
      }
  }

  
  if (!SOC_PPC_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type) ||
      ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) == 0)) {
      
      oem1_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
      oem1_key.your_discr = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) && SOC_IS_JERICHO(unit);
      
      oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);

      
      if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
           ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
          res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_prev_payload, &found);
          SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      }
      if (update == 1) {
          oem1_payload = oem1_prev_payload;
      }

      oem1_payload.mp_profile = classifier_mep_entry->non_acc_profile;

      
      if (update == 1 && (!found &&
          (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
             ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
              (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
              (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
              (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
              (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
              (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))))) {
          SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("MEP to update does not exist")));
      }
      if (SOC_PPC_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type) && update != 1 && found) { 
          LOG_ERROR(BSL_LS_SOC_OAM,
                    (BSL_META_U(unit,
                                " Error: MEP with given your discriminator exists.\n\r")));
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_EXISTS_ERR, 19, exit);
      }

      if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
          
          if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
               ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
              res = soc_jer_pp_oam_oem1_mep_add(unit,classifier_mep_entry,&oem1_key,&oem1_prev_payload,&oem1_payload,1,update == 1);
              SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res, 222, exit, res);
          }
      } else {
          if (!SOC_PPC_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type)) { 
              if (update != 1) {
                  if (found) {
                  
                      if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
                           ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
                            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
                            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
                            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
                            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
                            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
                          

                          
                          new_mep_bitmap = oem1_prev_payload.mep_bitmap;
                          new_mip_bitmap = oem1_prev_payload.mip_bitmap;
                          if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) ||
                                  (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) && ((oem1_prev_payload.mp_profile != ARAD_PP_OAM_PROFILE_PASSIVE) || (!is_mep)))) {

                              
                              if (oem1_prev_payload.mp_profile != classifier_mep_entry->non_acc_profile && update != 2) {
                                  LOG_ERROR(BSL_LS_SOC_OAM,
                                          (BSL_META_U(unit,
                                                      " Collision in mep parameters direction with existing endpoints on the lif (direction/TOD mode).\n\r")));
                                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_INCONSISTENT_LIF_DATA_ERR, 21, exit);
                              }
                              if ((is_mep) &&
                                      (classifier_mep_entry->counter != 0) &&
                                      (oem1_prev_payload.counter_ndx != 0) &&
                                      (oem1_prev_payload.counter_ndx != classifier_mep_entry->counter)) {
                                  LOG_ERROR(BSL_LS_SOC_OAM,
                                          (BSL_META_U(unit,
                                                      " Counter existing on lif:%d. New counter: %d\n\r"), oem1_prev_payload.counter_ndx, classifier_mep_entry->counter));
                                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_INCONSISTENT_LIF_DATA_ERR, 25, exit);
                              }
                          }
                          else if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                              if ((oem1_prev_payload.mep_bitmap & (~(1 << classifier_mep_entry->md_level))) != 0) { 
                                  if (is_upmep) {
                                      LOG_WARN(BSL_LS_SOC_OAM,
                                              (BSL_META_U(unit,
                                                          " Adding upmep endpoint - behavior might be incosistant because downmep exists on different level.\n\r")));
                                      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_EXISTS_ERR, 25, exit);
                                  } else {
                                      LOG_WARN(BSL_LS_SOC_OAM,
                                              (BSL_META_U(unit,
                                                          " Adding downmep endpoint - behavior might be incosistant because upmep exists on different level.\n\r")));
                                      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_EXISTS_ERR, 25, exit);
                                  }
                              }
                          }
                      }
                  }
                  if (is_mep) {
                      new_mep_bitmap |= (1 << classifier_mep_entry->md_level);
                  } else {
                      if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                          new_mip_bitmap |= (1 << classifier_mep_entry->md_level);
                      } else {
                          
                          if ((new_mip_bitmap & 7) == 0) {
                              new_mip_bitmap |= classifier_mep_entry->md_level;
                          }
                          
                          else if ((new_mip_bitmap >> 3) == 0) {
                              new_mip_bitmap |= (classifier_mep_entry->md_level << 3);
                          } else {
                                LOG_ERROR(BSL_LS_SOC_OAM,
                                          (BSL_META_U(unit,
                                                      " Two mips on same lif already exist\n\r")));
                                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 25, exit);
                          }
                      }
                  }

                  
                  highest_mep = 0;
                  while ((new_mep_bitmap >> (highest_mep+1)) != 0) {
                      highest_mep ++;
                  }
                  if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                      if (((new_mip_bitmap >> highest_mep) << highest_mep) != new_mip_bitmap) {
                          
                        LOG_ERROR(BSL_LS_SOC_OAM,
                                  (BSL_META_U(unit,
                                              " New endpoint in level:%d can not be inserted with existing endpoints.\n\r"), classifier_mep_entry->md_level));
                          SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_MDLEVEL_CHECK_FAIL_ERR, 30, exit);
                      }
                  }
                  oem1_payload.mep_bitmap = new_mep_bitmap;
                  oem1_payload.mip_bitmap = new_mip_bitmap;
              }

              if (is_mep) {
                  oem1_payload.counter_ndx = classifier_mep_entry->counter;
              } else if (oem1_prev_payload.counter_ndx) {
                 
                 oem1_payload.counter_ndx = oem1_prev_payload.counter_ndx;
              }
              LOG_DEBUG(BSL_LS_SOC_OAM,
                       (BSL_META_U(unit,
                                   " Inserting counter to OEM1: lif: %d counter: %d\n\r"), oem1_key.oam_lif, oem1_payload.counter_ndx));
          }
      }

      
      if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
           ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
          if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->pp.oam_hierarchical_lm && found &&
              classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
              
              res = soc_qax_pp_oam_classifier_oem1_entry_set_for_hlm(unit, &oem1_key, &oem1_payload, classifier_mep_entry,
                                                         update==1 ? _QAX_PP_OAM_HLM_EP_OPERATION_UPDATE : _QAX_PP_OAM_HLM_EP_OPERATION_ADD);
              SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
          } else {
              
              res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
              SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          }
      }

      
      if (!SOC_PPC_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type) &&
          (classifier_mep_entry->passive_side_lif != _BCM_OAM_INVALID_LIF)) {
          oem1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
          oem1_key.oam_lif = classifier_mep_entry->passive_side_lif;
          oem1_key.your_discr = 0;

          
          res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_prev_payload, &found);
          SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

          if (update == 1) {
              oem1_payload = oem1_prev_payload;
              oem1_payload.counter_ndx = 0;
              if (classifier_mep_entry->counter) {
                  
                  oem1_payload.counter_ndx = classifier_mep_entry->counter;
                  if (SOC_IS_QUX(unit)) {
                      
                      if (oem1_key.ingress) {
                          
                          oem1_payload.counter_ndx |= _QUX_PP_OAM_DIRECTION_BIT_ON_ADDITION;
                      }
                  }
              }

              if (SOC_IS_QAX(unit) && found && SOC_DPP_CONFIG(unit)->pp.oam_hierarchical_lm &&
                  classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
                  
                  res = soc_qax_pp_oam_classifier_oem1_entry_set_for_hlm(unit, &oem1_key, &oem1_payload, classifier_mep_entry,
                                                             update==1 ? _QAX_PP_OAM_HLM_EP_OPERATION_UPDATE : _QAX_PP_OAM_HLM_EP_OPERATION_ADD);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
              }
              else {
                  res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
              }
          } else {
              if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
                  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload);
                  res = soc_jer_pp_oam_oem1_mep_add(unit, classifier_mep_entry, &oem1_key, &oem1_prev_payload, &oem1_payload, 0 , update == 1);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res, 222, exit, res);

                  if (SOC_IS_QAX(unit) && found && SOC_DPP_CONFIG(unit)->pp.oam_hierarchical_lm &&
                      classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
                      
                      res = soc_qax_pp_oam_classifier_oem1_entry_set_for_hlm(unit, &oem1_key, &oem1_payload, classifier_mep_entry,
                                                                 update==1 ? _QAX_PP_OAM_HLM_EP_OPERATION_UPDATE : _QAX_PP_OAM_HLM_EP_OPERATION_ADD);
                      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                  }
                  else {
                      res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
                      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                  }
              } else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
                      
                  oem1_payload.mp_profile = classifier_mep_entry->non_acc_profile_passive;
                  res = arad_pp_oam_mep_passive_active_enable_get_unsafe(unit, classifier_mep_entry->non_acc_profile_passive, &passive_active_enable);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
                  if ((!(is_mep)) || (passive_active_enable) || SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
                      

                      if (found) {
                          new_mep_bitmap = oem1_prev_payload.mep_bitmap;
                          new_mip_bitmap = oem1_prev_payload.mip_bitmap;
                      }
                      else {
                          new_mep_bitmap = 0;
                          new_mip_bitmap = 0;
                      }

                      if (!SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) || !is_mep) {
                          
                          if ((new_mip_bitmap & 7) == 0) {
                              new_mip_bitmap |= classifier_mep_entry->md_level;
                          }
                          
                          else if ((new_mip_bitmap >> 3) == 0) {
                              new_mip_bitmap |= classifier_mep_entry->md_level << 3;
                          }
                          else {
                              if (is_mep) {
                                LOG_ERROR(BSL_LS_SOC_OAM,
                                          (BSL_META_U(unit,
                                                      " Two meps on same lif already exist\n\r")));
                                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 25, exit);
                              }
                              else {
                                LOG_ERROR(BSL_LS_SOC_OAM,
                                          (BSL_META_U(unit,
                                                      " Two mips on same lif already exist\n\r")));
                                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 25, exit);
                              }
                          }
                      }
                      else {
                          
                          new_mep_bitmap |= (1 << classifier_mep_entry->md_level);
                      }

                      oem1_payload.mep_bitmap = new_mep_bitmap;
                      oem1_payload.mip_bitmap = new_mip_bitmap;

                      res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
                      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                  }
              } else {
                  
                  if (found && is_mep && SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) &&
                      (oem1_prev_payload.mp_profile != ARAD_PP_OAM_PROFILE_PASSIVE) &&
                      (oem1_prev_payload.mep_bitmap & (1 << classifier_mep_entry->md_level)) != 0) {
                      LOG_WARN(BSL_LS_SOC_OAM,
                               (BSL_META_U(unit,
                                           "Arad mode skip passive entry creation already one "
                                           "active entry found on same level same lif.\n\r")));
                  } else {
                      oem1_payload.mp_profile = ARAD_PP_OAM_PROFILE_PASSIVE;
                      if (found && is_mep && (oem1_prev_payload.mp_profile != ARAD_PP_OAM_PROFILE_PASSIVE) &&
                          (oem1_prev_payload.mep_bitmap & (~(1 << classifier_mep_entry->md_level))) != 0) {
                          LOG_WARN(BSL_LS_SOC_OAM,
                               (BSL_META_U(unit,
                                           " Skipping passive entry insert because of existing meps with different direction."
                                           "It is expected that another mep with opposite direction will be added next.\n\r")));
                      } else {
                          
                          res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
                          SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                      }
                  }
              }
          }
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem_mep_add_unsafe()", 0, 0);
}



soc_error_t
  arad_pp_oam_classifier_default_profile_add(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint8                              update_action_only
  )
{
    int res;
    uint32 reg32;
    soc_reg_above_64_val_t reg_above_64;

    uint32 mep_bitmap;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(classifier_mep_entry);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);

    if (!update_action_only) {
        if (mep_index >= ARAD_PP_OAM_DEFAULT_EP_EGRESS_0) {
            

            
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 25, exit, READ_IHB_OAM_DEFAULT_COUNTERSr(unit, reg_above_64));
            soc_reg_above_64_field32_set(unit, IHB_OAM_DEFAULT_COUNTERSr, reg_above_64, EGRESS_DEFAULT_COUNTERf,
                                         classifier_mep_entry->counter);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 26, exit, WRITE_IHB_OAM_DEFAULT_COUNTERSr(unit, reg_above_64));

            
            mep_bitmap = (1 << classifier_mep_entry->md_level) | ((1 << classifier_mep_entry->md_level) - 1);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 55, exit, READ_IHB_OAM_DEFAULT_EGRESS_BITMAPSr(unit, &reg32));
            soc_reg_field_set(unit, IHB_OAM_DEFAULT_EGRESS_BITMAPSr, &reg32, EGRESS_DEFAULT_MEP_BITMAPf, mep_bitmap);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 56, exit, WRITE_IHB_OAM_DEFAULT_EGRESS_BITMAPSr(unit, reg32));

        } 
        else {
            

            
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 25, exit, READ_IHB_OAM_DEFAULT_COUNTERSr(unit, reg_above_64));
            soc_reg_above_64_field32_set(unit, IHB_OAM_DEFAULT_COUNTERSr, reg_above_64, COUNTER_FOR_PROFILE_0f + (int)mep_index,
                                         classifier_mep_entry->counter);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 26, exit, WRITE_IHB_OAM_DEFAULT_COUNTERSr(unit, reg_above_64));

            
            mep_bitmap = (1 << classifier_mep_entry->md_level) | ((1 << classifier_mep_entry->md_level) - 1);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_IHB_OAM_MEP_BITMAPr(unit, &reg32));
            soc_reg_field_set(unit, IHB_OAM_MEP_BITMAPr, &reg32, MEP_BITMAP_FOR_PROFILE_0f + (int)mep_index, mep_bitmap);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 51, exit, WRITE_IHB_OAM_MEP_BITMAPr(unit, reg32));
        }
    }

    res = arad_pp_oam_classifier_default_profile_action_set(unit,mep_index,classifier_mep_entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_default_profile_add()", mep_index, 0);
}


soc_error_t
  arad_pp_oam_classifier_default_profile_remove(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index
  ) {
    int res;
    uint32 reg32;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (mep_index >= ARAD_PP_OAM_DEFAULT_EP_EGRESS_0) {
        

        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 55, exit, READ_IHB_OAM_DEFAULT_EGRESS_BITMAPSr(unit, &reg32));
        soc_reg_field_set(unit, IHB_OAM_DEFAULT_EGRESS_BITMAPSr, &reg32, EGRESS_DEFAULT_MEP_BITMAPf, 0);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 56, exit, WRITE_IHB_OAM_DEFAULT_EGRESS_BITMAPSr(unit, reg32));

    }
    else {
        

        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_IHB_OAM_MEP_BITMAPr(unit, &reg32));
        soc_reg_field_set(unit, IHB_OAM_MEP_BITMAPr, &reg32, MEP_BITMAP_FOR_PROFILE_0f + (int)mep_index, 0);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 51, exit, WRITE_IHB_OAM_MEP_BITMAPr(unit, reg32));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_default_profile_remove()", 0, 0);
}


soc_error_t
  arad_pp_oam_classifier_default_profile_action_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  )
{
    int res;
    uint32 reg32;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(classifier_mep_entry);

    if (mep_index >= ARAD_PP_OAM_DEFAULT_EP_EGRESS_0) {
        

        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, READ_IHB_OAM_DEFAULT_EGRESS_BITMAPSr(unit, &reg32));
        soc_reg_field_set(unit, IHB_OAM_DEFAULT_EGRESS_BITMAPSr, &reg32, EGRESS_DEFAULT_MIP_BITMAPf,
                          ((uint32)classifier_mep_entry->non_acc_profile) >> 2);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 36, exit, WRITE_IHB_OAM_DEFAULT_EGRESS_BITMAPSr(unit, reg32));

        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 45, exit, READ_IHB_OAM_DEFAULT_ACC_MEP_PROFILESr(unit, &reg32));
        soc_reg_field_set(unit, IHB_OAM_DEFAULT_ACC_MEP_PROFILESr, &reg32, EGRESS_DEFAULT_ACC_MEP_PROFILEf,
                          ((uint32)classifier_mep_entry->non_acc_profile) & 0x3);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 46, exit, WRITE_IHB_OAM_DEFAULT_ACC_MEP_PROFILESr(unit, reg32));

    } 
    else {
        

        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_OAM_MIP_BITMAPr(unit, &reg32));
        soc_reg_field_set(unit, IHB_OAM_MIP_BITMAPr, &reg32, MIP_BITMAP_FOR_PROFILE_0f + (int)mep_index,
                          ((uint32)classifier_mep_entry->non_acc_profile) >> 2);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, WRITE_IHB_OAM_MIP_BITMAPr(unit, reg32));

        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IHB_OAM_DEFAULT_ACC_MEP_PROFILESr(unit, &reg32));
        soc_reg_field_set(unit, IHB_OAM_DEFAULT_ACC_MEP_PROFILESr, &reg32, ACC_MEP_PROFILE_FOR_PROFILE_0f + (int)mep_index,
                          ((uint32)classifier_mep_entry->non_acc_profile) & 0x3);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 41, exit, WRITE_IHB_OAM_DEFAULT_ACC_MEP_PROFILESr(unit, reg32));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_default_profile_action_set()", mep_index, 0);
}



uint32
  arad_pp_oam_classifier_mep_delete_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                           mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  )
{
  uint32 res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_MEP_DELETE_VERIFY);
  res = SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_verify(unit, classifier_mep_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res , 10, exit);
  if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
      SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile_passive, SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 60, exit);
  }
  else {
      SOC_SAND_ERR_IF_ABOVE_MAX(classifier_mep_entry->non_acc_profile, SOC_PPC_OAM_NON_ACC_PROFILES_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 65, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem_mep_delete_verify()", mep_index, 0);
}

uint32
  arad_pp_oam_classifier_mep_delete_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  )
{
  uint32 res;
  uint8  new_mip_bitmap = 0;
  uint8  new_mep_bitmap = 0;
  uint8  found=0;
  uint8  is_mep = 0, is_upmep = 0, passive_active_enable = 0;
  uint32 new_mp_type_vector=0;
  uint8  level, num_active_meps_mips;

  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY       oem1_key;
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD   oem1_payload;
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY       oem1_passive_key;
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD   oem1_passive_payload;
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD   oem1_prev_payload;
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY       oem2_key;
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD   oem2_payload;
  const SOC_SAND_PP_MAC_ADDRESS    *dst_mac_address_ptr = NULL;
  uint8 mdl_mp_type = 0, matching_mdl_mp_type = 0, active_active_combo = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_MEP_DELETE_UNSAFE);

  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(&oem1_key);
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload);
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(&oem1_passive_key);
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_passive_payload);
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_prev_payload);
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY_clear(&oem2_key);
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_clear(&oem2_payload);

  
  if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
       ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
    is_upmep = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);
    is_mep = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)!= 0);

    
    oem2_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
    if (_ARAD_PP_OAM_IS_BFD_O_MPLS_SPECIAL_TTL(classifier_mep_entry->mep_type)) {
        oem2_key.oam_lif = classifier_mep_entry->your_discriminator;
    }
    else {
        oem2_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
    }
    oem2_key.mdl = classifier_mep_entry->md_level;
    oem2_key.your_disc = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type);
    if((classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS && SOC_IS_JERICHO(unit)) && (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_TP_CC_CV)){
      oem2_key.oam_lif = classifier_mep_entry->lif;
      oem2_key.your_disc = 0;
    }
    res = arad_pp_oam_classifier_oem2_entry_get_unsafe(unit, &oem2_key, &oem2_payload, &found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    if (found) {
        if (SOC_PPC_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type)) {
            
            res = arad_pp_oam_classifier_oem2_entry_delete_unsafe(unit, &oem2_key);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            if (_ARAD_PP_OAM_IS_BFD_O_MPLS_SPECIAL_TTL(classifier_mep_entry->mep_type)) {
                oem2_key.oam_lif = classifier_mep_entry->lif;
                oem2_key.your_disc = 0;
                arad_pp_oam_classifier_oem2_entry_delete_unsafe(unit, &oem2_key);
            }
            SOC_SAND_EXIT_NO_ERROR;
        }
    }

    if(!SOC_PPC_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type)) {
        if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && is_mep) {
            
            oem2_key.ingress = oem2_key.ingress ? 0 : 1; 
            res = arad_pp_oam_classifier_oem2_entry_get_unsafe(unit, &oem2_key, &oem2_payload, &found);
            SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);
            if (found) {
                oem2_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
                oem2_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
                oem2_key.mdl = classifier_mep_entry->md_level;
                oem2_key.your_disc = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type);
                res = arad_pp_oam_classifier_oem2_entry_delete_unsafe(unit, &oem2_key);
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
                SOC_SAND_EXIT_NO_ERROR;
            }
        }
    }
  }

  
  oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
  oem1_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
  oem1_key.your_discr = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) * SOC_IS_JERICHO(unit);
  if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
       ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
    res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, &found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    if (!found) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 50, exit);
    }
  }

  if (!SOC_PPC_OAM_IS_MEP_TYPE_BFD(classifier_mep_entry->mep_type)) { 
      if (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) { 
            dst_mac_address_ptr = &(classifier_mep_entry->dst_mac_address);
          
          if (SOC_SAND_CHECK_MAC_IS_UNICAST(dst_mac_address_ptr)) {
              res = arad_pp_oam_my_cfm_mac_delete_unsafe(unit, classifier_mep_entry->port_core, dst_mac_address_ptr, classifier_mep_entry->port);
              SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
          }
      }

      
      if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
          if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
               ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
                (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
             if (JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(oem1_payload.mp_type_vector, classifier_mep_entry->md_level) == 0) {
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 90, exit);
             }
             if(classifier_mep_entry->passive_side_lif != _BCM_OAM_INVALID_LIF){
                 oem1_passive_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
                 oem1_passive_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->passive_side_lif;
                 oem1_passive_key.your_discr = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) && SOC_IS_JERICHO(unit);
                 res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_passive_key, &oem1_passive_payload, &found);
                 SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
             }

             mdl_mp_type = JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(
                     oem1_payload.mp_type_vector, classifier_mep_entry->md_level);
             matching_mdl_mp_type = JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(
                     oem1_passive_payload.mp_type_vector, classifier_mep_entry->md_level);
             if ((mdl_mp_type == matching_mdl_mp_type) && (mdl_mp_type == _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH)) {
                 active_active_combo = 1; 
             }
             
             res = soc_jer_pp_oam_oem1_mep_delete(unit,classifier_mep_entry,&oem1_key,&new_mp_type_vector, active_active_combo);
             SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
          }
      } else {
          new_mep_bitmap = oem1_payload.mep_bitmap;
          new_mip_bitmap = oem1_payload.mip_bitmap;
          
          if (is_mep) {
              new_mep_bitmap &= ~(1 << classifier_mep_entry->md_level);
          } else {
              if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                  new_mip_bitmap &= ~(1 << classifier_mep_entry->md_level);
              } else {
                  
                  if ((new_mip_bitmap & 7) == classifier_mep_entry->md_level) {
                      
                      new_mip_bitmap &= 0x38;
                  } else if ((new_mip_bitmap >> 3) == classifier_mep_entry->md_level) {
                      
                      new_mip_bitmap &= 0x7;
                  } else {
                      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 90, exit);
                  }
              }
          }
      }
      
      
      if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
           ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
            (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
          if ((new_mip_bitmap != 0) || (new_mep_bitmap != 0) || new_mp_type_vector) {
              
              
              oem1_prev_payload.counter_ndx = oem1_payload.counter_ndx;
              oem1_prev_payload.mp_profile = oem1_payload.mp_profile;
              oem1_prev_payload.mp_type_vector = oem1_payload.mp_type_vector;

              
              oem1_payload.mep_bitmap = new_mep_bitmap;
              oem1_payload.mip_bitmap = new_mip_bitmap;
              oem1_payload.mp_type_vector = new_mp_type_vector;

              if (active_active_combo) {
                  
                  num_active_meps_mips = 0;
                  for (level = 0; level < 8; level++) {
                      if ((JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(new_mp_type_vector, level) == _JER_PP_OAM_MDL_MP_TYPE_MIP) ||
                          (JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(new_mp_type_vector, level) == _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH)) {
                          num_active_meps_mips++;
                      }
                  }
                  if (num_active_meps_mips == 0) {
                      
                      oem1_payload.mp_profile = 0;
                  }
              }

              if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->pp.oam_hierarchical_lm &&
                  classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
                  
                  res = soc_qax_pp_oam_classifier_oem1_entry_set_for_hlm(unit, &oem1_key, &oem1_payload, classifier_mep_entry, _QAX_PP_OAM_HLM_EP_OPERATION_DELETE);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
              } else {
                  res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
              }
          } else {
              
              if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) && found && is_mep &&
                  (oem1_payload.mp_profile != ARAD_PP_OAM_PROFILE_PASSIVE) &&
                  (classifier_mep_entry->passive_side_lif != _BCM_OAM_INVALID_LIF)) {
                  
                  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY       oem1_key_1;
                  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(&oem1_key_1);
                  oem1_key_1.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
                  oem1_key_1.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->passive_side_lif;
                  oem1_key_1.your_discr = 0;
                  res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key_1, &oem1_prev_payload, &found);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
                  if (found && (oem1_prev_payload.mep_bitmap == oem1_payload.mep_bitmap) &&
                      (oem1_prev_payload.mp_profile != ARAD_PP_OAM_PROFILE_PASSIVE)) {
                      oem1_payload.mp_profile = ARAD_PP_OAM_PROFILE_PASSIVE;
                      
                      res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
                      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                      SOC_SAND_EXIT_NO_ERROR;
                  }
              }
              res = arad_pp_oam_classifier_oem1_entry_delete_unsafe(unit, &oem1_key);
              SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
          }
      }

      
      
      if (classifier_mep_entry->passive_side_lif != _BCM_OAM_INVALID_LIF) {
        oem1_key.ingress = !IS_INGRESS_WHEN_ACTIVE(is_upmep);
        oem1_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->passive_side_lif;
        oem1_key.your_discr = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) && SOC_IS_JERICHO(unit);
        res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, &found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

        if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
          if (!found) {
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 90, exit);
          }

          if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
              
              if ((!((SOC_PPC_OAM_IS_MEP_TYPE_Y1731(classifier_mep_entry->mep_type)||(classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS)||(classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE)) 
                  &&(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_downmep_pwe_classification", 1) == 0)))) {
                  if (JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(oem1_payload.mp_type_vector, classifier_mep_entry->md_level) == 0) {
                      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 90, exit);
                  }

                  res = soc_jer_pp_oam_oem1_mep_delete(unit,classifier_mep_entry,&oem1_key,&new_mp_type_vector, 0);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
              }
          }

          if (active_active_combo) {
              
          } else if ((new_mip_bitmap!=0) || (new_mep_bitmap!=0) || new_mp_type_vector) {
              
              
              oem1_prev_payload.counter_ndx = oem1_payload.counter_ndx;
              oem1_prev_payload.mp_profile = oem1_payload.mp_profile;
              oem1_prev_payload.mp_type_vector = oem1_payload.mp_type_vector;

              
              oem1_payload.mep_bitmap = new_mep_bitmap;
              oem1_payload.mip_bitmap = new_mip_bitmap;
              if ((!((SOC_PPC_OAM_IS_MEP_TYPE_Y1731(classifier_mep_entry->mep_type)||(classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS)||(classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE))
                  && ((SOC_IS_JERICHO(unit) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_downmep_pwe_classification", 1) == 0)) ||
 (SOC_IS_ARADPLUS_AND_BELOW(unit) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_downmep_pwe_classification", 0) == 0)))))) {
                  oem1_payload.mp_type_vector = new_mp_type_vector;
              } else {
                  oem1_payload.mp_type_vector = 0;
              }

              if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->pp.oam_hierarchical_lm &&
                  classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
                  
                  res = soc_qax_pp_oam_classifier_oem1_entry_set_for_hlm(unit, &oem1_key, &oem1_payload, classifier_mep_entry, _QAX_PP_OAM_HLM_EP_OPERATION_DELETE);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
              } else {
                  res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
              }
          } else {
              
              res = arad_pp_oam_classifier_oem1_entry_delete_unsafe(unit, &oem1_key);
              SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
          }
      }
      else {
          
          
          
          res = arad_pp_oam_mep_passive_active_enable_get_unsafe(unit, classifier_mep_entry->non_acc_profile, &passive_active_enable);
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
          if ((!is_mep) || (passive_active_enable) || SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
              new_mep_bitmap = oem1_payload.mep_bitmap;
              new_mip_bitmap = oem1_payload.mip_bitmap;

              if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && is_mep) {
                  new_mep_bitmap &= ~(1 << classifier_mep_entry->md_level);
              }
              else {
                  
                  if ((new_mip_bitmap & 7) == classifier_mep_entry->md_level) {
                      
                      new_mip_bitmap &= 0x38;
                  }
                  else if ((new_mip_bitmap >> 3) == classifier_mep_entry->md_level) {
                      
                      new_mip_bitmap &= 0x7;
                  }
                  else {
                      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR, 90, exit);
                  }
              }

              oem1_payload.mep_bitmap = new_mep_bitmap;
              oem1_payload.mip_bitmap = new_mip_bitmap;

              
              if ((new_mip_bitmap != 0) || (new_mep_bitmap != 0)) {
                  res = arad_pp_oam_classifier_oem1_entry_set_unsafe(unit, &oem1_key, &oem1_payload);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
              } else {
                  res = arad_pp_oam_classifier_oem1_entry_delete_unsafe(unit, &oem1_key);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
              }
          }
      }
    }
  } else {
      
      res = arad_pp_oam_classifier_oem1_entry_delete_unsafe(unit, &oem1_key);
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

  
  if (!((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
       ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
    is_upmep = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);
    is_mep = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)!= 0);

    
    oem2_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
    oem2_key.oam_lif = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type) ? classifier_mep_entry->your_discriminator : classifier_mep_entry->lif;
    oem2_key.mdl = classifier_mep_entry->md_level;
    oem2_key.your_disc = _ARAD_PP_OAM_BFD_IS_MEP_TYPE_USING_YOUR_DISC(classifier_mep_entry->mep_type);
    res = arad_pp_oam_classifier_oem2_entry_get_unsafe(unit, &oem2_key, &oem2_payload, &found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    if (found) {
        res = arad_pp_oam_classifier_oem2_entry_delete_unsafe(unit, &oem2_key);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_oem_mep_delete_unsafe()", mep_index, 0);
}



int _arad_pp_oam_bfd_mep_db_mem_by_entry_type_get(int unit, SOC_PPC_OAM_MEP_TYPE mep_type, soc_mem_t *mem){

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(mem);

    switch (mep_type) {
    case SOC_PPC_OAM_MEP_TYPE_ETH_OAM:
        *mem = OAMP_MEP_DBm;
        break;
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP:
    case SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS:
        *mem = OAMP_MEP_DB_Y_1731_ON_MPLSTPm;
        break;
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE:
    case SOC_PPC_OAM_MEP_TYPE_Y1711_PWE:
        *mem = OAMP_MEP_DB_Y_1731_ON_PWEm;
        break;
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL:
        *mem = OAMP_MEP_DB_Y_1731_ON_MPLSTPm;
        break;
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION:
        *mem = OAMP_MEP_DB_Y_1731_ON_PWEm;
        break;
    case SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE:
    case SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP:
    case SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION:
        *mem = OAMP_MEP_DB_RFC_6374_ON_MPLSTPm;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:
        *mem = OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
        *mem = OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE:
        *mem = OAMP_MEP_DB_BFD_ON_PWEm;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS:
        *mem = OAMP_MEP_DB_BFD_ON_MPLSm;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP:
        *mem = OAMP_MEP_DB_BFD_CC_ON_MPLSTPm;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Unsupported MEP-Type: %d ."), mep_type));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  arad_pp_oam_oamp_mep_db_entry_set_internal_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  uint32                   short_name,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry,
    SOC_SAND_IN  int                      is_new_ep
  )
{
  uint32  res;
  soc_mem_t mem;
  uint32  reg_val;
  soc_reg_above_64_val_t entry_above_64;
  soc_reg_above_64_val_t entry_above_64_get;
  uint32  inner_tpid, inner_vid_dei_pcp, outer_tpid, outer_vid_dei_pcp;
  uint32 eep_shift;
  uint32 user_header_size, user_header_0_size, user_header_1_size;
  uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
  uint32 temp_mep_pe_profile = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_INTERNAL_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 21, exit, _arad_pp_oam_bfd_mep_db_mem_by_entry_type_get(unit, mep_db_entry->mep_type, &mem));

  SOC_REG_ABOVE_64_CLEAR(entry_above_64);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, mep_index, &entry_above_64));

  res = arad_pmf_db_fes_user_header_sizes_get( unit,
                                                     &user_header_0_size,
                                                     &user_header_1_size,
                                                     &user_header_egress_pmf_offset_0_dummy,
                                                     &user_header_egress_pmf_offset_1_dummy);
  user_header_size = (user_header_0_size + user_header_1_size) / 8;

  reg_val = mep_db_entry->mep_type;


  if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) {
        
        reg_val = SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP;
  }
  
  if((SOC_IS_QAX(unit))&&(mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS)){
      reg_val = SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP;
  }
  else if((SOC_IS_QAX(unit))&&(mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE)){
      reg_val = SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE;
  }
    
  if ((mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION)) {
        
        reg_val = 7;
  }

  if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) {
        
        reg_val = SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE;
  }
  soc_mem_field_set(unit, mem, entry_above_64, MEP_TYPEf, &reg_val);
  if (mep_db_entry->mep_type != SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
      reg_val = mep_db_entry->priority;
      soc_mem_field_set(unit, mem, entry_above_64, PRIORITYf, &reg_val);
  }
  if (PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)   ) {
      if (mep_db_entry->local_port !=-1) {
          reg_val = mep_db_entry->local_port;
          soc_mem_field_set(unit, mem, entry_above_64, LOCAL_PORTf, &reg_val);
      }
  }
  else {
      reg_val = mep_db_entry->system_port;
      soc_mem_field_set(unit, mem, entry_above_64, DEST_SYS_PORT_AGRf, &reg_val);
  }

  eep_shift = SOC_IS_ARADPLUS_A0(unit) ? 1 : 0; 
  if ((mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL)) { 

      
      if(SOC_IS_JERICHO_PLUS(unit) && !is_new_ep) {
          soc_mem_field_get(unit,mem,entry_above_64 ,MEP_PE_PROFILEf, &temp_mep_pe_profile);
      }

      if (mep_db_entry->do_not_set_interval==0) {
          reg_val = mep_db_entry->ccm_interval;
          soc_mem_field_set(unit, mem, entry_above_64, CCM_INTERVALf, &reg_val);
      }
      reg_val = mep_db_entry->mdl;
      soc_mem_field_set(unit, mem, entry_above_64, MDLf, &reg_val);
      reg_val = mep_db_entry->icc_ndx;
      soc_mem_field_set(unit, mem, entry_above_64, ICC_INDEXf, &reg_val);

      
      if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) && mep_db_entry->is_maid_48) {
          reg_val = mep_db_entry->maid_48_index;
      } else {
          reg_val = short_name;
      }    
      
      if((SOC_IS_QAX(unit))&&(mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS)){
           arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_CONSTRUCT_1711_MPLS_TP_DIS_COUNT,&reg_val);
          soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
      }
      else if((SOC_IS_QAX(unit))&&(mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE)){
          arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_CONSTRUCT_1711_PWE_DIS_COUNT,&reg_val);
          soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
      }
      soc_mem_field_set(unit, mem, entry_above_64, MAIDf, &reg_val);
      reg_val = mep_db_entry->mep_id;
      soc_mem_field_set(unit, mem, entry_above_64, MEP_IDf, &reg_val);
      if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
          
          reg_val = mep_db_entry->rdi;
          soc_mem_field_set(unit, mem, entry_above_64, RDIf, &reg_val);
      } else {
          uint32 rdi=0;
          reg_val = mep_db_entry->counter_pointer;
          soc_mem_field_set(unit, mem, entry_above_64, COUNTER_POINTERf, &reg_val);
          reg_val = mep_db_entry->port_status_tlv_en;
          soc_mem_field_set(unit, mem, entry_above_64, PORT_STATUS_TLV_ENf, &reg_val);
          reg_val = mep_db_entry->port_status_tlv_val;
          soc_mem_field_set(unit, mem, entry_above_64, PORT_STATUS_TLV_VALf, &reg_val);
          reg_val = mep_db_entry->interface_status_tlv_control;
          soc_mem_field_set(unit, mem, entry_above_64, INTERFACE_STATUS_TLV_CODEf, &reg_val);

          soc_mem_field_get(unit, mem, entry_above_64, RDIf, &rdi);
          
          if (!mep_db_entry->is_ais_entry)
          {
              reg_val = mep_db_entry->rdi <<1 | (rdi &1); 
          } else {
              
              reg_val = mep_db_entry->rdi;
          }
          soc_mem_field_set(unit, mem, entry_above_64, RDIf, &reg_val);
      }

      if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) { 
          reg_val = mep_db_entry->is_upmep;
          soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, UP_1_DOWN_0f, &reg_val);
          if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              reg_val = 7;
              soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, UP_PTCH_OPAQUE_PT_ATTRf, &reg_val);
          } else {
              
              reg_val =0; 
              soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, UP_PTCH_OPAQUE_PT_ATTR_PROFILEf, &reg_val);
              
              reg_val = mep_db_entry->src_mac_lsb + ((mep_db_entry->src_mac_msb_profile &0x1)<<8);
              soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, SA_GEN_PROFILEf, &reg_val);

              
              if (SOC_IS_QUX(unit)) {
                  reg_val = 1;
                  soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, DA_MC_1_UC_0_SELf, &reg_val);
              }
          }

          switch (mep_db_entry->tags_num) {
          case 0: 
              inner_tpid = 0;
              inner_vid_dei_pcp = 0;
              outer_tpid = 0;
              outer_vid_dei_pcp = 0;
              break;
          case 1:
              
              inner_tpid = mep_db_entry->outer_tpid;
              inner_vid_dei_pcp = mep_db_entry->outer_vid_dei_pcp;
              outer_tpid = 0;
              outer_vid_dei_pcp = 0;
              break;
          case 2:
              inner_tpid = mep_db_entry->inner_tpid; 
              inner_vid_dei_pcp = mep_db_entry->inner_vid_dei_pcp; 
              outer_tpid = mep_db_entry->outer_tpid;
              outer_vid_dei_pcp = mep_db_entry->outer_vid_dei_pcp; 
              break;
          default:
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 82, exit);
          }
          soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, INNER_TPIDf, &inner_tpid);
          soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, INNER_VID_DEI_PCPf, &inner_vid_dei_pcp);
          soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, OUTER_TPIDf, &outer_tpid);
          soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, OUTER_VID_DEI_PCPf, &outer_vid_dei_pcp);
          reg_val = mep_db_entry->tags_num;
          soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, ENCAPSULATIONf, &reg_val);

          if (SOC_IS_ARADPLUS(unit)) {
            
            if (mep_db_entry->remote_recycle_port && mep_db_entry->is_11b_maid) {
                
                arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_ON_SERVER,&reg_val);
                soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
            } else if (mep_db_entry->remote_recycle_port) {
                
                arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER,&reg_val);
                soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
            } else if (mep_db_entry->is_11b_maid && user_header_size && !mep_db_entry->is_upmep) {
                
                arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_UDH, &reg_val);
                soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, MEP_PE_PROFILEf, &reg_val);
            } else if (mep_db_entry->is_11b_maid) {
                
                arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV, &reg_val);
                soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, MEP_PE_PROFILEf, &reg_val);
            } else {
                if(SOC_IS_JERICHO(unit))
                {
                    
                    if (mep_db_entry->is_11b_maid && mep_db_entry->is_upmep) {
                        
                        arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_MAID_11B_JER, &reg_val);
                        soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, MEP_PE_PROFILEf, &reg_val);
                    }
                    else if(user_header_size && mep_db_entry->is_upmep) {
                        arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_UP_MEP_UDH_DEFAULT_JER, &reg_val);
                        soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, MEP_PE_PROFILEf, &reg_val);
                    }
                } else {
                    
                    ARAD_PP_OAMP_PE_PROGRAMS prog_used = mep_db_entry->is_upmep? ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC :
                            ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX;
                    arad_pp_oamp_pe_program_profile_get(unit, prog_used, &reg_val);
                    soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, MEP_PE_PROFILEf, &reg_val);
                }
            }
          }
          
          if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) && mep_db_entry->is_maid_48) {
              
              if (user_header_size && !mep_db_entry->is_upmep && SOC_IS_JERICHO(unit)) {
                  arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_MAID_48_UDH, &reg_val);
                  soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, MEP_PE_PROFILEf, &reg_val);
              } else {
                  
                  arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_MAID_48, &reg_val);
                  soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, MEP_PE_PROFILEf, &reg_val);
              }
          }

          
          if(SOC_IS_QAX(unit) && (mep_db_entry->out_lif !=0) && (!mep_db_entry->is_maid_48) && (!mep_db_entry->is_upmep)) {
              
            if(is_new_ep){
                arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_EGR_INJ, &reg_val);
                
                reg_val |= (mep_db_entry->out_lif & 0x30000) >> 16;
                soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
            }else{
                
                soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &temp_mep_pe_profile);
            }
          }

          if(SOC_IS_JERICHO_PLUS(unit) && !is_new_ep ){
              
              if (temp_mep_pe_profile & ARAD_PP_OAMP_MEP_PE_PROFILE_SLM_OFFSET){
                soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &temp_mep_pe_profile);
              }
          }

          
          if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_itmh_mc", 0) && mep_db_entry->is_mc) {
              arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC, &reg_val);
              soc_OAMP_MEP_DBm_field_set(unit, entry_above_64, MEP_PE_PROFILEf, &reg_val);
          }
      }
      else if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP || 
               mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS || 
               mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL){

          if(mep_db_entry->egress_if == 0){
              reg_val = mep_db_entry->egress_if; 
          }
          else{
              reg_val = mep_db_entry->egress_if >> eep_shift; 
          }
          soc_mem_field_set(unit, mem, entry_above_64, EEPf, &reg_val);
          reg_val = mep_db_entry->label; 
          soc_mem_field_set(unit, mem, entry_above_64, MPLS_LABELf, &reg_val);
          reg_val = mep_db_entry->push_profile; 
          soc_mem_field_set(unit, mem, entry_above_64, MPLS_PUSH_PROFILEf, &reg_val);
      }
      else { 
          reg_val = mep_db_entry->egress_if >> eep_shift; 
          soc_mem_field_set(unit, mem, entry_above_64, EEPf, &reg_val);
          reg_val = mep_db_entry->label; 
          soc_mem_field_set(unit, mem, entry_above_64, PWE_LABELf, &reg_val);
          reg_val = mep_db_entry->push_profile; 
          soc_mem_field_set(unit, mem, entry_above_64, PWE_PUSH_PROFILEf, &reg_val);


          if (SOC_IS_QAX(unit) && (mep_db_entry->out_lif != 0) && (!mep_db_entry->is_maid_48)) {
             
              if(is_new_ep) {
                      arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_CCM_COUNT_QAX, &reg_val);
                      reg_val |= (mep_db_entry->out_lif & 0x30000) >> 16;
                      soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
              }else{
                  
                  soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &temp_mep_pe_profile);
              }
          }
      }
    
    if(SOC_IS_QAX(unit)) {
      if (mep_db_entry->is_maid_48) {
          if(is_new_ep) {
          
          soc_maid_48_qax_mep_pe_profile(unit, mep_db_entry->is_upmep,mep_db_entry->mep_type,mep_db_entry->out_lif, &reg_val);

          soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);

          reg_val = mep_db_entry->flex_data_ptr;
          soc_mem_field_set(unit, mem, entry_above_64, EXTRA_DATA_PTRf, &reg_val);
          }else{
              
              soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &temp_mep_pe_profile);
          }
      }
      
    }
  } 
  else if ((mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION)) {

      if((SOC_IS_JERICHO(unit)) && !(SOC_IS_QAX(unit)) && (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP))
      {
          arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_RFC_6374_PWE_BOS_FIX,&reg_val);
          soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
      }

    reg_val = mep_db_entry->egress_if; 
    soc_mem_field_set(unit, mem, entry_above_64, EEPf, &reg_val);
    reg_val = mep_db_entry->label; 
    soc_mem_field_set(unit, mem, entry_above_64, MPLS_LABELf, &reg_val);
    reg_val = mep_db_entry->push_profile; 
    soc_mem_field_set(unit, mem, entry_above_64, MPLS_PUSH_PROFILEf, &reg_val);
    reg_val = (mep_db_entry->bfd_pwe_router_alert)>0;
    soc_mem_field_set(unit, mem, entry_above_64, ROUTER_ALERTf, &reg_val);
    reg_val = mep_db_entry->counter_pointer;
    soc_mem_field_set(unit, mem, entry_above_64, COUNTER_POINTERf, &reg_val);
    reg_val = mep_db_entry->timestamp_format;
    soc_mem_field_set(unit, mem, entry_above_64, TIMESTAMP_FORMATf, &reg_val);
    reg_val = mep_db_entry->session_identifier_id;
    soc_mem_field_set(unit, mem, entry_above_64, SESSION_IDENTIFIERf, &reg_val);
    
    reg_val = 1;
    soc_mem_field_set(unit, mem, entry_above_64, ACHf, &reg_val);
    if(mep_db_entry->label == 13) { 
      reg_val = 0;
      soc_mem_field_set(unit, mem, entry_above_64, GALf, &reg_val);
    } else if ((mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP)) {
      reg_val = 1; 
      soc_mem_field_set(unit, mem, entry_above_64, GALf, &reg_val);
    }
  }
  else { 
      reg_val = mep_db_entry->ccm_interval; 
      soc_mem_field_set(unit, mem, entry_above_64, TX_RATEf, &reg_val);
      reg_val = mep_db_entry->egress_if >> eep_shift; 
      soc_mem_field_set(unit, mem, entry_above_64, EEPf, &reg_val);
      if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
          reg_val = mep_db_entry->pbit; 
          soc_mem_field_set(unit, mem, entry_above_64, PBITf, &reg_val);
          reg_val = mep_db_entry->fbit; 
          soc_mem_field_set(unit, mem, entry_above_64, FBITf, &reg_val);
          reg_val = mep_db_entry->local_detect_mult; 
      }
      else {
          reg_val = mep_db_entry->diag_profile; 
          soc_mem_field_set(unit, mem, entry_above_64, DIAG_PROFILEf, &reg_val);
          reg_val = mep_db_entry->flags_profile; 
          soc_mem_field_set(unit, mem, entry_above_64, FLAGS_PROFILEf, &reg_val);
          reg_val = mep_db_entry->sta; 
          soc_mem_field_set(unit, mem, entry_above_64, STAf, &reg_val);
      }
      reg_val = mep_db_entry->local_detect_mult; 
      soc_mem_field_set(unit, mem, entry_above_64, DETECT_MULTf, &reg_val);
      reg_val = mep_db_entry->min_rx_interval_ptr; 
      soc_mem_field_set(unit, mem, entry_above_64, MIN_RX_INTERVAL_PTRf, &reg_val);
      reg_val = mep_db_entry->min_tx_interval_ptr; 
      soc_mem_field_set(unit, mem, entry_above_64, MIN_TX_INTERVAL_PTRf, &reg_val);

      if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) {
          if (SOC_IS_ARAD_B0_AND_ABOVE((unit))) {
              reg_val = mep_db_entry->tunnel_is_mpls; 
              soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_set(unit, entry_above_64, TUNNEL_IS_MPLSf, &reg_val);
          }
          if (SOC_IS_JERICHO(unit)) {
              reg_val = mep_db_entry->dst_ip_add;
              soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_set(unit, entry_above_64, DST_IPf, &reg_val);
              reg_val = mep_db_entry->ip_subnet_len;
              soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_set(unit, entry_above_64, IP_SUBNET_LENf, &reg_val);
          }
          
          if (mep_db_entry->disc_type_update) {
              arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_MHOP_SHOP_BFD, &reg_val);
              if (reg_val == -1) {
                  SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("ARAD_PP_OAMP_PE_PROGS_MHOP_SHOP_BFD program not loaded.")));
              }
              soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
          }
      }
      else if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
          reg_val = mep_db_entry->tos_ttl_profile; 
          soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_set(unit, entry_above_64, IP_TTL_TOS_PROFILEf, &reg_val);
          reg_val = mep_db_entry->dst_ip_add; 
          soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_set(unit, entry_above_64, DST_IPf, &reg_val);
          if (SOC_IS_QAX(unit)) {
              
              reg_val = mep_db_entry->priority;
              soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_set(unit, entry_above_64, PRIORITYf, &reg_val);

          }
          if (mep_db_entry->s_bfd_flag == SOC_PPC_OAM_S_BFD_INITIATOR) {
              arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_SEAMLESS_BFD,&reg_val);
              soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
              if (SOC_IS_QAX(unit)) {
                reg_val = _QAX_PP_BFD_UDP_SRC_PORT_CHECK_DIS_OAMP_MEP_PROFILE;
                
                soc_mem_field_set(unit, mem, entry_above_64, MEP_PROFILEf, &reg_val); 
              }
          }
          
          if ((mep_db_entry->disc_type_update) && 
              (mep_db_entry->flex_tos_s_hop_m_bfd_flag == SOC_PPC_OAM_FLEX_TOS_S_HOP)) {
              arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_DPORT_AND_MYDISCR_UPDATE_BFD, &reg_val);
              if (reg_val == -1) {
                  SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("ARAD_PP_OAMP_PE_PROGS_MHOP_SHOP_BFD program not loaded.")));
              }
              soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
          } else if (mep_db_entry->flex_tos_s_hop_m_bfd_flag == SOC_PPC_OAM_FLEX_TOS_S_HOP) {
               
              arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_CHANGE_DPORT_TO_SHOP_BFD, &reg_val);
              if (reg_val == -1) {
                  SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("ARAD_PP_OAMP_PE_PROGS_MHOP_SHOP_BFD program not loaded.")));
              }
              soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
          } else if (mep_db_entry->disc_type_update) {
              
              arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_MHOP_SHOP_BFD, &reg_val);
              if (reg_val == -1) {
                  SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("ARAD_PP_OAMP_PE_PROGS_MHOP_SHOP_BFD program not loaded.")));
              }
              soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
          }
      }
      else if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE) {
          reg_val = mep_db_entry->label; 
          soc_OAMP_MEP_DB_BFD_ON_PWEm_field_set(unit, entry_above_64, PWE_LABELf, &reg_val);
          reg_val = mep_db_entry->push_profile; 
          soc_OAMP_MEP_DB_BFD_ON_PWEm_field_set(unit, entry_above_64, PWE_PUSH_PROFILEf, &reg_val);

          if (SOC_IS_ARADPLUS((unit))) {
              reg_val = (mep_db_entry->bfd_pwe_router_alert)>0;
              soc_OAMP_MEP_DB_BFD_ON_PWEm_field_set(unit, entry_above_64, ROUTER_ALERTf, &reg_val);
              reg_val = (mep_db_entry->bfd_pwe_ach>0);
              soc_OAMP_MEP_DB_BFD_ON_PWEm_field_set(unit, entry_above_64, ACHf, &reg_val);
              if (SOC_IS_JERICHO(unit) && reg_val) {
                  
                  switch (mep_db_entry->bfd_pwe_ach) {
                  case SOC_PPC_BFD_ACH_TYPE_PWE_CW:
                      reg_val = 0;
                      break;
                  case SOC_PPC_BFD_ACH_TYPE_GACH_CC:
                      reg_val = 1;
                      break;
                  default:
                      break;
                  }
                  soc_OAMP_MEP_DB_BFD_ON_PWEm_field_set(unit, entry_above_64, ACH_SELf, &reg_val);
              }
              if (mep_db_entry->bfd_pwe_gal) {
                  
                  if(mep_db_entry->label == 13) { 
                    reg_val = 0; 
                    soc_OAMP_MEP_DB_BFD_ON_PWEm_field_set(unit, entry_above_64, GALf, &reg_val);
                  } else { 
                    
                    reg_val = 1; 
                    soc_OAMP_MEP_DB_BFD_ON_PWEm_field_set(unit, entry_above_64, GALf, &reg_val);
                    arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX, &reg_val);

                    if (reg_val == -1) {
                        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX program not loaded.")));
                    }
                    soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
                  }
              }
          }
      }
      else if ((mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS) || (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP)){
          reg_val = mep_db_entry->label; 
          soc_mem_field_set(unit, mem, entry_above_64, MPLS_LABELf, &reg_val);
          reg_val = mep_db_entry->push_profile; 
          soc_mem_field_set(unit, mem, entry_above_64, MPLS_PUSH_PROFILEf, &reg_val);
          if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS) {
              reg_val = mep_db_entry->tos_ttl_profile; 
              soc_mem_field_set(unit, mem, entry_above_64, IP_TTL_TOS_PROFILEf, &reg_val);
              if (mep_db_entry->s_bfd_flag == SOC_PPC_OAM_S_BFD_INITIATOR) {
                  arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_SEAMLESS_BFD,&reg_val);
                  soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
                  if (SOC_IS_QAX(unit)) {
                      reg_val = _QAX_PP_BFD_UDP_SRC_PORT_CHECK_DIS_OAMP_MEP_PROFILE;
                      
                      soc_mem_field_set(unit, mem, entry_above_64, MEP_PROFILEf, &reg_val); 
                  }
              }
              if (SOC_IS_JERICHO(unit) && (mep_db_entry->is_vccv)) {
                  arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_MPLS_TP_VCCV,&reg_val);
                  soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
              }
              
          }
      }

      if (mep_db_entry->mep_type != SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP) {
          reg_val = mep_db_entry->remote_discr;
          soc_mem_field_set(unit, mem, entry_above_64, YOUR_DISCf, &reg_val);
          if (mep_db_entry->mep_type != SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE){
              reg_val = mep_db_entry->src_ip_add_ptr; 
              soc_mem_field_set(unit, mem, entry_above_64, SRC_IP_PTRf, &reg_val);
          }
      }

      if (mep_db_entry->micro_bfd) {
          arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_MICRO_BFD, &reg_val); 
          if (reg_val == -1) {
              SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("ARAD_PP_OAMP_PE_PROGS_MICRO_BFD program not loaded.")));
          }
          soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val); 
      }


      if (SOC_IS_QAX(unit)) {
          
          if (mep_db_entry->diag_profile == 0xf) {
              
              arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_BFD_ECHO, &reg_val);
              if (reg_val == -1) {
                  SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("ARAD_PP_OAMP_PE_PROGS_BFD_ECHO program not loaded.")));
              }
              soc_mem_field_set(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
          }
      }
  }

  if(!is_new_ep) {
      do
      {
          
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, mep_index, &entry_above_64));
          
          SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, mep_index, &entry_above_64_get));
          
          reg_val = 0;
          if(SOC_IS_JERICHO(unit)) {
              soc_mem_field_set(unit, mem, entry_above_64, ECCf, &reg_val);
              soc_mem_field_set(unit, mem, entry_above_64_get, ECCf, &reg_val);
          } else if (SOC_IS_ARAD(unit)){
              soc_mem_field_set(unit, mem, entry_above_64, PARITYf, &reg_val);
              soc_mem_field_set(unit, mem, entry_above_64_get, PARITYf, &reg_val);
          }
      } while (sal_memcmp(entry_above_64, entry_above_64_get, (soc_mem_entry_bytes(unit,mem))));
  } else {
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, mep_index, &entry_above_64));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_set_internal_unsafe()", mep_index, 0);
}


uint32
  arad_pp_oam_oamp_mep_db_entry_get_internal_unsafe(
    SOC_SAND_IN   int                   unit,
    SOC_SAND_IN   uint32                   mep_index,
    SOC_SAND_OUT  uint32                   *short_name,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  )
{
  uint32  res;
  soc_mem_t mem;
  uint32  reg_val;
  uint32 dummy , read_outlif = 0;
  soc_reg_above_64_val_t entry_above_64;
  uint32 inner_tpid, outer_tpid, inner_vid_dei_pcp, outer_vid_dei_pcp;
  uint32 eep_shift;
  uint32 short_name_lcl[1];
  uint32 flex_lm_dm_ptr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_INTERNAL_UNSAFE);

  sal_memset(mep_db_entry, 0, sizeof(*mep_db_entry));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, mep_index, entry_above_64));
  soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, MEP_TYPEf, &mep_db_entry->mep_type);

  
  if (SOC_IS_JERICHO(unit) && mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP)
  {
      mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE;
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 11, exit, _arad_pp_oam_bfd_mep_db_mem_by_entry_type_get(unit, mep_db_entry->mep_type, &mem));

  if (PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)) {
      reg_val = 0; 
      soc_mem_field_get(unit, mem, entry_above_64, LOCAL_PORTf, &reg_val);
      mep_db_entry->local_port = reg_val;
  }
  else {
      reg_val = 0; 
      soc_mem_field_get(unit, mem, entry_above_64, DEST_SYS_PORT_AGRf, &reg_val);
      mep_db_entry->local_port = reg_val;
  }

  if (mep_db_entry->mep_type != SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, PRIORITYf, &reg_val);
      mep_db_entry->priority = (uint8)reg_val;
  }

  if (SOC_IS_QAX(unit) && !_ARAD_PP_OAM_MEP_TYPE_IS_RFC6374(mep_db_entry->mep_type)) {
      flex_lm_dm_ptr = soc_mem_field32_get(unit, mem, entry_above_64, FLEX_LM_DM_PTRf);
      mep_db_entry->lm_dm_ptr = _ARAD_PP_OAM_HW_INDEX_TO_ACTUAL_INDEX(flex_lm_dm_ptr);
  }

  eep_shift = SOC_IS_ARADPLUS_A0(unit) ? 1 : 0;
  if ((mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL)) { 

      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, CCM_INTERVALf, &reg_val);
      mep_db_entry->ccm_interval = (uint8)reg_val;

      
      
      reg_val = 0;
      if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) && mep_db_entry->is_maid_48) {
          soc_mem_field_get(unit, mem, entry_above_64, MAIDf, &reg_val);
          mep_db_entry->maid_48_index = (uint8)reg_val;
      } else {
          soc_mem_field_get(unit, mem, entry_above_64, MAIDf, short_name_lcl);
          *short_name = *short_name_lcl;
      }    
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, MDLf, &reg_val);
      mep_db_entry->mdl = (uint8)reg_val; 
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, ICC_INDEXf, &reg_val);
      mep_db_entry->icc_ndx = (uint8)reg_val;
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, MEP_IDf, &reg_val);
      mep_db_entry->mep_id = (uint16)reg_val;
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, RDIf, &reg_val); 
      mep_db_entry->rdi = (uint8)reg_val;

      if (SOC_IS_QAX(unit)) {
          
          MBCM_PP_DRIVER_CALL(unit, mbcm_pp_get_mep_data_in_gen_mem, (unit, mep_index, &read_outlif, &dummy));
          mep_db_entry->out_lif = read_outlif;
      }
      if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
          reg_val = 0;
          soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, UP_1_DOWN_0f, &reg_val);
          mep_db_entry->is_upmep = (uint8)reg_val;
          soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, INNER_TPIDf, &inner_tpid); 
          soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, INNER_VID_DEI_PCPf, &inner_vid_dei_pcp);
          soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, OUTER_TPIDf, &outer_tpid); 
          soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, OUTER_VID_DEI_PCPf, &outer_vid_dei_pcp);
          reg_val = 0;
          soc_OAMP_MEP_DBm_field_get(unit, &entry_above_64, ENCAPSULATIONf, &reg_val);
          mep_db_entry->tags_num = (uint8)reg_val;
          switch (mep_db_entry->tags_num) {
          case 0: 
              mep_db_entry->inner_tpid = 0;
              mep_db_entry->inner_vid_dei_pcp = 0;
              mep_db_entry->outer_tpid = 0;
              mep_db_entry->outer_vid_dei_pcp = 0;
              break;
          case 1:
              
              mep_db_entry->outer_tpid = (uint8)inner_tpid;
              mep_db_entry->outer_vid_dei_pcp = (uint16)inner_vid_dei_pcp;
              mep_db_entry->inner_tpid = 0;
              mep_db_entry->inner_vid_dei_pcp = 0;
              break;
          case 2:
              mep_db_entry->inner_tpid = (uint8)inner_tpid; 
              mep_db_entry->inner_vid_dei_pcp = (uint16)inner_vid_dei_pcp; 
              mep_db_entry->outer_tpid = (uint8)outer_tpid;
              mep_db_entry->outer_vid_dei_pcp = (uint16)outer_vid_dei_pcp; 
              break;
          default:
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 82, exit);
          }

          if (SOC_IS_JERICHO(unit)) {
              uint32 sa_gen_profile;
              sa_gen_profile = soc_OAMP_MEP_DBm_field32_get(unit,&entry_above_64,SA_GEN_PROFILEf);
              
              mep_db_entry->src_mac_msb_profile = (sa_gen_profile >> 8 ) & 0x1;
              mep_db_entry->src_mac_lsb = sa_gen_profile & 0xff;
          }

      }
      else if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP ||
	           mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS || 
               mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL){
          reg_val = 0;        
          soc_mem_field_get(unit, mem, entry_above_64, EEPf, &reg_val);
          mep_db_entry->egress_if = reg_val << eep_shift;
          reg_val = 0; 
          soc_mem_field_get(unit, mem, entry_above_64, MPLS_LABELf, &reg_val);
          mep_db_entry->label = reg_val;
          reg_val = 0; 
          soc_mem_field_get(unit, mem, entry_above_64, MPLS_PUSH_PROFILEf, &reg_val);
          mep_db_entry->push_profile = reg_val;
      }
      else { 
          reg_val = 0;        
          soc_mem_field_get(unit, mem, entry_above_64, EEPf, &reg_val);
          mep_db_entry->egress_if = reg_val << eep_shift;
          reg_val = 0; 
          soc_mem_field_get(unit, mem, entry_above_64, PWE_LABELf, &reg_val);
          mep_db_entry->label = reg_val;
          reg_val = 0; 
          soc_mem_field_get(unit, mem, entry_above_64, PWE_PUSH_PROFILEf, &reg_val);
          mep_db_entry->push_profile = reg_val;
      }

      if (SOC_IS_ARADPLUS(unit)) {
          reg_val = 0; 
          soc_mem_field_get(unit, mem, entry_above_64, COUNTER_POINTERf, &reg_val);
          mep_db_entry->counter_pointer = reg_val;

          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, PORT_STATUS_TLV_ENf, &reg_val);
          mep_db_entry->port_status_tlv_en = reg_val;

          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, PORT_STATUS_TLV_VALf, &reg_val);
          mep_db_entry->port_status_tlv_val = reg_val;

          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, INTERFACE_STATUS_TLV_CODEf, &reg_val);
          mep_db_entry->interface_status_tlv_control = reg_val; 
      }
  }
  else if (_ARAD_PP_OAM_MEP_TYPE_IS_RFC6374(mep_db_entry->mep_type)) {
    reg_val = 0;
    soc_mem_field_get(unit, mem, entry_above_64, EEPf, &reg_val);
    mep_db_entry->egress_if = reg_val;

    reg_val = 0;
    soc_mem_field_get(unit, mem, entry_above_64, MPLS_PUSH_PROFILEf, &reg_val);
    mep_db_entry->push_profile = reg_val;

    reg_val = 0;
    soc_mem_field_get(unit, mem, entry_above_64, MPLS_LABELf, &reg_val);
    mep_db_entry->label = reg_val;

    reg_val = 0;
    soc_mem_field_get(unit, mem, entry_above_64, GALf, &reg_val);

    if (mep_db_entry->label == 13) {
      mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION;
    } else if (reg_val ) {
      mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP;
    }

    reg_val = 0;
    soc_mem_field_get(unit, mem, entry_above_64, COUNTER_POINTERf, &reg_val);
    mep_db_entry->counter_pointer = reg_val;

    reg_val = 0;
    soc_mem_field_get(unit, mem, entry_above_64, TIMESTAMP_FORMATf, &reg_val);
    mep_db_entry->timestamp_format = reg_val;

    reg_val = 0;
    soc_mem_field_get(unit, mem, entry_above_64, SESSION_IDENTIFIERf, &reg_val);
    mep_db_entry->session_identifier_id = reg_val;
  }
  else { 
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, TX_RATEf, &reg_val);
      mep_db_entry->ccm_interval = (uint8)reg_val;
      reg_val = 0;        
      soc_mem_field_get(unit, mem, entry_above_64, EEPf, &reg_val);
      mep_db_entry->egress_if = reg_val << eep_shift;
      if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, PBITf, &reg_val);
          mep_db_entry->pbit = reg_val; 
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, FBITf, &reg_val);
          mep_db_entry->fbit = reg_val; 
      }
      else {
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, DIAG_PROFILEf, &reg_val);
          mep_db_entry->diag_profile = reg_val;
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, FLAGS_PROFILEf, &reg_val);
          mep_db_entry->flags_profile = reg_val;
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, STAf, &reg_val);
          mep_db_entry->sta = reg_val;
      }
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, DETECT_MULTf, &reg_val);
      mep_db_entry->local_detect_mult = reg_val; 
      reg_val = 0;
      soc_mem_field_get(unit, mem, entry_above_64, MIN_RX_INTERVAL_PTRf, &reg_val);
      mep_db_entry->min_rx_interval_ptr = reg_val; 
      reg_val = 0; 
      soc_mem_field_get(unit, mem, entry_above_64, MIN_TX_INTERVAL_PTRf, &reg_val);
      mep_db_entry->min_tx_interval_ptr = reg_val;
      if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) {
          if (SOC_IS_ARAD_B0_AND_ABOVE((unit))) {
              reg_val = 0; 
              soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_get(unit, &entry_above_64, TUNNEL_IS_MPLSf, &reg_val);
              mep_db_entry->tunnel_is_mpls = reg_val;
              if (SOC_IS_JERICHO(unit)) {
                  uint32 mep_pe_prof_micro_bfd;
                  uint32 mep_pe_prof_disc_type_update_1;
                  soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_get(unit, &entry_above_64, DST_IPf, &reg_val);
                  mep_db_entry->dst_ip_add = reg_val;
                  soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_get(unit, &entry_above_64, IP_SUBNET_LENf, &reg_val);
                   mep_db_entry->ip_subnet_len = reg_val ;
                   arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_MICRO_BFD,&mep_pe_prof_micro_bfd);
                   soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_get(unit, &entry_above_64, MEP_PE_PROFILEf, &reg_val);
                   mep_db_entry->micro_bfd = (reg_val==mep_pe_prof_micro_bfd);
                   if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_discriminator_type_update", 0)) {
                       arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_MHOP_SHOP_BFD,&mep_pe_prof_disc_type_update_1);
                       soc_OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm_field_get(unit, &entry_above_64, MEP_PE_PROFILEf, &reg_val);
                       mep_db_entry->disc_type_update = (reg_val==mep_pe_prof_disc_type_update_1);
                   }
              }
          }
      }
      else if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
          reg_val = 0; 
          soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_get(unit, &entry_above_64, IP_TTL_TOS_PROFILEf, &reg_val);
          mep_db_entry->tos_ttl_profile = reg_val;
          reg_val = 0; 
          soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_get(unit, &entry_above_64, DST_IPf, &reg_val);
          mep_db_entry->dst_ip_add = reg_val;
          if (SOC_IS_QAX(unit)){
              soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_get(unit, &entry_above_64, PRIORITYf, &reg_val);
              mep_db_entry->priority = reg_val;
          }
          mep_db_entry->flex_tos_s_hop_m_bfd_flag = SOC_PPC_OAM_FLEX_TOS_M_HOP_OR_FEAT_NOT_SET;
          if (ARAD_PP_OAM_BFD_SEAMLESS_BFD_FEATURE_ENABLED(unit)) {
              uint32 mep_pe_prof_seamless_bfd; 
              arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_SEAMLESS_BFD,&mep_pe_prof_seamless_bfd);
              soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_get(unit, &entry_above_64, MEP_PE_PROFILEf, &reg_val);
              mep_db_entry->s_bfd_flag = (reg_val == mep_pe_prof_seamless_bfd)?SOC_PPC_OAM_S_BFD_INITIATOR:0;
          }
          if (ARAD_PP_OAM_BFD_IPv4_FLEX_IPTOS_FEATURE_SET(unit)) {
              uint32 mep_pe_prof_flex_tos_1, mep_pe_prof_flex_tos_2;
              uint32 mep_pe_prof_micro_bfd;
              arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_MICRO_BFD,&mep_pe_prof_micro_bfd);
              soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_get(unit, &entry_above_64, MEP_PE_PROFILEf, &reg_val);
              mep_db_entry->micro_bfd = (reg_val==mep_pe_prof_micro_bfd);
              if (mep_db_entry->micro_bfd == 1) {
                  mep_db_entry->flex_tos_s_hop_m_bfd_flag = SOC_PPC_OAM_FLEX_TOS_MICRO_BFD;
              } else {
                  arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_DPORT_AND_MYDISCR_UPDATE_BFD,&mep_pe_prof_flex_tos_1);
                  arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_CHANGE_DPORT_TO_SHOP_BFD,&mep_pe_prof_flex_tos_2);
                  soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_get(unit, &entry_above_64, MEP_PE_PROFILEf, &reg_val);
                  if (reg_val==mep_pe_prof_flex_tos_1 || reg_val == mep_pe_prof_flex_tos_2) {
                        mep_db_entry->flex_tos_s_hop_m_bfd_flag = SOC_PPC_OAM_FLEX_TOS_S_HOP;
                  }
              }
          }
          if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_discriminator_type_update", 0)) {
              uint32 mep_pe_prof_disc_type_update_1, mep_pe_prof_disc_type_update_2;
              arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_MHOP_SHOP_BFD,&mep_pe_prof_disc_type_update_1);
              arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_DPORT_AND_MYDISCR_UPDATE_BFD,&mep_pe_prof_disc_type_update_2);
              soc_OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm_field_get(unit, &entry_above_64, MEP_PE_PROFILEf, &reg_val);
              mep_db_entry->disc_type_update = (reg_val==mep_pe_prof_disc_type_update_1 || reg_val == mep_pe_prof_disc_type_update_2);
          }
      }
      else if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE){
          reg_val = 0; 
          soc_OAMP_MEP_DB_BFD_ON_PWEm_field_get(unit, &entry_above_64, PWE_LABELf, &reg_val);
          mep_db_entry->label = reg_val;
          reg_val = 0; 
          soc_OAMP_MEP_DB_BFD_ON_PWEm_field_get(unit, &entry_above_64, PWE_PUSH_PROFILEf, &reg_val);
          mep_db_entry->push_profile = reg_val;
          if (SOC_IS_JERICHO(unit)) {
              
              reg_val = 0; 
              soc_OAMP_MEP_DB_BFD_ON_PWEm_field_get(unit, &entry_above_64, ACHf, &reg_val);
              mep_db_entry->bfd_pwe_ach = reg_val;
              soc_OAMP_MEP_DB_BFD_ON_PWEm_field_get(unit, &entry_above_64, ROUTER_ALERTf, &reg_val);
              mep_db_entry->bfd_pwe_router_alert = reg_val;
              soc_OAMP_MEP_DB_BFD_ON_PWEm_field_get(unit, &entry_above_64, GALf, &reg_val);
              mep_db_entry->bfd_pwe_gal = reg_val;
              if (mep_db_entry->bfd_pwe_gal) {
                  mep_db_entry->bfd_pwe_ach=SOC_PPC_BFD_ACH_TYPE_GACH_CC;
              }
          }
      }
      else if ((mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS) || (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP)){
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, MPLS_LABELf, &reg_val);
          mep_db_entry->label = reg_val; 
          reg_val = 0;
          soc_mem_field_get(unit, mem, entry_above_64, MPLS_PUSH_PROFILEf, &reg_val);
          mep_db_entry->push_profile = reg_val; 
          if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS) {
              reg_val = 0; 
              soc_mem_field_get(unit, mem, entry_above_64, IP_TTL_TOS_PROFILEf, &reg_val);
              mep_db_entry->tos_ttl_profile = reg_val;
              if (ARAD_PP_OAM_BFD_SEAMLESS_BFD_FEATURE_ENABLED(unit)) {
                  uint32 mep_pe_prof_seamless_bfd; 
                  arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_SEAMLESS_BFD,&mep_pe_prof_seamless_bfd);
                  soc_mem_field_get(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
                  mep_db_entry->s_bfd_flag = (reg_val == mep_pe_prof_seamless_bfd)?SOC_PPC_OAM_S_BFD_INITIATOR:0;
              }
              if (SOC_IS_JERICHO(unit) && (mep_db_entry->is_vccv)) {
                  uint32 mep_pe_prof_mplstp_vccv; 
                  arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_MPLS_TP_VCCV,&mep_pe_prof_mplstp_vccv);
                  soc_mem_field_get(unit, mem, entry_above_64, MEP_PE_PROFILEf, &reg_val);
                  mep_db_entry->is_vccv = (reg_val == mep_pe_prof_mplstp_vccv) ? 1 : 0;
              }
          }
      }

      if (mep_db_entry->mep_type != SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP) {
          reg_val = 0; 
          soc_mem_field_get(unit, mem, entry_above_64, YOUR_DISCf, &reg_val);
          mep_db_entry->remote_discr = reg_val;
          if (mep_db_entry->mep_type != SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE){
              reg_val = 0;
              soc_mem_field_get(unit, mem, entry_above_64, SRC_IP_PTRf, &reg_val);
              mep_db_entry->src_ip_add_ptr = reg_val; 
          }
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_get_internal_unsafe()", mep_index, 0);
}


uint32
  arad_pp_oam_oamp_mep_db_entry_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry,
    SOC_SAND_IN  uint8                    allocate_icc_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_MA_NAME      name
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(mep_db_entry->mdl, _ARAD_PP_OAM_MAX_MD_LEVEL, ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_set_verify()", mep_index, 0);
}

uint32
  arad_pp_oam_oamp_mep_db_entry_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry,
    SOC_SAND_IN  uint8                    allocate_icc_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_MA_NAME      name,
    SOC_SAND_IN  int                      is_new_ep
  )
{
  uint32  res;
  uint32  short_name = 0;
  uint32  mac_add_U32[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
  uint32  entry;
  uint32  reg_val;
  uint64  msb_mac, msb_mac_existing;
  uint32  entry_buffer[2];

  SOC_PPC_OAM_ICC_MAP_DATA  icc_map_data;
  soc_reg_above_64_val_t  umc_name_above_64;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);

  COMPILER_64_ZERO(msb_mac);
  COMPILER_64_ZERO(msb_mac_existing);
  ARAD_PP_CLEAR(entry_buffer, uint32, 2);

 LOG_DEBUG(BSL_LS_SOC_OAM,
           (BSL_META_U(unit,
                       " Inserting entry to MEPDB: key %d\n\r"), mep_index));
   
  if ((mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) ||
      (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL)) { 
  
      
      res = soc_sand_pp_mac_address_struct_to_long(&mep_db_entry->src_mac_address, mac_add_U32);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM && SOC_IS_ARADPLUS_AND_BELOW(unit) ) {
          
          _ARAD_PP_OAM_MAC_ADD_MSB_GET(mac_add_U32, msb_mac);

          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, OAMP_CCM_MAC_SAr, REG_PORT_ANY, 0, BASE_MAC_SAf, &msb_mac_existing));

          if (COMPILER_64_IS_ZERO(msb_mac_existing)) { 
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, OAMP_CCM_MAC_SAr, REG_PORT_ANY, 0, BASE_MAC_SAf,  msb_mac));
          }
          else if (COMPILER_64_NE(msb_mac_existing,msb_mac)) {
                  
                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_OAMP_SA_SUFFIX_CONFLICT_ERR, 35, exit); 
          }
      }

      
      SOC_REG_ABOVE_64_CLEAR(umc_name_above_64);
      _arad_pp_oam_ma_name_divide(name, _ARAD_PP_OAM_IS_SHORT_FORMAT(mep_db_entry->icc_ndx), umc_name_above_64, &icc_map_data, &short_name);
      if (!_ARAD_PP_OAM_IS_SHORT_FORMAT(mep_db_entry->icc_ndx) && (mep_db_entry->is_maid_48 == 0)) {
          if (allocate_icc_ndx) {
              arad_pp_oam_icc_map_register_set_unsafe(unit, mep_db_entry->icc_ndx, &icc_map_data);
          }
          
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_OAMP_UMC_TABLEm(unit, MEM_BLOCK_ANY, _ARAD_PP_OAM_INDEX_TO_HW_LEGAL_INDEX(mep_index), entry_buffer));
          soc_OAMP_UMC_TABLEm_field_set(unit, entry_buffer, UMCf, umc_name_above_64);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_UMC_TABLEm(unit, MEM_BLOCK_ANY, _ARAD_PP_OAM_INDEX_TO_HW_LEGAL_INDEX(mep_index), entry_buffer));
      }
  }

  if (PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)) {
      if (mep_db_entry->local_port!=-1 && ((SOC_IS_ARAD_B0_AND_ABOVE(unit) && !mep_db_entry->is_upmep) || SOC_PPC_OAM_IS_MEP_TYPE_BFD(mep_db_entry->mep_type) ||
           mep_db_entry->remote_recycle_port)) {
          
          
          reg_val = mep_db_entry->remote_recycle_port? mep_db_entry->remote_recycle_port : mep_db_entry->system_port ; 
          if (SOC_IS_ARADPLUS(unit) && !SOC_IS_QAX(unit)) {
              entry=0;
              soc_OAMP_MEM_20000m_field32_set(unit, &entry, ITEM_0_15f, reg_val);
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_OAMP_MEM_20000m(unit, MEM_BLOCK_ANY,  mep_db_entry->local_port, &entry));
          } else {
              soc_field_t field = SOC_IS_QAX(unit) ? ITEM_0_15f : SYS_PORTf;
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_OAMP_LOCAL_PORT_2_SYSTEM_PORTm(unit, MEM_BLOCK_ANY, mep_db_entry->local_port, &entry));
              soc_OAMP_LOCAL_PORT_2_SYSTEM_PORTm_field_set(unit, &entry, field, &reg_val);
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_OAMP_LOCAL_PORT_2_SYSTEM_PORTm(unit, MEM_BLOCK_ANY, mep_db_entry->local_port, &entry));
          }
      }
  }

   
  res =   arad_pp_oam_oamp_mep_db_entry_set_internal_unsafe(unit, mep_index,short_name, mep_db_entry,is_new_ep);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_set_unsafe()", 0, 0);
}


uint32
  arad_pp_oam_oamp_mep_db_entry_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_get_verify()", mep_index, 0);
}

uint32
  arad_pp_oam_oamp_mep_db_entry_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  )
{
  uint32  res = 0;
  uint32  mac_add_U32[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S]={0};
  uint32  entry = 0;
  uint32  short_name;
  uint64  msb_mac;
  uint32  entry_buffer[2];

  SOC_PPC_OAM_ICC_MAP_DATA  icc_map_data;
  soc_reg_above_64_val_t  umc_name_above_64;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);

  COMPILER_64_ZERO(msb_mac);
  ARAD_PP_CLEAR(entry_buffer, uint32, 2);
   
    
  res = arad_pp_oam_oamp_mep_db_entry_get_internal_unsafe(unit, mep_index,&short_name, mep_db_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!SOC_PPC_OAM_IS_MEP_TYPE_BFD(mep_db_entry->mep_type)) { 
       
      if (!_ARAD_PP_OAM_IS_SHORT_FORMAT(mep_db_entry->icc_ndx) && (_ARAD_PP_OAM_INDEX_TO_HW_LEGAL_INDEX(mep_index) < SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit))) {
          res = arad_pp_oam_icc_map_register_get_unsafe(unit, mep_db_entry->icc_ndx, &icc_map_data);
          SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
          
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_OAMP_UMC_TABLEm(unit, MEM_BLOCK_ANY, _ARAD_PP_OAM_INDEX_TO_HW_LEGAL_INDEX(mep_index), entry_buffer));
          soc_OAMP_UMC_TABLEm_field_get(unit, entry_buffer, UMCf, umc_name_above_64);
      }

      if (mep_db_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_ETH_OAM ) {
          
          if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field64_read(unit, OAMP_CCM_MAC_SAr, REG_PORT_ANY, 0, BASE_MAC_SAf, &msb_mac)); 

              _ARAD_PP_OAM_MAC_ADD_MSB_SET(msb_mac, mac_add_U32);
              _ARAD_PP_OAM_MAC_ADD_LSB_SET(mep_db_entry->local_port, mac_add_U32);
          }
      }

      
         
      res = soc_sand_pp_mac_address_long_to_struct(mac_add_U32, &mep_db_entry->src_mac_address);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  if (PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)) {
      if (!mep_db_entry->is_upmep || SOC_PPC_OAM_IS_MEP_TYPE_BFD(mep_db_entry->mep_type)) {
          if (SOC_IS_ARAD_B0_AND_ABOVE(unit) || SOC_PPC_OAM_IS_MEP_TYPE_BFD(mep_db_entry->mep_type)) {
              
              if (SOC_IS_ARADPLUS(unit) && !SOC_IS_QAX(unit)) { 
                  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_OAMP_MEM_20000m(unit, MEM_BLOCK_ANY, mep_db_entry->local_port, &entry));
                  mep_db_entry->system_port = soc_OAMP_MEM_20000m_field32_get(unit, &entry, ITEM_0_15f); 
              } else {
                  soc_field_t field = SOC_IS_QAX(unit) ? ITEM_0_15f : SYS_PORTf;
                  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_OAMP_LOCAL_PORT_2_SYSTEM_PORTm(unit, MEM_BLOCK_ANY, mep_db_entry->local_port, &entry));
                  soc_OAMP_LOCAL_PORT_2_SYSTEM_PORTm_field_get(unit, &entry, field, &mep_db_entry->system_port);
              }
          } else {
              mep_db_entry->system_port = mep_db_entry->local_port;
          }
      } 
  }
  else {
     mep_db_entry->system_port = mep_db_entry->local_port;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_get_unsafe()", 0, 0);
}



uint32
  arad_pp_oam_oamp_mep_db_entry_delete_verify(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                   mep_index,
      SOC_SAND_IN  uint8                    deallocate_icc_ndx,
      SOC_SAND_IN  uint8                    is_last_mep
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_delete_verify()", mep_index, 0);
}

uint32
  arad_pp_oam_oamp_mep_db_entry_delete_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry,
    SOC_SAND_IN  uint8                    deallocate_icc_ndx,
    SOC_SAND_IN  uint8                    is_last_mep
  )
{
    uint32  res;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  lcl_mep_db_entry;
    const SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *p_mep_db_entry;
    soc_reg_above_64_val_t entry_above_64;
    soc_reg_above_64_val_t entry_above_64_get;
    uint32  entry_buffer[2];
    SOC_PPC_OAM_ICC_MAP_DATA icc_data;
    uint64  val64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE_UNSAFE);

    ARAD_PP_CLEAR(entry_buffer, uint32, 2);

    if (mep_db_entry == NULL) {
        SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&lcl_mep_db_entry);
        res = arad_pp_oam_oamp_mep_db_entry_get_unsafe(unit, mep_index, &lcl_mep_db_entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        p_mep_db_entry = &lcl_mep_db_entry;
    }
    else {
        p_mep_db_entry = mep_db_entry;
    }

    SOC_REG_ABOVE_64_CLEAR(entry_above_64);

    
    do
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, mep_index, entry_above_64));
        
        SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, mep_index, entry_above_64_get));

    } while (sal_memcmp(entry_above_64, entry_above_64_get, (soc_mem_entry_bytes(unit,OAMP_MEP_DBm))));

    
    if (!SOC_PPC_OAM_IS_MEP_TYPE_BFD(p_mep_db_entry->mep_type)) {

        if (deallocate_icc_ndx) {
            COMPILER_64_ZERO(icc_data);
            arad_pp_oam_icc_map_register_set_unsafe(unit, (int)(p_mep_db_entry->icc_ndx), &icc_data);
        }
        if (is_last_mep && p_mep_db_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_ETH_OAM &&SOC_IS_ARADPLUS_AND_BELOW(unit) ) {
                      
            COMPILER_64_ZERO(val64);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, OAMP_CCM_MAC_SAr, REG_PORT_ANY, 0, BASE_MAC_SAf,  val64));
        }
        if (!_ARAD_PP_OAM_IS_SHORT_FORMAT(p_mep_db_entry->icc_ndx) && (mep_index < SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit))) {
            
            SOC_REG_ABOVE_64_CLEAR(entry_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_OAMP_UMC_TABLEm(unit, MEM_BLOCK_ANY, mep_index, entry_buffer));
            soc_OAMP_UMC_TABLEm_field_set(unit, entry_buffer, UMCf, entry_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_OAMP_UMC_TABLEm(unit, MEM_BLOCK_ANY, mep_index, entry_buffer));
        }
    }

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_mep_db_entry_delete_unsafe()", 0, 0);
}



uint32
  arad_pp_oam_oamp_rmep_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint16                   rmep_id,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rmep_index, SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit), ARAD_PP_OAM_RMEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_set_verify()", rmep_index, 0);
}


static uint32 arad_pp_oam_oamp_rmep_db_set_unsafe(int unit,
                                                                uint32                   rmep_index,
                                                                uint32                   mep_index,
                                                                const SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry ,
                                                                uint8                    update )
{
    uint32  res;
    uint32  rmep_scan_time, lifetime, lifetime_units;
    uint64  val64;
    soc_reg_above_64_val_t  reg_above_64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_SET_UNSAFE);

    
    rmep_scan_time = _ARAD_PP_OAM_RMEP_SCAN(unit,arad_chip_kilo_ticks_per_sec_get(unit));
    if (SOC_SAND_DIV_ROUND_DOWN(rmep_db_entry->ccm_period, rmep_scan_time)<= 0x3FF){
        lifetime_units = _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD;
        lifetime = ((rmep_db_entry->ccm_period/ rmep_scan_time)< 1)? 1 :(rmep_db_entry->ccm_period / rmep_scan_time);
    } else if (SOC_SAND_DIV_ROUND_DOWN(rmep_db_entry->ccm_period/100, rmep_scan_time)<= 0x3FF){
        lifetime_units = _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD_100;
        lifetime = (((rmep_db_entry->ccm_period/100)/ rmep_scan_time)< 1)? 1 :((rmep_db_entry->ccm_period/100)/ rmep_scan_time);
    }    else {
        lifetime_units = _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD_10000;
        lifetime = (((((rmep_db_entry->ccm_period/1000)))/(rmep_scan_time*10))< 1)? 1 :((((rmep_db_entry->ccm_period/1000)))/(rmep_scan_time*10));
    }

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit,soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_RMEP_DBm, 0, MEM_BLOCK_ANY, rmep_index, reg_above_64));

    COMPILER_64_SET(val64, 0, lifetime);

    
    soc_mem_field64_set(unit, OAMP_RMEP_DBm,reg_above_64, LIFETIMEf, val64);
    soc_mem_field64_set(unit, OAMP_RMEP_DBm,reg_above_64, LAST_CCM_LIFETIMEf, val64);

    
    COMPILER_64_SET(val64, 0,(rmep_db_entry->last_ccm_lifetime_valid_on_create));
    soc_mem_field64_set(unit,OAMP_RMEP_DBm, reg_above_64, LAST_CCM_LIFETIME_VALIDf, val64);


    COMPILER_64_SET(val64, 0, lifetime_units);
    soc_mem_field64_set(unit,OAMP_RMEP_DBm, reg_above_64, LIFETIME_UNITSf, val64);
    if (rmep_db_entry->loc_clear_threshold == 0){
        COMPILER_64_SET(val64, 0, 3);
        soc_mem_field64_set(unit, OAMP_RMEP_DBm,reg_above_64, LOC_CLEAR_LIMTf, val64);
        COMPILER_64_SET(val64, 0, 0);
        soc_mem_field64_set(unit,OAMP_RMEP_DBm, reg_above_64, LOC_CLEAR_ENABLEf, val64);
    } else {
        COMPILER_64_SET(val64, 0, rmep_db_entry->loc_clear_threshold);
        soc_mem_field64_set(unit,OAMP_RMEP_DBm, reg_above_64, LOC_CLEAR_LIMTf, val64);
        COMPILER_64_SET(val64, 0, 1);
        soc_mem_field64_set(unit,OAMP_RMEP_DBm, reg_above_64, LOC_CLEAR_ENABLEf, val64);
    }
    if (SOC_IS_ARAD_B1_AND_BELOW(unit)){
        COMPILER_64_SET(val64, 0, rmep_db_entry->is_event_mask);
        soc_mem_field64_set(unit,OAMP_RMEP_DBm, reg_above_64, EVENT_MASKf, val64);
        COMPILER_64_SET(val64, 0, rmep_db_entry->is_state_auto_handle);
        soc_mem_field64_set(unit,OAMP_RMEP_DBm, reg_above_64, STATE_AUTO_HANDLEf, val64);
    }

    if (SOC_IS_ARADPLUS(unit)){
        COMPILER_64_SET(val64, 0, rmep_db_entry->punt_profile);
        soc_mem_field64_set(unit, OAMP_RMEP_DBm, reg_above_64, PUNT_PROFILEf, val64);

        COMPILER_64_SET(val64, 0, rmep_db_entry->rmep_state);
        soc_mem_field64_set(unit, OAMP_RMEP_DBm, reg_above_64, REMOTE_STATEf, val64);
    }

    if (SOC_IS_QAX(unit)) {
        
        soc_mem_field32_set(unit, OAMP_RMEP_DBm, reg_above_64, MEP_DB_PTRf, _ARAD_PP_OAM_INDEX_TO_HW_LEGAL_INDEX(mep_index));
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_RMEP_DBm(unit, MEM_BLOCK_ANY, rmep_index, reg_above_64));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("something went wrong", rmep_index, 0);
}

static uint32 arad_pp_oam_oamp_rmep_emc_set_unsafe(int unit,
                                                   uint32                   rmep_index,
                                                   uint16                   rmep_id,
                                                   uint32                   mep_index,
                                                   SOC_PPC_OAM_MEP_TYPE     mep_type) { 
  uint32  res;
  uint32  entry_buffer[2];
  uint32  reg_val;
  uint32  rmep_key;
  uint8   found;
#if SOC_DPP_IS_EM_HW_ENABLE
  uint64  fail_reason_val, fail_key_val;
  uint64  fail_valid_val;
  uint64  failure;

#endif
   uint32  mep_db_ptr_val_before = 0, mep_db_ptr_val_after = 0;
   soc_reg_above_64_val_t  reg_above_64;

   SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_SET_UNSAFE);

   res   = _arad_pp_oam_rmep_db_entry_key_get(unit, mep_type, _ARAD_PP_OAM_INDEX_TO_HW_LEGAL_INDEX(mep_index), &rmep_key, rmep_id);
   SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

     LOG_DEBUG(BSL_LS_SOC_OAM,
               (BSL_META_U(unit,
                           " Inserting entry to RMEPEM: key %d payload: %d\n\r"), rmep_key, rmep_index));

     if(SOC_IS_QAX(unit)) {
         SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit,soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_RMEP_DBm, 0 , MEM_BLOCK_ANY, 0, reg_above_64));
         mep_db_ptr_val_before = soc_mem_field32_get(unit, OAMP_RMEP_DBm,reg_above_64, MEP_DB_PTRf);
     }
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_RMAPEM_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0, entry_buffer));
      reg_val = 1; 
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_TYPEf, &reg_val);
      reg_val = 0; 
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_STAMPf, &reg_val);
      reg_val = 0; 
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_SELFf, &reg_val);
      reg_val = 0;
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_RESERVED_LOWf, &reg_val);
      reg_val = 3;
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_AGE_STATUSf, &reg_val);
      reg_val = rmep_key;
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_KEYf, &reg_val);
      reg_val = rmep_index; 
      soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_PAYLOADf, &reg_val);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_OAMP_RMAPEM_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0, entry_buffer));

      if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
          res = chip_sim_exact_match_entry_add_unsafe(
              unit,
              ARAD_CHIP_SIM_RMAPEM_BASE,
              &rmep_key,
              ARAD_CHIP_SIM_RMAPEM_KEY,
              &rmep_index,
              ARAD_CHIP_SIM_RMAPEM_PAYLOAD,
              &found
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
      }

#if SOC_DPP_IS_EM_HW_ENABLE

      res = arad_polling(
              unit,
              ARAD_TIMEOUT,
              ARAD_MIN_POLLS,
              OAMP_REMOTE_MEP_EXACT_MATCH_INTERRUPT_REGISTER_ONEr,
              REG_PORT_ANY,
              0,
              RMAPEM_MANAGEMENT_COMPLETEDf,
              1
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 75, exit, READ_OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_FAILUREr(unit, &failure));
      fail_valid_val = soc_reg64_field_get(unit, OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_FAILUREr, failure, REMOTE_MEP_EXACT_MATCH_MNGMNT_UNIT_FAILURE_VALIDf);

      if (COMPILER_64_LO(fail_valid_val)) {
                 
          fail_key_val = soc_reg64_field_get(unit, OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_FAILUREr, failure, REMOTE_MEP_EXACT_MATCH_MNGMNT_UNIT_FAILURE_KEYf);

          if (COMPILER_64_LO(fail_key_val) != rmep_key)
          {
              goto exit;
          }

          fail_reason_val = soc_reg64_field_get(unit, OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_FAILUREr, failure, REMOTE_MEP_EXACT_MATCH_MNGMNT_UNIT_FAILURE_REASONf);
          if (COMPILER_64_LO(fail_reason_val)==0x80) {
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_EM_FULL_ERR, 80, exit);
          }
          else {
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_EM_INTERNAL_ERR, 90, exit);
          }
      }
#endif

      if(SOC_IS_QAX(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit,soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_RMEP_DBm, 0, MEM_BLOCK_ANY, 0, reg_above_64));
          mep_db_ptr_val_after = soc_mem_field32_get(unit, OAMP_RMEP_DBm,reg_above_64, MEP_DB_PTRf);

          
          if((mep_db_ptr_val_before != mep_db_ptr_val_after) && (rmep_index)) {
              SOC_REG_ABOVE_64_CLEAR(reg_above_64);
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_RMEP_DBm(unit, MEM_BLOCK_ANY, 0, reg_above_64));
          }
      }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("something went wrong", rmep_index, 0);
}

uint32
  arad_pp_oam_oamp_rmep_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint16                   rmep_id,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry,
    SOC_SAND_IN  uint8                    update
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_SET_UNSAFE); 
  SOC_SAND_CHECK_NULL_INPUT(rmep_db_entry);
  
  
  SOC_SAND_CHECK_FUNC_RESULT(arad_pp_oam_oamp_rmep_db_set_unsafe(unit, rmep_index, mep_index,rmep_db_entry,update), 23, exit); 

  if (!update) { 
      SOC_SAND_CHECK_FUNC_RESULT(arad_pp_oam_oamp_rmep_emc_set_unsafe(unit, rmep_index, rmep_id, mep_index, mep_type), 23, exit);
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_set_unsafe()", 0, 0);
}


uint32
  arad_pp_oam_oamp_rmep_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rmep_index, SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit), ARAD_PP_OAM_RMEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_get_verify()", rmep_index, 0);
}

uint32
  arad_pp_oam_oamp_rmep_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  )
{
  uint32  res;
  soc_reg_above_64_val_t  entry;
  uint32  lifetime, lifetime_units, rmep_scan_time;
  uint32  field;
  uint32  loc_clear_enabled;
     
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_GET_UNSAFE); 
  SOC_SAND_CHECK_NULL_INPUT(rmep_db_entry);

  SOC_REG_ABOVE_64_CLEAR(entry);

    
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_RMEP_DBm, 0, MEM_BLOCK_ANY, rmep_index, entry));

  soc_OAMP_RMEP_DBm_field_get(unit, entry, LIFETIMEf, &lifetime);
  soc_OAMP_RMEP_DBm_field_get(unit, entry, LIFETIME_UNITSf, &lifetime_units);
  rmep_scan_time = _ARAD_PP_OAM_RMEP_SCAN(unit,arad_chip_kilo_ticks_per_sec_get(unit));
  if (lifetime_units == _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD) {
      rmep_db_entry->ccm_period = lifetime*rmep_scan_time;
  }
  else if (lifetime_units == _ARAD_PP_OAM_OAMP_RMEP_DB_LIFETIME_UNITS_SCAN_PERIOD_100){
      rmep_db_entry->ccm_period = (lifetime*rmep_scan_time)*100;
  }
  else {
      rmep_db_entry->ccm_period = SOC_SAND_DIV_ROUND(lifetime*rmep_scan_time, 100);
  } 

  soc_OAMP_RMEP_DBm_field_get(unit, entry, LOC_CLEAR_ENABLEf, &loc_clear_enabled);
  if (loc_clear_enabled) {
      soc_OAMP_RMEP_DBm_field_get(unit, entry, LOC_CLEAR_LIMTf, &(rmep_db_entry->loc_clear_threshold));
  }
  else {
      rmep_db_entry->loc_clear_threshold = 0;
  }

  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      soc_OAMP_RMEP_DBm_field_get(unit, entry, EVENT_MASKf, &field);
      rmep_db_entry->is_event_mask = (uint8)field;
      soc_OAMP_RMEP_DBm_field_get(unit, entry, STATE_AUTO_HANDLEf, &field);
      rmep_db_entry->is_state_auto_handle = (uint8)field;
  }
  else {
      soc_OAMP_RMEP_DBm_field_get(unit, entry, PUNT_PROFILEf, &field);
      rmep_db_entry->punt_profile = (uint8)field;

      soc_OAMP_RMEP_DBm_field_get(unit, entry, REMOTE_STATEf, &field);
      rmep_db_entry->rmep_state = field;

  }

  soc_OAMP_RMEP_DBm_field_get(unit, entry, RDI_RECEIVEDf, &field);
  rmep_db_entry->rdi_received = (uint8)field;

  soc_OAMP_RMEP_DBm_field_get(unit, entry, LOCf, &field);
  rmep_db_entry->loc =field; 
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_get_unsafe()", 0, 0);
}


uint32
  arad_pp_oam_oamp_rmep_delete_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint16                   rmep_id,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_DELETE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rmep_index, SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit), ARAD_PP_OAM_RMEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_delete_verify()", rmep_index, 0);
}

uint32
  arad_pp_oam_oamp_rmep_delete_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint16                   rmep_id,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type
  )
{
  uint32  res;
  uint32  reg_val;
  uint64  entry64;
  uint32  entry_buffer[2];
  uint32  rmep_key;
#if SOC_DPP_IS_EM_HW_ENABLE
  uint64  fail_reason_val, fail_key_val;
  uint64  fail_valid_val;
  uint64  failure;
#endif
  uint32  mep_db_ptr_val_before = 0, mep_db_ptr_val_after = 0;
  soc_reg_above_64_val_t  reg_above_64;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_DELETE_UNSAFE);

  ARAD_PP_CLEAR(entry_buffer, uint32, 2);

  res   = _arad_pp_oam_rmep_db_entry_key_get(unit, mep_type, _ARAD_PP_OAM_INDEX_TO_HW_LEGAL_INDEX(mep_index), &rmep_key, rmep_id);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  
  if(SOC_IS_QAX(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit,soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_RMEP_DBm, 0 , MEM_BLOCK_ANY, 0, reg_above_64));
      mep_db_ptr_val_before = soc_mem_field32_get(unit, OAMP_RMEP_DBm,reg_above_64, MEP_DB_PTRf);
  }
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_RMAPEM_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0, entry_buffer));
  reg_val = 0; 
  soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_TYPEf, &reg_val);
  reg_val = 0; 
  soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_STAMPf, &reg_val);
  soc_OAMP_RMAPEM_MANAGEMENT_REQUESTm_field_set(unit, entry_buffer, RMAPEM_KEYf, &rmep_key);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_RMAPEM_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ANY, 0, entry_buffer));

#if SOC_DPP_IS_EM_HW_ENABLE

  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          OAMP_REMOTE_MEP_EXACT_MATCH_INTERRUPT_REGISTER_ONEr,
          REG_PORT_ANY,
          0,
          RMAPEM_MANAGEMENT_COMPLETEDf,
          1
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 75, exit, READ_OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_FAILUREr(unit, &failure));
  fail_valid_val = soc_reg64_field_get(unit, OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_FAILUREr, failure, REMOTE_MEP_EXACT_MATCH_MNGMNT_UNIT_FAILURE_VALIDf);

  if (COMPILER_64_LO(fail_valid_val)) {
             
      fail_key_val = soc_reg64_field_get(unit, OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_FAILUREr, failure, REMOTE_MEP_EXACT_MATCH_MNGMNT_UNIT_FAILURE_KEYf);

      if (COMPILER_64_LO(fail_key_val) != rmep_key)
      {
          goto exit;
      }

      fail_reason_val = soc_reg64_field_get(unit, OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_UNIT_FAILUREr, failure, REMOTE_MEP_EXACT_MATCH_MNGMNT_UNIT_FAILURE_REASONf);
      if (COMPILER_64_LO(fail_reason_val)==0x80) {
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_EM_FULL_ERR, 80, exit);
      }
      else {
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_EM_INTERNAL_ERR, 90, exit);
      }
  }
#endif 

      if(SOC_IS_QAX(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit,soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_RMEP_DBm, 0, MEM_BLOCK_ANY, 0, reg_above_64));
          mep_db_ptr_val_after = soc_mem_field32_get(unit, OAMP_RMEP_DBm,reg_above_64, MEP_DB_PTRf);

          
          if((mep_db_ptr_val_before != mep_db_ptr_val_after) && (rmep_index)) {
              SOC_REG_ABOVE_64_CLEAR(reg_above_64);
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_RMEP_DBm(unit, MEM_BLOCK_ANY, 0, reg_above_64));
          }
      }

  
  COMPILER_64_ZERO(entry64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_OAMP_RMEP_DBm(unit, MEM_BLOCK_ANY, rmep_index, &entry64));

  if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
        res = chip_sim_exact_match_entry_remove_unsafe(
                    unit,
                    ARAD_CHIP_SIM_RMAPEM_BASE,
                    &rmep_key,
                    ARAD_CHIP_SIM_RMAPEM_KEY
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_delete_unsafe()", 0, 0);
}


uint32
  arad_pp_oam_oamp_rmep_index_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint16                   rmep_id,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type
  )
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_INDEX_GET_VERIFY);

    SOC_SAND_ERR_IF_ABOVE_MAX(mep_index, SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit), ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR, 10, exit);

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_delete_verify()", 0, 0);

}

uint32
  arad_pp_oam_oamp_rmep_index_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint16                   rmep_id,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type,
    SOC_SAND_OUT uint32                   *rmep_index,
    SOC_SAND_OUT  uint8                   *is_found
  )
{
  uint32  res;
  uint32  rmep_key;
  uint32  fld_val;
  uint32  reg_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_INDEX_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(rmep_index);

  res   = _arad_pp_oam_rmep_db_entry_key_get(unit, mep_type, mep_index, &rmep_key, rmep_id);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_KEYr, REG_PORT_ANY, 0, REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_KEYf,  rmep_key));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_REMOTE_MEP_EXACT_MATCH_DIAGNOSTICSr, REG_PORT_ANY, 0, REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_LOOKUPf,  1));

#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
  if (SOC_DPP_IS_EM_SIM_ENABLE(unit) 
#ifdef CRASH_RECOVERY_SUPPORT
      || ((SOC_IS_DONE_INIT(unit)) && (BCM_UNIT_DO_HW_READ_WRITE(unit)))
#endif 
      )
  {
      

      res = chip_sim_exact_match_entry_get_unsafe(
              unit,
              ARAD_CHIP_SIM_RMAPEM_BASE,
              &rmep_key,
              ARAD_CHIP_SIM_RMAPEM_KEY,
              &fld_val,
              ARAD_CHIP_SIM_RMAPEM_PAYLOAD,
              is_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

      if (*is_found) {
          *rmep_index = fld_val;
      }
      else {
          LOG_DEBUG(BSL_LS_SOC_OAM,
                    (BSL_META_U(unit,
                                " Getting entry to RMAPEM: key %d not found\n\r"), rmep_key));
          }
      ARAD_PP_DO_NOTHING_AND_EXIT;
  }
#endif

  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          OAMP_REMOTE_MEP_EXACT_MATCH_DIAGNOSTICSr,
          REG_PORT_ANY,
          0,
          REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_LOOKUPf,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_OAMP_REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_LOOKUP_RESULTr(unit, &reg_val)); 

  ARAD_FLD_FROM_REG(OAMP_REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_LOOKUP_RESULTr, REMOTE_MEP_EXACT_MATCH_ENTRY_FOUNDf, fld_val, reg_val, 50, exit);
  *is_found = fld_val;

  if (*is_found) {
      ARAD_FLD_FROM_REG(OAMP_REMOTE_MEP_EXACT_MATCH_DIAGNOSTICS_LOOKUP_RESULTr, REMOTE_MEP_EXACT_MATCH_ENTRY_PAYLOADf, fld_val, reg_val, 60, exit);
      *rmep_index = fld_val;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_rmep_index_get_unsafe()", 0, 0);
}



uint32
  arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   lif,
    SOC_SAND_IN  uint8                    md_level,
    SOC_SAND_IN  uint8                    is_upmep
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL_VERIFY);

    SOC_SAND_ERR_IF_ABOVE_MAX(md_level, _ARAD_PP_OAM_MAX_MD_LEVEL, ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(lif, OAM_LIF_MAX_VALUE(unit), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 20, exit); 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_verify()", 0, 0);
}

uint32
  arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   lif,
    SOC_SAND_IN  uint8                    md_level,
    SOC_SAND_IN  uint8                    is_upmep,
    SOC_SAND_OUT uint8                    *found_mep,
    SOC_SAND_OUT uint32                   *profile,
    SOC_SAND_OUT uint8                    *found_profile,
    SOC_SAND_OUT uint8                    *is_mp_type_flexible,
    SOC_SAND_OUT  uint8                    *is_mip
  )
{
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      oem1_key;
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  oem1_payload;
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY      oem2_key;
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  oem2_payload;
    uint8 passive_active_enable = 0;
    uint32  res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL_UNSAFE);

    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(&oem1_key);
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload);
    
    oem1_key.oam_lif = lif;
    oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
    res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, found_profile);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    *found_mep = FALSE;
    *is_mip = FALSE;
    *is_mp_type_flexible = TRUE;

    
    if (*found_profile) {
        *profile = oem1_payload.mp_profile;
        if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
            int mdl;
            int mp_type = JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(oem1_payload.mp_type_vector, md_level);
            *found_mep = mp_type == _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH || mp_type == _JER_PP_OAM_MDL_MP_TYPE_MIP;
            *found_profile = 2; 
            for (mdl=0 ; mdl < SOC_PPC_OAM_NOF_ETH_MP_LEVELS ; ++ mdl) {
                
                mp_type = JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(oem1_payload.mp_type_vector, mdl);
                if (mp_type == _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH || mp_type == _JER_PP_OAM_MDL_MP_TYPE_MIP) {
                    
                    *found_profile = 1;
                    break;
                }
            }
        } else {
            *found_mep = ((oem1_payload.mep_bitmap >> md_level) & 1);
            if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
                 res = arad_pp_oam_mep_passive_active_enable_get_unsafe(unit, *profile, &passive_active_enable);
                 SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            }

            if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && (!(*found_mep)) && (md_level != 0 )) {
                
                if (!passive_active_enable || SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) ) {
                    
                   *found_mep |= (((oem1_payload.mip_bitmap & 0x7) == md_level) || ((oem1_payload.mip_bitmap >> 3) & 0x7) == md_level);
                   if (*found_mep) {
                       *is_mip = TRUE;
                   }
                }
            }
            else if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                if ((oem1_payload.mip_bitmap >> md_level) & 1) {
                    *found_mep = *is_mip = TRUE;
                }
            }
        }

        if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
            
            if ((*found_mep) && (*profile==ARAD_PP_OAM_PROFILE_PASSIVE)) {
                *found_mep = FALSE;
            }
        }
        if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
            
            if (*found_mep) {
                SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY_clear(&oem2_key);
                SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_clear(&oem2_payload);
                
                oem2_key.oam_lif = lif;
                oem2_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
                oem2_key.mdl = md_level;
                res = arad_pp_oam_classifier_oem2_entry_get_unsafe(unit, &oem2_key, &oem2_payload, found_mep); 
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            }
        }
        else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
            
            if (oem1_payload.mip_bitmap == 0) {
                *is_mp_type_flexible = TRUE;
            }
            else if (passive_active_enable && (oem1_payload.mep_bitmap == 0)) {
                *is_mp_type_flexible = TRUE;
            }
            else {
                *is_mp_type_flexible = FALSE;
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_unsafe()", 0, 0);
}

uint32 arad_pp_oam_counter_range_set_unsafe( SOC_SAND_IN   int    unit,
                                             SOC_SAND_IN   uint32 counter_range_min,
                                             SOC_SAND_IN   uint32 counter_range_max)
{
    uint64 data;
    uint32  res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_COUNTER_RANGE_SET_UNSAFE);
    COMPILER_64_ZERO(data);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_OAM_COUNTER_RANGEr(unit, SOC_CORE_ALL, &data));

    soc_reg64_field32_set(unit, IHP_OAM_COUNTER_RANGEr, &data, OAM_COUNTER_MINf, counter_range_min);
    soc_reg64_field32_set(unit, IHP_OAM_COUNTER_RANGEr, &data, OAM_COUNTER_MAXf, counter_range_max);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_OAM_COUNTER_RANGEr(unit, SOC_CORE_ALL, data));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_counter_range_set_unsafe()", 0, 0);
}


uint32 arad_pp_oam_counter_range_set_verify( SOC_SAND_IN   int    unit,
                                             SOC_SAND_IN   uint32 counter_range_min,
                                             SOC_SAND_IN   uint32 counter_range_max)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_COUNTER_RANGE_SET_VERIFY);
    SOC_SAND_ERR_IF_ABOVE_NOF(counter_range_min, SOC_DPP_DEFS_GET(unit, nof_counters), ARAD_COUNTER_NDX_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_ABOVE_NOF(counter_range_max, SOC_DPP_DEFS_GET(unit, nof_counters), ARAD_COUNTER_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_counter_range_set_verify()", 0, 0);
}

uint32 arad_pp_oam_counter_range_get_unsafe( SOC_SAND_IN   int    unit,
                                             SOC_SAND_OUT  uint32 *counter_range_min,
                                             SOC_SAND_OUT  uint32 *counter_range_max )
{
    uint32  res;
    uint64  reg;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_COUNTER_RANGE_GET_UNSAFE);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_OAM_COUNTER_RANGEr(unit, 0, &reg));
    *counter_range_min = soc_reg64_field32_get(unit, IHP_OAM_COUNTER_RANGEr, reg, OAM_COUNTER_MINf);
    *counter_range_max = soc_reg64_field32_get(unit, IHP_OAM_COUNTER_RANGEr, reg, OAM_COUNTER_MAXf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_counter_range_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_counter_range_get_verify(
    SOC_SAND_IN   int                                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_COUNTER_RANGE_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_counter_range_get_verify()", 0, 0);

}


uint32
  arad_pp_oam_eth_oam_opcode_map_set_unsafe(
    SOC_SAND_IN   int                                     unit
  )
{
  uint8 internal_opcode;
  uint32 ethernet_opcode;
  uint32 reg;
  uint32 entry;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  internal_opcode_init[0].additional_info = ARAD_PP_OAM_OPCODE_CCM;

  for (ethernet_opcode=1; ethernet_opcode < SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT; ethernet_opcode++) {
      res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit, ethernet_opcode, &internal_opcode);
 
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
      if (!SOC_WARM_BOOT(unit)) {

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_ETHERNET_OAM_OPCODE_MAPm(unit, MEM_BLOCK_ANY, ethernet_opcode, &entry));
        reg = internal_opcode;
        soc_IHP_ETHERNET_OAM_OPCODE_MAPm_field_set(unit, &entry, OPCODEf, &reg);
        reg = _ARAD_PP_OAM_OPCODE_STAMP_OFFSET(internal_opcode, ethernet_opcode);
        soc_IHP_ETHERNET_OAM_OPCODE_MAPm_field_set(unit, &entry, OFFSETf, &reg);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_ETHERNET_OAM_OPCODE_MAPm(unit, MEM_BLOCK_ANY, ethernet_opcode, &entry));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EPNI_ETH_OAM_OPCODE_MAPm(unit, MEM_BLOCK_ANY, ethernet_opcode, &entry));
        reg = internal_opcode;
        soc_EPNI_ETH_OAM_OPCODE_MAPm_field_set(unit, &entry, ETH_OAM_OPCODE_MAP_DATAf, &reg);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_EPNI_ETH_OAM_OPCODE_MAPm(unit, MEM_BLOCK_ANY, ethernet_opcode, &entry));
      }

      switch (internal_opcode) {
      case SOC_PPC_OAM_OPCODE_MAP_CCM:
          internal_opcode_init[internal_opcode].additional_info = ARAD_PP_OAM_OPCODE_CCM;
          break;
      case SOC_PPC_OAM_OPCODE_MAP_LBR:
          internal_opcode_init[internal_opcode].additional_info = ARAD_PP_OAM_OPCODE_LBR;
          break;
      case SOC_PPC_OAM_OPCODE_MAP_LBM:
          internal_opcode_init[internal_opcode].additional_info = ARAD_PP_OAM_OPCODE_LBM;
          break;
      case SOC_PPC_OAM_OPCODE_MAP_LMR:
      case SOC_PPC_OAM_OPCODE_MAP_LMM:
          internal_opcode_init[internal_opcode].additional_info = ARAD_PP_OAM_OPCODE_LM;
          break;
      case SOC_PPC_OAM_OPCODE_MAP_SLM_SLR:
         if (SOC_IS_JERICHO_PLUS(unit)) {
            internal_opcode_init[internal_opcode].additional_info = ARAD_PP_OAM_OPCODE_SLM;
         }else{
            internal_opcode_init[internal_opcode].additional_info = ARAD_PP_OAM_OPCODE_LM;
         }
         break;
      case SOC_PPC_OAM_OPCODE_MAP_DMR:
      case SOC_PPC_OAM_OPCODE_MAP_DMM:
      case SOC_PPC_OAM_OPCODE_MAP_1DM:
          internal_opcode_init[internal_opcode].additional_info = ARAD_PP_OAM_OPCODE_DM;
          break;
      default:
          internal_opcode_init[internal_opcode].additional_info = ARAD_PP_OAM_OPCODE_OTHER;
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_eth_oam_opcode_map_set_unsafe()", 0, 0);
}




uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
    SOC_SAND_IN  SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  uint32 entry_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tos_ttl_data);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  entry_val = tos_ttl_data->ttl + (tos_ttl_data->tos << SOC_PPC_OAMP_LENGTH_TTL);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_IPV4_TOS_TTL_SELECTr_WITH_FAST_ACCESS(unit, reg_val));
  SHR_BITCOPY_RANGE(reg_val, SOC_PPC_OAMP_IPV4_TOS_TTL_DATA_NOF_BITS*ipv4_tos_ttl_select_index, &entry_val, 0, SOC_PPC_OAMP_IPV4_TOS_TTL_DATA_NOF_BITS);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_IPV4_TOS_TTL_SELECTr_WITH_FAST_ACCESS(unit, reg_val));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_tos_ttl_select_set_unsafe()", ipv4_tos_ttl_select_index, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
    SOC_SAND_IN  SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET_VERIFY);
  ARAD_STRUCT_VERIFY(SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA, tos_ttl_data, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_tos_ttl_select_set_verify()", 0, 0);

}

uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
    SOC_SAND_OUT SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  uint32 entry_val = 0;
  uint32 tmp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tos_ttl_data);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_IPV4_TOS_TTL_SELECTr_WITH_FAST_ACCESS(unit, reg_val));
  SHR_BITCOPY_RANGE(&entry_val, 0, reg_val, SOC_PPC_OAMP_IPV4_TOS_TTL_DATA_NOF_BITS*ipv4_tos_ttl_select_index, SOC_PPC_OAMP_IPV4_TOS_TTL_DATA_NOF_BITS);
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, &entry_val, 0, SOC_PPC_OAMP_LENGTH_TTL);
  tos_ttl_data->ttl = tmp & 0xff;
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, &entry_val, SOC_PPC_OAMP_IPV4_TOS_TTL_LENGTH_TOS, SOC_PPC_OAMP_LENGTH_TTL);
  tos_ttl_data->tos = tmp & 0xff;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_tos_ttl_select_get_unsafe()", ipv4_tos_ttl_select_index, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_tos_ttl_select_get_verify()", 0, 0);
}






uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
    SOC_SAND_IN  uint32                                       src_addr
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(reg_val);


  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_IPV4_SRC_ADDR_SELECTr_WITH_FAST_ACCESS(unit, reg_val));
  SHR_BITCOPY_RANGE(reg_val, SOC_PPC_OAMP_IPV4_SRC_ADDR_DATA_NOF_BITS*ipv4_src_addr_select_index, &src_addr, 0, SOC_PPC_OAMP_IPV4_SRC_ADDR_DATA_NOF_BITS);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_IPV4_SRC_ADDR_SELECTr_WITH_FAST_ACCESS(unit, reg_val));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_src_addr_select_set_unsafe()", ipv4_src_addr_select_index, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
    SOC_SAND_IN  uint32                                       src_addr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_src_addr_select_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
    SOC_SAND_OUT uint32                                       *src_addr
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(src_addr);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_IPV4_SRC_ADDR_SELECTr_WITH_FAST_ACCESS(unit, reg_val));
  SHR_BITCOPY_RANGE(src_addr, 0, reg_val, SOC_PPC_OAMP_IPV4_SRC_ADDR_DATA_NOF_BITS*ipv4_src_addr_select_index, SOC_PPC_OAMP_IPV4_SRC_ADDR_DATA_NOF_BITS);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_src_addr_select_set_unsafe()", ipv4_src_addr_select_index, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_src_addr_select_get_verify()", 0, 0);
}



uint32
  arad_pp_oam_bfd_tx_rate_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
    SOC_SAND_IN  uint32                                       tx_rate
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  soc_field_t bfd_rate_field[] = {BFD_TX_RATE_0f,BFD_TX_RATE_1f,BFD_TX_RATE_2f,BFD_TX_RATE_3f,BFD_TX_RATE_4f,BFD_TX_RATE_5f,BFD_TX_RATE_6f,
      BFD_TX_RATE_7f};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_RATE_SET_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_TX_RATEr_WITH_FAST_ACCESS(unit, reg_val));

  soc_reg_above_64_field32_set(unit,OAMP_BFD_TX_RATEr, reg_val,bfd_rate_field[bfd_tx_rate_index], tx_rate);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_BFD_TX_RATEr_WITH_FAST_ACCESS(unit, reg_val));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_rate_set_unsafe()", bfd_tx_rate_index, 0);
}

uint32
  arad_pp_oam_bfd_tx_rate_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
    SOC_SAND_IN  uint32                                       tx_rate
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_RATE_SET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_tx_rate_index, _ARAD_PP_OAM_TX_RATE_INDEX_MAX, ARAD_PP_OAM_TX_RATE_INDEX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_rate_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_tx_rate_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
    SOC_SAND_OUT uint32                                       *tx_rate
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  soc_field_t bfd_rate_field[] = { BFD_TX_RATE_0f, BFD_TX_RATE_1f, BFD_TX_RATE_2f, BFD_TX_RATE_3f, BFD_TX_RATE_4f, BFD_TX_RATE_5f, BFD_TX_RATE_6f,
      BFD_TX_RATE_7f };

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_RATE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(tx_rate);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_TX_RATEr_WITH_FAST_ACCESS(unit, reg_val));
  *tx_rate = soc_reg_above_64_field32_get(unit,OAMP_BFD_TX_RATEr, reg_val,bfd_rate_field[bfd_tx_rate_index]);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_rate_get_unsafe()", bfd_tx_rate_index, 0);
}

uint32
  arad_pp_oam_bfd_tx_rate_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        bfd_tx_rate_index
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_RATE_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(bfd_tx_rate_index, _ARAD_PP_OAM_TX_RATE_INDEX_MAX, ARAD_PP_OAM_TX_RATE_INDEX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_rate_get_verify()", 0, 0);
}


uint32
  arad_pp_oam_bfd_req_interval_pointer_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        req_interval_pointer,
    SOC_SAND_IN  uint32                                       req_interval
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_REQ_INTERVAL_POINTERr_WITH_FAST_ACCESS(unit, reg_val));
  SHR_BITCOPY_RANGE(reg_val, SOC_PPC_OAMP_BFD_REQ_INTERVAL_DATA_NOF_BITS*req_interval_pointer, &req_interval, 0, SOC_PPC_OAMP_BFD_REQ_INTERVAL_DATA_NOF_BITS);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_BFD_REQ_INTERVAL_POINTERr_WITH_FAST_ACCESS(unit, reg_val));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_req_interval_pointer_set_unsafe()", req_interval_pointer, 0);
}

uint32
  arad_pp_oam_bfd_req_interval_pointer_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        req_interval_pointer,
    SOC_SAND_IN  uint32                                       req_interval
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(req_interval_pointer, _ARAD_PP_OAM_REQ_INTERVAL_POINTER_MAX, ARAD_PP_OAM_REQ_INTERVAL_POINTER_OUT_OF_RANGE_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_req_interval_pointer_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_req_interval_pointer_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        req_interval_pointer,
    SOC_SAND_OUT uint32                                       *req_interval
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(req_interval);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_REQ_INTERVAL_POINTERr_WITH_FAST_ACCESS(unit, reg_val));
  SHR_BITCOPY_RANGE(req_interval, 0, reg_val, SOC_PPC_OAMP_BFD_REQ_INTERVAL_DATA_NOF_BITS*req_interval_pointer, SOC_PPC_OAMP_BFD_REQ_INTERVAL_DATA_NOF_BITS);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_req_interval_pointer_get_unsafe()", req_interval_pointer, 0);
}

uint32
  arad_pp_oam_bfd_req_interval_pointer_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        req_interval_pointer
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(req_interval_pointer, _ARAD_PP_OAM_REQ_INTERVAL_POINTER_MAX, ARAD_PP_OAM_REQ_INTERVAL_POINTER_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_req_interval_pointer_get_verify()", 0, 0);
}


uint32
  arad_pp_oam_mpls_pwe_profile_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        push_profile,
    SOC_SAND_IN  SOC_PPC_MPLS_PWE_PROFILE_DATA            *push_data
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  uint32 entry_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MPLS_PWE_PROFILE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(push_data);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  entry_val = push_data->ttl + (push_data->exp << SOC_PPC_OAMP_LENGTH_TTL);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_MPLS_PWE_PROFILEr_WITH_FAST_ACCESS(unit, reg_val));
  SHR_BITCOPY_RANGE(reg_val, SOC_PPC_OAMP_MPLS_PWE_PROFILE_NOF_BITS*push_profile, &entry_val, 0, SOC_PPC_OAMP_MPLS_PWE_PROFILE_NOF_BITS);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_MPLS_PWE_PROFILEr_WITH_FAST_ACCESS(unit, reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mpls_pwe_profile_set_unsafe()", push_profile, 0);
}

uint32
  arad_pp_oam_mpls_pwe_profile_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        push_profile,
    SOC_SAND_IN  SOC_PPC_MPLS_PWE_PROFILE_DATA            *push_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MPLS_PWE_PROFILE_SET_VERIFY);
  ARAD_STRUCT_VERIFY(SOC_PPC_MPLS_PWE_PROFILE_DATA, push_data, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mpls_pwe_profile_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_mpls_pwe_profile_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        push_profile,
    SOC_SAND_OUT SOC_PPC_MPLS_PWE_PROFILE_DATA            *push_data
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  uint32 entry_val = 0;
  uint32 tmp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MPLS_PWE_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(push_data);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_MPLS_PWE_PROFILEr_WITH_FAST_ACCESS(unit, reg_val));
  SHR_BITCOPY_RANGE(&entry_val, 0, reg_val, SOC_PPC_OAMP_MPLS_PWE_PROFILE_NOF_BITS*push_profile, SOC_PPC_OAMP_MPLS_PWE_PROFILE_NOF_BITS);
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, &entry_val, 0, SOC_PPC_OAMP_LENGTH_TTL);
  push_data->ttl = tmp & 0xff;
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, &entry_val, SOC_PPC_OAMP_MPLS_PWE_PROFILE_LENGTH_EXP, SOC_PPC_OAMP_LENGTH_TTL);
  push_data->exp = tmp & 0xff;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mpls_pwe_profile_get_unsafe()", push_profile, 0);
}

uint32
  arad_pp_oam_mpls_pwe_profile_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        push_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MPLS_PWE_PROFILE_GET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mpls_pwe_profile_get_verify()", 0, 0);
}


    uint32
arad_pp_seamless_bfd_rx_dst_port_set(
        SOC_SAND_IN  int                                       unit,
        SOC_SAND_IN  uint32                                     flags,
        SOC_SAND_IN  uint32                                     udp_port
        )
{
    uint32  res;
    uint64 field_64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    COMPILER_64_SET(field_64,0,udp_port);
    
    if (flags == _ARAD_PP_OAM_TCAM_FLAGS_SBFD_INITIATOR) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res,  10,  exit, soc_reg_above_64_field64_modify(unit, IHP_OAM_BFD_OVER_IP_CFGr, REG_PORT_ANY, 0, UDP_DST_PORT_BFD_MULTI_HOPf, field_64));
    }
    else if(flags == _ARAD_PP_OAM_TCAM_FLAGS_SBFD_REFLECTOR) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res,  10,  exit, soc_reg_above_64_field64_modify(unit, IHP_OAM_BFD_OVER_IP_CFGr, REG_PORT_ANY, 0, UDP_DST_PORT_BFD_MULTI_HOPf, field_64));
    }
    else if(flags == _ARAD_PP_OAM_TCAM_FLAGS_SBFD_REFLECTOR_OVER_MPLS) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res,  10,  exit, soc_reg_above_64_field64_modify(unit, IHP_OAM_BFD_OVER_IP_CFGr, REG_PORT_ANY, 0, UDP_DST_PORT_BFD_ONE_HOPf, field_64));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_seamless_bfd_rx_dst_port_set()", 0, 0);
}



    uint32
arad_pp_seamless_bfd_rx_dst_port_get(
        SOC_SAND_IN  int                                       unit,
        SOC_SAND_OUT  uint32                                    *udp_port
        )
{
    uint32  res;
    uint64  reg;


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    COMPILER_64_ZERO(reg);


    
    SOC_SAND_SOC_IF_ERROR_RETURN(res,  10,  exit, READ_IHP_OAM_BFD_OVER_IP_CFGr(unit, 0, &reg));

    *udp_port =  soc_reg64_field32_get(unit, IHP_OAM_BFD_OVER_IP_CFGr, reg, UDP_DST_PORT_BFD_MULTI_HOPf);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_seamless_bfd_rx_dst_port_set()", 0, 0);
}


uint32
  arad_pp_oam_bfd_mpls_udp_sport_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint16                                          udp_sport
  )
{
  uint32  res;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_BFD_MPLS_UDP_SPORTr(unit, udp_sport));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_mpls_udp_sport_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_mpls_udp_sport_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint16                                          udp_sport
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_mpls_udp_sport_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_mpls_udp_sport_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_OUT uint16                                       *udp_sport
  )
{
  uint32  res;
  uint32  reg;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(udp_sport);
  reg = 0;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_MPLS_UDP_SPORTr(unit, &reg));

  *udp_sport = (uint16)reg;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_mpls_udp_sport_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_mpls_udp_sport_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_mpls_udp_sport_get_verify()", 0, 0);
}


uint32
  arad_pp_oam_bfd_ipv4_udp_sport_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint16                                          udp_sport
  )
{
  uint32  res;
  uint32  reg;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET_UNSAFE);

  reg = udp_sport;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_BFD_IPV4_UDP_SPORTr(unit, reg));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_udp_sport_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_udp_sport_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint16                                          udp_sport
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_udp_sport_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_ipv4_udp_sport_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_OUT  uint16                                      *udp_sport
  )
{
  uint32  res;
  uint32  reg;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(udp_sport);
  reg = 0;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_IPV4_UDP_SPORTr(unit, &reg));
  *udp_sport = (uint16)reg;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_udp_sport_set_unsafe()", 0, 0);
}


uint32
  arad_pp_oam_bfd_ipv4_udp_sport_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_ipv4_udp_sport_get_verify()", 0, 0);
}




uint32
  arad_pp_oam_bfd_pdu_static_register_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  )
{
  uint32  res;
  uint64  reg, field64;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET_UNSAFE);
  COMPILER_64_ZERO(reg);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_PDU_STATIC_REGISTERr(unit, &reg));
  COMPILER_64_SET(field64,0, bfd_pdu->bfd_vers);
  soc_reg64_field_set(unit, OAMP_BFD_PDU_STATIC_REGISTERr, &reg, BFD_VERSf, field64);

  COMPILER_64_SET(field64,0, bfd_pdu->bfd_length);
  soc_reg64_field_set(unit, OAMP_BFD_PDU_STATIC_REGISTERr, &reg, BFD_LENGTHf, field64);

  COMPILER_64_SET(field64,0, bfd_pdu->bfd_req_min_echo_rx_interval);
  soc_reg64_field_set(unit, OAMP_BFD_PDU_STATIC_REGISTERr, &reg, BFD_REQ_MIN_ECHO_RX_INTERVALf, field64);

  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      COMPILER_64_SET(field64,0, bfd_pdu->bfd_diag);
      soc_reg64_field_set(unit, OAMP_BFD_PDU_STATIC_REGISTERr, &reg, BFD_DIAGf, field64);

      COMPILER_64_SET(field64,0, bfd_pdu->bfd_sta);
      soc_reg64_field_set(unit, OAMP_BFD_PDU_STATIC_REGISTERr, &reg, BFD_STAf, field64);

      COMPILER_64_SET(field64,0, bfd_pdu->bfd_flags);
      soc_reg64_field_set(unit, OAMP_BFD_PDU_STATIC_REGISTERr, &reg, BFD_FLAGSf, field64);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_BFD_PDU_STATIC_REGISTERr(unit, reg));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_pdu_static_register_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_pdu_static_register_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET_VERIFY);
  ARAD_STRUCT_VERIFY(SOC_PPC_BFD_PDU_STATIC_REGISTER, bfd_pdu, 10, exit);

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_pdu_static_register_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_pdu_static_register_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_OUT SOC_PPC_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  )
{
  uint32  res;
  uint64  reg,val64;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET_UNSAFE);
  COMPILER_64_ZERO(reg);
  SOC_SAND_CHECK_NULL_INPUT(bfd_pdu);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_PDU_STATIC_REGISTERr(unit, &reg));
  val64 = soc_reg64_field_get(unit, OAMP_BFD_PDU_STATIC_REGISTERr, reg, BFD_VERSf);
  bfd_pdu->bfd_vers = COMPILER_64_LO(val64);

  val64 = soc_reg64_field_get(unit, OAMP_BFD_PDU_STATIC_REGISTERr, reg, BFD_LENGTHf);
  bfd_pdu->bfd_length = COMPILER_64_LO(val64);

  val64 = soc_reg64_field_get(unit, OAMP_BFD_PDU_STATIC_REGISTERr, reg, BFD_REQ_MIN_ECHO_RX_INTERVALf);
  bfd_pdu->bfd_req_min_echo_rx_interval = COMPILER_64_LO(val64);

  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      val64 = soc_reg64_field_get(unit, OAMP_BFD_PDU_STATIC_REGISTERr, reg, BFD_DIAGf);
      bfd_pdu->bfd_diag = COMPILER_64_LO(val64);

      val64 = soc_reg64_field_get(unit, OAMP_BFD_PDU_STATIC_REGISTERr, reg, BFD_STAf);
      bfd_pdu->bfd_sta = COMPILER_64_LO(val64);

      val64 = soc_reg64_field_get(unit, OAMP_BFD_PDU_STATIC_REGISTERr, reg, BFD_FLAGSf);
      bfd_pdu->bfd_flags = COMPILER_64_LO(val64);
  }

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_pdu_static_register_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_pdu_static_register_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_pdu_static_register_get_verify()", 0, 0);
}


uint32
  arad_pp_oam_bfd_cc_packet_static_register_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  )
{
    soc_reg_above_64_val_t reg;
    uint32 res;
    uint8 first_word_in_reg[4] = {0};
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_CC_PACKETr(unit,reg));
    
    first_word_in_reg[0] = (bfd_cc_packet->bfd_static_reg_fields.bfd_vers) << 5 | (bfd_cc_packet->bfd_static_reg_fields.bfd_diag & 0x1f);
    
    first_word_in_reg[1] = bfd_cc_packet->bfd_static_reg_fields.bfd_sta <<6 | (bfd_cc_packet->bfd_static_reg_fields.bfd_flags &0x3f );
    
    first_word_in_reg[3] = bfd_cc_packet->bfd_static_reg_fields.bfd_length;
    SHR_BITCOPY_RANGE(reg,160, (uint32*)first_word_in_reg,0,32);
    
       
   
   SHR_BITCOPY_RANGE(reg, 128, &(bfd_cc_packet->bfd_my_discr), 0, 32);
   
   SHR_BITCOPY_RANGE(reg, 96, &(bfd_cc_packet->bfd_your_discr), 0, 32); 
   
   SHR_BITCOPY_RANGE(reg, 0, &(bfd_cc_packet->bfd_static_reg_fields.bfd_req_min_echo_rx_interval), 0, 32);

   SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_BFD_CC_PACKETr(unit,reg));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_cc_packet_static_register_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_cc_packet_static_register_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  ARAD_STRUCT_VERIFY(SOC_PPC_BFD_PDU_STATIC_REGISTER, &(bfd_cc_packet->bfd_static_reg_fields), 10, exit);
  if (SOC_IS_ARADPLUS(unit)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" Error: Unsupported device\n")));
  }

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_cc_packet_static_register_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_cc_packet_static_register_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_OUT SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  )
{
    soc_reg_above_64_val_t reg;
    uint32 res, just_a_num=0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_CC_PACKETr(unit,reg));
    

    
    SHR_BITCOPY_RANGE(&(just_a_num),0,reg,189,3);
    bfd_cc_packet->bfd_static_reg_fields.bfd_vers = just_a_num;
    
    just_a_num = 0;
    SHR_BITCOPY_RANGE(&just_a_num, 0,reg, 184,5);
    bfd_cc_packet->bfd_static_reg_fields.bfd_diag = just_a_num;
    
    just_a_num = 0;
   SHR_BITCOPY_RANGE(&just_a_num,0,reg,182,2);
   bfd_cc_packet->bfd_static_reg_fields.bfd_sta = just_a_num;
   
   just_a_num = 0;
   SHR_BITCOPY_RANGE(&just_a_num,0,reg, 176,6); 
   bfd_cc_packet->bfd_static_reg_fields.bfd_flags = just_a_num;
   
   just_a_num = 0;
   SHR_BITCOPY_RANGE( &just_a_num,0,reg, 160, 8);
   bfd_cc_packet->bfd_static_reg_fields.bfd_length  = just_a_num;
   
   SHR_BITCOPY_RANGE(&(bfd_cc_packet->bfd_my_discr), 0, reg, 128, 32);
   
   SHR_BITCOPY_RANGE(&(bfd_cc_packet->bfd_your_discr), 0, reg,96, 32); 
   
   SHR_BITCOPY_RANGE(&(bfd_cc_packet->bfd_static_reg_fields.bfd_req_min_echo_rx_interval) ,0,reg, 0, 32);




exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_cc_packet_static_register_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_cc_packet_static_register_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  if (SOC_IS_ARADPLUS(unit)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" Error: Unsupported device\n")));
  }
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_cc_packet_static_register_get_verify()", 0, 0);
}


uint32
  arad_pp_oam_bfd_discriminator_range_registers_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN uint32                                      range
  )
{
  uint32  res;
  uint64  reg;
  uint32  field_value;
  uint64  field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET_UNSAFE);
  COMPILER_64_ZERO(reg);

  
  field_value = range << SOC_PPC_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_BFD_MY_DISCRIMINATOR_RANGE_STARTr, REG_PORT_ANY, 0, BFD_MY_DISCRIMINATOR_RANGE_STARTf,  field_value));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, 0, &reg));
  field_value = range >>  (SOC_PPC_BFD_RX_YOUR_DISCRIMINATOR_RANGE_BIT_START-SOC_PPC_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START  );

  if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_discriminator_type_update", 0)) {
      
      uint32 min_range = field_value, count = 0;

      while (min_range >>= 1) {
          count++;
      }
      min_range = field_value & ~( 1<< count);
      COMPILER_64_SET(field64,0,min_range);
      soc_reg64_field_set(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, &reg, BFD_YOUR_DISCR_RANGE_MINf, field64);
  } else {
      COMPILER_64_SET(field64,0,field_value);
      soc_reg64_field_set(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, &reg, BFD_YOUR_DISCR_RANGE_MINf, field64);
  }
  COMPILER_64_SET(field64,0,field_value);
  soc_reg64_field_set(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, &reg, BFD_YOUR_DISCR_RANGE_MAXf, field64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, SOC_CORE_ALL, reg));

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_discriminator_range_registers_set_unsafe()", 0, 0);
}

soc_error_t arad_pp_oam_bfd_discriminator_rx_range_set(
   int unit,
   int  range_min,
   int  range_max)
{
    uint64  reg;
    uint64  field64;
    SOCDNX_INIT_FUNC_DEFS;


    COMPILER_64_ZERO(reg);

    SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, 0, &reg));


  COMPILER_64_SET(field64,0,range_min);
  soc_reg64_field_set(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, &reg, BFD_YOUR_DISCR_RANGE_MINf, field64);
  COMPILER_64_SET(field64,0,range_max);
  soc_reg64_field_set(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, &reg, BFD_YOUR_DISCR_RANGE_MAXf, field64);

  SOCDNX_SAND_IF_ERR_EXIT( WRITE_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, SOC_CORE_ALL, reg));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t arad_pp_oam_bfd_discriminator_rx_range_get(
   int unit,
   int * range_min,
   int * range_max)
{
    uint64  reg;
    SOCDNX_INIT_FUNC_DEFS;


    COMPILER_64_ZERO(reg);

    SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr(unit, 0, &reg));
    
    *range_min =  soc_reg64_field32_get(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, reg, BFD_YOUR_DISCR_RANGE_MINf);
    *range_max =  soc_reg64_field32_get(unit, IHP_OAM_BFD_YOUR_DISCR_TO_LIF_CFGr, reg, BFD_YOUR_DISCR_RANGE_MAXf); 


exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_oam_bfd_discriminator_range_registers_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint32                                        range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET_VERIFY);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_discriminator_range_registers_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_discriminator_range_registers_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT uint32                                      *range
  )
{
  uint32  res;
  uint32  field_value;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(range);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_BFD_MY_DISCRIMINATOR_RANGE_STARTr, REG_PORT_ANY, 0, BFD_MY_DISCRIMINATOR_RANGE_STARTf, &field_value));
  *range = field_value >> SOC_PPC_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START;

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_discriminator_range_registers_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_discriminator_range_registers_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_discriminator_range_registers_get_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS                   *ipv6_addr                            
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_above_64;
  uint32  valid;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET_UNSAFE);
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);

  valid = ipv6_addr->address[0] || ipv6_addr->address[1] || ipv6_addr->address[2] || ipv6_addr->address[3] ; 
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MY_BFD_DIPm(unit, MEM_BLOCK_ANY, dip_index, reg_above_64));
  soc_IHP_MY_BFD_DIPm_field_set(unit, reg_above_64, DIPf, (uint32*)ipv6_addr->address);
  soc_IHP_MY_BFD_DIPm_field_set(unit, reg_above_64, VALIDf, &valid);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_IHP_MY_BFD_DIPm(unit, MEM_BLOCK_ANY, dip_index, reg_above_64));

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_my_bfd_dip_ipv4_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint16                                       dip_index,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS                     * dip    
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET_VERIFY);
  SOC_SAND_CHECK_NULL_INPUT(dip);
  SOC_SAND_ERR_IF_ABOVE_MAX(dip_index, _ARAD_PP_OAM_DIP_INDEX_MAX, ARAD_PP_OAM_DIP_INDEX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_my_bfd_dip_ipv4_set_verify()", 0, 0);
}


uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
    SOC_SAND_OUT SOC_SAND_PP_IPV6_ADDRESS                    *ipv6_addr                            
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_above_64;
  uint32  valid;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(ipv6_addr);  
  soc_sand_SAND_PP_IPV6_ADDRESS_clear(ipv6_addr);
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MY_BFD_DIPm(unit, MEM_BLOCK_ANY, dip_index, reg_above_64));
  soc_IHP_MY_BFD_DIPm_field_get(unit, reg_above_64, VALIDf, &valid);
  if (valid) {
      soc_IHP_MY_BFD_DIPm_field_get(unit, reg_above_64, DIPf, ipv6_addr->address);
  }


  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_my_bfd_dip_ipv4_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint16                                       dip_index
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(dip_index, _ARAD_PP_OAM_DIP_INDEX_MAX, ARAD_PP_OAM_DIP_INDEX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_my_bfd_dip_ipv4_get_verify()", 0, 0);
}


uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  )
{
  uint32  res;
  soc_reg_above_64_val_t reg; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_UNSAFE);
  
  SOC_REG_ABOVE_64_CLEAR(reg);

  if (!SOC_IS_QAX(unit)) {
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_TX_IPV4_MULTI_HOPr_WITH_FAST_ACCESS(unit, reg));
      soc_reg_field_set(unit, OAMP_BFD_TX_IPV4_MULTI_HOPr, reg, BFD_IPV4_MULTI_HOP_TCf, tx_ipv4_multi_hop_att->tc);
      soc_reg_field_set(unit, OAMP_BFD_TX_IPV4_MULTI_HOPr, reg, BFD_IPV4_MULTI_HOP_DPf, tx_ipv4_multi_hop_att->dp);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_BFD_TX_IPV4_MULTI_HOPr_WITH_FAST_ACCESS(unit, reg));
  }

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_ipv4_multi_hop_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_VERIFY);
  ARAD_STRUCT_VERIFY(SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES, tx_ipv4_multi_hop_att, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_ipv4_multi_hop_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_OUT SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES               *tx_ipv4_multi_hop_att
  )
{
    uint32  res;
    soc_reg_above_64_val_t reg; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET_UNSAFE);
    SOC_REG_ABOVE_64_CLEAR(reg);

    if (!SOC_IS_QAX(unit)) {
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_TX_IPV4_MULTI_HOPr_WITH_FAST_ACCESS(unit, reg));
        tx_ipv4_multi_hop_att->tc = soc_reg_above_64_field32_get(unit, OAMP_BFD_TX_IPV4_MULTI_HOPr, reg, BFD_IPV4_MULTI_HOP_TCf);
        tx_ipv4_multi_hop_att->dp = soc_reg_above_64_field32_get(unit, OAMP_BFD_TX_IPV4_MULTI_HOPr, reg, BFD_IPV4_MULTI_HOP_DPf);
    }

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_ipv4_multi_hop_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_ipv4_multi_hop_get_verify()", 0, 0);
}



uint32
  arad_pp_oam_oamp_tx_priority_registers_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint32                                       priority,
    SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_oam_att
  )
{
  uint32  res;
  uint32  reg;
  uint32  field;

 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET_UNSAFE);
  reg = 0;

  field = (uint32)tx_oam_att->tc;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_PR_2_FW_DTCr(unit, &reg));
  SHR_BITCOPY_RANGE(&reg, (_ARAD_PP_OAM_TC_NOF_BITS+1)*priority, &field, 0, _ARAD_PP_OAM_TC_NOF_BITS); 
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_PR_2_FW_DTCr(unit, reg));
 
  field = (uint32)tx_oam_att->dp;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_OAMP_PR_2_FWDDPr(unit, &reg));
  SHR_BITCOPY_RANGE(&reg, _ARAD_PP_OAM_DP_NOF_BITS*priority, &field, 0, _ARAD_PP_OAM_DP_NOF_BITS);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_OAMP_PR_2_FWDDPr(unit, reg));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_tx_priority_registers_set_unsafe()", priority, 0);
}

uint32
  arad_pp_oam_oamp_tx_priority_registers_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint32                                       priority,
    SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_oam_att
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET_VERIFY);

  ARAD_STRUCT_VERIFY(SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES, tx_oam_att, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(priority, 7, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_tx_priority_registers_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_oamp_tx_priority_registers_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint32                                       priority,
    SOC_SAND_OUT  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES             *tx_oam_att
  )
{
    uint32  res;
    uint32  reg;
    uint32  field;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET_UNSAFE);
    reg = 0;

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_PR_2_FW_DTCr(unit, &reg));
    SHR_BITCOPY_RANGE(&field, 0, &reg, (_ARAD_PP_OAM_TC_NOF_BITS+1)*priority, _ARAD_PP_OAM_TC_NOF_BITS);  
    tx_oam_att->tc = (uint8)field;

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_OAMP_PR_2_FWDDPr(unit, &reg));
    SHR_BITCOPY_RANGE(&field, 0, &reg, _ARAD_PP_OAM_DP_NOF_BITS*priority, _ARAD_PP_OAM_DP_NOF_BITS);
    tx_oam_att->dp = (uint8)field;

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_tx_priority_registers_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_oamp_tx_priority_registers_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint32                                       priority
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(priority, 7, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_tx_priority_registers_get_verify()", 0, 0);
}

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        *interrupt_message_event_bmp
  )
{
    uint32  res;
    uint32  reg;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET_UNSAFE);

    reg = 0;
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr(unit, &reg));
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_LOC_SET_EVENTf, interrupt_message_event_bmp[SOC_PPC_OAM_EVENT_LOC_SET]);
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_LOC_CLEAR_EVENTf, interrupt_message_event_bmp[SOC_PPC_OAM_EVENT_LOC_CLR]);
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_RDI_SET_EVENTf, interrupt_message_event_bmp[SOC_PPC_OAM_EVENT_RDI_SET]);
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_RDI_CLEAR_EVENTf, interrupt_message_event_bmp[SOC_PPC_OAM_EVENT_RDI_CLR]);
    soc_reg_field_set(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, &reg, ENABLE_ALMOST_LOC_SET_EVENTf, interrupt_message_event_bmp[SOC_PPC_OAM_EVENT_ALMOST_LOC_SET]);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr(unit, reg)); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_enable_interrupt_message_event_set_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        *interrupt_message_event_bmp
  )
{
  uint32 i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET_VERIFY);
  for (i=0; i<SOC_PPC_OAM_EVENT_COUNT; i++) {
      if (i != SOC_PPC_OAM_EVENT_RMEP_STATE_CHANGE) {
          SOC_SAND_ERR_IF_ABOVE_MAX(interrupt_message_event_bmp[i], 1, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_enable_interrupt_message_event_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_OUT  uint8                                       *interrupt_message_event_bmp
  )
{
    uint32  res;
    uint32  reg;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET_UNSAFE);

    reg = 0;
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr(unit, &reg));
    interrupt_message_event_bmp[SOC_PPC_OAM_EVENT_NULL] = 0;
    interrupt_message_event_bmp[SOC_PPC_OAM_EVENT_LOC_SET] = soc_reg_field_get(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, reg, ENABLE_LOC_SET_EVENTf);
    interrupt_message_event_bmp[SOC_PPC_OAM_EVENT_LOC_CLR] = soc_reg_field_get(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, reg, ENABLE_LOC_CLEAR_EVENTf);
    interrupt_message_event_bmp[SOC_PPC_OAM_EVENT_RDI_SET] = soc_reg_field_get(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, reg, ENABLE_RDI_SET_EVENTf);
    interrupt_message_event_bmp[SOC_PPC_OAM_EVENT_RDI_CLR] = soc_reg_field_get(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, reg, ENABLE_RDI_CLEAR_EVENTf);
    interrupt_message_event_bmp[SOC_PPC_OAM_EVENT_ALMOST_LOC_SET] = soc_reg_field_get(unit, OAMP_ENABLE_INTERRUPT_MESSAGE_EVENTr, reg, ENABLE_ALMOST_LOC_SET_EVENTf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_enable_interrupt_message_event_get_unsafe()", 0, 0);
}

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_get_verify(
    SOC_SAND_IN  int                                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_enable_interrupt_message_event_get_verify()", 0, 0);
}

uint32
  arad_pp_oam_event_fifo_read_verify(
    SOC_SAND_IN  int                                        unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_EVENT_FIFO_READ_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_enable_interrupt_message_event_get_verify()", 0, 0);
}



uint32 arad_pp_oam_fifo_get_interrupt_mesage_dma_internal(
   SOC_SAND_IN  int                                        unit,
   SOC_SAND_OUT SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA   * interrupt_data) {

    uint32 empty_fifo_value = 0xffffffff;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if ((interrupt_data->last_interrupt_message_num == _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO ||
        (interrupt_data->interrupt_message[interrupt_data->last_interrupt_message_num] == empty_fifo_value && 
         (interrupt_data->num_entries_available_in_local_buffer -interrupt_data->num_entries_read_in_local_buffer ) > 0)) &&
        (interrupt_data->buffer_copied_from_dma_host_memory != NULL)) {
        
        
        interrupt_data->num_entries_read_in_local_buffer = (interrupt_data->num_entries_read_in_local_buffer== interrupt_data->num_entries_available_in_local_buffer)?
            interrupt_data->num_entries_read_in_local_buffer-1:  interrupt_data->num_entries_read_in_local_buffer;

        sal_memcpy(interrupt_data->interrupt_message, interrupt_data->buffer_copied_from_dma_host_memory + interrupt_data->num_entries_read_in_local_buffer * 20
                   , _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA);
        
        sal_memset(interrupt_data->buffer_copied_from_dma_host_memory + interrupt_data->num_entries_read_in_local_buffer * 20, 0xff
                   , _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA);
        ++interrupt_data->num_entries_read_in_local_buffer;
        interrupt_data->last_interrupt_message_num = 0;

        LOG_DEBUG(BSL_LS_SOC_OAM,
                  (BSL_META_U(unit,
                              "gonna read from the %d-th chunk in the host memory"), last_dma_interrupt_message_num[unit]));
    } 

    goto exit;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 arad_pp_oam_get_interrupt_mesages_from_dma_buffer_internal(
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_OUT uint32                              * dma_messages_data,
   SOC_SAND_OUT SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA   * interrupt_data) {

    uint32 empty_fifo_value = 0xffffffff;
    int message_index = 0;
    int first_message_index = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    if ((interrupt_data->last_interrupt_message_num == _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO ||
        (interrupt_data->interrupt_message[interrupt_data->last_interrupt_message_num] == empty_fifo_value && 
         (interrupt_data->num_entries_available_in_local_buffer -interrupt_data->num_entries_read_in_local_buffer ) > 0)) &&
        (interrupt_data->buffer_copied_from_dma_host_memory != NULL)) {
        
        
        interrupt_data->num_entries_read_in_local_buffer = (interrupt_data->num_entries_read_in_local_buffer== interrupt_data->num_entries_available_in_local_buffer)?
            interrupt_data->num_entries_read_in_local_buffer-1:  interrupt_data->num_entries_read_in_local_buffer;

        
        for (message_index=0; message_index < _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO; message_index++)
        {
          sal_memcpy(interrupt_data->interrupt_message, interrupt_data->buffer_copied_from_dma_host_memory + message_index * _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO
                     , _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA);
          dma_messages_data[message_index] = interrupt_data->interrupt_message[first_message_index];
        }
    } 

    goto exit;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 arad_pp_oam_event_fifo_get_interrupt_mesage_register_internal(
   SOC_SAND_IN  int                                        unit,
   SOC_SAND_OUT SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA   * interrupt_data) {
    uint32 res;
    soc_reg_above_64_val_t data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (interrupt_data->last_interrupt_message_num == _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_INTERRUPT_MESSAGEr(unit, data));
        soc_reg_above_64_field_get(unit, OAMP_INTERRUPT_MESSAGEr, data, INTERRUPT_MESSAGEf, interrupt_data->interrupt_message);
        interrupt_data->last_interrupt_message_num = 0;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 arad_pp_oam_event_fifo_get_interrupt_stat_mesage_register_internal(
   SOC_SAND_IN  int                                        unit,
   SOC_SAND_OUT SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA   * interrupt_data) {
    uint32 res;
    soc_reg_above_64_val_t data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (interrupt_data->last_interrupt_message_num == _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_STAT_INTERRUPT_MESSAGEr(unit, data));
        soc_reg_above_64_field_get(unit, OAMP_STAT_INTERRUPT_MESSAGEr, data, STAT_INTERRUPT_MESSAGEf, interrupt_data->interrupt_message);
        interrupt_data->last_interrupt_message_num = 0;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 arad_pp_oam_dma_dump_read_and_display_unsafe(
   SOC_SAND_IN  int                                        unit,
   SOC_SAND_OUT SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA   * interrupt_data) 
{
    uint32 res;
    int message_index = 0;
    uint32 dma_buffer_event_message[SOC_REG_ABOVE_64_MAX_SIZE_U32];
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit, arad_pp_oam_get_interrupt_mesages_from_dma_buffer_internal(unit,dma_buffer_event_message,interrupt_data));                                

    
    for (message_index = 0; message_index < _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO; ++message_index)
    {
      LOG_CLI((BSL_META_U(unit,"Reading from the DMA buffer message[%d]: 0x%08x\n"),message_index,dma_buffer_event_message[message_index]));
    }

    SOC_SAND_EXIT_NO_ERROR;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32 arad_pp_oam_event_fifo_read_unsafe(
    SOC_SAND_IN   int                                           unit,
    SOC_SAND_IN   int                                           event_type,
    SOC_SAND_OUT  uint32                                       *rmeb_db_ndx,
    SOC_SAND_OUT  uint32                                       *event_id,
    SOC_SAND_OUT  uint32                                       *valid,
    SOC_SAND_OUT  uint32                                       *event_buffer,
    SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA               *interrupt_data
  ) 
{
    uint32 res;
    SOC_PPC_OAM_EVENT_DATA event_data; 
    SOC_PPC_OAM_RX_REPORT_EVENT_DATA rx_report_data;
    uint32 field_32;
    uint32 empty_fifo_value;
    uint32 start_of_message;
    uint32 event_ndx;
    soc_dpp_config_pp_t *dpp_pp;
    uint32 rmeb_db_ndx_lcl[1];
    uint32 (*fill_interrupt_message) (int, SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA *);
    uint32 report_mode = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_EVENT_FIFO_READ_UNSAFE);

    dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

    
    if (event_type == SOC_PPC_OAM_DMA_EVENT_TYPE_STAT_EVENT) 
    {
        if (dpp_pp->oam_use_report_fifo_dma)
        { 
            fill_interrupt_message = arad_pp_oam_fifo_get_interrupt_mesage_dma_internal;
        }
        else 
        {
            fill_interrupt_message = arad_pp_oam_event_fifo_get_interrupt_stat_mesage_register_internal;
        }
    }
    else 
    {
        if (dpp_pp->oam_use_event_fifo_dma)
        { 
            fill_interrupt_message = arad_pp_oam_fifo_get_interrupt_mesage_dma_internal;
        }
        else 
        {
            fill_interrupt_message = arad_pp_oam_event_fifo_get_interrupt_mesage_register_internal;
        }
    }

    empty_fifo_value = SOC_IS_ARADPLUS(unit) ? 0xffffffff : 0xffffff;

    
    for (event_ndx=0; event_ndx<SOC_PPC_OAM_EVENT_COUNT; event_ndx++) {
        event_id[event_ndx]=0;
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit, fill_interrupt_message(unit,interrupt_data));                                

    LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          "Reading from the interrupt message. here are the first few bytes: 0x%08x%08x%08x%08x%08x%08x%08x\n"),interrupt_data->interrupt_message[6],interrupt_data->interrupt_message[5],interrupt_data->interrupt_message[4],
                          interrupt_data->interrupt_message[3],interrupt_data->interrupt_message[2],interrupt_data->interrupt_message[1],interrupt_data->interrupt_message[0]));

    LOG_VERBOSE(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "Here is the interruprt : %d \n"),interrupt_data->interrupt_message[0] ));


    if (interrupt_data->interrupt_message[interrupt_data->last_interrupt_message_num] == empty_fifo_value || 
        (SOC_IS_ARAD_B1_AND_BELOW(unit) && interrupt_data->interrupt_message[interrupt_data->last_interrupt_message_num] ==0) ) {
        *valid = 0;
        interrupt_data->last_interrupt_message_num = _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO;
    } else {   
        *valid = 1;
        *rmeb_db_ndx_lcl = 0;
        start_of_message = interrupt_data->last_interrupt_message_num*32;
        SHR_BITCOPY_RANGE(rmeb_db_ndx_lcl, 0, interrupt_data->interrupt_message, start_of_message, 16);
        *rmeb_db_ndx = *rmeb_db_ndx_lcl;

        if (SOC_IS_JERICHO(unit)) {
            
            SHR_BITCOPY_RANGE(&report_mode, 0, interrupt_data->interrupt_message, start_of_message + 21, 2);

        }

        if (!report_mode) {
            if (SOC_IS_ARADPLUS(unit)) {
                SOC_PPC_OAM_EVENT_DATA_clear(&event_data);
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+16, 2);
                event_data.loc = (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+18, 1);
                event_data.rdi_set = (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+19, 1);
                event_data.rdi_clear = (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+20, 1);
                event_data.rmep_state_change = (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&(event_data.rmep_state), 0, interrupt_data->interrupt_message, start_of_message+23, 9);
                event_buffer[0] = event_data.rmep_state;
                
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+23, 1);
                event_data.sd_set = (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+24, 1);
                event_data.sd_clear= (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+25, 1);
                event_data.sf_set = (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+26, 1);
                event_data.sf_clear = (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+27, 1);
                event_data.dExcess_set = (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+28, 1);
                event_data.dMissmatch= (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+29, 1);
                event_data.dMissmerge= (uint8)field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+30, 1);
                event_data.dAll_clear= (uint8)field_32;
                field_32 = 0;

                if (event_data.loc == 1) {
                    event_id[SOC_PPC_OAM_EVENT_LOC_SET] = 1;
                }
                else if (event_data.loc == 2) {
                    event_id[SOC_PPC_OAM_EVENT_ALMOST_LOC_SET] = 1;
                }
                else if (event_data.loc == 3) {
                    event_id[SOC_PPC_OAM_EVENT_LOC_CLR] = 1;
                }
                if (event_data.rdi_set) {
                    event_id[SOC_PPC_OAM_EVENT_RDI_SET] = 1;
                }
                else if (event_data.rdi_clear) {
                    event_id[SOC_PPC_OAM_EVENT_RDI_CLR] = 1;
                }
                if (event_data.sd_set) {
                    event_id[SOC_PPC_OAM_EVENT_SD_SET] = 1;
                }
                if (event_data.sd_clear) {
                    event_id[SOC_PPC_OAM_EVENT_SD_CLR] = 1;
                }
                if (event_data.sf_set) {
                    event_id[SOC_PPC_OAM_EVENT_SF_SET] = 1;
                }
                if (event_data.sf_clear) {
                    event_id[SOC_PPC_OAM_EVENT_SF_CLR] = 1;
                }
                if (event_data.dExcess_set) {
                    event_id[SOC_PPC_OAM_EVENT_DEXCESS_SET] = 1;
                }
                if (event_data.dMissmatch) {
                    event_id[SOC_PPC_OAM_EVENT_DMISSMATCH] = 1;
                }
                if (event_data.dMissmerge) {
                    event_id[SOC_PPC_OAM_EVENT_DMISSMERGE] = 1;
                }
                if (event_data.dAll_clear) {
                    event_id[SOC_PPC_OAM_EVENT_DALL_CLR] = 1;
                }
                if (event_data.rmep_state_change) {
                    event_id[SOC_PPC_OAM_EVENT_RMEP_STATE_CHANGE] = 1;
                }
            }
            else {
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+16, 8);
                switch (field_32) {
                case SOC_PPC_OAM_EVENT_LOC_SET:
                    event_id[SOC_PPC_OAM_EVENT_LOC_SET] = 1;
                    break;
                case SOC_PPC_OAM_EVENT_ALMOST_LOC_SET:
                    event_id[SOC_PPC_OAM_EVENT_ALMOST_LOC_SET] = 1;
                    break;
                case SOC_PPC_OAM_EVENT_LOC_CLR:
                    event_id[SOC_PPC_OAM_EVENT_LOC_CLR] = 1;
                    break;
                case SOC_PPC_OAM_EVENT_RDI_SET:
                    event_id[SOC_PPC_OAM_EVENT_RDI_SET] = 1;
                    break;
                case SOC_PPC_OAM_EVENT_RDI_CLR:
                    event_id[SOC_PPC_OAM_EVENT_RDI_CLR] = 1;
                    break;
                }
            }
        }
        else {
            if (report_mode == 2) {
                SOC_PPC_OAM_RX_REPORT_EVENT_DATA_clear(&rx_report_data);
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+16, 4);
                rx_report_data.event_size = field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+20, 1);
                rx_report_data.event_type = (uint8)field_32;
                
                SHR_BITCOPY_RANGE(event_buffer, 0, interrupt_data->interrupt_message, start_of_message + 32, 32 * (rx_report_data.event_size - 1));
                interrupt_data->last_interrupt_message_num += rx_report_data.event_size;
                
                if (rx_report_data.event_type == 1 ) {
                    if (rx_report_event_mode[unit] == SOC_PPC_OAM_REPORT_MODE_COMPACT) {
                        
                        event_buffer[1] = 0;
                        SHR_BITCOPY_RANGE(&event_buffer[1], 0, interrupt_data->interrupt_message, start_of_message+23, 31 - 23 + 1);
                    }
                    event_id[SOC_PPC_OAM_EVENT_REPORT_RX_DM] = 1;
                }
                else {
                    if (rx_report_event_mode[unit] == SOC_PPC_OAM_REPORT_MODE_COMPACT) {
                        
                        event_buffer[1] = event_buffer[0] & 0xFFFF;
                        event_buffer[0] = (event_buffer[0] >> 16) & 0xFFFF;
                    }
                    event_id[SOC_PPC_OAM_EVENT_REPORT_RX_LM] = 1;
                }
            }
            else if (report_mode == 3) {  
                SOC_PPC_OAM_RX_REPORT_EVENT_DATA_clear(&rx_report_data);
                
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+0, 5);
                
                event_id[SOC_PPC_OAM_EVENT_SAT_FLOW_ID] = field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+16, 4);
                rx_report_data.event_size = field_32;
                field_32 = 0;
                SHR_BITCOPY_RANGE(&field_32, 0, interrupt_data->interrupt_message, start_of_message+14, 2);
                rx_report_data.event_type = (uint8)field_32;
                
                SHR_BITCOPY_RANGE(event_buffer, 0, interrupt_data->interrupt_message, start_of_message + 32, 32 * (rx_report_data.event_size - 1));
                interrupt_data->last_interrupt_message_num += rx_report_data.event_size;
                
                if (rx_report_data.event_type & 1) { 
                    event_id[SOC_PPC_OAM_EVENT_REPORT_RX_DM] = 1;
                }

                if (rx_report_data.event_type & 2){ 
                    event_id[SOC_PPC_OAM_EVENT_REPORT_RX_LM] = 1;
                }

                
                *rmeb_db_ndx = 0xffffffff;
            }
            else {
                
                *valid = 0;
                interrupt_data->last_interrupt_message_num = _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO;
                LOG_DEBUG(BSL_LS_SOC_OAM,
                          (BSL_META_U(unit,
                                      " MEP performance event received. Not supported.\n")));
                SOC_SAND_EXIT_NO_ERROR;
            }
        }

        if (report_mode != 3) {
            interrupt_data->last_interrupt_message_num++;
        }
        LOG_DEBUG(BSL_LS_SOC_OAM,
                  (BSL_META_U(unit,
                              " rmeb_db_ndx=%d\n"), *rmeb_db_ndx));
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_oam_event_fifo_read()", 0, 0);
}

uint32
  arad_pp_oam_pp_pct_profile_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  oam_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EPNI_PP_PCT_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_PP_PCT_PROFILE_SET_UNSAFE);
  
  res = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core_id, local_port_ndx, &tbl_data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  tbl_data.oam_port_profile = oam_profile;

  res = arad_pp_epni_pp_pct_tbl_set_unsafe(unit, core_id, local_port_ndx, &tbl_data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_pp_pct_profile_set_unsafe()", local_port_ndx, 0);
}

uint32
  arad_pp_oam_pp_pct_profile_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  oam_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_PP_PCT_PROFILE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(oam_profile, 1, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_pp_pct_profile_set_verify()", local_port_ndx, 0);
}

uint32
  arad_pp_oam_pp_pct_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_PP_PCT_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_pp_pct_profile_get_verify()", local_port_ndx, 0);
}

uint32
  arad_pp_oam_pp_pct_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *oam_profile
  )
{
  ARAD_PP_EPNI_PP_PCT_TBL_DATA
    tbl_data;
  soc_error_t
    rv;

  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_PP_PCT_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(oam_profile);

  rv = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core_id, local_port_ndx, &tbl_data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  *oam_profile = tbl_data.oam_port_profile;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_pp_pct_profile_get_unsafe()", local_port_ndx, 0);
}



uint32
  arad_pp_oam_bfd_diag_profile_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx,
    SOC_SAND_IN  uint32                                       diag_profile
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_DIAG_PROFILEr(unit, reg_val));
  SHR_BITCOPY_RANGE(reg_val, SOC_PPC_OAMP_BFD_DIAG_PROFILE_NOF_BITS*profile_ndx, &diag_profile, 0, SOC_PPC_OAMP_BFD_DIAG_PROFILE_NOF_BITS);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_BFD_DIAG_PROFILEr(unit, reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_diag_profile_set_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_oam_bfd_diag_profile_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx,
    SOC_SAND_IN  uint32                                       diag_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_diag_profile_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_diag_profile_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx,
    SOC_SAND_OUT  uint32                                      *diag_profile
  )
{
  uint32  res;
  soc_reg_above_64_val_t  reg_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(diag_profile);
  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_DIAG_PROFILEr(unit, reg_val));
  SHR_BITCOPY_RANGE(diag_profile, 0, reg_val, SOC_PPC_OAMP_BFD_DIAG_PROFILE_NOF_BITS*profile_ndx, SOC_PPC_OAMP_BFD_DIAG_PROFILE_NOF_BITS);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_diag_profile_get_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_oam_bfd_diag_profile_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_diag_profile_get_verify()", 0, 0);
}


uint32
  arad_pp_oam_bfd_flags_profile_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx,
    SOC_SAND_IN  uint32                                       flags_profile
  )
{
  uint32  res;
  uint64  reg_val;
  soc_reg_above_64_val_t  reg_above_64_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  COMPILER_64_ZERO(reg_val);
  SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_BFD_FLADS_PROFILEr(unit, &reg_val));
  reg_above_64_val[0] = COMPILER_64_LO(reg_val);
  reg_above_64_val[1] = COMPILER_64_HI(reg_val);
  SHR_BITCOPY_RANGE(reg_above_64_val, SOC_PPC_OAMP_BFD_FLAGS_PROFILE_NOF_BITS*profile_ndx, &flags_profile, 0, SOC_PPC_OAMP_BFD_FLAGS_PROFILE_NOF_BITS);
  COMPILER_64_SET(reg_val, reg_above_64_val[1], reg_above_64_val[0]);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_BFD_FLADS_PROFILEr(unit, reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_flags_profile_set_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_oam_bfd_flags_profile_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx,
    SOC_SAND_IN  uint32                                       flags_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_flags_profile_set_verify()", 0, 0);
}

uint32
  arad_pp_oam_bfd_flags_profile_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx,
    SOC_SAND_OUT  uint32                                      *flags_profile
  )
{
  uint32  res;
  uint64  reg_val;
  soc_reg_above_64_val_t  reg_above_64_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(flags_profile);
  COMPILER_64_ZERO(reg_val);
  SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_OAMP_BFD_FLADS_PROFILEr(unit, &reg_val));
  reg_above_64_val[0] = COMPILER_64_LO(reg_val);
  reg_above_64_val[1] = COMPILER_64_HI(reg_val);
  SHR_BITCOPY_RANGE(flags_profile, 0, reg_above_64_val, SOC_PPC_OAMP_BFD_FLAGS_PROFILE_NOF_BITS*profile_ndx, SOC_PPC_OAMP_BFD_DIAG_PROFILE_NOF_BITS);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_flags_profile_get_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_oam_bfd_flags_profile_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  uint8                                        profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_flags_profile_get_verify()", 0, 0);
}

uint32
  arad_pp_oam_mep_passive_active_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  uint8                                  enable
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32 reg, field;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  reg = 0;
  field = (uint32)enable;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_OAM_MEP_PASSIVE_ACTIVE_ENABLEr(unit, &reg));
  SHR_BITCOPY_RANGE(&reg, profile_ndx, &field, 0, 1); 
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_OAM_MEP_PASSIVE_ACTIVE_ENABLEr(unit, reg)); 
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mep_passive_active_enable_set_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_oam_mep_passive_active_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  uint8                                  enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(enable, 1, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mep_passive_active_enable_set_verify()", profile_ndx, 0);
}

uint32
  arad_pp_oam_mep_passive_active_enable_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mep_passive_active_enable_get_verify()", profile_ndx, 0);
}

uint32
  arad_pp_oam_mep_passive_active_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  uint8                                 *enable
  )
{
  uint32
      res = SOC_SAND_OK;
  uint32 reg, field=0;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  reg = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_OAM_MEP_PASSIVE_ACTIVE_ENABLEr(unit, &reg)); 
  SHR_BITCOPY_RANGE(&field, 0, &reg, profile_ndx, 1); 
  *enable = (uint8)field;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_mep_passive_active_enable_get_unsafe()", profile_ndx, 0);
}



uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_punt_event_hendling_profile_set_verify(
          unit,
		  profile_ndx,
		  punt_profile_data							
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_punt_event_hendling_profile_set_unsafe(
          unit,
		  profile_ndx,
		  punt_profile_data	
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_punt_event_hendling_profile_set()", 0, 0);
}

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint64 reg, field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  COMPILER_64_ZERO(reg);




  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_PUNT_EVENT_HENDLINGr(unit, &reg));
  switch (profile_ndx) {
  case 0:
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_PUNT_RATEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_PUNT_ENABLEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->rx_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_RX_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->scan_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_SCAN_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_MEP_RDI_UPDATE_LOC_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_clear_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_MEP_RDI_UPDATE_LOC_CLEAR_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_rx_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_0_MEP_RDI_UPDATE_RX_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      break;
  case 1:
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_PUNT_RATEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_PUNT_ENABLEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->rx_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_RX_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->scan_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_SCAN_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_MEP_RDI_UPDATE_LOC_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_clear_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_MEP_RDI_UPDATE_LOC_CLEAR_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_rx_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_1_MEP_RDI_UPDATE_RX_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      break;
  case 2:
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_PUNT_RATEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_PUNT_ENABLEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->rx_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_RX_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->scan_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_SCAN_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_MEP_RDI_UPDATE_LOC_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_clear_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_MEP_RDI_UPDATE_LOC_CLEAR_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_rx_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_2_MEP_RDI_UPDATE_RX_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      break;
  case 3:
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_PUNT_RATEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_PUNT_ENABLEf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->rx_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_RX_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->scan_state_update_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_SCAN_STATE_UPDATE_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_MEP_RDI_UPDATE_LOC_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_clear_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_MEP_RDI_UPDATE_LOC_CLEAR_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_rx_enable);
      soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_3_MEP_RDI_UPDATE_RX_ENf, field64);
      COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
      break;
  default:
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Internal error\n")));
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_OAMP_PUNT_EVENT_HENDLINGr(unit, reg));
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_punt_event_hendling_profile_set_unsafe()", profile_ndx, 0);
}

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PPC_OAM_OAMP_NUMBER_OF_PUNT_PROFILES(unit), ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_punt_event_hendling_profile_set_verify()", profile_ndx, 0);
}


uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_punt_event_hendling_profile_get_verify(
          unit,
		  profile_ndx						
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_punt_event_hendling_profile_get_unsafe(
          unit,
		  profile_ndx,
		  punt_profile_data	
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_punt_event_hendling_profile_get()", 0, 0);
}

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PPC_OAM_OAMP_NUMBER_OF_PUNT_PROFILES(unit), ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_punt_event_hendling_profile_get_verify()", profile_ndx, 0);
}

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint64 reg, field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  COMPILER_64_ZERO(reg);




  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_PUNT_EVENT_HENDLINGr(unit, &reg));
  switch (profile_ndx) {
  case 0:
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_PUNT_RATEf);
      punt_profile_data->punt_rate = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_PUNT_ENABLEf);
      punt_profile_data->punt_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_RX_STATE_UPDATE_ENf);
      punt_profile_data->rx_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_SCAN_STATE_UPDATE_ENf);
      punt_profile_data->scan_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_MEP_RDI_UPDATE_LOC_ENf);
      punt_profile_data->mep_rdi_update_loc_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_MEP_RDI_UPDATE_LOC_CLEAR_ENf);
      punt_profile_data->mep_rdi_update_loc_clear_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_0_MEP_RDI_UPDATE_RX_ENf);
      punt_profile_data->mep_rdi_update_rx_enable = COMPILER_64_LO(field64);
      break;
  case 1:
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_PUNT_RATEf);
      punt_profile_data->punt_rate = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_PUNT_ENABLEf);
      punt_profile_data->punt_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_RX_STATE_UPDATE_ENf);
      punt_profile_data->rx_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_SCAN_STATE_UPDATE_ENf);
      punt_profile_data->scan_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_MEP_RDI_UPDATE_LOC_ENf);
      punt_profile_data->mep_rdi_update_loc_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_MEP_RDI_UPDATE_LOC_CLEAR_ENf);
      punt_profile_data->mep_rdi_update_loc_clear_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_1_MEP_RDI_UPDATE_RX_ENf);
      punt_profile_data->mep_rdi_update_rx_enable = COMPILER_64_LO(field64);
      break;
  case 2:
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_PUNT_RATEf);
      punt_profile_data->punt_rate = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_PUNT_ENABLEf);
      punt_profile_data->punt_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_RX_STATE_UPDATE_ENf);
      punt_profile_data->rx_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_SCAN_STATE_UPDATE_ENf);
      punt_profile_data->scan_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_MEP_RDI_UPDATE_LOC_ENf);
      punt_profile_data->mep_rdi_update_loc_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_MEP_RDI_UPDATE_LOC_CLEAR_ENf);
      punt_profile_data->mep_rdi_update_loc_clear_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_2_MEP_RDI_UPDATE_RX_ENf);
      punt_profile_data->mep_rdi_update_rx_enable = COMPILER_64_LO(field64);
      break;
  case 3:
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_PUNT_RATEf);
      punt_profile_data->punt_rate = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_PUNT_ENABLEf);
      punt_profile_data->punt_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_RX_STATE_UPDATE_ENf);
      punt_profile_data->rx_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_SCAN_STATE_UPDATE_ENf);
      punt_profile_data->scan_state_update_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_MEP_RDI_UPDATE_LOC_ENf);
      punt_profile_data->mep_rdi_update_loc_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_MEP_RDI_UPDATE_LOC_CLEAR_ENf);
      punt_profile_data->mep_rdi_update_loc_clear_enable = COMPILER_64_LO(field64);
      field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_3_MEP_RDI_UPDATE_RX_ENf);
      punt_profile_data->mep_rdi_update_rx_enable = COMPILER_64_LO(field64);
      break;
  default:
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Internal error\n")));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_punt_event_hendling_profile_get_unsafe()", profile_ndx, 0);
}

 uint32 
     arad_pp_oam_oamp_read_pph_header(
     SOC_SAND_IN  int  unit,
     SOC_SAND_IN  uint8  field1,
     SOC_SAND_IN  uint8  field2,
     SOC_SAND_IN  uint8  field3,
     PPH_base *pph_header 
     )
{
    soc_field_t field6 = (SOC_IS_QAX_A0(unit) || SOC_IS_QUX(unit))  ? FIELD_6_7f: TX_PPH_IN_LIF_ORIENTATIONf;
    soc_field_t field7 = (SOC_IS_QAX_A0(unit) || SOC_IS_QUX(unit))  ? FIELD_11_11f: TX_PPH_UNKNOWN_DAf;
    soc_field_t field9 = (SOC_IS_QAX_A0(unit) || SOC_IS_QUX(unit))  ? FIELD_12_27f: TX_PPH_VSI_OR_VRFf;
    soc_field_t field10 = (SOC_IS_QAX_A0(unit) || SOC_IS_QUX(unit))  ? FIELD_28_45f: TX_PPH_IN_LIF_OR_IN_RIFf;
    soc_reg_t   reg = (SOC_IS_QAX_A0(unit) || SOC_IS_QUX(unit)) ? OAMP_REG_0130r : OAMP_TX_PPHr;
    soc_reg_above_64_val_t reg_data;
    uint32 res = SOC_SAND_OK;


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    res = soc_reg_above_64_get(unit, reg, SOC_CORE_DEFAULT, 0, reg_data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    pph_header->field1 =  field1; 
    pph_header->field2 =  field2;
    pph_header->field3 =  field3;
    pph_header->field4 =  0;
    pph_header->field5 =  0;
    pph_header->field6 =  soc_reg_above_64_field32_get(unit, reg, reg_data, field6);
    pph_header->field7 =  soc_reg_above_64_field32_get(unit, reg, reg_data, field7);
    pph_header->field8 =  0;
    pph_header->field9 =  soc_reg_above_64_field32_get(unit, reg, reg_data, field9);
    pph_header->field10 = soc_reg_above_64_field32_get(unit, reg, reg_data, field10);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_oamp_protection_packet_header_set_unsafe(
      SOC_SAND_IN  int                                    unit,
      SOC_SAND_IN  uint32                                 dest_system_port,
      SOC_SAND_IN  uint32                                 trap_id
  )
{
    uint32 res = SOC_SAND_OK, reg32=0;
    uint32 header_size =0;
    soc_reg_above_64_val_t      pkt_header, reg_data, reg_field;
    PPH_base pph_header = {0};
    soc_reg_above_64_val_t  data_above_64;
    int tc, dp, ssp;

    uint32 user_header_0_size, user_header_1_size,udh_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
  
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = arad_pmf_db_fes_user_header_sizes_get(
        unit,
        &user_header_0_size,
        &user_header_1_size,
        &user_header_egress_pmf_offset_0_dummy,
        &user_header_egress_pmf_offset_1_dummy
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    udh_size = user_header_1_size / 8 + user_header_0_size / 8; 
 

    SOC_REG_ABOVE_64_CLEAR(pkt_header);
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_CPUPORTr(unit, data_above_64));

    
    ssp = soc_reg_above_64_field32_get(unit,OAMP_CPUPORTr,data_above_64, CPU_PTCH_PP_SSPf);
    tc = soc_reg_above_64_field32_get(unit,OAMP_CPUPORTr,data_above_64, CP_UPORT_TCf);
    dp = soc_reg_above_64_field32_get(unit,OAMP_CPUPORTr,data_above_64, CP_UPORT_DPf);

    header_size += _insert_ptch2(pkt_header,1,_ARAD_PP_OAM_PTCH_OPAQUE_VALUE,ssp);
    header_size += _insert_itmh(unit, pkt_header, dest_system_port,dp,tc ,0,0, 1   );

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, arad_pp_oam_oamp_read_pph_header(unit, 1,_ARAD_PP_OAM_FWD_CODE_CPU,0,&pph_header));

    header_size += _insert_pph(pkt_header, &pph_header, header_size);

    header_size += _insert_fhei(pkt_header, 0, trap_id, header_size );

    if(udh_size) {
        header_size += _insert_udh(pkt_header, udh_size);
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2342, exit, WRITE_OAMP_PROTECTION_PACKET_HEADERr(unit,pkt_header));

    soc_reg_field_set(unit,OAMP_PROTECTION_PACKET_CONFr,&reg32, PROTECTION_PACKET_HEADER_NUM_OF_BYTESf, 120   );

    
    soc_reg_field_set(unit,OAMP_PROTECTION_PACKET_CONFr,&reg32, PROTECTION_PACKET_EDIT_PROGRAMf , _OAMP_PE_TCAM_KEY_PROTECTION_PACKET);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2342, exit, WRITE_OAMP_PROTECTION_PACKET_CONFr(unit,reg32));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_oamp_protection_packet_header_raw_set(
      SOC_SAND_IN  int                                    unit,
	  SOC_SAND_IN  SOC_PPC_OAM_OAMP_PROTECTION_HEADER     *packet_protection_header
  )
{
	uint32 res = SOC_SAND_OK, reg32=0;
	soc_reg_above_64_val_t      pkt_header;
    uint32 i,temp;


	SOC_SAND_INIT_ERROR_DEFINITIONS(0);

	SOC_REG_ABOVE_64_CLEAR(pkt_header);
	
    sal_memcpy(pkt_header,packet_protection_header->data,packet_protection_header->len);

    for (i=0 ; i<SOC_REG_ABOVE_64_MAX_SIZE_U32/2; i++) {
        temp= pkt_header[SOC_REG_ABOVE_64_MAX_SIZE_U32-(i+1)];
        pkt_header[SOC_REG_ABOVE_64_MAX_SIZE_U32-(i+1)]= pkt_header[i];
        pkt_header[i]= temp;
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2342, exit, WRITE_OAMP_PROTECTION_PACKET_HEADERr(unit,pkt_header));

    
    soc_reg_field_set(unit,OAMP_PROTECTION_PACKET_CONFr,&reg32, PROTECTION_PACKET_HEADER_NUM_OF_BYTESf, SOC_PPC_OAM_FULL_PROTECTION_HEADER_SIZE);

    
    soc_reg_field_set(unit,OAMP_PROTECTION_PACKET_CONFr,&reg32, PROTECTION_PACKET_EDIT_PROGRAMf , _OAMP_PE_TCAM_KEY_PROTECTION_PACKET);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2342, exit, WRITE_OAMP_PROTECTION_PACKET_CONFr(unit,reg32));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_oamp_protection_packet_header_raw_get(
      int                                    unit,
	  SOC_PPC_OAM_OAMP_PROTECTION_HEADER     *packet_protection_header
  )
{
	uint32 res = SOC_SAND_OK;
	soc_reg_above_64_val_t      pkt_header;
    uint32 i,temp;
 


	SOC_SAND_INIT_ERROR_DEFINITIONS(0);

	SOC_REG_ABOVE_64_CLEAR(pkt_header);
	
	SOC_SAND_SOC_IF_ERROR_RETURN(res, 2342, exit, READ_OAMP_PROTECTION_PACKET_HEADERr(unit,pkt_header));

    for (i=0 ; i<SOC_REG_ABOVE_64_MAX_SIZE_U32/2; i++) {
        temp= pkt_header[SOC_REG_ABOVE_64_MAX_SIZE_U32-(i+1)];
        pkt_header[SOC_REG_ABOVE_64_MAX_SIZE_U32-(i+1)]= pkt_header[i];
        pkt_header[i]= temp;
    }
	
	sal_memcpy(packet_protection_header->data,pkt_header,80);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_IN  uint32                                 trap_id,
    SOC_SAND_IN  SOC_TMC_DEST_INFO                         dest_info
  )
{
  soc_reg_above_64_val_t  data_above_64, dest_above_64, trap_above_64;
  uint32
    res = SOC_SAND_OK;
  uint32 destination;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_REG_ABOVE_64_CLEAR(data_above_64);
  SOC_REG_ABOVE_64_CLEAR(dest_above_64);
  SOC_REG_ABOVE_64_CLEAR(trap_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_CPUPORTr(unit, data_above_64));

  res = arad_hpu_itmh_fwd_dest_info_build(unit, dest_info, soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, 0), FALSE, &destination);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  dest_above_64[0] = destination;
  trap_above_64[0] = trap_id;

  switch (trap_type) {
  case SOC_PPC_OAM_OAMP_TRAP_ERR:
      if (SOC_IS_QAX(unit))
      {
          soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TRAP_CODE_MISSf, dest_above_64);
          soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TRAP_CODE_MISSf, trap_above_64);
      }
      else
      {
          soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TRAP_MISSf, dest_above_64);
          soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TRAP_MISSf, trap_above_64);
      }
      break;
  case SOC_PPC_OAM_OAMP_TYPE_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TYPE_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TYPE_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_RMEP_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_EMC_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_EMC_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_MAID_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_MAID_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_MAID_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_MDL_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_MDL_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_MDL_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_CCM_INTERVAL_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_CCM_INTRVL_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_CCM_INTRVL_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_MY_DISC_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_ERR_MY_DISC_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_ERR_MY_DISC_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_SRC_IP_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_ERR_SRC_IP_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_ERR_SRC_IP_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_YOUR_DISC_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_ERR_YOUR_DISC_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_ERR_YOUR_DISC_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_SRC_PORT_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_SRC_PORT_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_SRC_PORT_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_RMEP_STATE_CHANGE:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_RMEP_STATE_CHANGEf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_RMEP_STATE_CHANGEf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_PARITY_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_PARITY_ERRORf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_PARITY_ERRORf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_TIMESTAMP_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TIMESTAMP_MISSf, dest_above_64);     
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TIMESTAMP_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_PROTECTION:
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, arad_pp_oam_oamp_protection_packet_header_set_unsafe(unit, destination ,trap_id));
      break;  
  case SOC_PPC_OAM_OAMP_CHANNEL_TYPE_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_CHAN_TYPE_MISSf, dest_above_64);
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_CHAN_TYPE_MISSf, trap_above_64);
      break;
  case SOC_PPC_OAM_OAMP_FLEX_CRC_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_FLEX_CRC_MISSf, dest_above_64);
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_FLEX_CRC_MISSf, trap_above_64);
      break;
  case SOC_PPC_OAM_OAMP_RFC_PUNT_ERR:
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_RFC_PUNTf, dest_above_64);
      soc_reg_above_64_field_set(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_RFC_PUNTf, trap_above_64);
      break;
  default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
  }
    
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, WRITE_OAMP_CPUPORTr(unit, data_above_64));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_error_trap_id_and_destination_set_unsafe()", trap_type, 0);
}

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_IN  uint32                                 trap_id,
    SOC_SAND_IN  SOC_TMC_DEST_INFO                        dest_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" Error: Unsupported device\n")));
  } 
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_id, 255, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_type, SOC_PPC_OAM_OAMP_TRAP_TYPE_COUNT, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_error_trap_id_and_destination_set_verify()", trap_type, 0);
}

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_OUT  uint32                                *trap_id,
    SOC_SAND_OUT  SOC_TMC_DEST_INFO                        *dest_info
  )
{
  soc_reg_above_64_val_t  data_above_64, dest_above_64, trap_above_64;
  uint32
    res = SOC_SAND_OK;
  int32  dest;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_REG_ABOVE_64_CLEAR(data_above_64);
  SOC_REG_ABOVE_64_CLEAR(dest_above_64);
  SOC_REG_ABOVE_64_CLEAR(trap_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_CPUPORTr(unit, data_above_64));

  switch (trap_type) {
  case SOC_PPC_OAM_OAMP_TRAP_ERR:
      if (SOC_IS_QAX(unit))
      {
          soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TRAP_CODE_MISSf, dest_above_64);
          soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TRAP_CODE_MISSf, trap_above_64);
      }
      else
      {
          soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TRAP_MISSf, dest_above_64);
          soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TRAP_MISSf, trap_above_64);
      }
      break;  
  case SOC_PPC_OAM_OAMP_TYPE_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TYPE_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TYPE_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_RMEP_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_EMC_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_EMC_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_MAID_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_MAID_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_MAID_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_MDL_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_MDL_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_MDL_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_CCM_INTERVAL_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_CCM_INTRVL_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_CCM_INTRVL_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_MY_DISC_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_ERR_MY_DISC_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_ERR_MY_DISC_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_SRC_IP_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_ERR_SRC_IP_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_ERR_SRC_IP_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_YOUR_DISC_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_ERR_YOUR_DISC_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_ERR_YOUR_DISC_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_SRC_PORT_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_SRC_PORT_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_SRC_PORT_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_RMEP_STATE_CHANGE:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_RMEP_STATE_CHANGEf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_RMEP_STATE_CHANGEf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_PARITY_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_PARITY_ERRORf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_PARITY_ERRORf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_TIMESTAMP_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_TIMESTAMP_MISSf, dest_above_64);     
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_TIMESTAMP_MISSf, trap_above_64);   
      break;  
  case SOC_PPC_OAM_OAMP_CHANNEL_TYPE_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_CHAN_TYPE_MISSf, dest_above_64);
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_CHAN_TYPE_MISSf, trap_above_64);
      break;
  case SOC_PPC_OAM_OAMP_FLEX_CRC_ERR:
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_SYS_PORT_FLEX_CRC_MISSf, dest_above_64);
      soc_reg_above_64_field_get(unit, OAMP_CPUPORTr, data_above_64, CPU_TRAP_CODE_FLEX_CRC_MISSf, trap_above_64);
      break;
  default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
  }   

  *trap_id = trap_above_64[0];
  dest = dest_above_64[0];

  res = arad_hpu_itmh_fwd_dest_info_parse(unit, dest,  soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, 0), FALSE, dest_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_error_trap_id_and_destination_get_unsafe()", trap_type, 0);
}

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" Error: Unsupported device\n")));
  } 
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_type, SOC_PPC_OAM_OAMP_TRAP_TYPE_COUNT, ARAD_PP_OAM_INTERNAL_ERROR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_error_trap_id_and_destination_get_verify()", trap_type, 0);
}







uint32 
    arad_pp_oam_move_entry_in_oamp_mep_db_internal(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 src,
    SOC_SAND_IN  uint32                                 dst
       )
{
    soc_reg_above_64_val_t entry;
    soc_reg_above_64_val_t entry_above_64_get;
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_REG_ABOVE_64_CLEAR(entry);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,src, entry));
    do
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,dst, entry));

        
        SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
        SOC_SAND_SOC_IF_ERROR_RETURN(res,32,exit,_ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit,MEM_BLOCK_ANY, dst, entry_above_64_get));

    } while (sal_memcmp(entry, entry_above_64_get, (soc_mem_entry_bytes(unit,OAMP_MEP_DBm))));


    CLEAR_OAMP_MEP_DB_ENTRY(src, entry);



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
   arad_pp_reset_oamp_mep_db_entry_internal(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  uint32                             entry_id
      )
{
    uint32 res = SOC_SAND_OK;
    uint32 mep_type, mep_type_get;
    soc_reg_above_64_val_t reg_data;
    soc_reg_above_64_val_t entry_above_64_get;
  
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, entry_id,reg_data);
    
    soc_OAMP_MEP_DB_DM_STATm_field32_set(unit, reg_data, MEP_TYPEf, mep_type);

    do
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,entry_id, reg_data));

        
        SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,entry_id, entry_above_64_get));
        soc_mem_field_get(unit, OAMP_MEP_DBm, entry_above_64_get, MEP_TYPEf, &mep_type_get);

    } while (mep_type != mep_type_get);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
    arad_pp_oam_oamp_write_to_nic_and_oui_tables_internal(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  uint32                             lsb_to_nic,
      SOC_SAND_IN  uint32                             msb_to_oui,
      SOC_SAND_IN  int                             nic_profile,
      SOC_SAND_IN  int                             oui_profile
       )
{
    uint32 res = SOC_SAND_OK;
    uint32 reg32_data, reg32_field;
    soc_reg_t oui_fields[]  = {LMM_DA_OUI_0f,LMM_DA_OUI_1f,LMM_DA_OUI_2f,LMM_DA_OUI_3f,LMM_DA_OUI_4f,LMM_DA_OUI_5f,LMM_DA_OUI_6f,LMM_DA_OUI_7f};
    soc_reg_above_64_val_t reg_data, reg_field;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_LMM_DA_OUIr(unit, reg_data));
    reg_field[0] = msb_to_oui;
    soc_reg_above_64_field_set(unit, OAMP_LMM_DA_OUIr, reg_data, oui_fields[oui_profile], reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_LMM_DA_OUIr(unit, reg_data));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_LMM_DA_NIC_TABLEm(unit, MEM_BLOCK_ANY, nic_profile, &reg32_data));
    reg32_field = lsb_to_nic;
    soc_OAMP_LMM_DA_NIC_TABLEm_field_set(unit, &reg32_data, LMM_DA_NICf, &reg32_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_LMM_DA_NIC_TABLEm(unit, MEM_BLOCK_ALL, nic_profile, &reg32_data)); 


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
    arad_pp_oam_oamp_write_eth1731_profile_internal(
       SOC_SAND_IN  int                                              unit,
       SOC_SAND_IN  uint8                             profile_indx,
       SOC_SAND_IN SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY      *eth1731_profile
       )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data, reg_field;
    soc_reg_t eth_1731_field;
    soc_reg_t offset_field; 
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 


    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res,32,exit,READ_OAMP_ETH_1731_MEP_PROFILEr(unit, reg_data));

    reg_field[0] = eth1731_profile->lmm_rate;
    GET_OAMP_ETH_1731_FIELD_BY_INDEX_LMM_RATE(eth_1731_field, profile_indx);
    soc_reg_above_64_field_set(unit, OAMP_ETH_1731_MEP_PROFILEr, reg_data, eth_1731_field, reg_field);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    reg_field[0] = eth1731_profile->dmm_rate;
    GET_OAMP_ETH_1731_FIELD_BY_INDEX_DMM_RATE(eth_1731_field,profile_indx);
    soc_reg_above_64_field_set(unit, OAMP_ETH_1731_MEP_PROFILEr, reg_data, eth_1731_field, reg_field);


    SOC_REG_ABOVE_64_CLEAR(reg_field);
    reg_field[0] = eth1731_profile->piggy_back_lm;
    GET_OAMP_ETH_1731_FIELD_BY_INDEX_PIGGY_BACK(eth_1731_field, profile_indx);
    soc_reg_above_64_field_set(unit, OAMP_ETH_1731_MEP_PROFILEr, reg_data, eth_1731_field, reg_field);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    reg_field[0] = eth1731_profile->lmm_da_oui_prof;
    GET_OAMP_ETH_1731_FIELD_BY_INDEX_DA_OUI_PROF(eth_1731_field, profile_indx);
    soc_reg_above_64_field_set(unit, OAMP_ETH_1731_MEP_PROFILEr, reg_data, eth_1731_field, reg_field);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    reg_field[0] = eth1731_profile->rdi_gen_method;
    GET_OAMP_ETH_1731_FIELD_BY_INDEX_RDI_GEN_METHOD(eth_1731_field, profile_indx);
    soc_reg_above_64_field_set(unit, OAMP_ETH_1731_MEP_PROFILEr, reg_data, eth_1731_field, reg_field);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, WRITE_OAMP_ETH_1731_MEP_PROFILEr(unit, reg_data)); 

    
    SOC_REG_ABOVE_64_CLEAR(reg_data); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res,32,exit,READ_OAMP_OAM_TS_OFFSET_PROFILEr(unit, reg_data));

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_DMM_OFFSET(offset_field, profile_indx);
    reg_field[0] = eth1731_profile -> dmm_offset;
    LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          "dmm offset is gonna be %d\n"),reg_field[0] ));
    soc_reg_above_64_field_set(unit, OAMP_OAM_TS_OFFSET_PROFILEr, reg_data, offset_field, reg_field); 

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_DMR_OFFSET(offset_field, profile_indx);
    reg_field[0] = eth1731_profile -> dmr_offset;
    LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          "dmr offset is gonna be %d\n"),reg_field[0] ));
    soc_reg_above_64_field_set(unit, OAMP_OAM_TS_OFFSET_PROFILEr, reg_data, offset_field, reg_field); 

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_LMM_OFFSET(offset_field, profile_indx);
    reg_field[0] = eth1731_profile->lmm_offset;
    LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          "lmm offset is gonna be %d\n"),reg_field[0] ));
    soc_reg_above_64_field_set(unit, OAMP_OAM_TS_OFFSET_PROFILEr, reg_data, offset_field, reg_field); 

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    GET_OAMP_OAM_TS_OFFSET_PROFILE_FIELD_BY_INDEX_LMR_OFFSET(offset_field, profile_indx);
    reg_field[0] = eth1731_profile->lmr_offset;
    LOG_DEBUG(BSL_LS_SOC_OAM,
              (BSL_META_U(unit,
                          "lmr offset is gonna be %d\n"),reg_field[0] ));
    soc_reg_above_64_field_set(unit, OAMP_OAM_TS_OFFSET_PROFILEr, reg_data, offset_field, reg_field); 

    SOC_SAND_SOC_IF_ERROR_RETURN(res,32,exit,WRITE_OAMP_OAM_TS_OFFSET_PROFILEr(unit, reg_data));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_oamp_slm_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *mep_db_entry
  )
{
    soc_reg_above_64_val_t reg_data, reg_field;
    soc_reg_above_64_val_t entry_above_64_get;
    uint32     res = SOC_SAND_OK;
    uint32     reg32_field, reg32_field_get;
    soc_mem_t  mem = OAMP_MEP_DBm ;
    uint32     is_upmep = 0;
    uint32     mep_pe_profile = 0;
    uint32     mep_pe_profile_offset = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_oam_oamp_slm_set_verify(unit, mep_db_entry );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);

    res = get_ccm_entry(unit, mep_db_entry->mep_id, &reg_data, &mem);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

    if (mem == OAMP_MEP_DBm) {
        soc_mem_field_get(unit, mem, reg_data, UP_1_DOWN_0f, &is_upmep);
    }

    
    soc_mem_field_get(unit, mem, reg_data, MEP_PE_PROFILEf, &mep_pe_profile);

    if ((mep_db_entry->is_slm) && (!is_upmep)){
        arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_EGR_INJ, &mep_pe_profile_offset);
        if (mep_pe_profile_offset == -1) {
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("ARAD_PP_OAMP_PE_PROGS_EGR_INJ program not loaded.")));
        }

        
        if(mep_pe_profile < mep_pe_profile_offset) {
            
            reg32_field = mep_pe_profile | (1 << ARAD_PP_OAMP_MEP_PE_PROFILE_SLM_OFFSET); 
        } else {
            
            reg32_field = mep_pe_profile | (1 << ARAD_PP_OAMP_MEP_PE_PROFILE_COUNT_SLM_OFFSET); 
        }

       soc_mem_field_set(unit, mem, reg_data, MEP_PE_PROFILEf, &reg32_field);
       do
       {
           SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ALL, mep_db_entry->mep_id, reg_data));

           
           SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
           SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ALL, mep_db_entry->mep_id, entry_above_64_get));
           soc_mem_field_get(unit, mem, entry_above_64_get, MEP_PE_PROFILEf, &reg32_field_get);

       } while (reg32_field != reg32_field_get);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_oamp_lm_dm_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *mep_db_entry
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data, reg_field;
    soc_reg_above_64_val_t entry_above_64_get;
    uint32 reg32_field, reg32_field_get;
    soc_mem_t mem = OAMP_MEP_DBm ;
    SOC_PPC_OAM_MEP_TYPE base_mep_type;
    uint32 user_header_size, user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    uint32 is_upmep = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);

    res = arad_pmf_db_fes_user_header_sizes_get(unit,
                                                &user_header_0_size,
                                                &user_header_1_size,
                                                &user_header_egress_pmf_offset_0_dummy,
                                                &user_header_egress_pmf_offset_1_dummy);

    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    user_header_size = (user_header_0_size + user_header_1_size) / 8;


    
    if (mep_db_entry->is_1DM) {
        res = get_ccm_entry(unit, mep_db_entry->mep_id, &reg_data, &mem);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

        if (mem == OAMP_MEP_DBm) {
            soc_mem_field_get(unit, mem, reg_data, UP_1_DOWN_0f, &is_upmep);
        }

        
        if (SOC_IS_JERICHO(unit) && user_header_size && !is_upmep) {
            arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_1DM_DOWN, &reg32_field);
        } else {
            arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_1DM, &reg32_field);
        }

        if (reg32_field == -1) {
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("1DM OAMP PE program not loaded.")));
        }

        soc_mem_field_set(unit, mem, reg_data, MEP_PE_PROFILEf, &reg32_field);
        do
       {
           SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ALL, mep_db_entry->mep_id, reg_data));

           
           SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
           SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ALL, mep_db_entry->mep_id, entry_above_64_get));
           soc_mem_field_get(unit, mem, entry_above_64_get, MEP_PE_PROFILEf, &reg32_field_get);

       } while (reg32_field != reg32_field_get);
        SOC_SAND_SOC_IF_ERROR_RETURN(res,32,exit,_ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit,mem,MEM_BLOCK_ALL,mep_db_entry->mep_id,reg_data));
        
        if (SOC_IS_QAX(unit)) {
            ARAD_PP_DO_NOTHING_AND_EXIT;
        }
    } else {
        if (mep_db_entry->is_piggyback_down && SOC_IS_ARADPLUS_A0(unit) && mem==OAMP_MEP_DBm) {
            
            
            arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP,&reg32_field);
            if (reg32_field==-1) {
                
                reg32_field = 0;
            }
            res = get_ccm_entry(unit, mep_db_entry->mep_id, &reg_data, &mem);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);
            soc_mem_field_set(unit, mem, reg_data, MEP_PE_PROFILEf, &reg32_field);
            do
            {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ALL, mep_db_entry->mep_id, reg_data));

                
                SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ALL, mep_db_entry->mep_id, entry_above_64_get));
                soc_mem_field_get(unit, mem, entry_above_64_get, MEP_PE_PROFILEf, &reg32_field_get);
            } while (reg32_field != reg32_field_get);
        }
        
        if(mep_db_entry->is_jumbo_dm){
            res = get_ccm_entry(unit, mep_db_entry->mep_id, &reg_data, &mem);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);
            if(mep_db_entry->name_type == _BCM_OAM_MA_NAME_TYPE_STRING_11_BYTES){
                arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_JUMBO_DM_TLV_11B_MAID,&reg32_field);
            }
            else if(mep_db_entry->name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE){
                arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_JUMBO_DM_TLV_48B_MAID,&reg32_field);
            }
            else {
                
                arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_JUMBO_DM_TLV,&reg32_field);
            }
            if (reg32_field==-1) {
                 SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Jumbo TLV OAMP PE program not loaded.")));
            }
            soc_mem_field_set(unit, mem, reg_data, MEP_PE_PROFILEf, &reg32_field);
            do
            {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ALL, mep_db_entry->mep_id, reg_data));
                
                SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ALL, mep_db_entry->mep_id, entry_above_64_get));
                soc_mem_field_get(unit, mem, entry_above_64_get, MEP_PE_PROFILEf, &reg32_field_get);
            } while (reg32_field != reg32_field_get);
        }
    }

    if (mep_db_entry->is_update==0) {
        
        
        GET_TYPE_OF_OAMP_MEP_DB_ENTRY(base_mep_type, mep_db_entry->mep_id +1 ,reg_data);
        if (base_mep_type== SOC_PPC_OAM_MEP_TYPE_DM) { 
            int offset = (mep_db_entry->entry_type == SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM_STAT)? 2:1;
            SOC_REG_ABOVE_64_CLEAR(reg_data); 
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit, 
                                         arad_pp_oam_move_entry_in_oamp_mep_db_internal(unit,mep_db_entry->mep_id +1,mep_db_entry->mep_id +1 + offset));
        }

        SOC_REG_ABOVE_64_CLEAR(reg_data); 
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        if (mep_db_entry->entry_type == SOC_PPC_OAM_LM_DM_ENTRY_TYPE_DM) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit, READ_OAMP_MEP_DB_DM_STATm(unit,MEM_BLOCK_ANY, mep_db_entry->allocated_id  , reg_data)); 
            reg_field[0] = SOC_PPC_OAM_MEP_TYPE_DM;
            soc_OAMP_MEP_DB_DM_STATm_field_set(unit,reg_data, MEP_TYPEf, reg_field);
            reg_field[0] =0xffffffff;
            reg_field[1] = 0x3ff; 
            soc_OAMP_MEP_DB_DM_STATm_field_set(unit,reg_data, MIN_DELAYf, reg_field);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit, WRITE_OAMP_MEP_DB_DM_STATm(unit,MEM_BLOCK_ALL, mep_db_entry->allocated_id , reg_data));
        }  else {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_LM_DBm(unit, MEM_BLOCK_ANY, mep_db_entry->mep_id + 1, reg_data)); 
            reg_field[0] = SOC_PPC_OAM_MEP_TYPE_LM; 
            soc_OAMP_MEP_DB_LM_DBm_field_set(unit,reg_data, MEP_TYPEf, reg_field);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_LM_DBm(unit, MEM_BLOCK_ALL, mep_db_entry->mep_id + 1, reg_data));
            if (mep_db_entry->entry_type == SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM_STAT) { 
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_LM_STATm(unit, MEM_BLOCK_ANY, mep_db_entry->mep_id + 2, reg_data));
                reg_field[0] = SOC_PPC_OAM_MEP_TYPE_LM_STAT;
                soc_OAMP_MEP_DB_LM_DBm_field_set(unit, reg_data, MEP_TYPEf, reg_field);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_LM_STATm(unit, MEM_BLOCK_ALL, mep_db_entry->mep_id + 2, reg_data));
            }
        }
    } else if (mep_db_entry->entry_type != SOC_PPC_OAM_LM_DM_ENTRY_TYPE_DM) { 
        
        GET_TYPE_OF_OAMP_MEP_DB_ENTRY(base_mep_type, mep_db_entry->mep_id + 2, reg_data); 
        if (base_mep_type == SOC_PPC_OAM_MEP_TYPE_LM_STAT && mep_db_entry->entry_type == SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM) {
            
            GET_TYPE_OF_OAMP_MEP_DB_ENTRY(base_mep_type, mep_db_entry->mep_id + 3, reg_data);
            if (base_mep_type == SOC_PPC_OAM_MEP_TYPE_DM) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 43, exit,
                                             arad_pp_oam_move_entry_in_oamp_mep_db_internal(unit, mep_db_entry->mep_id + 3, mep_db_entry->mep_id + 2));
            } else {
                CLEAR_OAMP_MEP_DB_ENTRY(mep_db_entry->mep_id + 2, reg_data);
            }
        } else if (base_mep_type != SOC_PPC_OAM_MEP_TYPE_LM_STAT && mep_db_entry->entry_type == SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM_STAT) {
            
            if (base_mep_type == SOC_PPC_OAM_MEP_TYPE_DM) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 43, exit,
                                             arad_pp_oam_move_entry_in_oamp_mep_db_internal(unit, mep_db_entry->mep_id + 2, mep_db_entry->mep_id + 3));
            }
            SOC_REG_ABOVE_64_CLEAR(reg_data);
            reg_field[0] = SOC_PPC_OAM_MEP_TYPE_LM_STAT;
            soc_OAMP_MEP_DB_LM_STATm_field_set(unit, reg_data, MEP_TYPEf, reg_field);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_LM_STATm(unit, MEM_BLOCK_ALL, mep_db_entry->mep_id + 2, reg_data));
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_oamp_slm_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY     *mep_db_entry
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);
    SOC_SAND_MAGIC_NUM_VERIFY(mep_db_entry);


    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }


    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_oamp_lm_dm_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY     *mep_db_entry
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(mep_db_entry);
    SOC_SAND_MAGIC_NUM_VERIFY(mep_db_entry);


    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }


    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_oamp_lm_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_LM_INFO_GET     *lm_info
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(lm_info);
    SOC_SAND_MAGIC_NUM_VERIFY(lm_info);
    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


soc_error_t
  arad_pp_oam_oamp_lm_get(
    int                                 unit,
    SOC_PPC_OAM_OAMP_LM_INFO_GET        *lm_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_lm_get_verify(unit, lm_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_lm_get_unsafe(unit, lm_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}
    

uint32
  arad_pp_oam_oamp_lm_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_LM_INFO_GET     *lm_info
  )
{
    soc_reg_above_64_val_t reg_data;
    uint32 res = SOC_SAND_OK;
    uint32 type;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 12, exit,soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_MEP_DB_LM_DBm, 0, MEM_BLOCK_ANY, lm_info->entry_id +1, reg_data));

    type = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data, MEP_TYPEf) ;

    if ( type != SOC_PPC_OAM_MEP_TYPE_LM ) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("No LM entry for endpoint %d"), lm_info->entry_id));
    }

    lm_info->my_tx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data,MY_TXf);
    lm_info->my_rx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data,MY_RXf);
    lm_info->peer_tx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data,PEER_TXf);
    lm_info->peer_rx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data,PEER_RXf);


    

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 12, exit, soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_MEP_DB_LM_STATm, 0, MEM_BLOCK_ANY, lm_info->entry_id + 2, reg_data));

    if (soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data, MEP_TYPEf) == SOC_PPC_OAM_MEP_TYPE_LM_STAT) {
        lm_info->is_extended = 1;
        lm_info->last_lm_near = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,LAST_LM_NEARf);
        lm_info->last_lm_far = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,LAST_LM_FARf);
        lm_info->acc_lm_near = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,ACC_LM_NEARf);
        lm_info->acc_lm_far = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,ACC_LM_FARf);
        lm_info->max_lm_near = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,MAX_LM_NEARf);
        lm_info->max_lm_far = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,MAX_LM_FARf);
        
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit,reg_data, MAX_LM_NEARf , 0);                
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit,reg_data, MAX_LM_FARf , 0);                
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit,reg_data, ACC_LM_NEARf , 0);                
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit,reg_data, ACC_LM_FARf , 0);        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 12, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_LM_STATm(unit, MEM_BLOCK_ALL, lm_info->entry_id + 2, reg_data));

    } else {
        lm_info->is_extended =0;
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

soc_error_t
  arad_pp_oam_oamp_dm_get(
    int                                 unit,
    SOC_PPC_OAM_OAMP_DM_INFO_GET     *dm_info,
    uint8                                      * is_1dm
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_dm_get_verify(unit, dm_info, is_1dm);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  
  res = arad_pp_oam_oamp_dm_get_unsafe(unit, dm_info, is_1dm);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_oamp_dm_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_DM_INFO_GET     *dm_info,
    SOC_SAND_OUT uint8                                      * is_1dm
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(dm_info);

    SOC_SAND_MAGIC_NUM_VERIFY(dm_info);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_oamp_dm_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_DM_INFO_GET     *dm_info,
    SOC_SAND_OUT uint8                                      * is_1dm
  )
{
    soc_reg_above_64_val_t reg_data, reg_field;
    uint32 offset, type, oneDM_program;
    uint32 res = SOC_SAND_OK;
    uint32 user_header_size, user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    uint32 is_upmep = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);

    res = arad_pmf_db_fes_user_header_sizes_get(unit,
                                                &user_header_0_size,
                                                &user_header_1_size,
                                                &user_header_egress_pmf_offset_0_dummy,
                                                &user_header_egress_pmf_offset_1_dummy);

    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    user_header_size = (user_header_0_size + user_header_1_size) / 8;

    
    
    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(type, dm_info->entry_id + 1, reg_data);
    if (type == SOC_PPC_OAM_MEP_TYPE_DM) {
        offset = 1;
    } else if (type == SOC_PPC_OAM_MEP_TYPE_LM)  { 
            GET_TYPE_OF_OAMP_MEP_DB_ENTRY(type, dm_info->entry_id + 2, reg_data);
            if (type == SOC_PPC_OAM_MEP_TYPE_DM) {
                offset = 2;
            } else if  (type == SOC_PPC_OAM_MEP_TYPE_LM_STAT) {  
                GET_TYPE_OF_OAMP_MEP_DB_ENTRY(type, dm_info->entry_id + 3, reg_data);
                if (type == SOC_PPC_OAM_MEP_TYPE_DM) {
                    offset = 3;
                } else { 
                    SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("No DM entry for endpoint %d "),dm_info->entry_id)); 
                    }
            } else {
                SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("No DM entry for endpoint %d "),dm_info->entry_id));
            }
    } else {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("No DM entry for endpoint %d "),dm_info->entry_id));
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 12, exit, soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_MEP_DB_DM_STATm, 0, MEM_BLOCK_ANY, dm_info->entry_id + offset, reg_data));  

    if (soc_OAMP_MEP_DB_DM_STATm_field32_get(unit, reg_data, MEP_TYPEf) != SOC_PPC_OAM_MEP_TYPE_DM) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Internal error: profile index incorrect for DM ")));
    }

    soc_OAMP_MEP_DB_DM_STATm_field_get(unit,reg_data,LAST_DELAYf,reg_field);
    SHR_BITCOPY_RANGE( &(dm_info->last_delay_sub_seconds),0,reg_field,0,30);
    SHR_BITCOPY_RANGE( &(dm_info->last_delay_second),0,reg_field,30,12);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    soc_OAMP_MEP_DB_DM_STATm_field_get(unit,reg_data,MAX_DELAYf,reg_field);
    SHR_BITCOPY_RANGE( &(dm_info->max_delay_sub_seconds),0,reg_field,0,30);
    SHR_BITCOPY_RANGE( &(dm_info->max_delay_second),0,reg_field,30,12);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    soc_OAMP_MEP_DB_DM_STATm_field_get(unit,reg_data,MIN_DELAYf,reg_field);
    SHR_BITCOPY_RANGE( &(dm_info->min_delay_sub_seconds),0,reg_field,0,30);
    SHR_BITCOPY_RANGE( &(dm_info->min_delay_second),0,reg_field,30,12);

    SOC_REG_ABOVE_64_CLEAR(reg_field); 
    soc_OAMP_MEP_DB_DM_STATm_field_set(unit,reg_data, MAX_DELAYf, reg_field );
    reg_field[0] =0xffffffff;
    reg_field[1] = 0x3ff; 
    soc_OAMP_MEP_DB_DM_STATm_field_set(unit,reg_data, MIN_DELAYf, reg_field );

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit, WRITE_OAMP_MEP_DB_DM_STATm(unit,MEM_BLOCK_ALL, dm_info->entry_id + offset , reg_data));

    
    dm_info->last_delay_sub_seconds <<=2;
    dm_info->max_delay_sub_seconds <<=2;
    dm_info->min_delay_sub_seconds <<=2;

    
    res = get_ccm_entry(unit, dm_info->entry_id, &reg_data, (soc_mem_t *)&type);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);
    if (type == OAMP_MEP_DBm) {
        soc_mem_field_get(unit, type, reg_data, UP_1_DOWN_0f, &is_upmep);
    }
    
    if (SOC_IS_JERICHO(unit) && user_header_size && !is_upmep) {
        arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_1DM_DOWN, &oneDM_program);
    } else {
        arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_1DM, &oneDM_program);
    }

    *is_1dm = (soc_mem_field32_get(unit, type, reg_data, MEP_PE_PROFILEf) == oneDM_program);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}




uint32
  arad_pp_oam_oamp_next_index_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *next_index,
    SOC_SAND_OUT    uint8                              *has_dm
  )
{
  uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(next_index);
    SOC_SAND_CHECK_NULL_INPUT(has_dm);
    
    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }


    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_oamp_next_index_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *next_index,
    SOC_SAND_OUT    uint8                              *has_dm
  )
{
        soc_reg_above_64_val_t reg_data;
    uint32 res = SOC_SAND_OK;
    uint32  mep_type, next_type;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, endpoint_id+1, reg_data);

    if (mep_type == 0) {
        *next_index = endpoint_id+1; 
        *has_dm = 0;
    } else if (mep_type == SOC_PPC_OAM_MEP_TYPE_DM) {
        *next_index =endpoint_id+2;  
        *has_dm = 1;
    } else if (mep_type == SOC_PPC_OAM_MEP_TYPE_LM) {
        GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, endpoint_id+2, reg_data);
        if (mep_type ==SOC_PPC_OAM_MEP_TYPE_LM_STAT ) {
            GET_TYPE_OF_OAMP_MEP_DB_ENTRY(next_type, endpoint_id+3, reg_data);
            if ( next_type ==SOC_PPC_OAM_MEP_TYPE_DM ) {
              *next_index =endpoint_id+4; 
              *has_dm = 1;
            } else {
              *next_index =endpoint_id+3; 
              *has_dm = 0;
            }
        } else {
            if (mep_type == SOC_PPC_OAM_MEP_TYPE_DM) {
                *next_index = endpoint_id + 3;
                *has_dm = 1;
            } else {
                *next_index = endpoint_id + 2;
                *has_dm = 0;
            }
        }
    } else {
        *next_index = 0; 
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}




uint32
  arad_pp_oam_oamp_eth1731_and_oui_profiles_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *eth1731_prof,
    SOC_SAND_OUT uint32                               *da_oui_prof
  )
{
      uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(eth1731_prof);
    SOC_SAND_CHECK_NULL_INPUT(da_oui_prof);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_oamp_eth1731_and_oui_profiles_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *eth1731_prof,
    SOC_SAND_OUT uint32                               *da_oui_prof
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data, reg_field;
    int nic_profile;
    soc_reg_t lmm_da_oui_prof_field;
    uint32 mep_type;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);

    COMPILER_REFERENCE(nic_profile);
    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, endpoint_id, reg_data);
    res = get_mep_and_da_nic_profiles_from_oamp_mep_db_entry(unit, eth1731_prof, &nic_profile, mep_type, endpoint_id, &reg_data);
    SOC_SAND_SOC_IF_ERROR_RETURN(res,2321,exit,res);

    if (mep_type==SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
        
        if (SOC_IS_JERICHO(unit)) {
            SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY eth_1731_prof_entry={0};
            res = soc_jer_pp_oam_oamp_eth1731_profile_get(unit,*eth1731_prof,&eth_1731_prof_entry );
            SOC_SAND_SOC_IF_ERROR_RETURN(res,2322,exit,res);
            *da_oui_prof = eth_1731_prof_entry.lmm_da_oui_prof;
        } else {
            SOC_REG_ABOVE_64_CLEAR(reg_data); 
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_ETH_1731_MEP_PROFILEr(unit, reg_data)); 
            GET_OAMP_ETH_1731_FIELD_BY_INDEX_DA_OUI_PROF(lmm_da_oui_prof_field, *eth1731_prof);
            soc_reg_above_64_field_get(unit, OAMP_ETH_1731_MEP_PROFILEr, reg_data, lmm_da_oui_prof_field, reg_field);
            *da_oui_prof = reg_field[0];
        }
    }
        
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}





uint32
    arad_pp_oam_oamp_nic_profile_get_verify(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               *da_nic_prof
       )
{
      uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(da_nic_prof);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
    arad_pp_oam_oamp_nic_profile_get_unsafe(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               *da_nic_prof
       )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(da_nic_prof);


    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,endpoint_id, reg_data));

    *da_nic_prof = soc_OAMP_MEP_DBm_field32_get(unit, reg_data, LMM_DA_NIC_PROFILEf);


    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}





soc_error_t
    arad_pp_oam_oamp_search_for_lm_dm(
       int                                 unit,
       uint32                                 endpoint_id,
       uint32                               * found_bitmap
       )
{
    soc_error_t
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;


    res = arad_pp_oam_oamp_search_for_lm_dm_verify(unit, endpoint_id,found_bitmap);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_oamp_search_for_lm_dm_unsafe(unit, endpoint_id,found_bitmap);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);


  exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

soc_error_t
    arad_pp_oam_oamp_search_for_lm_dm_verify(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               * found_bitmap
       )
{
      uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(found_bitmap);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



soc_error_t
    arad_pp_oam_oamp_search_for_lm_dm_unsafe(
       int                                    unit,
       uint32                                 endpoint_id,
       uint32                               * found_bitmap
       )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data;
    uint32  mep_type;
    uint32 cur = endpoint_id + 1; 
    uint32 found_bitmap_lcl[1];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found_bitmap_lcl=0; 

   
    if (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit) == cur) {
        
        GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, endpoint_id, reg_data);
        if (mep_type == SOC_PPC_OAM_MEP_TYPE_LM ||
             mep_type==SOC_PPC_OAM_MEP_TYPE_DM  ||
             mep_type == SOC_PPC_OAM_MEP_TYPE_LM_STAT) {
            SHR_BITSET(found_bitmap_lcl,mep_type);
        }
    } else {
    
        GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, cur, reg_data);
        while ((mep_type == SOC_PPC_OAM_MEP_TYPE_LM ||
                 mep_type==SOC_PPC_OAM_MEP_TYPE_DM  ||
                 mep_type == SOC_PPC_OAM_MEP_TYPE_LM_STAT) &&
                ((SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit) > cur))) {
            GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, cur, reg_data);
            SHR_BITSET(found_bitmap_lcl,mep_type );
            ++cur;
        }
    }
    *found_bitmap = *found_bitmap_lcl;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}





uint32
  arad_pp_oam_oamp_create_new_eth1731_profile_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          was_previously_alloced,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    SOC_SAND_CHECK_NULL_INPUT(eth1731_profile);
    if (profile_indx > SOC_PPC_OAM_MAX_NUMBER_OF_ETH1731_MEP_PROFILES(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Profile index must be between 0 and %d."),SOC_PPC_OAM_MAX_NUMBER_OF_ETH1731_MEP_PROFILES(unit) ));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_oamp_create_new_eth1731_profile_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          was_previously_alloced,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 reg32_field, reg32_field_get;
    soc_reg_above_64_val_t reg_data, reg_data_get={0};
    soc_mem_t mem;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!was_previously_alloced) {
        if (SOC_IS_JERICHO(unit)) {
            res = soc_jer_pp_oam_oamp_eth1731_profile_set(unit,profile_indx, eth1731_profile);
            SOC_SAND_SOC_CHECK_FUNC_RESULT_ERR_VAL(res, 222,exit,res);
        } else {
            res = arad_pp_oam_oamp_write_eth1731_profile_internal(unit, profile_indx,  eth1731_profile);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
    }
    
    res = get_ccm_entry(unit, endpoint_id, &reg_data, &mem);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

    do
    {
        reg32_field = (uint32) profile_indx;
        soc_mem_field_set(unit, mem, reg_data, MEP_PROFILEf, &reg32_field);

    SOC_SAND_SOC_IF_ERROR_RETURN(res,32,exit,_ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit,mem,MEM_BLOCK_ALL,endpoint_id, reg_data));

    

        SOC_SAND_SOC_IF_ERROR_RETURN(res,32,exit,_ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit,mem,MEM_BLOCK_ALL,endpoint_id, reg_data_get));
        
        soc_mem_field_get(unit, mem, reg_data_get, MEP_PROFILEf, &reg32_field_get);
        if (reg32_field_get != reg32_field)
        {
                  LOG_WARN(BSL_LS_SOC_OAM,
                (BSL_META_U(unit,
                            "*****Warning: write on OAMP-MEP-DB failed**** Trying again \n\r")));
        }
    }
    while (reg32_field_get != reg32_field); 


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_oamp_set_oui_nic_registers_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
    SOC_SAND_IN  uint8                          profile_indx_nic
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));

}

uint32
  arad_pp_oam_oamp_set_oui_nic_registers_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
    SOC_SAND_IN  uint8                          profile_indx_nic
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data;
    soc_reg_above_64_val_t entry_above_64_get;
    uint32 profile_indx_nic_get;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 12, exit,
                                 arad_pp_oam_oamp_write_to_nic_and_oui_tables_internal(unit, lsb_to_nic, msb_to_oui,
                                                                                       profile_indx_nic, profile_indx_oui));
    SOC_REG_ABOVE_64_CLEAR(reg_data);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, endpoint_id, reg_data));
    soc_OAMP_MEP_DBm_field32_set(unit, reg_data, LMM_DA_NIC_PROFILEf,profile_indx_nic );

    do
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ALL, endpoint_id, reg_data));

        
        SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ALL, endpoint_id, entry_above_64_get));
        soc_mem_field_get(unit, OAMP_MEP_DBm, entry_above_64_get, LMM_DA_NIC_PROFILEf, &profile_indx_nic_get);

    } while (profile_indx_nic != (uint8)profile_indx_nic_get);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_oamp_lm_dm_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint32                                        endpoint_id,
    SOC_SAND_IN  uint8                          is_lm,
    SOC_SAND_IN uint8                           exists_piggy_back_down,
    SOC_SAND_OUT uint8                               * num_removed,
    SOC_SAND_OUT uint32                              * removed_index
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);



    SOC_SAND_CHECK_NULL_INPUT(num_removed);
    SOC_SAND_CHECK_NULL_INPUT(removed_index);
    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_oamp_lm_dm_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          is_lm,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *mep_db_entry,
    SOC_SAND_IN uint8                           exists_or_removing_piggy_back_down,
    SOC_SAND_OUT uint8                               * num_removed,
    SOC_SAND_OUT uint32                              * removed_index
  )
{
    soc_reg_above_64_val_t   entry, reg_field;
    soc_reg_above_64_val_t entry_above_64_get;
    uint32 next_type , mep_type;
    soc_mem_t mem;
    uint32 res = SOC_SAND_OK;
    uint32 is_1dm_set = 0;
    uint32 prog_1dm = 0, prog_1dm_down = 0;
    uint32 mep_pe_programmed = 0;
    uint32 mep_pe_prof=0;
    uint32 mep_pe_prof_get=0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
   
    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, endpoint_id+1,entry);

    
    if (mep_type== SOC_PPC_OAM_MEP_TYPE_DM) { 
        
        CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id + 1, entry);
        *num_removed = 1;
        *removed_index = endpoint_id + 1; 
    } else if (mep_type == SOC_PPC_OAM_MEP_TYPE_LM) {

        
        if((endpoint_id+2)<=soc_mem_index_max(unit,OAMP_MEP_DBm)){
            GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, endpoint_id+2,entry);
        } else {
            mep_type = 0;
        }
        
        if (is_lm) { 
            if (mep_type== SOC_PPC_OAM_MEP_TYPE_DM) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 12,exit,arad_pp_oam_move_entry_in_oamp_mep_db_internal(unit,endpoint_id + 2, endpoint_id+1));
                *num_removed  =1;
                *removed_index = endpoint_id+2;
            }
            else if (mep_type== SOC_PPC_OAM_MEP_TYPE_LM_STAT) {

                
                if((endpoint_id+3)<=soc_mem_index_max(unit,OAMP_MEP_DBm)){
                    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(next_type, endpoint_id+3,entry);
                } else {
                    next_type = 0;
                }

                *num_removed  = 2;
                if (next_type == SOC_PPC_OAM_MEP_TYPE_DM) {
                    SOC_SAND_SOC_IF_ERROR_RETURN(res, 12, exit, arad_pp_oam_move_entry_in_oamp_mep_db_internal(unit, endpoint_id + 3, endpoint_id + 1));
                    *removed_index = endpoint_id+2;
                    CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id +2,entry);
                } else {
                    CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id +1,entry);
                    CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id +2,entry);
                    *removed_index = endpoint_id+1;
                }
            } else { 
                *num_removed  = 1;
                *removed_index = endpoint_id+1;
                CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id + 1, entry); 
            }
        } else { 
            *num_removed  =1;
            
            if (mep_type== SOC_PPC_OAM_MEP_TYPE_DM) {
                CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id +2,entry);
                *removed_index = endpoint_id+2;
            }
            else if (mep_type== SOC_PPC_OAM_MEP_TYPE_LM_STAT) {
                    CLEAR_OAMP_MEP_DB_ENTRY(endpoint_id +3,entry);
                    *removed_index = endpoint_id+3;
                } 
            }
    }

    
    
    
    
    


    SOC_REG_ABOVE_64_CLEAR(entry);
    res = get_ccm_entry(unit, endpoint_id, &entry, &mem);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, endpoint_id, entry));
    
    arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_1DM_DOWN, &prog_1dm_down);
    arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_1DM, &prog_1dm);

    mep_pe_programmed = soc_OAMP_MEP_DBm_field32_get(unit, entry, MEP_PE_PROFILEf);
    
    is_1dm_set = ((prog_1dm == mep_pe_programmed) || (prog_1dm_down == mep_pe_programmed));

    if ((!is_lm && is_1dm_set) || (is_lm && exists_or_removing_piggy_back_down)) {
        uint32 ccm_type;
     
        ccm_type =  soc_OAMP_MEP_DBm_field32_get(unit, entry, MEP_TYPEf) ;

        if ((SOC_IS_JERICHO(unit) || ccm_type != SOC_PPC_OAM_MEP_TYPE_ETH_OAM) && !is_lm){
            soc_mem_field_set(unit, mem, entry, MEP_PE_PROFILEf, &mep_pe_prof);
            do
            {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, endpoint_id, entry));

                
                SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit,_ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit,MEM_BLOCK_ANY, endpoint_id, entry_above_64_get));
                soc_mem_field_get(unit, mem, entry_above_64_get, MEP_PE_PROFILEf, &mep_pe_prof_get);

            } while (mep_pe_prof != mep_pe_prof_get);
        } 
        
        if ( (SOC_IS_ARADPLUS_A0(unit) &&  ccm_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM)
             &&  ((is_lm && exists_or_removing_piggy_back_down) || !is_lm)) {
            uint32 is_up, prog_used;
            soc_OAMP_MEP_DBm_field_get(unit, entry, UP_1_DOWN_0f, &is_up);
            prog_used = is_up ? ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC : ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX;
            arad_pp_oamp_pe_program_profile_get(unit, prog_used, &mep_pe_prof);
            soc_OAMP_MEP_DBm_field_set(unit, entry, MEP_PE_PROFILEf, &mep_pe_prof);
            do
            {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, endpoint_id, entry));

                
                SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit,_ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit,MEM_BLOCK_ANY, endpoint_id, entry_above_64_get));
                soc_mem_field_get(unit, mem, entry_above_64_get, MEP_PE_PROFILEf, &mep_pe_prof_get);

            } while (mep_pe_prof != mep_pe_prof_get);
        }
    }

    if(mep_db_entry->is_jumbo_dm){
       mep_pe_prof = 0;
       mep_pe_prof_get = 0;
       if (mep_db_entry->name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE) {
           
           arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_MAID_48, &mep_pe_prof);
       }
       else if(mep_db_entry->name_type == _BCM_OAM_MA_NAME_TYPE_STRING_11_BYTES){
           
           arad_pp_oamp_pe_program_profile_get(unit, ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV, &mep_pe_prof);
       }
        
       soc_mem_field_set(unit, mem, entry, MEP_PE_PROFILEf, &mep_pe_prof);
       do
       {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, endpoint_id, entry));
            
            
            SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 23,exit,_ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit,MEM_BLOCK_ANY, endpoint_id, entry_above_64_get));
            soc_mem_field_get(unit, mem, entry_above_64_get, MEP_PE_PROFILEf, &mep_pe_prof_get);
       } while (mep_pe_prof != mep_pe_prof_get);
    }

    
    SOC_REG_ABOVE_64_CLEAR(entry); 
    SOC_REG_ABOVE_64_CLEAR(reg_field); 

    GET_TYPE_OF_OAMP_MEP_DB_ENTRY(mep_type, endpoint_id, entry);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


soc_error_t arad_pp_oam_oamp_1dm_set(
   int unit,
   int endpoint_id,
   uint8 use_1dm)
{
    int rv;
   soc_reg_above_64_val_t   entry;
   soc_reg_above_64_val_t   entry_above_64_get;
   uint32 pe_prof=0;
   uint32 mep_pe_prof_get=0;
   SOCDNX_INIT_FUNC_DEFS;


    SOC_REG_ABOVE_64_CLEAR(entry);
    rv = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, endpoint_id, entry);
    SOCDNX_SAND_IF_ERR_EXIT(rv);


    if (use_1dm) {
        arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_1DM,&pe_prof);
    }
    
    soc_OAMP_MEP_DBm_field32_set(unit, entry, MEP_PE_PROFILEf, pe_prof) ;

    do
    {
        rv =  _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,endpoint_id, entry);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
        
        SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
        
        rv =  _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,endpoint_id, entry_above_64_get);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
        
        soc_mem_field_get(unit, OAMP_MEP_DBm, entry_above_64_get, MEP_PE_PROFILEf, &mep_pe_prof_get);

    } while (pe_prof != mep_pe_prof_get);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t arad_pp_oam_oamp_1dm_get(
   int unit,
   int endpoint_id,
   uint8 *use_1dm)
{
    int rv;
    soc_reg_above_64_val_t   entry;
    uint32 one_dm_pe_prof=0;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(entry);
    rv = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, endpoint_id, entry);
    SOCDNX_SAND_IF_ERR_EXIT(rv); 

    arad_pp_oamp_pe_program_profile_get(unit,ARAD_PP_OAMP_PE_PROGS_1DM,&one_dm_pe_prof);

    *use_1dm = soc_OAMP_MEP_DBm_field32_get(unit,entry,MEP_PE_PROFILEf ) == one_dm_pe_prof;

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_pp_oamp_pe_use_1dm_check(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                              endpoint_id
  )
{
    uint32  res;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry = {0};
    uint32 whatever;
    SOCDNX_INIT_FUNC_DEFS;

    

    res = arad_pp_oam_oamp_mep_db_entry_get_internal_unsafe(unit, endpoint_id, &whatever, &mep_db_entry);
    SOCDNX_SAND_IF_ERR_EXIT(res); 

    
    if (mep_db_entry.port_status_tlv_en || mep_db_entry.interface_status_tlv_control ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE,(_BSL_SOCDNX_MSG("Feauture unavailable together with Port/Interface TLV")));
    }

    if (mep_db_entry.is_upmep) {
        
        if (mep_db_entry.local_port != (mep_db_entry.counter_pointer >>1) ) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE,(_BSL_SOCDNX_MSG("LSB of the src mac address must match the local port (both set in endpoint create)")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  arad_pp_oam_classifier_counter_disable_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_IN  uint8                                  counter_enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
  if (!SOC_IS_ARADPLUS(unit)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(profile, SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(packet_is_oam, 1, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(counter_enable, 1, ARAD_PP_OAM_INTERNAL_ERROR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_classifier_counter_disable_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_IN  uint8                                  counter_enable
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 reg1,reg2;
    uint32 map_key;
    uint32 field_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (SOC_IS_QUX(unit) && counter_enable == 1) { 
        SOC_SAND_EXIT_NO_ERROR;
    }

    if(SOC_IS_JERICHO_PLUS(unit))
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res,10,exit,READ_IHP_OAM_COUNTER_DISABLE_OUTER_MAPr(unit, &reg1));        
        SOC_SAND_SOC_IF_ERROR_RETURN(res,10,exit,READ_IHP_OAM_COUNTER_DISABLE_INNER_MAPr(unit, &reg2));        
    }
    else
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res,10,exit,READ_IHP_OAM_COUNTER_DISABLE_MAPr(unit, 0, &reg1));        
    }

    map_key = profile + (packet_is_oam << 4);
    field_val = (uint32)counter_enable;
    SHR_BITCOPY_RANGE(&reg1, map_key, &field_val, 0, 1);
    SHR_BITCOPY_RANGE(&reg2, map_key, &field_val, 0, 1);

    if(SOC_IS_JERICHO_PLUS(unit))
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res,20,exit,WRITE_IHP_OAM_COUNTER_DISABLE_OUTER_MAPr(unit, reg1));
        SOC_SAND_SOC_IF_ERROR_RETURN(res,20,exit,WRITE_IHP_OAM_COUNTER_DISABLE_INNER_MAPr(unit, reg2));
    }
    else
    {
        map_key = 0;
        if (counter_enable) {
            field_val = 0xff;
        } else {
            field_val = 0;
        }
        SHR_BITCOPY_RANGE(&reg1, map_key, &field_val, 0, 8);
        SOC_SAND_SOC_IF_ERROR_RETURN(res,20,exit,WRITE_IHP_OAM_COUNTER_DISABLE_MAPr(unit, SOC_CORE_ALL, reg1));
    }    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_classifier_counter_disable_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
  if (!SOC_IS_ARADPLUS(unit)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(profile, SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM, ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(packet_is_oam, 1, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_classifier_counter_disable_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_OUT  uint8                                 *counter_enable
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 reg;
    uint32 map_key;
    uint32 field_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(SOC_IS_JERICHO_PLUS(unit))
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res,10,exit,READ_IHP_OAM_COUNTER_DISABLE_OUTER_MAPr(unit, &reg));        
    }
    else
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res,10,exit,READ_IHP_OAM_COUNTER_DISABLE_MAPr(unit, 0, &reg));
    }


    map_key = profile + (packet_is_oam << 4);
    SHR_BITCOPY_RANGE(&field_val, 0, &reg, map_key, 1);
    *counter_enable = (uint8)field_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}




uint32 
    arad_pp_oam_oamp_loopback_set_verify(
       SOC_SAND_IN  int                                                    unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                              *loopback_info
       )
{
        uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(loopback_info);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only used only on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 
    arad_pp_oam_oamp_loopback_set(
       SOC_SAND_IN  int                                                    unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                              *loopback_info
       )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_oam_oamp_loopback_set_verify(unit, loopback_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_oamp_loopback_set_unsafe(unit, loopback_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

int _insert_ptch2(soc_reg_above_64_val_t pkt_header_reg, int next_header_is_itmh, int opaque_value, int pp_ssp) {
    INSERT_INTO_REG(pkt_header_reg, 0,2, 0,8,pp_ssp);
    INSERT_INTO_REG(pkt_header_reg, 0,2, 12,3,opaque_value);
    INSERT_INTO_REG(pkt_header_reg, 0,2, 15,1,!next_header_is_itmh);
    return 2;
}



int _insert_itmh(int unit ,soc_reg_above_64_val_t pkt_header_reg, int fwd_dst_info , int fwd_dp, int fwd_traffic_class, int snoop_cmd,int  in_mirr_flag,int pph_type) {
    if (soc_property_get(unit, spn_ITMH_ARAD_MODE_ENABLE, 0) || (SOC_IS_ARADPLUS_AND_BELOW(unit))) {
        INSERT_INTO_REG(pkt_header_reg, 2,4, 0,20,fwd_dst_info);
        INSERT_INTO_REG(pkt_header_reg, 2,4, 20,2,fwd_dp);
        INSERT_INTO_REG(pkt_header_reg, 2,4, 22,3,fwd_traffic_class);
        INSERT_INTO_REG(pkt_header_reg, 2,4, 25,4,snoop_cmd);
        INSERT_INTO_REG(pkt_header_reg, 2,4, 30,2,pph_type);
    } else {
        INSERT_INTO_REG(pkt_header_reg, 2,4, 30,2,pph_type);
        INSERT_INTO_REG(pkt_header_reg, 2,4, 27,2,fwd_dp);
        INSERT_INTO_REG(pkt_header_reg, 2,4, 1,3,fwd_traffic_class);
        INSERT_INTO_REG(pkt_header_reg, 2,4, 4,4,snoop_cmd);
        INSERT_INTO_REG(pkt_header_reg, 2,4, 8,19,fwd_dst_info);
    }
    return 4;
}



int _insert_udh(soc_reg_above_64_val_t pkt_header_reg, int udh_size) {
    return udh_size;
}



int _insert_itmh_ing_dest_info_extension(soc_reg_above_64_val_t pkt_header_reg,int fwd_dst_info){
    INSERT_INTO_REG(pkt_header_reg, 6,3, 0,20,fwd_dst_info);
    return 3;
}


int _insert_pph(soc_reg_above_64_val_t pkt_header_reg,const PPH_base * pph_fields, int offset) {
    
    INSERT_INTO_REG(pkt_header_reg,offset,7, 52, 2 ,pph_fields->field1);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 48, 4 ,pph_fields->field2);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 41, 7, pph_fields->field3);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 40, 1, pph_fields->field4);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 38, 2, pph_fields->field5);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 36, 2, pph_fields->field6);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 35, 1, pph_fields->field7);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 34, 1, pph_fields->field8);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 18, 16, pph_fields->field9);
    INSERT_INTO_REG(pkt_header_reg,offset,7, 0, 18, pph_fields->field10);

    return 7;
}


STATIC
int arad_insert_eth(soc_reg_above_64_val_t pkt_header_reg,const uint32 * dest_addr,const uint32 * src_addr,const  uint16 *vlans, const uint16 *tpids,
              int no_tags,   int follows_itmh, int udh_size) {
    int offset = follows_itmh? 6 + udh_size : 2;
    int size = 14 + no_tags*4, bit_offset=0, i=0;
    INSERT_INTO_REG(pkt_header_reg,offset, size,bit_offset, 16, SOC_PPC_L2_NEXT_PRTCL_TYPE_CFM);   
    bit_offset +=16;
    while (no_tags) {
        INSERT_INTO_REG(pkt_header_reg,offset,size,bit_offset , 16, vlans[i]); 
        bit_offset +=16;
        INSERT_INTO_REG(pkt_header_reg,offset,size, bit_offset, 16, tpids[i]); 
        bit_offset +=16;
        --no_tags;
        ++i;
    }
    INSERT_INTO_REG_BUFFER(pkt_header_reg,offset,size,bit_offset,48,src_addr); 
    bit_offset += 48;
    INSERT_INTO_REG_BUFFER(pkt_header_reg,offset,size,bit_offset,48,dest_addr);
    return size ;
}


int _insert_mpls_label(soc_reg_above_64_val_t pkt_header_reg, int label, uint8 exp, uint8 bos, uint8 ttl, int offset) {
    INSERT_INTO_REG(pkt_header_reg,offset,4,0,8,ttl); 
    INSERT_INTO_REG(pkt_header_reg,offset,4,8,1,bos); 
    INSERT_INTO_REG(pkt_header_reg,offset,4,9,3,exp);
    INSERT_INTO_REG(pkt_header_reg,offset,4,12,20,label); 
    return 4;
}


int _insert_4_bytes(soc_reg_above_64_val_t pkt_header_reg, uint32 stuff, int offset) {
    INSERT_INTO_REG(pkt_header_reg,offset,4,0,32,stuff); 
    return 4;
}


int _insert_oam_pdu_header(soc_reg_above_64_val_t pkt_header_reg, int level,int is_lbm, int offset){
    int opcode = is_lbm? SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBM: 
        SOC_PPC_OAM_ETHERNET_PDU_OPCODE_TST; 
    INSERT_INTO_REG(pkt_header_reg,offset,4, 0,8, 0x4);
    INSERT_INTO_REG(pkt_header_reg,offset,4, 16,8, opcode);
    INSERT_INTO_REG(pkt_header_reg,offset,4, 29,3,level);
    return 4;
}



int _insert_fhei(soc_reg_above_64_val_t pkt_header_reg, int cpu_trap_code_qualifier, int cpu_trap_code, int offset){
    INSERT_INTO_REG(pkt_header_reg,offset,3,8,16, cpu_trap_code_qualifier);
    INSERT_INTO_REG(pkt_header_reg,offset,3,0,8, cpu_trap_code);
    return 3;
}


uint32 
    arad_pp_oam_oamp_loopback_set_unsafe(
       SOC_SAND_IN  int                                                    unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                              *loopback_info
       )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t      pkt_header, reg_data, reg_field, tst_lbm_ctrl;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  mep_db_entry;
    uint32 short_name, entry;
    int header_size=0;
    int next_is_itmh;
    uint32  ssp;
    uint32 user_header_0_size, user_header_1_size,udh_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    uint32 tiks_per_ms, result_period;
    SOC_SAND_PP_MAC_ADDRESS *mac_address = &loopback_info->mac_address;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = arad_pmf_db_fes_user_header_sizes_get(
        unit,
        &user_header_0_size,
        &user_header_1_size,
        &user_header_egress_pmf_offset_0_dummy,
        &user_header_egress_pmf_offset_1_dummy
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    udh_size = user_header_1_size / 8 + user_header_0_size / 8; 


    SOC_REG_ABOVE_64_CLEAR(pkt_header);
    SOC_REG_ABOVE_64_CLEAR(tst_lbm_ctrl);
    SOC_REG_ABOVE_64_CLEAR(reg_field);
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_TST_LBM_CTRLr(unit, tst_lbm_ctrl));
    soc_reg_above_64_field_get(unit, OAMP_TST_LBM_CTRLr, tst_lbm_ctrl, TST_LBM_GEN_ENf, reg_field);

    if (reg_field[0] && loopback_info->is_added==0) { 
        goto exit;
    }

    SOC_REG_ABOVE_64_CLEAR(tst_lbm_ctrl); 

    reg_field[0] = 1;
    soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, tst_lbm_ctrl, TST_LBM_GEN_ENf, reg_field);
    
    
    
    tiks_per_ms = arad_chip_kilo_ticks_per_sec_get(unit);
    if (!loopback_info->is_period_in_pps) {
        
        result_period = loopback_info->tx_period * tiks_per_ms;
        
    }
    else {
        
        if (loopback_info->tx_period == 0) {
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("tx_period can't be 0!")));
        }
        result_period = SOC_SAND_DIV_ROUND_DOWN(tiks_per_ms , ((loopback_info->tx_period +1) / 1000) ) ;
        if (result_period >=2) {
            result_period -=2;
        }
    }
    reg_field[0] = (result_period > 0xffffff)? 0xffffff : result_period;
    soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, tst_lbm_ctrl, TST_LBM_PERIODf, reg_field);
    reg_field[0]=1;
    soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, tst_lbm_ctrl, TST_0_LBM_1f, reg_field);
    if (loopback_info->has_tst_tlv) {
        
        reg_field[0] = loopback_info->tst_tlv_len;
        soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, tst_lbm_ctrl, TST_LBM_TLV_LENf, reg_field);
        
        reg_field[0] = loopback_info->tst_tlv_type;
        soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, tst_lbm_ctrl, TST_LBM_TLV_PATTERN_TYPEf, reg_field);
        
        reg_field[0] = 32; 
        soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, tst_lbm_ctrl, TST_LBM_TLV_TYPEf, reg_field);
    }
    else {
        reg_field[0] = 0; 
        soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, tst_lbm_ctrl, TST_LBM_TLV_LENf, reg_field);
        soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, tst_lbm_ctrl, TST_LBM_TLV_PATTERN_TYPEf, reg_field);
        soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, tst_lbm_ctrl, TST_LBM_TLV_TYPEf, reg_field);
    }
    reg_field[0] = 0x8; 
    soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, tst_lbm_ctrl, TST_LBM_THRESHOLDf, reg_field);

    reg_field[0] =0x4; 
    soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, tst_lbm_ctrl, TST_LBM_PE_KEYf, reg_field);

    
    


    res = arad_pp_oam_oamp_mep_db_entry_get_internal_unsafe(unit,loopback_info->endpoint_id,&short_name,&mep_db_entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 234, exit);

    

    next_is_itmh = (mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE || mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP ||
                    mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION ||   
                    mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL || (mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_ETH_OAM && (!loopback_info->is_up_mep)));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 234, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_DOWN_PTCHr, REG_PORT_ANY, 0, DOWN_PTCH_PP_SSPf, &ssp));
    ssp = (mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_ETH_OAM && mep_db_entry.is_upmep)? mep_db_entry.local_port : ssp;
	if(SOC_DPP_CONFIG(unit)->pp.oam_ccm_rx_wo_tx && (mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_ETH_OAM && loopback_info->is_up_mep)){
		ssp = loopback_info->local_port;
	}

    header_size += _insert_ptch2(pkt_header,next_is_itmh,_ARAD_PP_OAM_PTCH_OPAQUE_VALUE ,
                               ssp ); 
        

    if (next_is_itmh) {
        soc_field_t fwd_tc[] = {CCM_CLASS_0f,CCM_CLASS_1f,CCM_CLASS_2f,CCM_CLASS_3f,CCM_CLASS_4f,CCM_CLASS_5f,CCM_CLASS_6f,CCM_CLASS_7f};
        soc_field_t fwd_dp[] = {CCM_DP_0f, CCM_DP_1f, CCM_DP_2f, CCM_DP_3f, CCM_DP_4f, CCM_DP_5f, CCM_DP_6f, CCM_DP_7f};
        uint32 tc, dp, fwd_dst_info;
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 2344, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_PR_2_FW_DTCr, REG_PORT_ANY, 0, fwd_tc[mep_db_entry.priority], &tc));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 2344, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_PR_2_FWDDPr, REG_PORT_ANY, 0, fwd_dp[mep_db_entry.priority], &dp));
        if (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_OAMP_MEM_20000m(unit, MEM_BLOCK_ANY, mep_db_entry.local_port, &entry)); 
            if(SOC_DPP_CONFIG(unit)->pp.oam_ccm_rx_wo_tx){
                entry = loopback_info->sys_port;
            }
            fwd_dst_info = 0xc0000 | entry;
        } else {
            fwd_dst_info = 0xa0000 | mep_db_entry.egress_if;
        }
        
        if (loopback_info->int_pri != -1) {
            tc = ((uint32)(loopback_info->int_pri & 0x1F)) >> 2;
            dp = ((uint32)loopback_info->int_pri) & 0x3;
        }
		header_size += _insert_itmh(unit, pkt_header, fwd_dst_info, dp, tc, 0, 0, (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP || mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE || mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION || mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL)); 
    }

    if (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
        uint16 vlans[2], tpids[2];
        uint32 src_mac_for_real[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S], dst_addr_in_uint32s[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
        uint64 src_addr;
        soc_field_t tpid_field[] = {CCM_TPID_0f, CCM_TPID_1f, CCM_TPID_2f, CCM_TPID_3f};
        uint64 uint64_field;

        if (next_is_itmh) {
            
            header_size += _insert_udh(pkt_header,udh_size);
        }

        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, OAMP_CCM_MAC_SAr, REG_PORT_ANY, 0, BASE_MAC_SAf, &src_addr));

        _ARAD_PP_OAM_MAC_ADD_MSB_SET(src_addr, src_mac_for_real);
        if (! loopback_info->is_up_mep) {
            _ARAD_PP_OAM_MAC_ADD_LSB_SET(mep_db_entry.local_port, src_mac_for_real);
        }
        else {
            
            _ARAD_PP_OAM_MAC_ADD_LSB_SET(((mep_db_entry.counter_pointer)>>1), src_mac_for_real);
        }
        dst_addr_in_uint32s[0] = mac_address->address[0] | mac_address->address[1] << 8 | mac_address->address[2] << 16
                | mac_address->address[3] <<24;
        dst_addr_in_uint32s[1] = mac_address->address[4] | mac_address->address[5] << 8;
 
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 234, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, OAMP_CCM_TPID_MAPr, REG_PORT_ANY, 0, tpid_field[mep_db_entry.outer_tpid], &uint64_field));
        tpids[0] = COMPILER_64_LO(uint64_field);      
        vlans[0] = mep_db_entry.outer_vid_dei_pcp;
        if (loopback_info->pkt_pri != 0xff) {
            
            vlans[0] = ((loopback_info->pkt_pri & 0xf) << 12) + (mep_db_entry.outer_vid_dei_pcp & 0xfff);
        }
        if (mep_db_entry.tags_num == 2) {
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 234, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, OAMP_CCM_TPID_MAPr, REG_PORT_ANY, 0, tpid_field[mep_db_entry.inner_tpid], &uint64_field));
            tpids[1] = tpids[0];
            vlans[1] = vlans[0];
            tpids[0] = COMPILER_64_LO(uint64_field);
            vlans[0] = mep_db_entry.inner_vid_dei_pcp;
            if (loopback_info->inner_pkt_pri != 0xff) {
                
                vlans[0] = ((loopback_info->inner_pkt_pri & 0xf) << 12) + (mep_db_entry.inner_vid_dei_pcp & 0xfff);
            }
        }
       
        header_size += arad_insert_eth(pkt_header,dst_addr_in_uint32s,src_mac_for_real, vlans, tpids,mep_db_entry.tags_num, !(loopback_info->is_up_mep), udh_size );
    } else {
        
        PPH_base pph;
        uint8 exp, ttl;
        soc_field_t mpls_pwe_prf_fields[] = {MPLS_PWE_PROFILE_0f, MPLS_PWE_PROFILE_1f, MPLS_PWE_PROFILE_2f, MPLS_PWE_PROFILE_3f, MPLS_PWE_PROFILE_4f, 
        MPLS_PWE_PROFILE_5f, MPLS_PWE_PROFILE_6f, MPLS_PWE_PROFILE_7f, MPLS_PWE_PROFILE_8f, MPLS_PWE_PROFILE_9f, MPLS_PWE_PROFILE_10f, 
        MPLS_PWE_PROFILE_11f, MPLS_PWE_PROFILE_12f, MPLS_PWE_PROFILE_13f, MPLS_PWE_PROFILE_14f, MPLS_PWE_PROFILE_15f, };


        header_size += _insert_itmh_ing_dest_info_extension(pkt_header, 0xc0000 | mep_db_entry.local_port);

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, arad_pp_oam_oamp_read_pph_header(unit, 0,_ARAD_PP_OAM_FWD_CODE_MPLS,udh_size,&pph));

        header_size += _insert_pph(pkt_header, &pph, header_size);

        
        header_size += _insert_udh(pkt_header,udh_size );


        SOC_REG_ABOVE_64_CLEAR(reg_data);
        SOC_REG_ABOVE_64_CLEAR(reg_field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_MPLS_PWE_PROFILEr_WITH_FAST_ACCESS(unit, reg_data));
        soc_reg_above_64_field_get(unit, OAMP_MPLS_PWE_PROFILEr, reg_data, mpls_pwe_prf_fields[mep_db_entry.push_profile],reg_field); 

        ttl = reg_field[0] & 0xff; 
        exp = (reg_field[0] & 0x700) >> 8 ; 
         
        if (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) {
            
            uint32 gal_gach;

            header_size += _insert_mpls_label(pkt_header, mep_db_entry.label,exp, 0, ttl, header_size);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1234, exit  , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_Y_1731O_MPLSTP_GALr, REG_PORT_ANY, 0, Y_1731O_MPLSTP_GALf, &gal_gach));
            header_size += _insert_4_bytes(pkt_header ,gal_gach,header_size);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1234, exit  , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_Y_1731O_MPLSTP_GACHr, REG_PORT_ANY, 0, Y_1731O_MPLSTP_GACHf, &gal_gach));
            header_size += _insert_4_bytes(pkt_header,gal_gach ,header_size);
        }

        
    }

    header_size += _insert_oam_pdu_header(pkt_header, mep_db_entry.mdl, 1, header_size);

    
    if (header_size < 0x18) {
        header_size += 4;
    }

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  124,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_TST_LBM_PACKET_HEADER_NUM_OF_BYTESr, REG_PORT_ANY, 0, TST_LBM_PACKET_HEADER_NUM_OF_BYTESf,  header_size));

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    soc_reg_above_64_field_set(unit, OAMP_TST_LBM_PACKET_HEADERr, reg_data, TST_LBM_PACKET_HEADERf, pkt_header);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_TST_LBM_PACKET_HEADERr(unit, reg_data));


    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_TST_LBM_CTRLr(unit, tst_lbm_ctrl));

    loopback_info->is_added = 1;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}






uint32 
    arad_pp_oam_oamp_loopback_get(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                         *loopback_info
       )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_oam_oamp_loopback_get_verify(unit, loopback_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_oamp_loopback_get_unsafe(unit, loopback_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 
    arad_pp_oam_oamp_loopback_get_verify(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                         *loopback_info
       )
{
        uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(loopback_info);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 
    arad_pp_oam_oamp_loopback_get_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                         *loopback_info
       )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t      reg_data, reg_field;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 234, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_RXB_DEBUGr, REG_PORT_ANY, 0, RX_LBR_COUNTf, &loopback_info->rx_packet_count));

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);   
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_TST_LBM_STATUSr(unit, reg_data));

    soc_reg_above_64_field_get(unit, OAMP_TST_LBM_STATUSr, reg_data, TST_LBM_PKT_CNTf, reg_field);
    loopback_info->tx_packet_count = reg_field[0];

    soc_reg_above_64_field_get(unit, OAMP_TST_LBM_STATUSr, reg_data, TST_LBM_DISCARD_PKT_CNTf, reg_field);
    loopback_info->discard_count = reg_field[0];

    soc_reg_above_64_field_get(unit, OAMP_TST_LBM_STATUSr, reg_data, TST_LBM_FIFO_CNTf, reg_field);
    loopback_info->fifo_count = reg_field[0];

    res = arad_pp_oam_oamp_loopback_get_config_unsafe(unit, loopback_info);
    SOC_SAND_IF_ERR_EXIT(res);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 
    arad_pp_oam_oamp_loopback_get_config_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                         *loopback_info
       )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t      reg_data, reg_field;
    uint32 tiks_per_ms = arad_chip_kilo_ticks_per_sec_get(unit);
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_TST_LBM_CTRLr(unit, reg_data));

    
    soc_reg_above_64_field_get(unit, OAMP_TST_LBM_CTRLr, reg_data, TST_LBM_PERIODf, reg_field);
    if (!loopback_info->is_period_in_pps) {
        
        loopback_info->tx_period =
            reg_field[0] ? SOC_SAND_DIV_ROUND(reg_field[0], tiks_per_ms) : -1;
    } 
    else {
        
        reg_field[0] +=2; 
        loopback_info->tx_period =
            reg_field[0] ? SOC_SAND_DIV_ROUND(tiks_per_ms *1000, reg_field[0] ) : tiks_per_ms * 1000;
    }

    
    soc_reg_above_64_field_get(unit, OAMP_TST_LBM_CTRLr, reg_data, TST_LBM_TLV_TYPEf, reg_field);
    if (reg_field[0] == 32) { 

        loopback_info->has_tst_tlv = 1;

        
        soc_reg_above_64_field_get(unit, OAMP_TST_LBM_CTRLr, reg_data, TST_LBM_TLV_LENf, reg_field);
        loopback_info->tst_tlv_len = reg_field[0];

        
        soc_reg_above_64_field_get(unit, OAMP_TST_LBM_CTRLr, reg_data, TST_LBM_TLV_PATTERN_TYPEf, reg_field);
        loopback_info->tst_tlv_type = reg_field[0];
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



   

uint32 
    arad_pp_oam_oamp_loopback_remove_verify(
       SOC_SAND_IN  int                                                  unit
       )
{
        uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32 
    arad_pp_oam_oamp_loopback_remove_unsafe(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32 res = SOC_SAND_OK;
    uint32 field; 
    soc_reg_above_64_val_t      reg_data, reg_field;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 234, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, OAMP_RXB_DEBUGr, REG_PORT_ANY, 0, RX_LBR_COUNTf, &field)); 
  
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(reg_field);      

    reg_field[0] =1;
    soc_reg_above_64_field_set(unit,OAMP_TST_LBM_CTRLr, reg_data, TST_LBM_RSTf, reg_field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_TST_LBM_CTRLr(unit, reg_data)); 


    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  124,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OAMP_TST_LBM_PACKET_HEADER_NUM_OF_BYTESr, REG_PORT_ANY, 0, TST_LBM_PACKET_HEADER_NUM_OF_BYTESf,  0));
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_OAMP_TST_LBM_PACKET_HEADERr(unit, reg_data));
   

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


soc_error_t
    arad_pp_oam_oamp_rx_trap_codes_set(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
       )
{
    uint32 res = 0;
    uint32 field;
    uint32 field_name;
    uint32 reg_index;
    uint32 internal_trap_code;
    SOCDNX_INIT_FUNC_DEFS;

    res = arad_pp_oam_oamp_rx_trap_codes_set_verify(unit, mep_type, trap_code);
    SOCDNX_IF_ERR_EXIT(res);

    switch (mep_type) {
    case SOC_PPC_OAM_MEP_TYPE_ETH_OAM: field_name = TRAP_N_ETHf;
        break;
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP:
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL:
        field_name = TRAP_N_1731O_MPLSTPf;
        break;
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE:
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION:
        field_name = TRAP_N_1731O_PWEf;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP: field_name = TRAP_N_BFDO_IPV4f;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS: field_name = TRAP_N_BFDO_MPLSf;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE: field_name = TRAP_N_BFDO_PWEf;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP: field_name = TRAP_N_BFDO_PWEf;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                             (_BSL_SOCDNX_MSG("Illegal mep type")));
    }

    res = _arad_pp_oam_trap_code_to_internal(unit,trap_code,&internal_trap_code);
    SOCDNX_IF_ERR_EXIT(res);

    
    
    for (reg_index=1; reg_index<=3; reg_index++) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, OAMP_RX_TRAP_CODESr, REG_PORT_ANY, reg_index, field_name, &field));

        if (field == internal_trap_code) {
            break;
        }
        else if (field == 0) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, OAMP_RX_TRAP_CODESr, REG_PORT_ANY, reg_index, field_name, internal_trap_code));
            break;
        }
    }

    if (reg_index > 3) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE,
                             (_BSL_SOCDNX_MSG("Out of resources, no room for another trap code")));
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t 
    arad_pp_oam_oamp_rx_trap_codes_set_verify(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
       )
{
    SOCDNX_INIT_FUNC_DEFS;

     if (!SOC_IS_ARADPLUS(unit)) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                              (_BSL_SOCDNX_MSG("Function for Arad+ and above.")));
     }

     if (mep_type > SOC_PPC_OAM_MEP_TYPE_COUNT) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                              (_BSL_SOCDNX_MSG("INTERNAL: MEP type above maximal.")));
     }

     if (trap_code > SOC_PPC_NOF_TRAP_CODES - 1) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                              (_BSL_SOCDNX_MSG("INTERNAL: trap code above maximal.")));
     }


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
    arad_pp_oam_oamp_rx_trap_codes_delete(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
       )
{
    uint32 res = 0;
    uint32 field; 
    uint32 field_name;
    uint32 reg_index;
    uint32 internal_trap_code;
    SOCDNX_INIT_FUNC_DEFS;

    res = arad_pp_oam_oamp_rx_trap_codes_delete_verify(unit,mep_type,trap_code);
    SOCDNX_IF_ERR_EXIT(res);

    switch (mep_type) {
    case SOC_PPC_OAM_MEP_TYPE_ETH_OAM: field_name = TRAP_N_ETHf;
        break;
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP:
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL:
        field_name = TRAP_N_1731O_MPLSTPf;
        break;
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE:
    case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION:
         field_name = TRAP_N_1731O_PWEf;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP: field_name = TRAP_N_BFDO_IPV4f;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS: field_name = TRAP_N_BFDO_MPLSf;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE: field_name = TRAP_N_BFDO_PWEf;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP: field_name = TRAP_N_BFDO_PWEf;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                             (_BSL_SOCDNX_MSG("Illegal mep type")));
    }

    res = _arad_pp_oam_trap_code_to_internal(unit,trap_code,&internal_trap_code);
    SOCDNX_IF_ERR_EXIT(res);

    
    
    for (reg_index=1; reg_index<=3; reg_index++) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, OAMP_RX_TRAP_CODESr, REG_PORT_ANY, reg_index, field_name, &field));

        if (field == internal_trap_code) {
            field = 0;
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, OAMP_RX_TRAP_CODESr, REG_PORT_ANY, reg_index, field_name, field));
            break;
        }
    }

exit:
 SOCDNX_FUNC_RETURN;
}

soc_error_t 
    arad_pp_oam_oamp_rx_trap_codes_delete_verify(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
       )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_ARADPLUS(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("Function for Arad+ and above.")));
    }

    if (mep_type > SOC_PPC_OAM_MEP_TYPE_COUNT) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("INTERNAL: MEP type above maximal.")));
    }

    if (trap_code > SOC_PPC_NOF_TRAP_CODES-1) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("INTERNAL: trap code above maximal.")));
    }

exit:
  SOCDNX_FUNC_RETURN;}




soc_error_t
  arad_pp_oam_inlif_profile_map_set(
     SOC_SAND_IN  int                                                  unit,
     SOC_SAND_IN  uint32                                               inlif_profile,
     SOC_SAND_IN  uint32                                               oam_profile
  ) {
    uint32
      res = SOC_SAND_OK,
      profile_map;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    SOC_SAND_ERR_IF_ABOVE_MAX(inlif_profile, ARAD_PP_LIF_LIF_PROFILE_MAX, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(oam_profile, _BCM_OAM_LIF_PROFILE_MAX, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR, READ_IHB_OAM_IN_LIF_PROFILE_MAPr(unit,  &profile_map));
    SHR_BITCOPY_RANGE(&profile_map, (inlif_profile*2), &oam_profile, 0, 2);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR, WRITE_IHB_OAM_IN_LIF_PROFILE_MAPr(unit,  profile_map));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


soc_error_t
  arad_pp_oam_inlif_profile_map_get(
     SOC_SAND_IN  int                                                  unit,
     SOC_SAND_IN  uint32                                               inlif_profile,
     SOC_SAND_OUT uint32                                               *oam_profile
  ) {
    uint32
      res = SOC_SAND_OK,
      profile_map;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    SOC_SAND_ERR_IF_ABOVE_MAX(inlif_profile, ARAD_PP_LIF_LIF_PROFILE_MAX, ARAD_PP_OAM_INTERNAL_ERROR, 20, exit);
    SOC_SAND_CHECK_NULL_INPUT(oam_profile);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR, READ_IHB_OAM_IN_LIF_PROFILE_MAPr(unit,  &profile_map));
    *oam_profile = (profile_map >> (inlif_profile*2)) & 0x3;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}




uint32 
    arad_pp_oam_dma_reset_verify(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }


    ARAD_PP_DO_NOTHING_AND_EXIT;
    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong"))); 
}

uint32 
    arad_pp_oam_dma_reset_unsafe(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32 res = SOC_SAND_OK;
    uint8 use_report_dma=0; 
    uint8 use_event_dma=0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    use_event_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_event_fifo_dma;        
    if (SOC_IS_JERICHO(unit)) 
    {
        use_report_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_report_fifo_dma;
    }

    if (use_event_dma) {
        res = arad_pp_oam_dma_set_unsafe(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res,12323,exit);
    } 

    if (use_report_dma) 
    {
        res = arad_pp_oam_report_dma_set_unsafe(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res,12323,exit);
    }

    last_dma_interrupt_message_num[unit]=0 ; 


    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}




uint32 
    arad_pp_oam_dma_clear_verify(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Function only works on Arad+")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong"))); 
}

uint32 
    arad_pp_oam_dma_clear_unsafe(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32 res = SOC_SAND_OK;
    uint32 use_dma = 0;
    uint32 use_report_dma = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    use_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_event_fifo_dma;
    if (SOC_IS_JERICHO(unit)) 
    {
        use_report_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_report_fifo_dma;
    }

    if (use_dma) {
        res = arad_pp_oam_dma_unset_unsafe(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res,12323,exit);
    }

    if (use_report_dma) {
        res = arad_pp_oam_report_dma_unset_unsafe(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res,12323,exit);
    }

    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 soc_arad_pp_oam_dma_set(int unit){
   uint32 res = SOC_SAND_OK;
   uint32 use_dma = 0;
   uint32 use_report_dma = 0;

   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   if (SOC_IS_ARADPLUS(unit)) {

      SOC_SAND_CHECK_DRIVER_AND_DEVICE;

      SOC_SAND_TAKE_DEVICE_SEMAPHORE;

      use_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_event_fifo_dma;
      if (SOC_IS_JERICHO(unit)) 
      {
          use_report_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_report_fifo_dma;
      }

      if (use_dma) {
         res = arad_pp_oam_dma_set_unsafe(unit);
         SOC_SAND_CHECK_FUNC_RESULT(res,12323,exit);
      }

      if (use_report_dma) {
          res = arad_pp_oam_report_dma_set_unsafe(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res,12323,exit);
      }

      SOC_SAND_GIVE_DEVICE_SEMAPHORE;
   }

   ARAD_PP_DO_NOTHING_AND_EXIT;
exit: 
   SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32 soc_arad_pp_oam_dma_clear(int unit){
   uint32 res = SOC_SAND_OK;
   uint32 use_dma = 0;
   uint32 use_report_dma = 0;

   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   if (SOC_IS_ARADPLUS(unit)) {

      SOC_SAND_CHECK_DRIVER_AND_DEVICE;

      res = arad_pp_oam_dma_clear_verify(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      SOC_SAND_TAKE_DEVICE_SEMAPHORE;

      use_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_event_fifo_dma;
      if (SOC_IS_JERICHO(unit)) 
      {
          use_report_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_report_fifo_dma;
      }

      if (use_dma) {
         res = arad_pp_oam_dma_unset_unsafe(unit);
         SOC_SAND_CHECK_FUNC_RESULT(res,12323,exit);
         if (dma_host_memory[unit] != NULL) {
             soc_cm_sfree(unit, dma_host_memory[unit]);
             dma_host_memory[unit] = NULL;
         }
      }

      if (use_report_dma) {
          res = arad_pp_oam_report_dma_unset_unsafe(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res,12323,exit);
          if (report_dma_host_memory[unit] != NULL) {
              soc_cm_sfree(unit, report_dma_host_memory[unit]);
              report_dma_host_memory[unit] = NULL;
          }
      }

      SOC_SAND_GIVE_DEVICE_SEMAPHORE;
   }

   ARAD_PP_DO_NOTHING_AND_EXIT;
exit: 
   SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 
    arad_pp_oam_register_dma_event_handler_callback_verify(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT        dma_event_handler_cb_t          event_handler_cb
       )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_DO_NOTHING_AND_EXIT;
    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong"))); 

}

uint32 
    arad_pp_oam_register_dma_event_handler_callback_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT        dma_event_handler_cb_t                      event_handler_cb
       )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    dma_event_handler_cb[unit] = event_handler_cb; 

    ARAD_PP_DO_NOTHING_AND_EXIT;
    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong"))); 

}




uint32 
   arad_pp_oam_dma_event_handler_verify(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_IN   int                                           event_type
       )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if ((!SOC_IS_JERICHO(unit)) && (event_type != SOC_PPC_OAM_DMA_EVENT_TYPE_EVENT)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Internal error - Statistics event are only available on Jericho")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong"))); 
}


soc_error_t arad_pp_oam_interrupt_init_internal(int unit, SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA * interrupt_data, int cmc, int ch) {
    uint32 reg,field_32;
    uint32 use_dma = 0;
    uint32 overflow;
    uint32 timeout;
    uint32 interrupt_enabled=0;
    int  num_entries=0,res;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(interrupt_data,0, sizeof(SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA) );

    use_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_event_fifo_dma;

    interrupt_data->last_interrupt_message_num = _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO;


    reg = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));
    timeout = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, reg, HOSTMEM_TIMEOUTf);
    overflow = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, reg, HOSTMEM_OVERFLOWf);



    if (use_dma && (timeout || overflow)) {

        interrupt_data->num_entries_available_in_local_buffer = 0;
        interrupt_data->num_entries_read_in_local_buffer = 0 ;

        
        res = _soc_mem_sbus_fifo_dma_get_num_entries(unit, (cmc*NOF_DMA_FIFO_PER_CMC+ch), &num_entries);

        if (res != SOC_E_EMPTY ) {
            interrupt_data->buffer_copied_from_dma_host_memory= soc_cm_salloc(unit, num_entries *_ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA , "oam DMA local ");
            if (!interrupt_data->buffer_copied_from_dma_host_memory) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS,
                                     (_BSL_SOCDNX_MSG("malloc failed: buffer_copied_from_dma_host_memory")));
            }

            interrupt_data->internal_buffer_is_allocated = 1;
            
            if ((last_dma_interrupt_message_num[unit] + num_entries) > arad_pp_num_entries_in_dma_host_memory[unit] ) {
                
                    int entries_to_write = arad_pp_num_entries_in_dma_host_memory[unit]  - last_dma_interrupt_message_num[unit];
                    int entries_written;

                    sal_memcpy(interrupt_data->buffer_copied_from_dma_host_memory, dma_host_memory[unit] + last_dma_interrupt_message_num[unit] * 20,
                           _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA * entries_to_write );
                    entries_written = entries_to_write;
                    entries_to_write = num_entries -  entries_written;
                    sal_memcpy(interrupt_data->buffer_copied_from_dma_host_memory + entries_written*20, dma_host_memory[unit],
                                entries_to_write * _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA );
            } else {
                
                sal_memcpy(interrupt_data->buffer_copied_from_dma_host_memory, dma_host_memory[unit] + last_dma_interrupt_message_num[unit] * 20,
                           _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA * num_entries);
            }

            interrupt_data->num_entries_available_in_local_buffer = num_entries;
            interrupt_data->num_entries_read_in_local_buffer = 0;
            last_dma_interrupt_message_num[unit] = (last_dma_interrupt_message_num[unit] + num_entries) % arad_pp_num_entries_in_dma_host_memory[unit] ;

            field_32 = 0;
            
            if(timeout)
            {
                soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &field_32, HOSTMEM_TIMEOUTf, 1);
            }
            if(overflow)
            {
                soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &field_32, HOSTMEM_OVERFLOWf, 1);
            }
            soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(cmc, ch), field_32);

            res = _soc_mem_sbus_fifo_dma_set_entries_read(unit, (cmc*NOF_DMA_FIFO_PER_CMC+ch), num_entries);
            SOCDNX_IF_ERR_EXIT(res);

            
            soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));
            interrupt_enabled = 1;

            sal_memset(interrupt_data->interrupt_message, 0xff, _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA);
        } else {
            field_32 = 0;
            
            if(overflow)
            {
                soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &field_32, HOSTMEM_OVERFLOWf, 1);
            }
            if(timeout)
            {
                soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &field_32, HOSTMEM_TIMEOUTf, 1);
            }
            soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(cmc, ch), field_32);

            
            sal_memset(interrupt_data->interrupt_message, 0xff, _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA);
            SOCDNX_EXIT_WITH_ERR_NO_MSG(res);
        }
    }

exit:
    if (use_dma && (interrupt_enabled==0)) {
    
    soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));
    }

    SOCDNX_FUNC_RETURN;
}


soc_error_t arad_pp_report_oam_interrupt_init_internal(int unit, SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA * interrupt_data, int cmc, int ch) {
    uint32 field_32, reg;
    uint32 overflow;
    uint32 timeout;
    uint32 interrupt_enabled=0;
    int  num_entries=0,res, use_dma;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(interrupt_data,0, sizeof(SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA) );


    interrupt_data->last_interrupt_message_num = _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO;
    use_dma = SOC_DPP_CONFIG(unit)->pp.oam_use_report_fifo_dma;

    reg = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));
    timeout = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, reg, HOSTMEM_TIMEOUTf);
    overflow = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, reg, HOSTMEM_OVERFLOWf);



    if (use_dma && (timeout || overflow) ) {
        interrupt_data->num_entries_available_in_local_buffer = 0;
        interrupt_data->num_entries_read_in_local_buffer = 0;

        
        res = _soc_mem_sbus_fifo_dma_get_num_entries(unit, (cmc*NOF_DMA_FIFO_PER_CMC+ch), &num_entries);

        if (res != SOC_E_EMPTY) {
            interrupt_data->buffer_copied_from_dma_host_memory = soc_cm_salloc(unit, num_entries *_ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA , "report oam DMA local");
            if (!interrupt_data->buffer_copied_from_dma_host_memory) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS,
                                     (_BSL_SOCDNX_MSG("malloc failed: buffer_copied_from_dma_host_memory")));
            }
            interrupt_data->internal_buffer_is_allocated = 1;
            
            if ((last_report_dma_interrupt_message_num[unit] + num_entries) > arad_pp_num_entries_in_report_dma_host_memory[unit] ) {
                
                    int entries_to_write = arad_pp_num_entries_in_report_dma_host_memory[unit] - last_report_dma_interrupt_message_num[unit];
                    int entries_written;
                    sal_memcpy(interrupt_data->buffer_copied_from_dma_host_memory, report_dma_host_memory[unit] + last_report_dma_interrupt_message_num[unit] * 20,
                           _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA * entries_to_write );
                    entries_written = entries_to_write;
                    entries_to_write = num_entries -  entries_written;
                    sal_memcpy(interrupt_data->buffer_copied_from_dma_host_memory + entries_written*20, report_dma_host_memory[unit],
                                entries_to_write * _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA );
            } else {
                
                sal_memcpy(interrupt_data->buffer_copied_from_dma_host_memory, report_dma_host_memory[unit] + last_report_dma_interrupt_message_num[unit] * 20,
                           _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA * num_entries);
            }

            interrupt_data->num_entries_available_in_local_buffer = num_entries;
            interrupt_data->num_entries_read_in_local_buffer = 0;
            last_report_dma_interrupt_message_num[unit] = (last_report_dma_interrupt_message_num[unit] + num_entries) % arad_pp_num_entries_in_report_dma_host_memory[unit] ;

            field_32 = 0;
            
            if(timeout)
            {
                soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &field_32, HOSTMEM_TIMEOUTf, 1);
            }
            if(overflow)
            {
                soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &field_32, HOSTMEM_OVERFLOWf, 1);
            }
            soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(cmc, ch), field_32);

            res = _soc_mem_sbus_fifo_dma_set_entries_read(unit, (cmc*NOF_DMA_FIFO_PER_CMC+ch), num_entries);
            SOCDNX_IF_ERR_EXIT(res);

            
            soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));
            interrupt_enabled = 1;

            sal_memset(interrupt_data->interrupt_message, 0xff, _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA);
        } else {
            field_32 = 0;
            
            if(timeout)
            {
                soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &field_32, HOSTMEM_TIMEOUTf, 1);
            }
            if(overflow)
            {
                soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &field_32, HOSTMEM_OVERFLOWf, 1);
            }
            soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(cmc, ch), field_32);

            
            sal_memset(interrupt_data->interrupt_message, 0xff, _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA);
            SOCDNX_EXIT_WITH_ERR_NO_MSG(res);
        }
    }

exit:
    if (use_dma && !interrupt_enabled) {
        
        soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));
    }
    SOCDNX_FUNC_RETURN;
}

void arad_pp_oam_interrupt_free_internal(int unit,SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA * interrupt_data ) {
    if (interrupt_data->internal_buffer_is_allocated) {
        soc_cm_sfree(unit, interrupt_data->buffer_copied_from_dma_host_memory);
    } 
}


soc_error_t 
   arad_pp_oam_dma_event_handler_unsafe(
       SOC_SAND_IN   int                                           unit,
       SOC_SAND_IN   int                                           event_type,
       SOC_SAND_IN   int                                           cmc,
       SOC_SAND_IN   int                                           ch       
       )
{
    SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA interrupt_data;
    int res;
    SOCDNX_INIT_FUNC_DEFS;

    if (event_type == SOC_PPC_OAM_DMA_EVENT_TYPE_STAT_EVENT)
    {
        res = arad_pp_report_oam_interrupt_init_internal(unit, &interrupt_data, cmc, ch); 
        if (res == SOC_E_EMPTY) {
            
            SOC_EXIT;
        } else {
            SOCDNX_IF_ERR_EXIT(res);
        }
    }
    else 
    {
        res = arad_pp_oam_interrupt_init_internal(unit, &interrupt_data, cmc, ch); 
        if (res == SOC_E_EMPTY) {
            
            SOC_EXIT;
        } else {
            SOCDNX_IF_ERR_EXIT(res);
        }
    }

    res =  (dma_event_handler_cb[unit]) ? (dma_event_handler_cb[unit](unit, event_type, &interrupt_data)) : 0; 
    SOCDNX_IF_ERR_EXIT(res);
    

exit:
    arad_pp_oam_interrupt_free_internal(unit, &interrupt_data);
    SOCDNX_FUNC_RETURN; 
}





uint32
  arad_pp_oam_diag_print_lookup_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  soc_ppd_oam_diag_print_dma_dump_unsafe(
      SOC_SAND_IN   int                                           unit
       )
{
    SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA interrupt_data;
    int res=0;
    int cmc=0, ch=0;
    int channel_number = -1;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO(unit))
    {
      SOCDNX_IF_ERR_EXIT(jer_mgmt_dma_fifo_channel_get(unit, dma_fifo_channel_src_oam_event, &channel_number));  

      if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
          cmc = channel_number / NOF_DMA_FIFO_PER_CMC;
          ch = channel_number % NOF_DMA_FIFO_PER_CMC;    
      } else {
          cmc = SOC_PCI_CMC(unit);
          ch = channel_number % NOF_DMA_FIFO_PER_CMC;  
      }
    }
    res = arad_pp_oam_interrupt_init_internal(unit, &interrupt_data, cmc, ch); 
    if (res == SOC_E_EMPTY) {
        
        LOG_CLI((BSL_META_U(unit,"The DMA buffer is empty! CMIC:%d , CHANNEL:%d\n"),cmc,ch));
        SOC_EXIT;
    } else {
        SOCDNX_IF_ERR_EXIT(res);
    }
    
    SOCDNX_IF_ERR_EXIT(arad_pp_oam_dma_dump_read_and_display_unsafe(unit,&interrupt_data));
    

exit:
    arad_pp_oam_interrupt_free_internal(unit, &interrupt_data);
    SOCDNX_FUNC_RETURN; 
}

uint32
  arad_pp_oam_diag_print_lookup_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
    uint64 oema_ll_reg, oemb_ll_reg, rmapem_ll_reg;
    uint32 res, printed =0;
    uint64 key, result, found;
    uint32 key32, result32[2];
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      oem1_key = {0};
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  oem1_payload= {0};
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY     oem2_key= {0};
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  oem2_payload= {0};
    soc_reg_t
        last_lookup_regs[2];
    uint32
        lookup_reg_idx,
        nof_last_lookup_regs;
    soc_field_t key_field[2]={0}, result_field[2]={0}, found_field[2]={0};
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    
    if(SOC_IS_JERICHO(unit)) {
        last_lookup_regs[0] = PPDB_A_OEMA_LAST_LOOKUP_MASTERr;
        last_lookup_regs[1] = PPDB_A_OEMA_LAST_LOOKUP_SLAVEr;
        key_field[0] = OEMA_LAST_LOOKUP_MASTER_KEYf;
        key_field[1] = OEMA_LAST_LOOKUP_SLAVE_KEYf;
        result_field[0] = OEMA_LAST_LOOKUP_MASTER_RESULTf;
        result_field[1] = OEMA_LAST_LOOKUP_SLAVE_RESULTf;
        found_field[0] = OEMA_LAST_LOOKUP_MASTER_RESULT_FOUNDf;
        found_field[1] = OEMA_LAST_LOOKUP_SLAVE_FOUNDf  ;
        
        if (SOC_IS_QAX(unit) || SOC_IS_QUX(unit)) found_field[1] = OEMA_LAST_LOOKUP_SLAVE_RESULT_FOUNDf;
        nof_last_lookup_regs = 2;

    } else {
        last_lookup_regs[0] = IHB_OEMA_LAST_LOOKUPr;
        key_field[0] = OEMA_LAST_LOOKUP_KEYf;
        result_field[0] = OEMA_LAST_LOOKUP_RESULTf;
        found_field[0] = OEMA_LAST_LOOKUP_RESULT_FOUNDf;

        nof_last_lookup_regs = 1;
    }

    for(lookup_reg_idx = 0; lookup_reg_idx < nof_last_lookup_regs; ++lookup_reg_idx) {
        res = soc_reg_get(unit, last_lookup_regs[lookup_reg_idx], REG_PORT_ANY, 0, &oema_ll_reg);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

        key = soc_reg64_field_get(unit,last_lookup_regs[lookup_reg_idx], oema_ll_reg,key_field[lookup_reg_idx]);
        result = soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oema_ll_reg, result_field[lookup_reg_idx]);
        found = soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oema_ll_reg, found_field[lookup_reg_idx]);
        if (COMPILER_64_LO(found) || COMPILER_64_LO(key)) {
            key32 = COMPILER_64_LO(key);
            result32[0] = COMPILER_64_LO(result);
            result32[1] = COMPILER_64_HI(result);
            _ARAD_PP_OAM_OEM1_PAYLOAD_TO_PAYLOAD_STRUCT(result32, oem1_payload);
            _ARAD_PP_OAM_OEM1_KEY_TO_KEY_STRUCT(oem1_key, key32);
            LOG_CLI((BSL_META_U(unit,
                                "IHB OEMA last lookup: Key=0X%x, result=0X%x%x\n"), key32,result32[1],result32[0]));
            _ARAD_PP_OAM_PRINT_OEM1_KEY(oem1_key);
            if (COMPILER_64_LO(found)) {
                _ARAD_PP_OAM_PRINT_OEM1_PAYLOAD(oem1_payload);

            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "\tNot found.\n")));
            }
            printed = 1;
        }
    }

    
    if(SOC_IS_JERICHO(unit)) {
        last_lookup_regs[0] = PPDB_A_OEMB_LAST_LOOKUP_MASTERr;
        last_lookup_regs[1] = PPDB_A_OEMB_LAST_LOOKUP_SLAVEr;
        key_field[0] = OEMB_LAST_LOOKUP_MASTER_KEYf;
        key_field[1] = OEMB_LAST_LOOKUP_SLAVE_KEYf;
        result_field[0] = OEMB_LAST_LOOKUP_MASTER_RESULTf;
        result_field[1] = OEMB_LAST_LOOKUP_SLAVE_RESULTf;
        found_field[0] = OEMB_LAST_LOOKUP_MASTER_RESULT_FOUNDf;
        found_field[1] = OEMB_LAST_LOOKUP_SLAVE_FOUNDf  ;
        
        if (SOC_IS_QAX(unit) || SOC_IS_QUX(unit)) found_field[1] = OEMB_LAST_LOOKUP_SLAVE_RESULT_FOUNDf;
        nof_last_lookup_regs = 2;
    } else {
        last_lookup_regs[0] = IHB_OEMB_LAST_LOOKUPr;
        key_field[0] = OEMB_LAST_LOOKUP_KEYf;
        result_field[0] = OEMB_LAST_LOOKUP_RESULTf;
        found_field[0] = OEMB_LAST_LOOKUP_RESULT_FOUNDf;
        nof_last_lookup_regs = 1;
    }

    for(lookup_reg_idx = 0; lookup_reg_idx < nof_last_lookup_regs; ++lookup_reg_idx) {
        res = soc_reg_get(unit, last_lookup_regs[lookup_reg_idx], REG_PORT_ANY, 0, &oemb_ll_reg);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
        key =soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oemb_ll_reg, key_field[lookup_reg_idx]);
        result = soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oemb_ll_reg, result_field[lookup_reg_idx]);
        found =  soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oemb_ll_reg, found_field[lookup_reg_idx]);
        if (COMPILER_64_LO(found) || COMPILER_64_LO(key)) {
            key32 = COMPILER_64_LO(key);
            result32[0] = COMPILER_64_LO(result); 
            _ARAD_PP_OAM_OEM2_PAYLOAD_TO_PAYLOAD_STRUCT(result32[0], oem2_payload);
            _ARAD_PP_OAM_OEM2_KEY_TO_KEY_STRUCT(oem2_key, key32);
            LOG_CLI((BSL_META_U(unit,
                                "IHB OEMB last lookup: Key=0X%x  , result=0X%x\n"),key32,result32[0]));
            LOG_CLI((BSL_META_U(unit,
                                    "\tOEMB key: ingress: %d, MDL: %d, OAM LIF: 0x%x, your disc: %d\n"),  oem2_key.ingress, oem2_key.mdl,  oem2_key.oam_lif, oem2_key.your_disc)); 
            if (COMPILER_64_LO(found)) {
                LOG_CLI((BSL_META_U(unit,
                                            "\t\tOEMB payload: MP profile: 0x%x,OAM ID: 0x%x\n"), oem2_payload.mp_profile, oem2_payload.oam_id));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                            "\tNot found.\n")));
            }
            printed = 1;
        }
    }

    
   SOC_SAND_SOC_IF_ERROR_RETURN(res, 10,exit, READ_OAMP_RMAPEM_LAST_LOOKUPr(unit,&rmapem_ll_reg));

   key = soc_reg64_field_get(unit, OAMP_RMAPEM_LAST_LOOKUPr, rmapem_ll_reg, RMAPEM_LAST_LOOKUP_KEYf);
   result = soc_reg64_field_get(unit, OAMP_RMAPEM_LAST_LOOKUPr, rmapem_ll_reg, RMAPEM_LAST_LOOKUP_RESULTf);
   found = soc_reg64_field_get(unit, OAMP_RMAPEM_LAST_LOOKUPr, rmapem_ll_reg, RMAPEM_LAST_LOOKUP_RESULT_FOUNDf); 
   if (COMPILER_64_LO(found)|| COMPILER_64_LO(key)) {
       LOG_CLI((BSL_META_U(unit,
                           "\tOAMP RMAPEM last lookup: Key=0X%x , result=0X%x"),COMPILER_64_LO(key), COMPILER_64_LO(result)));
       PRINT_FOUND(found);
       printed = 1;
   }

   if (printed == 0) {
       LOG_CLI((BSL_META_U(unit,
                           "\tNothing to display.\n")));    
   }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));

}

uint32
arad_pp_get_crps_counter_verify(
   SOC_SAND_IN int unit,
   SOC_SAND_IN uint8 crps_counter_number,
   SOC_SAND_IN uint32 reg_number,
   SOC_SAND_OUT uint32 *value
   ) {
   uint32
      res = SOC_SAND_OK;
   SOC_SAND_INIT_ERROR_DEFINITIONS(0);


   if (SOC_IS_JERICHO(unit)) {
      
      if (crps_counter_number >= (SOC_DPP_DEFS_GET(unit, nof_counter_processors))) { 
         SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("crps_counter number must be 0 to 15, value given is %d"),crps_counter_number));
      }
      if (reg_number >= 0x4000) {
         SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("register number must be between 0 and (16K-1), value given is %d"),reg_number));
      }
   } else {
      if (crps_counter_number >= (SOC_DPP_DEFS_GET(unit, nof_counter_processors))) { 
         SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("crps_counter number must be 0,1,2 0r 3, value given is %d"),crps_counter_number));
      }
      if (reg_number >= 0x8000) {
         SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("register number must be between 0 and (32K-1), value given is %d"),reg_number));
      }
   }
   SOC_SAND_CHECK_NULL_INPUT(value);

   ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_get_crps_counter_verify()", 0, 0);
}


uint32
arad_pp_get_crps_counter_unsafe(
   SOC_SAND_IN int unit,
   SOC_SAND_IN uint8 crps_counter_number,
   SOC_SAND_IN uint32 reg_number,
   SOC_SAND_OUT uint32 *value
   ) {
   soc_reg_above_64_val_t crps_reg;
   uint32 res;
   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   if (SOC_IS_QAX(unit)) {
       SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_CNTS_MEMm(unit, crps_counter_number, MEM_BLOCK_ANY, reg_number, &crps_reg));
       soc_CRPS_CRPS_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
       SOC_SAND_EXIT_NO_ERROR;
   }

   switch (crps_counter_number) {
   case 0:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_0_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_0_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 1:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_1_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_1_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 2:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_2_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_2_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 3:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_3_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 4:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_4_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 5:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_5_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 6:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_6_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 7:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_7_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 8:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_8_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 9:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_9_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 10:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_10_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 11:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_11_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 12:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_12_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 13:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_13_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 14:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_14_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 15:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_15_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 16:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_16_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   case 17:
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_CRPS_CRPS_17_CNTS_MEMm(unit, MEM_BLOCK_ANY, reg_number, &crps_reg));
      soc_CRPS_CRPS_3_CNTS_MEMm_field_get(unit, crps_reg, PACKETS_COUNTERf, value);
      break;
   default:
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Illegal CRPS engine number.\n")));
   }

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_get_crps_counter_unsafe()", crps_counter_number, reg_number);
}


soc_error_t
  arad_pp_oamp_report_mode_set(int unit, SOC_PPC_OAM_REPORT_MODE mode) {

    uint32 rv;
    uint32 reg;

    SOCDNX_INIT_FUNC_DEFS;

    if (mode < SOC_PPC_OAM_REPORT_MODE_COUNT) {

        
        if (mode == SOC_PPC_OAM_REPORT_MODE_RAW) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Error - Raw report mode unsupported")));
        }

        rv = READ_OAMP_MODE_REGISTERr(unit, &reg);
        SOCDNX_SAND_IF_ERR_EXIT(rv);

        soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, REPORT_MODEf, mode);
        rx_report_event_mode[unit] = mode;

        rv = WRITE_OAMP_MODE_REGISTERr(unit,reg);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_pp_oamp_report_mode_get(int unit, SOC_PPC_OAM_REPORT_MODE *mode) {

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(mode);

    *mode = rx_report_event_mode[unit];

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_arad_pp_oamp_control_punt_packet_int_pri_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *DP_and_TC
  )
{
    soc_reg_above_64_val_t reg_above_64;
    uint32 soc_sand_rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(DP_and_TC);

    soc_sand_rv = READ_OAMP_CPUPORTr(unit, reg_above_64);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    *DP_and_TC = soc_reg_above_64_field32_get(unit, OAMP_CPUPORTr, reg_above_64, CP_UPORT_DPf);           
    *DP_and_TC |= (soc_reg_above_64_field32_get(unit, OAMP_CPUPORTr, reg_above_64, CP_UPORT_TCf) << 2);   

exit:
        SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_arad_pp_oamp_control_ccm_weight_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *ccm_weight
  )
{
    uint32 oamp_arbiter_weight;
    uint32 soc_sand_rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(ccm_weight);

    soc_sand_rv = READ_OAMP_ARBITER_WEIGHTr(unit, &oamp_arbiter_weight);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    *ccm_weight = soc_reg_field_get(unit, OAMP_ARBITER_WEIGHTr, oamp_arbiter_weight, TXM_ARB_WEIGHTf);

exit:
        SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_arad_pp_oamp_control_sat_weight_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *sat_weight
  )
{
    uint32 oamp_arbiter_weight;
    uint32 soc_sand_rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(sat_weight);

    soc_sand_rv = READ_OAMP_ARBITER_WEIGHTr(unit, &oamp_arbiter_weight);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    *sat_weight = soc_reg_field_get(unit, OAMP_ARBITER_WEIGHTr, oamp_arbiter_weight, TST_ARB_WEIGHTf);

exit:
        SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_arad_pp_oamp_control_response_weight_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *response_weight
  )
{
    uint32 oamp_arbiter_weight;
    uint32 soc_sand_rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(response_weight);

    soc_sand_rv = READ_OAMP_ARBITER_WEIGHTr(unit, &oamp_arbiter_weight);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    *response_weight = soc_reg_field_get(unit, OAMP_ARBITER_WEIGHTr, oamp_arbiter_weight, RSP_ARB_WEIGHTf);

exit:
        SOCDNX_FUNC_RETURN;
}


int
  arad_pp_oam_bfd_acc_endpoint_tx_disable(int unit, uint32 mep_index, const SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry) {

    soc_mem_t mem;
    uint32 reg_val, reg_val_get;
    soc_reg_above_64_val_t entry_above_64;
    soc_reg_above_64_val_t entry_above_64_get;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(mep_db_entry);

    SOCDNX_IF_ERR_EXIT(_arad_pp_oam_bfd_mep_db_mem_by_entry_type_get(unit, mep_db_entry->mep_type, &mem));

    
    SOCDNX_SAND_IF_ERR_EXIT(_ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, mep_index, &entry_above_64));

    
    reg_val = 15;
    soc_mem_field_set(unit, mem, entry_above_64, MEP_TYPEf, &reg_val);

    
    do
    {
        SOCDNX_SAND_IF_ERR_EXIT(_ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, mep_index, &entry_above_64));

        
        SOC_REG_ABOVE_64_CLEAR(entry_above_64_get);
        SOCDNX_SAND_IF_ERR_EXIT(_ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit, mem, MEM_BLOCK_ANY, mep_index, &entry_above_64_get));
        soc_mem_field_get(unit, mem, entry_above_64_get, MEP_TYPEf, &reg_val_get);

    } while (reg_val != reg_val_get);

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_oam_diag_print_rx_verify(
     SOC_SAND_IN int unit
   )
{
    uint32
       res = SOC_SAND_OK;


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

  


uint32 
    arad_pp_oam_last_lookup_found(
        SOC_SAND_IN int unit,
        SOC_SAND_OUT uint8 *val
    )
{
    uint32
        res; 
    uint64 oema_ll_reg, oemb_ll_reg, found;
    soc_reg_t
        last_lookup_regs[2];
    soc_field_t
        last_lookup_fields[2];
    uint32
        lookup_reg_idx,
        nof_last_lookup_regs;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *val = 0;
    if(SOC_IS_JERICHO(unit)) {
        last_lookup_regs[0] = PPDB_A_OEMA_LAST_LOOKUP_MASTERr;
        last_lookup_regs[1] = PPDB_A_OEMA_LAST_LOOKUP_SLAVEr;
        last_lookup_fields[0] = OEMA_LAST_LOOKUP_MASTER_RESULT_FOUNDf;
        last_lookup_fields[1] = OEMA_LAST_LOOKUP_SLAVE_FOUNDf  ;
        
        if (SOC_IS_QAX(unit) || SOC_IS_QUX(unit)) last_lookup_fields[1] = OEMA_LAST_LOOKUP_SLAVE_RESULT_FOUNDf;
        nof_last_lookup_regs = 2;
    } else {
        last_lookup_regs[0] = IHB_OEMA_LAST_LOOKUPr;
        last_lookup_fields[0] = OEMA_LAST_LOOKUP_RESULT_FOUNDf;
        nof_last_lookup_regs = 1;
    }

    for(lookup_reg_idx = 0; lookup_reg_idx < nof_last_lookup_regs; ++lookup_reg_idx) {
        res = soc_reg_get(unit, last_lookup_regs[lookup_reg_idx], REG_PORT_ANY, 0, &oema_ll_reg);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        found = soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oema_ll_reg, last_lookup_fields[lookup_reg_idx]);
        if (COMPILER_64_LO(found)) {
            
            *val = 1;
            ARAD_DO_NOTHING_AND_EXIT;
        }
    }

    if(SOC_IS_JERICHO(unit)) {
        last_lookup_regs[0] = PPDB_A_OEMB_LAST_LOOKUP_MASTERr;
        last_lookup_regs[1] = PPDB_A_OEMB_LAST_LOOKUP_SLAVEr;
        last_lookup_fields[0] = OEMB_LAST_LOOKUP_MASTER_RESULT_FOUNDf;
        last_lookup_fields[1] = OEMB_LAST_LOOKUP_SLAVE_FOUNDf  ;
        
        if (SOC_IS_QAX(unit) || SOC_IS_QUX(unit)) last_lookup_fields[1] = OEMB_LAST_LOOKUP_SLAVE_RESULT_FOUNDf;
        nof_last_lookup_regs = 2;
    } else {
        last_lookup_regs[0] = IHB_OEMB_LAST_LOOKUPr;
        last_lookup_fields[0] = OEMB_LAST_LOOKUP_RESULT_FOUNDf;
        nof_last_lookup_regs = 1;
    }

    for(lookup_reg_idx = 0; lookup_reg_idx < nof_last_lookup_regs; ++lookup_reg_idx) {
        res = soc_reg_get(unit, last_lookup_regs[lookup_reg_idx], REG_PORT_ANY, 0, &oemb_ll_reg);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        found = soc_reg64_field_get(unit, last_lookup_regs[lookup_reg_idx], oemb_ll_reg, last_lookup_fields[lookup_reg_idx]);
        if (COMPILER_64_LO(found)) {
            
            *val = 1;
            ARAD_DO_NOTHING_AND_EXIT;
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_diag_print_rx_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id

   )
{ 
    uint32
       res = SOC_SAND_OK;
    uint32 reg=0, opaque_pt_attr=0, next_header_is_not_itmh=0, recycle_port;
    SOC_PPC_DIAG_RECEIVED_PACKET_INFO rpi;
    soc_port_if_t port_type;
    int i, dir_printed=0, trap_code ;
#ifdef BCM_88470_A0
    soc_reg_t sticky_fields[] = {STICKY_PORT_ERR_PACKETf, STICKY_PORT_CHAN_TYPE_MISSf, STICKY_PORT_TYPE_MISSf, STICKY_PORT_EMC_MISSf, STICKY_PORT_MAID_MISSf,
        STICKY_PORT_FLEX_CRC_MISSf, STICKY_PORT_MDL_MISSf, STICKY_PORT_CCM_INTRVL_MISSf, STICKY_PORT_ERR_MY_DISC_MISSf, STICKY_PORT_ERR_SRC_IP_MISSf  ,STICKY_PORT_ERR_YOUR_DISC_MISSf,
        STICKY_PORT_SRC_PORT_MISSf , STICKY_PORT_BFD_FBITf, STICKY_PORT_BFD_PBITf, STICKY_PARITY_ERRORf, STICKY_PORT_TRAP_CODE_MISSf};
    char *stikcy_port_msg[] = {"error packet", "channel type miss", "type miss", "emc miss", "maid miss", "crc miss", "mdl miss", "ccm interval miss", "my discriminator miss","error src IP miss" , "error your discriminator miss"  ,
        "source port miss", "BFD f-bit", "BFD p-bit", "parity error", "trap miss"};
#else
    soc_reg_t sticky_fields[] = {STICKY_PORT_ERR_PACKETf, STICKY_PORT_TRAP_MISSf, STICKY_PORT_TYPE_MISSf, STICKY_PORT_EMC_MISSf, STICKY_PORT_MAID_MISSf,
    STICKY_PORT_MDL_MISSf, STICKY_PORT_CCM_INTRVL_MISSf, STICKY_PORT_ERR_MY_DISC_MISSf, STICKY_PORT_ERR_SRC_IP_MISSf  ,STICKY_PORT_ERR_YOUR_DISC_MISSf,
        STICKY_PORT_SRC_PORT_MISSf , STICKY_PORT_BFD_FBITf, STICKY_PORT_BFD_PBITf, STICKY_PARITY_ERRORf};
    char *stikcy_port_msg[] = {"error packet", "trap miss", "type miss", "emc miss", "maid miss", "mdl miss", "ccm interval miss", "my discriminator miss","error src IP miss" , "error your discriminator miss"  ,
        "source port miss", "BFD f-bit", "BFD p-bit", "parity error"};
#endif
    uint8 ll_found;
    ARAD_PP_DIAG_REG_FIELD fld;
    uint32 regs_val[ARAD_PP_DIAG_DBG_VAL_LEN] = {0};
    uint32 oam_trap_code, oam_trap_code_forward;
    char *sub_type = "", *trap_word;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

     if (SOC_IS_ARADPLUS(unit)) {
         
         sticky_fields[11] = STICKY_PORT_RMEP_STATE_CHANGEf;
         sticky_fields[12] = STICKY_PORT_PARITY_ERRORf;
         sticky_fields[13] = STICKY_PORT_TIMESTAMP_MISSf;

         stikcy_port_msg[11] = "RMEP state change";
         stikcy_port_msg[12] = "parity error";
         stikcy_port_msg[13] = "time stamp miss";
     }

    SOC_PPC_DIAG_RECEIVED_PACKET_INFO_clear(&rpi);
    soc_ppd_diag_received_packet_info_get(unit, core_id, &rpi, &res);
     if (soc_sand_update_error_code(res, &ex ) != no_err ) { 
        LOG_CLI((BSL_META_U(unit,
                            "Diagnostic Failed. Perhaps no packet was sent.\n")));
        goto exit;
    }

    
    res = soc_port_sw_db_interface_type_get(unit,rpi.in_tm_port, &port_type);
    SOC_SAND_CHECK_FUNC_RESULT(res,20,exit);

    if (port_type == SOC_PORT_IF_OAMP) { 
        
        
        SHR_BITCOPY_RANGE(&opaque_pt_attr, 0, rpi.packet_header.buff, 12, 3);
        
        SHR_BITCOPY_RANGE(&next_header_is_not_itmh, 0, rpi.packet_header.buff, 15, 3);
        LOG_DEBUG(BSL_LS_SOC_OAM,
                  (BSL_META_U(unit,
                              "The opaque pt-attribute taken from the PTCH header is %d, the opaque value is %d\n"),opaque_pt_attr,_ARAD_PP_OAM_PTCH_OPAQUE_VALUE));
    } 


    if ((port_type == SOC_PORT_IF_OAMP) &&  (opaque_pt_attr != _ARAD_PP_OAM_PTCH_OPAQUE_VALUE)) {
        
        reg = 0;
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_OAMP_VALIDITY_CHECK_STICKYr(unit, &reg));
        
        if (sizeof(sticky_fields) / sizeof(soc_reg_t) != sizeof(stikcy_port_msg) / sizeof(char *)) {
            LOG_ERROR(BSL_LS_SOC_OAM,
                      (BSL_META_U(unit,
                                  "Something went wrong with internal arrays\n")));
        }
        
        for (i = 0; i < (sizeof(sticky_fields) / sizeof(soc_reg_t)); ++i) {
            if (soc_reg_field_get(unit, OAMP_VALIDITY_CHECK_STICKYr, reg, sticky_fields[i])) {
                LOG_CLI((BSL_META_U(unit,
                                    "Packet was trapped to CPU due to %s.\n"), stikcy_port_msg[i]));
            }
        }
    } else {
        if (opaque_pt_attr == _ARAD_PP_OAM_PTCH_OPAQUE_VALUE) {
            LOG_CLI((BSL_META_U(unit,
                                "Packet was injected, ")));
            dir_printed = 1;
            if (next_header_is_not_itmh) {
                LOG_CLI((BSL_META_U(unit,
                                    "direction: up\n")));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "direction: down\n")));
            }
        } else { 
            recycle_port = soc_property_get(unit, spn_OAM_RCY_PORT, -1);
            if (rpi.in_tm_port == recycle_port) {
                LOG_CLI((BSL_META_U(unit,
                                    "Packet is Loopback\n")));
            } else if (port_type == SOC_PORT_IF_RCY) {
                LOG_CLI((BSL_META_U(unit,
                                    "Packet was mirrored => Direction is up.\n")));
            }
        }

        res = arad_pp_oam_last_lookup_found(unit, &ll_found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        regs_val[0] = 0;
        res = get_signal_for_trap_code(unit, &fld, core_id, regs_val);
        SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);
        trap_code = regs_val[0];


        if (ll_found) {
            res = get_signal_for_direction(unit, &fld, core_id, regs_val);
            SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

            if (regs_val[0] == 0) { 
                regs_val[0] = 0;
                res = get_signal_for_subtype_down(unit, &fld, core_id, regs_val);
                SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);
                sub_type_msg_from_code(unit, &sub_type, regs_val[0]);

                regs_val[0] = 0;
                res = get_signal_for_oam_id_down(unit, &fld, core_id, regs_val);
                SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);
                if (dir_printed == 0) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Direction is down, ")));
                }
                
                if ((trap_code == SOC_PPC_TRAP_CODE_TRAP_ETH_OAM) ||  (trap_code == SOC_PPC_TRAP_CODE_TRAP_Y1731_O_MPLS_TP) || (trap_code == SOC_PPC_TRAP_CODE_TRAP_BFDCV_O_MPLS_TP) ||
                    (trap_code == SOC_PPC_TRAP_CODE_TRAP_BFDCC_O_MPLS_TP) || (trap_code == SOC_PPC_TRAP_CODE_TRAP_BFD_O_PWE) || (trap_code == SOC_PPC_TRAP_CODE_TRAP_BFD_O_MPLS) ||
                    (trap_code == SOC_PPC_TRAP_CODE_TRAP_BFD_O_IPV4) || (trap_code == SOC_PPC_TRAP_CODE_TRAP_Y1731_O_PWE) || (trap_code == SOC_PPC_TRAP_CODE_TRAP_Y1731_O_MPLS_TP) ||
                    (trap_code == SOC_PPC_TRAP_CODE_OAM_LEVEL)) {

                    LOG_CLI((BSL_META_U(unit,
                                        "OAM ID is 0X%x. "), regs_val[0]));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                                        "packet sent from non accelerated end point, ID cannot be retrieved. ")));
                }

                LOG_CLI((BSL_META_U(unit,
                                    "OAM subtype is %s\n"),  sub_type));
            } else {
                regs_val[0] = 0;

                res = get_signal_for_subtype_up(unit, &fld, core_id, regs_val);
                SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
                sub_type_msg_from_code(unit, &sub_type, regs_val[0]);
                regs_val[0] = 0;
                res = get_signal_for_oam_id_up(unit, &fld, core_id, regs_val);
                SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);

                if (dir_printed == 0) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Direction is up, ")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "OAM subtype is %s.\n"),sub_type));
            }
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "Lookup not found: OAM subtype, ID cannot be retrieved.\n")));
        }

        if ((trap_code >= SOC_PPC_TRAP_CODE_USER_DEFINED_0 ) && (trap_code<=SOC_PPC_TRAP_CODE_USER_DEFINED_59)) {
            trap_word = "user defined";
        } else if ((trap_code >= SOC_PPC_TRAP_CODE_TRAP_ETH_OAM ) && (trap_code<=SOC_PPC_TRAP_CODE_OAM_LEVEL)) {
            trap_word = "oam";
        } else if (trap_code==SOC_PPC_TRAP_CODE_OAM_PASSIVE) {
            trap_word = "oam passive";
        } else if ((trap_code >= SOC_PPC_TRAP_CODE_PBP_SA_DROP_0 ) && (trap_code<=SOC_PPC_TRAP_CODE_PBP_SA_DROP_3)) {
            trap_word = "drop";
        } else {
            trap_word = "other";
        }
        

        LOG_CLI((BSL_META_U(unit,
                            "Trap code is %d (%s).\n"), trap_code, trap_word));
        regs_val[0] = 0;
        res = get_signal_for_oam_snoop_strength(unit, &fld, core_id, regs_val);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);
        if (regs_val[0] != 0) {
            LOG_CLI((BSL_META_U(unit,
                                "Packet was snooped.\n")));
        }
        res = _arad_pp_oam_trap_code_to_internal(unit,trap_code,&oam_trap_code);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
        
      res = sw_state_access[unit].dpp.soc.arad.pp.oam.trap_code_frwrd.get(unit, &oam_trap_code_forward);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);
      if (oam_trap_code == oam_trap_code_forward) {
          LOG_CLI((BSL_META_U(unit,
                              "Packet was forwarded.\n")));
      }

    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_diag_print_em_verify(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int LIF
   )
{
    uint32
       res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_ERR_IF_ABOVE_NOF(LIF, OAM_LIF_MAX_VALUE(unit), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 110, exit);

    ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}




uint32
  arad_pp_oam_diag_print_em_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int LIF
   )
{
    uint32
       res = SOC_SAND_OK;
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      oem1_key = {0};
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY      oem2_key = {0};
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  oem1_payload= {0};
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  oem2_payload= {0};
    int ingress,mdl,your_discr;
    uint8 found;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for (ingress=0; ingress<=1 ; ++ingress) {
        oem1_key.oam_lif = LIF;
        oem1_key.ingress = ingress;
          res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, &found);
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          _ARAD_PP_OAM_PRINT_OEM1_KEY(oem1_key);
          if (found) {
              _ARAD_PP_OAM_PRINT_OEM1_PAYLOAD(oem1_payload);
               if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
                  uint8 passive_active_enable;
                  res = arad_pp_oam_mep_passive_active_enable_get_unsafe(unit,  oem1_payload.mp_profile, &passive_active_enable);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
                  LOG_CLI((BSL_META_U(unit,
                                      "\t\tPassive active enable: %d\n"),  passive_active_enable)); 

              }
          } else {
               LOG_CLI((BSL_META_U(unit,
                              "\tEntry not found\n"))); 
          }
    }

    oem2_key.oam_lif = LIF;
    for (ingress=0 ; ingress<=1 ; ++ingress) {
        for (mdl = 0; mdl < 8; ++mdl) {
            for (your_discr=0 ; your_discr<=1; ++your_discr) {
                oem2_key.ingress = ingress;
                oem2_key.mdl = mdl;
                oem2_key.your_disc = your_discr;
                res = arad_pp_oam_classifier_oem2_entry_get_unsafe(unit, &oem2_key, &oem2_payload, &found);
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit); 
                if (found) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\tOEMB key: ingress: %d, Your-Discriminator: %d ,OAM LIF: 0x%x, MD-Level: %d\n"),  oem2_key.ingress, oem2_key.your_disc,oem2_key.oam_lif,oem2_key.mdl));
                    LOG_CLI((BSL_META_U(unit,
                                        "\t\tOEMB payload: acc-MP-profile: %d, OAM-ID: 0x%x\n"), oem2_payload.mp_profile, oem2_payload.oam_id));              
                }
            }
        }
    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


char *arad_pp_oam_diag_mp_type_to_mp_type_str[] = {
  "Match",
  "Above",
  "Below",
  "Between",
  "Invalid"
};

char *arad_plus_pp_oam_diag_mp_type_to_mp_type_str[] = {
  "MEP or Active match",
  "MP type below",
  "MIP or Passive match",
  "MP type above",
  "Invalid"
};

char *jericho_pp_oam_diag_mp_type_to_mp_type_str[] = {
   "MIP match" ,
   "Active match",
   "Passive match",
   "Below highest MEP",
   "Above all",
   "BFD",
   "Invalid"
};

char *qax_jer_plus_pp_oam_diag_mp_type_to_mp_type_str[] = {
   "MIP match" ,
   "Active match",
   "Passive match",
   "Between MEPs",
   "Below all",
   "Above all",
   "BFD",
   "Invalid"
};

char *arad_pp_oam_diag_mp_type_to_mp_type_str_get(uint8 mp_type)
{
    if (mp_type > SOC_PPC_OAM_MP_TYPE_COUNT) {
        return "invalid";
    }

    return arad_pp_oam_diag_mp_type_to_mp_type_str[mp_type];
}

char *arad_plus_pp_oam_diag_mp_type_to_mp_type_str_get(uint8 mp_type)
{
    if (mp_type > SOC_PPC_OAM_MP_TYPE_COUNT_PLUS) {
        return "invalid";
    }

    return arad_plus_pp_oam_diag_mp_type_to_mp_type_str[mp_type];
}

char *jericho_pp_oam_diag_mp_type_to_mp_type_str_get(uint8 mp_type)
{
    if (mp_type > SOC_PPC_OAM_MP_TYPE_JERICHO_COUNT) {
        return "invalid";
    }

    return jericho_pp_oam_diag_mp_type_to_mp_type_str[mp_type];
}

char *qax_jer_plus_pp_oam_diag_mp_type_to_mp_type_str_get(uint8 mp_type)
{
    if (mp_type > SOC_PPC_OAM_MP_TYPE_QAX_COUNT) {
        return "invalid";
    }

    return qax_jer_plus_pp_oam_diag_mp_type_to_mp_type_str[mp_type];
}

uint32
  arad_pp_oam_diag_mp_type_resolution_simulate(
     SOC_SAND_IN    int unit,
     SOC_SAND_IN    SOC_PPC_OAM_ACTION_KEY_PARAMS *key_params,
     SOC_SAND_IN    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload,
     SOC_SAND_OUT   SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY *act_key
  ) {

    uint32 res;
    uint32 mask;
    uint32 index;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    LOG_CLI((BSL_META_U(unit, "Entry found in OEM-1\n")));

    if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
        uint32 mdl_mp_type;
        uint32 mep_bitmap;
        uint32 not_above;
        uint32 not_below; 

        soc_reg_above_64_val_t cfg_mp_type_map = {0};

        
        mdl_mp_type = JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(oem1_payload->mp_type_vector, key_params->level);

        
        mep_bitmap = 0;
        for (mask = (1<<15); mask > 0; mask >>= 2) {
            mep_bitmap <<= 1;
            mep_bitmap |= ((oem1_payload->mp_type_vector) & mask) ? 1 : 0;
        }

        
        not_above = (mep_bitmap >> key_params->level) ? 1 : 0;

        
        not_below = (mep_bitmap & ((1<<(key_params->level))-1)) ? 1 : 0;

        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_OAM_MP_TYPE_MAPr(unit, SOC_CORE_DEFAULT, cfg_mp_type_map));

        
        index = 3 * (((key_params->bfd) << 3) | (mdl_mp_type << 1) | not_above );
        if (SOC_IS_JERICHO_PLUS(unit)) {
            
            index = 3 * (((key_params->bfd) << 4) | (not_below << 3) | (mdl_mp_type << 1) | not_above );
        }
        SHR_BITCOPY_RANGE(&(act_key->mp_type_jr), 0, cfg_mp_type_map, index, 3);
        if (SOC_IS_JERICHO_PLUS(unit)) {
            LOG_CLI((BSL_META_U(unit, "MP-Type: %d (%s)\n"),
                    act_key->mp_type_jr,
                    qax_jer_plus_pp_oam_diag_mp_type_to_mp_type_str_get(act_key->mp_type_jr))); 
        } else {
            LOG_CLI((BSL_META_U(unit, "MP-Type: %d (%s)\n"),
                    act_key->mp_type_jr,
                    jericho_pp_oam_diag_mp_type_to_mp_type_str_get(act_key->mp_type_jr))); 
        }
        LOG_CLI((BSL_META_U(unit, "OAM-1 keys: Inject[1b]=%u | My_CFM_Mac[1b]=%u | MSB of MP-Type[1b]=%u |  Opcode[4b]=%u | Ingr[1b]=%u | LSB bits of MP-Type[2b]=%u |  MP_Profile[4b]=%u\n"),
                    act_key->inject, act_key->my_cfm_mac, (act_key->mp_type_jr & 4) >> 2, act_key->opcode, act_key->ingress, (act_key->mp_type_jr & 3), act_key->mp_profile));
    }
    else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
        

        uint8 mip_0 = oem1_payload->mip_bitmap & 0x7;
        uint8 mip_1 = (oem1_payload->mip_bitmap >> 3) & 0x7;

        uint32 passive_active_en = 0;
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_OAM_MEP_PASSIVE_ACTIVE_ENABLEr(unit, &passive_active_en));

        if (passive_active_en & (1 << (oem1_payload->mp_profile))) {
            if (oem1_payload->mep_bitmap & (1 << (key_params->level))) {
                
                act_key->mp_type = 0; 
            }
            else if (((oem1_payload->mep_bitmap) & 0xFF) >> (key_params->level)) {
                
                act_key->mp_type = 1; 
            }
            else if (((key_params->level == mip_0) && mip_0)
                     || ((key_params->level == mip_1) && mip_1)){
                
                act_key->mp_type = 2; 
            }
            else {
                act_key->mp_type = 3; 
            }
        }
        else {
            if (oem1_payload->mep_bitmap & (1 << (key_params->level))) {
                
                act_key->mp_type = 0; 
            }
            else if (((key_params->level == mip_0) && mip_0)
                     || ((key_params->level == mip_1) && mip_1)){
                
                act_key->mp_type = 2; 
            }
            else if ((((oem1_payload->mep_bitmap) & 0xFF) >> (key_params->level))
                     || (key_params->level < mip_0)
                     || (key_params->level < mip_1)) {
                act_key->mp_type = 1; 
            }
            else {
                act_key->mp_type = 3; 
            }
        }
        LOG_CLI((BSL_META_U(unit, "MP-Type: %d (%s)\n"),
                    act_key->mp_type,
                    arad_plus_pp_oam_diag_mp_type_to_mp_type_str_get(act_key->mp_type))); 
        LOG_CLI((BSL_META_U(unit, "OAM-1 keys: Inject[1b]=%u | My_CFM_Mac[1b]=%u | Is_BFD[1b]=%u |  Opcode[4b]=%u | Ingr[1b]=%u | MP-Type[2b]=%u |  MP_Profile[4b]=%u\n"),
                    act_key->inject, act_key->my_cfm_mac, act_key->is_bfd, act_key->opcode, act_key->ingress, act_key->mp_type , act_key->mp_profile));
    }
    else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
        
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(
           "Adavnced classifier with multiple MEPs on LIF is not supported for this diag yet.")));
    }
    else {
#define __ARAD_SET_MP_TYPE__(mp_type, bitmap) \
        do { \
            if ((bitmap)&(1 << (key_params->level))) { \
                 \
                (mp_type) = 0;  \
            } \
            else if ((bitmap) == 0) { \
                 \
                (mp_type) = 1;  \
            } \
            else if ((((bitmap) & 0x7) >> (key_params->level - 1)) == 0) { \
                 \
                (mp_type) = 1;  \
            } \
            else if (((bitmap) & ((1 << key_params->level)-1)) == 0) { \
                 \
                (mp_type) = 2;  \
            } \
            else { \
                (mp_type) = 3;  \
            } \
        } while(0)

        __ARAD_SET_MP_TYPE__(act_key->mep_type, oem1_payload->mep_bitmap);
        __ARAD_SET_MP_TYPE__(act_key->mip_type, oem1_payload->mip_bitmap);
        LOG_CLI((BSL_META_U(unit, "MEP-Type: %d (%s)\n"),
                    act_key->mep_type,
                    arad_pp_oam_diag_mp_type_to_mp_type_str_get(act_key->mep_type))); 
        LOG_CLI((BSL_META_U(unit, "MIP-Type: %d (%s)\n"),
                    act_key->mip_type,
                    arad_pp_oam_diag_mp_type_to_mp_type_str_get(act_key->mip_type))); 
        LOG_CLI((BSL_META_U(unit, "OAM-1 keys: Inject[1b]=%u | My_CFM_Mac[1b]=%u | Is_BFD[1b]=%u |  Opcode[4b]=%u | Ingr[1b]=%u | MIP-Type[2b]=%u | MEP-Type[2b]=%u | MP_Profile[2b]=%u\n"),
                    act_key->inject, act_key->my_cfm_mac, act_key->is_bfd, act_key->opcode, act_key->ingress, act_key->mip_type , act_key->mep_type, act_key->mp_profile));

#undef __ARAD_SET_MP_TYPE__

    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  arad_pp_oam_diag_action_key_get_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_INOUT SOC_PPC_OAM_ACTION_KEY_PARAMS *key_params,
     SOC_SAND_OUT uint32 *key,
     SOC_SAND_OUT uint8  *found
  ) {
    uint32 res = SOC_SAND_OK;

    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY     oem1_key = {0};
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD oem1_payload = {0};

    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY     oem2_key = {0};
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD oem2_payload = {0};

    uint8 internal_opcode;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found = 0;

    
    res = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit,
                                    key_params->opcode, &internal_opcode);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    key_params->opcode = internal_opcode;

    
    oem2_key.oam_lif = key_params->lif;
    oem2_key.ingress = key_params->ing;
    oem2_key.mdl = key_params->level;
    oem2_key.your_disc = key_params->your_disc;
    res = arad_pp_oam_classifier_oem2_entry_get_unsafe(unit, &oem2_key, &oem2_payload, found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (*found) { 
        SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY act_key;
        SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY_clear(&act_key);

        
        act_key.ingress = key_params->ing;
        act_key.inject = key_params->inj;
        act_key.my_cfm_mac = key_params->mymac;
        act_key.is_bfd = key_params->bfd;
        act_key.mp_profile = oem2_payload.mp_profile;
        act_key.opcode = internal_opcode;

        
        _ARAD_PP_OAM_OAM2_KEY_STRUCT_TO_KEY(act_key, *key);

        LOG_CLI((BSL_META_U(unit, "Entry found in OEM-2\n")));
        LOG_CLI((BSL_META_U(unit, 
                        "OAM-2 keys : Inject[1b]=%u | My_CFM_Mac[1b]=%u | Is_BFD[1b]=%u | Opcode[4b]=%u | Ingr[1b]=%u | Acc_MP_Profile[4b]=%u\n"),
                    act_key.inject, act_key.my_cfm_mac, act_key.is_bfd,
                    act_key.opcode, act_key.ingress, act_key.mp_profile));

        *found = 2; 
    }
    else { 
        oem1_key.oam_lif = key_params->lif;
        oem1_key.ingress = key_params->ing;
        oem1_key.your_discr = key_params->your_disc;
        res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if (*found) {
            SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY act_key;
            SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&act_key);

            
            act_key.ingress = key_params->ing;
            act_key.inject = key_params->inj;
            act_key.my_cfm_mac = key_params->mymac;
            act_key.is_bfd = key_params->bfd;
            act_key.mp_profile = oem1_payload.mp_profile;
            act_key.opcode = internal_opcode;
            res = arad_pp_oam_diag_mp_type_resolution_simulate(unit, key_params, &oem1_payload, &act_key);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

            
            _arad_pp_oam_oam1_key_struct_to_key(unit, act_key, key);

            *found = 1; 
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_diag_print_ak_verify(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_OAM_ACTION_KEY_PARAMS *key_params
   ) {
    uint32
       res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(key_params);
    SOC_SAND_ERR_IF_ABOVE_MAX(key_params->level, (key_params->bfd ? 0 : 7), ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR, 10, exit);

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_diag_print_ak_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_OAM_ACTION_KEY_PARAMS *provided_key_params
   ){
    uint32 res = SOC_SAND_OK;

    SOC_PPC_OAM_ACTION_KEY_PARAMS key_params;

    uint32 entry;
    
    uint32 f_up_mep;
    uint32 f_sub_type;
    uint32 f_snoop_strength;
    uint32 f_reserve;
    uint32 f_parity;
    uint32 f_meter_disable;
    uint32 f_forwarding_strength;
    uint32 f_cpu_trap_code = 0;
    uint32 f_counter_disable;
    uint32 f_action;
    
    uint32 f_mirror_profile;
    uint32 f_forward_disable;
    
    uint32 f_mirror_strength;
    uint32 f_mirror_enable;
    uint32 f_mirror_command;
    uint32 f_fwd_strength;
    uint32 f_forward_enable;

    char* key_found[] = {"ACTION KEY NOT FOUND", "KEY FOUND FOR OAM-1", "KEY FOUND FOR OAM-2"};
    char extension1[20][128];

    uint32 key = -1; 
    uint8 found = 0;

    uint8 lif_found;
    uint8 lif_accessed;
    int local_lif;

    uint32 trap_code;

    int i;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    key_params.lif = provided_key_params->lif;
    key_params.level = provided_key_params->level;
    key_params.bfd = provided_key_params->bfd;
    key_params.opcode = provided_key_params->opcode;
    key_params.ing = provided_key_params->ing;
    key_params.inj = provided_key_params->inj;
    key_params.mymac = provided_key_params->mymac;
    key_params.your_disc = provided_key_params->your_disc;

    
	if (SOC_IS_JERICHO(unit) && !key_params.ing) {
        
        res = soc_jer_lif_glem_access_entry_by_key_get(unit,key_params.lif, &local_lif, &lif_accessed, &lif_found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

		if (lif_found) {
            
            key_params.lif = local_lif;
            LOG_CLI((BSL_META_U(unit, "Global lif 0x%0x was converted to local lif 0x%0x to deduce the key\n"), provided_key_params->lif, local_lif));
		}
	}

    res = arad_pp_oam_diag_action_key_get_unsafe(unit, &key_params, &key, &found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

	if (found) {
        
        if (found == 1) {
            if (SOC_IS_JERICHO(unit)) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IHP_OAMAm(unit, MEM_BLOCK_ANY, key, &entry));
                soc_IHP_OAMAm_field_get(unit, &entry, UP_MEPf, &f_up_mep);
                soc_IHP_OAMAm_field_get(unit, &entry, SUB_TYPEf, &f_sub_type);
                soc_IHP_OAMAm_field_get(unit, &entry, SNOOP_STRENGTHf, &f_snoop_strength);
                soc_IHP_OAMAm_field_get(unit, &entry, RESERVEf, &f_reserve);
                soc_IHP_OAMAm_field_get(unit, &entry, PARITYf, &f_parity);
                soc_IHP_OAMAm_field_get(unit, &entry, MIRROR_STRENGTHf, &f_mirror_strength);
                soc_IHP_OAMAm_field_get(unit, &entry, MIRROR_ENABLEf, &f_mirror_enable);
                soc_IHP_OAMAm_field_get(unit, &entry, MIRROR_COMMANDf, &f_mirror_command);
                soc_IHP_OAMAm_field_get(unit, &entry, METER_DISABLEf, &f_meter_disable);
                soc_IHP_OAMAm_field_get(unit, &entry, FWD_STRENGTHf, &f_fwd_strength);
                soc_IHP_OAMAm_field_get(unit, &entry, FORWARD_ENABLEf, &f_forward_enable);
                soc_IHP_OAMAm_field_get(unit, &entry, FORWARDING_STRENGTHf, &f_forwarding_strength);
                soc_IHP_OAMAm_field_get(unit, &entry, CPU_TRAP_CODEf, &f_cpu_trap_code);
                soc_IHP_OAMAm_field_get(unit, &entry, COUNTER_DISABLEf, &f_counter_disable);
                soc_IHP_OAMAm_field_get(unit, &entry, ACTIONf, &f_action);
            } else {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IHB_OAMAm(unit, MEM_BLOCK_ANY, key, &entry));
                soc_IHB_OAMAm_field_get(unit, &entry, UP_MEPf, &f_up_mep);
                soc_IHB_OAMAm_field_get(unit, &entry, SUB_TYPEf, &f_sub_type);
                soc_IHB_OAMAm_field_get(unit, &entry, SNOOP_STRENGTHf, &f_snoop_strength);
                soc_IHB_OAMAm_field_get(unit, &entry, RESERVEf, &f_reserve);
                soc_IHB_OAMAm_field_get(unit, &entry, PARITYf, &f_parity);
                soc_IHB_OAMAm_field_get(unit, &entry, MIRROR_PROFILEf, &f_mirror_profile);
                soc_IHB_OAMAm_field_get(unit, &entry, METER_DISABLEf, &f_meter_disable);
                soc_IHB_OAMAm_field_get(unit, &entry, FORWARD_DISABLEf, &f_forward_disable);
                soc_IHB_OAMAm_field_get(unit, &entry, FORWARDING_STRENGTHf, &f_forwarding_strength);
                soc_IHB_OAMAm_field_get(unit, &entry, CPU_TRAP_CODEf, &f_cpu_trap_code);
                soc_IHB_OAMAm_field_get(unit, &entry, COUNTER_DISABLEf, &f_counter_disable);
                soc_IHB_OAMAm_field_get(unit, &entry, ACTIONf, &f_action);
            }
        } else {
            if (SOC_IS_JERICHO(unit)) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IHP_OAMBm(unit, MEM_BLOCK_ANY, key, &entry));
                soc_IHP_OAMBm_field_get(unit, &entry, UP_MEPf, &f_up_mep);
                soc_IHP_OAMBm_field_get(unit, &entry, SUB_TYPEf, &f_sub_type);
                soc_IHP_OAMBm_field_get(unit, &entry, SNOOP_STRENGTHf, &f_snoop_strength);
                soc_IHP_OAMBm_field_get(unit, &entry, RESERVEDf, &f_reserve);
                soc_IHP_OAMBm_field_get(unit, &entry, PARITYf, &f_parity);
                soc_IHP_OAMBm_field_get(unit, &entry, MIRROR_STRENGTHf, &f_mirror_strength);
                soc_IHP_OAMBm_field_get(unit, &entry, MIRROR_ENABLEf, &f_mirror_enable);
                soc_IHP_OAMBm_field_get(unit, &entry, MIRROR_COMMANDf, &f_mirror_command);
                soc_IHP_OAMBm_field_get(unit, &entry, METER_DISABLEf, &f_meter_disable);
                soc_IHP_OAMBm_field_get(unit, &entry, FWD_STRENGTHf, &f_fwd_strength);
                soc_IHP_OAMBm_field_get(unit, &entry, FORWARD_ENABLEf, &f_forward_enable);
                soc_IHP_OAMBm_field_get(unit, &entry, FORWARDING_STRENGTHf, &f_forwarding_strength);
                soc_IHP_OAMBm_field_get(unit, &entry, CPU_TRAP_CODEf, &f_cpu_trap_code);
                soc_IHP_OAMBm_field_get(unit, &entry, COUNTER_DISABLEf, &f_counter_disable);
                soc_IHP_OAMBm_field_get(unit, &entry, ACTIONf, &f_action);
            } else {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IHB_OAMBm(unit, MEM_BLOCK_ANY, key, &entry));
                soc_IHB_OAMBm_field_get(unit, &entry, UP_MEPf, &f_up_mep);
                soc_IHB_OAMBm_field_get(unit, &entry, SUB_TYPEf, &f_sub_type);
                soc_IHB_OAMBm_field_get(unit, &entry, SNOOP_STRENGTHf, &f_snoop_strength);
                soc_IHB_OAMBm_field_get(unit, &entry, RESERVEDf, &f_reserve);
                soc_IHB_OAMBm_field_get(unit, &entry, PARITYf, &f_parity);
                soc_IHB_OAMBm_field_get(unit, &entry, MIRROR_PROFILEf, &f_mirror_profile);
                soc_IHB_OAMBm_field_get(unit, &entry, METER_DISABLEf, &f_meter_disable);
                soc_IHB_OAMBm_field_get(unit, &entry, FORWARD_DISABLEf, &f_forward_disable);
                soc_IHB_OAMBm_field_get(unit, &entry, FORWARDING_STRENGTHf, &f_forwarding_strength);
                soc_IHB_OAMBm_field_get(unit, &entry, CPU_TRAP_CODEf, &f_cpu_trap_code);
                soc_IHB_OAMBm_field_get(unit, &entry, COUNTER_DISABLEf, &f_counter_disable);
                soc_IHB_OAMBm_field_get(unit, &entry, ACTIONf, &f_action);
            }
        }

        
        if (!key_params.ing) {
            
            if (SOC_IS_JERICHO(unit)) {
                sal_sprintf(extension1[3],"            * Ingress fields are not shown");
                sal_sprintf(extension1[4],"             _________");
                sal_sprintf(extension1[5],"            |         |---FORWARD-ENABLE------(0x%08x)->",f_forward_enable);
                sal_sprintf(extension1[6],"            |         |---MIRROR-COMMAND------(0x%08x)->",f_mirror_command);
                sal_sprintf(extension1[7],"            |         |---MIRROR-ENABLE-------(0x%08x)->",f_mirror_enable);
                sal_sprintf(extension1[8],"            |         |---FWD-STRENGTH--------(0x%08x)->",f_fwd_strength);
                sal_sprintf(extension1[9],"            |         |---MIRROR-STRENGTH-----(0x%08x)->",f_mirror_strength);
            } else {
                sal_sprintf(extension1[3]," ");
                sal_sprintf(extension1[4]," ");
                sal_sprintf(extension1[5]," ");
                sal_sprintf(extension1[6],"            * Ingress fields are not shown");
                sal_sprintf(extension1[7],"             _________");
                sal_sprintf(extension1[8],"            |         |---FORWARD-DISABLE-----(0x%08x)->",f_forward_disable);
                sal_sprintf(extension1[9],"            |         |---MIRROR-PROFILE------(0x%08x)->",f_mirror_profile);
            }
		} else {
            
            sal_sprintf(extension1[3],    "            * Egress fields are not shown");
            sal_sprintf(extension1[4],    "             _________");
            sal_sprintf(extension1[5],    "            |         |---CPU-TRAP-CODE-------(0x%08x)->",f_cpu_trap_code);
            sal_sprintf(extension1[6],    "            |         |---FORWARDING-STRENGTH-(0x%08x)->",f_forwarding_strength);
            sal_sprintf(extension1[7],    "            |         |---SNOOP-STRENGTH------(0x%08x)->",f_snoop_strength);
            sal_sprintf(extension1[8],    "            |         |---UP-MEP--------------(0x%08x)->",f_up_mep);
            sal_sprintf(extension1[9],    "            |         |---METER-DISABLE-------(0x%08x)->",f_meter_disable);
        }
        
        sal_sprintf(extension1[10],      "       +--->|  OAM-%d  |---ACTION--------------(0x%08x)->",found,f_action);
        sal_sprintf(extension1[11],       "       |    |         |---RESERVE-------------(0x%08x)->",f_reserve);
        sal_sprintf(extension1[12],       "       |    |         |---SUB-TYPE------------(0x%08x)->",f_sub_type);
        sal_sprintf(extension1[13],       "       |    |         |---COUNTER-DISABLE-----(0x%08x)->",f_counter_disable);
        sal_sprintf(extension1[14],       "       |    |_________|---PARITY--------------(0x%08x)->",f_parity);
        sal_sprintf(extension1[15],       "       |");
        sal_sprintf(extension1[16],       "       |___________________");
        sal_sprintf(extension1[17],       "                           |");
        sal_sprintf(extension1[18],                            "      |");
        sal_sprintf(extension1[19],                                "  |");
    } else {
        for (i=3;i<=19;i++) {
            sal_sprintf(extension1[i], " ");
        }
    }

    
    LOG_CLI((BSL_META_U(unit,
"\n\n                               __________\
\n                              |          |\
\n  --INGR--------(0x%08x)->|          |----->|%s\
\n  --YOUR-DISC---(0x%08x)->|  O-EM-1  |      |%s\
\n  --OAM-LIF-----(0x%08x)->|          |      |%s\
\n                              |__________|      |%s\
\n                                                |%s\
\n  --MDL---------(0x%08x)------------------->|%s"),
    key_params.ing,extension1[3],key_params.your_disc,extension1[4],key_params.lif,
    extension1[5],extension1[6],extension1[7],key_params.level,extension1[8]));
    LOG_CLI((BSL_META_U(unit,
"\n  --MY-CFM-MAC--(0x%08x)------------------->|%s\
\n  --PCKT-IS-BFD-(0x%08x)------------------->|%s\
\n  --OPCODE------(0x%08x)------------------->|%s\
\n  --INJECT------(0x%08x)------------------->|%s\
\n  --INGR--------(0x%08x)------------------->|%s"),
    key_params.mymac,extension1[9],key_params.bfd,extension1[10],key_params.opcode,
    extension1[11],key_params.inj,extension1[12],key_params.ing,extension1[13]));
    LOG_CLI((BSL_META_U(unit,
"\n                               __________       |%s\
\n  --MDL---------(0x%08x)->|          |      |%s\
\n  --INGR--------(0x%08x)->|          |----->|%s\
\n  --YOUR-DISC---(0x%08x)->|  O-EM-2  |      |%s\
\n  --OAM-LIF-----(0x%08x)->|          |      |  %s%s\
\n                              |__________|      |  0x%08x (%10d)%s\
\n                                                +---------------------------+\n\n"),
    extension1[14],key_params.level,extension1[15],key_params.ing,extension1[16],
    key_params.your_disc,extension1[17],key_params.lif,key_found[found],
    extension1[18],key,key,extension1[19]));

    
    if (!key_params.ing) {
        
        
        if (SOC_IS_JERICHO(unit)) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ANY, f_mirror_command, &entry));
        } else {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ANY, f_mirror_profile, &entry));
        }
        soc_IHP_RECYCLE_COMMANDm_field_get(unit, &entry, CPU_TRAP_CODEf, &trap_code);

        LOG_CLI((BSL_META_U(unit, "Trap code is 0x%0x (deduced by mirror profile)\n"), trap_code));
    } else {
        
        trap_code = f_cpu_trap_code;
        LOG_CLI((BSL_META_U(unit, "Trap code is 0x%0x\n"), trap_code));
    }
    LOG_CLI((BSL_META_U(unit, "\n")));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_diag_print_ks_verify(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_OAM_KEY_SELECT_PARAMS *key_params
   ) {
    uint32
       res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_IS_QAX(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Device not supported")));
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_diag_print_ks_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_OAM_KEY_SELECT_PARAMS *provided_key_params
   ){
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_val;
    uint32 payload = 0;
    uint8 key_in;

    uint8 oam_inner_key_select,
          oam_outer_key_select,
          mp_profile_set,
          oam_key_your_disc_inner,
          oam_key_your_disc_outer;

    char* key_select[] = {"OAM-LIF-Inner", "OAM-LIF-Outer", "OAM-LIF", "NULL"};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_val);

    if (provided_key_params->ing) {
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IHP_OAM_INGRESS_KEY_SELECTr(unit, reg_val));

        
        key_in = (provided_key_params->olo<<4) | (provided_key_params->oli<<3) | (provided_key_params->leo<<2) |
                 (provided_key_params->lei<<1) | provided_key_params->ydv;
    } else {
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IHP_OAM_EGRESS_KEY_SELECTr(unit, reg_val));

        
        key_in = (provided_key_params->olo<<4) | (provided_key_params->oli<<3) | (provided_key_params->pio<<2) |
                 (provided_key_params->cp<<1) | provided_key_params->inj;
    }

    
    SHR_BITCOPY_RANGE(&payload, 0, reg_val, key_in*7, 7);

    
    oam_inner_key_select    = (payload>>5);
    oam_outer_key_select    = (payload>>3)&3;
    mp_profile_set          = (payload>>2)&1;
    oam_key_your_disc_inner = (payload>>1)&1;
    oam_key_your_disc_outer = (payload)&1;

    
    if (provided_key_params->ing) {
        LOG_CLI((BSL_META_U(unit,
"\n Ingress key selection for O-EM1:\
\n OAM-LIF-Outer-Valid        = %d          OAM-Hirarchical-Inner-Key-sel = %d (%s)\
\n OAM-LIF-Inner-Valid        = %d          OAM-Hirarchical-Outer-Key-sel = %d (%s)\
\n LIF-Equal-To-OAM-LIF-Outer = %d   ===>   Mp-Profile-sel                = %d\
\n LIF-Equal-To-OAM-LIF-Inner = %d          OAM-Key-Your-Disc-Inner       = %d\
\n Your-Disc-Valid            = %d          OAM-Key-Your-Disc-Outer       = %d\n\n"),
        provided_key_params->olo,oam_inner_key_select,key_select[oam_inner_key_select],
        provided_key_params->oli,oam_outer_key_select,key_select[oam_outer_key_select],
        provided_key_params->leo,mp_profile_set,
        provided_key_params->lei,oam_key_your_disc_inner,
        provided_key_params->ydv,oam_key_your_disc_outer));
    } else {
        LOG_CLI((BSL_META_U(unit,
"\n Egress key selection for O-EM1:\
\n OAM-LIF-Outer-Valid = %d          OAM-Hirarchical-Inner-Key-sel = %d (%s)\
\n OAM-LIF-Inner-Valid = %d          OAM-Hirarchical-Outer-Key-sel = %d (%s)\
\n Packet-Is-OAM       = %d   ===>   Mp-Profile-sel                = %d\
\n Counter-Ptr-Valid   = %d          OAM-Key-Your-Disc-Inner       = %d\
\n Injected            = %d          OAM-Key-Your-Disc-Outer       = %d\n\n"),
        provided_key_params->olo,oam_inner_key_select,key_select[oam_inner_key_select],
        provided_key_params->oli,oam_outer_key_select,key_select[oam_outer_key_select],
        provided_key_params->pio,mp_profile_set,
        provided_key_params->cp,oam_key_your_disc_inner,
        provided_key_params->inj,oam_key_your_disc_outer));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_diag_print_oamp_counter_verify(
     SOC_SAND_IN int unit
   )
{
    uint32
       res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_diag_print_oamp_counter_unsafe(
     SOC_SAND_IN int unit
   )
{
    uint32
       res = SOC_SAND_OK;
    uint32 reg_val;
    uint32 rx_counter;
    uint32 tx_counter;
        
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if (SOC_IS_QAX(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_REG_016Ar(unit, &reg_val));
        rx_counter = soc_reg_field_get(unit, OAMP_REG_016Ar, reg_val, FIELD_0_15f);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_REG_016Br(unit, &reg_val));
        tx_counter = soc_reg_field_get(unit, OAMP_REG_016Br, reg_val, FIELD_0_15f);
    }
    else if (SOC_IS_JERICHO(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_REG_0168r_REG32(unit, &reg_val));
        rx_counter = soc_reg_field_get(unit, OAMP_REG_0168r, reg_val, FIELD_0_15f);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_REG_0169r(unit, &reg_val));
        tx_counter = soc_reg_field_get(unit, OAMP_REG_0169r, reg_val, FIELD_0_15f);
    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_REG_0154r(unit, &reg_val));
        rx_counter = soc_reg_field_get(unit, OAMP_REG_0154r, reg_val, FIELD_0_15f);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_REG_0155r(unit, &reg_val));
        tx_counter = soc_reg_field_get(unit, OAMP_REG_0155r, reg_val, FIELD_0_15f);
    }
    
    LOG_CLI((BSL_META_U(unit,
                              "Rx: %d\n"),  rx_counter));
    LOG_CLI((BSL_META_U(unit,
                              "Tx: %d\n"),  tx_counter));        

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_diag_print_debug_verify(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int mode
   )
{
    uint32
       res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_ERR_IF_OUT_OF_RANGE(mode, 0, 1, ARAD_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_diag_print_debug_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int cfg,
     SOC_SAND_IN int mode
   )
{
    uint32
       res = SOC_SAND_OK;
    int core;
    uint32 oam_id_arad_mode; 
    uint64 reg_val, field_oam_id_arad_mode;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if (SOC_IS_QAX(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("access to registers should be fixed for QAX at places we used _REG(32|64) access routines")));
    }
    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("access to registers should be fixed for Jericho Plus at places we used _REG(32|64) access routines")));
    }


    if (SOC_IS_JERICHO(unit)) {
        if (!cfg) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_FLP_GENERAL_CFGr_REG64(unit, 0, &reg_val));
            field_oam_id_arad_mode = soc_reg64_field_get(unit, IHP_FLP_GENERAL_CFGr, reg_val, OAM_ID_ARAD_MODEf);
            COMPILER_64_TO_32_LO(oam_id_arad_mode, field_oam_id_arad_mode);
            LOG_CLI((BSL_META_U(unit,
                                      "OAM ID ARAD mode: %d\n"),  oam_id_arad_mode));
        } else {
            for (core = 0; core <= 1; core++) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_FLP_GENERAL_CFGr_REG64(unit, core, &reg_val));
                oam_id_arad_mode = (uint32)mode;   
                COMPILER_64_SET(field_oam_id_arad_mode, 0, oam_id_arad_mode);     
                soc_reg64_field_set(unit, IHP_FLP_GENERAL_CFGr, &reg_val, OAM_ID_ARAD_MODEf, field_oam_id_arad_mode);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_FLP_GENERAL_CFGr_REG64(unit, core, reg_val));
            }
        }           
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_diag_print_oam_id_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  arad_pp_oam_diag_print_oam_id_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id
   )
{ 
    uint32
       res = SOC_SAND_OK;
    uint32 ret_val = 0;
    SOC_PPC_DIAG_RECEIVED_PACKET_INFO rpi;
    uint32 offset_byte;
    uint32 offset_word;    
    uint32 offset_bit;
    uint32 pph_type = 0;    
    uint32 dsp_ext_present = 0;
    uint32 oam_ts_type = 0;
    uint32 mep_type = 0;
    uint32 fhei_size = 0;
    uint32 fwd_code = 0;
    uint32 rpi_found = 0;
    uint8 ll_found;
    ARAD_PP_DIAG_REG_FIELD fld;
    uint32 oam_id = 0;
    uint32 regs_val[ARAD_PP_DIAG_DBG_VAL_LEN] = {0};    
    uint64 reg_val64;
    uint32 oam_id_arad_mode = 0;
    uint64 field_oam_id_arad_mode;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    COMPILER_64_SET(field_oam_id_arad_mode, 0, 0);

    if (SOC_IS_QAX(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("access to registers should be fixed for QAX at places we used _REG(32|64) access routines")));
    }
    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("access to registers should be fixed for Jericho Plus at places we used _REG(32|64) access routines")));
    }

      
    res = arad_pp_oam_last_lookup_found(unit, &ll_found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (ll_found) {
        res = get_signal_for_direction(unit, &fld, core_id, regs_val);
        SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);
        if (regs_val[0] == 0) { 
            regs_val[0] = 0;
            res = get_signal_for_oam_id_down(unit, &fld, core_id, regs_val);
            SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
            oam_id = regs_val[0];
        } 
    }

     
    res = soc_ppd_diag_received_packet_info_get(unit,core_id,&rpi,&ret_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (soc_sand_update_error_code(ret_val, &ex ) != no_err ) { 
        LOG_CLI((BSL_META_U(unit,
                            "Diagnostic Failed. Perhaps no packet was sent.\n")));
        goto exit;
    }

    offset_byte = SOC_PPC_DIAG_BUFF_MAX_SIZE * 4;
    _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_FTMH_BASE_SIZE, offset_byte, offset_word, offset_bit);    
    SHR_BITCOPY_RANGE(&pph_type, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 25, 2);
    SHR_BITCOPY_RANGE(&dsp_ext_present, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 3, 1);
    if (pph_type == 3) {         
        if (SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_stacking_ext_mode) {
            _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_FTMH_STACKING_EXTENSION_SIZE, offset_byte, offset_word, offset_bit); 
        }
        if (SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_lb_ext_mode) {
            _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_FTMH_LB_EXTENSION_SIZE, offset_byte, offset_word, offset_bit); 
        }
        if (dsp_ext_present) {
            _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_FTMH_DSP_EXTENSION_SIZE, offset_byte, offset_word, offset_bit); 
        }
        _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_OAM_TS_SIZE, offset_byte, offset_word, offset_bit); 
        SHR_BITCOPY_RANGE(&oam_ts_type, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 46, 2);
        SHR_BITCOPY_RANGE(&mep_type, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 42, 1);
        if (!oam_ts_type && mep_type) { 
            _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_PPH_BASE_SIZE, offset_byte, offset_word, offset_bit); 
            SHR_BITCOPY_RANGE(&fhei_size, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 52, 2);
            SHR_BITCOPY_RANGE(&fwd_code, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 48, 4);
            if ((fhei_size == 1) && (fwd_code == 7)) { 
                _ARAD_PP_OAM_PACKET_OFFSET_SET(_ARAD_PP_OAM_FHEI_3B_SIZE, offset_byte, offset_word, offset_bit); 
                oam_id = 0;
                SHR_BITCOPY_RANGE(&oam_id, 0, &(rpi.packet_header.buff[offset_word]), offset_bit + 8, 16); 
                rpi_found = 1;
            }
        }
    } 

    if (ll_found || rpi_found ) { 
        LOG_CLI((BSL_META_U(unit,
                                "OAM ID is 0x%x.\n"), oam_id));
        
        if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_FLP_GENERAL_CFGr_REG64(unit, 0, &reg_val64));
            field_oam_id_arad_mode = soc_reg64_field_get(unit, IHP_FLP_GENERAL_CFGr, reg_val64, OAM_ID_ARAD_MODEf);
            COMPILER_64_TO_32_LO(oam_id_arad_mode, field_oam_id_arad_mode);
        }

        
        if (SOC_IS_JERICHO(unit) && !oam_id_arad_mode &&
            _ARAD_PP_OAM_IS_OEM1_ENTRY(oam_id)) {
            SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY oam1_key;

            SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);

            _arad_pp_oam_oam1_key_to_key_struct (unit, oam_id, &oam1_key);

            LOG_CLI((BSL_META_U(unit,
                        "%16s: %d.\n"), "MEP-Profile (4)", oam1_key.mp_profile));
            LOG_CLI((BSL_META_U(unit,
                            "%16s: %d.\n"), "MP-Type (3)", oam1_key.mp_type_jr));
            LOG_CLI((BSL_META_U(unit,
                        "%16s: %d.\n"), "Ingress (1)", oam1_key.ingress));
            LOG_CLI((BSL_META_U(unit,
                        "%16s: %d.\n"), "OAM-Opcode (4)", oam1_key.opcode));
            LOG_CLI((BSL_META_U(unit,
                        "%16s: %d.\n"), "My-CFM-MAC (1)", oam1_key.my_cfm_mac));
            LOG_CLI((BSL_META_U(unit,
                        "%16s: %d.\n"), "Inject (1)", oam1_key.inject));              
        }
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "Lookup not found: OAM subtype, ID cannot be retrieved.\n")));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_diag_print_tcam_entries_verify(
     SOC_SAND_IN int unit
   ) {
    uint32
       res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_diag_print_hw_tcam_entries_verify(
     SOC_SAND_IN int unit
   ) {
    uint32
       res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

STATIC
  uint32
    arad_pp_oam_tcam_entry_print(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  int                          core_id
  )
{
    int    res;
    uint32 mask, val = 0;
    uint32 gal_begin_bit = 0;
    uint32 cfm_begin_bit = 0;
    uint32 ip_begin_bit = 0;
    uint32 tcam_entry_key_header_start;
    uint32 ether_type_begin_bit = 0;
    uint32 offset = 0;
    uint32 field_size = 0;
    ARAD_PP_DIAG_REG_FIELD fld;
    uint32 ttc = 0, forward_code = 0;
    uint32 tcam_key_val[ARAD_TCAM_ENTRY_MAX_LEN] = {0};
    uint32 total_tcam_size = _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = get_signal_for_oam_tcam_key_all(unit, &fld, core_id, tcam_key_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);

    offset = total_tcam_size;
    SHR_BITCOPY_RANGE(&ttc,0,tcam_key_val,(offset-4),4);

    offset -= 4;
    SHR_BITCOPY_RANGE(&forward_code,0,tcam_key_val,(offset-4),4);

    if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "y1711_enabled",0) == 1) ||
        (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable",0) == 1)) {
        
        mask=0;
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |         Tunnel-Termination-Code         |              Mask             |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                     %d                   |             0x%08x        |\n\r"),
                            ttc, mask));
 
        
        mask = 0xFFFFFFFF; 
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |             Forwarding-Code             |              Mask             |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                            forward_code, mask));

        
        offset -= 4;
        field_size = SOC_IS_JERICHO_PLUS(unit) ? 4 : 3;
        val = 0;
        SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-field_size),field_size);
        mask = 0;
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |              In-LIF-Profile             |              Mask             |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                            val, mask));

        
        offset -= field_size;
        val = 0;
        SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-1),1);
        mask = 0xFFFFFFFF;
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |            VTT-OAM-LIF-Valid            |              Mask             |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                            val, mask));

        offset -= 1;
 
        if(SOC_IS_JERICHO_PLUS(unit)){
            
            val = 0;
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-1),1);
            mask = 0xFFFFFFFF;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |          VTT-OAM-LIF-Valid Outer        |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                val, mask));

            offset -= 1;
        }

        offset -= (SOC_IS_JERICHO_PLUS(unit) ? 22 : 20);
        

        
        SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-12),12);
        mask = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable",0) == 1) ? 0x100 : 0xFFFFFFFF;
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |          Y1711-LM-Match-Label           |              Mask             |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                     0x%03x               |           0x%08x          |\n\r"),
                            val, mask));

        offset -= 12;
        val = 0;
        SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-8),8);
        mask = 0xFFFFFFFF;
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |           Y1711-FUNCTION-TYPE           |              Mask             |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                   0x%02x                  |           0x%08x          |\n\r"),
                            val, mask));

    } else {
        
        if ((((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_downmep_pwe_classification", 0) == 1) && (SOC_IS_ARADPLUS_AND_BELOW(unit))))
            &&  forward_code == _ARAD_PP_OAM_FWD_CODE_ETHERNET && ttc >= SOC_PPC_PKT_TERM_TYPE_MPLS_ETH && ttc <= SOC_PPC_PKT_TERM_TYPE_CW_MPLS3_ETH) {
            uint32 ether_type_begin_bit;

            mask = 0xFFFFFFFE;  
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |         Tunnel-Termination-Code         |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |             0x%08x        |\n\r"),
                                ttc, mask));

            
            mask = 0xFFFFFFFF; 
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |             Forwarding-Code             |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                forward_code, mask));


            
            offset -= 4;
            field_size = SOC_IS_JERICHO_PLUS(unit) ? 4 : 3;
            val = 0;
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-field_size),field_size);
            mask = 0;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |              In-LIF-Profile             |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                val, mask));

            
            offset -= field_size;
            val = 0;
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-1),1);
            mask = 0;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |            VTT-OAM-LIF-Valid            |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                            val, mask));

            offset -= 1;

            if(SOC_IS_JERICHO_PLUS(unit)){
                
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-1),1);
                mask = 0;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |          VTT-OAM-LIF-Valid Outer        |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= 1;
            }
            tcam_entry_key_header_start = offset;
            ether_type_begin_bit = tcam_entry_key_header_start + (12 * 8);
            cfm_begin_bit =  ether_type_begin_bit + 2*8 ;
            field_size = 16;

            offset = total_tcam_size - (ether_type_begin_bit + (SOC_IS_JERICHO_PLUS(unit) ? 18:16));
            val = 0;
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-16),16);

            mask = 0xFFFFFFFF;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                Ether-Type               |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                val, mask));

            offset = total_tcam_size - (cfm_begin_bit + 3);
            val = 0;
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-3),3);

            mask = 0xFFFFFFFF;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                   MDL                   |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                val, mask));
        } else {
            mask = (ttc == SOC_PPC_NOF_PKT_TERM_TYPES) ? 0 : 0xFFFFFFFE;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |         Tunnel-Termination-Code         |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |             0x%08x        |\n\r"),
                                ttc, mask));

            mask = 0xFFFFFFFF; 
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |             Forwarding-Code             |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                forward_code, mask));


            offset -= 4;
            val = 0;
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-3),3);
            mask = 0;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |              In-LIF-Profile             |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                val, mask));

            offset -= 3;
            val = 0;
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-1),1);
            mask = 0;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |            VTT-OAM-LIF-Valid            |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                val, mask));

            tcam_entry_key_header_start = 12;

            if (forward_code== _ARAD_PP_OAM_FWD_CODE_IPV4_UC) {
                ip_begin_bit = tcam_entry_key_header_start; 
            } else {
                switch (ttc) {
                case SOC_PPC_NOF_PKT_TERM_TYPES:
                    ether_type_begin_bit = tcam_entry_key_header_start + 12 * 8;
                    cfm_begin_bit = tcam_entry_key_header_start + 14 * 8;
                    break;
                case SOC_PPC_PKT_TERM_TYPE_MPLS_ETH:
                case SOC_PPC_PKT_TERM_TYPE_CW_MPLS_ETH:
                    gal_begin_bit = tcam_entry_key_header_start + 4 * 8; 
                    break;
                case SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH:
                case SOC_PPC_PKT_TERM_TYPE_CW_MPLS2_ETH:
                    gal_begin_bit = tcam_entry_key_header_start + 2 * 4 * 8;
                    break;
                default:
                    gal_begin_bit = tcam_entry_key_header_start;
                }
            }

            if (ip_begin_bit > 0) {
                
                offset = total_tcam_size - ip_begin_bit;
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-4),4);

                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |               IP-Version                |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= 4;
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-4),4);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                 IP-IHL                  |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= 64;
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-8),8);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                 IP-TTL                  |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= 8;
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-8),8);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |               IP-Protocol               |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= 56;
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-8),8);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                 IP-DIP                  |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= (24 + 2);  
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-2),2);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |           UDP.Source-Prot-Msbs          |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= 30;  
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-16),16);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |              UDP.Dest-Port              |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));
            } else if (cfm_begin_bit > 0) { 
                offset = total_tcam_size - ether_type_begin_bit - 16;
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-16),16);
                mask = 0xFFFFFFFF; 
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                Ether-Type               |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset = total_tcam_size - cfm_begin_bit - 3;
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-3),3);
                mask = 0xFFFFFFFF; 
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                    MDL                  |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset = total_tcam_size - cfm_begin_bit - 8;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-5),5);
                mask = _ARAD_PP_OAM_TCAM_ENTRY_BUILD_SET_MASK_FOR_VERSION_BY_OPCODE(val);
                val = 0; 
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                 Version                 |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset = total_tcam_size - cfm_begin_bit - 16;
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-16),16);
                mask = (val == (SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT - 1)) ? 0 : 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                  OpCode                 |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));
            } else { 
                offset = total_tcam_size - gal_begin_bit;
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-20),20);
                mask = 0xFFFFFFFF; 
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                MPLS-Label               |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset = total_tcam_size - gal_begin_bit - 24;
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-8),8);
                mask = 0; 
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |        MPLS label with all the rest     |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset = total_tcam_size - gal_begin_bit - 32;
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-16),16);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |           G-Ach, version,Reserved       |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset = total_tcam_size - gal_begin_bit - 48;
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-16),16);
                mask = 0xFFFFFFFF; 
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                Channel-type             |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset = total_tcam_size - gal_begin_bit - 64;
                val = 0;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-3),3);

                if (val == _ARAD_PP_OAM_Y1731_DEFAULT_MDLEVEL) {
                    mask = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0) ? 0 : 0xFFFFFFFF; 
                    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     MDL                 |              Mask             |\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                        val, mask));

                    offset = total_tcam_size - gal_begin_bit - 67;
                    val = 0;
                    SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-5),5);
                    mask = _ARAD_PP_OAM_TCAM_ENTRY_BUILD_SET_MASK_FOR_VERSION_BY_OPCODE(val);
                    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                   Version               |              Mask             |\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                        val, mask));

                    offset = total_tcam_size - gal_begin_bit - 72;
                    val = 0;
                    SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-8),8);
                    mask = (val == (SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT - 1)) ? 0 : 0xFFFFFFFF;
                    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                   OpCode                |              Mask             |\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                        val, mask));
                }
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

STATIC
  uint32
    arad_pp_oam_diag_print_tcam_action_unsafe(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32 action_match,
      SOC_SAND_IN ARAD_TCAM_ACTION *action
   )
{
    uint32 res;
    SOC_PPC_OAM_TCAM_ENTRY_ACTION entry;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PPC_OAM_TCAM_ENTRY_ACTION_clear(&entry);

    res = arad_pp_oam_tcam_action_to_entry(action, &entry);
    SOCDNX_SAND_IF_ERR_RETURN(res);

    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |  TYPE  |  IS_OAM  |  IS_BFD  |   OPCODE   |   MD_LEVEL   |  YOUR_DISCR  |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |    %d   |     %d    |     %d    |      %d     |      %d       |      %d       |\n\r"),
                        entry.type, entry.is_oam, entry.is_bfd, entry.opcode, entry.mdl, entry.your_discr));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |        LIF_TCAM_RESULT       |    LIF_TCAM_RESULT_VALID  |  MY_CFM_MA   |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |               %5d          |              %d            |       %d      |\n\r"),
                        entry.oam_lif_tcam_result, entry.oam_lif_tcam_result_valid, entry.my_cfm_mac));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |     STAMP_OFFSET   |     OAM_OFFSET  |   OAM_PCP  |   OAM_ACTION_MATCH  |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |          %3d       |        %3d      |     %d      |          %d          |\n\r"),
                        entry.oam_stamp_offset, entry.oam_offset, entry.oam_pcp, action_match));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    SOC_EXIT;

exit:
    SOCDNX_FUNC_RETURN;

}


uint32
  arad_pp_oam_diag_print_tcam_entries_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id
   )
{
    int res;
    ARAD_PP_DIAG_REG_FIELD fld;
    uint32 regs_val[ARAD_TCAM_ENTRY_MAX_LEN] = {0};
    uint32 action_match;
    ARAD_TCAM_ACTION action;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "=============================================================================\n")));

    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |                              OAM TCAM KEY INFO                          |\n\r")));

    res = arad_pp_oam_tcam_entry_print(unit, core_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    ARAD_TCAM_ACTION_clear(&action);
    res = get_signal_for_oam_tcam_action_0(unit, &fld, core_id, regs_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    SHR_BITCOPY_RANGE(action.value,0,regs_val,0,48);

    res = get_signal_for_oam_tcam_action_match_0(unit, &fld, core_id, regs_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    action_match = regs_val[0];

    LOG_CLI((BSL_META_U(unit, "    |                               OAM TCAM ACTION                           |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |                            1st TCAM action result                       |\n\r")));

    res = arad_pp_oam_diag_print_tcam_action_unsafe(unit, action_match, &action);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

    ARAD_TCAM_ACTION_clear(&action);
    res = get_signal_for_oam_tcam_action_1(unit, &fld, core_id, regs_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    SHR_BITCOPY_RANGE(action.value,0,regs_val,0,48);

    res = get_signal_for_oam_tcam_action_match_1(unit, &fld, core_id, regs_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    action_match = regs_val[0];

    LOG_CLI((BSL_META_U(unit, "    |                            2st TCAM action result                       |\n\r")));
    res = arad_pp_oam_diag_print_tcam_action_unsafe(unit, action_match, &action);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
 
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

STATIC
  void
    arad_pp_oam_hw_tcam_entry_key_print(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN uint32                *tcam_key_val
  )
{
   uint32 mask, val = 0;

    uint32 gal_begin_bit = 0;
    uint32 cfm_begin_bit = 0;
    uint32 ip_begin_bit = 0;
    uint32 tcam_entry_key_header_start;
    uint32 ether_type_begin_bit = 0;
    uint32 offset = 0;
    uint32 field_size = 0;
    uint32 ttc = 0, forward_code = 0;
    uint32 total_tcam_size = _ARAD_PP_OAM_FLP_TCAM_ENTRY_SIZE;
                                                
    offset = total_tcam_size;
    SHR_BITCOPY_RANGE(&ttc,0,tcam_key_val,(offset-4),4);

    offset -= 4;
    SHR_BITCOPY_RANGE(&forward_code,0,tcam_key_val,(offset-4),4);

    if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "y1711_enabled",0) == 1) ||
        (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable",0) == 1)) {
        
        mask=0;
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |         Tunnel-Termination-Code         |              Mask             |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                     %d                   |             0x%08x        |\n\r"),
                            ttc, mask));
 
        
        mask = 0xFFFFFFFF; 
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |             Forwarding-Code             |              Mask             |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                            forward_code, mask));

        
        offset -= 4;
        field_size = SOC_IS_JERICHO_PLUS(unit) ? 4 : 3;
        val = 0;
        SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-field_size),field_size);
        mask = 0;
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |              In-LIF-Profile             |              Mask             |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                            val, mask));

        
        offset -= field_size;
        val = 0;
        SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-1),1);
        mask = 0xFFFFFFFF;
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |            VTT-OAM-LIF-Valid            |              Mask             |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                            val, mask));

        offset -= 1;
 
        if(SOC_IS_JERICHO_PLUS(unit)){
            
            val = 0;
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-1),1);
            mask = 0xFFFFFFFF;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |          VTT-OAM-LIF-Valid Outer        |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                val, mask));

            offset -= 1;
        }

        offset -= (SOC_IS_JERICHO_PLUS(unit) ? 22 : 20);
        

        
        SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-12),12);
        mask = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable",0) == 1) ? 0x100 : 0xFFFFFFFF;
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |          Y1711-LM-Match-Label           |              Mask             |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                     0x%03x               |           0x%08x          |\n\r"),
                            val, mask));

        offset -= 12;
        val = 0;
        SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-8),8);
        mask = 0xFFFFFFFF;
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |           Y1711-FUNCTION-TYPE           |              Mask             |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                   0x%02x                  |           0x%08x          |\n\r"),
                            val, mask));

    } else {
        
        if ((((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_downmep_pwe_classification", 0) == 1) && (SOC_IS_ARADPLUS_AND_BELOW(unit))))
            &&  forward_code == _ARAD_PP_OAM_FWD_CODE_ETHERNET && ttc >= SOC_PPC_PKT_TERM_TYPE_MPLS_ETH && ttc <= SOC_PPC_PKT_TERM_TYPE_CW_MPLS3_ETH) {
            uint32 ether_type_begin_bit;
            static uint8 max_tags, num_tags = 0, mdl = 0;
            max_tags = 3;
            
            if(SOC_IS_ARADPLUS_AND_BELOW(unit)){
                max_tags = 2;
            }
            
            num_tags = num_tags % max_tags;
            mask = 0xFFFFFFFE;  
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |         Tunnel-Termination-Code         |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |             0x%08x        |\n\r"),
                                ttc, mask));

            
            mask = 0xFFFFFFFF; 
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |             Forwarding-Code             |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                forward_code, mask));


            
            offset -= 4;
            field_size = SOC_IS_JERICHO_PLUS(unit) ? 4 : 3;
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-field_size),field_size);
            mask = 0;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |              In-LIF-Profile             |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                val, mask));

            
            offset -= field_size;
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-1),1);
            mask = 0;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |            VTT-OAM-LIF-Valid            |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                            val, mask));

            offset -= 1;

            if(SOC_IS_JERICHO_PLUS(unit)){
                
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-1),1);
                mask = 0;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |          VTT-OAM-LIF-Valid Outer        |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= 1;
            }
            tcam_entry_key_header_start = offset;
            ether_type_begin_bit = tcam_entry_key_header_start - (12 * 8)  - (num_tags * 4 * 8);
            cfm_begin_bit =  ether_type_begin_bit - 2*8 ;
            
            if(SOC_IS_ARADPLUS_AND_BELOW(unit)){
                 
                mdl++;
                num_tags += mdl/8;
                mdl = mdl % 8;
            } else {
                num_tags++;
            }
            field_size = 16;

            offset = (ether_type_begin_bit - (SOC_IS_JERICHO_PLUS(unit) ? 18:16));
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,offset,16);

            mask = 0xFFFFFFFF;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                Ether-Type               |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                val, mask));

            offset = cfm_begin_bit - 3;
            val=0;
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,offset,3);

            mask = 0xFFFFFFFF;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                   MDL                   |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                val, mask));
        } else {
            mask = (ttc == SOC_PPC_NOF_PKT_TERM_TYPES) ? 0 : 0xFFFFFFFE;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |         Tunnel-Termination-Code         |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |             0x%08x        |\n\r"),
                                ttc, mask));

            mask = 0xFFFFFFFF; 
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |             Forwarding-Code             |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                forward_code, mask));


            offset -= 4;
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-4),4);
            mask = 0;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |              In-LIF-Profile             |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                val, mask));

            offset -= 4;
            SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-4),4);
            mask = 0;
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |            VTT-OAM-LIF-Valid            |              Mask             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                val, mask));

            tcam_entry_key_header_start = 12;

            if (forward_code== _ARAD_PP_OAM_FWD_CODE_IPV4_UC) {
                ip_begin_bit = tcam_entry_key_header_start; 
            } else {
                switch (ttc) {
                case 0:   
                    ether_type_begin_bit = tcam_entry_key_header_start + 12 * 8;
                    cfm_begin_bit = tcam_entry_key_header_start + 14 * 8;
                    break;
                case SOC_PPC_PKT_TERM_TYPE_MPLS_ETH:
                case SOC_PPC_PKT_TERM_TYPE_CW_MPLS_ETH:
                    gal_begin_bit = tcam_entry_key_header_start + 4 * 8; 
                    break;
                case SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH:
                case SOC_PPC_PKT_TERM_TYPE_CW_MPLS2_ETH:
                    gal_begin_bit = tcam_entry_key_header_start + 2 * 4 * 8;
                    break;
                default:
                    gal_begin_bit = tcam_entry_key_header_start;
                }
            }

            if (ip_begin_bit > 0) {
                
                offset = total_tcam_size - ip_begin_bit;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-4),4);

                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |               IP-Version                |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= 4;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-4),4);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                 IP-IHL                  |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= 64;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-8),8);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                 IP-TTL                  |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= 8;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-8),8);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |               IP-Protocol               |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= 56;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-8),8);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                 IP-DIP                  |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= (24 + 2);  
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-2),2);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |           UDP.Source-Prot-Msbs          |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset -= 30;  
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,(offset-16),16);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |              UDP.Dest-Port              |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));
            } else if (cfm_begin_bit > 0) { 
                offset = total_tcam_size - ether_type_begin_bit - 16;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,offset,16);
                mask = 0xFFFFFFFF; 
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                Ether-Type               |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset = total_tcam_size - cfm_begin_bit - 3;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,offset,3);
                mask = 0xFFFFFFFF; 
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                    MDL                  |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val & 0x7, mask));

                offset = total_tcam_size - cfm_begin_bit - 8;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,offset,5);
                mask = _ARAD_PP_OAM_TCAM_ENTRY_BUILD_SET_MASK_FOR_VERSION_BY_OPCODE(val);
                val = 0; 
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                 Version                 |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset = total_tcam_size - cfm_begin_bit - 16;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,offset,16);
                mask = (val == (SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT - 1)) ? 0 : 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                  OpCode                 |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));
            } else { 
                offset = total_tcam_size - gal_begin_bit - 20;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,offset,20);
                mask = 0xFFFFFFFF; 
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                MPLS-Label               |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset = total_tcam_size - gal_begin_bit - 32;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,offset,8);
                mask = 0; 
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |        MPLS label with all the rest     |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset = total_tcam_size - gal_begin_bit - 48;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,offset,16);
                mask = 0xFFFFFFFF;
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |           G-Ach, version,Reserved       |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset = total_tcam_size - gal_begin_bit - 64;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,offset,16);
                mask = 0xFFFFFFFF; 
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                Channel-type             |              Mask             |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                    val, mask));

                offset = total_tcam_size - gal_begin_bit - 67;
                SHR_BITCOPY_RANGE(&val,0,tcam_key_val,offset,3);

                if (val == _ARAD_PP_OAM_Y1731_DEFAULT_MDLEVEL) {
                    mask = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0) ? 0 : 0xFFFFFFFF; 
                    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     MDL                 |              Mask             |\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                        val, mask));

                    offset = total_tcam_size - gal_begin_bit - 72;
                    SHR_BITCOPY_RANGE(&val,0,tcam_key_val,offset,5);
                    mask = _ARAD_PP_OAM_TCAM_ENTRY_BUILD_SET_MASK_FOR_VERSION_BY_OPCODE(val);
                    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                   Version               |              Mask             |\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                        val, mask));

                    offset = total_tcam_size - gal_begin_bit - 80;
                    SHR_BITCOPY_RANGE(&val,0,tcam_key_val,offset,8);
                    mask = (val == (SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT - 1)) ? 0 : 0xFFFFFFFF;
                    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                   OpCode                |              Mask             |\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     %d                   |            0x%08x         |\n\r"),
                                        val, mask));
                }
            }
        }
    }

  
}


int _arad_pp_oam_hw_tcam_entry_print(int                       unit,
                                uint32                         tcam_entry_id)
{
   ARAD_TCAM_ENTRY   _tcam_entry;
   ARAD_TCAM_ACTION  _tcam_action;
   int               res;

   SOCDNX_INIT_FUNC_DEFS;

   ARAD_TCAM_ENTRY_clear(&_tcam_entry);
   ARAD_TCAM_ACTION_clear(&_tcam_action);

   res = arad_tcam_managed_db_entry_get_unsafe(
      unit,
      ARAD_PP_TCAM_STATIC_ACCESS_ID_OAM_IDENTIFICATION,
      tcam_entry_id,
      FALSE,
      &_tcam_entry,
      &_tcam_action);
   
   SOCDNX_SAND_IF_ERR_RETURN(res);
   
   arad_pp_oam_hw_tcam_entry_key_print(unit, _tcam_entry.value);

   res = arad_pp_oam_diag_print_tcam_action_unsafe(unit,0,&_tcam_action);
   SOCDNX_SAND_IF_ERR_RETURN(res);
   
   SOC_EXIT;
exit:
   SOCDNX_FUNC_RETURN;
}

uint32
  arad_pp_oam_diag_print_hw_tcam_entries_unsafe(
     SOC_SAND_IN int unit
   )
{
    
    uint32 _rv = SOC_SAND_OK;
    uint32 res = SOC_SAND_OK;

    uint32 nof_tcam_entries, iter_entry;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = sw_state_access[unit].dpp.soc.arad.pp.oam.tcam_last_entry_id.get(unit, &nof_tcam_entries);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);

    for (iter_entry=0; iter_entry<nof_tcam_entries; iter_entry++) {
        LOG_CLI((BSL_META_U(unit,
                        "-------------Printing tcam entry id %u--------\n"),iter_entry));
        res = _arad_pp_oam_hw_tcam_entry_print(unit, iter_entry);
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  arad_pp_oam_ecn_init( SOC_SAND_IN  int unit) 
{

    uint32 _rv = SOC_SAND_OK;
    uint32 res = SOC_SAND_OK;
    uint32 reg32, aux_reg32;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    _rv = READ_IHB_BUILD_OAM_TS_HEADERr(unit, SOC_CORE_ALL, &reg32);
    SOCDNX_IF_ERR_EXIT(_rv);

    reg32 = soc_reg_field_get(unit,IHB_BUILD_OAM_TS_HEADERr, reg32, BUILD_OAM_TS_HEADERf);

    
    aux_reg32 = 128;
    soc_reg_field_set(unit,IHB_BUILD_OAM_TS_HEADERr, &reg32, BUILD_OAM_TS_HEADERf, reg32 | aux_reg32);
    _rv = WRITE_IHB_BUILD_OAM_TS_HEADERr(unit, SOC_CORE_ALL, reg32);
    SOCDNX_IF_ERR_EXIT(_rv);

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR, WRITE_IHB_OAM_SUB_TYPE_MAPr(unit, SOC_CORE_ALL,  0x8000)); 


    if (SOC_IS_JERICHO(unit)) {
        SOC_SAND_IF_ERR_EXIT(soc_jer_pp_oam_init_eci_tod(unit, 1, 0));
    }
    else {
        
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  205,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, ECI_GLOBALFr, REG_PORT_ANY, 0, TOD_MODEf,  3));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  210,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, ECI_TOD_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, TOD_SYNC_ENABLEf,  1));
        
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0,               _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_CONTROL);
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x13576543,      _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_FRAC_SEC_LOWER);
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1ffff00,       _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_FRAC_SEC_UPPER);
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x44b82fa1,      _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_FREQ_CONTROL);
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x80804560,      _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_SEC);
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1f,            _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_CONTROL);
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x0,             _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_CONTROL);
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x20,            _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_CONTROL);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ecn_init()", 0, 0);
}

soc_error_t arad_pp_oam_tod_set(
    int                                 unit,
   uint8                                is_ntp,
   uint64                               data
   )
{
    uint32 res = SOC_SAND_OK;
    uint32 time_frac = COMPILER_64_LO(data);
    uint32 time_sec = COMPILER_64_HI(data);
    uint32 tmp;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  205,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, ECI_GLOBALFr, REG_PORT_ANY, 0, TOD_MODEf,  3));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  210,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, ECI_TOD_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, TOD_SYNC_ENABLEf,  1)); 


    
    if (is_ntp) {
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1f,         _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_CONTROL);
        
         tmp=0;
        SHR_BITCOPY_RANGE(&tmp, 26, &time_frac, 0, 6);
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(tmp,           _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_FRAC_SEC_LOWER);
        
        SHR_BITCOPY_RANGE(&tmp, 0, &time_frac, 6, 26);
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(tmp,           _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_FRAC_SEC_UPPER);
        tmp=0;
        
        SHR_BITCOPY_RANGE(&tmp, 0, &time_sec, 1, 31);
        SHR_BITCOPY_RANGE(&tmp, 31, &time_sec, 0, 1);
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(tmp,            _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_SEC);
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x44b82fa1,    _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_FREQ_CONTROL);
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x20,          _ARAD_PP_OAM_TOD_BLOCK_ADDR_NTP_TIME_CONTROL);
    }
    else {
        tmp=0;
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x1f,         _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_CONTROL);
        
        SHR_BITCOPY_RANGE(&tmp, 26, &time_frac, 0, 6);
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(tmp,           _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_FRAC_SEC_LOWER);
        
        tmp=0;
        SHR_BITCOPY_RANGE(&tmp, 0, &time_frac, 6, 26);
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(tmp,           _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_FRAC_SEC_UPPER);
        
        tmp=0;
        SHR_BITCOPY_RANGE(&tmp, 0, &time_sec, 1, 31);
        SHR_BITCOPY_RANGE(&tmp, 31, &time_sec, 0, 1);
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(tmp,            _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_SEC);
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x10000000,    _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_FREQ_CONTROL);
        
        _ARAD_PP_OAM_ECI_TOD_COMMAND_WR_DATA(0x20,          _ARAD_PP_OAM_TOD_BLOCK_ADDR_IEEE1588_TIME_CONTROL);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_tod_set()", 0, 0);
}

soc_error_t arad_pp_oam_tod_get(
   int                                 unit,
   uint8                               is_ntp,
   uint64                              *data
   )
{
    soc_reg_above_64_val_t reg_above_64= {0}, field_above_64 = {0};
    soc_field_t tod_last_value[] = {TOD_NTP_LAST_VALUEf, TOD_1588_LAST_VALUEf };
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = READ_IHB_TOD_LAST_VALUEr(unit, REG_PORT_ANY, reg_above_64);
    SOCDNX_IF_ERR_EXIT(rv);

    if (is_ntp) {
        soc_reg_above_64_field_get(unit, IHB_TOD_LAST_VALUEr, reg_above_64, tod_last_value[0], field_above_64);
    }
    else {
        soc_reg_above_64_field_get(unit, IHB_TOD_LAST_VALUEr, reg_above_64, tod_last_value[1], field_above_64);
    }

    COMPILER_64_SET(*data, field_above_64[1], field_above_64[0]);

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_arad_oamp_sat_arb_weight_set(int unit,uint32 sat_arbiter_weight){
   uint32   oamp_arbiter_weight;
   int rv = BCM_E_NONE;

   BCMDNX_INIT_FUNC_DEFS;

   rv = READ_OAMP_ARBITER_WEIGHTr(unit, &oamp_arbiter_weight);
   BCMDNX_IF_ERR_EXIT(rv);

   soc_reg_field_set(unit, OAMP_ARBITER_WEIGHTr, &oamp_arbiter_weight, TST_ARB_WEIGHTf, sat_arbiter_weight );   

   rv = WRITE_OAMP_ARBITER_WEIGHTr(unit, oamp_arbiter_weight);
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


soc_error_t soc_arad_oamp_cpu_port_dp_tc_set(int unit,uint32 dp, uint32 tc){
   soc_reg_above_64_val_t reg_above_64;
   int rv = BCM_E_NONE;

   BCMDNX_INIT_FUNC_DEFS;

   rv = READ_OAMP_CPUPORTr(unit, reg_above_64);
   BCMDNX_IF_ERR_EXIT(rv);

   soc_reg_above_64_field32_set(unit, OAMP_CPUPORTr, reg_above_64, CP_UPORT_DPf, dp );   
   soc_reg_above_64_field32_set(unit, OAMP_CPUPORTr, reg_above_64, CP_UPORT_TCf, tc );   

   rv = WRITE_OAMP_CPUPORTr(unit, reg_above_64);
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


soc_error_t soc_arad_oamp_tmx_arb_weight_set(int unit,uint32 txm_arbiter_weight){
   uint32   oamp_arbiter_weight;
   int rv = BCM_E_NONE;

   BCMDNX_INIT_FUNC_DEFS;

   rv = READ_OAMP_ARBITER_WEIGHTr(unit, &oamp_arbiter_weight);
   BCMDNX_IF_ERR_EXIT(rv);

   soc_reg_field_set(unit, OAMP_ARBITER_WEIGHTr, &oamp_arbiter_weight, TXM_ARB_WEIGHTf, txm_arbiter_weight );   

   rv = WRITE_OAMP_ARBITER_WEIGHTr(unit, oamp_arbiter_weight);
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


soc_error_t soc_arad_oamp_response_weight_set(int unit,uint32 rsp_arbiter_weight){
   uint32   oamp_arbiter_weight;
   int rv = BCM_E_NONE;

   BCMDNX_INIT_FUNC_DEFS;

   rv = READ_OAMP_ARBITER_WEIGHTr(unit, &oamp_arbiter_weight);
   BCMDNX_IF_ERR_EXIT(rv);

   soc_reg_field_set(unit, OAMP_ARBITER_WEIGHTr, &oamp_arbiter_weight, RSP_ARB_WEIGHTf, rsp_arbiter_weight );   

   rv = WRITE_OAMP_ARBITER_WEIGHTr(unit, oamp_arbiter_weight);
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


soc_error_t
soc_arad_pp_oam_classifier_find_ep_id_by_lif_dir_and_mdl(int unit,
                                                         uint32 lif,
                                                         uint8  mdl,
                                                         uint8  is_upmep,
                                                         uint8  *is_mip,
                                                         uint8  *found,
                                                         int    *oam_id)
{

    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      oem1_key = {0};
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY      oem2_key = {0};
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  oem1_payload= {0};
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  oem2_payload= {0};
    uint8                                      found_profile = 0, passive_active_enable = 0;
    uint32                                     res, profile = 0;

    SOCDNX_INIT_FUNC_DEFS;

    *found = 0;
    *is_mip = 0;

    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY_clear(&oem2_key);
    SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_clear(&oem2_payload);
    oem2_key.oam_lif = lif;
    oem2_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);
    oem2_key.mdl = mdl;
    oem2_key.your_disc = 0;

    res = arad_pp_oam_classifier_oem2_entry_get_unsafe(unit, &oem2_key,
            &oem2_payload, &found_profile);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    if (found_profile) {
        *oam_id = oem2_payload.oam_id;
        *found = 2;
        SOC_EXIT;
    }

    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(&oem1_key);
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload);
    found_profile = 0;
    oem1_key.oam_lif = lif;
    oem1_key.ingress = IS_INGRESS_WHEN_ACTIVE(is_upmep);

    res = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key,
            &oem1_payload, &found_profile);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    if (found_profile) {
        profile = oem1_payload.mp_profile;
        if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
            int mp_type = JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(oem1_payload.mp_type_vector, mdl);
            if (mp_type == _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH || mp_type == _JER_PP_OAM_MDL_MP_TYPE_MIP) {
                *found = 1;
                if (mp_type == _JER_PP_OAM_MDL_MP_TYPE_MIP) {
                    *is_mip = 1;
                }
            }
        } else {
            *found = ((oem1_payload.mep_bitmap >> mdl) & 1);
            if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
                res = arad_pp_oam_mep_passive_active_enable_get_unsafe(unit, profile, &passive_active_enable);
                SOCDNX_SAND_IF_ERR_EXIT(res);
            }

            if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && (!(*found)) && (mdl != 0 )) {
                if (!passive_active_enable || SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) ) {
                    
                    *found |= (((oem1_payload.mip_bitmap & 0x7) == mdl) || ((oem1_payload.mip_bitmap >> 3) & 0x7) == mdl);
                    if (*found) {
                        *is_mip = 1;
                    }
                } else { 
                    if (oem1_payload.mip_bitmap | mdl) {
                        *found = 3;
                        *is_mip = 0;
                    }
                }
            } else if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                if ((oem1_payload.mip_bitmap >> mdl) & 1) {
                    *found = *is_mip = 1;
                }
            }
        }
        if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
            if ((*found) && (profile==ARAD_PP_OAM_PROFILE_PASSIVE) &&
                (!(*is_mip))) {
                *found = 0;
            }
        }
    } else {
        *found = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>
