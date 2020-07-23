/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UTILS_ERR_DEFS_H_INCLUDED__
/* { */
#define __UTILS_ERR_DEFS_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

/*************
 * INCLUDES  *
 */
/* { */

#include "soc/dpp/SAND/Utils/sand_os_interface.h"
#include "soc/dpp/SAND/Management/sand_error_code.h"

/* } */

#define UTILS_GEN_SVR_ERR(e_err_str, e_proc_name, e_err_code, e_svr_wrn) \
    gen_err(FALSE,FALSE,0,0,e_err_str, e_proc_name, e_svr_wrn,\
              e_err_code, TRUE, 0, 601, FALSE)


#define UTILS_INIT_ERR_DEFS(e_proc_name)  \
  unsigned int m_ret = 0;                 \
  unsigned int m_silent_flag;             \
  char* m_proc_name = e_proc_name;        \
  m_silent_flag = 1;

#define UTILS_SET_ERR_AND_EXIT(e_err_code)  \
{                                           \
  m_ret = e_err_code;                       \
  goto exit ;                               \
}

#define UTILS_PRINT_ERR_MSG(e_silent_flag, e_err_msg)   \
{                                                       \
  m_silent_flag = e_silent_flag;                        \
  if (!m_silent_flag)                                   \
  {                                                     \
    soc_sand_os_printf(                                     \
            "\n\r--> ERROR: %s\n\r", e_err_msg);        \
  }                                                     \
}

#define UTILS_PRINT_MSG(e_silent_flag, e_msg)           \
{                                                       \
  m_silent_flag = e_silent_flag;                        \
  if (!m_silent_flag)                                   \
  {                                                     \
    soc_sand_os_printf(                                     \
            "--> %s\n\r", e_msg);                   \
  }                                                     \
}



#define UTILS_EXIT_IF_ERR(e_sand_err,e_err_code) \
{                                         \
  m_ret = soc_sand_get_error_code_from_error_word(e_sand_err); \
  if(m_ret != 0)                          \
  {                                       \
    m_ret = e_err_code ;                  \
    goto exit ;                           \
  }                                       \
}

#define UTILS_FUNC_VALIDATE(e_func_name, e_err_code) \
{                                         \
  if (e_func_name == NULL)                \
  {                                       \
    UTILS_SET_ERR_AND_EXIT(e_err_code)  \
  }                                       \
}

#define UTILS_ERR_IF_NULL(e_ptr, e_err_code) \
{                                           \
  if (e_ptr == NULL)                        \
  {                                         \
    UTILS_SET_ERR_AND_EXIT(e_err_code)      \
  }                                         \
}



/*
 * Macro to handle procedure call which returns standard soc_sand
 * error code and, in case of error, sets error code, performs an exit function and quits.
 * Assumes local variables: ret, error_id, soc_sand_err
 * Assumes label: exit
 */
#define UTILS_EXIT_AND_PRINT_IF_ERR(e_result, e_silent_flag, e_err_code, e_err_msg)     \
  m_silent_flag = e_silent_flag;                        \
  m_ret = soc_sand_get_error_code_from_error_word(e_result);\
  if (SOC_SAND_OK != m_ret)                                 \
  {                                                     \
   /*                                                   \
    * Error detected. Quit with error.                  \
    */                                                  \
    UTILS_PRINT_ERR_MSG(m_silent_flag, e_err_msg)       \
                                                        \
    goto exit ;                                         \
  }


#define UTILS_EXIT_AND_PRINT_ERR           \
{                                          \
  if ((m_ret) && (m_silent_flag == FALSE)) \
    {                                      \
      soc_sand_os_printf(\
        "\n\r    + %-60s -- ERROR %u.\n\r",m_proc_name, m_ret);\
    }                                      \
    return m_ret;                          \
}

#ifdef  __cplusplus
}
#endif


/* } __UTILS_ERR_DEFS_H_INCLUDED__*/
#endif


