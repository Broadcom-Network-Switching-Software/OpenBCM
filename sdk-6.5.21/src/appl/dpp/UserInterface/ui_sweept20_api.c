/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#if LINK_TIMNA_LIBRARIES
#include <appl/diag/dpp/ref_sys.h>
#include <bcm_app/dpp/../H/usrApp.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/Timna/timna_api_policer.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/diag/dpp/utils_line_TEVB.h>
#include <appl/dpp/UserInterface/ui_defi.h>

#include <appl/dpp/UserInterface/ui_pure_defi_sweept20_api.h>
#include <Sweep/Timna/sweept20_api_8021ad.h>
#include <Sweep/Timna/sweept20_api_init.h>
#include <Sweep/Timna/sweept20_api_ports.h>
#include <Sweep/Timna/sweept20_api_mac.h>
#include <Sweep/Timna/sweept20_api_vsi.h>
#include <Sweep/Timna/sweept20_api_vlan.h>
#include <Sweep/Timna/sweept20_api_policer.h>
#include <Sweep/Timna/sweept20_api_mpls.h>
#include <Sweep/Timna/sweept20_api_vpls.h>
#include <Sweep/Timna/sweept20_api_acl.h>
#include <Sweep/Timna/sweept20_api_bsp.h>
#include <Sweep/Timna/sweept20_api_sys_info.h>
#include <Sweep/Timna/sweept20_api_profiler.h>
#include <Sweep/Timna/sweep_timna_app.h>

#include <soc/dpp/Timna/timna_api_vid_res.h>
#include <soc/dpp/Timna/timna_api_filter.h>
#include <soc/dpp/Timna/timna_api_edit.h>
#include <soc/dpp/Timna/timna_api_acl.h>
#include <soc/dpp/Timna/timna_api_ipv4_uc.h>
#include <soc/dpp/Timna/timna_api_policer.h>
#include <soc/dpp/Timna/timna_api_ipv4_uc_test.h>
#include <soc/dpp/Timna/timna_api_qinq.h>
#include <soc/dpp/Timna/timna_api_mngr.h>
#include <soc/dpp/Timna/timna_api_policer.h>

#include <DuneApp/timna_app.h>
#include <DuneApp/timna_app_mpls_te.h>


