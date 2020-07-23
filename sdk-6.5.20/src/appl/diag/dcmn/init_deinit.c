/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        init_deinit.c
 * Purpose:     DCMN initialization sequence.
 */

/* 
 * Includes
 */ 
#include <shared/bsl.h>

#include <ibde.h>
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#if !defined(__KERNEL__) && defined (LINUX)
    #include <stdlib.h>
#endif
#endif
#include <soc/cmext.h>
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/port_sw_db.h>
#endif /* BCM_PETRA_SUPPORT */

#include <soc/drv.h>
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <soc/dcmn/dcmn_wb.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#ifdef CRASH_RECOVERY_SUPPORT
#include <shared/swstate/access/sw_state_access.h>
#endif

#include <bcm/init.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm/port.h>
#include <bcm/stack.h>
#include <bcm/switch.h>
#include <bcm/types.h>

#include <appl/diag/diag.h>

#include <appl/diag/system.h>

#include <appl/diag/sysconf.h>

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <appl/diag/dcmn/init_deinit.h>
#include <appl/diag/dcmn/init.h>
#include <appl/diag/dcmn/rx.h>
#include <appl/diag/dcmn/counter.h>
#include <appl/dcmn/interrupts/interrupt_handler.h>
#include <appl/dcmn/interrupts/dcmn_intr.h>
#include <appl/dcmn/rx_los/rx_los.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#if defined(BCM_WARM_BOOT_SUPPORT)
#include <shared/swstate/sw_state.h>
#endif /* defined(BCM_WARM_BOOT_SUPPORT)) */

#ifdef BCM_PETRA_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif

#ifdef CRASH_RECOVERY_SUPPORT
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <soc/dcmn/dcmn_crash_recovery.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */
#include <shared/swstate/sw_state_journal.h>
#endif

uint32 previous_init_was_warmboot[SOC_MAX_NUM_DEVICES] = {0};

#ifdef CRASH_RECOVERY_SUPPORT
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
extern soc_dcmn_cr_t *dcmn_cr_info[BCM_MAX_NUM_UNITS];
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */
#endif

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <appl/diag/dpp/kbp.h>
#endif /* defined(INCLUDE_KBP) */

#ifndef AGGRESSIVE_ALLOC_DEBUG_TESTING
  #define AGGRESSIVE_ALLOC_DEBUG_TESTING 0
#endif

#define SAVE_RV_IF_NOT_FAIL(_func_rv, _rv) \
    if (_func_rv == BCM_E_NONE) { \
        _func_rv = _rv; \
    } \

#define BCM_EXIT_IF_FAILURE(rv, message) \
do { \
    if (BCM_FAILURE(rv)) { \
        LOG_ERROR(BSL_LS_APPL_SHELL,(BSL_META_U(unit, message))); \
        goto exit ; \
    } \
} while(0)

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
char appl_dcmn_init_usage[] = 
#ifdef COMPILER_STRING_CONST_LIMIT
"Deinit init Test Usage:\n"
"  Repeat=<value>, NoInit=<value>, NoDeinit=<value>, NoBcm=<value>, NoSoc=<value>.\n"
"  NoInt=<value>, NoRxLos=<value>, NoAppl=<value>         .\n"
#if defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP)
"  NoElk=<value>.\n"
#endif /* defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) */
"  1: The test will not run.\n"
#else
"application Init/Deinit Usage:\n"
"  NoInit=<value>         1: Init sequence will not be performed on the chip. (default=0)\n"
"  NoDeinit=<value>       1: Deinit sequence will not be performed on the chip. (default=1) at init sequence, (default=0) at test mode.\n"
"  NoBcm=<value>          1: BCM sequence will not be performed on the chip. (default=0)\n"
"  NoSoc=<value>          1: SOC sequence will not be performed on the chip. (default=0)\n"
"  NoAttach=<value>       1: Attach/Deattach sequence will not be performed on the chip. (default=1) at init sequence, (default=0) at test mode.\n"
"  NoInt=<value>          1: Interrupt application will not be performed on the chip. (default=0)\n"
"  NoRxLos=<value>        1: Rx Los application will not be performed on the chip. (default=0)\n"
"  NoLinkscan=<value>     1: Linkscan sequence will not be performed on the chip.(default=0)\n"
"  CosqDisable=<value>    1: Cosq application will not be performed on the chip. (default=0)\n"
"  Flags=<value>          Flags for init/deinit sequence. (default=0)\n"
"  ModID=<value>          Module id for attach. (default=0) at init sequence, (default=previous value) at test mode.\n"
"                         BaseModID should be added when BaseModID is not 0.\n"
"  BaseModID=<value>      Base module id. (default=0) at init sequence, (default=previous value) at test mode. Should be added to ModID.\n\n"
#if defined(BCM_WARM_BOOT_SUPPORT)
"  Warmboot=<0/1>         0: Init/Deinit sequence will be performed (default).\n"
"                         1: Warmboot sequence will be performed.\n\n"
#endif /* #if defined(BCM_WARM_BOOT_SUPPORT)*/
#if defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP)
"  NoElkDevice=<value>    1: ELK init sequence will not be performed on the chip. (default=0)\n"
"  NoElkSecDevice=<value> 1: ELK init sequence will not be performed on the chip for the second ELK device. (default=0)\n"
"  NoElkAppl=<value>      1: ELK application will not be performed on the chip. (default=0)\n"
"  ElkIlknRev=<value>     ELK revision number. (default=1)\n"
"  ElkMdioId=<value>      ELK MDIO id number. (default for ARAD=0x101, default for JERICHO=0x161) \n\n"
"  ElkMdioSecId=<value>   ELK MDIO second KBP id number. (default for JERICHO=0x141)\n\n"
#endif /* defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) */
"  NoAppl=<value>         1: Diag application will not be performed on the chip. (default=0)\n"
"  NoApplStk=<value>      1: STK application will not be performed on the chip. (default=0)\n"
"  NoItmhProgMode=<value> 1: ITMH programmable mode application will not be performed on the chip. (default=0)\n"
"  ApplFlags=<value>      Flags for application initialization. (default=0)\n"
"  ApplTrEnStage=<value>  The stage in which traffic enable sequence will be performed. 0 - disable , (default=0x1)- before application init.\n\n"
"  NoPktRx=<value>        1: Packet Rx sequence will not be performed on the chip. (default=0)\n"
"  PktRxCosq=<value>      Cosq parameter for Packet Rx sequence. (default=0)\n\n"
#if defined(BCM_GEN_ERR_MECHANISM)
"  PartialInit=<value>    (test mode only)"
"                         0: the test will run Full test.(default)\n"
"                         1: the test will run Partial init.\n"
"                         2: the test will run Partial deinit.\n"
"                         3: the test will run Partial init and deinit.\n"
"  FakeBcmStart=<value>   (test mode only) fake error start during BCM init sequence.\n"
"  FakeBcmRange=<value>   (test mode only) fake error range during BCM init sequence.\n"
"  FakeSocStart=<value>   (test mode only) fake error start during Soc init sequence.\n"
"  FakeSocRange=<value>   (test mode only) fake error range during Soc init sequence.\n"
#endif
"  Repeat=<value>         (test mode only) the test will loop as many times as stated. (default=1)\n"
"  RcLoad=<value>         (test mode only) 1: test will load the rc file instead of Init/Deinit sequence. The default value is 0.\n"
"  ErrorOnLeak=<0/1>      if 1 failed the test if NumallocatedBytes>NumFreeBytes when running the tr\n"
#if AGGRESSIVE_ALLOC_DEBUG_TESTING
"  ResoucesLeakCheck=<0/1>       (test mode only) Test to verify that all the allocation done free. (default=0)\n"
"                                Test sequence: deinit, sample memory allocations, init, deinit, sample memory allocation.\n"
"                                If allocated memory is different after the two de-init's - print the extra allocations to file.\n"
"  AggressiveNumToTest=<0/value> (test mode only) Number of allocation entries to scan each time a memory allocation is required\n"
"                                (default=0 meaning that the number would be AGGRESSIVE_NUM_TO_TEST). See ResoucesLeakCheck.\n"
"  AggressiveKeepOrder=<0/1>     (test mode only) Control on whether to keep the order of allocations (in time) on global allocations\n"
"                                array (Allocs). (default=AGGRESSIVE_ALLOC_DEBUG_TESTING_KEEP_ORDER). 0 means 'no order is kept'.\n"
"                                If non-zero then, when 'free' is applied, the corresponding entry in Allocs is deleted and all following\n"
"                                entries are pushed 'down'. See ResoucesLeakCheck.\n"
"  AggressiveLongFreeSearch=<0/1> (test mode only) Control on whether, on 'free', to keep searching Allocs[] after the first match\n"
"                                 (default=AGGRESSIVE_LONG_FREE_SEARCH). 0 means 'stop searching after first match'.\n"
#endif
#endif
;
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#if defined(BCM_PETRA_SUPPORT)
STATIC int appl_dcmn_appl_init(int unit, appl_dcmn_init_param_t* init_param)
{
    int rv = BCM_E_NONE;
    int device;

    if (!init_param->warmboot) {
        rv = bcm_stk_modid_set(unit, init_param->modid);
        if (BCM_FAILURE(rv)) { 
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "bcm_stk_modid_set:\n")));
            return rv;
        }
    } 

    if (init_param->nof_devices == -1) {
        init_param->nof_devices = 0;
        /* Get number of devices*/
        for (device = 0; device < SOC_MAX_NUM_DEVICES; device++) {
            if (bcm_unit_valid(device) && !SOC_IS_DFE(device)) {
                init_param->nof_devices++;
            }
        }
    }

    rv = appl_dpp_bcm_diag_init(unit, init_param); 
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "appl_dpp_bcm_diag_init () Failed:\n"))); 
    } 
    return rv; 
}

/*
* Function:   init_deinit_test_linkscan_init
* Purpose:    initialize linkscan in init_deinit_test
* Parameters: unit
* Returns:    0
*/
STATIC int appl_dcmn_linkscan_init(int unit, int warm_boot){
    int rv = BCM_E_NONE; 
    int pbmp_port;
    bcm_pbmp_t pbmp;
    uint32 linkscan_interval, sw_db_flags;
    bcm_port_config_t pcfg;
    /* in warmboot will be init by the BCM */
    if (warm_boot) {
        int interval;
        rv = bcm_linkscan_enable_get(unit, &interval);
        if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "read Linkscan interval Failed:\n")));
            return rv;
        }
        rv = bcm_linkscan_enable_set(unit, interval);
        if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "Init Linkscan Failed:\n")));
            return rv;
        }
    } else {
        /* Turn linkscan on */
        linkscan_interval = soc_property_get(unit, spn_BCM_LINKSCAN_INTERVAL,
                                             BCM_LINKSCAN_INTERVAL_DEFAULT);
        rv = bcm_linkscan_enable_set(unit, linkscan_interval);
        if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "bcm_linkscan_enable_set() Failed:\n")));
           return rv;
        }

        rv = bcm_port_config_get(unit, &pcfg);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "bcm ports not initialized\n")));
            return rv;
        }
        /* Adding ports ge,xe,xl,il,e to linkscan */
 
        BCM_PBMP_CLEAR(pbmp); 
        BCM_PBMP_OR(pbmp, pcfg.ge);
        BCM_PBMP_OR(pbmp, pcfg.xe);
        BCM_PBMP_OR(pbmp, pcfg.xl);
        BCM_PBMP_OR(pbmp, pcfg.il);
        BCM_PBMP_OR(pbmp, pcfg.e);
        BCM_PBMP_OR(pbmp, pcfg.ce);
        BCM_PBMP_AND(pbmp, pcfg.port);

        BCM_PBMP_ITER(pbmp,pbmp_port){
            if(!BCM_PBMP_MEMBER(pcfg.sfi, pbmp_port)) {
                rv = soc_port_sw_db_flags_get(unit, pbmp_port, &sw_db_flags);
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "Could not get flags port %s info: %s\n"),
                                          BCM_PORT_NAME(unit, pbmp_port), bcm_errmsg(rv)));
                    return rv;
                }
                if(SOC_PORT_IS_STAT_INTERFACE(sw_db_flags)) {
                    BCM_PBMP_PORT_REMOVE(pbmp, pbmp_port);
                }
            }
        }

        rv = bcm_linkscan_mode_set_pbm(unit, pbmp, BCM_LINKSCAN_MODE_SW); 
        if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "bcm_linkscan_mode_set_pbm() Failed:\n")));
           return rv;
        }
    }
    return BCM_E_NONE;
}
#endif /* defined(BCM_PETRA_SUPPORT) */

#if defined(BCM_DFE_SUPPORT)
STATIC int appl_dcmn_dfe_stk_init(int unit,int modid)
{
    int rv = BCM_E_NONE;

    rv = bcm_stk_modid_set(unit, modid); 
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "bcm_stk_modid_get:\n"))); 
        return rv; 
    } 
    rv = bcm_stk_module_enable(unit, modid, -1, 0x1); 
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "bcm_stk_module_enable:\n")));
    }
    return rv; 
}
#endif /* defined(BCM_DFE_SUPPORT) */ 


/*
 * Function:   appl_dcmn_init
 * Purpose:    Init sequencse for DNX
 * Parameters:    u - unit #.
 *        partial_init - partial init.
 *        init_param - ignored cookie.
 * Returns:    0
 */
