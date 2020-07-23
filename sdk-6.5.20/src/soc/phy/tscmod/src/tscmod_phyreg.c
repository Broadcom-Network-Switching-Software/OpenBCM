/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * Copyright: (c) 2012 Broadcom Corp.
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : tscmod_phyreg.c
 * Description: Implements register access functions for TSCMod
 *---------------------------------------------------------------------*/
#include <shared/bsl.h>
#include "tscmod_main.h"
#include "tscmod_phyreg.h"

#if defined (_SDK_TSCMOD_)
#include <soc/util.h>
#include <soc/cmic.h>
#include <soc/drv.h>
#endif


#ifdef _SDK_TSCMOD_
/* INCLUDE_XGXS_TSCMOD defined in phydefs.h  in SDK */
#include "phydefs.h"
#endif

#ifdef INCLUDE_XGXS_TSCMOD

/*!
\brief TSCMod clause 45 read driver.

\param pc

\param addr

\param data

\returns SOC_E_ERROR (not implemented)

\details  Not yet impelemented.

*/

/*
#define TSCMOD_PCB_DEBUG 1
*/

/* forward decl */
int tscmod_reg_aer_write_core(uint32 unit, tscmod_st *pc, uint32 a, uint16 d) ;
int tscmod_reg_aer_modify_core(uint32 unit, tscmod_st *pc,uint32 a,uint16 d,uint16 m) ;

#if defined (TSCMOD_PCB_DEBUG)
int tscmod_pcb_status_check(uint32 unit, tscmod_st *pc, uint32 a, uint16 d, uint16 m, int rd) ;
#endif

int tscmod_cl45_read(tscmod_st *pc,uint32 addr,uint16 *data)
{
  int rv=0;
#if defined (_DV_TRIDENT2)
  int tmpData;
#endif
#if defined  (_SDK_TSCMOD_)
  rv = soc_esw_miimc45_read(pc->unit, pc->phy_ad, addr, data);
#endif
#if defined (_MDK_TSCMOD_)
  printf("%-22s Not implemented function\n",FUNCTION_NAME());
  rv = SOC_E_ERROR;
#endif
#if defined (STANDALONE)
  printf("%-22s Not implemented function\n",FUNCTION_NAME());
  rv = SOC_E_ERROR;
#endif
#if defined (_DV_TRIDENT2)
  dv_reg_cl45_read(pc->unit,pc->port, pc->prt_ad, pc->this_lane,addr,&tmpData);
  *data = (0xffff & tmpData);
  rv = SOC_E_NONE;
#endif

  if (pc->verbosity & TSCMOD_DBG_MDIO)
    printf("%-22s: unit:%d port:%d phy_ad:%d addr:0X%08x data:0X%08x\n",
                                  FUNCTION_NAME(),pc->unit,pc->port,pc->phy_ad,addr,*data);
  return rv;
}

/*!
\brief TSCMod clause 22 read driver.

\param pc

\param addr

\param data

\returns SOC_E_NONE if no errors. SOC_E_ERROR else.

\details
This is the lowest level driver. It resolves platform level dependencies and
connects TSCMod to the appropriate system-platform register access drivers.
*/

int tscmod_cl22_read(tscmod_st *pc,uint32 addr,uint16 *data)
{
  int rv=0;
#if defined (_DV_TRIDENT2)
  int tmpData;
#endif
  int phy_ad = 0;
#if defined  (_SDK_TSCMOD_)
  rv = soc_esw_miim_read(pc->unit, pc->phy_ad, addr, data);
#endif
#if defined (_MDK_TSCMOD_)
  uint32_t tmpData;
  int phy_ad = getLane0Phyad(pc->phy_ad);
  rv = cdk_xgs_miim_read(pc->unit, phy_ad, addr, &tmpData);
  *data = (0xffff & tmpData);
  return rv;
#endif
#if defined (STANDALONE)
  printf("%-22s Not implemented function\n",FUNCTION_NAME());
  rv = SOC_E_NONE;
#endif
#if defined (_DV_TRIDENT2)
  phy_ad = pc->phy_ad; /* for printing */
  dv_reg_cl22_read(pc->unit,pc->port, pc->prt_ad, pc->this_lane,addr,&tmpData);
  *data = (0xffff & tmpData);
  rv = SOC_E_NONE;
#endif
  if (pc->verbosity & TSCMOD_DBG_MDIO)
    printf("%-22s: unit:%d port:%d phy_ad:%d addr:0x%08x data:0x%04x\n",
                                  FUNCTION_NAME(),pc->unit,pc->port,phy_ad,addr,*data);
  return rv;
}

/*!
\brief TSCMod to_tsc read driver.

\param pc

\param addr

\param data

\returns SOC_E_NONE if no errors. SOC_E_ERROR else.

\details
This is the lowest level driver. It resolves platform level dependencies and
connects TSCMod to the appropriate system-platform register access drivers.
*/

int tscmod_totsc_read(tscmod_st *pc,uint32 addr,uint16 *data)
{
   int rv ; uint32 raddr ;
   raddr = (pc->phy_ad & 0x1f) ;
   raddr = (raddr << 19) | (addr & 0xff00ffff) ;
   /* call sbub command read */
   if (pc->verbosity & TSCMOD_DBG_MDIO)
      printf("%-22s: unit:%d port:%d phy_ad:%d addr:0x%08x data:0x%04x radd=%x\n",
             FUNCTION_NAME(),pc->unit,pc->port, pc->phy_ad,addr,*data, raddr);
   rv = SOC_E_NONE;
   return rv;
}

/*!
\brief TSCMod clause 45 write driver.

\param pc

\param addr

\param data

\returns SOC_E_ERROR.  Not yet implemented.

\details Not yet implemented.
*/
int tscmod_cl45_write(tscmod_st *pc,uint32 addr,uint16 data)
{
  int rv;
  if (pc->verbosity &TSCMOD_DBG_MDIO)
    printf("%-22s: unit:%d port:%d phy_ad:%d addr:0X%08x data:0X%08x\n",
			      FUNCTION_NAME(),pc->unit,pc->port,pc->phy_ad,addr,data);
#if defined (_SDK_TSCMOD_)
  soc_esw_miimc45_write(pc->unit, pc->phy_ad, addr, data);
  rv =  SOC_E_NONE;
#endif
#if defined (_MDK_TSCMOD_)
  cdk_xgs_miim_write(pc->unit, phy_ad, addr, data);
  rv =  SOC_E_NONE;
#endif
#if defined (STANDALONE)
  printf("%-22s Not implemented function\n",FUNCTION_NAME());
  rv =  SOC_E_ERROR;
#endif
#if defined (_TD2_DV)
  dv_reg_cl45_write(pc->unit, pc->port, pc->prt_ad, pc->this_lane, addr, data);
  rv = SOC_E_NONE;
#endif
  rv = SOC_E_NONE;
  return rv;
}

/*!
\brief TSCMod clause 22 write driver.

\param pc

\param addr

\param data

\returns SOC_E_NONE if no errors, SOC_E_ERROR else.

\details
This is the lowest level driver. It resolves platform level dependencies and
connects TSCMod to the appropriate system-platform register access drivers.
*/
int tscmod_cl22_write(tscmod_st *pc,uint32 addr,uint16 data)
{
  int rv;
  if (pc->verbosity & TSCMOD_DBG_MDIO)
    printf("%-22s: unit:%d port:%d phy_ad:%d addr:0x%08x data:0x%04x\n",
			       FUNCTION_NAME(),pc->unit,pc->port,pc->phy_ad,addr,data);
#if defined (_SDK_TSCMOD_)
  soc_esw_miim_write(pc->unit, pc->phy_ad, addr, data);
  rv =  SOC_E_NONE;
#endif
#if defined (_MDK_TSCMOD_)
  int phy_ad = getLane0Phyad(pc->phy_ad);
  cdk_xgs_miim_write(pc->unit, phy_ad, addr, data);
  rv =  SOC_E_NONE;
#endif

#if defined (STANDALONE)
  printf("%-22s Not implemented function\n",FUNCTION_NAME());
  rv =  SOC_E_ERROR;
#endif
#if defined (_DV_TRIDENT2)
  dv_reg_cl22_write(pc->unit, pc->port, pc->prt_ad, pc->this_lane, addr, data);
  rv = SOC_E_NONE;
#endif
  rv = SOC_E_NONE;
  return rv;
}


