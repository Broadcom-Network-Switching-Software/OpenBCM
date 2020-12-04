/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


/*
 * Basic_include_file.
 */
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_host_board.h>
#include <appl/diag/dcmn/utils_board_general.h>
#include <appl/diag/dpp/utils_front_end_host_card.h>
#include <appl/diag/dpp/utils_line_PTG.h>
#include <appl/diag/dpp/utils_nvram_configuration.h>

#include <appl/dpp/UserInterface/ui_pure_defi.h>

#if !DUNE_BCM
/*
 * General include file for reference design.
 */
#include "pub/include/ref_sys.h"
#include "pub/include/d_string_macros.h"

#include "DuneDriver/fap20v/include/fap20v_framework.h"
#include "DuneDriver/fap20v/include/fap20v_chip_defines.h"

#include "../../../../h/drv/mem/eeprom.h"
#include "../../../../h/usrApp.h"

/*
 * INCLUDE FILES:
 * {
 */
#ifdef SAND_LOW_LEVEL_SIMULATION
/* { */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
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
#include "msgQLib.h"

#include "sigLib.h"
#include "intLib.h"

/*
 * This file is required to complete definitions
 * related to timers and clocks. For display/debug
 * purposes only.
 */
#include "private/timerLibP.h"
#include "logLib.h"
#include "taskHookLib.h"
#include "shellLib.h"
#include "dbgLib.h"
#include "drv/mem/eeprom.h"
#include "usrApp.h"

#include "arch/ppc/vxPpcLib.h"

/* } */
#endif
/*
 * Utilities include file.
 */
#include "Utilities/include/utils_defi.h"
#include "UserInterface/include/ui_pure_defi.h"
/*
 * Background include file.
 */
#include "Background/include/bckg_defi.h"


#include "Utilities/include/utils_board_general.h"
#include "Utilities/include/utils_host_board.h"
#include "Utilities/include/utils_line_card.h"
#include "Utilities/include/utils_front_end_host_card.h"
#include "Utilities/include/utils_fe200_card.h"
#include "Utilities/include/utils_fe600_card.h"
#include "Utilities/include/utils_i2c_mem.h"
#include "Utilities/include/utils_version.h"
#include "Utilities/include/utils_line_TGS.h"
#include "Utilities/include/utils_line_FTG.h"
#include "Utilities/include/utils_line_PTG.h"
#include "Utilities/include/utils_line_GFA.h"
#include "Utilities/include/utils_line_GFA_FAP21V.h"
#include "Utilities/include/utils_line_TEVB.h"
#include "Utilities/include/utils_nvram_configuration.h"
/*
 * }
 */

#else /* !DUNE_BCM */


#define test_d_printf d_printf

#endif /* !DUNE_BCM */

/*
 * Do Module init and checks.
 */
#if defined(__DUNE_GTO_BCM_CPU__) && (defined(LINUX) || defined(UNIX))
STATIC void
  utils_host_board_INIT()
{
  const char
    *msg = NULL,
    *proc_name = "utils_host_board_INIT";

  d_printf("Enter: utils_host_board_INIT\n\r");

  if ( sizeof(D_BOARD_INFO_STRCT) > (D_BOARD_INFO_SIZE_IN_LONG*sizeof(long)) )
  {
    /*
     * The size of 'D_BOARD_INFO_STRCT' should be less than '(D_BOARD_INFO_SIZE_IN_LONG*sizeof(long))'
     * The PC SW uses this structure.
     */
    msg = "Notify all parties to change the size of D_BOARD_INFO. That is, HANA.";
    gen_err(FALSE,TRUE,0,0,
            msg,
            proc_name,SVR_ERR,0,TRUE,0,0,FALSE);
  }

  if ( SOC_D_USR_APP_FLAVOR_NOF_ENUMS > 255 )
  {
    /*
     * SOC_D_USR_APP_FLAVOR_NOF_ENUMS must be less than 255. This number is stored in the NvRam as 8 bit number
     */
    msg = "SOC_D_USR_APP_FLAVOR_NOF_ENUMS must be less than 255. This number is stored in the NvRam as 8 bit number.";
    gen_err(FALSE,TRUE,0,0,
            msg,
            proc_name,SVR_ERR,0,TRUE,0,0,FALSE);
  }

  /*
   * Other boards INIT()...
   */

}
#endif /* #if defined(__DUNE_GTO_BCM_CPU__) && (defined(LINUX) || defined(UNIX)) */

/*
 * Resetting a device through mezzanine epld reset word (offset 0x08).
 * User of this method should know which bit he wants to reset.
 *
 *  const unsigned int deassert_not_assert_reset     -
 *    TRUE  - Out of reset.
 *    FALSE - In reset
 *  const unsigned int device_location_on_reset_word -
 *    In fabric card: 0 - fe1, 1 - fe2, 2 - host epld
 */