int
appl_dcmn_init(int unit, appl_dcmn_init_param_t* init_param) {

    int rv = BCM_E_NONE;
#ifdef BCM_PETRA_SUPPORT
    int disable_rx_appl;
#endif /* BCM_PETRA_SUPPORT */
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    ARAD_INIT_ELK *elk=NULL;
#endif
    
    /* check input parameters */
    if ( ( init_param->no_init || init_param->no_deinit) && (init_param->no_attach == FALSE) ) { 
        cli_out("Error: Init can not be called with attach without doing detach before. NoAttach forced to 1!!\n"); 
        init_param->no_attach = 1;
    }
     
    if(init_param->no_init) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "appl_dcmn_init should not be called with no_init parameter set\n"))); 
        rv = BCM_E_PARAM; 
        goto exit ;
    }
             
    /* turn on warmboot flag */
    if (init_param->warmboot) {
        SOC_WARM_BOOT_START(unit);
    }
    /* if warmboot is already on, turn on init_param->warmboot */
    if (SOC_WARM_BOOT(unit)) {
        init_param->warmboot = TRUE;
    }

    /* attach unit */
    if(!init_param->no_attach) {
        const ibde_dev_t *dev = bde->get_dev(unit); /* assumes unit==bde_dev */
        uint16 devid = dev->device; 
        uint8 revid = dev->rev; 

    /* call to sysconf_chip_override cause kernel build error */
#ifndef __KERNEL__
       sysconf_chip_override(unit, &devid, &revid); 
#endif

        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "%d: Attach unit.\n"), unit));
        rv = soc_cm_device_create(devid, revid, NULL);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "soc_cm_device_create () for devid %u Failed:\n"), devid));
            goto exit ;                
        }
        rv = sysconf_attach(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "sysconf_attach () Failed:\n")));
            goto exit ; 
        }
    }         

    /* init SOC */
    if(!init_param->no_soc) {
        DISPLAY_MEM ;
#if defined(BCM_WARM_BOOT_SUPPORT)
        DISPLAY_SW_STATE_MEM ;
#endif /* BCM_WARM_BOOT_SUPPORT */
        DISPLAY_MEM_PRINTF(("%s(), Going to init SOC: unit %d\r\n",__func__,unit)) ;

        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "%d: Init SOC.\n"), unit));
        rv = soc_reset_init(unit);
        if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "soc_reset_init () Failed:\n")));
           goto exit ; 
        }
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "%d: Init SOC Done.\n"), unit));
    }

    /* KBP application init for warmboot */
#if defined(BCM_WARM_BOOT_SUPPORT)
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        /* init KBP application */
        if (SOC_DPP_IS_ELK_ENABLE(unit)) {
            if(init_param->warmboot) {

                DISPLAY_MEM ;
                DISPLAY_SW_STATE_MEM ;
                /*
                 * Prepare initializing KBP, including global resources
                 * allocation, callback register, device config, etc.
                 */
                if (SOC_IS_ARAD(unit) && (!init_param->no_elk_device)) {
                    rv = dpp_kbp_init_prepare(unit, 0, init_param->elk_mdio_id, init_param->elk_ilkn_rev);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_APPL_SHELL,
                                  (BSL_META_U(unit,
                                              "Failed to prepare KBP initialization data for core0. \n")));
                        goto exit;
                    }
                }
                if (SOC_IS_JERICHO(unit) && (!init_param->no_elk_second_device)) {
                    rv = dpp_kbp_init_prepare(unit, 1, init_param->elk_mdio_second_id, init_param->elk_ilkn_rev);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_APPL_SHELL,
                                  (BSL_META_U(unit,
                                              "Failed to prepare KBP initialization data for core1. \n")));
                        goto exit;
                    }
                }
                /*
                 * Init KBP XPT
                 */
                LOG_INFO(BSL_LS_BCM_INIT,
                         (BSL_META_U(unit,
                                     "%d: Init KBP Transport.\n"), unit));
                if (SOC_IS_ARAD(unit) && (!init_param->no_elk_device)) {
                    rv = dpp_kbp_init_kbp_transport(unit, 0);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_APPL_SHELL,
                                  (BSL_META_U(unit,
                                              "KBP transport init core 0 Failed:\n")));
                        goto exit; 
                    }
                }
                if (SOC_IS_JERICHO(unit) && (!init_param->no_elk_second_device)) {
                    rv = dpp_kbp_init_kbp_transport(unit, 1);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_APPL_SHELL,
                                  (BSL_META_U(unit,
                                              "KBP transport init core 1 Failed:\n")));
                        goto exit; 
                    }
                }
                if (!init_param->no_elk_appl) {
                    LOG_INFO(BSL_LS_BCM_INIT,
                             (BSL_META_U(unit,
                                         "%d: Init KBP Appl.\n"), unit));
                    rv = dpp_kbp_init_appl(unit, (SOC_IS_JERICHO(unit) && (!init_param->no_elk_second_device)), elk);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_APPL_SHELL,
                                  (BSL_META_U(unit,
                                              "init_kbp_app () Failed:\n"))); 
                        goto exit ; 
                    }
                }
            }
        }
        /* init KAPS application */
#if defined(BCM_JERICHO_SUPPORT)
        if (JER_KAPS_ENABLE(unit)){
            if(init_param->warmboot) {

                DISPLAY_MEM ;
                DISPLAY_SW_STATE_MEM ;

                LOG_INFO(BSL_LS_BCM_INIT,
                         (BSL_META_U(unit,
                                     "%d: Init KAPS Appl.\n"), unit));
                rv = dpp_kaps_init(unit);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "init_kaps_app () Failed:\n")));
                    goto exit ;
                }
            }
        }
#endif /* defined(BCM_JERICHO_SUPPORT) */
#endif /* defined(INCLUDE_KBP) */
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* init BCM */
	if(!init_param->no_bcm) {

        char mcs_msg[128];
        DISPLAY_MEM ;
        memset(mcs_msg, 0x0, sizeof(mcs_msg));
        if (init_param->warmboot) {
            sal_snprintf(mcs_msg, sizeof(mcs_msg) -1, "mcsmsg INIT;");
            sh_process_command(unit, mcs_msg);
            memset(mcs_msg, 0x0, sizeof(mcs_msg));
            sal_snprintf(mcs_msg, sizeof(mcs_msg) -1, "mcsmsg 0;");
            sh_process_command(unit, mcs_msg);
            sal_snprintf(mcs_msg, sizeof(mcs_msg) -1, "mcsmsg 1;");
            sh_process_command(unit, mcs_msg);
        }

#if defined(BCM_WARM_BOOT_SUPPORT)
        DISPLAY_SW_STATE_MEM ;
#endif /* BCM_WARM_BOOT_SUPPORT */

        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "%d: Init BCM.\n"), unit));
        rv = bcm_init(unit);
        if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "bcm_init () Failed:\n")));
           goto exit ; 
        }

#if defined(BCM_WARM_BOOT_SUPPORT)
        DISPLAY_SW_STATE_MEM ;
#endif /* BCM_WARM_BOOT_SUPPORT */

        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "%d: Init BCM Done.\n"), unit));
    }
	