/*!
\brief TSCMod to_tsc write driver.

\param pc

\param addr

\param data

\returns SOC_E_NONE if no errors, SOC_E_ERROR else.

\details
This is the lowest level driver. It resolves platform level dependencies and
connects TSCMod to the appropriate system-platform register access drivers.
*/
int tscmod_totsc_write(tscmod_st *pc,uint32 addr,uint16 data)
{
   int rv ; uint32 raddr ;
   raddr = (pc->phy_ad & 0x1f) ;
   raddr = (raddr << 19) | (addr & 0xff00ffff) ;
   if (pc->verbosity & TSCMOD_DBG_MDIO)
    printf("%-22s: unit:%d port:%d phy_ad:%d addr:0x%08x data:0x%04x rad=%x\n",
           FUNCTION_NAME(),pc->unit,pc->port,pc->phy_ad,addr,data, raddr);
   /* call sbub command write */
   rv = SOC_E_NONE;
   return rv;
}



int tscmod_cl45_modify(tscmod_st *pc,uint32 addr,uint16 data, uint16 mask)
{
  int rv;
  uint16  tmpData, otmp;

  rv = tscmod_cl45_read(pc, addr, &tmpData);
  otmp     = tmpData;
  tmpData &= ~mask;
  tmpData |= data;

  if (otmp != tmpData) {
    rv = tscmod_cl45_write(pc, addr, tmpData);
  }
  return rv;
}




/*!
\brief spin on a register bit.

\param pc

\param reg

\param bit_num

\param bitset

\param The timeout input argument is the timeout limit in SDK.  In simualtions,
       the (register read) polling count which is timeout/1000 is used in tscmod_timeout_check.

\returns SOC_E_NONE if no errors, SOC_E_ERROR else.

\details
Waits for a reg. bit to be set. Spins until bit is set or time out. The time out
is programmable.
*/
int tscmod_regbit_set_wait_check(tscmod_st* pc,
                  int reg,              /* register to check */
                  int bit_num,          /* bit number to check */
                  int bitset,           /* check bit set or bit clear */
                  int timeout)          /* max wait time to check */
{
  uint16         data16;
  soc_timeout_t  to;
  int            rv;
  int            cond;

  /* override min_polls with iteration count for DV. */
#ifdef _SDK_TSCMOD_
  soc_timeout_init(&to, timeout, 0);
#else
  tscmod_timeout_init(&to, timeout, timeout/1000);
#endif

  do {
     rv = TSC_REG_READ(pc->unit, pc, 0,reg,&data16);
     pc->accData = data16;
     if (pc->verbosity & TSCMOD_DBG_TO)
        printf("%-22s: p=%0d reg:0x%08x bit_num:0x%08x bitset:%d d=%x\n",
               FUNCTION_NAME(), pc->port, reg, bit_num, bitset, data16);
     cond = (bitset  && (data16 & bit_num)) ||
            (!bitset && !(data16 & bit_num));
     if (cond || (rv < 0)) {
       break;
    }
#ifdef _SDK_TSCMOD_
  } while (!soc_timeout_check(&to));
#else
  } while (!tscmod_timeout_check(pc, &to));
#endif
  return (cond? SOC_E_NONE: SOC_E_TIMEOUT);
}

/*!
\brief Internal function to implement counter based timeout while polling for a
bit in a register.

\param to soc_timeout_t structure pointer

\param usec Microseconds to wait before timeout

\param min_polls Number of iterations before calling timeout.

\returns Nothing

\details
*/
void tscmod_timeout_init(soc_timeout_t *to, uint32 usec, int min_polls)
{
#if defined (_SDK_TSCMOD_) || defined(_MDK_TSCMOD_)
  /* Not yet implemented. This is used to keep runaway waits in check */
  printf("%-22s not implemented\n", FUNCTION_NAME());
#endif
#if defined (STANDALONE)
  to->min_polls = min_polls;
  to->usec = usec;
  to->polls = 1;
  to->exp_delay = 1;   /* In case caller sets min_polls < 0 */
#endif
#if defined (_DV_TRIDENT2)
  /* printf("%-22s usec=%0d min_polls=%0d\n", FUNCTION_NAME(), usec, min_polls); */
  to->min_polls = min_polls;
  to->usec = usec;
  to->polls = 1;
  to->exp_delay = 1;   /* In case caller sets min_polls < 0 */
#endif
}

/* to detect non tsc clock domain. return 0 if it is in tsc clock domain  */
int tscmod_reg_domain_test(tscmod_st *pc, uint32 a, int access_mode, int rpt_mode)
{
   int viol ; uint32 addr ;
   viol = 0 ;

   addr = a & 0xf800ffff;

   if((pc->ctrl_type & TSCMOD_CTRL_TYPE_FW_LOADED) && (!(pc->ctrl_type &TSCMOD_CTRL_TYPE_UC_STALL))) {

      if(pc->ctrl_type &TSCMOD_CTRL_TYPE_ALL_PROXY) {          /* SDK-46060 WAR */
         viol = 6 ;    /* all registers through proxy except few registers */
         if((addr & 0xf800fff0)==0xffc0) viol=0 ;
         else if(addr==0xc138) viol=0 ; /* RX_X4_CONTROL0_RX_SIGDET */
         /* else if(addr==0xffde) viol=0 ; */
         /* if(addr & 0xf8000000) viol =0 ; device id !=0 using proxy for now */
      } else if(pc->ctrl_type &TSCMOD_CTRL_TYPE_SOME_PROXY) {
         if     ((addr>=0xc200) && (addr< 0xc250)) viol = 1 ;
         else if((addr>=0xc010) && (addr< 0xc030)) viol = 2 ;
         else if((addr>=0x9010) && (addr< 0x9020)) viol = 3 ;
         else if( addr==TSC40_RX_X4_CONTROL0_PMA_CONTROL_0r ) {
            if(pc->ctrl_type &TSCMOD_CTRL_TYPE_UC_RXP) viol = 4;
         }
      }

      if(rpt_mode && viol) {
         switch(access_mode) {
         case TSCMOD_REG_WR: printf("Error: %-22s: p=%0d wr addr=%0x violation=%0d\n",
                        FUNCTION_NAME(), pc->port, addr, viol); break ;
         case TSCMOD_REG_MO: printf("Error: %-22s: p=%0d mo addr=%0x violation=%0d\n",
                        FUNCTION_NAME(), pc->port, addr, viol) ; break ;
         default:printf("Error: %-22s: p=%0d rd addr=%0x violation=%0d\n",
                        FUNCTION_NAME(), pc->port, addr, viol) ; break ;
         }
      }
      return viol ;
   } else {
      return 0 ;
   }
}

int tscmod_reg_set_porxy_blk_addr(uint32 unit, tscmod_st *pc) {

   if (pc->regacc_type == TSCMOD_MDIO_CL22) {
       CHK_RET_VAL_FUNC(tscmod_cl22_write(pc, 0x1f, TSCMOD_UC_SYNC_BLOCK));
       pc->blk_adr = TSCMOD_UC_SYNC_BLOCK;
   }
   return SOC_E_NONE ;
}

int tscmod_is_warm_boot(uint32 unit, tscmod_st *pc) {
   if ((SOC_WARM_BOOT(pc->unit)||SOC_IS_RELOADING(pc->unit))&&
       !(pc->ctrl_type&TSCMOD_CTRL_TYPE_WB_DIS)) {
      return TRUE ;
   } else {
      return FALSE ;
   }
}

/*!
\brief Tier1 API for register read.

\param core Core Number

\param pc Pointer to tscmod_st

\param a 32 bit address

\param phy_data 16 bit data pointer.

\returns  0 on success. Non-zero on fail.

\details
*/
int tscmod_reg_aer_read_core(uint32 unit, tscmod_st *pc, uint32 a, uint16 *phy_data)
{
   int    cl45Devid, cl45_ln_no, cl45_addr, cl45_uaddr, data;
  uint32 phy_reg_aer=0, phy_reg_blk=0,phy_reg_addr=0, addr;
  uint16 data16;   int rpt, access_mode ;

  data=0;     /* Temporary holder for phy_data */
  data16=0;
  rpt =1 ;
  access_mode = TSCMOD_REG_RD ;

  addr = a & 0xf800ffff;

  tscmod_reg_domain_test(pc, a, access_mode, rpt) ;

  if (pc->lane_select != TSCMOD_LANE_BCST) {
     addr |= ((pc->this_lane & 0x3)<<16);
  } else if (pc->dxgxs & 0x3) {
     if(pc->dxgxs & 0x2) { addr = (0x2 << 16) | addr; }
  }
  pc->accAddr = addr ;

  if (pc->regacc_type == TSCMOD_MDIO_PCB) {
#if defined (_DV_TRIDENT2)
    svSetScope(svGetScopeFromName("SerdesInitializer_pkg"));

    /* assume bcst writes AER has been setup */
    if ((pc->lane_select == TSCMOD_LANE_BCST)||
        (pc->dxgxs & 0x3)) {
       data = pc->this_lane ;
       dv_slam_write_reg((pc->unit + pc->this_lane), 0xffde, data);
    }

    data = 0 ;
    addr = a & 0xf800ffff;

    dv_slam_read_reg((pc->unit + pc->this_lane), addr, &data);
    *phy_data = (data & 0xffff);

    /* restore bcst setup */
    if((pc->lane_select == TSCMOD_LANE_BCST)||
       (pc->dxgxs & 0x3)) {
       if (pc->lane_select == TSCMOD_LANE_BCST) {
         data = 0x6 ;
       } else if ((pc->dxgxs & 0x3) == 0x1 ) {
          data = 0x4 ;
       } else if((pc->dxgxs & 0x3) ==0x2) {
          data = 0x5 ;
       }
       dv_slam_write_reg((pc->unit + pc->this_lane), 0xffde, data);
    }

    return SOC_E_NONE;
#endif
#if defined (_SDK_TSCMOD_) || defined(_MDK_TSCMOD_)
    printf("%s FATAL. No PCB routines in non-Verilog mode\n", FUNCTION_NAME());
    data = 0xdead;
    *phy_data = (data & 0xffff);
    return SOC_E_ERROR;
#endif
  } else if        (pc->regacc_type == TSCMOD_MDIO_CL45) {
    phy_reg_addr = PHY_AER_REG_ADDR_CL45_REGAD(addr);   /* 0000FFFF  */
    phy_reg_aer  = PHY_AER_REG_ADDR_AER(addr); 	 	     /*(FFFF0000 >> 16)*/
    cl45Devid    = PHY_AER_REG_ADDR_CL45_DEVID(addr);   /* F8000000 >> 27 */
    cl45_ln_no   = PHY_AER_REG_ADDR_LANE_NUM(addr);     /* 07FF0000 >> 16 */
    cl45_addr    = SOC_PHY_CLAUSE45_ADDR(0,0xffde);
    cl45_uaddr   = SOC_PHY_CLAUSE45_ADDR(cl45Devid,phy_reg_addr) ;
    if (cl45_ln_no) /* write the lane number to the AER only if nonzero */
      tscmod_cl45_write(pc,cl45_addr,cl45_ln_no);

    tscmod_cl45_read(pc,cl45_uaddr,&data16);

    if (cl45_ln_no) /* restore back the lane number to 0 */
      tscmod_cl45_write(pc,cl45_addr,0);

  } else if (pc->regacc_type == TSCMOD_MDIO_CL22) {
    phy_reg_aer  = PHY_AER_REG_ADDR_AER(addr);    /* upper 16 bits */
    phy_reg_blk  = PHY_AER_REG_ADDR_BLK(addr);    /* 12 bits mask=0xfff0 */
    phy_reg_addr = PHY_AER_REG_ADDR_REGAD(addr);  /* 5 bits {15,3,2,1,0} */

    if((pc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY)&&
       (pc->ctrl_type&TSCMOD_CTRL_TYPE_FW_LOADED)&&
       ((a & 0xf800fff0)==TSCMOD_UC_SYNC_BLOCK)){
       /* no need to set aer register */
    } else {
       CHK_RET_VAL_FUNC(tscmod_cl22_write(pc, 0x1f, 0xffd0));
       pc->blk_adr = 0xffd0 ;
       CHK_RET_VAL_FUNC(tscmod_cl22_write(pc, 0x1e, phy_reg_aer));
    }
    if((pc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR)&&
       (pc->ctrl_type&TSCMOD_CTRL_TYPE_FW_LOADED)&&
       ((a & 0xf800fff0)==TSCMOD_UC_SYNC_BLOCK)&&(pc->blk_adr==TSCMOD_UC_SYNC_BLOCK)){
       /* no need to set blk register */
    } else {
       CHK_RET_VAL_FUNC(tscmod_cl22_write(pc, 0x1f, phy_reg_blk)); /* Map block */
       pc->blk_adr = phy_reg_blk ;
    }
    CHK_RET_VAL_FUNC(tscmod_cl22_read(pc, phy_reg_addr, &data16)); /* Read reg */
  }
  if (pc->verbosity & TSCMOD_DBG_REG)
      printf("%-22s: u:%0d p:%0d phy_ad=%x Addr/Data:0x%x/0x%x\n",FUNCTION_NAME(),
             pc->unit, pc->port, pc->phy_ad, addr,data16);
  *phy_data = data16;
  return SOC_E_NONE;
}

#if defined (TSCMOD_PCB_DEBUG)
int tscmod_pcb_status_check(uint32 unit, tscmod_st *pc, uint32 a, uint16 d, uint16 m, int rd)
{
   int rv, cnt ;  uint16 rdata0, rdata, data ;  uint32 addr ;
   rv  = SOC_E_NONE ;
   cnt = 0 ;
   addr = pc->accAddr ;
   data = pc->accData ;

   if(pc->ctrl_type & TSCMOD_CTRL_TYPE_PCB_STALL1) return rv ;
   rv = tscmod_reg_aer_write_core(unit, pc, 0xffcc, 0x5404) ;
   cnt = 4 ; rdata = 0x10 ;
   while(cnt-->0) {
      rv = tscmod_reg_aer_read_core(unit, pc, 0xffc6, &rdata0) ;
      rdata &= rdata0 ;
   }
   if((rdata&0x10)||(rdata0&0x10)) {
      cnt=200 ; rdata = 0x10 ;
      while(cnt-->0) {
         rv = tscmod_reg_aer_read_core(unit, pc, 0xffc6, &rdata0) ;
         rdata &= rdata0 ;
         if((rdata&0x10)==0) break ;
      }
      if(rdata&0x10) {
         printf("Error: PCB stall_0 u=%0d p=%0d rd=%0d addr=%0x d=%0x m=%0x lane=%0d sel=%0x\n",
                pc->unit, pc->port, rd, a, d, m, pc->this_lane, pc->lane_select) ;
         if(pc->ctrl_type & TSCMOD_CTRL_TYPE_PCB_STALL0) pc->ctrl_type |= TSCMOD_CTRL_TYPE_PCB_STALL1 ;
         pc->ctrl_type |= TSCMOD_CTRL_TYPE_PCB_STALL0 ;
      } else {
         if (pc->ctrl_type & TSCMOD_CTRL_TYPE_PCB_STALL0) {
            pc->ctrl_type ^= TSCMOD_CTRL_TYPE_PCB_STALL0 ;
         }
      }
   } else {
      if (pc->ctrl_type & TSCMOD_CTRL_TYPE_PCB_STALL0) {
        pc->ctrl_type ^= TSCMOD_CTRL_TYPE_PCB_STALL0 ;
      }
   }
   pc->accAddr = addr ;
   pc->accData = data ;
   return rv ;
}
#endif

