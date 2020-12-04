
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_fec.h>






















uint32
  soc_ppd_frwrd_fec_glbl_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = arad_pp_frwrd_fec_glbl_info_set_verify(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_glbl_info_set_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_glbl_info_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_fec_glbl_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = arad_pp_frwrd_fec_glbl_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_glbl_info_get_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_glbl_info_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_fec_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_TYPE              protect_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_INFO              *protect_info,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(working_fec);
  
  SOC_SAND_CHECK_NULL_INPUT(protect_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_fec_entry_add_verify(
          unit,
          fec_ndx,
          protect_type,
          working_fec,
          protect_fec,
          protect_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_entry_add_unsafe(
          unit,
          fec_ndx,
          protect_type,
          working_fec,
          protect_fec,
          protect_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_entry_add()", fec_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_ecmp_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                  nof_entries,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_fec_ecmp_add_verify(
          unit,
          fec_ndx,
          fec_array,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_ecmp_add_unsafe(
          unit,
          fec_ndx,
          fec_array,
          nof_entries,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_add()", fec_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_ecmp_update(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_UPDATE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(fec_range);

  res = arad_pp_frwrd_fec_ecmp_update_verify(
          unit,
          fec_ndx,
          fec_array,
          fec_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_ecmp_update_unsafe(
          unit,
          fec_ndx,
          fec_array,
          fec_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_update()", fec_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_entry_use_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO            *fec_entry_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fec_entry_info);

  res = arad_pp_frwrd_fec_entry_use_info_get_verify(
          unit,
          fec_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_entry_use_info_get_unsafe(
          unit,
          fec_ndx,
          fec_entry_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_entry_use_info_get()", fec_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_ecmp_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              ecmp_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_INFO       *accessed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ECMP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);
  res = arad_pp_frwrd_fec_ecmp_info_set_verify(
          unit,
          ecmp_ndx,
          accessed_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_ecmp_info_set_unsafe(
          unit,
          ecmp_ndx,
          accessed_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_info_set()", ecmp_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_ecmp_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              ecmp_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_INFO       *accessed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ECMP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  res = arad_pp_frwrd_fec_ecmp_info_get_verify(
          unit,
          ecmp_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_ecmp_info_get_unsafe(
          unit,
          ecmp_ndx,
          accessed_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_info_get()", ecmp_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_PROTECT_TYPE              *protect_type,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_PROTECT_INFO              *protect_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(protect_type);
  SOC_SAND_CHECK_NULL_INPUT(working_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_info);

  res = arad_pp_frwrd_fec_entry_get_verify(
          unit,
          fec_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_entry_get_unsafe(
          unit,
          fec_ndx,
          protect_type,
          working_fec,
          protect_fec,
          protect_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_entry_get()", fec_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_ecmp_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fec_range);
  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_frwrd_fec_ecmp_get_verify(
          unit,
          fec_ndx,
          fec_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_ecmp_get_unsafe(
          unit,
          fec_ndx,
          fec_range,
          fec_array,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_get()", fec_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_fec_remove_verify(
          unit,
          fec_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_remove_unsafe(
          unit,
          fec_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_remove()", fec_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_protection_oam_instance_status_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_IN  uint8                                 up
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_fec_protection_oam_instance_status_set_verify(
          unit,
          oam_instance_ndx,
          up
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_protection_oam_instance_status_set_unsafe(
          unit,
          oam_instance_ndx,
          up
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_protection_oam_instance_status_set()", oam_instance_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_protection_oam_instance_status_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_OUT uint8                                 *up
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(up);

  res = arad_pp_frwrd_fec_protection_oam_instance_status_get_verify(
          unit,
          oam_instance_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_protection_oam_instance_status_get_unsafe(
          unit,
          oam_instance_ndx,
          up
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_protection_oam_instance_status_get()", oam_instance_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_protection_sys_port_status_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_IN  uint8                                 up
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sys_port_ndx);

  res = arad_pp_frwrd_fec_protection_sys_port_status_set_verify(
          unit,
          sys_port_ndx,
          up
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_protection_sys_port_status_set_unsafe(
          unit,
          sys_port_ndx,
          up
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_protection_sys_port_status_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_fec_protection_sys_port_status_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_OUT uint8                                 *up
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sys_port_ndx);
  SOC_SAND_CHECK_NULL_INPUT(up);

  res = arad_pp_frwrd_fec_protection_sys_port_status_get_verify(
          unit,
          sys_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_protection_sys_port_status_get_unsafe(
          unit,
          sys_port_ndx,
          up
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_protection_sys_port_status_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_fec_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT uint32                                  *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_frwrd_fec_get_block_verify(
          unit,
          rule,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_get_block_unsafe(
          unit,
          rule,
          block_range,
          fec_array,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_get_block()", 0, 0);
}


uint32
  soc_ppd_frwrd_fec_entry_accessed_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);
  res = arad_pp_frwrd_fec_entry_accessed_info_set_verify(
          unit,
          fec_id_ndx,
          accessed_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_entry_accessed_info_set_unsafe(
          unit,
          fec_id_ndx,
          accessed_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_entry_accessed_info_set()", fec_id_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_entry_accessed_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  uint8                                 clear_access_stat,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  res = arad_pp_frwrd_fec_entry_accessed_info_get_verify(
          unit,
          fec_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_entry_accessed_info_get_unsafe(
          unit,
          fec_id_ndx,
          clear_access_stat,
          accessed_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_entry_accessed_info_get()", fec_id_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_ecmp_hashing_global_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);

  res = arad_pp_frwrd_fec_ecmp_hashing_global_info_set_verify(
          unit,
          glbl_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_ecmp_hashing_global_info_set_unsafe(
          unit,
          glbl_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_hashing_global_info_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_fec_ecmp_hashing_global_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);

  res = arad_pp_frwrd_fec_ecmp_hashing_global_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_ecmp_hashing_global_info_get_unsafe(
          unit,
          glbl_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_hashing_global_info_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_fec_ecmp_hashing_port_info_set(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_hash_info);

  res = arad_pp_frwrd_fec_ecmp_hashing_port_info_set_verify(
          unit,
          port_ndx,
          port_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_ecmp_hashing_port_info_set_unsafe(
          unit,
		  core_id,
          port_ndx,
          port_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_hashing_port_info_set()", port_ndx, 0);
}


uint32
  soc_ppd_frwrd_fec_ecmp_hashing_port_info_get(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_hash_info);

  res = arad_pp_frwrd_fec_ecmp_hashing_port_info_get_verify(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_fec_ecmp_hashing_port_info_get_unsafe(
          unit,
		  core_id,
          port_ndx,
          port_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_hashing_port_info_get()", port_ndx, 0);
}


#if SOC_PPC_DEBUG_IS_LVL1

uint32
  soc_ppd_frwrd_fec_print_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_IN SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_IN uint32                                *fec_array,
    SOC_SAND_IN uint32                                nof_entries
  )
{
  uint32
    indx;
  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO
    fec_entry_use_info;
  SOC_PPC_FRWRD_FEC_ENTRY_INFO
    working_fec,
    protect_fec;
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE
    protect_type;
  SOC_PPC_FRWRD_FEC_PROTECT_INFO
    protect_info;
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO
    accessed_info;
  SOC_SAND_PP_SYS_PORT_ID
    sys_port;
  uint8
    stts;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(fec_array);

  LOG_CLI((BSL_META_U(unit,
                      " -----------------------------------------------------------------------------\n\r"
                      "|                                   FEC Table                                 |\n\r"
                      " -----------------------------------------------------------------------------\n\r"
                      "|      |                              |   |   Destination   |                 |\n\r"
                      "|offset|   type   |size|protect| inst |stt| Type     | Val  |EEP/AC | RIF |acc|\n\r"
                      " -----------------------------------------------------------------------------\n\r"
               )));



  for (indx = 0; indx < nof_entries; ++indx)
  {
    LOG_CLI((BSL_META_U(unit,
                        "| %-5u"), fec_array[indx]));
    res = soc_ppd_frwrd_fec_entry_use_info_get(
            unit,
            fec_array[indx],
            &fec_entry_use_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_ppd_frwrd_fec_entry_get(
            unit,
            fec_array[indx],
            &protect_type,
            &working_fec,
            &protect_fec,
            &protect_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
    LOG_CLI((BSL_META_U(unit,
                        "|%-10s"), SOC_PPC_FEC_TYPE_to_string_short(working_fec.type)));
    
    LOG_CLI((BSL_META_U(unit,
                        "|%-4u"), fec_entry_use_info.nof_entries));
    if (fec_entry_use_info.type == SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_FACILITY_PROTECT)
    {
      if(
          (working_fec.dest.dest_type == SOC_SAND_PP_DEST_LAG) ||
          (working_fec.dest.dest_type == SOC_SAND_PP_DEST_SINGLE_PORT)
        )
      {
        sys_port.sys_port_type = (working_fec.dest.dest_type == SOC_SAND_PP_DEST_LAG)?
        SOC_SAND_PP_SYS_PORT_TYPE_LAG: SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;

        sys_port.sys_id = working_fec.dest.dest_val;

        res = soc_ppd_frwrd_fec_protection_sys_port_status_get(
                unit,
                &(sys_port),
                &stts
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      }
      else
      {
        stts = TRUE;
      }
      LOG_CLI((BSL_META_U(unit,
                          "|%-7s"), " fac"));
      LOG_CLI((BSL_META_U(unit,
                          "|%-4s"), " Dest"));
      LOG_CLI((BSL_META_U(unit,
                          "| %s "), (stts)?"V":"X"));
    }
    else if (fec_entry_use_info.type == SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_PATH_PROTECT)
    {
      LOG_CLI((BSL_META_U(unit,
                          "|%-7s"), " path"));
      LOG_CLI((BSL_META_U(unit,
                          "| %-5u"), protect_info.oam_instance_id));
      res = soc_ppd_frwrd_fec_protection_oam_instance_status_get(
              unit,
              protect_info.oam_instance_id,
              &stts
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      LOG_CLI((BSL_META_U(unit,
                          "| %s "), (stts)?"V":"X"));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          "|%-7s"), ""));
      LOG_CLI((BSL_META_U(unit,
                          "|%-6s"), ""));
      LOG_CLI((BSL_META_U(unit,
                          "| %s "), " "));
    }

    LOG_CLI((BSL_META_U(unit,
                        "|")));
    soc_sand_SAND_PP_DESTINATION_ID_table_format_print(&(working_fec.dest));

    res = soc_ppd_frwrd_fec_entry_accessed_info_get(
            unit,
            fec_array[indx],
            TRUE,
            &accessed_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (working_fec.type == SOC_PPC_FEC_TYPE_BRIDGING_WITH_AC)
    {
      LOG_CLI((BSL_META_U(unit,
                          "| %-6u"), working_fec.app_info.out_ac_id));
      LOG_CLI((BSL_META_U(unit,
                          "| %-4s"), ""));
    }
    else if (working_fec.type == SOC_PPC_FEC_TYPE_IP_UC || working_fec.type == SOC_PPC_FEC_TYPE_MPLS_LSR || working_fec.type ==  SOC_PPC_FEC_TYPE_ROUTING)
    {
      LOG_CLI((BSL_META_U(unit,
                          " %-6u"), working_fec.eep));
      LOG_CLI((BSL_META_U(unit,
                          "| %-4u"), working_fec.app_info.out_rif));
    }
    else if (working_fec.type == SOC_PPC_FEC_TYPE_TRILL_MC)
    {
      LOG_CLI((BSL_META_U(unit,
                          " %-6u"), working_fec.eep));
      LOG_CLI((BSL_META_U(unit,
                          "| %-4u"), working_fec.app_info.dist_tree_nick));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          " %-6u"), working_fec.eep));
      LOG_CLI((BSL_META_U(unit,
                          "| %-4s"), ""));
    }
    LOG_CLI((BSL_META_U(unit,
                        "| %s "), (accessed_info.accessed)?"V":"X"));

    LOG_CLI((BSL_META_U(unit,
                        "|\n\r")));
  }

  LOG_CLI((BSL_META_U(unit,
                      " -----------------------------------------------------------------------------\n\r"
               )));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_print_block()", 0, 0);
}
#endif



#include <soc/dpp/SAND/Utils/sand_footer.h>



