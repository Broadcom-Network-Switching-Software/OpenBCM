/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  Copyright: (c) 2012 Broadcom Corp.
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  File       : tscmod_main.c
 *  Description: Implements function that selects the tier 1
 *             : It is the main entry point to the TSC serdes driver.
 *---------------------------------------------------------------------- */
#include <shared/bsl.h>
#include "tscmod_main.h"

#ifdef _SDK_TSCMOD_
/* INCLUDE_XGXS_TSCMOD defined in phydefs.h  in SDK */
#include "phydefs.h"
#endif

#ifdef _DV_TRIDENT2
/* INCLUDE_XGXS_TSCMOD needs to be defined for verilog sims.  */
/* In some cases it is done in the makefile. For others we do it here */
#ifndef INCLUDE_XGXS_TSCMOD
#define INCLUDE_XGXS_TSCMOD
#endif /* INCLUDE_XGXS_TSCMOD */
#endif

#ifdef INCLUDE_XGXS_TSCMOD
#include "tscmod_main.h"
#include "tscmod_functions.h"

/* extern void tscmod_init();  */
extern void tscmod_configuration_selector(const char* cfg, /* INOUT */tscmod_st *ws, /* OUTPUT */int *retVal);
extern void init_tscmod_st(/* INOUT */tscmod_st *ws);

extern int tscmod_pcs_bypass_ctl(tscmod_st* pc) ;
extern int tscmod_master_slave_control(tscmod_st* pc) ;
extern int tscmod_slave_wakeup_control(tscmod_st* pc) ;
extern int tscmod_credit_set(tscmod_st* pc) ;
extern int tscmod_encode_set  (tscmod_st* pc) ;
extern int tscmod_decode_set (tscmod_st* pc) ;
extern int tscmod_credit_control (tscmod_st* pc) ;
extern int tscmod_afe_reset_analog  (tscmod_st* pc) ;
extern int tscmod_afe_rxseq_start_control (tscmod_st* pc) ;
extern int tscmod_afe_speed_up_dsc_vga  (tscmod_st* pc) ;
extern int tscmod_tx_lane_control  (tscmod_st* pc) ;
extern int tscmod_rx_lane_control  (tscmod_st* pc) ;
extern int tscmod_100fx_control   (tscmod_st* pc) ;
extern int tscmod_tx_lane_disable  (tscmod_st* pc) ;
extern int tscmod_power_control  (tscmod_st* pc) ;
extern int tscmod_autoneg_set  (tscmod_st* pc) ;
extern int tscmod_autoneg_get  (tscmod_st* pc) ;
extern int tscmod_autoneg_control  (tscmod_st* pc) ;
extern int tscmod_autoneg_page_set(tscmod_st* pc) ;
extern int tscmod_carrier_extension_set  (tscmod_st* pc) ;
extern int tscmod_reg_read  (tscmod_st* pc) ;
extern int tscmod_reg_write  (tscmod_st* pc) ;
extern int tscmod_prbs_check  (tscmod_st* pc) ;
extern int tscmod_cjpat_crpat_control  (tscmod_st* pc) ;
extern int tscmod_cjpat_crpat_check  (tscmod_st* pc) ;
extern int tscmod_diag  (tscmod_st* pc) ;
extern int tscmod_lane_swap  (tscmod_st* pc) ;
extern int tscmod_parallel_detect_control  (tscmod_st* pc) ;
extern int tscmod_clause72_control  (tscmod_st* pc) ;
extern int tscmod_pll_sequencer_control  (tscmod_st* pc) ;
extern int tscmod_pll_lock_wait(tscmod_st* pc) ;
extern int tscmod_prog_data  (tscmod_st* pc) ;
extern int tscmod_duplex_control  (tscmod_st* pc) ;
extern int tscmod_revid_read  (tscmod_st* pc) ;
extern int tscmod_rx_seq_control  (tscmod_st* pc) ;
extern int tscmod_tx_rx_polarity  (tscmod_st* pc) ;
extern int tscmod_set_aer  (tscmod_st* pc) ;
extern int tscmod_set_port_mode  (tscmod_st* pc) ;
extern int tscmod_set_an_port_mode  (tscmod_st* pc) ;
extern int tscmod_prbs_control  (tscmod_st* pc) ;
extern int tscmod_prbs_mode (tscmod_st* pc) ;
extern int tscmod_prbs_seed_a1a0  (tscmod_st* pc) ;
extern int tscmod_prbs_seed_a3a2  (tscmod_st* pc) ;
extern int tscmod_prbs_seed_load_control  (tscmod_st* pc) ;
extern int tscmod_prbs_decouple_control (tscmod_st* pc) ;
extern int tscmod_scrambler_control  (tscmod_st* pc) ;
extern int tscmod_rx_DFE_tap1_control  (tscmod_st* pc) ;
extern int tscmod_rx_DFE_tap2_control  (tscmod_st* pc) ;
extern int tscmod_rx_DFE_tap3_control  (tscmod_st* pc) ;
extern int tscmod_rx_DFE_tap4_control  (tscmod_st* pc) ;
extern int tscmod_rx_DFE_tap5_control  (tscmod_st* pc) ;
extern int tscmod_rx_PF_control  (tscmod_st* pc) ;
extern int tscmod_rx_VGA_control  (tscmod_st* pc) ;
extern int tscmod_soft_reset  (tscmod_st* pc) ;
extern int tscmod_aer_lane_select  (tscmod_st* pc) ;
extern int tscmod_set_spd_intf  (tscmod_st* pc) ;
extern int tscmod_speed_intf_control  (tscmod_st* pc) ;
extern int tscmod_tx_bert_control  (tscmod_st* pc) ;
extern int tscmod_rx_loopback_control  (tscmod_st* pc) ;
extern int tscmod_rx_loopback_pcs_bypass  (tscmod_st* pc) ;
extern int tscmod_tx_tap_control  (tscmod_st* pc) ;
extern int tscmod_tx_pi_control    (tscmod_st* pc) ;
extern int tscmod_tx_loopback_control  (tscmod_st* pc) ;
extern int tscmod_port_stat_display  (tscmod_st* pc) ;
extern int tscmod_core_reset  (tscmod_st* pc) ;
extern int tscmod_fwmode_control  (tscmod_st* pc) ;
extern int tscmod_tx_amp_control  (tscmod_st* pc) ;
extern int tscmod_rx_p1_slicer_control  (tscmod_st* pc) ;
extern int tscmod_rx_m1_slicer_control  (tscmod_st* pc) ;
extern int tscmod_rx_d_slicer_control  (tscmod_st* pc) ;
extern int tscmod_refclk_set  (tscmod_st* pc) ;
extern int tscmod_asymmetric_mode_set  (tscmod_st* pc) ;
extern int tscmod_wait_pmd_lock  (tscmod_st* pc) ;
extern int tscmod_misc_control  (tscmod_st* pc) ;
extern int tscmod_firmware_set  (tscmod_st* pc) ;
extern int tscmod_uc_sync_cmd (tscmod_st* pc) ;
extern int tscmod_FEC_control (tscmod_st* pc) ;
extern int tscmod_higig2_control(tscmod_st* pc);
extern int tscmod_EEE_pass_thru_set(tscmod_st* pc);

