/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Generic memory table Tests.
 *
 * Insert/Lookup/Delete, hashing, bucket overflow tests.
 */

#include <soc/mem.h>
#include <appl/diag/system.h>
#include "testlist.h"
#include <bcm/init.h>
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/link.h>
#include <soc/l2x.h>
#include <soc/ism.h>
#include <appl/diag/mem_table_test.h>
#include <shared/bsl.h>
#ifdef BCM_XGS_SWITCH_SUPPORT
#include <bcm_int/esw/triumph.h>
#ifdef BCM_MONTEREY_SUPPORT
#include <soc/monterey.h>
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#endif
#ifdef BCM_APACHE_SUPPORT
#include <soc/apache.h>
#endif
#ifdef BCM_TRIDENT2_SUPPORT
#include <soc/trident2.h>
#endif
#ifdef BCM_TRIDENT3_SUPPORT
#include <soc/trident3.h>
#endif
#ifdef BCM_HELIX5_SUPPORT
#include <soc/helix5.h>
#endif
#if defined(CANCUN_SUPPORT)
#include "cancun.h"
#endif

STATIC test_generic_hash_test_t hash_test[SOC_MAX_NUM_DEVICES];

STATIC void
_test_generic_hash_setup(int unit, test_generic_hash_test_t *ht)
{
    test_generic_hash_testdata_t *htd;
    soc_mem_t tmem = L2_ENTRY_1m;
    
    if (ht->setup_done) {
        return;
    }
    ht->setup_done = TRUE;
    ht->unit = unit;
    
    htd = &ht->testdata;
    htd->mem = tmem;
    htd->mem_str = sal_strdup("l2_entry_1");
    htd->opt_count = 100; /*soc_mem_index_count(unit, tmem);*/
    htd->opt_verbose = FALSE;
    htd->opt_reset = FALSE;
    htd->opt_key_base = sal_strdup("0");
    htd->opt_key_incr = 1;
    htd->opt_banks = -1; /* All applicable banks by default */
    htd->opt_hash_offsets = sal_strdup("");
}

STATIC int
_test_generic_hash_init(int unit, test_generic_hash_testdata_t *htd,
                        args_t *a)
{
    int rv = -1, val;
    parse_table_t pt;
    char *offset, *buf = NULL;
    char *tokstr;
    
    parse_table_init(unit, &pt);
    
    parse_table_add(&pt, "Mem", PQ_DFL|PQ_STRING|PQ_STATIC, 0, &htd->mem_str, NULL);
    parse_table_add(&pt, "Count", PQ_INT|PQ_DFL, 0, &htd->opt_count, NULL);
    parse_table_add(&pt, "Verbose", PQ_BOOL|PQ_DFL, 0, &htd->opt_verbose, NULL);
    parse_table_add(&pt, "Reset", PQ_BOOL|PQ_DFL, 0, &htd->opt_reset, NULL);
    parse_table_add(&pt, "Key base", PQ_DFL|PQ_STRING|PQ_STATIC, 0, &htd->opt_key_base, NULL);
    parse_table_add(&pt, "Key incr", PQ_INT|PQ_DFL, 0, &htd->opt_key_incr, NULL);
    parse_table_add(&pt, "Banks", PQ_HEX|PQ_DFL, 0, &htd->opt_banks, NULL);
    parse_table_add(&pt, "Bank offsets", PQ_DFL|PQ_STRING|PQ_STATIC, 0, &htd->opt_hash_offsets, NULL);
    
    if (htd->opt_count < 1) {
        test_error(unit, "Illegal count %d\n", htd->opt_count);
        goto done;
    }
    
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
        test_error(unit,
                   "%s: Invalid option: %s\n", ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        return -1;
    }

    if (parse_memory_name(unit, &htd->mem, htd->mem_str,
                          &htd->copyno, 0) < 0) {
        test_error(unit, "Memory \"%s\" is invalid\n",
                   htd->mem_str);
        parse_arg_eq_done(&pt);
        return -1;
    }
    
    if (!isint(htd->opt_key_base)) {
        test_error(unit, "Key val is not valid\n");
        parse_arg_eq_done(&pt);
        return -1;
    }
    
    htd->offset_count = 0;
    
    if ((buf = sal_alloc(ARGS_BUFFER, "bank_offsets")) == NULL) {
        test_error(unit, "Unable to allocate memory for bank offsets\n");
        return -1;
    }
    
    strncpy(buf, htd->opt_hash_offsets, ARGS_BUFFER);/* Don't destroy input string */
    buf[ARGS_BUFFER - 1] = 0;        
    
    offset = sal_strtok_r(buf, ",", &tokstr);
    while (offset != 0) {
        val = parse_integer(offset);
        if (val > 64) {
            val = 64;
        } 
        htd->offsets[htd->offset_count] = (uint8)val;
        htd->offset_count++;
        offset = sal_strtok_r(NULL, ",", &tokstr);
    }
    
    /*
     * Re-initialize chip to ensure tables are clear
     * at start of test.
     */

    if (htd->opt_reset) {
        rv = bcm_linkscan_enable_set(unit, 0);
        if (rv && rv != BCM_E_UNIT) {
            test_error(unit, "Linkscan disabling failed\n");
            goto done;
        }
        if (soc_reset_init(unit) < 0) {
            test_error(unit, "SOC initialization failed\n");
            goto done;
        }

#if defined(CANCUN_SUPPORT)
        if (soc_cancun_pre_misc_init_load(unit) < 0) {
            test_error(unit, "CMH/CCH/CEH load failed");
            goto done;
        }
#endif

        if (soc_misc_init(unit) < 0) {
            test_error(unit, "MISC initialization failed\n");
            goto done;
        }

#if defined(CANCUN_SUPPORT)
        if (soc_cancun_post_misc_init_load(unit) < 0) {
            test_error(unit, "CIH/CFH load failed");
            goto done;
        }
#endif

        if (soc_mmu_init(unit) < 0) {
            test_error(unit, "MMU initialization failed\n");
            goto done;
        }

        if (mbcm_init(unit) < 0) { 
            test_error(unit, "BCM initialization failed\n");
            goto done;
        }
    }

    rv = 0;

done:
    if (buf) {
        sal_free(buf);
    }
    parse_arg_eq_done(&pt);
    return rv;
}

/* Generic hash test init */
int
test_generic_hash_init(int unit, args_t *a, void **p)
{
    test_generic_hash_test_t *ht = &hash_test[unit];
    test_generic_hash_testdata_t *htd = &ht->testdata;
    int rv;
    
    _test_generic_hash_setup(unit, ht);
    
    ht->ctp = htd;
    
    if ((rv = _test_generic_hash_init(unit, htd, a)) < 0) {
        return rv;
    } else {
        *p = htd;
    }
    return 0;
}

#if defined(BCM_TRIDENT2_SUPPORT)
/*
 * Test of hash memory
 *
 *   This test tries a number of keys against one of the hashing functions,
 *   checking a software hash against the hardware hash, then searching the
 *   bucket to find the entry after inserting.
 */
int
test_td2_generic_hash(int unit, args_t *a, void *p)
{
    test_generic_hash_testdata_t *htd = p;
    uint32 entry[SOC_MAX_MEM_WORDS] = {0}, soft_bucket;
    int ix, r, rv = 0;
    int iterations;
    int index, key_type = 0;
    int bank, bank_count;
#ifdef BCM_HELIX5_SUPPORT
    int phy_bank = 0;
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        r = soc_tomahawk_hash_bank_count_get(unit, htd->mem, &bank_count);
    } else
