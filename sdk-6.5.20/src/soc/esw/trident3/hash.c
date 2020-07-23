/*
 * *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hash.c
 * Purpose:     Trident3 hash table calculation routines
 * Requires:
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/hash.h>
#include <soc/esw/flow_db.h>

#if defined(BCM_TRIDENT3_SUPPORT)

#include <soc/trident3.h>

int
soc_td3_tbl_entries_per_sram_entry_get(int unit, soc_mem_t mem,
                       int *table_entries_per_sram_entry)
{
    switch (mem) {
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_ONLY_SINGLEm:
         *table_entries_per_sram_entry = 4;
         break;

    case L3_ENTRY_DOUBLEm:
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
         *table_entries_per_sram_entry = 2;
         break;

    case L3_ENTRY_QUADm:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
         *table_entries_per_sram_entry = 1;
         break;

    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_2_SINGLEm:
    case MPLS_ENTRY_SINGLEm:
         *table_entries_per_sram_entry = 2;
         break;

    case VLAN_XLATE_1_DOUBLEm:
    case VLAN_XLATE_2_DOUBLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
    case MPLS_ENTRYm:
         *table_entries_per_sram_entry = 1;
         break;

    default:
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

int
soc_td3_shared_hash_mem_bucket_read(int unit, int ent, int entry_width, int *width,
                                    soc_mem_t base_mem, soc_mem_t *mem, 
                                    void *bkt_entry, void *entry)
{

    soc_mem_info_t meminfo;
    soc_field_info_t fieldinfo;
    /* Exclude HIT bit */
    int entry_bits = 0, hit_bits=0;
    if (entry_width <= 0) {
        return (SOC_E_PARAM);
    }

    switch (base_mem) {
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
        switch (entry_width) {
        case 1:  /* 1 base_entry wide */
            *width = 1;
            *mem = L3_ENTRY_SINGLEm;
            break;
        case 2:  /* 2 base entries wide */
            *width = 2;
            *mem = L3_ENTRY_DOUBLEm;
            break;
        default:
            *width = 4; /* 4 base_entries wide */
            *mem = L3_ENTRY_QUADm;
        }
        hit_bits = *width;
        break;

    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_4_PIPE0m:
        switch (entry_width) {
            case 2:
                *width = 1;
                *mem = EXACT_MATCH_2_PIPE0m;
                break;
            case 4:
            default:
                *width = 2;
                *mem = EXACT_MATCH_4_PIPE0m;
        }
        hit_bits = *width;
        break;

    case EXACT_MATCH_2_PIPE1m:
    case EXACT_MATCH_4_PIPE1m:
        switch (entry_width) {
            case 2:
                *width = 1;
                *mem = EXACT_MATCH_2_PIPE1m;
                break;
            case 4:
            default:
                *width = 2;
                *mem = EXACT_MATCH_4_PIPE1m;
        }
        hit_bits = *width;
        break;

    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
        switch (entry_width) {
            case 2:
                *width = 1;
                *mem = EXACT_MATCH_2m;
                break;
            case 4:
            default:
                *width = 2;
                *mem = EXACT_MATCH_4m;
        }
        hit_bits = *width;
        break;

    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
        switch (entry_width) {
        case 1:
            *width = 1;
            *mem = VLAN_XLATE_1_SINGLEm;
            break;
        case 2:
        default:
            *width = 2;
            *mem = VLAN_XLATE_1_DOUBLEm;
        }
        break;

    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:
        switch (entry_width) {
        case 1:
            *width = 1;
            *mem = VLAN_XLATE_2_SINGLEm;
            break;
        case 2:
        default:
            *width = 2;
            *mem = VLAN_XLATE_2_DOUBLEm;
        }
        break;

    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
        switch (entry_width) {
        case 1:
            *width = 1;
            *mem = EGR_VLAN_XLATE_1_SINGLEm;
            break;
        case 2:
        default:
            *width = 2;
            *mem = EGR_VLAN_XLATE_1_DOUBLEm;
        }
        break;

    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
        switch (entry_width) {
        case 1:
            *width = 1;
            *mem = EGR_VLAN_XLATE_2_SINGLEm;
            break;
        case 2:
        default:
            *width = 2;
            *mem = EGR_VLAN_XLATE_2_DOUBLEm;
        }
        break;

    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
        switch (entry_width) {
        case 1:
            *width = 1;
            *mem = MPLS_ENTRY_SINGLEm;
            break;
        case 2:
        default:
            *width = 2;
            *mem = MPLS_ENTRYm;
        }
        break;

    default:
        return SOC_E_PARAM;
    }

    entry_bits = soc_mem_entry_bits(unit, *mem) - hit_bits;

    meminfo.flags = 0;
    meminfo.bytes = SOC_MEM_WORDS(unit, base_mem) * 4;
    fieldinfo.flags = SOCF_LE;
    fieldinfo.len = entry_bits;

    fieldinfo.bp =  entry_bits * (ent/entry_width);
    (void)soc_meminfo_fieldinfo_field_get(bkt_entry, &meminfo,
                                          &fieldinfo, entry);
    return SOC_E_NONE;

}


int
soc_td3_hash_mem_status_get(int unit, soc_mem_t mem, void* entry, uint32 *v) {
    int i;
    v[0] = v[1] = v[2] = v[3] = 0;

    switch (mem) {
    case L3_ENTRY_QUADm:
    case EXACT_MATCH_4m:
        soc_mem_field_get(unit, mem, entry, BASE_VALID_3f, &v[3]);
        soc_mem_field_get(unit, mem, entry, BASE_VALID_2f, &v[2]);
        /* pass through */
    case L3_ENTRY_DOUBLEm:
    case EXACT_MATCH_2m:
    case MPLS_ENTRYm:
    case VLAN_XLATE_1_DOUBLEm:
    case VLAN_XLATE_2_DOUBLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
        soc_mem_field_get(unit, mem, entry, BASE_VALID_1f, &v[1]);
        soc_mem_field_get(unit, mem, entry, BASE_VALID_0f, &v[0]);
        break;
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_ONLY_SINGLEm:
    case MPLS_ENTRY_SINGLEm:
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_2_SINGLEm:
        soc_mem_field_get(unit, mem, entry, BASE_VALIDf, &v[0]);
        break;
    default:
        v[0] = v[1] = v[2] = v[3] = 1;
        break;

    }
    /* Use v[i] for constructing a valid bitmap */
    for (i = 0; i < 4; i++) {
        v[i] = (v[i] > 0);
    }
    return SOC_E_NONE;
}

STATIC int
soc_td3_iuat_hash_bank_count_get(int unit, soc_mem_t mem, int *num_16k_banks,
                                 int *num_8k_banks) {
    int count;
    int uat_16k_banks_other, uat_8k_banks_other;
    int uat_16k_banks = 0, uat_8k_banks = 0;
    
    switch (mem) {
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
        count = soc_mem_index_count(unit, MPLS_ENTRY_SINGLEm);
        SOC_IF_ERROR_RETURN(soc_trident3_iuat_sizing(unit, count,
                                            &uat_16k_banks, &uat_8k_banks));
        *num_16k_banks = uat_16k_banks;
        *num_8k_banks = uat_8k_banks;
        break;

    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
        count = soc_mem_index_count(unit, VLAN_XLATE_1_SINGLEm);
        SOC_IF_ERROR_RETURN(soc_trident3_iuat_sizing(unit, count,
                                            &uat_16k_banks, &uat_8k_banks));
        uat_16k_banks_other = uat_16k_banks;
        uat_8k_banks_other = uat_8k_banks;
        count = soc_mem_index_count(unit, VLAN_XLATE_1_SINGLEm);
        SOC_IF_ERROR_RETURN(soc_trident3_iuat_sizing(unit, count,
                                            &uat_16k_banks, &uat_8k_banks));
        *num_16k_banks = uat_16k_banks - uat_16k_banks_other;
        *num_8k_banks = uat_8k_banks - uat_8k_banks_other;
        break;

    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:
        count = soc_mem_index_count(unit, VLAN_XLATE_2_SINGLEm);
        SOC_IF_ERROR_RETURN(soc_trident3_iuat_sizing(unit, count,
                                            &uat_16k_banks, &uat_8k_banks));
        count = soc_mem_index_count(unit, VLAN_XLATE_1_SINGLEm);
        SOC_IF_ERROR_RETURN(soc_trident3_iuat_sizing(unit, count,
                                            &uat_16k_banks, &uat_8k_banks));
        uat_16k_banks_other = uat_16k_banks;
        uat_8k_banks_other = uat_8k_banks;
        count = soc_mem_index_count(unit, VLAN_XLATE_2_SINGLEm);
        SOC_IF_ERROR_RETURN(soc_trident3_iuat_sizing(unit, count,
                                            &uat_16k_banks, &uat_8k_banks));
        *num_16k_banks = uat_16k_banks - uat_16k_banks_other;
        *num_8k_banks = uat_8k_banks - uat_8k_banks_other;
        break;
    }
    return SOC_E_NONE;
}

/* Get number of banks for the hash table according to the config
 */
