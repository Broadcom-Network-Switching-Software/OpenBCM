
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_MANAGEMENT



#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_framework.h>

#include <soc/dpp/SAND/Management/sand_device_management.h>






#define ARAD_PP_MGMT_ELK_MODE_MAX                                (ARAD_PP_NOF_MGMT_ELK_MODES-1)
#define ARAD_PP_MGMT_LKP_TYPE_MAX                                (ARAD_PP_NOF_MGMT_LKP_TYPES-1)
#define ARAD_PP_MGMT_USE_ELK_MAX                                 (ARAD_PP_NOF_MGMT_LKP_TYPES-1)
















CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_mgmt[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_GET_ERRS_PTR),
  

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_DEVICE_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_DEVICE_CLOSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_OPERATION_MODE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_OPERATION_MODE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_OPERATION_MODE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_OPERATION_MODE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_OPERATION_MODE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_ELK_MODE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_USE_ELK_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MGMT_PROC_ERR_MECH_INIT),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_mgmt[] =
{
  
  {
    ARAD_PP_MGMT_ELK_MODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_MGMT_ELK_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'elk_mode' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MGMT_ELK_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MGMT_INGRESS_PKT_RATE_OUT_OF_RANGE_ERR,
    "ARAD_PP_MGMT_INGRESS_PKT_RATE_OUT_OF_RANGE_ERR",
    "The parameter 'ingress_pkt_rate' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MGMT_ELK_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MGMT_LKP_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_MGMT_LKP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'lkp_type' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MGMT_LKP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MGMT_USE_ELK_OUT_OF_RANGE_ERR,
    "ARAD_PP_MGMT_USE_ELK_OUT_OF_RANGE_ERR",
    "The parameter 'use_elk' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MGMT_LKP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};






STATIC uint32
  arad_pp_mgmt_proc_err_mech_init(void)
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_PP_MGMT_PROC_ERR_MECH_INIT);

  
  res = arad_pp_errors_desc_add();
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  
  res = arad_pp_procedure_desc_add() ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_proc_err_mech_init()",0,0);
}


STATIC uint32
  arad_pp_mgmt_device_init(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE *op_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_DEVICE_INIT);

  res = arad_pp_sw_db_device_init(
          unit,
          op_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_device_init()",0,0);
}

uint32
  arad_pp_mgmt_device_close_unsafe(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_DEVICE_CLOSE);

  res = arad_pp_sw_db_device_close(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_device_close_unsafe()",0,0);
}


uint32
  arad_pp_mgmt_operation_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE *op_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_OPERATION_MODE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  
  res = arad_pp_mgmt_proc_err_mech_init();
  SOC_SAND_CHECK_FUNC_RESULT(res,10, exit);

  
  res = arad_pp_sw_db_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 
  res = arad_pp_mgmt_device_init(
          unit,
          op_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_operation_mode_set_unsafe()",0,0);
}


uint32
  arad_pp_mgmt_operation_mode_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE      *op_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_OPERATION_MODE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_operation_mode_verify()",0,0);
}


uint32
  arad_pp_mgmt_operation_mode_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT ARAD_PP_MGMT_OPERATION_MODE     *op_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_OPERATION_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.get(unit,op_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_mgmt_operation_mode_get_unsafe()",0,0);
}


uint32
  arad_pp_mgmt_elk_mode_set_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_ELK_MODE elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_ELK_MODE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ingress_pkt_rate);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_elk_mode_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mgmt_elk_mode_set_verify(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_ELK_MODE elk_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_ELK_MODE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(elk_mode, ARAD_PP_MGMT_ELK_MODE_MAX, ARAD_PP_MGMT_ELK_MODE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_elk_mode_set_verify()", 0, 0);
}

uint32
  arad_pp_mgmt_elk_mode_get_verify(
    SOC_SAND_IN  int           unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_ELK_MODE_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_elk_mode_get_verify()", 0, 0);
}


uint32
  arad_pp_mgmt_elk_mode_get_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT ARAD_PP_MGMT_ELK_MODE *elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_ELK_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(elk_mode);
  SOC_SAND_CHECK_NULL_INPUT(ingress_pkt_rate);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_elk_mode_get_unsafe()", 0, 0);
}


uint32
  arad_pp_mgmt_use_elk_set_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_IN  uint8           use_elk
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_USE_ELK_SET_UNSAFE);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_use_elk_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mgmt_use_elk_set_verify(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_IN  uint8           use_elk
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_USE_ELK_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lkp_type, ARAD_PP_MGMT_LKP_TYPE_MAX, ARAD_PP_MGMT_LKP_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(use_elk, ARAD_PP_MGMT_USE_ELK_MAX, ARAD_PP_MGMT_USE_ELK_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_use_elk_set_verify()", 0, 0);
}

uint32
  arad_pp_mgmt_use_elk_get_verify(
    SOC_SAND_IN  int           unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_USE_ELK_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_use_elk_get_verify()", 0, 0);
}


uint32
  arad_pp_mgmt_use_elk_get_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT ARAD_PP_MGMT_LKP_TYPE *lkp_type,
    SOC_SAND_OUT uint8           *use_elk
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MGMT_USE_ELK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lkp_type);
  SOC_SAND_CHECK_NULL_INPUT(use_elk);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mgmt_use_elk_get_unsafe()", 0, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_mgmt_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_mgmt;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_mgmt_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_mgmt;
}


