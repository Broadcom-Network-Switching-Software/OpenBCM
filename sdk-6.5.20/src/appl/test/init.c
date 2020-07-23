/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Built-in init related tests
 */

#if defined(BCM_WARM_BOOT_API_TEST)
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>    
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#endif

#include <appl/diag/system.h> /* for parse_small_integers */
#include <ibde.h>

#include <soc/debug.h>
#include <soc/cmext.h>
#include <soc/wb_engine.h>
#include <shared/bsl.h>
#include <bcm/stack.h>
#include <soc/drv.h>
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/port_sw_db.h>
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/drv.h>
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
#endif /* BCM_PETRA_SUPPORT */
#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm/link.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm/switch.h>
#ifdef BCM_PETRA_SUPPORT
#include <bcm_int/dpp/switch.h>
#endif /* BCM_PETRA_SUPPORT */
#endif /*BCM_WARM_BOOT_SUPPORT*/

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#include <appl/diag/parse.h>
#include <appl/diag/test.h>
#include <appl/diag/shell.h>
#include <appl/diag/sysconf.h>
#ifdef SW_STATE_DIFF_DEBUG
#include <shared/swstate/alloc_list.h>
#endif
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
#include <appl/diag/dcmn/init.h>
#include <appl/dcmn/interrupts/interrupt_handler.h>
#include <appl/dcmn/rx_los/rx_los.h>

#include <soc/dcmn/dcmn_wb.h>

#include <appl/diag/dcmn/init_deinit.h>
#endif

#ifndef AGGRESSIVE_ALLOC_DEBUG_TESTING
  #define AGGRESSIVE_ALLOC_DEBUG_TESTING 0
#endif

#if defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <appl/diag/dpp/kbp.h>
#endif /* defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030) */


#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
#define DEINIT_IF_ERR_EXIT(_pi) \
    if (_pi == 0) { \
        goto done; \
    } \

#define INIT_IF_ERR_EXIT(_pi) \
    if (_pi) { \
        continue; \
    } else { \
        goto done; \
    } \

#define TEST_DUMP_LEVEL(level) \
    ((init_param->no_dump == 2) ? SW_STATE_EFFICIENT_DUMP : 0)


char warmboot_test_usage[] = 
#ifdef COMPILER_STRING_CONST_LIMIT
"Warmboot Test Usage:\n"
"  Repeat=<value>, NoInit=<value>, NoDeinit=<value>, NoBcm=<value>, NoSoc=<value>.\n"
"  NoInt=<value>, NoRxLos=<value>, NoAppl=<value>  NoDump=<value> .\n"
"  1: The test will not run.\n"
#else
"Warmboot Test Usage:\n"
"  Repeat=<value>         the test will loop as many times as stated. (default=1)\n"
"  NoSync=<value>         1: The test will not perform a sync to persistant storage (to be used with autosync mode).\n"
"  NoInit=<value>         1: The test will not Init the Chip. \n"
"  NoDeinit=<value>       1: The test will not deinit the Chip.\n"
"  NoBcm=<value>          1: The test will not run on BCM.\n"
"  NoSoc=<value>          1: The test will not run on Soc.\n"
"  NoAttach=<value>       1: The test will not do device attech detach, set to 1 by default.\n"
"  NoInt=<value>          1: The test will not run on Interrupts.\n"
"  NoRxLos=<value>        1: The test will not run on RX LOS App.\n"
"  NoLinkscan=<value>     1: The test will not run on Linkscan.\n"
"  NoAppl=<value>         1: The test will not run on Application.\n"
"  NoDump=<value>         1: No state dump comparison 2: Efficient state dump comparison.\n"
#endif
;

STATIC appl_dcmn_init_param_t init_deinit[SOC_MAX_NUM_DEVICES];

#if AGGRESSIVE_ALLOC_DEBUG_TESTING
/* { */
#include <src/sal/core/unix/alloc_debug.h>

