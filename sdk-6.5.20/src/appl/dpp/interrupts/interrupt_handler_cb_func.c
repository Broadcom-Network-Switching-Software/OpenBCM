/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    interrupt_handler_cb_func.c
 * Purpose:    Implement CallBacks function for ARAD interrupts.
 */
 
 #ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_BCM_INTR

/* 
 *  include  
 */ 
#include <shared/bsl.h>
#include <sal/core/time.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/ARAD/arad_interrupts.h>
#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/dpp/ARAD/arad_defs.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_em_ser.h>

#include <soc/dpp/drv.h>

#include <bcm/error.h>
#include <bcm/debug.h>

#include <bcm_int/common/debug.h>

#include <appl/diag/system.h>

#include <appl/dpp/interrupts/interrupt_handler.h>
#include <appl/dpp/interrupts/interrupt_handler_cb_func.h>
#include <appl/dpp/interrupts/interrupt_handler_corr_act_func.h>

#include <appl/dcmn/interrupts/interrupt_handler.h>


#define ARAD_INT_FDRPRMALTOA_COR_ITERATION 50
#define ARAD_INT_FDRPRMALTOA_COR_DELAY_US  10000;
/*
 * Init Functions
 */
void arad_interrupt_add_interrupt_handler_init(int unit)
{
    /* Init ARAD interrupt data base */

    interrupt_add_interrupt_handler(unit,                              ARAD_INT_CFC_HCFCOOBRX0CRCERR,     0,     0,                              arad_interrupt_handle_cfchcfcoobrx0crcerr, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_CFC_HCFCOOBRX0HEADERERR,     0,     0,                           arad_interrupt_handle_cfchcfcoobrx0headererr, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_CFC_HCFCOOBRX1CRCERR,     0,     0,                              arad_interrupt_handle_cfchcfcoobrx1crcerr, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_CFC_HCFCOOBRX1HEADERERR,     0,     0,                           arad_interrupt_handle_cfchcfcoobrx1headererr, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_CFC_ILKN0OOBRXCRCERR,     0,     0,                              arad_interrupt_handle_cfcilkn0oobrxcrcerr, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_CFC_ILKN0OOBRXIFSTATERR,     0,     0,                           arad_interrupt_handle_cfcilkn0oobrxifstaterr, NULL);
    interrupt_add_interrupt_handler(unit,                        ARAD_INT_CFC_ILKN0OOBRXLANESSTATERR,     0,     0,                        arad_interrupt_handle_cfcilkn0oobrxlanesstaterr, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_CFC_ILKN0OOBRXOVERFLOW,     0,     0,                            arad_interrupt_handle_cfcilkn0oobrxoverflow, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_CFC_ILKN1OOBRXCRCERR,     0,     0,                              arad_interrupt_handle_cfcilkn1oobrxcrcerr, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_CFC_ILKN1OOBRXIFSTATERR,     0,     0,                           arad_interrupt_handle_cfcilkn1oobrxifstaterr, NULL);
    interrupt_add_interrupt_handler(unit,                        ARAD_INT_CFC_ILKN1OOBRXLANESSTATERR,     0,     0,                        arad_interrupt_handle_cfcilkn1oobrxlanesstaterr, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_CFC_ILKN1OOBRXOVERFLOW,     0,     0,                            arad_interrupt_handle_cfcilkn1oobrxoverflow, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_CFC_PARITYERRINT,     0,     0,                                  arad_interrupt_handle_cfcparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_CFC_SPIOOBRX0DIP2ALARM,     0,     0,                            arad_interrupt_handle_cfcspioobrx0dip2alarm, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_CFC_SPIOOBRX0DIP2ERR,     0,     0,                              arad_interrupt_handle_cfcspioobrx0dip2err, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_CFC_SPIOOBRX0FRMERR,     0,     0,                               arad_interrupt_handle_cfcspioobrx0frmerr, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_CFC_SPIOOBRX0LOCKERR,     0,     0,                              arad_interrupt_handle_cfcspioobrx0lockerr, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_CFC_SPIOOBRX0OUTOFFRM,     0,     0,                             arad_interrupt_handle_cfcspioobrx0outoffrm, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_CFC_SPIOOBRX0WDERR,     0,     0,                                arad_interrupt_handle_cfcspioobrx0wderr, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_CFC_SPIOOBRX1DIP2ALARM,     0,     0,                            arad_interrupt_handle_cfcspioobrx1dip2alarm, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_CFC_SPIOOBRX1DIP2ERR,     0,     0,                              arad_interrupt_handle_cfcspioobrx1dip2err, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_CFC_SPIOOBRX1FRMERR,     0,     0,                               arad_interrupt_handle_cfcspioobrx1frmerr, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_CFC_SPIOOBRX1LOCKERR,     0,     0,                              arad_interrupt_handle_cfcspioobrx1lockerr, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_CFC_SPIOOBRX1OUTOFFRM,     0,     0,                             arad_interrupt_handle_cfcspioobrx1outoffrm, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_CFC_SPIOOBRX1WDERR,     0,     0,                                arad_interrupt_handle_cfcspioobrx1wderr, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_CRPS_CRPS0CNTOVF,     0,     0,                                  arad_interrupt_handle_crpscrps0cntovf, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_CRPS_CRPS0PREREADFIFOFULL,     0,     0,                         arad_interrupt_handle_crpscrps0prereadfifofull, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_CRPS_CRPS1CNTOVF,     0,     0,                                  arad_interrupt_handle_crpscrps1cntovf, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_CRPS_CRPS1PREREADFIFOFULL,     0,     0,                         arad_interrupt_handle_crpscrps1prereadfifofull, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_CRPS_CRPS2CNTOVF,     0,     0,                                  arad_interrupt_handle_crpscrps2cntovf, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_CRPS_CRPS2PREREADFIFOFULL,     0,     0,                         arad_interrupt_handle_crpscrps2prereadfifofull, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_CRPS_CRPS3CNTOVF,     0,     0,                                  arad_interrupt_handle_crpscrps3cntovf, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_CRPS_CRPS3PREREADFIFOFULL,     0,     0,                         arad_interrupt_handle_crpscrps3prereadfifofull, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_CRPS_PARITYERRINT,     0,     0,                                 arad_interrupt_handle_crpsparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_CRPS_EGQINVLDADACC,     0,     0,                                arad_interrupt_handle_crpsegqinvldadacc, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_CRPS_EPNIAINVLDADACC,     0,     0,                              arad_interrupt_handle_crpsepniainvldadacc, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_CRPS_EPNIBINVLDADACC,     0,     0,                              arad_interrupt_handle_crpsepnibinvldadacc, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_CRPS_IQMAINVLDADACC,     0,     0,                               arad_interrupt_handle_crpsiqmainvldadacc, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_CRPS_IQMBINVLDADACC,     0,     0,                               arad_interrupt_handle_crpsiqmbinvldadacc, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_CRPS_IQMCINVLDADACC,     0,     0,                               arad_interrupt_handle_crpsiqmcinvldadacc, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_CRPS_IQMDINVLDADACC,     0,     0,                               arad_interrupt_handle_crpsiqmdinvldadacc, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_CRPS_IRPPAINVLDADACC,     0,     0,                              arad_interrupt_handle_crpsirppainvldadacc, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_CRPS_IRPPBINVLDADACC,     0,     0,                              arad_interrupt_handle_crpsirppbinvldadacc, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_DRCA_OPPDATAERRINT,     0,     0,                                arad_interrupt_handle_drcaoppdataerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_DRCB_OPPDATAERRINT,     0,     0,                                arad_interrupt_handle_drcboppdataerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_DRCC_OPPDATAERRINT,     0,     0,                                arad_interrupt_handle_drccoppdataerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_DRCD_OPPDATAERRINT,     0,     0,                                arad_interrupt_handle_drcdoppdataerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_DRCE_OPPDATAERRINT,     0,     0,                                arad_interrupt_handle_drceoppdataerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_DRCF_OPPDATAERRINT,     0,     0,                                arad_interrupt_handle_drcfoppdataerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_DRCG_OPPDATAERRINT,     0,     0,                                arad_interrupt_handle_drcgoppdataerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_DRCH_OPPDATAERRINT,     0,     0,                                arad_interrupt_handle_drchoppdataerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_EGQ_ECC_1BERRINT,     0,     0,                                  arad_interrupt_handle_egqecc_1berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_EGQ_ECC_2BERRINT,     0,     0,                                  arad_interrupt_handle_egqecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_EGQ_PARITYERRINT,     0,     0,                                  arad_interrupt_handle_egqparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_EGQ_INVALIDOTMINT,     0,     0,                                 arad_interrupt_handle_egqinvalidotmint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_EGQ_DELETEFIFOFULL,     0,     0,                                arad_interrupt_handle_egqdeletefifofull, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_EGQ_DROPPEDUCDBINT,     0,     0,                                arad_interrupt_handle_egqdroppeducdbint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_EGQ_DROPPEDUCPDINT,     0,     0,                                arad_interrupt_handle_egqdroppeducpdint, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_EGQ_UCPKTPORTFF,     0,     0,                                   arad_interrupt_handle_egqucpktportff, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_EGQ_CSRMISSINGEOPERR,     0,     0,                              arad_interrupt_handle_egqcsrmissingeoperr, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_EGQ_CSRPKTSIZEERR,     0,     0,                                 arad_interrupt_handle_egqcsrpktsizeerr, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_EGQ_CSRSOPANDEOPERR,     0,     0,                               arad_interrupt_handle_egqcsrsopandeoperr, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_EGQ_CSRUNEXPECTEDEOPERR,     0,     0,                           arad_interrupt_handle_egqcsrunexpectedeoperr, NULL);
    interrupt_add_interrupt_handler(unit,                                      ARAD_INT_EGQ_RCMMCERR,  1000,     1,                                      arad_interrupt_handle_egqrcmmcerr, arad_interrupt_handle_recurring_action_egqrcmmcerr);
    interrupt_add_interrupt_handler(unit,                                      ARAD_INT_EGQ_RCMUCERR,  1000,     1,                                      arad_interrupt_handle_egqrcmucerr, arad_interrupt_handle_recurring_action_egqrcmucerr);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_EGQ_VSC256MCCELLSIZEERR,  1000,     1,                           arad_interrupt_handle_egqvsc256mccellsizeerr, arad_interrupt_handle_recurring_action_egqvsc256mccellsizeerr);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_EGQ_VSC256MCEOPERR,  1000,     1,                                arad_interrupt_handle_egqvsc256mceoperr, arad_interrupt_handle_recurring_action_egqvsc256mceoperr);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_EGQ_VSC256MCFRAGNUMERR,  1000,     1,                            arad_interrupt_handle_egqvsc256mcfragnumerr, arad_interrupt_handle_recurring_action_egqvsc256mcfragnumerr);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_EGQ_VSC256MCMISSINGSOPERR,  1000,     1,                         arad_interrupt_handle_egqvsc256mcmissingsoperr, arad_interrupt_handle_recurring_action_egqvsc256mcmissingsoperr);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_EGQ_VSC256MCPKTCRCERR,  1000,     1,                             arad_interrupt_handle_egqvsc256mcpktcrcerr, arad_interrupt_handle_recurring_action_egqvsc256mcpktcrcerr);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_EGQ_VSC256MCPKTSIZEERR,  1000,     1,                            arad_interrupt_handle_egqvsc256mcpktsizeerr, arad_interrupt_handle_recurring_action_egqvsc256mcpktsizeerr);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_EGQ_VSC256MCSOPINTRMOPERR,  1000,     1,                         arad_interrupt_handle_egqvsc256mcsopintrmoperr, arad_interrupt_handle_recurring_action_egqvsc256mcsopintrmoperr);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_EGQ_VSC256UCCELLSIZEERR,  1000,     1,                           arad_interrupt_handle_egqvsc256uccellsizeerr, arad_interrupt_handle_recurring_action_egqvsc256uccellsizeerr);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_EGQ_VSC256UCEOPERR,  1000,     1,                                arad_interrupt_handle_egqvsc256uceoperr, arad_interrupt_handle_recurring_action_egqvsc256uceoperr);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_EGQ_VSC256UCFRAGNUMERR,  1000,     1,                            arad_interrupt_handle_egqvsc256ucfragnumerr, arad_interrupt_handle_recurring_action_egqvsc256ucfragnumerr);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_EGQ_VSC256UCMISSINGSOPERR,  1000,     1,                         arad_interrupt_handle_egqvsc256ucmissingsoperr, arad_interrupt_handle_recurring_action_egqvsc256ucmissingsoperr);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_EGQ_VSC256UCPKTCRCERR,  1000,     1,                             arad_interrupt_handle_egqvsc256ucpktcrcerr, arad_interrupt_handle_recurring_action_egqvsc256ucpktcrcerr);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_EGQ_VSC256UCPKTSIZEERR,  1000,     1,                            arad_interrupt_handle_egqvsc256ucpktsizeerr, arad_interrupt_handle_recurring_action_egqvsc256ucpktsizeerr);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_EGQ_VSC256UCSOPINTRMOPERR,  1000,     1,                         arad_interrupt_handle_egqvsc256ucsopintrmoperr, arad_interrupt_handle_recurring_action_egqvsc256ucsopintrmoperr);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_EGQ_VSCEOPSIZEERR,  1000,     1,                                 arad_interrupt_handle_egqvsceopsizeerr, arad_interrupt_handle_recurring_action_egqvsceopsizeerr);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_EGQ_VSCFIX129ERR,  1000,     1,                                  arad_interrupt_handle_egqvscfix129err, arad_interrupt_handle_recurring_action_egqvscfix129err);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_EGQ_VSCFRAGNUMERR,  1000,     1,                                 arad_interrupt_handle_egqvscfragnumerr, arad_interrupt_handle_recurring_action_egqvscfragnumerr);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_EGQ_VSCMISSINGSOPERR,  1000,     1,                              arad_interrupt_handle_egqvscmissingsoperr, arad_interrupt_handle_recurring_action_egqvscmissingsoperr);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_EGQ_VSCPKTCRCERR,  1000,     1,                                  arad_interrupt_handle_egqvscpktcrcerr, arad_interrupt_handle_recurring_action_egqvscpktcrcerr);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_EGQ_VSCPKTSIZEERR,  1000,     1,                                 arad_interrupt_handle_egqvscpktsizeerr, arad_interrupt_handle_recurring_action_egqvscpktsizeerr);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_EGQ_VSCSOPINTRMOPERR,  1000,     1,                              arad_interrupt_handle_egqvscsopintrmoperr, arad_interrupt_handle_recurring_action_egqvscsopintrmoperr);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_EPNI_ECC_1BERRINT,     0,     0,                                 arad_interrupt_handle_epniecc_1berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_EPNI_ECC_2BERRINT,     0,     0,                                 arad_interrupt_handle_epniecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_EPNI_PARITYERRINT,     0,     0,                                 arad_interrupt_handle_epniparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_EPNI_MIRROVFINT,     0,     0,                                   arad_interrupt_handle_epnimirrovfint, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_EPNI_EEITYPEERR,     0,     0,                                   arad_interrupt_handle_epnieeitypeerr, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_EPNI_EESLASTACTIONNOTAC,     0,     0,                           arad_interrupt_handle_epnieeslastactionnotac, NULL);
    interrupt_add_interrupt_handler(unit,                    ARAD_INT_EPNI_FIRSTENCMPLSHEADERCODEERR,     0,     0,                    arad_interrupt_handle_epnifirstencmplsheadercodeerr, NULL);
    interrupt_add_interrupt_handler(unit,                  ARAD_INT_EPNI_FIRSTENCMPLSHEADERCODESNOOP,     0,     0,                  arad_interrupt_handle_epnifirstencmplsheadercodesnoop, NULL);
    interrupt_add_interrupt_handler(unit,                                       ARAD_INT_EPNI_PHPERR,     0,     0,                                       arad_interrupt_handle_epniphperr, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_EPNI_PHPNEXTIPPROTOCOLERR,     0,     0,                         arad_interrupt_handle_epniphpnextipprotocolerr, NULL);
    interrupt_add_interrupt_handler(unit,               ARAD_INT_EPNI_SECONDENCIPTUNNELHEADERCODEERR,     0,     0,               arad_interrupt_handle_epnisecondenciptunnelheadercodeerr, NULL);
    interrupt_add_interrupt_handler(unit,             ARAD_INT_EPNI_SECONDENCIPTUNNELHEADERCODESNOOP,     0,     0,             arad_interrupt_handle_epnisecondenciptunnelheadercodesnoop, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_EPNI_SECONDENCMPLSHEADERCODEERR,     0,     0,                   arad_interrupt_handle_epnisecondencmplsheadercodeerr, NULL);
    interrupt_add_interrupt_handler(unit,                 ARAD_INT_EPNI_SECONDENCMPLSHEADERCODESNOOP,     0,     0,                 arad_interrupt_handle_epnisecondencmplsheadercodesnoop, NULL);
    interrupt_add_interrupt_handler(unit,                  ARAD_INT_EPNI_SECONDENCTRILLHEADERCODEERR,     0,     0,                  arad_interrupt_handle_epnisecondenctrillheadercodeerr, NULL);
    interrupt_add_interrupt_handler(unit,                ARAD_INT_EPNI_SECONDENCTRILLHEADERCODESNOOP,     0,     0,                arad_interrupt_handle_epnisecondenctrillheadercodesnoop, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_FCR_CPUCNTCELLFNE,     0,     0,                                 arad_interrupt_handle_fcrcpucntcellfne, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_FCR_REACHFIFOOVF,     0,     0,                                  arad_interrupt_handle_fcrreachfifoovf, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_FCR_ECC_1BERRINT,     0,     0,                                 arad_interrupt_handle_fcrecc_1berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_FCR_ECC_2BERRINT,     0,     0,                                 arad_interrupt_handle_fcrecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_FCT_UNRCHDESTEVENT,     0,     0,                                 arad_interrupt_handle_fctunrchdestevent, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FDR_PRMFDRADESCCNTOB,     0,     0,                              arad_interrupt_handle_fdrprmfdradesccntob, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FDR_PRMFDRBUNEXPCELL,     0,     0,                              arad_interrupt_handle_fdrprmfdrbunexpcell, NULL);
    interrupt_add_interrupt_handler(unit,                                      ARAD_INT_FDR_PRMALTOA,     10,    10,                                      arad_interrupt_handle_fdrprmaltoa, arad_interrupt_handle_recurring_action_fdrprmaltoa);
    interrupt_add_interrupt_handler(unit,                                      ARAD_INT_FDR_PRMALTOB,     10,    10,                                      arad_interrupt_handle_fdrprmaltob, arad_interrupt_handle_recurring_action_fdrprmaltob);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_FDR_PRMCPUDATACELLFNEA0,     0,     0,                           arad_interrupt_handle_fdrprmcpudatacellfnea0, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_FDR_PRMCPUDATACELLFNEA1,     0,     0,                           arad_interrupt_handle_fdrprmcpudatacellfnea1, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_FDR_PRMCPUDATACELLFNEB0,     0,     0,                           arad_interrupt_handle_fdrprmcpudatacellfneb0, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_FDR_PRMCPUDATACELLFNEB1,     0,     0,                           arad_interrupt_handle_fdrprmcpudatacellfneb1, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_FDR_PRMFDROUTPUTOUTOFSYNC,     0,     0,                         arad_interrupt_handle_fdrprmfdroutputoutofsync, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FDR_PRMFDRAOUTOFSYNC,     0,     0,                              arad_interrupt_handle_fdrprmfdraoutofsync, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FDR_PRMFDRBOUTOFSYNC,     0,     0,                              arad_interrupt_handle_fdrprmfdrboutofsync, NULL);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FDR_PRMIFMAFOA,    10,     1,                                    arad_interrupt_handle_fdrprmifmafoa, arad_interrupt_handle_recurring_action_fdrprmifmafoa);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FDR_PRMIFMAFOB,    10,     1,                                    arad_interrupt_handle_fdrprmifmafob, arad_interrupt_handle_recurring_action_fdrprmifmafob);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FDR_PRMIFMBFOA,    10,     1,                                    arad_interrupt_handle_fdrprmifmbfoa, arad_interrupt_handle_recurring_action_fdrprmifmbfoa);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FDR_PRMIFMBFOB,    10,     1,                                    arad_interrupt_handle_fdrprmifmbfob, arad_interrupt_handle_recurring_action_fdrprmifmbfob);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FDR_PRMFDRAUNEXPCELL,     0,     0,                              arad_interrupt_handle_fdrprmfdraunexpcell, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FDR_PRMFDRADESCCNTOA,     0,     0,                              arad_interrupt_handle_fdrprmfdradesccntoa, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FDR_SECFDRAUNEXPCELL,     0,     0,                              arad_interrupt_handle_fdrsecfdraunexpcell, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FDR_SECFDRADESCCNTOA,     0,     0,                              arad_interrupt_handle_fdrsecfdradesccntoa, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FDR_SECFDRADESCCNTOB,     0,     0,                              arad_interrupt_handle_fdrsecfdradesccntob, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FDR_SECFDRBUNEXPCELL,     0,     0,                              arad_interrupt_handle_fdrsecfdrbunexpcell, NULL);
    interrupt_add_interrupt_handler(unit,                                      ARAD_INT_FDR_SECALTOA,     10,   10,                            arad_interrupt_handle_fdrsecaltoa, arad_interrupt_handle_recurring_action_fdrprmaltoa);
    interrupt_add_interrupt_handler(unit,                                      ARAD_INT_FDR_SECALTOB,     10,   10,                            arad_interrupt_handle_fdrsecaltob, arad_interrupt_handle_recurring_action_fdrprmaltob);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_FDR_SECCPUDATACELLFNEA0,     0,     0,                           arad_interrupt_handle_fdrseccpudatacellfnea0, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_FDR_SECCPUDATACELLFNEA1,     0,     0,                           arad_interrupt_handle_fdrseccpudatacellfnea1, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_FDR_SECCPUDATACELLFNEB0,     0,     0,                           arad_interrupt_handle_fdrseccpudatacellfneb0, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_FDR_SECCPUDATACELLFNEB1,     0,     0,                           arad_interrupt_handle_fdrseccpudatacellfneb1, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_FDR_SECFDROUTPUTOUTOFSYNC,     0,     0,                         arad_interrupt_handle_fdrsecfdroutputoutofsync, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FDR_SECFDRAOUTOFSYNC,     0,     0,                              arad_interrupt_handle_fdrsecfdraoutofsync, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FDR_SECFDRBOUTOFSYNC,     0,     0,                              arad_interrupt_handle_fdrsecfdrboutofsync, NULL);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FDR_SECIFMAFOA,    10,     1,                                    arad_interrupt_handle_fdrsecifmafoa, arad_interrupt_handle_recurring_action_fdrsecifmafoa);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FDR_SECIFMAFOB,    10,     1,                                    arad_interrupt_handle_fdrsecifmafob, arad_interrupt_handle_recurring_action_fdrsecifmafob);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FDR_SECIFMBFOA,    10,     1,                                    arad_interrupt_handle_fdrsecifmbfoa, arad_interrupt_handle_recurring_action_fdrsecifmbfoa);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FDR_SECIFMBFOB,    10,     1,                                    arad_interrupt_handle_fdrsecifmbfob, arad_interrupt_handle_recurring_action_fdrsecifmbfob);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_FDR_FDRUNEXPERROR,     0,     0,                                 arad_interrupt_handle_fdrfdrunexperror, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_FDR_INBANDFIFOFULL,     0,     0,                                arad_interrupt_handle_fdrinbandfifofull, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_FDR_PRMECC_2BERRINT,     0,     0,                               arad_interrupt_handle_fdrprmecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_FDR_SECECC_2BERRINT,     0,     0,                               arad_interrupt_handle_fdrsececc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_FDT_ECC_2BERRINT,     0,     0,                                  arad_interrupt_handle_fdtecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_FDT_ECC_1BERRINT,     0,     0,                                  arad_interrupt_handle_fdtecc_1berrint, NULL);
    interrupt_add_interrupt_handler(unit,                        ARAD_INT_FDT_ILEGALIREPACKETSIZEINT,     0,     0,                        arad_interrupt_handle_fdtilegalirepacketsizeint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_FDT_PARITYERRINT,     0,     0,                                  arad_interrupt_handle_fdtparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FMAC_RXCRCERRN_INT_0,     100,   10,                              arad_interrupt_handle_fmacrxcrcerrn_int_0, arad_interrupt_handle_recurring_action_fmacrxcrcerrn_int_0);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FMAC_RXCRCERRN_INT_1,     100,   10,                              arad_interrupt_handle_fmacrxcrcerrn_int_1, arad_interrupt_handle_recurring_action_fmacrxcrcerrn_int_1);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FMAC_RXCRCERRN_INT_2,     100,   10,                              arad_interrupt_handle_fmacrxcrcerrn_int_2, arad_interrupt_handle_recurring_action_fmacrxcrcerrn_int_2);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FMAC_RXCRCERRN_INT_3,     100,   10,                              arad_interrupt_handle_fmacrxcrcerrn_int_3, arad_interrupt_handle_recurring_action_fmacrxcrcerrn_int_3);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FMAC_WRONGSIZE_INT_0,    100,    10,                              arad_interrupt_handle_fmacwrongsize_int_0, arad_interrupt_handle_recurring_action_fmacwrongsize_int_0);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FMAC_WRONGSIZE_INT_1,    100,    10,                              arad_interrupt_handle_fmacwrongsize_int_1, arad_interrupt_handle_recurring_action_fmacwrongsize_int_1);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FMAC_WRONGSIZE_INT_2,    100,    10,                              arad_interrupt_handle_fmacwrongsize_int_2, arad_interrupt_handle_recurring_action_fmacwrongsize_int_2);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_FMAC_WRONGSIZE_INT_3,    100,    10,                              arad_interrupt_handle_fmacwrongsize_int_3, arad_interrupt_handle_recurring_action_fmacwrongsize_int_3);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_FSRD_FSRD_0_SYNCSTATUSCHANGED_0,     0,     0,                   arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_0, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_FSRD_FSRD_1_SYNCSTATUSCHANGED_0,     0,     0,                   arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_0, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_FSRD_FSRD_2_SYNCSTATUSCHANGED_0,     0,     0,                   arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_0, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_FSRD_FSRD_0_SYNCSTATUSCHANGED_1,     0,     0,                   arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_1, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_FSRD_FSRD_1_SYNCSTATUSCHANGED_1,     0,     0,                   arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_1, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_FSRD_FSRD_2_SYNCSTATUSCHANGED_1,     0,     0,                   arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_1, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_FSRD_FSRD_0_SYNCSTATUSCHANGED_2,     0,     0,                   arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_2, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_FSRD_FSRD_1_SYNCSTATUSCHANGED_2,     0,     0,                   arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_2, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_FSRD_FSRD_2_SYNCSTATUSCHANGED_2,     0,     0,                   arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_2, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_FSRD_FSRD_0_SYNCSTATUSCHANGED_3,     0,     0,                   arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_3, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_FSRD_FSRD_1_SYNCSTATUSCHANGED_3,     0,     0,                   arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_3, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_FSRD_FSRD_2_SYNCSTATUSCHANGED_3,     0,     0,                   arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_3, NULL);
    interrupt_add_interrupt_handler(unit,                      ARAD_INT_FSRD_FSRD_0_TXPLLLOCKCHANGED,     0,     0,                       arad_interrupt_handle_fsrd_fsrd_txplllockchanged, NULL);
    interrupt_add_interrupt_handler(unit,                      ARAD_INT_FSRD_FSRD_1_TXPLLLOCKCHANGED,     0,     0,                       arad_interrupt_handle_fsrd_fsrd_txplllockchanged, NULL);
    interrupt_add_interrupt_handler(unit,                      ARAD_INT_FSRD_FSRD_2_TXPLLLOCKCHANGED,     0,     0,                       arad_interrupt_handle_fsrd_fsrd_txplllockchanged, NULL);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FMAC_LOS_INT_0,     0,     0,                                    arad_interrupt_handle_fmaclos_int_0, NULL);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FMAC_LOS_INT_1,     0,     0,                                    arad_interrupt_handle_fmaclos_int_1, NULL);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FMAC_LOS_INT_2,     0,     0,                                    arad_interrupt_handle_fmaclos_int_2, NULL);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FMAC_LOS_INT_3,     0,     0,                                    arad_interrupt_handle_fmaclos_int_3, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_FMAC_RXLOSTOFSYNC_0,     0,     0,                               arad_interrupt_handle_fmacrxlostofsync_0, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_FMAC_RXLOSTOFSYNC_1,     0,     0,                               arad_interrupt_handle_fmacrxlostofsync_1, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_FMAC_RXLOSTOFSYNC_2,     0,     0,                               arad_interrupt_handle_fmacrxlostofsync_2, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_FMAC_RXLOSTOFSYNC_3,     0,     0,                               arad_interrupt_handle_fmacrxlostofsync_3, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_FMAC_DECERR_INT_0,    10,     10,                                 arad_interrupt_handle_fmacdecerr_int_0, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_FMAC_DECERR_INT_1,    10,    10,                                 arad_interrupt_handle_fmacdecerr_int_1, arad_interrupt_handle_recurring_fmacdecerr_int_1);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_FMAC_DECERR_INT_2,    10,    10,                                 arad_interrupt_handle_fmacdecerr_int_2, arad_interrupt_handle_recurring_fmacdecerr_int_2);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_FMAC_DECERR_INT_3,    10,    10,                                 arad_interrupt_handle_fmacdecerr_int_3, arad_interrupt_handle_recurring_fmacdecerr_int_3);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FMAC_OOF_INT_0,    10,    10,                                    arad_interrupt_handle_fmacoof_int_0, arad_interrupt_handle_recurring_action_fmacoof_int_0);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FMAC_OOF_INT_1,    10,    10,                                    arad_interrupt_handle_fmacoof_int_1, arad_interrupt_handle_recurring_action_fmacoof_int_1);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FMAC_OOF_INT_2,    10,    10,                                    arad_interrupt_handle_fmacoof_int_2, arad_interrupt_handle_recurring_action_fmacoof_int_2);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_FMAC_OOF_INT_3,    10,    10,                                    arad_interrupt_handle_fmacoof_int_3, arad_interrupt_handle_recurring_action_fmacoof_int_3);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_FMAC_RXCTRLOVERFLOW_INT_0,     0,     0,                         arad_interrupt_handle_fmacrxctrloverflow_int_0, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_FMAC_RXCTRLOVERFLOW_INT_1,     0,     0,                         arad_interrupt_handle_fmacrxctrloverflow_int_1, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_FMAC_RXCTRLOVERFLOW_INT_2,     0,     0,                         arad_interrupt_handle_fmacrxctrloverflow_int_2, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_FMAC_RXCTRLOVERFLOW_INT_3,     0,     0,                         arad_interrupt_handle_fmacrxctrloverflow_int_3, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_FMAC_TRANSMITERR_INT_0,     0,     0,                            arad_interrupt_handle_fmactransmiterr_int_0, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_FMAC_TRANSMITERR_INT_1,     0,     0,                            arad_interrupt_handle_fmactransmiterr_int_1, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_FMAC_TRANSMITERR_INT_2,     0,     0,                            arad_interrupt_handle_fmactransmiterr_int_2, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_FMAC_TRANSMITERR_INT_3,     0,     0,                            arad_interrupt_handle_fmactransmiterr_int_3, NULL);
    interrupt_add_interrupt_handler(unit,                          ARAD_INT_FMAC_TXFDRCIFFAULT_INT_0,     0,     0,                          arad_interrupt_handle_fmactxfdrciffault_int_0, NULL);
    interrupt_add_interrupt_handler(unit,                          ARAD_INT_FMAC_TXFDRCIFFAULT_INT_1,     0,     0,                          arad_interrupt_handle_fmactxfdrciffault_int_1, NULL);
    interrupt_add_interrupt_handler(unit,                          ARAD_INT_FMAC_TXFDRCIFFAULT_INT_2,     0,     0,                          arad_interrupt_handle_fmactxfdrciffault_int_2, NULL);
    interrupt_add_interrupt_handler(unit,                          ARAD_INT_FMAC_TXFDRCIFFAULT_INT_3,     0,     0,                          arad_interrupt_handle_fmactxfdrciffault_int_3, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_FMAC_ECC_1BERRINT,  1000,   100,                                 arad_interrupt_handle_fmacecc_1berrint, arad_interrupt_handle_recurring_action_fmacecc_1berrint);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_FMAC_ECC_2BERRINT,     0,     0,                                 arad_interrupt_handle_fmacecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IDR_ECC_2BERRINT,     0,     0,                                  arad_interrupt_handle_idrecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IDR_ECC_1BERRINT,     0,     0,                                  arad_interrupt_handle_idrecc_1berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IDR_PARITYERRINT,     0,     0,                                  arad_interrupt_handle_idrparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                     ARAD_INT_IDR_ERRORFULLMULTICASTRECYCLE,     0,     0,                     arad_interrupt_handle_idrerrorfullmulticastrecycle, NULL);
    interrupt_add_interrupt_handler(unit,                     ARAD_INT_IDR_ERRORMINIMULTICASTRECYCLE,     0,     0,                     arad_interrupt_handle_idrerrorminimulticastrecycle, NULL);
    interrupt_add_interrupt_handler(unit,                      ARAD_INT_IDR_ERROROCBMULTICASTRECYCLE,     0,     0,                      arad_interrupt_handle_idrerrorocbmulticastrecycle, NULL);
    interrupt_add_interrupt_handler(unit,                        ARAD_INT_IDR_ERROROCBUNICASTRECYCLE,     0,     0,                        arad_interrupt_handle_idrerrorocbunicastrecycle, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_IDR_ERRORUNICASTRECYCLE,     0,     0,                           arad_interrupt_handle_idrerrorunicastrecycle, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_IDR_MMUECC_2BERRINT,     0,     0,                               arad_interrupt_handle_idrmmuecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_IHB_BADCHANNELNUM,     0,     0,                                 arad_interrupt_handle_ihbbadchannelnum, NULL);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_IHB_BADLKPTYPE,     0,     0,                                    arad_interrupt_handle_ihbbadlkptype, NULL);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_IHB_BADLRNTYPE,     0,     0,                                    arad_interrupt_handle_ihbbadlrntype, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IHB_ECC_1BERRINT,  1000,   100,                                  arad_interrupt_handle_ihbecc_1berrint, arad_interrupt_handle_recurring_action_ihbecc_1berrint);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IHB_ECC_2BERRINT,     0,     0,                                  arad_interrupt_handle_ihbecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_IHB_FLPFIFOFULL,     0,     0,                                   arad_interrupt_handle_ihbflpfifofull, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_IHB_FLPLOOKUPTIMEOUT,     0,     0,                              arad_interrupt_handle_ihbflplookuptimeout, NULL);
    interrupt_add_interrupt_handler(unit,                       ARAD_INT_IHB_LBECMPLAGUSESAMEHASHINT,     0,     0,                       arad_interrupt_handle_ihblbecmplagusesamehashint, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_IHB_LBVECTORINDEXILLEGALCALCINT,     0,     0,                   arad_interrupt_handle_ihblbvectorindexillegalcalcint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_IHB_NIFPHYSICALERR,     0,     0,                                arad_interrupt_handle_ihbnifphysicalerr, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IHB_NIFRXFIFOOVF,     0,     0,                                  arad_interrupt_handle_ihbnifrxfifoovf, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_IHB_NIFTXFIFOFULL,     0,     0,                                 arad_interrupt_handle_ihbniftxfifofull, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_IHB_OAMBFDMISCONFIGINT,     0,     0,                            arad_interrupt_handle_ihboambfdmisconfigint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IHB_PARITYERRINT,     0,     0,                                  arad_interrupt_handle_ihbparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_IHB_RXBROKENRECORD,     0,     0,                                arad_interrupt_handle_ihbrxbrokenrecord, NULL);
    interrupt_add_interrupt_handler(unit,               ARAD_INT_IHB_SEQRXBIGERSEQEXPANDSMALLERSEQTX,     0,     0,               arad_interrupt_handle_ihbseqrxbigerseqexpandsmallerseqtx, NULL);
    interrupt_add_interrupt_handler(unit,             ARAD_INT_IHB_SEQRXSMALLERSEQEXPORBIGGEREQSEQTX,     0,     0,             arad_interrupt_handle_ihbseqrxsmallerseqexporbiggereqseqtx, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_IHB_TCAMPROTECTIONERROR,     0,     0,                           arad_interrupt_handle_ihbtcamprotectionerror, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_IHB_TCAMQUERYFAILUREVALID,     0,     0,                         arad_interrupt_handle_ihbtcamqueryfailurevalid, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_IHB_OEMAERRORCAMTABLEFULL,     0,     0,                         arad_interrupt_handle_ihboemaerrorcamtablefull, NULL);
    interrupt_add_interrupt_handler(unit,                       ARAD_INT_IHB_OEMAERRORTABLECOHERENCY,     0,     0,                       arad_interrupt_handle_ihboemaerrortablecoherency, NULL);
    interrupt_add_interrupt_handler(unit,                ARAD_INT_IHB_OEMAMANAGEMENTUNITFAILUREVALID,     0,     0,                arad_interrupt_handle_ihboemamanagementunitfailurevalid, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_IHB_OEMBERRORCAMTABLEFULL,     0,     0,                         arad_interrupt_handle_ihboemberrorcamtablefull, NULL);
    interrupt_add_interrupt_handler(unit,                       ARAD_INT_IHB_OEMBERRORTABLECOHERENCY,     0,     0,                       arad_interrupt_handle_ihboemberrortablecoherency, NULL);
    interrupt_add_interrupt_handler(unit,                ARAD_INT_IHB_OEMBMANAGEMENTUNITFAILUREVALID,     0,     0,                arad_interrupt_handle_ihboembmanagementunitfailurevalid, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IHP_ECC_1BERRINT,  1000,   100,                                  arad_interrupt_handle_ihpecc_1berrint, arad_interrupt_handle_recurring_action_ihpecc_1berrint);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IHP_ECC_2BERRINT,     0,     0,                                  arad_interrupt_handle_ihpecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IHP_PARITYERRINT,     0,     0,                                  arad_interrupt_handle_ihpparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                          ARAD_INT_IHP_ISAERRORCAMTABLEFULL,     0,     0,                          arad_interrupt_handle_ihpisaerrorcamtablefull, NULL);
    interrupt_add_interrupt_handler(unit,                        ARAD_INT_IHP_ISAERRORTABLECOHERENCY,     0,     0,                        arad_interrupt_handle_ihpisaerrortablecoherency, NULL);
    interrupt_add_interrupt_handler(unit,                 ARAD_INT_IHP_ISAMANAGEMENTUNITFAILUREVALID,     0,     0,                 arad_interrupt_handle_ihpisamanagementunitfailurevalid, NULL);
    interrupt_add_interrupt_handler(unit,                          ARAD_INT_IHP_ISBERRORCAMTABLEFULL,     0,     0,                          arad_interrupt_handle_ihpisberrorcamtablefull, NULL);
    interrupt_add_interrupt_handler(unit,                        ARAD_INT_IHP_ISBERRORTABLECOHERENCY,     0,     0,                        arad_interrupt_handle_ihpisberrortablecoherency, NULL);
    interrupt_add_interrupt_handler(unit,                 ARAD_INT_IHP_ISBMANAGEMENTUNITFAILUREVALID,     0,     0,                 arad_interrupt_handle_ihpisbmanagementunitfailurevalid, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_IHP_MACTEVENTREADY,     0,     0,                                arad_interrupt_handle_ihpmacteventready, NULL);
    interrupt_add_interrupt_handler(unit,                   ARAD_INT_IHP_MACTLOOKUPREQUESTCONTENTION,     0,     0,                   arad_interrupt_handle_ihpmactlookuprequestcontention, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_IHP_MACTREPLYREADY,     0,     0,                                arad_interrupt_handle_ihpmactreplyready, NULL);
    interrupt_add_interrupt_handler(unit,                 ARAD_INT_IHP_MACTSRCORLLLOOKUPONWRONGCYCLE,     0,     0,                 arad_interrupt_handle_ihpmactsrcorlllookuponwrongcycle, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_IHP_MACTERRORCAMTABLEFULL,     0,     0,                         arad_interrupt_handle_ihpmacterrorcamtablefull, NULL);
    interrupt_add_interrupt_handler(unit,                       ARAD_INT_IHP_MACTERRORTABLECOHERENCY,     0,     0,                       arad_interrupt_handle_ihpmacterrortablecoherency, NULL);
    interrupt_add_interrupt_handler(unit,                ARAD_INT_IHP_MACTMANAGEMENTUNITFAILUREVALID,     0,     0,                arad_interrupt_handle_ihpmactmanagementunitfailurevalid, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_IPS_DEQCOMMANDTIMEOUT,     0,     0,                             arad_interrupt_handle_ipsdeqcommandtimeout, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IPS_DQCQDEPTHOVF,     0,     0,                                  arad_interrupt_handle_ipsdqcqdepthovf, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IPS_DQCQOVERFLOW,     0,     0,                                  arad_interrupt_handle_ipsdqcqoverflow, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IPS_ECC_1BERRINT,  1000,   100,                                  arad_interrupt_handle_ipsecc_1berrint, arad_interrupt_handle_recurring_action_ipsecc_1berrint);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IPS_ECC_2BERRINT,     0,     0,                                  arad_interrupt_handle_ipsecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_IPS_EMPTYDQCQWRITE,     0,     0,                                arad_interrupt_handle_ipsemptydqcqwrite, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_IPS_ENQBLOCKOVERFLOW,  1000,     1,                              arad_interrupt_handle_ipsenqblockoverflow, arad_interrupt_handle_recurring_action_ipsenqblockoverflow);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IPS_PARITYERRINT,     0,     0,                                  arad_interrupt_handle_ipsparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IPT_ECC_1BERRINT,  1000,   100,                                  arad_interrupt_handle_iptecc_1berrint, arad_interrupt_handle_recurring_action_iptecc_1berrint);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IPT_ECC_2BERRINT,     0,     0,                                  arad_interrupt_handle_iptecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IPT_PARITYERRINT,     0,     0,                                  arad_interrupt_handle_iptparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                     ARAD_INT_IPT_CRCDELETEDBUFFERSFIFOFULL,     0,     0,                     arad_interrupt_handle_iptcrcdeletedbuffersfifofull, NULL);
    interrupt_add_interrupt_handler(unit,                 ARAD_INT_IPT_CRCDELETEDBUFFERSFIFONOTEMPTY,     0,     0,                 arad_interrupt_handle_iptcrcdeletedbuffersfifonotempty, NULL);
    interrupt_add_interrupt_handler(unit,                                     ARAD_INT_IPT_CRCERRPKT,   100,     1,                                     arad_interrupt_handle_iptcrcerrpkt, arad_interrupt_handle_recurring_action_iptcrcerrpkt);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IQM_ECC_1BERRINT,  1000,   100,                                  arad_interrupt_handle_iqmecc_1berrint, arad_interrupt_handle_recurring_action_iqmecc_1berrint);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IQM_ECC_2BERRINT,     0,     0,                                  arad_interrupt_handle_iqmecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IQM_PARITYERRINT,     0,     0,                                  arad_interrupt_handle_iqmparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_IQM_CNMPKTRJCT,     0,     0,                                    arad_interrupt_handle_iqmcnmpktrjct, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_IQM_DOUBLECDERR,     0,     0,                                   arad_interrupt_handle_iqmdoublecderr, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_IQM_DRAMDYNSIZEROLLOVER,     0,     0,                           arad_interrupt_handle_iqmdramdynsizerollover, NULL);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_IQM_ENQQNUMOVF,     0,     0,                                    arad_interrupt_handle_iqmenqqnumovf, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_IQM_FRFLMCDBROLLOVER,     0,     0,                              arad_interrupt_handle_iqmfrflmcdbrollover, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_IQM_FRMNMCDBROLLOVER,     0,     0,                              arad_interrupt_handle_iqmfrmnmcdbrollover, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_IQM_FRUCDBROLLOVER,     0,     0,                                arad_interrupt_handle_iqmfrucdbrollover, NULL);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_IQM_FREEBDBOVF,     0,     0,                                    arad_interrupt_handle_iqmfreebdbovf, NULL);
    interrupt_add_interrupt_handler(unit,                                    ARAD_INT_IQM_FREEBDBUNF,     0,     0,                                    arad_interrupt_handle_iqmfreebdbunf, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IQM_FULLUSCNTOVF,     0,     0,                                  arad_interrupt_handle_iqmfulluscntovf, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IQM_INGMC2ISPERR,     0,     0,                                  arad_interrupt_handle_iqmingmc2isperr, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IQM_MINIUSCNTOVF,     0,     0,                                  arad_interrupt_handle_iqmminiuscntovf, NULL);
    interrupt_add_interrupt_handler(unit,                                     ARAD_INT_IQM_QROLLOVER,     0,     0,                                     arad_interrupt_handle_iqmqrollover, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_IQM_VSQROLLOVER,     0,     0,                                   arad_interrupt_handle_iqmvsqrollover, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IRE_ECC_2BERRINT,     0,     0,                                  arad_interrupt_handle_ireecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IRE_ECC_1BERRINT,     0,     0,                                  arad_interrupt_handle_ireecc_1berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IRE_PARITYERRINT,     0,     0,                                  arad_interrupt_handle_ireparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                     ARAD_INT_IRE_ERRORBADREASSEMBLYCONTEXT,     0,     0,                     arad_interrupt_handle_ireerrorbadreassemblycontext, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_IRE_REGIERRDATAARRIVED,     0,     0,                            arad_interrupt_handle_ireregierrdataarrived, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_IRE_REGIERRPACKETSIZE,     0,     0,                             arad_interrupt_handle_ireregierrpacketsize, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_IRE_REGIERR_64BYTESPACK,     0,     0,                           arad_interrupt_handle_ireregierr_64bytespack, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IRR_ECC_2BERRINT,     0,     0,                                  arad_interrupt_handle_irrecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IRR_ECC_1BERRINT,     0,     0,                                  arad_interrupt_handle_irrecc_1berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_IRR_PARITYERRINT,     0,     0,                                  arad_interrupt_handle_irrparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_IRR_ERRISMAXREPLICATION,     0,     0,                           arad_interrupt_handle_irrerrismaxreplication, NULL);
    interrupt_add_interrupt_handler(unit,                         ARAD_INT_IRR_ERRISREPLICATIONEMPTY,     0,     0,                         arad_interrupt_handle_irrerrisreplicationempty, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_IRR_ERRMAXREPLICATION,     0,     0,                             arad_interrupt_handle_irrerrmaxreplication, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_IRR_ERRREPLICATIONEMPTY,     0,     0,                           arad_interrupt_handle_irrerrreplicationempty, NULL);
    interrupt_add_interrupt_handler(unit,                     ARAD_INT_IRR_ERRORRESEQUENCER0OUTOFSEQ,     0,     0,                     arad_interrupt_handle_irrerrorresequencer0outofseq, NULL);
    interrupt_add_interrupt_handler(unit,                    ARAD_INT_IRR_ERRORRESEQUENCER0OUTOFSYNC,     0,     0,                    arad_interrupt_handle_irrerrorresequencer0outofsync, NULL);
    interrupt_add_interrupt_handler(unit,                     ARAD_INT_IRR_ERRORRESEQUENCER1OUTOFSEQ,     0,     0,                     arad_interrupt_handle_irrerrorresequencer1outofseq, NULL);
    interrupt_add_interrupt_handler(unit,                    ARAD_INT_IRR_ERRORRESEQUENCER1OUTOFSYNC,     0,     0,                    arad_interrupt_handle_irrerrorresequencer1outofsync, NULL);
    interrupt_add_interrupt_handler(unit,                     ARAD_INT_IRR_ERRORRESEQUENCER2OUTOFSEQ,     0,     0,                     arad_interrupt_handle_irrerrorresequencer2outofseq, NULL);
    interrupt_add_interrupt_handler(unit,                    ARAD_INT_IRR_ERRORRESEQUENCER2OUTOFSYNC,     0,     0,                    arad_interrupt_handle_irrerrorresequencer2outofsync, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_MMU_ECC_1BERRINT,    10,     1,                                  arad_interrupt_handle_mmuecc_1berrint, arad_interrupt_handle_recurring_action_mmuecc_1berrint);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_MMU_ECC_2BERRINT,    10,     1,                                  arad_interrupt_handle_mmuecc_2berrint, arad_interrupt_handle_recurring_action_mmuecc_2berrint);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_MMU_DRAMOPPCRCERRINT,    10,     1,                              arad_interrupt_handle_mmudramoppcrcerrint, arad_interrupt_handle_recurring_action_mmudramoppcrcerrint);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_MMU_OCBOPPCRCERRINT,    10,     1,                               arad_interrupt_handle_mmuocboppcrcerrint, arad_interrupt_handle_recurring_action_mmuocboppcrcerrint);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_NBI_ECC_1BERRINT,  1000,   100,                                  arad_interrupt_handle_nbiecc_1berrint, arad_interrupt_handle_recurring_action_nbiecc_1berrint);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_0,     0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_1,     0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_2,     0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_3,     0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_4,     0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_5,     0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_6,     0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_7,     0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_8,     0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_9,     0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_10,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_11,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_12,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_13,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_14,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_15,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_16,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_17,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_18,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_19,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_20,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_21,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_22,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_23,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_24,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_25,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_26,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);
    interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_27,    0,     0,                             arad_interrupt_handle_nbi_link_status_change, NULL);   
	interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_ILKNRXPORT0STATUSCHANGEINT,              0,     0,                             arad_interrupt_handle_ilkn_rx_status_change,  NULL);    
	interrupt_add_interrupt_handler(unit,           ARAD_INT_NBI_ILKNRXPORT1STATUSCHANGEINT,              0,     0,                             arad_interrupt_handle_ilkn_rx_status_change,  NULL);
	interrupt_add_interrupt_handler(unit,                                  ARAD_INT_NBI_ECC_2BERRINT,     0,     0,                                  arad_interrupt_handle_nbiecc_2berrint, NULL);
	interrupt_add_interrupt_handler(unit,                                  ARAD_INT_NBI_PARITYERRINT,     0,     0,                                  arad_interrupt_handle_nbiparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,        ARAD_INT_NBI_ILKNCRC32LANEERRINDICATIONLANES0_11INT,     0,     0,        arad_interrupt_handle_nbiilkncrc32laneerrindicationlanes0_11int, NULL);
    interrupt_add_interrupt_handler(unit,       ARAD_INT_NBI_ILKNCRC32LANEERRINDICATIONLANES12_23INT,     0,     0,       arad_interrupt_handle_nbiilkncrc32laneerrindicationlanes12_23int, NULL);
    interrupt_add_interrupt_handler(unit,         ARAD_INT_NBI_ILKNLANEBADFRAMINGTYPEERRLANES0_11INT,    10,     1,         arad_interrupt_handle_nbiilknlanebadframingtypeerrlanes0_11int, arad_interrupt_handle_recurring_action_nbiilknlanebadframingtypeerrlanes0_11int);
    interrupt_add_interrupt_handler(unit,        ARAD_INT_NBI_ILKNLANEBADFRAMINGTYPEERRLANES12_23INT,    10,     1,        arad_interrupt_handle_nbiilknlanebadframingtypeerrlanes12_23int, arad_interrupt_handle_recurring_action_nbiilknlanebadframingtypeerrlanes12_23int);
    interrupt_add_interrupt_handler(unit,                ARAD_INT_NBI_ILKNLANEFRAMINGERRLANES0_11INT,    10,     1,                arad_interrupt_handle_nbiilknlaneframingerrlanes0_11int, arad_interrupt_handle_recurring_action_nbiilknlaneframingerrlanes0_11int);
    interrupt_add_interrupt_handler(unit,               ARAD_INT_NBI_ILKNLANEFRAMINGERRLANES12_23INT,    10,     1,               arad_interrupt_handle_nbiilknlaneframingerrlanes12_23int, arad_interrupt_handle_recurring_action_nbiilknlaneframingerrlanes12_23int);
    interrupt_add_interrupt_handler(unit,        ARAD_INT_NBI_ILKNLANEMETAFRAMELENGTHERRLANES0_11INT,    10,     1,        arad_interrupt_handle_nbiilknlanemetaframelengtherrlanes0_11int, arad_interrupt_handle_recurring_action_nbiilknlanemetaframelengtherrlanes0_11int);
    interrupt_add_interrupt_handler(unit,       ARAD_INT_NBI_ILKNLANEMETAFRAMELENGTHERRLANES12_23INT,    10,     1,       arad_interrupt_handle_nbiilknlanemetaframelengtherrlanes12_23int, arad_interrupt_handle_recurring_action_nbiilknlanemetaframelengtherrlanes12_23int);
    interrupt_add_interrupt_handler(unit,        ARAD_INT_NBI_ILKNLANEMETAFRAMEREPEATERRLANES0_11INT,    10,     1,        arad_interrupt_handle_nbiilknlanemetaframerepeaterrlanes0_11int, arad_interrupt_handle_recurring_action_nbiilknlanemetaframerepeaterrlanes0_11int);
    interrupt_add_interrupt_handler(unit,       ARAD_INT_NBI_ILKNLANEMETAFRAMEREPEATERRLANES12_23INT,    10,     1,       arad_interrupt_handle_nbiilknlanemetaframerepeaterrlanes12_23int, arad_interrupt_handle_recurring_action_nbiilknlanemetaframerepeaterrlanes12_23int);
    interrupt_add_interrupt_handler(unit,      ARAD_INT_NBI_ILKNLANEMETAFRAMESYNCWORDERRLANES0_11INT,    10,     1,      arad_interrupt_handle_nbiilknlanemetaframesyncworderrlanes0_11int, arad_interrupt_handle_recurring_action_nbiilknlanemetaframesyncworderrlanes0_11int);
    interrupt_add_interrupt_handler(unit,     ARAD_INT_NBI_ILKNLANEMETAFRAMESYNCWORDERRLANES12_23INT,    10,     1,     arad_interrupt_handle_nbiilknlanemetaframesyncworderrlanes12_23int, arad_interrupt_handle_recurring_action_nbiilknlanemetaframesyncworderrlanes12_23int);
    interrupt_add_interrupt_handler(unit,         ARAD_INT_NBI_ILKNLANESCRAMBLERSTATEERRLANES0_11INT,    10,     1,         arad_interrupt_handle_nbiilknlanescramblerstateerrlanes0_11int, arad_interrupt_handle_recurring_action_nbiilknlanescramblerstateerrlanes0_11int);
    interrupt_add_interrupt_handler(unit,        ARAD_INT_NBI_ILKNLANESCRAMBLERSTATEERRLANES12_23INT,    10,     1,        arad_interrupt_handle_nbiilknlanescramblerstateerrlanes12_23int, arad_interrupt_handle_recurring_action_nbiilknlanescramblerstateerrlanes12_23int);
    interrupt_add_interrupt_handler(unit,                  ARAD_INT_NBI_ILKNLANESYNCHERRLANES0_11INT,     0,     0,                  arad_interrupt_handle_nbiilknlanesyncherrlanes0_11int, NULL);
    interrupt_add_interrupt_handler(unit,                 ARAD_INT_NBI_ILKNLANESYNCHERRLANES12_23INT,     0,     0,                 arad_interrupt_handle_nbiilknlanesyncherrlanes12_23int, NULL);
    interrupt_add_interrupt_handler(unit,                          ARAD_INT_NBI_ILKNRX0RETRANSERRINT,     0,     0,                          arad_interrupt_handle_nbiilknrx0retranserrint, NULL);
    interrupt_add_interrupt_handler(unit,                          ARAD_INT_NBI_ILKNRX1RETRANSERRINT,     0,     0,                          arad_interrupt_handle_nbiilknrx1retranserrint, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_NBI_ILKNRXPARITYERRINT,     0,     0,                            arad_interrupt_handle_nbiilknrxparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                        ARAD_INT_NBI_ILKNRXPORT0ALIGNERRINT,     0,     0,                        arad_interrupt_handle_nbiilknrxport0alignerrint, NULL);
    interrupt_add_interrupt_handler(unit,                       ARAD_INT_NBI_ILKNRXPORT0STATUSERRINT,   100,     1,                       arad_interrupt_handle_nbiilknrxport0statuserrint, arad_interrupt_handle_recurring_action_nbiilknrxport0statuserrint);
    interrupt_add_interrupt_handler(unit,                        ARAD_INT_NBI_ILKNRXPORT1ALIGNERRINT,     0,     0,                        arad_interrupt_handle_nbiilknrxport1alignerrint, NULL);
    interrupt_add_interrupt_handler(unit,                       ARAD_INT_NBI_ILKNRXPORT1STATUSERRINT,   100,     1,                       arad_interrupt_handle_nbiilknrxport1statuserrint, arad_interrupt_handle_recurring_action_nbiilknrxport1statuserrint);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_NBI_ILKNTXPARITYERRINT,     0,     0,                            arad_interrupt_handle_nbiilkntxparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                       ARAD_INT_NBI_ILKNTXPORT0STATUSERRINT,     0,     0,                       arad_interrupt_handle_nbiilkntxport0statuserrint, NULL);
    interrupt_add_interrupt_handler(unit,                       ARAD_INT_NBI_ILKNTXPORT1STATUSERRINT,     0,     0,                       arad_interrupt_handle_nbiilkntxport1statuserrint, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_NBI_ERRBITSFROMEGQINT,   100,     1,                             arad_interrupt_handle_nbierrbitsfromegqint, arad_interrupt_handle_recurring_action_nbierrbitsfromegqint);
    interrupt_add_interrupt_handler(unit,                     ARAD_INT_NBI_ILEGALILKNINPUTFROMRX0INT,     0,     0,                     arad_interrupt_handle_nbiilegalilkninputfromrx0int, NULL);
    interrupt_add_interrupt_handler(unit,                     ARAD_INT_NBI_ILEGALILKNINPUTFROMRX1INT,     0,     0,                     arad_interrupt_handle_nbiilegalilkninputfromrx1int, NULL);
    interrupt_add_interrupt_handler(unit,             ARAD_INT_NBI_ILKNTX1_32_BYTE_CONSECUTIVECLKSINT,     0,     0,             arad_interrupt_handle_nbiilkntx1_32byte_consecutiveclksint, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_NBI_RXELKOVFINT,     0,     0,                                   arad_interrupt_handle_nbirxelkovfint, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_NBI_RXFIFOOVERFLOWINT ,     0,     0,                             arad_interrupt_handle_nbirxfifooverflowint, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_NBI_TXFIFOOVERFLOWINT,     0,     0,                             arad_interrupt_handle_nbitxfifooverflowint, NULL);
    interrupt_add_interrupt_handler(unit,                               ARAD_INT_NBI_WRONGEGQWORDINT,     0,     0,                               arad_interrupt_handle_nbiwrongegqwordint, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_NBI_WRONGPORTFROMEGQINT,     0,     0,                           arad_interrupt_handle_nbiwrongportfromegqint, NULL);
    interrupt_add_interrupt_handler(unit,                           ARAD_INT_NBI_WRONGWORDFROMMALINT,     0,     0,                           arad_interrupt_handle_nbiwrongwordfrommalint, NULL);
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_NBI_STATREADERRINT,     0,     0,                                arad_interrupt_handle_nbistatreaderrint, NULL);
    interrupt_add_interrupt_handler(unit,                  ARAD_INT_NBI_STATTXBURSTLENGTHOVERFLOWINT,     0,     0,                  arad_interrupt_handle_nbistattxburstlengthoverflowint, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_OAMP_ECC_1BERRINT,     0,     0,                                 arad_interrupt_handle_oampecc_1berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_OAMP_ECC_2BERRINT,     0,     0,                                 arad_interrupt_handle_oampecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_OAMP_PARITYERRINT,     0,     0,                                 arad_interrupt_handle_oampparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_OAMP_PENDINGEVENT,     0,     0,                                 arad_interrupt_handle_oamppendingevent, NULL);
    interrupt_add_interrupt_handler(unit,                      ARAD_INT_OAMP_RMAPEMERRORCAMTABLEFULL,     0,     0,                      arad_interrupt_handle_oamprmapemerrorcamtablefull, NULL);
    interrupt_add_interrupt_handler(unit,                  ARAD_INT_OAMP_RMAPEMERRORDELETEUNKNOWNKEY,     0,     0,                  arad_interrupt_handle_oamprmapemerrordeleteunknownkey, NULL);
    interrupt_add_interrupt_handler(unit,              ARAD_INT_OAMP_RMAPEMERRORREACHEDMAXENTRYLIMIT,     0,     0,              arad_interrupt_handle_oamprmapemerrorreachedmaxentrylimit, NULL);
    interrupt_add_interrupt_handler(unit,                    ARAD_INT_OAMP_RMAPEMERRORTABLECOHERENCY,     0,     0,                    arad_interrupt_handle_oamprmapemerrortablecoherency, NULL);
    interrupt_add_interrupt_handler(unit,             ARAD_INT_OAMP_RMAPEMMANAGEMENTUNITFAILUREVALID,     0,     0,             arad_interrupt_handle_oamprmapemmanagementunitfailurevalid, NULL);
    interrupt_add_interrupt_handler(unit,                ARAD_INT_OAMP_RMAPEMWARNINGINSERTEDEXISTING,     0,     0,                arad_interrupt_handle_oamprmapemwarninginsertedexisting, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_OLP_ECC_2BERRINT,     0,     0,                                  arad_interrupt_handle_olpecc_2berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_OLP_ECC_1BERRINT,     0,     0,                                  arad_interrupt_handle_olpecc_1berrint, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_OLP_PARITYERRINT,     0,     0,                                  arad_interrupt_handle_olpparityerrint, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_PORT_MIB_TSC0_MEM_ERR,     0,     0,                            arad_interrupt_handle_port_mib_tsc0_mem_err, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_PORT_MIB_TSC1_MEM_ERR,     0,     0,                            arad_interrupt_handle_port_mib_tsc1_mem_err, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_PORT_MIB_RSC0_MEM_ERR,     0,     0,                            arad_interrupt_handle_port_mib_rsc0_mem_err, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_PORT_MIB_RSC1_MEM_ERR,     0,     0,                            arad_interrupt_handle_port_mib_rsc1_mem_err, NULL);  
    interrupt_add_interrupt_handler(unit,                                ARAD_INT_RTP_LINKMASKCHANGE,     0,     0,                                arad_interrupt_handle_rtplinkmaskchange, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_SCH_DCDECCERRORFIXED,     0,     0,                              arad_interrupt_handle_schdcdeccerrorfixed, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_SCH_DHDECCERRORFIXED,     0,     0,                              arad_interrupt_handle_schdhdeccerrorfixed, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_SCH_FDMDECCERRORFIXED,     0,     0,                             arad_interrupt_handle_schfdmdeccerrorfixed, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_SCH_FLHCLECCERRORFIXED,     0,     0,                            arad_interrupt_handle_schflhcleccerrorfixed, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_SCH_FLHFQECCERRORFIXED,     0,     0,                            arad_interrupt_handle_schflhfqeccerrorfixed, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_SCH_FLHHRECCERRORFIXED,     0,     0,                            arad_interrupt_handle_schflhhreccerrorfixed, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_SCH_FLTCLECCERRORFIXED,     0,     0,                            arad_interrupt_handle_schfltcleccerrorfixed, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_SCH_FLTFQECCERRORFIXED,     0,     0,                            arad_interrupt_handle_schfltfqeccerrorfixed, NULL);
    interrupt_add_interrupt_handler(unit,                            ARAD_INT_SCH_FLTHRECCERRORFIXED,     0,     0,                            arad_interrupt_handle_schflthreccerrorfixed, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_SCH_FSMECCERRORFIXED,     0,     0,                              arad_interrupt_handle_schfsmeccerrorfixed, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_SCH_SHDDECCERRORFIXED,     0,     0,                             arad_interrupt_handle_schshddeccerrorfixed, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_DCDECCERROR,     0,     0,                                   arad_interrupt_handle_schdcdeccerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_DHDECCERROR,     0,     0,                                   arad_interrupt_handle_schdhdeccerror, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_FDMDECCERROR,     0,     0,                                  arad_interrupt_handle_schfdmdeccerror, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_SCH_FLHCLECCERROR,     0,     0,                                 arad_interrupt_handle_schflhcleccerror, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_SCH_FLHFQECCERROR,     0,     0,                                 arad_interrupt_handle_schflhfqeccerror, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_SCH_FLHHRECCERROR,     0,     0,                                 arad_interrupt_handle_schflhhreccerror, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_SCH_FLTCLECCERROR,     0,     0,                                 arad_interrupt_handle_schfltcleccerror, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_SCH_FLTFQECCERROR,     0,     0,                                 arad_interrupt_handle_schfltfqeccerror, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_SCH_FLTHRECCERROR,     0,     0,                                 arad_interrupt_handle_schflthreccerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_FSMECCERROR,     0,     0,                                   arad_interrupt_handle_schfsmeccerror, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_SHDDECCERROR,     0,     0,                                  arad_interrupt_handle_schshddeccerror, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_SCH_ACTFLOW_BADPARAMS,     0,     0,                             arad_interrupt_handle_schactflow_badparams, NULL);
    interrupt_add_interrupt_handler(unit,                              ARAD_INT_SCH_RESTARTFLOWEVENT,     0,     0,                              arad_interrupt_handle_schrestartflowevent, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_SCH_SMPFULLLEVEL1,     0,     0,                                 arad_interrupt_handle_schsmpfulllevel1, NULL);
    interrupt_add_interrupt_handler(unit,                                 ARAD_INT_SCH_SMPFULLLEVEL2,     0,     0,                                 arad_interrupt_handle_schsmpfulllevel2, NULL);
    interrupt_add_interrupt_handler(unit,                             ARAD_INT_SCH_SHPFLOW_BADPARAMS,     0,     0,                             arad_interrupt_handle_schshpflow_badparams, NULL);
    interrupt_add_interrupt_handler(unit,                                     ARAD_INT_SCH_SMPBADMSG,     0,     0,                                     arad_interrupt_handle_schsmpbadmsg, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_CAL0PARERROR,     0,     0,                                  arad_interrupt_handle_schcal0parerror, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_CAL1PARERROR,     0,     0,                                  arad_interrupt_handle_schcal1parerror, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_CAL2PARERROR,     0,     0,                                  arad_interrupt_handle_schcal2parerror, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_CAL3PARERROR,     0,     0,                                  arad_interrupt_handle_schcal3parerror, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_CAL4PARERROR,     0,     0,                                  arad_interrupt_handle_schcal4parerror, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_CSSTPARERROR,     0,     0,                                  arad_interrupt_handle_schcsstparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_DPNPARERROR,     0,     0,                                   arad_interrupt_handle_schdpnparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_DRMPARERROR,     0,     0,                                   arad_interrupt_handle_schdrmparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_DSMPARERROR,     0,     0,                                   arad_interrupt_handle_schdsmparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_FDMSPARERROR,     0,     0,                                  arad_interrupt_handle_schfdmsparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_FFMPARERROR,     0,     0,                                   arad_interrupt_handle_schffmparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_FGMPARERROR,     0,     0,                                   arad_interrupt_handle_schfgmparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_FIMPARERROR,     0,     0,                                   arad_interrupt_handle_schfimparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_FQMPARERROR,     0,     0,                                   arad_interrupt_handle_schfqmparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_FSFPARERROR,     0,     0,                                   arad_interrupt_handle_schfsfparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_PSSTPARERROR,     0,     0,                                  arad_interrupt_handle_schpsstparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_CSDTPARERROR,     0,     0,                                  arad_interrupt_handle_schcsdtparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_PSDDPARERROR,     0,     0,                                  arad_interrupt_handle_schpsddparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_PSDTPARERROR,     0,     0,                                  arad_interrupt_handle_schpsdtparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_TMCPARERROR,     0,     0,                                  arad_interrupt_handle_schtmcparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_PSWPARERROR,     0,     0,                                   arad_interrupt_handle_schpswparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_SCCPARERROR,     0,     0,                                   arad_interrupt_handle_schsccparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_SCTPARERROR,     0,     0,                                   arad_interrupt_handle_schsctparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_SEMPARERROR,     0,     0,                                   arad_interrupt_handle_schsemparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_SHCPARERROR,     0,     0,                                   arad_interrupt_handle_schshcparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                  ARAD_INT_SCH_SHDSPARERROR,     0,     0,                                  arad_interrupt_handle_schshdsparerror, NULL);
    interrupt_add_interrupt_handler(unit,                                   ARAD_INT_SCH_SIMPARERROR,     0,     0,                                   arad_interrupt_handle_schsimparerror, NULL);

}

