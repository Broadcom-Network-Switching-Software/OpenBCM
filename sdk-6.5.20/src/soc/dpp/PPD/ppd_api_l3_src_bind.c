/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_L3



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_l3_src_bind.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>





















uint32
  soc_ppd_src_bind_ipv4_host_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE       *block_range
  );

uint32
  soc_ppd_src_bind_ipv6_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE   *block_range_key
  );




uint32
  soc_ppd_src_bind_ipv4_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV4_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_src_bind_ipv4_add_verify(
          unit,
          src_bind_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv4_add_unsafe(
          unit,
          src_bind_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv4_add()", 0, 0);
}


uint32
  soc_ppd_src_bind_ipv4_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT uint8                           *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV4_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_src_bind_ipv4_get_verify(
        unit,
        src_bind_info
      );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv4_get_unsafe(
          unit,
          src_bind_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv4_get()", 0, 0);
}


uint32
  soc_ppd_src_bind_ipv4_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV4_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_src_bind_ipv4_remove_verify(
          unit,
          src_bind_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv4_remove_unsafe(
          unit,
          src_bind_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv4_remove()", 0, 0);
}


uint32
  soc_ppd_src_bind_ipv4_table_clear(
    SOC_SAND_IN  int                           unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV4_TABLE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv4_table_clear_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv4_table_clear()", 0, 0);
}


uint32
  soc_ppd_src_bind_ipv4_host_get_block(
  SOC_SAND_IN    int                              unit,
  SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE      *block_range,
  SOC_SAND_OUT   SOC_PPC_SRC_BIND_IPV4_ENTRY     *src_bind_info,
  SOC_SAND_OUT   SOC_PPC_FRWRD_IP_ROUTE_STATUS   *routes_status,
  SOC_SAND_OUT   uint32                          *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = soc_ppd_src_bind_ipv4_host_get_block_verify(
          unit,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv4_host_get_block_unsafe(
          unit,
          block_range,
          src_bind_info,
          routes_status,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv4_host_get_block()", 0, 0);
}

uint32
  soc_ppd_src_bind_ipv4_host_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE       *block_range
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(block_range->entries_to_act, DPP_PP_LEM_NOF_ENTRIES(unit), ARAD_PP_BLOCK_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(block_range->iter, DPP_PP_LEM_NOF_ENTRIES(unit), ARAD_PP_BLOCK_RANGE_ITER_OUT_OF_RANGE_ERR, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_get_block_verify()", 0, 0);
}


uint32
  soc_ppd_src_bind_ipv6_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV6_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_src_bind_ipv6_add_verify(
          unit,
          src_bind_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv6_add_unsafe(
          unit,
          src_bind_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv6_add()", 0, 0);
}


uint32
  soc_ppd_src_bind_ipv6_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT uint8                           *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV6_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_src_bind_ipv6_get_verify(
          unit,
          src_bind_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv6_get_unsafe(
          unit,
          src_bind_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv4_get()", 0, 0);
}


uint32
  soc_ppd_src_bind_ipv6_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV6_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_src_bind_ipv6_remove_verify(
          unit,
          src_bind_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv6_remove_unsafe(
          unit,
          src_bind_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv6_remove()", 0, 0);
}


uint32
  soc_ppd_src_bind_ipv6_table_clear(
    SOC_SAND_IN  int                           unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV6_TABLE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv6_table_clear_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv6_table_clear()", 0, 0);
}


uint32
  soc_ppd_src_bind_ipv6_get_block(
  SOC_SAND_IN    int                              unit,
  SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE  *block_range,
  SOC_SAND_OUT   SOC_PPC_SRC_BIND_IPV6_ENTRY     *src_bind_info,
  SOC_SAND_OUT   SOC_PPC_FRWRD_IP_ROUTE_STATUS   *routes_status,
  SOC_SAND_OUT   uint32                          *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = soc_ppd_src_bind_ipv6_get_block_verify(
          unit,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv6_get_block_unsafe(
          unit,
          block_range,
          src_bind_info,
          routes_status,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv6_get_block()", 0, 0);
}

uint32
  soc_ppd_src_bind_ipv6_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE   *block_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_IP_ROUTING_TABLE_RANGE, block_range_key, 10, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_src_bind_ipv6_get_block_unsafe_verify()", 0, 0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>



