/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __REF_DESIGN_STARTER_H_INCLUDED__
/* { */
#define __REF_DESIGN_STARTER_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

/*************
 * INCLUDES  *
 */
/* { */

/* } */

/*************
 * DEFINES   *
 */
/* { */

#if !DUNE_BCM
need to make these defines dynamic (hardcode for now)
#else
#define dune_rpc_are_all_line_type_x(x) (FALSE)
#define dune_rpc_are_all_line_type_x_family(x) ((x==LINE_CARD_GFA_PETRA_X) ? TRUE : FALSE)
#define dune_rpc_auto_learn_network(x) (x)
#define dune_rpc_get_is_auto_learning_valid() (TRUE)
#define dune_rpc_is_usr_app_flavor_in_the_system_compatible(x) (x)
#define dune_rpc_get_number_of_fabric_cards() (0)

#define dune_rpc_get_number_of_line_cards() (1)
#define dune_rpc_set_number_of_units(x) (0)
#define dune_rpc_is_fe_ready(x) (x)

#define tevb_is_timna_connected() (0)
#endif

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

void
  ref_design_print_hw_status(
    void
  );

/* } */

#ifdef  __cplusplus
}
#endif

/* } __REF_DESIGN_STARTER_H_INCLUDED__*/
#endif