int
soc_trident3_hash_bank_count_get(int unit, soc_mem_t mem, int *num_banks)
{
    int count;
    int uat_16k_banks = 0, uat_8k_banks = 0;

    switch (mem) {
    case L2Xm:
        /*
         * 16k single wide entries in each of 2 dedicated L2 bank
         * 32k single wide entries in each shared bank (8 shared banks max)
         */
        count = soc_mem_index_count(unit, mem);
        *num_banks = 2 + (count - 2 * 16 * 1024) / (32 * 1024);
        break;
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_ONLY_DOUBLEm:
    case L3_ENTRY_ONLY_QUADm:
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
        /*
         * 8k single wide entries in each of 2 dedicated L3 bank
         * 32k entries in each shared bank (8 shared banks max)
         */
        count = soc_mem_index_count(unit, L3_ENTRY_ONLY_SINGLEm);
        *num_banks = 2 + (count - 2 * 8 * 1024) / (32 * 1024);
        break;

    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
    case EXACT_MATCH_2_ENTRY_ONLYm:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
    case EXACT_MATCH_4_ENTRY_ONLYm:
        /*
         * 16k double wide entries in each shared bank
         */
        count = soc_mem_index_count(unit, EXACT_MATCH_2m);
        *num_banks = count / (16 * 1024);
        break;

    /* EUAT */
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
        /*
         * 8k single wide entries in each shared bank
         */
        count = soc_mem_index_count(unit, EGR_VLAN_XLATE_1_SINGLEm);
        *num_banks = count / (8 * 1024);
        break;

    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
        /*
         * 8k single wide entries in each shared bank
         */
        count = soc_mem_index_count(unit, EGR_VLAN_XLATE_2_SINGLEm);
        *num_banks = count / (8 * 1024);
        break;

    case ING_VP_VLAN_MEMBERSHIPm:
    case EGR_VP_VLAN_MEMBERSHIPm:
    case ING_DNAT_ADDRESS_TYPEm:
    case SUBPORT_ID_TO_SGPP_MAPm:
        *num_banks = 2;
        break;

    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:
        SOC_IF_ERROR_RETURN(soc_td3_iuat_hash_bank_count_get(unit, mem,
                                                &uat_16k_banks, &uat_8k_banks));
        *num_banks = uat_16k_banks + uat_8k_banks;
        break;

    default:
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

/* Map logical bank to physical bank for a memory 
 * mem          - memory
 * log_bank     - logical bank number
 * phy_bank_id  - physical bank number
 */
int
soc_td3_log_to_phy_bank_map(int unit, soc_mem_t mem, int log_bank,
                            int *phy_bank_id)
{
    uint32 hash_control_entry[4];
    int done = 0;
    static const soc_field_t logical_to_physical_fields[] = {
        LOGICAL_BANK_0_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_4_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_5_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_6_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_7_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_8_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_9_PHYSICAL_BANK_LOCATIONf
    };
    int logical_to_physical_fields_num = sizeof(logical_to_physical_fields)/
                                         sizeof(soc_field_t );

    if(log_bank < 0 || log_bank >= logical_to_physical_fields_num) {
        return SOC_E_PARAM;
    }

    sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));

    switch(mem) {
    case L2Xm:
        /* Dedicated Banks */
        if (log_bank < 2) {
            *phy_bank_id = log_bank;
            done = 1;
        } else { /* Shared Banks */
            mem = L2_ENTRY_HASH_CONTROLm;
        }
        break;
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_ONLY_DOUBLEm:
    case L3_ENTRY_QUADm:
    case L3_ENTRY_ONLY_QUADm:
        /* Dedicated Banks */
        if (log_bank < 2) {
            *phy_bank_id = log_bank + 10;
            done = 1;
        } else { /* Shared Banks */
            mem = L3_ENTRY_HASH_CONTROLm;
        }
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
        if (log_bank < 0 || log_bank > 7) {
            return SOC_E_PARAM;
        } else {
            mem = EXACT_MATCH_HASH_CONTROLm;
        }
        break;
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
        if (log_bank < 0 || log_bank > 7) {
            return SOC_E_PARAM;
        } else {
            mem = MPLS_ENTRY_HASH_CONTROLm;
        }
        break;
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
        if (log_bank < 0 || log_bank > 7) {
            return SOC_E_PARAM;
        } else {
            mem = VLAN_XLATE_1_HASH_CONTROLm;
        }
        break;
    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:
        if (log_bank < 0 || log_bank > 7) {
            return SOC_E_PARAM;
        } else {
            mem = VLAN_XLATE_2_HASH_CONTROLm;
        }
        break;
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
        if (log_bank < 0 || log_bank > 3) {
            return SOC_E_PARAM;
        } else {
            mem = EGR_VLAN_XLATE_1_HASH_CONTROLm;
        }
        break;
    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
        if (log_bank < 0 || log_bank > 3) {
            return SOC_E_PARAM;
        } else {
            mem = EGR_VLAN_XLATE_2_HASH_CONTROLm;
        }
        break;
    default:
        *phy_bank_id = log_bank;
        done = 1;
    }
    /* Shared Banks */
    if (done == 0) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, 0,
                                         hash_control_entry));
        *phy_bank_id = soc_mem_field32_get(unit, mem, hash_control_entry,
                                           logical_to_physical_fields[log_bank]);
    }
    return SOC_E_NONE;
}

/* Map physical to logical bank for a memory 
 * mem      - memory
 * phy_bank - physical bank number
 * log_bank - logical bank number
 */
int
soc_td3_phy_to_log_bank_map(int unit, soc_mem_t mem, int phy_bank,
                            int *log_bank)
{
    uint32 hash_control_entry[4];
    int i, done = 0;
    int bank_min, bank_max;
    static const soc_field_t logical_to_physical_fields[] = {
        LOGICAL_BANK_0_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_4_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_5_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_6_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_7_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_8_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_9_PHYSICAL_BANK_LOCATIONf
    };

    sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));

    switch(mem) {
    case L2Xm:
        bank_min = 2;
        bank_max = 9;
        /* Dedicated Banks */
        if (phy_bank < 2) {
            *log_bank = phy_bank;
            done = 1;
        } else { /* Shared Banks */
            mem = L2_ENTRY_HASH_CONTROLm;
        }
        break;
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_ONLY_DOUBLEm:
    case L3_ENTRY_QUADm:
    case L3_ENTRY_ONLY_QUADm:
        bank_min = 2;
        bank_max = 11;
        /* Dedicated Banks */
        if (phy_bank > 9) {
            *log_bank = phy_bank - 10;
            done = 1;
        } else { /* Shared Banks */
            mem = L3_ENTRY_HASH_CONTROLm;
        }
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
        bank_min = 0;
        bank_max = 7;
        mem = EXACT_MATCH_HASH_CONTROLm;
        break;
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
        bank_min = 0;
        bank_max = 7;
        mem = MPLS_ENTRY_HASH_CONTROLm;
        break;
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
        bank_min = 0;
        bank_max = 7;
        mem = VLAN_XLATE_1_HASH_CONTROLm;
        break;
    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:
        bank_min = 0;
        bank_max = 7;
        mem = VLAN_XLATE_2_HASH_CONTROLm;
        break;
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
        bank_min = 0;
        bank_max = 7;
        mem = EGR_VLAN_XLATE_1_HASH_CONTROLm;
        break;
    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
        bank_min = 0;
        bank_max = 7;
        mem = EGR_VLAN_XLATE_2_HASH_CONTROLm;
        break;
    default:
        *log_bank = phy_bank;
        done = 1;
    }

    /* Shared Banks */
    if (done == 0) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, 0,
                                         hash_control_entry));
        /* If Shared banks are used between ALPM and non-ALPM memories,
         * then ALPM uses Shared Bank B2, B3, B4, B5 and non-ALPM uses
         * B6, B7, B8, B9 banks. Max physical bank number used for UFT is 9.
         */
        if (_soc_alpm_mode[unit] && ((mem == L2_ENTRY_HASH_CONTROLm) ||
                            (mem == L3_ENTRY_HASH_CONTROLm) ||
                            (mem == EXACT_MATCH_HASH_CONTROLm)) &&
                            (phy_bank <=5)) {
            phy_bank += 4;
        }
        for (i = bank_min; i <= bank_max; i++) {
            if(soc_mem_field32_get(unit, mem, hash_control_entry,
                                   logical_to_physical_fields[i]) == phy_bank) {
                *log_bank = i;
                done = 1;
                break;
            }
        }
    }
    return (done ? SOC_E_NONE : SOC_E_PARAM);
}

/* Get bank bitmap for the hash table 
 * Required only for shared hash tables
 */
int
soc_td3_hash_bank_phy_bitmap_get(int unit, soc_mem_t mem, uint32 *bitmap)
{
    uint32 hash_control_entry[4];
    switch(mem) {
    case L2Xm:
        mem = L2_ENTRY_HASH_CONTROLm;
        break;
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_ONLY_DOUBLEm:
    case L3_ENTRY_QUADm:
    case L3_ENTRY_ONLY_QUADm:
        mem = L3_ENTRY_HASH_CONTROLm;
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
        mem = EXACT_MATCH_HASH_CONTROLm;
        break;
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
        mem = MPLS_ENTRY_HASH_CONTROLm;
        break;
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
        mem = VLAN_XLATE_1_HASH_CONTROLm;
        break;
    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:
        mem = VLAN_XLATE_2_HASH_CONTROLm;
        break;
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
        mem = EGR_VLAN_XLATE_1_HASH_CONTROLm;
        break;
    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
        mem = EGR_VLAN_XLATE_2_HASH_CONTROLm;
        break;
    default:
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, 0,
                                     hash_control_entry));
    *bitmap = soc_mem_field32_get(unit, mem, hash_control_entry,
                                  HASH_TABLE_BANK_CONFIGf);

    return SOC_E_NONE;
}

/* Map bank sequence number to physical bank number */
int
soc_td3_hash_bank_number_get(int unit, soc_mem_t mem, int seq_num,
                             int *bank_num) {
    int count;

    SOC_IF_ERROR_RETURN(soc_trident3_hash_bank_count_get(unit, mem, &count));
    if (seq_num < 0 || seq_num >= count) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(soc_td3_log_to_phy_bank_map(unit, mem, seq_num,
                                                        bank_num));

    return SOC_E_NONE;
}

