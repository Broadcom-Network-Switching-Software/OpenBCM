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

#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_front_end_host_card.h>
#include <appl/diag/dpp/utils_nvram_configuration.h>

#if !DUNE_BCM
#include "Pub/include/ref_sys.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Pub/include/utils_defx.h"
#include "Pub/include/bckg_defx.h"

#include "../../../../h/usrApp.h"

#include "utils_front_end_host_card.h"
#include "utils_e2prom_AT24C64.h"
#include "utils_nvram_configuration.h"

#include "DuneDriver/SAND/Utils/include/sand_os_interface.h"
#include "DuneDriver/SAND/Utils/include/sand_workload_status.h"
#include "DuneDriver/SAND/Utils/include/sand_bitstream.h"

#include "Bsp_drv/include/bsp_cards_consts.h"

#endif
#define test_d_printf d_printf

void
  front_end_FPGAs_power_on(
    const unsigned int power_on,
    const unsigned int verbose
  )
{
  unsigned int
    epld_offset;
  unsigned char
    reset_control,
    MASK_A,
    MASK_B,
    MASK_PLL,
    XENPACK_A,
    XENPACK_B;
  int
    delay_ticks;

  static int
    first_reset = TRUE;

  delay_ticks = 60;

  /*
   * 76543210
   * ---------
   *   APB
   * A: Reset - A
   * B: Reset - B
   * P: Reset - PLL
   */
  XENPACK_B = BIT(6);
  XENPACK_A = BIT(7);
  MASK_A    = BIT(5);
  MASK_PLL  = BIT(4);
  MASK_B    = BIT(3);

  /*
   * Quick out of reset, ans read one register.
   */
  epld_offset = (int)(&(((EPLD_REGS *)0)->reset_control)) ;
  reset_control = read_epld_reg(epld_offset) ;

  /*
   * All in reset
   */
  if (first_reset)
  {
    reset_control &= (~(MASK_A | MASK_B | MASK_PLL | XENPACK_A | XENPACK_B));
  }
  else
  {
    reset_control &= (~(MASK_A | MASK_B | XENPACK_A | XENPACK_B));
  }
  write_epld_reg(reset_control,epld_offset) ;
  if (verbose)
  {
    d_printf("     FPGA A&B, (PLL,) Xenpacks           .....     pass\n\r");
  }
  sal_usleep(delay_ticks);

  if (power_on == FALSE)
  {
    /*
     * All in reset.
     */
    goto exit;
  }

  /*
   * take out of reset.
   */


  if (first_reset)
  {
    /*
     */
    reset_control |= MASK_PLL;
    write_epld_reg(reset_control,epld_offset) ;
    if (verbose)
    {
      d_printf("     PLL out of reset                    .....     pass\n\r");
    }
    sal_usleep(delay_ticks);
  }

  /*
   */
  reset_control |= MASK_A;
  write_epld_reg(reset_control,epld_offset) ;
  if (verbose)
  {
    d_printf("     FPGA A out of reset                 .....     pass\n\r");
  }
  sal_usleep(delay_ticks);


  /*
   */
  reset_control |= MASK_B;
  write_epld_reg(reset_control,epld_offset) ;
  if (verbose)
  {
    d_printf("     FPGA B out of reset                 .....     pass\n\r");
  }
  sal_usleep(delay_ticks);


  /*
   */
  reset_control |= (XENPACK_A|XENPACK_B);
  write_epld_reg(reset_control,epld_offset) ;
  if (verbose)
  {
    d_printf("     Xenpack A&B out of reset            .....     pass\n\r");
  }
  sal_usleep(delay_ticks);


  if (first_reset == TRUE)
  {
    first_reset = FALSE;
  }
exit:
  return;
}


