/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */


#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#include <appl/diag/dpp/ref_design_starter.h>
#include <appl/diag/dpp/ref_design_petra_starter.h>

#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_error_defs.h>
#include <appl/diag/dpp/utils_host_board.h>
#include <appl/diag/dpp/utils_dffs_cpu_mez.h>
#include <appl/diag/dpp/ui_defx.h>
#include <stdio.h>

#if !DUNE_BCM
/*
 * Tasks information.
 */

#include "pub/include/tasks_info.h"
/*
 * INCLUDE FILES:
 */

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

/*
 * User Interface include file.
 */
#include "pub/include/ui_defx.h"
#include "UserInterface/include/ui_defi.h"
/*
 * Utilities include file.
 */
#include "pub/include/utils_defx.h"
/*
 * DUNE_RPC include file.
 */


#include "RefDesign/include/dune_rpc.h"

#include "RefDesign/include/dune_remote_access.h"
#include "RefDesign/include/dune_rpc_auto_network_learning.h"
#include "../../../../h/usrApp.h"
#include "sysLed.h"

#include "CSR/include/csr_server.h"

#include "Utilities/include/utils_fe600_card.h"

#include "sweep/SOC_SAND_FE600/include/sweep_fe600_app.h"
#include "DuneDriver/SOC_SAND_FE600/include/fe600_general.h"/*TODO!!*/

#include "sweep/PCP/include/sweep_pcp_app.h"

#include "Utilities/include/utils_init_dpss_for_demo.h"
#include "Utilities/include/utils_mem_flash.h"
#include "Utilities/include/utils_nvram_configuration.h"
#include "Utilities/include/utils_ip_mgmt.h"
#include "Utilities/include/utils_dffs_cpu_mez.h"
#if LINK_FAP20V_LIBRARIES
  #include "sweep/fap20v/include/sweep_fap20v_app.h"
  #include "FMC/include/fap20v_agent_callback.h"
  #include "Utilities/include/utils_line_TGS.h"
  #include "Utilities/include/utils_app_tgs.h"
  #include "Utilities/include/utils_line_GFA.h"
#endif
#if LINK_FAP21V_LIBRARIES
  #include "Utilities/include/utils_line_GFA_FAP21V.h"
  #include "Utilities/include/utils_line_TGS.h"
  #include "Utilities/include/utils_app_tgs_fap21v.h"
  #include "FMC/include/fap21v_agent_callback.h"
  #include "../include/ref_design21v_starter.h"
#endif
#if LINK_PETRA_LIBRARIES
  #include "Utilities/include/utils_line_PTG.h"
  #include "../include/ref_design_petra_starter.h"
  #include "FMC/include/petra_agent_callback.h"
#endif
#if LINK_T20E_LIBRARIES
  #include "Utilities/include/utils_line_TEVB.h"
#endif
#include "Utilities/include/utils_hw_diagnostics.h"
#include "DCL/include/dcl_transport_layer_extern.h"
#include "Pub/include/bckg_defx.h"
#include "sweep/SOC_SAND_FE200/include/sweep_fe200_app.h"

#define REF_DESIGN_INTERRUPT_HANDLING_ENABLE 1

#if REF_DESIGN_INTERRUPT_HANDLING_ENABLE
  #include "DuneDriver/SOC_SAND_FAP21V/include/fap21v_api_general.h"
  #include "DuneDriver/Petra/include/petra_api_general.h"
  #include "DuneDriver/SOC_SAND_FE600/include/fe600_api_general.h"
  #include "DuneDriver/T20E/include/t20e_api_general.h"
#endif

#else
/* debug code */
#define test_d_printf d_printf


STATUS
  taskLock(void);
STATUS
  taskUnlock(void);
void errnoSet(int);

void dune_rpc_set_local_unit(int unit, int unit)
{
}

int  gen_err(
        int compare,
        int reverse_logic,
        int status,
        int reference,
    const char *msg,
    const char *proc_name,
        int severity,
        int err_id,
        unsigned int store_in_nv,
        unsigned int alarm_flags,
        short min_time_between_events,
        unsigned int send_trap
  )
{
    int error_flag = TRUE;

    if (compare) {
        if (reverse_logic) {
            if (status != reference) {
                error_flag = FALSE ;
            }
        } else {
            if (status == reference) {
                error_flag = FALSE ;
            }
        }
    } else {
        error_flag = FALSE;
    }

    if (error_flag) {
        d_printf("gen_err in %s : %s \n\r", proc_name, msg);
    }

    return error_flag;
}

int
  utils_led_start_blink_led(
    unsigned int led_color,
    unsigned int nof_delay_ticks
  )
{
  return OK;
}
void
  utils_led_stop_blink_led(
    unsigned int led_color,
    unsigned int keep_it_on
  )
{
}

int  get_prog_ver(void)
{
  return 0x6403;
}

int  get_run_vals(
    char *string,
    int  strLen,
    int  offset
    )
{
  return OK;
}


#endif /* !DUNE_BCM */

/*************************************************************************
*
* progStop - stops the program
*
* Call this routine to end it all.
 */
