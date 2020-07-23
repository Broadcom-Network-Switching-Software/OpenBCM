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
#include <soc/trident3.h>

#if defined(BCM_HELIX5_SUPPORT)

#include <soc/helix5.h>

int
soc_hx5_shared_hash_mem_bucket_read(int unit, int ent, int key_type, int *width,
                                    soc_mem_t base_mem, soc_mem_t *mem,
                                    void *bkt_entry, void *entry)
{

    soc_mem_info_t meminfo;
    soc_field_info_t fieldinfo;
    /* Exclude HIT bit */
    int entry_bits = 0;

    switch (base_mem) {
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
        switch (key_type) {
        case TD3_L3_HASH_KEY_TYPE_V4UC:
        case TD3_L3_HASH_KEY_TYPE_TRILL:
        case TD3_L3_HASH_KEY_TYPE_FCOE_DOMAIN:
        case TD3_L3_HASH_KEY_TYPE_FCOE_HOST:
        case TD3_L3_HASH_KEY_TYPE_FCOE_SOURCE_MAP:
            *width = 1;
            *mem = L3_ENTRY_SINGLEm;
            break;
        case TD3_L3_HASH_KEY_TYPE_V4MC:
        case TD3_L3_HASH_KEY_TYPE_L2_IPV4_MULTICAST:
        case TD3_L3_HASH_KEY_TYPE_L2VP_IPV4_MULTICAST:
        case TD3_L3_HASH_KEY_TYPE_DST_NAT:
        case TD3_L3_HASH_KEY_TYPE_DST_NAPT:
        case TD3_L3_HASH_KEY_TYPE_V6UC:
            *width = 2;
            *mem = L3_ENTRY_DOUBLEm;
            break;
        case TD3_L3_HASH_KEY_TYPE_V6MC:
        case TD3_L3_HASH_KEY_TYPE_L2_IPV6_MULTICAST:
        case TD3_L3_HASH_KEY_TYPE_L2VP_IPV6_MULTICAST:
        default:
            *width = 4;
            *mem = L3_ENTRY_QUADm;
        }
        entry_bits = (soc_mem_entry_bits(unit, L3_ENTRY_SINGLEm) - 1) * (*width);
        meminfo.bytes = SOC_MEM_WORDS(unit, L3_ENTRY_QUADm) * 4;
        break;

    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
        switch (key_type) {
        case TD3_FPEM_HASH_KEY_TYPE_128B:
        case TD3_FPEM_HASH_KEY_TYPE_160B:
            *width = 2;
            *mem = EXACT_MATCH_2m;
            break;
        case TD3_FPEM_HASH_KEY_TYPE_320B:
        default:
            *width = 4;
            *mem = EXACT_MATCH_4m;
        }
        entry_bits = (soc_mem_entry_bits(unit, EXACT_MATCH_2m) - 1) * ((*width)/2);
        meminfo.bytes = SOC_MEM_WORDS(unit, EXACT_MATCH_4m) * 4;
        break;

    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:
        switch (key_type) {
        case TD3_VLXLT_HASH_KEY_TYPE_L2GRE_DIP:
        case TD3_VLXLT_HASH_KEY_TYPE_VXLAN_DIP:
            *width = 1;
            *mem = VLAN_XLATE_1_SINGLEm;
            break;
        case TD3_VLXLT_HASH_KEY_TYPE_IVID:
        case TD3_VLXLT_HASH_KEY_TYPE_IVID_VSAN:
        case TD3_VLXLT_HASH_KEY_TYPE_OVID:
        case TD3_VLXLT_HASH_KEY_TYPE_OVID_VSAN:
        case TD3_VLXLT_HASH_KEY_TYPE_IVID_OVID:
        case TD3_VLXLT_HASH_KEY_TYPE_IVID_OVID_VSAN:
        case TD3_VLXLT_HASH_KEY_TYPE_OTAG:
        case TD3_VLXLT_HASH_KEY_TYPE_ITAG:
        case TD3_VLXLT_HASH_KEY_TYPE_VIF_OTAG:
        case TD3_VLXLT_HASH_KEY_TYPE_VIF_ITAG:
        case TD3_VLXLT_HASH_KEY_TYPE_VIF_VLAN:
        case TD3_VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
        case TD3_VLXLT_HASH_KEY_TYPE_VIF:
        case TD3_VLXLT_HASH_KEY_TYPE_HPAE:
        case TD3_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
        case TD3_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
        case TD3_VLXLT_HASH_KEY_TYPE_PRI_CFI:
        default:
            *width = 2;
            *mem = VLAN_XLATE_1_DOUBLEm;
        }
        entry_bits = (soc_mem_entry_bits(unit, VLAN_XLATE_1_SINGLEm) - 1)
                                                                * (*width);
        meminfo.bytes = SOC_MEM_WORDS(unit, VLAN_XLATE_1_DOUBLEm) * 4 * 2;

        break;

    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
        switch (key_type) {
        case TD3_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI:
        case TD3_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI_DVP:
        case TD3_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI:
        case TD3_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI_DVP:
            *width = 1;
            *mem = EGR_VLAN_XLATE_1_SINGLEm;
            break;
        case TD3_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE:
        case TD3_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_DVP:
        case TD3_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE:
        case TD3_EVLXLT_HASH_KEY_TYPE_ISID_XLATE:
        case TD3_EVLXLT_HASH_KEY_TYPE_ISID_DVP_XLATE:
        case TD3_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE_DVP:
        case TD3_ELVXLT_HASH_KEY_TYPE_VFI_DVP_GROUP:
        case TD3_ELVXLT_HASH_KEY_TYPE_VRF_MAPPING:
        case TD3_ELVXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI:
        default:
            *width = 2;
            *mem = EGR_VLAN_XLATE_1_DOUBLEm;
        }
        entry_bits = (soc_mem_entry_bits(unit, EGR_VLAN_XLATE_1_SINGLEm) - 1)
                                                                    * (*width);
        meminfo.bytes = SOC_MEM_WORDS(unit, EGR_VLAN_XLATE_1_DOUBLEm) * 4 * 2;
        break;

    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
        switch (key_type) {
        case TD3_MPLS_HASH_KEY_TYPE_MPLS:
        case TD3_MPLS_HASH_KEY_TYPE_MIM_NVP:
        case TD3_MPLS_HASH_KEY_TYPE_MIM_ISID:
        case TD3_MPLS_HASH_KEY_TYPE_MIM_ISID_SVP:
        case TD3_MPLS_HASH_KEY_TYPE_L2GRE_VPNID_SIP:
        case TD3_MPLS_HASH_KEY_TYPE_TRILL:
        case TD3_MPLS_HASH_KEY_TYPE_L2GRE_SIP:
        case TD3_MPLS_HASH_KEY_TYPE_L2GRE_VPNID:
        case TD3_MPLS_HASH_KEY_TYPE_VXLAN_SIP:
        case TD3_MPLS_HASH_KEY_TYPE_VXLAN_VPNID:
        case TD3_MPLS_HASH_KEY_TYPE_VXLAN_VPNID_SIP:
        default:
            *width = 2;
            *mem = MPLS_ENTRYm;
        }
        entry_bits = (soc_mem_entry_bits(unit, MPLS_ENTRY_SINGLEm) - 1)
                                                                    * (*width);
        meminfo.bytes = SOC_MEM_WORDS(unit, MPLS_ENTRYm) * 4 * 2;
        break;
    default:
        return SOC_E_PARAM;
    }

    meminfo.flags = 0;
    fieldinfo.flags = SOCF_LE;
    fieldinfo.bp =  entry_bits * (ent / *width);
    fieldinfo.len = entry_bits;

    (void)soc_meminfo_fieldinfo_field_get(bkt_entry, &meminfo,
                                          &fieldinfo, entry);
    return SOC_E_NONE;

}


