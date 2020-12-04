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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_bmact.h>






















uint32
  soc_ppd_frwrd_bmact_init(
    SOC_SAND_IN  int                                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_BMACT_INIT);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_bmact_init_unsafe(
      unit       
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_init()", 0, 0);
}


uint32
  soc_ppd_frwrd_bmact_bvid_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                        bvid_ndx,
    SOC_SAND_IN  SOC_PPC_BMACT_BVID_INFO                    *bvid_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_BMACT_BVID_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bvid_info);

  res = arad_pp_frwrd_bmact_bvid_info_set_verify(
          unit,
          bvid_ndx,
          bvid_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_bmact_bvid_info_set_unsafe(
          unit,
          bvid_ndx,
          bvid_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_bvid_info_set()", bvid_ndx, 0);
}


uint32
  soc_ppd_frwrd_bmact_bvid_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                        bvid_ndx,
    SOC_SAND_OUT SOC_PPC_BMACT_BVID_INFO                    *bvid_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_BMACT_BVID_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bvid_info);

  res = arad_pp_frwrd_bmact_bvid_info_get_verify(
          unit,
          bvid_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_bmact_bvid_info_get_unsafe(
          unit,
          bvid_ndx,
          bvid_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_bvid_info_get()", bvid_ndx, 0);
}


uint32
  soc_ppd_frwrd_bmact_pbb_te_bvid_range_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_BMACT_PBB_TE_VID_RANGE             *pbb_te_bvids
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pbb_te_bvids);

  res = arad_pp_frwrd_bmact_pbb_te_bvid_range_set_verify(
          unit,
          pbb_te_bvids
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_bmact_pbb_te_bvid_range_set_unsafe(
          unit,
          pbb_te_bvids
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_pbb_te_bvid_range_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_bmact_pbb_te_bvid_range_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PPC_BMACT_PBB_TE_VID_RANGE             *pbb_te_bvids
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pbb_te_bvids);

  res = arad_pp_frwrd_bmact_pbb_te_bvid_range_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_bmact_pbb_te_bvid_range_get_unsafe(
          unit,
          pbb_te_bvids
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_pbb_te_bvid_range_get()", 0, 0);
}


uint32
  soc_ppd_frwrd_bmact_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_KEY                    *bmac_key,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_INFO                   *bmact_entry_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                   *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_BMACT_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bmac_key);
  SOC_SAND_CHECK_NULL_INPUT(bmact_entry_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_bmact_entry_add_verify(
          unit,
          bmac_key,
          bmact_entry_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_bmact_entry_add_unsafe(
          unit,
          bmac_key,
          bmact_entry_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_entry_add()", 0, 0);
}


uint32
  soc_ppd_frwrd_bmact_entry_remove(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_BMACT_ENTRY_KEY                    *bmac_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_BMACT_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bmac_key);

  res = arad_pp_frwrd_bmact_entry_remove_verify(
          unit,
          bmac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_bmact_entry_remove_unsafe(
          unit,
          bmac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_entry_remove()", 0, 0);
}


uint32
  soc_ppd_frwrd_bmact_entry_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_INOUT  SOC_PPC_BMACT_ENTRY_KEY                    *bmac_key,
    SOC_SAND_OUT SOC_PPC_BMACT_ENTRY_INFO                   *bmact_entry_info,
    SOC_SAND_OUT uint8                                      *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_BMACT_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bmac_key);

  res = arad_pp_frwrd_bmact_entry_get_verify(
          unit,
          bmac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_bmact_entry_get_unsafe(
          unit,
          bmac_key,
          bmact_entry_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_entry_get()", 0, 0);
}

uint32
  soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_bmact_eg_vlan_pcp_map_set_verify(
          unit,
          tc_ndx,
          dp_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_bmact_eg_vlan_pcp_map_set_unsafe(
          unit,
          tc_ndx,
          dp_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set()", 0, 0);
}

uint32
  soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  res = arad_pp_frwrd_bmact_eg_vlan_pcp_map_get_verify(
          unit,
          tc_ndx,
          dp_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_bmact_eg_vlan_pcp_map_get_unsafe(
          unit,
          tc_ndx,
          dp_ndx,
          out_pcp,
          out_dei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get()", 0, 0);
}
#include <soc/dpp/SAND/Utils/sand_footer.h>