void
  progStop(
    void
  )
{
#if !DUNE_BCM
  char
    *proc_name ;
  int
    ii,
    current_task_id,
    task_id ;
  proc_name = "progStop" ;

  current_task_id = d_taskIdSelf() ;

  /*
   * Clean up
   */
  /*
   * Actually timers should be canceled here.
   */
  timer_delete(get_ui_timer_id()) ;
  for (ii = 0; ii < NUM_TASKS ; ii++)
  {
    task_id = get_task_id(ii) ;
    if (current_task_id != task_id)
    {
      if (task_id != -1)
      {
        /*
         * Close only if task has been created.
         */
        taskDelete(task_id) ;
      }
    }
  }
  taskDelete(current_task_id) ;
#endif
}

/*
 * Check crate (Gobi) type againt the line-cards types.
 */
int 
  init_high_level_check_crate(
  )
{
  unsigned int
    crate_type;
  int
    err;
  const char
    *proc_name ="init_high_level_check_crate";

  err = FALSE;

  if (utils_is_in_crate() == FALSE)
  {
    goto exit;
  }

  crate_type = get_crate_type();

  if (utils_is_crate_gobi(crate_type)==FALSE)
  {
    goto exit;
  }
  switch (crate_type)
  {
  case CRATE_64_PORT_10G_STANDALONE:
    if (dune_rpc_are_all_line_type_x(LINE_CARD_01) == FALSE)
    {
      gen_err(
        FALSE,FALSE,0,0,
        "  Not all SOC_SAND_FAP10M card in SOC_SAND_FAP10M Gobi\r\n",
        proc_name, SVR_ERR, INIT_HIGH_LEVEL_CHECK_CRATE_ERR_01, TRUE, 0, -1, FALSE
      );
      err = TRUE;
    }

    break;
  case CRATE_32_PORT_40G_STANDALONE:
    if (
        (dune_rpc_are_all_line_type_x_family(LINE_CARD_GFA_FAP20V) == FALSE) &&
        (dune_rpc_are_all_line_type_x_family(LINE_CARD_GFA_FAP21V) == FALSE) &&
        (dune_rpc_are_all_line_type_x_family(LINE_CARD_GFA_PETRA_X) == FALSE) &&
        (dune_rpc_are_all_line_type_x_family(LINE_CARD_GFA_T20E) == FALSE)
       )
    {
      gen_err(
        FALSE,FALSE,0,0,
        "  Not all FAP2XV card in FAP2XV Gobi\r\n",
        proc_name, SVR_ERR, INIT_HIGH_LEVEL_CHECK_CRATE_ERR_02, TRUE, 0, -1, FALSE
      );
      err = TRUE;
    }
    break;


  default:
    gen_err(
      FALSE,FALSE,0,0,
      "  New card? Add needed checks!\r\n",
      proc_name, SVR_WRN, INIT_HIGH_LEVEL_CHECK_CRATE_ERR_03, TRUE, 0, -1, FALSE
    );
    err = FALSE;
  }


exit:
  return err;
}

unsigned int
  add_additional_memory(
      unsigned int verbose
   );

/*
 * High-Level Application.
 * Here we have the NP/PP/FAP/FE application
 */
