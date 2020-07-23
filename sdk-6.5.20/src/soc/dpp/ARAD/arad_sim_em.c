/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_SIMEM

#ifdef _MSC_VER
#pragma warning(disable:4308)
#endif 


#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>


#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/ARAD/arad_sim_em.h>
#if 0
#include <soc/dpp/ARAD/arad_sim_mem.h>
#include <soc/dpp/ARAD/arad_sim_log.h>
#include <soc/dpp/ARAD/arad_sim.h>
#endif

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif
#define CHIP_SIM_EM_BYTE_FILLER  0x0000


CHIP_SIM_EM_BLOCK
Soc_pb_em_blocks[SOC_MAX_NUM_DEVICES][SOC_PB_EM_TABLE_ID_LAST + 1];
uint32
  chip_sim_exact_match_entry_add_unsafe(
          SOC_SAND_IN  int         unit,
          SOC_SAND_IN  uint32      offset, 
          SOC_SAND_IN  uint32      *key_ptr,
          SOC_SAND_IN  uint32     key_size,
          SOC_SAND_IN  uint32      *data_ptr,
          SOC_SAND_IN  uint32     data_size,
          SOC_SAND_OUT uint8     *success
            )
{

  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE] = {0},
    data8[CHIP_SIM_TABLE_DATA_MAX_SIZE] = {0};
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
    key_id, id, ref_count=0;
  uint8
    first_appear,
    is_success;
  uint32
    status = OK ;
  UINT32
    data_nof_bytes;
  uint32
    data_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *success = TRUE;
  status = chip_sim_em_offset_to_table_id (unit, offset, &id);
  if (status != OK)
  {
    ARAD_DO_NOTHING_AND_EXIT;
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.get(unit, id, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_U32_to_U8(
    key_ptr,
    key_size,
    key8
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_multi_set_member_add(
    unit,
    multi_set,
    key8,
    &key_id,
    &first_appear,
    &is_success
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  
  if (!is_success)
  {
    *success = FALSE;
    status = ERROR;
    ARAD_DO_NOTHING_AND_EXIT;
  }

  
  if (first_appear &&  (key_id == SOC_SAND_HASH_TABLE_NULL))  {
      res = soc_sand_multi_set_member_lookup(
        unit,
        multi_set,
        key8,
        &key_id,
        &ref_count
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

  }
  res = soc_sand_U32_to_U8(
    data_ptr,
    data_size,
    data8
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

 
  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.get(unit, id, &data_nof_bytes);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  for(data_idx = 0; data_idx < data_nof_bytes; ++data_idx) {
    res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.base.set(
            unit,
            id,
            key_id*data_nof_bytes + data_idx,
            data8[data_idx]);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in chip_sim_exact_match_entry_add_unsafe()",0,0);
}

uint32
chip_sim_exact_match_entry_get_unsafe(
        SOC_SAND_IN  int         unit,
        SOC_SAND_IN  uint32      offset,
        SOC_SAND_IN  uint32      *key_ptr,
        SOC_SAND_IN  uint32     key_size,
        SOC_SAND_OUT  uint32     *data_ptr,
        SOC_SAND_IN  uint32     data_size,
        SOC_SAND_OUT  uint8    *found
                                  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE], 
    data8[CHIP_SIM_TABLE_DATA_MAX_SIZE];
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
   id, indx, ref_count;
  uint32
    status = OK ;
  UINT32
    data_nof_bytes;
  uint32
    data_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *found = FALSE;
  sal_memset(data8, 0x0, CHIP_SIM_ISEM_KEY_SIZE * sizeof(uint8));
  sal_memset(key8, 0x0, CHIP_SIM_ISEM_KEY_SIZE * sizeof(uint8));

  status = chip_sim_em_offset_to_table_id (unit, offset, &id);
  if (status != OK)
  {
    ARAD_DO_NOTHING_AND_EXIT;
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.get(unit, id, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_U32_to_U8(
    key_ptr,
    key_size,
    key8
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_multi_set_member_lookup(
    unit,
    multi_set,
    key8,
    &indx,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (indx == SOC_SAND_MULTI_SET_NULL)
  {
    *found = FALSE;
    ARAD_DO_NOTHING_AND_EXIT;
  }
  *found = TRUE;

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.get(unit, id, &data_nof_bytes);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  for(data_idx = 0; data_idx < data_nof_bytes; ++data_idx) {
    res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.base.get(
            unit,
            id,
            indx*data_nof_bytes + data_idx,
            &data8[data_idx]);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }
  res = soc_sand_U8_to_U32(
                data8,
                data_nof_bytes,
                data_ptr);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in chip_sim_exact_match_entry_get_unsafe()",0,0);
}

uint32
soc_sand_exact_match_entry_get_by_index_unsafe(
  SOC_SAND_IN  int         unit,
  SOC_SAND_IN  uint32      tbl_offset,
  SOC_SAND_IN  uint32      entry_offset,
  SOC_SAND_OUT  uint32      *key_ptr,
  SOC_SAND_IN  uint32     key_size,
  SOC_SAND_OUT  uint32     *data_ptr,
  SOC_SAND_IN  uint32     data_size,
  SOC_SAND_OUT  uint8    *found
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE], 
    data8[CHIP_SIM_TABLE_DATA_MAX_SIZE];
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
   id, ref_count;
 
  uint32
    status = OK ;
  UINT32
    data_nof_bytes;
  uint32
    data_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *found = FALSE;

  status = chip_sim_em_offset_to_table_id (unit, tbl_offset, &id);
  if (status != OK || id == 0xffffffff)
  {
    ARAD_DO_NOTHING_AND_EXIT;
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.get(unit, id, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_multi_set_get_by_index(
    unit,
    multi_set,
    entry_offset,
    key8,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ref_count == 0)
  {
    *found = FALSE;
    ARAD_DO_NOTHING_AND_EXIT;
  }
  *found = TRUE;

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.get(unit, id, &data_nof_bytes);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  for(data_idx = 0; data_idx < data_nof_bytes; ++data_idx) {
    res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.base.get(
            unit,
            id,
            entry_offset*data_nof_bytes + data_idx,
            &data8[data_idx]);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }

  res = soc_sand_U8_to_U32(
                data8,
                data_nof_bytes,
                data_ptr);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_sand_U8_to_U32(
                key8,
                key_size,
                key_ptr);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_exact_match_entry_get_by_index_unsafe()",0,0);
}


uint32
chip_sim_exact_match_entry_remove_unsafe(
            SOC_SAND_IN  int         unit,
            SOC_SAND_IN  uint32      offset,
            SOC_SAND_IN  uint32      *key_ptr,
            SOC_SAND_IN  uint32     key_size
                                        )
{
  uint32
    id, data_indx;
  uint8
    is_last;
  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE];
  SOC_SAND_MULTI_SET_PTR
    multi_set;

  uint32
    status = OK ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  status = chip_sim_em_offset_to_table_id (unit, offset, &id);
  if (status != OK)
  {
    ARAD_DO_NOTHING_AND_EXIT;
  }

  res = soc_sand_U32_to_U8(
    key_ptr,
    key_size,
    key8
    );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.get(unit, id, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_multi_set_member_remove(
                          unit,
                          multi_set,
                          key8,
                          &data_indx,
                          &is_last);

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in chip_sim_exact_match_entry_remove_unsafe()", 0, 0);
}


STATIC int
chip_sim_em_init_chip_specifics_arad(
                    int unit,
                    int is_lem_shadow_supported,
                    int is_cr_supported
                    )
{
    soc_error_t rv;
    uint8 is_allocated;
    int table_count = ARAD_EM_TABLE_ID_LAST + 1;
    SOCDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.is_allocated(unit, &is_allocated);
    SOCDNX_IF_ERR_EXIT(rv);

    if(is_allocated) {
        SOC_EXIT;
    }

    if (is_cr_supported == 0) {
        
        table_count--;
    }
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.alloc(unit, table_count);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, table_count-1, INVALID_ADDRESS);
    SOCDNX_IF_ERR_EXIT(rv);


    if (is_lem_shadow_supported) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_LEM, ARAD_CHIP_SIM_LEM_BASE);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_LEM, ARAD_CHIP_SIM_LEM_KEY);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_LEM, ARAD_CHIP_SIM_LEM_PAYLOAD);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_LEM, 0);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_LEM, ARAD_CHIP_SIM_LEM_TABLE_SIZE);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_LEM, 0);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_ISEM_A, ARAD_CHIP_SIM_ISEM_A_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_ISEM_A, ARAD_CHIP_SIM_ISEM_A_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_ISEM_A, ARAD_CHIP_SIM_ISEM_A_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_ISEM_A, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_ISEM_A, ARAD_CHIP_SIM_ISEM_A_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_ISEM_A, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_ISEM_B, ARAD_CHIP_SIM_ISEM_B_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_ISEM_B, ARAD_CHIP_SIM_ISEM_B_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_ISEM_B, ARAD_CHIP_SIM_ISEM_B_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_ISEM_B, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_ISEM_B, ARAD_CHIP_SIM_ISEM_B_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_ISEM_B, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_ESEM, ARAD_CHIP_SIM_ESEM_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_ESEM, ARAD_CHIP_SIM_ESEM_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_ESEM, ARAD_CHIP_SIM_ESEM_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_ESEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_ESEM, ARAD_CHIP_SIM_ESEM_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_ESEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_RMAPEM, ARAD_CHIP_SIM_RMAPEM_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_RMAPEM, ARAD_CHIP_SIM_RMAPEM_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_RMAPEM, ARAD_CHIP_SIM_RMAPEM_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_RMAPEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_RMAPEM, ARAD_CHIP_SIM_RMAPEM_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_RMAPEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_OAMEM_A, ARAD_CHIP_SIM_OEMA_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_OAMEM_A, ARAD_CHIP_SIM_OEMA_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_OAMEM_A, ARAD_CHIP_SIM_OEMA_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_OAMEM_A, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_OAMEM_A, ARAD_CHIP_SIM_OEMA_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_OAMEM_A, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_OAMEM_B, ARAD_CHIP_SIM_OEMB_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_OAMEM_B, ARAD_CHIP_SIM_OEMB_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_OAMEM_B, ARAD_CHIP_SIM_OEMB_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_OAMEM_B, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_OAMEM_B, ARAD_CHIP_SIM_OEMB_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_OAMEM_B, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_GLEM, ARAD_CHIP_SIM_GLEM_BASE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_GLEM, ARAD_CHIP_SIM_GLEM_KEY);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_GLEM, ARAD_CHIP_SIM_GLEM_PAYLOAD);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_GLEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_GLEM, ARAD_CHIP_SIM_GLEM_TABLE_SIZE);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_GLEM, 0);
    SOCDNX_IF_ERR_EXIT(rv);

    if (is_cr_supported) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.set(unit, ARAD_EM_TABLE_LEM_CR, ARAD_CHIP_SIM_LEM_CR_BASE);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.set(unit, ARAD_EM_TABLE_LEM_CR, ARAD_CHIP_SIM_LEM_KEY);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.set(unit, ARAD_EM_TABLE_LEM_CR, ARAD_CHIP_SIM_LEM_PAYLOAD);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.set(unit, ARAD_EM_TABLE_LEM_CR, 0);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.set(unit, ARAD_EM_TABLE_LEM_CR, ARAD_CHIP_SIM_LEM_CR_TABLE_SIZE);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.set(unit, ARAD_EM_TABLE_LEM_CR, 0);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
