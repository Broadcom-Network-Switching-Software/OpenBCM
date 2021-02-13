/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: Externization of all DPI functions needed by TSCMod.
 *---------------------------------------------------------------------*/

/*!
\file tscmod_phyreg.h Declares verilog interface functions.
This file contains declarations of functions that are necessary to 
hook up TSCMod to various verilog platforms. The drivers of TSCMod are
hooked to verilog tasks as a first level interface to the actual drivers in the 
verilog testbench. These 'C' functions and verilog tasks are attached via 
a mechanism called 'DPI' (direct programming interface) supported by all
verilog simulators.  Note that C platforms need no special attachments.
*/

#ifndef   _TSCMOD_PHYREG_H_
#define   _TSCMOD_PHYREG_H_

#include "tscmod_main.h"  
#include "tscmod_defines.h"

#if defined (_MDK_TSCMOD_)
extern int cdk_xgs_miim_read(int u, uint32_t pa, uint32_t ra, uint32_t *val);
extern int cdk_xgs_miim_write(int u, uint32_t pa, uint32_t ra, uint32_t val);
#endif

#ifdef _DV_TRIDENT2
/* normally the externs are generated by irun/vcs -dpiheader. */
/* Manually typing this in to reduce header files. */
#ifdef CADENCE
/* svdpi.h to be conditional to cadence (or irun). vcs does not have this. */
#include "svdpi.h"
#endif /* CADENCE */
#include "svdpi.h"

extern void dv_reg_cl45_read  (int ut,int pt,int pa,int ln,int ad,int *dt);
extern void dv_reg_cl22_read  (int ut,int pt,int pa,int ln,int ad,int *dt);

extern void dv_reg_cl45_write (int ut,int pt,int pa,int ln,int ad,int dt);
extern void dv_reg_cl22_write (int ut,int pt,int pa,int ln,int ad,int dt);

extern void dv_reg_cl45_modify(int ut,int pt,int pa,int ln,int ad,int dt,int m);
extern void dv_reg_cl22_modify(int ut,int pt,int pa,int ln,int ad,int dt,int m);

extern void dv_wait (int delay);

extern void dv_slam_read_reg  (int core, int addr, int *data);
extern void dv_slam_write_reg (int core, int addr, int  data);
extern void dv_slam_modify_reg(int core, int addr, int  data, int m);

#endif /* _DV_TRIDENT2 */

extern int  tscmod_reg_aer_read  (uint32 u,tscmod_st* w,uint32 a,uint16 *d);
extern int  tscmod_reg_aer_write (uint32 u,tscmod_st* w,uint32 a,uint16  d);
extern int  tscmod_reg_aer_modify(uint32 u,tscmod_st* w,uint32 a,uint16 d,uint16 m);

extern int  tscmod_cl22_read  (tscmod_st* ws,uint32 addr,uint16 *data);
extern int  tscmod_cl22_write (tscmod_st* ws,uint32 addr,uint16 data);
extern int  tscmod_cl45_read  (tscmod_st* ws,uint32 addr,uint16 *data);
extern int  tscmod_cl45_write (tscmod_st* ws,uint32 addr,uint16 data);
extern void tscmod_timeout_init(soc_timeout_t *to, uint32 usec, int min_polls);  
extern int  tscmod_timeout_check(tscmod_st* ws,soc_timeout_t *to);  
extern int  tscmod_regbit_set_wait_check(tscmod_st* w,int r,int b,int bt,int to); 
#endif   /*  _TSCMOD_PHYREG_H_ */