#endif
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        r = soc_apache_hash_bank_count_get(unit, htd->mem, &bank_count);
    } else
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5X(unit)) {
        r = soc_helix5_hash_bank_count_get(unit, htd->mem, &bank_count);
    } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        r = soc_trident3_hash_bank_count_get(unit, htd->mem, &bank_count);
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
        r = soc_trident2_hash_bank_count_get(unit, htd->mem, &bank_count);
    }
    if (r) {
        test_error(unit, "Could not get bank count\n");
        rv = -1;
        goto done;
    }
    iterations = htd->opt_count;

    if (htd->opt_verbose) {
        cli_out("Starting Generic Memory hashing test. Iterations: %d\n",iterations);
    }

    for( ix=0; ix < iterations; ix++) {
        for (bank = 0; bank < bank_count; bank++) {

            /* Prepare the entry */
            if (SOC_MEM_FIELD_VALID(unit, htd->mem, VALIDf)) {
                soc_mem_field32_set(unit, htd->mem, entry, VALIDf, 1);
            } else
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, htd->mem, BASE_VALIDf)) {
                soc_mem_field32_set(unit, htd->mem, entry, BASE_VALIDf, 1);
            } else if (SOC_MEM_FIELD_VALID(unit, htd->mem, BASE_VALID_0f) &&
                       SOC_MEM_FIELD_VALID(unit, htd->mem, BASE_VALID_1f)) {
                soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_0f, 1);
                soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_1f, 1);
            } else
#endif
            {
                soc_mem_field32_set(unit, htd->mem, entry, VALID_0f, 1);
                soc_mem_field32_set(unit, htd->mem, entry, VALID_1f, 1);
                if (SOC_MEM_FIELD_VALID(unit, htd->mem, VALID_2f)) {
                    soc_mem_field32_set(unit, htd->mem, entry, VALID_2f, 1);
                    soc_mem_field32_set(unit, htd->mem, entry, VALID_3f, 1);
                }
            }
            switch (htd->mem) {
                case L3_ENTRY_IPV4_UNICASTm:
                    key_type = TD2_L3_HASH_KEY_TYPE_V4UC;
                    break;
                case L3_ENTRY_IPV4_MULTICASTm:
                    key_type = TD2_L3_HASH_KEY_TYPE_V4MC;
                    break;
                case L3_ENTRY_IPV6_UNICASTm:
                    key_type = TD2_L3_HASH_KEY_TYPE_V6UC;
                    break;
                case L3_ENTRY_IPV6_MULTICASTm:
                    key_type = TD2_L3_HASH_KEY_TYPE_V6MC;
            }
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                switch (htd->mem) {
                case L3_ENTRY_DOUBLEm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_L3_HASH_KEY_TYPE_V6UC;
                    break;
                case L3_ENTRY_QUADm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_L3_HASH_KEY_TYPE_V6MC;
                    break;
                case VLAN_XLATE_1_DOUBLEm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_VLXLT_HASH_KEY_TYPE_IVID_OVID;
                    break;
                case VLAN_XLATE_2_DOUBLEm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_VLXLT_HASH_KEY_TYPE_VLAN_MAC;
                    break;
                case EGR_VLAN_XLATE_1_DOUBLEm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE;
                    break;
                case EGR_VLAN_XLATE_2_DOUBLEm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_ELVXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI;
                    break;
                case MPLS_ENTRYm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_MPLS_HASH_KEY_TYPE_MPLS;
                    break;
                default:
                    break;
                }
            }
#endif
            if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPEf)) {
                soc_mem_field32_set(unit, htd->mem, entry, KEY_TYPEf, key_type);
            } else {
                if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPE_0f)) {
                    soc_mem_field32_set(unit, htd->mem, entry, KEY_TYPE_0f, key_type);
                }
                if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPE_1f)) {
                    soc_mem_field32_set(unit, htd->mem, entry, KEY_TYPE_1f, key_type);
                }
                if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPE_2f)) {
                    soc_mem_field32_set(unit, htd->mem, entry, KEY_TYPE_2f, key_type);
                }
                if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPE_3f)) {
                    soc_mem_field32_set(unit, htd->mem, entry, KEY_TYPE_3f, key_type);
                }
            }

#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                if (SOC_MEM_FIELD_VALID(unit, htd->mem, DATA_TYPEf)) {
                    soc_mem_field32_set(unit, htd->mem, entry, DATA_TYPEf, key_type);
                }
                if (htd->mem == L3_ENTRY_DOUBLEm) {
                    soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_0f, 3);
                    soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_1f, 4);
                    if (SOC_VRF_MAX(unit)) {
                        soc_mem_field32_set(unit, htd->mem, entry, IPV6UC__VRF_IDf, 1);
                    }
                } else if (htd->mem == L3_ENTRY_QUADm) {
                    soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_0f, 5);
                    soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_1f, 6);
                    soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_2f, 6);
                    soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_3f, 7);
                    if (SOC_VRF_MAX(unit)) {
                        soc_mem_field32_set(unit, htd->mem, entry, IPV6MC__VRF_IDf, 1);
                    }
                } else if (htd->mem == VLAN_XLATE_1_DOUBLEm ||
                           htd->mem == VLAN_XLATE_2_DOUBLEm ||
                           htd->mem == EGR_VLAN_XLATE_1_DOUBLEm ||
                           htd->mem == EGR_VLAN_XLATE_2_DOUBLEm ||
                           htd->mem == MPLS_ENTRYm) {
                    soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_0f, 3);
                    soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_1f, 7);
                }
            }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit)) {
                r = soc_th_hash_bucket_get(unit, htd->mem, bank, entry, &soft_bucket);
            } else
#endif
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit)) {
                r = soc_ap_hash_bucket_get(unit, htd->mem, bank, entry, &soft_bucket);
            } else
#endif
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) ||
                SOC_IS_FIREBOLT6(unit)) {
                phy_bank = bank;
#ifdef BCM_HURRICANE4_SUPPORT
                if (SOC_IS_HURRICANE4(unit)) {
                    r = soc_hx5_hash_bank_number_get(unit, htd->mem, bank, &phy_bank);
                    if (r) {
                        test_error(unit, "Could not get phy bank\n");
                        rv = -1;
                        goto done;
                    }
                }
#endif
                r = soc_hx5_hash_bucket_get(unit, htd->mem, phy_bank, entry, &soft_bucket);
            } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                r = soc_td3_hash_bucket_get(unit, htd->mem, bank, entry, &soft_bucket);
            } else
#endif /* BCM_TRIDENT3_SUPPORT */
            {
               r = soc_hash_bucket_get(unit, htd->mem, bank, entry, &soft_bucket);
            }

            if (r) {
                test_error(unit, "Could not get bucket\n");
                rv = -1;
                goto done;
            }

#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit)) {
                index = soc_th_hash_index_get(unit, htd->mem, bank, soft_bucket);
            } else
#endif
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit)) {
                index = soc_ap_hash_index_get(unit, htd->mem, bank, soft_bucket);
            } else
#endif
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) ||
                SOC_IS_FIREBOLT6(unit)) {
                index = soc_hx5_hash_index_get(unit, htd->mem, phy_bank, soft_bucket);
            } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                index = soc_td3_hash_index_get(unit, htd->mem, bank, soft_bucket);
            } else
