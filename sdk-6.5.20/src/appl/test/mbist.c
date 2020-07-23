/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Memory Built-in Self Test
 */


#include <sal/types.h>
#include <soc/mem.h>
#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <bcm/link.h>
#include "testlist.h"

#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#endif

#if defined(BCM_TRIDENT2_SUPPORT)

typedef enum {
    MEMORY_BIST,
    LOGIC_BIST,
    ALL_BIST
} bist_type_t;

static bist_type_t bist_test_parameters[SOC_MAX_NUM_DEVICES];

int
memory_logic_bist_test_init(int u, args_t *a, void **p)
{
    char * test_type_name = NULL;
    bist_type_t * bist_type = NULL;
    parse_table_t pt;

    bist_type = &bist_test_parameters[u];
    parse_table_init(u, &pt);
    parse_table_add(&pt,  "type",    PQ_STRING, "memory",
            &(test_type_name), NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return -1;
    }

    if (!sal_strcasecmp(test_type_name, "memory")) {
        *bist_type = MEMORY_BIST;
    } else if (!sal_strcasecmp(test_type_name, "logic")) {
        *bist_type = LOGIC_BIST;
    } else if (!sal_strcasecmp(test_type_name, "all")) {
        *bist_type = ALL_BIST;
    } else {
        parse_arg_eq_done(&pt);
        cli_out("Invalid test type selected.\n");
        return -1;
    }
    if (soc_feature(u, soc_feature_mbist)){
    	  BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(u, 0));
        if (!SOC_IS_TOMAHAWK3(u)) {
            if ((soc_reset_init(u)) < 0) {
                parse_arg_eq_done(&pt);
                return -1;
            }
        }
    }
    *p = bist_type;
    parse_arg_eq_done(&pt);
    return 0;
}

int
memory_logic_bist_test(int u, args_t *a, void *p)
{
    bist_type_t * bist_type = p;

    if ((*bist_type == MEMORY_BIST) || (*bist_type == ALL_BIST)) {
        if (SOC_IS_TRIDENT2(u)) {
            cli_out("MEMORY BIST...\n");
            (void)trident2_mem_bist(u);
            cli_out("REGFILE BIST...\n");
            (void)trident2_regfile_bist(u);
        }
#ifdef BCM_TOMAHAWK2_SUPPORT
        else if (SOC_IS_TOMAHAWK2(u)) {
            cli_out("MEMORY BIST...\n");
            SOC_IF_ERROR_RETURN(tomahawk2_mem_bist(u));
        }
#endif /* BCM_TOMAHAWK2_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if (SOC_IS_TOMAHAWK3(u)) {
            cli_out("MEMORY BIST...\n");
            SOC_IF_ERROR_RETURN(tomahawk3_mem_bist(u));
        }
#endif /* BCM_TOMAHAWK2_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
        else if (SOC_IS_TRIDENT3(u))  {
            cli_out("MEMORY BIST...\n");
            SOC_IF_ERROR_RETURN(trident3_mem_bist(u));
        }
#endif /* BCM_TRIDENT3_SUPPORT*/
#ifdef BCM_MAVERICK2_SUPPORT
        else if (SOC_IS_MAVERICK2(u))  {
            cli_out("MEMORY BIST...\n");
            SOC_IF_ERROR_RETURN(maverick2_mem_bist(u));
        }
#endif /* BCM_MAVERICK2_SUPPORT*/
#ifdef BCM_FIREBOLT6_SUPPORT
        else if (SOC_IS_FIREBOLT6(u))  {
            cli_out("MEMORY BIST...\n");
            SOC_IF_ERROR_RETURN(firebolt6_mem_bist(u));
        }
#endif /* BCM_FIREBOLT6_SUPPORT*/

#ifdef BCM_TOMAHAWK_SUPPORT
        else if (SOC_IS_TOMAHAWK(u)) {
            cli_out("MEMORY BIST...\n");
            SOC_IF_ERROR_RETURN(tomahawk_mem_bist(u));
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
    }
    return 0;
}

int
memory_logic_bist_test_done(int u, void *p)
{
    if (soc_feature(u, soc_feature_mbist)){
        if ((soc_reset_init(u)) < 0) {
            return -1;
        }
    }
    return 0;
}

#endif
#if defined(BCM_PETRA_SUPPORT)
int
tcam_bist_test(int u, args_t *a, void *p)
{
    int rv = 0;

    rv = soc_dpp_tcam_bist_check(u);;

    if (rv < 0)
    {
        test_error(u, "TCAM BIST Failed!!!\n");
        return rv;
    }

    return rv;
}

#endif
