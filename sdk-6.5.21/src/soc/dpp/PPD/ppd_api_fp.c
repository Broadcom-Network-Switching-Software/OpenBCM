/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FP



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/ARAD/arad_header_parsing_utils.h>























uint32
  soc_ppd_fp_packet_format_group_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_IN  SOC_PPC_PMF_PFG_INFO            *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FP_PACKET_FORMAT_GROUP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_packet_format_group_set_verify(
          unit,
          pfg_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_packet_format_group_set_unsafe(
          unit,
          pfg_ndx,
          info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_packet_format_group_set()", pfg_ndx, 0);
}


uint32
  soc_ppd_fp_packet_format_group_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_INOUT SOC_PPC_PMF_PFG_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FP_PACKET_FORMAT_GROUP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_packet_format_group_get_verify(
          unit,
          pfg_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_packet_format_group_get_unsafe(
          unit,
          pfg_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_packet_format_group_get()", pfg_ndx, 0);
}


uint32
  soc_ppd_fp_database_create(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT  SOC_PPC_FP_DATABASE_INFO                    *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_DATABASE_CREATE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_database_create_verify(
          unit,
          db_id_ndx,
          info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  if (*success == SOC_SAND_SUCCESS) {
      res = arad_pp_fp_database_create_unsafe(
              unit,
              db_id_ndx,
              info,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
  }

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_database_create()", db_id_ndx, 0);
}


uint32
  soc_ppd_fp_database_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_OUT SOC_PPC_FP_DATABASE_INFO                    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_DATABASE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_database_get_verify(
          unit,
          db_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_database_get()", db_id_ndx, 0);
}


uint32
  soc_ppd_fp_database_destroy(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_DATABASE_DESTROY);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_database_destroy_verify(
          unit,
          db_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_database_destroy_unsafe(
          unit,
          db_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_database_destroy()", db_id_ndx, 0);
}


uint32
  soc_ppd_fp_entry_add(
    SOC_SAND_IN     int                                 unit,
    SOC_SAND_IN     uint32                              db_id_ndx,
    SOC_SAND_IN     uint32                              entry_id_ndx,
    SOC_SAND_INOUT  SOC_PPC_FP_ENTRY_INFO               *info,
    SOC_SAND_OUT    SOC_SAND_SUCCESS_FAILURE            *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_entry_add_verify(
          unit,
          db_id_ndx,
          entry_id_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_entry_add_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx,
          info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_entry_add()", db_id_ndx, entry_id_ndx);
}


uint32
  soc_ppd_fp_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                  *is_found,
    SOC_SAND_INOUT SOC_PPC_FP_ENTRY_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_found);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_entry_get_verify(
          unit,
          db_id_ndx,
          entry_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_entry_get_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx,
          is_found,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_entry_get()", db_id_ndx, entry_id_ndx);
}

uint32 
    soc_ppd_fp_entry_remove_by_key(
       SOC_SAND_IN  int                               unit,
       SOC_SAND_IN  uint32                               db_id_ndx,
       SOC_SAND_INOUT SOC_PPC_FP_ENTRY_INFO              *info
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_fp_entry_remove_by_key_verify(
          unit,
          db_id_ndx,
          info
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

    res = arad_pp_fp_entry_remove_by_key_unsafe(
          unit,
          db_id_ndx,
          info
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:    
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_entry_remove_by_key()", db_id_ndx, 0);
}

uint32
  soc_ppd_fp_entry_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  uint32                                 is_sw_remove_only
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_entry_remove_verify(
          unit,
          db_id_ndx,
          entry_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_entry_remove_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx,
          is_sw_remove_only
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_entry_remove()", db_id_ndx, entry_id_ndx);
}


uint32
  soc_ppd_fp_database_entries_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PPC_FP_ENTRY_INFO                       *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_DATABASE_ENTRIES_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_database_entries_get_block_verify(
          unit,
          db_id_ndx,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_database_entries_get_block_unsafe(
          unit,
          db_id_ndx,
          block_range,
          entries,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_database_entries_get_block()", db_id_ndx, 0);
}


uint32
  soc_ppd_fp_direct_extraction_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO              *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_direct_extraction_entry_add_verify(
          unit,
          db_id_ndx,
          entry_id_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_direct_extraction_entry_add_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx,
          info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_direct_extraction_entry_add()", db_id_ndx, entry_id_ndx);
}


uint32
  soc_ppd_fp_direct_extraction_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                 *is_found,
    SOC_SAND_OUT SOC_PPC_FP_DIR_EXTR_ENTRY_INFO              *info
  )
{
  uint8 fem_count;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_found);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_direct_extraction_entry_get_verify(
          unit,
          db_id_ndx,
          entry_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_direct_extraction_entry_get_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx,
          is_found,
          &fem_count,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_direct_extraction_entry_get()", db_id_ndx, entry_id_ndx);
}


uint32
  soc_ppd_fp_direct_extraction_entry_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_direct_extraction_entry_remove_verify(
          unit,
          db_id_ndx,
          entry_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_direct_extraction_entry_remove_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_direct_extraction_entry_remove()", db_id_ndx, entry_id_ndx);
}


uint32
  soc_ppd_fp_direct_extraction_db_entries_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PPC_FP_DIR_EXTR_ENTRY_INFO              *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_direct_extraction_db_entries_get_verify(
          unit,
          db_id_ndx,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_direct_extraction_db_entries_get_unsafe(
          unit,
          db_id_ndx,
          block_range,
          entries,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_direct_extraction_db_entries_get()", db_id_ndx, 0);
}


uint32
  soc_ppd_fp_control_set(
    SOC_SAND_IN  int                    unit,
	SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INFO        *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_CONTROL_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(control_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_control_set_verify(
          unit,
          control_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_control_set_unsafe(
          unit,
		  core_id,
          control_ndx,
          info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_control_set()", 0, 0);
}


uint32
  soc_ppd_fp_control_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  SOC_PPC_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_OUT SOC_PPC_FP_CONTROL_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_CONTROL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(control_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_control_get_verify(
          unit,
          control_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_control_get_unsafe(
          unit,
          core_id,
          control_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_control_get()", 0, 0);
}


uint32
  soc_ppd_fp_egr_db_map_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_IN  uint32                     db_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_EGR_DB_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_egr_db_map_set_verify(
          unit,
          fwd_type_ndx,
          port_profile_ndx,
          db_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_egr_db_map_set_unsafe(
          unit,
          fwd_type_ndx,
          port_profile_ndx,
          db_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_egr_db_map_set()", 0, port_profile_ndx);
}


uint32
  soc_ppd_fp_egr_db_map_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_OUT uint32                     *db_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_EGR_DB_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(db_id);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_egr_db_map_get_verify(
          unit,
          fwd_type_ndx,
          port_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_egr_db_map_get_unsafe(
          unit,
          fwd_type_ndx,
          port_profile_ndx,
          db_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_egr_db_map_get()", 0, port_profile_ndx);
}


uint32
  soc_ppd_fp_database_compress(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  db_id_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_database_compress_unsafe(
          unit,
          db_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_database_compress()", 0, db_id_ndx);
}



uint32
  soc_ppd_fp_packet_diag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT SOC_PPC_FP_PACKET_DIAG_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FP_PACKET_DIAG_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_packet_diag_get_unsafe(
          unit,
          core_id,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_packet_diag_get()", 0, 0);
}


uint32
  soc_ppd_fp_dbs_action_info_show(
    SOC_SAND_IN		int unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_dbs_action_info_show_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore); 
  
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_dbs_action_info_show()", 0, 0);
}


uint32
  soc_ppd_fp_action_info_show(
    SOC_SAND_IN		int unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_action_info_show_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore); 
  
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_action_info_show()", 0, 0);
}

uint32
  soc_ppd_fp_resource_diag_get(
    SOC_SAND_IN		int							unit,
    SOC_SAND_IN		SOC_PPC_FP_RESOURCE_DIAG_MODE	mode,
    SOC_SAND_OUT 	SOC_PPC_FP_RESOURCE_DIAG_INFO	*info
  )
{
	uint32
	  res = SOC_SAND_OK;

	SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_RESOURCE_DIAG_GET);

	SOC_SAND_CHECK_DRIVER_AND_DEVICE;

	SOC_SAND_CHECK_NULL_INPUT(info);
	
	SOC_SAND_TAKE_DEVICE_SEMAPHORE;

	res = arad_pp_fp_resource_diag_get_unsafe(
			unit,
			mode,
			info
		  );
	SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore); 
	
exit_semaphore:
	SOC_SAND_GIVE_DEVICE_SEMAPHORE;
	ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_resource_diag_get()", 0, 0);
}

uint32
    soc_ppd_fp_ire_traffic_send(
        SOC_SAND_IN int          unit,
        SOC_SAND_IN SOC_PPC_FP_PACKET  *packet,
        SOC_SAND_IN int          tx_count,
        SOC_SAND_IN int          core
    )
{
    uint32
        res = SOC_SAND_OK;
    int i;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_fp_ire_traffic_send_verify(
              unit,
              packet
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);
    for (i = 0 ; i < tx_count ; i++){  
        res = arad_pp_fp_ire_traffic_send_unsafe(
              unit,
              packet,
              core
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
    }

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_ire_traffic_send()", 0, 0);
}


uint32
  soc_ppd_fp_select_line_clear(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_select_line_clear_unsafe(
          unit,
          core_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_select_line_clear()", 0, 0);
}


uint32
  soc_ppd_fp_select_line_get(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  int                         core_id,
    SOC_SAND_OUT SOC_PPC_FP_SELECT_LINE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_select_line_get_unsafe(
          unit,
          core_id,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_select_line_get()", 0, 0);
}


uint32
  soc_ppd_fp_last_fem_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  int                             core_id,
    SOC_SAND_IN  uint32                          fem_ndx,
    SOC_SAND_OUT SOC_PPC_FP_LAST_FEM_INOUT_INFO  *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_last_fem_get_verify(
          unit,
          fem_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_last_fem_get_unsafe(
          unit,
          core_id,
          fem_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_last_fem_get_unsafe()", 0, 0);
}


uint32
  soc_ppd_fp_last_fes_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  int                             core_id,
    SOC_SAND_IN  uint32                          fes_ndx,
    SOC_SAND_OUT SOC_PPC_FP_LAST_FES_INOUT_INFO  *info

  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_fp_last_fem_get_verify(
          unit,
          fes_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_last_fes_get_unsafe(
          unit,
          core_id,
          fes_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_db_last_fes_get_unsafe()", 0, 0);
}


uint32
  soc_ppd_fp_ce_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE         stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                             is_second_lookup,
    SOC_SAND_OUT SOC_PPC_FP_CE_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = arad_pp_fp_ce_info_get_verify(
          unit,
          stage,
          pmf_pgm_ndx,
          ce_ndx,
          is_second_lookup);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_fp_ce_info_get_unsafe(
          unit,
          stage,
          pmf_pgm_ndx,
          ce_ndx,
          is_second_lookup,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_fp_ce_info_get()", 0, 0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