/* check for tscmod_uc_rx_sig_det */
int tscmod_reg_rx_sig_det(tscmod_st *pc, int core_wise, int forced_det_0, int *restore_val, int access_mode,
                                uint32 a, uint16 d, uint16 m)
{
   int rv, tmp_sel, tmp_lane, tmp_dxgxs, val, lane_s, lane_e, i ;  uint16 data, mask ;
   rv       = SOC_E_NONE ;
   tmp_sel  = pc->lane_select ;
   tmp_lane = pc->this_lane ;
   tmp_dxgxs= pc->dxgxs ;
   val      = 0 ;
   lane_s   = 0 ;
   lane_e   = 0 ;
   i        = 0 ;

   if(core_wise) {
      if(forced_det_0) {   /* create restore value under core_wise */
         for(i=0; i<4; i++) {
            pc->lane_select = getLaneSelect(i) ;
            pc->this_lane   = i ;
            pc->dxgxs       = 0 ;
            rv = tscmod_reg_aer_read_core(pc->unit, pc, 0xc138, &data) ;
            if(data & RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK) {
               val |= 1 << i ;
            }
         }
         *restore_val = val ;
      } else {             /* restore the original value under core_wise */
         for(i=0; i<4; i++) {
            pc->lane_select = getLaneSelect(i) ;
            pc->this_lane   = i ;
            val = *restore_val ;
            if(val & (1<<i)) {
               data = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK ;
            } else {
               data = 0 ;
            }
            mask = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK |
                   RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_VALUE_SIGNAL_OK_MASK ;
            /* 0xc138 */
            rv = tscmod_reg_aer_modify_core(pc->unit, pc, 0xc138, data, mask) ;
         }
      }
   }

   if(core_wise && forced_det_0) {
      pc->lane_select = TSCMOD_LANE_BCST ;
      pc->this_lane   = 0 ;
      lane_e          = 4 ;
   } else {
      switch(pc->port_type) {
      case TSCMOD_SINGLE_PORT:
         pc->lane_select = TSCMOD_LANE_BCST ;
         pc->this_lane   = 0 ;
         lane_e          = 4 ;
         break ;
      case TSCMOD_MULTI_PORT:
         pc->lane_select = getLaneSelect(tmp_lane) ;
         lane_s          = tmp_lane ;
         lane_e          = tmp_lane+1 ;
         break ;
      case TSCMOD_DXGXS:
         if(tmp_lane>=2) {  /* dual BCST */
            pc->dxgxs = 0x2 ;
            lane_s    = 2 ;
            lane_e    = 4 ;
         } else {
            pc->dxgxs = 0x1 ;
            lane_s    = 0 ;
            lane_e    = 2 ;
         }
         break ;
      default:
         break ;
      }
   }
   if(core_wise==0) {  /* port wise */
      if(forced_det_0) {
         /* create restore value */
         val = 0 ;
         for(i=0; i<4; i++) {
            if((i>=lane_s) &&(i<lane_e)) {
               pc->lane_select = getLaneSelect(i) ;
               pc->this_lane   = i ;
               pc->dxgxs       = 0 ;
               /*
                * COVERITY
                *
                * omit the return value from function call of reading register
                */
               /* coverity[returned_value] */
               rv = tscmod_reg_aer_read_core(pc->unit, pc, 0xc138, &data) ;
               if(data & RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK) {
                  val |= 1 << i ;
               }
            }
         }
         *restore_val = val ;

         switch(pc->port_type) {
         case TSCMOD_SINGLE_PORT:
            pc->lane_select = TSCMOD_LANE_BCST ;
            break ;
         case TSCMOD_DXGXS:
            if(tmp_lane>=2) {  /* dual BCST */
               pc->dxgxs = 0x2 ;
            } else {
               pc->dxgxs = 0x1 ;
            }
            break ;
         default:
            break ;
         }
         /* set sig det */
         data = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK ;
         mask = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK |
            RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_VALUE_SIGNAL_OK_MASK ;
         /* 0xc138 */
         rv = tscmod_reg_aer_modify_core(pc->unit, pc, 0xc138, data, mask) ;
      } else {
         /* port wise restore */
         for(i=0; i<4; i++) {
            if((i>=lane_s) &&(i<lane_e)) {
               pc->lane_select = getLaneSelect(i) ;
               pc->this_lane   = i ;
               val = *restore_val ;
               if(val & (1<<i)) {
                  data = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK ;
               } else {
                  data = 0 ;
               }
               mask = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK |
                  RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_VALUE_SIGNAL_OK_MASK ;
               /* 0xc138 */
               rv = tscmod_reg_aer_modify_core(pc->unit, pc, 0xc138, data, mask) ;
            }
         }
      }
   } else {
      /* core_wise */
      if(forced_det_0) {  /* set sig det */
         data = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK ;
         mask = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK |
            RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_VALUE_SIGNAL_OK_MASK ;
         /* 0xc138 */
         rv = tscmod_reg_aer_modify_core(pc->unit, pc, 0xc138, data, mask) ;
      }
   }

   pc->lane_select = tmp_sel ;
   pc->this_lane   = tmp_lane ;
   pc->dxgxs       = tmp_dxgxs ;

   if(forced_det_0==0) return rv ;

   if(core_wise) {
      if((pc->msg_code & TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_MASK)==TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_MASK) {

      } else {
         val = (pc->msg_code & TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_MASK)>>TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_SHIFT ;
         val = (((val<<1) | 1) <<TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_SHIFT)&TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_MASK  ;
         pc->msg_code |= val ;
         if(pc->verbosity&TSCMOD_DBG_PRINT) {
            printf("Warning: uC proxy core recovry attempt u=%0d p=%0d addr=%x access=%0d d=%x m=%x sel=0x%x l=%0d\n",
                   pc->unit, pc->port, a, access_mode, d, m, pc->lane_select, pc->this_lane) ;
         }
      }
   } else {
      if((pc->msg_code & TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_MASK)==TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_MASK) {

      } else {
         val = (pc->msg_code & TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_MASK)>>TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_SHIFT ;
         val = (((val<<1) | 1) <<TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_SHIFT)&TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_MASK ;
         pc->msg_code |= val ;
         if(pc->verbosity&TSCMOD_DBG_PRINT) {
            printf("Warning: uC proxy port recovry attempt u=%0d p=%0d addr=%x access=%0d d=%x m=%x sel=0x%x l=%0d\n",
                   pc->unit, pc->port, a, access_mode, d, m, pc->lane_select, pc->this_lane) ;
         }
      }
   }
   return rv ;
}

/* to report error */
int tscmod_reg_uc_sync_error_debug(uint32 unit, tscmod_st *pc)
{
   int rv ;  uint16 rdata0, rdata1, rdata2, rdata3 ; int tmp_sel, tmp_lane ;
   tmp_sel = pc->lane_select ;
   tmp_lane= pc->this_lane ;
   printf("Diagnosis: sync_cmd=") ;
   /* TSCMOD_UC_SYNC_CMDr has one copy per core. TSC623_SDK_42585_A1 */
   rv = tscmod_reg_aer_read_core(unit, pc, TSCMOD_UC_SYNC_CMDr, &rdata0) ;
   printf(" 0x%x", rdata0) ;
   rv = tscmod_reg_aer_read_core(unit, pc, TSCMOD_UC_SYNC_LADDr, &rdata0) ;
   rv = tscmod_reg_aer_read_core(unit, pc, TSCMOD_UC_SYNC_HADDr, &rdata1) ;
   printf(" Haddr=%0x Laddr=%0x", rdata1, rdata0) ;
   printf("\n") ;
   pc->lane_select = tmp_sel ;
   pc->this_lane   = tmp_lane ;

   rv = tscmod_reg_aer_write_core(unit, pc, 0xffcc, 0x3404) ;
   rv = tscmod_reg_aer_read_core(unit, pc, 0xffc6, &rdata0) ;
   rv = tscmod_reg_aer_read_core(unit, pc, 0xffc6, &rdata1) ;
   rv = tscmod_reg_aer_read_core(unit, pc, 0xffc6, &rdata2) ;
   rv = tscmod_reg_aer_read_core(unit, pc, 0xffc6, &rdata3) ;
   printf("Diagnosis: u=%0d p=%0d 0xffcc=0x3404 0xffc6=%x %x %x %x\n",
          pc->unit, pc->port, rdata0, rdata1, rdata2, rdata3) ;
   rv = tscmod_reg_aer_write_core(unit, pc, 0xffcc, 0x5404) ;
   rv = tscmod_reg_aer_read_core(unit, pc, 0xffc6, &rdata0) ;
   rv = tscmod_reg_aer_read_core(unit, pc, 0xffc6, &rdata1) ;
   rv = tscmod_reg_aer_read_core(unit, pc, 0xffc6, &rdata2) ;
   rv = tscmod_reg_aer_read_core(unit, pc, 0xffc6, &rdata3) ;
   printf("Diagnosis: u=%0d p=%0d 0xffcc=0x5404 0xffc6=%x %x %x %x\n",
          pc->unit, pc->port, rdata0, rdata1, rdata2, rdata3) ;

   return rv;
}