int
soc_hx5_hash_mem_status_get(int unit, soc_mem_t mem, void* entry, uint32 *v) {
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

/* Get number of banks for the hash table according to the config
 */
int
soc_helix5_hash_bank_count_get(int unit, soc_mem_t mem, int *num_banks)
{
    int count;
    int ip_uft_bank_mul = 1; /* multiplier for ip uft. */
    int uat_base_entries = 512; /* Base entries in uat bank. */

    if (SOC_IS_HURRICANE4(unit)) {
        ip_uft_bank_mul = 0;
    } else if (SOC_IS_HELIX5(unit)) {
        uat_base_entries = 4 * 1024;
    } else {
        ip_uft_bank_mul = 2;
        uat_base_entries = 8 * 1024;
    }

    switch (mem) {
    case L2Xm:
        /*
         * 8k single wide entries in each of 2 dedicated L2 bank
         * 16k single wide entries in each shared bank (6 shared banks max)
         */
        count = soc_mem_index_count(unit, mem);

        /* Dedicated banks for all devices are for 2. */
        *num_banks = 2;
        if (ip_uft_bank_mul) {
            *num_banks += ((count - (ip_uft_bank_mul * 2 * 8 * 1024)) /
                                        (ip_uft_bank_mul* 2 * 8 * 1024));
        }
        break;
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_ONLY_DOUBLEm:
    case L3_ENTRY_ONLY_QUADm:
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
        /*
         * 8k single wide entries in each of 2 dedicated L3 bank
         * 16k entries in each shared bank (6 shared banks max)
         */
        count = soc_mem_index_count(unit, L3_ENTRY_ONLY_SINGLEm);
        /* Dedicated banks for all devices are 2. */
        *num_banks = 2;

        if (ip_uft_bank_mul) {
            *num_banks += ((count - (ip_uft_bank_mul * 2 * 8 * 1024)) /
                                        (ip_uft_bank_mul* 2 * 8 * 1024));
        }
        break;

    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_ENTRY_ONLYm:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_ENTRY_ONLYm:
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            return SOC_E_INTERNAL;
        }
#endif
        /*
         * HX5 : 8k double wide entries in each shared bank.
         * FB6 : 16K double wide entries in each shared bank.
         */
        count = soc_mem_index_count(unit, EXACT_MATCH_2m);
        *num_banks = count / (ip_uft_bank_mul * 8 * 1024);
        break;

    /* EUAT
     * HX5: 4k single wide entries in each shared bank
     * HR4: 512 single wide entries in each shared bank
     * FB6: 8k single wide entries in each shared bank
     */
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
        count = soc_mem_index_count(unit, EGR_VLAN_XLATE_1_SINGLEm);
        *num_banks = count / uat_base_entries;
        break;

    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
        count = soc_mem_index_count(unit, EGR_VLAN_XLATE_2_SINGLEm);
        *num_banks = count / uat_base_entries;
        break;

    case ING_DNAT_ADDRESS_TYPEm:
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            return SOC_E_INTERNAL;
        }
#endif
        /* coverity[fallthrough: FALSE] */
    case ING_VP_VLAN_MEMBERSHIPm:
    case EGR_VP_VLAN_MEMBERSHIPm:
        *num_banks = 2;
        break;

#ifdef BCM_HURRICANE4_SUPPORT
    case SUBPORT_ID_TO_SGPP_MAPm:
        if (SOC_IS_HELIX5(unit)) {
            return SOC_E_INTERNAL;
        }
        *num_banks = 2;
        break;
#endif

    /* IUAT
     * HX5: 4k single wide entries in each shared bank
     * HR4: 512 single wide entries in each shared bank
     */
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
        count = soc_mem_index_count(unit, MPLS_ENTRY_SINGLEm);
        *num_banks = count / uat_base_entries;
        break;

    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
        count = soc_mem_index_count(unit, VLAN_XLATE_1_SINGLEm);
        *num_banks = count / uat_base_entries;
        break;

    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:
        count = soc_mem_index_count(unit, VLAN_XLATE_2_SINGLEm);
        *num_banks = count / uat_base_entries;
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
soc_hx5_log_to_phy_bank_map(int unit, soc_mem_t mem, int log_bank,
                            int *phy_bank_id)
{
    uint32 hash_control_entry[4];
    int done = 0;
    int l3_phy_ded_start_id=8;
    int max_uft_shared_banks_id=7;
    static const soc_field_t logical_to_physical_fields[] = {
        LOGICAL_BANK_0_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_4_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_5_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_6_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_7_PHYSICAL_BANK_LOCATIONf
#if defined(BCM_FIREBOLT6_SUPPORT)
        ,LOGICAL_BANK_8_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_9_PHYSICAL_BANK_LOCATIONf
#endif

    };
    int logical_to_physical_fields_num = sizeof(logical_to_physical_fields)/
                                         sizeof(soc_field_t);

    if(log_bank < 0 || log_bank >= logical_to_physical_fields_num) {
        return SOC_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_uft_shared_8_banks_32k)) {
        l3_phy_ded_start_id = 10;
        max_uft_shared_banks_id = 9;
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
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            if (log_bank < 2) {
                *phy_bank_id = log_bank;
                done = 1;
            } else { /* Shared Banks */
                return SOC_E_INTERNAL;
            }
        } else
#endif
        /* Dedicated Banks */
        if (log_bank < 2) {
            *phy_bank_id = log_bank + l3_phy_ded_start_id;
            done = 1;
        } else { /* Shared Banks */
            mem = L3_ENTRY_HASH_CONTROLm;
        }
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            return SOC_E_INTERNAL;
        }
#endif
        /* max_uft_shared_banks_id includes 2 banks from l2. reduce them. */
        if (log_bank < 0 || log_bank > max_uft_shared_banks_id-2) {
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
soc_hx5_phy_to_log_bank_map(int unit, soc_mem_t mem, int phy_bank,
                            int *log_bank)
{
    uint32 hash_control_entry[4];
    int i, done = 0;
    int bank_min, bank_max;
    int l3_phy_ded_start_id=8;
    int max_uft_shared_banks_id=7;
    static const soc_field_t logical_to_physical_fields[] = {
        LOGICAL_BANK_0_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_4_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_5_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_6_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_7_PHYSICAL_BANK_LOCATIONf
#if defined(BCM_FIREBOLT6_SUPPORT)
        ,LOGICAL_BANK_8_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_9_PHYSICAL_BANK_LOCATIONf
#endif

    };
    if (soc_feature(unit, soc_feature_uft_shared_8_banks_32k)) {
        l3_phy_ded_start_id = 10;
        max_uft_shared_banks_id = 9;
    }

    sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));

    switch(mem) {
    case L2Xm:
        /* Shared bank start. */
        bank_min = 2;
        /* Max shared logical index plus dedicated banks.*/
        bank_max = max_uft_shared_banks_id;
        /* Dedicated Banks */
        if (phy_bank < 2) {
            *log_bank = phy_bank;
            done = 1;
        } else { /* Shared Banks */
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                return SOC_E_INTERNAL;
            }
#endif
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
        /* shared banks ID + dedicated banks which happens to be 2. */
        bank_max = max_uft_shared_banks_id+2;
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            if (phy_bank < 2) {
                *log_bank = phy_bank;
                done = 1;
            } else { /* Shared Banks */
                return SOC_E_INTERNAL;
            }
        } else
