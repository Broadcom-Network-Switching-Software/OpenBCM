/** \file test_dnxc_init_deinit.c
 * 
 * DNX init deinit test - Testing DNXC init deinit procedure 
 * For additional details about Device Data Component goto 'appl_ref_init_deinit.h' 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA

/*
 * INCLUDE FILES:
 * {
 */
/*appl*/
#include <appl/diag/test.h>
#include <appl/diag/diag.h>
#include <appl/diag/parse.h>
#include <appl/diag/shell.h>
/*bcm*/
#include <bcm/switch.h>
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
/*soc*/
#include <soc/drv.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/sw_state_features.h>

#ifdef BCM_WARM_BOOT_API_TEST
#include <soc/dnxc/dnxc_wb_test_utils.h>
#endif
/*sal*/
#include <sal/appl/sal.h>
#include <sal/appl/io.h>


/* for wb test */
#ifndef NO_FILEIO
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#endif /* NO_FILEIO */

#include <appl/reference/dnxc/appl_ref_init_deinit.h>
/*
 * }
 */
/*
* Definitions:
* {
*/
typedef struct test_dnxc_init_params {

    /** DNXC Init params */
    appl_dnxc_init_param_t appl_init_params;

    /** nof times to repeat the test */
    int repeat;

} test_dnxc_init_params_t;

STATIC test_dnxc_init_params_t test_init_params[SOC_MAX_NUM_DEVICES];



char test_init_deinit_usage[] = 
#ifdef COMPILER_STRING_CONST_LIMIT
"Warmboot Test Usage:\n"
"  Repeat=<value>, NoInit=<value>, NoDeinit=<value> Warmboot=<value>, ResoucesLeakCheck=<value>.\n"
#else
"Warmboot Test Usage:\n"
"  Repeat=<value>         the test will loop as many times as stated. (default=1)\n"
"  Warmboot=<value>       1: The test will perform Warmboot sequence.\n"
"  NoInit=<value>         1: The test will not Init the Chip. \n"
"  NoDeinit=<value>       1: The test will not deinit the Chip.\n"
#if AGGRESSIVE_ALLOC_DEBUG_TESTING
"  ResoucesLeakCheck=<value>       1: Test to verify that all the allocation done free..\n"
"  AggressiveNumToTest=<0/value>   Number of allocation entries to scan each time a memory allocation is required\n"
"  AggressiveKeepOrder=<0/1>       1: When 'free' is applied, corresponding entry in Allocs[] is deleted and all following pushed down\n"
"  AggressiveLongFreeSearch=<0/1>  1: When 'free' is applied, keep searching Allocs[] for another match\n"
#endif
#endif
;



/*
 * }
 */