void
  init_high_level_local_app(
    const SOC_D_USR_APP_FLAVOR       usr_app_flavor,
    const unsigned short         host_board_type,
          unsigned int           *err_on_app
  )
{
  char
    *proc_name ;
  int
    err ;
  unsigned int
    ;
  uint32
    ex,
    no_err;

  soc_sand_initialize_error_word(0, 0, &ex);

  no_err = ex;

  proc_name = "init_high_level_local_app";
  err = 0;


  if (usr_app_flavor == SOC_D_USR_APP_FLAVOR_NONE)
  {
    /* Nothing to do*/
    goto exit;
  }

  /*
   * Auto system learning, if we are in crate...
   */
  if ( utils_is_in_crate() )
  {
    utils_led_start_blink_led(FRONT_PANEL_YELLOW_LED, 30);
#ifdef DUNE_RPC_FORCE_LINE_CARD_DETECT
    if ((host_board_type == FABRIC_CARD_01) || (host_board_type == FABRIC_CARD_FE600_01) || (host_board_type == LOAD_BOARD_FE600))
    {
      if(dune_rpc_auto_learn_network_force(0))
      {
        gen_err(
        FALSE,FALSE,0,0,
        "\r\n"
        "  Failed to learn network architecture \r\n",
        proc_name, SVR_ERR, HIGH_LEVEL_LOCAL_APP_ERR_02, TRUE, 0, -1, FALSE
        );
      }
    }
    else
#endif
    if ( dune_rpc_auto_learn_network(0) )
    {
      gen_err(
        FALSE,FALSE,0,0,
        "\r\n"
        "  Failed to learn network architecture \r\n",
        proc_name, SVR_ERR, HIGH_LEVEL_LOCAL_APP_ERR_02, TRUE, 0, -1, FALSE
        );
    }
    utils_led_stop_blink_led(FRONT_PANEL_YELLOW_LED, FALSE);


    if( !dune_rpc_get_is_auto_learning_valid() )
    {
      gen_err(
        FALSE,FALSE,0,0,
        "\r\n"
        "  Network architecture is not valid\r\n",
        proc_name, SVR_ERR, HIGH_LEVEL_LOCAL_APP_ERR_03, TRUE, 0, -1, FALSE
      );
      goto exit;
    }

    if (dune_rpc_is_usr_app_flavor_in_the_system_compatible(TRUE) == FALSE)
    {
      gen_err(
        FALSE,FALSE,0,0,
        "  Call on all cards 'nvram update next_startup usr_app_flavor ...'\r\n"
        "  Than reset the cards\r\n",
        proc_name, SVR_ERR, HIGH_LEVEL_LOCAL_APP_ERR_04, TRUE, 0, -1, FALSE
      );
      goto exit;
    }
  }
#if LINK_T20E_LIBRARIES
  /*
   * T20E {
   */
  if (tevb_is_timna_connected())
  {
    d_printf(
        "Detected T20E. Run T20E application.\n\r");

    err = tevb_startup_sequence(
            verbose
          );

    gen_err(
      TRUE,FALSE,(int)err,0,
      "starterProc - tevb_startup_sequence fail...",
      proc_name, SVR_WRN, HIGH_LEVEL_LOCAL_APP_ERR_01, TRUE, 0, -1, FALSE) ;
#if REF_DESIGN_INTERRUPT_HANDLING_ENABLE

      do 
      {
        char
          err_msg[320];

        attach_irq((void *)t20e_interrupt_handler, 3, unit_, err_msg);

      } while(0);

#endif
  }
  /*
   * }
   */
#endif /*LINK_T20E_LIBRARIES*/

  /*Gobi crate checks*/
  if( init_high_level_check_crate() )
  {
    err = TRUE;
    goto exit;
  }

  if ( bsp_card_is_same_family(host_board_type, LINE_CARD_GFA_FAP20V) )
  {
#if LINK_FAP20V_LIBRARIES
/* { */
    if (gfa_is_fap20v_exist_on_board() && !tevb_is_timna_connected())
    {
      err = sweep_fap20v_app_init(get_crate_type(),
                                  dune_rpc_get_number_of_fabric_cards(),
                                  FALSE
                                  );
      gen_err(
        TRUE,FALSE,(int)err,0,
        "starterProc - fap20v_sweep_app_init fail...",
        proc_name, SVR_WRN, HIGH_LEVEL_LOCAL_APP_ERR_03, TRUE, 0, -1, FALSE) ;

      if (is_line_card_tgs_connected() && (err == FALSE))
      {
        err = tgs_application(0, TRUE, 1, FALSE);
        gen_err(
          TRUE,FALSE,(int)err,0,
          "starterProc - is_line_card_tgs_connected fail...",
          proc_name, SVR_WRN, HIGH_LEVEL_LOCAL_APP_ERR_04, TRUE, 0, -1, FALSE) ;
      }
      if (is_front_end_ixp2800_connected())
      {
        d_printf(
          "Detected IXP2800 Front card. Run IXP2800 application.\n\r"
        );
      }
      if (is_front_end_x10_connected())
      {
        d_printf(
          "Detected X10 Front card. Run X10 application.\n\r"
        );
      }

      /*
       * After all application of the FAP & NP were loaded,
       * clear all board and device counters and indications.
       */
      gfa_clear_counters_and_indications(0) ;
    }
    else
    {
      d_printf(
        "SOC_SAND_FAP20V does not exist on board.\n\r"
      );
    }
/* } */
#endif /*LINK_FAP20V_LIBRARIES*/
  }
  else if ( bsp_card_is_same_family(host_board_type, LINE_CARD_GFA_FAP21V) )
  {
#if LINK_FAP21V_LIBRARIES
/* { */
    if (gfa_fap21v_is_exist_on_board() 
#if LINK_T20E_LIBRARIES
      && !tevb_is_timna_connected()
#endif
      )
    {
      err = ref_design21v_app_init(
              get_crate_type(),
              dune_rpc_get_number_of_fabric_cards(),
              FALSE,
              &unit,
              FALSE
            );

      dune_rpc_set_local_unit(0, unit) ;

      gen_err(
        TRUE,FALSE,(int)err,0,
        "starterProc - sweep21v_app_init fail...",
        proc_name, SVR_WRN, HIGH_LEVEL_LOCAL_APP_ERR_03, TRUE, 0, -1, FALSE) ;

      if (usr_app_flavor != SOC_D_USR_APP_FLAVOR_SERVER)
      {
        if (is_line_card_tgs_connected())
        {
          soc_sand_set_error_code_into_error_word(
            soc_sand_get_error_code_from_error_word(err),
            &ex
          );
          if(ex == no_err)
          {
            err = tgs_application(unit, TRUE, 4, FALSE);

            gen_err(
              TRUE,FALSE,(int)err,0,
              "starterProc - is_line_card_tgs_connected fail...",
              proc_name, SVR_WRN, HIGH_LEVEL_LOCAL_APP_ERR_04, TRUE, 0, -1, FALSE) ;

            if (is_front_end_ixp2800_connected())
            {
              d_printf(
                "Detected IXP2800 Front card. Run IXP2800 application.\n\r"
              );
            }
            if (is_front_end_x10_connected())
            {
              d_printf(
                "Detected X10 Front card. Run X10 application.\n\r"
              );
            }
          }
        }
      }
      else
      {
        soc_sand_os_printf(
          " Due to User Flavor, TGS is not initialized.\n\r"
        );
      }
#if REF_DESIGN_INTERRUPT_HANDLING_ENABLE

      do 
      {
        char
          err_msg[320];

        attach_irq((void *)fap21v_interrupt_handler, 1, unit_, err_msg);

      } while(0);

#endif
    }
    else
    {
      d_printf(
        "SOC_SAND_FAP21V does not exist on board.\n\r"
      );
    }
/* } */
#endif /*LINK_FAP21V_LIBRARIES*/
  }
  else if ( (host_board_type == LINE_CARD_GFA_PETRA_DDR2) ||
            (host_board_type == LINE_CARD_GFA_PETRA_DDR3) ||
            (host_board_type == LINE_CARD_GFA_PETRA_DDR3_STREAMING) ||
            (host_board_type == LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3) ||
            (host_board_type == LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3_STREAMING))            
  {
#if LINK_PETRA_LIBRARIES
/* { */

    err = ref_design_petra_app_init(
            get_crate_type(),
            SOC_SAND_DEV_PETRA,
            dune_rpc_get_number_of_fabric_cards(),
            FALSE,
            &unit,
            FALSE
          );

    dune_rpc_set_local_unit(0, unit) ;

    gen_err(
      TRUE,FALSE,(int)err,0,
      "starterProc - swp_p_tm_app_init fail...",
      proc_name, SVR_WRN, HIGH_LEVEL_LOCAL_APP_ERR_03, TRUE, 0, -1, FALSE) ;

#if REF_DESIGN_INTERRUPT_HANDLING_ENABLE

    do 
    {
      char
        err_msg[320];

      attach_irq((void *)soc_petra_interrupt_handler, 1, unit_, err_msg);

    } while(0);

#endif
/* } */
#endif /*LINK_PETRA_LIBRARIES*/
  }
  else if ((host_board_type == FABRIC_CARD_FE600_01) )
  {
#if LINK_FE600_LIBRARIES
    fe600_bsp_fe600_reset(FALSE);
    sal_msleep(100);
    fe600_bsp_fe600_reset(FALSE);
    sal_msleep(100);
    fe600_bsp_fe600_reset(FALSE);
    sal_msleep(100);

    err = sweep_fe600_app_init(
            FALSE,
            FALSE
          );
    gen_err( 
      TRUE,FALSE,(int)err,0,
      "starterProc - sweep_fe600_app_init fail...",
      proc_name, SVR_WRN, HIGH_LEVEL_LOCAL_APP_ERR_02, TRUE, 0, -1, FALSE) ;

    sweep_fe600_bsp_led_blink_led_stop(FRONT_PANEL_YELLOW_LED, TRUE);

#if REF_DESIGN_INTERRUPT_HANDLING_ENABLE

    do 
    {
      char
        err_msg[320];

      attach_irq((void *)fe600_interrupt_handler, 2, unit_, err_msg);

    } while(0);

#endif
#endif /*#if LINK_FE600_LIBRARIES*/
  }
  else if ((host_board_type == LOAD_BOARD_FE600) )
  {
#if LINK_FE600_LIBRARIES
    lb_bsp_fe600_reset(FALSE);
    sal_msleep(100);
    lb_bsp_fe600_reset(FALSE);
    sal_msleep(100);
    lb_bsp_fe600_reset(FALSE);
    sal_msleep(100);

    err = sweep_fe600_app_init(
            FALSE,
            FALSE
          );
    gen_err( 
      TRUE,FALSE,(int)err,0,
      "starterProc - sweep_fe600_app_init fail...",
      proc_name, SVR_WRN, HIGH_LEVEL_LOCAL_APP_ERR_02, TRUE, 0, -1, FALSE) ;

    sweep_fe600_bsp_led_blink_led_stop(FRONT_PANEL_YELLOW_LED, TRUE);

#if REF_DESIGN_INTERRUPT_HANDLING_ENABLE

    do 
    {
      char
        err_msg[320];

      attach_irq((void *)fe600_interrupt_handler, 2, unit_, err_msg);

    } while(0);

#endif
#endif /*#if LINK_FE600_LIBRARIES*/
  }
  else if ((host_board_type == FABRIC_CARD_01) )
  {
#if LINK_FE200_LIBRARIES
    err = sweep_fe200_app_init(
            FALSE,
            FALSE
          );
#endif /*#if LINK_FE200_LIBRARIES*/
  }
  else if ( (host_board_type == LINE_CARD_FAP10M_B) ||
            (host_board_type == LINE_CARD_01)
          )
  {
#if INCLUDE_DUNE_RPC
/* { */
    err = init_dune_rpc(FALSE) ;
    gen_err(
      TRUE,FALSE,(int)err,0,
      "starterProc - DUNE RPC initialization fail...",
      proc_name,SVR_WRN,HIGH_LEVEL_LOCAL_APP_ERR_05,TRUE,0,-1,FALSE) ;
/* } */
#endif
  }

  if (usr_app_flavor == SOC_D_USR_APP_FLAVOR_SERVER           ||
      usr_app_flavor == SOC_D_USR_APP_FLAVOR_SERVER_WITH_INIT ||
      usr_app_flavor == SOC_D_USR_APP_FLAVOR_NORMAL_AND_SERVER
    )
  {
    soc_sand_tcm_callback_engine_stop();

#if !DUNE_BCM
    taskSpawn(
      "csrServer", 35, 0, STARTER_TASK_STACK_SIZE,(FUNCPTR)csr_server_init,
      4001,0,0,0,0,0,0,0,0,0
    );
#endif

  }
exit:
  if (err && (err_on_app!=NULL))
  {
    *err_on_app = TRUE;
  }
  return;
}

