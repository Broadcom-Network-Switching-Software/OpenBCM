/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        diag.h
 * Purpose:     Device diagnostics commands.
 */


#ifndef _GPORT_H_INCLUDED_
#define _GPORT_H_INCLUDED_

#include <soc/chip.h>

#define DPP_CMD_COSQ_GPORT_SHOW_ALL 2
#define DPP_CMD_COSQ_GPORT_COUNT    3
/* Future cmds could be remove, remove all */

typedef struct gport_count_s {
      int nof_conn;
      int nof_uc_qs;
      int nof_mc_qs;
      int nof_cl_ses;
      int nof_fq_ses;
      int nof_hr_ses;
} gport_count_t;

typedef struct dpp_gport_cb_params_s {
  int cmd;
  int verbose;
  int modid;
  int port;
  gport_count_t gport_count;
} dpp_gport_cb_params_t;


#endif /*_GPORT_H_INCLUDED_*/