int
soc_td3_hash_seq_number_get(int unit, soc_mem_t mem, int bank_num, int *seq_num)
{
    int count;

    SOC_IF_ERROR_RETURN(soc_td3_phy_to_log_bank_map(unit, mem, bank_num,
                                                    seq_num));

    SOC_IF_ERROR_RETURN(soc_trident3_hash_bank_count_get(unit, mem, &count));
    if (*seq_num < 0 || *seq_num >= count) {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}


int
soc_td3_hash_bank_info_get(int unit, soc_mem_t mem, int bank,
                           int *entries_per_bank, int *entries_per_row,
                           int *entries_per_bucket, int *bank_base,
                           int *bucket_offset)
{
    int bank_size, row_size, bucket_size, base, offset;
    int seq_num = 0;
    int uat_8k_banks = 0, uat_16k_banks = 0;

    /* Get logical bank number from input physical bank number */
    SOC_IF_ERROR_RETURN(soc_td3_phy_to_log_bank_map(unit, mem, bank, &seq_num));
    bank = seq_num;

    switch(mem) {
    case L2Xm:
        /*
         * 4 entries per bucket (1 bucket per row)
         * bank 0 - 1 -> dedicated banks - 16k entries each
         * bank 2 - 9 -> shared banks - 32k entries each
         */
        row_size = 4;
        bucket_size = 4;
        offset = 0;
        if (bank < 0 || bank > 9) {
            return SOC_E_INTERNAL;
        } else if (bank < 2) { /* Dedicated bank */
            bank_size = 16384;
            base = bank * bank_size;
        } else { /* Shared bank */
            bank_size = 32768;
            base = 32768 + (bank - 2) * bank_size;
        }
        break;
    case L3_ENTRY_ONLY_SINGLEm:
#ifdef BCM_TRIDENT3_SUPPORT
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
#endif /* BCM_TRIDENT3_SUPPORT */
        /*
         * 4 entries per bucket (1 bucket per row)
         * bank 0 - 1 -> dedicated banks - 8k entries each
         * bank 2 - 9 -> shared banks - 32k entries each
         */
        row_size = 4;
        bucket_size = 4;
        offset = 0;
        if (bank < 0 || bank > 9) {
            return SOC_E_INTERNAL;
        } else if(bank < 2) { /* Dedicated bank */
            if (mem == L3_ENTRY_QUADm) {
                bank_size = 8192/4;
                base = bank * bank_size;
            } else if (mem == L3_ENTRY_DOUBLEm) {
                bank_size = 8192/2;
                base = bank * bank_size;
            } else {
                bank_size = 8192;
                base = bank * bank_size;
            }
        } else { /* Shared bank */
            bank_size = 32768;
            if (mem == L3_ENTRY_QUADm) {
                bank_size = bank_size/4;
                base = 16384/4;
            } else if (mem == L3_ENTRY_DOUBLEm) {
                bank_size = bank_size/2;
                base = 16384/2;
            } else {
                base = 16384;
            }
            base += ((bank-2)*bank_size);
        }
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
        /* 2 entries per row
         * bank 0 - 7 -> shared banks - 16k entries each
         */
        row_size = 2;
        bucket_size = 4;
        offset = 0;
        bank_size = 16384;
        base = bank * bank_size;
        break;
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
        /* 1 entries per row
         * bank 0 - 7 -> shared banks - 8k entries each
         */
        row_size = 1;
        bucket_size = 4;
        offset = 0;
        bank_size = 8192;
        base = bank * bank_size;
        break;        
    case EGR_VLAN_XLATE_1_DOUBLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_2_SINGLEm:
        /* 4 entries per bucket
         * bank 0 - 3 -> shared banks - 8k entries each
         */
        row_size = ((mem == EGR_VLAN_XLATE_1_DOUBLEm) ||
                    (mem == EGR_VLAN_XLATE_2_DOUBLEm)) ? 2 : 4;
        bucket_size = 4;
        offset = 0;
        bank_size = 8192;
        if ((mem == EGR_VLAN_XLATE_1_DOUBLEm) ||
            (mem == EGR_VLAN_XLATE_2_DOUBLEm)) {
            bank_size /= 2;
        }
        base = bank * bank_size;
        break;
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
    case VLAN_XLATE_1_DOUBLEm:
    case VLAN_XLATE_2_DOUBLEm:
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_2_SINGLEm:
        SOC_IF_ERROR_RETURN(soc_td3_iuat_hash_bank_count_get(unit, mem,
                                            &uat_16k_banks, &uat_8k_banks));
        row_size = ((mem == VLAN_XLATE_1_DOUBLEm) ||
                    (mem == VLAN_XLATE_2_DOUBLEm) ||
                    (mem == MPLS_ENTRYm)) ? 2 : 4;
        bucket_size = 4;
        offset = 0;
        if (bank < uat_8k_banks) {
            bank_size = 8192;
            base = bank * bank_size;
        } else {
            bank_size = 16384;
            base = 8192 * uat_8k_banks + bank_size * (bank - uat_8k_banks);
        }
        if ((mem == MPLS_ENTRYm) ||
            (mem == VLAN_XLATE_1_DOUBLEm) ||
            (mem == VLAN_XLATE_2_DOUBLEm)) {
            base /= 2;
        }
        break;
    case ING_VP_VLAN_MEMBERSHIPm:
    case EGR_VP_VLAN_MEMBERSHIPm:
    case ING_DNAT_ADDRESS_TYPEm:
    case SUBPORT_ID_TO_SGPP_MAPm:
        /* 
         * First half of entries in each row are for bank 0
         * Second half of entries are for bank 1
         */
        bank_size = soc_mem_index_count(unit, mem) / 2;
        row_size = 4;
        bucket_size = 4;
        base = bank * bank_size;
        offset = 0;
        break;
    default:
        return SOC_E_INTERNAL;
    }

    if (entries_per_bank != NULL) {
        *entries_per_bank = bank_size;
    }
    if (entries_per_row != NULL) {
        *entries_per_row = row_size;
    }
    if (entries_per_bucket != NULL) {
        *entries_per_bucket = bucket_size;
    }
    if (bank_base != NULL) {
        *bank_base = base;
    }
    if (bucket_offset != NULL) {
        *bucket_offset = offset;
    }
    return SOC_E_NONE;
}

STATIC int
_soc_td3_hash_entry_to_key(int unit, int bank, void *entry, uint8 *key,
          soc_mem_t mem, soc_field_t *field_list, soc_mem_t mem_view)
{
    soc_field_t field;
    int index, key_index, val_index, fval_index;
    int right_shift_count, left_shift_count;
    uint32 val[SOC_MAX_MEM_WORDS], fval[SOC_MAX_MEM_WORDS];
    int bits, val_bits, fval_bits, nbits;
    int8 field_length[16];

#ifdef SOC_ROBUST_HASH
    soc_robust_hash_config_t *rbh_cfg = NULL;
#endif /* SOC_ROBUST_HASH */

    val_bits = 0;
    for (index = 0; field_list[index] != INVALIDf; index++) {
        field = field_list[index];
        if (mem_view) {
            field_length[index] = soc_mem_field_length(unit, mem_view, field);
        } else {
            field_length[index] = soc_mem_field_length(unit, mem, field);
        }
        val_bits += field_length[index];
    }

    switch (mem) {
    case L2Xm:
        val_bits = SOC_TD3_L2_MAX_KEY_WIDTH;
        break;
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
        val_bits = SOC_TD3_L3_MAX_KEY_WIDTH;
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
        val_bits = SOC_TD3_EXACT_MATCH_MAX_KEY_WIDTH;
        break;
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
        val_bits = SOC_TD3_MPLS_MAX_KEY_WIDTH;
        break;
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:
        val_bits = SOC_TD3_VLAN_XLATE_MAX_KEY_WIDTH;
        break;
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
        val_bits = SOC_TD3_EGR_VLAN_XLATE_MAX_KEY_WIDTH;
        break;
    case ING_VP_VLAN_MEMBERSHIPm:
        val_bits = SOC_TD3_ING_VP_VLAN_MEMBERSHIP_MAX_KEY_WIDTH;
        break;
    case EGR_VP_VLAN_MEMBERSHIPm:
        val_bits = SOC_TD3_EGR_VP_VLAN_MEMBERSHIP_MAX_KEY_WIDTH;
        break;
    case ING_DNAT_ADDRESS_TYPEm:
        val_bits = SOC_TD3_ING_DNAT_ADDRESS_TYPE_MAX_KEY_WIDTH;
        break;
    case SUBPORT_ID_TO_SGPP_MAPm:
        val_bits = SOC_TD3_SUBPORT_ID_SGPP_MAP_MAX_KEY_WIDTH;
        break;
    default:
        break;
    }

    nbits = val_bits;

#ifdef SOC_ROBUST_HASH
    if (soc_feature(unit, soc_feature_robust_hash)) {
        switch (mem) {
        case L2Xm:
            rbh_cfg = &(ROBUSTHASH(unit)->l2);
            break;
        case L3_ENTRY_ONLY_SINGLEm:
        case L3_ENTRY_SINGLEm:
        case L3_ENTRY_DOUBLEm:
        case L3_ENTRY_QUADm:
            rbh_cfg = &(ROBUSTHASH(unit)->l3);
            break;
        case EXACT_MATCH_4m:
        case EXACT_MATCH_2m:
            rbh_cfg = &(ROBUSTHASH(unit)->exact_match);
            break;
        case VLAN_XLATE_1_SINGLEm:
        case VLAN_XLATE_1_DOUBLEm:
            rbh_cfg = &(ROBUSTHASH(unit)->ing_xlate_1);
            break;
        case VLAN_XLATE_2_SINGLEm:
        case VLAN_XLATE_2_DOUBLEm:
            rbh_cfg = &(ROBUSTHASH(unit)->ing_xlate_2);
            break;
        case MPLS_ENTRY_SINGLEm:
        case MPLS_ENTRYm:
            rbh_cfg = &(ROBUSTHASH(unit)->mpls_entry);
            break;
        case EGR_VLAN_XLATE_1_SINGLEm:
        case EGR_VLAN_XLATE_1_DOUBLEm:
            rbh_cfg = &(ROBUSTHASH(unit)->egr_xlate_1);
            break;
        case EGR_VLAN_XLATE_2_SINGLEm:
        case EGR_VLAN_XLATE_2_DOUBLEm:
            rbh_cfg = &(ROBUSTHASH(unit)->egr_xlate_2);
            break;
        case ING_VP_VLAN_MEMBERSHIPm:
            rbh_cfg = &(ROBUSTHASH(unit)->ing_vp_vlan_member);
            break;
        case EGR_VP_VLAN_MEMBERSHIPm:
            rbh_cfg = &(ROBUSTHASH(unit)->egr_vp_vlan_member);
            break;
        case ING_DNAT_ADDRESS_TYPEm:
            rbh_cfg = &(ROBUSTHASH(unit)->ing_dnat_address);
            break;
        case SUBPORT_ID_TO_SGPP_MAPm:
            rbh_cfg = &(ROBUSTHASH(unit)->subport_id_sgpp_map);
            break;
        default:
            break;
        }
    }

#endif /* SOC_ROBUST_HASH */
    bits = val_bits;
    val_bits = 0;

    sal_memset(val, 0, sizeof(val));
    for (index = 0; field_list[index] != INVALIDf; index++) {
        field = field_list[index];
        if (mem_view) {
            soc_mem_field_get(unit, mem_view, entry, field, fval);
        } else {
            soc_mem_field_get(unit, mem, entry, field, fval);
        }
        fval_bits = field_length[index];

        val_index = val_bits >> 5;
        fval_index = 0;
        left_shift_count = val_bits & 0x1f;
        right_shift_count = 32 - left_shift_count;
        val_bits += fval_bits;

        if (left_shift_count) {
            for (; fval_bits > 0; fval_bits -= 32) {
                val[val_index++] |= fval[fval_index] << left_shift_count;
                val[val_index] |= fval[fval_index++] >> right_shift_count;
            }
        } else {
            for (; fval_bits > 0; fval_bits -= 32) {
                val[val_index++] = fval[fval_index++];
            }
        }
    }

    key_index = 0;
    for (val_index = 0; val_bits > 0; val_index++) {
        for (right_shift_count = 0; right_shift_count < 32;
             right_shift_count += 8) {
            if (val_bits <= 0) {
                break;
            }
            key[key_index++] = (val[val_index] >> right_shift_count) & 0xff;
            val_bits -= 8;
        }
    }

    if ((bits + 7) / 8 > key_index) {
        sal_memset(&key[key_index], 0, (bits + 7) / 8 - key_index);
    }

    bits = (((nbits - 1) / 32) + 1) * 32;

#ifdef SOC_ROBUST_HASH
    if (soc_feature(unit, soc_feature_robust_hash)) {
        if (rbh_cfg) {
            if (rbh_cfg->enable) {
                (void) soc_robust_hash_get(unit, rbh_cfg, bank, key, nbits);
            }

        }
    }

#endif /* SOC_ROBUST_HASH */

    bits += ROBUST_HASH_KEY_SPACE_WIDTH;
    return bits;
}

int
soc_td3_hash_offset_get(int unit, soc_mem_t mem, int bank, int *hash_offset,
                        int *use_lsb)
{
    uint32 hash_control_entry[4];
    int is_shared = 1;
    soc_mem_t shared_mem = INVALIDm;
    soc_field_t field;    
    static const soc_field_t uft_fields[] = {
        HASH_OFFSET_DEDICATED_BANK_0f, HASH_OFFSET_DEDICATED_BANK_1f,
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf, B3_HASH_OFFSETf,
        B4_HASH_OFFSETf, B5_HASH_OFFSETf, B6_HASH_OFFSETf, B7_HASH_OFFSETf

    };
    static const soc_field_t uat_fields[] = {
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf, B3_HASH_OFFSETf
    };

    sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));

    switch(mem) {
    case L2Xm:
        /* Sanity Check */
        if (bank < 0 || bank > 9) {
            return SOC_E_PARAM;
        }
        mem = L2_ENTRY_HASH_CONTROLm;
        shared_mem = UFT_SHARED_BANKS_CONTROLm;
        is_shared = bank > 1;
        field = uft_fields[bank];
        break;
    case L3_ENTRY_ONLY_SINGLEm:
#ifdef BCM_TRIDENT3_SUPPORT
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
#endif /* BCM_TRIDENT3_SUPPORT */
        if (bank < 2 || bank > 11) {
            return SOC_E_PARAM;
        }
        /* Dedicated banks */
        if (bank >= 10 && bank <= 11) {
            bank = bank - 10;
            is_shared = 0;
        }
        mem = L3_ENTRY_HASH_CONTROLm;
        shared_mem = UFT_SHARED_BANKS_CONTROLm;
        field = uft_fields[bank];
        break;
    case EXACT_MATCH_2m:
        if (bank < 2 || bank > 9) {
            return SOC_E_PARAM;
        }
        mem = EXACT_MATCH_HASH_CONTROLm;
        shared_mem = UFT_SHARED_BANKS_CONTROLm;
        field = uft_fields[bank];
        break;
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
        if (bank < 0 || bank > 3) {
            return SOC_E_PARAM;
        }
        mem = EGR_VLAN_XLATE_1_HASH_CONTROLm;
        shared_mem = EP_UAT_SHARED_BANKS_CONTROLm;
        field = uat_fields[bank];
        break;
    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
        if (bank < 0 || bank > 3) {
            return SOC_E_PARAM;
        }
        mem = EGR_VLAN_XLATE_2_HASH_CONTROLm;
        shared_mem = EP_UAT_SHARED_BANKS_CONTROLm;
        field = uat_fields[bank];
        break;
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
        if (bank < 0 || bank > 7) {
            return SOC_E_PARAM;
        }
        mem = MPLS_ENTRY_HASH_CONTROLm;
        if (bank >= 4) { /* 16k size bank */
            bank = bank - 4;
            shared_mem = IP_UAT_16K_SHARED_BANKS_CONTROLm;
        } else { /* 8k size bank */
            shared_mem = IP_UAT_8K_SHARED_BANKS_CONTROLm;
        }
        field = uat_fields[bank];
        break;
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
        if (bank < 0 || bank > 7) {
            return SOC_E_PARAM;
        }
        mem = VLAN_XLATE_1_HASH_CONTROLm;
        if (bank >= 4) { /* 16k size bank */
            bank = bank - 4;
            shared_mem = IP_UAT_16K_SHARED_BANKS_CONTROLm;
        } else { /* 8k size bank */
            shared_mem = IP_UAT_8K_SHARED_BANKS_CONTROLm;
        }
        field = uat_fields[bank];
        break;
    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:
        if (bank < 0 || bank > 7) {
            return SOC_E_PARAM;
        }
        mem = VLAN_XLATE_2_HASH_CONTROLm;
        if (bank >= 4) { /* 16k size bank */
            bank = bank - 4;
            shared_mem = IP_UAT_16K_SHARED_BANKS_CONTROLm;
        } else { /* 8k size bank */
            shared_mem = IP_UAT_8K_SHARED_BANKS_CONTROLm;
        }
        field = uat_fields[bank];
        break;
    case ING_VP_VLAN_MEMBERSHIPm:
        if (bank < 0 || bank > 2) {
            return SOC_E_PARAM;
        }
        mem = ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLm;
        field = uft_fields[bank];
        is_shared = 0;
        break;
    case EGR_VP_VLAN_MEMBERSHIPm:
        if (bank < 0 || bank > 2) {
            return SOC_E_PARAM;
        }
        mem = EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLm;
        field = uft_fields[bank];
        is_shared = 0;  
        break;
    case ING_DNAT_ADDRESS_TYPEm:
        if (bank < 0 || bank > 2) {
            return SOC_E_PARAM;
        }
        mem = ING_DNAT_ADDRESS_TYPE_HASH_CONTROLm;
        field = uft_fields[bank];
        is_shared = 0; 
        break;
    case SUBPORT_ID_TO_SGPP_MAPm:
        if (bank < 0 || bank > 2) {
            return SOC_E_PARAM;
        }
        mem = SUBPORT_ID_TO_SGPP_MAP_HASH_CONTROLm;
        field = uft_fields[bank];
        is_shared = 0; 
        break;
    default:
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, 0,
                                     hash_control_entry));
    *use_lsb = soc_mem_field32_get(unit, mem, hash_control_entry,
                                   HASH_TABLE_TEST_MODEf);

    /* Shared Banks */
    if (is_shared > 0) {
        sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, shared_mem, MEM_BLOCK_ALL, 0,
                                         hash_control_entry));
        /* If Shared banks are used between ALPM and non-ALPM memories,
         * then ALPM uses Shared Bank B2, B3, B4, B5 and non-ALPM uses
         * B6, B7, B8, B9 banks. Maximum phy bank number for UFT is 9.
         */
        if (_soc_alpm_mode[unit] && ((mem == L2_ENTRY_HASH_CONTROLm) ||
                            (mem == L3_ENTRY_HASH_CONTROLm) ||
                            (mem == EXACT_MATCH_HASH_CONTROLm)) &&
                            (bank <=5)) {
            field = uft_fields[bank + 4];
        }
        *hash_offset = soc_mem_field32_get(unit, shared_mem,
                                           hash_control_entry, field);
    } else {
        *hash_offset = soc_mem_field32_get(unit, mem, hash_control_entry, field);
    }

    return SOC_E_NONE;

}