#ifdef BCM_PETRA_SUPPORT
	/*Load the uKernel if mcs_load_uc0 or mcs_load_uc1 is not "none" */
    if (SOC_IS_DPP(unit) && soc_feature(unit, soc_feature_cmicm) && (!soc_property_get(unit, spn_RCPU_ONLY, 0))) {
        const ibde_dev_t *dev = bde->get_dev(CMDEV(unit).dev.dev);
        const char *dev_rev;
        char mcs_load[256];
        uint16 devid = dev->device; 
        uint8 revid = dev->rev; 
        uint8 core;
        char *propval;
        char *propval1;
        char uc_image_name_prefix[64];

    /* call to sysconf_chip_override cause kernel build error */
#ifndef __KERNEL__
        sysconf_chip_override(unit, &devid, &revid); 
#endif   
        dev_rev = soc_cm_get_device_name(devid, revid);
        for (core = 0; core < 2; core++) {
            memset(mcs_load, 0x0, sizeof(mcs_load));
            propval1 = soc_property_port_num_get_str(unit, core, "fw_core");
            propval = soc_property_get_str(unit, (core == 0) ? spn_MCS_LOAD_UC0 : spn_MCS_LOAD_UC1);
            if ((propval1 != NULL) ||
                ((propval != NULL) && (strcmp(propval, "none") != 0))) {
                if (!init_param->warmboot) {
                    if (propval1 != NULL) {
                        sal_snprintf(mcs_load, sizeof(mcs_load) -1, "mcsload %d %s %s;", core, propval1, (core == 0) ? "ResetUC=true InitMCS=true StartUC=true StartMSG=true" : "");
                    } else if (soc_feature(unit, soc_feature_uc_image_name_with_no_chip_rev) && (strcmp(propval, "pon") != 0)) {
                        uc_image_name_prefix[0] = '\0';
                        /* coverity[overrun-local] */
                        sal_strncat(uc_image_name_prefix, dev_rev, sal_strlen(dev_rev));
                        uc_image_name_prefix[sal_strlen(uc_image_name_prefix) - 3]= '\0';
                        if (core == 0) {
                            sal_snprintf(mcs_load, sizeof(mcs_load) -1, "mcsload 0 %s_0_%s.srec ResetUC=true InitMCS=true StartUC=true StartMSG=true;", uc_image_name_prefix, propval);
                        } else if (core == 1) {
                            sal_snprintf(mcs_load, sizeof(mcs_load) -1, "mcsload 1 %s_1_%s.srec;", uc_image_name_prefix, propval);
                        }
                    } else {
                        if (core == 0) {
                            sal_snprintf(mcs_load, sizeof(mcs_load) -1, "mcsload 0 %s_0_%s.srec ResetUC=true InitMCS=true StartUC=true StartMSG=true;", dev_rev, propval);
                        } else if (core == 1) {
                            sal_snprintf(mcs_load, sizeof(mcs_load) -1, "mcsload 1 %s_1_%s.srec;", dev_rev, propval);
                        }
                    }
                LOG_INFO(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "%s \n"), mcs_load));
                sh_process_command(unit, mcs_load);
                LOG_INFO(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "mcs cmd: Done \n")));
                sh_process_command(unit, "mcsstat");
                }
            }
        }
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
        /* Check if BFD application is enabled */
        if(SOC_IS_DPP(unit) && init_param->warmboot) {
            if (SOC_DPP_CONFIG(unit)->pp.bfd_enable) {
                DCMN_RUNTIME_DEBUG_PRINT(unit);
                LOG_INFO(BSL_LS_BCM_INIT,
                        (BSL_META_U(unit,"%d: Init BFD Appl.\n"), unit));
                rv = bcm_bfd_init(unit);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_COMMON,
                                (BSL_META_U(unit, "appl_dpp_bfd_init: failed. Error:%d (%s) \n"),
                                rv, bcm_errmsg(rv)));
                return rv;
                }
            }
        }
#endif
#endif

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    /* init KBP device */
    if (SOC_DPP_IS_ELK_ENABLE(unit)) {

        DISPLAY_MEM ;
#if defined(BCM_WARM_BOOT_SUPPORT)
        DISPLAY_SW_STATE_MEM ;
#endif /* BCM_WARM_BOOT_SUPPORT */

#if !defined(__KERNEL__) && defined (LINUX) && defined(DNX_INTERNAL)
        if (ARAD_KBP_IS_OP_OR_OP2) {

            
            system("/sbin/insmod kbp_driver.ko");
        }
#endif
        if (!init_param->warmboot) {
            /*
             * Prepare initializing KBP, including global resources
             * allocation, callback register, device config, etc.
             */
            if (SOC_IS_ARAD(unit) && (!init_param->no_elk_device)) {
                rv = dpp_kbp_init_prepare(unit, 0, init_param->elk_mdio_id, init_param->elk_ilkn_rev);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "Failed to prepare KBP initialization data for core0. \n")));
                    goto exit;
                }
            }
            if (SOC_IS_JERICHO(unit) && (!init_param->no_elk_second_device)) {
                rv = dpp_kbp_init_prepare(unit, 1, init_param->elk_mdio_second_id, init_param->elk_ilkn_rev);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "Failed to prepare KBP initialization data for core1. \n")));
                    goto exit;
                }
            }
            /*
             * Initialize KBP XPT
             */
            LOG_INFO(BSL_LS_BCM_INIT,
                     (BSL_META_U(unit,
                                 "%d: Init KBP Transport.\n"), unit));

            if (SOC_IS_ARAD(unit) && (!init_param->no_elk_device)) {
                rv = dpp_kbp_init_kbp_transport(unit, 0);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "KBP transport init core 0 Failed:\n")));
                    goto exit; 
                }
            }
            if (SOC_IS_JERICHO(unit) && (!init_param->no_elk_second_device)) {
                rv = dpp_kbp_init_kbp_transport(unit, 1);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "KBP transport init core 1 Failed:\n")));
                    goto exit; 
                }
            }
        }

        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "%d: Init KBP interface.\n"), unit));

        if (SOC_IS_ARAD(unit) && (!init_param->no_elk_device)) {
            rv = dpp_kbp_init_kbp_interface(unit, 0); 
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META_U(unit,
                                      "KBP device init core 0 Failed:\n")));
                goto exit ; 
            }
        }
        if (SOC_IS_JERICHO(unit) && (!init_param->no_elk_second_device)) {
            rv = dpp_kbp_init_kbp_interface(unit, 1);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META_U(unit,
                                      "KBP device init core 1 Failed:\n")));
                goto exit ; 
            }
        }
    }
#endif 

#ifdef INCLUDE_INTR
    /* init interrupts */
    if (!init_param->no_intr){
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "%d: Init Interrupts Appl.\n"), unit));

#if defined(BCM_JERICHO_SUPPORT) || defined(BCM_DFE_SUPPORT)
        if (SOC_IS_JERICHO(unit) || (SOC_IS_FE3200(unit))){
            rv = interrupt_appl_init(unit);
            if (BCM_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_APPL_SHELL,
                         (BSL_META_U(unit,
                                     "interrupt_appl_initn () Failed:\n")));
               goto exit ; 
            }
        } else
