/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_LLP



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_cos.h>






















uint32
  soc_ppd_llp_cos_glbl_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_GLBL_INFO                   *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = arad_pp_llp_cos_glbl_info_set_verify(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_glbl_info_set_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_glbl_info_set()", 0, 0);
}


uint32
  soc_ppd_llp_cos_glbl_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_LLP_COS_GLBL_INFO                   *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = arad_pp_llp_cos_glbl_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_glbl_info_get_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_glbl_info_get()", 0, 0);
}


uint32
  soc_ppd_llp_cos_port_info_set(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  int                                         core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_INFO                   *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_pp_llp_cos_port_info_set_verify(
          unit,
          local_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_port_info_set_unsafe(
          unit,
          core_id,
          local_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_port_info_set()", local_port_ndx, 0);
}


uint32
  soc_ppd_llp_cos_port_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_INFO                   *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_pp_llp_cos_port_info_get_verify(
          unit,
          local_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_port_info_get_unsafe(
          unit,
          core_id,
          local_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_port_info_get()", local_port_ndx, 0);
}


uint32
  soc_ppd_llp_cos_mapping_table_entry_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(entry);

  res = arad_pp_llp_cos_mapping_table_entry_set_verify(
          unit,
          mapping_tbl_ndx,
          table_id_ndx,
          entry_ndx,
          entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_mapping_table_entry_set_unsafe(
          unit,
          mapping_tbl_ndx,
          table_id_ndx,
          entry_ndx,
          entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_mapping_table_entry_set()", 0, table_id_ndx);
}


uint32
  soc_ppd_llp_cos_mapping_table_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(entry);

  res = arad_pp_llp_cos_mapping_table_entry_get_verify(
          unit,
          mapping_tbl_ndx,
          table_id_ndx,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_mapping_table_entry_get_unsafe(
          unit,
          mapping_tbl_ndx,
          table_id_ndx,
          entry_ndx,
          entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_mapping_table_entry_get()", 0, table_id_ndx);
}


uint32
  soc_ppd_llp_cos_l4_port_range_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  res = arad_pp_llp_cos_l4_port_range_info_set_verify(
          unit,
          range_ndx,
          range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_l4_port_range_info_set_unsafe(
          unit,
          range_ndx,
          range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_l4_port_range_info_set()", range_ndx, 0);
}


uint32
  soc_ppd_llp_cos_l4_port_range_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  res = arad_pp_llp_cos_l4_port_range_info_get_verify(
          unit,
          range_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_l4_port_range_info_get_unsafe(
          unit,
          range_ndx,
          range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_l4_port_range_info_get()", range_ndx, 0);
}


uint32
  soc_ppd_llp_cos_l4_port_range_to_tc_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_TC_INFO                     *tc_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tc_info);

  res = arad_pp_llp_cos_l4_port_range_to_tc_info_set_verify(
          unit,
          l4_prtcl_type_ndx,
          src_port_match_range_ndx,
          dest_port_match_range_ndx,
          tc_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_l4_port_range_to_tc_info_set_unsafe(
          unit,
          l4_prtcl_type_ndx,
          src_port_match_range_ndx,
          dest_port_match_range_ndx,
          tc_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_l4_port_range_to_tc_info_set()", 0, src_port_match_range_ndx);
}


uint32
  soc_ppd_llp_cos_l4_port_range_to_tc_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_TC_INFO                     *tc_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tc_info);

  res = arad_pp_llp_cos_l4_port_range_to_tc_info_get_verify(
          unit,
          l4_prtcl_type_ndx,
          src_port_match_range_ndx,
          dest_port_match_range_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_l4_port_range_to_tc_info_get_unsafe(
          unit,
          l4_prtcl_type_ndx,
          src_port_match_range_ndx,
          dest_port_match_range_ndx,
          tc_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_l4_port_range_to_tc_info_get()", 0, src_port_match_range_ndx);
}


uint32
  soc_ppd_llp_cos_ipv4_subnet_based_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(subnet);
  SOC_SAND_CHECK_NULL_INPUT(subnet_based_info);

  res = arad_pp_llp_cos_ipv4_subnet_based_set_verify(
          unit,
          entry_ndx,
          subnet,
          subnet_based_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_ipv4_subnet_based_set_unsafe(
          unit,
          entry_ndx,
          subnet,
          subnet_based_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_ipv4_subnet_based_set()", entry_ndx, 0);
}


uint32
  soc_ppd_llp_cos_ipv4_subnet_based_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_OUT SOC_PPC_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_IPV4_SUBNET_BASED_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(subnet);
  SOC_SAND_CHECK_NULL_INPUT(subnet_based_info);

  res = arad_pp_llp_cos_ipv4_subnet_based_get_verify(
          unit,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_ipv4_subnet_based_get_unsafe(
          unit,
          entry_ndx,
          subnet,
          subnet_based_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_ipv4_subnet_based_get()", entry_ndx, 0);
}


uint32
  soc_ppd_llp_cos_protocol_based_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                        ether_type_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_COS_PRTCL_INFO                  *prtcl_assign_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_PROTOCOL_BASED_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prtcl_assign_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_llp_cos_protocol_based_set_verify(
          unit,
          profile_ndx,
          ether_type_ndx,
          prtcl_assign_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_protocol_based_set_unsafe(
          unit,
          profile_ndx,
          ether_type_ndx,
          prtcl_assign_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_protocol_based_set()", profile_ndx, ether_type_ndx);
}


uint32
  soc_ppd_llp_cos_protocol_based_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                        ether_type_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_COS_PRTCL_INFO                  *prtcl_assign_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_COS_PROTOCOL_BASED_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prtcl_assign_info);

  res = arad_pp_llp_cos_protocol_based_get_verify(
          unit,
          profile_ndx,
          ether_type_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_cos_protocol_based_get_unsafe(
          unit,
          profile_ndx,
          ether_type_ndx,
          prtcl_assign_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_protocol_based_get()", profile_ndx, ether_type_ndx);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>



