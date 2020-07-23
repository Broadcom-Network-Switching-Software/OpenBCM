/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DFE INTERRUPT
 */
 
#ifdef BCM_DFE_SUPPORT
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dfe/fe1600/fe1600_interrupts.h>
#include <soc/dfe/fe1600/fe1600_defs.h>
#include <soc/cm.h>
#include <soc/error.h>
#include <soc/intr.h>
#include <soc/chip.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_warm_boot.h>


char ___fe1600_interrupts_junk_array1[_FE1600_INT_LAST - FE1600_INT_LAST + 1];
char ___fe1600_interrupts_junk_array2[FE1600_INT_LAST - _FE1600_INT_LAST + 1];

static soc_interrupt_tree_t fe1600_interrupt_tree_blk_invalid                = {INVALIDr, INVALIDr, 0, {-1}};
static soc_interrupt_tree_t fe1600_interrupt_tree_blk_eci                     = {ECI_INTERRUPT_REGISTERr,  ECI_INTERRUPT_MASK_REGISTERr, 0, {-1, -1, -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , FE1600_INT_ECI_ECIINTERNALINT, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_blk_occg                     = {OCCG_INTERRUPT_REGISTERr, OCCG_INTERRUPT_MASK_REGISTERr, 0, {-1, -1, -1, -1, -1, -1, -1,FE1600_INT_OCCG_TESTMODECMDFINISHINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_blk_dch                     = {DCH_INTERRUPT_REGISTERr, DCH_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_DCH_DCHDESCCNTOP,FE1600_INT_DCH_DCHUNEXPCELLP,FE1600_INT_DCH_IFMAFOPINT,FE1600_INT_DCH_IFMBFOPINT,FE1600_INT_DCH_CPUDATACELLFNEA0PINT,FE1600_INT_DCH_CPUDATACELLFNEA1PINT,FE1600_INT_DCH_ALTOPINT,FE1600_INT_DCH_UNREACHDESTEVPINT,FE1600_INT_DCH_DRPALINKACTVERRPINT,FE1600_INT_DCH_DRPBLINKACTVERRPINT,FE1600_INT_DCH_ERRORFILTERPINT,FE1600_INT_DCH_INTREG1INT,FE1600_INT_DCH_INTREG2INT,FE1600_INT_DCH_CELLTYPEERRINT,FE1600_INT_DCH_DCHPNEXPERROR2,FE1600_INT_DCH_DCHSNEXPERROR2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_blk_dcl                     = {DCL_INTERRUPT_REGISTERr, DCL_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_DCL_CCP0SRCDVCNGLINKINT,FE1600_INT_DCL_CCP1SRCDVCNGLINKINT,FE1600_INT_DCL_CCP0ILLCELLINT,FE1600_INT_DCL_CCP1ILLCELLINT,FE1600_INT_DCL_CCP0CHFOVFINT,FE1600_INT_DCL_CCP0CLFOVFINT,FE1600_INT_DCL_CCP1CHFOVFINT,FE1600_INT_DCL_CCP1CLFOVFINT,FE1600_INT_DCL_DCLUTAGPARERRINT,FE1600_INT_DCL_DCLMTAGPARERRINT,FE1600_INT_DCL_MACADATACRCERRINT,FE1600_INT_DCL_MACBDATACRCERRINT,FE1600_INT_DCL_ECC_1BERRINT,FE1600_INT_DCL_ECC_2BERRINT,FE1600_INT_DCL_PARITYERRINT,FE1600_INT_DCL_DRPIPPINT,FE1600_INT_DCL_DRPIPSINT,-1,-1,FE1600_INT_DCL_OUTOFSYNCINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_blk_dcma                     = {DCMA_INTERRUPT_REGISTERr, DCMA_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_DCMA_DCMUADRPIPINT, FE1600_INT_DCMA_DCMUADRPINT, FE1600_INT_DCMA_DCMMADRPIPINT, FE1600_INT_DCMA_DCMMADRPINT, FE1600_INT_DCMA_ECC_1BERRINT, FE1600_INT_DCMA_ECC_2BERRINT, FE1600_INT_DCMA_PARITYERRINT, FE1600_INT_DCMA_OUTOFSYNCPINT, FE1600_INT_DCMA_OUTOFSYNCSINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_blk_dcmb                     = {DCMB_INTERRUPT_REGISTERr, DCMB_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_DCMB_DCMUBDRPIPINT, FE1600_INT_DCMB_DCMUBDRPINT, FE1600_INT_DCMB_DCMMBDRPIPINT, FE1600_INT_DCMB_DCMMBDRPINT, FE1600_INT_DCMB_ECC_1BERRINT, FE1600_INT_DCMB_ECC_2BERRINT, FE1600_INT_DCMB_PARITYERRINT, FE1600_INT_DCMB_OUTOFSYNCPINT, FE1600_INT_DCMB_OUTOFSYNCSINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_blk_ccs                     = {CCS_INTERRUPT_REGISTERr, CCS_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_CCS_CPUCAPTCELLFNEINT, FE1600_INT_CCS_UNRCHDSTINT, FE1600_INT_CCS_CDMALPFIFOVFINT, FE1600_INT_CCS_CDMBLPFIFOVFINT, -1, FE1600_INT_CCS_ECC_1BERRINT, FE1600_INT_CCS_ECC_2BERRINT, FE1600_INT_CCS_PARITYERRINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_blk_rtp                     = {RTP_INTERRUPT_REGISTERr, RTP_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_RTP_DRHP0INTREG, FE1600_INT_RTP_DRHP1INTREG, FE1600_INT_RTP_DRHP2INTREG, FE1600_INT_RTP_DRHP3INTREG, FE1600_INT_RTP_DRHS0INTREG, FE1600_INT_RTP_DRHS1INTREG, FE1600_INT_RTP_DRHS2INTREG, FE1600_INT_RTP_DRHS3INTREG, FE1600_INT_RTP_CRH0INTREG, FE1600_INT_RTP_CRH1INTREG, FE1600_INT_RTP_GENERALINTREG, FE1600_INT_RTP_ECC1BERRINTREG, FE1600_INT_RTP_ECC2BERRINTREG, FE1600_INT_RTP_PARERRINTREG,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_blk_fmac                     = {FMAC_INTERRUPT_REGISTERr, FMAC_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_MAC_INTREG1, FE1600_INT_MAC_INTREG2, FE1600_INT_MAC_INTREG3, FE1600_INT_MAC_INTREG4, FE1600_INT_MAC_INTREG5, FE1600_INT_MAC_INTREG6, FE1600_INT_MAC_INTREG7, FE1600_INT_MAC_INTREG8, FE1600_INT_MAC_INTREG9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_blk_fsrd                     = {FSRD_INTERRUPT_REGISTERr, FSRD_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_SRD_INTREG0, FE1600_INT_SRD_INTREG1, FE1600_INT_SRD_INTREG2, FE1600_INT_SRD_INTREG3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_eci_mac         = {ECI_MAC_INTERRUPT_REGISTERr, ECI_MAC_INTERRUPT_MASK_REGISTERr, 0, {-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_eciint         = {ECI_ECI_INTERNAL_INTERRUPT_REGISTERr, ECI_ECI_INTERNAL_INTERRUPT_MASK_REGISTERr, 0, {-1, FE1600_INT_ECI_SERINT, FE1600_INT_ECI_OCCGINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_dch_int1     = {DCH_INTERRUPT_REGISTER_1r, DCH_INTERRUPT_MASK_REGISTER_1r, 0, {FE1600_INT_DCH_DCHDESCCNTOS, FE1600_INT_DCH_DCHUNEXPCELLS,FE1600_INT_DCH_IFMAFOSINT, FE1600_INT_DCH_IFMBFOSINT, FE1600_INT_DCH_CPUDATACELLFNEA0SINT, FE1600_INT_DCH_CPUDATACELLFNEA1SINT, FE1600_INT_DCH_ALTOSINT, FE1600_INT_DCH_UNREACHDESTEVSINT, FE1600_INT_DCH_DRPALINKACTVERRSINT, FE1600_INT_DCH_DRPBLINKACTVERRSINT, FE1600_INT_DCH_ERRORFILTERSINT,FE1600_INT_DCH_OUTOFSYNCINTP,FE1600_INT_DCH_OUTOFSYNCINTS,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_dch_int2     = {DCH_INTERRUPT_REGISTER_2r, DCH_INTERRUPT_MASK_REGISTER_2r, 0, {FE1600_INT_DCH_ECC_1BERRINT, FE1600_INT_DCH_ECC_2BERRINT, FE1600_INT_DCH_PARITYERRINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_drhp0     = {RTP_DRHP_INTERRUPT_REGISTERr, RTP_DRHP_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT, FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT, FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT,-1,-1, FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_drhp1     = {RTP_DRHP_INTERRUPT_REGISTERr, RTP_DRHP_INTERRUPT_MASK_REGISTERr, 1, {FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT, FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT, FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT,-1,-1, FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_drhp2     = {RTP_DRHP_INTERRUPT_REGISTERr, RTP_DRHP_INTERRUPT_MASK_REGISTERr, 2, {FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT, FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT, FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT,-1,-1, FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_drhp3     = {RTP_DRHP_INTERRUPT_REGISTERr, RTP_DRHP_INTERRUPT_MASK_REGISTERr, 3, {FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT, FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT, FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT,-1,-1, FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_drhs0     = {RTP_DRHS_INTERRUPT_REGISTERr, RTP_DRHS_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT, FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT, FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT,-1,-1, FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_drhs1     = {RTP_DRHS_INTERRUPT_REGISTERr, RTP_DRHS_INTERRUPT_MASK_REGISTERr, 1, {FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT, FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT, FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT,-1,-1, FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_drhs2     = {RTP_DRHS_INTERRUPT_REGISTERr, RTP_DRHS_INTERRUPT_MASK_REGISTERr, 2, {FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT, FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT, FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT,-1,-1, FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_drhs3     = {RTP_DRHS_INTERRUPT_REGISTERr, RTP_DRHS_INTERRUPT_MASK_REGISTERr, 3, {FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT, FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT, FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT,-1,-1, FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_crh0     = {RTP_CRH_INTERRUPT_REGISTERr, RTP_CRH_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT, -1,FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT, FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT, FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_crh1     = {RTP_CRH_INTERRUPT_REGISTERr, RTP_CRH_INTERRUPT_MASK_REGISTERr, 1, {FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT, -1,FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT, FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT, FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_general = {RTP_GENERAL_INTERRUPT_REGISTERr, RTP_GENERAL_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT, FE1600_INT_RTP_UNICASTTABLECHANGEDINT, -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_ecc1 = {RTP_ECC_1B_ERR_INTERRUPT_REGISTERr, RTP_ECC_1B_ERR_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_RTP_ECC_1BERRINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_ecc2 = {RTP_ECC_2B_ERR_INTERRUPT_REGISTERr, RTP_ECC_2B_ERR_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_RTP_ECC_2BERRINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_rtp_parity = {RTP_PAR_ERR_INTERRUPT_REGISTERr, RTP_PAR_ERR_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_RTP_PARITYERRINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_fmac_int1 = {FMAC_INTERRUPT_REGISTER_1r, FMAC_INTERRUPT_MASK_REGISTER_1r, 0, {FE1600_INT_MAC_RXCRCERRN_INT_0, FE1600_INT_MAC_RXCRCERRN_INT_1, FE1600_INT_MAC_RXCRCERRN_INT_2, FE1600_INT_MAC_RXCRCERRN_INT_3, FE1600_INT_MAC_WRONGSIZE_INT_0, FE1600_INT_MAC_WRONGSIZE_INT_1, FE1600_INT_MAC_WRONGSIZE_INT_2, FE1600_INT_MAC_WRONGSIZE_INT_3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_fmac_int2 = {FMAC_INTERRUPT_REGISTER_2r, FMAC_INTERRUPT_MASK_REGISTER_2r, 0, {FE1600_INT_MAC_LOS_INT_0, FE1600_INT_MAC_LOS_INT_1, FE1600_INT_MAC_LOS_INT_2, FE1600_INT_MAC_LOS_INT_3, FE1600_INT_MAC_RXLOSTOFSYNC_0, FE1600_INT_MAC_RXLOSTOFSYNC_1, FE1600_INT_MAC_RXLOSTOFSYNC_2, FE1600_INT_MAC_RXLOSTOFSYNC_3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_fmac_int3 = {FMAC_INTERRUPT_REGISTER_3r, FMAC_INTERRUPT_MASK_REGISTER_3r, 0, {FE1600_INT_MAC_LNKLVLAGEN_INT_0, FE1600_INT_MAC_LNKLVLAGEN_INT_1, FE1600_INT_MAC_LNKLVLAGEN_INT_2, FE1600_INT_MAC_LNKLVLAGEN_INT_3, FE1600_INT_MAC_LNKLVLHALTN_INT_0, FE1600_INT_MAC_LNKLVLHALTN_INT_1, FE1600_INT_MAC_LNKLVLHALTN_INT_2, FE1600_INT_MAC_LNKLVLHALTN_INT_3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_fmac_int4 = {FMAC_INTERRUPT_REGISTER_4r, FMAC_INTERRUPT_MASK_REGISTER_4r, 0, {FE1600_INT_MAC_OOF_INT_0, FE1600_INT_MAC_OOF_INT_1, FE1600_INT_MAC_OOF_INT_2, FE1600_INT_MAC_OOF_INT_3, FE1600_INT_MAC_DECERR_INT_0, FE1600_INT_MAC_DECERR_INT_1, FE1600_INT_MAC_DECERR_INT_2, FE1600_INT_MAC_DECERR_INT_3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_fmac_int5 = {FMAC_INTERRUPT_REGISTER_5r, FMAC_INTERRUPT_MASK_REGISTER_5r, 0, {FE1600_INT_MAC_TRANSMITERR_INT_0, FE1600_INT_MAC_TRANSMITERR_INT_1, FE1600_INT_MAC_TRANSMITERR_INT_2, FE1600_INT_MAC_TRANSMITERR_INT_3, FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0, FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1, FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2, FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_fmac_int6 = {FMAC_INTERRUPT_REGISTER_6r, FMAC_INTERRUPT_MASK_REGISTER_6r, 0, {FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0, FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1, FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2, FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_fmac_int7 = {FMAC_INTERRUPT_REGISTER_7r, FMAC_INTERRUPT_MASK_REGISTER_7r, 0, {FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0, FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1, FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2, FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_fmac_int8 = {FMAC_INTERRUPT_REGISTER_8r, FMAC_INTERRUPT_MASK_REGISTER_8r, 0, {FE1600_INT_MAC_TXFDRCIFCRC_INT_0, FE1600_INT_MAC_TXFDRCIFCRC_INT_1, FE1600_INT_MAC_TXFDRCIFCRC_INT_2, FE1600_INT_MAC_TXFDRCIFCRC_INT_3, FE1600_INT_MAC_TXFDRCIFPARITY_INT_0, FE1600_INT_MAC_TXFDRCIFPARITY_INT_1, FE1600_INT_MAC_TXFDRCIFPARITY_INT_2, FE1600_INT_MAC_TXFDRCIFPARITY_INT_3, FE1600_INT_MAC_TXFDRCIFFAULT_INT_0, FE1600_INT_MAC_TXFDRCIFFAULT_INT_1, FE1600_INT_MAC_TXFDRCIFFAULT_INT_2, FE1600_INT_MAC_TXFDRCIFFAULT_INT_3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_fmac_int9 = {FMAC_INTERRUPT_REGISTER_9r, FMAC_INTERRUPT_MASK_REGISTER_9r, 0, {FE1600_INT_MAC_ECC_1BERRINT, FE1600_INT_MAC_ECC_2BERRINT,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_fsrd_int0 = {FSRD_QUAD_INTERRUPT_REGISTERr, FSRD_QUAD_INTERRUPT_MASK_REGISTERr, 0, {FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0, FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1, FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2, FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3,-1,-1,-1,-1, FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_fsrd_int1 = {FSRD_QUAD_INTERRUPT_REGISTERr, FSRD_QUAD_INTERRUPT_MASK_REGISTERr, 1, {FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0, FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1, FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2, FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3,-1,-1,-1,-1, FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_fsrd_int2 = {FSRD_QUAD_INTERRUPT_REGISTERr, FSRD_QUAD_INTERRUPT_MASK_REGISTERr, 2, {FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0, FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1, FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2, FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3,-1,-1,-1,-1, FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};
static soc_interrupt_tree_t fe1600_interrupt_tree_vecotr_fsrd_int3 = {FSRD_QUAD_INTERRUPT_REGISTERr, FSRD_QUAD_INTERRUPT_MASK_REGISTERr, 3, {FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0, FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1, FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2, FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3,-1,-1,-1,-1, FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }};


static soc_interrupt_clear_read_fifo_t FE1600_INT_DCH_CPUDATACELLFNEA0PINT_read_fifo = {
    DCH_CPU_DATA_CELL_APr,
    FE1600_INT_DCH_CPU_DATA_CELL_AP_READ_COUNT_NUM
};

static soc_interrupt_clear_read_fifo_t FE1600_INT_DCH_CPUDATACELLFNEA0SINT_read_fifo = {
    DCH_CPU_DATA_CELL_ASr,
    FE1600_INT_DCH_CPU_DATA_CELL_AS_READ_COUNT_NUM
};

static soc_interrupt_clear_read_fifo_t FE1600_INT_DCH_CPUDATACELLFNEA1PINT_read_fifo = {
    DCH_CPU_DATA_CELL_BPr,
    FE1600_INT_DCH_CPU_DATA_CELL_BP_READ_COUNT_NUM 
};

static soc_interrupt_clear_read_fifo_t FE1600_INT_DCH_CPUDATACELLFNEA1SINT_read_fifo = {
    DCH_CPU_DATA_CELL_BSr,
    FE1600_INT_DCH_CPU_DATA_CELL_BS_READ_COUNT_NUM 
};

static soc_interrupt_clear_read_fifo_t FE1600_INT_DCH_ERRORFILTERPINT_read_fifo = {
    DCH_ERROR_FILTER_CNT_Pr,
    FE1600_INT_DCH_ERROR_FILTER_CNT_P_READ_COUNT_NUM
};

static soc_interrupt_clear_read_fifo_t FE1600_INT_DCH_ERRORFILTERSINT_read_fifo = {
    DCH_ERROR_FILTER_CNT_Sr,
    FE1600_INT_DCH_ERROR_FILTER_CNT_S_READ_COUNT_NUM
};

static soc_interrupt_clear_read_fifo_t FE1600_INT_CCS_CPUCAPTCELLFNEINT_read_fifo = {
    CCS_CAPTURED_CELLr,
    FE1600_INT_CCS_CAPTURED_CELL_READ_COUNT_NUM
};

static soc_interrupt_clear_read_fifo_t FE1600_INT_DCL_CCPSRCDCNGLINK_read_fifo = {
    DCL_CONNECTIVITY_LINKS_EVENTSr,
    FE1600_INT_CCPSRCDCNGLINK_READ_COUNT_NUM
};


int fe1600_interrupts_array_init(int unit)  
{
    int intr_id;
    char* interrupts_info_allocation= "interrupts_info_allocation";
    soc_interrupt_db_t *fe1600_interrupts;            
    soc_interrupt_tree_t *fe1600_interrupt_tree;
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);

    SOC_CONTROL(unit)->interrupts_info = sal_alloc(sizeof(soc_interrupt_t), "DFE: fe1600_interrupts");
    if (SOC_CONTROL(unit)->interrupts_info == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate fe1600_interrupts")));
    }

    fe1600_interrupts = sal_alloc((FE1600_INT_LAST + 1) * sizeof(soc_interrupt_db_t), "DFE: fe1600_interrupts");
    if (fe1600_interrupts == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate fe1600_interrupts")));
    }
    fe1600_interrupt_tree = sal_alloc(FE1600_INT_CIMIC_BLK_NUM * sizeof(soc_interrupt_tree_t), "DFE: fe1600_interrupt_tree"); 
    if (fe1600_interrupt_tree == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate fe1600_interrupt_tree")));
    }

    sal_memset(SOC_CONTROL(unit)->interrupts_info, 0, sizeof(soc_interrupt_t));
    sal_memset(fe1600_interrupts, 0, (FE1600_INT_LAST + 1) * sizeof(soc_interrupt_db_t));
    sal_memset(fe1600_interrupt_tree, 0, FE1600_INT_CIMIC_BLK_NUM * sizeof(soc_interrupt_tree_t));

    SOC_CONTROL(unit)->interrupts_info->interrupt_db_info = fe1600_interrupts;
    SOC_CONTROL(unit)->interrupts_info->interrupt_tree_info = fe1600_interrupt_tree;

       
    memcpy(&fe1600_interrupt_tree[0],  &fe1600_interrupt_tree_blk_eci, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[1],  &fe1600_interrupt_tree_blk_occg, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[2],  &fe1600_interrupt_tree_blk_dch, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[3],  &fe1600_interrupt_tree_blk_dch, sizeof(soc_interrupt_tree_t));          
    memcpy(&fe1600_interrupt_tree[4],  &fe1600_interrupt_tree_blk_dch, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[5],  &fe1600_interrupt_tree_blk_dch, sizeof(soc_interrupt_tree_t));        
    memcpy(&fe1600_interrupt_tree[6],  &fe1600_interrupt_tree_blk_dch, sizeof(soc_interrupt_tree_t));          
    memcpy(&fe1600_interrupt_tree[7],  &fe1600_interrupt_tree_blk_dcl, sizeof(soc_interrupt_tree_t));          
    memcpy(&fe1600_interrupt_tree[8],  &fe1600_interrupt_tree_blk_dcl, sizeof(soc_interrupt_tree_t));          
    memcpy(&fe1600_interrupt_tree[9],  &fe1600_interrupt_tree_blk_dcl, sizeof(soc_interrupt_tree_t));          
    memcpy(&fe1600_interrupt_tree[10], &fe1600_interrupt_tree_blk_dcl, sizeof(soc_interrupt_tree_t));         
    memcpy(&fe1600_interrupt_tree[11], &fe1600_interrupt_tree_blk_dcma, sizeof(soc_interrupt_tree_t));        
    memcpy(&fe1600_interrupt_tree[12], &fe1600_interrupt_tree_blk_dcmb, sizeof(soc_interrupt_tree_t));        
    memcpy(&fe1600_interrupt_tree[13], &fe1600_interrupt_tree_blk_dcma, sizeof(soc_interrupt_tree_t));        
    memcpy(&fe1600_interrupt_tree[14], &fe1600_interrupt_tree_blk_dcmb, sizeof(soc_interrupt_tree_t));        
    memcpy(&fe1600_interrupt_tree[15], &fe1600_interrupt_tree_blk_invalid, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[16], &fe1600_interrupt_tree_blk_ccs, sizeof(soc_interrupt_tree_t));         
    memcpy(&fe1600_interrupt_tree[17], &fe1600_interrupt_tree_blk_ccs, sizeof(soc_interrupt_tree_t));         
    memcpy(&fe1600_interrupt_tree[18], &fe1600_interrupt_tree_blk_rtp, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[19], &fe1600_interrupt_tree_blk_invalid, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[20], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[21], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[22], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[23], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[24], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[25], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[26], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[27], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[28], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[29], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[30], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[31], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[32], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[33], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[34], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[35], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[36], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[37], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[38], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[39], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[40], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[41], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[42], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[43], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[44], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[45], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[46], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[47], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[48], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[49], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[50], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[51], &fe1600_interrupt_tree_blk_fmac, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[52], &fe1600_interrupt_tree_blk_fsrd, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[53], &fe1600_interrupt_tree_blk_fsrd, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[54], &fe1600_interrupt_tree_blk_fsrd, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[55], &fe1600_interrupt_tree_blk_fsrd, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[56], &fe1600_interrupt_tree_blk_fsrd, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[57], &fe1600_interrupt_tree_blk_fsrd, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[58], &fe1600_interrupt_tree_blk_fsrd, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[59], &fe1600_interrupt_tree_blk_fsrd, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[60], &fe1600_interrupt_tree_blk_invalid, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[61], &fe1600_interrupt_tree_blk_invalid, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[62], &fe1600_interrupt_tree_blk_invalid, sizeof(soc_interrupt_tree_t));
    memcpy(&fe1600_interrupt_tree[63], &fe1600_interrupt_tree_blk_invalid, sizeof(soc_interrupt_tree_t));

    
    for (intr_id = 0; intr_id <= FE1600_INT_LAST; ++intr_id) {
        fe1600_interrupts[intr_id].cnt_reg      = INVALIDr;  
    }
    
#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].name = "Dch0Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].id = FE1600_INT_ECI_DCH0INT;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].field = DCH_0_INTf;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].mask_field = DCH_0_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH0INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH0INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH0INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH0INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH0INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].name = "Dch1Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].id = FE1600_INT_ECI_DCH1INT;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].field = DCH_1_INTf;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].mask_field = DCH_1_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH1INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH1INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH1INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH1INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH1INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].name = "Dch2Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].id = FE1600_INT_ECI_DCH2INT;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].field = DCH_2_INTf;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].mask_field = DCH_2_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH2INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH2INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH2INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH2INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH2INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].name = "Dch3Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].id = FE1600_INT_ECI_DCH3INT;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].field = DCH_3_INTf;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].mask_field = DCH_3_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH3INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH3INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH3INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH3INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH3INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].name = "Dcl0Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].id = FE1600_INT_ECI_DCL0INT;
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].field = DCL_0_INTf;
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].mask_field = DCL_0_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL0INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL0INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL0INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCL0INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL0INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].name = "Dcl1Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].id = FE1600_INT_ECI_DCL1INT;
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].field = DCL_1_INTf;
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].mask_field = DCL_1_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL1INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL1INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL1INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCL1INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL1INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].name = "Dcl2Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].id = FE1600_INT_ECI_DCL2INT;
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].field = DCL_2_INTf;
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].mask_field = DCL_2_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL2INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL2INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL2INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCL2INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL2INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].name = "Dcl3Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].id = FE1600_INT_ECI_DCL3INT;
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].field = DCL_3_INTf;
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].mask_field = DCL_3_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL3INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL3INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL3INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCL3INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCL3INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].name = "Dcm0Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].id = FE1600_INT_ECI_DCM0INT;
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].field = DCM_0_INTf;
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].mask_field = DCM_0_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM0INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM0INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM0INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCM0INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM0INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].name = "Dcm1Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].id = FE1600_INT_ECI_DCM1INT;
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].field = DCM_1_INTf;
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].mask_field = DCM_1_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM1INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM1INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM1INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCM1INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM1INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].name = "Dcm2Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].id = FE1600_INT_ECI_DCM2INT;
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].field = DCM_2_INTf;
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].mask_field = DCM_2_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM2INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM2INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM2INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCM2INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM2INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].name = "Dcm3Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].id = FE1600_INT_ECI_DCM3INT;
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].field = DCM_3_INTf;
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].mask_field = DCM_3_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM3INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM3INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM3INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCM3INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCM3INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].name = "Ccs0Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].id = FE1600_INT_ECI_CCS0INT;
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].field = CCS_0_INTf;
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].mask_field = CCS_0_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_CCS0INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_CCS0INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_CCS0INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_CCS0INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_CCS0INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].name = "Ccs1Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].id = FE1600_INT_ECI_CCS1INT;
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].field = CCS_1_INTf;
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].mask_field = CCS_1_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_CCS1INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_CCS1INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_CCS1INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_CCS1INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_CCS1INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].name = "RtpInt";
#endif
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].id = FE1600_INT_ECI_RTPINT;
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].field = RTP_INTf;
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].mask_field = RTP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_RTPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_RTPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_RTPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_RTPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_RTPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MACINT].name = "MacInt";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MACINT].id = FE1600_INT_ECI_MACINT;
    fe1600_interrupts[FE1600_INT_ECI_MACINT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MACINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MACINT].field = MAC_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MACINT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MACINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MACINT].mask_field = MAC_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MACINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MACINT].vector_id = 1;
    fe1600_interrupts[FE1600_INT_ECI_MACINT].vector_info = &fe1600_interrupt_tree_vecotr_eci_mac;
    fe1600_interrupts[FE1600_INT_ECI_MACINT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MACINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MACINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MACINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MACINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MACINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MACINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MACINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MACINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));

#ifdef BCM_88754_A0

    if (SOC_IS_BCM88754_A0(unit))
    { 
#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].name = "Mas0MswaInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].id = FE1600_INT_ECI_MAS0MSWAINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].name = "Mas0MswbInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].id = FE1600_INT_ECI_MAS0MSWBINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].name = "Mas1MswaInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].id = FE1600_INT_ECI_MAS1MSWAINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].name = "Mas1MswbInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].id = FE1600_INT_ECI_MAS1MSWBINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].name = "Mas2MswaInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].id = FE1600_INT_ECI_MAS2MSWAINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].name = "Mas2MswbInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].id = FE1600_INT_ECI_MAS2MSWBINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].name = "Mas3MswaInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].id = FE1600_INT_ECI_MAS3MSWAINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].name = "Mas3MswbInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].id = FE1600_INT_ECI_MAS3MSWBINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].storm_detection_start_time  = NULL;

    } else 
#endif 

    {
#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].name = "Mas0MswaInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].id = FE1600_INT_ECI_MAS0MSWAINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].reg = ECI_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].field = MAS_0_MSWA_INTf;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].mask_field = MAS_0_MSWA_INT_MASKf;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].interrupt_clear = NULL; 
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MSWAINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].name = "Mas0MswbInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].id = FE1600_INT_ECI_MAS0MSWBINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].reg = ECI_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].field = MAS_0_MSWB_INTf;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].mask_field = MAS_0_MSWB_INT_MASKf;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].interrupt_clear = NULL; 
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MSWBINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].name = "Mas1MswaInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].id = FE1600_INT_ECI_MAS1MSWAINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].reg = ECI_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].field = MAS_1_MSWA_INTf;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].mask_field = MAS_1_MSWA_INT_MASKf;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].interrupt_clear = NULL; 
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MSWAINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].name = "Mas1MswbInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].id = FE1600_INT_ECI_MAS1MSWBINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].reg = ECI_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].field = MAS_1_MSWB_INTf;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].mask_field = MAS_1_MSWB_INT_MASKf;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].interrupt_clear = NULL; 
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MSWBINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].name = "Mas2MswaInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].id = FE1600_INT_ECI_MAS2MSWAINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].reg = ECI_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].field = MAS_2_MSWA_INTf;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].mask_field = MAS_2_MSWA_INT_MASKf;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].interrupt_clear = NULL; 
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MSWAINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].name = "Mas2MswbInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].id = FE1600_INT_ECI_MAS2MSWBINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].reg = ECI_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].field = MAS_2_MSWB_INTf;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].mask_field = MAS_2_MSWB_INT_MASKf;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].interrupt_clear = NULL; 
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MSWBINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].name = "Mas3MswaInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].id = FE1600_INT_ECI_MAS3MSWAINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].reg = ECI_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].field = MAS_3_MSWA_INTf;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].mask_field = MAS_3_MSWA_INT_MASKf;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].interrupt_clear = NULL; 
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MSWAINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].name = "Mas3MswbInt";
#endif
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].id = FE1600_INT_ECI_MAS3MSWBINT;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].reg = ECI_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].field = MAS_3_MSWB_INTf;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].mask_field = MAS_3_MSWB_INT_MASKf;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].interrupt_clear = NULL; 
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MSWBINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    }


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].name = "EciInternalInt";
#endif
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].id = FE1600_INT_ECI_ECIINTERNALINT;
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].reg = ECI_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].field = ECI_INTERNAL_INTf;
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].mask_field = ECI_INTERNAL_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].vector_id = 1;
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].vector_info = &fe1600_interrupt_tree_vecotr_eciint;
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_ECIINTERNALINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].name = "Mas0Mac0Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].id = FE1600_INT_ECI_MAS0MAC0INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].field = MAS_0_MAC_0_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].mask_field = MAS_0_MAC_0_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC0INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].name = "Mas0Mac1Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].id = FE1600_INT_ECI_MAS0MAC1INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].field = MAS_0_MAC_1_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].mask_field = MAS_0_MAC_1_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC1INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].name = "Mas0Mac2Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].id = FE1600_INT_ECI_MAS0MAC2INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].field = MAS_0_MAC_2_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].mask_field = MAS_0_MAC_2_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC2INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].name = "Mas0Mac3Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].id = FE1600_INT_ECI_MAS0MAC3INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].field = MAS_0_MAC_3_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].mask_field = MAS_0_MAC_3_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC3INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].name = "Mas0Mac4Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].id = FE1600_INT_ECI_MAS0MAC4INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].field = MAS_0_MAC_4_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].mask_field = MAS_0_MAC_4_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC4INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].name = "Mas0Mac5Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].id = FE1600_INT_ECI_MAS0MAC5INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].field = MAS_0_MAC_5_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].mask_field = MAS_0_MAC_5_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC5INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].name = "Mas0Mac6Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].id = FE1600_INT_ECI_MAS0MAC6INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].field = MAS_0_MAC_6_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].mask_field = MAS_0_MAC_6_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC6INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].name = "Mas0Mac7Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].id = FE1600_INT_ECI_MAS0MAC7INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].field = MAS_0_MAC_7_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].mask_field = MAS_0_MAC_7_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS0MAC7INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].name = "Mas1Mac0Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].id = FE1600_INT_ECI_MAS1MAC0INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].field = MAS_1_MAC_0_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].mask_field = MAS_1_MAC_0_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC0INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].name = "Mas1Mac1Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].id = FE1600_INT_ECI_MAS1MAC1INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].field = MAS_1_MAC_1_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].mask_field = MAS_1_MAC_1_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC1INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].name = "Mas1Mac2Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].id = FE1600_INT_ECI_MAS1MAC2INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].field = MAS_1_MAC_2_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].mask_field = MAS_1_MAC_2_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC2INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].name = "Mas1Mac3Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].id = FE1600_INT_ECI_MAS1MAC3INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].field = MAS_1_MAC_3_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].mask_field = MAS_1_MAC_3_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC3INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].name = "Mas1Mac4Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].id = FE1600_INT_ECI_MAS1MAC4INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].field = MAS_1_MAC_4_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].mask_field = MAS_1_MAC_4_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC4INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].name = "Mas1Mac5Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].id = FE1600_INT_ECI_MAS1MAC5INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].field = MAS_1_MAC_5_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].mask_field = MAS_1_MAC_5_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC5INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].name = "Mas1Mac6Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].id = FE1600_INT_ECI_MAS1MAC6INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].field = MAS_1_MAC_6_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].mask_field = MAS_1_MAC_6_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC6INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].name = "Mas1Mac7Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].id = FE1600_INT_ECI_MAS1MAC7INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].field = MAS_1_MAC_7_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].mask_field = MAS_1_MAC_7_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS1MAC7INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].name = "Mas2Mac0Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].id = FE1600_INT_ECI_MAS2MAC0INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].field = MAS_2_MAC_0_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].mask_field = MAS_2_MAC_0_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC0INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].name = "Mas2Mac1Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].id = FE1600_INT_ECI_MAS2MAC1INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].field = MAS_2_MAC_1_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].mask_field = MAS_2_MAC_1_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC1INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].name = "Mas2Mac2Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].id = FE1600_INT_ECI_MAS2MAC2INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].field = MAS_2_MAC_2_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].mask_field = MAS_2_MAC_2_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC2INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].name = "Mas2Mac3Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].id = FE1600_INT_ECI_MAS2MAC3INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].field = MAS_2_MAC_3_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].mask_field = MAS_2_MAC_3_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC3INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].name = "Mas2Mac4Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].id = FE1600_INT_ECI_MAS2MAC4INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].field = MAS_2_MAC_4_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].mask_field = MAS_2_MAC_4_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC4INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].name = "Mas2Mac5Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].id = FE1600_INT_ECI_MAS2MAC5INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].field = MAS_2_MAC_5_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].mask_field = MAS_2_MAC_5_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC5INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].name = "Mas2Mac6Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].id = FE1600_INT_ECI_MAS2MAC6INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].field = MAS_2_MAC_6_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].mask_field = MAS_2_MAC_6_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC6INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].name = "Mas2Mac7Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].id = FE1600_INT_ECI_MAS2MAC7INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].field = MAS_2_MAC_7_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].mask_field = MAS_2_MAC_7_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS2MAC7INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].name = "Mas3Mac0Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].id = FE1600_INT_ECI_MAS3MAC0INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].field = MAS_3_MAC_0_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].mask_field = MAS_3_MAC_0_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC0INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].name = "Mas3Mac1Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].id = FE1600_INT_ECI_MAS3MAC1INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].field = MAS_3_MAC_1_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].mask_field = MAS_3_MAC_1_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC1INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].name = "Mas3Mac2Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].id = FE1600_INT_ECI_MAS3MAC2INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].field = MAS_3_MAC_2_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].mask_field = MAS_3_MAC_2_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC2INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].name = "Mas3Mac3Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].id = FE1600_INT_ECI_MAS3MAC3INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].field = MAS_3_MAC_3_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].mask_field = MAS_3_MAC_3_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC3INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].name = "Mas3Mac4Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].id = FE1600_INT_ECI_MAS3MAC4INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].field = MAS_3_MAC_4_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].mask_field = MAS_3_MAC_4_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC4INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].name = "Mas3Mac5Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].id = FE1600_INT_ECI_MAS3MAC5INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].field = MAS_3_MAC_5_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].mask_field = MAS_3_MAC_5_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC5INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].name = "Mas3Mac6Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].id = FE1600_INT_ECI_MAS3MAC6INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].field = MAS_3_MAC_6_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].mask_field = MAS_3_MAC_6_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC6INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].name = "Mas3Mac7Int";
#endif
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].id = FE1600_INT_ECI_MAS3MAC7INT;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].reg = ECI_MAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].field = MAS_3_MAC_7_INTf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].mask_reg = ECI_MAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].mask_field = MAS_3_MAC_7_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_MAS3MAC7INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_SERINT].name = "SerInt";
#endif
    fe1600_interrupts[FE1600_INT_ECI_SERINT].id = FE1600_INT_ECI_SERINT;
    fe1600_interrupts[FE1600_INT_ECI_SERINT].reg = ECI_ECI_INTERNAL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_SERINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_SERINT].field = SER_INTf;
    fe1600_interrupts[FE1600_INT_ECI_SERINT].mask_reg = ECI_ECI_INTERNAL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_SERINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_SERINT].mask_field = SER_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_SERINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_SERINT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_SERINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_SERINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_SERINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_SERINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_SERINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_SERINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_SERINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_SERINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].name = "OccgInt";