#endif
        /* Dedicated Banks */
        if (phy_bank > max_uft_shared_banks_id) {
            *log_bank = phy_bank - l3_phy_ded_start_id;
            done = 1;
        } else { /* Shared Banks */
            mem = L3_ENTRY_HASH_CONTROLm;
        }
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            return SOC_E_INTERNAL;
        }
#endif
        bank_min = 0;
        bank_max = max_uft_shared_banks_id - 2;
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
        /* Sanity */
        if (phy_bank < bank_min || phy_bank > bank_max) {
            return SOC_E_PARAM;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, 0,
                                         hash_control_entry));
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
soc_hx5_hash_bank_phy_bitmap_get(int unit, soc_mem_t mem, uint32 *bitmap)
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
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            return SOC_E_INTERNAL;
        }
#endif
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
soc_hx5_hash_bank_number_get(int unit, soc_mem_t mem, int seq_num,
                             int *bank_num) {
    int count;

    SOC_IF_ERROR_RETURN(soc_helix5_hash_bank_count_get(unit, mem, &count));
    if (seq_num < 0 || seq_num >= count) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(soc_hx5_log_to_phy_bank_map(unit, mem, seq_num,
                                                        bank_num));

    return SOC_E_NONE;
}

int
soc_hx5_hash_seq_number_get(int unit, soc_mem_t mem, int bank_num, int *seq_num)
{
    int count;

    SOC_IF_ERROR_RETURN(soc_hx5_phy_to_log_bank_map(unit, mem, bank_num,
                                                    seq_num));

    SOC_IF_ERROR_RETURN(soc_helix5_hash_bank_count_get(unit, mem, &count));
    if (*seq_num < 0 || *seq_num >= count) {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}


int
soc_hx5_hash_bank_info_get(int unit, soc_mem_t mem, int bank,
                           int *entries_per_bank, int *entries_per_row,
                           int *entries_per_bucket, int *bank_base,
                           int *bucket_offset)
{
    int bank_size, row_size, bucket_size, base, offset;
    int seq_num = 0;
    int l3_bank_size;
    int bank_ent_mul_factor = 1;
    int max_uft_shared_banks_id = 7;

    /* int uat_8k_banks = 0, uat_16k_banks = 0; */

    /* Get logical bank number from input physical bank number */
    SOC_IF_ERROR_RETURN(soc_hx5_phy_to_log_bank_map(unit, mem, bank, &seq_num));
    bank = seq_num;

    if (soc_feature(unit, soc_feature_uft_shared_8_banks_32k)) {
        max_uft_shared_banks_id = 9;
        bank_ent_mul_factor = 2;
    }


    switch(mem) {
    case L2Xm:
        /*
         * 4 entries per bucket (1 bucket per row)
         * bank 0 - 1 -> dedicated banks - 8k entries each
         * bank 2 - 7 -> shared banks - 16k entries each
         */
        row_size = 4;
        bucket_size = 4;
        offset = 0;
        if (bank < 0 || bank > max_uft_shared_banks_id) {
            return SOC_E_INTERNAL;
        } else if (bank < 2) { /* Dedicated bank */
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                /* bank 0 - 1 -> dedicated banks - 32k entries each */
                bank_size = 32 * 1024;
            } else
#endif
            {
                bank_size = bank_ent_mul_factor * 8192;
            }
            base = bank * bank_size;
        } else { /* Shared bank */
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                return SOC_E_INTERNAL;
            }
#endif
            /* Dedicated banks. */
            bank_size = bank_ent_mul_factor * 16384;
            /* Dedicated banks + (shared_logical_bank * shared bank size)*/
            base = bank_size + (bank - 2) * bank_size;
        }
        break;
    case L3_ENTRY_ONLY_SINGLEm:
#ifdef BCM_HELIX5_SUPPORT
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
#endif /* BCM_HELIX5_SUPPORT */
        /*
         * 4 entries per bucket (1 bucket per row)
         * bank 0 - 1 -> dedicated banks - 8k entries each
         * bank 2 - 7 -> shared banks - 16k entries each
         */
        row_size = 4;
        bucket_size = 4;
        offset = 0;
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            /* bank 0 - 1 -> dedicated banks - 16k entries each */
            l3_bank_size = 16 * 1024;
        } else
#endif
        {
            l3_bank_size = bank_ent_mul_factor * 8192;
        }
        if (bank < 0 || bank > max_uft_shared_banks_id) {
            return SOC_E_INTERNAL;
        } else if (bank < 2) { /* Dedicated bank */
            if (mem == L3_ENTRY_QUADm) {
                bank_size = l3_bank_size/4;
                base = bank * bank_size;
                row_size = 4;
            } else if (mem == L3_ENTRY_DOUBLEm) {
                bank_size = l3_bank_size/2;
                base = bank * bank_size;
                row_size = 4;
            } else {
                bank_size = l3_bank_size;
                base = bank * bank_size;
            }
        } else { /* Shared bank */
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                return SOC_E_INTERNAL;
            }
#endif
            bank_size = bank_ent_mul_factor * 16384;
            if (mem == L3_ENTRY_QUADm) {
                base = bank_size/4;
                bank_size = bank_size/4;
                row_size = 1;
            } else if (mem == L3_ENTRY_DOUBLEm) {
                base = bank_size/2;
                bank_size = bank_size/2;
                row_size = 2;
            } else {
                base = bank_size;
            }
            base += ((bank-2)*bank_size);
        }
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            return SOC_E_INTERNAL;
        }
#endif
        /* 2 entries per bucket
         * bank 0 - 5 -> shared banks - 8k entries each
         */
        row_size = 2;
        bucket_size = 2;
        offset = 0;
        bank_size = bank_ent_mul_factor*8192;
        base = bank * bank_size;
        break;
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            return SOC_E_INTERNAL;
        }
#endif
        /* 1 entries per bucket
         * bank 0 - 5 -> shared banks - 4k entries each
         */
        row_size = 1;
        bucket_size = 1;
        offset = 0;
        bank_size = bank_ent_mul_factor * 4096;
        base = bank * bank_size;
        break;
    case EGR_VLAN_XLATE_1_DOUBLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_2_SINGLEm:
        /* 2 entries per bucket
         * bank 0 - 3 -> shared banks - 4k entries each
         */
        row_size = ((mem == EGR_VLAN_XLATE_1_DOUBLEm) ||
                    (mem == EGR_VLAN_XLATE_2_DOUBLEm)) ? 2 : 4;
        offset = 0;
        bucket_size = 4;
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            bank_size = 512;
        } else
