/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Built-in Self Test for DNXF fabric devices (Snake test)
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_FABRIC

#include <soc/defs.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm_int/control.h>

#include <appl/diag/parse.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#ifdef BCM_DNXF_SUPPORT

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric_cell_snake_test.h>
#include <soc/dnxf/cmn/dnxf_diag.h>

/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>

/*Fabric snake test action flags*/
#define TEST_DNXF_FABRIC_SNAKE_PREPARE_ONLY      (0x1)
#define TEST_DNXF_FABRIC_SNAKE_RUN_ONLY          (0x2)
#define TEST_DNXF_FABRIC_SNAKE_ALL               (TEST_DNXF_FABRIC_SNAKE_RUN_ONLY | TEST_DNXF_FABRIC_SNAKE_PREPARE_ONLY)


char test_dnxf_fabric_snake_usage[] =
"Fabric snake test usage: \n"
"   Loopback=<loopback_name>            - valid <loopback_name> are: PHY (default), MAC, EXTERNAL\n"
"   Action=<action_name>                - valid <action_name> are: PREPARE_ONLY, RUN_ONLY, ALL (default)\n"
"   RunMode=<run_mode_name>             - valid <run_mode_name> are: INFINITE, STOP, NORMAL (default)\n"
"This test requires enabling all ports\n"
;

typedef struct test_dnxf_fabric_snake_params_s
{
    uint32                                  loopback_flag;
    uint32                                  snake_test_prepare_extra_flags;
    uint32                                  action_flags;
    uint32                                  snake_test_run_flags;
    soc_dnxf_fabric_cell_snake_test_results_t    res;
} test_dnxf_fabric_snake_params_t;


static void
test_dnxf_fabric_snake_test_result_print(
    int unit,
    test_dnxf_fabric_snake_params_t * test_params)
{
    int i,
    rv;
    soc_dnxf_fabric_cell_snake_test_results_t results;
    results = test_params->res;
    cli_out("Fabric snake test results\n");
    cli_out("-------------------------\n");

    /*print test status*/
    if ((test_params->snake_test_run_flags & SOC_DNXF_SNAKE_INFINITE_RUN) && !results.test_failed) {
        cli_out(" Snake is running... \n In order to stop the snake and check the result use 'tr 131 RunMode=STOP' \n");
    } else {
        cli_out("Result: %s\n", results.test_failed ? "fail" : "pass");
    }

    /*print failure stages*/
    if (results.failure_stage_flags)
    {
        cli_out("Fail stages (0x%x):\n", results.failure_stage_flags);
        for (i=0; soc_dnxf_diag_flag_str_fabric_cell_snake_test_stages[i].name != NULL; i++)
        {
            if (soc_dnxf_diag_flag_str_fabric_cell_snake_test_stages[i].flag & results.failure_stage_flags)
            {
                cli_out("%s\n",  soc_dnxf_diag_flag_str_fabric_cell_snake_test_stages[i].name);
            }
        }
        cli_out("\n");
    }

    /*print interrupts*/
    if (!SHR_BITNULL_RANGE(results.interrupts_status, 0, 32*SOC_REG_ABOVE_64_MAX_SIZE_U32))
    {
        const soc_dnxf_diag_flag_str_t *interrupts_names;

        cli_out("Interrupts (0x%0x, 0x%0x, 0x%0x, 0x%0x): ", results.interrupts_status[3], results.interrupts_status[2], results.interrupts_status[1], results.interrupts_status[0]);

        rv = soc_dnxf_diag_fabric_cell_snake_test_interrupts_name_get(unit, &interrupts_names);
        if (interrupts_names == NULL || rv != BCM_E_NONE)
        {
            return;
        }
        for (i=0; interrupts_names[i].name != NULL; i++)
        {
            if (SHR_BITGET(results.interrupts_status, interrupts_names[i].flag))
            {
                cli_out("%s ",  interrupts_names[i].name);
            }
        }
        cli_out("\n");
    }

    cli_out("-------------------------\n\n");

}