#endif /* BCM_TRIDENT3_SUPPORT */
            {
                index = soc_hash_index_get(unit, htd->mem, bank, soft_bucket);
            }

            if (htd->opt_verbose) {
                cli_out("Inserting ");
                soc_mem_entry_dump(unit, htd->mem, entry, BSL_LSS_CLI);
                cli_out("\n");
            }
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                r = soc_mem_generic_insert(unit, htd->mem, MEM_BLOCK_ALL, phy_bank,
                                           entry, NULL, &index);
            } else
#endif
            {
                r = soc_mem_generic_insert(unit, htd->mem, MEM_BLOCK_ALL, bank,
                                           entry, NULL, &index);
            }
            if (r < 0) {
                if (r == SOC_E_FULL) {
                    /* Bucket overflow, just pass on */
                    break;
                } else {
                      test_error(unit, "Insert failed at bucket %d\n", soft_bucket);
                      rv = -1;
                      goto done;
                }
            }

            /* Search for the entry, should be found */
            if (soc_mem_search(unit, htd->mem, MEM_BLOCK_ANY, &index,
                               entry, entry, 0) < 0) {
                test_error(unit, "Search failed in bucket %d\n", soft_bucket);
                rv = -1;
                return rv;
            }

            /* Delete the entry */
            if (soc_mem_delete(unit, htd->mem, MEM_BLOCK_ALL, entry) < 0) {
                test_error(unit, "Delete failed at bucket %d\n", soft_bucket);
                rv = -1;
                goto done;
            }
        }
    }
done:
    return rv;
}
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_ISM_SUPPORT
/*
 * Test of hash memory
 *
 *   This test tries a number of keys against one of the hashing functions,
 *   checking a software hash against the hardware hash, then searching the
 *   bucket to find the entry after inserting.
 */
int
test_tr3_generic_hash(int unit, args_t *a, void *p)
{
    int ix, index, rc = 0;
    int incr, iter, hw_index;
    uint8 *is = NULL, *isptr, inc_mode_hw = 0;
    uint8 tmp, inc_mode = 0, num_flds, key[64] = {0};
    uint8 banks[_SOC_ISM_MAX_BANKS], req_bcount = 0, bcount;
    uint32 opres, full = 0;
    uint32 entry[SOC_MAX_MEM_WORDS] = {0};
    uint32 rentry[SOC_MAX_MEM_WORDS] = {0};
    uint32 ifail = 0, sfail = 0, dfail = 0;
    uint32 fvalue[SOC_MAX_MEM_WORDS] = {0};    
    uint32 *buff = NULL, *bptr, sskip = 0;
    uint32 bmask = -1, sizes[_SOC_ISM_MAX_BANKS] = {0};
    soc_field_t lsbf, keyf[4] = {0};
    test_generic_hash_testdata_t *htd = p;
    
    COMPILER_REFERENCE(a);
    
    if (htd->opt_count > soc_mem_index_count(unit, htd->mem)) {
        htd->opt_count = soc_mem_index_count(unit, htd->mem);
    }
    iter = htd->opt_count;
    incr = htd->opt_key_incr;
    if (!incr) {
        iter = 1;
    }
    if (incr > 10) {
        /* Artificially limiting the increment value to max 100 */
        incr = htd->opt_key_incr = 10; 
    }
    if (htd->opt_verbose) {
        cli_out("Starting generic hash test: iter: %d, inc: %d\n",
                iter, incr);
    }    
    parse_long_integer((uint32*)key, sizeof(key)/sizeof(uint32), htd->opt_key_base);
    /* prepare entry and get s/w insert index */
    rc = soc_gen_entry_from_key(unit, htd->mem, key, entry);
    if (rc) {
        test_error(unit, "Could not generate entry from key: Test aborted.\n");
        return 0;
    }
    /* Fix the key_types to be the same */
    if (soc_mem_field_valid(unit, htd->mem, KEY_TYPE_0f)) {
        soc_mem_field32_set(unit, htd->mem, entry, KEY_TYPE_1f, 
                            soc_mem_field32_get(unit, htd->mem, entry, KEY_TYPE_0f));
        if (soc_mem_field_valid(unit, htd->mem, KEY_TYPE_2f)) {
            soc_mem_field32_set(unit, htd->mem, entry, KEY_TYPE_2f, 
                                soc_mem_field32_get(unit, htd->mem, entry, KEY_TYPE_0f));
            soc_mem_field32_set(unit, htd->mem, entry, KEY_TYPE_3f, 
                                soc_mem_field32_get(unit, htd->mem, entry, KEY_TYPE_0f));
        }
    }
    rc = soc_generic_get_hash_key(unit, htd->mem, entry, keyf, &lsbf, &num_flds);
    if (rc) {
        test_error(unit, "Could not retreive key fields from entry: Test aborted.\n");
        return 0;
    }
    
    rc = soc_ism_get_banks_for_mem(unit, htd->mem, banks, sizes, &bcount);
    if (rc || !bcount) {
        test_error(unit, "No banks configured for this memory: Test aborted.\n");
        return 0;
    }
    if (htd->opt_verbose) {
        cli_out("Num of configured banks: %d\n", bcount);
    }
    if (htd->opt_banks != -1) {
        bmask = 0;
        for (ix = 0; ix < bcount; ix++) {
            bmask |= 1 << banks[ix];
        }
        if (htd->opt_verbose) {
            cli_out("Supported banks mask: 0x%x\n", bmask);
        }
        if (!(htd->opt_banks & bmask)) {
            test_error(unit, "Invalid input banks mask: 0x%4x\n", htd->opt_banks);
            return 0;
        }
        if ((htd->opt_banks | bmask) != bmask) {
            test_error(unit, "Invalid input banks mask: 0x%4x\n", htd->opt_banks);
            return 0;
        }
        cli_out("Num of requested banks: %d\n", req_bcount);
    }
    htd->restore = 0;
    if (htd->offset_count) {
        if (htd->opt_verbose) {
            cli_out("Requested offsets: %d\n", htd->offset_count);
        }
        if (htd->offset_count > bcount) {
            test_error(unit, "Offsets count(%d) does not match banks count(%d).\n",
                       htd->offset_count, bcount);
            htd->offset_count = bcount;
        }
        for (ix = 0; ix < htd->offset_count; ix++) {
            if (htd->offsets[ix] >= 48) {
                inc_mode++; /* lsb mode */
            }
        }
        /* retreive, set and save offsets */
        for (ix = 0; ix < htd->offset_count; ix++) {
            rc = soc_ism_hash_offset_config_get(unit, banks[ix], &htd->cur_offset[ix]);
            if (rc) {
                test_error(unit, "Error retreiving offset for %s: bank: %d\n",
                           htd->mem_str, banks[ix]);
                return 0;
            }
            if (inc_mode && (htd->offsets[ix] < 48)) {
                /* Artificially bump the offset to lsb */
                if (htd->opt_verbose) {
                    cli_out("Updating offset for bank %d from %d to 48\n", 
                            banks[ix], htd->offsets[ix]);
                }
                htd->offsets[ix] = 48;
            }
            if (htd->offsets[ix] == htd->cur_offset[ix]) {
                continue;
            }
            rc = soc_ism_hash_offset_config(unit, banks[ix], htd->offsets[ix]);
            if (rc) {
                test_error(unit, "Error setting offset for %s: bank: %d\n",
                           htd->mem_str, banks[ix]);
                return 0;
            }
            htd->restore |= 1 << banks[ix];
            htd->config_banks[ix] = banks[ix];
            if (htd->opt_verbose) {
                cli_out("Set offset %d in place of %d for bank %d\n", 
                        htd->offsets[ix], htd->cur_offset[ix], banks[ix]);
            }
        }
    }
    
    /* determine inc_mode again from h/w config as something already with lsb 
       offset might not have been modified above */
    if (htd->opt_banks != -1) {
        for (ix = 0; ix < _SOC_ISM_MAX_BANKS; ix++) {
            if (htd->opt_banks & (1 << ix)) {
                rc = soc_ism_hash_offset_config_get(unit, ix, &tmp);
                if (rc) {
                    test_error(unit, "Error retreiving offset for %s: bank: %d\n",
                               htd->mem_str, ix);
                    return 0;
                }
                if (tmp >= 48) {
                    inc_mode_hw++;
                }
                req_bcount++;
            }
        }
    } else {
        for (ix = 0; ix < bcount; ix++) {
            rc = soc_ism_hash_offset_config_get(unit, banks[ix], &tmp);
            if (rc) {
                test_error(unit, "Error retreiving offset for %s: bank: %d\n",
                           htd->mem_str, banks[ix]);
                return 0;
            }
            if (tmp >= 48) {
                inc_mode_hw++;
            }
        }
    }
    if ((inc_mode && (inc_mode != inc_mode_hw)) || 
        (inc_mode_hw && (inc_mode_hw != req_bcount && inc_mode_hw != bcount))) {
        test_error(unit, "Conflicting or unsupported offset config detected: Test aborted.\n");
        return 0;
    }
    if (!inc_mode && inc_mode_hw) {
        inc_mode = inc_mode_hw;
    }
    
    if (!inc_mode) {
        soc_mem_field_get(unit, htd->mem, entry, keyf[0], fvalue);
    } else {
        soc_mem_field_get(unit, htd->mem, entry, lsbf, fvalue);
    }
    
    if ((buff = sal_alloc(iter * SOC_MAX_MEM_WORDS * sizeof(uint32), 
                          "hash_test_buff")) == NULL) {
        test_error(unit, "Buffer allocation failure: Test aborted.\n");
        return 0;
    }
    bptr = buff;
    if ((is = sal_alloc(iter * sizeof(uint8), "hash_test_stat")) == NULL) {
        test_error(unit, "Status buffer allocation failure: Test aborted.\n");
        rc = 0;
        goto done;
    }
    isptr = is;
    for (ix = 0; ix < iter; ix++) {
        rc = soc_generic_hash(unit, htd->mem, entry, htd->opt_banks, TABLE_INSERT_CMD_MSG,
                              &index, &opres, NULL, NULL);
        if (rc) {
            test_error(unit, "Error calculating hash: Test aborted.\n");
            goto done;
        }
        /* Insert in h/w */
        rc = soc_mem_generic_insert(unit, htd->mem, MEM_BLOCK_ANY, htd->opt_banks,
                                    entry, NULL, &hw_index);
        if (rc && ((rc != SOC_E_FULL) && (rc != SOC_E_EXISTS))) {
            test_error(unit, "Error in h/w insert: Test aborted.\n");
            goto done;
        } else {
            isptr[ix] = 1;
            rc = 0;
        }
        if ((opres == SCHAN_GEN_RESP_TYPE_FULL) && (hw_index == -1)) {
            cli_out("Bucket full for iter: %d\n", ix);
            isptr[ix] = 0;
            full++;
            bptr += (sizeof(entry) / sizeof(uint32));
            continue;
        }
        /* Compare indexes */
        if (index == hw_index) {
            if (htd->opt_verbose) {
                cli_out("Matching s/w and h/w insert(%d) index: %d\n",
                        ix, index);
            }
            /* Search for the entry, should be found */
            if (soc_mem_search(unit, htd->mem, MEM_BLOCK_ALL, &hw_index, 
                               entry, rentry, 0) < 0) {
                test_error(unit, "Search failed at index %d\n", index);
                rc = -1;
                break;
            }
            if (index != hw_index) {
                test_error(unit, "Mismatch in s/w and h/w search index: %d vs %d\n", index, hw_index);
                sfail++;
            } else {
                if (htd->opt_verbose) {
                    cli_out("Matching s/w and h/w search(%d) index: %d\n",
                            ix, index);
                }
            }
        } else {
            cli_out("Mismatch in s/w and h/w insert index: %d vs %d\n",
                    index, hw_index);
            ifail++;
            sskip++;
        }        
        sal_memcpy(bptr, entry, sizeof(entry));
        bptr += ((sizeof(entry) / sizeof(uint32)));
        
        if (!inc_mode) {
            if (soc_mem_field_valid(unit, htd->mem, KEY_TYPEf)) {
                fvalue[0] += incr << soc_mem_field_length(unit, htd->mem, KEY_TYPEf);
                soc_mem_field_set(unit, htd->mem, entry, keyf[0], fvalue);
            } else {
                fvalue[0] += incr << soc_mem_field_length(unit, htd->mem, KEY_TYPE_0f);
                soc_mem_field_set(unit, htd->mem, entry, keyf[0], fvalue);
            }
        } else {
            fvalue[0] += incr;
            fvalue[0] &= ((1 << soc_mem_field_length(unit, htd->mem, lsbf)) - 1);            
            soc_mem_field_set(unit, htd->mem, entry, lsbf, fvalue);
        }
    }    
    /* Delete entries */
    bptr = buff;
    isptr = is;
    for (ix = 0; ix < iter; ix++) {
        if (!isptr[ix]) {
            bptr += ((sizeof(entry) / sizeof(uint32)));
            continue;
        }
        if (soc_mem_delete(unit, htd->mem, MEM_BLOCK_ALL, bptr) < 0) {
            test_error(unit, "Delete failed for entry %d\n", ix);
            dfail++;
        }
        bptr += ((sizeof(entry) / sizeof(uint32)));
    }
    cli_out("Insert result: %d iter(s) passed out of %d iter(s), "
            "%d iter(s) skipped.\n", 
            iter-ifail-full, iter, full);
    cli_out("Search result: %d iter(s) passed out of %d iter(s), "
            "%d iter(s) skipped.\n", 
            iter-sfail-full-sskip, iter, full+sskip);
    cli_out("Delete result: %d entr(y/ies) deleted.\n",
            iter-dfail-full);
done:
    if (buff) {
        sal_free(buff);
    }
    if (is) {
        sal_free(is);
    }
    return rc;
}

int
test_generic_hash(int unit, args_t *a, void *p)
{
    int rv = -1;

    if (soc_feature(unit, soc_feature_ism_memory)) {
#ifdef BCM_ISM_SUPPORT
        rv = test_tr3_generic_hash(unit, a, p);
#endif /* BCM_ISM_SUPPORT */
    } else {
#ifdef BCM_TRIDENT2_SUPPORT
        rv = test_td2_generic_hash(unit, a, p);
#endif /* BCM_TRIDENT2_SUPPORT */
    }

    return rv;
}

int
test_generic_hash_ov_init(int unit, args_t *a, void **p)
{
    test_generic_hash_test_t *ht = &hash_test[unit];
    test_generic_hash_testdata_t *htd = &ht->testdata;
    int rv;
    
    _test_generic_hash_setup(unit, ht);
    
    ht->ctp = htd;
    
    if ((rv = _test_generic_hash_init(unit, htd, a)) < 0) {
        return rv;
    } else {
        *p = htd;
    }
    return 0;
}

