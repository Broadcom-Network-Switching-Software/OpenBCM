/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * TRX Hash Table Tests.
 *
 * Insert/Lookup/Delete, hashing, bucket overflow tests.
 */



#include <soc/mem.h>
#include <soc/hash.h>
#include <shared/bsl.h>
#if defined(BCM_TRIUMPH_SUPPORT) /* DPPCOMPILEENABLE */
#include <soc/triumph.h>
#endif /* DPPCOMPILEENABLE */
#include <appl/diag/system.h>
#include <appl/test/tr_hash.h>
#include "testlist.h"
#include <bcm/init.h>
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/stack.h>
#include <bcm/link.h>
#ifdef BCM_ESW_SUPPORT 
#include <bcm_int/esw/firebolt.h>
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
#include <soc/apache.h>
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
#include <soc/monterey.h>
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
#include <soc/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
#include <soc/helix5.h>
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */
#if defined(CANCUN_SUPPORT)
#include "cancun.h"
#endif

#include <appl/diag/progress.h>

#if defined (BCM_PETRA_SUPPORT)
#include <soc/dpp/mbcm.h>
#include <soc/dpp/ARAD/arad_api_action_cmd.h>
#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/arad_api_end2end_scheduler.h>
#endif

#if defined(BCM_ESW_SUPPORT) /* || defined (BCM_PETRA_SUPPORT)  */


/*
 * work structure
 */

STATIC tr_hash_test_t tr_vlan_xlate_work[SOC_MAX_NUM_DEVICES];
STATIC tr_hash_test_t tr_egr_vlan_xlate_work[SOC_MAX_NUM_DEVICES];
#ifdef BCM_TRIUMPH_SUPPORT
STATIC tr_hash_test_t tr_mpls_work[SOC_MAX_NUM_DEVICES];
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * Utility routines for testing
 */

void
tr_hash_time_start(tr_hash_testdata_t *ad)
{
    ad->tm = SAL_TIME_DOUBLE();
}

void
tr_hash_time_end(tr_hash_testdata_t *ad)
{
    ad->tm = SAL_TIME_DOUBLE() - ad->tm;
    if (ad->opt_verbose) {
        cli_out("    time: %"COMPILER_DOUBLE_FORMAT" msec\n", ad->tm * 1000);
    }
}


static int
tr_hash_bucket_search(int unit, tr_hash_testdata_t *ad, soc_mem_t mem,
                      int bucket, int bucket_size, int validf, void *expect,
                      int dual, int bank)
{
    uint32 chip_entry[SOC_MAX_MEM_FIELD_WORDS];
    int ix, mem_table_index, iter_count;
    int rv = -1; /* Assume failed unless we find it */

    if (dual == TRUE) {
        iter_count = bucket_size / 2;
    } else {
        iter_count = bucket_size;
    }
    for (ix = 0; ix < iter_count; ix++) {
        mem_table_index = (bucket * bucket_size) + ix;
        if (bank) {
            mem_table_index += (bucket_size / 2);
        }

        if (soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                         mem_table_index, chip_entry) < 0) {
            test_error(unit,
                       "Read failed at bucket %d, offset %d\n",
                       bucket, ix);
            break;
        }

        if (validf != -1) {
            int validv = 0;
            if (soc_feature(unit, soc_feature_base_valid) &&
                validf == VALIDf) {
                validv = soc_mem_field32_get(unit, mem, chip_entry,
                                             BASE_VALID_0f) == 3 &&
                         soc_mem_field32_get(unit, mem, chip_entry,
                                             BASE_VALID_1f) == 7;
            } else {
                validv = soc_mem_field32_get(unit, mem, chip_entry, validf);
            }
            if (!validv) {
                /* Valid bit unset, entry blank */
                continue;
            }
        }

        if (soc_mem_compare_key(unit, mem, expect, &chip_entry) == 0) {
            /* Found the matching entry */
            rv = 0;
            break;
        }
    }

    return rv;
}

/************************ START OF VLAN XLATE TESTS ************************/
/*
 * Test initialization routine used for Vlan xlate tests
 */

STATIC int
tr_vlan_xlate_test_init(int unit, tr_hash_testdata_t *ad, args_t *a)
{
    int                         rv = -1, dual = 0;
    parse_table_t               pt;
    uint32                      hash_read = 0;
#if defined(BCM_ESW_SUPPORT) && defined(BCM_TRIDENT3_SUPPORT)
    l2_entry_hash_control_entry_t xlate_hash_read;
#endif /* BCM_ESW_SUPPORT && BCM_TRIDENT3_SUPPORT */
    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Count", PQ_INT|PQ_DFL, 0, &ad->opt_count, NULL);
    parse_table_add(&pt, "Verbose", PQ_BOOL|PQ_DFL, 0, &ad->opt_verbose, NULL);
    parse_table_add(&pt, "Reset", PQ_BOOL|PQ_DFL, 0, &ad->opt_reset, NULL);
    parse_table_add(&pt, "Hash", PQ_INT|PQ_DFL, 0, &ad->opt_hash, NULL);
    parse_table_add(&pt, "DualHash", PQ_INT|PQ_DFL, 0, &ad->opt_dual_hash, NULL);
    parse_table_add(&pt, "DualEnable", PQ_INT|PQ_DFL, 0, &dual, NULL);
    parse_table_add(&pt, "BaseOVID", PQ_INT|PQ_DFL, 0, &ad->opt_base_ovid, NULL);
    parse_table_add(&pt, "BaseIVID", PQ_INT|PQ_DFL, 0, &ad->opt_base_ivid, NULL);
    parse_table_add(&pt, "VidIncrement", PQ_INT|PQ_DFL, 0,
                    &ad->opt_vid_inc, NULL);

    /* Test the obvious parsings before wasting time with malloc */
    if (parse_arg_eq(a, &pt) < 0) {
        test_error(unit,
                   "%s: Error: Invalid option: %s\n", ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        goto done;
    }

    if (ad->opt_count < 1) {
        test_error(unit, "Illegal count %d\n", ad->opt_count);
        goto done;
    }

    if (ad->opt_hash >= ad->hash_count) {
        test_error(unit, "Illegal hash selection %d\n", ad->opt_hash);
        goto done;
    }

    if (dual == 1) {
        if (ad->opt_dual_hash >= ad->hash_count) {
            test_error(unit, "Illegal dual hash selection %d\n", ad->opt_dual_hash);
            goto done;
        }
    } else {
        ad->opt_dual_hash = -1;
    }

    if (ad->opt_base_ovid >= (1 << 12)) {
        test_error(unit, "Out of range Outer VLAN ID selection %d\n",
                   ad->opt_base_ovid);
        goto done;
    }

    if (ad->opt_base_ivid >= (1 << 12)) {
        test_error(unit, "Out of range Inner VLAN ID selection %d\n",
                   ad->opt_base_ivid);
        goto done;
    }

    /*
     * Re-initialize chip to ensure tables are clear
     * at start of test.
     */

    if (ad->opt_reset) {
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 0));
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
#if defined (BCM_PETRA_SUPPORT)		
		
	  	if (mbcm_dpp_init(unit) < 0) { 
	  		test_error(unit, "BCM initialization failed\n");
	  		goto done;
	  	}
#else
        if (mbcm_init(unit) < 0) { 
            test_error(unit, "BCM initialization failed\n");
            goto done;
        }
#endif
    }

#if defined(BCM_ESW_SUPPORT) 
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        if (soc_mem_read(unit, VLAN_XLATE_1_HASH_CONTROLm, MEM_BLOCK_ALL,
                         0, &xlate_hash_read) < 0) {
            test_error(unit, "Hash Control read failed\n");
            goto done;
        }
        sal_memcpy(&ad->save_xlate_hash_control, &xlate_hash_read,
                    sizeof(xlate_hash_read));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
    if (READ_VLAN_XLATE_HASH_CONTROLr(unit, &hash_read) < 0) {
        test_error(unit, "Hash select read failed\n");
        goto done;
    }
    }
#endif
    ad->save_hash_control = hash_read;
#if defined(BCM_ESW_SUPPORT) 

    if (!(soc_feature(unit, soc_feature_base_valid))) {
    soc_reg_field_set(unit, VLAN_XLATE_HASH_CONTROLr, &hash_read,
                      HASH_SELECT_Af, ad->opt_hash);
    if (ad->opt_dual_hash != -1) {
        soc_reg_field_set(unit, VLAN_XLATE_HASH_CONTROLr, &hash_read,
                          HASH_SELECT_Bf, ad->opt_dual_hash);
    } else {
        soc_reg_field_set(unit, VLAN_XLATE_HASH_CONTROLr, &hash_read,
                          HASH_SELECT_Bf, ad->opt_hash);
    }

    if (WRITE_VLAN_XLATE_HASH_CONTROLr(unit, hash_read) < 0) {
        test_error(unit, "Hash select setting failed\n");
        goto done;
    }
    }
#endif
    rv = 0;

 done:

    parse_arg_eq_done(&pt);
    return rv;
}

STATIC void
tr_vlan_xlate_hash_setup(int unit, tr_hash_test_t *dw)
{
    tr_hash_testdata_t *ad;
    soc_mem_t mem = VLAN_XLATEm;

    if (dw->lw_set_up) {
        return;
    }

    dw->lw_set_up = TRUE;
    dw->lw_unit = unit;

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }
#endif /* BCM_TRIDENT3_SUPPORT */
    dw->lp_hash.mem = mem;
    dw->lp_ov.mem = mem;

    /* Hash */
    ad = &dw->lp_hash;