int
test_dnxf_fabric_snake_test_init(int unit, args_t *a, void **p)
{
    char *loopback_str,
         *action_str,
         *run_mode_str;
    parse_table_t pt;
    test_dnxf_fabric_snake_params_t *test_params;
    int rv = 0;
    int strnlen_loopback_str;
    int strnlen_action_str;
    int strnlen_run_mode_str;

    if (p == NULL)
    {
        return -1;
    }

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS))
    {
        cli_out("Invalid unit %d\n", unit);
        return -1;
    }

    /*Parse test params*/
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Loopback", PQ_STRING, "PHY", &loopback_str, NULL);
    parse_table_add(&pt, "Action", PQ_STRING, "ALL", &action_str, NULL);
    parse_table_add(&pt, "RunMode", PQ_STRING, "NORMAL", &run_mode_str, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",test_dnxf_fabric_snake_usage);
        rv = -1;
    } else if (ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",test_dnxf_fabric_snake_usage);
        rv = -1;
    }

    
    /*init test_dnxf_fabric_snake_params structure*/
    if ((test_params = sal_alloc(sizeof(test_dnxf_fabric_snake_params_t), "Fabric snake test tr")) == NULL)
    {
        cli_out("Failed to allocate memory\n");
        parse_arg_eq_done(&pt);
        return -1;
    }
    sal_memset(test_params, 0, sizeof(test_dnxf_fabric_snake_params_t));
    *p = test_params;

   /*fill test_dnxf_fabric_snake_params structure */
    /*Loopback config*/
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_loopback_str = sal_strnlen(loopback_str,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (! sal_strncasecmp(loopback_str, "PHY",strnlen_loopback_str) ) 
    {
        test_params->loopback_flag = SOC_DNXF_ENABLE_PHY_LOOPBACK;
    } else if (! sal_strncasecmp(loopback_str, "MAC",strnlen_loopback_str) ) 
    {
        test_params->loopback_flag = SOC_DNXF_ENABLE_MAC_LOOPBACK;
    } else if (! sal_strncasecmp(loopback_str, "EXTERNAL",strnlen_loopback_str) )
    {
        test_params->loopback_flag = SOC_DNXF_ENABLE_EXTERNAL_LOOPBACK;
    } else {
        cli_out("Unknown Loopback value: %s\n",loopback_str);
        cli_out("%s\n",test_dnxf_fabric_snake_usage);
        rv = -1;
    }

    /*Action config*/
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_action_str = sal_strnlen(action_str,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (! sal_strncasecmp(action_str, "ALL", strnlen_action_str) ) 
    {
        test_params->action_flags = TEST_DNXF_FABRIC_SNAKE_ALL;
    } else if (! sal_strncasecmp(action_str, "PREPARE_ONLY", strnlen_action_str) ) 
    {
        test_params->action_flags = TEST_DNXF_FABRIC_SNAKE_PREPARE_ONLY;
    } else if (! sal_strncasecmp(action_str, "RUN_ONLY", strnlen_action_str) )
    {
        test_params->action_flags = TEST_DNXF_FABRIC_SNAKE_RUN_ONLY;
    } else {
        cli_out("Unknown Action value: %s\n",action_str);
        cli_out("%s\n",test_dnxf_fabric_snake_usage);
        rv = -1;
    }

    /*RunMode config*/
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_run_mode_str = sal_strnlen(run_mode_str,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (! sal_strncasecmp(run_mode_str, "NORMAL", strnlen_run_mode_str) ) 
    {
        test_params->snake_test_run_flags = 0x0;
    } else if (! sal_strncasecmp(run_mode_str, "INFINITE", strnlen_run_mode_str) ) 
    {
        test_params->snake_test_run_flags = SOC_DNXF_SNAKE_INFINITE_RUN;
    } else if (! sal_strncasecmp(run_mode_str, "STOP", strnlen_run_mode_str) )
    {
        test_params->snake_test_run_flags = SOC_DNXF_SNAKE_STOP_TEST;
    } else {
        cli_out("Unknown RunMode value: %s\n",run_mode_str);
        cli_out("%s\n",test_dnxf_fabric_snake_usage);
        rv = -1;
    }


    if (rv != -1)
    {
        cli_out("Fabric snake test params: \n");
        cli_out("------------------------- \n");
        cli_out("Loopback: %s\n", loopback_str);
        cli_out("Action: %s\n", action_str);
        cli_out("RunMode: %s\n", run_mode_str);
        cli_out("------------------------- \n\n");
    }

    parse_arg_eq_done(&pt);
    return rv;
}


int
test_dnxf_fabric_snake_test_done(int unit, void *p)
{
    test_dnxf_fabric_snake_params_t *test_params;
    int rv = 0;

    if (p == NULL)
    {
        return -1;
    }
    test_params = (test_dnxf_fabric_snake_params_t *) p;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS))
    {
        cli_out("Invalid unit %d\n", unit);
        rv = -1;
    }
    /*print results*/
    if (rv != -1)
    {
        /* coverity[overrun-call] */        
        test_dnxf_fabric_snake_test_result_print(unit, test_params);
    }

    if (test_params != NULL)
    {
        sal_free(test_params);
        p = test_params = NULL;
    }

    return rv;
}