int
soc_td3_hash_offset_set(int unit, soc_mem_t mem, int bank, int hash_offset,
                        int use_lsb)
{
    uint32 hash_control_entry[4];
    int is_shared = 1;
    soc_mem_t shared_mem = INVALIDm;
    soc_field_t field;
    static const soc_field_t uft_fields[] = {
        HASH_OFFSET_DEDICATED_BANK_0f, HASH_OFFSET_DEDICATED_BANK_1f,
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf, B3_HASH_OFFSETf,
        B4_HASH_OFFSETf, B5_HASH_OFFSETf, B6_HASH_OFFSETf, B7_HASH_OFFSETf

    };
    static const soc_field_t uat_fields[] = {
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf, B3_HASH_OFFSETf
    };

    sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));

    switch(mem) {
    case L2Xm:
        /* Sanity Check */
        if (bank < 0 || bank > 9) {
            return SOC_E_PARAM;
        }
        mem = L2_ENTRY_HASH_CONTROLm;
        shared_mem = UFT_SHARED_BANKS_CONTROLm;
        is_shared = bank > 1;
        field = uft_fields[bank];
        break;
    case L3_ENTRY_ONLY_SINGLEm:
        if (bank < 2 || bank > 11) {
            return SOC_E_PARAM;
        }
        /* Dedicated banks */
        if (bank >= 10 && bank <= 11) {
            is_shared = 0;
        }
        mem = L3_ENTRY_HASH_CONTROLm;
        shared_mem = UFT_SHARED_BANKS_CONTROLm;
        field = uft_fields[(bank > 9)? (bank - 10) : bank];
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
        if (bank < 2 || bank > 9) {
            return SOC_E_PARAM;
        }
        mem = EXACT_MATCH_HASH_CONTROLm;
        shared_mem = UFT_SHARED_BANKS_CONTROLm;
        field = uft_fields[bank];
        break;
    case EGR_VLAN_XLATE_1_SINGLEm:
        if (bank < 0 || bank > 3) {
            return SOC_E_PARAM;
        }
        mem = EGR_VLAN_XLATE_1_HASH_CONTROLm;
        shared_mem = EP_UAT_SHARED_BANKS_CONTROLm;
        field = uat_fields[bank];
        break;
    case EGR_VLAN_XLATE_2_SINGLEm:
        if (bank < 0 || bank > 3) {
            return SOC_E_PARAM;
        }
        mem = EGR_VLAN_XLATE_2_HASH_CONTROLm;
        shared_mem = EP_UAT_SHARED_BANKS_CONTROLm;
        field = uat_fields[bank];
        break;
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
        if (bank < 0 || bank > 7) {
            return SOC_E_PARAM;
        }
        mem = MPLS_ENTRY_HASH_CONTROLm;
        if (bank >= 4) { /* 16k size bank */
            bank = bank - 4;
            shared_mem = IP_UAT_16K_SHARED_BANKS_CONTROLm;
        } else { /* 8k size bank */
            shared_mem = IP_UAT_8K_SHARED_BANKS_CONTROLm;
        }
        field = uat_fields[bank];
        break;
    case VLAN_XLATE_1_SINGLEm:
        if (bank < 0 || bank > 7) {
            return SOC_E_PARAM;
        }
        mem = VLAN_XLATE_1_HASH_CONTROLm;
        if (bank >= 4) { /* 16k size bank */
            bank = bank - 4;
            shared_mem = IP_UAT_16K_SHARED_BANKS_CONTROLm;
        } else { /* 8k size bank */
            shared_mem = IP_UAT_8K_SHARED_BANKS_CONTROLm;
        }
        field = uat_fields[bank];
        break;
    case VLAN_XLATE_2_SINGLEm:
        if (bank < 0 || bank > 7) {
            return SOC_E_PARAM;
        }
        mem = VLAN_XLATE_2_HASH_CONTROLm;
        if (bank >= 4) { /* 16k size bank */
            bank = bank - 4;
            shared_mem = IP_UAT_16K_SHARED_BANKS_CONTROLm;
        } else { /* 8k size bank */
            shared_mem = IP_UAT_8K_SHARED_BANKS_CONTROLm;
        }
        field = uat_fields[bank];
        break;
    case ING_VP_VLAN_MEMBERSHIPm:
        if (bank < 0 || bank > 2) {
            return SOC_E_PARAM;
        }
        mem = ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLm;
        field = uft_fields[bank];
        is_shared = 0;
        break;
    case EGR_VP_VLAN_MEMBERSHIPm:
        if (bank < 0 || bank > 2) {
            return SOC_E_PARAM;
        }
        mem = EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLm;
        field = uft_fields[bank];
        is_shared = 0;  
        break;
    case ING_DNAT_ADDRESS_TYPEm:
        if (bank < 0 || bank > 2) {
            return SOC_E_PARAM;
        }
        mem = ING_DNAT_ADDRESS_TYPE_HASH_CONTROLm;
        field = uft_fields[bank];
        is_shared = 0; 
        break;
    case SUBPORT_ID_TO_SGPP_MAPm:
        if (bank < 0 || bank > 2) {
            return SOC_E_PARAM;
        }
        mem = SUBPORT_ID_TO_SGPP_MAP_HASH_CONTROLm;
        field = uft_fields[bank];
        is_shared = 0; 
        break;
    default:
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, 0,
                                     hash_control_entry));
    soc_mem_field32_set(unit, mem, hash_control_entry, HASH_TABLE_TEST_MODEf,
                        use_lsb);
    if (is_shared == 0) {
        soc_mem_field32_set(unit, mem, hash_control_entry, field, hash_offset);
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0,
                                        hash_control_entry));

    if (is_shared > 0) {
        sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, shared_mem, MEM_BLOCK_ALL, 0,
                                            hash_control_entry));
        soc_mem_field32_set(unit, shared_mem, hash_control_entry, field,
                                hash_offset);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, shared_mem, MEM_BLOCK_ALL, 0,
                                            hash_control_entry));
    }
    return SOC_E_NONE;
}