#ifdef BCM_TRIDENT2_SUPPORT
/*
 * Test of hash memory overflow behavior
 *
 *   This test fills each bucket, then inserts another entry to see
 *   that the last entry fails to insert.
 */
int
test_td2_generic_hash_ov(int unit, args_t *a, void *p)
{
    test_generic_hash_testdata_t *htd = p;
    uint32 *entry_tmp = NULL, entry[SOC_MAX_MEM_WORDS] = {0};
    uint32 bucket = 0, result[SOC_MAX_MEM_WORDS] = {0}, hash;
    int ix, jx, r, idx, rv = 0;
    int ivid = 0, port = 0, key_type = 0, vrf_id = 0, vfi = 0, vp = 0;
    int iter = htd->opt_count;
    int bucket_size, num_entries;
    int bank_count, bank_num = 0, bank;
    int alloc_sz, offset;

    COMPILER_REFERENCE(a);

    if (htd->opt_verbose) {
       cli_out("Resetting hash selection to LSB\n");
    }
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        r = soc_tomahawk_hash_bank_count_get(unit, htd->mem, &bank_count);
        if (r) {
            test_error(unit, "Could not get bank count\n");
            rv = -1;
            goto done;
        }
        r = soc_tomahawk_hash_bank_number_get(unit, htd->mem, 0, &bank_num);
        if (r) {
            test_error(unit, "Could not get bank number\n");
            rv = -1;
            goto done;
        }
        r = soc_tomahawk_hash_bank_info_get(unit, htd->mem, bank_num, NULL, NULL,
                                      &bucket_size, NULL, NULL);
        if (r) {
            test_error(unit, "Could not get bucket size\n");
            rv = -1;
            goto done;
        }
    } else
#endif
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        r = soc_apache_hash_bank_count_get(unit, htd->mem, &bank_count);
        if (r) {
            test_error(unit, "Could not get bank count\n");
            rv = -1;
            goto done;
        }
        r = soc_apache_hash_bank_number_get(unit, htd->mem, 0, &bank_num);
        if (r) {
            test_error(unit, "Could not get bank number\n");
            rv = -1;
            goto done;
        }
        r = soc_apache_hash_bank_info_get(unit, htd->mem, bank_num, NULL, NULL,
                                      &bucket_size, NULL, NULL);
        if (r) {
            test_error(unit, "Could not get bucket size\n");
            rv = -1;
            goto done;
        }
    } else
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5X(unit)) {
        r = soc_helix5_hash_bank_count_get(unit, htd->mem, &bank_count);
        if (r) {
            test_error(unit, "Could not get bank count\n");
            rv = -1;
            goto done;
        }
        r = soc_hx5_hash_bank_number_get(unit, htd->mem, 0, &bank_num);
        if (r) {
            test_error(unit, "Could not get bank number\n");
            rv = -1;
            goto done;
        }
        r = soc_hx5_hash_bank_info_get(unit, htd->mem, bank_num, NULL, NULL,
                                       &bucket_size, NULL, NULL);
        if (r) {
            test_error(unit, "Could not get bucket size\n");
            rv = -1;
            goto done;
        }
    } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        r = soc_trident3_hash_bank_count_get(unit, htd->mem, &bank_count);
        if (r) {
            test_error(unit, "Could not get bank count\n");
            rv = -1;
            goto done;
        }
        r = soc_td3_hash_bank_number_get(unit, htd->mem, 0, &bank_num);
        if (r) {
            test_error(unit, "Could not get bank number\n");
            rv = -1;
            goto done;
        }
        r = soc_td3_hash_bank_info_get(unit, htd->mem, bank_num, NULL, NULL,
                                      &bucket_size, NULL, NULL);
        if (r) {
            test_error(unit, "Could not get bucket size\n");
            rv = -1;
            goto done;
        }
    } else