#endif
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].id = FE1600_INT_ECI_OCCGINT;
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].reg = ECI_ECI_INTERNAL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].field = OCCG_INTf;
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].mask_reg = ECI_ECI_INTERNAL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].mask_field = OCCG_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_OCCGINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_OCCGINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_OCCGINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_OCCGINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_OCCGINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].name = "Dch0IntForce";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].id = FE1600_INT_ECI_DCH0INTFORCE;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].reg = ECI_INTERRUPT_FORCE_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].field = DCH_0_INT_FORCEf;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].mask_field = DCH_0_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].interrupt_clear = soc_interrupt_clear_on_clear;
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH0INTFORCE].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].name = "Dch1IntForce";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].id = FE1600_INT_ECI_DCH1INTFORCE;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].reg = ECI_INTERRUPT_FORCE_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].field = DCH_1_INT_FORCEf;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].mask_field = DCH_1_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].interrupt_clear = soc_interrupt_clear_on_clear;
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH1INTFORCE].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].name = "Dch2IntForce";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].id = FE1600_INT_ECI_DCH2INTFORCE;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].reg = ECI_INTERRUPT_FORCE_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].field = DCH_2_INT_FORCEf;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].mask_field = DCH_2_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].interrupt_clear = soc_interrupt_clear_on_clear;
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH2INTFORCE].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].name = "Dch3IntForce";
#endif
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].id = FE1600_INT_ECI_DCH3INTFORCE;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].reg = ECI_INTERRUPT_FORCE_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].field = DCH_3_INT_FORCEf;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].mask_reg = ECI_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].mask_field = DCH_3_INT_MASKf;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].interrupt_clear = soc_interrupt_clear_on_clear;
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_ECI_DCH3INTFORCE].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_ECI * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].name = "TestModeCmdFinishInt";
#endif
    fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].id = FE1600_INT_OCCG_TESTMODECMDFINISHINT;
    fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].reg = OCCG_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].field = TEST_MODE_CMD_FINISH_INTf;
    fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].mask_reg = OCCG_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].mask_field = TEST_MODE_CMD_FINISH_INT_MASKf;
    fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_OCCG * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_OCCG * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_OCCG * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_OCCG * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_OCCG * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_OCCG * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_OCCG * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_OCCG_TESTMODECMDFINISHINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_OCCG * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].name = "DcHdesccntop";
#endif
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].id = FE1600_INT_DCH_DCHDESCCNTOP;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].field = DCH_PDESCCNTOf;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].mask_field = DC_HUDESCCNTO_A_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOP].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].name = "DcHUnExpCellP";
#endif
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].id = FE1600_INT_DCH_DCHUNEXPCELLP;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].field = DCH_UN_EXP_CELL_Pf;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].mask_field = DC_HU_UN_EXP_CELL_P_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLP].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));

#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].name = "IfmafoPInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].id = FE1600_INT_DCH_IFMAFOPINT;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].field = IFMAFO_P_INTf;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].mask_field = IFMAFO_P_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMAFOPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].name = "IfmbfoPInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].id = FE1600_INT_DCH_IFMBFOPINT;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].field = IFMBFO_P_INTf;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].mask_field = IFMBFO_P_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMBFOPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].name = "CpudatacellfneA0PInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].id = FE1600_INT_DCH_CPUDATACELLFNEA0PINT;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].field = CPUDATACELLFNE_A_0_P_INTf;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].mask_field = CPUDATACELLFNE_A_0_P_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].interrupt_clear = soc_interrupt_clear_on_read_fifo; 
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].interrupt_clear_param1 = (void*)(&FE1600_INT_DCH_CPUDATACELLFNEA0PINT_read_fifo);
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].interrupt_clear_param2 = NULL;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0PINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].name = "CpudatacellfneA1PInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].id = FE1600_INT_DCH_CPUDATACELLFNEA1PINT;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].field = CPUDATACELLFNE_A_1_P_INTf;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].mask_field = CPUDATACELLFNE_A_1_P_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].interrupt_clear = soc_interrupt_clear_on_read_fifo; 
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].interrupt_clear_param1 = (void*)(&FE1600_INT_DCH_CPUDATACELLFNEA1PINT_read_fifo);
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].interrupt_clear_param2 = NULL;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1PINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].name = "AltoPInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].id = FE1600_INT_DCH_ALTOPINT;
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].field = ALTO_P_INTf;
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].mask_field = ALTO_P_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ALTOPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].name = "UnreachDestEvPInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].id = FE1600_INT_DCH_UNREACHDESTEVPINT;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].field = UNREACH_DEST_EV_P_INTf;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].mask_field = UNREACH_DEST_EV_P_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].name = "DrpaLinkActvErrPInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].id = FE1600_INT_DCH_DRPALINKACTVERRPINT;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].field = DRPA_LINK_ACTV_ERR_P_INTf;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].mask_field = DRPA_LINK_ACTV_ERR_P_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].name = "DrpbLinkActvErrPInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].id = FE1600_INT_DCH_DRPBLINKACTVERRPINT;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].field = DRPB_LINK_ACTV_ERR_P_INTf;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].mask_field = DRPB_LINK_ACTV_ERR_P_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].name = "ErrorFilterPInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].id = FE1600_INT_DCH_ERRORFILTERPINT;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].field = ERROR_FILTER_P_INTf;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].mask_field = ERROR_FILTER_P_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].interrupt_clear = soc_interrupt_clear_on_read_fifo; 
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].interrupt_clear_param1 =  (void*)(&FE1600_INT_DCH_ERRORFILTERPINT_read_fifo);
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].name = "IntReg1Int";
#endif
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].id = FE1600_INT_DCH_INTREG1INT;
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].field = INT_REG_1_INTf;
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].mask_field = INT_REG_1_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].vector_id = 1;
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].vector_info = &fe1600_interrupt_tree_vecotr_dch_int1;
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_INTREG1INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].name = "IntReg2Int";
#endif
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].id = FE1600_INT_DCH_INTREG2INT;
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].field = INT_REG_2_INTf;
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].mask_field = INT_REG_2_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].vector_id = 1;
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].vector_info = &fe1600_interrupt_tree_vecotr_dch_int2;
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_INTREG2INT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].name = "DchUnExpError";
#endif
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].id = FE1600_INT_DCH_DCHUNEXPERROR;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].reg = DCH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].field = DCH_UN_EXP_ERRORf;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].mask_field = LINK_OTCR_DROP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPERROR].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));

#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].name = "DcHUnExpCellS";
#endif
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].id = FE1600_INT_DCH_DCHUNEXPCELLS;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].reg = DCH_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].field = DCH_UN_EXP_CELL_Sf;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].mask_reg = DCH_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].mask_field = DC_HM_UN_EXP_CELL_S_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHUNEXPCELLS].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));



#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].name = "DcHdesccntos";
#endif
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].id = FE1600_INT_DCH_DCHDESCCNTOS;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].reg = DCH_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].field = DCH_SDESCCNTOf;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].mask_reg = DCH_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].mask_field = DC_HMDESCCNTO_A_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHDESCCNTOS].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].name = "IfmafoSInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].id = FE1600_INT_DCH_IFMAFOSINT;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].reg = DCH_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].field = IFMAFO_S_INTf;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].mask_reg = DCH_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].mask_field = IFMAFO_S_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMAFOSINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].name = "IfmbfoSInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].id = FE1600_INT_DCH_IFMBFOSINT;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].reg = DCH_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].field = IFMBFO_S_INTf;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].mask_reg = DCH_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].mask_field = IFMBFO_S_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_IFMBFOSINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].name = "CpudatacellfneA0SInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].id = FE1600_INT_DCH_CPUDATACELLFNEA0SINT;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].reg = DCH_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].field = CPUDATACELLFNE_A_0_S_INTf;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].mask_reg = DCH_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].mask_field = CPUDATACELLFNE_A_0_S_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].interrupt_clear = soc_interrupt_clear_on_read_fifo; 
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].interrupt_clear_param1 = (void*)(&FE1600_INT_DCH_CPUDATACELLFNEA0SINT_read_fifo);
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].interrupt_clear_param2 = NULL;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA0SINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].name = "CpudatacellfneA1SInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].id = FE1600_INT_DCH_CPUDATACELLFNEA1SINT;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].reg = DCH_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].field = CPUDATACELLFNE_A_1_S_INTf;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].mask_reg = DCH_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].mask_field = CPUDATACELLFNE_A_1_S_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].interrupt_clear = soc_interrupt_clear_on_read_fifo; 
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].interrupt_clear_param1 = (void*)(&FE1600_INT_DCH_CPUDATACELLFNEA1SINT_read_fifo);
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].interrupt_clear_param2 = NULL;
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_CPUDATACELLFNEA1SINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].name = "AltoSInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].id = FE1600_INT_DCH_ALTOSINT;
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].reg = DCH_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].field = ALTO_S_INTf;
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].mask_reg = DCH_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].mask_field = ALTO_S_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ALTOSINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].name = "UnreachDestEvSInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].id = FE1600_INT_DCH_UNREACHDESTEVSINT;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].reg = DCH_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].field = UNREACH_DEST_EV_S_INTf;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].mask_reg = DCH_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].mask_field = UNREACH_DEST_EV_S_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_UNREACHDESTEVSINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].name = "DrpaLinkActvErrSInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].id = FE1600_INT_DCH_DRPALINKACTVERRSINT;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].reg = DCH_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].field = DRPA_LINK_ACTV_ERR_S_INTf;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].mask_reg = DCH_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].mask_field = DRPA_LINK_ACTV_ERR_S_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPALINKACTVERRSINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].name = "DrpbLinkActvErrSInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].id = FE1600_INT_DCH_DRPBLINKACTVERRSINT;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].reg = DCH_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].field = DRPB_LINK_ACTV_ERR_S_INTf;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].mask_reg = DCH_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].mask_field = DRPB_LINK_ACTV_ERR_S_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_DRPBLINKACTVERRSINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].name = "ErrorFilterSInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].id = FE1600_INT_DCH_ERRORFILTERSINT;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].reg = DCH_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].field = ERROR_FILTER_S_INTf;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].mask_reg = DCH_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].mask_field = ERROR_FILTER_S_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].interrupt_clear = soc_interrupt_clear_on_read_fifo; 
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].interrupt_clear_param1 =  (void*)(&FE1600_INT_DCH_ERRORFILTERSINT_read_fifo);
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ERRORFILTERSINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].name = "OutOfSyncIntP";
#endif
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].id = FE1600_INT_DCH_OUTOFSYNCINTP;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].reg = DCH_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].field = OUT_OF_SYNC_INT_Pf;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].mask_reg = DCH_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].mask_field = OUT_OF_SYNC_INT_P_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTP].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].name = "OutOfSyncIntS";
#endif
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].id = FE1600_INT_DCH_OUTOFSYNCINTS;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].reg = DCH_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].field = OUT_OF_SYNC_INT_Sf;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].mask_reg = DCH_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].mask_field = OUT_OF_SYNC_INT_S_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_OUTOFSYNCINTS].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].name = "DCH_Ecc_1bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].id = FE1600_INT_DCH_ECC_1BERRINT;
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].reg = DCH_INTERRUPT_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].field = ECC_1B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].mask_reg = DCH_INTERRUPT_MASK_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].mask_field = ECC_1B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].cnt_reg         = DCH_ECC_1B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ECC_1BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].name = "DCH_Ecc_2bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].id = FE1600_INT_DCH_ECC_2BERRINT;
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].reg = DCH_INTERRUPT_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].field = ECC_2B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].mask_reg = DCH_INTERRUPT_MASK_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].mask_field = ECC_2B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].cnt_reg         = DCH_ECC_2B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32)); 
    fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_ECC_2BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].name = "DCH_ParityErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].id = FE1600_INT_DCH_PARITYERRINT;
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].reg = DCH_INTERRUPT_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].field = PARITY_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].mask_reg = DCH_INTERRUPT_MASK_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].mask_field = PARITY_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].cnt_reg         = DCH_PARITY_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCH_PARITYERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));


 #ifdef BCM_88754_A0
    
    if (SOC_IS_BCM88754_A0(unit))
    {
#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].name = "DchCellLTypeErrInt";
#endif
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].id = FE1600_INT_DCH_CELLTYPEERRINT;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].reg = DCH_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].reg_index = 0;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].field = CELL_TYPE_ERR_INTf;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].mask_field = CELL_TYPE_ERR_INT_MASKf;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].name = "DchPnExpError2";
#endif
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].id = FE1600_INT_DCH_DCHPNEXPERROR2;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].reg = DCH_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].reg_index = 0;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].field = DCH_PN_EXP_ERROR_2f;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].mask_field = DCH_PN_EXP_ERROR_2_INT_MASKf;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].name = "DchSnExpError2";
#endif
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].id = FE1600_INT_DCH_DCHSNEXPERROR2;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].reg = DCH_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].reg_index = 0;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].field = DCH_SN_EXP_ERROR_2f;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].mask_reg = DCH_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].mask_field = DCH_SN_EXP_ERROR_2_INT_MASKf;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCH * sizeof(uint32));

    } else 