#if defined(BCM_ESW_SUPPORT) 
    ad->opt_count      = soc_mem_index_count(unit, mem);
#elif defined(BCM_PETRA_SUPPORT)
    ad->opt_count      = soc_mem_index_count(unit, EGQ_VLAN_TABLEm);
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_extended_hash)) {
        ad->opt_hash       = FB_HASH_CRC32_LOWER;
        ad->opt_dual_hash  = FB_HASH_CRC32_UPPER;
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    {
        ad->opt_hash       = FB_HASH_CRC16_LOWER;
        ad->opt_dual_hash  = -1;
    }
    ad->hash_count     = FB_HASH_COUNT;
    ad->opt_base_ovid  = 0;
    ad->opt_base_ivid  = 0;
    ad->opt_vid_inc    = 1;

    /* Overflow */
    ad = &dw->lp_ov;

    ad->opt_count      = 2048;
    ad->opt_hash       = TR_DEFAULT_HASH;
    ad->opt_dual_hash  = -1;
    ad->hash_count     = FB_HASH_COUNT;
    ad->opt_base_ovid  = 0;
    ad->opt_base_ivid  = 0;
    ad->opt_vid_inc    = 1;
}

/* Individual test init wrappers */
int
tr_vlan_xlate_hash_test_init(int unit, args_t *a, void **p)
{
    tr_hash_test_t        *dw = &tr_vlan_xlate_work[unit];
    tr_hash_testdata_t    *dp = &dw->lp_hash;
    int                    rv;

    tr_vlan_xlate_hash_setup(unit, dw);

    /* Set working data to hash */
    dw->lp_cur = dp;

    if ((rv = tr_vlan_xlate_test_init(unit, dp, a)) < 0) {
        return rv;
    } else {
        *p = dp;
    }

    return 0;
}

int
tr_vlan_xlate_ov_test_init(int unit, args_t *a, void **p)
{
    tr_hash_test_t        *dw = &tr_vlan_xlate_work[unit];
    tr_hash_testdata_t    *dp = &dw->lp_ov;
    int                    rv;

    tr_vlan_xlate_hash_setup(unit, dw);

    /* Set working data to hash */
    dw->lp_cur = dp;

    if ((rv = tr_vlan_xlate_test_init(unit, dp, a)) < 0) {
        return rv; 
    } else {
        *p = dp;
    }

    return 0;
}

/*
 * Test of VLAN XLATE hashing
 *
 *   This test tries a number of keys against one of the hashing functions,
 *   checking a software hash against the hardware hash, then searching the
 *   bucket to find the entry after inserting.
 */
int
tr_vlan_xlate_test_hash(int unit, args_t *a, void *p)
{
    tr_hash_testdata_t         *ad = p;

#if defined(BCM_ESW_SUPPORT) 
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];
#endif    
    int soft_bucket, ix, r, rv = 0;
    int hash = ad->opt_hash;
    uint8 key[XGS_HASH_KEY_SIZE];
    int iterations, ovid, ivid, num_bits;
    int vid_inc = ad->opt_vid_inc;
    int bucket_size;
    int index;
    int dual = FALSE;
    int bank, banks, bank_count = 1;
    soc_mem_t mem;

    COMPILER_REFERENCE(a);

    if (ad->opt_verbose) {
        cli_out("Starting VLAN xlate hash test\n");
    }

    sal_memset(key, 0, sizeof(key));
    mem = ad->mem;

    /* This seems to be a fair "guess". is there a way to find the bucket size used in the HW? */
    if (mem != VLAN_XLATEm) {
        bucket_size = 4;
    } else {
    bucket_size = (soc_mem_index_max(unit, VLAN_MACm) < 32767) ? 8 : 16;
    }

#ifdef BCM_APACHE_SUPPORT
    if(SOC_IS_APACHE(unit)) {
        bucket_size = 8;
    }
#endif

#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
	    /* Even though with the reduced table size saber2 has the same bucket size as KT2 for this table */
	    bucket_size = 16;
    }
#endif

    iterations = ad->opt_count;
    ovid = ad->opt_base_ovid;
    ivid = ad->opt_base_ivid;
    if (ad->opt_dual_hash != -1) {
        dual = TRUE;
        bank_count = 2;
    }

    for (ix = 0; ix < iterations; ix++) {
        for (bank = 0; bank < bank_count; bank++) {
            sal_memset(entry, 0, sizeof (entry));
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, mem, entry, BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, mem, entry, BASE_VALID_1f, 7);
            } else {
                soc_mem_field32_set(unit, mem, entry, VALIDf, 1);
            }
            soc_mem_field32_set(unit, mem, entry, KEY_TYPEf, 0);
            soc_mem_field32_set(unit, mem, entry, OVIDf, ovid);
            soc_mem_field32_set(unit, mem, entry, IVIDf, ivid);

#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
                int phy_bank;
                soc_hx5_hash_bank_number_get(unit, mem, bank, &phy_bank);
                soft_bucket = soc_hx5_vlan_xlate_bank_entry_hash(unit, mem,
                                                   phy_bank, (uint32 *) entry);
            } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                soft_bucket = soc_td3_vlan_xlate_bank_entry_hash(unit, mem, bank,
                                                 (uint32 *) entry);
            } else
#endif
            {
            num_bits = soc_tr_vlan_xlate_base_entry_to_key
                    (unit, (uint32 *) entry, key);
            soft_bucket = soc_tr_vlan_xlate_hash(unit, hash, num_bits, 
                                                 (uint32 *) entry, key);
            }
            banks = 0;
            if (dual == TRUE) {
#ifdef BCM_TRIDENT2_SUPPORT
#ifdef BCM_TRIDENT3_SUPPORT
                if (SOC_IS_TRIDENT3X(unit)) {
                } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
                if (SOC_IS_APACHE(unit)) {
                    soft_bucket = soc_ap_vlan_xlate_bank_entry_hash(unit,
                                                    bank, (uint32 *) entry);
                } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit)) {
                    soft_bucket = soc_th_vlan_xlate_bank_entry_hash(unit,
                                                    bank, (uint32 *) entry);
                } else
#endif /* BCM_TOMAHAWK_SUPPORT */
                if (soc_feature(unit, soc_feature_extended_hash)) {
                    soft_bucket = soc_td2_vlan_xlate_bank_entry_hash(unit,
                                                    bank, (uint32 *) entry);
                } else