#endif
        {
            bank_size = bank_ent_mul_factor * 4096;
        }
        base = bank * bank_size;
        if ((mem == EGR_VLAN_XLATE_1_DOUBLEm) ||
            (mem == EGR_VLAN_XLATE_2_DOUBLEm)) {
            base /= 2;
        }
        break;
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:

        row_size = ((mem == VLAN_XLATE_1_DOUBLEm) ||
                    (mem == VLAN_XLATE_2_DOUBLEm) ||
                    (mem == MPLS_ENTRYm)) ? 2 : 4;

        bucket_size = 4;
        offset = 0;
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            bank_size = 512;
        } else
#endif
        {
            bank_size = bank_ent_mul_factor * 4096;
        }
        base = bank * bank_size;

        if ((mem == MPLS_ENTRYm) ||
            (mem == VLAN_XLATE_1_DOUBLEm) ||
            (mem == VLAN_XLATE_2_DOUBLEm)) {
            base /= 2;
        }

        break;

    case ING_DNAT_ADDRESS_TYPEm:
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            return SOC_E_INTERNAL;
        }
#endif
        /* coverity[fallthrough: FALSE] */
    case ING_VP_VLAN_MEMBERSHIPm:
    case EGR_VP_VLAN_MEMBERSHIPm:
        /* 
         * First half of entries in each row are for bank 0
         * Second half of entries are for bank 1
         */
        bank_size = soc_mem_index_count(unit, mem) / 2;
        bank_size = soc_mem_index_count(unit, mem) / 2;
        row_size = 4;
        bucket_size = 4;
        base = 0;
        offset = bank * bucket_size;
        break;
#ifdef BCM_HURRICANE4_SUPPORT
    case SUBPORT_ID_TO_SGPP_MAPm:
        if (SOC_IS_HELIX5(unit)) {
            return SOC_E_INTERNAL;
        }
        /* 
         * First half of entries in each row are for bank 0
         * Second half of entries are for bank 1
         */
        bank_size = soc_mem_index_count(unit, mem) / 2;
        bank_size = soc_mem_index_count(unit, mem) / 2;
        row_size = 4;
        bucket_size = 4;
        base = 0;
        offset = bank * bucket_size;
        break;
#endif
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
_soc_hx5_hash_entry_to_key(int unit, int bank, void *entry, uint8 *key,
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
        field_length[index] = soc_mem_field_length(unit, mem, field);
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
        soc_mem_field_get(unit, mem, entry, field, fval);
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

            bits += ROBUST_HASH_KEY_SPACE_WIDTH;
        }
    }

#endif /* SOC_ROBUST_HASH */

    return bits;
}

int
soc_hx5_hash_offset_get(int unit, soc_mem_t mem, int bank, int *hash_offset,
                        int *use_lsb)
{
    uint32 hash_control_entry[4];
    int is_shared = 1;
    soc_mem_t shared_mem = INVALIDm;
    soc_field_t field;
    int l3_phy_ded_start_id = 8;
    int max_uft_shared_banks_id = 7;

    static const soc_field_t uft_fields[] = {
        HASH_OFFSET_DEDICATED_BANK_0f, HASH_OFFSET_DEDICATED_BANK_1f,
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf, B3_HASH_OFFSETf,
        B4_HASH_OFFSETf, B5_HASH_OFFSETf
#if defined(BCM_FIREBOLT6_SUPPORT)
        , B6_HASH_OFFSETf, B7_HASH_OFFSETf
#endif

    };
    static const soc_field_t uat_fields[] = {
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf, B3_HASH_OFFSETf,
        B4_HASH_OFFSETf, B5_HASH_OFFSETf, B6_HASH_OFFSETf, B7_HASH_OFFSETf,
    };

    if (soc_feature(unit, soc_feature_uft_shared_8_banks_32k)) {
        l3_phy_ded_start_id = 10;
        max_uft_shared_banks_id = 9;
    }



    sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));

    switch(mem) {
    case L2Xm:
        /* Sanity Check */
        if (bank < 0 || bank > max_uft_shared_banks_id) {
            return SOC_E_PARAM;
        }
        mem = L2_ENTRY_HASH_CONTROLm;
        shared_mem = UFT_SHARED_BANKS_CONTROLm;
        is_shared = bank > 1;
        field = uft_fields[bank];
        break;
    case L3_ENTRY_ONLY_SINGLEm:
#ifdef BCM_HELIX5_SUPPORT
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            if (bank >= 2) {
                /* No Shared Banks */
                return SOC_E_INTERNAL;
            }
            is_shared = 0;
        } else
#endif
        {
           /* L3: 2 dedicated banks. so max is ded_start+1 */
            if (bank < 2 || bank > (l3_phy_ded_start_id+1)) {
                return SOC_E_PARAM;
            }
            /* Dedicated banks */
            if ((bank >= l3_phy_ded_start_id) && (bank <= l3_phy_ded_start_id+1)) {
                bank = bank - l3_phy_ded_start_id;
                is_shared = 0;
            }
        }
        mem = L3_ENTRY_HASH_CONTROLm;
        shared_mem = UFT_SHARED_BANKS_CONTROLm;
        field = uft_fields[bank];
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            return SOC_E_INTERNAL;
        }
#endif
        if (bank < 2 || bank > max_uft_shared_banks_id) {
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
        shared_mem = IP_UAT_SHARED_BANKS_CONTROLm;
        field = uat_fields[bank];
        break;
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
        if (bank < 0 || bank > 7) {
            return SOC_E_PARAM;
        }
        mem = VLAN_XLATE_1_HASH_CONTROLm;
        shared_mem = IP_UAT_SHARED_BANKS_CONTROLm;
        field = uat_fields[bank];
        break;
    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_2_DOUBLEm:
        if (bank < 0 || bank > 7) {
            return SOC_E_PARAM;
        }
        mem = VLAN_XLATE_2_HASH_CONTROLm;
        shared_mem = IP_UAT_SHARED_BANKS_CONTROLm;
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
#ifdef BCM_HURRICANE4_SUPPORT
    case SUBPORT_ID_TO_SGPP_MAPm:
        if (SOC_IS_HELIX5(unit)) {
            return SOC_E_INTERNAL;
        }
        if (bank < 0 || bank > 2) {
            return SOC_E_PARAM;
        }
        mem = SUBPORT_ID_TO_SGPP_MAP_HASH_CONTROLm;
        field = uft_fields[bank];
        is_shared = 0;
        break;
#endif
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
        *hash_offset = soc_mem_field32_get(unit, shared_mem,
                                           hash_control_entry, field);
    } else {
        *hash_offset = soc_mem_field32_get(unit, mem, hash_control_entry, field);
    }

    return SOC_E_NONE;

}