#endif    
    {
#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].name = "DchCellLTypeErrInt";
#endif
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].id = FE1600_INT_DCH_CELLTYPEERRINT;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_DCH_CELLTYPEERRINT].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].name = "DchPnExpError2";
#endif
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].id = FE1600_INT_DCH_DCHPNEXPERROR2;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_DCH_DCHPNEXPERROR2].storm_detection_start_time  = NULL;
#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].name = "DchSnExpError2";
#endif
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].id = FE1600_INT_DCH_DCHSNEXPERROR2;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_DCH_DCHSNEXPERROR2].storm_detection_start_time  = NULL;
    }


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].name = "CCP0SrcDvCngLinkInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].id = FE1600_INT_DCL_CCP0SRCDVCNGLINKINT;
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].field = CCP_0_SRC_DV_CNG_LINK_INTf;
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].mask_field = CCP_0_SRC_DV_CNG_LINK_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].interrupt_clear = soc_interrupt_clear_on_read_fifo; 
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].interrupt_clear_param1 = (void*)(&FE1600_INT_DCL_CCPSRCDCNGLINK_read_fifo);
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].interrupt_clear_param2 = NULL;
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0SRCDVCNGLINKINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].name = "CCP1SrcDvCngLinkInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].id = FE1600_INT_DCL_CCP1SRCDVCNGLINKINT;
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].field = CCP_1_SRC_DV_CNG_LINK_INTf;
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].mask_field = CCP_1_SRC_DV_CNG_LINK_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].interrupt_clear = soc_interrupt_clear_on_read_fifo; 
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].interrupt_clear_param1 = (void*)(&FE1600_INT_DCL_CCPSRCDCNGLINK_read_fifo);
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].interrupt_clear_param2 = NULL;
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1SRCDVCNGLINKINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].name = "CCP0IllCellInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].id = FE1600_INT_DCL_CCP0ILLCELLINT;
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].field = CCP_0_ILL_CELL_INTf;
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].mask_field = CCP_0_ILL_CELL_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0ILLCELLINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].name = "CCP1IllCellInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].id = FE1600_INT_DCL_CCP1ILLCELLINT;
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].field = CCP_1_ILL_CELL_INTf;
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].mask_field = CCP_1_ILL_CELL_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1ILLCELLINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].name = "CCP0ChfOvfInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].id = FE1600_INT_DCL_CCP0CHFOVFINT;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].field = CCP_0_CHF_OVF_INTf;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].mask_field = CCP_0_CHF_OVF_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0CHFOVFINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].name = "CCP0ClfOvfInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].id = FE1600_INT_DCL_CCP0CLFOVFINT;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].field = CCP_0_CLF_OVF_INTf;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].mask_field = CCP_0_CLF_OVF_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP0CLFOVFINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].name = "CCP1ChfOvfInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].id = FE1600_INT_DCL_CCP1CHFOVFINT;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].field = CCP_1_CHF_OVF_INTf;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].mask_field = CCP_1_CHF_OVF_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1CHFOVFINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].name = "CCP1ClfOvfInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].id = FE1600_INT_DCL_CCP1CLFOVFINT;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].field = CCP_1_CLF_OVF_INTf;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].mask_field = CCP_1_CLF_OVF_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_CCP1CLFOVFINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].name = "DcluTagParErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].id = FE1600_INT_DCL_DCLUTAGPARERRINT;
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].field = DCLU_TAG_PAR_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].mask_field = DCLU_TAG_PAR_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DCLUTAGPARERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].name = "DclmTagParErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].id = FE1600_INT_DCL_DCLMTAGPARERRINT;
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].field = DCLM_TAG_PAR_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].mask_field = DCLM_TAG_PAR_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DCLMTAGPARERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].name = "MacaDataCrcErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].id = FE1600_INT_DCL_MACADATACRCERRINT;
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].field = MACA_DATA_CRC_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].mask_field = MACA_DATA_CRC_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_MACADATACRCERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].name = "MacbDataCrcErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].id = FE1600_INT_DCL_MACBDATACRCERRINT;
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].field = MACB_DATA_CRC_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].mask_field = MACB_DATA_CRC_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_MACBDATACRCERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].name = "DCL_Ecc_1bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].id = FE1600_INT_DCL_ECC_1BERRINT;
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].field = ECC_1B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].mask_field = ECC_1B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].cnt_reg         = DCL_ECC_1B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_ECC_1BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].name = "DCL_Ecc_2bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].id = FE1600_INT_DCL_ECC_2BERRINT;
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].field = ECC_2B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].mask_field = ECC_2B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].cnt_reg         = DCL_ECC_2B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_ECC_2BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].name = "DCL_ParityErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].id = FE1600_INT_DCL_PARITYERRINT;
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].field = PARITY_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].mask_field = PARITY_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].cnt_reg         = DCL_PARITY_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_PARITYERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].name = "DrpIpPInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].id = FE1600_INT_DCL_DRPIPPINT;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].field = DRP_IP_P_INTf;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].mask_field = DRP_IP_P_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DRPIPPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].name = "DrpIpSInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].id = FE1600_INT_DCL_DRPIPSINT;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].field = DRP_IP_S_INTf;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].mask_field = DRP_IP_S_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_DRPIPSINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].name = "OutOfSyncInt";
#endif
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].id = FE1600_INT_DCL_OUTOFSYNCINT;
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].reg = DCL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].field = OUT_OF_SYNC_INTf;
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].mask_reg = DCL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].mask_field = OUT_OF_SYNC_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCL_OUTOFSYNCINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCL * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].name = "DcmuaDrpIpInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].id = FE1600_INT_DCMA_DCMUADRPIPINT;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].reg = DCMA_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].field = DCMUA_DRP_IP_INTf;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].mask_reg = DCMA_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].mask_field = DCMUA_DRP_IP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPIPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].name = "DcmuaDrpInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].id = FE1600_INT_DCMA_DCMUADRPINT;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].reg = DCMA_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].field = DCMUA_DRP_INTf;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].mask_reg = DCMA_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].mask_field = DCMUA_DRP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMUADRPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].name = "DcmmaDrpIpInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].id = FE1600_INT_DCMA_DCMMADRPIPINT;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].reg = DCMA_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].field = DCMMA_DRP_IP_INTf;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].mask_reg = DCMA_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].mask_field = DCMMA_DRP_IP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPIPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].name = "DcmmaDrpInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].id = FE1600_INT_DCMA_DCMMADRPINT;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].reg = DCMA_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].field = DCMMA_DRP_INTf;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].mask_reg = DCMA_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].mask_field = DCMMA_DRP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_DCMMADRPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].name = "DCMA_Ecc_1bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].id = FE1600_INT_DCMA_ECC_1BERRINT;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].reg = DCMA_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].field = ECC_1B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].mask_reg = DCMA_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].mask_field = ECC_1B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].cnt_reg         = DCMA_ECC_1B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_ECC_1BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].name = "DCMA_Ecc_2bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].id = FE1600_INT_DCMA_ECC_2BERRINT;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].reg = DCMA_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].field = ECC_2B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].mask_reg = DCMA_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].mask_field = ECC_2B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].cnt_reg         = DCMA_ECC_2B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_ECC_2BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].name = "DCMA_ParityErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].id = FE1600_INT_DCMA_PARITYERRINT;
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].reg = DCMA_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].field = PARITY_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].mask_reg = DCMA_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].mask_field = PARITY_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].cnt_reg         = DCMA_PARITY_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_PARITYERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].name = "DCMA_OutOfSyncPInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].id = FE1600_INT_DCMA_OUTOFSYNCPINT;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].reg = DCMA_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].field = OUT_OF_SYNC_P_INTf;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].mask_reg = DCMA_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].mask_field = OUT_OF_SYNC_P_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].name = "DCMA_OutOfSyncSInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].id = FE1600_INT_DCMA_OUTOFSYNCSINT;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].reg = DCMA_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].field = OUT_OF_SYNC_S_INTf;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].mask_reg = DCMA_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].mask_field = OUT_OF_SYNC_S_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMA_OUTOFSYNCSINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMA * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].name = "DcmubDrpIpInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].id = FE1600_INT_DCMB_DCMUBDRPIPINT;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].reg = DCMB_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].field = DCMUB_DRP_IP_INTf;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].mask_reg = DCMB_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].mask_field = DCMUB_DRP_IP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPIPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].name = "DcmubDrpInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].id = FE1600_INT_DCMB_DCMUBDRPINT;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].reg = DCMB_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].field = DCMUB_DRP_INTf;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].mask_reg = DCMB_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].mask_field = DCMUB_DRP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMUBDRPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].name = "DcmmbDrpIpInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].id = FE1600_INT_DCMB_DCMMBDRPIPINT;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].reg = DCMB_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].field = DCMMB_DRP_IP_INTf;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].mask_reg = DCMB_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].mask_field = DCMMB_DRP_IP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPIPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].name = "DcmmbDrpInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].id = FE1600_INT_DCMB_DCMMBDRPINT;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].reg = DCMB_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].field = DCMMB_DRP_INTf;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].mask_reg = DCMB_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].mask_field = DCMMB_DRP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_DCMMBDRPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].name = "DCMB_Ecc_1bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].id = FE1600_INT_DCMB_ECC_1BERRINT;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].reg = DCMB_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].field = ECC_1B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].mask_reg = DCMB_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].mask_field = ECC_1B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].cnt_reg         = DCMB_ECC_1B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_ECC_1BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].name = "DCMB_Ecc_2bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].id = FE1600_INT_DCMB_ECC_2BERRINT;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].reg = DCMB_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].field = ECC_2B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].mask_reg = DCMB_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].mask_field = ECC_2B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].cnt_reg         = DCMB_ECC_2B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_ECC_2BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].name = "DCMB_ParityErrInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].id = FE1600_INT_DCMB_PARITYERRINT;
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].reg = DCMB_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].field = PARITY_ERR_INTf;
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].mask_reg = DCMB_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].mask_field = PARITY_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].cnt_reg         = DCMB_PARITY_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_PARITYERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].name = "DCMB_OutOfSyncPInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].id = FE1600_INT_DCMB_OUTOFSYNCPINT;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].reg = DCMB_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].field = OUT_OF_SYNC_P_INTf;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].mask_reg = DCMB_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].mask_field = OUT_OF_SYNC_P_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].name = "DCMB_OutOfSyncSInt";
#endif
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].id = FE1600_INT_DCMB_OUTOFSYNCSINT;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].reg = DCMB_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].field = OUT_OF_SYNC_S_INTf;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].mask_reg = DCMB_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].mask_field = OUT_OF_SYNC_S_INT_MASKf;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_DCMB_OUTOFSYNCSINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_DCMB * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].name = "CpuCaptCellFneInt";
#endif
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].id = FE1600_INT_CCS_CPUCAPTCELLFNEINT;
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].reg = CCS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].field = CPU_CAPT_CELL_FNE_INTf;
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].mask_reg = CCS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].mask_field = CPU_CAPT_CELL_FNE_INT_MASKf;
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].interrupt_clear = soc_interrupt_clear_on_read_fifo; 
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].interrupt_clear_param1 = (void*)(&FE1600_INT_CCS_CPUCAPTCELLFNEINT_read_fifo);
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].interrupt_clear_param2 = NULL;
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_CPUCAPTCELLFNEINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].name = "UnrchDstInt";
#endif
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].id = FE1600_INT_CCS_UNRCHDSTINT;
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].reg = CCS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].field = UNRCH_DST_INTf;
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].mask_reg = CCS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].mask_field = UNRCH_DST_INT_MASKf;
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_UNRCHDSTINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].name = "CdmaLpFifOvfInt";
#endif
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].id = FE1600_INT_CCS_CDMALPFIFOVFINT;
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].reg = CCS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].field = CDMA_LP_FIF_OVF_INTf;
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].mask_reg = CCS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].mask_field = CDMA_LP_FIF_OVF_INT_MASKf;
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_CDMALPFIFOVFINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].name = "CdmbLpFifOvfInt";
#endif
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].id = FE1600_INT_CCS_CDMBLPFIFOVFINT;
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].reg = CCS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].field = CDMB_LP_FIF_OVF_INTf;
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].mask_reg = CCS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].mask_field = CDMB_LP_FIF_OVF_INT_MASKf;
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_CDMBLPFIFOVFINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].name = "CCS_Ecc_1bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].id = FE1600_INT_CCS_ECC_1BERRINT;
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].reg = CCS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].field = ECC_1B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].mask_reg = CCS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].mask_field = ECC_1B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].cnt_reg         = CCS_ECC_1B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_ECC_1BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].name = "CCS_Ecc_2bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].id = FE1600_INT_CCS_ECC_2BERRINT;
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].reg = CCS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].field = ECC_2B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].mask_reg = CCS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].mask_field = ECC_2B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].cnt_reg         = CCS_ECC_2B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_ECC_2BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].name = "CCS_ParityErrInt";
#endif
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].id = FE1600_INT_CCS_PARITYERRINT;
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].reg = CCS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].field = PARITY_ERR_INTf;
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].mask_reg = CCS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].mask_field = PARITY_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].cnt_reg         = CCS_CRP_PARITY_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_CCS_PARITYERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_CCS * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].name = "drhp0IntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].id = FE1600_INT_RTP_DRHP0INTREG;
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].field = DRHP_0_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].mask_field = DRHP_0_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_drhp0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP0INTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].name = "drhp1IntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].id = FE1600_INT_RTP_DRHP1INTREG;
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].field = DRHP_1_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].mask_field = DRHP_1_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_drhp1;
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP1INTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].name = "drhp2IntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].id = FE1600_INT_RTP_DRHP2INTREG;
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].field = DRHP_2_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].mask_field = DRHP_2_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_drhp2;
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP2INTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].name = "drhp3IntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].id = FE1600_INT_RTP_DRHP3INTREG;
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].field = DRHP_3_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].mask_field = DRHP_3_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_drhp3;
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP3INTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].name = "drhs0IntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].id = FE1600_INT_RTP_DRHS0INTREG;
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].field = DRHS_0_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].mask_field = DRHS_0_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_drhs0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS0INTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].name = "drhs1IntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].id = FE1600_INT_RTP_DRHS1INTREG;
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].field = DRHS_1_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].mask_field = DRHS_1_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_drhs1;
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS1INTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].name = "drhs2IntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].id = FE1600_INT_RTP_DRHS2INTREG;
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].field = DRHS_2_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].mask_field = DRHS_2_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_drhs2;
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS2INTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].name = "drhs3IntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].id = FE1600_INT_RTP_DRHS3INTREG;
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].field = DRHS_3_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].mask_field = DRHS_3_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_drhs3;
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS3INTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].name = "crh0IntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].id = FE1600_INT_RTP_CRH0INTREG;
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].field = CRH_0_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].mask_field = CRH_0_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_crh0;
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH0INTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].name = "crh1IntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].id = FE1600_INT_RTP_CRH1INTREG;
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].field = CRH_1_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].mask_field = CRH_1_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_crh1;
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH1INTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].name = "generalIntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].id = FE1600_INT_RTP_GENERALINTREG;
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].field = GENERAL_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].mask_field = GENERAL_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_general;
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_GENERALINTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].name = "Ecc1bErrIntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].id = FE1600_INT_RTP_ECC1BERRINTREG;
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].field = ECC_1B_ERR_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].mask_field = ECC_1B_ERR_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].cnt_reg         = RTP_ECC_1B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_ecc1;
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC1BERRINTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].name = "Ecc2bErrIntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].id = FE1600_INT_RTP_ECC2BERRINTREG;
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].field = ECC_2B_ERR_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].mask_field = ECC_2B_ERR_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].cnt_reg         = RTP_ECC_2B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_ecc2;
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC2BERRINTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].name = "ParErrIntReg";
#endif
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].id = FE1600_INT_RTP_PARERRINTREG;
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].reg = RTP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].field = PAR_ERR_INT_REGf;
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].mask_reg = RTP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].mask_field = PAR_ERR_INT_MASK_REGf;
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].vector_id = 1;
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].vector_info = &fe1600_interrupt_tree_vecotr_rtp_parity;
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_PARERRINTREG].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].name = "drhp_0_QueryDestinationAbove1kInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].id = FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].field = DRHP_N_QUERY_DESTINATION_ABOVE_1K_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].mask_field = DRHP_N_QUERY_DESTINATION_ABOVE_1K_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].name = "drhp_1_QueryDestinationAbove1kInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].id = FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].field = DRHP_N_QUERY_DESTINATION_ABOVE_1K_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].mask_reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].mask_field = DRHP_N_QUERY_DESTINATION_ABOVE_1K_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].name = "drhp_2_QueryDestinationAbove1kInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].id = FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].field = DRHP_N_QUERY_DESTINATION_ABOVE_1K_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].mask_reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].mask_field = DRHP_N_QUERY_DESTINATION_ABOVE_1K_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].name = "drhp_3_QueryDestinationAbove1kInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].id = FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].field = DRHP_N_QUERY_DESTINATION_ABOVE_1K_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].mask_reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].mask_field = DRHP_N_QUERY_DESTINATION_ABOVE_1K_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].name = "drhp_0_QueryEmptyMulticastIdInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].id = FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].field = DRHP_N_QUERY_EMPTY_MULTICAST_ID_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].mask_field = DRHP_N_QUERY_EMPTY_MULTICAST_ID_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].name = "drhp_1_QueryEmptyMulticastIdInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].id = FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].field = DRHP_N_QUERY_EMPTY_MULTICAST_ID_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].mask_reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].mask_field = DRHP_N_QUERY_EMPTY_MULTICAST_ID_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].name = "drhp_2_QueryEmptyMulticastIdInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].id = FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].field = DRHP_N_QUERY_EMPTY_MULTICAST_ID_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].mask_reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].mask_field = DRHP_N_QUERY_EMPTY_MULTICAST_ID_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].name = "drhp_3_QueryEmptyMulticastIdInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].id = FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].field = DRHP_N_QUERY_EMPTY_MULTICAST_ID_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].mask_reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].mask_field = DRHP_N_QUERY_EMPTY_MULTICAST_ID_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].name = "drhp_0_QueryEmptyLinkMapInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].id = FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].field = DRHP_N_QUERY_EMPTY_LINK_MAP_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].mask_field = DRHP_N_QUERY_EMPTY_LINK_MAP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYEMPTYLINKMAPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].name = "drhp_1_QueryEmptyLinkMapInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].id = FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].field = DRHP_N_QUERY_EMPTY_LINK_MAP_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].mask_reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].mask_field = DRHP_N_QUERY_EMPTY_LINK_MAP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYEMPTYLINKMAPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].name = "drhp_2_QueryEmptyLinkMapInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].id = FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].field = DRHP_N_QUERY_EMPTY_LINK_MAP_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].mask_reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].mask_field = DRHP_N_QUERY_EMPTY_LINK_MAP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYEMPTYLINKMAPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].name = "drhp_3_QueryEmptyLinkMapInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].id = FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].field = DRHP_N_QUERY_EMPTY_LINK_MAP_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].mask_reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].mask_field = DRHP_N_QUERY_EMPTY_LINK_MAP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYEMPTYLINKMAPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].name = "drhp_0_QueryUnreachableMulticastInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].id = FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].field = DRHP_N_QUERY_UNREACHABLE_MULTICAST_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].mask_field = DRHP_N_QUERY_UNREACHABLE_MULTICAST_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].name = "drhp_1_QueryUnreachableMulticastInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].id = FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].field = DRHP_N_QUERY_UNREACHABLE_MULTICAST_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].mask_reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].mask_field = DRHP_N_QUERY_UNREACHABLE_MULTICAST_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].name = "drhp_2_QueryUnreachableMulticastInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].id = FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].field = DRHP_N_QUERY_UNREACHABLE_MULTICAST_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].mask_reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].mask_field = DRHP_N_QUERY_UNREACHABLE_MULTICAST_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].name = "drhp_3_QueryUnreachableMulticastInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].id = FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].reg = RTP_DRHP_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].field = DRHP_N_QUERY_UNREACHABLE_MULTICAST_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].mask_reg = RTP_DRHP_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].mask_reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].mask_field = DRHP_N_QUERY_UNREACHABLE_MULTICAST_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].name = "drhs_0_QueryDestinationAbove1kInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].id = FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].field = DRHS_N_QUERY_DESTINATION_ABOVE_1K_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].mask_field = DRHS_N_QUERY_DESTINATION_ABOVE_1K_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].name = "drhs_1_QueryDestinationAbove1kInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].id = FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].field = DRHS_N_QUERY_DESTINATION_ABOVE_1K_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].mask_reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].mask_field = DRHS_N_QUERY_DESTINATION_ABOVE_1K_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].name = "drhs_2_QueryDestinationAbove1kInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].id = FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].field = DRHS_N_QUERY_DESTINATION_ABOVE_1K_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].mask_reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].mask_field = DRHS_N_QUERY_DESTINATION_ABOVE_1K_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].name = "drhs_3_QueryDestinationAbove1kInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].id = FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].field = DRHS_N_QUERY_DESTINATION_ABOVE_1K_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].mask_reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].mask_field = DRHS_N_QUERY_DESTINATION_ABOVE_1K_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].name = "drhs_0_QueryEmptyMulticastIdInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].id = FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].field = DRHS_N_QUERY_EMPTY_MULTICAST_ID_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].mask_field = DRHS_N_QUERY_EMPTY_MULTICAST_ID_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].name = "drhs_1_QueryEmptyMulticastIdInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].id = FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].field = DRHS_N_QUERY_EMPTY_MULTICAST_ID_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].mask_reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].mask_field = DRHS_N_QUERY_EMPTY_MULTICAST_ID_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].name = "drhs_2_QueryEmptyMulticastIdInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].id = FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].field = DRHS_N_QUERY_EMPTY_MULTICAST_ID_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].mask_reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].mask_field = DRHS_N_QUERY_EMPTY_MULTICAST_ID_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].name = "drhs_3_QueryEmptyMulticastIdInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].id = FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].field = DRHS_N_QUERY_EMPTY_MULTICAST_ID_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].mask_reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].mask_field = DRHS_N_QUERY_EMPTY_MULTICAST_ID_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYMULTICASTIDINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].name = "drhs_0_QueryEmptyLinkMapInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].id = FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].field = DRHS_N_QUERY_EMPTY_LINK_MAP_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].mask_field = DRHS_N_QUERY_EMPTY_LINK_MAP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYEMPTYLINKMAPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].name = "drhs_1_QueryEmptyLinkMapInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].id = FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].field = DRHS_N_QUERY_EMPTY_LINK_MAP_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].mask_reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].mask_field = DRHS_N_QUERY_EMPTY_LINK_MAP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYEMPTYLINKMAPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].name = "drhs_2_QueryEmptyLinkMapInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].id = FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].field = DRHS_N_QUERY_EMPTY_LINK_MAP_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].mask_reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].mask_field = DRHS_N_QUERY_EMPTY_LINK_MAP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYEMPTYLINKMAPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].name = "drhs_3_QueryEmptyLinkMapInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].id = FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].field = DRHS_N_QUERY_EMPTY_LINK_MAP_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].mask_reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].mask_field = DRHS_N_QUERY_EMPTY_LINK_MAP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYEMPTYLINKMAPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].name = "drhs_0_QueryUnreachableMulticastInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].id = FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].field = DRHS_N_QUERY_UNREACHABLE_MULTICAST_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].mask_field = DRHS_N_QUERY_UNREACHABLE_MULTICAST_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].name = "drhs_1_QueryUnreachableMulticastInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].id = FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].field = DRHS_N_QUERY_UNREACHABLE_MULTICAST_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].mask_reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].mask_field = DRHS_N_QUERY_UNREACHABLE_MULTICAST_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].name = "drhs_2_QueryUnreachableMulticastInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].id = FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].field = DRHS_N_QUERY_UNREACHABLE_MULTICAST_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].mask_reg_index = 2;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].mask_field = DRHS_N_QUERY_UNREACHABLE_MULTICAST_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].name = "drhs_3_QueryUnreachableMulticastInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].id = FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].reg = RTP_DRHS_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].field = DRHS_N_QUERY_UNREACHABLE_MULTICAST_INTf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].mask_reg = RTP_DRHS_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].mask_reg_index = 3;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].mask_field = DRHS_N_QUERY_UNREACHABLE_MULTICAST_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].name = "crh_0_QueryDestinationAbove1kInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].id = FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].reg = RTP_CRH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].field = CRH_N_QUERY_DESTINATION_ABOVE_1K_INTf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].mask_reg = RTP_CRH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].mask_field = CRH_N_QUERY_DESTINATION_ABOVE_1K_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].name = "crh_1_QueryDestinationAbove1kInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].id = FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].reg = RTP_CRH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].field = CRH_N_QUERY_DESTINATION_ABOVE_1K_INTf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].mask_reg = RTP_CRH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].mask_reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].mask_field = CRH_N_QUERY_DESTINATION_ABOVE_1K_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].name = "crh_0_QueryEmptyLinkMapInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].id = FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].reg = RTP_CRH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].field = CRH_N_QUERY_EMPTY_LINK_MAP_INTf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].mask_reg = RTP_CRH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].mask_field = CRH_N_QUERY_EMPTY_LINK_MAP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYEMPTYLINKMAPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].name = "crh_1_QueryEmptyLinkMapInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].id = FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].reg = RTP_CRH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].field = CRH_N_QUERY_EMPTY_LINK_MAP_INTf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].mask_reg = RTP_CRH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].mask_reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].mask_field = CRH_N_QUERY_EMPTY_LINK_MAP_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYEMPTYLINKMAPINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].name = "crh_0_QueryDestinationAboveMaxBaseIndexInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].id = FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].reg = RTP_CRH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].field = CRH_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INTf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].mask_reg = RTP_CRH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].mask_field = CRH_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].name = "crh_1_QueryDestinationAboveMaxBaseIndexInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].id = FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].reg = RTP_CRH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].field = CRH_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INTf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].mask_reg = RTP_CRH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].mask_reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].mask_field = CRH_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].name = "crh_0_QueryDestinationAboveUpdateBaseIndexInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].id = FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].reg = RTP_CRH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].field = CRH_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INTf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].mask_reg = RTP_CRH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].mask_field = CRH_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].name = "crh_1_QueryDestinationAboveUpdateBaseIndexInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].id = FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].reg = RTP_CRH_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].field = CRH_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INTf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].mask_reg = RTP_CRH_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].mask_reg_index = 1;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].mask_field = CRH_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].name = "LinkIntegrityChangedInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].id = FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT;
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].reg = RTP_GENERAL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].field = LINK_INTEGRITY_CHANGED_INTf;
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].mask_reg = RTP_GENERAL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].mask_field = LINK_INT_MASKEGRITY_CHANGED_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].name = "UnicastTableChangedInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].id = FE1600_INT_RTP_UNICASTTABLECHANGEDINT;
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].reg = RTP_GENERAL_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].field = UNICAST_TABLE_CHANGED_INTf;
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].mask_reg = RTP_GENERAL_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].mask_field = MAINTENANCE_TABLE_CHANGED_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_UNICASTTABLECHANGEDINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


  
#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].name = "RTP_Ecc_1bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].id = FE1600_INT_RTP_ECC_1BERRINT;
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].reg = RTP_ECC_1B_ERR_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].field = ECC_1B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].mask_reg = RTP_ECC_1B_ERR_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].mask_field = ECC_1B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].cnt_reg         = RTP_ECC_1B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC_1BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].name = "RTP_Ecc_2bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].id = FE1600_INT_RTP_ECC_2BERRINT;
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].reg = RTP_ECC_2B_ERR_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].field = ECC_2B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].mask_reg = RTP_ECC_2B_ERR_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].mask_field = ECC_2B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].cnt_reg         = RTP_ECC_2B_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_ECC_2BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].name = "RTP_ParityErrInt";
#endif
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].id = FE1600_INT_RTP_PARITYERRINT;
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].reg = RTP_PAR_ERR_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].field = PARITY_ERR_INTf;
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].mask_reg = RTP_PAR_ERR_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].mask_field = PARITY_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].cnt_reg         = RTP_PARITY_ERR_CNTr;
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_RTP_PARITYERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_RTP * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].name = "MAC_IntReg1";
#endif
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].id = FE1600_INT_MAC_INTREG1;
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].reg = FMAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].field = INT_REG_1f;
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].mask_reg = FMAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].mask_field = INT_REG_1_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].vector_id = 1;
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].vector_info = &fe1600_interrupt_tree_vecotr_fmac_int1;
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].name = "MAC_IntReg2";
#endif
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].id = FE1600_INT_MAC_INTREG2;
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].reg = FMAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].field = INT_REG_2f;
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].mask_reg = FMAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].mask_field = INT_REG_2_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].vector_id = 1;
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].vector_info = &fe1600_interrupt_tree_vecotr_fmac_int2;
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].name = "MAC_IntReg3";
#endif
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].id = FE1600_INT_MAC_INTREG3;
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].reg = FMAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].field = INT_REG_3f;
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].mask_reg = FMAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].mask_field = INT_REG_3_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].vector_id = 1;
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].vector_info = &fe1600_interrupt_tree_vecotr_fmac_int3;
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].name = "MAC_IntReg4";
#endif
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].id = FE1600_INT_MAC_INTREG4;
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].reg = FMAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].field = INT_REG_4f;
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].mask_reg = FMAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].mask_field = INT_REG_4_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].vector_id = 1;
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].vector_info = &fe1600_interrupt_tree_vecotr_fmac_int4;
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG4].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG4].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG4].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG4].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG4].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].name = "MAC_IntReg5";
#endif
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].id = FE1600_INT_MAC_INTREG5;
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].reg = FMAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].field = INT_REG_5f;
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].mask_reg = FMAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].mask_field = INT_REG_5_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].vector_id = 1;
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].vector_info = &fe1600_interrupt_tree_vecotr_fmac_int5;
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG5].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG5].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG5].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG5].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG5].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].name = "MAC_IntReg6";
#endif
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].id = FE1600_INT_MAC_INTREG6;
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].reg = FMAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].field = INT_REG_6f;
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].mask_reg = FMAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].mask_field = INT_REG_6_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].vector_id = 1;
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].vector_info = &fe1600_interrupt_tree_vecotr_fmac_int6;
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG6].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG6].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG6].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG6].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG6].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].name = "MAC_IntReg7";
#endif
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].id = FE1600_INT_MAC_INTREG7;
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].reg = FMAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].field = INT_REG_7f;
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].mask_reg = FMAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].mask_field = INT_REG_7_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].vector_id = 1;
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].vector_info = &fe1600_interrupt_tree_vecotr_fmac_int7;
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG7].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG7].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG7].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG7].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG7].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].name = "MAC_IntReg8";
#endif
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].id = FE1600_INT_MAC_INTREG8;
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].reg = FMAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].field = INT_REG_8f;
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].mask_reg = FMAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].mask_field = INT_REG_8_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].vector_id = 1;
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].vector_info = &fe1600_interrupt_tree_vecotr_fmac_int8;
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG8].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG8].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG8].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG8].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG8].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].name = "MAC_IntReg9";
#endif
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].id = FE1600_INT_MAC_INTREG9;
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].reg = FMAC_INTERRUPT_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].field = INT_REG_9f;
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].mask_reg = FMAC_INTERRUPT_MASK_REGISTERr;
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].mask_field = INT_REG_9_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].vector_id = 1;
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].vector_info = &fe1600_interrupt_tree_vecotr_fmac_int9;
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].interrupt_clear = NULL; 
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG9].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG9].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG9].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_INTREG9].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_INTREG9].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].name = "RxCRCErrN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].id = FE1600_INT_MAC_RXCRCERRN_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].reg = FMAC_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].field = RX_CRC_ERR_N_INTf;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].mask_field = RX_CRC_ERR_N_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].bit_in_field = FE1600_INT_RX_CRCERR_N_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].name = "RxCRCErrN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].id = FE1600_INT_MAC_RXCRCERRN_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].reg = FMAC_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].field = RX_CRC_ERR_N_INTf;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].mask_field = RX_CRC_ERR_N_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].bit_in_field = FE1600_INT_RX_CRCERR_N_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].name = "RxCRCErrN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].id = FE1600_INT_MAC_RXCRCERRN_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].reg = FMAC_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].field = RX_CRC_ERR_N_INTf;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].mask_field = RX_CRC_ERR_N_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].bit_in_field = FE1600_INT_RX_CRCERR_N_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].name = "RxCRCErrN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].id = FE1600_INT_MAC_RXCRCERRN_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].reg = FMAC_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].field = RX_CRC_ERR_N_INTf;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].mask_field = RX_CRC_ERR_N_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].bit_in_field = FE1600_INT_RX_CRCERR_N_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCRCERRN_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].name = "WrongSize_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].id = FE1600_INT_MAC_WRONGSIZE_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].reg = FMAC_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].field = WRONG_SIZE_INTf;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].mask_field = WRONG_SIZE_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].bit_in_field = FE1600_INT_WRONG_SIZE_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].name = "WrongSize_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].id = FE1600_INT_MAC_WRONGSIZE_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].reg = FMAC_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].field = WRONG_SIZE_INTf;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].mask_field = WRONG_SIZE_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].bit_in_field = FE1600_INT_WRONG_SIZE_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].name = "WrongSize_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].id = FE1600_INT_MAC_WRONGSIZE_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].reg = FMAC_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].field = WRONG_SIZE_INTf;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].mask_field = WRONG_SIZE_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].bit_in_field = FE1600_INT_WRONG_SIZE_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].name = "WrongSize_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].id = FE1600_INT_MAC_WRONGSIZE_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].reg = FMAC_INTERRUPT_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].field = WRONG_SIZE_INTf;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_1r;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].mask_field = WRONG_SIZE_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].bit_in_field = FE1600_INT_WRONG_SIZE_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_WRONGSIZE_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].name = "LOS_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].id = FE1600_INT_MAC_LOS_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].reg = FMAC_INTERRUPT_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].field = LOS_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].mask_field = LOS_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].bit_in_field = FE1600_INT_LOS_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].name = "LOS_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].id = FE1600_INT_MAC_LOS_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].reg = FMAC_INTERRUPT_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].field = LOS_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].mask_field = LOS_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].bit_in_field = FE1600_INT_LOS_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].name = "LOS_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].id = FE1600_INT_MAC_LOS_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].reg = FMAC_INTERRUPT_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].field = LOS_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].mask_field = LOS_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].bit_in_field = FE1600_INT_LOS_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].name = "LOS_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].id = FE1600_INT_MAC_LOS_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].reg = FMAC_INTERRUPT_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].field = LOS_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].mask_field = LOS_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].bit_in_field = FE1600_INT_LOS_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LOS_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].name = "RxLostOfSync";
#endif
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].id = FE1600_INT_MAC_RXLOSTOFSYNC_0;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].reg = FMAC_INTERRUPT_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].field = RX_LOST_OF_SYNCf;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].mask_field = RX_LOST_OF_SYNC_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].bit_in_field = FE1600_INT_RX_LOST_OF_SYNC_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].name = "RxLostOfSync";
#endif
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].id = FE1600_INT_MAC_RXLOSTOFSYNC_1;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].reg = FMAC_INTERRUPT_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].field = RX_LOST_OF_SYNCf;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].mask_field = RX_LOST_OF_SYNC_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].bit_in_field = FE1600_INT_RX_LOST_OF_SYNC_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].name = "RxLostOfSync";
#endif
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].id = FE1600_INT_MAC_RXLOSTOFSYNC_2;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].reg = FMAC_INTERRUPT_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].field = RX_LOST_OF_SYNCf;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].mask_field = RX_LOST_OF_SYNC_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].bit_in_field = FE1600_INT_RX_LOST_OF_SYNC_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].name = "RxLostOfSync";
#endif
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].id = FE1600_INT_MAC_RXLOSTOFSYNC_3;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].reg = FMAC_INTERRUPT_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].field = RX_LOST_OF_SYNCf;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_2r;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].mask_field = RX_LOST_OF_SYNC_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].bit_in_field = FE1600_INT_RX_LOST_OF_SYNC_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXLOSTOFSYNC_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].name = "LnklvlAgeN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].id = FE1600_INT_MAC_LNKLVLAGEN_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].reg = FMAC_INTERRUPT_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].field = LNKLVL_AGE_N_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].mask_field = LNKLVL_AGE_N_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].bit_in_field = FE1600_INT_LNKLVL_AGE_N_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].name = "LnklvlAgeN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].id = FE1600_INT_MAC_LNKLVLAGEN_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].reg = FMAC_INTERRUPT_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].field = LNKLVL_AGE_N_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].mask_field = LNKLVL_AGE_N_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].bit_in_field = FE1600_INT_LNKLVL_AGE_N_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].name = "LnklvlAgeN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].id = FE1600_INT_MAC_LNKLVLAGEN_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].reg = FMAC_INTERRUPT_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].field = LNKLVL_AGE_N_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].mask_field = LNKLVL_AGE_N_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].bit_in_field = FE1600_INT_LNKLVL_AGE_N_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].name = "LnklvlAgeN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].id = FE1600_INT_MAC_LNKLVLAGEN_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].reg = FMAC_INTERRUPT_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].field = LNKLVL_AGE_N_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].mask_field = LNKLVL_AGE_N_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].bit_in_field = FE1600_INT_LNKLVL_AGE_N_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGEN_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].name = "LnklvlHaltN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].id = FE1600_INT_MAC_LNKLVLHALTN_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].reg = FMAC_INTERRUPT_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].field = LNKLVL_HALT_N_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].mask_field = LNKLVL_HALT_N_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].bit_in_field = FE1600_INT_LNKLVL_HALT_N_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].name = "LnklvlHaltN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].id = FE1600_INT_MAC_LNKLVLHALTN_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].reg = FMAC_INTERRUPT_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].field = LNKLVL_HALT_N_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].mask_field = LNKLVL_HALT_N_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].bit_in_field = FE1600_INT_LNKLVL_HALT_N_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].name = "LnklvlHaltN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].id = FE1600_INT_MAC_LNKLVLHALTN_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].reg = FMAC_INTERRUPT_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].field = LNKLVL_HALT_N_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].mask_field = LNKLVL_HALT_N_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].bit_in_field = FE1600_INT_LNKLVL_HALT_N_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].name = "LnklvlHaltN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].id = FE1600_INT_MAC_LNKLVLHALTN_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].reg = FMAC_INTERRUPT_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].field = LNKLVL_HALT_N_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_3r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].mask_field = LNKLVL_HALT_N_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].bit_in_field = FE1600_INT_LNKLVL_HALT_N_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTN_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].name = "OOF_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].id = FE1600_INT_MAC_OOF_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].reg = FMAC_INTERRUPT_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].field = OOF_INTf;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].mask_field = OOF_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].bit_in_field = FE1600_INT_OOF_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].name = "OOF_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].id = FE1600_INT_MAC_OOF_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].reg = FMAC_INTERRUPT_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].field = OOF_INTf;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].mask_field = OOF_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].bit_in_field = FE1600_INT_OOF_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].name = "OOF_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].id = FE1600_INT_MAC_OOF_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].reg = FMAC_INTERRUPT_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].field = OOF_INTf;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].mask_field = OOF_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].bit_in_field = FE1600_INT_OOF_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].name = "OOF_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].id = FE1600_INT_MAC_OOF_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].reg = FMAC_INTERRUPT_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].field = OOF_INTf;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].mask_field = OOF_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].bit_in_field = FE1600_INT_OOF_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_OOF_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].name = "DecErr_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].id = FE1600_INT_MAC_DECERR_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].reg = FMAC_INTERRUPT_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].field = DEC_ERR_INTf;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].mask_field = DEC_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].bit_in_field = FE1600_INT_DEC_ERR_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].name = "DecErr_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].id = FE1600_INT_MAC_DECERR_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].reg = FMAC_INTERRUPT_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].field = DEC_ERR_INTf;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].mask_field = DEC_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].bit_in_field = FE1600_INT_DEC_ERR_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].name = "DecErr_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].id = FE1600_INT_MAC_DECERR_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].reg = FMAC_INTERRUPT_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].field = DEC_ERR_INTf;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].mask_field = DEC_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].bit_in_field = FE1600_INT_DEC_ERR_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].name = "DecErr_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].id = FE1600_INT_MAC_DECERR_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].reg = FMAC_INTERRUPT_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].field = DEC_ERR_INTf;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_4r;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].mask_field = DEC_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].bit_in_field = FE1600_INT_DEC_ERR_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_DECERR_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].name = "TransmitErr_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].id = FE1600_INT_MAC_TRANSMITERR_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].reg = FMAC_INTERRUPT_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].field = TRANSMIT_ERR_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].mask_field = TRANSMIT_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].bit_in_field = FE1600_INT_TRANSMIT_ERR_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].name = "TransmitErr_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].id = FE1600_INT_MAC_TRANSMITERR_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].reg = FMAC_INTERRUPT_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].field = TRANSMIT_ERR_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].mask_field = TRANSMIT_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].bit_in_field = FE1600_INT_TRANSMIT_ERR_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].name = "TransmitErr_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].id = FE1600_INT_MAC_TRANSMITERR_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].reg = FMAC_INTERRUPT_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].field = TRANSMIT_ERR_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].mask_field = TRANSMIT_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].bit_in_field = FE1600_INT_TRANSMIT_ERR_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].name = "TransmitErr_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].id = FE1600_INT_MAC_TRANSMITERR_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].reg = FMAC_INTERRUPT_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].field = TRANSMIT_ERR_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].mask_field = TRANSMIT_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].bit_in_field = FE1600_INT_TRANSMIT_ERR_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TRANSMITERR_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].name = "RxCtrlOverflow_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].id = FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].reg = FMAC_INTERRUPT_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].field = RX_CTRL_OVERFLOW_INTf;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].mask_field = RX_CTRL_OVERFLOW_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].bit_in_field = FE1600_INT_RX_CTRL_OVERFLOW_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].name = "RxCtrlOverflow_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].id = FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].reg = FMAC_INTERRUPT_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].field = RX_CTRL_OVERFLOW_INTf;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].mask_field = RX_CTRL_OVERFLOW_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].bit_in_field = FE1600_INT_RX_CTRL_OVERFLOW_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].name = "RxCtrlOverflow_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].id = FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].reg = FMAC_INTERRUPT_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].field = RX_CTRL_OVERFLOW_INTf;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].mask_field = RX_CTRL_OVERFLOW_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].bit_in_field = FE1600_INT_RX_CTRL_OVERFLOW_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].name = "RxCtrlOverflow_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].id = FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].reg = FMAC_INTERRUPT_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].field = RX_CTRL_OVERFLOW_INTf;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_5r;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].mask_field = RX_CTRL_OVERFLOW_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].bit_in_field = FE1600_INT_RX_CTRL_OVERFLOW_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].name = "LnklvlHaltCtxBN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].id = FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].reg = FMAC_INTERRUPT_REGISTER_6r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].field = LNKLVL_HALT_CTX_BN_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_6r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].mask_field = LNKLVL_HALT_CTX_BN_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].bit_in_field = FE1600_INT_LNKLVL_HALT_CTX_BN_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].name = "LnklvlHaltCtxBN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].id = FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].reg = FMAC_INTERRUPT_REGISTER_6r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].field = LNKLVL_HALT_CTX_BN_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_6r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].mask_field = LNKLVL_HALT_CTX_BN_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].bit_in_field = FE1600_INT_LNKLVL_HALT_CTX_BN_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].name = "LnklvlHaltCtxBN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].id = FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].reg = FMAC_INTERRUPT_REGISTER_6r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].field = LNKLVL_HALT_CTX_BN_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_6r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].mask_field = LNKLVL_HALT_CTX_BN_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].bit_in_field = FE1600_INT_LNKLVL_HALT_CTX_BN_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].name = "LnklvlHaltCtxBN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].id = FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].reg = FMAC_INTERRUPT_REGISTER_6r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].field = LNKLVL_HALT_CTX_BN_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_6r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].mask_field = LNKLVL_HALT_CTX_BN_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].bit_in_field = FE1600_INT_LNKLVL_HALT_CTX_BN_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLHALTCTXBN_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].name = "LnklvlAgeCtxBN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].id = FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].reg = FMAC_INTERRUPT_REGISTER_7r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].field = LNKLVL_AGE_CTX_BN_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_7r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].mask_field = LNKLVL_AGE_CTX_BN_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].bit_in_field = FE1600_INT_LNKLVL_AGE_CTX_BN_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].name = "LnklvlAgeCtxBN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].id = FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].reg = FMAC_INTERRUPT_REGISTER_7r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].field = LNKLVL_AGE_CTX_BN_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_7r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].mask_field = LNKLVL_AGE_CTX_BN_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].bit_in_field = FE1600_INT_LNKLVL_AGE_CTX_BN_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].name = "LnklvlAgeCtxBN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].id = FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].reg = FMAC_INTERRUPT_REGISTER_7r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].field = LNKLVL_AGE_CTX_BN_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_7r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].mask_field = LNKLVL_AGE_CTX_BN_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].bit_in_field = FE1600_INT_LNKLVL_AGE_CTX_BN_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].name = "LnklvlAgeCtxBN_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].id = FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].reg = FMAC_INTERRUPT_REGISTER_7r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].field = LNKLVL_AGE_CTX_BN_INTf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_7r;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].mask_field = LNKLVL_AGE_CTX_BN_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].bit_in_field = FE1600_INT_LNKLVL_AGE_CTX_BN_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].name = "TxFdrcIfCrc_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].id = FE1600_INT_MAC_TXFDRCIFCRC_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].reg = FMAC_INTERRUPT_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].field = TX_FDRC_IF_CRC_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].mask_field = TX_FDRC_IF_CRC_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].bit_in_field = FE1600_INT_TX_FDRC_IF_CRC_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].name = "TxFdrcIfCrc_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].id = FE1600_INT_MAC_TXFDRCIFCRC_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].reg = FMAC_INTERRUPT_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].field = TX_FDRC_IF_CRC_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].mask_field = TX_FDRC_IF_CRC_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].bit_in_field = FE1600_INT_TX_FDRC_IF_CRC_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].name = "TxFdrcIfCrc_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].id = FE1600_INT_MAC_TXFDRCIFCRC_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].reg = FMAC_INTERRUPT_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].field = TX_FDRC_IF_CRC_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].mask_field = TX_FDRC_IF_CRC_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].bit_in_field = FE1600_INT_TX_FDRC_IF_CRC_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].name = "TxFdrcIfCrc_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].id = FE1600_INT_MAC_TXFDRCIFCRC_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].reg = FMAC_INTERRUPT_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].field = TX_FDRC_IF_CRC_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].mask_field = TX_FDRC_IF_CRC_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].bit_in_field = FE1600_INT_TX_FDRC_IF_CRC_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFCRC_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].name = "TxFdrcIfParity_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].id = FE1600_INT_MAC_TXFDRCIFPARITY_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].reg = FMAC_INTERRUPT_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].field = TX_FDRC_IF_PARITY_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].mask_field = TX_FDRC_IF_PARITY_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].bit_in_field = FE1600_INT_TX_FDRC_IF_PARITY_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].name = "TxFdrcIfParity_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].id = FE1600_INT_MAC_TXFDRCIFPARITY_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].reg = FMAC_INTERRUPT_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].field = TX_FDRC_IF_PARITY_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].mask_field = TX_FDRC_IF_PARITY_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].bit_in_field = FE1600_INT_TX_FDRC_IF_PARITY_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].name = "TxFdrcIfParity_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].id = FE1600_INT_MAC_TXFDRCIFPARITY_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].reg = FMAC_INTERRUPT_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].field = TX_FDRC_IF_PARITY_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].mask_field = TX_FDRC_IF_PARITY_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].bit_in_field = FE1600_INT_TX_FDRC_IF_PARITY_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].name = "TxFdrcIfParity_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].id = FE1600_INT_MAC_TXFDRCIFPARITY_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].reg = FMAC_INTERRUPT_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].field = TX_FDRC_IF_PARITY_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].mask_field = TX_FDRC_IF_PARITY_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].bit_in_field = FE1600_INT_TX_FDRC_IF_PARITY_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFPARITY_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].name = "TxFdrcIfFault_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].id = FE1600_INT_MAC_TXFDRCIFFAULT_INT_0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].reg = FMAC_INTERRUPT_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].field = TX_FDRC_IF_FAULT_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].mask_field = TX_FDRC_IF_FAULT_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].bit_in_field = FE1600_INT_TX_FDRC_IF_FAULT_INT_0_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].name = "TxFdrcIfFault_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].id = FE1600_INT_MAC_TXFDRCIFFAULT_INT_1;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].reg = FMAC_INTERRUPT_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].field = TX_FDRC_IF_FAULT_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].mask_field = TX_FDRC_IF_FAULT_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].bit_in_field = FE1600_INT_TX_FDRC_IF_FAULT_INT_1_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].name = "TxFdrcIfFault_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].id = FE1600_INT_MAC_TXFDRCIFFAULT_INT_2;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].reg = FMAC_INTERRUPT_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].field = TX_FDRC_IF_FAULT_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].mask_field = TX_FDRC_IF_FAULT_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].bit_in_field = FE1600_INT_TX_FDRC_IF_FAULT_INT_2_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].name = "TxFdrcIfFault_Int";
#endif
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].id = FE1600_INT_MAC_TXFDRCIFFAULT_INT_3;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].reg = FMAC_INTERRUPT_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].field = TX_FDRC_IF_FAULT_INTf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_8r;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].mask_field = TX_FDRC_IF_FAULT_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].bit_in_field = FE1600_INT_TX_FDRC_IF_FAULT_INT_3_BIT_IN_FIELD;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_TXFDRCIFFAULT_INT_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].name = "MAC_Ecc_1bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].id = FE1600_INT_MAC_ECC_1BERRINT;
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].reg = FMAC_INTERRUPT_REGISTER_9r;
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].field = ECC_1B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_9r;
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].mask_field = ECC_1B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_ECC_1BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].name = "MAC_Ecc_2bErrInt";
#endif
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].id = FE1600_INT_MAC_ECC_2BERRINT;
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].reg = FMAC_INTERRUPT_REGISTER_9r;
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].field = ECC_2B_ERR_INTf;
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].mask_reg = FMAC_INTERRUPT_MASK_REGISTER_9r;
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].mask_field = ECC_2B_ERR_INT_MASKf;
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].interrupt_clear = soc_interrupt_clear_on_write;
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));
    fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32), interrupts_info_allocation);
    sal_memset(fe1600_interrupts[FE1600_INT_MAC_ECC_2BERRINT].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC * sizeof(uint32));