void
  ref_design_print_hw_status(
    void
  )
{
  char
    *slot_id_text = "",
    *crate_type_text = "";
  char
    host_board_cat_number[B_HOST_MAX_CATALOG_NUMBER_LEN+1],
    host_board_description[B_HOST_MAX_BOARD_DESCRIPTION_LEN+1],
    host_board_version[B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN+1],
#if (LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES)
    host_db_board_cat_number[B_HOST_DB_MAX_CATALOG_NUMBER_LEN+1],
    host_db_board_description[B_HOST_DB_MAX_BOARD_DESCRIPTION_LEN+1],
    host_db_board_version[B_HOST_DB_MAX_DEFAULT_BOARD_VERSION_LEN+1],
#endif
    err;
  unsigned short
    host_board_sn,
    host_board_type;
#if (LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES)
  unsigned short
    host_db_board_sn,
    host_db_board_type;
#endif
  unsigned char
    slot_id,
    crate_type,
    crate_id;


  d_printf("\n\r");
  d_printf("HW STATUS\n\r");
  d_printf("=========\n\r");

  slot_id = get_slot_id() ;
  if (slot_id == 0)
  {
    slot_id_text = "(Stand alone)" ;
  }
  else if (slot_id <= MAX_SLOT_ID_LINE_CARD)
  {
    slot_id_text = "(Probably line card)" ;
  }
  else if (slot_id <= MAX_SLOT_ID_FABRIC_CARD)
  {
    slot_id_text = "(Probably fabric card)" ;
  }

  d_printf(
    "Slot ID of this station   : %u %s"
    "\r\n",
    (unsigned short)slot_id,slot_id_text
    ) ;
  if (!slot_id)
  {
    goto exit;
  }


  /*
   * Crate_id and Crate_type (and host board specifiers) are only meaningful if slot_id is non-zero
   * (for line card or fabric card).
   */
  crate_id = get_crate_id() ;
  d_printf(
    "Crate ID of this station  : %u"
    "\r\n",
    (unsigned short)crate_id
    ) ;
  crate_type = get_crate_type() ;
  crate_type_text = get_crate_type_text((unsigned int)crate_type) ;
  d_printf(
    "Crate TYPE of this station: %u (%s)"
    "\r\n\n",
    (unsigned short)crate_type,crate_type_text
    ) ;
  err  =  host_board_cat_number_from_nv(host_board_cat_number);
  err |=  host_board_description_from_nv(host_board_description);
  err |=  host_board_version_from_nv(host_board_version);
  err |=  host_board_sn_from_nv(&host_board_sn);
  err |=  host_board_type_from_nv(&host_board_type);
  if (err)
  {
    d_printf(
      "Could not get board HW identifiers from host board serial EEPROM!\r\n"
    ) ;
  }
  else
  {
    d_printf(
      "Host Board\r\n"
      "==========\r\n"
      "Catalog number            : %s\r\n"
      "Description               : %s\r\n"
      "Version                   : %s\r\n"
      "Serial number             : %04u\r\n"
      "Type                      : %s\r\n",
      host_board_cat_number, host_board_description,
      host_board_version, host_board_sn,
      bsp_card_enum_to_str(host_board_type)
    );
  }

  d_printf("\n\r");

#if LINK_FAP21V_LIBRARIES
  if((gfa_fap21v_is_gfa_mb()))
  {
    err  =  host_db_board_cat_number_from_nv(host_db_board_cat_number);
    err |=  host_db_board_description_from_nv(host_db_board_description);
    err |=  host_db_board_version_from_nv(host_db_board_version);
    err |=  host_db_board_sn_from_nv(&host_db_board_sn);
    err |=  host_db_board_type_from_nv(&host_db_board_type);
    if (err)
    {
      d_printf(
        "Could not get board HW identifiers from host_db board serial EEPROM!\r\n"
        ) ;
    }
    else
    {
      d_printf(
        "Daughter Board\r\n"
        "==========\r\n"
        "Catalog number            : %s\r\n"
        "Description               : %s\r\n"
        "Version                   : %s\r\n"
        "Serial number             : %04u\r\n"
        "Type                      : %s\r\n",
        host_db_board_cat_number, host_db_board_description,
        host_db_board_version, host_db_board_sn,
        bsp_card_enum_to_str(host_db_board_type)
        );
    }
  }
#endif /*#if LINK_FAP21V_LIBRARIES*/

#if LINK_FAP20V_LIBRARIES

  if((gfa_is_gfa_mb()))
  {
    err  =  host_db_board_cat_number_from_nv(host_db_board_cat_number);
    err |=  host_db_board_description_from_nv(host_db_board_description);
    err |=  host_db_board_version_from_nv(host_db_board_version);
    err |=  host_db_board_sn_from_nv(&host_db_board_sn);
    err |=  host_db_board_type_from_nv(&host_db_board_type);
    if (err)
    {
      d_printf(
        "Could not get board HW identifiers from host_db board serial EEPROM!\r\n"
        ) ;
    }
    else
    {
      d_printf(
        "Daughter Board\r\n"
        "==========\r\n"
        "Catalog number            : %s\r\n"
        "Description               : %s\r\n"
        "Version                   : %s\r\n"
        "Serial number             : %04u\r\n"
        "Type                      : %s\r\n",
        host_db_board_cat_number, host_db_board_description,
        host_db_board_version, host_db_board_sn,
        bsp_card_enum_to_str(host_db_board_type)
        );
    }
  }
#endif /*LINK_FAP20V_LIBRARIES*/



exit:
  return;
}

/*************************************************************************
*
* starterProc - Do first action for demo project.
*
* Create various semaphores and spawn various tasks, while doing
* incredibly little error checking.
*
* RETURNS: OK
 */
STATUS
   starterProc(
 void
)
{
#if !DUNE_BCM
  STATUS
    status ;
#endif
  char
    *proc_name ;
  unsigned short
    host_board ;
#if !DUNE_BCM
  char
    dune_mem_device[MEM_DEVICE_NAME_LEN + 1] ;
  struct stat
    tmp_stat ;
#endif
  SOC_D_USR_APP_FLAVOR
    usr_app_flavor;
  unsigned int
    err_on_app,
    fs_version = BT_FS_VERSION_0,
    err;

  err = FALSE;
  err_on_app = FALSE;
  proc_name = "starterProc" ;
  usr_app_flavor = SOC_D_USR_APP_FLAVOR_NONE;

  d_app_state_set_state(D_APP_STATE_E_BOOT_FIN); /*Set-State that the boot is finished. App is started*/

#if !DUNE_BCM
  set_front_panel_led(FRONT_PANEL_RED_LED,     FRONT_PANEL_LED_OFF);
  {
    /*
     * Start clock. This is required for timers, according to spec.
     */
    struct timespec
      time_spec ;
    time_spec.tv_sec = 0 ;
    time_spec.tv_nsec = 0 ;
    status = clock_settime(CLOCK_REALTIME,&time_spec) ;
  }
#endif

  ref_design_print_hw_status();

  /*
   * Get started
   */

  test_d_printf("host_board_type_from_nv: %x\n\r", host_board);
  host_board_type_from_nv(&host_board);
  test_d_printf("host_board_type_from_nv: %x\n\r", host_board);

  /*
   * Stop task switching untill all initializations are OK.
   */
  taskLock() ;
  /*
   * kill the file that we were loaded from.
   */
#if !DUNE_BCM
  mem_device_name_from_nv(dune_mem_device);
  if ( !stat(dune_mem_device, &tmp_stat))
  {
    memDevDelete(dune_mem_device);
    free(get_mem_file_base());
  }

  /*
   * Update the FLASH file size, if needed.
   */
  update_flash_file_size();
#endif

#if !DUNE_BCM
  init_task_id() ;
#endif
  test_d_printf("skipping init_task_id\n\r");

  /*
   * Initialize General Utilities Module.
   */
#if !DUNE_BCM
  init_utils() ;
#endif
  test_d_printf("skipping init_utils\n\r");

  /*
   * Initialize FMC tramsport layer (by Dune).
   */
#if !DUNE_BCM
  init_dcl_d_transport() ;
  /*
   * Allocate background resources. Mainly, we allocate is a I2C semaphore.
   */
  background_allocate_resource();
#else
  I2c_bus_semaphore = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE) ;
