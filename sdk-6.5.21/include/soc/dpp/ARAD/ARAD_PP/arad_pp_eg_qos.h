/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_EG_QOS_INCLUDED__

#define __ARAD_PP_EG_QOS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_eg_qos.h>
#include <soc/dpp/PPC/ppc_api_port.h>













typedef enum
{
  
  SOC_PPC_EG_QOS_PORT_INFO_SET = ARAD_PP_PROC_DESC_BASE_EG_QOS_FIRST,
  SOC_PPC_EG_QOS_PORT_INFO_SET_PRINT,
  SOC_PPC_EG_QOS_PORT_INFO_SET_UNSAFE,
  SOC_PPC_EG_QOS_PORT_INFO_SET_VERIFY,
  SOC_PPC_EG_QOS_PORT_INFO_GET,
  SOC_PPC_EG_QOS_PORT_INFO_GET_PRINT,
  SOC_PPC_EG_QOS_PORT_INFO_GET_VERIFY,
  SOC_PPC_EG_QOS_PORT_INFO_GET_UNSAFE,
  SOC_PPC_EG_QOS_PARAMS_PHP_REMARK_SET,
  SOC_PPC_EG_QOS_PARAMS_PHP_REMARK_SET_PRINT,
  SOC_PPC_EG_QOS_PARAMS_PHP_REMARK_SET_UNSAFE,
  SOC_PPC_EG_QOS_PARAMS_PHP_REMARK_SET_VERIFY,
  SOC_PPC_EG_QOS_PARAMS_PHP_REMARK_GET,
  SOC_PPC_EG_QOS_PARAMS_PHP_REMARK_GET_PRINT,
  SOC_PPC_EG_QOS_PARAMS_PHP_REMARK_GET_VERIFY,
  SOC_PPC_EG_QOS_PARAMS_PHP_REMARK_GET_UNSAFE,
  SOC_PPC_EG_QOS_PARAMS_REMARK_SET,
  SOC_PPC_EG_QOS_PARAMS_REMARK_SET_PRINT,
  SOC_PPC_EG_QOS_PARAMS_REMARK_SET_UNSAFE,
  SOC_PPC_EG_QOS_PARAMS_REMARK_SET_VERIFY,
  SOC_PPC_EG_QOS_PARAMS_REMARK_GET,
  SOC_PPC_EG_QOS_PARAMS_REMARK_GET_PRINT,
  SOC_PPC_EG_QOS_PARAMS_REMARK_GET_VERIFY,
  SOC_PPC_EG_QOS_PARAMS_REMARK_GET_UNSAFE,
  SOC_PPC_EG_ENCAP_QOS_PARAMS_REMARK_SET,
  SOC_PPC_EG_ENCAP_QOS_PARAMS_REMARK_SET_PRINT,
  SOC_PPC_EG_ENCAP_QOS_PARAMS_REMARK_SET_UNSAFE,
  SOC_PPC_EG_ENCAP_QOS_PARAMS_REMARK_SET_VERIFY,
  SOC_PPC_EG_ENCAP_QOS_PARAMS_REMARK_GET,
  SOC_PPC_EG_ENCAP_QOS_PARAMS_REMARK_GET_PRINT,
  SOC_PPC_EG_ENCAP_QOS_PARAMS_REMARK_GET_VERIFY,
  SOC_PPC_EG_ENCAP_QOS_PARAMS_REMARK_GET_UNSAFE,
  ARAD_PP_EG_QOS_GET_PROCS_PTR,
  ARAD_PP_EG_QOS_GET_ERRS_PTR,
  

  
  ARAD_PP_EG_QOS_PROCEDURE_DESC_LAST
} ARAD_PP_EG_QOS_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_EG_QOS_DSCP_EXP_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_EG_QOS_FIRST,
  ARAD_PP_EG_QOS_IN_DSCP_EXP_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_QOS_EXP_MAP_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_QOS_PHP_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_PKT_HDR_OUT_OF_RANGE_ERR,
  ARAD_PP_REMARK_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_EG_ENCAP_QOS_PKT_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_ECN_CAPABLE_OUT_OF_RANGE_ERR,
  


  
  ARAD_PP_EG_QOS_ERR_LAST
} ARAD_PP_EG_QOS_ERR;









uint32
  arad_pp_eg_qos_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_eg_qos_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PORT_INFO                    *port_qos_info
  );

uint32
  arad_pp_eg_qos_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PORT_INFO                    *port_qos_info
  );

uint32
  arad_pp_eg_qos_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  );


uint32
  arad_pp_eg_qos_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_EG_QOS_PORT_INFO                    *port_qos_info
  );


uint32
  arad_pp_eg_qos_params_php_remark_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PHP_REMARK_KEY               *php_key,
    SOC_SAND_IN  uint32                                  dscp_exp
  );

uint32
  arad_pp_eg_qos_params_php_remark_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PHP_REMARK_KEY               *php_key,
    SOC_SAND_IN  uint32                                  dscp_exp
  );

uint32
  arad_pp_eg_qos_params_php_remark_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PHP_REMARK_KEY               *php_key
  );


uint32
  arad_pp_eg_qos_params_php_remark_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PHP_REMARK_KEY               *php_key,
    SOC_SAND_OUT uint32                                  *dscp_exp
  );

#ifdef BCM_88660
uint32
  arad_pp_eg_qos_params_marking_set_verify(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_KEY *qos_key,
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_PARAMS *qos_params
  );

uint32
  arad_pp_eg_qos_params_marking_set_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_KEY *qos_key,
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_PARAMS *qos_params
  );

