
#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_DRAM



#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_api_dram.h>
#include <soc/dpp/ARAD/arad_dram.h>



























uint32
  arad_dram_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  ARAD_DRAM_TYPE           dram_type,
    SOC_SAND_IN  ARAD_DRAM_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_dram_info_verify(
          unit,
          dram_freq,
          dram_type,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_dram_info_set_unsafe(
          unit,
          dram_ndx,
          dram_freq,
          dram_type,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_info_set()",0,0);
}


uint32
  arad_dram_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_OUT ARAD_DRAM_TYPE           *dram_type,
    SOC_SAND_OUT ARAD_DRAM_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_dram_info_get_unsafe(
          unit,
          dram_ndx,
          dram_freq,
          dram_type,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_info_get()",0,0);
}

uint32
    arad_dram_bist_test_start(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32 dram_ndx,
      SOC_SAND_IN ARAD_DRAM_BIST_TEST_RUN_INFO *info
    )
{
    uint32
        res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_BIST_TEST_START);
    
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_dram_bist_test_start_verify(unit, dram_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_dram_bist_test_start_unsafe(unit, dram_ndx, info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_bist_test_start()", 0, 0);
}


uint32
    arad_dram_mmu_indirect_get_logical_address_full(
         SOC_SAND_IN int unit,
         SOC_SAND_IN uint32 buf_num,
         SOC_SAND_IN uint32 index,
         SOC_SAND_OUT uint32* addr_full 
    )
{
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_dram_mmu_indirect_get_logical_address_full_unsafe(unit, buf_num, index, addr_full);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_mmu_indirect_get_logical_address_full()", 0, 0);
}

uint32
    arad_dram_mmu_indirect_read(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 logical_mod,
        SOC_SAND_IN uint32 addr,
        soc_reg_above_64_val_t* data
    )
{
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    SOC_SAND_CHECK_NULL_INPUT(data);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_dram_mmu_indirect_read_unsafe(unit, logical_mod, addr, data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_mmu_indirect_read()", 0, 0);
}

    
uint32
    arad_dram_mmu_indirect_write(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 logical_mod,
        SOC_SAND_IN uint32 addr,
        soc_reg_above_64_val_t* data
    )
{
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(data);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_dram_mmu_indirect_write_unsafe(unit, logical_mod, addr, data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_mmu_indirect_write()", 0, 0);
}


 
uint32
    arad_dram_crc_delete_buffer_enable(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 fifo_depth)
{
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;  


    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_dram_crc_delete_buffer_enable_unsafe(unit, fifo_depth);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_crc_delete_buffer_enable()", 0, 0);
}
 
uint32
    arad_dram_crc_del_buffer_max_reclaims_set(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 max_err)
{
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_dram_crc_del_buffer_max_reclaims_set_unsafe(unit, max_err);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_appl_max_buff_crc_err_set()", 0, 0);
} 
 
uint32
    arad_dram_crc_del_buffer_max_reclaims_get(
        SOC_SAND_IN int unit,
        SOC_SAND_OUT uint32 *max_err)
{
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    SOC_SAND_CHECK_NULL_INPUT(max_err);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_dram_crc_del_buffer_max_reclaims_get_unsafe(unit, max_err);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_appl_max_buff_crc_err_get()", 0, 0);
}



uint32
    arad_dram_buffer_get_info(
            SOC_SAND_IN int unit,
            SOC_SAND_IN  uint32 buf,
            SOC_SAND_OUT arad_dram_buffer_info_t *buf_info)
{
     uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_dram_buffer_get_info_verify(unit, buf, buf_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_dram_buffer_get_info_unsafe(unit, buf, buf_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_buffer_get_info()", 0, 0);
}
            

uint32
    arad_dram_delete_buffer_read_fifo(
           SOC_SAND_IN int unit, 
           SOC_SAND_IN uint32 del_buf_max, 
           SOC_SAND_OUT uint32 *del_buf_array, 
           SOC_SAND_OUT uint32 *del_buf_count)
{
     uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_dram_delete_buffer_read_fifo_verify(unit, del_buf_max, del_buf_array, del_buf_count);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_dram_delete_buffer_read_fifo_unsafe(unit, del_buf_max, del_buf_array, del_buf_count);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_delete_buffer_read_fifo()", 0, 0);
}
                   

uint32
    arad_dram_delete_buffer_test(
           SOC_SAND_IN int unit, 
           SOC_SAND_IN uint32 del_buf, 
           SOC_SAND_OUT uint32 *is_pass)
{
     uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_dram_delete_buffer_test_verify(unit, del_buf, is_pass);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_dram_delete_buffer_test_unsafe(unit, del_buf, is_pass);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_delete_buffer_test()", 0, 0);
}

uint32
    arad_dram_delete_buffer_action(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 buf, 
        SOC_SAND_IN uint32 should_delete)
{
     uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_dram_delete_buffer_action_verify(unit, buf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_dram_delete_buffer_action_unsafe(unit, buf, should_delete);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_delete_buffer_action()", 0, 0);
}

uint32
    arad_dram_init_buffer_error_cntr(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 buf)
{
     uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_dram_init_buffer_error_cntr_verify(unit, buf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_dram_init_buffer_error_cntr_unsafe(unit, buf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_init_buffer_error_cntr()", 0, 0);
}

uint32
    arad_dram_get_buffer_error_cntr_in_list_index(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 index_in_list, 
        SOC_SAND_OUT uint32 *is_buf,
        SOC_SAND_OUT uint32 *buf )
{
     uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_dram_get_buffer_error_cntr_in_list_index_verify(unit, index_in_list, is_buf, buf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_dram_get_buffer_error_cntr_in_list_index_unsafe(unit, index_in_list, is_buf, buf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_get_buffer_error_cntr_info()", 0, 0);
}



void
  arad_ARAD_DRAM_MR_INFO_clear(
    SOC_SAND_OUT ARAD_DRAM_MODE_REGS_INFO  *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  soc_sand_os_memset(info, 0x0, sizeof(ARAD_DRAM_MODE_REGS_INFO));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_DRAM_INFO_clear(
    SOC_SAND_OUT ARAD_DRAM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  soc_sand_os_memset(info, 0x0, sizeof(ARAD_DRAM_INFO));
  arad_ARAD_DRAM_MR_INFO_clear(&info->mode_regs);
  info->auto_mode = TRUE;
  info->t_rc = 0;
  info->t_rfc = 0;
  info->t_ras = 0;
  info->t_faw = 0;
  info->t_rcd_rd = 0;
  info->t_rcd_wr = 0;
  info->t_rrd = 0;
  info->t_ref = 0;
  info->t_rp = 0;
  info->t_wr = 0;
  info->t_wtr = 0;
  info->c_cas_latency = 0;
  info->c_wr_latency = 0;
  info->t_rtp = 0;
  info->burst_size = ARAD_DRAM_BURST_SIZE_32;
  info->nof_banks = ARAD_DRAM_NUM_BANKS_4;
  info->nof_cols = ARAD_NOF_DRAM_NUMS_COLUMNS;
  info->ap_bit_pos = ARAD_NOF_DRAM_AP_POSITIONS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1

const char*
  arad_ARAD_DRAM_TYPE_to_string(
    SOC_SAND_IN ARAD_DRAM_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_DRAM_TYPE_DDR3:
    str = "ARAD_DRAM_TYPE_DDR3";
    break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  arad_ARAD_DRAM_BURST_SIZE_to_string(
    SOC_SAND_IN ARAD_DRAM_BURST_SIZE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_DRAM_BURST_SIZE_16:
    str = "ARAD_DRAM_BURST_SIZE_16";
  break;
  case ARAD_DRAM_BURST_SIZE_32:
    str = "ARAD_DRAM_BURST_SIZE_32";
  break;

  case ARAD_DRAM_NOF_BURST_SIZES:
    str = "ARAD_DRAM_NOF_BURST_SIZES";
  break;

  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  arad_ARAD_DRAM_NUM_BANKS_to_string(
    SOC_SAND_IN ARAD_DRAM_NUM_BANKS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_DRAM_NUM_BANKS_4:
    str = "DRAM_BANKS_4";
  break;

  case ARAD_DRAM_NUM_BANKS_8:
    str = "DRAM_BANKS_8";
  break;

  case ARAD_NOF_DRAM_NUM_BANKS:
    str = "ARAD_NOF_DRAM_NUM_BANKS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  arad_ARAD_DRAM_AP_POSITION_to_string(
    SOC_SAND_IN ARAD_DRAM_AP_POSITION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_DRAM_AP_POSITION_08:
    str = "08";
  break;

  case ARAD_DRAM_AP_POSITION_09:
    str = "09";
  break;

  case ARAD_DRAM_AP_POSITION_10:
    str = "10";
  break;

  case ARAD_DRAM_AP_POSITION_11:
    str = "11";
  break;

  case ARAD_DRAM_AP_POSITION_12:
    str = "12";
  break;

  case ARAD_NOF_DRAM_AP_POSITIONS:
    str = "ARAD_NOF_DRAM_AP_POSITIONS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  arad_ARAD_DRAM_NUM_COLUMNS_to_string(
    SOC_SAND_IN ARAD_DRAM_NUM_COLUMNS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_DRAM_NUM_COLUMNS_256:
    str = "256";
  break;

  case ARAD_DRAM_NUM_COLUMNS_512:
    str = "512";
  break;

  case ARAD_DRAM_NUM_COLUMNS_1024:
    str = "1024";
  break;

  case ARAD_DRAM_NUM_COLUMNS_2048:
    str = "2048";
  break;

  case ARAD_DRAM_NUM_COLUMNS_4096:
    str = "4096";
  break;

  case ARAD_DRAM_NUM_COLUMNS_8192:
    str = "8192";
  break;

  case ARAD_NOF_DRAM_NUMS_COLUMNS:
    str = "ARAD_NOF_DRAM_NUMS_COLUMNS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  arad_ARAD_DRAM_MR_INFO_print(
    SOC_SAND_IN ARAD_DRAM_TYPE           dram_type,
    SOC_SAND_IN ARAD_DRAM_MODE_REGS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (dram_type == ARAD_DRAM_TYPE_DDR3)
  {
    LOG_CLI((BSL_META_U(unit,
                        "mrs0_wr1: %x\n\r"),info->ddr3.mrs0_wr1));
    LOG_CLI((BSL_META_U(unit,
                        "mrs0_wr2: %x\n\r"),info->ddr3.mrs0_wr2));
    LOG_CLI((BSL_META_U(unit,
                        "mrs1_wr1: %x\n\r"),info->ddr3.mrs1_wr1));
    LOG_CLI((BSL_META_U(unit,
                        "mrs2_wr1: %x\n\r"),info->ddr3.mrs2_wr1));
    LOG_CLI((BSL_META_U(unit,
                        "mrs3_wr1: %x\n\r"),info->ddr3.mrs3_wr1));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_DRAM_INFO_print(
    SOC_SAND_IN ARAD_DRAM_TYPE dram_type,
    SOC_SAND_IN ARAD_DRAM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "c_cas_latency: %u\n\r"),info->c_cas_latency));
  LOG_CLI((BSL_META_U(unit,
                      "c_wr_latency: %u\n\r"),info->c_wr_latency));
  LOG_CLI((BSL_META_U(unit,
                      "burst_size: %s\n\r"),arad_ARAD_DRAM_BURST_SIZE_to_string(info->burst_size)));
  LOG_CLI((BSL_META_U(unit,
                      "nof_banks: %s\n\r"),arad_ARAD_DRAM_NUM_BANKS_to_string(info->nof_banks)));
  LOG_CLI((BSL_META_U(unit,
                      "nof_cols: %s\n\r"),arad_ARAD_DRAM_NUM_COLUMNS_to_string(info->nof_cols)));
  LOG_CLI((BSL_META_U(unit,
                      "nof_cols: %s\n\r"),arad_ARAD_DRAM_AP_POSITION_to_string(info->ap_bit_pos)));
  LOG_CLI((BSL_META_U(unit,
                      "t_rc: %u\n\r"),info->t_rc));
  LOG_CLI((BSL_META_U(unit,
                      "t_rfc: %u\n\r"),info->t_rfc));
  LOG_CLI((BSL_META_U(unit,
                      "t_ras: %u\n\r"),info->t_ras));
  LOG_CLI((BSL_META_U(unit,
                      "t_faw: %u\n\r"),info->t_faw));
  LOG_CLI((BSL_META_U(unit,
                      "t_rcd_rd: %u\n\r"),info->t_rcd_rd));
  LOG_CLI((BSL_META_U(unit,
                      "t_rcd_wr: %u\n\r"),info->t_rcd_wr));
  LOG_CLI((BSL_META_U(unit,
                      "t_rrd: %u\n\r"),info->t_rrd));
  LOG_CLI((BSL_META_U(unit,
                      "t_ref: %u\n\r"),info->t_ref));
  LOG_CLI((BSL_META_U(unit,
                      "t_rp: %u\n\r"),info->t_rp));
  LOG_CLI((BSL_META_U(unit,
                      "t_wr: %u\n\r"),info->t_wr));
  LOG_CLI((BSL_META_U(unit,
                      "t_wtr: %u\n\r"),info->t_wtr));
  LOG_CLI((BSL_META_U(unit,
                      "t_rtp: %u\n\r"),info->t_rtp));
  arad_ARAD_DRAM_MR_INFO_print(dram_type, &info->mode_regs);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 