#endif
        {
            rv = interrupt_handler_appl_init(unit);
            if (BCM_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_APPL_SHELL,
                         (BSL_META_U(unit,
                                     "interrupt_handler_appl_init () Failed:\n")));
               goto exit ; 
            }
        }
    }

    /* turn off warmboot flag */
    SOC_WARM_BOOT_DONE(unit);

    /* init RX los application */
    if( (!init_param->no_rx_los) && ((SOC_IS_ARAD(unit) && !SOC_IS_ARDON(unit) && !SOC_IS_JERICHO(unit)) || SOC_IS_FE1600(unit)) ) {
        bcm_pbmp_t pbmp_default;
        BCM_PBMP_CLEAR(pbmp_default);

        DISPLAY_MEM ;
#if defined(BCM_WARM_BOOT_SUPPORT)
        DISPLAY_SW_STATE_MEM ;
#endif /* BCM_WARM_BOOT_SUPPORT */

        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "%d: Init RX Los Appl.\n"), unit));
        rv = rx_los_set_config(0, 0, 0, 0, 0, 0);
        if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "rx_los_set_config() Failed:\n"))); 
           goto exit ; 
        }
        rv = rx_los_unit_attach(unit, pbmp_default, init_param->warmboot);
        if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "rx_los_unit_attach() Failed:\n"))); 
           goto exit ; 
        }
    }
#else
    if (!init_param->no_intr){
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "Warning at appl_dcmn_init: INTR is not supported while interrupt is enabled.\n"))); 
    }
#endif /* INCLUDE_INTR */

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)){
        /* init linkscan */
        if (!init_param->no_linkscan){
            DISPLAY_MEM ;
#if defined(BCM_WARM_BOOT_SUPPORT)
            DISPLAY_SW_STATE_MEM ;
#endif /* BCM_WARM_BOOT_SUPPORT */

            LOG_INFO(BSL_LS_BCM_INIT,
                     (BSL_META_U(unit,
                                 "%d: Init LinkScan Appl.\n"), unit));
            rv = appl_dcmn_linkscan_init(unit, init_param->warmboot);
            if (BCM_FAILURE(rv)) {
                   LOG_ERROR(BSL_LS_APPL_SHELL,
                             (BSL_META_U(unit,
                                         "Init Linkscan Failed:\n")));
                goto exit ; 
            }
        }


        /* init KBP if not warmboot */
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        /* init KBP application */
        if (SOC_DPP_IS_ELK_ENABLE(unit) && !init_param->no_elk_appl) {
            if(!init_param->warmboot) {

                DISPLAY_MEM ;
#if defined(BCM_WARM_BOOT_SUPPORT)
                DISPLAY_SW_STATE_MEM ;
#endif /* BCM_WARM_BOOT_SUPPORT */

                LOG_INFO(BSL_LS_BCM_INIT,
                         (BSL_META_U(unit,
                                     "%d: Init KBP Appl.\n"), unit));

                rv = dpp_kbp_init_appl(unit, (SOC_IS_JERICHO(unit) && (!init_param->no_elk_second_device)), elk);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "init_kbp_app () Failed:\n"))); 
                    goto exit ; 
                }
            }
        }
        /* init KAPS application */
#if defined(BCM_JERICHO_SUPPORT)
        if (JER_KAPS_ENABLE(unit)){
            if(!init_param->warmboot) {

                DISPLAY_MEM ;


                LOG_INFO(BSL_LS_BCM_INIT,
                         (BSL_META_U(unit,
                                     "%d: Init KAPS Appl.\n"), unit));
                rv = dpp_kaps_init(unit);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "init_kaps_app () Failed:\n")));
                    goto exit ;
                }
            }
        }
#endif /* defined(BCM_JERICHO_SUPPORT) */
#endif 

        disable_rx_appl = 0;

#ifndef ADAPTER_SERVER_MODE
        disable_rx_appl = SAL_BOOT_PLISIM;
#endif
        /* open traffic RX path */
        if(!init_param->no_packet_rx && !disable_rx_appl) {

            DISPLAY_MEM ;


            LOG_INFO(BSL_LS_BCM_INIT,
                     (BSL_META_U(unit,
                                 "%d: Init Packet RX Appl.\n"), unit));
            rv = appl_dcmn_rx_activate(unit, init_param->packet_rx_cosq);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META_U(unit,
                                      "appl_dcmn_rx_activate() Failed:\n"))); 
                goto exit ; 
            }
        }

        /* init application */
        if (!init_param->no_appl) {

            DISPLAY_MEM ;

            DISPLAY_MEM_PRINTF(("%s(): unit %d: Going to call %s\r\n",__func__,unit,"appl_dcmn_appl_init")) ;

            rv = appl_dcmn_appl_init(unit, init_param);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META_U(unit,
                                      "appl_dcmn_appl_init () Failed:\n"))); 
                goto exit ; 
            } 
        }
    }
#endif /* defined(BCM_PETRA_SUPPORT) */

#if defined(BCM_DFE_SUPPORT)
    /* init STK */
    if (SOC_IS_DFE(unit))
    {
        if (!init_param->warmboot && !init_param->no_appl_stk && (init_param->appl_traffic_enable_stage != TRAFFIC_EN_STAGE_DISABLE) ) {

            DISPLAY_MEM ;
#if defined(BCM_WARM_BOOT_SUPPORT)
            DISPLAY_SW_STATE_MEM ;
#endif /* BCM_WARM_BOOT_SUPPORT */

            LOG_INFO(BSL_LS_BCM_INIT,
                     (BSL_META_U(unit,
                                 "%d: Init STK module.\n"), unit));
            rv = appl_dcmn_dfe_stk_init(unit,init_param->modid);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META_U(unit,
                                      "appl_dcmn_dfe_stk_init () Failed:\n"))); 
                goto exit ; 
            }                      
        }
    }
#endif /* defined(BCM_DFE_SUPPORT) */

#ifdef BCM_WARM_BOOT_API_TEST
#ifdef DCMN_AUTOSYNC_TEST_MODE
                 /* enable autosync only if autosync regression is enabled */
                 cli_out("Warmboot test mode - enabling autosync mode (may take a few seconds for first full sync)\n");
                 bcm_switch_control_set(unit, bcmSwitchControlAutoSync, 0x1);
                 /* enable reboot after every api call */
#endif
#endif 

    /* HW Log - can be started */
    SOC_CONTROL(unit)->soc_flags |= SOC_F_ALL_MODULES_INITED;
	

 
