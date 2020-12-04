/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __BSP_DRV_ERR_DEFS_H_INCLUDED__
/* { */
#define __BSP_DRV_ERR_DEFS_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

/*************
 * INCLUDES  *
 */
/* { */

#if !DUNE_BCM
  #include "DuneDriver/SAND/Utils/include/sand_os_interface.h"
  #include "DuneDriver/SAND/Management/include/sand_error_code.h"
  #include "pub/include/utils_defx.h"
#endif
/* } */

/*************
 * DEFINES   *
 */
/* { */
#define BSP_DRV_SILENT TRUE
#define BSP_DRV_NOT_SILENT !BSP_DRV_SILENT

/* } */

/*************
 *  MACROS   *
 */
/* { */
#define BSP_DRV_BOOT_FUNC_VALIDATE(e_func_name, e_err_code) \
{                                         \
  if (e_func_name == NULL)                \
  {                                       \
    BSP_DRV_SET_ERR_AND_EXIT(e_err_code)  \
  }                                       \
}

#define BSP_DRV_ERR_IF_NULL(e_ptr_, e_err_code) \
{                                         \
  if (e_ptr_ == NULL)                     \
  {                                       \
    BSP_DRV_SET_ERR_AND_EXIT(e_err_code)  \
  }                                       \
}


#define BSP_DRV_GEN_SVR_ERR(e_err_str, e_proc_name, e_err_code) \
    gen_err(FALSE,FALSE,0,0,e_err_str, e_proc_name, SVR_ERR,\
              e_err_code, TRUE, 0, 601, FALSE)


#define BSP_DRV_INIT_ERR_DEFS(e_proc_name, e_silent_flag)  \
  unsigned int m_ret = 0;                 \
  unsigned int m_silent_flag;             \
  char* m_proc_name = e_proc_name;        \
  m_silent_flag = e_silent_flag;

#define BSP_DRV_SET_ERR_AND_EXIT(e_err_code)  \
{                                           \
  m_ret = e_err_code;                       \
  goto exit ;                               \
}

#define BSP_DRV_PRINT_ERR_MSG(e_err_msg)   \
{                                                       \
  if (!m_silent_flag)                                   \
  {                                                     \
    d_printf(                                     \
            "\n\r--> ERROR: %s\n\r", e_err_msg);        \
  }                                                     \
}

#define BSP_DRV_PRINT_MSG(e_msg)           \
{                                                       \
  if (!m_silent_flag)                                   \
  {                                                     \
    d_printf(                                     \
            "\n\r--> %s\n\r", e_msg);                   \
  }                                                     \
}


#define BSP_DRV_EXIT_IF_ERR(e_sand_err,e_err_code) \
{                                         \
  m_ret = soc_sand_get_error_code_from_error_word(e_sand_err); \
  if(m_ret != 0)                          \
  {                                       \
    m_ret = e_err_code ;                  \
    goto exit ;                           \
  }                                       \
}


/*
 * Macro to handle procedure call which returns standard soc_sand
 * error code and, in case of error, sets error code, performs an exit function and quits.
 * Assumes local variables: ret, error_id, soc_sand_err
 * Assumes label: exit
 */
#define BSP_DRV_EXIT_AND_PRINT_IF_ERR(e_result, e_err_code, e_err_msg)     \
  m_ret = soc_sand_get_error_code_from_error_word(e_result);\
  if (SOC_SAND_OK != m_ret)                                 \
  {                                                     \
   /*                                                   \
    * Error detected. Quit with error.                  \
    */                                                  \
    BSP_DRV_PRINT_ERR_MSG(e_err_msg)       \
                                                        \
    goto exit ;                                         \
  }


#define BSP_DRV_EXIT_AND_PRINT_ERR    \
{                                     \
  if (m_ret && !(m_silent_flag))      \
    {                                 \
      d_printf(\
        "\n\r    + %-60s -- ERROR %u.\n\r",m_proc_name, m_ret);\
    }                                 \
    return m_ret;                     \
}

/* } */

/*************
 * TYPE DEFS *
 */
/* { */

/* } */

/*************
 * GLOBALS   *
 */
/* { */

/* } */

/*************
 * FUNCTIONS *
 */
/* { */

/* } */

#ifdef  __cplusplus
}
#endif


/* } __BSP_DRV_ERR_DEFS_H_INCLUDED__*/
#endif