#endif
    {
        r = soc_trident2_hash_bank_count_get(unit, htd->mem, &bank_count);
        if (r) {
            test_error(unit, "Could not get bank count\n");
            rv = -1;
            goto done;
        }
        r = soc_trident2_hash_bank_number_get(unit, htd->mem, 0, &bank_num);
        if (r) {
            test_error(unit, "Could not get bank number\n");
            rv = -1;
            goto done;
        }
        r = soc_trident2_hash_bank_info_get(unit, htd->mem, bank_num, NULL, NULL,
                                      &bucket_size, NULL, NULL);
        if (r) {
            test_error(unit, "Could not get bucket size\n");
            rv = -1;
            goto done;
        }
    }

    num_entries = (bucket_size * bank_count);

    alloc_sz = sizeof(uint32) * SOC_MAX_MEM_WORDS * num_entries;
    entry_tmp = sal_alloc(alloc_sz, "entry_tmp");
    if (entry_tmp == NULL) {
        rv = -1;
        goto done;
    }

    /* Program the respective registers */
    switch (htd->mem) {
        case L2Xm:
            for (bank = 0; bank < bank_count; bank++) {
                if (htd->opt_verbose) {
                    cli_out("Resetting hash bank %d selection to LSB\n", bank);
            }
                if ((r = bcm_switch_hash_banks_config_set(unit, bcmHashTableL2, bank,
                                                          BCM_HASH_LSB, 48)) < 0) {
                    test_error(unit, "Hash bank setting failed\n");
                rv = -1;
                goto done;
            }
            }
            break;
#ifdef BCM_HURRICANE4_SUPPORT
        case L3_ENTRY_SINGLEm:
        case L3_ENTRY_DOUBLEm:
        case L3_ENTRY_QUADm:
#endif
        case L3_ENTRY_IPV4_UNICASTm:
        case L3_ENTRY_IPV4_MULTICASTm:
        case L3_ENTRY_IPV6_UNICASTm:
        case L3_ENTRY_IPV6_MULTICASTm:
            for (bank = 0; bank < bank_count; bank++) {
                if (htd->opt_verbose) {
                    cli_out("Resetting hash bank %d selection to LSB\n", bank);
            }
                if ((r = bcm_switch_hash_banks_config_set(unit, bcmHashTableL3, bank,
                                                          BCM_HASH_LSB, 48)) < 0) {
                    test_error(unit, "Hash bank setting failed\n");
                rv = -1;
                goto done;
            }
            }
            break;
        case ING_VP_VLAN_MEMBERSHIPm:
            if (SOC_REG_IS_VALID(unit, ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLr)) {
                if (READ_ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLr(unit, &hash) < 0) {
                    test_error(unit, "Hash select read failed\n");
                    goto done;
                }
                soc_reg_field_set(unit, ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLr, &hash,
                                  HASH_SELECT_Af, FB_HASH_LSB);
                soc_reg_field_set(unit, ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLr, &hash,
                                  HASH_SELECT_Bf, FB_HASH_LSB);
                if (WRITE_ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLr(unit, hash) < 0) {
                    test_error(unit, "Hash select setting failed\n");
                    rv = -1;
                    goto done;
                }
            }
            break;
        case EGR_VP_VLAN_MEMBERSHIPm:
            if (SOC_REG_IS_VALID(unit, EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLr)) {
                if (READ_EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLr(unit, &hash) < 0) {
                    test_error(unit, "Hash select read failed\n");
                    goto done;
                }
                soc_reg_field_set(unit, EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLr, &hash,
                                  HASH_SELECT_Af, FB_HASH_LSB);
                soc_reg_field_set(unit, EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLr, &hash,
                                  HASH_SELECT_Bf, FB_HASH_LSB);
                if (WRITE_EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLr(unit, hash) < 0) {
                    test_error(unit, "Hash select setting failed\n");
                    rv = -1;
                    goto done;
                }
            }
            break;
        case ING_DNAT_ADDRESS_TYPEm:
            if (READ_ING_DNAT_ADDRESS_TYPE_HASH_CONTROLr(unit, &hash) < 0) {
                test_error(unit, "Hash select read failed\n");
                goto done;
            }
            soc_reg_field_set(unit, ING_DNAT_ADDRESS_TYPE_HASH_CONTROLr, &hash,
                              HASH_SELECT_Af, FB_HASH_LSB);
            soc_reg_field_set(unit, ING_DNAT_ADDRESS_TYPE_HASH_CONTROLr, &hash,
                              HASH_SELECT_Bf, FB_HASH_LSB);
            if (WRITE_ING_DNAT_ADDRESS_TYPE_HASH_CONTROLr(unit, hash) < 0) {
                test_error(unit, "Hash select setting failed\n");
                rv = -1;
                goto done;
            }
            break;
        case L2_ENDPOINT_IDm:
            if (READ_L2_ENDPOINT_ID_HASH_CONTROLr(unit, &hash) < 0) {
                test_error(unit, "Hash select read failed\n");
                goto done;
            }
            soc_reg_field_set(unit, L2_ENDPOINT_ID_HASH_CONTROLr, &hash,
                              HASH_SELECT_Af, FB_HASH_LSB);
            soc_reg_field_set(unit, L2_ENDPOINT_ID_HASH_CONTROLr, &hash,
                              HASH_SELECT_Bf, FB_HASH_LSB);
            if (WRITE_L2_ENDPOINT_ID_HASH_CONTROLr(unit, hash) < 0) {
                test_error(unit, "Hash select setting failed\n");
                rv = -1;
                goto done;
            }
            break;
        case ENDPOINT_QUEUE_MAPm:
            if (READ_ENDPOINT_QUEUE_MAP_HASH_CONTROLr(unit, &hash) < 0) {
                test_error(unit, "Hash select read failed\n");
                goto done;
            }
            soc_reg_field_set(unit, ENDPOINT_QUEUE_MAP_HASH_CONTROLr, &hash,
                              HASH_SELECT_Af, FB_HASH_LSB);
            soc_reg_field_set(unit, ENDPOINT_QUEUE_MAP_HASH_CONTROLr, &hash,
                              HASH_SELECT_Bf, FB_HASH_LSB);
            if (WRITE_ENDPOINT_QUEUE_MAP_HASH_CONTROLr(unit, hash) < 0) {
                test_error(unit, "Hash select setting failed\n");
                rv = -1;
                goto done;
            }
            break;
        case VLAN_XLATEm:
            if (READ_VLAN_XLATE_HASH_CONTROLr(unit, &hash) < 0) {
                test_error(unit, "Hash select read failed\n");
                goto done;
            }
            soc_reg_field_set(unit, VLAN_XLATE_HASH_CONTROLr, &hash,
                              HASH_SELECT_Af, FB_HASH_LSB);
            soc_reg_field_set(unit, VLAN_XLATE_HASH_CONTROLr, &hash,
                              HASH_SELECT_Bf, FB_HASH_LSB);
            if (WRITE_VLAN_XLATE_HASH_CONTROLr(unit, hash) < 0) {
                test_error(unit, "Hash select setting failed\n");
                rv = -1;
                goto done;
            }
            break;
        case EGR_VLAN_XLATEm:
            if (READ_EGR_VLAN_XLATE_HASH_CONTROLr(unit, &hash) < 0) {
                test_error(unit, "Hash select read failed\n");
                goto done;
            }
            soc_reg_field_set(unit, EGR_VLAN_XLATE_HASH_CONTROLr, &hash,
                              HASH_SELECT_Af, FB_HASH_LSB);
            soc_reg_field_set(unit, EGR_VLAN_XLATE_HASH_CONTROLr, &hash,
                              HASH_SELECT_Bf, FB_HASH_LSB);
            if (WRITE_EGR_VLAN_XLATE_HASH_CONTROLr(unit, hash) < 0) {
                test_error(unit, "Hash select setting failed\n");
                rv = -1;
                goto done;
            }
            break;
        case MPLS_ENTRYm:
            if (SOC_REG_IS_VALID(unit, MPLS_ENTRY_HASH_CONTROLr)) {
                if (READ_MPLS_ENTRY_HASH_CONTROLr(unit, &hash) < 0) {
                    test_error(unit, "Hash select read failed\n");
                    goto done;
                }
                soc_reg_field_set(unit, MPLS_ENTRY_HASH_CONTROLr, &hash,
                                  HASH_SELECT_Af, FB_HASH_LSB);
                soc_reg_field_set(unit, MPLS_ENTRY_HASH_CONTROLr, &hash,
                                  HASH_SELECT_Bf, FB_HASH_LSB);
                if (WRITE_MPLS_ENTRY_HASH_CONTROLr(unit, hash) < 0) {
                    test_error(unit, "Hash select setting failed\n");
                    rv = -1;
                    goto done;
                }
            }
    }

    while (iter--) {
        for (ix = 0; ix < num_entries; ix++) {

            offset = ix * SOC_MAX_MEM_WORDS;
            /* Prepare the entry */
            sal_memset (&(entry_tmp[offset]), 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            if (SOC_MEM_FIELD_VALID(unit, htd->mem, VALIDf)) {
                soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), VALIDf, 1);
            } else
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, htd->mem, BASE_VALIDf)) {
                soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), BASE_VALIDf, 1);
            } else if (SOC_MEM_FIELD_VALID(unit, htd->mem, BASE_VALID_0f) &&
                       SOC_MEM_FIELD_VALID(unit, htd->mem, BASE_VALID_1f)) {
                soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), BASE_VALID_0f, 1);
                soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), BASE_VALID_1f, 1);
            } else
#endif
            {
                soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), VALID_0f, 1);
                soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), VALID_1f, 1);
                if (SOC_MEM_FIELD_VALID(unit, htd->mem, VALID_2f)) {
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), VALID_2f, 1);
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), VALID_3f, 1);
                }
            }
            switch (htd->mem) {
                case L2Xm:
                    key_type = TD2_L2_HASH_KEY_TYPE_VFI;
                    break;
                case L3_ENTRY_IPV4_UNICASTm:
                    key_type = TD2_L3_HASH_KEY_TYPE_V4UC;
                    break;
                case L3_ENTRY_IPV4_MULTICASTm:
                    key_type = TD2_L3_HASH_KEY_TYPE_V4MC;
                    break;
                case L3_ENTRY_IPV6_UNICASTm:
                    key_type = TD2_L3_HASH_KEY_TYPE_V6UC;
                    break;
                case L3_ENTRY_IPV6_MULTICASTm:
                    key_type = TD2_L3_HASH_KEY_TYPE_V6MC;
            }
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                switch (htd->mem) {
                case L3_ENTRY_DOUBLEm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_L3_HASH_KEY_TYPE_V6UC;
                    break;
                case L3_ENTRY_QUADm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_L3_HASH_KEY_TYPE_V6MC;
                    break;
                case VLAN_XLATE_1_DOUBLEm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_VLXLT_HASH_KEY_TYPE_IVID_OVID;
                    break;
                case VLAN_XLATE_2_DOUBLEm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_VLXLT_HASH_KEY_TYPE_VLAN_MAC;
                    break;
                case EGR_VLAN_XLATE_1_DOUBLEm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE;
                    break;
                case EGR_VLAN_XLATE_2_DOUBLEm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_ELVXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI;
                    break;
                case MPLS_ENTRYm:
                    /* coverity[mixed_enums] */
                    key_type = TD3_MPLS_HASH_KEY_TYPE_VXLAN_SIP;
                    break;
                default:
                    break;
                }
            }