#endif /* BCM_TRIDENT2_SUPPORT */
                {
                    soft_bucket = soc_tr_vlan_xlate_bank_entry_hash
                        (unit, bank, (uint32 *) entry);
                }
                banks = (bank == 0) ? 2 : 1;
            }
    
            if (ad->opt_verbose) {
                cli_out("Inserting ");
                soc_mem_entry_dump(unit, VLAN_XLATEm, &entry, BSL_LSS_CLI);
                cli_out("\n");
                if (dual) {
                    cli_out("into bucket 0x%x (bank %d)\n",
                            soft_bucket, bank);
                } else {
                   cli_out("into bucket 0x%x\n", soft_bucket);
                }
            }
    
            if ((r = soc_mem_bank_insert(unit, mem, banks,
                                         MEM_BLOCK_ALL, entry, NULL)) < 0) {
                if (r == SOC_E_FULL) {
                    /* Bucket overflow, just pass on */
                    break;
                } else {
                    test_error(unit,
                               "Vlan xlate insert failed at bucket %d\n", soft_bucket);
                    rv = -1;
                    goto done;
                }
            }
    
            /* Now we search for the entry */
    
            /* Only do a quick check vs. expected bucket here */
            if (tr_hash_bucket_search(unit, ad, mem, soft_bucket,
                                      bucket_size, VALIDf, entry, dual, bank) < 0) {
                test_error(unit,
                           "Vlan xlate entry with key "
                           "0x%02x%02x%02x%02x%02x%02x%02x%01x "
                           "not found in predicted bucket %d\n",
                           key[7], key[6], key[5], key[4],
                           key[3], key[2], key[1], (key[0] >> 4) & 0xf,
                           soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Search for the entry, should be found */
            if (soc_mem_search(unit, mem, MEM_BLOCK_ALL, &index,
                               entry, entry, 0) < 0) {
                test_error(unit, "Vlan xlate search failed in bucket %d\n",
                           soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Insert the entry again, should fail. */
            if (soc_mem_bank_insert(unit, mem, banks,
                                    MEM_BLOCK_ALL, entry, NULL) != SOC_E_EXISTS) {
                test_error(unit,
                           "Vlan xlate insert should have failed at bucket %d\n", soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Delete the entry */
            if (soc_mem_delete(unit, mem, MEM_BLOCK_ALL, entry) < 0) {
                test_error(unit, "Vlan xlate delete failed at bucket %d\n",
                           soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Delete the entry again, should fail.*/
            if (soc_mem_delete(unit, mem,
                               MEM_BLOCK_ALL, entry) != SOC_E_NOT_FOUND) {
                test_error(unit, "Vlan xlate delete should have failed in bucket %d\n",
                           soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Search for the entry again, should not be found */
            if (soc_mem_search(unit, mem, MEM_BLOCK_ALL, &index,
                               entry, entry, 0) != SOC_E_NOT_FOUND) {
                test_error(unit, "Vlan xlate search should have failed in bucket %d\n",
                           soft_bucket);
                rv = -1;
                goto done;
            }
    
        }
        ovid += vid_inc;
        if (ovid > TR_VID_MAX) {
            ovid = 1;
        }
        ivid += vid_inc;
        if (ivid > TR_VID_MAX) {
            ivid = 1;
        }
    }

 done:
    return rv;
}

/*
 * Test of Vlan xlate overflow behavior
 *
 *   This test fills each bucket, then inserts another entry to see
 *   that the last entry fails to insert.
 */

int
tr_vlan_xlate_test_ov(int unit, args_t *a, void *p)
{
    tr_hash_testdata_t  *ad = p;
#if defined(BCM_ESW_SUPPORT) 
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];
    uint32 result[SOC_MAX_MEM_FIELD_WORDS];
    /* 20 is good enough for VLAN_XLATE TABLEs */
    uint32 entry_tmp[16][20];
#endif    
    int                 ix, jx, r, idx, rv = 0;
    int ovid, ivid;
    int bucket = 0;
    uint32 hash = ad->opt_hash;
    int vid_inc = ad->opt_vid_inc;
    int iter = ad->opt_count;
    uint8 key[XGS_HASH_KEY_SIZE];
    int bucket_size;
    soc_mem_t mem;

    COMPILER_REFERENCE(a);

    mem = ad->mem;

    /* This seems to be a fair "guess". is there a way to find the bucket size used in the HW? */
    if (mem != VLAN_XLATEm) {
        bucket_size = 4;
    } else {
    bucket_size = (soc_mem_index_max(unit, VLAN_MACm) < 32767) ? 8 : 16;
    }

#ifdef BCM_APACHE_SUPPORT
    if(SOC_IS_APACHE(unit)) {
        bucket_size = 8;
    }
#endif

#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
	    /* Even though with the reduced table size saber2 has the same bucket size as KT2 for this table */
	    bucket_size = 16;
    }
#endif

    if (hash != FB_HASH_LSB) {
        if (ad->opt_verbose) {
            cli_out("Resetting hash selection to LSB\n");
        }

        hash = ad->save_hash_control;
        soc_reg_field_set(unit, VLAN_XLATE_HASH_CONTROLr, &hash,
                          HASH_SELECT_Af, FB_HASH_LSB);
        soc_reg_field_set(unit, VLAN_XLATE_HASH_CONTROLr, &hash,
                          HASH_SELECT_Bf, FB_HASH_LSB);
    
        if (WRITE_VLAN_XLATE_HASH_CONTROLr(unit, hash) < 0) {
            test_error(unit, "Hash select setting failed\n");
            goto done;
        }

        ad->opt_hash = hash = FB_HASH_LSB;
    }

    if (iter > soc_mem_index_count(unit, mem)) {
        iter = soc_mem_index_count(unit, mem);
    }
    ovid = 0;
    ivid = ad->opt_base_ivid;

    while (iter--) {
        for (ix = 0; ix < bucket_size; ix++) {
            sal_memset(entry_tmp[ix], 0, sizeof(entry_tmp[0]));
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, mem, entry_tmp[ix], BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, mem, entry_tmp[ix], BASE_VALID_1f, 7);
            } else {
                soc_mem_field32_set(unit, mem, entry_tmp[ix], VALIDf, 1);
            }
            soc_mem_field32_set(unit, mem, entry_tmp[ix], OVIDf, ovid);
            soc_mem_field32_set(unit, mem, entry_tmp[ix], IVIDf, ivid);
            soc_mem_field32_set(unit, mem, entry_tmp[ix], KEY_TYPEf, 0);

            if (ix == 0) {
                int num_bits;
#ifdef BCM_HURRICANE4_SUPPORT
                if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
                    int phy_bank;
                    soc_hx5_hash_bank_number_get(unit, mem, 0, &phy_bank);
                    bucket =
                        soc_hx5_vlan_xlate_bank_entry_hash(unit, mem, phy_bank,
                                                    (uint32 *) entry_tmp[ix]);
                } else
#endif
                {
                num_bits = soc_tr_vlan_xlate_base_entry_to_key
                    (unit, (uint32 *) entry_tmp[ix], key);
                bucket = soc_tr_vlan_xlate_hash
                    (unit, hash, num_bits, (uint32 *) entry_tmp[ix], key);

                }
                if (ad->opt_verbose) {
                    cli_out("Filling bucket %d\n", bucket);
                }
            }

            if ((r = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry_tmp[ix])) < 0) {
                if (r == SOC_E_FULL) {
                    /* Already full, stop wasting time */
                    break;
                } else {
                    test_error(unit,
                               "Vlan xlate insert failed at bucket %d\n", bucket);
                    rv = -1;
                    goto done;
                }
            }

            /* key for LSB is the OVID, so we must keep it constant */
            ivid += vid_inc;
            if (ivid > TR_VID_MAX) {
                ivid = 1;
            }
        }

        if (ad->opt_verbose) {
            cli_out("Inserting %dth entry in bucket %d, should fail\n",
                    (bucket_size + 1), bucket);
        }

        sal_memset(entry, 0,  sizeof(entry));
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, mem, entry, BASE_VALID_0f, 3);
            soc_mem_field32_set(unit, mem, entry, BASE_VALID_1f, 7);
        } else {
            soc_mem_field32_set(unit, mem, entry, VALIDf, 1);
        }
        soc_mem_field32_set(unit, mem, entry, OVIDf, ovid);
        soc_mem_field32_set(unit, mem, entry, IVIDf, ivid);
        soc_mem_field32_set(unit, mem, entry, KEY_TYPEf, 0);

        if ((r = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry)) < 0) {
            if (r != SOC_E_FULL) {
                test_error(unit,
                           "Vlan xlate insert failed\n");
                rv = -1;
                goto done;
            }
        } else {
            test_error(unit, "Vlan xlate insert to full bucket succeeded\n");
            rv = -1;
            goto done;
        }

        if (ad->opt_verbose) {
            cli_out("Verifying entries present\n");
        }

        /* Verify bucket contains our added entries */
        for (jx = 0; jx < ix; jx++) {
            if (tr_hash_bucket_search(unit, ad, mem, bucket,
                                      bucket_size, VALIDf, entry_tmp[jx], 0, 0) < 0) {
                test_error(unit, "VLAN xlate entry missing at bucket %d\n", bucket);
                rv = -1;
                goto done;
            }
            if (soc_mem_search(unit, mem, MEM_BLOCK_ANY, &idx,
                               entry_tmp[jx], result, 0) < 0) {
                test_error(unit, "VLAN xlate entry missing at bucket %d\n", bucket);
                rv = -1;
                goto done;
            }
            if (bucket != (idx / bucket_size)) {
                test_error(unit, "VLAN xlate entry inserted into wrong bucket"
                           " Expected %d Actual %d\n", bucket, idx);
                rv = -1;
                goto done;
            }
        }

        if (ad->opt_verbose) {
            cli_out("Cleaning bucket %d\n", bucket);
        }

        /* Remove the entries that we added */
        for (jx = 0; jx < ix; jx++) {
            if (soc_mem_delete(unit, mem, MEM_BLOCK_ALL, entry_tmp[jx]) < 0) {
                test_error(unit, "Vlan xlate delete failed at bucket %d\n", bucket);
                rv = -1;
                goto done;
            }
        }

        /* We want the increment to change buckets by one. For VLAN XLATE,
         * the LSB hash bucket for KEY_TYPE 0 is determined by OVID. 
         */
        ovid += 1;
        if (ovid > TR_VID_MAX) {
            ovid = 0;
        }
    }

 done:

    return rv;
}

/*
 * Test clean-up routine used for all Vlan xlate tests
 */

int
tr_vlan_xlate_test_done(int unit, void *p)
{
    tr_hash_testdata_t *ad = p;
    soc_mem_t mem;

    if (ad == NULL) {
	return 0;
    }

    mem = ad->mem;

    /* Check if empty at the end of the test */
    if (ad->opt_reset) {
        int rv, ix;
        int index_min = soc_mem_index_min(unit, mem);
        int index_max = soc_mem_index_max(unit, mem);
        uint32 *buf = 0;
        uint32 *ent;
        uint32 count;

        buf = soc_cm_salloc(unit,
                            SOC_MEM_TABLE_BYTES(unit, mem),
                            "vlan_xlate_test");
        if (!buf) {
            test_error(unit, "Memory allocation failed\n");
            return (-1);
        }

        if ((rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                          index_min, index_max, buf)) < 0) {
            test_error(unit, "Memory DMA of VLAN_XLATEm entries failed\n");
            return (-1);
        }

        count = soc_mem_index_count(unit, mem);
        for (ix = 0; ix < count; ix++) {
            int vld = 0;
            ent = soc_mem_table_idx_to_pointer(unit, mem,
                                               uint32 *, buf, ix);

            if (soc_feature(unit, soc_feature_base_valid)) {
                vld = soc_mem_field32_get(unit, mem, ent, BASE_VALID_0f) == 3 &&
                      soc_mem_field32_get(unit, mem, ent, BASE_VALID_1f) == 7;
            } else
            {
                vld = soc_mem_field32_get(unit, mem, ent, VALIDf);
            }

            if (vld) {
                test_error(unit, "Vlan xlate table not empty after test entry = %d\n",
                                ix);
                soc_mem_entry_dump(unit, VLAN_XLATEm, ent, BSL_LSS_CLI);
                return (-1);
            }
        }

        soc_cm_sfree(unit, buf);
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_base_valid)) {
        if (soc_mem_write(unit, VLAN_XLATE_1_HASH_CONTROLm, MEM_BLOCK_ALL,
                         0, &ad->save_xlate_hash_control) < 0) {
            test_error(unit, "Hash Control restore failed\n");
        }
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
    if (WRITE_VLAN_XLATE_HASH_CONTROLr(unit, ad->save_hash_control) < 0) {
        test_error(unit, "Hash select restore failed\n");
    }
    }

    return 0;
}