/* handle uc_sync read and write */
int tscmod_reg_uc_sync_cmd_core(uint32 unit, tscmod_st *pc, int access_mode,
                                uint32 a, uint16 *phy_data, uint16 d, uint16 m)
{
   int rv, loop, cnt, strike, restore_val1, restore_val2 ;  uint16 cmd, add, rdata ;
   uint32            start_time, diff_time ;

   if ((pc->verbosity & TSCMOD_DBG_REG)&&(access_mode==TSCMOD_REG_WR))
      printf("%-22s: u=%0d p=%0d addr=%x WR rd=%x wd=%x mask=%x l=%0d sel=%x\n",
             FUNCTION_NAME(), unit, pc->port, a, *phy_data, d, m, pc->this_lane, pc->lane_select) ;
   if(pc->tsc_touched &&(pc->ctrl_type&TSCMOD_CTRL_TYPE_SEMA_CHK))
      printf("Error: uC Proxy semaphore violation u=%0d p=%0d addr=%x rd=%x wd=%x mask=%x paddr=%0x\n",
             unit, pc->port, a, *phy_data, d, m, pc->tsc_touched) ;
   if(a) pc->tsc_touched = a; else  pc->tsc_touched =1 ;

   rv = SOC_E_NONE ;  cmd = 0 ;
   if(access_mode ==TSCMOD_REG_WR) cmd =  TSCMOD_UC_SYNC_WR_MASK ;

   /* rv = tscmod_reg_aer_read_core(unit, pc, TSCMOD_UC_SYNC_CMDr, &rdata) ; read back for debug */

   /* clear cmd */
   rv = tscmod_reg_aer_write_core(unit, pc, TSCMOD_UC_SYNC_CMDr, 0) ;

   /* rv = tscmod_reg_aer_read_core(unit, pc, TSCMOD_UC_SYNC_CMDr, &rdata) ; read back for debug */

   /* lower address */
   add= a ;
   rv = tscmod_reg_aer_write_core(unit, pc, TSCMOD_UC_SYNC_LADDr, add) ;
   /* upper address */
   add = (a>>16) ;
   rv = tscmod_reg_aer_write_core(unit, pc, TSCMOD_UC_SYNC_HADDr, add) ;
   /* write the data */
   if(access_mode ==TSCMOD_REG_WR)
      rv |= tscmod_reg_aer_write_core(unit, pc, TSCMOD_UC_SYNC_DATAr, d) ;
   /* write the cmd */
   cmd |= TSCMOD_UC_SYNC_REQ_MASK ;
   rv |= tscmod_reg_aer_write_core(unit, pc, TSCMOD_UC_SYNC_CMDr, cmd) ;
   /* wait for uC done for write or read */
   loop = 0 ; cnt = 0 ; rdata = 0 ;
   start_time = sal_time_usecs() ;
   diff_time  = 0 ;  restore_val1 =0 ;  restore_val2 =0 ;  strike =0 ;
   while(1) {
      tscmod_reg_aer_read_core(unit, pc, TSCMOD_UC_SYNC_CMDr, &rdata) ;
      if( rdata & TSCMOD_UC_SYNC_DONE_MASK ) break ;
      if(loop++ >= TSCMOD_UC_PROXY_LOOP_CNT) {
         loop = 0 ;  cnt ++ ;
         if (pc->verbosity & TSCMOD_DBG_REG) {
            diff_time = sal_time_usecs() - start_time ;
            printf("%-22s: u=%0d p=%0d addr=%x access=%0d wait done (%0d usc) pending\n",
                   FUNCTION_NAME(), pc->unit, pc->port, a, access_mode, diff_time) ;
         }
      }
      if(cnt >TSCMOD_UC_PROXY_CNT_CAP) {
         diff_time = sal_time_usecs() - start_time  ;
         if(diff_time <= TSCMOD_UC_PROXY_WAIT_TIME) {
            cnt-- ;
         } else if(diff_time <= TSCMOD_UC_PROXY_WAIT_TIME_STRIKE1) {
            cnt-- ;
            if(strike==0) {
               strike = 1 ;
               /* port-wise recovery */
               rv |= tscmod_reg_rx_sig_det(pc, 0, 1, &restore_val1, access_mode, a, d, m) ;
            }
         } else if(diff_time <= TSCMOD_UC_PROXY_WAIT_TIME_STRIKE2) {
            cnt-- ;
            if(strike==0) {
               strike = 1 ;
               /* port-wise recovery */
               rv |= tscmod_reg_rx_sig_det(pc, 0, 1, &restore_val1, access_mode, a, d, m) ;
            } else if(strike==1) {
               strike = 2 ;
               /* core-wise recovery */
               rv |= tscmod_reg_rx_sig_det(pc, 1, 1, &restore_val2, access_mode, a, d, m) ;
            }
         } else {
            if(!(pc->err_code&TSCMOD_ERR_UC_SYNC_CMD_TO)||(pc->verbosity&TSCMOD_DBG_PRINT)) {
               printf("Error: uC proxy access aborted u=%0d p=%0d addr=%x access=%0d d=%x m=%x sel=0x%x l=%0d usec=%0d\n",
                      pc->unit, pc->port, a, access_mode, d, m, pc->lane_select, pc->this_lane, diff_time) ;
            }
            pc->err_code  |= TSCMOD_ERR_UC_SYNC_CMD_TO ;
            pc->err_code  |= TSCMOD_ERR_REG_UC_SYNC_CMD_TO ;
            pc->ctrl_type |= TSCMOD_CTRL_TYPE_ERR ;
            pc->ctrl_type |= TSCMOD_CTRL_TYPE_UC_STALL ;
            if(strike) {
               if(strike==2) { rv |= tscmod_reg_rx_sig_det(pc, 1, 0, &restore_val2, access_mode, a, d, m) ; }
               rv |= tscmod_reg_rx_sig_det(pc, 0, 0, &restore_val1, access_mode, a, d, m) ;
            }
            pc->accAddr = a ;
            if(pc->verbosity&(TSCMOD_DBG_PRINT|TSCMOD_DBG_LINK|TSCMOD_DBG_SPD|TSCMOD_DBG_AN|TSCMOD_DBG_INIT))
               tscmod_reg_uc_sync_error_debug(unit, pc) ;
            pc->tsc_touched = 0 ;
            return SOC_E_NONE ;
         }
      }
   }
   if(pc->verbosity & TSCMOD_DBG_REG) {
      diff_time = sal_time_usecs() - start_time ;
      if(access_mode !=TSCMOD_REG_RD)
         printf("%-22s: u=%0d p=%0d addr=%x access=%0d wait usec=%0d\n",
                FUNCTION_NAME(), unit, pc->port, a, access_mode, diff_time) ;
   }
   if(access_mode ==TSCMOD_REG_RD) {
      rv |= tscmod_reg_aer_read_core(unit, pc, TSCMOD_UC_SYNC_DATAr, &rdata) ;
      *phy_data = rdata ;
   }
   if(strike) {
      if(strike==2) { rv |= tscmod_reg_rx_sig_det(pc, 1, 0, &restore_val2, access_mode, a, d, m) ; }
      rv |= tscmod_reg_rx_sig_det(pc, 0, 0, &restore_val1, access_mode, a, d, m) ;
   }
   pc->accAddr = a ;
   pc->tsc_touched = 0 ;
   if ((pc->verbosity & TSCMOD_DBG_REG) && (access_mode ==TSCMOD_REG_RD))
      printf("%-22s: u=%0d p=%0d addr=%x RD access=%0d rd=%x wait usec=%0d\n",
             FUNCTION_NAME(), unit, pc->port, a, access_mode, rdata, diff_time) ;
   return rv ;
}

int tscmod_reg_uc_sync_cmd(uint32 unit, tscmod_st *pc, int access_mode, uint32 a, uint16 *phy_data, uint16 d, uint16 m)
{
   uint32 addr ;
   int tmp_sel, tmp_lane, tmp_dxgxs, len_e, len_s, idx, rv;
   uint16 wdata ;

   rv   = SOC_E_NONE ;
   addr = a & 0xf800ffff ;

   tmp_sel   = pc->lane_select ;
   tmp_lane  = pc->this_lane ;
   tmp_dxgxs = pc->dxgxs ;

   rv = SOC_E_NONE ;

   if((access_mode==TSCMOD_REG_RD)||(access_mode==TSCMOD_REG_MO)) {
      if (pc->lane_select == TSCMOD_LANE_BCST) {
         pc->lane_select = getLaneSelect(0) ;
         pc->this_lane   = 0 ;
         pc->dxgxs = 0 ;
      } else if(pc->dxgxs & 0x3) {
         if(pc->dxgxs & 0x2) {
            addr |= (0x2) << 16 ;
            pc->this_lane   = 2 ;
         } else {
            pc->this_lane   = 0 ;
         }
         pc->dxgxs = 0 ;
      }
      addr |= ((pc->this_lane & 0x3)<<16);
      rv = tscmod_reg_uc_sync_cmd_core(unit, pc, TSCMOD_REG_RD, addr, phy_data, d, m) ;
   }
   if(access_mode==TSCMOD_REG_RD) {
      pc->lane_select = tmp_sel ;
      pc->this_lane   = tmp_lane ;
      pc->dxgxs       = tmp_dxgxs ;
      return rv ;
   }
   len_s = tmp_lane ; len_e = len_s+1 ;
   if (tmp_sel == TSCMOD_LANE_BCST) {
      len_e = 4 ; len_s = 0 ;
   } else if ((tmp_dxgxs & 0x3) == 0x1 ) {
      len_e = 2 ;  len_s = 0 ;
   } else if((tmp_dxgxs & 0x3) ==0x2) {
      len_e = 4 ; len_s = 2 ;
   }

   wdata = (d & m) | (*phy_data & ~m) ;
   for(idx=len_s;idx<len_e; idx++) {
      pc->this_lane   = idx ;
      pc->lane_select = getLaneSelect(idx) ;
      pc->dxgxs       = 0 ;
      addr = a & 0xf800ffff ;
      addr |= ((pc->this_lane & 0x3)<<16);
      rv |= tscmod_reg_uc_sync_cmd_core(unit, pc, TSCMOD_REG_WR, addr, phy_data, wdata, m) ;
   }
   pc->lane_select = tmp_sel ;
   pc->this_lane   = tmp_lane ;
   pc->dxgxs       = tmp_dxgxs ;
   return rv ;
}

int tscmod_reg_aer_read(uint32 unit, tscmod_st *pc, uint32 a, uint16 *phy_data)
{
   int rpt_mode, access_mode, rv ;
   uint16 d, m ;

   rpt_mode = 0 ; access_mode =TSCMOD_REG_RD ;  d=0 ; m =0 ;
   if(pc->reg_sync&&(pc->ctrl_type&TSCMOD_CTRL_TYPE_SEMA_CHK))
      printf("Error: TSC RD sema_viol u=%0d p=%0d addr=%x paddr=%x l=%0d sel=%x\n",
             unit, pc->port, a, pc->reg_sync, pc->this_lane, pc->lane_select) ;
   if(a) pc->reg_sync = a ; else pc->reg_sync = 1 ;
   if( tscmod_reg_domain_test(pc, a, access_mode, rpt_mode) )
      rv=tscmod_reg_uc_sync_cmd(unit, pc, access_mode, a, phy_data, d, m) ;
   else
      rv=tscmod_reg_aer_read_core(unit, pc, a, phy_data) ;

#if defined (TSCMOD_PCB_DEBUG)
   rv=tscmod_pcb_status_check(unit, pc, a, *phy_data, 0, 1) ;
#endif
   pc->reg_sync = 0 ;
   return rv ;
}


/*!
\brief Tier1 API for register write.

\param unit Passed in. Need not be used.

\param pc Pointer to tscmod_st

\param a 32 bit address

\param d 16 bit data to be written.

\returns  0 on success. Non-zero on fail.

\details
*/
int tscmod_reg_aer_write_core(uint32 unit, tscmod_st *pc, uint32 a, uint16 d)
{
  uint32 phy_reg_aer, phy_reg_blk,phy_reg_addr, addr;
  int    cl45_devid, cl45_ln_no, cl45_addr, cl45_uaddr;
  int    rv, access_mode, rpt_mode ;
  int data = d;

  addr = a & 0xf800ffff;
  rv = SOC_E_NONE ;
  access_mode = TSCMOD_REG_WR ; rpt_mode = 1;

  tscmod_reg_domain_test(pc, a, access_mode, rpt_mode) ;

  if (pc->lane_select == TSCMOD_LANE_BCST) {
     /* addr |= (pc->aer_bcst_ofs_strap) << 16; */
     addr |= (0x6) << 16;
  } else if (pc->dxgxs & 0x3) {
     if(pc->dxgxs & 0x2) addr |= (0x5) << 16;
     else                addr |= (0x4) << 16;
  } else {
     addr |= ((pc->this_lane & 0xf)<<16);
  }
  pc->accAddr = addr ;

  if (pc->verbosity & TSCMOD_DBG_REG)
    printf("%-22s: u:%0d p:%0d phy_ad=%x addr:0x%x(%x) data:0x%x\n",
           FUNCTION_NAME(),pc->unit,pc->port, pc->phy_ad, a, addr, d);

  if (pc->regacc_type == TSCMOD_MDIO_PCB) {
#if defined (_DV_TRIDENT2)
    addr = a & 0xf800ffff;
    svSetScope(svGetScopeFromName("SerdesInitializer_pkg"));
    dv_slam_write_reg((pc->unit + pc->this_lane),addr,data);
    return SOC_E_NONE;
#endif
#if defined (_SDK_TSCMOD_) || defined(_MDK_TSCMOD_)
    printf("%s FATAL. No PCB routines in non-Verilog mode\n", FUNCTION_NAME());
    return SOC_E_ERROR;
#endif
  } else if (pc->regacc_type == TSCMOD_MDIO_CL45) {
  } else if        (pc->regacc_type == TSCMOD_MDIO_CL45) {
    phy_reg_addr = PHY_AER_REG_ADDR_CL45_REGAD(addr);   /* 0000FFFF  */
    phy_reg_aer  = PHY_AER_REG_ADDR_AER(addr); 		/*(FFFF0000 >> 16)*/
    cl45_devid   = PHY_AER_REG_ADDR_CL45_DEVID(addr);   /* F8000000 >> 27 */
    cl45_ln_no   = PHY_AER_REG_ADDR_LANE_NUM(addr);     /* 07FF0000 >> 16 */
    cl45_addr    = SOC_PHY_CLAUSE45_ADDR(0,0xffde);
    cl45_uaddr   = SOC_PHY_CLAUSE45_ADDR(cl45_devid,phy_reg_addr) ;
    if (cl45_ln_no) /* write the lane number to the AER only if nonzero */
       tscmod_cl45_write(pc,cl45_addr,cl45_ln_no);

    tscmod_cl45_write(pc, cl45_uaddr,data);

    if (cl45_ln_no) /* restore back the lane number to 0 */
       tscmod_cl45_write(pc,cl45_addr,0);

  } else if (pc->regacc_type == TSCMOD_MDIO_CL22) {
    phy_reg_aer  = PHY_AER_REG_ADDR_AER(addr);
    phy_reg_blk  = PHY_AER_REG_ADDR_BLK(addr);
    phy_reg_addr = PHY_AER_REG_ADDR_REGAD(addr);

    if((pc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY)&&
       (pc->ctrl_type&TSCMOD_CTRL_TYPE_FW_LOADED)&&
       ((a & 0xf800fff0)==TSCMOD_UC_SYNC_BLOCK)){
       /* no need to set aer register */
    } else {
       rv = tscmod_cl22_write(pc, 0x1f, 0xffd0);
       pc->blk_adr = 0xffd0 ;
       rv |= tscmod_cl22_write(pc, 0x1e, phy_reg_aer);
    }
    if((pc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR)&&
       (pc->ctrl_type&TSCMOD_CTRL_TYPE_FW_LOADED)&&
       ((a & 0xf800fff0)==TSCMOD_UC_SYNC_BLOCK)&&(pc->blk_adr==TSCMOD_UC_SYNC_BLOCK)){
       /* no need to set blk register */
    } else {
       rv = tscmod_cl22_write(pc, 0x1f, phy_reg_blk); /* map block */
       pc->blk_adr = phy_reg_blk ;
    }
    rv |= tscmod_cl22_write(pc, phy_reg_addr, data); /* write reg */
  } else {
    printf("%-22s Error: Bad regacc_type:%d\n",FUNCTION_NAME(), pc->regacc_type);
    return SOC_E_ERROR;
  }
  return rv;
}

