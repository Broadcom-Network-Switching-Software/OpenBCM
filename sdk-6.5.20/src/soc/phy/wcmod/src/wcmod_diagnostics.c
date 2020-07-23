/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <sal/types.h>
#include <shared/bsl.h>
#include "wcmod_main.h"
#include "wcmod.h"
#include "wcmod_defines.h"
#ifndef __KERNEL__
#include <math.h>
#endif
#ifdef _SDK_WCMOD_
#include <sal/types.h>
#include <sal/core/spl.h>
#include <sal/core/thread.h>
#include <soc/eyescan.h>
#include "wcmod_phyreg.h"
#include "wcmod_diagnostics.h"
#endif

int _wcmod_enable_livelink_diagnostics(wcmod_st* ws);
int _wcmod_disable_livelink_diagnostics(wcmod_st* ws);
int _wcmod_enable_deadlink_diagnostics(wcmod_st* ws);
int _wcmod_disable_deadlink_diagnostics(wcmod_st* ws);
int _wcmod_set_voff(wcmod_st* ws, int voff);
int _wcmod_set_hoff(wcmod_st* ws, int hoff);
int _wcmod_get_max_voffset(wcmod_st* ws);
int _wcmod_get_min_voffset(wcmod_st* ws); 
int _wcmod_get_init_voffset(wcmod_st* ws); 
int _wcmod_get_max_hoffset_left(wcmod_st* ws);
int _wcmod_get_max_hoffset_right(wcmod_st* ws);
int _wcmod_start_livelink_diagnostics(wcmod_st* ws);
int _wcmod_start_deadlink_diagnostics(wcmod_st* ws);
int _wcmod_stop_livelink_diagnostics(wcmod_st* ws);
int _wcmod_clear_livelink_diagnostics(wcmod_st* ws);
int _wcmod_read_livelink_diagnostics(wcmod_st* ws);
int _wcmod_read_deadlink_diagnostics(wcmod_st* ws);

/* to hook up other info debug functions. comment out for now 
extern int wcmod_info_all(wcmod_st * ws) ;
extern int wcmod_info_all_lanes(wcmod_st * ws) ;
*/


static char *SPEED_NAME[] = {
"10M",
"100M",
"1G",
"2p5G",
"5G_X4",
"6G_X4",
"10G_HiG",
"10G_CX4",
"12G_HiG",
"12p5G_X4",
"13G_X4",
"15G_X4",
"16G_X4",
"1G_KX",
"10G_KX4",
"10G_KR",
"5G",
"6p4G",
"20G_X4",
"21G_X4",
"25G_X4",
"10G_HiG_DXGXS",
"10G_DXGXS",
"10p5G_HiG_DXGXS",
"10p5G_DXGXS",
"12p773G_HiG_DXGXS",
"12p773G_DXGXS",
"10G_XFI",
"40G",
"20G_HiG_DXGXS",
"20G_DXGXS",
"10G_SFI",
"31p5G",
"32p7G",
"20G_SCR",
"10G_HiG_DXGXS_SCR",
"10G_DXGXS_SCR",
"12G_R2",
"10G_X2",
"40G_KR4",
"40G_CR4",
"100G_CR10"
};

#define WC_DIAG_MSG_BUFFER_SIZE 2048
#define WC_DIAG_MAX_NUM_APPEND(str) (WC_DIAG_MSG_BUFFER_SIZE - strlen(str) - 1)

char *getSpeedString(int speed)
{
  if (speed < 0)
    return NULL;
  
  if (speed > DIGITAL5_ACTUAL_SPEED_ACTUAL_SPEED_TX_dr_100G_CR10)
    return NULL;
  return SPEED_NAME[speed];
}

char* an_adv0_print(uint16 data)
{
  static char p[100]; strcpy (p,"");
  if (data & AN_IEEE1BLK_AN_ADVERTISEMENT0_PAUSE_pause_MASK) {
    strcpy (p, "pause");
  } else {
    strcpy (p, "No pause");
  }
  if (data & AN_IEEE1BLK_AN_ADVERTISEMENT0_PAUSE_asm_dir_MASK) {
    strcat (p, "asymmetric direction");
  }
  return (p);
}

char* an_adv1_print(uint16 data)
{
  static char p[100]; strcpy (p,"");
  if (data & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_100G_CR10_MASK) {
    strcat (p,"\t 100G CR10");
  }
  if (data & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_40G_CR4_MASK) {
    strcat (p,"\t 40G CR4");
  }
  if (data & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_40G_KR4_MASK) {
    strcat (p,"\t 40G KR4");
  }
  if (data & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_10G_KR_MASK) {
    strcat (p,"\t 10G KR");
  }
  if (data & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_10G_KX4_MASK) {
    strcat (p,"\t 10G KX4");
  }
  if (data & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_1G_KX_MASK) {
    strcat (p,"\t 1G KX");
  }
  return p;
}

char* an_adv2_print(uint16 data)
{
  static char p[100]; strcpy (p,"");
  if (data & AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_fec_requested_MASK) {
    strcpy (p, "FEC requested");
  } else {
    strcpy (p, "FEC NOT requested.");
  }
  if (data & AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_fec_ability_MASK) {
    strcat (p, "FEC Ability");
  } else {
    strcat (p, "No FEC Ability");
  }
  return p;
}

int wcmod_diag_tx_loopback(wcmod_st *ws)
{
  uint16 enable = 0;
  uint16 tmp_lane;
  if (ws->this_lane >= 4) {
    tmp_lane = ws->this_lane;
    ws->this_lane = 4;
  READ_WC40_XGXSBLK1_LANECTRL2r(ws->unit,ws,&enable);
    ws->this_lane = tmp_lane;
  } else {
  	ws->lane_num_ignore = 1;
    READ_WC40_XGXSBLK1_LANECTRL2r(ws->unit,ws,&enable);
	ws->lane_num_ignore = 0;
  }
  ws->accData = enable;
  return SOC_E_NONE;
}

int wcmod_diag_rx_loopback(wcmod_st *ws)
{
  uint16 data16;
  uint16 value = 0;

  wcmod_spd_intfc_set spd_intf = ws->spd_intf;

  if ((spd_intf == WCMOD_SPD_10000_XFI) || (spd_intf == WCMOD_SPD_10000_SFI) ||
      (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5)) {
        SOC_IF_ERROR_RETURN(READ_WC40_TX66_CONTROLr(ws->unit, ws, &data16));
        value = (data16 & TX66_CONTROL_RLOOP_EN_MASK) >> TX66_CONTROL_RLOOP_EN_SHIFT;
  } else if ((spd_intf == WCMOD_SPD_10000_HI) || (spd_intf == WCMOD_SPD_10000) ||
           (spd_intf == WCMOD_SPD_R2_12000) || (spd_intf == WCMOD_SPD_13000) ||
           (spd_intf == WCMOD_SPD_16000)   || (spd_intf == WCMOD_SPD_20000) ||
           (spd_intf == WCMOD_SPD_21000)   || (spd_intf == WCMOD_SPD_25455) ||
           (spd_intf == WCMOD_SPD_31500)   || (spd_intf == WCMOD_SPD_40G_KR4) ||
           (spd_intf == WCMOD_SPD_40G_XLAUI) || (spd_intf == WCMOD_SPD_42G_XLAUI) ||
           (spd_intf == WCMOD_SPD_15750_HI_DXGXS) || (spd_intf == WCMOD_SPD_40G) ||
           (spd_intf == WCMOD_SPD_20000_HI_DXGXS) || (spd_intf == WCMOD_SPD_42G) || 
           (spd_intf == WCMOD_SPD_10000_DXGXS) || (spd_intf == WCMOD_SPD_X2_10000) || 
           (spd_intf == WCMOD_SPD_X2_23000)) {

        READ_WC40_XGXSBLK0_XGXSCONTROLr(ws->unit, ws, &data16);
        value = (data16 & XGXSBLK0_XGXSCONTROL_RLOOP_MASK) >> XGXSBLK0_XGXSCONTROL_RLOOP_SHIFT;

  } else if ((spd_intf == WCMOD_SPD_1000_FIBER) || (spd_intf == WCMOD_SPD_10_SGMII)
         || (spd_intf == WCMOD_SPD_100_SGMII) || (spd_intf == WCMOD_SPD_1000_SGMII)){
        READ_WC40_SERDESDIGITAL_CONTROL1000X1r(ws->unit, ws, &data16);
        value = (data16 & SERDESDIGITAL_CONTROL1000X1_REMOTE_LOOPBACK_MASK) >> 
                 SERDESDIGITAL_CONTROL1000X1_REMOTE_LOOPBACK_SHIFT;
  }
  ws->accData = value;
  ws->lane_num_ignore = 0;

  return SOC_E_NONE;
}

int wcmod_diag_rx_pcs_bypass_loopback(wcmod_st *ws)
{
  uint16 mask16;
  uint16 data16;
  uint16 value = 0;

  ws->lane_num_ignore = 1;

  mask16 = 1 << (XGXSBLK1_LANECTRL2_RLOOP1G_SHIFT + ws->this_lane);
  SOC_IF_ERROR_RETURN(READ_WC40_XGXSBLK1_LANECTRL2r(ws->unit, ws, &data16));
  value = (mask16 & data16)? TRUE: FALSE;
  ws->accData = value;
  ws->lane_num_ignore = 0;

  return SOC_E_NONE;
}


int wcmod_diag_speed(wcmod_st *ws)
{
  uint16 speed_val = 0;
  
  switch (ws->lane_select) {
  case WCMOD_LANE_0_0_0_1:
      if ((ws->model_type == WCMOD_XN) || (ws->model_type == WCMOD_QS_A0) || (ws->model_type == WCMOD_QS)) {
        if (ws->port_type == WCMOD_COMBO) {
              SOC_IF_ERROR_RETURN
                (READ_WC40_XGXSBLK4_XGXSSTATUS1r(ws->unit, ws, &speed_val));
              speed_val &= 0xf;
        } else {
              SOC_IF_ERROR_RETURN
                (READ_WC40_SERDESDIGITAL_STATUS1000X1r(ws->unit, ws, &speed_val));
               speed_val &= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_MASK;
               speed_val = (speed_val >> SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_SHIFT);
               speed_val &= 0x3;
       }
      } else {
          SOC_IF_ERROR_RETURN(READ_WC40_GP2_REG_GP2_2r(ws->unit,ws, &speed_val));
          speed_val &= WL_GP2_REG_GP2_2_ACTUAL_SPEED_LN0_MASK;
          speed_val >>= GP2_REG_GP2_2_ACTUAL_SPEED_LN0_SHIFT;
      }
      break;
  case WCMOD_LANE_0_0_1_0:
      if ((ws->model_type == WCMOD_XN) || (ws->model_type == WCMOD_QS_A0) || (ws->model_type == WCMOD_QS)) {
        if (ws->port_type == WCMOD_COMBO) {
              SOC_IF_ERROR_RETURN
                (READ_WC40_XGXSBLK4_XGXSSTATUS1r(ws->unit, ws, &speed_val));
              speed_val &= 0xf;
        } else {
              SOC_IF_ERROR_RETURN
                (READ_WC40_SERDESDIGITAL_STATUS1000X1r(ws->unit, ws, &speed_val));
               speed_val &= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_MASK;
               speed_val = (speed_val >> SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_SHIFT);
               speed_val &= 0x3;
        }        
      } else {
          SOC_IF_ERROR_RETURN(READ_WC40_GP2_REG_GP2_2r(ws->unit,ws, &speed_val));
          speed_val &= WL_GP2_REG_GP2_2_ACTUAL_SPEED_LN1_MASK;
          speed_val >>= GP2_REG_GP2_2_ACTUAL_SPEED_LN1_SHIFT;
      }
      break;
  case WCMOD_LANE_0_1_0_0:
      if ((ws->model_type == WCMOD_XN) || (ws->model_type == WCMOD_QS_A0) || (ws->model_type == WCMOD_QS)) {
        if (ws->port_type == WCMOD_COMBO) {
              SOC_IF_ERROR_RETURN
                (READ_WC40_XGXSBLK4_XGXSSTATUS1r(ws->unit, ws, &speed_val));
              speed_val &= 0xf;
        } else {
              SOC_IF_ERROR_RETURN
                (READ_WC40_SERDESDIGITAL_STATUS1000X1r(ws->unit, ws, &speed_val));
               speed_val &= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_MASK;
               speed_val = (speed_val >> SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_SHIFT);
               speed_val &= 0x3;
        }        
      } else {
          SOC_IF_ERROR_RETURN(READ_WC40_GP2_REG_GP2_3r(ws->unit,ws, &speed_val));
          speed_val &= WL_GP2_REG_GP2_3_ACTUAL_SPEED_LN2_MASK;
          speed_val >>= GP2_REG_GP2_3_ACTUAL_SPEED_LN2_SHIFT;
      }
      break;
  case WCMOD_LANE_1_0_0_0:
      if ((ws->model_type == WCMOD_XN) || (ws->model_type == WCMOD_QS_A0) || (ws->model_type == WCMOD_QS)) {
        if (ws->port_type == WCMOD_COMBO) {
              SOC_IF_ERROR_RETURN
                (READ_WC40_XGXSBLK4_XGXSSTATUS1r(ws->unit, ws, &speed_val));
              speed_val &= 0xf;
        } else {
              SOC_IF_ERROR_RETURN
                (READ_WC40_SERDESDIGITAL_STATUS1000X1r(ws->unit, ws, &speed_val));
               speed_val &= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_MASK;
               speed_val = (speed_val >> SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_SHIFT);
               speed_val &= 0x3; 
        }
      } else {
          SOC_IF_ERROR_RETURN(READ_WC40_GP2_REG_GP2_3r(ws->unit,ws, &speed_val));
          speed_val &= WL_GP2_REG_GP2_3_ACTUAL_SPEED_LN3_MASK;
          speed_val >>= GP2_REG_GP2_3_ACTUAL_SPEED_LN3_SHIFT;
      }
      break;
  case WCMOD_LANE_0_0_0_1_0_0_0_0:
      SOC_IF_ERROR_RETURN
         (READ_WC40_SERDESDIGITAL_STATUS1000X1r(ws->unit, ws, &speed_val));
      speed_val &= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_MASK;
      speed_val = (speed_val >> SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_SHIFT);
      speed_val &= 0x3; 
      break;
  case WCMOD_LANE_0_0_1_0_0_0_0_0:
      SOC_IF_ERROR_RETURN
         (READ_WC40_SERDESDIGITAL_STATUS1000X1r(ws->unit, ws, &speed_val));
      speed_val &= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_MASK;
      speed_val = (speed_val >> SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_SHIFT);
      speed_val &= 0x3; 
      break;
  case WCMOD_LANE_0_1_0_0_0_0_0_0:
      SOC_IF_ERROR_RETURN
         (READ_WC40_SERDESDIGITAL_STATUS1000X1r(ws->unit, ws, &speed_val));
      speed_val &= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_MASK;
      speed_val = (speed_val >> SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_SHIFT);
      speed_val &= 0x3; 
      break;
  case WCMOD_LANE_1_0_0_0_0_0_0_0:
      SOC_IF_ERROR_RETURN
         (READ_WC40_SERDESDIGITAL_STATUS1000X1r(ws->unit, ws, &speed_val));
      speed_val &= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_MASK;
      speed_val = (speed_val >> SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_SHIFT);
      speed_val &= 0x3; 
      break;
  default:
      return SOC_E_ERROR;
  }
  ws->accData = speed_val;
  return SOC_E_NONE;
}


/*!
  An autoneg diagnostic function.
 */
