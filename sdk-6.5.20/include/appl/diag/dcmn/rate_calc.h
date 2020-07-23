/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        rate_calc.h
 * Purpose:     TBD
 */

#ifndef _RATE_CALC_H
#define _RATE_CALC_H

/* #include <soc/drv.h> */

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/fabric.h>
#include <appl/diag/diag.h>
#include <appl/diag/diag_pp.h>
#include <appl/diag/diag_field.h>
#include <sal/appl/sal.h>
#include <appl/diag/dcmn/diag.h>
#include <appl/diag/system.h>
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/arad_api_diagnostics.h>
#include <soc/dpp/ARAD/arad_debug.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/port_sw_db.h>
#endif
#include <soc/defs.h>
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/TMC/tmc_api_statistics.h>
#include <bcm_int/petra_dispatch.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/cosq.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/error.h>
#endif
extern int calc_pqp_rate(int unit, int local_port_id, int tc_indx);
extern int calc_epe_port_rate(int unit, int local_port_id, int tc_indx);
extern int calc_epni_rate(int unit, int check_bw_scheme, int check_bw_num);
extern int calc_epe_if_rate(int unit, int if_num);
extern int calc_egq_rate(int unit, int local_port_id, int tc_indx);
extern int calc_ire_rate(int unit, int local_port_id);
extern int calc_iqm_rate(int unit, int local_port_id);
extern int calc_cgm_rate(int unit);
extern int calc_ipt_rate(int unit);
extern int calc_pts_rate(int unit);
extern int calc_txq_rate(int unit);
extern int calc_sch_ps_rate(int unit, int local_port_id);
extern int calc_sch_flow_bw(int unit, int flow_id);
extern int gtimer_enable(int unit, char* block_name, int period_num);
extern int gtimer_trigger(int unit, char* block_name);
extern int gtimer_stop(int unit, char* block_name);
#endif
