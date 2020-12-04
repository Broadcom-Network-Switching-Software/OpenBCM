/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <sal/types.h>
#include <shared/bsl.h>
/* FILE: wcmod_main.c */
/* DATE: Tue Jun 28 15:21:20 PDT 2011 */

#include "wcmod_main.h"
#ifdef _SDK_WCMOD_
/* INCLUDE_XGXS_WCMOD defined in phydefs.h  in SDK */
#include "phydefs.h"
#endif 

#ifdef VERILOG
/* INCLUDE_XGXS_WCMOD needs to be defined for verilog sims.  */
/* In some cases it is done in the makefile. For others we do it here */
#ifndef INCLUDE_XGXS_WCMOD
#define INCLUDE_XGXS_WCMOD
#endif /* INCLUDE_XGXS_WCMOD */
#endif

#ifdef INCLUDE_XGXS_WCMOD
#include "wcmod_main.h"
#include "wcmod_functions.h"
#ifndef _SDK_WCMOD_
#include "wcmod_platform_defines.h"
#endif

#ifdef _SDK_WCMOD_
#include <sal/core/sync.h>
#endif


str2PCSFn   wcmod_pcsFuncTable[NUMBER_PCS_FUNCTION_TABLE_ENTRIES]; /* no malloc */
sal_sem_t wcmod_mutex[NUMBER_OF_LOCKS];
wcmod_int_st _wcmod_int_st;

/* files below are included to get a monolithic code. The reason being a
 * mandate from some customers to have static functions (i.e. only accessible
 * within a file.). If this requirement be proved false, we can move to more
 * conventional methods of code management */

/*! 
\brief Figure out if lane is active.

\param ls #wcmod_st::wcmod_lane_select (selects 1 or more lanes)
\param lane lane under consideration. We always go from 0 to 3.

\returns TRUE if lane enabled, FALSE if not, SOC_E_ERROR on any error.

\details This function indicates whethere a particular lane within the
#wcmod_lane_select enum is enabled or not.  This function returns a non-zero
value if a lane is enabled as indicated by the enum value #wcmod_lane_select,
and a value of 0 if the lane is disabled.  Note that this function does not
always return the same non-zero value.
*/
int isLaneEnabled(wcmod_lane_select ls, int lane, wcmod_st *ws)
{
  int bound = 0;
  if ((ls == WCMOD_LANE_BCST) && lane == 0) {
    return TRUE;
  }

  if ((ws->model_type == WCMOD_QS_A0) || (ws->model_type == WCMOD_QS_B0)) {
    bound = 8;
  } else {
    bound = 4;
  }
  
  if ((lane < 0) || (lane >= bound)) {
    printf("%-22s FATAL (Internal): Bad lane:%d\n", FUNCTION_NAME(), lane);
    return SOC_E_ERROR;
  }
  if ((ls < WCMOD_LANE_0_0_0_1) || (ls > WCMOD_LANE_ILLEGAL)) {
    printf("%-22s FATAL (Internal): Bad lane sel:%d\n", FUNCTION_NAME(),ls);
    return SOC_E_ERROR;
  }
  return (e2n_wcmod_lane_select[ls] & (0x1 << lane) ? TRUE : FALSE);
}

/*! 
\brief (Internal). Get generic model type from revision specific type.

\param model #wcmod_model_type

\returns  Generic Type or WCMOD_MODEL_TYPE_ILLEGAL

\detail This function returns the generic model type for a particular core.  Use
this if you just want to know what type of core you have without regard to the
revision letter or number.  For instance it maps #WCMOD_WC_A0, #WCMOD_WC_B1
onto #WCMOD_WC.
*/
wcmod_model_type getGenericModelType(wcmod_model_type model)
{
  switch(model) {
    case WCMOD_WC:
    case WCMOD_WC_A0:
    case WCMOD_WC_A1:
    case WCMOD_WC_A2:
    case WCMOD_WC_B0:
    case WCMOD_WC_B1:
    case WCMOD_WC_B2:   
    case WCMOD_WC_C0:
    case WCMOD_WC_C1:
    case WCMOD_WC_C2:
    case WCMOD_WC_D0:
    case WCMOD_WC_D1:
    case WCMOD_WC_D2:
      return WCMOD_WC;
    case WCMOD_XN:
      return WCMOD_XN;
    case WCMOD_WL:
    case WCMOD_WL_A0:
      return WCMOD_WL;
    default:
      return WCMOD_MODEL_TYPE_ILLEGAL;
  }
}

/*! 
\brief (Internal). Convert #wcmod_spd_intfc_set string to enum number.

\param s string

\returns A value in the enum #wcmod_spd_intfc_set if a legitimate name for the
enum was passed to the function, and the value WCMOD_SPD_ILLEGAL otherwise.

\details
<B>Do not directly call this function.</B>. This converts a string
(case sensitive) of the enum #wcmod_spd_intfc_set to the enum value.
*/
wcmod_spd_intfc_set wcmod_spd_intf_s2e(char* s)
{
  int i = 0;
  for (i = 0;i < WCMOD_SPD_ILLEGAL;i++) {
    if (!sal_strcmp (s, e2s_wcmod_spd_intfc_set[i])) {
      return i;
    }
  }
  printf("%-22s Warning: Failed to match speed string to enum\n", FUNCTION_NAME());
  return WCMOD_SPD_ILLEGAL;
}

/*!
\brief Selection function for Tier1 functions. 

\param cs  Tier1 string handle (case-sensitive). Please refer to the list below.

\param ws  Warpcore Context struct

\param returnValue  The return values from a Tier1 function is placed here.

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
Users may call Tier1 functions directly, or indirectly via this function by
passing an appropriate string handle.

The Tier1 handle is a string at the user interface level. Platforms
call tier1 functions by invoking this selector function with the string.
Please refer to the respective Tier1 functions to determine how to set up
#wcmod_st.

All arguments to the Tier1 function is encapsulated in the warpcore context
struct (#wcmod_st) which is also passed to the Tier1 function. Tier1 functions
which returns values (ex. when you read a register) update a a field inside
#wcmod_st (ex. wcmod_st::accRead). Tier1 functions also update #wcmod_st to
reflect warpcore configuration. So platforms communicate with WCMod exclusively
through the selection string and #wcmod_st structures.

When multiple lanes are selected (using #wcmod_st::lane_select), tier1 functions
are called  multiple times. Lane specific info is in #wcmod_st::per_lane_control
which is a 32 bit integer. Generally 8 bits are allocated for each lane to
allow specifying values simultaneously.

If more than 8 bits are needed by the tier1 function, we take this approach.

\li The tier1 must be called once per lane. This means #wcmod_st::lane_select
can only be WCMOD_LANE_0_0_0_1, WCMOD_LANE_0_0_1_0, WCMOD_LANE_0_1_0_0, or
WCMOD_LANE_1_0_0_0. 
\li Set #wcmod_st::per_lane_control_x1 through #wcmod_st::per_lane_control_x8
\li #wcmod_st::per_lane_control_x0  is set to imply #wcmod_st::per_lane_control
is not valid. Set consequtive bits in per_lane_control_x0 to indicate which of 
#wcmod_st::per_lane_control_x1 through #wcmod_st::per_lane_control_x8 are
active.

For example to specify a 256 bit for lane 2
\li set #wcmod_st::wcmod_lane_select to WCMOD_LANE_0_1_0_0
\li set #wcmod_st::per_lane_control_x0 to 0x000000ff (8 bits)
\li set #wcmod_st::per_lane_control_x1 through #wcmod_st::per_lane_control_x8
to needed 256 bit value.

If the tier1 uses aggregate registers (i.e. multiple lane information in single
register) or if broadcast mode is used (i.e. multiple registers accessed
simultaneously) the tier1 will optimise the register access and return a
special status to wcmod_tier1_selector, to avoid multiple calls.

The active lane is stored in #wcmod_st::this_lane.
*/

int wcmod_tier1_selector(char* cs,wcmod_st* ws, int *returnValue)
{
  int lane, num_lane;
  int rv=0;
  int tmp_this_lane ;

  wcmod_init_called(TRUE); /* init must be called. */

  if (ws->verbosity > 0) printf("%-22s(%-18s): Unit/Port/Phy_ad %d/%d/0x%x\n",
                                      FUNCTION_NAME(),cs,ws->unit,ws->port,ws->phy_ad);
  #ifndef _SDK_WCMOD_
  if (sal_sem_take(wcmod_mutex[ws->id], sal_sem_FOREVER)) {
    printf("%-22s(%-18s): WARN: sem_wait fail\n", FUNCTION_NAME(), cs);
  } else {
    /* printf("%-22s(%-18s): sem_wait.\n", FUNCTION_NAME(), cs); */
  }
  #endif
  ws->lane_num_ignore = 0; /* always reset ignore to zero. */
  tmp_this_lane       = ws->this_lane ;

  if ((ws->model_type == WCMOD_QS_A0) || (ws->model_type == WCMOD_QS_B0)) {
    num_lane = 8;
  } else {
    num_lane = 4;
  }

  for(lane = 0; lane < num_lane; lane++) {
    if (!isLaneEnabled(ws->lane_select,lane,ws))
      continue; /* check next loop iteration  */
    if (ws->dxgxs > 3)  {
      if (ws->verbosity > 0) printf("%s Error: Invalid dxgxs value:%d", FUNCTION_NAME(), ws->dxgxs);
      return SOC_E_ERROR;
    }
    if (ws->verbosity > 0) printf("%-22s(%-18s): %s\n",FUNCTION_NAME(),cs,
                                e2s_wcmod_lane_select[ws->lane_select]);
    ws->this_lane = lane;
    rv = searchPCSFuncTable(cs,ws); /* search for functions in the table. */
    if        (rv == SOC_E_FUNC_NOT_FOUND) {
      /* no function found. Continue looking for it in WCMod functions */
    } else if (rv == SOC_E_NONE) {
      continue; /* proceed to the next lane if any */
    } else if (rv == SOC_E_MULT_REG) {
      ws->this_lane= tmp_this_lane ;
      return SOC_E_NONE; /* i.e. do not execute for multiple lanes. */
    } else if (rv == SOC_E_ERROR) {
      if (ws->verbosity > 0) printf("Function executed from PCS Func Table with errors.\n");
      /* found in PCS table, executed and returned error code. */
      return rv;
    } else {
      if (ws->verbosity > 0) printf("Function in PCS Func Table returned non-standard value\n");
      return SOC_E_ERROR;
    }
    /* use the handle to search for WCMod functions */
    /* using following define to meet 80 column width rule */
#define IF_WCM_HNDL_CMP(_str,_func)  if(!sal_strcmp((_str),cs)){rv=(_func)(ws);}
         IF_WCM_HNDL_CMP(PCS_BYPASS_CTL         ,wcmod_pcs_bypass_ctl)
    else IF_WCM_HNDL_CMP(MASTER_SLAVE_CONTROL   ,wcmod_master_slave_control)
    else IF_WCM_HNDL_CMP(SLAVE_WAKEUP_CONTROL   ,wcmod_slave_wakeup_control)
    else IF_WCM_HNDL_CMP(_100FX_CONTROL         ,wcmod_100fx_control)
    else IF_WCM_HNDL_CMP(TX_LANE_DISABLE        ,wcmod_tx_lane_disable)
    else IF_WCM_HNDL_CMP(POWER_CONTROL          ,wcmod_power_control)
    else IF_WCM_HNDL_CMP(AUTONEG_SET            ,wcmod_autoneg_set)
    else IF_WCM_HNDL_CMP(AUTONEG_CONTROL        ,wcmod_autoneg_control)
    else IF_WCM_HNDL_CMP(AUTONEG_PAGE_SET       ,wcmod_autoneg_page_set)
    else IF_WCM_HNDL_CMP(ANEG_CONTROL           ,wcmod_autoneg_control)
    else IF_WCM_HNDL_CMP(CARRIER_EXTENSION      ,wcmod_carrier_extension_set)
    else IF_WCM_HNDL_CMP(PRBS_CHECK             ,wcmod_prbs_check)
    else IF_WCM_HNDL_CMP(CJPAT_CRPAT_CONTROL    ,wcmod_cjpat_crpat_control)
    else IF_WCM_HNDL_CMP(CJPAT_CRPAT_CHECK      ,wcmod_cjpat_crpat_check)
    else IF_WCM_HNDL_CMP(WCMOD_DIAG             ,wcmod_diag)
    else IF_WCM_HNDL_CMP(LANE_SWAP              ,wcmod_lane_swap)
    else IF_WCM_HNDL_CMP(PARALLEL_DETECT_CONTROL,wcmod_parallel_detect_control)
    else IF_WCM_HNDL_CMP(CLAUSE_72_CONTROL      ,wcmod_clause72_control)
    else IF_WCM_HNDL_CMP(PLL_SEQUENCER_CONTROL  ,wcmod_pll_sequencer_control)
    else IF_WCM_HNDL_CMP(PROG_DATA              ,wcmod_prog_data)
    else IF_WCM_HNDL_CMP(DUPLEX_CONTROL         ,wcmod_duplex_control)
    else IF_WCM_HNDL_CMP(REVID_READ             ,wcmod_revid_read)
    else IF_WCM_HNDL_CMP(RX_SEQ_CONTROL         ,wcmod_rx_seq_control)
    else IF_WCM_HNDL_CMP(SET_POLARITY           ,wcmod_tx_rx_polarity)
    else IF_WCM_HNDL_CMP(SET_PORT_MODE          ,wcmod_set_port_mode)
    else IF_WCM_HNDL_CMP(PRBS_CONTROL           ,wcmod_prbs_control)
    else IF_WCM_HNDL_CMP(PRBS_DECOUPLE_CONTROL  ,wcmod_prbs_decouple_control)
    else IF_WCM_HNDL_CMP(SCRAMBLER_CONTROL      ,wcmod_scrambler_control)
    else IF_WCM_HNDL_CMP(RX_DFE_TAP1_CONTROL    ,wcmod_rx_DFE_tap1_control)
    else IF_WCM_HNDL_CMP(RX_DFE_TAP2_CONTROL    ,wcmod_rx_DFE_tap2_control)
    else IF_WCM_HNDL_CMP(RX_DFE_TAP3_CONTROL    ,wcmod_rx_DFE_tap3_control)
    else IF_WCM_HNDL_CMP(RX_DFE_TAP4_CONTROL    ,wcmod_rx_DFE_tap4_control)
    else IF_WCM_HNDL_CMP(RX_DFE_TAP5_CONTROL    ,wcmod_rx_DFE_tap5_control)
    else IF_WCM_HNDL_CMP(RX_PF_CONTROL          ,wcmod_rx_PF_control)
    else IF_WCM_HNDL_CMP(RX_LOW_FREQ_PF_CONTROL ,wcmod_rx_low_freq_PF_control)
    else IF_WCM_HNDL_CMP(RX_VGA_CONTROL         ,wcmod_rx_VGA_control)
    else IF_WCM_HNDL_CMP(SOFT_RESET             ,wcmod_soft_reset)
    else IF_WCM_HNDL_CMP(TX_RESET               ,wcmod_tx_reset)
    else IF_WCM_HNDL_CMP(RX_RESET               ,wcmod_rx_reset)
    else IF_WCM_HNDL_CMP(AER_LANE_SELECT        ,wcmod_aer_lane_select)
    else IF_WCM_HNDL_CMP(SET_SPD_INTF           ,wcmod_speed_intf_set)
    else IF_WCM_HNDL_CMP(SPD_INTF_CONTROL       ,wcmod_speed_intf_control)
    else IF_WCM_HNDL_CMP(TX_BERT_CONTROL        ,wcmod_tx_bert_control)
    else IF_WCM_HNDL_CMP(RX_LOOPBACK_CONTROL    ,wcmod_rx_loopback_control)
    else IF_WCM_HNDL_CMP(RX_LOOPBACK_PCSBYP     ,wcmod_rx_loopback_pcs_bypass)
    else IF_WCM_HNDL_CMP(TX_TAP_CONTROL         ,wcmod_tx_tap_control)
    else IF_WCM_HNDL_CMP(TX_PI_CONTROL          ,wcmod_tx_pi_control)
    else IF_WCM_HNDL_CMP(TX_LOOPBACK_CONTROL    ,wcmod_tx_loopback_control)
    else IF_WCM_HNDL_CMP(PORT_STAT_DISPLAY      ,wcmod_port_stat_display)
    else IF_WCM_HNDL_CMP(CORE_RESET             ,wcmod_core_reset)
    else IF_WCM_HNDL_CMP(TX_AMP_CONTROL         ,wcmod_tx_amp_control)
    else IF_WCM_HNDL_CMP(RX_P1_SLICER_CONTROL   ,wcmod_rx_p1_slicer_control)
    else IF_WCM_HNDL_CMP(RX_M1_SLICER_CONTROL   ,wcmod_rx_m1_slicer_control)
    else IF_WCM_HNDL_CMP(RX_D_SLICER_CONTROL    ,wcmod_rx_d_slicer_control)
    else IF_WCM_HNDL_CMP(REFCLK_SET             ,wcmod_refclk_set)
    else IF_WCM_HNDL_CMP(ASYMMETRIC_MODE_SET    ,wcmod_asymmetric_mode_set)
    else IF_WCM_HNDL_CMP(FIRMWARE_MODE_SET      ,wcmod_firmware_mode_set)
    else IF_WCM_HNDL_CMP(EEE_PASS_THRU_SET      ,wcmod_EEE_pass_thru_set)
    /* ... implement other functions here. */
    else {
      if (ws->verbosity > 0) printf("%-22s FATAL: Unit:%d Port:%d Bad Tier1 handle:%s\n", 
	                              FUNCTION_NAME(), ws->unit, ws->port,cs);
      rv = SOC_E_ERROR;
    }
    /* if the returnValue indicates an error, then break the loop */
    /* also only do 1 pass through the loop in the event of broadcast */
    if ((rv != SOC_E_NONE) || (ws->lane_select == WCMOD_LANE_BCST))
      break;
  }
  ws->this_lane= tmp_this_lane ;
  *returnValue = rv;
  if (rv == SOC_E_MULT_REG) {
    if (ws->verbosity > 0) printf("%-22s(%-18s): Aggregate reg. prog. Unit/Port/Phy_ad %d/%d/0x%x\n",
	   FUNCTION_NAME(), cs, ws->unit, ws->port,ws->phy_ad);
  } else if (rv != SOC_E_NONE) {
    if (rv == SOC_E_TIMEOUT) {
      if (ws->verbosity > 0) printf("%-22s(%-18s): FATAL: Timeout. Unit/Port/Phy_ad %d/%d/0x%x\n",
             FUNCTION_NAME(), cs, ws->unit, ws->port,ws->phy_ad);
    } else {
      if (ws->verbosity > 0) printf("%-22s(%-18s): FATAL: Failed. Unit/Port/Phy_ad %d/%d/0x%x\n",
             FUNCTION_NAME(), cs, ws->unit, ws->port,ws->phy_ad);
    }
    return SOC_E_ERROR;
  }
  #ifndef _SDK_WCMOD_
  if (sal_sem_give(wcmod_mutex[ws->id])) {
    printf("%-22s(%-18s): WARN: sem_post fail. Unit/Port/Phy_ad %d/%d/0x%x\n",
	   FUNCTION_NAME(), cs, ws->unit, ws->port,ws->phy_ad);
    return SOC_E_ERROR;
  } else {
    if (ws->verbosity > 0) printf("%-22s(%-18s): sem_post Unit/Port/Phy_ad %d/%d/0x%x\n",
     	   FUNCTION_NAME(), cs, ws->unit, ws->port,ws->phy_ad);
  }
  #endif
  return SOC_E_NONE;
}