#ifdef UI_SWEEPT20_API_INIT
int
  ui_sweept20_api_init_generic_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_app_device_handle;
  uint32
    prm_pp_id = 0;
  uint32
    prm_p_unit = 0;
  uint8
    is_opened;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_init");
  soc_sand_proc_name = "sweept20_generic_init";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_GENERIC_INIT_GENERIC_INIT_APP_DEVICE_HANDLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_GENERIC_INIT_GENERIC_INIT_APP_DEVICE_HANDLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_app_device_handle = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_GENERIC_INIT_GENERIC_INIT_PP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_GENERIC_INIT_GENERIC_INIT_PP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pp_id = (uint32)param_val->value.ulong_value;
  }

  ret = sweept20_generic_init(
          prm_app_device_handle,
          prm_pp_id,
          &prm_p_unit,
          &is_opened
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_generic_init - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_generic_init - SUCCEEDED", TRUE);
  send_string_to_screen("--> p_unit:", TRUE);
  soc_sand_os_printf( "p_unit: %lu\n",prm_p_unit);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_init_stand_alone_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_init");
  soc_sand_proc_name = "sweept20_stand_alone_init";

  unit = timna_get_default_unit();



  ret = sweept20_stand_alone_init(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_stand_alone_init - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_stand_alone_init - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_init_bridge_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_nof_vlans=0;
  SWEEPT20_MACT_INFO
    prm_default_macs;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_init");
  soc_sand_proc_name = "sweept20_bridge_init";

  unit = timna_get_default_unit();
  sweept20_SWEEPT20_MACT_INFO_clear(&prm_default_macs);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_NOF_VLANS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_NOF_VLANS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_vlans = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_3_LAST_SVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_3_LAST_SVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_default_macs.preconfig_range[3].last_svid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_3_FIRST_SVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_3_FIRST_SVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_default_macs.preconfig_range[3].first_svid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_3_LAST_MAC_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_3_LAST_MAC_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_3_FIRST_MAC_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_3_FIRST_MAC_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_2_LAST_SVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_2_LAST_SVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_default_macs.preconfig_range[2].last_svid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_2_FIRST_SVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_2_FIRST_SVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_default_macs.preconfig_range[2].first_svid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_2_LAST_MAC_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_2_LAST_MAC_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_2_FIRST_MAC_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_2_FIRST_MAC_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_1_LAST_SVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_1_LAST_SVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_default_macs.preconfig_range[1].last_svid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_1_FIRST_SVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_1_FIRST_SVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_default_macs.preconfig_range[1].first_svid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_1_LAST_MAC_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_1_LAST_MAC_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_1_FIRST_MAC_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_1_FIRST_MAC_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_0_LAST_SVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_0_LAST_SVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_default_macs.preconfig_range[0].last_svid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_0_FIRST_SVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_0_FIRST_SVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_default_macs.preconfig_range[0].first_svid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_0_LAST_MAC_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_0_LAST_MAC_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_0_FIRST_MAC_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_PRECONFIG_RANGE_0_FIRST_MAC_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_DEFAULT_MACS_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_default_macs.enable = (uint32)param_val->value.ulong_value;
  }
  timna_app_reset(FALSE);

  ret = timna_app_bridge_init(
          prm_nof_vlans,
          &prm_default_macs
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_bridge_init - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_bridge_init - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_init_ad_peb_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_nof_svlans = 1000;
  uint32
    prm_nof_cvlans = 4000;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_init");
  soc_sand_proc_name = "sweept20_ad_peb_init";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_AD_PEB_INIT_AD_PEB_INIT_NOF_SVLANS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_AD_PEB_INIT_AD_PEB_INIT_NOF_SVLANS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_svlans = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_AD_PEB_INIT_AD_PEB_INIT_NOF_CVLANS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_AD_PEB_INIT_AD_PEB_INIT_NOF_CVLANS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_cvlans = (uint32)param_val->value.ulong_value;
  }

  timna_app_reset(FALSE);

  ret = timna_app_ad_peb_init(
          prm_nof_svlans,
          prm_nof_cvlans
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_ad_peb_init - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_ad_peb_init - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_init_vpls_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_nof_vsis = 4094;
  uint32
    prm_nof_vcs_per_vsi = 10;
  SWEEPT20_POLICE_OP
    prm_customer_policer;
  SWEEPT20_POLICE_OP
    prm_layer3_policer;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_init");
  soc_sand_proc_name = "sweept20_vpls_init";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_NOF_VSIS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_NOF_VSIS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_vsis = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_NOF_VCS_PER_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_NOF_VCS_PER_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_vcs_per_vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_CUSTOMER_POLICER_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_CUSTOMER_POLICER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_customer_policer = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_LAYER3_POLICER_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_LAYER3_POLICER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_layer3_policer = param_val->numeric_equivalent;
  }

  timna_app_reset(FALSE);

  ret = timna_app_vpls_init(
          prm_nof_vsis,
          prm_nof_vcs_per_vsi,
          prm_customer_policer,
          prm_layer3_policer
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_vpls_init - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_vpls_init - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_init_mpls_lsr_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_nof_lsr_labels=1000;
  uint32
    prm_nof_encap_options=1;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_init");
  soc_sand_proc_name = "sweept20_mpls_lsr_init";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MPLS_LSR_INIT_MPLS_LSR_INIT_NOF_LSR_LABELS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MPLS_LSR_INIT_MPLS_LSR_INIT_NOF_LSR_LABELS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_lsr_labels = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MPLS_LSR_INIT_MPLS_LSR_INIT_NOF_ENCAP_OPTIONS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MPLS_LSR_INIT_MPLS_LSR_INIT_NOF_ENCAP_OPTIONS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_encap_options = (uint32)param_val->value.ulong_value;
  }

  timna_app_reset(FALSE);

  ret = sweept20_mpls_lsr_init(
          unit,
          prm_nof_lsr_labels,
          prm_nof_encap_options
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_mpls_lsr_init - FAIL", TRUE);
    sweept20_disp_result(ret, "sweept20_mpls_lsr_init");
    goto exit;
  }

  send_string_to_screen(" *** sweept20_mpls_lsr_init - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_init_ipv4_routing_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_nof_vrfs = 1;
  uint32
    prm_nof_outlifs = 1000;
  uint32
    prm_vrf_table_size = 1000;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_init");
  soc_sand_proc_name = "sweept20_api_ipv4_routing_init";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_ROUTING_INIT_IPV4_ROUTING_INIT_NOF_VRFS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_ROUTING_INIT_IPV4_ROUTING_INIT_NOF_VRFS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_vrfs = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_ROUTING_INIT_IPV4_ROUTING_INIT_START_VRF_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_ROUTING_INIT_IPV4_ROUTING_INIT_START_VRF_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_outlifs = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_ROUTING_INIT_IPV4_ROUTING_INIT_VRF_TABLE_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_ROUTING_INIT_IPV4_ROUTING_INIT_VRF_TABLE_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vrf_table_size = (uint32)param_val->value.ulong_value;
  }
  timna_app_reset(FALSE);

  ret = sweept20_ipv4_routing_init(
          unit,
          prm_nof_vrfs,
          prm_nof_outlifs,
          prm_vrf_table_size
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_ipv4_routing_init - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_ipv4_routing_init - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_init_acl_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_init");
  soc_sand_proc_name = "sweept20_api_init_acl_init";

  unit = timna_get_default_unit();

  timna_app_reset(FALSE);

  ret = sweept20_acl_init(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_acl_init - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_acl_init - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_init_enable_traffic(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_do_enable;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_init");
  soc_sand_proc_name = "sweept20_enable_traffic";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ENABLE_TRAFFIC_ENABLE_TRAFFIC_DO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ENABLE_TRAFFIC_ENABLE_TRAFFIC_DO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_do_enable = (uint8)param_val->value.ulong_value;
  }

  ret = sweept20_enable_traffic(
          unit,
          prm_do_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_enable_traffic - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_enable_traffic - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_init_close(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_init");
  soc_sand_proc_name = "sweept20_close";

  unit = timna_get_default_unit();



  ret = sweept20_close(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_close - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_close - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_PORTS
int
  ui_sweept20_api_ports_mapping_to_sys_ports(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_sys_dev_ndx = 0;
  uint32
    prm_local_port_ndx = 0;
  uint32
    prm_system_phy_port;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_ports");
  soc_sand_proc_name = "sweept20_ports_mapping_to_sys_ports";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PORTS_MAPPING_TO_SYS_PORTS_MAPPING_TO_SYS_PORTS_SYS_DEV_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_PORTS_MAPPING_TO_SYS_PORTS_MAPPING_TO_SYS_PORTS_SYS_DEV_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_dev_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_dev_ndx after mapping_to_sys_ports***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PORTS_MAPPING_TO_SYS_PORTS_MAPPING_TO_SYS_PORTS_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_PORTS_MAPPING_TO_SYS_PORTS_MAPPING_TO_SYS_PORTS_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after mapping_to_sys_ports***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PORTS_MAPPING_TO_SYS_PORTS_MAPPING_TO_SYS_PORTS_SYSTEM_PHY_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_PORTS_MAPPING_TO_SYS_PORTS_MAPPING_TO_SYS_PORTS_SYSTEM_PHY_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_system_phy_port = (uint32)param_val->value.ulong_value;
  }

  ret = sweept20_ports_mapping_to_sys_ports(
          unit,
          prm_sys_dev_ndx,
          prm_local_port_ndx,
          prm_system_phy_port
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_ports_mapping_to_sys_ports - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_ports_mapping_to_sys_ports - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_ports_map_to_lag(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_lag_ndx = 0;
  SWEEPT20_API_PORTS_LAG_INFO
    prm_lag_info;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_ports");
  soc_sand_proc_name = "sweept20_ports_map_to_lag";

  unit = timna_get_default_unit();
  sweept20_SWEEPT20_API_PORTS_LAG_INFO_clear(&prm_lag_info);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lag_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lag_ndx after map_to_lag***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_SYS_PHY_PORTS_3_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_SYS_PHY_PORTS_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lag_info.sys_phy_ports[3] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_SYS_PHY_PORTS_2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_SYS_PHY_PORTS_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lag_info.sys_phy_ports[2] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_SYS_PHY_PORTS_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_SYS_PHY_PORTS_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lag_info.sys_phy_ports[1] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_SYS_PHY_PORTS_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_SYS_PHY_PORTS_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lag_info.sys_phy_ports[0] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_NOF_PORST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_NOF_PORST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lag_info.nof_porst = (uint32)param_val->value.ulong_value;
  }

  ret = sweept20_ports_map_to_lag(
          unit,
          prm_lag_ndx,
          &prm_lag_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_ports_map_to_lag - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_ports_map_to_lag - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_MAC
int
  ui_sweept20_api_mac_learn_agent(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_mac");
  soc_sand_proc_name = "sweept20_mac_learn_agent";

  unit = timna_get_default_unit();



  ret = sweept20_mac_learn_agent(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_mac_learn_agent - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_mac_learn_agent - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_mac_process_cpu_msg(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_mac");
  soc_sand_proc_name = "sweept20_mac_process_cpu_msg";

  unit = timna_get_default_unit();



  ret = sweept20_mac_process_cpu_msg(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_mac_process_cpu_msg - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_mac_process_cpu_msg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_mac_range_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SWEEPT20_API_MACT_RANGE
    prm_keys_range;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_mac");
  soc_sand_proc_name = "sweept20_mac_range_add";

  unit = timna_get_default_unit();
  sweept20_SWEEPT20_API_MACT_RANGE_clear(&prm_keys_range);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_KEYS_RANGE_LAST_SVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_KEYS_RANGE_LAST_SVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_keys_range.last_svid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_KEYS_RANGE_FIRST_SVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_KEYS_RANGE_FIRST_SVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_keys_range.first_svid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_KEYS_RANGE_LAST_MAC_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_KEYS_RANGE_LAST_MAC_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_KEYS_RANGE_FIRST_MAC_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_KEYS_RANGE_FIRST_MAC_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

  }

  ret = sweept20_mac_range_add(
          unit,
          &prm_keys_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_mac_range_add - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_mac_range_add - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_mac_range_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SWEEPT20_API_MACT_RANGE
    prm_keys_range;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_mac");
  soc_sand_proc_name = "sweept20_mac_range_remove";

  unit = timna_get_default_unit();
  sweept20_SWEEPT20_API_MACT_RANGE_clear(&prm_keys_range);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_KEYS_RANGE_LAST_SVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_KEYS_RANGE_LAST_SVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_keys_range.last_svid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_KEYS_RANGE_FIRST_SVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_KEYS_RANGE_FIRST_SVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_keys_range.first_svid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_KEYS_RANGE_LAST_MAC_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_KEYS_RANGE_LAST_MAC_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_KEYS_RANGE_FIRST_MAC_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_KEYS_RANGE_FIRST_MAC_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

  }

  ret = sweept20_mac_range_remove(
          unit,
          &prm_keys_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_mac_range_remove - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_mac_range_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_VSI
int
  ui_sweept20_api_vsi_open(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vsid_ndx = 0,
    vsi_i,
    last_vsid_ndx,
    param_i;
  SWEEPT20_VSI_INFO
    prm_service_info;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vsi");
  soc_sand_proc_name = "sweept20_vsi_open";

  unit = timna_get_default_unit();
  sweept20_SWEEPT20_VSI_INFO_clear(&prm_service_info);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_OPEN_OPEN_VSID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_OPEN_OPEN_VSID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsid_ndx = (uint32)param_val->value.ulong_value;
    last_vsid_ndx = prm_vsid_ndx;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsid_ndx after open***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_OPEN_OPEN_LAST_VSID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_OPEN_OPEN_LAST_VSID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    last_vsid_ndx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_OPEN_OPEN_SERVICE_INFO_MSTP_TOPOLOGY_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_OPEN_OPEN_SERVICE_INFO_MSTP_TOPOLOGY_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_service_info.mstp_topology_id = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_OPEN_OPEN_SERVICE_INFO_EGRESS_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_OPEN_OPEN_SERVICE_INFO_EGRESS_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_service_info.egress_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_OPEN_OPEN_SERVICE_INFO_ENABLE_FLOODING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_OPEN_OPEN_SERVICE_INFO_ENABLE_FLOODING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_service_info.enable_flooding = (uint8)param_val->value.ulong_value;
  }

  for (param_i=1; param_i<=SWEEPT20_VSI_MAX_PORTS_IN_VSI; ++param_i)
  {
    ui_ret = get_val_of( current_line, (int *)&match_index,
      PARAM_SWEEPT20_API_VSI_OPEN_OPEN_SERVICE_INFO_PORTS_LIST_0_ID, param_i,
      &param_val, VAL_NUMERIC, err_msg
      );
    if (0 != ui_ret)
    {
      ui_ret=0;
      break;
    }
    prm_service_info.ports_list[param_i-1] = param_val->value.ulong_value;
  }
  prm_service_info.nof_ports = param_i-1;

  for(vsi_i=prm_vsid_ndx; vsi_i<=last_vsid_ndx;vsi_i++)
  {
    ret = sweept20_vsi_open(
      unit,
      vsi_i,
      &prm_service_info
      );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** sweept20_vsi_open - FAIL", TRUE);

      goto exit;
    }
  }

  send_string_to_screen(" *** sweept20_vsi_open - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_vsi_update(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vsid_ndx = 0;
  SWEEPT20_VSI_INFO
    prm_service_info;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vsi");
  soc_sand_proc_name = "sweept20_vsi_update";

  unit = timna_get_default_unit();
  sweept20_SWEEPT20_VSI_INFO_clear(&prm_service_info);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_VSID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_VSID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsid_ndx after update***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_MSTP_TOPOLOGY_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_MSTP_TOPOLOGY_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_service_info.mstp_topology_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_NOF_PORTS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_NOF_PORTS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_service_info.nof_ports = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_PORTS_LIST_3_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_PORTS_LIST_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_service_info.ports_list[3] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_PORTS_LIST_2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_PORTS_LIST_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_service_info.ports_list[2] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_PORTS_LIST_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_PORTS_LIST_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_service_info.ports_list[1] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_PORTS_LIST_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_PORTS_LIST_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_service_info.ports_list[0] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_EGRESS_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_EGRESS_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_service_info.egress_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_ENABLE_FLOODING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_SERVICE_INFO_ENABLE_FLOODING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_service_info.enable_flooding = (uint8)param_val->value.ulong_value;
  }

  ret = sweept20_vsi_update(
          unit,
          prm_vsid_ndx,
          &prm_service_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_vsi_update - FAIL", TRUE);
    sweept20_disp_result(ret, "sweept20_vsi_update");
    goto exit;
  }

  send_string_to_screen(" *** sweept20_vsi_update - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_vsi_info_display(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vsid_ndx = 0;
  uint32
    prm_last_vsid_ndx = 0;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vsi");
  soc_sand_proc_name = "sweept20_vsi_info_display";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_INFO_DISPLAY_INFO_DISPLAY_VSID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_INFO_DISPLAY_INFO_DISPLAY_VSID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsid_ndx after info_display***", TRUE);
    goto exit;
  }

  prm_last_vsid_ndx = prm_vsid_ndx;
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_INFO_DISPLAY_INFO_DISPLAY_LAST_VSID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VSI_INFO_DISPLAY_INFO_DISPLAY_LAST_VSID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_last_vsid_ndx = (uint32)param_val->value.ulong_value;
  }

  ret = sweept20_vsi_info_display(
          unit,
          prm_vsid_ndx,
          prm_last_vsid_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_vsi_info_display - FAIL", TRUE);
    sweept20_disp_result(ret, "sweept20_vsi_info_display");
    goto exit;
  }

  send_string_to_screen(" *** sweept20_vsi_info_display - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_vsi_outlif_info_display(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_lif_ndx = 0;
  uint32
    prm_last_lif_ndx = 0;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vsi");
  soc_sand_proc_name = "sweept20_outlif_info_display";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_OUTLIF_INFO_DISPLAY_OUTLIF_INFO_DISPLAY_LIF_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_OUTLIF_INFO_DISPLAY_OUTLIF_INFO_DISPLAY_LIF_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_ndx = (uint32)param_val->value.ulong_value;
    prm_last_lif_ndx = prm_lif_ndx;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lif_ndx after outlif_info_display***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_OUTLIF_INFO_DISPLAY_OUTLIF_INFO_DISPLAY_LAST_LIF_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_OUTLIF_INFO_DISPLAY_OUTLIF_INFO_DISPLAY_LAST_LIF_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_last_lif_ndx = (uint32)param_val->value.ulong_value;
  }

  ret = sweept20_outlif_info_display(
          unit,
          prm_lif_ndx,
          prm_last_lif_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_outlif_info_display - FAIL", TRUE);
    sweept20_disp_result(ret, "sweept20_outlif_info_display");
    goto exit;
  }

  send_string_to_screen(" *** sweept20_outlif_info_display - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_VLAN
int
  ui_sweept20_api_vlan_ports_add_remove(
    CURRENT_LINE *current_line,
    uint8 remove
  )
{
  uint32
    ret;
  uint32
    prm_vsid_ndx = 0,
    vsi_i,
    prm_vid,
    prm_last_vsid_ndx;
  SWEEPT20_VLAN_INFO
    prm_vlan_info;
  SWEEPT20_VLAN_PORTS_INFO
    prm_ports_list;
  uint32
    param_i;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vlan");
  soc_sand_proc_name = "sweept20_vlan_ports_add";

  unit = timna_get_default_unit();
  sweept20_SWEEPT20_VLAN_INFO_clear(&prm_vlan_info);
  sweept20_SWEEPT20_VLAN_PORTS_INFO_clear(&prm_ports_list);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_VSID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_VSID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsid_ndx = (uint32)param_val->value.ulong_value;
    prm_last_vsid_ndx = prm_vsid_ndx;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsid_ndx after ports_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_LAST_VSID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_LAST_VSID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_last_vsid_ndx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_VLAN_INFO_C_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_VLAN_INFO_C_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_info.c_vid_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_VLAN_INFO_C_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_VLAN_INFO_C_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_info.c_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_VLAN_INFO_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_VLAN_INFO_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_info.vid = (uint32)param_val->value.ulong_value;
  }

  for (param_i=1; param_i<=SWEEPT20_VSI_MAX_PORTS_IN_VSI; ++param_i)
  {
    ui_ret = get_val_of( current_line, (int *)&match_index,
      PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_PORTS_LIST_PORTS_ID, param_i,
      &param_val, VAL_NUMERIC, err_msg
      );
    if (0 != ui_ret)
    {
      ui_ret=0;
      break;
    }
    prm_ports_list.ports_membership[param_i-1] = param_val->value.ulong_value;

    ui_ret = get_val_of( current_line, (int *)&match_index,
      PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_PORTS_LIST_PORTS_SEND_TAGGED_ID, param_i,
      &param_val, VAL_NUMERIC, err_msg
      );
    if (0 == ui_ret)
    {
      prm_ports_list.send_tagged[param_i-1] = SOC_SAND_NUM2BOOL(param_val->value.ulong_value);
    }
  }
  prm_ports_list.nof_ports = param_i-1;

  prm_vid = prm_vlan_info.vid;
  if(remove)
  {
    for(vsi_i = prm_vsid_ndx; vsi_i<=prm_last_vsid_ndx;vsi_i++)
    {
      prm_vlan_info.vid = prm_vid + vsi_i - prm_vsid_ndx;
      ret = timna_app_vlan_ports_remove(
              vsi_i,
              &prm_vlan_info,
              &prm_ports_list
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        send_string_to_screen(" *** sweept20_vlan_ports_add - FAIL", TRUE);
        goto exit;
      }
    }
    send_string_to_screen(" *** sweept20_vlan_ports_add - SUCCEEDED", TRUE);
  }
  else
  {
    for(vsi_i = prm_vsid_ndx; vsi_i<=prm_last_vsid_ndx;vsi_i++)
    {
      prm_vlan_info.vid = prm_vid + vsi_i - prm_vsid_ndx;
      ret = timna_app_vlan_ports_add(
              vsi_i,
              &prm_vlan_info,
              &prm_ports_list
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        send_string_to_screen(" *** sweept20_vlan_ports_add - FAIL", TRUE);

        goto exit;
      }
    }
    send_string_to_screen(" *** sweept20_vlan_ports_add - SUCCEEDED", TRUE);
  }

exit:
  return ui_ret;
}
int
  ui_sweept20_api_vlan_ports_remove(
    CURRENT_LINE *current_line
  )
{
  soc_sand_os_printf( "NOT IMPLEMENTED\n\r");
  return 0;
}

#endif
#ifdef UI_SWEEPT20_API_POLICER


int
  ui_timna_app_policer_glbl_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TIMNA_POLICER_GLBL_INFO
    prm_policer_glbl_info;
  uint32
    indx = 0,
    val = 0;

  UI_MACROS_INIT_FUNCTION("ui_timna_app_policer");
  soc_sand_proc_name = "timna_policer_glbl_info_set";

  unit = timna_get_default_unit();
  timna_TIMNA_POLICER_GLBL_INFO_clear(&prm_policer_glbl_info);

  /* This is a set function, so call GET function first */
  ret = timna_policer_glbl_info_get(
          unit,
          &prm_policer_glbl_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_policer_glbl_info_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_policer_glbl_info_get");
    goto exit;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_POLICER_GLBL_INFO_SET_GLBL_INFO_SET_POLICER_GLBL_INFO_EBS_VALS_3_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_POLICER_GLBL_INFO_SET_GLBL_INFO_SET_POLICER_GLBL_INFO_EBS_VALS_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_POLICER_GLBL_INFO_SET_GLBL_INFO_SET_POLICER_GLBL_INFO_EBS_VALS_2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_POLICER_GLBL_INFO_SET_GLBL_INFO_SET_POLICER_GLBL_INFO_EBS_VALS_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    indx = (uint32)param_val->value.ulong_value;
  }

  prm_policer_glbl_info.burst_vals[indx] = val;


 /* Call function */
  ret = timna_policer_glbl_info_set(
          unit,
          &prm_policer_glbl_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_policer_glbl_info_set - FAIL", TRUE);
    timna_disp_result(ret, "timna_policer_glbl_info_set");
    goto exit;
  }

  send_string_to_screen(" *** timna_policer_glbl_info_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: glbl_info_get (section policer)
 */
int
  ui_timna_app_policer_glbl_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TIMNA_POLICER_GLBL_INFO
    prm_policer_glbl_info;

  UI_MACROS_INIT_FUNCTION("ui_timna_app_policer");
  soc_sand_proc_name = "timna_policer_glbl_info_get";

  unit = timna_get_default_unit();
  timna_TIMNA_POLICER_GLBL_INFO_clear(&prm_policer_glbl_info);

  /* Get parameters */

  /* Call function */
  ret = timna_policer_glbl_info_get(
          unit,
          &prm_policer_glbl_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_policer_glbl_info_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_policer_glbl_info_get");
    goto exit;
  }

  send_string_to_screen(" *** timna_policer_glbl_info_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> policer_glbl_info:", TRUE);
  timna_TIMNA_POLICER_GLBL_INFO_print(&prm_policer_glbl_info);


  goto exit;
exit:
  return ui_ret;
}

int
  ui_sweept20_api_policer_map_range_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_port_ndx = 0;
  uint32
    prm_policer_ndx = 0;
  uint32
    prm_last_policer_ndx = 0;
  SWEEPT20_POLICE_OP
    prm_map_type;
  uint32
    prm_first_id = 0;
  uint32
    prm_last_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_policer");
  soc_sand_proc_name = "sweept20_policer_map_range_set";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_POLICER_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_POLICER_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_policer_ndx = (uint32)param_val->value.ulong_value;
    prm_last_policer_ndx = prm_policer_ndx;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter policer_ndx after map_range_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_LAST_POLICER_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_LAST_POLICER_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_last_policer_ndx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_MAP_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_MAP_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_FIRST_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_FIRST_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_first_id = (uint32)param_val->value.ulong_value;
    prm_last_id = prm_first_id;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_LAST_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_LAST_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_last_id = (uint32)param_val->value.ulong_value;
  }

  ret = sweept20_policer_map_range_set(
          unit,
          sweept20_sys_info_get_local_port(unit, prm_port_ndx),
          prm_policer_ndx,
          prm_last_policer_ndx,
          prm_map_type,
          prm_first_id,
          prm_last_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_policer_map_range_set - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_policer_map_range_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_policer_map_range_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_port_ndx = 0;
  uint32
    prm_policer_ndx = 0;
  uint32
    prm_last_policer_ndx = 0;
  SWEEPT20_POLICE_OP
    prm_map_type;
  uint32
    prm_first_id = 0;
  uint32
    prm_last_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_policer");
  soc_sand_proc_name = "sweept20_policer_map_range_get";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_MAP_RANGE_GET_MAP_RANGE_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_MAP_RANGE_GET_MAP_RANGE_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after map_range_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_MAP_RANGE_GET_MAP_RANGE_GET_POLICER_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_MAP_RANGE_GET_MAP_RANGE_GET_POLICER_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_policer_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter policer_ndx after map_range_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_MAP_RANGE_GET_MAP_RANGE_GET_LAST_POLICER_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_MAP_RANGE_GET_MAP_RANGE_GET_LAST_POLICER_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_last_policer_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter last_policer_ndx after map_range_get***", TRUE);
    goto exit;
  }

  ret = sweept20_policer_map_range_get(
          unit,
          sweept20_sys_info_get_local_port(unit, prm_port_ndx),
          prm_policer_ndx,
          prm_last_policer_ndx,
          &prm_map_type,
          &prm_first_id,
          &prm_last_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_policer_map_range_get - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_policer_map_range_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> map_type:", TRUE);
  soc_sand_os_printf( "map_type: %s\n",sweept20_SWEEPT20_POLICE_OP_to_string(prm_map_type));

  send_string_to_screen("--> first_id:", TRUE);
  soc_sand_os_printf( "first_id: %lu\n",prm_first_id);

  send_string_to_screen("--> last_id:", TRUE);
  soc_sand_os_printf( "last_id: %lu\n",prm_last_id);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_policer_params_range_set(
    CURRENT_LINE *current_line,
    uint8     set_commnad
  )
{
  uint32
    ret;
  uint32
    prm_policer_ndx=0;
  uint32
    policer_i,
    prm_last_policer_ndx=0;
  TIMNA_POLICER_PARAMS
    prm_ingress_policer_params;
  uint32
    prm_cir_step=0;
  uint32
    prm_eir_step=0;
  TIMNA_POLICER_PARAMS
    prm_exact_ingress_policer_params;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_policer");
  soc_sand_proc_name = "sweept20_policer_params_range_set";

  unit = timna_get_default_unit();
  timna_TIMNA_POLICER_PARAMS_clear(&prm_ingress_policer_params);
  timna_TIMNA_POLICER_PARAMS_clear(&prm_exact_ingress_policer_params);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_POLICER_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_POLICER_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_policer_ndx = (uint32)param_val->value.ulong_value;
  }

  prm_last_policer_ndx = prm_policer_ndx;
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_LAST_POLICER_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_LAST_POLICER_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_last_policer_ndx = (uint32)param_val->value.ulong_value;
  }

  for(policer_i=prm_policer_ndx; policer_i<=prm_last_policer_ndx; policer_i++)
  {
    ret = timna_policer_ingress_params_get(
      unit,
      policer_i,
      &prm_ingress_policer_params
      );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** sweept20_policer_params_range_get - FAIL", TRUE);
      sweept20_disp_result(ret, "sweept20_policer_params_range_get");
      goto exit;
    }
    if(!set_commnad)
    {
      if(policer_i - prm_policer_ndx > 50)
      {
        soc_sand_os_printf( "  Printing only the first 50 entries\n\r");
        break;
      }
      soc_sand_os_printf( "  policer %lu: ", policer_i);
      timna_TIMNA_POLICER_PARAMS_print(&prm_ingress_policer_params);
      continue;
    }
    else
    {
      break;
    }
  }
  if(!set_commnad)
  {
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_EIR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_EIR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ingress_policer_params.eir = (uint32)param_val->value.ulong_value;
    prm_ingress_policer_params.ebs = 30;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_EBS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_EBS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ingress_policer_params.ebs = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_CIR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_CIR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ingress_policer_params.cir = (uint32)param_val->value.ulong_value;
    prm_ingress_policer_params.cbs = 30;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_CBS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_CBS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ingress_policer_params.cbs = (uint32)param_val->value.ulong_value;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_COLOR_AWARE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_COLOR_AWARE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ingress_policer_params.color_aware = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_CIR_STEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_CIR_STEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cir_step = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_EIR_STEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_EIR_STEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_eir_step = (uint32)param_val->value.ulong_value;
  }

  ret = sweept20_policer_params_range_set(
    unit,
    prm_policer_ndx,
    prm_last_policer_ndx,
    &prm_ingress_policer_params,
    prm_cir_step,
    prm_eir_step,
    &prm_exact_ingress_policer_params
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_mpls_tunnel_encap_get - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_policer_params_range_set - SUCCEEDED", TRUE);
  send_string_to_screen("--> exact_ingress_policer_params:", TRUE);

  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: in_vc_associated_policer_get (section vsid)
 */
int
  ui_timna_app_vsid_in_vc_associated_policer_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vc_label_ndx = 0;
  uint32
    prm_policer_id = 0;
  uint32
    prm_control_ptr;

  UI_MACROS_INIT_FUNCTION("ui_timna_app_vsid");
  soc_sand_proc_name = "timna_vsid_in_vc_associated_policer_get";

  unit = timna_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_VSID_IN_VC_ASSOCIATED_POLICER_GET_IN_VC_ASSOCIATED_POLICER_GET_VC_LABEL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_VSID_IN_VC_ASSOCIATED_POLICER_GET_IN_VC_ASSOCIATED_POLICER_GET_VC_LABEL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vc_label_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vc_lbl_ndx after in_vc_associated_policer_get***", TRUE);
  }


  /* Call function */
  ret = timna_vsid_in_vc_associated_policer_get(
          unit,
          prm_vc_label_ndx,
          &prm_policer_id,
          &prm_control_ptr
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_vsid_in_vc_associated_policer_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_vsid_in_vc_associated_policer_get");
    goto exit;
  }

  send_string_to_screen(" *** timna_vsid_in_vc_associated_policer_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> policer_id:", TRUE);
  soc_sand_os_printf( "policer_id: %ld\n",prm_policer_id);

  send_string_to_screen("--> control_ptr:", TRUE);
  soc_sand_os_printf( "control_ptr: %u\n",prm_control_ptr);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: port_vlan_associated_policer_get (section vsid)
 */
int
  ui_timna_app_vsid_port_vlan_associated_policer_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TIMNA_VSID_ETHERNET_KEY
    prm_eth_info_ndx;
  uint32
    prm_policer_id = 0;
  uint32
    prm_control_ptr;

  UI_MACROS_INIT_FUNCTION("ui_timna_app_vsid");
  soc_sand_proc_name = "timna_vsid_port_vlan_associated_policer_get";

  unit = timna_get_default_unit();
  timna_TIMNA_VSID_ETHERNET_KEY_clear(&prm_eth_info_ndx);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ETH_INFO_NDX_C_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ETH_INFO_NDX_C_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_eth_info_ndx.c_vid_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ETH_INFO_NDX_C_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ETH_INFO_NDX_C_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_eth_info_ndx.c_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ETH_INFO_NDX_S_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ETH_INFO_NDX_S_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_eth_info_ndx.vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ETH_INFO_NDX_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ETH_INFO_NDX_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_eth_info_ndx.port = (uint32)param_val->value.ulong_value;

    prm_eth_info_ndx.port = sweept20_sys_info_get_local_port(unit, prm_eth_info_ndx.port);

  }


  /* Call function */
  ret = timna_vsid_port_vlan_associated_policer_get(
          unit,
          &prm_eth_info_ndx,
          &prm_policer_id,
          &prm_control_ptr
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_vsid_port_vlan_associated_policer_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_vsid_port_vlan_associated_policer_get");
    goto exit;
  }

  send_string_to_screen(" *** timna_vsid_port_vlan_associated_policer_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> policer_id:", TRUE);
  soc_sand_os_printf( "policer_id: %ld\n",prm_policer_id);

  send_string_to_screen("--> control_ptr:", TRUE);
  soc_sand_os_printf( "control_ptr: %u\n",prm_control_ptr);


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_SWEEPT20_API_EGRESS_ENCAP
int
  ui_sweept20_api_egress_encap_pointer_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_pointer_ndx = 0;
  uint32
    up_to_pointer = 0;
  SWEEPT20_EGRESS_ENCAP_INFO
    prm_info;
  SWEEPT20_EGRESS_ENCAP_RANGE_INFO
    prm_range_info;


  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_egress_encap");
  soc_sand_proc_name = "sweept20_egress_encap_pointer_set";

  unit = timna_get_default_unit();
  sweept20_SWEEPT20_EGRESS_ENCAP_INFO_clear(&prm_info);
  prm_info.is_valid = TRUE;
  prm_info.is_tunnel_valid = FALSE;
  sweept20_SWEEPT20_EGRESS_ENCAP_RANGE_INFO_clear(&prm_range_info);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_EGRESS_ENCAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_EGRESS_ENCAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pointer_ndx = (uint32)param_val->value.ulong_value;
    up_to_pointer = prm_pointer_ndx;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pointer_ndx after pointer_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_RANGE_UP_TO_EGRESS_ENCAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_RANGE_UP_TO_EGRESS_ENCAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    up_to_pointer = (uint8)param_val->value.ulong_value;
  }


  if (up_to_pointer == prm_pointer_ndx)
  {

    ret = sweept20_egress_encap_pointer_get(
            unit,
            prm_pointer_ndx,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** sweept20_egress_encap_pointer_get - FAIL", TRUE);
      goto exit;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_IS_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_IS_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.is_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_OUT_TUNNEL_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_OUT_TUNNEL_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tunnel_label.label_id = (uint32)param_val->value.ulong_value;
    prm_info.is_tunnel_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tunnel_label.exp_qos = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_IS_TUNNEL_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_IS_TUNNEL_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.is_tunnel_valid = (uint8)param_val->value.ulong_value;
  }

 /*
  * ARP info
  */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_TAGGED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_TAGGED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.arp_info.tagged = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_USER_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_USER_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.arp_info.user_priority = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.arp_info.vid = (uint8)param_val->value.ulong_value;
  }
  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_MAC_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_info.arp_info.mac));
  }

 /*
  * Range info
  */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_RANGE_INC_MAC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_RANGE_INC_MAC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_mac = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_RANGE_INC_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_RANGE_INC_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_vid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_RANGE_INC_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_RANGE_INC_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_tunnel_label = (uint8)param_val->value.ulong_value;
  }

  if (up_to_pointer == prm_pointer_ndx)
  {
    ret = sweept20_egress_encap_pointer_set(
            unit,
            prm_pointer_ndx,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** sweept20_egress_encap_pointer_set - FAIL", TRUE);

      goto exit;
    }
  }
  else
  {
    ret = sweept20_egress_encap_pointer_range_set(
            unit,
            prm_pointer_ndx,
            up_to_pointer,
            &prm_info,
            &prm_range_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** sweept20_egress_encap_pointer_set - FAIL", TRUE);

      goto exit;
    }

  }

  send_string_to_screen(" *** sweept20_egress_encap_pointer_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

int
  ui_sweept20_api_egress_encap_pointer_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_pointer_ndx = 0;
  SWEEPT20_EGRESS_ENCAP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_egress_encap");
  soc_sand_proc_name = "sweept20_egress_encap_pointer_get";

  unit = timna_get_default_unit();
  sweept20_SWEEPT20_EGRESS_ENCAP_INFO_clear(&prm_info);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_GET_EGRESS_ENCAP_GET_EGRESS_ENCAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_EGRESS_ENCAP_GET_EGRESS_ENCAP_GET_EGRESS_ENCAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pointer_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pointer_ndx after pointer_get***", TRUE);
    goto exit;
  }

  ret = sweept20_egress_encap_pointer_get(
          unit,
          prm_pointer_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_egress_encap_pointer_get - FAIL", TRUE);

    goto exit;
  }
  send_string_to_screen(" *** sweept20_egress_encap_pointer_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> info:", TRUE);
  sweept20_SWEEPT20_EGRESS_ENCAP_INFO_print(&prm_info);
  goto exit;
exit:
  return ui_ret;
}
#endif

#ifdef UI_SWEEPT20_API_MPLS
int
  ui_sweept20_api_mpls_lsr_label_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_in_label_ndx = 0;
  SWEEPT20_MPLS_LSR_LABEL_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_mpls");
  soc_sand_proc_name = "sweept20_mpls_lsr_label_set";

  unit = timna_get_default_unit();
  sweept20_SWEEPT20_MPLS_LSR_LABEL_INFO_clear(&prm_info);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_IN_LABEL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_IN_LABEL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_in_label_ndx = (uint32)param_val->value.ulong_value;
  }

  ret = sweept20_mpls_lsr_label_get(
          unit,
          prm_in_label_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_mpls_lsr_label_get - FAIL", TRUE);
    sweept20_disp_result(ret, "sweept20_mpls_lsr_label_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_TUNNEL_ENCAP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_TUNNEL_ENCAP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tunnel_encap_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_IS_PHP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_IS_PHP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.is_php = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_OUT_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_OUT_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.out_label = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_OUT_SYS_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_OUT_SYS_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.out_sys_port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_IS_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_IS_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.is_valid = (uint8)param_val->value.ulong_value;
  }

  ret = sweept20_mpls_lsr_label_set(
          unit,
          prm_in_label_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_mpls_lsr_label_set - FAIL", TRUE);
    sweept20_disp_result(ret, "sweept20_mpls_lsr_label_set");
    goto exit;
  }

  send_string_to_screen(" *** sweept20_mpls_lsr_label_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_mpls_lsr_label_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_in_label_ndx = 0;
  SWEEPT20_MPLS_LSR_LABEL_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_mpls");
  soc_sand_proc_name = "sweept20_mpls_lsr_label_get";

  unit = timna_get_default_unit();
  sweept20_SWEEPT20_MPLS_LSR_LABEL_INFO_clear(&prm_info);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MPLS_LSR_LABEL_GET_LSR_LABEL_GET_IN_LABEL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MPLS_LSR_LABEL_GET_LSR_LABEL_GET_IN_LABEL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_in_label_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter in_label_ndx after lsr_label_get***", TRUE);
    goto exit;
  }

  ret = sweept20_mpls_lsr_label_get(
          unit,
          prm_in_label_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_mpls_lsr_label_get - FAIL", TRUE);
    sweept20_disp_result(ret, "sweept20_mpls_lsr_label_get");
    goto exit;
  }

  send_string_to_screen(" *** sweept20_mpls_lsr_label_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> info:", TRUE);
  sweept20_SWEEPT20_MPLS_LSR_LABEL_INFO_print(&prm_info);
  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_VPLS
int
  ui_sweept20_api_vpls_vc_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vsid_ndx = 0,
    last_vsid_ndx,
    nof_consequtive_vc_labels=1,
    label_i,
    vsi_i;
  uint32
    prm_provider_port_ndx = 0;
  SWEEPT20_VPLS_VC_INFO
    prm_vc_info,
    vc_info_i;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vpls");
  soc_sand_proc_name = "sweept20_vpls_vc_add";

  unit = timna_get_default_unit();
  sweept20_SWEEPT20_VPLS_VC_INFO_clear(&prm_vc_info);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VSID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VSID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsid_ndx = (uint32)param_val->value.ulong_value;
    last_vsid_ndx = prm_vsid_ndx;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsid_ndx after vc_add***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_LAST_VSID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_LAST_VSID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    last_vsid_ndx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_PROVIDER_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_PROVIDER_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_provider_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter provider_port_ndx after vc_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_POLICER_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_POLICER_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    vc_info_i.policer_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_TUNNEL_ENCAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_TUNNEL_ENCAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    vc_info_i.tunnel_encap = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_OUT_VC_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_OUT_VC_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vc_info.out_vc_label = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_IN_VC_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_IN_VC_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vc_info.in_vc_label = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_UP_TO_IN_VC_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_UP_TO_IN_VC_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_consequtive_vc_labels = (uint32)param_val->value.ulong_value;
  }

  for(vsi_i=prm_vsid_ndx; vsi_i <= last_vsid_ndx; vsi_i++)
  {
    for(label_i=0;label_i<nof_consequtive_vc_labels;label_i++)
    {
      vc_info_i.in_vc_label =
        prm_vc_info.in_vc_label +
          (vsi_i - prm_vsid_ndx) * nof_consequtive_vc_labels +
          label_i;

      vc_info_i.out_vc_label =
        prm_vc_info.out_vc_label +
        (vsi_i - prm_vsid_ndx) * nof_consequtive_vc_labels +
        label_i;

      ret = timna_app_vpls_vc_add(
              vsi_i,
              prm_provider_port_ndx,
              &vc_info_i
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        send_string_to_screen(" *** sweept20_vpls_vc_add - FAIL", TRUE);

        goto exit;
      }
    }
  }

  send_string_to_screen(" *** sweept20_vpls_vc_add - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_vpls_vc_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vsid_ndx = 0;
  uint32
    prm_provider_port_ndx = 0;
  SWEEPT20_VPLS_VC_INFO
    prm_vc_info;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vpls");
  soc_sand_proc_name = "sweept20_vpls_vc_remove";

  unit = timna_get_default_unit();
  sweept20_SWEEPT20_VPLS_VC_INFO_clear(&prm_vc_info);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_VSID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_VSID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsid_ndx after vc_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_PROVIDER_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_PROVIDER_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_provider_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter provider_port_ndx after vc_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_VC_INFO_POLICER_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_VC_INFO_POLICER_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vc_info.policer_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_VC_INFO_TUNNEL_ENCAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_VC_INFO_TUNNEL_ENCAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vc_info.tunnel_encap = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_VC_INFO_OUT_VC_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_VC_INFO_OUT_VC_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vc_info.out_vc_label = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_VC_INFO_IN_VC_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_VC_INFO_IN_VC_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vc_info.in_vc_label = (uint32)param_val->value.ulong_value;
  }

  ret = timna_app_vpls_vc_remove(
          prm_vsid_ndx,
          prm_provider_port_ndx,
          prm_vc_info.in_vc_label
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_vpls_vc_remove - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_vpls_vc_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_ACL
int
  ui_sweept20_api_acl_template_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TIMNA_ACL_DEFINED_TEMPLATE_INFO
    prm_acl_info;
  uint32
    prm_acl_id = 0;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_acl");
  soc_sand_proc_name = "timna_acl_template_add";

  unit = timna_get_default_unit();
  timna_TIMNA_ACL_DEFINED_TEMPLATE_INFO_clear(&prm_acl_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_TEMPLATE_ADD_TEMPLATE_ADD_ACL_INFO_PREDEFINED_TEMPLATE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_API_ACL_TEMPLATE_ADD_TEMPLATE_ADD_ACL_INFO_PREDEFINED_TEMPLATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_info.predefined_template = (TIMNA_ACL_KEY_TEMPLATE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_TEMPLATE_ADD_TEMPLATE_ADD_ACL_INFO_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_TEMPLATE_ADD_TEMPLATE_ADD_ACL_INFO_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_info.size = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_TEMPLATE_ADD_TEMPLATE_ADD_ACL_INFO_TRAFFIC_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_API_ACL_TEMPLATE_ADD_TEMPLATE_ADD_ACL_INFO_TRAFFIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_info.traffic = (TIMNA_ACL_TRAFFIC_TYPE)param_val->numeric_equivalent;
  }

  /* Call function */
  ret = timna_acl_template_add(
          unit,
          &prm_acl_info,
          &prm_acl_id,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_acl_template_add - FAIL", TRUE);
    timna_disp_result(ret, "timna_acl_template_add");
    goto exit;
  }

  send_string_to_screen(" *** timna_acl_template_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> acl_id:", TRUE);
  soc_sand_os_printf( "acl_id: %lu\n",prm_acl_id);

  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n",prm_success);


  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_acl_ace_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx = 0;
  uint32
    prm_ace_ndx = 0;
  TIMNA_ACL_ACE_INFO
    prm_ace;
  TIMNA_ACL_ACTION_INFO
    prm_action_info;
  uint8
    prm_overwrite;
  char
    *mac_addr ;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_acl");
  soc_sand_proc_name = "timna_acl_ace_add";

  unit = timna_get_default_unit();
  timna_TIMNA_ACL_ACE_INFO_clear(&prm_ace);
  timna_TIMNA_ACL_ACTION_INFO_clear(&prm_action_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after ace_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ace_ndx after ace_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_PORT_RANGE_MAX_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_PORT_RANGE_MAX_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l4_info.port_range.max_port = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_PORT_RANGE_MIN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_PORT_RANGE_MIN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l4_info.port_range.min_port = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_PORT_RANGE_IS_SRC_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_PORT_RANGE_IS_SRC_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l4_info.port_range.is_src_port = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_DEST_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_DEST_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l4_info.dest_port = (uint16)param_val->value.ulong_value;
    prm_ace.l4_info.dest_port_mask = 0XFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_DEST_PORT_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_DEST_PORT_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l4_info.dest_port_mask = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_SRC_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_SRC_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l4_info.src_port = (uint16)param_val->value.ulong_value;
    prm_ace.l4_info.src_port_mask = 0XFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_SRC_PORT_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_SRC_PORT_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l4_info.src_port_mask = (uint16)param_val->value.ulong_value;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_DEST_IP_ADD_ID,1))
  {
    if (!get_val_of(
      current_line,(int *)&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_DEST_IP_ADD_ID,1,
      &param_val,VAL_IP,err_msg))
    {
       prm_ace.l3_info.dest_ip.ip_address = param_val->value.ip_value;
       prm_ace.l3_info.dest_ip.prefix_len = 32;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_DEST_IP_PREFFIX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_DEST_IP_PREFFIX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l3_info.dest_ip.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_SRC_IP_ADD_ID,1))
  {
    if (!get_val_of(
      current_line,(int *)&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_SRC_IP_ADD_ID,1,
      &param_val,VAL_IP,err_msg))
    {
       prm_ace.l3_info.src_ip.ip_address = param_val->value.ip_value;
       prm_ace.l3_info.src_ip.prefix_len = 32;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_SRC_IP_PREFFIX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_SRC_IP_PREFFIX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l3_info.src_ip.prefix_len = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_PROTOCOL_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_PROTOCOL_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l3_info.protocol_code = (uint8)param_val->value.ulong_value;
    prm_ace.l3_info.protocol_code_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_PROTOCOL_CODE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_PROTOCOL_CODE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l3_info.protocol_code_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_TOS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_TOS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l3_info.tos = (uint8)param_val->value.ulong_value;
    prm_ace.l3_info.tos_mask = 0XFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_TOS_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_TOS_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l3_info.tos_mask = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_DEST_MAC_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    mac_addr = param_val->value.val_text ;

    soc_sand_pp_mac_address_string_parse(mac_addr, &(prm_ace.l2_info.dest_mac));
    prm_ace.l2_info.dest_mac_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_DEST_MAC_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_DEST_MAC_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.dest_mac_valid = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_SRC_MAC_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    mac_addr = param_val->value.val_text ;
    soc_sand_pp_mac_address_string_parse(mac_addr, &(prm_ace.l2_info.src_mac));
    prm_ace.l2_info.src_mac_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_SRC_MAC_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_SRC_MAC_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.src_mac_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.s_tag.dei = (uint8)param_val->value.ulong_value;
    prm_ace.l2_info.s_tag.dei_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_DEI_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_DEI_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.s_tag.dei_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.s_tag.pcp = (uint8)param_val->value.ulong_value;
    prm_ace.l2_info.s_tag.pcp_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_PCP_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_PCP_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.s_tag.pcp_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_TAG_EXIST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_TAG_EXIST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.s_tag.tag_exist = (uint8)param_val->value.ulong_value;
    prm_ace.l2_info.s_tag.tag_exist_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_TAG_EXIST_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_TAG_EXIST_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.s_tag.tag_exist_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.s_tag.vid = (uint32)param_val->value.ulong_value;
    prm_ace.l2_info.s_tag.vid_mask = 0XFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_VID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_VID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.s_tag.vid_mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.c_tag.dei = (uint8)param_val->value.ulong_value;
    prm_ace.l2_info.c_tag.dei_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_DEI_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_DEI_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.c_tag.dei_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.c_tag.pcp = (uint8)param_val->value.ulong_value;
    prm_ace.l2_info.c_tag.pcp_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_PCP_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_PCP_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.c_tag.pcp_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_TAG_EXIST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_TAG_EXIST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.c_tag.tag_exist = (uint8)param_val->value.ulong_value;
    prm_ace.l2_info.c_tag.tag_exist_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_TAG_EXIST_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_TAG_EXIST_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.c_tag.tag_exist_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.c_tag.vid = (uint32)param_val->value.ulong_value;
    prm_ace.l2_info.c_tag.vid_mask = 0XFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_VID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_VID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.c_tag.vid_mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.port_type = (SOC_SAND_PP_PORT_L2_TYPE)param_val->value.ulong_value;
    prm_ace.l2_info.port_type_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_PORT_TYPE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_PORT_TYPE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.port_type_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_ETHER_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_ETHER_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.ether_type = (uint16)param_val->value.ulong_value;
    prm_ace.l2_info.ether_type_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_ETHER_TYPE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_ETHER_TYPE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.l2_info.ether_type_valid = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_INLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_INLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.general_info.inlif = (uint32)param_val->value.ulong_value;
    prm_ace.general_info.inlif_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_INLIF_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_INLIF_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.general_info.inlif_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_VSID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_VSID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.general_info.vsid = (uint32)param_val->value.ulong_value;
    prm_ace.general_info.vsid_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_VSID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_VSID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.general_info.vsid_valid = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_LOCAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_LOCAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.general_info.local_port = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
    prm_ace.general_info.local_port_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_LOCAL_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_LOCAL_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.general_info.local_port_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_OVERWRITE_CONTROL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_OVERWRITE_CONTROL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.overwrite_control = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_PARAMS_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_PARAMS_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.cos_params.drop_precedence = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_PARAMS_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_PARAMS_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.cos_params.traffic_class = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_PARAMS_UP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_PARAMS_UP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.cos_params.user_priority = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_POLICER_CONTROLLER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_POLICER_CONTROLLER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.policer_controller = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_POLICER_POINTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_POLICER_POINTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.policer_pointer = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.outlif = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.type = (TIMNA_ACL_ACTION_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_OVERWRITE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_OVERWRITE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_overwrite = (uint8)param_val->value.ulong_value;
  }


  soc_sand_os_printf( "calling add \n");
  /* Call function */
  ret = timna_acl_ace_add(
          unit,
          prm_acl_ndx,
          prm_ace_ndx,
          &prm_ace,
          &prm_action_info,
          prm_overwrite,
          &prm_success
        );
  soc_sand_os_printf( "return add \n");
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_acl_ace_add - FAIL", TRUE);
    timna_disp_result(ret, "timna_acl_ace_add");
    goto exit;
  }

  send_string_to_screen(" *** timna_acl_ace_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n",prm_success);


  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_acl_ace_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx = 0;
  TIMNA_ACL_BLOCK_INFO
    prm_block_range;
  uint32
    prm_pos_handle;
  TIMNA_ACL_ACE_INFO
    prm_ace_block[1000];
  TIMNA_ACL_ACTION_INFO
    prm_actions_block[1000];
  uint32
    prm_nof_entries,
    indx = 0;
  TIMNA_ACL_STAT_INFO
    prm_stat_info;
  uint8
    prm_in_use;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_acl");
  soc_sand_proc_name = "timna_acl_ace_get_block";


  unit = timna_get_default_unit();
  timna_TIMNA_ACL_BLOCK_INFO_clear(&prm_block_range);
  prm_block_range.entries_to_act = 100;
  prm_block_range.entries_to_scan = 1000;
  prm_pos_handle = 0;


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after ace_get_block***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_BLOCK_RANGE_ENTRIES_TO_ACT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_BLOCK_RANGE_ENTRIES_TO_ACT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_block_range.entries_to_act = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_BLOCK_RANGE_ENTRIES_TO_SCAN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_BLOCK_RANGE_ENTRIES_TO_SCAN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_block_range.entries_to_scan = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = timna_acl_ace_get_block(
          unit,
          prm_acl_ndx,
          &prm_block_range,
          &prm_pos_handle,
          prm_ace_block,
          prm_actions_block,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_acl_ace_get_block - FAIL", TRUE);
    timna_disp_result(ret, "timna_acl_ace_get_block");
    goto exit;
  }

  send_string_to_screen(" *** timna_acl_ace_get_block - SUCCEEDED", TRUE);

  send_string_to_screen(" *** timna_mact_entry_get_block - SUCCEEDED", TRUE);
  send_string_to_screen("+---------------------------------------------------------------------------------------------------------------------------------+", TRUE);
  send_string_to_screen("|                                               ACL Table                                                                         |", TRUE);
  send_string_to_screen("|---------------------------------------------------------------------------------------------------------------------------------|", TRUE);
  send_string_to_screen("|                         ACE                                                       |     ACTION                                  |", TRUE);
  send_string_to_screen("|---------------------------------------------------------------------------------------------------------------------------------|", TRUE);

  soc_sand_os_printf( "|");
  soc_sand_os_printf( "%-7s|%-7s|%-7s| ", " port ", "outLif", "  VSI");

  ret = timna_acl_stat_get(
          unit,
          prm_acl_ndx,
          &prm_stat_info,
          &prm_in_use
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_acl_stat_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_acl_stat_get");
    goto exit;
  }

  for (indx = 0; indx < prm_stat_info.init_info.key_template.nof_fields; ++indx)
  {
    soc_sand_os_printf( "%-7s|", timna_TIMNA_ACL_KEY_SELECT_to_string_short(prm_stat_info.init_info.key_template.key_fields[indx]));
  }

  soc_sand_os_printf( "%15s","|\n");

  send_string_to_screen("|---------------------------------------------------------------------------------------------------------------------------------|", TRUE);

  for (indx = 0; indx < prm_nof_entries; ++indx)
  {
    soc_sand_os_printf( "|");
    timna_TIMNA_ACL_ACE_INFO_print_in_table(
      &prm_ace_block[indx],
      prm_stat_info.init_info.key_template.key_fields,
      prm_stat_info.init_info.key_template.nof_fields
    );
    if (prm_actions_block[indx].type == TIMNA_ACL_ACTION_TYPE_DENY ||
        prm_actions_block[indx].type == TIMNA_ACL_ACTION_TYPE_NOP)
    {
      soc_sand_os_printf( " %-20s |", timna_TIMNA_ACL_ACTION_TYPE_to_string_short(prm_actions_block[indx].type));
    }
    if (prm_actions_block[indx].type == TIMNA_ACL_ACTION_TYPE_UPDATE_OUTLIF ||
        prm_actions_block[indx].type == TIMNA_ACL_ACTION_TYPE_UPDATE_POLICIING_OUTLIF ||
        prm_actions_block[indx].type == TIMNA_ACL_ACTION_TYPE_UPDATE_POLICIING_OUTLIF_COS
       )
    {
      soc_sand_os_printf( "outlif:%-6lu,", prm_actions_block[indx].outlif);
    }
    if (prm_actions_block[indx].type == TIMNA_ACL_ACTION_TYPE_UPDATE_POLICIING ||
        prm_actions_block[indx].type == TIMNA_ACL_ACTION_TYPE_UPDATE_POLICIING_OUTLIF ||
        prm_actions_block[indx].type == TIMNA_ACL_ACTION_TYPE_UPDATE_POLICIING_OUTLIF_COS
       )
    {
      soc_sand_os_printf( "plcr:%-4lu,%-4lu |", prm_actions_block[indx].policer_pointer, prm_actions_block[indx].policer_controller);
    }
    if (prm_actions_block[indx].type == TIMNA_ACL_ACTION_TYPE_UPDATE_POLICIING_OUTLIF_COS)
    {
      soc_sand_os_printf( "UP:%-3lu,DP:%-3lu,TC:%-3lu ", prm_actions_block[indx].cos_params.user_priority, prm_actions_block[indx].cos_params.drop_precedence, prm_actions_block[indx].cos_params.traffic_class);
    }
    soc_sand_os_printf( "\n");
    send_string_to_screen("|---------------------------------------------------------------------------------------------------------------------------------|", TRUE);
/*    if (indx % TIMNA_UI_MACT_LINES_IN_SCREEN == 0)
    {
      soc_sand_os_printf( "Press 'q' to quit or any other key to continue\n");
      if (ch == 'q')
      {
        break;
      }
    }
*/
  }
  soc_sand_os_printf( "Number of Entires: %ld\n",prm_nof_entries);
  soc_sand_os_printf( "End indx: %ld\n",prm_pos_handle);



  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_acl_ace_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx = 0;
  uint32
    prm_ace_ndx = 0;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_acl");
  soc_sand_proc_name = "timna_acl_ace_remove";

  unit = timna_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_REMOVE_ACE_REMOVE_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_REMOVE_ACE_REMOVE_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after ace_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_REMOVE_ACE_REMOVE_ACE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_ACL_ACE_REMOVE_ACE_REMOVE_ACE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ace_ndx after ace_remove***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = timna_acl_ace_remove(
          unit,
          prm_acl_ndx,
          prm_ace_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_acl_ace_remove - FAIL", TRUE);
    timna_disp_result(ret, "timna_acl_ace_remove");
    goto exit;
  }

  send_string_to_screen(" *** timna_acl_ace_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
#endif
#ifdef UI_SWEEPT20_API_BSP
int
  ui_sweept20_api_bsp_reset_device(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_bsp");
  soc_sand_proc_name = "sweept20_bsp_reset_device";

  unit = timna_get_default_unit();



  ret = sweept20_bsp_reset_device(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_bsp_reset_device - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_bsp_reset_device - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_SYS_INFO
int
  ui_sweept20_api_sys_info_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_sys_info");
  soc_sand_proc_name = "sweept20_sys_info_init";

  unit = timna_get_default_unit();



  ret = sweept20_sys_info_init(
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_sys_info_init - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_sys_info_init - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_sys_info_is_local_port(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_sys_port_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_sys_info");
  soc_sand_proc_name = "sweept20_sys_info_is_local_port";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_SYS_INFO_IS_LOCAL_PORT_IS_LOCAL_PORT_SYS_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_SYS_INFO_IS_LOCAL_PORT_IS_LOCAL_PORT_SYS_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_port_id = (uint32)param_val->value.ulong_value;
  }

  ret = sweept20_sys_info_is_local_port(
          unit,
          prm_sys_port_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_sys_info_is_local_port - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" *** sweept20_sys_info_is_local_port - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_sys_info_get_local_port(
    CURRENT_LINE *current_line
  )
{
  uint32
    prm_sys_port_id,
    local_port_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_sys_info");
  soc_sand_proc_name = "sweept20_sys_info_get_local_port";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_SYS_INFO_GET_LOCAL_PORT_GET_LOCAL_PORT_SYS_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_SYS_INFO_GET_LOCAL_PORT_GET_LOCAL_PORT_SYS_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_port_id = (uint32)param_val->value.ulong_value;
  }

  local_port_id = sweept20_sys_info_get_local_port(
          unit,
          prm_sys_port_id
        );
  soc_sand_os_printf( 
    "System port ID %lu is mapped to local port ID %lu\n\r",
    prm_sys_port_id,
    local_port_id
  );

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_8021AD
int
  ui_sweept20_api_8021ad_c_vid_register(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_port_ndx = 0;
  uint32
    prm_c_vid_ndx = 0;
  uint32
    prm_s_vid;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_8021ad");
  soc_sand_proc_name = "timna_qinq_c_vid_register";

  unit = timna_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_C_VID_REGISTER_C_VID_REGISTER_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_C_VID_REGISTER_C_VID_REGISTER_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (SOC_SAND_PP_LOCAL_PORT_ID)(param_val->value.ulong_value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prt_ndx after c_vid_register***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_C_VID_REGISTER_C_VID_REGISTER_C_VID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_C_VID_REGISTER_C_VID_REGISTER_C_VID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_c_vid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter c_vid_ndx after c_vid_register***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_C_VID_REGISTER_C_VID_REGISTER_S_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_C_VID_REGISTER_C_VID_REGISTER_S_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_s_vid = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter s_vid after c_vid_register***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = timna_qinq_c_vid_register(
          unit,
          sweept20_sys_info_get_local_port(unit, prm_port_ndx),
          prm_c_vid_ndx,
          prm_s_vid
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_qinq_c_vid_register - FAIL", TRUE);
    timna_disp_result(ret, "timna_qinq_c_vid_register");
    goto exit;
  }

  send_string_to_screen(" *** timna_qinq_c_vid_register - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pep_info_set (section qinq)
 */
int
  ui_sweept20_api_8021ad_pep_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_pep_id_ndx = 0;
  TIMNA_QINQ_PEP_INFO
    prm_pep_info;
  uint32
    prm_port_id,
    prm_svid;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_8021ad");
  soc_sand_proc_name = "timna_qinq_pep_info_set";

  unit = timna_get_default_unit();
  timna_TIMNA_QINQ_PEP_INFO_clear(&prm_pep_info);


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_PORT_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_PORT_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_id = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pep_id_ndx after pep_info_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_SVID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_SVID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_svid = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pep_id_ndx after pep_info_set***", TRUE);
    goto exit;
  }

  prm_pep_id_ndx = sweept20_sys_info_get_local_port(unit, prm_port_id);
  prm_pep_id_ndx = SOC_SAND_PP_LOCAL_PORT_PEP_SET(prm_pep_id_ndx, prm_svid);

  /* This is a set function, so call GET function first */
  ret = timna_qinq_pep_info_get(
          unit,
          prm_pep_id_ndx,
          &prm_pep_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_qinq_pep_info_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_qinq_pep_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_INGRESS_S_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_INGRESS_S_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_info.ingress.s_dei = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_INGRESS_S_USER_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_INGRESS_S_USER_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_info.ingress.s_user_priority = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_INGRESS_C_TAGGED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_INGRESS_C_TAGGED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_info.ingress.c_tagged = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_EGRESS_C_USER_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_EGRESS_C_USER_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_info.egress.c_user_priority = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_EGRESS_C_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_EGRESS_C_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_info.egress.pep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_EGRESS_ACCEPTED_FRAMES_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_EGRESS_ACCEPTED_FRAMES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_info.egress.accepted_frames = (TIMNA_FILTER_ACCEPT_FRAME_TYPE)param_val->numeric_equivalent;
  }


  /* Call function */
  ret = timna_qinq_pep_info_set(
          unit,
          prm_pep_id_ndx,
          &prm_pep_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_qinq_pep_info_set - FAIL", TRUE);
    timna_disp_result(ret, "timna_qinq_pep_info_set");
    goto exit;
  }

  send_string_to_screen(" *** timna_qinq_pep_info_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pep_info_get (section qinq)
 */
int
  ui_sweept20_api_8021ad_pep_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_pep_id_ndx = 0;
  TIMNA_QINQ_PEP_INFO
    prm_pep_info;
  uint32
    prm_port_id,
    prm_svid;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_8021ad");
  soc_sand_proc_name = "timna_qinq_pep_info_get";

  unit = timna_get_default_unit();
  timna_TIMNA_QINQ_PEP_INFO_clear(&prm_pep_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_INFO_GET_PEP_INFO_GET_PORT_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_PEP_INFO_GET_PEP_INFO_GET_PORT_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_id = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pep_id_ndx after pep_info_get***", TRUE);
    goto exit;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_INFO_GET_PEP_INFO_GET_SVID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_PEP_INFO_GET_PEP_INFO_GET_SVID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_svid = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pep_id_ndx after pep_info_get***", TRUE);
    goto exit;
  }

  prm_pep_id_ndx = sweept20_sys_info_get_local_port(unit, prm_port_id);
  prm_pep_id_ndx = SOC_SAND_PP_LOCAL_PORT_PEP_SET(prm_pep_id_ndx, prm_svid);

  /* Call function */
  ret = timna_qinq_pep_info_get(
          unit,
          prm_pep_id_ndx,
          &prm_pep_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_qinq_pep_info_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_qinq_pep_info_get");
    goto exit;
  }

  send_string_to_screen(" *** timna_qinq_pep_info_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> pep_info:", TRUE);
  timna_TIMNA_QINQ_PEP_INFO_print(&prm_pep_info);


  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
 *  Function handler: cep_info_set (section qinq)
 */
int
  ui_sweept20_api_8021ad_cep_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_cep_id_ndx = 0;
  TIMNA_QINQ_CEP_INFO
    prm_cep_info;
  uint32
    prm_cvid_ndx = 0;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_8021ad");
  soc_sand_proc_name = "timna_qinq_cep_info_set";

  unit = timna_get_default_unit();
  timna_TIMNA_QINQ_CEP_INFO_clear(&prm_cep_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_CEP_INFO_SET_CEP_INFO_SET_CEP_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_CEP_INFO_SET_CEP_INFO_SET_CEP_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cep_id_ndx = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter expect port id ***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_CEP_INFO_SET_CEP_INFO_SET_CEP_VLAN_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_CEP_INFO_SET_CEP_INFO_SET_CEP_VLAN_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cvid_ndx = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter expect cvlan id ***", TRUE);
    goto exit;
  }


  prm_cep_id_ndx = sweept20_sys_info_get_local_port(unit, prm_cep_id_ndx);
  prm_cep_id_ndx = SOC_SAND_PP_LOCAL_PORT_CEP_SET(prm_cep_id_ndx);

  /* This is a set function, so call GET function first */
  ret = timna_qinq_cep_info_get(
          unit,
          prm_cep_id_ndx,
          prm_cvid_ndx,
          &prm_cep_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_qinq_cep_info_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_qinq_cep_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_CEP_INFO_SET_CEP_INFO_SET_CEP_INFO_ID_C_TAG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_CEP_INFO_SET_CEP_INFO_SET_CEP_INFO_ID_C_TAG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cep_info.egress.c_tagged = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = timna_qinq_cep_info_set(
          unit,
          prm_cep_id_ndx,
          prm_cvid_ndx,
          &prm_cep_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_qinq_cep_info_set - FAIL", TRUE);
    timna_disp_result(ret, "timna_qinq_cep_info_set");
    goto exit;
  }

  send_string_to_screen(" *** timna_qinq_cep_info_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: cep_info_get (section qinq)
 */
int
  ui_sweept20_api_8021ad_cep_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_cep_id_ndx = 0;
  TIMNA_QINQ_CEP_INFO
    prm_cep_info;
  uint32
    prm_cvid_ndx = 0;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_8021ad");
  soc_sand_proc_name = "timna_qinq_cep_info_get";

  unit = timna_get_default_unit();
  timna_TIMNA_QINQ_CEP_INFO_clear(&prm_cep_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_CEP_INFO_GET_CEP_INFO_GET_ID_CEP_ID_NDX,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_CEP_INFO_GET_CEP_INFO_GET_ID_CEP_ID_NDX);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cep_id_ndx = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cep_id_ndx after cep_info_get***", TRUE);
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_CEP_INFO_GET_CEP_INFO_GET_CEP_VLAN_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_CEP_INFO_GET_CEP_INFO_GET_CEP_VLAN_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cvid_ndx = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cep_id_ndx after cep_info_get***", TRUE);
    goto exit;
  }

  prm_cep_id_ndx = sweept20_sys_info_get_local_port(unit, prm_cep_id_ndx);
  prm_cep_id_ndx = SOC_SAND_PP_LOCAL_PORT_CEP_SET(prm_cep_id_ndx);

  /* Call function */
  ret = timna_qinq_cep_info_get(
          unit,
          prm_cep_id_ndx,
          prm_cvid_ndx,
          &prm_cep_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_qinq_cep_info_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_qinq_cep_info_get");
    goto exit;
  }

  send_string_to_screen(" *** timna_qinq_cep_info_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> cep_info:", TRUE);
  timna_TIMNA_QINQ_CEP_INFO_print(&prm_cep_info);


  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
 *  Function handler: c_vid_unregister (section qinq)
 */
int
  ui_sweept20_api_8021ad_c_vid_unregister(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_port_ndx = 0;
  uint32
    prm_c_vid_ndx = 0;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_8021ad");
  soc_sand_proc_name = "timna_qinq_c_vid_unregister";

  unit = timna_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_C_VID_UNREGISTER_C_VID_UNREGISTER_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_C_VID_UNREGISTER_C_VID_UNREGISTER_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prt_ndx after c_vid_unregister***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_C_VID_UNREGISTER_C_VID_UNREGISTER_C_VID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_C_VID_UNREGISTER_C_VID_UNREGISTER_C_VID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_c_vid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter c_vid_ndx after c_vid_unregister***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = timna_qinq_c_vid_unregister(
          unit,
          sweept20_sys_info_get_local_port(unit, prm_port_ndx),
          prm_c_vid_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_qinq_c_vid_unregister - FAIL", TRUE);
    timna_disp_result(ret, "timna_qinq_c_vid_unregister");
    goto exit;
  }

  send_string_to_screen(" *** timna_qinq_c_vid_unregister - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pep_registeration_status_get (section qinq)
 */
int
  ui_sweept20_api_8021ad_pep_registeration_status_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_pep_id_ndx = 0;
  uint32
    prm_c_vids[100],
    mem_indx = 0;
  uint32
    prm_c_vids_len;
  uint32
    prm_max_members;
  uint32
    prm_iter,
    prm_port_id,
    prm_svid;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_8021ad");
  soc_sand_proc_name = "timna_qinq_pep_registeration_status_get";

  unit = timna_get_default_unit();
  prm_max_members = 100;
  prm_iter = 0;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_PEP_PORT_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_PEP_PORT_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_id = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pep_id_ndx after pep_registeration_status_get***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_PEP_SVID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_PEP_SVID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_svid = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pep_id_ndx after pep_registeration_status_get***", TRUE);
    goto exit;
  }

  prm_pep_id_ndx = sweept20_sys_info_get_local_port(unit, prm_port_id);
  prm_pep_id_ndx = SOC_SAND_PP_LOCAL_PORT_PEP_SET(prm_pep_id_ndx, prm_svid);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_MAX_MEMBERS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_MAX_MEMBERS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_max_members = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_ITER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_ITER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_iter = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = timna_qinq_pep_registeration_status_get(
          unit,
          prm_pep_id_ndx,
          prm_max_members,
          &prm_iter,
          prm_c_vids,
          &prm_c_vids_len
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_qinq_pep_registeration_status_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_qinq_pep_registeration_status_get");
    goto exit;
  }

  send_string_to_screen(" *** timna_qinq_pep_registeration_status_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> c_vids:", TRUE);
  for (mem_indx = 0; mem_indx < prm_c_vids_len; ++mem_indx)
  {
    soc_sand_os_printf( "%lu) %lu\n",mem_indx, prm_c_vids[mem_indx]);
  }


  send_string_to_screen("--> c_vids_len:", TRUE);
  soc_sand_os_printf( "c_vids_len: %lu\n",prm_c_vids_len);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: c_vid_registeration_status_get (section qinq)
 */
int
  ui_sweept20_api_8021ad_c_vid_registeration_status_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_port_ndx = 0;
  uint32
    prm_c_vid_ndx = 0;
  uint32
    prm_s_vid;
  uint8
    prm_registered;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_8021ad");
  soc_sand_proc_name = "timna_qinq_c_vid_registeration_status_get";

  unit = timna_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_C_VID_REGISTERATION_STATUS_GET_C_VID_REGISTERATION_STATUS_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_C_VID_REGISTERATION_STATUS_GET_C_VID_REGISTERATION_STATUS_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prt_ndx after c_vid_registeration_status_get***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_C_VID_REGISTERATION_STATUS_GET_C_VID_REGISTERATION_STATUS_GET_C_VID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_8021AD_C_VID_REGISTERATION_STATUS_GET_C_VID_REGISTERATION_STATUS_GET_C_VID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_c_vid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter c_vid_ndx after c_vid_registeration_status_get***", TRUE);
  }


  /* Call function */
  ret = timna_qinq_c_vid_registeration_status_get(
          unit,
          sweept20_sys_info_get_local_port(unit, prm_port_ndx),
          prm_c_vid_ndx,
          &prm_s_vid,
          &prm_registered
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_qinq_c_vid_registeration_status_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_qinq_c_vid_registeration_status_get");
    goto exit;
  }

  send_string_to_screen(" *** timna_qinq_c_vid_registeration_status_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> s_vid:", TRUE);
  soc_sand_os_printf( "s_vid: %ld\n",prm_s_vid);

  send_string_to_screen("--> registered:", TRUE);
  soc_sand_os_printf( "registered: %u\n",prm_registered);


  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_FILTER
int
  ui_sweept20_api_filter_port_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_port_ndx = 0;
  TIMNA_FILTER_PORT_INFO
    prm_port_info;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_filter");
  soc_sand_proc_name = "timna_filter_port_info_set";

  unit = timna_get_default_unit();
  timna_TIMNA_FILTER_PORT_INFO_clear(&prm_port_info);


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prt_ndx after port_info_set***", TRUE);
  }

  ret = timna_filter_port_info_get(
          unit,
          sweept20_sys_info_get_local_port(unit, prm_port_ndx),
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_filter_port_info_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_filter_port_info_get");
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_UNKNOWN_DA_UNKNOWN_BC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_UNKNOWN_DA_UNKNOWN_BC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.unknown_da.unknown_bc = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_UNKNOWN_DA_UNKNOWN_MC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_UNKNOWN_DA_UNKNOWN_MC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.unknown_da.unknown_mc = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_UNKNOWN_DA_UNKNOWN_UC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_UNKNOWN_DA_UNKNOWN_UC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.unknown_da.unknown_uc = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_AUTHORIZATION_INFO_NOT_AUTHORIZED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_AUTHORIZATION_INFO_NOT_AUTHORIZED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.authorization_info.not_authorized = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_EGRESS_FILTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_EGRESS_FILTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.egress_filter = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_FRWRD_ACTION_TYPES_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_FRWRD_ACTION_TYPES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.frwrd_action_types = (TIMNA_FILTER_ACCEPT_FRWRD_ACTION_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_ACCEPTED_FRAMES_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_ACCEPTED_FRAMES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.accepted_frames = (TIMNA_FILTER_ACCEPT_FRAME_TYPE)param_val->numeric_equivalent;
  }

  ret = timna_filter_port_info_set(
          unit,
          sweept20_sys_info_get_local_port(unit, prm_port_ndx),
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_filter_port_info_set - FAIL", TRUE);
    timna_disp_result(ret, "timna_filter_port_info_set");
    goto exit;
  }

  send_string_to_screen(" *** timna_filter_port_info_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_SWEEPT20_API_EDIT/* { edit*/

/********************************************************************
 *  Function handler: port_info_set (section edit)
 */
int
  ui_sweept20_api_edit_vsid_port_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_port_ndx = 0;
  uint32
    prm_vsid_ndx = 0;
  uint8
    prm_tagged;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_edit");
  soc_sand_proc_name = "ui_sweept20_api_edit_vsid_port_info_set";

  unit = timna_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_EDIT_PORT_INFO_SET_PORT_INFO_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_EDIT_PORT_INFO_SET_PORT_INFO_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prt_ndx after port_info_set***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_EDIT_PORT_INFO_SET_PORT_INFO_SET_VSID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_EDIT_PORT_INFO_SET_PORT_INFO_SET_VSID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsd_ndx after port_info_set***", TRUE);
  }

  /* This is a set function, so call GET function first */
  ret = timna_edit_vsid_port_info_get(
          unit,
          prm_vsid_ndx,
          sweept20_sys_info_get_local_port(unit, prm_port_ndx),
          &prm_tagged
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_edit_vsid_port_info_get - FAIL", TRUE);
    sweept20_disp_result(ret, "timna_edit_vsid_port_info_get");
    goto exit;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_EDIT_PORT_INFO_SET_PORT_INFO_SET_TAGGED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_EDIT_PORT_INFO_SET_PORT_INFO_SET_TAGGED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tagged = (uint8)param_val->value.ulong_value;
  }



  /* Call function */
  ret = timna_edit_vsid_port_info_set(
          unit,
          prm_vsid_ndx,
          sweept20_sys_info_get_local_port(unit, prm_port_ndx),
          prm_tagged
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_edit_vsid_port_info_set - FAIL", TRUE);
    sweept20_disp_result(ret, "sweept20_edit_vsid_port_info_set");
    goto exit;
  }

  send_string_to_screen(" *** sweept20_edit_vsid_port_info_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: port_info_get (section edit)
 */
int
  ui_sweept20_api_edit_vsid_port_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_port_ndx = 0;
  uint32
    prm_vsid_ndx = 0;
  uint8
    prm_tagged;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_edit");
  soc_sand_proc_name = "sweept20_edit_vsid_port_info_get";

  unit = timna_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_EDIT_PORT_INFO_GET_PORT_INFO_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_EDIT_PORT_INFO_GET_PORT_INFO_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prt_ndx after port_info_get***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_EDIT_PORT_INFO_GET_PORT_INFO_GET_VSID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_EDIT_PORT_INFO_GET_PORT_INFO_GET_VSID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsd_ndx after port_info_get***", TRUE);
  }

  /* Call function */
  ret = timna_edit_vsid_port_info_get(
          unit,
          prm_vsid_ndx,
          sweept20_sys_info_get_local_port(unit, prm_port_ndx),
          &prm_tagged
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_edit_vsid_port_info_get - FAIL", TRUE);
    sweept20_disp_result(ret, "sweept20_edit_vsid_port_info_get");
    goto exit;
  }

  send_string_to_screen(" *** sweept20_edit_vsid_port_info_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> tagged:", TRUE);
  soc_sand_os_printf( "tagged: %u\n",prm_tagged);


  goto exit;
exit:
  return ui_ret;
}
#endif/* { UI_SWEEPT20_API_EDIT*/

#ifdef UI_SWEEPT20_API_PROFILER
int
  ui_sweept20_api_profiler_in_label_to_lif(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_in_label;
  uint32
    prm_lif;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_profiler");
  soc_sand_proc_name = "sweept20_profiler_in_label_to_lif";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PROFILER_IN_LABEL_TO_LIF_IN_LABEL_TO_LIF_IN_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_PROFILER_IN_LABEL_TO_LIF_IN_LABEL_TO_LIF_IN_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_in_label = (uint32)param_val->value.ulong_value;
  }

  ret = sweept20_profiler_in_label_to_lif(
          prm_in_label,
          &prm_lif
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_profiler_in_label_to_lif - FAIL", TRUE);
    sweept20_disp_result(ret, "sweept20_profiler_in_label_to_lif");
    goto exit;
  }

  send_string_to_screen(" *** sweept20_profiler_in_label_to_lif - SUCCEEDED", TRUE);
  send_string_to_screen("--> lif:", TRUE);
  soc_sand_os_printf( "lif: %lu\n",prm_lif);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_profiler_lif_to_in_label(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_lif;
  uint32
    prm_in_label;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_profiler");
  soc_sand_proc_name = "sweept20_profiler_lif_to_in_label";

  unit = timna_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PROFILER_LIF_TO_IN_LABEL_LIF_TO_IN_LABEL_LIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_PROFILER_LIF_TO_IN_LABEL_LIF_TO_IN_LABEL_LIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif = (uint32)param_val->value.ulong_value;
  }

  ret = sweept20_profiler_lif_to_in_label(
          prm_lif,
          &prm_in_label
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_profiler_lif_to_in_label - FAIL", TRUE);
    sweept20_disp_result(ret, "sweept20_profiler_lif_to_in_label");
    goto exit;
  }

  send_string_to_screen(" *** sweept20_profiler_lif_to_in_label - SUCCEEDED", TRUE);
  send_string_to_screen("--> in_label:", TRUE);
  soc_sand_os_printf( "in_label: %lu\n",prm_in_label);
  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_IPV4_UC
int
  ui_sweept20_api_ipv4_uc_ipv4_unicast_range_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_first_vrf_id = 0;
  uint32
    prm_last_vrf_id = 0;
  TIMNA_IPV4_UC_RANGE
    prm_range_info;
  TIMNA_IPV4_SYS_FEC_RANGE_TYPE
    prm_fec_range_type;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_id = 0;
  uint32
    prm_count;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_ipv4_uc_test");
  soc_sand_proc_name = "timna_ipv4_unicast_test_range_add";

  unit = timna_get_default_unit();
  timna_TIMNA_IPV4_UC_RANGE_clear(&prm_range_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_FIRST_VRF_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_FIRST_VRF_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_first_vrf_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_LAST_VRF_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_LAST_VRF_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_last_vrf_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_STEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_STEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.step = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_COUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_COUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.count = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_PREFIX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_PREFIX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.prefix = (uint8)param_val->value.ulong_value;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_TO_ID,1))
  {
    if (!get_val_of(
      current_line,(int *)&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_TO_ID,1,
      &param_val,VAL_IP,err_msg))
    {
      prm_range_info.to = param_val->value.ip_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_FROM_ID,1))
  {
    if (!get_val_of(
      current_line,(int *)&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_FROM_ID,1,
      &param_val,VAL_IP,err_msg))
    {
      prm_range_info.from = param_val->value.ip_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.type = (TIMNA_IPV4_RANGE_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_FEC_RANGE_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_FEC_RANGE_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_range_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_SYS_FEC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_SYS_FEC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_id = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
  }


  /* Call function */
  ret = timna_ipv4_unicast_test_range_add(
          unit,
          prm_first_vrf_id,
          prm_last_vrf_id,
          &prm_range_info,
          prm_fec_range_type,
          prm_sys_fec_id,
          &prm_count
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_ipv4_unicast_test_range_add - FAIL", TRUE);
    timna_disp_result(ret, "timna_ipv4_unicast_test_range_add");
    goto exit;
  }

  send_string_to_screen(" *** timna_ipv4_unicast_test_range_add - SUCCEEDED", TRUE);


  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_ipv4_uc_ipv4_unicast_range_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_first_vrf_id = 0;
  uint32
    prm_last_vrf_id = 0;
  TIMNA_IPV4_UC_RANGE
    prm_range_info;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_ipv4_uc_test");
  soc_sand_proc_name = "timna_ipv4_unicast_test_range_remove";

  unit = timna_get_default_unit();
  timna_TIMNA_IPV4_UC_RANGE_clear(&prm_range_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_FIRST_VRF_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_FIRST_VRF_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_first_vrf_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_LAST_VRF_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_LAST_VRF_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_last_vrf_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_STEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_STEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.step = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_COUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_COUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.count = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_PREFIX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_PREFIX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.prefix = (uint8)param_val->value.ulong_value;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_TO_ID,1))
  {
    if (!get_val_of(
      current_line,(int *)&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_TO_ID,1,
      &param_val,VAL_IP,err_msg))
    {
      prm_range_info.to = param_val->value.ip_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_FROM_ID,1))
  {
    if (!get_val_of(
      current_line,(int *)&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_FROM_ID,1,
      &param_val,VAL_IP,err_msg))
    {
      prm_range_info.from = param_val->value.ip_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.type = (TIMNA_IPV4_RANGE_TYPE)param_val->numeric_equivalent;
  }



  /* Call function */
  ret = timna_ipv4_unicast_test_range_remove(
          unit,
          prm_first_vrf_id,
          prm_last_vrf_id,
          &prm_range_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_ipv4_unicast_test_range_remove - FAIL", TRUE);
    timna_disp_result(ret, "timna_ipv4_unicast_test_range_remove");
    goto exit;
  }

  send_string_to_screen(" *** timna_ipv4_unicast_test_range_remove - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);


  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_ipv4_uc_ipv4_unicast_route_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vrf_id = 0;
  SOC_SAND_PP_IPV4_SUBNET
    prm_route_key;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_id = 0;
  uint8
    prm_override;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_ipv4_uc");
  soc_sand_proc_name = "timna_ipv4_unicast_route_add";

  unit = timna_get_default_unit();
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_route_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_VRF_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_VRF_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vrf_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_ROUTE_KEY_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_ROUTE_KEY_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_ROUTE_KEY_IP_ADDRESS_ID,1))
  {
    if (!get_val_of(
      current_line,(int *)&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_ROUTE_KEY_IP_ADDRESS_ID,1,
      &param_val,VAL_IP,err_msg))
    {
      prm_route_key.ip_address = param_val->value.ip_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_SYS_FEC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_SYS_FEC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_id = (SOC_SAND_PP_SYSTEM_FEC_ID)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_OVERRIDE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_OVERRIDE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_override = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = timna_ipv4_unicast_route_add(
          unit,
          prm_vrf_id,
          &prm_route_key,
          prm_sys_fec_id,
          prm_override,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_ipv4_unicast_route_add - FAIL", TRUE);
    timna_disp_result(ret, "timna_ipv4_unicast_route_add");
    goto exit;
  }

  send_string_to_screen(" *** timna_ipv4_unicast_route_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n",prm_success);


  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_ipv4_uc_ipv4_unicast_route_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vrf_id = 0;
  SOC_SAND_PP_IPV4_SUBNET
    prm_route_key;
  uint8
    prm_exact_match;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_id = 0;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_ipv4_uc");
  soc_sand_proc_name = "timna_ipv4_unicast_route_get";

  unit = timna_get_default_unit();
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_route_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_VRF_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_VRF_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vrf_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_ROUTE_KEY_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_ROUTE_KEY_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_ROUTE_KEY_IP_ADDRESS_ID,1))
  {
    if (!get_val_of(
      current_line,(int *)&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_ROUTE_KEY_IP_ADDRESS_ID,1,
      &param_val,VAL_IP,err_msg))
    {
      prm_route_key.ip_address = param_val->value.ip_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_EXACT_MATCH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_EXACT_MATCH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_exact_match = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = timna_ipv4_unicast_route_get(
          unit,
          prm_vrf_id,
          &prm_route_key,
          prm_exact_match,
          &prm_sys_fec_id,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_ipv4_unicast_route_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_ipv4_unicast_route_get");
    goto exit;
  }

  send_string_to_screen(" *** timna_ipv4_unicast_route_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> sys_fec_id:", TRUE);
  soc_sand_os_printf( "prm_sys_fec_id: %lu\n",prm_sys_fec_id);

  send_string_to_screen("--> found:", TRUE);
  soc_sand_os_printf( "found: %u\n",prm_found);


  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_ipv4_uc_ipv4_unicast_route_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vrf_id = 0;
  SOC_SAND_PP_IPV4_SUBNET
    prm_route_key;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_ipv4_uc");
  soc_sand_proc_name = "timna_ipv4_unicast_route_remove";

  unit = timna_get_default_unit();
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_route_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_REMOVE_IPV4_UNICAST_ROUTE_REMOVE_VRF_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_REMOVE_IPV4_UNICAST_ROUTE_REMOVE_VRF_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vrf_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_REMOVE_IPV4_UNICAST_ROUTE_REMOVE_ROUTE_KEY_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_REMOVE_IPV4_UNICAST_ROUTE_REMOVE_ROUTE_KEY_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_REMOVE_IPV4_UNICAST_ROUTE_REMOVE_ROUTE_KEY_IP_ADDRESS_ID,1))
  {
    if (!get_val_of(
      current_line,(int *)&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_REMOVE_IPV4_UNICAST_ROUTE_REMOVE_ROUTE_KEY_IP_ADDRESS_ID,1,
      &param_val,VAL_IP,err_msg))
    {
       prm_route_key.ip_address = param_val->value.ip_value;
    }
  }


  /* Call function */
  ret = timna_ipv4_unicast_route_remove(
          unit,
          prm_vrf_id,
          &prm_route_key,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_ipv4_unicast_route_remove - FAIL", TRUE);
    timna_disp_result(ret, "timna_ipv4_unicast_route_remove");
    goto exit;
  }

  send_string_to_screen(" *** timna_ipv4_unicast_route_remove - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n",prm_success);


  goto exit;
exit:
  return ui_ret;
}

int
  ui_timna_app_ipv4_uc_ipv4_unicast_fec_range_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_first_outlif;
  uint32
    prm_last_outlif;
  TIMNA_IPV4_UC_FEC_INFO
    prm_fec_info;
  TIMNA_IPV4_UC_FEC_RANGE_INFO
    prm_fec_range_info;

  UI_MACROS_INIT_FUNCTION("ui_timna_app_ipv4_uc");
  soc_sand_proc_name = "timna_ipv4_unicast_fec_range_set";

  unit = timna_get_default_unit();
  timna_TIMNA_IPV4_UC_FEC_INFO_clear(&prm_fec_info);
  timna_TIMNA_IPV4_UC_FEC_RANGE_INFO_clear(&prm_fec_range_info);
  /* Get parameters */

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_FIRST_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_FIRST_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_first_outlif = (uint32)param_val->value.ulong_value;
    prm_last_outlif = prm_first_outlif;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_LAST_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_LAST_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_last_outlif = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_ENCAP_POINTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_ENCAP_POINTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_info.edit_info.encap_pointer = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_LABEL_TTL_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_LABEL_TTL_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_info.edit_info.vc_label.ttl_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_LABEL_EXP_QOS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_LABEL_EXP_QOS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_info.edit_info.vc_label.exp_qos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_LABEL_LABEL_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_LABEL_LABEL_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_info.edit_info.vc_label.label_id = (uint32)param_val->value.ulong_value;
    prm_fec_info.edit_info.vc_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_VC_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_VC_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_info.edit_info.vc_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_SYS_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_SYS_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_info.sys_port.sys_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_INC_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_INC_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_range_info.inc_encap_pointer = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_INC_VC_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_INC_VC_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_range_info.inc_vc_label = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = timna_ipv4_unicast_test_edit_range_set(
          unit,
          prm_first_outlif,
          prm_last_outlif,
          &prm_fec_info,
          &prm_fec_range_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_ipv4_unicast_fec_range_set - FAIL", TRUE);
    timna_disp_result(ret, "timna_ipv4_unicast_fec_range_set");
    goto exit;
  }

  send_string_to_screen(" *** timna_ipv4_unicast_fec_range_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: ipv4_unicast_route_get_block (section ipv4_uc)
 */
int
  ui_sweept20_api_ipv4_uc_ipv4_unicast_route_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TIMNA_IPV4_UC_ROUTE_TABLE_STATUS_INFO
    prm_stat;
  uint32
    prm_vrf_id = 0;
  TIMNA_L3_TABLE_ITER
    prm_iter;
  uint32
    prm_entries_to_scan;
  uint32
    prm_entries_to_get;
  TIMNA_IPV4_UC_ROUTE_ENTRY
    prm_route_table[100];
  TIMNA_L3_TABLE_ITER_TYPE
    prm_get_type;
  char
    decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING + 3];
  char
    user_msg[5] = "";
  uint32
    con;
  uint32
    prm_nof_entries,
    indx,
    cur_scan,
    cur_read,
    total_entries;


  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_ipv4_uc");
  soc_sand_proc_name = "timna_ipv4_unicast_route_get_block";
  prm_get_type = TIMNA_L3_TABLE_ITER_TYPE_FAST;

  unit = timna_get_default_unit();
  timna_TIMNA_IPV4_UC_ROUTE_TABLE_STATUS_INFO_clear(&prm_stat);

  timna_TIMNA_L3_TABLE_ITER_clear(&prm_iter);
  prm_entries_to_get = 1000;
  prm_entries_to_scan = 1000;
  cur_scan = 500;
  cur_read = 15;
  total_entries = 0;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_IPV4_UNICAST_ROUTE_GET_BLOCK_IPV4_UNICAST_ROUTE_GET_BLOCK_VRF_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_IPV4_UNICAST_ROUTE_GET_BLOCK_IPV4_UNICAST_ROUTE_GET_BLOCK_VRF_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vrf_id = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vrf_if ***", TRUE);
    goto exit;
  }

  prm_iter.type = prm_get_type;


 ret = timna_ipv4_unicast_route_table_status(
          unit,
          prm_vrf_id,
          &prm_stat
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_ipv4_unicast_route_get_block - FAIL", TRUE);
    timna_disp_result(ret, "timna_ipv4_unicast_route_get_block");
    goto exit;
  }

  total_entries = prm_stat.used;
  send_string_to_screen("--> nof_entries:", TRUE);
  soc_sand_os_printf( "nof_entries: %lu\n",total_entries);

  send_string_to_screen("/-----------------------------------------------\\", TRUE);
  send_string_to_screen("|                 Routing Table                 |", TRUE);
  send_string_to_screen("|-----------------------------------------------|", TRUE);
  send_string_to_screen("|   Destination IP         |      Sys FEC       |", TRUE);
  send_string_to_screen("|-----------------------------------------------|", TRUE);

  while (!TIMNA_L3_TABLE_ITER_IS_END(&prm_iter.payload) && prm_entries_to_get && prm_entries_to_scan)
  {
    if (prm_entries_to_get <= cur_read )
    {
      cur_read = prm_entries_to_get;
      prm_entries_to_get = 0;
    }
    else
    {
      prm_entries_to_get -= cur_read;
    }
    /* Call function */
    ret = timna_ipv4_unicast_route_get_block(
            unit,
            prm_vrf_id,
            &prm_iter,
            prm_entries_to_scan,
            cur_read,
            prm_route_table,
            &prm_nof_entries
          );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        send_string_to_screen(" *** timna_ipv4_unicast_route_get_block - FAIL", TRUE);
        timna_disp_result(ret, "timna_ipv4_unicast_route_get_block");
        goto exit;
      }
    for(indx = 0; indx < prm_nof_entries; ++indx)
    {
      soc_sand_pp_ip_long_to_string(prm_route_table[indx].key.ip_address,1,decimal_ip);
      sal_sprintf(decimal_ip,"%s/%lu",decimal_ip,prm_route_table[indx].key.prefix_len);
      soc_sand_os_printf( "|     %-20s |", decimal_ip);
      soc_sand_os_printf( "         %-10lu |",prm_route_table[indx].sys_fec_id);
      send_string_to_screen("\n|-----------------------------------------------|", TRUE);
    }
    if (!TIMNA_L3_TABLE_ITER_IS_END(&prm_iter.payload))
    {
      soc_sand_os_printf( "Hit Space to continue/Any Key to Abort.\n");
      con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
      if (!con)
      {
        break;
      }
    }
  }
  send_string_to_screen("--> nof_entries:", TRUE);
  soc_sand_os_printf( "nof_entries: %lu\n",total_entries);
  goto exit;
exit:
  return ui_ret;
}


#endif
#ifdef UI_SWEEPT20_API_INIT
int
  ui_sweept20_api_init(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_init");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_GENERIC_INIT_GENERIC_INIT_ID,1))
  {
    ui_ret = ui_sweept20_api_init_generic_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_RESET_FAP_ID,1))
  {
    ui_ret = sweep_timna_app_reset_tevb();
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_STAND_ALONE_INIT_STAND_ALONE_INIT_ID,1))
  {
    ui_ret = ui_sweept20_api_init_stand_alone_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_ID,1))
  {
    ui_ret = ui_sweept20_api_init_bridge_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_AD_PEB_INIT_AD_PEB_INIT_ID,1))
  {
    ui_ret = ui_sweept20_api_init_ad_peb_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_ID,1))
  {
    ui_ret = ui_sweept20_api_init_vpls_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MPLS_LSR_INIT_MPLS_LSR_INIT_ID,1))
  {
    ui_ret = ui_sweept20_api_init_mpls_lsr_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_ROUTING_INIT_IPV4_ROUTING_INIT_ID,1))
  {
    ui_ret = ui_sweept20_api_init_ipv4_routing_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_INIT_IPV4_ACL_INIT_ID,1))
  {
    ui_ret = ui_sweept20_api_init_acl_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ENABLE_TRAFFIC_ENABLE_TRAFFIC_ID,1))
  {
    ui_ret = ui_sweept20_api_init_enable_traffic(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_CLOSE_CLOSE_ID,1))
  {
    ui_ret = ui_sweept20_api_init_close(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after init***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_PORTS
int
  ui_sweept20_api_ports(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_ports");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PORTS_MAPPING_TO_SYS_PORTS_MAPPING_TO_SYS_PORTS_ID,1))
  {
    ui_ret = ui_sweept20_api_ports_mapping_to_sys_ports(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_ID,1))
  {
    ui_ret = ui_sweept20_api_ports_map_to_lag(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ports***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_MAC
int
  ui_sweept20_api_mac(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_mac");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_LEARN_AGENT_LEARN_AGENT_ID,1))
  {
    ui_ret = ui_sweept20_api_mac_learn_agent(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_PROCESS_CPU_MSG_PROCESS_CPU_MSG_ID,1))
  {
    ui_ret = ui_sweept20_api_mac_process_cpu_msg(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_ID,1))
  {
    ui_ret = ui_sweept20_api_mac_range_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_ID,1))
  {
    ui_ret = ui_sweept20_api_mac_range_remove(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mac***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_FILTER
int
  ui_sweept20_api_filter(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_mac");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_ID,1))
  {
    ui_ret = ui_sweept20_api_filter_port_info_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mac***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
#endif

#ifdef UI_SWEEPT20_API_EDIT/* { edit*/
/********************************************************************
 *  Section handler: edit
 */
int
  ui_sweept20_api_edit(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_edit");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_EDIT_PORT_INFO_SET_PORT_INFO_SET_ID,1))
  {
    ui_ret = ui_sweept20_api_edit_vsid_port_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_EDIT_PORT_INFO_GET_PORT_INFO_GET_ID,1))
  {
    ui_ret = ui_sweept20_api_edit_vsid_port_info_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after edit***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { UI_SWEEPT20_API_EDIT*/

#ifdef UI_SWEEPT20_API_MACT
/********************************************************************
 *  Function handler: mact_entry_get_block (section mac_lkup)
 */
int
  ui_timna_app_mac_lkup_mact_entry_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_freeze_mact;
  TIMNA_MACT_ENTRIES_RANGE
    prm_range;
  TIMNA_MACT_MATCH_RULE
    prm_rule;
  TIMNA_MACT_ENTRY_INFO
    prm_table[1000];
  uint32
    prm_nof_entires;
  uint32
    prm_end_indx,
    tmp_count;
  uint32
    indx = 0;

  UI_MACROS_INIT_FUNCTION("ui_timna_app_mac_lkup");
  soc_sand_proc_name = "timna_mact_entry_get_block";

  unit = timna_get_default_unit();
  timna_TIMNA_MACT_ENTRIES_RANGE_clear(&prm_range);
  timna_TIMNA_MACT_MATCH_RULE_clear(&prm_rule);
  prm_nof_entires = 0;

  prm_rule.including_static = TRUE;
  prm_rule.including_dynamic = TRUE;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_FREEZE_MACT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_FREEZE_MACT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_freeze_mact = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RANGE_ENTRIES_TO_ACT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RANGE_ENTRIES_TO_ACT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range.entries_to_act = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RANGE_ENTRIES_TO_SCAN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RANGE_ENTRIES_TO_SCAN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range.entries_to_scan = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RANGE_START_INDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RANGE_START_INDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range.start_indx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_FLUSH_STATIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_FLUSH_STATIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.including_static = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_FLUSH_DYNAMIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_FLUSH_DYNAMIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.including_dynamic = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_DEST_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_DEST_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.dest.dest_val = (uint32)param_val->value.ulong_value;
  }
  else

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.dest.dest_type = (SOC_SAND_PP_DEST_TYPE)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_COMPARE_DEST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_COMPARE_DEST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.compare_dest = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.fid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_COMPARE_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_COMPARE_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.compare_fid = (uint8)param_val->value.ulong_value;
  }

  if (!prm_rule.compare_fid)
  {
    tmp_count = prm_range.entries_to_act;
    ret = timna_mact_entry_get_count(
            unit,
            &prm_range,
            &prm_rule,
            &prm_nof_entires
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** timna_mact_entry_get_block - FAIL", TRUE);
      timna_disp_result(ret, "timna_mact_entry_get_block");
      goto exit;
    }
    prm_range.entries_to_act = tmp_count;
    soc_sand_os_printf( "number of entries: %ld\n", prm_nof_entires);
    if (prm_nof_entires >= 1000 || timna_get_print_level()==0 )
    {
      soc_sand_os_printf( "number of entries: %ld\n", prm_nof_entires);
      goto exit;
    }
  }

  ret = timna_mact_entry_get_block(
          unit,
          prm_freeze_mact,
          &prm_range,
          &prm_rule,
          prm_table,
          &prm_nof_entires,
          &prm_end_indx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_mact_entry_get_block - FAIL", TRUE);
    timna_disp_result(ret, "timna_mact_entry_get_block");
    goto exit;
  }

  send_string_to_screen(" *** timna_mact_entry_get_block - SUCCEEDED", TRUE);

  soc_sand_os_printf( "|---------------------------------------------------------------------------------------------------|\r\n");
  soc_sand_os_printf( "|                                               MAC Table                                           |\r\n");
  soc_sand_os_printf( "|---------------------------------------------------------------------------------------------------|\r\n");
  soc_sand_os_printf( "|    FID    |        MAC        |   Dest Type   |   Dest Val   |Age Status| Dynamic |  Forward Type |\r\n");
  soc_sand_os_printf( "|---------------------------------------------------------------------------------------------------|\r\n");
  for (indx = 0; indx < prm_nof_entires; ++indx)
  {
    soc_sand_os_printf( "| %9ld | %02x:%02x:%02x:%02x:%02x:%02x |",
      prm_table[indx].key.fid,
      prm_table[indx].key.mac.address[0],
      prm_table[indx].key.mac.address[1],
      prm_table[indx].key.mac.address[2],
      prm_table[indx].key.mac.address[3],
      prm_table[indx].key.mac.address[4],
      prm_table[indx].key.mac.address[5]
    );
    soc_sand_SAND_PP_DESTINATION_ID_table_format_print(&(prm_table[indx].value.da_info.destination));
    soc_sand_os_printf( "     %-4s |",  timna_TIMNA_MACT_AGE_STATUS_to_string_short(prm_table[indx].value.aging_info.age_status));
    if (prm_table[indx].value.sa_info.is_dynamic)
    {
      soc_sand_os_printf( "    V    |");
    }
    else
    {
      soc_sand_os_printf( "    X    |" );
    }
    soc_sand_os_printf( " %-13s |\n",  soc_sand_SAND_PP_FRWRD_ACTION_TYPE_to_string_short(prm_table[indx].value.da_info.forward_type));
    soc_sand_os_printf( "|---------------------------------------------------------------------------------------------------|\r\n");
  }

  soc_sand_os_printf( "Number of Entires: %ld\n",prm_nof_entires);
  soc_sand_os_printf( "End indx: %ld\n",prm_end_indx);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mact_flush (section mac_lkup)
 */
int
  ui_timna_app_mac_lkup_mact_flush(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TIMNA_MACT_FLUSH_INFO
    prm_flush_info;

  UI_MACROS_INIT_FUNCTION("ui_timna_app_mac_lkup");
  soc_sand_proc_name = "timna_mact_flush";

  unit = timna_get_default_unit();
  timna_TIMNA_MACT_FLUSH_INFO_clear(&prm_flush_info);

  prm_flush_info.mode = TIMNA_MACT_FLUSH_MODE_CLEAR;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_NEW_DEST_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_NEW_DEST_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.new_dest.dest_val = (uint32)param_val->value.ulong_value;
    prm_flush_info.mode = TIMNA_MACT_FLUSH_MODE_TRANSPLANT;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_NEW_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_NEW_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.new_dest.dest_type = (SOC_SAND_PP_DEST_TYPE)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_FLUSH_STATIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_FLUSH_STATIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.including_static = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_FLUSH_DYNAMIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_FLUSH_DYNAMIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.including_dynamic = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_DEST_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_DEST_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.dest.dest_val = (uint32)param_val->value.ulong_value;
    prm_flush_info.rule.compare_dest = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.dest.dest_type = (SOC_SAND_PP_DEST_TYPE)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_COMPARE_DEST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_COMPARE_DEST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.compare_dest = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.fid = (uint32)param_val->value.ulong_value;
    prm_flush_info.rule.compare_fid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_COMPARE_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_COMPARE_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.compare_fid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.mode = (TIMNA_MACT_FLUSH_MODE)param_val->numeric_equivalent;
  }

  ret = timna_mact_flush(
          unit,
          &prm_flush_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_mact_flush - FAIL", TRUE);
    timna_disp_result(ret, "timna_mact_flush");
    goto exit;
  }

  send_string_to_screen(" *** timna_mact_flush - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mact_aging_info_set (section mac_lkup)
 */
int
  ui_timna_app__mac_lkup_mact_aging_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TIMNA_MACT_AGING_INFO
    prm_aging_info,
    exact_aging_info;

  UI_MACROS_INIT_FUNCTION("ui_timna_app_mac_lkup");
  soc_sand_proc_name = "timna_mact_aging_info_set";

  unit = timna_get_default_unit();
  timna_TIMNA_MACT_AGING_INFO_clear(&prm_aging_info);
  timna_TIMNA_MACT_AGING_INFO_clear(&exact_aging_info);

  ret = timna_mact_aging_info_get(
          unit,
          &prm_aging_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_mact_aging_info_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_mact_aging_info_get");
    goto exit;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_AGING_INFO_SET_MACT_AGING_INFO_SET_AGING_INFO_CYCLE_NANO_SEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_AGING_INFO_SET_MACT_AGING_INFO_SET_AGING_INFO_CYCLE_NANO_SEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_aging_info.cycle.nano_sec = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_AGING_INFO_SET_MACT_AGING_INFO_SET_AGING_INFO_CYCLE_SEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_AGING_INFO_SET_MACT_AGING_INFO_SET_AGING_INFO_CYCLE_SEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_aging_info.cycle.sec = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_AGING_INFO_SET_MACT_AGING_INFO_SET_AGING_INFO_ENABLE_AGING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_MACT_AGING_INFO_SET_MACT_AGING_INFO_SET_AGING_INFO_ENABLE_AGING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_aging_info.enable_aging = (uint8)param_val->value.ulong_value;
  }

  ret = timna_mact_aging_info_set(
          unit,
          &prm_aging_info,
          &exact_aging_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_mact_aging_info_set - FAIL", TRUE);
    timna_disp_result(ret, "timna_mact_aging_info_set");
    goto exit;
  }

  send_string_to_screen(" *** timna_mact_aging_info_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_mact_lkup(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_mac");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_ID,1))
  {
    ui_ret = ui_timna_app_mac_lkup_mact_flush(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_ID,1))
  {
    ui_ret = ui_timna_app_mac_lkup_mact_entry_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MACT_AGING_INFO_SET_MACT_AGING_INFO_SET_ID,1))
  {
    ui_ret = ui_timna_app__mac_lkup_mact_aging_info_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mact***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_VID_RES
int
  ui_sweept20_api_vid_res_vid_port_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_port_ndx = 0;
  TIMNA_VID_PORT_INFO
    prm_port_info;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vid_res");
  soc_sand_proc_name = "timna_vid_port_info_set";

  unit = timna_get_default_unit();
  timna_TIMNA_VID_PORT_INFO_clear(&prm_port_info);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prt_ndx after vid_port_info_set***", TRUE);
  }

  ret = timna_vid_port_info_get(
          unit,
          sweept20_sys_info_get_local_port(unit, prm_port_ndx),
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_vid_port_info_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_vid_port_info_get");
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_PORT_INFO_ENABLE_PROTOCOL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_PORT_INFO_ENABLE_PROTOCOL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.enable_protocol = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_PORT_INFO_ENABLE_SUBNET_IP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_PORT_INFO_ENABLE_SUBNET_IP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.enable_subnet_ip = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_PORT_INFO_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_PORT_INFO_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.pvid = (uint32)param_val->value.ulong_value;
  }

  ret = timna_vid_port_info_set(
          unit,
          sweept20_sys_info_get_local_port(unit, prm_port_ndx),
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_vid_port_info_set - FAIL", TRUE);
    timna_disp_result(ret, "timna_vid_port_info_set");
    goto exit;
  }

  send_string_to_screen(" *** timna_vid_port_info_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: vid_port_info_get (section vid_res)
 */
int
  ui_sweept20_api_vid_res_vid_port_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_LOCAL_PORT_ID
    prm_port_ndx = 0;
  TIMNA_VID_PORT_INFO
    prm_port_info;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vid_res");
  soc_sand_proc_name = "timna_vid_port_info_get";

  unit = timna_get_default_unit();
  timna_TIMNA_VID_PORT_INFO_clear(&prm_port_info);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_PORT_INFO_GET_VID_PORT_INFO_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VID_PORT_INFO_GET_VID_PORT_INFO_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prt_ndx after vid_port_info_get***", TRUE);
  }

  ret = timna_vid_port_info_get(
          unit,
          sweept20_sys_info_get_local_port(unit, prm_port_ndx),
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_vid_port_info_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_vid_port_info_get");
    goto exit;
  }

  send_string_to_screen(" *** timna_vid_port_info_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> port_info:", TRUE);
  timna_TIMNA_VID_PORT_INFO_print(&prm_port_info);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_vid_res_vid_cos_ip_subnet_info_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_subnet_ip_ndx = 0;
  TIMNA_VID_SUBNET_IP_INFO
    prm_subnet_ip_info;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vid_res");
  soc_sand_proc_name = "timna_vid_cos_ip_subnet_info_set";

  unit = timna_get_default_unit();
  timna_TIMNA_VID_SUBNET_IP_INFO_clear(&prm_subnet_ip_info);


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_CVLAN_USER_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_CVLAN_USER_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_subnet_ip_info.cvlan_user_priority = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_subnet_ip_info.traffic_class = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_subnet_ip_info.vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_SUBNET_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_SUBNET_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_subnet_ip_info.subnet_mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_IS_ENTRY_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_IS_ENTRY_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_subnet_ip_info.is_entry_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_IP_ADDRESS_ID,1))
  {
    if (!get_val_of(
      current_line,(int *)&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_IP_ADDRESS_ID,1,
      &param_val,VAL_IP,err_msg))
    {
       prm_subnet_ip_info.ip_address = param_val->value.ip_value;
    }
  }

  ret = sweept20_vlan_vid_resolution_ip_subnet_info_add(
          unit,
          &prm_subnet_ip_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_vid_cos_ip_subnet_info_set - FAIL", TRUE);
    timna_disp_result(ret, "timna_vid_cos_ip_subnet_info_set");
    goto exit;
  }

  send_string_to_screen(" *** timna_vid_cos_ip_subnet_info_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_vid_res_vid_cos_ip_subnet_info_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_subnet_ip_ndx = 0;
  uint32
    ip_address,
    subnet_mask;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vid_res");
  soc_sand_proc_name = "timna_vid_cos_ip_subnet_info_set";

  unit = timna_get_default_unit();


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_SUBNET_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_SUBNET_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    subnet_mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_IP_ADDRESS_ID,1))
  {
    if (!get_val_of(
      current_line,(int *)&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_IP_ADDRESS_ID,1,
      &param_val,VAL_IP,err_msg))
    {
       ip_address = param_val->value.ip_value;
    }
  }

  ret = sweept20_vlan_vid_resolution_ip_subnet_info_remove(
          unit,
          ip_address,
          subnet_mask
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** sweept20_vlan_vid_resolution_ip_subnet_info_remove - FAIL", TRUE);
    timna_disp_result(ret, "sweept20_vlan_vid_resolution_ip_subnet_info_remove");
    goto exit;
  }

  send_string_to_screen(" *** sweept20_vlan_vid_resolution_ip_subnet_info_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_vid_res_vid_cos_ip_subnet_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_subnet_ip_ndx = 0;
  TIMNA_VID_SUBNET_IP_INFO
    prm_subnet_ip_info;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vid_res");
  soc_sand_proc_name = "timna_vid_cos_ip_subnet_info_get";

  unit = timna_get_default_unit();
  timna_TIMNA_VID_SUBNET_IP_INFO_clear(&prm_subnet_ip_info);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_GET_VID_COS_IP_SUBNET_INFO_GET_SUBNET_IP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_GET_VID_COS_IP_SUBNET_INFO_GET_SUBNET_IP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_subnet_ip_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sbnt_ip_ndx after vid_cos_ip_subnet_info_get***", TRUE);
  }

  ret = timna_vid_cos_ip_subnet_info_get(
          unit,
          prm_subnet_ip_ndx,
          &prm_subnet_ip_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** timna_vid_cos_ip_subnet_info_get - FAIL", TRUE);
    timna_disp_result(ret, "timna_vid_cos_ip_subnet_info_get");
    goto exit;
  }

  send_string_to_screen(" *** timna_vid_cos_ip_subnet_info_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> subnet_ip_info:", TRUE);
  timna_TIMNA_VID_SUBNET_IP_INFO_print(&prm_subnet_ip_info);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_sweept20_api_vid_res(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vid_res");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_ID,1))
  {
    ui_ret = ui_sweept20_api_vid_res_vid_port_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_PORT_INFO_GET_VID_PORT_INFO_GET_ID,1))
  {
    ui_ret = ui_sweept20_api_vid_res_vid_port_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_ID,1))
  {
    ui_ret = ui_sweept20_api_vid_res_vid_cos_ip_subnet_info_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_REMOVE_VID_COS_IP_SUBNET_INFO_REMOVE_ID,1))
  {
    ui_ret = ui_sweept20_api_vid_res_vid_cos_ip_subnet_info_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_GET_VID_COS_IP_SUBNET_INFO_GET_ID,1))
  {
    ui_ret = ui_sweept20_api_vid_res_vid_cos_ip_subnet_info_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after vid_res***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
#endif
#ifdef UI_SWEEPT20_API_VSI
int
  ui_sweept20_api_vsi(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_vsi");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_OPEN_OPEN_ID,1))
  {
    ui_ret = ui_sweept20_api_vsi_open(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_UPDATE_UPDATE_ID,1))
  {
    ui_ret = ui_sweept20_api_vsi_update(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_INFO_DISPLAY_INFO_DISPLAY_ID,1))
  {
    ui_ret = ui_sweept20_api_vsi_info_display(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_OUTLIF_INFO_DISPLAY_OUTLIF_INFO_DISPLAY_ID,1))
  {
    ui_ret = ui_sweept20_api_vsi_outlif_info_display(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_ID,1))
  {
    ui_ret = ui_sweept20_api_vlan_ports_add_remove(current_line, FALSE);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VLAN_PORTS_REMOVE_PORTS_REMOVE_ID,1))
  {
    ui_ret = ui_sweept20_api_vlan_ports_add_remove(current_line, TRUE);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_ID,1))
  {
    ui_ret = ui_sweept20_api_vpls_vc_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_ID,1))
  {
    ui_ret = ui_sweept20_api_vpls_vc_remove(current_line);
  }

  else
  {
    send_string_to_screen(" *** SW error - expecting function name after vsi***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_POLICER
int
  ui_sweept20_api_policer(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_policer");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_ID,1))
  {
    ui_ret = ui_sweept20_api_policer_map_range_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_MAP_RANGE_GET_MAP_RANGE_GET_ID,1))
  {
    ui_ret = ui_sweept20_api_policer_map_range_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_ID,1))
  {
    ui_ret = ui_sweept20_api_policer_params_range_set(current_line, TRUE);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_GET_ID,1))
  {
    ui_ret = ui_sweept20_api_policer_params_range_set(current_line, FALSE);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_VSID_IN_VC_ASSOCIATED_POLICER_GET_IN_VC_ASSOCIATED_POLICER_GET_ID,1))
  {
    ui_ret = ui_timna_app_vsid_in_vc_associated_policer_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ID,1))
  {
    ui_ret = ui_timna_app_vsid_port_vlan_associated_policer_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_POLICER_GLBL_INFO_GET_GLBL_INFO_GET_ID,1))
  {
    ui_ret = ui_timna_app_policer_glbl_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_POLICER_GLBL_INFO_SET_GLBL_INFO_SET_ID,1))
  {
    ui_ret = ui_timna_app_policer_glbl_info_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after policer***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif


#ifdef UI_SWEEPT20_API_EGRESS_ENCAP
int
  ui_sweept20_api_egress_encap(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_egress_encap");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_ID,1))
  {
    ui_ret = ui_sweept20_api_egress_encap_pointer_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EGRESS_ENCAP_GET_EGRESS_ENCAP_GET_ID,1))
  {
    ui_ret = ui_sweept20_api_egress_encap_pointer_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after egress_encap***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
#endif

#ifdef UI_SWEEPT20_API_MPLS
int
  ui_sweept20_api_mpls(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_mpls");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_ID,1))
  {
    ui_ret = ui_sweept20_api_mpls_lsr_label_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MPLS_LSR_LABEL_GET_LSR_LABEL_GET_ID,1))
  {
    ui_ret = ui_sweept20_api_mpls_lsr_label_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mpls***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_ACL
int
  ui_sweept20_api_acl(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_acl");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_TEMPLATE_ADD_TEMPLATE_ADD_ID,1))
  {
    ui_ret = ui_sweept20_api_acl_template_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ID,1))
  {
    ui_ret = ui_sweept20_api_acl_ace_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ID,1))
  {
    ui_ret = ui_sweept20_api_acl_ace_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ACE_REMOVE_ACE_REMOVE_ID,1))
  {
    ui_ret = ui_sweept20_api_acl_ace_remove(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after acl***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_BSP
int
  ui_sweept20_api_bsp(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_bsp");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BSP_RESET_DEVICE_RESET_DEVICE_ID,1))
  {
    ui_ret = ui_sweept20_api_bsp_reset_device(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after bsp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_SYS_INFO
int
  ui_sweept20_api_sys_info(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_sys_info");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_SYS_INFO_INIT_INIT_ID,1))
  {
    ui_ret = ui_sweept20_api_sys_info_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_SYS_INFO_IS_LOCAL_PORT_IS_LOCAL_PORT_ID,1))
  {
    ui_ret = ui_sweept20_api_sys_info_is_local_port(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_SYS_INFO_GET_LOCAL_PORT_GET_LOCAL_PORT_ID,1))
  {
    ui_ret = ui_sweept20_api_sys_info_get_local_port(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after sys_info***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_8021AD
int
  ui_sweept20_api_8021ad(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_8021ad");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_C_VID_REGISTER_C_VID_REGISTER_ID,1))
  {
    ui_ret = ui_sweept20_api_8021ad_c_vid_register(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_ID,1))
  {
    ui_ret = ui_sweept20_api_8021ad_pep_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_INFO_GET_PEP_INFO_GET_ID,1))
  {
    ui_ret = ui_sweept20_api_8021ad_pep_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_CEP_INFO_SET_CEP_INFO_SET_ID,1))
  {
    ui_ret = ui_sweept20_api_8021ad_cep_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_CEP_INFO_GET_CEP_INFO_GET_ID,1))
  {
    ui_ret = ui_sweept20_api_8021ad_cep_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_C_VID_UNREGISTER_C_VID_UNREGISTER_ID,1))
  {
    ui_ret = ui_sweept20_api_8021ad_c_vid_unregister(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_ID,1))
  {
    ui_ret = ui_sweept20_api_8021ad_pep_registeration_status_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_C_VID_REGISTERATION_STATUS_GET_C_VID_REGISTERATION_STATUS_GET_ID,1))
  {
    ui_ret = ui_sweept20_api_8021ad_c_vid_registeration_status_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after qinq***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_SWEEPT20_API_PROFILER
int
  ui_sweept20_api_profiler(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_profiler");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PROFILER_IN_LABEL_TO_LIF_IN_LABEL_TO_LIF_ID,1))
  {
    ui_ret = ui_sweept20_api_profiler_in_label_to_lif(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PROFILER_LIF_TO_IN_LABEL_LIF_TO_IN_LABEL_ID,1))
  {
    ui_ret = ui_sweept20_api_profiler_lif_to_in_label(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after profiler***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif

int
  ui_timna_app_mpls_te_qos(
    CURRENT_LINE *current_line,
    uint8     is_eep,
    uint8     is_set,
    uint32      obj_id,
    uint32      nof_consequtive_elements
)
{
  uint32
    obj_i,
    dest_sys_port;
  uint8
    is_opened;
  MPLS_TE_QOS_INFO
    qos_params;

  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_profiler");

  for(obj_i = obj_id; obj_i < obj_id + nof_consequtive_elements; obj_i++)
  {
    if(is_eep)
    {
      ui_ret = timna_app_mpls_te_tunnel_qos_get(
        obj_i,
        &dest_sys_port,
        &is_opened,
        &qos_params
      );
      if(!is_set)
      {
        if(!is_opened)
        {
          soc_sand_os_printf( "EEP %lu QoS attributes are not defined\n\r", obj_i);
        }
        else
        {
          soc_sand_os_printf( 
            "EEP %lu mapped to port %lu QoS attributes:\n\r  ",
            obj_i,
            dest_sys_port
          );
          timna_app_MPLS_TE_QOS_INFO_print(&qos_params);
        }
        continue;
      }
    }
    else
    {
      ui_ret = timna_app_mpls_te_vc_fec_qos_get(
        obj_i,
        &is_opened,
        &qos_params
        );
      if(!is_set)
      {
        if(!is_opened)
        {
          soc_sand_os_printf( "VC FEC %lu QoS attributes are not defined\n\r", obj_i);
        }
        else
        {
          soc_sand_os_printf( "VC FEC %lu QoS attributes:\n\r", obj_i);
          timna_app_MPLS_TE_QOS_INFO_print(&qos_params);
        }
        continue;
      }
    }
    if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_MPLS_TE_CIR_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_SWEEPT20_MPLS_TE_CIR_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      qos_params.cir_rate = param_val->value.ulong_value;
    }
    if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_MPLS_TE_EIR_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_SWEEPT20_MPLS_TE_EIR_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      qos_params.eir_rate = param_val->value.ulong_value;
    }
    if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_MPLS_TE_CBS_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_SWEEPT20_MPLS_TE_CBS_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      qos_params.cir_burst = param_val->value.ulong_value;
    }
    if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_MPLS_TE_EBS_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_SWEEPT20_MPLS_TE_EBS_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      qos_params.eir_burst = param_val->value.ulong_value;
    }
    if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_MPLS_TE_SP_ID,1))
    {
      UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEPT20_SWEEPT20_MPLS_TE_SP_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      qos_params.sp_level = param_val->numeric_equivalent;
    }
    if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_MPLS_TE_WEIGHT_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_SWEEPT20_MPLS_TE_WEIGHT_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      qos_params.weight = param_val->value.ulong_value;
    }

    dest_sys_port = SWEEPT20_PROFILER_SYS_PORT_1;

    if(is_eep)
    {
      ui_ret = timna_app_mpls_te_tunnel_qos_set(
        obj_i,
        dest_sys_port,
        &qos_params
        );
    }
    else
    {
      ui_ret = timna_app_mpls_te_vc_fec_qos_set(
        obj_i,
        &qos_params
        );
    }
    if (soc_sand_get_error_code_from_error_word(ui_ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** ui_timna_app_mpls_te_qos - FAIL", TRUE);
      goto exit;
    }
  }
  send_string_to_screen(" *** ui_timna_app_mpls_te_qos - Succeeded", TRUE);
  goto exit;

exit:
  return ui_ret;
}

int
  ui_timna_app_mpls_te(
    CURRENT_LINE *current_line
  )
{
  uint32
    nof_consequtive_elements=1,
    obj_id;
  UI_MACROS_INIT_FUNCTION("ui_timna_app_mpls_te");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_MPLS_TE_NOF_CONSEQUTIVE_IDS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_SWEEPT20_MPLS_TE_NOF_CONSEQUTIVE_IDS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_consequtive_elements = param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_MPLS_TE_ENABLE_ID,1))
  {
    ui_ret = timna_app_mpls_te_init(FALSE);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_MPLS_TE_EEP_QOS_GET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_SWEEPT20_MPLS_TE_EEP_QOS_GET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    obj_id = (uint32)param_val->value.ulong_value;
    ui_ret = ui_timna_app_mpls_te_qos(current_line, TRUE, FALSE, obj_id, nof_consequtive_elements);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_MPLS_TE_VC_FEC_QOS_GET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_SWEEPT20_MPLS_TE_VC_FEC_QOS_GET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    obj_id = (uint32)param_val->value.ulong_value;
    ui_ret = ui_timna_app_mpls_te_qos(current_line, FALSE, FALSE, obj_id, nof_consequtive_elements);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_MPLS_TE_EEP_QOS_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_SWEEPT20_MPLS_TE_EEP_QOS_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    obj_id = (uint32)param_val->value.ulong_value;
    ui_ret = ui_timna_app_mpls_te_qos(current_line, TRUE, TRUE, obj_id, nof_consequtive_elements);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_MPLS_TE_VC_FEC_QOS_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWEEPT20_SWEEPT20_MPLS_TE_VC_FEC_QOS_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    obj_id = (uint32)param_val->value.ulong_value;
    ui_ret = ui_timna_app_mpls_te_qos(current_line, FALSE, TRUE, obj_id, nof_consequtive_elements);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ui_timna_app_mpls_te***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#ifdef UI_SWEEPT20_API_IPV4_UC

int
  ui_sweept20_api_ipv4_uc(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_sweept20_api_ipv4_uc_test");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_ID,1))
  {
    ui_ret = ui_sweept20_api_ipv4_uc_ipv4_unicast_range_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_ID,1))
  {
    ui_ret = ui_sweept20_api_ipv4_uc_ipv4_unicast_range_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_ID,1))
  {
    ui_ret = ui_sweept20_api_ipv4_uc_ipv4_unicast_route_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_ID,1))
  {
    ui_ret = ui_sweept20_api_ipv4_uc_ipv4_unicast_route_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_REMOVE_IPV4_UNICAST_ROUTE_REMOVE_ID,1))
  {
    ui_ret = ui_sweept20_api_ipv4_uc_ipv4_unicast_route_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_ID,1))
  {
    ui_ret = ui_timna_app_ipv4_uc_ipv4_unicast_fec_range_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_IPV4_UNICAST_ROUTE_GET_BLOCK_IPV4_UNICAST_ROUTE_GET_BLOCK_ID,1))
  {
    ui_ret = ui_sweept20_api_ipv4_uc_ipv4_unicast_route_get_block(current_line);
  }


  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ipv4_uc_test***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
int
  subject_sweept20_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  TIMNA_MNGR_OPERATION_MODE
    oper_mode;
  unsigned int
    match_index;
  int
    ui_ret;
  char
    err_msg[80*8] = "" ;
  unsigned int
    unit = 0;
  char
    *proc_name ;

  proc_name = "subject_sweept20_api" ;
  ui_ret = FALSE ;
  unit = 0;

  sal_memset(&oper_mode, 0x0, sizeof(TIMNA_MNGR_OPERATION_MODE));

  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_sweept20_api()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_sweept20_api').
   */

  send_array_to_screen("\r\n",2) ;



#ifdef UI_SWEEPT20_API_INIT
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_INIT_ID,1))
  {
    ui_ret = ui_sweept20_api_init(current_line);
    goto exit;
  }
#endif
#ifndef SAND_LOW_LEVEL_SIMULATION
  ui_ret = timna_mngr_operation_mode_get(unit,&oper_mode);
  if (soc_sand_get_error_code_from_error_word(ui_ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** No Application is initialized Yet. - FAIL", TRUE);
    send_string_to_screen(" *** Use subject: timna_app init ..", TRUE);
    goto exit;
  }
#endif
#ifdef UI_SWEEPT20_API_PORTS
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PORTS_ID,1))
  {
    ui_ret = ui_sweept20_api_ports(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_MAC
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_ID,1))
  {
    ui_ret = ui_sweept20_api_mac(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_FILTER
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_FILTER_ID,1))
  {
    ui_ret = ui_sweept20_api_filter(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_EDIT
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EDIT_ID,1))
  {
    ui_ret = ui_sweept20_api_edit(current_line);
  }
