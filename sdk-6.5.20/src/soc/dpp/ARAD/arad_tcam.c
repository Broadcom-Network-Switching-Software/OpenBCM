/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM
#include <shared/bsl.h>




#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_tcam.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/dpp/ARAD/arad_tcam_access_crash_recovery.h>
#endif

#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/ARAD/arad_tcam_mgmt.h>
#include <soc/dpp/ARAD/arad_sw_db_tcam_mgmt.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_general.h>






#define ARAD_TCAM_TCAM_DB_ID_MAX                                 (ARAD_TCAM_MAX_NOF_LISTS)
#define ARAD_TCAM_ENTRY_ID_MAX                                   (SOC_SAND_U32_MAX)
#define ARAD_TCAM_BITS_MAX                                       (SOC_SAND_U32_MAX)
#define ARAD_TCAM_LENGTH_MAX                                     (SOC_SAND_UINT_MAX)
#define SOC_DPP_DEFS_TCAM_ENTRY_MAX(unit)                        (2 * SOC_DPP_DEFS_GET(unit, nof_tcam_big_bank_lines))

#define ARAD_TCAM_WORD_SIZE_IN_BITS                              (160)
#define ARAD_TCAM_ACTION_WORD_SIZE_IN_BITS                       (SOC_DPP_DEFS_GET(unit, tcam_action_width))
#define ARAD_TCAM_DB_LOCATION_KEY_SIZE                           (2) 

#define ARAD_TCAM_NOF_BANK_OWNERS_IN_BITS     (2)

 
#define ARAD_TCAM_INSERTION_ALGORITHM_MIDDLE_FREE (1)

#if ARAD_TCAM_INSERTION_ALGORITHM_MIDDLE_FREE

#define ARAD_TCAM_INSERTION_ALGORITHM_MIDDLE_FREE_FIND(found_taken, direction, place_taken, start, end) \
    ((((found_taken)? ((direction == ARAD_TCAM_DIRECTION_FORWARD)? (place_taken - 1): (place_taken + 1)): \
    ((direction == ARAD_TCAM_DIRECTION_FORWARD)? end: start)) + \
    ((direction == ARAD_TCAM_DIRECTION_FORWARD)? start: end) + ((direction == ARAD_TCAM_DIRECTION_FORWARD)?1:0)) / 2)


#define ARAD_TCAM_NOF_OCCUPATIONS_BITMAPS (2)

#else


#define ARAD_TCAM_NOF_OCCUPATIONS_BITMAPS (1)

#endif 

#define ARAD_TCAM_BANK_INIT_MOVE_ENTRIES_NOF_ADJACENT_BANKS     (2)






#ifdef BROADCOM_DEBUG
#define ARAD_TCAM_DB_EXISTS_ASSERT                           \
  {                                                        \
    res = arad_tcam_db_exists_assert(unit, tcam_db_id); \
    SOC_SAND_CHECK_FUNC_RESULT(res, 9, exit);                 \
  }

#define ARAD_TCAM_DB_ENTRY_EXISTS_ASSERT(__entry_id)                                        \
  {                                                                             \
    res = arad_tcam_db_entry_exists_assert(unit, tcam_db_id, __entry_id);    \
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);                                  \
  }
#else  
#define ARAD_TCAM_DB_EXISTS_ASSERT                           
#define ARAD_TCAM_DB_ENTRY_EXISTS_ASSERT(__entry_id)                                        
#endif 

#define ARAD_TCAM_ENTRY_LINE_PAIR_80B(entry_line) ((((entry_line) / 2) * 2) + (1 - ((entry_line) % 2)))

#define ARAD_TCAM_ENTRY_LINE_PAIR_CLOSEST_ID(entry_line, direction) \
  (((entry_line % 2) == ((direction == ARAD_TCAM_DIRECTION_BACKWARD)? 0: 1))? entry_line: ARAD_TCAM_ENTRY_LINE_PAIR_80B(entry_line))


#define ARAD_TCAM_ENTRY_LINE_GET(place, entry_size) \
  ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS)? ((place) / 2): (place))

#define ARAD_TCAM_ENTRY_NOF_LINES_GET(place1, place2, entry_size) \
  soc_sand_abs(ARAD_TCAM_ENTRY_LINE_GET(place1, entry_size) - ARAD_TCAM_ENTRY_LINE_GET(place2, entry_size))


#define ARAD_TCAM_LOCATION_ENCODING_BANK_LSB_IN_SECOND_UINT8 (soc_sand_log2_round_up(2  * SOC_DPP_DEFS_GET(unit, nof_tcam_big_bank_lines)) - 8)

#define ARAD_TCAM_LOCATION_ENCODING_ENTRY_MASK (~((uint32) ((1 << (ARAD_TCAM_LOCATION_ENCODING_BANK_LSB_IN_SECOND_UINT8 + 8)) - 1)))




typedef enum
{
  ARAD_TCAM_DIRECTION_FORWARD,
  ARAD_TCAM_DIRECTION_BACKWARD
} ARAD_TCAM_DIRECTION;










STATIC
  uint32
    arad_tcam_db_location_encode(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_TCAM_LOCATION *location,
      SOC_SAND_OUT uint8               *key
    )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#ifdef BROADCOM_DEBUG
  {
      uint32 res;
      res = ARAD_TCAM_LOCATION_verify(unit, location);
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

      if ((location->entry & ARAD_TCAM_LOCATION_ENCODING_ENTRY_MASK) != 0)
      {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Entry %d - Failed to encode location. Invalid entry.\n\r"), location->entry));
        SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_ENTRY_NDX_OUT_OF_RANGE_ERR, 10, exit);
      }
  }
#endif 

  
  key[0] = (uint8) location->entry & 0xff;
  if(ARAD_TCAM_LOCATION_ENCODING_BANK_LSB_IN_SECOND_UINT8 >= 8) {
      key[1] = 0;
  } else {
      key[1] = (location->bank_id << ARAD_TCAM_LOCATION_ENCODING_BANK_LSB_IN_SECOND_UINT8) & 0xff;
  }
  key[1] |= (location->entry >> 8) & 0xff;

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_location_encode", 0, 0);
}

STATIC
  void
    arad_tcam_db_location_decode(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  uint8          *key,
      SOC_SAND_OUT ARAD_TCAM_LOCATION *location
    )
{
    
    if(ARAD_TCAM_LOCATION_ENCODING_BANK_LSB_IN_SECOND_UINT8 > 8 ) {
        location->entry = 0;
	location->bank_id = 0;
    } else {
        
        location->entry = (((uint32) key[1] & ((1 << (ARAD_TCAM_LOCATION_ENCODING_BANK_LSB_IN_SECOND_UINT8))-1)) << 8);
	location->bank_id = key[1] >> ARAD_TCAM_LOCATION_ENCODING_BANK_LSB_IN_SECOND_UINT8;
    }
    location->entry |= key[0];
}


uint32
    arad_tcam_global_location_encode(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_TCAM_LOCATION *location,
      SOC_SAND_OUT uint32              *global_location_id
    )
{
  uint8         
      key[ARAD_TCAM_DB_LOCATION_KEY_SIZE] = {0};
  uint32
      res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res  = arad_tcam_db_location_encode(unit, location, key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *global_location_id = key[0] + (key[1] << 8);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_global_location_encode", 0, 0);
}

STATIC
  void
    arad_tcam_db_prio_list_data_encode(
      SOC_SAND_IN  ARAD_TCAM_PRIO_LOCATION *prio_location,
      SOC_SAND_OUT uint8  *data
    )
{
    sal_memcpy(data, prio_location, sizeof(ARAD_TCAM_PRIO_LOCATION));
}

STATIC
  uint32
    arad_tcam_db_priority_data_decode(
        SOC_SAND_IN  int        unit,
        SOC_SAND_IN  uint32     tcam_db_id,
        SOC_SAND_IN  uint8      *data,
        SOC_SAND_OUT ARAD_TCAM_PRIO_LOCATION *prio_location
    )
{
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memcpy(prio_location, data, sizeof(ARAD_TCAM_PRIO_LOCATION));

    
    res  = ARAD_TCAM_PRIO_LOCATION_verify(unit, tcam_db_id, prio_location);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_priority_data_decode", 0, 0);
}


uint32
    arad_tcam_db_prio_list_priority_value_decode(
        SOC_SAND_IN  uint8      *data
    )
{
    uint32
      value;

    soc_sand_U8_to_U32(
      data,
      sizeof(uint32),
      &value
    );

  return value;
}

STATIC
  void
    arad_tcam_db_prio_list_priority_value_encode(
      SOC_SAND_IN  uint32     priority,
      SOC_SAND_OUT uint8     *data
    )
{
    soc_sand_U32_to_U8(
      &priority,
      sizeof(uint32),
      data
    );
}

STATIC
  void
    arad_tcam_db_entry_id_encode(
      SOC_SAND_IN uint32        tcam_db_id,
      SOC_SAND_IN  uint32 entry_id,
      SOC_SAND_OUT uint8   *data
    )
{
  soc_sand_U32_to_U8(
    &entry_id,
    sizeof(uint32),
    data
  );
  
  data[4] = (uint8) (tcam_db_id & 0xFF);
  data[5] = (uint8) ((tcam_db_id >> 8) & 0xFF);
}


  int32
    arad_tcam_db_priority_list_cmp_priority(
      SOC_SAND_IN uint8  *buffer1,
      SOC_SAND_IN uint8  *buffer2,
              uint32 size
    )
{
  uint32
    prio_1,
    prio_2;
  ARAD_TCAM_PRIO_LOCATION 
      prio_location_1,
      prio_location_2;

  if ((buffer1 == NULL) || (buffer2 == NULL)) {
      
      assert(0);
      return 0; 
  }
  
  if (size == ARAD_TCAM_DB_LIST_KEY_SIZE) {
      prio_1 = arad_tcam_db_prio_list_priority_value_decode(buffer1);
      prio_2 = arad_tcam_db_prio_list_priority_value_decode(buffer2);
      return (((int32) prio_1) - ((int32) prio_2));
  }
  else if (size == ARAD_TCAM_DB_LIST_DATA_SIZE) { 
      ARAD_TCAM_PRIO_LOCATION_clear(&prio_location_1);
      ARAD_TCAM_PRIO_LOCATION_clear(&prio_location_2);
      sal_memcpy(&prio_location_1, buffer1, sizeof(ARAD_TCAM_PRIO_LOCATION));
      sal_memcpy(&prio_location_2, buffer2, sizeof(ARAD_TCAM_PRIO_LOCATION));
      return ((prio_location_1.entry_id_first != prio_location_2.entry_id_first)
              || (prio_location_1.entry_id_last != prio_location_2.entry_id_last))? 1 :0;
  } else {
      
      assert(0);
      return 0;
  }
}



  uint32
    arad_tcam_db_data_structure_entry_set(
      SOC_SAND_IN    int prime_handle,
      SOC_SAND_IN    uint32 sec_handle,
      SOC_SAND_INOUT uint8  *buffer,
      SOC_SAND_IN    uint32 offset,
      SOC_SAND_IN    uint32 len,
      SOC_SAND_IN    uint8  *data
    )
{
  sal_memcpy(
    buffer + (offset * len),
    data,
    len
  );
  return SOC_SAND_OK;
}


  uint32
    arad_tcam_db_data_structure_entry_get(
      SOC_SAND_IN  int prime_handle,
      SOC_SAND_IN  uint32 sec_handle,
      SOC_SAND_IN  uint8  *buffer,
      SOC_SAND_IN  uint32 offset,
      SOC_SAND_IN  uint32 len,
      SOC_SAND_OUT uint8  *data
    )
{
  sal_memcpy(
    data,
    buffer + (offset * len),
    len
  );
  return SOC_SAND_OK;
}


STATIC
  uint32
    arad_tcam_db_block_entry_insertion_rule_respected_get(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  uint32                        tcam_db_id,
      SOC_SAND_OUT uint8                         *insertion_rule_respected
    )
{
    ARAD_TCAM_BANK_OWNER
      db_bank_owner;
    uint8
      no_insertion_priority_order;
    uint32
      access_profile_id,
      res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, tcam_db_id, 0, &access_profile_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.bank_owner.get(
            unit,
            access_profile_id,
            &db_bank_owner
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.no_insertion_priority_order.get(unit, tcam_db_id, &no_insertion_priority_order);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
    
    
    *insertion_rule_respected =  (((db_bank_owner == ARAD_TCAM_BANK_OWNER_PMF_0) 
                                   || (db_bank_owner == ARAD_TCAM_BANK_OWNER_PMF_1) 
                                   || (db_bank_owner == ARAD_TCAM_BANK_OWNER_EGRESS_ACL)) 
                                  && (!no_insertion_priority_order));
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_block_entry_insertion_rule_respected_get()", 0, 0);
}


uint32
    arad_tcam_db_use_new_method_per_db_get(
      SOC_SAND_IN  int                        unit,
      SOC_SAND_IN  uint32                     tcam_db_id,
      SOC_SAND_OUT uint8                      *move_per_block
    )
{
    uint8
        sparse_priorities;
    ARAD_TCAM_BANK_OWNER
        db_bank_owner;
    ARAD_TCAM_BANK_ENTRY_SIZE
      entry_size;
    uint32
      access_profile_id,
      res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit,tcam_db_id, &entry_size);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, tcam_db_id, 0, &access_profile_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.bank_owner.get(
            unit,
            access_profile_id,
            &db_bank_owner
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.sparse_priorities.get(unit, tcam_db_id, &sparse_priorities);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    *move_per_block = ((db_bank_owner == ARAD_TCAM_BANK_OWNER_FLP_TCAM)
                       || (db_bank_owner == ARAD_TCAM_BANK_OWNER_FLP_TRAPS)
                       || (db_bank_owner == ARAD_TCAM_BANK_OWNER_VT)
                       || (db_bank_owner == ARAD_TCAM_BANK_OWNER_TT)
                       || (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS)
                       || sparse_priorities
                       )? FALSE : TRUE;

#ifdef PLISIM   
      if (SAL_BOOT_PLISIM) { 
          
          *move_per_block = FALSE;
      }
#endif 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_use_new_method_per_db_get()", 0, 0);
}


uint32
    arad_tcam_bank_entry_size_to_entry_count_get(
        SOC_SAND_IN  int                       unit,
        SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
        SOC_SAND_IN  uint32                    bank_ndx
     )
{
  uint32
    ret = 0;

  switch (entry_size)
  {
  case ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS:
    ret = 2 * ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx);
    break;

  case ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS:
  case ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS:
  ret = ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx);
    break;

  default:
    break;
  }

  return ret;
}

STATIC
  uint32
    arad_tcam_db_entries_per_bank_get(
      SOC_SAND_IN  int    unit,
      SOC_SAND_IN  uint32 tcam_db_id,
      SOC_SAND_IN  uint32 bank_id,
      SOC_SAND_OUT uint32 *db_entries_per_bank
    )
{
    ARAD_TCAM_BANK_ENTRY_SIZE entry_size;
    uint32
        res;
    uint8
        is_direct;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.is_direct.get(unit, tcam_db_id, &is_direct);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    if(is_direct){
        entry_size = ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS;
    } else {
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit, tcam_db_id, &entry_size);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    }
    *db_entries_per_bank = arad_tcam_bank_entry_size_to_entry_count_get(unit, entry_size, bank_id);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entries_per_bank_get()", 0, 0);
}