int
  reset_device_on_host_board(
    const unsigned int deassert_not_assert_reset,
    const unsigned int device_location_on_reset_word
  )
{
  int ex;
  unsigned int  offset;
  unsigned char prev_reset_val, new_reset_val;
  /*
   */
  ex = 0;
  offset = (int)(&(((EPLD_REGS *)0)->reset_control));
  /*
   */
  if(device_location_on_reset_word > 7)
  {
    ex = -1;
    goto exit;
  }
  /*
   */
  prev_reset_val = read_epld_reg(offset);
  if(deassert_not_assert_reset)
  {
    new_reset_val = prev_reset_val | BIT(device_location_on_reset_word);
  }
  else
  {
    new_reset_val = prev_reset_val & RBIT(device_location_on_reset_word);
  }
  write_epld_reg(new_reset_val, offset);
  goto exit;
  /*
   */
exit:
  return ex;
}
/*
 */

/*****************************************************
*NAME
*  init_host_board
*TYPE: PROC
*DATE: 24/JUL/2002
*MODIFIED: 26/JAN/2003
*FUNCTION:
*  the mezzanine carry out initializations related to the
*  host bord he's clinged to. currently only base / option
*  registers 4/5 of the 860 are done here, but future
*  initialization to the host board should be doen here.
*CALLING SEQUENCE:
*  init_host_bord()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    NVRam information regarding base/option registers 4/5.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Initialized base/optionregisters 4/5.
*REMARKS:
*  This procedure may not return to the caller if a
*  fatal error is encountered.
*SEE ALSO:
 */
void 
  init_host_board(
  )
{
#if !defined (SAND_LOW_LEVEL_SIMULATION)
#ifdef NEGEV
  unsigned long
    reg_base,
    reg_vals[4];
#endif
#endif
#if defined(__DUNE_GTO_BCM_CPU__) && (defined(LINUX) || defined(UNIX))
  unsigned short
    card_type,
    host_board_serial_number;
  char
    board_version[B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN];
#if LINK_PSS_LIBRARIES
  int
    err=0;
#endif
  unsigned int
    chip_ver;

  /**/
  sal_usleep(1);

  test_d_printf("Enter: init_host_board\n\r");
  utils_host_board_INIT();

  if ( get_slot_id() )
  {
    /* we have a host card */
    host_board_type_from_nv(&card_type);
    host_board_version_from_nv(board_version);
    host_board_sn_from_nv(&host_board_serial_number);

    d_printf("card type: %d\n\r", card_type);
    switch(card_type)
    {
    case LINE_CARD_GFA_PETRA_DDR2:
    case LINE_CARD_GFA_PETRA_DDR3:
    case LINE_CARD_GFA_PETRA_DDR3_STREAMING:
    {
      break;
    }
    case LINE_CARD_GFA_PETRA_B_DDR3:
    case LINE_CARD_GFA_PETRA_B_DDR3_STREAMING:
    case LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3:
    case LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3_STREAMING:
    {
      break;
    }
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN:
	case LINE_CARD_GFA_PETRA_B_INTERLAKEN_2:
    {
      break;  
    }
    case FABRIC_CARD_FE600_01:
      {
#if LINK_FE600_LIBRARIES
        int
          err ;
        err = fe600_bsp_init_host_board(card_type);
        if (err)
        {
          d_printf(
            "ERROR reported by fe600_bsp_init_host_board (code %d)\r\n",
            err
            ) ;
        }
        board_set_device_type(
          D_BOARD_CHIP_TYPE_FE600
          );
#endif /*#if LINK_FE600_LIBRARIES*/

        break;
      }
#if LINK_FE200_LIBRARIES
    case FABRIC_CARD_01:
      {
        int
          err ;
        err = init_dune_fabric_card() ;
        if (err)
        {
          d_printf(
            "ERROR reported by init_dune_fabric_card (code %d)\r\n",
            err
          ) ;
        }
        board_set_device_type(
          D_BOARD_CHIP_TYPE_FE200
        );
        break;
      }
#endif /*#if LINK_FE200_LIBRARIES*/
#if LINK_PSS_LIBRARIES
    case LINE_CARD_01:
    case LINE_CARD_FAP10M_B:
      {
        err = line_card_init_host_board(
          board_version,
          host_board_serial_number,
          &chip_ver
          );
        if (err)
        {
          d_printf(
            "ERROR reported by line_card_init_host_board (code %d)\r\n",
            err
            ) ;
        }
        if(chip_ver == FAP10M_EXPECTED_CHIP_VER_01)
        {
          board_set_device_type(
            D_BOARD_CHIP_TYPE_FAP10M_A
            );
        }
        else
        {
          board_set_device_type(
            D_BOARD_CHIP_TYPE_FAP10M_B
            );
        }
        break;

      }
#endif
    case LOAD_BOARD_PB:
      {
        break;
      }
    case LOAD_BOARD_FE600:
      {
#if LINK_FE600_LIBRARIES
        int
          err ;
        err = fe600_bsp_init_host_board(card_type) ;
        if (err)
        {
          d_printf(
            "ERROR reported by load_board_fe_init_board (code %d)\r\n",
            err
            ) ;
        }
        board_set_device_type(
          D_BOARD_CHIP_TYPE_FE600
          );
#endif /*#if LINK_FE600_LIBRARIES */
        break;
      }
default:
      {
#ifndef SAND_LOW_LEVEL_SIMULATION
#ifdef NEGEV
        /* standalone / unknown mezzanine card - standard init from nvram*/
        base_and_option_registers_from_nv(reg_vals);
        reg_base = vxImmrGet();
        *BR4(reg_base) = reg_vals[0];
        *BR5(reg_base) = reg_vals[1];
        *OR4(reg_base) = reg_vals[2];
        *OR5(reg_base) = reg_vals[3];
        /* gen_err (msg) that we found slot_id but card is satnd alone*/
#endif /* NEGEV */
/*}*/
#else
/*{*/
/*}*/
#endif
        break;
      }
    }
#ifndef SAND_LOW_LEVEL_SIMULATION

    /*
     * Check whether front-end card exist.
     */
    {
      if (front_does_card_exist_app())
      {
        unsigned long
          front_end_type ;
        front_end_type = SOC_UNKNOWN_CARD ;

        front_end_get_host_board_type(&front_end_type);

        switch (front_end_type)
        {
          case FRONT_END_TGS:
          case FRONT_END_FTG:
          case FRONT_END_TEVB:
          {
            /*
             * Power on the FPGA
             */
            d_printf("\r\n"
              "Power UP FPGA\n\r");
            d_printf("======================================\n\r");
            front_end_FPGAs_power_on(TRUE, TRUE);
            break ;
          }
          case SOC_FRONT_END_PTG:
          {
            break ;
          }
        }

        switch (front_end_type)
        {
#if LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES
          case FRONT_END_TGS:
          {
            tgs_init_host_board(board_version) ;
            break ;
          }
          case FRONT_END_FTG:
          {
            ftg_init_host_board(board_version) ;
            break ;
          }
#endif /* LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES */
#if LINK_PETRA_LIBRARIES
          case SOC_FRONT_END_PTG:
          {
            d_printf("utils_ptg_init_host_board: %x\n\r", board_version);
            utils_ptg_init_host_board(board_version) ;
            break ;
          }
#endif /* LINK_PETRA_LIBRARIES */
#if LINK_TIMNA_LIBRARIES || LINK_T20E_LIBRARIES
          case FRONT_END_TEVB:
          {
            tevb_init_host_board();
            break;
          }
#endif /* LINK_TIMNA_LIBRARIES || LINK_T20E_LIBRARIES*/
          default:
          {
            d_printf(
              "init_host_board(): UNKNOWN front end type (%u). ==> NO initialization!\r\n",
              front_end_type
            ) ;
            break ;
          }
        }
      }
    }
#endif /* !SAND_LOW_LEVEL_SIMULATION */
  }
  else
  {
#ifndef SAND_LOW_LEVEL_SIMULATION
#ifdef NEGEV
    /* standalone mezzanine card - standard init from nvram*/
    base_and_option_registers_from_nv(reg_vals);
    reg_base = vxImmrGet();
    *BR4(reg_base) = reg_vals[0];
    *BR5(reg_base) = reg_vals[1];
    *OR4(reg_base) = reg_vals[2];
    *OR5(reg_base) = reg_vals[3];
#endif /* NEGEV */
/*}*/
#else
/*{*/
/*}*/
#endif
  }
#endif /*#if defined(__DUNE_GTO_BCM_CPU__) && (defined(LINUX) || defined(UNIX))*/
}