#endif

#if !DUNE_BCM
compiler generates warning
  if(boot_get_fs_version)
#else
  if(1)
#endif
  {
    fs_version = boot_get_fs_version();
  }
  test_d_printf("boot_get_fs_version %d \n\r", fs_version);

  if(fs_version >= BT_FS_VERSION_1)
  {
    err =
      utils_dffs_cpu_init(FALSE);
    if(err)
    {
      d_printf("\n\rERROR: Failed to initialize Flash File System\n\r");
    }
    else
    {
      utils_dffs_cpu_dir(
        DFFS_SHORT_FORMAT,
        FALSE
      );
    }
  }

#if !DUNE_BCM
  utils_ip_init_info(FALSE);
#endif
  /*
   * if host board is presented we call this method that switches to the
   * right initialization according to board type.
   */
  init_host_board() ;
  test_d_printf("init_host_board\n\r");
  /*
   * Initialize User Interface Module.
   */
  init_ui_module() ;
  test_d_printf("init_ui_module\n\r");


  /*
   * Initialize thermometer access routines.
   */
#if !DUNE_BCM
  init_thermometer() ;
#endif
  test_d_printf("skipping init_thermometer\n\r");
  /*
   * Initialize Background Module.
   */
#if !DUNE_BCM
  init_bckg_module() ;
