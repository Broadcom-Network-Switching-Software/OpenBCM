#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD

#include <shared/bsl.h>
#include <soc/mem.h>




#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_reg_access.h>


#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/drv.h>

#ifdef PLISIM
  #include <sim/dpp/ChipSim/chip_sim_em.h>
#else
  #include <soc/dpp/ARAD/arad_sim_em.h>
#endif

#ifdef CRASH_RECOVERY_SUPPORT

#include <soc/hwstate/hw_log.h>
#include <soc/dcmn/dcmn_crash_recovery.h>

#endif 

#include <shared/swstate/access/sw_state_access.h>




#define ARAD_PP_MACT_DEBUG_PRINT 0
#define ARAD_PP_MACT_REPLY_FIFO_SIZE (128)

#define SOC_PPC_FRWRD_MACT_ADD_TYPE_MAX                          (SOC_PPC_NOF_FRWRD_MACT_ADD_TYPES-1)
#define SOC_PPC_FRWRD_MACT_KEY_TYPE_MAX                          (SOC_PPC_NOF_FRWRD_MACT_KEY_TYPES-1)
#define ARAD_PP_FRWRD_MACT_AGE_STATUS_MIN                        (1)
#define ARAD_PP_FRWRD_MACT_AGE_STATUS_MAX                        (6)
#define ARAD_PP_FRWRD_MACT_SEC_MAX                               (SOC_SAND_U32_MAX)
#define ARAD_PP_FRWRD_MACT_MILI_SEC_MAX                          (999999)
#define ARAD_PP_FRWRD_MACT_COMPARE_MASK_MAX                      (SOC_SAND_U32_MAX)
#define SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_MAX              (SOC_PPC_NOF_FRWRD_MACT_TRAVERSE_ACTION_TYPES-1)
#define ARAD_PP_FRWRD_MACT_NOF_MATCHED_ENTRIES_MAX               (SOC_SAND_U32_MAX)

#define ARAD_PP_FRWRD_MACT_IPV4_MC_DIP_BASE_VAL                  (0xE0000000)

#define ARAD_PP_FRWRD_MACT_IPV4_DIP_LSB                           (28)
#define ARAD_PP_FRWRD_MACT_IPV4_DIP_MSB                           (31)
#define ARAD_PP_FRWRD_MACT_IPV4_DIP_VALUE                         (0xE)

#define ARAD_PP_FRWRD_MACT_NOF_BITS_IN_MAC_ADDR                   (8)
#define ARAD_PP_FRWRD_MACT_NOF_MAC_ADDR_IN_UINT32                 (4)


#define ARAD_PP_FRWRD_MACT_TICKS_PER_ACCESS                       (20)

#define ARAD_PP_FRWRD_MACT_MASK_DEFAULT_NULL                  (0xFFFFFFFF)

#define ARAD_PP_FRWRD_MACT_TABLE_TRAVERSE_NOF_LOOPS           (1000)

#define ARAD_PP_MACT_FLUSH_ACCESS_IN_CLOCKS                   (5)

#define ARAD_PP_LEM_EEI_NOF_BITS (24)

#define ARAD_PP_LEM_LIF_VALUE_LSB (19)
#define ARAD_PP_LEM_EEI_FEC_VALUE_LSB(unit) SOC_DPP_DEFS_GET(unit, fec_nof_bits)
#define ARAD_PP_LEM_OUTLIF_NOF_BITS SOC_DPP_DEFS_GET(unit, out_lif_nof_bits) 
#define ARAD_PP_LEM_ENTRY_TYPE_LSB (SOC_IS_JERICHO(unit)?41:39)





#define ARAR_PP_MACT_FLD_NOF_U32(__nof_bit) (((__nof_bit)+31)/32)





typedef enum
{
  ARAD_PP_FRWRD_MACT_RULE_MASK_CONV_0_IGNORE,
  ARAD_PP_FRWRD_MACT_RULE_MASK_CONV_1_IGNORE,
  ARAD_PP_FRWRD_MACT_RULE_MASK_CONV_DONT_CARE
  
} ARAD_PP_FRWRD_MACT_RULE_MASK_CONV;





CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_frwrd_mact[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_PACK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_PACK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_PACK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ENTRY_PACK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAVERSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAVERSE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAVERSE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAVERSE_BY_MAC_LIMIT_PER_TUNNEL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAVERSE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_PRINT_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_PRINT_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_PRINT_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_PRINT_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_GET_ERRS_PTR),
  

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_KEY_CONVERT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_PAYLOAD_MASK_BUILD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_KEY_PARSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_PAYLOAD_CONVERT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_RULE_TO_KEY_CONVERT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_PAYLOAD_BUILD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_PAYLOAD_MASK_RETRIEVE),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_frwrd_mact[] =
{
  
  {
    SOC_PPC_FRWRD_MACT_ADD_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PPC_FRWRD_MACT_ADD_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'add_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_MACT_ADD_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_FLAVORS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_FLAVORS_OUT_OF_RANGE_ERR",
    "The parameter 'flavors' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_DIP_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_DIP_OUT_OF_RANGE_ERR",
    "The parameter 'dip' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PPC_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'key_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_MACT_KEY_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_AGE_STATUS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_AGE_STATUS_OUT_OF_RANGE_ERR",
    "The parameter 'age_status' is out of range. \n\r "
    "The range is: (Arad - B) 0 - 6.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_SEC_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_SEC_OUT_OF_RANGE_ERR",
    "The parameter 'sec' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_MILI_SEC_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_MILI_SEC_OUT_OF_RANGE_ERR",
    "The parameter 'mili_sec' is out of range. \n\r "
    "The range is: 0 - 999.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_DIP_MASK_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_DIP_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'dip_mask' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_COMPARE_MASK_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_COMPARE_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'compare_mask' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_MACT_TRAVERSE_ACTION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_UPDATE_MASK_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_UPDATE_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'update_mask' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_NOF_MATCHED_ENTRIES_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_NOF_MATCHED_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_matched_entries' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  {
    ARAD_PP_BLOCK_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR,
    "ARAD_PP_BLOCK_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR",
    "The parameter 'entries_to_act' is out of range. \n\r "
    "The range is: 0 - 100.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_BLOCK_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR,
    "ARAD_PP_BLOCK_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR",
    "The parameter 'entries_to_scan' is out of range. \n\r "
    "The range is: 0 - 64K+32.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_BLOCK_RANGE_ITER_OUT_OF_RANGE_ERR,
    "ARAD_PP_BLOCK_RANGE_ITER_OUT_OF_RANGE_ERR",
    "The parameter 'iter' is out of range. \n\r "
    "The range is: 0 - 64K+32-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_ACTION_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_ACTION_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_MACT_TRAVERSE_ACTION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_TABLE_TRAVERSE_WAITING_ERR,
    "ARAD_PP_FRWRD_MACT_TABLE_TRAVERSE_WAITING_ERR",
    "The MACT traverse API has failed due to \n\r "
    "an excessive waiting time to finish as desired.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MACT_ACCESSED_UPDATE_ILLEGAL_ERR,
    "ARAD_PP_MACT_ACCESSED_UPDATE_ILLEGAL_ERR",
    "trying to set to 1 the accessed bit in entry \n\r "
    "accesed has to be FALSE when update or add entry.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_FLUSH_OUT_OF_RESOURCE_ERR,
    "ARAD_PP_FRWRD_MACT_FLUSH_OUT_OF_RESOURCE_ERR",
    "traverse/flush db out of resource \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_UNKOWN_PREFIX_APP_ID_ERR,
    "ARAD_PP_FRWRD_MACT_UNKOWN_PREFIX_APP_ID_ERR",
    "The parameter 'app_id' isn't defined. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_MACT_PREFIX_APP_IDS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};
uint32
  arad_pp_lem_traverse_bitmap_build(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                         *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                         *rule_key_mask,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE   *rule_value,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA             *tbl_data
    );

uint32
  _arad_pp_lem_traverse_bitmap_build(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                         *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                         *rule_key_mask,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE   *rule_val,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_RULE_MASK_CONV              mask_conv,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA             *tbl_data
  );
uint32
  arad_pp_frwrd_lem_sw_traverse_internal_unsafe(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                     *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                     *rule_key_mask,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION         *action,
    SOC_SAND_OUT uint32                                     *nof_matched_entries,
    SOC_SAND_INOUT    SOC_SAND_TABLE_BLOCK_RANGE            *block_range
  );
uint32
  _arad_pp_frwrd_lem_traverse_internal_unsafe(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                         *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                         *rule_key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE    *rule_val,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                   *block_range,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION             *action,
    SOC_SAND_IN  uint8                                          wait_till_finish,
    SOC_SAND_OUT uint32                                         *nof_matched_entries
  );
STATIC uint32
  _arad_pp_frwrd_lem_get_block_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE   *value_rule,
    SOC_SAND_OUT uint32                                  access_only,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range,
    SOC_SAND_OUT  ARAD_PP_LEM_ACCESS_KEY                 *read_keys,
    SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD              *read_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  );
STATIC uint32
  _arad_pp_frwrd_lem_get_counter_by_block_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE   *value_rule,
    SOC_SAND_OUT uint32                                  access_only,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range,
    SOC_SAND_OUT uint32                                  *nof_entries
  );
uint32
      arad_pp_frwrd_lem_get_sw_block_by_key_unsafe(
              SOC_SAND_IN  int                                           unit,
              SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                       *rule_key,
              SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                       *rule_key_mask,
              SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
              SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
              SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY                        *lem_key_array,
              SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD                    *payload_array,
              SOC_SAND_OUT uint32                                       *nof_matched_entries);
STATIC uint32
  _arad_pp_frwrd_mact_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  );
STATIC uint32
  _arad_pp_frwrd_mact_get_counter_by_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT uint32                                  *nof_entries
  );
uint32
  _arad_pp_frwrd_lem_traverse_by_mac_limit_per_tunnel_internal_unsafe(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                       *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                       *rule_key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION           *action,
    SOC_SAND_OUT uint32                                       *nof_matched_entries
  );



    
uint32
  arad_pp_frwrd_mact_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res,
    access_in_clks;
  soc_reg_t
    JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERr = INVALIDr;
  soc_field_t
        JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTER_OVERFLOWf = INVALIDf,
        JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERf = INVALIDf;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  if (SOC_IS_JERICHO(unit)) {
        if (SOC_DPP_DEFS_GET(unit, nof_cores) == 1)
        {
            JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERr = PPDB_B_LARGE_EM_JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERr;
            JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTER_OVERFLOWf = LARGE_EM_JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTER_OVERFLOWf;
            JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERf = LARGE_EM_JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERf;
        }
        else
        {
            JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERr = PPDB_B_LARGE_EM_MASTER_JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERr;
            JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTER_OVERFLOWf = LARGE_EM_MASTER_JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTER_OVERFLOWf;
            JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERf = LARGE_EM_MASTER_JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERf;
        }
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, LARGE_EM_MNGMNT_UNIT_ENABLEf,  0x1));

  access_in_clks = ARAD_PP_MACT_FLUSH_ACCESS_IN_CLOCKS;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_ACCESS_SHAPERf,  access_in_clks));

        
    if (SOC_IS_JERICHO(unit)) {
        
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR, soc_reg_field32_modify(unit, PPDB_B_LARGE_EM_TRANSLATION_ERRr, REG_PORT_ANY, LARGE_EM_TRANSLATION_ERR_JR_TO_ARf, 0));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR, soc_reg_field32_modify(unit, PPDB_B_LARGE_EM_TRANSLATION_ERRr, REG_PORT_ANY, LARGE_EM_TRANSLATION_ERR_AR_TO_JRf, 0));
        
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR, soc_reg_field32_modify(unit, JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERr, REG_PORT_ANY, JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTER_OVERFLOWf, 0));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR, soc_reg_field32_modify(unit, JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERr, REG_PORT_ANY, JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERf, 0));

        if (SOC_DPP_DEFS_GET(unit, nof_cores) > 1)
        {
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR, soc_reg_field32_modify(unit, PPDB_B_LARGE_EM_SLAVE_JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERr, REG_PORT_ANY, LARGE_EM_SLAVE_JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTER_OVERFLOWf, 0));
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR, soc_reg_field32_modify(unit, PPDB_B_LARGE_EM_SLAVE_JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERr, REG_PORT_ANY, LARGE_EM_SLAVE_JR_TO_AR_TRANSLATION_ERR_EVENT_COUNTERf, 0));
        }        
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_init_unsafe()", 0, 0);
}

STATIC l2_event_fifo_interrupt_handler_cb_t learn_event_handler_cb = NULL;

soc_error_t arad_pp_frwrd_mact_event_handler_callback_register(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  l2_event_fifo_interrupt_handler_cb_t     event_handler_cb)
{
    learn_event_handler_cb = event_handler_cb;

    return SOC_E_NONE;
}

void arad_pp_frwrd_mact_handle_learn_event(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 *learn_event)
{
    if (learn_event_handler_cb) {
        learn_event_handler_cb(unit, (void*)learn_event);
    }
}

STATIC void
  arad_pp_frwrd_mact_payload_build_group(
      SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE *mac_entry_value,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD     *payload
    )
{
    payload->tpid_profile = mac_entry_value->group & 3;
    payload->has_cw = (mac_entry_value->group >> 2) & 1;
}