/* Since wcmod cannot have global variables, use this trick to ensure
 * wcmod_init is called before other functions.  SDK et. al. don't
 * know boolean types. Hence int types are used.
 */
void wcmod_init_called(int x)
{
  return;
}

#ifdef STANDALONE
int main(int argc, char** argv)
{
  int returnValue;
#else
/*!
\brief Initialize any structures needed to run WCMod.

\details
*/

void wcmod_init()
{
#endif
  int n;
#ifdef STANDALONE
  wcmod_st dummy1, dummy2; /* for two warpcores. */
#endif
  wcmod_init_called(FALSE); /* init must not hav been called. */

  #if defined (_DV_REDSTONE)
    svSetScope(svGetScopeFromName("Sys.Env"));
  #endif

  for(n = 0; n < NUMBER_OF_LOCKS; ++n) {
    wcmod_mutex[n] = sal_sem_create("serdes_mutex", sal_sem_BINARY, 1);
  }

  for(n = 0; n < NUMBER_PCS_FUNCTION_TABLE_ENTRIES; ++n) {
    wcmod_pcsFuncTable[n].p = NULL;
    wcmod_pcsFuncTable[n].fp = NULL;
  }

  _wcmod_int_st.asymmetric_mode = 0;

/* cfg.txt is broken since it conforms to old SDK structures. It has to
 * be rewritten. For now the read func. is commented out and wcmod_read_cfg.c
 * file is removed from Makefile.
 * I think cfg.txt will only make sense in the verilog world. */

#ifdef STANDALONE
  dummy1.unit  = 0;     /* chip number */
  dummy1.port  = 1;     /* port number. might not begin with a 1.  */
  dummy1.spd_intf  = WCMOD_SPD_1000_FIBER;
  dummy1.mdio_type = WCMOD_MDIO_CL22;
  dummy1.os_type    = WCMOD_OS5;
  dummy1.port_type = WCMOD_COMBO;
  dummy1.per_lane_control    = 0;

  dummy2.unit  = 0;             /* same chip as dummy1. */
  dummy2.port  = 5;             /* this is presumably the second warpcore. */
  dummy2.spd_intf  = WCMOD_SPD_1000_FIBER;
  dummy2.mdio_type = WCMOD_MDIO_CL22;
  dummy2.os_type    = WCMOD_OS5;
  dummy2.port_type = WCMOD_COMBO;
  dummy2.per_lane_control    = 0;

  wcmod_pcsFuncTable[0].p = "RX_SEQ_CONTROL";
  wcmod_pcsFuncTable[0].fp = wcmod_rx_seq_control;

  /* print_wcmod_st("First  Warpcore",&dummy1); */
  /* print_wcmod_st("Second Warpcore",&dummy2); */

  /* disable rx seq. start */
  dummy1.en_dis = dummy2.en_dis = WCMOD_DISABLE;
  wcmod_tier1_selector("RX_SEQ_CONTROL", &dummy1, &returnValue);
  wcmod_tier1_selector("RX_SEQ_CONTROL", &dummy2, &returnValue);

  /* issue soft reset for PCS registers. */
  wcmod_tier1_selector("SET_PORT_MODE", &dummy1, &returnValue);
  wcmod_tier1_selector("SET_PORT_MODE", &dummy2, &returnValue);
  
  /* issue soft reset for PCS registers. */
  wcmod_tier1_selector("SET_SOFT_RESET", &dummy1, &returnValue);
  wcmod_tier1_selector("SET_SOFT_RESET", &dummy2, &returnValue);
  
  /* disable cl37, cl73 AN and disable 1G and 10G Parallel Detect. */
  wcmod_tier1_selector("DISABLE_ALL_AN_PDET", &dummy1, &returnValue);
  wcmod_tier1_selector("DISABLE_ALL_AN_PDET", &dummy2, &returnValue);

  /* stops PLL sequencer, sets speed, sets intf, starts PLL sequencer */
  wcmod_tier1_selector("SET_SPD_INTF", &dummy1, &returnValue);
  wcmod_tier1_selector("SET_SPD_INTF", &dummy2, &returnValue);

  /* enable rx seq. start */
  dummy1.en_dis = dummy2.en_dis = WCMOD_ENABLE;
  wcmod_tier1_selector("RX_SEQ_CONTROL", &dummy1, &returnValue);
  wcmod_tier1_selector("RX_SEQ_CONTROL", &dummy2, &returnValue);

  return returnValue;
  #else
    printf("%s completed\n",FUNCTION_NAME());
    return;
  #endif        /* STANDALONE */
}
/* } just to make parens (main, STANDALONE) match*/

/* keep this in sync with wcmod_st changes. */
void copy_wcmod_st(wcmod_st *from, wcmod_st *to)
{
  to->id                 = from->id;
  to->platform_info      = from->platform_info;
  to->unit               = from->unit;
  to->port               = from->port;
  to->phy_ad             = from->phy_ad;
  to->prt_ad             = from->prt_ad;
  to->lane_select        = from->lane_select;
  to->this_lane          = from->this_lane;
  to->lane_num_ignore    = from->lane_num_ignore;
  to->per_lane_control   = from->per_lane_control;
  to->en_dis             = from->en_dis;
  to->aer_ofs_strap      = from->aer_ofs_strap;
  to->multiport_addr_en  = from->multiport_addr_en;
  to->aer_bcst_ofs_strap = from->aer_bcst_ofs_strap;
  to->spd_intf           = from->spd_intf;
  to->mdio_type          = from->mdio_type;
  to->port_type          = from->port_type;
  to->os_type            = from->os_type;
  to->dxgxs              = from->dxgxs;
  to->wc_touched         = from->wc_touched;
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
#ifndef _SDK_WCMOD_
  copy_platform_info(from->platform_info, to->platform_info);
#endif
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
int wcmod_reg_copy_cnt(int addr)
{
  /* 0x0004 ieeeSpeed      */
  /* 0x0005 ieeeDevInPkg1  */
  /* 0x0006 ieeeDevInPkg2  */
  /* 0x0009 ieeetxDisable  */
  if(   (addr == 0x0004)
     || (addr == 0x0005)
     || (addr == 0x0006)
     || (addr == 0x0009)
     || ((addr >= 0x800D) && (addr <= 0x800E))
     || ((addr >= 0x8015) && (addr <= 0x801A))
     || ((addr >= 0x8105) && (addr <= 0x810C))
     || ((addr >= 0x8130) && (addr <= 0x813E))
     || ((addr >= 0x8140) && (addr <= 0x8146))
     || ((addr >= 0x8160) && (addr <= 0x816E)) 
     || ((addr >= 0x8190) && (addr <= 0x8197)) 
     || ((addr >= 0x81D0) && (addr <= 0x81DE)) 
     || ((addr >= 0x81E0) && (addr <= 0x81EF)) 
     || ((addr >= 0x81F0) && (addr <= 0x81FF))) {
    return 1;
  }
  /* 0x000A, ieeeRxSigDet  */
  /* 0x000B ieeePMDExtAbil */
  /* 0x0018 ieeeLnStat  */
  if(    (addr == 0x000A)
     ||  (addr == 0x000B)
     ||  (addr == 0x0018)
     || ((addr >= 0x8000) && (addr <= 0x800C))
     || ((addr >= 0x8010) && (addr <= 0x8014))
     || ((addr >= 0x801B) && (addr <= 0x801E))
     || ((addr >= 0x8100) && (addr <= 0x8104))
     || ((addr >= 0x810D) && (addr <= 0x810E))
     || ((addr >= 0x8110) && (addr <= 0x8119)) 
     || ((addr >= 0x8121) && (addr <= 0x8123))
     ||  (addr == 0x8129)
     || ((addr >= 0x8150) && (addr <= 0x815E)) 
     || ((addr >= 0x8170) && (addr <= 0x8172))) {
    return 2;
  }
  return 4;
}
/*!
\brief Internal function to search for functions substituted by customer.

\param selStr Tier1 Selection string (same as other Tier1 functions) 

\param ws Pointer to wcmod_st for WC

\returns SOC_E_FUNC_NOT_FOUND if function not found.
If function is found, it returns what the function returns, (SOC_E_NONE,
SOC_E_MULT_REG, SOC_E_ERROR, SOC_E_TIMEOUT etc.)

\details
PCS FuncTable contains many functions that override WCMod Tier1 functions,
or functions that encapsulate extra functionality.  As the name implies, in
most cases we override or augment PCS function.

This table associates functions with strings handles and we select functions
via these handles just like Tier1s. A handle not in the current set of WCMOD
handles implies a new function. To override existing functions associate the
new function with an existing handle.

<B>The method for creating and compiling the handles with WCMod it TBD</B>

Here we search the #pcsFuncTable for the function using the handle. If found
we execute it. Else we return and #tier1_selector will execute the native
function.
*/

int searchPCSFuncTable(char* selStr, wcmod_st* ws)
{
  /* int index = 0, rv = -1; */
  return SOC_E_FUNC_NOT_FOUND;

#if 0
  if (wcmod_pcsFuncTable[index].p == NULL) {
    /* printf("Uninitialized PCS Function Table. No search done.\n"); */
    return SOC_E_FUNC_NOT_FOUND;
  }
  while (wcmod_pcsFuncTable[index].p != NULL) {
    if (!sal_strcmp(selStr, wcmod_pcsFuncTable[index].p)) {
      if (ws->verbosity > 0) printf("Match in PCS Function Table:%s\n", wcmod_pcsFuncTable[index].p);
      /* check if function exists. */
      if (wcmod_pcsFuncTable[index].fp) {
        rv = wcmod_pcsFuncTable[index].fp(ws);
        return rv;
      } else {
        /* function not implemented. It might be implemented in WCMod.
         * so continue looking, and tier1_selector will execute
         * the WCMod Tier1. */
        return SOC_E_FUNC_NOT_FOUND;
      }
      break;
    }
    index++;
    if (index >= NUMBER_PCS_FUNCTION_TABLE_ENTRIES) {
      /* printf("No match for handle in PCS Function Table. %s\n", selStr); */
      return SOC_E_FUNC_NOT_FOUND; /* means continue searching in WCMod */
    }
  }
  if (ws->verbosity > 0) printf("%s Internal Error. Bad Search %s ", FUNCTION_NAME(), selStr);
  return SOC_E_ERROR; /* you should never arrive here. */
#endif
}

int dv_configuration_selector(char *str, wcmod_st* ws, int *rv)
{
  printf("%s:wcmod_configuration_selector deprecated this func.\n", FUNCTION_NAME());
  return 0; /* not an error. Let user deal with it. */
}
#else
typedef int _xgxs_wcmod_main_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_XGXS_WCMOD */
