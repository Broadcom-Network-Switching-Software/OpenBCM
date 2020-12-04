/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/******************************************************************
*
* FILENAME:       ref_design_petra_starter.c
*
* MODULE PREFIX:  AUTO_CODER:TODO
*
* SYSTEM:           Dune Semiconductors LTD Proprietary information
*
* CREATION DATE:  aa/aa/aa
*
* LAST CHANGED:   aa/aa/aa
*
* REVISION:       $2.650$
*
* FILE DESCRIPTION:
*
* REMARKS:
 */

/*************
 * INCLUDES  *
 */
/* { */

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#include <appl/diag/dpp/ref_design_starter.h>
#include <appl/diag/dpp/ref_design_petra_starter.h>

#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_error_defs.h>
#include <appl/diag/dpp/utils_host_board.h>
#include <appl/diag/dpp/ui_defx.h>

#define DUNE_SWEEP 1

#if !DUNE_BCM

#include "Pub/include/utils_defx.h"
#include "Pub/include/ui_defx.h"

#include "../include/dune_remote_access.h"
#include "../include/dune_rpc_auto_network_learning.h"
#include "../include/ref_design_petra_starter.h"

#include "sweep/Petra/include/swp_p_tm_info.h"
#include "sweep/Petra/include/swp_p_tm_api_init.h"
#include "sweep/Petra/include/swp_p_tm_app.h"

#include "Utilities/include/utils_line_PTG.h"

#include "TempWinLib/chassis.h"

#endif

#define test_d_printf d_printf

/* } */

/*************
 * DEFINES   *
 */
/* { */


/* } */

/*************
 *  MACROS   *
 */
/* { */