static void test_dnxc_init_port_info_dump(int unit) {

    int i = 0;
    int j = 0;
    DNX_SW_STATE_PRINT(
                unit,
                "port info (non-sw-state variables):\n");
    for (i=0; i < SOC_MAX_NUM_PORTS; i++) {

        DNX_SW_STATE_PRINT(unit, "port_l2p_mapping[%d]:", i);
        dnx_sw_state_print_int(unit, &(SOC_INFO(unit).port_l2p_mapping[i]));

        DNX_SW_STATE_PRINT(unit, "port_p2l_mapping[%d]:", i);
        dnx_sw_state_print_int(unit, &(SOC_INFO(unit).port_p2l_mapping[i]));

        DNX_SW_STATE_PRINT(unit, "port_num_lanes[%d]:", i);
        dnx_sw_state_print_int(unit, &(SOC_INFO(unit).port_num_lanes[i]));

        DNX_SW_STATE_PRINT(unit, "port_type[%d]:", i);
        dnx_sw_state_print_int(unit, &(SOC_INFO(unit).port_type[i]));

        DNX_SW_STATE_PRINT(unit, "port_name[%d]:%s\n", i, SOC_INFO(unit).port_name[i]);
        DNX_SW_STATE_PRINT(unit, "port_name[%d]:%s\n", i, SOC_INFO(unit).port_name_alter[i]);
    }

    for (i=0; i < SOC_MAX_NUM_BLKS; i++) {
        DNX_SW_STATE_PRINT(unit, "block_port[%d]:", i);
        dnx_sw_state_print_int(unit, &(SOC_INFO(unit).block_port[i]));
    }

    DNX_SW_STATE_PRINT(unit, "fabric_logical_port_base:");
    dnx_sw_state_print_int(unit, &(SOC_INFO(unit).fabric_logical_port_base));

    for (j=0; j < SOC_MAX_NUM_BLKS; j++) {
        DNX_SW_STATE_PRINT(unit, "block_bitmap pbmp members[%d]:", j);
        PBMP_ITER(SOC_INFO(unit).block_bitmap[j], i)
        {
            DNX_SW_STATE_PRINT(unit, "%d,", i);
        }
        DNX_SW_STATE_PRINT(unit, "\n");
    }

    DNX_SW_STATE_PRINT(unit, "cmic_bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).cmic_bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    DNX_SW_STATE_PRINT(unit, "all.bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).all.bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    DNX_SW_STATE_PRINT(unit, "port.bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).port.bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    DNX_SW_STATE_PRINT(unit, "tdm.bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).tdm.bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    DNX_SW_STATE_PRINT(unit, "sfi.bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).sfi.bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    DNX_SW_STATE_PRINT(unit, "rcy.bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).rcy.bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    DNX_SW_STATE_PRINT(unit, "cde.bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).cde.bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    DNX_SW_STATE_PRINT(unit, "cc.bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).cc.bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    DNX_SW_STATE_PRINT(unit, "ce.bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).ce.bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    DNX_SW_STATE_PRINT(unit, "le.bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).le.bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    DNX_SW_STATE_PRINT(unit, "xl.bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).xl.bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    DNX_SW_STATE_PRINT(unit, "xe.bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).xe.bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    DNX_SW_STATE_PRINT(unit, "il.bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).il.bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    DNX_SW_STATE_PRINT(unit, "ether.bitmap pbmp members:");
    PBMP_ITER(SOC_INFO(unit).ether.bitmap, i)
    {
        DNX_SW_STATE_PRINT(unit, "%d,", i);
    }
    DNX_SW_STATE_PRINT(unit, "\n");

    return;
}

int
test_dnxc_init_params_parse(int unit, args_t *a, test_dnxc_init_params_t *test_params)
{
    parse_table_t pt;
    int rv;

    parse_table_init(unit, &pt);

    /** parse dnxc appl init params */
    {
        parse_table_add(&pt, "NoDeinit", PQ_INT, (void *) 0,
          &(test_params->appl_init_params.no_deinit), NULL);

        parse_table_add(&pt, "NoInit", PQ_INT, (void *) 0,
          &(test_params->appl_init_params.no_init), NULL);

        parse_table_add(&pt, "Warmboot", PQ_INT, (void *) 0,
          &(test_params->appl_init_params.warmboot), NULL);

        parse_table_add(&pt, "NoDump", PQ_INT, (void *) 0,
          &(test_params->appl_init_params.no_dump), NULL);

        parse_table_add(&pt, "No_Clean_up", PQ_INT, (void *) 0,
          &(test_params->appl_init_params.no_clean_up), NULL);
#if AGGRESSIVE_ALLOC_DEBUG_TESTING
        parse_table_add(&pt, "ResoucesLeakCheck", PQ_INT, (void *) 0,
          &(test_params->appl_init_params.is_resources_check), NULL);
        parse_table_add(&pt, "AggressiveNumToTest", PQ_INT, (void *) 0,
          &(test_params->appl_init_params.aggressive_num_to_test), NULL);
        parse_table_add(&pt, "AggressiveKeepOrder", PQ_INT, (void *) 0,
          &(test_params->appl_init_params.aggressive_alloc_debug_testing_keep_order), NULL);
        parse_table_add(&pt, "AggressiveLongFreeSearch", PQ_INT, (void *) 0,
          &(test_params->appl_init_params.aggressive_long_free_search), NULL);
#endif
    }

    /** parse dnxc test params */
    {
        parse_table_add(&pt, "Repeat", PQ_INT, (void *) 1,
          &(test_params->repeat), NULL);
    }


    rv = parse_arg_eq(a, &pt);
    if (rv < 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - failed parsing test arguments.\n")));
        return 1; 
    }

    /*
     * make sure that no extra args were recieved as input
     */
    if (ARG_CNT(a) != 0)
    {
        cli_out("%s: extra options starting with \"%s\"\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n", test_init_deinit_usage);

        return 1;
    }

    /*
     * This is used to free memory allocated for parse_table_init
     */
    parse_arg_eq_done(&pt);

    return 0;
}



/**
 * \brief - Main init deinit test procedure
 */
int
test_dnxc_init_test(int unit, args_t *a, void *p)
{
    int rv, i;
    test_dnxc_init_params_t *test_params;
#if !defined(NO_FILEIO) && defined(DNX_SW_STATE_DIAGNOSTIC)
    char cwd[1024] = {0};
    char pre_reboot_file_name[1024] = {0};
    char post_reboot_file_name[1024] = {0};
    char bshell_command[1024] = {0};
    char diff_command[1024] = {0};
    char reboot_file_name_suffix [32] = {0};
    char clean_command[1027] = {0};
    sal_time_t timestamp = 0;

#endif

    test_params = &test_init_params[unit];



    /** iterate according to "repeat" test parameter */
    for (i = 0; i < test_params->repeat; i++)
    {
#if !defined(NO_FILEIO)
        if (test_params->appl_init_params.warmboot) {
            if(dnxc_sw_state_is_warmboot_supported_get(unit) == FALSE) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - couldn't run warmboot test. warmboot feature is disabled by soc property.\n")));
                return CMD_FAIL;       /** fail */
            } else {

                cli_out("%d: Sync sw state to persistent storage.\n", unit);

                rv = bcm_switch_control_set(unit, bcmSwitchControlSync, BCM_SWITCH_CONTROL_SYNC_FULL);

                if (rv != 0)
                {
                    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - dnxc init deinit test failed - sync failed.\n")));
                    return CMD_FAIL;       /** fail */
                }
#if defined(DNX_SW_STATE_DIAGNOSTIC)
                if (!test_params->appl_init_params.no_dump) {
                    if (getcwd(cwd, sizeof(cwd)) == NULL) {
                        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - couldn't get current working directory.\n")));
                    }

                    sal_strncpy(pre_reboot_file_name, cwd, sizeof(cwd) - 1);
                    sal_strncpy(post_reboot_file_name, cwd, sizeof(cwd) - 1);
                    sal_strncat(pre_reboot_file_name, "/sw_state_pre_reboot", sizeof(pre_reboot_file_name) - strlen(pre_reboot_file_name) - 1);
                    sal_strncat(post_reboot_file_name, "/sw_state_post_reboot", sizeof(post_reboot_file_name) - strlen(post_reboot_file_name) - 1);
                    timestamp = sal_time();
                    sal_sprintf(reboot_file_name_suffix, "_%lu.txt", timestamp );
                    sal_strcat(pre_reboot_file_name, reboot_file_name_suffix);
                    sal_strcat(post_reboot_file_name, reboot_file_name_suffix);


                    cli_out("%d: Dumping SW State Data Block to file - sw_state_pre_reboot_%lu.txt.\n", unit, timestamp);

                    sal_strncpy(bshell_command, "swstate config dump file_path=", sizeof(bshell_command) - 1);
                    sal_strncat(bshell_command, pre_reboot_file_name, sizeof(bshell_command) - strlen(bshell_command) - 1);
                    sh_process_command(unit, bshell_command);
                    
                    sal_strncpy(bshell_command, "swstate dump", sizeof(bshell_command) - 1);
                    sh_process_command(unit, bshell_command);

                    test_dnxc_init_port_info_dump(unit);
                    
                    sal_strncpy(bshell_command, "swstate config dump file_unset", sizeof(bshell_command) - 1);
                    sh_process_command(unit, bshell_command);
                }
#endif /* DNX_SW_STATE_DIAGNOSTIC */
            }
        }
#endif /* NO_FILEIO */

#ifdef BCM_WARM_BOOT_API_TEST
        /* temporary disable WB test mode */
        dnxc_wb_api_test_counter_increase(unit);

        /*
         * Mark the deinit-init sequence was run during WB test.
         */
        if (test_params->appl_init_params.warmboot)
        {
            dnxc_wb_deinit_init_during_wb_test_set(unit, TRUE);
        }
#endif
#if AGGRESSIVE_ALLOC_DEBUG_TESTING
    if ( (test_params->appl_init_params.is_resources_check) && (test_params->appl_init_params.warmboot) ) { 
        cli_out("Error, Warmboot test mode is not support with Resource-check.\n"); 
        return CMD_FAIL;       /** fail */ 
    }  

    if (test_params->appl_init_params.is_resources_check)
    {
        if(test_params->repeat > 2) {
            cli_out("Error, resources leak check always does two iterations.\n");
        }
        test_params->repeat = 2;
    }
    else
    {
        if (test_params->appl_init_params.aggressive_num_to_test != 0)
        {
            cli_out("Error, Cannot specify AggressiveNumToTest without specifying Resource-check.\n"); 
            return CMD_FAIL;       /** fail */ 
        }
        if (test_params->appl_init_params.aggressive_alloc_debug_testing_keep_order != 0)
        {
            cli_out("Error, Cannot specify AggressiveKeepOrder without specifying Resource-check.\n"); 
            return CMD_FAIL;       /** fail */ 
        }
        if (test_params->appl_init_params.aggressive_long_free_search != 0)
        {
            cli_out("Error, Cannot specify AggressiveLongFreeSearch without specifying Resource-check.\n"); 
            return CMD_FAIL;       /** fail */ 
        }
    }

    test_params->appl_init_params.loop_id = i;

#endif
        /** Run init deinit application sequence */
        rv = appl_dnxc_init_step_list_run(unit, &(test_params->appl_init_params));

#ifdef BCM_WARM_BOOT_API_TEST
        /* re-enable WB test mode */
        dnxc_wb_api_test_counter_decrease(unit);

        /*
         * Restore the deinit-init sequence run during WB test flag.
         */
        if (test_params->appl_init_params.warmboot)
        {
            dnxc_wb_deinit_init_during_wb_test_set(unit, FALSE);
        }
#endif
        if (rv != 0)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - dnxc init deinit test failed.\n")));
            return CMD_FAIL;       /** fail */
        }
#if !defined(NO_FILEIO) && defined(DNX_SW_STATE_DIAGNOSTIC)
        if (test_params->appl_init_params.warmboot && !test_params->appl_init_params.no_dump) {
            cli_out("%d: Dumping SW State Data Block to file - sw_state_post_reboot_%lu.txt.\n", unit, timestamp);

            sal_strncpy(bshell_command, "swstate config dump file_path=", sizeof(bshell_command) - 1);
            sal_strncat(bshell_command, post_reboot_file_name, sizeof(bshell_command) - strlen(bshell_command) - 1);
            sh_process_command(unit, bshell_command);
            
            sal_strncpy(bshell_command, "swstate dump", sizeof(bshell_command) - 1);
            sh_process_command(unit, bshell_command);

            test_dnxc_init_port_info_dump(unit);

            sal_strncpy(bshell_command, "swstate config dump file_unset", sizeof(bshell_command) - 1);
            sh_process_command(unit, bshell_command);

            strncpy(diff_command, "diff -q ", sizeof(diff_command));
            strncat(diff_command, pre_reboot_file_name, sizeof(diff_command) - strlen(diff_command) - 1);
            strncat(diff_command, " ", sizeof(diff_command) - 1);
            strncat(diff_command, post_reboot_file_name, sizeof(diff_command) - strlen(diff_command) - 1);
            rv = system(diff_command);
            if (rv != 0)
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - dnxc init deinit test failed.\n"
                    "directories sw_state_pre_reboot sw_state_post_reboot differ.\n")));

                /* diff again but this time output the diff to screen */
                sal_memset(diff_command, 0, sizeof(diff_command));
                strncpy(diff_command, "diff -u ", sizeof(diff_command));
                strncat(diff_command, pre_reboot_file_name, sizeof(diff_command) - strlen(diff_command) - 1);
                strncat(diff_command, " ", sizeof(diff_command) - 1);
                strncat(diff_command, post_reboot_file_name, sizeof(diff_command) - strlen(diff_command) - 1);
                system(diff_command);

                return CMD_FAIL;       /** fail */
            }
            if(!test_params->appl_init_params.no_clean_up)
            {
                /*delete pre-reboot.txt*/
                sal_memset(clean_command, 0, sizeof(clean_command));
                strncpy(clean_command, "rm ", sizeof(clean_command));
                strncat(clean_command, pre_reboot_file_name, sizeof(clean_command) - strlen(clean_command) - 1);
                system(clean_command);
                /*delete post-reboot.txt*/
                sal_memset(clean_command, 0, sizeof(clean_command));
                strncpy(clean_command, "rm ", sizeof(clean_command));
                strncat(clean_command, " ", sizeof(clean_command) - 1);
                strncat(clean_command, post_reboot_file_name, sizeof(clean_command) - strlen(clean_command) - 1);
                system(clean_command);
            }


        }
#endif /* NO_FILEIO && DNX_SW_STATE_DIAGNOSTIC */
    }



    return CMD_OK;
}

/**
 * \brief - Init function for init deinit test procedure 
 * This function parses test arguments 
 */
int
test_dnxc_init_test_init(int unit, args_t *a, void **p)
{
    int rv;
    test_dnxc_init_params_t *test_params;

    test_params = &test_init_params[unit];

    cli_out("DNXC Init-Deinit Test - Start\n");

    sal_memset(test_params, 0x0, sizeof(test_dnxc_init_params_t));

    rv = test_dnxc_init_params_parse(unit, a, test_params);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - failed parsing arguments.\n")));
        return CMD_FAIL;       /** fail */
    }

    return CMD_OK;
}

/**
 * \brief - Done function for init deinit test procedure 
 */
int
test_dnxc_init_test_done(int unit, void *p)
{
    cli_out("Init Deinit Test Done\n");

    return CMD_OK;
}

#endif /* defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */
