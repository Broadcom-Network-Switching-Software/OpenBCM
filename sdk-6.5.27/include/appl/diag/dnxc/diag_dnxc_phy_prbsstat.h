/*! \file diag_dnxc_phy_prbsstat.h
 * Purpose: Extern declarations for "phy prbsstat" diag commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNXC_PHY_PRBSSTAT_INCLUDED
#define DIAG_DNXC_PHY_PRBSSTAT_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <appl/diag/sand/diag_sand_framework.h>
/*************
 *  DEFINES  *
 *************/
#define DIAG_DNXC_PHY_PRBSSTAT_DEINIT (1)
#define DIAG_DNXC_PHY_PRBSSTAT_CMD    (0)

/*************
 *  MACROES  *
 *************/

/*************
 * GLOBALS   *
 *************/
extern sh_sand_man_t dnxc_phy_prbsstat_man;
extern sh_sand_cmd_t sh_dnxc_phy_prbsst_cmds[];

 /*************
  * FUNCTIONS *
  *************/
/**
 * \brief - Stop PRBSStat thread
 *
 * \param [in] unit - unit #
 * \param [in] is_deinit - should be DIAG_DNXC_PHY_PRBSSTAT_DEINIT if called during deinit and 
 * DIAG_DNXC_PHY_PRBSSTAT_CMD if called from a shell command
 * \return
 * \see
 *   * None
 */
shr_error_e diag_dnxc_phy_prbs_stat_stop(
    int unit,
    int is_deinit);

#endif /* DIAG_DNXC_PHY_PRBSSTAT_INCLUDED */