#define REF_DESIGN_PETRA_SAVE_ERROR_TO_NVRAM_NO_EXIT(err_ndx, proc_name)                              \
            gen_err(TRUE, FALSE, 1, 0, #err_ndx, proc_name, SVR_ERR, err_ndx, TRUE, 0, -1, FALSE); \

#define REF_DESIGN_PETRA_SAVE_ERROR_TO_NVRAM_AND_EXIT(err_ndx, proc_name)                             \
            gen_err(TRUE, FALSE, 1, 0, #err_ndx, proc_name, SVR_ERR, err_ndx, TRUE, 0, -1, FALSE); \
            goto exit;                                                                             \

#define REF_DESIGN_PETRA_INIT_ERROR_DEFINITIONS                                                       \
          uint32                                                                                 \
            ex,                                                                                    \
            no_err;                                                                                \
          const char                                                                               \
            *err_message = NULL;                                                                   \
          soc_sand_initialize_error_word(0, 0, &ex);                                                   \
          no_err = ex;

#define REF_DESIGN_PETRA_CHECK_FUNC_RESULT(f_res,err_msg,err_exit_label)                              \
          soc_sand_set_error_code_into_error_word(                                                     \
            soc_sand_get_error_code_from_error_word(f_res),                                            \
            &ex                                                                                    \
          );                                                                                       \
          if(ex != no_err)                                                                         \
          {                                                                                        \
            err_message = err_msg;                                                                 \
            goto err_exit_label;                                                                   \
          }

#define REF_DESIGN_PETRA_EXIT_AND_PRINT_ERROR(error_name,var_a,var_b)                                 \
          if (ex != no_err)                                                                        \
          {                                                                                        \
            if (!silent)                                                                           \
            {                                                                                      \
              cli_out(\
                      "Error in %s: %s", error_name, err_message                                         \
                      );                                                                                   \
            }                                                                                      \
          }                                                                                        \
          return ex;


#define REF_DESIGN_PETRA_CHECK_NULL_INPUT(p_input)                                                    \
          if(NULL == p_input)                                                                      \
          {                                                                                        \
            err_message = "Null pointer error";                                                    \
            goto exit;                                                                             \
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

#if LINK_PETRA_LIBRARIES

uint32
  ref_design_petra_remote_access_system_init(
    SOC_SAND_IN  uint8  silent
  )
{
  uint32
    num_units,
    unit_i;

  REF_DESIGN_PETRA_INIT_ERROR_DEFINITIONS;

  num_units = dune_rpc_get_number_of_fabric_cards() + dune_rpc_get_number_of_line_cards();

  if(0 != dune_rpc_set_number_of_units(num_units))
  {
    REF_DESIGN_PETRA_SAVE_ERROR_TO_NVRAM_AND_EXIT(SWEEP_PETRA_APP_INIT_ERR, "sweep_init_remote_access_system learning - Error allocating memory")
  }

  for (unit_i = 0 ; unit_i < num_units ; ++unit_i)
  {
#if !DUNE_BCM
    if(unit_i < dune_rpc_get_number_of_line_cards())
    {
      dune_rpc_set_remote_device_and_unit(
        unit_i,
        unit_i | SOC_SAND_BIT(30),
        dune_rpc_get_ip_of_line_card_at(unit_i)
      );
    }
    else /* fabric cards */
    {
      dune_rpc_set_remote_device_and_unit(
        unit_i,
        (2*unit_i)   | SOC_SAND_BIT(31),
        dune_rpc_get_ip_of_fabric_card_at(unit_i - dune_rpc_get_number_of_line_cards()));
      dune_rpc_set_remote_device_and_unit(
        unit_i,
        (2*unit_i+1) | SOC_SAND_BIT(31),
        dune_rpc_get_ip_of_fabric_card_at(unit_i - dune_rpc_get_number_of_line_cards()));
    }
#endif
  }

exit:
  REF_DESIGN_PETRA_EXIT_AND_PRINT_ERROR( "error in ref_design_petra_remote_access_system_init()",0,0);
}

uint32
  ref_design_petra_fap_id_from_slot_id_get(
    SOC_SAND_IN  uint32  slot_id,
    SOC_SAND_OUT uint32  *fap_id,
    SOC_SAND_IN  uint8  silent
  )
{
  REF_DESIGN_PETRA_INIT_ERROR_DEFINITIONS;

  REF_DESIGN_PETRA_CHECK_NULL_INPUT(fap_id);

  *fap_id = slot_id - 1;

exit:
  REF_DESIGN_PETRA_EXIT_AND_PRINT_ERROR( "error in ref_design_petra_fap_id_from_slot_id_get()",0,0);
}

uint32
  ref_design_petra_params_for_app_flavor_update(
    SOC_SAND_OUT uint32  *max_nof_faps_in_system,
    SOC_SAND_OUT uint32  *fap_id,
    SOC_SAND_IN  uint8  silent
  )
{
  SOC_D_USR_APP_FLAVOR
    usr_app_flavor;

  REF_DESIGN_PETRA_INIT_ERROR_DEFINITIONS;

  REF_DESIGN_PETRA_CHECK_NULL_INPUT(max_nof_faps_in_system);
  REF_DESIGN_PETRA_CHECK_NULL_INPUT(fap_id);

  d_usr_app_flavor_get(&usr_app_flavor);

  if(usr_app_flavor == SOC_D_USR_APP_FLAVOR_WIN_01)
  {
    *max_nof_faps_in_system = 2;
    *fap_id                 = 0;
  }

exit:
  REF_DESIGN_PETRA_EXIT_AND_PRINT_ERROR( "error in ref_design_petra_params_for_app_flavor_update()",0,0);
}

uint32
  ref_design_petra_app_init(
    SOC_SAND_IN  uint8    crate_type,
    SOC_SAND_IN  SOC_SAND_DEVICE_TYPE  dev_type,
    SOC_SAND_IN  uint32  nof_fabric_cards,
    SOC_SAND_IN  uint8  is_hot_start,
    SOC_SAND_OUT uint32  *p_unit,
    SOC_SAND_IN  uint8  silent
  )
{
  uint32
    res;
#if DUNE_SWEEP
  uint32
    base_address=0;
#endif
  uint32
    unit = 0,
    fap_id,
    max_nof_faps_in_system = 0;
#if !DUNE_BCM
  uint32
    time_to_wait = 0;
#endif
  uint8
    fe_ready;
  unsigned short
    board_type;

  REF_DESIGN_PETRA_INIT_ERROR_DEFINITIONS;

  REF_DESIGN_PETRA_CHECK_NULL_INPUT(p_unit);

#if REF_DESIGN_PETRA_PRINT_MORE
  cli_out("System starts with %d fabric cards\r\n", nof_fabric_cards);
#endif


  host_board_type_from_nv(&board_type);
  if((board_type == LOAD_BOARD_PB) || (board_type == LOAD_BOARD_FE600))
  {
    fe_ready = TRUE;
    max_nof_faps_in_system = 2;
  }
  else
  {
    fe_ready = dune_rpc_is_fe_ready(TRUE);
    cli_out("\n\r") ;

    if (!fe_ready)
    {
      cli_out("FE has not finished loading yet. \n\r") ;
    }

    /*
     * Set the max number of FAPs for the sweep all-to-all application
     * according to the chassis
     */
    if( utils_is_crate_negev(crate_type) )
    {
      max_nof_faps_in_system = 2;
    }
    else if( utils_is_crate_gobi(crate_type) )
    {
      max_nof_faps_in_system = 16;
    }
    else
    {
      REF_DESIGN_PETRA_SAVE_ERROR_TO_NVRAM_NO_EXIT(SWEEP_PETRA_APP_INIT_ERR, "Unknown crate_type - %d..\n\r");
    }
  }
  res = ref_design_petra_remote_access_system_init(
          silent
        );
  REF_DESIGN_PETRA_CHECK_FUNC_RESULT(res, "", exit);
  res = ref_design_petra_fap_id_from_slot_id_get(
          get_slot_id(),
          &fap_id,
          silent
        );
  REF_DESIGN_PETRA_CHECK_FUNC_RESULT(res, "", exit);
   res = ref_design_petra_params_for_app_flavor_update(
          &max_nof_faps_in_system,
          &fap_id,
          silent
        );
  REF_DESIGN_PETRA_CHECK_FUNC_RESULT(res, "", exit);
  res = swp_p_tm_bsp_base_address(
		      0,
          &base_address
        );
  REF_DESIGN_PETRA_CHECK_FUNC_RESULT(res, "", exit);
    cli_out("\n"
            "Starting Application..."
            "\n"
            );

 /*
 */
  if (tevb_is_timna_connected())
  {
    d_printf(
        "Detected Timna. Run Do nothing.\n\r");
    goto exit;
  }
  else
  {
    d_printf(
      "Run Soc_petra.\n\r");
  }

  /*
  *	Initialize to 0. This is only relevant if
  *  the sweep application fails
  */
  soc_petra_set_default_unit(0);

#if DUNE_SWEEP
  test_d_printf("call swp_p_tm_app\n\r");
  res = swp_p_tm_app(
          fap_id,
          dev_type,
          base_address,
          is_hot_start,
          &unit,
          FALSE
        );
  REF_DESIGN_PETRA_CHECK_FUNC_RESULT(res, "", exit);
#endif


  /*
   *	Set the actual device id as returned by the sweep
   */
  soc_petra_set_default_unit(unit);
  test_d_printf("swp_p_tm_app unit %d \n\r",unit);

  *p_unit = unit;

  

exit:
  REF_DESIGN_PETRA_EXIT_AND_PRINT_ERROR( "error in ref_design_petra_app_init()",0,0);
}


uint32
  ref_design_petra_device_info_get(
    SOC_SAND_OUT  uint32           *fap_id,
    SOC_SAND_OUT  uint32            *device_base_address
  )
{
  uint32
    res;
  uint32
    /* unit = 0, */
    max_nof_faps_in_system = 0;
#if !DUNE_BCM
  uint32
    time_to_wait = 0;
#endif
  uint8  
    silent=TRUE;

  REF_DESIGN_PETRA_INIT_ERROR_DEFINITIONS;

  REF_DESIGN_PETRA_CHECK_NULL_INPUT(fap_id);
  REF_DESIGN_PETRA_CHECK_NULL_INPUT(device_base_address);

  res = ref_design_petra_fap_id_from_slot_id_get(
          get_slot_id(),
          fap_id,
          silent
        );
  REF_DESIGN_PETRA_CHECK_FUNC_RESULT(res, "", exit);
   res = ref_design_petra_params_for_app_flavor_update(
          &max_nof_faps_in_system,
          fap_id,
          silent
        );
  REF_DESIGN_PETRA_CHECK_FUNC_RESULT(res, "", exit);
  res = swp_p_tm_bsp_base_address(
		      0,
          device_base_address
        );
  REF_DESIGN_PETRA_CHECK_FUNC_RESULT(res, "", exit);

exit:
  REF_DESIGN_PETRA_EXIT_AND_PRINT_ERROR( "error in ref_design_petra_device_info_get()",0,0);
}

#endif /*  LINK_PETRA_LIBRARIES */

/* } */

