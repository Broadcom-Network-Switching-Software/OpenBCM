/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: sw_state_tests.c
 */
#include <appl/diag/system.h> /* for parse_small_integers */
#include <ibde.h>

#include <shared/bsl.h>

#include <soc/debug.h>
#include <soc/cmext.h>
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/dpp_config_defs.h>
#endif /* BCM_PETRA_SUPPORT */
#include <shared/bsl.h>
#include <bcm/stack.h>
#include <bcm/l2.h>
#include <bcm/trunk.h>
#include <bcm/vswitch.h>
#include <bcm/mpls.h>
#include <bcm/oam.h>
#include <bcm/bfd.h>
#include <bcm/policer.h>
#include <soc/drv.h>
#ifdef BCM_PETRA_SUPPORT
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/drv.h>
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
#endif /* BCM_PETRA_SUPPORT */
#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm/link.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#include <appl/diag/parse.h>
#include <appl/diag/test.h>
#include <appl/diag/shell.h>
#include <appl/diag/sysconf.h>
#include <appl/diag/bslenable.h>
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <appl/diag/dcmn/init_deinit.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#if defined (BCM_PETRA_SUPPORT) && !defined (__KERNEL__) && defined (_SHR_SW_STATE_EXM)

#include <shared/swstate/access/sw_state_access.h>

extern int init_deinit_test(int u, args_t *a, void *p);
#include <stdlib.h>
#define IS_RAND 1
#define MAX_RAND 20
#define MAX_RAND_BITS 200
#define ARR_ARR_NOF_INSTANCES 5
#define ARR_PTR_NOF_INSTANCES 5
#define NOF_BITS_BITDCL_SINGLE 100
#define BITDCL_DBL_PTR_NOF_INSTANCES 5
#define MINI_EXM_PATH sw_state[unit]->miniExm
#define MINI_EXM_ACCESS sw_state_access[unit].miniExm
#define GET_SIZE() MINI_EXM_ACCESS.get_size(unit, &size)
#define ASSERT() do { \
	GET_SIZE(); \
	if (size == expected_size) { \
		cli_out("\tPASSED!\n"); \
  } else { \
		cli_out("\tFAILED!\n"); \
        rv = _SHR_E_INTERNAL; \
	} \
  } \
	while (0)

char sw_state_usage[] = 
"SW_STATE Usage:\n"
"  size=<value>.\n"
;

typedef struct sw_state_test_init_param_s {
    uint32 unit;
    int32  size;
} sw_state_test_init_param_t;


STATIC sw_state_test_init_param_t sw_state_test_p[SOC_MAX_NUM_DEVICES];

void cli_out_aligned(char* str){
    cli_out("%-50s",str);
}