str2PCSFn   tscmod_pcsFuncTable[NUMBER_PCS_FUNCTION_TABLE_ENTRIES]; /* no malloc */
tscmod_int_st _tscmod_int_st;

/* files below are included to get a monolithic code. The reason being a
 * mandate from some customers to have static functions (i.e. only accessible
 * within a file.). If this requirement be proved false, we can move to more
 * conventional methods of code management */

#if defined(_DV_TRIDENT2) || defined(STANDALONE)

#include "tscmod_enum_defines.c"
#include "tscmod_phyreg.c"
#include "tscmod_cfg_seq.c"
#ifndef _SDK_TSCMOD_
#include "tscmod_configuration_selector.c"
#endif
#include "tscmod_diagnostics.c"

#endif

/*!
\brief Figure out if lane is active.

\param ls #tscmod_st::tscmod_lane_select (selects 1 or more lanes)
\param lane lane under consideration. We always go from 0 to 3.

\returns TRUE if lane enabled, FALSE if not, SOC_E_ERROR on any error.

\details This function indicates whethere a particular lane within the
#tscmod_lane_select enum is enabled or not.  This function returns a non-zero
value if a lane is enabled as indicated by the enum value #tscmod_lane_select,
and a value of 0 if the lane is disabled.  Note that this function does not
always return the same non-zero value.
*/
static int isLaneEnabled(tscmod_lane_select ls, int lane)
{
  if ((ls == TSCMOD_LANE_BCST) && lane == 0) {
    return TRUE;
  }
  if ((lane < 0) || (lane >= 4)) {
    printf("%-22s FATAL (Internal): Bad lane:%d\n", FUNCTION_NAME(), lane);
    return SOC_E_ERROR;
  }
  if ((ls < TSCMOD_LANE_0_0_0_1) || (ls >= TSCMOD_LANE_ILLEGAL)) {
    printf("%-22s FATAL (Internal): Bad lane sel:%d\n", FUNCTION_NAME(),ls);
    return SOC_E_ERROR;
  }
  return (e2n_tscmod_lane_select[ls] & (0x1 << lane) ? TRUE : FALSE);
}

int getLaneSelect(int lane)
{
   int rv ;
   switch(lane) {
   case 0: rv= TSCMOD_LANE_0_0_0_1 ; break ;
   case 1: rv= TSCMOD_LANE_0_0_1_0 ; break ;
   case 2: rv= TSCMOD_LANE_0_1_0_0 ; break ;
   case 3: rv= TSCMOD_LANE_1_0_0_0 ; break ;
   case 4: rv= TSCMOD_LANE_0_0_1_1 ; break ;
   case 5: rv= TSCMOD_LANE_1_1_0_0 ; break ;
   case 6: rv= TSCMOD_LANE_1_1_1_1 ; break ;
   default:
      rv =  TSCMOD_LANE_ILLEGAL; break ;

   }
   return rv ;
}