void
  ARAD_PP_MGMT_OPERATION_MODE_clear(
    SOC_SAND_OUT ARAD_PP_MGMT_OPERATION_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_PP_MGMT_OPERATION_MODE));
  info->authentication_enable = FALSE;
  info->system_vsi_enable = FALSE;
  info->hairpin_enable = FALSE;
  info->split_horizon_filter_enable = FALSE;
  ARAD_PP_MGMT_P2P_INFO_clear(&info->p2p_info);
  ARAD_PP_MGMT_IPV4_INFO_clear(&info->ipv4_info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MGMT_P2P_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MGMT_P2P_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_PP_MGMT_P2P_INFO));
  info->mim_vsi = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MGMT_IPV4_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MGMT_IPV4_INFO *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_PP_MGMT_IPV4_INFO));
  info->pvlan_enable = 0;
  info->nof_vrfs = 0;
  for (ind = 0; ind < SOC_DPP_DEFS_MAX(NOF_VRFS); ++ind)
  {
    info->max_routes_in_vrf[ind] = 0;
  }

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

void
  ARAD_PP_MGMT_OPERATION_MODE_print(
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "authentication_enable: %u\n\r"),info->authentication_enable));
  LOG_CLI((BSL_META_U(unit,
                      "system_vsi_enable: %u\n\r"),info->system_vsi_enable));
  LOG_CLI((BSL_META_U(unit,
                      "hairpin_enable: %u\n\r"),info->hairpin_enable));
  LOG_CLI((BSL_META_U(unit,
                      "split_horizon_filter_enable: %u\n\r"),info->split_horizon_filter_enable));
  LOG_CLI((BSL_META_U(unit,
                      "p2p_info :\n\r")));
  ARAD_PP_MGMT_P2P_INFO_print(&info->p2p_info);
  ARAD_PP_MGMT_IPV4_INFO_print(&info->ipv4_info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MGMT_P2P_INFO_print(
    SOC_SAND_IN  ARAD_PP_MGMT_P2P_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mim_vsi: %u\n\r"),info->mim_vsi));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MGMT_IPV4_INFO_print(
    SOC_SAND_IN  ARAD_PP_MGMT_IPV4_INFO *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "pvlan_enable: %u\n\r"),info->pvlan_enable));
  LOG_CLI((BSL_META_U(unit,
                      "nof_vrfs: %u\n\r"),info->nof_vrfs));
  for (ind = 0; ind < info->nof_vrfs; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "info->max_routes_in_vrf[ind]: %u\n\r"),info->max_routes_in_vrf[ind]));
  }
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  ARAD_PP_MGMT_ELK_MODE_to_string(
    SOC_SAND_IN  ARAD_PP_MGMT_ELK_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_PP_MGMT_ELK_MODE_NONE:
    str = "none";
  break;
  case ARAD_PP_MGMT_ELK_MODE_NORMAL:
    str = "normal";
  break;
  case ARAD_PP_MGMT_ELK_MODE_B0_SHORT:
    str = "b0_short";
  break;
  case ARAD_PP_MGMT_ELK_MODE_B0_LONG:
    str = "b0_long";
  break;
  case ARAD_PP_MGMT_ELK_MODE_B0_BOTH:
    str = "b0_both";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  ARAD_PP_MGMT_LKP_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_MGMT_LKP_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_PP_MGMT_LKP_TYPE_P2P:
    str = "p2p";
  break;
  case ARAD_PP_MGMT_LKP_TYPE_ETH:
    str = "eth";
  break;
  case ARAD_PP_MGMT_LKP_TYPE_TRILL_UC:
    str = "trill_uc";
  break;
  case ARAD_PP_MGMT_LKP_TYPE_TRILL_MC:
    str = "trill_mc";
  break;
  case ARAD_PP_MGMT_LKP_TYPE_IPV3_UC:
    str = "ipv3_uc";
  break;
  case ARAD_PP_MGMT_LKP_TYPE_IPV4_MC:
    str = "ipv4_mc";
  break;
  case ARAD_PP_MGMT_LKP_TYPE_IPV6_UC:
    str = "ipv6_uc";
  break;
  case ARAD_PP_MGMT_LKP_TYPE_IPV6_MC:
    str = "ipv6_mc";
  break;
  case ARAD_PP_MGMT_LKP_TYPE_LSR:
    str = "lsr";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

#endif 


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