#define UNFREE_ALLOCS_FILE "unfree_allocs.txt"
STATIC int init_aggressive_alloc_debug_testing(int itertaion_idx)
{
    static int32 after_deinit_nof_allocs = 0;
    int32 after_deinit_init_deinit_nof_allocs;
    static int32 after_deinit_dma_usgae = 0;
    int32 after_deinit_init_deinit_dma_usage;
    alloc_info *unfree_allocs;
    FILE *unfree_allocs_file;
    int rv = 0;
    uint32 allocation_idx;
    uint32 backtrace_idx;
    extern int _dma_get_usage(void); /* _dma_get_usage is implemented in linux-user-bde.c, which has no .h file. */

    if(itertaion_idx == 0) {     
        after_deinit_dma_usgae = _dma_get_usage();
        after_deinit_nof_allocs = sal_alloc_debug_nof_allocs_get();
    }

    if(itertaion_idx == 1) {
        after_deinit_init_deinit_dma_usage = _dma_get_usage();
        if(after_deinit_init_deinit_dma_usage > after_deinit_dma_usgae) {
            rv = -1;
            cli_out("Error: There are dma allocations that haven't been freed.\n");
            /* we don't return. We continue to check if there are unfree allocations done be sal_alloc */
        }
        after_deinit_init_deinit_nof_allocs = sal_alloc_debug_nof_allocs_get();

        if(after_deinit_init_deinit_nof_allocs > after_deinit_nof_allocs) {
            cli_out("The are unfree allocations done by sal_alloc.\n");
            rv = -1;
            unfree_allocs_file = sal_fopen(UNFREE_ALLOCS_FILE, "w");
            if(!unfree_allocs_file) {
                cli_out("Filed to open file %s.\n", UNFREE_ALLOCS_FILE);
                return rv;
            }
            unfree_allocs = sal_alloc(sizeof(alloc_info)*(after_deinit_init_deinit_nof_allocs - after_deinit_nof_allocs + 1), "");/* include this allocation*/
            if(!unfree_allocs) {
                cli_out("Memory allocation Failure.\n\r");
                return rv;
            }
            sal_alloc_debug_last_allocs_get(unfree_allocs, after_deinit_init_deinit_nof_allocs - after_deinit_nof_allocs + 1);
            for(allocation_idx = 0; allocation_idx < after_deinit_init_deinit_nof_allocs - after_deinit_nof_allocs; ++allocation_idx) {
                sal_fprintf(unfree_allocs_file, "allocation 0x%lx %d  from ",
                    (unsigned long)unfree_allocs[allocation_idx].addr, unfree_allocs[allocation_idx].size);
                for(backtrace_idx = ALLOC_DEBUG_TESTING_NOF_BACKTRACES - 1; backtrace_idx > 0; --backtrace_idx){
                    sal_fprintf(unfree_allocs_file, "%p -> ", unfree_allocs[allocation_idx].stack[backtrace_idx]);
                }
                sal_fprintf(unfree_allocs_file, "%p Description : %s\n",
                   unfree_allocs[allocation_idx].stack[0],
                   unfree_allocs[allocation_idx].desc);
            }
            sal_free(unfree_allocs);
            sal_fclose(unfree_allocs_file);
            return rv;
        } else {
            return rv;
        }
    }

    return rv;
}
/* } */
#endif