/*****************************************************
*NAME
*  utils_is_in_crate
*TYPE: PROC
*DATE: 22/JUL/2004
*MODIFIED: 22/JUL/2004
*FUNCTION:
*  Check if the CPU is in CRATE.
*CALLING SEQUENCE:
*  utils_is_in_crate()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Crate if exist.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int
*      Indicator.
*      - TRUE:  In crate.
*      - FALSE: Not in crate.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */
unsigned int
  utils_is_in_crate(
    void
  )
{
  unsigned char
    crate_type;
  unsigned int
    in_crate;

  in_crate = FALSE;
  crate_type = get_crate_type();

  switch (crate_type)
  {
    case CRATE_TEST_EVAL:
    case CRATE_32_PORT_40G_STANDALONE:
    case CRATE_64_PORT_10G_STANDALONE:
    case CRATE_32_PORT_40G_IO:
    case CRATE_32_PORT_10G_IO:
    case CRATE_FABRIC_BOX:
    case CRATE_NEGEV_REV_B:
      in_crate = TRUE;
      break ;

    default:
      in_crate = FALSE ;
      break ;
  }

  return in_crate;
}

/*****************************************************
*NAME
*  utils_is_crate_gobi
*TYPE: PROC
*DATE: 16/AUG/2004
*MODIFIED: 16/AUG/2004
*FUNCTION:
*  Check if the crate ID is Gobi CRATE.
*CALLING SEQUENCE:
*  utils_is_crate_gobi()
*INPUT:
*  SOC_SAND_DIRECT:
*    const unsigned int crate_type -
*      Crate type
*  SOC_SAND_INDIRECT:
*    Nonr.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int
*      Indicator.
*      - TRUE:  Is Gobi Id.
*      - FALSE: Not Gobi Id.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */
unsigned int
  utils_is_crate_gobi(
    const unsigned int crate_type
  )
{
  unsigned int
    is_crate_gobi;

  is_crate_gobi = FALSE;

  switch (crate_type)
  {
    case CRATE_32_PORT_40G_STANDALONE:
    case CRATE_64_PORT_10G_STANDALONE:
       is_crate_gobi = TRUE;
      break ;

    default:
      is_crate_gobi = FALSE ;
      break ;
  }

  return is_crate_gobi;
}

