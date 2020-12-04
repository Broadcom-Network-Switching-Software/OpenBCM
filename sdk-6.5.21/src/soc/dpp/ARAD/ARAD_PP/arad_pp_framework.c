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

#define _ERR_MSG_MODULE_NAME BSL_SOC_COMMON



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

















CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_framework[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_PROCEDURE_DESC_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_ERRORS_DESC_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRAMEWORK_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRAMEWORK_GET_ERRS_PTR),
  

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_framework[] =
{
  
  


  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};






CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_framework_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_framework;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_framework_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_framework;
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