/************************** END OF VLAN XLATE TESTS ************************/

/************************ START OF EGR VLAN XLATE TESTS ********************/
/*
 * Test initialization routine used for Egress Vlan xlate tests
 */

STATIC int
tr_egr_vlan_xlate_test_init(int unit, tr_hash_testdata_t *ad, args_t *a)
{
    int                         rv = -1, dual = 0;
    parse_table_t               pt;
    uint32                      hash_read;
#if defined(BCM_TRIDENT3_SUPPORT)
    l2_entry_hash_control_entry_t xlate_hash_read;
#endif /* BCM_TRIDENT3_SUPPORT */

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Count", PQ_INT|PQ_DFL, 0, &ad->opt_count, NULL);
    parse_table_add(&pt, "Verbose", PQ_BOOL|PQ_DFL, 0, &ad->opt_verbose, NULL);
    parse_table_add(&pt, "Reset", PQ_BOOL|PQ_DFL, 0, &ad->opt_reset, NULL);
    parse_table_add(&pt, "Hash", PQ_INT|PQ_DFL, 0, &ad->opt_hash, NULL);
    parse_table_add(&pt, "DualHash", PQ_INT|PQ_DFL, 0, &ad->opt_dual_hash, NULL);
    parse_table_add(&pt, "DualEnable", PQ_INT|PQ_DFL, 0, &dual, NULL);
    parse_table_add(&pt, "BaseOVID", PQ_INT|PQ_DFL, 0, &ad->opt_base_ovid, NULL);
    parse_table_add(&pt, "BaseIVID", PQ_INT|PQ_DFL, 0, &ad->opt_base_ivid, NULL);
    parse_table_add(&pt, "VidIncrement", PQ_INT|PQ_DFL, 0,
                    &ad->opt_vid_inc, NULL);

    /* Test the obvious parsings before wasting time with malloc */
    if (parse_arg_eq(a, &pt) < 0) {
        test_error(unit,
                   "%s: Error: Invalid option: %s\n", ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        goto done;
    }

    if (ad->opt_count < 1) {
        test_error(unit, "Illegal count %d\n", ad->opt_count);
        goto done;
    }

    if (ad->opt_hash >= ad->hash_count) {
        test_error(unit, "Illegal hash selection %d\n", ad->opt_hash);
        goto done;
    }

    if (dual == 1) {
        if (ad->opt_dual_hash >= ad->hash_count) {
            test_error(unit, "Illegal dual hash selection %d\n", ad->opt_dual_hash);
            goto done;
        }
    } else {
        ad->opt_dual_hash = -1;
    }

    if (ad->opt_base_ovid >= (1 << 12)) {
        test_error(unit, "Out of range Outer VLAN ID selection %d\n",
                   ad->opt_base_ovid);
        goto done;
    }

    if (ad->opt_base_ivid >= (1 << 12)) {
        test_error(unit, "Out of range Inner VLAN ID selection %d\n",
                   ad->opt_base_ivid);
        goto done;
    }

    /*
     * Re-initialize chip to ensure tables are clear
     * at start of test.
     */

    if (ad->opt_reset) {
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 0));
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
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_base_valid)) {
        if (soc_mem_read(unit, EGR_VLAN_XLATE_1_HASH_CONTROLm, MEM_BLOCK_ALL,
                         0, &xlate_hash_read) < 0) {
            test_error(unit, "Hash Control read failed\n");
            goto done;
        }

        sal_memcpy(&ad->save_xlate_hash_control, &xlate_hash_read,
                   sizeof(xlate_hash_read));
        /* Set robust hash, test mode settings if need be..,
           For now - we are using default setting */
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
    if (READ_EGR_VLAN_XLATE_HASH_CONTROLr(unit, &hash_read) < 0) {
        test_error(unit, "Hash select read failed\n");
        goto done;
    }

    ad->save_hash_control = hash_read;

    soc_reg_field_set(unit, EGR_VLAN_XLATE_HASH_CONTROLr, &hash_read,
                      HASH_SELECT_Af, ad->opt_hash);
    if (ad->opt_dual_hash != -1) {
        soc_reg_field_set(unit, EGR_VLAN_XLATE_HASH_CONTROLr, &hash_read,
                          HASH_SELECT_Bf, ad->opt_dual_hash);
    } else {
        soc_reg_field_set(unit, EGR_VLAN_XLATE_HASH_CONTROLr, &hash_read,
                          HASH_SELECT_Bf, ad->opt_hash);
    }

    if (WRITE_EGR_VLAN_XLATE_HASH_CONTROLr(unit, hash_read) < 0) {
        test_error(unit, "Hash select setting failed\n");
        goto done;
    }
    }

    if ((rv = soc_mem_clear(unit, ad->mem, COPYNO_ALL, TRUE)) < 0) {
        goto done;
    }

    rv = 0;

 done:

    parse_arg_eq_done(&pt);
    return rv;
}

STATIC void
tr_egr_vlan_xlate_hash_setup(int unit, tr_hash_test_t *dw)
{
    tr_hash_testdata_t *ad;
    soc_mem_t mem = EGR_VLAN_XLATEm;

    if (dw->lw_set_up) {
        return;
    }

    dw->lw_set_up = TRUE;
    dw->lw_unit = unit;

    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
    }
    dw->lp_hash.mem = mem;
    dw->lp_ov.mem = mem;

    /* Hash */
    ad = &dw->lp_hash;

    ad->opt_count      = soc_mem_index_count(unit, mem);
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_extended_hash)) {
        ad->opt_hash       = FB_HASH_CRC32_LOWER;
        ad->opt_dual_hash  = FB_HASH_CRC32_UPPER;
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    {
        ad->opt_hash       = FB_HASH_CRC16_LOWER;
        ad->opt_dual_hash  = -1;
    }
    ad->hash_count     = FB_HASH_COUNT;
    ad->opt_base_ovid  = 0;
    ad->opt_base_ivid  = 0;
    ad->opt_vid_inc    = 1;

    /* Overflow */
    ad = &dw->lp_ov;

    ad->opt_count      = 2048;
    ad->opt_hash       = TR_DEFAULT_HASH;
    ad->opt_dual_hash  = -1;
    ad->hash_count     = FB_HASH_COUNT;
    ad->opt_base_ovid  = 0;
    ad->opt_base_ivid  = 0;
    ad->opt_vid_inc    = 1;
}

/* Individual test init wrappers */
int
tr_egr_vlan_xlate_hash_test_init(int unit, args_t *a, void **p)
{
    tr_hash_test_t        *dw = &tr_egr_vlan_xlate_work[unit];
    tr_hash_testdata_t    *dp = &dw->lp_hash;
    int                    rv;

    tr_egr_vlan_xlate_hash_setup(unit, dw);

    /* Set working data to hash */
    dw->lp_cur = dp;

    if ((rv = tr_egr_vlan_xlate_test_init(unit, dp, a)) < 0) {
        return rv;
    } else {
        *p = dp;
    }

    return 0;
}

int
tr_egr_vlan_xlate_ov_test_init(int unit, args_t *a, void **p)
{
    tr_hash_test_t        *dw = &tr_egr_vlan_xlate_work[unit];
    tr_hash_testdata_t    *dp = &dw->lp_ov;
    int                    rv;

    tr_egr_vlan_xlate_hash_setup(unit, dw);

    /* Set working data to hash */
    dw->lp_cur = dp;

    if ((rv = tr_egr_vlan_xlate_test_init(unit, dp, a)) < 0) {
        return rv; 
    } else {
        *p = dp;
    }

    return 0;
}

/*
 * Test of EGRESS VLAN XLATE hashing
 *
 *   This test tries a number of keys against one of the hashing functions,
 *   checking a software hash against the hardware hash, then searching the
 *   bucket to find the entry after inserting.
 */
int
tr_egr_vlan_xlate_test_hash(int unit, args_t *a, void *p)
{
    tr_hash_testdata_t         *ad = p;
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];
    int soft_bucket, ix, r, rv = 0;
    int hash = ad->opt_hash;
    uint8 key[XGS_HASH_KEY_SIZE];
    int iterations, ovid, ivid, num_bits;
    int vid_inc = ad->opt_vid_inc;
    int bucket_size;
    int index;
    int dual = FALSE;
    int bank, banks, bank_count = 1;
    soc_mem_t mem = ad->mem;

    COMPILER_REFERENCE(a);

    if (ad->opt_verbose) {
        cli_out("Starting EGR VLAN xlate hash test\n");
    }

    sal_memset(key, 0, sizeof(key));

    /* This seems to be a fair "guess". is there a way to find the bucket size used in the HW? */
    bucket_size = (soc_mem_index_max(unit, mem) < 32767) ? 8 : 16;

    if (SOC_IS_TRIDENT3X(unit)) {
        bucket_size = 4;
    }

#ifdef BCM_APACHE_SUPPORT
    if(SOC_IS_APACHE(unit)) {
        bucket_size = 8;
    }
#endif

#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
	    /* Even though with the reduced table size saber2 has the same bucket size as KT2 for this table */
	    bucket_size = 16;
    }