int
soc_hx5_hash_offset_set(int unit, soc_mem_t mem, int bank, int hash_offset,
                        int use_lsb)
{
    uint32 hash_control_entry[4];
    int is_shared = 1;
    soc_mem_t shared_mem = INVALIDm;
    soc_field_t field;
    int fid = 0;
    int l3_phy_ded_start_id = 8;
    int max_uft_shared_banks_id = 7;

    static const soc_field_t uft_fields[] = {
        HASH_OFFSET_DEDICATED_BANK_0f, HASH_OFFSET_DEDICATED_BANK_1f,
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf, B3_HASH_OFFSETf,
        B4_HASH_OFFSETf, B5_HASH_OFFSETf
#if defined(BCM_FIREBOLT6_SUPPORT)
        , B6_HASH_OFFSETf, B7_HASH_OFFSETf
#endif

    };
    static const soc_field_t uat_fields[] = {
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf, B3_HASH_OFFSETf
    };

    if (soc_feature(unit, soc_feature_uft_shared_8_banks_32k)) {
        l3_phy_ded_start_id = 10;
        max_uft_shared_banks_id = 9;
    }


    sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));

    switch(mem) {
    case L2Xm:
        /* Sanity Check */
        if (bank < 0 || bank > max_uft_shared_banks_id) {
            return SOC_E_PARAM;
        }
        mem = L2_ENTRY_HASH_CONTROLm;
        shared_mem = UFT_SHARED_BANKS_CONTROLm;
        is_shared = bank > 1;
        field = uft_fields[bank];
        break;
    case L3_ENTRY_ONLY_SINGLEm:
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            if (bank >= 2) {
                /* No Shared Banks */
                return SOC_E_INTERNAL;
            }
            is_shared = 0;
        } else
#endif
        {
           /* L3: 2 dedicated banks. so max is ded_start+1 */
            if (bank < 2 || bank > (l3_phy_ded_start_id+1)) {
                return SOC_E_PARAM;
            }
            /* Dedicated banks */
            if ((bank >= l3_phy_ded_start_id) && (bank <= l3_phy_ded_start_id+1)) {
                is_shared = 0;
            }
        }
        mem = L3_ENTRY_HASH_CONTROLm;
        shared_mem = UFT_SHARED_BANKS_CONTROLm;
        fid = ((bank > max_uft_shared_banks_id) ?
                (bank - l3_phy_ded_start_id) : bank);
        field = uft_fields[fid];

        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            return SOC_E_INTERNAL;
        }
#endif
        if (bank < 2 || bank > max_uft_shared_banks_id) {
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
        if (bank < 0 || bank > 3) {
            return SOC_E_PARAM;
        }
        mem = MPLS_ENTRY_HASH_CONTROLm;
        shared_mem = IP_UAT_SHARED_BANKS_CONTROLm;
        field = uat_fields[bank];
        break;
    case VLAN_XLATE_1_SINGLEm:
        if (bank < 0 || bank > 3) {
            return SOC_E_PARAM;
        }
        mem = VLAN_XLATE_1_HASH_CONTROLm;
        shared_mem = IP_UAT_SHARED_BANKS_CONTROLm;
        field = uat_fields[bank];
        break;
    case VLAN_XLATE_2_SINGLEm:
        if (bank < 0 || bank > 3) {
            return SOC_E_PARAM;
        }
        mem = VLAN_XLATE_2_HASH_CONTROLm;
        shared_mem = IP_UAT_SHARED_BANKS_CONTROLm;
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
#ifdef BCM_HURRICANE4_SUPPORT
    case SUBPORT_ID_TO_SGPP_MAPm:
        if (SOC_IS_HELIX5(unit)) {
            return SOC_E_INTERNAL;
        }
        if (bank < 0 || bank > 2) {
            return SOC_E_PARAM;
        }
        mem = SUBPORT_ID_TO_SGPP_MAP_HASH_CONTROLm;
        field = uft_fields[bank];
        is_shared = 0;
        break;
#endif
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
_soc_hx5_shared_hash(int unit, int hash_offset, uint32 key_nbits, uint8 *keyA,
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
soc_hx5_hash_index_get(int unit, int mem, int bank, int bucket)
{
    int rv;
    int index;
    int entries_per_row, bank_base, bucket_offset, entries_per_bucket;

    switch(mem) {
    case L2Xm:
        rv = soc_hx5_hash_bank_info_get(unit, mem, bank, NULL, &entries_per_row,
                                        NULL, &bank_base, &bucket_offset);
        if (SOC_SUCCESS(rv)) {
            return bank_base + bucket * entries_per_row + bucket_offset;
        }
        break;
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_SINGLEm:
    case L3_ENTRY_DOUBLEm:
    case L3_ENTRY_QUADm:
        rv = soc_hx5_hash_bank_info_get(unit, mem, bank, NULL,
                                        &entries_per_row, NULL, &bank_base,
                                        &bucket_offset);
        if (SOC_SUCCESS(rv)) {
            index = bank_base + bucket * entries_per_row + bucket_offset;
            return index;
        }
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:
        rv = soc_hx5_hash_bank_info_get(unit, mem, bank, NULL,
                                        &entries_per_row, &entries_per_bucket,
                                        &bank_base, &bucket_offset);
        if (SOC_SUCCESS(rv)) {
            index = bank_base + bucket * entries_per_bucket + bucket_offset;
            return index;
        }
        break;
    case ING_VP_VLAN_MEMBERSHIPm:
    case EGR_VP_VLAN_MEMBERSHIPm:
        rv = soc_hx5_hash_bank_info_get(unit, mem, bank, NULL,
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

uint32
soc_hx5_l2x_hash(int unit, int bank, int hash_offset, int use_lsb,
                 int key_nbits, void *base_entry, uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;
    uint32 bank_size = 4096;

    if (soc_feature(unit, soc_feature_uft_shared_8_banks_32k)) {
        bank_size = 8192;
    }

    bank_size = ((bank < 2) ? (bank_size/2) : bank_size);

    hash_mask = (bank_size - 1);
    hash_bits = _shr_popcount(hash_mask);

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        hash_mask = 0x1FFF; /* 8k buckets per dedicated L2 bank */
        hash_bits = 13;
    }
#endif
#if 0
    if (bank < 2) {
        hash_mask = 0x0FFF; /* 2k buckets per dedicated L2 bank */
        hash_bits = 12;
    } else {
        hash_mask = 0x01FFF; /* 4k buckets per shared bank */
        hash_bits = 13;
    }
#endif

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
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit) &&
                soc_feature(unit, soc_feature_hr4_a0_sobmh_war) &&
                (soc_mem_field32_get(unit, L2Xm, base_entry, KEY_TYPEf) == 15)) {
                lsb_val = soc_mem_field32_get(unit, L2Xm, base_entry,
                                              L2__HASH_LSBf);
                break;
            }
#endif
            lsb_val = 0;
            break;
        }
    } else {
        lsb_val = 0;
    }

    return _soc_hx5_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_hx5_l2x_base_entry_to_key(int unit, int bank, uint32 *entry, uint8 *key)
{
    soc_field_t field_list[2];
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
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit) &&
            soc_feature(unit, soc_feature_hr4_a0_sobmh_war) &&
            (soc_mem_field32_get(unit, L2Xm, entry, KEY_TYPEf) == 15)) {
            field_list[0] = L2__KEYf;
            break;
        }
#endif
        return 0;
    }

    return _soc_hx5_hash_entry_to_key(unit, bank, entry, key,
                            L2Xm, field_list, flex_mem_view);
}