#ifndef NO_FILEIO
/* 
 * function to compare two files, 
 * returns BCM_E_NONE if the files are the same. 
*/ 
int 
diff_two_files(int unit, char *file_name1, char *file_name2)
{
    FILE *file1;
    FILE *file2;
    char str1[256];
    char str2[256];
    char *is_null1;
    char *is_null2;
    int rv = BCM_E_NONE;

    if ((file1 = sal_fopen(file_name1, "r")) == 0) {
        cli_out("Error opening sw dump file %s\n", file_name1);
        return BCM_E_INTERNAL;
    }
    if ((file2 = sal_fopen(file_name2, "r")) == 0) {
        sal_fclose(file1);
        cli_out("Error opening sw dump file %s\n", file_name2);
        return BCM_E_INTERNAL;
    }

    while (TRUE) {
        is_null1 = fgets(str1, 256, file1);
        is_null2 = fgets(str2, 256, file2);
        if ((is_null1 == NULL) || (is_null2 == NULL)) {
            break;
        }
        if(sal_strcmp(str1,str2) != 0) {
#ifdef SW_STATE_DIFF_DEBUG
            char *p1, *p2;
            int i = 0;
            uint32 offset;
#endif
            rv = BCM_E_INTERNAL;
            cli_out("sw state has changed after warm reboot. to see the difference compare file %s with %s\n", file_name1, file_name2);
            cli_out("printing the first difference:\nbefore: %s\nafter: %s\n", str1, str2);
#ifdef SW_STATE_DIFF_DEBUG
            for (p1 = str1, p2 = str2 ; *p1 == *p2 ; p1++, p2++);
            shr_sw_state_alloc_list_search(unit, sw_state_str_to_hex(str1) + (p1 - str1 - 9 /*offset*/)/2, str2, &offset);
            /* remove the "_alloc" suffix */
            while(str2[++i] != '\0');
            str2[i-6] = '\0';
            cli_out("sw state variable is: %s inside_offset: %d\n", str2, sw_state_str_to_hex(str1) + (p1 - str1)/2 - offset);
#else
            cli_out("for more information compile with the flag \"SW_STATE_DIFF_DEBUG\""
                        "(pmake.sh --flags \"-DSW_STATE_DIFF_DEBUG\") and run again.\n");
#endif
            goto exit;
        }
    }

    /* if only one file has reached EOF */
    if (is_null1 != is_null2){
            rv = BCM_E_INTERNAL;
            cli_out("sw state has changed after warm reboot. file %s and file %s have different length\n", file_name1, file_name2);
            goto exit;
    }

exit:
    sal_fclose(file1);
    sal_fclose(file2);

    if (rv != BCM_E_NONE) {
/* for performance, remove the linux diff system call */
#ifdef DO_SYSTEM_DIFF
        char diff_cmd[256];

        /*build diff command*/
        sal_strncpy(diff_cmd,  "diff ", sizeof(diff_cmd));
        sal_strncat_s(diff_cmd, file_name1, sizeof(diff_cmd));
        sal_strncat_s(diff_cmd, " ", sizeof(diff_cmd));
        sal_strncat_s(diff_cmd, file_name2, sizeof(diff_cmd));

        cli_out("********* diffing %s with %s **********\n", file_name1, file_name2);
        cli_out("note: using linux diff, diff may fail.\n");
        cli_out("note: when running in gdb diff is not printed to screen.\n");
        cli_out("      if no diff is printed, use breakpoint on this line\n");
        cli_out("      and manually diff the files immediately after the break\n");
        /* diff wont work when running inside gdb */
        system(diff_cmd);
        cli_out("*******************************************************************\n");
#else
        cli_out("********* diff was between %s and %s **********\n", file_name1, file_name2);
#endif
    }

#if defined(BCM_WARM_BOOT_API_TEST)
    /* delete the files if in wb test mode*/
    sal_remove(file_name1);
    sal_remove(file_name2);
#else
    /* in case of success, delete the files */
    if (rv == BCM_E_NONE) {
        sal_remove(file_name1);
        sal_remove(file_name2);
    }
#endif

    return rv;

}

#endif /*NO_FILEIO*/


unsigned int
do_error_on_leak_test(appl_dcmn_init_param_t *init_param)
{
    if (
        init_param->error_on_leak &&
        !init_param->no_init && 
        !init_param->no_deinit 
        ) {
        return 1;
    }

    return 0;

   
}



/*
 * Function:     init_deinit_test
 * Purpose:    Test Init & Deinit
 * Parameters:    u - unit #.
 *        a - pointer to arguments.
 *        p - ignored cookie.
 * Returns:    0
 */
