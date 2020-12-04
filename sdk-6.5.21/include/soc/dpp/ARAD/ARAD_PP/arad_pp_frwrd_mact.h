
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_FRWRD_MACT_INCLUDED__

#define __ARAD_PP_FRWRD_MACT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/error.h>

#include <soc/dpp/PPC/ppc_api_frwrd_mact_mgmt.h>






#define ARAD_PP_FRWRD_MACT_NOF_MS_IN_SEC                         (1000)
#define ARAD_PP_FRWRD_MACT_NOF_MICROSEC_IN_MS                    (1000)

#define ARAD_PP_MACT_NOF_FLUSH_ENTRIES                   (8)
#define ARAD_PP_L2_TRVRS_ITER_BLK_SIZE                   (130)

#define ARAD_PP_FRWRD_MACT_ILLEGAL_AGE_STATUS            (8)










typedef int (*l2_event_fifo_interrupt_handler_cb_t)(int, uint32*) ;


typedef enum
{
  
  ARAD_PP_FRWRD_MACT_ENTRY_ADD = ARAD_PP_PROC_DESC_BASE_FRWRD_MACT_FIRST,
  ARAD_PP_FRWRD_MACT_ENTRY_ADD_PRINT,
  ARAD_PP_FRWRD_MACT_ENTRY_ADD_UNSAFE,
  ARAD_PP_FRWRD_MACT_ENTRY_ADD_VERIFY,
  ARAD_PP_FRWRD_MACT_ENTRY_REMOVE,
  ARAD_PP_FRWRD_MACT_ENTRY_REMOVE_PRINT,
  ARAD_PP_FRWRD_MACT_ENTRY_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_MACT_ENTRY_REMOVE_VERIFY,
  ARAD_PP_FRWRD_MACT_ENTRY_GET,
  ARAD_PP_FRWRD_MACT_ENTRY_GET_PRINT,
  ARAD_PP_FRWRD_MACT_ENTRY_GET_UNSAFE,
  ARAD_PP_FRWRD_MACT_ENTRY_GET_VERIFY,
  ARAD_PP_FRWRD_MACT_ENTRY_PACK,
  ARAD_PP_FRWRD_MACT_ENTRY_PACK_PRINT,
  ARAD_PP_FRWRD_MACT_ENTRY_PACK_UNSAFE,
  ARAD_PP_FRWRD_MACT_ENTRY_PACK_VERIFY,
  ARAD_PP_FRWRD_MACT_SW_ENTRY_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_MACT_TRAVERSE,
  ARAD_PP_FRWRD_MACT_TRAVERSE_PRINT,
  ARAD_PP_FRWRD_MACT_TRAVERSE_UNSAFE,
  ARAD_PP_FRWRD_MACT_TRAVERSE_VERIFY,
  ARAD_PP_FRWRD_MACT_TRAVERSE_BY_MAC_LIMIT_PER_TUNNEL_UNSAFE,
  ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET,
  ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET_PRINT,
  ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET_UNSAFE,
  ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET_VERIFY,
  ARAD_PP_FRWRD_MACT_GET_BLOCK,
  ARAD_PP_FRWRD_MACT_GET_BLOCK_PRINT,
  ARAD_PP_FRWRD_MACT_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_MACT_GET_BLOCK_VERIFY,
  ARAD_PP_FRWRD_MACT_PRINT_BLOCK,
  ARAD_PP_FRWRD_MACT_PRINT_BLOCK_PRINT,
  ARAD_PP_FRWRD_MACT_PRINT_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_MACT_PRINT_BLOCK_VERIFY,
  ARAD_PP_FRWRD_MACT_GET_PROCS_PTR,
  ARAD_PP_FRWRD_MACT_GET_ERRS_PTR,
  

  ARAD_PP_FRWRD_MACT_KEY_CONVERT,
  ARAD_PP_FRWRD_MACT_PAYLOAD_MASK_BUILD,
  ARAD_PP_FRWRD_MACT_KEY_PARSE,
  ARAD_PP_FRWRD_MACT_PAYLOAD_CONVERT,
  ARAD_PP_FRWRD_MACT_RULE_TO_KEY_CONVERT,
  ARAD_PP_FRWRD_MACT_PAYLOAD_BUILD,
  ARAD_PP_FRWRD_MACT_PAYLOAD_MASK_RETRIEVE,
  ARAD_PP_FRWRD_MACT_SW_TRAVERSE_UNSAFE,
  ARAD_PP_FRWRD_LEM_GET_BLOCK_UNSAFE,
  ARAD_PP_FRWRD_MACT_GET_BLOCK_BY_KEY_UNSAFE,
  ARAD_PP_FRWRD_MACT_SW_ENTRY_ADD_UNSAFE,
  ARAD_PP_FRWRD_MACT_CONVERT_KEY_VALUE_INTO_REQUEST_PAYLOAD,
  
  ARAD_PP_FRWRD_MACT_PROCEDURE_DESC_LAST
} ARAD_PP_FRWRD_MACT_PROCEDURE_DESC;

typedef enum
{
  
  SOC_PPC_FRWRD_MACT_ADD_TYPE_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_FRWRD_MACT_FIRST,
  ARAD_PP_FRWRD_MACT_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_FLAVORS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_DIP_OUT_OF_RANGE_ERR,
  SOC_PPC_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_AGE_STATUS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_SEC_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_MILI_SEC_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_DIP_MASK_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_COMPARE_MASK_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_UPDATE_MASK_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_NOF_MATCHED_ENTRIES_OUT_OF_RANGE_ERR,
  

  ARAD_PP_BLOCK_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR,
  ARAD_PP_BLOCK_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR,
  ARAD_PP_BLOCK_RANGE_ITER_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_ACTION_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_MACT_RETRIEVE_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_MACT_TABLE_TRAVERSE_WAITING_ERR,
  ARAD_PP_MACT_ACCESSED_UPDATE_ILLEGAL_ERR,
  ARAD_PP_FRWRD_MACT_FLUSH_OUT_OF_RESOURCE_ERR,
  ARAD_PP_FRWRD_MACT_UNKOWN_PREFIX_APP_ID_ERR,
  
  ARAD_PP_FRWRD_MACT_ERR_LAST
} ARAD_PP_FRWRD_MACT_ERR;

typedef struct
{
    uint32 flush_db_data[7];
} ARAD_PP_FRWRD_MACT_FLUSH_DB_DATA_ARR;

typedef struct
{
  SOC_PPC_FRWRD_MACT_LEARNING_MODE            learning_mode;
  uint8                                       is_petra_a_compatible;
  ARAD_PP_FRWRD_MACT_FLUSH_DB_DATA_ARR        flush_db_data_arr[8];
  uint32 flush_entry_use;
  uint8 traverse_mode;
  uint32 num_entries_in_dma_host_memory; 
  uint32 size_of_learn_event;
  uint32 host_memory_allocated_bytes; 
} ARAD_PP_FWD_MACT;










void arad_pp_frwrd_mact_handle_learn_event(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 *learn_event);

soc_error_t arad_pp_frwrd_mact_event_handler_callback_register(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  l2_event_fifo_interrupt_handler_cb_t     event_handler_cb);

uint32
  arad_pp_frwrd_mact_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_mact_app_to_prefix_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        app_id,
    SOC_SAND_OUT uint32                        *prefix);




uint32
  arad_pp_frwrd_mact_entry_pack_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint8                                       insert,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint32                                      *data,
    SOC_SAND_OUT uint8                                       *data_len
  );

uint32
  arad_pp_frwrd_mact_entry_pack_verify(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value
  );


uint32
  arad_pp_frwrd_mact_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

uint32
  arad_pp_frwrd_mact_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value
  );


uint32
  arad_pp_frwrd_mact_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  int                                sw_only
  );

uint32
  arad_pp_frwrd_mact_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key
  );

uint32
  arad_pp_frwrd_mact_ipmc_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_frwrd_mact_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  arad_pp_frwrd_mact_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_entry_key
  );


uint32
  arad_pp_frwrd_mact_traverse_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_IN  uint8                                 wait_till_finish,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  );

uint32
  arad_pp_frwrd_mact_rule_to_key_convert(
      SOC_SAND_IN  int                             unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE *rule,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY                  *rule_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY                  *rule_key_mask,
      SOC_SAND_IN  uint32                              mask_val
    );

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
  );

uint32
  arad_pp_frwrd_mact_traverse_by_mac_limit_per_tunnel_internal_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *rule_key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION        *action,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  );

uint32
  arad_pp_frwrd_mact_traverse_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_IN  uint8                                 wait_till_finish
  );


uint32
  arad_pp_frwrd_mact_traverse_status_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO     *status
  );

uint32
  arad_pp_frwrd_mact_traverse_mode_info_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  );

uint32
  arad_pp_frwrd_mact_traverse_mode_info_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  );


uint32
  arad_pp_frwrd_mact_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  );


uint32
  arad_pp_frwrd_mact_get_counter_by_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT uint32                                  *nof_entries
  );



uint32
  arad_pp_frwrd_mact_get_sw_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_frwrd_mact_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );

uint32
  arad_pp_frwrd_mact_key_parse(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *key,
      SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY *mac_entry_key
    );

uint32
  arad_pp_frwrd_mact_payload_convert(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_PAYLOAD     *payload,
      SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE *mac_entry_value
  );


#define ARAD_PP_FRWRD_LEM_GET_BLOCK_ACCESSED_ONLY (0x1)
#define ARAD_PP_FRWRD_LEM_GET_BLOCK_CLEAR_ACCESSED (0x2)
#define ARAD_PP_FRWRD_LEM_GET_BLOCK_KBP_HOST_INDICATION (0x4)


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
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  arad_pp_frwrd_lem_traverse_internal_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                  *key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *value_rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION        *action,
    SOC_SAND_IN  uint8                                 wait_till_finish,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  );

uint32
  arad_pp_frwrd_lem_traverse_by_mac_limit_per_tunnel_internal_unsafe(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                       *rule_key,
    SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY                       *rule_key_mask,
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE  *rule_val,
    SOC_SAND_INOUT    SOC_SAND_TABLE_BLOCK_RANGE              *block_range,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION           *action,
    SOC_SAND_OUT uint32                                       *nof_matched_entries 
  );

uint32
  arad_pp_frwrd_mact_key_convert(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY *mac_entry_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *key
    );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_mact_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_mact_get_errs_ptr(void);

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *info
  );

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC *info
  );

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL *info
  );

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY *info
  );

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO *info
  );

uint32
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE *info
  );

uint32
  SOC_PPC_FRWRD_MACT_TIME_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TIME *info
  );

uint32
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_verify(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC *info
  );

uint32
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_verify(
    SOC_SAND_IN  int                                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC *info
  );

uint32
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE *info
  );

uint32
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_verify(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE *info
  );

uint32
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION *info
  );

uint32
  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO *info
  );

uint32
arad_pp_lem_dbal_access_mact_entry_add_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY      *mac_entry_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE    *mac_entry_value,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success
  );

uint32
  arad_pp_lem_dbal_access_mact_entry_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY      *mac_entry_key,
    SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD        *payload,
    SOC_SAND_OUT uint8                             *found
  );

uint32
  arad_pp_lem_dbal_access_mact_entry_remove_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY      *mac_entry_key
  );



void arad_pp_frwrd_mact_clear_flush_operation(SOC_SAND_IN  int unit);


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