STATIC uint32
  arad_pp_frwrd_mact_payload_build(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE *mac_entry_value,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD     *payload
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_PAYLOAD_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(payload);

  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(payload);
  
  payload->age = mac_entry_value->aging_info.age_status;
  payload->is_dynamic = mac_entry_value->aging_info.is_dynamic;

  
  res = arad_pp_fwd_decision_in_buffer_build(
          unit,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
          &(mac_entry_value->frwrd_info.forward_decision),
          &(payload->dest),
          &(payload->asd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  payload->sa_drop = mac_entry_value->frwrd_info.drop_when_sa_is_known;

  if(mac_entry_value->frwrd_info.forward_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY){
      payload->flags = ARAD_PP_FWD_DECISION_PARSE_EEI;
  }
  else if(mac_entry_value->frwrd_info.forward_decision.additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE) {
      payload->flags = (mac_entry_value->frwrd_info.forward_decision.additional_info.outlif.type == SOC_PPC_OUTLIF_ENCODE_TYPE_RAW_INVALID) ?
          ARAD_PP_FWD_DECISION_PARSE_OUTLIF_INVALID : ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
  }
  else if(mac_entry_value->frwrd_info.forward_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC) {
      
      payload->flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
  }
  else{
       
      payload->flags = ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
  }

  if (mac_entry_value->group != 0) {
      if (payload->flags == ARAD_PP_FWD_DECISION_PARSE_OUTLIF || payload->flags == ARAD_PP_FWD_DECISION_PARSE_DEST) {
          arad_pp_frwrd_mact_payload_build_group(mac_entry_value,payload);
      } else if (SOC_IS_JERICHO(unit) && payload->flags == ARAD_PP_FWD_DECISION_PARSE_OUTLIF_INVALID) {
          arad_pp_frwrd_mact_payload_build_group(mac_entry_value,payload);
      } else { 
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 20, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_payload_build()", 0, 0);
}


STATIC uint32
  arad_pp_frwrd_mact_payload_mask_build(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE  *entry_val,
      SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE  *entry_val_mask,
      SOC_SAND_IN  uint32                    compare_mask,
      SOC_SAND_OUT uint32                    payload_data[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
      SOC_SAND_OUT uint32                    payload_mask[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
      SOC_SAND_IN  uint32                    mask_val 
    )
{
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload,
    payload_lem_mask;
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE  
    tmp_entry_val;
  uint8
    payload_valid=FALSE,
    is_eei_mc = 0,
    is_eei_fec = 0;
  uint32
    lag_mode,
    tmp = 0,
    zero_val = 0;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_PAYLOAD_MASK_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(payload_data);
  SOC_SAND_CHECK_NULL_INPUT(payload_mask);

  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload_lem_mask);

  payload_mask[0] = 0xffffffff;
  payload_mask[1] = (SOC_IS_JERICHO(unit) ? 0x1fff : 0x7ff);

#ifdef BCM_88660_A0
  
  if (SOC_IS_ARADPLUS(unit) && (compare_mask & (SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_COMPARE_SLB | 
                                                     SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_REPLACE_SLB))) {
    const uint32 nof_lag_bits = 14;
    const uint32 nof_fec_bits = 15;
    const uint32 lag_fmt_mask = (1 << nof_lag_bits) - 1;
    const uint32 fec_fmt_mask = (1 << nof_fec_bits) - 1;
    const uint32 lag_lsb = 0;
    const uint32 fec_lsb = 15;
    const uint32 match_lag_lsb = 38;
    const uint32 match_fec_lsb = 39;
    
    
    
     

    
    

    
    
    
     
     
    
     
    
    

    
    
    
    

    
    
    
    
    
    
    uint32 lag = entry_val->slb_info.lag_group_and_member & lag_fmt_mask;
    uint32 fec = entry_val->slb_info.fec & fec_fmt_mask;
    uint32 lag_mask = entry_val_mask->slb_info.lag_group_and_member & lag_fmt_mask;
    uint32 fec_mask = entry_val_mask->slb_info.fec & fec_fmt_mask;

    uint32 match_lag = entry_val->slb_info.match_lag_entries & 1;
    uint32 match_fec = entry_val->slb_info.match_fec_entries & 1;
    uint32 match_lag_mask = entry_val_mask->slb_info.match_lag_entries & 1;
    uint32 match_fec_mask = entry_val_mask->slb_info.match_fec_entries & 1;

    
    if ((compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_COMPARE_SLB) &&
        (compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_REPLACE_SLB)) {
      SOC_SAND_SET_ERROR_CODE(0, 1000, exit);
    }

    
    
    if (compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_COMPARE_SLB) {
      
      if (((match_lag_mask == 0) || (match_lag == 0)) && (lag_mask != 0)) {
        SOC_SAND_SET_ERROR_CODE(0, 2000, exit);
      }
      if (((match_fec_mask == 0) || (match_fec == 0)) && (fec_mask != 0)) {
        SOC_SAND_SET_ERROR_CODE(0, 3000, exit);
      }

      
      
      if (!((match_lag && match_lag_mask) || (match_fec && match_fec_mask)))
      {
        SOC_SAND_SET_ERROR_CODE(0, 4000, exit);
      }
    } else  {
      
      match_lag_mask = 0;
      match_fec_mask = 0;
    }

    
    

    
    payload_valid = FALSE;

    
    lag_mask = ~lag_mask;
    fec_mask = ~fec_mask;
    match_lag_mask = ~match_lag_mask;
    match_fec_mask = ~match_fec_mask;

    
    SHR_BITCOPY_RANGE(payload_data, lag_lsb,        &lag,            0, nof_lag_bits);
    SHR_BITCOPY_RANGE(payload_data, fec_lsb,        &fec,            0, nof_fec_bits);
    SHR_BITCOPY_RANGE(payload_data, match_lag_lsb,  &match_lag,      0, 1);
    SHR_BITCOPY_RANGE(payload_data, match_fec_lsb,  &match_fec,      0, 1);

    SHR_BITCOPY_RANGE(payload_mask, lag_lsb,        &lag_mask,       0, nof_lag_bits);
    SHR_BITCOPY_RANGE(payload_mask, fec_lsb,        &fec_mask,       0, nof_fec_bits);
    SHR_BITCOPY_RANGE(payload_mask, match_lag_lsb,  &match_lag_mask, 0, 1);
    SHR_BITCOPY_RANGE(payload_mask, match_fec_lsb,  &match_fec_mask, 0, 1);  
    
  }
#endif 

  
  if(compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO) {

      
      if(entry_val->frwrd_info.forward_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY) {
          
          if(!(compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE)){
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_TYPE_OUT_OF_RANGE_ERR, 10, exit);
          }
          if(entry_val->frwrd_info.forward_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC){
              is_eei_fec = 1;
          }
          else if(entry_val->frwrd_info.forward_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_MC){
              is_eei_mc = 1;
          }
          else {
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_TYPE_OUT_OF_RANGE_ERR, 20, exit);
          }
          
          if(compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_GROUP){
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_TYPE_OUT_OF_RANGE_ERR, 25, exit);
          }

      }
      else { 

          if(((compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE)) && (!(compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE))){
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_TYPE_OUT_OF_RANGE_ERR, 30, exit);
          }
      }
  }
  
  
    
  if ((compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL) != 0)
  {
    if (is_eei_mc) {
        
        tmp = ~entry_val_mask->frwrd_info.forward_decision.dest_id;
        SHR_BITCOPY_RANGE(&payload_mask[0],0,&tmp,0,SOC_DPP_DEFS_GET(unit, multicast_group_nof_bits));
    }
    else if (is_eei_fec) {
        
        tmp = ~entry_val_mask->frwrd_info.forward_decision.dest_id;
        SHR_BITCOPY_RANGE(&payload_mask[0],0,&tmp,0,SOC_DPP_DEFS_GET(unit, fec_nof_bits));
    }
    else {
        
        if (entry_val->frwrd_info.forward_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG)
        {
            if (SOC_IS_JERICHO(unit)) {
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, ECI_GLOBAL_GENERAL_CFG_3r, REG_PORT_ANY, 0, LAG_MODEf, &lag_mode)); 
            }
            else
            {
                    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, ECI_GLOBALFr, REG_PORT_ANY, 0, LAG_MODEf, &lag_mode)); 
            }

            
            tmp = ~entry_val_mask->frwrd_info.forward_decision.dest_id;
            SHR_BITCOPY_RANGE(&payload_mask[0],0,&tmp,0,ARAD_PP_LEM_ACCESS_DEST_NOF_BITS);
            payload_mask[0] |= SOC_SAND_BITS_MASK(13,10-lag_mode);
        }
        else
        {
            
            tmp = ~entry_val_mask->frwrd_info.forward_decision.dest_id;
            SHR_BITCOPY_RANGE(&payload_mask[0],0,&tmp,0,ARAD_PP_LEM_ACCESS_DEST_NOF_BITS);
        }
    }
  }

  
  if ((compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO) != 0)
  {
      tmp = ~entry_val_mask->frwrd_info.forward_decision.additional_info.eei.val.outlif;
      if (is_eei_mc) {
          res = soc_sand_bitstream_set_any_field(
                  &tmp,
                  SOC_DPP_DEFS_GET(unit, multicast_group_nof_bits),
                  ARAD_PP_LEM_EEI_NOF_BITS+1, 
                  (payload_mask)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      }
      else if (is_eei_fec) {
          res = soc_sand_bitstream_set_any_field(
                  &tmp,
                  ARAD_PP_LEM_EEI_FEC_VALUE_LSB(unit),
                  ARAD_PP_LEM_EEI_NOF_BITS+2, 
                  (payload_mask)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      }
      else {
          tmp = ~entry_val_mask->frwrd_info.forward_decision.additional_info.outlif.val;
          
          res = soc_sand_bitstream_set_any_field(
                  &tmp,
                  ARAD_PP_LEM_LIF_VALUE_LSB,
                  ARAD_PP_LEM_OUTLIF_NOF_BITS+1, 
                  (payload_mask)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

          res = soc_sand_bitstream_set_any_field(
                  &zero_val,
                  ARAD_PP_LEM_ENTRY_TYPE_LSB,
                  2, 
                  (payload_mask)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }
  }

  if ((compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC) != 0)
  {
    payload_mask[1] &= SOC_SAND_ZERO_BITS_MASK(ARAD_PP_LEM_ACCESS_IS_DYN_LSB, ARAD_PP_LEM_ACCESS_IS_DYN_LSB);
  }

  if ((compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA) != 0)
  {
    payload_mask[1] &= SOC_SAND_ZERO_BITS_MASK(ARAD_PP_LEM_ACCESS_DROP_SA, ARAD_PP_LEM_ACCESS_DROP_SA);
  }

  if ((compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_GROUP) != 0)
  {
    
    
      tmp = ~(entry_val_mask->group);
      SHR_BITCOPY_RANGE(&payload_mask[1],ARAD_PP_LEM_ACCESS_GROUP_LSB,&tmp,0,3);
    
    payload_mask[1] &= SOC_SAND_ZERO_BITS_MASK(ARAD_PP_LEM_ACCESS_ENTRY_TYPE_MSB, ARAD_PP_LEM_ACCESS_ENTRY_TYPE_LSB);
  }

  if (compare_mask & (
                      SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA|
                      SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC|
                      SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO|
                      SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL|
                      SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_GROUP
                     )
     )
  {
    payload_valid = TRUE;
  }

  if (payload_valid)
  {

    
    sal_memcpy(&tmp_entry_val,(entry_val),sizeof(SOC_PPC_FRWRD_MACT_ENTRY_VALUE));
    if(tmp_entry_val.frwrd_info.forward_decision.type == SOC_PPC_NOF_FRWRD_DECISION_TYPES)
    {
      tmp_entry_val.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    }
    res = arad_pp_frwrd_mact_payload_build(
              unit,
              &tmp_entry_val,
              &payload
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    
    res = arad_pp_lem_access_payload_build(
            unit,
            &payload,
            payload_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  if (mask_val == 0)
  {
    payload_mask[0] = ~payload_mask[0];
    payload_mask[1] = ~payload_mask[1] & (SOC_IS_JERICHO(unit) ? 0x1fff : 0x7ff);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_payload_mask_buildt()", 0, 0);
}

STATIC uint32
  arad_pp_frwrd_mact_payload_mask_retrieve(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                    payload_data[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
      SOC_SAND_IN  uint32                    payload_mask[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
      SOC_SAND_OUT  ARAD_PP_LEM_ACCESS_PAYLOAD   *payload,
      SOC_SAND_OUT  uint32                   *compare_mask
    )
{
  uint32
    mask;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_PAYLOAD_MASK_RETRIEVE);

  SOC_SAND_CHECK_NULL_INPUT(compare_mask);
  SOC_SAND_CHECK_NULL_INPUT(payload);

  payload->dest = SOC_SAND_GET_BITS_RANGE(payload_data[0], ARAD_PP_LEM_ACCESS_DEST_NOF_BITS-1, 0);
  payload->asd = SOC_SAND_GET_BITS_RANGE(payload_data[0], ARAD_PP_LEM_ACCESS_ASD_FIRST_REG_MSB, ARAD_PP_LEM_ACCESS_ASD_FIRST_REG_LSB)
    + SOC_SAND_GET_BITS_RANGE(payload_data[1], ARAD_PP_LEM_ACCESS_ASD_SCND_REG_MSB, ARAD_PP_LEM_ACCESS_ASD_SCND_REG_LSB);
  payload->is_dynamic = SOC_SAND_GET_BIT(payload_data[1], ARAD_PP_LEM_ACCESS_IS_DYN_LSB);
  payload->tpid_profile = SOC_SAND_GET_BITS_RANGE(payload_data[1], ARAD_PP_LEM_ACCESS_GROUP_MSB-1, ARAD_PP_LEM_ACCESS_GROUP_LSB);
  payload->has_cw = SOC_SAND_GET_BITS_RANGE(payload_data[1], ARAD_PP_LEM_ACCESS_GROUP_MSB, ARAD_PP_LEM_ACCESS_GROUP_MSB);

  
  mask = 0;
  
  if (SOC_SAND_GET_BITS_RANGE(payload_mask[0], ARAD_PP_LEM_ACCESS_DEST_NOF_BITS-1, 0) != 0x0)
  {
    mask += SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL;
  }

  if (
      (SOC_SAND_GET_BITS_RANGE(payload_mask[0], ARAD_PP_LEM_ACCESS_ASD_FIRST_REG_MSB, ARAD_PP_LEM_ACCESS_ASD_FIRST_REG_LSB) != 0x0)
      && (SOC_SAND_GET_BITS_RANGE(payload_mask[1], ARAD_PP_LEM_ACCESS_ASD_SCND_REG_MSB, ARAD_PP_LEM_ACCESS_ASD_SCND_REG_LSB) != 0x0)
     )
  {
    mask += SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO;
  }

  if (SOC_SAND_GET_BIT(payload_mask[0], ARAD_PP_LEM_ACCESS_IS_DYN_LSB) != 0x0)
  {
    mask += SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC;
  }

  if (SOC_SAND_GET_BITS_RANGE(payload_data[1], ARAD_PP_LEM_ACCESS_GROUP_MSB, ARAD_PP_LEM_ACCESS_GROUP_LSB) != 0x0)
  {
    mask += SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_GROUP;
  }
 

  *compare_mask = mask;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_payload_mask_retrieve()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_key_convert(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY *mac_entry_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *key
    )
{
  uint32
    res = SOC_SAND_OK;
  uint32 out_of_range = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_KEY_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(key);

  switch(mac_entry_key->key_type)
  {
  case SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR:
    key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_MAC;
    key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_MAC;

    
       
    res = soc_sand_pp_mac_address_struct_to_long(
            &(mac_entry_key->key_val.mac.mac),
            key->param[0].value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    key->param[1].value[0] = mac_entry_key->key_val.mac.fid;
    key->param[1].value[1] = 0;
    key->param[0].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC;
    key->param[1].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_MAC;
    key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_MAC;
    key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_MAC(unit);

    break;
  
  case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC:
    key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_IPV4_MC;
    key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IPV4_MC;
    key->param[0].value[0] = SOC_SAND_GET_BITS_RANGE(mac_entry_key->key_val.ipv4_mc.dip, ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC-1, 0);
    key->param[0].value[1] = 0;
    key->param[1].value[0] = mac_entry_key->key_val.ipv4_mc.fid;
    key->param[1].value[1] = 0;
    key->param[0].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC;
    key->param[1].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC;
    key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV4_MC;
    key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_IPV4_MC;
    break;

  case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC_SSM:
    key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_IPV4_MC_SSM;
    key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IPV4_MC_SSM;
    key->param[0].value[0] = SOC_SAND_GET_BITS_RANGE(mac_entry_key->key_val.ipv4_mc.dip, ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC_SSM-1, 0);
    key->param[0].value[1] = 0;
    key->param[1].value[0] = SOC_SAND_GET_BITS_RANGE(mac_entry_key->key_val.ipv4_mc.sip, ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC_SSM-1, 0);
    key->param[1].value[1] = 0;
    key->param[2].value[0] = SOC_SAND_GET_BITS_RANGE(mac_entry_key->key_val.ipv4_mc.fid, ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_IPV4_MC_SSM-1, 0);
    key->param[2].value[1] = 0;
    key->param[0].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC_SSM;
    key->param[1].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC_SSM;
    key->param[2].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_IPV4_MC_SSM;
    key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV4_MC_SSM;
    key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IPV4_MC_SSM_VAL;
    break;

  case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV6_MC_SSM_EUI:
    key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_IPV6_MC_SSM_EUI;
    key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_MAC;

    
    
    res = soc_sand_pp_mac_address_struct_to_long(
            &(mac_entry_key->key_val.mac.mac),
            key->param[0].value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    key->param[1].value[0] = mac_entry_key->key_val.mac.fid;
    key->param[1].value[1] = 0;
    key->param[0].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC;
    key->param[1].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_MAC;
    key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV6_MC_SSM_EUI;
    key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_IPV6_MC_SSM_EUI;

    break;


  case SOC_PPC_NOF_FRWRD_MACT_KEY_TYPES:
    key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_IPV4_MC;
    key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IPV4_MC;
    key->param[0].value[0] = SOC_SAND_GET_BITS_RANGE(mac_entry_key->key_val.ipv4_mc.dip, ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC-1, 0);
    key->param[0].value[1] = 0;
    key->param[1].value[0] = mac_entry_key->key_val.ipv4_mc.fid;
    key->param[1].value[1] = 0;
    key->param[0].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC;
    key->param[1].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC;
    key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV4_MC;
    key->prefix.value = 0;
    break;
  default:
    out_of_range = 1;
  }

  if (out_of_range) {
    SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_key_convert()", 0, 0);
}

    
uint32
  arad_pp_frwrd_mact_rule_to_key_convert(
      SOC_SAND_IN  int                             unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE *rule,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY                  *rule_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY                  *rule_key_mask,
      SOC_SAND_IN  uint32                              mask_val
    )
{
  uint32
    res;
  SOC_PPC_FRWRD_MACT_ENTRY_KEY
    mac_entry_key_rule,
    mac_entry_key;
  uint32
    mac_add_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_RULE_TO_KEY_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(rule);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&mac_entry_key);
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&mac_entry_key_rule);

  mac_entry_key.key_type = rule->key_type;
  mac_entry_key_rule.key_type = rule->key_type;
  switch(rule->key_type)
  {
  case SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR:
  case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV6_MC_SSM_EUI:
    ARAD_COPY(&(mac_entry_key.key_val.mac.mac), &(rule->key_rule.mac.mac), SOC_SAND_PP_MAC_ADDRESS, 1);
    ARAD_COPY(&(mac_entry_key_rule.key_val.mac.mac), &(rule->key_rule.mac.mac_mask), SOC_SAND_PP_MAC_ADDRESS, 1);
    if (mask_val)
    {
      
      for (mac_add_ndx = 0; mac_add_ndx < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; mac_add_ndx++)
      {
        mac_entry_key_rule.key_val.mac.mac.address[mac_add_ndx] = ~(mac_entry_key_rule.key_val.mac.mac.address[mac_add_ndx]);
      }
    }

    mac_entry_key.key_val.mac.fid = rule->key_rule.mac.fid;
    mac_entry_key_rule.key_val.mac.fid = (mask_val)?~rule->key_rule.mac.fid_mask:rule->key_rule.mac.fid_mask;
    break;

  case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC:
    mac_entry_key.key_val.ipv4_mc.dip = rule->key_rule.ipv4_mc.dip;
    mac_entry_key_rule.key_val.ipv4_mc.dip = (mask_val)?~rule->key_rule.ipv4_mc.dip_mask:rule->key_rule.ipv4_mc.dip_mask;
    mac_entry_key.key_val.ipv4_mc.fid = rule->key_rule.ipv4_mc.fid;
    mac_entry_key_rule.key_val.ipv4_mc.fid = (mask_val)?~rule->key_rule.ipv4_mc.fid_mask:rule->key_rule.ipv4_mc.fid_mask;
    break;

  case SOC_PPC_NOF_FRWRD_MACT_KEY_TYPES:
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

  res = arad_pp_frwrd_mact_key_convert(
          unit,
          &mac_entry_key,
          rule_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = arad_pp_frwrd_mact_key_convert(
          unit,
          &mac_entry_key_rule,
          rule_key_mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  

  if (mask_val == 0) {
      rule_key_mask->prefix.value = (1 << rule_key_mask->prefix.nof_bits) -1;
  }
  else{
      rule_key_mask->prefix.value = 0;
  }

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_rule_to_key_convert()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_key_parse(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *key,
      SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY *mac_entry_key
    )
{
  uint32
    reg_ndx,
    shift_ndx,
    mac_add_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_KEY_PARSE);

  SOC_SAND_CHECK_NULL_INPUT(key);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(mac_entry_key);

  switch(key->type)
  {
  case ARAD_PP_LEM_ACCESS_KEY_TYPE_MAC:
    mac_entry_key->key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
    for (mac_add_ndx = 0; mac_add_ndx < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++mac_add_ndx)
    {
      reg_ndx = mac_add_ndx / ARAD_PP_FRWRD_MACT_NOF_MAC_ADDR_IN_UINT32;
      shift_ndx = mac_add_ndx % ARAD_PP_FRWRD_MACT_NOF_MAC_ADDR_IN_UINT32;
      mac_entry_key->key_val.mac.mac.address[mac_add_ndx] = (uint8)
        SOC_SAND_GET_BITS_RANGE(key->param[0].value[reg_ndx],
          ARAD_PP_FRWRD_MACT_NOF_BITS_IN_MAC_ADDR*shift_ndx + (ARAD_PP_FRWRD_MACT_NOF_BITS_IN_MAC_ADDR-1),
          ARAD_PP_FRWRD_MACT_NOF_BITS_IN_MAC_ADDR*shift_ndx
        );
    }
    mac_entry_key->key_val.mac.fid = key->param[1].value[0];
    break;
  case ARAD_PP_LEM_ACCESS_KEY_TYPE_IPV4_MC:
    mac_entry_key->key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC;
    mac_entry_key->key_val.ipv4_mc.dip = key->param[0].value[0] + (key->param[1].value[0] << 16) + ARAD_PP_FRWRD_MACT_IPV4_MC_DIP_BASE_VAL;
    mac_entry_key->key_val.ipv4_mc.fid = key->param[2].value[0];
    break;
  
#ifdef BCM_88660_A0

  case ARAD_PP_LEM_ACCESS_KEY_TYPE_SLB:
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0)) {
       

      mac_entry_key->key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_SLB;

      SHR_BITCOPY_RANGE(mac_entry_key->key_val.slb.flow_label,         0, key->param[0].value, 0, ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_SLB);
      SHR_BITCOPY_RANGE(&mac_entry_key->key_val.slb.destination,        0, key->param[1].value, 0, ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_SLB);
      SHR_BITCOPY_RANGE(&mac_entry_key->key_val.slb.is_destination_fec, 0, key->param[2].value, 0, ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_SLB);
    }
    
    break;
#endif 

  default:
      mac_entry_key->key_type = SOC_PPC_NOF_FRWRD_MACT_KEY_TYPES;

    break; 
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_key_parse()", 0, 0);
}

STATIC uint32
  arad_pp_frwrd_mact_rule_from_key_build(
      SOC_SAND_IN  int                             unit,
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_KEY                  *rule_key,
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_KEY                  *rule_key_mask,
      SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE *rule
    )
{
  uint32
    res;
  SOC_PPC_FRWRD_MACT_ENTRY_KEY
    mac_entry_key_rule,
    mac_entry_key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(rule);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&mac_entry_key);
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&mac_entry_key_rule);

  res = arad_pp_frwrd_mact_key_parse(
          unit,
          rule_key,
          &mac_entry_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_frwrd_mact_key_parse(
          unit,
          rule_key_mask,
          &mac_entry_key_rule
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  rule->key_type = mac_entry_key.key_type;
  switch(mac_entry_key.key_type)
  {
  case SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR:
    ARAD_COPY(&(rule->key_rule.mac.mac), &(mac_entry_key.key_val.mac.mac), SOC_SAND_PP_MAC_ADDRESS, 1);
    ARAD_COPY(&(rule->key_rule.mac.mac_mask), &(mac_entry_key_rule.key_val.mac.mac), SOC_SAND_PP_MAC_ADDRESS, 1);
    rule->key_rule.mac.fid = mac_entry_key.key_val.mac.fid;
    rule->key_rule.mac.fid_mask = mac_entry_key_rule.key_val.mac.fid;
    break;

  case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC:
    rule->key_rule.ipv4_mc.dip = mac_entry_key.key_val.ipv4_mc.dip;
    rule->key_rule.ipv4_mc.dip_mask = mac_entry_key_rule.key_val.ipv4_mc.dip;
    rule->key_rule.ipv4_mc.fid = mac_entry_key.key_val.ipv4_mc.fid;
    rule->key_rule.ipv4_mc.fid_mask = mac_entry_key_rule.key_val.ipv4_mc.fid;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_rule_from_key_build()", 0, 0);
}


    
uint32
  arad_pp_frwrd_mact_payload_convert(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_PAYLOAD     *payload,
      SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE *mac_entry_value
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_PAYLOAD_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);

  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(mac_entry_value);

  
  mac_entry_value->aging_info.age_status = (uint8) payload->age;
  mac_entry_value->aging_info.is_dynamic = payload->is_dynamic;
  mac_entry_value->frwrd_info.drop_when_sa_is_known = payload->sa_drop;
  mac_entry_value->group =  (payload->has_cw << 2) | payload->tpid_profile;

  
  res = arad_pp_fwd_decision_in_buffer_parse(
          unit,          
          payload->dest,
          payload->asd,
          payload->flags,
          &(mac_entry_value->frwrd_info.forward_decision)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  mac_entry_value->accessed = (payload->flags & ARAD_PP_FWD_DECISION_PARSE_ACCESSED)?1:0;

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0)) {
    mac_entry_value->slb_info.match_fec_entries = payload->slb_fec_valid;
    mac_entry_value->slb_info.match_lag_entries = payload->slb_lag_valid;
    mac_entry_value->slb_info.fec = payload->slb_fec;
    mac_entry_value->slb_info.lag_group_and_member = payload->slb_lag;
  }
#endif 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_payload_convert()", 0, 0);
}


STATIC 
uint32
  arad_pp_frwrd_mact_traverse_remain_time_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TIME                    *time_to_finish,
    SOC_SAND_OUT uint32                                 *nof_matched_entries
  );


uint32
  arad_pp_frwrd_mact_app_to_prefix_get_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint32                                      app_id,
    SOC_SAND_OUT uint32                                      *prefix
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32 cur_app_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prefix);

  switch(app_id)
  {
    case SOC_PPC_FRWRD_MACT_PREFIX_APP_ID_OMAC:
      cur_app_id = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_ETH(unit);
      break;
    case SOC_PPC_FRWRD_MACT_PREFIX_APP_ID_OMAC_2_VMAC:
      cur_app_id = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_OMAC_2_VMAC;
      break;
    case SOC_PPC_FRWRD_MACT_PREFIX_APP_ID_VMAC:
      cur_app_id = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_VMAC;
      break;
    case SOC_PPC_FRWRD_MACT_PREFIX_APP_ID_VMAC_2_OMAC:
      cur_app_id = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_VMAC_2_OMAC;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_UNKOWN_PREFIX_APP_ID_ERR, 10, exit);
  }

  res = arad_pp_lem_access_app_to_prefix_get(
          unit,
          cur_app_id,
          prefix);

  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_app_to_prefix_get_unsafe()", 0, 0);
}



uint32
arad_pp_frwrd_mact_convert_key_value_into_request_payload(
            SOC_SAND_IN  int                                         unit,
            SOC_SAND_IN  int                                         insert,
            SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ADD_TYPE                 add_type,
            SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
            SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
            SOC_SAND_OUT ARAD_PP_LEM_ACCESS_REQUEST                  *request,
            SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD                  *payload

)
{
    uint32                  res = SOC_SAND_OK;
    ARAD_PP_LEM_ACCESS_CMD  access_cmd;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_CONVERT_KEY_VALUE_INTO_REQUEST_PAYLOAD);

    SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
    SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);
    SOC_SAND_CHECK_NULL_INPUT(request);
    SOC_SAND_CHECK_NULL_INPUT(payload);


    ARAD_PP_LEM_ACCESS_REQUEST_clear(request);
    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(payload);

    
    if (insert)
    {
        switch(add_type)
        {
        case SOC_PPC_FRWRD_MACT_ADD_TYPE_INSERT:
          access_cmd = ARAD_PP_LEM_ACCESS_CMD_INSERT;
            break;

        case SOC_PPC_FRWRD_MACT_ADD_TYPE_TRANSPLANT:
          access_cmd = ARAD_PP_LEM_ACCESS_CMD_TRANSPLANT;
            break;

        case SOC_PPC_FRWRD_MACT_ADD_TYPE_LEARN:
          access_cmd = ARAD_PP_LEM_ACCESS_CMD_LEARN;
            break;

        case SOC_PPC_FRWRD_MACT_ADD_TYPE_REFRESH:
          access_cmd = ARAD_PP_LEM_ACCESS_CMD_REFRESH;
            break;

        default:
          SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_ADD_TYPE_OUT_OF_RANGE_ERR, 10, exit);
        }
    }
    else
    {
        access_cmd = ARAD_PP_LEM_ACCESS_CMD_DELETE;
    }
    request->command = access_cmd;
    request->stamp = 0;

    
    res = arad_pp_frwrd_mact_key_convert(
            unit,
            mac_entry_key,
            &(request->key)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    
    res = arad_pp_frwrd_mact_payload_build(
              unit,
              mac_entry_value,
              payload
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_convert_key_value_into_request_payload()", 0, 0);

}




uint32
  arad_pp_frwrd_mact_entry_pack_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint8                                       insert,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint32                                      *data,
    SOC_SAND_OUT uint8                                       *data_len
  )
{
  uint32                                        res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_OUTPUT                     request_all;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_PACK);
  ARAD_PP_LEM_ACCESS_OUTPUT_clear(&request_all);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_SAND_CHECK_NULL_INPUT(data_len);
  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_convert_key_value_into_request_payload(unit, insert, add_type, mac_entry_key, mac_entry_value, &(request_all.request), &(request_all.payload));
   SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  request_all.req_origin = ARAD_PP_LEM_ACCCESS_REQ_ORIGIN_CPU;
  if (insert)
	  request_all.is_learned_first_by_me = TRUE;

  res = arad_pp_lem_access_next_stamp_get(unit, &request_all.request.stamp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit_semaphore);

  res = arad_pp_lem_request_pack(
                                 unit,
                                 &request_all,
                                 data,
                                 data_len);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_entry_add()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_sw_entry_add_unsafe(
              SOC_SAND_IN  int                                         unit,
              SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ADD_TYPE                 add_type,
              SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
              SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
              SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
            )
{
    uint32                          res = SOC_SAND_OK;
    ARAD_PP_LEM_ACCESS_REQUEST      request;
    ARAD_PP_LEM_ACCESS_PAYLOAD      payload;
    uint8                           sw_res = SOC_SAND_ERR;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_SW_ENTRY_ADD_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
    SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);
    SOC_SAND_CHECK_NULL_INPUT(success);

    res = arad_pp_frwrd_mact_convert_key_value_into_request_payload(unit, TRUE, add_type, mac_entry_key, mac_entry_value, &request, &payload);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = arad_pp_lem_sw_entry_update_unsafe(unit, &request, &payload, &sw_res);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    SOC_SAND_CHECK_FUNC_RESULT(sw_res, 40, exit);

    *success = SOC_SAND_SUCCESS;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_sw_entry_add_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32                             res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_REQUEST         request;
  ARAD_PP_LEM_ACCESS_PAYLOAD         payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS      ack_status;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);

  
  if(mac_entry_key->key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC && (SOC_IS_ARADPLUS(unit)) )
  {
      arad_pp_lem_dbal_access_mact_entry_add_unsafe(unit,
                                                   mac_entry_key,
                                                   mac_entry_value,
                                                   success);
  }
  else
  {
      ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);

      res = arad_pp_frwrd_mact_convert_key_value_into_request_payload(unit, TRUE, add_type, mac_entry_key, mac_entry_value, &request, &payload);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      
      res = arad_pp_lem_access_entry_add_unsafe(
              unit,
              &request,
              &payload,
              &ack_status
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      if (ack_status.is_success == TRUE)
      {
        *success = SOC_SAND_SUCCESS;
      }
      else
      {
        switch(ack_status.reason)
        {
        case ARAD_PP_LEM_ACCESS_FAIL_REASON_MACT_FULL:
        case ARAD_PP_LEM_ACCESS_FAIL_REASON_CAM_FULL:
          *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
          break;

        case ARAD_PP_LEM_ACCESS_FAIL_REASON_FID_LIMIT:
          *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_2;
          break;

        case ARAD_PP_LEM_ACCESS_FAIL_REASON_REQUEST_NOT_SENT:
       case ARAD_PP_LEM_ACCESS_FAIL_REASON_WRONG_STAMP:
        case ARAD_PP_LEM_ACCESS_FAIL_REASON_FID_UNKNOWN:
        case ARAD_PP_LEM_ACCESS_FAIL_REASON_UNKNOWN:
          *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_3;
          break;

       case ARAD_PP_LEM_ACCESS_FAIL_REASON_CHANGE_STATIC:
        case ARAD_PP_LEM_ACCESS_FAIL_REASON_LEARN_STATIC:
          *success = SOC_SAND_FAILURE_REMOVE_ENTRY_FIRST;
          break;

        case ARAD_PP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN:
        default:
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR, 50, exit);
        }
      }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_entry_add_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value
  )
{
 uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(add_type, SOC_PPC_FRWRD_MACT_ADD_TYPE_MAX, SOC_PPC_FRWRD_MACT_ADD_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_FRWRD_MACT_ENTRY_KEY_verify(unit, mac_entry_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_ENTRY_VALUE, mac_entry_value, 30, exit);

  if(mac_entry_value->accessed)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_MACT_ACCESSED_UPDATE_ILLEGAL_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_entry_add_verify()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_entry_pack_verify(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value
  )
{
 uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_PACK_VERIFY);

  res = SOC_PPC_FRWRD_MACT_ENTRY_KEY_verify(unit, mac_entry_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_ENTRY_VALUE, mac_entry_value, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_entry_pack_verify()", 0, 0);
}



uint32 _arad_pp_frwrd_mact_sw_entry_remove_unsafe(
      SOC_SAND_IN   int                        unit,
      SOC_SAND_IN   ARAD_PP_LEM_ACCESS_REQUEST  *request
    )
{
    uint32 res = SOC_SAND_OK;
    ARAD_PP_LEM_ACCESS_KEY_ENCODED key_in_buffer;
    uint32 key[SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S];

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_SW_ENTRY_REMOVE_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(request);

    ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);

    soc_sand_os_memset(key, 0x0, ARAD_CHIP_SIM_LEM_KEY);

    res = arad_pp_lem_key_encoded_build(
          unit,
          &(request->key),
          0,
          &key_in_buffer
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

    key[0] = key_in_buffer.buffer[0];
    key[1] = key_in_buffer.buffer[1];
    key[2] = key_in_buffer.buffer[2];

    res = chip_sim_exact_match_entry_remove_unsafe(
            unit,
            ARAD_CHIP_SIM_LEM_BASE,
            key,
            ARAD_CHIP_SIM_LEM_KEY
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_frwrd_mact_sw_entry_remove_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_mact_entry_remove_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY      *mac_entry_key,
    SOC_SAND_IN  int                                sw_only
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack_status;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);

  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);

  if(mac_entry_key->key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC && (SOC_IS_ARADPLUS(unit)) ) {
      arad_pp_lem_dbal_access_mact_entry_remove_unsafe(unit, mac_entry_key);
  } else {
      
      request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;
      request.stamp = 0;

      res = arad_pp_frwrd_mact_key_convert(
              unit,
              mac_entry_key,
              &(request.key)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

      if (sw_only)
      {
          res = _arad_pp_frwrd_mact_sw_entry_remove_unsafe(unit, &request);
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
      else
      {
          
          res = arad_pp_lem_access_entry_remove_unsafe(
                  unit,
                  &request,
                  &ack_status
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          if (!((ack_status.is_success == TRUE) || (ack_status.reason == ARAD_PP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN)))
          {
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR, 50, exit);
          }
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_entry_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_REMOVE_VERIFY);

  res = SOC_PPC_FRWRD_MACT_ENTRY_KEY_verify(unit, mac_entry_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_entry_remove_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_ipmc_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_REMOVE_VERIFY);

  res = arad_pp_dbal_table_clear(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_LEM);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_ipmc_table_clear_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_mact_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_KEY
    key;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  uint8
    is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(mac_entry_value);
  ARAD_PP_LEM_ACCESS_KEY_clear(&key);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

  
   if(mac_entry_key->key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC && (SOC_IS_ARADPLUS(unit)) )
   {
       arad_pp_lem_dbal_access_mact_entry_get_unsafe(unit,
                                                    mac_entry_key,
                                                    &payload,
                                                    &is_found);
   }
   else
   {


      
      res = arad_pp_frwrd_mact_key_convert(
              unit,
              mac_entry_key,
              &(key)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      
      res = arad_pp_lem_access_entry_by_key_get_unsafe(
              unit,
              &key,
              &payload,
              &is_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
   }
  
  res = arad_pp_frwrd_mact_payload_convert(
          unit,
          &payload,
          mac_entry_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *found = is_found;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_entry_get_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_GET_VERIFY);

  res = SOC_PPC_FRWRD_MACT_ENTRY_KEY_verify(unit, mac_entry_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_entry_get_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_traverse_internal_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION        *action,
    SOC_SAND_IN  uint8                                 wait_till_finish,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  )
{

    return arad_pp_frwrd_lem_traverse_internal_unsafe(
            unit,
            rule_key,
            rule_key_mask,
            rule_val,
            block_range,
            action,
            wait_till_finish, 
            nof_matched_entries
          );
}


uint32
  arad_pp_frwrd_mact_traverse_by_mac_limit_per_tunnel_internal_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION        *action,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  )
{
    return arad_pp_frwrd_lem_traverse_by_mac_limit_per_tunnel_internal_unsafe(
             unit,
             rule_key,
             rule_key_mask,
             rule_val,
             block_range,
             action,
             nof_matched_entries
             );
}


uint32
  arad_pp_frwrd_mact_traverse_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_IN  uint8                                 wait_till_finish
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_VERIFY);

  

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_traverse_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_traverse_status_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO     *status
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  uint32
    entry_offset = 0;
  uint32
    nof_entries = 0;
  ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA
    tbl_data;
  ARAD_PP_LEM_ACCESS_KEY
    rule_key,
    rule_key_mask;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload_new,
    payload;
  ARAD_PP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer_mask,
    key_in_buffer;
  uint32
    buff_data[2],
    buff_mask[2];


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(status);

  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_clear(status);

  ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key_mask);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload_new);
  ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);
  ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer_mask);

#ifdef CRASH_RECOVERY_SUPPORT
  if (SOC_IS_DONE_INIT(unit))
  {
      if (BCM_UNIT_DO_HW_READ_WRITE(unit))
      {
          LOG_ERROR(BSL_LS_SOC_HWLOG,
                  (BSL_META_U(unit, "WARNING! HW LOG is not implemented in arad_pp_frwrd_mact_traverse_status_get_unsafe \n")));
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 8050, exit);
      }
  }
#endif 


  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &fld_val));

  
  res = arad_pp_frwrd_mact_traverse_remain_time_get_unsafe(unit, &(status->time_to_finish), &nof_entries);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  res = arad_pp_ihp_flush_db_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_REPORT_HITSf, &fld_val));

  
  if (tbl_data.action_drop == 0x1)
  {
    status->action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;
  }
  else if (fld_val == 0x1)
  {
    status->action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE;
  }
  else
  {
    if ((COMPILER_64_LO(tbl_data.action_transplant_payload_mask) == 0x0) && (COMPILER_64_HI(tbl_data.action_transplant_payload_mask) == 0x0))
    {
      status->action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT;
    }
    else
    {
      status->action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_UPDATE;
      
      buff_data[0] = COMPILER_64_LO(tbl_data.action_transplant_payload_data);
      buff_data[1] = COMPILER_64_HI(tbl_data.action_transplant_payload_data);


      buff_mask[0] = COMPILER_64_LO(tbl_data.action_transplant_payload_mask);
      buff_mask[1] = COMPILER_64_HI(tbl_data.action_transplant_payload_mask);

      res = arad_pp_frwrd_mact_payload_mask_retrieve(
              unit,
              buff_data,
              buff_mask,
              &payload_new,
              &(status->action.update_mask)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = arad_pp_frwrd_mact_payload_convert(
              unit,
              &payload_new,
              &(status->action.updated_val)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }
  

  SHR_BITCOPY_RANGE(key_in_buffer.buffer,48, &tbl_data.compare_key_20_data, 0, 16);
  SHR_BITCOPY_RANGE(key_in_buffer.buffer,70, &tbl_data.compare_key_20_data, 16, 4);

  SHR_BITCOPY_RANGE(key_in_buffer_mask.buffer,48, &tbl_data.compare_key_20_mask, 0, 16);
  SHR_BITCOPY_RANGE(key_in_buffer_mask.buffer,70, &tbl_data.compare_key_20_mask, 16, 4);

  res = arad_pp_lem_key_encoded_parse(
          unit,
          &key_in_buffer,
          &rule_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = arad_pp_lem_key_encoded_parse(
          unit,
          &key_in_buffer_mask,
          &rule_key_mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = arad_pp_frwrd_mact_rule_from_key_build(
          unit,
          &rule_key,
          &rule_key_mask,
          &(status->rule)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 85, exit);

  
  buff_data[0] = COMPILER_64_LO(tbl_data.action_transplant_payload_data);
  buff_data[1] = COMPILER_64_HI(tbl_data.action_transplant_payload_data);
  buff_mask[0] = COMPILER_64_LO(tbl_data.action_transplant_payload_mask);
  buff_mask[1] = COMPILER_64_HI(tbl_data.action_transplant_payload_mask);

  res = arad_pp_frwrd_mact_payload_mask_retrieve(
          unit,
          buff_data,
          buff_mask,
          &payload,
          &(status->rule.value_rule.compare_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = arad_pp_frwrd_mact_payload_convert(
          unit,
          &payload,
          &(status->rule.value_rule.val)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  
  if (status->time_to_finish.mili_sec == 0 && status->time_to_finish.sec == 0)
  {
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_HIT_COUNTERr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_HIT_COUNTERf, &fld_val));
    status->nof_matched_entries = fld_val;

    tbl_data.compare_valid = FALSE;

    res = arad_pp_ihp_flush_db_tbl_set_unsafe(
            unit,
            entry_offset,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_traverse_status_get_unsafe()", 0, 0);
}


STATIC uint32
  arad_pp_frwrd_lem_traverse_db_invalidate(
    SOC_SAND_IN  int             unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_IMP_DEFS_MAX(PPDB_LARGE_EM_FLUSH_DB_NOF_LONGS)];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  ARAD_CLEAR(data, uint32, SOC_DPP_IMP_DEFS_MAX(PPDB_LARGE_EM_FLUSH_DB_NOF_LONGS));
  arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_FLUSH_DBm, MEM_BLOCK_ANY, data);

  
  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.flush_entry_use.set(unit, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_lem_traverse_db_invalidate()", 0, 0);
}



uint32
  _arad_pp_frwrd_lem_traverse_check_flush_is_done(int unit,
                                                                                       SOC_SAND_OUT uint32                               *nof_matched_entries,
                                                                                       SOC_SAND_OUT uint32                               *waiting_time_in_ms,
                                                                                       SOC_SAND_OUT int                                  *flush_not_running)
{
       uint32
               entry_offset,
               res = SOC_SAND_OK;
       SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO
               status;
       ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA
               tbl_data;


       SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_UNSAFE);
       SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_clear(&status);

       
       res = arad_pp_frwrd_mact_traverse_remain_time_get_unsafe(
               unit,
               &status.time_to_finish,
               &status.nof_matched_entries
               );
       SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

#ifdef SAND_LOW_LEVEL_SIMULATION
       status.time_to_finish.mili_sec = 0;
       status.time_to_finish.sec = 0;
       status.nof_matched_entries = 0;
#endif

       if ((status.time_to_finish.sec == 0) && (status.time_to_finish.mili_sec == 0)) {
               *flush_not_running = TRUE;
               *nof_matched_entries = status.nof_matched_entries;
       } else {
               *flush_not_running = FALSE;
               *waiting_time_in_ms = status.time_to_finish.mili_sec
                       + (status.time_to_finish.sec * ARAD_PP_FRWRD_MACT_NOF_MS_IN_SEC);
       }


       if (*flush_not_running == TRUE) {
               
               for (entry_offset = 0; entry_offset < ARAD_PP_MACT_NOF_FLUSH_ENTRIES; entry_offset++) {
                       ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA_clear(&tbl_data);
                       tbl_data.compare_valid = 0;
                       res = arad_pp_ihp_flush_db_tbl_set_unsafe(
                               unit,
                               entry_offset,
                               &tbl_data
                               );
                       SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

                       res = arad_pp_ihp_flush_db_tbl_get_unsafe(
                               unit,
                               entry_offset,
                               &tbl_data
                               );
                       SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
               }
       }


exit:
       SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_frwrd_lem_traverse_wait_if_necessary_blocking()", 0, 0);
}



STATIC uint32
  arad_pp_frwrd_lem_traverse_run(
    SOC_SAND_IN  int            unit
  )
{
    uint32 res = SOC_SAND_OK;
    int32 wait_loop_ndx;
    uint32 waiting_time_in_ms;
    uint32 nof_matched_entries;
    int is_wait_success;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf,  DPP_PP_LEM_NOF_ENTRIES(unit)));

    
    is_wait_success = FALSE;
    for (wait_loop_ndx = 0; wait_loop_ndx < ARAD_PP_FRWRD_MACT_TABLE_TRAVERSE_NOF_LOOPS; ++wait_loop_ndx) {
        res = _arad_pp_frwrd_lem_traverse_check_flush_is_done(unit, &nof_matched_entries, &waiting_time_in_ms, &is_wait_success);
        SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);
        if (!is_wait_success) {
            res = soc_sand_os_task_delay_milisec(waiting_time_in_ms);
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
        } else {
            break;
        }
    }

    if (is_wait_success == FALSE) {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_TABLE_TRAVERSE_WAITING_ERR, 90, exit);
    }

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_lem_traverse_run()", 0, 0);
}

STATIC uint32
  arad_pp_frwrd_lem_traverse_db_get_next_available(
    SOC_SAND_IN  int              unit,
    SOC_SAND_OUT uint32              *flush_entry_offset
  )
{
  uint32
    res,
    entry_offset = 0;
  uint8
    in_use;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (entry_offset = 0; entry_offset < ARAD_PP_MACT_NOF_FLUSH_ENTRIES; entry_offset++) {
      res = arad_pp_sw_db_mact_traverse_flush_entry_use_get(
              unit,
              entry_offset,
              &in_use
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (in_use == 0) {
          *flush_entry_offset = entry_offset;
          ARAD_DO_NOTHING_AND_EXIT;
      }
  }
  *flush_entry_offset = ARAD_PP_MACT_NOF_FLUSH_ENTRIES;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_lem_traverse_db_get_next_available()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_traverse_mode_info_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (mode->state) {
  case SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_NORMAL:
  
      res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.traverse_mode.set(unit,mode->state);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_AGGREGATE:
      if (!SOC_DPP_IS_LEM_SIM_ENABLE(unit)) {
          res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.traverse_mode.set(unit,mode->state);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      }
  break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_RESET:
      arad_pp_frwrd_lem_traverse_db_invalidate(unit);
  break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_RUN:
      arad_pp_frwrd_lem_traverse_run(unit);
  break;
  default:
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_traverse_mode_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_traverse_mode_info_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  )
{
  uint32
    next_available,
    res = SOC_SAND_OK;
  uint8
    mode_lcl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.traverse_mode.get(unit, &mode_lcl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  mode->state = mode_lcl;

  res = arad_pp_frwrd_lem_traverse_db_get_next_available(unit,&next_available);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  mode->nof_available_rules =  ARAD_PP_MACT_NOF_FLUSH_ENTRIES - next_available;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_traverse_mode_info_get_unsafe()", 0, 0);
}


STATIC
uint32
  arad_pp_frwrd_mact_traverse_remain_time_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TIME                    *time_to_finish,
    SOC_SAND_OUT uint32                                 *nof_matched_entries
  )
{
  uint32
    ticks_per_ms = 0,
    time_remaning_ms,
    fld_val,
    res = SOC_SAND_OK;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(time_to_finish);
  SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);

  
  ticks_per_ms = arad_chip_kilo_ticks_per_sec_get(unit);
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &fld_val));

  if ((fld_val == 0))
  {
    
    time_remaning_ms = 0;
  }
  else
  {
    
    time_remaning_ms = fld_val * ARAD_PP_FRWRD_MACT_TICKS_PER_ACCESS / ticks_per_ms;

    
    if (time_remaning_ms == 0) {
        time_remaning_ms = 1;
    }
  }

  time_to_finish->sec = time_remaning_ms / ARAD_PP_FRWRD_MACT_NOF_MS_IN_SEC;
  time_to_finish->mili_sec = time_remaning_ms % ARAD_PP_FRWRD_MACT_NOF_MS_IN_SEC;

  if (time_to_finish->mili_sec == 0 && time_to_finish->sec == 0)
  {
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_HIT_COUNTERr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_HIT_COUNTERf, &fld_val));
    *nof_matched_entries = fld_val;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_traverse_remain_time_get_unsafe()", 0, 0);
}

