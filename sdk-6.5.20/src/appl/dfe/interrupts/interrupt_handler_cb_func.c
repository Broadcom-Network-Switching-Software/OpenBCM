/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_BCM_INTR

/* 
 *  include  
 */ 
#include <sal/core/time.h>

#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/fe1600/fe1600_interrupts.h>
#include <soc/dfe/fe1600/fe1600_port.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm_int/dfe_dispatch.h>
#include <bcm_int/common/debug.h>

#include <appl/diag/system.h>

#include <appl/dfe/interrupts/interrupt_handler.h>
#include <appl/dfe/interrupts/interrupt_handler_cb_func.h>
#include <appl/dfe/interrupts/interrupt_handler_corr_act_func.h>

#include <appl/dcmn/interrupts/interrupt_handler.h>


/* 
 *  init functions
 */ 
void
fe1600_interrupt_add_interrupt_handler_init(
    int unit)
{
    /* init fe1600 interrupt data base */
    interrupt_add_interrupt_handler(unit,               FE1600_INT_ECI_SERINT,     0,     0, fe1600_interrupt_handle_eci_serint , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_ECI_DCH0INTFORCE,     0,     0, fe1600_interrupt_handle_eci_dch0intforce , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_ECI_DCH1INTFORCE,     0,     0, fe1600_interrupt_handle_eci_dch1intforce , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_ECI_DCH2INTFORCE,     0,     0, fe1600_interrupt_handle_eci_dch2intforce , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_ECI_DCH3INTFORCE,     0,     0, fe1600_interrupt_handle_eci_dch3intforce , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_DCH_DCHDESCCNTOP,     0,     0, fe1600_interrupt_handle_dch_dchdesccntop , NULL);
    interrupt_add_interrupt_handler(unit,           FE1600_INT_DCH_IFMAFOPINT,    10,     1, fe1600_interrupt_handle_dch_ifmafopint ,fe1600_interrupt_recurring_action_handle_dch_ifmafopint);
    interrupt_add_interrupt_handler(unit,           FE1600_INT_DCH_IFMBFOPINT,    10,     1, fe1600_interrupt_handle_dch_ifmbfopint ,fe1600_interrupt_recurring_action_handle_dch_ifmbfopint);
    interrupt_add_interrupt_handler(unit, FE1600_INT_DCH_CPUDATACELLFNEA0PINT,     0,     0, fe1600_interrupt_handle_dch_cpudatacellfnea0pint , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_DCH_CPUDATACELLFNEA1PINT,     0,     0, fe1600_interrupt_handle_dch_cpudatacellfnea1pint , NULL);
    interrupt_add_interrupt_handler(unit,             FE1600_INT_DCH_ALTOPINT,    10,    10, fe1600_interrupt_handle_dch_altopint ,fe1600_interrupt_recurring_action_handle_dch_altopint);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_DCH_UNREACHDESTEVPINT,     0,     0, fe1600_interrupt_handle_dch_unreachdestevpint, NULL );
    interrupt_add_interrupt_handler(unit,        FE1600_INT_DCH_DCHUNEXPERROR,     0,     0, fe1600_interrupt_handle_dch_dchunexperror , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_DCH_DCHDESCCNTOS,     0,     0, fe1600_interrupt_handle_dch_dchdesccntos , NULL);
    interrupt_add_interrupt_handler(unit,           FE1600_INT_DCH_IFMAFOSINT,    10,     1, fe1600_interrupt_handle_dch_ifmafosint ,fe1600_interrupt_recurring_action_handle_dch_ifmafosint);
    interrupt_add_interrupt_handler(unit,           FE1600_INT_DCH_IFMBFOSINT,    10,     1, fe1600_interrupt_handle_dch_ifmbfosint ,fe1600_interrupt_recurring_action_handle_dch_ifmbfosint);
    interrupt_add_interrupt_handler(unit, FE1600_INT_DCH_CPUDATACELLFNEA0SINT,     0,     0, fe1600_interrupt_handle_dch_cpudatacellfnea0sint , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_DCH_CPUDATACELLFNEA1SINT,     0,     0, fe1600_interrupt_handle_dch_cpudatacellfnea1sint , NULL);
    interrupt_add_interrupt_handler(unit,             FE1600_INT_DCH_ALTOSINT,    10,    10, fe1600_interrupt_handle_dch_altosint ,fe1600_interrupt_recurring_action_handle_dch_altosint);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_DCH_UNREACHDESTEVSINT,     0,     0, fe1600_interrupt_handle_dch_unreachdestevsint , NULL);
    interrupt_add_interrupt_handler(unit,        FE1600_INT_DCH_OUTOFSYNCINTP,     0,     0, fe1600_interrupt_handle_dch_outofsyncintp , NULL);
    interrupt_add_interrupt_handler(unit,        FE1600_INT_DCH_OUTOFSYNCINTS,     0,     0, fe1600_interrupt_handle_dch_outofsyncints , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_DCH_ECC_1BERRINT,  1000,   100, fe1600_interrupt_handle_dch_ecc_1berrint ,fe1600_interrupt_recurring_action_handle_dch_ecc_1berrint);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_DCH_ECC_2BERRINT,     0,     0, fe1600_interrupt_handle_dch_ecc_2berrint , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_DCH_PARITYERRINT,     0,     0, fe1600_interrupt_handle_dch_parityerrint , NULL);
    interrupt_add_interrupt_handler(unit,      FE1600_INT_DCH_ERRORFILTERPINT,     0,     0, fe1600_interrupt_handle_dch_errorfilterpint, NULL);
    interrupt_add_interrupt_handler(unit,      FE1600_INT_DCH_ERRORFILTERSINT,     0,     0, fe1600_interrupt_handle_dch_errorfiltersint, NULL);
    interrupt_add_interrupt_handler(unit,  FE1600_INT_DCL_CCP0SRCDVCNGLINKINT,    10,    10, fe1600_interrupt_handle_dcl_ccp0srcdvcnglinkint ,fe1600_interrupt_recurring_action_handle_dcl_ccp0srcdvcnglinkint);
    interrupt_add_interrupt_handler(unit,  FE1600_INT_DCL_CCP1SRCDVCNGLINKINT,    10,    10, fe1600_interrupt_handle_dcl_ccp1srcdvcnglinkint ,fe1600_interrupt_recurring_action_handle_dcl_ccp1srcdvcnglinkint);
    interrupt_add_interrupt_handler(unit,        FE1600_INT_DCL_CCP0CHFOVFINT,  1000,   100, fe1600_interrupt_handle_dcl_ccp0chfovfint ,fe1600_interrupt_recurring_action_handle_dcl_ccp0chfovfint);
    interrupt_add_interrupt_handler(unit,        FE1600_INT_DCL_CCP0CLFOVFINT,  1000,   100, fe1600_interrupt_handle_dcl_ccp0clfovfint ,fe1600_interrupt_recurring_action_handle_dcl_ccp0clfovfint);
    interrupt_add_interrupt_handler(unit,        FE1600_INT_DCL_CCP1CHFOVFINT,  1000,   100, fe1600_interrupt_handle_dcl_ccp1chfovfint ,fe1600_interrupt_recurring_action_handle_dcl_ccp1chfovfint);
    interrupt_add_interrupt_handler(unit,        FE1600_INT_DCL_CCP1CLFOVFINT,  1000,   100, fe1600_interrupt_handle_dcl_ccp1clfovfint ,fe1600_interrupt_recurring_action_handle_dcl_ccp1clfovfint);
    interrupt_add_interrupt_handler(unit,     FE1600_INT_DCL_DCLUTAGPARERRINT,  1000,   100, fe1600_interrupt_handle_dcl_dclutagparerrint ,fe1600_interrupt_recurring_action_handle_dcl_dclutagparerrint);
    interrupt_add_interrupt_handler(unit,     FE1600_INT_DCL_DCLMTAGPARERRINT,  1000,   100, fe1600_interrupt_handle_dcl_dclmtagparerrint ,fe1600_interrupt_recurring_action_handle_dcl_dclmtagparerrint);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_DCL_MACADATACRCERRINT,  1000,   100, fe1600_interrupt_handle_dcl_macadatacrcerrint ,fe1600_interrupt_recurring_action_handle_dcl_macadatacrcerrint);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_DCL_MACBDATACRCERRINT,  1000,   100, fe1600_interrupt_handle_dcl_macbdatacrcerrint ,fe1600_interrupt_recurring_action_handle_dcl_macbdatacrcerrint);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_DCL_ECC_1BERRINT,  1000,   100, fe1600_interrupt_handle_dcl_ecc_1berrint ,fe1600_interrupt_recurring_action_handle_dcl_ecc_1berrint);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_DCL_ECC_2BERRINT,     0,     0, fe1600_interrupt_handle_dcl_ecc_2berrint , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_DCL_PARITYERRINT,     0,     0, fe1600_interrupt_handle_dcl_parityerrint , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_DCL_OUTOFSYNCINT,     0,     0, fe1600_interrupt_handle_dcl_outofsyncint , NULL);
    interrupt_add_interrupt_handler(unit,        FE1600_INT_DCMA_ECC_1BERRINT,  1000,   100, fe1600_interrupt_handle_dcma_ecc_1berrint ,fe1600_interrupt_recurring_action_handle_dcma_ecc_1berrint);
    interrupt_add_interrupt_handler(unit,        FE1600_INT_DCMA_ECC_2BERRINT,     0,     0, fe1600_interrupt_handle_dcma_ecc_2berrint , NULL);
    interrupt_add_interrupt_handler(unit,        FE1600_INT_DCMA_PARITYERRINT,     0,     0, fe1600_interrupt_handle_dcma_parityerrint , NULL);
    interrupt_add_interrupt_handler(unit,       FE1600_INT_DCMA_OUTOFSYNCPINT,     0,     0, fe1600_interrupt_handle_dcma_outofsyncpint , NULL);
    interrupt_add_interrupt_handler(unit,       FE1600_INT_DCMA_OUTOFSYNCSINT,     0,     0, fe1600_interrupt_handle_dcma_outofsyncsint , NULL);
    interrupt_add_interrupt_handler(unit,        FE1600_INT_DCMB_ECC_1BERRINT,  1000,   100, fe1600_interrupt_handle_dcmb_ecc_1berrint ,fe1600_interrupt_recurring_action_handle_dcmb_ecc_1berrint);
    interrupt_add_interrupt_handler(unit,        FE1600_INT_DCMB_ECC_2BERRINT,     0,     0, fe1600_interrupt_handle_dcmb_ecc_2berrint , NULL);
    interrupt_add_interrupt_handler(unit,        FE1600_INT_DCMB_PARITYERRINT,     0,     0, fe1600_interrupt_handle_dcmb_parityerrint , NULL);
    interrupt_add_interrupt_handler(unit,       FE1600_INT_DCMB_OUTOFSYNCPINT,     0,     0, fe1600_interrupt_handle_dcmb_outofsyncpint , NULL);
    interrupt_add_interrupt_handler(unit,       FE1600_INT_DCMB_OUTOFSYNCSINT,     0,     0, fe1600_interrupt_handle_dcmb_outofsyncsint , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_CCS_CPUCAPTCELLFNEINT,     0,     0, fe1600_interrupt_handle_ccs_cpucaptcellfneint , NULL);
    interrupt_add_interrupt_handler(unit,          FE1600_INT_CCS_UNRCHDSTINT,     0,     0, fe1600_interrupt_handle_ccs_unrchdstint , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_CCS_ECC_2BERRINT,    10,     0, fe1600_interrupt_handle_ccs_ecc_2berrint ,fe1600_interrupt_recurring_action_handle_ccs_ecc_2berrint);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_CCS_PARITYERRINT,    10,     0, fe1600_interrupt_handle_ccs_parityerrint ,fe1600_interrupt_recurring_action_handle_ccs_parityerrint);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_RTP_ECC_2BERRINT,     0,     0, fe1600_interrupt_handle_rtp_scrub_shadow_write , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_RTP_ECC_1BERRINT,     0,     0, fe1600_interrupt_handle_rtp_scrub_shadow_write , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_RTP_PARITYERRINT,     0,     0, fe1600_interrupt_handle_rtp_scrub_shadow_write , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHP_0_QUERYDESTINATIONABOVE1KINT,     0,     0, fe1600_interrupt_handle_rtp_drhp_0_querydestinationacove1kint , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHP_1_QUERYDESTINATIONABOVE1KINT,     0,     0, fe1600_interrupt_handle_rtp_drhp_1_querydestinationacove1kint , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHP_2_QUERYDESTINATIONABOVE1KINT,     0,     0, fe1600_interrupt_handle_rtp_drhp_2_querydestinationacove1kint , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHP_3_QUERYDESTINATIONABOVE1KINT,     0,     0, fe1600_interrupt_handle_rtp_drhp_3_querydestinationacove1kint , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHP_0_QUERYUNREACHABLEMULTICASTINT,  0,     0, fe1600_interrupt_handle_rtp_drhp_0_queryunreachablemulticastint, NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHP_1_QUERYUNREACHABLEMULTICASTINT,  0,     0, fe1600_interrupt_handle_rtp_drhp_1_queryunreachablemulticastint, NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHP_2_QUERYUNREACHABLEMULTICASTINT,  0,     0, fe1600_interrupt_handle_rtp_drhp_2_queryunreachablemulticastint, NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHP_3_QUERYUNREACHABLEMULTICASTINT,  0,     0, fe1600_interrupt_handle_rtp_drhp_3_queryunreachablemulticastint, NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHS_0_QUERYDESTINATIONABOVE1KINT,     0,     0, fe1600_interrupt_handle_rtp_drhs_0_querydestinationacove1kint , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHS_1_QUERYDESTINATIONABOVE1KINT,     0,     0, fe1600_interrupt_handle_rtp_drhs_1_querydestinationacove1kint , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHS_2_QUERYDESTINATIONABOVE1KINT,     0,     0, fe1600_interrupt_handle_rtp_drhs_2_querydestinationacove1kint , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHS_3_QUERYDESTINATIONABOVE1KINT,     0,     0, fe1600_interrupt_handle_rtp_drhs_3_querydestinationacove1kint , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHS_0_QUERYUNREACHABLEMULTICASTINT,  0,     0, fe1600_interrupt_handle_rtp_drhs_0_queryunreachablemulticastint, NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHS_1_QUERYUNREACHABLEMULTICASTINT,  0,     0, fe1600_interrupt_handle_rtp_drhs_1_queryunreachablemulticastint, NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHS_2_QUERYUNREACHABLEMULTICASTINT,  0,     0, fe1600_interrupt_handle_rtp_drhs_2_queryunreachablemulticastint, NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_DRHS_3_QUERYUNREACHABLEMULTICASTINT,  0,     0, fe1600_interrupt_handle_rtp_drhs_3_queryunreachablemulticastint, NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVE1KINT,     0,     0, fe1600_interrupt_handle_rtp_crh_0_querydestinationacove1kint , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVE1KINT,     0,     0, fe1600_interrupt_handle_rtp_crh_1_querydestinationacove1kint , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEMAXBASEINDEXINT,  0,     0, fe1600_interrupt_handle_rtp_crh_0_querydestinationabovemaxbaseindexint, NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEMAXBASEINDEXINT,  0,     0, fe1600_interrupt_handle_rtp_crh_1_querydestinationabovemaxbaseindexint, NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_CRH_0_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT,  0,     0, fe1600_interrupt_handle_rtp_crh_0_querydestinationaboveupdatebaseindexint, NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_CRH_1_QUERYDESTINATIONABOVEUPDATEBASEINDEXINT,  0,     0, fe1600_interrupt_handle_rtp_crh_0_querydestinationaboveupdatebaseindexint, NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_RTP_LINKINTEGRITYCHANGEDINT,    0,     0, fe1600_interrupt_handle_rtp_linkintegritychangedint , NULL);
    interrupt_add_interrupt_handler(unit,      FE1600_INT_MAC_RXCRCERRN_INT_0,   100,    10, fe1600_interrupt_handle_mac_rxcrcerrn_int_0 ,fe1600_interrupt_recurring_action_handle_mac_rxcrcerrn_int_0);
    interrupt_add_interrupt_handler(unit,      FE1600_INT_MAC_RXCRCERRN_INT_1,   100,    10, fe1600_interrupt_handle_mac_rxcrcerrn_int_1 ,fe1600_interrupt_recurring_action_handle_mac_rxcrcerrn_int_1);
    interrupt_add_interrupt_handler(unit,      FE1600_INT_MAC_RXCRCERRN_INT_2,   100,    10, fe1600_interrupt_handle_mac_rxcrcerrn_int_2 ,fe1600_interrupt_recurring_action_handle_mac_rxcrcerrn_int_2);
    interrupt_add_interrupt_handler(unit,      FE1600_INT_MAC_RXCRCERRN_INT_3,   100,    10, fe1600_interrupt_handle_mac_rxcrcerrn_int_3 ,fe1600_interrupt_recurring_action_handle_mac_rxcrcerrn_int_3);
    interrupt_add_interrupt_handler(unit,      FE1600_INT_MAC_WRONGSIZE_INT_0,   100,    10, fe1600_interrupt_handle_mac_wrongsize_int_0 ,fe1600_interrupt_recurring_action_handle_mac_wrongsize_int_0);
    interrupt_add_interrupt_handler(unit,      FE1600_INT_MAC_WRONGSIZE_INT_1,   100,    10, fe1600_interrupt_handle_mac_wrongsize_int_1 ,fe1600_interrupt_recurring_action_handle_mac_wrongsize_int_1);
    interrupt_add_interrupt_handler(unit,      FE1600_INT_MAC_WRONGSIZE_INT_2,   100,    10, fe1600_interrupt_handle_mac_wrongsize_int_2 ,fe1600_interrupt_recurring_action_handle_mac_wrongsize_int_2);
    interrupt_add_interrupt_handler(unit,      FE1600_INT_MAC_WRONGSIZE_INT_3,   100,    10, fe1600_interrupt_handle_mac_wrongsize_int_3 ,fe1600_interrupt_recurring_action_handle_mac_wrongsize_int_3);
    interrupt_add_interrupt_handler(unit,            FE1600_INT_MAC_LOS_INT_0,     0,     0, fe1600_interrupt_handle_mac_los_int_0 , NULL);
    interrupt_add_interrupt_handler(unit,            FE1600_INT_MAC_LOS_INT_1,     0,     0, fe1600_interrupt_handle_mac_los_int_1 , NULL);
    interrupt_add_interrupt_handler(unit,            FE1600_INT_MAC_LOS_INT_2,     0,     0, fe1600_interrupt_handle_mac_los_int_2 , NULL);
    interrupt_add_interrupt_handler(unit,            FE1600_INT_MAC_LOS_INT_3,     0,     0, fe1600_interrupt_handle_mac_los_int_3 , NULL);
    interrupt_add_interrupt_handler(unit,       FE1600_INT_MAC_RXLOSTOFSYNC_0,     0,     0, fe1600_interrupt_handle_mac_rxlostofsync_0 , NULL);
    interrupt_add_interrupt_handler(unit,       FE1600_INT_MAC_RXLOSTOFSYNC_1,     0,     0, fe1600_interrupt_handle_mac_rxlostofsync_1 , NULL);
    interrupt_add_interrupt_handler(unit,       FE1600_INT_MAC_RXLOSTOFSYNC_2,     0,     0, fe1600_interrupt_handle_mac_rxlostofsync_2 , NULL);
    interrupt_add_interrupt_handler(unit,       FE1600_INT_MAC_RXLOSTOFSYNC_3,     0,     0, fe1600_interrupt_handle_mac_rxlostofsync_3 , NULL);
    interrupt_add_interrupt_handler(unit,     FE1600_INT_MAC_LNKLVLAGEN_INT_0,     0,     0, fe1600_interrupt_handle_mac_lnklvlagen_int_0 , NULL);
    interrupt_add_interrupt_handler(unit,     FE1600_INT_MAC_LNKLVLAGEN_INT_1,     0,     0, fe1600_interrupt_handle_mac_lnklvlagen_int_1 , NULL);
    interrupt_add_interrupt_handler(unit,     FE1600_INT_MAC_LNKLVLAGEN_INT_2,     0,     0, fe1600_interrupt_handle_mac_lnklvlagen_int_2 , NULL);
    interrupt_add_interrupt_handler(unit,     FE1600_INT_MAC_LNKLVLAGEN_INT_3,     0,     0, fe1600_interrupt_handle_mac_lnklvlagen_int_3 , NULL);
    interrupt_add_interrupt_handler(unit,            FE1600_INT_MAC_OOF_INT_0,    10,    10, fe1600_interrupt_handle_mac_oof_int_0 ,fe1600_interrupt_recurring_action_handle_mac_oof_int_0);
    interrupt_add_interrupt_handler(unit,            FE1600_INT_MAC_OOF_INT_1,    10,    10, fe1600_interrupt_handle_mac_oof_int_1 ,fe1600_interrupt_recurring_action_handle_mac_oof_int_1);
    interrupt_add_interrupt_handler(unit,            FE1600_INT_MAC_OOF_INT_2,    10,    10, fe1600_interrupt_handle_mac_oof_int_2 ,fe1600_interrupt_recurring_action_handle_mac_oof_int_2);
    interrupt_add_interrupt_handler(unit,            FE1600_INT_MAC_OOF_INT_3,    10,    10, fe1600_interrupt_handle_mac_oof_int_3 ,fe1600_interrupt_recurring_action_handle_mac_oof_int_3);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_MAC_DECERR_INT_0,     0,     0, fe1600_interrupt_handle_mac_decerr_int_0 , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_MAC_DECERR_INT_1,     0,     0, fe1600_interrupt_handle_mac_decerr_int_1 , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_MAC_DECERR_INT_2,     0,     0, fe1600_interrupt_handle_mac_decerr_int_2 , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_MAC_DECERR_INT_3,     0,     0, fe1600_interrupt_handle_mac_decerr_int_3 , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_MAC_TRANSMITERR_INT_0,     0,     0, fe1600_interrupt_handle_mac_transmiterr_int_0 , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_MAC_TRANSMITERR_INT_1,     0,     0, fe1600_interrupt_handle_mac_transmiterr_int_1 , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_MAC_TRANSMITERR_INT_2,     0,     0, fe1600_interrupt_handle_mac_transmiterr_int_2 , NULL);
    interrupt_add_interrupt_handler(unit,    FE1600_INT_MAC_TRANSMITERR_INT_3,     0,     0, fe1600_interrupt_handle_mac_transmiterr_int_3 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_MAC_RXCTRLOVERFLOW_INT_0,     0,     0, fe1600_interrupt_handle_mac_rxctrloverflow_int_0 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_MAC_RXCTRLOVERFLOW_INT_1,     0,     0, fe1600_interrupt_handle_mac_rxctrloverflow_int_1 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_MAC_RXCTRLOVERFLOW_INT_2,     0,     0, fe1600_interrupt_handle_mac_rxctrloverflow_int_2 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_MAC_RXCTRLOVERFLOW_INT_3,     0,     0, fe1600_interrupt_handle_mac_rxctrloverflow_int_3 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_MAC_LNKLVLAGECTXBN_INT_0,     0,     0, fe1600_interrupt_handle_mac_lnklvlagectxbn_int_0 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_MAC_LNKLVLAGECTXBN_INT_1,     0,     0, fe1600_interrupt_handle_mac_lnklvlagectxbn_int_1 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_MAC_LNKLVLAGECTXBN_INT_2,     0,     0, fe1600_interrupt_handle_mac_lnklvlagectxbn_int_2 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_MAC_LNKLVLAGECTXBN_INT_3,     0,     0, fe1600_interrupt_handle_mac_lnklvlagectxbn_int_3 , NULL);
    interrupt_add_interrupt_handler(unit,  FE1600_INT_MAC_TXFDRCIFFAULT_INT_0,     0,     0, fe1600_interrupt_handle_mac_txfdrciffault_int_0 , NULL);
    interrupt_add_interrupt_handler(unit,  FE1600_INT_MAC_TXFDRCIFFAULT_INT_1,     0,     0, fe1600_interrupt_handle_mac_txfdrciffault_int_1 , NULL);
    interrupt_add_interrupt_handler(unit,  FE1600_INT_MAC_TXFDRCIFFAULT_INT_2,     0,     0, fe1600_interrupt_handle_mac_txfdrciffault_int_2 , NULL);
    interrupt_add_interrupt_handler(unit,  FE1600_INT_MAC_TXFDRCIFFAULT_INT_3,     0,     0, fe1600_interrupt_handle_mac_txfdrciffault_int_3 , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_MAC_ECC_1BERRINT,     0,     0, fe1600_interrupt_handle_mac_ecc_1berrint , NULL);
    interrupt_add_interrupt_handler(unit,         FE1600_INT_MAC_ECC_2BERRINT,     0,     0, fe1600_interrupt_handle_mac_ecc_2berrint , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_0 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_1,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_1 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_2,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_2 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_3,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_3 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_0,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_0 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_1,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_1 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_2,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_2 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_1_SYNCSTATUSCHANGED_3,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_3 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_0,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_0 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_1,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_1 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_2,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_2 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_2_SYNCSTATUSCHANGED_3,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_3 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_0,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_0 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_1,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_1 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_2,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_2 , NULL);
    interrupt_add_interrupt_handler(unit, FE1600_INT_SRD_FSRD_3_SYNCSTATUSCHANGED_3,     0,     0, fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_3 , NULL);
}


/*
 * Generic None handles - for CB without specific handling
 */
int
fe1600_interrupt_handle_generic_none(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    
    SOC_INIT_FUNC_DEFS;

    /* data collection */
    
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_generic_none(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    
    SOC_INIT_FUNC_DEFS;

    /* data collection */
    
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

/*
 * interrupt handeler functions.
 */
int
fe1600_interrupt_handle_rtp_linkintegritychangedint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    soc_reg_above_64_val_t integrity_vec;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    SOC_INIT_FUNC_DEFS;
    rc = READ_RTP_LINK_INTEGRITY_VECTORr(unit, integrity_vec);
    _SOC_IF_ERR_EXIT(rc);  
    sal_sprintf(special_msg, "RTP_LINK_INTEGRITY_VECTOR = 0x%x%x%x%x", integrity_vec[0], integrity_vec[1], integrity_vec[2], integrity_vec[3]);
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_ALL_REACHABLE_FIX, special_msg);
    _SOC_IF_ERR_EXIT(rc);
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_ALL_REACHABLE_FIX, &integrity_vec, NULL);
    _SOC_IF_ERR_EXIT(rc);
exit:
    SOC_FUNC_RETURN;
}
 int
fe1600_interrupt_handle_eci_serint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    uint32 err_addr,err_cnt;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = READ_ECI_PARITY_ERR_ADDRr(unit, &err_addr); 
    _SOC_IF_ERR_EXIT(rc);

    rc = soc_reg32_get(unit, ECI_PARITY_ERR_CNTr, block_instance, 0, &err_cnt);
    _SOC_IF_ERR_EXIT(rc);  

    sal_sprintf(special_msg, "ECI_PARITY_ERR_ADDR = 0x%08x,ECI_PARITY_ERR_CNT = 0x%08x",err_addr, err_cnt);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_eci_dch0intforce(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_FORCE, NULL);
    _SOC_IF_ERR_EXIT(rc);



    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_FORCE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_eci_dch1intforce(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_FORCE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_FORCE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_eci_dch2intforce(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_FORCE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_FORCE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_eci_dch3intforce(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_FORCE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_FORCE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_dchdesccntop(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_ifmafopint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_dch_ifm( unit,block_instance,en_fe1600_interrupt, &corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);
    
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action,special_msg );
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, corrective_action , NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dch_ifmafopint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_dch_ifm(unit, block_instance, en_fe1600_interrupt, &corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_ifmbfopint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_dch_ifm(unit, block_instance, en_fe1600_interrupt, &corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);
    
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, corrective_action , NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dch_ifmbfopint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_dch_ifm(unit, block_instance, en_fe1600_interrupt, &corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_cpudatacellfnea0pint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    uint32 fifo_status_p;
    uint32 cpu_data_cell_foa_0_pf;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    char *p_message;

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = READ_DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr(unit, block_instance, &fifo_status_p);
    _SOC_IF_ERR_EXIT(rc);
    
    cpu_data_cell_foa_0_pf = soc_reg_field_get(unit, DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr, fifo_status_p, CPU_DATA_CELL_FOA_0_Pf);
    if(cpu_data_cell_foa_0_pf) {
        sal_sprintf(special_msg, "At list one cell toward CPU got droped");
        p_message = special_msg;
    } else {
        p_message = NULL;
    }

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, p_message);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_cpudatacellfnea1pint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    uint32 fifo_status_p;
    uint32 cpu_data_cell_foa_1_pf;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    char* p_message;

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = READ_DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr(unit, block_instance, &fifo_status_p);
    _SOC_IF_ERR_EXIT(rc);

    cpu_data_cell_foa_1_pf = soc_reg_field_get(unit, DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Pr, fifo_status_p, CPU_DATA_CELL_FOA_1_Pf);
    if(cpu_data_cell_foa_1_pf) {
        sal_sprintf(special_msg, "At list one cell toward CPU got droped");
        p_message = special_msg;
    } else {
        p_message = NULL;
    }

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, p_message);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_altopint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dch_altopint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{

    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;
    dcmn_int_corr_act_type corrective_action ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links, FE1600_INT_LINK_INVOLVED, (SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_recuring_action_dch_alto(unit, block_instance, en_fe1600_interrupt, &corrective_action, array_links, special_msg);
    _SOC_IF_ERR_EXIT(rc); 
    
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,corrective_action,array_links,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;

}

int
fe1600_interrupt_handle_dch_unreachdestevpint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    fe1600_interrupt_mc_rtp_table_correct_info mc_rtp_table_correct_info;
    dcmn_int_corr_act_type corrective_action;
    int unicast_index;
    void *param1;
    
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOC_INIT_FUNC_DEFS;
    
    corrective_action = DCMN_INT_CORR_ACT_NONE;
    param1 = NULL;
    
    /* data collection */

    rc = fe1600_interrupt_data_collection_for_handle_unreachable_destination(unit, block_instance, en_fe1600_interrupt, special_msg, &corrective_action, &unicast_index, &mc_rtp_table_correct_info);
    _SOC_IF_ERR_EXIT(rc);

    if(DCMN_INT_CORR_ACT_MC_RTP_CORRECT == corrective_action) {
        param1 = (void*)(&mc_rtp_table_correct_info);
    }
    else if (DCMN_INT_CORR_ACT_UC_RTP_CORRECT == corrective_action) {
        param1 = (void*)(&unicast_index);
    }

    /* print info */

    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */

    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, corrective_action, param1, NULL);
    _SOC_IF_ERR_EXIT(rc);

    
exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_dchunexperror(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_dchdesccntos(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_ifmafosint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_dch_ifm(unit, block_instance, en_fe1600_interrupt, &corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);
    
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, corrective_action , NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dch_ifmafosint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_dch_ifm(unit, block_instance, en_fe1600_interrupt, &corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_ifmbfosint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_dch_ifm(unit, block_instance, en_fe1600_interrupt, &corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dch_ifmbfosint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_dch_ifm(unit, block_instance, en_fe1600_interrupt, &corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_cpudatacellfnea0sint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    uint32 fifo_status_s;
    uint32 cpu_data_cell_foa_0_sf; 
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    char* p_message;

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = READ_DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr(unit, block_instance, &fifo_status_s);
    _SOC_IF_ERR_EXIT(rc);

    cpu_data_cell_foa_0_sf = soc_reg_field_get(unit, DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr, fifo_status_s, CPU_DATA_CELL_FOA_0_Sf);
    if (cpu_data_cell_foa_0_sf) {
        sal_sprintf(special_msg, "At list one cell toward CPU got droped");
        p_message = special_msg;
    } else {
        p_message = NULL;
    }

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, p_message);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_cpudatacellfnea1sint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    uint32 fifo_status_p;
    uint32 cpu_data_cell_foa_1_sf; 
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    char* p_message;

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = READ_DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr(unit, block_instance, &fifo_status_p);
    _SOC_IF_ERR_EXIT(rc);

    cpu_data_cell_foa_1_sf = soc_reg_field_get(unit, DCH_DCH_OVERFLOWS_AND_FIFOS_STATUSES_Sr, fifo_status_p, CPU_DATA_CELL_FOA_1_Sf);
    if(cpu_data_cell_foa_1_sf) {
        sal_sprintf(special_msg, "At list one cell toward CPU got droped");
        p_message = special_msg;
    } else {
        p_message = NULL;
    }

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, p_message);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_altosint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    
    SOC_INIT_FUNC_DEFS;

    /* data collection */

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dch_altosint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{

    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;
    dcmn_int_corr_act_type corrective_action ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
 
    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links, FE1600_INT_LINK_INVOLVED , ( SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_recuring_action_dch_alto(unit, block_instance, en_fe1600_interrupt, &corrective_action, array_links, special_msg);
    _SOC_IF_ERR_EXIT(rc); 
    
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,corrective_action,array_links ,NULL);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_unreachdestevsint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    fe1600_interrupt_mc_rtp_table_correct_info mc_rtp_table_correct_info;
    dcmn_int_corr_act_type corrective_action;
    int unicast_index;
    void *param1;
    
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOC_INIT_FUNC_DEFS;
    
    corrective_action = DCMN_INT_CORR_ACT_NONE;
    param1 = NULL;
    
    /* data collection */

    rc = fe1600_interrupt_data_collection_for_handle_unreachable_destination(unit, block_instance, en_fe1600_interrupt, special_msg, &corrective_action, &unicast_index, &mc_rtp_table_correct_info);
    _SOC_IF_ERR_EXIT(rc);

    if(DCMN_INT_CORR_ACT_MC_RTP_CORRECT == corrective_action) {
        param1 = (void*)(&mc_rtp_table_correct_info);
    }
    else if (DCMN_INT_CORR_ACT_UC_RTP_CORRECT == corrective_action) {
        param1 = (void*)(&unicast_index);
    }

    /* print info */

    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */

    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, corrective_action, param1, NULL);
    _SOC_IF_ERR_EXIT(rc);

    
exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_outofsyncintp(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_outofsyncints(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_ecc_1berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dch_ecc_1berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_ecc_2berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_parityerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_errorfilterpint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    uint32 dch_error_filter_counter;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = READ_DCH_ERROR_FILTER_CNT_Pr(unit, block_instance, &dch_error_filter_counter);
    _SOC_IF_ERR_EXIT(rc);

    /* prepare message */
    sal_sprintf(special_msg, "Dch_err_filter_counter_p=0x%08x",dch_error_filter_counter);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dch_errorfiltersint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    uint32 dch_error_filter_counter;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = READ_DCH_ERROR_FILTER_CNT_Sr(unit, block_instance, &dch_error_filter_counter);
    _SOC_IF_ERR_EXIT(rc);

    /* prepare message */
    sal_sprintf(special_msg, "Dch_err_filter_counter_s=0x%08x",dch_error_filter_counter);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}


int
fe1600_interrupt_handle_dcl_ccp0srcdvcnglinkint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_dcl_ccpsrcdcnglink_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dcl_ccp0srcdvcnglinkint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_dcl_ccpsrcdcnglink_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_PRINT, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_PRINT, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcl_ccp1srcdvcnglinkint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_dcl_ccpsrcdcnglink_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dcl_ccp1srcdvcnglinkint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_dcl_ccpsrcdcnglink_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_PRINT, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_PRINT, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcl_ccp0chfovfint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    SOC_INIT_FUNC_DEFS;
    /* data collection */

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);
exit:
    SOC_FUNC_RETURN;
}
int
fe1600_interrupt_recurring_action_handle_dcl_ccp0chfovfint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    SOC_INIT_FUNC_DEFS;
    /* data collection */

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);
exit:
    SOC_FUNC_RETURN;
}
int
fe1600_interrupt_handle_dcl_ccp1chfovfint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dcl_ccp1chfovfint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    SOC_INIT_FUNC_DEFS;
    /* data collection */
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);
    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);
exit:
    SOC_FUNC_RETURN;
}
int
fe1600_interrupt_handle_dcl_ccp0clfovfint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    SOC_INIT_FUNC_DEFS;
    /* data collection */
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);
    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);
exit:
    SOC_FUNC_RETURN;
}
int
fe1600_interrupt_recurring_action_handle_dcl_ccp0clfovfint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcl_ccp1clfovfint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dcl_ccp1clfovfint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcl_dclutagparerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dcl_dclutagparerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcl_dclmtagparerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dcl_dclmtagparerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcl_macadatacrcerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dcl_macadatacrcerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcl_macbdatacrcerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dcl_macbdatacrcerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcl_ecc_1berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dcl_ecc_1berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcl_ecc_2berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcl_parityerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcl_outofsyncint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcma_ecc_1berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    uint32 dcma_ecc_1b_err_counter;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = READ_DCMA_ECC_1B_ERR_CNTr(unit, block_instance, &dcma_ecc_1b_err_counter);
    _SOC_IF_ERR_EXIT(rc);

    /* prepare message */
    sal_sprintf(special_msg, "Ecc_err_1b_counter=0x%08x",dcma_ecc_1b_err_counter );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE,special_msg );
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dcma_ecc_1berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    uint32 dcma_ecc_1b_err_counter;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = READ_DCMA_ECC_1B_ERR_CNTr(unit, block_instance, &dcma_ecc_1b_err_counter);
    _SOC_IF_ERR_EXIT(rc);

    /* prepare message */
    sal_sprintf(special_msg, "Ecc_err_1b_counter=0x%08x",dcma_ecc_1b_err_counter );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET ,special_msg );
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcma_ecc_2berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    uint32 dcma_ecc_2b_err_counter;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = READ_DCMA_ECC_2B_ERR_CNTr(unit, block_instance, &dcma_ecc_2b_err_counter);
    _SOC_IF_ERR_EXIT(rc);

    /* prepare message */
    sal_sprintf(special_msg, "Ecc_err_2b_counter=0x%08x",dcma_ecc_2b_err_counter );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcma_parityerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcma_outofsyncpint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcma_outofsyncsint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcmb_ecc_1berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    uint32 dcmb_ecc_1b_err_counter;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = READ_DCMB_ECC_1B_ERR_CNTr(unit, block_instance, &dcmb_ecc_1b_err_counter);
    _SOC_IF_ERR_EXIT(rc);

    /* prepare message */
    sal_sprintf(special_msg, "Ecc_err_1b_counter=0x%08x",dcmb_ecc_1b_err_counter );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE,special_msg );
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_dcmb_ecc_1berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    uint32 dcmb_ecc_1b_err_counter;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = READ_DCMB_ECC_1B_ERR_CNTr(unit, block_instance, &dcmb_ecc_1b_err_counter);
    _SOC_IF_ERR_EXIT(rc);

    /* prepare message */
    sal_sprintf(special_msg, "Ecc_err_1b_counter=0x%08x",dcmb_ecc_1b_err_counter );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET,special_msg );
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcmb_ecc_2berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    uint32 dcmb_ecc_2b_err_counter;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = READ_DCMB_ECC_2B_ERR_CNTr(unit, block_instance, &dcmb_ecc_2b_err_counter);
    _SOC_IF_ERR_EXIT(rc);

    /* prepare message */
    sal_sprintf(special_msg, "Ecc_err_2b_counter=0x%08x",dcmb_ecc_2b_err_counter );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcmb_parityerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcmb_outofsyncpint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_dcmb_outofsyncsint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_ccs_cpucaptcellfneint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_ccs_cpucaptcellfneint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, special_msg, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_ccs_unrchdstint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_ccs_ecc_2berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_ccs_ecc_2berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_ccs_parityerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_ccs_parityerrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_rtp_scrub_shadow_write(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    fe1600_interrupt_mc_rtp_table_correct_info mc_rtp_table_correct_info;
    dcmn_int_corr_act_type corrective_action;
    dcmn_interrupt_mem_err_info  shadow_correct_info;
    int unicast_index;
    void *param1;

    SOC_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;
    param1 = NULL;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_err(unit, block_instance, en_fe1600_interrupt, special_msg, &corrective_action, &unicast_index, &mc_rtp_table_correct_info, &shadow_correct_info);
    _SOC_IF_ERR_EXIT(rc);

    if(DCMN_INT_CORR_ACT_MC_RTP_CORRECT == corrective_action) {
        param1 = (void*)(&mc_rtp_table_correct_info);
    } else if (DCMN_INT_CORR_ACT_UC_RTP_CORRECT == corrective_action) {
        param1 = (void*)(&unicast_index);
    } else if ((DCMN_INT_CORR_ACT_SHADOW == corrective_action) ||
               (DCMN_INT_CORR_ACT_ECC_1B_FIX == corrective_action) ||
               (DCMN_INT_CORR_ACT_RTP_SLSCT == corrective_action)){
        param1 = (void*)(&shadow_correct_info);
    }

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, corrective_action, param1, NULL);
    _SOC_IF_ERR_EXIT(rc);
 
exit:
    SOC_FUNC_RETURN;
}

int fe1600_interrupt_handle_rtp_drhp_0_querydestinationacove1kint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationacove1kint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;

}

int fe1600_interrupt_handle_rtp_drhp_1_querydestinationacove1kint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationacove1kint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;

}

int fe1600_interrupt_handle_rtp_drhp_2_querydestinationacove1kint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationacove1kint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;

}

int fe1600_interrupt_handle_rtp_drhp_3_querydestinationacove1kint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationacove1kint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;

}

int fe1600_interrupt_handle_rtp_drhs_0_querydestinationacove1kint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationacove1kint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;

}


int fe1600_interrupt_handle_rtp_drhs_1_querydestinationacove1kint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationacove1kint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;

}

int fe1600_interrupt_handle_rtp_drhs_2_querydestinationacove1kint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationacove1kint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;

}

int fe1600_interrupt_handle_rtp_drhs_3_querydestinationacove1kint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationacove1kint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;

}

int fe1600_interrupt_handle_rtp_crh_0_querydestinationacove1kint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationacove1kint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;

}

int fe1600_interrupt_handle_rtp_crh_1_querydestinationacove1kint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationacove1kint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;

}

int 
fe1600_interrupt_handle_rtp_crh_0_querydestinationabovemaxbaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationabovemaxbaseindexint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_crh_1_querydestinationabovemaxbaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationabovemaxbaseindexint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}



