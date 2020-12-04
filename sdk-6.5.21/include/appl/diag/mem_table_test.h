/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Generic Hash Memory Test definitions.
 */

#ifndef __TEST_TABLE_GENERIC__H
#define __TEST_TABLE_GENERIC__H

#ifdef BCM_XGS_SWITCH_SUPPORT

#ifdef BCM_ISM_SUPPORT
#define TEST_HASH_MAX_BANKS _SOC_ISM_MAX_BANKS
#else
#define TEST_HASH_MAX_BANKS 20
#endif

/*
 * Generic Hashing Test Data Structure
 */
typedef struct test_generic_hash_testdata_s {
    int unit;
    char *mem_str;
    soc_mem_t mem;
    int copyno;
    
    int opt_count; /* entries for hash, buckets for overflow */
    int opt_verbose;
    int opt_reset;
    
    char *opt_key_base;
    int opt_key_incr;    
    int32 opt_banks;
    char *opt_hash_offsets;
    uint8 offset_count;
    uint8 offsets[TEST_HASH_MAX_BANKS];
    uint32 restore;
    uint8 config_banks[TEST_HASH_MAX_BANKS];
    uint8 cur_offset[TEST_HASH_MAX_BANKS];
} test_generic_hash_testdata_t;

typedef struct test_generic_hash_test_s {
    uint8 setup_done;
    test_generic_hash_testdata_t testdata;
    test_generic_hash_testdata_t *ctp;
    int unit;
} test_generic_hash_test_t;

#endif /* BCM_XGS_SWITCH_SUPPORT */

#endif /*!__TEST_TABLE_GENERIC__H */