#endif

    iterations = ad->opt_count;
    ovid = ad->opt_base_ovid;
    ivid = ad->opt_base_ivid;
    if (ad->opt_dual_hash != -1) {
        dual = TRUE;
        bank_count = 2;
    }


    for (ix = 0; ix < iterations; ix++) {
        for (bank = 0; bank < bank_count; bank++) {
            sal_memset(entry, 0, sizeof (entry));
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, mem, entry, BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, mem, entry, BASE_VALID_1f, 7);
            } else {
                soc_mem_field32_set(unit, mem, entry, VALIDf, 1);
            }
            soc_mem_field32_set(unit, mem, entry, OVIDf, ovid);
            soc_mem_field32_set(unit, mem, entry, IVIDf, ivid);

#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
                int phy_bank;
                soc_hx5_hash_bank_number_get(unit, mem, bank, &phy_bank);
                soft_bucket =
                    soc_hx5_egr_vlan_xlate_bank_entry_hash(unit, mem,
                                                   phy_bank, (uint32 *) entry);
            } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                soft_bucket = soc_td3_egr_vlan_xlate_bank_entry_hash(unit, mem, bank,
                                                 (uint32 *) entry);
                if (mem == EGR_VLAN_XLATE_1_DOUBLEm) {
                    /* Double Wide Entry , extract 10 bits only */
                    soft_bucket &= 0x3FF;
                }
            } else
#endif
            {
            num_bits = soc_tr_egr_vlan_xlate_base_entry_to_key
                    (unit, (uint32 *) entry, key);
            soft_bucket = soc_tr_egr_vlan_xlate_hash(unit, hash, num_bits,
                                                     (uint32 *)entry, key);
            }
            banks = 0;
            if (dual == TRUE) {
#ifdef BCM_TRIDENT2_SUPPORT
#ifdef BCM_TRIDENT3_SUPPORT
                if (SOC_IS_TRIDENT3X(unit)) {
                    /* soft_bucket is already computed on the top */
                } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
                if (SOC_IS_APACHE(unit)) {
                    soft_bucket = soc_ap_egr_vlan_xlate_bank_entry_hash(unit,
                                                    bank, (uint32 *) entry);
                } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit)) {
                    soft_bucket = soc_th_egr_vlan_xlate_bank_entry_hash(unit,
                                                    bank, (uint32 *) entry);
                } else
#endif /* BCM_TOMAHAWK_SUPPORT */
                if (soc_feature(unit, soc_feature_extended_hash)) {
                    soft_bucket = soc_td2_egr_vlan_xlate_bank_entry_hash(unit,
                                                    bank, (uint32 *) entry);
                } else
#endif /* BCM_TRIDENT2_SUPPORT */
                {
                    soft_bucket = soc_tr_egr_vlan_xlate_bank_entry_hash
                        (unit, bank, (uint32 *) entry);
                }
                banks = (bank == 0) ? 2 : 1;
            }
    
    
            if (ad->opt_verbose) {
                cli_out("Inserting ");
                soc_mem_entry_dump(unit, mem, &entry, BSL_LSS_CLI);
                cli_out("\n");
                if (dual) {
                    cli_out("into bucket 0x%x (bank %d)\n",
                            soft_bucket, bank);
                } else {
                   cli_out("into bucket 0x%x\n", soft_bucket);
                }
            }
    
            if ((r = soc_mem_bank_insert(unit, mem, banks,
                                         MEM_BLOCK_ALL, entry, NULL)) < 0) {
                if (r == SOC_E_FULL) {
                    /* Bucket overflow, just pass on */
                    break;
                } else {
                    test_error(unit,
                               "EGR Vlan xlate insert failed at bucket %d\n", soft_bucket);
                    rv = -1;
                    goto done;
                }
            }
    
            /* Now we search for the entry */
    
            /* Only do a quick check vs. expected bucket here */
            if (tr_hash_bucket_search(unit, ad, mem, soft_bucket,
                                      bucket_size, VALIDf, entry, dual, bank) < 0) {
                test_error(unit,
                           "EGR Vlan xlate entry with key "
                           "0x%02x%02x%02x%02x%02x%02x%02x%01x "
                           "not found in predicted bucket %d\n",
                           key[7], key[6], key[5], key[4],
                           key[3], key[2], key[1], (key[0] >> 4) & 0xf,
                           soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Search for the entry, should be found */
            if (soc_mem_search(unit, mem, MEM_BLOCK_ALL, &index,
                               entry, entry, 0) < 0) {
                test_error(unit, "EGR Vlan xlate search failed in bucket %d\n",
                           soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Insert the entry again, should fail. */
            if (soc_mem_bank_insert(unit, mem, banks,
                                    MEM_BLOCK_ALL, entry, NULL) != SOC_E_EXISTS) {
                test_error(unit,
                           "EGR Vlan xlate insert should have failed at bucket %d\n", 
                            soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Delete the entry */
            if (soc_mem_delete(unit, mem, MEM_BLOCK_ALL, entry) < 0) {
                test_error(unit, "EGR Vlan xlate delete failed at bucket %d\n",
                           soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Delete the entry again, should fail.*/
            if (soc_mem_delete(unit, mem,
                               MEM_BLOCK_ALL, entry) != SOC_E_NOT_FOUND) {
                test_error(unit, "EGR Vlan xlate delete should have failed in bucket %d\n",
                           soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Search for the entry again, should not be found */
            if (soc_mem_search(unit, mem, MEM_BLOCK_ALL, &index,
                               entry, entry, 0) != SOC_E_NOT_FOUND) {
                test_error(unit, "EGR Vlan xlate search should have failed in bucket %d\n",
                           soft_bucket);
                rv = -1;
                goto done;
            }
        }
        ovid += vid_inc;
        if (ovid > TR_VID_MAX) {
            ovid = 1;
        }
        ivid += vid_inc;
        if (ivid > TR_VID_MAX) {
            ivid = 1;
        }
    }

 done:
    return rv;
}

/*
 * Test of Egress Vlan xlate overflow behavior
 *
 *   This test fills each bucket, then inserts another entry to see
 *   that the last entry fails to insert.
 */

int
tr_egr_vlan_xlate_test_ov(int unit, args_t *a, void *p)
{
    tr_hash_testdata_t  *ad = p;

    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];
    uint32 result[SOC_MAX_MEM_FIELD_WORDS];
    /* 20 is good enough for EGR_VLAN_XLATE TABLEs */
    uint32 entry_tmp[16][20];

    int                 ix, jx, r, idx, rv = 0;
    int ivid, port_group;
    volatile int ovid;
    int bucket = 0;
    uint32 hash = ad->opt_hash;
    volatile int iter = ad->opt_count;
    uint8 key[XGS_HASH_KEY_SIZE];
    int bucket_size;
    soc_mem_t mem = ad->mem;

    COMPILER_REFERENCE(a);

    /* This seems to be a fair "guess". is there a way to find the bucket size used in the HW? */
    bucket_size = (soc_mem_index_max(unit, mem) < 32767) ? 8 : 16;

    if (SOC_IS_TRIDENT3X(unit)) {
        bucket_size = 4;
    }

#ifdef BCM_APACHE_SUPPORT
    if(SOC_IS_APACHE(unit)) {
        bucket_size = 8;
    }
#endif

#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
	    /* Even though with the reduced table size saber2 has the same bucket size as KT2 for this table */
	    bucket_size = 16;
    }
#endif

    if (hash != FB_HASH_LSB) {
        if (ad->opt_verbose) {
            cli_out("Resetting hash selection to LSB\n");
        }

        hash = ad->save_hash_control;
        soc_reg_field_set(unit, EGR_VLAN_XLATE_HASH_CONTROLr, &hash,
                          HASH_SELECT_Af, FB_HASH_LSB);
        soc_reg_field_set(unit, EGR_VLAN_XLATE_HASH_CONTROLr, &hash,
                          HASH_SELECT_Bf, FB_HASH_LSB);
    
        if (WRITE_EGR_VLAN_XLATE_HASH_CONTROLr(unit, hash) < 0) {
            test_error(unit, "Hash select setting failed\n");
            goto done;
        }

        ad->opt_hash = hash = FB_HASH_LSB;
    }

    if (iter > soc_mem_index_count(unit, mem)) {
        iter = soc_mem_index_count(unit, mem);
    }
    port_group = 0;
    ovid = 0;
    ivid = 0;

    while (iter--) {
        for (ix = 0; ix < bucket_size; ix++) {
            sal_memset(entry_tmp[ix], 0, sizeof(entry_tmp[0]));
            if (soc_feature(unit, soc_feature_base_valid)) {
                soc_mem_field32_set(unit, mem, entry_tmp[ix], BASE_VALID_0f, 3);
                soc_mem_field32_set(unit, mem, entry_tmp[ix], BASE_VALID_1f, 7);
            } else {
                soc_mem_field32_set(unit, mem, entry_tmp[ix], VALIDf, 1);
            }
            soc_mem_field32_set(unit, mem, entry_tmp[ix], OVIDf, ovid);
            soc_mem_field32_set(unit, mem, entry_tmp[ix], IVIDf, ivid);
            soc_mem_field32_set(unit, mem, entry_tmp[ix], PORT_GROUP_IDf, port_group);

            if (ix == 0) {
                int num_bits;
#ifdef BCM_HURRICANE4_SUPPORT
                if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
                    int phy_bank;
                    soc_hx5_hash_bank_number_get(unit, mem, 0, &phy_bank);
                    bucket =
                        soc_hx5_egr_vlan_xlate_bank_entry_hash(unit, mem,
                                          phy_bank, (uint32 *) entry_tmp[ix]);
                } else
#endif
                {
                num_bits = soc_tr_egr_vlan_xlate_base_entry_to_key
                    (unit, (uint32 *) entry_tmp[ix], key);
                bucket = soc_tr_egr_vlan_xlate_hash(unit, hash, num_bits,
                                                    (uint32 *)entry_tmp[ix],
                                                    key);
                }

                if (ad->opt_verbose) {
                    cli_out("Filling bucket %d\n", bucket);
                }
            }

            if ((r = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry_tmp[ix])) < 0) {
                if (r == SOC_E_FULL) {
                    /* Already full, stop wasting time */
                    break;
                } else {
                    test_error(unit,
                               "EGR Vlan xlate insert failed at bucket %d\n", bucket);
                    rv = -1;
                    goto done;
                }
            }

            /* key for LSB is the OVID , so we must keep it constant */
            ivid += 1;
            if (ivid > TR_VID_MAX) {
                ivid = 0;
    /*    coverity[assignment : FALSE]    */
                port_group += 1;
                if (port_group > 0x3f) {
                    port_group = 0;
                }
            }
        }

        if (ad->opt_verbose) {
            cli_out("Inserting %dth entry in bucket %d, should fail\n",
                    (bucket_size + 1), bucket);
        }

        sal_memset(entry, 0,  sizeof(entry));
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, mem, entry, BASE_VALID_0f, 3);
            soc_mem_field32_set(unit, mem, entry, BASE_VALID_1f, 7);
        } else {
            soc_mem_field32_set(unit, mem, entry, VALIDf, 1);
        }
        soc_mem_field32_set(unit, mem, entry, OVIDf, ovid);
        soc_mem_field32_set(unit, mem, entry, IVIDf, ivid);
        soc_mem_field32_set(unit, mem, entry, PORT_GROUP_IDf, port_group);

        if ((r = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry)) < 0) {
            if (r != SOC_E_FULL) {
                test_error(unit,
                           "EGR Vlan xlate insert failed\n");
                rv = -1;
                goto done;
            }
        } else {
            test_error(unit, "EGR Vlan xlate insert to full bucket succeeded\n");
            rv = -1;
            goto done;
        }

        if (ad->opt_verbose) {
            cli_out("Verifying entries present\n");
        }

        /* Verify bucket contains our added entries */
        for (jx = 0; jx < ix; jx++) {
            if (tr_hash_bucket_search(unit, ad, mem, bucket,
                                      bucket_size, VALIDf, entry_tmp[jx], 0, 0) < 0) {
                test_error(unit, "EGR VLAN xlate entry missing at bucket %d\n", bucket);
                rv = -1;
                goto done;
            }
            if (soc_mem_search(unit, mem, MEM_BLOCK_ANY, &idx,
                               entry_tmp[jx], result, 0) < 0) {
                test_error(unit, "EGR VLAN xlate entry missing at bucket %d\n", bucket);
                rv = -1;
                goto done;
            }
            if (bucket != (idx / bucket_size)) {
                test_error(unit, "EGR VLAN xlate entry inserted into wrong bucket"
                           " Expected %d Actual %d\n", bucket, idx);
                rv = -1;
                goto done;
            }
        }

        if (ad->opt_verbose) {
            cli_out("Cleaning bucket %d\n", bucket);
        }

        /* Remove the entries that we added */
        for (jx = 0; jx < ix; jx++) {
            if (soc_mem_delete(unit, mem, MEM_BLOCK_ALL, entry_tmp[jx]) < 0) {
                test_error(unit, "EGR Vlan xlate delete failed at bucket %d\n", bucket);
                rv = -1;
                goto done;
            }
        }

        /* We want the increment to change buckets by one. For EGR VLAN XLATE,
         * the LSB hash bucket is determined by {ovid}. 
         */
        ovid += 1;
        if (ovid == TR_VID_MAX) {
            ovid = 0;
        }
    }

 done:

    return rv;
}

/*
 * Test clean-up routine used for all EGR Vlan xlate tests
 */

int
tr_egr_vlan_xlate_test_done(int unit, void *p)
{
    tr_hash_testdata_t *ad = p;
    soc_mem_t mem;

    if (ad == NULL) {
	return 0;
    }

    mem = ad->mem;

    /* Check if empty at the end of the test */
    if (ad->opt_reset) {
        int rv, ix;
        int index_min = soc_mem_index_min(unit, mem);
        int index_max = soc_mem_index_max(unit, mem);
        uint32 *buf = 0;
        uint32 *ent;
        uint32 count;

        buf = soc_cm_salloc(unit,
                            SOC_MEM_TABLE_BYTES(unit, mem),
                            "egr_vlan_xlate_test");
        if (!buf) {
            test_error(unit, "Memory allocation failed\n");
            return (-1);
        }

        if ((rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                          index_min, index_max, buf)) < 0) {
            test_error(unit, "Memory DMA of EGR_VLAN_XLATEm entries failed\n");
            return (-1);
        }

        count = soc_mem_index_count(unit, mem);
        for (ix = 0; ix < count; ix++) {
            int vld = 0;
            ent = soc_mem_table_idx_to_pointer(unit, mem,
                                               uint32 *, buf, ix);
            if (soc_feature(unit, soc_feature_base_valid)) {
                vld = soc_mem_field32_get(unit, mem, ent, BASE_VALID_0f) == 3 &&
                      soc_mem_field32_get(unit, mem, ent, BASE_VALID_1f) == 7;
            } else
            {
                vld = soc_mem_field32_get(unit, mem, ent, VALIDf);
            }

            if (vld) {
                test_error(unit, "EGR Vlan xlate table not empty after test entry = %d\n",
                                ix);
                soc_mem_entry_dump(unit, EGR_VLAN_XLATEm, ent, BSL_LSS_CLI);
                return (-1);
            }
        }

        soc_cm_sfree(unit, buf);
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_base_valid)) {
        if (soc_mem_write(unit, EGR_VLAN_XLATE_1_HASH_CONTROLm, MEM_BLOCK_ALL,
                         0, &ad->save_xlate_hash_control) < 0) {
            test_error(unit, "Hash Control restore failed\n");
        }
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
    if (WRITE_EGR_VLAN_XLATE_HASH_CONTROLr(unit, ad->save_hash_control) < 0) {
        test_error(unit, "Hash select restore failed\n");
    }
    }

    return 0;
}
/************************** END OF EGR VLAN XLATE TESTS ********************/

/************************** START OF MPLS TESTS ****************************/
#ifdef BCM_TRIUMPH_SUPPORT
/*
 * Test initialization routine used for MPLS tests
 */

STATIC int
tr_mpls_test_init(int unit, tr_hash_testdata_t *ad, args_t *a)
{
    int                         rv = -1, dual = 0;
    parse_table_t               pt;
    uint32                      hash_read;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Count", PQ_INT|PQ_DFL, 0, &ad->opt_count, NULL);
    parse_table_add(&pt, "Verbose", PQ_BOOL|PQ_DFL, 0, &ad->opt_verbose, NULL);
    parse_table_add(&pt, "Reset", PQ_BOOL|PQ_DFL, 0, &ad->opt_reset, NULL);
    parse_table_add(&pt, "Hash", PQ_INT|PQ_DFL, 0, &ad->opt_hash, NULL);
    parse_table_add(&pt, "DualHash", PQ_INT|PQ_DFL, 0, &ad->opt_dual_hash, NULL);
    parse_table_add(&pt, "DualEnable", PQ_INT|PQ_DFL, 0, &dual, NULL);
    parse_table_add(&pt, "BaseLabel", PQ_INT|PQ_DFL, 0, &ad->opt_base_label, NULL);
    parse_table_add(&pt, "LabelIncrement", PQ_INT|PQ_DFL, 0,
                    &ad->opt_label_inc, NULL);

    /* Test the obvious parsings before wasting time with malloc */
    if (parse_arg_eq(a, &pt) < 0) {
        test_error(unit,
                   "%s: Error: Invalid option: %s\n", ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        goto done;
    }

    if (ad->opt_count < 1) {
        test_error(unit, "Illegal count %d\n", ad->opt_count);
        goto done;
    }

    if (ad->opt_hash >= ad->hash_count) {
        test_error(unit, "Illegal hash selection %d\n", ad->opt_hash);
        goto done;
    }

    if (dual == 1) {
        if (ad->opt_dual_hash >= ad->hash_count) {
            test_error(unit, "Illegal dual hash selection %d\n", ad->opt_dual_hash);
            goto done;
        }
    } else {
        ad->opt_dual_hash = -1;
    }

    if (ad->opt_base_label >= (1 << 20)) {
        test_error(unit, "Out of range MPLS label selection %d\n",
                   ad->opt_base_label);
        goto done;
    }

    /*
     * Re-initialize chip to ensure tables are clear
     * at start of test.
     */

    if (ad->opt_reset) {
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 0));
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

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_td3_style_mpls)) {
        sal_memset(ad->td3_hash_tbl_data, 0, sizeof(ad->td3_hash_tbl_data));
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
            rv = soc_hx5_mpls_hash_control_set(ad->td3_hash_tbl_data);
        } else
#endif
        {
            rv = soc_td3_mpls_hash_control_set(ad->td3_hash_tbl_data);
        }
        if (rv < 0) {
            test_error(unit, "Hash select read failed\n");
            goto done;
        }
    } else
#endif
    {
    if (READ_MPLS_ENTRY_HASH_CONTROLr(unit, &hash_read) < 0) {
        test_error(unit, "Hash select read failed\n");
        goto done;
    }

    ad->save_hash_control = hash_read;

    soc_reg_field_set(unit, MPLS_ENTRY_HASH_CONTROLr, &hash_read,
                      HASH_SELECT_Af, ad->opt_hash);
    if (ad->opt_dual_hash != -1) {
        soc_reg_field_set(unit, MPLS_ENTRY_HASH_CONTROLr, &hash_read,
                          HASH_SELECT_Bf, ad->opt_dual_hash);
    } else {
        soc_reg_field_set(unit, MPLS_ENTRY_HASH_CONTROLr, &hash_read,
                          HASH_SELECT_Bf, ad->opt_hash);
    }

    if (WRITE_MPLS_ENTRY_HASH_CONTROLr(unit, hash_read) < 0) {
        test_error(unit, "Hash select setting failed\n");
        goto done;
    }
    }
    rv = 0;

 done:

    parse_arg_eq_done(&pt);
    return rv;
}

STATIC void
tr_mpls_hash_setup(int unit, tr_hash_test_t *dw)
{
    tr_hash_testdata_t *ad;

    if (dw->lw_set_up) {
        return;
    }

    dw->lw_set_up = TRUE;
    dw->lw_unit = unit;

    /* Hash */
    ad = &dw->lp_hash;

    ad->opt_verbose = FALSE;
    if (!SOC_MEM_IS_VALID (unit, MPLS_ENTRYm))
    {
        ad->opt_count = -1;
        return;
    } 
    ad->opt_count      = soc_mem_index_count(unit, MPLS_ENTRYm);
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_extended_hash)) {
        ad->opt_hash       = FB_HASH_CRC32_LOWER;
        ad->opt_dual_hash  = FB_HASH_CRC32_UPPER;
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    {
        ad->opt_hash       = FB_HASH_CRC16_LOWER;
        ad->opt_dual_hash  = -1;
    }
    ad->hash_count     = FB_HASH_COUNT;
    ad->opt_base_label = 0;
    ad->opt_label_inc  = 1;

    /* Overflow */
    ad = &dw->lp_ov;

    ad->opt_count      = 2048;
    ad->opt_hash       = TR_DEFAULT_HASH;
    ad->opt_dual_hash  = -1;
    ad->hash_count     = FB_HASH_COUNT;
    ad->opt_base_label = 0;
    ad->opt_label_inc  = 1;
}

/* Individual test init wrappers */
int
tr_mpls_hash_test_init(int unit, args_t *a, void **p)
{
    tr_hash_test_t        *dw = &tr_mpls_work[unit];
    tr_hash_testdata_t    *dp = &dw->lp_hash;
    int                    rv;

    tr_mpls_hash_setup(unit, dw);

    /* Set working data to hash */
    dw->lp_cur = dp;

    if ((rv = tr_mpls_test_init(unit, dp, a)) < 0) {
        return rv;
    } else {
        *p = dp;
    }

    return 0;
}

int
tr_mpls_ov_test_init(int unit, args_t *a, void **p)
{
    tr_hash_test_t        *dw = &tr_mpls_work[unit];
    tr_hash_testdata_t    *dp = &dw->lp_ov;
    int                    rv;

    tr_mpls_hash_setup(unit, dw);

    /* Set working data to hash */
    dw->lp_cur = dp;

    if ((rv = tr_mpls_test_init(unit, dp, a)) < 0) {
        return rv; 
    } else {
        *p = dp;
    }

    return 0;
}

/*
 * Test of MPLS XLATE hashing
 *
 *   This test tries a number of keys against one of the hashing functions,
 *   checking a software hash against the hardware hash, then searching the
 *   bucket to find the entry after inserting.
 */