chip_sim_em_malloc_pb(
             uint32 device_id, 
             CHIP_SIM_EM_BLOCK* em_blocks, 
             int                block_cnt
             )
{
  uint32
    status = OK;
  UINT32
    nof_addresses =0;
  int
    blk_ndx;
  CHIP_SIM_EM_BLOCK*
    block_p;
  CHIP_SIM_EM_TABLE_TYPE
    table_type = SOC_PB_EM_TABLE_ISEM;
  uint8
    is_first_loop = TRUE;
  SOC_SAND_MULTI_SET_PTR
    multi_set_info;
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;

  for (blk_ndx = 0; blk_ndx < block_cnt; blk_ndx++) {
      block_p = &(em_blocks[blk_ndx]);

      if (is_first_loop == TRUE) 
      {
        is_first_loop = FALSE;
        table_type = SOC_PB_EM_TABLE_ISEM;
      } 
      else 
      {
        table_type++;
      }

    nof_addresses = block_p->end_address - block_p->start_address + 1;
      block_p->base =
        (uint8*)sal_alloc(nof_addresses*block_p->data_nof_bytes, "EM buffer");
      if (NULL == block_p->base)
      {
        
        status = ERROR ;
        
        ARAD_DO_NOTHING_AND_EXIT;
      }
      
      soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
      multi_set_init_info.get_entry_fun = NULL;
      multi_set_init_info.set_entry_fun = NULL;
      multi_set_init_info.max_duplications = 1;
      multi_set_init_info.sec_handle = 0;
      multi_set_init_info.prime_handle = device_id;
      multi_set_init_info.member_size = block_p->key_size; 
      multi_set_init_info.nof_members = block_p->end_address - block_p->start_address + 1; 

      
      soc_sand_multi_set_create(
        device_id,
        &multi_set_info,
        multi_set_init_info
      );
      block_p->multi_set = multi_set_info ;
      {
        
    #if 0
        UINT32
          byte_i;
    #endif
        sal_memset(
          block_p->base,
          0x0,
          nof_addresses*block_p->data_nof_bytes
          );
    #if 0
        for (byte_i=0; byte_i<nof_addresses*block_p->data_nof_bytes; byte_i++)
        {
          (block_p->base[i])[byte_i] = filler_i;
          if (CHIP_SIM_EM_BYTE_FILLER != 0x0)
          {
            
            filler_i++;
            if (filler_i==0x0)
            {
              filler_i = CHIP_SIM_EM_BYTE_FILLER;
            }
          }
        }
    #endif
      }
  }


exit:
  return status;
}