STATIC
  uint32
    arad_tcam_entry_word_extract(
      SOC_SAND_IN  ARAD_TCAM_ENTRY                *entry,
      SOC_SAND_IN  uint8                    take_lsb,
      SOC_SAND_OUT ARAD_PP_IHB_TCAM_BANK_TBL_DATA *tbl_data
    )
{
  uint32
    first_bit,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  first_bit = take_lsb ? 0 : ARAD_TCAM_WORD_SIZE_IN_BITS;
  res = soc_sand_bitstream_get_any_field(
          entry->value,
          first_bit,
          ARAD_TCAM_WORD_SIZE_IN_BITS,
          tbl_data->value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_bitstream_get_any_field(
          entry->mask,
          first_bit,
          ARAD_TCAM_WORD_SIZE_IN_BITS,
          tbl_data->mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_entry_word_extract()", take_lsb, 0);
}


STATIC
  uint32
    arad_tcam_entry_word_recover(
      SOC_SAND_IN  ARAD_PP_IHB_TCAM_BANK_TBL_DATA *tbl_data,
      SOC_SAND_IN  uint8                    take_lsb,
      SOC_SAND_OUT ARAD_TCAM_ENTRY                *entry
    )
{
  uint32
    first_bit,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  first_bit = take_lsb ? 0 : ARAD_TCAM_WORD_SIZE_IN_BITS;
  entry->valid = tbl_data->valid;
  res = soc_sand_bitstream_set_any_field(
          tbl_data->value,
          first_bit,
          ARAD_TCAM_WORD_SIZE_IN_BITS,
          entry->value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_bitstream_set_any_field(
          tbl_data->mask,
          first_bit,
          ARAD_TCAM_WORD_SIZE_IN_BITS,
          entry->mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_entry_word_recover()", take_lsb, 0);
}

STATIC
  uint32
    arad_tcam_action_word_recover(
       SOC_SAND_IN  int unit,
      SOC_SAND_IN  ARAD_PP_IHB_TCAM_ACTION_TBL_DATA *tbl_data,
      SOC_SAND_IN  uint8                    word,
      SOC_SAND_OUT ARAD_TCAM_ACTION         *action
    )
{
  uint32
    start_bit,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  start_bit = word*ARAD_TCAM_ACTION_WORD_SIZE_IN_BITS;
  res = soc_sand_bitstream_set_any_field(
          &tbl_data->action,
          start_bit,
          ARAD_TCAM_ACTION_WORD_SIZE_IN_BITS,
          action->value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_entry_word_recover()", word, 0);
}


STATIC
  uint32
    arad_tcam_action_word_extract(
       SOC_SAND_IN  int unit,
      SOC_SAND_IN  ARAD_TCAM_ACTION         *action,
      SOC_SAND_IN  uint8                    word,      
      SOC_SAND_OUT ARAD_PP_IHB_TCAM_ACTION_TBL_DATA *tbl_data
    )
{
  uint32
    start_bit,
    res = SOC_SAND_OK;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);  

  start_bit = word*ARAD_TCAM_ACTION_WORD_SIZE_IN_BITS;

  res = soc_sand_bitstream_get_any_field(
            action->value,
            start_bit,
            ARAD_TCAM_ACTION_WORD_SIZE_IN_BITS,
            &(tbl_data->action)
          );     
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_entry_word_extract()", word, 0);
}

STATIC
  uint32
    arad_tcam_tbl_bank_write(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  uint32               bank_id,
      SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
      SOC_SAND_IN  uint32                base_address,
      SOC_SAND_IN  ARAD_TCAM_ENTRY           *entry
    )
{
  uint32
    nof_words,
      nof_entries_in_line,
    word,
    res = SOC_SAND_OK;
  ARAD_PP_IHB_TCAM_BANK_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  tbl_data.valid = entry->valid;
  nof_words = (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) ? 2 : 1;
  nof_entries_in_line = (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) ? 3 : 
                          ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS) ? 2 : 1);
  for (word = 0; word < nof_words; ++ word)
  {
    res = arad_tcam_entry_word_extract(
            entry,
            (word == 0) ? TRUE : FALSE,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    res = arad_pp_ihb_tcam_tbl_write_unsafe(
            unit,
            bank_id + word,
            nof_entries_in_line,
            base_address,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_tbl_bank_write()", bank_id, base_address);
}

STATIC
  uint32
    arad_tcam_tbl_bank_read(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  uint32               bank_id,
      SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
      SOC_SAND_IN  uint32                base_address,
      SOC_SAND_OUT ARAD_TCAM_ENTRY           *entry
    )
{
  uint32
    nof_words,
      nof_entries_in_line,
    word,
    res = SOC_SAND_OK;

  ARAD_PP_IHB_TCAM_BANK_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  nof_words = (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) ? 2 : 1;
  nof_entries_in_line = (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) ? 3 : 
                          ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS) ? 2 : 1);
  for (word = 0; word < nof_words; ++ word)
  {
    res = arad_pp_ihb_tcam_tbl_read_unsafe(
            unit,
            bank_id + word,
            nof_entries_in_line,
            base_address,
            &tbl_data
          );

    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);  

    res = arad_tcam_entry_word_recover(
            &tbl_data,
            (word == 0) ? TRUE : FALSE,
            entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_tbl_bank_read()", bank_id, base_address);
}

STATIC
  uint32
    arad_tcam_tbl_bank_compare(
      SOC_SAND_IN  int     unit,
      SOC_SAND_IN  uint32     bank_id,
      SOC_SAND_IN  ARAD_TCAM_ENTRY *key,
      SOC_SAND_OUT uint32     *base_address,
      SOC_SAND_OUT uint8     *found
    )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      nof_entries_in_line,
    ndx;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;
  ARAD_PP_IHB_TCAM_BANK_TBL_DATA
    compare_data;
  ARAD_PP_IHB_TCAM_COMPARE_DATA
    found_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (ndx = 0; ndx < ARAD_PP_IHB_TCAM_DATA_WIDTH; ++ndx)
  {
    compare_data.value[ndx] = key->value[ndx];
    compare_data.mask[ndx]  = key->mask[ndx];
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.entry_size.get(
          unit,
          bank_id,
          &entry_size
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  nof_entries_in_line = (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) ? 3 : 
                          ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS) ? 2 : 1);
  res = arad_pp_ihb_tcam_tbl_compare_unsafe(
          unit,
          bank_id,
          nof_entries_in_line,
          &compare_data,
          &found_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *found = SOC_SAND_NUM2BOOL(found_data.found);
  if (*found)
  {
    *base_address = found_data.address;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_tbl_bank_compare()", bank_id, 0);
}

STATIC
  uint32
    arad_tcam_tbl_calc_action_banks_and_lines(
        SOC_SAND_IN  int            unit,
        SOC_SAND_IN  uint32         bank_id,
        SOC_SAND_IN  uint32         base_address,
        SOC_SAND_INOUT  ARAD_TCAM_ACTION_SIZE *action_bitmap_ndx,
        SOC_SAND_OUT uint32         *line,
        SOC_SAND_OUT uint32         *action_tbl

        )
{
    ARAD_TCAM_BANK_ENTRY_SIZE
        entry_size;
    uint32
        nof_action_writes=0,
        res = SOC_SAND_OK;
    uint32
      action_ndx,
      action_bit;
    ARAD_TCAM_ACTION_SIZE
        act_action_bitmap_ndx = *action_bitmap_ndx;


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.entry_size.get(
            unit,
            bank_id,
            &entry_size
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

    
    if(entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS) {
        nof_action_writes = 1;
        action_tbl[0] = bank_id * 2 + (base_address)%2; 
        line[0] = base_address/2; 
        *action_bitmap_ndx = act_action_bitmap_ndx = 1;
    }
    else{ 
        for (action_ndx = 0; action_ndx < ARAD_TCAM_NOF_ACTION_SIZES; ++action_ndx)
        {
            action_bit = (1 << action_ndx);
            if(action_bit & act_action_bitmap_ndx) {
                action_tbl[nof_action_writes] = bank_id * 2 + action_ndx;
                line[nof_action_writes] = base_address;
            }
            ++nof_action_writes;
        }
    }
    exit:
      SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_tbl_calc_action_banks_and_lines()", bank_id, base_address);
}

STATIC
  uint32
    arad_tcam_tbl_action_write(
      SOC_SAND_IN  int unit,
      SOC_SAND_IN  uint32 bank_id,
      SOC_SAND_IN  uint32 base_address,
      SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE action_bitmap_ndx,      
      SOC_SAND_IN uint8         hit_bit,
      SOC_SAND_IN  ARAD_TCAM_ACTION *action
    )
{
  uint32
    action_ndx,
    action_bit,
    word,
    line[ARAD_TCAM_NOF_ACTION_SIZES],
    action_tbl[ARAD_TCAM_NOF_ACTION_SIZES],
    res = SOC_SAND_OK;
  ARAD_TCAM_ACTION_SIZE 
      act_action_bitmap_ndx = action_bitmap_ndx;
  ARAD_PP_IHB_TCAM_ACTION_TBL_DATA
    tbl_data_hit_bit,
    tbl_data;



  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&tbl_data, ARAD_PP_IHB_TCAM_ACTION_TBL_DATA, 1);
  ARAD_CLEAR(&tbl_data_hit_bit, ARAD_PP_IHB_TCAM_ACTION_TBL_DATA, 1);
  tbl_data_hit_bit.action = hit_bit;

  res = arad_tcam_tbl_calc_action_banks_and_lines(unit, bank_id, base_address, &act_action_bitmap_ndx, line, action_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  
  
  
  for (action_ndx = 0, word = 0; action_ndx < ARAD_TCAM_NOF_ACTION_SIZES; ++action_ndx)
  {
    action_bit = (1 << action_ndx);

    if (act_action_bitmap_ndx & action_bit)
    {
       
       res = arad_tcam_action_word_extract(
              unit,
              action,
              word,
              &tbl_data
              );
       SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      
       res = arad_pp_ihb_tcam_action_tbl_set_unsafe(
            unit,
            action_tbl[action_ndx],
            line[action_ndx],
            FALSE ,
            &tbl_data
          );
       SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

       
       res = arad_pp_ihb_tcam_action_tbl_set_unsafe(
                unit,
                action_tbl[action_ndx],
                line[action_ndx],
                TRUE ,
                &tbl_data_hit_bit
              );
       SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

       word++;
    }            
  } 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_tbl_action_write()", bank_id, base_address);
}

STATIC
  uint32
    arad_tcam_tbl_action_read(
      SOC_SAND_IN  int unit,
      SOC_SAND_IN  uint32 bank_id,
      SOC_SAND_IN  uint32 base_address,
      SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE action_bitmap_ndx,
      SOC_SAND_OUT uint8             *hit_bit,
      SOC_SAND_OUT ARAD_TCAM_ACTION  *action
    )
{
  uint32
    action_ndx,
    action_bit,
    word,
    line[ARAD_TCAM_NOF_ACTION_SIZES],
    action_tbl[ARAD_TCAM_NOF_ACTION_SIZES],
    res = SOC_SAND_OK;
  ARAD_TCAM_ACTION_SIZE 
      act_action_bitmap_ndx = action_bitmap_ndx;
  ARAD_PP_IHB_TCAM_ACTION_TBL_DATA
      tbl_data_hit_bit,
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *hit_bit = 0;

  ARAD_TCAM_ACTION_clear(action);


  res = arad_tcam_tbl_calc_action_banks_and_lines(unit, bank_id, base_address, &act_action_bitmap_ndx, line, action_tbl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);

  
  for (action_ndx = 0, word = 0; action_ndx < ARAD_TCAM_NOF_ACTION_SIZES; ++action_ndx)
  {
    action_bit = (1 << action_ndx);

    if (act_action_bitmap_ndx & action_bit)
    {
      res = arad_pp_ihb_tcam_action_tbl_get_unsafe(
              unit,
              action_tbl[action_ndx],            
              line[action_ndx],
              FALSE ,
              &tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = arad_pp_ihb_tcam_action_tbl_get_unsafe(
              unit,
              action_tbl[action_ndx],            
              line[action_ndx],
              TRUE ,
              &tbl_data_hit_bit
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      
      *hit_bit |= tbl_data_hit_bit.action;

      res = arad_tcam_action_word_recover(
            unit,
            &tbl_data,
            word,
            action
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      word++;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_tbl_action_read()", bank_id, base_address);
}

#ifdef BROADCOM_DEBUG
STATIC
  uint32
    arad_tcam_db_exists_assert(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32 tcam_db_id
    )
{
  uint8
    valid;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.get(
          unit,
          tcam_db_id,
          &valid
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  
  if (!valid)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d Tcam DB id %d - The data base doesn\'t exist.\n\r"), unit, tcam_db_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_exists_assert", 0, 0);
}
#endif 

#ifdef BROADCOM_DEBUG
STATIC
  uint32
    arad_tcam_db_entry_exists_assert(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32 tcam_db_id,
      SOC_SAND_IN uint32 entry_id
    )
{
    SOC_SAND_HASH_TABLE_PTR
      hash_tbl;
    uint8
        found,
      hash_key[ARAD_TCAM_DB_HASH_TBL_KEY_SIZE];
    uint32
        res,
        data_indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.entry_id_to_location.get(unit, &hash_tbl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  arad_tcam_db_entry_id_encode(tcam_db_id, entry_id, hash_key);
  res = soc_sand_hash_table_entry_lookup(
          unit,
          hash_tbl,
          hash_key,
          &data_indx,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (!found)
  {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Entry %d DB %d - Failed to find it in the hash table. Invalid entry.\n\r"), entry_id, tcam_db_id));
       SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_ENTRY_NDX_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_exists_assert", 0, 0);
}
#endif 


STATIC
  uint32
    arad_tcam_db_has_banks(
      SOC_SAND_IN  int    unit,
      SOC_SAND_IN  uint32 tcam_db_id,
      SOC_SAND_OUT uint8  *has_bank
    )
{
  uint32
    bank_id,
    res;
  uint8
    is_used;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *has_bank = FALSE;

  for(bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
  {
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(unit, tcam_db_id, bank_id, &is_used);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    if (is_used)
    {
      *has_bank = TRUE;
      ARAD_DO_NOTHING_AND_EXIT;
    }
  }

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_has_banks()", 0, 0);
}


STATIC
  uint32
    arad_tcam_db_next_closest_bank_get(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  uint32             tcam_db_id,
      SOC_SAND_IN  ARAD_TCAM_DIRECTION direction,
      SOC_SAND_IN  uint32               bank_id_curr,
      SOC_SAND_IN  uint8                has_db_entries, 
      SOC_SAND_OUT uint32               *bank_id_next,
      SOC_SAND_OUT uint8                *found
    )
{
  uint32
      bank_iter,
      nof_banks_to_consider,
      bank_min,
      bank_max,
      nof_banks_jump,
      nof_entries,
      res;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;
  ARAD_TCAM_SMALL_BANKS
    use_small_banks;
  int32
    bank_curr; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit, tcam_db_id, &entry_size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.use_small_banks.get(unit, tcam_db_id, &use_small_banks);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  nof_banks_jump = ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) && !use_small_banks)? 2: 1;
  nof_banks_to_consider = SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit) / nof_banks_jump;
  bank_min = 0;
  bank_max = SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit) - 1;

  *bank_id_next = 0;
  *found = FALSE;
  for (bank_iter = 1; (bank_iter < nof_banks_to_consider) && (*found == FALSE); bank_iter++)
  {
      
      bank_curr = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? (bank_id_curr - (nof_banks_jump * bank_iter)): (bank_id_curr + (nof_banks_jump * bank_iter));
      if ((bank_curr < bank_min) || (bank_curr > bank_max))
      {
          
          *found = FALSE;
          break;
      }
      else if (has_db_entries) {
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.get(unit, tcam_db_id, bank_curr, &nof_entries);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          if(nof_entries > 0) {
              
              *found = TRUE;
              *bank_id_next = bank_curr;
              break;
          }
      }
      else if (!has_db_entries) {
          
          *found = TRUE;
          *bank_id_next = bank_curr;
          break;
      }
      
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_next_closest_bank_get()", 0, 0);
}



STATIC
  uint32
    arad_tcam_db_next_closest_location_get(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  uint32             tcam_db_id,
      SOC_SAND_IN  ARAD_TCAM_DIRECTION direction,
      SOC_SAND_IN  ARAD_TCAM_LOCATION      *location,
      SOC_SAND_IN  uint8                has_db_entries, 
      SOC_SAND_OUT ARAD_TCAM_LOCATION        *next_location,
      SOC_SAND_OUT uint8                *found
    )
{
  uint32
    nof_entries_max_in_bank,
    nof_entries,
    res = SOC_SAND_OK;
  uint8
      go_to_next_bank;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit, tcam_db_id, &entry_size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);
  nof_entries_max_in_bank = arad_tcam_bank_entry_size_to_entry_count_get(unit, entry_size, location->bank_id);
  go_to_next_bank = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? (location->entry == 0): (location->entry == (nof_entries_max_in_bank - 1));
  if (has_db_entries)
  {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.get(unit, tcam_db_id, location->bank_id, &nof_entries);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
      if(nof_entries == 0) {
          
          go_to_next_bank = TRUE;
      }
  }

  
  if (!go_to_next_bank)
  {
      
      next_location->bank_id = location->bank_id;
      next_location->entry = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? (location->entry - 1): (location->entry + 1);
      *found = TRUE;
  }
  else
  {
      
      res = arad_tcam_db_next_closest_bank_get(
                unit,
                tcam_db_id,
                direction,
                location->bank_id,
                TRUE , 
                &(next_location->bank_id),
                found
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      if (*found == TRUE)
      {
          
          nof_entries_max_in_bank = arad_tcam_bank_entry_size_to_entry_count_get(unit, entry_size, next_location->bank_id);
          next_location->entry = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? (nof_entries_max_in_bank - 1): 0;
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_next_closest_location_get()", 0, 0);
}



STATIC
  uint32
    arad_tcam_db_occ_bmp_entry_closest_get(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  uint32             tcam_db_id,
      SOC_SAND_IN  ARAD_TCAM_DIRECTION direction,
      SOC_SAND_IN  ARAD_TCAM_LOCATION      *location,
      SOC_SAND_OUT uint32                    *entry_id,
      SOC_SAND_OUT uint32                    *next_priority,
      SOC_SAND_OUT ARAD_TCAM_LOCATION        *next_location,
      SOC_SAND_OUT uint8                *found
    )
{
  uint32
      global_location_id,
      place_first_free,
      nof_entries_max_in_bank,
      curr_line,
      bank_ndx,
      bank_id,
      nof_banks,
      nof_entries,
    res = SOC_SAND_OK;
  SOC_SAND_OCC_BM_PTR
    bank_bm;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;
  ARAD_TCAM_GLOBAL_LOCATION 
      global_location;
  uint8
      is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit, tcam_db_id, &entry_size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  
  *found = FALSE;
  is_found = FALSE;

  
  ARAD_TCAM_LOCATION_clear(next_location);
  res = arad_tcam_db_next_closest_location_get(
            unit,
            tcam_db_id,
            direction,
            location,
            1, 
            next_location,
            &is_found
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (is_found == TRUE)
  {
      
      nof_banks = (direction == ARAD_TCAM_DIRECTION_BACKWARD)?
          (next_location->bank_id + 1) :(SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit) - next_location->bank_id);
      is_found = FALSE;
      for (bank_ndx = 0; (bank_ndx < nof_banks) && (is_found == FALSE); bank_ndx++) {
          bank_id = (direction == ARAD_TCAM_DIRECTION_FORWARD)? (next_location->bank_id + bank_ndx): (next_location->bank_id - bank_ndx);
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.get(unit, tcam_db_id, bank_id, &nof_entries);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
          if (nof_entries == 0) {
              
              continue;
          }
          nof_entries_max_in_bank = arad_tcam_bank_entry_size_to_entry_count_get(unit, entry_size, bank_id);
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_entries_used.get(unit, tcam_db_id, bank_id, &bank_bm);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
          curr_line = (bank_id == next_location->bank_id)? next_location->entry :
              ((direction == ARAD_TCAM_DIRECTION_FORWARD)? 0: (nof_entries_max_in_bank - 1));
          res = soc_sand_occ_bm_get_next_in_range(
                  unit,
                  bank_bm,
                  ((direction == ARAD_TCAM_DIRECTION_BACKWARD)? 0: curr_line), 
                  ((direction == ARAD_TCAM_DIRECTION_BACKWARD)? curr_line: (nof_entries_max_in_bank - 1)), 
                  (direction == ARAD_TCAM_DIRECTION_FORWARD)? TRUE : FALSE,
                  &place_first_free,
                  &is_found
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
          if (is_found)
          {
              next_location->bank_id = bank_id;
              next_location->entry = place_first_free;

              
              res = arad_tcam_global_location_encode(unit, next_location, &global_location_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
              res = sw_state_access[unit].dpp.soc.arad.tm.tcam.global_location_tbl.get(unit, global_location_id, &global_location);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
              *entry_id = global_location.entry_id;
              *next_priority = global_location.priority;

              
    #ifdef BROADCOM_DEBUG
              if (global_location.tcam_db_id != tcam_db_id)
              {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                          (BSL_META_U(unit,
                                      "Unit %d Tcam DB id %d Entry id %d is stored in location (bank %d, entry %d), "
                                      "but we were looking for next entry of TCAM DB %d from location (bank %d, entry %d) with direction %d.\n\r"),
                           unit, global_location.tcam_db_id, *entry_id, next_location->bank_id, next_location->entry,
                           tcam_db_id, location->bank_id, location->entry, direction));
                SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 50, exit);
              }
    #endif 
          }
      }
  }

  *found = is_found;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_occ_bmp_entry_closest_get()", 0, 0);
}


STATIC
  uint32
    arad_tcam_db_priority_list_insertion_range_find(
      SOC_SAND_IN  int     unit,
      SOC_SAND_IN  uint32     tcam_db_id,
      SOC_SAND_IN  uint32      priority,
      SOC_SAND_IN  uint8      is_for_update,
      SOC_SAND_IN  uint32     entry_id,
      SOC_SAND_OUT ARAD_TCAM_RANGE *range
    )
{
  uint32
      entry_id_prev,
      priority_prev,
      entry_id_next,
      priority_next,
    res = SOC_SAND_OK;
  SOC_SAND_SORTED_LIST_PTR
    list;
  SOC_SAND_SORTED_LIST_ITER
    current,
    found_node;
  uint8
      found_prev,
      found_next,
    priority_found,
    found,
    key_buffer_current[ARAD_TCAM_DB_LIST_KEY_SIZE],
    data_buffer[ARAD_TCAM_DB_LIST_DATA_SIZE],
    insertion_order_respected;
  uint32
    node_priority = 0;
  ARAD_TCAM_LOCATION
      location_prev,
      location_next,
      location;
  ARAD_TCAM_PRIO_LOCATION
      prio_location;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(range);
  ARAD_TCAM_RANGE_clear(range);

  ARAD_TCAM_DB_EXISTS_ASSERT;


  
  res = arad_tcam_db_block_entry_insertion_rule_respected_get(unit, tcam_db_id, &insertion_order_respected);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (is_for_update && insertion_order_respected) {
      
      res = arad_tcam_db_entry_id_to_location_entry_get(
              unit,
              tcam_db_id,
              entry_id,
              TRUE,
              &location,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
      if (!found)
      {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Unit %d Tcam db id %d Entry id %d - The entry is for update and it hasn't been found.\n\r"),
                   unit, tcam_db_id, entry_id));
        SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 18, exit);
      }

      
      res = arad_tcam_db_occ_bmp_entry_closest_get(
                unit,
                tcam_db_id,
                ARAD_TCAM_DIRECTION_BACKWARD,
                &location,
                &entry_id_prev,
                &priority_prev,
                &location_prev,
                &found_prev
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

      res = arad_tcam_db_occ_bmp_entry_closest_get(
                unit,
                tcam_db_id,
                ARAD_TCAM_DIRECTION_FORWARD,
                &location,
                &entry_id_next,
                &priority_next,
                &location_next,
                &found_next
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
  }
  else {
      
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_priorities.get(unit, tcam_db_id, &list);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);

      
      priority_found = FALSE;
      
      current  = SOC_SAND_SORTED_LIST_ITER_END(unit,list);
      res = soc_sand_sorted_list_get_prev( 
              unit,
              list,
              &current,
              key_buffer_current,
              data_buffer
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      found_prev = FALSE ;
      do
      {
        uint32 num_elements ;
        uint8 is_indices_enabled ;
        res = soc_sand_sorted_list_is_indices_enabled(unit, list,&is_indices_enabled) ;
        SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit) ;
         
        if (!is_indices_enabled)
        {
          
          while (!priority_found && (current != SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,list))) 
          {
            node_priority = arad_tcam_db_prio_list_priority_value_decode(key_buffer_current);
            
            if ( (insertion_order_respected && (priority >= node_priority))
                || (!(insertion_order_respected) && (priority > node_priority)))
            {
                
                priority_found = TRUE;
                break;
            }
            else
            {
              res = soc_sand_sorted_list_get_prev( 
                      unit,
                      list,
                      &current,
                      key_buffer_current,
                      data_buffer
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);
            }
          }
        }
        else
        {
          
          uint32
            index_on_indices,
            bottom_index_on_indices,
            top_index_on_indices ;
          
          res = soc_sand_sorted_list_get_num_elements(unit,list,&num_elements) ;
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit) ;
          if (num_elements == 0)
          {
            
            current  = SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,list);
            break ;
          }
          
          node_priority = arad_tcam_db_prio_list_priority_value_decode(key_buffer_current) ;
          if ( (insertion_order_respected && (priority >= node_priority))
              || (!(insertion_order_respected) && (priority > node_priority)) )
          {
            
            
            priority_found = TRUE ;
            break ;
          }
          
          res = soc_sand_sorted_list_get_iter_from_indices(unit,list,0,&current) ;
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit) ;
          res = soc_sand_sorted_list_entry_value(unit,list,current,key_buffer_current,data_buffer) ;
          SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit) ;
          node_priority = arad_tcam_db_prio_list_priority_value_decode(key_buffer_current) ;
          if ( (insertion_order_respected && (priority < node_priority))
              || (!(insertion_order_respected) && (priority <= node_priority)) )
          {
            
            current  = SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,list);
            break ;
          }
          
          top_index_on_indices = num_elements - 1 ;
          bottom_index_on_indices = 0 ;
          while (!priority_found)
          {
            index_on_indices = (top_index_on_indices - bottom_index_on_indices) / 2 ;
            if (index_on_indices == 0)
            {
              
              res = soc_sand_sorted_list_get_iter_from_indices(unit,list,top_index_on_indices,&current) ;
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit) ;
              res = soc_sand_sorted_list_entry_value(unit,list,current,key_buffer_current,data_buffer) ;
              SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit) ;
              node_priority = arad_tcam_db_prio_list_priority_value_decode(key_buffer_current) ;
              if ( (insertion_order_respected && (priority >= node_priority))
                  || (!(insertion_order_respected) && (priority > node_priority)) )
              {
                
                priority_found = TRUE ;
                break ;
              }
              res = soc_sand_sorted_list_get_iter_from_indices(unit,list,bottom_index_on_indices,&current) ;
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit) ;
              res = soc_sand_sorted_list_entry_value(unit,list,current,key_buffer_current,data_buffer) ;
              SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit) ;
              node_priority = arad_tcam_db_prio_list_priority_value_decode(key_buffer_current) ;
              if ( (insertion_order_respected && (priority >= node_priority))
                  || (!(insertion_order_respected) && (priority > node_priority)) )
              {
                
                priority_found = TRUE ;
                break ;
              }
              
              res = SOC_SAND_GET_ERR_TXT_ERR ;
              SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit) ;
              
              break ;
            }
            index_on_indices += bottom_index_on_indices ;
            
            res = soc_sand_sorted_list_get_iter_from_indices(unit,list,index_on_indices,&current) ;
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 46, exit) ;
            res = soc_sand_sorted_list_entry_value(unit,list,current,key_buffer_current,data_buffer) ;
            SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit) ;
            node_priority = arad_tcam_db_prio_list_priority_value_decode(key_buffer_current) ;
            if (insertion_order_respected && (priority == node_priority))
            {
              
              bottom_index_on_indices = index_on_indices ;
              continue ;
            }
            
            if (priority > node_priority)
            {
              
              bottom_index_on_indices = index_on_indices ;
              continue ;
            }
            else
            {
              
              top_index_on_indices = index_on_indices ;
              continue ;
            }
          }
          if (!priority_found)
          {
            
            current  = SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,list);
          }
        }
      } while (0) ;

      found_node = current;
      if ( priority_found && (found_node != SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,list)) ) { 
          found_prev = TRUE;
          
          res = soc_sand_sorted_list_entry_value(
                  unit,
                  list,
                  found_node,
                  key_buffer_current,
                  data_buffer
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

          res = arad_tcam_db_priority_data_decode(unit, tcam_db_id, data_buffer, &prio_location);
          SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

          res = arad_tcam_db_entry_id_to_location_entry_get(
                    unit,
                    tcam_db_id,
                    prio_location.entry_id_last,
                    TRUE,
                    &location_prev,
                    &found_prev
                  );
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
          if (!found_prev) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "Unit %d DB id %d  Entry-ID %d not found altough defined in priority range %d.\n\r"), 
                          unit, tcam_db_id, prio_location.entry_id_last, node_priority));
               SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_BANK_UNINITIALIZED_ERR, 10, exit);
          }
      }
      else {
          
          found_prev = FALSE;
      }


      
      priority_found = FALSE;
      current = found_node;

      res = soc_sand_sorted_list_get_next(
              unit,
              list,
              &current,
              key_buffer_current,
              data_buffer
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 604, exit);
      found_next = FALSE;
      while (!priority_found && (current != SOC_SAND_SORTED_LIST_ITER_END(unit,list)))
      {
        node_priority = arad_tcam_db_prio_list_priority_value_decode(key_buffer_current);
        if (priority < node_priority) 
        {
            if ( current != SOC_SAND_SORTED_LIST_ITER_END(unit,list))  { 
              priority_found = TRUE;
              found_next = TRUE;
              
              res = arad_tcam_db_priority_data_decode(unit, tcam_db_id, data_buffer, &prio_location);
              SOC_SAND_CHECK_FUNC_RESULT(res, 66, exit);

              res = arad_tcam_db_entry_id_to_location_entry_get(
                        unit,
                        tcam_db_id,
                        prio_location.entry_id_first,
                        TRUE,
                        &location_next,
                        &found_next
                      );
              SOC_SAND_CHECK_FUNC_RESULT(res, 68, exit);
              if (!found_next) {
                   LOG_ERROR(BSL_LS_SOC_TCAM,
                             (BSL_META_U(unit,
                                         "Unit %d DB id %d  Entry-ID %d not found altough defined in priority range %d.\n\r"), 
                              unit, tcam_db_id, prio_location.entry_id_last, node_priority));
                   SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_BANK_UNINITIALIZED_ERR, 72, exit);
              }
            }
            else {
                
                found_next = FALSE;
                break;
            }
        }
        else
        {
          res = soc_sand_sorted_list_get_next(
                  unit,
                  list,
                  &current,
                  key_buffer_current,
                  data_buffer
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 74, exit);
        }
      }
  }

  
  if (found_prev == TRUE)
  {
      range->min.bank_id = location_prev.bank_id;
      range->min.entry = location_prev.entry;
  }
  else
  {
    range->min.bank_id = 0;
    range->min.entry   = 0;
    range->min_not_found = TRUE;
  }

  if (found_next == TRUE)
  {
      range->max.bank_id = location_next.bank_id;
      range->max.entry = location_next.entry;
  }
  else
  {
    range->max.bank_id = SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit) - 1;
    res = arad_tcam_db_entries_per_bank_get(unit, tcam_db_id, range->max.bank_id, &range->max.entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    --(range->max.entry);
    range->max_not_found = TRUE;
  }

    
#ifdef BROADCOM_DEBUG
  if ((range->min.bank_id > range->max.bank_id) || ((range->min.bank_id == range->max.bank_id) && (range->min.entry > range->max.entry)))
  {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Unit %d Tcam DB id %d Entry %d Priority %d Is-for-update %d has received an illegal "
                             "location range from (bank %d, entry %d) to (bank %d, entry %d).\n\r"),
                  unit, tcam_db_id, entry_id, priority, is_for_update, range->min.bank_id, range->min.entry, range->max.bank_id, range->max.entry));
       SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 50, exit);
  }