int
init_deinit_test_internal(int unit, args_t *a, void *p)
{
    int rv = 0x0;
    int i = 0;
    int r = 1;
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
    int partial_init = 0;
#endif
    int partial_deinit = 0;
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)
    int j = 0;
    int compare = 0;
    int loop = 1;
#endif
#if (defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
    int warmboot_test_mode_enable;
#endif
    char file_name1[SOC_PROPERTY_NAME_MAX + 256];
    char file_name2[SOC_PROPERTY_NAME_MAX + 256];

#if defined(BCM_WARM_BOOT_API_TEST)
    int s;
    struct ifreq buffer;
    char mac_str[18];
#endif
    appl_dcmn_init_param_t *init_param = p;
    char    *stable_filename = NULL;

#if defined(BCM_WARM_BOOT_SUPPORT)    
    bcm_switch_service_config_t bcm_switch_service_config = {0};
#endif    
    COMPILER_REFERENCE(a);
    COMPILER_REFERENCE(p);

    file_name1[0] = '\0';
    file_name2[0] = '\0';

    stable_filename = soc_property_get_str(unit, spn_STABLE_FILENAME);

    /* prefixing with unique files name to enable more than one parallel run from same SDK folder  */
    /* assuming stable_filename is unique for each separate run */
    if (NULL != stable_filename) {
#ifdef BCM_WARM_BOOT_API_TEST
        /* in WB test mode save it in negev memory instead on stable_filename location on the network HDD */
        sal_strncat_s(file_name1, "/dev/shm/warmboot_data", sizeof(file_name1));
        sal_strncat_s(file_name2, "/dev/shm/warmboot_data", sizeof(file_name2));
#else
        sal_strncat_s(file_name1, stable_filename, sizeof(file_name1));
        sal_strncat_s(file_name2, stable_filename, sizeof(file_name2));
#endif
        sal_strncat_s(file_name1, "_", sizeof(file_name1));
        sal_strncat_s(file_name2, "_", sizeof(file_name2));
    }

    sal_strncat_s(file_name1, "sw_state_pre_reboot.txt", sizeof(file_name1));
    sal_strncat_s(file_name2, "sw_state_post_reboot.txt", sizeof(file_name2));

    /*concatanate mac adress to file name if in special wb test mode*/
#if defined(BCM_WARM_BOOT_API_TEST)
    /*get HW Adress*/
    s = socket(PF_INET, SOCK_DGRAM, 0);
    sal_memset(&buffer, 0x00, sizeof(buffer));
    sal_strcpy(buffer.ifr_name, "eth0");
    ioctl(s, SIOCGIFHWADDR, &buffer);
    close(s);
    sal_snprintf(mac_str, sizeof(mac_str), "%02x%02x%02x%02x%02x%02x",
         (unsigned char)buffer.ifr_hwaddr.sa_data[0], (unsigned char)buffer.ifr_hwaddr.sa_data[1], (unsigned char)buffer.ifr_hwaddr.sa_data[2],
         (unsigned char)buffer.ifr_hwaddr.sa_data[3], (unsigned char)buffer.ifr_hwaddr.sa_data[4], (unsigned char)buffer.ifr_hwaddr.sa_data[5]);

    sal_strcat(file_name1, mac_str);
    sal_strcat(file_name2, mac_str);
#endif

    if(r < init_param->repeat)
    {
        r = init_param->repeat;
    }
  
#if AGGRESSIVE_ALLOC_DEBUG_TESTING
    if ( (init_param->is_resources_check) && (init_param->warmboot) ) { 
        cli_out("Error, Warmboot test mode is not support with Resource-check.\n"); 
        rv = BCM_E_PARAM; 
        goto done; 
    }  

    if (init_param->is_resources_check)
    {
        if(r > 2) {
            cli_out("Error, resources leak check always does two iterations.\n");
        }
        r = 2;
    }
    else
    {
        if (init_param->aggressive_num_to_test != 0)
        {
            cli_out("Error, Can not specify AggressiveNumToTest without Resource-check.\n"); 
            rv = BCM_E_PARAM; 
            goto done; 
        }
        if (init_param->aggressive_alloc_debug_testing_keep_order != 0)
        {
            cli_out("Error, Can not specify AggressiveKeepOrder without Resource-check.\n"); 
            rv = BCM_E_PARAM; 
            goto done; 
        }
        if (init_param->aggressive_long_free_search != 0)
        {
            cli_out("Error, Can not specify AggressiveLongFreeSearch without Resource-check.\n"); 
            rv = BCM_E_PARAM; 
            goto done; 
        }

    }
