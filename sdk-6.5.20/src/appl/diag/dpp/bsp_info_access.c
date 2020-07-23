/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


/*************
 * INCLUDES  *
 */

#if !DUNE_BCM
#include "bsp_cards_consts.h"
#include "bsp_info_access.h"
#include "ref_sys.h"
#else
#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dpp/bsp_info_access.h>
#include <appl/diag/dpp/utils_defx.h>
#endif


/*************
 * DEFINES   *
 */

/*************
 * TYPEDEFS  *
 */

/*************
 *  GLOBALS  *
 */


static const unsigned long Bsp_fe200_base_addr[SOC_BSP_MAX_NUM_FES_ON_BRD] =
{
  FE_1_BASE_ADDR,
  FE_2_BASE_ADDR
};

/*************
 * FUNCTIONS *
 */

/*
 * gets the requested FE base address, according to its index (fe_idx).
 * the index starts with 0.
 */
unsigned long bsp_get_fe_base_addr(unsigned int fe_idx)
{
  unsigned long fe_addr;

  fe_addr = Bsp_fe200_base_addr[fe_idx];

  return fe_addr;
}