#endif

#ifdef UI_SWEEPT20_API_VSI
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VSI_ID,1))
  {
    ui_ret = ui_sweept20_api_vsi(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_POLICER
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_POLICER_ID,1))
  {
    ui_ret = ui_sweept20_api_policer(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_EGRESS_ENCAP
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_EEP_ID,1))
  {
    ui_ret = ui_sweept20_api_egress_encap(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_MPLS
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MPLS_ID,1))
  {
    if (!oper_mode.mpls.enable_mpls)
    {
      soc_sand_os_printf( "MPLS Functionality has not been initialized. \n\r");
      ui_ret = SOC_SAND_ERR;
      goto exit;
    }
    ui_ret = ui_sweept20_api_mpls(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_ACL
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_ACL_ID,1))
  {
    if (!oper_mode.acl.enable_acl)
    {
      soc_sand_os_printf( "ACL Functionality has not been initialized. \n\r");
      ui_ret = SOC_SAND_ERR;
      goto exit;
    }
    ui_ret = ui_sweept20_api_acl(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_BSP
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_BSP_ID,1))
  {
    ui_ret = ui_sweept20_api_bsp(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_SYS_INFO
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_SYS_INFO_ID,1))
  {
    ui_ret = ui_sweept20_api_sys_info(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_8021AD
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_8021AD_ID,1))
  {
    if (!oper_mode.bridiging.enable_qinq)
    {
      soc_sand_os_printf( "QinQ Functionality has not been initialized. \n\r");
      ui_ret = SOC_SAND_ERR;
      goto exit;
    }

    ui_ret = ui_sweept20_api_8021ad(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_MACT
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_MAC_LKUP_ID,1))
  {
    ui_ret = ui_sweept20_api_mact_lkup(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_VID_RES
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_VID_RES_ID,1))
  {
    ui_ret = ui_sweept20_api_vid_res(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_PROFILER
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_PROFILER_ID,1))
  {
    ui_ret = ui_sweept20_api_profiler(current_line);
  }
#endif
#ifdef UI_SWEEPT20_API_IPV4_UC
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_API_IPV4_UC_ID,1))
  {
    if (!oper_mode.routing.enable_ip_routing)
    {
      soc_sand_os_printf( "Ipv4 Routing Functionality has not been initialized. \n\r");
      ui_ret = SOC_SAND_ERR;
      goto exit;
    }
    ui_ret = ui_sweept20_api_ipv4_uc(current_line);
  }
#endif
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_STATUS_GET_ID,1))
  {
    ui_ret = 0;
    timna_app_diags();
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWEEPT20_SWEEPT20_MPLS_TE_ID,1))
  {
    ui_ret = 0;
    ui_timna_app_mpls_te(current_line);
  }
  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\r\n"
      "*** sweept20_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}

#endif