uint32
      arad_pp_frwrd_lem_get_sw_block_by_key_unsafe(
              SOC_SAND_IN  int                                           unit,
              SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                       *rule_key,
              SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                       *rule_key_mask,
              SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
              SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
              SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY                        *lem_key_array,
              SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD                    *payload_array,
              SOC_SAND_OUT uint32                                       *nof_matched_entries)
{
    uint32                                  res             = SOC_SAND_OK;
    ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA      tbl_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_GET_BLOCK_BY_KEY_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(rule_key);
    SOC_SAND_CHECK_NULL_INPUT(rule_key_mask);
    SOC_SAND_CHECK_NULL_INPUT(block_range);
    SOC_SAND_CHECK_NULL_INPUT(lem_key_array);
    SOC_SAND_CHECK_NULL_INPUT(payload_array);
    SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);

    ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA_clear(&tbl_data);

    res = _arad_pp_lem_traverse_bitmap_build(unit, rule_key, rule_key_mask, rule_val, ARAD_PP_FRWRD_MACT_RULE_MASK_CONV_0_IGNORE, &tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

    arad_pp_lem_block_get(unit, (void *)&tbl_data, block_range, lem_key_array, payload_array, nof_matched_entries);
    SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_lem_get_sw_block_by_key_unsafe()", 0, 0);

}