uint32
soc_hx5_l2x_entry_hash(int unit, int bank, int hash_offset, int use_lsb,
                      uint32 *entry) {
    int key_nbits;
    uint8 key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8 key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_hx5_l2x_base_entry_to_key(unit, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_hx5_l2x_base_entry_to_key(unit, 1, entry, key2);

    return soc_hx5_l2x_hash(unit, bank, hash_offset, use_lsb, key_nbits, entry,
                            key1, key2);
}

uint32
soc_hx5_l2x_bank_entry_hash(int unit, int bank, uint32 *entry) {
    int rv, hash_offset, use_lsb;

    rv = soc_hx5_hash_offset_get(unit, L2Xm, bank, &hash_offset, &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }
    return soc_hx5_l2x_entry_hash(unit, bank, hash_offset, use_lsb, entry);
}

uint32
soc_hx5_l3x_hash(int unit, int bank, int hash_offset, int use_lsb,
                 int key_nbits, void *base_entry, uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;
    uint32 bank_size = 4096;
    int max_uft_shared_banks_id = 7;

    if (soc_feature(unit, soc_feature_uft_shared_8_banks_32k)) {
        max_uft_shared_banks_id = 9;
        bank_size = 8192;
    }

    bank_size = ((bank > max_uft_shared_banks_id) ? (bank_size/2) : bank_size);

    hash_mask = (bank_size - 1);
    hash_bits = _shr_popcount(hash_mask);


#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        hash_mask = 0xFFF; /* 4k buckets per dedicated bank */
        hash_bits = 12;
    }
#endif

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

    return _soc_hx5_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_hx5_l3x_base_entry_to_key(int unit, int bank, uint32 *entry, uint8 *key)
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
    return _soc_hx5_hash_entry_to_key(unit, bank, entry, key,
                             mem, field_list, flex_mem_view);
}