STATIC int
chip_sim_em_malloc_arad(
             int unit,
             int block_cnt,
             int is_lem_shadow_supported,
             int is_cr_supported
             )
{
  soc_error_t rv;
  UINT32
    nof_addresses =0;
  int
    blk_ndx;
  CHIP_SIM_EM_TABLE_TYPE
    table_type = SOC_PB_EM_TABLE_ISEM;
  uint8
    is_first_loop = TRUE;
  SOC_SAND_MULTI_SET_PTR
    multi_set_info;
  SOC_SAND_MULTI_SET_INIT_INFO
    multi_set_init_info;
  UINT32 end_address;
  UINT32 start_address;
  UINT32 data_nof_bytes;
  uint8 is_allocated;

  SOCDNX_INIT_FUNC_DEFS;

  
  rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.base.is_allocated(unit, 0, &is_allocated);
  SOCDNX_IF_ERR_EXIT(rv);
  if(is_allocated) {
        SOC_EXIT;
  }

  for (blk_ndx = 0; blk_ndx < block_cnt; blk_ndx++) {
      if (is_first_loop == TRUE) 
      {
        is_first_loop = FALSE;
        table_type = SOC_PB_EM_TABLE_ISEM;
      } 
      else 
      {
        table_type++;
      }

      if ((is_lem_shadow_supported == 0) && (table_type == ARAD_EM_TABLE_LEM)) {
          continue;
      }
      if ((is_cr_supported == 0) && (table_type == ARAD_EM_TABLE_LEM_CR)) {
          continue;
      }

      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.end_address.get(unit, blk_ndx, &end_address);
      SOCDNX_IF_ERR_EXIT(rv);
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.start_address.get(unit, blk_ndx, &start_address);
      SOCDNX_IF_ERR_EXIT(rv);
      nof_addresses = end_address - start_address + 1;
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.get(unit, blk_ndx, &data_nof_bytes);
      SOCDNX_IF_ERR_EXIT(rv);
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.base.alloc(unit, blk_ndx, nof_addresses * data_nof_bytes);
      SOCDNX_IF_ERR_EXIT(rv);
      soc_sand_os_memset(&multi_set_init_info, 0x0, sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
      multi_set_init_info.get_entry_fun = NULL;
      multi_set_init_info.set_entry_fun = NULL;
      multi_set_init_info.max_duplications = 1;
      multi_set_init_info.sec_handle = 0;
      multi_set_init_info.prime_handle = unit;
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.key_size.get(unit, blk_ndx, &multi_set_init_info.member_size);
      SOCDNX_IF_ERR_EXIT(rv);
      multi_set_init_info.nof_members = end_address - start_address + 1; 


      
      rv = soc_sand_multi_set_create(
        unit,
        &multi_set_info,
        multi_set_init_info
      );
      SOCDNX_SAND_IF_ERR_EXIT(rv);
      rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.set(unit, blk_ndx, multi_set_info);
      SOCDNX_IF_ERR_EXIT(rv);

  }


exit:
    SOCDNX_FUNC_RETURN;
}


uint32
chip_sim_em_init(  int                  unit, 
                   SOC_SAND_DEVICE_TYPE device_type,
                   int                  is_lem_shadow_supported,
                   int                  is_cr_supported)
{
  int                block_cnt;
  soc_error_t rv;


  SOC_SAND_INTERRUPT_INIT_DEFS;
   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_INTERRUPTS_STOP; 

  block_cnt = 0;

  switch(device_type)
  {
  case SOC_SAND_DEV_ARAD:
    block_cnt = ARAD_EM_TABLE_ID_LAST;
    if (!SOC_WARM_BOOT(unit)) {
        rv = chip_sim_em_init_chip_specifics_arad(unit, is_lem_shadow_supported, is_cr_supported);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
        rv = chip_sim_em_malloc_arad(unit, block_cnt, is_lem_shadow_supported, is_cr_supported);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
    }    
    break;
  default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_FUNC_UNSUPPORTED_ERR, 10, exit);     
      break;
  }


exit:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
  SOC_SAND_EXIT_AND_SEND_ERROR("error in chip_sim_em_init()",0,0);
  

}