#endif 


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_priority_list_insertion_range_find", tcam_db_id, priority);
}

STATIC
  uint32
    arad_tcam_bank_entry_set_unsafe(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN uint8         set_hw_also,
      SOC_SAND_IN uint8         is_bank_share_update,
      SOC_SAND_IN uint8         hit_bit,
      SOC_SAND_IN  uint32            bank_id,
      SOC_SAND_IN  uint32             address,
      SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE action_bitmap_ndx,
      SOC_SAND_IN  ARAD_TCAM_ENTRY    *entry,
      SOC_SAND_IN  ARAD_TCAM_ACTION   *action
   )
{
  uint32
    res = SOC_SAND_OK,
    entries_free = 0;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size = 0;
  SOC_SAND_OCC_BM_PTR
    occ_bm;
  uint8
      is_inverse,
      is_valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.valid.get(unit, bank_id, &is_valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  
#ifdef CRASH_RECOVERY_SUPPORT
  
  if(BCM_UNIT_DO_HW_READ_WRITE(unit)) {
    arad_tcam_access_cr_entry_add(unit, bank_id, address, hit_bit, entry, action);
  }
#endif

  if (!is_valid)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d Bank id %d - Trying to insert entry to uninitialized bank.\n\r"), unit, bank_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_BANK_UNINITIALIZED_ERR, 10, exit);
  }

  
  if (set_hw_also) {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.entry_size.get(
              unit,
              bank_id,
              &entry_size
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit); 
      
      if (entry->valid) {
          res = arad_tcam_tbl_action_write(
                  unit,
                  bank_id, 
                  address,
                  action_bitmap_ndx,     
                  hit_bit,     
                  action
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

          res = arad_tcam_tbl_bank_write(
                  unit,
                  bank_id,
                  entry_size,
                  address,
                  entry
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
      }
      else { 
          res = arad_tcam_tbl_bank_write(
                  unit,
                  bank_id,
                  entry_size,
                  address,
                  entry
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

          res = arad_tcam_tbl_action_write(
                  unit,
                  bank_id, 
                  address,
                  action_bitmap_ndx,          
                  hit_bit,     
                  action
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);
      }
  }

  
  for (is_inverse = 0; is_inverse < ARAD_TCAM_NOF_OCCUPATIONS_BITMAPS; is_inverse++) {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_bank_entries_used.get(
                 unit,
                 bank_id,
                 is_inverse,
                 &occ_bm
               );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
      res = soc_sand_occ_bm_occup_status_set(
              unit,
              occ_bm,
              address,
              is_inverse? FALSE: TRUE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  if (!is_bank_share_update) {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.nof_entries_free.get(
              unit, 
              bank_id,
              &entries_free);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

      if(0 == entries_free)
      {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Unit %d Bank id %d - Trying to insert entry to full bank.\n\r"), unit, bank_id));
        SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_BANK_ENTRY_SIZE_OUT_OF_RANGE_ERR, 50, exit);
      }
      entries_free--;
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.nof_entries_free.set(
        unit, 
        bank_id, 
        entries_free);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_bank_entry_set_unsafe()", bank_id, address);
}

STATIC
  uint32
    arad_tcam_bank_entry_get_unsafe(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  uint32               bank_id,
      SOC_SAND_IN  uint32               address,
      SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE action_bitmap_ndx,
      SOC_SAND_IN  uint8                hit_bit_clear, 
      SOC_SAND_OUT ARAD_TCAM_ENTRY      *entry,
      SOC_SAND_OUT ARAD_TCAM_ACTION     *action,
      SOC_SAND_OUT uint8                *hit_bit
    )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;
  uint8
    is_valid;
#ifdef CRASH_RECOVERY_SUPPORT
  uint8
    found;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.valid.get(unit, bank_id, &is_valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  
  if (!is_valid)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d Band id %d - Failed to get tcam entry bank. The bank is not valid.\n\r"), unit, bank_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_BANK_UNINITIALIZED_ERR, 10, exit);
  }

#ifdef CRASH_RECOVERY_SUPPORT
  if(BCM_UNIT_DO_HW_READ_WRITE(unit)) {
    found = FALSE;
    arad_tcam_access_cr_entry_get(unit, bank_id, address, hit_bit, entry, action, &found);
    
    if(found) {
        return SOC_E_NONE;
    }
}
soc_hw_set_immediate_hw_access(unit);
#endif

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.entry_size.get(
          unit,
          bank_id,
          &entry_size
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
  
  res = arad_tcam_tbl_bank_read(
          unit,
          bank_id,
          entry_size,
          address,
          entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_tcam_tbl_action_read(
          unit,
          bank_id, 
          address, 
          action_bitmap_ndx,         
          hit_bit,
          action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  if (hit_bit_clear) {
      res = arad_tcam_tbl_action_write(
              unit,
              bank_id, 
              address,
              action_bitmap_ndx,     
              0, 
              action
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);
  }

exit:
#ifdef CRASH_RECOVERY_SUPPORT
  soc_hw_restore_immediate_hw_access(unit);
#endif

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_bank_entry_get_unsafe()", bank_id, address);
}

STATIC
  uint32
    arad_tcam_bank_entry_invalidate_unsafe(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint8         set_hw_also,
      SOC_SAND_IN uint8         is_bank_share_update,
      SOC_SAND_IN uint8         is_keep_location,
      SOC_SAND_IN uint32 bank_id,
      SOC_SAND_IN uint32  address
    )
{
  uint32
    res = SOC_SAND_OK,
      nof_entries_in_line,
    entries_free;
  SOC_SAND_OCC_BM_PTR
    occ_bm;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;
  uint8
      is_inverse,
      is_valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.valid.get(unit, bank_id, &is_valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  if (!is_valid)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d Band id %d - Failed to invalidate tcam entry bank. The bank is not valid.\n\r"), unit, bank_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_BANK_UNINITIALIZED_ERR, 10, exit);
  }

#ifdef CRASH_RECOVERY_SUPPORT
  
  if(BCM_UNIT_DO_HW_READ_WRITE(unit)) {
    arad_tcam_access_cr_entry_remove(unit, bank_id, address);
  }
#endif

  
  if (!is_keep_location) {
    for (is_inverse = 0; is_inverse < ARAD_TCAM_NOF_OCCUPATIONS_BITMAPS; is_inverse++) {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_bank_entries_used.get(
                 unit,
                 bank_id,
                 is_inverse,
                 &occ_bm
               );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

      res = soc_sand_occ_bm_occup_status_set(
              unit,
              occ_bm,
              address,
              is_inverse? TRUE: FALSE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
    }
  }
  if (!is_bank_share_update) {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.nof_entries_free.get(
              unit, 
              bank_id,
              &entries_free
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 17, exit);

      entries_free++;
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.nof_entries_free.set(
        unit, 
        bank_id, 
        entries_free);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);
  }
  
  
  if (set_hw_also) {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.entry_size.get(
              unit,
              bank_id,
              &entry_size
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 19, exit);
      nof_entries_in_line = (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) ? 3 : 
                              ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS) ? 2 : 1);

      res = arad_pp_ihb_tcam_invalid_bit_set_unsafe(
              unit,
              bank_id,
              nof_entries_in_line,
              address
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_bank_entry_invalidate_unsafe()", bank_id, address);
}

 
STATIC
  void
    arad_tcam_db_bit_search_range_in_bank_mode_get(
      SOC_SAND_IN  ARAD_TCAM_RANGE *range,
      SOC_SAND_IN  uint32     bank_id,
      SOC_SAND_IN  uint32     entry_count,
      SOC_SAND_OUT uint32      *start,
      SOC_SAND_OUT uint32      *end,
      SOC_SAND_OUT uint8     *skip
    )
{
  if (bank_id < range->min.bank_id || bank_id > range->max.bank_id)
  {
    *skip = TRUE;
  }
  else
  {
    *skip  = FALSE;
    *start = (bank_id == range->min.bank_id) ? range->min.entry : 0;
    *end   = (bank_id == range->max.bank_id) ? range->max.entry : entry_count - 1;
  }
}


STATIC
  uint32
    arad_tcam_db_bit_search_range_get(
      SOC_SAND_IN  int     unit,
      SOC_SAND_IN  uint32     tcam_db_id,
      SOC_SAND_IN  ARAD_TCAM_RANGE *range,
      SOC_SAND_IN  uint32     bank_id,
      SOC_SAND_OUT uint32      *start,
      SOC_SAND_OUT uint32      *end,
      SOC_SAND_OUT uint8     *skip
    )
{
  uint8
    bank_used;
  uint32
    entry_count,
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(
          unit,
          tcam_db_id,
          bank_id,
          &bank_used
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  if (!bank_used)
  {
    *skip = TRUE;
  }
  else
  {
    *skip = FALSE;
      res = arad_tcam_db_entries_per_bank_get(unit, tcam_db_id, bank_id, &entry_count);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      arad_tcam_db_bit_search_range_in_bank_mode_get(
        range,
        bank_id,
        entry_count,
        start,
        end,
        skip
      );
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_bit_search_range_get()", 0, 0);
}

STATIC
  uint32
    arad_tcam_db_priority_list_entry_iter_get(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  uint32                       tcam_db_id,
      SOC_SAND_IN  uint32                       priority,
      SOC_SAND_OUT SOC_SAND_SORTED_LIST_ITER    *iter,
      SOC_SAND_OUT uint8                        *found
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SORTED_LIST_PTR
    prio_list;
  uint8
    key_buffer[ARAD_TCAM_DB_LIST_KEY_SIZE];
  uint8
    is_indices_enabled ;
  uint32
    index_on_indices ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  index_on_indices = -1 ;
  arad_tcam_db_prio_list_priority_value_encode(priority, key_buffer);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_priorities.get(unit, tcam_db_id, &prio_list);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_sorted_list_is_indices_enabled(unit,prio_list,&is_indices_enabled) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  if (is_indices_enabled) {
    res = soc_sand_sorted_list_find_lower_eq_key(unit,prio_list,key_buffer,iter,&index_on_indices,found) ;
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit) ;
  } else {
    res = soc_sand_sorted_list_entry_lookup(
          unit,
          prio_list,
          key_buffer,
          NULL,
          found,
          iter
        ) ;
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit) ;
  }
exit:

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_priority_list_entry_iter_get()", 0, 0) ;
}

STATIC
  uint32
    arad_tcam_closest_free_line_find_helper(
      SOC_SAND_IN  int         unit,
      SOC_SAND_IN  uint32         bank_id,
      SOC_SAND_IN  ARAD_TCAM_DIRECTION direction,
      SOC_SAND_IN  uint32         start,
      SOC_SAND_IN  uint32         end,
      SOC_SAND_IN  uint8          is_inserted_top,
      
      SOC_SAND_OUT uint32         *place_first_free,
      SOC_SAND_OUT uint32         *place_chosen,
      SOC_SAND_OUT uint8         *found
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_OCC_BM_PTR
    bank_bm;
#if ARAD_TCAM_INSERTION_ALGORITHM_MIDDLE_FREE
  uint32
      start_lcl_middle,
      end_lcl_middle,
      place_taken;
   uint8         
       found_taken;
#endif 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_bank_entries_used.get(unit, bank_id, FALSE, &bank_bm);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  res = soc_sand_occ_bm_get_next_in_range(
          unit,
          bank_bm,
          start,
          end,
          (direction == ARAD_TCAM_DIRECTION_FORWARD)? TRUE : FALSE,
          place_first_free,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  *place_chosen = *place_first_free;

#if ARAD_TCAM_INSERTION_ALGORITHM_MIDDLE_FREE
  if ((*found) && (!is_inserted_top)) {
      
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_bank_entries_used.get(unit, bank_id, TRUE , &bank_bm);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
      start_lcl_middle = (direction == ARAD_TCAM_DIRECTION_FORWARD)? (*place_first_free): start;
      end_lcl_middle = (direction == ARAD_TCAM_DIRECTION_FORWARD)? end: (*place_first_free);
      res = soc_sand_occ_bm_get_next_in_range(
              unit,
              bank_bm,
              start_lcl_middle,
              end_lcl_middle,
              (direction == ARAD_TCAM_DIRECTION_FORWARD)? TRUE : FALSE,
              &place_taken,
              &found_taken
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      *place_chosen = ARAD_TCAM_INSERTION_ALGORITHM_MIDDLE_FREE_FIND(found_taken, direction, place_taken, start_lcl_middle, end_lcl_middle);
  }
#endif 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_closest_free_line_find_helper()", 0, direction);
}


STATIC
  uint32
    arad_tcam_jump_line_find(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  uint32                       bank_id,
      SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE    entry_size,
      SOC_SAND_IN  ARAD_TCAM_DIRECTION          direction,
      SOC_SAND_IN  uint32                       start,
      SOC_SAND_IN  uint32                       end,
      SOC_SAND_IN  uint32                       nof_free_entries_jump,
      SOC_SAND_OUT uint32                       *place_chosen,
      SOC_SAND_OUT uint8                        *found
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_OCC_BM_PTR
    bank_bm;
  uint32
      free_entry_idx,
      start_lcl,
      end_lcl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *found = FALSE;
  start_lcl = start;
  end_lcl = end;
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_bank_entries_used.get(unit, bank_id, FALSE, &bank_bm);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  *found = TRUE;
  if (nof_free_entries_jump == 0) {
      *found = TRUE;
      *place_chosen = (direction == ARAD_TCAM_DIRECTION_FORWARD)? start: end;
  }
  else {
      for (free_entry_idx = 0; (free_entry_idx < nof_free_entries_jump) && (*found == TRUE); free_entry_idx++) {
          res = soc_sand_occ_bm_get_next_in_range(
                  unit,
                  bank_bm,
                  start_lcl,
                  end_lcl,
                  (direction == ARAD_TCAM_DIRECTION_FORWARD)? TRUE : FALSE,
                  place_chosen,
                  found
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
          if (direction == ARAD_TCAM_DIRECTION_FORWARD) {
              start_lcl = *place_chosen + 1;
          }
          else {
              end_lcl = *place_chosen - 1;
          }
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_jump_line_find()", 0, direction);
}



STATIC
  uint32
    arad_tcam_closest_free_line_find(
      SOC_SAND_IN  int         unit,
      SOC_SAND_IN  uint32         tcam_db_id,
      SOC_SAND_IN  uint32         bank_id,
      SOC_SAND_IN  ARAD_TCAM_DIRECTION direction,
      SOC_SAND_IN  uint32         start,
      SOC_SAND_IN  uint32         end,
      SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE  entry_size,
      SOC_SAND_IN  uint8          is_inserted_top,
      
      SOC_SAND_IN  uint8          allow_pair_same_db, 
      SOC_SAND_OUT uint32         *place_first_free,
      SOC_SAND_OUT uint32         *place_chosen,
      SOC_SAND_OUT uint8         *found
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  
      res = arad_tcam_closest_free_line_find_helper(
                unit,
                bank_id,
                direction,
                start,
                end,
                is_inserted_top,
                place_first_free,
                place_chosen,
                found
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_closest_free_line_find()", tcam_db_id, direction);
}

STATIC
  uint32
    arad_tcam_db_empty_location_in_range_find(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  uint8                is_inserted_top,
      SOC_SAND_IN  ARAD_TCAM_RANGE        *range,
      SOC_SAND_OUT ARAD_TCAM_LOCATION     *location,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success
    )
{
  uint32
    res = SOC_SAND_OK,
      place_first_free,
    start_bit,
    end_bit;
  uint32
    bank_id;
  uint8
    found,
    skip;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(range);
  SOC_SAND_CHECK_NULL_INPUT(location);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit, tcam_db_id, &entry_size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  
  found = FALSE;
  for (bank_id = 0; (!found) && (bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit)); ++bank_id)
  {
    res = arad_tcam_db_bit_search_range_get(
      unit,
      tcam_db_id,
      range,
      bank_id,
      &start_bit,
      &end_bit,
      &skip
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (!skip)
    {
        
        res = arad_tcam_closest_free_line_find(
                unit,
                tcam_db_id,
                bank_id,
                ARAD_TCAM_DIRECTION_FORWARD,
                start_bit,
                end_bit,
                entry_size,
                is_inserted_top,
                TRUE, 
                &place_first_free,
                &location->entry,
                &found
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      location->bank_id = bank_id;

      
      if (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) {
          ++bank_id;
      }
    }
  }

  *success = found ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_empty_location_in_range_find()", tcam_db_id, 0);
}


#ifdef BROADCOM_DEBUG

STATIC
  uint32
    arad_tcam_db_priority_list_entry_verify(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  uint32             priority
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SORTED_LIST_PTR
    prio_list;
  uint8
    prio_node_found,
      found,
      key_buffer_previous[ARAD_TCAM_DB_LIST_KEY_SIZE],
      data_buffer_previous[ARAD_TCAM_DB_LIST_DATA_SIZE],
      key_buffer_next[ARAD_TCAM_DB_LIST_KEY_SIZE],
      data_buffer_next[ARAD_TCAM_DB_LIST_DATA_SIZE],
    key_buffer[ARAD_TCAM_DB_LIST_KEY_SIZE],
    data_buffer[ARAD_TCAM_DB_LIST_DATA_SIZE];
  SOC_SAND_SORTED_LIST_ITER    
      current,
      iter;
  ARAD_TCAM_PRIO_LOCATION 
      prio_location_previous,
      prio_location_next,
      prio_location;
  ARAD_TCAM_LOCATION 
      location_compare_before,
      location_compare_after;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_priorities.get(unit, tcam_db_id, &prio_list);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  res = arad_tcam_db_priority_list_entry_iter_get(
            unit,
            tcam_db_id,
            priority,
            &iter,
            &prio_node_found
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

  if (!prio_node_found) {
      
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Unit %d Tcam DB id %d has no priority %d node although expected.\n\r"),
                  unit, tcam_db_id, priority));
       SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 12, exit);
  }


      
     res = soc_sand_sorted_list_entry_value(
            unit,
            prio_list,
            iter,
            key_buffer,
            data_buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    
    if (priority != arad_tcam_db_prio_list_priority_value_decode(key_buffer)) {
         LOG_ERROR(BSL_LS_SOC_TCAM,
                   (BSL_META_U(unit,
                               "Unit %d Tcam DB id %d has priority %d but the matched iter %d has different priority %d.\n\r"),
                    unit, tcam_db_id, priority, iter, arad_tcam_db_prio_list_priority_value_decode(key_buffer)));
         SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 50, exit);
    }

    res = arad_tcam_db_priority_data_decode(unit, tcam_db_id, data_buffer, &prio_location);
    SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

    
    current = iter;
    res = soc_sand_sorted_list_get_next(
            unit,
            prio_list,
            &current,
            key_buffer_next,
            data_buffer_next
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);
    if (current != SOC_SAND_SORTED_LIST_ITER_END(unit,prio_list)) {
        res = arad_tcam_db_priority_data_decode(unit, tcam_db_id, data_buffer_next, &prio_location_next);
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
        if (arad_tcam_db_prio_list_priority_value_decode(key_buffer_next) <= arad_tcam_db_prio_list_priority_value_decode(key_buffer)) {
             LOG_ERROR(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "Unit %d Tcam DB id %d priority %d node is before priority %d node.\n\r"),
                                   unit, tcam_db_id, arad_tcam_db_prio_list_priority_value_decode(key_buffer), 
                        arad_tcam_db_prio_list_priority_value_decode(key_buffer_next)));
             SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 44, exit);
        }

        
        res = arad_tcam_db_entry_id_to_location_entry_get(
                unit,
                tcam_db_id,
                prio_location.entry_id_last,
                TRUE,
                &location_compare_before,
                &found
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);
        if (!found) {
             LOG_ERROR(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "Unit %d Tcam DB id %d entry %d is supposed to be found.\n\r"),
                        unit, tcam_db_id, prio_location.entry_id_last));
             SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 41, exit);
        }

        
        res = arad_tcam_db_entry_id_to_location_entry_get(
                unit,
                tcam_db_id,
                prio_location_next.entry_id_first,
                TRUE,
                &location_compare_after,
                &found
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);
        if (!found) {
             LOG_ERROR(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "Unit %d Tcam DB id %d entry %d is supposed to be found.\n\r"),
                        unit, tcam_db_id, prio_location_next.entry_id_first));
             SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 53, exit);
        }

        if ((location_compare_after.bank_id < location_compare_before.bank_id) || 
            ((location_compare_after.bank_id == location_compare_before.bank_id) && (location_compare_after.entry < location_compare_before.entry))) {    
             LOG_ERROR(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "Unit %d Tcam DB id %d the priority %d node last entry %d in location (%d, %d)"
                                   "was suppposed to be before the priority %d node first entry %d in location (%d, %d).\n\r"),
                        unit, tcam_db_id, arad_tcam_db_prio_list_priority_value_decode(key_buffer), prio_location.entry_id_last, 
                        location_compare_before.bank_id, location_compare_before.entry, arad_tcam_db_prio_list_priority_value_decode(key_buffer_next),
                        prio_location_next.entry_id_first, location_compare_after.bank_id, location_compare_after.entry));
             SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 55, exit);
        }
    }


    
    current = iter;
    res = soc_sand_sorted_list_get_prev(
            unit,
            prio_list,
            &current,
            key_buffer_previous,
            data_buffer_previous
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);
    if (current != SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,prio_list)) {
        res = arad_tcam_db_priority_data_decode(unit, tcam_db_id, data_buffer_previous, &prio_location_previous);
        SOC_SAND_CHECK_FUNC_RESULT(res, 59, exit);

        if (arad_tcam_db_prio_list_priority_value_decode(key_buffer_previous) >= arad_tcam_db_prio_list_priority_value_decode(key_buffer)) {
             LOG_ERROR(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "Unit %d Tcam DB id %d priority %d node is after priority %d node.\n\r"),
                                   unit, tcam_db_id, arad_tcam_db_prio_list_priority_value_decode(key_buffer), 
                        arad_tcam_db_prio_list_priority_value_decode(key_buffer_previous)));
             SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 61, exit);
        }
        
        
        res = arad_tcam_db_entry_id_to_location_entry_get(
                unit,
                tcam_db_id,
                prio_location_previous.entry_id_last,
                TRUE,
                &location_compare_before,
                &found
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 63, exit);
        if (!found) {
             LOG_ERROR(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "Unit %d Tcam DB id %d entry %d is supposed to be found.\n\r"),
                        unit, tcam_db_id, prio_location_previous.entry_id_last));
             SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 65, exit);
        }

        
        res = arad_tcam_db_entry_id_to_location_entry_get(
                unit,
                tcam_db_id,
                prio_location.entry_id_first,
                TRUE,
                &location_compare_after,
                &found
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 67, exit);
        if (!found) {
             LOG_ERROR(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "Unit %d Tcam DB id %d entry %d is supposed to be found.\n\r"),
                        unit, tcam_db_id, prio_location.entry_id_first));
             SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 69, exit);
        }

        if ((location_compare_after.bank_id < location_compare_before.bank_id) || 
            ((location_compare_after.bank_id == location_compare_before.bank_id) && (location_compare_after.entry < location_compare_before.entry))) {    
             LOG_ERROR(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "Unit %d Tcam DB id %d the priority %d node last entry %d in location (%d, %d)"
                                   "was suppposed to be before the priority %d node first entry %d in location (%d, %d).\n\r"),
                        unit, tcam_db_id, arad_tcam_db_prio_list_priority_value_decode(key_buffer_previous), prio_location_previous.entry_id_last, 
                        location_compare_before.bank_id, location_compare_before.entry, arad_tcam_db_prio_list_priority_value_decode(key_buffer),
                        prio_location.entry_id_first, location_compare_after.bank_id, location_compare_after.entry));
             SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 71, exit);
        }
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_priority_list_entry_verify()", tcam_db_id, 0);
}
#endif 