#ifdef BCM_88754_A0
    if (SOC_IS_BCM88754_A0(unit))
    {
#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].name = "SRD_IntReg0";
#endif
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].id = FE1600_INT_SRD_INTREG0;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].name = "SRD_IntReg1";
#endif
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].id = FE1600_INT_SRD_INTREG1;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].name = "SRD_IntReg2";
#endif
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].id = FE1600_INT_SRD_INTREG2;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].name = "SRD_IntReg3";
#endif
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].id = FE1600_INT_SRD_INTREG3;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].name = "Fsrd_0_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].id = FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].name = "Fsrd_0_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].id = FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].name = "Fsrd_0_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].id = FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].name = "Fsrd_0_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].id = FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].name = "Fsrd_1_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].id = FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].name = "Fsrd_1_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].id = FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].name = "Fsrd_1_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].id = FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].name = "Fsrd_1_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].id = FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].name = "Fsrd_2_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].id = FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].name = "Fsrd_2_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].id = FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].name = "Fsrd_2_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].id = FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].name = "Fsrd_2_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].id = FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].name = "Fsrd_3_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].id = FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].name = "Fsrd_3_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].id = FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].name = "Fsrd_3_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].id = FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].name = "Fsrd_3_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].id = FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].name = "Fsrd_0_TxpllLockChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].id = FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].name = "Fsrd_1_TxpllLockChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].id = FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].name = "Fsrd_2_TxpllLockChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].id = FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].storm_detection_start_time  = NULL;