uint32
soc_hx5_l3x_entry_hash(int unit, int bank, int hash_offset, int use_lsb,
                      uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_hx5_l3x_base_entry_to_key(unit, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_hx5_l3x_base_entry_to_key(unit, 1, entry, key2);

    return soc_hx5_l3x_hash(unit, bank, hash_offset, use_lsb, key_nbits,
                            entry, key1, key2);
}

uint32
soc_hx5_l3x_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_hx5_hash_offset_get(unit, L3_ENTRY_ONLY_SINGLEm, bank, &hash_offset,
                                &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_hx5_l3x_entry_hash(unit, bank, hash_offset, use_lsb, entry);
}

uint32
soc_hx5_exact_match_hash(int unit, int bank, int hash_offset, int use_lsb,
                         int key_nbits, void *base_entry, uint8 *key1,
                         uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;
    uint32 bank_size = 2048;


    if (soc_feature(unit, soc_feature_uft_shared_8_banks_32k)) {
        bank_size = 4096;
    }

    hash_mask = (bank_size - 1);
    hash_bits = _shr_popcount(hash_mask);

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

    return _soc_hx5_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_hx5_exact_match_base_entry_to_key(int unit, int bank, uint32 *entry,
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
    return _soc_hx5_hash_entry_to_key(unit, bank, entry, key,
                             mem, field_list, flex_mem_view);
}

uint32
soc_hx5_exact_match_entry_hash(int unit, int bank, int hash_offset,
                              int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4];
    uint8           key2[SOC_MAX_MEM_WORDS * 4];

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_hx5_exact_match_base_entry_to_key(unit, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_hx5_exact_match_base_entry_to_key(unit, 1, entry, key2);

    return soc_hx5_exact_match_hash(unit, bank, hash_offset, use_lsb,
                                    key_nbits, entry, key1, key2);
}

uint32
soc_hx5_exact_match_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_hx5_hash_offset_get(unit, EXACT_MATCH_2m, bank, &hash_offset,
                                 &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_hx5_exact_match_entry_hash(unit, bank, hash_offset, use_lsb,
                                          entry);
}

uint32
soc_hx5_mpls_hash(int unit, soc_mem_t mem, int bank, int hash_offset, int use_lsb,
                  int key_nbits, void *base_entry, uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

   uint32 bank_size = 2048;


    if (soc_feature(unit, soc_feature_uft_shared_8_banks_32k)) {
        bank_size = 4096;
    }


    hash_mask = (bank_size - 1);
    hash_bits = _shr_popcount(hash_mask);

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        hash_mask = 0xFF; /* 256 buckets per shared bank of size 512 */
        hash_bits = 8;
    }
#endif

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

    return _soc_hx5_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_hx5_mpls_base_entry_to_key(int unit, soc_mem_t mem, int bank, void *entry, uint8 *key) {
    
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
    }
    return _soc_hx5_hash_entry_to_key(unit, bank, entry, key,
                      mem, field_list, flex_mem_view);
}

uint32
soc_hx5_mpls_entry_hash(int unit, soc_mem_t mem, int bank,
                        int hash_offset, int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_hx5_mpls_base_entry_to_key(unit, mem, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_hx5_mpls_base_entry_to_key(unit, mem, 1, entry, key2);

    return soc_hx5_mpls_hash(unit, mem, bank, hash_offset, use_lsb,
                             key_nbits, entry, key1, key2);
}

uint32
soc_hx5_mpls_bank_entry_hash(int unit, soc_mem_t mem,
                             int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_hx5_hash_offset_get(unit, mem, bank, &hash_offset,
                                 &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_hx5_mpls_entry_hash(unit, mem, bank, hash_offset, use_lsb, entry);
}

uint32
soc_hx5_vlan_xlate_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                        int use_lsb, int key_nbits, void *base_entry,
                        uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;
    uint32 bank_size = 2048;


    if (soc_feature(unit, soc_feature_uft_shared_8_banks_32k)) {
        bank_size = 4096;
    }

    hash_mask = (bank_size - 1);
    hash_bits = _shr_popcount(hash_mask);

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        hash_mask = 0xFF; /* 256 buckets per shared bank of size 512 */
        hash_bits = 8;
    }
#endif

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

    return _soc_hx5_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_hx5_vlan_xlate_base_entry_to_key(int unit, soc_mem_t mem, int bank,
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
    return _soc_hx5_hash_entry_to_key(unit, bank, entry, key,
                             mem, field_list, flex_mem_view);
}

uint32
soc_hx5_vlan_xlate_entry_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                              int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_hx5_vlan_xlate_base_entry_to_key(unit, mem, 0, entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_hx5_vlan_xlate_base_entry_to_key(unit, mem, 1, entry, key2);

    return soc_hx5_vlan_xlate_hash(unit, mem, bank, hash_offset, use_lsb,
                                   key_nbits, entry, key1, key2);
}

uint32
soc_hx5_vlan_xlate_bank_entry_hash(int unit, soc_mem_t mem, int bank,
                                   uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_hx5_hash_offset_get(unit, mem, bank, &hash_offset,
                                 &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_hx5_vlan_xlate_entry_hash(unit, mem, bank, hash_offset,
                                         use_lsb, entry);
}

uint32
soc_hx5_egr_vlan_xlate_hash(int unit, soc_mem_t mem, int bank, int hash_offset,
                            int use_lsb, int key_nbits, void *base_entry,
                            uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;
    uint32 bank_size = 2048;


    if (soc_feature(unit, soc_feature_uft_shared_8_banks_32k)) {
        bank_size = 4096;
    }


    hash_mask = (bank_size - 1);
    hash_bits = _shr_popcount(hash_mask);


#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        hash_mask = 0xFF; /* 256 buckets per shared bank of size 512 */
        hash_bits = 8;
    }
#endif

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

    return _soc_hx5_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_hx5_egr_vlan_xlate_base_entry_to_key(int unit, soc_mem_t mem, int bank,
                                         void *entry, uint8 *key)
{
    soc_field_t field_list[2];
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
        field_list[0] = CHANGE_L2_FIELDS__KEY_0f;
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
        break;
    }
    return _soc_hx5_hash_entry_to_key(unit, bank, entry,
                   key, mem, field_list, flex_mem_view);

}

uint32
soc_hx5_egr_vlan_xlate_entry_hash(int unit, soc_mem_t mem, int bank,
                                  int hash_offset, int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_hx5_egr_vlan_xlate_base_entry_to_key(unit, mem, 0,
                                                            entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_hx5_egr_vlan_xlate_base_entry_to_key(unit, mem, 1,
                                                            entry, key2);

    return soc_hx5_egr_vlan_xlate_hash(unit, mem, bank, hash_offset, use_lsb,
                                       key_nbits, entry, key1, key2);
}

uint32
soc_hx5_egr_vlan_xlate_bank_entry_hash(int unit, soc_mem_t mem, int bank,
                                       uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_hx5_hash_offset_get(unit, mem, bank, &hash_offset,
                                 &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_hx5_egr_vlan_xlate_entry_hash(unit, mem, bank, hash_offset,
                                             use_lsb, entry);
}

uint32
soc_hx5_ing_vp_vlan_member_hash(int unit, int bank, int hash_offset,
                                int use_lsb, int key_nbits, void *base_entry,
                                uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        hash_mask = 0x1FF; /* 512 buckets per shared bank of size 2k */
        hash_bits = 9;
    } else
#endif
    {
        hash_mask = 0x7FF; /* 2k buckets per dedicated bank of size 8k */
        hash_bits = 11;
    }

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        lsb_val = soc_mem_field32_get(unit, ING_VP_VLAN_MEMBERSHIPm,
                                      base_entry, HASH_LSBf);
    } else {
        lsb_val = 0;
    }

    return _soc_hx5_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_hx5_ing_vp_vlan_member_base_entry_to_key(int unit, int bank, void *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEYf,
        INVALIDf
    };

    return _soc_hx5_hash_entry_to_key(unit, bank, entry, key,
                                      ING_VP_VLAN_MEMBERSHIPm, field_list, 0);
}

uint32
soc_hx5_ing_vp_vlan_member_entry_hash(int unit, int bank, int hash_offset,
                                      int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_hx5_ing_vp_vlan_member_base_entry_to_key(unit, 0,
                                                             entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_hx5_ing_vp_vlan_member_base_entry_to_key(unit, 1,
                                                             entry, key2);

    return soc_hx5_ing_vp_vlan_member_hash(unit, bank, hash_offset, use_lsb,
                                           key_nbits, entry, key1, key2);
}

uint32
soc_hx5_ing_vp_vlan_member_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_hx5_hash_offset_get(unit, ING_VP_VLAN_MEMBERSHIPm, bank,
                                 &hash_offset, &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_hx5_ing_vp_vlan_member_entry_hash(unit, bank, hash_offset,
                                                 use_lsb, entry);
}

uint32
soc_hx5_egr_vp_vlan_member_hash(int unit, int bank, int hash_offset,
                                int use_lsb, int key_nbits, void *base_entry,
                                uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        hash_mask = 0x1FF; /* 512 buckets per shared bank of size 2k */
        hash_bits = 9;
    } else
#endif
    {
        hash_mask = 0x7FF; /* 2k buckets per dedicated bank of size 8k */
        hash_bits = 11;
    }

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        lsb_val = soc_mem_field32_get(unit, EGR_VP_VLAN_MEMBERSHIPm,
                                      base_entry, HASH_LSBf);
    } else {
        lsb_val = 0;
    }

    return _soc_hx5_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_hx5_egr_vp_vlan_member_base_entry_to_key(int unit, int bank, void *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEYf,
        INVALIDf
    };

    return _soc_hx5_hash_entry_to_key(unit, bank, entry, key,
                                      EGR_VP_VLAN_MEMBERSHIPm, field_list, 0);
}

uint32
soc_hx5_egr_vp_vlan_member_entry_hash(int unit, int bank, int hash_offset,
                                      int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_hx5_egr_vp_vlan_member_base_entry_to_key(unit, 0,
                                                             entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_hx5_egr_vp_vlan_member_base_entry_to_key(unit, 1,
                                                             entry, key2);

    return soc_hx5_egr_vp_vlan_member_hash(unit, bank, hash_offset, use_lsb,
                                           key_nbits, entry, key1, key2);
}

uint32
soc_hx5_egr_vp_vlan_member_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_hx5_hash_offset_get(unit, EGR_VP_VLAN_MEMBERSHIPm, bank,
                                 &hash_offset, &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_hx5_egr_vp_vlan_member_entry_hash(unit, bank, hash_offset,
                                                 use_lsb, entry);
}

#ifdef BCM_HURRICANE4_SUPPORT
uint32
soc_hx5_subport_id_to_sgpp_map_hash(int unit, int bank, int hash_offset,
                                    int use_lsb, int key_nbits,
                                    void *base_entry, uint8 *key1, uint8 *key2)
{
    uint32 lsb_val;
    uint32 hash_mask;
    uint32 hash_bits;

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        hash_mask = 0xFF; /* 256 buckets per shared bank of size 1k */
        hash_bits = 8;
    } else
#endif
    {
        hash_mask = 0x3FF; /* 1k buckets per dedicated bank of size 4k */
        hash_bits = 10;
    }

    if (use_lsb && (hash_offset + hash_bits > 32)) {
        lsb_val = soc_mem_field32_get(unit, SUBPORT_ID_TO_SGPP_MAPm,
                                      base_entry, HASH_LSBf);
    } else {
        lsb_val = 0;
    }

    return _soc_hx5_shared_hash(unit, hash_offset, key_nbits, key1, key2,
                                hash_mask, lsb_val, use_lsb);
}

int
soc_hx5_subport_id_to_sgpp_map_base_entry_to_key(int unit, int bank,
                                                 void *entry, uint8 *key)
{
    static soc_field_t field_list[] = {
        KEYf,
        INVALIDf
    };
    soc_mem_t   flex_mem_view = 0;

    return _soc_hx5_hash_entry_to_key(unit, bank, entry, key,
                                      SUBPORT_ID_TO_SGPP_MAPm, field_list, flex_mem_view);
}

uint32
soc_hx5_subport_id_to_sgpp_map_entry_hash(int unit, int bank, int hash_offset,
                                          int use_lsb, uint32 *entry)
{
    int             key_nbits;
    uint8           key1[SOC_MAX_MEM_WORDS * 4] = {0};
    uint8           key2[SOC_MAX_MEM_WORDS * 4] = {0};

    /* Robust Hashed Key using Remap and Action Tables A */
    key_nbits = soc_hx5_subport_id_to_sgpp_map_base_entry_to_key(unit, 0,
                                                                 entry, key1);
    /* Robust Hashed Key using Remap and Action Tables B */
    key_nbits = soc_hx5_subport_id_to_sgpp_map_base_entry_to_key(unit, 1,
                                                                 entry, key2);

    return soc_hx5_subport_id_to_sgpp_map_hash(unit, bank, hash_offset,
                                                      use_lsb, key_nbits, entry,
                                                      key1, key2);
}

uint32
soc_hx5_subport_id_to_sgpp_map_bank_entry_hash(int unit, int bank,
                                               uint32 *entry)
{
    int rv, hash_offset, use_lsb;

    rv = soc_hx5_hash_offset_get(unit, SUBPORT_ID_TO_SGPP_MAPm, bank,
                                 &hash_offset, &use_lsb);
    if (SOC_FAILURE(rv)) {
        return 0;
    }

    return soc_hx5_subport_id_to_sgpp_map_entry_hash(unit, bank, hash_offset,
                                                     use_lsb, entry);
}
#endif

int
soc_hx5_hash_bucket_get(int unit, int mem, int bank, uint32 *entry,
                        uint32 *bucket)
{

    int bank_size = 4096;
    int bank_ent_mul_factor = 1;
    int max_uft_shared_banks_id = 7;
    uint32 hash_mask = 0xFFF;

    if (soc_feature(unit, soc_feature_uft_shared_8_banks_32k)) {
        bank_ent_mul_factor = 2;
        max_uft_shared_banks_id = 9;
    }

    switch (mem) {
    case L2Xm:
        *bucket = soc_hx5_l2x_bank_entry_hash(unit, bank, entry);
        break;
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_SINGLEm:
        *bucket = soc_hx5_l3x_bank_entry_hash(unit, bank, entry);
        break;
    case L3_ENTRY_DOUBLEm:
        *bucket = soc_hx5_l3x_bank_entry_hash(unit, bank, entry);
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            *bucket &= 0x7FF;
        } else
#endif
        {

            bank_size = bank_ent_mul_factor * bank_size;
            bank_size = ((bank > max_uft_shared_banks_id)?
                            (bank_size/2) : bank_size);

            /* Divide 2 for the L3 DOuble. */
            bank_size /= 2;

            hash_mask = (bank_size - 1);

            *bucket &= hash_mask;
        }
        break;
    case L3_ENTRY_QUADm:
        *bucket = soc_hx5_l3x_bank_entry_hash(unit, bank, entry);
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            *bucket &= 0x3FF;
        } else
#endif
        {
            bank_size = bank_ent_mul_factor * bank_size;
            bank_size = ((bank > max_uft_shared_banks_id)?
                            (bank_size/2) : bank_size);

            /* Divide 4 for the L3 Quad. */
            bank_size /= 4;

            hash_mask = (bank_size - 1);
            *bucket &= hash_mask;
        }
        break;
    case EXACT_MATCH_2m:
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            return SOC_E_INTERNAL;
        }