STATIC int
_soc_td3_shared_hash(int unit, int hash_offset, uint32 key_nbits, uint8 *keyA,
                     uint8 *keyB, uint32 result_mask, uint16 lsb, int use_lsb)
{
   uint32 crc_hi, crc_lo;
   uint64 hash_vector, mask;

   if (use_lsb) {
       /* Debug Mode 
        * Hash Vector [63:48] ->  Zero
        * Hash Vector [47:32] ->  Hash LSB
        */
       crc_hi = lsb;
   } else {
       crc_hi = soc_crc32b(keyB, key_nbits);
   }

   crc_lo = soc_crc32b(keyA, key_nbits);

   COMPILER_64_SET(hash_vector, crc_hi, crc_lo);
   COMPILER_64_SET(mask, 0, result_mask);
   COMPILER_64_SHR(hash_vector, hash_offset);
   COMPILER_64_AND(hash_vector, mask);
   return (int)COMPILER_64_LO(hash_vector);
}

int
soc_td3_hash_index_get(int unit, int mem, int bank, int bucket)
{
    int rv;
    int index;
    int entries_per_row, bank_base, bucket_offset, entries_per_bucket;

    switch(mem) {
    case L2Xm:
        rv = soc_td3_hash_bank_info_get(unit, mem, bank, NULL, &entries_per_row,
                                        NULL, &bank_base, &bucket_offset);
        if (SOC_SUCCESS(rv)) {
            return bank_base + bucket * entries_per_row + bucket_offset;
        }
        break;
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
        rv = soc_td3_hash_bank_info_get(unit, mem, bank, NULL,
                                        &entries_per_row, NULL, &bank_base,
                                        &bucket_offset);
        if (SOC_SUCCESS(rv)) {
            index = bank_base + bucket * entries_per_row + bucket_offset;
            return index;
        }
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
        rv = soc_td3_hash_bank_info_get(unit, mem, bank, NULL,
                                        &entries_per_row, &entries_per_bucket,
                                        &bank_base, &bucket_offset);
        if (SOC_SUCCESS(rv)) {
            index = bank_base + bucket * entries_per_bucket + bucket_offset;
            return index;
        }
        break;
    case ING_VP_VLAN_MEMBERSHIPm:
    case EGR_VP_VLAN_MEMBERSHIPm:
    case ING_DNAT_ADDRESS_TYPEm:
    case SUBPORT_ID_TO_SGPP_MAPm:
        rv = soc_td3_hash_bank_info_get(unit, mem, bank, NULL,
                                    &entries_per_row, NULL, &bank_base,
                                    &bucket_offset);
        if (SOC_SUCCESS(rv)) {
            index = bank_base + bucket * entries_per_row + bucket_offset;
            return index;
        }
        break;
    default:
        break;
    }

    return 0;

}

#define SOC_FLEX_HASH_KEY_NUM 4
extern
int soc_flex_hash_mem_view_get(int unit, soc_mem_t mem,uint32 *entry,
                       soc_mem_t *mem_view, soc_field_t *field_list)
{
    int data_type = 0;
    int key_type = 0;
    int rv, i=0;
    soc_mem_t mem_view_id;
    soc_field_t key_fields[SOC_FLEX_HASH_KEY_NUM] =
        {KEY_0f, KEY_1f, KEY_2f, KEY_3f};


    if (SOC_MEM_FIELD_VALID(unit,mem,KEY_TYPEf)) {
        key_type = soc_mem_field32_get(unit, mem, entry, KEY_TYPEf);
    }
    if (SOC_MEM_FIELD_VALID(unit,mem,DATA_TYPEf)) {
        data_type = soc_mem_field32_get(unit, mem, entry, DATA_TYPEf);
    }

    /* find the memory view based on the memory and key type */
    rv = soc_flow_db_mem_to_view_id_get (unit, mem, key_type, data_type,
                    0, NULL, (uint32 *)&mem_view_id);

    SOC_IF_ERROR_RETURN(rv);

    *mem_view = mem_view_id;

    for (i = 0; i < SOC_FLEX_HASH_KEY_NUM; i++) {
        if (soc_mem_view_field_valid(unit, mem_view_id, key_fields[i])) {
            field_list[i] = key_fields[i];
        } else {
            break;
        }
    }
    field_list[i] = INVALIDf;
    return SOC_E_NONE;
}

uint32
soc_td3_l2x_hash(int unit, int bank, int hash_offset, int use_lsb,
                 int key_nbits, void *base_entry, uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    if (bank < 2) {
        hash_mask = 0x0FFF; /* 4k buckets per dedicated L2 bank */
        hash_bits = 12;
    } else {
        hash_mask = 0x1FFF; /* 8k buckets per shared bank */
        hash_bits = 13;
    }

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        switch (soc_mem_field32_get(unit, L2Xm, base_entry, KEY_TYPEf)) {
        case TD3_L2_HASH_KEY_TYPE_BRIDGE:
        case TD3_L2_HASH_KEY_TYPE_VFI:
        case TD3_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          L2__HASH_LSBf);
            break;
        case TD3_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
        case TD3_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          VLAN__HASH_LSBf);
            break;
        case TD3_L2_HASH_KEY_TYPE_VIF:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          VIF__HASH_LSBf);
            break;
        case TD3_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          TRILL_NONUC_NETWORK_LONG__HASH_LSBf);
            break;
        case TD3_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
            lsb_val = soc_mem_field32_get
                (unit, L2Xm, base_entry, TRILL_NONUC_NETWORK_SHORT__HASH_LSBf);
            break;
        case TD3_L2_HASH_KEY_TYPE_BFD:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          BFD__HASH_LSBf);
            break;
        case TD3_L2_HASH_KEY_TYPE_PE_VID:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          PE_VID__HASH_LSBf);
            break;
        case TD3_L2_HASH_KEY_TYPE_FCOE_ZONE:
            lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                          FCOE_ZONE__HASH_LSBf);
            break;
        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }

    return _soc_td3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_td3_l2x_base_entry_to_key(int unit, int bank, uint32 *entry, uint8 *key)
{
    soc_field_t field_list[5];
    soc_mem_t   flex_mem_view = 0;

    field_list[1] = INVALIDf;

    switch (soc_mem_field32_get(unit, L2Xm, entry, KEY_TYPEf)) {
    case TD3_L2_HASH_KEY_TYPE_BRIDGE:
    case TD3_L2_HASH_KEY_TYPE_VFI:
    case TD3_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
        field_list[0] = L2__KEYf;
        break;
    case TD3_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
    case TD3_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT:
        field_list[0] = VLAN__KEYf;
        break;
    case TD3_L2_HASH_KEY_TYPE_VIF:
        field_list[0] = VIF__KEYf;
        break;
    case TD3_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
        field_list[0] = TRILL_NONUC_NETWORK_LONG__KEYf;
        break;
    case TD3_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
        field_list[0] = TRILL_NONUC_NETWORK_SHORT__KEYf;
        break;
    case TD3_L2_HASH_KEY_TYPE_BFD:
        field_list[0] = BFD__KEYf;
        break;
    case TD3_L2_HASH_KEY_TYPE_PE_VID:
        field_list[0] = PE_VID__KEYf;
        break;
    case TD3_L2_HASH_KEY_TYPE_FCOE_ZONE:
        field_list[0] = FCOE_ZONE__KEYf;
        break;
    default:
        SOC_IF_ERROR_RETURN(soc_flex_hash_mem_view_get(unit,
                   L2Xm, entry, &flex_mem_view, field_list));
        break;
    }

    return _soc_td3_hash_entry_to_key(unit, bank, entry, key,
                            L2Xm, field_list, flex_mem_view);
}