uint32
  arad_pp_frwrd_mact_get_sw_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
    uint32                                  res = SOC_SAND_OK;
    ARAD_PP_LEM_ACCESS_KEY                  rule_key,
                                            rule_key_mask;
    ARAD_PP_LEM_ACCESS_PAYLOAD              *payload_array  = NULL;
    ARAD_PP_LEM_ACCESS_KEY                  *lem_key_array  = NULL;
    ARAD_PP_LEM_ACCESS_KEY                  *lem_key        = NULL; 
    SOC_PPC_FRWRD_MACT_ENTRY_KEY            mac_key;
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE          mac_entry_value;
    ARAD_PP_LEM_ACCESS_KEY_TYPE             key_type        = 0;

    ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA      tbl_data;
    uint32                                  i;
    uint32                                  nof_total_entries;
    uint32                                  param_ndx,
                                            value_ndx;
    uint32                                  check_key,
	                                        is_key_for_rule;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_GET_BLOCK_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(rule);
    SOC_SAND_CHECK_NULL_INPUT(block_range);
    SOC_SAND_CHECK_NULL_INPUT(mact_keys);
    SOC_SAND_CHECK_NULL_INPUT(mact_vals);
    SOC_SAND_CHECK_NULL_INPUT(nof_entries);

    ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key);
    ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key_mask);
    SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&mac_key);
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&mac_entry_value);

    ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA_clear(&tbl_data);

    
    if (block_range->iter > DPP_PP_LEM_NOF_ENTRIES(unit) - 1)
    {
        *nof_entries = 0;
        SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));
        ARAD_PP_DO_NOTHING_AND_EXIT;
    }

    res = arad_pp_frwrd_mact_rule_to_key_convert(
            unit,
            rule,
            &rule_key,
            &rule_key_mask,
            0
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    payload_array = (ARAD_PP_LEM_ACCESS_PAYLOAD*) sal_alloc( sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * (block_range->entries_to_act), "arad_pp_lem_block_get");
    if (payload_array == NULL)
    {
        SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 20, exit);
    }
    sal_memset(payload_array, 0, sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * (block_range->entries_to_act));

    lem_key_array = (ARAD_PP_LEM_ACCESS_KEY*) sal_alloc( sizeof(ARAD_PP_LEM_ACCESS_KEY) * (block_range->entries_to_act), "arad_pp_lem_block_get");
    if (lem_key_array == NULL)
    {
        SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 30, exit);
    }
    sal_memset(lem_key_array, 0, sizeof(ARAD_PP_LEM_ACCESS_KEY) * (block_range->entries_to_act));

    res = _arad_pp_lem_traverse_bitmap_build(unit, &rule_key, &rule_key_mask, &(rule->value_rule), ARAD_PP_FRWRD_MACT_RULE_MASK_CONV_0_IGNORE, &tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

    arad_pp_lem_block_get(unit, (void *)&tbl_data, block_range, lem_key_array, payload_array, &nof_total_entries);

    
    *nof_entries = 0;
    for (i = 0; i < nof_total_entries; i++)
    {
        is_key_for_rule = TRUE;
        lem_key = &lem_key_array[i];

        
        if (rule->key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR
                && rule->key_rule.mac.fid_mask == 0
                && SOC_SAND_PP_MAC_ADDRESS_IS_ZERO(rule->key_rule.mac.mac_mask))
        {
            check_key = 0;
        }
        else if (rule->key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC
                && rule->key_rule.ipv4_mc.dip_mask == 0
                && rule->key_rule.ipv4_mc.fid_mask == 0)
        {
            check_key = 0;
        }
        else
        {
            check_key = 1;
            switch (rule->key_type)
            {
            case SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR:
                key_type = ARAD_PP_LEM_ACCESS_KEY_TYPE_MAC;
                break;
            case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC:
                key_type = ARAD_PP_LEM_ACCESS_KEY_TYPE_IPV4_MC;
                break;
            default:
                SOC_SAND_SET_ERROR_CODE(
                        SOC_PPC_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR, 20, exit);
            }
        }

        if (lem_key->type != key_type)
        {
#if ARAD_PP_MACT_DEBUG_PRINT
            LOG_INFO(BSL_LS_SOC_FORWARD,
            (BSL_META_U(unit,
                    "if(lem_key->type (%d) != key_type (%d)) \n\r"), lem_key->type, key_type));
#endif

            continue;
        }

        if (check_key)
        {
        for (param_ndx = 0; param_ndx < ARAD_PP_LEM_KEY_MAX_NOF_PARAMS; ++param_ndx)
            {
          for (value_ndx = 0; value_ndx < ARAD_PP_LEM_KEY_PARAM_MAX_IN_UINT32S(LEM); ++value_ndx)
                {
                    if ((lem_key->param[param_ndx].value[value_ndx]
                            & rule_key_mask.param[param_ndx].value[value_ndx])
                            != (rule_key.param[param_ndx].value[value_ndx]
                                    & rule_key_mask.param[param_ndx].value[value_ndx]))
                    {
#if ARAD_PP_MACT_DEBUG_PRINT
        LOG_INFO(BSL_LS_SOC_FORWARD,
                 (BSL_META_U(unit,
                             "value_ndx %d, lem_key.value[value_ndx] %d, rule_key_mask.value[value_ndx] %d, "
                             "rule_key.value[value_ndx] %d  \n\r"), 
                  value_ndx, lem_key->param[param_ndx].value[value_ndx], 
                  rule_key_mask.param[param_ndx].value[value_ndx], rule_key.param[param_ndx].value[value_ndx]));
#endif
                        is_key_for_rule = FALSE;
                        break;
                    }
                }
                if (!is_key_for_rule)
                {
                    break;
                }
            }
            if (!is_key_for_rule)
            { 
                continue;
            }
        }

        
        res = arad_pp_frwrd_mact_payload_convert(unit, &payload_array[i], &mac_entry_value);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


        res = arad_pp_frwrd_mact_key_parse(unit, &lem_key_array[i], &mac_key);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

        ARAD_COPY(&(mact_keys[*nof_entries]), &(mac_key),         SOC_PPC_FRWRD_MACT_ENTRY_KEY,   1);
        ARAD_COPY(&(mact_vals[*nof_entries]), &(mac_entry_value), SOC_PPC_FRWRD_MACT_ENTRY_VALUE, 1);
        (*nof_entries)++;

    }

    exit:

    if (payload_array != NULL)
        sal_free(payload_array);
    if (lem_key_array != NULL)
        sal_free(lem_key_array);

    SOC_SAND_EXIT_AND_SEND_ERROR(
            "error in arad_pp_frwrd_mact_get_sw_block_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_get_block_unsafe(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE         *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                   *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY                   *mact_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE                 *mact_vals,
    SOC_SAND_OUT uint32                                         *nof_entries
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_GET_BLOCK_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(rule);
    SOC_SAND_CHECK_NULL_INPUT(block_range);
    SOC_SAND_CHECK_NULL_INPUT(mact_keys);
    SOC_SAND_CHECK_NULL_INPUT(mact_vals);
    SOC_SAND_CHECK_NULL_INPUT(nof_entries);

    *nof_entries = 0;

    return _arad_pp_frwrd_mact_get_block_unsafe(unit,  rule, block_range, mact_keys, mact_vals, nof_entries);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_get_block_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_get_counter_by_block_unsafe(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE         *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                   *block_range,
    SOC_SAND_OUT uint32                                         *nof_entries
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_GET_BLOCK_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(rule);
    SOC_SAND_CHECK_NULL_INPUT(block_range);
    SOC_SAND_CHECK_NULL_INPUT(nof_entries);

    *nof_entries = 0;

    return _arad_pp_frwrd_mact_get_counter_by_block_unsafe(unit,  rule, block_range, nof_entries);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_get_counter_by_block_unsafe()", 0, 0);
}



STATIC uint32
  _arad_pp_frwrd_mact_get_block_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE *rule,    
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE           *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY           *mact_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE         *mact_vals,
    SOC_SAND_OUT uint32                                 *nof_entries
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE mac_entry_value;
    ARAD_PP_LEM_ACCESS_KEY rule_key, rule_key_mask;
    uint32 result_index;
    ARAD_PP_LEM_ACCESS_KEY            *read_keys=NULL;
    ARAD_PP_LEM_ACCESS_PAYLOAD        *read_vals=NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_GET_BLOCK_UNSAFE);     

    SOC_SAND_CHECK_NULL_INPUT(rule);    
    SOC_SAND_CHECK_NULL_INPUT(mact_keys);
    SOC_SAND_CHECK_NULL_INPUT(mact_vals);
  
    SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(mact_keys);
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(mact_vals);
    ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key);
    ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key_mask);
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&mac_entry_value);

    res = arad_pp_frwrd_mact_rule_to_key_convert(
            unit,
            rule,
            &rule_key,
            &rule_key_mask,
            0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

          
    read_keys = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range->entries_to_act,"read_keys");
    read_vals = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * block_range->entries_to_act,"read_vals");

    res = _arad_pp_frwrd_lem_get_block_unsafe(
                unit,
                &rule_key,
                &rule_key_mask,
                &rule->value_rule,
                0,
                block_range,
                read_keys,
                read_vals,
                nof_entries);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    for (result_index=0; result_index < *nof_entries; result_index++)
    {        
        res = arad_pp_frwrd_mact_payload_convert(
            unit,
            &read_vals[result_index],
            &(mact_vals[result_index]));
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
              
        res = arad_pp_frwrd_mact_key_parse(
            unit,
            &read_keys[result_index],
            &(mact_keys[result_index]));
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

exit:
    if (read_keys!=NULL) {
        soc_sand_os_free_any_size(read_keys);
    }

    if (read_vals!=NULL) {
        soc_sand_os_free_any_size(read_vals);
    }

    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_frwrd_mact_get_block_unsafe()", 0, 0);
}

