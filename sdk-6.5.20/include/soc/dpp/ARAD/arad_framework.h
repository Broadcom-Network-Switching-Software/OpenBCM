/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_FRAMEWORK_H_INCLUDED__

#define __ARAD_FRAMEWORK_H_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>





#define ARAD_DEBUG (SOC_SAND_DEBUG)
#define ARAD_DEBUG_IS_LVL1   (ARAD_DEBUG >= SOC_SAND_DBG_LVL1)
#define ARAD_DEBUG_IS_LVL2   (ARAD_DEBUG >= SOC_SAND_DBG_LVL2)
#define ARAD_DEBUG_IS_LVL3   (ARAD_DEBUG >= SOC_SAND_DBG_LVL3)


#define ARAD_REVISION_FLD_VAL_A0  0x0
#define ARAD_REVISION_FLD_VAL_A1  0x10
#define ARAD_REVISION_FLD_VAL_A2  0x30
#define ARAD_REVISION_FLD_VAL_A3  0x32
#define ARAD_REVISION_FLD_VAL_A4  0x832





typedef enum
{
  ARAD_DEV_VER_A = 0, 
  ARAD_DEV_VER_B,
  ARAD_DEV_NOV_VERS
}ARAD_DEV_VER;








#define ARAD_ERR_IF_NULL(e_ptr_, e_err_code) \
{                                         \
  if (e_ptr_ == NULL)                     \
  {                                       \
    ARAD_SET_ERR_AND_EXIT(e_err_code)    \
  }                                       \
}

#define ARAD_INIT_ERR_DEFS                 \
  SOC_SAND_RET m_ret = SOC_SAND_OK;

#define ARAD_SET_ERR_AND_EXIT(e_err_code)  \
{                                           \
  m_ret = e_err_code;                       \
  goto exit ;                               \
}

#define ARAD_EXIT_IF_ERR(e_sand_err,e_err_code) \
{                                         \
  m_ret = soc_sand_get_error_code_from_error_word(e_sand_err); \
  if(m_ret != 0)                          \
  {                                       \
    m_ret = e_err_code ;                  \
    goto exit ;                           \
  }                                       \
}

#define ARAD_RETURN                  \
{                                     \
  return m_ret;                       \
}

#define ARAD_STRUCT_VERIFY(type, name, exit_num, exit_place) \
  res = type##_verify(                                \
        name                                               \
      );                                                   \
SOC_SAND_CHECK_FUNC_RESULT(res, exit_num, exit_place);

















uint32
  arad_disp_result(
    SOC_SAND_IN uint32          arad_api_result,
    SOC_SAND_IN char              *proc_name
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