int
  front_does_card_exist_app(
    void
  )
{
  int
    ret,
    sem_took ;
  char
    *proc_name ;
  static
    unsigned int
      have_answer = FALSE,
      answer = FALSE ;
  unsigned short
    card_type;
  proc_name = "front_does_card_exist_app" ;
  ret = 0 ;
  sem_took = FALSE;

  if (have_answer)
  {
    goto exit ;
  }
  have_answer = TRUE ;
  answer = FALSE ;

  host_board_type_from_nv(&card_type);
  if (bsp_card_is_same_family(LINE_CARD_GFA_FAP20V,card_type) == FALSE)
  {
    /* do nothing, before TEVB we returned here with FALSE */
  }

  if (bsp_card_is_same_family(LINE_CARD_GFA_FAP21V,card_type) == FALSE)
  {
    /* do nothing, before TEVB we returned here with FALSE */
  }

  if (bsp_card_is_same_family(LINE_CARD_GFA_PETRA_X,card_type) == FALSE)
  {
    /* do nothing, before TEVB we returned here with FALSE */
  }

  test_d_printf("front_does_card_exist_app card_type = %d\n\r", card_type);
  /*
   * Prevent multiple accessing to the I2C bus.
   * Because other tasks ('background_proc') are accessing the I2C.
   */
#if !DUNE_BCM
  if (sal_mutex_take(I2c_bus_semaphore, WAIT_FOREVER))
  {
    d_printf("%s - failed on sal_mutex_take()", proc_name) ;
    ret = TRUE ;
    goto exit ;
  }
#endif
  sem_took = TRUE;

  {
    unsigned
      int
        not_ready ;
    char
      err_msg[160] ;
    unsigned
      char
        dest_address ;
    unsigned
      int
        err_flg ;
    unsigned
      short
        i2c_data_len ;
    unsigned
      char
        i2c_data[SER_EPROM_BLOCK_SIZE + 1] ;
    dest_address = FRONT_SER_EPROM_I2C_ADDRESS_READ ;
    i2c_data_len = SER_EPROM_BLOCK_SIZE ;
    /*
     * Just a dummy bytes, to contain response.
     */
    memset(i2c_data,(int)0x00FF,i2c_data_len) ;
    i2c_load_buffer(
         dest_address,i2c_data_len,
         i2c_data,&err_flg,err_msg) ;
    if (err_flg)
    {
      goto exit ;
    }
    i2c_start() ;
    not_ready = i2c_poll_tx(&err_flg,err_msg) ;
    if (err_flg || not_ready)
    {
      goto exit ;
    }
    not_ready =
      i2c_get_buffer(
        &i2c_data_len,i2c_data,
        &err_flg,err_msg
      ) ;
    if (err_flg || not_ready)
    {
      goto exit ;
    }
    if (i2c_data_len != SER_EPROM_BLOCK_SIZE)
    {
      goto exit ;
    }
    answer = TRUE ;
  }


exit:
#if !DUNE_BCM
  if (sem_took)
  {
    sal_mutex_give(I2c_bus_semaphore) ;
  }
#endif

  ret = (int)answer ;
  return (ret) ;
}

int
  front_end_get_host_board_type(
    unsigned long* host_board_type
  )
{
  int
    ret;
  unsigned long
    reg_val,
    type_field;
  static unsigned long
    known_host_board_type,
    is_known = FALSE;
  unsigned short
    eeprom_type;
  SOC_BSP_CARDS_DEFINES
    card_type=0;


  ret = 0;
  eeprom_type = 0;

  if (is_known)
  {
    *host_board_type = known_host_board_type;
    goto exit;
  }

  if (front_does_card_exist_app() == FALSE)
  {
    /*
     * We do not have front end.
     */
    goto exit;
  }


  if (host_front_end_type_from_nv(&card_type))
  {
    ret = 1 ;
    goto exit ;
  }

  test_d_printf("front_end_get_host_board_type card_type = %d\n\r", card_type);

  if(card_type==FRONT_END_TEVB)
  {
    *host_board_type = FRONT_END_TEVB;
    goto exit;
  }
  else if (card_type==SOC_FRONT_END_PTG)
  {
    *host_board_type = SOC_FRONT_END_PTG;
    goto exit;
  }
  else
  {
    /*
     * Read-Wait-Read: Let system settle after reset.
     */
    reg_val = *((unsigned long*) 0x50100000);
    reg_val = 0;
    sal_usleep(100);
    reg_val = *((unsigned long*) 0x50100000);

    type_field = reg_val>>29;
    if (type_field == 0x0 || type_field == 0x3)
    {
      *host_board_type = FRONT_END_FTG;
    }
    else
    {
      *host_board_type = FRONT_END_TGS;
    }
  }


exit:

  is_known = TRUE;
  known_host_board_type = *host_board_type;
  return ret;
}



