
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>
#include <soc/dpp/PPC/ppc_api_llp_mirror.h>






















uint32
  soc_ppd_eg_encap_entry_init(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                     lif_eep_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_entry_init_unsafe(
          unit,
          lif_eep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_entry_init()", 0, 0);
}



uint32
  soc_ppd_eg_encap_range_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_RANGE_INFO                     *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_RANGE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  res = arad_pp_eg_encap_range_info_set_verify(
          unit,
          range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_range_info_set_unsafe(
          unit,
          range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_range_info_set()", 0, 0);
}


uint32
  soc_ppd_eg_encap_range_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_RANGE_INFO                     *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_RANGE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  res = arad_pp_eg_encap_range_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_range_info_get_unsafe(
          unit,
          range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_range_info_get()", 0, 0);
}


uint32 soc_ppd_eg_encap_protection_info_set(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  int                                    out_lif_id,
    SOC_SAND_IN  uint8                                  is_full_entry_extension,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PROTECTION_INFO       *protection_info)
{
  uint32 res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  if (protection_info->is_protection_valid)
  {
      res = arad_pp_eg_encap_protection_info_set_verify(unit, protection_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      SOC_SAND_TAKE_DEVICE_SEMAPHORE;

      res = arad_pp_eg_encap_protection_info_set_unsafe(unit, out_lif_id,
                is_full_entry_extension, protection_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
  }
  else
  {
      ARAD_PP_DO_NOTHING_AND_EXIT;
  }
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_protection_info_set()", 0, 0);
}


uint32
  soc_ppd_eg_encap_null_lif_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lif_eep_ndx,
    SOC_SAND_IN  uint32                                      next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_encap_null_lif_entry_add_verify(
          unit,
          lif_eep_ndx,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_null_lif_entry_add_unsafe(
          unit,
          lif_eep_ndx,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_null_lif_entry_add()", lif_eep_ndx, 0);
}



uint32
  soc_ppd_eg_encap_lif_field_set(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                                lif_eep_ndx,
      SOC_SAND_IN  uint32                                flags,
      SOC_SAND_IN  uint32                                val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_encap_lif_field_set_verify(
          unit,
          lif_eep_ndx,
          flags,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_lif_field_set_unsafe(
          unit,
          lif_eep_ndx,
          flags,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_null_lif_entry_add()", lif_eep_ndx, 0);
}



uint32
  soc_ppd_eg_encap_lif_field_get(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  uint32                                lif_eep_ndx,
      SOC_SAND_IN  uint32                                flags,
      SOC_SAND_OUT  uint32                               *val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_lif_field_get_unsafe(
          unit,
          lif_eep_ndx,
          flags,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_lif_field_get()", lif_eep_ndx, flags);
}


uint32
  soc_ppd_eg_encap_data_local_to_global_cud_init(
    SOC_SAND_IN  int                                 unit
  )
{
    uint32 
        res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_INIT);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_eg_encap_data_local_to_global_cud_init_verify(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_eg_encap_data_local_to_global_cud_init_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_data_local_to_global_cud_init()", 0, 0);
}



uint32
  soc_ppd_eg_encap_data_local_to_global_cud_set(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  in_cud,
    SOC_SAND_IN  uint32                                  out_cud
  )
{
    uint32 
        res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_SET);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_eg_encap_data_local_to_global_cud_set_verify(unit,in_cud,out_cud);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
    res = arad_pp_eg_encap_data_local_to_global_cud_set_unsafe(unit,in_cud,out_cud);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_data_local_to_global_cud_set()", in_cud, out_cud);
}



uint32
  soc_ppd_eg_encap_data_local_to_global_cud_get(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  in_cud,
    SOC_SAND_OUT uint32*                                 out_cud
  )
{
    uint32 
        res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_GET);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_eg_encap_data_local_to_global_cud_get_verify(unit,in_cud,out_cud);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pp_eg_encap_data_local_to_global_cud_get_unsafe(unit,in_cud,out_cud);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_data_local_to_global_cud_get()", in_cud, 0);
}


uint32
  soc_ppd_eg_encap_data_lif_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO              *data_info,
    SOC_SAND_IN  uint8                                   next_eep_valid,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(data_info);

  res = arad_pp_eg_encap_data_lif_entry_add_verify(
          unit,
          lif_eep_ndx,
          data_info,
          next_eep_valid,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_data_lif_entry_add_unsafe(
          unit,
          lif_eep_ndx,
          data_info,
          next_eep_valid,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_null_lif_entry_add()", lif_eep_ndx, 0);
}


uint32
  soc_ppd_eg_encap_swap_command_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_SWAP_INFO                      *swap_info,
    SOC_SAND_IN  uint32                                      next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(swap_info);

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_swap_command_entry_add_verify(
          unit,
          lif_eep_ndx,
          swap_info,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_eg_encap_swap_command_entry_add_unsafe(
          unit,
          lif_eep_ndx,
          swap_info,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_swap_command_entry_add()", lif_eep_ndx, 0);
}


uint32
  soc_ppd_eg_encap_pwe_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_INFO                       *pwe_info,
    SOC_SAND_IN  uint32                                      next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_PWE_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pwe_info);

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_pwe_entry_add_verify(
          unit,
          lif_eep_ndx,
          pwe_info,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_eg_encap_pwe_entry_add_unsafe(
          unit,
          lif_eep_ndx,
          pwe_info,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_pwe_entry_add()", lif_eep_ndx, 0);
}


uint32
  soc_ppd_eg_encap_pop_command_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INFO                       *pop_info,
    SOC_SAND_IN  uint32                                      next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pop_info);

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_pop_command_entry_add_verify(
          unit,
          lif_eep_ndx,
          pop_info,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_eg_encap_pop_command_entry_add_unsafe(
          unit,
          lif_eep_ndx,
          pop_info,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_pop_command_entry_add()", lif_eep_ndx, 0);
}


uint32
  soc_ppd_eg_encap_vsi_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lif_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO                 *vsi_info, 
    SOC_SAND_IN  uint8                                     next_eep_valid,
    SOC_SAND_IN  uint32                                      next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_VSI_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_vsi_entry_add_verify(
          unit,
          lif_eep_ndx,
          vsi_info,
          next_eep_valid,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_eg_encap_vsi_entry_add_unsafe(
          unit,
          lif_eep_ndx,
          vsi_info,
          next_eep_valid,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_vsi_entry_add()", lif_eep_ndx, 0);
}


uint32
  soc_ppd_eg_encap_mpls_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO                *mpls_encap_info,
    SOC_SAND_IN  uint32                                      ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_MPLS_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mpls_encap_info);

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_mpls_entry_add_verify(
          unit,
          tunnel_eep_ndx,
          mpls_encap_info,
          ll_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_eg_encap_mpls_entry_add_unsafe(
          unit,
          tunnel_eep_ndx,
          mpls_encap_info,
          ll_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_mpls_entry_add()", tunnel_eep_ndx, 0);
}


uint32
  soc_ppd_eg_encap_ipv4_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO                *ipv4_encap_info,
    SOC_SAND_IN  uint32                                      ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ipv4_encap_info);

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_ipv4_entry_add_verify(
          unit,
          tunnel_eep_ndx,
          ipv4_encap_info,
          ll_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_eg_encap_ipv4_entry_add_unsafe(
          unit,
          tunnel_eep_ndx,
          ipv4_encap_info,
          ll_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_entry_add()", tunnel_eep_ndx, 0);
}


uint32
  soc_ppd_eg_encap_ipv6_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      tunnel_eep_ndx,
    SOC_SAND_INOUT  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO         *ipv6_encap_info,
    SOC_SAND_IN  uint32                                      ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV6_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ipv6_encap_info);

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_ipv6_entry_add_verify(
          unit,
          tunnel_eep_ndx,
          ipv6_encap_info,
          ll_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_eg_encap_ipv6_entry_add_unsafe(
          unit,
          tunnel_eep_ndx,
          ipv6_encap_info,
          ll_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv6_entry_add()", tunnel_eep_ndx, 0);
}



    uint32 
   soc_ppd_eg_encap_overlay_arp_data_entry_add(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint32                                 overlay_ll_eep_ndx, 
     SOC_SAND_INOUT  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *ll_encap_info
   )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ll_encap_info);

  if (SOC_IS_JERICHO(unit)) {
      res = soc_jer_pp_eg_encap_overlay_arp_data_entry_add(unit, overlay_ll_eep_ndx, ll_encap_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else {
      res = arad_pp_eg_encap_overlay_arp_data_entry_add_verify(
              unit,
              overlay_ll_eep_ndx,
              ll_encap_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      SOC_SAND_TAKE_DEVICE_SEMAPHORE;

      res = arad_pp_eg_encap_overlay_arp_data_entry_add_unsafe(
              unit,
              overlay_ll_eep_ndx,
              ll_encap_info
            );
 
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
      SOC_SAND_GIVE_DEVICE_SEMAPHORE;
      ARAD_PP_DO_NOTHING_AND_EXIT;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_overlay_arp_data_entry_add()", overlay_ll_eep_ndx, 0);
}



uint32
  soc_ppd_eg_encap_mirror_entry_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_MIRROR_ENTRY_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mirror_encap_info);

  res = arad_pp_eg_encap_mirror_entry_set_verify(
          unit,
          mirror_ndx,
          mirror_encap_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_mirror_entry_set_unsafe(
          unit,
          mirror_ndx,
          mirror_encap_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_mirror_entry_set()", mirror_ndx, 0);
}


uint32
  soc_ppd_eg_encap_mirror_entry_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_MIRROR_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mirror_encap_info);

  SOC_SAND_ERR_IF_ABOVE_MAX(mirror_ndx, DPP_MIRROR_ACTION_NDX_MAX, ARAD_PP_EG_ENCAP_MIRROR_ID_OUT_OF_RANGE_ERR, 5, exit);

  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_clear(mirror_encap_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_mirror_entry_get_unsafe(
          unit,
          mirror_ndx,
          mirror_encap_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_mirror_entry_get()", mirror_ndx, 0);
}


uint32
  soc_ppd_eg_encap_port_erspan_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  is_erspan
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_PORT_ERSPAN_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_NOF_LOCAL_PORTS(unit), ARAD_FAP_PORT_ID_INVALID_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(is_erspan, 1, ARAD_PP_EG_ENCAP_IS_ERSPAN_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_port_erspan_set_unsafe(
          unit,
          core_id,
          local_port_ndx,
          is_erspan
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_port_erspan_set()", local_port_ndx, is_erspan);
}


uint32
  soc_ppd_eg_encap_port_erspan_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *is_erspan
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_PORT_ERSPAN_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_NOF_LOCAL_PORTS(unit), ARAD_FAP_PORT_ID_INVALID_ERR, 10, exit);
  SOC_SAND_CHECK_NULL_INPUT(is_erspan);


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_port_erspan_get_unsafe(
          unit,
          core_id,
          local_port_ndx,
          is_erspan
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_port_erspan_get()", local_port_ndx, 0);
}


uint32
  soc_ppd_eg_encap_ll_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                ll_eep_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_LL_INFO                 *ll_encap_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_LL_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ll_encap_info);

  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_ll_entry_add_verify(
          unit,
          ll_eep_ndx,
          ll_encap_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_eg_encap_ll_entry_add_unsafe(
          unit,
          ll_eep_ndx,
          ll_encap_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ll_entry_add()", ll_eep_ndx, 0);
}


uint32
  soc_ppd_eg_encap_entry_remove(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE                       eep_type_ndx,
    SOC_SAND_IN  uint32                                      eep_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_entry_remove_verify(
          unit,
          eep_type_ndx,
          eep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_pp_eg_encap_entry_remove_unsafe(
          unit,
          eep_type_ndx,
          eep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_entry_remove()", 0, eep_ndx);
}


uint32
  soc_ppd_eg_encap_entry_type_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 eep_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_TYPE            *entry_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_ENTRY_TYPE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(entry_type);

  res = arad_pp_eg_encap_entry_type_get_verify(
          unit,
          eep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_entry_type_get_unsafe(
          unit,
          eep_ndx,
          entry_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_entry_type_get()", 0, eep_ndx);
}


uint32
  soc_ppd_eg_encap_entry_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE                       eep_type_ndx,
    SOC_SAND_IN  uint32                                      eep_ndx,
    SOC_SAND_IN  uint32                                      depth,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO                     encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX],
    SOC_SAND_OUT uint32                                      next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX],
    SOC_SAND_OUT uint32                                      *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_eg_encap_entry_get_verify(
          unit,
          eep_type_ndx,
          eep_ndx,
          depth
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_entry_get_unsafe(
          unit,
          eep_type_ndx,
          eep_ndx,
          depth,
          encap_entry_info,
          next_eep,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_entry_get()", 0, eep_ndx);
}







uint32
soc_ppd_eg_encap_entry_data_info_to_overlay_arp_encap_info(
 SOC_SAND_IN  int                                    unit, 
   SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO                 *encap_entry_info
   )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(encap_entry_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_entry_data_info_to_overlay_arp_encap_info_unsafe(
          unit,
          encap_entry_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_entry_data_info_to_overlay_arp_encap_info()", 0, 0);
}


uint32
  soc_ppd_eg_encap_push_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO              *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_eg_encap_push_profile_info_set_verify(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_push_profile_info_set_unsafe(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_push_profile_info_set()", profile_ndx, 0);
}


uint32
  soc_ppd_eg_encap_push_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO              *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_eg_encap_push_profile_info_get_verify(
          unit,
          profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_push_profile_info_get_unsafe(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_push_profile_info_get()", profile_ndx, 0);
}


uint32
  soc_ppd_eg_encap_push_exp_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY                   *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                          exp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(exp_key);

  res = arad_pp_eg_encap_push_exp_info_set_verify(
          unit,
          exp_key,
          exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_push_exp_info_set_unsafe(
          unit,
          exp_key,
          exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_push_exp_info_set()", 0, 0);
}


uint32
  soc_ppd_eg_encap_push_exp_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY                   *exp_key,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_EXP                          *exp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_PUSH_EXP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(exp_key);
  SOC_SAND_CHECK_NULL_INPUT(exp);

  res = arad_pp_eg_encap_push_exp_info_get_verify(
          unit,
          exp_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_push_exp_info_get_unsafe(
          unit,
          exp_key,
          exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_push_exp_info_get()", 0, 0);
}


uint32
  soc_ppd_eg_encap_pwe_glbl_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO                  *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = arad_pp_eg_encap_pwe_glbl_info_set_verify(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_pwe_glbl_info_set_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_pwe_glbl_info_set()", 0, 0);
}


uint32
  soc_ppd_eg_encap_pwe_glbl_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PWE_GLBL_INFO                  *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = arad_pp_eg_encap_pwe_glbl_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_pwe_glbl_info_get_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_pwe_glbl_info_get()", 0, 0);
}


uint32
  soc_ppd_eg_encap_glbl_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_GLBL_INFO                  *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = arad_pp_eg_encap_glbl_info_set_verify(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_glbl_info_set_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_glbl_info_set()", 0, 0);
}


uint32
  soc_ppd_eg_encap_glbl_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_GLBL_INFO                  *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = arad_pp_eg_encap_glbl_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_glbl_info_get_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_glbl_info_get()", 0, 0);
}


uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      entry_ndx,
    SOC_SAND_IN  uint32                                      src_ip
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_verify(
          unit,
          entry_ndx,
          src_ip
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_unsafe(
          unit,
          entry_ndx,
          src_ip
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set()", entry_ndx, 0);
}


uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      entry_ndx,
    SOC_SAND_OUT uint32                                      *src_ip
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_ip);

  res = arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_verify(
          unit,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_unsafe(
          unit,
          entry_ndx,
          src_ip
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get()", entry_ndx, 0);
}


uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_verify(
          unit,
          entry_ndx,
          ttl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_unsafe(
          unit,
          entry_ndx,
          ttl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set()", entry_ndx, 0);
}


uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ttl);

  res = arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_verify(
          unit,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_unsafe(
          unit,
          entry_ndx,
          ttl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get()", entry_ndx, 0);
}


uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          tos
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_encap_ipv4_tunnel_glbl_tos_set_verify(
          unit,
          entry_ndx,
          tos
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_ipv4_tunnel_glbl_tos_set_unsafe(
          unit,
          entry_ndx,
          tos
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set()", entry_ndx, 0);
}


uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_TOS                          *tos
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tos);

  res = arad_pp_eg_encap_ipv4_tunnel_glbl_tos_get_verify(
          unit,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_ipv4_tunnel_glbl_tos_get_unsafe(
          unit,
          entry_ndx,
          tos
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get()", entry_ndx, 0);
}


uint32
  soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_set(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint8                                is_exp_copy
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_set_verify(
          unit,
          is_exp_copy
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_set_unsafe(
          unit,
          is_exp_copy
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_set()", is_exp_copy, 0);
}


uint32
  soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_get(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_OUT uint8                                *is_exp_copy
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_mpls_pipe_mode_is_exp_copy_get_unsafe(
          unit,
          is_exp_copy
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_get()", 0, 0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>