#endif
            if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPEf)) {
                soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), KEY_TYPEf, key_type);
            } else {
                if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPE_0f)) {
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), KEY_TYPE_0f, key_type);
                }
                if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPE_1f)) {
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), KEY_TYPE_1f, key_type);
                }
                if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPE_2f)) {
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), KEY_TYPE_2f, key_type);
                }
                if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPE_3f)) {
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), KEY_TYPE_3f, key_type);
                }
            }
            switch (htd->mem) {
                case L2Xm:
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), VFIf, vfi);
                    break;
                case L3_ENTRY_IPV4_UNICASTm:
                case L3_ENTRY_IPV4_MULTICASTm:
                case L3_ENTRY_IPV6_UNICASTm:
                case L3_ENTRY_IPV6_MULTICASTm:
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), VRF_IDf, vrf_id);
                    break;
                case ING_VP_VLAN_MEMBERSHIPm:
                case EGR_VP_VLAN_MEMBERSHIPm:
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), VPf, vp);
                    break;
                case ING_DNAT_ADDRESS_TYPEm:
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), VRFf, vrf_id);
                    break;
                case L2_ENDPOINT_IDm:
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), VFIf, vfi);
                    break;
                case ENDPOINT_QUEUE_MAPm:
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), DEST_PORTf, port);
                    break;
                case VLAN_XLATEm:
                case EGR_VLAN_XLATEm:
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), IVIDf, ivid);
                    break;
                case MPLS_ENTRYm:
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), PORT_NUMf, port);
            }
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                if (SOC_MEM_FIELD_VALID(unit, htd->mem, DATA_TYPEf)) {
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), DATA_TYPEf, key_type);
                }
                if (htd->mem == L3_ENTRY_SINGLEm) {
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), IPV4UC__VRF_IDf,  vrf_id);
                } else if (htd->mem == L3_ENTRY_DOUBLEm) {
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), BASE_VALID_0f, 3);
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), BASE_VALID_1f, 4);
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), IPV6UC__VRF_IDf,  vrf_id);
                } else if (htd->mem == L3_ENTRY_QUADm) {
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), BASE_VALID_0f, 5);
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), BASE_VALID_1f, 6);
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), BASE_VALID_2f, 6);
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), BASE_VALID_3f, 7);
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), IPV6MC__VRF_IDf, vrf_id);
                } else if (htd->mem == VLAN_XLATE_1_DOUBLEm ||
                           htd->mem == VLAN_XLATE_2_DOUBLEm ||
                           htd->mem == EGR_VLAN_XLATE_1_DOUBLEm ||
                           htd->mem == EGR_VLAN_XLATE_2_DOUBLEm ||
                           htd->mem == MPLS_ENTRYm) {
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), BASE_VALID_0f, 3);
                    soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), BASE_VALID_1f, 7);
                    if (htd->mem != MPLS_ENTRYm) {
                        soc_mem_field32_set(unit, htd->mem, &(entry_tmp[offset]), IVIDf, ivid);
                    }
                }
            }
#endif

            if (ix == 0) {
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit)) {
                    soc_th_hash_bucket_get(unit, htd->mem, 0, &(entry_tmp[offset]), &bucket);
                    idx = soc_th_hash_index_get(unit, htd->mem, 0, bucket);
                } else
#endif
#ifdef BCM_APACHE_SUPPORT
                if (SOC_IS_APACHE(unit)) {
                    soc_ap_hash_bucket_get(unit, htd->mem, 0, &(entry_tmp[offset]), &bucket);
                    idx = soc_ap_hash_index_get(unit, htd->mem, 0, bucket);
                } else
#endif
#ifdef BCM_HELIX5_SUPPORT
                if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) ||
                    SOC_IS_FIREBOLT6(unit)) {

                    soc_hx5_hash_bucket_get(unit, htd->mem, bank_num, &(entry_tmp[offset]), &bucket);
                    idx = soc_hx5_hash_index_get(unit, htd->mem, bank_num, bucket);
                } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
                if (SOC_IS_TRIDENT3X(unit)) {
                    soc_td3_hash_bucket_get(unit, htd->mem, 0, &(entry_tmp[offset]), &bucket);
                    idx = soc_td3_hash_index_get(unit, htd->mem, 0, bucket);
                } else
#endif
                {
                    soc_hash_bucket_get(unit, htd->mem, 0, &(entry_tmp[offset]), &bucket);
                    idx = soc_hash_index_get(unit, htd->mem, 0, bucket);
                }
                if (htd->opt_verbose) {
                   cli_out("Filling bucket %d\n", bucket);
                }
            }
            if (htd->opt_verbose) {
                cli_out("Inserting entry at bucket %d\n", bucket);
                soc_mem_entry_dump(unit, htd->mem, &(entry_tmp[ix]), BSL_LSS_CLI);
                cli_out("\n");
            }

            if ((r = soc_mem_insert(unit, htd->mem, MEM_BLOCK_ALL, &(entry_tmp[offset]))) < 0) {
                if (r == SOC_E_FULL) {
                    /* Already full, stop wasting time */
                    break;
                } else {
                    test_error(unit, "Insert failed at bucket %d return:%d\n", bucket, r);
                    rv = -1;
                    goto done;
                }
            }

            ivid = (ivid > 0xfff) ? 1 : ivid + 1;
            port = (port > 0x3f) ? 1 : port + 1;
            vrf_id = (vrf_id >= SOC_VRF_MAX(unit)) ? 1 : vrf_id + 1;
            vp = (vp > 0xfff) ? 1 : vp + 1;
            vfi = (vfi > 0xfff) ? 1 : vfi + 1;
        }

        /* Prepare the entry which should overflow */
        sal_memset (entry, 0, sizeof(entry));
        if (SOC_MEM_FIELD_VALID(unit, htd->mem, VALIDf)) {
            soc_mem_field32_set(unit, htd->mem, entry, VALIDf, 1);
        } else
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_MEM_FIELD_VALID(unit, htd->mem, BASE_VALIDf)) {
            soc_mem_field32_set(unit, htd->mem, entry, BASE_VALIDf, 1);
        } else if (SOC_MEM_FIELD_VALID(unit, htd->mem, BASE_VALID_0f) &&
                   SOC_MEM_FIELD_VALID(unit, htd->mem, BASE_VALID_1f)) {
            soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_0f, 1);
            soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_1f, 1);
        } else