int
sw_state_test(int unit, args_t *a, void *p)
{
    int rv = _SHR_E_NONE;
    int size = 0, expected_size = 0, rand_num = 0, i = 0, rand_bool = 0;
    sw_state_test_init_param_t *init_param = p;
    
    COMPILER_REFERENCE(a);
    COMPILER_REFERENCE(p);

    init_deinit_test(unit,a,p);
    if (init_param->size) {
        cli_out_aligned("Test miniExm size:");
        MINI_EXM_ACCESS.alloc(0);
    	expected_size += sizeof(MINI_EXM_PATH) + sizeof(*MINI_EXM_PATH);
    	ASSERT ();

        cli_out_aligned("Test single pointer size (without hint):");
        MINI_EXM_ACCESS.singlePtr.alloc(0);
    	expected_size += sizeof (*MINI_EXM_PATH->singlePtr);
    	ASSERT();

        cli_out_aligned("Test pointer variable (with hint):");
        MINI_EXM_ACCESS.ptrVar.alloc(0);
    	expected_size += sizeof (*MINI_EXM_PATH->ptrVar);
    	ASSERT();

        cli_out_aligned("Test array variable:");
    	rand_num = rand() % MAX_RAND + 1;
    	MINI_EXM_ACCESS.arrVar.alloc(0, rand_num);
    	expected_size += rand_num * sizeof (MINI_EXM_PATH->arrVar[0]);
    	ASSERT();

        cli_out_aligned("Test ARR_ARR ptr alloc:");
        MINI_EXM_ACCESS.arrArr.ptr_alloc(0, ARR_ARR_NOF_INSTANCES);
    	expected_size += ARR_ARR_NOF_INSTANCES * sizeof (MINI_EXM_PATH->arrArr[0]);
    	ASSERT();

        cli_out_aligned("Test ARR_ARR alloc:");
    	for (i = 0; i < ARR_ARR_NOF_INSTANCES; ++i) {
    		rand_bool = rand() % 2;
    		if (rand_bool) {
    			rand_num = rand() % MAX_RAND + 1;
    			MINI_EXM_ACCESS.arrArr.alloc(0, i, rand_num);
    			expected_size += rand_num * sizeof (MINI_EXM_PATH->arrArr[0][0]);
    		}
    	}
    	ASSERT();

        cli_out_aligned("Test ARR_PTR pointer alloc:");
        MINI_EXM_ACCESS.arrPtr.ptr_alloc(0, ARR_PTR_NOF_INSTANCES);
    	expected_size += ARR_PTR_NOF_INSTANCES * sizeof (MINI_EXM_PATH->arrPtr[0]);
    	ASSERT();

        cli_out_aligned("Test ARR_PTR alloc:");
        for (i = 0; i < ARR_PTR_NOF_INSTANCES; ++i) {
            rand_bool = rand() % 2;
            if (rand_bool) {
                MINI_EXM_ACCESS.arrPtr.alloc(0, i);
                expected_size += sizeof (*MINI_EXM_PATH->arrPtr[0]);
            }
    	}
    	ASSERT();
        
        cli_out_aligned("Test shr_bitdcl single pointer:");
        MINI_EXM_ACCESS.shr_bitdcl_singlePtr.alloc_bitmap(0, NOF_BITS_BITDCL_SINGLE);
    	expected_size += SHR_BITALLOCSIZE (NOF_BITS_BITDCL_SINGLE);
    	ASSERT();
        
        cli_out_aligned("Test shr_bitdcl double pointer ptr_alloc:");
        MINI_EXM_ACCESS.shr_bitdcl_doublePtr.ptr_alloc(0, BITDCL_DBL_PTR_NOF_INSTANCES);
    	expected_size += BITDCL_DBL_PTR_NOF_INSTANCES * sizeof(MINI_EXM_PATH->shr_bitdcl_doublePtr[0]);
    	ASSERT();
        
        cli_out_aligned("Test shr_bitdcl double pointer alloc:");
        	for (i = 0; i < BITDCL_DBL_PTR_NOF_INSTANCES; ++i) {
    		rand_bool = rand() % 2;
    		if (rand_bool) {
    			rand_num = rand() % MAX_RAND_BITS + 1;
    			MINI_EXM_ACCESS.shr_bitdcl_doublePtr.alloc_bitmap(0, i, rand_num);
    			expected_size += SHR_BITALLOCSIZE(rand_num);
    		}
    	}
    	GET_SIZE();
    	ASSERT();
    }
    return rv;	
}

/*
 * Function:     sw_state_usage_parse
 * Purpose:      sw_state_usage_parse command
 * Parameters:   u - unit number to operate on
 *               a - args (none expected)
 *               init_param - return parsed parameter struct 
 * Returns:      CMD_OK/CMD_FAIL.
 */
cmd_result_t
sw_state_usage_parse(int unit, args_t *a, sw_state_test_init_param_t *init_param)
{
    int rv = CMD_OK;
    parse_table_t pt;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "size", PQ_INT, (void *) 0,
       &(init_param->size), NULL);
    
    if (parse_arg_eq(a, &pt) <= 0) {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",sw_state_usage);
        rv = CMD_FAIL;
    } else if (ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",sw_state_usage);
        rv = CMD_FAIL;
    }
    parse_arg_eq_done(&pt);
    return rv;
}

int
sw_state_test_init(int unit, args_t *a, void **p)
{
    sw_state_test_init_param_t *init_param;
    init_param = &sw_state_test_p[unit];
    sal_memset(init_param, 0x0, sizeof(sw_state_test_init_param_t));

    *p = init_param;
    return sw_state_usage_parse(unit, a, init_param);
}

int
sw_state_test_done(int unit, void *p)
{
    cli_out("SW_STATE Test Done\n");
    return 0;
}
#endif