#endif
  test_d_printf("skipping init_bckg_module\n\r");

  /*
   * Initialize SNMP Module.
   */
#if INCLUDE_NET_SNMP
/* { */
  init_snmp_module() ;
/* } */
#endif
  /*
   * Add 'delete task hook'. Currently just for debugging.
   */
#if !DUNE_BCM
  status = taskDeleteHookAdd((FUNCPTR)task_delete_proc) ;
  gen_err(TRUE,TRUE,(int)status,ERROR,
        "starterProc - taskDeleteHookAdd",
                      proc_name,SVR_FTL,STARTERPROC_ERR_01,FALSE,0,-1,FALSE) ;
  /*
   * Make sure this routine is called every time a task is created.
   * Currently use it for debug only.
   */
  status = taskCreateHookAdd((FUNCPTR)task_create_proc) ;
  gen_err(TRUE,TRUE,(int)status,ERROR,
        "starterProc - taskCreateHookAdd",
                      proc_name,SVR_FTL,STARTERPROC_ERR_02,FALSE,0,-1,FALSE) ;
  {
    /*
     * set hook procedures for NVRAM (delay, poll)
     */
    par_nv_poll_init() ;
    /*
     * Initialize parallel NVRAM semaphore.
     */
    init_par_nv_sem() ;
  }
#endif

  errnoSet(0); /*Clear the Error*/
  gen_err(FALSE,TRUE,(int)0,0,
      "starterProc - STARTING THE APPLICATION...",
        proc_name,SVR_MSG,STARTERPROC_ERR_03,TRUE,0,0,FALSE) ;

#if !DUNE_BCM
  init_watchdog_module() ;
#endif
  test_d_printf("skipping init_watchdog_module\n\r");

#if !DUNE_BCM
  init_software_exception_module() ;
#endif
  test_d_printf("skipping init_software_exception_module\n\r");
  /*
   * Start task switching.
   */
  taskUnlock() ;




  /*
   * This is application activation.
   * If we are on line-card/fabric-card, and the value is not ZERO:
   * activate specific demo application.
   */
  d_usr_app_flavor_get(&usr_app_flavor) ;

  if (usr_app_flavor != SOC_D_USR_APP_FLAVOR_NONE)
  {
    init_high_level_local_app(usr_app_flavor, host_board, &err_on_app);
    d_printf(
      "\n\r"
      "Completed Application Start Up with %s.\n\r",
      (err_on_app)?"Failure":"Success"
      );

  }
  else
  {
    d_printf(
      "\n\r"
      "*********************************************************\n\r"
      "*********************************************************\n\r"
      "**** Application is not running on this system.          \n\r"
      "**** To run the application next time, type in the CLI:  \n\r"
      "**** 'nvram update both usr_app_flavor normal'           \n\r"
      "*********************************************************\n\r"
      "*********************************************************\n\r"
      "\n\r"
      "\n\r"
    );
  }

#if !DUNE_BCM
  ui_triger_page_mode();
  /*
   * Display CLI prompt.
   */
  display_cli_prompt() ;
#endif
#if !DUNE_BCM
  ui_proc();
#endif


  /*Set application run-tiem state*/
  if (usr_app_flavor)
  {
    if (err_on_app)
    {
      d_app_state_set_state(D_APP_STATE_E_APP_ERR);
    }
    else
    {
      d_app_state_set_state(D_APP_STATE_E_START_UP_APP_FIN);
    }
  }
  else
  {
    d_app_state_set_state(D_APP_STATE_E_START_UP_FIN_NO_APP);
  }

  /*
   * Leave this task cycling forever, taking very little
   * CPU time. Use to reconnect Ethernet in case cable was not inserted at startup.
   */

  return (OK) ;
}
/*************************************************************************
*
* progStart - start the sample program.
*
* Just invoke starterProc().
*
* RETURNS: OK
*/
STATUS
   progStart_negev(
     void
   )
{
#if !DUNE_BCM
  char
    *proc_name ;
  proc_name = "progStart" ;
  /*
   * Spawn all tasks
   */
  taskSpawn(
      "stProcTask", 50, 0, STARTER_TASK_STACK_SIZE,(FUNCPTR)starterProc,
      0,0,0,0,0,0,0,0,0,0);
  /*
   * Leave this task cycling forever, taking very little
   * CPU time. Use to reconnect Ethernet in case cable was not inserted at startup.
   */
  cli_out("progStart: Reduce priority of progStartTask\n") ;
  taskPrioritySet(d_taskIdSelf(),ROOT_TASK_PRIORITY) ;
  while (TRUE)
  {
    appLedBarInvert(LED3,MULTI_LED) ;
    sal_usleep(100) ;
    /*
     * The following code has not yet been properly tested.
     */
    /*
     * Keep checking whether Ethernet has been connected at startup and,
     * if not, attach it.
     * {
     */
    if (has_usrEndLibInit_failed())
    {
      /*
       * Enter if Ethernet connection has not been successfully terminated.
       */
      usrEndLibInit() ;
      if (!has_usrEndLibInit_failed())
      {
        d_printf("\r\n"
            "Ethernet reconnected... Hit carriage return to get prompt.\r\n") ;
        gen_err(FALSE,TRUE,(int)0,0,
            "Seems Ethernet connection is OK now...",
          proc_name,SVR_MSG,PROGSTART_ERR_01,TRUE,0,0,FALSE) ;
      }
    }
    /*
     * }
     */
  }

#endif
  return (OK) ;
}


/*****************************************************
*NAME
*  should_additional_memory_be_added
*TYPE: PROC
*DATE: 20/MAR/2007
*FUNCTION:
*  Checks if GT64131 additional memory should be added.
*  Returns true if running under the PMC that do not have 10M
*INPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    None
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int
*       true if additional memory should be added
*  SOC_SAND_INDIRECT:
*
*REMARKS:
*  None
*SEE ALSO:
* None
 */
unsigned int
  should_additional_memory_be_added()
{
  unsigned
    int
      rtn = FALSE;
  unsigned
    short
      card_type = SOC_UNKNOWN_CARD;


  if (get_epld_board_id() == (unsigned char)NEW_MEZZANINE_WITH_PCI)
  {
    /*
     * Get the host card type from NvRam.
     */
    host_board_type_from_nv(&card_type) ;

    /*
     * check if 10M
     */
    if (
        (bsp_card_is_same_family(LINE_CARD_01, card_type) == TRUE)
       )
    {
      rtn = FALSE;
    }
    else
    {
      rtn = TRUE;
    }
  }
  else /* not NEW_MEZZANINE_WITH_PCI */
  {
    rtn = FALSE;
  }

  return rtn;
}

/*****************************************************
*NAME
*  add_additional_memory_unsafe
*TYPE: PROC
*DATE: 28/FEB/2007
*FUNCTION:
*   Add extra 32MB of memory to system (GT64131 SDRAM)
*       a. Add to pool
*       b. Enable cache
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int
*     verbose
*       If true, details are printed
*  SOC_SAND_INDIRECT:
*    None
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int
*       error code
*  SOC_SAND_INDIRECT:
*
*REMARKS:
*  None
*SEE ALSO:
* None
 */
unsigned int
  add_additional_memory_unsafe(
      unsigned int verbose
   )
{
#if !DUNE_BCM
  unsigned long
    mem_base = GT64131_SDRAM_BASE_ADDR,
    mem_size = GT64131_SDRAM_SIZE;
  STATUS
    status = OK;




  /* Enable cache in new extra 32 MB of memory */
  if (verbose)
  {
    cli_out("Adding extra 32 MB of memory (GT64131 SDRAM)\n\r");
    cli_out("---->Enabling cache\n\r");
  }
  status = vmBaseStateSet (NULL, (char *)mem_base,
                      ((VM_PAGE_SIZE + mem_size)/VM_PAGE_SIZE) * VM_PAGE_SIZE,
                      VM_STATE_MASK_CACHEABLE,
                      VM_STATE_CACHEABLE);
  if (status!=OK)
  {
    if (verbose)
    {
      cli_out("---->Error - cannot add to pool\n\r");
    }
    goto exit;

  }


  /* Add additional 32MB of memory to pool */
  if (verbose)
  {
    cli_out("---->Adding memory to pool\n\r");
  }
  memAddToPool(
    (char *)mem_base,         /*0x10000000*/
    mem_size                  /*0x02000000*/
  );


exit:

  return status;
#else
  return OK;
#endif
}

/*****************************************************
*NAME
*  add_additional_memory
*TYPE: PROC
*DATE: 28/FEB/2007
*FUNCTION:
*   Checks if not under 10M and add extra memory to system (GT64131 SDRAM)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int
*     verbose
*       If true, details are printed
*  SOC_SAND_INDIRECT:
*    None
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int
*       error code
*  SOC_SAND_INDIRECT:
*
*REMARKS:
*  None
*SEE ALSO:
* None
 */
unsigned int
  add_additional_memory(
      unsigned int verbose
   )
{
  STATUS
    status = OK;

  if (should_additional_memory_be_added()==TRUE)
  {
    status = add_additional_memory_unsafe(verbose);
  }

  return status;
}