#endif
        {
            soc_mem_field32_set(unit, htd->mem, entry, VALID_0f, 1);
            soc_mem_field32_set(unit, htd->mem, entry, VALID_1f, 1);
            if (SOC_MEM_FIELD_VALID(unit, htd->mem, VALID_2f)) {
                soc_mem_field32_set(unit, htd->mem, entry, VALID_2f, 1);
                soc_mem_field32_set(unit, htd->mem, entry, VALID_3f, 1);
            }
        }
        if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPEf)) {
            soc_mem_field32_set(unit, htd->mem, entry, KEY_TYPEf, key_type);
        } else {
            if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPE_0f)) {
                soc_mem_field32_set(unit, htd->mem, entry, KEY_TYPE_0f, key_type);
            }
            if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPE_1f)) {
                soc_mem_field32_set(unit, htd->mem, entry, KEY_TYPE_1f, key_type);
            }
            if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPE_2f)) {
                soc_mem_field32_set(unit, htd->mem, entry, KEY_TYPE_2f, key_type);
            }
            if (SOC_MEM_FIELD_VALID(unit, htd->mem, KEY_TYPE_3f)) {
                soc_mem_field32_set(unit, htd->mem, entry, KEY_TYPE_3f, key_type);
            }
        }

        switch (htd->mem) {
            case L2Xm:
                soc_mem_field32_set(unit, htd->mem, entry, VFIf, vfi);
                break;
            case L3_ENTRY_IPV4_UNICASTm:
            case L3_ENTRY_IPV4_MULTICASTm:
            case L3_ENTRY_IPV6_UNICASTm:
            case L3_ENTRY_IPV6_MULTICASTm:
                soc_mem_field32_set(unit, htd->mem, entry, VRF_IDf, vrf_id);
                break;
            case ING_VP_VLAN_MEMBERSHIPm:
            case EGR_VP_VLAN_MEMBERSHIPm:
                soc_mem_field32_set(unit, htd->mem, entry, VPf, vp);
                break;
            case ING_DNAT_ADDRESS_TYPEm:
                soc_mem_field32_set(unit, htd->mem, entry, VRFf, vrf_id);
                break;
            case L2_ENDPOINT_IDm:
                soc_mem_field32_set(unit, htd->mem, entry, VFIf, vfi);
                break;
            case ENDPOINT_QUEUE_MAPm:
                soc_mem_field32_set(unit, htd->mem, entry, DEST_PORTf, port);
                break;
            case VLAN_XLATEm:
            case EGR_VLAN_XLATEm:
                soc_mem_field32_set(unit, htd->mem, entry, IVIDf, ivid);
                break;
            case MPLS_ENTRYm:
                soc_mem_field32_set(unit, htd->mem, entry, PORT_NUMf, port);
        }

#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            if (SOC_MEM_FIELD_VALID(unit, htd->mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, htd->mem, entry, DATA_TYPEf, key_type);
            }
            if (htd->mem == L3_ENTRY_SINGLEm) {
                soc_mem_field32_set(unit, htd->mem, entry, IPV4UC__VRF_IDf,  vrf_id);
            } else if (htd->mem == L3_ENTRY_DOUBLEm) {
                soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_1f, 4);
                soc_mem_field32_set(unit, htd->mem, entry, IPV6UC__VRF_IDf,  vrf_id);
            } else if (htd->mem == L3_ENTRY_QUADm) {
                soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_0f, 5);
                soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_1f, 6);
                soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_2f, 6);
                soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_3f, 7);
                soc_mem_field32_set(unit, htd->mem, entry, IPV6MC__VRF_IDf, vrf_id);
            } else if (htd->mem == VLAN_XLATE_1_DOUBLEm ||
                       htd->mem == VLAN_XLATE_2_DOUBLEm ||
                       htd->mem == EGR_VLAN_XLATE_1_DOUBLEm ||
                       htd->mem == EGR_VLAN_XLATE_2_DOUBLEm ||
                       htd->mem == MPLS_ENTRYm) {
                soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, htd->mem, entry, BASE_VALID_1f, 7);
                if (htd->mem != MPLS_ENTRYm) {
                    soc_mem_field32_set(unit, htd->mem, entry, IVIDf, ivid);
                }
            }
        }
#endif
        if (htd->opt_verbose) {
            cli_out("Inserting the extra entry in bucket %d, should fail\n", bucket);
            soc_mem_entry_dump(unit, htd->mem, entry, BSL_LSS_CLI);
            cli_out("\n");
        }
        /* Insert the extra entry, should overflow */
        if ((r = soc_mem_insert(unit, htd->mem, MEM_BLOCK_ALL, (entry))) < 0) {
            if (r != SOC_E_FULL) {
                test_error(unit, "Insert failed.\n");
                rv = -1;
                goto done;
            }
        } else {
            test_error(unit, "Insert to full bucket succeeded \n");
            rv = -1;
            goto done;
        }

        if (htd->opt_verbose) {
            cli_out("Verifying entries present\n");
        }

        /* Verify bucket contains our added entries */
        for (jx = 0; jx < ix; jx++) {
            offset = jx * SOC_MAX_MEM_WORDS;
            if (soc_mem_search(unit, htd->mem, MEM_BLOCK_ANY, &idx,
                               &(entry_tmp[offset]), result, 0) < 0) {
                test_error(unit, "Entry missing at bucket %d\n", bucket);
                rv = -1;
                goto done;
            }
        }

        if (htd->opt_verbose) {
            cli_out("Cleaning bucket %d\n", bucket);
        }

        /* Remove the entries that we added */
        for (jx = 0; jx < ix; jx++) {
            offset = jx * SOC_MAX_MEM_WORDS;
            if (soc_mem_delete(unit, htd->mem, MEM_BLOCK_ALL, &(entry_tmp[offset])) < 0) {
                test_error(unit, "Delete failed at bucket %d\n", bucket);
                rv = -1;
                goto done;
            }
        }
    }

done:
    if (entry_tmp) {
        sal_free(entry_tmp);
        entry_tmp = NULL;
    }
    return rv;
}
#endif /* BCM_TRIDENT2_SUPPORT */

/*
 * Test of hash memory overflow behavior
 *
 *   This test fills each bucket, then inserts another entry to see
 *   that the last entry fails to insert.
 */
int
test_generic_hash_ov(int unit, args_t *a, void *p)
{
    test_generic_hash_testdata_t *htd = p;
    int rv = -1;

    COMPILER_REFERENCE(a);
    if (htd->opt_verbose) {
        cli_out("Starting generic hash overflow test\n");
    }
    if (soc_feature(unit, soc_feature_ism_memory)) {
        rv = 0;
#ifdef BCM_TRIDENT2_SUPPORT
    } else {
        rv = test_td2_generic_hash_ov(unit, a, p);
#endif /* BCM_TRIDENT2_SUPPORT */
    }

    return rv;
}

/*
 * Test clean-up routine used for generic hash tests
 */

int
test_generic_hash_done(int unit, void *p)
{
    uint8 ix;
    int rc = 0;
    test_generic_hash_testdata_t *htd = p;
    
    if (htd->restore) {
        for (ix = 0; ix < htd->offset_count; ix++) {
            if (htd->restore & (1 << htd->config_banks[ix])) {
                rc = soc_ism_hash_offset_config(unit, htd->config_banks[ix], 
                                                htd->cur_offset[ix]);
                if (rc) {
                    test_error(unit, "Error setting offset for %s: bank: %d\n",
                               htd->mem_str, htd->config_banks[ix]);
                }
                if (htd->opt_verbose) {
                    cli_out("Restore offset %d for bank %d\n", 
                            htd->cur_offset[ix], htd->config_banks[ix]);
                }
            }
        }
    }
    return rc;
}

#endif /* BCM_ISM_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */

