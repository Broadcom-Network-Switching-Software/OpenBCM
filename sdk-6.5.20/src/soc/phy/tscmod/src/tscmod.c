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
 *  File       : tscmod.c
 *  Description: SDK interface to Broadcom TSC Vertical SerDes Driver
 *              (TSC 40nm and later. Not compatible with Warpcore and earlier)
 *---------------------------------------------------------------------*/


#include <sal/types.h>
#include <sal/core/spl.h>
#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/port_ability.h>
#include <soc/phyctrl.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif

#include "phydefs.h"      /* Must include before other phy related includes */


/* static variables */
#if defined(INCLUDE_XGXS_TSCMOD)
STATIC char tscmod_device_name[] = "TSC";
#endif

int (*_phy_tscmod_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *,int) = {NULL} ;
sal_sem_t  tscmod_core_sema[SOC_MAX_NUM_DEVICES][32] ;

#if defined(INCLUDE_XGXS_TSCMOD)
#include "phyconfig.h"     /* Must include before other phy related includes */
#include "phyreg.h"
#include "phyfege.h"
#include "phynull.h"
#include "serdesid.h"
#include "tscmod.h"
#include "tscmod_extra.h"
#include "tscmod_phyreg.h"
#include "tscmod_defines.h"
#include "tscmod_functions.h"

/* uController's firmware */
extern uint8  tscmod_ucode_TSC_revA0[];
extern int    tscmod_ucode_TSC_revA0_len;
extern uint16 tscmod_ucode_TSC_revA0_crc;
extern uint16 tscmod_ucode_TSC_revA0_ver;
extern uint16 tscmod_ucode_TSC_revA0_ect_addr;  /* empty     credit table */
extern uint16 tscmod_ucode_TSC_revA0_cct_addr;  /* classic   credit table */
extern uint16 tscmod_ucode_TSC_revA0_act_addr;  /* alternate credit table */

STATIC TSCMOD_UCODE_DESC tscmod_ucodes[] = {
   {&tscmod_ucode_TSC_revA0[0], &tscmod_ucode_TSC_revA0_len, TSCMOD_SERDES_ID0_REVID_A0},
   {&tscmod_ucode_TSC_revA0[0], &tscmod_ucode_TSC_revA0_len, TSCMOD_SERDES_ID0_REVID_A1},
   {&tscmod_ucode_TSC_revA0[0], &tscmod_ucode_TSC_revA0_len, TSCMOD_SERDES_ID0_REVID_A2}
};
#define TSCMOD_UCODE_NUM_ENTRIES  (sizeof(tscmod_ucodes)/sizeof(tscmod_ucodes[0]))

/* function forward declaration */
STATIC int _tscmod_phy_parameter_copy(phy_ctrl_t *pc, tscmod_st *wc);
STATIC int _phy_tscmod_notify_duplex(int unit, soc_port_t port, uint32 duplex);
STATIC int _phy_tscmod_notify_speed(int unit, soc_port_t port, uint32 speed);
STATIC int _phy_tscmod_notify_stop(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_tscmod_notify_resume(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_tscmod_notify_interface(int unit, soc_port_t port, uint32 intf);
STATIC int _phy_tscmod_control_tx_driver_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value);
STATIC int _phy_tscmod_per_lane_control_tx_driver_set(int unit, soc_port_t port, int lane,
                                soc_phy_control_t type, uint32 value);
STATIC int _phy_tscmod_control_preemphasis_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value);
STATIC int _phy_tscmod_per_lane_control_preemphasis_set(int unit, soc_port_t port, int lane,
                                                        soc_phy_control_t type, uint32 value, uint16 mode);
STATIC int _tscmod_init_tx_driver_regs(int unit, soc_port_t port) ;
STATIC int _phy_tscmod_an_set(int unit, soc_port_t port, int an);
STATIC int phy_tscmod_lb_set(int unit, soc_port_t port, int enable);
STATIC int _phy_tscmod_ability_advert_set(int unit, soc_port_t port,
                                        soc_port_ability_t *ability);
STATIC int _phy_tscmod_ability_local_get(int unit, soc_port_t port,
                                       soc_port_ability_t *ability);
STATIC int _phy_tscmod_speed_set(int unit, soc_port_t port, int speed);
STATIC int _phy_tscmod_speed_get(int unit, soc_port_t port, int *speed, int *intf, int *asp_mode, int *scr);
STATIC int phy_tscmod_an_get(int unit, soc_port_t port, int *an, int *an_done);
STATIC int _tscmod_soft_reset(int unit, phy_ctrl_t  *pc);
STATIC int phy_tscmod_reg_aer_read(int unit, phy_ctrl_t *pc, uint32 flags,
                  uint32 phy_reg_addr, uint16 *phy_data);
STATIC int phy_tscmod_reg_aer_write(int unit, phy_ctrl_t *pc,  uint32 flags,
                   uint32 phy_reg_addr, uint16 phy_data);
STATIC int phy_tscmod_reg_aer_modify(int unit, phy_ctrl_t *pc,  uint32 flags,
                    uint32 phy_reg_addr, uint16 phy_data,uint16 phy_data_mask);
STATIC int _tscmod_chip_init_done(int unit,int chip_num,int phy_mode, int * init_mode, uint32 *uc_info);
STATIC int _tscmod_chip_an_init_done(int unit,int chip_num);
STATIC int phy_tscmod_firmware_load(int unit, int port, int offset,
                                    uint8 *array,int datalen, uint16 *uc_ver, uint16 *uc_crc);
int _phy_tscmod_control_prbs_polynomial_get(tscmod_st *tsc, uint32 *value);
int _phy_tscmod_control_prbs_tx_invert_data_get(tscmod_st *tsc, uint32 *value);
int _phy_tscmod_control_prbs_rx_invert_data_get(tscmod_st *tsc, uint32 *value);
int _phy_tscmod_control_prbs_check_mode_get(tscmod_st *tsc, uint32 *value);
int _phy_tscmod_control_prbs_enable_get(tscmod_st *tsc, uint32 *value);

STATIC int phy_tscmod_per_lane_control_prbs_polynomial_set(int unit, soc_port_t port, int lane, uint32 value) ;
STATIC int phy_tscmod_per_lane_control_prbs_tx_invert_data_set(int unit, soc_port_t port, int lane, uint32 value) ;
STATIC int phy_tscmod_per_lane_control_prbs_rx_invert_data_set(int unit, soc_port_t port, int lane, uint32 value) ;

STATIC int phy_tscmod_lb_get(int unit, soc_port_t port, int *enable);
STATIC int phy_tscmod_diag_ctrl(int, soc_port_t, uint32, int, int, void *);
STATIC int tscmod_uc_status_dump(int unit, soc_port_t port);

STATIC int _phy_tscmod_rx_polarity_set(int unit, phy_ctrl_t *pc, uint32 enable);
STATIC int _phy_tscmod_tx_polarity_set(int unit, phy_ctrl_t *pc, uint32 enable);

extern int _tscmod_getRevDetails(tscmod_st* pc);
extern int _tscmod_lane_enable(tscmod_st* pc);

STATIC int _phy_tscmod_cfg_dump(int unit, soc_port_t port) ;

STATIC char *tscmod_ability_msg0(int data) ;
STATIC char *tscmod_cl73_ability_msg0(int data) ;
STATIC char *tscmod_cl37bam_ability_msg0(int data) ;
STATIC char *tscmod_cl37bam_ability_msg1(int data) ;

extern int tscmod_diag_g_rd_ram(tscmod_st* ws, int mode, int lane);
extern int tscmod_diag_g_an(tscmod_st *ws, int lane) ;

STATIC int _phy_tscmod_control_set(int unit, soc_port_t port,
                       soc_phy_control_t type, uint32 value) ;

STATIC int _phy_tscmod_control_get(int unit, soc_port_t port,
                                  soc_phy_control_t type, uint32 *value) ;

STATIC int tscmod_get_lane_swap(int core, int sub_core, int tx, int svk_id) ;
STATIC int  _phy_tscmod_port_lkdn_pmd_lock_handler(int unit, soc_port_t port, int dsc_restart_mode, int debug_id) ;
STATIC int  _phy_tscmod_port_lkup_pmd_lock_handler(int unit, soc_port_t port, int *link) ;
STATIC int  _phy_tscmod_port_lkuc_rxp_handler(int unit, soc_port_t port, int *link) ;

STATIC int tscmod_an_cl72_control_set(int unit, phy_ctrl_t *pc) ;
STATIC void tscmod_uc_cap_set(tscmod_st *tsc, TSCMOD_DEV_CFG_t  *pCfg, int cap, int ver) ;
STATIC int tscmod_uc_cmd_seq(int unit, soc_port_t port, int lane, int cmd) ;

STATIC void tscmod_sema_lock(int unit, soc_port_t port, const char *str) ;
STATIC void tscmod_sema_unlock(int unit, soc_port_t port) ;
STATIC void tscmod_sema_create(int unit, soc_port_t port) ;
STATIC int _phy_tscmod_control_vco_freq_get(int unit, phy_ctrl_t *pc, uint32 *value);
STATIC int  tscmod_tr_if_mode(int soc_port) ;

extern int tscmod_uc_sync_mask_wr_cmd(tscmod_st* pc) ;

STATIC int tscmod_wb_sc_handler(int unit, soc_port_t port, TSCMOD_WB_HANDLER_TYPE_t type, TSCMOD_WB_DATA_ID_t id) ;

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int tscmod_wb_id_command(int unit, soc_port_t port, int offset, 
                                TSCMOD_WB_HANDLER_TYPE_t type, TSCMOD_WB_DATA_ID_t id, int *size) ;
STATIC int tscmod_wb_sc_size_count(int unit, soc_port_t port, int *size) ;
STATIC int tscmod_wb_sc_init(int unit, soc_port_t port) ;
STATIC int tscmod_wb_sc_realloc_init(int unit, soc_port_t port) ;
STATIC int tscmod_wb_sc_sync(int unit, soc_port_t port) ;
STATIC int tscmod_wb_sc_reinit(int unit, soc_port_t port) ;
#endif

STATIC int
_tscmod_phy_parameter_copy(phy_ctrl_t  *pc, tscmod_st *tsc)
{
	if ((pc == NULL) || (tsc == NULL))
	{
		return SOC_E_EMPTY;
	}
	else
	{
		tsc->unit       = pc->unit;
		tsc->port       = pc->port;
		tsc->phy_ad     = pc->phy_id;

      tsc->regacc_type  = TSCMOD_MDIO_CL22;

      /* tsc->platform_info = pc;  */
      /* tsc->read = pc->read;  */
      /* tsc->write = pc->write;  */
      tsc->this_lane   = 0 ;
      tsc->lane_select = TSCMOD_LANE_0_0_0_1 ;

      return SOC_E_NONE;
	}
}


STATIC int
_phy_tscmod_ucode_get(int unit, soc_port_t port, uint8 **ppdata, int *len,
                    int *mem_alloced)
{
    int ix;
    phy_ctrl_t        *pc;
    TSCMOD_DEV_DESC_t *pDesc;
    tscmod_st         *tsc;

    *mem_alloced = FALSE;
    *ppdata = NULL;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   = (tscmod_st *)( pDesc + 1);

    /* check which firmware to load */
    for (ix = 0; ix < TSCMOD_UCODE_NUM_ENTRIES; ix++) {
        if (tscmod_ucodes[ix].chip_rev == TSCMOD_REVID(pc)) {
            break;
        }
    }
    if (ix >= TSCMOD_UCODE_NUM_ENTRIES) {
       if(tsc->verbosity&TSCMOD_DBG_PRINT) {
          LOG_WARN(BSL_LS_SOC_PHY,
                   (BSL_META_U(unit,
                               "no firmware matches the chip rev number(%x)!!! use default\n"), TSCMOD_REVID(pc)));
       }
       ix = TSCMOD_UCODE_NUM_ENTRIES - 1;
       tsc->ctrl_type |= TSCMOD_CTRL_TYPE_MSG ;
       tsc->msg_code  |= TSCMOD_MSG_FW_CHIP_VER_UNKNOWN ;
    }
    for (; ix >= 0; ix--) {
        if ((tscmod_ucodes[ix].pdata != NULL) &&
            (*(tscmod_ucodes[ix].plen) != 0)) {
            break;
        }
    }
    if (ix < 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "no valid firmware found!!!\n")));
        return SOC_E_NOT_FOUND;
    }

    *ppdata = tscmod_ucodes[ix].pdata;
    *len = *(tscmod_ucodes[ix].plen);
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_tscmod_firmware_set
 * Purpose:
 *      write the given firmware to the uController's RAM
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      offset - offset to the data stream
 *      array  - the given data
 *      datalen- the data length
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_tscmod_firmware_set(int unit, int port, int offset, uint8 *array,int datalen)
{
   return SOC_E_FAIL;
}

STATIC int
phy_tscmod_firmware_load(int unit, int port, int offset, uint8 *array,int datalen,
                         uint16 *uc_ver, uint16 *uc_crc)
{
   phy_ctrl_t        *pc;
   TSCMOD_DEV_DESC_t *pDesc;
   tscmod_st         *tsc;
   int no_cksum, rv, len, data32, i, count, tmp_verb ;
   uint16 ver = 0 ;
   uint16 cksum = 0;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   = (tscmod_st *)( pDesc + 1);

   rv = SOC_E_NONE;  tmp_verb = 0 ;

   no_cksum = !(DEV_CFG_PTR(pc)->uc_cksum);

   if (tsc->verbosity & TSCMOD_DBG_INIT )
          printf ("%s: u=%0d p=%0d loading firmware datalen=%0d\n",
                 FUNCTION_NAME(), unit, port, datalen);

   tsc->per_lane_control = TSCMOD_UC_INIT ;
   tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);
   if(rv != SOC_E_NONE ) {
      printf ("Error: u=%0d p=%0d loading firmware init failed\n", unit, port);
      return rv ;
   }

   tsc->per_lane_control = TSCMOD_UC_OFFSET | (offset << TSCMOD_UC_MACRO_SHIFT) ;
   tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);

   tmp_verb = tsc->verbosity ;
   if ((_phy_tscmod_firmware_set_helper[unit] != NULL) && (DEV_CFG_PTR(pc)->load_mthd == 2)) {
      /* transfer size, 16bytes quantity*/
      if (datalen % 16) {
         len = (((datalen / 16) +1 ) * 16) -1;
      } else {
         len = datalen - 1;
      }

      if(tsc->verbosity & TSCMOD_DBG_INIT)
         printf ("%s: u=%0d p=%0d loading firmware 16B mode\n",
                 FUNCTION_NAME(), unit, port);

      tsc->per_lane_control = TSCMOD_UC_SIZE | (len << TSCMOD_UC_MACRO_SHIFT) ;
      tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);

      tsc->per_lane_control = TSCMOD_UC_EXT_ON ;
      tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);

      TSCMOD_USLEEP(1000);

      rv = _phy_tscmod_firmware_set_helper[unit](unit,port,array,datalen);

      tsc->per_lane_control = TSCMOD_UC_EXT_OFF ;
      tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);

      TSCMOD_USLEEP(1000);
   } else {
      /* transfer size */
      tsc->per_lane_control = TSCMOD_UC_SIZE | ((datalen-1) << TSCMOD_UC_MACRO_SHIFT) ;
      tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);

      /* start write operation */
      tsc->per_lane_control = TSCMOD_UC_W_OP ;
      tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);

      if((tsc->verbosity & TSCMOD_DBG_FW_LOAD)==0) tsc->verbosity = 0 ;

      count = 0 ;
      /* write 16-bit word to data register */
      for (i = 0; i < datalen / 2; i++) {
         data32 = array[2 * i] | (array[2 * i + 1] << 8);
         tsc->per_lane_control = TSCMOD_UC_W_DATA | (data32<< TSCMOD_UC_MACRO_SHIFT) ;
         tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);
         count ++ ;
         if( ((count & 0x3ff) == 0)&&(tsc->verbosity & TSCMOD_DBG_INIT))
            printf ("%s: u=%0d p=%0d loading firmware count=%0d\n", FUNCTION_NAME(), unit, port, count);
      }

      /* check if the firmware size is odd number */
      if (datalen % 2) {
         data32 = array[datalen - 1];
         tsc->per_lane_control = TSCMOD_UC_W_DATA | (data32<< TSCMOD_UC_MACRO_SHIFT) ;
         tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);
      }

      tsc->verbosity = tmp_verb ;
      /* complete the writing */
      tsc->per_lane_control = TSCMOD_UC_STOP ;
      tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);
      if (tsc->verbosity & TSCMOD_DBG_INIT )
         printf ("%s: u=%0d p=%0d loading firmware load stopped\n", FUNCTION_NAME(), unit, port);
   }

   tsc->per_lane_control = TSCMOD_UC_LOAD_STATUS ;
   tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);

   if(tsc->accData&(UC_DOWNLOAD_STATUS_ERR1_MASK|UC_DOWNLOAD_STATUS_ERR0_MASK)){
      printf ("TSCMOD : uC RAM download fails: u=%d p=%d status=%x\n",
             unit, port, tsc->accData) ;
      return (SOC_E_FAIL);
   }
   if(no_cksum) {
      tsc->per_lane_control = TSCMOD_UC_NO_CKSUM ;
      tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);
   }

   /* release uC's reset */
   tsc->per_lane_control =  TSCMOD_UC_RELEASE ;
   tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);

   /* wait for checksum to be written to this register by uC */
   if (!no_cksum) {
      tsc->per_lane_control =  TSCMOD_UC_R_CKSUM ;
      tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);
      cksum = tsc->accData ;
      *uc_crc = cksum ;
      if(cksum != tscmod_ucode_TSC_revA0_crc) {
         printf ("Warning: u=%0d p=%0d uC crc mismatch %x vs expected %x\n",
                unit, port, cksum, tscmod_ucode_TSC_revA0_crc);
      }
   }
   tsc->per_lane_control =  TSCMOD_UC_R_VER ;
   tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);
   ver  = tsc->accData ;
   *uc_ver = ver ;
   if(ver !=tscmod_ucode_TSC_revA0_ver) {
      if(tsc->verbosity & (TSCMOD_DBG_UC))
         printf ("u=%0d p=%0d uC ver mismatch %x vs expected %x\n",
                unit, port, ver, tscmod_ucode_TSC_revA0_ver);
      *uc_ver = tscmod_ucode_TSC_revA0_ver;
   }

   if(tsc->verbosity & (TSCMOD_DBG_UC|TSCMOD_DBG_CFG|TSCMOD_DBG_INIT) ) {
      printf ("TSCMOD : uC RAM download success: u=%d p=%d ver=%x", unit, port,ver);
      if(!no_cksum) {
         printf (" cksum=0x%x\n", cksum) ;
      } else {
         printf (" no_cksum\n") ;
      }
   }

   return SOC_E_NONE ;
}


STATIC int
_init_tscmod_st(int unit, soc_port_t port, tscmod_st *tsc)
{
   int i ;
      /* printf ("%s: Initializing tscmod_st object\n", FUNCTION_NAME()); */
      tsc->id                 = 0x33333333;
      tsc->unit               = 0;
      tsc->port               = 0;
      tsc->phy_ad             = 0;
      tsc->prt_ad             = 0;
      tsc->this_lane          = 0;
      tsc->lane_select        = TSCMOD_LANE_ILLEGAL;
      tsc->lane_num_ignore    = 0;
      tsc->per_lane_control   = 0;
      /* tsc->en_dis             = 0;  */
      /* tsc->aer_ofs_strap      = 0;  */
      /* tsc->multiport_addr_en  = 0;  */
      /* tsc->aer_bcst_ofs_strap = 0x1ff; */ /* good for TR3 and FE1600 */
      /*
    #if defined (_MDK_TSCMOD_)
      tsc->aer_bcst_ofs_strap = 0x1ff;  only guaranteed for Radian.
    #endif
      */
      tsc->spd_intf           = TSCMOD_SPD_ILLEGAL;
      tsc->regacc_type        = TSCMOD_MDIO_CL22;
      tsc->port_type          = TSCMOD_PORT_MODE_ILLEGAL;
      tsc->os_type            = TSCMOD_OS_ILLEGAL;
      tsc->dxgxs              = 0;
      tsc->tsc_touched        = 0;
      tsc->reg_sync           = 0;
      tsc->blk_adr            = 0;
      tsc->lane_swap          = 0;  /* default: even; to load even or odd core info */
      tsc->duplex             = 0;
      tsc->accAddr            = 0;
      tsc->accData            = 0;
      tsc->diag_type          = TSCMOD_DIAG_ILLEGAL;
      tsc->model_type         = TSCMOD_MODEL_TYPE_ILLEGAL;
      tsc->an_type            = TSCMOD_AN_NONE ;
      tsc->an_rf              = 0;
      tsc->an_pause           = 0;
      tsc->an_tech_ability    = 0;
      tsc->an_fec             = 0;
      tsc->verbosity          = 0;
      tsc->ctrl_type          = TSCMOD_CTRL_TYPE_DEFAULT ;
      tsc->err_code           = 0;
      tsc->msg_code           = 0;
      tsc->firmware_mode      = -1;
      tsc->iintf               = 0 ;
      tsc->os_type            = TSCMOD_OS1 ;

      for(i=0;i<4; i++)
         tsc->linkup_bert_cnt[i] = 0 ;
      /* tsc->asymmetric_mode    = 0;  */
      /*  tsc->s100g_plus         = 0;  */

      if (tsc->verbosity & TSCMOD_DBG_INIT )
         printf ("%s: u=%0d p=%0d Done Initializing tscmod_st object %p\n",
                 FUNCTION_NAME(), unit, port, (void *)tsc);
      return SOC_E_NONE;
}

int
soc_phy_tsc_set_verbose(int unit, int port, int verbose)
{
    phy_ctrl_t      *pc;
    TSCMOD_DEV_DESC_t *pDesc;
    tscmod_st    *tsc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

    tsc->verbosity = verbose;

    printf ("%s: u=%0d p=%0d verbosity=%x\n",
                 FUNCTION_NAME(), unit, port, tsc->verbosity);

    return SOC_E_NONE;

}

STATIC int
_phy_tscmod_config_init(int unit, soc_port_t port)
{
   phy_ctrl_t      *pc;
   TSCMOD_DEV_CFG_t *pCfg;
   TSCMOD_DEV_INFO_t  *pInfo;
   TSCMOD_DEV_DESC_t *pDesc;
   tscmod_st    *tsc;
   soc_phy_info_t *pi;
   int len;
   uint16 serdes_id0;
   int i, rv;
   TSCMOD_TX_DRIVE_t *p_tx;
   soc_info_t *si;
   int phy_port;  /* physical port number */
   int tx_lane_swap, rx_lane_swap, num_core, loop,
      preemphasis, pdriver, idriver, post2_driver ;
   int phy_passthru ;
   int phy_serdes  ;
   int line_intf ;
   int ctrl_flag ;

   pc = INT_PHY_SW_STATE(unit, port);

   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

   /* clear the memory */
   TSCMOD_MEM_SET(pDesc, 0, sizeof(*pDesc));
   TSCMOD_MEM_SET(tsc, 0, sizeof(*tsc));

   phy_passthru = 0 ;
   phy_serdes   = 0 ;
   line_intf    = -1 ;
   ctrl_flag    = 1 ;

   /* initialize the tsc*/
   SOC_IF_ERROR_RETURN
      (_init_tscmod_st(unit, port, tsc));

    pCfg = &pDesc->cfg;
    pInfo = &pDesc->info;

    /*added for the tsc structure init. */
    SOC_IF_ERROR_RETURN
    	(_tscmod_phy_parameter_copy(pc, tsc));

    pCfg->diag_regs = 1 ;
    pCfg->diag_verb = 0 ;
    pCfg->linkup_bert= 0 ;
    pCfg->fault_disable = 0 ;
    pCfg->svk_id     = 0 ;
    pCfg->diag_port = 0 ;
    pCfg->los_usec  = 75000 ;
    pCfg->an_ctl    = 0 ;
    tsc->an_ctl     = 0 ;
    pCfg->an_link_fail_nocl72_wait_timer=0 ;
    pCfg->pll_vco_step = 0 ;
    pCfg->ability_mask = 0 ;
    pCfg->pll_divider  = 0 ;
    pCfg->ctrl_flag    = 0 ;
    pCfg->macd         = NULL ;
    pCfg->am_reserved  = 0 ;



    /* make sure this_lane and lane_select is inited */
    tscmod_tier1_selector("REVID_READ", tsc, &rv);

    pInfo->serdes_id0 = tsc->accData ;

    if((tsc->verbosity &TSCMOD_DBG_INIT))
       printf ("%s u=%0d(%0d) p=%0d(%0d) ptr=%p serdesID=%x sel=%x"
          "ln=%0d dxgxs=%0d\n", FUNCTION_NAME(), unit, tsc->unit, port, tsc->port,
          (void *)tsc,tsc->accData,tsc->lane_select,tsc->this_lane,tsc->dxgxs);

    /* init the default configuration values */

    for (i = 0; i < NUM_LANES; i++) {
        pCfg->preemph[i] = TSCMOD_NO_CFG_VALUE;
        pCfg->idriver[i] = TSCMOD_NO_CFG_VALUE;
        pCfg->pdriver[i] = TSCMOD_NO_CFG_VALUE;
        pCfg->post2driver[i] = TSCMOD_NO_CFG_VALUE;
    }

   /* default */
    p_tx = &pCfg->tx_drive[TXDRV_DFT_INX];
    p_tx->u.tap.post  = 0x00;
    p_tx->u.tap.main  = 0x00;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x00;

    for(i=0; i<TXDRV_LAST_INX; i++) {
       p_tx = &pCfg->tx_drive[i] ;
       p_tx->u_kx.tap.post  = 0x00;
       p_tx->u_kx.tap.main  = 0x3f;
       p_tx->u_kx.tap.pre   = 0x00;
       p_tx->u_kx.tap.force = 0x00;

       p_tx->u_os.tap.post  = 0x00;
       p_tx->u_os.tap.main  = 0x3f;
       p_tx->u_os.tap.pre   = 0x00;
       p_tx->u_os.tap.force = 0x00;

       p_tx->u_br.tap.post  = 0x16;
       p_tx->u_br.tap.main  = 0x25;
       p_tx->u_br.tap.pre   = 0x04;
       p_tx->u_br.tap.force = 0x00;

       p_tx->u_kr.tap.post  = 0x16;
       p_tx->u_kr.tap.main  = 0x25;
       p_tx->u_kr.tap.pre   = 0x04;
       p_tx->u_kr.tap.force = 0x00;

       p_tx->u_2p5.tap.post  = 0x00;
       p_tx->u_2p5.tap.main  = 0x3f;
       p_tx->u_2p5.tap.pre   = 0x00;
       p_tx->u_2p5.tap.force = 0x00;
    }

    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x09;
    p_tx->ipredrive = 0x09;

    pCfg->hg_mode    = FALSE;
    pCfg->sgmii_mstr = FALSE;
    pCfg->pdetect10g = FALSE;   /* 10G KX4 */
    pCfg->cx42hg     = FALSE;   /* CX4_to_HG */
    pCfg->medium     = SOC_PORT_MEDIUM_FIBER;
    pCfg->cx4_10g    = TRUE;
    pCfg->lane0_rst  = TRUE;
    pCfg->rxaui      = FALSE;
    pCfg->scrambler_en = FALSE ;
    pCfg->load_mthd  = 2;
    pCfg->load_ucode_done = 0 ;
    pCfg->uc_cksum   = 0x100 ;
    pCfg->uc_ver     = 0;
    pCfg->uc_crc     = 0;
    pCfg->firmware_mode = -1 ;
    pCfg->refclk     = 156;

    pCfg->init_mode  = 0 ;
    pCfg->init_speed = 1000000 ;

    pCfg->rxlos_link_wait_timer   = 0;
    pCfg->rxlos_restart_timer     = 0;
    pCfg->rxlos_first_linkup_time = 0;
    pCfg->los_usec
        = soc_property_port_get(unit, port, spn_SERDES_TX_LOS_USEC, pCfg->los_usec);

    /* this property is for debug and diagnostic purpose.
     * byte0:
     * 0: not loading TSC firmware
     * 1: load from MDIO. default method.
     * 2: load from parallel bus if applicable. Provide fast downloading time
     *
     * byte1:
     * 0: inform uC not to perform checksum calculation(default). Save ~70ms for TSC init time
     * 1: inform uC to perform checksum calculation.
     */
    pCfg->load_mthd = soc_property_port_get(unit, port, spn_LOAD_FIRMWARE, pCfg->load_mthd);
    pCfg->load_mthd &= 0xff;
    pCfg->uc_cksum = soc_property_port_get(unit, port, spn_LOAD_FIRMWARE, pCfg->uc_cksum);
    pCfg->uc_cksum = (pCfg->uc_cksum >> 8) & 0xff;
    pCfg->firmware_mode = soc_property_port_get(unit, port,
                             spn_SERDES_FIRMWARE_MODE, pCfg->firmware_mode);

    DEV_CFG_PTR(pc)->sw_rx_los.enable      = FALSE ;
    DEV_CFG_PTR(pc)->sw_rx_los.sys_link    = 0;
    DEV_CFG_PTR(pc)->sw_rx_los.state       = RXLOS_RESET;
    DEV_CFG_PTR(pc)->sw_rx_los.link_status = 0;
    DEV_CFG_PTR(pc)->sw_rx_los.count       = 0;
    DEV_CFG_PTR(pc)->sw_rx_los.rf_dis      = 0;
  
    si = &SOC_INFO(unit);
    if (soc_feature(unit, soc_feature_logical_port_num)) {
       phy_port = si->port_l2p_mapping[port];
    } else {
       phy_port = port;
    }

    pc->lane_num = SOC_DRIVER(unit)->port_info[phy_port].bindex;
    pc->chip_num = SOC_BLOCK_NUMBER(unit,
                   SOC_DRIVER(unit)->port_info[phy_port].blk);

    /* convert to phy_ctrl macros */
    if (si->port_num_lanes[port] == 4) {
       pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
    } else if (si->port_num_lanes[port] == 2) {
       pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
       pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
    } else if (si->port_num_lanes[port] == 1) {
       pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
       pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
    }

    if(SOC_IS_HURRICANE2(unit)) {
       TSCMOD_MODEL_PROJ_SET(tsc,TSCMOD_MODEL_PROJ_HR2) ;
    } else {
       TSCMOD_MODEL_PROJ_SET(tsc,TSCMOD_MODEL_PROJ_TD2) ;
    }

    /* warpcore intstance number and lane number */
    if (SOC_IS_HURRICANE2(unit)) {
       pc->lane_num = (phy_port - 2) % 4;
       pc->chip_num = (phy_port - 2) / 4;
    } else {
       pc->lane_num = (phy_port - 1) % 4;
       pc->chip_num = (phy_port - 1) / 4;
    }

    for (i = 0; i < (MAX_NUM_LANES/4) ; i++) {
       pCfg->rxlane_map[i] = tscmod_get_lane_swap(pc->chip_num, i, 0, pCfg->svk_id) ;
       pCfg->txlane_map[i] = tscmod_get_lane_swap(pc->chip_num, i, 1, pCfg->svk_id) ;
    }

    /* to figure out the core is even or odd core */
    if( pc->chip_num % 2 )
       tsc->lane_swap = 1 ; /* odd */
    else
       tsc->lane_swap = 0 ; /* even */

    /* retrieve chip level information for tsc */
    pi       = &SOC_PHY_INFO(unit, port);

    serdes_id0 = pInfo->serdes_id0;
    if ((serdes_id0 & MAIN0_SERDESID_MODEL_NUMBER_MASK) == TSCMOD_MODEL_TSC ) {
        sal_strlcpy(pInfo->name, "TSC-", sizeof(pInfo->name));
        len = sal_strlen(pInfo->name);
        /* add rev letter */
        pInfo->name[len++] = 'A' + ((serdes_id0 >>
                              MAIN0_SERDESID_REV_LETTER_SHIFT) & 0x3);
        /* add rev number */
        pInfo->name[len++] = '0' + ((serdes_id0 >>
                              MAIN0_SERDESID_REV_NUMBER_SHIFT) & 0x7);
        pInfo->name[len++] = '/';
        pInfo->name[len++] = (pc->chip_num / 10) % 10 + '0';
        pInfo->name[len++] = pc->chip_num % 10 + '0';
        pInfo->name[len++] = '/';

        /* phy_mode: 0 single port mode, port uses all four lanes of Warpcore
         *           1 dual port mode, port uses 2 lanes
         *           2 quad port mode, port uses 1 lane
         */
        if (IS_DUAL_LANE_PORT(pc)) { /* dual-xgxs mode */
            if (pc->lane_num < 2) { /* the first dual-xgxs port */
                pInfo->name[len++] = '0';
                pInfo->name[len++] = '-';
                pInfo->name[len++] = '1';
            } else {
                pInfo->name[len++] = '2';
                pInfo->name[len++] = '-';
                pInfo->name[len++] = '3';
            }
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
            /* set PHY_FLAGS_INDEPENDENT_LANE for dual port */
        } else if (pc->phy_mode == PHYCTRL_ONE_LANE_PORT || CUSTOM_MODE(pc)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
            pInfo->name[len++] = pc->lane_num + '0';
        } else {
            pInfo->name[len++] = '4';
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        }
        pInfo->name[len] = 0;  /* string terminator */

        if (len > TSCMOD_LANE_NAME_LEN) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "TSC info string length %d exceeds max length 0x%x: u=%d p=%d\n"),
                       len,TSCMOD_LANE_NAME_LEN,unit, port));
            return SOC_E_MEMORY;
        }
    } else {
       printf ("Error: u=%0d p=%0d shouldn't invoke %s id=%0x\n", unit, port, FUNCTION_NAME(), serdes_id0) ;
       return SOC_E_UNAVAIL ;
    }

    if (!PHY_EXTERNAL_MODE(unit, port)) {
        pi->phy_name = pInfo->name;
    }

    if (CUSTOM_MODE(pc)) {
        pCfg->custom = pc->phy_mode;
    }
    if (CUSTOM1_MODE(pc)) {
        pCfg->custom1 = pc->phy_mode;
    }

    if (PHY_EXTERNAL_MODE(unit, port)) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_PASSTHRU);
        phy_passthru = soc_property_port_get(unit, port,
                            spn_PHY_PCS_REPEATER, phy_passthru);
        if (phy_passthru) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_PASSTHRU);
        }
        phy_serdes = soc_property_port_get(unit, port,
                            spn_PHY_SERDES, phy_serdes);
    }

    pCfg->hg_mode = IS_HG_PORT(unit,port);
    if (PHY_EXTERNAL_MODE(unit, port)&&(phy_serdes !=1)) {
       /* always IEEE speed mode if connected with an external PHY */
       pCfg->hg_mode = FALSE;

       /* exception: combo mode && passthru */
       if ((!PHY_INDEPENDENT_LANE_MODE(unit, port)) && phy_passthru) {
          pCfg->hg_mode = IS_HG_PORT(unit,port);
       }
    }

    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        (PHY_PASSTHRU_MODE(unit, port)) ||
        (!PHY_INDEPENDENT_LANE_MODE(unit, port)) ||
         (pc->speed_max >= 10000)) {
        pCfg->fiber_pref = TRUE;
    } else {
        pCfg->fiber_pref = FALSE;
    }

    if(pCfg->hg_mode)
       pCfg->cl73an           = FALSE ;
    else
       pCfg->cl73an           = TSCMOD_CL73_WO_BAM ;

    pCfg->cl37an              = FALSE ;

    pCfg->an_cl72          = TRUE  ;
    pCfg->forced_init_cl72 = -1    ;
    pCfg->sw_init_drive    = FALSE ;

    pCfg->pcs_mode         = -1 ;
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port) ) {
       /* PHY_SGMII_AUTONEG_MODE(unit, port) mutex with 1G PDETECT */
        pCfg->pdetect1000x = TRUE;
    } else {
        pCfg->pdetect1000x = FALSE;
    }

    /* JIRA CRTSC-719: CL72 multiple-restart
    pCfg->pdetect1000x = FALSE ;  */

    if (1) {
        /* PCS by pass mode */
        p_tx = &pCfg->tx_drive[TXDRV_6250_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x3f;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;

        /* put mode specific values or use the default which has been set.
        p_tx->u_kx.tap.post  = 0x00;
        p_tx->u_kx.tap.main  = 0x3f;
        p_tx->u_kx.tap.pre   = 0x00;
        p_tx->u_kx.tap.force = 0x00;

        p_tx->u_os.tap.post  = 0x00;
        p_tx->u_os.tap.main  = 0x3f;
        p_tx->u_os.tap.pre   = 0x00;
        p_tx->u_os.tap.force = 0x00;

        p_tx->u_br.tap.post  = 0x16;
        p_tx->u_br.tap.main  = 0x25;
        p_tx->u_br.tap.pre   = 0x04;
        p_tx->u_br.tap.force = 0x00;

        p_tx->u_kr.tap.post  = 0x16;
        p_tx->u_kr.tap.main  = 0x25;
        p_tx->u_kr.tap.pre   = 0x04;
        p_tx->u_kr.tap.force = 0x00;

        p_tx->u_2p5.tap.post  = 0x00;
        p_tx->u_2p5.tap.main  = 0x3f;
        p_tx->u_2p5.tap.pre   = 0x00;
        p_tx->u_2p5.tap.force = 0x00;
        */

        /* PCS by pass mode */
        p_tx = &pCfg->tx_drive[TXDRV_103125_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x3f;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;


        /* PCS by pass mode */
        p_tx = &pCfg->tx_drive[TXDRV_109375_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x3f;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;

        /* PCS by pass mode */
        p_tx = &pCfg->tx_drive[TXDRV_12500_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x3f;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;

        /* PCS by pass mode */
        p_tx = &pCfg->tx_drive[TXDRV_11500_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x3f;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;


        /* Irvin lab.  borrow from 40G XLAUI */
        p_tx = &pCfg->tx_drive[TXDRV_XFI_INX];
        p_tx->u.tap.post  = 0x08;
        p_tx->u.tap.main  = 0x33;
        p_tx->u.tap.pre   = 0x04;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x04;
        p_tx->ipredrive = 0x04;

        /* XFI  */
        p_tx = &pCfg->tx_drive[TXDRV_XFI_INX];
        p_tx->u.tap.post  = 0x08;
        p_tx->u.tap.main  = 0x37;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x03;


        /* SFI SR */
        p_tx = &pCfg->tx_drive[TXDRV_SFI_INX];
        p_tx->u.tap.post  = 0x12;
        p_tx->u.tap.main  = 0x2d;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;

        /* SFI DAC */
        p_tx = &pCfg->tx_drive[TXDRV_SFIDAC_INX];
        p_tx->u.tap.post  = 0x12;
        p_tx->u.tap.main  = 0x2d;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x02;
        p_tx->ipredrive = 0x02;

        /* SR4 */
        p_tx = &pCfg->tx_drive[TXDRV_SR4_INX];
        p_tx->u.tap.post  = 0x13;
        p_tx->u.tap.main  = 0x2c;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x05;
        p_tx->ipredrive = 0x05;

        /* XLAUI */
        p_tx = &pCfg->tx_drive[TXDRV_XLAUI_INX];
        p_tx->u.tap.post  = 0x18;
        p_tx->u.tap.main  = 0x27;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x04;
        p_tx->ipredrive = 0x04;

        /* TXDRV_6GOS1_INX */
        p_tx = &pCfg->tx_drive[TXDRV_6GOS1_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;

        /* TXDRV_6GOS2_INX */
        p_tx = &pCfg->tx_drive[TXDRV_6GOS2_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x3f;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;

        /* TXDRV_6GOS2_CX4_INX */
        p_tx = &pCfg->tx_drive[TXDRV_6GOS2_CX4_INX];
        p_tx->u.tap.post  = 0x0e;
        p_tx->u.tap.main  = 0x31;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x01;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x08;
        p_tx->ipredrive = 0x08;

        /* Autoneg */
        p_tx = &pCfg->tx_drive[TXDRV_AN_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x00;
        p_tx->post2     = 0x02;
        p_tx->idrive    = 0x06;
        p_tx->ipredrive = 0x09;

        /* default */
        p_tx = &pCfg->tx_drive[TXDRV_DFT_INX];
        p_tx->u.tap.post  = 0x00;
        p_tx->u.tap.main  = 0x00;
        p_tx->u.tap.pre   = 0x00;
        p_tx->u.tap.force = 0x00;
        p_tx->post2     = 0x00;
        p_tx->idrive    = 0x09;
        p_tx->ipredrive = 0x09;
    }

    /* config property overrides */
    loop = NUM_LANES ;

    for (i = 0; i < loop; i++) {
        preemphasis = soc_property_port_suffix_num_get(unit, port, i,
                                    spn_SERDES_PREEMPHASIS,
                                    "lane", pCfg->preemph[i]);
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Port %d   - lane  %d -> preemphasis 0x%x\n"),
                  port, i, preemphasis));
        pCfg->preemph[i] = preemphasis;
    }

    for (i = 0; i < loop; i++) {
        idriver = soc_property_port_suffix_num_get(unit, port, i,
                                    spn_SERDES_DRIVER_CURRENT,
                                    "lane", pCfg->idriver[i]);
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Port %d   - lane  %d -> idriver 0x%x\n"),
                  port, i, idriver));
        pCfg->idriver[i] = idriver;
    }

    for (i = 0; i < loop; i++) {
        pdriver = soc_property_port_suffix_num_get(unit, port, i,
                                    spn_SERDES_PRE_DRIVER_CURRENT,
                                    "lane", pCfg->pdriver[i]);
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Port %d   - lane  %d -> pdriver 0x%x\n"),
                  port, i, pdriver));
        pCfg->pdriver[i] = pdriver;
    }

    for (i = 0; i < loop; i++) {
        post2_driver = soc_property_port_suffix_num_get(unit, port, i,
                                    spn_SERDES_POST2_DRIVER_CURRENT,
                                    "lane", pCfg->post2driver[i]);
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Port %d   - lane  %d -> post2_driver 0x%x\n"),
                  port, i, post2_driver));
        pCfg->post2driver[i] = post2_driver;
    }

    if(tsc->verbosity & TSCMOD_DBG_TXDRV) {
       for (i = 0; i < loop; i++) {
          printf ("u=%0d p=%0d l=%0d cfg preemphasis=0x%x idriver=%0d pdriver=%0d"
           " post2=%0d\n", unit, port, i, pCfg->preemph[i], pCfg->idriver[i],
          pCfg->pdriver[i], pCfg->post2driver[i]);
       }
    }

    pCfg->auto_medium = soc_property_port_get(unit, port,
                              spn_SERDES_AUTOMEDIUM, pCfg->auto_medium);
    pCfg->fiber_pref = soc_property_port_get(unit, port,
                              spn_SERDES_FIBER_PREF, pCfg->fiber_pref);
    pCfg->sgmii_mstr = soc_property_port_get(unit, port,
                              spn_SERDES_SGMII_MASTER, pCfg->sgmii_mstr);
    pCfg->pdetect10g = soc_property_port_get(unit, port,
                              spn_XGXS_PDETECT_10G, pCfg->pdetect10g);
    pCfg->cx42hg = soc_property_port_get(unit, port,
                              spn_CX4_TO_HIGIG, pCfg->cx42hg);

    for(i = 0; i < (MAX_NUM_LANES/4) ; i++) {
       pCfg->rxlane_map[i] = soc_property_port_get(unit, port,
                                                spn_XGXS_RX_LANE_MAP, pCfg->rxlane_map[i]);
       pCfg->txlane_map[i] = soc_property_port_get(unit, port,
                                                spn_XGXS_TX_LANE_MAP, pCfg->txlane_map[i]);
    }

    pCfg->cl73an  = soc_property_port_get(unit, port,
                                          spn_PHY_AN_C73, pCfg->cl73an); /* no L: C73, not CL73 */

    pCfg->cl37an = soc_property_port_get(unit, port,
                                        spn_PHY_AN_C37, pCfg->cl37an); /* no L: C37, not CL37 */

    pCfg->an_cl72 = soc_property_port_get(unit, port,
                                        spn_PHY_AN_C72, pCfg->an_cl72);
    pCfg->forced_init_cl72 = soc_property_port_get(unit, port,
                                        spn_PORT_INIT_CL72, pCfg->forced_init_cl72);

    pCfg->rxaui = soc_property_port_get(unit, port,
                                    spn_SERDES_2WIRE_XAUI, FALSE);

    pCfg->txpol = soc_property_port_get(unit, port,
                            spn_PHY_XAUI_TX_POLARITY_FLIP, pCfg->txpol);
    pCfg->rxpol = soc_property_port_get(unit, port,
                            spn_PHY_XAUI_RX_POLARITY_FLIP, pCfg->rxpol);
    /*
    pCfg->refclk= soc_property_port_get(unit, port,
                                        spn_XGXS_LCPLL_XTAL_REFCLK,  pCfg->refclk) ;
    */
    tsc->refclk = pCfg->refclk;

    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        pCfg->lane_mode = soc_property_port_get(unit, port,
                          spn_PHY_HL65_1LANE_MODE, pCfg->lane_mode);
    }
    pCfg->cx4_10g = soc_property_port_get(unit, port,
                          spn_10G_IS_CX4, pCfg->cx4_10g);
    pCfg->lane0_rst = soc_property_get(unit,
                             spn_SERDES_LANE0_RESET, pCfg->lane0_rst);
    /* revisit
    pCfg->refclk = soc_property_port_get(unit,port,
                             spn_XGXS_PHY_VCO_FREQ, pCfg->refclk);
    */
    pCfg->init_speed = soc_property_port_get(unit, port,
                             spn_PORT_INIT_SPEED, pCfg->init_speed);

    pCfg->pll_divider = soc_property_port_get(unit, port,
                             spn_XGXS_PHY_PLL_DIVIDER, pCfg->pll_divider);

    PHY_FLAGS_SET(unit,port,PHY_FLAGS_SERVICE_INT_PHY_LINK_GET);

    pCfg->sw_rx_los.enable = soc_property_port_get(unit, port,
                             spn_SERDES_RX_LOS, pCfg->sw_rx_los.enable);


    if (!PHY_EXTERNAL_MODE(unit, port)) {

        if (pCfg->fiber_pref) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        }

        if (pCfg->cl73an) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_C73);
        }
    }

    /*get all the warpCore tx/rx lane swap for multi-core port */
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0; i < num_core; i++) {
            tx_lane_swap = soc_property_port_suffix_num_get(unit, port, i,
                                        spn_XGXS_TX_LANE_MAP,
                                        "core", pCfg->txlane_map[i]);
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "Port %d WarpCore tx lane swap - core %d -> tx_lane_swap 0x%x\n"),
                      port, i, tx_lane_swap));
            pCfg->txlane_map[i] = tx_lane_swap;
        }
    }

    /*get all the warpCore tx/rx lane swap for multi-core port */
    if ((SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) || (SOC_INFO(unit).port_num_lanes[pc->port] >= 10)) {
        num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
        for (i = 0; i < num_core; i++) {
            rx_lane_swap = soc_property_port_suffix_num_get(unit, port, i,
                                        spn_XGXS_RX_LANE_MAP,
                                        "core", pCfg->rxlane_map[i]);
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "Port %d WarpCore rx lane swap - core %d -> rx_lane_swap 0x%x\n"),
                      port, i, rx_lane_swap));
            pCfg->rxlane_map[i] = rx_lane_swap;
        }
    }

    line_intf = soc_property_port_get(unit, port, spn_SERDES_IF_TYPE, line_intf) ;
    if(line_intf>0) {
        pCfg->line_intf = 1 << tscmod_tr_if_mode(line_intf) ;
    }

    pCfg->am_reserved = soc_property_port_get(unit, port,
                             spn_SERDES_PCS_20G_ALIGNMENT_MARKER_RESERVED,
                                              pCfg->am_reserved) ;

    ctrl_flag = soc_property_port_get(unit, port,
                             spn_SERDES_USE_PROXY_REMOVAL, ctrl_flag);
    if(ctrl_flag !=0) 
        pCfg->ctrl_flag = TSCMOD_CFG_CTRL_FLAG_WR_MASK_EN;

    /* update tsc members */
    if(pCfg->forced_init_cl72<=0)
       tsc->firmware_mode = pCfg->firmware_mode ;

    if (PHY_INDEPENDENT_LANE_MODE(pc->unit, pc->port)) {
       tsc->port_type  = TSCMOD_MULTI_PORT;
       if (IS_DUAL_LANE_PORT(pc)) {
          tsc->dxgxs = (pc->lane_num) ? 2 : 1 ;
          tsc->port_type  = TSCMOD_DXGXS ;
       }
    }  else {
       tsc->port_type  = TSCMOD_SINGLE_PORT;
    }
    switch (pc->lane_num) {
    case 0:
       tsc->lane_select = TSCMOD_LANE_0_0_0_1;
       break;
    case 1:
       tsc->lane_select = TSCMOD_LANE_0_0_1_0;
       break;
    case 2:
       tsc->lane_select = TSCMOD_LANE_0_1_0_0;
       break;
    case 3:
       tsc->lane_select = TSCMOD_LANE_1_0_0_0;
       break;
    default:
       return SOC_E_PARAM;
    }
    tsc->this_lane = pc->lane_num;

    tsc->an_ctl = pCfg->an_ctl ;

    tsc->plldiv = pCfg->pll_divider ;

    /* for forced speed mode */
    if((pCfg->forced_init_cl72>0)&&(pCfg->forced_init_cl72<16))
       FORCE_CL72_MODE(pc)    = 1 ;
    else
       FORCE_CL72_MODE(pc)    = 0 ;

    FORCE_CL72_ENABLED(pc) = 0 ;
    FORCE_CL72_STATE(pc)   = TSCMOD_FORCE_CL72_IDLE ;
    FORCE_CL72_RESTART_CNT(pc) = 0 ;

    if(DEV_CFG_PTR(pc)->hg_mode)
        tsc->ctrl_type = tsc->ctrl_type | TSCMOD_CTRL_TYPE_HG ;

    if(pCfg->fault_disable)
       tsc->ctrl_type = tsc->ctrl_type | TSCMOD_CTRL_TYPE_FAULT_DIS ;

    if(pCfg->load_mthd)
       tsc->ctrl_type = tsc->ctrl_type | TSCMOD_CTRL_TYPE_FW_AVAIL ;

    pCfg->cidL = 0 ;
    pCfg->cidH = 0 ;
    pCfg->cidU = 0 ;
    pCfg->cidV = 0 ;

    pCfg->cidL |= TSC623_SDK_45197 ;
    pCfg->cidL |= TSC623_SDK_42585_A1 ;
    pCfg->cidL |= TSC623_PHY_887 ;
    pCfg->cidL |= TSC623_PHY_891 ;
    pCfg->cidL |= TSC623_PHY_884 ;
    pCfg->cidL |= TSC623_PHY_896 ;
    pCfg->cidL |= TSC623_PHY_867 ;
    pCfg->cidL |= TSC623_SDK_45418 ;
    pCfg->cidL |= TSC623_SDK_45283 ;
    pCfg->cidL |= TSC623_SDK_45731 ;
    pCfg->cidL |= TSC623_PHY_916 ;
    pCfg->cidL |= TSC623_PHY_892 ;
    pCfg->cidL |= TSC623_SDK_45682 ;
    pCfg->cidL |= TSC623_SDK_45789_A1 ;
    pCfg->cidL |= TSC623_SDK_46061 ;
    pCfg->cidL |= TSC623_SDK_46060 ;
    pCfg->cidL |= TSC623_PHY_917 ;
    pCfg->cidL |= TSC623_PHY_863 ;
    pCfg->cidL |= TSC623_PHY_864 ;
    pCfg->cidL |= TSC623_SDK_46338 ;
    pCfg->cidL |= TSC623_SDK_43138 ;
    pCfg->cidL |= TSC623_SDK_46475 ;
    pCfg->cidL |= TSC623_SDK_46753 ;
    pCfg->cidL |= TSC623_SDK_46913 ;
    pCfg->cidL |= TSC623_SDK_46955 ;

    pCfg->cidH |= TSC623_SDK_46725_P0 ;
    pCfg->cidH |= TSC623_SDK_46725_P1 ;
    pCfg->cidH |= TSC623_SDK_47112 ;
    pCfg->cidH |= TSC623_SDK_47114 ;
    pCfg->cidH |= TSC623_SDK_46764 ;
    pCfg->cidH |= TSC623_SDK_46923 ;
    pCfg->cidH |= TSC623_PHY_999 ;
    pCfg->cidH |= TSC623_PHY_926 ;
    pCfg->cidH |= TSC623_SDK_47195 ;
    pCfg->cidH |= TSC623_SDK_47058 ;
    pCfg->cidH |= TSC623_SDK_47365 ;
    pCfg->cidH |= TSC623_PHY_1015 ;
    pCfg->cidH |= TSC623_SDK_46788 ;
    pCfg->cidH |= TSC623_PHY_965 ;
    pCfg->cidH |= TSC623_PHY_1023 ;
    pCfg->cidH |= TSC623_SDK_46785 ;
    pCfg->cidH |= TSC623_SDK_46786 ;
    pCfg->cidH |= TSC623_PHY_1013 ;
    pCfg->cidH |= TSC623_PHY_1073 ;
    pCfg->cidH |= TSC623_SDK_48904 ;
    pCfg->cidH |= TSC623_PHY_1100 ;
    pCfg->cidH |= TSC623_SDK_48705 ;

    pCfg->cidH |= TSC628_PHY_1095 ;
    pCfg->cidH |= TSC628_PHY_1108 ;
    pCfg->cidH |= TSC628_PHY_1105 ;

    pCfg->cidU |= TSC628_PHY_1111 ;
    pCfg->cidU |= TSC628_SDK_49139 ;
    pCfg->cidU |= TSC628_PHY_1116 ;
    pCfg->cidU |= TSC628_PHY_1119 ;
    pCfg->cidU |= TSC628_SDK_49889 ;
    pCfg->cidU |= TSC628_PHY_1092 ;
    pCfg->cidU |= TSC628_PHY_1123 ;
    pCfg->cidU |= TSC628_SDK_49835 ;
    pCfg->cidU |= TSC628_SDK_50095 ;
    pCfg->cidU |= TSC628_SDK_48752 ;
    pCfg->cidU |= TSC628_PHY_1166 ;
    pCfg->cidU |= TSC628_SDK_47653 ;
    pCfg->cidU |= TSC628_SDK_51441 ;
    pCfg->cidU |= TSC628_SDK_51674 ;
    pCfg->cidU |= TSC628_SDK_48449 ;
    pCfg->cidU |= TSC628_SDK_51547 ;
    pCfg->cidU |= TSC628_SDK_52038 ;
    pCfg->cidU |= TSC628_SDK_50212 ;
    pCfg->cidU |= TSC628_SDK_48449_P2 ;
    pCfg->cidU |= TSC628_SDK_52241 ;
    pCfg->cidU |= TSC628_SDK_52713 ;
    pCfg->cidU |= TSC628_SDK_52715 ;
    pCfg->cidU |= TSC628_SDK_51300 ;
    pCfg->cidU |= TSC628_SDK_52833 ;
    pCfg->cidU |= TSC628_SDK_53261 ;

    pCfg->cidV |= TSC640_SDK_39435 ;
    pCfg->cidV |= TSC640_SDK_53070 ;
    pCfg->cidV |= TSC640_SDK_53940 ;
    pCfg->cidV |= TSC640_SDK_54357 ;
    pCfg->cidV |= TSC640_SDK_54392 ;
    pCfg->cidV |= TSC640_SDK_54943 ;
    pCfg->cidV |= TSC640_SDK_53855 ;
    pCfg->cidV |= TSC640_SDK_54606 ;
    pCfg->cidV |= TSC640_SDK_54950 ;
    pCfg->cidV |= TSC640_SDK_48996 ;
    pCfg->cidV |= TSC640_SDK_55448 ;
    pCfg->cidV |= TSC640_SDK_56119 ;
    pCfg->cidV |= TSC640_SDK_56749 ;
    pCfg->cidV |= TSC640_SDK_55315 ;
    pCfg->cidV |= TSC640_SDK_57764 ;
    pCfg->cidV |= TSC640_SDK_58338 ;

    if(pCfg->cidL & TSC623_SDK_46061) {
       if(!(pCfg->cidL & TSC623_SDK_43138)){
          tsc->ctrl_type |= TSCMOD_CTRL_TYPE_SEMA_ON ;
       }
       tsc->ctrl_type |= TSCMOD_CTRL_TYPE_SEMA_CHK ;
    }
    
    SOC_IF_ERROR_RETURN
            (soc_mac_probe(unit, port, &DEV_CFG_PTR(pc)->macd));
    if(DEV_CFG_PTR(pc)->macd==NULL) {
        printf("%-22s: u=%0d p=%0d mac ptr is not initialized in TSCMOD\n",
               FUNCTION_NAME(), tsc->unit, tsc->port) ;
    }

    if(tsc->verbosity & TSCMOD_DBG_CFG) {
       printf ("%-22s: u=%0d p=%0d port_type=%0d\n",
              FUNCTION_NAME(), tsc->unit, tsc->port, tsc->port_type) ;
       printf ("       lane_num=%0d this_lane=%0d lane_sel=%0d\n",
              pc->lane_num, tsc->this_lane, tsc->lane_select) ;
    }

    tscmod_sema_create(tsc->unit, tsc->port) ;

    /* the warm boot support.  make sure that current_ver is updated per SW release */
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
       int hg, fw_mode, intf;
       int scr;
       int asp_mode, speed ;
       int current_ver ;
       /* uint16 data16; */

       intf = 0;
       current_ver = TSCMOD_WB_SC_VER1 ;
       /* correct HG state */
       tsc->diag_type = TSCMOD_DIAG_HG;
       tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
       hg             = tsc->accData ;
       if(hg) {
          tsc->ctrl_type |= TSCMOD_CTRL_TYPE_HG ;
       } else {
          if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_HG)
             tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_HG ;
       }

       /* correct intf and etc */
       rv = _phy_tscmod_speed_get(tsc->unit, port, &speed, &intf, &asp_mode, &scr);

       pCfg->line_intf =  1 << tscmod_tr_if_mode(intf);
       pCfg->scrambler_en = scr;

       if(pCfg->firmware_mode==-1) {
          tsc->per_lane_control = TSCMOD_FWMODE_RD ;
          tscmod_tier1_selector("FWMODE_CONTROL", tsc, &rv);
          fw_mode = tsc->accData ;
          if(fw_mode == TSCMOD_UC_CTRL_SFP_DAC){
             pCfg->medium = SOC_PORT_MEDIUM_COPPER;
          } else {
             pCfg->medium = SOC_PORT_MEDIUM_FIBER;
          }
       }
       
       SOC_IF_ERROR_RETURN(tscmod_wb_sc_reinit(unit, port)) ;

       if(current_ver > pCfg->scache_ver) { /* upgrade expandsion and update */ 
           SOC_IF_ERROR_RETURN(tscmod_wb_sc_realloc_init(unit, port)) ;
           SOC_IF_ERROR_RETURN(tscmod_wb_sc_sync(unit, port)) ;
       }

    } else {
        /* cold boot here */
        SOC_IF_ERROR_RETURN(tscmod_wb_sc_init(unit, port)) ;

        SOC_IF_ERROR_RETURN(tscmod_wb_sc_sync(unit, port)) ;
    }
#endif

    return rv;
}

STATIC
int _tscmod_soft_reset(int unit, phy_ctrl_t  *pc)
{
    tscmod_st            *tsc;      /*tscmod structure */
    TSCMOD_DEV_DESC_t    *pDesc;
    int rv;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);


    tsc->per_lane_control = 0 ;
    tscmod_tier1_selector("SOFT_RESET", tsc, &rv);


    return(rv|SOC_E_NONE);
}

STATIC
int phy_tscmod_soft_reset(int unit, soc_port_t port, void * pointer)
{
    phy_ctrl_t    *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;

    /*copy the phy control pataremeter into tscmod*/
    SOC_IF_ERROR_RETURN
        (_tscmod_soft_reset(unit, pc));
    tscmod_sema_unlock(unit, port) ;

    return(SOC_E_NONE);
}

STATIC
int _phy_tscmod_tx_reset(int unit, phy_ctrl_t  *pc, int enable)
{
    tscmod_st            *tsc;      /*tscmod structure */
    TSCMOD_DEV_DESC_t    *pDesc;
    TSCMOD_DEV_CFG_t     *pCfg;

    int         tmp_sel, tmp_lane = 0 ;
    int rv;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);
    pCfg  = &pDesc->cfg;

    rv    = SOC_E_NONE ;

    tmp_sel              = tsc->lane_select ;
    tmp_lane             = tsc->this_lane   ;

    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
       tsc->lane_select      = TSCMOD_LANE_BCST ;
    }

    if(!enable) {
       if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_TX_SQUELCH)) {
           tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
           tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
       }
       tsc->per_lane_control = 1;
       tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

       tsc->per_lane_control = 1;
       tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

    } else {

       tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

       /* no need to call TSCMOD_SRESET_AFE_TX_ANALOG */
       tsc->per_lane_control = 0;
       tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

       tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
       tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

       tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
       tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

       sal_usleep(pCfg->los_usec) ;
    }

    tsc->lane_select = tmp_sel  ;  /* restore */
    tsc->this_lane   = tmp_lane ;

    return(rv|SOC_E_NONE);
}

STATIC
int _phy_tscmod_rx_reset(int unit, phy_ctrl_t  *pc, int enable)
{
    tscmod_st            *tsc;      /*tscmod structure */
    TSCMOD_DEV_DESC_t    *pDesc;
    int rv;
    int         tmp_sel, tmp_lane ;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);

    rv    = SOC_E_NONE ;

    tmp_sel              = tsc->lane_select ;
    tmp_lane             = tsc->this_lane   ;

    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
       tsc->lane_select      = TSCMOD_LANE_BCST ;
    }

    if(!enable) {
       tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

       sal_usleep(1000) ;
       if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
          tsc->lane_select      = TSCMOD_LANE_BCST ;
       }

       if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
          tsc->per_lane_control = TSCMOD_RXP_UC_ON ;
          tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
       } else {
          tsc->per_lane_control = TSCMOD_RXP_REG_ON ;
          tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
       }

    } else {

       /* no need to call TSCMOD_SRESET_AFE_RX_ANALOG unless pll change */

       tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

       sal_usleep(1000) ;
       if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
          tsc->lane_select      = TSCMOD_LANE_BCST ;
       }

       if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
          tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
          tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
       }
       tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
       tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

    }

    tsc->lane_select = tmp_sel  ;  /* restore */
    tsc->this_lane   = tmp_lane ;

    return(SOC_E_NONE);
}

STATIC int
_tscmod_get_lane_select(int unit, soc_port_t port, int lane_index)
{
    switch(lane_index) {
    case 0: return TSCMOD_LANE_0_0_0_1 ;
    case 1: return TSCMOD_LANE_0_0_1_0 ;
    case 2: return TSCMOD_LANE_0_1_0_0 ;
    case 3: return TSCMOD_LANE_1_0_0_0 ;
    default: {
        printf ("%s FATAL: u=%0d p=%0d index=%0d\n", FUNCTION_NAME(), unit, port, lane_index) ;
        return TSCMOD_LANE_0_0_0_1 ;
        }
    }
}

STATIC int
_phy_tscmod_cl72_enable_set(int unit, phy_ctrl_t *pc, int en)
{
   int rv, tmp_sel;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   TSCMOD_DEV_CFG_t      *pCfg;

   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;

   tmp_sel = tsc->lane_select ;

   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      tsc->lane_select      = TSCMOD_LANE_BCST ;
   }

   if(tsc->verbosity & TSCMOD_DBG_LINK)
      printf ("%-22s: u=%0d p=%0d cl72 en=%0d an_type=%s\n", FUNCTION_NAME(), tsc->unit,
             tsc->port, en, e2s_tscmod_an_type[tsc->an_type]) ;

   if((tsc->an_type != TSCMOD_AN_TYPE_ILLEGAL)&&
      (tsc->an_type != TSCMOD_AN_NONE)) {

      tsc->per_lane_control = TSCMOD_CL72_AN_NO_FORCED ;  /* let AN decide CL72 bit */
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_CL72_LINK_FAIL_TIMER ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_CL72_MAX_WAIT_TIMER ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      pCfg->an_cl72 = en ;
   } else {
      if(en) {
         /*When the interface type is 10G-CR, the firmware mode should not be changed*/
         if (!(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_CR)) {
             tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_FIRMWARE_MODE ;
             tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
         }

         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_CL72_LINK_FAIL_TIMER ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_CL72_MAX_WAIT_TIMER ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_TAP_MUXSEL ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         tsc->per_lane_control = TSCMOD_CL72_HW_ENABLE ;
         tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

         FORCE_CL72_MODE(pc)    = 1  ;
         FORCE_CL72_ENABLED(pc) = 1  ;
         FORCE_CL72_STATE(pc)   = TSCMOD_FORCE_CL72_CMD_DIS_IDLE  ;
         tsc->ctrl_type |= TSCMOD_CTRL_TYPE_CL72_WAR_ON ;
         pCfg->forced_init_cl72 = en ;
      } else {
         tsc->per_lane_control = TSCMOD_CL72_HW_DISABLE;
         tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_CL72_LINK_FAIL_TIMER ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         /*
         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_CL72_MAX_WAIT_TIMER ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
         */
         FORCE_CL72_MODE(pc)    = 0 ;
         FORCE_CL72_ENABLED(pc) = 0 ;
         FORCE_CL72_STATE(pc)   = TSCMOD_FORCE_CL72_IDLE  ;
         if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_CL72_WAR_ON)
             tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_CL72_WAR_ON ;
         pCfg->forced_init_cl72 = 0 ;
      }
   }

   tsc->lane_select    = tmp_sel ;
   return (rv|SOC_E_NONE);
}

STATIC int
_phy_tscmod_cl72_enable_get(int unit, phy_ctrl_t *pc, uint32 *en)
{
   int rv = SOC_E_NONE ;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   TSCMOD_DEV_CFG_t      *pCfg;

   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;

   if((tsc->an_type != TSCMOD_AN_TYPE_ILLEGAL)&&
      (tsc->an_type != TSCMOD_AN_NONE)) {
       *en = pCfg->an_cl72 ;
   } else {
       tsc->per_lane_control = TSCMOD_CL72_STATUS_READ ;
       tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
       
       *en = tsc->accData ;
   }
   return rv ;
}

/* lane control is slected in the caller func */
STATIC int
tscmod_an_cl72_control_set(int unit, phy_ctrl_t *pc)
{
   int rv ;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   TSCMOD_DEV_CFG_t      *pCfg;

   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)(pDesc + 1);
   pCfg  = &pDesc->cfg;

   rv = SOC_E_NONE ;

   /* for AN, HW shall not be set */
   tsc->per_lane_control = TSCMOD_CL72_HW_DISABLE ;
   tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

   if((tsc->an_type==TSCMOD_CL73)||(tsc->an_type==TSCMOD_CL73_BAM)||(tsc->an_type==TSCMOD_HPAM)) {
      if(pCfg->an_cl72) {
         tsc->per_lane_control = TSCMOD_CL72_AN_FORCED_ENABLE ; /* forced to enable CL72 */
         tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
      } else {
         tsc->per_lane_control = TSCMOD_CL72_AN_FORCED_DISABLE ; /* forced to disable CL72 */
         tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
      }
   }
   if((tsc->an_type==TSCMOD_CL37)||(tsc->an_type==TSCMOD_CL37_BAM)||(tsc->an_type==TSCMOD_CL37_10G)) {
      if(pCfg->an_cl72) {
         if(pCfg->forced_init_cl72>0) {
            tsc->per_lane_control = TSCMOD_CL72_AN_FORCED_ENABLE ; /* forced to disable CL72 */
            tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
         } else {
            tsc->per_lane_control = TSCMOD_CL72_AN_NO_FORCED ;
            tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
         }
      } else {
         if(pCfg->forced_init_cl72>0) {
            tsc->per_lane_control = TSCMOD_CL72_AN_FORCED_DISABLE ;
            tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
         } else {
            tsc->per_lane_control = TSCMOD_CL72_AN_NO_FORCED ;
            tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
         }
      }
   }

   return rv ;
}

STATIC int
tscmod_uc_fw_control(tscmod_st *tsc, int code_word, int en)
{
   int offset, rv, rerun; uint16 mask, enable, disable ;
   int tmp_select, tmp_lane, tmp_dxgxs ;
   int mode, invalid ;
   int loop, found, lane ; uint16 data0,data1,data2 ;

   tmp_select           = tsc->lane_select ;
   tmp_lane             = tsc->this_lane   ;
   tmp_dxgxs            = tsc->dxgxs       ;
   loop=0; found =0 ; lane =0; data0=0; data1=0; data2=0;

   if(tsc->verbosity & (TSCMOD_DBG_PRINT|TSCMOD_DBG_LINK)) {
           printf("uc_fw_control: u=%0d p=%0d sel=%x l=%0d dxgxs=%x port_t=%0d code_w=%x en=%0x\n",
                  tsc->unit, tsc->port, tsc->lane_select,
                  tmp_lane, tmp_dxgxs, tsc->port_type, code_word, en);
   }

   if(tsc->port_type == TSCMOD_SINGLE_PORT ) {
       mode = TSCMOD_UC_CORE ; 
   } else if (tsc->port_type == TSCMOD_DXGXS ) {
       if((tsc->dxgxs & 3)==2) { mode=TSCMOD_UC_D1 ; }
       else { mode = TSCMOD_UC_D0; }
   } else {
       mode = tsc->this_lane ;
   }
   invalid = 0 ;
   tsc->lane_select      = TSCMOD_LANE_0_0_0_1 ;
   tsc->this_lane        = 0 ;
   tsc->dxgxs            = 0 ;

   enable = 0x0e00; disable = 0xdd00;

   switch(code_word) {
      /* CL72_SWITCH_OVER is automatically reset to disable after CL72 finished by FW,
         regardless it is CL72 trained or not */
   case TSCMOD_UC_CL72_SWITCH_OVER:
       if(mode<TSCMOD_UC_CORE) {
           switch(mode) {
           case 0: offset = 0x4e4 ; break ;
           case 1: offset = 0x5e4 ; break ;
           case 2: offset = 0x6e4 ; break ;
           case 3: offset = 0x7e4 ; break ;
           default:
               offset = 0x4e4 ; break ;
           }
           tsc->this_lane        = tmp_lane ;
           tsc->lane_select      = getLaneSelect(tmp_lane) ;
       } else if(mode==TSCMOD_UC_CORE) {
           offset = 0x114;
       } else if((mode==TSCMOD_UC_D0)||(mode==TSCMOD_UC_D1)) {
           if(mode==TSCMOD_UC_D0) {
               offset = 0x12a ;
               enable = 0xe300;
           } else {
               offset = 0x12c ;
               enable = 0xec00;
           }
       } else {
           invalid = 1 ;
           offset  = 0 ;
       }
       break ;
   case TSCMOD_UC_CL72_CLEAR:
       enable = 0x1e00 ;
       if(mode<TSCMOD_UC_CORE) {
           switch(mode) {
           case 0: offset = 0x4e4 ; break ;
           case 1: offset = 0x5e4 ; break ;
           case 2: offset = 0x6e4 ; break ;
           case 3: offset = 0x7e4 ; break ;
           default:
               offset = 0x4e4 ; break ;
           }
           tsc->this_lane        = tmp_lane ;
           tsc->lane_select      = getLaneSelect(tmp_lane) ;
       } else if(mode==TSCMOD_UC_CORE) {
           offset = 0x114;
       } else if((mode==TSCMOD_UC_D0)||(mode==TSCMOD_UC_D1)) {
           if(mode==TSCMOD_UC_D0) {
               offset = 0x12a ;
           } else {
               offset = 0x12c ;
           }
       } else {
           invalid = 1 ;
           offset  = 0 ;
       }
       break ;
   case TSCMOD_UC_CL72_WAIT_CLEAN:
       if(mode<TSCMOD_UC_CORE) {
           switch(mode) {
           case 0: offset = 0x4e4 ; break ;
           case 1: offset = 0x5e4 ; break ;
           case 2: offset = 0x6e4 ; break ;
           case 3: offset = 0x7e4 ; break ;
           default:
               offset = 0x4e4 ; break ;
           }
           tsc->this_lane        = tmp_lane ;
           tsc->lane_select      = getLaneSelect(tmp_lane) ;
       } else if(mode==TSCMOD_UC_CORE) {
           offset = 0x114;
       } else if((mode==TSCMOD_UC_D0)||(mode==TSCMOD_UC_D1)) {
           if(mode==TSCMOD_UC_D0) {
               offset = 0x12a ;
           } else {
               offset = 0x12c ;
           }
       } else {
           invalid = 1 ;
           offset  = 0 ;
       }
       break ;
   case TSCMOD_UC_CREDIT_PROGRAM:   offset = 0x12e; break;
   case TSCMOD_UC_CL37_HR2_WAR:
      switch(tmp_lane) {
      case 0: offset = 0x4d4 ; break ;
      case 1: offset = 0x5d4 ; break ;
      case 2: offset = 0x6d4 ; break ;
      case 3: offset = 0x7d4 ; break ;
      default:
         offset = 0x4d4 ; break ;
      }
      tsc->this_lane        = tmp_lane ;
      tsc->lane_select      = getLaneSelect(tmp_lane) ;
      break ;
   case TSCMOD_UC_CL72_DEBUG_INFO:
      offset  = 0x490;
      break ;
   default:
       invalid = 1 ; 
       offset = 0x12e;  break;
   }
   mask   = 0xffff;  /* enable mask */
   rerun  = 0 ;
   rv     = SOC_E_NONE ;

   if (SOC_WARM_BOOT(tsc->unit) || SOC_IS_RELOADING(tsc->unit)) {
      return rv;
   }
   if(invalid !=0){
       tsc->ctrl_type |= TSCMOD_CTRL_TYPE_ERR ;
       tsc->err_code  |= TSCMOD_ERR_PLLDIV_SPD_SET ;
       if(tsc->verbosity & (TSCMOD_DBG_PRINT|TSCMOD_DBG_INIT|TSCMOD_DBG_SPD)) {
           printf ("Error: u=%0d p=%0d sel=%x l=%0d dxgxs=%x port_t=%0d code_w=%x\n",
                  tsc->unit, tsc->port, tsc->lane_select,
                  tmp_lane, tmp_dxgxs, tsc->port_type, code_word);
       }
       tsc->dxgxs       = tmp_dxgxs ;
       tsc->lane_select = tmp_select ;  /* restore */
       tsc->this_lane   = tmp_lane ;
       return rv ; 
   }
   if(code_word==TSCMOD_UC_CL72_WAIT_CLEAN) {
       found = 0 ;
       for(loop=0;loop<100;loop++){
           tsc->this_lane        = tmp_lane ;
           tsc->lane_select      = getLaneSelect(tmp_lane) ;
           tsc->accData          = 0 ;
           tsc->accAddr          = offset ;
           tsc->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
           tscmod_tier1_selector("UC_SYNC_CMD", tsc, &rv) ;
           if(tsc->accData == disable) {
               found = 1;
               break ;
           }
       }
       if(found==0) {
           if((tsc->verbosity & (TSCMOD_DBG_PRINT|TSCMOD_DBG_LINK|TSCMOD_DBG_SPD))||
              ((tsc->err_code&TSCMOD_ERR_UC_CLR_FAIL_CL72)==0)) {
               printf("Error: u=%0d p=%0d l=%0d sel=%0x dxgxs=%x port_t=%0d code_w=%x cl72 clr failed d=%0x\n",
                      tsc->unit, tsc->port, tmp_lane, tsc->lane_select,
                      tmp_dxgxs, tsc->port_type, code_word, tsc->accData);
           }
           tsc->ctrl_type |= TSCMOD_CTRL_TYPE_ERR ;
           tsc->err_code  |= TSCMOD_ERR_UC_CLR_FAIL_CL72 ;
           
           tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_DEBUG_INFO, 0) ;
       }
   } else if(code_word==TSCMOD_UC_CL72_DEBUG_INFO){
       for(lane=0; lane<4; lane++) {
           tsc->this_lane        = lane ;
           tsc->lane_select      = getLaneSelect(lane) ;
           tsc->accData          = 0 ;
           tsc->accAddr          = 0x490 + 0x100 * lane ;
           tsc->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
           tscmod_tier1_selector("UC_SYNC_CMD", tsc, &rv) ;
           data0 = tsc->accData ; 
           tsc->accAddr          = 0x492 + 0x100 * lane ;
           tsc->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
           tscmod_tier1_selector("UC_SYNC_CMD", tsc, &rv) ;
           data1 = tsc->accData ; 
           tsc->accAddr          = 0x49 + 0x100 * lane ;
           tsc->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
           tscmod_tier1_selector("UC_SYNC_CMD", tsc, &rv) ;
           data2 = tsc->accData ;
           printf("Error: u=%0d p=%0d l=%0d code_w=%x cl72 clr failed (0x%0x 0x92 0x94) d=%0x %0x %0x\n",
                  tsc->unit, tsc->port, lane, code_word, (0x490 + 0x100 * lane), data0, data1, data2);
       }
   } else {
       if(en) {
           tsc->accAddr          = offset ;
           tsc->accData          = 0 ;
           tsc->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
           tscmod_tier1_selector("UC_SYNC_CMD", tsc, &rv) ;
           
           if(tsc->accData==enable) rerun = 1 ;
           
           tsc->accAddr          = offset ;
           tsc->accData          = enable ;
           tsc->per_lane_control = (mask<<4)|TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_WR_MODE ;
           tscmod_tier1_selector("UC_SYNC_CMD", tsc, &rv) ;
           
           if(rerun) {
               tsc->accAddr          = offset ;
               tsc->accData          = enable ;
               tsc->per_lane_control = (mask<<4)|TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_WR_MODE ;
               tscmod_tier1_selector("UC_SYNC_CMD", tsc, &rv) ;
           }
       } else {
           tsc->accData          = disable ;
           tsc->accAddr          = offset ;
           tsc->per_lane_control = (mask<<4)|TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_WR_MODE ;
           tscmod_tier1_selector("UC_SYNC_CMD", tsc, &rv) ;
       }
   }
   
   tsc->dxgxs       = tmp_dxgxs ;
   tsc->lane_select = tmp_select ;  /* restore */
   tsc->this_lane   = tmp_lane ;
   return SOC_E_NONE ;
}

STATIC int
_phy_tscmod_cl72_status_get(int unit, phy_ctrl_t *pc, uint32 *status)
{
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   int rv, lane_s, lane_e, lane, i ;
   int tmp_sel, tmp_dxgxs, tmp_lane ;

   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

   tmp_sel   = tsc->lane_select ;
   tmp_dxgxs = tsc->dxgxs ;
   tmp_lane  = tsc->this_lane ;

   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      lane_s = 0 ; lane_e = 4 ;
   } else if (tsc->port_type == TSCMOD_DXGXS ) {
      if((tsc->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
      else { lane_s = 0; lane_e = 2 ; }
   } else {
      lane_s = tsc->this_lane ; lane_e = lane_s + 1 ;
   }

   *status = 0 ;  i = 0 ;
   for(lane=lane_s; lane<lane_e; lane++) {
      tsc->lane_select = _tscmod_get_lane_select(unit, tsc->port, lane) ;
      tsc->this_lane   = lane ;
      tsc->per_lane_control = TSCMOD_MISC_CTL_CL72_STAT_GET ;
      tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);

      *status = *status | ((tsc->accData & 0x1) << i) ;
      i++ ;
   }

   tsc->lane_select = tmp_sel ;
   tsc->dxgxs       = tmp_dxgxs ;
   tsc->this_lane   = tmp_lane ;

   return (SOC_E_NONE|rv);
}

/* workaround for JIRA CRTSC-690.  Different from WC  */
/* XFI has no uC CL72 supports */
STATIC int
_phy_tscmod_force_cl72_sw_link_recovery(int unit, soc_port_t port,int link)
{
   phy_ctrl_t      *pc;
   TSCMOD_DEV_DESC_t *pDesc;
   tscmod_st    *tsc;
   int rv, cl72_done, cl72_lock, lane, lane_s, lane_e,  tmp_select, tmp_lane, tmp_verb ;

   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);
   rv    = SOC_E_NONE ;

   lane                 = 0 ;
   tmp_select           = tsc->lane_select ;
   tmp_lane             = tsc->this_lane   ;
   tmp_verb             = tsc->verbosity   ;

   if(tsc->verbosity & TSCMOD_DBG_SCAN) {
   } else {  tsc->verbosity = 0 ; }

   if(tsc->verbosity & TSCMOD_DBG_LINK)
      printf ("%-22s: u=%0d p=%0d link=%0d state=%0x\n", FUNCTION_NAME(), tsc->unit,
             tsc->port, link, FORCE_CL72_STATE(pc)) ;

   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      lane_s = 0 ; lane_e = 4 ;
   } else if (tsc->port_type == TSCMOD_DXGXS ) {
      if((tsc->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
      else { lane_s = 0; lane_e = 2 ; }
   } else {
      lane_s = tsc->this_lane ; lane_e = lane_s + 1 ;
   }

   if( tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      tsc->lane_select = TSCMOD_LANE_BCST ;
   }

   tsc->lane_select = tmp_select ;  /* restore */
   tsc->this_lane   = tmp_lane ;

   cl72_done = 0 ;
   cl72_lock = 0 ;
   if(FORCE_CL72_STATE(pc)== TSCMOD_FORCE_CL72_IDLE) {

   } else if(FORCE_CL72_STATE(pc)== TSCMOD_FORCE_CL72_RESTART) {
      /* restart code */
      /* txp reset */
      tsc->per_lane_control = 0;
      tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);  

      tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_CL72_FW) 
         tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_SWITCH_OVER, 0) ;

      /* clean up */
      tsc->per_lane_control = TSCMOD_CL72_HW_DISABLE ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      tsc->per_lane_control = TSCMOD_CL72_STATUS_RESET ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
      
      tsc->per_lane_control = TSCMOD_CL72_HW_RESTART ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
      
      /* restart */
      tsc->per_lane_control = TSCMOD_CL72_HW_ENABLE ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      tsc->per_lane_control = TSCMOD_CL72_STATUS_RESET ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
     
      tsc->per_lane_control = TSCMOD_CL72_HW_RESTART ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      
      /* txp enabled */
      tsc->per_lane_control = 1;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      tsc->per_lane_control = 1;
      tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

      /* end of restart code */
      FORCE_CL72_STATE(pc)    = TSCMOD_FORCE_CL72_WAIT_TRAINING_LOCK ;
      FORCE_CL72_TICK_CNT(pc) = 0 ;
      if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf ("%-22s: u=%0d p=%0d link=%0d state=%0x txp en\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, link, FORCE_CL72_STATE(pc)) ;

   } else if(FORCE_CL72_STATE(pc)== TSCMOD_FORCE_CL72_DONE) {
      /* un-pulg the cable after link up */
      if(link) FORCE_CL72_TICK_CNT(pc)=0 ;
      else     FORCE_CL72_TICK_CNT(pc) = FORCE_CL72_TICK_CNT(pc) + 1 ;

      if(FORCE_CL72_TICK_CNT(pc)>=2) {
         FORCE_CL72_STATE(pc)    = TSCMOD_FORCE_CL72_RESTART ;
         FORCE_CL72_TICK_CNT(pc) = 0 ;
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf ("%-22s: u=%0d p=%0d link=%0d state=%0x restart\n", FUNCTION_NAME(), tsc->unit,
                tsc->port, link, FORCE_CL72_STATE(pc)) ;
      }
   } else if(FORCE_CL72_STATE(pc)== TSCMOD_FORCE_CL72_WAIT_TRAINING_LOCK){

      cl72_lock = 1 ;
      for(lane=lane_s; lane<lane_e; lane++) {
         tsc->lane_select = _tscmod_get_lane_select(unit, tsc->port, lane) ;
         tsc->this_lane   = lane ;

         tsc->diag_type = TSCMOD_DIAG_CL72;
         tsc->per_lane_control = TSCMOD_DIAG_CL72_LOCK ;   /* coarse lock */
         tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
         if(tsc->accData !=1) { cl72_lock=0 ; break ; }
      }

      if(tsc->verbosity & TSCMOD_DBG_LINK)
         printf ("%-22s: u=%0d p=%0d link=%0d state=%0x cl72_lock=%0d\n", FUNCTION_NAME(), tsc->unit,
                tsc->port, link, FORCE_CL72_STATE(pc), cl72_lock) ;

      if(cl72_lock) {

         tsc->lane_select = tmp_select ;  /* restore */
         tsc->this_lane   = tmp_lane ;

         if( tsc->port_type   == TSCMOD_SINGLE_PORT ) {
            tsc->lane_select = TSCMOD_LANE_BCST ;
         } else {
            tsc->lane_select = tmp_select ;
         }
         tsc->this_lane   = tmp_lane ;

         if(tsc->ctrl_type &TSCMOD_CTRL_TYPE_UC_RXP) {
            tsc->per_lane_control =  TSCMOD_RXP_REG_ON ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

            tsc->per_lane_control =  TSCMOD_RXP_UC_ON ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
         } else {
            tsc->per_lane_control =  TSCMOD_RXP_REG_ON ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
         }

         FORCE_CL72_STATE(pc) = TSCMOD_FORCE_CL72_WAIT_TRAINING_DONE ;

         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf ("%-22s: u=%0d p=%0d link=%0d state=%0x wait_training_done\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, link, FORCE_CL72_STATE(pc)) ;

      }
   } else if(FORCE_CL72_STATE(pc)== TSCMOD_FORCE_CL72_WAIT_TRAINING_DONE) {
      cl72_done = 1 ;
      for(lane=lane_s; lane<lane_e; lane++) {
         tsc->lane_select = _tscmod_get_lane_select(unit, tsc->port, lane) ;
         tsc->this_lane   = lane ;

         tsc->diag_type = TSCMOD_DIAG_CL72;
         tsc->per_lane_control = TSCMOD_DIAG_CL72_DONE ;
         tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
         if(tsc->accData !=1) { cl72_done=0 ; break ; }
      }

      if(tsc->verbosity & TSCMOD_DBG_LINK)
         printf ("%-22s: u=%0d p=%0d link=%0d state=%0x cl72_done=%0d\n", FUNCTION_NAME(), tsc->unit,
                tsc->port, link, FORCE_CL72_STATE(pc), cl72_done) ;

      if(cl72_done) {

         if( tsc->port_type   == TSCMOD_SINGLE_PORT ) {
            tsc->lane_select = TSCMOD_LANE_BCST ;
         } else {
            tsc->lane_select = tmp_select ;
         }
         tsc->this_lane   = tmp_lane ;

         /* reset TXP and RXP for once after CL72 done */
         if(tsc->ctrl_type &TSCMOD_CTRL_TYPE_UC_RXP) {
            tsc->per_lane_control =  TSCMOD_RXP_UC_ON ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
         } else {
            tsc->per_lane_control =  TSCMOD_RXP_REG_OFF ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
         }

         tsc->per_lane_control = 0;
         tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

         /*
         tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
         tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
         */

         tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
         tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

         tsc->per_lane_control = 1 ;
         tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

         if(tsc->ctrl_type &TSCMOD_CTRL_TYPE_UC_RXP) {
            /* TSCMOD_RXP_UC_ON is set */
         } else {
            tsc->per_lane_control =  TSCMOD_RXP_REG_ON ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
         }

         tsc->per_lane_control = 1;
         tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

         FORCE_CL72_STATE(pc) = TSCMOD_FORCE_CL72_TXP_ENABLED ;
      }
   } else if(FORCE_CL72_STATE(pc) == TSCMOD_FORCE_CL72_TXP_ENABLED ) {
      if(link) {
         FORCE_CL72_STATE(pc) = TSCMOD_FORCE_CL72_DONE ;
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf ("%-22s: u=%0d p=%0d link=%0d state=%0x frm txp_en\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, link, FORCE_CL72_STATE(pc)) ;
      }
   } else {
      if(tsc->verbosity & TSCMOD_DBG_LINK)
         printf ("%-22s: u=%0d p=%0d link=%0d state=%0x cl72_done=%0d unknown\n", FUNCTION_NAME(), tsc->unit,
                tsc->port, link, FORCE_CL72_STATE(pc), cl72_done) ;
   }
   tsc->lane_select = tmp_select ;  /* restore */
   tsc->this_lane   = tmp_lane ;
   tsc->verbosity   = tmp_verb ;

   return SOC_E_NONE ;
}

STATIC int
tscmod_force_cl72_wait_uc_clr(tscmod_st *tsc, int dbg_id)
{   
    int rv, tmp_select, tmp_lane, tmp_dxgxs, tmp_verb ;
    int lane, lane_s, lane_e, loop, found ;
    lane =0 ; lane_s=0 ; lane_e = 0 ; loop=0; found = 0;
    
    tmp_select           = tsc->lane_select ;
    tmp_lane             = tsc->this_lane   ;
    tmp_dxgxs            = tsc->dxgxs ;
    tmp_verb             = tsc->verbosity   ;

    tsc->per_lane_control = TSCMOD_RXP_UC_CLR ;
    tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
    
    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
        lane_s = 0 ; lane_e = 4 ;
    } else if (tsc->port_type == TSCMOD_DXGXS ) {
        if((tsc->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
        else { lane_s = 0; lane_e = 2 ; }
    } else {
        lane_s = tsc->this_lane ; lane_e = lane_s + 1 ;
    }
    tsc->dxgxs = 0 ; 
    
    for(lane=lane_s; lane<lane_e; lane++) {
        tsc->lane_select = getLaneSelect(lane) ;
        tsc->this_lane   = lane ;
        found = 0 ;
        for(loop=0; loop<100;loop++) {
            tsc->per_lane_control = TSCMOD_RXP_UC_RD ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
            if(tsc->accData==0xdd00) { found = 1 ; break ; }
        }
        if(found==0) {
            if((tsc->verbosity & (TSCMOD_DBG_PRINT|TSCMOD_DBG_LINK|TSCMOD_DBG_SPD))||
               ((tsc->err_code&TSCMOD_ERR_UC_CLR_FAIL_RXP)==0)) {
                printf("Error: u=%0d p=%0d l=%0d l_e=%0d sel=%0x dxgxs=%x port_t=%0d rxp clr failed d=%0x @cid=%0d\n",
                       tsc->unit, tsc->port, lane, lane_e, tsc->lane_select,
                       tsc->dxgxs, tsc->port_type, tsc->accData, dbg_id);
            }
            tsc->ctrl_type |= TSCMOD_CTRL_TYPE_ERR ;
            tsc->err_code  |= TSCMOD_ERR_UC_CLR_FAIL_RXP ;
            
            tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_DEBUG_INFO, 0) ;
            
        } else {
            if(tsc->verbosity & (TSCMOD_DBG_PRINT|TSCMOD_DBG_LINK|TSCMOD_DBG_SPD)) {
                printf("u=%0d p=%0d l=%0d l_e=%0d sel=%0x dxgxs=%x port_t=%0d rxp clr done d=%0x @cid=%0d\n",
                       tsc->unit, tsc->port, lane, lane_e, tsc->lane_select,
                       tsc->dxgxs, tsc->port_type, tsc->accData, dbg_id);
            }
        }
    }

    tsc->lane_select = tmp_select ;  /* restore */
    tsc->this_lane   = tmp_lane ;
    tsc->dxgxs       = tmp_dxgxs ;
    tsc->verbosity   = tmp_verb ;
 
    return SOC_E_NONE ;
}

/* used when uc_rxp is enabled */
STATIC int
_phy_tscmod_force_cl72_sw_link_uc_rxp(int unit, soc_port_t port,int *link)
{
   phy_ctrl_t      *pc;
   TSCMOD_DEV_DESC_t *pDesc;
   tscmod_st    *tsc;
   int rv, tmp_select, tmp_lane, tmp_dxgxs, tmp_verb ;
   uint32            current_time ;

   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);
   rv    = SOC_E_NONE ;
   current_time = 0 ;

   tmp_select           = tsc->lane_select ;
   tmp_lane             = tsc->this_lane   ;
   tmp_dxgxs            = tsc->dxgxs ;
   tmp_verb             = tsc->verbosity   ;

   if((TSCMOD_MODEL_REV_GET(tsc)>=2)||
      (TSCMOD_MODEL_PROJ_GET(tsc)==TSCMOD_MODEL_PROJ_HR2)) {
      /* rev>=2 or HR2 */
   } else {
      return rv ;
   }

   if( tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      tsc->lane_select = TSCMOD_LANE_BCST ;
   }

   if((tsc->verbosity & TSCMOD_DBG_FSM)&&
      ((FORCE_CL72_STATE(pc)!=TSCMOD_FORCE_CL72_IDLE)&&
       (FORCE_CL72_STATE(pc)!=TSCMOD_FORCE_CL72_DONE))) {
       printf("%-22s: u=%0d p=%0d link=%0d state=%0x\n", FUNCTION_NAME(), tsc->unit,
              tsc->port, *link, FORCE_CL72_STATE(pc)) ;
   }
   if(FORCE_CL72_STATE(pc)== TSCMOD_FORCE_CL72_IDLE) {
      FORCE_CL72_TICK_CNT(pc) = 0 ;
   } else if(FORCE_CL72_STATE(pc)== TSCMOD_FORCE_CL72_CMD_DIS_IDLE) { /* disabled by cmd */
      FORCE_CL72_TICK_CNT(pc) = 0 ;
      if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_LINK_DIS) {

      } else {
         /* enabled by port command */
         FORCE_CL72_STATE(pc)    = TSCMOD_FORCE_CL72_RESTART ;
         FORCE_CL72_TICK_CNT(pc) = 0 ;
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf ("%-22s: u=%0d p=%0d link=%0d state=%0x restart frm cmd dis\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, FORCE_CL72_STATE(pc)) ;
      }
   } else if(FORCE_CL72_STATE(pc)== TSCMOD_FORCE_CL72_DONE) { /* link up */
      if(FORCE_CL72_TICK_CNT(pc)>=32) { /* flip between link up and down */
         *link = 0 ; 
         FORCE_CL72_STATE(pc)    = TSCMOD_FORCE_CL72_RESTART ;
         FORCE_CL72_TICK_CNT(pc) = 0 ;
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf ("%-22s: u=%0d p=%0d link=%0d state=%0x restart frm done\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, FORCE_CL72_STATE(pc)) ;
      }
      if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_LINK_DIS) {
         FORCE_CL72_STATE(pc)    = TSCMOD_FORCE_CL72_CMD_DIS_IDLE ;
         FORCE_CL72_TICK_CNT(pc) = 0 ;
         rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_CL72_WAR) ;
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf ("%-22s: u=%0d p=%0d link=%0d state=%0x cmd dis\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, FORCE_CL72_STATE(pc)) ;
      } else if((*link==0) &&(tsc->ctrl_type&TSCMOD_CTRL_TYPE_CL72_WAR_ON)) {
         FORCE_CL72_STATE(pc)    = TSCMOD_FORCE_CL72_LINK_DOWN ;
         FORCE_CL72_TICK_CNT(pc) = 0 ;
         rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_CL72_WAR) ;
         current_time = sal_time_usecs() ;
         FORCE_CL72_START_TIME(pc) = current_time + ((current_time&0xf0)<<8) + TSCMOD_FORCE_CL72_RESTART_TIME ;
         /* 625000 us */
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf ("%-22s: u=%0d p=%0d link=%0d state=%0x lk down\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, FORCE_CL72_STATE(pc)) ;
      }
   } else if(FORCE_CL72_STATE(pc)== TSCMOD_FORCE_CL72_LINK_DOWN) {
      if(FORCE_CL72_TICK_CNT(pc)<255) {
         FORCE_CL72_TICK_CNT(pc) = FORCE_CL72_TICK_CNT(pc) + 1 ; 
      }
      if(*link) {
         FORCE_CL72_STATE(pc)    = TSCMOD_FORCE_CL72_DONE ;
         FORCE_CL72_TICK_CNT(pc) = 0 ;
         rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_CL72_WAR) ;
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf ("%-22s: u=%0d p=%0d link=%0d state=%0x done frm lkdwn\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, FORCE_CL72_STATE(pc)) ;
      } else {
         /* link down long enough */
         current_time = sal_time_usecs() ;
         if((SAL_USECS_SUB(current_time,FORCE_CL72_START_TIME(pc))>=0)&&(FORCE_CL72_TICK_CNT(pc)>=TSCMOD_FORCE_CL72_RESTART_CNT)) {    
            FORCE_CL72_STATE(pc)    = TSCMOD_FORCE_CL72_RESTART ;
            FORCE_CL72_TICK_CNT(pc) = 0 ;
            if(tsc->verbosity & TSCMOD_DBG_FSM)
               printf ("%-22s: u=%0d p=%0d link=%0d state=%0x restart frm lkdwm t=%0x\n", FUNCTION_NAME(), tsc->unit,
                      tsc->port, *link, FORCE_CL72_STATE(pc), current_time) ;
         }
      }
   } else if(FORCE_CL72_STATE(pc)== TSCMOD_FORCE_CL72_RESTART) {
       current_time = sal_time_usecs() ;
       if(FORCE_CL72_TICK_CNT(pc)==0) {
           FORCE_CL72_START_TIME(pc) = current_time + TSCMOD_FORCE_CL72_RESTART_TXIDLE_TIME ;
           
           tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
           tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

           tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
           tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
           if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY) {
               tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY ;
               tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
           }

           /* txp reset */
           tsc->per_lane_control = 0;
           tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);  

           tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
           tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
           
           tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
           tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
      
           if(tsc->verbosity & TSCMOD_DBG_FSM)
               printf("%-22s: u=%0d p=%0d link=%0d state=%0x restart idle t=%0x\n", FUNCTION_NAME(), tsc->unit,
                      tsc->port, *link, FORCE_CL72_STATE(pc), current_time) ;
           
       } else {
           if((SAL_USECS_SUB(current_time,FORCE_CL72_START_TIME(pc))>=0)&&
              (FORCE_CL72_TICK_CNT(pc)>=TSCMOD_FORCE_CL72_RESTART_TXIDLE_CNT)){
               FORCE_CL72_STATE(pc)    = TSCMOD_FORCE_CL72_RESTART_ACTION ;
               FORCE_CL72_TICK_CNT(pc) = 0 ;
               if(tsc->verbosity & TSCMOD_DBG_FSM)
                   printf("%-22s: u=%0d p=%0d link=%0d state=%0x to restart_action t=%0x\n", FUNCTION_NAME(), tsc->unit,
                          tsc->port, *link, FORCE_CL72_STATE(pc), current_time) ;
           }
       }
       FORCE_CL72_TICK_CNT(pc) = FORCE_CL72_TICK_CNT(pc)+1 ;
   } else if(FORCE_CL72_STATE(pc)== TSCMOD_FORCE_CL72_RESTART_ACTION) {
      /* clean up */
      tsc->per_lane_control = TSCMOD_CL72_HW_DISABLE ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      tsc->per_lane_control = TSCMOD_CL72_STATUS_RESET ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
      
      tsc->per_lane_control = TSCMOD_CL72_HW_RESTART ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_CL72_FW) 
         tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_SWITCH_OVER, 0) ;
      
      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_CL72_FW) {
         tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

         tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }
      
      /* restart */
      tsc->per_lane_control = TSCMOD_CL72_HW_ENABLE ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      tsc->per_lane_control = TSCMOD_CL72_STATUS_RESET ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
      
      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_CL72_FW) {     
         tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_CLEAR, 1) ;

         tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_WAIT_CLEAN, 1) ;     

         tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

         tscmod_force_cl72_wait_uc_clr(tsc, 1) ;
         
         tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_SWITCH_OVER, 1) ;

         if( tsc->port_type   == TSCMOD_SINGLE_PORT ) {
             tsc->lane_select = TSCMOD_LANE_BCST ;
         }
         tsc->dxgxs             = tmp_dxgxs ;
         tsc->this_lane         = tmp_lane ; 

         tsc->per_lane_control = TSCMOD_RXP_REG_ON ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

         tsc->per_lane_control = TSCMOD_RXP_UC_ON ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }
      
      tsc->per_lane_control = TSCMOD_CL72_HW_RESTART ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      /* txp enabled */
      tsc->per_lane_control = 1;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      tsc->per_lane_control = 1;
      tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

      if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_TX_SQUELCH)) {
          tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }
      tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY) {
          tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }

      /* end of restart code */
      FORCE_CL72_STATE(pc)    = TSCMOD_FORCE_CL72_TXP_ENABLED ;
      FORCE_CL72_TICK_CNT(pc) = 0 ;
      FORCE_CL72_RESTART_CNT(pc) = FORCE_CL72_RESTART_CNT(pc)+1 ;
      if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf ("%-22s: u=%0d p=%0d link=%0d state=%0x txp_en t=%0x\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, FORCE_CL72_STATE(pc), sal_time_usecs()) ;
   } else if(FORCE_CL72_STATE(pc)== TSCMOD_FORCE_CL72_TXP_ENABLED ) {
      if(*link) {
         FORCE_CL72_STATE(pc)    = TSCMOD_FORCE_CL72_DONE ;
         FORCE_CL72_TICK_CNT(pc) = 0 ;
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf ("%-22s: u=%0d p=%0d link=%0d state=%0x done frm txp_en\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, FORCE_CL72_STATE(pc)) ;
      } else {
         if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_CL72_WAR_ON) {
            FORCE_CL72_STATE(pc)      = TSCMOD_FORCE_CL72_LINK_DOWN ;
            FORCE_CL72_TICK_CNT(pc)   = 0 ;
            rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_CL72_WAR) ;

            current_time = sal_time_usecs() ;
            FORCE_CL72_START_TIME(pc) = current_time + ((current_time&0xf0)<<8) + TSCMOD_FORCE_CL72_RESTART_TIME ;
            if(tsc->verbosity & TSCMOD_DBG_FSM)
               printf ("%-22s: u=%0d p=%0d link=%0d state=%0x txp_en to link down to=%0x curr_t=%0x\n", FUNCTION_NAME(), tsc->unit,
                      tsc->port, *link, FORCE_CL72_STATE(pc), FORCE_CL72_START_TIME(pc), current_time) ;
         }
      }
   }
   else {
      if(tsc->verbosity & TSCMOD_DBG_LINK)
         printf ("%-22s: u=%0d p=%0d link=%0d state=%0x unknown\n", FUNCTION_NAME(), tsc->unit,
                tsc->port, *link, FORCE_CL72_STATE(pc)) ;
   }
   tsc->lane_select = tmp_select ;  /* restore */
   tsc->this_lane   = tmp_lane ;
   tsc->dxgxs       = tmp_dxgxs ;
   tsc->verbosity   = tmp_verb ;

   return rv ;
}

STATIC int
_phy_tscmod_control_firmware_mode_set (int unit,  phy_ctrl_t *pc, uint32 value)
{

   TSCMOD_DEV_DESC_t *pDesc;
   TSCMOD_DEV_CFG_t  *pCfg;
   tscmod_st    *tsc;
   int              rv, tmp_select, tmp_lane;

   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   pCfg  = &pDesc->cfg;
 	tsc   =  (tscmod_st *)( pDesc + 1);

   rv = SOC_E_NONE ;

   tmp_select           = tsc->lane_select ;
   tmp_lane             = tsc->this_lane   ;

   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      tsc->lane_select      = TSCMOD_LANE_BCST ;
   }
   tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

   tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

   tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

   tsc->per_lane_control = 0;
   tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

   tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
   tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

   tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
   tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

   if(tsc->ctrl_type &TSCMOD_CTRL_TYPE_UC_RXP) {
      tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
   }
   tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
   tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

   if(value >= 0xf) {
       value = 0 ;
       tsc->firmware_mode = -1 ;
   } else {
       tsc->firmware_mode = value ;
   }
   rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_FW_MODE) ;

   tsc->accData = value ;
   tsc->per_lane_control = TSCMOD_FWMODE_WR ;
   tscmod_tier1_selector("FWMODE_CONTROL", tsc, &rv);

   tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

   sal_usleep(pCfg->los_usec);

   tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

   sal_usleep(1000) ;
   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      tsc->lane_select      = TSCMOD_LANE_BCST ;
   }

   if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_LINK_DIS) {
      /* do nothing */
   } else {
      if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_TX_SQUELCH)) {
          tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
          
          tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }
      tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      tsc->per_lane_control = 1;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      tsc->per_lane_control = 1;
      tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

      if(tsc->ctrl_type &TSCMOD_CTRL_TYPE_UC_RXP) {
         tsc->per_lane_control = TSCMOD_RXP_UC_ON ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }
   }
   tsc->lane_select = tmp_select ; /* restore */
   tsc->this_lane   = tmp_lane ;

   return rv ;
}


STATIC int
_phy_tscmod_control_firmware_mode_get (int unit, phy_ctrl_t *pc, uint32 *value)
{

   TSCMOD_DEV_DESC_t *pDesc;
   tscmod_st    *tsc;
   int            rv ;

   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

   rv = SOC_E_NONE ;

   tsc->per_lane_control = TSCMOD_FWMODE_RD ;
   tscmod_tier1_selector("FWMODE_CONTROL", tsc, &rv);

   *value = tsc->accData ;
   return SOC_E_NONE ;
}


STATIC int
_phy_tscmod_tx_control_get(int unit, soc_port_t port, TSCMOD_TX_DRIVE_t *tx_drv, int inx)
{
   phy_ctrl_t     *pc;
   int rv = SOC_E_NONE;
   int size;
   int i;

   pc = INT_PHY_SW_STATE(unit, port);

   size = (SOC_INFO(unit).port_num_lanes[pc->port]);

   for (i = 0; i < size; i++) {
      if (DEV_CFG_PTR(pc)->idriver[i] == TSCMOD_NO_CFG_VALUE) {
         /* take from the SW default value table */
         (tx_drv + i)->idrive    = DEV_CFG_PTR(pc)->tx_drive[inx].idrive;
      } else {
         /* taking from user */
         (tx_drv + i)->idrive = DEV_CFG_PTR(pc)->idriver[i];
      }
      if (DEV_CFG_PTR(pc)->pdriver[i] == TSCMOD_NO_CFG_VALUE) {
         /* take from the SW default value table */
         (tx_drv + i)->ipredrive = DEV_CFG_PTR(pc)->tx_drive[inx].ipredrive;
      } else {
         /* taking from user */
         (tx_drv + i)->ipredrive = DEV_CFG_PTR(pc)->pdriver[i];
      }
      if (DEV_CFG_PTR(pc)->post2driver[i] == TSCMOD_NO_CFG_VALUE) {
         /* take from the SW default value table */
         (tx_drv + i)->post2 = DEV_CFG_PTR(pc)->tx_drive[inx].post2;
      } else {
         /* taking from user */
         (tx_drv + i)->post2 = DEV_CFG_PTR(pc)->post2driver[i];
      }

      if ((DEV_CFG_PTR(pc)->preemph[i] == TSCMOD_NO_CFG_VALUE)) {
         /* take from the SW default value table */
         (tx_drv + i)->u.preemph = TXDRV_PREEMPH(DEV_CFG_PTR(pc)->tx_drive[inx].u.tap);
      } else {
         /* taking from user */
         (tx_drv + i)->u.preemph = DEV_CFG_PTR(pc)->preemph[i];
      }

      /* TSCOP-002: sw_init_drive is true when u_kx/u_os/.../u_2p4 is configure-able */
      (tx_drv + i)->u_kx.preemph  = TXDRV_PREEMPH(DEV_CFG_PTR(pc)->tx_drive[inx].u_kx.tap);
      (tx_drv + i)->u_os.preemph  = TXDRV_PREEMPH(DEV_CFG_PTR(pc)->tx_drive[inx].u_os.tap);
      (tx_drv + i)->u_br.preemph  = TXDRV_PREEMPH(DEV_CFG_PTR(pc)->tx_drive[inx].u_br.tap);
      (tx_drv + i)->u_kr.preemph  = TXDRV_PREEMPH(DEV_CFG_PTR(pc)->tx_drive[inx].u_kr.tap);
      (tx_drv + i)->u_2p5.preemph = TXDRV_PREEMPH(DEV_CFG_PTR(pc)->tx_drive[inx].u_2p5.tap);
   }

   return rv;
}

STATIC int
_phy_tscmod_tx_control_set(int unit, soc_port_t port, TSCMOD_TX_DRIVE_t *tx_drv)
{
   phy_ctrl_t     *pc;
   tscmod_st    *tsc;
   uint32          preemph;
   uint32          idriver;
   uint32          pdriver;
   uint32          post2;
   TSCMOD_DEV_DESC_t     *pDesc;
   TSCMOD_DEV_CFG_t      *pCfg;
   int             size, i, mode ;

   pc = INT_PHY_SW_STATE(unit, port);

   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   pCfg  = &pDesc->cfg;
   tsc   =  (tscmod_st *)( pDesc + 1);
   size = (SOC_INFO(unit).port_num_lanes[pc->port]);

   mode = TSCMOD_TAP_REG ;
   for(i=0;i<size; i++){
      idriver = (tx_drv+i)->idrive;
      pdriver = (tx_drv+i)->ipredrive;
      post2   = (tx_drv+i)->post2;
      preemph = (tx_drv+i)->u.preemph;

      /* next set Tx driver, pdriver and post2 */
      /* use SW defualt AMP for now or not  */
      _phy_tscmod_per_lane_control_tx_driver_set
         (unit, port, i, SOC_PHY_CONTROL_DRIVER_CURRENT, idriver);
      _phy_tscmod_per_lane_control_tx_driver_set
         (unit, port, i, SOC_PHY_CONTROL_PRE_DRIVER_CURRENT,pdriver);
      _phy_tscmod_per_lane_control_tx_driver_set
         (unit, port, i, SOC_PHY_CONTROL_DRIVER_POST2_CURRENT,post2);

      if(tsc->an_type !=TSCMOD_AN_NONE) {  /* AN mode */
         if((pCfg->an_cl72==0)&&(pCfg->forced_init_cl72<=0)) {
            /* no CL72 */
            mode = TSCMOD_TAP_REG  ;
            if(pCfg->sw_init_drive) {
               mode = mode | TSCMOD_TAP_KX | TSCMOD_TAP_OS | TSCMOD_TAP_BR | TSCMOD_TAP_KR | TSCMOD_TAP_2P5  ;
            }
         } else {
            /* CL72 is on */
            if(preemph&CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK) {
               preemph =
                  CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK^preemph ;
            }
            mode = TSCMOD_TAP_REG  ;
            if(pCfg->sw_init_drive) {
               /* need more variables from SW table */
               mode = mode | TSCMOD_TAP_KX | TSCMOD_TAP_OS | TSCMOD_TAP_BR | TSCMOD_TAP_KR | TSCMOD_TAP_2P5  ;
            }
         }

      } else {   /* forced speed mode */
         if(FORCE_CL72_MODE(pc)) {
            if(preemph&CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK) {
               preemph =
                  CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK^preemph ;
            }
         }
         mode = TSCMOD_TAP_REG  ;
         if(pCfg->sw_init_drive) {
            mode = mode | TSCMOD_TAP_KX | TSCMOD_TAP_OS | TSCMOD_TAP_BR | TSCMOD_TAP_KR | TSCMOD_TAP_2P5  ;
         }
      }

      if(tsc->verbosity & TSCMOD_DBG_TXDRV ) {
         printf ("tx_control: u=%0d p=%0d l=%0d lane_num=%0d preemphasis=0x%x idriver=%0d pdriver=%0d"
                " post2=%0d sw_init_dr=%0d mode=%0x\n", unit, port, i, pc->lane_num, preemph, idriver, pdriver, post2, pCfg->sw_init_drive, mode);
      }

      if(mode & TSCMOD_TAP_REG) {
         _phy_tscmod_per_lane_control_preemphasis_set
            (unit,port,i,SOC_PHY_CONTROL_PREEMPHASIS, preemph, TSCMOD_TAP_REG);
      }
      if(mode & TSCMOD_TAP_KX) {
         preemph = (tx_drv+i)->u_kx.preemph;
         _phy_tscmod_per_lane_control_preemphasis_set
            (unit,port,i,SOC_PHY_CONTROL_PREEMPHASIS, preemph, TSCMOD_TAP_KX);
      }
      if(mode & TSCMOD_TAP_OS) {
         preemph = (tx_drv+i)->u_os.preemph;
         _phy_tscmod_per_lane_control_preemphasis_set
            (unit,port,i,SOC_PHY_CONTROL_PREEMPHASIS, preemph, TSCMOD_TAP_OS);
 }
      if(mode & TSCMOD_TAP_BR) {
         preemph = (tx_drv+i)->u_br.preemph;
         _phy_tscmod_per_lane_control_preemphasis_set
            (unit,port,i,SOC_PHY_CONTROL_PREEMPHASIS, preemph, TSCMOD_TAP_BR);
      }
      if(mode & TSCMOD_TAP_KR) {
         preemph = (tx_drv+i)->u_kr.preemph;
         _phy_tscmod_per_lane_control_preemphasis_set
            (unit,port,i,SOC_PHY_CONTROL_PREEMPHASIS, preemph, TSCMOD_TAP_KR);
      }
      if(mode & TSCMOD_TAP_2P5) {
         preemph = (tx_drv+i)->u_2p5.preemph;
         _phy_tscmod_per_lane_control_preemphasis_set
            (unit,port,i,SOC_PHY_CONTROL_PREEMPHASIS, preemph, TSCMOD_TAP_2P5);
      }

   }
   return SOC_E_NONE ;
}

STATIC int
_phy_tscmod_rxlos_timer_get (int unit, phy_ctrl_t *pc, int type, uint32 *value)
{
   if(type==SOC_PHY_CONTROL_SOFTWARE_RX_LOS_LINK_WAIT_TIMER_US) {
       if(DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_UP_FAST_WAR) {
           *value = (DEV_CFG_PTR(pc)->rxlos_link_wait_timer ==0) ? TSCMOD_RXLOS_TIMEOUTp1 : DEV_CFG_PTR(pc)->rxlos_link_wait_timer;
       } else {
           *value = (DEV_CFG_PTR(pc)->rxlos_link_wait_timer ==0) ? TSCMOD_RXLOS_TIMEOUTp25: DEV_CFG_PTR(pc)->rxlos_link_wait_timer;
       }
   } else {
       if(DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_UP_FAST_WAR) {
           *value = (DEV_CFG_PTR(pc)->rxlos_restart_timer ==0) ? TSCMOD_RXLOS_TIMEOUTp1 : DEV_CFG_PTR(pc)->rxlos_restart_timer;
       } else {
           *value = (DEV_CFG_PTR(pc)->rxlos_restart_timer ==0) ? TSCMOD_RXLOS_TIMEOUTp5 : DEV_CFG_PTR(pc)->rxlos_restart_timer;
       }
   }
   return SOC_E_NONE ;
}

STATIC int
_phy_tscmod_spd_selection(int unit, soc_port_t port, int speed, int *actual_spd, int *txdrv)
{
    int spd_intf, asp_id, rv;
   phy_ctrl_t        *pc ;
   TSCMOD_DEV_DESC_t *pDesc ;
   TSCMOD_DEV_CFG_t  *pCfg;
   tscmod_st         *tsc ;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;
   rv    = SOC_E_NONE ;

   if(tsc->verbosity & TSCMOD_DBG_SPD)
      printf ("%-22s: started u=%0d p=%0d speed=%0d aspd_vec=%x tsc_ptr=%p"
      " spd=%s(%0d) port_type=%0d iintf=%0x\n", FUNCTION_NAME(), unit, port, speed, *actual_spd,
      (void *)tsc, e2s_tscmod_spd_intfc_type[tsc->spd_intf], tsc->spd_intf,
             tsc->port_type, tsc->iintf);

   spd_intf = TSCMOD_SPD_100_SGMII ;
   asp_id   = TSCMOD_ASPD_1000M ;
   *txdrv   = TXDRV_6GOS2_INX ;
   /* find out what spd_intf to use by speed, mode, line_intf */
   /* return actual_spd vector, and update line_intf, spd_intf */
   if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_CL72_WAR_ON)
      tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_CL72_WAR_ON ;
   if((pCfg->forced_init_cl72>0)&&(pCfg->forced_init_cl72<16)) {
      if(pCfg->forced_init_cl72==0x2) FORCE_CL72_MODE(pc) = 0 ;
      else                            FORCE_CL72_MODE(pc) = 1 ;
   } else
      FORCE_CL72_MODE(pc) = 0 ;
   tsc->iintf = 0 ;
   switch(speed) {
   case 10:
      spd_intf = TSCMOD_SPD_10_SGMII ;
      asp_id   = TSCMOD_ASPD_10M ;
      break ;
   case 100:
      spd_intf = TSCMOD_SPD_100_SGMII ;
      asp_id   = TSCMOD_ASPD_100M ;
      break ;
   case 1000:
      /* if (PHY_FIBER_MODE(unit, port))   {
         spd_intf = TSCMOD_SPD_1000_FIBER ;
         asp_id   = TSCMOD_ASPD_1000M ;
         } else {  } */
      spd_intf = TSCMOD_SPD_1000_SGMII ;
      asp_id   = TSCMOD_ASPD_1000M ;
      break ;
   case 2500:
      spd_intf = TSCMOD_SPD_2500 ;
      asp_id   = TSCMOD_ASPD_2p5G_X1 ;
      break ;
   case 5000:
      spd_intf = TSCMOD_SPD_5000 ;
      asp_id   = TSCMOD_ASPD_5G_X1 ;
      break ;
   case 10000:
      if((tsc->port_type==TSCMOD_MULTI_PORT)||
         ((tsc->port_type==TSCMOD_SINGLE_PORT)&&(pCfg->cx4_10g==0))||
         (DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_KR)||
         (DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_SR) ||
         (DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_LR) ||
         (DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_CR) ||
         (DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_SFI) ||
         (DEV_CFG_PTR(pc)->line_intf& TSCMOD_IF_XFI) ||
         (DEV_CFG_PTR(pc)->pcs_mode ==0)) {
          spd_intf = TSCMOD_SPD_10000_XFI ;
          *txdrv   = TXDRV_XFI_INX ;
          
          if((DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_KR)||(DEV_CFG_PTR(pc)->pcs_mode ==0)) {
              asp_id   = TSCMOD_ASPD_10G_KR1 ;
              *txdrv   = TXDRV_XLAUI_INX ;
          } else if ((DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_SFI) ||
                     (DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_SR) ||
                     (DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_LR) ||
                     (DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_CR)) {
              asp_id     = TSCMOD_ASPD_10G_SFI ;
              *txdrv     = TXDRV_SFI_INX ;
              if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_CR) {
                  tsc->iintf = TSCMOD_IIF_SFPDAC ;
                  *txdrv     = TXDRV_SFIDAC_INX ;
              } else if(DEV_CFG_PTR(pc)->medium == SOC_PORT_MEDIUM_COPPER) {
                  tsc->iintf = TSCMOD_IIF_SFPDAC ;
                  *txdrv     = TXDRV_SFIDAC_INX ;
              }
          } else {
              asp_id   = TSCMOD_ASPD_10G_X1 ;   /* XFI */
              FORCE_CL72_MODE(pc) = 0 ;
          }
      } else {
          if((tsc->port_type   == TSCMOD_SINGLE_PORT)&&(pCfg->cx4_10g)) {
              if(DEV_CFG_PTR(pc)->hg_mode) {
                  spd_intf = TSCMOD_SPD_10000 ;  /* HG 10G XAUI */
                  asp_id   = TSCMOD_ASPD_10G_X4 ;
              } else {
                  spd_intf = TSCMOD_SPD_10000 ;  /* 10G XAUI */
                  asp_id   = TSCMOD_ASPD_10G_CX4 ;
              }
          } else if ( tsc->port_type == TSCMOD_DXGXS ) {
              spd_intf = TSCMOD_SPD_10000_DXGXS ;
              asp_id   = TSCMOD_ASPD_10G_CX2_NOSCRAMBLE ;
          } else {
               /* no reachable */
          }
      }
      break ;
   case 10500:
      spd_intf = TSCMOD_SPD_10000_HI ;  /* HG 10G XAUI */
      asp_id   = TSCMOD_ASPD_10G_X4 ;
      break ;
   case 11000:
      if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         if(DEV_CFG_PTR(pc)->hg_mode) {
            spd_intf = TSCMOD_SPD_10000_HI ;  /* HG 10G XAUI at HG port */
            asp_id   = TSCMOD_ASPD_10G_X4 ;
         } else {
            spd_intf = TSCMOD_SPD_10000_HI ;  /* HG 10G XAUI at XE port */
            asp_id   = TSCMOD_ASPD_10G_CX4 ;
         }
      } else {
         spd_intf = TSCMOD_SPD_10600_XFI_HG ;
         *txdrv   = TXDRV_XFI_INX ;

         if((DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_KR)||(DEV_CFG_PTR(pc)->pcs_mode ==0)) {
             asp_id   = TSCMOD_ASPD_10G_KR1 ;
             *txdrv   = TXDRV_XLAUI_INX ;
         } else if ((DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_SFI) ||
                    (DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_SR) ||
                    (DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_CR)) {
             asp_id     = TSCMOD_ASPD_10G_SFI ;
             *txdrv     = TXDRV_SFI_INX ;
             if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_CR) {
                 tsc->iintf = TSCMOD_IIF_SFPDAC ;
                 *txdrv     = TXDRV_SFIDAC_INX ;
             } else if(DEV_CFG_PTR(pc)->medium == SOC_PORT_MEDIUM_COPPER) {
                 tsc->iintf = TSCMOD_IIF_SFPDAC ;
                 *txdrv     = TXDRV_SFIDAC_INX ;
             }
         } else {
             asp_id   = TSCMOD_ASPD_10G_X1 ;  /* XFI */
             FORCE_CL72_MODE(pc) = 0 ;
         }
      }
      break ;
   case 12700:
   case 13000:
      if(tsc->port_type   == TSCMOD_SINGLE_PORT) {
         spd_intf = TSCMOD_SPD_13000 ;
         asp_id   = TSCMOD_ASPD_13G_X4 ;
      } else if ( tsc->port_type == TSCMOD_DXGXS ) {
            spd_intf = TSCMOD_SPD_12773_DXGXS ;
            asp_id   = TSCMOD_ASPD_12p7G_X2 ;
      }
      /* spd_intf = TSCMOD_SPD_12773_HI_DXGXS ; not needed */
      break ;
   case 15000:
      spd_intf = TSCMOD_SPD_15000 ;
      asp_id   = TSCMOD_ASPD_15G_X4 ;
      break ;
   case 16000:
      spd_intf = TSCMOD_SPD_16000 ;
      asp_id   = TSCMOD_ASPD_16G_X4 ;
      break ;
   case 20000:
      if((DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_CR)||
         (DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_CR2)) {
          spd_intf = TSCMOD_SPD_20G_MLD_DXGXS ;
          asp_id   = TSCMOD_ASPD_20G_CR2  ;
          
      } else if((DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_KR)||
                (DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_KR2)
                ) {
          spd_intf = TSCMOD_SPD_20G_MLD_DXGXS ;
          asp_id   = TSCMOD_ASPD_20G_KR2  ;
      } else {
         if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
             if(DEV_CFG_PTR(pc)->scrambler_en) {
                 spd_intf = TSCMOD_SPD_20000_SCR ;
                 asp_id   = TSCMOD_ASPD_20G_X4 ;
             } else {
                 spd_intf = TSCMOD_SPD_20000 ;
              asp_id   = TSCMOD_ASPD_20G_CX4 ;
             }
         } else {
             spd_intf = TSCMOD_SPD_20G_DXGXS ;  /* HG */
             asp_id   = TSCMOD_ASPD_20G_CX2  ;
         }
      }
      break ;
   case 21000:
      if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         spd_intf = TSCMOD_SPD_21000 ;
         asp_id   = TSCMOD_ASPD_21G_X4 ;
      } else {
         spd_intf = TSCMOD_SPD_21G_HI_DXGXS ;  /* HG */
         asp_id   = TSCMOD_ASPD_20G_CX2  ;
      }
      break ;
   case 25000:
      spd_intf = TSCMOD_SPD_25455  ;
      asp_id   = TSCMOD_ASPD_25p45G_X4 ;
      break ;
   case 30000:
      spd_intf = TSCMOD_SPD_31500; 
      asp_id   = TSCMOD_ASPD_31p5G_X4;
      break;
   case 32000:
      spd_intf = TSCMOD_SPD_31500; 
      asp_id   = TSCMOD_ASPD_31p5G_X4;
      break;
   case 40000:
      *txdrv   = TXDRV_XLAUI_INX ;
      if(DEV_CFG_PTR(pc)->hg_mode) {
          if((DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_KR4)||(pCfg->pcs_mode==0)) {
            spd_intf = TSCMOD_SPD_40G_MLD ;   /* HG 40G MLD */
            asp_id   = TSCMOD_ASPD_40G_KR4 ;
         } else {
            spd_intf = TSCMOD_SPD_40G_X4 ;
            asp_id   = TSCMOD_ASPD_40G_X4 ;
         }
      } else {
         if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_CR4) {
            spd_intf = TSCMOD_SPD_40G_MLD ;
            asp_id   = TSCMOD_ASPD_40G_CR4 ;
         } else {
            if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_XLAUI)
               tsc->iintf = TSCMOD_IIF_XLAUI ;
            else if((DEV_CFG_PTR(pc)->line_intf & (TSCMOD_IF_SR4|TSCMOD_IF_SR)) ||
                    (DEV_CFG_PTR(pc)->line_intf & (TSCMOD_IF_LR4|TSCMOD_IF_LR))) {
               tsc->iintf = TSCMOD_IIF_SR4 ;
	       *txdrv     = TXDRV_SR4_INX  ;
	    }

            spd_intf = TSCMOD_SPD_40G_MLD ;
            asp_id   = TSCMOD_ASPD_40G_KR4 ;
         }
      }
      break ;
   case 42000:
      *txdrv   = TXDRV_XLAUI_INX ;
      if(DEV_CFG_PTR(pc)->hg_mode) {
         if((DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_KR4)||(pCfg->pcs_mode==0)) {
            spd_intf = TSCMOD_SPD_42G_MLD ;   /* HG 42G MLD */
            asp_id   = TSCMOD_ASPD_40G_KR4 ;
         } else {
            spd_intf = TSCMOD_SPD_42G_X4 ;
            asp_id   = TSCMOD_ASPD_40G_X4 ;
            if((pCfg->forced_init_cl72==0)||(pCfg->forced_init_cl72>=16))
               FORCE_CL72_MODE(pc) = 0 ;
            else
               FORCE_CL72_MODE(pc) = 1 ;
         }
      } else {
         if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_CR4) {
            spd_intf = TSCMOD_SPD_42G_MLD ;
            asp_id   = TSCMOD_ASPD_40G_CR4 ;
         } else {
            if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_XLAUI)
               tsc->iintf = TSCMOD_IIF_XLAUI ;
            else if((DEV_CFG_PTR(pc)->line_intf & (TSCMOD_IF_SR4|TSCMOD_IF_SR)) ||
                    (DEV_CFG_PTR(pc)->line_intf & (TSCMOD_IF_LR4|TSCMOD_IF_LR))) {
               tsc->iintf = TSCMOD_IIF_SR4 ;
               *txdrv     = TXDRV_SR4_INX  ;
            }

            spd_intf = TSCMOD_SPD_42G_MLD ;
            asp_id   = TSCMOD_ASPD_40G_KR4 ;
         }
      }
      break ;
   default:
      if(tsc->verbosity & TSCMOD_DBG_SPD)
         printf ("%-22s: Error: u=%0d p=%0d speed=%0d not found\n", FUNCTION_NAME(),
                unit, port, speed) ;
      return SOC_E_NONE ;
      break ;
   }
   if(FORCE_CL72_MODE(pc)){
       if((TSCMOD_MODEL_REVID_GET(tsc)<TSCMOD_MODEL_REVID_A2)&&
          (TSCMOD_MODEL_PROJ_GET(tsc)!=TSCMOD_MODEL_PROJ_HR2)) {
           FORCE_CL72_MODE(pc) = 0 ;
       }
   }

   if(FORCE_CL72_MODE(pc))
      if(pCfg->forced_init_cl72 !=0)
         tsc->ctrl_type |= TSCMOD_CTRL_TYPE_CL72_WAR_ON ;
   
   rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_CTRL_TYPE) ;

   *actual_spd   = e2n_tscmod_aspd_type[asp_id] ;
   tsc->spd_intf = spd_intf ;

   if(tsc->verbosity & TSCMOD_DBG_SPD)
      printf("%-22s: u=%0d p=%0d speed=%0d aspd_vec=0x%x(%s) spd=d%d(%s) intf=%0x iintf=%0x\n", FUNCTION_NAME(),
             unit, port, speed, *actual_spd, e2s_tscmod_aspd_type[asp_id],
             spd_intf, e2s_tscmod_spd_intfc_type[spd_intf], DEV_CFG_PTR(pc)->line_intf, tsc->iintf);

   return rv ;
}


/* If FW is not loaded, we could start from scratch */
STATIC int
tscmod_init_state_set(int unit, soc_port_t port)
{
   phy_ctrl_t        *pc ;
   TSCMOD_DEV_CFG_t  *pCfg;
   TSCMOD_DEV_DESC_t *pDesc ;
   tscmod_st         *tsc;
   uint16    data_a, data_b ;
   int rv ;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;

   rv = SOC_E_NONE ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_UC_COMMANDr(tsc->unit, tsc, &data_a));
   if(data_a&UC_COMMAND_MDIO_UC_RESET_N_MASK) data_a = 1 ; else data_a = 0 ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_UC_DOWNLOAD_STATUSr(tsc->unit, tsc, &data_b)) ;
   if(data_b&UC_DOWNLOAD_STATUS_INIT_DONE_MASK) data_b=1 ; else data_b = 0 ;

   if(data_a && data_b){
      tsc->ctrl_type |= TSCMOD_CTRL_TYPE_FW_LOADED ;
      pCfg->load_ucode_done   = 1 ;

      tsc->per_lane_control =  TSCMOD_UC_R_CKSUM ;
      tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);
      pCfg->uc_crc     = tsc->accData ;

      tsc->per_lane_control =  TSCMOD_UC_R_VER ;
      tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);
      pCfg->uc_ver     = tsc->accData ;

      /* crc 0x47a7 & ver=0 means version a030 */
      if((pCfg->uc_ver==0x0)&&(pCfg->uc_crc==0x47a7))
         pCfg->uc_ver=0xa030;

      if((tsc->verbosity & TSCMOD_DBG_UC)&&
         (pCfg->uc_ver != tscmod_ucode_TSC_revA0_ver))
         printf ("Warning: u=%0d p=%0d uC ver mismatch %x vs expected %x\n",
                unit, port, pCfg->uc_ver, tscmod_ucode_TSC_revA0_ver);

      if((tsc->verbosity & TSCMOD_DBG_UC)&&
         (pCfg->uc_crc != tscmod_ucode_TSC_revA0_crc))
         printf ("Warning: u=%0d p=%0d uC crc mismatch %x vs expected %x\n",
                unit, port, pCfg->uc_crc, tscmod_ucode_TSC_revA0_crc);

      tscmod_uc_cap_set(tsc, pCfg, TSCMOD_UC_CAP_ALL, pCfg->uc_ver) ;
      if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_UC_CAP_RXP)
         tsc->ctrl_type |= TSCMOD_CTRL_TYPE_UC_RXP ;

      /* align AER and blk_adr */
      tsc->ctrl_type |= TSCMOD_CTRL_TYPE_WB_DIS ;
      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY) {
         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }
      if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_WB_DIS)
         tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_WB_DIS ;

      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR) {
         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY_BLK_ADR ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }

      tsc->diag_type = TSCMOD_DIAG_PLL;
      tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
      tsc->plldiv          = tsc->accData ;

      

      /* clean LB, RMT_LB, PRBS, CL72 */

      /* pc->flags |= PHYCTRL_INIT_DONE; */
   }
   if(tsc->verbosity & (TSCMOD_DBG_FLEX|TSCMOD_DBG_INIT))
      printf ("%-22s: u=%0d p=%0d reset_n=%0d init_done=%0d ctrl=%x\n",
             FUNCTION_NAME(), tsc->unit, tsc->port, data_a, data_b, tsc->ctrl_type) ;

   return rv ;
}

STATIC int
_phy_tscmod_port_init_start(int unit, soc_port_t port)
{
    phy_ctrl_t        *pc, *pc2;
    TSCMOD_DEV_CFG_t  *pCfg;
    TSCMOD_DEV_DESC_t *pDesc, *pDesc2 ;
    tscmod_st         *tsc, *tsc2 ;

    int               rv, tmp_select, tmp_lane, tmp_dxgxs;
    int               actual_spd_vec ;
    int               txdrv ;
    int               i, tx_swap, rx_swap ;
    int               speed , port_type_mode ;
    int               init_mode ;
    int               speed_max = 0;
    uint32            uc_info ;
    int unit0; soc_port_t port0 ;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);
    pCfg  = &pDesc->cfg;

    speed = 0 ; actual_spd_vec =0 ;  init_mode = 0 ;  uc_info = 0 ;
    unit0 = unit ; port0 = port ;

    tmp_select           = tsc->lane_select ;
    tmp_lane             = tsc->this_lane   ;
    tmp_dxgxs            = tsc->dxgxs ;

    tscmod_init_state_set(unit, port) ;

    port_type_mode = tsc->port_type;

    if (!_tscmod_chip_init_done(unit,pc->chip_num,pc->phy_mode, &init_mode, &uc_info)) {
       if(tsc->verbosity & (TSCMOD_DBG_INIT|TSCMOD_DBG_LINK) )
          printf ("%s u=%d p=%d init started \n", FUNCTION_NAME(), unit, port) ;
    } else {
       if(tsc->verbosity & (TSCMOD_DBG_INIT|TSCMOD_DBG_LINK) )
          printf ("%s u=%0d p=%0d returned due to prior init_mode=%x uc_info=%x\n",
                 FUNCTION_NAME(), unit, port, init_mode, uc_info) ;

       DEV_CFG_PTR(pc)->init_mode = (DEV_CFG_PTR(pc)->init_mode|TSCMOD_INIT_MODE_SKIP) ;

       if(init_mode & TSCMOD_CTRL_TYPE_FW_LOADED ) {
          DEV_CFG_PTR(pc)->load_ucode_done   = 1 ;  /* no need to load uCode */
          tsc->ctrl_type = tsc->ctrl_type | TSCMOD_CTRL_TYPE_FW_LOADED ;
          pCfg->uc_ver = ((uc_info)>>16) & 0xffff ;
          pCfg->uc_crc = uc_info & 0xffff ;

          tscmod_uc_cap_set(tsc, pCfg, TSCMOD_UC_CAP_ALL, pCfg->uc_ver) ;
          if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_UC_CAP_RXP )
             tsc->ctrl_type |= TSCMOD_CTRL_TYPE_UC_RXP ;

          if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY) {
             tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY ;
             tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
          }
       }

       if((tsc->an_type != TSCMOD_AN_TYPE_ILLEGAL)&&(tsc->an_type != TSCMOD_AN_NONE)) {
          if(!_tscmod_chip_an_init_done(unit,pc->chip_num)) {
             if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
                tsc->lane_select      = TSCMOD_LANE_BCST ;
             }
             tsc->per_lane_control = TSCMOD_CTL_AN_MODE_INIT ;  /* not a complete chip init for AN */
             tscmod_tier1_selector("SET_AN_PORT_MODE", tsc, &rv);
          }
          pc->flags                   |= PHYCTRL_ANEG_INIT_DONE ;
       }
       pc->flags                   |= PHYCTRL_INIT_DONE;

       /* get the plldiv from register */
       tsc->diag_type = TSCMOD_DIAG_PLL;
       tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
       tsc->plldiv = tsc->accData ;

       tsc->lane_select      = tmp_select ;  /* restore */
       tsc->this_lane        = tmp_lane ;

       return SOC_E_NONE ;
    }

    tsc->lane_select     = TSCMOD_LANE_BCST ;

    /* lane swap */
    if(tsc->lane_swap) {  /* odd core */
       tx_swap = 0 ;
       rx_swap = 0 ;
       for(i=0; i<4; i++) {
          tx_swap = tx_swap | ((3-((pCfg->txlane_map[0]>>(4*i)) & 0xf)) << (4*i)) ;
          rx_swap = rx_swap | (((pCfg->rxlane_map[0]>>(4*(3-i))) & 0xf) << (4*i)) ;
       }
    } else {
       tx_swap = pCfg->txlane_map[0] ;
       rx_swap = pCfg->rxlane_map[0] ;
    }

    if(tsc->verbosity & TSCMOD_DBG_PATH) {
       printf ("%-22s: u=%0d p=%0d swap cfg_tx=%x rx=%x reg_tx=%x rx=%x odd=%0d\n",
              FUNCTION_NAME(), unit, port, pCfg->txlane_map[0], pCfg->rxlane_map[0],
             tx_swap, rx_swap, tsc->lane_swap);
    }

    tsc->per_lane_control = rx_swap << 16 | tx_swap ;
    tscmod_tier1_selector("LANE_SWAP", tsc, &rv);

    /******************************/
    /* bring up seq ***************/
    /******************************/
    tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
    tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

    tsc->per_lane_control = TSCMOD_MISC_CTL_ENABLE_PLL ;
    tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);

    tsc->per_lane_control = TSCMOD_MISC_CTL_ENABLE_PWR_TIMER ;
    tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);

    /* enabling all 4 lanes (not necessary due to the HW deafult setting */
    tsc->per_lane_control = 0xf0f0;  /*  4 bits for 4 lanes */
    tscmod_tier1_selector("POWER_CONTROL", tsc, &rv);

    sal_usleep(1000) ;
    tsc->lane_select     = TSCMOD_LANE_BCST ;

    /* set rxSeqStart_AN_disable=1 */
    tsc->per_lane_control = 1;
    tscmod_tier1_selector("AFE_RXSEQ_START_CONTROL", tsc, &rv);

    tsc->per_lane_control = TSCMOD_MISC_CTL_TEMP_DEFAULT ;
    tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);

    tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_ANALOG ;
    tscmod_tier1_selector("SOFT_RESET", tsc, &rv);


    /* DSC SM reset */
    tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM;
    tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

    sal_usleep(1000) ;

    tsc->lane_select      = getLaneSelect(tmp_lane) ;
    tsc->dxgxs            = 0;
    tsc->per_lane_control = 0;
    tscmod_tier1_selector("PLL_SEQUENCER_CONTROL", tsc, &rv);

    tsc->lane_select     = TSCMOD_LANE_BCST ;

    tsc->per_lane_control = TSCMOD_MISC_CTL_CLK_SEL_156 ;
    tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);

    sal_usleep(2000) ;
    tsc->lane_select     = TSCMOD_LANE_BCST ;

    tsc->per_lane_control = TSCMOD_MISC_CTL_LFCK_BYPASS_0 ;
    tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);

    /* set the TSC to SW default values for some registers */
    tsc->per_lane_control = 1 ;
    tscmod_tier1_selector("CORE_RESET", tsc, &rv);

    /* empty credit table for TSC A1 and non-HR2 */
    if((TSCMOD_MODEL_REVID_GET(tsc)<=TSCMOD_MODEL_REVID_A1) &&
       (TSCMOD_MODEL_PROJ_GET(tsc)!=TSCMOD_MODEL_PROJ_HR2)){
       tsc->per_lane_control = (TSCMOD_RAM_BASE_ADDR)|(tscmod_ucode_TSC_revA0_ect_addr<<TSCMOD_COLD_RESET_MODE_SHIFT);
    } else {
       tsc->per_lane_control = (TSCMOD_RAM_BASE_ADDR)|(tscmod_ucode_TSC_revA0_cct_addr<<TSCMOD_COLD_RESET_MODE_SHIFT);
    }
    tscmod_tier1_selector("CORE_RESET", tsc, &rv);

    if(pCfg->an_link_fail_nocl72_wait_timer) {
       tsc->per_lane_control = pCfg->an_link_fail_nocl72_wait_timer ;
       tsc->per_lane_control = (tsc->per_lane_control<<TSCMOD_COLD_RESET_MODE_SHIFT)| TSCMOD_AN_CTL_LINK_FAIL_NOCL72_WAIT_TIMER ;
       tscmod_tier1_selector("CORE_RESET", tsc, &rv);
    }
    if(pCfg->pll_vco_step) {
       tsc->per_lane_control = (pCfg->pll_vco_step<<TSCMOD_COLD_RESET_MODE_SHIFT)|TSCMOD_PLL_VCO_STEP ;
       tscmod_tier1_selector("CORE_RESET", tsc, &rv);
    }

    _tscmod_init_tx_driver_regs(unit, port) ;

    if((tsc->an_type != TSCMOD_AN_TYPE_ILLEGAL)&&(tsc->an_type != TSCMOD_AN_NONE)) {
       tsc->per_lane_control = TSCMOD_CTL_AN_CHIP_INIT ;
       tscmod_tier1_selector("SET_AN_PORT_MODE", tsc, &rv);
       pc->flags                   |= PHYCTRL_ANEG_INIT_DONE ;

       unit0 = unit ; port0 = port ;
       PBMP_ALL_ITER(unit0, port0) {
          pc2 = INT_PHY_SW_STATE(unit0, port0);
          if(tsc->verbosity&TSCMOD_DBG_INIT)
             printf ("%-22s PBMP_ALL_ITER u=%0d p=%0d AN scan\n", FUNCTION_NAME(), unit0, port0) ;
          if (pc2 == NULL) {
             continue;
          }
          if (!pc2->dev_name || pc2->dev_name != tscmod_device_name) {
             continue;
          }
          if (pc2->chip_num != pc->chip_num) {
             continue;
          }
          if ((pc2->phy_mode == pc->phy_mode)) {
             if(tsc->verbosity&TSCMOD_DBG_INIT)
                printf ("%-22s PBMP_ALL_ITER u=%0d p=%0d AN scan corrected tsc->plldiv=%0d\n",
                       FUNCTION_NAME(), unit0, port0, tsc->plldiv) ;
             pDesc2 = (TSCMOD_DEV_DESC_t *)(pc2 + 1);
             tsc2   = (tscmod_st *)( pDesc2 + 1);
             tsc2->plldiv = tsc->plldiv ;
          }
       }

       /* for sim
          tsc->per_lane_control = 1;
          tscmod_tier1_selector("AFE_SPEED_UP_DSC_VGA", tsc, &rv);
       */
    }
    else {   /* forced speed/no AN */
       speed_max = ((pc->speed_max == 40000) && DEV_CFG_PTR(pc)->hg_mode) ? 42000 : pc->speed_max;
       if( pCfg->init_speed <= speed_max ) {
          speed = pCfg->init_speed ;
       } else {
          speed = pc->speed_max ;
       }
       _phy_tscmod_spd_selection(unit, port, speed, &actual_spd_vec, &txdrv) ;

       if(speed<2500) {
          if(tsc->port_type != TSCMOD_MULTI_PORT) {
             tsc->port_type = TSCMOD_MULTI_PORT ;
          }
       }
       /* test the water */
       tsc->per_lane_control = 0;
       tscmod_tier1_selector("SET_SPD_INTF", tsc, &rv);
       if(speed<2500) {
          tsc->port_type = port_type_mode ;
       }

       if( 1 ) {  /* PHY-887: During init, always set_port_mode, regardless tsc->plldiv is modified or not */
                  /*          tsc->plldiv still serves as user input */
          if(tsc->accData)
             tsc->plldiv = tsc->accData ;

          /* every port in the same core has the same plldiv */
          unit0 = unit ; port0 = port ;
          PBMP_ALL_ITER(unit0, port0) {
             pc2 = INT_PHY_SW_STATE(unit0, port0);
             if(tsc->verbosity&TSCMOD_DBG_INIT)
                printf ("%-22s PBMP_ALL_ITER u=%0d p=%0d FS scan unit=%0d port=%0d\n", FUNCTION_NAME(), unit, port, unit0, port0) ;
             if (pc2 == NULL) {
                continue;
             }
             if (!pc2->dev_name || pc2->dev_name != tscmod_device_name) {
                continue;
             }
             if (pc2->chip_num != pc->chip_num) {
                continue;
             }
             if ((pc2->phy_mode == pc->phy_mode)) {
                if(tsc->verbosity&TSCMOD_DBG_INIT)
                   printf ("%-22s PBMP_ALL_ITER u=%0d p=%0d FS  scan u=%0d p=%0d corrected tsc->plldiv=%0d\n",
                       FUNCTION_NAME(), unit, port, unit0, port0, tsc->plldiv) ;
                pDesc2 = (TSCMOD_DEV_DESC_t *)(pc2 + 1);
                tsc2   = (tscmod_st *)( pDesc2 + 1);
                tsc2->plldiv = tsc->plldiv ;
             }
          }
       }
       /* always call this func when reaching here */
       tscmod_tier1_selector("SET_PORT_MODE", tsc, &rv);

       sal_usleep(1000);

       tsc->per_lane_control = TSCMOD_MISC_CTL_PORT_CLEAN_UP ;
       tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);
    }

    tsc->lane_select      = getLaneSelect(tmp_lane) ;
    tsc->dxgxs            = 0;
    tsc->per_lane_control = 1;
    tscmod_tier1_selector("PLL_SEQUENCER_CONTROL", tsc, &rv);

    /* wait for pll lock */
    tscmod_tier1_selector("PLL_LOCK_WAIT", tsc, &rv);

    pc->flags |= PHYCTRL_INIT_DONE;

    tsc->lane_select      = tmp_select ;  /* restore */
    tsc->dxgxs            = tmp_dxgxs ;
    tsc->this_lane        = tmp_lane ;

    return  rv ;
}

STATIC int
_phy_tscmod_port_init_pll_lock_wait(int unit, soc_port_t port)
{
    phy_ctrl_t        *pc ;
    TSCMOD_DEV_DESC_t *pDesc ;
    tscmod_st         *tsc ;
    TSCMOD_DEV_CFG_t  *pCfg;
    int               rv, tmp_select, tmp_lane, tmp_dxgxs;
    soc_port_ability_t  ability;

    int               actual_spd_vec, plldiv;
    int               txdrv_inx ;
    int               speed_max = 0;
    TSCMOD_TX_DRIVE_t tx_drv[NUM_LANES];
    int               speed , port_type_mode ;
    uint16            uc_ver, uc_crc ;
    uint16            data ;

    speed  = 11000;
    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);
    pCfg  = &pDesc->cfg;


    tmp_select           = tsc->lane_select ;
    tmp_lane             = tsc->this_lane   ;
    tmp_dxgxs            = tsc->dxgxs       ;
    uc_ver = 0 ;
    uc_crc = 0 ;
    actual_spd_vec    = 0 ;
    data   = 0 ;
    plldiv = 0 ;
    port_type_mode       = tsc->port_type;

    /* wait for a long time */
    tsc->lane_select      = TSCMOD_LANE_0_0_0_1 ;
    tscmod_tier1_selector("PLL_LOCK_WAIT", tsc, &rv);

    /* on lane 0 for 10G KX4 */
    if(pCfg->pdetect10g)   {
        data = TSCMOD_PDET_CONTROL_10G | TSCMOD_PDET_CONTROL_MASK |
               TSCMOD_PDET_CONTROL_CMD_MASK;
    }
    tsc->per_lane_control = data ;
    tscmod_tier1_selector("PARALLEL_DETECT_CONTROL", tsc, &rv);

    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
       tsc->lane_select      = TSCMOD_LANE_BCST ;
       tsc->per_lane_control = TSCMOD_LANE_BCST ;
    } else if (tsc->port_type   ==  TSCMOD_DXGXS) {
       tsc->lane_select      = tmp_select ;
       tsc->per_lane_control = tmp_select ;
    } else {
       tsc->lane_select      = tmp_select ;
       tsc->per_lane_control = tmp_select ;
    }

    /* 100FX */

    /* polarity */
    _phy_tscmod_rx_polarity_set(unit, pc, pCfg->rxpol) ;
    _phy_tscmod_tx_polarity_set(unit, pc, pCfg->txpol) ;

    tsc->per_lane_control = 0;
    tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

    tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
    tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

    tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
    tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

    if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
       tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
       tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
    }
    tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
    tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

    SOC_IF_ERROR_RETURN
        (_phy_tscmod_ability_local_get(unit, port, &ability));

    SOC_IF_ERROR_RETURN
        (_phy_tscmod_ability_advert_set(unit, port, &ability));


    if((tsc->an_type != TSCMOD_AN_TYPE_ILLEGAL)&&
       (tsc->an_type != TSCMOD_AN_NONE)) {

       tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_FIRMWARE_MODE ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

       /* set rxSeqStart_AN_disable=0 */
       tsc->per_lane_control = 0;
       tscmod_tier1_selector("AFE_RXSEQ_START_CONTROL", tsc, &rv);

       tsc->lane_select      = getLaneSelect(tmp_lane) ;
       tsc->this_lane        = tmp_lane ;
       tsc->dxgxs            = 0 ;

       tscmod_tier1_selector("AUTONEG_CONTROL", tsc, &rv);

       SOC_IF_ERROR_RETURN
           (_phy_tscmod_an_set(unit, port, TRUE)) ;

       tsc->dxgxs           = tmp_dxgxs ;
       tsc->lane_select     = tmp_select ;
       tsc->this_lane       = tmp_lane ;
       if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
          tsc->lane_select      = TSCMOD_LANE_BCST ;
       }

       tscmod_an_cl72_control_set(unit, pc) ;

       /* TSCOP-005: revisit */
       tsc->per_lane_control = 1;
       tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

       tsc->per_lane_control = 1;
       tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

       /* TSCOP-005: revisit */
       tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_ANALOG ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

       /* TSCOP-005: revisit */
       tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

       if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
          tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
          tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
       }
       tsc->per_lane_control = TSCMOD_RXP_REG_ON ;
       tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

       tsc->lane_select      = tmp_select ;   /* restore */
       tsc->this_lane        = tmp_lane ;

       return rv ;
    }

    if( pCfg->init_mode & TSCMOD_INIT_MODE_SKIP ) {
       tsc->diag_type = TSCMOD_DIAG_PLL;
       tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
       plldiv = tsc->accData ;
       if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
          switch(plldiv) {
          case 70: speed = 42000 ; break ;
          case 66: speed = 40000 ; break ;
          default: plldiv =0 ;
          }
       } else if (tsc->port_type   ==  TSCMOD_DXGXS) {
          switch(plldiv) {
          case 70: speed = 21000 ; break ;
          case 66: speed = 20000 ; break ;
          default: plldiv =0 ;
          }
       } else {
          switch(plldiv) {
          case 70: speed = 11000 ; break ;
          case 66: speed = 10000 ; break ;
          default: plldiv =0 ;
          }
       }
       if(plldiv==0) {
          speed = pc->speed_max ;
          plldiv= tsc->accData ;
       }
    } else {
       speed_max = ((pc->speed_max == 40000) && DEV_CFG_PTR(pc)->hg_mode) ? 42000 : pc->speed_max;
       if( pCfg->init_speed <= speed_max ) {
            speed = pCfg->init_speed ;
       } else {
            speed = pc->speed_max ;
       }
    }
    _phy_tscmod_spd_selection(unit, port, speed, &actual_spd_vec, &txdrv_inx) ;

    if(tsc->verbosity&TSCMOD_DBG_SPD)
       printf ("%-22s: u=%0d p=%0d actual_spd_vec=%0x sp=%0d speed_max=%0d plldiv=%0d init_sp=%0d init_mode=%0x\n",
              FUNCTION_NAME(), tsc->unit, tsc->port,actual_spd_vec, speed, pc->speed_max, plldiv, pCfg->init_speed, pCfg->init_mode) ;

    if(speed<2500) {
      if(tsc->port_type != TSCMOD_MULTI_PORT) {
         tsc->port_type = TSCMOD_MULTI_PORT ;
      }
    }
    tsc->per_lane_control = 1 | (actual_spd_vec <<4) ;
    tscmod_tier1_selector("SET_SPD_INTF", tsc, &rv);

    if((pCfg->am_reserved)&&(tsc->spd_intf!=TSCMOD_SPD_10000_XFI)) {
       if(tsc->spd_intf==TSCMOD_SPD_20G_MLD_DXGXS) {
           tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_HP_BAM_AM ;
           tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
       } else {
           tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_HP_BAM_AM ;
           tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
       }
    }

    if((tsc->spd_intf==TSCMOD_SPD_10_SGMII)||
       (tsc->spd_intf==TSCMOD_SPD_100_SGMII)) {
        tsc->per_lane_control = 1;
        tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

        tsc->per_lane_control = 1 ;
        tscmod_tier1_selector("ENCODE_SET", tsc, &rv);
        
        tsc->per_lane_control = 1 ;
        tscmod_tier1_selector("DECODE_SET", tsc, &rv);
        
        tsc->per_lane_control = 1 ;
        tscmod_tier1_selector("CREDIT_SET", tsc, &rv);
    }

    if(speed<2500) {
       tsc->port_type = port_type_mode ;
    }

    /* update plldiv for other port in the same core to prevent another PLL seq
       for the other port*/
    if( tsc->accData ) {
       tsc->ctrl_type |= TSCMOD_CTRL_TYPE_ERR ;
       tsc->err_code  |= TSCMOD_ERR_PLLDIV_INIT_PLL ;
       if(tsc->verbosity & (TSCMOD_DBG_PRINT|TSCMOD_DBG_INIT|TSCMOD_DBG_SPD)) {
          printf ("Error: u=%0d p=%0d init plldiv is not set correctly current plldiv=%0d \
new plldiv=%0d\n", tsc->unit, tsc->port, tsc->plldiv, tsc->accData);
       }
    }

    SOC_IF_ERROR_RETURN
       (_phy_tscmod_tx_control_get(unit, port, &tx_drv[0], txdrv_inx));
    SOC_IF_ERROR_RETURN
       (_phy_tscmod_tx_control_set(unit, port, &tx_drv[0]));

    /* loading firmware */
    if (TSCMOD_MODEL_TYPE_GET(tsc) == TSCMOD_WC) {
       uint8 *pdata;
       int   ucode_len;
       int   alloc_flag;
       if ( DEV_CFG_PTR(pc)->load_mthd && (DEV_CFG_PTR(pc)->load_ucode_done==0) &&
            (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit) )) {

          /* PHY-1116 */
          tsc->lane_select      = TSCMOD_LANE_BCST ;
          tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

          SOC_IF_ERROR_RETURN
             (_phy_tscmod_ucode_get(tsc->unit,tsc->port,&pdata,&ucode_len,&alloc_flag));
          SOC_IF_ERROR_RETURN
             (phy_tscmod_firmware_load(tsc->unit,tsc->port,0,pdata, ucode_len, &uc_ver, &uc_crc));

          pCfg->uc_ver = uc_ver ;  pCfg->uc_crc = uc_crc ;
          DEV_CFG_PTR(pc)->load_ucode_done =1 ;
          tsc->ctrl_type = tsc->ctrl_type | TSCMOD_CTRL_TYPE_FW_LOADED ;

          if (alloc_flag) {
             sal_free(pdata);
          }
       }
    }

    if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED ) {
       if((TSCMOD_MODEL_REVID_GET(tsc)<=TSCMOD_MODEL_REVID_A1) &&
          (TSCMOD_MODEL_PROJ_GET(tsc)!=TSCMOD_MODEL_PROJ_HR2)){
          /* enable FW credit_programming for A0, A1 TD2 only */
          rv=tscmod_uc_fw_control(tsc, TSCMOD_UC_CREDIT_PROGRAM, 1) ;
       }

       tscmod_uc_cap_set(tsc, pCfg, TSCMOD_UC_CAP_ALL, pCfg->uc_ver) ;
       if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_UC_CAP_RXP)
          tsc->ctrl_type |= TSCMOD_CTRL_TYPE_UC_RXP ;
    }
    if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY) {
       tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
    }

    /* PHY-916 */
    tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_ANALOG ;
    tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

    if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
       /* DSC SM restart */
       tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
       sal_usleep(1000) ;
    }

    tsc->lane_select      = tmp_select ;   /* restore */
    tsc->this_lane        = tmp_lane ;

    return rv ;
}

STATIC int
_phy_tscmod_port_init_wait_pmd_lock(int unit, soc_port_t port)
{
    phy_ctrl_t        *pc ;
    TSCMOD_DEV_DESC_t *pDesc ;
    tscmod_st         *tsc ;

    int               rv, lane, lane_s, lane_e, tmp_select, tmp_lane, pmd_locked ;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);


    lane                 = 0 ;
    tmp_select           = tsc->lane_select ;
    tmp_lane             = tsc->this_lane   ;

    if((tsc->an_type != TSCMOD_AN_TYPE_ILLEGAL) && (tsc->an_type != TSCMOD_AN_NONE) ) {
       /* no need to wait for AN compeleted. Link status tells all */
       return SOC_E_NONE ;
    }

    pmd_locked = 1 ;

    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         lane_s = 0 ; lane_e = 4 ;
    } else if (tsc->port_type == TSCMOD_DXGXS ) {
       if((tsc->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
       else { lane_s = 0; lane_e = 2 ; }
    } else {
       lane_s = tsc->this_lane ; lane_e = lane_s + 1 ;
    }
    for(lane=lane_s; lane<lane_e; lane++) {
       tsc->lane_select = _tscmod_get_lane_select(unit, tsc->port, lane) ;
       tsc->this_lane   = lane ;

       tsc->per_lane_control = TSCMOD_DIAG_PMD_LOCK_LATCH ;
       tsc->diag_type        = TSCMOD_DIAG_PMD ;
       tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
       if(tsc->verbosity & TSCMOD_DBG_SUB)
          printf ("%-22s: u=%0d p=%0d init wait l=%0d lane_sel=%x this_lane=%0d lock=%0d\n",
                 FUNCTION_NAME(), unit, port, lane, tsc->lane_select, tsc->this_lane, tsc->accData);
       if(tsc->accData==0) {
          pmd_locked = 0 ;
          /* don't use break because of LATCH version through 4 lanes, or call this func for 4 times to clear 4 latched low */
       }
    }

    /*
     * finally allow traffic to mac; per link
     */
    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
        tsc->per_lane_control = TSCMOD_LANE_BCST ;    /* BCST */
        tsc->lane_select      = TSCMOD_LANE_BCST ;
    } else if ( tsc->port_type   == TSCMOD_DXGXS ) {
        tsc->lane_select      = _tscmod_get_lane_select(unit, port, tmp_lane) ;
        tsc->per_lane_control = 1 ;                   /* dual-BCST */
    } else {
        tsc->lane_select      = _tscmod_get_lane_select(unit, port, tmp_lane) ;
        tsc->per_lane_control = tsc->lane_select ;
    }

    if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
       tsc->per_lane_control = TSCMOD_RXP_UC_ON ;
       tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
    }

    /* TSCOP-004: */
    if(pmd_locked) {
       if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
          /* let the uC to enable RXP */
       } else {
          tsc->per_lane_control = TSCMOD_RXP_REG_ON ;
          tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
       }
    }

    tsc->lane_select = tmp_select ;  /* restore */
    tsc->this_lane   = tmp_lane ;
    return rv ;
}

/* when FW RXP EN logic is on */
STATIC int
_phy_tscmod_port_lkuc_rxp_handler(int unit, soc_port_t port, int *link)
{
   phy_ctrl_t        *pc ;
   TSCMOD_DEV_DESC_t *pDesc ;
   tscmod_st         *tsc ;
   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);
   /* no action required in this mode */
   if(tsc->verbosity & TSCMOD_DBG_SCAN)
      printf ("%-22s: u=%0d p=%0d link=%0d l=%0d lane_sel=%x this_lane=%0d\n",
             FUNCTION_NAME(), unit, port, *link, tsc->this_lane, tsc->lane_select, tsc->this_lane);
   return SOC_E_NONE ;
}

/* during linkup, if pmd latched low, reset RXP and report link down */
STATIC int
_phy_tscmod_port_lkup_pmd_lock_handler(int unit, soc_port_t port, int *link)
{
   phy_ctrl_t        *pc ;
   TSCMOD_DEV_DESC_t *pDesc ;
   tscmod_st         *tsc ;
   int               rv, lane, lane_s, lane_e, tmp_select, tmp_lane, pmd_locked, tmp_verb ;

   pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);


    lane                 = 0 ;
    tmp_select           = tsc->lane_select ;
    tmp_lane             = tsc->this_lane   ;
    tmp_verb             = tsc->verbosity ;

    if(tsc->verbosity & TSCMOD_DBG_SCAN) {
    } else {  tsc->verbosity = 0 ; }

    pmd_locked = 1 ;
    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         lane_s = 0 ; lane_e = 4 ;
    } else if (tsc->port_type == TSCMOD_DXGXS ) {
       if((tsc->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
       else { lane_s = 0; lane_e = 2 ; }
    } else {
       lane_s = tsc->this_lane ; lane_e = lane_s + 1 ;
    }

    for(lane=lane_s; lane<lane_e; lane++) {
       tsc->lane_select = _tscmod_get_lane_select(unit, tsc->port, lane) ;
       tsc->this_lane   = lane ;

       tsc->per_lane_control = TSCMOD_DIAG_PMD_LOCK_LATCH ;
       tsc->diag_type        = TSCMOD_DIAG_PMD ;
       tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
       if(tsc->verbosity & TSCMOD_DBG_SUB)
          printf ("%-22s: u=%0d p=%0d WAIT_PMD_LOCK lkup l=%0d lane_sel=%x this_lane=%0d lock=%0d\n",
                 FUNCTION_NAME(), unit, port, lane, tsc->lane_select, tsc->this_lane, tsc->accData);
       if(tsc->accData==0) {
          pmd_locked = 0 ;
          /* don't use break because of LATCH version through 4 lanes, or call this func for 4 times to clear 4 latched low */
       }
    }
    if(pmd_locked==0) {
       if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
          tsc->per_lane_control = TSCMOD_LANE_BCST ;    /* BCST */
       } else if ( tsc->port_type   == TSCMOD_DXGXS ) {
          tsc->lane_select      = _tscmod_get_lane_select(unit, port, tmp_lane) ;
       } else {
          tsc->lane_select      = _tscmod_get_lane_select(unit, port, tmp_lane) ;
       }
       tsc->this_lane  = tmp_lane  ;

       tsc->verbosity = tmp_verb ;
       if(tsc->verbosity & TSCMOD_DBG_LINK)
          printf ("%-22s: u=%0d p=%0d DIAG_PMD_LOCK lkup l=%0d lane_sel=%x this_lane=%0d lock=%0d\n",
                 FUNCTION_NAME(), unit, port, lane, tsc->lane_select, tsc->this_lane, pmd_locked);

       tsc->per_lane_control = 0 ;
       tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* diable RX path */

       TSCMOD_SP_CNT(pc) = 0 ;
       *link =0 ;
    }


    tsc->verbosity   = tmp_verb ;
    tsc->lane_select = tmp_select ;  /* restore */
    tsc->this_lane   = tmp_lane ;
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_port_lkdn_pmd_lock_handler(int unit, soc_port_t port, int dsc_restart_mode, int debug_id)
{
   phy_ctrl_t        *pc ;
    TSCMOD_DEV_DESC_t *pDesc ;
    tscmod_st         *tsc ;

    int               rv, lane, lane_s, lane_e, tmp_select, tmp_lane, pmd_locked, tmp_verb, rx_lane;
    uint32            current_time ;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);


    lane                 = 0 ;
    tmp_select           = tsc->lane_select ;
    tmp_lane             = tsc->this_lane   ;
    tmp_verb             = tsc->verbosity ;

    if(tsc->verbosity & TSCMOD_DBG_SCAN) {
    } else {  tsc->verbosity = 0 ; }

    pmd_locked = 1 ;
    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         lane_s = 0 ; lane_e = 4 ;
    } else if (tsc->port_type == TSCMOD_DXGXS ) {
       if((tsc->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
       else { lane_s = 0; lane_e = 2 ; }
    } else {
       lane_s = tsc->this_lane ; lane_e = lane_s + 1 ;
    }
    for(lane=lane_s; lane<lane_e; lane++) {
       tsc->lane_select = _tscmod_get_lane_select(unit, tsc->port, lane) ;
       tsc->this_lane   = lane ;

       tsc->per_lane_control = TSCMOD_DIAG_PMD_LOCK_LATCH ;
       tsc->diag_type        = TSCMOD_DIAG_PMD ;
       tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
       if(tsc->verbosity & TSCMOD_DBG_SUB)
          printf ("%-22s: u=%0d p=%0d WAIT_PMD_LOCK l=%0d lane_sel=%x this_lane=%0d lock=%0d\n",
                 FUNCTION_NAME(), unit, port, lane, tsc->lane_select, tsc->this_lane, tsc->accData);
       if(tsc->accData==0) {
          pmd_locked = 0 ;
          /* don't use break because of LATCH version through 4 lanes, or call this func for 4 times to clear 4 lacthed low */
       }
    }

    /* enabble/disable RX lane path */
    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
        tsc->lane_select = TSCMOD_LANE_BCST ;    /* BCST */
    } else if ( tsc->port_type   == TSCMOD_DXGXS ) {
        tsc->lane_select      = _tscmod_get_lane_select(unit, port, tmp_lane) ;
    } else {
        tsc->lane_select      = _tscmod_get_lane_select(unit, port, tmp_lane) ;
    }
    tsc->this_lane  = tmp_lane  ;

    /* read back the RX LANE control bit */
    tsc->per_lane_control = 0xa ;
    tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
    rx_lane = tsc->accData ;   /* enabled or not */

    if(tsc->verbosity &(TSCMOD_DBG_FSM|TSCMOD_DBG_PATH))
       printf ("%-22s: u=%0d p=%0d pmd_lck=%0d rx_lane=%0d ctrl_type=%x"
         " current_usce=%0x cnt=%0d usec=%0x restart=%0d dbid=%0d\n", FUNCTION_NAME(),
              tsc->unit, tsc->port, pmd_locked, rx_lane, tsc->ctrl_type,
              sal_time_usecs(), TSCMOD_SP_CNT(pc),TSCMOD_SP_TIME(pc),
              dsc_restart_mode, debug_id) ;

    tsc->verbosity = tmp_verb ;

    if(pmd_locked) {
       if((dsc_restart_mode==1)&&(tsc->ctrl_type&TSCMOD_CTRL_TYPE_FW_LOADED)
          && (FORCE_CL72_ENABLED(pc)==0) && (!(tsc->ctrl_type&TSCMOD_CTRL_TYPE_PRBS_ON)) ) {
          if(TSCMOD_SP_CNT(pc)>3) {  /* wait for 3 when linkscan is off. linkscan is 125ms default*/
             current_time = sal_time_usecs() ;
             if(SAL_USECS_SUB(current_time,TSCMOD_SP_TIME(pc))>=0) {
                if(tsc->verbosity &(TSCMOD_DBG_FSM|TSCMOD_DBG_PATH))
                   printf ("%-22s: u=%0d p=%0d pmd_lck link_dn wait to long diable DSC+RX path\n",
                          FUNCTION_NAME(), tsc->unit, tsc->port) ;

                tsc->per_lane_control = 0;
                tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* diable RX path */

                tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM;
                tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
                sal_usleep(1000) ;

                if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
                   tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
                   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
                   sal_usleep(1000) ;
                }

                rx_lane = 1 ;   /* not allow to enable RXP if just DSC restarted */
                TSCMOD_SP_CNT(pc) = 0 ;
             }
          }
       }

       if( rx_lane == 0 ) {
          if(tsc->verbosity &(TSCMOD_DBG_FSM|TSCMOD_DBG_PATH))
             printf ("%-22s: u=%0d p=%0d enable RX path\n", FUNCTION_NAME(),
                    tsc->unit, tsc->port) ;
          /* TSCOP-006: should wait longer or not */
          tsc->per_lane_control = 1;
          tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* enable RX path */

          TSCMOD_SP_CNT(pc) = 0 ;
       }
    } else {
       if( rx_lane == 1 ) {
          if(tsc->verbosity &(TSCMOD_DBG_FSM|TSCMOD_DBG_PATH))
             printf ("%-22s: u=%0d p=%0d disable RX path\n", FUNCTION_NAME(),
                    tsc->unit, tsc->port) ;

          tsc->per_lane_control = 0;
          tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* diable RX path */

          TSCMOD_SP_CNT(pc) = 0 ;
       }
    }

    if(TSCMOD_SP_CNT(pc)==0) {
       TSCMOD_SP_RECORD_TIME(pc) ;
       TSCMOD_SP_TIME(pc) = TSCMOD_SP_TIME(pc) + 2000000 ; /* 2.0 second */
       TSCMOD_SP_CNT(pc) = 1 ;
    } else {
       TSCMOD_SP_CNT_INC(pc) ;
    }

    tsc->lane_select = tmp_select ;  /* restore */
    tsc->this_lane   = tmp_lane ;

    if(pmd_locked) tsc->accData = 1 ;
    else           tsc->accData = 0 ;

    return SOC_E_NONE ;
}

STATIC int
_phy_tscmod_cl73_cl37_handler(int unit, soc_port_t port, int *link)
{
   phy_ctrl_t        *pc ;
   TSCMOD_DEV_DESC_t *pDesc ;
   tscmod_st         *tsc ;
   int               rv, tmp_select, tmp_lane, tmp_dxgxs, tmp_verb, pd_cmpl, pd_kx_cmpl ;
   uint32            current_time ;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);
   current_time = 0 ;

   tmp_select           = tsc->lane_select ;
   tmp_lane             = tsc->this_lane   ;
   tmp_dxgxs            = tsc->dxgxs ;
   tmp_verb             = tsc->verbosity ;

   rv = SOC_E_NONE ;

   if(TSCMOD_AN_STATE(pc)==TSCMOD_AN_CL73_CL37_ON) {
      
      tsc->an_type = TSCMOD_CL73 ;
      tscmod_tier1_selector("AUTONEG_CONTROL", tsc, &rv);
      
      TSCMOD_AN_STATE(pc)    = TSCMOD_AN_WAIT_CL73 ;
      TSCMOD_AN_TIME(pc)     = sal_time_usecs() + TSCMOD_AN_WAIT_CL73_TMR ;
      TSCMOD_AN_TICK_CNT(pc) = 0  ;
      /* no mode change */
      if(tsc->verbosity & TSCMOD_DBG_FSM)
         printf ("%-22s: u=%0d p=%0d link=%0d state=%0x tick=%0d m=%0x CL73 frm ON\n", FUNCTION_NAME(), tsc->unit,
                tsc->port, *link, TSCMOD_AN_STATE(pc), TSCMOD_AN_TICK_CNT(pc), TSCMOD_AN_MODE(pc)) ;
   } else if(TSCMOD_AN_STATE(pc)==TSCMOD_AN_WAIT_CL73){
      TSCMOD_AN_TICK_CNT(pc) = TSCMOD_AN_TICK_CNT(pc) +1 ;
      if(*link) {
         tsc->per_lane_control = TSCMOD_MISC_CTL_PDET_COMPL ;
         tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);
         pd_cmpl = tsc->accData ;
         if(pd_cmpl) {
            tsc->per_lane_control = TSCMOD_MISC_CTL_PDET_KX_COMPL ;
            tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);
            pd_kx_cmpl = tsc->accData ;
            if(pd_kx_cmpl) {
               if(TSCMOD_AN_MODE(pc) & TSCMOD_AN_MODE_CL37_TRIED ){
                  /* CL36 done */
                  TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_DONE ;
               } else {
                  tsc->an_type = TSCMOD_CL37_10G ;
                  tscmod_tier1_selector("AUTONEG_CONTROL", tsc, &rv);
                  
                  TSCMOD_AN_STATE(pc)    = TSCMOD_AN_WAIT_CL37 ;
                  TSCMOD_AN_TIME(pc)     = sal_time_usecs() + TSCMOD_AN_WAIT_CL37_TMR ;
                  TSCMOD_AN_TICK_CNT(pc) = 0  ;
                  TSCMOD_AN_MODE(pc)     = TSCMOD_AN_MODE_CL37_TRIED ;  /* CL37_10G tried after 1K pdet */
                  *link = 0 ;
               }
            } else {
               /* CL48 done */
               TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_DONE ;
            }
         } else {
            /* CL73 done */
            TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_DONE ;
         }
      } else {
         TSCMOD_AN_TICK_CNT(pc) = TSCMOD_AN_TICK_CNT(pc) + 1 ;
         if(((SAL_USECS_SUB(sal_time_usecs(), TSCMOD_AN_TIME(pc))>=0)&&(TSCMOD_AN_TICK_CNT(pc)>1))||
            (TSCMOD_AN_TICK_CNT(pc)>128)){
            TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_ON ;
            TSCMOD_AN_TICK_CNT(pc) = 0;
            TSCMOD_AN_MODE(pc)     = 0;
         }
      }
   } else if(TSCMOD_AN_STATE(pc)==TSCMOD_AN_WAIT_CL37){
      TSCMOD_AN_TICK_CNT(pc) = TSCMOD_AN_TICK_CNT(pc) +1 ;
      if(*link) {
         /* CL37 done */
         TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_DONE ;
      } else {
         current_time = sal_time_usecs() ;
         if(((SAL_USECS_SUB(current_time, TSCMOD_AN_TIME(pc))>=0)&&(TSCMOD_AN_TICK_CNT(pc)>1))||
            (TSCMOD_AN_TICK_CNT(pc)>128)){
            TSCMOD_AN_STATE(pc)=TSCMOD_AN_CL73_CL37_ON ;
            /* no mode change */
         }
      }
   } else if(TSCMOD_AN_STATE(pc)==TSCMOD_AN_CL73_CL37_DONE){
      if(*link) {
         
      } else {
         TSCMOD_AN_STATE(pc)  = TSCMOD_AN_CL73_CL37_ON ;
         TSCMOD_AN_MODE(pc)   = 0 ;
      }
   } else {
      TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_OFF ;
      TSCMOD_AN_MODE(pc)     = 0 ;
      TSCMOD_AN_TICK_CNT(pc) = 0 ;
   }

   tsc->lane_select      = tmp_select ;   /* restore */
   tsc->this_lane        = tmp_lane ;
   tsc->dxgxs            = tmp_dxgxs ;
   tsc->verbosity        = tmp_verb ;

   return SOC_E_NONE;
}

STATIC int
_phy_tscmod_hpam_cl37_handler(int unit, soc_port_t port, int *link)
{
   phy_ctrl_t        *pc ;
   TSCMOD_DEV_DESC_t *pDesc ;
   tscmod_st         *tsc ;
   int               rv, tmp_select, tmp_lane, tmp_dxgxs, tmp_verb, pd_cmpl, pd_kx_cmpl ;
   uint32            current_time ;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);
   current_time = 0 ;

   tmp_select           = tsc->lane_select ;
   tmp_lane             = tsc->this_lane   ;
   tmp_dxgxs            = tsc->dxgxs ;
   tmp_verb             = tsc->verbosity ;
   
   rv = SOC_E_NONE ;

   tsc->dxgxs   = 0 ;

   if(TSCMOD_AN_STATE(pc)==TSCMOD_AN_CL73_CL37_ON) {
      tsc->an_type = TSCMOD_HPAM ;
      tscmod_tier1_selector("AUTONEG_CONTROL", tsc, &rv);
      
      TSCMOD_AN_STATE(pc)    = TSCMOD_AN_WAIT_CL73 ;
      TSCMOD_AN_TIME(pc)     = sal_time_usecs() + TSCMOD_AN_WAIT_HPBAM_TMR ;
      TSCMOD_AN_TICK_CNT(pc) = 0  ;
      /* no mode change */
      if(tsc->verbosity & TSCMOD_DBG_FSM)
         printf ("%-22s: u=%0d p=%0d link=%0d state=%0x tick=%0d m=%0x CL73 frm ON\n", FUNCTION_NAME(), tsc->unit,
                tsc->port, *link, TSCMOD_AN_STATE(pc), TSCMOD_AN_TICK_CNT(pc), TSCMOD_AN_MODE(pc)) ;
   } else if(TSCMOD_AN_STATE(pc)==TSCMOD_AN_WAIT_CL73){
      TSCMOD_AN_TICK_CNT(pc) = TSCMOD_AN_TICK_CNT(pc) +1 ;
      if(*link) {
         tsc->per_lane_control = TSCMOD_MISC_CTL_PDET_COMPL ;
         tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);
         pd_cmpl = tsc->accData ;
         if(pd_cmpl) {
            tsc->per_lane_control = TSCMOD_MISC_CTL_PDET_KX_COMPL ;
            tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);
            pd_kx_cmpl = tsc->accData ;
            if(pd_kx_cmpl) {
               if(TSCMOD_AN_MODE(pc) & TSCMOD_AN_MODE_CL37_TRIED ){
                  /* CL36 done */
                  TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_DONE ;
               } else {
                  tsc->an_type = TSCMOD_CL37_10G ;
                  tscmod_tier1_selector("AUTONEG_CONTROL", tsc, &rv);
                  
                  TSCMOD_AN_STATE(pc)    = TSCMOD_AN_WAIT_CL37 ;
                  TSCMOD_AN_TIME(pc)     = sal_time_usecs() + TSCMOD_AN_WAIT_CL37_TMR ;
                  TSCMOD_AN_TICK_CNT(pc) = 0  ;
                  TSCMOD_AN_MODE(pc)     = TSCMOD_AN_MODE_CL37_TRIED ;  /* CL37_10G tried after 1K pdet */
                  *link = 0 ;
               }
            } else {
               /* CL48 done */
               TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_DONE ;
            }
         } else {
            /* CL73 done */
            TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_DONE ;
         }
      } else {
         TSCMOD_AN_TICK_CNT(pc) = TSCMOD_AN_TICK_CNT(pc) + 1 ;
         if(((SAL_USECS_SUB(sal_time_usecs(), TSCMOD_AN_TIME(pc))>=0)&&(TSCMOD_AN_TICK_CNT(pc)>1))||
            (TSCMOD_AN_TICK_CNT(pc)>128)){
            TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_ON ;
            TSCMOD_AN_TICK_CNT(pc) = 0;
            TSCMOD_AN_MODE(pc)     = 0;
         }
      }
   } else if(TSCMOD_AN_STATE(pc)==TSCMOD_AN_WAIT_CL37){
      TSCMOD_AN_TICK_CNT(pc) = TSCMOD_AN_TICK_CNT(pc) +1 ;
      if(*link) {
         /* CL37 done */
         TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_DONE ;
      } else {
         current_time = sal_time_usecs() ;
         if(((SAL_USECS_SUB(current_time, TSCMOD_AN_TIME(pc))>=0)&&(TSCMOD_AN_TICK_CNT(pc)>1))||
            (TSCMOD_AN_TICK_CNT(pc)>128)){
            TSCMOD_AN_STATE(pc)=TSCMOD_AN_CL73_CL37_ON ;
               /* no mode change */
         }
      }
   } else if(TSCMOD_AN_STATE(pc)==TSCMOD_AN_CL73_CL37_DONE){
      if(*link) {
         
      } else {
         TSCMOD_AN_STATE(pc)  = TSCMOD_AN_CL73_CL37_ON ;
         TSCMOD_AN_MODE(pc)   = 0 ;
      }
   } else {
      TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_OFF ;
      TSCMOD_AN_MODE(pc)     = 0 ;
      TSCMOD_AN_TICK_CNT(pc) = 0 ;
   }

   tsc->lane_select      = tmp_select ;   /* restore */
   tsc->this_lane        = tmp_lane ;
   tsc->dxgxs            = tmp_dxgxs ;
   tsc->verbosity        = tmp_verb ;

   return SOC_E_NONE;
}

STATIC int
_phy_tscmod_an_war_handler(int unit, soc_port_t port, int *link)
{
   phy_ctrl_t        *pc ;
   TSCMOD_DEV_DESC_t *pDesc ;
   TSCMOD_DEV_CFG_t  *pCfg;
   tscmod_st         *tsc ;
   int               rv, tmp_verb;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg; 
   
   rv = SOC_E_NONE ;
   tmp_verb             = tsc->verbosity ;

   if(tsc->verbosity & TSCMOD_DBG_SCAN) {
   } else {
      if(tsc->verbosity & TSCMOD_DBG_FSM) tsc->verbosity |=TSCMOD_DBG_FSM;
      else                                tsc->verbosity = 0 ;
   }

   if((pCfg->cl73an==TSCMOD_CL73_CL37)||(pCfg->cl73an==TSCMOD_CL73_SW_CL37))
      rv = _phy_tscmod_cl73_cl37_handler(unit, port, link) ;
   else if((pCfg->cl73an==TSCMOD_CL73_HPAM_CL37)||(pCfg->cl73an==TSCMOD_CL73_HPAM_AUTOCL73_CL37))
      rv = _phy_tscmod_hpam_cl37_handler(unit, port, link) ;
   
   tsc->verbosity        = tmp_verb ;
   return rv ;
}


/*
 * Function:
 *      phy_tscmod_init
 * Purpose:
 *      Initialize hc phys
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_tscmod_init(int unit, soc_port_t port)
{
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_tscmod_init: u=%d p=%d\n"), unit, port));
    /* init the configuration value */
    _phy_tscmod_config_init(unit,port);

    tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;

    _phy_tscmod_port_init_start(unit, port) ;

    _phy_tscmod_port_init_pll_lock_wait(unit, port) ;

    _phy_tscmod_port_init_wait_pmd_lock(unit, port) ;

    tscmod_sema_unlock(unit, port) ;

    return SOC_E_NONE;
}

/* SDK-49889: defined the requirement of timer precision and accuracy.
 */
STATIC int
phy_tscmod_sw_rx_los_uc_link_handler(int unit, soc_port_t port, int *link)
{
   int tmp_sel, tmp_lane, tmp_dxgxs, lane, lane_s, lane_e;
   phy_ctrl_t *pc;
   tscmod_st  *tsc ;
   TSCMOD_DEV_DESC_t *pDesc;
   int rv, sig_ldet, factor, live_link ;
   uint32  current_time , delta_t = 0;
   uint32 timer1_p1=0, timer1_p25=0, timer2_p1=0, timer2_p5=0;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   = (tscmod_st *)( pDesc + 1);

   tmp_sel              = tsc->lane_select ;
   tmp_lane             = tsc->this_lane   ;
   tmp_dxgxs            = tsc->dxgxs ;
   current_time         = 0 ;
   factor               = 0 ;
   live_link            = 0 ;
   rv                   = SOC_E_NONE ;

   if(tsc->verbosity & TSCMOD_DBG_SCAN) {
      factor = 4 ;  /* slow down timeout for debug */
   } else {  tsc->verbosity = 0 ; }

   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      lane_s = 0 ; lane_e = 4 ;
   } else if (tsc->port_type == TSCMOD_DXGXS ) {
      if((tsc->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
      else { lane_s = 0; lane_e = 2 ; }
   } else {
      lane_s = tsc->this_lane ; lane_e = lane_s + 1 ;
   }

   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      tsc->lane_select      = TSCMOD_LANE_BCST ;
   }

   if(DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_FLAP_WAR) {
       if(DEV_CFG_PTR(pc)->macd==NULL) {
           SOC_IF_ERROR_RETURN
               (soc_mac_probe(unit, port, &DEV_CFG_PTR(pc)->macd));
       }
   }
   
   if(tsc->verbosity &(TSCMOD_DBG_LINK|TSCMOD_DBG_PATH))
       printf ("%-22s: u=%0d p=%0d pcs_l=%0d state=%x"
         " current_usc=%0x cnt=%0d sw_rx_usec=%0x\n",
              FUNCTION_NAME(), tsc->unit, tsc->port, *link, DEV_CFG_PTR(pc)->sw_rx_los.state,
              sal_time_usecs(), DEV_CFG_PTR(pc)->sw_rx_los.count, DEV_CFG_PTR(pc)->sw_rx_los.start_time) ;

   switch(DEV_CFG_PTR(pc)->sw_rx_los.state) {
   case RXLOS_RESET:
      if(DEV_CFG_PTR(pc)->sw_rx_los.count++==0) {
         current_time = sal_time_usecs() ;
         DEV_CFG_PTR(pc)->sw_rx_los.start_time = current_time +
                                                 ((current_time & 0x3ff) <<10) +  /* random delay */
                                                 (TSCMOD_RXLOS_TIMEOUT2 << factor) ;

         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
            sal_usleep(1000) ;
            tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
            tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
         }

         *link = FALSE ;
         break ;
      }
      if(*link) {
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf ("%-22s: u=%0d p=%0d link=%0d state=%0d 2wait_p1\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, DEV_CFG_PTR(pc)->sw_rx_los.state) ;
         factor                                = (tsc->verbosity&TSCMOD_DBG_SCAN) ? 4: 0 ;
         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_LINK_WAIT_P1 ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
         
         timer1_p1  = (DEV_CFG_PTR(pc)->rxlos_link_wait_timer ==0) ? TSCMOD_RXLOS_TIMEOUTp1 : DEV_CFG_PTR(pc)->rxlos_link_wait_timer;
         timer1_p25 = (DEV_CFG_PTR(pc)->rxlos_link_wait_timer ==0) ? TSCMOD_RXLOS_TIMEOUTp25: DEV_CFG_PTR(pc)->rxlos_link_wait_timer;
         if(DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_UP_FAST_WAR) {
            DEV_CFG_PTR(pc)->sw_rx_los.start_time = sal_time_usecs() + (timer1_p1 << factor) ;
         } else {
            DEV_CFG_PTR(pc)->sw_rx_los.start_time = sal_time_usecs() + (timer1_p25 << factor) ;
         }
         DEV_CFG_PTR(pc)->rxlos_first_linkup_time = sal_time_usecs();
      } else {
         current_time = sal_time_usecs() ;
         if(((SAL_USECS_SUB(current_time,DEV_CFG_PTR(pc)->sw_rx_los.start_time)>=0)&&
             (DEV_CFG_PTR(pc)->sw_rx_los.count>=2))||
            (DEV_CFG_PTR(pc)->sw_rx_los.count>=128)) {

            if(tsc->verbosity & TSCMOD_DBG_FSM)
               printf("%-22s: u=%0d p=%0d link=%0d state=%0d 2reset cnt=%0d t=%0x\n", FUNCTION_NAME(), tsc->unit,
                      tsc->port, *link, DEV_CFG_PTR(pc)->sw_rx_los.state, DEV_CFG_PTR(pc)->sw_rx_los.count, current_time) ;
            DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
            DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
            if(++factor>4) factor = 4 ;
         }
         if((DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_FLAP_WAR)&&
            (DEV_CFG_PTR(pc)->sw_rx_los.rf_dis==1)) {
             SOC_IF_ERROR_RETURN
                 (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                  SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
             DEV_CFG_PTR(pc)->sw_rx_los.rf_dis = 0 ;
             rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_RXLOS) ;
         }
      }
      *link = FALSE ;
      break ;
   case RXLOS_LINK_WAIT_P1:
       current_time = sal_time_usecs() ;
       if((*link)&&
          (SAL_USECS_SUB(current_time,DEV_CFG_PTR(pc)->sw_rx_los.start_time)>=0)){
           if(tsc->verbosity & TSCMOD_DBG_FSM)
               printf ("%-22s: u=%0d p=%0d *link=%0d state=%0d 2init_up\n", 
                      FUNCTION_NAME(), tsc->unit, tsc->port, *link, 
                      DEV_CFG_PTR(pc)->sw_rx_los.state) ;
         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_INITIAL_LINK_UP ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
         DEV_CFG_PTR(pc)->sw_rx_los.rf_dis     = 1 ;
         rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_RXLOS) ;
         if(DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_FLAP_WAR) {
             SOC_IF_ERROR_RETURN
                 (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                  SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, FALSE));
         }
       } else if(*link==0) {
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf("%-22s: u=%0d p=%0d *link=%0d state=%0x 2reset\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, DEV_CFG_PTR(pc)->sw_rx_los.state) ;
         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
         if((DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_FLAP_WAR)&&
            (DEV_CFG_PTR(pc)->sw_rx_los.rf_dis==1)) {
             SOC_IF_ERROR_RETURN
                 (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                  SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
             DEV_CFG_PTR(pc)->sw_rx_los.rf_dis = 0 ;
             rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_RXLOS) ;
         }
      }
      *link = FALSE ;
      break ;
   case RXLOS_INITIAL_LINK_UP:
      if(*link) {
         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
            sal_usleep(1000) ;
            tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
            tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
         }
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf("%-22s: u=%0d p=%0d *link=%0d state=%0d 2restart\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, DEV_CFG_PTR(pc)->sw_rx_los.state) ;
         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESTART ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
         current_time = sal_time_usecs() ;
         delta_t      = current_time - DEV_CFG_PTR(pc)->rxlos_first_linkup_time;
         tsc->rxlos_restart_time = delta_t ;

         timer2_p1 = (DEV_CFG_PTR(pc)->rxlos_restart_timer ==0) ? TSCMOD_RXLOS_TIMEOUTp1 : DEV_CFG_PTR(pc)->rxlos_restart_timer;
         timer2_p5 = (DEV_CFG_PTR(pc)->rxlos_restart_timer ==0) ? TSCMOD_RXLOS_TIMEOUTp5 : DEV_CFG_PTR(pc)->rxlos_restart_timer;
         if(DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_UP_FAST_WAR) {
             DEV_CFG_PTR(pc)->sw_rx_los.count_time = current_time + (timer2_p1 << factor) ;
         } 
         DEV_CFG_PTR(pc)->sw_rx_los.start_time = current_time + (timer2_p5 << factor) ;
      } else {
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf("%-22s: u=%0d p=%0d link=%0d state=%0d 2reset\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, DEV_CFG_PTR(pc)->sw_rx_los.state) ;
         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
         if((DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_FLAP_WAR)&&
            (DEV_CFG_PTR(pc)->sw_rx_los.rf_dis==1)) {
             SOC_IF_ERROR_RETURN
                 (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                  SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
             DEV_CFG_PTR(pc)->sw_rx_los.rf_dis = 0 ;
             rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_RXLOS) ;
         }
      }
      *link = FALSE ;
      break ;
   case RXLOS_RESTART:
      DEV_CFG_PTR(pc)->sw_rx_los.count++ ;
      sig_ldet = 1;

      for(lane=lane_s; lane<lane_e; lane++) {
         tsc->lane_select = _tscmod_get_lane_select(unit, tsc->port, lane) ;
         tsc->this_lane   = lane ;

         tsc->per_lane_control = TSCMOD_DIAG_PMD_INFO_LATCH ;
         tsc->diag_type        = TSCMOD_DIAG_PMD ;
         tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
         if(tsc->verbosity & TSCMOD_DBG_SUB)
            printf("%-22s: u=%0d p=%0d WAIT_PMD_LOCK l=%0d lane_sel=%x this_lane=%0d lock=%0d\n",
                   FUNCTION_NAME(), unit, port, lane, tsc->lane_select, tsc->this_lane, tsc->accData);
         if((tsc->accData & TSCMOD_DIAG_PMD_INFO_LOCK_BIT)==0) {
            /* don't use break because of LATCH version through 4 lanes, or call this func for 4 times to clear 4 lacthed low */
         }
         if((tsc->accData & TSCMOD_DIAG_PMD_INFO_SDET_BIT)==0) {
            sig_ldet = 0 ;
            /* don't use break because of LATCH version through 4 lanes, or call this func for 4 times to clear 4 lacthed low */
         }
      }
      
      if(DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_UP_FAST_WAR) {
          tsc->lane_select      = tmp_sel ;
          tsc->this_lane        = tmp_lane ;
          tsc->diag_type        = TSCMOD_DIAG_LINK;
          tsc->per_lane_control = TSCMOD_DIAG_LINK_LATCH ;
          tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv); 
          live_link = tsc->accData ;
      }

      current_time = sal_time_usecs() ;
      if(sig_ldet==0) {
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf("%-22s: u=%0d p=%0d link=%0d state=%0d sig_det_0 2reset\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, DEV_CFG_PTR(pc)->sw_rx_los.state) ;
         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
         if((DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_FLAP_WAR)&&
            (DEV_CFG_PTR(pc)->sw_rx_los.rf_dis==1)) {
             SOC_IF_ERROR_RETURN
                 (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                  SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
             DEV_CFG_PTR(pc)->sw_rx_los.rf_dis = 0 ;
             rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_RXLOS) ;
         }
      } else if((*link==0)&&(((SAL_USECS_SUB(current_time,DEV_CFG_PTR(pc)->sw_rx_los.start_time)>=0)&&
                              (DEV_CFG_PTR(pc)->sw_rx_los.count>=2))||
                             (DEV_CFG_PTR(pc)->sw_rx_los.count>=64))){
         if(tsc->verbosity & TSCMOD_DBG_FSM)
            printf("%-22s: u=%0d p=%0d link=%0d state=%0d 2reset\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, DEV_CFG_PTR(pc)->sw_rx_los.state) ;
         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
         if((DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_FLAP_WAR)&&
            (DEV_CFG_PTR(pc)->sw_rx_los.rf_dis==1)) {
             SOC_IF_ERROR_RETURN
                 (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                  SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
             DEV_CFG_PTR(pc)->sw_rx_los.rf_dis = 0 ;
             rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_RXLOS) ;
         }
      } else {
         if(DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_UP_FAST_WAR) {
             if((live_link==1)&&(SAL_USECS_SUB(current_time,DEV_CFG_PTR(pc)->sw_rx_los.count_time)>=0)){
                 if(tsc->verbosity & TSCMOD_DBG_FSM)
                     printf("%-22s: u=%0d p=%0d link=%0d state=%0d 2wait_p2_a\n", FUNCTION_NAME(), tsc->unit,
                   tsc->port, *link, DEV_CFG_PTR(pc)->sw_rx_los.state) ;
                 DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_LINK_WAIT_P2 ;
                 DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
             } else if((live_link==0)&&(SAL_USECS_SUB(current_time,DEV_CFG_PTR(pc)->sw_rx_los.count_time)>=0)) {
                 if(tsc->verbosity & TSCMOD_DBG_FSM) {
                     printf("%-22s: u=%0d p=%0d link=%0d state=%0d TO reset\n", FUNCTION_NAME(), tsc->unit,
                            tsc->port, *link, DEV_CFG_PTR(pc)->sw_rx_los.state) ;
                 }
                 DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
                 DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
                 if((DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_FLAP_WAR)&&
                    (DEV_CFG_PTR(pc)->sw_rx_los.rf_dis==1)) {
                     SOC_IF_ERROR_RETURN
                         (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                          SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
                     DEV_CFG_PTR(pc)->sw_rx_los.rf_dis = 0 ;
                     rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_RXLOS) ;
                 }
             }
         } else {
             if((*link==1)&&(((DEV_CFG_PTR(pc)->sw_rx_los.count>=2)&&
                              (SAL_USECS_SUB(current_time,DEV_CFG_PTR(pc)->sw_rx_los.start_time)>=0))||
                             (DEV_CFG_PTR(pc)->sw_rx_los.count>=8))) {
                 if(tsc->verbosity & TSCMOD_DBG_FSM)
                     printf("%-22s: u=%0d p=%0d link=%0d state=%0d 2wait_p2_b\n", FUNCTION_NAME(), tsc->unit,
                            tsc->port, *link, DEV_CFG_PTR(pc)->sw_rx_los.state) ;
                 DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_LINK_WAIT_P2 ;
                 DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
             }
         }
      }
      *link = FALSE ;
      break ;
   case RXLOS_LINK_WAIT_P2:
      if(*link) {
         delta_t                               = sal_time_usecs() - DEV_CFG_PTR(pc)->rxlos_first_linkup_time;
         tsc->rxlos_linkup_time                = delta_t ;
         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_LINK_UP ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
      } else {
         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
         if((DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_FLAP_WAR)&&
            (DEV_CFG_PTR(pc)->sw_rx_los.rf_dis==1)) {
             SOC_IF_ERROR_RETURN
                 (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                  SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
             DEV_CFG_PTR(pc)->sw_rx_los.rf_dis = 0 ;
             rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_RXLOS) ;
         }
      }
      
      if(tsc->verbosity & TSCMOD_DBG_FSM)
         printf("%-22s: u=%0d p=%0d link=%0d state=%0d from wait_p2 current_usc=%0x cnt=%0d usec=%0x\n",
                FUNCTION_NAME(), tsc->unit, tsc->port, *link, DEV_CFG_PTR(pc)->sw_rx_los.state,
                sal_time_usecs(), DEV_CFG_PTR(pc)->sw_rx_los.count, DEV_CFG_PTR(pc)->sw_rx_los.start_time) ;
      break ;
   case RXLOS_LINK_UP:
      if(*link==FALSE) {
         if(tsc->verbosity & TSCMOD_DBG_FSM)
               printf("%-22s: u=%0d p=%0d link=%0d state=%0d 2reset\n", FUNCTION_NAME(), tsc->unit,
                      tsc->port, *link, DEV_CFG_PTR(pc)->sw_rx_los.state) ;
         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
      }
      if((DEV_CFG_PTR(pc)->sw_rx_los.enable & TSCMOD_RXLOS_LINK_FLAP_WAR)&&
             (DEV_CFG_PTR(pc)->sw_rx_los.rf_dis==1)) {
              SOC_IF_ERROR_RETURN
                  (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                   SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
              DEV_CFG_PTR(pc)->sw_rx_los.rf_dis = 0 ;
              rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_RXLOS) ;
      }
      break ;
   default:
      DEV_CFG_PTR(pc)->sw_rx_los.state = RXLOS_RESET ;
      DEV_CFG_PTR(pc)->sw_rx_los.count = 0 ;
      break ;
   }

   tsc->lane_select     = tmp_sel ;
   tsc->this_lane       = tmp_lane   ;
   tsc->dxgxs           = tmp_dxgxs  ;
   return rv;
}

STATIC int
phy_tscmod_sw_rx_los_link_handler(int unit, soc_port_t port, int *link)
{
   int tmp_sel, tmp_lane, lane, lane_s, lane_e;
   phy_ctrl_t *pc;
   tscmod_st  *tsc ;
   TSCMOD_DEV_DESC_t *pDesc;
   int rv, pmd_llocked, sig_ldet, rxp, factor ;
   uint32  current_time ;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   = (tscmod_st *)( pDesc + 1);

   tmp_sel              = tsc->lane_select ;
   tmp_lane             = tsc->this_lane   ;
   rxp                  = 0 ;
   current_time         = 0 ;
   factor               = 1 ;

   if(tsc->verbosity & TSCMOD_DBG_SCAN) {
      factor = 4 ;  /* slow down timeout for debug */
   } else {  tsc->verbosity = 0 ; }

   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      lane_s = 0 ; lane_e = 4 ;
   } else if (tsc->port_type == TSCMOD_DXGXS ) {
      if((tsc->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
      else { lane_s = 0; lane_e = 2 ; }
   } else {
      lane_s = tsc->this_lane ; lane_e = lane_s + 1 ;
   }

   pmd_llocked = 1 ;  sig_ldet = 1;
   for(lane=lane_s; lane<lane_e; lane++) {
      tsc->lane_select = _tscmod_get_lane_select(unit, tsc->port, lane) ;
      tsc->this_lane   = lane ;

      tsc->per_lane_control = TSCMOD_DIAG_PMD_INFO_LATCH ;
      tsc->diag_type        = TSCMOD_DIAG_PMD ;
      tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
      if(tsc->verbosity & TSCMOD_DBG_SUB)
         printf("%-22s: u=%0d p=%0d WAIT_PMD_LOCK l=%0d lane_sel=%x this_lane=%0d lock=%0d\n",
                FUNCTION_NAME(), unit, port, lane, tsc->lane_select, tsc->this_lane, tsc->accData);
      if((tsc->accData & TSCMOD_DIAG_PMD_INFO_LOCK_BIT)==0) {
         pmd_llocked = 0 ;
         /* don't use break because of LATCH version through 4 lanes, or call this func for 4 times to clear 4 lacthed low */
      }
      if((tsc->accData & TSCMOD_DIAG_PMD_INFO_SDET_BIT)==0) {
         sig_ldet = 0 ;
         /* don't use break because of LATCH version through 4 lanes, or call this func for 4 times to clear 4 lacthed low */
      }
   }

   tsc->lane_select     = tmp_sel ;
   tsc->this_lane       = tmp_lane   ;

   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      tsc->lane_select      = TSCMOD_LANE_BCST ;
   }

   if(tsc->verbosity &(TSCMOD_DBG_LINK|TSCMOD_DBG_PATH))
       printf("%-22s: u=%0d p=%0d pcs_l=%0d pmd_lck=%0d sig_ldet=%0d state=%x"
         " current_usc=%0x cnt=%0d usec=%0x\n",
              FUNCTION_NAME(), tsc->unit, tsc->port, *link, pmd_llocked, sig_ldet, DEV_CFG_PTR(pc)->sw_rx_los.state,
              sal_time_usecs(), DEV_CFG_PTR(pc)->sw_rx_los.count, DEV_CFG_PTR(pc)->sw_rx_los.start_time) ;

   switch(DEV_CFG_PTR(pc)->sw_rx_los.state) {
   case RXLOS_RESET:
      if(DEV_CFG_PTR(pc)->sw_rx_los.count++==0)
         DEV_CFG_PTR(pc)->sw_rx_los.start_time = sal_time_usecs() + (TSCMOD_RXLOS_TIMEOUT2 << factor) ;

      tsc->per_lane_control = 0xa ;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      rxp = tsc->accData;

      *link = FALSE ;
      if(pmd_llocked) {
         tsc->per_lane_control = 1 ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv) ;

         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_LINK_WAIT_P1 ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
      } else {
         current_time = sal_time_usecs() ;
         if( rxp == 1 ) {
            tsc->per_lane_control = 0;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* diable RX path */
         }

         if(SAL_USECS_SUB(current_time,DEV_CFG_PTR(pc)->sw_rx_los.start_time)>=0) {
            tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM ;
            tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

            if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
               sal_usleep(1000) ;
               tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
               tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
            }

            DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
            DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
         }
      }
      break ;
   case RXLOS_LINK_WAIT_P1:
      if(DEV_CFG_PTR(pc)->sw_rx_los.count ++ ==0)
         DEV_CFG_PTR(pc)->sw_rx_los.start_time = sal_time_usecs() + (TSCMOD_RXLOS_TIMEOUT1 << factor) ;

      if((*link !=FALSE)&&pmd_llocked) {
         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_INITIAL_LINK_UP ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
      } else {
         current_time = sal_time_usecs() ;
         if((SAL_USECS_SUB(current_time,DEV_CFG_PTR(pc)->sw_rx_los.start_time)>=0)) {
            tsc->per_lane_control = 0;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* diable RX path */

            tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM ;
            tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

            if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
               sal_usleep(1000) ;
               tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
               tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
            }

            DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
            DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
         } else if(pmd_llocked==0) {
            tsc->per_lane_control = 0;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* diable RX path */

            DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
            DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
         }
      }
      *link = FALSE ;
      break ;
   case RXLOS_INITIAL_LINK_UP:
      tsc->per_lane_control = 0;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* diable RX path */
      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
         sal_usleep(1000) ;
         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }
      DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESTART ;
      DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
      DEV_CFG_PTR(pc)->sw_rx_los.start_time = sal_time_usecs() + (TSCMOD_RXLOS_TIMEOUT1 << factor) ;
      *link = FALSE ;
      break ;
   case RXLOS_RESTART:
      if(pmd_llocked) DEV_CFG_PTR(pc)->sw_rx_los.count++ ;
      else            DEV_CFG_PTR(pc)->sw_rx_los.count=0 ;
      if(DEV_CFG_PTR(pc)->sw_rx_los.count>=2) {
         tsc->per_lane_control = 1 ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* enable RX path */

         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_LINK_WAIT_P2 ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
      } else {
         current_time = sal_time_usecs() ;
         if((SAL_USECS_SUB(current_time,DEV_CFG_PTR(pc)->sw_rx_los.start_time)>=0)||(sig_ldet==0)){
            tsc->per_lane_control = 0;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* diable RX path */

            DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
            DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
         }
      }
      *link = FALSE ;
      break ;
   case RXLOS_LINK_WAIT_P2:
      if(DEV_CFG_PTR(pc)->sw_rx_los.count ++ ==0)
         DEV_CFG_PTR(pc)->sw_rx_los.start_time = sal_time_usecs() + (TSCMOD_RXLOS_TIMEOUT1 << factor) ;

      if((*link != FALSE)&&pmd_llocked) {
         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_LINK_UP ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
      } else {
         current_time = sal_time_usecs() ;
         if((SAL_USECS_SUB(current_time,DEV_CFG_PTR(pc)->sw_rx_los.start_time)>=0)||(pmd_llocked==0)){
            tsc->per_lane_control = 0;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* diable RX path */

            DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
            DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;
         }
         *link = FALSE ;
      }
      if(tsc->verbosity &(TSCMOD_DBG_LINK|TSCMOD_DBG_PATH))
       printf("%-22s: u=%0d p=%0d pmd_lck=%0d sig_ldet=%0d state=%x"
         " current_usc=%0x cnt=%0d usec=%0x\n",
              FUNCTION_NAME(), tsc->unit, tsc->port, pmd_llocked, sig_ldet, DEV_CFG_PTR(pc)->sw_rx_los.state,
              sal_time_usecs(), DEV_CFG_PTR(pc)->sw_rx_los.count, DEV_CFG_PTR(pc)->sw_rx_los.start_time) ;
      break ;
   case RXLOS_LINK_UP:
      if((*link==FALSE)||(pmd_llocked==0)) {
         tsc->per_lane_control = 0;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* diable RX path */

         DEV_CFG_PTR(pc)->sw_rx_los.state      = RXLOS_RESET ;
         DEV_CFG_PTR(pc)->sw_rx_los.count      = 0 ;

         *link = FALSE ;
      }
      break ;
   default:
      DEV_CFG_PTR(pc)->sw_rx_los.state = RXLOS_RESET ;
      DEV_CFG_PTR(pc)->sw_rx_los.count = 0 ;
      break ;
   }

   tsc->lane_select     = tmp_sel ;
   tsc->this_lane       = tmp_lane   ;
   return SOC_E_NONE;
}

/*
 *      phy_tscmod_link_get
 * Purpose:
 *      Get layer2 connection status.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      link - address of memory to store link up/down state.
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_tscmod_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t *pc;
    tscmod_st  *tsc ;
    TSCMOD_DEV_DESC_t *pDesc;
    int rv, tmp_verb ;
    TSCMOD_DEV_CFG_t  *pCfg;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);
    pCfg  = &pDesc->cfg;

   tmp_verb = tsc->verbosity ;
   if(tsc->verbosity & TSCMOD_DBG_SCAN) {
   } else {
      if(tsc->verbosity & TSCMOD_DBG_FSM) tsc->verbosity =TSCMOD_DBG_FSM;
      else                                tsc->verbosity = 0 ;
   }

   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;

   /* preventive care */
   if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR) {
      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY_BLK_ADR ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
   }

   tsc->diag_type        = TSCMOD_DIAG_LINK;
   tsc->per_lane_control = TSCMOD_DIAG_LINK_LATCH ;
   if(TSCMOD_MODEL_PROJ_GET(tsc)==TSCMOD_MODEL_PROJ_HR2) {
      /* SDK-49139/CRTSC-742 */
      if((pCfg->uc_ver >= 0xa034)&&
         ((tsc->an_type==TSCMOD_CL37)||(tsc->an_type==TSCMOD_CL37_10G))&&
         ((pCfg->cl37an==TSCMOD_CL37_HR2SPM)||(pCfg->cl37an==TSCMOD_CL37_HR2SPM_W_10G))) {
         tsc->per_lane_control = TSCMOD_DIAG_LINK_LIVE ;
      }
   }
   tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
   *link = tsc->accData;

   if (tsc->verbosity & TSCMOD_DBG_LINK)
      printf("%-22s: u=%0d p=%0d link=%0d verb=%x %x\n", FUNCTION_NAME(), unit, port, *link, tsc->verbosity, tmp_verb) ;

   tsc->verbosity = tmp_verb ;  /* restore */

   if(*link==0) {
      /* link down now */
      if(tsc->ctrl_type & (TSCMOD_CTRL_TYPE_LB|TSCMOD_CTRL_TYPE_PRBS_ON|TSCMOD_CTRL_TYPE_RMT_LB)) {
         /* do nothing: PMD lock is not obtainable for LB, so RX LANE is always on */
      } else if(tsc->an_type ==TSCMOD_AN_TYPE_ILLEGAL ||
                tsc->an_type ==TSCMOD_AN_NONE ) {
         if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_UC_RXP ) {
            /* need to consider FORCE_CL72_ENABLED and DEV_CFG_PTR(pc)->sw_rx_los.enable */
            if(FORCE_CL72_ENABLED(pc)) {
               if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_UC_CL72_FW )
                  rv = _phy_tscmod_force_cl72_sw_link_uc_rxp(unit,port, link);
               else
                  rv = _phy_tscmod_force_cl72_sw_link_recovery(unit,port, *link);
            } else if(DEV_CFG_PTR(pc)->sw_rx_los.enable) {
               rv = phy_tscmod_sw_rx_los_uc_link_handler(unit, port, link) ;
            } else {
               rv =  _phy_tscmod_port_lkuc_rxp_handler(unit, port, link) ;
            }
         } else {
            if(FORCE_CL72_ENABLED(pc)) {
               rv = _phy_tscmod_force_cl72_sw_link_recovery(unit,port,*link);
            } else if(DEV_CFG_PTR(pc)->sw_rx_los.enable) {
               rv = phy_tscmod_sw_rx_los_link_handler(unit, port, link) ;
            } else {
               rv = _phy_tscmod_port_lkdn_pmd_lock_handler(unit, port, 1, 1) ;
            }
         }
      } else {
         /* for AN, RX lane control is always set */
         rv = _phy_tscmod_an_war_handler(unit, port, link) ;
      }
   } else {
      if(tsc->ctrl_type & (TSCMOD_CTRL_TYPE_LB|TSCMOD_CTRL_TYPE_PRBS_ON|TSCMOD_CTRL_TYPE_RMT_LB)) {
         /* do nothing: PMD lock is not obtainable, so RX LANE is always on */
      } else if(tsc->an_type ==TSCMOD_AN_TYPE_ILLEGAL ||
         tsc->an_type ==TSCMOD_AN_NONE ) {
         if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_UC_RXP ) {
             /* need to consider FORCE_CL72_ENABLED and DEV_CFG_PTR(pc)->sw_rx_los.enable */
            if(FORCE_CL72_ENABLED(pc)) {
               if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_UC_CL72_FW )
                  rv = _phy_tscmod_force_cl72_sw_link_uc_rxp(unit,port,link);
               else
                  rv = _phy_tscmod_force_cl72_sw_link_recovery(unit,port,*link);
            } else if(DEV_CFG_PTR(pc)->sw_rx_los.enable) {
               rv = phy_tscmod_sw_rx_los_uc_link_handler(unit, port, link) ;
            } else {
               rv =  _phy_tscmod_port_lkuc_rxp_handler(unit, port, link) ;
            }
         } else {
            if(FORCE_CL72_ENABLED(pc)) {
               rv = _phy_tscmod_force_cl72_sw_link_recovery(unit,port,*link);
            } else if(DEV_CFG_PTR(pc)->sw_rx_los.enable) {
               rv = phy_tscmod_sw_rx_los_link_handler(unit, port, link) ;
            } else {
               rv = _phy_tscmod_port_lkup_pmd_lock_handler(unit, port, link) ;
               if(*link)
                  TSCMOD_SP_CNT(pc)=0 ;
            }
         }
      } else {
         /* for AN, RX lane control is always set  state */
         rv = _phy_tscmod_an_war_handler(unit, port, link) ;
      }
   }

   tscmod_sema_unlock(unit, port) ;
   return rv ;

}


/*
 * Function:
 *      phy_tscmod_enable_set
 * Purpose:
 *      Enable/Disable phy
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - on/off state to set
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_tscmod_enable_set(int unit, soc_port_t port, int enable)
{
    int rv;
    phy_ctrl_t  *pc;
    tscmod_st  *tsc;
    TSCMOD_DEV_DESC_t *pDesc;
    int         spd_intf ;
    int         tmp_sel, tmp_lane, tmp_dxgxs;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   = (tscmod_st *)( pDesc + 1);

    tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;

    rv    = SOC_E_NONE ;
    spd_intf = TSCMOD_SPD_10_SGMII;

    tmp_sel              = tsc->lane_select ;
    tmp_lane             = tsc->this_lane   ;
    tmp_dxgxs            = tsc->dxgxs ;

    /* clean up previous states */
    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
       tsc->lane_select      = TSCMOD_LANE_BCST ;
    }

    if((tsc->verbosity &(TSCMOD_DBG_LINK|TSCMOD_DBG_PATH)))
       printf("%-22s: u=%0d p=%0d enable=%0d sel=%0x ln=%0d dxgxs=%0d f_cl72_en=%0d an=%0d\n", FUNCTION_NAME(),
              tsc->unit, tsc->port, enable, tsc->lane_select, tsc->this_lane, tsc->dxgxs, FORCE_CL72_ENABLED(pc), tsc->an_type) ;

    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    /* preventive care */
    if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR) {
       tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY_BLK_ADR ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
    }

    if(enable) {
       if(FORCE_CL72_MODE(pc)) {
          FORCE_CL72_ENABLED(pc) = 1 ;
          FORCE_CL72_TICK_CNT(pc)= 0 ;
          if((FORCE_CL72_STATE(pc) == TSCMOD_FORCE_CL72_IDLE)&&
             (tsc->ctrl_type&TSCMOD_CTRL_TYPE_CL72_WAR_ON)) {
              FORCE_CL72_STATE(pc)    = TSCMOD_FORCE_CL72_CMD_DIS_IDLE  ;
          } 
       }

       if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_LINK_DIS) {
          /* SDK-46338
          tsc->per_lane_control = 0;
          tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

          tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
          tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

          tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
          tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
          */
          
          /* CL72 HW is about to be disable */
          FORCE_CL72_STATE(pc)    = TSCMOD_FORCE_CL72_CMD_DIS_IDLE  ;

          /* A2 CL72 clean up */
          tsc->per_lane_control = TSCMOD_CL72_HW_DISABLE ;
          tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

          tsc->per_lane_control = TSCMOD_CL72_STATUS_RESET ;
          tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

          tsc->per_lane_control = TSCMOD_CL72_HW_RESTART ;
          tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

          /* TSCOP_007: */
          tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

          sal_usleep(1000) ;
          if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
             tsc->lane_select      = TSCMOD_LANE_BCST ;
          }

          if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
             /* this call affects AN */
             tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM ;
             tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

             sal_usleep(1000) ;
             if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
                tsc->lane_select      = TSCMOD_LANE_BCST ;
             }
          }
       }

       /* PHY-1116 */
       tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

       if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY) {
          tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
       }

       if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_TX_SQUELCH)) {
           /* TSCOP_008: */
           tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
           tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
           
           tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
           tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
           sal_usleep(1000) ;
           
           if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
               tsc->lane_select      = TSCMOD_LANE_BCST ;
           }
       }
       /* TSCOP-004: loopback and AN only */
       if((tsc->an_type != TSCMOD_AN_NONE)||(tsc->ctrl_type&TSCMOD_CTRL_TYPE_LB)) {
          if(tsc->an_type != TSCMOD_AN_NONE) {
             /* Due to SDK 48705, no need for this toggle */
             /* Due to PHY-1105, we need this toggle */
             /* remove TSCMOD_MODEL_REVID_A2 condition due to HR2 SDK-54943 */
              tsc->lane_select = tmp_sel  ;  /* restore */
              tsc->this_lane   = tmp_lane ;
              tsc->dxgxs       = 0 ;
              
              tscmod_tier1_selector("AUTONEG_CONTROL", tsc, &rv);
              
              tsc->per_lane_control = TSCMOD_MISC_CTL_AN_RESTART ;
              tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);
          }
          if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
              tsc->lane_select      = TSCMOD_LANE_BCST ;
          }
          tsc->dxgxs       = tmp_dxgxs ;

          if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
             tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
             tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
          }
          tsc->per_lane_control = TSCMOD_RXP_REG_ON ;
          tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

          if((tsc->ctrl_type&TSCMOD_CTRL_TYPE_LINK_DIS)&&
             (tsc->an_type == TSCMOD_AN_NONE) &&
             ((tsc->spd_intf==TSCMOD_SPD_10_SGMII)||
              (tsc->spd_intf==TSCMOD_SPD_100_SGMII))) {
              spd_intf = tsc->spd_intf ;

              tsc->spd_intf = TSCMOD_SPD_1000_SGMII ;
              tsc->per_lane_control = 1 ;
              tscmod_tier1_selector("ENCODE_SET", tsc, &rv);
              
              tsc->per_lane_control = 1 ;
              tscmod_tier1_selector("DECODE_SET", tsc, &rv);
              
              tsc->per_lane_control = 1 ;
              tscmod_tier1_selector("CREDIT_SET", tsc, &rv);
              
              tsc->per_lane_control = 1;
              tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
              
              /* back to 10M or 100M */
              tsc->spd_intf = spd_intf ;
              tsc->per_lane_control = 1 ;
              tscmod_tier1_selector("ENCODE_SET", tsc, &rv);
              
              tsc->per_lane_control = 1 ;
              tscmod_tier1_selector("DECODE_SET", tsc, &rv);
              
              tsc->per_lane_control = 1 ;
              tscmod_tier1_selector("CREDIT_SET", tsc, &rv);
              
              tsc->per_lane_control = 1;
              tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);               
              
          } else { 
              tsc->per_lane_control = 1;
              tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
              
              if(tsc->an_type != TSCMOD_AN_NONE) {
                  
              } else {
                  /* no credit set for AN */
                  tsc->per_lane_control = 1;
                  tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
              }
          }
       } else {
           if((tsc->ctrl_type&TSCMOD_CTRL_TYPE_LINK_DIS)&&
              ((tsc->spd_intf==TSCMOD_SPD_10_SGMII)||
               (tsc->spd_intf==TSCMOD_SPD_100_SGMII))) {
               spd_intf = tsc->spd_intf ;

               tsc->spd_intf = TSCMOD_SPD_1000_SGMII ;
               tsc->per_lane_control = 1 ;
               tscmod_tier1_selector("ENCODE_SET", tsc, &rv);
               
               tsc->per_lane_control = 1 ;
               tscmod_tier1_selector("DECODE_SET", tsc, &rv);
               
               tsc->per_lane_control = 1 ;
               tscmod_tier1_selector("CREDIT_SET", tsc, &rv);
          
               tsc->per_lane_control = 1;
               tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

               /* back to 10M or 100M */
               tsc->spd_intf = spd_intf ;
               tsc->per_lane_control = 1 ;
               tscmod_tier1_selector("ENCODE_SET", tsc, &rv);
               
               tsc->per_lane_control = 1 ;
               tscmod_tier1_selector("DECODE_SET", tsc, &rv);

               tsc->per_lane_control = 1 ;
               tscmod_tier1_selector("CREDIT_SET", tsc, &rv);

               tsc->per_lane_control = 1;
               tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);               

           } else {               
               tsc->per_lane_control = 1;
               tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
               
               /* no credit set for AN */
               tsc->per_lane_control = 1;
               tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
           }
           
           if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
               tsc->per_lane_control = TSCMOD_RXP_UC_ON ;
               tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
           }
       }

       if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_LINK_DIS)
          tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_LINK_DIS ;
       
    } else {
       FORCE_CL72_ENABLED(pc) = 0 ;
       FORCE_CL72_STATE(pc)       = TSCMOD_FORCE_CL72_IDLE ;
       FORCE_CL72_RESTART_CNT(pc) = 0 ;

       if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
          tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
          tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
       }
       tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
       tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
       
       if((tsc->spd_intf==TSCMOD_SPD_10_SGMII)||
          (tsc->spd_intf==TSCMOD_SPD_100_SGMII)) {           
           tsc->per_lane_control = 0;
           tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

           tsc->per_lane_control = 0 ;
           tscmod_tier1_selector("CREDIT_SET", tsc, &rv);

           tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
           tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
           
           tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
           tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

       } else {
           tsc->per_lane_control = 0;
           tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
           
           tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
           tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
           
           tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
           tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
       }

       sal_usleep(100) ;
       /* wait for sending out bits in fifo */
       if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
          tsc->lane_select      = TSCMOD_LANE_BCST ;
       }

       tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

       tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

       /* disable AN */
       tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_AN_MODES ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

       /* PHY-1116 */
       tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

       if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY) {
         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
       }

       /* SDK-46338
       tsc->per_lane_control = 1;
       tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
       */

       /* DSC SM reset */
       tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

       /* no LOS */
       tsc->ctrl_type |= TSCMOD_CTRL_TYPE_LINK_DIS ;
       if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_TX_SQUELCH) {
           tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_TX_SQUELCH ;
       }
    }
    rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_CTRL_TYPE) ;
    
    tsc->lane_select = tmp_sel  ;  /* restore */
    tsc->this_lane   = tmp_lane ;
    tsc->dxgxs       = tmp_dxgxs ;

    tscmod_sema_unlock(unit, port) ;
    return rv;
}

/*
 * Function:
 *      phy_tscmod_enable_get
 * Purpose:
 *      Get Enable/Disable status
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - address of where to store on/off state
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_tscmod_enable_get(int unit, soc_port_t port, int *enable)
{
    *enable = !PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE);

    return SOC_E_NONE;
}

/* if a port change speed with new plldiv */
STATIC int
_phy_tscmod_speed_set(int unit, soc_port_t port, int speed)
{
   phy_ctrl_t  *pc, *pc2;
   tscmod_st   *tsc, *tsc2;
   TSCMOD_DEV_DESC_t *pDesc, *pDesc2;
   TSCMOD_DEV_CFG_t  *pCfg;
   int         actual_spd_vec ;
   int         lane, lane_s, lane_e,  tmp_sel, tmp_lane, tmp_dxgxs ;
   int         rv, pll_mode_change, port_type_mode ;
   int         cnt, cl72_on[4], cl72_on_port ;
#if 0
   int         lane, pmd_locked;
#endif
   int         txdrv_inx ;  

   TSCMOD_TX_DRIVE_t tx_drv[NUM_LANES];

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   = (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;

   tmp_sel              = tsc->lane_select ;
   tmp_dxgxs            = tsc->dxgxs ;
   tmp_lane             = tsc->this_lane   ;

   actual_spd_vec = 0; 
   lane = 0 ; lane_s = 0 ; lane_e =0 ;
   cnt = 10 ;  cl72_on[0] = 0, cl72_on[1]=0, cl72_on[2]=0, cl72_on[3]=0 ;
   cl72_on_port = 0 ;
   port_type_mode = tsc->port_type ;

   /* clean up previous states */
   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      tsc->lane_select      = TSCMOD_LANE_BCST ;
   }

   /* preventive care */
   if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR) {
      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY_BLK_ADR ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
   }
   
   tsc->per_lane_control = TSCMOD_CL72_HW_DISABLE ;
   tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

   tsc->per_lane_control = TSCMOD_CL72_STATUS_RESET ;
   tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

   tsc->per_lane_control = TSCMOD_CL72_HW_RESTART ;
   tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

   if(FORCE_CL72_ENABLED(pc)) {
       
       FORCE_CL72_ENABLED(pc) = 0 ;
       FORCE_CL72_MODE(pc)    = 0 ;
       FORCE_CL72_STATE(pc)   = TSCMOD_FORCE_CL72_IDLE ;
       FORCE_CL72_RESTART_CNT(pc) = 0 ;
       
       tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_SWITCH_OVER, 0) ;
       tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
       tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

       tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
       tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
       
       tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_CLEAR, 1) ;
       tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_WAIT_CLEAN, 1) ;  
       
       tscmod_force_cl72_wait_uc_clr(tsc, 2) ;
   }
   
   FORCE_CL72_ENABLED(pc) = 0 ;
   FORCE_CL72_MODE(pc)    = 0 ;
   FORCE_CL72_STATE(pc)   = TSCMOD_FORCE_CL72_IDLE ;
   FORCE_CL72_RESTART_CNT(pc) = 0 ;
   rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_CL72_WAR) ;

   /* SDK-49465  */
   tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AN_MODES ;
   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

   _phy_tscmod_spd_selection(unit, port, speed, &actual_spd_vec, &txdrv_inx) ;

   /* now start the speed set */
   if(tsc->verbosity&(TSCMOD_DBG_SPD|TSCMOD_DBG_LINK))
     printf("%-22s start u=%0d(%0d) p=%0d(%0d) speed=%0d actual_spd_vec=%0x\n",
            FUNCTION_NAME(), unit, tsc->unit, port, tsc->port, speed, actual_spd_vec) ;

   if(speed<2500) {
      if(tsc->port_type != TSCMOD_MULTI_PORT) {
         tsc->port_type = TSCMOD_MULTI_PORT ;
      }
   }
   tsc->per_lane_control = 0;
   tscmod_tier1_selector("SET_SPD_INTF", tsc, &rv);
   if(speed<2500) {
      tsc->port_type = port_type_mode ;
   }

   pll_mode_change = 0 ;
   if( tsc->accData ) pll_mode_change =1 ;
   if( pll_mode_change ) {  /* re-program PLL required */
      int unit0; soc_port_t port0 ;
      tsc->plldiv = tsc->accData ;
      unit0 = unit ; port0 = port ;
      /* every port in the same core has the same plldiv */
      PBMP_ALL_ITER(unit0, port0) {
         pc2 = INT_PHY_SW_STATE(unit0, port0);
         if(tsc->verbosity&TSCMOD_DBG_INIT)
            printf("%-22s PBMP_ALL_ITER u=%0d p=%0d scan u=%0d p=%0d \n", FUNCTION_NAME(), unit, port, unit0, port0) ;
         if (pc2 == NULL) {
            continue;
         }
         if (!pc2->dev_name || pc2->dev_name != tscmod_device_name) {
            continue;
         }
         if (pc2->chip_num != pc->chip_num) {
            continue;
         }
         if ((pc2->phy_mode == pc->phy_mode)) {
            if(tsc->verbosity&TSCMOD_DBG_INIT)
               printf("%-22s PBMP_ALL_ITER u=%0d p=%0d scan u=%0d p=%0d corrected tsc->plldiv=%0d\n",
                   FUNCTION_NAME(), unit, port, unit0, port0, tsc->plldiv) ;
            pDesc2 = (TSCMOD_DEV_DESC_t *)(pc2 + 1);
            tsc2   = (tscmod_st *)( pDesc2 + 1);
            tsc2->plldiv = tsc->plldiv ;
         }
      }

      tsc->lane_select     = TSCMOD_LANE_BCST ;

      tsc->per_lane_control = 0;
      tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

      tsc->per_lane_control = 0 ;
      tscmod_tier1_selector("CREDIT_SET", tsc, &rv);
      
      tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
      
      tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      sal_usleep(100) ; /* drain bits */
      tsc->lane_select     = TSCMOD_LANE_BCST ;

      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
          tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
      }
      tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_AFE_ANALOG ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      /* set rxSeqStart_AN_disable */
      tsc->per_lane_control = 1;
      tscmod_tier1_selector("AFE_RXSEQ_START_CONTROL", tsc, &rv);

      /* DSC SM reset */
      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         tsc->lane_select      = TSCMOD_LANE_BCST ;
      }

      tsc->lane_select      = getLaneSelect(tmp_lane) ;
      tsc->dxgxs            = 0;
      tsc->per_lane_control = 0;
      tscmod_tier1_selector("PLL_SEQUENCER_CONTROL", tsc, &rv);

      tscmod_tier1_selector("SET_PORT_MODE", tsc, &rv);

      sal_usleep(1000);
      if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         tsc->lane_select      = TSCMOD_LANE_BCST ;
      }
      tsc->dxgxs            = tmp_dxgxs ;

      tsc->per_lane_control = TSCMOD_MISC_CTL_PORT_CLEAN_UP ;
      tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);

      if(pCfg->los_usec>4000) {
          sal_usleep(pCfg->los_usec-4000);
      }

      tsc->lane_select      = getLaneSelect(tmp_lane) ;
      tsc->dxgxs            = 0 ;
      tsc->per_lane_control = 1;
      tscmod_tier1_selector("PLL_SEQUENCER_CONTROL", tsc, &rv);

      if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         tsc->lane_select      = TSCMOD_LANE_BCST ;
      }
      tsc->dxgxs            = tmp_dxgxs ;
      /* sal_usleep(1000); WAR on PHY-1073 */

      tsc->lane_select      = TSCMOD_LANE_0_0_0_1 ;
      tscmod_tier1_selector("PLL_LOCK_WAIT", tsc, &rv);

      if(1) {
         tsc->lane_select     = TSCMOD_LANE_BCST ;
         /* deassert afe reset */
         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_AFE_ANALOG ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         /* 1 ms delay to wait for Analog to settle */
         sal_usleep(1000);
      }

      tsc->this_lane       = tmp_lane ;
      tsc->dxgxs           = tmp_dxgxs ;
      tsc->lane_select     = tmp_sel ;
   }

   if(speed<2500) {
      if(tsc->port_type != TSCMOD_MULTI_PORT) {
         tsc->port_type = TSCMOD_MULTI_PORT ;
         tsc->per_lane_control = TSCMOD_MISC_CTL_PORT_MODE_SELECT ;
         tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);
         tsc->port_type = port_type_mode ;
      }
   }

   /* per port */
   if(tsc->port_type == TSCMOD_SINGLE_PORT) {
      tsc->lane_select     = TSCMOD_LANE_BCST ;
   } else {
      tsc->lane_select     = _tscmod_get_lane_select(unit, port, tmp_lane) ;
   }

   tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

   if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY) {
      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
   }

   /* gracefully stop uC */
   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         lane_s = 0 ; lane_e = 4 ;
   } else if (tsc->port_type == TSCMOD_DXGXS ) {
      if((tsc->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
      else { lane_s = 0; lane_e = 2 ; }
   } else {
      lane_s = tsc->this_lane ; lane_e = lane_s + 1 ;
   }
   for(lane=lane_s; lane<lane_e; lane++) {
      if(TSCMOD_MODEL_REVID_GET(tsc)<TSCMOD_MODEL_REVID_A2)
         tscmod_uc_cmd_seq(unit, port, lane, 0) ;
   }

   tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

   if(pll_mode_change==0) {
      /* will not reset TSCMOD_SRESET_AFE_ANALOG since pll change is not required */

      /* set rxSeqStart_AN_disable */
      tsc->per_lane_control = 1;
      tscmod_tier1_selector("AFE_RXSEQ_START_CONTROL", tsc, &rv);

      tsc->per_lane_control = 0;
      tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

      tsc->per_lane_control = 0 ;
      tscmod_tier1_selector("CREDIT_SET", tsc, &rv);

      tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      sal_usleep(100) ;  /* drain bits */
      if( tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         tsc->lane_select     = TSCMOD_LANE_BCST ;
      }

      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
         tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }
      tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      
      if(pCfg->los_usec>4000) {
          sal_usleep(pCfg->los_usec-4000) ;
      }
   }

   if(tsc->verbosity&TSCMOD_DBG_SPD)
      printf("%-22s: u=%0d p=%0d speed=%0d pre actual_spd_vec=%0x\n",
             FUNCTION_NAME(), unit, port, speed, actual_spd_vec) ;
   _phy_tscmod_spd_selection(unit, port, speed, &actual_spd_vec, &txdrv_inx) ;

   if(tsc->verbosity&TSCMOD_DBG_SPD)
      printf("%-22s: u=%0d p=%0d speed=%0d post actual_spd_vec=%0x\n",
             FUNCTION_NAME(), tsc->unit, tsc->port, speed, actual_spd_vec) ;


   if(speed<2500) {
      if(tsc->port_type != TSCMOD_MULTI_PORT) {
         tsc->port_type = TSCMOD_MULTI_PORT ;
      }
   }
   tsc->per_lane_control = 3 | (actual_spd_vec <<4) ;
   tscmod_tier1_selector("SET_SPD_INTF", tsc, &rv);

   if(pCfg->am_reserved&&(tsc->spd_intf!=TSCMOD_SPD_10000_XFI)) {
       if(tsc->spd_intf==TSCMOD_SPD_20G_MLD_DXGXS) {
           tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_HP_BAM_AM ;
           tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
       } else {
           tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_HP_BAM_AM ;
           tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
       }
   }

   /* if speed 100M/10M, at this point, 1G is programmed */
   if(speed<2500) {
      tsc->port_type = port_type_mode ;
   }

   /* update plldiv for other port in the same core to prevent another PLL seq
      for the other port*/
   if( tsc->accData ) {
      tsc->ctrl_type |= TSCMOD_CTRL_TYPE_ERR ;
      tsc->err_code  |= TSCMOD_ERR_PLLDIV_SPD_SET ;
      if(tsc->verbosity & (TSCMOD_DBG_PRINT|TSCMOD_DBG_INIT|TSCMOD_DBG_SPD)) {
         printf("Error: u=%0d p=%0d plldiv is not set correctly current plldiv=%0d\
 new plldiv=%0d pll_mode_change=%0d\n", tsc->unit, tsc->port, tsc->plldiv, tsc->accData, pll_mode_change);
      }
   }

   if(FORCE_CL72_MODE(pc)&&
      !((tsc->ctrl_type &TSCMOD_CTRL_TYPE_LB)||
        (((tsc->spd_intf == TSCMOD_SPD_10000_XFI)||
          (tsc->spd_intf == TSCMOD_SPD_10600_XFI_HG)) &&
         actual_spd_vec==TSCMOD_ASPD_10G_X1))) {
       txdrv_inx = TXDRV_AN_INX ;

       if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_CL72_FW) {
           FORCE_CL72_ENABLED(pc) = 1 ;
       }
           
   }
   SOC_IF_ERROR_RETURN
      (_phy_tscmod_tx_control_get(unit, port, &tx_drv[0], txdrv_inx));
   SOC_IF_ERROR_RETURN
      (_phy_tscmod_tx_control_set(unit, port, &tx_drv[0]));


   tsc->per_lane_control = 0 ;
   tsc->per_lane_control = TSCMOD_PDET_CONTROL_1G | TSCMOD_PDET_CONTROL_10G |
                           TSCMOD_PDET_CONTROL_CMD_MASK;
   tscmod_tier1_selector("PARALLEL_DETECT_CONTROL", tsc, &rv);


   if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_CL72_TR_DIS) ) {
      if(FORCE_CL72_MODE(pc)) {
         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_FIRMWARE_MODE ;
         /* tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_CL72_FW_MODE ; */
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_CL72_LINK_FAIL_TIMER ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_CL72_MAX_WAIT_TIMER ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      } else {
         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_CL72_LINK_FAIL_TIMER ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         /*
         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_CL72_MAX_WAIT_TIMER ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
         */
      }
   }

   if(FORCE_CL72_ENABLED(pc)) {  /* SDK-52713  */
      /* do nothing or it will triger CL72 by TSCMOD_SRESET_DSC_SM */
   } else {
      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      sal_usleep(1000) ;
   }
   if( tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      tsc->lane_select     = TSCMOD_LANE_BCST ;
   }

   /* gracefully resume */
   for(lane=lane_s; lane<lane_e; lane++) {
      if(TSCMOD_MODEL_REVID_GET(tsc)<TSCMOD_MODEL_REVID_A2)
         tscmod_uc_cmd_seq(unit, port, lane, 2) ;
   }
   
   if(FORCE_CL72_ENABLED(pc)) {
       /* remove RX_SIG_DET reset at the end */
   } else {
       if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_LINK_DIS)) {
           tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
           tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
           
           if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY) {
               tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY ;
               tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
           }
       }
   }

   if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
      /* DSC SM on */
      tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
   }
   sal_usleep(1000) ;
   if( tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      tsc->lane_select     = TSCMOD_LANE_BCST ;
   }

   if(tsc->ctrl_type &TSCMOD_CTRL_TYPE_LB) { /* for Gloopback, RXP enabled first */
      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
         tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }
      tsc->per_lane_control = TSCMOD_RXP_REG_ON ;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
   }

 if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_CL72_TR_DIS) ) {
    if(FORCE_CL72_MODE(pc) &&
       !((tsc->ctrl_type &TSCMOD_CTRL_TYPE_LB)||
         (((tsc->spd_intf == TSCMOD_SPD_10000_XFI)||
          (tsc->spd_intf == TSCMOD_SPD_10600_XFI_HG)) &&
              actual_spd_vec==TSCMOD_ASPD_10G_X1)))    /* exclude XFI mode */
    {
      tsc->per_lane_control = 0;
      tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

      tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
          tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
          tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }
      tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_TAP_MUXSEL ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      tsc->per_lane_control = TSCMOD_CL72_HW_ENABLE ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      tsc->per_lane_control = TSCMOD_CL72_STATUS_RESET ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_CL72_FW) {
          
         tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_SWITCH_OVER, 1) ;

         FORCE_CL72_ENABLED(pc) = 1 ;
         FORCE_CL72_STATE(pc) = TSCMOD_FORCE_CL72_TXP_ENABLED ;
         rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_CL72_WAR) ;

         tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

         tsc->per_lane_control = TSCMOD_RXP_REG_ON ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

         tsc->per_lane_control = TSCMOD_RXP_UC_ON ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }

      tsc->per_lane_control = TSCMOD_CL72_HW_RESTART ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      if(FORCE_CL72_ENABLED(pc)) {

      } else {
          tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

          if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
              sal_usleep(1000) ;
              if( tsc->port_type   == TSCMOD_SINGLE_PORT ) {
                  tsc->lane_select     = TSCMOD_LANE_BCST ;
              }
              
              tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
              tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
              sal_usleep(1000) ;
          }

          if( tsc->port_type   == TSCMOD_SINGLE_PORT ) {
              tsc->lane_select     = TSCMOD_LANE_BCST ;
          }
          
          if(!(tsc->ctrl_type & (TSCMOD_CTRL_TYPE_LINK_DIS|TSCMOD_CTRL_TYPE_TX_SQUELCH))) {
              tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
              tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
              
              tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
              tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
              
              sal_usleep(1000) ;
          }
      }
          
      if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         tsc->lane_select      = TSCMOD_LANE_BCST ;
      }

      tsc->per_lane_control = 1;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      tsc->per_lane_control = 1;
      tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

      if(FORCE_CL72_ENABLED(pc)) {
          if(!(tsc->ctrl_type & (TSCMOD_CTRL_TYPE_LINK_DIS|TSCMOD_CTRL_TYPE_TX_SQUELCH))) {
              tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
              tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
              
              tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
              tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
          }
          if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_LINK_DIS)) {
              tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
              tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
              
              if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY) {
               tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY ;
               tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
              }
          }
      }

      if(!(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_CL72_FW)){
         if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
            tsc->per_lane_control = TSCMOD_RXP_UC_ON ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
         }
      }

      if(!(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_CL72_FW)) {
         /* no RXP enabled */
         if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
            lane_s = 0 ; lane_e = 4 ;
         } else if (tsc->port_type == TSCMOD_DXGXS ) {
            if((tsc->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
            else { lane_s = 0; lane_e = 2 ; }
         } else {
            lane_s = tsc->this_lane ; lane_e = lane_s + 1 ;
         }

         cnt = 5 ;
         while((cnt--)>0) {
            for(lane=lane_s; lane<lane_e; lane++) {
               tsc->lane_select = _tscmod_get_lane_select(unit, tsc->port, lane) ;
               tsc->this_lane   = lane ;
               tsc->diag_type = TSCMOD_DIAG_CL72;
               tsc->per_lane_control = TSCMOD_DIAG_CL72_START ;
               tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
            if(tsc->accData) cl72_on[lane]=1 ;
            }
            cl72_on_port = 1 ;
            for(lane=lane_s; lane<lane_e; lane++) {
               if(cl72_on[lane]==0) {
                  cl72_on_port = 0 ;
                  break ;
               }
            }
         }
         if(cl72_on_port==0) {
            printf("%-22s: Error: u=%0d p=%0d cl72 is not started gracefully\n",
                   FUNCTION_NAME(), tsc->unit, tsc->port) ;
         }
         /* always CL72 is on already now */
         FORCE_CL72_ENABLED(pc) = 1 ;
         FORCE_CL72_STATE(pc)   = TSCMOD_FORCE_CL72_WAIT_TRAINING_LOCK ;
      }
    }  /* CL72 on */
    else {
      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_CL72_FW)
         tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_SWITCH_OVER, 0) ;

      tsc->per_lane_control = TSCMOD_CL72_HW_DISABLE ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      FORCE_CL72_ENABLED(pc) = 0 ;
      FORCE_CL72_STATE(pc)   = TSCMOD_FORCE_CL72_IDLE ;

      if((tsc->spd_intf==TSCMOD_SPD_10_SGMII)||
         (tsc->spd_intf==TSCMOD_SPD_100_SGMII)) {
        tsc->per_lane_control = 1;
        tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

        tsc->per_lane_control = 1 ;
        tscmod_tier1_selector("ENCODE_SET", tsc, &rv);
       
        tsc->per_lane_control = 1 ;
        tscmod_tier1_selector("DECODE_SET", tsc, &rv);
        
        tsc->per_lane_control = 1 ;
        tscmod_tier1_selector("CREDIT_SET", tsc, &rv);

        tsc->per_lane_control = 1;
        tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);   
        
      } else {
          tsc->per_lane_control = 1;
          tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
          
          tsc->per_lane_control = 1;
          tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
      }

      if(!(tsc->ctrl_type & (TSCMOD_CTRL_TYPE_LINK_DIS|TSCMOD_CTRL_TYPE_TX_SQUELCH))) {
          tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
          
          tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      
          sal_usleep(1000) ;
      }
      if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         tsc->lane_select      = TSCMOD_LANE_BCST ;
      }

      if(!(tsc->ctrl_type &TSCMOD_CTRL_TYPE_LB)&&(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP)) {
          tsc->per_lane_control = TSCMOD_RXP_UC_ON ;
          tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }
    } /* CL72 off */
 }  /* TSCMOD_CTRL_TYPE_CL72_TR_DIS==0 */
 else {
    /* TSCMOD_CTRL_TYPE_CL72_TR_DIS==1 */
    tsc->per_lane_control = TSCMOD_CL72_HW_DISABLE ;
    tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

    FORCE_CL72_ENABLED(pc) = 0 ;
    FORCE_CL72_STATE(pc)   = TSCMOD_FORCE_CL72_IDLE ;

    if(!(tsc->ctrl_type & (TSCMOD_CTRL_TYPE_LINK_DIS|TSCMOD_CTRL_TYPE_TX_SQUELCH))) {
        tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
        tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
        
        tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
        tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
        sal_usleep(1000) ;
    }

    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
       tsc->lane_select      = TSCMOD_LANE_BCST ;
    }

    tsc->per_lane_control = 1;
    tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

    tsc->per_lane_control = 1;
    tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

    if(!(tsc->ctrl_type &TSCMOD_CTRL_TYPE_LB)&&(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP)) {
       tsc->per_lane_control = TSCMOD_RXP_UC_ON ;
       tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
    }
 }

   tsc->lane_select = tmp_sel  ;  /* restore */
   tsc->dxgxs       = tmp_dxgxs ;
   tsc->this_lane   = tmp_lane ;

   if(tsc->verbosity&(TSCMOD_DBG_LINK))
      printf("%-22s end u=%0d(%0d) p=%0d(%0d) speed=%0d actual_spd_vec=%0x\n",
             FUNCTION_NAME(), unit, tsc->unit, port, tsc->port, speed, actual_spd_vec) ;

   return SOC_E_NONE;
}

/*
 * Function:
 *      phy_tscmod_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_tscmod_speed_set(int unit, soc_port_t port, int speed)
{
   int rv;
   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
   rv = _phy_tscmod_speed_set(unit,port,speed);
   tscmod_sema_unlock(unit, port) ;
   return rv;
}

STATIC int
_phy_tscmod_tx_fifo_reset(int unit, soc_port_t port,uint32 speed)
{

    return SOC_E_NONE;
}

/* 1. speed, asp_mode, scr (for scrambler) are outputs of the function */
/* 2. asp_mode is the enum type */
/* 3. intf is output or not changed */
/* 4. use firmware_mode, hg_mode, an_on, speed to decide interface type */
/*    last resort is to use cfg info */
/* 5. reserve KR4,CR4,KR for AN */
STATIC int
_phy_tscmod_speed_mode_decode(phy_ctrl_t *pc, int speed_mode, int hg_mode, int fw_mode, int an_on, int *speed, int *intf, int *asp_mode, int *scr)
{
   int idx ;
   *scr  = FALSE;
   *intf = SOC_PORT_IF_XGMII;  /* default to XGMII */

   for(idx=0; idx<CNT_tscmod_aspd_type; idx++) {
      if( e2n_tscmod_aspd_type[idx] == speed_mode ) {
         *asp_mode = idx ;
         break ;
      }
   }
   switch (*asp_mode) {
   case TSCMOD_ASPD_10M:
      *speed = 10 ;
      *intf  = SOC_PORT_IF_MII;
      break ;
   case TSCMOD_ASPD_100M :
      *speed = 100 ;
      *intf  = SOC_PORT_IF_MII;
      break;
   case TSCMOD_ASPD_1000M :
      *speed = 1000 ;
      *intf  = SOC_PORT_IF_GMII;
      break ;
   case TSCMOD_ASPD_2p5G_X1 :
      *speed = 2500 ;
      *intf  = SOC_PORT_IF_GMII;
      break ;
   case TSCMOD_ASPD_5G_X4 :
      *speed = 5000 ;
      *intf  = SOC_PORT_IF_GMII;
      break ;
   case TSCMOD_ASPD_6G_X4 :
      *speed = 6000 ;
      *intf  = SOC_PORT_IF_GMII;
      break ;
   case TSCMOD_ASPD_10G_X4:
      *speed = 10000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_10G_CX4:
      *speed = 10000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_12G_X4:
      *speed = 12000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_12p5G_X4:
      *speed = 12500 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_13G_X4:
      *speed = 13000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_15G_X4:
      *speed = 15000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_16G_X4:
      *speed = 16000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_1G_KX1:
      *speed = 1000 ;
      *intf  = SOC_PORT_IF_GMII;
      break ;
   case TSCMOD_ASPD_10G_KX4:
      *speed = 10000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_10G_KR1:
      *speed = 10000 ;
      *intf  = SOC_PORT_IF_KR;
      break ;
   case TSCMOD_ASPD_5G_X1:
      *speed = 5000 ;
      *intf  = SOC_PORT_IF_GMII;
      break ;
   case TSCMOD_ASPD_6p36G_X1:
      *speed = 6360 ;
      *intf  = SOC_PORT_IF_GMII;
      break ;
   case TSCMOD_ASPD_20G_CX4:
      *speed = 20000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_21G_X4:
      *speed = 21000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_25p45G_X4:
      *speed = 25000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_10G_X2_NOSCRAMBLE:
      *speed = 10000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_10G_CX2_NOSCRAMBLE:
      *speed = 10000 ;
      *intf  = SOC_PORT_IF_RXAUI ;
      break ;
   case TSCMOD_ASPD_10p5G_X2:
      *speed = 11000 ;    /* 10500 */
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_10p5G_CX2_NOSCRAMBLE:
      *speed = 11000 ;    /* 10500 */
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_12p7G_X2:
      *speed = 13000 ;   /* 12700 round up */
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_12p7G_CX2:
      *speed = 13000 ;   /* 12700 round up */
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_10G_X1:
      *speed = 10000 ;
      *intf  = SOC_PORT_IF_XFI;
      break ;
   case TSCMOD_ASPD_40G_X4:
      *speed = 40000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_20G_X2:
      *speed = 20000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_20G_CX2:
      *speed = 20000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_10G_SFI:
      *speed = 10000 ;
      if(fw_mode==TSCMOD_UC_CTRL_SFP_DAC) {
          if(an_on)
              *intf = SOC_PORT_IF_CR;
          else {
              if (DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_CR)
                  *intf = SOC_PORT_IF_CR;
              else
                  *intf = SOC_PORT_IF_SFI;
          }
      } else {
         if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_SFI) {
            *intf  = SOC_PORT_IF_SFI;
         } else if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_SR) {
            *intf  = SOC_PORT_IF_SR;
         } else if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_LR) {
            *intf  = SOC_PORT_IF_LR;
         } else if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_CR) {
            *intf  = SOC_PORT_IF_CR;
         } else {
            *intf  = SOC_PORT_IF_SFI;
         }
      }
      break ;
   case TSCMOD_ASPD_31p5G_X4:
      *speed = 32000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_32p7G_X4:
      *speed = 32700 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_20G_X4:
      *speed = 20000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_10G_X2:
      *speed = 10000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_10G_CX2:
      *speed = 10000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_12G_SCO_R2:
      *speed = 12000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_10G_SCO_X2:
      *speed = 10000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_40G_KR4:
      *speed = 40000 ;
      *intf = SOC_PORT_IF_XGMII ;
      if(hg_mode) {
         if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_KR4){
            /* HG 40G by default is for XGMII. XGMII is the only mode that will work for Higig AN. */
            if (!an_on) {
                *intf = SOC_PORT_IF_KR4;
            }
         }
      } else {
         if(fw_mode==TSCMOD_UC_CTRL_XLAUI) {
            *intf = SOC_PORT_IF_XLAUI ;
         } else if((fw_mode==TSCMOD_UC_CTRL_SR4)||
                   (fw_mode==TSCMOD_UC_CTRL_OPT_PF_TEMP_COMP)) {
            if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_LR4) {
                *intf = SOC_PORT_IF_LR4 ;
            } else {
                *intf = SOC_PORT_IF_SR4 ;
            }
         } else {
            if(an_on) *intf = SOC_PORT_IF_KR4 ;
         }
      }
      break ;
   case TSCMOD_ASPD_40G_CR4:
      *speed = 40000 ;
      *intf  = SOC_PORT_IF_CR4;
      break ;
   case TSCMOD_ASPD_100G_CR10:
      *speed = 100000 ;
      *intf  = SOC_PORT_IF_CR4;
      break ;
   case TSCMOD_ASPD_5G_X2:
      *speed = 5000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_15p75G_X2:
      *speed = 15750 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_2G_FC:
      *speed = 2000 ;
      *intf  = SOC_PORT_IF_GMII;
      break ;
   case TSCMOD_ASPD_4G_FC:
      *speed = 4000 ;
      *intf  = SOC_PORT_IF_GMII;
      break ;
   case TSCMOD_ASPD_8G_FC:
      *speed = 8000 ;
      *intf  = SOC_PORT_IF_GMII;
      break ;
   case TSCMOD_ASPD_10G_CX1:
      *speed = 10000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_1G_CX1:
      *speed = 1000 ;
      *intf  = SOC_PORT_IF_XGMII;
      break ;
   case TSCMOD_ASPD_20G_KR2:
      *speed = 20000 ;
      *intf  = SOC_PORT_IF_KR2;
      break ;
   case TSCMOD_ASPD_20G_CR2:
      *speed = 20000 ;
      *intf  = SOC_PORT_IF_CR2;
      break ;
   default:
      *speed = 0 ;
      break ;
   }

   return SOC_E_NONE;
}

STATIC int
_phy_tscmod_speed_get(int unit, soc_port_t port, int *speed, int *intf, int *asp_mode, int *scr)
{
   phy_ctrl_t *pc;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t *pDesc;
   int rv, tmp_verb;
   int speed_mode, hg_mode, fw_mode, an_on, plldiv;

   rv = 0 ;
   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1) ;

   tmp_verb = tsc->verbosity ;
   if(tsc->verbosity & TSCMOD_DBG_SCAN) {

   } else {  tsc->verbosity = 0 ; }

   tsc->diag_type = TSCMOD_DIAG_SPEED;
   (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
   speed_mode = tsc->accData;

   hg_mode = (tsc->ctrl_type & TSCMOD_CTRL_TYPE_HG) ? 1 : 0 ;

   tsc->per_lane_control = TSCMOD_FWMODE_RD ;
   tscmod_tier1_selector("FWMODE_CONTROL", tsc, &rv);
   fw_mode = tsc->accData ;

   if(tsc->an_type != TSCMOD_AN_NONE ) an_on =1 ; else an_on = 0 ;
   rv = _phy_tscmod_speed_mode_decode(pc, speed_mode, hg_mode, fw_mode, an_on, speed, intf, asp_mode, scr);

   /* get the plldiv from register */
   tsc->diag_type = TSCMOD_DIAG_PLL;
   tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);

   plldiv = tsc->accData ;
   if(tsc->port_type == TSCMOD_SINGLE_PORT ) {
       if(plldiv==70) { 
           *speed = 42000 ; 
       } else if(plldiv==42) {
           if (*asp_mode == TSCMOD_ASPD_10G_X4) {
               *speed = 11000;
           }
       }
   } else if (tsc->port_type == TSCMOD_DXGXS) {
       if(plldiv==70) {
           *speed = 21000 ;
       }    
   } else {
       if(plldiv==70) {
           *speed = 11000 ;
           /* if(*asp_mode ==TSCMOD_ASPD_10G_X4) { *speed = 10500 ; }  */
       }
   }

   if(tsc->verbosity & TSCMOD_DBG_SPD)
      printf("%-22s: u=%0d p=%0d speed_mode=%x speed=%0d intf=%x asp=%0d %s scr=%0d\n",
             FUNCTION_NAME(), tsc->unit, tsc->port, speed_mode, *speed, *intf, *asp_mode,
             e2s_tscmod_aspd_type[*asp_mode], *scr) ;

   tsc->verbosity = tmp_verb;
   return SOC_E_NONE;
}



/*
 * Function:
 *      phy_tscmod_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - current link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_tscmod_speed_get(int unit, soc_port_t port, int *speed)
{
    int rv;
    int intf;
    int asp_mode, scr;

    tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
    rv = _phy_tscmod_speed_get(unit, port, speed, &intf, &asp_mode, &scr);
    tscmod_sema_unlock(unit, port) ;
    return rv;
}

/*
 * Function:
 *      phy_tscmod_an_set
 * Purpose:
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      an   - Boolean, if true, auto-negotiation is enabled
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:
 *      SOC_E_XXX
 */
int
phy_tscmod_an_set(int unit, soc_port_t port, int an)
{
   int rv , enable = 0;
   int speed;

   speed = 0;

   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
   phy_tscmod_speed_get(unit, port, &speed);
   phy_tscmod_lb_get(unit, port, &enable);
   if((enable && an) && (speed >= 10000)) {
       printf("AN on Loopback port is not supported unit %d port %d \n", unit, port);
       rv =  SOC_E_FAIL;
   } else {    
       rv = _phy_tscmod_an_set(unit, port, an) ;
   }    
   tscmod_sema_unlock(unit, port) ;
   return rv ;
}

/* return 1 if new AN mode (required PLL reset) is found */
int tscmod_new_an_mode_check(int old_an_type, int new_an_type)
{
   if((old_an_type==TSCMOD_CL73)||(old_an_type==TSCMOD_CL73_BAM)||
      (old_an_type==TSCMOD_HPAM)||(old_an_type==TSCMOD_CL37_10G)) {
      if((new_an_type==TSCMOD_CL73)||(new_an_type==TSCMOD_CL73_BAM)||
         (new_an_type==TSCMOD_HPAM)||(new_an_type==TSCMOD_CL37_10G)) {
         return 0 ;
      } else {
         return 1 ;
      }
   } else {
      if((new_an_type==TSCMOD_CL73)||(new_an_type==TSCMOD_CL73_BAM)||
         (new_an_type==TSCMOD_HPAM)||(new_an_type==TSCMOD_CL37_10G)) {
         return 1 ;
      } else {
         return 0 ;
      }
   }
   return 1 ;
}

int
_phy_tscmod_an_set(int unit, soc_port_t port, int an)
{
   phy_ctrl_t         *pc, *pc2;
   TSCMOD_DEV_DESC_t  *pDesc, *pDesc2;
   TSCMOD_DEV_CFG_t   *pCfg;
   tscmod_st *tsc, *tsc2;
   TSCMOD_TX_DRIVE_t tx_drv[NUM_LANES];
   int rv, an_type, need_set_port_mode, found, data, pll_diff, pll_target ;
   int lane, lane_s, lane_e, tmp_select, tmp_lane, tmp_dxgxs;
   int unit0; soc_port_t port0 ;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   = (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;

   rv    = SOC_E_NONE ;
   data  = 0 ;  lane=0; lane_s=0; lane_e=0;
   need_set_port_mode = 0 ;
   unit0 = unit ; port0 = port ;
   pll_diff   = 0 ;
   pll_target = 66 ;
   if(tsc->verbosity&(TSCMOD_DBG_AN|TSCMOD_DBG_LINK))
      printf("%-22s: u=%0d p=%0d an=(%0x) cfg_cl73=%0x cl37=%0x port_type=%0d\n",
             FUNCTION_NAME(), unit, port, an, pCfg->cl73an, pCfg->cl37an, tsc->port_type) ;

   tmp_select   = tsc->lane_select ;
   tmp_dxgxs    = tsc->dxgxs ;
   tmp_lane     = tsc->this_lane ;
   
   an_type = TSCMOD_AN_NONE ;
   if( an ) {
      FORCE_CL72_ENABLED(pc) = 0 ;       
      FORCE_CL72_MODE(pc)    = 0 ;
      FORCE_CL72_STATE(pc)   = TSCMOD_FORCE_CL72_IDLE ;
      FORCE_CL72_RESTART_CNT(pc) = 0 ;

      TSCMOD_AN_STATE(pc) = TSCMOD_AN_CL73_CL37_OFF ;
      TSCMOD_AN_MODE(pc)  = 0 ;

      if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_HPAM_AUTODET) 
         tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_HPAM_AUTODET ;

      if( pCfg->cl73an ) {  /* imply XE port for TSCMOD_CL73 */
         switch(pCfg->cl73an) {
         case TSCMOD_CL73_W_BAM:   an_type = TSCMOD_CL73_BAM ;  break ;
         case TSCMOD_CL73_WO_BAM:  an_type = TSCMOD_CL73     ;  break ;
         case TSCMOD_CL73_HPAM_VS_SW:    
                                   an_type = TSCMOD_HPAM     ;  break ;  /* against TD+ */
         case TSCMOD_CL73_HPAM:
                                   an_type = TSCMOD_HPAM     ;  break ;  /* against TR3 */
         case TSCMOD_CL73_CL37:    an_type = TSCMOD_CL73     ;  break ;
         case TSCMOD_CL73_SW_CL37: an_type = TSCMOD_CL73     ;  break ;
         case TSCMOD_CL73_HPAM_AUTOCL73:
                                   an_type = TSCMOD_HPAM     ;  
                                   tsc->ctrl_type |=TSCMOD_CTRL_TYPE_HPAM_AUTODET;
                                   break ;
         case TSCMOD_CL73_HPAM_AUTOCL73_CL37:
                                   an_type = TSCMOD_HPAM     ;
                                   tsc->ctrl_type |=TSCMOD_CTRL_TYPE_HPAM_AUTODET;
                                   break ;
         case TSCMOD_CL73_HPAM_CL37:
                                   an_type = TSCMOD_HPAM     ;  break ;
         case TSCMOD_CL73_HG:  
                                   an_type = TSCMOD_CL73     ;  pll_target = 70 ; break ;
         default:
            break ;
         }
      } else if( pCfg->cl37an ) {
         pll_target = 40 ;
         switch(pCfg->cl37an) {
         case TSCMOD_CL37_WO_BAM:
            if(pCfg->pll_divider==66) {
               an_type = TSCMOD_CL37_10G ;
               pll_target = 66 ;
            } else
               an_type = TSCMOD_CL37     ;
            break ;
         case TSCMOD_CL37_W_BAM:           an_type = TSCMOD_CL37_BAM ;  break ;
         case TSCMOD_CL37_W_10G:           an_type = TSCMOD_CL37_10G ;  pll_target = 66 ; break ;
            /*  no SDK-49139/CRTSC-742 special mode */
         case TSCMOD_CL37_HR2SPM:          an_type = TSCMOD_CL37     ;  break ;
         case TSCMOD_CL37_HR2SPM_W_10G:    an_type = TSCMOD_CL37_10G ;  pll_target = 66 ; break ;
         case TSCMOD_CL37_W_BAM_HG:        an_type = TSCMOD_CL37_BAM ;  pll_target = 70 ; break ;
         default:
            break ;
         }
      } else {
         pll_target = 40 ;
         if(pCfg->hg_mode) {
            an_type = TSCMOD_CL37_BAM ;  /* default mode */
         } else {
            if(pCfg->fiber_pref) {
               if(pCfg->pll_divider==66) {
                  an_type = TSCMOD_CL37_10G  ;  /* 1G, BR=10G */
                  pll_target = 66 ;
               } else
                  an_type = TSCMOD_CL37 ;       /*  1G (BR=6.25G) */
            } else {
               an_type = TSCMOD_CL37_SGMII ;
            }
         }
      }
      if(an_type==TSCMOD_AN_NONE)
         printf("Warning: u=%0d p=%0d an=(%0x) configuration error cfg_cl73=%0x cl37=%0x\n",
                  unit, port, an, pCfg->cl73an, pCfg->cl37an) ;

      need_set_port_mode =0 ; /* decide to call set_an_port_mode or not */
      if(tsc->an_type == an_type) {
         /* no need to call set_an_port_mode */
      } else {
         if(tsc->verbosity&TSCMOD_DBG_INIT)
            printf("%-22s PBMP_ALL_ITER u=%0d p=%0d AN type scan an=%s to an=%s\n",
                   FUNCTION_NAME(), unit0, port0, e2s_tscmod_an_type[tsc->an_type],e2s_tscmod_an_type[an_type]) ;
         unit0 = unit ; port0 = port ;   found = 0 ;
         PBMP_ALL_ITER(unit0, port0) {
            pc2 = INT_PHY_SW_STATE(unit0, port0);
            if (pc2 == NULL) {
               continue;
            }
            if(tsc->verbosity&TSCMOD_DBG_INIT)
               printf( "%-22s PBMP_ALL_ITER u=%0d p=%0d AN type scan init=%0d and an_init=%0d num=%0d\n",
                      FUNCTION_NAME(), unit0, port0, ((pc2->flags & PHYCTRL_INIT_DONE)?1:0),
                      ((pc2->flags & PHYCTRL_ANEG_INIT_DONE)?1:0), pc2->chip_num) ;
            if (!pc2->dev_name || pc2->dev_name != tscmod_device_name) {
               continue;
            }
            if (pc2->chip_num != pc->chip_num) {
               continue;
            }
            pDesc2 = (TSCMOD_DEV_DESC_t *)(pc2 + 1);
            tsc2   = (tscmod_st *)( pDesc2 + 1);
            if(tsc2->plldiv != pll_target) {
               if(tsc->verbosity&TSCMOD_DBG_INIT)
                  printf("%-22s PBMP_ALL_ITER u=%0d p=%0d AN pll diff=%0d vs %0d target=%0d\n",
                         FUNCTION_NAME(), unit0, port0, tsc2->plldiv, tsc->plldiv, pll_target) ;
               pll_diff = 1 ;
            }
            if((pc2->flags & PHYCTRL_INIT_DONE)&&(pc2->flags & PHYCTRL_ANEG_INIT_DONE)) {
               found  = 1 ;
               if(tscmod_new_an_mode_check(tsc2->an_type, an_type)) {
                  tsc2->an_type = an_type ;
                  need_set_port_mode =1 ;
               }
               if(tsc->verbosity&TSCMOD_DBG_INIT)
                  printf("%-22s PBMP_ALL_ITER u=%0d p=%0d AN type scan by p=%0d to correct an=%s set_port_mode=%0d\n",
                         FUNCTION_NAME(), unit0, port0, port, e2s_tscmod_an_type[tsc2->an_type], need_set_port_mode) ;
            }
         }
         if(!found) {
            need_set_port_mode =1 ;
         }
      }

      tsc->an_type = an_type ;
      if(pll_target==70) {
          tsc->an_hg_vco = 1 ;
      } else {
          tsc->an_hg_vco = 0 ;
      }

      if(tsc->port_type !=TSCMOD_SINGLE_PORT) {
         if(pll_diff==0) {
            /* per port operation */
            if(tsc->verbosity&TSCMOD_DBG_INIT)
               printf("%-22s u=%0d p=%0d AN SHARE pll=%0d target=%0d\n",
                      FUNCTION_NAME(), unit, port, tsc->plldiv, pll_target) ;
            tsc->per_lane_control= TSCMOD_CTL_AN_MODE_SHARE ;
            tscmod_tier1_selector("SET_AN_PORT_MODE", tsc, &rv);
            need_set_port_mode = 0 ;
         }
      }

      /* per port */
      if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         tsc->lane_select      = TSCMOD_LANE_BCST ;
      }
      if(need_set_port_mode) {
         if(!_tscmod_chip_an_init_done(unit,pc->chip_num)) {
            need_set_port_mode = 1 ;
         } else {
            need_set_port_mode = 0 ;
         }
         pc->flags                   |= PHYCTRL_ANEG_INIT_DONE ;
      }
      if(need_set_port_mode) {
         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         tsc->per_lane_control = 0;
         tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

         tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
         tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

         tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
         tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

         tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_SWITCH_OVER, 0) ;

         if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
            tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
         }
         tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);


         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_AFE_ANALOG ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         sal_usleep(1000) ;

         tsc->lane_select      = getLaneSelect(tmp_lane) ;
         tsc->dxgxs            = 0;
         tsc->per_lane_control = 0;
         tscmod_tier1_selector("PLL_SEQUENCER_CONTROL", tsc, &rv);

         sal_usleep(2000) ;
         if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
            tsc->lane_select      = TSCMOD_LANE_BCST ;
         }
         tsc->dxgxs            = tmp_dxgxs ;

         /* set an port mode per port */
         if(tsc->verbosity&TSCMOD_DBG_INIT)
               printf("%-22s u=%0d p=%0d AN CHIP_INIT pll=%0d target=%0d\n",
                      FUNCTION_NAME(), unit, port, tsc->plldiv, pll_target) ;
         tsc->per_lane_control= TSCMOD_CTL_AN_CHIP_INIT; /* preemptive to change plldiv */
         tscmod_tier1_selector("SET_AN_PORT_MODE", tsc, &rv);
         pc->flags                   |= PHYCTRL_ANEG_INIT_DONE ;

         if(pCfg->los_usec>4000) {
             sal_usleep(pCfg->los_usec-4000) ;
         }
         unit0 = unit ; port0 = port ;
         PBMP_ALL_ITER(unit0, port0) {
            pc2 = INT_PHY_SW_STATE(unit0, port0);
            if (pc2 == NULL) {
               continue;
            }
            if (!pc2->dev_name || pc2->dev_name != tscmod_device_name) {
               continue;
            }
            if (pc2->chip_num != pc->chip_num) {
               continue;
            }
            if (1) {
               if(tsc->verbosity&TSCMOD_DBG_INIT)
                  printf("%-22s PBMP_ALL_ITER u=%0d p=%0d AN scan by p=%0d to correct tsc->plldiv=%0d\n",
                         FUNCTION_NAME(), unit0, port0, port, tsc->plldiv) ;
               pDesc2 = (TSCMOD_DEV_DESC_t *)(pc2 + 1);
               tsc2   = (tscmod_st *)( pDesc2 + 1);
               tsc2->plldiv = tsc->plldiv ;
            }
         }

         tsc->lane_select      = getLaneSelect(tmp_lane) ;
         tsc->dxgxs            = 0;
         tsc->per_lane_control = 1;
         tscmod_tier1_selector("PLL_SEQUENCER_CONTROL", tsc, &rv);

         tscmod_tier1_selector("PLL_LOCK_WAIT", tsc, &rv);

         if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
            tsc->lane_select      = TSCMOD_LANE_BCST ;
         }
         tsc->dxgxs            = tmp_dxgxs ;

         /* AMP */
         SOC_IF_ERROR_RETURN
            (_phy_tscmod_tx_control_get(unit, port, &tx_drv[0], TXDRV_AN_INX));
         SOC_IF_ERROR_RETURN
            (_phy_tscmod_tx_control_set(unit, port, &tx_drv[0]));


         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_AFE_ANALOG ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         sal_usleep(1000) ;

      }

      /* per port */
      if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         tsc->lane_select      = TSCMOD_LANE_BCST ;
      }

      /* gracefully stop uC */
      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY) {
         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }

      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      /* SDK-50095 */
      if(TSCMOD_MODEL_REVID_GET(tsc)<TSCMOD_MODEL_REVID_A2) {
         if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
            lane_s = 0 ; lane_e = 4 ;
         } else if (tsc->port_type == TSCMOD_DXGXS ) {
            if((tsc->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
            else { lane_s = 0; lane_e = 2 ; }
         } else {
            lane_s = tsc->this_lane ; lane_e = lane_s + 1 ;
         }
         for(lane=lane_s; lane<lane_e; lane++) {
            tscmod_uc_cmd_seq(unit, port, lane, 0) ;
         }
      } else {
         /* SDK-50095 */
         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AN_MODES ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }

      if( tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         tsc->lane_select     = TSCMOD_LANE_BCST ;
      } else {
         tsc->lane_select     = _tscmod_get_lane_select(unit, port, tmp_lane) ;
      }

      /* set rxSeqStart_AN_disable=0 */
      tsc->per_lane_control = 0;
      tscmod_tier1_selector("AFE_RXSEQ_START_CONTROL", tsc, &rv);

      /* not needed.  1/12/2013
      tsc->per_lane_control = 1 | TSCMOD_AN_SET_RF_DISABLE ;
      tscmod_tier1_selector("AUTONEG_SET", tsc, &rv);
      */

      /* for 10gpdetect */
      tsc->per_lane_control = 1 | TSCMOD_AN_SET_CL48_SYNC ;
      tscmod_tier1_selector("AUTONEG_SET", tsc, &rv);


#ifdef BRCM_TSCMOD_DEBUG
      if(tsc->verbosity&TSCMOD_DBG_AN)
         tscmod_diag_g_an(tsc, tsc->this_lane) ;
#endif

      /* per port control */
      if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         tsc->lane_select      = TSCMOD_LANE_BCST ;
      }

      tsc->per_lane_control = 0;
      tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

      tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      tscmod_uc_fw_control(tsc, TSCMOD_UC_CL72_SWITCH_OVER, 0) ;

      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
         tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }
      tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

      /* SDK-50095 */
      tsc->per_lane_control = TSCMOD_CL72_HW_DISABLE ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      tsc->per_lane_control = TSCMOD_CL72_STATUS_RESET ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      tsc->per_lane_control = TSCMOD_CL72_HW_RESTART ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      if(need_set_port_mode==0){
         SOC_IF_ERROR_RETURN
            (_phy_tscmod_tx_control_get(unit, port, &tx_drv[0], TXDRV_AN_INX));
         SOC_IF_ERROR_RETURN
            (_phy_tscmod_tx_control_set(unit, port, &tx_drv[0]));
      }

      /* for experiment chaning timers, comment the codes */
      tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_CL72_LINK_FAIL_TIMER ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_CL72_MAX_WAIT_TIMER ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      /* CRTSC-723 */
      if((tsc->an_type !=TSCMOD_AN_NONE)&&(pCfg->an_cl72)){
         tsc->per_lane_control = 1 | TSCMOD_AN_SET_LK_FAIL_INHIBIT_TIMER_NO_CL72 ;
         tscmod_tier1_selector("AUTONEG_SET", tsc, &rv);
      } else {
         tsc->per_lane_control = 0 | TSCMOD_AN_SET_LK_FAIL_INHIBIT_TIMER_NO_CL72 ;
         tscmod_tier1_selector("AUTONEG_SET", tsc, &rv);
      }

      /* PHY-1078, PHY-1166 */
      if((pCfg->cl73an==TSCMOD_CL73_CL37)||(pCfg->cl73an==TSCMOD_CL73_SW_CL37) || (an_type ==TSCMOD_CL37_10G)||
         (pCfg->cl73an==TSCMOD_CL73_HPAM_AUTOCL73_CL37)||(pCfg->cl73an==TSCMOD_CL73_HPAM_CL37)) {
         tsc->per_lane_control = (TSCMOD_RAM_BASE_ADDR)|(tscmod_ucode_TSC_revA0_act_addr<<TSCMOD_COLD_RESET_MODE_SHIFT);
      } else {
         if((TSCMOD_MODEL_REVID_GET(tsc)<=TSCMOD_MODEL_REVID_A1) &&
            (TSCMOD_MODEL_PROJ_GET(tsc)!=TSCMOD_MODEL_PROJ_HR2)){
            tsc->per_lane_control = (TSCMOD_RAM_BASE_ADDR)|(tscmod_ucode_TSC_revA0_ect_addr<<TSCMOD_COLD_RESET_MODE_SHIFT);
         } else {
            tsc->per_lane_control = (TSCMOD_RAM_BASE_ADDR)|(tscmod_ucode_TSC_revA0_cct_addr<<TSCMOD_COLD_RESET_MODE_SHIFT);
         }
      }
      tscmod_tier1_selector("CORE_RESET", tsc, &rv);

      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_FIRMWARE_MODE ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      /* JIRA CRTSC-720
      if(need_set_port_mode==0)
         sal_usleep(pCfg->los_usec-1000) ;
      */

      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_TAP_MUXSEL ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      /* for AN, HW shall not be set */
      /* equavlent to tscmod_an_cl72_control_set() */
      tsc->per_lane_control = TSCMOD_CL72_HW_DISABLE ;
      tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);

      if((tsc->an_type ==TSCMOD_CL73)||(tsc->an_type ==TSCMOD_CL73_BAM)||
               (tsc->an_type ==TSCMOD_HPAM)) {
         if(pCfg->an_cl72) {
            /* remove the forced bit */
            tsc->per_lane_control = 0x0 ;
            tscmod_tier1_selector("TX_TAP_CONTROL", tsc, &rv) ;

            tsc->per_lane_control = TSCMOD_CL72_AN_NO_FORCED ;
            tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
         } else {
            tsc->per_lane_control = TSCMOD_CL72_AN_FORCED_DISABLE ; /* forced to disable CL72 */
            tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
         }

         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_BAM37_ACOL_SWAP ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      } else if(an_type == TSCMOD_CL37_BAM) {
         if(pCfg->an_cl72) {
            /* remove the forced bit */
            tsc->per_lane_control = 0x0 ;
            tscmod_tier1_selector("TX_TAP_CONTROL", tsc, &rv) ;

            if(pCfg->forced_init_cl72>0) {
               tsc->per_lane_control = TSCMOD_CL72_AN_FORCED_ENABLE ;
               tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv) ;
            } else {
               tsc->per_lane_control = TSCMOD_CL72_AN_NO_FORCED ;
               tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
            }

         } else {
            if(pCfg->forced_init_cl72>0) {
               tsc->per_lane_control = TSCMOD_CL72_AN_FORCED_ENABLE ;
               tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv) ;
            } else {
               tsc->per_lane_control = TSCMOD_CL72_AN_NO_FORCED ;
               tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
            }
         }

         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_BAM37_ACOL_SWAP ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }

      /* SDK-50095 */
      if(TSCMOD_MODEL_REVID_GET(tsc)<TSCMOD_MODEL_REVID_A2) {
         /* gracefully resume */
         for(lane=lane_s; lane<lane_e; lane++) {
            tscmod_uc_cmd_seq(unit, port, lane, 2) ;
         }
      }

      /* PHY-1105
      tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY) {
         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }
      */

      if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }
      sal_usleep(1000) ;

      if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         tsc->lane_select      = TSCMOD_LANE_BCST ;
      }

      if(tsc->an_type ==TSCMOD_HPAM) {
         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_HP_BAM_AM ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
         if(pCfg->cl73an==TSCMOD_CL73_HPAM_VS_SW) {
            tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_HP_BAM_SWM_TIMER ;
            tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
         } else {
            tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_HP_BAM_SWM_TIMER ;
            tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
         }
      } else {
         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_HP_BAM_AM ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_HP_BAM_SWM_TIMER ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }

      if(!(tsc->ctrl_type & (TSCMOD_CTRL_TYPE_LINK_DIS|TSCMOD_CTRL_TYPE_TX_SQUELCH))) {
          tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }
      /* first lane (lane0) */
      tsc->lane_select      = getLaneSelect(tmp_lane) ;
      tsc->this_lane        = tmp_lane ;
      tsc->dxgxs            = 0 ;

      data = 0 ;
      if(an_type == TSCMOD_CL37_BAM) {
         /* should not enable pdetect */
          data = TSCMOD_PDET_CONTROL_1G | TSCMOD_PDET_CONTROL_10G |
                 TSCMOD_PDET_CONTROL_CMD_MASK;
      } else {
         if(pCfg->pdetect1000x) {
             data |= TSCMOD_PDET_CONTROL_1G | TSCMOD_PDET_CONTROL_MASK |
                    TSCMOD_PDET_CONTROL_CMD_MASK;
         }
         if ((pCfg->pdetect10g) && (tsc->port_type==TSCMOD_SINGLE_PORT)) {
             data |= TSCMOD_PDET_CONTROL_10G | TSCMOD_PDET_CONTROL_MASK |
                     TSCMOD_PDET_CONTROL_CMD_MASK;
         }
      }
      tsc->per_lane_control = data ;
      tscmod_tier1_selector("PARALLEL_DETECT_CONTROL", tsc, &rv);

      /* SDK-49139/CRTSC-742 */
      if((TSCMOD_MODEL_PROJ_GET(tsc)==TSCMOD_MODEL_PROJ_HR2) &&
         (pCfg->uc_ver >= 0xa034)&&
         ((tsc->an_type==TSCMOD_CL37)||(tsc->an_type==TSCMOD_CL37_10G))&&
         ((pCfg->cl37an==TSCMOD_CL37_HR2SPM)||(pCfg->cl37an==TSCMOD_CL37_HR2SPM_W_10G))) {

            rv=tscmod_uc_fw_control(tsc, TSCMOD_UC_CL37_HR2_WAR, 1) ;
      }

      tscmod_tier1_selector("AUTONEG_CONTROL", tsc, &rv);

      if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         tsc->lane_select      = TSCMOD_LANE_BCST ;
      }
      tsc->dxgxs            = tmp_dxgxs ;

      tsc->per_lane_control = 1;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
      /* turn on TX lanes on the same time */
      tsc->per_lane_control = 1;
      tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

      if(!(tsc->ctrl_type & (TSCMOD_CTRL_TYPE_LINK_DIS|TSCMOD_CTRL_TYPE_TX_SQUELCH))) {
          tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }

      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
         tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }
      tsc->per_lane_control = TSCMOD_RXP_REG_ON ;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

      /* PHY-1105 */
      if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_LINK_DIS)) {
          tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

          if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY) {
              tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY ;
              tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
          }
      }
      /* SDK_50095 */
      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }

      if((pCfg->cl73an==TSCMOD_CL73_CL37)||(pCfg->cl73an==TSCMOD_CL73_SW_CL37)) {
         TSCMOD_AN_STATE(pc)    = TSCMOD_AN_WAIT_CL73 ;
         TSCMOD_AN_TIME(pc)     = sal_time_usecs() + TSCMOD_AN_WAIT_CL73_TMR ;
         TSCMOD_AN_MODE(pc)     = 0 ;
         TSCMOD_AN_TICK_CNT(pc) = 0 ;
      } else if((pCfg->cl73an==TSCMOD_CL73_HPAM_CL37)||(pCfg->cl73an==TSCMOD_CL73_HPAM_AUTOCL73_CL37)) {
         TSCMOD_AN_STATE(pc)    = TSCMOD_AN_WAIT_CL73 ;
         TSCMOD_AN_TIME(pc)     = sal_time_usecs() + TSCMOD_AN_WAIT_HPBAM_TMR ;
         TSCMOD_AN_MODE(pc)     = 0 ;
         TSCMOD_AN_TICK_CNT(pc) = 0 ;
      }
   } else {
      if( pc->flags & PHYCTRL_ANEG_INIT_DONE ) pc->flags ^= PHYCTRL_ANEG_INIT_DONE ;

      if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
         tsc->lane_select      = TSCMOD_LANE_BCST ;
      }
      tsc->dxgxs            = tmp_dxgxs ;

      if((pCfg->am_reserved==0)&&(tsc->spd_intf!=TSCMOD_SPD_10000_XFI)) {
          tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_HP_BAM_AM ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }

      /* CRTSC-723 */
      tsc->per_lane_control = 0 | TSCMOD_AN_SET_LK_FAIL_INHIBIT_TIMER_NO_CL72 ;
      tscmod_tier1_selector("AUTONEG_SET", tsc, &rv);

      /* first lane (lane0) */
      tsc->lane_select      = getLaneSelect(tmp_lane) ;
      tsc->this_lane        = tmp_lane ;
      tsc->dxgxs            = 0 ;

      /* SDK-49139/CRTSC-742 */
      if((TSCMOD_MODEL_PROJ_GET(tsc)==TSCMOD_MODEL_PROJ_HR2) &&
         (pCfg->uc_ver >= 0xa034)&&
         ((tsc->an_type==TSCMOD_CL37)||(tsc->an_type==TSCMOD_CL37_10G))&&
         ((pCfg->cl37an==TSCMOD_CL37_HR2SPM)||(pCfg->cl37an==TSCMOD_CL37_HR2SPM_W_10G))) {

            rv=tscmod_uc_fw_control(tsc, TSCMOD_UC_CL37_HR2_WAR, 1) ;
      }

      tsc->an_hg_vco = 0;
      if(tsc->an_type != TSCMOD_AN_NONE ) {
         tsc->an_type = TSCMOD_AN_NONE ;
         (tscmod_tier1_selector("AUTONEG_CONTROL", tsc, &rv));
      }

      if((pCfg->cl73an==TSCMOD_CL73_CL37)||(pCfg->cl73an==TSCMOD_CL73_SW_CL37)) {
         TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_OFF ;
         TSCMOD_AN_MODE(pc)     = 0 ;
      } else if((pCfg->cl73an==TSCMOD_CL73_HPAM_CL37)||(pCfg->cl73an==TSCMOD_CL73_HPAM_AUTOCL73_CL37)) {
         TSCMOD_AN_STATE(pc)    = TSCMOD_AN_CL73_CL37_OFF ;
         TSCMOD_AN_MODE(pc)     = 0 ;
      }
   }

   tsc->lane_select      = tmp_select ;   /* restore */
   tsc->this_lane        = tmp_lane ;
   tsc->dxgxs            = tmp_dxgxs ;

   rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_AN_MODE) ;
   return rv ;
}

/*
 * Function:
 *      phy_tscmod_an_get
 * Purpose:
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      an   - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if an == false.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_tscmod_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
   phy_ctrl_t  *pc;
   tscmod_st   *tsc;
   TSCMOD_DEV_DESC_t *pDesc;
   int rv;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);

   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;

   *an = 0;
   *an_done = 0;

   tsc->diag_type        = TSCMOD_DIAG_ANEG;
   tsc->per_lane_control = 1 << TSCMOD_DIAG_AN_DONE ;
   (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));

   *an      = ((tsc->accData) & TSCMOD_AN_ENABLE_MASK) ? TRUE : FALSE;
   *an_done = ((tsc->accData) & TSCMOD_AN_DONE_MASK) ? TRUE : FALSE ;

   tscmod_sema_unlock(unit, port) ;
   return SOC_E_NONE;
}

STATIC int
tscmod_limit_an_speed_set(int unit, soc_port_t port, int lane_mode)
{
   phy_ctrl_t  *pc;
   tscmod_st   *tsc;
   TSCMOD_DEV_DESC_t *pDesc;
   int         ability ;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);

   ability = tsc->an_bam37_ability;

   if(lane_mode==2) {  /* 2-lane port */
#if 0
      if(tsc->an_bam37_ability & TSCMOD_BAM37ABL_20G_X2)     tsc->an_bam37_ability = TSCMOD_BAM37ABL_20G_X2 ;     /* 10.3125 */
      if(tsc->an_bam37_ability & TSCMOD_BAM37ABL_20G_X2_CX4) tsc->an_bam37_ability = TSCMOD_BAM37ABL_20G_X2_CX4 ; /* 10.3125 */
      if(tsc->an_bam37_ability & TSCMOD_BAM37ABL_15P75G_R2)  tsc->an_bam37_ability = TSCMOD_BAM37ABL_15P75G_R2;   /* 8.125 */
      if(tsc->an_bam37_ability & TSCMOD_BAM37ABL_12P7_DXGXS) {
         tsc->an_bam37_ability = TSCMOD_BAM37ABL_12P7_DXGXS ; /* 6.5625 */
      } else if(tsc->an_bam37_ability & TSCMOD_BAM37ABL_10P5G_DXGXS) {
         tsc->an_bam37_ability = TSCMOD_BAM37ABL_10P5G_DXGXS ;
      }
#endif
      /* the rest is 6.25 */
   } else if(lane_mode==1) {/* 1-lane port */
      /* all is 6.25 */
   } else {  /* 4-lane port */
      /* do nothing, no restriction */
   }
   if(tsc->verbosity&TSCMOD_DBG_AN) {
      printf("%-22s u=%0d p=%0d limit ability from %s %s(%x) to %s %s(%x)\n",
             FUNCTION_NAME(), unit, port, tscmod_cl37bam_ability_msg0(ability), tscmod_cl37bam_ability_msg1(ability), ability,
             tscmod_cl37bam_ability_msg0(tsc->an_bam37_ability), tscmod_cl37bam_ability_msg1(tsc->an_bam37_ability),
             tsc->an_bam37_ability);
   }
   return SOC_E_NONE ;
}


/*
 * Function:
 *      phy_tscmod_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_tscmod_ability_advert_set(int unit, soc_port_t port,
                       soc_port_ability_t *ability)
{
   int rv ;
   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
   rv = _phy_tscmod_ability_advert_set(unit, port, ability) ;
   tscmod_sema_unlock(unit, port) ;
   return rv ;
}

STATIC int
_phy_tscmod_ability_advert_set(int unit, soc_port_t port,
                       soc_port_ability_t *ability)
{
   phy_ctrl_t        *pc;
   TSCMOD_DEV_DESC_t *pDesc;
   TSCMOD_DEV_CFG_t  *pCfg;
   tscmod_st         *tsc;
   int               sgmii_speed ;

   int rv ;
   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;

   tsc->an_tech_ability  = 0 ;
   tsc->an_bam37_ability = 0 ;
   /* map ability to tsc->an_tech_ability */
   if(tsc->port_type == TSCMOD_SINGLE_PORT) {
      if(ability->speed_full_duplex & SOC_PA_SPEED_40GB) {
          if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_KR4) {
              tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_40G_KR4) ;
          } else {
              tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_40G_CR4) ;
              tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_40G_KR4) ;
          }
          tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_40G) ;
      }
      /*
      if(ability->speed_full_duplex & SOC_PA_SPEED_33GB) {
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_32P7G);
         }  */
      /*
      if(ability->speed_full_duplex & SOC_PA_SPEED_32GB) {
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_31P5G);
         }  */
      if(ability->speed_full_duplex & SOC_PA_SPEED_25GB) {
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_25P455G);
      }
      if(ability->speed_full_duplex & SOC_PA_SPEED_21GB)
         tsc->an_bam37_ability  |= (1<<TSCMOD_BAM37ABL_21G_X4);

      if(ability->speed_full_duplex & SOC_PA_SPEED_20GB) {
          if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_KR2) {
              tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_20G_KR2) ;
          } else {
              tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_20G_CR2) ;
              tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_20G_KR2) ;
          }
         /*
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_20G_X2_CX4) ;
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_20G_X2) ;  * if if=CX, use TSCMOD_BAM37ABL_20G_X2_CX4 *
         */
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_20G_X4) ; /* if if=CX, use TSCMOD_BAM37ABL_20G_X4_CX4 */
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_20G_X4_CX4) ;

      }
      if(ability->speed_full_duplex & SOC_PA_SPEED_16GB) {
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_16G_X4) ;
         /*
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_15P75G_R2) ;
         */
      }
      if(ability->speed_full_duplex & SOC_PA_SPEED_15GB)
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_15G_X4) ;

      if(ability->speed_full_duplex & SOC_PA_SPEED_13GB) {
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_13G_X4) ;
         /*
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_12P7_DXGXS) ;
         */
      }
      if(ability->speed_full_duplex & SOC_PA_SPEED_12P5GB) {
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_12P5_X4) ;
      }
      if(ability->speed_full_duplex & SOC_PA_SPEED_12GB) {
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_12G_X4) ;
      }
      /*
      if(ability->speed_full_duplex & SOC_PA_SPEED_11GB)
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_10P5G_DXGXS) ;
      */
      if(ability->speed_full_duplex & SOC_PA_SPEED_10GB) {
         tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_10G_KX4);
         if((DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_KR) ||
            (DEV_CFG_PTR(pc)->cx4_10g==0)) {
             tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_10G_KR) ;
         }
         /*
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_10G_X2_CX4) ;

         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_10G_DXGXS);   if if=CX, use TSCMOD_BAM37ABL_10G_X2_CX4
         */
         if (DEV_CFG_PTR(pc)->cx4_10g) {
            tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_10G_CX4);
         } else {
            tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_10G_HIGIG); /* if if=CX, use TSCMOD_BAM37ABL_10G_CX4 */
         }
      }
      if(ability->speed_full_duplex & SOC_PA_SPEED_6000MB)
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_6G_X4) ;

      if(ability->speed_full_duplex & SOC_PA_SPEED_5000MB)
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_5G_X4) ;

      if(ability->speed_full_duplex & SOC_PA_SPEED_2500MB) {
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_2P5G) ;
      }
      if(ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
         tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_1G_KX);
      }
      tscmod_limit_an_speed_set(unit, port, 4) ;

   } else if(tsc->port_type == TSCMOD_MULTI_PORT) { /* one-lane port */
      if(ability->speed_full_duplex & SOC_PA_SPEED_10GB) {
         tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_10G_KR) ;
      }
      if(ability->speed_full_duplex & SOC_PA_SPEED_2500MB) {
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_2P5G) ;
      }
      if(ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
         tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_1G_KX);
      }
      tscmod_limit_an_speed_set(unit, port, 1) ;
   } else { /* dual port */
      if(ability->speed_full_duplex & SOC_PA_SPEED_20GB) {
          if(DEV_CFG_PTR(pc)->line_intf & TSCMOD_IF_KR2) {
              tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_20G_KR2) ;
          } else {
              tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_20G_CR2) ;
              tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_20G_KR2) ;
          }
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_20G_X2) ; /* if if=CX, use  TSCMOD_BAM37ABL_20G_X2_CX4 */
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_20G_X2_CX4) ;
      }
      if(ability->speed_full_duplex & SOC_PA_SPEED_16GB)
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_15P75G_R2) ;

      if(ability->speed_full_duplex & SOC_PA_SPEED_13GB)
         tsc->an_bam37_ability |=  (1<<TSCMOD_BAM37ABL_12P7_DXGXS) ;

      if(ability->speed_full_duplex & SOC_PA_SPEED_11GB)
         tsc->an_bam37_ability |=  (1<<TSCMOD_BAM37ABL_10P5G_DXGXS) ;

      if(ability->speed_full_duplex & SOC_PA_SPEED_10GB) {
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_10G_X2_CX4) ;
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_10G_DXGXS); /* if if=CX, use TSCMOD_BAM37ABL_10G_X2_CX4 */
      }
      if(ability->speed_full_duplex & SOC_PA_SPEED_10GB) {
         tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_10G_KR) ;
      }
      if(ability->speed_full_duplex & SOC_PA_SPEED_2500MB) {
         tsc->an_bam37_ability |= (1<<TSCMOD_BAM37ABL_2P5G) ;
      }
      if(ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
         tsc->an_tech_ability  |= (1<<TSCMOD_ABILITY_1G_KX);
      }
      tscmod_limit_an_speed_set(unit, port, 2) ;
   }

   switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
   case SOC_PA_PAUSE_TX:
      tsc->an_tech_ability |=TSCMOD_ABILITY_ASYM_PAUSE ;
      break;
   case SOC_PA_PAUSE_RX:
      tsc->an_tech_ability |=(TSCMOD_ABILITY_SYMM_PAUSE|TSCMOD_ABILITY_ASYM_PAUSE) ;
      break;
   case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
      tsc->an_tech_ability |=TSCMOD_ABILITY_SYMM_PAUSE ;
      break;
   }
   if(tsc->verbosity&TSCMOD_DBG_AN) {
      printf("%-22s: u=%0d p=%0d %s full_duplex ability %s(=%0x) pause=%0x\n",
             FUNCTION_NAME(), unit, port, e2s_tscmod_an_type[tsc->an_type], tscmod_ability_msg0(ability->speed_full_duplex),
             ability->speed_full_duplex, ability->pause);
      printf("   cl73=(%0x)%s\n", tsc->an_tech_ability,
             tscmod_cl73_ability_msg0(tsc->an_tech_ability)) ;
      printf("   cl37=(%0x)%s %s\n",  tsc->an_bam37_ability,
             tscmod_cl37bam_ability_msg0(tsc->an_bam37_ability), tscmod_cl37bam_ability_msg1(tsc->an_bam37_ability)) ;
   }
   tsc->per_lane_control = 1 ;
   tscmod_tier1_selector("AUTONEG_SET", tsc, &rv);

   tsc->per_lane_control = TSCMOD_AN_SET_CL37_ATTR ;
   if(pCfg->an_cl72) tsc->per_lane_control |= TSCMOD_AN_SET_CL72_MODE ;
   if(tsc->an_fec)   tsc->per_lane_control |= TSCMOD_AN_SET_FEC_MODE ;
   if(pCfg->hg_mode) tsc->per_lane_control |= TSCMOD_AN_SET_HG_MODE ;
   tscmod_tier1_selector("AUTONEG_SET", tsc, &rv);

   /* when this func is called, an_type is not set yet*/
   sgmii_speed = 0x3 ; /* reserved speed */
   if(1||tsc->an_type==TSCMOD_CL37_SGMII) {

      if(ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
         sgmii_speed = 0x2 ;
      } else if(ability->speed_full_duplex & SOC_PA_SPEED_100MB) {
         sgmii_speed = 0x1 ;
      } else if(ability->speed_full_duplex & SOC_PA_SPEED_10MB) {
         sgmii_speed = 0x0 ;
      }

      tsc->per_lane_control = sgmii_speed | ((pCfg->sgmii_mstr?1:0)<<4) | TSCMOD_AN_SET_SGMII_SPEED ;
      if(tsc->verbosity&TSCMOD_DBG_AN)
         printf("%-22s u=%0d p=%0d sgmii_speed=%0d master=%0d cntl=%0x\n",
                FUNCTION_NAME(), unit, port, sgmii_speed, pCfg->sgmii_mstr, tsc->per_lane_control);

      tscmod_tier1_selector("AUTONEG_SET", tsc, &rv);
   }

   return SOC_E_NONE;
}


/*
 * Function:
 *      phy_tscmod_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_tscmod_ability_advert_get(int unit, soc_port_t port,
                           soc_port_ability_t *ability)
{
   int cl37_reg, cl73_reg ;  uint32 reg73_ability, reg37_ability, reg_ability; soc_port_mode_t  ability_mode;
   phy_ctrl_t        *pc;
   TSCMOD_DEV_DESC_t *pDesc;
   tscmod_st         *tsc;
   TSCMOD_DEV_CFG_t  *pCfg;
   int rv ;
   int an_mode;

   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;

   TSCMOD_MEM_SET(ability, 0, sizeof(*ability));

   /* To consider when AN mode is not decided: make CL73 and CL37 abaility coherent */
   cl73_reg = 0 ; cl37_reg = 0 ;   ability_mode = 0 ;  reg73_ability = 0 ; reg37_ability = 0  ;
   
   /*get the AN mode*/
   tsc->diag_type = TSCMOD_DIAG_ANEG;
   tsc->per_lane_control = 1 << TSCMOD_DIAG_AN_MODE ;
   tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
   an_mode        = tsc->accData ;

   if(an_mode) {  /* dynamic inputs from registers */
       if ((an_mode&TSCMOD_DIAG_AN_MODE_HPAM)|
           (an_mode&TSCMOD_DIAG_AN_MODE_CL73BAM)|
           (an_mode&TSCMOD_DIAG_AN_MODE_CL73)) {
           cl73_reg = 1;
       }
       if ((an_mode&TSCMOD_DIAG_AN_MODE_CL37)|
           (an_mode&TSCMOD_DIAG_AN_MODE_CL37BAM)){
           cl37_reg = 1;
       }
   } else {
       /* use data to decide AN mode */
       if(pCfg->cl73an) {
           cl73_reg =1 ;
       } else if(pCfg->cl37an) {
           cl37_reg = 1;
       } else {
           cl37_reg = 1; 
       }
   }

   if(cl73_reg) {
      tsc->per_lane_control=TSCMOD_AN_GET_CL73_ABIL ;
      tscmod_tier1_selector("AUTONEG_GET", tsc, &rv);
      reg73_ability = tsc->accData ;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_40G_CR4)) ?SOC_PA_SPEED_40GB:0;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_40G_KR4)) ?SOC_PA_SPEED_40GB:0;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_20G_CR2)) ?SOC_PA_SPEED_20GB:0;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_20G_KR2)) ?SOC_PA_SPEED_20GB:0;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_10G_KR))  ?SOC_PA_SPEED_10GB:0;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_10G_KX4)) ?SOC_PA_SPEED_10GB:0;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_1G_KX))   ?SOC_PA_SPEED_1000MB:0;
   }
   if(cl37_reg) {
      tsc->per_lane_control=TSCMOD_AN_GET_CL37_ABIL ;
      tscmod_tier1_selector("AUTONEG_GET", tsc, &rv);
      reg37_ability = tsc->accData ;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_40G))?        SOC_PA_SPEED_40GB:0;
      /* ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_32P7G))?      SOC_PA_SPEED_33GB:0; */
      /* ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_31P5G))?      SOC_PA_SPEED_32GB:0; */
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_25P455G))?    SOC_PA_SPEED_25GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_21G_X4))?     SOC_PA_SPEED_21GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_20G_X2_CX4))? SOC_PA_SPEED_20GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_20G_X2))?     SOC_PA_SPEED_20GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_20G_X4))?     SOC_PA_SPEED_20GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_20G_X4_CX4))? SOC_PA_SPEED_20GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_16G_X4))?     SOC_PA_SPEED_16GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_15P75G_R2))?  SOC_PA_SPEED_16GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_15G_X4))?     SOC_PA_SPEED_15GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_13G_X4))?     SOC_PA_SPEED_13GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_12P7_DXGXS))? SOC_PA_SPEED_13GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_12P5_X4))?    SOC_PA_SPEED_12P5GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_12G_X4))?     SOC_PA_SPEED_12GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_10P5G_DXGXS))?SOC_PA_SPEED_11GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_10G_X2_CX4))? SOC_PA_SPEED_10GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_10G_DXGXS))?  SOC_PA_SPEED_10GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_10G_CX4))?    SOC_PA_SPEED_10GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_10G_HIGIG))?  SOC_PA_SPEED_10GB:0; /* 4-lane */
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_6G_X4))?      SOC_PA_SPEED_6000MB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_5G_X4))?      SOC_PA_SPEED_5000MB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_2P5G))?       SOC_PA_SPEED_2500MB:0;
      if(!pCfg->hg_mode) {
          /* HG and CL37 are not compatible, and HG port does not support 1000MB ability */
          ability_mode|= SOC_PA_SPEED_1000MB ;
      }
   }

   ability->speed_full_duplex = ability_mode ;

   tsc->per_lane_control=TSCMOD_AN_GET_MISC_ABIL ;
   tscmod_tier1_selector("AUTONEG_GET", tsc, &rv);
   reg_ability = tsc->accData ;
   switch(reg_ability & (TSCMOD_SYMM_PAUSE|TSCMOD_ASYM_PAUSE)) {
   case TSCMOD_SYMM_PAUSE:
      ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
      break ;
   case TSCMOD_ASYM_PAUSE:
      ability->pause = SOC_PA_PAUSE_TX;
      break ;
   case TSCMOD_SYMM_PAUSE|TSCMOD_ASYM_PAUSE:
      ability->pause = SOC_PA_PAUSE_RX;
      break ;
   }

   if(tsc->verbosity&TSCMOD_DBG_AN) {
      printf("%-22s u=%0d p=%0d %s full_duplex ability %s(=%0x) pause=%x\n",
             FUNCTION_NAME(), unit, port, e2s_tscmod_an_type[tsc->an_type],
             tscmod_ability_msg0(ability->speed_full_duplex),  ability->speed_full_duplex, ability->pause);
      printf("   cl73=(%0x)%s\n", reg73_ability,
             tscmod_cl73_ability_msg0(reg73_ability)) ;
      printf("   cl37=(%0x)%s %s\n",  reg37_ability,
             tscmod_cl37bam_ability_msg0(reg37_ability), tscmod_cl37bam_ability_msg1(reg37_ability)) ;
      printf("   misc=%x\n", reg_ability) ;

   }
   tscmod_sema_unlock(unit, port) ;
   return SOC_E_NONE;
}

/*
 * Function:
 *      phy_tscmod_ability_remote_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_tscmod_ability_remote_get(int unit, soc_port_t port,
                        soc_port_ability_t *ability)
{
   phy_ctrl_t        *pc;
   TSCMOD_DEV_DESC_t *pDesc;
   tscmod_st         *tsc;
   int cl37_reg, cl73_reg, sgmii_speed ;  uint32 reg73_ability, reg37_ability, reg_ability; soc_port_mode_t  ability_mode;
   int rv ;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);

   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;

   TSCMOD_MEM_SET(ability, 0, sizeof(*ability));

   cl73_reg = 0 ; cl37_reg = 0 ;  sgmii_speed =0 ;  ability_mode = 0 ;  reg73_ability = 0 ; reg37_ability = 0  ;
   if((tsc->an_type==TSCMOD_CL73)||(tsc->an_type==TSCMOD_CL73_BAM)) {
      cl73_reg = 1 ;
   } else if(tsc->an_type==TSCMOD_CL37_BAM) {
      cl37_reg = 1 ;
   } else {
      cl37_reg = 1 ; cl73_reg = 1 ;
   }

   if(cl73_reg) {
      tsc->per_lane_control=TSCMOD_AN_GET_LP_CL73_ABIL ;
      tscmod_tier1_selector("AUTONEG_GET", tsc, &rv);
      reg73_ability = tsc->accData ;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_40G_CR4)) ?SOC_PA_SPEED_40GB:0;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_40G_KR4)) ?SOC_PA_SPEED_40GB:0;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_20G_CR2)) ?SOC_PA_SPEED_20GB:0;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_20G_KR2)) ?SOC_PA_SPEED_20GB:0;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_10G_KR))  ?SOC_PA_SPEED_10GB:0;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_10G_KX4)) ?SOC_PA_SPEED_10GB:0;
      ability_mode|= (reg73_ability &(1<<TSCMOD_ABILITY_1G_KX))   ?SOC_PA_SPEED_1000MB:0;
   }
   if(cl37_reg) {
      tsc->per_lane_control=TSCMOD_AN_GET_LP_CL37_ABIL ;
      tscmod_tier1_selector("AUTONEG_GET", tsc, &rv);
      reg37_ability = tsc->accData ;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_40G))?        SOC_PA_SPEED_40GB:0;
      /* ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_32P7G))?      SOC_PA_SPEED_33GB:0; */
      /* ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_31P5G))?      SOC_PA_SPEED_32GB:0; */
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_25P455G))?    SOC_PA_SPEED_25GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_21G_X4))?     SOC_PA_SPEED_21GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_20G_X2_CX4))? SOC_PA_SPEED_20GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_20G_X2))?     SOC_PA_SPEED_20GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_20G_X4))?     SOC_PA_SPEED_20GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_20G_X4_CX4))? SOC_PA_SPEED_20GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_16G_X4))?     SOC_PA_SPEED_16GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_15P75G_R2))?  SOC_PA_SPEED_16GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_15G_X4))?     SOC_PA_SPEED_15GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_13G_X4))?     SOC_PA_SPEED_13GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_12P7_DXGXS))? SOC_PA_SPEED_13GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_12P5_X4))?    SOC_PA_SPEED_12P5GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_12G_X4))?     SOC_PA_SPEED_12GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_10P5G_DXGXS))?SOC_PA_SPEED_11GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_10G_X2_CX4))? SOC_PA_SPEED_10GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_10G_DXGXS))?  SOC_PA_SPEED_10GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_10G_CX4))?    SOC_PA_SPEED_10GB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_10G_HIGIG))?  SOC_PA_SPEED_10GB:0; /* 4-lane */
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_6G_X4))?      SOC_PA_SPEED_6000MB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_5G_X4))?      SOC_PA_SPEED_5000MB:0;
      ability_mode|= (reg37_ability &(1<<TSCMOD_BAM37ABL_2P5G))?       SOC_PA_SPEED_2500MB:0;
   }

   ability->speed_full_duplex = ability_mode ;

   tsc->per_lane_control=TSCMOD_AN_GET_LP_MISC_ABIL ;
   tscmod_tier1_selector("AUTONEG_GET", tsc, &rv);
   reg_ability = tsc->accData ;
   switch(reg_ability & (TSCMOD_SYMM_PAUSE|TSCMOD_ASYM_PAUSE)) {
   case TSCMOD_SYMM_PAUSE:
      ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
      break ;
   case TSCMOD_ASYM_PAUSE:
      ability->pause = SOC_PA_PAUSE_TX;
      break ;
   case TSCMOD_SYMM_PAUSE|TSCMOD_ASYM_PAUSE:
      ability->pause = SOC_PA_PAUSE_RX;
      break ;
   }

   if(tsc->an_type==TSCMOD_CL37_SGMII) {
      tsc->per_lane_control=TSCMOD_AN_GET_LP_SGMII_ABIL ;
      tscmod_tier1_selector("AUTONEG_GET", tsc, &rv);
      reg_ability = tsc->accData ;

      sgmii_speed = reg_ability & 0x3 ;
   }

   if(tsc->verbosity&TSCMOD_DBG_AN) {
      printf("%-22s u=%0d p=%0d full_duplex REMOTE ability %s(=%0x) pause=%x sgmii_speed=%0d\n",
             FUNCTION_NAME(), unit, port, tscmod_ability_msg0(ability->speed_full_duplex),
             ability->speed_full_duplex, ability->pause, sgmii_speed);
   }

   tscmod_sema_unlock(unit, port) ;
   return SOC_E_NONE;
}

STATIC int
phy_tscmod_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
   int rv ;
   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
   rv = _phy_tscmod_ability_local_get(unit, port, ability) ;
   tscmod_sema_unlock(unit, port) ;
   return rv ;
}
STATIC int
_phy_tscmod_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
   phy_ctrl_t *pc;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t *pDesc;
   TSCMOD_DEV_CFG_t  *pCfg;
   int        speed_max ;

   if (NULL == ability) {
      return SOC_E_PARAM;
   }

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc    =  (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;

   TSCMOD_MEM_SET(ability, 0, sizeof(*ability));

   ability->loopback  = SOC_PA_LB_PHY;
   ability->interface = SOC_PA_INTF_XGMII;
   ability->medium    = pCfg->medium;
   ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;

   ability->flags     = SOC_PA_AUTONEG;
   speed_max          = pc->speed_max ;

   if(tsc->port_type == TSCMOD_SINGLE_PORT) {
      if(speed_max>=42000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
      }
      if(speed_max>=40000) {
         if(DEV_CFG_PTR(pc)->hg_mode)
            ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
         ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
      }
      if(speed_max>=32000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_32GB;
      }
      if(speed_max>=30000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_30GB;
      }
      if(speed_max>=25000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_25GB;
      }
      if(speed_max>=21000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
      }
      if(speed_max>=20000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
      }
      if(speed_max>=16000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_16GB;
      }
      if(speed_max>=15000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_15GB;
      }
      if(speed_max>=13000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
      }
      if(speed_max>=12500) {
         ability->speed_full_duplex |= SOC_PA_SPEED_12P5GB;
      }
      if(speed_max>=11000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
      }
      if(speed_max>=10000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
      }
      if(speed_max>=6000) {
         ability->speed_full_duplex  |= SOC_PA_SPEED_6000MB;
      }
      if(speed_max>=5000) {
         ability->speed_full_duplex  |= SOC_PA_SPEED_5000MB;
      }
      if(speed_max>=2500) {
         ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB;
      }
      if(speed_max>=1000) {
         ability->speed_full_duplex  |= SOC_PA_SPEED_1000MB ;

         ability->speed_full_duplex  |= SOC_PA_SPEED_100MB ;
         ability->speed_full_duplex  |= SOC_PA_SPEED_10MB ;

         ability->interface          =  SOC_PA_INTF_SGMII;
      }
      if(speed_max>=10000) {
         ability->interface          =  SOC_PA_INTF_XGMII;
      }
   } else if(tsc->port_type == TSCMOD_MULTI_PORT) {
      if(speed_max>=13000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
      }
      if(speed_max>=11000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
      }
      if(speed_max>=10000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
         if(DEV_CFG_PTR(pc)->hg_mode)
            ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
      }
      if(speed_max>=5000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_5000MB;
      }
      if(speed_max>=2500) {
         ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB;
      }
      if(speed_max>=1000) {
         ability->speed_full_duplex  |= SOC_PA_SPEED_1000MB ;

         ability->speed_full_duplex  |= SOC_PA_SPEED_100MB ;
         ability->speed_full_duplex  |= SOC_PA_SPEED_10MB ;

         ability->interface          =  SOC_PA_INTF_SGMII;
      }
      if(speed_max>=10000) {
         ability->interface          =  SOC_PA_INTF_XGMII;
      }
   } else { /* dual port */
      if(speed_max>=21000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
      }
      if(speed_max>=20000) {
         if(DEV_CFG_PTR(pc)->hg_mode)
            ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
         ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
      }
      if(speed_max>=16000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_16GB;
      }
      if(speed_max>=12700) {
         ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
      }
      if(speed_max>=10000) {
         ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
      }
      if(speed_max>=2500) {
         ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB;
      }
      if(speed_max>=1000) {
         ability->speed_full_duplex  |= SOC_PA_SPEED_1000MB ;
      }
   }

   ability->speed_full_duplex = ability->speed_full_duplex & ~pCfg->ability_mask  ;

   if((tsc->verbosity&TSCMOD_DBG_AN)||(tsc->verbosity&TSCMOD_DBG_SPD))
      printf("%-22s: u=%0d p=%0d %s full_duplex ability %s(=%0x) pasue=%x mask=%x\n",
             FUNCTION_NAME(), unit, port, e2s_tscmod_an_type[tsc->an_type], tscmod_ability_msg0(ability->speed_full_duplex),
             ability->speed_full_duplex, ability->pause, pCfg->ability_mask) ;

   return (SOC_E_NONE);
}



/*
 * Function:
 *      phy_tscmod_lb_set
 * Purpose:
 *      Put hc/FusionCore in PHY TX->RX loopback
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - binary value for on/off (1/0)
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_tscmod_lb_set(int unit, soc_port_t port, int enable)
{
   phy_ctrl_t *pc;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   int value, rv, en_val, tmp_select, flipped;
   uint32 data, i, spd_intf ;
   int speed;

   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;

   tmp_select           = tsc->lane_select ;

   rv     = SOC_E_NONE ;
   spd_intf= 0 ;
   flipped= 0 ;
   speed = 0;

   /* Can not set loopback on a port in AN mode */

   phy_tscmod_speed_get(unit, port, &speed);
 
   if ((tsc->an_type != TSCMOD_AN_NONE) && (speed >= 10000)) {
       printf(" Loopback on AN Port is not supported !!! unit %d port %d \n",
               unit, port);
       return SOC_E_FAIL;
   }    

   tsc->diag_type = TSCMOD_DIAG_TX_LOOPBACK;
   tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
   value = tsc->accData;

   en_val = enable? 1 :0 ;
   data = 0;
   if(tsc->port_type==TSCMOD_SINGLE_PORT) {
      if( (((value>>0) & 0x1)!=en_val)||(((value>>1) & 0x1)!=en_val)||
          (((value>>2) & 0x1)!=en_val)||(((value>>1) & 0x3)!=en_val)) flipped = 1 ;
      data |= en_val << (0 * 8) | en_val << (1 * 8) | en_val << (2 * 8) | en_val << (3 * 8);
   } else {
      if(tsc->port_type==TSCMOD_MULTI_PORT) {
         for(i = 0; i < 4; i++)  {
            if(pc->lane_num == i) {
               if(((value>>i)&0x1)!=en_val) flipped = 1 ;
               continue;
            } else {
               if(value&(1<<i)) data |= 1 << (8 * i);
            }
         }
         data |= en_val << ((pc->lane_num) * 8);
      } else if(tsc->port_type==TSCMOD_DXGXS) {
         for(i = 0; i < 4; i++)  {
            if(((pc->lane_num!=0) && ((i==2)||(i==3)))||
               ((pc->lane_num==0) && ((i==0)||(i==1)))) {
               if(((value>>i)&0x1)!=en_val) flipped = 1 ;
               continue;
            } else {
               if(value&(1<<i)) data |= 1 << (8 * i);
            }
         }
         data |= (en_val << ((pc->lane_num) * 8)) | (en_val << ((pc->lane_num+1) * 8))  ;
      } else {

      }
   }

   if(tsc->verbosity & (TSCMOD_DBG_LINK|TSCMOD_DBG_PATH))
      printf("%-22s: u=%0d p=%0d lb en=%0d rdbk=%x tx lb ctl=%x flipped=%0d\n",
             FUNCTION_NAME(), tsc->unit, tsc->port, enable, value, data, flipped);

   if(flipped==0) {
      tscmod_sema_unlock(unit, port) ;
      return SOC_E_NONE ;
   }

   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      tsc->lane_select      = TSCMOD_LANE_BCST ;
   }

   tsc->per_lane_control = 0;
   tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

   if((tsc->spd_intf==TSCMOD_SPD_10_SGMII)||
      (tsc->spd_intf==TSCMOD_SPD_100_SGMII)) {    
       tsc->per_lane_control = 0 ;
       tscmod_tier1_selector("CREDIT_SET", tsc, &rv);
   }

   tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
   tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);  /* diable TX path */

   tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
   tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);  /* reset TX path */

   if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
      tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
   }
   tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
   tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* diable RX path */

   tsc->per_lane_control = data;
   (tscmod_tier1_selector("TX_LOOPBACK_CONTROL", tsc, &rv));

   if(enable) {
      tsc->ctrl_type = tsc->ctrl_type | TSCMOD_CTRL_TYPE_LB ;
      if((tsc->an_type != TSCMOD_AN_TYPE_ILLEGAL) && (tsc->an_type != TSCMOD_AN_NONE)) {
         tsc->per_lane_control = 1<<TSCMOD_MISC_CTL_SHIFT | TSCMOD_MISC_CTL_NONCE_MATCH ;
         tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);
      }
      if(FORCE_CL72_ENABLED(pc)){
         FORCE_CL72_STATE(pc)   = TSCMOD_FORCE_CL72_IDLE ;
         FORCE_CL72_ENABLED(pc) = 0 ;

         /* CL72 clean up*/
         tsc->per_lane_control = TSCMOD_CL72_HW_DISABLE ;
         tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
         
         tsc->per_lane_control = TSCMOD_CL72_STATUS_RESET ;
         tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
         
         tsc->per_lane_control = TSCMOD_CL72_HW_RESTART ;
         tscmod_tier1_selector("CLAUSE_72_CONTROL", tsc, &rv);
      }
   } else {
      if( tsc->ctrl_type & TSCMOD_CTRL_TYPE_LB ) {
         tsc->ctrl_type = tsc->ctrl_type ^ TSCMOD_CTRL_TYPE_LB ;
         if((tsc->an_type != TSCMOD_AN_TYPE_ILLEGAL) && (tsc->an_type != TSCMOD_AN_NONE)) {
            tsc->per_lane_control = 0 << TSCMOD_MISC_CTL_SHIFT | TSCMOD_MISC_CTL_NONCE_MATCH ;
            tscmod_tier1_selector("MISC_CONTROL", tsc, &rv);
         }
      }
      if(FORCE_CL72_MODE(pc)) {
         FORCE_CL72_STATE(pc)   = TSCMOD_FORCE_CL72_CMD_DIS_IDLE ;
         FORCE_CL72_ENABLED(pc) = 1 ;
      }
   }

   if(enable) {
      tsc->per_lane_control = TSCMOD_RXP_REG_ON ;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* enable RX path */
   } else {
      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
         tsc->per_lane_control = TSCMOD_RXP_UC_ON ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }
   }

   if((tsc->spd_intf==TSCMOD_SPD_10_SGMII)||
      (tsc->spd_intf==TSCMOD_SPD_100_SGMII)) {
       spd_intf = tsc->spd_intf ;
       
       tsc->spd_intf = TSCMOD_SPD_1000_SGMII ;
       tsc->per_lane_control = 1 ;
       tscmod_tier1_selector("ENCODE_SET", tsc, &rv);
       
       tsc->per_lane_control = 1 ;
       tscmod_tier1_selector("DECODE_SET", tsc, &rv);
       
       tsc->per_lane_control = 1 ;
       tscmod_tier1_selector("CREDIT_SET", tsc, &rv);
       
       tsc->per_lane_control = 1;
       tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
       
       /* back to 10M or 100M */
       tsc->spd_intf = spd_intf ;
       tsc->per_lane_control = 1 ;
       tscmod_tier1_selector("ENCODE_SET", tsc, &rv);
       
       tsc->per_lane_control = 1 ;
       tscmod_tier1_selector("DECODE_SET", tsc, &rv);

       tsc->per_lane_control = 1 ;
       tscmod_tier1_selector("CREDIT_SET", tsc, &rv);
       
       tsc->per_lane_control = 1;
       tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);               
       
   } else {
       tsc->per_lane_control = 1 ;
       tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);  /* enable TX path */

       tsc->per_lane_control = 1;
       tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
   }

   tsc->lane_select  = tmp_select ;

   tscmod_sema_unlock(unit, port) ;
   return rv|SOC_E_NONE;
}

/*
 * Function:
 *      phy_tscmod_lb_get
 * Purpose:
 *      Get hc/FusionCore PHY TX->RX loopback state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_tscmod_lb_get(int unit, soc_port_t port, int *enable)
{
   phy_ctrl_t *pc;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   int val, rv;

   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;

   tsc->diag_type = TSCMOD_DIAG_TX_LOOPBACK;
   tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
   val = tsc->accData ;

   /* assume all lanes consistent */
   if( val & (1<<pc->lane_num) ) *enable = 1 ; else *enable = 0 ;

   if(tsc->verbosity & TSCMOD_DBG_PATH)
      printf("%-22s: u=%0d p=%0d lb en=%0d\n",
          FUNCTION_NAME(), tsc->unit, tsc->port, *enable);

   tscmod_sema_unlock(unit, port) ;
   return rv;
}

STATIC int
phy_tscmod_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    phy_ctrl_t      *pc;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    int  rv ;
    uint32 intf;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);
    rv    = SOC_E_NONE ;
    intf = DEV_CFG_PTR(pc)->line_intf;
    if (TSCMOD_40G_10G_INTF(tscmod_tr_if_mode(pif))) {
       intf &= ~TSCMOD_40G_10G_INTF_ALL;  /* clear all 10G/40G significant interfaces */
       intf |= 1 << tscmod_tr_if_mode(pif) ;
    } else {
        /* during init, intf will be set by XGMII sometimes forcefully, then serdes_if_type lost its info */
        intf |= 1 << tscmod_tr_if_mode(pif);
    }

    DEV_CFG_PTR(pc)->line_intf = intf;
    rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_INTF) ;

    if(tsc->verbosity & TSCMOD_DBG_SPD)
      printf("%-22s: u=%0d p=%0d intf=%0x pif=%0d\n",
             FUNCTION_NAME(), tsc->unit, tsc->port, intf, pif);
    return rv ;
}

STATIC int
phy_tscmod_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t *pc;

    int speed;
    int intf;
    int scr, asp_mode;
    int rv;

    pc = INT_PHY_SW_STATE(unit, port);

    tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
    rv = _phy_tscmod_speed_get(unit, port, &speed,&intf, &asp_mode, &scr);

    if (IS_ONE_LANE_PORT(pc)) {
        if (speed < 10000) {
            if (DEV_CFG_PTR(pc)->fiber_pref) {
                *pif = SOC_PORT_IF_GMII;
            } else {
                *pif = SOC_PORT_IF_SGMII;
            }
        } else {
            *pif = intf;
        }
    } else { /* combo mode */
        if (intf == SOC_PORT_IF_KR4) {
            /*
            SOC_IF_ERROR_RETURN
            (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(tsc->unit, tsc, &data16));
            if (data16 == TSCMOD_UC_CTRL_XLAUI) {
                intf = SOC_PORT_IF_XLAUI;
            } else if (data16 == TSCMOD_UC_CTRL_SR4) {
                intf = SOC_PORT_IF_SR;
            }
            */
        }
        *pif = intf;
    }
    tscmod_sema_unlock(unit, port) ;
    return rv;
}


#define PHY_TSCMOD_LANEPRBS_LANE_SHIFT   4

#if 0
STATIC int
_phy_tscmod_control_prbs_decouple_set(int unit, soc_port_t port, uint32 value)
{
    int rv;
    tscmod_st  *tsc;
    phy_ctrl_t *pc;
    TSCMOD_DEV_DESC_t *pDesc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

    tsc->per_lane_control = value;
    (tscmod_tier1_selector("PRBS_DECOUPLE_CONTROL", tsc, &rv));

    return SOC_E_NONE;
}
#endif  /* disable for now */


int
_phy_tscmod_control_prbs_polynomial_set(tscmod_st *tsc, uint32 value)
{
   uint16 data, ctl ;  int tmp_sel, rv ;
   uint32 prbs_poly;

   rv = SOC_E_NONE ;
   tsc->per_lane_control = TSCMOD_DIAG_PRBS_CTRL_GET ;
   tsc->diag_type        = TSCMOD_DIAG_PRBS;
   (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));

   data = tsc->accData ;
   ctl = 0 ;
   tmp_sel =  tsc->lane_select ;
   if(tsc->port_type==TSCMOD_SINGLE_PORT) {
      tsc->lane_select      = TSCMOD_LANE_BCST ;
   }
   data &= ~(PATGEN0_PRBSCTRL_PRBS_SEL_TX_MASK|PATGEN0_PRBSCTRL_PRBS_SEL_RX_MASK) ;
   switch (value) {
       case SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY7;
           break;
       case SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY15;
           break;
       case SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY23;
           break;
       case SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY31;
           break;
       case SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY9;
           break;
       case SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY11;
           break;
       case SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY58;
           break;
       default:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY7;
           break;
   }
   ctl =  (prbs_poly & PATGEN0_PRBSCTRL_PRBS_SEL_TX_MASK);
   ctl |= (ctl << PATGEN0_PRBSCTRL_PRBS_SEL_RX_SHIFT) ;
   ctl |= data ;

   tsc->per_lane_control = ctl ;
   tscmod_tier1_selector("PRBS_MODE", tsc, &rv) ;

   tsc->lane_select      = tmp_sel ;
   return SOC_E_NONE;
}


STATIC
int phy_tscmod_per_lane_control_firmware_mode_set(int unit, soc_port_t port, int lane, uint32 value)
{
    phy_ctrl_t *pc;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t *pDesc;
    TSCMOD_DEV_CFG_t  *pCfg;
    int tmp_sel, tmp_dxgxs, tmp_lane, rv ;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    pCfg  = &pDesc->cfg;
    tsc   =  (tscmod_st *)( pDesc + 1);
    
    tmp_sel  = tsc->lane_select ;
    tmp_lane = tsc->this_lane ;
    tmp_dxgxs= tsc->dxgxs ;

    tsc->dxgxs      = 0 ;
    tsc->this_lane  = lane ;
    tsc->lane_select= getLaneSelect(lane) ;

#if 0
    rv = SOC_E_NONE ;
    tsc->per_lane_control = TSCMOD_DIAG_PRBS_CTRL_GET ;
    tsc->diag_type        = TSCMOD_DIAG_PRBS;
    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
    data = tsc->accData ;

    data &= ~(PATGEN0_PRBSCTRL_PRBS_SEL_TX_MASK|PATGEN0_PRBSCTRL_PRBS_SEL_RX_MASK) ;
    ctl =  (value & PATGEN0_PRBSCTRL_PRBS_SEL_TX_MASK) ;
    ctl |= (ctl << PATGEN0_PRBSCTRL_PRBS_SEL_RX_SHIFT) ;
    ctl |= data ;

    tsc->per_lane_control = ctl ;
    tscmod_tier1_selector("PRBS_MODE", tsc, &rv) ;
#endif


   tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

   tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

   tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

   tsc->per_lane_control = 0;
   tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

   tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
   tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

   tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
   tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

   if(tsc->ctrl_type &TSCMOD_CTRL_TYPE_UC_RXP) {
      tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
   }
   tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
   tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

   if(value >= 0xf) {
       value = 0 ;
       tsc->firmware_mode = -1 ;
   } else {
       tsc->firmware_mode = value ;
   }
   rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_FW_MODE) ;

   tsc->accData = value ;
   tsc->per_lane_control = TSCMOD_FWMODE_WR ;
   tscmod_tier1_selector("FWMODE_CONTROL", tsc, &rv);

   tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

   sal_usleep(pCfg->los_usec);

   tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
   tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

   sal_usleep(1000) ;
   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      tsc->lane_select      = TSCMOD_LANE_BCST ;
   }

   if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_LINK_DIS) {
      /* do nothing */
   } else {
      if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_TX_SQUELCH)) {
          tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
          
          tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
          tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }
      tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_RX_SIG_DET ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      tsc->per_lane_control = 1;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

      tsc->per_lane_control = 1;
      tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

      if(tsc->ctrl_type &TSCMOD_CTRL_TYPE_UC_RXP) {
         tsc->per_lane_control = TSCMOD_RXP_UC_ON ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }
   }
    
    tsc->this_lane  = tmp_lane ;
    tsc->lane_select= tmp_sel  ;
    tsc->dxgxs      = tmp_dxgxs ;
    return rv ;
}

STATIC int
phy_tscmod_per_lane_control_firmware_mode_get(int unit, soc_port_t port, int lane,
                                                uint32 *value) 
{   
    phy_ctrl_t  *pc ;
    TSCMOD_DEV_DESC_t *pDesc;
    tscmod_st *tsc ;
    int rv, tmpLane, tmpSelect, tmpDxgxs ;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   = (tscmod_st *)(pDesc + 1);

    tmpLane   = tsc->this_lane;
    tmpSelect = tsc->lane_select;
    tmpDxgxs  = tsc->dxgxs ;
    rv        = SOC_E_NONE ;

    if(lane>=4||lane<0) lane =0 ;
    tsc->this_lane   = lane ;
    tsc->lane_select = getLaneSelect(lane); 
    tsc->dxgxs       = 0 ;

    tsc->per_lane_control = TSCMOD_FWMODE_RD ;
    tscmod_tier1_selector("FWMODE_CONTROL", tsc, &rv);

    *value = tsc->accData ;

    tsc->this_lane   = tmpLane; /* restore this_lane */
    tsc->lane_select = tmpSelect;
    tsc->dxgxs       = tmpDxgxs ;

    return rv ;
}




STATIC
int phy_tscmod_per_lane_control_prbs_polynomial_set(int unit, soc_port_t port, int lane, uint32 value)
{
    phy_ctrl_t *pc;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t *pDesc;
    uint16 data, ctl ;
    int tmp_sel, tmp_dxgxs, tmp_lane, rv ;
    uint32 prbs_poly;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);
    
    tmp_sel  = tsc->lane_select ;
    tmp_lane = tsc->this_lane ;
    tmp_dxgxs= tsc->dxgxs ;

    tsc->dxgxs      = 0 ;
    tsc->this_lane  = lane ;
    tsc->lane_select= getLaneSelect(lane) ;

    rv = SOC_E_NONE ;
    tsc->per_lane_control = TSCMOD_DIAG_PRBS_CTRL_GET ;
    tsc->diag_type        = TSCMOD_DIAG_PRBS;
    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
    data = tsc->accData ;

    data &= ~(PATGEN0_PRBSCTRL_PRBS_SEL_TX_MASK|PATGEN0_PRBSCTRL_PRBS_SEL_RX_MASK) ;
    switch (value) {
       case SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY7;
           break;
       case SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY15;
           break;
       case SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY23;
           break;
       case SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY31;
           break;
       case SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY9;
           break;
       case SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY11;
           break;
       case SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY58;
           break;
       default:
           prbs_poly = TSCMOD_DIAG_PRBS_POLY7;
           break;
    }
    ctl = (prbs_poly & PATGEN0_PRBSCTRL_PRBS_SEL_TX_MASK);
    ctl |= (ctl << PATGEN0_PRBSCTRL_PRBS_SEL_RX_SHIFT) ;
    ctl |= data ;

    tsc->per_lane_control = ctl ;
    tscmod_tier1_selector("PRBS_MODE", tsc, &rv) ;
    
    tsc->this_lane  = tmp_lane ;
    tsc->lane_select= tmp_sel  ;
    tsc->dxgxs      = tmp_dxgxs ;
    return rv ;
}

int
_phy_tscmod_control_prbs_polynomial_get(tscmod_st *tsc, uint32 *value)
{
    int rv;
    tsc->per_lane_control = TSCMOD_DIAG_PRBS_POLYNOMIAL_GET ;
    tsc->diag_type        = TSCMOD_DIAG_PRBS;

    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
    switch (tsc->accData) {
        case TSCMOD_DIAG_PRBS_POLY7:
            *value = SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1;
            break;
        case TSCMOD_DIAG_PRBS_POLY15:
            *value = SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1;
            break;
        case TSCMOD_DIAG_PRBS_POLY23:
            *value = SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1;
            break;
        case TSCMOD_DIAG_PRBS_POLY31:
            *value = SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1;
            break;
        case TSCMOD_DIAG_PRBS_POLY9:
            *value = SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1;
            break;
        case TSCMOD_DIAG_PRBS_POLY11:
            *value = SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1;
            break;
        case TSCMOD_DIAG_PRBS_POLY58:
            *value = SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1;
            break;
        default:
            *value = SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1;
            break;
    }

    return SOC_E_NONE;
}

STATIC int
phy_tscmod_per_lane_control_prbs_polynomial_get(int unit, soc_port_t port, int lane,
                                                uint32 *value) 
{   
    phy_ctrl_t  *pc ;
    TSCMOD_DEV_DESC_t *pDesc;
    tscmod_st *tsc ;
    int rv, tmpLane, tmpSelect, tmpDxgxs ;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   = (tscmod_st *)(pDesc + 1);

    tmpLane   = tsc->this_lane;
    tmpSelect = tsc->lane_select;
    tmpDxgxs  = tsc->dxgxs ;
    rv        = SOC_E_NONE ;

    if(lane>=4||lane<0) lane =0 ;
    tsc->this_lane   = lane ;
    tsc->lane_select = getLaneSelect(lane); 
    tsc->dxgxs       = 0 ;
    rv = _phy_tscmod_control_prbs_polynomial_get(tsc, value);

    tsc->this_lane   = tmpLane; /* restore this_lane */
    tsc->lane_select = tmpSelect;
    tsc->dxgxs       = tmpDxgxs ;

    return rv ;
}

#define INV_SHIFTER(ln)  (PHY_TSCMOD_LANEPRBS_LANE_SHIFT * (ln))

int
_phy_tscmod_control_prbs_tx_invert_data_set(tscmod_st *tsc, uint32 value)
{
    uint16 data, ctl ;  int tmp_sel, rv ;
    uint16 tmp_inv_data;

   rv = SOC_E_NONE ;
   tsc->per_lane_control = TSCMOD_DIAG_PRBS_CTRL_GET ;
   tsc->diag_type        = TSCMOD_DIAG_PRBS;
   (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));

   data = tsc->accData ;
   ctl = 0 ;
   tmp_sel =  tsc->lane_select ;
   if(tsc->port_type==TSCMOD_SINGLE_PORT) {
      tsc->lane_select      = TSCMOD_LANE_BCST ;
   }
   /* Match up the fix of JIRA PHY-804 */
   tmp_inv_data = (value & 0x1) ? 0 : 1;
   ctl  =  (tmp_inv_data << PATGEN0_PRBSCTRL_PRBS_INV_TX_SHIFT) ;
   data &= ~(PATGEN0_PRBSCTRL_PRBS_INV_TX_MASK);

   ctl |= data;

   tsc->per_lane_control = ctl ;
   tscmod_tier1_selector("PRBS_MODE", tsc, &rv) ;

   tsc->lane_select      = tmp_sel ;
   return SOC_E_NONE;

}

STATIC int 
phy_tscmod_per_lane_control_prbs_tx_invert_data_set(int unit, soc_port_t port, int lane, uint32 value)
{
    phy_ctrl_t *pc;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t *pDesc;
    uint16 data, ctl ;
    int tmp_sel, tmp_dxgxs, tmp_lane, rv ;
    uint16 tmp_inv_data;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);
    
    tmp_sel  = tsc->lane_select ;
    tmp_lane = tsc->this_lane ;
    tmp_dxgxs= tsc->dxgxs ;

    tsc->dxgxs      = 0 ;
    tsc->this_lane  = lane ;
    tsc->lane_select= getLaneSelect(lane) ;

    rv = SOC_E_NONE ;
    tsc->per_lane_control = TSCMOD_DIAG_PRBS_CTRL_GET ;
    tsc->diag_type        = TSCMOD_DIAG_PRBS;
    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
    data = tsc->accData ;

    /* Match up the fix of JIRA PHY-804 */
    tmp_inv_data = (value & 0x1) ? 0 : 1;
    ctl  =  (tmp_inv_data << PATGEN0_PRBSCTRL_PRBS_INV_TX_SHIFT) ;
    data &= ~(PATGEN0_PRBSCTRL_PRBS_INV_TX_MASK);

    ctl |= data;

    tsc->per_lane_control = ctl ;
    tscmod_tier1_selector("PRBS_MODE", tsc, &rv) ;
    
    tsc->this_lane  = tmp_lane ;
    tsc->lane_select= tmp_sel  ;
    tsc->dxgxs      = tmp_dxgxs ;
    return rv ;
}

STATIC int
phy_tscmod_per_lane_control_prbs_rx_invert_data_set(int unit, soc_port_t port, int lane, uint32 value)
{
    phy_ctrl_t *pc;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t *pDesc;
    uint16 data, ctl ;
    int tmp_sel, tmp_dxgxs, tmp_lane, rv ;
    uint16 tmp_inv_data;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);

    tmp_sel  = tsc->lane_select ;
    tmp_lane = tsc->this_lane ;
    tmp_dxgxs= tsc->dxgxs ;

    tsc->dxgxs      = 0 ;
    tsc->this_lane  = lane ;
    tsc->lane_select= getLaneSelect(lane) ;

    rv = SOC_E_NONE ;
    tsc->per_lane_control = TSCMOD_DIAG_PRBS_CTRL_GET ;
    tsc->diag_type        = TSCMOD_DIAG_PRBS;
    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
    data = tsc->accData ;

    /* Match up the fix of JIRA PHY-804 */
    tmp_inv_data = (value & 0x1) ? 0 : 1;
    ctl  =  (tmp_inv_data << PATGEN0_PRBSCTRL_PRBS_INV_RX_SHIFT) ;
    data &= ~(PATGEN0_PRBSCTRL_PRBS_INV_RX_MASK);

    ctl |= data;

    tsc->per_lane_control = ctl ;
    tscmod_tier1_selector("PRBS_MODE", tsc, &rv) ;

    tsc->this_lane  = tmp_lane ;
    tsc->lane_select= tmp_sel  ;
    tsc->dxgxs      = tmp_dxgxs ;
    return rv ;
}


int
_phy_tscmod_control_prbs_rx_invert_data_set(tscmod_st *tsc, uint32 value)
{
   uint16 data, ctl ;  int tmp_sel, rv ;
   uint16 tmp_inv_data;

   rv = SOC_E_NONE ;
   tsc->per_lane_control = TSCMOD_DIAG_PRBS_CTRL_GET ;
   tsc->diag_type        = TSCMOD_DIAG_PRBS;
   (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));

   data = tsc->accData ;
   ctl = 0 ;
   tmp_sel =  tsc->lane_select ;
   if(tsc->port_type==TSCMOD_SINGLE_PORT) {
      tsc->lane_select      = TSCMOD_LANE_BCST ;
   }

   /* Match up the fix of JIRA PHY-804 */
   tmp_inv_data = (value & 0x1) ? 0 : 1;
   ctl =  (tmp_inv_data << 11);
   data &= ~(0x1 << 11);
   ctl |= data;

   tsc->per_lane_control = ctl ;
   tscmod_tier1_selector("PRBS_MODE", tsc, &rv) ;

   tsc->lane_select      = tmp_sel ;
   return SOC_E_NONE;
}

int
_phy_tscmod_control_prbs_check_mode_set(tscmod_st *tsc, uint32 value)
{
   uint16 data, ctl ;  int tmp_sel, rv ;

   rv = SOC_E_NONE ;
   tsc->per_lane_control = TSCMOD_DIAG_PRBS_CTRL_GET ;
   tsc->diag_type        = TSCMOD_DIAG_PRBS;
   (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));

   data = tsc->accData ;
   ctl = 0 ;
   tmp_sel =  tsc->lane_select ;
   if(tsc->port_type==TSCMOD_SINGLE_PORT) {
      tsc->lane_select      = TSCMOD_LANE_BCST ;
   }
   ctl =  ((value & 0x7) << 12);
   data &= ~(0x7 <<12);
   ctl |= data;

   tsc->per_lane_control = ctl ;
   tscmod_tier1_selector("PRBS_MODE", tsc, &rv) ;

   tsc->lane_select      = tmp_sel ;
   return SOC_E_NONE;
}


int
_phy_tscmod_control_prbs_tx_invert_data_get(tscmod_st *tsc,uint32 *value)
{
   int rv;
   tsc->per_lane_control = TSCMOD_DIAG_PRBS_INVERT_DATA_GET ;
   tsc->diag_type        = TSCMOD_DIAG_PRBS;

   (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
   *value = tsc->accData ;
   return SOC_E_NONE;
}

STATIC int
phy_tscmod_per_lane_control_prbs_tx_invert_data_get(int unit, soc_port_t port, int lane,
                                                uint32 *value)
{
    phy_ctrl_t  *pc ;
    TSCMOD_DEV_DESC_t *pDesc;
    tscmod_st *tsc ;
    int rv, tmpLane, tmpSelect, tmpDxgxs ;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   = (tscmod_st *)(pDesc + 1);

    tmpLane   = tsc->this_lane;
    tmpSelect = tsc->lane_select;
    tmpDxgxs  = tsc->dxgxs ;
    rv        = SOC_E_NONE ;

    if(lane>=4||lane<0) lane =0 ;
    tsc->this_lane   = lane ;
    tsc->lane_select = getLaneSelect(lane); 
    tsc->dxgxs       = 0 ;
    rv = _phy_tscmod_control_prbs_tx_invert_data_get(tsc, value);

    tsc->this_lane   = tmpLane; /* restore this_lane */
    tsc->lane_select = tmpSelect;
    tsc->dxgxs       = tmpDxgxs ;

    return rv ;
}

STATIC int
phy_tscmod_per_lane_control_prbs_rx_invert_data_get(int unit, soc_port_t port, int lane,
                                                uint32 *value)
{
    phy_ctrl_t  *pc ;
    TSCMOD_DEV_DESC_t *pDesc;
    tscmod_st *tsc ;
    int rv, tmpLane, tmpSelect, tmpDxgxs ;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   = (tscmod_st *)(pDesc + 1);

    tmpLane   = tsc->this_lane;
    tmpSelect = tsc->lane_select;
    tmpDxgxs  = tsc->dxgxs ;
    rv        = SOC_E_NONE ;

    if (lane >= 4 || lane < 0) {
        lane =0;
    }
    tsc->this_lane   = lane ;
    tsc->lane_select = getLaneSelect(lane);
    tsc->dxgxs       = 0 ;
    rv = _phy_tscmod_control_prbs_rx_invert_data_get(tsc, value);

    tsc->this_lane   = tmpLane; /* restore this_lane */
    tsc->lane_select = tmpSelect;
    tsc->dxgxs       = tmpDxgxs ;

    return rv ;
}


int
_phy_tscmod_control_prbs_rx_invert_data_get(tscmod_st *tsc,uint32 *value)
{
   int rv;
   tsc->per_lane_control = TSCMOD_DIAG_PRBS_RX_INVERT_DATA_GET ;
   tsc->diag_type        = TSCMOD_DIAG_PRBS;

   (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
   *value = tsc->accData ;
   return SOC_E_NONE;
}

int _phy_tscmod_control_prbs_check_mode_get(tscmod_st *tsc, uint32 *value)
{
     int rv;
   tsc->per_lane_control = TSCMOD_DIAG_PRBS_CHECK_MODE_GET;
   tsc->diag_type        = TSCMOD_DIAG_PRBS;

   (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
   *value = tsc->accData ;
   return SOC_E_NONE;

}   

/* PRBS test
 * this routine enables the PRBS generator on applicable lanes depending on
 * the port mode. Before calling this function, a forced speed mode should
 * be set and either the external loopback or internal loopback should be
 * configured. Once this function is called, application should wait to
 * let the test run for a while and then calls the
 * _phy_tscmod_control_prbs_rx_status_get() to retrieve PRBS test status.
 * When calling this function to disable the PRBS test, the device or
 * specific lane will be re-initialized.
 */

STATIC int
_phy_tscmod_control_prbs_enable_set(tscmod_st *tsc, uint32 enable, int plane)
{
   phy_ctrl_t  *pc ;
   TSCMOD_DEV_DESC_t *pDesc;
   TSCMOD_DEV_CFG_t  *pCfg;

   uint16 en_val, data ;  int tmp_sel, tmp_lane, tmp_dxgxs, rv ;
   uint32 ctl ;    int tmp_spd, tmp_port_type ;
   int               txdrv_inx ; int               actual_spd_vec ;

   pc    = INT_PHY_SW_STATE(tsc->unit, tsc->port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   = (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;

   rv = SOC_E_NONE ;
   data = 0 ;
   txdrv_inx = 0 ; actual_spd_vec=0 ;

   /* if need to read status.
   tsc->per_lane_control = TSCMOD_DIAG_PRBS_LANE_EN_GET ;
   tsc->diag_type        = TSCMOD_DIAG_PRBS;
   (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
   data = tsc->accData ;   it is one bit in one of X4-type register
   */

   tmp_sel  = tsc->lane_select ;
   tmp_lane = tsc->this_lane ;
   tmp_dxgxs= tsc->dxgxs ;
   tmp_spd  = tsc->spd_intf ;
   tmp_port_type = tsc->port_type ;

   en_val = (enable) ? 1 : 0 ;
   if((plane >=0)&&(plane<4)) {
       tsc->this_lane   = plane ; 
       tsc->lane_select = getLaneSelect(plane) ;
       tsc->dxgxs       = 0;
   } else {
       if(tsc->port_type==TSCMOD_SINGLE_PORT) {
           tsc->lane_select      = TSCMOD_LANE_BCST ;
       }
   }
   if(enable) {
      tsc->per_lane_control = 1;
      tscmod_tier1_selector("PRBS_SEED_LOAD_CONTROL", tsc, &rv);
      tsc->per_lane_control = 0xcccccccc;
      tscmod_tier1_selector("PRBS_SEED_A1A0", tsc, &rv);
      tscmod_tier1_selector("PRBS_SEED_A3A2", tsc, &rv);
      tsc->per_lane_control = 0;
      tscmod_tier1_selector("PRBS_SEED_LOAD_CONTROL", tsc, &rv);
   }

   if(enable) {
      if(tsc->port_type== TSCMOD_MULTI_PORT) {
         /* not to reset TXP/RXP */
      } else {
         tsc->per_lane_control = 0;
         tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

         tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
         tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);  /* diable TX path */

         tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
         tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);  /* reset TX path */

         if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
            tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
         }
         tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* diable RX path */

         tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_DSC_SM;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         sal_usleep(1000) ;
         if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
            tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
            tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
         }
         if(pCfg->los_usec>1000) {
             sal_usleep(pCfg->los_usec-1000) ;
         }
      }

      /* Enable Elec_Idle - Incase of DC Coupled devices even if the Tx path is in reset Rx sees some signal that
 	   tunes the CDR to wrong clock. This would result in PRBS lock failure or PRBS error. Enabling Elec Idle would 
       resolve this issue*/
 	  tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_AFE_TX_VOLT ;
 	  tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
   }

   if(enable) {  /* credit adjustment due to aggrated modes. */
      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_PORT_MODE ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      if(tsc->port_type   == TSCMOD_SINGLE_PORT) {
         if((tsc->spd_intf==TSCMOD_SPD_40G_MLD)||(tsc->spd_intf==TSCMOD_SPD_40G_X4)) {
            tsc->port_type= TSCMOD_MULTI_PORT;
            _phy_tscmod_spd_selection(tsc->unit, tsc->port, 10000, &actual_spd_vec, &txdrv_inx) ;

            if(tsc->verbosity&TSCMOD_DBG_SPD)
               printf("PRBS set spd u=%0d p=%0d actual_spd_vec=%0x\n", tsc->unit, tsc->port,actual_spd_vec) ;

            tsc->per_lane_control = 1 ;
            tscmod_tier1_selector("CREDIT_SET", tsc, &rv);

         } else if((tsc->spd_intf==TSCMOD_SPD_42G_MLD)||(tsc->spd_intf==TSCMOD_SPD_42G_X4)) {
            tsc->port_type= TSCMOD_MULTI_PORT;
            _phy_tscmod_spd_selection(tsc->unit, tsc->port, 11000, &actual_spd_vec, &txdrv_inx) ;

            if(tsc->verbosity&TSCMOD_DBG_SPD)
               printf("PRBS set spd u=%0d p=%0d actual_spd_vec=%0x\n", tsc->unit, tsc->port,actual_spd_vec) ;

            tsc->per_lane_control = 1 ;
            tscmod_tier1_selector("CREDIT_SET", tsc, &rv);

         } else {
           printf("%-22s: u=%0d p=%0d single port mode incompatble for prbs spd=%0d(%s)\n",
                  FUNCTION_NAME(), tsc->unit, tsc->port, tsc->spd_intf,
                  e2s_tscmod_spd_intfc_type[tsc->spd_intf]);
         }
      } else if(tsc->port_type   ==  TSCMOD_DXGXS) {
         printf("%-22s: u=%0d p=%0d dual port mode incompatble for prbs spd=%0d(%s)\n",
                FUNCTION_NAME(), tsc->unit, tsc->port, tsc->spd_intf,
                e2s_tscmod_spd_intfc_type[tsc->spd_intf]);
      }
   }
   tsc->spd_intf=tmp_spd;
   tsc->port_type=tmp_port_type ;

   ctl = en_val | (0xf<<4) |  /* tx */
        (en_val << 8) | (0xf<<12) ;

   if(tsc->verbosity&TSCMOD_DBG_PAT) {
      printf("%-22s: u=%0d p=%0d prbs_ctl val=%x enable=%x rd=%x this_l=%0d\n",
         FUNCTION_NAME(), tsc->unit, tsc->port, ctl, enable, data, tsc->this_lane);
   }

   tsc->per_lane_control = ctl ;
   tscmod_tier1_selector("PRBS_CONTROL", tsc, &rv) ;

   if(enable) {
      if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_LB) {
         /*
         if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
            tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
         }
         */
         tsc->per_lane_control = TSCMOD_RXP_REG_ON ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);  /* enable RX path */
      } else {
         /*
         if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
            tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
         }
         */
         if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
            tsc->per_lane_control = TSCMOD_RXP_UC_ON ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
         }
      }

      tsc->per_lane_control = 1 ;
      tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);  /* enable TX path */

      /* Now that PRBS is on Disable Electrical Idle to start the PRBS */
 	  tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_AFE_TX_VOLT ;
 	  tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      
      tsc->per_lane_control = 1;
      tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);

      tsc->ctrl_type |=TSCMOD_CTRL_TYPE_PRBS_ON ;

   } else {
      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_PRBS_ON) tsc->ctrl_type ^=TSCMOD_CTRL_TYPE_PRBS_ON ;
   }

   tsc->this_lane  = tmp_lane ;
   tsc->lane_select= tmp_sel  ;
   tsc->dxgxs      = tmp_dxgxs ;
   return SOC_E_NONE;
}

int
phy_tscmod_control_prbs_enable_set(tscmod_st *tsc, uint32 enable)
{
    int rv ; 
    rv = _phy_tscmod_control_prbs_enable_set(tsc, enable, -1);
    return rv ;
}

STATIC int
phy_tscmod_per_lane_control_prbs_enable_set(int unit, soc_port_t port, int lane, uint32 value)
{
    phy_ctrl_t *pc;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t *pDesc;
    int rv ;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);

    rv = _phy_tscmod_control_prbs_enable_set(tsc, value, lane) ;
    return rv ;
}


int
_phy_tscmod_control_prbs_enable_get(tscmod_st *tsc, uint32 *value)
{
   int rv ;
   tsc->per_lane_control = TSCMOD_DIAG_PRBS_LANE_EN_GET ;
   tsc->diag_type        = TSCMOD_DIAG_PRBS;
   (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
   *value = tsc->accData ;
   return SOC_E_NONE;
}

STATIC int
phy_tscmod_per_lane_control_prbs_enable_get(int unit, soc_port_t port, int lane,
                                                uint32 *value)
{
    phy_ctrl_t  *pc ;
    TSCMOD_DEV_DESC_t *pDesc;
    tscmod_st *tsc ;
    int rv, tmpLane, tmpSelect, tmpDxgxs ;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   = (tscmod_st *)(pDesc + 1);

    tmpLane   = tsc->this_lane;
    tmpSelect = tsc->lane_select;
    tmpDxgxs  = tsc->dxgxs ;
    rv        = SOC_E_NONE ;

    if(lane>=4||lane<0) lane =0 ;
    tsc->this_lane   = lane ;
    tsc->lane_select = getLaneSelect(lane); 
    tsc->dxgxs       = 0 ;
    rv = _phy_tscmod_control_prbs_enable_get(tsc, value);

    tsc->this_lane   = tmpLane; /* restore this_lane */
    tsc->lane_select = tmpSelect;
    tsc->dxgxs       = tmpDxgxs ;

    return rv ;
}

/*
 * Returns value
 *      ==  0: PRBS receive is in sync
 *      == -1: PRBS receive never got into sync
 *      ==  n: number of errors
 */
int
_phy_tscmod_control_prbs_rx_status_get(tscmod_st *tsc, uint32 *value)
{
   int rv; int tmp_sel, tmp_lane ; int lane_s, lane_e, lane, result ;
   int pmd_llocked, rx_lane ;

   result = 0 ;
   tmp_sel = tsc->lane_select ;
   tmp_lane= tsc->this_lane ;
   pmd_llocked = 0 ;
   rx_lane     = 0 ;

   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      lane_s = 0 ; lane_e = 4 ;
   } else if (tsc->port_type == TSCMOD_DXGXS ) {
      if((tsc->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
      else { lane_s = 0; lane_e = 2 ; }
   } else {
      lane_s = tsc->this_lane ; lane_e = lane_s + 1 ;
   }

   if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_LB) {
      /* do nothing */
      if(tsc->verbosity&TSCMOD_DBG_PAT) {
         printf("%-22s: u=%0d p=%0d LB ctrl_type=%x\n",
                FUNCTION_NAME(), tsc->unit, tsc->port, tsc->ctrl_type);
      }
   } else {
      if(!(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP)) {
         pmd_llocked = 1 ;
         for(lane=lane_s; lane<lane_e; lane++) {
            tsc->lane_select = _tscmod_get_lane_select(tsc->unit, tsc->port, lane) ;
            tsc->this_lane   = lane ;
            tsc->per_lane_control = TSCMOD_DIAG_PMD_LOCK_LATCH ;
            tsc->diag_type        = TSCMOD_DIAG_PMD ;
            tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
            if(tsc->accData==0) {
               pmd_llocked = 0 ;  /* no break ; always scan through all lanes */
            }
         }

         tsc->per_lane_control = TSCMOD_RXP_REG_RD ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
         rx_lane = tsc->accData ;   /* enabled or not */
      }
      if(tsc->port_type==TSCMOD_SINGLE_PORT) {
         tsc->lane_select     = TSCMOD_LANE_BCST ;
      }
      tsc->this_lane = tmp_lane ;

      if(tsc->verbosity&TSCMOD_DBG_PAT) {
         printf("%-22s: u=%0d p=%0d pmd_llck=%0d\n",
                FUNCTION_NAME(), tsc->unit, tsc->port, pmd_llocked);
      }

      if(pmd_llocked && rx_lane) {
         /* good to go
            now read status */
      } else if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
         /* good to go
            now read status */
      } else {
         if(tsc->port_type== TSCMOD_MULTI_PORT) {
            /* printf("PRBS get u=%0d p=%0d nothing at RXP\n", tsc->unit, tsc->port) ; */
         } else {
            /* disable RXP */
            tsc->per_lane_control = TSCMOD_RXP_REG_OFF;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

            /* retune DSC */
            tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
            tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

            if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
               sal_usleep(1000)  ;
               tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
               tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
            }

            sal_usleep(100000) ; /* 100msec */
         }

         /* flush pmd latched version */
         for(lane=lane_s; lane<lane_e; lane++) {
            tsc->lane_select = _tscmod_get_lane_select(tsc->unit, tsc->port, lane) ;
            tsc->this_lane   = lane ;
            tsc->per_lane_control = TSCMOD_DIAG_PMD_LOCK_LATCH ;
            tsc->diag_type        = TSCMOD_DIAG_PMD ;
            tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
         }
         sal_usleep(10000) ; /* 10msec */
         pmd_llocked = 1 ;

         /* check pmd latched lock is set or not */
         for(lane=lane_s; lane<lane_e; lane++) {
            tsc->lane_select = _tscmod_get_lane_select(tsc->unit, tsc->port, lane) ;
            tsc->this_lane   = lane ;
            tsc->per_lane_control = TSCMOD_DIAG_PMD_LOCK_LATCH ;
            tsc->diag_type        = TSCMOD_DIAG_PMD ;
            tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
            if(tsc->accData==0) {
               pmd_llocked = 0 ;
            }
         }

         if(tsc->verbosity&TSCMOD_DBG_PAT) {
            printf("%-22s: u=%0d p=%0d restart pmd_llck=%0d\n",
                   FUNCTION_NAME(), tsc->unit, tsc->port, pmd_llocked);
         }

         if(pmd_llocked) {
            if(tsc->port_type==TSCMOD_SINGLE_PORT) {
               tsc->lane_select     = TSCMOD_LANE_BCST ;
            }
            tsc->this_lane = tmp_lane ;

            tsc->per_lane_control = TSCMOD_RXP_REG_ON ;
            tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
            sal_usleep(10000) ; /* 10msec */
         }
      }
   }

   /* pc->accData = -2 ; locked before but lost now */
   /* pc->accData = -1 ;   never locked */
   for(lane=lane_s; lane<lane_e; lane++) {
      tsc->lane_select = _tscmod_get_lane_select(tsc->unit, tsc->port, lane) ;
      tsc->this_lane   = lane ;
      tscmod_tier1_selector("PRBS_CHECK", tsc, &rv);
      if(tsc->accData==-2) {
         if(result>=0) result = -2 ;
      } else if(tsc->accData==-1){
         result = -1 ;
      } else if(tsc->accData==0) {
         /* result = result */
      } else if(tsc->accData==0x7fffffff) {
         result = 0x7fffffff ;
      } else {
         if(result>=0) { result +=tsc->accData;
             if(result<0) result = 0x7fffffff ;
         }
      }
   }
   *value = result ;
   tsc->lane_select = tmp_sel ;
   tsc->this_lane   = tmp_lane ;
   return SOC_E_NONE;
}

STATIC int
phy_tscmod_per_lane_control_prbs_rx_status_get(int unit, soc_port_t port, int lane,
                                                uint32 *value)
{
    phy_ctrl_t  *pc ;
    TSCMOD_DEV_DESC_t *pDesc;
    tscmod_st *tsc ;
    int rv, tmpLane, tmpSelect, tmpDxgxs, tmpPortType;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   = (tscmod_st *)(pDesc + 1);

    tmpLane   = tsc->this_lane;
    tmpSelect = tsc->lane_select;
    tmpDxgxs  = tsc->dxgxs ;
    tmpPortType = tsc->port_type ;
    rv        = SOC_E_NONE ;

    if(lane>=4||lane<0) lane =0 ;
    tsc->this_lane   = lane ;
    tsc->lane_select = getLaneSelect(lane); 
    tsc->dxgxs       = 0 ;
    tsc->port_type   = TSCMOD_MULTI_PORT ;
    rv = _phy_tscmod_control_prbs_rx_status_get(tsc, value);

    tsc->this_lane   = tmpLane; /* restore this_lane */
    tsc->lane_select = tmpSelect;
    tsc->dxgxs       = tmpDxgxs ;
    tsc->port_type   = tmpPortType ;

    return rv ;
}


STATIC int
_phy_tscmod_control_8b10b_set(int unit, phy_ctrl_t *pc, uint32 value)
{
    return SOC_E_NONE;
}

STATIC int
_tscmod_init_tx_driver_regs(int unit, soc_port_t port)
{
   int rv, tmp_sel, tmp_lane ;
   tscmod_st  *tsc;
   phy_ctrl_t *pc;
   TSCMOD_DEV_DESC_t *pDesc;
   int regTap, value, preTap, mainTap, postTap, i;
   uint32 sel_vec ;

   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);

   tmp_sel = tsc->lane_select ;
   tmp_lane= tsc->this_lane   ;

   tsc->lane_select = TSCMOD_LANE_BCST ;
   value = 0 ;
   for(i=0;i<5; i++) {
      switch(i) {
      case 0: value  = 0x03f0 ;  break ; /* KX pre=0 main=0x3f, post=0*/
      case 1: value  = 0x03f0 ;  break ; /* OS */
      case 2: value  = 0x5a54 ;  break ; /* BR pre=4 main=0x25 post=0x16*/
      case 3: value  = 0x5a54 ;  break ; /* KR pre=4 main=0x25 post=0x16*/
      case 4: value  = 0x03f0 ;  break ; /* 2p5 */
      }
      preTap  = (value & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_MASK)
         >> CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_SHIFT ;
      preTap  = 0x1000000 | preTap ;
      mainTap = (value & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK)
         >> CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT;
      mainTap = 0x2000000 | (mainTap << 8);
      postTap = (value & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK)
         >> CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
      postTap = 0x4000000 | (postTap << 16);
      regTap  = preTap| mainTap | postTap ;

      sel_vec = (i+1) << 28 ;   /* 0x1000_0000 ;  kx */
                                /* 0x2000_0000 ;  OS */
                                /* 0x3000_0000 ;  BR */
                                /* 0x4000_0000 ;  KR */
                                /* 0x5000_0000 ;  2p5 */
      tsc->per_lane_control = regTap | sel_vec ;
      tscmod_tier1_selector("TX_TAP_CONTROL", tsc, &rv) ;
   }

   tsc->per_lane_control = 0x6 << 8 | 0x2000000 ;  /* idrviver 0x6 */
   tscmod_tier1_selector("TX_AMP_CONTROL", tsc, &rv);

   tsc->this_lane        = tmp_lane ;
   tsc->lane_select      = tmp_sel ;
   return SOC_E_NONE;
}

/* in this function, lane index is relative to lane 0 of logic port */
STATIC int
_phy_tscmod_control_preemphasis_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value)
{
   int             size, i, mode;
   int             lane_s ;

   size = (SOC_INFO(unit).port_num_lanes[pc->port]);

   if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE0) {
      lane_s  = 0;  size = 1 ;
   } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE1) {
      lane_s  = 1;  size = 1 ;
   } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE2) {
      lane_s  = 2;  size = 1 ;
   } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE3) {
      lane_s  = 3;  size = 1 ;
   } else {
      lane_s  = 0 ;
   }

   for(i=lane_s;i<(lane_s+size); i++){
      mode = TSCMOD_TAP_REG ;
      _phy_tscmod_per_lane_control_preemphasis_set
         (unit, pc->port, i, SOC_PHY_CONTROL_PREEMPHASIS, value, mode);

      DEV_CFG_PTR(pc)->preemph[i] = value;
   }
   return SOC_E_NONE;
}

/* mode: Bit 0=KX, 1=OS, 2=BR, 3=KR, 4=2p5  */
/* value bit 15 decides to set tap_force bit or not */
STATIC int
_phy_tscmod_per_lane_control_preemphasis_set(int unit, soc_port_t port, int lane,
                                             soc_phy_control_t type, uint32 value, uint16 mode)
{
   int preTap, mainTap, postTap, regTap, rv ;
   int tmpLane, tmpSelect, tmpDxgxs ;
   tscmod_st    *tsc, *temp_tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   int    core_num, max_core_num, lane_num;
   phy_ctrl_t *pc;

   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   = (tscmod_st *)(pDesc + 1);
   rv    = SOC_E_NONE ;

   lane_num = (pc->lane_num + lane) % 4;
   core_num = (pc->lane_num + lane) / 4;

   max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
   if( core_num >= max_core_num ) { return SOC_E_PARAM; }

   temp_tsc =  (tscmod_st *)(tsc + core_num);

   tmpLane   = temp_tsc->this_lane;
   tmpSelect = temp_tsc->lane_select;
   tmpDxgxs  = temp_tsc->dxgxs ;

   temp_tsc->lane_select = _tscmod_get_lane_select(temp_tsc->unit, temp_tsc->port, lane_num) ;
   temp_tsc->this_lane   = lane_num ;

   preTap  = (value & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_MASK)
      >> CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_SHIFT ;
   preTap  = 0x1000000 | preTap ;
   mainTap = (value & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK)
      >> CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT;
   mainTap = 0x2000000 | (mainTap << 8);
   postTap = (value & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK)
      >> CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
   postTap = 0x4000000 | (postTap << 16);
   regTap  = preTap| mainTap | postTap ;

   if( mode & TSCMOD_TAP_REG ) {
      if((value & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK)){
         tsc->per_lane_control = regTap | 0x8000000 ;
      } else {
         /* remove the forced bit */
         tsc->per_lane_control = 0x0 ;
         tscmod_tier1_selector("TX_TAP_CONTROL", temp_tsc, &rv) ;
         tsc->per_lane_control = regTap ;
      }
      tscmod_tier1_selector("TX_TAP_CONTROL", temp_tsc, &rv) ;
   }

   if(mode & TSCMOD_TAP_KX) {
      tsc->per_lane_control = regTap | 0x10000000 ;
      tscmod_tier1_selector("TX_TAP_CONTROL", temp_tsc, &rv) ;  /* KX */
   }
   if( mode & TSCMOD_TAP_OS) {
      tsc->per_lane_control = regTap | 0x20000000 ;
      tscmod_tier1_selector("TX_TAP_CONTROL", temp_tsc, &rv) ;  /* OS */
   }
   if( mode & TSCMOD_TAP_BR) {
      tsc->per_lane_control = regTap | 0x30000000 ;
      tscmod_tier1_selector("TX_TAP_CONTROL", temp_tsc, &rv) ;  /* BR */
   }
   if( mode & TSCMOD_TAP_KR) {
      tsc->per_lane_control = regTap | 0x40000000 ;
      tscmod_tier1_selector("TX_TAP_CONTROL", temp_tsc, &rv) ;  /* KR */
   }
   if( mode & TSCMOD_TAP_2P5) {
      tsc->per_lane_control = regTap | 0x50000000 ;
      tscmod_tier1_selector("TX_TAP_CONTROL", temp_tsc, &rv) ;  /* 2p5*/
   }
   temp_tsc->this_lane   = tmpLane;
   temp_tsc->lane_select = tmpSelect ;
   temp_tsc->dxgxs       = tmpDxgxs;

   return SOC_E_NONE;
}



STATIC int
_phy_tscmod_control_tx_driver_field_get(soc_phy_control_t type,int *ln_ctrl, soc_phy_control_t *rtn_type)
{
   int lane_ctrl;

   lane_ctrl = 4 ;  /* the whole port */

   /* _LANEn(n=0-3) control type only applies to combo mode or dxgxs in
    * independent channel mode
    */
   switch(type) {
   case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
      /* fall through */
   case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
      /* fall through */
   case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
      /* fall through */
   case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
      /* fall through */
   case SOC_PHY_CONTROL_DRIVER_CURRENT:
      if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0) {
         lane_ctrl = 0;
      } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1) {
         lane_ctrl = 1;
      } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2) {
         lane_ctrl = 2;
      } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3) {
         lane_ctrl = 3;
      }
      *rtn_type = SOC_PHY_CONTROL_DRIVER_CURRENT;
      break;

   case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
      /* fall through */
   case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
      /* fall through */
   case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
      /* fall through */
   case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
      /* fall through */
   case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
      if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0) {
         lane_ctrl = 0;
      } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1) {
         lane_ctrl = 1;
      } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2) {
         lane_ctrl = 2;
      } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3) {
         lane_ctrl = 3;
      }
      *rtn_type = SOC_PHY_CONTROL_PRE_DRIVER_CURRENT ;
      break;
   case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:
      lane_ctrl = 4;
      *rtn_type = SOC_PHY_CONTROL_DRIVER_POST2_CURRENT ;
      break ;
   default:
      /* should never get here */
      return SOC_E_PARAM;
   }
   *ln_ctrl = lane_ctrl;
   return SOC_E_NONE;
}

/* logic port starts with logic lane 0 first */
STATIC int
_phy_tscmod_control_tx_driver_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value)
{
   int     lane_ctrl;
   soc_phy_control_t rtn_type ;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   TSCMOD_DEV_CFG_t      *pCfg;
   int             size, i ;


   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;

   lane_ctrl = 0 ;

   rtn_type = type ;

   SOC_IF_ERROR_RETURN
        (_phy_tscmod_control_tx_driver_field_get(type,&lane_ctrl,&rtn_type));

   if(tsc->verbosity & TSCMOD_DBG_DSC) {
      printf("%s p=%0d lane_ctrl=%x type=%0d rtn_type=%0d\n",
             FUNCTION_NAME(), tsc->port, lane_ctrl, type, rtn_type) ;
   }

   size = (SOC_INFO(unit).port_num_lanes[pc->port]);

   if(lane_ctrl<4) size = 1;
   else            lane_ctrl = 0 ;

   for(i=(0+lane_ctrl);i<(size+lane_ctrl); i++){
      _phy_tscmod_per_lane_control_tx_driver_set(unit, pc->port, i, rtn_type, value) ;

      if(rtn_type==SOC_PHY_CONTROL_PRE_DRIVER_CURRENT) {
         pCfg->pdriver[i] = value;
      } else if(rtn_type==SOC_PHY_CONTROL_DRIVER_CURRENT) {
         pCfg->idriver[i] = value;
      } else {
         pCfg->post2driver[i] = value;
      }
   }

   return SOC_E_NONE;
}

/* one lane at a time */
STATIC int
_phy_tscmod_per_lane_control_tx_driver_set(int unit, soc_port_t port, int lane,
                                soc_phy_control_t type, uint32 value)
{
   int rv;
   int tmpLane, tmpSelect, tmpDxgxs, tmpValue ;
   tscmod_st    *tsc, *temp_tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   int    core_num, max_core_num, lane_num;
   phy_ctrl_t *pc;

   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   = (tscmod_st *)(pDesc + 1);
   rv    = SOC_E_NONE ;

   lane_num = (pc->lane_num + lane) % 4;
   core_num = (pc->lane_num + lane) / 4;

   max_core_num = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
   if( core_num >= max_core_num ) { return SOC_E_PARAM; }

   temp_tsc =  (tscmod_st *)(tsc + core_num);

   tmpValue = (value & 0xff); /* only 8 bits taken */

   tmpLane   = temp_tsc->this_lane;
   tmpSelect = temp_tsc->lane_select;
   tmpDxgxs  = temp_tsc->dxgxs ;

   temp_tsc->lane_select = _tscmod_get_lane_select(temp_tsc->unit, temp_tsc->port, lane_num) ;
   temp_tsc->this_lane   = lane_num ;

   if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT) {
      /* lane select is unchanged */
      tmpValue |= 0x1000000;
   } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT) {
      /* lane select is unchanged */
      tmpValue <<= 8;
      tmpValue |= 0x2000000;
   } else if (type == SOC_PHY_CONTROL_DRIVER_POST2_CURRENT) {
      /* lane select is unchanged */
      tmpValue <<= 16;
      tmpValue |= 0x4000000;
   }

   temp_tsc->per_lane_control = tmpValue;
   tscmod_tier1_selector("TX_AMP_CONTROL", temp_tsc, &rv);

   temp_tsc->this_lane   = tmpLane;
   temp_tsc->lane_select = tmpSelect ;
   temp_tsc->dxgxs       = tmpDxgxs;

   return rv ;
}

STATIC int
_phy_tscmod_higig2_codec_control_set(int unit, phy_ctrl_t *pc, uint32 value)
{
    int rv = SOC_E_NONE;
    int tmp_sel, tmpLane ;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);

    tmpLane = tsc->this_lane;
    tmp_sel = tsc->lane_select ;

    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
       tsc->lane_select      = TSCMOD_LANE_BCST ;
    }

    if(value) {
        /* Enable codec */
        tsc->per_lane_control = TSCMOD_HG2_CONTROL_ENABLE_CODEC;
    } else {
        /* Disable codec */
        tsc->per_lane_control = TSCMOD_HG2_CONTROL_DISABLE_CODEC;
    }
    tscmod_tier1_selector("HIGIG2_CONTROL", tsc, &rv);

    tsc->this_lane      = tmpLane;
    tsc->lane_select    = tmp_sel ;

    return rv;
}

STATIC int
_phy_tscmod_higig2_codec_control_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    int rv = SOC_E_NONE;
    int tmp_sel, tmpLane ;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);

    tmpLane = tsc->this_lane;
    tmp_sel = tsc->lane_select ;

    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
       tsc->lane_select      = TSCMOD_LANE_BCST ;
    }

    tsc->per_lane_control = TSCMOD_HG2_CONTROL_READ_CODEC;

    tscmod_tier1_selector("HIGIG2_CONTROL", tsc, &rv);

    *value = tsc->accData;

    tsc->this_lane      = tmpLane;
    tsc->lane_select    = tmp_sel ;

    return rv;
}



STATIC int
_phy_tscmod_control_tx_lane_squelch_set(int unit, phy_ctrl_t *pc, uint32 value)
{
    int rv, tmp_sel, tmpLane ;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc  = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);
    rv     = SOC_E_NONE ;
    tmpLane = tsc->this_lane;
    tmp_sel = tsc->lane_select ;

    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
       tsc->lane_select      = TSCMOD_LANE_BCST ;
    }

    if(value) {
        tsc->ctrl_type |= TSCMOD_CTRL_TYPE_TX_SQUELCH ;

        tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_ANATX_INPUT ;
        tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

        tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
        tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
#if 0
        tsc->per_lane_control = 0;
        tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
        
        tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
        tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
        
        tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
        tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
#endif

    } else {
#if 0
        tsc->per_lane_control = 1;
        tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
        
        tsc->per_lane_control = 1;
        tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
#endif
        if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_TX_SQUELCH) tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_TX_SQUELCH ;

        tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AFE_TX_VOLT ;
        tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
        tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_ANATX_INPUT ;
        tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
    }
   
    rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_CTRL_TYPE) ;
    tsc->this_lane      = tmpLane;
    tsc->lane_select    = tmp_sel ;

    return rv;
}

STATIC int
_phy_tscmod_control_preemphasis_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    int rv, tmpLane, tmpSel, tmpDxgxs;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);
    tmpLane = tsc->this_lane; /* save it to restore later */
    tmpSel  = tsc->lane_select ;
    tmpDxgxs= tsc->dxgxs ;

    if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE0) {
      tsc->this_lane = 0;
    } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE1) {
      tsc->this_lane = 1;
    } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE2) {
      tsc->this_lane = 2;
    } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE3) {
      tsc->this_lane = 3;
    } else {
      tsc->this_lane = pc->lane_num;  /* assume default lanes */
    }
    tsc->dxgxs            = 0 ;
    tsc->per_lane_control = 0 ;
    tsc->diag_type = TSCMOD_DIAG_TX_TAPS;
    tsc->lane_select = _tscmod_get_lane_select(unit, tsc->port, tsc->this_lane) ;
    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
    *value = tsc->accData;
    tsc->this_lane  = tmpLane; /* restore this_lane */
    tsc->lane_select= tmpSel ;
    tsc->dxgxs      = tmpDxgxs ;
    return SOC_E_NONE;
}

STATIC int
phy_tscmod_per_lane_control_preemphasis_get(int unit, soc_port_t port, int lane, uint32 *value)
{
    int rv ;
    soc_phy_control_t type ;
    phy_ctrl_t *pc;

    pc    = INT_PHY_SW_STATE(unit, port);
    

    switch(lane) {
    case 0:
        type = SOC_PHY_CONTROL_PREEMPHASIS_LANE0 ;
        break ;
    case 1:
        type = SOC_PHY_CONTROL_PREEMPHASIS_LANE1 ;
        break ;
    case 2:
        type = SOC_PHY_CONTROL_PREEMPHASIS_LANE2 ;
        break ;
    case 3:
        type = SOC_PHY_CONTROL_PREEMPHASIS_LANE3 ;
        break ;
    default:
        type = SOC_PHY_CONTROL_PREEMPHASIS ;
        break ;
    }
    rv = SOC_E_NONE ;
    rv = _phy_tscmod_control_preemphasis_get(unit, pc, type, value);
    return rv ;
}

STATIC int
_phy_tscmod_control_tx_driver_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    int rv, tmpLane, tmpSel, tmpDxgxs ;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);
    tmpLane  = tsc->this_lane; /* save it to restore later */
    tmpSel   = tsc->lane_select ;
    tmpDxgxs = tsc->dxgxs ;

    tsc->dxgxs = 0;

    if (type == SOC_PHY_CONTROL_DRIVER_POST2_CURRENT) {
        tsc->per_lane_control = 0x3;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT) {
        tsc->per_lane_control = 0x2;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT) {
        tsc->per_lane_control = 0x1;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0) {
        tsc->this_lane = 0;
        tsc->per_lane_control = 0x2;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1) {
        tsc->this_lane = 1;
        tsc->per_lane_control = 0x2;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2) {
        tsc->this_lane = 2;
        tsc->per_lane_control = 0x2;
    } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3) {
        tsc->this_lane = 3;
        tsc->per_lane_control = 0x2;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0) {
        tsc->this_lane = 0;
        tsc->per_lane_control = 0x1;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1) {
        tsc->this_lane = 1;
        tsc->per_lane_control = 0x1;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2) {
        tsc->this_lane = 2;
        tsc->per_lane_control = 0x1;
    } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3) {
        tsc->this_lane = 3;
        tsc->per_lane_control = 0x1;
    } else {
        return SOC_E_PARAM;
    }

    tsc->diag_type   = TSCMOD_DIAG_TX_AMPS;
    tsc->lane_select =  _tscmod_get_lane_select(unit, tsc->port, tsc->this_lane) ;
    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
    *value = tsc->accData;
    tsc->this_lane   = tmpLane; /* restore this_lane */
    tsc->lane_select = tmpSel ;
    tsc->dxgxs       = tmpDxgxs ;

    return SOC_E_NONE;
}

STATIC int
phy_tscmod_per_lane_control_tx_driver_get(int unit, soc_port_t port, int lane, 
                                          soc_phy_control_t type, uint32 *value)
{
    int rv, tmpLane, tmpSel, tmpDxgxs;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    phy_ctrl_t *pc;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   = (tscmod_st *)(pDesc + 1);
    tmpLane = tsc->this_lane; /* save it to restore later */
    tmpSel  = tsc->lane_select ;
    tmpDxgxs= tsc->dxgxs ;
    rv      = SOC_E_NONE ;

    tsc->dxgxs = 0 ;
    if((type==SOC_PHY_CONTROL_DRIVER_CURRENT)||
       (type==SOC_PHY_CONTROL_PRE_DRIVER_CURRENT)||
       (type==SOC_PHY_CONTROL_DRIVER_POST2_CURRENT)) {
        if((lane>=4)||(lane <0)) lane = 0 ;
        tsc->this_lane  = lane ;
        tsc->lane_select= getLaneSelect(lane) ;
        rv = _phy_tscmod_control_tx_driver_get(unit, pc, type, value);
    } else {
        *value = 0;
        rv = SOC_E_UNAVAIL ;
    }

    tsc->this_lane   = tmpLane; /* restore this_lane */
    tsc->lane_select = tmpSel ;
    tsc->dxgxs       = tmpDxgxs ;
    
    return rv ;
}


/*
 * Works for 10GX4 8B/10B endec in forced mode. May not work with scrambler
 * enabled. Doesn't work for 1G/2.5G. Doesn't work for 10G XFI and 40G
 * using 64/66 endec yet.
 */
STATIC int
_phy_tscmod_control_bert_set(int unit, phy_ctrl_t *pc,
                                          soc_phy_control_t type, uint32 value)
{
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_control_bert_get(int unit, phy_ctrl_t *pc,
                                          soc_phy_control_t type, uint32 *value)
{
    int rv = SOC_E_NONE;
    return rv;
}

STATIC int
_phy_tscmod_control_eee_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv, tmp_sel, tmpLane, lpi_value = 0;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);
    rv    = SOC_E_NONE ;

    tmpLane = tsc->this_lane;
    tmp_sel = tsc->lane_select ;

    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
       tsc->lane_select      = TSCMOD_LANE_BCST ;
    }

    if (TSCMOD_LPI_BYPASS_GET(enable)) {
        lpi_value = enable & (~TSCMOD_LPI_BYPASS);
        if (lpi_value) {
            /* Enable the LPI pass-thru */
            tsc->per_lane_control = 1;
        } else {
            /* Disable the LPI pass-thru */
            tsc->per_lane_control = 0;
        }
    } else {
        return SOC_E_UNAVAIL;
    }

    tscmod_tier1_selector("EEE_PASS_THRU_SET", tsc, &rv);

    tsc->this_lane      = tmpLane;
    tsc->lane_select    = tmp_sel ;
    return rv ;
}

STATIC int
_phy_tscmod_control_eee_get(int unit, phy_ctrl_t *pc, uint32 *enable)
{
    int rv, tmp_sel, tmpLane ;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);
    rv    = SOC_E_NONE ;
    tmpLane = tsc->this_lane;
    tmp_sel = tsc->lane_select ;

    if (!TSCMOD_LPI_BYPASS_GET(*enable)) {
        return SOC_E_UNAVAIL;
    }

    tsc->diag_type = TSCMOD_DIAG_EEE;
    tsc->lane_select = _tscmod_get_lane_select(unit, tsc->port, tsc->this_lane) ;
    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));

    if(tsc->accData) { *enable = 1 ; } else { *enable = 0 ; }
    TSCMOD_LPI_BYPASS_SET(*enable);

    tsc->this_lane      = tmpLane;
    tsc->lane_select    = tmp_sel ;
    return rv;
}


STATIC int
_phy_tscmod_control_tx_pattern_20bit_set(int unit, phy_ctrl_t *pc,
                                        uint32 pattern_data)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    uint32 tmp_select, tmp_lane;
    int enable, mode ;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   = (tscmod_st *)( pDesc + 1);

    if(pattern_data == 0) return SOC_E_NONE;

    tmp_select           = tsc->lane_select ;
    tmp_lane             = tsc->this_lane   ;

    if(tsc->port_type==TSCMOD_SINGLE_PORT) {
       tsc->lane_select     = TSCMOD_LANE_BCST ;
    }
    pattern_data = pattern_data & 0x0fffff ;
    enable = 0x1 ; /* enable PTP */
    mode   = 0x1 ; /* 40 bit mode */
    tsc->per_lane_control = (tsc->this_lane<<1)<<4 ;
    tsc->accData          = (pattern_data|pattern_data<<20)  ;
    tscmod_tier1_selector("PROG_DATA", tsc, &rv) ;
    tsc->per_lane_control = ((tsc->this_lane<<1) +1)<<4 ;
    tsc->accData          = (pattern_data>>20)  ;
    tscmod_tier1_selector("PROG_DATA", tsc, &rv) ;

    tsc->per_lane_control = mode <<4 | enable ;
    tscmod_tier1_selector("PROG_DATA", tsc, &rv) ;

    tsc->this_lane  = tmp_lane; /* restore this_lane */
    tsc->lane_select= tmp_select ;
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_control_pdet_set(int unit, phy_ctrl_t *pc,
                                        uint32 data)
{
    int rv = SOC_E_NONE;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    uint32 tmp_select, tmp_lane;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   = (tscmod_st *)( pDesc + 1);

    tmp_select           = tsc->lane_select ;
    tmp_lane             = tsc->this_lane   ;


    if(tsc->port_type==TSCMOD_SINGLE_PORT) {
        tsc->lane_select     = TSCMOD_LANE_BCST ;
    }

    /* Call the Tier1 function */
    tsc->per_lane_control = data ;
    tscmod_tier1_selector("PARALLEL_DETECT_CONTROL", tsc, &rv);

    tsc->this_lane  = tmp_lane; /* restore this_lane */
    tsc->lane_select= tmp_select ;
    return rv;
}

STATIC int
_phy_tscmod_control_pdet_get(int unit, phy_ctrl_t *pc,
                        uint32 *value)
{
   int rv;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   uint32 tmp_select, tmp_lane;
   uint32 data;

   data = 0;

   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   = (tscmod_st *)( pDesc + 1);

   tmp_select           = tsc->lane_select ;
   tmp_lane             = tsc->this_lane   ;

   if (*value == TSCMOD_PDET_CONTROL_1G) {
       data = TSCMOD_PDET_CONTROL_1G;
   } else if (*value == TSCMOD_PDET_CONTROL_10G) {
       data = TSCMOD_PDET_CONTROL_10G;
   } else {
       /* Dummy else */
   }

   /* Call the Tier1 function */
   tsc->per_lane_control = data ;
   tscmod_tier1_selector("PARALLEL_DETECT_CONTROL", tsc, &rv);

   *value = 0;
   if (tsc->accData) {
       *value = 1;
   }

   tsc->this_lane  = tmp_lane; /* restore this_lane */
   tsc->lane_select= tmp_select ;
   return SOC_E_NONE;
}


STATIC int
_phy_tscmod_control_tx_pattern_20bit_get(int unit, phy_ctrl_t *pc,
                        uint32 *value)
{
   int rv;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   int enable  ;

   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   = (tscmod_st *)( pDesc + 1);

   enable = 0x4 ; /* read */
   tsc->per_lane_control = (tsc->this_lane<<1)<<4 | enable ;
   tscmod_tier1_selector("PROG_DATA", tsc, &rv) ;
   *value = tsc->accData & 0x0fffff ;
   return SOC_E_NONE;
}

STATIC int
_phy_tscmod_control_tx_pattern_256bit_set(int unit, phy_ctrl_t *pc,
                         uint32 enable)
{
   int rv;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   TSCMOD_DEV_CFG_t      *pCfg;
   uint32 tmp_select, tmp_lane;
   int en_type, mode, i;

   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   = (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;

   tmp_select           = tsc->lane_select ;
   tmp_lane             = tsc->this_lane   ;

   if(tsc->port_type==TSCMOD_SINGLE_PORT) {
       tsc->lane_select     = TSCMOD_LANE_BCST ;
   }


   for(i=0; i<PROG_DATA_NUM; i++) {
      tsc->accData = pCfg->pattern_data[i] ;
      tsc->per_lane_control = i<<4 ;
      tscmod_tier1_selector("PROG_DATA", tsc, &rv) ;
   }
   mode = 0x6 ; /*256bit */

   en_type = 0x1 ; /* enable PTP */
   tsc->per_lane_control = mode <<4 | en_type ;
   tscmod_tier1_selector("PROG_DATA", tsc, &rv) ;

   tsc->this_lane  = tmp_lane; /* restore this_lane */
   tsc->lane_select= tmp_select ;
   return SOC_E_NONE;
}

STATIC int
_phy_tscmod_control_tx_pattern_256bit_get(int unit, phy_ctrl_t *pc,
                        uint32 *value)
{
  int rv;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   int enable  ;

   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   = (tscmod_st *)( pDesc + 1);

   enable = 0x4 ; /* read */
   tsc->per_lane_control = 0 <<4 | enable ;  /* first 32-bit */
   tscmod_tier1_selector("PROG_DATA", tsc, &rv) ;
   *value = tsc->accData & 0x0fffff ;
   return rv|SOC_E_NONE;
}


STATIC int
_phy_tscmod_rx_DFE_tap1_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);


    tsc->per_lane_control = enable;

    (tscmod_tier1_selector("RX_DFE_TAP1_CONTROL", tsc, &rv));
    return SOC_E_NONE;
}


STATIC int
_phy_tscmod_rx_DFE_tap2_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

    tsc->per_lane_control = enable;

    (tscmod_tier1_selector("RX_DFE_TAP2_CONTROL", tsc, &rv));
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_rx_DFE_tap3_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

    tsc->per_lane_control = enable;

    (tscmod_tier1_selector("RX_DFE_TAP3_CONTROL", tsc, &rv));
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_rx_DFE_tap4_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

    tsc->per_lane_control = enable;

    (tscmod_tier1_selector("RX_DFE_TAP4_CONTROL", tsc, &rv));
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_rx_DFE_tap5_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

    tsc->per_lane_control = enable;

    (tscmod_tier1_selector("RX_DFE_TAP5_CONTROL", tsc, &rv));
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_rx_vga_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 value)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);

    tsc->per_lane_control = value;

    (tscmod_tier1_selector("RX_VGA_CONTROL", tsc, &rv));
    return SOC_E_NONE;
}


STATIC int
_phy_tscmod_rx_pf_ovr_control_set(int unit, phy_ctrl_t *pc, uint32 value)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);

    tsc->per_lane_control = value;

    (tscmod_tier1_selector("RX_PF_CONTROL", tsc, &rv));
    return SOC_E_NONE;
}

#if 0
STATIC int
_phy_tscmod_tx_pi_control_set(int unit, phy_ctrl_t *pc,
                                           soc_phy_control_t type,uint32 enable)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

    tsc->per_lane_control = enable;

    (tscmod_tier1_selector("TX_PI_CONTROL", tsc, &rv));
    return SOC_E_NONE;
}
#endif /* disable for now */


STATIC int
_phy_tscmod_rx_p1_slicer_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

    tsc->per_lane_control = enable;

    (tscmod_tier1_selector("RX_P1_SLICER_CONTROL", tsc, &rv));
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_rx_m1_slicer_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);



    tsc->per_lane_control = enable;

    (tscmod_tier1_selector("RX_M1_SLICER_CONTROL", tsc, &rv));
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_rx_d_slicer_control_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);


    tsc->per_lane_control = enable;

    (tscmod_tier1_selector("RX_D_SLICER_CONTROL", tsc, &rv));
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_pi_control_set (int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);



    tsc->per_lane_control = enable;

    (tscmod_tier1_selector("TX_PI_CONTROL", tsc, &rv));
    return SOC_E_NONE;
}

/* RX->TX PMD loopback if mode =0 */
/* RX->TX PCS loopback if mode =1 */
STATIC int
_phy_tscmod_rloop_set(int unit, phy_ctrl_t *pc, int mode, uint32 enable)
{
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    int value, rv, en_val, on_val, off_val, i, tmp_sel, tmp_lane ;
    uint32 data ;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);

    data  = 0 ;
    rv    = SOC_E_NONE ;
    tmp_sel = tsc->lane_select ;
    tmp_lane= tsc->this_lane ;

    /* PHY-867 */
    if(enable) {
       tsc->ctrl_type |= TSCMOD_CTRL_TYPE_RMT_LB ;
    } else {
       if( tsc->ctrl_type&TSCMOD_CTRL_TYPE_RMT_LB) tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_RMT_LB ;
    }

    if(enable) {
       if(tsc->port_type==TSCMOD_SINGLE_PORT) {
          tsc->lane_select     = TSCMOD_LANE_BCST ;
       }

       if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
          tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
          tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
       }
       tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
       tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

       tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
       tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

       tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
       tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

    }

    if(mode) {
       en_val = enable? 0x3 :0x1 ; /* 2'b11 means {ENABLE, PCS} */
       on_val = 0x3 ;
       off_val= 0x1 ;
    } else {
       en_val = enable? 0x2 :0x0 ; /* 2'b10 means {ENABLE, PMD} */
       on_val = 0x2 ;
       off_val= 0x0 ;
    }
    if(tsc->port_type==TSCMOD_SINGLE_PORT) {
       data |= en_val << (0 * 8) | en_val << (1 * 8) | en_val << (2 * 8) | en_val << (3 * 8);
    } else {
       tsc->diag_type = TSCMOD_DIAG_RX_LOOPBACK;
       tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);
       if(mode)
          value = (tsc->accData & 0xf0) >> 4 ; /*PCS*/
       else
          value = tsc->accData & 0xf ; /* PMD */

       if(tsc->port_type==TSCMOD_MULTI_PORT) {
          for(i = 0; i < 4; i++)  {
             if(pc->lane_num == i) {
                continue;
             } else {
               if(value&(1<<i)) data |= on_val << (8 * i);
               else             data |= off_val<< (8 * i);
             }
          }
          data |= en_val << ((pc->lane_num) * 8);
       } else if(tsc->port_type==TSCMOD_DXGXS) {
          for(i = 0; i < 4; i++)  {
             if(((pc->lane_num!=0) && ((i==2)||(i==3)))||
                ((pc->lane_num==0) && ((i==0)||(i==1)))) {
                continue;
             } else {
                if(value&(1<<i)) data |= on_val << (8 * i);
                else             data |= off_val<< (8 * i);
             }
          }
          data |= (en_val << ((pc->lane_num) * 8)) | (en_val << ((pc->lane_num+1) * 8))  ;
       }
   }
   tsc->per_lane_control = data;
   (tscmod_tier1_selector("RX_LOOPBACK_CONTROL", tsc, &rv));

   tsc->this_lane  = tmp_lane; /* restore this_lane */
   tsc->lane_select= tmp_sel ;

   if(tsc->port_type==TSCMOD_SINGLE_PORT) {
      tsc->lane_select     = TSCMOD_LANE_BCST ;
   }

   if(mode==0) {
      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

      if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
         sal_usleep(1000) ;
         /* this call affects AN */
         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      }
   }

   sal_usleep(200000) ;   /* 200ms */

   tsc->per_lane_control = 1;
   tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);

   tsc->per_lane_control = TSCMOD_RXP_REG_ON ;
   tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

   tsc->this_lane  = tmp_lane; /* restore this_lane */
   tsc->lane_select= tmp_sel ;
   return SOC_E_NONE;
}

/* enable is the polarity value */
STATIC int
_phy_tscmod_rx_polarity_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);

    tsc->per_lane_control  =  0x1;  /* enable SET_POLARITY function */
    tsc->per_lane_control |= (enable << 8);
    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
       tsc->per_lane_control |= 0xf << 4 ;
    } else if (tsc->port_type == TSCMOD_MULTI_PORT) {
       tsc->per_lane_control |= 0x1 << 4 ;
    } else {
       tsc->per_lane_control |= 0x3 << 4 ;
    }
    /* 0 on bit 1 means RX, no need to configure*/
    (tscmod_tier1_selector("SET_POLARITY", tsc, &rv));
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_tx_polarity_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    int rv;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);

    tsc->per_lane_control  =  0x1;  /* enable SET_POLARITY function */
    tsc->per_lane_control |= (enable << 8);
    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
       tsc->per_lane_control |= 0xf << 4 ;
    } else if (tsc->port_type == TSCMOD_MULTI_PORT) {
       tsc->per_lane_control |= 0x1 << 4 ;
    } else {
       tsc->per_lane_control |= 0x3 << 4 ;
    }
    tsc->per_lane_control |=  0x2;

    (tscmod_tier1_selector("SET_POLARITY", tsc, &rv));

    return SOC_E_NONE;
}

#if 0
STATIC int
phy_tscmod_tx_lane_disable(int unit, phy_ctrl_t *pc, uint32 disable)
{
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    int rv;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

    tsc->this_lane = pc->lane_num;
    switch (pc->lane_num) {
    case 0:
       tsc->per_lane_control = disable;
       break;
    case 1:
       tsc->per_lane_control = disable << 8;
       break;
    case 2:
       tsc->per_lane_control = disable << 16;
       break;
    case 3:
       tsc->per_lane_control = disable << 24;
       break;
   default:
       break;
   }

    (tscmod_tier1_selector("TX_LANE_DISABLE", tsc, &rv));
    return SOC_E_NONE;
}

#endif /* disable for now */



STATIC int
_phy_tscmod_rloop_get(int unit, phy_ctrl_t *pc, int mode, uint32 *value)
{
    int rv;
    int tmp_value;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);


    tsc->diag_type = TSCMOD_DIAG_RX_LOOPBACK;

    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));

    /* In Main0_loopback_control register, the remote loopback field has 4 bits, each bit for one lane.
       According with lane selection of port, the tmp_value will retrieve the bit value in the field.
       If the tmp_value is unequal to 0, RMT loopback is enabled, otherwise RMT loopback is disabled.
    */
    if(mode) {
        tmp_value = tsc->accData & 0xf;
    } else {
        tmp_value = (tsc->accData & 0xf0) >> 4;
    }
    tmp_value &= (1 << tsc->this_lane);
    if(tmp_value) {
        *value = 1;
    } else {
        *value = 0;
    }

    return SOC_E_NONE;
}


STATIC int
_phy_tscmod_fec_set(int unit, phy_ctrl_t *pc, uint32 fec_ctrl)
{
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   TSCMOD_DEV_CFG_t      *pCfg;
   int rv ;
   int         tmp_sel, tmp_lane ;
   int         disabled ;
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;
   rv    = SOC_E_NONE;

   tmp_sel              = tsc->lane_select ;
   tmp_lane             = tsc->this_lane   ;
   disabled             = 0 ;
   if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
      tsc->lane_select      = TSCMOD_LANE_BCST ;
   }

   switch(fec_ctrl) {
   case SOC_PHY_CONTROL_FEC_OFF:
      tsc->an_fec = 0 ;
      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_FS_FEC_ON) 
          tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_FS_FEC_ON;
      tsc->per_lane_control = TSCMOD_AN_SET_CL37_ATTR ;
      if(pCfg->an_cl72) tsc->per_lane_control |= TSCMOD_AN_SET_CL72_MODE ;
      if(tsc->an_fec)   tsc->per_lane_control |= TSCMOD_AN_SET_FEC_MODE ;
      if(pCfg->hg_mode) tsc->per_lane_control |= TSCMOD_AN_SET_HG_MODE ;
      tscmod_tier1_selector("AUTONEG_SET", tsc, &rv);

      if((tsc->an_type==TSCMOD_AN_NONE)&&!(tsc->ctrl_type &TSCMOD_CTRL_TYPE_LINK_DIS)){
          disabled =1 ;
          phy_tscmod_enable_set(unit, tsc->port, 0) ;
      }
      tsc->per_lane_control = TSCMOD_AN_SET_CL73_FEC_OFF ;
      tscmod_tier1_selector("AUTONEG_SET", tsc, &rv);

      if(tsc->an_type==TSCMOD_AN_NONE) {
          tsc->per_lane_control = 1 ;
          tscmod_tier1_selector("ENCODE_SET", tsc, &rv) ;
          tsc->per_lane_control = 1 ;
          tscmod_tier1_selector("DECODE_SET", tsc, &rv) ;
      }
      tsc->per_lane_control = (TSCMOD_FEC_CTL_FS_SET_OFF<<TSCMOD_FEC_CTRL_SHIFT) ;
      tscmod_tier1_selector("FEC_CONTROL", tsc, &rv) ;
      
      if(disabled) {
          sal_usleep(pCfg->los_usec) ;
          phy_tscmod_enable_set(unit, tsc->port, 1) ;
      }
      break ;
   case SOC_PHY_CONTROL_FEC_ON:
       if(!(tsc->ctrl_type &TSCMOD_CTRL_TYPE_LINK_DIS)) {
          disabled =1 ;
          phy_tscmod_enable_set(unit, tsc->port, 0) ;
      }
      tsc->ctrl_type |= TSCMOD_CTRL_TYPE_FS_FEC_ON ;
      tsc->per_lane_control = (TSCMOD_FEC_CTL_FS_SET_ON <<TSCMOD_FEC_CTRL_SHIFT) ;
      tscmod_tier1_selector("FEC_CONTROL", tsc, &rv) ;
      
      tsc->per_lane_control = 1 ;
      tscmod_tier1_selector("ENCODE_SET", tsc, &rv) ;
      tsc->per_lane_control = 1 ;
      tscmod_tier1_selector("DECODE_SET", tsc, &rv) ;

      if(disabled) {
          sal_usleep(pCfg->los_usec) ;
          phy_tscmod_enable_set(unit, tsc->port, 1) ;
      }
      break ;
   case SOC_PHY_CONTROL_FEC_AUTO:  /* turn on */
      tsc->an_fec = 1 ;
      tsc->per_lane_control = TSCMOD_AN_SET_CL37_ATTR ;
      if(pCfg->an_cl72) tsc->per_lane_control |= TSCMOD_AN_SET_CL72_MODE ;
      if(tsc->an_fec)   tsc->per_lane_control |= TSCMOD_AN_SET_FEC_MODE ;
      if(pCfg->hg_mode) tsc->per_lane_control |= TSCMOD_AN_SET_HG_MODE ;
      tscmod_tier1_selector("AUTONEG_SET", tsc, &rv);

      tsc->per_lane_control = TSCMOD_AN_SET_CL73_FEC_ON ;
      tscmod_tier1_selector("AUTONEG_SET", tsc, &rv);
      break ;
   default:
      rv = SOC_E_PARAM;
      break;
   }

   tsc->lane_select = tmp_sel  ;  /* restore */
   tsc->this_lane   = tmp_lane ;
   return rv ;
}

STATIC int
_phy_tscmod_fec_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   int rv ;

   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);
   rv    = SOC_E_NONE ;

   if(tsc->an_type != TSCMOD_AN_NONE) {
      if((tsc->an_type==TSCMOD_CL73)||(tsc->an_type==TSCMOD_CL73_BAM)||
         (tsc->an_type==TSCMOD_HPAM)) {
          tsc->per_lane_control = (TSCMOD_FEC_CTL_AN_CL73_RD <<TSCMOD_FEC_CTRL_SHIFT);
          tscmod_tier1_selector("FEC_CONTROL", tsc, &rv) ;
      } else {
          tsc->per_lane_control = (TSCMOD_FEC_CTL_AN_CL37_RD <<TSCMOD_FEC_CTRL_SHIFT);
          tscmod_tier1_selector("FEC_CONTROL", tsc, &rv) ;
      }
      if(tsc->accData)
         *value = SOC_PHY_CONTROL_FEC_AUTO;
      else
         *value = SOC_PHY_CONTROL_FEC_OFF;
   } else {
       tsc->per_lane_control = (TSCMOD_FEC_CTL_FS_RD <<TSCMOD_FEC_CTRL_SHIFT);
       tscmod_tier1_selector("FEC_CONTROL", tsc, &rv) ;

       if(tsc->accData) *value = SOC_PHY_CONTROL_FEC_ON ;
       else             *value = SOC_PHY_CONTROL_FEC_OFF;
   }
   return rv;
}


STATIC int
_phy_tscmod_control_linkdown_transmit_set(int unit, soc_port_t port, uint32 value)
{
    return SOC_E_NONE;
}
STATIC int
_phy_tscmod_control_linkdown_transmit_get(int unit, phy_ctrl_t *pc,uint32 *value)
{
    return SOC_E_NONE;
}

#if 0
STATIC int
_phy_tscmod_control_rx_taps_get(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    int rv;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);


    tsc->diag_type = TSCMOD_DIAG_RX_TAPS;

    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_control_slicers_get(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    int rv;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);


    tsc->diag_type = TSCMOD_DIAG_SLICERS;

    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
    return SOC_E_NONE;
}
#endif /* disable for now */


STATIC int
_phy_tscmod_control_rx_ppm_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    int rv;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);


    tsc->diag_type = TSCMOD_DIAG_RX_PPM;

    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
    return SOC_E_NONE;
}
STATIC int
_phy_tscmod_control_diag_rx_taps(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    int rv;
    int tmpLane;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);
    tmpLane = tsc->this_lane;

    if        (type==SOC_PHY_CONTROL_RX_PEAK_FILTER) { tsc->accData = 0x1;
    } else if (type==SOC_PHY_CONTROL_RX_VGA)         { tsc->accData = 0x2;
    } else if (type==SOC_PHY_CONTROL_RX_TAP1)        { tsc->accData = 0x3;
    } else if (type==SOC_PHY_CONTROL_RX_TAP2)        { tsc->accData = 0x4;
    } else if (type==SOC_PHY_CONTROL_RX_TAP3)        { tsc->accData = 0x5;
    } else if (type==SOC_PHY_CONTROL_RX_TAP4)        { tsc->accData = 0x6;
    } else if (type==SOC_PHY_CONTROL_RX_TAP5)        { tsc->accData = 0x7;
    }
    tsc->diag_type = TSCMOD_DIAG_RX_TAPS;
    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
    tsc->this_lane = tmpLane; /* restore this_lane */
    *value = tsc->accData;
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_control_diag_slicers(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    int rv;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);
    if       ( type == SOC_PHY_CONTROL_RX_PLUS1_SLICER ) { tsc->accData = 0x1;
    } else if( type == SOC_PHY_CONTROL_RX_MINUS1_SLICER) { tsc->accData = 0x2;
    } else if( type == SOC_PHY_CONTROL_RX_D_SLICER     ) { tsc->accData = 0x3;
    }


    tsc->diag_type = TSCMOD_DIAG_SLICERS;
    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
    *value = tsc->accData;
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_control_rx_sigdet_set(int unit, phy_ctrl_t *pc, uint32 value)
{
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    int rv, reset_val ;
    int         tmp_sel, tmp_lane ;
    
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   = (tscmod_st *)( pDesc + 1);
    rv    = SOC_E_NONE;

    tmp_sel              = tsc->lane_select ;
    tmp_lane             = tsc->this_lane   ;
    if(tsc->port_type   == TSCMOD_SINGLE_PORT ) {
        tsc->lane_select      = TSCMOD_LANE_BCST ;
    }

    reset_val = (value)? 0:1 ;
    tsc->per_lane_control = (reset_val << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_RX_SIG_DET ;
    tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

    tsc->lane_select = tmp_sel  ;  /* restore */
    tsc->this_lane   = tmp_lane ;
    return rv;
}

STATIC int
_phy_tscmod_control_rx_sigdet_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    int rv;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);


    tsc->diag_type = TSCMOD_DIAG_RX_SIGDET;
    (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
    *value = tsc->accData;
    return SOC_E_NONE;
}

STATIC int
_phy_tscmod_control_rx_seq_done_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    int rv;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);



    /* check RX seq done status */
    tsc->per_lane_control = 0x10;

    (tscmod_tier1_selector("RX_SEQ_CONTROL", tsc, &rv));
    *value = tsc->accData;
    return SOC_E_NONE;
}

STATIC int
tscmod_diag_poke_help(tscmod_st *tsc)
{
   printf("Syntax: phy diag <port> poke fb=<N> val=<n>\n") ;
   printf("TSCMOD_DIAG_P_HELP:         fb=%0d\n", TSCMOD_DIAG_P_HELP) ;
   printf("TSCMOD_DIAG_P_VERB:         fb=%0d val=verbosity_vec\n", TSCMOD_DIAG_P_VERB) ;
   printf("TSCMOD_DIAG_P_LINK_UP_BERT: fb=%0d val=ON(1)/OFF(0)\n", TSCMOD_DIAG_P_LINK_UP_BERT) ;
   printf("TSCMOD_DIAG_P_RAM_WRITE:    fb=%0d val={addr16, data16}\n", TSCMOD_DIAG_P_RAM_WRITE) ;
   printf("TSCMOD_DIAG_P_CTRL_TYPE:    fb=%0d val=CTRL_TYPE\n", TSCMOD_DIAG_P_CTRL_TYPE) ;
   printf("TSCMOD_DIAG_P_AN_CTL:       fb=%0d val=AN_CTL\n", TSCMOD_DIAG_P_AN_CTL) ;
   printf("TSCMOD_DIAG_P_SILICON:      fb=%0d val=SILICON\n", TSCMOD_DIAG_P_SILICON) ;
   printf("TSCMOD_DIAG_P_FEC_CTL:      fb=%0d val=FEC contrl mode\n", TSCMOD_DIAG_P_FEC_CTL) ;
   return SOC_E_NONE;
}

STATIC int
tscmod_diag_s_verbosity(tscmod_st *tsc, int val)
{
   int i, count ;
   static char *TSCMOD_DBG_NAME[] = {
      "FLOW", "REG", "MDIO", "WRCHK", "FUNC",
      "SUB", "SPD", "AN", "DSC", "TXDRV",
      "LINK","PATH", "TO", "INIT", "CFG",
      "PAT",  "UC", "FSM", "SCAN" "\0"
   } ;
   static int TSCMOD_DBG_VEC[] = {
      0x1, 0x2, 0x4, 0x8, 0x10,
      0x40, 0x100, 0x400, 0x1000, 0x2000,
      0x4000, 0x8000, 0x10000, 0x40000, 0x100000,
      0x400000, 0x1000000, 0x2000000, 0x4000000, 0
   } ;

   count = 0 ;
   tsc->verbosity = val ;
   printf("%-22s: u=%0d p=%0d verbosity=%x\n",
          FUNCTION_NAME(), tsc->unit, tsc->port, tsc->verbosity);
   for(i=0; i<19; i++) {
      if(tsc->verbosity & TSCMOD_DBG_VEC[i] )
         { printf("%s ", TSCMOD_DBG_NAME[i]) ; count ++ ; }
      if(count>=8) { printf("\n           "); count = 0 ; }
   }
   printf("\n") ;
   return SOC_E_NONE ;
}

STATIC int
tscmod_diag_s_ucode_ram(tscmod_st *tsc, int addr, int val)
{
   int rv  ;
   rv = SOC_E_NONE ;
   tsc->per_lane_control =  TSCMOD_UC_OFFSET | (addr << TSCMOD_UC_MACRO_SHIFT) ;
   tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);

   tsc->per_lane_control =  TSCMOD_UC_W_RAM_DATA | (val << TSCMOD_UC_MACRO_SHIFT) ;
   tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);

   tsc->per_lane_control =  TSCMOD_UC_W_RAM_OP ;
   tscmod_tier1_selector("FIRMWARE_SET", tsc, &rv);

   return rv ;
}

STATIC int
tscmod_diag_s_p_silicon(tscmod_st *tsc)
{
   return SOC_E_NONE ;
}

STATIC int
tscmod_diag_poke(tscmod_st *tsc, int fb, int val)
{
   uint16 addr, data ;  int rv ;
   rv = SOC_E_NONE ;
   printf("%s u=%0d p=%0d ctrl_type=%0x l=%0d sel=%x dxgxs=%0d\n", FUNCTION_NAME(), tsc->unit, tsc->port,
          tsc->ctrl_type, tsc->this_lane, tsc->lane_select, tsc->dxgxs) ;
   addr = 0 ; data = 0 ;
   switch(fb) {
   case TSCMOD_DIAG_P_VERB:
      tscmod_diag_s_verbosity(tsc, val) ;
      break ;
   case TSCMOD_DIAG_P_LINK_UP_BERT:
      if(val) {
         tsc->ctrl_type =  tsc->ctrl_type | TSCMOD_CTRL_TYPE_LINK_BER ;
      } else {
         if( tsc->ctrl_type & TSCMOD_CTRL_TYPE_LINK_BER )
            tsc->ctrl_type = tsc->ctrl_type ^ TSCMOD_CTRL_TYPE_LINK_BER ;
      }
      break ;
   case TSCMOD_DIAG_P_RAM_WRITE:
      data = val & 0xffff ;
      addr = (val >> 16 ) & 0xffff ;
      rv=tscmod_diag_s_ucode_ram(tsc, addr, data);
      break ;
   case TSCMOD_DIAG_P_CTRL_TYPE:
      tsc->ctrl_type = TSCMOD_CTRL_TYPE_DEFAULT|(val &TSCMOD_CTRL_TYPE_MASK);
      printf("%s u=%0d p=%0d ctrl_type=%0x post\n", FUNCTION_NAME(), tsc->unit, tsc->port,
          tsc->ctrl_type) ;
      break ;
   case TSCMOD_DIAG_P_AN_CTL:
      tsc->an_ctl   = val ;
      break ;
   case TSCMOD_DIAG_P_FEC_CTL:
      tsc->per_lane_control = val ;
      tscmod_tier1_selector("FEC_CONTROL", tsc, &rv);
      printf("%s u=%0d p=%0d accData=%0x\n", FUNCTION_NAME(), tsc->unit, tsc->port, tsc->accData) ;
      break ;
   case TSCMOD_DIAG_P_SILICON:
      tsc->per_lane_control = val ;
      rv = tscmod_diag_s_p_silicon(tsc) ;
      break ;
   default:
      rv=tscmod_diag_poke_help(tsc) ;
      break ;
   }
   return rv;
}

/* to read a ucode file */
STATIC int tscmod_diag_load_uC(tscmod_st *ws, int crc, int debug)
{

#if !defined(__KERNEL__) && !defined(NO_SAL_APPL)

   char *fname ;
   FILE                *fp;
   int                 ch ;
   int                 data, data0 ;
   int                 started, file_scan_stage, char_valid, data_cnt, line_cnt, byte_cnt ;
   uint16              data_crc, data_ver ;
   fname = "tscmod_ucode.bin" ;
   data = 0 ;  data0 = 0 ; char_valid = 0 ; data_crc = 0 ; data_ver = 0 ;
   started = 0 ;  file_scan_stage = 0 ; data_cnt = 0 ; line_cnt =0 ; byte_cnt=0 ;
   if((fp = sal_fopen(fname, "r")) == NULL) {
      printf("file not exist %s\n", fname) ;
      return SOC_E_ERROR ;
   }
   /* stop uC */


   /* start to read bytes */
   while ((ch = sal_fgetc(fp)) != EOF) {
      if(ch == '\n' ) line_cnt ++ ;
      if(started) {
         if( ch== '\n' || ch== ' ' || ch == ',' || ch == '}' ) {
            if(char_valid) {
               data_cnt ++ ;
               if(debug)
                  printf("byte=0x%x  cnt=%0d  data_cnt=%0d scan_stage=%0d\n", data, char_valid, data_cnt, file_scan_stage) ;
               byte_cnt += (char_valid + 1)/2 ;
            }
            if( ch == '}' ) started = 0 ;
            if(char_valid) {
               if(file_scan_stage==1) {
                  if(data_cnt==1) data_ver = data ;
                  if(data_cnt==2) data_crc = data ;
               } else if(file_scan_stage==2) {
                  if(byte_cnt==1) data0 = data ;
                  if(byte_cnt==2) {
                     /* do 2 bytes write */

                     byte_cnt = 0 ;
                  }
                  if(started==0) {  /* last single byte is not written */

                  }
               }
            }
            char_valid = 0 ; data = 0 ;
         } else {
            char_valid  ++ ;
            if(ch>='a' && ch <='f') {
               ch = ch-'a' + 10 ;
            } else if(ch>='A'&& ch <= 'F' ) {
               ch = ch-'A' + 10 ;
            } else if(ch>='0' && ch <= '9') {
               ch = ch-'0' ;
            } else {
               printf("Error: bad char (%c) at %s, line=%0d\n", ch, fname, line_cnt) ;
               char_valid = 0 ;
            }
            data = (data <<4) | ch ;
         }
      } else {
         if(ch == '{') {
            started = 1 ;  file_scan_stage++ ;
            if(file_scan_stage==2) data_cnt = 0 ;
            data = 0 ; char_valid = 0 ;
         }
      }
   }
   sal_fclose(fp) ;

   /* restart uC */

   /* check crc */

   if( debug || ((crc !=0) && (crc != data_crc)) ) {
      printf("crc=%0x ver=%0x cmd crc=%x data0=%x\n", data_crc, data_ver, crc, data0) ;
   }

#endif

   return SOC_E_NONE ;
}


/*
 * Function:
 *      phy_tscmod_diag_ctrl
 * Purpose:
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 */

STATIC int
phy_tscmod_diag_ctrl(
   int unit, /* unit */
   soc_port_t port, /* port */
   uint32 inst,     /* the specific device block the control action directs to */
   int op_type,     /* operation types: read,write or command sequence */
   int op_cmd,      /* command code */
   void *arg)       /* command argument based on op_type/op_cmd */
{
   phy_ctrl_t         *pc;
   tscmod_st          *tsc;      /*tscmod structure */
   TSCMOD_DEV_DESC_t  *pDesc;
   TSCMOD_DEV_CFG_t      *pCfg;
   int rv, func, lane, crc, uc_debug;
   int * pData = (int *)arg;

   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   = (tscmod_st *)( pDesc + 1);
   pCfg  = &pDesc->cfg;

   if(tsc->verbosity & TSCMOD_DBG_FLOW)
      printf("%-22s: u=%0d p=%0d inst=%0d type=%0d op_cmd=%0x called\n",
             FUNCTION_NAME(), tsc->unit, tsc->port, inst, op_type, op_cmd) ;

   rv = SOC_E_NONE ;

   switch(op_cmd) {
#if 0
   case PHY_DIAG_CTRL_EYE_MARGIN_LIVE_LINK:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_2D_LIVELINK_EYESCAN;
      tsc->arg = arg;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;
#endif
   case PHY_DIAG_CTRL_EYE_MARGIN_PRBS:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->arg = arg;
      tsc->per_lane_control = TSC_UTIL_2D_PRBS_EYESCAN;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;
   case PHY_DIAG_CTRL_EYE_ENABLE_LIVELINK:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_ENABLE_LIVELINK;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;
   case PHY_DIAG_CTRL_EYE_DISABLE_LIVELINK:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_DISABLE_LIVELINK;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;
   case PHY_DIAG_CTRL_EYE_ENABLE_DEADLINK:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_ENABLE_DEADLINK;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;
   case PHY_DIAG_CTRL_EYE_DISABLE_DEADLINK:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_DISABLE_DEADLINK;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;
   case PHY_DIAG_CTRL_EYE_SET_VOFFSET:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->accData =  *((int *)arg);
      tsc->per_lane_control = TSC_UTIL_SET_VOFFSET;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;
   case PHY_DIAG_CTRL_EYE_SET_HOFFSET:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->accData =  *((int *)arg);
      tsc->per_lane_control = TSC_UTIL_SET_HOFFSET;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;
   case PHY_DIAG_CTRL_EYE_GET_MAX_VOFFSET:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_GET_MAX_VOFFSET;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      *((int *)arg) = tsc->accData;
      break;
   case PHY_DIAG_CTRL_EYE_GET_MIN_VOFFSET:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_GET_MIN_VOFFSET;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      *((int *)arg) = tsc->accData;
      break;
   case PHY_DIAG_CTRL_EYE_GET_INIT_VOFFSET:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_GET_INIT_VOFFSET;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      *((int *)arg) = tsc->accData;
      break;
   case PHY_DIAG_CTRL_EYE_GET_MAX_LEFT_HOFFSET:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_GET_MAX_LEFT_HOFFSET;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      *((int *)arg) = tsc->accData;
      break;
   case PHY_DIAG_CTRL_EYE_GET_MAX_RIGHT_HOFFSET:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_GET_MAX_RIGHT_HOFFSET;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      *((int *)arg) = tsc->accData;
      break;
   case PHY_DIAG_CTRL_EYE_START_LIVELINK:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_START_LIVELINK;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;
   case PHY_DIAG_CTRL_EYE_START_DEADLINK:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_START_DEADLINK;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;
   case PHY_DIAG_CTRL_EYE_STOP_LIVELINK:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_STOP_LIVELINK;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;
   case PHY_DIAG_CTRL_EYE_CLEAR_LIVELINK:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_CLEAR_LIVELINK;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;
   case PHY_DIAG_CTRL_EYE_READ_LIVELINK:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_READ_LIVELINK;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      *((int *)arg) = tsc->accData;
      break;
   case PHY_DIAG_CTRL_EYE_READ_DEADLINK:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_READ_DEADLINK;
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      *((int *)arg) = tsc->accData;
      break;
   case PHY_DIAG_CTRL_EYE_MARGIN_VEYE:
   case PHY_DIAG_CTRL_EYE_MARGIN_VEYE_UP:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_VEYE_U ;
      tsc->live_link = *pData++;
      tsc->ber       = *pData++;
      tsc->tol       = *pData++;
      tsc->eye_lane  = *pData++;
      tsc->time_upper_bound = *pData ;
      if(tsc->verbosity &TSCMOD_DBG_DSC) printf("veye_up\n");
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;

   case PHY_DIAG_CTRL_EYE_MARGIN_VEYE_DOWN:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_VEYE_D ;
      tsc->live_link = *pData++;
      tsc->ber       = *pData++;
      tsc->tol       = *pData++;
      tsc->eye_lane  = *pData++;
      tsc->time_upper_bound = *pData ;
      if(tsc->verbosity &TSCMOD_DBG_DSC) printf("veye_down\n");
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;

   case PHY_DIAG_CTRL_EYE_MARGIN_HEYE_RIGHT:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_HEYE_R ;
      tsc->live_link = *pData++;
      tsc->ber       = *pData++;
      tsc->tol       = *pData++;
      tsc->eye_lane  = *pData++;
      tsc->time_upper_bound = *pData ;
      if(tsc->verbosity &TSCMOD_DBG_DSC) printf("heye_right\n");
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;

   case PHY_DIAG_CTRL_EYE_MARGIN_HEYE_LEFT:
      tsc->diag_type = TSCMOD_DIAG_EYE;
      tsc->per_lane_control = TSC_UTIL_HEYE_L ;
      tsc->live_link = *pData++;
      tsc->ber       = *pData++;
      tsc->tol       = *pData++;
      tsc->eye_lane  = *pData++;
      tsc->time_upper_bound = *pData ;
      if(tsc->verbosity &TSCMOD_DBG_DSC) printf("heye_left\n");
      (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
      break;

   case PHY_DIAG_CTRL_DSC:
      LOG_INFO(BSL_LS_SOC_PHY,
               (BSL_META_U(unit,
                           "phy_tscmod_diag_ctrl: "
                           "u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"),
                unit, port, PHY_DIAG_CTRL_DSC));
      tscmod_uc_status_dump(unit, port);
      break;

   case PHY_DIAG_CTRL_PEEK:
      func = *pData++; /* TSCMOD_DIAG_G_*  */
      lane = *pData++; /* lane 0, 1, 2, 3, or 4(all) */

      if(pCfg->diag_regs==0) return rv ;

      if(tsc->verbosity & TSCMOD_DBG_FLOW)
         printf("%-22s: u=%0d p=%0d diag_g_ func=%0d lane=%0d\n",
                FUNCTION_NAME(), tsc->unit, tsc->port, func, lane) ;

      if(func == TSCMOD_DIAG_G_CFG ) {
         _phy_tscmod_cfg_dump(pc->unit, pc->port) ;
      }
      tsc->per_lane_control = (func <<4) | (lane & 0xf) ;

      if((func == TSCMOD_DIAG_G_MISC1)||(func == TSCMOD_DIAG_G_MISC2))
         tsc->accData = lane ;

      tsc->diag_type        = TSCMOD_DIAG_GENERAL ;
      tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv);

      break ;

   case PHY_DIAG_CTRL_POKE:
      func = *pData++; /* TSCMOD_DIAG_G_*  */
      lane = *pData++; /* lane 0, 1, 2, 3, or 4(all) */

      if(pCfg->diag_regs==0) return rv ;

      if(tsc->verbosity & TSCMOD_DBG_FLOW)
         printf("%-22s: u=%0d p=%0d diag_g_ func=%0d lane=%0d\n",
                FUNCTION_NAME(), tsc->unit, tsc->port, func, lane) ;
      rv = tscmod_diag_poke(tsc,func,lane) ;
      break ;

   case PHY_DIAG_CTRL_LOAD_UC:
      crc      = *pData++;
      uc_debug = *pData++;
      rv = tscmod_diag_load_uC(tsc, crc, uc_debug) ;
      break ;
   default:
      if(tsc->verbosity & TSCMOD_DBG_FLOW)
        printf("%-22s: u=%0d p=%0d inst=%0d type=%0d op_cmd=%0d not defined\n",
                FUNCTION_NAME(), tsc->unit, tsc->port, inst, op_type, op_cmd) ;
      if (op_type == PHY_DIAG_CTRL_SET) {
         rv = _phy_tscmod_control_set(unit,port,op_cmd,PTR_TO_INT(arg));
      } else if (op_type == PHY_DIAG_CTRL_GET) {
         rv = _phy_tscmod_control_get(unit,port,op_cmd,(uint32 *)arg);
      }
      break ;
   }
   tscmod_sema_unlock(unit, port) ;
   return rv;
}


/*
 * Function:
 *      phy_tscmod_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - New setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_tscmod_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
   int rv ;
   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
   rv = _phy_tscmod_control_set(unit, port, type, value) ;
   tscmod_sema_unlock(unit, port) ;
   return rv ;
}

STATIC int
_phy_tscmod_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    phy_ctrl_t            *pc;
    tscmod_st             *tsc;      /*tscmod structure */
    TSCMOD_DEV_DESC_t     *pDesc;
    TSCMOD_DEV_CFG_t      *pCfg;
    int speed, data16 ; int scr, asp_mode; int interface;
    int an, an_done ;

    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	 tsc   =  (tscmod_st *)( pDesc + 1);
    pCfg  = &pDesc->cfg;
    /*copy the phy control pataremeter into tscmod  */

    tsc->this_lane = pc->lane_num;

    /* preventive care */
    if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR) {
       tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY_BLK_ADR ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
    }


    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /*
        *For the (forced speed) FS mode, whatever you want to write, always OR the value with 0x8000 (which means to force the preemphasis value you want to write).
        *Then when you read it back, 0x8000 will be dropped and you can get value which matches what you set before.
        *If you do not OR the value with 0x8000,  then an automatic preemphasis values will kick in by uC. Then you will get a different value with what you set.
        */
        rv = _phy_tscmod_control_preemphasis_set(unit, pc, type, value);
        break;

    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:     /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:     /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:     /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:     /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0: /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1: /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2: /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3: /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:           /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:       /* fall through */
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:     /* fall through */
        rv = _phy_tscmod_control_tx_driver_set(unit, pc, type, value);
      break;
    case SOC_PHY_CONTROL_DUMP:
        rv = tscmod_uc_status_dump(unit, port);
      break;
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = _phy_tscmod_control_tx_lane_squelch_set(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
       rv = _phy_tscmod_rx_pf_ovr_control_set(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_VGA:
       rv = _phy_tscmod_rx_vga_ovr_control_set(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP1:
       rv = _phy_tscmod_rx_DFE_tap1_ovr_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP2:
       rv = _phy_tscmod_rx_DFE_tap2_ovr_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP3:
       rv = _phy_tscmod_rx_DFE_tap3_ovr_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP4:
       rv = _phy_tscmod_rx_DFE_tap4_ovr_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP5:
       rv = _phy_tscmod_rx_DFE_tap5_ovr_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_PLUS1_SLICER:
       rv = _phy_tscmod_rx_p1_slicer_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_MINUS1_SLICER:
       rv = _phy_tscmod_rx_m1_slicer_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_D_SLICER:
       rv = _phy_tscmod_rx_d_slicer_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_SIGNAL_DETECT:
      rv = _phy_tscmod_control_rx_sigdet_set(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_PHASE_INTERP:
       rv = _phy_tscmod_pi_control_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_POLARITY:
       rv = _phy_tscmod_rx_polarity_set (unit, pc, value);
       pCfg->rxpol = value ;
       break;
    case SOC_PHY_CONTROL_TX_POLARITY:
       rv = _phy_tscmod_tx_polarity_set (unit, pc, value);
       pCfg->txpol = value ;
       break;
    case SOC_PHY_CONTROL_TX_RESET:
       rv = _phy_tscmod_tx_reset (unit, pc, value);
       break;
    case SOC_PHY_CONTROL_RX_RESET:
       rv = _phy_tscmod_rx_reset (unit, pc, value);
       break;
    case SOC_PHY_CONTROL_CL72:
       rv = _phy_tscmod_cl72_enable_set (unit, pc, value);
       break;
    /*
    case SOC_PHY_CONTROL_LANE_SWAP:
    rv = _phy_tscmod_control_lane_swap_set (unit, pc, value);
    break;
    */
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
       rv = _phy_tscmod_control_firmware_mode_set (unit, pc, value);
       break;
    case SOC_PHY_CONTROL_AUTONEG_MODE:
        rv = SOC_E_NONE;
        SOC_IF_ERROR_RETURN(
                phy_tscmod_an_get(unit, port, &an, &an_done));
        if(an) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "phy_wc40_control_set: PHY_CONTROL_AUTONEG_MODE "
                                 "-> disable autoneg u=%d p=%d\n"), unit, port));
            return SOC_E_FAIL;
        }
        switch(value) {
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL37:
           DEV_CFG_PTR(pc)->cl37an = TSCMOD_CL37_WO_BAM ;
           DEV_CFG_PTR(pc)->cl73an = TSCMOD_CL73_DISABLE;
           break;
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM:
           DEV_CFG_PTR(pc)->cl37an = TSCMOD_CL37_W_BAM ;
           DEV_CFG_PTR(pc)->cl73an = TSCMOD_CL73_DISABLE;
           break;
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL73:
           DEV_CFG_PTR(pc)->cl73an = TSCMOD_CL73_WO_BAM ;
           DEV_CFG_PTR(pc)->cl37an = TSCMOD_CL37_DISABLE;
           break;
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL73_BAM:
           DEV_CFG_PTR(pc)->cl73an = TSCMOD_CL73_W_BAM ;
           DEV_CFG_PTR(pc)->cl37an = TSCMOD_CL37_DISABLE;
           break;
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL37_CL73:
           DEV_CFG_PTR(pc)->cl73an = TSCMOD_CL73_CL37 ;
           break ;
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL37_CL73_BAM:
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM_CL73:
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM_CL73_BAM:
        default:
           rv = SOC_E_PARAM;
           break;
        }
        break;
    case SOC_PHY_CONTROL_HG2_BCM_CODEC_ENABLE:
        rv = _phy_tscmod_higig2_codec_control_set(unit, pc, value);
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_20BIT:
       rv = _phy_tscmod_control_tx_pattern_20bit_set (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_TX_PATTERN_256BIT:
       rv = _phy_tscmod_control_tx_pattern_256bit_set (unit, pc, value);
       break ;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA0:
        pCfg->pattern_data[0] = value;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA1:
        pCfg->pattern_data[1] = value;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA2:
        pCfg->pattern_data[2] = value;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA3:
        pCfg->pattern_data[3] = value;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA4:
        pCfg->pattern_data[4] = value;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA5:
        pCfg->pattern_data[5] = value;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA6:
        pCfg->pattern_data[6] = value;
        break;
    case SOC_PHY_CONTROL_TX_PATTERN_DATA7:
        pCfg->pattern_data[7] = value;
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = _phy_tscmod_control_prbs_polynomial_set(tsc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = _phy_tscmod_control_prbs_tx_invert_data_set(tsc, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        rv = _phy_tscmod_control_prbs_rx_invert_data_set(tsc, value); 
       break;
    case SOC_PHY_CONTROL_PRBS_CHECK_MODE:
       rv = _phy_tscmod_control_prbs_check_mode_set(tsc, value);
       break;        
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
       /* TX_ENABLE does both tx and rx */
       rv = phy_tscmod_control_prbs_enable_set(tsc, value);
       break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
       /* RX_ENABLE does both tx and rx */
       /* rv = phy_tscmod_control_prbs_enable_set(tsc, value); */
       rv = SOC_E_NONE;
       break;
    case SOC_PHY_CONTROL_8B10B:
        rv = _phy_tscmod_control_8b10b_set(unit, pc, value);
        break;
    case SOC_PHY_CONTROL_EEE:
        rv = _phy_tscmod_control_eee_set(unit,pc,value);  /* EEE pass thru */
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
       rv = _phy_tscmod_rloop_set(unit,pc, 1, value);   /* PCS rloop */
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
       rv = _phy_tscmod_rloop_set(unit,pc, 0, value);   /* PMD rloop */
       break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        rv = _phy_tscmod_fec_set(unit,pc,value);
        break;
    /* XXX obsolete */
    case SOC_PHY_CONTROL_CUSTOM1:
       DEV_CFG_PTR(pc)->custom1 = value? TRUE: FALSE ;
       rv = SOC_E_NONE;
       break;
    case SOC_PHY_CONTROL_SCRAMBLER:
        DEV_CFG_PTR(pc)->scrambler_en = value? TRUE: FALSE;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_tscmod_control_linkdown_transmit_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        if (value) {
            value = TSCMOD_PDET_CONTROL_MASK;
            pCfg->pdetect1000x = 1;
        } else {
            pCfg->pdetect1000x = 0;
        }
        value |= (TSCMOD_PDET_CONTROL_1G | TSCMOD_PDET_CONTROL_CMD_MASK);
        rv = _phy_tscmod_control_pdet_set(unit, pc, value);/* For 1G*/
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION_10G:
        if (value) {
            value = TSCMOD_PDET_CONTROL_MASK;
            pCfg->pdetect10g = 1;
        } else {
            pCfg->pdetect10g = 0;
        }
        value |= (TSCMOD_PDET_CONTROL_10G | TSCMOD_PDET_CONTROL_CMD_MASK);
        rv = _phy_tscmod_control_pdet_set(unit, pc, value);/* For 10G*/
        break;
    case SOC_PHY_CONTROL_BERT_PATTERN:     /* fall through */
    case SOC_PHY_CONTROL_BERT_RUN:         /* fall through */
    case SOC_PHY_CONTROL_BERT_PACKET_SIZE: /* fall through */
    case SOC_PHY_CONTROL_BERT_IPG:
        rv = _phy_tscmod_control_bert_set(unit,pc,type,value);
        break;
    case SOC_PHY_CONTROL_TX_PPM_ADJUST:
       /* value = offset
        * ----------------------------------
        * Reference Values:
        * PPM   Offset  Setting
        * -5    0xff99
        * -10   0xff31
        * -15   0xfec9
        * -20   0xfe61
        * -25   0xfdf9
        * -30   0xfd91
        * -35   0xfd29
        * -40   0xfcc1
        * -45   0xfc59
        * -50   0xfbf1
        * -55   0xfb89
        * -60   0xfb21
        * -65   0xfab9
        * -70   0xfa51
        *
        * 5     0x69
        * 10    0xd1
        * 15    0x139
        * 20    0x1a1
        * 25    0x209
        * 30    0x271
        * 35    0x2d0
        * 40    0x341
        * 45    0x3a9
        * 50    0x411
        * 55    0x479
        * 60    0x4e1
        * 65    0x549
        * 70    0x5b1
        * ----------------------------------
       */
       if(value&1) {
          tsc->per_lane_control = TSCMOD_TX_PI_PPM_SET | (value <<16) ;
       } else {
          tsc->per_lane_control = TSCMOD_TX_PI_PPM_RESET ;
       }
       tscmod_tier1_selector("TX_PI_CONTROL", tsc, &rv) ;
       break ;
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
        DEV_CFG_PTR(pc)->sw_rx_los.enable      = value ;
        DEV_CFG_PTR(pc)->sw_rx_los.sys_link    = 0;
        DEV_CFG_PTR(pc)->sw_rx_los.state       = RXLOS_RESET;
        DEV_CFG_PTR(pc)->sw_rx_los.link_status = 0;
        DEV_CFG_PTR(pc)->sw_rx_los.count       = 0;
        rv = SOC_E_NONE;
        rv |= tscmod_wb_sc_handler(unit, tsc->port, TSCMOD_WB_SC_UPDATE, TSCMOD_WB_RXLOS) ;
        /* THE FLAG IS SET IN INIT. FOR TSCMOD DRIVER
           TO WORK IT'S LINK_GET MUST BE CALLED */
        /* Manage the _SERVICE_INT_PHY_LINK_GET flag so that
           if external phy is present link_get for
           internal phy is still called to process
           software rx los feature
        if(value) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_SERVICE_INT_PHY_LINK_GET);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_SERVICE_INT_PHY_LINK_GET);
        }
        */
        break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER_TEMP_COMP:
       rv = _phy_tscmod_speed_get(unit, port, &speed, &interface, &asp_mode, &scr) ;
       /* interface may contain one significant interface and xgmii */
       if((speed == 40000)&&((interface ==SOC_PORT_IF_SR4)||(interface==SOC_PORT_IF_SR))) {
          if(value) {
             data16 = TSCMOD_UC_CTRL_OPT_PF_TEMP_COMP;
          } else {
             data16 = TSCMOD_UC_CTRL_SR4; /* default for SR4 */
          }
          _phy_tscmod_control_firmware_mode_set(unit, pc, data16) ;

       } else if((speed == 10000) && ( (interface==SOC_PORT_IF_SFI)
                                     ||(interface==SOC_PORT_IF_SR))) {
          if(value) {
             data16 = TSCMOD_UC_CTRL_OPT_PF_TEMP_COMP;
          } else {
             data16 = 0 ; /* default */
          }
          _phy_tscmod_control_firmware_mode_set(unit, pc, data16) ;
       }
       rv |= SOC_E_NONE;
       break ;
    case SOC_PHY_CONTROL_PCS_MODE:
        pCfg->pcs_mode = value ;
        rv = SOC_E_NONE;
        break ;
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS_LINK_WAIT_TIMER_US:
        pCfg->rxlos_link_wait_timer = value ;
        rv = SOC_E_NONE;
        break ;
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS_RESTART_TIMER_US:
        pCfg->rxlos_restart_timer = value ;
        rv = SOC_E_NONE;
        break ;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}

/*
 * Function:
 *      phy_tscmod_control_get
 * Purpose:
 *      Get current control settign of the PHY.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - (OUT)Current setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_tscmod_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
   int rv ;
   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
   rv = _phy_tscmod_control_get(unit, port, type, value) ;
   tscmod_sema_unlock(unit, port) ;
   return rv ;
}

STATIC int
_phy_tscmod_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    int intf;
    int scr, asp_mode;
    int speed;
    int fw_mode ;
    phy_ctrl_t *pc;
    TSCMOD_DEV_DESC_t           *pDesc;
    tscmod_st   *tsc;      /*tscmod structure */
    int cl37, cl73;

    if (NULL == value) {
      return SOC_E_PARAM;
    }
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
      return SOC_E_PARAM;
    }

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);

    if(tsc->verbosity & TSCMOD_DBG_LINK) {
       printf("%s u=%0d p=%0d type=%0d sel=%x l=%0d\n",
              FUNCTION_NAME(), tsc->unit, tsc->port, type, tsc->lane_select, tsc->this_lane) ;
    }
    tsc->this_lane = pc->lane_num;
    rv = SOC_E_UNAVAIL;

   /* preventive care */
    if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR) {
       tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) | TSCMOD_SRESET_AER_ALL_PROXY_BLK_ADR ;
       tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
    }


    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3: /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS:
      rv = _phy_tscmod_control_preemphasis_get(unit, pc, type, value);
      break;

    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:     /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:     /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:     /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:     /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0: /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1: /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2: /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3: /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:           /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:       /* fall through */
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:
      rv = _phy_tscmod_control_tx_driver_get(unit, pc, type, value);
      break;

    case SOC_PHY_CONTROL_RX_PEAK_FILTER:   /* fall through */
    case SOC_PHY_CONTROL_RX_VGA:           /* fall through */
    case SOC_PHY_CONTROL_RX_TAP1:          /* fall through */
    case SOC_PHY_CONTROL_RX_TAP2:          /* fall through */
    case SOC_PHY_CONTROL_RX_TAP3:          /* fall through */
    case SOC_PHY_CONTROL_RX_TAP4:          /* fall through */
    case SOC_PHY_CONTROL_RX_TAP5:          /* fall through */
      rv = _phy_tscmod_control_diag_rx_taps(unit, pc, type, value);
      break;
    case SOC_PHY_CONTROL_RX_PLUS1_SLICER:  /* fall through */
    case SOC_PHY_CONTROL_RX_MINUS1_SLICER: /* fall through */
    case SOC_PHY_CONTROL_RX_D_SLICER:      /* fall through */
      rv = _phy_tscmod_control_diag_slicers(unit, pc, type, value);
      break;
    case SOC_PHY_CONTROL_RX_SIGNAL_DETECT:
      rv = _phy_tscmod_control_rx_sigdet_get(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_SEQ_DONE:
      rv = _phy_tscmod_control_rx_seq_done_get(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_RX_PPM:
      rv = _phy_tscmod_control_rx_ppm_get(unit, pc, type, value);
      break;
    case SOC_PHY_CONTROL_CL72:
      rv = _phy_tscmod_cl72_enable_get(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_CL72_STATUS:
      rv = _phy_tscmod_cl72_status_get(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
      rv = _phy_tscmod_control_prbs_polynomial_get(tsc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
      rv = _phy_tscmod_control_prbs_tx_invert_data_get(tsc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
       rv = _phy_tscmod_control_prbs_rx_invert_data_get(tsc, value);
       break;
    case SOC_PHY_CONTROL_PRBS_CHECK_MODE:
       rv = _phy_tscmod_control_prbs_check_mode_get(tsc, value);
       break;    
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE: /* fall through */
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE: /* fall through */
      rv = _phy_tscmod_control_prbs_enable_get(tsc, value);
      break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
      rv = _phy_tscmod_control_prbs_rx_status_get(tsc, value);
      break;
    case SOC_PHY_CONTROL_EEE:
      rv = _phy_tscmod_control_eee_get(unit,pc,value); 
      break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
       rv = _phy_tscmod_rloop_get(unit,pc, 0, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
       rv = _phy_tscmod_rloop_get(unit,pc, 1, value);
       break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
      rv = _phy_tscmod_fec_get(unit,pc,value);
      break;
    case SOC_PHY_CONTROL_TX_PATTERN_256BIT:
       rv = _phy_tscmod_control_tx_pattern_256bit_get (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_TX_PATTERN_20BIT:
       rv = _phy_tscmod_control_tx_pattern_20bit_get (unit, pc, value);
      break;
    case SOC_PHY_CONTROL_CUSTOM1:
      *value = DEV_CFG_PTR(pc)->custom1;
      rv = SOC_E_NONE;
      break;
    case SOC_PHY_CONTROL_VCO_FREQ:
      rv = _phy_tscmod_control_vco_freq_get(unit, pc, value);
      rv = SOC_E_NONE;
      break;
    case SOC_PHY_CONTROL_SCRAMBLER:
       rv = _phy_tscmod_speed_get(unit, port, &speed,&intf, &asp_mode, &scr);
       *value = scr;
       break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
      rv = _phy_tscmod_control_linkdown_transmit_get(unit, pc, value);
      break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        *value = TSCMOD_PDET_CONTROL_1G;
        rv = _phy_tscmod_control_pdet_get(unit, pc, value);
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION_10G:
        *value = TSCMOD_PDET_CONTROL_10G;
        rv = _phy_tscmod_control_pdet_get(unit, pc, value);
        break;
    case SOC_PHY_CONTROL_BERT_TX_PACKETS:       /* fall through */
    case SOC_PHY_CONTROL_BERT_RX_PACKETS:       /* fall through */
    case SOC_PHY_CONTROL_BERT_RX_ERROR_BITS:    /* fall through */
    case SOC_PHY_CONTROL_BERT_RX_ERROR_BYTES:   /* fall through */
    case SOC_PHY_CONTROL_BERT_RX_ERROR_PACKETS: /* fall through */
    case SOC_PHY_CONTROL_BERT_PATTERN:          /* fall through */
    case SOC_PHY_CONTROL_BERT_PACKET_SIZE:      /* fall through */
    case SOC_PHY_CONTROL_BERT_IPG:              /* fall through */
        rv = _phy_tscmod_control_bert_get(unit,pc,type,value);
        break;
    case SOC_PHY_CONTROL_TX_PPM_ADJUST:
       tsc->per_lane_control = TSCMOD_TX_PI_PPM_RD ;
       tscmod_tier1_selector("TX_PI_CONTROL", tsc, &rv) ;
       *value = tsc->accData ;
       break ;
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
       rv = _phy_tscmod_control_firmware_mode_get (unit, pc, value);
       break;
    case SOC_PHY_CONTROL_AUTONEG_MODE:
        rv = SOC_E_NONE;
        cl37 = 0;
        cl73 = 0;
        switch(DEV_CFG_PTR(pc)->cl37an) {
        case TSCMOD_CL37_WO_BAM:
           cl37 = SOC_PHY_CONTROL_AUTONEG_MODE_CL37;
           break;
        case TSCMOD_CL37_W_BAM:
           cl37 = SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM;
           break;
        default:
           break;
        }
        switch(DEV_CFG_PTR(pc)->cl73an) {
        case TSCMOD_CL73_WO_BAM:
           cl73 = SOC_PHY_CONTROL_AUTONEG_MODE_CL73;
           break;
        case TSCMOD_CL73_W_BAM:
        case TSCMOD_CL73_HPAM_VS_SW:
        case TSCMOD_CL73_HPAM:
        case TSCMOD_CL73_HPAM_AUTOCL73:
        case TSCMOD_CL73_HPAM_AUTOCL73_CL37:
        case TSCMOD_CL73_HPAM_CL37:
           cl73 = SOC_PHY_CONTROL_AUTONEG_MODE_CL73_BAM;
           break;
        case TSCMOD_CL73_CL37:
           cl73 = SOC_PHY_CONTROL_AUTONEG_MODE_CL37_CL73_BAM ;
        default:
           break;
        }
        *value = cl73 ?  cl73 : cl37;
        break;
    case SOC_PHY_CONTROL_HG2_BCM_CODEC_ENABLE:
        rv = _phy_tscmod_higig2_codec_control_get(unit, pc, value);
        break;
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
       *value = DEV_CFG_PTR(pc)->sw_rx_los.enable ;
       rv = SOC_E_NONE ;
       break ;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER_TEMP_COMP:
        tsc->per_lane_control = TSCMOD_FWMODE_RD ;
        tscmod_tier1_selector("FWMODE_CONTROL", tsc, &rv);
        fw_mode = tsc->accData ;
        *value = (fw_mode == TSCMOD_UC_CTRL_OPT_PF_TEMP_COMP);
        break ;
    case SOC_PHY_CONTROL_PCS_MODE:
        *value = DEV_CFG_PTR(pc)->pcs_mode ;
        rv = SOC_E_NONE ;
	    break ; 
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS_LINK_WAIT_TIMER_US:
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS_RESTART_TIMER_US:
        rv = _phy_tscmod_rxlos_timer_get(unit, pc, type, value);
        break ;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}

/*
 * Function:
 *      phy_tscmod_per_lane_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      lane  - lane number
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_tscmod_per_lane_control_set(int unit, soc_port_t port, int lane,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* TX_ENABLE does both tx and rx */
        rv = _phy_tscmod_per_lane_control_preemphasis_set(unit, port, lane, SOC_PHY_CONTROL_PREEMPHASIS, value, TSCMOD_TAP_REG);
        break; 
    case SOC_PHY_CONTROL_DRIVER_CURRENT:           /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:       /* fall through */
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:     /* fall through */
        rv = _phy_tscmod_per_lane_control_tx_driver_set(unit, port, lane, type, value);
      break;
    /*
    case SOC_PHY_CONTROL_RX_TAP1:
       rv = _phy_tscmod_per_lane_rx_DFE_tap1_ovr_control_set (unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP1_RELEASE:
       rv = _phy_tscmod_per_lane_rx_DFE_tap1_ovr_control_set (unit, port, lane, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_TAP2:
       rv = _phy_tscmod_per_lane_rx_DFE_tap2_ovr_control_set (unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP2_RELEASE:
       rv = _phy_tscmod_per_lane_rx_DFE_tap2_ovr_control_set (unit, port, lane, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_TAP3:
       rv = _phy_tscmod_per_lane_rx_DFE_tap3_ovr_control_set (unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP3_RELEASE:
       rv = _phy_tscmod_per_lane_rx_DFE_tap3_ovr_control_set (unit, port, lane, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_TAP4:
       rv = _phy_tscmod_per_lane_rx_DFE_tap4_ovr_control_set (unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP4_RELEASE:
       rv = _phy_tscmod_per_lane_rx_DFE_tap4_ovr_control_set (unit, port, lane, 0x8000);
      break;
    case SOC_PHY_CONTROL_RX_TAP5:
       rv = _phy_tscmod_per_lane_rx_DFE_tap5_ovr_control_set (unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_TAP5_RELEASE:
       rv = _phy_tscmod_per_lane_rx_DFE_tap5_ovr_control_set (unit, port, lane, 0x8000);
       break;
    */
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
       rv = phy_tscmod_per_lane_control_prbs_polynomial_set(unit, port, lane, value);
       break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
       rv = phy_tscmod_per_lane_control_prbs_tx_invert_data_set(unit, port, lane, value);
       break;
    case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
       rv = phy_tscmod_per_lane_control_prbs_rx_invert_data_set(unit, port, lane, value);
       break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
       /* TX_ENABLE does both tx and rx */
       rv = phy_tscmod_per_lane_control_prbs_enable_set(unit, port, lane, value);
       break; 
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
       rv = SOC_E_NONE;
       break;
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
       rv = phy_tscmod_per_lane_control_firmware_mode_set(unit, port, lane, value);
       break;
    /*
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
       rv = _phy_tscmod_per_lane_rx_pf_ovr_control_set(unit, port,lane,  value);
      break;
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
       rv = _phy_tscmod_per_lane_rx_low_freq_pf_ovr_control_set(unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_VGA:
       rv = _phy_tscmod_per_lane_rx_vga_ovr_control_set(unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_RX_VGA_RELEASE:
       rv = _phy_tscmod_per_lane_rx_vga_ovr_control_set(unit, port, lane,  0x8000);
      break;
    case SOC_PHY_CONTROL_RX_PLUS1_SLICER:
       rv = _phy_tscmod_per_lane_rx_p1_slicer_control_set(unit, port, lane, value);
       break; 
    case SOC_PHY_CONTROL_RX_MINUS1_SLICER:
       rv = _phy_tscmod_per_lane_rx_m1_slicer_control_set(unit, port, lane, value);
       break; 
    case SOC_PHY_CONTROL_RX_D_SLICER:
       rv = _phy_tscmod_per_lane_rx_d_slicer_control_set(unit, port, lane, value);
      break;
    */
    default:
       rv = SOC_E_UNAVAIL;
       break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_tscmod_per_lane_control_get
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      lane  - lane number
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_tscmod_per_lane_control_get(int unit, soc_port_t port, int lane,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* TX_ENABLE does both tx and rx */
        rv = phy_tscmod_per_lane_control_preemphasis_get(unit, port, lane, value);
        break; 
    case SOC_PHY_CONTROL_DRIVER_CURRENT:           /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:       /* fall through */
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:     /* fall through */
        rv = phy_tscmod_per_lane_control_tx_driver_get(unit, port, lane, type, value);
      break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
       rv = phy_tscmod_per_lane_control_prbs_polynomial_get(unit, port, lane, value);
       break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
       rv = phy_tscmod_per_lane_control_prbs_tx_invert_data_get(unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
       rv = phy_tscmod_per_lane_control_prbs_rx_invert_data_get(unit, port, lane, value);
      break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE: /* fall through */
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE: /* fall through */
       rv = phy_tscmod_per_lane_control_prbs_enable_get(unit, port, lane, value);
       break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
       rv = phy_tscmod_per_lane_control_prbs_rx_status_get(unit, port, lane, value);
       break;
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
       rv = phy_tscmod_per_lane_control_firmware_mode_get(unit, port, lane, value);
       break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}



typedef struct {
    uint16 start;
    uint16 end;
} TSCMOD_LANE0_REG_BLOCK;

STATIC TSCMOD_LANE0_REG_BLOCK tscmod_lane0_reg_block[] = {
    {0x8050,0x80FE},  /* register in this block only valid at lane0 */
    {0x8000,0x8001},  /* valid at lane0 */
    {0x8015,0x8019}  /* valid at lane0 */
};

STATIC int
_tscmod_lane0_reg_access(int unit, phy_ctrl_t *pc,uint16 reg_addr)
{
    int ix = 0;
    int num_blk;
    TSCMOD_LANE0_REG_BLOCK * pBlk;

    num_blk = sizeof(tscmod_lane0_reg_block) / sizeof(tscmod_lane0_reg_block[0]);
    for (ix = 0; ix < num_blk; ix++) {
        pBlk = &tscmod_lane0_reg_block[ix];
        if ((reg_addr >= pBlk->start) && (reg_addr <= pBlk->end)) {
            return TRUE;
        }
    }
    return FALSE;
}

STATIC int
phy_tscmod_reg_aer_read(int unit, phy_ctrl_t *pc, uint32 flags,
                  uint32 phy_reg_addr, uint16 *phy_data)
{
    uint16 data;
    uint32 lane;         /* lane to access, override the default lane */
    tscmod_st            *tsc;      /*tscmod structure */
    TSCMOD_DEV_DESC_t    *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

    lane = flags & LANE_ACCESS_MASK;

    /* safety check */
    if ((lane > 4) || (lane == LANE_BCST)) {
        lane = LANE0_ACCESS;
    }

    /* check if register only available on lane0 */
    if (_tscmod_lane0_reg_access(unit,pc,(uint16)(phy_reg_addr & 0xffff))) {
        lane = LANE0_ACCESS;
    }


    if (lane)
    {
        tsc->lane_num_ignore = 1;
    }
    SOC_IF_ERROR_RETURN
        (tscmod_reg_aer_read(unit, tsc, phy_reg_addr, &data));

    *phy_data = data;

    return SOC_E_NONE;
}

STATIC int
phy_tscmod_reg_aer_write(int unit, phy_ctrl_t *pc, uint32 flags,
                   uint32 phy_reg_addr, uint16 phy_data)
{
    uint16       dxgxs,data;
    uint32 lane;
    tscmod_st            *tsc;      /*tscmod structure */
    TSCMOD_DEV_DESC_t    *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

    data      = (uint16) (phy_data & 0x0000FFFF);
    dxgxs = DEV_CFG_PTR(pc)->dxgxs;

    lane = flags & LANE_ACCESS_MASK;

    /* safety check */
    if ((lane > 4) && (lane != LANE_BCST)) {
        lane = LANE0_ACCESS;
    }

    /* check if register only available on lane0 or should be done from lane0 */
    if ((_tscmod_lane0_reg_access(unit,pc,(uint16)(phy_reg_addr & 0xffff))) || dxgxs) {
        lane = LANE0_ACCESS;
    }


    if (lane)
    {
        tsc->lane_num_ignore = 1;
    }
    SOC_IF_ERROR_RETURN
        (tscmod_reg_aer_write(unit, tsc, phy_reg_addr, data));


    return SOC_E_NONE;
}

STATIC int
phy_tscmod_reg_aer_modify(int unit, phy_ctrl_t *pc, uint32 flags,
                    uint32 phy_reg_addr, uint16 phy_data,
                    uint16 phy_data_mask)
{
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;
    uint16               lane;     /*lane number */
    tscmod_st            *tsc;      /*tscmod structure */
    TSCMOD_DEV_DESC_t    *pDesc;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

    data      = (uint16) (phy_data & 0x0000FFFF);
    mask      = (uint16) (phy_data_mask & 0x0000FFFF);

    lane = flags & LANE_ACCESS_MASK;

    /* safety check */
    if ((lane > 4) || (lane == LANE_BCST)) {
        lane = LANE0_ACCESS;
    }

    /* check if register only available on lane0 */
    if (_tscmod_lane0_reg_access(unit,pc,(uint16)(phy_reg_addr & 0xffff))) {
        lane = LANE0_ACCESS;
    }


    if (lane)
    {
        tsc->lane_num_ignore = 1;
    }

    SOC_IF_ERROR_RETURN
        (tscmod_reg_aer_modify(unit, tsc, phy_reg_addr, data, mask));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_tscmod_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_tscmod_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
    pc = INT_PHY_SW_STATE(unit, port);
    data = 0 ;

    SOC_IF_ERROR_RETURN
        (phy_tscmod_reg_aer_read(unit, pc, 0x00, phy_reg_addr, &data));

    *phy_data = data;

    tscmod_sema_unlock(unit, port) ;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_tscmod_reg_write
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - Value write to PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_tscmod_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_tscmod_reg_aer_write(unit, pc, 0x00, phy_reg_addr, data));

    tscmod_sema_unlock(unit, port) ;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_tscmod_reg_modify
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_mo_data  - New value for the bits specified in phy_mo_mask
 *      phy_mo_mask  - Bit mask to modify
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_tscmod_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask)
{
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;
    phy_ctrl_t    *pc;      /* PHY software state */

    tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);
    mask      = (uint16) (phy_data_mask & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_tscmod_reg_aer_modify(unit, pc, 0x00, phy_reg_addr, data, mask));
    tscmod_sema_unlock(unit, port) ;
    return SOC_E_NONE;
}

/* visit I */
STATIC int
phy_tscmod_probe(int unit, phy_ctrl_t *pc)
{
     tscmod_st            tsc;      /* tscmod structure    */
     uint16      serdes_id0;
     int rv;

    sal_memset(&tsc, 0, sizeof(tscmod_st));
    tsc.port = pc->port;
    tsc.unit = pc->unit;
    tsc.phy_ad     = pc->phy_id;
    tsc.regacc_type= TSCMOD_MDIO_CL22;
    tsc.port_type  = TSCMOD_MULTI_PORT;
    tsc.lane_num_ignore = TRUE;
    /*tsc.read  = pc->read; */
    /*tsc.write = pc->write; */


    (tscmod_tier1_selector("REVID_READ", &tsc, &rv));

    serdes_id0 = tsc.accData;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_tscmod_probe: u=%d p=%d\n"), pc->unit, pc->port));

    if(tsc.verbosity & TSCMOD_DBG_INIT)
        LOG_CLI((BSL_META_U(unit,
                            "p=%0d TSCmod ID 0x%04x\n"), pc->port, serdes_id0));

    if((tsc.model_type &TSCMOD_MODEL_TYPE_MASK) == TSCMOD_MODEL_TYPE_ILLEGAL) {
        return SOC_E_NOT_FOUND;
    }

    /* ask to allocate the driver specific descripor  */
    pc->size     = sizeof(TSCMOD_DEV_DESC_t) + sizeof(tscmod_st);
    pc->dev_name = tscmod_device_name;
    return SOC_E_NONE;
}

STATIC int
phy_tscmod_notify(int unit, soc_port_t port,
                       soc_phy_event_t event, uint32 value)
{
    int             rv;
    phy_ctrl_t    *pc;      /* PHY software state */
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    if (CUSTOM_MODE(pc)) {
        return SOC_E_NONE;
    }
    tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
    if(tsc->verbosity & TSCMOD_DBG_LINK) {
       printf("%s p=%0d notify=0x%0x value=%0x\n", FUNCTION_NAME(), tsc->port, event, value) ;
    }
    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        rv = (_phy_tscmod_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_phy_tscmod_notify_duplex(unit, port, value));
        break;
    case phyEventSpeed:
        rv = (_phy_tscmod_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_phy_tscmod_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_phy_tscmod_notify_resume(unit, port, value));
        break;
    case phyEventAutoneg:
        rv = (_phy_tscmod_an_set(unit, port, value));
        break;
    case phyEventTxFifoReset:
        rv = (_phy_tscmod_tx_fifo_reset(unit, port, value));
        break;
    case phyEventTxSquelch:
        rv = (_phy_tscmod_control_set(unit, port, 
                                     SOC_PHY_CONTROL_TX_LANE_SQUELCH, 
                                     value));        
        break;
    case phyEventLpiBypass:
        rv = _phy_tscmod_control_set(unit, port, SOC_PHY_CONTROL_EEE, TSCMOD_LPI_BYPASS_SET(value));
        break ;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }
    tscmod_sema_unlock(unit, port) ;
    return rv;
}



STATIC int
phy_tscmod_linkup_evt (int unit, soc_port_t port)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_tscmod_duplex_set
 * Purpose:
 *      Set the current duplex mode (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_tscmod_duplex_set(int unit, soc_port_t port, int duplex)
{
    return SOC_E_NONE;
}

STATIC int
phy_tscmod_duplex_get(int unit, soc_port_t port, int *duplex)
{
   *duplex = TRUE;
   return SOC_E_NONE;
}


/*
 * Function:
 *      phy_tscmod_medium_config_set
 * Purpose:
 *      set the configured medium the device is operating on.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - not used
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_tscmod_medium_config_set(int unit, soc_port_t port,
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    phy_ctrl_t       *pc;

    pc            = INT_PHY_SW_STATE(unit, port);

    switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        DEV_CFG_PTR(pc)->medium = SOC_PORT_MEDIUM_COPPER;
        break;
    case SOC_PORT_MEDIUM_FIBER:
        DEV_CFG_PTR(pc)->medium = SOC_PORT_MEDIUM_FIBER;
        break;
    default:
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_tscmod_medium_get
 * Purpose:
 *      Indicate the configured medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) One of:
 *              SOC_PORT_MEDIUM_COPPER
 *              SOC_PORT_MEDIUM_FIBER
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_tscmod_medium_get(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    phy_ctrl_t       *pc;

    if (medium == NULL) {
        return SOC_E_PARAM;
    }

    pc            = INT_PHY_SW_STATE(unit, port);
    *medium = DEV_CFG_PTR(pc)->medium;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_tscmod_master_get
 * Purpose:
 *      this function is meant for 1000Base-T PHY. Added here to support
 *      internal PHY regression test
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) SOC_PORT_MS_*
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The master mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_tscmod_master_get(int unit, soc_port_t port, int *master)
{
    *master = SOC_PORT_MS_NONE;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_tscmod_diag_ctrl
 * Purpose:
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 */

/*
 * Variable:
 *      phy_tscmod_drv
 * Purpose:
 *      Phy Driver for 10G (XAUI x 4) Serdes PHY.
 */
phy_driver_t phy_tscmod_hg = {
    /* .drv_name                      = */ "TSC PHY Driver",
    /* .pd_init                       = */ phy_tscmod_init,
    /* .pd_reset                      = */ phy_tscmod_soft_reset,
    /* .pd_link_get                   = */ phy_tscmod_link_get,
    /* .pd_enable_set                 = */ phy_tscmod_enable_set,
    /* .pd_enable_get                 = */ phy_tscmod_enable_get,
    /* .pd_duplex_set                 = */ phy_tscmod_duplex_set,
    /* .pd_duplex_get                 = */ phy_tscmod_duplex_get,
    /* .pd_speed_set                  = */ phy_tscmod_speed_set,
    /* .pd_speed_get                  = */ phy_tscmod_speed_get,
    /* .pd_master_set                 = */ phy_null_set,
    /* .pd_master_get                 = */ phy_tscmod_master_get,
    /* .pd_an_set                     = */ phy_tscmod_an_set,
    /* .pd_an_get                     = */ phy_tscmod_an_get,
    /* .pd_adv_local_set              = */ NULL, /* Deprecated */
    /* .pd_adv_local_get              = */ NULL, /* Deprecated */
    /* .pd_adv_remote_get             = */ NULL, /* Deprecated */
    /* .pd_lb_set                     = */ phy_tscmod_lb_set,
    /* .pd_lb_get                     = */ phy_tscmod_lb_get,
    /* .pd_interface_set              = */ phy_tscmod_interface_set,
    /* .pd_interface_get              = */ phy_tscmod_interface_get,
    /* .pd_ability                    = */ NULL, /* Deprecated */
    /* .pd_linkup_evt                 = */ phy_tscmod_linkup_evt,
    /* .pd_linkdn_evt                 = */ NULL,
    /* .pd_mdix_set                   = */ phy_null_mdix_set,
    /* .pd_mdix_get                   = */ phy_null_mdix_get,
    /* .pd_mdix_status_get            = */ phy_null_mdix_status_get,
    /* .pd_medium_config_set          = */ phy_tscmod_medium_config_set,
    /* .pd_medium_config_get          = */ NULL,
    /* .pd_medium_get                 = */ phy_tscmod_medium_get,
    /* .pd_cable_diag                 = */ NULL,
    /* .pd_link_change                = */ NULL,
    /* .pd_control_set                = */ phy_tscmod_control_set,
    /* .pd_control_get                = */ phy_tscmod_control_get,
    /* .pd_reg_read                   = */ phy_tscmod_reg_read,
    /* .pd_reg_write                  = */ phy_tscmod_reg_write,
    /* .pd_reg_modify                 = */ phy_tscmod_reg_modify,
    /* .pd_notify                     = */ phy_tscmod_notify,
    /* .pd_probe                      = */ phy_tscmod_probe,
    /* .pd_ability_advert_set         = */ phy_tscmod_ability_advert_set,
    /* .pd_ability_advert_get         = */ phy_tscmod_ability_advert_get,
    /* .pd_ability_remote_get         = */ phy_tscmod_ability_remote_get,
    /* .pd_ability_local_get          = */ phy_tscmod_ability_local_get,
    /* .pd_firmware_set               = */ phy_tscmod_firmware_set,
    /* .pdpd_timesync_config_set      = */ NULL,
    /* .pdpd_timesync_config_get      = */ NULL,
    /* .pdpd_timesync_control_set     = */ NULL,
    /* .pdpd_timesync_control_set     = */ NULL,
    /* .pd_diag_ctrl                  = */ phy_tscmod_diag_ctrl,
    /* .pd_lane_control_set           = */ phy_tscmod_per_lane_control_set,    
    /* .pd_lane_control_get           = */ phy_tscmod_per_lane_control_get
};

/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      _phy_tscmod_notify_duplex
 * Purpose:
 *      Program duplex if (and only if) serdestscmod is an intermediate PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, TRUE indicates full duplex, FALSE
 *              indicates half.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *      talk directly to an external fiber module.
 *
 *      If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *      pass-through mode to talk to an external SGMII PHY.
 *
 *      When used in pass-through mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */

STATIC int
_phy_tscmod_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
   phy_ctrl_t  *pc;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

   if(tsc->verbosity & TSCMOD_DBG_LINK) {
      printf("%s p=%0d duplex=0x%0x\n", FUNCTION_NAME(), tsc->port, duplex) ;
   }

   /* Put SERDES PHY in reset */
   SOC_IF_ERROR_RETURN
      (_phy_tscmod_notify_stop(unit, port, PHY_STOP_DUPLEX_CHG));

   /* Autonegotiation must be turned off to talk to external PHY if
    * SGMII autoneg is not enabled.
    */
   if ((!PHY_SGMII_AUTONEG_MODE(unit, port)) &&
       (!PHY_PASSTHRU_MODE(unit, port))) {
      SOC_IF_ERROR_RETURN
         (_phy_tscmod_an_set(unit, port, FALSE));
   }

   


   /* Take SERDES PHY out of reset */
   SOC_IF_ERROR_RETURN
      (_phy_tscmod_notify_resume(unit, port, PHY_STOP_DUPLEX_CHG));

   return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_tscmod_notify_speed
 * Purpose:
 *      Program duplex if (and only if) serdestscmod is an intermediate PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - Speed to program.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *      talk directly to an external fiber module.
 *
 *      If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *      pass-through mode to talk to an external SGMII PHY.
 *
 *      When used in pass-through mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */

STATIC int
_phy_tscmod_notify_speed(int unit, soc_port_t port, uint32 speed)
{
   phy_ctrl_t  *pc;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   int speed_local;
   int intf;
   int asp_mode, scr;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

   if(tsc->verbosity & TSCMOD_DBG_LINK) {
      printf("%s p=%0d speed=%0d\n", FUNCTION_NAME(), tsc->port, speed) ;
   }

   /* Autonegotiation must be turned off to talk to external PHY */
   if (!PHY_SGMII_AUTONEG_MODE(unit, port) && PHY_EXTERNAL_MODE(unit, port)) {
      SOC_IF_ERROR_RETURN
         (_phy_tscmod_an_set(unit, port, FALSE));
   }

   /* Put SERDES PHY in reset */

   /* Update speed */
   SOC_IF_ERROR_RETURN
      (_phy_tscmod_speed_get(unit,port,&speed_local, &intf, &asp_mode, &scr));

   if((speed < 10000)||(speed != speed_local)) {
   SOC_IF_ERROR_RETURN
       (_phy_tscmod_speed_set(unit, port, speed));
   }
   /* Take SERDES PHY out of reset */

   return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_tscmod_stop
 * Purpose:
 *      Put serdestscmod SERDES in or out of reset depending on conditions
 */

STATIC int
_phy_tscmod_stop(int unit, soc_port_t port)
{
   TSCMOD_DEV_DESC_t *pDesc;
   phy_ctrl_t  *pc;
   tscmod_st    *tsc;
   int                 stop, copper;
   int speed, rv, spd_intf ;
   int intf;
   int asp_mode, scr;

   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);
   
   rv  = SOC_E_NONE ;
   spd_intf = 0 ;
   if((tsc->port_type==TSCMOD_SINGLE_PORT)||(tsc->port_type==TSCMOD_DXGXS)) {
      return SOC_E_NONE;
   }

   SOC_IF_ERROR_RETURN
      (_phy_tscmod_speed_get(unit,port,&speed, &intf, &asp_mode, &scr));

   copper = (pc->stop &
              PHY_STOP_COPPER) != 0;

   stop = ((pc->stop &
             (PHY_STOP_PHY_DIS |
              PHY_STOP_DRAIN)) != 0 ||
            (copper &&
             (pc->stop &
              (PHY_STOP_MAC_DIS |
               PHY_STOP_DUPLEX_CHG |
               PHY_STOP_SPEED_CHG)) != 0));

   if(tsc->verbosity & TSCMOD_DBG_LINK) {
      printf("%s p=%0d stop=0x%0x speed=%0d copper=%0d\n",
             FUNCTION_NAME(), tsc->port, stop, speed, copper) ;
   }
   /* 'Stop' only for speeds < 10G.   */
   if(10000 <= speed) {  return SOC_E_NONE; }  /* PHY-1111 */

   if(stop) {
      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
         tsc->per_lane_control = TSCMOD_RXP_UC_OFF ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }
      tsc->per_lane_control = TSCMOD_RXP_REG_OFF ;
      tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);

      if((tsc->spd_intf==TSCMOD_SPD_10_SGMII)||
         (tsc->spd_intf==TSCMOD_SPD_100_SGMII)) {
          tsc->per_lane_control = 0;
          tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
          
          tsc->per_lane_control = 0 ;
          tscmod_tier1_selector("CREDIT_SET", tsc, &rv);
          
          tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
          tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
          
          tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
          tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
      } else {
          tsc->per_lane_control = 0;
          tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
          
          tsc->per_lane_control = 0 | TSCMOD_TX_LANE_TRAFFIC ;
          tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
          
          tsc->per_lane_control = 0 | TSCMOD_TX_LANE_RESET ;
          tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
      }
      tsc->ctrl_type |= TSCMOD_CTRL_TYPE_LINK_DIS ;
   } else {
      if((tsc->ctrl_type&TSCMOD_CTRL_TYPE_LINK_DIS)&&
         ((tsc->spd_intf==TSCMOD_SPD_10_SGMII)||
          (tsc->spd_intf==TSCMOD_SPD_100_SGMII))) {
          spd_intf = tsc->spd_intf ;
          
          tsc->spd_intf = TSCMOD_SPD_1000_SGMII ;
          tsc->per_lane_control = 1 ;
          tscmod_tier1_selector("ENCODE_SET", tsc, &rv);
          
          tsc->per_lane_control = 1 ;
          tscmod_tier1_selector("DECODE_SET", tsc, &rv);
          
          tsc->per_lane_control = 1 ;
          tscmod_tier1_selector("CREDIT_SET", tsc, &rv);
          
          tsc->per_lane_control = 1;
          tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
          
          /* back to 10M or 100M */
          tsc->spd_intf = spd_intf ;
          tsc->per_lane_control = 1 ;
          tscmod_tier1_selector("ENCODE_SET", tsc, &rv);
          
          tsc->per_lane_control = 1 ;
          tscmod_tier1_selector("DECODE_SET", tsc, &rv);
          
          tsc->per_lane_control = 1 ;
          tscmod_tier1_selector("CREDIT_SET", tsc, &rv);
          
          tsc->per_lane_control = 1;
          tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
      } else {
          tsc->per_lane_control = 1;
          tscmod_tier1_selector("TX_LANE_CONTROL", tsc, &rv);
          
          tsc->per_lane_control = 1;
          tscmod_tier1_selector("CREDIT_CONTROL", tsc, &rv);
      }

      if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_RXP) {
         tsc->per_lane_control = TSCMOD_RXP_UC_ON ;
         tscmod_tier1_selector("RX_LANE_CONTROL", tsc, &rv);
      }
      
      if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_LINK_DIS)
          tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_LINK_DIS ;
      
      tsc->per_lane_control = (1 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
      tscmod_tier1_selector("SOFT_RESET", tsc, &rv);
      sal_usleep(1000) ;

      if(!(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)) {
         /* this call affects AN */
         tsc->per_lane_control = (0 << TSCMOD_SRESET_SHIFT) |  TSCMOD_SRESET_DSC_SM ;
         tscmod_tier1_selector("SOFT_RESET", tsc, &rv);

         sal_usleep(1000) ;
      }
   }

   return rv;
}

/*
 * Function:
 *      _phy_tscmod_notify_stop
 * Purpose:
 *      Add a reason to put serdestscmod PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_tscmod_notify_stop(int unit, soc_port_t port, uint32 flags)
{
   INT_PHY_SW_STATE(unit, port)->stop |= flags;
   return _phy_tscmod_stop(unit, port);
}

/*
 * Function:
 *      _phy_tscmod_notify_resume
 * Purpose:
 *      Remove a reason to put serdestscmod PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_tscmod_notify_resume(int unit, soc_port_t port, uint32 flags)
{
   INT_PHY_SW_STATE(unit, port)->stop &= ~flags;
   return _phy_tscmod_stop(unit, port);
}

/*
 * Function:
 *      phy_tscmod_media_setup
 * Purpose:
 *      Configure
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      fiber_mode - Configure for fiber mode
 *      fiber_pref - Fiber preferrred (if fiber mode)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_tscmod_notify_interface(int unit, soc_port_t port, uint32 intf)
{
   phy_ctrl_t *pc;
   tscmod_st    *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;

   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   =  (tscmod_st *)( pDesc + 1);

   if (intf != SOC_PORT_IF_SGMII) {
      if(tsc->verbosity & TSCMOD_DBG_LINK) {
         printf("%s p=%0d intf=0x%0x not supported\n",
                FUNCTION_NAME(), tsc->port, intf) ;
      }
   } else {
      if(tsc->verbosity & TSCMOD_DBG_LINK) {
         printf("%s p=%0d intf=0x%0x SGMII\n",
                FUNCTION_NAME(), tsc->port, intf) ;
      }
      SOC_IF_ERROR_RETURN
         (_phy_tscmod_speed_set(unit, port, 1000));
   }
   return SOC_E_NONE;
}


STATIC
int _tscmod_chip_an_init_done(int unit,int chip_num)
{
    int port;
    phy_ctrl_t  *pc;
    tscmod_st            *tsc;      /*tscmod structure */
    TSCMOD_DEV_DESC_t    *pDesc;

    PBMP_ALL_ITER(unit, port) {  /* value of port will be changed after macro expanded */
        pc = INT_PHY_SW_STATE(unit, port);
        if (pc == NULL) {
            continue;
        }
        if (!pc->dev_name || pc->dev_name != tscmod_device_name) {
            continue;
        }
        if (pc->chip_num != chip_num) {
            continue;
        }
        if ((pc->flags & PHYCTRL_INIT_DONE)&&(pc->flags & PHYCTRL_ANEG_INIT_DONE)) {
           pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
           tsc   =  (tscmod_st *)( pDesc + 1);
           if(tsc->verbosity&TSCMOD_DBG_INIT)
              printf("%-22s PBMP_ALL_ITER scan u=%0d p=%0d trued\n", FUNCTION_NAME(), unit, port) ;
           return TRUE;
        }
    }
    return FALSE;
}
STATIC
int _tscmod_chip_init_done(int unit,int chip_num,int phy_mode, int *init_mode, uint32 *uc_info)
{
    int port;
    phy_ctrl_t  *pc;
    tscmod_st            *tsc;      /*tscmod structure */
    TSCMOD_DEV_DESC_t    *pDesc;
    TSCMOD_DEV_CFG_t     *pCfg;

    PBMP_ALL_ITER(unit, port) {  /* value of port will be changed after macro expanded */
        pc = INT_PHY_SW_STATE(unit, port);
        if (pc == NULL) {
            continue;
        }
        if (!pc->dev_name || pc->dev_name != tscmod_device_name) {
            continue;
        }
        if (pc->chip_num != chip_num) {
            continue;
        }
        if (pc->flags & PHYCTRL_INIT_DONE) {
           pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
           tsc   =  (tscmod_st *)( pDesc + 1);
           pCfg  = &pDesc->cfg;

           *init_mode = 0 ;
           if(tsc->verbosity&TSCMOD_DBG_INIT)
               printf("%-22s PBMP_ALL_ITER scan u=%0d p=%0d inited\n",
                   FUNCTION_NAME(), unit, port) ;
           if((tsc->ctrl_type & ~TSCMOD_CTRL_TYPE_MASK )==TSCMOD_CTRL_TYPE_DEFAULT) {
              if(tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED) {
                 *init_mode |= TSCMOD_CTRL_TYPE_FW_LOADED;
                 *uc_info = (pCfg->uc_ver) ;
                 *uc_info = (*uc_info<<16) | pCfg->uc_crc ;
              }
           }
           return TRUE;
        }
    }
    return FALSE;
}

STATIC int tscmod_get_lane_swap(int core, int sub_core, int tx, int svk_id)
{
   int lane_map ;
   if(svk_id==0x56850) {
      if(tx) {
         if(core % 2) lane_map = TSCMOD_TX_LANE_MAP_DEFAULT;
         else  {
            if(core==16) lane_map = TSCMOD_TX_LANE_MAP_DEFAULT;
            else         lane_map = 0x1230 ;
         }
      } else {
         lane_map = 0x3012 ;
      }
   } else {
      if(tx) lane_map = TSCMOD_TX_LANE_MAP_DEFAULT;
      else   lane_map = TSCMOD_RX_LANE_MAP_DEFAULT;
   }
   return lane_map ;
}

STATIC void tscmod_uc_cap_set(tscmod_st* tsc, TSCMOD_DEV_CFG_t  *pCfg, int cap, int ver)
{
   if(cap==TSCMOD_UC_CAP_ALL) {
      if((ver >= 0xa00c)&&(pCfg->cidL&TSC623_PHY_864)) {
         tsc->ctrl_type |= TSCMOD_CTRL_TYPE_UC_CAP_RXP ;
         tsc->ctrl_type |= TSCMOD_CTRL_TYPE_UC_CL72_FW ;
      }

      if(TSCMOD_MODEL_REVID_GET(tsc)<TSCMOD_MODEL_REVID_A2) {   /* WAR on PHY-1073 */
         tsc->ctrl_type |= TSCMOD_CTRL_TYPE_ALL_PROXY ;
         tsc->ctrl_type |= TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR ;
      } else {
          if(ver >=0xa036) {
              if(pCfg->ctrl_flag & TSCMOD_CFG_CTRL_FLAG_WR_MASK_EN) {
                  tsc->ctrl_type |= TSCMOD_CTRL_TYPE_UC_WR_MASK_EN ;
                  if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY)
                      tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_ALL_PROXY ;
                  if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR)
                      tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR ;
              } else {
                  if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_WR_MASK_EN)
                      tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_UC_WR_MASK_EN ;
                  tsc->ctrl_type |= TSCMOD_CTRL_TYPE_ALL_PROXY ;
                  tsc->ctrl_type |= TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR ;
              }
          } else {
              if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_UC_WR_MASK_EN)
                      tsc->ctrl_type ^= TSCMOD_CTRL_TYPE_UC_WR_MASK_EN ;
              tsc->ctrl_type |= TSCMOD_CTRL_TYPE_ALL_PROXY ;
              tsc->ctrl_type |= TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR ;
          }
      }
   }
}

/* 
 * WB functions
 */

/* this function will update scache or retrieve from scache or count the size of 
   each variable set indexed by id.  Set offset correctly for update or retrieve
 */
#ifdef BCM_WARM_BOOT_SUPPORT
STATIC 
int tscmod_wb_id_command(int unit, soc_port_t port, int offset, 
                         TSCMOD_WB_HANDLER_TYPE_t type, TSCMOD_WB_DATA_ID_t id, int *size) {
    phy_ctrl_t *pc;
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    TSCMOD_DEV_CFG_t *pCfg;
    uint8 *tsc_scache_ptr;
    int rv, vsize, loc_off, ver ;

    pc    = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
 	tsc   = (tscmod_st *)( pDesc + 1);
    pCfg  = &pDesc->cfg;
    rv    = SOC_E_NONE ;
    vsize = 0 ; 
    loc_off = offset ;

    tsc_scache_ptr = pCfg->scache_ptr ;
    ver            = pCfg->scache_ver ;
    rv = SOC_E_NONE ;
    
    if(type==TSCMOD_WB_SC_COUNT){
        /* no basic check at this moment */
    } else {
        /* check for ptr */
        if(tsc_scache_ptr==NULL) {
            return rv ;
        }
    }
    switch(id) {
    case TSCMOD_WB_SC_VER:
        if(ver >= TSCMOD_WB_SC_VER1) {
            TSCMOD_WB_ID_CMD(tsc_scache_ptr, pCfg->scache_ver,type,&loc_off,&vsize);
        }
        break ;
    case TSCMOD_WB_FW_MODE:
        if(ver >= TSCMOD_WB_SC_VER1) {
            TSCMOD_WB_ID_CMD(tsc_scache_ptr, tsc->firmware_mode, type,&loc_off,&vsize);
        }
        break ;
    case TSCMOD_WB_CTRL_TYPE:
        if(ver >= TSCMOD_WB_SC_VER1) {
            TSCMOD_WB_ID_CMD(tsc_scache_ptr, tsc->ctrl_type, type,&loc_off,&vsize);
        }
        break ;
    case TSCMOD_WB_RXLOS:
        if(ver >= TSCMOD_WB_SC_VER1) {
            TSCMOD_WB_ID_CMD(tsc_scache_ptr, DEV_CFG_PTR(pc)->sw_rx_los.state, type,&loc_off,&vsize);
            TSCMOD_WB_ID_CMD(tsc_scache_ptr, DEV_CFG_PTR(pc)->sw_rx_los.enable,type,&loc_off,&vsize);
            TSCMOD_WB_ID_CMD(tsc_scache_ptr, DEV_CFG_PTR(pc)->sw_rx_los.rf_dis,type,&loc_off,&vsize);
        }
        break ;
    case TSCMOD_WB_CL72_WAR:
        if(ver >= TSCMOD_WB_SC_VER1) {
            TSCMOD_WB_ID_CMD(tsc_scache_ptr, DEV_CTRL_PTR(pc)->cl72.state,  type,&loc_off,&vsize);
            TSCMOD_WB_ID_CMD(tsc_scache_ptr, DEV_CTRL_PTR(pc)->cl72.enabled,type,&loc_off,&vsize);
            TSCMOD_WB_ID_CMD(tsc_scache_ptr, DEV_CTRL_PTR(pc)->cl72.mode,   type,&loc_off,&vsize);
        }
        break ;
    case TSCMOD_WB_INTF:
        if(ver >= TSCMOD_WB_SC_VER1) {
            TSCMOD_WB_ID_CMD(tsc_scache_ptr, DEV_CFG_PTR(pc)->line_intf, type,&loc_off,&vsize);
        }
        break ;
    case TSCMOD_WB_AN_MODE:
        if(ver >= TSCMOD_WB_SC_VER1) {
            TSCMOD_WB_ID_CMD(tsc_scache_ptr, tsc->an_type, type,&loc_off,&vsize);
        }
    case TSCMOD_WB_SC_LAST:
        break ;
    default:
        rv = SOC_E_PARAM ;
        break ;
    }
    *size = vsize ;
    return rv ;    
}
#endif

STATIC 
int tscmod_wb_sc_handler(int unit, soc_port_t port, TSCMOD_WB_HANDLER_TYPE_t type, TSCMOD_WB_DATA_ID_t id) 
{
#ifdef BCM_WARM_BOOT_SUPPORT
    int i, rv, size, offset ;
    rv   = SOC_E_NONE ; 
    size   = 0 ;
    offset = 0 ;
    for(i=0; i<TSCMOD_WB_SC_LAST;i++) {
        if(i==id) {
            rv |= tscmod_wb_id_command(unit,port, offset, type, id, &size) ;
            break ;
        } else {
            rv |= tscmod_wb_id_command(unit, port, 0, TSCMOD_WB_SC_COUNT, i, &size) ;
            offset += size ;
        }
    }
    return rv ;
#else
    return SOC_E_NONE ;
#endif
}

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC 
int tscmod_wb_sc_size_count(int unit, soc_port_t port, int *size) 
{
    int i, rv, total, sz ;
    rv   = SOC_E_NONE ;
    sz    = 0 ;
    total = 0 ;
    for(i=0; i<TSCMOD_WB_SC_LAST;i++) {
        rv |= tscmod_wb_id_command(unit, port, 0, TSCMOD_WB_SC_COUNT, i, &sz) ;
        total += sz ;
    }
    *size = total ;
    return rv ;
}
#endif


/* copy all variables to SCache */
#ifdef BCM_WARM_BOOT_SUPPORT
STATIC 
int tscmod_wb_sc_sync(int unit, soc_port_t port) 
{
    int i, rv, offset, size ;


    rv     = SOC_E_NONE ;
    size   = 0 ;
    offset = 0 ;
    for(i=0; i<TSCMOD_WB_SC_LAST;i++) {
        rv |= tscmod_wb_id_command(unit, port, offset, TSCMOD_WB_SC_UPDATE, i, &size) ;
        offset += size ;
    }
    return rv ;
}
#endif

/* retrieve all variables from SCache */
#ifdef BCM_WARM_BOOT_SUPPORT
STATIC 
int tscmod_wb_sc_reinit(int unit, soc_port_t port) 
{
    int i, rv, offset, size ;
    soc_scache_handle_t scache_handle;
    uint8 *tsc_scache_ptr;
    uint32 alloc_get;
    int    alloc_size ;

    phy_ctrl_t *pc;
    TSCMOD_DEV_DESC_t     *pDesc;
    TSCMOD_DEV_CFG_t *pCfg;
    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    pCfg = &pDesc->cfg;

    rv     = SOC_E_NONE ;
    size   = 0 ;
    offset = 0 ;
    alloc_size = 0 ;
    alloc_get  = 0 ;

    
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          SOC_SCACHE_SERDES_HANDLE, port);
    /* Get the pointer for the TSC cache */
    rv = soc_scache_ptr_get(unit, scache_handle,
                            &tsc_scache_ptr, &alloc_get);
    
    if(rv) {
       printf("Warning: u=%0d p=%0d WB reinit scahce not available rv=%x\n", unit, port, rv);
       pCfg->scache_ptr = NULL ;
       pCfg->scache_ver = 0 ;
       return rv ;
    } else {
        /* to find out the existing scache is an old version or not */
       if(alloc_get < sizeof(pCfg->scache_ver)) {
           pCfg->scache_ptr = NULL ;
           pCfg->scache_ver = 0 ;
           return SOC_E_INTERNAL;
       } else {
           pCfg->scache_ptr = tsc_scache_ptr ;
           pCfg->scache_size= alloc_size ;
           /* copy from tsc_scache_ptr to pCfg->scache_ver */
           sal_memcpy(&pCfg->scache_ver, tsc_scache_ptr, sizeof(pCfg->scache_ver));
       }
       rv = tscmod_wb_sc_size_count(unit, port, &alloc_size) ;
       if((alloc_get < alloc_size)||(alloc_get > (alloc_size+15))) {
            /* Expected size doesn't match retrieved size */
            pCfg->scache_ptr = NULL ;
            pCfg->scache_ver = 0 ;
            return SOC_E_INTERNAL;
       }
       rv = SOC_E_NONE ;
    }
    
    for(i=0; i<TSCMOD_WB_SC_LAST;i++) {
        rv |= tscmod_wb_id_command(unit, port, offset, TSCMOD_WB_SC_RETRIEVE, i, &size) ;
        offset += size ;
    }
    return rv ;
}
#endif

/* initialize the space, only be called when !SOC_WARM_BOOT(unit) */
#ifdef BCM_WARM_BOOT_SUPPORT
STATIC 
int tscmod_wb_sc_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    TSCMOD_DEV_DESC_t     *pDesc;
    TSCMOD_DEV_CFG_t *pCfg;
    soc_scache_handle_t scache_handle;
    uint8 *tsc_scache_ptr;
    uint32 alloc_get;
    int    alloc_size ;
    int    current_ver ;
    int rv ;
    rv   = SOC_E_NONE ;

    alloc_size = 0 ;
    alloc_get  = 0 ;
    current_ver= TSCMOD_WB_SC_VER1 ;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    pCfg = &pDesc->cfg;
        
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          SOC_SCACHE_SERDES_HANDLE, port);
    /* Get the pointer for the TSC cache */
    rv = soc_scache_ptr_get(unit, scache_handle,
                            &tsc_scache_ptr, &alloc_get);
    if (SOC_E_NOT_FOUND == rv) {
        /* Not yet allocated in Cold Boot */
        pCfg->scache_ver = current_ver ;
        rv = tscmod_wb_sc_size_count(unit, port, &alloc_size) ;
        SOC_IF_ERROR_RETURN
            (soc_scache_alloc(unit, scache_handle, alloc_size));
        rv = soc_scache_ptr_get(unit, scache_handle,
                                &tsc_scache_ptr, &alloc_get);
        if (SOC_FAILURE(rv)) {
            return rv;
        } else if ((alloc_get < alloc_size)||(alloc_get > (alloc_size+15))) {
            /* Expected size doesn't match retrieved size */
            return SOC_E_INTERNAL;
        } else if (NULL == tsc_scache_ptr) {
            return SOC_E_MEMORY;
        }
        pCfg->scache_ptr = tsc_scache_ptr ;
        pCfg->scache_size= alloc_size ;
        
        /* set up version */
        pCfg->scache_ver = current_ver ;  
        sal_memcpy(tsc_scache_ptr, &pCfg->scache_ver, sizeof(pCfg->scache_ver));
    } else if(SOC_E_PARAM ==rv) {
        pCfg->scache_ver = 0 ;
        pCfg->scache_ptr = NULL ;
        pCfg->scache_size= 0 ;
    } else {
        /* mem space has been created already */
        pCfg->scache_ver = current_ver ;
        rv = tscmod_wb_sc_size_count(unit, port, &alloc_size) ;
        if((alloc_get < alloc_size)||(alloc_get > (alloc_size+15))) {
            /* Expected size doesn't match retrieved size */
            pCfg->scache_ptr = NULL ;
            return SOC_E_INTERNAL;
        }
        pCfg->scache_ptr = tsc_scache_ptr ;
        pCfg->scache_size= alloc_size ;
        pCfg->scache_ver = current_ver ;    
        sal_memcpy(tsc_scache_ptr, &pCfg->scache_ver, sizeof(pCfg->scache_ver));
    }
    return rv ;
}
#endif

/* need to expand space */
#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int tscmod_wb_sc_realloc_init(int unit, soc_port_t port) 
{
    phy_ctrl_t *pc;
    TSCMOD_DEV_DESC_t     *pDesc;
    TSCMOD_DEV_CFG_t *pCfg;
    soc_scache_handle_t scache_handle;
    uint8 *tsc_scache_ptr;
    uint32 alloc_get;
    int    alloc_size1 ;
    int    alloc_size2 ;
    int    current_ver ;
    int rv ;
    rv   = SOC_E_NONE ;

    alloc_size1= 0 ;
    alloc_size2= 0 ;
    alloc_get  = 0 ;
    current_ver= TSCMOD_WB_SC_VER1 ;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    pCfg = &pDesc->cfg;
        
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          SOC_SCACHE_SERDES_HANDLE, port);

    /* Get the pointer for the TSC cache */
    rv = soc_scache_ptr_get(unit, scache_handle,
                            &tsc_scache_ptr, &alloc_get);
    if (rv) {
        pCfg->scache_ver = 0 ;
        pCfg->scache_ptr = NULL ;
        pCfg->scache_size= 0 ;
        return SOC_E_INTERNAL;
    } else {
        /* mem space has been created already */
        rv = tscmod_wb_sc_size_count(unit, port, &alloc_size1) ;
        pCfg->scache_ver = current_ver ;
        rv = tscmod_wb_sc_size_count(unit, port, &alloc_size2) ;
        SOC_IF_ERROR_RETURN
            (soc_scache_realloc(unit, scache_handle, (alloc_size2-alloc_size1)));

        rv = soc_scache_ptr_get(unit, scache_handle,
                                &tsc_scache_ptr, &alloc_get);
        if((alloc_get < alloc_size2)||(alloc_get > (alloc_size2+15))) {
            /* Expected size doesn't match retrieved size */
            pCfg->scache_ptr = NULL ;
            return SOC_E_INTERNAL;
        }
        pCfg->scache_ptr = tsc_scache_ptr ;
        pCfg->scache_size= alloc_size2 ;
        pCfg->scache_ver = current_ver ;    
        sal_memcpy(tsc_scache_ptr, &pCfg->scache_ver, sizeof(pCfg->scache_ver));

    }
    return rv ;   
}
#endif


/******************************************************************************
*              Begin Diagnostic Code                                          *
 */

#define TSCMOD_DIAG_MSG_LEN1  80
#define TSCMOD_DIAG_MSG_LEN2  80
#define TSCMOD_DIAG_MSG_LEN3  140
#define TSCMOD_DIAG_MSG_LEN4  16

STATIC char* tscmod_ability_msg0(int data) {
   int i ;
   static char msg[TSCMOD_DIAG_MSG_LEN1] = {0} ;

   for(i=0; i<TSCMOD_DIAG_MSG_LEN1; i++) msg[i] = '\0' ;
   /* make sure no overflow */
   if(data&SOC_PA_SPEED_42GB)   strcat(msg,"42G ") ;
   if(data&SOC_PA_SPEED_40GB)   strcat(msg,"40G ") ;
   /* if(data&SOC_PA_SPEED_32GB)   strcat(msg,"32G ") ; */
   if(data&SOC_PA_SPEED_25GB)   strcat(msg,"25G ") ;
   if(data&SOC_PA_SPEED_20GB)   strcat(msg,"20G ") ;
   if(data&SOC_PA_SPEED_16GB)   strcat(msg,"16G ") ;
   if(data&SOC_PA_SPEED_15GB)   strcat(msg,"15G ") ;
   if(data&SOC_PA_SPEED_13GB)   strcat(msg,"13G ") ;
   if(data&SOC_PA_SPEED_12P5GB) strcat(msg,"12.5G ") ;
   if(data&SOC_PA_SPEED_11GB)   strcat(msg,"11G ") ;
   if(data&SOC_PA_SPEED_10GB)   strcat(msg,"10G ") ;
   if(data&SOC_PA_SPEED_5000MB) strcat(msg,"5G ") ;
   if(data&SOC_PA_SPEED_2500MB) strcat(msg,"2p5G ") ;
   if(data&SOC_PA_SPEED_1000MB) strcat(msg,"1G ") ;
   if(data&SOC_PA_SPEED_100MB)  strcat(msg,"100M ") ;
   if(data&SOC_PA_SPEED_100MB)  strcat(msg,"10M ") ;

   return (char *)msg;
}

STATIC char* tscmod_cl73_ability_msg0(int data) {
   int i ;
   static char msg[TSCMOD_DIAG_MSG_LEN2] = {0} ;

   for(i=0; i<TSCMOD_DIAG_MSG_LEN2; i++) msg[i] = '\0' ;

   if(data&(1<<TSCMOD_ABILITY_100G_CR10))strcat(msg,"100GCR10 ") ;
   if(data&(1<<TSCMOD_ABILITY_40G_CR4))  strcat(msg,"40GCR4 ") ;
   if(data&(1<<TSCMOD_ABILITY_40G_KR4))  strcat(msg,"40GKR4 ") ;
   if(data&(1<<TSCMOD_ABILITY_20G_CR2))  strcat(msg,"20GCR2 ") ;
   if(data&(1<<TSCMOD_ABILITY_20G_KR2))  strcat(msg,"20GKR2 ") ;
   if(data&(1<<TSCMOD_ABILITY_10G_KR))   strcat(msg,"10GKR ") ;
   if(data&(1<<TSCMOD_ABILITY_10G_KX4))  strcat(msg,"10GKX4 ") ;
   if(data&(1<<TSCMOD_ABILITY_1G_KX))    strcat(msg,"1GKX ") ;
   if(data&TSCMOD_ABILITY_SYMM_PAUSE)    strcat(msg,"SYM_PA ") ;
   if(data&TSCMOD_ABILITY_ASYM_PAUSE)    strcat(msg,"ASY_PA ") ;
   return (char *)msg;
}

STATIC char* tscmod_cl37bam_ability_msg0(int data) {
   int i ;
   static char msg[TSCMOD_DIAG_MSG_LEN3] = {0} ;
   for(i=0; i<TSCMOD_DIAG_MSG_LEN3; i++) msg[i] = '\0' ;

   if(data&(1<<TSCMOD_BAM37ABL_40G)){
       sal_strncat(msg, "40G ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_32P7G)){
      sal_strncat(msg, "32.7G ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_31P5G)){
      sal_strncat(msg, "31.5G ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_25P455G)){
      sal_strncat(msg, "25G ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_21G_X4)){
      sal_strncat(msg, "21G ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_20G_X2_CX4)){
      sal_strncat(msg, "20G_X2_CX4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_20G_X2)){
      sal_strncat(msg, "20G_X2 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_20G_X4)){
      sal_strncat(msg, "20G_X4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_20G_X4_CX4)){
      sal_strncat(msg, "20G_X4_CX4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_16G_X4)){
      sal_strncat(msg, "16G_X4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_15P75G_R2)){
      sal_strncat(msg, "15.75G_R2 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_15G_X4)){
      sal_strncat(msg, "15G_X4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_13G_X4)){
      sal_strncat(msg, "13G_X4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }


   return (char *)msg;
}
STATIC char* tscmod_cl37bam_ability_msg1(int data) {
   int i ;
   static char msg[TSCMOD_DIAG_MSG_LEN3] = {0} ;
   for(i=0; i<TSCMOD_DIAG_MSG_LEN1; i++) msg[i] = '\0' ;  
   /* no need to clean up the whole buffer */

   if(data&(1<<TSCMOD_BAM37ABL_12P7_DXGXS)){
       sal_strncat(msg,"12.7G_DX ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_12P5_X4)){
       sal_strncat(msg,"12.5G_X4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_12G_X4)){
       sal_strncat(msg,"12G_X4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_10P5G_DXGXS)){
       sal_strncat(msg,"10.5G_DX ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_10G_X2_CX4)){
       sal_strncat(msg,"10G_X2_CX4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_10G_DXGXS)){
       sal_strncat(msg,"10G_DX ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_10G_CX4)){
       sal_strncat(msg,"10G_CX4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_10G_HIGIG)){
       sal_strncat(msg,"10G_HG ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_6G_X4)){
       sal_strncat(msg,"6G_X4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_5G_X4)){
       sal_strncat(msg,"5G_X4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(data&(1<<TSCMOD_BAM37ABL_2P5G)){
       sal_strncat(msg,"2.5G ", sizeof(msg) - sal_strlen(msg) - 1);
   }

   return (char *)msg;
}

STATIC char* tscmod_line_intf_msg(uint32 line_intf) {
   int i ;
   static char msg[TSCMOD_DIAG_MSG_LEN3] = {0} ;
   for(i=0; i<TSCMOD_DIAG_MSG_LEN1; i++) msg[i] = '\0' ;
   /* no need to clean up the whole buffer */

   if(line_intf&TSCMOD_IF_SR){
       sal_strncat(msg, "SR ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(line_intf&TSCMOD_IF_SR4){
       sal_strncat(msg, "SR4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(line_intf&TSCMOD_IF_KR){
       sal_strncat(msg, "KR ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(line_intf&TSCMOD_IF_KR2){
       sal_strncat(msg, "KR2 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(line_intf&TSCMOD_IF_KR4){
       sal_strncat(msg, "KR4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(line_intf&TSCMOD_IF_CR){
       sal_strncat(msg, "CR ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(line_intf&TSCMOD_IF_CR){
       sal_strncat(msg, "CR2 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(line_intf&TSCMOD_IF_CR4){
       sal_strncat(msg, "CR4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(line_intf&TSCMOD_IF_XFI){
       sal_strncat(msg, "XFI ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(line_intf&TSCMOD_IF_SFI){
       sal_strncat(msg, "SFI ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(line_intf&TSCMOD_IF_XLAUI){
       sal_strncat(msg, "XLAUI ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(line_intf&TSCMOD_IF_XGMII){
       sal_strncat(msg, "XGMII ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   return (char *)msg;
}

STATIC int tscmod_tr_if_mode(int soc_port) {
    int tscmod_if ;
    /* SOC_PORT_IF_XAUI is mapped to SOC_PORT_IF_XGMII */
    switch(soc_port) {
    case SOC_PORT_IF_NOCXN:  tscmod_if = TSCMOD_PORT_IF_NOCXN ; break ;
    case SOC_PORT_IF_NULL:  tscmod_if = TSCMOD_PORT_IF_NULL ; break ;
    case SOC_PORT_IF_MII:  tscmod_if = TSCMOD_PORT_IF_MII ; break ;
    case SOC_PORT_IF_GMII:  tscmod_if = TSCMOD_PORT_IF_GMII ; break ;
    case SOC_PORT_IF_SGMII:  tscmod_if = TSCMOD_PORT_IF_SGMII ; break ;
    case SOC_PORT_IF_TBI:  tscmod_if = TSCMOD_PORT_IF_TBI ; break ;
    case SOC_PORT_IF_XGMII:  tscmod_if = TSCMOD_PORT_IF_XGMII ; break ;
    case SOC_PORT_IF_RGMII:  tscmod_if = TSCMOD_PORT_IF_RGMII ; break ;
    case SOC_PORT_IF_RvMII:  tscmod_if = TSCMOD_PORT_IF_RvMII ; break ;
    case SOC_PORT_IF_SFI:  tscmod_if = TSCMOD_PORT_IF_SFI ; break ;
    case SOC_PORT_IF_XFI:  tscmod_if = TSCMOD_PORT_IF_XFI ; break ;
    case SOC_PORT_IF_KR:  tscmod_if = TSCMOD_PORT_IF_KR ; break ;
    case SOC_PORT_IF_KR4:  tscmod_if = TSCMOD_PORT_IF_KR4 ; break ;
    case SOC_PORT_IF_CR:  tscmod_if = TSCMOD_PORT_IF_CR ; break ;
    case SOC_PORT_IF_CR4:  tscmod_if = TSCMOD_PORT_IF_CR4 ; break ;
    case SOC_PORT_IF_XLAUI:  tscmod_if = TSCMOD_PORT_IF_XLAUI ; break ;
    case SOC_PORT_IF_SR:  tscmod_if = TSCMOD_PORT_IF_SR ; break ;
    case SOC_PORT_IF_RXAUI:  tscmod_if = TSCMOD_PORT_IF_RXAUI ; break ;
    case SOC_PORT_IF_SR4:  tscmod_if = TSCMOD_PORT_IF_SR4 ; break ;
    case SOC_PORT_IF_LR:   tscmod_if = TSCMOD_PORT_IF_LR; break;
    case SOC_PORT_IF_LR4:  tscmod_if = TSCMOD_PORT_IF_LR4; break;
    case SOC_PORT_IF_KX:   tscmod_if = TSCMOD_PORT_IF_KX ; break ;
    case SOC_PORT_IF_SR2:  tscmod_if = TSCMOD_PORT_IF_SR2 ; break ;
    case SOC_PORT_IF_KR2:  tscmod_if = TSCMOD_PORT_IF_KR2 ; break ;
    case SOC_PORT_IF_CR2:  tscmod_if = TSCMOD_PORT_IF_CR2 ; break ;
    case SOC_PORT_IF_XFI2:  tscmod_if = TSCMOD_PORT_IF_XFI2 ; break ;
    case SOC_PORT_IF_XLAUI2:  tscmod_if = TSCMOD_PORT_IF_XLAUI2 ; break ;
    default: tscmod_if = TSCMOD_PORT_IF_COUNT ; break ;
    }
    return tscmod_if ;
}

STATIC char* tscmod_iintf_msg(int iintf) {
   int i ;
   static char msg[TSCMOD_DIAG_MSG_LEN1] = {0} ;
   for(i=0; i<TSCMOD_DIAG_MSG_LEN1; i++) msg[i] = '\0' ;

   if(iintf&TSCMOD_IIF_XLAUI){
       sal_strncat(msg, "IIF_XLAUI ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(iintf&TSCMOD_IIF_SR4){
       sal_strncat(msg, "IIF_SR4 ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(iintf&TSCMOD_IIF_SFPDAC){
       sal_strncat(msg, "IIF_SFPDAC ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   if(iintf&TSCMOD_IIF_NOXFI){
       sal_strncat(msg, "IIF_NOXFI ", sizeof(msg) - sal_strlen(msg) - 1);
   }
   return (char *)msg;
}

STATIC char* tscmod_version_msg(void) {
   int i ;
   static char msg[TSCMOD_DIAG_MSG_LEN4] = {0} ;
   for(i=0; i<TSCMOD_DIAG_MSG_LEN4; i++) msg[i] = '\0' ;

   strcat(msg,"rev=6.4.0") ;
   return (char *)msg;
}

#if 0
void tscmod_ability_print(int data) {
   int idx ;
   static char* tscmod_cl37bam_ability_e2s [CNT_tscmod_cl37bam_ability] = {
      "2P5G" ,      "5G_X4" ,      "6G_X4" ,
      "10G_HIGIG" , "10G_CX4" ,    "12G_X4" ,
      "12P5_X4" ,   "13G_X4" ,     "15G_X4" ,
      "16G_X4" ,    "20G_X4_CX4",  "20G_X4" ,
      "21G_X4" ,    "25P455G" ,    "31P5G" ,
      "32P7G" ,     "40G" ,        "10G_X2_CX4" ,
      "10G_DXGXS" , "10P5G_DXGXS", "12P7_DXGXS" ,
      "15P75G_R2" , "20G_X2_CX4" ,  "20G_X2" ,
      "TSCMOD_BAM37ABL_ILLEGAL"
   }; /* e2s_tscmod_cl37bam_ability */
   printf("bam37_ability:[") ;
   for(idx=0; idx<CNT_tscmod_cl37bam_ability; idx++) {
      if( data & (1<<idx) ) {
         printf(" %s", tscmod_cl37bam_ability_e2s[idx]) ;
      }
   }
   printf("]\n") ;
}

void tscmod_tech_print(int data) {
   int idx ;
   static char* tscmod_tech_ability_e2s [CNT_tscmod_tech_ability] = {
      "1G_KX" ,
      "10G_KX4" ,
      "10G_KR" ,
      "40G_KR4" ,
      "40G_CR4" ,
      "100G_CR10" ,
      "20G_KR2" ,
      "20G_CR2" ,
      "ABILITY_ILLEGAL"
   }; /* e2s_tscmod_tech_ability */

   printf("tech_ability:[") ;
   for(idx=0; idx<CNT_tscmod_tech_ability; idx++) {
      if( data & (1<<idx) ) {
         printf(" %s", tscmod_tech_ability_e2s[idx]) ;
      }
   }
   printf("]\n") ;
}

#endif   /* #if 0 */

STATIC int
_phy_tscmod_cfg_dump(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    TSCMOD_DEV_CFG_t *pCfg;
    TSCMOD_DEV_INFO_t  *pInfo;
    TSCMOD_DEV_DESC_t *pDesc;
    soc_phy_info_t *pi;
    tscmod_st    *tsc;

    int i;
    int size;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc   =  (tscmod_st *)( pDesc + 1);
    pi = &SOC_PHY_INFO(unit, port);


    pCfg = &pDesc->cfg;
    pInfo = &pDesc->info;
    if (tsc->port_type == TSCMOD_SINGLE_PORT ) {
        size = 4;
    } else if (tsc->dxgxs & 0x3) {
        size = 2;
    } else {
        size = 1;
    }

    LOG_CLI((BSL_META_U(unit,
                        "pc = 0x%x, pCfg = 0x%x, pInfo = 0x%x tsc=%p\n"), (int)(size_t)pc,
             (int)(size_t)pCfg,(int)(size_t)pInfo, (void *)tsc));
    for (i = 0; i < size; i++) {
        LOG_CLI((BSL_META_U(unit,
                            "preemph%d     0x%x\n"),
                 i, pCfg->preemph[i]));
        LOG_CLI((BSL_META_U(unit,
                            "idriver%d     0x%04x\n"),
                 i, pCfg->idriver[i]));
        LOG_CLI((BSL_META_U(unit,
                            "pdriver%d     0x%04x\n"),
                 i, pCfg->pdriver[i]));
    }
    LOG_CLI((BSL_META_U(unit,
                        "auto_medium  0x%04x\n"),
             pCfg->auto_medium));
    LOG_CLI((BSL_META_U(unit,
                        "fiber_pref   0x%04x\n"),
             pCfg->fiber_pref));
    LOG_CLI((BSL_META_U(unit,
                        "sgmii_mstr   0x%04x\n"),
             pCfg->sgmii_mstr));
    LOG_CLI((BSL_META_U(unit,
                        "medium       0x%04x\n"),
             pCfg->medium));
    LOG_CLI((BSL_META_U(unit,
                        "pdetect10g   0x%04x\n"),
             pCfg->pdetect10g));
    LOG_CLI((BSL_META_U(unit,
                        "pdetect1000x 0x%04x\n"),
             pCfg->pdetect1000x));
    LOG_CLI((BSL_META_U(unit,
                        "cx42hg       0x%04x\n"),
             pCfg->cx42hg));
    LOG_CLI((BSL_META_U(unit,
                        "rxlane_map   0x%04x\n"),
             pCfg->rxlane_map[0]));
    LOG_CLI((BSL_META_U(unit,
                        "txlane_map   0x%04x\n"),
             pCfg->txlane_map[0]));
    LOG_CLI((BSL_META_U(unit,
                        "rxpol        0x%04x\n"),
             pCfg->rxpol));
    LOG_CLI((BSL_META_U(unit,
                        "txpol        0x%04x\n"),
             pCfg->txpol));
    LOG_CLI((BSL_META_U(unit,
                        "cl73an       0x%04x\n"),
             pCfg->cl73an));
    LOG_CLI((BSL_META_U(unit,
                        "cl37an       0x%04x\n"),
             pCfg->cl37an));
    LOG_CLI((BSL_META_U(unit,
                        "ability_mask 0x%04x\n"),
             pCfg->ability_mask));
    LOG_CLI((BSL_META_U(unit,
                        "an_cl72      0x%04x\n"),
             pCfg->an_cl72));
    LOG_CLI((BSL_META_U(unit,
                        "forced_init_cl72  0x%04x\n"),
             pCfg->forced_init_cl72));
    LOG_CLI((BSL_META_U(unit,
                        "sw_init_drive     0x%04x\n"),
             pCfg->sw_init_drive));
    LOG_CLI((BSL_META_U(unit,
                        "phy_mode     0x%04x\n"),
             pc->phy_mode));
    LOG_CLI((BSL_META_U(unit,
                        "hg_mode      0x%04x\n"),
             pCfg->hg_mode));
    LOG_CLI((BSL_META_U(unit,
                        "port_type    0x%04x\n"),
             tsc->port_type));
    LOG_CLI((BSL_META_U(unit,
                        "cx4_10g      0x%04x\n"),
             pCfg->cx4_10g));
    LOG_CLI((BSL_META_U(unit,
                        "lane0_rst    0x%04x\n"),
             pCfg->lane0_rst));
    LOG_CLI((BSL_META_U(unit,
                        "rxaui        0x%04x\n"),
             pCfg->rxaui));
    LOG_CLI((BSL_META_U(unit,
                        "dxgxs        0x%04x (tsc 0x%x)\n"),
             pCfg->dxgxs, tsc->dxgxs));
    LOG_CLI((BSL_META_U(unit,
                        "line_intf    0x%0x %s\n"),
             pCfg->line_intf, tscmod_line_intf_msg(pCfg->line_intf)));
    LOG_CLI((BSL_META_U(unit,
                        "chip_num     0x%04x\n"),
             pc->chip_num));
    LOG_CLI((BSL_META_U(unit,
                        "lane_num     0x%04x\n"),
             pc->lane_num));
    LOG_CLI((BSL_META_U(unit,
                        "even_odd     0x%04x\n"),
             tsc->lane_swap));
    LOG_CLI((BSL_META_U(unit,
                        "speedMax     0d%0d iintf=%0x %s\n"),
             pc->speed_max, tsc->iintf, tscmod_iintf_msg(tsc->iintf)));
    LOG_CLI((BSL_META_U(unit,
                        "speed        spd=%s(%0d)\n"),
             e2s_tscmod_spd_intfc_type[tsc->spd_intf], tsc->spd_intf));
    LOG_CLI((BSL_META_U(unit,
                        "ctrl_type    0x%08x E(%0d) W(%0d)\n"),
             tsc->ctrl_type,
             ((tsc->ctrl_type&TSCMOD_CTRL_TYPE_ERR)?1:0),
             ((tsc->ctrl_type&TSCMOD_CTRL_TYPE_MSG)?1:0)));
    LOG_CLI((BSL_META_U(unit,
                        "ERR MSG      0x%0x 0x%0x\n"),
             tsc->err_code, tsc->msg_code));
    LOG_CLI((BSL_META_U(unit,
                        "an_type      0x%04x %s hg=%0d\n"),
             tsc->an_type, e2s_tscmod_an_type[tsc->an_type], tsc->an_hg_vco));
    LOG_CLI((BSL_META_U(unit,
                        "verbosity    0x%04x\n"),
             tsc->verbosity));
    LOG_CLI((BSL_META_U(unit,
                        "pc->flags    0x%04x\n"),
             pc->flags));
    LOG_CLI((BSL_META_U(unit,
                        "pc->stop     0x%04x\n"),
             pc->stop));
    LOG_CLI((BSL_META_U(unit,
                        "pi->phy_flags   0x%04x\n"),
             pi->phy_flags));
    LOG_CLI((BSL_META_U(unit,
                        "force_cl72_st   0x%04x en=%0d md=%0d\n"), 
             FORCE_CL72_STATE(pc), FORCE_CL72_ENABLED(pc),
             FORCE_CL72_MODE(pc)));
    LOG_CLI((BSL_META_U(unit,
                        "war rxlos    0x%04x cnt=%0d en=%0x rf_dis=%0x\n"),
             DEV_CFG_PTR(pc)->sw_rx_los.state, 
             DEV_CFG_PTR(pc)->sw_rx_los.count,
             DEV_CFG_PTR(pc)->sw_rx_los.enable,
             DEV_CFG_PTR(pc)->sw_rx_los.rf_dis));
    LOG_CLI((BSL_META_U(unit,
                        "war an       0x%04x tick=%0d m=%0x\n"),
             TSCMOD_AN_STATE(pc), TSCMOD_AN_TICK_CNT(pc),
             TSCMOD_AN_MODE(pc)));
    return SOC_E_NONE;
}


typedef struct {
    int tx_pre_cursor;
    int tx_main;
    int tx_post_cursor;
    char *vga_bias_reduced;
    int postc_metric;
    char pmd_mode_s[100];
    char fw_mode_s[32] ;
    int pf_ctrl;
    int vga_sum;
    int dfe1_bin;
    int dfe2_bin;
    int dfe3_bin;
    int dfe4_bin;
    int dfe5_bin;
    int integ_reg;
    int integ_reg_xfer;
    int clk90_offset;
   /* int clkp1_offset; */
    int sd;
   /* int lopf; */
    int p1_lvl;
    int m1_lvl;
   /* int sl_tg ; */
    int tx_drvr;
    int slicer_target;
    int offset_pe;
    int offset_ze;
    int offset_me;
    int offset_po;
    int offset_zo;
    int offset_mo;
    int dsc_sm;
} TSC_UC_DESC;

static TSC_UC_DESC uc_desc[MAX_NUM_LANES];

int phy_tscmod_uc_status_dump(int unit, soc_port_t port)
{
   int rv ;
   tscmod_sema_lock(unit, port, FUNCTION_NAME()) ;
   rv = tscmod_uc_status_dump(unit, port) ;
   tscmod_sema_unlock(unit, port) ;

   return rv ;
}

int
tscmod_uc_status_dump(int unit, soc_port_t port)
{

    phy_ctrl_t        *pc;
    TSCMOD_DEV_DESC_t *portDesc;
    TSC_UC_DESC       *pDesc;
    TSCMOD_DEV_CFG_t  *pCfg;
    TSCMOD_DEV_INFO_t  *pInfo;

    tscmod_st  *tsc[6];      /* tscmod structure    */
    int indx, num_core, core_num = 0, size;
    int tmp_lane_select = 0 ;
    uint16 data16, tmpLane, tmp_dxgxs, tempData, timeout;
    /* int p1_threshold[] ={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,24,26,28,30,32,34,36,38,40}; */
    int p1_mapped[] = {0,1,3,2,4,5,7,6,8,9,11,10,12,13,15,14,16,17,19,18,20,21,23,22,24,25,27,26,28,29,31,30};

    uint16 mask16;   int dsc_sm_s[24] ;
    int regval, rv, osr_mode, pmd_mode = 0, fw_mode = 0;
    int br_cdr_en, os_dfe_en ;
    int ext_det, afe_det, sig_ok, pmd_lck, temper, temper_idx, temper_val;
    int pmd_lk_lh, pmd_lk_ll, ext_sig_det_lh, ext_sig_det_ll, afe_det_lh, afe_det_ll, ok_lh, ok_ll ;

    char chip_v[TSC40_VER_NAME_LEN] ;

    pc = INT_PHY_SW_STATE(unit, port);

    portDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    pCfg  = &portDesc->cfg;
    pInfo = &portDesc->info;

    tsc[0]   = (tscmod_st *)(portDesc + 1);

    num_core = (SOC_INFO(unit).port_num_lanes[pc->port] + 3) / 4;
    size = num_core * NUM_LANES;
    rv   = SOC_E_NONE ;

    if(size > MAX_NUM_LANES) {
       printf("%-22s: p=%0d lane size error size=%0d max=%0d\n",
              FUNCTION_NAME(), tsc[0]->port, size, MAX_NUM_LANES) ;
       return SOC_E_ERROR ;
    }

    SOC_IF_ERROR_RETURN  /* f014 */
         (READ_WC40_UC_INFO_B1_TEMPERATUREr(tsc[0]->unit, tsc[0], &data16));
    temper_val = (data16 & UC_INFO_B1_TEMPERATURE_TEMPERATURE_MASK)>> UC_INFO_B1_TEMPERATURE_TEMPERATURE_SHIFT;
    temper_idx = (data16 & UC_INFO_B1_TEMPERATURE_TEMP_IDX_MASK) >> UC_INFO_B1_TEMPERATURE_TEMP_IDX_SHIFT;
    temper = 406 - (temper_val * 538)/1000 ;  /* close to round(407 - val * 538/1000) */

    for(indx=0;indx<TSC40_VER_NAME_LEN;indx++) chip_v[indx] = '\0';
    chip_v[0] = 'A' + ((pInfo->serdes_id0 >>
                              MAIN0_SERDESID_REV_LETTER_SHIFT) & 0x3);
    chip_v[1] = '0' + ((pInfo->serdes_id0 >>
                              MAIN0_SERDESID_REV_NUMBER_SHIFT) & 0x7);

    printf("\nTSC-%s uC ver=%x crc=%x T=%0d T_idx=%0d %s 0x%0x 0x%0x 0x%0x 0x%0x\n\n", chip_v, pCfg->uc_ver, pCfg->uc_crc, temper, temper_idx,
           tscmod_version_msg(), pCfg->cidV, pCfg->cidU, pCfg->cidH, pCfg->cidL) ;

    for (indx = 0; indx < size; indx++) {
       core_num = indx / 4;

       tmpLane         = tsc[core_num]->this_lane; /* save it to restore later */
       tmp_lane_select = tsc[core_num]->lane_select ;
       tmp_dxgxs       = tsc[core_num]->dxgxs ;

       tsc[core_num]->dxgxs     = 0 ;
       tsc[core_num]->this_lane = indx % 4;
        switch (indx % 4) {
        case 1:
            tsc[core_num]->lane_select = TSCMOD_LANE_0_0_1_0;
            break;
        case 2:
            tsc[core_num]->lane_select = TSCMOD_LANE_0_1_0_0;
            break;
        case 3:
            tsc[core_num]->lane_select = TSCMOD_LANE_1_0_0_0;
            break;
        default:
            tsc[core_num]->lane_select = TSCMOD_LANE_0_0_0_1;
            break ;
        }

        SOC_IF_ERROR_RETURN                   /* c150 */
           (READ_WC40_RX_X4_STATUS1_PMA_PMD_LIVE_STATUSr(tsc[core_num]->unit, tsc[core_num], &data16));

        ext_det = (data16&RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_EXTERNAL_SIGNAL_DETECT_MASK)
           >> RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_EXTERNAL_SIGNAL_DETECT_SHIFT;
        afe_det =(data16 & RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_AFE_SIGNAL_DETECT_MASK)
           >> RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_AFE_SIGNAL_DETECT_SHIFT;
        sig_ok  =(data16 & RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_SIGNAL_OK_MASK)
           >> RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_SIGNAL_OK_SHIFT ;
        pmd_lck =(data16 & RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_PMD_LOCK_MASK)
           >>RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_PMD_LOCK_SHIFT ;

        SOC_IF_ERROR_RETURN                  /* c151 */
           (READ_WC40_RX_X4_STATUS1_PMA_PMD_LATCHED_STATUSr(tsc[core_num]->unit, tsc[core_num], &data16));

        pmd_lk_lh = (data16&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_PMD_LOCK_LH_MASK)
                         >> RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_PMD_LOCK_LH_SHIFT ;
        pmd_lk_ll = (data16&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_PMD_LOCK_LL_MASK)
           >> RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_PMD_LOCK_LL_SHIFT ;
        ext_sig_det_lh =
           (data16&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_EXTERNAL_SIGNAL_DETECT_LH_MASK)
           >>RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_EXTERNAL_SIGNAL_DETECT_LH_SHIFT ;
        ext_sig_det_ll =
           (data16&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_EXTERNAL_SIGNAL_DETECT_LL_MASK)
           >>RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_EXTERNAL_SIGNAL_DETECT_LL_SHIFT ;
        afe_det_lh     =
           (data16&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_AFE_SIGNAL_DETECT_LH_MASK)
           >>RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_AFE_SIGNAL_DETECT_LH_SHIFT ;
        afe_det_ll     =
           (data16&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_AFE_SIGNAL_DETECT_LL_MASK)
           >>RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_AFE_SIGNAL_DETECT_LL_SHIFT ;
        ok_lh          =
           (data16&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_SIGNAL_OK_LH_MASK)
           >>RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_SIGNAL_OK_LH_SHIFT ;
        ok_ll          =
           (data16&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_SIGNAL_OK_LL_MASK)
           >>RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_SIGNAL_OK_LL_SHIFT ;

        printf("  L%0d ext_det=%0d (lh=%0d ll=%0d)  afe_det=%0d (lh=%0d ll=%0d)  sig_ok=%0d (lh=%0d ll=%0d)  pmd_lck=%0d (lh=%0d ll=%0d) \n",
               tsc[core_num]->this_lane, ext_det, ext_sig_det_lh,ext_sig_det_ll, afe_det, afe_det_lh,afe_det_ll,
               sig_ok, ok_lh, ok_ll, pmd_lck,pmd_lk_lh, pmd_lk_ll );

        /* restore this lane info */
       tsc[core_num]->this_lane   = tmpLane;
       tsc[core_num]->lane_select = tmp_lane_select ;
       tsc[core_num]->dxgxs       = tmp_dxgxs ;
    }

    printf("\n") ;
    /*

    for (i = 0 ; i < num_core; i++) {
        tsc[i]   =  (tscmod_st *)( tsc[0] + i);
        temp_id[i] = tsc[i]->phy_ad;
    }
    */
    TSCMOD_MEM_SET((char *)&uc_desc[0], 0, (sizeof(TSC_UC_DESC))*size);
    for (indx = 0; indx < size; indx++) {
        pDesc = &uc_desc[indx];
        core_num = indx / 4;

        tmpLane        = tsc[core_num]->this_lane; /* save it to restore later */
        tmp_lane_select = tsc[core_num]->lane_select ;
        tmp_dxgxs       = tsc[core_num]->dxgxs ;

        tsc[core_num]->dxgxs     = 0 ;
        tsc[core_num]->this_lane = indx % 4;
        switch (indx % 4) {
        case 1:
            tsc[core_num]->lane_select = TSCMOD_LANE_0_0_1_0;
            break;
        case 2:
            tsc[core_num]->lane_select = TSCMOD_LANE_0_1_0_0;
            break;
        case 3:
            tsc[core_num]->lane_select = TSCMOD_LANE_1_0_0_0;
            break;
        default:
            tsc[core_num]->lane_select = TSCMOD_LANE_0_0_0_1;
            break ;
        }

        /* first check if diagnostic_enable set or not @0xc20d */
        SOC_IF_ERROR_RETURN
           (READ_WC40_DSC1B0_DSC_DIAG_CTRL0r(tsc[core_num]->unit, tsc[core_num], &data16));
        if ((data16 & DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK) == 0) {
            /* first make sure that we pause the uc */
            /* check if uc is active or not */
            if (tsc[core_num]->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)  {
                /* wait for cmd to be read by micro. i.e it is ready to take next cmd */
                timeout = 0;
                do {
                   SOC_IF_ERROR_RETURN    /* 0xc20e */
                      (READ_WC40_DSC1B0_UC_CTRLr(tsc[core_num]->unit, tsc[core_num], &data16));
                } while (((data16 & DSC1B0_UC_CTRL_READY_FOR_CMD_MASK)==0)
                        && (++timeout <2000));

                if(timeout>=20000) {
                   printf("DSC dump : uController l=%0d timeOut frozen 0.9 !: u=%d rv=%0d data=%x\n",
                          indx, tsc[core_num]->unit, rv, data16);
                }

                /* check for error */
                SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(tsc[core_num]->unit, tsc[core_num], &data16));
                if (data16 & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) {
                   printf("%-22s: uCode l=%0d reported error\n", FUNCTION_NAME(), indx);
                }
                /* next issue command to stop Uc gracefully */
                SOC_IF_ERROR_RETURN   /* 0xc20e */
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(tsc[core_num]->unit, tsc[core_num], 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(tsc[core_num]->unit, tsc[core_num], 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(tsc[core_num]->unit, tsc[core_num], 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

                data16 = (0x1 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) |
                         (0x0 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
                mask16  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
                           DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK);
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(tsc[core_num]->unit, tsc[core_num], data16, mask16));

                sal_usleep(1000);

                /* wait for uC ready for command:  bit7=1    */
                rv = tscmod_regbit_set_wait_check(tsc[core_num], TSC40_DSC1B0_UC_CTRLr,
                         DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, TSC40_PLL_WAIT*1000);

                data16 = tsc[core_num]->accData;
                tsc[core_num]->accData = ((tsc[core_num]->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

                if ((rv <0) || (tsc[core_num]->accData != 0)) {
                   printf("DSC dump : uController l=%0d timeOut frozen 1.0 !: u=%d rv=%0d data=%x\n",
                          indx, tsc[core_num]->unit, rv, data16);
                }
            }
        }

        /* for print out. discard read back */
        tsc[core_num]->per_lane_control = 0x10 ;
        tsc[core_num]->diag_type        = TSCMOD_DIAG_TX_AMPS;
        (tscmod_tier1_selector("TSCMOD_DIAG", tsc[core_num], &rv));

        /* TSCOP-001: lane swapped */
        tsc[core_num]->per_lane_control = 0xa ;
        tsc[core_num]->diag_type = TSCMOD_DIAG_TX_TAPS;
        tscmod_tier1_selector("TSCMOD_DIAG", tsc[core_num], &rv);

        data16 = tsc[core_num]->accData ;
        /* tx_pre_cursor */
        pDesc->tx_pre_cursor = (data16 & ANATX_ASTATUS0_TX_FIR_TAP_PRE_MASK)
                                      >> ANATX_ASTATUS0_TX_FIR_TAP_PRE_SHIFT;
        /* tx_main */
        pDesc->tx_main = (data16 & ANATX_ASTATUS0_TX_FIR_TAP_MAIN_MASK)
                                >> ANATX_ASTATUS0_TX_FIR_TAP_MAIN_SHIFT;
        /* tx_post_cursor */
        pDesc->tx_post_cursor = (data16 & ANATX_ASTATUS0_TX_FIR_TAP_POST_MASK)
                                     >>   ANATX_ASTATUS0_TX_FIR_TAP_POST_SHIFT;

        /* vga_bias_reduced */
        SOC_IF_ERROR_RETURN
            (READ_WC40_ANARX_RXACONTROL2r(unit, tsc[core_num], &data16));
        regval = data16 & ANARX_RXACONTROL2_IMIN_VGA_MASK;
        if (regval) {
            pDesc->vga_bias_reduced = "88%";
        } else {
            pDesc->vga_bias_reduced = "100%";
        }

        /* postc_metric(lane) = rd22_postc_metric (phy, lane), DSC_5_ADDR=0x8241 */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC5B0_TUNING_SM_STATUS1r(unit, tsc[core_num], &data16));
        regval = data16 & DSC5B0_TUNING_SM_STATUS1_POSTC_METRIC_MASK;
        mask16 = (DSC5B0_TUNING_SM_STATUS1_POSTC_METRIC_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->postc_metric = regval;

        /* read firmware PMD mode 0xc222 */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_CDR_STATUS2r(unit, tsc[core_num], &data16));
        osr_mode =             ((data16 & DSC3B0_CDR_STATUS2_OSCDR_MODE_MASK)
                                     >> DSC3B0_CDR_STATUS2_OSCDR_MODE_SHIFT);
        br_cdr_en = (data16 & DSC3B0_CDR_STATUS2_BR_CDR_ENABLED_MASK ) ? 1:0 ;
        SOC_IF_ERROR_RETURN
           (READ_WC40_DSC2B0_ACQ_SM_CTRL3r(unit, tsc[core_num], &data16));
        os_dfe_en = (data16 & DSC2B0_ACQ_SM_CTRL3_OS_DFE_EN_MASK ) ? 1:0 ;

        /* next get sigdet signal and tx_drvr */
        SOC_IF_ERROR_RETURN
           (READ_WC40_ANARX_ASTATUS1r(unit, tsc[core_num], &data16));   /* 0xc02b */
        pDesc->sd = (data16 & ANARX_ASTATUS1_SIGDET_MASK) >> ANARX_ASTATUS1_SIGDET_SHIFT ;

        SOC_IF_ERROR_RETURN
           (READ_WC40_ANATX_TX_DRIVERr(unit, tsc[core_num], &data16));   /* 0xc017 */
        pDesc->tx_drvr = data16;

        SOC_IF_ERROR_RETURN
           (READ_WC40_UCSS_X4_FIRMWARE_MODEr(unit, tsc[core_num], &data16));   /* 0xc260 */
        fw_mode = data16 & UCSS_X4_FIRMWARE_MODE_FIRMWARE_MODE_MASK ;

        if((br_cdr_en==0) && (os_dfe_en==0)) {
           pmd_mode = 1 ;
        } else if((br_cdr_en==1) && (os_dfe_en==0)) {
           pmd_mode = 4 ;
        } else if((br_cdr_en==0) && (os_dfe_en==1)) {
           pmd_mode = 3 ;
        }

        if (osr_mode == 0 ) {
            if (pmd_mode == 1) {
                strcpy(pDesc->pmd_mode_s, "OSx1(");
            } else if (pmd_mode == 2) {
               strcpy(pDesc->pmd_mode_s,  "BRx1(");
            } else if (pmd_mode == 3) {
                strcpy(pDesc->pmd_mode_s, "OSx1+DFE(");
            } else {
                strcpy(pDesc->pmd_mode_s, "BRx1+DFE(");
            }
        } else if (osr_mode == 1) {
            if (pmd_mode == 1) {
                strcpy(pDesc->pmd_mode_s, "OSx2(");
            } else if (pmd_mode == 2) {
               strcpy(pDesc->pmd_mode_s,  "BRx2(");
            } else if (pmd_mode == 3) {
                strcpy(pDesc->pmd_mode_s, "OSx2+DFE(");
            } else {
                strcpy(pDesc->pmd_mode_s, "BRx2+DFE(");
            }
        } else if (osr_mode == 2) {
            strcpy(pDesc->pmd_mode_s, "OSx4(");
        } else if (osr_mode == 3) {
            strcpy(pDesc->pmd_mode_s, "OSx5(");
        } else if (osr_mode == 6) {
            strcpy(pDesc->pmd_mode_s, "OSx8.2(");
        } else if (osr_mode == 7) {
            strcpy(pDesc->pmd_mode_s, "OSx3.3(");
        } else if (osr_mode == 8) {
            strcpy(pDesc->pmd_mode_s, "OSx10(");
        } else if (osr_mode == 9) {
            strcpy(pDesc->pmd_mode_s, "OSx3(");
        } else if (osr_mode == 10) {
            strcpy(pDesc->pmd_mode_s, "OSx8(");
        }
        sprintf(pDesc->fw_mode_s, "%d)", fw_mode) ;
        strcat(pDesc->pmd_mode_s, pDesc->fw_mode_s) ;

        /* pf_ctrl(lane) = rd22_pf_ctrl (phy, lane) 0x822b */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS0r(unit, tsc[core_num], &data16));
        pDesc->pf_ctrl = data16 & DSC3B0_ANA_STATUS0_PF_CTRL_BIN_MASK;

        /* low pass filter(lane) = rd22_pf_ctrl (phy, lane) 0x821d
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC2B0_DSC_ANA_CTRL4r(unit, tsc[core_num], &data16));
            pDesc->lopf = (data16 & 0x700) >> 8;  */

        /* vga_sum(lane) = rd22_vga_sum (phy, lane) 0x8225  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS0r(unit, tsc[core_num], &data16));
        pDesc->vga_sum = data16 & DSC3B0_DFE_VGA_STATUS0_VGA_SUM_MASK;

        /* dfe1_bin(lane) = rd22_dfe_tap_1_bin (phy, lane) 0x8225  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS0r(unit, tsc[core_num], &data16));
        regval = (data16 & DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_MASK) >> DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_SHIFT;
        pDesc->dfe1_bin = regval;

        /* dfe2_bin(lane) = rd22_dfe_tap_2_bin (phy, lane) 0x8226  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS1r(unit, tsc[core_num], &data16));
        regval = data16 & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_2_BIN_MASK;
        mask16 = (DSC3B0_DFE_VGA_STATUS1_DFE_TAP_2_BIN_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->dfe2_bin = regval;

        /* dfe3_bin(lane) = rd22_dfe_tap_3_bin (phy, lane) 0x8226  */
        regval = (data16 & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_MASK) >> DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_SHIFT;
        mask16 = ((DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_MASK) >> (DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_SHIFT + 1)) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->dfe3_bin = regval;

        /* dfe4_bin(lane) = rd22_dfe_tap_4_bin (phy, lane) 0x8227  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS2r(unit, tsc[core_num], &data16));
        regval = data16 & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_4_BIN_MASK;
        mask16 = (DSC3B0_DFE_VGA_STATUS2_DFE_TAP_4_BIN_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->dfe4_bin = regval;

        /* dfe5_bin(lane) = rd22_dfe_tap_5_bin (phy, lane) 0x8227  */
        regval = (data16 & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_MASK) >> DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_SHIFT;
        mask16 = ((DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_MASK) >> (DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_SHIFT + 1)) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->dfe5_bin = regval;

        /* dsc ststus 0xc22a */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ACQ_SM_STATUS1r(unit, tsc[core_num], &data16));
        pDesc->dsc_sm = data16;

        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ACQ_SM_STATUS0r(unit, tsc[core_num], &data16));
        dsc_sm_s[indx] = data16 & DSC3B0_ACQ_SM_STATUS0_DSC_STATE_MASK;

        /* integ_reg(lane) = rd22_integ_reg (phy, lane) DSC_3_ADDR=0x8220  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_CDR_STATUS0r(unit, tsc[core_num], &data16));
        regval = data16 & DSC3B0_CDR_STATUS0_INTEG_REG_MASK;
        mask16 = (DSC3B0_CDR_STATUS0_INTEG_REG_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        regval /= 84;
        pDesc->integ_reg = regval;

        /* integ_reg_xfer(lane) = rd22_integ_reg_xfer (phy, lane)  0x8221  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_CDR_STATUS1r(unit, tsc[core_num], &data16));
        regval = data16 & DSC3B0_CDR_STATUS1_INTEG_REG_XFER_MASK;
        mask16 = (DSC3B0_CDR_STATUS1_INTEG_REG_XFER_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->integ_reg_xfer = regval;

        /* clkp1_offset(lane) = rd22_clkp1_phase_offset (phy, lane)  0x8223 */
        /* Ravi. Removed this mod. It is writing to a reserved bit */
        /* SOC_IF_ERROR_RETURN
           (MODIFY_WC40_DSC1B0_PI_CTRL0r(unit, tsc[core_num], 0x000, 0x400)); */

        /* clk90_offset(lane) = rd22_clk90_phase_offset (phy, lane)  0x8223 */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_PI_STATUS0r(unit, tsc[core_num], &data16));
        pDesc->clk90_offset = (data16 & DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_MASK) >> DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_SHIFT;

        /* Compute P1 ladder level  rd22_rx_p1_eyediag (phy, lane)   0x821c */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC2B0_DSC_ANA_CTRL3r(unit, tsc[core_num], &data16));
        tempData = (data16 & DSC2B0_DSC_ANA_CTRL3_RESERVED_FOR_ECO0_MASK) >> DSC2B0_DSC_ANA_CTRL3_RESERVED_FOR_ECO0_SHIFT;
        data16 = tempData;
        data16 &= 0x1f;
        data16 = p1_mapped[data16];
        /* pDesc->p1_lvl = p1_threshold[data16];
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC2B0_DSC_ANA_CTRL0r(unit, tsc[core_num], &data16));
        data16 = (data16 & DSC2B0_DSC_ANA_CTRL0_RESERVED_FOR_ECO0_MASK) >> DSC2B0_DSC_ANA_CTRL0_RESERVED_FOR_ECO0_SHIFT;
        pDesc->p1_lvl = ((pDesc->p1_lvl) * (150 + 100 * data16)) / 40;
        if ( tempData > 31)   pDesc->p1_lvl = -pDesc->p1_lvl;  */

        /* Compute M1 slicer level  rd22_rx_p1_eyediag (phy, lane)   0x821c */
        pDesc->m1_lvl = 0;
        pDesc->p1_lvl = 0;
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC2B0_DSC_ANA_CTRL3r(unit, tsc[core_num], &data16));
        tempData = (data16 & DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_MASK) >> DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_SHIFT;
        pDesc->p1_lvl = tempData *25 + 125;
        if (data16 & DSC2B0_DSC_ANA_CTRL3_FORCE_RX_M1_THRESH_ZERO_MASK) {
            tempData = (data16 & DSC2B0_DSC_ANA_CTRL3_RX_M1_THRESH_ZERO_MASK) >> DSC2B0_DSC_ANA_CTRL3_RX_M1_THRESH_ZERO_SHIFT;
            pDesc->m1_lvl =  pDesc->p1_lvl * (1 - tempData);
        }

        /* slicer_target(lane) = ((25*rd22_rx_thresh_sel (phy, lane)) + 125)   0x821c */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC2B0_DSC_ANA_CTRL3r(unit, tsc[core_num], &data16));
        regval = (data16 & DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_MASK) >> DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_SHIFT;
        pDesc->slicer_target = regval * 25 + 125;

        /* offset_pe(lane) = rd22_slicer_offset_pe (phy, lane)   0x822c*/
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS1r(unit, tsc[core_num],&data16));
        regval = data16 & DSC3B0_ANA_STATUS1_SLICER_OFFSET_PE_MASK;
        mask16 = (DSC3B0_ANA_STATUS1_SLICER_OFFSET_PE_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->offset_pe = regval;

        /* offset_ze(lane) = rd22_slicer_offset_ze (phy, lane)  0x822d  */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS2r(unit, tsc[core_num], &data16));
        regval = data16 & DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZE_MASK;
        mask16 = (DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZE_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->offset_ze = regval;

        /* offset_me(lane) = rd22_slicer_offset_me (phy, lane)  0x822e */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS3r(unit, tsc[core_num], &data16));
        regval = data16 & DSC3B0_ANA_STATUS3_SLICER_OFFSET_ME_MASK;
        mask16 = (DSC3B0_ANA_STATUS3_SLICER_OFFSET_ME_MASK >> 1) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->offset_me = regval;

        /* 0x822c  offset_po(lane) = rd22_slicer_offset_po (phy, lane)  0x822c */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS1r(unit, tsc[core_num], &data16));
        regval = (data16 & DSC3B0_ANA_STATUS1_SLICER_OFFSET_PO_MASK) >> DSC3B0_ANA_STATUS1_SLICER_OFFSET_PO_SHIFT;
        mask16 = (DSC3B0_ANA_STATUS1_SLICER_OFFSET_PO_MASK >> (DSC3B0_ANA_STATUS1_SLICER_OFFSET_PO_SHIFT + 1)) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->offset_po = regval;

        /*0x822d  offset_zo(lane) = rd22_slicer_offset_zo (phy, lane) 0x822d */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS2r(unit, tsc[core_num], &data16));
        regval = (data16 & DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZO_MASK) >> DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZO_SHIFT;
        mask16 = ((DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZO_MASK) >> (DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZO_SHIFT + 1)) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->offset_zo = regval;

        /*0x822e  offset_mo(lane) = rd22_slicer_offset_mo (phy, lane)  0x822e */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_ANA_STATUS3r(unit, tsc[core_num], &data16));
        regval = (data16 & DSC3B0_ANA_STATUS3_SLICER_OFFSET_MO_MASK) >> DSC3B0_ANA_STATUS3_SLICER_OFFSET_MO_SHIFT;
        mask16 = ((DSC3B0_ANA_STATUS3_SLICER_OFFSET_MO_MASK) >> (DSC3B0_ANA_STATUS3_SLICER_OFFSET_MO_SHIFT + 1)) + 1;
        if (regval >= mask16) {
            regval -= mask16 << 1;
        }
        pDesc->offset_mo = regval;


        /* next resume uc */
        /* first check if diagnostic_enable set or not */
        SOC_IF_ERROR_RETURN
           (READ_WC40_DSC1B0_DSC_DIAG_CTRL0r(tsc[core_num]->unit, tsc[core_num], &data16));
        if ((data16 & DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK) == 0) {
           /* check if uc is active or not */
            if (tsc[core_num]->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)  {
                /* wait for cmd to be read by micro. i.e it is ready to take next cmd */
                timeout = 0;
                do {
                    SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(tsc[core_num]->unit, tsc[core_num], &data16));
                } while (((data16 & DSC1B0_UC_CTRL_READY_FOR_CMD_MASK)==0)
                        && (++timeout <2000));

                if(timeout>=20000) {
                   printf("DSC dump : uController l=%0d timeOut frozen 1.9 !: u=%d rv=%0d data=%x\n",
                          indx, tsc[core_num]->unit, rv, data16);
                }


                /* check for error */
                SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(tsc[core_num]->unit, tsc[core_num], &data16));
                if (data16 & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) {
                    printf("%-22s: uCode reported error\n", FUNCTION_NAME());
                }
                /* next issue command to resume Uc gracefully */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(tsc[core_num]->unit, tsc[core_num], 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(tsc[core_num]->unit, tsc[core_num], 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(tsc[core_num]->unit, tsc[core_num], 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

                data16 = (0x1 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) |
                         (0x2 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
                mask16  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
                           DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK);
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(tsc[core_num]->unit, tsc[core_num], data16, mask16));

                sal_usleep(1000);

                /* wait for uC ready for command:  bit7=1 ; 0xc20e */
                rv = tscmod_regbit_set_wait_check(tsc[core_num], TSC40_DSC1B0_UC_CTRLr,
                         DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, TSC40_PLL_WAIT*1000);

                data16 = tsc[core_num]->accData;
                tsc[core_num]->accData = ((tsc[core_num]->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK)
                   >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

                if ((rv <0) || (tsc[core_num]->accData != 0)) {
                   printf("DSC dump : uController l=%0d not ready pass 2.0 !: u=%d timeout=%0d rv=%0d data=%x\n",
                          indx, tsc[core_num]->unit, timeout, rv, data16);
                }
            }
        }

        /* restore this lane info */
        tsc[core_num]->this_lane   = tmpLane;
        tsc[core_num]->lane_select = tmp_lane_select ;
        tsc[core_num]->dxgxs       = tmp_dxgxs ;
    }  /* end of for loop */

    /* 0x9018, bits [13:6].  */
    SOC_IF_ERROR_RETURN
       (READ_WC40_ANAPLL_ASTATUS1r(unit, tsc[core_num], &data16));
    data16 = (data16 & ANAPLL_ASTATUS1_PLL_RANGE_MASK) >> ANAPLL_ASTATUS1_PLL_RANGE_SHIFT ;

    /* display */
    LOG_CLI((BSL_META_U(unit,
                        "\n\nDSC parameters for port %d\n\n"), port));
    LOG_CLI((BSL_META_U(unit,
                        "PLL Range: %d(=0x%04x)\n\n"),data16, data16));

    LOG_CLI((BSL_META_U(unit,
                        "LN SD PMD(FW_MODE) PPM  PPM_XFR ck90_ofs | "
                        "PF   p1_lvl m1  |VGA BIAS DFE1 2   3   "
                        "|DFE4 5  TX_DRVR |PREC MAIN POSTC MTRC|  "
                        "PE   ZE  ME  PO  ZO  MO |DSC_SM|ST\n")));
    for (indx = 0; indx < size; indx++) {
        pDesc = &uc_desc[indx];
        LOG_CLI((BSL_META_U(unit,
                            "%02d %02d %-12s %04d %07d %09d| %04d  %4d %4d "
                            "|%02d  %4s %02d   %03d %03d |%03d %03d 0x%04x  "
                            "|%04d %04d %04d %+05d|%4d %4d %3d %3d %3d %3d "
                            "|0x%04x|%02x\n"),
                 indx,pDesc->sd,  pDesc->pmd_mode_s, pDesc->integ_reg,
                 pDesc->integ_reg_xfer,pDesc->clk90_offset,
                 pDesc->pf_ctrl, pDesc->p1_lvl, pDesc->m1_lvl,
                 pDesc->vga_sum,pDesc->vga_bias_reduced,
                 pDesc->dfe1_bin,pDesc->dfe2_bin,pDesc->dfe3_bin,
                 pDesc->dfe4_bin, pDesc->dfe5_bin,pDesc->tx_drvr,
                 pDesc->tx_pre_cursor,pDesc->tx_main,pDesc->tx_post_cursor,
                 pDesc->postc_metric,pDesc->offset_pe,
                 pDesc->offset_ze,pDesc->offset_me,
                 pDesc->offset_po,pDesc->offset_zo,pDesc->offset_mo,
                 pDesc->dsc_sm, dsc_sm_s[indx]));
    }

    return SOC_E_NONE;

}


/* per lane function  */
/* cmd= 0 ->stop gracefully, cmd=1 -> stop immdeidatly, 2 = resume */
STATIC int tscmod_uc_cmd_seq(int unit, soc_port_t port, int lane, int cmd)
{
   phy_ctrl_t  *pc;
   TSCMOD_DEV_DESC_t  *pDesc;
   tscmod_st    *tsc;

   uint16 data16 , mask16; int rv ;  int timeout ;
   int tmp_sel, tmp_lane, tmp_dxgxs ;

   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);

   rv = SOC_E_NONE ;
   tmp_sel  = tsc->lane_select ;
   tmp_lane = tsc->this_lane ;
   tmp_dxgxs= tsc->dxgxs ;

   tsc->dxgxs      = 0 ;
   tsc->this_lane  = lane ;
   tsc->lane_select= getLaneSelect(lane) ;

   if(tsc->verbosity&TSCMOD_DBG_UC)
      printf("uController l=%0d cmd=%0d u=%d p=%0d l=%0d this_l=%0d sel=%x called\n",
             lane, cmd, tsc->unit, tsc->port, lane, tmp_lane, tmp_sel);

   SOC_IF_ERROR_RETURN
      (READ_WC40_DSC1B0_DSC_DIAG_CTRL0r(tsc->unit, tsc, &data16));
   if ((data16 & DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK) == 0) {
      if (tsc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED)  {
         timeout = 0;
         do {
            SOC_IF_ERROR_RETURN    /* 0xc20e */
               (READ_WC40_DSC1B0_UC_CTRLr(tsc->unit, tsc, &data16));
         } while (((data16 & DSC1B0_UC_CTRL_READY_FOR_CMD_MASK)==0)
                  && (++timeout <2000));

         if(timeout>=20000) {
            printf("uController l=%0d cmd=%0d timeOut frozen 1.0 !: u=%d p=%0d rv=%0d data=%x\n",
                   lane, cmd, tsc->unit, tsc->port, rv, data16);
         }

         /* check for error */
         SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(tsc->unit, tsc, &data16));
         if (data16 & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) {
            printf("%-22s: uCode l=%0d reported error\n", FUNCTION_NAME(), lane);
         }
         /* next issue command to stop/resume Uc gracefully */
         SOC_IF_ERROR_RETURN   /* 0xc20e */
            (MODIFY_WC40_DSC1B0_UC_CTRLr(tsc->unit, tsc, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(tsc->unit, tsc, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(tsc->unit, tsc, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

         /* cmd= 0 ->stop gracefully, cmd=1 -> stop immdeidatly, 2 = resume */
         data16 = (0x1 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) |
                  (cmd << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
         mask16  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
                    DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK);
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(tsc->unit, tsc, data16, mask16));

         sal_usleep(1000);

         rv = tscmod_regbit_set_wait_check(tsc, TSC40_DSC1B0_UC_CTRLr,
                     DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, TSC40_PLL_WAIT*1000);

         data16 = tsc->accData;
         tsc->accData = ((tsc->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

         if ((rv <0) || (tsc->accData != 0)) {
            printf("uController l=%0d cmd=%0d timeOut frozen 2.0 !: u=%d p=%0d rv=%0d data=%x tl=%0d sel=%x\n",
                   lane, cmd, tsc->unit, tsc->port, rv, data16, tmp_lane, tmp_sel);
         }
      }
   }

   tsc->this_lane  = tmp_lane ;
   tsc->lane_select= tmp_sel  ;
   tsc->dxgxs      = tmp_dxgxs ;
   return rv ;
}


STATIC void tscmod_sema_lock(int unit, soc_port_t port, const char *str)
{
   phy_ctrl_t      *pc;
   tscmod_st       *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);

   if(tsc->verbosity&TSCMOD_DBG_LOCK)
      printf("sema_lock u=%d p=%0d %s\n", tsc->unit, tsc->port, str);

   if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_SEMA_ON) {
      if(tscmod_core_sema[unit][pc->chip_num] !=NULL) {
         if(sal_sem_take(tscmod_core_sema[unit][pc->chip_num], 1000000)<0){ ; /* 1 sec */
            printf("Warning: sema_time_out u=%d p=%0d %s\n", tsc->unit, tsc->port, str);
         }
      }
   }
   return ;
}

STATIC void tscmod_sema_unlock(int unit, soc_port_t port)
{
   phy_ctrl_t      *pc;
   tscmod_st       *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);

   if(tsc->verbosity&TSCMOD_DBG_LOCK) {
      printf("sema_unlock u=%d p=%0d\n", tsc->unit, tsc->port);
   }
   if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_SEMA_ON) {
      if(tscmod_core_sema[unit][pc->chip_num] !=NULL) {
         sal_sem_give(tscmod_core_sema[unit][pc->chip_num]) ;
      }
   }
   return ;
}

STATIC void tscmod_sema_create(int unit, soc_port_t port)
{
   phy_ctrl_t      *pc;
   tscmod_st       *tsc;
   TSCMOD_DEV_DESC_t     *pDesc;
   pc = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   =  (tscmod_st *)( pDesc + 1);

   if(tsc->ctrl_type&TSCMOD_CTRL_TYPE_SEMA_ON) {
      if(tscmod_core_sema[unit][pc->chip_num] ==NULL) {
         if(tsc->verbosity&TSCMOD_DBG_LOCK)
            printf("sema_created u=%d p=%0d\n", tsc->unit, tsc->port);
         tscmod_core_sema[unit][pc->chip_num] = sal_sem_create("tscmod_sema_lock", sal_sem_BINARY, 1);
      }
   }
   return ;
}


STATIC int
_phy_tscmod_control_vco_freq_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    tscmod_st    *tsc;
    TSCMOD_DEV_DESC_t     *pDesc;
    uint16 data, divider;

    pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
    tsc    =  (tscmod_st *)( pDesc + 1);

    SOC_IF_ERROR_RETURN(READ_WC40_MAIN0_SETUPr(unit, tsc, &data));
    switch( ( data & MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_MASK ) >> MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_SHIFT ){
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div32: divider  = 32;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div36: divider  = 36;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div40: divider  = 40;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div42: divider  = 42;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div48: divider  = 48;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div50: divider  = 50;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div52: divider  = 52;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div54: divider  = 54;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div60: divider  = 60;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div64: divider  = 64;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div66: divider  = 66;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div68: divider  = 68;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div70: divider  = 70;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div80: divider  = 80;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div92: divider  = 92;  break;
        case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div100: divider = 100; break;
        default: return SOC_E_CONFIG;
    } 
    *value = tsc->refclk * divider;
    return(SOC_E_NONE);
}


/********   Eye Margin Diagnostic  **********/

#ifndef __KERNEL__
#include <math.h>
/* DEBUG function: manually construct a set of test data to test
 * extrapolation algorithem.
 * type: 0 vertical eye, 1 horizontal right eye, 2 horizontal left eye
 * max_offset: the slicer max offset, for example 23
 * the clock rate is hardcoded to 10312500Khz
 */

int
tscmod_eye_margin(int unit, soc_port_t port)
{
   phy_ctrl_t      *pc;
   tscmod_st            *tsc;      /*tscmod structure */
   TSCMOD_DEV_DESC_t    *pDesc;
   int rv;

   pc    = INT_PHY_SW_STATE(unit, port);
   pDesc = (TSCMOD_DEV_DESC_t *)(pc + 1);
   tsc   = (tscmod_st *)( pDesc + 1);
   rv    = SOC_E_NONE ;

   tsc->diag_type = TSCMOD_DIAG_EYE;
   (tscmod_tier1_selector("TSCMOD_DIAG", tsc, &rv));
   return rv;
}
#else   /* #ifndef __KERNEL__ */

int
tscmod_eye_margin(int unit, soc_port_t port, int type)
{
    LOG_CLI((BSL_META_U(unit,
                        "\nThis function is not supported in Linux kernel mode\n")));
    return SOC_E_NONE;
}
#endif /* #ifndef __KERNEL__ */

#else /* INCLUDE_XGXS_TSCMOD */
typedef int _xgxs_tscmod_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_XGXS_TSCMOD */