int wcmod_diag_autoneg(wcmod_st *ws)
{
  uint16 data;
  uint16 mii_stat;
  char msg[WC_DIAG_MSG_BUFFER_SIZE] = {0};
  int an_done = 0;
  ws->accData = 0;

  /* Check clause-37 autoneg */
  READ_WC40_IEEE0BLK_MIICNTLr(ws->unit, ws, &data);
  if (data & IEEE0BLK_MIICNTL_AUTONEG_ENABLE_MASK) {
    strcat(msg, "CL37 autoneg enabled in IEEE0BLK_MIICNTL_NAME\n");
    ws->accData = AUTONEG_ENABLE_MASK;
  } else {
    strcat(msg, "CL37 autoneg NOT enabled in IEEE0BLK_MIICNTL_NAME\n");
  }

  /* Check clause-73 autoneg */
  READ_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(ws->unit, ws, &data);
  if (data & AN_IEEE0BLK_AN_IEEECONTROL1_AN_ENABLE_MASK) {
    strcat(msg,"CL73 autoneg enabled in AN_IEEE0BLK_AN_IEEECONTROL1_NAME\n");
    ws->accData |= AUTONEG_ENABLE_MASK;
  } else {
   strcat(msg,"CL73 autoneg NOT enabled in AN_IEEE0BLK_AN_IEEECONTROL1_NAME\n");
  }

  /* Check BAM */
  READ_WC40_CL73_USERB0_CL73_BAMCTRL1r(ws->unit, ws, &data);

  if (data & CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK) {
    strcat(msg,"CL73 BAM enabled in CL73_USERB0_CL73_BAMCTRL1_NAME \n");
    ws->accData |= AUTONEG_ENABLE_MASK;
  } else {
    strcat(msg,"CL73 BAM NOT enabled in CL73_USERB0_CL73_BAMCTRL1_NAME\n");
  }

  /* AN status only if the an is enabled */
  if (ws->accData == AUTONEG_ENABLE_MASK) {
      /*first check cl37 an status */
        SOC_IF_ERROR_RETURN
            (READ_WC40_GP2_REG_GP2_4r(ws->unit, ws, &mii_stat)); 
        an_done = (mii_stat & (1 << (ws->this_lane + GP2_REG_GP2_4_AUTONEG_COMPLETE_SHIFT))) ? TRUE: FALSE;
        if (an_done) {
            strcat(msg, "CL37 AN complete\n");
            ws->accData |= AUTONEG_DONE_MASK;
        } else {
            /* next check cl73 an status bit */  
            SOC_IF_ERROR_RETURN
                (READ_WC40_GP2_REG_GP2_4r(ws->unit, ws, &mii_stat)); 
            an_done = (mii_stat & (1 << (ws->this_lane + GP2_REG_GP2_4_CL73_AUTONEG_COMPLETE_SHIFT))) ? TRUE: FALSE;
            if (an_done) {
                strcat(msg, "CL73 AN complete\n");
                ws->accData |= AUTONEG_DONE_MASK;
            }
        }
  }
  if ((!an_done) && (ws->accData == AUTONEG_ENABLE_MASK))  {
      strcat(msg, "Neither CL37 nor CL73 AN complete\n");
  }

  /* AN status */
  READ_WC40_XGXSBLK4_TOPANSTATUS1r(ws->unit, ws, &data);

  if (data & XGXSBLK4_TOPANSTATUS1_CL73_MR_LP_AUTONEG_ABLE_MASK) {
    strcat(msg, "Link partner supports AN\n");
  } else {
    strcat(msg, "Link partner does NOT support AN\n");
  }

  if (data & XGXSBLK4_TOPANSTATUS1_LINK_STATUS_MASK) {
    strcat(msg, "Link is up\n");
  } else {
    strcat(msg, "Link is down\n");
  }

  if (ws->verbosity > 1) printf("%s\n", msg); strcpy (msg, "");

  /* Advertised abilities */
  /* advertised Local device */
  strcpy(msg, "AN supports the following (local):\n");
  /* Adv 0 reg */
  READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT0r(ws->unit, ws, &data);
  if (ws->verbosity > 1) sal_snprintf ((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "%s",an_adv0_print(data));

  /* Adv 1 reg */
  READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT1r(ws->unit, ws, &data);
  if (ws->verbosity > 1) sal_snprintf ((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "%s",an_adv1_print(data));

  /* Adv 2 reg */
  READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT2r(ws->unit, ws, &data);
  if (ws->verbosity > 1) sal_snprintf ((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "%s",an_adv2_print(data));

  if (ws->verbosity > 1) printf("%s\n", msg); strcpy (msg, "");

  /* advertised link partner */
  strcpy (msg, "AN supports the following (link partner):\n");
  if (ws->verbosity > 1) sal_snprintf ((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "%s",an_adv0_print(data));

  /* Adv 0 reg */
  READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY0r(ws->unit, ws, &data);
  if (ws->verbosity > 1) sal_snprintf ((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "%s",an_adv0_print(data));

  /* Adv 1 reg */
  READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY1r(ws->unit, ws, &data);
  if (ws->verbosity > 1) sal_snprintf ((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "%s",an_adv1_print(data));

  /* Adv 2 reg */
  READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY2r(ws->unit, ws, &data);
  if (ws->verbosity > 1) sal_snprintf ((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "%s",an_adv2_print(data));

  if (ws->verbosity > 1) printf("%s", msg);
  return SOC_E_NONE;
}

int wcmod_diag_topology(wcmod_st *ws)
{
  uint16 data;
  unsigned int laneSwapVal;
  unsigned int shift;
  char msg[WC_DIAG_MSG_BUFFER_SIZE] = {0};
  int n;
  int str_length;

  /* Check R-loop */
  ws->lane_num_ignore = 1;
  READ_WC40_XGXSBLK0_XGXSCONTROLr(ws->unit, ws, &data);
  ws->lane_num_ignore = 0;

  if (data & XGXSBLK0_XGXSCONTROL_RLOOP_MASK) {
    str_length = strlen("Rloop enabled in XGXSBLK0_XGXSCONTROL_NAME\n"); 
    strncpy(msg, "Rloop enabled in XGXSBLK0_XGXSCONTROL_NAME\n", str_length);
  } else {
    str_length = strlen("Rloop NOT enabled in XGXSBLK0_XGXSCONTROL_NAME\n"); 
    strncpy(msg, "Rloop NOT enabled in XGXSBLK0_XGXSCONTROL_NAME\n", str_length);
  }
  /* Check G-loop */
  READ_WC40_COMBO_IEEE0_MIICNTLr(ws->unit, ws, &data);
  if (data & COMBO_IEEE0_MIICNTL_GLOOPBACK_MASK) {
    sal_strncat(msg, "Gloop enabled in COMBO_IEEE0_MIICNTL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg, "Gloop NOT enabled in COMBO_IEEE0_MIICNTL_NAME\n",
             WC_DIAG_MAX_NUM_APPEND(msg));
  }
  ws->lane_num_ignore = 1;
  READ_WC40_XGXSBLK1_LANECTRL2r(ws->unit, ws, &data);
  ws->lane_num_ignore = 0;

  for (n = 0; n < 4; ++n) {
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "Lane %d: ", n);

    /* Check R-loop 1g */
    if (data & (0x1u << (XGXSBLK1_LANECTRL2_RLOOP1G_SHIFT + n))) {
      sal_strncat(msg, "Rloop-1G     enabled, ", 
              WC_DIAG_MAX_NUM_APPEND(msg));
    } else {
      sal_strncat(msg, "Rloop-1G NOT enabled, ",
              WC_DIAG_MAX_NUM_APPEND(msg));
    }

    /* Check G-loop 1g -- reads the same register as r-loop 1g */
    if (data & (0x1u << (XGXSBLK1_LANECTRL2_GLOOP1G_SHIFT + n))) {
      sal_strncat(msg, "Gloop-1G     enabled in XGXSBLK1_LANECTRL2_NAME\n",
              WC_DIAG_MAX_NUM_APPEND(msg));
    } else {
      sal_strncat(msg, "Gloop-1G NOT enabled in XGXSBLK1_LANECTRL2_NAME\n",
              WC_DIAG_MAX_NUM_APPEND(msg));
    }
  }

  /* Check TX lane swap values */
  /* Check TX digital-to-analog lane swap */
  ws->lane_num_ignore = 1;
  READ_WC40_XGXSBLK8_TXLNSWAP1r(ws->unit, ws, &data);
  ws->lane_num_ignore = 0;

  sal_strncat(msg,"TX digital-to-analog lane swaps in XGXSBLK8_TXLNSWAP1_NAME\n",
          WC_DIAG_MAX_NUM_APPEND(msg));
  for(n = 0; n < 4; ++n) {
    shift = (XGXSBLK8_TXLNSWAP1_TX0_LNSWAP_SEL_SHIFT + 2 * n);
    laneSwapVal=(data & (XGXSBLK8_TXLNSWAP1_TX0_LNSWAP_SEL_MASK << shift)) >> shift;
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "Lane %d -> Lane %d\n", n, laneSwapVal);
  }

  /* Check TX analog-to-digital lane swap */
  ws->lane_num_ignore = 1;
  READ_WC40_XGXSBLK8_TXLNSWAP2r(ws->unit, ws, &data);
  ws->lane_num_ignore = 0;

  sal_strncat(msg, "TX analog-to-digital lane swap in XGXSBLK8_TXLNSWAP2_NAME\n",
          WC_DIAG_MAX_NUM_APPEND(msg));
  for(n = 0; n < 4; ++n) {
    shift = (XGXSBLK8_TXLNSWAP2_TX0_ANA_LNSWAP_SEL_SHIFT + 2 * n);
    laneSwapVal = (data & (XGXSBLK8_TXLNSWAP2_TX0_ANA_LNSWAP_SEL_MASK << shift)) >> shift;
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "Lane %d -> Lane %d\n", n, laneSwapVal);
  }

  /* Check RX lane swap values */
  /* Check RX digital-to-analog lane swap */
  ws->lane_num_ignore = 1;
  READ_WC40_XGXSBLK8_RXLNSWAP1r(ws->unit, ws, &data);
  ws->lane_num_ignore = 0;

  sal_strncat(msg, "RX analog-to-digital lane swaps in XGXSBLK8_RXLNSWAP1_NAME\n",
          WC_DIAG_MAX_NUM_APPEND(msg));
  for(n = 0; n < 4; ++n) {
    shift = (XGXSBLK8_RXLNSWAP1_RX0_LNSWAP_SEL_SHIFT + 2 * n);
    laneSwapVal = (data & (XGXSBLK8_RXLNSWAP1_RX0_LNSWAP_SEL_MASK << shift)) >> shift;
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "Lane %d -> Lane %d\n", n, laneSwapVal);
  }

  /* RX analog-to-digital lane swap */
  ws->lane_num_ignore = 1;
  READ_WC40_XGXSBLK8_RXLNSWAP2r(ws->unit, ws, &data);
  ws->lane_num_ignore = 0;

  sal_strncat(msg, "RX digital-to-analog lane swap in XGXSBLK8_RXLNSWAP2_NAME\n",
          WC_DIAG_MAX_NUM_APPEND(msg));
  for(n = 0; n < 4; ++n) {
    shift = (XGXSBLK8_RXLNSWAP2_RX0_DIG_LNSWAP_SEL_SHIFT + 2 * n);
    laneSwapVal = (data & (XGXSBLK8_RXLNSWAP2_RX0_DIG_LNSWAP_SEL_MASK << shift)) >> shift;
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "Lane %d -> Lane %d\n", n, laneSwapVal);
  }

  /* TX polarity Lane 0 */
  READ_WC40_TX0_ANATXACONTROL0r(ws->unit, ws, &data);

  if (data & TX0_ANATXACONTROL0_TXPOL_FLIP_MASK) {
    sal_strncat(msg, "Lane 0: TX polarity flipped in TX0_ANATXACONTROL0_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg, "Lane 0: TX polarity NOT flipped in TX0_ANATXACONTROL0_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* RX polarity Lane 0 */
  READ_WC40_RX0_ANARXCONTROLPCIr(ws->unit, ws, &data);

  if (data & RX0_ANARXCONTROLPCI_RX_POLARITY_R_MASK) {
    sal_strncat(msg,"Lane 0: RX polarity flipped in RX0_ANARXCONTROLPCI_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg,"Lane 0: RX polarity NOT flipped in RX0_ANARXCONTROLPCI_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* TX polarity Lane 1 */
  READ_WC40_TX1_ANATXACONTROL0r(ws->unit, ws, &data);

  if (data & TX1_ANATXACONTROL0_TXPOL_FLIP_MASK) {
    sal_strncat(msg,"Lane 1: TX polarity flipped in TX1_ANATXACONTROL0_NAME \n",
           WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg,"Lane 1: TX polarity NOT flipped in TX1_ANATXACONTROL0_NAME\n",
           WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* RX polarity Lane 1 */
  READ_WC40_RX1_ANARXCONTROLPCIr(ws->unit, ws, &data);

  if (data & RX1_ANARXCONTROLPCI_RX_POLARITY_R_MASK) {
    sal_strncat(msg,"Lane 1: RX polarity     flipped in RX1_ANARXCONTROLPCI_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg,"Lane 1: RX polarity NOT flipped in RX1_ANARXCONTROLPCI_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* TX polarity Lane 2 */
  READ_WC40_TX2_ANATXACONTROL0r(ws->unit, ws, &data);

  if (data & TX2_ANATXACONTROL0_TXPOL_FLIP_MASK) {
    sal_strncat(msg,"Lane 2: TX polarity     flipped in TX2_ANATXACONTROL0_NAME \n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg,"Lane 2: TX polarity NOT flipped in TX2_ANATXACONTROL0_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* RX polarity Lane 2 */
  READ_WC40_RX2_ANARXCONTROLPCIr(ws->unit, ws, &data);

  if (data & RX2_ANARXCONTROLPCI_RX_POLARITY_R_MASK) {
    sal_strncat(msg,"Lane 2: RX polarity     flipped in RX2_ANARXCONTROLPCI_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg,"Lane 2: RX polarity NOT flipped in RX2_ANARXCONTROLPCI_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* TX polarity Lane 3 */
  READ_WC40_TX3_ANATXACONTROL0r(ws->unit, ws, &data);

  if (data & TX3_ANATXACONTROL0_TXPOL_FLIP_MASK) {
    sal_strncat(msg,"Lane 3: TX polarity     flipped in TX3_ANATXACONTROL0_NAME \n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg,"Lane 3: TX polarity NOT flipped in TX3_ANATXACONTROL0_NAME \n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* RX polarity Lane 3 */
  READ_WC40_RX3_ANARXCONTROLPCIr(ws->unit, ws, &data);

  if (data & RX3_ANARXCONTROLPCI_RX_POLARITY_R_MASK) {
    sal_strncat(msg,"Lane 3: RX polarity     flipped in RX3_ANARXCONTROLPCI_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg,"Lane 3: RX polarity NOT flipped in RX3_ANARXCONTROLPCI_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }
  printf("%s\n", msg);
  return SOC_E_NONE;
}

int wcmod_diag_general(wcmod_st *ws)
{
  uint16 data;
  char msg[WC_DIAG_MSG_BUFFER_SIZE] = {0};

  /* Read rev Id */
  READ_WC40_SERDESID_SERDESID0r(ws->unit, ws, &data);

  strcpy(msg, "Revision Id = ");
  switch ((data & SERDESID_SERDESID0_REV_LETTER_MASK) >> SERDESID_SERDESID0_REV_LETTER_SHIFT) {
  case 0:  sal_strncat(msg, "A", WC_DIAG_MAX_NUM_APPEND(msg)); break;
  case 1:  sal_strncat(msg, "B", WC_DIAG_MAX_NUM_APPEND(msg)); break;
  case 2:  sal_strncat(msg, "C", WC_DIAG_MAX_NUM_APPEND(msg)); break;
  default: sal_strncat(msg, "UNKNOWN", WC_DIAG_MAX_NUM_APPEND(msg)); break;
  }
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "Rev Num: %d\n",
                                 (data & SERDESID_SERDESID0_REV_NUMBER_MASK) >>
                                  SERDESID_SERDESID0_REV_NUMBER_SHIFT);
  printf("%s", msg); strcpy (msg, "");

  sal_strncpy_s (msg, "Model = ", sizeof(msg));
  switch((data & SERDESID_SERDESID0_MODEL_NUMBER_MASK)
          >> SERDESID_SERDESID0_MODEL_NUMBER_SHIFT) {
  case MODEL_SERDES_CL73_COMBO_CORE:
                             sal_strncat(msg, "Serdes_cl73_combo_core", WC_DIAG_MAX_NUM_APPEND(msg)); break;
  case MODEL_XGXS_16G    :   sal_strncat(msg, "XGXS_16G",  WC_DIAG_MAX_NUM_APPEND(msg));               break;
  case MODEL_HYPERCORE   :   sal_strncat(msg, "Hypercore", WC_DIAG_MAX_NUM_APPEND(msg));              break;
  case MODEL_HYPERLITE   :   sal_strncat(msg, "Hyperlite", WC_DIAG_MAX_NUM_APPEND(msg));              break;
  case MODEL_PCIE_G2_PIPE:   sal_strncat(msg, "PCIE_G2_PIPE", WC_DIAG_MAX_NUM_APPEND(msg));           break;
  case MODEL_1p25GBd_SERDES: sal_strncat(msg, "1.25GBd_Serdes", WC_DIAG_MAX_NUM_APPEND(msg));         break;
  case MODEL_SATA2      :    sal_strncat(msg, "Sata2", WC_DIAG_MAX_NUM_APPEND(msg));                  break;
  case MODEL_QSGMII     :    sal_strncat(msg, "QSGMII", WC_DIAG_MAX_NUM_APPEND(msg));                 break;
  case MODEL_XGXS10G    :    sal_strncat(msg, "XGXS10G", WC_DIAG_MAX_NUM_APPEND(msg));                break;
  case MODEL_WARPCORE   :    sal_strncat(msg, "Warpcore", WC_DIAG_MAX_NUM_APPEND(msg));               break;
  case MODEL_XGXS_CL73  :    sal_strncat(msg, "XGXS_CL73", WC_DIAG_MAX_NUM_APPEND(msg));              break;
  case MODEL_SERDES_CL73:    sal_strncat(msg, "Serdes_cl73", WC_DIAG_MAX_NUM_APPEND(msg));            break;
  case MODEL_WARPLITE   :    sal_strncat(msg, "WARPLITE", WC_DIAG_MAX_NUM_APPEND(msg));               break;
  case MODEL_QUADSGMII  :    sal_strncat(msg, "QUADSGMII", WC_DIAG_MAX_NUM_APPEND(msg));              break;
  case MODEL_WARPCORE_C :    sal_strncat(msg, "WARPCORE_C", WC_DIAG_MAX_NUM_APPEND(msg));             break;
  default               :    sal_strncat(msg, "UNKNOWN", WC_DIAG_MAX_NUM_APPEND(msg));                break;
  }
  printf("%s\n", msg); strncpy (msg, "", 1);

  /* Check PLL status */
  READ_WC40_PLL_ANAPLLSTATUSr(ws->unit, ws, &data);

  /* PLL startup sequence */
  if (data & PLL_ANAPLLSTATUS_PLLSEQDONE_MASK) {
    sal_strncpy_s (msg,"PLL startup sequence done\n", sizeof(msg));
  } else {
    sal_strncpy_s (msg,"PLL startup sequence not done\n", sizeof(msg));
  }

  if (data & PLL_ANAPLLSTATUS_PLLSEQPASS_MASK) {
    sal_strncat (msg,"PLL startup sequence passed and PLL is locked\n",
             WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat (msg,"PLL startup sequence FAILED. PLL Lock status UNKNOWN\n",
             WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* PLL frequence detection sequence */
  if (data & PLL_ANAPLLSTATUS_FREQDONE_MASK) {
    sal_strncat (msg,"PLL frequency detection sequence done\n",
             WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat (msg,"PLL frequency detection sequence NOT done\n",
             WC_DIAG_MAX_NUM_APPEND(msg));
  }

  if (data & PLL_ANAPLLSTATUS_FREQPASS_MASK) {
    sal_strncat (msg,"PLL frequency detection sequence passed\n",
             WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat (msg,"PLL frequency detection sequence FAILED\n",
             WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* VCO range selection sequence */
  if (data & PLL_ANAPLLSTATUS_VCODONE_MASK) {
    sal_strncat (msg,"VCO range selection sequence done\n",
             WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat (msg,"VCO range selection sequence NOT done\n",
             WC_DIAG_MAX_NUM_APPEND(msg));
  }

  if (data & PLL_ANAPLLSTATUS_VCOPASS_MASK) {
    sal_strncat (msg,"VCO range selection sequence passed\n",
             WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat (msg,"VCO range selection sequence FAILED\n",
             WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* PLL lock history check. Comment out. Folks just get confused.
  if (data & PLL_ANAPLLSTATUS_LOSTPLLLOCK_SM_MASK) {
    sal_strncat (msg,"PLL NOT locked\n");
  } else {
    sal_strncat (msg,"PLL locked\n");
  }
  */
  printf("%s", msg); strncpy (msg, "", 1);

  /* Frequency divider ratio */
  sal_strncpy_s (msg,"PLL frequency divider = ", sizeof(msg));
  switch((data & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK) >> PLL_ANAPLLSTATUS_PLL_MODE_AFE_SHIFT) {
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div32: sal_strncat(msg,"32",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div36: sal_strncat(msg,"36",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div40: sal_strncat(msg,"40",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div42: sal_strncat(msg,"42",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div48: sal_strncat(msg,"48",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div50: sal_strncat(msg,"50",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div52: sal_strncat(msg,"52",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div54: sal_strncat(msg,"54",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div60: sal_strncat(msg,"60",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div64: sal_strncat(msg,"64",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div66: sal_strncat(msg,"66",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div68: sal_strncat(msg,"68",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div70: sal_strncat(msg,"70",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div80: sal_strncat(msg,"80",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div92: sal_strncat(msg,"92",WC_DIAG_MAX_NUM_APPEND(msg));break;
   case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div100:sal_strncat(msg,"100",WC_DIAG_MAX_NUM_APPEND(msg));break;
   default:                              sal_strncat(msg,"UNKNOWN", WC_DIAG_MAX_NUM_APPEND(msg));break;
  }
  printf("%s", msg); strncpy (msg, "", 1);

  /* Check for PLL bypass */
  ws->lane_num_ignore = 1;
  READ_WC40_XGXSBLK0_XGXSCONTROLr(ws->unit, ws, &data);
  ws->lane_num_ignore = 0;

  if (data & XGXSBLK0_XGXSCONTROL_PLL_BYPASS_MASK) {
    sal_strncpy_s(msg,"PLL bypassedin XGXSBLK0_XGXSCONTROL_NAME\n", sizeof(msg));
  } else {
    sal_strncpy_s(msg,"PLL not bypassed in XGXSBLK0_XGXSCONTROL_NAME\n", sizeof(msg));
  }
  printf("%s", msg); strncpy (msg, "", 1);

  /* 8051 */
  READ_WC40_GP2_REG_GP2_8r(ws->unit, ws, &data);
  if (data & GP2_REG_GP2_8_MICRO_INIT_DONE_MASK) {
    sal_strncpy_s(msg,"Micro-controller init done", sizeof(msg));
  } else {
    sal_strncpy_s(msg,"Micro-controller init NOT done", sizeof(msg));
  }
  printf("%s", msg); strncpy (msg, "", 1);
  return SOC_E_NONE;
}

#define MII_STAT_LA             (1 << 2) /* Link Active */

int wcmod_diag_link(wcmod_st *ws)
{
    uint16 data, link, prevContents;
    int speed;
    char msg[WC_DIAG_MSG_BUFFER_SIZE] = {0};
    uint16      mii_stat;
    uint16      mask16;
    int cntl = ws->per_lane_control;

    /* pcs bypass mode link status check */
    if (cntl == 0x10) {
        if        (ws->this_lane == 0) { /*************** LANE 0 *****************/
            /* change RX control so that RX status reads out RX sequencer status */
            READ_WC40_RX0_ANARXCONTROLr(ws->unit, ws, &prevContents);

            data = (prevContents & ~RX0_ANARXCONTROL_STATUS_SEL_MASK) | (RX0_ANARXCONTROL_STATUS_SEL_sigdetStatus << RX0_ANARXCONTROL_STATUS_SEL_SHIFT);
            WRITE_WC40_RX0_ANARXCONTROLr(ws->unit, ws, data);

            /* now read RX status */
            READ_WC40_RX0_ANARXSTATUSr(ws->unit, ws, &data);

            mask16 = RX0_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK |
                   RX0_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_MASK;

            if ((data & mask16) == mask16) {
                ws->accData = TRUE;
            } else {
                ws->accData = FALSE;
            }
            /* restore RX control back to previous value */
            WRITE_WC40_RX0_ANARXCONTROLr(ws->unit, ws, prevContents);

            return SOC_E_NONE;
        } else if (ws->this_lane == 1) { /*************** LANE 1 *****************/
            /* change RX control so that RX status reads out RX sequencer status */
            READ_WC40_RX1_ANARXCONTROLr(ws->unit, ws, &prevContents);

            data = (prevContents & ~RX1_ANARXCONTROL_STATUS_SEL_MASK) | (RX1_ANARXCONTROL_STATUS_SEL_sigdetStatus << RX1_ANARXCONTROL_STATUS_SEL_SHIFT);
            WRITE_WC40_RX1_ANARXCONTROLr(ws->unit, ws, data);

            /* now read RX status */
            READ_WC40_RX1_ANARXSTATUSr(ws->unit, ws, &data);

            mask16 = RX1_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK |
                   RX1_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_MASK;

            if ((data & mask16) == mask16) {
                ws->accData = TRUE;
            } else {
                ws->accData = FALSE;
            }
            /* restore RX control back to previous value */
            WRITE_WC40_RX1_ANARXCONTROLr(ws->unit, ws, prevContents);

            return SOC_E_NONE;
        } else if (ws->this_lane == 2) { /*************** LANE 2 *****************/
            /* change RX control so that RX status reads out RX sequencer status */
            READ_WC40_RX2_ANARXCONTROLr(ws->unit, ws, &prevContents);

            data = (prevContents & ~RX2_ANARXCONTROL_STATUS_SEL_MASK) | (RX2_ANARXCONTROL_STATUS_SEL_sigdetStatus << RX2_ANARXCONTROL_STATUS_SEL_SHIFT);
            WRITE_WC40_RX2_ANARXCONTROLr(ws->unit, ws, data);

            /* now read RX status */
            READ_WC40_RX2_ANARXSTATUSr(ws->unit, ws, &data);

            mask16 = RX2_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK |
                   RX2_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_MASK;

            if ((data & mask16) == mask16) {
                ws->accData = TRUE;
            } else {
                ws->accData = FALSE;
            }
            /* restore RX control back to previous value */
            WRITE_WC40_RX2_ANARXCONTROLr(ws->unit, ws, prevContents);

            return SOC_E_NONE;

        } else if (ws->this_lane == 3) { /*************** LANE 2 *****************/
            /* change RX control so that RX status reads out RX sequencer status */
            READ_WC40_RX3_ANARXCONTROLr(ws->unit, ws, &prevContents);

            data = (prevContents & ~RX3_ANARXCONTROL_STATUS_SEL_MASK) | (RX3_ANARXCONTROL_STATUS_SEL_sigdetStatus << RX3_ANARXCONTROL_STATUS_SEL_SHIFT);
            WRITE_WC40_RX3_ANARXCONTROLr(ws->unit, ws, data);

            /* now read RX status */
            READ_WC40_RX3_ANARXSTATUSr(ws->unit, ws, &data);

            mask16 = RX3_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK |
                   RX3_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_MASK;

            if ((data & mask16) == mask16) {
                ws->accData = TRUE;
            } else {
                ws->accData = FALSE;
            }
            /* restore RX control back to previous value */
            WRITE_WC40_RX3_ANARXCONTROLr(ws->unit, ws, prevContents);

            return SOC_E_NONE;

        } else {
            printf("%-22s: Error: invalid wcmod_diag_link %d\n",FUNCTION_NAME(), cntl);
            return SOC_E_ERROR;
        }
    }
    ws->accData = FALSE;
    /* link status get not for pcs bypass mode */
    if (cntl == 0x1) {
        /* Check XAUI link first if XAUI mode */
        SOC_IF_ERROR_RETURN
            (READ_WC40_PCS_IEEE0BLK_PCS_IEEESTATUS1r(ws->unit, ws, &mii_stat));
        if (mii_stat & MII_STAT_LA) {
            ws->accData = TRUE;
            return SOC_E_NONE;
        }

        /* Finally Check combo link */
        if (ws->accData == FALSE) {
            SOC_IF_ERROR_RETURN
                (READ_WC40_COMBO_IEEE0_MIISTATr(ws->unit, ws,  &mii_stat));
            if (mii_stat & MII_STAT_LA) {
                ws->accData = TRUE;
                return SOC_E_NONE;
            }
        }

        /* need to read the 10G  link state */
        SOC_IF_ERROR_RETURN
            (READ_WC40_XGXSBLK4_XGXSSTATUS1r(ws->unit, ws, &link));

        if (link & XGXSBLK4_XGXSSTATUS1_LINK10G_MASK) {
            ws->accData = TRUE;
            return SOC_E_NONE;
        } else {
            ws->accData = FALSE;
        }

        /* next check 10G XFI or KR */
         SOC_IF_ERROR_RETURN
            (READ_WC40_GP2_REG_GP2_1r(ws->unit, ws, &link));
        data = (link & GP2_REG_GP2_1_LINK_KR_MASK) >> GP2_REG_GP2_1_LINK_KR_SHIFT;
        mask16 = 1 << (ws->this_lane);  
        if (data & mask16) {  
            ws->accData = TRUE;
        } else {
            ws->accData = FALSE;
        }
        return SOC_E_NONE;
    } else {

        /* RX Sequencer */
        /* Lane 0 */
        /* change RX control so that RX status reads out RX sequencer status */
        READ_WC40_RX0_ANARXCONTROLr(ws->unit, ws, &prevContents);

        data = (prevContents & ~RX0_ANARXCONTROL_STATUS_SEL_MASK) | (RX0_ANARXCONTROL_STATUS_SEL_syncStatus << RX0_ANARXCONTROL_STATUS_SEL_SHIFT);
        WRITE_WC40_RX0_ANARXCONTROLr(ws->unit, ws, data);

        /* now read RX status */
        READ_WC40_RX0_ANARXSTATUSr(ws->unit, ws, &data);

        if (data & RX0_ANARXSTATUS_SYNC_STATUS_RXSEQDONE_MASK) {
        sal_strncat(msg, "Lane 0:  RX sequencer done\n",
                WC_DIAG_MAX_NUM_APPEND(msg));
        } else {
        sal_strncat(msg, "Lane 0:  RX sequencer NOT done\n",
                WC_DIAG_MAX_NUM_APPEND(msg));
        }

        if (data & RX0_ANARXSTATUS_SYNC_STATUS_SYNC_STATUS_MASK) {
        sal_strncat(msg, "\t     Synced\n", WC_DIAG_MAX_NUM_APPEND(msg));
        } else {
        sal_strncat(msg, "\t NOT Synced\n", WC_DIAG_MAX_NUM_APPEND(msg));
        }

        /* restore RX control back to previous value */
        WRITE_WC40_RX0_ANARXCONTROLr(ws->unit, ws, prevContents);

        /* Lane 1 */
        /* change RX control so that RX status reads out RX sequencer status */
        READ_WC40_RX1_ANARXCONTROLr(ws->unit, ws, &prevContents);

        data = (prevContents & ~RX1_ANARXCONTROL_STATUS_SEL_MASK) | (RX1_ANARXCONTROL_STATUS_SEL_syncStatus << RX1_ANARXCONTROL_STATUS_SEL_SHIFT);
        WRITE_WC40_RX1_ANARXCONTROLr(ws->unit, ws, data);

        /* now read RX status */
        READ_WC40_RX1_ANARXSTATUSr(ws->unit, ws, &data);

        if (data & RX1_ANARXSTATUS_SYNC_STATUS_RXSEQDONE_MASK) {
        sal_strncat(msg, "Lane 1:  RX sequencer done\n",
                WC_DIAG_MAX_NUM_APPEND(msg));
        } else {
        sal_strncat(msg, "Lane 1:  RX sequencer NOT done\n",
                WC_DIAG_MAX_NUM_APPEND(msg));
        }

        if (data & RX1_ANARXSTATUS_SYNC_STATUS_SYNC_STATUS_MASK) {
        sal_strncat(msg, "\t     Synced\n", WC_DIAG_MAX_NUM_APPEND(msg));
        } else {
        sal_strncat(msg, "\t NOT Synced\n", WC_DIAG_MAX_NUM_APPEND(msg));
        }
        /* restore RX control back to previous value */
        WRITE_WC40_RX1_ANARXCONTROLr(ws->unit, ws, prevContents);

        /* Lane 2 */
        /* change RX control so that RX status reads out RX sequencer status */
        READ_WC40_RX2_ANARXCONTROLr(ws->unit, ws, &prevContents);

        data = (prevContents & ~RX2_ANARXCONTROL_STATUS_SEL_MASK) | (RX2_ANARXCONTROL_STATUS_SEL_syncStatus << RX2_ANARXCONTROL_STATUS_SEL_SHIFT);
        WRITE_WC40_RX2_ANARXCONTROLr(ws->unit, ws, data);

        /* now read RX status */
        READ_WC40_RX2_ANARXSTATUSr(ws->unit, ws, &data);

        if (data & RX2_ANARXSTATUS_SYNC_STATUS_RXSEQDONE_MASK) {
        sal_strncat(msg, "Lane 2:  RX sequencer done\n",
                WC_DIAG_MAX_NUM_APPEND(msg));
        } else {
        sal_strncat(msg, "Lane 2:  RX sequencer NOT done\n",
                WC_DIAG_MAX_NUM_APPEND(msg));
        }

        if (data & RX2_ANARXSTATUS_SYNC_STATUS_SYNC_STATUS_MASK) {
        sal_strncat(msg, "\t     Synced\n", WC_DIAG_MAX_NUM_APPEND(msg));
        } else {
        sal_strncat(msg, "\t NOT Synced\n", WC_DIAG_MAX_NUM_APPEND(msg));
        }

        /* restore RX control back to previous value */
        WRITE_WC40_RX2_ANARXCONTROLr(ws->unit, ws, prevContents);

        /* Lane 3 */
        /* change RX control so that RX status reads out RX sequencer status */
        READ_WC40_RX3_ANARXCONTROLr(ws->unit, ws, &prevContents);

        data = (prevContents & ~RX3_ANARXCONTROL_STATUS_SEL_MASK) | (RX3_ANARXCONTROL_STATUS_SEL_syncStatus << RX3_ANARXCONTROL_STATUS_SEL_SHIFT);
        WRITE_WC40_RX3_ANARXCONTROLr(ws->unit, ws, data);

        /* now read RX status */
        READ_WC40_RX3_ANARXSTATUSr(ws->unit, ws, &data);

        if (data & RX3_ANARXSTATUS_SYNC_STATUS_RXSEQDONE_MASK) {
        sal_strncat(msg, "Lane 3:  RX sequencer done\n",
                WC_DIAG_MAX_NUM_APPEND(msg));
        } else {
        sal_strncat(msg, "Lane 3:  RX sequencer NOT done\n",
                WC_DIAG_MAX_NUM_APPEND(msg));
        }

        if (data & RX3_ANARXSTATUS_SYNC_STATUS_SYNC_STATUS_MASK) {
        sal_strncat(msg, "\t     Synced\n", WC_DIAG_MAX_NUM_APPEND(msg));
        } else {
        sal_strncat(msg, "\t NOT Synced\n", WC_DIAG_MAX_NUM_APPEND(msg));
        }

        /* restore RX control back to previous value */
        WRITE_WC40_RX3_ANARXCONTROLr(ws->unit, ws, prevContents);

        /* end RX Sequencer status check  */
        /* Look at OS, Ind/Combo mode */

        ws->lane_num_ignore = 1;
        READ_WC40_XGXSBLK0_XGXSCONTROLr(ws->unit, ws, &data);
        ws->lane_num_ignore = 0;
        sal_strncat(msg, "Mode = ", WC_DIAG_MAX_NUM_APPEND(msg));
        switch((data & XGXSBLK0_XGXSCONTROL_MODE_10G_MASK) >> XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT) {
        case xgxsBlk0_operationModes_IndLane_OS8: sal_strncat(msg, "Indep_OS8", WC_DIAG_MAX_NUM_APPEND(msg)); break;
        case xgxsBlk0_operationModes_IndLane_OS5:  sal_strncat(msg, "Indep_OS5", WC_DIAG_MAX_NUM_APPEND(msg)); break;
        case XGXSBLK0_XGXSCONTROL_MODE_10G_ComboCoreMode:sal_strncat(msg, "combo", WC_DIAG_MAX_NUM_APPEND(msg));break;
        default: sal_strncat(msg, "UNKNOWN", WC_DIAG_MAX_NUM_APPEND(msg)); break;
        }

        /* read the TX actual speed */
        wcmod_diag_speed(ws);

        speed = (ws->accData & DIGITAL5_ACTUAL_SPEED_ACTUAL_SPEED_TX_MASK) >> DIGITAL5_ACTUAL_SPEED_ACTUAL_SPEED_TX_SHIFT;
        sal_strncat(msg, "TX Speed = ", WC_DIAG_MAX_NUM_APPEND(msg));
        if (strlen(getSpeedString(speed)) > sizeof(msg)) {
            return SOC_E_ERROR;
        } else {
            sal_strncat(msg, getSpeedString(speed), WC_DIAG_MAX_NUM_APPEND(msg));
            printf("%s\n", msg);
        }

        /* Determine raw line speed */
        /*READ_WC40_PLL_ANAPLLSTATUSr(ws->unit, ws, &data);

        divider = getFrequencyDividerRatio(data);
        if (divider > 0) {
        speed = SPEED_CALC_FREQ * divider;
        sal_snprintf(msg, 2048, "Raw line clk rate@ %.2f MHz=%.1f MHz\n", refClkFreq, speed);
        } else {
        sal_strncat(msg, "Cant compute raw line rate. Unknown value in PLL_ANAPLLSTATUS_NAME");
        }*/

        printf("%s\n", msg);
        return SOC_E_NONE;
    }
}

int wcmod_diag_internal_tfc(wcmod_st *ws)
{
  uint16 data, data2;
  unsigned int shift;
  int n;
  char msg[WC_DIAG_MSG_BUFFER_SIZE] = {0};

  /* PRBS */
  ws->lane_num_ignore = 1;
  READ_WC40_XGXSBLK1_LANEPRBSr(ws->unit, ws, &data);
  ws->lane_num_ignore = 0;

  for (n = 0; n < 4; ++n) {
    shift = 4*n;
    if (data & (XGXSBLK1_LANEPRBS_PRBS_EN0_MASK << shift)) {
      sal_snprintf(msg, WC_DIAG_MSG_BUFFER_SIZE, "Lane %d:  PRBS enabled\n", n);
      /* check inversion status */
      if (data & (XGXSBLK1_LANEPRBS_PRBS_INV0_MASK << shift)) {
	sal_strncat(msg, ", inverted\n", WC_DIAG_MAX_NUM_APPEND(msg));
      } else {
	sal_strncat(msg, ", NOT inverted\n", WC_DIAG_MAX_NUM_APPEND(msg));
      }
      /* Check PRBS order */
      switch ((data & (XGXSBLK1_LANEPRBS_PRBS_ORDER0_MASK << shift)) >> (shift + XGXSBLK1_LANEPRBS_PRBS_ORDER0_SHIFT)) {

       case XGXSBLK1_LANEPRBS_PRBS_ORDER0_prbs7:  sal_strncat(msg, "PRBS 7", WC_DIAG_MAX_NUM_APPEND(msg)) ;break;
       case XGXSBLK1_LANEPRBS_PRBS_ORDER0_prbs15: sal_strncat(msg, "PRBS 15", WC_DIAG_MAX_NUM_APPEND(msg));break;
       case XGXSBLK1_LANEPRBS_PRBS_ORDER0_prbs23: sal_strncat(msg, "PRBS 23", WC_DIAG_MAX_NUM_APPEND(msg));break;
       case XGXSBLK1_LANEPRBS_PRBS_ORDER0_prbs31: sal_strncat(msg, "PRBS 31", WC_DIAG_MAX_NUM_APPEND(msg));break;
       default:                      sal_strncat(msg, "PRBS order unknown\n", WC_DIAG_MAX_NUM_APPEND(msg));break;
      }
    } else {
      sal_snprintf(msg, WC_DIAG_MSG_BUFFER_SIZE, "Lane %d:  PRBS NOT enabled\n", n);
    }
    printf("%s\n", msg);
  }

  /* CJPAT / CRPAT */
  ws->lane_num_ignore = 1;
  READ_WC40_XGXSBLK0_XGXSCONTROLr(ws->unit, ws, &data);
  ws->lane_num_ignore = 0;

  strncpy (msg,"", 1);
  if (data & XGXSBLK0_XGXSCONTROL_PGEN_EN_MASK) {
    sal_strncat(msg, "Pattern generator enabled in XGXSBLK0_XGXSCONTROL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg, "Pattern generator NOT enabled in XGXSBLK0_XGXSCONTROL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  if (data & XGXSBLK0_XGXSCONTROL_PCMP_EN_MASK) {
    sal_strncat(msg, "Pattern comparator enabled in XGXSBLK0_XGXSCONTROL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg,"Pattern comparator NOT enabled in XGXSBLK0_XGXSCONTROL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* Digital 4 Misc 5 */
  READ_WC40_DIGITAL4_MISC5r(ws->unit, ws, &data);

  /* test gen enabled */
  if (data & DIGITAL4_MISC5_TGEN_DATA_SEL_MASK) {
    sal_strncat(msg,"Test generator data selected in DIGITAL4_MISC5_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg,"Test generator data NOT selected in DIGITAL4_MISC5_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* test link enabled */
  if (data & DIGITAL4_MISC5_TGEN_LINK_MASK) {
    sal_strncat(msg,"Test generator link enabled in DIGITAL4_MISC5_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg,"Test generator link NOT enabled in DIGITAL4_MISC5_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* RX CRC */
  if (data & DIGITAL4_MISC5_RCHK_EN_MASK) {
    sal_strncat(msg,"Rx crc enabled in DIGITAL4_MISC5_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg,"Rx crc NOT enabled in DIGITAL4_MISC5_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* RX Bert Control */
  ws->lane_num_ignore = 1;
  READ_WC40_RXBERT_RXBERTCTRLr(ws->unit, ws, &data);
  ws->lane_num_ignore = 1;

  if (data & LNRXBERT_RXBERTCTRL_CRCCHK_EN_MASK) {
    sal_strncat(msg, "Rx crc enabled in RXBERT_RXBERTCTRL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg, "Rx crc NOT enabled in RXBERT_RXBERTCTRL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* TX Bert Control */
  ws->lane_num_ignore = 1;
  READ_WC40_TXBERT_TXBERTCTRLr(ws->unit, ws, &data);
  ws->lane_num_ignore = 0;

  /* CJPat */
  if (data & TXBERT_TXBERTCTRL_CJPAT_EN_MASK) {
    sal_strncat(msg, "CJPat enabled in TXBERT_TXBERTCTRL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg, "CJPat NOT enabled in TXBERT_TXBERTCTRL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* CRPat */
  if (data & TXBERT_TXBERTCTRL_CRPAT_EN_MASK) {
    sal_strncat(msg, "CRPat enabled in TXBERT_TXBERTCTRL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg, "CRPat NOT enabled in TXBERT_TXBERTCTRL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  printf("%s\n", msg); strncpy (msg, "", 1);

  /* Prog data */
  /* Lane 0 */
  sal_strncat(msg, "Lane 0:  ", WC_DIAG_MAX_NUM_APPEND(msg));
  READ_WC40_TX0_ANATXACONTROL0r(ws->unit, ws, &data);

  if (data & TX0_ANATXACONTROL0_TX_MDATA_EN_MASK) {
    sal_strncat(msg, "Programmable data enabled\n",
            WC_DIAG_MAX_NUM_APPEND(msg));

    READ_WC40_TX0_ANATXMDATA0r(ws->unit, ws, &data);
    READ_WC40_TX0_ANATXMDATA1r(ws->unit, ws, &data2);

    data = (data & TX0_ANATXMDATA0_TXMDIOTSTDATAL_MASK) >> TX0_ANATXMDATA0_TXMDIOTSTDATAL_SHIFT;
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "\t 10-bit data: 0x%x\n", data);

    /* combine low and high regs to get 20-bit data */
    data2 = (data2 & TX0_ANATXMDATA1_TXMDIOTSTDATAH_MASK) >> TX0_ANATXMDATA1_TXMDIOTSTDATAH_SHIFT;

    data |= (data2 << TX0_ANATXMDATA0_TXMDIOTSTDATAL_BITS);
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "\t 20-bit data: 0x%x\n", data);
  } else {
    sal_strncat(msg, "Programmable data NOT enabled\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* Lane 1 */
  sal_strncat(msg, "Lane 1:  ", WC_DIAG_MAX_NUM_APPEND(msg));
  READ_WC40_TX1_ANATXACONTROL0r(ws->unit, ws, &data);

  if (data & TX1_ANATXACONTROL0_TX_MDATA_EN_MASK) {
    sal_strncat(msg, "programmable data enabled\n",
            WC_DIAG_MAX_NUM_APPEND(msg));

    READ_WC40_TX1_ANATXMDATA0r(ws->unit, ws, &data);
    READ_WC40_TX1_ANATXMDATA1r(ws->unit, ws, &data2);


    data = (data & TX1_ANATXMDATA0_TXMDIOTSTDATAL_MASK) >> TX1_ANATXMDATA0_TXMDIOTSTDATAL_SHIFT;
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "\t 10-bit data: 0x%x\n", data);

    /* combine low and high regs to get 20-bit data */
    data2 = (data2 & TX1_ANATXMDATA1_TXMDIOTSTDATAH_MASK) >> TX1_ANATXMDATA1_TXMDIOTSTDATAH_SHIFT;

    data |= (data2 << TX1_ANATXMDATA0_TXMDIOTSTDATAL_BITS);
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "\t 20-bit data: 0x%x\n", data);
  } else {
   sal_strncat(msg, "programmable data NOT enabled\n", WC_DIAG_MAX_NUM_APPEND(msg));
  }
  printf("%s\n", msg); strncpy (msg, "", 1);

  /* Lane 2 */
  sal_strncpy_s(msg, "Lane 2:  ", sizeof(msg));
  READ_WC40_TX2_ANATXACONTROL0r(ws->unit, ws, &data);

  if (data & TX2_ANATXACONTROL0_TX_MDATA_EN_MASK) {
    sal_strncat(msg, "programmable data enabled\n",
            WC_DIAG_MAX_NUM_APPEND(msg));

    READ_WC40_TX2_ANATXMDATA0r(ws->unit, ws, &data);
    READ_WC40_TX2_ANATXMDATA1r(ws->unit, ws, &data2);

    data = (data & TX2_ANATXMDATA0_TXMDIOTSTDATAL_MASK) >> TX2_ANATXMDATA0_TXMDIOTSTDATAL_SHIFT;
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "\t 10-bit data: 0x%x\n", data);

    /* combine low and high regs to get 20-bit data */
    data2 = (data2 & TX2_ANATXMDATA1_TXMDIOTSTDATAH_MASK) >> TX2_ANATXMDATA1_TXMDIOTSTDATAH_SHIFT;

    data |= (data2 << TX2_ANATXMDATA0_TXMDIOTSTDATAL_BITS);

    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "\t 20-bit data: 0x%x\n", data);
  } else {
   sal_strncat(msg, "programmable data NOT enabled\n",
           WC_DIAG_MAX_NUM_APPEND(msg));
  }
  printf("%s\n", msg); strncpy (msg, "", 1);

  /* Lane 3 */
  sal_strncat(msg, "Lane 3:  ", WC_DIAG_MAX_NUM_APPEND(msg));
  READ_WC40_TX3_ANATXACONTROL0r(ws->unit, ws, &data);
  if (data & TX3_ANATXACONTROL0_TX_MDATA_EN_MASK) {
    sal_strncat(msg, "programmable data enabled\n", WC_DIAG_MAX_NUM_APPEND(msg));

    READ_WC40_TX3_ANATXMDATA0r(ws->unit, ws, &data);
    READ_WC40_TX3_ANATXMDATA1r(ws->unit, ws, &data2);

    data = (data & TX3_ANATXMDATA0_TXMDIOTSTDATAL_MASK) >> TX3_ANATXMDATA0_TXMDIOTSTDATAL_SHIFT;
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "\t 10-bit data: 0x%x\n", data);

    /* combine low and high regs to get 20-bit data */
    data2 = (data2 & TX3_ANATXMDATA1_TXMDIOTSTDATAH_MASK) >> TX3_ANATXMDATA1_TXMDIOTSTDATAH_SHIFT;

    data |= (data2 << TX3_ANATXMDATA0_TXMDIOTSTDATAL_BITS);
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "\t 20-bit data: 0x%x\n", data);
  } else {
   sal_strncat(msg, "programmable data NOT enabled\n",
           WC_DIAG_MAX_NUM_APPEND(msg));
  }

  printf("%s\n", msg);
  return SOC_E_NONE;
}

void printTaps(int lane, uint16 data)
{
  unsigned int mainTap=0, postTap=0, preTap=0;
  char msg[WC_DIAG_MSG_BUFFER_SIZE] = {0};

  mainTap = (data & GP2_REG_GP2_9_TX0_FIR_TAP_MAIN_MASK) >> GP2_REG_GP2_9_TX0_FIR_TAP_MAIN_SHIFT;
  postTap = (data & GP2_REG_GP2_9_TX0_FIR_TAP_POST_MASK) >> GP2_REG_GP2_9_TX0_FIR_TAP_POST_SHIFT;
  preTap  = (data & GP2_REG_GP2_9_TX0_FIR_TAP_PRE_MASK) >> GP2_REG_GP2_9_TX0_FIR_TAP_PRE_SHIFT;

  sal_snprintf (msg, WC_DIAG_MSG_BUFFER_SIZE, "Lane %d TX Taps: Main:0x%02x Post:0x%02x Pre:0x%02x\n",
                                                lane, mainTap, postTap, preTap);

  if ((preTap + mainTap + postTap) > MAX_TAP_SUM_CONSTRAINT) {
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE,
              "  WARN: max tap sum constraint violation (pre(0x%2x)+main(0x%2x)+post(0x%2x)<=0x%2x\n",
              preTap, mainTap, postTap, MAX_TAP_SUM_CONSTRAINT);
  }
  if (!((mainTap + MIN_TAP_EYE_CONSTRAINT) >= (preTap + postTap))) {
    sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE,
              "  WARN: min eye constraint violation (main(0x%2x) + %x >= pre(0x%2x) + post(0x%2x)\n",
  mainTap, MIN_TAP_EYE_CONSTRAINT, preTap, postTap);
  }
  printf("%s\n", msg);
}

int wcmod_diag_dfe(wcmod_st *ws)
{
  uint16 data;
  char msg[WC_DIAG_MSG_BUFFER_SIZE] = {0};

  /* IDriver */

  READ_WC40_TX0_TX_DRIVERr(ws->unit, ws, &data);

  sal_strncpy_s(msg, "IDriver = ", sizeof(msg));
  switch ((data & TX0_TX_DRIVER_IDRIVER_MASK) >> TX0_TX_DRIVER_IDRIVER_SHIFT) {
    case TX0_TX_DRIVER_IDRIVER_v680mV: sal_strncat(msg, "680 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v730mV: sal_strncat(msg, "730 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v780mV: sal_strncat(msg, "780 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v830mV: sal_strncat(msg, "830 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v880mV: sal_strncat(msg, "880 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v930mV: sal_strncat(msg, "930 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v980mV: sal_strncat(msg, "980 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v1010mV: sal_strncat(msg, "1010 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v1040mV: sal_strncat(msg, "1040 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v1060mV: sal_strncat(msg, "1060 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v1070mV: sal_strncat(msg, "1070 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v1080mV: sal_strncat(msg, "1080 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v1085mV: sal_strncat(msg, "1085 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v1090mV: sal_strncat(msg, "1090 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v1095mV: sal_strncat(msg, "1095 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    case TX0_TX_DRIVER_IDRIVER_v1100mV: sal_strncat(msg, "1100 mV\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
    default:                            sal_strncat(msg, "UNKNOWN\n", WC_DIAG_MAX_NUM_APPEND(msg)); break;
  }
  /* Taps */
  READ_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr(ws->unit, ws, &data);

  if (data & CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK) {
    sal_strncat(msg, "TX fir taps forced\n", WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg, "TX fir taps NOT forced\n", WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* Lane 0 taps */
  READ_WC40_GP2_REG_GP2_9r(ws->unit, ws, &data);
  printTaps(0, data);

  /* Lane 1 taps */
  READ_WC40_GP2_REG_GP2_Ar(ws->unit, ws, &data);
  printTaps(1, data);

  /* Lane 2 taps */
  READ_WC40_GP2_REG_GP2_Br(ws->unit, ws, &data);
  printTaps(2, data);

  /* Lane 3 taps */
  READ_WC40_GP2_REG_GP2_Cr(ws->unit, ws, &data);
  printTaps(3, data);

  /* VGA */
  /* Lane 0  */
  READ_WC40_RX0_ANARXASTATUSr(ws->unit, ws, &data);

  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "Lane 0:  RX VGA = 0x%02x\n",
	  (data & RX0_ANARXASTATUS_VGA_MASK) >> RX0_ANARXASTATUS_VGA_SHIFT);

  /* Lane 1  */
  READ_WC40_RX1_ANARXASTATUSr(ws->unit, ws, &data);

  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "Lane 1:  RX VGA = 0x%02x\n",
	(data & RX1_ANARXASTATUS_VGA_MASK) >> RX1_ANARXASTATUS_VGA_SHIFT);

  /* Lane 2  */
  READ_WC40_RX2_ANARXASTATUSr(ws->unit, ws, &data);

  sal_snprintf((char*)(msg+strlen(msg)),WC_DIAG_MSG_BUFFER_SIZE, "Lane 2:  RX VGA = 0x%02x\n",
	(data & RX2_ANARXASTATUS_VGA_MASK) >> RX2_ANARXASTATUS_VGA_SHIFT);

  /* Lane 3  */
  READ_WC40_RX3_ANARXASTATUSr(ws->unit, ws, &data);

  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "Lane 3:  RX VGA = 0x%02x\n",
             (data & RX3_ANARXASTATUS_VGA_MASK) >> RX3_ANARXASTATUS_VGA_SHIFT);

  /* various register dumps -- no interpretation  */
  /* CDR status */
  READ_WC40_DSC3B0_CDR_STATUS0r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_CDR_STATUS0r:%d\n",data);
  READ_WC40_DSC3B0_CDR_STATUS1r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_CDR_STATUS1r:%d\n",data);
  READ_WC40_DSC3B0_CDR_STATUS2r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_CDR_STATUS2r:%d\n",data);

  /* PI */
  READ_WC40_DSC3B0_PI_STATUS0r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_PI_STATUS0r:%d\n",data);

  /* DFE VGA */
  READ_WC40_DSC3B0_DFE_VGA_STATUS0r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_DFE_VGA_STATUS0r:%d\n",data);

  READ_WC40_DSC3B0_DFE_VGA_STATUS1r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_DFE_VGA_STATUS0r%d\n",data);

  READ_WC40_DSC3B0_DFE_VGA_STATUS2r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_DFE_VGA_STATUS0r%d\n",data);

  READ_WC40_DSC3B0_DFE_VGA_STATUS3r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_DFE_VGA_STATUS0r%d\n",data);

  /* ACQ SM */
  READ_WC40_DSC3B0_ACQ_SM_STATUS0r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_ACQ_SM_STATUS0r%d\n",data);

  READ_WC40_DSC3B0_ACQ_SM_STATUS1r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_ACQ_SM_STATUS1r%d\n",data);

  /* ANA status */
  READ_WC40_DSC3B0_ANA_STATUS0r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_ANA_STATUS0r%d\n",data);

  READ_WC40_DSC3B0_ANA_STATUS1r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_ANA_STATUS1r%d\n",data);

  READ_WC40_DSC3B0_ANA_STATUS2r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_ANA_STATUS2r%d\n",data);

  READ_WC40_DSC3B0_ANA_STATUS3r(ws->unit, ws, &data);
  sal_snprintf((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "DSC3B0_ANA_STATUS3r%d\n",data);

  printf("%s\n", msg);
  return SOC_E_NONE;
}

int wcmod_diag_ieee(wcmod_st *ws)
{
  uint16 data;
  char msg[WC_DIAG_MSG_BUFFER_SIZE] = {0};

  /* clause-72 */
  READ_WC40_CL72_USERB1_CL72_FAULT_REGISTERr(ws->unit, ws, &data);

  if (data & CL72_USERB1_CL72_FAULT_REGISTER_FRAME_LOCK_MASK) {
    sal_strncpy_s(msg, "CL72 frame locked\n", sizeof(msg));
  } else {
    sal_strncpy_s(msg, "CL72 frame NOT locked\n", sizeof(msg));
  }

  /* FEC */
  READ_WC40_GP2_REG_GP2_Er(ws->unit, ws, &data);

  if (data & GP2_REG_GP2_E_FEC_ENABLE_MASK) {
    sal_strncat_s(msg, "FEC enabled\n", WC_DIAG_MSG_BUFFER_SIZE);

    sal_snprintf ((char*)(msg+strlen(msg)), WC_DIAG_MSG_BUFFER_SIZE, "FEC signal OK: 0x%x\n",
	    (data & GP2_REG_GP2_E_FEC_SIGNAL_OK_MASK) >> GP2_REG_GP2_E_FEC_SIGNAL_OK_SHIFT);
  } else {
    sal_strncat_s(msg, "FEC NOT enabled\n", WC_DIAG_MSG_BUFFER_SIZE);
  }
  printf("%s\n", msg);
  return SOC_E_NONE;
}

int wcmod_diag_eee(wcmod_st *ws)
{
  uint16 data;
  char msg[WC_DIAG_MSG_BUFFER_SIZE] = {0};

  READ_WC40_CL73_USERB0_CL73_EEECTRLr(ws->unit, ws, &data);

  if (data & CL73_USERB0_CL73_EEECTRL_CL73_EEEEN_MASK) {
    sal_strncpy_s(msg, "EEE enabled\n", sizeof(msg));
  } else {
    sal_strncpy_s(msg, "EEE NOT enabled\n", sizeof(msg));
  }

  if (data & CL73_USERB0_CL73_EEECTRL_HOLD_EEE_LP_ABILITY_MASK) {
    sal_strncat(msg, "Link partner EEE enabled\n", WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg, "Link partner EEE NOT enabled\n", WC_DIAG_MAX_NUM_APPEND(msg));
  }

  /* LPI */
  READ_WC40_XGXSBLK7_EEECONTROLr(ws->unit, ws, &data);

  if (data & XGXSBLK7_EEECONTROL_LPI_EN_RX_MASK) {
    sal_strncat(msg, "RX LPI pass-thru enabled in XGXSBLK7_EEECONTROL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg, "RX LPI pass-thru NOT enabled in XGXSBLK7_EEECONTROL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  if (data & XGXSBLK7_EEECONTROL_LPI_EN_TX_MASK) {
    sal_strncat(msg,"TX LPI pass-thru enabled in XGXSBLK7_EEECONTROL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg,"TX LPI pass-thru NOT enabled in XGXSBLK7_EEECONTROL_NAME\n",
            WC_DIAG_MAX_NUM_APPEND(msg));
  }

  READ_WC40_EEE_USERB0_EEE_COMBO_CONTROL0r(ws->unit, ws, &data);

  if (data & EEE_USERB0_EEE_COMBO_CONTROL0_LPI_EN_FORCE_MASK) {
    sal_strncat(msg, "LPI pass-thru forced\n", WC_DIAG_MAX_NUM_APPEND(msg));
  } else {
    sal_strncat(msg, "LPI pass-thru derived from autoneg\n", WC_DIAG_MAX_NUM_APPEND(msg));
  }
  printf("%s\n", msg);
  return SOC_E_NONE;
}

#ifndef __KERNEL__

float _wcmod_util_round_real( float original_value, int decimal_places ) 
{
  float shift_digits[WC_UTIL_MAX_ROUND_DIGITS+1] = { 0.0, 10.0, 100.0, 1000.0, 
                10000.0, 100000.0, 1000000.0, 10000000.0, 100000000.0 };
  float shifted;
  float rounded;   
  if (decimal_places > WC_UTIL_MAX_ROUND_DIGITS ) {
      printf("ERROR: Maximum digits to the right of decimal for rounding "
          "exceeded. Max %d, requested %d\n", 
             WC_UTIL_MAX_ROUND_DIGITS, decimal_places);
      return 0.0;
  } 
  /* shift to preserve the desired digits to the right of the decimal */   
  shifted = original_value * shift_digits[decimal_places];

  /* convert to integer and back to float to truncate undesired precision */
  shifted = (float)(floor(shifted+0.5));

  /* shift back to place decimal point correctly */   
  rounded = shifted / shift_digits[decimal_places];
  return rounded;
}

int _wcmod_avg_vga_dfe(wcmod_st* ws, int tapsel, int *avg_value) 
{
    uint16 vga_frzval;
    uint16 vga_frcfrz;
    uint16 dfe_frzval;
    uint16 dfe_frcfrz;
    uint16 frzval;
    uint16 frcfrz;
    int val_min = 0;
    int val_max = 0;
    int val_avg;
    int val_curr = 0;
    uint16 avg_cnt;   
    uint16 loop_var;
    uint16 data16;
    int rem;

   avg_cnt = 40;
   val_avg = 0;

    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws, &data16));
    vga_frzval = (data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK) >>
                 DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_SHIFT;
    vga_frcfrz = (data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK) >>
                  DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_SHIFT;
    dfe_frzval = (data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK) >>
                  DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_SHIFT;
    dfe_frcfrz = (data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK) >>
                  DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_SHIFT;

   if (tapsel == 0) {
       frzval = vga_frzval;
       frcfrz = vga_frcfrz;
       val_min = 47;
       val_max = 0;
   } else {
       frzval = (dfe_frzval & (1 << (tapsel-1))) / (1 << (tapsel-1));
       frcfrz = dfe_frcfrz; 
       if (tapsel == 1) {
           val_min = 63;
           val_max = 0;
       } else if ((tapsel == 2) || (tapsel == 3)) {
           val_min = 31;
           val_max = -31;
       } else if ((tapsel == 4) || (tapsel == 5)) {
           val_min = 15;
           val_max = -15;
       } 
   } 

   if ((frzval == 1) && (frcfrz == 1)) {
       avg_cnt = 1;
   } 

   /* Const DSC_3_ADDR = &H8220 */
   for (loop_var = 0; loop_var < avg_cnt; loop_var++) {
       switch(tapsel) {
           case 0:
               SOC_IF_ERROR_RETURN
                   (READ_WC40_DSC3B0_DFE_VGA_STATUS0r(ws->unit,ws, &data16));
               val_curr = data16 & DSC3B0_DFE_VGA_STATUS0_VGA_SUM_MASK;
               break;

           case 1:
               SOC_IF_ERROR_RETURN
                   (READ_WC40_DSC3B0_DFE_VGA_STATUS0r(ws->unit,ws,&data16));
               val_curr = (data16 & DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_MASK) >>
                          DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_SHIFT;
               break;

           case 2:
               SOC_IF_ERROR_RETURN
                   (READ_WC40_DSC3B0_DFE_VGA_STATUS1r(ws->unit,ws, &data16));
               val_curr = data16 & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_2_BIN_MASK;
               if (val_curr >= 32) {
                   val_curr -= 64;
               }
               break;

           case 3:
               SOC_IF_ERROR_RETURN
                   (READ_WC40_DSC3B0_DFE_VGA_STATUS1r(ws->unit,ws,&data16));
               val_curr = (data16 & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_MASK) >>
                          DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_SHIFT;
               if (val_curr >= 32) {
                   val_curr -= 64;
               }
               break;

           case 4:
               SOC_IF_ERROR_RETURN
                   (READ_WC40_DSC3B0_DFE_VGA_STATUS2r(ws->unit,ws, &data16));
               val_curr = data16 & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_4_BIN_MASK;
               if (val_curr >= 16) {
                   val_curr -= 32;
               }
               break;

           case 5:
               SOC_IF_ERROR_RETURN
                   (READ_WC40_DSC3B0_DFE_VGA_STATUS2r(ws->unit,ws,&data16));
               val_curr = (data16 & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_MASK) >>
                          DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_SHIFT;
               if (val_curr >= 16) {
                   val_curr -= 32;
               }
               break;

           default:
               break;
      } 

      LOG_INFO(BSL_LS_SOC_PHY,
              (BSL_META_U(ws->unit, "WC_VEYE : val_curr = %d, val_avg = %d, tap_select = %d\n"),
                    val_curr,val_avg,tapsel));
      val_avg = (val_avg + val_curr);

      if (val_curr < val_min) {
          val_min = val_curr;
      }

      if (val_curr > val_max) {
          val_max = val_curr;
      } 
      /* delay 20ms */
#ifdef _SDK_WCMOD_
     sal_usleep(20000);  /* 10ms */
#endif 
   } /* end for loop */ 

   /* average value */
   rem = val_avg % avg_cnt;
   val_avg = val_avg / avg_cnt;
   
   /* add one if remainder is more than half */ 
   val_avg += (2 * rem) / avg_cnt;
   *avg_value = val_avg;

   return SOC_E_NONE;
}

int  _wcmod_set_hoff(wcmod_st* ws,  int hoff)
{
    int rv = 0;

    /* write horizontal offset */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,  
                  hoff << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
		  (0x2| (hoff << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
		  (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

    /* wait for uC ready for command:  bit7=1    */
    rv = wcmod_regbit_set_wait_check(ws,DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT*1000);

    ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv <0) || (ws->accData != 0)) {
            printf("WC_EYE : uController not ready pass 1!: u=%d n", ws->unit);
    }
        
    return rv;
} 

int  _wcmod_get_max_hoffset_left(wcmod_st* ws)
{
    int rv = 0;
    uint16 data;


    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,  
                  127 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
		  (0x2| (127 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
		  (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

    /* wait for uC ready for command:  bit7=1    */
    rv = wcmod_regbit_set_wait_check(ws,DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT*1000);

    ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv <0) || (ws->accData != 1)) {
            printf("WC_EYE : uController not ready pass 1!: u=%d n", ws->unit);
            return (SOC_E_TIMEOUT);

    }
    /* read back the maximum offset */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, &data));
    data &= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK;
    data >>= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
    ws->accData = -data;
        
    return rv;
}

int  _wcmod_get_max_hoffset_right(wcmod_st* ws)
{
    int rv = 0;
    uint16 data;

 
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,  
                  -128 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                     (0x2| (-128 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                     (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

    /* wait for uC ready for command:  bit7=1    */
    rv = wcmod_regbit_set_wait_check(ws,DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT*1000);
    ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv < 0) || (ws->accData != 1)) {
            printf("WC_EYE : uController not ready pass 1!: u=%d n", ws->unit);
            return (SOC_E_TIMEOUT);
    }
    /* read back the maximum offset */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, &data));
    data &= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK;
    data >>= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
    ws->accData = data;
        
    return rv;
}

int  _wcmod_set_voff(wcmod_st* ws, int voff)
{
    int rv = 0;

    /* write vertical offset */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,  
                  voff << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                     (0x3| (voff << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                     (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

    /* wait for uC ready for command:  bit7=1    */
    rv = wcmod_regbit_set_wait_check(ws,DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT*1000);
    ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv <0) || (ws->accData != 0)) {
       printf("WC_EYE set_voffset: uController not ready pass 1!: u=%d\n", ws->unit);
    }

    return rv;
} 

int  _wcmod_get_min_voffset(wcmod_st* ws)
{
    int rv = 0;
    uint16 data;

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,  
                  -128 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                     (0x3| (-128 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                     (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

    /* wait for uC ready for command:  bit7=1    */
    rv = wcmod_regbit_set_wait_check(ws,DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT*1000);
    ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv <0) || (ws->accData != 1)) {
        printf("WC_EYE : uController not ready pass 1!: u=%d n", ws->unit);
        return (SOC_E_TIMEOUT);
    }
    /* read back the maximum offset */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, &data));
    data &= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK;
    data >>= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
    ws->accData = data - 256;
        
    return rv;
}


int  _wcmod_get_max_voffset(wcmod_st* ws)
{
    int rv = 0;
    uint16 data;

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,  
                  127 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                     (0x3| (127 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                     (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

    /* wait for uC ready for command:  bit7=1    */
    rv = wcmod_regbit_set_wait_check(ws,DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT*1000);
    ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv <0) || (ws->accData != 1)) {
        printf("WC_EYE : uController not ready pass 1!: u=%d n", ws->unit);
        return (SOC_E_TIMEOUT);
    }
    /* read back the maximum offset */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, &data));
    data &= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK;
    data >>= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
    ws->accData = data;
        
    return rv;
}

int  _wcmod_get_init_voffset(wcmod_st* ws)
{
    int rv = 0;
    uint16 data;

    SOC_IF_ERROR_RETURN(READ_WC40_DSC2B0_DSC_ANA_CTRL3r(ws->unit, ws, &data));
    data = (data & DSC2B0_DSC_ANA_CTRL3_RESERVED_FOR_ECO0_MASK) >> DSC2B0_DSC_ANA_CTRL3_RESERVED_FOR_ECO0_SHIFT;
    ws->accData = data;
        
    return rv;
}

int _wcmod_enable_livelink_diagnostics(wcmod_st* ws)
{
    uint16 data16, mask16;
    int regval, pf_ctrl, lopf,vga_sum, dfe1_bin, dfe2_bin, dfe3_bin, dfe4_bin, dfe5_bin;
    /* first enable livelink bit also */
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 
                 0x2,   0x2));

    /* 0x820d[0]=1 enable diagnostic */ 
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

    /* pf_ctrl(lane) = rd22_pf_ctrl (phy, lane) 0x822b */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC3B0_ANA_STATUS0r(ws->unit, ws, &data16));
    pf_ctrl = data16 & DSC3B0_ANA_STATUS0_PF_CTRL_BIN_MASK;

    /* low pass filter(lane) = rd22_pf_ctrl (phy, lane) 0x821d */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC2B0_DSC_ANA_CTRL4r(ws->unit, ws, &data16));
    lopf = (data16 & 0x700) >> 8;

    /* vga_sum(lane) = rd22_vga_sum (phy, lane) 0x8225  */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC3B0_DFE_VGA_STATUS0r(ws->unit, ws, &data16));
    vga_sum = data16 & DSC3B0_DFE_VGA_STATUS0_VGA_SUM_MASK;

    /* dfe1_bin(lane) = rd22_dfe_tap_1_bin (phy, lane) 0x8225  */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC3B0_DFE_VGA_STATUS0r(ws->unit, ws, &data16));
    regval = (data16 & DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_MASK) >> DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_SHIFT;
    dfe1_bin = regval;

    /* dfe2_bin(lane) = rd22_dfe_tap_2_bin (phy, lane) 0x8226  */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC3B0_DFE_VGA_STATUS1r(ws->unit, ws, &data16));
    regval = data16 & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_2_BIN_MASK;
    mask16 = (DSC3B0_DFE_VGA_STATUS1_DFE_TAP_2_BIN_MASK >> 1) + 1;
    if (regval >= mask16) {
        regval -= mask16 << 1;
    }
    dfe2_bin = regval;

    /* dfe3_bin(lane) = rd22_dfe_tap_3_bin (phy, lane) 0x8226  */
    regval = (data16 & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_MASK) >> DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_SHIFT;
    mask16 = ((DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_MASK) >> (DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_SHIFT + 1)) + 1;
    if (regval >= mask16) {
        regval -= mask16 << 1;
    }
    dfe3_bin = regval;

    /* dfe4_bin(lane) = rd22_dfe_tap_4_bin (phy, lane) 0x8227  */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC3B0_DFE_VGA_STATUS2r(ws->unit, ws, &data16));
    regval = data16 & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_4_BIN_MASK;
    mask16 = (DSC3B0_DFE_VGA_STATUS2_DFE_TAP_4_BIN_MASK >> 1) + 1;
    if (regval >= mask16) {
        regval -= mask16 << 1;
    }
    dfe4_bin = regval;

    /* dfe5_bin(lane) = rd22_dfe_tap_5_bin (phy, lane) 0x8227  */
    regval = (data16 & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_MASK) >> DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_SHIFT;
    mask16 = ((DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_MASK) >> (DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_SHIFT + 1)) + 1;
    if (regval >= mask16) {
        regval -= mask16 << 1;
    }
    dfe5_bin = regval;
    /* display the average value written to VGA/DFE */ 
    printf("WC_EYE livelink pf/vga/dfe: u=%d p=%d PF:%04d Lopf:%04d  VGA:%04d dfe1:%04d dfe2:%04d dfe3:%04d "
          "dfe4:%04d dfe5:%04d\n", ws->unit, ws->port, pf_ctrl, lopf, vga_sum, dfe1_bin,
          dfe2_bin, dfe3_bin,dfe4_bin,dfe5_bin);


    return (SOC_E_NONE);

}

int _wcmod_disable_livelink_diagnostics(wcmod_st* ws)
{
    /* disable diagnostics */
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 0,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

    /* next disable livelink bit also */
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 
                 0x0,   0x2));

    return (SOC_E_NONE);
}

int _wcmod_enable_deadlink_diagnostics(wcmod_st* ws)
{
    int dfe_vga[6];
    int loop_var;
    uint16 data16;
    /* 0x820d[0]=1 enable diagnostic */ 
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

    /* save original DFE/VGA settings */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws, &data16));
    ws->vga_frzval = data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK; 
    ws->vga_frcfrz = data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK;
    ws->dfe_frzval = data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK; 
    ws->dfe_frcfrz = data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK;

    for (loop_var = 0; loop_var <= 5; loop_var++) {
        (void)_wcmod_avg_vga_dfe (ws,loop_var,&dfe_vga[loop_var]);
        if (loop_var == 0) {
            /* freeze VGA */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws, 
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK |
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK,
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK |
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK));

            /* select VGA */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,  
                  loop_var << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_SHIFT,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_MASK));

            /* set the VGA value */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,  
                   dfe_vga[loop_var] << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_SHIFT,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_MASK));

            /* VGA/DFE write enable */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,  
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));

            /* VGA/DFE write disable */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,  
                  0,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));
        }
    }
    /* freeze the DFE */ 
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws,
          (0x1f << DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_SHIFT) |
          DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK,
          DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK |
          DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK));

    for (loop_var = 1; loop_var <= 5; loop_var++) {
        /* set DFE taps */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws, 
              loop_var << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_SHIFT,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_MASK));

        /* set the DFE tap value */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,  
               dfe_vga[loop_var] << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_SHIFT,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_MASK));

        /* VGA/DFE write enable */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws, 
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));

        /* VGA/DFE write disable */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,0,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));
    }

    /* display the average value written to VGA/DFE */ 
    printf("WC_EYE avg dfe/vga: u=%d p=%d, VGA:0x%x, dfe1:0x%x, dfe2:0x%x, dfe3:0x%x, "
          "dfe4:0x%x, dfe5:0x%x\n", ws->unit, ws->port, dfe_vga[0],dfe_vga[1],
          dfe_vga[2],dfe_vga[3],dfe_vga[4],dfe_vga[5]);

    return (SOC_E_NONE);

}

int _wcmod_disable_deadlink_diagnostics(wcmod_st* ws)
{
    /* restore the  vga/dfe freeze value before exit */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws,
                      ws->vga_frzval,
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws, 
                      ws->dfe_frzval,
                      DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws, 
                      ws->vga_frcfrz,
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws,
                      ws->dfe_frcfrz,
                      DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK));

    /* disable diagnostics */
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 0,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

    return (SOC_E_NONE);
}

int _wcmod_start_deadlink_diagnostics(wcmod_st* ws)
{
    SOC_IF_ERROR_RETURN(wcmod_prbs_check(ws));
    SOC_IF_ERROR_RETURN(wcmod_prbs_check(ws));
    return (SOC_E_NONE);

}

int _wcmod_read_deadlink_diagnostics(wcmod_st* ws)
{   
    int data;
    SOC_IF_ERROR_RETURN(wcmod_prbs_check(ws));
    /* not lock or temp lost lock all treated the same */
    if (ws->accData < 0) {
        data = 0x1fff;
    } else {
        data = ws->accData;
    }
                
    ws->accData = data;
    return (SOC_E_NONE);

}

int _wcmod_start_livelink_diagnostics(wcmod_st* ws)
{
    int rv = 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x6, DSC1B0_UC_CTRL_GP_UC_REQ_MASK));

    /* wait for uC ready for command:  bit7=1    */
    rv = wcmod_regbit_set_wait_check(ws,DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT*1000);
    ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv <0) || (ws->accData != 0)) {
        printf("WC_EYE start_live_link : uController not ready pass 1!: u=%d\n", ws->unit);
    }

    return rv;
}

int _wcmod_stop_livelink_diagnostics(wcmod_st* ws)
{
    int rv = 0;

    SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 
                                                     (0x1 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT),
                                                     DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                     (0x6| (0x1 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                     (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

    /* wait for uC ready for command:  bit7=1    */
    rv = wcmod_regbit_set_wait_check(ws,DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT*1000);
    ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv <0) || (ws->accData != 0)) {
        printf("WC_EYE start_live_link : uController not ready pass 1!: u=%d\n", ws->unit);
    }
    return rv;
}

int _wcmod_clear_livelink_diagnostics(wcmod_st* ws)
{
    int rv = 0;
          
    SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 
                                                     (0x3 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT),
                                                     DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
    SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                                     (0x6| (0x3 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                                     (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

    /* wait for uC ready for command:  bit7=1    */
    rv = wcmod_regbit_set_wait_check(ws,DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT*1000);
    ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv <0) || (ws->accData != 0)) {
        printf("WC_EYE start_live_link : uController not ready pass 1!: u=%d\n", ws->unit);
    }

    return rv;
}

int _wcmod_read_livelink_diagnostics(wcmod_st* ws)
{
    int rv = 0;
    int i = 0;
    uint32 error_counter = 0;
    uint16 data;


    for (i = 0; i < 4; i++) {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 
                                                       (0x2 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT),
                                                       DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
      SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
      SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
      SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
      SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                                       (0x6| (0x2 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                                       (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));
      /* wait for uC ready for command:  bit7=1    */
      rv = wcmod_regbit_set_wait_check(ws,DSC1B0_UC_CTRLr,
                 DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT*1000);
      ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;
      if ((rv <0) || (ws->accData != 0)) {
        printf("WC_EYE : uController not ready pass 1!: u=%d \n", ws->unit);
        return (SOC_E_TIMEOUT);
      }
      SOC_IF_ERROR_RETURN
            (READ_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, &data));
      data &= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK;
      data >>= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;

      error_counter +=  data << (8 * i);
    }
    ws->accData  = error_counter;
    return SOC_E_NONE;
}

#if 0
int _wcmod_2d_livelink_eye_margin_data_get(wcmod_st* ws) 
{
    int runtime_loop;
    int run_time_resolution;
    int max_runtime_loop;
    int curr_runtime;
    int loop_var_h, loop_var_v;
    int h_offset, v_offset, h_max, h_min,v_max, v_min;
    int rv = SOC_E_NONE;
    uint16 tempP1lvl;
    uint32 localErr, localRunTime, error_count;
    soc_port_phy_eyescan_results_t* results;
    soc_port_phy_eye_bounds_t localBound;
    results = ((soc_port_phy_eyescan_params_results_t*) ws->arg)->results;
    localBound = ((soc_port_phy_eyescan_params_results_t*) ws->arg)->params->bounds; 
    max_runtime_loop = ((soc_port_phy_eyescan_params_results_t*) ws->arg)->params->sample_time;
    run_time_resolution = ((soc_port_phy_eyescan_params_results_t*) ws->arg)->params->sample_resolution;
    max_runtime_loop /= run_time_resolution;
    printf("max loop count is %d and resolution is %d \n", max_runtime_loop, run_time_resolution);   

    /* next enable livelink bit also */
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 
                 0x2,   0x2));

    /* 0x820d[0]=1 enable diagnostic */ 
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

    sal_usleep(100000);
    /* read back the p1 lvl value */
    SOC_IF_ERROR_RETURN(READ_WC40_DSC2B0_DSC_ANA_CTRL3r(ws->unit, ws, &tempP1lvl));
    tempP1lvl = (tempP1lvl & DSC2B0_DSC_ANA_CTRL3_RESERVED_FOR_ECO0_MASK) >> DSC2B0_DSC_ANA_CTRL3_RESERVED_FOR_ECO0_SHIFT;

    loop_var_h = 1;
    loop_var_v = 1;

    /*next read the maximum offset from the chip */
    rv = _wcmod_get_max_hoffset_left(ws);
    h_min = ws->accData;
    if (rv == SOC_E_TIMEOUT) {
        printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
        return (SOC_E_TIMEOUT);
    }

    /*next read the maximum offset from the chip */
    rv = _wcmod_get_max_hoffset_right(ws);
    h_max = ws->accData;
    if (rv == SOC_E_TIMEOUT) {
        printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
        return (SOC_E_TIMEOUT);
    }

    /*next read the minimum v offset from the chip */
    rv = _wcmod_get_min_voffset(ws);
    v_min = ws->accData;
    if (rv == SOC_E_TIMEOUT) {
        printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
        return (SOC_E_TIMEOUT);
    }

    /*next read the maximum v offset from the chip */
    rv = _wcmod_get_max_voffset(ws);
    v_max = ws->accData;
    if (rv == SOC_E_TIMEOUT) {
        printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
        return (SOC_E_TIMEOUT);
    }

    /* need to check the bound comparing with user passed in value */
    if (h_max > localBound.horizontal_max) {
      h_max = localBound.horizontal_max;
    } else {/* the bound is too big */
      printf("The passed  horizontal bound is too big, use max available bound %d \n", h_max);
    }
    if (h_min < localBound.horizontal_min) {
      h_min = localBound.horizontal_min;
    } else {/* the bound is too big */
      printf("The passed  horizontal bound is too big, use max available bound %d \n", h_min);
    }

    if (v_max > localBound.vertical_max) {
      v_max = localBound.vertical_max;
    } else {/* the bound is too big */
      printf("The passed vertical  bound is too big, use max available bound %d \n", v_max);
    }
    if (v_min < localBound.vertical_min) {
      v_min = localBound.vertical_min;
    } else {/* the bound is too big */
      printf("The passed  vertical bound is too big, use max available bound %d \n", v_min);
    }

    printf("h_min is %d and h_max %d and step  %d\n", h_min, h_max, loop_var_h);
    printf("v_min is %d and v_max  %d and step v %d\n", v_min, v_max, loop_var_v);
    

  for (loop_var_h = h_min; loop_var_h <= h_max; loop_var_h++) {
      h_offset = loop_var_h;
      /* next set the h_offset */
      rv = _wcmod_set_hoff(ws, h_offset);
      if (rv == SOC_E_TIMEOUT) {
        printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
        return (SOC_E_TIMEOUT);
      }

      for (loop_var_v = v_min; loop_var_v <= v_max; loop_var_v++) {
            v_offset = loop_var_v;
            /* next set the vertical offset */
            rv = _wcmod_set_voff(ws, v_offset);
            if (rv == SOC_E_TIMEOUT) {
                printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
                return (SOC_E_TIMEOUT);
            }

            error_count = 0;
            localRunTime = 0;
            localErr = 0;
            for (runtime_loop = 1; runtime_loop <= max_runtime_loop; runtime_loop++) {
                curr_runtime = runtime_loop * run_time_resolution; 
                curr_runtime /= 1000;     /* convert to second from mili second */

                SOC_IF_ERROR_RETURN(_wcmod_start_livelink_diagnostics(ws));        
#ifdef _SDK_WCMOD_
                sal_usleep(curr_runtime * 1000000);
#endif

                SOC_IF_ERROR_RETURN(_wcmod_stop_livelink_diagnostics(ws));
                rv =  _wcmod_read_livelink_diagnostics(ws);
                error_count = (uint32) ws->accData;
                if (rv == SOC_E_TIMEOUT) {
                    printf("WCMOD_EYE: uC waits for error counter fail: u=%d p=%d\n",ws->unit,ws->port);
                    return (SOC_E_TIMEOUT);
                }

                printf("WC_EYE: h_offset %d  v_offset %d  error   0x%x \n", h_offset, v_offset,error_count);

                if (error_count == 0) {
                    localErr  += 1;
                } else {
                    localErr  += error_count;
                }

                SOC_IF_ERROR_RETURN(_wcmod_clear_livelink_diagnostics(ws));

                localRunTime +=  curr_runtime;
                if (localErr >= HI_CONFIDENCE_ERR_CNT) {
                    break;
                }
            }
            results->run_time[loop_var_h + SOC_PORT_PHY_EYESCAN_H_INDEX][loop_var_v + SOC_PORT_PHY_EYESCAN_V_INDEX] = localRunTime*1000 /*Back to milisec*/;
            results->error_count[loop_var_h +SOC_PORT_PHY_EYESCAN_H_INDEX][loop_var_v  + SOC_PORT_PHY_EYESCAN_V_INDEX] = localErr;

      }   /* loop_var_v */
  }      /* for loop_var_h */

  /* Undo setup */
  rv = _wcmod_set_hoff(ws, 0);
  if (rv == SOC_E_TIMEOUT) {
    printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
    return (SOC_E_TIMEOUT);
  }

  rv = _wcmod_set_voff(ws, (int) tempP1lvl);
  if (rv == SOC_E_TIMEOUT) {
    printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
    return (SOC_E_TIMEOUT);
  }

  /* disable diagnostics */
  SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 0,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

  /* next  disable livelink bit also */
  SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 
                 0x0,   0x2));
  return SOC_E_NONE;
}

int _wcmod_2d_prbs_eye_margin_data_get(wcmod_st* ws) 
{
    int runtime_loop;
    int run_time_resolution;
    int max_runtime_loop, max_runtime;
    int curr_runtime;
    int loop_var_h, loop_var_v;
    int h_offset, v_offset, h_max, h_min,v_max, v_min;
    int rv = SOC_E_NONE;
    int prbs_stky_flag;
    int prbs_stky_cnt;
    int max_stky_cnt;
    uint16 tempP1lvl;
    uint32 localErr, localRunTime, error_count;
    soc_port_phy_eyescan_results_t* results;
    soc_port_phy_eye_bounds_t localBound;
    results = ((soc_port_phy_eyescan_params_results_t*) ws->arg)->results;
    localBound = ((soc_port_phy_eyescan_params_results_t*) ws->arg)->params->bounds; 
    max_runtime = ((soc_port_phy_eyescan_params_results_t*) ws->arg)->params->sample_time;
    run_time_resolution = ((soc_port_phy_eyescan_params_results_t*) ws->arg)->params->sample_resolution;
    max_runtime_loop = max_runtime/run_time_resolution;
    printf("max loop count is %d and resolution is %d \n", max_runtime_loop, run_time_resolution);   

    /* 0x820d[0]=1 enable diagnostic */ 
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

    sal_usleep(100000);
    /* read back the p1 lvl value */
    SOC_IF_ERROR_RETURN(READ_WC40_DSC2B0_DSC_ANA_CTRL3r(ws->unit, ws, &tempP1lvl));
    tempP1lvl = (tempP1lvl & DSC2B0_DSC_ANA_CTRL3_RESERVED_FOR_ECO0_MASK) >> DSC2B0_DSC_ANA_CTRL3_RESERVED_FOR_ECO0_SHIFT;
    printf("the orginal P1lvl value is 0x%x \n", tempP1lvl);

    loop_var_h = 1;
    loop_var_v = 1;

    /*next read the maximum offset from the chip */
    rv = _wcmod_get_max_hoffset_left(ws);
    h_min = ws->accData;
    if (rv == SOC_E_TIMEOUT) {
        printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
        return (SOC_E_TIMEOUT);
    }

    /*next read the maximum offset from the chip */
    rv = _wcmod_get_max_hoffset_right(ws);
    h_max = ws->accData;
    if (rv == SOC_E_TIMEOUT) {
        printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
        return (SOC_E_TIMEOUT);
    }

    /*next read the minimum v offset from the chip */
    rv = _wcmod_get_min_voffset(ws);
    v_min = ws->accData;
    if (rv == SOC_E_TIMEOUT) {
        printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
        return (SOC_E_TIMEOUT);
    }

    /*next read the maximum v offset from the chip */
    rv = _wcmod_get_max_voffset(ws);
    v_max = ws->accData;
    if (rv == SOC_E_TIMEOUT) {
        printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
        return (SOC_E_TIMEOUT);
    }
  /* need to check the bound comparing with user passed in value */
    if (h_max > localBound.horizontal_max) {
      h_max = localBound.horizontal_max;
    } else {/* the bound is too big */
      printf("The passed  horizontal bound is too big, use max available bound %d \n", h_max);
    }
    if (h_min < localBound.horizontal_min) {
      h_min = localBound.horizontal_min;
    } else {/* the bound is too big */
      printf("The passed  horizontal bound is too big, use max available bound %d \n", h_min);
    }

    if (v_max > localBound.vertical_max) {
      v_max = localBound.vertical_max;
    } else {/* the bound is too big */
      printf("The passed vertical  bound is too big, use max available bound %d \n", v_max);
    }

    /* for prbs v_miv = 0 */
    v_min = 0;
    if (v_min < localBound.vertical_min) {
      v_min = localBound.vertical_min;
    } else {/* the bound is too big */
      printf("The passed  vertical bound is too big, use max available bound %d \n", v_min);
    }

    printf("h_min is %d and h_max %d and step  %d\n", h_min, h_max, loop_var_h);
    printf("v_min is %d and v_max  %d and step v %d\n", v_min, v_max, loop_var_v);
    
 
  for (loop_var_h = h_min; loop_var_h <= h_max; loop_var_h++) {
      h_offset = loop_var_h;
      {    /* next set the h_offset */
          rv = _wcmod_set_hoff(ws, h_offset); 
          if (rv == SOC_E_TIMEOUT) {
            printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
            return (SOC_E_TIMEOUT);
          }
      } 
           

      for (loop_var_v = abs(v_min); loop_var_v <= v_max; loop_var_v++) {
            v_offset = loop_var_v;
            /* next set the vertical offset */
            rv = _wcmod_set_voff(ws, v_offset);
            if (rv == SOC_E_TIMEOUT) {
                printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
                return (SOC_E_TIMEOUT);
            }

            error_count = 0;
            localRunTime = 0;
            localErr = 0;
            curr_runtime = 0;
            for (runtime_loop = 1; runtime_loop <= max_runtime_loop; runtime_loop++) {
                if (curr_runtime <= max_runtime) {
                    curr_runtime = (1 << (runtime_loop - 1)) * run_time_resolution;
                } else { 
                    break;
                }     
                curr_runtime /= 1000;     /* convert to second from mili second */

                prbs_stky_flag = 1;
                prbs_stky_cnt = 0;
                if (curr_runtime < 4) {
                    max_stky_cnt = 4;
                } else {
                    max_stky_cnt = 2;
                }

                while (prbs_stky_flag) {
                    /* wait for specified amount of time to collect the PRBS error */
                    SOC_IF_ERROR_RETURN(wcmod_prbs_check(ws));
        #ifdef _SDK_WCMOD_
                    sal_usleep(curr_runtime * 1000000);
        #endif
                    SOC_IF_ERROR_RETURN(wcmod_prbs_check(ws));
            
                    error_count = ws->accData;
                    prbs_stky_flag = 0;
                    if (error_count == -1) { /* not lock */
                        error_count = 0x3fff;  /* max error */
                    } else if (error_count  == -2) {
                        /* legacy PRBS momentarily lose link, retry limited times */
                        prbs_stky_cnt += 1;
                        if (prbs_stky_cnt < max_stky_cnt) { 
                            prbs_stky_flag = 1;
                        } else {
                            error_count = 0x3fff; /* default if retry fails */
                        }
                    }
                }
                
                error_count /= 2;
                printf("WC_EYE: h_offset %d  v_offset %d  runtime %d error   0x%x \n", h_offset, v_offset, curr_runtime, error_count);

                if (error_count == 0) {
                    localErr  += 1;
                } else {
                    localErr  += error_count;
                }

                localRunTime +=  curr_runtime;
                if (localErr >= HI_CONFIDENCE_ERR_CNT) {
                    break;
                }
            }
            results->run_time[loop_var_h + SOC_PORT_PHY_EYESCAN_H_INDEX][loop_var_v + SOC_PORT_PHY_EYESCAN_V_INDEX] = localRunTime*1000 /*convert to milisec*/;
            results->error_count[loop_var_h +SOC_PORT_PHY_EYESCAN_H_INDEX][loop_var_v  + SOC_PORT_PHY_EYESCAN_V_INDEX] = localErr;

      }   /* loop_var_v */
  }      /* for loop_var_h */

  /* Undo setup */
  rv = _wcmod_set_hoff(ws, 0);
  if (rv == SOC_E_TIMEOUT) {
    printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
    return (SOC_E_TIMEOUT);
  }

  rv = _wcmod_set_voff(ws, (int) tempP1lvl);
  if (rv == SOC_E_TIMEOUT) {
    printf("WCMOD_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
    return (SOC_E_TIMEOUT);
  }

  /* disable diagnostics */
  SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 0,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));
  return SOC_E_NONE;
}
#endif

int _wcmod_eye_margin_data_get(wcmod_st* ws, WCMOD_EYE_DIAG_INFOt *pInfo,int type) 
{
    int runtime_loop;
    int max_runtime_loop;
    int curr_runtime;
    int offset[MAX_LOOPS];
    int offset_max;
    int loop_var;
    int veye_cnt;
    int hi_confidence_cnt;
    uint32 prbs_status=0;
    int rv = SOC_E_NONE;
    uint16 data16;
    uint16 vga_frzval;
    uint16 vga_frcfrz;
    uint16 dfe_frzval;
    uint16 dfe_frcfrz;
    int pll_div[16] = {32,36,40,42,48,50,52,54,60,64,66,68,80,120,200,240};
    int ref_clk_freq[8] = {25000,100000,125000,156250,187500,161250,50000,106250};
    uint16 clk90_p_offset;
    int dfe_vga[6];
    int tmp=0;
    int max_total_time=0;
    int prbs_stky_flag;
    int prbs_stky_cnt;
    int max_stky_cnt;

    /* check what rate the test will run for */
    SOC_IF_ERROR_RETURN(READ_WC40_PLL_ANAPLLSTATUSr(ws->unit,ws,&data16));

    /* get the ref_clk divider first */
    pInfo->rate = pll_div[data16 & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK];

    /* get the ref_clk frequency */
    SOC_IF_ERROR_RETURN(READ_WC40_SERDESDIGITAL_MISC1r(ws->unit,ws,&data16));

    data16 = (data16 >> SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) & 7;
    pInfo->rate *= ref_clk_freq[data16];

    /* max loop_cnt for BER test with test time doubling every iteration */
    /* max_runtime_loop = log(MAX_RUNTIME/MIN_RUNTIME)/log(2); */
    max_runtime_loop = 80;
    pInfo->max_loops = MAX_LOOPS;

    /* Initialize BER array */
    for( loop_var = 0; loop_var < pInfo->max_loops; loop_var++ ) {
        pInfo->total_errs[loop_var] = 0;
    }

    /* 0x820d[0]=1 enable diagnostic */
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

    /* save original DFE/VGA settings */
    SOC_IF_ERROR_RETURN(READ_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws, &data16));
    vga_frzval = data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK;
    vga_frcfrz = data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK;
    dfe_frzval = data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK;
    dfe_frcfrz = data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK;

    for (loop_var = 0; loop_var <= 5; loop_var++) {
        (void)_wcmod_avg_vga_dfe(ws, loop_var, &dfe_vga[loop_var]);
        if (loop_var == 0) {
            /* freeze VGA */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws,  
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK |
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK,
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK |
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK));

            /* select VGA */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,  
                  loop_var << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_SHIFT,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_MASK));

            /* set the VGA value */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws, 
                   dfe_vga[loop_var] << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_SHIFT,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_MASK));

            /* VGA/DFE write enable */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,  
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));

            /* VGA/DFE write disable */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,  
                  0,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));
        }
    }
    /* freeze the DFE */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws,
        (0x1f << DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_SHIFT) |
        DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK,
        DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK |
        DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK));

    for (loop_var = 1; loop_var <= 5; loop_var++) {
        /* set DFE taps */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,  
              loop_var << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_SHIFT,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_MASK));

        /* set the DFE tap value */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws, 
               dfe_vga[loop_var] << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_SHIFT,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_MASK));

        /* VGA/DFE write enable */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws, 
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));

        /* VGA/DFE write disable */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,  
              0,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));
    }


    if (type == WC_UTIL_HEYE_L) {
        /* Write max to get left eye offset range */
        SOC_IF_ERROR_RETURN(WRITE_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x7f02));
    } else if (type == WC_UTIL_HEYE_R) {
        /* Write min to get right eye offset range */
        SOC_IF_ERROR_RETURN(WRITE_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,0x8002));
    } else {
        /* write max to get vertical offset range */
        SOC_IF_ERROR_RETURN(WRITE_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x7f03));
    }

    /* wait for uC ready for command:  bit7=1  */
    rv = wcmod_regbit_set_wait_check(ws,DSC1B0_UC_CTRLr,
                        DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT);

    if (rv == SOC_E_TIMEOUT) {
        LOG_WARN(BSL_LS_SOC_PHY,
                (BSL_META_U(ws->unit, "WC_EYE : uController not ready pass 1!: u=%d\n"), ws->unit));
        return (SOC_E_TIMEOUT);
    }

    /* read out the max value */
    SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,  &data16));
    offset_max = (data16 >> 8) & 0xff;

    if (type == WC_UTIL_HEYE_L || type == WC_UTIL_HEYE_R) {
        offset_max -= 4;
        pInfo->offset_max = offset_max;
    } else {
        pInfo->offset_max = offset_max;
    }
  
    SOC_IF_ERROR_RETURN(READ_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws, &data16));

    LOG_INFO(BSL_LS_SOC_PHY,
                (BSL_META_U(ws->unit, "WC_EYE: offset_max %d DSC2B0_ctrl 0x%x u=%d p=%d\n"),
                    offset_max,data16,ws->unit, ws->port));

