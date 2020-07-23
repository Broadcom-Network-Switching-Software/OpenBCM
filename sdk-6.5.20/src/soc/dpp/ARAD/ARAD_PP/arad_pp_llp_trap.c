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

#define _ERR_MSG_MODULE_NAME BSL_SOC_LLP



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_trap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>





#define ARAD_PP_LLP_TRAP_PROG_TRAP_NDX_MAX                       (3)
#define ARAD_PP_LLP_TRAP_RESERVED_MC_PROFILE_MAX                 (3)
#define ARAD_PP_LLP_TRAP_DA_MAC_ADDRESS_LSB_MAX                  (63)
#define ARAD_PP_LLP_TRAP_DEST_MAC_NOF_BITS_MAX                   (48)
#define ARAD_PP_LLP_TRAP_SUB_TYPE_MAX                            (SOC_SAND_U8_MAX) 
#define ARAD_PP_LLP_TRAP_SUB_TYPE_BITMAP_MAX                     (SOC_SAND_U8_MAX)

#define SOC_PPC_LLP_TRAP_PORT_INFO_TRAP_ENABLE_MASK_MAX (0x7ff)
#define ARAD_PP_LLP_TRAP_ACTION_PROFILE_TRAP_CODE_LSB_MAX (SOC_PPC_TRAP_CODE_RESERVED_MC_7)
#define ARAD_PP_LLP_TRAP_ACTION_PROFILE_TRAP_CODE_LSB_MIN (SOC_PPC_TRAP_CODE_RESERVED_MC_0)

#define ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_DA (1)
#define ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_ETHER_TYPE (2)
#define ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_SUB_TYPE (4)
#define ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_IP_PROTOCOL (8)
#define ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_L4_PORTS (0x10)

#define ARAD_PP_LLP_TRAP_PROG_L4_PORTS_FLD_SET(src, dest) \
  ((src<<16) | dest)
#define ARAD_PP_LLP_TRAP_PROG_L4_PORTS_FLD_GET_SRC(fld) \
  SOC_SAND_GET_BITS_RANGE(fld, 31, 16)
#define ARAD_PP_LLP_TRAP_PROG_L4_PORTS_FLD_GET_DEST(fld) \
  SOC_SAND_GET_BITS_RANGE(fld, 15, 0)














CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_llp_trap[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_ARP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_ARP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_ARP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_ARP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_ARP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_ARP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_ARP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LLP_TRAP_ARP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_GET_ERRS_PTR),
  

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_L3_PRTCL_PROCCESS),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_llp_trap[] =
{
  
  {
    ARAD_PP_LLP_TRAP_PROG_TRAP_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_PROG_TRAP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'prog_trap_ndx' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_RESERVED_MC_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_RESERVED_MC_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'reserved_mc_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'trap_enable_mask' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR",
    "The parameter 'da_mac_address_lsb' is out of range. \n\r "
    "The range is: 0 - 63.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_DEST_MAC_NOF_BITS_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_DEST_MAC_NOF_BITS_OUT_OF_RANGE_ERR",
    "The parameter 'dest_mac_nof_bits' is out of range. \n\r "
    "The range is: 0 - 48.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_SUB_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_SUB_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'sub_type' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_SUB_TYPE_BITMAP_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_SUB_TYPE_BITMAP_OUT_OF_RANGE_ERR",
    "The parameter 'sub_type_bitmap' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_IP_PROTOCOL_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_IP_PROTOCOL_OUT_OF_RANGE_ERR",
    "The parameter 'ip_protocol' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_SRC_PORT_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_SRC_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'src_port' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U16_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_SRC_PORT_BITMAP_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_SRC_PORT_BITMAP_OUT_OF_RANGE_ERR",
    "The parameter 'src_port_bitmap' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U16_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_DEST_PORT_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_DEST_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'dest_port' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U16_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_DEST_PORT_BITMAP_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_DEST_PORT_BITMAP_OUT_OF_RANGE_ERR",
    "The parameter 'dest_port_bitmap' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U16_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_ENABLE_BITMAP_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_ENABLE_BITMAP_OUT_OF_RANGE_ERR",
    "The parameter 'enable_bitmap' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_INVERSE_BITMAP_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_INVERSE_BITMAP_OUT_OF_RANGE_ERR",
    "The parameter 'inverse_bitmap' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  {
    ARAD_PP_ACTION_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_ACTION_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_action_strength' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_ACTION_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_ACTION_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'snoop_action_strength' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code' is out of range. \n\r "
    "The range is: 24 - 31.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_L3_PROTOCOL_EXCEEDS_CAPACITY_ERR,
    "ARAD_PP_LLP_TRAP_L3_PROTOCOL_EXCEEDS_CAPACITY_ERR",
    "Only 7 user defined l3 protocol are supported. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_TO_NDX_MISMATCH_ERR,
    "ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_TO_NDX_MISMATCH_ERR",
    "'prog_trap_ndx' larger than 1 may only be used to. \n\r "
    "test DA and ETHER_TYPE.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR",
    "The parameter 'trap_enable_mask' is out of range. \n\r "
    "The range is: 0 - 255 or 0xffffffff (see SOC_PPC_LLP_TRAP_PORT_ENABLE).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_TRAP_FAILED_TO_ALLOCATE_ETHER_TYPE_ERR,
    "ARAD_PP_LLP_TRAP_FAILED_TO_ALLOCATE_ETHER_TYPE_ERR",
    "Failed while trying to allocate ether_type.\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





uint32
  arad_pp_llp_trap_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_init_unsafe()", 0, 0);
}


uint32
  arad_pp_llp_trap_port_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PORT_INFO                      *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_PINFO_LLR_TBL_DATA
    ihp_pinfo_llr_tbl_data;
  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_in_pp_port_config_tbl_data;
  ARAD_PP_IHB_PINFO_FLP_TBL_DATA
    ihb_in_pp_port_flp_tbl_data;
  ARAD_PP_IHB_PINFO_FER_TBL_DATA
    ihb_in_pp_port_fer_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_TRAP_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_pp_ihp_pinfo_llr_tbl_get_unsafe(
          unit,
          core_id,
          local_port_ndx,
          &ihp_pinfo_llr_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ihp_pinfo_llr_tbl_data.reserved_mc_trap_profile =
    port_info->reserved_mc_profile;
  
  ihp_pinfo_llr_tbl_data.enable_arp_trap =
    SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_ARP);

  ihp_pinfo_llr_tbl_data.enable_dhcp_trap =
    SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_DHCP);

  ihp_pinfo_llr_tbl_data.enable_igmp_trap =
    SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_IGMP);

  ihp_pinfo_llr_tbl_data.enable_mld_trap =
    SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_MLD);

  
  ihp_pinfo_llr_tbl_data.general_trap_enable = (0xf &
    ((SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0)) |
     (SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_1) << 1) |
     (SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_2) << 2) |
     (SOC_SAND_BOOL2NUM(port_info->trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_3) << 3)));

  res = arad_pp_ihp_pinfo_llr_tbl_set_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &ihp_pinfo_llr_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &vtt_in_pp_port_config_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  vtt_in_pp_port_config_tbl_data.all_vlan_member = SOC_SAND_NUM2BOOL_INVERSE(port_info->trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_ING_VLAN_MEMBERSHIP);

  res = arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &vtt_in_pp_port_config_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


  res = arad_pp_ihb_pinfo_flp_tbl_get_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &ihb_in_pp_port_flp_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  ihb_in_pp_port_flp_tbl_data.enable_unicast_same_interface_filter = SOC_SAND_NUM2BOOL(port_info->trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_SAME_INTERFACE);

  res = arad_pp_ihb_pinfo_flp_tbl_set_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &ihb_in_pp_port_flp_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


  res = arad_pp_ihb_pinfo_fer_tbl_get_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &ihb_in_pp_port_fer_tbl_data
    );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);

  ihb_in_pp_port_fer_tbl_data.icmp_redirect_enable = SOC_SAND_NUM2BOOL(port_info->trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_ICMP_REDIRECT);

  res = arad_pp_ihb_pinfo_fer_tbl_set_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &ihb_in_pp_port_fer_tbl_data
    );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 110, exit);


  


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_port_info_set_unsafe()", local_port_ndx, 0);
}

uint32
  arad_pp_llp_trap_port_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PORT_INFO                      *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_TRAP_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_TRAP_PORT_INFO, port_info, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_port_info_set_verify()", local_port_ndx, 0);
}

uint32
  arad_pp_llp_trap_port_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_TRAP_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_port_info_get_verify()", local_port_ndx, 0);
}