#endif

#if ((defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT))
    /*currently test mode is relevant only for arad*/
    if (SOC_IS_SAND(unit)) {
        if(!init_param->warmboot) {
            /*Disable Warmboot test mode while tr 141 is running not in wb mode*/
            _DCMN_BCM_WARM_BOOT_API_TEST_OVERRIDE_WB_TEST_MODE(unit);
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    for(i = 0; i < r; i++)
    {   
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)        
        if (init_param->partial_init != 0) {
            GEN_ERR_TEST_SETUP(unit);
    		
            if (init_param->fake_soc_start != 0) {
                loop = ((init_param->fake_soc_range != 0) ? init_param->fake_soc_range : 1);   
                loop = (loop << 1) + 1;
            } else if (init_param->fake_bcm_start != 0) {
                loop = ((init_param->fake_bcm_range != 0) ? init_param->fake_bcm_range : 1); 
                loop = (loop << 1) + 1;
            } else {
                cli_out("Error, partial deinit test must contain the soc or bcm parameter.\n");
                rv = BCM_E_PARAM; 
                goto done;
            }
    	  } else {
    	    loop = 1;
        }
        GEN_ERR_CNT_SET(unit, GEN_ERR_TYPE_SOC, 0);
        GEN_ERR_CNT_SET(unit, GEN_ERR_TYPE_BCM, 0);
        for (j = 0; j < loop; j++) {
            if (init_param->partial_init != 0) {                
                if ((j % 2) == 0) {
                    if ((j + 1) != loop) {                    
                        if (init_param->fake_soc_start != 0) {
                            compare = init_param->fake_soc_start + (j/2);
                            GEN_ERR_CMP_SET(unit, GEN_ERR_TYPE_SOC, compare);                            
                        } else if (init_param->fake_bcm_start != 0) {
                            compare = init_param->fake_bcm_start + (j/2);
                            GEN_ERR_CMP_SET(unit, GEN_ERR_TYPE_BCM, compare);
                        }
                        partial_deinit = 1;
                        partial_init = 1;
                        cli_out("\nInit Deinit Tests - Partial deinit: %d\n", compare);
                        GEN_ERR_TEST_SET(unit, init_param->partial_init);                           
                    } else {
                        partial_deinit = 0;
                        partial_init = 0;
                        cli_out("\nInit Deinit Tests - Partial deinit cleanup\n");
                        GEN_ERR_TEST_SET(unit, GEN_ERR_TEST_CLEANUP);
                    }
                } else {
                    partial_deinit = 1;
                    partial_init = 0;
                    cli_out("\nInit Deinit Tests - Partial deinit check: %d\n", compare);
                    GEN_ERR_TEST_SET(unit, GEN_ERR_TEST_NONE);
                }
            } else {
                partial_deinit = 0;
                partial_init = 0;
                cli_out("\nInit Deinit Tests - Standard deinit\n");
                GEN_ERR_TEST_SET(unit, GEN_ERR_TEST_NONE);
            }
                
            GEN_ERR_RUN_SET(unit, GEN_ERR_TEST_DEINIT);
#endif
            /* unit not attached, directly do the init */
            if (SOC_CONTROL(unit) == NULL) {
                goto init;
            }

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
            if (SOC_IS_ARAD(unit) || SOC_IS_DFE(unit)) {
                if (init_param->modid == -1 && init_param->no_deinit == 1) {
                    init_param->modid = 0;
                    init_param->base_modid = 0;
                }

                if(!init_param->no_init && !init_param->no_deinit && !init_param->no_appl ) {
                    if (init_param->modid == -1) {
                        cli_out("%d: Stk modid get.\n", unit);
                        /* if warmboot API test, should skip it before/after API caller function */
#if (defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
                        soc_dcmn_wb_test_mode_get(unit, &warmboot_test_mode_enable);
                        if (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API == warmboot_test_mode_enable){
                            soc_dcmn_wb_test_mode_set(unit, 0);
                        }
#endif
                        rv = bcm_stk_modid_get(unit, &init_param->modid); 
#if (defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
                        if (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API == warmboot_test_mode_enable){
                            soc_dcmn_wb_test_mode_set(unit, _DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API);
                        }
#endif
                        if (rv < 0){
                            cli_out("Error, in bcm_stk_modid_get:\n");
                            /* Intentional check if partial deinit is 0,goto done*/
                            /* coverity[dead_error_line] */  
                            DEINIT_IF_ERR_EXIT(partial_deinit);
                        }
                    }
                }
            }
#endif

#if defined(BCM_WARM_BOOT_SUPPORT)
            if (init_param->warmboot) {
                if (!init_param->no_sync) {
                    /* detach the DMA and BG thread of CRPS */                       
                    if(!SOC_IS_DFE(unit)) {
                        /* get the status of BG thread */
                        rv = bcm_switch_service_get(unit, bcmServiceCounterCollection, &bcm_switch_service_config);
                        if (rv < 0) {
                            cli_out("ERROR: in bcm_switch_service_get-bcmServiceCounterCollection, failed to sync, rv= %d.\n", rv);
                            if(partial_deinit == 0) {
                                goto done;
                            }
                        }           
                        /* detach the DMA and BG thread of CRPS. if active - disable it */                       
                        /* It is made in sync mode only, to insure that new counters will not be collected between sync and deinit, */
                        /* otherwise, the counters that will be collected in that period will get lost */
                        /* it is user responsibility to enable it back after init warm-boot */  
                        if(bcm_switch_service_config.enabled == bcmServiceStateEnabled)
                        {
                            bcm_switch_service_config.enabled = bcmServiceStateDisabled;
                            rv = bcm_switch_service_set(unit, bcmServiceCounterCollection, &bcm_switch_service_config);
                            if (rv < 0) {
                                cli_out("ERROR: in bcm_switch_service_set-bcmServiceCounterCollection, failed to sync, rv= %d.\n", rv);
                                if(partial_deinit == 0) {
                                    goto done;
                                }
                            }  
                            /* hold the status as read in "get" API */
                            bcm_switch_service_config.enabled = bcmServiceStateEnabled;
                        }

                    }                    
                    /* sync the software state to external storage*/                    
                    cli_out("%d: Sync sw state to persistent storage.\n", unit);

                    rv = bcm_switch_control_set(unit, bcmSwitchControlSync, 1);
                    if (rv < 0) {
                        cli_out("ERROR: in bcm_switch_control_set-bcmSwitchControlSync, failed to sync, rv= %d.\n", rv);
                        if (rv < 0) {
                            if(partial_deinit == 0) {
                                goto done;
                            }
                        }
                    }
                } else {
#if defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030)

                    if (SOC_DPP_IS_ELK_ENABLE(unit))
                    {
                        rv = arad_kbp_sync(unit);
                        if (rv < 0) {
                            cli_out("ERROR: in arad_kbp_sync, rv= %d.\n", rv);
                            if (rv < 0) {
                                if(partial_deinit == 0) {
                                    goto done;
                                }
                            }
                        }
                    }
#if defined(BCM_88675_A0)
                    if (JER_KAPS_ENABLE(unit)) {
                        rv = jer_kaps_sync(unit);
                        if (rv < 0) {
                            cli_out("ERROR: in jer_kaps_sync, rv= %d.\n", rv);
                            if (rv < 0) {
                                if(partial_deinit == 0) {
                                    goto done;
                                }
                            }
                        }
                    }
#endif
#endif
                }
#if defined(BCM_PETRA_SUPPORT)
                if (SOC_IS_DPP(unit) && !(init_param->no_dump == 1)) {
                    /* perform software dump (1st dump prior to reboot) */
                    cli_out("%d: Dumping SW State Data Block to file.\n", unit);
                    shr_sw_state_block_dump(unit, TEST_DUMP_LEVEL(init_param->no_dump), file_name1, "w");
                    if (init_param->engine_dump == 1) {
                        cli_out("%d: Dumping WB Engine variables to file.\n", unit);
                        soc_wb_engine_dump(unit, 0, 0, -1, -1, 0, file_name1, "a");
                    }
                    /*sw_state_verbose[unit] = 1;*/
                }
#endif
            }
#endif 
            cli_out("%d: loop=%d (out of %d).\n", unit, i + 1, r);

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
            if (!init_param->no_deinit) {

                rv = appl_dcmn_deinit(unit, init_param);
                if (rv < 0) {
                    /* Intentional check if partial deinit is 0,goto done*/
                    /* coverity[dead_error_line] */
                    DEINIT_IF_ERR_EXIT(partial_deinit);
                }
            }
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */
    init:
            
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)
             GEN_ERR_RUN_SET(unit, GEN_ERR_TEST_INIT);
#endif


#if AGGRESSIVE_ALLOC_DEBUG_TESTING
            if (init_param->is_resources_check) {
                rv = init_aggressive_alloc_debug_testing(i);
                if (rv < 0) {
                    cli_out("Error: init_aggressive_alloc_debug_testing () Failed:\n");
                }
                set_aggressive_num_to_test(init_param->aggressive_num_to_test);
                set_aggressive_alloc_debug_testing_keep_order(init_param->aggressive_alloc_debug_testing_keep_order);
                set_aggressive_long_free_search(init_param->aggressive_long_free_search);
                if (i == 1) {
                    /* There is no need to init in the second loop */
                    goto done;
                }
            }
#endif
            if (init_param->rc_load) {
                rv = diag_rc_load(unit);
            } 
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
            else if (!init_param->no_init) {
                rv = appl_dcmn_init(unit, init_param);
                if (rv < 0) {
                   INIT_IF_ERR_EXIT(partial_init);
                }
            }
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#if defined(BCM_WARM_BOOT_SUPPORT)
            if(init_param->warmboot) {
#if defined(BCM_PETRA_SUPPORT)
                if (SOC_IS_DPP(unit) && !(init_param->no_dump == 1)) {
                    /* perform software dump (2nd dump after reboot) */
                    cli_out("%d: Dumping SW State Data Block to file.\n", unit);
                    shr_sw_state_block_dump(unit, TEST_DUMP_LEVEL(init_param->no_dump), file_name2, "w");
                    if (init_param->engine_dump == 1) {
                        cli_out("%d: Dumping WB Engine variables to file.\n", unit);
                        soc_wb_engine_dump(unit, 0, 0, -1, -1, 0, file_name2, "a");
                    }
                    /*sw_state_verbose[unit] = 0;*/

                    /*diff the two files*/
#ifndef NO_FILEIO
                    cli_out("%s(): Diffing files %s with %s:\n", FUNCTION_NAME(), file_name1, file_name2);
                    rv = diff_two_files(unit, file_name1, file_name2);
                    if (rv < 0) {
                       goto done;
                    }
#else /*NO_FILEIO*/
                    cli_out("%s(): Can't diff files as NO_FILEIO is defined (you can diff %s with %s externally).\n", FUNCTION_NAME(), file_name1, file_name2);
#endif /*NO_FILEIO*/
                }
#endif /*BCM_PETRA_SUPPORT*/
                /* turn off warmboot flag in case not already turned off because of INCLUDE_INTR */
                soc_state[unit] = 0;
            }
#endif

#if defined(BCM_WARM_BOOT_SUPPORT)
            if (init_param->warmboot) {
                if (!init_param->no_sync) {
                    /* attach back the DMA and BG thread of CRPS */                       
                    if(!SOC_IS_DFE(unit)) {
                        if(bcm_switch_service_config.enabled == bcmServiceStateEnabled)
                        {
                            rv = bcm_switch_service_set(unit, bcmServiceCounterCollection, &bcm_switch_service_config);
                            if (rv < 0) {
                                cli_out("ERROR: in bcm_switch_service_set-bcmServiceCounterCollection, failed to sync, rv= %d.\n", rv);
                                if(partial_deinit == 0) {
                                    goto done;
                                }
                            }  
                        }                        
                    }
                }
            }
#endif

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)
        }
        if (init_param->partial_init != 0) {
            GEN_ERR_TEST_CLEANUP(unit);
        }
#endif
    }

#if ((defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT))
    if (SOC_IS_SAND(unit)) {
        if(!init_param->warmboot) {
             /*allow wb test when tr 141 finished running*/
            _DCMN_BCM_WARM_BOOT_API_TEST_RETRACT_OVERRIDEN_WB_TEST_MODE(unit);
        }
    }
#endif 

done:

    if (rv < 0) {
        if(init_param->warmboot) {
            test_error(unit, "Warmboot test Failed!!!\n");
        }else {
            test_error(unit, "Deinit-Init test Failed!!!\n");
        }
        return rv;
    }

    return rv;
}

int
init_deinit_test(int unit, args_t *a, void *p)
{
    appl_dcmn_init_param_t *init_param = p;
    int rv = 0;
    unsigned long alloc_bytes_count_on_start=0,free_bytes_count_on_start=0;
    if (do_error_on_leak_test(init_param)) {
        unsigned long alloc_bytes_count;
        unsigned long free_bytes_count;

        /* only deinit */
        init_param->no_init = 1;
        init_param->no_deinit = 0;
        rv = init_deinit_test_internal(unit, a, p);

        /* taking memory snapshot */
        sal_get_alloc_counters(&(alloc_bytes_count_on_start),&(free_bytes_count_on_start)); 

        /* deinit-init only */
        init_param->no_init = 0;
        init_param->no_deinit = 1;
        rv = init_deinit_test_internal(unit, a, p);

        /* only deinit */
        init_param->no_init = 1;
        init_param->no_deinit = 0;
        rv = init_deinit_test_internal(unit, a, p);

        /* cehck that deinit got me to same memory allocation */

        sal_get_alloc_counters(&alloc_bytes_count,&free_bytes_count); 

        /* finish test with init*/
        init_param->no_init = 0;
        init_param->no_deinit = 1;
        rv = init_deinit_test_internal(unit, a, p);

        if ((alloc_bytes_count - alloc_bytes_count_on_start) != (free_bytes_count - free_bytes_count_on_start)) {
            cli_out("There is a Mem Leak in tr 141 allocated %lu bytes,  freed %lu bytes, leaked %lu bytes\n",
                    (alloc_bytes_count - alloc_bytes_count_on_start),
                    (free_bytes_count - free_bytes_count_on_start),
                    (alloc_bytes_count - alloc_bytes_count_on_start) - (free_bytes_count - free_bytes_count_on_start)
                    );
            rv = BCM_E_FAIL;
        }

    } else {
         return init_deinit_test_internal(unit, a, p);
    }
    return rv;
}


int
init_deinit_test_init(int unit, args_t *a, void **p)
{
    appl_dcmn_init_param_t *init_param;
    init_param = &init_deinit[unit];

    sal_memset(init_param, 0x0, sizeof(appl_dcmn_init_param_t));

    /* set default modid & base_modid with non-valid values, which denotes */
    /* that previous values should be taken if those params were not supplied */
    init_param->modid = -1;
    init_param->base_modid = -1;

    *p = init_param;
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
    return   appl_dcmn_init_usage_parse(unit, a, init_param);
#else
    return 0;
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

}

int
init_deinit_test_done(int unit, void *p)
{
    cli_out("Init Deinit Test Done\n");
    return 0;
}



#endif