#ifdef _SDK_WCMOD_
    sal_usleep(10000);  /* 10ms */
#endif 

    hi_confidence_cnt = 0;
    veye_cnt = 0;

    for (loop_var = 0; loop_var < offset_max; loop_var++) {
        offset[loop_var] = offset_max-loop_var;
        veye_cnt += 1;
        /* Set a offset */
        if (type == WC_UTIL_HEYE_R) {
            data16 =-offset[loop_var];
        } else {
            data16 = offset[loop_var];
        }
        /* write vertical offset */
        SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                  data16 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));

        /* 0x8223 register read out */
        SOC_IF_ERROR_RETURN(READ_WC40_DSC3B0_PI_STATUS0r(ws->unit, ws, &clk90_p_offset));
        clk90_p_offset >>= 7;
        clk90_p_offset &= 0x7f;
        tmp = (short)data16;
        LOG_INFO(BSL_LS_SOC_PHY,
                (BSL_META_U(ws->unit, "Starting BER msmt at offset: %d clk90_p_offset: 0x%x u=%d p=%d\n"),
                   tmp,clk90_p_offset,ws->unit, ws->port));

        if (type == WC_UTIL_HEYE_L || type == WC_UTIL_HEYE_R) {
            data16 = 2;
        } else {
            data16 = 3;
        }

        /* set offset cmd */
        SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, data16,
              DSC1B0_UC_CTRL_GP_UC_REQ_MASK));

        /* wait for uC ready for command:  bit7=1    */
        rv = wcmod_regbit_set_wait_check(ws,DSC1B0_UC_CTRLr,
                 DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT);

        if (rv == SOC_E_TIMEOUT) {
            LOG_WARN(BSL_LS_SOC_PHY,
                    (BSL_META_U(ws->unit, "WC_EYE: uC waits for offset fail: u=%d p=%d\n"),ws->unit,ws->port));
            return (SOC_E_TIMEOUT);
        }

        pInfo->total_errs[loop_var] = 0;
        pInfo->total_elapsed_time[loop_var] = 0;

        /* enable PRBS before this function. Clear PRBS error. read a few times */
        SOC_IF_ERROR_RETURN(wcmod_prbs_check(ws));
        SOC_IF_ERROR_RETURN(wcmod_prbs_check(ws));
        SOC_IF_ERROR_RETURN(wcmod_prbs_check(ws));

        for (runtime_loop = 0; runtime_loop <= max_runtime_loop; runtime_loop++) {
            if (runtime_loop == 0) {
                curr_runtime = 1 * MIN_RUNTIME;
            } else {
                curr_runtime = (1 << (runtime_loop - 1)) * MIN_RUNTIME;
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                (BSL_META_U(ws->unit, "Starting prbs run for %d seconds : u=%d p=%d\n"), curr_runtime,
                    ws->unit, ws->port));

            prbs_stky_flag = 1;
            prbs_stky_cnt = 0;
            if (curr_runtime < 4) {
                max_stky_cnt = 4;
            } else {
                max_stky_cnt = 2;
            }

            while (prbs_stky_flag) {
                /* wait for specified amount of time to collect the PRBS error */
#ifdef _SDK_WCMOD_
                sal_usleep(curr_runtime * 1000000);
#endif
                SOC_IF_ERROR_RETURN(wcmod_prbs_check(ws));
            
                prbs_status = ws->accData;
                prbs_stky_flag = 0;
                if (prbs_status == -1) { /* not lock */
                    LOG_WARN(BSL_LS_SOC_PHY,
                            (BSL_META_U(ws->unit, "PRBS not locked, loop_num %d status=%d u=%d p=%d\n"), 
                        loop_var,prbs_status,ws->unit, ws->port));
                    prbs_status = 0x7fff;  /* max error */
                } else if (prbs_status == -2) {
                    LOG_WARN(BSL_LS_SOC_PHY,
                        (BSL_META_U(ws->unit, "PRBS momentarily lost lock, loop_num %d u=%d p=%d\n"),
                                loop_var,ws->unit, ws->port));
                    /* legacy PRBS momentarily lose link, retry limited times */
                    prbs_stky_cnt += 1;
                    if (prbs_stky_cnt < max_stky_cnt) {
                        prbs_stky_flag = 1;
                    } else {
                        prbs_status = 0x7fff; /* default if retry fails */
                    }
                }
            }

            pInfo->total_errs[loop_var] +=  prbs_status / 2;
            pInfo->total_elapsed_time[loop_var] +=  curr_runtime;

            if ((pInfo->total_errs[loop_var] >= HI_CONFIDENCE_ERR_CNT) ||
                     (pInfo->total_elapsed_time[loop_var] >= max_total_time)) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(ws->unit, "WC_EYE: done PRBS err count: u=%d p=%d, total_err: %d"
                                " elapsed_time: %d, loop_num: %d\n"), ws->unit, ws->port,
                                pInfo->total_errs[loop_var],pInfo->total_elapsed_time[loop_var],loop_var));
                break;
            }
        }

        /* Determine high-confidence iterations */
        if (pInfo->total_errs[loop_var] >= HI_CONFIDENCE_ERR_CNT) {
            hi_confidence_cnt = hi_confidence_cnt + 1;
        }

        if (((hi_confidence_cnt >= 2) && (pInfo->total_errs[loop_var] < 
                HI_CONFIDENCE_MIN_ERR_CNT)) ||
               ((hi_confidence_cnt <  2) &&
               (pInfo->total_errs[loop_var] < LO_CONFIDENCE_MIN_ERR_CNT)) ) {
            break;  /* exit for loop */
        }
    }   /* for loop_var */

    /* Undo setup */
    if (type == WC_UTIL_HEYE_L || type == WC_UTIL_HEYE_R) {
        data16 = 2;
    } else {
        data16 = 3;
    }

    /* set vertical offset back to 0 */
    SOC_IF_ERROR_RETURN(WRITE_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, data16));

    /* wait for uC ready for command:  bit7=1 */
    rv = wcmod_regbit_set_wait_check(ws,DSC1B0_UC_CTRLr,
                        DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT);

    if (rv == SOC_E_TIMEOUT) {
        LOG_WARN(BSL_LS_SOC_PHY,
                (BSL_META_U(ws->unit, "WC_VEYE : uC waits for offset=0 fail!: u=%d p=%d\n"),ws->unit, ws->port));
        return (SOC_E_TIMEOUT);
    }

    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws,vga_frzval,
                    DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK));
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws,dfe_frzval,
                    DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK));
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws,vga_frcfrz,
                    DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK));
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws,dfe_frcfrz,
                    DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK));

    /* disable diagnostics */
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 0,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

    /* Clear prbs monitor. Ignore the return value and error count. */
    SOC_IF_ERROR_RETURN (wcmod_prbs_check(ws));
    pInfo->veye_cnt = veye_cnt;

    return SOC_E_NONE;
}