/*!
\brief (Internal). Get generic model type from revision specific type.

\param model #tscmod_model_type

\returns  Generic Type or TSCMOD_MODEL_TYPE_ILLEGAL

\detail This function returns the generic model type for a particular core.  Use
this if you just want to know what type of core you have without regard to the
revision letter or number.  For instance it maps #TSCMOD_XX_A0, #TSCMOD_XX_B1
onto #TSCMOD_XX.
*/
/*
static tscmod_model_type getGenericModelType(tscmod_model_type model)
{
  switch(model) {
    case TSCMOD_WC:
    case TSCMOD_A0:
    default:
      return TSCMOD_MODEL_TYPE_ILLEGAL;
  }
  return TSCMOD_WC ;
}
*/
/*!
\brief (Internal). Convert #tscmod_spd_intfc_type string to enum number.

\param s string

\returns A value in the enum #tscmod_spd_intfc_type if a legitimate name for the
enum was passed to the function, and the value TSCMOD_SPD_ILLEGAL otherwise.

\details
<B>Do not directly call this function.</B>. This converts a string
(case sensitive) of the enum #tscmod_spd_intfc_type to the enum value.
*/
tscmod_spd_intfc_type tscmod_spd_intf_s2e(char* s)
{
  int i=0;
  for (i=0;i<TSCMOD_SPD_ILLEGAL;i++) {
    if (!sal_strcmp (s, e2s_tscmod_spd_intfc_type[i])) {
      return i;
    }
  }
  printf("%-22s Warning: Failed to match speed string to enum\n", FUNCTION_NAME());
  return TSCMOD_SPD_ILLEGAL;
}

const char * tscmod_aspd_vec2s(int vec)
{  int i=0;
   for (i=0;i<TSCMOD_ASPD_TYPE_ILLEGAL;i++) {
      if(vec== e2n_tscmod_aspd_type[i]) {
         return e2s_tscmod_aspd_type[i] ;
      }
   }
   return e2s_tscmod_aspd_type[TSCMOD_ASPD_TYPE_ILLEGAL] ;
}



/*!
\brief Selection function for Tier1 functions.

\param cs  Tier1 string handle (case-sensitive). Please refer to the list below.

\param ws  TSC Context struct

\param returnValue  The return values from a Tier1 function is placed here.

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
Users may call Tier1 functions directly, or indirectly via this function by
passing an appropriate string handle.

The Tier1 handle is a string at the user interface level. Platforms
call tier1 functions by invoking this selector function with the string.
Please refer to the respective Tier1 functions to determine how to set up
#tscmod_st.

All arguments to the Tier1 function is encapsulated in the warpcore context
struct (#tscmod_st) which is also passed to the Tier1 function. Tier1 functions
which returns values (ex. when you read a register) update a a field inside
#tscmod_st (ex. tscmod_st::accRead). Tier1 functions also update #tscmod_st to
reflect serdes configuration. So platforms communicate with TSCMod exclusively
through the selection string and #tscmod_st structures.

When multiple lanes are selected (using #tscmod_st::lane_select), tier1 functions
are called multiple times. Lane specific info is in #tscmod_st::per_lane_control
which is a 32 bit integer. Generally 8 bits are allocated for each lane to
allow specifying values simultaneously.

If more than 8 bits are needed by the tier1 function, we take this approach.

\li The tier1 must be called once per lane. This means #tscmod_st::lane_select
can only be TSCMOD_LANE_0_0_0_1, TSCMOD_LANE_0_0_1_0, TSCMOD_LANE_0_1_0_0, or
TSCMOD_LANE_1_0_0_0.
\li Set #tscmod_st::per_lane_control_x1 through #tscmod_st::per_lane_control_x8
\li #tscmod_st::per_lane_control_x0  is set to imply #tscmod_st::per_lane_control
is not valid. Set consequtive bits in per_lane_control_x0 to indicate which of
#tscmod_st::per_lane_control_x1 through #tscmod_st::per_lane_control_x8 are
active.

For example to specify a 256 bit for lane 2
\li set #tscmod_st::tscmod_lane_select to TSCMOD_LANE_0_1_0_0
\li set #tscmod_st::per_lane_control_x0 to 0x000000ff (8 bits)
\li set #tscmod_st::per_lane_control_x1 through #tscmod_st::per_lane_control_x8
to needed 256 bit value.

If the tier1 uses aggregate registers (i.e. multiple lane information in single
register) or if broadcast mode is used (i.e. multiple registers accessed
simultaneously) the tier1 will optimise the register access and return a
special status to tscmod_tier1_selector, to avoid multiple calls.

The active lane is stored in #tscmod_st::this_lane.
*/

