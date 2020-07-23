
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */

/*
 * General include file for reference design.
 */

#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_i2c_mem.h>

#if !DUNE_BCM

#include "pub/include/ref_sys.h"
/*
 * INCLUDE FILES:
 * {
 */
#ifdef SAND_LOW_LEVEL_SIMULATION
/* { */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "conio.h"
/*
 * Utilities include file.
 */
#include "Utilities/include/utils_defi.h"
/* } */
#else
/* { */
#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "stdlib.h"
#include "taskLib.h"
#include "errnoLib.h"
#include "sysLib.h"
#include "usrLib.h"
#include "tickLib.h"
#include "ioLib.h"
#include "iosLib.h"
#include "logLib.h"
#include "pipeDrv.h"
#include "timers.h"
#include "sigLib.h"

/*
 * This file is required to complete definitions
 * related to timers and clocks. For display/debug
 * purposes only.
 */
#include "private\timerLibP.h"
#include "logLib.h"
#include "taskHookLib.h"
#include "shellLib.h"
#include "dbgLib.h"
#include "drv/mem/eeprom.h"
#include "usrApp.h"

/*
 * Utilities include file.
 */
#include "Utilities/include/utils_defi.h"

#include "UserInterface/include/ui_pure_defi.h"

#include "utils_i2c_mem.h"

#endif

/* } */
#endif
/*
 * }
 */



/*****************************************************
*NAME
*  utils_i2c_mem_read
*TYPE: PROC
*DATE: 25/JUNE/2003
*FUNCTION:
*  Read one long from the AT24C64-E2PROM.
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned long *read_data -
*      Loaded by this procedure, with the value
*      at the desired offset.
*    unsigned int  notify_errors -
*      TRUE: Notify, via 'gen_err' on errors.
*      FALSE: Do not notify, via 'gen_err' on errors.
*             This mode is good, when an error is expected to be found.
*             Example: look at 'ftg_e2prom_test_if_exist()'.
*  SOC_SAND_INDIRECT:
*    I2C mechanism.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      error if different from ZERO.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  + Do not take semaphore...
*    The calling procedure is responsible for that.
*SEE ALSO:
 */
int
  utils_i2c_mem_read(
    unsigned char device_address,/*7 bits*/
    unsigned char i2c_data[I2C_MEM_MAX_IC2_BUFF_SIZE],
    unsigned short  i2c_data_len,
    unsigned int  silent
  )
{
  int
    ret ;
  char
    err_msg[80*3] ;
  unsigned
    int
      err_flag,
      char_i;
  char
      *proc_name ;
  unsigned
    char
     read_addr,
     write_addr;

  proc_name = "utils_i2c_mem_read" ;
  read_addr = device_address << 1;
  read_addr |= 0x1;
  write_addr = device_address << 1;
  /*
   * Fill with Dummy data.
   */
  for(char_i=0;char_i<I2C_MEM_MAX_IC2_BUFF_SIZE;char_i++)
  {
    i2c_data[char_i] = 0xFF;
  }

  ret =
    i2c_load_buffer(
      (unsigned char)read_addr,
      i2c_data_len,
      i2c_data,&err_flag,err_msg
    ) ;
  gen_err(TRUE,FALSE,(int)err_flag,0,
      err_msg,proc_name,
      SVR_WRN,I2C_MEM_ERR_01,FALSE,0,-1,FALSE) ;
  gen_err(TRUE,FALSE,(int)ret,0,
      "I2C fail. #1 ",proc_name,
      SVR_ERR,I2C_MEM_ERR_02,FALSE,0,-1,FALSE) ;
  if (ret)
  {
    goto gete_exit ;
  }
  i2c_start() ;
  ret = i2c_poll_tx(&err_flag,err_msg) ;
  gen_err(TRUE,FALSE,(int)err_flag,0,
      err_msg,proc_name,
      SVR_WRN,I2C_MEM_ERR_03,FALSE,0,-1,FALSE) ;
  gen_err(TRUE,FALSE,(int)ret,0,
    "I2C fail. #2 ",proc_name,
    SVR_ERR,I2C_MEM_ERR_04,FALSE,0,-1,FALSE) ;
  if (ret)
  {
    goto gete_exit ;
  }
  /*
   * Wait 100 micro sec to make sure received bytes are registered.
   */
  sal_usleep(100) ;
  ret =
    i2c_get_buffer(&i2c_data_len,i2c_data,&err_flag,err_msg) ;
  gen_err(TRUE,FALSE,(int)err_flag,0,
      err_msg,proc_name,
      SVR_WRN,I2C_MEM_ERR_05,FALSE,0,-1,FALSE) ;
  gen_err(TRUE,FALSE,(int)ret,0,
    "I2C fail. #3 ",proc_name,
    SVR_ERR,I2C_MEM_ERR_06,FALSE,0,-1,FALSE) ;
  if (ret)
  {
    goto gete_exit ;
  }

gete_exit:
  return (ret) ;
}





/*****************************************************
*NAME
*  utils_i2c_mem_write
*TYPE: PROC
*DATE: 25/JUNE/2003
*FUNCTION:
*  Write one long from the AT24C64-E2PROM.
*INPUT:
*  SOC_SAND_DIRECT:
*  SOC_SAND_INDIRECT:
*    I2C mechanism.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      error if different from ZERO.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  + Do not take semaphore...
*    The calling procedure is responsible for that.
*SEE ALSO:
 */
int
  utils_i2c_mem_write(
    unsigned char device_address,/*7 bits*/
    unsigned char i2c_data[I2C_MEM_MAX_IC2_BUFF_SIZE],
    unsigned short  i2c_data_len,
    unsigned int  silent
  )
{
  int
    ret ;
  char
    err_msg[80*3] ;
  unsigned
    int
      err_flag;
  char
      *proc_name ;
  unsigned
    char
     write_addr;

  proc_name = "utils_i2c_mem_write" ;

  write_addr = device_address << 1;

  ret =
    i2c_load_buffer(
      (unsigned char)write_addr,
      i2c_data_len,
      i2c_data,&err_flag,err_msg
    ) ;
  gen_err(TRUE,FALSE,(int)err_flag,0,
      err_msg,proc_name,
      SVR_WRN,I2C_MEM_ERR_01,FALSE,0,-1,FALSE) ;
  gen_err(TRUE,FALSE,(int)ret,0,
      "I2C fail. #1 ",proc_name,
      SVR_ERR,I2C_MEM_ERR_02,FALSE,0,-1,FALSE) ;
  if (ret)
  {
    goto gete_exit ;
  }
  i2c_start() ;
  ret = i2c_poll_tx(&err_flag,err_msg) ;
  gen_err(TRUE,FALSE,(int)err_flag,0,
      err_msg,proc_name,
      SVR_WRN,I2C_MEM_ERR_03,FALSE,0,-1,FALSE) ;
  gen_err(TRUE,FALSE,(int)ret,0,
    "I2C fail. #2 ",proc_name,
    SVR_ERR,I2C_MEM_ERR_04,FALSE,0,-1,FALSE) ;
  if (ret)
  {
    goto gete_exit ;
  }
  /*
   * Wait 100 micro sec to make sure received bytes are registered.
   */
  sal_usleep(100) ;

gete_exit:
  return (ret) ;
}