/*
 * enable PRBS31 on both ports before executing this function
 * Example:
 * phy prbs xe0,xe1 set mode=hc p=3
 */
int 
_wcmod_eye_margin_ber_cal(WCMOD_EYE_DIAG_INFOt *pInfo, int type) 
{
#ifdef COMPILER_HAS_DOUBLE
    COMPILER_DOUBLE bers[MAX_LOOPS]; /* computed bit error rate */
    COMPILER_DOUBLE margins[MAX_LOOPS]; /* Eye margin @ each measurement*/
    int loop_var;
    COMPILER_DOUBLE eye_unit;
    COMPILER_DOUBLE curr_ber_log;
    COMPILER_DOUBLE prev_ber_log = 0;
    COMPILER_DOUBLE good_ber_level = -7.8;

    /* Initialize BER array */
    for( loop_var = 0; loop_var < MAX_LOOPS; loop_var++ ) {
        bers[loop_var] = 0.0;
    }

    if (type == WC_UTIL_HEYE_L || type == WC_UTIL_HEYE_R) {
        eye_unit = HEYE_UNIT;
    } else {
        eye_unit = VEYE_UNIT;
    }
 
    LOG_INFO(BSL_LS_SOC_PHY,
                (BSL_META("\nBER measurement at each offset, num_data_points: %d\n"), pInfo->veye_cnt));

    for (loop_var = 0; loop_var < pInfo->veye_cnt; loop_var++) { 
        margins[loop_var] = (pInfo->offset_max-loop_var)*eye_unit;
        LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("BER measurement at offset: %f\n"), margins[loop_var]));
             
        /* Compute BER */
        if (pInfo->total_errs[loop_var] == 0) { 
            bers[loop_var] = 1.0 / (COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_var] / pInfo->rate;
            bers[loop_var] /= 1000;

            LOG_WARN(BSL_LS_SOC_PHY,
                    (BSL_META("BER @ %04f %% = 1e%04f (%d errors in %d seconds)\n"),
                    (COMPILER_DOUBLE)((pInfo->offset_max-loop_var)*eye_unit),
                    1.0*(log10(bers[loop_var])),
                    pInfo->total_errs[loop_var],
                    pInfo->total_elapsed_time[loop_var]));
        } else {
            bers[loop_var] = (COMPILER_DOUBLE)(pInfo->total_errs[loop_var]) /
                             (COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_var] / pInfo->rate;

            /* the rate unit is KHZ, add -3(log10(1/1000)) for actual display  */
            bers[loop_var] /= 1000;
            LOG_WARN(BSL_LS_SOC_PHY,
                    (BSL_META("BER @ %2.2f%% = 1e%2.2f (%d errors in %d seconds)\n"),
                    (pInfo->offset_max-loop_var)*eye_unit,
                    log10(bers[loop_var]),
                    pInfo->total_errs[loop_var],
                    pInfo->total_elapsed_time[loop_var]));
        }
        curr_ber_log = log10(bers[loop_var]);

        /* Detect and record nonmonotonic data points */
        if ((loop_var > 0) && (curr_ber_log > prev_ber_log)) {
            pInfo->mono_flags[loop_var] = 1;
        }
        prev_ber_log = curr_ber_log;
        
        LOG_INFO(BSL_LS_SOC_PHY,
                (BSL_META("cur_be_log %2.2f\n"), curr_ber_log));
        /* find the first data point with good BER */
        if ((curr_ber_log <= good_ber_level) && 
            (pInfo->first_good_ber_idx == INDEX_UNINITIALIZED)) { 
            LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("cur_be_log %2.2f, loop_var %d\n"), curr_ber_log,loop_var));
            pInfo->first_good_ber_idx = loop_var;
        }
        if ((pInfo->total_errs[loop_var] < HI_CONFIDENCE_MIN_ERR_CNT) &&
            (pInfo->first_small_errcnt_idx == INDEX_UNINITIALIZED)) {
             /* find the first data point with small error count */
             pInfo->first_small_errcnt_idx = loop_var;
        }
        
    }   /* for loop_var */

    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}


int  
_wcmod_eye_margin_diagram_cal(WCMOD_EYE_DIAG_INFOt *pInfo, int type) 
{
#ifdef COMPILER_HAS_DOUBLE
    COMPILER_DOUBLE lbers[MAX_LOOPS]; /*Internal linear scale sqrt(-log(ber))*/
    COMPILER_DOUBLE margins[MAX_LOOPS]; /* Eye margin @ each measurement*/
    COMPILER_DOUBLE bers[MAX_LOOPS]; /* computed bit error rate */
    int delta_n;
    COMPILER_DOUBLE Exy = 0.0;
    COMPILER_DOUBLE Eyy = 0.0;
    COMPILER_DOUBLE Exx = 0.0;
    COMPILER_DOUBLE Ey  = 0.0;
    COMPILER_DOUBLE Ex  = 0.0;
    COMPILER_DOUBLE alpha;
    COMPILER_DOUBLE beta;
    COMPILER_DOUBLE proj_ber;
    COMPILER_DOUBLE proj_margin_12;
    COMPILER_DOUBLE proj_margin_15;
    COMPILER_DOUBLE proj_margin_18;
    COMPILER_DOUBLE sq_err2;
    COMPILER_DOUBLE ierr;
    int beta_max=0;
    int ber_conf_scale[20];
    int start_n;
    int stop_n;
    int low_confidence;
    int loop_index;
    COMPILER_DOUBLE outputs[4];
    COMPILER_DOUBLE eye_unit;
    int n_mono;

    /* Initialize BER confidence scale */
    ber_conf_scale[0] = 3.02;
    ber_conf_scale[1] = 4.7863;
    ber_conf_scale[2] = 3.1623;
    ber_conf_scale[3] = 2.6303;
    ber_conf_scale[4] = 2.2909;
    ber_conf_scale[5] = 2.138;
    ber_conf_scale[6] = 1.9953;
    ber_conf_scale[7] = 1.9055;
    ber_conf_scale[8] = 1.8197;
    ber_conf_scale[9] = 1.7783;
    ber_conf_scale[10] = 1.6982;
    ber_conf_scale[11] = 1.6596;
    ber_conf_scale[12] = 1.6218;
    ber_conf_scale[13] = 1.6218;
    ber_conf_scale[14] = 1.5849;
    ber_conf_scale[15] = 1.5488;
    ber_conf_scale[16] = 1.5488;
    ber_conf_scale[17] = 1.5136;
    ber_conf_scale[18] = 1.5136;
    ber_conf_scale[19] = 1.4791;

    LOG_INFO(BSL_LS_SOC_PHY,
                (BSL_META("first_good_ber_idx: %d, first_small_errcnt_idx: %d\n"),
                      pInfo->first_good_ber_idx,pInfo->first_small_errcnt_idx));

    /* Find the highest data point to use, currently based on at least 1E-8 BER level */
    if (pInfo->first_good_ber_idx == INDEX_UNINITIALIZED) {
        start_n = pInfo->veye_cnt;
    } else {
        start_n = pInfo->first_good_ber_idx;
    }
    stop_n = pInfo->veye_cnt;

    /* Find the lowest data point to use */
    if (pInfo->first_small_errcnt_idx == INDEX_UNINITIALIZED) {
        stop_n = pInfo->veye_cnt;
    } else { 
        stop_n = pInfo->first_small_errcnt_idx;
    }

    /* determine the number of non-monotonic outliers */
    n_mono = 0;
    for (loop_index = start_n; loop_index < stop_n; loop_index++) {
        if (pInfo->mono_flags[loop_index] == 1) {
            n_mono = n_mono + 1;
        }
    } 

    if (type == WC_UTIL_HEYE_L || type == WC_UTIL_HEYE_R) {
        eye_unit = HEYE_UNIT;
    } else {
        eye_unit = VEYE_UNIT;
    }

    for (loop_index = 0; loop_index < pInfo->veye_cnt; loop_index++) {
        if (pInfo->total_errs[loop_index] == 0) {
            bers[loop_index] = 1.0 / (COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_index] / pInfo->rate;
        } else {
            bers[loop_index] = (COMPILER_DOUBLE)pInfo->total_errs[loop_index] /
                               (COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_index] / pInfo->rate;
        }
        bers[loop_index] /= 1000;
        margins[loop_index] = (pInfo->offset_max-loop_index) * eye_unit;
    }

    if( start_n >= pInfo->veye_cnt ) {
        outputs[0] = -_wcmod_util_round_real(log(bers[pInfo->veye_cnt-1]) / log(10), 1);
        outputs[1] = -100.0;
        outputs[2] = -100.0;
        outputs[3] = -100.0;
	/*  No need to print out the decimal portion of the BER */
	LOG_CLI((BSL_META("BER *worse* than 1e-%d\n"), (int)outputs[0]));
	LOG_CLI((BSL_META("Negative margin @ 1e-12, 1e-15 & 1e-18\n")));
    } else {
        low_confidence = 0;
	if( (stop_n-start_n - n_mono) < 2  ) {
	    /* Code triggered when less than 2 statistically valid extrapolation points */
            for( loop_index = stop_n; loop_index < pInfo->veye_cnt; loop_index++ ) {
	        if( pInfo->total_errs[loop_index] < 20 ) {
		    bers[loop_index] = ber_conf_scale[pInfo->total_errs[loop_index]] * bers[loop_index];
		} else {
		    bers[loop_index] = ber_conf_scale[19] * bers[loop_index];
		}
                pInfo->mono_flags[loop_index] = 0;    /* remove flags; or assess again */
	    }
	    /* Add artificial point at 100% margin to enable interpolation */
            margins[pInfo->veye_cnt] = 100.0;
            bers[pInfo->veye_cnt] = 0.1;
            low_confidence = 1;
            stop_n = pInfo->veye_cnt + 1;
	}

	/* Below this point the code assumes statistically valid point available */
        delta_n = stop_n - start_n - n_mono;

        LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("start_n: %d, stop_n: %d, veye: %d, n_mono: %d\n"),
                          start_n,stop_n,pInfo->veye_cnt,n_mono));

	/* Find all the correlations */
	for( loop_index = start_n; loop_index < stop_n; loop_index++ ) {
	    lbers[loop_index] = (COMPILER_DOUBLE)sqrt(-log(bers[loop_index]));
	}

        LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("\tstart=%d, stop=%d, low_confidence=%d\n"),
                                     start_n, stop_n, low_confidence));
        for (loop_index=start_n; loop_index < stop_n; loop_index++){
            LOG_INFO(BSL_LS_SOC_PHY,
                        (BSL_META("\ttotal_errs[%d]=0x%08x\n"),
                         loop_index,(int)pInfo->total_errs[loop_index]));
            LOG_INFO(BSL_LS_SOC_PHY,
                        (BSL_META("\tbers[%d]=%f\n"),
                         loop_index,bers[loop_index]));
            LOG_INFO(BSL_LS_SOC_PHY,
                        (BSL_META("\tlbers[%d]=%f\n"),
                          loop_index,lbers[loop_index]));
	}

	for( loop_index = start_n; loop_index < stop_n; loop_index++ ) {
            if (pInfo->mono_flags[loop_index] == 0) {
	        Exy = Exy + margins[loop_index] * lbers[loop_index] / (COMPILER_DOUBLE)delta_n;
	        Eyy = Eyy + lbers[loop_index]*lbers[loop_index] / (COMPILER_DOUBLE)delta_n;
	        Exx = Exx + margins[loop_index]*margins[loop_index] / (COMPILER_DOUBLE)delta_n;
	        Ey  = Ey + lbers[loop_index] / (COMPILER_DOUBLE)delta_n;
	        Ex  = Ex + margins[loop_index]/(COMPILER_DOUBLE)delta_n;
            }
	}

	/* Compute fit slope and offset */
        alpha = (Exy - Ey * Ex) / (Exx - Ex * Ex);
        beta = Ey - Ex * alpha;
	
        LOG_INFO(BSL_LS_SOC_PHY,
	            (BSL_META("Exy=%f, Eyy=%f, Exx=%f, Ey=%f,Ex=%f alpha=%f, beta=%f\n"),
                         Exy,Eyy,Exx,Ey,Ex,alpha,beta));

	/* JPA> Due to the limit of floats, I need to test for a maximum Beta of 9.32 */
	if(beta > 9.32){
	    beta_max=1;
            LOG_INFO(BSL_LS_SOC_PHY,
	                (BSL_META("\n\tWARNING: intermediate float variable is maxed out, what this means is:\n")));
            LOG_INFO(BSL_LS_SOC_PHY,
	                (BSL_META("\t\t- The *extrapolated* minimum BER will be reported as 1E-37.\n")));
            LOG_INFO(BSL_LS_SOC_PHY,
	                (BSL_META("\t\t- This may occur if the channel is near ideal (e.g. test loopback)\n")));
            LOG_INFO(BSL_LS_SOC_PHY,
	                (BSL_META("\t\t- While not discrete, reporting an extrapolated BER < 1E-37 is numerically "
                                "corect, and informative\n\n")));
	}

       
        proj_ber = exp(-beta * beta);
        proj_margin_12 = (sqrt(-log(1e-12)) - beta) / alpha;
        proj_margin_15 = (sqrt(-log(1e-15)) - beta) / alpha;
        proj_margin_18 = (sqrt(-log(1e-18)) - beta) / alpha;

        sq_err2 = 0;
	for( loop_index = start_n; loop_index < stop_n; loop_index++ ) {
	    ierr = (lbers[loop_index] - (alpha * margins[loop_index] + beta));
	    sq_err2 = sq_err2 + ierr * ierr / (COMPILER_DOUBLE)delta_n;
        }

        outputs[0] = -_wcmod_util_round_real(log(proj_ber) / log(10),1);
	outputs[1] = _wcmod_util_round_real(proj_margin_18,1);
        outputs[2] = _wcmod_util_round_real(proj_margin_12,1);
        outputs[3] = _wcmod_util_round_real(proj_margin_15,1);

        LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("\t\tlog1e-12=%f, sq=%f\n"),(COMPILER_DOUBLE)log(1e-12),(COMPILER_DOUBLE)sqrt(-log(1e-12))));
        LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("\t\talpha=%f\n"),alpha));
        LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("\t\tbeta=%f\n"),beta));
        LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("\t\tproj_ber=%f\n"),proj_ber));
        LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("\t\tproj_margin12=%f\n"),proj_margin_12));
        LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("\t\tproj_margin12=%f\n"),proj_margin_15));
        LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("\t\tproj_margin18=%f\n"),proj_margin_18));
        LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("\t\toutputs[0]=%f\n"),outputs[0]));
        LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("\t\toutputs[1]=%f\n"),outputs[1]));
        LOG_INFO(BSL_LS_SOC_PHY,
                    (BSL_META("\t\toutputs[2]=%f\n"),outputs[2]));

        /* Extrapolated results, low confidence */
        if( low_confidence == 1 ) {
            if(beta_max){
                LOG_CLI((BSL_META("BER(extrapolated) is *better* than 1e-37\n")));
                LOG_CLI((BSL_META("Margin @ 1e-12    is *better* than %f\n"), outputs[2]));
                LOG_CLI((BSL_META("Margin @ 1e-15    is *better* than %f\n"), outputs[3]));
                LOG_CLI((BSL_META("Margin @ 1e-18    is *better* than %f\n"), outputs[1]));
            } else {
                LOG_CLI((BSL_META("BER(extrapolated) is *better* than 1e-%f\n"), outputs[0]));
                LOG_CLI((BSL_META("Margin @ 1e-12    is *better* than %f\n"), outputs[2]));
                LOG_CLI((BSL_META("Margin @ 1e-15    is *better* than %f\n"), outputs[3]));
                LOG_CLI((BSL_META("Margin @ 1e-18    is *better* than %f\n"), outputs[1]));
          }

        /* JPA> Extrapolated results, high confidence */
        } else {           
            if(beta_max){
                LOG_CLI((BSL_META("BER(extrapolated) = 1e-37\n")));
                LOG_CLI((BSL_META("Margin @ 1e-12    is *better* than %f\n"), outputs[2]));
                LOG_CLI((BSL_META("Margin @ 1e-15    is *better* than %f\n"), outputs[3]));
                LOG_CLI((BSL_META("Margin @ 1e-18    is *better* than %f\n"), outputs[1]));
            } else {
                LOG_CLI((BSL_META("BER(extrapolated) = 1e-%4.2f\n"), outputs[0]));
                LOG_CLI((BSL_META("Margin @ 1e-12    = %4.2f%%\n"), outputs[2]));
                LOG_CLI((BSL_META("Margin @ 1e-15    = %4.2f%%\n"), outputs[3]));
                LOG_CLI((BSL_META("Margin @ 1e-18    = %4.2f%%\n"), outputs[1]));
            }
        }
    }
    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}

/* to get the ppm units, the 2's complement value
must be divided by 83.89 for dsc c0 and 83.89*1.65 for dsc b0 */

/*  if (p1even & 0x20) {ip1even=((1+~(p1even & 0x1f)) & 0x1f); p1evenc = '-';}  */

int wcmod_diag_rx_ppm(wcmod_st *ws)
{
  uint16 data, idata;
  float fdata;
  char  datac=' ';

  READ_WC40_DSC3B0_CDR_STATUS0r(ws->unit, ws, &data);
 /* data = ~data + 1;  take the 2s compliment) */
  idata = data;if (data & 0x8000) {idata = ((1 + ~(data & 0x7fff)) & 0x7fff); datac = '-';} 
  if (ws->model_type == WCMOD_WL_A0 || ws->model_type == WCMOD_WC_C0) {
    fdata = (((float)idata) / 83.89);
  } else {
    fdata = (((float)idata) / (1.65 * 83.89));
  }
  ws->accData = (int) fdata;
  printf("%s PPM: 0x%02x(%c%f)\n", FUNCTION_NAME(), data, datac, fdata);
  return SOC_E_NONE;
}

#endif /* #ifndef _KERNEL_ */

/*!

\details
Perturb main data slicer horizontally/vertically. Measure BER to estimate error
rate for different slicer states. We use PRBS for (user selected polynomial)
for BER test. PRBS is sent for various durations, BERs recorded and real BER
is extrapolated using linear fit algorithm.

Slicer perturbation direction is controlled by #wcmod_st::wcmod_eye_slicer.
Vertical   movement is 24 steps from 0 to 40% in 1.75%
Horizontal movement is 64 steps from -0.5UI to +0.5UI

You can choose one mode of slicer pertabation only at a time. For vertical, the
algorithm will choose the direction. For horizontal you can choose
horizontal-left or horizonal-right with #wcmod_st::per_lane_control as follows.
(note: it is not one-hot coded)

\li #wcmod_st::per_lane_control 0x1 chooses live-link eyescan
\li #wcmod_st::per_lane_control 0x0 chooses prbs eyescan

*/
int wcmod_diag_eye(wcmod_st *ws)
{
  int rv = 0;
#ifndef __KERNEL__
  switch (ws->per_lane_control) {
#if 0
    case WC_UTIL_2D_LIVELINK_EYESCAN:
        rv = _wcmod_2d_livelink_eye_margin_data_get(ws);
        break;
    case WC_UTIL_2D_PRBS_EYESCAN:
        rv = _wcmod_2d_prbs_eye_margin_data_get(ws);
        break;
#endif
    case WC_UTIL_ENABLE_LIVELINK:
        rv = _wcmod_enable_livelink_diagnostics(ws);
        break;
    case WC_UTIL_DISABLE_LIVELINK:
        rv = _wcmod_disable_livelink_diagnostics(ws);
        break;
    case WC_UTIL_ENABLE_DEADLINK:
        rv = _wcmod_enable_deadlink_diagnostics(ws);
        break;
    case WC_UTIL_DISABLE_DEADLINK:
        rv = _wcmod_disable_deadlink_diagnostics(ws);
        break;                                                               
    case WC_UTIL_SET_VOFFSET:
        rv = _wcmod_set_voff(ws, ws->accData);
        break;
    case WC_UTIL_SET_HOFFSET:
        rv = _wcmod_set_hoff(ws, ws->accData);
        break;
    case WC_UTIL_GET_MAX_VOFFSET:
        rv = _wcmod_get_max_voffset(ws);
        break;
    case WC_UTIL_GET_MIN_VOFFSET:
        rv= _wcmod_get_min_voffset(ws); 
        break;
    case WC_UTIL_GET_INIT_VOFFSET:
        rv= _wcmod_get_init_voffset(ws); 
        break;
    case WC_UTIL_GET_MAX_LEFT_HOFFSET:
        rv = _wcmod_get_max_hoffset_left(ws);
        break;
    case WC_UTIL_GET_MAX_RIGHT_HOFFSET:
        rv = _wcmod_get_max_hoffset_right(ws);
        break;
    case WC_UTIL_START_LIVELINK:
        rv = _wcmod_start_livelink_diagnostics(ws);
        break;
    case WC_UTIL_START_DEADLINK:
        rv = _wcmod_start_deadlink_diagnostics(ws);
        break;
    case WC_UTIL_STOP_LIVELINK:
        rv = _wcmod_stop_livelink_diagnostics(ws);
        break;
    case WC_UTIL_CLEAR_LIVELINK:
        rv = _wcmod_clear_livelink_diagnostics(ws);
        break;
    case WC_UTIL_READ_LIVELINK:
        rv = _wcmod_read_livelink_diagnostics(ws);
        break;
    case WC_UTIL_READ_DEADLINK:
        rv = _wcmod_read_deadlink_diagnostics(ws);
        break;
    default:
        break;
  }            
#endif /* # !__KERNEL__ */

  return rv;
}

int wcmod_diag_slicers(wcmod_st *ws)
{
  uint16 data, p1even, p1odd, deven, dodd, m1even, m1odd;
  int    ip1even, ip1odd, ideven, idodd, im1even, im1odd;
  char   p1evenc=' ',p1oddc=' ', devenc=' ', doddc=' ', m1evenc=' ', m1oddc=' ';

  READ_WC40_DSC2B0_DSC_ANA_CTRL0r (ws->unit, ws, &data);
  p1even = (data & DSC2B0_DSC_ANA_CTRL0_P1_EVN_CTRL_MASK) >>
            DSC2B0_DSC_ANA_CTRL0_P1_EVN_CTRL_SHIFT;
  p1odd  = (data & DSC2B0_DSC_ANA_CTRL0_P1_ODD_CTRL_MASK) >>
            DSC2B0_DSC_ANA_CTRL0_P1_ODD_CTRL_SHIFT;
  if (ws->accData == 1) ws->accData =  ((p1even << 8) | p1odd);

  READ_WC40_DSC2B0_DSC_ANA_CTRL2r (ws->unit, ws, &data);
  m1even = (data & DSC2B0_DSC_ANA_CTRL2_M1_EVN_CTRL_MASK) >>
                DSC2B0_DSC_ANA_CTRL2_M1_EVN_CTRL_SHIFT;
  m1odd  = (data & DSC2B0_DSC_ANA_CTRL2_M1_ODD_CTRL_MASK) >>
                DSC2B0_DSC_ANA_CTRL2_M1_ODD_CTRL_SHIFT;
  if (ws->accData == 2) ws->accData =  ((m1even << 8) | m1odd);
  
  READ_WC40_DSC2B0_DSC_ANA_CTRL1r (ws->unit, ws, &data);
  deven = (data & DSC2B0_DSC_ANA_CTRL1_D_EVN_CTRL_MASK) >>
                DSC2B0_DSC_ANA_CTRL1_D_EVN_CTRL_SHIFT;
  dodd  = (data & DSC2B0_DSC_ANA_CTRL1_D_ODD_CTRL_MASK) >>
                DSC2B0_DSC_ANA_CTRL1_D_ODD_CTRL_SHIFT;
  if (ws->accData == 3) ws->accData =  ((deven << 8) | dodd);

  ip1even = p1even;
  ip1odd  = p1odd;
  ideven  = deven;
  idodd   = dodd;
  im1even = m1even;
  im1odd  = m1odd;
  if (p1even & 0x20) {ip1even = ((1 + ~(p1even & 0x1f)) & 0x1f); p1evenc = '-';}
  if (p1odd  & 0x20) {ip1odd = ((1 + ~(p1odd  & 0x1f)) & 0x1f); p1oddc  = '-';}
  if (deven  & 0x20) {ideven = ((1 + ~(deven  & 0x1f)) & 0x1f); devenc  = '-';}
  if (dodd   & 0x20) {idodd  = ((1 + ~(dodd   & 0x1f)) & 0x1f); doddc   = '-';}
  if (m1even & 0x20) {im1even = ((1 + ~(m1even & 0x1f)) & 0x1f); m1evenc = '-';}
  if (m1odd  & 0x20) {im1odd = ((1 + ~(m1odd  & 0x1f)) & 0x1f); m1oddc  = '-';}
  
  printf("%s P1Eeven:0x%02x(%c%d) P1Odd:0x%02x(%c%d) DEven:0x%02x(%c%d) DOdd:0x%02x(%c%d) M1Even:0x%02x(%c%d) M1Odd:0x%02x(%c%d)\n", FUNCTION_NAME(),
         p1even,p1evenc, ip1even,
         p1odd ,p1oddc,  ip1odd,
         deven ,devenc,  ideven,
         dodd  ,doddc,   idodd,
         m1even,m1evenc, im1even,
         m1odd ,m1oddc,  im1odd);
  return SOC_E_NONE;
}

int wcmod_diag_tx_amps(wcmod_st *ws)
{
  uint16 dt = 0, idrv = 0, predrv = 0, p2_cf = 0, tmp_lane=0;

  if (ws->model_type == WCMOD_QS_A0) {
    tmp_lane = ws->this_lane;
    ws->this_lane = (tmp_lane / 4) * 4;
  } else {
    tmp_lane = ws->this_lane;
  }

  if ((ws->model_type != WCMOD_QS_A0) && (tmp_lane >= 4)) {
    printf ("%s FATAL: Internal. Bad lane:%d\n", FUNCTION_NAME(), tmp_lane);
    return SOC_E_ERROR;
  }
  if (ws->model_type == WCMOD_QS_A0) {
     READ_WC40_TX0_TX_DRIVERr(ws->unit, ws, &dt);
     idrv  =(dt & 0x3f << 6) >> 6;
     printf ("Tx Amps. Lane %d: IDriver:%d \n", tmp_lane,idrv);
  } else { 
      if (ws->this_lane % 4 == 0)  {
        READ_WC40_TX0_TX_DRIVERr(ws->unit, ws, &dt);
      } else if (ws->this_lane % 4 == 1) {
        READ_WC40_TX1_TX_DRIVERr(ws->unit, ws, &dt);
      } else if (ws->this_lane % 4 == 2) {
        READ_WC40_TX2_TX_DRIVERr(ws->unit, ws, &dt);
      } else {
        READ_WC40_TX3_TX_DRIVERr(ws->unit, ws, &dt);
      }
      idrv  =(dt & TX0_TX_DRIVER_IDRIVER_MASK) >> TX0_TX_DRIVER_IDRIVER_SHIFT;
      predrv=(dt & TX0_TX_DRIVER_IPREDRIVER_MASK) >> TX0_TX_DRIVER_IPREDRIVER_SHIFT;
      p2_cf =(dt & TX0_TX_DRIVER_POST2_COEFF_MASK) >> TX0_TX_DRIVER_POST2_COEFF_SHIFT;
      printf ("Tx Amps. Lane %d: IDriver:%d PreDriver:%d P2_Coeff:%d\n",
                                                   tmp_lane,idrv,predrv,p2_cf);
  }

  if ((ws->accData) == 0x1)        {
    ws->accData = predrv;
  } else if ((ws->accData) == 0x2) {
    ws->accData = idrv;
  } else if ((ws->accData) == 0x3) {
    ws->accData = p2_cf;
  }
  ws->this_lane = tmp_lane;
  return SOC_E_NONE;
}

int wcmod_diag_prbs(wcmod_st* ws)
{
    uint16      data;
    ws->lane_num_ignore = 1;
    SOC_IF_ERROR_RETURN
        (READ_WC40_XGXSBLK1_LANEPRBSr(ws->unit, ws, &data));

    /* Extract prbs field setting from register */
    data = (data >> (XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT * ws->this_lane)); 
    ws->accData = data;
    ws->lane_num_ignore = 0;
    return SOC_E_NONE;
}

int wcmod_diag_prbs_decouple_tx(wcmod_st* ws)
{
    uint16      data = 0;
    switch (ws->this_lane) {
    case 0: 
        SOC_IF_ERROR_RETURN
            (READ_WC40_TX0_ANATXACONTROL5r(ws->unit, ws, &data));
        break;
    case 1: 
        SOC_IF_ERROR_RETURN
            (READ_WC40_TX1_ANATXACONTROL5r(ws->unit, ws, &data));
        break;
    case 2: 
        SOC_IF_ERROR_RETURN
            (READ_WC40_TX2_ANATXACONTROL5r(ws->unit, ws, &data));
        break;
    case 3: 
        SOC_IF_ERROR_RETURN
            (READ_WC40_TX3_ANATXACONTROL5r(ws->unit, ws, &data));
        break;
    default:
        SOC_IF_ERROR_RETURN
            (READ_WC40_TX0_ANATXACONTROL5r(ws->unit, ws, &data));
        break;
    }

    /* Extract prbs field setting from register */
    if (ws->per_lane_control == 0x1) {
        /*get enable field */
        ws->accData = (data & TX0_ANATXACONTROL5_TX_PRBS_EN_DECOUPLE_MASK) >>
                       TX0_ANATXACONTROL5_TX_PRBS_EN_DECOUPLE_SHIFT;
    } else if (ws->per_lane_control == 0x2) {
        /*get invert data info */
        ws->accData = (data & TX0_ANATXACONTROL5_TX_PRBS_INV_DECOUPLE_MASK) >>
                       TX0_ANATXACONTROL5_TX_PRBS_INV_DECOUPLE_SHIFT;
    } else {
        /*get the prbs order  info */
        ws->accData = (data & TX0_ANATXACONTROL5_TX_PRBS_ORDER_DECOUPLE_MASK) >>
                       TX0_ANATXACONTROL5_TX_PRBS_ORDER_DECOUPLE_SHIFT;
        ws->accData |= (data & TX0_ANATXACONTROL5_TX_PRBS_ORDER_2_MASK) >>
                       (TX0_ANATXACONTROL5_TX_PRBS_ORDER_2_SHIFT - 2);
    }
                            
    return SOC_E_NONE;
}

int wcmod_diag_prbs_decouple_rx(wcmod_st* ws)
{
    uint16      data = 0;
    switch (ws->this_lane) {
    case 0: 
        SOC_IF_ERROR_RETURN
            (READ_WC40_RX0_ANARXCONTROL2_1Gr(ws->unit, ws, &data));
        break;
    case 1: 
        SOC_IF_ERROR_RETURN
            (READ_WC40_RX1_ANARXCONTROL2_1Gr(ws->unit, ws, &data));
        break;
    case 2: 
        SOC_IF_ERROR_RETURN
            (READ_WC40_RX2_ANARXCONTROL2_1Gr(ws->unit, ws, &data));
        break;
    case 3: 
        SOC_IF_ERROR_RETURN
            (READ_WC40_RX3_ANARXCONTROL2_1Gr(ws->unit, ws, &data));
        break;
    default:
        SOC_IF_ERROR_RETURN
            (READ_WC40_RX0_ANARXCONTROL2_1Gr(ws->unit, ws, &data));
        break;
    }

    /* Extract prbs field setting from register */
    if (ws->per_lane_control == 0x1) {
        /*get enable field */
        ws->accData = (data & RX0_ANARXCONTROL2_1G_PRBS_EN_RX_R_MASK) >>
                       RX0_ANARXCONTROL2_1G_PRBS_EN_RX_R_SHIFT;
    } else if (ws->per_lane_control == 0x2) {
        /*get invert data info */
        ws->accData = (data & RX0_ANARXCONTROL2_1G_PRBS_INV_RX_R_MASK) >>
                       RX0_ANARXCONTROL2_1G_PRBS_INV_RX_R_SHIFT;
    } else {
        /*get the prbs order  info */
        ws->accData = (data & RX0_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_MASK) >>
                       RX0_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_SHIFT;
        ws->accData |= (data & RX0_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_2_MASK) >>
                       (RX0_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_2_SHIFT - 2);
    }
                            
    return SOC_E_NONE;
}


/* WCMOD_DIAG_TX_TAPS */
int wcmod_diag_tx_taps(wcmod_st *ws)
{
  uint16 dt = 0, pre = 0, mn = 0, post = 0, tmp_lane = 0;

  if (ws->model_type == WCMOD_QS_A0) {
    tmp_lane = ws->this_lane;
    ws->this_lane = (tmp_lane / 4) * 4;
  } else {
    tmp_lane = ws->this_lane;
  }

  if ((ws->model_type != WCMOD_QS_A0) && (tmp_lane >= 4)) {
    printf ("%s FATAL: Internal. Bad lane:%d\n", FUNCTION_NAME(), tmp_lane);
    return SOC_E_ERROR;
  }


  if (ws->model_type == WCMOD_XN) {
      switch (ws->this_lane) {
      case 0:
          SOC_IF_ERROR_RETURN
              (READ_WC40_TX0_ANATXACONTROL1r(ws->unit, ws, &dt));
          break;
      case 1:
          SOC_IF_ERROR_RETURN
              (READ_WC40_TX1_ANATXACONTROL1r(ws->unit, ws, &dt));
          break;
      case 2:
          SOC_IF_ERROR_RETURN
              (READ_WC40_TX2_ANATXACONTROL1r(ws->unit, ws, &dt));
          break;
      case 3:
          SOC_IF_ERROR_RETURN
              (READ_WC40_TX3_ANATXACONTROL1r(ws->unit, ws, &dt));
          break;
      default:
          break;
      }
      ws->accData = (dt & 0x7800) >> 11;
  } else if (ws->model_type == WCMOD_QS_A0) {
      SOC_IF_ERROR_RETURN
          (READ_WC40_TX0_ANATXACONTROL2r(ws->unit,ws, &dt));
      pre = (dt & 0x8000) >> 15;
      mn =  (dt & 0x7c00) >> 10;
      SOC_IF_ERROR_RETURN
          (READ_WC40_TX0_TX_DRIVERr(ws->unit,ws, &dt));
      post = (dt & 0xf) << 1 | pre;
      pre = 0;
      ws->this_lane = tmp_lane;
      ws->accData = pre | (mn <<4) | (post <<10) ;
  } else {
    switch (ws->this_lane) {
    case 1:
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX1_ANATXMDATA1r(ws->unit, ws, 1 << TX1_ANATXMDATA1_TXSTATUSSELECT_SHIFT, 
             TX1_ANATXMDATA1_TXSTATUSSELECT_MASK));
        SOC_IF_ERROR_RETURN
            (READ_WC40_TX1_ANATXASTATUS0r(ws->unit, ws, &dt)); 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX1_ANATXMDATA1r(ws->unit, ws, 0, 
             TX1_ANATXMDATA1_TXSTATUSSELECT_MASK));
        break;
    case 2:
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX2_ANATXMDATA1r(ws->unit, ws, 1 << TX2_ANATXMDATA1_TXSTATUSSELECT_SHIFT, 
             TX2_ANATXMDATA1_TXSTATUSSELECT_MASK));
        SOC_IF_ERROR_RETURN
            (READ_WC40_TX2_ANATXASTATUS0r(ws->unit, ws, &dt)); 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX2_ANATXMDATA1r(ws->unit, ws, 0, 
             TX2_ANATXMDATA1_TXSTATUSSELECT_MASK));
        break;
    case 3:
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX3_ANATXMDATA1r(ws->unit, ws, 1 << TX3_ANATXMDATA1_TXSTATUSSELECT_SHIFT, 
             TX3_ANATXMDATA1_TXSTATUSSELECT_MASK));
        SOC_IF_ERROR_RETURN
            (READ_WC40_TX3_ANATXASTATUS0r(ws->unit, ws, &dt)); 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX3_ANATXMDATA1r(ws->unit, ws, 0, 
             TX3_ANATXMDATA1_TXSTATUSSELECT_MASK));
        break;
    default:
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX0_ANATXMDATA1r(ws->unit, ws, 1 << TX0_ANATXMDATA1_TXSTATUSSELECT_SHIFT, 
             TX0_ANATXMDATA1_TXSTATUSSELECT_MASK));
        SOC_IF_ERROR_RETURN
            (READ_WC40_TX0_ANATXASTATUS0r(ws->unit, ws, &dt)); 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX0_ANATXMDATA1r(ws->unit, ws, 0, 
             TX0_ANATXMDATA1_TXSTATUSSELECT_MASK));
        break;
    }
    ws->accData = dt & (~CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK);
    /* tx_pre_cursor */
    pre = (dt & TX0_ANATXASTATUS0_TXSTATUSSELECT_EQUALS_1_TX_FIR_TAP_PRE_MASK)
                            >> TX0_ANATXASTATUS0_TXSTATUSSELECT_EQUALS_1_TX_FIR_TAP_PRE_SHIFT;
    /* tx_main */
    mn = (dt & TX0_ANATXASTATUS0_TXSTATUSSELECT_EQUALS_1_TX_FIR_TAP_MAIN_MASK)
                      >> TX0_ANATXASTATUS0_TXSTATUSSELECT_EQUALS_1_TX_FIR_TAP_MAIN_SHIFT;
    /* tx_post_cursor */
    post = (dt & TX0_ANATXASTATUS0_TXSTATUSSELECT_EQUALS_1_TX_FIR_TAP_POST_MASK)
                             >> TX0_ANATXASTATUS0_TXSTATUSSELECT_EQUALS_1_TX_FIR_TAP_POST_SHIFT;

  }

  if (ws->model_type != WCMOD_XN) {
      printf ("Tx_Taps, Lane:%d: Pre:0x%02x Main:0x%02x Post:0x%02x\n",
               tmp_lane, pre,mn  ,post);
  }

  return SOC_E_NONE;
}