STATIC uint32
  _arad_pp_frwrd_mact_get_counter_by_block_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE *rule,    
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE           *block_range,
    SOC_SAND_OUT uint32                                 *nof_entries
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE mac_entry_value;
    ARAD_PP_LEM_ACCESS_KEY rule_key, rule_key_mask;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_GET_BLOCK_UNSAFE);     

    SOC_SAND_CHECK_NULL_INPUT(rule);    
  
    ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key);
    ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key_mask);
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&mac_entry_value);

    res = arad_pp_frwrd_mact_rule_to_key_convert(
            unit,
            rule,
            &rule_key,
            &rule_key_mask,
            0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    res = _arad_pp_frwrd_lem_get_counter_by_block_unsafe(
                unit,
                &rule_key,
                &rule_key_mask,
                &rule->value_rule,
                0,
                block_range,
                nof_entries);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


exit:

    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_frwrd_mact_get_counter_by_block_unsafe()", 0, 0);
}



STATIC uint32
  _arad_pp_frwrd_lem_get_block_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE   *value_rule,
    SOC_SAND_OUT uint32                                  access_only,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range,
    SOC_SAND_OUT  ARAD_PP_LEM_ACCESS_KEY                 *read_keys,
    SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD              *read_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32

#if ARAD_PP_MACT_DEBUG_PRINT
    fld_val = 0,
#endif
    fifo_size,
    nof_valid_entries = 0,
    current_index=0,
    res = SOC_SAND_OK;
  uint8
    flush_in_run=1,
    check_key,
    is_key_for_rule,
    key_indx, param_indx,
    more_iterations_left=0;
  ARAD_PP_LEM_ACCESS_KEY 
    *lem_key; 
  uint32
    param_ndx,
    value_ndx;
  SOC_PPC_FRWRD_MACT_ENTRY_KEY
    mac_key;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    *payload;  
  soc_reg_above_64_val_t
    reg_above_64_val;
  uint32
    reply_ready;
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION
    action;
  uint32
    traverse_nof_matched_entries;
  ARAD_PP_LEM_ACCESS_OUTPUT
    request_all;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack_status;
  soc_reg_t
    interrupt_reg = SOC_IS_JERICHO(unit) ? PPDB_B_LARGE_EM_INTERRUPT_REGISTER_TWOr : IHP_MACT_INTERRUPT_REGISTER_TWOr;
  soc_field_t
    reply_ready_field = SOC_IS_JERICHO(unit) ? LARGE_EM_REPLY_READYf : MACT_REPLY_READYf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_GET_BLOCK_UNSAFE);
  
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);  
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&mac_key);  

  ARAD_PP_LEM_ACCESS_KEY_clear(read_keys);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(read_vals);

#ifdef PLISIM
  if (SAL_BOOT_PLISIM) {
	  
	  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 8050, exit);
  }
#endif  

#if ARAD_PP_MACT_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "Entering _arad_pp_frwrd_lem_get_block_unsafe \n\r")));
#endif

  
  if (block_range->iter > DPP_PP_LEM_NOF_ENTRIES(unit) - 1)
  {
    *nof_entries = 0;
    SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));
    ARAD_PP_DO_NOTHING_AND_EXIT;
  }

  
  
   SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &current_index));
#if ARAD_PP_MACT_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "flush-cur-index: %u \n\r"), current_index));
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "block_range->iter: %u \n\r"), block_range->iter));
#endif

  
  if((block_range->iter == 0) || (current_index != block_range->iter))
  {
#if ARAD_PP_MACT_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "run flush db again from index: %u \n\r"), block_range->iter));
#endif

    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&action);
    action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE;
    action.update_mask = 0;

    
    if(block_range->iter == 0) {
#if ARAD_PP_MACT_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "new run clear: exist events \n\r")));
#endif

        
        if(current_index != 0) {
            
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  7,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf,  0));
        }

        
        reply_ready = 1;
        while(reply_ready)
        {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, interrupt_reg, REG_PORT_ANY, 0, reply_ready_field, &reply_ready));
          if(!reply_ready)
          {
            break;
          }
          
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, READ_PPDB_B_LARGE_EM_REPLYr(unit, reg_above_64_val));
        }

        
#if ARAD_PP_MACT_DEBUG_PRINT
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_HIT_COUNTERr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_HIT_COUNTERf, &fld_val));
        LOG_INFO(BSL_LS_SOC_FORWARD,
                 (BSL_META_U(unit,
                             "hit in clear %u\n\r"), fld_val));
#endif
    }

    
    res = arad_pp_frwrd_lem_traverse_internal_unsafe(
            unit,
            rule_key,
            rule_key_mask,
            value_rule,
            block_range,
            &action,
            FALSE, 
            &traverse_nof_matched_entries
          );

    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
#if ARAD_PP_MACT_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "After arad_pp_frwrd_lem_traverse_internal_unsafe: traverse_nof_matched_entries %d \n\r"), traverse_nof_matched_entries));
#endif

    
  }

  

  
  check_key = 0;

  for (key_indx = 0; key_indx < rule_key_mask->nof_params; ++key_indx) {
      for (param_indx = 0; param_indx < ARAR_PP_MACT_FLD_NOF_U32(rule_key_mask->param[key_indx].nof_bits); ++param_indx) {
          if (rule_key_mask->param[key_indx].value[param_indx] != 0) {
              check_key = 1;
              break;
          }
      }
      if (check_key) {
          break;
      }
  }

  
  reply_ready = 1;
  current_index = 1;
  
  while(current_index != 0 || reply_ready)
  {
#if ARAD_PP_MACT_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "In while(current_index != 0 || reply_ready) \n\r")));
#endif
    
    if(block_range->entries_to_act >= ARAD_PP_MACT_REPLY_FIFO_SIZE && flush_in_run) {
#if ARAD_PP_MACT_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "In if(block_range->entries_to_act >= ARAD_PP_MACT_REPLY_FIFO_SIZE && flush_in_run) \n\r")));
#endif
        
        current_index = 1;
        fifo_size = 0;

        
        if(nof_valid_entries + ARAD_PP_MACT_REPLY_FIFO_SIZE > block_range->entries_to_act) {
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 1070, exit);
        }

        if(block_range->entries_to_act == ARAD_PP_MACT_REPLY_FIFO_SIZE + nof_valid_entries) {
            while(fifo_size < ARAD_PP_MACT_REPLY_FIFO_SIZE && current_index !=0) {
                
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_REPLY_FIFO_REPLY_FIFO_ENTRY_COUNTr, REG_PORT_ANY, 0, LARGE_EM_REPLY_FIFO_REPLY_FIFO_ENTRY_COUNTf, &fifo_size));
                
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &current_index));
                fifo_size += 1;
            }
#if ARAD_PP_MACT_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "Out while(fifo_size), fifo_size %d, current_index %d \n\r"), fifo_size, current_index));
#endif

            
            if(fifo_size >= ARAD_PP_MACT_REPLY_FIFO_SIZE) {
                if(current_index==0) {
                    
                    SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));
                }
                else{
                    more_iterations_left = 1;
                    block_range->iter = current_index;
                }

                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &current_index));
#ifdef CRASH_RECOVERY_SUPPORT
                soc_hw_set_immediate_hw_access(unit);
#endif
                
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf,  0));
                
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_REPORT_HITSf,  0));
#ifdef CRASH_RECOVERY_SUPPORT
                soc_hw_restore_immediate_hw_access(unit);
#endif

                flush_in_run = 0;
            }

            if(current_index == 0) {
                flush_in_run = 0;
                
                SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));
            }
        }