void tscmod_tier1_selector(const char* cs,tscmod_st* ws, int *returnValue)
{
  int lane, tmp_lane;
  int rv=0;

  tmp_lane = ws->this_lane ;
  if( ws->verbosity & TSCMOD_DBG_FUNC )
     printf("%-22s(%-18s): Unit/Port/Phy_ad %d/%d/0x%x cntl=%x sel=%x\n",
            FUNCTION_NAME(),cs,ws->unit,ws->port,ws->phy_ad, ws->per_lane_control, ws->lane_select);
  for(lane=0; lane < 4;lane++) {
     if (!isLaneEnabled(ws->lane_select,lane))
        continue; /* check next loop iteration  */
     if( ws->verbosity & TSCMOD_DBG_FUNC )
        printf("%-22s(%-18s): %s ln=%0d dxgxs=%0x\n",FUNCTION_NAME(),cs,
            e2s_tscmod_lane_select[ws->lane_select], ws->this_lane, ws->dxgxs);
     ws->this_lane = lane;
     rv = tscmod_searchPCSFuncTable(cs,ws); /* search for functions in the table. */
     if        (rv == SOC_E_FUNC_NOT_FOUND) {
        /* no function found. Continue looking for it in TSCMod functions */
     } else if (rv == SOC_E_NONE) {
        continue; /* proceed to the next lane if any */
     } else if (rv == SOC_E_MULT_REG) {
        *returnValue =  SOC_E_NONE; /* i.e. do not execute for multiple lanes.*/
     } else if (rv == SOC_E_ERROR) {
        printf("Function executed from PCS Func Table with errors.\n");
        /* found in PCS table, executed and returned error code. */
         *returnValue =  rv;
     } else {
        printf("Function in PCS Func Table returned non-standard value\n");
        *returnValue =  SOC_E_ERROR;
     }
     /* use the handle to search for TSCMod functions */
     /* using following define to meet 80 column width rule */
#define IF_TSCM_HNDL_CMP(_str,_func)  \
if(!sal_strcmp((_str),cs)){rv=(_func)(ws);}
         IF_TSCM_HNDL_CMP(PCS_BYPASS_CTL         ,tscmod_pcs_bypass_ctl)
    else IF_TSCM_HNDL_CMP(MASTER_SLAVE_CONTROL   ,tscmod_master_slave_control)
    else IF_TSCM_HNDL_CMP(SLAVE_WAKEUP_CONTROL   ,tscmod_slave_wakeup_control)
    else IF_TSCM_HNDL_CMP(CREDIT_SET             ,tscmod_credit_set)
    else IF_TSCM_HNDL_CMP(ENCODE_SET             ,tscmod_encode_set)
    else IF_TSCM_HNDL_CMP(DECODE_SET             ,tscmod_decode_set)
    else IF_TSCM_HNDL_CMP(CREDIT_CONTROL         ,tscmod_credit_control)
    else IF_TSCM_HNDL_CMP(AFE_RESET_ANALOG       ,tscmod_afe_reset_analog)
    else IF_TSCM_HNDL_CMP(AFE_RXSEQ_START_CONTROL,tscmod_afe_rxseq_start_control)
    else IF_TSCM_HNDL_CMP(AFE_SPEED_UP_DSC_VGA   ,tscmod_afe_speed_up_dsc_vga)
    else IF_TSCM_HNDL_CMP(TX_LANE_CONTROL        ,tscmod_tx_lane_control)
    else IF_TSCM_HNDL_CMP(RX_LANE_CONTROL        ,tscmod_rx_lane_control)
    else IF_TSCM_HNDL_CMP(_100FX_CONTROL         ,tscmod_100fx_control)
    else IF_TSCM_HNDL_CMP(TX_LANE_DISABLE        ,tscmod_tx_lane_disable)
    else IF_TSCM_HNDL_CMP(POWER_CONTROL          ,tscmod_power_control)
    else IF_TSCM_HNDL_CMP(AUTONEG_SET            ,tscmod_autoneg_set)
    else IF_TSCM_HNDL_CMP(AUTONEG_GET            ,tscmod_autoneg_get)
    else IF_TSCM_HNDL_CMP(AUTONEG_CONTROL        ,tscmod_autoneg_control)
    else IF_TSCM_HNDL_CMP(AUTONEG_PAGE_SET       ,tscmod_autoneg_page_set)
    else IF_TSCM_HNDL_CMP(ANEG_CONTROL           ,tscmod_autoneg_control)
    else IF_TSCM_HNDL_CMP(CARRIER_EXTENSION      ,tscmod_carrier_extension_set)
    else IF_TSCM_HNDL_CMP(REG_READ               ,tscmod_reg_read)
    else IF_TSCM_HNDL_CMP(REG_WRITE              ,tscmod_reg_write)
    else IF_TSCM_HNDL_CMP(PRBS_CHECK             ,tscmod_prbs_check)
    else IF_TSCM_HNDL_CMP(CJPAT_CRPAT_CONTROL    ,tscmod_cjpat_crpat_control)
    else IF_TSCM_HNDL_CMP(CJPAT_CRPAT_CHECK      ,tscmod_cjpat_crpat_check)
    else IF_TSCM_HNDL_CMP(TSCMOD_DIAG            ,tscmod_diag)
    else IF_TSCM_HNDL_CMP(LANE_SWAP              ,tscmod_lane_swap)
    else IF_TSCM_HNDL_CMP(PARALLEL_DETECT_CONTROL,tscmod_parallel_detect_control)
    else IF_TSCM_HNDL_CMP(CLAUSE_72_CONTROL      ,tscmod_clause72_control)
    else IF_TSCM_HNDL_CMP(PLL_SEQUENCER_CONTROL  ,tscmod_pll_sequencer_control)
    else IF_TSCM_HNDL_CMP(PLL_LOCK_WAIT          ,tscmod_pll_lock_wait)
    else IF_TSCM_HNDL_CMP(PROG_DATA              ,tscmod_prog_data)
    else IF_TSCM_HNDL_CMP(DUPLEX_CONTROL         ,tscmod_duplex_control)
    else IF_TSCM_HNDL_CMP(REVID_READ             ,tscmod_revid_read)
    else IF_TSCM_HNDL_CMP(RX_SEQ_CONTROL         ,tscmod_rx_seq_control)
    else IF_TSCM_HNDL_CMP(SET_POLARITY           ,tscmod_tx_rx_polarity)
    else IF_TSCM_HNDL_CMP(SET_AER                ,tscmod_set_aer)
    else IF_TSCM_HNDL_CMP(SET_PORT_MODE          ,tscmod_set_port_mode)
    else IF_TSCM_HNDL_CMP(SET_AN_PORT_MODE       ,tscmod_set_an_port_mode)
    else IF_TSCM_HNDL_CMP(PRBS_CONTROL           ,tscmod_prbs_control)
    else IF_TSCM_HNDL_CMP(PRBS_MODE              ,tscmod_prbs_mode)
    else IF_TSCM_HNDL_CMP(PRBS_SEED_A1A0         ,tscmod_prbs_seed_a1a0)
    else IF_TSCM_HNDL_CMP(PRBS_SEED_A3A2         ,tscmod_prbs_seed_a3a2)
    else IF_TSCM_HNDL_CMP(PRBS_SEED_LOAD_CONTROL ,tscmod_prbs_seed_load_control)
    else IF_TSCM_HNDL_CMP(PRBS_DECOUPLE_CONTROL  ,tscmod_prbs_decouple_control)
    else IF_TSCM_HNDL_CMP(SCRAMBLER_CONTROL      ,tscmod_scrambler_control)
    else IF_TSCM_HNDL_CMP(RX_DFE_TAP1_CONTROL    ,tscmod_rx_DFE_tap1_control)
    else IF_TSCM_HNDL_CMP(RX_DFE_TAP2_CONTROL    ,tscmod_rx_DFE_tap2_control)
    else IF_TSCM_HNDL_CMP(RX_DFE_TAP3_CONTROL    ,tscmod_rx_DFE_tap3_control)
    else IF_TSCM_HNDL_CMP(RX_DFE_TAP4_CONTROL    ,tscmod_rx_DFE_tap4_control)
    else IF_TSCM_HNDL_CMP(RX_DFE_TAP5_CONTROL    ,tscmod_rx_DFE_tap5_control)
    else IF_TSCM_HNDL_CMP(RX_PF_CONTROL          ,tscmod_rx_PF_control)
    else IF_TSCM_HNDL_CMP(RX_VGA_CONTROL         ,tscmod_rx_VGA_control)
    else IF_TSCM_HNDL_CMP(SOFT_RESET             ,tscmod_soft_reset)
    else IF_TSCM_HNDL_CMP(AER_LANE_SELECT        ,tscmod_aer_lane_select)
    else IF_TSCM_HNDL_CMP(SET_SPD_INTF           ,tscmod_set_spd_intf)
    else IF_TSCM_HNDL_CMP(SPD_INTF_CONTROL       ,tscmod_speed_intf_control)
    else IF_TSCM_HNDL_CMP(TX_BERT_CONTROL        ,tscmod_tx_bert_control)
    else IF_TSCM_HNDL_CMP(RX_LOOPBACK_CONTROL    ,tscmod_rx_loopback_control)
    else IF_TSCM_HNDL_CMP(RX_LOOPBACK_PCSBYP     ,tscmod_rx_loopback_pcs_bypass)
    else IF_TSCM_HNDL_CMP(TX_TAP_CONTROL         ,tscmod_tx_tap_control)
    else IF_TSCM_HNDL_CMP(TX_PI_CONTROL          ,tscmod_tx_pi_control)
    else IF_TSCM_HNDL_CMP(TX_LOOPBACK_CONTROL    ,tscmod_tx_loopback_control)
    else IF_TSCM_HNDL_CMP(PORT_STAT_DISPLAY      ,tscmod_port_stat_display)
    else IF_TSCM_HNDL_CMP(CORE_RESET             ,tscmod_core_reset)
    else IF_TSCM_HNDL_CMP(FWMODE_CONTROL         ,tscmod_fwmode_control)
    else IF_TSCM_HNDL_CMP(TX_AMP_CONTROL         ,tscmod_tx_amp_control)
    else IF_TSCM_HNDL_CMP(RX_P1_SLICER_CONTROL   ,tscmod_rx_p1_slicer_control)
    else IF_TSCM_HNDL_CMP(RX_M1_SLICER_CONTROL   ,tscmod_rx_m1_slicer_control)
    else IF_TSCM_HNDL_CMP(RX_D_SLICER_CONTROL    ,tscmod_rx_d_slicer_control)
    else IF_TSCM_HNDL_CMP(REFCLK_SET             ,tscmod_refclk_set)
    else IF_TSCM_HNDL_CMP(ASYMMETRIC_MODE_SET    ,tscmod_asymmetric_mode_set)
    else IF_TSCM_HNDL_CMP(WAIT_PMD_LOCK          ,tscmod_wait_pmd_lock)
    else IF_TSCM_HNDL_CMP(MISC_CONTROL           ,tscmod_misc_control)
    else IF_TSCM_HNDL_CMP(FIRMWARE_SET           ,tscmod_firmware_set)
    else IF_TSCM_HNDL_CMP(UC_SYNC_CMD            ,tscmod_uc_sync_cmd)
    else IF_TSCM_HNDL_CMP(FEC_CONTROL            ,tscmod_FEC_control)
    else IF_TSCM_HNDL_CMP(HIGIG2_CONTROL         ,tscmod_higig2_control)
    else IF_TSCM_HNDL_CMP(EEE_PASS_THRU_SET      ,tscmod_EEE_pass_thru_set)
    /* ... implement other functions here. */
    else {
      printf("%-22s FATAL: Unit:%d Port:%d Bad Tier1 handle:%s\n",
	                              FUNCTION_NAME(), ws->unit, ws->port,cs);
      rv = SOC_E_ERROR;
    }
    /* if the returnValue indicates an error, then break the loop */
    /* also only do 1 pass through the loop in the event of broadcast */
    if ((rv != SOC_E_NONE) || (ws->lane_select == TSCMOD_LANE_BCST))
      break;
  }
  ws->this_lane = tmp_lane ;
  *returnValue = rv;
  if (rv == SOC_E_MULT_REG) {
    printf("%-22s(%-18s): Aggregate reg. prog. Unit/Port/Phy_ad %d/%d/0x%x\n",
	   FUNCTION_NAME(), cs, ws->unit, ws->port,ws->phy_ad);
  } else if (rv != SOC_E_NONE) {
    if (rv == SOC_E_TIMEOUT) {
      printf("%-22s(%-18s): FATAL: Timeout. Unit/Port/Phy_ad %d/%d/0x%x\n",
             FUNCTION_NAME(), cs, ws->unit, ws->port,ws->phy_ad);
    } else {
      printf("%-22s(%-18s): FATAL: Failed(%0d). Unit/Port/Phy_ad %d/%d/0x%x\n",
             FUNCTION_NAME(), cs, rv, ws->unit, ws->port,ws->phy_ad);
    }
    *returnValue =  SOC_E_ERROR;
    return ;
  }
  *returnValue =  SOC_E_NONE;
  return ;
}