/* init crash recovery application */
/* NOTE: should be last! */
#ifdef CRASH_RECOVERY_SUPPORT
    if (SOC_IS_JERICHO(unit)) {
        if (DCMN_CR_JOURNALING_MODE_DISABLED != soc_dcmn_cr_journaling_mode_get(unit)) {
            LOG_INFO(BSL_LS_BCM_INIT,
                         (BSL_META_U(unit,
                                     "%d: Enable Crash Recovery Appl.\n"), unit));
            SOC_CONTROL(unit)->crash_recovery = TRUE;

            if (!init_param->warmboot) {
                dcmn_cr_info[unit]->transaction_status = DCMN_TRANSACTION_MODE_IDLE;
            }
        }
    }
    /* try recovering from a crash
     * (internally check if a crash occurred)
     * In case of roll back there is a need to reinit the soc and bcm.
     * There might be cases that some modules rely on the data
     * in the sw_state (by using sw_state getters) which is invalid because it's before updating
     * the sw_state in the roll-back process
     */
     {
         uint8 dcmn_cr_after_roll_back;
         sw_state_journal_is_after_roll_back_get(unit, &dcmn_cr_after_roll_back);
            if (!dcmn_cr_after_roll_back) {
                BCM_EXIT_IF_FAILURE(soc_dcmn_cr_recover(unit),
                    "soc_dcmn_cr_recover() Failed: Could not recover from Crash\n");
                BCM_EXIT_IF_FAILURE(sw_state_journal_is_after_roll_back_get(unit, &dcmn_cr_after_roll_back),
                    "sw_state_journal_is_after_roll_back_get() failed!\n");
                if (dcmn_cr_after_roll_back) {

#if defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030)

                if (SOC_DPP_IS_ELK_ENABLE(unit) && !(ARAD_KBP_IS_CR_MODE(unit))) {
                    int rv;
                    rv = arad_kbp_sync(unit);
                    if (rv < 0) {
                        cli_out("ERROR: in arad_kbp_sync, rv= %d.\n", rv);
                    }
                }
#if defined(BCM_88675_A0)
                if (JER_KAPS_ENABLE(unit) && !(JER_KAPS_IS_CR_MODE(unit))) {
                    int rv;
                    rv = jer_kaps_sync(unit);
                    if (rv < 0) {
                        cli_out("ERROR: in jer_kaps_sync, rv= %d.\n", rv);
                    }
                }
#endif
#endif

                   BCM_EXIT_IF_FAILURE(appl_dcmn_deinit(unit, init_param),
                       "appl_dcmn_deinit() Failed: deinit after roll back failed!\n");
                   BCM_EXIT_IF_FAILURE(appl_dcmn_init(unit, init_param),
                       "appl_dcmn_init() Failed: init after roll back failed!\n");
                   BCM_EXIT_IF_FAILURE(sw_state_journal_is_after_roll_back_set(unit, FALSE),
                       "sw_state_journal_is_after_roll_back_set() failed!\n");
                }
            }
     }

#endif


exit:

    DISPLAY_MEM ;

    DISPLAY_MEM_PRINTF(("%s(): unit %d: Exit\r\n",__func__,unit)) ;

    return rv;
}

/*
 * Function:    appl_dcmn_deinit
 * Purpose:     Deinit sequence for DNX
 * Parameters:    u - unit #.
 *        partial_deinit - partial deinit.
 *        init_param - ignored cookie.
 * Returns:    0
 */
int appl_dcmn_deinit(int unit, appl_dcmn_init_param_t* init_param) 
{
    int func_rv = BCM_E_NONE, rv = BCM_E_NONE;
#if (defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
    int warmboot_test_mode_enable;
#endif

        /* if warmboot API test, should skip it before/after API caller function */
#if (defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
       soc_dcmn_wb_test_mode_get(unit, &warmboot_test_mode_enable);
       if (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API == warmboot_test_mode_enable){
           soc_dcmn_wb_test_mode_set(unit, 0);
       }
#endif
#if defined(BCM_PETRA_SUPPORT)
#ifdef CRASH_RECOVERY_SUPPORT
    /* stop journaling during detach */
    if (SOC_IS_JERICHO(unit)) {
        rv = soc_dcmn_cr_journaling_mode_set(unit, 0);
        if (SOC_FAILURE(rv) < 0) {
           LOG_ERROR(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "deinit failed to turn off CR journalling:\n")));
           return rv;
        }
    }

    cli_out("Stop Autosync during detach\n");
    rv = bcm_switch_control_set(unit, bcmSwitchControlAutoSync, 0x0);
    if (SOC_FAILURE(rv) < 0) {
       LOG_ERROR(BSL_LS_APPL_SHELL,
                 (BSL_META_U(unit,
                             "deinit failed to turn off autosync:\n")));
       return rv;
    }

    SOC_CR_DISABLE(unit);
#endif /* CRASH_RECOVERY_SUPPORT */
#endif

    /* check input parameters */
    if ((init_param->no_init || init_param->no_deinit) && (init_param->no_attach == FALSE)) { 
        cli_out("deinit can not be called with detach without attach unit afterward. NoAttach forced to 1!!\n"); 
        init_param->no_attach = 1;
    }
     
    /* unit not attached, no deinit */
    if (SOC_CONTROL(unit) == NULL) {
        return BCM_E_FAIL;
    }

    if(init_param->no_deinit) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "appl_dcmn_deinit should not be called with no_deinit parameter set\n"))); 
        return BCM_E_PARAM; 
    }

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    /* deinit KBP */
    if (SOC_DPP_IS_ELK_ENABLE(unit) && !init_param->no_elk_appl) {
        cli_out("%d: Deinit KBP Appl.\n", unit);
        rv = arad_kbp_deinit_app(unit, (SOC_IS_JERICHO(unit) && (!init_param->no_elk_second_device)), init_param->no_sync);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "arad_kbp_deinit_app() Failed:\n")));
           return rv;
        }

        cli_out("%d: Deinit KBP Transport.\n", unit);
        rv = dpp_kbp_deinit_kbp_transport(unit, (SOC_IS_JERICHO(unit) && (!init_param->no_elk_second_device)));
        if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "arad_kbp_deinit_transport() Failed:\n")));
           return rv;
        }
    }

       /* init KAPS application */
#if defined(BCM_JERICHO_SUPPORT)
    if (JER_KAPS_ENABLE(unit)){
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "%d: Deinit KAPS Appl.\n"), unit));
        rv = jer_kaps_deinit_app(unit, init_param->no_sync);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "deinit_kaps_app () Failed:\n"))); 
            return rv;
        }
    }
#endif /* defined(BCM_JERICHO_SUPPORT) */
#endif 

    /* deinit linkscan */
    if (SOC_IS_ARAD(unit) && !init_param->no_linkscan) {
        cli_out("%d: Deinit Linkscan.\n", unit);
        rv = bcm_linkscan_detach(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "bcm_linkscan_detach () Failed:\n")));
            SAVE_RV_IF_NOT_FAIL(func_rv, rv);
        }
    }

    /*
     * deinit dcmn_counter_val so we won't have memory leak.
     */
    dcmn_counter_val_deinit(unit);