int 
fe1600_interrupt_handle_rtp_drhp_0_querydestinationaboveupdatebaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationaboveupdatebaseindexint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhp_1_querydestinationaboveupdatebaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationaboveupdatebaseindexint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhp_2_querydestinationaboveupdatebaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationaboveupdatebaseindexint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}



int 
fe1600_interrupt_handle_rtp_drhp_3_querydestinationaboveupdatebaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationaboveupdatebaseindexint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhs_0_querydestinationaboveupdatebaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationaboveupdatebaseindexint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhs_1_querydestinationaboveupdatebaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationaboveupdatebaseindexint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhs_2_querydestinationaboveupdatebaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationaboveupdatebaseindexint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhs_3_querydestinationaboveupdatebaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationaboveupdatebaseindexint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_crh_0_querydestinationaboveupdatebaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationaboveupdatebaseindexint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_crh_1_querydestinationaboveupdatebaseindexint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationaboveupdatebaseindexint(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhp_0_queryunreachablemulticastint(

    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drhp_queryunreachablemulticast(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhp_1_queryunreachablemulticastint(

    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drhp_queryunreachablemulticast(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhp_2_queryunreachablemulticastint(

    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drhp_queryunreachablemulticast(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhp_3_queryunreachablemulticastint(

    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drhp_queryunreachablemulticast(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhs_0_queryunreachablemulticastint(

    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drhs_queryunreachablemulticast(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhs_1_queryunreachablemulticastint(

    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drhs_queryunreachablemulticast(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhs_2_queryunreachablemulticastint(

    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drhs_queryunreachablemulticast(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int 
fe1600_interrupt_handle_rtp_drhs_3_queryunreachablemulticastint(

    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_rtp_drhs_queryunreachablemulticast(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_rxcrcerrn_int_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{

    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_rxcrcerrn_int(unit, block_instance, en_fe1600_interrupt, array_links, special_msg);
    _SOC_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, (void*)array_links ,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_mac_rxcrcerrn_int_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{

    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,FE1600_INT_LINK_INVOLVED , (SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_rxcrcerrn_int(unit, block_instance, en_fe1600_interrupt, array_links, special_msg);
    _SOC_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_LINKS, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_SHUTDOWN_LINKS, (void*)array_links ,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}


int
fe1600_interrupt_handle_mac_rxcrcerrn_int_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_rxcrcerrn_int(unit, block_instance, en_fe1600_interrupt, array_links, special_msg);
    _SOC_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */

    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, (void*)array_links ,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;    
}

int
fe1600_interrupt_recurring_action_handle_mac_rxcrcerrn_int_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,FE1600_INT_LINK_INVOLVED , ( SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_rxcrcerrn_int(unit, block_instance, en_fe1600_interrupt, array_links, special_msg);
    _SOC_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_LINKS, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_SHUTDOWN_LINKS, (void*)array_links ,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_rxcrcerrn_int_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_rxcrcerrn_int(unit, block_instance, en_fe1600_interrupt, array_links, special_msg);
    _SOC_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */

    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, (void*)array_links ,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_mac_rxcrcerrn_int_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,FE1600_INT_LINK_INVOLVED , ( SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_rxcrcerrn_int(unit, block_instance, en_fe1600_interrupt, array_links, special_msg);
    _SOC_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_LINKS, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */

    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_SHUTDOWN_LINKS,(void*)array_links ,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}


int
fe1600_interrupt_handle_mac_rxcrcerrn_int_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_rxcrcerrn_int(unit, block_instance, en_fe1600_interrupt, array_links, special_msg);
    _SOC_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */

    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, (void*)array_links ,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_mac_rxcrcerrn_int_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,FE1600_INT_LINK_INVOLVED , ( SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_rxcrcerrn_int(unit, block_instance, en_fe1600_interrupt, array_links, special_msg);
    _SOC_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_LINKS, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */

    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_SHUTDOWN_LINKS,(void*)array_links ,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_wrongsize_int_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_wrongsize_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_mac_wrongsize_int_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc ;
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;

    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,FE1600_INT_LINK_INVOLVED , ( SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_recurring_action_mac_wrongsize_int(unit, block_instance, en_fe1600_interrupt, special_msg , array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_LINKS, special_msg );
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,DCMN_INT_CORR_ACT_SHUTDOWN_LINKS,array_links,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_wrongsize_int_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */

    rc = fe1600_interrupt_data_collection_for_handle_mac_wrongsize_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_mac_wrongsize_int_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc ;
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;

    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links, FE1600_INT_LINK_INVOLVED, (SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc =fe1600_interrupt_data_collection_for_recurring_action_mac_wrongsize_int(unit, block_instance, en_fe1600_interrupt, special_msg , array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_LINKS, special_msg );
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,DCMN_INT_CORR_ACT_SHUTDOWN_LINKS,array_links,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_wrongsize_int_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */

    rc = fe1600_interrupt_data_collection_for_handle_mac_wrongsize_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_mac_wrongsize_int_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc ;
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;

    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    
    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links, FE1600_INT_LINK_INVOLVED, (SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_recurring_action_mac_wrongsize_int(unit, block_instance, en_fe1600_interrupt, special_msg , array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_LINKS, special_msg );
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,DCMN_INT_CORR_ACT_SHUTDOWN_LINKS,array_links,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_wrongsize_int_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */

    rc = fe1600_interrupt_data_collection_for_handle_mac_wrongsize_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_mac_wrongsize_int_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc ;
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;

    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    
    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links, FE1600_INT_LINK_INVOLVED, (SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_recurring_action_mac_wrongsize_int(unit, block_instance, en_fe1600_interrupt, special_msg , array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_LINKS, special_msg );
    _SOC_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,DCMN_INT_CORR_ACT_SHUTDOWN_LINKS,array_links,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_los_int_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_los_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_los_int_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_los_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_los_int_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_los_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_los_int_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_los_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_rxlostofsync_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    soc_port_t link;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_rxlostofsync_int(unit, block_instance, en_fe1600_interrupt, &link, special_msg);
    _SOC_IF_ERR_EXIT(rc);
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &link, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_rxlostofsync_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    soc_port_t link;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_rxlostofsync_int(unit, block_instance, en_fe1600_interrupt, &link, special_msg);
    _SOC_IF_ERR_EXIT(rc);
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &link, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_rxlostofsync_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    soc_port_t link;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_rxlostofsync_int(unit, block_instance, en_fe1600_interrupt, &link, special_msg);
    _SOC_IF_ERR_EXIT(rc);
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &link, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_rxlostofsync_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    soc_port_t link;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;
    
    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_rxlostofsync_int(unit, block_instance, en_fe1600_interrupt, &link, special_msg);
    _SOC_IF_ERR_EXIT(rc);
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &link, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_lnklvlagen_int_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_mac_lnklvlagen_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_lnklvlagen_int_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    /*int link;*/
    int rc;

    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_mac_lnklvlagen_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_lnklvlagen_int_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    /*int link;*/
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_mac_lnklvlagen_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_lnklvlagen_int_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    /*int link;*/
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_mac_lnklvlagen_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_oof_int_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_oof_int( unit, block_instance, en_fe1600_interrupt, &pcs, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_mac_oof_int_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{

    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,FE1600_INT_LINK_INVOLVED , ( SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_recurring_mac_oof_int( unit, block_instance,en_fe1600_interrupt, &pcs, &corrective_action, special_msg, array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,corrective_action,array_links ,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;

}

int
fe1600_interrupt_handle_mac_oof_int_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_oof_int( unit, block_instance, en_fe1600_interrupt, &pcs, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_mac_oof_int_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS];
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,FE1600_INT_LINK_INVOLVED , ( SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_recurring_mac_oof_int( unit,block_instance,en_fe1600_interrupt, &pcs, &corrective_action, special_msg, array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, corrective_action,special_msg );
    _SOC_IF_ERR_EXIT(rc);

    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,corrective_action,array_links ,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;

}

int
fe1600_interrupt_handle_mac_oof_int_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_oof_int( unit, block_instance, en_fe1600_interrupt, &pcs, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_mac_oof_int_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS];
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,FE1600_INT_LINK_INVOLVED , ( SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_recurring_mac_oof_int( unit,block_instance,en_fe1600_interrupt, &pcs, &corrective_action, special_msg, array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, corrective_action,special_msg );
    _SOC_IF_ERR_EXIT(rc);

    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,corrective_action,array_links ,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_oof_int_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_oof_int( unit, block_instance, en_fe1600_interrupt, &pcs, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_recurring_action_handle_mac_oof_int_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS];
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,FE1600_INT_LINK_INVOLVED , ( SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_recurring_mac_oof_int( unit,block_instance,en_fe1600_interrupt, &pcs, &corrective_action, special_msg, array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 1, corrective_action,special_msg );
    _SOC_IF_ERR_EXIT(rc);

    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,corrective_action,array_links ,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_decerr_int_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS];
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,FE1600_INT_LINK_INVOLVED , ( SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_decerr_int(unit, block_instance, en_fe1600_interrupt, &pcs, &corrective_action, special_msg, array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action, special_msg );
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,corrective_action,array_links,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_decerr_int_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS];
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,FE1600_INT_LINK_INVOLVED , ( SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_decerr_int(unit, block_instance, en_fe1600_interrupt, &pcs, &corrective_action, special_msg, array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action, special_msg );
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,corrective_action,array_links,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_decerr_int_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS];
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,FE1600_INT_LINK_INVOLVED , ( SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_decerr_int(unit, block_instance, en_fe1600_interrupt, &pcs, &corrective_action, special_msg, array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action, special_msg );
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,corrective_action,array_links,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_decerr_int_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS];
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,FE1600_INT_LINK_INVOLVED , ( SOC_FE1600_NOF_LINKS * sizeof(char)));

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_handle_mac_decerr_int(unit, block_instance, en_fe1600_interrupt, &pcs, &corrective_action, special_msg, array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action, special_msg );
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg,corrective_action,array_links,NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_transmiterr_int_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_mac_transmit_err_int(unit, block_instance, en_fe1600_interrupt, special_msg, &corrective_action, array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, corrective_action, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_transmiterr_int_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_mac_transmit_err_int(unit, block_instance, en_fe1600_interrupt, special_msg, &corrective_action, array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, corrective_action, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_transmiterr_int_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_mac_transmit_err_int(unit, block_instance, en_fe1600_interrupt, special_msg, &corrective_action, array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, corrective_action, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_transmiterr_int_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];
    /* links array for corrective action */    
    uint8 array_links[SOC_FE1600_NOF_LINKS] ;

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_mac_transmit_err_int(unit, block_instance, en_fe1600_interrupt, special_msg, &corrective_action, array_links);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, corrective_action, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, corrective_action, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_rxctrloverflow_int_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_rxctrloverflow_int_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_rxctrloverflow_int_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_rxctrloverflow_int_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_lnklvlagectxbn_int_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    /*int link;*/
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_mac_lnklvlagectxbn_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_lnklvlagectxbn_int_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    /*int link;*/
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_mac_lnklvlagectxbn_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_lnklvlagectxbn_int_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    /*int link;*/
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_mac_lnklvlagectxbn_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_lnklvlagectxbn_int_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    /*int link;*/
    int rc;
    /* message for syslog */
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_mac_lnklvlagectxbn_int(unit, block_instance, en_fe1600_interrupt, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_txfdrciffault_int_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_txfdrciffault_int_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_txfdrciffault_int_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_txfdrciffault_int_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_ecc_1berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;

    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_mac_ecc_2berrint(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    
    SOC_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_0(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    soc_port_t link;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_srd_fsrd_syncstatuschanged(unit, block_instance, en_fe1600_interrupt, &link, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &link, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_1(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    soc_port_t link;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_srd_fsrd_syncstatuschanged(unit, block_instance, en_fe1600_interrupt, &link, special_msg);
    _SOC_IF_ERR_EXIT(rc);
    
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &link, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_2(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    soc_port_t link;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_srd_fsrd_syncstatuschanged(unit, block_instance, en_fe1600_interrupt, &link, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &link, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

int
fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_3(
    int unit,
    int block_instance,
    fe1600_interrupt_type en_fe1600_interrupt,
    char *msg)
{
    int rc;
    soc_port_t link;
    char special_msg[FE1600_INTERRUPT_SPECIAL_MSG_SIZE];

    SOC_INIT_FUNC_DEFS;

    /* data collection */
    rc = fe1600_interrupt_data_collection_for_srd_fsrd_syncstatuschanged(unit, block_instance, en_fe1600_interrupt, &link, special_msg);
    _SOC_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe1600_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, NULL);
    _SOC_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = fe1600_interrupt_handles_corrective_action(unit, block_instance, en_fe1600_interrupt, msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &link, NULL);
    _SOC_IF_ERR_EXIT(rc);

exit:
    SOC_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