int tscmod_reg_aer_write(uint32 unit, tscmod_st *pc, uint32 a, uint16 d)
{
   int rpt_mode, access_mode, rv;
   uint16  m, rdata ;

   rpt_mode = 0 ; access_mode =TSCMOD_REG_WR ;  m =0xffff ;  rdata = 0 ;
   if(pc->reg_sync &&(pc->ctrl_type&TSCMOD_CTRL_TYPE_SEMA_CHK))
      printf("Error: TSC WR sema_viol u=%0d p=%0d addr=%x wd=%x paddr=%0x l=%0d sel=%x\n",
             unit, pc->port, a, d, pc->reg_sync, pc->this_lane, pc->lane_select) ;

   if ((SOC_WARM_BOOT(pc->unit)||SOC_IS_RELOADING(pc->unit))&&
       !(pc->ctrl_type&TSCMOD_CTRL_TYPE_WB_DIS)) {
      return SOC_E_NONE ;
   }

   if(a) pc->reg_sync = a ; else pc->reg_sync = 1 ;
   if( tscmod_reg_domain_test(pc, a, access_mode, rpt_mode) )
      rv=tscmod_reg_uc_sync_cmd(unit, pc, access_mode, a, &rdata, d, m) ;
   else
      rv=tscmod_reg_aer_write_core(unit, pc, a, d) ;
#if defined (TSCMOD_PCB_DEBUG)
   rv = tscmod_pcb_status_check(unit, pc, a, d, 0xffff, 0) ;
#endif
   pc->reg_sync =0 ;
   return rv ;
}
/*!
\brief Tier1 API for register read-modify-write.
considerations

\param unit Passed in. Need not be used.

\param pc Pointer to tscmod_st

\param a 32 bit address

\param d 16 bit data to be written.

\param m 16 bit data mask to control the read-modify-write sequence.

\returns  0 on success. Non-zero on fail.

\details
*/
int tscmod_reg_aer_modify_core(uint32 unit, tscmod_st *pc,uint32 a,uint16 d,uint16 m)
{
  uint16  otmp, tmp_data;
  int rv, data,  mask, cl45_devid, cl45_ln_no, cl45_addr, cl45_uaddr;
  uint32 phy_reg_aer, phy_reg_blk,phy_reg_addr, addr, raddr, waddr;
  int access_mode, rpt_mode ;

  rv=SOC_E_NONE ;
  access_mode = TSCMOD_REG_MO ;
  rpt_mode    = 1 ;

  data = (d & 0xffff);
  mask = m;
  raddr=a; waddr=a; addr = (a & 0xf800ffff);

  tscmod_reg_domain_test(pc, a, access_mode, rpt_mode ) ;

  if (pc->lane_select == TSCMOD_LANE_BCST) {
      raddr = addr & 0xf800ffff; /* kill extension. i.e. always read from 0. */
      waddr = (0x6 << 16) | addr;
  } else if (pc->dxgxs & 0x3) {
     if(pc->dxgxs & 0x2) {
        waddr = (0x5 << 16) | addr;
        raddr = (0x2 << 16) | addr;
     } else {
        waddr = (0x4 << 16) | addr;
        raddr = addr;
     }
  } else {
     addr |= ((pc->this_lane & 0x3) << 16);
     raddr = addr; waddr = addr;
  }
  pc->accAddr = raddr ;

  if (pc->verbosity & TSCMOD_DBG_REG)
    printf("%-22s: u:%0d p:%0d Pre Mod:addr:0x%08x->(w/r:%08x/%08x) data:0x%04x mask:0x%x\n",
           FUNCTION_NAME(), pc->unit, pc->port, a, waddr,raddr, data, mask);

  if (pc->regacc_type == TSCMOD_MDIO_PCB) {
#if defined (_DV_TRIDENT2)
    int rdata ;
    addr = (a & 0xf800ffff);
    /* svSetScope(svGetScopeFromName("envtop"));  */
    svSetScope(svGetScopeFromName("SerdesInitializer_pkg"));
    /* dv_slam_modify_reg((pc->unit + pc->this_lane), addr, data, mask); */

    /* assume bsct AER has been setup */
    if ((pc->lane_select == TSCMOD_LANE_BCST)||
        (pc->dxgxs & 0x3)) {
       rdata = pc->this_lane ;
       dv_slam_write_reg((pc->unit + pc->this_lane), 0xffde, rdata);
    }

    dv_slam_read_reg((pc->unit + pc->this_lane), addr, & rdata);
    printf(" tsc_reg_modify Pre  pnum=%0d addr=%08x rddata=%04x wdata=%04x mask=%04x\n", (pc->unit + pc->this_lane),  addr, rdata, data, mask);

    tmp_data = rdata ;

    /* restore bcst AER setup */
    if((pc->lane_select == TSCMOD_LANE_BCST)||
       (pc->dxgxs & 0x3)) {
       if (pc->lane_select == TSCMOD_LANE_BCST) {
         rdata = 0x6 ;
       } else if ((pc->dxgxs & 0x3) == 0x1 ) {
          rdata = 0x4 ;
       } else if((pc->dxgxs & 0x3) ==0x2) {
          rdata = 0x5 ;
       }
       dv_slam_write_reg((pc->unit + pc->this_lane), 0xffde, rdata);
    }

    data  = (tmp_data & (~mask)) | (data & mask);
    dv_slam_write_reg((pc->unit + pc->this_lane), addr, data);
    printf(" tsc_reg_modify Post pnum=%0d addr=%08x data=%04x mask=%04x\n", (pc->unit + pc->this_lane), addr, data, mask) ;

    return SOC_E_NONE;
#endif
#if defined (_SDK_TSCMOD_) || defined(_MDK_TSCMOD_)
  printf("%-22s FATAL. No PCB routines in non-Verilog mode\n", FUNCTION_NAME());
  return SOC_E_ERROR;
#endif
  } else if        (pc->regacc_type == TSCMOD_MDIO_CL45) {
    phy_reg_aer  = PHY_AER_REG_ADDR_AER(raddr);
    phy_reg_addr = PHY_AER_REG_ADDR_CL45_REGAD(raddr);
    cl45_devid = PHY_AER_REG_ADDR_CL45_DEVID(raddr);
    cl45_ln_no   = PHY_AER_REG_ADDR_LANE_NUM(raddr);     /* 07FF0000 >> 16 */
    cl45_addr    = SOC_PHY_CLAUSE45_ADDR(0,0xffde);
    cl45_uaddr   = SOC_PHY_CLAUSE45_ADDR(cl45_devid, phy_reg_addr);

    if (cl45_ln_no) /* write the lane number to the AER only if nonzero */
      tscmod_cl45_write(pc,cl45_addr,cl45_ln_no);

    /* read */
    tscmod_cl45_read(pc,cl45_uaddr,&tmp_data);

    /* modify */
    otmp     = tmp_data;
    tmp_data &= ~mask;
    tmp_data |= data;

    cl45_ln_no   = PHY_AER_REG_ADDR_LANE_NUM(raddr);
    if (cl45_ln_no)
       tscmod_cl45_write(pc,cl45_addr,cl45_ln_no);

    /* modify */
    if (otmp != tmp_data)
      tscmod_cl45_write(pc, cl45_addr,tmp_data);
    else
      printf("%-22s Write skipped. No need to write\n", FUNCTION_NAME());

    if (cl45_ln_no) /* restore back the lane number to 0 */
       tscmod_cl45_write(pc,cl45_addr,0);

  } else if (pc->regacc_type == TSCMOD_MDIO_CL22) {
    phy_reg_aer  = PHY_AER_REG_ADDR_AER(raddr);    /* upper 16 bits */
    phy_reg_blk  = PHY_AER_REG_ADDR_BLK(raddr);    /* 12 bits mask=0xfff0 */
    phy_reg_addr = PHY_AER_REG_ADDR_REGAD(raddr);  /* 5 bits {15,3,2,1,0} */

    /*  read out value. */
    if((pc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY)&&
       (pc->ctrl_type&TSCMOD_CTRL_TYPE_FW_LOADED)&&
       ((a & 0xf800fff0)==TSCMOD_UC_SYNC_BLOCK)){
       /* no need to set aer register */
    } else {
       rv = tscmod_cl22_write(pc, 0x1f, 0xffd0);
       pc->blk_adr = 0xffd0 ;
       rv = tscmod_cl22_write(pc, 0x1e, phy_reg_aer);
    }
    if((pc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR)&&
       (pc->ctrl_type&TSCMOD_CTRL_TYPE_FW_LOADED)&&
       ((a & 0xf800fff0)==TSCMOD_UC_SYNC_BLOCK)&&(pc->blk_adr==TSCMOD_UC_SYNC_BLOCK)){
       /* no need to set blk register */
    } else {
       rv = tscmod_cl22_write(pc, 0x1f, phy_reg_blk); /* map block */
       pc->blk_adr = phy_reg_blk ;
    }
    rv |= tscmod_cl22_read(pc, phy_reg_addr, &tmp_data); /* read reg */

    /* modify */
    otmp  = tmp_data ;
    data &= mask;
    tmp_data &= ~(mask);
    tmp_data |= data;

    /* write back */
    phy_reg_aer  = PHY_AER_REG_ADDR_AER(waddr);    /* upper 16 bits */
    phy_reg_blk  = PHY_AER_REG_ADDR_BLK(waddr);    /* 12 bits mask=0xfff0 */
    phy_reg_addr = PHY_AER_REG_ADDR_REGAD(waddr);  /* 5 bits {15,3,2,1,0} */

    if(raddr !=waddr){
       if((pc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY)&&
          (pc->ctrl_type&TSCMOD_CTRL_TYPE_FW_LOADED)&&
          ((a & 0xf800fff0)==TSCMOD_UC_SYNC_BLOCK)){
          /* no need to set aer register */
       } else {
          rv |= tscmod_cl22_write(pc, 0x1f, 0xffd0);
          pc->blk_adr = 0xffd0 ;
          rv |= tscmod_cl22_write(pc, 0x1e, phy_reg_aer);
       }
       if((pc->ctrl_type&TSCMOD_CTRL_TYPE_ALL_PROXY_BLK_ADR)&&
          (pc->ctrl_type&TSCMOD_CTRL_TYPE_FW_LOADED)&&
          ((a & 0xf800fff0)==TSCMOD_UC_SYNC_BLOCK)&&(pc->blk_adr==TSCMOD_UC_SYNC_BLOCK)){
          /* no need to set blk register */
       } else {
          rv |= tscmod_cl22_write(pc, 0x1f, phy_reg_blk);
          pc->blk_adr = phy_reg_blk ;
       }
    }
    rv |= tscmod_cl22_write(pc, phy_reg_addr, tmp_data);
  } else {
    printf("%-22s FATAL: Bad regacc_type:%d\n",FUNCTION_NAME(), pc->regacc_type);
    return SOC_E_ERROR;
  }
  if (pc->verbosity & TSCMOD_DBG_REG )
    printf("%-22s: u:%0d p:%0d Post Mod: phy_ad:%x addr:0x%08x rdb:0x%04x data:0x%04x mask:0x%x\n",
           FUNCTION_NAME(), pc->unit, pc->port, pc->phy_ad, waddr, otmp, tmp_data, mask);
  return rv;
}

int tscmod_reg_aer_modify(uint32 unit, tscmod_st *pc,uint32 a,uint16 d,uint16 m)
{

   int rpt_mode, access_mode, rv;
   uint16  rdata ;

   if(pc->reg_sync&&(pc->ctrl_type&TSCMOD_CTRL_TYPE_SEMA_CHK))
      printf("Error: TSC MO sema_viol u=%0d p=%0d addr=%x wd=%x mask=%0x paddr=%0x, l=%0d sel=%x\n",
             unit, pc->port, a, d, m, pc->reg_sync, pc->this_lane, pc->lane_select) ;

   if ((SOC_WARM_BOOT(pc->unit)||SOC_IS_RELOADING(pc->unit))&&
       !(pc->ctrl_type&TSCMOD_CTRL_TYPE_WB_DIS)) {
      return SOC_E_NONE ;
   }

   pc->reg_sync =1 ;
   rpt_mode = 0 ; access_mode =TSCMOD_REG_MO ;  rdata = 0 ;

   if( tscmod_reg_domain_test(pc, a, access_mode, rpt_mode) )
      rv=tscmod_reg_uc_sync_cmd(unit, pc, access_mode, a, &rdata, d, m) ;
   else
      rv=tscmod_reg_aer_modify_core(unit, pc, a, d, m) ;

#if defined (TSCMOD_PCB_DEBUG)
   rv=tscmod_pcb_status_check(unit, pc, a, d, m, -1) ;
#endif

   pc->reg_sync =0 ;
   return rv ;

}

/*!
\brief Check for timeout. Maintains down counter

\param to

\returns : 1 only if timeout. Else 0.

\details
*/
int tscmod_timeout_check(tscmod_st* pc, soc_timeout_t *to)
{
#if defined (_SDK_TSCMOD_)
  /* to be implemented */
  return SOC_E_NONE;
#else

#if defined (_MDK_TSCMOD_)
  /* to be implemented */
  return SOC_E_NONE;
#endif

#if defined (STANDALONE)
  to->usec--;
  printf("%-22s port=%0d Iteration:%d-us poll=%0d\n", FUNCTION_NAME(), pc->port, to->usec, to->min_polls);
  if (to->usec > 0)
    return SOC_E_NONE;
  return SOC_E_ERROR;
#endif

#if defined (_DV_TRIDENT2)
  to->min_polls--;
  printf("%-22s port=%0d Iteration:%d-us poll=%0d\n", FUNCTION_NAME(), pc->port, to->usec, to->min_polls);
  if (to->min_polls > 0) {
    dv_wait(1); /* exported verilog task which implements #1 * input_dleay */
    return SOC_E_NONE;
  }
  printf("%-22s FATAL: p=%0d Max iteration limit exceeded\n", FUNCTION_NAME(), pc->port);
  return SOC_E_ERROR;
#endif
  printf("%-22s FATAL: Unimplemented function\n",FUNCTION_NAME());
  return SOC_E_ERROR;
#endif
}

/*!
\brief Internal function. Platform dependant microsecond delay

\param usec delay in microseconds.

\returns : Nothing.

\details
*/
#ifndef _SDK_TSCMOD_
int sal_udelay(uint32 usec)
{
#if defined (_MDK_TSCMOD_)
/* To be implemented */
  printf("%-22s FATAL: not implemented in standalone mode\n",FUNCTION_NAME());
  return SOC_E_ERROR;
#endif
#if defined (STANDALONE)
  printf("%-22s FATAL: not implemented in standalone mode\n",FUNCTION_NAME());
  return SOC_E_ERROR;
#endif

#if defined (_DV_TRIDENT2)
  dv_wait(usec); /* exported verilog task which implements #1000 */
  return SOC_E_NONE;
#endif
  printf("%-22s FATAL: unimplemented function\n",FUNCTION_NAME());
  return SOC_E_ERROR;
}
#endif /*_SDK_TSCMOD_ */


#endif /* #ifdef INCLUDE_XGXS_TSCMOD */