#ifdef INCLUDE_INTR
    /* deinit RX los application */
    if((!init_param->no_rx_los) && ((SOC_IS_ARAD(unit) && !SOC_IS_ARDON(unit) && !SOC_IS_JERICHO(unit)) || SOC_IS_FE1600(unit))) {
        cli_out("%d: Deinit Rx Los Appl.\n", unit);
        rv = rx_los_unit_detach(unit);
        if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "rx_los_unit_detach () Failed:\n")));
           SAVE_RV_IF_NOT_FAIL(func_rv, rv);
        }
    }

    /* deinit interrupts */
    if (!init_param->no_intr) {
        cli_out("%d: Deinit Interrupts.\n", unit);

#if defined(BCM_JERICHO_SUPPORT) || defined(BCM_DFE_SUPPORT)
        if (SOC_IS_JERICHO(unit) || (SOC_IS_FE3200(unit))){
            rv = interrupt_appl_deinit(unit);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META_U(unit,
                                      "interrupt_appl_deinit () Failed:\n")));
                SAVE_RV_IF_NOT_FAIL(func_rv, rv);
            }
        } else
#endif
        {
            rv = interrupt_handler_appl_deinit(unit);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META_U(unit,
                                      "interrupt_handler_appl_deinit () Failed:\n")));
                SAVE_RV_IF_NOT_FAIL(func_rv, rv);
            }
        }
    }
#endif /* INCLUDE_INTR */

    if (SOC_IS_ARAD(unit) && !init_param->no_deinit && !init_param->no_packet_rx) {
        cli_out("%d: Deinit Packet RX Appl.\n", unit);
        rv = bcm_rx_stop(unit, NULL);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "bcm_rx_stop () Failed:\n")));
            SAVE_RV_IF_NOT_FAIL(func_rv, rv);
        }
    }

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARADPLUS(unit) && !init_param->no_deinit) {
        if (SOC_DPP_CONFIG(unit)->pp.oam_enable) {
            cli_out("%d: Deinit OAM.\n", unit);
            rv = bcm_oam_detach(unit);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                        (BSL_META_U(unit,
                                    "_bcm_petra_oam_detach () Failed:\n")));
                SAVE_RV_IF_NOT_FAIL(func_rv, rv);
            }
            cli_out("%d: Deinit OAM Done.\n", unit);
        }

        if (SOC_DPP_CONFIG(unit)->pp.bfd_enable) {
            cli_out("%d: Deinit BFD.\n", unit);
            rv = bcm_bfd_detach(unit);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                        (BSL_META_U(unit,
                                    "bcm_petra_bfd_detach () Failed:\n")));
                SAVE_RV_IF_NOT_FAIL(func_rv, rv);
            }
            cli_out("%d: Deinit BFD Done.\n", unit);
        }
    }
#endif

    /* deinit BCM */
    if (!init_param->no_bcm) {
        cli_out("%d: Deinit BCM.\n", unit);
        rv = bcm_detach(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "bcm_detach () Failed:\n")));
            SAVE_RV_IF_NOT_FAIL(func_rv, rv);
        }
        cli_out("%d: Deinit BCM Done.\n", unit);
    }

    /* deinit SOC */
    if (!init_param->no_soc) {
        cli_out("%d: Deinit SOC.\n", unit);
        rv = soc_deinit(unit);
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "soc_deinit () Failed:\n")));
           SAVE_RV_IF_NOT_FAIL(func_rv, rv);
        }
        cli_out("%d: Deinit SOC Done.\n", unit);
    }

    /* detach unit */
    if (!init_param->no_attach) {
        cli_out("%d: Detach unit.\n", unit);
        rv = soc_cm_device_destroy(unit);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "soc_cm_device_destroy () Failed:\n")));
            SAVE_RV_IF_NOT_FAIL(func_rv, rv);
        }         
    }

#if (defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
        if (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API == warmboot_test_mode_enable){
            soc_dcmn_wb_test_mode_set(unit, _DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API);
        }
#endif
    return func_rv;
}

/*
 * Function:     appl_dcmn_init_usage_parse
 * Purpose:      appl dcmn init usage parse command
 * Parameters:   u - unit number to operate on
 *               a - args (none expected)
 *               init_param - return parsed parameter struct 
 * Returns:      CMD_OK/CMD_FAIL.
 */
cmd_result_t
appl_dcmn_init_usage_parse(int unit, args_t *a, appl_dcmn_init_param_t *init_param)
{
    int rv = CMD_OK;
    parse_table_t pt;
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    char *propkey, *propval;
    int is_op = 0;
#endif

    if (SOC_IS_DFE(unit)) {
        init_param->no_elk_device = 0x1;
        init_param->no_elk_appl = 0x1;
    }
    parse_table_init(unit, &pt);

    parse_table_add(&pt, "NofDevices", PQ_INT, (void *)-1,
       &(init_param->nof_devices), NULL);
    parse_table_add(&pt, "Repeat", PQ_INT, (void *) 0,
      &(init_param->repeat), NULL);
    parse_table_add(&pt, "NoInit", PQ_INT, (void *) 0,
      &(init_param->no_init), NULL);
    parse_table_add(&pt, "NoDeinit", PQ_INT | PQ_DFL, (void *) 0, 
          &(init_param->no_deinit), NULL);
    parse_table_add(&pt, "NoAttach", PQ_INT | PQ_DFL, (void *)0, 
      &(init_param->no_attach), NULL);
    parse_table_add(&pt, "NoBcm", PQ_INT, (void *) 0,
      &(init_param->no_bcm), NULL);
    parse_table_add(&pt, "NoSoc", PQ_INT, (void *) 0,
      &(init_param->no_soc), NULL);
#ifdef ADAPTER_SERVER_MODE
    parse_table_add(&pt, "NoIntr", PQ_INT, (void *) 1,
      &(init_param->no_intr), NULL);
#else
    parse_table_add(&pt, "NoIntr", PQ_INT, (void *) 0,
      &(init_param->no_intr), NULL);
#endif /* ADAPTER_SERVER_MODE */
    parse_table_add(&pt, "NoRxLos", PQ_INT, (void *) 0,
      &(init_param->no_rx_los), NULL);
    parse_table_add(&pt, "NoAppl", PQ_INT, (void *) 0,
      &(init_param->no_appl), NULL);
    parse_table_add(&pt, "NoApplStk", PQ_INT, (void *) 0,
      &(init_param->no_appl_stk), NULL);
    parse_table_add(&pt, "NoPktRx", PQ_INT, (void *) 0,
      &(init_param->no_packet_rx), NULL);
    parse_table_add(&pt, "PktRxCosq", PQ_INT, (void *) 0,
      &(init_param->packet_rx_cosq), NULL);
    parse_table_add(&pt, "ApplTrEnStage", PQ_INT, (void *) TRAFFIC_EN_STAGE_AFTER_STK,
      &(init_param->appl_traffic_enable_stage), NULL);
    parse_table_add(&pt, "ApplFlags", PQ_INT, (void *) 0,
      &(init_param->appl_flags), NULL);
    parse_table_add(&pt, "ModID", PQ_INT | PQ_DFL, (void *) 0,
      &(init_param->modid), NULL); 
    parse_table_add(&pt, "BaseModID", PQ_INT | PQ_DFL, (void *) 0,
      &(init_param->base_modid), NULL);
#if defined(BCM_GEN_ERR_MECHANISM)
    parse_table_add(&pt, "PartialInit", PQ_INT, (void *) 0,
      &(init_param->partial_init), NULL);
    parse_table_add(&pt, "FakeBcmStart", PQ_INT, (void *) 0,
      &(init_param->fake_bcm_start), NULL);
    parse_table_add(&pt, "FakeBcmRange", PQ_INT, (void *) 0,
      &(init_param->fake_bcm_range), NULL);
    parse_table_add(&pt, "FakeSocStart", PQ_INT, (void *) 0,
      &(init_param->fake_soc_start), NULL);
    parse_table_add(&pt, "FakeSocRange", PQ_INT, (void *) 0,
      &(init_param->fake_soc_range), NULL);
#endif
    parse_table_add(&pt, "NoLinkscan", PQ_INT, (void *) 0,
      &(init_param->no_linkscan), NULL);
    parse_table_add(&pt, "Warmboot", PQ_INT, (void *) 0,
      &(init_param->warmboot), NULL);
    parse_table_add(&pt, "Engine", PQ_INT, (void *) 0,
      &(init_param->engine_dump), NULL);
    parse_table_add(&pt, "NoSync;", PQ_INT, (void *) 0,
      &(init_param->no_sync), NULL);
    parse_table_add(&pt, "NoDump;", PQ_INT, (void *) 0,
      &(init_param->no_dump), NULL);
    parse_table_add(&pt, "NoElkAppl", PQ_INT | PQ_DFL, (void *)0, 
    &(init_param->no_elk_appl), NULL);
    parse_table_add(&pt, "NoElkDevice", PQ_INT | PQ_DFL, (void *)0,
    &(init_param->no_elk_device), NULL);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    propkey = spn_EXT_TCAM_DEV_TYPE;
    propval = soc_property_get_str(unit, propkey);
    if (propval) {
        is_op = ((sal_strcmp(propval, "BCM52311") == 0) || (sal_strcmp(propval, "BCM52321") == 0));
    }
    parse_table_add(&pt, "ElkIlknRev", PQ_INT, (void *) ARAD_KBP_APPL_ILKN_REVERSE_DEFAULT,
    &(init_param->elk_ilkn_rev), NULL);
    if (is_op) {
        parse_table_add(&pt, "ElkMdioId", PQ_INT, (void *) INT_TO_PTR(JER_KBP_OP_APPL_MDIO_DEFAULT_ID),
                        &(init_param->elk_mdio_id), NULL);
    } else {
        parse_table_add(&pt, "ElkMdioId", PQ_INT, (void *) INT_TO_PTR(ARAD_KBP_APPL_MDIO_DEFAULT_ID),
                        &(init_param->elk_mdio_id), NULL);
    }
    if (SOC_IS_QAX(unit)) {
        parse_table_add(&pt, "NoElkSecDevice", PQ_INT, (void *)1,
                        &(init_param->no_elk_second_device), NULL);
    } else if (SOC_IS_JERICHO(unit)) {
        parse_table_add(&pt, "NoElkSecDevice", PQ_INT | PQ_DFL, (void *)0,
        &(init_param->no_elk_second_device), NULL);
        if (is_op) {
            parse_table_add(&pt, "ElkMdioSecId", PQ_INT, INT_TO_PTR(JER_KBP_OP_APPL_SEC_MDIO_DEFAULT_ID),
                            &(init_param->elk_mdio_second_id), NULL);
        } else {
            parse_table_add(&pt, "ElkMdioSecId", PQ_INT, INT_TO_PTR(JER_KBP_APPL_SEC_MDIO_DEFAULT_ID),
                            &(init_param->elk_mdio_second_id), NULL);
        }
    }
#endif
#if AGGRESSIVE_ALLOC_DEBUG_TESTING
    parse_table_add(&pt, "ResoucesLeakCheck", PQ_INT, (void *) 0,
      &(init_param->is_resources_check), NULL);
    parse_table_add(&pt, "AggressiveNumToTest", PQ_INT, (void *) 0,
      &(init_param->aggressive_num_to_test), NULL);
    parse_table_add(&pt, "AggressiveKeepOrder", PQ_INT, (void *) 0,
      &(init_param->aggressive_alloc_debug_testing_keep_order), NULL);
    parse_table_add(&pt, "AggressiveLongFreeSearch", PQ_INT, (void *) 0,
      &(init_param->aggressive_long_free_search), NULL);

#endif
    parse_table_add(&pt, "ErrorOnLeak", PQ_INT, (void *) 0,
      &(init_param->error_on_leak), NULL);
    parse_table_add(&pt, "CosqDisable", PQ_INT, (void *) 0,
      &(init_param->cosq_disable), NULL);
    parse_table_add(&pt, "NoItmhProgMode", PQ_INT, (void *) 0,
      &(init_param->no_itmh_prog_mode), NULL);
    parse_table_add(&pt, "RcLoad", PQ_INT, (void *) 0,
      &(init_param->rc_load), NULL);
    parse_table_add(&pt, "L2Mode", PQ_INT, (void *) 0,
      &(init_param->l2_mode), NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",appl_dcmn_init_usage);
        rv = CMD_FAIL;
    } else if (ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",appl_dcmn_init_usage);
        rv = CMD_FAIL;
    }
    parse_arg_eq_done(&pt); 

    /**
     * we cant compare  amount of memory allocated in warmboot state 
     * in init stage 
     * to amount memory freed at deinit stage in coldboot stage 
     * or 
     * vice versa 
     */
    if (init_param->error_on_leak && 
        (
         (previous_init_was_warmboot[unit]  &&
         !init_param->warmboot)  
         ||
         (!previous_init_was_warmboot[unit]  &&
          init_param->warmboot)          
        )) {
        cli_out("Can't run tr 141 with  ErrorOnLeak=1 if  previous init run in warmboot state and current deinit run in coldboot state or vice versa\n");
        rv = CMD_FAIL;
    }
    if (rv==CMD_OK) {
        previous_init_was_warmboot[unit] = init_param->warmboot; 
    }

    return rv; 
}

/*
 * Function:     sh_init_dnx
 * Purpose:      parse shell command and call init_dnx/deinit_dnx functions
 * Parameters:   u - unit number to operate on
 *               a - args (none expected)
 * Returns:      CMD_OK/CMD_FAIL.
 */
cmd_result_t
cmd_init_dnx(int u, args_t *a)
{
    appl_dcmn_init_param_t init_param;

    sal_memset(&init_param, 0x0, sizeof(init_param));
    /* default init values for no_attach & no_deinit & modid are different from tr 141 and need to be specified here */
    init_param.no_attach = 1;
    init_param.no_deinit = 1;

    if (appl_dcmn_init_usage_parse(u, a, &init_param) < 0) {
        return CMD_FAIL;
    }
    if (init_param.no_deinit == 0) {
        if (appl_dcmn_deinit(u, &init_param) < 0) {
            return CMD_FAIL;
        }
    }
    if (init_param.no_init == 0) {
        if (appl_dcmn_init(u, &init_param) < 0) {
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