uint32
soc_td3_l2x_entry_hash(int unit, int bank, int hash_offset, int use_lsb,
                      uint32 *entry) {
    int key_nbits;
    uint8 key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8 key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_td3_l2x_base_entry_to_key(unit, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_td3_l2x_base_entry_to_key(unit, 1, entry, key2);
    
    return soc_td3_l2x_hash(unit, bank, hash_offset, use_lsb, key_nbits, entry,
                            key1, key2);
}

uint32
soc_td3_l2x_bank_entry_hash(int unit, int bank, uint32 *entry) {
    int rv, hash_offset, use_lsb;

    rv = soc_td3_hash_offset_get(unit, L2Xm, bank, &hash_offset, &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }
    return soc_td3_l2x_entry_hash(unit, bank, hash_offset, use_lsb, entry);
}

uint32
soc_td3_l3x_hash(int unit, int bank, int hash_offset, int use_lsb,
                 int key_nbits, void *base_entry, uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    if (bank > 9) {
        hash_mask = 0x7FF; /* 2k buckets per dedicated banks */
        hash_bits = 11;
    } else {
        hash_mask = 0x1FFF; /* 8k buckets per shared bank */
        hash_bits = 13;
    }

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        switch (soc_mem_field32_get(unit, L3_ENTRY_ONLY_SINGLEm, base_entry,
                                    KEY_TYPEf)) {
        case TD3_L3_HASH_KEY_TYPE_V4UC:
        case TD3_L3_HASH_KEY_TYPE_V4MC:
        case TD3_L3_HASH_KEY_TYPE_V6UC:
        case TD3_L3_HASH_KEY_TYPE_V6MC:
            lsb_val = soc_mem_field32_get(unit, L3_ENTRY_SINGLEm,
                                          base_entry, IPV4UC__HASH_LSBf);
            break;
        case TD3_L3_HASH_KEY_TYPE_TRILL:
            lsb_val = soc_mem_field32_get(unit, L3_ENTRY_SINGLEm,
                                          base_entry, TRILL__HASH_LSBf);
            break;
        case TD3_L3_HASH_KEY_TYPE_DST_NAT:
        case TD3_L3_HASH_KEY_TYPE_DST_NAPT:
            lsb_val = soc_mem_field32_get(unit, L3_ENTRY_DOUBLEm,
                                          base_entry, NAT__HASH_LSBf);
            break;
        case TD3_L3_HASH_KEY_TYPE_FCOE_DOMAIN:
        case TD3_L3_HASH_KEY_TYPE_FCOE_HOST:
        case TD3_L3_HASH_KEY_TYPE_FCOE_SOURCE_MAP:
            lsb_val = soc_mem_field32_get(unit, L3_ENTRY_SINGLEm, base_entry,
                                          FCOE__HASH_LSBf);
            break;
        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }
    
    return _soc_td3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_td3_l3x_base_entry_to_key(int unit, int bank, uint32 *entry, uint8 *key)
{
    soc_mem_t mem;
    soc_field_t field_list[5];
    soc_mem_t   flex_mem_view = 0;
    
    switch (soc_mem_field32_get(unit, L3_ENTRY_ONLY_SINGLEm, entry, KEY_TYPEf)) {
    case TD3_L3_HASH_KEY_TYPE_V4UC:
        mem = L3_ENTRY_SINGLEm;
        field_list[0] = IPV4UC__KEYf;
        field_list[1] = INVALIDf;
        break;
    case TD3_L3_HASH_KEY_TYPE_V6UC:
        mem = L3_ENTRY_DOUBLEm;
        field_list[0] = IPV6UC__KEY_0f;
        field_list[1] = IPV6UC__KEY_1f;
        field_list[2] = INVALIDf;
        break;
    case TD3_L3_HASH_KEY_TYPE_V4MC:
    case TD3_L3_HASH_KEY_TYPE_L2_IPV4_MULTICAST:
    case TD3_L3_HASH_KEY_TYPE_L2VP_IPV4_MULTICAST:
        mem = L3_ENTRY_DOUBLEm;
        field_list[0] = IPV4MC__KEYf;
        field_list[1] = INVALIDf;
        break;
    case TD3_L3_HASH_KEY_TYPE_V6MC:
    case TD3_L3_HASH_KEY_TYPE_L2_IPV6_MULTICAST:
    case TD3_L3_HASH_KEY_TYPE_L2VP_IPV6_MULTICAST:
        mem = L3_ENTRY_QUADm;
        field_list[0] = IPV6MC__KEY_0f;
        field_list[1] = IPV6MC__KEY_1f;
        field_list[2] = IPV6MC__KEY_2f;
        field_list[3] = INVALIDf;
        break;
    case TD3_L3_HASH_KEY_TYPE_TRILL:
        mem = L3_ENTRY_SINGLEm;
        field_list[0] = TRILL__KEYf;
        field_list[1] = INVALIDf;
        break;
    case TD3_L3_HASH_KEY_TYPE_FCOE_DOMAIN:
    case TD3_L3_HASH_KEY_TYPE_FCOE_HOST:
    case TD3_L3_HASH_KEY_TYPE_FCOE_SOURCE_MAP:
        mem = L3_ENTRY_SINGLEm;
        field_list[0] = FCOE__KEYf;
        field_list[1] = INVALIDf;
        break;
    case TD3_L3_HASH_KEY_TYPE_DST_NAT:
    case TD3_L3_HASH_KEY_TYPE_DST_NAPT:
        mem = L3_ENTRY_DOUBLEm;
        field_list[0] = NAT__KEY_0f;
        field_list[1] = INVALIDf;
        break;

    default:
        mem = L3_ENTRY_DOUBLEm;
        SOC_IF_ERROR_RETURN(soc_flex_hash_mem_view_get(unit,
                   mem, entry, &flex_mem_view, field_list));
        break;
    }
    return _soc_td3_hash_entry_to_key(unit, bank, entry, key,
                             mem, field_list, flex_mem_view);
}

uint32
soc_td3_l3x_entry_hash(int unit, int bank, int hash_offset, int use_lsb,
                      uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_td3_l3x_base_entry_to_key(unit, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_td3_l3x_base_entry_to_key(unit, 1, entry, key2);

    return soc_td3_l3x_hash(unit, bank, hash_offset, use_lsb, key_nbits,
                            entry, key1, key2);
}

uint32
soc_td3_l3x_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_td3_hash_offset_get(unit, L3_ENTRY_ONLY_SINGLEm, bank, &hash_offset,
                                &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_td3_l3x_entry_hash(unit, bank, hash_offset, use_lsb, entry);
}

uint32
soc_td3_exact_match_hash(int unit, int bank, int hash_offset, int use_lsb,
                         int key_nbits, void *base_entry, uint8 *key1,
                         uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    hash_mask = 0xfff; /* 4k buckets per shared bank for EXACT_MATCH_2m */
    hash_bits = 12;

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        switch (soc_mem_field32_get(unit, EXACT_MATCH_2m, base_entry,
                                    KEY_TYPEf)) {
        case TD3_FPEM_HASH_KEY_TYPE_128B:
            lsb_val = soc_mem_field32_get(unit, EXACT_MATCH_2m, base_entry,
                                          MODE128__HASH_LSBf);
            break;
        case TD3_FPEM_HASH_KEY_TYPE_160B:
            lsb_val = soc_mem_field32_get(unit, EXACT_MATCH_2m, base_entry,
                                          MODE160__HASH_LSBf);
            break;
        case TD3_FPEM_HASH_KEY_TYPE_320B:
            lsb_val = soc_mem_field32_get(unit, EXACT_MATCH_4m, base_entry,
                                          MODE320__HASH_LSBf);
            break;
        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }

    return _soc_td3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_td3_exact_match_base_entry_to_key(int unit, int bank, uint32 *entry,
                                      uint8 *key) {
    soc_mem_t mem;
    soc_field_t field_list[5];
    soc_mem_t   flex_mem_view = 0;

    switch (soc_mem_field32_get(unit, EXACT_MATCH_2m, entry, KEY_TYPEf)) {
        case TD3_FPEM_HASH_KEY_TYPE_128B:
            mem = EXACT_MATCH_2m;
            field_list[0] = MODE128__KEY_0f;
            field_list[1] = MODE128__KEY_1f;
            field_list[2] = INVALIDf;
            break;
        case TD3_FPEM_HASH_KEY_TYPE_160B:
            mem = EXACT_MATCH_2m;
            field_list[0] = MODE160__KEY_0f;
            field_list[1] = MODE160__KEY_1f;
            field_list[2] = INVALIDf;
            break;
        case TD3_FPEM_HASH_KEY_TYPE_320B:
            mem = EXACT_MATCH_4m;
            field_list[0] = MODE320__KEY_0f;
            field_list[1] = MODE320__KEY_1f;
            field_list[2] = MODE320__KEY_2f;
            field_list[3] = MODE320__KEY_3f;
            field_list[4] = INVALIDf;
            break;
        default:
            mem = EXACT_MATCH_4m;
            SOC_IF_ERROR_RETURN(soc_flex_hash_mem_view_get(unit,
                   mem, entry, &flex_mem_view, field_list));
        break;
    }
    return _soc_td3_hash_entry_to_key(unit, bank, entry, key,
                             mem, field_list, flex_mem_view);
}

uint32
soc_td3_exact_match_entry_hash(int unit, int bank, int hash_offset,
                              int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_td3_exact_match_base_entry_to_key(unit, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_td3_exact_match_base_entry_to_key(unit, 1, entry, key2);

    return soc_td3_exact_match_hash(unit, bank, hash_offset, use_lsb,
                                    key_nbits, entry, key1, key2);
}

uint32
soc_td3_exact_match_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_td3_hash_offset_get(unit, EXACT_MATCH_2m, bank, &hash_offset,
                                 &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_td3_exact_match_entry_hash(unit, bank, hash_offset, use_lsb,
                                          entry);
}

uint32
soc_td3_mpls_hash(int unit, soc_mem_t mem, int bank, int hash_offset, int use_lsb,
                  int key_nbits, void *base_entry, uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    if (bank < 4) {
        hash_mask = 0x7FF; /* 2k buckets per shared bank of size 8k */
        hash_bits = 11;
    } else {
        hash_mask = 0xFFF; /* 4k buckets per shared bank of size 16k */
        hash_bits = 12;
    }

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        switch (soc_mem_field32_get(unit, mem, base_entry,
                                    KEY_TYPEf)) {
        case TD3_MPLS_HASH_KEY_TYPE_MPLS:
            lsb_val = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                          MPLS__HASH_LSBf);
            break;
        case TD3_MPLS_HASH_KEY_TYPE_MIM_NVP:
            lsb_val = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                          MIM_NVP__HASH_LSBf);
            break;
        case TD3_MPLS_HASH_KEY_TYPE_MIM_ISID:
        case TD3_MPLS_HASH_KEY_TYPE_MIM_ISID_SVP:
            lsb_val = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                          MIM_ISID__HASH_LSBf);
            break;
        case TD3_MPLS_HASH_KEY_TYPE_TRILL:
            lsb_val = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                          TRILL__HASH_LSBf);
            break;
        case TD3_MPLS_HASH_KEY_TYPE_L2GRE_SIP:
            lsb_val = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                          L2GRE_SIP__HASH_LSBf);
        break;
        case TD3_MPLS_HASH_KEY_TYPE_L2GRE_VPNID_SIP:
        case TD3_MPLS_HASH_KEY_TYPE_L2GRE_VPNID:
            lsb_val = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                          L2GRE_VPNID__HASH_LSBf);
            break;
        case TD3_MPLS_HASH_KEY_TYPE_VXLAN_SIP:
            lsb_val = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                          VXLAN_SIP__HASH_LSBf);
            break;
        case TD3_MPLS_HASH_KEY_TYPE_VXLAN_VPNID:
        case TD3_MPLS_HASH_KEY_TYPE_VXLAN_VPNID_SIP:
            lsb_val = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry,
                                          VXLAN_VN_ID__HASH_LSBf);
            break;

        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }

    return _soc_td3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_td3_mpls_base_entry_to_key(int unit, soc_mem_t mem, int bank, void *entry, uint8 *key) {
    
    soc_field_t field_list[5];
    soc_mem_t   flex_mem_view = 0;

    field_list[1] = INVALIDf;
    switch (soc_mem_field32_get(unit, mem, entry, KEY_TYPEf)) {
    case TD3_MPLS_HASH_KEY_TYPE_MPLS:
        field_list[0] = MPLS__KEYf;
        break;
    case TD3_MPLS_HASH_KEY_TYPE_MIM_NVP:
        field_list[0] = MIM_NVP__KEYf;
        break;
    case TD3_MPLS_HASH_KEY_TYPE_MIM_ISID:
    case TD3_MPLS_HASH_KEY_TYPE_MIM_ISID_SVP:
        field_list[0] = MIM_ISID__KEYf;
        break;
    case TD3_MPLS_HASH_KEY_TYPE_TRILL:
        field_list[0] = TRILL__KEYf;
        break;
    case TD3_MPLS_HASH_KEY_TYPE_L2GRE_SIP:
        field_list[0] = L2GRE_SIP__KEYf;
        break;
    case TD3_MPLS_HASH_KEY_TYPE_L2GRE_VPNID_SIP:
    case TD3_MPLS_HASH_KEY_TYPE_L2GRE_VPNID:
        field_list[0] = L2GRE_VPNID__KEYf;
        break;
    case TD3_MPLS_HASH_KEY_TYPE_VXLAN_SIP:
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode) &&
            (mem == MPLS_ENTRY_SINGLEm)) {
            field_list[0] = VXLAN_FLEX_IPV4_SIP__KEYf;
        }
        else {
            field_list[0] = VXLAN_SIP__KEYf;
        }
        break;
    case TD3_MPLS_HASH_KEY_TYPE_VXLAN_VPNID:
    case TD3_MPLS_HASH_KEY_TYPE_VXLAN_VPNID_SIP:
        field_list[0] = VXLAN_VN_ID__KEYf;
        break;
    case TD3_MPLS_HASH_KEY_TYPE_VXLAN_FLEX_IPV4_DIP:
        field_list[0] = VXLAN_FLEX_IPV4_DIP__KEYf;
        break;
    default:
        SOC_IF_ERROR_RETURN(soc_flex_hash_mem_view_get(unit,
            mem, entry, &flex_mem_view, field_list));
        break;
    }
    return _soc_td3_hash_entry_to_key(unit, bank, entry, key,
                      mem, field_list, flex_mem_view);
}