/*
 * Generic None handles - for CB without specific handling
 */
int arad_interrupt_handle_generic_none(int unit, int block_instance, arad_interrupt_type en_arad_interrupt,char *msg)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    
    /* print info */
    LOG_ERROR(BSL_LS_BCM_INTR,
              (BSL_META_U(unit,
                          "WARNING:unhadle error: ")));

    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_recurring_action_handle_generic_none(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Interrupt Handeler functions.
 */
int arad_interrupt_handle_cfchcfcoobrx0crcerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "HCFC-OOB-RX0: CRC error reported.");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfchcfcoobrx0headererr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "HCFC-OOB-RX0: error reported on the MSG-Type or FWD fields.");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfchcfcoobrx1crcerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "HCFC-OOB-RX1: CRC error reported.");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfchcfcoobrx1headererr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "HCFC-OOB-RX1: error reported on the MSG-Type or FWD fields.");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcilkn0oobrxcrcerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "ILKN-OOB-RX: CRC error received on interface 0." );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcilkn0oobrxifstaterr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "ILKN-OOB-RX: Interface-Status error received on interface 0." );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcilkn0oobrxlanesstaterr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, CFC_ILKN_0_OOB_RX_LANES_STATUSr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt = soc_reg_field_get(unit, CFC_ILKN_0_OOB_RX_LANES_STATUSr, err_cnt, ILKN_0_OOB_RX_LANES_STATUSf);
    sal_sprintf(special_msg, "ILKN-OOB-RX: Lane-Status error received on interface 0. ILKN_0_OOB_RX_LANES_STATUS= 0x%08x", err_field_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcilkn0oobrxoverflow(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "ILKN-OOB-RX: Overflow error received on interface 0." );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcilkn1oobrxcrcerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "ILKN-OOB-RX: CRC error received on interface 1." );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcilkn1oobrxifstaterr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "ILKN-OOB-RX: Interface-Status error received on interface 1." );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcilkn1oobrxlanesstaterr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, CFC_ILKN_1_OOB_RX_LANES_STATUSr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt = soc_reg_field_get(unit, CFC_ILKN_1_OOB_RX_LANES_STATUSr, err_cnt, ILKN_1_OOB_RX_LANES_STATUSf);
    sal_sprintf(special_msg, "ILKN-OOB-RX: Lane-Status error received on interface 1. ILKN_1_OOB_RX_LANES_STATUS= 0x%08x", err_field_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcilkn1oobrxoverflow(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Overflow error received on interface 1." );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_cfcspioobrx0dip2alarm(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "DIP2 error counter is above SpiOobRx0Dip2AlrmTh");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcspioobrx0dip2err(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, CFC_SPI_OOB_RX_1_ERROR_COUNTERr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt = soc_reg_field_get(unit, CFC_SPI_OOB_RX_0_ERROR_COUNTERr, err_cnt, SPI_OOB_RX_0_DIP_2_ERR_CNTf);
    sal_sprintf(special_msg, "DIP2 error detected. SPI_OOB_RX_0_DIP_2_ERR_CNT= 0x%08x", err_field_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcspioobrx0frmerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, CFC_SPI_OOB_RX_0_ERROR_COUNTERr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt = soc_reg_field_get(unit, CFC_SPI_OOB_RX_0_ERROR_COUNTERr, err_cnt, SPI_OOB_RX_0_FRM_ERR_CNTf);
    sal_sprintf(special_msg, "Frame error occurred. SPI_OOB_RX_0_FRM_ERR_CNT= 0x%08x", err_field_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}



int arad_interrupt_handle_cfcspioobrx0lockerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "SPI-OOB-RX-0 isn't locked.No correct framing pattern");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_cfcspioobrx0outoffrm(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Out-Of-Frame error counter is above SpiOobRx0OutOfFrmTh");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcspioobrx0wderr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "SPI-OOB-RX-0: Watch-Dog error detected" );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcspioobrx1dip2alarm(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "DIP2 error counter is above SpiOobRx1Dip2AlrmTh");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcspioobrx1dip2err(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, CFC_SPI_OOB_RX_1_ERROR_COUNTERr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt = soc_reg_field_get(unit, CFC_SPI_OOB_RX_1_ERROR_COUNTERr, err_cnt, SPI_OOB_RX_1_DIP_2_ERR_CNTf);
    sal_sprintf(special_msg, " DIP2 error detected. SPI_OOB_RX_1_DIP_2_ERR_CNT= 0x%08x", err_field_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcspioobrx1frmerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, CFC_SPI_OOB_RX_1_ERROR_COUNTERr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt = soc_reg_field_get(unit, CFC_SPI_OOB_RX_1_ERROR_COUNTERr, err_cnt, SPI_OOB_RX_1_FRM_ERR_CNTf);
    sal_sprintf(special_msg, "Frame error occurred. SPI_OOB_RX_1_FRM_ERR_CNT= 0x%08x", err_field_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_cfcspioobrx1lockerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */

    sal_sprintf(special_msg, "SPI-OOB-RX-1 isn't locked");
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_cfcspioobrx1outoffrm(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Out-Of-Frame error counter is above SpiOobRx0OutOfFrmTh");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_cfcspioobrx1wderr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Watch-Dog error detected" );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpscrps0cntovf(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, CRPS_CRPS_PIPE_COUNTERS_2r, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);                             
   
    err_field_cnt = soc_reg_field_get(unit, CRPS_CRPS_PIPE_COUNTERS_2r, err_cnt, CRPS_N_OVF_CNTRS_CNTf);
    sal_sprintf(special_msg, "Counter is overflown. CRPS_N_OVF_CNTRS_CNT= 0x%08x", err_field_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpscrps0prereadfifofull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    sal_sprintf(special_msg, "clear the FIFO (by reading counters values) to allow more counters to be written to it. " );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpscrps1cntovf(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* print info */
    sal_sprintf(special_msg, "read CrpsOvfCntrsCnt 0x0141" );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpscrps1prereadfifofull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;
    /* print info */
    sal_sprintf(special_msg, "clear the FIFO (by reading counters values) to allow more counters to be written to it." );


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpscrps2cntovf(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* print info */
    sal_sprintf(special_msg, "read CrpsOvfCntrsCnt (0x0142) ." );


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpscrps2prereadfifofull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* print info */
    sal_sprintf(special_msg, "clear the FIFO (by reading counters values) to allow more counters to be written to it. " );


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpscrps3cntovf(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* print info */
    sal_sprintf(special_msg, "Crps3CntOvf: Indicates a counter has overflown,read CrpsOvfCntrsCnt (0x0143)" );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpscrps3prereadfifofull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* print info */
    sal_sprintf(special_msg, "Indicates the pre fetch fifo is full. Crps counters should be read. Valid only when working in PREFETCH read mode" );


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpsparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpsegqinvldadacc(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Check EPNI configuration for command EGQ");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpsepniainvldadacc(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Check EPNI configuration for command EPNI-A");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpsepnibinvldadacc(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Check EPNI configuration for command EPNI-B");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpsiqmainvldadacc(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Check EPNI configuration for command IQM-A");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpsiqmbinvldadacc(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Check EPNI configuration for command IQM-B");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpsiqmcinvldadacc(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Check EPNI configuration for command IQM-C");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpsiqmdinvldadacc(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Check EPNI configuration for command IQM-D");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpsirppainvldadacc(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Check EPNI configuration for command IRPP-A");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_crpsirppbinvldadacc(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "IrppBInvldAdAcc: Check EPNI configuration for command IRPP-B");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_drcaoppdataerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_drcboppdataerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_drccoppdataerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_drcdoppdataerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_drceoppdataerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_drcfoppdataerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_drcgoppdataerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_drchoppdataerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqinvalidotmint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Packet is filtered or trapped as the OTM value is invalid (Verify configuration sending device). ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqdeletefifofull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Delete FIFO is full.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}
 
int arad_interrupt_handle_egqdroppeducdbint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Packet was discarded due to lack of unicast data buffers.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqdroppeducpdint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Packet was discarded due to lack of unicast packet descriptors. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqucpktportff(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Received a unicast packet with out-fap port equal 255. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqcsrmissingeoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Packet injected from CPU with  missing EOP error. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqcsrpktsizeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Packet injected from CPU with size error. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqcsrsopandeoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " Packet injected from CPU with size error. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqcsrunexpectedeoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " Packet injected from CPU with  unexpected EOP error. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqrcmmcerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqrcmmcerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqrcmucerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqrcmucerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256mccellsizeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256mccellsizeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 secondary pipe - cell size error.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256mceoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256mceoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 secondary pipe - eop error.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256mcfragnumerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256mcfragnumerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 secondary pipe - frag number error.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256mcmissingsoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256mcmissingsoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 secondary pipe - missing sop error.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256mcpktcrcerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256mcpktcrcerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 secondary pipe - packet crc error. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256mcpktsizeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256mcpktsizeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 secondary pipe - packet size error. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256mcsopintrmoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256mcsopintrmoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 secondary pipe - sop intr mop error. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256uccellsizeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256uccellsizeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 primary pipe - cell size error. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256uceoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256uceoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 primary pipe - eop error. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256ucfragnumerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256ucfragnumerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 primary pipe - frag number error. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256ucmissingsoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256ucmissingsoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 primary pipe - missing sop error. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256ucpktcrcerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256ucpktcrcerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 primary pipe - packer crc error.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256ucpktsizeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256ucpktsizeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 primary pipe - packet size error. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsc256ucsopintrmoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsc256ucsopintrmoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC 256 primary pipe - sop intr mop error.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvsceopsizeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvsceopsizeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC EOP size error. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvscfix129err(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvscfix129err(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC Fix 129 error.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvscfragnumerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvscfragnumerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
   int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC fragment number error.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvscmissingsoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvscmissingsoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC missing SOP error.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvscpktcrcerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvscpktcrcerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{

    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC packet CRC error.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvscpktsizeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvscpktsizeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC packet size error.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_egqvscsopintrmoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_egqvscsopintrmoperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " VSC SOP in MOP error.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epniecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epniecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epniparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;    
    dcmn_interrupt_mem_err_info  shadow_correct_info;    
    uint32 address;
    void *info;
    uint8 is_em_ser;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = arad_pp_em_ser_address_get(unit, shadow_correct_info.mem, shadow_correct_info.array_index, shadow_correct_info.min_index, &address, &is_em_ser);
    SOCDNX_IF_ERR_EXIT(rc); 

    if (is_em_ser) {
        /* EM SER */
        corrective_action = DCMN_INT_CORR_ACT_EPNI_EM_SOFT_RECOVERY;
        info = (void*)(&address);
    }
    else {
        /* non-EM SER */
        info = (void*)(&shadow_correct_info);
    }

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, info, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}
 
int arad_interrupt_handle_epnimirrovfint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " Mirroring FIFO in the egress queue has overflowed.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epnieeitypeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " EEI type encoding is unknown.Verify the configuration of ingress PP generation of system headers. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epnieeslastactionnotac(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Last EEDB entry is not AC. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epnifirstencmplsheadercodeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "MPLS build command generated in first encapsulation while header code is not ETH/IP/MPLS/SNOOP. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epnifirstencmplsheadercodesnoop(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "MPLS build command generated in first encapsulation while header code is SNOOP.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epniphperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " MPLS pop requested by the encapsulation entry and FHEI extension.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epniphpnextipprotocolerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " Egress PP performed PHP to IP and IP version error was identified.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epnisecondenciptunnelheadercodeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " IP tunnel build command generated in second encapsulation while header code is not ETH/IP/SNOOP. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epnisecondenciptunnelheadercodesnoop(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " IP tunnel build command generated in second encapsulation while header code is SNOOP. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epnisecondencmplsheadercodeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "MPLS build command generated in second encapsulation while header code is not ETH/IP/MPLS/SNOOP. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epnisecondencmplsheadercodesnoop(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "MPLS build command generated in second encapsulation while header code is SNOOP. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epnisecondenctrillheadercodeerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " TRILL build command generated in first encapsulation while header code is not ETH/SNOOP. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_epnisecondenctrillheadercodesnoop(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "TRILL build command generated in first encapsulation while header code is SNOOP. ");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fcrcpucntcellfne(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    soc_reg_above_64_val_t pr_reg;
    char val_str[110];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg_above_64_get(unit, FCR_CONTROL_CELL_FIFO_BUFFERr, block_instance, 0, pr_reg);
    SOCDNX_IF_ERR_EXIT(rc);  
   
                                                                            
    format_long_integer(val_str, pr_reg, SOC_REG_ABOVE_64_MAX_SIZE_U32);

    sal_sprintf(special_msg, "CPU-Control-FIFO not empty.FCR_CONTROL_CELL_FIFO_BUFFER=%s", val_str);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_fcrreachfifoovf(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "ReachFifoOvf: Check rechability message Generation Rate (RMGR) configuration of link partner.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fcrecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fcrecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fctunrchdestevent(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    uint32 fap_id = 0x0, cell_type=0;
    dcmn_int_corr_act_type corrective_action = DCMN_INT_CORR_ACT_NONE;
    char cell_type_credit[] = "Credit";
    char cell_type_flow_status[] = "Flow-Status";
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    /* get the fap ID of the unreachable destination */

    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        rc = arad_interrupt_data_collection_for_fctunrchdestevent(unit, block_instance, en_arad_interrupt, &fap_id, &cell_type, &corrective_action);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    /* print info */
    sal_sprintf(special_msg, "unreach destination fap_id= 0x%x cell_type=%s\n", fap_id, cell_type == ARAD_INTERRUPTS_FCT_UNREACHABLE_DEST_CELL_TYPE_CREDIT ? cell_type_credit : cell_type_flow_status);
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, &fap_id, NULL);
    BCMDNX_IF_ERR_EXIT(rc);
 
exit:
    SOCDNX_FUNC_RETURN; 
  
}

int arad_interrupt_handle_fcrprmecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrprmfdradesccntob(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " Indicates that the counter Fifo Discard Counter Primary has overflowed");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrprmfdrbunexpcell(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{


    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Indicates an irrecoverable error in the primary path link 18-35.Reflected in counter FDR_FDR_FIFO_DISCARD_COUNTER_B_PRIMARY");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrprmaltoa(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    int iterations, delay_us;
    
    SOCDNX_INIT_FUNC_DEFS;

    iterations = ARAD_INT_FDRPRMALTOA_COR_ITERATION;
    delay_us = ARAD_INT_FDRPRMALTOA_COR_DELAY_US;

    /* data collection */
    sal_sprintf(special_msg, "One of links went down logically - primary path");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_CLEAR_CHECK, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_CLEAR_CHECK, (void*)&iterations, (void*)&delay_us);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_fdrprmaltoa(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;
    dcmn_int_corr_act_type corrective_action ;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links, ARAD_INT_LINK_INVOLVED, (SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_recuring_action_fdr_alto(unit, block_instance, en_arad_interrupt, &corrective_action, array_links, special_msg);
    SOCDNX_IF_ERR_EXIT(rc); 
    
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,array_links,NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}



int arad_interrupt_handle_fdrprmaltob(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "PrmAltoB: One of links went down logically - primary path");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_fdrprmaltob(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;
    dcmn_int_corr_act_type corrective_action ;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links, ARAD_INT_LINK_INVOLVED, (SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_recuring_action_fdr_alto(unit, block_instance, en_arad_interrupt, &corrective_action, array_links, special_msg);
    SOCDNX_IF_ERR_EXIT(rc); 
    
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,array_links,NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}





int arad_interrupt_handle_fdrprmcpudatacellfnea0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{

    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDRA_PRIMARYr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt = soc_reg_field_get(unit, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDRA_PRIMARYr, err_cnt, PRM_CPU_DATA_CELL_FOA_0f);
    sal_sprintf(special_msg, "CPU FIFO of Links 0-11 primary path is non empty  PRM_CPU_DATA_CELL_FOA_0= 0x%08x", err_field_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrprmcpudatacellfnea1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDRA_PRIMARYr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt = soc_reg_field_get(unit, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDRA_PRIMARYr, err_cnt, PRM_CPU_DATA_CELL_FOA_1f);
    sal_sprintf(special_msg, "CPU FIFO of Links 12-17 primary path is non empty  PRM_CPU_DATA_CELL_FOA_1= 0x%08x", err_field_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrprmcpudatacellfneb0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDRA_PRIMARYr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt = soc_reg_field_get(unit, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDRA_PRIMARYr, err_cnt, PRM_CPU_DATA_CELL_FOA_0f);
    sal_sprintf(special_msg, "CPU FIFO of Links 24-35 primary path is non empty.PRM_CPU_DATA_CELL_FOA_0 = 0x%08x", err_field_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrprmcpudatacellfneb1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDRA_PRIMARYr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt = soc_reg_field_get(unit, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDRA_PRIMARYr, err_cnt, PRM_CPU_DATA_CELL_FOA_1f);
    sal_sprintf(special_msg, "CPU FIFO of Links 18-23 primary path is non empty.PRM_CPU_DATA_CELL_FOA_1 = 0x%08x", err_field_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_fdrprmfdroutputoutofsync(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "PrmFdrOutputOutOfSync:Indicates an irrecoverable error in the output of the primary path" );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_fdrprmfdraoutofsync(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Indicates an irrecoverable error in the primary path link 0-17");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_fdrprmfdrboutofsync(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "PrmFdrbOutOfSync:Indicates an irrecoverable error in the primary path link 18-35");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;


}

int arad_interrupt_handle_fdrprmifmafoa(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_fdrprmifmbfob(unit, block_instance,&corrective_action, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDRA_PRIMARYr, 
                                                          PRM_MAX_IFMAFFDRAf, special_msg);
    SOCDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,NULL,NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;


}

int arad_interrupt_handle_recurring_action_fdrprmifmafoa(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    sal_sprintf(special_msg, "IFMA FIFO overflow - primary path." );
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrprmifmafob(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_fdrprmifmbfob(unit,block_instance, &corrective_action, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDR_B_PRIMARYr, 
                                                          PRM_MAX_IFMAFFDRBf, special_msg);
    SOCDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,NULL,NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_fdrprmifmafob(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrprmifmbfoa(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_fdrprmifmbfob(unit, block_instance,&corrective_action, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDRA_PRIMARYr, 
                                                          PRM_MAX_IFMBFFDRAf, special_msg);
    SOCDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,NULL,NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_fdrprmifmbfoa(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    sal_sprintf(special_msg, "IFMA FIFO overflow - Secondary path.. Recurring" );
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrprmifmbfob(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_fdrprmifmbfob(unit,block_instance, &corrective_action, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDR_B_PRIMARYr, 
                                                          PRM_MAX_IFMBFFDRBf, special_msg);
    SOCDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,NULL,NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_fdrprmifmbfob(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrprmfdraunexpcell(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Unexpected cell error - path links 0-17.");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrprmfdradesccntoa(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "FDRAdesccntoA: Discard cell counter overflow - Secondary path");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrsecfdraunexpcell(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrsecfdradesccntoa(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrsecfdradesccntob(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrsecfdrbunexpcell(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrsecaltoa(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "SecAltoA: One of links went down logically - Secondary path A" );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_fdrsecaltob(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "SecAltoB: One of links went down logically - Secondary path B" );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_fdrseccpudatacellfnea0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "SecCpudatacellfneA0: CPU FIFO of Links 0-11 Secondary path is non empty " );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrseccpudatacellfnea1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "SecCpudatacellfneA1: CPU FIFO of Links 11-17 primary path is non empty");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrseccpudatacellfneb0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDR_B_SECONDARYr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt = soc_reg_field_get(unit, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDR_B_SECONDARYr, err_cnt, FIELD_0_0f);
    sal_sprintf(special_msg, "PrmCpudatacellfneB0:CPU FIFO of Links 24-35 secondary path is non empty.PRM_MAX_IFMFA_NUM_FDRB0 = 0x%08x", err_field_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrseccpudatacellfneb1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDR_B_SECONDARYr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt = soc_reg_field_get(unit, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDR_B_SECONDARYr, err_cnt, FIELD_1_1f);
    sal_sprintf(special_msg, "PrmCpudatacellfneB0:CPU FIFO of Links 18-23 secondary path is non empty.PRM_MAX_IFMFA_NUM_FDRB1 = 0x%08x", err_field_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrsecfdroutputoutofsync(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "SecFdrOutputOutOfSync:  irrecoverable error in the output of the secondary path" );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrsecfdraoutofsync(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "SecFdraOutOfSync: irrecoverable error in the secondary path link 0-17" );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;



}

int arad_interrupt_handle_fdrsecfdrboutofsync(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "SecFdrbOutOfSync: irrecoverable error in the secondary path link 18-35" );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrsecifmafoa(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_fdrprmifmbfob(unit,block_instance, &corrective_action, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDRA_SECONDARYr, 
                                                          SEC_MAX_IFMAFFDRAf, special_msg);
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_fdrsecifmafoa(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrsecifmafob(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_fdrprmifmbfob(unit,block_instance, &corrective_action, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDR_B_SECONDARYr, 
                                                          SEC_MAX_IFMAFFDRBf, special_msg);
    SOCDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,NULL,NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_recurring_action_fdrsecifmafob(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    sal_sprintf(special_msg, "IFMA FIFO overflow - Secondary path.. Recurring" );
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrsecifmbfoa(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_fdrprmifmbfob(unit,block_instance, &corrective_action, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDRA_SECONDARYr, 
                                                          SEC_MAX_IFMBFFDRAf, special_msg);
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_fdrsecifmbfoa(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrsecifmbfob(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_fdrprmifmbfob(unit,block_instance, &corrective_action, FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDR_B_SECONDARYr, 
                                                           SEC_MAX_IFMBFFDRBf, special_msg);
    SOCDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,NULL,NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_recurring_action_fdrsecifmbfob(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* print info */
    sal_sprintf(special_msg, "IFMB FIFO overflow - Secondary path.. Recurring" );
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrfdrunexperror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* print info */
    sal_sprintf(special_msg, "FdrUnExpError: link Unexpected cell error." );


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrinbandfifofull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;


    /* print info */
    sal_sprintf(special_msg, "InBandFifoFull Reduce FDT inband shaper rate. " );


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrprmecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdrsececc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdtecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdtecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdtilegalirepacketsizeint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    sal_sprintf(special_msg, "Ilegal packet size from IRE, Reprogram IRE filter.");

    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fdtparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmacrxcrcerrn_int_0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{

    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_rxcrcerrn_int(unit, block_instance, en_arad_interrupt, array_links, special_msg);
    BCMDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, (void*)array_links ,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_interrupt_handle_recurring_action_fmacrxcrcerrn_int_0(
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    char *msg)
{

    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , (SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_rxcrcerrn_int(unit, block_instance, en_arad_interrupt, array_links, special_msg);
    BCMDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, (void*)array_links ,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_fmacrxcrcerrn_int_1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{

    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_rxcrcerrn_int(unit, block_instance, en_arad_interrupt, array_links, special_msg);
    BCMDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, (void*)array_links ,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_interrupt_handle_recurring_action_fmacrxcrcerrn_int_1(
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    char *msg)
{

    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , (SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_rxcrcerrn_int(unit, block_instance, en_arad_interrupt, array_links, special_msg);
    BCMDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, (void*)array_links ,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_interrupt_handle_fmacrxcrcerrn_int_2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_rxcrcerrn_int(unit, block_instance, en_arad_interrupt, array_links, special_msg);
    BCMDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, (void*)array_links ,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_interrupt_handle_recurring_action_fmacrxcrcerrn_int_2(
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    char *msg)
{

    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , (SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_rxcrcerrn_int(unit, block_instance, en_arad_interrupt, array_links, special_msg);
    BCMDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, (void*)array_links ,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_interrupt_handle_fmacrxcrcerrn_int_3(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_rxcrcerrn_int(unit, block_instance, en_arad_interrupt, array_links, special_msg);
    BCMDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, (void*)array_links ,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_interrupt_handle_recurring_action_fmacrxcrcerrn_int_3(
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    char *msg)
{

    int rc ;
    /* links array for corrective action */      
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , (SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_rxcrcerrn_int(unit, block_instance, en_arad_interrupt, array_links, special_msg);
    BCMDNX_IF_ERR_EXIT(rc); 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, (void*)array_links ,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_interrupt_handle_fmacwrongsize_int_0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{

     int rc;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_wrongsize_int(unit, block_instance, en_arad_interrupt, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_recurring_action_fmacwrongsize_int_0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{

    int rc ;
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;

    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , ( SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_recurring_action_mac_wrongsize_int(unit, block_instance, en_arad_interrupt, special_msg , array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, NULL);
    BCMDNX_IF_ERR_EXIT(rc);
    
    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
   
}

int arad_interrupt_handle_fmacwrongsize_int_1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
   int rc;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_wrongsize_int(unit, block_instance, en_arad_interrupt, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
 
}

int arad_interrupt_handle_recurring_action_fmacwrongsize_int_1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc ;
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;

    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , ( SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_recurring_action_mac_wrongsize_int(unit, block_instance, en_arad_interrupt, special_msg , array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, NULL);
    BCMDNX_IF_ERR_EXIT(rc);
    
    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;  
    
}

int arad_interrupt_handle_fmacwrongsize_int_2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_wrongsize_int(unit, block_instance, en_arad_interrupt, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 

}

int arad_interrupt_handle_recurring_action_fmacwrongsize_int_2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc ;
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;

    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , ( SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_recurring_action_mac_wrongsize_int(unit, block_instance, en_arad_interrupt, special_msg , array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, NULL);
    BCMDNX_IF_ERR_EXIT(rc);
    
    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;   
}

int arad_interrupt_handle_fmacwrongsize_int_3(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_wrongsize_int(unit, block_instance, en_arad_interrupt, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;  
}

int arad_interrupt_handle_recurring_action_fmacwrongsize_int_3(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc ;
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;

    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , ( SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_recurring_action_mac_wrongsize_int(unit, block_instance, en_arad_interrupt, special_msg , array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, NULL);
    BCMDNX_IF_ERR_EXIT(rc);
    
    /* corrective actions */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;      
}

int arad_interrupt_handle_fmaclos_int_0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_los_int(unit, block_instance, en_arad_interrupt, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:

SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmaclos_int_1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_los_int(unit, block_instance, en_arad_interrupt, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmaclos_int_2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_los_int(unit, block_instance, en_arad_interrupt, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmaclos_int_3(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_los_int(unit, block_instance, en_arad_interrupt, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmacrxlostofsync_0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    soc_port_t link;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_rxlostofsync_int(unit, block_instance, en_arad_interrupt, &link, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &link, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmacrxlostofsync_1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    soc_port_t link;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_rxlostofsync_int(unit, block_instance, en_arad_interrupt, &link, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &link , NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmacrxlostofsync_2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    soc_port_t link;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_rxlostofsync_int(unit, block_instance, en_arad_interrupt, &link, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &link, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmacrxlostofsync_3(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    soc_port_t link;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_rxlostofsync_int(unit, block_instance, en_arad_interrupt, &link, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &link, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_fmacdecerr_int_0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;   
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_decerr_int(unit, block_instance, en_arad_interrupt, FALSE, &corrective_action, special_msg, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg );
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

int arad_interrupt_handle_recurring_fmacdecerr_int_0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , ( SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_decerr_int(unit, block_instance, en_arad_interrupt, TRUE, &corrective_action, special_msg, array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg );
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,array_links,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

int arad_interrupt_handle_fmacdecerr_int_1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;   
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_decerr_int(unit, block_instance, en_arad_interrupt, FALSE, &corrective_action, special_msg, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg );
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

int arad_interrupt_handle_recurring_fmacdecerr_int_1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , ( SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_decerr_int(unit, block_instance, en_arad_interrupt, TRUE, &corrective_action, special_msg, array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg );
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,array_links,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;   
}

int arad_interrupt_handle_fmacdecerr_int_2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;   
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_decerr_int(unit, block_instance, en_arad_interrupt, FALSE, &corrective_action, special_msg, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg );
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

int arad_interrupt_handle_recurring_fmacdecerr_int_2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , ( SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_decerr_int(unit, block_instance, en_arad_interrupt, TRUE, &corrective_action, special_msg, array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg );
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,array_links,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}
int arad_interrupt_handle_fmacdecerr_int_3(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;   
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_decerr_int(unit, block_instance, en_arad_interrupt, FALSE, &corrective_action, special_msg, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg );
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

int arad_interrupt_handle_recurring_fmacdecerr_int_3(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , (SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_decerr_int(unit, block_instance, en_arad_interrupt, TRUE, &corrective_action, special_msg, array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg );
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,array_links,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmacoof_int_0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_oof_int( unit, block_instance, en_arad_interrupt, &pcs, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_fmacoof_int_0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , ( SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_recurring_mac_oof_int( unit, block_instance,en_arad_interrupt, &pcs, &corrective_action, special_msg, array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, corrective_action, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,array_links ,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_fmacoof_int_1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_oof_int( unit, block_instance, en_arad_interrupt, &pcs, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;  
}

int arad_interrupt_handle_recurring_action_fmacoof_int_1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , ( SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_recurring_mac_oof_int( unit, block_instance,en_arad_interrupt, &pcs, &corrective_action, special_msg, array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,array_links ,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;    
}

int arad_interrupt_handle_fmacoof_int_2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_oof_int( unit, block_instance, en_arad_interrupt, &pcs, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;    
}

int arad_interrupt_handle_recurring_action_fmacoof_int_2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , ( SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_recurring_mac_oof_int( unit, block_instance,en_arad_interrupt, &pcs, &corrective_action, special_msg, array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,array_links ,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;      
}

int arad_interrupt_handle_fmacoof_int_3(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_oof_int( unit, block_instance, en_arad_interrupt, &pcs, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;      
}

int arad_interrupt_handle_recurring_action_fmacoof_int_3(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action ;
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;
    soc_dcmn_port_pcs_t pcs;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    
    SOCDNX_INIT_FUNC_DEFS;

    /* initialize links array to default state - enable */
    sal_memset(array_links,ARAD_INT_LINK_INVOLVED , ( SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS) * sizeof(char)));

    /* data collection */
    rc = arad_interrupt_data_collection_for_recurring_mac_oof_int( unit, block_instance,en_arad_interrupt, &pcs, &corrective_action, special_msg, array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg,corrective_action,array_links ,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;        
}

int arad_interrupt_handle_fmacrxctrloverflow_int_0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    /* message for syslog */
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    sal_sprintf(special_msg, "Receiver control FIFO overflow. Reconfigure transmitter.");
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmacrxctrloverflow_int_1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmacrxctrloverflow_int_2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmacrxctrloverflow_int_3(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmactransmiterr_int_0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_transmit_err_int(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, corrective_action, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmactransmiterr_int_1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_transmit_err_int(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmactransmiterr_int_2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_transmit_err_int(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmactransmiterr_int_3(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    /* links array for corrective action */    
    uint8 array_links[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)] ;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_mac_transmit_err_int(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, array_links);
    BCMDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmactxfdrciffault_int_0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmactxfdrciffault_int_1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmactxfdrciffault_int_2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmactxfdrciffault_int_3(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fmacecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;    
}


int arad_interrupt_handle_recurring_action_fmacecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_fmacecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_frsdoppdataerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_frsdoppdataerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_0(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{

    int rc;
    soc_port_t link, port;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_fsrd_syncstatuschanged(unit, block_instance, en_arad_interrupt, &link, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    port = SOC_DPP_FABRIC_LINK_TO_PORT(unit, link);
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &port, NULL);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    soc_port_t link, port;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_fsrd_syncstatuschanged(unit, block_instance, en_arad_interrupt, &link, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    port = SOC_DPP_FABRIC_LINK_TO_PORT(unit, link);
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &port, NULL);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    soc_port_t link, port;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_fsrd_syncstatuschanged(unit, block_instance, en_arad_interrupt, &link, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    port = SOC_DPP_FABRIC_LINK_TO_PORT(unit, link);
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &port, NULL);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fsrd_fsrd_syncstatuschanged_3(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    soc_port_t link, port;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_fsrd_syncstatuschanged(unit, block_instance, en_arad_interrupt, &link, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    port = SOC_DPP_FABRIC_LINK_TO_PORT(unit, link);
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_RX_LOS_HANDLE, &port, NULL);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_fsrd_fsrd_txplllockchanged(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_idrecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_idrecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_idrparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_idrerrorfullmulticastrecycle(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_idrerrorminimulticastrecycle(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_idrerrorocbmulticastrecycle(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_idrerrorocbunicastrecycle(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_idrerrorunicastrecycle(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_idrmmuecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbbadchannelnum(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "Fix configuration");
                                                                        
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbbadlkptype(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "Fix configuration and reset IPP");
                                                                        
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbbadlrntype(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "Fix configuration and reset IPP");
                                                                        
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}




int arad_interrupt_handle_recurring_action_ihbecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "1bit error reccuring");


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbflpfifofull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "ELK link rate cannot sustain FLP requests rate");
                                                                        
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbflplookuptimeout(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "ELK issue or NifPhysicalError.");
                                                                        
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihblbecmplagusesamehashint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "Fix configuration");
                                                                        
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihblbvectorindexillegalcalcint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "fix LB configuration" );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbnifphysicalerr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "need to check H/W. might be issue with ELK connection." );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbnifrxfifoovf(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, " ELK alignment is wrong, meaning NOP are inserted in packet start" );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbniftxfifofull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "ELK link rate can not sustain FLP requests rate" );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihboambfdmisconfigint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "Oam-Your-Discriminator& Vtt-Lif-Valid both set,Fix configuration" );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;    
    dcmn_interrupt_mem_err_info  shadow_correct_info;    
    uint32 address;
    void *info;
    uint8 is_em_ser;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = arad_pp_em_ser_address_get(unit, shadow_correct_info.mem, shadow_correct_info.array_index, shadow_correct_info.min_index, &address, &is_em_ser);
    SOCDNX_IF_ERR_EXIT(rc); 

    if (is_em_ser) {
        /* EM SER */
        corrective_action = DCMN_INT_CORR_ACT_IHB_EM_SOFT_RECOVERY;
        info = (void*)(&address);
    }
    else {
        /* non-EM SER */
        info = (void*)(&shadow_correct_info);
    }

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, info, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_ihbrxbrokenrecord(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "packet loss in ELK and should be treated in ELK." );

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbseqrxbigerseqexpandsmallerseqtx(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "replies were lost,check NifPhysicalError/ELK" );
                                                                        
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbseqrxsmallerseqexporbiggereqseqtx(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbtcamprotectionerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    ARAD_TCAM_LOCATION location;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = dcmn_interrupt_data_collection_for_tcamprotectionerror(unit, block_instance, en_arad_interrupt, &location, &corrective_action);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, corrective_action, (void*)(&location), NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihbtcamqueryfailurevalid(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "Fix configuration");
                                                                        
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihboemaerrorcamtablefull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "defrag should be initiated.");
                                                                        
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihboemaerrortablecoherency(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "OemaErrorTableCoherencyCounter is not equal to 0");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihboemamanagementunitfailurevalid(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    soc_reg_above_64_val_t pr_reg;
    char val_str[110];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg_above_64_get(unit, IHB_OEMA_MANAGEMENT_UNIT_FAILUREr, block_instance, 0, pr_reg);
    SOCDNX_IF_ERR_EXIT(rc);  
   
                                                                            
    format_long_integer(val_str, pr_reg, SOC_REG_ABOVE_64_MAX_SIZE_U32);

    sal_sprintf(special_msg, "Check:IHB_OEMA_MANAGEMENT_UNIT_FAILURE=%s", val_str);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihboemberrorcamtablefull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
     sal_sprintf(special_msg, "defrag should be initiated.");
                                                                        
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihboemberrortablecoherency(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "OemaErrorTableCoherencyCounter is not equal to 0");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihboembmanagementunitfailurevalid(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    soc_reg_above_64_val_t pr_reg;
    char val_str[110];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg_above_64_get(unit, IHB_OEMB_MANAGEMENT_UNIT_FAILUREr, block_instance, 0, pr_reg);
    SOCDNX_IF_ERR_EXIT(rc);  
   
                                                                            
    format_long_integer(val_str, pr_reg, SOC_REG_ABOVE_64_MAX_SIZE_U32);

    sal_sprintf(special_msg, "Check:IHB_OEMB_MANAGEMENT_UNIT_FAILURE=%s", val_str);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_ihpecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;    
    dcmn_interrupt_mem_err_info  shadow_correct_info;    
    uint32 address;
    void *info;
    uint8 is_em_ser;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = arad_pp_em_ser_address_get(unit, shadow_correct_info.mem, shadow_correct_info.array_index, shadow_correct_info.min_index, &address, &is_em_ser);
    SOCDNX_IF_ERR_EXIT(rc); 

    if (is_em_ser) {
        /* EM SER */
        corrective_action = DCMN_INT_CORR_ACT_IHP_EM_SOFT_RECOVERY;
        info = (void*)(&address);
    }
    else {
        /* non-EM SER */
        info = (void*)(&shadow_correct_info);
    }

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, info, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_ihpisaerrorcamtablefull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "IsaErrorCamTableFullCounter!=0,Defrag should be initiated." );


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpisaerrortablecoherency(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "IsaErrorTableCoherencyCounter!=0" );


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpisamanagementunitfailurevalid(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    soc_reg_above_64_val_t pr_reg;
    char val_str[110];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg_above_64_get(unit, IHP_ISA_MANAGEMENT_UNIT_FAILUREr, block_instance, 0, pr_reg);
    SOCDNX_IF_ERR_EXIT(rc);  
   
                                                                            
    format_long_integer(val_str, pr_reg, SOC_REG_ABOVE_64_MAX_SIZE_U32);

    sal_sprintf(special_msg, "Check:IHP_ISA_MANAGEMENT_UNIT_FAILURE=%s", val_str);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpisberrorcamtablefull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "IsbErrorCamTableFullCounter!=0,Defrag should be initiated." );


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpisberrortablecoherency(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "IsbErrorTableCoherencyCounter!=0" );


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpisbmanagementunitfailurevalid(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    soc_reg_above_64_val_t pr_reg;
    char val_str[110];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg_above_64_get(unit, IHP_ISB_MANAGEMENT_UNIT_FAILUREr, block_instance, 0, pr_reg);
    SOCDNX_IF_ERR_EXIT(rc);  
   
                                                                            
    format_long_integer(val_str, pr_reg, SOC_REG_ABOVE_64_MAX_SIZE_U32);

    sal_sprintf(special_msg, "Check:IHP_ISB_MANAGEMENT_UNIT_FAILURE=%s", val_str);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpmacteventready(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    sal_sprintf(special_msg, "%s", "Check:IHP_MACT_EVENTr");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HANDLE_MACT_EVENT_FIFO, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HANDLE_MACT_EVENT_FIFO, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpmactlookuprequestcontention(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Link-Layer Lookup & Source/Destination Lookup both requested simult,Fix configuration");


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpmactreplyready(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    soc_reg_above_64_val_t pr_reg;
    char val_str[110];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg_above_64_get(unit, IHP_MACT_REPLYr, block_instance, 0, pr_reg);
    SOCDNX_IF_ERR_EXIT(rc);  
   
                                                                            
    format_long_integer(val_str, pr_reg, SOC_REG_ABOVE_64_MAX_SIZE_U32);

    sal_sprintf(special_msg, "Check:IHP_MACT_REPLY=%s", val_str);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpmactsrcorlllookuponwrongcycle(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Link-Layer Lookup/Source Lookup wrong cycle request,Fix configuration");


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpmacterrorcamtablefull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "MactErrorCamTableFullCounter!=0,Defrag should be initiated." );


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpmacterrortablecoherency(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "MactErrorTableCoherencyCounter!=0" );


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ihpmactmanagementunitfailurevalid(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    soc_reg_above_64_val_t pr_reg;
    char val_str[110];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg_above_64_get(unit, IHP_MACT_MANAGEMENT_UNIT_FAILUREr, block_instance, 0, pr_reg);
    SOCDNX_IF_ERR_EXIT(rc);  
   
                                                                            
    format_long_integer(val_str, pr_reg, SOC_REG_ABOVE_64_MAX_SIZE_U32);

    sal_sprintf(special_msg, "Check:IHP_MACT_MANAGEMENT_UNIT_FAILURE=%s", val_str);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_ipsdeqcommandtimeout(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "DEQ report isnt received in DeqCmdTimeout." );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ipsdqcqdepthovf(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "fix the configuration in the faulted dqcq depth registers and resume traffic" );


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ipsdqcqoverflow(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "DQCQ encountered a write to a full DQCQ." );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ipsecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_ipsecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ipsecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ipsemptydqcqwrite(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Write DQCQ with depth 0." );
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ipsenqblockoverflow(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_ipsenqblockoverflow(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ipsparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iptecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_iptecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iptecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iptparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iptcrcdeletedbuffersfifofull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;  

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iptcrcdeletedbuffersfifonotempty(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;  

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_iptcrcerrpkt(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;  

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_recurring_action_iptcrcerrpkt(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, IPT_LST_RD_DBUFFr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    sal_sprintf(special_msg, "IPT - reccuring CRC error on packets arriving from the MMU. Make ingerss soft init IPT_LST_RD_DBUFF= 0x%08x", err_cnt);
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_iqmecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "IQM reccurring ecc 1b error.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmcnmpktrjct(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "CNM packet is dropped,check CNM shaper and CNM thresholds");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmdoublecderr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "packet enqueue command with both IPT-CD & ISP-CD. IQM init is required");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmdramdynsizerollover(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];


    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "DRAM dynamic size counter overflow or under-flow");


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmenqqnumovf(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "ENQ command received pointing to Queue number above 96K-1");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmfrflmcdbrollover(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Free Full-MC Dbuffs counter rolled over(> max number/ <0).malfunction of the device, ingress soft init");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmfrmnmcdbrollover(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Free Mini-MC Dbuffs counter rolled over(> max number/ <0).malfunction of the device, ingress soft init");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmfrucdbrollover(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Free Unicast Dbuffs counter rolled over(> max number/ <0).malfunction of the device, ingress soft init");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmfreebdbovf(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Free-BDB counter has overflowed, ingress soft init");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmfreebdbunf(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Free-BDB counter has underflowed.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmfulluscntovf(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Full User count overflowed. Received over 4K multiplications for the same DB., ingress soft init");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmingmc2isperr(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "ingress replication packet pointed and ISP Queue");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmminiuscntovf(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Mini User count overflowed. Received over 4 multiplications for the same DB.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmqrollover(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "One of the packet queues Q-size rolled over (overflowed or underflowed), ingress soft init");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_iqmvsqrollover(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "One of the virtual queues Q-size rolled over (overflowed or underflowed), ingress soft init");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_INGRESS_HARD_RESET, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ireecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ireecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ireparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ireerrorbadreassemblycontext(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, " packet arrived from cfg-bad-reassembly-context");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ireregierrdataarrived(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Reg interface transmitted data and the error signal was set.");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ireregierrpacketsize(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Reg interface transmitted packet smaller than 32 btyes.");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_ireregierr_64bytespack(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, " This error might be a change of port or channel in the middle of a packet or a start of packet without an end of packet.");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_irrecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_irrecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_irrparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_irrerrismaxreplication(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " ingress multicast packet is replicated more than a configurable value.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_irrerrisreplicationempty(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "an ingress multicast group is empty -  check the MC DB");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_irrerrmaxreplication(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " ingress multicast packet is replicated more than a configurable value.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_irrerrreplicationempty(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "an ingress multicast group is empty - check the MC DB.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_irrerrorresequencer0outofseq(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_irrerrorresequencer0outofsync(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_irrerrorresequencer1outofseq(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_irrerrorresequencer1outofsync(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_irrerrorresequencer2outofseq(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_irrerrorresequencer2outofsync(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_mmuecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_mmuecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_mmuecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_mmuecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);
 
exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_mmudramoppcrcerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_mmudramoppcrcerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);
 
exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_mmuocboppcrcerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_mmuocboppcrcerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);
 
exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_nbiecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbi_link_status_change(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_PRINT_MSG_SIZE];   
    soc_port_t port;
    dcmn_int_corr_act_type corrective_action;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_nbi_link_status_change(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &port);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */   
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg , corrective_action, (void*)(&port), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_ilkn_rx_status_change(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
	int rc;
    char special_msg[ARAD_INTERRUPT_PRINT_MSG_SIZE];   
    soc_port_t port;
	soc_port_if_t interface_type;
	dcmn_int_corr_act_type corrective_action;
	SOCDNX_INIT_FUNC_DEFS;

	/* data collection */
	rc = arad_interrupt_data_collection_for_ilkn_rx_status_change(unit,block_instance,en_arad_interrupt,special_msg, &corrective_action, &port);
	SOCDNX_IF_ERR_EXIT(rc);

	SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit,port,&interface_type));

	if (interface_type !=  _SHR_PORT_IF_ILKN)
	{	
		SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOC_MSG("port isn't ilkn"))); 
	}

	/* print info */
	rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
	SOCDNX_IF_ERR_EXIT(rc);

	/* corrective action */
	rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&port), NULL);
	SOCDNX_IF_ERR_EXIT(rc);

exit:
	SOCDNX_FUNC_RETURN;


}

int arad_interrupt_handle_nbiecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilkncrc32laneerrindicationlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "ndicates that CRC32 in received Diagnostic Word for lanes 0-11 is not as expected.Verify configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilkncrc32laneerrindicationlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Indicates CRC32 received Diagnostic Word for lanes 12-23 not as expected.Verify the configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}
int arad_interrupt_handle_nbiilknlanebadframingtypeerrlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_recurring_action_nbiilknlanebadframingtypeerrlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Illegal framing layer block type was detected.Verify the configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknlanebadframingtypeerrlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_recurring_action_nbiilknlanebadframingtypeerrlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Illegal framing layer block type was detected.Verify the configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknlaneframingerrlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}
int arad_interrupt_handle_recurring_action_nbiilknlaneframingerrlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Illegal framing pattern,verify configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);
    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknlaneframingerrlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_nbiilknlaneframingerrlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Illegal framing pattern,verify configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);
    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknlanemetaframelengtherrlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_nbiilknlanemetaframelengtherrlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Length of MetaFrame received inrespective lane is different from expected.Verify the configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknlanemetaframelengtherrlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_nbiilknlanemetaframelengtherrlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Length of MetaFrame received inrespective lane is different from expected.Verify the configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}
int arad_interrupt_handle_nbiilknlanemetaframerepeaterrlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}
int arad_interrupt_handle_recurring_action_nbiilknlanemetaframerepeaterrlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "8 MetaFrame Sync Word error/3 Scrambler State Control Word error.Verify the configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknlanemetaframerepeaterrlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_nbiilknlanemetaframerepeaterrlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "8 MetaFrame Sync Word error/3 Scrambler State Control Word error.Verify the configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknlanemetaframesyncworderrlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_recurring_action_nbiilknlanemetaframesyncworderrlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Meta Frame Synchronization Word error.Verify the configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknlanemetaframesyncworderrlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_nbiilknlanemetaframesyncworderrlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Meta Frame Synchronization Word error.Verify the configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknlanescramblerstateerrlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_nbiilknlanescramblerstateerrlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Scrambler State Control Word error.Verify the configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknlanescramblerstateerrlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}



int arad_interrupt_handle_recurring_action_nbiilknlanescramblerstateerrlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Scrambler State Control Word error. Verify the configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknlanesyncherrlanes0_11int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "64B/67B Word Boundary Lock wasnt achieved/Errors detected on Framing bits65:64 after sync/Meta Frame Sync never achieved. Verify configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknlanesyncherrlanes12_23int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "64B/67B Word Boundary Lock wasnt achieved/Errors detected on Framing bits65:64 after sync/Meta Frame Sync never achieved. Verify configuration of peer ILKN device");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);
    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknrx0retranserrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Rx retransmit logic reached an error state.Retransmission sequence couldnt complete.if not set to automatically reset retransmit logic need to reset is by SW");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknrx1retranserrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Rx retransmit logic reached an error state.Retransmission sequence couldnt complete.if not set to automatically reset retransmit logic need to reset is by SW");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);
    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknrxparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknrxport0alignerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Indicates the lane alignment failed after several attempts/lane alignment lost.Verify ILKN device peer configuration");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknrxport0statuserrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "One of the errors in RxIlknStatus0 has occurred");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_nbiilknrxport0statuserrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Error in RxIlknStatus0.");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_nbiilknrxport1alignerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Indicates lane alignment failed after several attempts/lane alignment lost.Verify peer ILKN device configuration.");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilknrxport1statuserrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Error in RxIlknStatus1.");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_recurring_action_nbiilknrxport1statuserrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Error in RxIlknStatus1.");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilkntxparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilkntxport0statuserrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt0, err_field_cnt1, err_field_cnt2, err_field_cnt3;
    dcmn_int_corr_act_type corr_act;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, NBI_TX_ILKN_STATUSr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt0 = soc_reg_field_get(unit, NBI_TX_ILKN_STATUSr, err_cnt, TX_N_STAT_UNDERFLOW_ERRf);
    err_field_cnt1 = soc_reg_field_get(unit, NBI_TX_ILKN_STATUSr, err_cnt, TX_N_STAT_BURST_ERRf);
    err_field_cnt2 = soc_reg_field_get(unit, NBI_TX_ILKN_STATUSr, err_cnt, TX_N_STAT_OVERFLOW_ERRf);
    err_field_cnt3 = soc_reg_field_get(unit, NBI_TX_ILKN_STATUSr, err_cnt, TX_N_OVFOUTf);     


    if (err_field_cnt0 + err_field_cnt1 + err_field_cnt2 + err_field_cnt3 > 0 ) {
        corr_act = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;
        sal_sprintf(special_msg, "Unexpected error in ILKN0 Tx.  Hard reset");
    } else {
        corr_act = DCMN_INT_CORR_ACT_NONE;
        sal_sprintf(special_msg, "error in ILKN0 Tx.");
    }

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corr_act, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corr_act, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_handle_nbiilkntxport1statuserrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, err_field_cnt0, err_field_cnt1, err_field_cnt2, err_field_cnt3;
    dcmn_int_corr_act_type corr_act;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, NBI_TX_ILKN_STATUSr, block_instance, 1, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt0 = soc_reg_field_get(unit, NBI_TX_ILKN_STATUSr, err_cnt, TX_N_STAT_UNDERFLOW_ERRf);
    err_field_cnt1 = soc_reg_field_get(unit, NBI_TX_ILKN_STATUSr, err_cnt, TX_N_STAT_BURST_ERRf);
    err_field_cnt2 = soc_reg_field_get(unit, NBI_TX_ILKN_STATUSr, err_cnt, TX_N_STAT_OVERFLOW_ERRf);
    err_field_cnt3 = soc_reg_field_get(unit, NBI_TX_ILKN_STATUSr, err_cnt, TX_N_OVFOUTf); 
 
    if (err_field_cnt0 + err_field_cnt1 + err_field_cnt2 + err_field_cnt3 > 0 ) {
        corr_act = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;
        sal_sprintf(special_msg, "Unexpected error in ILKN1 Tx.  Hard reset");
    } else {
        corr_act = DCMN_INT_CORR_ACT_NONE;
        sal_sprintf(special_msg, "error in ILKN1 Tx.");
    }

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corr_act, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corr_act, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbierrbitsfromegqint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_recurring_action_nbierrbitsfromegqint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 1, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilegalilkninputfromrx0int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Illegal ILKN-24 input on Rx, Check Ilkn configuration");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilegalilkninputfromrx1int(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Illegal ILKN-12 input on Rx, Check Ilkn configuration");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiilkntx1_32byte_consecutiveclksint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint64 pr_reg, pr_field, val64;
    dcmn_int_corr_act_type corr_act;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg64_get(unit, NBI_HRF_TX_CONTROLLER_CONFIGr, block_instance, 0, &pr_reg);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    pr_field = soc_reg64_field_get(unit,NBI_HRF_TX_CONTROLLER_CONFIGr, pr_reg, HRF_TX_N__512B_IN_2_CLKSf);
    COMPILER_64_SET(val64, 0, 1);
    if (COMPILER_64_EQ(pr_field, val64)) {
        corr_act = DCMN_INT_CORR_ACT_NONE;
        sal_sprintf(special_msg, "ILKN-12 wrapper Tx buffer overflow,HrfTx<1>_512bIn_2clks =1");
    } else {
        corr_act = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;
        sal_sprintf(special_msg, "ILKN-12 wrapper Tx buffer overflow,HrfTx<1>_512bIn_2clks is 0, hard reset");
    }
  

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corr_act, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corr_act, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbirxelkovfint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "ELK RX buffer reached overflow.Check Elk engine configuration. External ELK should not send reply at such high rate");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbirxfifooverflowint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    uint32 reg_val;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
 
   /* reading of the register is operation required  before the clear action */
    rc = soc_reg32_get(unit, NBI_RX_MLF_STATUSr, block_instance, 0, &reg_val);
    SOCDNX_IF_ERR_EXIT(rc); 

     sal_sprintf(special_msg, "NBI_RX_MLF_STATUS = %u", reg_val);
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */

    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbitxfifooverflowint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Tx FIFO Overflow,EGQ max entries config doesnt match credits number sent from NIF,Check also HrfTx<n>NumCreditsToEgq");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiwrongegqwordint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Illegal BE value recived from EGQ.Check EGQ configuration");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);
    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiwrongportfromegqint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "EGQ transmitted data to an invalid port,Check EGQ configuration");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbiwrongwordfrommalint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "Received an illegal word from the Uniport Rx.Check Uniport configuration");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbistatreaderrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "StatReadErrInt:Statistics read error interrupt,Clear the SW error and reinitiate access to Stat counters");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_nbistattxburstlengthoverflowint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
   
    sal_sprintf(special_msg, "A packet larger than 16384B on EGQ2NIF interface,Verify size of packets send by EGQ is smaller than 16K");
 
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_oampecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_oampecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_oampparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;    
    dcmn_interrupt_mem_err_info  shadow_correct_info;    
    uint32 address;
    void *info;
    uint8 is_em_ser;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = arad_pp_em_ser_address_get(unit, shadow_correct_info.mem, shadow_correct_info.array_index, shadow_correct_info.min_index, &address, &is_em_ser);
    SOCDNX_IF_ERR_EXIT(rc); 

    if (is_em_ser) {
        /* EM SER */
        corrective_action = DCMN_INT_CORR_ACT_OAMP_EM_SOFT_RECOVERY;
        info = (void*)(&address);
    }
    else {
        /* non-EM SER */
        info = (void*)(&shadow_correct_info);
    }
    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, info, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_oamppendingevent(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_oamprmapemerrorcamtablefull(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_oamprmapemerrordeleteunknownkey(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Last delete action from the CPU is not valid.");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_oamprmapemerrorreachedmaxentrylimit(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Driver should not add new keys to the table.");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_oamprmapemerrortablecoherency(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_oamprmapemmanagementunitfailurevalid(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_oamprmapemwarninginsertedexisting(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
     int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, "Driver tried to add an existing key.");
 

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_olpecc_2berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_olpecc_1berrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_olpparityerrint(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_port_mib_tsc0_mem_err(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action;      

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_port_mib(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_port_mib_tsc1_mem_err(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action;      

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_port_mib(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_port_mib_rsc0_mem_err(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action;      

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_port_mib(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_port_mib_rsc1_mem_err(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action;      

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = arad_interrupt_data_collection_for_port_mib(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_rtplinkmaskchange(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, DCMN_INT_CORR_ACT_RTP_LINK_MASK_CHANGE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schdcdeccerrorfixed(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schdhdeccerrorfixed(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schfdmdeccerrorfixed(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schflhcleccerrorfixed(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schflhfqeccerrorfixed(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schflhhreccerrorfixed(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schfltcleccerrorfixed(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schfltfqeccerrorfixed(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schflthreccerrorfixed(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schfsmeccerrorfixed(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schshddeccerrorfixed(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];   
    dcmn_interrupt_mem_err_info  ecc_1b_correct_info;
    dcmn_int_corr_act_type corrective_action ;
    SOCDNX_INIT_FUNC_DEFS;


    /* data collection */
    rc = arad_interrupt_data_collection_for_ecc_1b(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &ecc_1b_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&ecc_1b_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schdcdeccerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schdhdeccerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schfdmdeccerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schflhcleccerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schflhfqeccerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schflhhreccerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schfltcleccerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schfltfqeccerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schflthreccerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schfsmeccerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schshddeccerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schactflow_badparams(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, SCH_ATTEMPT_TO_ACTIVATE_FLOW___SCHEDULER_WITH_BAD_PARAMETERSr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  

    sal_sprintf(special_msg, " configuration error in the scheduler flow. bad_params=%u, bad_sch=%u, cosn_valid=%u, flow_id=%d",
                soc_reg_field_get(unit, SCH_ATTEMPT_TO_ACTIVATE_FLOW___SCHEDULER_WITH_BAD_PARAMETERSr, err_cnt, ACT_FLOW_BAD_PARAMSf),
                soc_reg_field_get(unit, SCH_ATTEMPT_TO_ACTIVATE_FLOW___SCHEDULER_WITH_BAD_PARAMETERSr, err_cnt, ACT_FLOW_BAD_SCHf),
                soc_reg_field_get(unit, SCH_ATTEMPT_TO_ACTIVATE_FLOW___SCHEDULER_WITH_BAD_PARAMETERSr, err_cnt, ACT_FLOW_COSN_VALIDf),
                soc_reg_field_get(unit, SCH_ATTEMPT_TO_ACTIVATE_FLOW___SCHEDULER_WITH_BAD_PARAMETERSr, err_cnt, ACT_FLOW_IDf));

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schrestartflowevent(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, SCH_LAST_FLOW_RESTART_EVENTr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);

    sal_sprintf(special_msg, " If user didn't changes flow configuration on the fly, it indicates something bad going in the databases. flow restart: event=%u, ID=%u",
                soc_reg_field_get(unit, SCH_LAST_FLOW_RESTART_EVENTr, err_cnt, RESTART_FLOW_EVENTf),
                soc_reg_field_get(unit, SCH_LAST_FLOW_RESTART_EVENTr, err_cnt, RESTART_FLOW_IDf));

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schsmpfulllevel1(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " SMP FIFO in pipe reached level1.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schsmpfulllevel2(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    sal_sprintf(special_msg, " SMP FIFO in pipe reached level2.");

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schshpflow_badparams(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERSr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  

    sal_sprintf(special_msg, " shaper configuration error. flow_bad_param=%u, flow_id=%u",
                    soc_reg_field_get(unit, SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERSr, err_cnt, SHP_FLOW_BAD_PARAMSf),
                    soc_reg_field_get(unit, SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERSr, err_cnt, SHP_FLOW_IDf));

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schsmpbadmsg(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 err_cnt, msg_reg1, msg_reg2;

    SOCDNX_INIT_FUNC_DEFS;

    /* data collection */
    rc = soc_reg32_get(unit, SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATIONr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  

    if(err_cnt & 0x10000000) { /* varify that bit 28 is 1 */
         rc = soc_reg32_get(unit, SCH_INCORRECT_STATUS_MESSAGE___REGISTER_1r, block_instance, 0, &msg_reg1);
         SOCDNX_IF_ERR_EXIT(rc); 
         rc = soc_reg32_get(unit, SCH_INCORRECT_STATUS_MESSAGE___REGISTER_2r, block_instance, 0, &msg_reg2);
         SOCDNX_IF_ERR_EXIT(rc); 

         sal_sprintf(special_msg, " bad message was sent to the scheduler: bad_val=%u, flow=%u, port_3_0=%u, type=%u, val=%u, fap=%u, queue=%u",
                     soc_reg_field_get(unit, SCH_INCORRECT_STATUS_MESSAGE___REGISTER_1r, msg_reg1, SMP_MSG_BAD_VALf),
                     soc_reg_field_get(unit, SCH_INCORRECT_STATUS_MESSAGE___REGISTER_1r, msg_reg1, SMP_MSG_FLOWf), 
                     soc_reg_field_get(unit, SCH_INCORRECT_STATUS_MESSAGE___REGISTER_1r, msg_reg1, SMP_MSG_PORT_3_0f), 
                     soc_reg_field_get(unit, SCH_INCORRECT_STATUS_MESSAGE___REGISTER_1r, msg_reg1, SMP_MSG_TYPEf), 
                     soc_reg_field_get(unit, SCH_INCORRECT_STATUS_MESSAGE___REGISTER_1r, msg_reg1, SMP_MSG_VALf), 
                     soc_reg_field_get(unit, SCH_INCORRECT_STATUS_MESSAGE___REGISTER_2r, msg_reg2, SMP_MSG_FAPf), 
                     soc_reg_field_get(unit, SCH_INCORRECT_STATUS_MESSAGE___REGISTER_2r, msg_reg2, SMP_MSG_QUEf));
    }else {
        sal_sprintf(special_msg, " bad message was sent to the scheduler, unable to get the details");
    }


    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_NONE, special_msg);
    BCMDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schcal0parerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schcal1parerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schcal2parerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schcal3parerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schcal4parerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schcsstparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schdpnparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schdrmparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schdsmparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_handle_schfdmsparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schffmparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schfgmparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schfimparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schfqmparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schfsfparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{       
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schpsstparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{ 
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schcsdtparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{ 
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schpsdtparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{ 
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schpsddparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{ 
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schtmcparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{ 
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schpswparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schsccparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schsctparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schsemparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schshcparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schshdsparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handle_schsimparerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    char special_msg[ARAD_INTERRUPT_SPECIAL_MSG_SIZE];
    dcmn_int_corr_act_type corrective_action ;        
    dcmn_interrupt_mem_err_info  shadow_correct_info;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;

    /* data collection */
    rc = arad_interrupt_data_collection_for_shadowing(unit, block_instance, en_arad_interrupt, special_msg, &corrective_action, &shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);

    /* print info */
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    /* corrective action */
    rc = arad_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, special_msg, corrective_action, (void*)(&shadow_correct_info), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME
