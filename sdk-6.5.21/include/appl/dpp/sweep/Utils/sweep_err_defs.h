/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SWEEP_ERR_DEFS_H_INCLUDED__
/* { */
#define __SWEEP_ERR_DEFS_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#define SWP_PRINT_MSG(e_silent_flag, e_msg)   \
{                                             \
  if (!e_silent_flag)                         \
  {                                           \
    soc_sand_os_printf(                           \
            "\r--> %s\n\r", e_msg);         \
  }                                           \
}



#define SWP_INIT_ERR_DEFS(e_proc_name)    \
  unsigned int m_ret = 0;                 \
  unsigned int m_silent_flag;             \
  char* m_proc_name = e_proc_name;        \
  m_silent_flag = 1;

#define SWP_SET_ERR_AND_EXIT(e_err_code)  \
{                                         \
  m_ret = e_err_code;                       \
  goto exit ;                             \
}

#define SWP_PRINT_ERR_MSG(e_silent_flag, e_err_msg)     \
{                                                       \
  m_silent_flag = e_silent_flag;                        \
  if (!m_silent_flag)                                   \
  {                                                     \
    soc_sand_os_printf(                                     \
            "\n\r--> ERROR: %s\n\r", e_err_msg);        \
  }                                                     \
}

#define SWP_SET_ERR_AND_EXIT_WITH_MSG(e_err_code, e_err_msg) \
  {                                                         \
  m_ret = e_err_code;                                     \
  SWP_PRINT_MSG(FALSE, e_err_msg);                             \
  goto exit ;                                             \
}


#define SWP_EXIT_IF_ERR(e_sand_err,e_err_code) \
{                                         \
  if(soc_sand_get_error_code_from_error_word(e_sand_err))                          \
  {                                       \
    m_ret = e_err_code ;                  \
    goto exit ;                           \
  }                                       \
}

#define SWP_IS_ERR_RES(e_sand_err) (soc_sand_get_error_code_from_error_word(e_sand_err) == 0?FALSE:TRUE)

/*
 * Macro to handle procedure call which returns standard soc_sand
 * error code and, in case of error, sets error code, performs an exit function and quits.
 * Assumes local variables: ret, error_id, soc_sand_err
 * Assumes label: exit
 */
#define SWP_EXIT_AND_PRINT_IF_ERR(e_result, e_silent_flag, e_err_code, e_err_msg)     \
  m_silent_flag = e_silent_flag;                        \
  m_ret = soc_sand_get_error_code_from_error_word(e_result);\
  if (SOC_SAND_OK != m_ret)                                 \
  {                                                    \
   /*                                                  \
    * Error detected. Quit with error.                 \
    */                                                 \
    SWP_PRINT_ERR_MSG(m_silent_flag, e_err_msg)        \
                                                       \
    goto exit ;                                        \
  }


#define SWP_EXIT_AND_PRINT_ERR        \
{                                     \
  if (m_ret)                            \
    {                                 \
      soc_sand_os_printf(\
        "\n\r    + %-60s -- ERROR %u.\n\r",m_proc_name, m_ret);\
    }                                 \
    return m_ret;                       \
}

#ifdef  __cplusplus
}
#endif


/* } __SWEEP_ERR_DEFS_H_INCLUDED__*/
#endif


