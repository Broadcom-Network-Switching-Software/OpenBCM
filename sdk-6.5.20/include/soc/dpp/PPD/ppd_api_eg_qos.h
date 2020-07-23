/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_EG_QOS_INCLUDED__

#define __SOC_PPD_API_EG_QOS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_eg_qos.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_EG_QOS_PORT_INFO_SET = SOC_PPD_PROC_DESC_BASE_EG_QOS_FIRST,
  SOC_PPD_EG_QOS_PORT_INFO_SET_PRINT,
  SOC_PPD_EG_QOS_PORT_INFO_GET,
  SOC_PPD_EG_QOS_PORT_INFO_GET_PRINT,
  SOC_PPD_EG_QOS_PARAMS_PHP_REMARK_SET,
  SOC_PPD_EG_QOS_PARAMS_PHP_REMARK_SET_PRINT,
  SOC_PPD_EG_QOS_PARAMS_PHP_REMARK_GET,
  SOC_PPD_EG_QOS_PARAMS_PHP_REMARK_GET_PRINT,
  SOC_PPD_EG_QOS_PARAMS_REMARK_SET,
  SOC_PPD_EG_QOS_PARAMS_REMARK_SET_PRINT,
  SOC_PPD_EG_QOS_PARAMS_REMARK_GET,
  SOC_PPD_EG_QOS_PARAMS_REMARK_GET_PRINT,
  SOC_PPD_EG_QOS_GET_PROCS_PTR,
  SOC_PPD_EG_ENCAP_QOS_PARAMS_REMARK_SET,
  SOC_PPD_EG_ENCAP_QOS_PARAMS_REMARK_SET_PRINT,
  SOC_PPD_EG_ENCAP_QOS_PARAMS_REMARK_GET,
  SOC_PPD_EG_ENCAP_QOS_PARAMS_REMARK_GET_PRINT,
  
  
  SOC_PPD_EG_QOS_PROCEDURE_DESC_LAST
} SOC_PPD_EG_QOS_PROCEDURE_DESC;










uint32
  soc_ppd_eg_qos_port_info_set(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PORT_INFO                    *port_qos_info
  );


uint32
  soc_ppd_eg_qos_port_info_get(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_EG_QOS_PORT_INFO                    *port_qos_info
  );


uint32
  soc_ppd_eg_qos_params_php_remark_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PHP_REMARK_KEY               *php_key,
    SOC_SAND_IN  uint32                                dscp_exp
  );


uint32
  soc_ppd_eg_qos_params_php_remark_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PHP_REMARK_KEY               *php_key,
    SOC_SAND_OUT uint32                                *dscp_exp
  );

#ifdef BCM_88660

uint32
  soc_ppd_eg_qos_params_marking_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_KEY *qos_key,
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_PARAMS *qos_params
  );


uint32
  soc_ppd_eg_qos_params_marking_get(
    SOC_SAND_IN     int unit,
    SOC_SAND_IN     SOC_PPC_EG_QOS_MARKING_KEY *qos_key,
    SOC_SAND_OUT    SOC_PPC_EG_QOS_MARKING_PARAMS *qos_params
  );


uint32 
  soc_ppd_eg_qos_global_info_set(
   SOC_SAND_IN int unit,
   SOC_SAND_IN SOC_PPC_EG_QOS_GLOBAL_INFO *info
  );


uint32
  soc_ppd_eg_qos_global_info_get(
   SOC_SAND_IN int unit,
   SOC_SAND_OUT SOC_PPC_EG_QOS_GLOBAL_INFO *info
  );

#endif 


uint32
  soc_ppd_eg_qos_params_remark_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_MAP_KEY                      *in_qos_key,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PARAMS                       *out_qos_params
  );


uint32
  soc_ppd_eg_qos_params_remark_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_MAP_KEY                      *in_qos_key,
    SOC_SAND_OUT SOC_PPC_EG_QOS_PARAMS                       *out_qos_params
  );


uint32
  soc_ppd_eg_encap_qos_params_remark_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  );    


uint32
  soc_ppd_eg_encap_qos_params_remark_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