STATIC
  uint32
    arad_tcam_db_priority_list_entry_add(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  ARAD_TCAM_LOCATION     *location,
      SOC_SAND_IN  uint32            entry_id,
      SOC_SAND_IN  uint32             priority,
      SOC_SAND_IN  uint8        is_update_prio, 
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SORTED_LIST_PTR
    prio_list;
  uint8
    prio_node_found,
      found,
    key_buffer[ARAD_TCAM_DB_LIST_KEY_SIZE],
    data_buffer[ARAD_TCAM_DB_LIST_DATA_SIZE];
  uint8
    entry_added;
  SOC_SAND_SORTED_LIST_ITER    
      iter;
  ARAD_TCAM_PRIO_LOCATION 
      prio_location;
  ARAD_TCAM_LOCATION 
      location_compare;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_priorities.get(unit, tcam_db_id, &prio_list);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  

  res = arad_tcam_db_priority_list_entry_iter_get(
            unit,
            tcam_db_id,
            priority,
            &iter,
            &prio_node_found
          );

  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

  if (!prio_node_found) {
      if (is_update_prio) {
          
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Unit %d Tcam DB id %d entry %d has priority %d but no node although expected during update.\n\r"),
                      unit, tcam_db_id, entry_id, priority));
           SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 50, exit);
      }

      arad_tcam_db_prio_list_priority_value_encode(priority, key_buffer);
      ARAD_TCAM_PRIO_LOCATION_clear(&prio_location);
      prio_location.entry_id_first = entry_id;
      prio_location.entry_id_last = entry_id;
      arad_tcam_db_prio_list_data_encode(&prio_location, data_buffer);

      res = soc_sand_sorted_list_entry_add(
              unit,
              prio_list,
              key_buffer,
              data_buffer,
              &entry_added
            );

      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      *success = entry_added? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }
  else {

      
      *success = SOC_SAND_SUCCESS;

      
     res = soc_sand_sorted_list_entry_value(
            unit,
            prio_list,
            iter,
            key_buffer,
            data_buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    
    if (priority != arad_tcam_db_prio_list_priority_value_decode(key_buffer)) {
         LOG_ERROR(BSL_LS_SOC_TCAM,
                   (BSL_META_U(unit,
                               "Unit %d Tcam DB id %d entry %d has priority %d but the matched iter %d has different priority %d.\n\r"),
                    unit, tcam_db_id, entry_id, priority, iter, arad_tcam_db_prio_list_priority_value_decode(key_buffer)));
         SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 50, exit);
    }

    res = arad_tcam_db_priority_data_decode(unit, tcam_db_id, data_buffer, &prio_location);
    SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

    
    res = arad_tcam_db_entry_id_to_location_entry_get(
            unit,
            tcam_db_id,
            prio_location.entry_id_first,
            TRUE,
            &location_compare,
            &found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

    
    if ((!found) || 
        (((prio_location.entry_id_first == entry_id) || (prio_location.entry_id_last == entry_id)) && (!is_update_prio))) {
         LOG_ERROR(BSL_LS_SOC_TCAM,
                   (BSL_META_U(unit,
                               "Unit %d Tcam DB id %d entry %d is supposed to be found because in priority %d range.\n\r"),
                    unit, tcam_db_id, prio_location.entry_id_first, priority));
         SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 41, exit);
    }

    if (is_update_prio && ((prio_location.entry_id_first == entry_id) || (prio_location.entry_id_last == entry_id))) {
        
        ARAD_DO_NOTHING_AND_EXIT;
    }
    else if ((location->bank_id < location_compare.bank_id) || 
        ((location->bank_id == location_compare.bank_id) && (location->entry < location_compare.entry))) {
        
        prio_location.entry_id_first = entry_id;
        arad_tcam_db_prio_list_data_encode(&prio_location, data_buffer);
        res = soc_sand_sorted_list_entry_update(
                unit,
                prio_list,
                iter,
                key_buffer,
                data_buffer
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);
    }
    else {
        
        res = arad_tcam_db_entry_id_to_location_entry_get(
                unit,
                tcam_db_id,
                prio_location.entry_id_last,
                TRUE,
                &location_compare,
                &found
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 59, exit);
        
        if (!found) {
             LOG_ERROR(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "Unit %d Tcam DB id %d entry %d is supposed to be found because in priority %d range.\n\r"),
                        unit, tcam_db_id, prio_location.entry_id_first, priority));
             SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 41, exit);
        }

        if ((location->bank_id > location_compare.bank_id) || 
            ((location->bank_id == location_compare.bank_id) && (location->entry > location_compare.entry))) {
            
            prio_location.entry_id_last = entry_id;
            arad_tcam_db_prio_list_data_encode(&prio_location, data_buffer);
            res = soc_sand_sorted_list_entry_update(
                    unit,
                    prio_list,
                    iter,
                    key_buffer,
                    data_buffer
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 61, exit);
        }
    }

  }

#ifdef BROADCOM_DEBUG
  

  res = arad_tcam_db_priority_list_entry_verify(unit, tcam_db_id, priority);

  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
#endif 

exit:

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_priority_list_entry_add()", tcam_db_id, entry_id);
}

STATIC
  uint32
    arad_tcam_global_location_table_entry_add(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  ARAD_TCAM_LOCATION     *location,
      SOC_SAND_IN  uint32            entry_id,
      SOC_SAND_IN  uint32             priority
    )
{
  uint32
      global_location_id,
    res = SOC_SAND_OK;
  ARAD_TCAM_GLOBAL_LOCATION 
      global_location;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_tcam_global_location_encode(unit, location, &global_location_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  global_location.tcam_db_id = tcam_db_id;
  global_location.entry_id = entry_id;
  global_location.priority = priority;
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.global_location_tbl.set(unit, global_location_id, &global_location);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_global_location_table_entry_add()", tcam_db_id, entry_id);
}

STATIC
  uint32
    arad_tcam_db_priority_list_entry_get(
      SOC_SAND_IN  int        unit,
      SOC_SAND_IN  uint32        tcam_db_id,
      SOC_SAND_IN  ARAD_TCAM_LOCATION *location,
      SOC_SAND_OUT uint32        *entry_id,
      SOC_SAND_OUT uint32         *priority
    )
{
  uint32
      global_location_id,
    res = SOC_SAND_OK;
  ARAD_TCAM_GLOBAL_LOCATION 
      global_location;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_tcam_global_location_encode(unit, location, &global_location_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.global_location_tbl.get(unit, global_location_id, &global_location);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  *priority = global_location.priority;
  *entry_id = global_location.entry_id;

  if (global_location.tcam_db_id != tcam_db_id) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Unit %d Tcam DB id %d in location (bank %d, entry %d) was indicated in global location table to be for DB %d.\n\r"),
                  unit, tcam_db_id, location->bank_id, location->entry, global_location.tcam_db_id));
       SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_priority_list_entry_get()", tcam_db_id, 0);
}


STATIC
  uint32
    arad_tcam_db_priority_list_entry_remove(
      SOC_SAND_IN int        unit,
      SOC_SAND_IN uint32        tcam_db_id,
      SOC_SAND_IN ARAD_TCAM_LOCATION *location,
      SOC_SAND_IN uint32        entry_id,
      SOC_SAND_IN  uint32       priority,
      SOC_SAND_IN  uint8        is_update_prio 
    )
{
  uint32
      entry_id_next,
      next_priority,
    res = SOC_SAND_OK;
  SOC_SAND_SORTED_LIST_PTR
    prio_list;
  SOC_SAND_SORTED_LIST_ITER
    iter;
  uint8
      prio_node_found,
      found,
      key_buffer[ARAD_TCAM_DB_LIST_KEY_SIZE],
      data_buffer[ARAD_TCAM_DB_LIST_DATA_SIZE];
  ARAD_TCAM_PRIO_LOCATION 
      prio_location;
  ARAD_TCAM_LOCATION
      next_location;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#ifdef BROADCOM_DEBUG
  
  res = arad_tcam_db_priority_list_entry_verify(unit, tcam_db_id, priority);
  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
#endif 

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_priorities.get(unit, tcam_db_id, &prio_list);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  res = arad_tcam_db_priority_list_entry_iter_get(
            unit,
            tcam_db_id,
            priority,
            &iter,
            &prio_node_found
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
  if (!prio_node_found) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Unit %d Tcam DB id %d entry %d has priority %d but no such node found in priority list.\n\r"),
                  unit, tcam_db_id, entry_id, priority));
       SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 9, exit);
  }

     
     res = soc_sand_sorted_list_entry_value(
            unit,
            prio_list,
            iter,
            key_buffer,
            data_buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    
    if (priority != arad_tcam_db_prio_list_priority_value_decode(key_buffer)) {
         LOG_ERROR(BSL_LS_SOC_TCAM,
                   (BSL_META_U(unit,
                               "Unit %d Tcam DB id %d entry %d has priority %d but the matched iter %d has different priority %d.\n\r"),
                    unit, tcam_db_id, entry_id, priority, iter, arad_tcam_db_prio_list_priority_value_decode(key_buffer)));
         SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 50, exit);
    }

    res = arad_tcam_db_priority_data_decode(unit, tcam_db_id, data_buffer, &prio_location);
    SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

    
    if ((prio_location.entry_id_first != entry_id) && (prio_location.entry_id_last != entry_id)) {
        
        ARAD_DO_NOTHING_AND_EXIT;
    }
    else if ((prio_location.entry_id_first == entry_id) && (prio_location.entry_id_last == entry_id)) {
        
        if (is_update_prio) {
            
            ARAD_DO_NOTHING_AND_EXIT;
        }
        else {
            res = soc_sand_sorted_list_entry_remove_by_iter(unit, prio_list, iter);
            SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
        }
    }
    else if ((prio_location.entry_id_first == entry_id) && (prio_location.entry_id_last != entry_id)) {
        
        res = arad_tcam_db_occ_bmp_entry_closest_get(
                  unit,
                  tcam_db_id,
                  ARAD_TCAM_DIRECTION_FORWARD,
                  location,
                  &entry_id_next,
                  &next_priority,
                  &next_location,
                  &found
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
        if ((!found) || (next_priority != priority)) {
             LOG_ERROR(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "Unit %d Tcam DB id %d entry %d has priority %d but the next entry %d in range has priority %d and is found %d.\n\r"),
                        unit, tcam_db_id, entry_id, priority, entry_id_next, next_priority, found));
             SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 47, exit);
        }
        prio_location.entry_id_first = entry_id_next;
        arad_tcam_db_prio_list_data_encode(&prio_location, data_buffer);
        res = soc_sand_sorted_list_entry_update(
                unit,
                prio_list,
                iter,
                key_buffer,
                data_buffer
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);
    }
    else if ((prio_location.entry_id_first != entry_id) && (prio_location.entry_id_last == entry_id)) {
        
        res = arad_tcam_db_occ_bmp_entry_closest_get(
                  unit,
                  tcam_db_id,
                  ARAD_TCAM_DIRECTION_BACKWARD,
                  location,
                  &entry_id_next,
                  &next_priority,
                  &next_location,
                  &found
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
        if ((!found) || (next_priority != priority)) {
             LOG_ERROR(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "Unit %d Tcam DB id %d entry %d has priority %d but the next entry %d in range has priority %d and is found %d.\n\r"),
                        unit, tcam_db_id, entry_id, priority, entry_id_next, next_priority, found));
             SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 47, exit);
        }
        prio_location.entry_id_last = entry_id_next;
        arad_tcam_db_prio_list_data_encode(&prio_location, data_buffer);
        res = soc_sand_sorted_list_entry_update(
                unit,
                prio_list,
                iter,
                key_buffer,
                data_buffer
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 59, exit);
    }
    else {
        SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 100, exit);
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_priority_list_entry_remove()", 0, 0);
}

STATIC
  uint32
    arad_tcam_global_location_table_entry_remove(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  ARAD_TCAM_LOCATION     *location,
      SOC_SAND_OUT uint32           *priority
    )
{
  uint32
      global_location_id,
    res = SOC_SAND_OK;
  ARAD_TCAM_GLOBAL_LOCATION 
      global_location;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_tcam_global_location_encode(unit, location, &global_location_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.global_location_tbl.get(unit, global_location_id, &global_location);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  *priority = global_location.priority;
  if (global_location.tcam_db_id != tcam_db_id) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Unit %d Tcam DB id %d in location (bank %d, entry %d) was indicated in global location table to be for DB %d.\n\r"),
                  unit, tcam_db_id, location->bank_id, location->entry, global_location.tcam_db_id));
       SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 50, exit);
  }

  ARAD_TCAM_GLOBAL_LOCATION_clear(&global_location);
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.global_location_tbl.set(unit, global_location_id, &global_location);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_global_location_table_entry_remove()", tcam_db_id, 0);
}


STATIC
  uint32
    arad_tcam_db_bank_occ_bitmap_entry_get(
      SOC_SAND_IN int        unit,
      SOC_SAND_IN uint32        tcam_db_id,
      SOC_SAND_IN ARAD_TCAM_LOCATION *location,
      SOC_SAND_OUT uint8        *is_occupied
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_OCC_BM_PTR
    bank_bm;
  uint8
    bit_set;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_entries_used.get(unit, tcam_db_id, location->bank_id, &bank_bm);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_occ_bm_is_occupied(unit, bank_bm, location->entry, &bit_set);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  *is_occupied = bit_set? FALSE: TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_bank_occ_bitmap_entry_get()", 0, 0);
}



STATIC
  uint32
    arad_tcam_db_bank_occ_bitmap_entry_set(
      SOC_SAND_IN int        unit,
      SOC_SAND_IN uint32        tcam_db_id,
      SOC_SAND_IN ARAD_TCAM_LOCATION *location,
      SOC_SAND_IN uint8        is_occupied
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_OCC_BM_PTR
    bank_bm;
  uint8
    bit_set;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
#ifdef BROADCOM_DEBUG
  {
      uint8
        is_occupied_before = 0;

      
      res = arad_tcam_db_bank_occ_bitmap_entry_get(unit, tcam_db_id,  location, &is_occupied_before);
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      if (is_occupied_before == is_occupied)
      {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Unit %d Tcam DB id %d in location (bank %d, entry %d) was occupied %d but tries to set the same occupation %d.\n\r"),
                      unit, tcam_db_id, location->bank_id, location->entry, is_occupied_before, is_occupied));
           SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 50, exit);
      }
  }
#endif 


  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_entries_used.get(unit, tcam_db_id, location->bank_id, &bank_bm);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  bit_set = is_occupied? FALSE: TRUE;
  res = soc_sand_occ_bm_occup_status_set(unit, bank_bm, location->entry, bit_set);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_bank_occ_bitmap_entry_set()", 0, 0);
}



  uint32
    arad_tcam_db_entry_id_to_location_entry_add(
      SOC_SAND_IN int        unit,
      SOC_SAND_IN uint32        tcam_db_id,
      SOC_SAND_IN uint32        entry_id,
      SOC_SAND_IN ARAD_TCAM_LOCATION *location
    )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  SOC_SAND_HASH_TABLE_PTR
    hash_tbl;
  uint8
    key_buffer[ARAD_TCAM_DB_HASH_TBL_KEY_SIZE];
  uint8
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.entry_id_to_location.get(unit, &hash_tbl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  arad_tcam_db_entry_id_encode(tcam_db_id, entry_id, key_buffer);

  res = soc_sand_hash_table_entry_add(
          unit,
          hash_tbl,
          key_buffer,
          &data_indx,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!success)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d, Tcam db id %d Entry id %d, Failed to insert the entry to the hash table.\n\r"),
               unit, tcam_db_id, entry_id));
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 20, exit);
  }

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.db_location_tbl.set(unit, data_indx, location);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_id_to_location_entry_add()", 0, 0);
}

STATIC
  uint32
    arad_tcam_db_entry_id_to_location_entry_index_get(
      SOC_SAND_IN  int unit,
      SOC_SAND_IN  uint32 tcam_db_id,
      SOC_SAND_IN  uint32 entry_id,
      SOC_SAND_OUT uint32  *index,
      SOC_SAND_OUT uint8 *found
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_HASH_TABLE_PTR
    entry_id_to_location;
  uint8
    key_buffer[ARAD_TCAM_DB_HASH_TBL_KEY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.entry_id_to_location.get(unit, &entry_id_to_location);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  arad_tcam_db_entry_id_encode(tcam_db_id, entry_id, key_buffer);

  res = soc_sand_hash_table_entry_lookup(
          unit,
          entry_id_to_location,
          key_buffer,
          index,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_id_to_location_entry_index_get()", 0, 0);
}


uint32
  arad_tcam_db_entry_id_to_location_entry_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        tcam_db_id,
    SOC_SAND_IN  uint32        entry_id,
    SOC_SAND_IN  uint32        isLocation,
    SOC_SAND_OUT ARAD_TCAM_LOCATION *location,
    SOC_SAND_OUT uint8        *found
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_tcam_db_entry_id_to_location_entry_index_get(
          unit,
          tcam_db_id,
          entry_id,
          &data_indx,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*found)
  {
      
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.db_location_tbl.get(unit, data_indx, location);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);

      
#ifdef BROADCOM_DEBUG
      if (isLocation)
      {
          uint8
            is_occupied = 0;

          
          res = arad_tcam_db_bank_occ_bitmap_entry_get(unit, tcam_db_id, location, &is_occupied);
          SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
          if (is_occupied == FALSE)
          {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "Unit %d Tcam DB id %d in location (bank %d, entry %d) has an entry but indicated not to be occupied %d .\n\r"),
                          unit, tcam_db_id, location->bank_id, location->entry, is_occupied));
               SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 50, exit);
          }
      }
