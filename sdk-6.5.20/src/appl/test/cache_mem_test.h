/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _CACHE_MEM_TEST_H
#define _CACHE_MEM_TEST_H

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)

#include <appl/diag/shell.h>

int		do_cache_mem_test(int , args_t *a, void *);
int		do_cache_mem_test_init(int, args_t *, void **);
int		do_cache_mem_test_done(int, void *);



typedef enum cache_mem_test_write_value_pattern_e {
	cache_mem_test_write_value_pattern_all_ones = 0,
	cache_mem_test_write_value_pattern_all_zeroes,
	cache_mem_test_write_value_pattern_incremental,
	cache_mem_test_write_value_pattern_smart
}cache_mem_test_write_value_pattern_t;

typedef enum cache_mem_test_type_e {
	cache_mem_test_type_single = 0,
	cache_mem_test_type_all_mems
}cache_mem_test_type_t;

typedef enum cache_mem_test_write_type_e {
	cache_mem_test_write_type_dma = 0,
	cache_mem_test_write_type_schan
}cache_mem_test_write_type_t;

typedef enum cache_mem_test_partial_e {
	cache_mem_test_full = 0,
	cache_mem_test_write_only,
	cache_mem_test_read_only,
	cache_mem_test_cache_only
}cache_mem_test_partial_t;

typedef struct tr_do_cache_mem_test_e
{
	cache_mem_test_type_t				test_type;
	cache_mem_test_write_type_t			write_type;
	cache_mem_test_write_value_pattern_t write_value_pattern;
	cache_mem_test_partial_t			test_part;
    int                                 stat_mem_not_tested_cnt;
    int                                 stat_mem_succeed_cnt;
    int                                 stat_mem_fail_cnt;
    int                                 stat_mem_total_cnt;
    soc_mem_t                           mem_id;
	cmd_result_t						result;

    uint32                              mem_test_result_flag[NUM_SOC_MEM];
} tr_do_cache_mem_test_t;

#define CACHE_MEM_TEST_ALIAS_SKIP_FLAG          (1 << 0)
#define CACHE_MEM_TEST_NON_CACHEABLE_SKIP_FLAG  (1 << 1)
#define CACHE_MEM_TEST_NOT_SHADOWED_SKIP_FLAG   (1 << 2)
#define CACHE_MEM_TEST_FORCE_SKIP_FLAG          (1 << 3)
#define CACHE_MEM_TEST_READ_FAILUE_FLAG         (1 << 4)
#define CACHE_MEM_TEST_WRITE_FAILUE_FLAG        (1 << 5)
#define CACHE_MEM_TEST_COMPARE_FAILUE_FLAG      (1 << 6)

#endif /*#(BCM_ESW_SUPPORT) || (BCM_PETRA_SUPPORT) || (BCM_DFE_SUPPORT) */
#endif /*_CACHE_MEM_TEST_H*/