#if ARAD_PP_MACT_DEBUG_PRINT
    LOG_INFO(BSL_LS_SOC_FORWARD,
             (BSL_META_U(unit,
                         "Out of if(block_range->entries_to_act >= ARAD_PP_MACT_REPLY_FIFO_SIZE && flush_in_run) \n\r")));
#endif
    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &current_index));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, interrupt_reg, REG_PORT_ANY, 0, reply_ready_field, &reply_ready));
    if(!reply_ready)
    {
#if ARAD_PP_MACT_DEBUG_PRINT
    LOG_INFO(BSL_LS_SOC_FORWARD,
             (BSL_META_U(unit,
                         "In if(!reply_ready), current_index %d, nof_valid_entries %d \n\r"), 
                         current_index, nof_valid_entries));
#endif
      continue;
    }

    ARAD_PP_LEM_ACCESS_OUTPUT_clear(&request_all);

    
      res = arad_pp_lem_access_parse(
            unit,
            0, 
            &request_all,
            &ack_status);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      
      

      
      
      if(request_all.request.command != ARAD_PP_LEM_ACCESS_CMD_DEFRAG){
#if ARAD_PP_MACT_DEBUG_PRINT
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "if(request_all.request.command (%d) != ARAD_PP_LEM_ACCESS_CMD_DEFRAG) \n\r"), request_all.request.command));
#endif
        continue;
      }

      lem_key = &(request_all.request.key);
      is_key_for_rule = TRUE;
      payload = &(request_all.payload);

      if ((lem_key->type != rule_key->type) && rule_key->type != ARAD_PP_LEM_ACCESS_ALL_TYPES)
      {
          continue;
      }

      
      if ((value_rule != NULL) &&
          (value_rule->val.aging_info.age_status != ARAD_PP_FRWRD_MACT_ILLEGAL_AGE_STATUS) &&
          (value_rule->val.aging_info.age_status != payload->age)) {
          continue;
      }
      if(check_key) {
          for (param_ndx = 0; param_ndx < ARAD_PP_LEM_KEY_MAX_NOF_PARAMS; ++param_ndx)
          {
            for (value_ndx = 0; value_ndx < ARAD_PP_LEM_KEY_PARAM_MAX_IN_UINT32S(LEM); ++value_ndx)
            {
              if ((lem_key->param[param_ndx].value[value_ndx] & rule_key_mask->param[param_ndx].value[value_ndx])
                != (rule_key->param[param_ndx].value[value_ndx] & rule_key_mask->param[param_ndx].value[value_ndx]))
              {
#if ARAD_PP_MACT_DEBUG_PRINT
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "is_key_for_rule %d, value_ndx %d, lem_key.value[value_ndx] %d, rule_key_mask->value[value_ndx] %d, "
                               "rule_key->value[value_ndx] %d  \n\r"), 
                    is_key_for_rule, value_ndx, lem_key->param[param_ndx].value[value_ndx], 
                    rule_key_mask->param[param_ndx].value[value_ndx], rule_key->param[param_ndx].value[value_ndx]));
#endif
                is_key_for_rule = FALSE;

                break;
              }
            }
            if (!is_key_for_rule) {
                break;
            }
          }
          if (!is_key_for_rule) { 
              continue;
          }
      }

        ARAD_COPY(&(read_keys[nof_valid_entries]), (lem_key), ARAD_PP_LEM_ACCESS_KEY, 1);
        ARAD_COPY(&(read_vals[nof_valid_entries]), (payload), ARAD_PP_LEM_ACCESS_PAYLOAD, 1);
        nof_valid_entries++;
#if ARAD_PP_MACT_DEBUG_PRINT
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "nof_valid_entries ++, (%d)  \n\r"), nof_valid_entries));
#endif

        if(nof_valid_entries >= block_range->entries_to_act)
        {
#if ARAD_PP_MACT_DEBUG_PRINT
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "In nof_valid_entries (%d) >= block_range->entries_to_act (%d)  \n\r"), 
                               nof_valid_entries, block_range->entries_to_act));
#endif
          
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &current_index));
          
          *nof_entries = nof_valid_entries;
            
#if ARAD_PP_MACT_DEBUG_PRINT
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  95,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_HIT_COUNTERr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_HIT_COUNTERf, &fld_val));
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "get-block done, hit %u\n\r"), fld_val));
#endif
            
            reply_ready = 1;
            while(reply_ready) {
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, interrupt_reg, REG_PORT_ANY, 0, reply_ready_field, &reply_ready));
                if(reply_ready) {
                    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, READ_PPDB_B_LARGE_EM_REPLYr(unit, reg_above_64_val));
                }
            }
              goto exit;
#if ARAD_PP_MACT_DEBUG_PRINT
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "Out nof_valid_entries (%d) >= block_range->entries_to_act (%d)  \n\r"), 
                               nof_valid_entries, block_range->entries_to_act));
#endif
      }
  } 
  
#if ARAD_PP_MACT_DEBUG_PRINT
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  95,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_HIT_COUNTERr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_HIT_COUNTERf, &fld_val));
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "get-all mact done, hit %u\n\r"), fld_val));
#endif

  if (!more_iterations_left) {
      
      SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));
  }
  *nof_entries = nof_valid_entries;
#if ARAD_PP_MACT_DEBUG_PRINT
          LOG_INFO(BSL_LS_SOC_FORWARD,
                   (BSL_META_U(unit,
                               "*nof_entries (%d), block_range->iter (%d)  \n\r"), 
                               *nof_entries, block_range->iter));
#endif

#endif 


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_frwrd_mact_get_block_unsafe()", 0, 0);
}

uint32
  _arad_pp_frwrd_lem_traverse_counter_internal_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION        *action,
    SOC_SAND_IN  uint8                                 wait_till_finish,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  )
{
    int
      is_wait_success;
    uint32
      fld_val,
      res = SOC_SAND_OK;
    uint32
      wait_loop_ndx,
      wait_loop_nof_iteraions,
      waiting_time_in_ms = 0,
      entry_offset = 0;
    ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA
      tbl_data;
    uint8
      traverse_mode,
      should_wait_be_done=1,
      waiting=1;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(rule_key);
    SOC_SAND_CHECK_NULL_INPUT(rule_key_mask);
    SOC_SAND_CHECK_NULL_INPUT(block_range);
    SOC_SAND_CHECK_NULL_INPUT(action);
    SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);

    wait_loop_nof_iteraions = ARAD_PP_FRWRD_MACT_TABLE_TRAVERSE_NOF_LOOPS;
    if (SOC_DPP_CONFIG(unit)->emulation_system)
    {
        wait_loop_nof_iteraions = wait_loop_nof_iteraions * 100;
    }

    
    if(action->type != SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT)
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
    }

    
    fld_val = 0x0;
    if(wait_till_finish)
    {
        fld_val = 0;
    }
    else
    {
        if(block_range->iter == 0)
        {
            fld_val = DPP_PP_LEM_NOF_ENTRIES(unit) - block_range->entries_to_act;
        }
        else
        {
            fld_val = (block_range->iter <= block_range->entries_to_act) ? 0 : block_range->iter - block_range->entries_to_act;
        }
    }

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_END_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_END_INDEXf, fld_val));
    res = arad_pp_lem_traverse_bitmap_build(unit, rule_key, rule_key_mask, rule_val, &tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 29, exit);
    tbl_data.action_transplant_accessed_clear = SOC_SAND_BOOL2NUM(action->update_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ACCESSED);
    tbl_data.action_drop = 0x0;

    
    fld_val = 0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_REPORT_HITSf,  fld_val));

    
    COMPILER_64_SET(tbl_data.action_transplant_payload_data, 0x0, 0x0);
    COMPILER_64_SET(tbl_data.action_transplant_payload_mask, 0x0, 0x0);

    
    res = arad_pp_frwrd_lem_traverse_db_get_next_available(unit,&entry_offset);
    SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

    if (entry_offset == ARAD_PP_MACT_NOF_FLUSH_ENTRIES) 
    { 
        SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 120, exit);
    }

    
    res = arad_pp_ihp_flush_db_tbl_set_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

    res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.traverse_mode.get(unit, &traverse_mode);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);
  
    
    if (traverse_mode == SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_AGGREGATE ) 
    {
        
        res = arad_pp_sw_db_mact_traverse_flush_entry_use_set(
              unit,
              entry_offset,
              1
            );
        SOC_SAND_CHECK_FUNC_RESULT(res,  65, exit);
        ARAD_DO_NOTHING_AND_EXIT;
    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_HIT_COUNTERr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_HIT_COUNTERf, &fld_val));

    
    if(block_range->iter == 0) 
    {
        fld_val = DPP_PP_LEM_NOF_ENTRIES(unit);
    }
    else
    {
        fld_val =  block_range->iter;
    }

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf,  fld_val));

    
	waiting = wait_till_finish && should_wait_be_done;

    if (waiting)
    {
        
        is_wait_success = FALSE;
        for (wait_loop_ndx = 0; wait_loop_ndx < wait_loop_nof_iteraions; ++wait_loop_ndx)
        {
            res = _arad_pp_frwrd_lem_traverse_check_flush_is_done(unit, nof_matched_entries, &waiting_time_in_ms, &is_wait_success);
            SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);
            if (!is_wait_success) 
            {
                res = soc_sand_os_task_delay_milisec(waiting_time_in_ms);
             SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
            } 
            else 
            {
                break;
            }
        }

        if (is_wait_success == FALSE)
        {
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_TABLE_TRAVERSE_WAITING_ERR, 90, exit);
        }
    } 
    else
    {
        
        *nof_matched_entries = 0;
    }

exit:
	SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_frwrd_lem_traverse_counter_internal_unsafe()", 0, 0);
}

STATIC uint32
  _arad_pp_frwrd_lem_get_counter_by_block_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE   *value_rule,
    SOC_SAND_OUT uint32                                  access_only,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
    uint32
      current_index = 0, end_index = 0,
      res = SOC_SAND_OK;
    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION
      action;
    uint32
      traverse_nof_matched_entries = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_GET_BLOCK_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(block_range);
    SOC_SAND_CHECK_NULL_INPUT(nof_entries);    

#ifdef PLISIM
    if (SAL_BOOT_PLISIM) 
    {
        
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 8050, exit);
    }
#endif

    
    if (block_range->iter > DPP_PP_LEM_NOF_ENTRIES(unit) - 1)
    {
        *nof_entries = 0;
        SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));
        ARAD_PP_DO_NOTHING_AND_EXIT;
    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &current_index));

    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&action);
    action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT;
    action.update_mask = 0;

    
    if(block_range->iter == 0) 
    {
        
        if(current_index != 0)
        {
            
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  7,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf,  0));
        }
    }

    
    if(block_range->iter == 0) 
    {
        end_index = DPP_PP_LEM_NOF_ENTRIES(unit) - block_range->entries_to_act;
    }
    else{
        end_index = (block_range->iter <= block_range->entries_to_act) ? 0 : block_range->iter - block_range->entries_to_act;
    }
    res = soc_sand_os_task_delay_milisec(1000);
    SOC_SAND_CHECK_FUNC_RESULT(res, 29, exit);
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_HIT_COUNTERr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_HIT_COUNTERf, &traverse_nof_matched_entries));

    
    res = _arad_pp_frwrd_lem_traverse_counter_internal_unsafe(
            unit,
            rule_key,
            rule_key_mask,
            value_rule,
            block_range,
            &action,
            FALSE, 
            &traverse_nof_matched_entries
          );

    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &current_index));
    
    while (current_index > end_index)
    {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &current_index));
    }

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_HIT_COUNTERr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_HIT_COUNTERf, &traverse_nof_matched_entries));
    *nof_entries = traverse_nof_matched_entries;

    if(current_index == 0)
    {
        
        SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));
    }
    else
    {
        block_range->iter = current_index;
        
        res = _arad_pp_frwrd_lem_traverse_counter_internal_unsafe(
              unit,
              rule_key,
              rule_key_mask,
              value_rule,
              block_range,
              &action,
              TRUE, 
              &traverse_nof_matched_entries
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_frwrd_lem_get_counter_by_block_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_lem_traverse_by_mac_limit_per_tunnel_internal_unsafe(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                       *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                       *rule_key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
    SOC_SAND_INOUT    SOC_SAND_TABLE_BLOCK_RANGE              *block_range,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION           *action,
    SOC_SAND_OUT uint32                                       *nof_matched_entries
    
  )
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_UNSAFE);
    SOC_SAND_CHECK_NULL_INPUT(rule_key);
    SOC_SAND_CHECK_NULL_INPUT(rule_key_mask);
    
    SOC_SAND_CHECK_NULL_INPUT(action);
    SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);

    *nof_matched_entries = 0;

    return _arad_pp_frwrd_lem_traverse_by_mac_limit_per_tunnel_internal_unsafe(unit, rule_key, rule_key_mask, rule_val, action, nof_matched_entries);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_lem_traverse_by_mac_limit_per_tunnel_internal_unsafe()", 0, 0);
}