/*
 * For DNXF - PHY loopback cannot be enabled with lane swap.
 * This function removes all lanes swaps in order to run snake test with PHY loopback
 */
static
shr_error_e test_dnxf_fabric_snake_test_lane_swap_port_clear_all(int unit)
{
    int lane_idx;
    bcm_pbmp_t detached_pbmp, attached_pbmp, originally_attached_pbmp;
    bcm_port_lane_to_serdes_map_t* lane_to_serdes_map = NULL;
    bcm_port_resource_t* resource = NULL;
    uint32 nof_lanes;

    SHR_FUNC_INIT_VARS(unit);

    lane_to_serdes_map = sal_alloc(sizeof(bcm_port_lane_to_serdes_map_t)*DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS, "lane_to_serdes_map");
    if (lane_to_serdes_map == NULL)
    {
        cli_out("lane_to_serdes_map allocation failed - unit %d\n", unit);
        SHR_IF_ERR_EXIT(_SHR_E_MEMORY);
    }
    sal_memset(lane_to_serdes_map, 0, sizeof(bcm_port_lane_to_serdes_map_t)*DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS);

    resource = sal_alloc(sizeof(bcm_port_resource_t)*DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS, "resource");
    if (resource == NULL)
    {
        cli_out("resource allocation failed - unit %d\n", unit);
        SHR_IF_ERR_EXIT(_SHR_E_MEMORY);
    }
    sal_memset(resource, 0, sizeof(bcm_port_resource_t)*DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS);

    /* get original resource of attached ports */
    PBMP_SFI_ITER(unit, lane_idx)
    {
        SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, lane_idx, &resource[lane_idx]));
    }

    /* get attached fabric ports pbmp */
    SOC_PBMP_ASSIGN(originally_attached_pbmp, PBMP_SFI_ALL(unit));

    /* detach attached fabric ports */
    SHR_IF_ERR_EXIT(bcm_port_detach(unit, originally_attached_pbmp, &detached_pbmp));

    /* configure one to one mapping on all existing fabric ports - both attached and detached */
    nof_lanes = dnxf_data_port.general.nof_links_get(unit);
    for (lane_idx = 0 ; lane_idx < nof_lanes ; lane_idx++)
    {
        lane_to_serdes_map[lane_idx].serdes_rx_id = lane_idx;
        lane_to_serdes_map[lane_idx].serdes_tx_id = lane_idx;
    }

    /* configure lane map */
    SHR_IF_ERR_EXIT(bcm_port_lane_to_serdes_map_set(unit, BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE, nof_lanes, lane_to_serdes_map));

    /* attach all fabric ports that were originally attached */
    SHR_IF_ERR_EXIT(bcm_port_probe(unit, originally_attached_pbmp, &attached_pbmp));

    sal_usleep(10000);

    /* restore original resource of the attached ports */
    PBMP_SFI_ITER(unit, lane_idx)
    {
        SHR_IF_ERR_EXIT(bcm_port_resource_set(unit, lane_idx, &resource[lane_idx]));
    }

    sal_usleep(10000);

    /* enable the attached ports */
    PBMP_SFI_ITER(unit, lane_idx)
    {
        SHR_IF_ERR_EXIT(bcm_port_enable_set(unit, lane_idx, 1));
    }