#endif 
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_id_to_location_entry_get()", 0, 0);
}

  uint32
    arad_tcam_db_entry_id_to_location_entry_remove(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32 tcam_db_id,
      SOC_SAND_IN uint32 entry_id
    )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  SOC_SAND_HASH_TABLE_PTR
    entry_id_to_location;
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_tcam_db_entry_id_to_location_entry_index_get(
          unit,
          tcam_db_id,
          entry_id,
          &data_indx,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (found)
  {
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.entry_id_to_location.get(unit, &entry_id_to_location);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
    res = soc_sand_hash_table_entry_remove_by_index(
            unit,
            entry_id_to_location,
            data_indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_id_to_location_entry_remove()", 0, 0);
}

STATIC
  uint32
    arad_tcam_db_entry_prefix_stamp(
      SOC_SAND_IN    int     unit,
      SOC_SAND_IN    uint32     tcam_db_id,
      SOC_SAND_INOUT ARAD_TCAM_ENTRY *entry
    )
{
  uint32
    res = SOC_SAND_OK,
    mask;
  ARAD_TCAM_PREFIX
    prefix;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;
  uint32
    entry_nof_bits;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.prefix.get(
    unit,
    tcam_db_id,
    &prefix
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);

  if (prefix.length > 0)
  {
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(
            unit,
            tcam_db_id,
            &entry_size
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
    switch (entry_size)
    {
    case ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS:
      entry_nof_bits = 80;
      break;

    case ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS:
      entry_nof_bits = 160;
      break;

    case ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS:
      entry_nof_bits = 320;
      break;

    default:
      
      entry_nof_bits = (uint32) -1;
      break;
    }

    res = soc_sand_bitstream_set_any_field(
            &prefix.bits,
            entry_nof_bits - prefix.length,
            prefix.length,
            entry->value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    mask = SOC_SAND_BITS_MASK(prefix.length - 1, 0);
    res = soc_sand_bitstream_set_any_field(
            &mask,
            entry_nof_bits - prefix.length,
            prefix.length,
            entry->mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_prefix_stamp()", tcam_db_id, 0);
}


STATIC
  uint32
    arad_tcam_location_distance_get(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  uint32                   tcam_db_id,
      SOC_SAND_IN  ARAD_TCAM_LOCATION       *low,
      SOC_SAND_IN  ARAD_TCAM_LOCATION       *high,
      SOC_SAND_OUT int32                    *distance
    )
{
  uint32
      bank_ndx,
      res;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;
  uint8
    is_used;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(
          unit,
          tcam_db_id,
          &entry_size
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    if (low->bank_id == high->bank_id)
    {
      *distance = high->entry - low->entry;
    }
    else
    {
      
      *distance = arad_tcam_bank_entry_size_to_entry_count_get(unit, entry_size, low->bank_id) - low->entry;
      for (bank_ndx = (low->bank_id + 1); bank_ndx < high->bank_id; bank_ndx++) {
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(unit, tcam_db_id, bank_ndx, &is_used);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
          if (is_used) {
            *distance += arad_tcam_bank_entry_size_to_entry_count_get(unit, entry_size, bank_ndx);
          }
      }
      *distance    += high->entry;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_location_distance_get()", tcam_db_id, 0);
}


STATIC
  uint32
    arad_tcam_closest_free_location_find(
      SOC_SAND_IN  int         unit,
      SOC_SAND_IN  uint32         tcam_db_id,
      SOC_SAND_IN  uint8          is_inserted_top,
      SOC_SAND_IN  ARAD_TCAM_LOCATION  *location,
      SOC_SAND_IN  uint8          location_not_found, 
      SOC_SAND_IN  ARAD_TCAM_DIRECTION direction,
      SOC_SAND_OUT ARAD_TCAM_LOCATION  *free_location_first, 
      SOC_SAND_OUT ARAD_TCAM_LOCATION  *free_location, 
      SOC_SAND_OUT int32         *distance,
      SOC_SAND_OUT uint8         *success
    )
{
  uint32
    res = SOC_SAND_OK,
    start,
    end,
      bank_id,
      place_first_free,
    place;
  int32
    current_distance = 0;
  uint8
    skip,
    found;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;
  ARAD_TCAM_RANGE
    search_range;
  ARAD_TCAM_LOCATION
    current;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_TCAM_RANGE_clear(&search_range);
  ARAD_TCAM_LOCATION_clear(&current);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(
          unit,
          tcam_db_id,
          &entry_size
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  
  switch (direction)
  {
  case ARAD_TCAM_DIRECTION_BACKWARD:
    search_range.min.bank_id = 0;
    search_range.min.entry   = 0;
    search_range.max.bank_id = location->bank_id;
    search_range.max.entry   = location->entry;
    break;

  case ARAD_TCAM_DIRECTION_FORWARD:
    search_range.min.bank_id = location->bank_id;
    search_range.min.entry   = location->entry;
    search_range.max.bank_id = SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit) - 1;
    search_range.max.entry   = arad_tcam_bank_entry_size_to_entry_count_get(unit, entry_size, search_range.max.bank_id) - 1;
    break;

  default:
    break;
  }

  *success = FALSE;
  
  if (location_not_found) {
      *success = FALSE;
      ARAD_DO_NOTHING_AND_EXIT;
  }


  for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
  {
      
      if ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) && (bank_id % 2)) {
          continue;
      }

    res = arad_tcam_db_bit_search_range_get(
      unit,
      tcam_db_id,
      &search_range,
      bank_id,
      &start,
      &end,
      &skip
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (!skip)
    {
        
        res = arad_tcam_closest_free_line_find(
                unit,
                tcam_db_id,
                bank_id,
                direction,
                start,
                end,
                entry_size,
                is_inserted_top,
                FALSE, 
                &place_first_free,
                &place,
                &found
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if (found)
      {
        current.bank_id  = bank_id;
        current.entry    = place;
        res = arad_tcam_location_distance_get(
                unit,
                tcam_db_id,
                (direction == ARAD_TCAM_DIRECTION_FORWARD)? location : &current, 
                (direction == ARAD_TCAM_DIRECTION_FORWARD)? &current : location ,
                &current_distance
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
#if ARAD_TCAM_INSERTION_ALGORITHM_MIDDLE_FREE
        
        if ((current.bank_id == location->bank_id) && (!is_inserted_top)) {
            
            current_distance = (-current_distance);
        }
#endif 

        if (!*success || (current_distance < *distance))
        {
          *free_location = current;
          *free_location_first = current;
          free_location_first->entry = place_first_free;
          *distance      = current_distance;
          *success       = TRUE;
        }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_closest_free_location_find()", tcam_db_id, direction);
}


STATIC
  uint32
    arad_tcam_closest_free_location_to_range_find(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  uint8             is_inserted_top,
      SOC_SAND_IN  ARAD_TCAM_RANGE        *range,
      SOC_SAND_OUT ARAD_TCAM_LOCATION     *free_location_first, 
      SOC_SAND_OUT ARAD_TCAM_LOCATION     *free_location,  
      SOC_SAND_OUT ARAD_TCAM_DIRECTION    *direction,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success
    )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    found_below,
    found_above;
  int32
    distance_below,
    distance_above;
  ARAD_TCAM_LOCATION
      below_first,
      above_first,
    below,
    above;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_tcam_closest_free_location_find(
          unit,
          tcam_db_id,
          is_inserted_top,
          &range->min,
          range->min_not_found,
          ARAD_TCAM_DIRECTION_BACKWARD,
          &below_first,
          &below,
          &distance_below,
          &found_below
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_tcam_closest_free_location_find(
          unit,
          tcam_db_id,
          is_inserted_top,
          &range->max,
          range->max_not_found,
          ARAD_TCAM_DIRECTION_FORWARD,
          &above_first,
          &above,
          &distance_above,
          &found_above
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!found_below && !found_above)
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
    if (found_below)
    {
      *free_location = below;
      *free_location_first = below_first;
      *direction     = ARAD_TCAM_DIRECTION_BACKWARD;
      if (found_above && (distance_above < distance_below))
      {
        *free_location = above;
        *free_location_first = above_first;
        *direction     = ARAD_TCAM_DIRECTION_FORWARD;
      }
    }
    else
    {
      *free_location = above;
      *free_location_first = above_first;
      *direction     = ARAD_TCAM_DIRECTION_FORWARD;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_closest_free_location_to_range_find()", tcam_db_id, 0);
}

void
    arad_tcam_db_location_decode_print(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN uint8  *buffer
    )
{
  ARAD_TCAM_LOCATION
    lhs;

  arad_tcam_db_location_decode(unit, buffer, &lhs);

  ARAD_TCAM_LOCATION_print(&lhs);
}

STATIC
  uint32
    arad_tcam_db_entry_move(
      SOC_SAND_IN int        unit,
      SOC_SAND_IN uint8         set_hw_also,
      SOC_SAND_IN uint8         is_bank_share_update,
      SOC_SAND_IN ARAD_TCAM_LOCATION *source,
      SOC_SAND_IN ARAD_TCAM_LOCATION *destination
    )
{
  uint32
      global_location_id_source,
      global_location_id_dest,
      bank_nof_entries,
      tcam_db_id = 0,
      priority,
    res = SOC_SAND_OK,
    data_indx,
    entry_id;
  SOC_SAND_HASH_TABLE_PTR
    hash_tbl;
  uint8
    hash_key[ARAD_TCAM_DB_HASH_TBL_KEY_SIZE];
  uint8
      hit_bit,
    found;
  ARAD_TCAM_ENTRY
    entry;
  ARAD_TCAM_ACTION
    action;
  ARAD_TCAM_ACTION_SIZE 
    action_bitmap_ndx;
  ARAD_TCAM_GLOBAL_LOCATION 
      global_location,
      global_location_clear;
  SOC_SAND_SUCCESS_FAILURE 
      success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  
  res = arad_tcam_global_location_encode(unit, source, &global_location_id_source);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.global_location_tbl.get(unit, global_location_id_source, &global_location);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);
  tcam_db_id = global_location.tcam_db_id;
  entry_id = global_location.entry_id;
  priority = global_location.priority;

  
  res = arad_tcam_global_location_encode(unit, destination, &global_location_id_dest);
  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.global_location_tbl.set(unit, global_location_id_dest, &global_location);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
  ARAD_TCAM_GLOBAL_LOCATION_clear(&global_location_clear);
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.global_location_tbl.set(unit, global_location_id_source, &global_location_clear);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 9, exit);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.entry_id_to_location.get(unit, &hash_tbl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  arad_tcam_db_entry_id_encode(tcam_db_id, entry_id, hash_key);
  res = soc_sand_hash_table_entry_lookup(
          unit,
          hash_tbl,
          hash_key,
          &data_indx,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  if (!found) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Unit %d Tcam DB id %d entry %d is expected to be in the hash table but is not existing.\n\r"), 
                  unit, tcam_db_id, entry_id));
       SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 7, exit);
  }

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.db_location_tbl.set(unit, data_indx, destination);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.action_bitmap_ndx.get(
          unit,
          tcam_db_id,
          &action_bitmap_ndx
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  
  if (set_hw_also) {
      res = arad_tcam_bank_entry_get_unsafe(
              unit,
              source->bank_id,
              source->entry,
              action_bitmap_ndx,
              TRUE ,
              &entry,
              &action,
              &hit_bit
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit); 

      
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.get(unit, tcam_db_id, source->bank_id, &bank_nof_entries);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 76, exit);
      bank_nof_entries--;
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.set(unit, tcam_db_id, source->bank_id, bank_nof_entries);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 77, exit);

      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.get(unit, tcam_db_id, destination->bank_id, &bank_nof_entries);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 78, exit);
      bank_nof_entries++;
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.set(unit, tcam_db_id, destination->bank_id, bank_nof_entries);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 79, exit);
  }

  
  res = arad_tcam_bank_entry_set_unsafe(
          unit,
          set_hw_also,
          is_bank_share_update,
          hit_bit,
          destination->bank_id,
          destination->entry,
          action_bitmap_ndx,
          &entry,
          &action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = arad_tcam_db_bank_occ_bitmap_entry_set(
          unit,
          global_location.tcam_db_id,
          destination,
          TRUE 
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 85, exit);

  
  res = arad_tcam_bank_entry_invalidate_unsafe(
          unit,
          set_hw_also,
          is_bank_share_update,
          FALSE,
          source->bank_id,
          source->entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = arad_tcam_db_bank_occ_bitmap_entry_set(
          unit,
          global_location.tcam_db_id,
          source,
          FALSE 
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 85, exit);

  
  if (!is_bank_share_update) {
      
      res = arad_tcam_db_priority_list_entry_remove(
              unit,
              tcam_db_id,
              source,
              entry_id,
              priority,
              TRUE 
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);

      res = arad_tcam_db_priority_list_entry_add(
              unit,
              tcam_db_id,
              destination,
              entry_id,
              priority,
              TRUE, 
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
      if (success != SOC_SAND_SUCCESS) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Unit %d Tcam DB id %d entry %d is expected to be added to the prio list but has failed.\n\r"), 
                      unit, tcam_db_id, entry_id));
           SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 11, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_move()", tcam_db_id, 0);
}




STATIC
  uint32
    arad_tcam_db_sw_db_after_move_update(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_TCAM_DIRECTION      direction,
      SOC_SAND_IN  uint32                   bank_id,
      SOC_SAND_IN  uint32                   nof_entries_shift,
      SOC_SAND_IN  uint32                   entry_from_min,
      SOC_SAND_IN  uint32                   entry_from_max
    )
{
  uint32
      line_ndx,
    res = SOC_SAND_OK;
  ARAD_TCAM_LOCATION
    range_start,
    range_end;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_TCAM_LOCATION_clear(&range_start);
  ARAD_TCAM_LOCATION_clear(&range_end);

  

  for (line_ndx = 0; line_ndx < (entry_from_max - entry_from_min + 1); line_ndx++)
  {
      range_start.bank_id = bank_id;
      range_start.entry = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? (entry_from_min + line_ndx): (entry_from_max - line_ndx);
      range_end.bank_id = bank_id;
      range_end.entry = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? (range_start.entry - nof_entries_shift): (range_start.entry + nof_entries_shift);

    
    res = arad_tcam_db_entry_move(
            unit,
            FALSE, 
            TRUE, 
            &range_start,
            &range_end
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }
              
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_sw_db_after_move_update()", 0, 0);
}


STATIC uint32
arad_pp_ihb_tcam_action_tbl_cache_after_move_update(
      SOC_SAND_IN   int                              unit,
      SOC_SAND_IN   uint32                           bank_ndx,
      SOC_SAND_IN   uint32                           tcam_db_id,
      SOC_SAND_IN   uint32                           move_couple,
      SOC_SAND_IN   uint32                           nof_entries_in_line,
      SOC_SAND_IN   uint32                           source_entry_start_ndx,
      SOC_SAND_IN   uint32                           source_entry_end_ndx,
      SOC_SAND_IN   uint32                           dest_entry_start_ndx
)
    {
    uint32
    range,
    src_arr_index,
    dest_arr_index,
    action_bit,
    res;
    soc_mem_t mem_tbl;

    ARAD_TCAM_ACTION_SIZE action_tbl_ndx;
    ARAD_TCAM_ACTION_SIZE action_bitmap_ndx;
    uint32
       src_line[ARAD_TCAM_NOF_ACTION_SIZES]         = {0,0,0,0},
       src_action_tbl[ARAD_TCAM_NOF_ACTION_SIZES]   = {0,0,0,0},
       dest_line[ARAD_TCAM_NOF_ACTION_SIZES]        = {0,0,0,0},
       dest_action_tbl[ARAD_TCAM_NOF_ACTION_SIZES]  = {0,0,0,0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_TBL_MOVE_UNSAFE);

    LOG_VERBOSE(BSL_LS_SOC_MEM,
           (BSL_META_U(unit,
                       SOC_DPP_MSG("      "
                       "TCAM-CACHE-Block-move: bank:%d, entry-source-start:%d, entry-source-end:%d, "
                       "entry-dest-start:%d, is-320b: %d, nof_entries_in_line: %d\n\r")),
            bank_ndx, source_entry_start_ndx, source_entry_end_ndx,
            dest_entry_start_ndx, move_couple, nof_entries_in_line));

    
    if (nof_entries_in_line == 0 || nof_entries_in_line > 2)
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 10, exit);
    }
    
    if (source_entry_start_ndx >= nof_entries_in_line * ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx))
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 20, exit);
    }
    if (dest_entry_start_ndx >= nof_entries_in_line * ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx))
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 20, exit);
    }

    if (nof_entries_in_line == 1)
        range = source_entry_end_ndx - source_entry_start_ndx + 1;
    else
        range = (source_entry_end_ndx - source_entry_start_ndx + 2) / 2;

    
    action_bitmap_ndx = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS | ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;
    if (move_couple) {
        action_bitmap_ndx |= ARAD_TCAM_ACTION_SIZE_THIRD_20_BITS | ARAD_TCAM_ACTION_SIZE_FORTH_20_BITS;        
    }

    res = arad_tcam_tbl_calc_action_banks_and_lines(unit, bank_ndx, source_entry_start_ndx, &action_bitmap_ndx, src_line, src_action_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    res = arad_tcam_tbl_calc_action_banks_and_lines(unit, bank_ndx, dest_entry_start_ndx, &action_bitmap_ndx, dest_line, dest_action_tbl);
        SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);

        for (action_tbl_ndx = 0; action_tbl_ndx < ARAD_TCAM_NOF_ACTION_SIZES; action_tbl_ndx++)
        {
            action_bit = (1 << action_tbl_ndx);

            if (action_bitmap_ndx & action_bit)
            {
                res = arad_pp_ihb_tcam_action_mem_and_index_get(unit, src_action_tbl[action_tbl_ndx], FALSE, &mem_tbl, &src_arr_index);
                SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
                res = arad_pp_ihb_tcam_action_mem_and_index_get(unit, dest_action_tbl[action_tbl_ndx], FALSE, &mem_tbl, &dest_arr_index);
                SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

                res = soc_mem_cache_block_move(unit, 0x0, mem_tbl, src_arr_index, dest_arr_index, MEM_BLOCK_ALL, src_line[action_tbl_ndx], dest_line[action_tbl_ndx], range);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 57, exit);
            }
        }
    

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_cache_after_move_update()", bank_ndx, source_entry_start_ndx);
}





STATIC uint32 arad_pp_ihb_tcam_tbl_cache_after_move_update(
      SOC_SAND_IN   int                              unit,
      SOC_SAND_IN   uint32                           bank_ndx,
      SOC_SAND_IN   uint32                           move_couple,
      SOC_SAND_IN   uint32                           nof_entries_in_line,
      SOC_SAND_IN   uint32                           source_entry_start_ndx,
      SOC_SAND_IN   uint32                           source_entry_end_ndx,
      SOC_SAND_IN   uint32                           dest_entry_start_ndx
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_TBL_MOVE_UNSAFE);

    LOG_VERBOSE(BSL_LS_SOC_TCAM,
           (BSL_META_U(unit,
                       SOC_DPP_MSG("      "
                       "TCAM-CACHE-Block-move: bank:%d, entry-source-start:%d, entry-source-end:%d, "
                       "entry-dest-start:%d, is-320b: %d, nof_entries_in_line: %d\n\r")),
            bank_ndx, source_entry_start_ndx, source_entry_end_ndx,
            dest_entry_start_ndx, move_couple, nof_entries_in_line));

    
    if (nof_entries_in_line != 1 && nof_entries_in_line != 2)
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 10, exit);
    }
    
    if (source_entry_start_ndx >= nof_entries_in_line * ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx))
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 20, exit);
    }
    if (dest_entry_start_ndx >= nof_entries_in_line * ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx))
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_tbl_cache_after_move_update()", bank_ndx, source_entry_start_ndx);
}




STATIC
  uint32
    arad_tcam_db_last_entry_previous_bank_move(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                   tcam_db_id,
      SOC_SAND_IN  ARAD_TCAM_RANGE          *range,
      SOC_SAND_IN  ARAD_TCAM_LOCATION       *empty_location,
      SOC_SAND_IN  ARAD_TCAM_DIRECTION      direction,
      SOC_SAND_IN  uint32                   nof_entries_per_line,
      SOC_SAND_IN  uint32                   nof_entries_max_in_bank,
      SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE  entry_size,
      SOC_SAND_INOUT ARAD_TCAM_LOCATION     *final_location, 
      SOC_SAND_INOUT  ARAD_TCAM_LOCATION    *empty
    )
{
  uint32
      res = SOC_SAND_OK,
      entry_id,
    next_priority;
  ARAD_TCAM_LOCATION
    next_location,
      empty_lcl;
  uint8
      found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_TCAM_LOCATION_clear(&next_location);
  ARAD_TCAM_LOCATION_clear(&empty_lcl);

  
  res = arad_tcam_db_occ_bmp_entry_closest_get(
            unit,
            tcam_db_id,
            (direction == ARAD_TCAM_DIRECTION_FORWARD)? ARAD_TCAM_DIRECTION_BACKWARD: ARAD_TCAM_DIRECTION_FORWARD,
            empty_location,
            &entry_id,
            &next_priority,
            &next_location,
            &found
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (found == FALSE)
  {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "     "
                             "Error in previous bank - no entry found,"
                             "for DB %d, bank %d and line %d and direction %d.\n\r"), 
                  tcam_db_id, empty_location->bank_id, empty_location->entry, direction));
     SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DATABASE_ALREADY_EXISTS_ERR, 10, exit);

  }

  
  if ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS) 
       && (empty->entry != ARAD_TCAM_ENTRY_LINE_PAIR_CLOSEST_ID(empty->entry, direction))){
      
      empty->entry = ARAD_TCAM_ENTRY_LINE_PAIR_80B(empty->entry);
  }
  res = arad_tcam_db_entry_move(
          unit,
          TRUE, 
          FALSE, 
          &next_location,
          empty
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  

  if ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS)
      && (next_location.entry == ARAD_TCAM_ENTRY_LINE_PAIR_CLOSEST_ID(next_location.entry, direction))
      
      && (!((final_location->bank_id == next_location.bank_id) && (next_location.entry == final_location->entry)))){
      
      next_location.entry = ARAD_TCAM_ENTRY_LINE_PAIR_80B(next_location.entry);
      empty->entry = ARAD_TCAM_ENTRY_LINE_PAIR_80B(empty->entry);

      
      res = arad_tcam_db_entry_move(
              unit,
              TRUE, 
              FALSE, 
              &next_location,
              empty
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }
  *empty = next_location;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_last_entry_previous_bank_move()", tcam_db_id, 0);
}


STATIC
  uint32
    arad_tcam_db_entries_migrate(
      SOC_SAND_IN  int         unit,
      SOC_SAND_IN  uint32         tcam_db_id,
      SOC_SAND_IN  uint8          is_inserted_top,
      SOC_SAND_IN  ARAD_TCAM_RANGE        *range,
      SOC_SAND_IN  ARAD_TCAM_LOCATION  *empty_location_first, 
      SOC_SAND_IN  ARAD_TCAM_LOCATION  *empty_location, 
      SOC_SAND_IN  uint32          target_priority,
      SOC_SAND_IN  ARAD_TCAM_DIRECTION direction,
      SOC_SAND_OUT ARAD_TCAM_LOCATION  *free_location
    )
{
  uint32
      nof_entries_max_in_bank,
      nof_entries_per_line,
      nof_entries_shift = 0,
      entry_dest_min,
      entry_first,
      line_free_first,
      entry_from_min,
      entry_from_max,
      next_entry,
      entry_stop,
      bank_id,
      bank_ndx,
      nof_banks,
    res = SOC_SAND_OK;
  SOC_SAND_SORTED_LIST_PTR
    prio_list;
  SOC_SAND_SORTED_LIST_ITER
    next;
  uint8
      move_per_block = FALSE,
      move_block_not_needed,
      has_not_finished,
      key_buffer[ARAD_TCAM_DB_LIST_KEY_SIZE],
      data_buffer[ARAD_TCAM_DB_LIST_DATA_SIZE],
      found,
      has_direct_table,
      insertion_rule_respected;
  uint32
    next_priority;
  ARAD_TCAM_LOCATION
    current_location,
    next_location,
    empty,
    empty_current,
    range_start;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;
  ARAD_TCAM_PRIO_LOCATION 
      prio_location;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_TCAM_LOCATION_clear(&current_location);
  ARAD_TCAM_LOCATION_clear(&next_location);
  ARAD_TCAM_LOCATION_clear(&empty);
  ARAD_TCAM_LOCATION_clear(&range_start);
  ARAD_TCAM_LOCATION_clear(free_location);

  empty = *empty_location;

  
  
  ARAD_TCAM_LOCATION_clear(free_location);
  free_location->bank_id = (direction == ARAD_TCAM_DIRECTION_BACKWARD)?
      range->min.bank_id: range->max.bank_id;
  free_location->entry = (direction == ARAD_TCAM_DIRECTION_BACKWARD)?
      range->min.entry: range->max.entry;
  nof_banks = (direction == ARAD_TCAM_DIRECTION_BACKWARD)?
      (range->min.bank_id - empty_location->bank_id + 1) :(empty_location->bank_id - range->max.bank_id + 1);
  for (bank_ndx = 0; bank_ndx < nof_banks; bank_ndx++) {
      bank_id = empty.bank_id; 
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.entry_size.get(unit, bank_id, &entry_size);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
      nof_entries_per_line = (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS)? 2: 1;
      entry_first = (bank_ndx == 0)? empty_location_first->entry : empty.entry;
      line_free_first = ARAD_TCAM_ENTRY_LINE_GET(entry_first, entry_size);
      if (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS) {
          line_free_first = ARAD_TCAM_ENTRY_LINE_GET(((direction == ARAD_TCAM_DIRECTION_BACKWARD)? (entry_first - 1) : (entry_first + 1)), entry_size);
      }
      move_block_not_needed = FALSE;
      nof_entries_max_in_bank = arad_tcam_bank_entry_size_to_entry_count_get(unit, entry_size, bank_id);
      if ((line_free_first * nof_entries_per_line) == ((direction == ARAD_TCAM_DIRECTION_BACKWARD)? (nof_entries_max_in_bank-1): 0)) {
          move_block_not_needed = TRUE;
      }
      entry_first = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? (entry_first + 1): (entry_first - 1);

       

      
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.has_direct_table.get(unit, bank_id, &has_direct_table);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      if(has_direct_table) {
        move_per_block = TRUE;
      } else {
        res = arad_tcam_db_use_new_method_per_db_get(unit, tcam_db_id, &move_per_block);
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
      }

      
      if ((!move_block_not_needed) || (!move_per_block)) {
          
          if (bank_id == free_location->bank_id) {
              entry_from_min = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? entry_first: range->max.entry;
              entry_from_max = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? range->min.entry: entry_first;
              entry_stop = ((direction == ARAD_TCAM_DIRECTION_BACKWARD)? entry_from_max: entry_from_min);
              if (entry_stop == empty.entry) {
                  LOG_DEBUG(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                        "Unit %d Tcam DB id %d - no need to move entries since empty location fits for the entry to insert.\n\r"),
                             unit, tcam_db_id));
                  ARAD_DO_NOTHING_AND_EXIT;
              }
              if ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS) && ((entry_from_min % 2) == 1)) {
                  entry_from_min = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? (entry_from_min - 1): (entry_from_min + 1);
              }
              if ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS) && ((entry_from_max % 2) == 0))  {
                  entry_from_max = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? (entry_from_max - 1): (entry_from_max + 1);
              }
              if (entry_from_min > entry_from_max)
              {
                move_block_not_needed = TRUE;
              }

          }
          else {
              
              entry_from_min = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? entry_first: 0;
              entry_from_max = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? (nof_entries_max_in_bank - 1): entry_first;
          }

        nof_entries_shift = 0;
        if ((!move_block_not_needed) || (!move_per_block))
        {
            
            nof_entries_shift = nof_entries_per_line * 
                ARAD_TCAM_ENTRY_NOF_LINES_GET(empty.entry, 
                  ((direction == ARAD_TCAM_DIRECTION_BACKWARD)? entry_from_min : entry_from_max), entry_size);
            entry_dest_min = (direction == ARAD_TCAM_DIRECTION_BACKWARD)?
                    (entry_from_min - nof_entries_shift): (entry_from_min + nof_entries_shift);


          

          
          if (!move_per_block) {
              

              
              res = arad_tcam_db_occ_bmp_entry_closest_get(
                        unit,
                        tcam_db_id,
                        (direction == ARAD_TCAM_DIRECTION_FORWARD)? ARAD_TCAM_DIRECTION_BACKWARD: ARAD_TCAM_DIRECTION_FORWARD,
                        empty_location,
                        &next_entry,
                        &next_priority,
                        &next_location,
                        &found
                      );
              SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

              if (found) {
                  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_priorities.get(unit, tcam_db_id, &prio_list);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
                  res = arad_tcam_db_priority_list_entry_iter_get(
                            unit,
                            tcam_db_id,
                            next_priority,
                            &next,
                            &found
                          );
                  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
                  if (!found) {
                       LOG_ERROR(BSL_LS_SOC_TCAM,
                                 (BSL_META_U(unit,
                                             "Unit %d Tcam DB id %d entry %d exists in occupation bitmap but not in priority %d table.\n\r"), 
                                  unit, tcam_db_id, next_entry, next_priority));
                       SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 7, exit);
                  }

                  
                  res = arad_tcam_db_block_entry_insertion_rule_respected_get(unit, tcam_db_id, &insertion_rule_respected);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
                  if (!insertion_rule_respected) {
                      has_not_finished = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? 
                                            (next_priority < target_priority): (next_priority > target_priority);
                  } else {
                      has_not_finished = (direction == ARAD_TCAM_DIRECTION_BACKWARD)?
                          (next_priority <= target_priority): (next_priority > target_priority);
                  }

                  while (has_not_finished)
                  {
                    
                      res = arad_tcam_db_block_entry_insertion_rule_respected_get(unit, tcam_db_id, &insertion_rule_respected);
                      SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);
                      if (!insertion_rule_respected) {
                          
                          res = soc_sand_sorted_list_entry_value(
                                 unit,
                                 prio_list,
                                 next,
                                 key_buffer,
                                 data_buffer
                               );
                         SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
                         
                         if (next_priority != arad_tcam_db_prio_list_priority_value_decode(key_buffer)) {
                              LOG_ERROR(BSL_LS_SOC_TCAM,
                                        (BSL_META_U(unit,
                                                    "Unit %d Tcam DB id %d has priority %d but the matched iter %d has different priority %d.\n\r"),
                                         unit, tcam_db_id, next_priority, next, arad_tcam_db_prio_list_priority_value_decode(key_buffer)));
                              SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 50, exit);
                         }

                         
                         res = arad_tcam_db_priority_data_decode(unit, tcam_db_id, data_buffer, &prio_location);
                         SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
                         res = arad_tcam_db_entry_id_to_location_entry_get(
                                 unit,
                                 tcam_db_id,
                                 ((direction == ARAD_TCAM_DIRECTION_BACKWARD)? prio_location.entry_id_last: prio_location.entry_id_first),
                                 TRUE,
                                 &range_start,
                                 &found
                               );
                         SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
                         
                         if (!found) {
                              LOG_ERROR(BSL_LS_SOC_TCAM,
                                        (BSL_META_U(unit,
                                                    "Unit %d Tcam DB id %d entry not existing.\n\r"), 
                                         unit, tcam_db_id));
                              SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 57, exit);
                         }

                         
                         if (direction == ARAD_TCAM_DIRECTION_BACKWARD)
                         {
                           res = soc_sand_sorted_list_get_next(
                                   unit,
                                   prio_list,
                                   &next,
                                   key_buffer,
                                   data_buffer
                                 );
                           SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                         }
                         else
                         {
                           res = soc_sand_sorted_list_get_prev(
                                   unit,
                                   prio_list,
                                   &next,
                                   key_buffer,
                                   data_buffer
                                 );
                           SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
                         }
                         next_priority = arad_tcam_db_prio_list_priority_value_decode(key_buffer);
                         has_not_finished = (direction == ARAD_TCAM_DIRECTION_BACKWARD) ?
                                    ((next != SOC_SAND_SORTED_LIST_ITER_END(unit,prio_list)) && (next_priority < target_priority))
                                    : ((next != SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,prio_list)) && (next_priority > target_priority));
                      }
                      else {
                          
                          range_start = next_location;

                          
                          res = arad_tcam_db_occ_bmp_entry_closest_get(
                                    unit,
                                    tcam_db_id,
                                    (direction == ARAD_TCAM_DIRECTION_FORWARD)? ARAD_TCAM_DIRECTION_BACKWARD: ARAD_TCAM_DIRECTION_FORWARD,
                                    &range_start,
                                    &next_entry,
                                    &next_priority,
                                    &next_location,
                                    &found
                                  );
                          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
                          has_not_finished = found && ((direction == ARAD_TCAM_DIRECTION_BACKWARD)?
                                                       (next_priority <= target_priority): (next_priority > target_priority));
                      }

                      
                      res = arad_tcam_db_entry_move(
                            unit,
                            TRUE, 
                            FALSE, 
                            &range_start,
                            &empty
                          );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

                    empty = range_start;
                  } 
                  *free_location = empty;
              }
              else {
                  
                  *free_location = *empty_location;
              }

              
              break;
          }
          else {
              
              res = arad_pp_ihb_tcam_tbl_move_unsafe(
                    unit,
                    bank_id,
                    (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS), 
                    nof_entries_per_line, 
                    entry_from_min,
                    entry_from_max, 
                    entry_dest_min
              );
              SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

              
              res = arad_pp_ihb_tcam_action_tbl_cache_after_move_update(
                      unit,
                      bank_id,
                      tcam_db_id,
                      (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS), 
                      nof_entries_per_line,
                      entry_from_min,
                      entry_from_max,
                      entry_dest_min
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);


              
              res = arad_pp_ihb_tcam_tbl_cache_after_move_update(
                      unit,
                      bank_id,
                      (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS), 
                      nof_entries_per_line,
                      entry_from_min,
                      entry_from_max,
                      entry_dest_min
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);

              
              res = arad_tcam_db_sw_db_after_move_update(
                        unit,
                        direction,
                        bank_id,
                        nof_entries_shift,
                        entry_from_min,
                        entry_from_max
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 122, exit);

              
              empty.entry = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? 
                  (entry_from_max + 1 - nof_entries_shift): (entry_from_min + nof_entries_shift - 1);
          } 
        } 
      } 

      
      if (move_per_block) {
          if (bank_id != free_location->bank_id) {
              
              res = arad_tcam_db_last_entry_previous_bank_move(
                        unit,
                        tcam_db_id,
                        range,
                        &empty,
                        direction,
                        nof_entries_per_line,
                        nof_entries_max_in_bank,
                        entry_size,
                        free_location,
                        &empty
                      );
              SOC_SAND_CHECK_FUNC_RESULT(res, 124, exit);
          } 
          else {
              
              entry_stop = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? range->min.entry: range->max.entry;
              empty_current = empty;
              empty.entry = entry_stop;

              if ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS)
                   && (entry_stop == ARAD_TCAM_ENTRY_LINE_PAIR_CLOSEST_ID(entry_stop, direction))) {
                  
                  res = arad_tcam_db_entry_move(
                          unit,
                          TRUE, 
                          FALSE, 
                          &empty,
                          &empty_current
                        );
                  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
                  empty_current.entry = (direction == ARAD_TCAM_DIRECTION_BACKWARD)? (empty_current.entry + 1): (empty_current.entry - 1);
              }

              
              free_location->entry = empty.entry;
        #if ARAD_TCAM_INSERTION_ALGORITHM_MIDDLE_FREE
              if (!is_inserted_top) {
                  free_location->entry = ARAD_TCAM_INSERTION_ALGORITHM_MIDDLE_FREE_FIND(0, direction, 0, empty.entry, empty_current.entry);
              }
        #endif 

              
              break;
          }
      } 
  } 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entries_migrate()", tcam_db_id, target_priority);
}

STATIC
 uint32
    arad_tcam_db_free_location_allocate(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                   tcam_db_id,
      SOC_SAND_IN  uint8                    is_inserted_top,
      SOC_SAND_IN  uint32                   priority,
      SOC_SAND_IN  uint8                    is_for_update,
      SOC_SAND_IN  uint8                    is_keep_location,
      SOC_SAND_IN  uint32                   entry_id,
      SOC_SAND_OUT uint8                    *is_update_and_invalid,
      SOC_SAND_OUT ARAD_TCAM_LOCATION       *location,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success
    )
{
  uint32
    access_profile_id,
    res = SOC_SAND_OK;
  ARAD_TCAM_RANGE
    range;
  ARAD_TCAM_LOCATION
      free_location_first,
    free_location;
  ARAD_TCAM_DIRECTION
    direction=0;
  uint8
      hit_bit,
      found;
  ARAD_TCAM_ACTION_SIZE     
      action_bitmap_ndx = 0;
  ARAD_TCAM_ENTRY     
      entry;
  ARAD_TCAM_ACTION 
      action;
  ARAD_TCAM_BANK_OWNER
      db_bank_owner;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_TCAM_RANGE_clear(&range);
  ARAD_TCAM_LOCATION_clear(location);
  ARAD_TCAM_LOCATION_clear(&free_location);
  ARAD_TCAM_LOCATION_clear(&free_location_first);

  *is_update_and_invalid = FALSE;
  
  if (is_for_update)
  {
      res = arad_tcam_db_entry_id_to_location_entry_get(
              unit,
              tcam_db_id,
              entry_id,
              TRUE,
              location,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

      
      if (!found) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Unit %d Tcam DB id %d entry %d is expected to be updaed but is not existing.\n\r"), 
                      unit, tcam_db_id, entry_id));
           SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 7, exit);
      }

      
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, tcam_db_id, 0, &access_profile_id);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.bank_owner.get(
              unit,
              access_profile_id,
              &db_bank_owner
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);

      if (found 
          && ((db_bank_owner == ARAD_TCAM_BANK_OWNER_PMF_0) 
              || (db_bank_owner == ARAD_TCAM_BANK_OWNER_PMF_1) 
              || (db_bank_owner == ARAD_TCAM_BANK_OWNER_EGRESS_ACL)))
      {
        
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.action_bitmap_ndx.get(
                  unit,
                  tcam_db_id,
                  &action_bitmap_ndx
                );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);

        res = arad_tcam_bank_entry_get_unsafe(
                unit,
                location->bank_id,
                location->entry,
                action_bitmap_ndx,
                FALSE ,
                &entry,
                &action,
                &hit_bit
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
        if (!entry.valid)
        {
            
            *success = SOC_SAND_SUCCESS;
            *is_update_and_invalid = TRUE;
            ARAD_DO_NOTHING_AND_EXIT;
        }
      }
  } else if (is_keep_location) {
      res = arad_tcam_db_entry_id_to_location_entry_get(
              unit,
              tcam_db_id,
              entry_id,
              TRUE,
              location,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

      
      if (!found) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Unit %d Tcam DB id %d entry %d is expected to be keep but is not existing.\n\r"), 
                      unit, tcam_db_id, entry_id));
           SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 7, exit);
      }

      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, tcam_db_id, 0, &access_profile_id);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.bank_owner.get(
              unit,
              access_profile_id,
              &db_bank_owner
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);

      if (found 
          && ((db_bank_owner == ARAD_TCAM_BANK_OWNER_PMF_0) 
              || (db_bank_owner == ARAD_TCAM_BANK_OWNER_PMF_1) 
              || (db_bank_owner == ARAD_TCAM_BANK_OWNER_EGRESS_ACL)))
      {
 
            *success = SOC_SAND_SUCCESS;
            *is_update_and_invalid = FALSE;
            ARAD_DO_NOTHING_AND_EXIT; 
      }
  }

    

    res = arad_tcam_db_priority_list_insertion_range_find(
              unit,
              tcam_db_id,
              priority,
              is_for_update,
              entry_id,
              &range
            );

    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    

    res = arad_tcam_db_empty_location_in_range_find(
              unit,
              tcam_db_id,
              is_inserted_top,
              &range,
              location,
              success
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (*success != SOC_SAND_SUCCESS)
  {
    
    res = arad_tcam_closest_free_location_to_range_find(
            unit,
            tcam_db_id,
            is_inserted_top,
            &range,
            &free_location_first,
            &free_location,
            &direction,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (*success == SOC_SAND_SUCCESS)
    {
      res = arad_tcam_db_entries_migrate(
              unit,
              tcam_db_id,
              is_inserted_top,
              &range,
              &free_location_first,
              &free_location,
              priority,
              direction,
              location
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_free_location_allocate()", tcam_db_id, priority);
}

STATIC
  uint32
    arad_tcam_db_entry_exists_get(
      SOC_SAND_IN  int unit,
      SOC_SAND_IN  uint32 tcam_db_id,
      SOC_SAND_IN  uint32 entry_id,
      SOC_SAND_OUT uint8 *found
    )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  SOC_SAND_HASH_TABLE_PTR
    hash_tbl;
  uint8
    key_buffer[ARAD_TCAM_DB_HASH_TBL_KEY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.entry_id_to_location.get(unit, &hash_tbl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  arad_tcam_db_entry_id_encode(tcam_db_id, entry_id, key_buffer);
  res = soc_sand_hash_table_entry_lookup(
          unit,
          hash_tbl,
          key_buffer,
          &data_indx,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_exists_get()", tcam_db_id, entry_id);
}




uint32
  arad_tcam_bank_owner_get_unsafe(
    SOC_SAND_IN    int               unit,
    SOC_SAND_IN    uint32               bank_id,
    SOC_SAND_OUT   ARAD_TCAM_BANK_OWNER *bank_owner
  )
{
  uint32
      res = SOC_SAND_OK,
      tcam_db_ndx,
      access_profile_id,
      access_array_ndx;
  uint8
      is_used = FALSE;
  ARAD_TCAM_BANK_ENTRY_SIZE
      entry_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  *bank_owner = ARAD_TCAM_NOF_BANK_OWNERS;

  
  for (tcam_db_ndx = 0; tcam_db_ndx < ARAD_TCAM_MAX_NOF_LISTS; tcam_db_ndx++)
  {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(unit, tcam_db_ndx, bank_id, &is_used);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

      if(is_used)
      {
          
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit, tcam_db_ndx, &entry_size);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

          
          access_array_ndx = (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) ? (bank_id % 2) : 0;

          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, tcam_db_ndx, access_array_ndx, &access_profile_id);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.bank_owner.get(unit, access_profile_id, bank_owner);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_bank_owner_get_unsafe()", bank_id, 0);
}

uint32
    arad_tcam_bank_owner_set_unsafe(
      SOC_SAND_IN    int               unit,
      SOC_SAND_IN    uint32               bank_id,
      SOC_SAND_IN   ARAD_TCAM_BANK_OWNER  bank_owner
    )
{
  uint32
    res,
    field_val,
    start_bit;
  ARAD_TCAM_HW_BANK_OWNER
    hw_bank_owner;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_tcam_bank_owner_verify(
          unit,
          bank_id,
          bank_owner
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

  switch (bank_owner) 
  {
  
  case ARAD_TCAM_BANK_OWNER_PMF_0:
  case ARAD_TCAM_BANK_OWNER_PMF_1:
      hw_bank_owner = ARAD_TCAM_HW_BANK_OWNER_PMF;
      break;
  
  case ARAD_TCAM_BANK_OWNER_FLP_TCAM:
  case ARAD_TCAM_BANK_OWNER_FLP_TRAPS:
      hw_bank_owner = ARAD_TCAM_HW_BANK_OWNER_FLP;
      break;
  
  case ARAD_TCAM_BANK_OWNER_VT:
  case ARAD_TCAM_BANK_OWNER_TT:
      hw_bank_owner = ARAD_TCAM_HW_BANK_OWNER_VTT;
      break;
  
  case ARAD_TCAM_BANK_OWNER_EGRESS_ACL:
      hw_bank_owner = ARAD_TCAM_HW_BANK_OWNER_ERPP;
      break;
  default:
      
      SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_MGMT_BANK_OWNER_OUT_OF_RANGE_ERR, 10, exit);

  }


  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_A_TCAM_BANK_BLOCK_OWNERr, REG_PORT_ANY, 0, TCAM_BANK_BLOCK_OWNERf, &field_val));

  start_bit = bank_id * ARAD_TCAM_NOF_BANK_OWNERS_IN_BITS;
  res = soc_sand_bitstream_set_any_field(&hw_bank_owner, start_bit, ARAD_TCAM_NOF_BANK_OWNERS_IN_BITS, &field_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_A_TCAM_BANK_BLOCK_OWNERr, REG_PORT_ANY, 0, TCAM_BANK_BLOCK_OWNERf,  field_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_bank_owner_set_unsafe()", bank_id, bank_owner);
}

uint32
    arad_tcam_bank_owner_verify(
      SOC_SAND_IN    int               unit,
      SOC_SAND_IN    uint32               bank_id,
      SOC_SAND_OUT   ARAD_TCAM_BANK_OWNER bank_owner
    )
{  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(bank_owner, ARAD_TCAM_BANK_OWNER_MAX, ARAD_TCAM_MGMT_BANK_OWNER_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_bank_owner_verify()", bank_id, 0);
}



uint32
  arad_tcam_bank_init_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  uint8                is_direct,
    SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
    SOC_SAND_IN  ARAD_TCAM_BANK_OWNER bank_owner
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    valid,
    create_bitmap = FALSE;
  ARAD_TCAM_BANK_ENTRY_SIZE
    old_entry_size;
  uint32
    nof_entries;
  SOC_SAND_OCC_BM_PTR
    occ_bm[2];
  SOC_SAND_OCC_BM_INIT_INFO
    occ_bm_init_info;
  uint8
      is_inverse;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_BANK_INIT_UNSAFE);

  
  res = arad_tcam_bank_init_verify(
          unit,
          bank_id,
          is_direct,
          entry_size,
          bank_owner
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  res = arad_pp_ihb_tcam_tbl_flush_unsafe(
          unit,
          bank_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  res = arad_tcam_bank_owner_set_unsafe(
          unit,
          bank_id,
          bank_owner
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);


  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.valid.get(
          unit,
          bank_id,
          &valid
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
  if (!valid)
  {
    create_bitmap = TRUE;
  }
  else
  {
    
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.entry_size.get(
            unit,
            bank_id,
            &old_entry_size
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 17, exit);
    for (is_inverse = 0; is_inverse < ARAD_TCAM_NOF_OCCUPATIONS_BITMAPS; is_inverse++) {
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_bank_entries_used.get(
                           unit,
                           bank_id,
                           is_inverse,
                           &occ_bm[is_inverse]
                         );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

        if (!is_direct
            && entry_size == old_entry_size)
        {
          
          soc_sand_occ_bm_clear(unit, occ_bm[is_inverse]);
        }
        else
        {
          soc_sand_occ_bm_destroy(unit, occ_bm[is_inverse]);
          create_bitmap = TRUE;
        }
    }
  }
  
  
  nof_entries = arad_tcam_bank_entry_size_to_entry_count_get(
                    unit,
                    is_direct ? ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS : entry_size, 
                    bank_id);
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.nof_entries_free.set(unit, bank_id, nof_entries);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);

  if (create_bitmap)
  {
    
    for (is_inverse = 0; is_inverse < ARAD_TCAM_NOF_OCCUPATIONS_BITMAPS; is_inverse++) {
        soc_sand_SAND_OCC_BM_INIT_INFO_clear(&occ_bm_init_info);
        occ_bm_init_info.size     = nof_entries;
        occ_bm_init_info.init_val = is_inverse? TRUE:FALSE;

        res = soc_sand_occ_bm_create(
                unit,
                &occ_bm_init_info,
                &occ_bm[is_inverse]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_bank_entries_used.set(unit, bank_id, is_inverse, occ_bm[is_inverse]);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);
    }

    
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.entry_size.set(unit, bank_id, entry_size);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.valid.set(
    unit,
    bank_id,
    TRUE
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_bank_init_unsafe()", bank_id, 0);
}

uint32
  arad_tcam_bank_init_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  uint8                is_direct,
    SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
    SOC_SAND_IN  ARAD_TCAM_BANK_OWNER bank_owner
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_BANK_INIT_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(bank_id, SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit), ARAD_TCAM_BANK_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_size, ARAD_TCAM_ENTRY_SIZE_MAX+(is_direct?1:0), ARAD_TCAM_ENTRY_SIZE_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bank_owner, ARAD_TCAM_BANK_OWNER_MAX, ARAD_TCAM_MGMT_BANK_OWNER_OUT_OF_RANGE_ERR, 40, exit);

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_bank_init_verify()", 0, 0);
}


uint32
  arad_tcam_db_create_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    tcam_db_id,
    SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
    SOC_SAND_IN  uint32                    prefix_size,
    SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE     action_bitmap_ndx,
    SOC_SAND_IN  uint8                     use_small_banks,
    SOC_SAND_IN  uint8                     no_insertion_priority_order,
    SOC_SAND_IN  uint8                     sparse_priorities,
    SOC_SAND_IN  uint8                     is_direct
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_count_per_bank,
    max_nof_entries,
    bank_id;
  uint8
    valid;
  SOC_SAND_SORTED_LIST_PTR
    priorities;
  SOC_SAND_SORTED_LIST_INIT_INFO
    priorities_init_info;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_CREATE_UNSAFE);

  
  res = arad_tcam_db_create_verify(
          unit,
          tcam_db_id,
          entry_size,
          prefix_size,
          action_bitmap_ndx,
          is_direct
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.get(
          unit,
          tcam_db_id,
          &valid
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);
  if (valid)
  {
      LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                            "     "
                            "Error in TCAM bank create: "
                            "the TCAM database %d already exists.\n\r"), tcam_db_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DATABASE_ALREADY_EXISTS_ERR, 10, exit);
  }

  
  entry_count_per_bank = arad_tcam_bank_entry_size_to_entry_count_get(
                            unit,
                            is_direct ? ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS : entry_size, 
                            0);
  max_nof_entries      = entry_count_per_bank * SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit);

  
  soc_sand_os_memset(&priorities_init_info, 0x0, sizeof(SOC_SAND_SORTED_LIST_INIT_INFO));

  priorities_init_info.prime_handle  = unit;
  priorities_init_info.sec_handle    = 0;
  priorities_init_info.list_size     = max_nof_entries;
  priorities_init_info.key_size      = ARAD_TCAM_DB_LIST_KEY_SIZE * sizeof(uint8);
  priorities_init_info.data_size     = ARAD_TCAM_DB_LIST_DATA_SIZE * sizeof(uint8);
  priorities_init_info.get_entry_fun = NULL;
  priorities_init_info.set_entry_fun = NULL;
  priorities_init_info.cmp_func_type   = SOC_SAND_SORTED_LIST_CMP_FUNC_TYPE_TCAM;

  res = soc_sand_sorted_list_create(unit, &priorities, priorities_init_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_priorities.set(unit, tcam_db_id, priorities);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.set(
    unit,
    tcam_db_id,
    entry_size
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.action_bitmap_ndx.set(
    unit,
    tcam_db_id,
    action_bitmap_ndx
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.prefix_size.set(
    unit,
    tcam_db_id,
    prefix_size
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.is_direct.set(
    unit, 
    tcam_db_id, 
    is_direct
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.use_small_banks.set(
    unit, 
    tcam_db_id, 
    use_small_banks
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.no_insertion_priority_order.set(
    unit, 
    tcam_db_id, 
    no_insertion_priority_order
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 80, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.sparse_priorities.set(
    unit, 
    tcam_db_id, 
    sparse_priorities
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 90, exit);

  for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
  {
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.set(
      unit,
      tcam_db_id,
      bank_id,
      FALSE
    );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.set(
    unit,
    tcam_db_id,
    TRUE
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 110, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_create_unsafe()", tcam_db_id, 0);
}

uint32
  arad_tcam_db_create_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
    SOC_SAND_IN  uint32               prefix_size,
    SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE     action_bitmap_ndx,
    SOC_SAND_IN  uint8                     is_direct
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_CREATE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_TCAM_DB_ID_MAX, ARAD_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_size, ARAD_TCAM_ENTRY_SIZE_MAX+(is_direct?1:0), ARAD_TCAM_ENTRY_SIZE_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(prefix_size, ARAD_TCAM_PREFIX_SIZE_MAX, ARAD_TCAM_PREFIX_SIZE_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(action_bitmap_ndx, ARAD_TCAM_ACTION_SIZE_MAX, ARAD_TCAM_ACTION_SIZE_OUT_OF_RANGE_ERR, 50, exit);
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_create_verify()", tcam_db_id, 0);
}


uint32
  arad_tcam_db_destroy_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  )
{
  uint32
      next_priority,
      next_entry,
      tcam_db_other_ndx,
    res = SOC_SAND_OK;
  uint32
    bank_id;
  ARAD_TCAM_LOCATION
      location_curr,
    location;
  SOC_SAND_SORTED_LIST_PTR
    prio_list;
  uint8
      found,
      is_forbidden;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_DESTROY_UNSAFE);

  ARAD_TCAM_DB_EXISTS_ASSERT;

  
  res = arad_tcam_db_destroy_verify(
          unit,
          tcam_db_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  
  ARAD_TCAM_LOCATION_clear(&location_curr);
  location_curr.bank_id = 0;
  location_curr.entry = 0;

  do
  {
      res = arad_tcam_db_occ_bmp_entry_closest_get(
                unit,
                tcam_db_id,
                ARAD_TCAM_DIRECTION_FORWARD,
                &location_curr,
                &next_entry,
                &next_priority,
                &location,
                &found
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if(found)
    {
        res = arad_tcam_bank_entry_invalidate_unsafe(
                unit,
                TRUE, 
                FALSE, 
                FALSE,
                location.bank_id,
                location.entry
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      location_curr = location;
    }
  } while (found);

  for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
  {
      
      res = arad_tcam_db_bank_remove_unsafe(unit, tcam_db_id, bank_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);
  }

  
  for (tcam_db_other_ndx = 0; tcam_db_other_ndx < ARAD_TCAM_MAX_NOF_LISTS; tcam_db_other_ndx++) {
      res = arad_sw_db_tcam_db_forbidden_dbs_get(unit, tcam_db_other_ndx, tcam_db_id, &is_forbidden);
      SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
      if (is_forbidden) {
          
          res = arad_sw_db_tcam_db_forbidden_dbs_set(unit, tcam_db_other_ndx, tcam_db_id, FALSE );
          SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
      }
  }


  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_priorities.get(unit,tcam_db_id, &prio_list);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_sorted_list_destroy(unit, prio_list);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.set(unit, tcam_db_id, FALSE);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_destroy_unsafe()", tcam_db_id, 0);
}

uint32
  arad_tcam_db_destroy_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_DESTROY_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_TCAM_DB_ID_MAX, ARAD_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_destroy_verify()", tcam_db_id, 0);
}


uint32
  arad_tcam_db_bank_add_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             tcam_db_id,
    SOC_SAND_IN  uint32             access_profile_array_id, 
    SOC_SAND_IN  uint32             bank_id,
    SOC_SAND_IN  uint8              is_direct, 
    SOC_SAND_IN  ARAD_TCAM_PREFIX   *prefix
  )
{
  uint32
      nof_entries,
      access_profile_id,
    res = SOC_SAND_OK;
  uint8
    is_used,
    has_direct_table;
  ARAD_TCAM_BANK_ENTRY_SIZE
    db_entry_size,
    bank_entry_size;
  ARAD_TCAM_BANK_OWNER
    db_bank_owner,
    bank_owner;
  SOC_SAND_OCC_BM_PTR
    occ_bm;
  SOC_SAND_OCC_BM_INIT_INFO
    occ_bm_init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_BANK_ADD_UNSAFE);
  
  SOC_SAND_CHECK_NULL_INPUT(prefix);

  ARAD_TCAM_DB_EXISTS_ASSERT;

  
  res = arad_tcam_db_bank_add_verify(
          unit,
          tcam_db_id,
          bank_id,
          prefix
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(
          unit,
          tcam_db_id,
          bank_id,
          &is_used
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  if (is_used)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d Tcam DB id %d Bank id %d - Failed to add bank. The bank already exists.\n\r"),
               unit, tcam_db_id, bank_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_BANK_ALREADY_USED_ERR, 20, exit);
  }

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit, tcam_db_id, &db_entry_size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.entry_size.get(unit, bank_id, &bank_entry_size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

  if (db_entry_size != bank_entry_size 
      && !is_direct)
  {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.has_direct_table.get(unit, bank_id, &has_direct_table);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
      if(!has_direct_table) {
          LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                                "     "
                                "Error in TCAM bank create: "
                                "For TCAM database %d and bank %d, different sizes between bank %s and DB %s \n\r"), 
                     tcam_db_id, bank_id, SOC_TMC_TCAM_BANK_ENTRY_SIZE_to_string(bank_entry_size), 
                     SOC_TMC_TCAM_BANK_ENTRY_SIZE_to_string(db_entry_size)));
          LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                                "\n\r")));
          SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_ENTRY_SIZE_MISMATCH_ERR, 30, exit);
      }
  }

  res = arad_tcam_bank_owner_get_unsafe(unit, bank_id, &bank_owner);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit,tcam_db_id, access_profile_array_id, &access_profile_id);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.bank_owner.get(
          unit,
          access_profile_id,
          &db_bank_owner
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  
  if ((db_bank_owner != bank_owner) && (bank_owner != ARAD_TCAM_NOF_BANK_OWNERS))
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d Tcam DB id %d Bank id %d - Failed to add bank.\n\r"
                          "Owner mismatch : Data base owner %d, Bank owner %d\n\r"), unit, tcam_db_id, bank_id, db_bank_owner, bank_owner));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_BANK_OWNER_MISMATCH_ERR, 45, exit);
  }

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.set(
    unit,
    tcam_db_id,
    bank_id,
    TRUE
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  if(is_direct) {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.has_direct_table.set(
        unit, 
        bank_id, 
        is_direct
      );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.prefix.set(
    unit,
    tcam_db_id,
    prefix
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 21, exit);

   
  if (access_profile_array_id == 0) {
      
      nof_entries = arad_tcam_bank_entry_size_to_entry_count_get(
                        unit,
                        is_direct ? ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS : bank_entry_size, 
                        bank_id);

      
      soc_sand_SAND_OCC_BM_INIT_INFO_clear(&occ_bm_init_info);
      occ_bm_init_info.size     = nof_entries;
      occ_bm_init_info.init_val = TRUE;    

      res = soc_sand_occ_bm_create(unit, &occ_bm_init_info, &occ_bm);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_entries_used.set(unit, tcam_db_id, bank_id, occ_bm);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_bank_add_unsafe()", tcam_db_id, bank_id);
}

uint32
  arad_tcam_db_bank_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  ARAD_TCAM_PREFIX          *prefix
  )
{  
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_BANK_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_TCAM_DB_ID_MAX, ARAD_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(bank_id, SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit), ARAD_TCAM_BANK_ID_OUT_OF_RANGE_ERR, 20, exit);
  ARAD_STRUCT_VERIFY(ARAD_TCAM_PREFIX, prefix, 30, exit);
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_bank_add_verify()", 0, 0);
}


uint32
  arad_tcam_db_bank_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  )
{
  uint32
      nof_access_profiles,
      access_profile_array_id,
      access_profile_id,
      pd_profile_id,
      action_hw,
    res = SOC_SAND_OK;
  uint32
    shift,
    nof_dbs;
  uint8
    base,
    ndx;
  ARAD_TCAM_PREFIX
    prefix;
  SOC_SAND_OCC_BM_PTR
    occ_bm;
  uint8
    is_inverse,
    is_used,
    has_direct_table,
    is_direct;
  ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_DATA
    access_tbl_data;
  ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_DATA
    pd_tbl_data;
  uint32
    bitmap_lcl[1];
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_BANK_REMOVE_UNSAFE);

  ARAD_TCAM_DB_EXISTS_ASSERT;

  
  res = arad_tcam_db_bank_remove_verify(
          unit,
          tcam_db_id,
          bank_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(unit, tcam_db_id, bank_id, &is_used);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit); 
  if (is_used)
  {
    
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.prefix.get(unit, tcam_db_id, &prefix);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);

    
    shift = 4 - prefix.length;
    base = (uint8) (prefix.bits << shift);
    for (ndx = 0; ndx < SOC_SAND_BIT(shift); ++ndx)
    {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.banks.prefix_db.set(unit, bank_id, base | ndx, ARAD_TCAM_MAX_NOF_LISTS);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);
    }

      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.set(unit, tcam_db_id, bank_id, FALSE);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);

      
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.has_direct_table.get(unit, bank_id, &has_direct_table);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);
      if(has_direct_table)
      {
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.is_direct.get(unit, tcam_db_id, &is_direct);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);
          
          if(is_direct) {
              res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.has_direct_table.set(unit, bank_id, FALSE);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);
          }
      }

      
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.set(unit, tcam_db_id, bank_id, 0 );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);

      
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.banks.nof_dbs.get(unit, bank_id, &nof_dbs);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.banks.nof_dbs.set(unit, bank_id, --nof_dbs);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
      if (nof_dbs == 0)
      {
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.valid.set(unit, bank_id, FALSE);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 9, exit);

        for (is_inverse = 0; is_inverse < ARAD_TCAM_NOF_OCCUPATIONS_BITMAPS; is_inverse++) {

            res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_bank_entries_used.get(unit, bank_id, is_inverse, &occ_bm);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
            soc_sand_occ_bm_destroy(unit, occ_bm);

            SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
        }      
      }
  }

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit, tcam_db_id, &entry_size);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
  nof_access_profiles = (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS)? 2 : 1;
  for (access_profile_array_id = 0; access_profile_array_id < nof_access_profiles; access_profile_array_id++) {
      
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, tcam_db_id, access_profile_array_id, &access_profile_id);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 17, exit);

      res = arad_pp_ihb_tcam_access_profile_tbl_read_unsafe(
              unit,
              access_profile_id,
              &access_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      action_hw = 0;
      SHR_BITCOPY_RANGE(&access_tbl_data.action_bitmap, bank_id*ARAD_TCAM_ACTION_SIZE_NOF_BITS, &action_hw, 0, ARAD_TCAM_ACTION_SIZE_NOF_BITS);
      res = arad_pp_ihb_tcam_access_profile_tbl_write_unsafe(
              unit,
              access_profile_id,
              &access_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      
      pd_profile_id = access_profile_id; 
      res = arad_pp_ihb_tcam_pd_profile_tbl_read_unsafe(
              unit,
              pd_profile_id,
              &pd_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      *bitmap_lcl = pd_tbl_data.bitmap;
      SHR_BITCLR(bitmap_lcl, bank_id);
      pd_tbl_data.bitmap = *bitmap_lcl;
      res = arad_pp_ihb_tcam_pd_profile_tbl_write_unsafe(
              unit,
              pd_profile_id,
              &pd_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      if (access_profile_array_id == 0) {
          if (is_used) {
              res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_entries_used.get(unit, tcam_db_id, bank_id, &occ_bm);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
              if (occ_bm) {
                  
                  soc_sand_occ_bm_destroy(unit, occ_bm) ;
                  
                  occ_bm = (SOC_SAND_OCC_BM_PTR)PTR_TO_INT(NULL) ;
                  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db_entries_used.set(unit, tcam_db_id, bank_id, occ_bm);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
              }
          }
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_bank_remove_unsafe()", 0, 0);
}

uint32
  arad_tcam_db_bank_remove_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_BANK_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_TCAM_DB_ID_MAX, ARAD_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(bank_id, SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit), ARAD_TCAM_BANK_ID_OUT_OF_RANGE_ERR, 20, exit);

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_bank_remove_verify()", 0, 0);
}


uint32
  arad_tcam_db_nof_banks_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_OUT uint32               *nof_banks
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    bank_id;
  uint8
    is_used;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_NOF_BANKS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(nof_banks);

  
  res = arad_tcam_db_nof_banks_get_verify(
          unit,
          tcam_db_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ARAD_TCAM_DB_EXISTS_ASSERT;

  *nof_banks = 0;
  for (bank_id = 0; bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_id)
  {
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(unit, tcam_db_id, bank_id, &is_used);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    if (is_used)
    {
      ++*nof_banks;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_nof_banks_get_unsafe()", 0, 0);
}

uint32
  arad_tcam_db_nof_banks_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_NOF_BANKS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_TCAM_DB_ID_MAX, ARAD_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_nof_banks_get_verify()", 0, 0);
}


uint32
  arad_tcam_db_bank_prefix_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_OUT ARAD_TCAM_PREFIX          *prefix
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    has_bank;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_BANK_PREFIX_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prefix);

  
  res = arad_tcam_db_bank_prefix_get_verify(
          unit,
          tcam_db_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  ARAD_TCAM_PREFIX_clear(prefix);

  ARAD_TCAM_DB_EXISTS_ASSERT;

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(
          unit,
          tcam_db_id,
          bank_id,
          &has_bank
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  if (!has_bank)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d Tcam Db id %d Bank id %d - Failed to get prefix. The bank is not in use.\n\r"),
               unit, tcam_db_id, bank_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_BANK_NOT_USED_ERR, 20, exit);
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.prefix.get(
    unit,
    tcam_db_id,
    prefix
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_bank_prefix_get_unsafe()", 0, 0);
}

uint32
  arad_tcam_db_bank_prefix_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_BANK_PREFIX_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_TCAM_DB_ID_MAX, ARAD_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_bank_prefix_get_verify()", 0, 0);
}


uint32
  arad_tcam_db_entry_size_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_OUT ARAD_TCAM_BANK_ENTRY_SIZE *entry_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_ENTRY_SIZE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry_size);

  
  res = arad_tcam_db_entry_size_get_verify(
          unit,
          tcam_db_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  ARAD_TCAM_DB_EXISTS_ASSERT;

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(
          unit,
          tcam_db_id,
          entry_size
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_size_get_unsafe()", 0, 0);
}

uint32
  arad_tcam_db_entry_size_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_ENTRY_SIZE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_TCAM_DB_ID_MAX, ARAD_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_size_get_verify()", 0, 0);
}


uint32
  arad_tcam_db_direct_tbl_entry_set_unsafe(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                   tcam_db_id,
      SOC_SAND_IN  uint32                   bank_id,
      SOC_SAND_IN  uint32                   address,
      SOC_SAND_IN  uint8                    valid,
      SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE    action_bitmap_ndx,      
      SOC_SAND_INOUT  ARAD_TCAM_ACTION      *action
  )
{
  uint32
      res = SOC_SAND_OK;
  ARAD_TCAM_LOCATION 
      location;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_DIRECT_TBL_ENTRY_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action);

  res = arad_tcam_db_direct_tbl_entry_set_verify(
          unit,
          bank_id,
          address,
          action_bitmap_ndx,
          action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!valid) {
      action->value[0] = 0;
  }

  res = arad_tcam_tbl_action_write(
          unit,
          bank_id, 
          address,
          action_bitmap_ndx,  
          0,         
          action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  ARAD_TCAM_LOCATION_clear(&location);
  location.bank_id = bank_id;
  location.entry = address;
  res = arad_tcam_db_bank_occ_bitmap_entry_set(
            unit,
            tcam_db_id,
            &location,
            valid
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_direct_tbl_entry_set_unsafe()", address, 0);
}


uint32
  arad_tcam_db_direct_tbl_entry_get_unsafe(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                   tcam_db_id,
      SOC_SAND_IN  uint32                   bank_id,
      SOC_SAND_IN  uint32                   address,
      SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE    action_bitmap_ndx,      
      SOC_SAND_INOUT  ARAD_TCAM_ACTION      *action,
      SOC_SAND_OUT  uint8                   *valid
  )
{
  uint32
      res = SOC_SAND_OK;
  uint8
      hit_bit;
  ARAD_TCAM_LOCATION
      location;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_DIRECT_TBL_ENTRY_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action);

  res = arad_tcam_db_direct_tbl_entry_set_verify(
          unit,
          bank_id,
          address,
          action_bitmap_ndx,
          action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_tcam_tbl_action_read(
          unit,
          bank_id,
          address,
          action_bitmap_ndx,
          &hit_bit,
          action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
    
  ARAD_TCAM_LOCATION_clear(&location);
  location.bank_id = bank_id;
  location.entry = address;
  res = arad_tcam_db_bank_occ_bitmap_entry_get(
            unit,
            tcam_db_id,
            &location,
            valid
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_direct_tbl_entry_get_unsafe()", address, 0);
}

uint32
  arad_tcam_db_direct_tbl_entry_set_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 bank_id,
    SOC_SAND_IN  uint32                 entry_id,
    SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE  action_bitmap_ndx,      
    SOC_SAND_IN  ARAD_TCAM_ACTION       *action
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_DIRECT_TBL_ENTRY_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(bank_id, SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit), ARAD_TCAM_BANK_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id, ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_id), ARAD_TCAM_ENTRY_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(action_bitmap_ndx, ARAD_TCAM_ACTION_SIZE_MAX, ARAD_TCAM_ACTION_SIZE_OUT_OF_RANGE_ERR, 30, exit);
  ARAD_STRUCT_VERIFY(ARAD_TCAM_ACTION, action, 40, exit);
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_add_verify()", bank_id, entry_id);
}


uint32
  arad_tcam_db_entry_add_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint32               priority,
    SOC_SAND_IN  ARAD_TCAM_ENTRY      *entry,
    SOC_SAND_IN  ARAD_TCAM_ACTION     *action,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  uint32
      bank_nof_entries,
    res = SOC_SAND_OK;
  uint8
    is_update_and_invalid,
    entry_exists;
  uint8 has_banks = FALSE;
  ARAD_TCAM_LOCATION
    location;
  ARAD_TCAM_ENTRY
    entry_buffer;
  ARAD_TCAM_ACTION_SIZE 
    action_bitmap_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_ENTRY_ADD_UNSAFE);

  
  res = arad_tcam_db_entry_add_verify(
          unit,
          tcam_db_id,
          entry_id,
          priority,
          entry,
          action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ARAD_TCAM_DB_EXISTS_ASSERT;

  
  res = arad_tcam_db_has_banks(
          unit,
          tcam_db_id,
          &has_banks
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (!has_banks)
  {
    LOG_VERBOSE(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                            "Unit %d Tcam DB id %d - The data base has no bank.\n\r"), unit, tcam_db_id));
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    ARAD_DO_NOTHING_AND_EXIT;
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.action_bitmap_ndx.get(
          unit,
          tcam_db_id,
          &action_bitmap_ndx
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);

  
  res = arad_tcam_db_entry_exists_get(
          unit,
          tcam_db_id,
          entry_id,
          &entry_exists
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  
  if ((entry_exists && (!entry->is_for_update)) 
      || ((!entry_exists) && entry->is_for_update))
  {
    if (!entry->is_keep_location) {
        *success = SOC_SAND_FAILURE_REMOVE_ENTRY_FIRST;
        ARAD_DO_NOTHING_AND_EXIT;
    }
  }
  res = arad_tcam_db_free_location_allocate(
          unit,
          tcam_db_id,
          entry->is_inserted_top,
          priority,
          entry->is_for_update,
          entry->is_keep_location,
          entry_id,
          &is_update_and_invalid,
          &location,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  if (*success == SOC_SAND_SUCCESS)
  {
    

      
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.get(unit, tcam_db_id, location.bank_id, &bank_nof_entries);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 53, exit);
      bank_nof_entries ++;
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.set(unit, tcam_db_id, location.bank_id, bank_nof_entries);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 55, exit);

     
    if (entry->is_for_update && is_update_and_invalid) {
        res = arad_tcam_db_entry_remove_unsafe(
                unit,
                FALSE ,
                tcam_db_id,
                entry_id,
                FALSE
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }

    entry_buffer = *entry;
    res = arad_tcam_db_entry_prefix_stamp(
            unit,
            tcam_db_id,
            &entry_buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = arad_tcam_bank_entry_set_unsafe(
            unit,
            TRUE, 
            FALSE, 
            0, 
            location.bank_id,
            location.entry,
            action_bitmap_ndx,
            &entry_buffer,
            action
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    
    if (entry->is_for_update && (!is_update_and_invalid)) {
        res = arad_tcam_db_entry_remove_unsafe(
                unit,
                TRUE ,
                tcam_db_id,
                entry_id,
                FALSE
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 82, exit);
    }

      if (!entry->is_keep_location) {
          
          res = arad_tcam_db_entry_id_to_location_entry_add(
                  unit,
                  tcam_db_id,
                  entry_id,
                  &location
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
          
          res = arad_tcam_global_location_table_entry_add(
                  unit,
                  tcam_db_id,
                  &location,
                  entry_id,
                  priority
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
      
          
          res = arad_tcam_db_bank_occ_bitmap_entry_set(
                  unit,
                  tcam_db_id,
                  &location,
                  TRUE 
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);
      
          
          res = arad_tcam_db_priority_list_entry_add(
                  unit,
                  tcam_db_id,
                  &location,
                  entry_id,
                  priority,
                  FALSE, 
                  success
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 92, exit);
      }

  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_add_unsafe()", tcam_db_id, entry_id);
}

uint32
  arad_tcam_db_entry_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint32               priority,
    SOC_SAND_IN  ARAD_TCAM_ENTRY       *entry,
    SOC_SAND_IN  ARAD_TCAM_ACTION      *action
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_ENTRY_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_TCAM_DB_ID_MAX, ARAD_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id, ARAD_TCAM_ENTRY_ID_MAX, ARAD_TCAM_ENTRY_ID_OUT_OF_RANGE_ERR, 20, exit);

  ARAD_STRUCT_VERIFY(ARAD_TCAM_ENTRY, entry, 40, exit);
  ARAD_STRUCT_VERIFY(ARAD_TCAM_ACTION, action, 50, exit);
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_add_verify()", entry_id, priority);
}


uint32
  arad_tcam_db_entry_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint8               hit_bit_clear, 
    SOC_SAND_OUT uint32               *priority,
    SOC_SAND_OUT ARAD_TCAM_ENTRY     *entry,
    SOC_SAND_OUT ARAD_TCAM_ACTION    *action,
    SOC_SAND_OUT uint8               *found,
    SOC_SAND_OUT uint8               *hit_bit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    stored_entry_id;
  ARAD_TCAM_LOCATION
    location;
  ARAD_TCAM_ACTION_SIZE 
    action_bitmap_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_ENTRY_GET_UNSAFE);

  
  res = arad_tcam_db_entry_get_verify(
          unit,
          tcam_db_id,
          entry_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_CHECK_NULL_INPUT(priority);
  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_NULL_INPUT(found);

  ARAD_TCAM_ENTRY_clear(entry);
  ARAD_TCAM_ACTION_clear(action);
  *hit_bit = 0;

  ARAD_TCAM_DB_EXISTS_ASSERT;

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.action_bitmap_ndx.get(
          unit,
          tcam_db_id,
          &action_bitmap_ndx
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);


  res = arad_tcam_db_entry_id_to_location_entry_get(
          unit,
          tcam_db_id,
          entry_id,
          TRUE,
          &location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (*found)
  {
    
    res = arad_tcam_bank_entry_get_unsafe(
            unit,
            location.bank_id,
            location.entry,
            action_bitmap_ndx,
            hit_bit_clear,
            entry,
            action,
            hit_bit
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    
    res = arad_tcam_db_priority_list_entry_get(
            unit,
            tcam_db_id,
            &location,
            &stored_entry_id,
            priority
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    if (stored_entry_id != entry_id)
    {
      LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                            "Unit %d Tcam DB id %d Entry id %d - The location where the entry lies leads to differnt entry id - %d.\n\r"),
                 unit, tcam_db_id, entry_id, stored_entry_id));
      SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 50, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_get_unsafe()", tcam_db_id, entry_id);
}

uint32
  arad_tcam_db_entry_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_TCAM_DB_ID_MAX, ARAD_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id, ARAD_TCAM_ENTRY_ID_MAX, ARAD_TCAM_ENTRY_ID_OUT_OF_RANGE_ERR, 20, exit);
 
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_get_verify()", tcam_db_id, entry_id);
}


uint32
  arad_tcam_db_entry_search_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  ARAD_TCAM_ENTRY           *key,
    SOC_SAND_OUT uint32               *entry_id,
    SOC_SAND_OUT uint8               *found
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    hit=0,
    is_used;
  uint32
    priority;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;
  ARAD_TCAM_LOCATION
    first_hit,
    current;
  ARAD_TCAM_ENTRY
    stamped;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_ENTRY_SEARCH_UNSAFE);

  
  res = arad_tcam_db_entry_search_verify(
          unit,
          tcam_db_id,
          key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(entry_id);
  SOC_SAND_CHECK_NULL_INPUT(found);

  ARAD_TCAM_DB_EXISTS_ASSERT;

  ARAD_TCAM_LOCATION_clear(&current);
  ARAD_TCAM_LOCATION_clear(&first_hit);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(
                 unit,
                 tcam_db_id,
                 &entry_size
               );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  if (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Entry search is not supported for 320 bits bank.\n\r")));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_ENTRY_SEARCH_SIZE_NOT_SUPPORTED_ERR, 20, exit);
  }

  
  *found = FALSE;
  for (current.bank_id = 0; current.bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++current.bank_id)
  {
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(unit, tcam_db_id, current.bank_id, &is_used);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    if (is_used)
    {
      
      stamped = *key;
      res = arad_tcam_db_entry_prefix_stamp(
              unit,
              tcam_db_id,
              &stamped
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      res = arad_tcam_tbl_bank_compare(
              unit,
              current.bank_id,
              key,
              &current.entry,
              &hit
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      if (hit)
      {
          first_hit = current;
          *found    = TRUE;
          break;
      }
    }
  }

  if (*found)
  {
    
    res = arad_tcam_db_priority_list_entry_get(
            unit,
            tcam_db_id,
            &first_hit,
            entry_id,
            &priority
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_search_unsafe()", 0, 0);
}

uint32
  arad_tcam_db_entry_search_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  ARAD_TCAM_ENTRY           *key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_ENTRY_SEARCH_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_TCAM_DB_ID_MAX, ARAD_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_STRUCT_VERIFY(ARAD_TCAM_ENTRY, key, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_search_verify()", 0, 0);
}


uint32
  arad_tcam_db_entry_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint8                is_bank_freed_if_no_entry, 
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint8               is_keep_location   
  )
{
  uint32
      bank_nof_entries,
      priority = 0,
      res = SOC_SAND_OK;
  ARAD_TCAM_LOCATION
    location;
  uint8
    found;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_ENTRY_REMOVE_UNSAFE);

  ARAD_TCAM_DB_EXISTS_ASSERT;

  
  res = arad_tcam_db_entry_remove_verify(
          unit,
          tcam_db_id,
          entry_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_tcam_db_entry_id_to_location_entry_get(
          unit,
          tcam_db_id,
          entry_id,
          TRUE,
          &location,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (found)
  {
    res = arad_tcam_bank_entry_invalidate_unsafe(
            unit,
            TRUE, 
            FALSE, 
            is_keep_location,
            location.bank_id,
            location.entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if (!is_keep_location) {
        res = arad_tcam_global_location_table_entry_remove(
                unit,
                tcam_db_id,
                &location,
                &priority
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

        res = arad_tcam_db_priority_list_entry_remove(
                unit,
                tcam_db_id,
                &location,
                entry_id,
                priority,
                FALSE 
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        res = arad_tcam_db_bank_occ_bitmap_entry_set(
                unit,
                tcam_db_id,
                &location,
                FALSE 
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

        res = arad_tcam_db_entry_id_to_location_entry_remove(
                unit,
                tcam_db_id,
                entry_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
    
    
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.get(unit, tcam_db_id, location.bank_id, &bank_nof_entries);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);
    if (bank_nof_entries == 0) {
        
        SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 18, exit);
    }
    
    if (!is_keep_location) {    
        bank_nof_entries --;
    }

    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.set(unit, tcam_db_id, location.bank_id, bank_nof_entries);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);

    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit, tcam_db_id, &entry_size);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);
    
    if ((bank_nof_entries == 0) && is_bank_freed_if_no_entry) {
        
        res = arad_tcam_db_bank_remove_unsafe(unit, tcam_db_id, location.bank_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

        
        if (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) {
            res = arad_tcam_db_bank_remove_unsafe(unit, tcam_db_id, location.bank_id + 1);
            SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
        }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_remove_unsafe()", 0, 0);
}

uint32
  arad_tcam_db_entry_remove_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_ENTRY_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_TCAM_DB_ID_MAX, ARAD_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id, ARAD_TCAM_ENTRY_ID_MAX, ARAD_TCAM_ENTRY_ID_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_entry_remove_verify()", 0, 0);
}


uint32
  arad_tcam_db_is_bank_used_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_OUT uint8               *is_used
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_IS_BANK_USED_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_used);

  ARAD_TCAM_DB_EXISTS_ASSERT;

  
  res = arad_tcam_db_is_bank_used_verify(
          unit,
          tcam_db_id,
          bank_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(
               unit,
               tcam_db_id,
               bank_id,
               is_used
             );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_is_bank_used_unsafe()", tcam_db_id, bank_id);
}

uint32
  arad_tcam_db_is_bank_used_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_DB_IS_BANK_USED_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_TCAM_DB_ID_MAX, ARAD_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(bank_id, SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit), ARAD_TCAM_BANK_ID_OUT_OF_RANGE_ERR, 20, exit);
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_db_is_bank_used_verify()", tcam_db_id, bank_id);
}



uint32
  arad_tcam_managed_db_compress_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32               tcam_db_id
  )
{
  uint32
      next_entry,
      next_priority,
      bank_id_to,
      bank_id_from,
      bank_nof_entries,
      nof_entries_free,
    res = SOC_SAND_OK;
  ARAD_TCAM_LOCATION
      location_to,
      location,
      location_curr;
  ARAD_TCAM_RANGE
    range;
  SOC_SAND_SUCCESS_FAILURE 
      success;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;
  uint8
      found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    

  ARAD_TCAM_DB_EXISTS_ASSERT;

  
  
  ARAD_TCAM_LOCATION_clear(&location_curr);
  location_curr.bank_id = 0;
  location_curr.entry = 0;
  res = arad_tcam_db_occ_bmp_entry_closest_get(
            unit,
            tcam_db_id,
            ARAD_TCAM_DIRECTION_FORWARD,
            &location_curr,
            &next_entry,
            &next_priority,
            &location,
            &found
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  bank_id_to = location.bank_id; 
  while (found)
  {
      bank_id_from = location.bank_id; 
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.nof_entries_free.get(unit, bank_id_to, &nof_entries_free);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      if (nof_entries_free == 0) {
          
          bank_id_to = bank_id_from;
      }

      
      
      if (!((bank_id_to == bank_id_from) && (location.entry == 0))) {
          
          ARAD_TCAM_RANGE_clear(&range);
          range.min.bank_id = bank_id_to;
          range.min.entry = 0;
          range.max.bank_id = bank_id_from;
          range.max.entry = location.entry;
          res = arad_tcam_db_empty_location_in_range_find(
                unit,
                tcam_db_id,
                TRUE, 
                &range,
                &location_to,
                &success
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

          if (success == SOC_SAND_SUCCESS) {
              
              if (location_to.bank_id != bank_id_to) {
                  
                  LOG_ERROR(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                        "Unit %d Tcam db id %d: entry in line (%d, %d) is supposed to move to Bank id %d = Bank id %d and line %d.\n\r"),
                             unit, tcam_db_id, location.bank_id, location.entry, bank_id_to, location_to.bank_id, location_to.entry));
                  SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 25, exit);
              }
              res = arad_tcam_db_entry_move(
                      unit,
                      TRUE, 
                      FALSE, 
                      &location,
                      &location_to
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

              
              res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit, tcam_db_id, &entry_size);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);
              res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.get(unit, tcam_db_id, location.bank_id, &bank_nof_entries);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);
              if (bank_nof_entries == 0) {
                  
                  res = arad_tcam_db_bank_remove_unsafe(unit, tcam_db_id, location.bank_id);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

                  
                  if (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) {
                      res = arad_tcam_db_bank_remove_unsafe(unit, tcam_db_id, location.bank_id + 1);
                      SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
                  }
              }
          }
      }

      location_curr = location;
      res = arad_tcam_db_occ_bmp_entry_closest_get(
                unit,
                tcam_db_id,
                ARAD_TCAM_DIRECTION_FORWARD,
                &location_curr,
                &next_entry,
                &next_priority,
                &location,
                &found
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_managed_db_compress_unsafe()", tcam_db_id, 0);
}


#ifdef ARAD_TCAM_BANK_INIT_MOVE_ENTRIES_ENABLE
    
uint32
    arad_tcam_new_bank_move_entries(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  uint32            bank_id,
      SOC_SAND_IN  uint8             is_direct,
      SOC_SAND_IN  uint32            access_profile_array_id,
      SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
      SOC_SAND_IN  uint8             is_inserted_top
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    bank_idx,
    nof_lines_jump = 0, 
      line_idx = 0, 
    line_ratio = 0, 
    entry_idx, 
      entry_id,
    nof_entries_to_move[ARAD_TCAM_BANK_INIT_MOVE_ENTRIES_NOF_ADJACENT_BANKS] = {0, 0}, 
    nof_entries_db[ARAD_TCAM_BANK_INIT_MOVE_ENTRIES_NOF_ADJACENT_BANKS] = {0, 0}, 
    nof_entries_free = 0, 
    start,
    end;
  uint8
    is_bank_upper,
    other_bank_ndx,
      found,
    bank_found_other[ARAD_TCAM_BANK_INIT_MOVE_ENTRIES_NOF_ADJACENT_BANKS] = {0, 0},
    line_found,
    is_used;
  uint32
    next_priority;
  ARAD_TCAM_LOCATION
      curr_location,
    next_location;
  ARAD_TCAM_LOCATION       
      empty_location;
  ARAD_TCAM_DIRECTION 
      direction;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    if (is_direct || (is_inserted_top == TRUE)
        || ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) && (access_profile_array_id == 0))) {
        
        ARAD_DO_NOTHING_AND_EXIT;
    }


    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.nof_entries_free.get(unit, bank_id, &nof_entries_free);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    for (bank_idx = 0; bank_idx < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); ++bank_idx)
    {
        if (bank_idx == bank_id) {
            
            continue;
        }

        if ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) && ((bank_idx % 2) == 1)) {
            
            continue;
        }

        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(unit, tcam_db_id, bank_idx, &is_used);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
        if (!is_used) {
            
            continue;
        }

        is_bank_upper = (bank_idx > bank_id)? 1: 0;
        bank_found_other[is_bank_upper] = TRUE;
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_nof_entries.get(unit, tcam_db_id, bank_idx, &(nof_entries_db[is_bank_upper]));
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
    }

    
    if (bank_found_other[0] || bank_found_other[1]) {
        for (other_bank_ndx = 0; other_bank_ndx < ARAD_TCAM_BANK_INIT_MOVE_ENTRIES_NOF_ADJACENT_BANKS; ++other_bank_ndx)
        {
            
            nof_entries_to_move[other_bank_ndx] = (nof_entries_db[other_bank_ndx] * nof_entries_free) 
                / (nof_entries_free + nof_entries_db[0] + nof_entries_db[1]);
        }
        
        if ((nof_entries_to_move[0] == 0) && (nof_entries_to_move[1] == 0)) {
            ARAD_DO_NOTHING_AND_EXIT;
        }

        
        nof_lines_jump = nof_entries_free / (nof_entries_to_move[0] + nof_entries_to_move[1]); 
        if ((nof_lines_jump == 0) || (((nof_entries_to_move[0] + nof_entries_to_move[1]) * nof_lines_jump) > nof_entries_free)){
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                  "Unit %d Tcam DB id %d. incorrect numbers: number-of-lines-jump %d, nof_entries_free %d.\n\r"),
                       unit, tcam_db_id, nof_lines_jump, nof_entries_free));
            SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 70, exit);
        }

        
        res = arad_tcam_jump_line_find(
                unit,
                bank_id,
                entry_size,
                ARAD_TCAM_DIRECTION_FORWARD,
                0, 
                arad_tcam_bank_entry_size_to_entry_count_get(unit, entry_size, bank_id) - 1, 
                ((nof_entries_to_move[0] * nof_entries_free) / (nof_entries_to_move[0] + nof_entries_to_move[1])),
                &line_ratio,
                &line_found
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
        if (!line_found) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                  "Unit %d Tcam DB id %d. move not found: number-of-lines-jump %d, nof_entries_free %d.\n\r"),
                       unit, tcam_db_id, nof_lines_jump, nof_entries_free));
            SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 90, exit);
        }


        
        for (other_bank_ndx = 0; other_bank_ndx < ARAD_TCAM_BANK_INIT_MOVE_ENTRIES_NOF_ADJACENT_BANKS; ++other_bank_ndx)
        {
            start = (other_bank_ndx == 0)? 0: line_ratio;
            end = (other_bank_ndx == 0)? line_ratio: arad_tcam_bank_entry_size_to_entry_count_get(unit, entry_size, bank_id);

            
            ARAD_TCAM_LOCATION_clear(&empty_location);
            empty_location.bank_id = bank_id;
            empty_location.entry = line_ratio; 
            direction = ((other_bank_ndx == 0)? ARAD_TCAM_DIRECTION_BACKWARD: ARAD_TCAM_DIRECTION_FORWARD);
            res = arad_tcam_db_occ_bmp_entry_closest_get(
                      unit,
                      tcam_db_id,
                      direction,
                      &empty_location,
                      &entry_id,
                      &next_priority,
                      &next_location,
                      &found
                    );
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

            for (entry_idx = 0; entry_idx < nof_entries_to_move[other_bank_ndx]; ++entry_idx)
            {
                
                res = arad_tcam_jump_line_find(
                        unit,
                        bank_id,
                        entry_size,
                        ((other_bank_ndx == 0)? ARAD_TCAM_DIRECTION_BACKWARD: ARAD_TCAM_DIRECTION_FORWARD),
                        start,
                        end,
                        nof_lines_jump,
                        &line_idx,
                        &line_found
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
                if (!line_found) {
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                              (BSL_META_U(unit,
                                          "Unit %d Tcam DB id %d. move not found: number-of-lines-jump %d, nof_entries_free %d.\n\r"),
                               unit, tcam_db_id, nof_lines_jump, nof_entries_free));
                    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 90, exit);
                }

                
                empty_location.bank_id = bank_id;
                empty_location.entry = line_idx; 
                res = arad_tcam_db_entry_move(
                        unit,
                        TRUE, 
                        FALSE, 
                        &next_location,
                        &empty_location
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

                
                curr_location = next_location;
                res = arad_tcam_db_occ_bmp_entry_closest_get(
                          unit,
                          tcam_db_id,
                          direction,
                          &curr_location,
                          &entry_id,
                          &next_priority,
                          &next_location,
                          &found
                        );
                SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
                if (!found) {
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                              (BSL_META_U(unit,
                                          "Unit %d Tcam DB id %d. entry move not found from location bank %d entry %d and direction %d.\n\r"),
                               unit, tcam_db_id, curr_location.bank_id, curr_location.entry, direction));
                    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR, 102, exit);
                }

                
                start = (other_bank_ndx == 0)? 0: line_idx;
                end = (other_bank_ndx == 0)? line_idx: arad_tcam_bank_entry_size_to_entry_count_get(unit, entry_size, bank_id);
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_access_profile_bank_add()", 0, tcam_db_id);
}
#endif 