/*****************************************************
*NAME
*  utils_is_crate_negev
*TYPE: PROC
*DATE: 22/AUG/2004
*MODIFIED: 22/AUG/2004
*FUNCTION:
*  Check if the crate ID is Negev CRATE.
*CALLING SEQUENCE:
*  utils_is_crate_negev()
*INPUT:
*  SOC_SAND_DIRECT:
*    const unsigned int crate_type -
*      Crate type
*  SOC_SAND_INDIRECT:
*    Nonr.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int
*      Indicator.
*      - TRUE:  Is Negev Id.
*      - FALSE: Not Negev Id.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */
unsigned int
  utils_is_crate_negev(
    const unsigned int crate_type
  )
{
  unsigned int
    is_crate_negev;

  is_crate_negev = FALSE;

  switch (crate_type)
  {
    case CRATE_TEST_EVAL:
    case CRATE_NEGEV_REV_B:
      is_crate_negev = TRUE;
      break ;

    default:
      is_crate_negev = FALSE ;
      break ;
  }

  return is_crate_negev;
}


/*
 * {
 * D_BOARD_INFO structure
 */
/*****************************************************
*NAME
*  board_information_get
*TYPE: PROC
*DATE: 22/AUG/2004
*MODIFIED: 22/AUG/2004
*FUNCTION:
*  Get Run-Time infromation of the board.
*
*INPUT:
*  SOC_SAND_DIRECT:
*    D_BOARD_INFO* b_info
*      Poniter to 'D_BOARD_INFO' structure.
*      Loaded with BOARD information.
*  SOC_SAND_INDIRECT:
*    Nonr.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int
*      Indicator.
*      - FALSE: OK.
*      - TRUE:  ERROR.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */
int
  board_information_get(
    SOC_SAND_OUT D_BOARD_INFO* b_info
  )
#ifndef SAND_LOW_LEVEL_SIMULATION
{
  int
    err;
  unsigned short
    board_type;
  D_BOARD_NV_CPU_INFO
    cpu_info;
  char
    board_version[B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN];
  
  err = FALSE;

  board_information_clear(b_info);

  /*
   * Get the device type.
   */
  host_board_type_from_nv(&board_type);
  b_info->inf.host_board_type = board_type;
  b_info->inf.slot_id    = get_slot_id();

  host_board_version_from_nv(board_version);
/*
 * Get the device type.
 * Assuming that all the devices on the board are from the same type
 * and get the version of the first one.
 */
  {
    unsigned long
      version_reg = 0,
      version_reg_offset = 0,
      dbg_ver_reg = 0,
      dbg_ver_reg_offset = 0,
      chip_type;
    uint32
      dbg_ver;

    version_reg_offset =
#if !DUNE_BCM
      SOC_SAND_OFFSETOF(FAP20V_REGS, global_regs.detail.version);
#else
      0;
#endif
    version_reg =
      *((unsigned long *)(GP1_BASE_ADDR | version_reg_offset));

    chip_type = 
      SOC_SAND_GET_FLD_FROM_PLACE(version_reg, CHIP_TYPE_SHIFT,  CHIP_TYPE_MASK);
    
    if(bsp_card_is_same_family(board_type, LINE_CARD_GFA_PETRA_A))
    {
      if( 
          (strcmp(board_version, "v1.01") == 0) ||
          (strcmp(board_version, "v1.02") == 0)
        )
      {
        dbg_ver = SOC_PETRA_REVISION_FLD_VAL_A1;
      }
      else if (strcmp(board_version, "v1.03") == 0)
      {
        dbg_ver = SOC_PETRA_REVISION_FLD_VAL_A2;
      }
      else if (strcmp(board_version, "v1.04") == 0)
      {
        dbg_ver = SOC_PETRA_REVISION_FLD_VAL_A3;
      }
      else if (strcmp(board_version, "v1.05") == 0)
      {
        dbg_ver = SOC_PETRA_REVISION_FLD_VAL_A4;
      }
      else
      {
        dbg_ver = 0xff;
      }
      b_info->inf.device_dbg_ver = dbg_ver;
      
    }
    else if(bsp_card_is_same_family(board_type, LINE_CARD_GFA_PETRA_B))
    {
      dbg_ver = 0xff;
    }
    else
    {
      dbg_ver_reg_offset =
#if !DUNE_BCM
      SOC_SAND_OFFSETOF(FAP20V_REGS, global_regs.detail.spare_reg);
#else
      2;
#endif
      dbg_ver_reg =
        *((unsigned long *)(GP1_BASE_ADDR | dbg_ver_reg_offset) );

      b_info->inf.device_ver =
         SOC_SAND_GET_FLD_FROM_PLACE(version_reg, CHIP_VER_SHIFT,  CHIP_VER_MASK);

#if !DUNE_BCM
      if(b_info->inf.device_ver == FAP20V_EXPECTED_CHIP_VER_02)
      {
        /* in rev B devices dbg_version is located in the spare register */
        b_info->inf.device_dbg_ver =
          SOC_SAND_GET_FLD_FROM_PLACE(dbg_ver_reg, FAP20V_B_DBG_VER_SHIFT, FAP20V_B_DBG_VER_MASK);
      }
      else
      {
         b_info->inf.device_dbg_ver =
          SOC_SAND_GET_FLD_FROM_PLACE(version_reg, DBG_VER_SHIFT,  DBG_VER_MASK);
      }
#endif
    }
  }

  host_board_sn_from_nv(&b_info->inf.host_board_serial_number);
  b_info->inf.crate_type   = get_crate_type();
  b_info->inf.prog_version = get_prog_ver();

  board_nv_cpu_info_get(&cpu_info);
  b_info->inf.cpu_board_serial_number = cpu_info.board_serial_number;
  if (b_info->inf.slot_id != 0)
  {
    b_info->inf.crate_id = get_crate_id();
  }
  d_app_state_get(&b_info->inf.app_state);
  d_usr_app_flavor_get(&b_info->inf.usr_app_flavor);

  b_info->inf.front_host_board_serial_number = 0; /* marked not exist*/
  if (front_does_card_exist())
  {
    front_board_sn_from_nv(&b_info->inf.front_host_board_serial_number);
  }

  b_info->inf.host_db_serial_number = 0; /* marked not exist*/
#if (LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES)
  if (
#if (LINK_FAP20V_LIBRARIES && LINK_FAP21V_LIBRARIES)
     ||
#endif
    )
  {
    host_db_board_sn_from_nv(&b_info->inf.host_db_serial_number);
  }
#endif

  return err;
}
#else /* SAND_LOW_LEVEL_SIMULATION */
{
  int
    err;
  unsigned short
    board_type;
  D_BOARD_NV_CPU_INFO
    cpu_info;

  err = FALSE;

  board_information_clear(b_info);

  /*
   * Get the device type.
   */
  host_board_type_from_nv(&board_type);
  b_info->inf.host_board_type = board_type;
  b_info->inf.slot_id    = get_slot_id();
/*
 * Get the device type.
 * Assuming that all the devices on the board are from the same type
 * and get the version of the first one.
 */
  {
    unsigned long
      version_reg;

    version_reg = *((unsigned long *)(GP1_BASE_ADDR | DUNE_VERSION_REGISTERS_ADDRESS));
    b_info->inf.device_ver =
       SOC_SAND_GET_FLD_FROM_PLACE(version_reg, CHIP_VER_SHIFT,  CHIP_VER_MASK);
    b_info->inf.device_dbg_ver =
       SOC_SAND_GET_FLD_FROM_PLACE(version_reg, DBG_VER_SHIFT,  DBG_VER_MASK);
  }

  host_board_sn_from_nv(&b_info->inf.host_board_serial_number);
  b_info->inf.crate_type   = get_crate_type();
  b_info->inf.prog_version = get_prog_ver();

  board_nv_cpu_info_get(&cpu_info);
  b_info->inf.cpu_board_serial_number = cpu_info.board_serial_number;
  if (b_info->inf.slot_id != 0)
  {
    b_info->inf.crate_id = get_crate_id();
  }
  d_app_state_get(&b_info->inf.app_state);
  d_usr_app_flavor_get(&b_info->inf.usr_app_flavor);

  b_info->inf.front_host_board_serial_number = 0; /* marked not exist*/
  if (front_does_card_exist != NULL)
  {
    if (front_does_card_exist())
    {
      front_board_sn_from_nv(&b_info->inf.front_host_board_serial_number);
    }
  }

  b_info->inf.host_db_serial_number = 0; /* marked not exist*/
  if (
    (0) )
  {
    host_db_board_sn_from_nv(&b_info->inf.host_db_serial_number);
  }
  return err;
}
#endif /* SAND_LOW_LEVEL_SIMULATION */

/*
 * Print 'D_BOARD_INFO'
 */
void
  board_information_print(
    const D_BOARD_INFO* b_info,
    const unsigned int  short_format
  )
{
  if (b_info == NULL)
  {
    d_printf("board_information_print() Got NULL\n\r");
    goto exit;
  }

  d_printf("Host Board Type  0x%X\n\r", b_info->inf.host_board_type);
  d_printf("Slot Id :        %d\n\r", b_info->inf.slot_id);
  d_printf("Device Ver:      %d\n\r", b_info->inf.device_ver);
  d_printf("Device Dbg Ver:  %d\n\r", b_info->inf.device_dbg_ver);
  d_printf("Host Board SN    %d\n\r", b_info->inf.host_board_serial_number);
  d_printf("CPU Board SN     %d\n\r", b_info->inf.cpu_board_serial_number);
  d_printf("Crate Type:      0x%X\n\r", b_info->inf.crate_type);
  d_printf("Prog Version:    %d\n\r", b_info->inf.prog_version);
  d_printf("Crate Id:        %d\n\r", b_info->inf.crate_id);
  d_printf("RunTimeAppState: %d\n\r",
           b_info->inf.app_state.enum_state,
           d_app_state_set_state_to_str(&b_info->inf.app_state, FALSE));
  d_printf("UsrAppFlavor:    %d\n\r",
           b_info->inf.usr_app_flavor,
           d_usr_app_flavor_to_str(b_info->inf.usr_app_flavor, FALSE));
  d_printf("Front Card SN:   %d\n\r", b_info->inf.front_host_board_serial_number);
  d_printf("Host_db Card SN: %d\n\r", b_info->inf.host_db_serial_number);


exit:
  return;
}

/*
 * Clears 'D_BOARD_INFO'
 */
void
  board_information_clear(
    D_BOARD_INFO* b_info
  )
{
  if (b_info == NULL)
  {
    d_printf("board_information_clear() Got NULL\n\r");
    goto exit;
  }
  memset(b_info, 0xFF, sizeof(D_BOARD_INFO));

exit:
  return;
}

/*
 * }
 */

#ifndef SAND_LOW_LEVEL_SIMULATION /* NV paramss !SAND_LOW_LEVEL_SIMULATION { */
/*
 * {
 * D_BOARD_NV_APP_INFO structure
 */

/*****************************************************
*NAME
*  board_nv_app_information_get
*  board_nv_app_information_set
*TYPE: PROC
*DATE: 8/JUN/2005
*MODIFIED: 8/JUN/2005
*FUNCTION:
*  Get/Set from/to the BOARD NvRam application deepened values.
*  'BOARD' is the Fap10M-Line-Card/FE-card/GFA-Line.
*  Every card interpret this block differently.
*  The NvRam is EEPROM of size of 8K-Bytes.
*  This function give the ability to read/write all of the block.
*  Every card that need, should implement a warper on top of this functions.
*  Refer to 'gfa_nv_app_information_get, D_GFA_NV_APP_INFO' for example.
*INPUT:
*  SOC_SAND_DIRECT:
*    D_BOARD_NV_APP_INFO* b_app
*      Poniter to 'D_BOARD_NV_APP_INFO' structure.
*      Loaded with BOARD EEPROM information.
*  SOC_SAND_INDIRECT:
*    Nonr.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int
*      Indicator.
*      - FALSE: OK.
*      - TRUE:  ERROR.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */

int
  board_nv_app_information_get(
    D_BOARD_NV_APP_INFO* b_app
  )
{
  unsigned int
    offset;
  STATUS
    status;
  int
    err;

  err = FALSE;

  board_nv_app_information_clear(b_app);

  offset = (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
              host_ser_ee_block_02.un_host_serial_ee_block_02.
              host_serial_ee_block_02.host_board_param_1)) +
              SERIAL_EE_FIRST_OFFSET ;
  status = sysNvRamGet((char*)b_app,sizeof(D_BOARD_NV_APP_INFO),(int)offset) ;
  if (status == ERROR)
  {
    err = 1 ;
    goto exit ;
  }

exit:
  return err;
}

int
  board_nv_app_information_set(
    const D_BOARD_NV_APP_INFO* b_app
  )
{
  unsigned int
    offset;
  STATUS
    status;
  int
    err;

  err = FALSE;


 offset = (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
             host_ser_ee_block_02.un_host_serial_ee_block_02.
             host_serial_ee_block_02.host_board_param_1)) +
             SERIAL_EE_FIRST_OFFSET ;
 status = sysNvRamSet((char*)b_app,sizeof(D_BOARD_NV_APP_INFO),(int)offset) ;
 if (status == ERROR)
 {
   err = 1 ;
   goto exit ;
 }

exit:
  return err;
}

void
  board_nv_app_information_print(
    const D_BOARD_NV_APP_INFO* b_app,
    const unsigned int         short_format
  )
{
  if (b_app == NULL)
  {
    d_printf("board_nv_app_information_print() Got NULL\n\r");
    goto exit;
  }

  soc_sand_print_hex_buff((const char*)&b_app, sizeof(D_BOARD_NV_APP_INFO),10 );

exit:
  return;
}

void
  board_nv_app_information_clear(
    D_BOARD_NV_APP_INFO* b_app
  )
{
  if (b_app == NULL)
  {
    d_printf("board_nv_app_information_clear() Got NULL\n\r");
    goto exit;
  }
  memset(b_app, 0x00, sizeof(D_BOARD_NV_APP_INFO));

exit:
  return;
}



/*
 * }
 */


/*
 * D_BOARD_NV_CPU_INFO -- CPU board - NvRam values
 * {
 */

/*****************************************************
*NAME
*  board_nv_cpu_info_get
*TYPE: PROC
*DATE: 8/JUN/2005
*MODIFIED: 8/JUN/2005
*FUNCTION:
*  Get/Set from/to the BOARD NvRam application deepened values.
*  'BOARD' is the Fap10M-Line-Card/FE-card/GFA-Line.
*  Every card interpret this block differently.
*  The NvRam is EEPROM of size of 8K-Bytes.
*  This function give the ability to read/write all of the block.
*  Every card that need, should implement a warper on top of this functions.
*  Refer to 'gfa_nv_app_information_get, D_GFA_NV_APP_INFO' for example.
*INPUT:
*  SOC_SAND_DIRECT:
*    D_BOARD_NV_APP_INFO* b_app
*      Poniter to 'D_BOARD_NV_APP_INFO' structure.
*      Loaded with BOARD EEPROM information.
*  SOC_SAND_INDIRECT:
*    Nonr.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int
*      Indicator.
*      - FALSE: OK.
*      - TRUE:  ERROR.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */

int
  board_nv_cpu_info_get(
    D_BOARD_NV_CPU_INFO* cpu_info
  )
{
  unsigned int
    offset;
  STATUS
    status;
  int
    err;

  err = FALSE;

  board_nv_cpu_info_get_clear(cpu_info);


  offset = (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
                ser_ee_block_02.un_serial_ee_block_02.
                serial_ee_block_02.board_serial_number)) +
                SERIAL_EE_FIRST_OFFSET;
  status = sysNvRamGet((char*)cpu_info,sizeof(D_BOARD_NV_CPU_INFO),(int)offset) ;
  if (status == ERROR)
  {
    err = 1 ;
    goto exit ;
  }

exit:
  return err;
}


void
  board_nv_cpu_info_print(
    const D_BOARD_NV_CPU_INFO* cpu_info,
    const unsigned int         short_format
  )
{
  if (cpu_info == NULL)
  {
    d_printf("board_nv_cpu_info_print() Got NULL\n\r");
    goto exit;
  }

  soc_sand_print_hex_buff((const char*)&cpu_info, sizeof(D_BOARD_NV_CPU_INFO),10 );

exit:
  return;
}

void
  board_nv_cpu_info_get_clear(
    D_BOARD_NV_CPU_INFO* cpu_info
  )
{
  if (cpu_info == NULL)
  {
    d_printf("board_nv_cpu_info_get_clear() Got NULL\n\r");
    goto exit;
  }
  memset(cpu_info, 0xFF, sizeof(D_BOARD_NV_CPU_INFO));

exit:
  return;
}



/*
 * }
 */
#endif /* NV paramss !SAND_LOW_LEVEL_SIMULATION } */



/*
 * App State
 * {
 */

static
  D_APP_STATE
    D_app_state = { 0 };

/*****************************************************
*NAME
*  d_app_state_get
*TYPE: PROC
*DATE: 30/AUG/2005
*MODIFIED: 30/AUG/2005
*FUNCTION:
*  Get the local state of the board.
*INPUT:
*  SOC_SAND_DIRECT:
*    void.
*  SOC_SAND_INDIRECT:
*    "d_app_state" buffer Loaded with "D_app_state"
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */
void
  d_app_state_get(
     D_APP_STATE* d_app_state
  )
{
  memcpy(d_app_state, &D_app_state, sizeof(D_app_state));
}

/*****************************************************
*NAME
*  d_app_state_set_state
*TYPE: PROC
*DATE: 30/AUG/2005
*MODIFIED: 30/AUG/2005
*FUNCTION:
*  Get the local state of the board.
*INPUT:
*  SOC_SAND_DIRECT:
*    const D_APP_STATE_ENUM e_state -
*      Enumerator of local state
*  SOC_SAND_INDIRECT:
*    None
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */
void
  d_app_state_set_state(
     const D_APP_STATE_ENUM e_state
  )
{
  D_app_state.enum_state = e_state;
}

D_APP_STATE_ENUM 
  d_app_state_get_state(
  )
{
  return D_app_state.enum_state;
}

/*****************************************************
*NAME
*  d_app_state_set_state_to_str
*TYPE: PROC
*DATE: 30/AUG/2005
*MODIFIED: 30/AUG/2005
*FUNCTION:
*  Prints the enum string.
*
*INPUT:
*  SOC_SAND_DIRECT:
*    const D_APP_STATE* d_app_state
*      Structure to print
*    const unsigned int short_format
*      Format to print.
*      In case of TRUE, short format is used,
*      The string size is not more than D_APP_STATE_SET_STATE_PRINT_SHORT_MAX.
*
*  SOC_SAND_INDIRECT:
*    None
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */
const char*
  d_app_state_set_state_to_str(
    const D_APP_STATE* d_app_state,
    const unsigned int short_format
  )
{
  const char
    *string_p = "";

  switch (d_app_state->enum_state)
  {
  case D_APP_STATE_E_NOT_VALID:
    if (short_format)
    {
      string_p = "NT_VLD";
    }
    else
    {
      string_p = "D_APP_STATE_E_NOT_VALID";
    }
    break;
  case D_APP_STATE_E_BOOT_FIN:
    if (short_format)
    {
      string_p = "BT_FIN";
    }
    else
    {
      string_p = "D_APP_STATE_E_BOOT_FIN";
    }
    break;
  case D_APP_STATE_E_START_UP_APP_FIN:
    if (short_format)
    {
      string_p = "AP_FIN";
    }
    else
    {
      string_p = "D_APP_STATE_E_START_UP_APP_FIN";
    }
    break;
  case D_APP_STATE_E_START_UP_FIN_NO_APP:
    if (short_format)
    {
      string_p = "NO_APP";
    }
    else
    {
      string_p = "D_APP_STATE_E_START_UP_FIN_NO_APP";
    }
    break;

  case D_APP_STATE_E_APP_ERR:
    if (short_format)
    {
      string_p = "ER_APP";
    }
    else
    {
      string_p = "D_APP_STATE_E_APP_ERR";
    }
    break;


  default:
    string_p = "'d_app_state_set_state_print' - received not valid enum number";
    break;
  }

  return string_p;
}


/*
 * }
 */




/*
 * App Activate
 * {
 */


/*
 * String structure.
 * Use the API "d_usr_app_flavor_to_str()" to access it in runtime.
 *
 * Short-Format: The max short format byte size is 15.
 */
static const char* const
  D_usr_app_flavor_strs[SOC_D_USR_APP_FLAVOR_NOF_ENUMS][2] =
{                                       /*123456789012345*/
  {D_TO_STR(SOC_D_USR_APP_FLAVOR_NONE),      "UsrAppFlvrNone"},
  {D_TO_STR(SOC_D_USR_APP_FLAVOR_NORMAL),    "UsrAppFlvrNrmal"},
  {D_TO_STR(SOC_D_USR_APP_FLAVOR_MINIMAL),   "UsrAppFlvrMinml"},
  {D_TO_STR(SOC_D_USR_APP_FLAVOR_WIN_01),    "UsrAppFlvrWin01"},
  {D_TO_STR(SOC_D_USR_APP_FLAVOR_SERVER),   "UsrAppSrvrOnly"},
  {D_TO_STR(SOC_D_USR_APP_FLAVOR_NORMAL_AND_SERVER),"UsrAppWithSrvr"},
  {D_TO_STR(SOC_D_USR_APP_FLAVOR_SERVER_WITH_INIT), "UsrAppSrvrAndInit"},
  {D_TO_STR(SOC_D_USR_APP_FLAVOR_MINIMAL_NO_APP),   "UsrAppMinNoApp"},

};



#ifndef SAND_LOW_LEVEL_SIMULATION
/*****************************************************
*NAME
*  d_usr_app_flavor_get
*TYPE: PROC
*DATE: 30/AUG/2005
*MODIFIED: 30/AUG/2005
*FUNCTION:
*  Get the application activation flavor from NvRam.
*  The setting is done only via the CLI (nvram ...).
*INPUT:
*  SOC_SAND_DIRECT:
*    void.
*  SOC_SAND_INDIRECT:
*    NvRam state
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */
void
  d_usr_app_flavor_get(
     SOC_D_USR_APP_FLAVOR* usr_app_flavor
  )
{
  int
    err;
  EE_AREA
    *ee_area ;
  unsigned int
    offset ;
  char
    nv_value;
  char
    *proc_name = "d_usr_app_flavor_get";

  nv_value = 0;
  ee_area = (EE_AREA *)0 ;
  offset  = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.usr_app_flavor)) ;

  /*
   * Using the Nv value, and not the run-time, since:
   * 1. this value is not to be change in run-time.
   * 2. At the BSP load, the run-time image might not be updated yet.
   */
  err =
    nvRamGet(&nv_value,sizeof(nv_value),(int)offset) ;
  if (err)
  {
    /*
     * In case of error - report and do not activate the application
     */
    nv_value = SOC_D_USR_APP_FLAVOR_NONE;
    gen_err(
      TRUE,FALSE,(int)err,0,
      "d_usr_app_flavor_get - fail. Setting no APP.",
      proc_name, SVR_ERR, D_APP_ACTIVATE_GET_01, TRUE, 0, -1, FALSE) ;
    goto exit ;
  }

exit:
  if (usr_app_flavor)
  {
#if !DUNE_BCM
    *usr_app_flavor = (SOC_D_USR_APP_FLAVOR)nv_value;
#else
    *usr_app_flavor = SOC_D_USR_APP_FLAVOR_NORMAL;
#endif
  }
  return;
}
#endif


/*****************************************************
*NAME
*  d_usr_app_flavor_to_str
*TYPE: PROC
*DATE: 30/AUG/2005
*MODIFIED: 30/AUG/2005
*FUNCTION:
*  Prints the enum string.
*
*INPUT:
*  SOC_SAND_DIRECT:
*    const SOC_D_USR_APP_FLAVOR  usr_app_flavor
*      Structure to print
*    const unsigned int      short_format
*      Format to print.
*      In case of TRUE, short format is used,
*      the max short format byte size is 15.
*
*  SOC_SAND_INDIRECT:
*    None
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */
const char*
  d_usr_app_flavor_to_str(
    const SOC_D_USR_APP_FLAVOR usr_app_flavor,
    const unsigned         short_format
  )
{
  const char
    *string_p = "";

  if (usr_app_flavor >= SOC_D_USR_APP_FLAVOR_NOF_ENUMS)
  {
    string_p = "'d_usr_app_flavor_to_str' - received not valid enum number";
    goto exit;
  }

  if (short_format)
  {
    string_p = D_usr_app_flavor_strs[usr_app_flavor][1];
  }
  else
  {
    string_p = D_usr_app_flavor_strs[usr_app_flavor][0];
  }

exit:
  return string_p;
}


/*
 * }
 */



