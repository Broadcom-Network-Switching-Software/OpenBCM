/***********************************************************************************
 ***********************************************************************************
 *  File Name     :  merlin_sesto_functions.h                                        *
 *  Created On    :  29/04/2013                                                    *
 *  Created By    :  Kiran Divakar                                                 *
 *  Description   :  Header file with API functions for Serdes IPs                 *
 *  Revision      :  $Id: merlin_sesto_functions.h 844 2015-01-12 20:58:16Z kirand $ *
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                           *
 *  No portions of this material may be reproduced in any form without             *
 *  the written permission of:                                                     *
 *      Broadcom Corporation                                                       *
 *      5300 California Avenue                                                     *
 *      Irvine, CA  92617                                                          *
 *                                                                                 *
 *  All information contained in this document is Broadcom Corporation             *
 *  company private proprietary, and trade secret.                                 *
 */

/** @file merlin_sesto_functions.h
 * Protoypes of all API functions for engineering use 
 */

#ifndef MERLIN_SESTO_API_FUNCTIONS_H
#define MERLIN_SESTO_API_FUNCTIONS_H

/* include all .h files, even though some are redundant */

#include "merlin_sesto_usr_includes.h"
#include "merlin_sesto_ipconfig.h"
#include "merlin_sesto_dependencies.h"
#include "merlin_sesto_interface.h"
#include "merlin_sesto_debug_functions.h"
#include "merlin_sesto_common.h"
#include "eagle_merlin_api_uc_common.h"
#include "../common/srds_api_uc_common.h"

#include "merlin_sesto_field_access.h"
#include "merlin_sesto_enum.h"
#include "../common/srds_api_err_code.h"

#define _error(args) args


/* declare __ERR macro for register read and RAM variable read access macros */
#define __ERR &__err

/* RAM access */
#include "eagle_merlin_api_uc_vars_rdwr_defns.h"

/* Reg access */

      #include "merlin_sesto_fields.h"



#define DIAG_MAX_SAMPLES (32)



/**********************************************/
/* Macros to simplify checking for error_code */
/**********************************************/

/** Macro to invoke a function with automatic return of error */
#define EFUN(fun) do {err_code_t __err = (fun); if (__err) return(__err); } while(0)

/** Macro to execute a statement with automatic return of error */
#define ESTM(statement) do {err_code_t __err = ERR_CODE_NONE; statement; if (__err) return(__err); } while(0)

/** Macro to invoke a function with automatic return of error pointer */
#define EPFUN(fun) do { *err_code_p |= (fun); if (*err_code_p) return(0); } while(0)

/** Macro to execute a statement with automatic return of error pointer */
#define EPSTM(statement) do {err_code_t __err = ERR_CODE_NONE; statement; *err_code_p |= __err; if (*err_code_p) return(0); } while(0)

/*----------------------*/
/*  Misc Useful macros  */
/*----------------------*/
/** Macro to display signed integer variable */
#define DISP(x) ESTM(USR_PRINTF(("%s = %d\n",#x,x)))

/** Macro to display unsigned integer variable */
#define DISPU(x) ESTM(USR_PRINTF(("%s = %u\n",#x,x)))

/** Macro to display floating point variable */
#define DISPF(x) ESTM(USR_PRINTF(("%s = %f\n",#x,x)))

/** Macro to display integer variable in hex */
#define DISPX(x) ESTM(USR_PRINTF(("%s = %x\n",#x,x)))

/** Macro to read and display value of lane register field in decimal*/
#define DISP_REG(x) ESTM(USR_PRINTF(("%s = %d\n",#x,rd_ ## x ## ())))

/** Macro to read and display value of lane register field in hex*/
#define DISP_REGX(x) ESTM(USR_PRINTF(("%s = %x\n",#x,rd_ ## x ## ())))

/** Macro to read and display value of core register field in hex*/
#define DISP_REGC(x) ESTM(USR_PRINTF(("%s = %x\n",#x,rdc_ ## x ## ())))

/** Macro to find clockwise distance between phase counters */
#define dist_cw(a,b) (((a)<=(b))?((b)-(a)):((uint16_t)256-(a)+(b)))

/** Macro to find counter clockwise distance between phase counters */
#define dist_ccw(a,b) (((a)>=(b))?((a)-(b)):((uint16_t)256+(a)-(b)))

/** Macro to display single variable stored in lane struct */
#define DISP_LN_VARS(name,param,format) USR_PRINTF(("%-16s\t",name)); for(i=0;i<num_lanes;i++) { \
    USR_PRINTF((format,(lane_st[i].param))); } USR_PRINTF(("\n")); 

