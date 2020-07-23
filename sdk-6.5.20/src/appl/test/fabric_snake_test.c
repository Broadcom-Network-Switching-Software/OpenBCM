/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Built-in Self Test for DFE fabric devices (Snake test)
 */

#include <soc/defs.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm_int/control.h>

#include <appl/diag/parse.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#ifdef BCM_DFE_SUPPORT
#include <appl/diag/dcmn/fabric.h>

#include <appl/diag/dfe/fabric.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_fabric_cell_snake_test.h>
#include <soc/dfe/cmn/dfe_diag.h>

#ifdef  INCLUDE_INTR
#include <appl/dcmn/rx_los/rx_los.h>
#endif /*INCLUDE_INTR*/



/*Fabric snake test action flags*/
#define FABRIC_SNAKE_TEST_PREPARE_ONLY      (0x1)
#define FABRIC_SNAKE_TEST_RUN_ONLY          (0x2)
#define FABRIC_SNAKE_TEST_ALL               (FABRIC_SNAKE_TEST_RUN_ONLY | FABRIC_SNAKE_TEST_PREPARE_ONLY)


char fabric_snake_test_usage[] = 
"Fabric snake test usage: \n"
"   Loopback=<loopback_name>            - valid <loopback_name> are: PHY (default), MAC, EXTERNAL\n"
"   Action=<action_name>                - valid <action_name> are: PREPARE_ONLY, RUN_ONLY, ALL (default)\n"
"   RunMode=<run_mode_name>             - valid <run_mode_name> are: INFINITE, STOP, NORMAL (default)\n"
;

typedef struct fabric_snake_test_params_s
{
    uint32                                  loopback_flag;
    uint32                                  snake_test_prepare_extra_flags;
    uint32                                  action_flags;
    uint32                                  snake_test_run_flags;
    soc_fabric_cell_snake_test_results_t    res;
} fabric_snake_test_params_t;

int
fabric_snake_test_init(int unit, args_t *a, void **p)
{
    char *loopback_str,
         *action_str,
         *run_mode_str;
    parse_table_t pt;
    fabric_snake_test_params_t *test_params;
    int rv = 0;

    if (p == NULL)
    {
        return -1;
    }

    if (!BCM_UNIT_VALID(unit))
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
        cli_out("%s\n",fabric_snake_test_usage);
        rv = -1;
    } else if (ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",fabric_snake_test_usage);
        rv = -1;
    }

    
    /*init fabric_snake_test_params structure*/
    if ((test_params = sal_alloc(sizeof(fabric_snake_test_params_t), "Fabric snake test tr")) == NULL)
    {
        cli_out("Failed to allocate memory\n");
        parse_arg_eq_done(&pt);
        return -1;
    }
    sal_memset(test_params, 0, sizeof(fabric_snake_test_params_t));
    *p = test_params;

   /*fill fabric_snake_test_params structure */
    /*Loopback config*/
    if (! sal_strncasecmp(loopback_str, "PHY", sal_strlen(loopback_str)) ) 
    {
        test_params->loopback_flag = SOC_DFE_ENABLE_PHY_LOOPBACK;
    } else if (! sal_strncasecmp(loopback_str, "MAC", sal_strlen(loopback_str)) ) 
    {
        test_params->loopback_flag = SOC_DFE_ENABLE_MAC_LOOPBACK;
    } else if (! sal_strncasecmp(loopback_str, "EXTERNAL", sal_strlen(loopback_str)) )
    {
        test_params->loopback_flag = SOC_DFE_ENABLE_EXTERNAL_LOOPBACK;
    } else {
        cli_out("Unknown Loopback value: %s\n",loopback_str);
        cli_out("%s\n",fabric_snake_test_usage);
        rv = -1;
    }

    /*Action config*/
    if (! sal_strncasecmp(action_str, "ALL", sal_strlen(action_str)) ) 
    {
        test_params->action_flags = FABRIC_SNAKE_TEST_ALL;
    } else if (! sal_strncasecmp(action_str, "PREPARE_ONLY", sal_strlen(action_str)) ) 
    {
        test_params->action_flags = FABRIC_SNAKE_TEST_PREPARE_ONLY;
    } else if (! sal_strncasecmp(action_str, "RUN_ONLY", sal_strlen(action_str)) )
    {
        test_params->action_flags = FABRIC_SNAKE_TEST_RUN_ONLY;
    } else {
        cli_out("Unknown Action value: %s\n",action_str);
        cli_out("%s\n",fabric_snake_test_usage);
        rv = -1;
    }

    /*RunMode config*/
    if (! sal_strncasecmp(run_mode_str, "NORMAL", sal_strlen(run_mode_str)) ) 
    {
        test_params->snake_test_run_flags = 0x0;
    } else if (! sal_strncasecmp(run_mode_str, "INFINITE", sal_strlen(run_mode_str)) ) 
    {
        test_params->snake_test_run_flags = SOC_DFE_SNAKE_INFINITE_RUN;
    } else if (! sal_strncasecmp(run_mode_str, "STOP", sal_strlen(run_mode_str)) )
    {
        test_params->snake_test_run_flags = SOC_DFE_SNAKE_STOP_TEST;
    } else {
        cli_out("Unknown RunMode value: %s\n",run_mode_str);
        cli_out("%s\n",fabric_snake_test_usage);
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
fabric_snake_test_done(int unit, void *p)
{
    fabric_snake_test_params_t *test_params;
    int rv = 0;

    if (p == NULL)
    {
        return -1;
    }
    test_params = (fabric_snake_test_params_t *) p;

    if (!BCM_UNIT_VALID(unit))
    {
        cli_out("Invalid unit %d\n", unit);
        rv = -1;
    }
    /*print results*/
    if (rv != -1)
    {
        /* coverity[overrun-call] */        
        diag_dfe_fabric_cell_snake_test_result_print(unit, &test_params->res);
    }

    if (test_params != NULL)
    {
        sal_free(test_params);
        p = test_params = NULL;
    }

    return rv;
}

int
fabric_snake_test(int unit, args_t *a, void *p)
{
    int rv;
    fabric_snake_test_params_t *test_params;

    if (p == NULL)
    {
        return -1;
    }
    test_params = (fabric_snake_test_params_t *) p;

    if (!BCM_UNIT_VALID(unit))
    {
        cli_out("Invalid unit %d\n", unit);
        return  -1;
    }

    /*RX LOS App shut down*/
#ifdef  INCLUDE_INTR
    if (SOC_IS_FE1600(unit))
    {
        rx_los_unit_detach(unit); /*no need to check error value*/
    }
#endif

    if ((test_params->action_flags & FABRIC_SNAKE_TEST_PREPARE_ONLY) && 
        !(test_params->snake_test_run_flags & SOC_DFE_SNAKE_STOP_TEST))
    {
        rv = soc_dfe_cell_snake_test_prepare(unit, test_params->loopback_flag);
        if (rv != BCM_E_NONE)
        {
            return -1;
        }
    }

    if (test_params->action_flags & FABRIC_SNAKE_TEST_RUN_ONLY)
    {
        rv = soc_dfe_cell_snake_test_run(unit, test_params->snake_test_run_flags, &test_params->res);
        if (rv != BCM_E_NONE)
        {
            test_params->res.test_failed = 1; /*make sure test marked as failed*/
            return -1;
        }
    }

    return (test_params->res.test_failed ? -1 : 0);
         
}

#endif /*BCM_DFE_SUPPORT*/