uint32
  arad_pp_llp_trap_port_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PORT_INFO                      *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_PINFO_LLR_TBL_DATA
    ihp_pinfo_llr_tbl_data;
  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_in_pp_port_config_tbl_data;
  ARAD_PP_IHB_PINFO_FLP_TBL_DATA
    ihb_in_pp_port_flp_tbl_data;
  ARAD_PP_IHB_PINFO_FER_TBL_DATA
    ihb_in_pp_port_fer_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_TRAP_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PPC_LLP_TRAP_PORT_INFO_clear(port_info);

  res = arad_pp_ihp_pinfo_llr_tbl_get_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &ihp_pinfo_llr_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  port_info->reserved_mc_profile =
    ihp_pinfo_llr_tbl_data.reserved_mc_trap_profile;

  port_info->trap_enable_mask = 0;

  if (ihp_pinfo_llr_tbl_data.enable_arp_trap)
  {
    port_info->trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_ARP;
  }
  if (ihp_pinfo_llr_tbl_data.enable_dhcp_trap)
  {
    port_info->trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_DHCP;
  }
  if (ihp_pinfo_llr_tbl_data.enable_igmp_trap)
  {
    port_info->trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_IGMP;
  }
  if (ihp_pinfo_llr_tbl_data.enable_mld_trap)
  {
    port_info->trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_MLD;
  }
  
  
  if (SOC_SAND_GET_BIT(ihp_pinfo_llr_tbl_data.general_trap_enable,0))
  {
    port_info->trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0;
  }
  if (SOC_SAND_GET_BIT(ihp_pinfo_llr_tbl_data.general_trap_enable,1))
  {
    port_info->trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_1;
  }
  if (SOC_SAND_GET_BIT(ihp_pinfo_llr_tbl_data.general_trap_enable,2))
  {
    port_info->trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_2;
  }
  if (SOC_SAND_GET_BIT(ihp_pinfo_llr_tbl_data.general_trap_enable,3))
  {
    port_info->trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_3;
  }
  
  res = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &vtt_in_pp_port_config_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


  if(!vtt_in_pp_port_config_tbl_data.all_vlan_member) {
      port_info->trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_ING_VLAN_MEMBERSHIP;
  }

  res = arad_pp_ihb_pinfo_flp_tbl_get_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &ihb_in_pp_port_flp_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (ihb_in_pp_port_flp_tbl_data.enable_unicast_same_interface_filter) {
    port_info->trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_SAME_INTERFACE;
  }
  

  res = arad_pp_ihb_pinfo_fer_tbl_get_unsafe(
    unit,
    core_id,
    local_port_ndx,
    &ihb_in_pp_port_fer_tbl_data
    );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);

  if(ihb_in_pp_port_fer_tbl_data.icmp_redirect_enable) {
      port_info->trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_ICMP_REDIRECT;
  }

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_port_info_get_unsafe()", local_port_ndx, 0);
}


uint32
  arad_pp_llp_trap_arp_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_ARP_INFO                     *arp_info
  )
{
  uint32
    res;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_TRAP_ARP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(arp_info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_MY_ARP_IP_1r, SOC_CORE_ALL, 0, MY_ARP_IP_1f,  arp_info->my_ips[0]));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_MY_ARP_IP_2r, SOC_CORE_ALL, 0, MY_ARP_IP_2f,  arp_info->my_ips[1]));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_MAC_LAYER_TRAP_ARPr, SOC_CORE_ALL, 0, ARP_TRAP_IGNORE_DAf,  SOC_SAND_BOOL2NUM(arp_info->ignore_da)));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_arp_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_llp_trap_arp_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_ARP_INFO                     *arp_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_TRAP_ARP_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_TRAP_ARP_INFO, arp_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_arp_info_set_verify()", 0, 0);
}

uint32
  arad_pp_llp_trap_arp_info_get_verify(
    SOC_SAND_IN  int                                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_TRAP_ARP_INFO_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_arp_info_get_verify()", 0, 0);
}