#endif
        /* 8K entries => 2K buckets => 0x800 buckets */
        *bucket = soc_hx5_exact_match_bank_entry_hash(unit, bank, entry);
        break;
    case EXACT_MATCH_4m:
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            return SOC_E_INTERNAL;
        }
#endif
        *bucket = soc_hx5_exact_match_bank_entry_hash(unit, bank, entry);
        /* 4K entries => 1K buckets => 0x400 buckets */
            bank_size = bank_ent_mul_factor * bank_size;

            /* Divide 4 for the EXACT_MATCH_4. */
            bank_size /= 4;

            hash_mask = (bank_size - 1);
            *bucket &= hash_mask;
        break;

    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRYm:

        bank_size = (bank_ent_mul_factor * bank_size)/2;

        /*
         * Divide based on entry type. MPLS_ENTRY takes double width of single.
         * MPLS_ENTRY_SINGLE takes 2 base entries.
         */
        bank_size /= ((mem == MPLS_ENTRYm) ? 4 : 2);

        hash_mask = (bank_size - 1);

        *bucket = soc_hx5_mpls_bank_entry_hash(unit, mem, bank, entry);
        *bucket &= hash_mask;
        break;
    case VLAN_XLATE_1_SINGLEm:
    case VLAN_XLATE_2_SINGLEm:
    case VLAN_XLATE_1_DOUBLEm:
    case VLAN_XLATE_2_DOUBLEm:
        *bucket = soc_hx5_vlan_xlate_bank_entry_hash(unit, mem, bank, entry);
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            *bucket &= 0x7F;
            *bucket &= (((mem == VLAN_XLATE_1_DOUBLEm) ||
                         (mem == VLAN_XLATE_2_DOUBLEm)) ?
                        0x3F : 0x7F);
        } else
#endif
        {
            bank_size = (bank_ent_mul_factor * bank_size)/2;

            /*
             * Divide based on entry type. MPLS_ENTRY takes double width of single.
             * MPLS_ENTRY_SINGLE takes 2 base entries.
             */
            bank_size /= (((mem == VLAN_XLATE_1_DOUBLEm) ||
                         (mem == VLAN_XLATE_2_DOUBLEm)) ? 4: 2);

            hash_mask = (bank_size - 1);

            *bucket &= hash_mask;

        }
        break;

    case EGR_VLAN_XLATE_1_SINGLEm:
    case EGR_VLAN_XLATE_2_SINGLEm:
    case EGR_VLAN_XLATE_1_DOUBLEm:
    case EGR_VLAN_XLATE_2_DOUBLEm:
        *bucket = soc_hx5_egr_vlan_xlate_bank_entry_hash(unit, mem, bank, entry);
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            *bucket &= (((mem == EGR_VLAN_XLATE_1_DOUBLEm) ||
                         (mem == EGR_VLAN_XLATE_2_DOUBLEm)) ?
                        0x3F : 0x7F);
        } else
#endif
        {
            bank_size = (bank_ent_mul_factor * bank_size)/2;

            /*
             * Divide based on entry type. MPLS_ENTRY takes double width of single.
             * MPLS_ENTRY_SINGLE takes 2 base entries.
             */
            bank_size /= (((mem == EGR_VLAN_XLATE_1_DOUBLEm) ||
                         (mem == EGR_VLAN_XLATE_2_DOUBLEm)) ? 4: 2);

            hash_mask = (bank_size - 1);

            *bucket &= hash_mask;

        }
       break;

    case ING_VP_VLAN_MEMBERSHIPm:
        *bucket = soc_hx5_ing_vp_vlan_member_bank_entry_hash(unit,
                                                             bank, entry);
        break;
    case EGR_VP_VLAN_MEMBERSHIPm:
        *bucket = soc_hx5_egr_vp_vlan_member_bank_entry_hash(unit,
                                                             bank, entry);
        break;
#ifdef BCM_HURRICANE4_SUPPORT
    case SUBPORT_ID_TO_SGPP_MAPm:
        *bucket = soc_hx5_subport_id_to_sgpp_map_bank_entry_hash(unit,
                                                                 bank, entry);
        break;
#endif
    default:
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}
#endif /* BCM_HELIX5_SUPPORT */