#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].name = "Fsrd_3_TxpllLockChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].id = FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].mask_reg = INVALIDr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].mask_field = INVALIDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].interrupt_clear = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].statistics_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].storm_nominal_count  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].storm_detection_occurrences  = NULL;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].storm_detection_start_time  = NULL;
    } else 
#endif 
    {
#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].name = "SRD_IntReg0";
#endif
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].id = FE1600_INT_SRD_INTREG0;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].reg = FSRD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].field = INT_REG_0f;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].mask_reg = FSRD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].mask_field = INT_REG_0_INT_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].vector_id = 1;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].vector_info = &fe1600_interrupt_tree_vecotr_fsrd_int0;
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].interrupt_clear = NULL; 
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_INTREG0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].name = "SRD_IntReg1";
#endif
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].id = FE1600_INT_SRD_INTREG1;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].reg = FSRD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].field = INT_REG_1f;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].mask_reg = FSRD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].mask_field = INT_REG_1_INT_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].vector_id = 1;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].vector_info = &fe1600_interrupt_tree_vecotr_fsrd_int1;
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].interrupt_clear = NULL; 
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_INTREG1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].name = "SRD_IntReg2";
#endif
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].id = FE1600_INT_SRD_INTREG2;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].reg = FSRD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].field = INT_REG_2f;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].mask_reg = FSRD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].mask_field = INT_REG_2_INT_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].vector_id = 1;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].vector_info = &fe1600_interrupt_tree_vecotr_fsrd_int2;
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].interrupt_clear = NULL; 
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_INTREG2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].name = "SRD_IntReg3";
#endif
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].id = FE1600_INT_SRD_INTREG3;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].reg = FSRD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].field = INT_REG_3f;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].mask_reg = FSRD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].mask_field = INT_REG_3_INT_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].vector_id = 1;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].vector_info = &fe1600_interrupt_tree_vecotr_fsrd_int3;
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].interrupt_clear = NULL; 
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_INTREG3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_INTREG3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].name = "Fsrd_0_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].id = FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_0_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].name = "Fsrd_0_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].id = FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_1_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].name = "Fsrd_0_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].id = FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_2_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].name = "Fsrd_0_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].id = FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_3_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].name = "Fsrd_1_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].id = FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].reg_index = 1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].mask_reg_index = 1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_0_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].name = "Fsrd_1_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].id = FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].reg_index = 1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].mask_reg_index = 1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_1_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].name = "Fsrd_1_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].id = FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].reg_index = 1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].mask_reg_index = 1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_2_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].name = "Fsrd_1_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].id = FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].reg_index = 1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].mask_reg_index = 1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_3_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].name = "Fsrd_2_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].id = FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].reg_index = 2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].mask_reg_index = 2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_0_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].name = "Fsrd_2_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].id = FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].reg_index = 2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].mask_reg_index = 2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_1_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].name = "Fsrd_2_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].id = FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].reg_index = 2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].mask_reg_index = 2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_2_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].name = "Fsrd_2_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].id = FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].reg_index = 2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].mask_reg_index = 2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_3_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].name = "Fsrd_3_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].id = FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].reg_index = 3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].mask_reg_index = 3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_0_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].name = "Fsrd_3_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].id = FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].reg_index = 3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].mask_reg_index = 3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_1_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].name = "Fsrd_3_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].id = FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].reg_index = 3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].mask_reg_index = 3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_2_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].name = "Fsrd_3_SyncStatusChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].id = FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].reg_index = 3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].field = FSRD_N_SYNC_STATUS_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].mask_reg_index = 3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].mask_field = FSRD_N_SYNC_STATUS_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].bit_in_field = FE1600_INT_FSRD_0_SYNC_STATUS_CHANGED_3_BIT_IN_FIELD;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].name = "Fsrd_0_TxpllLockChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].id = FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].field = FSRD_N_TXPLL_LOCK_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].mask_reg_index = 0;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].mask_field = FSRD_N_TXPLL_LOCK_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_0_TXPLLLOCKCHANGED].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].name = "Fsrd_1_TxpllLockChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].id = FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].reg_index = 1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].field = FSRD_N_TXPLL_LOCK_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].mask_reg_index = 1;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].mask_field = FSRD_N_TXPLL_LOCK_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_1_TXPLLLOCKCHANGED].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].name = "Fsrd_2_TxpllLockChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].id = FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].reg_index = 2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].field = FSRD_N_TXPLL_LOCK_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].mask_reg_index = 2;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].mask_field = FSRD_N_TXPLL_LOCK_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_2_TXPLLLOCKCHANGED].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));


#if !defined(SOC_NO_NAMES)
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].name = "Fsrd_3_TxpllLockChanged";
#endif
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].id = FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].reg = FSRD_QUAD_INTERRUPT_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].reg_index = 3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].field = FSRD_N_TXPLL_LOCK_CHANGEDf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].mask_reg = FSRD_QUAD_INTERRUPT_MASK_REGISTERr;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].mask_reg_index = 3;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].mask_field = FSRD_N_TXPLL_LOCK_CHANGED_MASKf;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].interrupt_clear = soc_interrupt_clear_on_write;
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].statistics_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].statistics_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].storm_nominal_count  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].storm_nominal_count , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].storm_detection_occurrences  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].storm_detection_occurrences , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));
        fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].storm_detection_start_time  = sal_alloc(SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32), interrupts_info_allocation);
        sal_memset(fe1600_interrupts[FE1600_INT_SRD_FSRD_3_TXPLLLOCKCHANGED].storm_detection_start_time , 0, SOC_FE1600_NOF_INSTANCES_MAC_FSRD * sizeof(uint32));

    }

    
#if !defined(SOC_NO_NAMES)
    fe1600_interrupts[FE1600_INT_LAST].name = "";
#endif
    fe1600_interrupts[FE1600_INT_LAST].reg = INVALIDr;
    fe1600_interrupts[FE1600_INT_LAST].reg_index = 0;
    fe1600_interrupts[FE1600_INT_LAST].field = INVALIDf;
    fe1600_interrupts[FE1600_INT_LAST].mask_reg = INVALIDr;
    fe1600_interrupts[FE1600_INT_LAST].mask_reg_index = 0;
    fe1600_interrupts[FE1600_INT_LAST].mask_field = INVALIDf;
    fe1600_interrupts[FE1600_INT_LAST].bit_in_field = SOC_INTERRUPT_BIT_FIELD_DONT_CARE;
    fe1600_interrupts[FE1600_INT_LAST].interrupt_clear = NULL;
    fe1600_interrupts[FE1600_INT_LAST].statistics_count  = NULL;
    fe1600_interrupts[FE1600_INT_LAST].storm_nominal_count  = NULL;
    fe1600_interrupts[FE1600_INT_LAST].storm_detection_occurrences  = NULL;
    fe1600_interrupts[FE1600_INT_LAST].storm_detection_start_time  = NULL;

exit:               
    SOCDNX_FUNC_RETURN;  

}

void 
fe1600_interrupts_array_deinit(int unit)  
{
    soc_interrupt_db_t *dfe_interrupts;            
    soc_interrupt_tree_t *dfe_interrupt_tree;
    int inter = 0;
	int nof_intr;
    SOCDNX_INIT_FUNC_DEFS;
	

    if(NULL == SOC_CONTROL(unit)->interrupts_info) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOCDNX_MSG("interrupts_info isn't initialized")));
    }

    dfe_interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    dfe_interrupt_tree = SOC_CONTROL(unit)->interrupts_info->interrupt_tree_info;
    nof_intr = SOC_DFE_DEFS_GET(unit, nof_interrupts);

    for (inter = 0; inter < nof_intr ; inter++) {
        if (dfe_interrupts[inter].statistics_count != NULL) {
            sal_free(dfe_interrupts[inter].statistics_count);
            dfe_interrupts[inter].statistics_count = NULL;
        }
        if (dfe_interrupts[inter].storm_nominal_count != NULL) {
            sal_free(dfe_interrupts[inter].storm_nominal_count);
            dfe_interrupts[inter].storm_nominal_count = NULL;
        }

        if (dfe_interrupts[inter].storm_detection_occurrences != NULL) {
            sal_free(dfe_interrupts[inter].storm_detection_occurrences);
            dfe_interrupts[inter].storm_detection_occurrences = NULL;
        }
        if (dfe_interrupts[inter].storm_detection_start_time != NULL) {
            sal_free(dfe_interrupts[inter].storm_detection_start_time);
            dfe_interrupts[inter].storm_detection_start_time = NULL;
        }
    }

    if (dfe_interrupts) {
        sal_free(dfe_interrupts);
        SOC_CONTROL(unit)->interrupts_info->interrupt_db_info = NULL;
    }
    if (dfe_interrupt_tree) {
        sal_free(dfe_interrupt_tree);
        SOC_CONTROL(unit)->interrupts_info->interrupt_tree_info = NULL;
    }
    if(SOC_CONTROL(unit)->interrupts_info) {
        sal_free(SOC_CONTROL(unit)->interrupts_info);
        SOC_CONTROL(unit)->interrupts_info = NULL;
    }

exit:               
    SOCDNX_FUNC_RETURN_VOID;  
}

int fe1600_interrupts_dfe_control_data_init(int unit)  
{

    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;                    

    if (!SOC_WARM_BOOT(unit))
    {
        rc = SOC_DFE_WARM_BOOT_ARR_MEMSET(unit, INTR_STORM_TIMED_PERIOD, 0);
        SOCDNX_IF_ERR_EXIT(rc);
        rc = SOC_DFE_WARM_BOOT_ARR_MEMSET(unit, INTR_STORM_TIMED_COUNT, 0);
        SOCDNX_IF_ERR_EXIT(rc);
        rc = SOC_DFE_WARM_BOOT_ARR_MEMSET(unit, INTR_FLAGS, 0);
        SOCDNX_IF_ERR_EXIT(rc);
    }
    

exit:               
    SOCDNX_FUNC_RETURN;
} 

void fe1600_interrupts_dfe_control_data_deinit(int unit)  
{
	
    
    
            
}  


int
soc_fe1600_nof_interrupts(int unit, int *nof_interrupts) {
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);
    (*nof_interrupts) = FE1600_INT_LAST;
    SOCDNX_FUNC_RETURN;
}

int 
soc_fe1600_interrupt_all_enable_set(int unit, int enable)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (enable) {
        soc_intr_block_lo_enable(unit, 0xFFFFFFFF);
        soc_intr_block_hi_enable(unit, 0xFFFFFFFF);
    } else {
        soc_intr_block_lo_disable(unit, 0xFFFFFFFF);
        soc_intr_block_hi_disable(unit, 0xFFFFFFFF);
    }

    SOCDNX_FUNC_RETURN;
}

int 
soc_fe1600_interrupt_all_enable_get(int unit, int *enable)
{
    int rc;
    int mask;
    SOCDNX_INIT_FUNC_DEFS;

    rc = soc_interrupt_is_all_mask(unit, &mask);
    SOCDNX_IF_ERR_EXIT(rc);

    *enable = mask ? 0 : 1;
exit:
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

#endif 