uint32
  arad_pp_eg_qos_params_marking_get_verify(
    SOC_SAND_IN     int unit,
    SOC_SAND_IN     SOC_PPC_EG_QOS_MARKING_KEY *qos_key,
    SOC_SAND_OUT    SOC_PPC_EG_QOS_MARKING_PARAMS *qos_params
  );
uint32
  arad_pp_eg_qos_params_marking_get_unsafe(
    SOC_SAND_IN     int unit,
    SOC_SAND_IN     SOC_PPC_EG_QOS_MARKING_KEY *qos_key,
    SOC_SAND_OUT    SOC_PPC_EG_QOS_MARKING_PARAMS *qos_params
  );

uint32
  arad_pp_eg_qos_global_info_set_verify(
   SOC_SAND_IN int unit,
   SOC_SAND_IN SOC_PPC_EG_QOS_GLOBAL_INFO *info
  );

uint32
  arad_pp_eg_qos_global_info_set_unsafe(
   SOC_SAND_IN int unit,
   SOC_SAND_IN SOC_PPC_EG_QOS_GLOBAL_INFO *info
  );

uint32 
  arad_pp_eg_qos_global_info_get_verify(
   SOC_SAND_IN int unit,
   SOC_SAND_OUT SOC_PPC_EG_QOS_GLOBAL_INFO *info
  );

uint32 
  arad_pp_eg_qos_global_info_get_unsafe(
   SOC_SAND_IN int unit,
   SOC_SAND_OUT SOC_PPC_EG_QOS_GLOBAL_INFO *info
  );

#endif 

uint32
  arad_pp_eg_qos_params_remark_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_MAP_KEY                      *in_qos_key,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PARAMS                       *out_qos_params
  );

uint32
  arad_pp_eg_qos_params_remark_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_MAP_KEY                      *in_qos_key,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PARAMS                       *out_qos_params
  );

uint32
  arad_pp_eg_qos_params_remark_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_MAP_KEY                      *in_qos_key
  );


uint32
  arad_pp_eg_qos_params_remark_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_MAP_KEY                      *in_qos_key,
    SOC_SAND_OUT SOC_PPC_EG_QOS_PARAMS                       *out_qos_params
  );


uint32
  arad_pp_eg_encap_qos_params_remark_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  );

uint32
  arad_pp_eg_encap_qos_params_remark_set_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  );

uint32
  arad_pp_eg_encap_qos_params_remark_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key
  );


uint32
  arad_pp_eg_encap_qos_params_remark_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_MAP_KEY                *in_encap_qos_key,
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_QOS_PARAMS                 *out_encap_qos_params
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_qos_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_qos_get_errs_ptr(void);

uint32
  SOC_PPC_EG_QOS_MAP_KEY_verify(
    SOC_SAND_IN  SOC_PPC_EG_QOS_MAP_KEY *info
  );

uint32
  SOC_PPC_EG_ENCAP_QOS_MAP_KEY_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_MAP_KEY *info
  );

uint32
  SOC_PPC_EG_QOS_PARAMS_verify(
    SOC_SAND_IN  SOC_PPC_EG_QOS_PARAMS *info
  );

uint32
  SOC_PPC_EG_ENCAP_QOS_PARAMS_verify(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_PARAMS *info
  );

uint32
  SOC_PPC_EG_QOS_PHP_REMARK_KEY_verify(
    SOC_SAND_IN  SOC_PPC_EG_QOS_PHP_REMARK_KEY *info
  );

uint32
  SOC_PPC_EG_QOS_PORT_INFO_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_PORT_INFO *info
  );

#ifdef BCM_88660
uint32
  SOC_PPC_EG_QOS_MARKING_KEY_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_EG_QOS_MARKING_KEY *info
  );
uint32
  SOC_PPC_EG_QOS_MARKING_PARAMS_verify(
    SOC_SAND_IN  SOC_PPC_EG_QOS_MARKING_PARAMS *info
  );
uint32 SOC_PPC_EG_QOS_GLOBAL_INFO_verify(SOC_SAND_IN SOC_PPC_EG_QOS_GLOBAL_INFO *info);
#endif

uint32 arad_pp_port_eg_qos_marking_set(
   SOC_SAND_IN int unit,
   SOC_SAND_IN bcm_port_t port,
   SOC_SAND_IN int enable
  );
uint32   arad_pp_port_eg_qos_marking_get(
   SOC_SAND_IN int unit,
   SOC_SAND_IN bcm_port_t port,
   SOC_SAND_OUT int *enable
  );

uint32
  arad_pp_port_eg_ttl_inheritance_set(
   SOC_SAND_IN int unit,
   SOC_SAND_IN uint64 outlif_profiles
  );
uint32 
  arad_pp_port_eg_ttl_inheritance_get(
   SOC_SAND_IN int unit,
   SOC_SAND_OUT uint64 *outlif_profiles
   );
uint32
  arad_pp_port_eg_qos_inheritance_set(
   SOC_SAND_IN int unit,
   SOC_SAND_IN uint64 outlif_profiles
  );
uint32 
  arad_pp_port_eg_qos_inheritance_get(
   SOC_SAND_IN int unit,
   SOC_SAND_OUT uint64 *outlif_profiles
  );

uint32
  arad_pp_eg_qos_ether_dscp_remark_set(
   SOC_SAND_IN int unit,
   SOC_SAND_IN int enable
  );
uint32 
  arad_pp_eg_qos_ether_dscp_remark_get(
   SOC_SAND_IN int unit,
   SOC_SAND_OUT int *enable
  );


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