uint32
  _arad_pp_frwrd_lem_traverse_by_mac_limit_per_tunnel_internal_unsafe(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                       *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                       *rule_key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION           *action,
    SOC_SAND_OUT uint32                                       *nof_matched_entries
  )
{
  uint32
    reply_ready,
    fld_val,
    one_val = 0x1,
    payload_ndx,
    current_index,
    lem_key_length,
    fifo_size,
    end_index,
    inv_index,
    payload_data[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
    payload_mask[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
    payload_new_data[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
    payload_new_mask[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
    res = SOC_SAND_OK;
  uint32
    entry_offset = 0;
  ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA
    tbl_data;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload_new,
    payload;
  ARAD_PP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer_mask,
    key_in_buffer;
  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO
    status;
  uint8
    traverse_mode;
  soc_reg_t
    interrupt_reg = SOC_IS_JERICHO(unit) ? PPDB_B_LARGE_EM_INTERRUPT_REGISTER_TWOr : IHP_MACT_INTERRUPT_REGISTER_TWOr;
  soc_field_t
    reply_ready_field = SOC_IS_JERICHO(unit) ? LARGE_EM_REPLY_READYf : MACT_REPLY_READYf; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_BY_MAC_LIMIT_PER_TUNNEL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rule_key);
  SOC_SAND_CHECK_NULL_INPUT(rule_key_mask);
  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);
  
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload_new);
  ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);
  ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer_mask);
  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_clear(&status);

  lem_key_length = SOC_DPP_DEFS_GET(unit, lem_width);

  for (payload_ndx = 0; payload_ndx < ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S; ++payload_ndx)
  {
    payload_data[payload_ndx] = 0;
    payload_new_data[payload_ndx] = 0;
    payload_mask[payload_ndx] = ARAD_PP_FRWRD_MACT_MASK_DEFAULT_NULL;
    payload_new_mask[payload_ndx] = ARAD_PP_FRWRD_MACT_MASK_DEFAULT_NULL;
  }

  if (rule_val != NULL)
  {
    res = arad_pp_frwrd_mact_payload_mask_build(
          unit,
          &(rule_val->val),
          &(rule_val->val_mask),
          rule_val->compare_mask,
          payload_data,
          payload_mask,
          1
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    
    payload_mask[1] &= 0x7ff;
  }

  
  tbl_data.compare_valid = TRUE;
 
  tbl_data.compare_key_data_location = 1;
  
  res = arad_pp_lem_key_encoded_build(
          unit,
          rule_key,
          0,
          &key_in_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_lem_key_encoded_build(
          unit,
          rule_key_mask,
          1,
          &key_in_buffer_mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for(inv_index = 0; inv_index < SOC_DPP_DEFS_LEM_WIDTH_IN_UINT32S(unit); ++inv_index) 
  {
    key_in_buffer_mask.buffer[inv_index] = ~ key_in_buffer_mask.buffer[inv_index];
  }

  
  
  tbl_data.compare_key_20_data = 0;
  SHR_BITCOPY_RANGE(&tbl_data.compare_key_20_data,0, key_in_buffer.buffer, 48, 16);
  SHR_BITCOPY_RANGE(&tbl_data.compare_key_20_data,16, key_in_buffer.buffer, (lem_key_length-4), 4); 
  
  tbl_data.compare_key_20_mask = 0;
  SHR_BITCOPY_RANGE(&tbl_data.compare_key_20_mask,0, key_in_buffer_mask.buffer, 48, 15);

  
  SHR_BITCOPY_RANGE(&tbl_data.compare_key_20_mask,15, &one_val, 0, 1);

  SHR_BITCOPY_RANGE(&tbl_data.compare_key_20_mask,16, key_in_buffer_mask.buffer, (lem_key_length-4), 4); 
  
  COMPILER_64_SET(tbl_data.compare_payload_data, payload_data[1], payload_data[0]);
  COMPILER_64_SET(tbl_data.compare_payload_mask, payload_mask[1], payload_mask[0]);

  
  if (rule_val != NULL) 
  {
      tbl_data.compare_accessed_data = SOC_SAND_BOOL2NUM(rule_val->val.accessed);
      tbl_data.compare_accessed_mask = (rule_val->compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ACCESSED)?0:1;
  }
  else
  {
      tbl_data.compare_accessed_data = 0;
      tbl_data.compare_accessed_mask = 1;
  }

  tbl_data.action_transplant_accessed_clear = SOC_SAND_BOOL2NUM(action->update_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ACCESSED);
  if (action->type == SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE)
  {
    tbl_data.action_drop = 0x1;
  }
  else
  {
    tbl_data.action_drop = 0x0;
  }

  
  fld_val = 1;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_REPORT_HITSf,  fld_val));

  if(action->type == SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE)
  {
    COMPILER_64_SET(tbl_data.action_transplant_payload_data, 0x0, 0x0);
    COMPILER_64_SET(tbl_data.action_transplant_payload_mask, 0x0, 0x0);
  }
  else if (action->type == SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_UPDATE)
  {
    res = arad_pp_frwrd_mact_payload_mask_build(
            unit,
            &action->updated_val,
            &action->updated_val_mask,
            action->update_mask,
            payload_new_data,
            payload_new_mask,
            0
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    COMPILER_64_SET(tbl_data.action_transplant_payload_data, payload_new_data[1], payload_new_data[0]);
    COMPILER_64_SET(tbl_data.action_transplant_payload_mask, payload_new_mask[1], payload_new_mask[0]);
  }
  else
  {
    
    COMPILER_64_SET(tbl_data.action_transplant_payload_data, 0x0, 0x0);
    COMPILER_64_SET(tbl_data.action_transplant_payload_mask, 0x0, 0x0);
  }

  
  res = arad_pp_frwrd_lem_traverse_db_get_next_available(unit,&entry_offset);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  if (entry_offset == ARAD_PP_MACT_NOF_FLUSH_ENTRIES)
  {
    
    SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 120, exit);
  }

  
  res = arad_pp_ihp_flush_db_tbl_set_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.traverse_mode.get(unit, &traverse_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 62, exit);
  
  
  if (traverse_mode == SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_AGGREGATE ) 
  {
    
    res = arad_pp_sw_db_mact_traverse_flush_entry_use_set(
            unit,
            entry_offset,
            1
          );
    SOC_SAND_CHECK_FUNC_RESULT(res,  65, exit);
    ARAD_DO_NOTHING_AND_EXIT;
  }

  
  end_index = DPP_PP_LEM_NOF_ENTRIES(unit) - ARAD_PP_MACT_REPLY_FIFO_SIZE;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_END_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_END_INDEXf,  end_index));

  
  current_index = DPP_PP_LEM_NOF_ENTRIES(unit); 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf,  current_index));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &current_index));
  while (current_index != 0)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &current_index));
    if (current_index == end_index)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_REPLY_FIFO_REPLY_FIFO_ENTRY_COUNTr, REG_PORT_ANY, 0, LARGE_EM_REPLY_FIFO_REPLY_FIFO_ENTRY_COUNTf, &fifo_size));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_HIT_COUNTERr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_HIT_COUNTERf, &fld_val));
      *nof_matched_entries += fld_val;

      if (fifo_size < ARAD_PP_MACT_REPLY_FIFO_SIZE)
      {
        fifo_size++;
      }
      if(current_index > (ARAD_PP_MACT_REPLY_FIFO_SIZE - fifo_size))
      {
        end_index = current_index - ARAD_PP_MACT_REPLY_FIFO_SIZE + fifo_size;
      }
      else
      {
        end_index = 0;
      }
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_END_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_END_INDEXf,  end_index));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf,  current_index));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &current_index));
    }
  }

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 110,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, interrupt_reg, REG_PORT_ANY, 0, reply_ready_field, &reply_ready));
  while(reply_ready != 0)
  {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 110,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, interrupt_reg, REG_PORT_ANY, 0, reply_ready_field, &reply_ready));
  }
  
  tbl_data.compare_valid = 0;
  res = arad_pp_ihp_flush_db_tbl_set_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

#if ARAD_PP_MACT_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "Reply-FIFO had clear by ARM CPU\n")));
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "*nof_matched_entries: %u \n\r"), *nof_matched_entries));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &payload_ndx));
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "flush-cur-index: %u \n\r"), payload_ndx));
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_frwrd_lem_traverse_by_mac_limit_per_tunnel_internal_unsafe()", 0, 0);
}




uint32
  arad_pp_frwrd_lem_sw_traverse_internal_unsafe(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                     *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                     *rule_key_mask,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION         *action,
    SOC_SAND_OUT uint32                                     *nof_matched_entries,
    SOC_SAND_INOUT    SOC_SAND_TABLE_BLOCK_RANGE            *block_range
  )
{
    uint32          i                          = 0;
    uint32          res                        = SOC_SAND_OK;
    uint8           success                    = FALSE;
    uint32          *key                       = NULL;
    uint32          *value                     = NULL;
    ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA          tbl_data;


    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_SW_TRAVERSE_UNSAFE);

    ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA_clear(&tbl_data);

    
    res = _arad_pp_lem_traverse_bitmap_build(unit, rule_key, rule_key_mask, rule_val, ARAD_PP_FRWRD_MACT_RULE_MASK_CONV_1_IGNORE, &tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
    
    key = (uint32*)sal_alloc(sizeof(uint32) * SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S * (block_range->entries_to_act), "arad_pp_frwrd_lem_sw_traverse_internal_unsafe");
    if (key == NULL)
    {
        SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 10, exit);
    }
    value= (uint32*)sal_alloc(sizeof(uint32) * ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S * (block_range->entries_to_act), "arad_pp_frwrd_lem_sw_traverse_internal_unsafe");
    if (value == NULL)
    {
        SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 20, exit);
    }

    
    res = chip_sim_em_get_block(unit, ARAD_CHIP_SIM_LEM_BASE,  ARAD_CHIP_SIM_LEM_KEY, ARAD_CHIP_SIM_LEM_PAYLOAD,
                                (void *)&tbl_data, key, value, nof_matched_entries, block_range);

    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    switch (action->type)
    {
        case    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_NONE:
            

            LOG_ERROR(BSL_LS_SOC_FORWARD,
                      (BSL_META_U(unit,
                                  "SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_NONE, OK  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\r")));
            break;
        case    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE:
            for (i = 0; i < *nof_matched_entries; i++)
            {
                res = chip_sim_exact_match_entry_remove_unsafe(unit,
                                                               ARAD_CHIP_SIM_LEM_BASE,
                                                               & key[i * SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S],
                                                               ARAD_CHIP_SIM_LEM_KEY);
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
            }
            break;

        case    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_UPDATE:
            for (i = 0; i < *nof_matched_entries; i++)
            {
                res = chip_sim_exact_match_entry_add_unsafe(unit,
                                                            ARAD_CHIP_SIM_LEM_BASE,
                                                            &key[i * SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S],
                                                            ARAD_CHIP_SIM_LEM_KEY,
                                                            &value[i * ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S],
                                                            ARAD_CHIP_SIM_LEM_PAYLOAD,
                                                            &success);
                SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
            }
            break;

        case    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT:
            
            LOG_ERROR(BSL_LS_SOC_FORWARD,
              (BSL_META_U(unit,
                          "SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT, do nothing, not implemented   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\r")));
            break;

        case    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE:
            
            LOG_ERROR(BSL_LS_SOC_FORWARD,
              (BSL_META_U(unit,
                          "SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE, OK   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\r")));
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_FORWARD,
                          (BSL_META_U(unit,
                                      "error in arad_pp_frwrd_lem_sw_traverse_internal_unsafe() - unknown action")));
            SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RANGE_ERR, 30, exit);

    };
exit:
    if (key)
        sal_free(key);
    if (value)
        sal_free(value);

    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_lem_sw_traverse_internal_unsafe()", 0, 0);

}



uint32
  arad_pp_frwrd_lem_traverse_internal_unsafe(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                     *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                     *rule_key_mask,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE               *block_range,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION         *action,
    SOC_SAND_IN  uint8                                      wait_till_finish,
    SOC_SAND_OUT uint32                                     *nof_matched_entries
  )
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_UNSAFE);
    SOC_SAND_CHECK_NULL_INPUT(rule_key);
    SOC_SAND_CHECK_NULL_INPUT(rule_key_mask);
    
    SOC_SAND_CHECK_NULL_INPUT(block_range);
    SOC_SAND_CHECK_NULL_INPUT(action);
    SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);

    *nof_matched_entries = 0;

    return _arad_pp_frwrd_lem_traverse_internal_unsafe(unit, rule_key, rule_key_mask, rule_val, block_range, action, wait_till_finish, nof_matched_entries);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_lem_traverse_internal_unsafe()", 0, 0);
}




uint32
_arad_pp_check_all_flush_actions_are_delete(int unit, uint8 *all_entries_are_delete) {
	uint32 	entry_offset = 0, res;
	ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA tbl_data;

	SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_UNSAFE);

	*all_entries_are_delete = TRUE;

	ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA_clear(&tbl_data);

	for (entry_offset = 0; entry_offset < ARAD_PP_MACT_NOF_FLUSH_ENTRIES; entry_offset++) {
		res = arad_pp_ihp_flush_db_tbl_get_unsafe(
			unit,
			entry_offset,
			&tbl_data
			);
		SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

		if ((tbl_data.compare_valid) && !(tbl_data.action_drop == 1)) {
			*all_entries_are_delete = FALSE;
			break;
		}
	}

exit:
	SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_frwrd_lem_traverse_wait_if_necessary_blocking()", 0, 0);
}


uint32
  _arad_pp_frwrd_lem_traverse_internal_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION        *action,
    SOC_SAND_IN  uint8                                 wait_till_finish,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  )
{
  int
    is_wait_success;
  uint32
    fld_val,
    payload_new_data[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
    payload_new_mask[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
    payload_ndx,
    res = SOC_SAND_OK;
  uint32
    key_indx,
    param_indx,
    wait_loop_ndx,
	wait_loop_nof_iteraions,
    waiting_time_in_ms = 0,
    entry_offset = 0;
  ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA
    tbl_data;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload_new,
    payload;
  ARAD_PP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer_mask,
    key_in_buffer;
  uint8
    check_key,
    check_payload=0,
    traverse_mode,
	should_wait_be_done=1,
    all_are_delete_entries,
	waiting=1;
#ifdef CRASH_RECOVERY_SUPPORT
  uint8 hw_access=0;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rule_key);
  SOC_SAND_CHECK_NULL_INPUT(rule_key_mask);
  
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);
  
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload_new);
  ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);
  ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer_mask);

#ifdef CRASH_RECOVERY_SUPPORT
  
  soc_dcmn_cr_ondemand_em_traverse_hit(unit);

  
  hw_access = ((SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_NONE == action->type) ||
              (SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT == action->type) ||
              (SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE == action->type)) ? 1 : 0;
  if(hw_access) {
      soc_hw_set_immediate_hw_access(unit);
  }
#endif

  wait_loop_nof_iteraions = ARAD_PP_FRWRD_MACT_TABLE_TRAVERSE_NOF_LOOPS;
  if (SOC_DPP_CONFIG(unit)->emulation_system)
  {
	  wait_loop_nof_iteraions = wait_loop_nof_iteraions * 100;
  }

  
  if((action->type != SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT && action->type != SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE)
     && block_range->iter != 0
     ) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
  }

    
  check_key = 0;
  if(action->type == SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT && block_range->iter == 0) {
      
      for (key_indx = 0; key_indx <rule_key->nof_params; ++key_indx) {
          for (param_indx = 0; param_indx< ARAR_PP_MACT_FLD_NOF_U32(rule_key_mask->param[key_indx].nof_bits); ++param_indx) {
              if (rule_key_mask->param[key_indx].value[param_indx]!=0) {
                  check_key = 1;
                  break;
              }
          }
          if (check_key) {
              break;
          }
      }
      
        
      if (rule_val != NULL) {
          check_payload = rule_val->compare_mask != SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_NONE;
      }
#if ARAD_PP_MACT_DEBUG_PRINT
      LOG_INFO(BSL_LS_SOC_FORWARD,
               (BSL_META_U(unit,
                           "read all entries in MAC from reg: %u \n\r"), *nof_matched_entries));
#endif
      if(!check_payload && !check_key && rule_key->type == ARAD_PP_LEM_ACCESS_KEY_TYPE_MAC) {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, READ_PPDB_B_LARGE_EM_COUNTER_LIMIT_LARGE_EM_DB_ENTRIES_COUNTr(unit, nof_matched_entries));
          goto exit; 
      }
  }


  
  fld_val = 0x0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_END_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_END_INDEXf,  fld_val));

  res = arad_pp_lem_traverse_bitmap_build(unit, rule_key, rule_key_mask, rule_val, &tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 29, exit);


  tbl_data.action_transplant_accessed_clear = SOC_SAND_BOOL2NUM(action->update_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ACCESSED);

  if (action->type == SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE)
  {
    tbl_data.action_drop = 0x1;
  }
  else
  {
    tbl_data.action_drop = 0x0;
  }

  
  fld_val = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_REPORT_HITSf,  fld_val));

  if(action->type == SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE)
  {
    fld_val = 1;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_REPORT_HITSf,  fld_val));
    COMPILER_64_SET(tbl_data.action_transplant_payload_data, 0x0, 0x0);
    COMPILER_64_SET(tbl_data.action_transplant_payload_mask, 0x0, 0x0);
  }

  else if (action->type == SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_UPDATE)
  {
    for (payload_ndx = 0; payload_ndx < ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S; ++payload_ndx)
    {
        payload_new_data[payload_ndx] = 0;
        payload_new_mask[payload_ndx] = ARAD_PP_FRWRD_MACT_MASK_DEFAULT_NULL;
    }
    res = arad_pp_frwrd_mact_payload_mask_build(
            unit,
            &action->updated_val,
            &action->updated_val_mask,
            action->update_mask,
            payload_new_data,
            payload_new_mask,
            0
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    COMPILER_64_SET(tbl_data.action_transplant_payload_data, payload_new_data[1], payload_new_data[0]);
    COMPILER_64_SET(tbl_data.action_transplant_payload_mask, payload_new_mask[1], payload_new_mask[0]);
  }
  else
  {
    
    COMPILER_64_SET(tbl_data.action_transplant_payload_data, 0x0, 0x0);
    COMPILER_64_SET(tbl_data.action_transplant_payload_mask, 0x0, 0x0);
  }

  
  res = arad_pp_frwrd_lem_traverse_db_get_next_available(unit,&entry_offset);
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  if (entry_offset == ARAD_PP_MACT_NOF_FLUSH_ENTRIES) { 
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RESOURCES, 120, exit);
  }

  
  res = arad_pp_ihp_flush_db_tbl_set_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.traverse_mode.get(unit, &traverse_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);
  
  
  if (traverse_mode == SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_AGGREGATE ) {
      
      res = arad_pp_sw_db_mact_traverse_flush_entry_use_set(
              unit,
              entry_offset,
              1
            );
    SOC_SAND_CHECK_FUNC_RESULT(res,  65, exit);
    ARAD_DO_NOTHING_AND_EXIT;
  }

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_HIT_COUNTERr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_HIT_COUNTERf, &fld_val));

  
  if(block_range->iter == 0) {
      fld_val = DPP_PP_LEM_NOF_ENTRIES(unit); 
  }
  else{
      fld_val =  block_range->iter;
  }

#if ARAD_PP_MACT_DEBUG_PRINT
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &payload_ndx));
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "get prev flush-cur-index: %u \n\r"), payload_ndx));
#endif

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf,  fld_val));

#if ARAD_PP_MACT_DEBUG_PRINT
      LOG_INFO(BSL_LS_SOC_FORWARD,
               (BSL_META_U(unit,
                           "set flush-cur-index to: %u \n\r"), fld_val));

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &fld_val));
      LOG_INFO(BSL_LS_SOC_FORWARD,
               (BSL_META_U(unit,
                           "get after set flush-cur-index: %u \n\r"), fld_val));
#endif

  
#if ARAD_PP_MACT_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_FORWARD,
           (BSL_META_U(unit,
                       "wait_till_finish: %u \n\r"), wait_till_finish));
#endif
	
	if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "flush_non_blocking_when_all_actions_delete", 0)) {
		all_are_delete_entries = FALSE;
		res = _arad_pp_check_all_flush_actions_are_delete(unit, &all_are_delete_entries);
		SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

		should_wait_be_done = !all_are_delete_entries; 
	}

	waiting = wait_till_finish && should_wait_be_done;

#ifdef CRASH_RECOVERY_SUPPORT
    
    if(BCM_UNIT_DO_HW_READ_WRITE(unit)) {
        res = arad_polling(
          unit,
          ARAD_TIMEOUT*5, 
          ARAD_MIN_POLLS,
          PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr,
          REG_PORT_ANY,
          0,
          LARGE_EM_FLU_MACHINE_CURRENT_INDEXf,
          0
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    
    for (entry_offset = 0; entry_offset < ARAD_PP_MACT_NOF_FLUSH_ENTRIES; entry_offset++) {
         ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA_clear(&tbl_data);
         tbl_data.compare_valid = 0;
         res = arad_pp_ihp_flush_db_tbl_set_unsafe(
                 unit,
                 entry_offset,
                 &tbl_data
                 );
         SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
    }

  
  waiting=0;
}
#endif
	if (waiting) {
		
		is_wait_success = FALSE;
		for (wait_loop_ndx = 0; wait_loop_ndx < wait_loop_nof_iteraions; ++wait_loop_ndx) {
			res = _arad_pp_frwrd_lem_traverse_check_flush_is_done(unit, nof_matched_entries, &waiting_time_in_ms, &is_wait_success);
			SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);
			if (!is_wait_success) {
				res = soc_sand_os_task_delay_milisec(waiting_time_in_ms);
				SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
			} else {
				break;
			}
		}

		if (is_wait_success == FALSE) {
			SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_TABLE_TRAVERSE_WAITING_ERR, 90, exit);
		}
	} else {
		
		*nof_matched_entries = 0;
	}


#if ARAD_PP_MACT_DEBUG_PRINT
	LOG_INFO(BSL_LS_SOC_FORWARD,
			 (BSL_META_U(unit,
						 "*nof_matched_entries: %u \n\r"),*nof_matched_entries));
	SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf, &payload_ndx));
	LOG_INFO(BSL_LS_SOC_FORWARD,
			 (BSL_META_U(unit,
						 "flush-cur-index: %u \n\r"), payload_ndx));
#endif

exit:
#ifdef CRASH_RECOVERY_SUPPORT
    
    if(hw_access) {
        soc_hw_restore_immediate_hw_access(unit);
    }
#endif
	SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_frwrd_lem_traverse_internal_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_lem_get_block_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE   *value_rule,
    SOC_SAND_OUT uint32                                  access_only,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range,
    SOC_SAND_OUT  ARAD_PP_LEM_ACCESS_KEY                 *read_keys,
    SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD              *read_vals,
    SOC_SAND_OUT uint32                                  *nof_entries )
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_LEM_GET_BLOCK_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(key);
    SOC_SAND_CHECK_NULL_INPUT(key_mask);
    SOC_SAND_CHECK_NULL_INPUT(block_range);
    SOC_SAND_CHECK_NULL_INPUT(read_keys);
    SOC_SAND_CHECK_NULL_INPUT(read_vals);
    SOC_SAND_CHECK_NULL_INPUT(nof_entries);

    *nof_entries = 0;

    return _arad_pp_frwrd_lem_get_block_unsafe(unit, key, key_mask, value_rule, access_only, block_range, read_keys, read_vals, nof_entries);

    exit:
      SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_lem_get_block_unsafe()", 0, 0);

}

uint32
  arad_pp_frwrd_mact_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_GET_BLOCK_VERIFY);

  res = SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_verify(unit, rule);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(block_range->entries_to_act, DPP_PP_LEM_NOF_ENTRIES(unit), ARAD_PP_BLOCK_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(block_range->iter, DPP_PP_LEM_NOF_ENTRIES(unit), ARAD_PP_BLOCK_RANGE_ITER_OUT_OF_RANGE_ERR, 22, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_get_block_verify()", 0, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_mact_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_frwrd_mact;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_mact_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_frwrd_mact;
}

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_SAND_PP_MAC_ADDRESS, &(info->mac), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->fid, SOC_DPP_DEFS_GET(unit, nof_fids), SOC_PPC_FID_OUT_OF_RANGE_ERR, 11, exit);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (SOC_SAND_GET_BITS_RANGE(info->dip, ARAD_PP_FRWRD_MACT_IPV4_DIP_MSB, ARAD_PP_FRWRD_MACT_IPV4_DIP_LSB) != ARAD_PP_FRWRD_MACT_IPV4_DIP_VALUE)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_DIP_OUT_OF_RANGE_ERR, 90, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_NOF(info->fid, SOC_DPP_DEFS_GET(unit, nof_fids), SOC_PPC_FID_OUT_OF_RANGE_ERR, 11, exit);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_verify(unit, &(info->mac));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_verify(unit, &(info->ipv4_mc));
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->key_type, SOC_PPC_FRWRD_MACT_KEY_TYPE_MAX, SOC_PPC_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  if ((info->key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR) || (info->key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV6_MC_SSM_EUI))
  {
    res = SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_verify(unit, &(info->key_val.mac));
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    res = SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_verify(unit, &(info->key_val.ipv4_mc));
    SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);
  }
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_ENTRY_KEY_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->age_status, ARAD_PP_FRWRD_MACT_AGE_STATUS_MIN, ARAD_PP_FRWRD_MACT_AGE_STATUS_MAX, ARAD_PP_FRWRD_MACT_AGE_STATUS_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_DECISION_INFO, &(info->forward_decision), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO, &(info->frwrd_info), 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO, &(info->aging_info), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_ENTRY_VALUE_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_TIME_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TIME *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->sec, ARAD_PP_FRWRD_MACT_SEC_MAX, ARAD_PP_FRWRD_MACT_SEC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mili_sec, ARAD_PP_FRWRD_MACT_MILI_SEC_MAX, ARAD_PP_FRWRD_MACT_MILI_SEC_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_TIME_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_verify(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_NOF(info->fid, SOC_DPP_DEFS_GET(unit, nof_fids), SOC_PPC_FID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->fid_mask, SOC_DPP_DEFS_GET(unit, nof_fids), SOC_PPC_FID_OUT_OF_RANGE_ERR, 11, exit);
  
  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_verify(
    SOC_SAND_IN  int                                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  SOC_SAND_ERR_IF_ABOVE_NOF(info->fid, SOC_DPP_DEFS_GET(unit, nof_fids), SOC_PPC_FID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->fid_mask, SOC_DPP_DEFS_GET(unit, nof_fids), SOC_PPC_FID_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_ENTRY_VALUE, &(info->val), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->compare_mask, ARAD_PP_FRWRD_MACT_COMPARE_MASK_MAX, ARAD_PP_FRWRD_MACT_COMPARE_MASK_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_verify(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->key_type, SOC_PPC_FRWRD_MACT_KEY_TYPE_MAX, SOC_PPC_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  if (info->key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR)
  {
    res = SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_verify(unit, &(info->key_rule.mac));
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
  }
  else 
  {
    res = SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_verify(unit, &(info->key_rule.ipv4_mc));
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }

  


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_MAX, ARAD_PP_FRWRD_MACT_ACTION_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_ENTRY_VALUE, &(info->updated_val), 11, exit);

  if(info->updated_val.accessed)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_MACT_ACCESSED_UPDATE_ILLEGAL_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_verify(unit, &(info->rule));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION, &(info->action), 11, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_TIME, &(info->time_to_finish), 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_matched_entries, ARAD_PP_FRWRD_MACT_NOF_MATCHED_ENTRIES_MAX, ARAD_PP_FRWRD_MACT_NOF_MATCHED_ENTRIES_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_verify()",0,0);
}


uint32
  _arad_pp_lem_traverse_bitmap_build(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                         *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                         *rule_key_mask,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE   *rule_val,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_RULE_MASK_CONV              mask_conv,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA             *tbl_data
  )
{
  uint32
    one_val = 0x1,
    payload_ndx,
    inv_index,
    payload_data[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
    payload_mask[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
	
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload_new,
    payload;
  ARAD_PP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer_mask,
    key_in_buffer;
  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO
    status;
  uint32 mask_val = 1;

   
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(tbl_data);
  SOC_SAND_CHECK_NULL_INPUT(rule_key);
  SOC_SAND_CHECK_NULL_INPUT(rule_key_mask);
  
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload_new);
  ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);
  ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer_mask);
  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_clear(&status);

  for (payload_ndx = 0; payload_ndx < ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S; ++payload_ndx)
  {
    payload_data[payload_ndx] = 0;
    
    payload_mask[payload_ndx] = ARAD_PP_FRWRD_MACT_MASK_DEFAULT_NULL;
   
  }

  if (!SOC_DPP_IS_EM_HW_ENABLE) {
      mask_val = 0;
  }

  switch (mask_conv) {
  case ARAD_PP_FRWRD_MACT_RULE_MASK_CONV_0_IGNORE:
      mask_val = 0;
      break;
  case ARAD_PP_FRWRD_MACT_RULE_MASK_CONV_1_IGNORE:
      mask_val = 1;
      break;
  case ARAD_PP_FRWRD_MACT_RULE_MASK_CONV_DONT_CARE:
      break;
  }

  if (rule_val != NULL) {
        res = arad_pp_frwrd_mact_payload_mask_build(
              unit,
              &(rule_val->val),
              &(rule_val->val_mask),
              rule_val->compare_mask,
              payload_data,
              payload_mask,
              mask_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else {
      
      payload_mask[1] &= (SOC_IS_JERICHO(unit) ? 0x1fff : 0x7ff);
  }

  
  tbl_data->compare_valid = TRUE;
 
  tbl_data->compare_key_data_location = 1;
  
  res = arad_pp_lem_key_encoded_build(
          unit,
          rule_key,
          0,
          &key_in_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  res = arad_pp_lem_key_encoded_build(
          unit,
          rule_key_mask,
          1,
          &key_in_buffer_mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

  for(inv_index = 0; inv_index < SOC_DPP_DEFS_LEM_WIDTH_IN_UINT32S(unit); ++inv_index) {
      key_in_buffer_mask.buffer[inv_index] = ~ key_in_buffer_mask.buffer[inv_index];
  }
  

  
  
  tbl_data->compare_key_20_data = 0;
  SHR_BITCOPY_RANGE(&tbl_data->compare_key_20_data,0, key_in_buffer.buffer, 48, 16);
  SHR_BITCOPY_RANGE(&tbl_data->compare_key_20_data,
                    16, 
                    key_in_buffer.buffer, 
                    SOC_DPP_DEFS_GET(unit, lem_width) - 4, 
                    4); 
  
  tbl_data->compare_key_20_mask = 0;
  SHR_BITCOPY_RANGE(&tbl_data->compare_key_20_mask,0, key_in_buffer_mask.buffer, 48, 15);

  
  SHR_BITCOPY_RANGE(&tbl_data->compare_key_20_mask,15, &one_val, 0, 1);

  SHR_BITCOPY_RANGE(&tbl_data->compare_key_20_mask,
                    16, 
                    key_in_buffer_mask.buffer, 
                    SOC_DPP_DEFS_GET(unit, lem_width) - 4, 
                    4); 

  if (!SOC_DPP_IS_EM_HW_ENABLE ||
      mask_conv == ARAD_PP_FRWRD_MACT_RULE_MASK_CONV_0_IGNORE) {
      tbl_data->compare_key_20_mask = (~ tbl_data->compare_key_20_mask) & 0xFFFF ;
  }

  
  COMPILER_64_SET(tbl_data->compare_payload_data, payload_data[1], payload_data[0]);
  COMPILER_64_SET(tbl_data->compare_payload_mask, payload_mask[1], payload_mask[0]);

  

  if (rule_val != NULL) {
      tbl_data->compare_accessed_data = SOC_SAND_BOOL2NUM(rule_val->val.accessed);
      tbl_data->compare_accessed_mask = (rule_val->compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ACCESSED)?0:1;
  }
  else{
      tbl_data->compare_accessed_data = 0;
      tbl_data->compare_accessed_mask = 1;
  }


 
    

 



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lem_traverse_bitmap_build()", 0, 0);
}

uint32
  arad_pp_lem_traverse_bitmap_build(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                         *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                         *rule_key_mask,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE   *rule_val,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA             *tbl_data
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = _arad_pp_lem_traverse_bitmap_build(unit,
                                      rule_key,
                                      rule_key_mask,
                                      rule_val,
                                      ARAD_PP_FRWRD_MACT_RULE_MASK_CONV_DONT_CARE,
                                      tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lem_traverse_bitmap_build()", 0, 0);
}



uint32
  arad_pp_lem_dbal_access_mact_entry_add_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY      *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE    *mac_entry_value,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success
  )
{
    uint32 res = SOC_SAND_OK;
    ARAD_PP_LEM_ACCESS_PAYLOAD         payload;
    SOC_DPP_DBAL_SW_TABLE_IDS          table_id;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LEM_ACCESS_ENTRY_ADD_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(success);

    DBAL_QUAL_VALS_CLEAR(qual_vals);
    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

    switch(mac_entry_key->key_type)
    {
        case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC:
            DBAL_QUAL_VAL_ENCODE_AFTER_FWD_IPV4_DIP(&qual_vals[0], mac_entry_key->key_val.ipv4_mc.dip, 32);
            DBAL_QUAL_VAL_ENCODE_FID(&qual_vals[1], mac_entry_key->key_val.ipv4_mc.fid);
            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_LEM;
            break;
        default:
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }


    
    res = arad_pp_frwrd_mact_payload_build(
              unit,
              mac_entry_value,
              &payload
          );

    res = arad_pp_dbal_entry_add(unit, table_id, qual_vals, 0,  &payload, success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lem_dbal_access_mact_entry_add_unsafe()", 0, 0);
}



uint32
  arad_pp_lem_dbal_access_mact_entry_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY      *mac_entry_key,
    SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD        *payload,
    SOC_SAND_OUT uint8                             *found
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_DPP_DBAL_SW_TABLE_IDS          table_id;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LEM_ACCESS_ENTRY_BY_KEY_GET_UNSAFE);

    DBAL_QUAL_VALS_CLEAR(qual_vals);

    switch(mac_entry_key->key_type)
    {
        case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC:
            DBAL_QUAL_VAL_ENCODE_AFTER_FWD_IPV4_DIP(&qual_vals[0], mac_entry_key->key_val.ipv4_mc.dip, 32);
            DBAL_QUAL_VAL_ENCODE_FID(&qual_vals[1], mac_entry_key->key_val.ipv4_mc.fid);
            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_LEM;
            break;
        default:
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }

    res = arad_pp_dbal_entry_get(unit, table_id, qual_vals, payload, 0, NULL, found);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lem_dbal_access_mact_entry_get_unsafe()", 0, 0);
}


uint32
  arad_pp_lem_dbal_access_mact_entry_remove_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY      *mac_entry_key
  )
{
    uint32 res = SOC_SAND_OK;
    SOC_DPP_DBAL_SW_TABLE_IDS          table_id;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_SUCCESS_FAILURE     success;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LEM_ACCESS_ENTRY_REMOVE_UNSAFE);

    DBAL_QUAL_VALS_CLEAR(qual_vals);

    switch(mac_entry_key->key_type)
    {
        case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC:
            DBAL_QUAL_VAL_ENCODE_AFTER_FWD_IPV4_DIP(&qual_vals[0], mac_entry_key->key_val.ipv4_mc.dip, 32);
            DBAL_QUAL_VAL_ENCODE_FID(&qual_vals[1], mac_entry_key->key_val.ipv4_mc.fid);
            table_id = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_LEM;
            break;
        default:
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }

    res = arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lem_dbal_access_mact_entry_remove_unsafe()", 0, 0);
}





void arad_pp_frwrd_mact_clear_flush_operation(SOC_SAND_IN  int unit)
{
    uint32 res = SOC_SAND_OK;
    uint32 entry_offset;
    ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA tbl_data;
    soc_field_t flu_machine_pause = SOC_IS_JERICHO(unit) ? LARGE_EM_FLU_MACHINE_PAUSEf : MACT_FLU_MACHINE_PAUSEf; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for (entry_offset = 0; entry_offset < ARAD_PP_MACT_NOF_FLUSH_ENTRIES; entry_offset++) {
        ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA_clear(&tbl_data);
        tbl_data.compare_valid = 0;
        res = arad_pp_ihp_flush_db_tbl_set_unsafe(
            unit,
            entry_offset,
            &tbl_data
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
    }


    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  7,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf,  0));

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0, flu_machine_pause,  0));

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_clear_flush_operation()", 0, 0);
}






#include <soc/dpp/SAND/Utils/sand_footer.h>