uint32
soc_td3_mpls_entry_hash(int unit, soc_mem_t mem, int bank,
                        int hash_offset, int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_td3_mpls_base_entry_to_key(unit, mem, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_td3_mpls_base_entry_to_key(unit, mem, 1, entry, key2);

    return soc_td3_mpls_hash(unit, mem, bank, hash_offset, use_lsb,
                             key_nbits, entry, key1, key2);
}

uint32
soc_td3_mpls_bank_entry_hash(int unit, soc_mem_t mem,
                             int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_td3_hash_offset_get(unit, mem, bank, &hash_offset,
                                 &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_td3_mpls_entry_hash(unit, mem, bank, hash_offset, use_lsb, entry);
}

uint32
soc_td3_vlan_xlate_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                        int use_lsb, int key_nbits, void *base_entry,
                        uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    if (bank < 4) {
        hash_mask = 0x7FF; /* 2k buckets per shared bank of size 8k */
        hash_bits = 11;
    } else {
        hash_mask = 0xFFF; /* 4k buckets per shared bank of size 16k */
        hash_bits = 12;
    }

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        switch (soc_mem_field32_get(unit, mem, base_entry,
                                    KEY_TYPEf)) {
        case TD3_VLXLT_HASH_KEY_TYPE_IVID_OVID:
        case TD3_VLXLT_HASH_KEY_TYPE_OTAG:
        case TD3_VLXLT_HASH_KEY_TYPE_ITAG:
        case TD3_VLXLT_HASH_KEY_TYPE_OVID:
        case TD3_VLXLT_HASH_KEY_TYPE_IVID:
        case TD3_VLXLT_HASH_KEY_TYPE_PRI_CFI:
        case TD3_VLXLT_HASH_KEY_TYPE_IVID_OVID_VSAN:
        case TD3_VLXLT_HASH_KEY_TYPE_IVID_VSAN:
        case TD3_VLXLT_HASH_KEY_TYPE_OVID_VSAN:
            lsb_val = soc_mem_field32_get(unit, mem, base_entry,
                                          XLATE__HASH_LSBf);
            break;
        case TD3_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
            lsb_val = soc_mem_field32_get(unit, mem, base_entry,
                                          MAC__HASH_LSBf);
            break;
        case TD3_VLXLT_HASH_KEY_TYPE_HPAE:
            lsb_val = soc_mem_field32_get(unit, mem, base_entry,
                                          MAC_IP_BIND__HASH_LSBf);
            break;
        case TD3_VLXLT_HASH_KEY_TYPE_VIF:
        case TD3_VLXLT_HASH_KEY_TYPE_VIF_VLAN:
        case TD3_VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
        case TD3_VLXLT_HASH_KEY_TYPE_VIF_OTAG:
        case TD3_VLXLT_HASH_KEY_TYPE_VIF_ITAG:
            lsb_val = soc_mem_field32_get(unit, mem, base_entry,
                                          VIF__HASH_LSBf);
            break;
        case TD3_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
            lsb_val = soc_mem_field32_get(unit, mem, base_entry,
                                          MAC_PORT__HASH_LSBf);
            break;
        case TD3_VLXLT_HASH_KEY_TYPE_L2GRE_DIP:
            lsb_val = soc_mem_field32_get(unit, mem, base_entry,
                                          L2GRE_DIP__HASH_LSBf);
            break;
        case TD3_VLXLT_HASH_KEY_TYPE_VXLAN_DIP:
            lsb_val = soc_mem_field32_get(unit, mem, base_entry,
                                          VXLAN_DIP__HASH_LSBf);
            break;
        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }

    return _soc_td3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_td3_vlan_xlate_base_entry_to_key(int unit, soc_mem_t mem, int bank,
                                     void *entry, uint8 *key)
{
    soc_field_t field_list[5];
    soc_mem_t   flex_mem_view = 0;

    field_list[1] = INVALIDf;
    switch (soc_mem_field32_get(unit, mem, entry, KEY_TYPEf)) {
    case TD3_VLXLT_HASH_KEY_TYPE_IVID_OVID:
    case TD3_VLXLT_HASH_KEY_TYPE_OTAG:
    case TD3_VLXLT_HASH_KEY_TYPE_ITAG:
    case TD3_VLXLT_HASH_KEY_TYPE_OVID:
    case TD3_VLXLT_HASH_KEY_TYPE_IVID:
    case TD3_VLXLT_HASH_KEY_TYPE_PRI_CFI:
    case TD3_VLXLT_HASH_KEY_TYPE_IVID_OVID_VSAN:
    case TD3_VLXLT_HASH_KEY_TYPE_OVID_VSAN:
    case TD3_VLXLT_HASH_KEY_TYPE_IVID_VSAN:
        field_list[0] = XLATE__KEY_0f;
        break;
    case TD3_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
        field_list[0] = MAC__KEY_0f;
        break;
    case TD3_VLXLT_HASH_KEY_TYPE_HPAE:
        field_list[0] = MAC_IP_BIND__KEY_0f;
        break;
    case TD3_VLXLT_HASH_KEY_TYPE_VIF:
    case TD3_VLXLT_HASH_KEY_TYPE_VIF_VLAN:
    case TD3_VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
    case TD3_VLXLT_HASH_KEY_TYPE_VIF_OTAG:
    case TD3_VLXLT_HASH_KEY_TYPE_VIF_ITAG:
        field_list[0] = VIF__KEY_0f;
        break;
    case TD3_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
        field_list[0] = MAC_PORT__KEY_0f;
        break;
    case TD3_VLXLT_HASH_KEY_TYPE_L2GRE_DIP:
        field_list[0] = L2GRE_DIP__KEY_0f;
        break;
    case TD3_VLXLT_HASH_KEY_TYPE_VXLAN_DIP:
        field_list[0] = VXLAN_DIP__KEY_0f;
        break;
    case TD3_VLXLT_HASH_KEY_TYPE_VXLAN_FLEX_VNID:
        field_list[0] = KEY_TYPEf;
        field_list[1] = VXLAN_FLEX__VNIDf;
        field_list[2] = INVALIDf;
        break;
    case TD3_VLXLT_HASH_KEY_TYPE_VXLAN_FLEX_SIP_VNID:
        field_list[0] = VXLAN_FLEX__KEY_0f;
        break;
    case TD3_VLXLT_HASH_KEY_TYPE_MIM_BMACSA_BVID_ISID:
        field_list[0] = MIM_BMACSA_BVID_ISID__KEY_0f;
        break;
    default:
        SOC_IF_ERROR_RETURN(soc_flex_hash_mem_view_get(unit,
                   mem, entry, &flex_mem_view, field_list));
        break;
    }
    return _soc_td3_hash_entry_to_key(unit, bank, entry, key,
                             mem, field_list, flex_mem_view);
}

uint32
soc_td3_vlan_xlate_entry_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                              int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_td3_vlan_xlate_base_entry_to_key(unit, mem, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_td3_vlan_xlate_base_entry_to_key(unit, mem, 1, entry, key2);

    return soc_td3_vlan_xlate_hash(unit, mem, bank, hash_offset, use_lsb,
                                   key_nbits, entry, key1, key2);
}

uint32
soc_td3_vlan_xlate_bank_entry_hash(int unit, soc_mem_t mem, int bank,
                                   uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_td3_hash_offset_get(unit, mem, bank, &hash_offset,
                                 &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_td3_vlan_xlate_entry_hash(unit, mem, bank, hash_offset, 
                                         use_lsb, entry);
}

uint32
soc_td3_egr_vlan_xlate_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                            int use_lsb, int key_nbits, void *base_entry,
                            uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    hash_mask = 0x7FF; /* 2k buckets per shared bank of size 8k */
    hash_bits = 11;

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        switch (soc_mem_field32_get(unit, mem, base_entry,
                                    KEY_TYPEf)) {
        case TD3_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE:
        case TD3_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_DVP:
        case TD3_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE:
        case TD3_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE_DVP:
            lsb_val = soc_mem_field32_get(unit, mem, base_entry,
                                          XLATE__HASH_LSBf);
            break;
        case TD3_EVLXLT_HASH_KEY_TYPE_ISID_XLATE:
        case TD3_EVLXLT_HASH_KEY_TYPE_ISID_DVP_XLATE:
            lsb_val = soc_mem_field32_get(unit, mem, base_entry,
                                          MIM_ISID__HASH_LSBf);
            break;
        case TD3_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI:
        case TD3_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI_DVP:
            lsb_val = soc_mem_field32_get(unit, mem, base_entry,
                                          L2GRE_VFI__HASH_LSBf);
            break;
        case TD3_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI:
        case TD3_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI_DVP:
            lsb_val = soc_mem_field32_get(unit, mem, base_entry,
                                          VXLAN_VFI__HASH_LSBf);
            break;
        default:
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }

    return _soc_td3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_td3_egr_vlan_xlate_base_entry_to_key(int unit, soc_mem_t mem, int bank,
                                         void *entry, uint8 *key)
{
    soc_field_t field_list[5];
    soc_mem_t   flex_mem_view = 0;

    field_list[1] = INVALIDf;
    switch (soc_mem_field32_get(unit, mem, entry, KEY_TYPEf)) {
    case TD3_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE:
    case TD3_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE:
    case TD3_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_DVP:
    case TD3_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE_DVP:
        field_list[0] = XLATE__KEY_0f;
        break;
    case TD3_EVLXLT_HASH_KEY_TYPE_ISID_XLATE:
    case TD3_EVLXLT_HASH_KEY_TYPE_ISID_DVP_XLATE:
        field_list[0] = MIM_ISID__KEY_0f;
        break;
    case TD3_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI:
    case TD3_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI_DVP:
        field_list[0] = L2GRE_VFI__KEY_0f;
        break;
    case TD3_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI:
    case TD3_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI_DVP:
         if (soc_feature(unit, soc_feature_vxlan_decoupled_mode) &&
            (mem == EGR_VLAN_XLATE_1_SINGLEm)) {
            field_list[0] = VXLAN_VFI_FLEX__KEY_0f;
         } else {
            field_list[0] = VXLAN_VFI__KEY_0f;
         }
        break;
    case TD3_ELVXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI:
        field_list[0] = VLAN_XLATE_VFI__KEY_0f;
        break;
    case TD3_ELVXLT_HASH_KEY_TYPE_VFI_DVP_GROUP:
        field_list[0] = VFI_DVP_GROUP__KEY_0f;
        break;
    case TD3_ELVXLT_HASH_KEY_TYPE_VRF_MAPPING:
        field_list[0] = VRF_MAPPING__KEY_0f;
        break;
    case TD3_ELVXLT_HASH_KEY_TYPE_ERSPAN3_VXLAN_MIRROR:
        field_list[0] = ERSPAN3_VXLAN_MIRROR__KEY_0f;
        break;
    case TD3_ELVXLT_HASH_KEY_TYPE_DVP_VFI_ASSIGN_ISID:
        field_list[0] = DVP_VFI_ASSIGN_ISID__KEY_0f;
        break;
    case TD3_ELVXLT_HASH_KEY_TYPE_CHANGE_L2_FIELDS:
        if (mem == EGR_VLAN_XLATE_1_DOUBLEm) {
            /* TD3_ELVXLT_1_HASH_KEY_TYPE_DGPP_OVID_IVID */
            field_list[0] = VXLAN_SUB_CHANNEL__KEY_0f;
        } else {
            field_list[0] = CHANGE_L2_FIELDS__KEY_0f;
        }
        break;
    case TD3_ELVXLT_HASH_KEY_TYPE_LS_NAT_L3UC_SNAT:
        field_list[0] = LS_NAT_L3UC_SNAT__KEY_0f;
        break;
    case TD3_ELVXLT_HASH_KEY_TYPE_LS_NAT_L3UC_DNAT:
      /* 
       * EGR_VLAN_XLATE_1_DOUBLE has the same value KEY_TYPE
       * for a different view. Check that the memory is EVXLT_2.
       */
        if (mem == EGR_VLAN_XLATE_2_DOUBLEm) {
            field_list[0] = LS_NAT_L3UC_DNAT__KEY_0f;
        }
        else {
            field_list[0] = XLATE__KEY_0f;
        }
        break;
    case TD3_ELVXLT_HASH_KEY_TYPE_LS_NAT_L3MC:
        field_list[0] = LS_NAT_L3MC__KEY_0f;
        break;
    default:
        SOC_IF_ERROR_RETURN(soc_flex_hash_mem_view_get(unit,
                   mem, entry, &flex_mem_view, field_list));
    }
    return _soc_td3_hash_entry_to_key(unit, bank, entry,
                   key, mem, field_list, flex_mem_view);
}

uint32
soc_td3_egr_vlan_xlate_entry_hash(int unit, soc_mem_t mem, int bank,
                                  int hash_offset, int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_td3_egr_vlan_xlate_base_entry_to_key(unit, mem, 0,
                                                            entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_td3_egr_vlan_xlate_base_entry_to_key(unit, mem, 1,
                                                            entry, key2);

    return soc_td3_egr_vlan_xlate_hash(unit, mem, bank, hash_offset, use_lsb,
                                       key_nbits, entry, key1, key2);
}

uint32
soc_td3_egr_vlan_xlate_bank_entry_hash(int unit, soc_mem_t mem, int bank,
                                       uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_td3_hash_offset_get(unit, mem, bank, &hash_offset,
                                 &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_td3_egr_vlan_xlate_entry_hash(unit, mem, bank, hash_offset, 
                                             use_lsb, entry);
}

uint32
soc_td3_ing_vp_vlan_member_hash(int unit, int bank, int hash_offset,
                                int use_lsb, int key_nbits, void *base_entry,
                                uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

        hash_mask = 0x7FF; /* 2k buckets per dedicated bank of size 8k */
        hash_bits = 11;

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        lsb_val = soc_mem_field32_get(unit, ING_VP_VLAN_MEMBERSHIPm,
                                      base_entry, HASH_LSBf);
    } else {
        lsb_val = 0;
    }

    return _soc_td3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_td3_ing_vp_vlan_member_base_entry_to_key(int unit, int bank, void *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEYf,
        INVALIDf
    };

    return _soc_td3_hash_entry_to_key(unit, bank, entry, key,
                                      ING_VP_VLAN_MEMBERSHIPm, field_list, 0);
}

uint32
soc_td3_ing_vp_vlan_member_entry_hash(int unit, int bank, int hash_offset,
                                      int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_td3_ing_vp_vlan_member_base_entry_to_key(unit, 0,
                                                             entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_td3_ing_vp_vlan_member_base_entry_to_key(unit, 1, 
                                                             entry, key2);

    return soc_td3_ing_vp_vlan_member_hash(unit, bank, hash_offset, use_lsb,
                                           key_nbits, entry, key1, key2);
}

uint32
soc_td3_ing_vp_vlan_member_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_td3_hash_offset_get(unit, ING_VP_VLAN_MEMBERSHIPm, bank,
                                 &hash_offset, &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_td3_ing_vp_vlan_member_entry_hash(unit, bank, hash_offset,
                                                 use_lsb, entry);
}

uint32
soc_td3_egr_vp_vlan_member_hash(int unit, int bank, int hash_offset,
                                int use_lsb, int key_nbits, void *base_entry,
                                uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

        hash_mask = 0x7FF; /* 2k buckets per dedicated bank of size 8k */
        hash_bits = 11;

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        lsb_val = soc_mem_field32_get(unit, EGR_VP_VLAN_MEMBERSHIPm,
                                      base_entry, HASH_LSBf);
    } else {
        lsb_val = 0;
    }

    return _soc_td3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_td3_egr_vp_vlan_member_base_entry_to_key(int unit, int bank, void *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEYf,
        INVALIDf
    };

    return _soc_td3_hash_entry_to_key(unit, bank, entry, key,
                                      EGR_VP_VLAN_MEMBERSHIPm, field_list, 0);
}

uint32
soc_td3_egr_vp_vlan_member_entry_hash(int unit, int bank, int hash_offset,
                                      int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_td3_egr_vp_vlan_member_base_entry_to_key(unit, 0,
                                                             entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_td3_egr_vp_vlan_member_base_entry_to_key(unit, 1, 
                                                             entry, key2);

    return soc_td3_egr_vp_vlan_member_hash(unit, bank, hash_offset, use_lsb,
                                           key_nbits, entry, key1, key2);
}

uint32
soc_td3_egr_vp_vlan_member_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_td3_hash_offset_get(unit, EGR_VP_VLAN_MEMBERSHIPm, bank,
                                 &hash_offset, &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_td3_egr_vp_vlan_member_entry_hash(unit, bank, hash_offset,
                                                 use_lsb, entry);
}

uint32
soc_td3_ing_dnat_address_type_hash(int unit, int bank, int hash_offset,
                                   int use_lsb, int key_nbits,
                                   void *base_entry, uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    hash_mask = 0xFF; /* 256 buckets per dedicated bank of size 1k */
    hash_bits = 8;

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        lsb_val = soc_mem_field32_get(unit, ING_DNAT_ADDRESS_TYPEm,
                                      base_entry, HASH_LSBf);
    } else {
        lsb_val = 0;
    }

    return _soc_td3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_td3_ing_dnat_address_type_base_entry_to_key(int unit, int bank, void *entry,
                                                uint8 *key)
{
    static soc_field_t field_list[] = {
        KEYf,
        INVALIDf
    };

    return _soc_td3_hash_entry_to_key(unit, bank, entry, key,
                                      ING_DNAT_ADDRESS_TYPEm, field_list, 0);
}

uint32
soc_td3_ing_dnat_address_type_entry_hash(int unit, int bank, int hash_offset,
                                         int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_td3_ing_dnat_address_type_base_entry_to_key(unit, 0,
                                                                entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_td3_ing_dnat_address_type_base_entry_to_key(unit, 1, 
                                                                entry, key2);

    return soc_td3_ing_dnat_address_type_hash(unit, bank, hash_offset,
                                              use_lsb, key_nbits, entry,
                                              key1, key2);
}

uint32
soc_td3_ing_dnat_address_type_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_td3_hash_offset_get(unit, ING_DNAT_ADDRESS_TYPEm, bank,
                                 &hash_offset, &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_td3_ing_dnat_address_type_entry_hash(unit, bank, hash_offset,
                                                    use_lsb, entry);
}

uint32
soc_td3_subport_id_to_sgpp_map_hash(int unit, int bank, int hash_offset,
                                    int use_lsb, int key_nbits,
                                    void *base_entry, uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

    hash_mask = 0x3FF; /* 1k buckets per dedicated bank of size 4k */
    hash_bits = 10;

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        lsb_val = soc_mem_field32_get(unit, SUBPORT_ID_TO_SGPP_MAPm,
                                      base_entry, HASH_LSBf);
    } else {
        lsb_val = 0;
    }

    return _soc_td3_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_td3_subport_id_to_sgpp_map_base_entry_to_key(int unit, int bank,
                                                 void *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEYf,
        INVALIDf
    };

    return _soc_td3_hash_entry_to_key(unit, bank, entry, key,
                                      SUBPORT_ID_TO_SGPP_MAPm, field_list, 0);
}

uint32
soc_td3_subport_id_to_sgpp_map_entry_hash(int unit, int bank, int hash_offset,
                                          int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_td3_subport_id_to_sgpp_map_base_entry_to_key(unit, 0,
                                                                 entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_td3_subport_id_to_sgpp_map_base_entry_to_key(unit, 1, 
                                                                 entry, key2);

    return soc_td3_subport_id_to_sgpp_map_hash(unit, bank, hash_offset,
                                                      use_lsb, key_nbits, entry,
                                                      key1, key2);
}

uint32
soc_td3_subport_id_to_sgpp_map_bank_entry_hash(int unit, int bank,
                                               uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_td3_hash_offset_get(unit, SUBPORT_ID_TO_SGPP_MAPm, bank,
                                 &hash_offset, &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_td3_subport_id_to_sgpp_map_entry_hash(unit, bank, hash_offset,
                                                     use_lsb, entry);
}

int
soc_td3_hash_bucket_get(int unit, int mem, int bank, uint32 *entry,
                        uint32 *bucket)
{
    switch (mem) {
    case L2Xm:
        *bucket = soc_td3_l2x_bank_entry_hash(unit, bank, entry);
        break;
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_SINGLEm:
        *bucket = soc_td3_l3x_bank_entry_hash(unit, bank, entry);
        break;
    case L3_ENTRY_DOUBLEm:
        *bucket = soc_td3_l3x_bank_entry_hash(unit, bank, entry);
        *bucket &= (bank > 9) ? 0x3FF : 0xFFF;
        break;
    case L3_ENTRY_QUADm:
        *bucket = soc_td3_l3x_bank_entry_hash(unit, bank, entry);
        *bucket &= (bank > 9) ? 0x1FF : 0x7FF;
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
        /* 16K entries => 4K buckets => 0x1000 buckets */ 
        *bucket = soc_td3_exact_match_bank_entry_hash(unit, bank, entry);
        break;
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
        /* 8K entries => 2K buckets => 0x800 buckets */ 
        *bucket = soc_td3_exact_match_bank_entry_hash(unit, bank, entry) & 0x7ff;
        break;
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
        *bucket = soc_td3_mpls_bank_entry_hash(unit, mem, bank, entry);
        if (bank < 4) {
            *bucket &= 0x3FF;
        } else {
            *bucket &= 0x7FF;
        }
        break;
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_2_SINGLEm:
        *bucket = soc_td3_vlan_xlate_bank_entry_hash(unit, mem, bank, entry);
        break;
    case VLAN_XLATE_1_DOUBLEm:
    case VLAN_XLATE_2_DOUBLEm:
        *bucket = soc_td3_vlan_xlate_bank_entry_hash(unit, mem, bank, entry);
        if (bank < 4) {
            *bucket &= 0x3FF;
        } else {
            *bucket &= 0x7FF;
        }
        break;
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_2_SINGLEm:
        *bucket = soc_td3_egr_vlan_xlate_bank_entry_hash(unit, mem,
                                                         bank, entry);
        break;
    case EGR_VLAN_XLATE_1_DOUBLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
        *bucket = soc_td3_egr_vlan_xlate_bank_entry_hash(unit, mem,
                                              bank, entry) & 0x3FF;
        break;
    case ING_VP_VLAN_MEMBERSHIPm:
        *bucket = soc_td3_ing_vp_vlan_member_bank_entry_hash(unit,
                                                             bank, entry);
        break;
    case EGR_VP_VLAN_MEMBERSHIPm:
        *bucket = soc_td3_egr_vp_vlan_member_bank_entry_hash(unit, 
                                                             bank, entry);
        break;
    case ING_DNAT_ADDRESS_TYPEm:
        *bucket = soc_td3_ing_dnat_address_type_bank_entry_hash(unit,
                                                                bank, entry);
        break;
    case SUBPORT_ID_TO_SGPP_MAPm:
        *bucket = soc_td3_subport_id_to_sgpp_map_bank_entry_hash(unit,
                                                                 bank, entry);
        break;
    default:
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}
#endif /* BCM_TRIDENT3_SUPPORT */