exit:
    if (lane_to_serdes_map != NULL)
    {
        sal_free(lane_to_serdes_map);
    }
    if (resource != NULL)
    {
        sal_free(resource);
    }
    SHR_FUNC_EXIT;
}

int
test_dnxf_fabric_snake_test(int unit, args_t *a, void *p)
{
    int rv;
    test_dnxf_fabric_snake_params_t *test_params;
    bcm_port_t port;
    int enabled;

    if (p == NULL)
    {
        return -1;
    }
    test_params = (test_dnxf_fabric_snake_params_t *) p;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS))
    {
        cli_out("Invalid unit %d\n", unit);
        test_params->res.test_failed = 1; /*make sure test marked as failed*/
        return  -1;
    }

    if ((test_params->action_flags & TEST_DNXF_FABRIC_SNAKE_PREPARE_ONLY) &&
        !(test_params->snake_test_run_flags & SOC_DNXF_SNAKE_STOP_TEST))
    {
        PBMP_SFI_ITER(unit, port)
        {
            rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_enable_get, (unit, port, &enabled));
            if (rv != BCM_E_NONE)
            {
                test_params->res.test_failed = 1; /*make sure test marked as failed*/
                return -1;
            }
            if (!enabled)
            {
                /* port_init_speed=-1 / disabled ports is not allowed */
                cli_out("unit %d: Invalid configuration. The test requires enabling all ports. Port %d is not enabled.\n", unit, port);
                test_params->res.test_failed = 1; /*make sure test marked as failed*/
                return -1;
            }
        }

        /* PHY Loopack - clear ports lane swap */
        if (test_params->loopback_flag == SOC_DNXF_ENABLE_PHY_LOOPBACK)
        {
            rv = test_dnxf_fabric_snake_test_lane_swap_port_clear_all(unit);
            if (rv != BCM_E_NONE)
            {
                test_params->res.test_failed = 1; /*make sure test marked as failed*/
                return -1;
            }

            /*
             * test_dnxf_fabric_snake_test_lane_swap_port_clear_all needs to detach and re-attach some of the ports.
             * Once the ports are re-attached they might accumulate CRC errors in their process of stabilizing.
             * This might confuse later on the user that the actual HW snake accumulated these CRC errors so we clear it
             */
            sal_sleep(2);
            PBMP_SFI_ITER(unit, port)
            {
                rv = bcm_stat_clear(unit, port);
                if (rv != BCM_E_NONE)
                {
                    test_params->res.test_failed = 1; /*make sure test marked as failed*/
                    return -1;
                }
            }
        }

        rv = soc_dnxf_cell_snake_test_prepare(unit, test_params->loopback_flag);
        if (rv != BCM_E_NONE)
        {
            test_params->res.test_failed = 1; /*make sure test marked as failed*/
            return -1;
        }
    }

    if (test_params->action_flags & TEST_DNXF_FABRIC_SNAKE_RUN_ONLY)
    {
        rv = soc_dnxf_cell_snake_test_run(unit, test_params->snake_test_run_flags, &test_params->res);
        if (rv != BCM_E_NONE)
        {
            test_params->res.test_failed = 1; /*make sure test marked as failed*/
            return -1;
        }
    }

    return (test_params->res.test_failed ? -1 : 0);
         
}

#endif /*BCM_DNXF_SUPPORT*/