void
  ARAD_TCAM_PREFIX_clear(
    SOC_SAND_OUT ARAD_TCAM_PREFIX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_TCAM_PREFIX));
  info->bits = 0;
  info->length = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TCAM_LOCATION_clear(
    SOC_SAND_OUT ARAD_TCAM_LOCATION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_TCAM_LOCATION));
  info->bank_id = 0;
  info->entry = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TCAM_GLOBAL_LOCATION_clear(
    SOC_SAND_OUT ARAD_TCAM_GLOBAL_LOCATION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_TCAM_GLOBAL_LOCATION));
  info->tcam_db_id = ARAD_TCAM_DB_ID_INVALID;
  info->entry_id = 0;
  info->priority = 0;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TCAM_PRIO_LOCATION_clear(
    SOC_SAND_OUT ARAD_TCAM_PRIO_LOCATION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_TCAM_PRIO_LOCATION));
  info->entry_id_first = 0;
  info->entry_id_last = 0;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TCAM_RANGE_clear(
    SOC_SAND_OUT ARAD_TCAM_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_TCAM_RANGE));
  ARAD_TCAM_LOCATION_clear(&(info->min));
  ARAD_TCAM_LOCATION_clear(&(info->max));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TCAM_ENTRY_clear(
    SOC_SAND_OUT ARAD_TCAM_ENTRY *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_TCAM_ENTRY));
  for (ind = 0; ind < ARAD_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    info->value[ind] = 0;
  }
  for (ind = 0; ind < ARAD_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    info->mask[ind] = 0;
  }
  info->is_for_update = FALSE;
  info->is_inserted_top = FALSE;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TCAM_ACTION_clear(
    SOC_SAND_OUT ARAD_TCAM_ACTION *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_TCAM_ACTION));
  for (ind = 0; ind < ARAD_TCAM_ACTION_MAX_LEN; ++ind)
  {
    info->value[ind] = 0;
  }
  
#if defined(INCLUDE_KBP)
  for (ind = 0; ind < SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES; ++ind)
  {
    info->elk_ad_value[ind] = 0;
  }
#endif 

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
  ARAD_TCAM_PREFIX_verify(
    SOC_SAND_IN  ARAD_TCAM_PREFIX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->bits, ARAD_TCAM_BITS_MAX, ARAD_TCAM_BITS_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->length, ARAD_TCAM_LENGTH_MAX, ARAD_TCAM_LENGTH_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TCAM_PREFIX_verify()",0,0);
}

uint32
  ARAD_TCAM_LOCATION_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  ARAD_TCAM_LOCATION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_NOF(info->bank_id, SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit), ARAD_TCAM_BANK_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entry, SOC_DPP_DEFS_TCAM_ENTRY_MAX(unit), ARAD_TCAM_ENTRY_OUT_OF_RANGE_ERR, 11, exit);



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TCAM_LOCATION_verify()",0,0);
}

uint32
  ARAD_TCAM_GLOBAL_LOCATION_verify(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  ARAD_TCAM_GLOBAL_LOCATION *info
  )
{
    uint32
        tcam_db_id = info->tcam_db_id;
#ifdef BROADCOM_DEBUG
    uint32
        res;
#endif 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_TCAM_DB_ID_MAX, ARAD_TCAM_BANK_ID_OUT_OF_RANGE_ERR, 10, exit);
  
  if (tcam_db_id != ARAD_TCAM_DB_ID_INVALID) {
      ARAD_TCAM_DB_EXISTS_ASSERT;

      
      ARAD_TCAM_DB_ENTRY_EXISTS_ASSERT(info->entry_id);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TCAM_GLOBAL_LOCATION_verify()",0,0);
}

uint32
  ARAD_TCAM_PRIO_LOCATION_verify(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32     tcam_db_id,
    SOC_SAND_IN  ARAD_TCAM_PRIO_LOCATION *info
  )
{
#ifdef BROADCOM_DEBUG
    uint32
        res;
#endif 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  ARAD_TCAM_DB_ENTRY_EXISTS_ASSERT(info->entry_id_first);

  
  ARAD_TCAM_DB_ENTRY_EXISTS_ASSERT(info->entry_id_last);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TCAM_PRIO_LOCATION_verify()",0,0);
}

uint32
  ARAD_TCAM_RANGE_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_TCAM_RANGE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = ARAD_TCAM_LOCATION_verify(unit, &(info->min));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = ARAD_TCAM_LOCATION_verify(unit, &(info->max));
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TCAM_RANGE_verify()",0,0);
}

uint32
  ARAD_TCAM_ENTRY_verify(
    SOC_SAND_IN  ARAD_TCAM_ENTRY *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < ARAD_TCAM_ENTRY_MAX_LEN; ++ind)
  {
  }
  for (ind = 0; ind < ARAD_TCAM_ENTRY_MAX_LEN; ++ind)
  {
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TCAM_ENTRY_verify()",0,0);
}

uint32
  ARAD_TCAM_ACTION_verify(
    SOC_SAND_IN  ARAD_TCAM_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TCAM_ENTRY_verify()",0,0);
}

#if ARAD_DEBUG_IS_LVL1

const char*
  ARAD_TCAM_BANK_ENTRY_SIZE_to_string(
    SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE enum_val
  )
{
  return SOC_TMC_TCAM_BANK_ENTRY_SIZE_to_string(enum_val);
}

void
  ARAD_TCAM_PREFIX_print(
    SOC_SAND_IN  ARAD_TCAM_PREFIX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "bits: %u\n\r"),info->bits));
  LOG_CLI((BSL_META_U(unit,
                      "length: %u\n\r"),info->length));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TCAM_LOCATION_print(
    SOC_SAND_IN  ARAD_TCAM_LOCATION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "bank_id: %u\n\r"),info->bank_id));
  LOG_CLI((BSL_META_U(unit,
                      "entry: %u\n\r"),info->entry));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TCAM_GLOBAL_LOCATION_print(
    SOC_SAND_IN  ARAD_TCAM_GLOBAL_LOCATION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tcam_db_id: %u, "),info->tcam_db_id));
  LOG_CLI((BSL_META_U(unit,
                      "entry_id: %u, "),info->entry_id));
  LOG_CLI((BSL_META_U(unit,
                      "priority: %u\n\r"),info->priority));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TCAM_RANGE_print(
    SOC_SAND_IN  ARAD_TCAM_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "min:")));
  ARAD_TCAM_LOCATION_print(&(info->min));
  LOG_CLI((BSL_META_U(unit,
                      "max:")));
  ARAD_TCAM_LOCATION_print(&(info->max));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_TCAM_ENTRY_print(
    SOC_SAND_IN  ARAD_TCAM_ENTRY *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < ARAD_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "value[%u]: %u\n\r"),ind,info->value[ind]));
  }
  for (ind = 0; ind < ARAD_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "mask[%u]: %u\n\r"),ind,info->mask[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 

uint32
  arad_tcam_resource_db_entries_find(
      SOC_SAND_IN  int     unit,
      SOC_SAND_IN  uint32     tcam_db_id,
      SOC_SAND_OUT uint32     *db_nof_entries
    )
{
  uint32
      next_entry,
      next_priority,
    res = SOC_SAND_OK;
  ARAD_TCAM_LOCATION 
      location_curr,
    location;
  uint8
      found,
      is_occupied,
      is_used;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(db_nof_entries);

  ARAD_TCAM_DB_EXISTS_ASSERT;

  sal_memset(db_nof_entries, 0x0, (sizeof(uint32) * SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit)));

  
  ARAD_TCAM_LOCATION_clear(&location_curr);
  location_curr.bank_id = 0;
  location_curr.entry = 0;

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(unit, tcam_db_id, location_curr.bank_id, &is_used);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit); 
  if(is_used){
      res = arad_tcam_db_bank_occ_bitmap_entry_get(
                unit,
                tcam_db_id,
                &location_curr,
                &is_occupied
                );
      SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
      if (is_occupied) {
          db_nof_entries[location_curr.bank_id]++;
      }
  }

  
  do
  {
      res = arad_tcam_db_occ_bmp_entry_closest_get(
                unit,
                tcam_db_id,
                ARAD_TCAM_DIRECTION_FORWARD,
                &location_curr,
                &next_entry,
                &next_priority,
                &location,
                &found
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if(found)
    {
      db_nof_entries[location.bank_id]++;
      location_curr = location;
    }
  } while (found);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_resource_db_entries_find", tcam_db_id, 0);
}

uint32
  arad_tcam_resource_db_entries_validate(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
	  SOC_SAND_IN  uint32 address,
    SOC_SAND_OUT ARAD_TCAM_ENTRY *entry
  )
{
	uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(entry);

  res = arad_tcam_tbl_bank_read(
          unit,
          bank_id,
          entry_size,
          address,
          entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_resource_db_entries_validate", 0, 0);
}

uint32
  arad_tcam_resource_db_entries_priority_validate(
      SOC_SAND_IN  int        unit,
      SOC_SAND_IN  uint32        tcam_db_id,
      SOC_SAND_IN  ARAD_TCAM_LOCATION *location,
      SOC_SAND_OUT uint32        *entry_id,
      SOC_SAND_OUT uint32         *priority
  )
{
    uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(location);
  SOC_SAND_CHECK_NULL_INPUT(entry_id);
  SOC_SAND_CHECK_NULL_INPUT(priority);

  res = arad_tcam_db_priority_list_entry_get(
          unit,
          tcam_db_id,
          location,
          entry_id,
          priority
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_resource_db_entries_priority_validate", 0, 0);
}

uint32
  arad_tcam_entry_rewrite(
      SOC_SAND_IN       int                          unit,
      SOC_SAND_IN       uint8                        entry_exists,
      SOC_SAND_IN       uint32                       tcam_db_id,
      SOC_SAND_IN       ARAD_TCAM_LOCATION           *location,
      SOC_SAND_IN       ARAD_TCAM_BANK_ENTRY_SIZE    entry_size,
      SOC_SAND_INOUT    ARAD_TCAM_ENTRY              *entry
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(location);
    SOC_SAND_CHECK_NULL_INPUT(entry);

    ARAD_STRUCT_VERIFY(ARAD_TCAM_ENTRY, entry, 40, exit);

    if (entry_exists)
    {
        
        res = arad_tcam_db_entry_prefix_stamp(unit, tcam_db_id, entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }

    
    res = arad_tcam_tbl_bank_write(unit, location->bank_id, entry_size, location->entry, entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_entry_rewrite", 0, 0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>

#undef _ERR_MSG_MODULE_NAME

#endif 