uint32 
chip_sim_em_offset_to_table_id (SOC_SAND_IN   uint32      unit,
                                SOC_SAND_IN   uint32      offset,
                                SOC_SAND_OUT  uint32      *id)
{
  uint32
    block_idx;
  UINT32
    read_result_address;
  UINT32
    block_offset;
  soc_error_t rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (block_idx= 0; block_idx < ARAD_EM_TABLE_ID_LAST + 1; ++block_idx)
  {
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.read_result_address.get(unit, block_idx, &read_result_address);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    if(read_result_address == INVALID_ADDRESS) {
      break;
    }
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.offset.get(unit, block_idx, &block_offset);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    if (block_offset == offset)
    {
      *id = block_idx;
      ARAD_DO_NOTHING_AND_EXIT;
    }
  }
  *id = -1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in chip_sim_em_offset_to_table_id()",0,0);
}

uint32
chip_sim_em_get_block(
  SOC_SAND_IN       int         unit,
  SOC_SAND_IN       uint32      tbl_offset,
  SOC_SAND_IN       uint32     key_size,
  SOC_SAND_IN       uint32     data_size,
  SOC_SAND_IN       void*      *filter_data,
  SOC_SAND_INOUT    uint32     *key_array,
  SOC_SAND_INOUT    uint32     *data_array,
  SOC_SAND_OUT      uint32     *num_entries,
  SOC_SAND_INOUT    SOC_SAND_TABLE_BLOCK_RANGE  *block_range 
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    key8[CHIP_SIM_TABLE_KEY_MAX_SIZE];
  uint8  
    data8[CHIP_SIM_TABLE_DATA_MAX_SIZE];
  SOC_SAND_MULTI_SET_PTR
    multi_set ;
  uint32    id, cnt;
  SOC_SAND_MULTI_SET_ITER   iter;
  uint32                     hash_data_ndx, ref_count;
  uint32
    status = OK ;
  uint32 tmp_key;
  SOC_SAND_IN ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA                      *tbl_data;
  uint32    eq, is_diff, max_entries_to_return;
  UINT32
    data_nof_bytes;
  uint32
    data_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  tbl_data = (ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA *)filter_data;
  cnt = 0;
  status = chip_sim_em_offset_to_table_id (unit, tbl_offset, &id);
  if (status != OK)
  {
    ARAD_DO_NOTHING_AND_EXIT;
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.get(unit, id, &multi_set);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  if (block_range == NULL || SOC_SAND_TBL_ITER_IS_BEGIN(&(block_range->iter))) 
  {
      
      SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(&iter); 
  }
  else
  {
      
      iter = (SOC_SAND_MULTI_SET_ITER)(block_range->iter);
  }
  if (block_range != NULL)
  {
      max_entries_to_return = block_range->entries_to_act;
  }
  else
  {
      
      max_entries_to_return = SOC_SAND_U32_MAX - 1;
  }

  while (cnt < max_entries_to_return) {

      res = soc_sand_multi_set_get_next(unit, multi_set, &iter, key8, &hash_data_ndx, &ref_count);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
      {
          break;
      }

      res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.data_nof_bytes.get(unit, id, &data_nof_bytes);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

      for(data_idx = 0; data_idx < data_nof_bytes; ++data_idx) {
        res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.base.get(
                unit,
                id,
                hash_data_ndx*data_nof_bytes + data_idx,
                &data8[data_idx]);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      }


      
      res = soc_sand_U8_to_U32(
                    data8,
                    data_nof_bytes,
                    &data_array[cnt * ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S]);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
     
      eq = TRUE;
      is_diff = 0;

      if (tbl_data != NULL) {
          is_diff =( data_array[cnt * ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S] ^ COMPILER_64_LO(tbl_data->compare_payload_data));
          is_diff = is_diff & COMPILER_64_LO(tbl_data->compare_payload_mask);
          if (is_diff != 0) {
              eq = FALSE;
          } else {
              is_diff =( data_array[cnt * ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S+1] ^ COMPILER_64_HI(tbl_data->compare_payload_data));
              is_diff = is_diff & COMPILER_64_HI(tbl_data->compare_payload_mask);
              if (is_diff != 0) {
                  eq = FALSE;
              }
          }
      }


      
      res = soc_sand_U8_to_U32(
                    key8,
                    key_size,
                    &key_array[cnt * ARAD_PP_LEM_KEY_PARAM_MAX_IN_UINT32S_LEM]);
                    
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      
      tmp_key = (uint32)(key8[6]) | (uint32)(key8[7]<<8) | (uint32)(key8[8]<<16) | (uint32)(key8[9]<<24);
     
      is_diff = 0;

      
      if (tbl_data != NULL) {
          is_diff =( tmp_key ^ (tbl_data->compare_key_20_data));
          is_diff = is_diff & (tbl_data->compare_key_20_mask);
          if (is_diff != 0) {
              eq = FALSE;
          } 
      }
      if (!eq) {
          continue;
      }

      cnt++;
  }
  *num_entries = cnt;

  
  if (block_range != NULL) 
  {
      if (SOC_SAND_HASH_TABLE_ITER_IS_END(&iter)) 
      {
          SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));
      }
      else
      {
          
          block_range->iter = (uint32)iter;
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_LEM_ACCESS_PAYLOAD_verify()",0,0);
  
}

uint32
  chip_sim_em_match_rule_mac(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY                *mac_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint32                                      *match) 
{
    uint8   eq;
    int     i;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *match = FALSE;

    if (mac_key->key_type != SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR) {
        ARAD_DO_NOTHING_AND_EXIT;
    }
    if (rule->key_type != SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    
    for (i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; i++) {
         eq = ((mac_key->key_val.mac.mac.address[i] ^ rule->key_rule.mac.mac.address[i]) &  rule->key_rule.mac.mac_mask.address[i]);
         if (eq != 0 ) {
             ARAD_DO_NOTHING_AND_EXIT;
         }
    }
    eq = (rule->key_rule.mac.fid ^  mac_key->key_val.mac.fid) & rule->key_rule.mac.fid_mask;
    if (eq != 0 ) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    
    if (rule->value_rule.compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC) {
        if (rule->value_rule.val.aging_info.is_dynamic != mac_entry_value->aging_info.is_dynamic) {
            ARAD_DO_NOTHING_AND_EXIT;
        }
    }
    if ((rule->value_rule.compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL) 
        && (rule->value_rule.compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE))
    {
        if (rule->value_rule.val.frwrd_info.forward_decision.dest_id != mac_entry_value->frwrd_info.forward_decision.dest_id) {
            ARAD_DO_NOTHING_AND_EXIT;
        }
        if (rule->value_rule.val.frwrd_info.forward_decision.type != mac_entry_value->frwrd_info.forward_decision.type) {
            ARAD_DO_NOTHING_AND_EXIT;
        }

    }
    *match = TRUE;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in chip_sim_em_match_rule_mac()",0,0);
  
}

#define SIM_EM_REMOVE_ALL_NOF_ENTRIES_FOR_TABLE_BLOCK_GET 50
uint32
  chip_sim_em_delete_all(
    SOC_SAND_IN       int        unit,
    SOC_SAND_IN       uint32     tbl_offset, 
    SOC_SAND_IN       uint32     key_size,
    SOC_SAND_IN       uint32     key_width,
    SOC_SAND_IN       uint32     data_size,
    SOC_SAND_IN       uint32     prefix,
    SOC_SAND_IN       uint32     prefix_size)
{
    uint32                      nof_entries = 0;
    uint32                      entry_key_array[SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S * SIM_EM_REMOVE_ALL_NOF_ENTRIES_FOR_TABLE_BLOCK_GET] = {0};
    uint32                      data_out[ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S * SIM_EM_REMOVE_ALL_NOF_ENTRIES_FOR_TABLE_BLOCK_GET * 10 ] = {0};
    SOC_SAND_TABLE_BLOCK_RANGE  block_range;
    int                         i;
    uint32                      res = SOC_SAND_OK;
    uint32                      entry_prefix;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = SIM_EM_REMOVE_ALL_NOF_ENTRIES_FOR_TABLE_BLOCK_GET;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))){

        res = chip_sim_em_get_block(unit, tbl_offset,  key_size, data_size,
                                NULL, entry_key_array, data_out, &nof_entries, &block_range);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        for (i = 0; i < nof_entries; i++) {
            
            if (prefix != -1) {
                soc_sand_bitstream_get_any_field(&(entry_key_array[i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]),
                      key_width - prefix_size, prefix_size, &entry_prefix);
            }
            
            if (prefix == -1 || entry_prefix == prefix) {
                res = chip_sim_exact_match_entry_remove_unsafe(unit,
                                         tbl_offset, &entry_key_array[i*SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S], key_size);
            }
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        }
    }

    exit:
      SOC_SAND_EXIT_AND_SEND_ERROR("error in chip_sim_em_match_rule_mac()",0,0);
}

#ifdef CRASH_RECOVERY_SUPPORT
uint32 chip_sim_exact_match_cr_shadow_clear(SOC_SAND_IN int unit)
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
   id;
  uint32
    status = OK;
  uint32
    offset = ARAD_CHIP_SIM_LEM_CR_BASE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  status = chip_sim_em_offset_to_table_id (unit, offset, &id);
  if (status != OK)
  {
    ARAD_DO_NOTHING_AND_EXIT;
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_em_blocks.multi_set.get(unit, id, &multi_set);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_multi_set_clear(unit, multi_set);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in chip_sim_exact_match_cr_shadow_clear()",0,0);
}
#endif