uint32
  arad_pp_llp_trap_arp_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_ARP_INFO                     *arp_info
  )
{
  uint32
    res,
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LLP_TRAP_ARP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(arp_info);

  SOC_PPC_LLP_TRAP_ARP_INFO_clear(arp_info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_MY_ARP_IP_1r, SOC_CORE_ALL, 0, MY_ARP_IP_1f, &fld_val));
  arp_info->my_ips[0] = fld_val;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_MY_ARP_IP_2r, SOC_CORE_ALL, 0, MY_ARP_IP_2f, &fld_val));
  arp_info->my_ips[1] = fld_val;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_MAC_LAYER_TRAP_ARPr, SOC_CORE_ALL, 0, ARP_TRAP_IGNORE_DAf, &fld_val));
  arp_info->ignore_da = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_arp_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_llp_trap_reserved_mc_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY                *reserved_mc_key,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                          *trap_action
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  ARAD_PP_IHP_RESERVED_MC_TBL_DATA
    ihp_reserved_mc_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(reserved_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(trap_action);

  entry_offset = ARAD_TBL_IHP_RESERVED_MC_KEY_ENTRY_OFFSET(
    reserved_mc_key->reserved_mc_profile,
    reserved_mc_key->da_mac_address_lsb);

  ihp_reserved_mc_tbl_data.cpu_trap_code = (trap_action->trap_code - SOC_PPC_TRAP_CODE_RESERVED_MC_0);
  ihp_reserved_mc_tbl_data.fwd = trap_action->frwrd_action_strength;
  ihp_reserved_mc_tbl_data.snp = trap_action->snoop_action_strength;

  res = arad_pp_ihp_reserved_mc_tbl_set_unsafe(
    unit,
    entry_offset,
    &ihp_reserved_mc_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_reserved_mc_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_llp_trap_reserved_mc_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY                *reserved_mc_key,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                          *trap_action
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_TRAP_RESERVED_MC_KEY, reserved_mc_key, 10, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_action->frwrd_action_strength, ARAD_PP_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_MAX, ARAD_PP_ACTION_FRWRD_ACTION_STRENGTH_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_action->snoop_action_strength, ARAD_PP_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_MAX, ARAD_PP_ACTION_SNOOP_ACTION_STRENGTH_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(trap_action->trap_code, ARAD_PP_LLP_TRAP_ACTION_PROFILE_TRAP_CODE_LSB_MIN, ARAD_PP_LLP_TRAP_ACTION_PROFILE_TRAP_CODE_LSB_MAX, ARAD_PP_LLP_TRAP_ACTION_TRAP_CODE_LSB_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_reserved_mc_info_set_verify()", 0, 0);
}

uint32
  arad_pp_llp_trap_reserved_mc_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY                *reserved_mc_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_TRAP_RESERVED_MC_KEY, reserved_mc_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_reserved_mc_info_get_verify()", 0, 0);
}


uint32
  arad_pp_llp_trap_reserved_mc_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY                *reserved_mc_key,
    SOC_SAND_OUT SOC_PPC_ACTION_PROFILE                          *trap_action
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  ARAD_PP_IHP_RESERVED_MC_TBL_DATA
    ihp_reserved_mc_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_RESERVED_MC_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(reserved_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(trap_action);

  SOC_PPC_ACTION_PROFILE_clear(trap_action);

  entry_offset = ARAD_TBL_IHP_RESERVED_MC_KEY_ENTRY_OFFSET(
    reserved_mc_key->reserved_mc_profile,
    reserved_mc_key->da_mac_address_lsb);

  res = arad_pp_ihp_reserved_mc_tbl_get_unsafe(
    unit,
    entry_offset,
    &ihp_reserved_mc_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  trap_action->trap_code = ihp_reserved_mc_tbl_data.cpu_trap_code + SOC_PPC_TRAP_CODE_RESERVED_MC_0;
  trap_action->frwrd_action_strength = ihp_reserved_mc_tbl_data.fwd;
  trap_action->snoop_action_strength = ihp_reserved_mc_tbl_data.snp;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_reserved_mc_info_get_unsafe()", 0, 0);
}

STATIC uint32
  arad_pp_llp_trap_prog_trap_info_l3_prtcl_proccess(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      prog_trap_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER            *prog_trap_qual,
    SOC_SAND_OUT uint8                                       *prctl_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  uint8
    ip_protocol_ndx,
    ip_protocol_ndx_in_db;
  uint8
    is_found,
    is_current_pre_defined;
  SOC_SAND_SUCCESS_FAILURE
    success;
  SOC_PPC_L3_NEXT_PRTCL_TYPE
    current_l3_protocol;
  soc_reg_above_64_val_t
	  fld_above64_val,
	  reg_above64_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_L3_PRTCL_PROCCESS);

  SOC_SAND_CHECK_NULL_INPUT(prog_trap_qual);

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
  SOC_REG_ABOVE_64_CLEAR(fld_above64_val);
  
  

  res = arad_pp_l3_prtcl_to_ndx(
          prog_trap_qual->l3_info.ip_protocol,
          &ip_protocol_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_pp_l3_next_protocol_find(
          unit,
          prog_trap_qual->l3_info.ip_protocol,
          &ip_protocol_ndx_in_db,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  res = READ_IHP_GENERAL_TRAPr(unit, REG_PORT_ANY, prog_trap_ndx , reg_above64_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  soc_reg_above_64_field_get(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_IP_PROTOCOLf, fld_above64_val);

  res = arad_pp_ndx_to_l3_prtcl(
          (uint8)(fld_above64_val[0]),
          &current_l3_protocol,
          &is_current_pre_defined
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!is_current_pre_defined)
  {
    
    if (ip_protocol_ndx_in_db != (uint8)(fld_above64_val[0]))
    {
      res = arad_pp_l3_next_protocol_remove(
              unit,
              (uint8)(fld_above64_val[0])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      is_found = FALSE;
    }
  }

  if (ip_protocol_ndx == ARAD_PP_L3_NEXT_PRTCL_NDX_USER_DEFINED)
  {
    if (is_found == FALSE)
    {
      
      res = arad_pp_l3_next_protocol_add(
              unit,
              prog_trap_qual->l3_info.ip_protocol,
              &ip_protocol_ndx,
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      if (success != SOC_SAND_SUCCESS)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_LLP_TRAP_L3_PROTOCOL_EXCEEDS_CAPACITY_ERR, 60, exit);
      }
    }
    else
    {
      ip_protocol_ndx = ip_protocol_ndx_in_db;
    }
  }

  *prctl_ndx = ip_protocol_ndx;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_prog_trap_info_l3_prtcl_proccess()", 0, 0);
}


uint32
  arad_pp_llp_trap_prog_trap_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      prog_trap_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER            *prog_trap_qual
  )
{
  uint32
    res = SOC_SAND_OK,
    mac_add_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    ether_type_internal,
    ether_type_internal_old,
    ether_type_old;
  uint8
    ip_protocol_ndx=0;
  uint8
    found;
  SOC_SAND_SUCCESS_FAILURE
    success;
  soc_reg_above_64_val_t
	  fld_above64_val,
	  reg_above64_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prog_trap_qual);

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
  SOC_REG_ABOVE_64_CLEAR(fld_above64_val);

  
  res = READ_IHP_GENERAL_TRAPr(unit, REG_PORT_ANY, prog_trap_ndx, reg_above64_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  soc_reg_above_64_field_get(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_ETHERNET_TYPEf, fld_above64_val);
  ether_type_internal_old = fld_above64_val[0];

  res = arad_pp_l2_next_prtcl_type_from_internal_find(
          unit,
          ether_type_internal_old,
          &ether_type_old,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);

  if (found)
  {
    res = arad_pp_l2_next_prtcl_type_deallocate(
            unit,
            ether_type_old
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);
  }

  res = arad_pp_l2_next_prtcl_type_allocate(
          unit,
          prog_trap_qual->l2_info.ether_type,
          &ether_type_internal,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);

  if (success == SOC_SAND_SUCCESS)
  {
	  fld_above64_val[0] = ether_type_internal;
	  soc_reg_above_64_field_set(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_ETHERNET_TYPEf, fld_above64_val);
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LLP_TRAP_FAILED_TO_ALLOCATE_ETHER_TYPE_ERR, 12, exit);
  }
 
  
  res = arad_pp_llp_trap_prog_trap_info_l3_prtcl_proccess(
    unit,
    prog_trap_ndx,
    prog_trap_qual,
    &ip_protocol_ndx
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  fld_above64_val[0] = ip_protocol_ndx;
  soc_reg_above_64_field_set(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_IP_PROTOCOLf, fld_above64_val);

  
  
     
  res = soc_sand_pp_mac_address_struct_to_long(
    &(prog_trap_qual->l2_info.dest_mac),
    mac_add_long
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  fld_above64_val[0] = mac_add_long[0];
  fld_above64_val[1] = mac_add_long[1];
  soc_reg_above_64_field_set(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_DAf, fld_above64_val);
  fld_above64_val[1] = 0;

  fld_above64_val[0] = SOC_SAND_PP_MAC_ADDRESS_NOF_BITS - prog_trap_qual->l2_info.dest_mac_nof_bits;
  soc_reg_above_64_field_set(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_DA_BITSf, fld_above64_val);

  
  fld_above64_val[0] = prog_trap_qual->l2_info.sub_type;
  soc_reg_above_64_field_set(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_SUB_TYPEf, fld_above64_val);

  fld_above64_val[0] = prog_trap_qual->l2_info.sub_type_bitmap;
  soc_reg_above_64_field_set(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_SUB_TYPE_MASKf, fld_above64_val);

  
  fld_above64_val[0] = ARAD_PP_LLP_TRAP_PROG_L4_PORTS_FLD_SET( prog_trap_qual->l4_info.src_port, prog_trap_qual->l4_info.dest_port);
  soc_reg_above_64_field_set(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_L_4_PORTf, fld_above64_val);

  fld_above64_val[0] = ARAD_PP_LLP_TRAP_PROG_L4_PORTS_FLD_SET( prog_trap_qual->l4_info.src_port_bitmap, prog_trap_qual->l4_info.dest_port_bitmap);
  soc_reg_above_64_field_set(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_L_4_PORT_MASKf, fld_above64_val);

  
  fld_above64_val[0] = 0;
  if (prog_trap_qual->enable_bitmap & SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_DA)
  {
    fld_above64_val[0] |= ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_DA;
  }
  if (prog_trap_qual->enable_bitmap & SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE)
  {
    fld_above64_val[0] |= ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_ETHER_TYPE;
  }
  if (prog_trap_qual->enable_bitmap & SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE)
  {
    fld_above64_val[0] |= ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_SUB_TYPE;
  }
  if (prog_trap_qual->enable_bitmap & SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_IP_PRTCL)
  {
    fld_above64_val[0] |= ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_IP_PROTOCOL;
  }
  if (prog_trap_qual->enable_bitmap & SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_L4_PORTS)
  {
    fld_above64_val[0] |= ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_L4_PORTS;
  }
  soc_reg_above_64_field_set(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_ENABLE_BMPf, fld_above64_val);

  
  fld_above64_val[0] = 0;
  if (prog_trap_qual->inverse_bitmap & SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_DA)
  {
    fld_above64_val[0] |= ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_DA;
  }
  if (prog_trap_qual->inverse_bitmap & SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE)
  {
    fld_above64_val[0] |= ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_ETHER_TYPE;
  }
  if (prog_trap_qual->inverse_bitmap & SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE)
  {
    fld_above64_val[0] |= ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_SUB_TYPE;
  }
  if (prog_trap_qual->inverse_bitmap & SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_IP_PRTCL)
  {
    fld_above64_val[0] |= ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_IP_PROTOCOL;
  }
  if (prog_trap_qual->inverse_bitmap & SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_L4_PORTS)
  {
    fld_above64_val[0] |= ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_L4_PORTS;
  }
  soc_reg_above_64_field_set(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_INVERSE_BMPf, fld_above64_val);

  
  res = WRITE_IHP_GENERAL_TRAPr(unit, REG_PORT_ANY, prog_trap_ndx , reg_above64_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_prog_trap_info_set_unsafe()", prog_trap_ndx, 0);
}

uint32
  arad_pp_llp_trap_prog_trap_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      prog_trap_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER            *prog_trap_qual
  )
{
  uint32
    res = SOC_SAND_OK,
    enable_bitmap_test,
    inverse_bitmap_test;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(prog_trap_ndx, ARAD_PP_LLP_TRAP_PROG_TRAP_NDX_MAX, ARAD_PP_LLP_TRAP_PROG_TRAP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER, prog_trap_qual, 20, exit);

  if (prog_trap_ndx > 1)
  {
    
    enable_bitmap_test =
      (prog_trap_qual->enable_bitmap & (~(SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_DA | SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE)));
    inverse_bitmap_test =
      (prog_trap_qual->inverse_bitmap & (~(SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_DA | SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE)));

    if (enable_bitmap_test || inverse_bitmap_test)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_TO_NDX_MISMATCH_ERR, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_prog_trap_info_set_verify()", prog_trap_ndx, 0);
}

uint32
  arad_pp_llp_trap_prog_trap_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      prog_trap_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(prog_trap_ndx, ARAD_PP_LLP_TRAP_PROG_TRAP_NDX_MAX, ARAD_PP_LLP_TRAP_PROG_TRAP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_prog_trap_info_get_verify()", prog_trap_ndx, 0);
}


uint32
  arad_pp_llp_trap_prog_trap_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      prog_trap_ndx,
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER            *prog_trap_qual
  )
{
  uint32
    res = SOC_SAND_OK,
    mac_add_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    ether_type_internal,
    ether_type; 
  uint8
    is_current_pre_defined,
    found;
  SOC_PPC_L3_NEXT_PRTCL_TYPE
    current_l3_protocol;
  ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA
    ihp_parser_ip_protocols_tbl_data;
  soc_reg_above_64_val_t
	  fld_above64_val,
	  reg_above64_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_TRAP_PROG_TRAP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prog_trap_qual);

  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_clear(prog_trap_qual);

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
  SOC_REG_ABOVE_64_CLEAR(fld_above64_val);

  
  res = READ_IHP_GENERAL_TRAPr(unit, REG_PORT_ANY, prog_trap_ndx, reg_above64_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  soc_reg_above_64_field_get(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_IP_PROTOCOLf, fld_above64_val);

  res = arad_pp_ndx_to_l3_prtcl(
    (uint8)(fld_above64_val[0]),
    &current_l3_protocol,
    &is_current_pre_defined
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (is_current_pre_defined)
  {
    prog_trap_qual->l3_info.ip_protocol = current_l3_protocol;
  }
  else
  {
    res = arad_pp_ihp_parser_ip_protocols_tbl_get_unsafe(
      unit,
      ARAD_PP_IP_FIELD_VAL_TO_PROTOCOL_UD_NDX(fld_above64_val[0]),
      &ihp_parser_ip_protocols_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    prog_trap_qual->l3_info.ip_protocol =
      (uint8)ihp_parser_ip_protocols_tbl_data.ip_protocol;
  }

  
  fld_above64_val[0] = 0;
  fld_above64_val[1] = 0;
  soc_reg_above_64_field_get(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_DAf, fld_above64_val);
  mac_add_long[0] = fld_above64_val[0];
  mac_add_long[1] = fld_above64_val[1];
  mac_add_long[0] &= 0xff;

  
     
  res = soc_sand_pp_mac_address_long_to_struct(
    mac_add_long,
    &(prog_trap_qual->l2_info.dest_mac)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  fld_above64_val[0] = 0;
  soc_reg_above_64_field_get(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_DA_BITSf, fld_above64_val);

  prog_trap_qual->l2_info.dest_mac_nof_bits = (uint8)(SOC_SAND_PP_MAC_ADDRESS_NOF_BITS - fld_above64_val[0]);

  fld_above64_val[0] = 0;
  soc_reg_above_64_field_get(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_ETHERNET_TYPEf, fld_above64_val);
  ether_type_internal = fld_above64_val[0];

  res = arad_pp_l2_next_prtcl_type_from_internal_find(
          unit,
          ether_type_internal,
          &ether_type,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);

  if (found)
  {
    prog_trap_qual->l2_info.ether_type = (uint16)ether_type;
  }

  
  fld_above64_val[0] = 0;
  soc_reg_above_64_field_get(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_SUB_TYPEf, fld_above64_val);
  prog_trap_qual->l2_info.sub_type = (uint8)(fld_above64_val[0]);

  fld_above64_val[0] = 0;
  soc_reg_above_64_field_get(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_SUB_TYPE_MASKf, fld_above64_val);
  prog_trap_qual->l2_info.sub_type_bitmap = (uint8)(fld_above64_val[0]);

  
  fld_above64_val[0] = 0;
  soc_reg_above_64_field_get(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_L_4_PORTf, fld_above64_val);

  prog_trap_qual->l4_info.src_port = (uint8)(ARAD_PP_LLP_TRAP_PROG_L4_PORTS_FLD_GET_SRC(fld_above64_val[0]));
  prog_trap_qual->l4_info.dest_port = (uint8)(ARAD_PP_LLP_TRAP_PROG_L4_PORTS_FLD_GET_DEST(fld_above64_val[0]));

  
  fld_above64_val[0] = 0;
  soc_reg_above_64_field_get(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_L_4_PORT_MASKf, fld_above64_val);

  prog_trap_qual->l4_info.src_port_bitmap = (uint16)(ARAD_PP_LLP_TRAP_PROG_L4_PORTS_FLD_GET_SRC(fld_above64_val[0]));
  prog_trap_qual->l4_info.dest_port_bitmap = (uint16)(ARAD_PP_LLP_TRAP_PROG_L4_PORTS_FLD_GET_DEST(fld_above64_val[0]));

  
  fld_above64_val[0] = 0;
  soc_reg_above_64_field_get(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_ENABLE_BMPf, fld_above64_val);

  prog_trap_qual->enable_bitmap = 0;
  if (fld_above64_val[0] & ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_DA)
  {
    prog_trap_qual->enable_bitmap |= SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_DA;
  }
  if (fld_above64_val[0] & ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_ETHER_TYPE)
  {
    prog_trap_qual->enable_bitmap |= SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE;
  }
  if (fld_above64_val[0] & ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_SUB_TYPE)
  {
    prog_trap_qual->enable_bitmap |= SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE;
  }
  if (fld_above64_val[0] & ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_IP_PROTOCOL)
  {
    prog_trap_qual->enable_bitmap |= SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_IP_PRTCL;
  }
  if (fld_above64_val[0] & ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_L4_PORTS)
  {
    prog_trap_qual->enable_bitmap |= SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_L4_PORTS;
  }

  
  fld_above64_val[0] = 0;
  soc_reg_above_64_field_get(unit, IHP_GENERAL_TRAPr, reg_above64_val, GENERAL_TRAP_INVERSE_BMPf, fld_above64_val);

  prog_trap_qual->inverse_bitmap = 0;
  if (fld_above64_val[0] & ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_DA)
  {
    prog_trap_qual->inverse_bitmap |= SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_DA;
  }
  if (fld_above64_val[0] & ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_ETHER_TYPE)
  {
    prog_trap_qual->inverse_bitmap |= SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE;
  }
  if (fld_above64_val[0] & ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_SUB_TYPE)
  {
    prog_trap_qual->inverse_bitmap |= SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE;
  }
  if (fld_above64_val[0] & ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_IP_PROTOCOL)
  {
    prog_trap_qual->inverse_bitmap |= SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_IP_PRTCL;
  }
  if (fld_above64_val[0] & ARAD_PP_LLP_TRAP_PROG_TRAP_BITMAP_L4_PORTS)
  {
    prog_trap_qual->inverse_bitmap |= SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_L4_PORTS;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_trap_prog_trap_info_get_unsafe()", prog_trap_ndx, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_llp_trap_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_llp_trap;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_llp_trap_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_llp_trap;
}

uint32
  SOC_PPC_LLP_TRAP_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->reserved_mc_profile, ARAD_PP_LLP_TRAP_RESERVED_MC_PROFILE_MAX, ARAD_PP_LLP_TRAP_RESERVED_MC_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  
  if (info->trap_enable_mask != SOC_PPC_LLP_TRAP_PORT_ENABLE_ALL)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->trap_enable_mask, SOC_PPC_LLP_TRAP_PORT_INFO_TRAP_ENABLE_MASK_MAX, ARAD_PP_LLP_TRAP_ENABLE_MASK_OUT_OF_RANGE_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_TRAP_PORT_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_TRAP_ARP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_ARP_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  for (ind = 0; ind < SOC_PPC_TRAP_NOF_MY_IPS; ++ind)
  {
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_TRAP_ARP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->reserved_mc_profile, ARAD_PP_LLP_TRAP_RESERVED_MC_PROFILE_MAX, ARAD_PP_LLP_TRAP_RESERVED_MC_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->da_mac_address_lsb, ARAD_PP_LLP_TRAP_DA_MAC_ADDRESS_LSB_MAX, ARAD_PP_LLP_TRAP_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_verify()",0,0);
}

uint32
  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_mac_nof_bits, ARAD_PP_LLP_TRAP_DEST_MAC_NOF_BITS_MAX, ARAD_PP_LLP_TRAP_DEST_MAC_NOF_BITS_OUT_OF_RANGE_ERR, 11, exit);
  
  
  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_verify(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO, &(info->l2_info), 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO, &(info->l3_info), 11, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO, &(info->l4_info), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_verify()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