/** Macro to display two variables stored in lane struct */
#define DISP_LNQ_VARS(name,param1,param2,format) USR_PRINTF(("%-16s\t",name));for(i=0;i<num_lanes;i++) { \
    USR_PRINTF((format,(lane_st[i].param1),(lane_st[i].param2)));} USR_PRINTF(("\n")); 

/** Macro to find smaller between two variables */ 
#define _min(a,b) (((a)>(b)) ? (b) : (a))

/** Macro to find larger between two variables */ 
#define _max(a,b) (((a)>(b)) ? (a) : (b))

/** Macro to extract absolute value */ 
#define _abs(a) (((a)>0) ? (a) : (-(a)))

/*------------------------------*/
/** Serdes OSR Mode Structure   */
/*------------------------------*/
typedef struct {
    /** TX OSR Mode */
    uint8_t tx;
    /** RX OSR Mode */
    uint8_t rx;
    /** OSR Mode for TX and RX (used when both TX and RX should have same OSR Mode) */
    uint8_t tx_rx;
}merlin_sesto_osr_mode_define_st;

/*------------------------------*/
/** Serdes Lane State Structure */
/*------------------------------*/
typedef struct {
       /** uC lane configuration */
       uint16_t ucv_config;
       /** Frequency offset of local reference clock with respect to RX data in ppm */
       int16_t rx_ppm;
       /** Vertical threshold voltage of p1 slicer (mV) */
       int16_t p1_lvl;
       /** Vertical threshold voltage of m1 slicer (mV) */
       int16_t m1_lvl;
       /** Link time in milliseconds */
       uint16_t link_time;
       /** OSR Mode */
       merlin_sesto_osr_mode_define_st osr_mode;
       /** Signal Detect */
       uint8_t sig_det;
       /** PMD RX Lock */
       uint8_t rx_lock;
       /** Delay of zero crossing slicer, m1, wrt to data in PI codes */
       int8_t clk90;
       /** Delay of diagnostic/lms slicer, p1, wrt to data in PI codes */
       int8_t clkp1;
       /** Peaking Filter Main Settings */
       int8_t pf_main;
       /** Peaking Filter Hiz mode enable */
       int8_t pf_hiz;
       /** Peaking Filter DC gain adjustment for CTLE */
       int8_t pf_bst;
       /** Low Frequency Peaking filter control */
       int8_t pf2_ctrl;
       /** Variable Gain Amplifier settings */
       int8_t vga;
       /** DC offset DAC control value */
       int8_t dc_offset;
       /** P1 eyediag status */
       int8_t p1_lvl_ctrl;
       /** DFE tap 1 value */
       int8_t dfe1;
       /** DFE tap 2 value */
       int8_t dfe2;
       /** DFE tap 3 value */
       int8_t dfe3;
       /** DFE tap 4 value */
       int8_t dfe4;
       /** DFE tap 5 value */
       int8_t dfe5;
       /** DFE tap 6 value */
       int8_t dfe6;
       /** DFE tap 1 Duty Cycle Distortion */
       int8_t dfe1_dcd;
       /** DFE tap 2 Duty Cycle Distortion */
       int8_t dfe2_dcd;
       /** Slicer calibration control codes (p1 even) */
       int8_t pe;
       /** Slicer calibration control codes (data even) */
       int8_t ze;
       /** Slicer calibration control codes (m1 even) */
       int8_t me;
       /** Slicer calibration control codes (p1 odd) */
       int8_t po;
       /** Slicer calibration control codes (data odd) */
       int8_t zo;
       /** Slicer calibration control codes (m1 odd) */
       int8_t mo;
       /** Frequency offset of local reference clock with respect to TX data in ppm */
       int16_t tx_ppm;
       /** TX equalization FIR pre tap weight */
       int8_t txfir_pre;
       /** TX equalization FIR main tap weight */
       int8_t txfir_main;
       /** TX equalization FIR post1 tap weight */
       int8_t txfir_post1;
       /** TX equalization FIR post2 tap weight */
       int8_t txfir_post2;
       /** TX equalization FIR post3 tap weight */
       int8_t txfir_post3;
       /** Horizontal left eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
       uint16_t heye_left;
       /** Horizontal right eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
       uint16_t heye_right;
       /** Vertical upper eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
       uint16_t veye_upper;
       /** Vertical lower eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
       uint16_t veye_lower;
       /** Baud Rate Phase Detector enable */
       uint8_t br_pd_en;
} merlin_sesto_lane_state_define_st;

err_code_t _merlin_sesto_read_lane_state_define(const phymod_access_t *pa, merlin_sesto_lane_state_define_st *istate);

err_code_t _merlin_sesto_eye_margin_est_get(const phymod_access_t *pa,int *left_eye_mUI, int *right_eye_mUI, int *upper_eye_mV, int *lower_eye_mV);
#endif
