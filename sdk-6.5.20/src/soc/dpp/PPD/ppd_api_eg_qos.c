
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

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_qos.h>






















uint32
  soc_ppd_eg_qos_port_info_set(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PORT_INFO                        *port_qos_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_QOS_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_qos_info);

  res = arad_pp_eg_qos_port_info_set_verify(
          unit,
          local_port_ndx,
          port_qos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_port_info_set_unsafe(
          unit,
		  core_id,
          local_port_ndx,
          port_qos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_qos_port_info_set()", local_port_ndx, 0);
}


uint32
  soc_ppd_eg_qos_port_info_get(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_EG_QOS_PORT_INFO                        *port_qos_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_QOS_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_qos_info);

  res = arad_pp_eg_qos_port_info_get_verify(
          unit,
          local_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_port_info_get_unsafe(
          unit,
		  core_id,
          local_port_ndx,
          port_qos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_qos_port_info_get()", local_port_ndx, 0);
}


uint32
  soc_ppd_eg_qos_params_php_remark_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PHP_REMARK_KEY                   *php_key,
    SOC_SAND_IN  uint32                                      dscp_exp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_QOS_PARAMS_PHP_REMARK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(php_key);

  res = arad_pp_eg_qos_params_php_remark_set_verify(
          unit,
          php_key,
          dscp_exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_params_php_remark_set_unsafe(
          unit,
          php_key,
          dscp_exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_qos_params_php_remark_set()", 0, 0);
}


uint32
  soc_ppd_eg_qos_params_php_remark_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PHP_REMARK_KEY                   *php_key,
    SOC_SAND_OUT uint32                                      *dscp_exp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_QOS_PARAMS_PHP_REMARK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(php_key);
  SOC_SAND_CHECK_NULL_INPUT(dscp_exp);

  res = arad_pp_eg_qos_params_php_remark_get_verify(
          unit,
          php_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_params_php_remark_get_unsafe(
          unit,
          php_key,
          dscp_exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_qos_params_php_remark_get()", 0, 0);
}

#ifdef BCM_88660
uint32
  soc_ppd_eg_qos_params_marking_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_KEY *qos_key,
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_PARAMS *qos_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_qos_params_marking_set_verify(unit, qos_key, qos_params);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_params_marking_set_unsafe(unit, qos_key, qos_params);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_qos_params_marking_set()", 0, 0);
}

uint32
  soc_ppd_eg_qos_params_marking_get(
    SOC_SAND_IN     int unit,
    SOC_SAND_IN     SOC_PPC_EG_QOS_MARKING_KEY *qos_key,
    SOC_SAND_OUT    SOC_PPC_EG_QOS_MARKING_PARAMS *qos_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_qos_params_marking_get_verify(unit, qos_key, qos_params);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_params_marking_get_unsafe(unit, qos_key, qos_params);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_qos_params_marking_get()", 0, 0);
}

uint32
  soc_ppd_eg_qos_global_info_set(
   SOC_SAND_IN int unit,
   SOC_SAND_IN SOC_PPC_EG_QOS_GLOBAL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_qos_global_info_set_verify(unit, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_global_info_set_unsafe(unit, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_qos_global_info_set()", 0, 0);
}

uint32 
  soc_ppd_eg_qos_global_info_get(
   SOC_SAND_IN int unit,
   SOC_SAND_OUT SOC_PPC_EG_QOS_GLOBAL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_eg_qos_global_info_get_verify(unit, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_global_info_get_unsafe(unit, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_qos_global_info_get()", 0, 0);
}

#endif 


uint32
  soc_ppd_eg_qos_params_remark_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_MAP_KEY                          *in_qos_key,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PARAMS                           *out_qos_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_QOS_PARAMS_REMARK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(in_qos_key);
  SOC_SAND_CHECK_NULL_INPUT(out_qos_params);

  res = arad_pp_eg_qos_params_remark_set_verify(
          unit,
          in_qos_key,
          out_qos_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_params_remark_set_unsafe(
          unit,
          in_qos_key,
          out_qos_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_qos_params_remark_set()", 0, 0);
}


uint32
  soc_ppd_eg_qos_params_remark_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_MAP_KEY                          *in_qos_key,
    SOC_SAND_OUT SOC_PPC_EG_QOS_PARAMS                           *out_qos_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_QOS_PARAMS_REMARK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(in_qos_key);
  SOC_SAND_CHECK_NULL_INPUT(out_qos_params);

  res = arad_pp_eg_qos_params_remark_get_verify(
          unit,
          in_qos_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_qos_params_remark_get_unsafe(
          unit,
          in_qos_key,
          out_qos_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_qos_params_remark_get()", 0, 0);
}


uint32
  soc_ppd_eg_encap_qos_params_remark_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_QOS_PARAMS_REMARK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(in_encap_qos_key);
  SOC_SAND_CHECK_NULL_INPUT(out_encap_qos_params);

  res = arad_pp_eg_encap_qos_params_remark_set_verify(
          unit,
          in_encap_qos_key,
          out_encap_qos_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_qos_params_remark_set_unsafe(
          unit,
          in_encap_qos_key,
          out_encap_qos_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_qos_params_remark_set()", 0, 0);
}


uint32
  soc_ppd_eg_encap_qos_params_remark_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_ENCAP_QOS_PARAMS_REMARK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(in_encap_qos_key);
  SOC_SAND_CHECK_NULL_INPUT(out_encap_qos_params);

  res = arad_pp_eg_encap_qos_params_remark_get_verify(
          unit,
          in_encap_qos_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_eg_encap_qos_params_remark_get_unsafe(
          unit,
          in_encap_qos_key,
          out_encap_qos_params
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_qos_params_remark_get()", 0, 0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>