int
tr_mpls_test_hash(int unit, args_t *a, void *p)
{
    tr_hash_testdata_t         *ad = p;
    mpls_entry_entry_t         entry;
    int soft_bucket, ix, r, rv = 0;
    int hash = ad->opt_hash;
    uint8 key[XGS_HASH_KEY_SIZE];
    int iterations, label, num_bits;
    int label_inc = ad->opt_label_inc;
    int bucket_size = 8;
    int index;
    int dual = FALSE;
    int bank, bank_count = 1;

    COMPILER_REFERENCE(a);

    if (ad->opt_verbose) {
        cli_out("Starting MPLS hash test\n");
    }

    iterations = ad->opt_count;
    label = ad->opt_base_label;
    if (ad->opt_dual_hash != -1) {
        dual = TRUE;
        bank_count = 2;
    }

    for (ix = 0; ix < iterations; ix++) {
        for (bank = 0; bank < bank_count; bank++) {
            sal_memset(&entry, 0, sizeof (entry));
            if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                soc_MPLS_ENTRYm_field32_set(unit, &entry, BASE_VALID_0f, 3);
                soc_MPLS_ENTRYm_field32_set(unit, &entry, BASE_VALID_1f, 7);
            } else {
            soc_MPLS_ENTRYm_field32_set(unit, &entry, VALIDf, 1);
            }
            soc_MPLS_ENTRYm_field32_set(unit, &entry, MPLS_LABELf, label);
            num_bits = soc_tr_mpls_base_entry_to_key
                (unit, (uint32 *) &entry, key);
            soft_bucket = soc_tr_mpls_hash(unit, hash, num_bits, 
                                           (uint32 *) &entry, key);
            if (dual == TRUE) {
#ifdef BCM_TRIDENT2_SUPPORT
#ifdef BCM_APACHE_SUPPORT
                if (SOC_IS_APACHE(unit)) {
                    soft_bucket = soc_ap_mpls_bank_entry_hash(unit, bank,
                                                        (uint32 *) &entry);
                } else
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
                if (SOC_IS_TOMAHAWK3(unit) &&
                    soc_feature(unit, soc_feature_base_valid)) {
                    soft_bucket =
                        soc_tomahawk3_mpls_bank_entry_hash(
                                unit, bank, (uint32 *) &entry);
                } else
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
                if (soc_feature(unit, soc_feature_base_valid)) {
                    soft_bucket = soc_td3_mpls_bank_entry_hash(unit, MPLS_ENTRYm, bank,
                                (uint32 *) &entry);
                } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit)) {
                    soft_bucket = soc_th_mpls_bank_entry_hash(unit, bank,
                                                        (uint32 *) &entry);
                } else
#endif /* BCM_TOMAHAWK_SUPPORT */
                if (soc_feature(unit, soc_feature_extended_hash)) {
                    soft_bucket = soc_td2_mpls_bank_entry_hash(unit, bank,
                                                        (uint32 *) &entry);
                } else
#endif /* BCM_TRIDENT2_SUPPORT */
                {
                    soft_bucket = soc_tr_mpls_bank_entry_hash
                        (unit, bank, (uint32 *) &entry);
                }
            }
    
            if (ad->opt_verbose) {
                cli_out("Inserting ");
                soc_mem_entry_dump(unit, MPLS_ENTRYm, &entry, BSL_LSS_CLI);
                cli_out("\n");
                if (dual) {
                    cli_out("into bucket 0x%x (bank %d)\n",
                            soft_bucket, bank);
                } else {
                   cli_out("into bucket 0x%x\n", soft_bucket);
                }
            }
    
            if ((r = soc_mem_bank_insert(unit, MPLS_ENTRYm, bank,
                                         MEM_BLOCK_ALL, &entry, NULL)) < 0) {
                if (r == SOC_E_FULL) {
                    /* Bucket overflow, just pass on */
                    break;
                } else {
                    test_error(unit,
                               "MPLS insert failed at bucket %d\n", soft_bucket);
                    rv = -1;
                    goto done;
                }
            }
    
            /* Now we search for the entry */
            if (!soc_feature(unit, soc_feature_shared_hash_mem)) {
                /* Only do a quick check vs. expected bucket here */
                if (tr_hash_bucket_search(unit, ad, MPLS_ENTRYm, soft_bucket,
                                      bucket_size, VALIDf, &entry, dual, bank) < 0) {
                    test_error(unit,
                           "MPLS entry with key "
                           "0x%02x%02x%02x%02x%02x%02x%02x%01x "
                           "not found in predicted bucket %d\n",
                           key[7], key[6], key[5], key[4],
                           key[3], key[2], key[1], (key[0] >> 4) & 0xf,
                           soft_bucket);
                    rv = -1;
                    goto done;
                }
            }
    
            /* Search for the entry, should be found */
            if (soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &index,
                               &entry, &entry, 0) < 0) {
                test_error(unit, "MPLS search failed in bucket %d\n",
                           soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Insert the entry again, should fail. */
            if (soc_mem_bank_insert(unit, MPLS_ENTRYm, bank,
                                    MEM_BLOCK_ALL, &entry, NULL) != SOC_E_EXISTS) {
                test_error(unit,
                           "MPLS insert should have failed at bucket %d\n", soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Delete the entry */
            if (soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &entry) < 0) {
                test_error(unit, "MPLS delete failed at bucket %d\n",
                           soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Delete the entry again, should fail.*/
            if (soc_mem_delete(unit, MPLS_ENTRYm,
                               MEM_BLOCK_ALL, &entry) != SOC_E_NOT_FOUND) {
                test_error(unit, "MPLS delete should have failed in bucket %d\n",
                           soft_bucket);
                rv = -1;
                goto done;
            }
    
            /* Search for the entry again, should not be found */
            if (soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &index,
                               &entry, &entry, 0) != SOC_E_NOT_FOUND) {
                test_error(unit, "MPLS search should have failed in bucket %d\n",
                           soft_bucket);
                rv = -1;
                goto done;
            }
        }
        label += label_inc;
        if (label > TR_LABEL_MAX) {
            label = 1;
        }
    }

 done:
    return rv;
}

/*
 * Test of MPLS overflow behavior
 *
 *   This test fills each bucket, then inserts another entry to see
 *   that the last entry fails to insert.
 */

int
tr_mpls_test_ov(int unit, args_t *a, void *p)
{
    tr_hash_testdata_t  *ad = p;
    mpls_entry_entry_t  entry, result, entry_tmp[8];
    int                 ix, jx, r, idx, rv = 0;
    int port, module, label;
    int bucket = 0;
    uint32 hash = ad->opt_hash;
    int iter = ad->opt_count;
    uint8 key[XGS_HASH_KEY_SIZE];
    int bucket_size = 8;

    COMPILER_REFERENCE(a);

    if (hash != FB_HASH_LSB) {
        if (ad->opt_verbose) {
            cli_out("Resetting hash selection to LSB\n");
        }

        hash = ad->save_hash_control;
        soc_reg_field_set(unit, MPLS_ENTRY_HASH_CONTROLr, &hash,
                          HASH_SELECT_Af, FB_HASH_LSB);
        soc_reg_field_set(unit, MPLS_ENTRY_HASH_CONTROLr, &hash,
                          HASH_SELECT_Bf, FB_HASH_LSB);
    
        if (WRITE_MPLS_ENTRY_HASH_CONTROLr(unit, hash) < 0) {
            test_error(unit, "Hash select setting failed\n");
            goto done;
        }

        ad->opt_hash = hash = FB_HASH_LSB;
    }

    if (iter > soc_mem_index_count(unit, MPLS_ENTRYm)) {
        iter = soc_mem_index_count(unit, MPLS_ENTRYm);
    }
    port = 0;
    module = 0;
    label = 0;

    while (iter--) {
        for (ix = 0; ix < bucket_size; ix++) {
            sal_memset(&entry_tmp[ix], 0, sizeof(mpls_entry_entry_t));
            if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                soc_MPLS_ENTRYm_field32_set(unit, &(entry_tmp[ix]), BASE_VALID_0f, 3);
                soc_MPLS_ENTRYm_field32_set(unit, &(entry_tmp[ix]), BASE_VALID_1f, 7);
            } else {
                soc_MPLS_ENTRYm_field32_set(unit, &(entry_tmp[ix]), VALIDf, 1);
            }
            soc_MPLS_ENTRYm_field32_set(unit, &(entry_tmp[ix]), PORT_NUMf, port);
            soc_MPLS_ENTRYm_field32_set(unit, &(entry_tmp[ix]), MODULE_IDf, module);
            soc_MPLS_ENTRYm_field32_set(unit, &(entry_tmp[ix]), MPLS_LABELf, label);

            if (ix == 0) {
                int num_bits;
#ifdef BCM_HURRICANE4_SUPPORT
                if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
                    int phy_bank;
                    soc_hx5_hash_bank_number_get(unit, MPLS_ENTRYm, 0, &phy_bank);
                    bucket =
                        soc_hx5_mpls_bank_entry_hash(unit, MPLS_ENTRYm, phy_bank,
                                                     (uint32 *)&entry_tmp[ix]);
                } else
#endif
                {
                num_bits = soc_tr_mpls_base_entry_to_key
                    (unit, (uint32 *) &(entry_tmp[ix]), key);
                bucket = soc_tr_mpls_hash(unit, hash, num_bits, 
                                          (uint32 *) &(entry_tmp[ix]), key);
                }
                if (ad->opt_verbose) {
                    cli_out("Filling bucket %d\n", bucket);
                }
            }

            if ((r = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &entry_tmp[ix])) < 0) {
                if (r == SOC_E_FULL) {
                    /* Already full, stop wasting time */
                    break;
                } else {
                    test_error(unit,
                               "MPLS insert failed at bucket %d\n", bucket);
                    rv = -1;
                    goto done;
                }
            }

            /* key for LSB is the label, so we must keep it constant */
            port += 1;
            if (port > 0x3f) {
    /*    coverity[assignment : FALSE]    */
                port = 0;
    /* coverity[assignment] */
                module += 1;
                if (module > 0x7f) {
                    module = 0;
                }
            }
        }

        if (ad->opt_verbose) {
            cli_out("Inserting %dth entry in bucket %d, should fail\n",
                    (bucket_size + 1), bucket);
        }

        sal_memset(&entry, 0,  sizeof(mpls_entry_entry_t));
        if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                soc_MPLS_ENTRYm_field32_set(unit, &entry, BASE_VALID_0f, 3);
                soc_MPLS_ENTRYm_field32_set(unit, &entry, BASE_VALID_1f, 7);
            } else {
                soc_MPLS_ENTRYm_field32_set(unit, &entry, VALIDf, 1);
        }
        soc_MPLS_ENTRYm_field32_set(unit, &entry, PORT_NUMf, port);
        soc_MPLS_ENTRYm_field32_set(unit, &entry, MODULE_IDf, module);
        soc_MPLS_ENTRYm_field32_set(unit, &entry, MPLS_LABELf, label);

        if ((r = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &entry)) < 0) {
            if (r != SOC_E_FULL) {
                test_error(unit,
                           "MPLS insert failed\n");
                rv = -1;
                goto done;
            }
        } else {
            test_error(unit, "MPLS insert to full bucket succeeded\n");
            rv = -1;
            goto done;
        }

        if (ad->opt_verbose) {
            cli_out("Verifying entries present\n");
        }

        /* Verify bucket contains our added entries */
        for (jx = 0; jx < ix; jx++) {
            if (tr_hash_bucket_search(unit, ad, MPLS_ENTRYm, bucket, 
                                      bucket_size, VALIDf, &(entry_tmp[jx]), 0, 0) < 0) {
                test_error(unit, "MPLS entry missing at bucket %d\n", bucket);
                rv = -1;
                goto done;
            }
            if (soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &idx, 
                               &entry_tmp[jx], &result, 0) < 0) {
                test_error(unit, "MPLS entry missing at bucket %d\n", bucket);
                rv = -1;
                goto done;
            }
            if (bucket != (idx / bucket_size)) {
                test_error(unit, "MPLS entry inserted into wrong bucket"
                           " Expected %d Actual %d\n", bucket, idx);
                rv = -1;
                goto done;
            }
        }

        if (ad->opt_verbose) {
            cli_out("Cleaning bucket %d\n", bucket);
        }

        /* Remove the entries that we added */
        for (jx = 0; jx < ix; jx++) {
            if (soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &(entry_tmp[jx])) < 0) {
                test_error(unit, "MPLS delete failed at bucket %d\n", bucket);
                rv = -1;
                goto done;
            }
        }

        /* We want the increment to change buckets by one. For MPLS_ENTRYm,
         * the LSB hash bucket is determined by {label}. 
         */
        label += 1;
        if (label > TR_LABEL_MAX) {
            label = 0;
        }
    }

 done:

    return rv;
}

/*
 * Test clean-up routine used for all MPLS tests
 */

int
tr_mpls_test_done(int unit, void *p)
{
    tr_hash_testdata_t *ad = p;

    if (ad == NULL) {
	return 0;
    }

    /* Check if empty at the end of the test */
    if (ad->opt_reset) {
        int rv, ix;
        int index_min = soc_mem_index_min(unit, MPLS_ENTRYm);
        int index_max = soc_mem_index_max(unit, MPLS_ENTRYm);
        uint32 *buf = 0;
        uint32 *ent;
        uint32 count;

        buf = soc_cm_salloc(unit,
                            SOC_MEM_TABLE_BYTES(unit, MPLS_ENTRYm),
                            "mpls_entry_test");
        if (!buf) {
            test_error(unit, "Memory allocation failed\n");
            return (-1);
        }

        if ((rv = soc_mem_read_range(unit, MPLS_ENTRYm, MEM_BLOCK_ANY,
                          index_min, index_max, buf)) < 0) {
            test_error(unit, "Memory DMA of MPLS_ENTRYm entries failed\n");
            return (-1);
        }

        count = soc_mem_index_count(unit, MPLS_ENTRYm);
        for (ix = 0; ix < count; ix++) {
            ent = soc_mem_table_idx_to_pointer(unit, MPLS_ENTRYm,
                                               uint32 *, buf, ix);

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
            if (soc_feature(unit, soc_feature_base_valid)) {
                if ((soc_MPLS_ENTRYm_field32_get(unit, ent, BASE_VALID_0f) == 3) &&
                        (soc_MPLS_ENTRYm_field32_get(unit, ent, BASE_VALID_1f) == 7)) {
                    test_error(unit, "MPLS_ENTRY table not empty after test entry = %d\n",
                            ix);
                    soc_mem_entry_dump(unit, MPLS_ENTRYm, ent, BSL_LSS_CLI);
                    return (-1);
                }
            } else
#endif
            {
            if (soc_MPLS_ENTRYm_field32_get(unit, ent, VALIDf)) {
                test_error(unit, "MPLS_ENTRY table not empty after test entry = %d\n",
                                ix);
                soc_mem_entry_dump(unit, MPLS_ENTRYm, ent, BSL_LSS_CLI);
                return (-1);
            }
        }
        }

        soc_cm_sfree(unit, buf);
    }

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_base_valid)) {
        if (WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                    ad->td3_hash_tbl_data) < 0) {
            test_error(unit, "Hash select restore failed\n");
        }
    } else
#endif
    {
    if (WRITE_MPLS_ENTRY_HASH_CONTROLr(unit, ad->save_hash_control) < 0) {
        test_error(unit, "Hash select restore failed\n");
    }
    }

    return 0;
}
#endif /* BCM_TRIUMPH_SUPPORT */
/************************** END OF MPLS TESTS ******************************/
#endif /* BCM_TRX_SUPPORT */