/* Since tscmod cannot have global variables, use this trick to ensure
 * tscmod_init is called before other functions.  SDK et. al. don't
 * know boolean types. Hence int types are used.
 */
#ifdef STANDALONE
int main(int argc, char** argv)
{
  int returnValue;
#else
/*!
\brief Initialize any structures needed to run TSCMod.

\details
*/

void tscmod_init()
{
#endif
  int n;
#ifdef STANDALONE
  tscmod_st dummy1, dummy2; /* for two warpcores. */
#endif

  #if defined (_DV_REDSTONE)
    svSetScope(svGetScopeFromName("Sys.Env"));
  #endif

  for(n = 0; n < NUMBER_PCS_FUNCTION_TABLE_ENTRIES; ++n) {
    tscmod_pcsFuncTable[n].p = NULL;
    tscmod_pcsFuncTable[n].fp = NULL;
  }

  _tscmod_int_st.asymmetric_mode = 0;

/* cfg.txt is broken since it conforms to old SDK structures. It has to
 * be rewritten. For now the read func. is commented out and tscmod_read_cfg.c
 * file is removed from Makefile.
 * I think cfg.txt will only make sense in the verilog world. */

#ifdef STANDALONE
  init_tscmod_st(&dummy1) ;
  dummy1.verbosity        = TSCMOD_DBG_REG ;
  dummy1.unit             = 0;     /* chip number */
  dummy1.port             = 3;     /* port number. might not begin with a 1.  */
  dummy1.spd_intf         = TSCMOD_SPD_1000_SGMII;
  dummy1.regacc_type      = TSCMOD_MDIO_CL22;
  dummy1.os_type          = TSCMOD_OS5;
  dummy1.port_type        = TSCMOD_MULTI_PORT;
  dummy1.per_lane_control = 0;
  dummy1.this_lane        = 0;
  dummy1.lane_select      = TSCMOD_LANE_0_0_0_1 ;
  dummy1.dxgxs            = 0;

  init_tscmod_st(&dummy2) ;
  dummy2.unit             = 0;             /* same chip as dummy1. */
  dummy2.port             = 5;             /* this is presumably the second warpcore. */
  dummy2.spd_intf         = TSCMOD_SPD_1000_SGMII;
  dummy2.regacc_type      = TSCMOD_MDIO_CL22;
  dummy2.os_type          = TSCMOD_OS5;
  dummy2.port_type        = TSCMOD_MULTI_PORT;
  dummy2.per_lane_control = 0;
  dummy2.this_lane        = 1;
  dummy2.lane_select      = TSCMOD_LANE_0_0_1_0 ;
  dummy2.dxgxs            = 0;

  tscmod_pcsFuncTable[0].p  = "RX_SEQ_CONTROL";
  tscmod_pcsFuncTable[0].fp = tscmod_rx_seq_control;

  /* print_tscmod_st("First  TSC",&dummy1); */
  /* print_tscmod_st("Second TSC",&dummy2); */

  /* disable rx seq. start */
  tscmod_tier1_selector("RX_SEQ_CONTROL", &dummy1, &returnValue);
  tscmod_tier1_selector("RX_SEQ_CONTROL", &dummy2, &returnValue);

  /* issue soft reset for PCS registers. */
  tscmod_tier1_selector("SET_PORT_MODE", &dummy1, &returnValue);
  tscmod_tier1_selector("SET_PORT_MODE", &dummy2, &returnValue);

  /* issue soft reset for PCS registers. */
  tscmod_tier1_selector("SOFT_RESET", &dummy1, &returnValue);
  tscmod_tier1_selector("SOFT_RESET", &dummy2, &returnValue);

  /* disable cl37, cl73 AN and disable 1G and 10G Parallel Detect.
  tscmod_tier1_selector("DISABLE_ALL_AN_PDET", &dummy1, &returnValue);
  tscmod_tier1_selector("DISABLE_ALL_AN_PDET", &dummy2, &returnValue);
  */

  /* stops PLL sequencer, sets speed, sets intf, starts PLL sequencer */
  tscmod_tier1_selector("SET_SPD_INTF", &dummy1, &returnValue);
  tscmod_tier1_selector("SET_SPD_INTF", &dummy2, &returnValue);

  /* enable rx seq. start */
  tscmod_tier1_selector("RX_SEQ_CONTROL", &dummy1, &returnValue);
  tscmod_tier1_selector("RX_SEQ_CONTROL", &dummy2, &returnValue);

  return returnValue;
#else
    printf("%s completed\n",FUNCTION_NAME());
    return;
#endif        /* STANDALONE */
}
/* } just to make parens (main, STANDALONE) match*/

/* keep this in sync with tscmod_st changes. */
void copy_tscmod_st(tscmod_st *from, tscmod_st *to)
{
  to->id                 = from->id;
  to->unit               = from->unit;
  to->port               = from->port;
  to->phy_ad             = from->phy_ad;
  to->prt_ad             = from->prt_ad;
  to->lane_select        = from->lane_select;
  to->this_lane          = from->this_lane;
  to->per_lane_control   = from->per_lane_control;
  to->spd_intf           = from->spd_intf;
  to->regacc_type        = from->regacc_type;
  to->port_type          = from->port_type;
  to->os_type            = from->os_type;
  to->tsc_touched         = from->tsc_touched;
  to->lane_swap          = from->lane_swap;
  to->duplex             = from->duplex;
  to->accAddr            = from->accAddr;
  to->accData            = from->accData;
  to->diag_type          = from->diag_type;
  to->model_type         = from->model_type;
  to->an_type            = from->an_type;
  to->an_rf              = from->an_rf;
  to->an_pause           = from->an_pause;
  to->an_tech_ability    = from->an_tech_ability;
  to->an_fec             = from->an_fec;
  to->an_higig2          = from->an_higig2;
  to->an_cl72            = from->an_cl72;
  to->verbosity          = from->verbosity;
  to->refclk             = from->refclk;
  to->plldiv             = from->plldiv;
}

/*!
\brief (Internal) Initializes tscmod_st.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else

\details Most elements are initialized to illegal values.

*/

void init_tscmod_st(tscmod_st *pc)
{
  printf("%s: Initializing tscmod_st object\n", FUNCTION_NAME());
  pc->id                 = 0;
  pc->unit               = 0;
  pc->port               = 0;
  pc->phy_ad             = 0;
  pc->prt_ad             = 0;
  pc->this_lane          = 0;
  pc->lane_select        = TSCMOD_LANE_ILLEGAL;
  pc->per_lane_control   = 0;
  pc->spd_intf           = TSCMOD_SPD_ILLEGAL;
  pc->regacc_type          = TSCMOD_MDIO_CL22;
  pc->port_type          = TSCMOD_PORT_MODE_ILLEGAL;
  pc->os_type            = TSCMOD_OS_ILLEGAL;
  pc->tsc_touched         = 0;
  pc->lane_swap          = 0;
  pc->duplex             = 0;
  pc->accAddr            = 0;
  pc->accData            = 0;
  pc->diag_type          = TSCMOD_DIAG_ILLEGAL;
  pc->model_type         = TSCMOD_MODEL_TYPE_ILLEGAL;
  pc->an_type            = TSCMOD_AN_TYPE_ILLEGAL;
  pc->an_hg_vco          = 0;
  pc->an_rf              = 0;
  pc->an_pause           = 0;
  pc->an_tech_ability    = 0;
  pc->an_fec             = 0;
  pc->verbosity          = 10; /* Set it to max.. */
  pc->refclk             = 156;
  pc->plldiv             = 66;
  printf("%s: Done Initializing tscmod_st object\n", FUNCTION_NAME());
  return;
}


/*!
\brief Internal function to identify registers with 1, 2 and 4 copies.

\param addr The register address offeset

\returns int The number of copies, 1, 2 or 4.

\details
In the register set of Warpcore, Most registers have 4 copies i.e one per lane.
Some are 2 copies, i.e. one per 20G lane.  Some are 1 copy i.e. one per
warpcore. This function identifies the number of copies based on the address.
Currently this table is not complete, so this function is not used.
*/
int tscmod_reg_copy_cnt(int addr)
{
  return 4;
}
/*!
\brief Internal function to search for functions substituted by customer.

\param selStr Tier1 Selection string (same as other Tier1 functions)

\param ws Pointer to tscmod_st for TSC

\returns SOC_E_FUNC_NOT_FOUND if function not found.
If function is found, it returns what the function returns, (SOC_E_NONE,
SOC_E_MULT_REG, SOC_E_ERROR, SOC_E_TIMEOUT etc.)

\details
PCS FuncTable contains many functions that override TSCMod Tier1 functions,
or functions that encapsulate extra functionality.  As the name implies, in
most cases we override or augment PCS function.

This table associates functions with strings handles and we select functions
via these handles just like Tier1s. A handle not in the current set of TSCMOD
handles implies a new function. To override existing functions associate the
new function with an existing handle.

<B>The method for creating and compiling the handles with TSCMod it TBD</B>

Here we search the #pcsFuncTable for the function using the handle. If found
we execute it. Else we return and #tier1_selector will execute the native
function.
*/

int tscmod_searchPCSFuncTable(const char* selStr, tscmod_st* ws)
{
  int index = 0, rv = -1;
  if (tscmod_pcsFuncTable[index].p == NULL) {
    /* printf("Uninitialized PCS Function Table. No search done.\n"); */
    return SOC_E_FUNC_NOT_FOUND;
  }
  while (tscmod_pcsFuncTable[index].p != NULL) {
    if (!sal_strcmp(selStr, tscmod_pcsFuncTable[index].p)) {
      printf("Match in PCS Function Table:%s\n", tscmod_pcsFuncTable[index].p);
      /* check if function exists. */
      if (tscmod_pcsFuncTable[index].fp) {
        rv = tscmod_pcsFuncTable[index].fp(ws);
        return rv;
      } else {
        /* function not implemented. It might be implemented in TSCMod.
         * so continue looking, and tier1_selector will execute
         * the TSCMod Tier1. */
        return SOC_E_FUNC_NOT_FOUND;
      }
      break;
    }
    index++;
    if (index >= NUMBER_PCS_FUNCTION_TABLE_ENTRIES) {
      /* printf("No match for handle in PCS Function Table. %s\n", selStr); */
      return SOC_E_FUNC_NOT_FOUND; /* means continue searching in TSCMod */
    }
  }
  /* printf("%s Internal Error. Bad Search %s ", FUNCTION_NAME(), selStr); */
  return SOC_E_FUNC_NOT_FOUND; /* you should never arrive here. */
}

/*
static int dv_configuration_selector(char *str, tscmod_st* ws, int *rv)
{
  printf("%s:tscmod_configuration_selector deprecated this func.\n", FUNCTION_NAME());
return 0;  // ** not an error. Let user deal with it. **
}
*/
#else
typedef int _xgxs_tscmod_main_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_XGXS_TSCMOD */