/* WCMOD_DIAG_RX_TAPS */
int wcmod_diag_rx_taps(wcmod_st* ws)
{
  uint16 dt, vga, t1, t2, t3, t4, t5, pf, low_pf, tempData;

  tempData = 0;
  
  SOC_IF_ERROR_RETURN(READ_WC40_DSC2B0_DSC_ANA_CTRL4r(ws->unit, ws, &dt));
  low_pf = (dt & DSC2B0_DSC_ANA_CTRL4_RX_PF2_LOWP_CTRL_MASK) >>
                                    DSC2B0_DSC_ANA_CTRL4_RX_PF2_LOWP_CTRL_SHIFT;
  SOC_IF_ERROR_RETURN(READ_WC40_DSC3B0_ANA_STATUS0r(ws->unit, ws, &dt));
  pf = (dt & DSC3B0_ANA_STATUS0_PF_CTRL_BIN_MASK) >>
                                    DSC3B0_ANA_STATUS0_PF_CTRL_BIN_SHIFT;
  SOC_IF_ERROR_RETURN(READ_WC40_DSC3B0_DFE_VGA_STATUS0r(ws->unit, ws, &dt));
  vga =(dt & DSC3B0_DFE_VGA_STATUS0_VGA_SUM_MASK) >>
                                          DSC3B0_DFE_VGA_STATUS0_VGA_SUM_SHIFT;
  t1 = (dt & DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_MASK) >>
                                    DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_SHIFT;
  SOC_IF_ERROR_RETURN(READ_WC40_DSC3B0_DFE_VGA_STATUS1r(ws->unit, ws, &dt));
  t2 = (dt & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_2_BIN_MASK) >>
                                    DSC3B0_DFE_VGA_STATUS1_DFE_TAP_2_BIN_SHIFT;
  t3 = (dt & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_MASK) >>
                                    DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_SHIFT;
  SOC_IF_ERROR_RETURN(READ_WC40_DSC3B0_DFE_VGA_STATUS2r(ws->unit, ws, &dt));
  t4 = (dt & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_4_BIN_MASK) >>
                                    DSC3B0_DFE_VGA_STATUS2_DFE_TAP_4_BIN_SHIFT;
  t5 = (dt & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_MASK) >>
                                    DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_SHIFT;
  switch (ws->accData) {
    case 1:
        tempData = pf;
        break;
    case 2:
        tempData = vga;
        break;
    case 3:
        tempData = t1;
        break;
    case 4:
        tempData = t2;
        break;
    case 5:
        tempData = t3;
        break;
    case 6:
        tempData = t4;
        break;
    case 7:
        tempData = t5;
        break;
    case 8:
        tempData = low_pf;
        break;
    default:
        break;
  }

  ws->accData = tempData;
  printf("Rx_Taps, Lane:%d: VGA:0x%02x, TAP1:0x%02x, TAP2:0x%02x, TAP3:0x%02x, TAP4:0x%02x, TAP5:0x%02x, PF:0x%02x, LOW_PF:0x%02x \n",
                                ws->this_lane, vga, t1, t2, t3, t4, t5, pf, low_pf);
  return SOC_E_NONE;
}

/* WCMOD_DIAG_RX_SIGDET. signal detect found?*/
int wcmod_diag_rx_sigdet(wcmod_st* ws)
{
  uint16 dt;

  ws->lane_num_ignore = 1;
  SOC_IF_ERROR_RETURN(READ_WC40_GP2_REG_GP2_0r(ws->unit, ws, &dt));
  ws->lane_num_ignore = 0;
  ws->accData = (dt & (1 << (GP2_REG_GP2_0_SIGDET_SHIFT + ws->this_lane)));
   if (ws->verbosity > 0) printf("%s. SigDet Status: (Raw Reg:0x%x)0x%x\n", FUNCTION_NAME(), dt,
               ((dt & GP2_REG_GP2_0_SIGDET_MASK) >> GP2_REG_GP2_0_SIGDET_SHIFT));
  return SOC_E_NONE;
}

/* WCMOD_DIAG_TX_SQUELCH. TX got squelched*/
int wcmod_diag_tx_squelch(wcmod_st* ws)
{
  uint16 dt;
  switch(ws->this_lane) {
    case 0:
        SOC_IF_ERROR_RETURN(
		    READ_WC40_TX0_TX_DRIVERr(ws->unit,ws, &dt));
        ws->accData = (dt & TX0_TX_DRIVER_ELECIDLE_MASK) >> TX0_TX_DRIVER_ELECIDLE_SHIFT;
      break;
    case 1:
        SOC_IF_ERROR_RETURN(
		    READ_WC40_TX1_TX_DRIVERr(ws->unit,ws, &dt));
        ws->accData = (dt & TX1_TX_DRIVER_ELECIDLE_MASK) >> TX1_TX_DRIVER_ELECIDLE_SHIFT;
      break;
    case 2:
        SOC_IF_ERROR_RETURN(
		    READ_WC40_TX2_TX_DRIVERr(ws->unit,ws, &dt));
        ws->accData = (dt & TX2_TX_DRIVER_ELECIDLE_MASK) >> TX2_TX_DRIVER_ELECIDLE_SHIFT;
      break;
    case 3:
        SOC_IF_ERROR_RETURN(
		    READ_WC40_TX3_TX_DRIVERr(ws->unit,ws, &dt));
        ws->accData = (dt & TX3_TX_DRIVER_ELECIDLE_MASK) >> TX3_TX_DRIVER_ELECIDLE_SHIFT;
      break;
    default:
      break;
  }
  return SOC_E_NONE;
}


int wcmod_tx_rx_polarity_get(wcmod_st *pc)
{
    uint16 data = 0;
    int lane, is_tx;
    is_tx =  pc->per_lane_control;
    lane =(1 << pc->this_lane);  
    if(!is_tx) {
      /* RX lane 0 */
        if(lane & (0x1 << 0)) {
          READ_WC40_RX0_ANARXCONTROLPCIr(pc->unit, pc,&data);
        }
         /* RX lane 1 */
         if(lane & (0x1 << 1)) {
            READ_WC40_RX1_ANARXCONTROLPCIr(pc->unit, pc,&data);
         }
         /* RX lane 2 */
         if(lane & (0x1 << 2)) {
            READ_WC40_RX2_ANARXCONTROLPCIr(pc->unit, pc,&data);
         }
         /* RX lane 3 */
         if(lane & (0x1 << 3)) {
            READ_WC40_RX3_ANARXCONTROLPCIr(pc->unit, pc,&data);
         }
            data = ((data >> RX0_ANARXCONTROLPCI_RX_POLARITY_R_SHIFT) & 0x0001);
    } else {  
          /* TX lane 0 */
          if(lane & (0x1 << 0)) {
            READ_WC40_TX0_ANATXACONTROL0r(pc->unit, pc,&data);
          }
          
          /* TX lane 1 */
          if(lane & (0x1 << 1)) {
            READ_WC40_TX1_ANATXACONTROL0r(pc->unit, pc,&data);
         }
         
         /* TX lane 2 */
         if(lane & (0x1 << 2)) {
            READ_WC40_TX2_ANATXACONTROL0r(pc->unit, pc,&data);
         }
         
         /* TX lane 3 */
         if(lane & (0x1 << 3)) {
            READ_WC40_TX3_ANATXACONTROL0r(pc->unit, pc,&data);
         }
         data = ((data>>TX0_ANATXACONTROL0_TXPOL_FLIP_SHIFT) & 0x0001);
    }

    pc->accData = data;
    return SOC_E_NONE;
}

int wcmod_diag(wcmod_st *ws)
{
  int rv = SOC_E_NONE;
  if (ws->verbosity > 0) printf("diag_type=%x port=%0d\n", ws->diag_type, ws->port)  ;
  if(ws->diag_type & WCMOD_DIAG_GENERAL)     rv |= wcmod_diag_general(ws);
  if(ws->diag_type & WCMOD_DIAG_LINK)        rv |= wcmod_diag_link(ws);
  if(ws->diag_type & WCMOD_DIAG_ANEG)        rv |= wcmod_diag_autoneg(ws);
  if(ws->diag_type & WCMOD_DIAG_TFC)         rv |= wcmod_diag_internal_tfc(ws);
  if(ws->diag_type & WCMOD_DIAG_DFE)         rv |= wcmod_diag_dfe(ws);
  if(ws->diag_type & WCMOD_DIAG_IEEE)        rv |= wcmod_diag_ieee(ws);
  if(ws->diag_type & WCMOD_DIAG_TOPOLOGY)    rv |= wcmod_diag_topology(ws);
  if(ws->diag_type & WCMOD_DIAG_EEE)         rv |= wcmod_diag_eee(ws);
  if(ws->diag_type & WCMOD_DIAG_SLICERS)     rv |= wcmod_diag_slicers(ws);
  if(ws->diag_type & WCMOD_DIAG_TX_AMPS)     rv |= wcmod_diag_tx_amps(ws);
  if(ws->diag_type & WCMOD_DIAG_TX_SQUELCH)  rv |= wcmod_diag_tx_squelch(ws);
  if(ws->diag_type & WCMOD_DIAG_TX_TAPS)     rv |= wcmod_diag_tx_taps(ws);
  if(ws->diag_type & WCMOD_DIAG_RX_TAPS)     rv |= wcmod_diag_rx_taps(ws);
  if(ws->diag_type & WCMOD_DIAG_SPEED)       rv |= wcmod_diag_speed(ws);
  if(ws->diag_type & WCMOD_DIAG_TX_LOOPBACK) rv |= wcmod_diag_tx_loopback(ws);
  if(ws->diag_type & WCMOD_DIAG_RX_LOOPBACK) rv |= wcmod_diag_rx_loopback(ws);
  if(ws->diag_type & WCMOD_DIAG_EYE)         rv |= wcmod_diag_eye(ws);
#ifndef __KERNEL__
  if(ws->diag_type & WCMOD_DIAG_RX_PPM)      rv |= wcmod_diag_rx_ppm(ws);
#endif
  if(ws->diag_type & WCMOD_DIAG_RX_SIGDET)   rv |= wcmod_diag_rx_sigdet(ws);
  if(ws->diag_type & WCMOD_DIAG_PRBS)        rv |= wcmod_diag_prbs(ws);
  if(ws->diag_type & WCMOD_DIAG_POLARITY)    rv |= wcmod_tx_rx_polarity_get(ws);
  if(ws->diag_type & WCMOD_DIAG_RX_PCS_BYPASS_LOOPBACK) rv |=  wcmod_diag_rx_pcs_bypass_loopback(ws);
  if(ws->diag_type & WCMOD_DIAG_PRBS_DECOUPLE_TX)        rv |= wcmod_diag_prbs_decouple_tx(ws);
  if(ws->diag_type & WCMOD_DIAG_PRBS_DECOUPLE_RX)        rv |= wcmod_diag_prbs_decouple_rx(ws);
  return rv;
}


#if 0 
/* for debug info */
/**********************************************
 * the following functions are for wcmod info *
 */

STATIC void 
wcmod_info_reg_set_lane_select(wcmod_st * ws, int ind) 
{    
    switch(ind) {
    case 0:  ws->lane_select = WCMOD_LANE_0_0_0_1 ;  ws->this_lane= 0; break ;
    case 1:  ws->lane_select = WCMOD_LANE_0_0_1_0 ;  ws->this_lane= 1;  break ; 
    case 2:  ws->lane_select = WCMOD_LANE_0_1_0_0 ;  ws->this_lane= 2; break ;
    case 3:  ws->lane_select = WCMOD_LANE_1_0_0_0 ;  ws->this_lane= 3; break ;
    }
}


/* need to review */
STATIC void 
wcmod_printfDigital3Up1(unsigned short int data)
{
    char msg[128] = {0};
    strncpy(msg, "\t ") ;
    if(data&0x1)   sal_strncat(msg,"2P5G,"); 
    if(data&0x2)   sal_strncat(msg,"5G_X4,"); 
    if(data&0x4)   sal_strncat(msg,"6G_X4,"); 
    if(data&0x8)   sal_strncat(msg,"10G_HIGIG,"); 
    if(data&0x10)  sal_strncat(msg,"10G_CX4,"); 
    if(data&0x20)  sal_strncat(msg,"12G_X4,"); 
    if(data&0x40)  sal_strncat(msg,"12P5_X4,"); 
    if(data&0x80)  sal_strncat(msg,"13G_X4,"); 
    if(data&0x100) sal_strncat(msg,"15G_X4,"); 
    if(data&0x200) sal_strncat(msg,"16G_X4,"); 
    if(data&0x400) sal_strncat(msg,"20G_X4,"); 
    printf("%s\n", msg); 
}
/* need to review */
STATIC void 
wcmod_printfDigital3Up3(uint16 data)
{
    char msg[128] = {0};
    strncpy(msg, "\t ") ;
    if(data&0x20)  sal_strncat(msg,"40G,"); 
    if(data&0x80)  sal_strncat(msg,"31P5G,"); 
    if(data&0x100) sal_strncat(msg,"25P455G,"); 
    if(data&0x200) sal_strncat(msg,"21G_X4,"); 
    if(data&0x400) sal_strncat(msg,"LAST=1,"); 
    if(data&0x1)   sal_strncat(msg,"Higig2=1,"); 
    if(data&0x2)   sal_strncat(msg,"FEC=1,"); 
    if(data& DIGITAL3_UP3_CL72_MASK)   sal_strncat(msg,"CL72=1,"); 
    printf("%s\n", msg); 
}

STATIC void 
wcmod_printf_an_adv1(uint16 data)
{
    char msg[64] = {0};
    strncpy(msg, "\t ") ;
    if (data & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_100G_CR10_MASK) 
        sal_strncat (msg,"100G_CR10,");
 
    if (data & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_40G_CR4_MASK) 
        sal_strncat (msg,"40G_CR4,");
  
    if (data & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_40G_KR4_MASK) 
        sal_strncat (msg,"40G_KR4,");
  
    if (data & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_10G_KR_MASK) 
        sal_strncat (msg,"10G_KR,");
  
    if (data & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_10G_KX4_MASK) 
        sal_strncat (msg,"10G_KX4,");
  
    if (data & AN_IEEE1BLK_AN_ADVERTISEMENT1_TECH_ABILITY_1G_KX_MASK) 
        sal_strncat (msg,"1G_KX,");
    printf("%s\n", msg);       
}


STATIC int
wcmod_info_an(wcmod_st * ws) 
{
    int i, tmp_lane_select, tmp_lane, rv = SOC_E_NONE;
    uint16 data16;

    tmp_lane_select = ws->lane_select ;
    tmp_lane        = ws->this_lane ; 

    printf("%s:\n", FUNCTION_NAME()) ;
    for(i=0; i<4; i++) {
        wcmod_info_reg_set_lane_select(ws, i) ;

        SOC_IF_ERROR_RETURN
            (READ_WC40_DIGITAL3_UP1r(ws->unit, ws, &data16));
        printf("\tLane=%0d up1(0x8329) data=%04x\n",i, data16);
        wcmod_printfDigital3Up1(data16);
        
        SOC_IF_ERROR_RETURN
            (READ_WC40_DIGITAL3_UP3r(ws->unit, ws, &data16));
        printf("\tLane=%0d up3(0x832b) data=%04x\n", i, data16);
        wcmod_printfDigital3Up3(data16);

        SOC_IF_ERROR_RETURN
            (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT1r(ws->unit, ws, &data16));
        printf("\tLane=%0d AN_IEEE1BLK_AN_ADVERTISEMENT1r (0x38000011) data=%04x\n", i, data16);
        wcmod_printf_an_adv1(data16) ;

        SOC_IF_ERROR_RETURN
            (READ_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(ws->unit, ws, &data16));
        printf("\tLane=%0d CL72_USERB0_CL72_MISC1_CONTROLr (0x82e3) data=%04x link_control_force=%1x force_val=%1x tx_fir_tap_main_kx_init_val=%2x\n", i, data16,
               (data16 & CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK)>>CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_SHIFT,
               (data16 & CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_MASK)>> CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_SHIFT,
               (data16 & CL72_USERB0_CL72_MISC1_CONTROL_TX_FIR_TAP_MAIN_KX_INIT_VAL_MASK)>>CL72_USERB0_CL72_MISC1_CONTROL_TX_FIR_TAP_MAIN_KX_INIT_VAL_SHIFT);

        SOC_IF_ERROR_RETURN
            (READ_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(ws->unit, ws, &data16));
        printf("\tLane=%0d PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(0x08000096) data=%x\n", i, data16) ;

        SOC_IF_ERROR_RETURN
            (READ_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151r(ws->unit, ws, &data16));
        printf("\tLane=%0d PMD_IEEE9BLK_TENGBASE_KR STATUS (0x08000097) data=%x fail=%x training=%x lock=%x recevier=%x\n", i, data16,
               (data16 & PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151_TRAINING_FAILURE_MASK) 
               >> PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151_TRAINING_FAILURE_SHIFT,
               (data16 & PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151_TRAINING_STATUS_MASK)
               >> PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151_TRAINING_STATUS_SHIFT,
               (data16 & PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151_FRAME_LOCK_MASK)
               >> PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151_FRAME_LOCK_SHIFT,
               (data16 & PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151_RECEIVER_STATUS_MASK)
               >> PMD_IEEE9BLK_TENGBASE_KR_PMD_STATUS_REGISTER_151_RECEIVER_STATUS_SHIFT ) ;
        printf("\t----------------\n") ;
    }
    ws->lane_select = tmp_lane_select ;
    ws->this_lane   = tmp_lane ;
    return rv ;
}

STATIC int 
wcmod_info_speed(wcmod_st * ws) 
{
    int i, tmp_lane_select, tmp_lane, rv = SOC_E_NONE;
    uint16 data;

    tmp_lane_select = ws->lane_select ;
    tmp_lane        = ws->this_lane ; 
    printf("(Addr=0x8308) WC40_SERDESDIGITAL_MISC1r:\n") ;
    for(i=0; i<4; i++) {
        wcmod_info_reg_set_lane_select(ws, i) ;
         
        SOC_IF_ERROR_RETURN(READ_WC40_SERDESDIGITAL_MISC1r(ws->unit, ws, &data));
        printf("\tLane%0d data=%x ref_clk=%x force_pll_mode_afe_sel=%1x pll_mode_afe=%2x force_speed=%2x\n", i, data,
               (data & SERDESDIGITAL_MISC1_REFCLK_SEL_MASK) >> SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT, 
               (data & SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK) >> SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_SHIFT,
               (data & SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK ) >> SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT,
               (data & SERDESDIGITAL_MISC1_FORCE_SPEED_MASK)>>SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT );
    }
    
    printf("(Addr=0x81d2) WC40_GP2_REG_GP2_2r:\n") ;
    for(i=0; i<4; i++) {
        wcmod_info_reg_set_lane_select(ws, i) ;
         
        SOC_IF_ERROR_RETURN(READ_WC40_GP2_REG_GP2_2r(ws->unit,ws, &data));
        printf("\tLane%0d data=%x LN1=%2x LN0=%2x\n", i, data,
               (data & GP2_REG_GP2_2_ACTUAL_SPEED_LN1_MASK) >> GP2_REG_GP2_2_ACTUAL_SPEED_LN1_SHIFT,
               (data & GP2_REG_GP2_2_ACTUAL_SPEED_LN0_MASK) >> GP2_REG_GP2_2_ACTUAL_SPEED_LN0_SHIFT ) ;
    }
    printf("(Addr=0x81d3) WC40_GP2_REG_GP2_3r:\n") ;
    for(i=0; i<4; i++) {
        wcmod_info_reg_set_lane_select(ws, i) ;
         
        SOC_IF_ERROR_RETURN(READ_WC40_GP2_REG_GP2_3r(ws->unit,ws, &data));
        printf("\tLane%0d data=%x LN3=%2x LN2=%2x\n", i, data,
               (data & GP2_REG_GP2_3_ACTUAL_SPEED_LN3_MASK) >> GP2_REG_GP2_3_ACTUAL_SPEED_LN3_SHIFT,
               (data & GP2_REG_GP2_3_ACTUAL_SPEED_LN2_MASK) >> GP2_REG_GP2_3_ACTUAL_SPEED_LN2_SHIFT ) ;
    }

    ws->lane_select = tmp_lane_select ;
    ws->this_lane   = tmp_lane ;
    return rv ;
}

int wcmod_info_get(wcmod_st *ws)
{
  int rv = SOC_E_NONE;

  if(ws->info_type & WCMOD_INFO_SPEED)       rv |= wcmod_info_speed(ws);
  if(ws->info_type & WCMOD_INFO_ANEG)        rv |= wcmod_info_an(ws);
  /* to hook up other info debug functions here. comment out for now 
  if(ws->info_type & WCMOD_INFO_ALL)         rv != wcmod_info_all(ws) ;
  if(ws->info_type & WCMOD_INFO_ALL_LANES)   rv != wcmod_info_all_lanes(ws) ; 
  */
  return rv;
}

int wcmod_info_reg_get(wcmod_st *ws, int lane, uint32 addr)
{
    uint16 data ;
    int tmp_lane_select, tmp_lane ;
    tmp_lane_select = ws->lane_select ;
    tmp_lane        = ws->this_lane ; 

    wcmod_info_reg_set_lane_select(ws, lane) ;    
    SOC_IF_ERROR_RETURN(WC40_REG_READ(ws->unit, ws, 0x0, addr, &data)) ;
    printf("Read addr=%x data=%x lane=%0d\n", addr, data, lane) ;

    ws->lane_select = tmp_lane_select ;             
    ws->this_lane   = tmp_lane ;
    return SOC_E_NONE;
}

int wcmod_info_reg_set(wcmod_st *ws, int lane, uint32 addr, uint16 data)
{
    int tmp_lane_select, tmp_lane ;
    tmp_lane_select = ws->lane_select ;
    tmp_lane        = ws->this_lane ;

    wcmod_info_reg_set_lane_select(ws, lane) ;    
    SOC_IF_ERROR_RETURN(WC40_REG_WRITE(ws->unit, ws, 0x0, addr, data)) ;
    printf("Write addr=%x data=%x lane=%0d\n", addr, data, lane) ;

    ws->lane_select = tmp_lane_select ;             
    ws->this_lane   = tmp_lane  ;
    return SOC_E_NONE;
}

#endif
