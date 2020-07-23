/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    stack.c
 * Purpose: This file manages MODPORT_MAP_SW table in Trident.
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TRIDENT_SUPPORT) 

#include <soc/mem.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/trunk.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/stack.h>

#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif

#define _BCM_TD_STK_HGT_GROUP_MAX    128

#define _BCM_STK_HGT_GROUP_MAX    _BCM_TD_STK_HGT_GROUP_MAX

typedef struct _bcm_td_modport_map_entry_s {
    uint8 dest_enable[2];
    uint8 dest_istrunk[2];
    uint8 dest[2];
    SHR_BITDCL higig_trunk_override[_SHR_BITDCLSIZE(_BCM_STK_HGT_GROUP_MAX)];
    uint8 voq_cos_valid;
    uint8 voq_group_id;
} _bcm_td_modport_map_entry_t;

typedef struct _bcm_td_modport_map_profile_s {
    int ref_count;
    int num_entries;
    _bcm_td_modport_map_entry_t *entry_array; 
} _bcm_td_modport_map_profile_t;

typedef struct _bcm_td_modport_map_info_s {
    int auto_include_disable; /* Disables automatic inclusion of all members
                                 of a Higig trunk group when one of its members
                                 is specified as the steering destination for
                                 a remote module */
    int num_profiles;
    _bcm_td_modport_map_profile_t *profile_array;
} _bcm_td_modport_map_info_t;

STATIC _bcm_td_modport_map_info_t _bcm_td_modport_map_info[BCM_MAX_NUM_UNITS];

#define MODPORT_MAP_INFO(unit) _bcm_td_modport_map_info[unit]
#define MODPORT_MAP_PROFILE(unit, index) \
    _bcm_td_modport_map_info[unit].profile_array[index]

/*
 * Function:
 *      _bcm_td_modport_map_mirror_profile_hw_copy
 * Purpose:
 *      Copy modport map profile from MODPORT_MAP_SW to MODPORT_MAP_MIRROR.
 * Parameters:
 *      unit - SOC unit #.
 *      index_min - Staring index.
 *      index_max - Ending index.
 *      modport_map_sw_buf - Buffer of MODPORT_MAP_SW.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td_modport_map_mirror_profile_hw_copy(int unit, int index_min,
        int index_max, uint8 *modport_map_sw_buf)
{
    int rv = BCM_E_NONE;
    int num_entries;
    int mirror_buf_size;
    uint8 *mirror_buf = NULL;
    int i;
    modport_map_sw_entry_t *modport_map_sw_entry;
    int enable, istrunk, dest;
    modport_map_mirror_entry_t *mirror_entry;

    num_entries = index_max - index_min + 1;
    mirror_buf_size = num_entries * 
        soc_mem_entry_words(unit, MODPORT_MAP_MIRRORm) * 4;
    mirror_buf = soc_cm_salloc(unit, mirror_buf_size,
            "modport map mirror buffer");
    if (NULL == mirror_buf) {
        return BCM_E_MEMORY;
    }
    sal_memset(mirror_buf, 0, mirror_buf_size);

    for (i = 0; i < num_entries; i++) {
        modport_map_sw_entry = soc_mem_table_idx_to_pointer(unit, MODPORT_MAP_SWm,
                modport_map_sw_entry_t *, modport_map_sw_buf, i);
        enable = soc_MODPORT_MAP_SWm_field32_get(unit, modport_map_sw_entry,
                ENABLE0f);
        istrunk = soc_MODPORT_MAP_SWm_field32_get(unit, modport_map_sw_entry,
                ISTRUNK0f);
        dest = soc_MODPORT_MAP_SWm_field32_get(unit, modport_map_sw_entry,
                DEST0f);

        mirror_entry = soc_mem_table_idx_to_pointer(unit, MODPORT_MAP_MIRRORm,
                modport_map_mirror_entry_t *, mirror_buf, i);
        soc_MODPORT_MAP_MIRRORm_field32_set(unit, mirror_entry,
                ENABLEf, enable);
        soc_MODPORT_MAP_MIRRORm_field32_set(unit, mirror_entry,
                ISTRUNKf, istrunk);
        soc_MODPORT_MAP_MIRRORm_field32_set(unit, mirror_entry,
                DESTf, dest);
    }
    rv = soc_mem_write_range(unit, MODPORT_MAP_MIRRORm, MEM_BLOCK_ALL,
            index_min, index_max, mirror_buf); 
    soc_cm_sfree(unit, mirror_buf);


#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (SOC_IS_TD2P_TT2P(unit) || SOC_IS_APACHE(unit) || SOC_IS_TRIDENT3X(unit)) {
        modport_map_subport_mirror_entry_t subport_m_entry;
        int port;

        sal_memset(&subport_m_entry, 0, sizeof(subport_m_entry));
        soc_mem_field32_set(unit, MODPORT_MAP_SUBPORT_M0m, &subport_m_entry, ENABLEf, 1);

        /* my_modid and other modid related initialization */
        PBMP_ALL_ITER(unit, port) {
            /* configure logical subport mirror numbers */    
            soc_mem_field32_set(unit, MODPORT_MAP_SUBPORT_M0m, &subport_m_entry, DESTf, port);
            if (soc_feature(unit, soc_feature_channelized_switching)) {

                soc_mem_field32_set(unit, MODPORT_MAP_SUBPORT_M0m, &subport_m_entry,
                    DST_SUBPORT_NUMf, port);
            }

            SOC_IF_ERROR_RETURN                        
                (WRITE_MODPORT_MAP_SUBPORT_M0m(unit, MEM_BLOCK_ALL, port, &subport_m_entry));
            SOC_IF_ERROR_RETURN                        
                (WRITE_MODPORT_MAP_SUBPORT_M1m(unit, MEM_BLOCK_ALL, port, &subport_m_entry));
            SOC_IF_ERROR_RETURN                        
                (WRITE_MODPORT_MAP_SUBPORT_M2m(unit, MEM_BLOCK_ALL, port, &subport_m_entry));
            SOC_IF_ERROR_RETURN                        
                (WRITE_MODPORT_MAP_SUBPORT_M3m(unit, MEM_BLOCK_ALL, port, &subport_m_entry));
                                                    
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_td_modport_map_profile_hw_write
 * Purpose:
 *      Write modport map profile to hardware.
 * Parameters:
 *      unit - SOC unit #.
 *      profile_index - Profile number.
 *      profile - Modport map profile.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td_modport_map_profile_hw_write(int unit, int profile_index,
        _bcm_td_modport_map_profile_t *profile)
{
    int rv = BCM_E_NONE;
    int profile_buf_size;
    uint8 *profile_buf = NULL;
    int i, j;
    modport_map_sw_entry_t *modport_map_entry;
    _bcm_td_modport_map_entry_t profile_entry;
    soc_field_t enable_field[2]  = {ENABLE0f, ENABLE1f};
    soc_field_t istrunk_field[2] = {ISTRUNK0f, ISTRUNK1f};
    soc_field_t dest_field[2]    = {DEST0f, DEST1f};
    bcm_gport_t gport;
    bcm_trunk_t tid;
    bcm_trunk_chip_info_t chip_info;
    int hg_trunk_group;
    int index_min, index_max;
    int dest[2] = {0};
#if defined(BCM_SABER2_SUPPORT)
	int mask1 = 0, bitlen1 = 0 , mask2 = 0, bitlen2= 0;
#endif
    profile_buf_size = profile->num_entries *
        soc_mem_entry_words(unit, MODPORT_MAP_SWm) * 4;
    profile_buf = soc_cm_salloc(unit, profile_buf_size, "modport map buffer");
    if (NULL == profile_buf) {
        return BCM_E_MEMORY;
    }
    sal_memset(profile_buf, 0, profile_buf_size);

    for (i = 0; i < profile->num_entries; i++) {
        modport_map_entry = soc_mem_table_idx_to_pointer(unit, MODPORT_MAP_SWm,
                modport_map_sw_entry_t *, profile_buf, i);
        profile_entry = profile->entry_array[i];

        if (soc_mem_field_valid(unit, MODPORT_MAP_SWm, VOQ_COS_VALIDf)) {
            soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_entry,
                    VOQ_COS_VALIDf, profile_entry.voq_cos_valid);
        }
        if (soc_mem_field_valid(unit, MODPORT_MAP_SWm, VOQ_GRP_IDf)) {
            soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_entry,
                    VOQ_GRP_IDf, profile_entry.voq_group_id);
        }

        for (j = 0; j < 2; j++) {
            if (profile_entry.dest_enable[j] == 0) {
                continue;
            }
            soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_entry,
                    enable_field[j], profile_entry.dest_enable[j]);

            if (profile_entry.dest_istrunk[j]) {
                soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_entry,
                        istrunk_field[j], 1);
                soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_entry,
                        dest_field[j], profile_entry.dest[j]);
            } else {
#if defined(BCM_KATANA2_SUPPORT)
                if (soc_feature(unit, soc_feature_linkphy_coe) ||
                    soc_feature(unit, soc_feature_subtag_coe)) {

                    /* For SB2, DEST0 11:0
                       ISTRUNK0 = 0:
                        11:7: Egress Physical Port
                         6:0: Egress PP_PORT
                       ISTRUNK0 = 1:
                        2:0: Egress HiGig Trunk Group ID */
#ifdef BCM_SABER2_SUPPORT
                    if (SOC_IS_SABER2(unit)) {
                        bitlen1  = soc_mem_field_length(unit, MODPORT_MAP_SWm, PHYSICAL_PORT1f);
                        mask1 = (1 << bitlen1) - 1;
                        bitlen2  = soc_mem_field_length(unit, MODPORT_MAP_SWm, PP_PORT1f);
                        mask2 = ( 1 << bitlen2) - 1;
                        dest[j] = ((profile_entry.dest[j] & mask1) << bitlen2) |
                            (profile_entry.dest[j] & mask2);
                    } else
#endif
                    if (SOC_IS_KATANA2(unit)){

                    /* DEST0 13:0
                       ISTRUNK0 = 0:
                        13:8: Egress Physical Port
                         7:0: Egress PP_PORT
                       ISTRUNK0 = 1:
                        3:0: Egress HiGig Trunk Group ID */
                    /* For KT2 physical port to pp_port is 1:1 mapped */
                         dest[j] = ((profile_entry.dest[j] & 0x3f) << 8) |
                              (profile_entry.dest[j] & 0xff);
                    } else {
                        LOG_ERROR(BSL_LS_BCM_COMMON,
                                (BSL_META_U(unit,
                                            "TODO: DEST0 to be programmed in required format\n")));
                        dest[j] = profile_entry.dest[j];
                    } 
                } else
#endif
                {
                    dest[j] = profile_entry.dest[j];
                }
                if (MODPORT_MAP_INFO(unit).auto_include_disable) {
                    soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_entry,
                            istrunk_field[j], 0);
                    soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_entry,
                            dest_field[j], dest[j]);
                } else {
                    SOC_GPORT_LOCAL_SET(gport, profile_entry.dest[j]);
                    rv = bcm_esw_trunk_find(unit, 0, gport, &tid);
                    if (rv == BCM_E_NOT_FOUND) {
                        /* dest is not a member of a Higig trunk group */
                        soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_entry,
                                istrunk_field[j], 0);
                        soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_entry,
                                dest_field[j], dest[j]);
                    } else if (BCM_FAILURE(rv)) {
                        goto cleanup;
                    } else {
                        /* dest is a member of a Higig trunk group */
                        rv = bcm_esw_trunk_chip_info_get(unit, &chip_info);
                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }
                        if (tid < chip_info.trunk_fabric_id_min) {
                            rv = BCM_E_PARAM;
                            goto cleanup;
                        }
                        hg_trunk_group = tid - chip_info.trunk_fabric_id_min;

                        if (SHR_BITGET(
                            profile_entry.higig_trunk_override, 
                            hg_trunk_group)) {
                            soc_MODPORT_MAP_SWm_field32_set(unit,
                                    modport_map_entry,
                                    istrunk_field[j], 0);
                            soc_MODPORT_MAP_SWm_field32_set(unit,
                                    modport_map_entry,
                                    dest_field[j], dest[j]);
                        } else {
                            soc_MODPORT_MAP_SWm_field32_set(unit,
                                    modport_map_entry,
                                    istrunk_field[j], 1);
                            soc_MODPORT_MAP_SWm_field32_set(unit,
                                    modport_map_entry,
                                    dest_field[j], hg_trunk_group);
                        }
                    }
                }
            }
        }
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
        /* Set the HW entries to 0 if the link status is down */
        if (soc_feature(unit, soc_feature_epc_linkflap_recover) &&
            profile_entry.dest_enable[0] == 1 &&
            profile_entry.dest_enable[1] == 1) {
            int k;
            for (k = 0; k < 2; k++) {
                int link = -1;
                bcm_esw_port_link_status_get(unit, profile_entry.dest[k], &link);
                if (link == 0 && !profile_entry.dest_istrunk[k]) {
                    soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_entry,
                                                    enable_field[k], 0);
                    soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_entry,
                                                    dest_field[k], 0);
                }
            }
        }
#endif /*defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)*/
    }

    index_min = profile_index * profile->num_entries;
    index_max = index_min + profile->num_entries - 1;
    rv = soc_mem_write_range(unit, MODPORT_MAP_SWm, MEM_BLOCK_ALL,
            index_min, index_max, profile_buf); 
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    rv = _bcm_td_modport_map_mirror_profile_hw_copy(unit, index_min, index_max,
            profile_buf); 
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

cleanup:
    if (NULL != profile_buf) {
        soc_cm_sfree(unit, profile_buf);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td_modport_map_profile_add
 * Purpose:
 *      Add a modport map profile.
 * Parameters:
 *      unit - SOC unit #.
 *      profile - Modport map profile.
 *      profile_index - (OUT) Profile number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td_modport_map_profile_add(int unit,
        _bcm_td_modport_map_profile_t *profile,
        int *profile_index) 
{
    int i;

    /* Check if the given profile matches any existing profile */

    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
        if (MODPORT_MAP_PROFILE(unit, i).ref_count == 0) {
            continue;
        }
        if (MODPORT_MAP_PROFILE(unit, i).num_entries != profile->num_entries) {
            return BCM_E_INTERNAL;
        }
        if (0 == sal_memcmp(MODPORT_MAP_PROFILE(unit, i).entry_array,
                    profile->entry_array,
                    profile->num_entries * sizeof(_bcm_td_modport_map_entry_t))) {
            MODPORT_MAP_PROFILE(unit, i).ref_count++;
            *profile_index = i;
            return BCM_E_NONE;
        }
    }

    /* The given profile does not match any existing profile */

    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
        if (MODPORT_MAP_PROFILE(unit, i).ref_count == 0) {
            MODPORT_MAP_PROFILE(unit, i).num_entries = profile->num_entries;
            sal_memcpy(MODPORT_MAP_PROFILE(unit, i).entry_array,
                    profile->entry_array,
                    profile->num_entries * sizeof(_bcm_td_modport_map_entry_t));
            MODPORT_MAP_PROFILE(unit, i).ref_count++;
            *profile_index = i;
            BCM_IF_ERROR_RETURN
                (_bcm_td_modport_map_profile_hw_write(unit, i, profile));
            return BCM_E_NONE;
        }
    }

    /* No available profile */
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_td_modport_map_profile_delete
 * Purpose:
 *      Delete a modport map profile.
 * Parameters:
 *      unit - SOC unit #.
 *      profile_index - Profile number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td_modport_map_profile_delete(int unit, int profile_index) 
{
    if (MODPORT_MAP_PROFILE(unit, profile_index).ref_count == 0) {
        return BCM_E_INTERNAL;
    }

    MODPORT_MAP_PROFILE(unit, profile_index).ref_count--;

    if (MODPORT_MAP_PROFILE(unit, profile_index).ref_count == 0) {
        sal_memset(MODPORT_MAP_PROFILE(unit, profile_index).entry_array, 0,
                sizeof(_bcm_td_modport_map_entry_t) *
                MODPORT_MAP_PROFILE(unit, profile_index).num_entries);
        BCM_IF_ERROR_RETURN(_bcm_td_modport_map_profile_hw_write(unit,
                    profile_index,
                    &MODPORT_MAP_PROFILE(unit, profile_index)));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td_modport_map_free_resources
 * Purpose:
 *      Free resources of modport map profiles.
 * Parameters:
 *      unit - SOC unit #.
 * Returns:
 *      None
 */
STATIC void
_bcm_td_modport_map_free_resources(int unit)
{
    int i;

    if (MODPORT_MAP_INFO(unit).profile_array) {
        for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
            if (MODPORT_MAP_PROFILE(unit, i).entry_array) {
                sal_free(MODPORT_MAP_PROFILE(unit, i).entry_array);
                MODPORT_MAP_PROFILE(unit, i).entry_array = NULL;
            }
        }
        sal_free(MODPORT_MAP_INFO(unit).profile_array);
        MODPORT_MAP_INFO(unit).profile_array = NULL;
    }
}

/*
 * Function:
 *      bcm_td_modport_map_init
 * Purpose:
 *      Initialize modport map profiles.
 * Parameters:
 *      unit - SOC unit #.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td_modport_map_init(int unit)
{
    int rv = BCM_E_NONE;
    int i;
    _bcm_td_modport_map_profile_t profile;
    int profile_index;
    int port_count;
    bcm_port_t port;
    bcm_pbmp_t all_pbmp;

    BCM_PBMP_CLEAR(all_pbmp);
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &all_pbmp);
    }
    if (SOC_IS_KATANA2(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &all_pbmp));
    }
#endif

    sal_memset(&MODPORT_MAP_INFO(unit), 0,
            sizeof(_bcm_td_modport_map_info_t));

    /* By default, all members of a Higig trunk group is automatically
     * included if one of its members is specified as the steering
     * destination for a remote module.
     */
    MODPORT_MAP_INFO(unit).auto_include_disable = 0;

    MODPORT_MAP_INFO(unit).num_profiles =
        soc_mem_index_count(unit, MODPORT_MAP_SWm) / (SOC_MODID_MAX(unit) + 1);

    if (NULL == MODPORT_MAP_INFO(unit).profile_array) {
        MODPORT_MAP_INFO(unit).profile_array =
            sal_alloc(sizeof(_bcm_td_modport_map_profile_t) *
                    MODPORT_MAP_INFO(unit).num_profiles, "modport map profiles");
        if (NULL == MODPORT_MAP_INFO(unit).profile_array) {
            _bcm_td_modport_map_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(MODPORT_MAP_INFO(unit).profile_array, 0,
            sizeof(_bcm_td_modport_map_profile_t) *
            MODPORT_MAP_INFO(unit).num_profiles);

    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
        MODPORT_MAP_PROFILE(unit, i).num_entries = SOC_MODID_MAX(unit) + 1;
        if (NULL == MODPORT_MAP_PROFILE(unit, i).entry_array) {
            MODPORT_MAP_PROFILE(unit, i).entry_array =
                sal_alloc(sizeof(_bcm_td_modport_map_entry_t) *
                        MODPORT_MAP_PROFILE(unit, i).num_entries, "modport map entries");
            if (NULL == MODPORT_MAP_PROFILE(unit, i).entry_array) {
                _bcm_td_modport_map_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(MODPORT_MAP_PROFILE(unit, i).entry_array, 0,
                sizeof(_bcm_td_modport_map_entry_t) *
                MODPORT_MAP_PROFILE(unit, i).num_entries);
    }

    if (!SOC_WARM_BOOT(unit)) {  
        /* Create a profile of all zeroes and have all ports point to it */

        profile.num_entries = SOC_MODID_MAX(unit) + 1;
        profile.entry_array = sal_alloc(sizeof(_bcm_td_modport_map_entry_t) *
                profile.num_entries, "profile entry array");
        if (NULL == profile.entry_array) {
            return BCM_E_MEMORY;
        }
        sal_memset(profile.entry_array, 0, sizeof(_bcm_td_modport_map_entry_t) *
                profile.num_entries);
        rv = _bcm_td_modport_map_profile_add(unit, &profile, &profile_index); 
        sal_free(profile.entry_array);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        port_count = 0;
        PBMP_ITER(all_pbmp, port) {
            BCM_IF_ERROR_RETURN(WRITE_MODPORT_MAP_SELr(unit, port, profile_index));
            port_count++;
        }
        MODPORT_MAP_PROFILE(unit, profile_index).ref_count = port_count;
    }

    return rv;
}

/*
 * Function:
 *      bcm_td_modport_map_detach
 * Purpose:
 *      Detach modport map profiles.
 * Parameters:
 *      unit - SOC unit #.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td_modport_map_detach(int unit)
{
    _bcm_td_modport_map_free_resources(unit);
    return BCM_E_NONE;
}

int 
bcm_td_modport_map_hw_write_entry(int unit, int index, 
                int select_index, bcm_port_t dest_port, int set)
{
    int rv = BCM_E_NONE;
    uint8 *profile_buf = NULL;
    int profile_buf_size;
    modport_map_sw_entry_t *modport_map_sw_entry;
    int enable[2];
    int dest[2];
    soc_field_t enable_field[2]  = {ENABLE0f, ENABLE1f};
    soc_field_t dest_field[2]    = {DEST0f, DEST1f};
    int i, num_dest_supported;

    num_dest_supported = 2;
    sal_memset(enable, 0, sizeof(int) * 2);
    sal_memset(dest, 0, sizeof(int) * 2);

    /* 1 Entry x 1 Word x 4 (bytes) */
    profile_buf_size = 1 * 4 * soc_mem_entry_words(unit, MODPORT_MAP_SWm);
    profile_buf = soc_cm_salloc(unit, profile_buf_size,
                    "modport map entry buffer");
    if (NULL == profile_buf) {
        return BCM_E_MEMORY;
    }

    rv = soc_mem_read(unit, MODPORT_MAP_SWm, MEM_BLOCK_ANY,
                            index, profile_buf);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, profile_buf);
        return BCM_E_RESOURCE;
    }
    modport_map_sw_entry = soc_mem_table_idx_to_pointer(unit,
        MODPORT_MAP_SWm, modport_map_sw_entry_t *, profile_buf, 0);

    for (i = 0; i < num_dest_supported; i++) {
        if (soc_mem_field_valid(unit, MODPORT_MAP_SWm, enable_field[i])) {
            enable[i] = soc_MODPORT_MAP_SWm_field32_get(unit, modport_map_sw_entry,
                    enable_field[i]);
        }
        if (soc_mem_field_valid(unit, MODPORT_MAP_SWm, dest_field[i])) {
            dest[i] = soc_MODPORT_MAP_SWm_field32_get(unit, modport_map_sw_entry,
                    dest_field[i]);
        }
    }
    if ((set == 1)) {
        soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_sw_entry,
                enable_field[select_index], set);
        soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_sw_entry,
                dest_field[select_index], dest_port);
    } else if (enable[select_index] && (dest[select_index] == dest_port)) {
        soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_sw_entry,
                enable_field[select_index], 0);
        soc_MODPORT_MAP_SWm_field32_set(unit, modport_map_sw_entry,
                dest_field[select_index], 0);
    } else {
        soc_cm_sfree(unit, profile_buf);
        return BCM_E_INTERNAL;
    }
    rv = soc_mem_write(unit, MODPORT_MAP_SWm, MEM_BLOCK_ANY,
                            index, profile_buf);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, profile_buf);
        return BCM_E_RESOURCE;
    }
    rv = _bcm_td_modport_map_mirror_profile_hw_copy(unit, index, index,
                        profile_buf);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, profile_buf);
        return BCM_E_RESOURCE;
    }
    soc_cm_sfree(unit, profile_buf);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td_stk_is_port_hgtid_member
 * Purpose:
 *      Helper function to find if the hgtid
 *      (got from MODPORT_MAP_SW.DEST for trunk case)
 *      has the port as its member.
 *      Also returns the corresponding trunk id if the port
 *      is member of HGTID.
 * Parameters:
 *      unit    - (IN) SOC unit #
 *      hgtid   - (IN) Higig Trunk id (index of HG_TRUNK_BITMAP)
 *      port    - (IN) Member port
 *      trunkid - (OUT) trunk id of the given port
 * Returns:
 *      True  - Port is a member of hgtid
 *      False - Port is not a member of hgtid
 */
STATIC int
_bcm_td_stk_is_port_hgtid_member(int unit, int hgtid,
                        bcm_port_t port, int *trunkid)
{
    int tid, rv, mymodid;
    bcm_trunk_chip_info_t chip_info;
    BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &chip_info));
    if (chip_info.trunk_fabric_id_min == -1) {
        return 0;
    }
    rv = bcm_esw_stk_my_modid_get(unit, &mymodid);
    if (BCM_FAILURE(rv)) {
        return 0;
    }
    rv = bcm_esw_trunk_find(unit, mymodid, port, &tid);
    if (BCM_FAILURE(rv)) {
        return 0;
    }
    tid -= chip_info.trunk_fabric_id_min;
    if (hgtid == tid) {
        *trunkid = tid + chip_info.trunk_fabric_id_min;
        return 1;
    }
    return 0;
}

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
STATIC int
_bcm_td_sdk_is_port_set_in_epc_link_bmap(int unit, bcm_port_t port,
                                         int *set)
{
   epc_link_bmap_entry_t  epc_link_bmap_entry;
   soc_pbmp_t             link_pbmp;

   SOC_PBMP_CLEAR(link_pbmp);

   BCM_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(
                unit, MEM_BLOCK_ANY, 0, &epc_link_bmap_entry));
   soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm,
            &epc_link_bmap_entry, PORT_BITMAPf,&link_pbmp);
   if (SOC_PBMP_MEMBER(link_pbmp, port)) {
       /* port is there in EPC_LINK_BMAP */
       *set = 1;
   } else {
       *set = 0;
   }
   return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_td_stk_modport_map_is_member_link_handler
 * Purpose:
 *      Called from bcm_esw_trunk_set and bcm_esw_trunk_member_add
 *      to check if this member shall be added.
 * Parameters:
 *      unit      - (IN) SOC unit #
 *      gport     - (IN) gport for member port
 *      trunk_id  - (IN) trunk identifier
 *      test_only - (IN) if set to 1 no need to update cache.
 *      status    - (OUT)set to 1 if port needs to be added to trunk.
 *                       else set to 0.
 * Returns:
 *      BCM_E_XXXX
 */
int
_bcm_td_stk_modport_map_member_link_handler(int unit,
                                            bcm_gport_t gport,
                                            bcm_trunk_t trunk_id,
                                            int test_only,
                                            int *status)
{
    int                           i, j, k;
    int                           enable[2];
    int                           dest[2];
    int                           istrunk[2];
    int                           num_dest_supported;
    int                           set = 1;
    int                           rv = BCM_E_NONE;
    bcm_port_t                    port;
    _bcm_td_modport_map_profile_t *profile;

    if (!status) {
        return BCM_E_PARAM;
    }

    num_dest_supported = 2;
    rv =  bcm_esw_port_local_get(unit, gport, &port);
    if (rv != BCM_E_NONE) {
        *status = 1;
        return BCM_E_NONE;
    }

    /* if port is not local hg port return */
    if (!IS_ST_PORT(unit, port)) {
        *status = 1;
        return BCM_E_NONE;
    }

    rv = _bcm_td_sdk_is_port_set_in_epc_link_bmap(unit, port, &set);
    if (rv != BCM_E_NONE) {
        *status = 1;
        return BCM_E_NONE;
    }
    if (set) {
        *status = 1;
        return BCM_E_NONE;
    }
    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
         if (MODPORT_MAP_PROFILE(unit, i).ref_count != 0) {
             profile = &MODPORT_MAP_PROFILE(unit, i);
             for (j = 0; j < profile->num_entries; j++) {
                 for (k = 0; k < num_dest_supported; k++) {
                       enable[k] = profile->entry_array[j].dest_enable[k];
                       dest[k] = profile->entry_array[j].dest[k];
                      istrunk[k] = profile->entry_array[j].dest_istrunk[k];
                  }
                  for (k = 0; k < num_dest_supported; k++) {
                       int port_is_cached = 0;
                      bcm_trunk_t dummy_tid;
                       if (enable[k] && istrunk[k] &&
                           enable[(k+1)%num_dest_supported]) {
                           if (_bcm_td_stk_is_port_hgtid_member(
                               unit, dest[k], port, &dummy_tid)) {
                               int mymodid;
                               rv = bcm_esw_stk_my_modid_get(unit, &mymodid);
                               if (BCM_FAILURE(rv)) {
                                   return rv;
                               }
                           }
                           if ((trunk_id == -1) &&
                               (_BCM_PORT_TRUNK_LINKFLAP_GET(unit, port) !=
                               BCM_TRUNK_INVALID)) {
                               port_is_cached = 1;
                               trunk_id =
                               _BCM_PORT_TRUNK_LINKFLAP_GET(unit, port);
                           }
                           if (trunk_id != -1) {
                               if (!port_is_cached) {
                                   if (!test_only) {
                                        _BCM_PORT_TRUNK_LINKFLAP_SET(
                                                      unit, port, trunk_id);
                                   }
                                   *status = 0;
                               } else {
                                   *status = 1;
                               }
                           }
                       }
                  }
             }
         }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td_stk_modport_map_linkscan_handler
 * Purpose:
 *      Called from linkscan handler when port toggles.
 *      Enables and Disables the port in MODPORT_MAP_SW.
 * Parameters:
 *      unit  - (IN) SOC unit #
 *      info  - (IN) port info
 * Returns:
 *      None
 */
int 
_bcm_td_stk_modport_map_linkscan_handler(int unit, 
                bcm_port_t port, bcm_port_info_t *info)
{
    int i, j, k;
    int enable[2];
    int dest[2];
    int istrunk[2];
    int num_dest_supported;
    _bcm_td_modport_map_profile_t *profile;
    int index, set;
    int rv = BCM_E_NONE;

    if (!info) {
        return BCM_E_PARAM;
    }

    num_dest_supported = 2;
    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
        if (MODPORT_MAP_PROFILE(unit, i).ref_count != 0) {
            profile = &MODPORT_MAP_PROFILE(unit, i);
            for (j = 0; j < profile->num_entries; j++) {
                for (k = 0; k < num_dest_supported; k++) {
                    enable[k] = profile->entry_array[j].dest_enable[k];
                    dest[k] = profile->entry_array[j].dest[k];
                    istrunk[k] = profile->entry_array[j].dest_istrunk[k];
                }
                /* For cases where the profile points to a trunk
                 * and the port in question is a member of the trunk,
                 * the port is removed from the trunk group when it is shut.
                 * This state is saved so that upon port enable, the port is added
                 * back to the trunk group.
                 */
                for (k = 0; k < num_dest_supported; k++) {
                    int port_is_cached = 0;
                    bcm_trunk_t trunk_id = -1;
                    /* Remove the port from trunk, only if both DEST are enabled */
                    if (enable[k] && istrunk[k] && enable[(k+1)%num_dest_supported]) {
                        if (_bcm_td_stk_is_port_hgtid_member(unit, dest[k], port, &trunk_id)) {
                            int mymodid;
                            rv = bcm_esw_stk_my_modid_get(unit, &mymodid);
                            if (BCM_FAILURE(rv)) {
                                return rv;
                            }
                        }
                        if ((trunk_id == -1) &&
                            (_BCM_PORT_TRUNK_LINKFLAP_GET(unit, port) != BCM_TRUNK_INVALID)) {
                            port_is_cached = 1;
                            trunk_id = _BCM_PORT_TRUNK_LINKFLAP_GET(unit, port);
                        }
                        if (trunk_id != -1) {
                            bcm_trunk_member_t p_trunk_member;
                            bcm_trunk_member_t_init(&p_trunk_member);
                            BCM_GPORT_LOCAL_SET(p_trunk_member.gport, port);
                            if ((!port_is_cached) &&
                                (info->linkstatus == BCM_PORT_LINK_STATUS_DOWN)) {
                                rv = bcm_esw_trunk_member_delete(unit, trunk_id, &p_trunk_member);
                                if (BCM_FAILURE(rv)) {
                                    return rv;
                                }
                                _BCM_PORT_TRUNK_LINKFLAP_SET(unit, port, trunk_id);
                            } else if ((port_is_cached) &&
                                       (info->linkstatus == BCM_PORT_LINK_STATUS_UP)) {
                                /* Check if EPC_PORT_BMAP is updated */
                                BCM_IF_ERROR_RETURN(_bcm_td_sdk_is_port_set_in_epc_link_bmap(
                                                                              unit, port, &set));
                                if (set) {
                                    rv = bcm_esw_trunk_member_add(unit, trunk_id, &p_trunk_member);
                                    if (BCM_FAILURE(rv)) {
                                        return rv;
                                    }
                                    _BCM_PORT_TRUNK_LINKFLAP_CLEAR(unit, port);
                                }

                            }
                            continue;
                        }
                    }
                }
                /* Max Number of destination for a modid is currently 2 */
                if (!istrunk[0] && !istrunk[1] && enable[0] && enable[1]) {
                    int select;
                    if (dest[0] == port) {
                        select = 0;
                    } else if (dest[1] == port) {
                        select = 1;
                    } else {
                        continue;
                    }
                    index = (i * profile->num_entries) + j;
                    if (info->linkstatus == BCM_PORT_LINK_STATUS_DOWN) {
                        rv = bcm_td_modport_map_hw_write_entry(unit, index, 
                                                            select, port, 0);
                    } else  if (info->linkstatus == BCM_PORT_LINK_STATUS_UP) {
                        rv = bcm_td_modport_map_hw_write_entry(unit, index, 
                                                            select, port, 1);
                    }
                    if (BCM_FAILURE(rv)){
                        return rv;
                    }
                }
            }
        }
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      bcm_td_stk_port_modport_op
 * Purpose:
 *      Set, add, or delete modport mappings.
 * Parameters:
 *      unit - SOC unit #.
 *      op - Set, add, or delete.
 *      ing_port - Ingress port.
 *      dest_modid - Destination module ID.
 *      dest_port - Array of destination Higig ports.
 *      dest_port_count - Size of dest_port array.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td_stk_port_modport_op(int unit, int op, bcm_port_t ing_port,
                           bcm_module_t dest_modid, bcm_port_t *dest_port,
                           int dest_port_count)
{
    int rv = BCM_E_NONE;
    int num_dest_supported;
    int modid_count, modid_min, modid_max;
    int hgtid[SOC_MAX_NUM_PORTS];
    int i, k, j;
    bcm_trunk_t tid;
    bcm_trunk_chip_info_t chip_info;
    uint32 rval;
    int profile_index, new_profile_index;
    _bcm_td_modport_map_profile_t profile;
    int entry_array_size;
    int enable[2];
    int istrunk[2];
    int dest[2];
    int match;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (op <= 0 || op >= _BCM_STK_PORT_MODPORT_OP_COUNT) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(ing_port)) {
        BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, ing_port, &ing_port));
    }
    if (!SOC_PORT_VALID(unit, ing_port)) {
        return BCM_E_PORT;
    }

    modid_count = SOC_MODID_MAX(unit) + 1;
    if (dest_modid == -1) {
        modid_min = 0;
        modid_max = modid_count - 1;
    } else {
        if (!SOC_MODID_ADDRESSABLE(unit, dest_modid)) {
            return BCM_E_PARAM;
        }
        modid_min = modid_max = dest_modid;
    }

    num_dest_supported = 2;
    if (dest_port_count > num_dest_supported) {
        return BCM_E_RESOURCE;
    }

    sal_memset(hgtid, 0, sizeof(hgtid));
    for (i = 0; i < dest_port_count; i++) {
        if (BCM_GPORT_IS_TRUNK(dest_port[i])) {
            tid = SOC_GPORT_TRUNK_GET(dest_port[i]);
            BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &chip_info));
            if (chip_info.trunk_fabric_id_min >= 0 &&
                    tid >= chip_info.trunk_fabric_id_min) { /* fabric trunk */
                hgtid[i] = tid - chip_info.trunk_fabric_id_min;
            } else {
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, dest_port[i])) {
                return BCM_E_PORT;
            }
        }
    }

    soc_mem_lock(unit, MODPORT_MAP_SWm);

    /* Get ingress port's current profile index */
    rv = READ_MODPORT_MAP_SELr(unit, ing_port, &rval);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        return rv;
    }
    profile_index = soc_reg_field_get(unit, MODPORT_MAP_SELr, rval,
                MODPORT_MAP_INDEX_UPPERf);

    /* Get current profile */
    profile.num_entries = MODPORT_MAP_PROFILE(unit, profile_index).num_entries;
    entry_array_size = profile.num_entries * sizeof(_bcm_td_modport_map_entry_t);
    profile.entry_array = sal_alloc(entry_array_size, "modport map profile entry array");
    if (NULL == profile.entry_array) {
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        return BCM_E_MEMORY;
    }
    sal_memcpy(profile.entry_array,
            MODPORT_MAP_PROFILE(unit, profile_index).entry_array,
            entry_array_size);

    /* Modify profile */
    for (i = modid_min; i <= modid_max; i++) {
        for (k = 0; k < num_dest_supported; k++) {
            enable[k] = profile.entry_array[i].dest_enable[k]; 
            istrunk[k] = profile.entry_array[i].dest_istrunk[k]; 
            dest[k] = profile.entry_array[i].dest[k]; 
        }

        switch (op) {
            case _BCM_STK_PORT_MODPORT_OP_SET:
                for (k = 0; k < dest_port_count; k++) {
                    enable[k] = 1;
                    if (BCM_GPORT_IS_TRUNK(dest_port[k])) {
                        istrunk[k] = 1;
                        dest[k] = hgtid[k];
                    } else {
                        istrunk[k] = 0;
                        dest[k] = dest_port[k];
                    }
                }
                for (k = dest_port_count; k < num_dest_supported; k++) {
                    enable[k] = 0;
                    istrunk[k] = 0;
                    dest[k] = 0;
                }
                break;

            case _BCM_STK_PORT_MODPORT_OP_ADD:
                for (k = 0; k < dest_port_count; k++) {
                    /* Find the first free destination */
                    for (j = 0; j < num_dest_supported; j++) {
                        if (enable[j] == 0) {
                            break;
                        }
                    }
                    if (j == num_dest_supported) {
                        /* A free destination cannot be found */
                        soc_mem_unlock(unit, MODPORT_MAP_SWm);
                        sal_free(profile.entry_array);
                        return BCM_E_RESOURCE;
                    }
                    enable[j] = 1;
                    if (BCM_GPORT_IS_TRUNK(dest_port[k])) {
                        istrunk[j] = 1;
                        dest[j] = hgtid[k];
                    } else {
                        istrunk[j] = 0;
                        dest[j] = dest_port[k];
                    }
                }
                break;

            case _BCM_STK_PORT_MODPORT_OP_DELETE:
                for (k = 0; k < dest_port_count; k++) {
                    /* Find the matching destination */
                    match = 0;
                    for (j = 0; j < num_dest_supported; j++) {
                        if (enable[j] == 1) {
                            if (BCM_GPORT_IS_TRUNK(dest_port[k])) {
                                if (istrunk[j] == 1 &&
                                        dest[j] == hgtid[k]) {
                                    match = 1;
                                }
                            } else {
                                if (istrunk[j] == 0 &&
                                        dest[j] == dest_port[k]) {
                                    match = 1;
                                } 
                            }
                            if (match) {
                                enable[j] = 0;
                                istrunk[j] = 0;
                                dest[j] = 0;
                                break;
                            }
                        }
                    }
                    if (!match) {
                        soc_mem_unlock(unit, MODPORT_MAP_SWm);
                        sal_free(profile.entry_array);
                        return BCM_E_NOT_FOUND;
                    }
                }
                break;

            default:
                soc_mem_unlock(unit, MODPORT_MAP_SWm);
                sal_free(profile.entry_array);
                return BCM_E_INTERNAL;
        }

        for (k = 0; k < num_dest_supported; k++) {
            profile.entry_array[i].dest_enable[k] = enable[k];
            profile.entry_array[i].dest_istrunk[k] = istrunk[k];
            profile.entry_array[i].dest[k] = dest[k];
        }
    }

    /* Add modified profile */
    rv = _bcm_td_modport_map_profile_add(unit, &profile, &new_profile_index);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        sal_free(profile.entry_array);
        return rv;
    }

    /* Update ingress port's profile index */
    soc_reg_field_set(unit, MODPORT_MAP_SELr, &rval,
                MODPORT_MAP_INDEX_UPPERf, new_profile_index);
    rv = WRITE_MODPORT_MAP_SELr(unit, ing_port, rval);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        sal_free(profile.entry_array);
        return rv;
    }

    /* Remove old profile */
    rv = _bcm_td_modport_map_profile_delete(unit, profile_index);
    soc_mem_unlock(unit, MODPORT_MAP_SWm);
    sal_free(profile.entry_array);
    return rv;
}

/*
 * Function:
 *      bcm_td_stk_port_modport_get
 * Purpose:
 *      Get modport mappings.
 * Parameters:
 *      unit - SOC unit #.
 *      ing_port - Ingress port.
 *      dest_modid - Destination module ID.
 *      dest_port_max - Size of dest_port_array.
 *      dest_port_array - (OUT) Array of destination Higig ports.
 *      dest_port_count - (OUT) Number of ports returned in dest_port array.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td_stk_port_modport_get(int unit, bcm_port_t ing_port,
                          bcm_module_t dest_modid, int dest_port_max,
                          bcm_port_t *dest_port_array,
                          int *dest_port_count)
{
    uint32 rval;
    int profile_index;
    _bcm_td_modport_map_profile_t profile;
    int count;
    int num_dest_supported;
    int i;
    int enable, istrunk, dest;
    bcm_trunk_chip_info_t chip_info;
    bcm_trunk_t tid;

    if (BCM_GPORT_IS_SET(ing_port)) {
        BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, ing_port, &ing_port));
    }
    if (!SOC_PORT_VALID(unit, ing_port)) {
        return BCM_E_PORT;
    }

    if (!SOC_MODID_ADDRESSABLE(unit, dest_modid)) {
        return BCM_E_PARAM;
    }

    if (dest_port_max < 0 || dest_port_array == NULL ||
            dest_port_count == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_MODPORT_MAP_SELr(unit, ing_port, &rval));
    profile_index = soc_reg_field_get(unit, MODPORT_MAP_SELr, rval,
            MODPORT_MAP_INDEX_UPPERf);
    profile = MODPORT_MAP_PROFILE(unit, profile_index);

    count= 0;
    num_dest_supported = 2;
    for (i = 0; i < num_dest_supported; i++) {
        enable = profile.entry_array[dest_modid].dest_enable[i]; 
        istrunk = profile.entry_array[dest_modid].dest_istrunk[i]; 
        dest = profile.entry_array[dest_modid].dest[i]; 

        if (enable) {
            if (dest_port_max > count) {
            	if (istrunk) {
                    BCM_IF_ERROR_RETURN
                       (bcm_esw_trunk_chip_info_get(unit, &chip_info));
                    tid = dest + chip_info.trunk_fabric_id_min;
                    BCM_GPORT_TRUNK_SET(dest_port_array[count], tid);
                } else {
                    dest_port_array[count] = dest;
                }
            }
            if ((dest_port_max != 0) &&
                (dest_port_max == count)) {
                break;
            }
            count++;
        }
    }
    *dest_port_count = count;

    return *dest_port_count ? BCM_E_NONE : BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcm_td_stk_modport_voq_op
 * Purpose:
 *      Set/get VoQ info.
 * Parameters:
 *      unit - SOC unit #.
 *      ing_port - Ingress port.
 *      dest_modid - Destination module ID.
 *      grp_id - (IN/OUT) VoQ group ID.
 *      op - Set/get.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td_stk_modport_voq_op(int unit, bcm_port_t ing_port,
        bcm_module_t dest_modid, int *grp_id, int op)
{
    int rv = BCM_E_NONE;
    int modid_count, modid_min, modid_max;
    uint32 rval;
    int profile_index, new_profile_index;
    _bcm_td_modport_map_profile_t profile;
    int entry_array_size;
    int voq_cos_valid, voq_group_id;
    int i;

    if (BCM_GPORT_IS_SET(ing_port)) {
        BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, ing_port, &ing_port));
    }
    if (!SOC_PORT_VALID(unit, ing_port)) {
        return BCM_E_PORT;
    }

    modid_count = SOC_MODID_MAX(unit) + 1;
    if (dest_modid == -1 && op == _BCM_STK_PORT_MODPORT_DMVOQ_GRP_OP_SET) {
        modid_min = 0;
        modid_max = modid_count - 1;
    } else {
        if (!SOC_MODID_ADDRESSABLE(unit, dest_modid)) {
            return BCM_E_PARAM;
        }
        modid_min = modid_max = dest_modid;
    }

    if (op <= 0 || op >= _BCM_STK_PORT_MODPORT_DMVOQ_GRP_OP_COUNT) {
        return BCM_E_PARAM;
    }

    soc_mem_lock(unit, MODPORT_MAP_SWm);

    /* Get ingress port's current profile index */
    rv = READ_MODPORT_MAP_SELr(unit, ing_port, &rval);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        return rv;
    }
    profile_index = soc_reg_field_get(unit, MODPORT_MAP_SELr, rval,
            MODPORT_MAP_INDEX_UPPERf);

    /* Get current profile */
    profile.num_entries = MODPORT_MAP_PROFILE(unit, profile_index).num_entries;
    entry_array_size = profile.num_entries * sizeof(_bcm_td_modport_map_entry_t);
    profile.entry_array = sal_alloc(entry_array_size, "modport map profile entry array");
    if (NULL == profile.entry_array) {
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        return BCM_E_MEMORY;
    }
    sal_memcpy(profile.entry_array,
            MODPORT_MAP_PROFILE(unit, profile_index).entry_array,
            entry_array_size);

    if (op == _BCM_STK_PORT_MODPORT_DMVOQ_GRP_OP_SET) {
        voq_cos_valid = (*grp_id >= 0) ? 1: 0;
        voq_group_id = (*grp_id >= 0) ? *grp_id : 0;

        /* Modify profile */
        for (i = modid_min; i <= modid_max; i++) {
            profile.entry_array[i].voq_cos_valid = voq_cos_valid;
            profile.entry_array[i].voq_group_id = voq_group_id;
        }

        /* Add modified profile */
        rv = _bcm_td_modport_map_profile_add(unit, &profile, &new_profile_index);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, MODPORT_MAP_SWm);
            sal_free(profile.entry_array);
            return rv;
        }

        /* Update ingress port's profile index */
        soc_reg_field_set(unit, MODPORT_MAP_SELr, &rval,
                MODPORT_MAP_INDEX_UPPERf, new_profile_index);
        rv = WRITE_MODPORT_MAP_SELr(unit, ing_port, rval);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, MODPORT_MAP_SWm);
            sal_free(profile.entry_array);
            return rv;
        }

        /* Remove old profile */
        rv = _bcm_td_modport_map_profile_delete(unit, profile_index);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, MODPORT_MAP_SWm);
            sal_free(profile.entry_array);
            return rv;
        }
    } else if (op == _BCM_STK_PORT_MODPORT_DMVOQ_GRP_OP_GET) {
        voq_cos_valid = profile.entry_array[dest_modid].voq_cos_valid;
        voq_group_id = profile.entry_array[dest_modid].voq_group_id;
        *grp_id = (voq_cos_valid) ? voq_group_id: -1;
    }

    soc_mem_unlock(unit, MODPORT_MAP_SWm);
    sal_free(profile.entry_array);
    return rv;
}

/*
 * Function:
 *      bcm_td_stk_modport_map_update
 * Purpose:
 *      Update modport map profiles due to changes in Higig trunk membership.
 * Parameters:
 *      unit - SOC unit #.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td_stk_modport_map_update(int unit)
{
    int i;

    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
        if (MODPORT_MAP_PROFILE(unit, i).ref_count == 0) {
            continue;
        }
        BCM_IF_ERROR_RETURN(_bcm_td_modport_map_profile_hw_write(unit, i,
                    &MODPORT_MAP_PROFILE(unit, i)));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td_stk_trunk_override_ucast_set
 * Purpose:
 *      Set/clear HiGig Trunk Override in modport map profile.
 * Parameters:
 *      unit - SOC unit
 *      ing_port - Ingress Port
 *      hgtid - Higig Trunk ID
 *      modid - Module ID  
 *      enable - HighGig Trunk Override set/clear flag.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td_stk_trunk_override_ucast_set(int unit, bcm_port_t ing_port,
                               bcm_trunk_t hgtid, int modid, int enable)
{
    int rv = BCM_E_NONE;
    uint32 rval;
    int profile_index, new_profile_index;
    _bcm_td_modport_map_profile_t profile;
    int entry_array_size;

    if (BCM_GPORT_IS_SET(ing_port)) {
        BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, ing_port, &ing_port));
    }
    if (!SOC_PORT_VALID(unit, ing_port)) {
        return BCM_E_PORT;
    }

    if (!SOC_MODID_ADDRESSABLE(unit, modid)) {
        return BCM_E_PARAM;
    }

    soc_mem_lock(unit, MODPORT_MAP_SWm);

    /* Get ingress port's current profile index */
    rv = READ_MODPORT_MAP_SELr(unit, ing_port, &rval);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        return rv;
    }
    profile_index = soc_reg_field_get(unit, MODPORT_MAP_SELr, rval,
            MODPORT_MAP_INDEX_UPPERf);

    /* Get current profile */
    profile.num_entries = MODPORT_MAP_PROFILE(unit, profile_index).num_entries;
    entry_array_size = profile.num_entries * sizeof(_bcm_td_modport_map_entry_t);
    profile.entry_array = sal_alloc(entry_array_size, "modport map profile entry array");
    if (NULL == profile.entry_array) {
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        return BCM_E_MEMORY;
    }
    sal_memcpy(profile.entry_array,
            MODPORT_MAP_PROFILE(unit, profile_index).entry_array,
            entry_array_size);

    /* Modify profile */
    if (enable) {
        SHR_BITSET(profile.entry_array[modid].higig_trunk_override, hgtid);
    } else {
        SHR_BITCLR(profile.entry_array[modid].higig_trunk_override, hgtid);
    }

    /* Add modified profile */
    rv = _bcm_td_modport_map_profile_add(unit, &profile, &new_profile_index);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        sal_free(profile.entry_array);
        return rv;
    }

    /* Update ingress port's profile index */
    soc_reg_field_set(unit, MODPORT_MAP_SELr, &rval,
            MODPORT_MAP_INDEX_UPPERf, new_profile_index);
    rv = WRITE_MODPORT_MAP_SELr(unit, ing_port, rval);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        sal_free(profile.entry_array);
        return rv;
    }

    /* Remove old profile */
    rv = _bcm_td_modport_map_profile_delete(unit, profile_index);
    soc_mem_unlock(unit, MODPORT_MAP_SWm);
    sal_free(profile.entry_array);
    return rv;
}

/*
 * Function:
 *      bcm_td_stk_trunk_override_ucast_get
 * Purpose:
 *      Get HiGig Trunk Override from modport map profile.
 * Parameters:
 *      unit - SOC unit
 *      ing_port - Ingress Port
 *      hgtid - Trunk ID
 *      modid - Module ID  
 *      enable - HighGig Trunk Override flag.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td_stk_trunk_override_ucast_get(int unit, bcm_port_t ing_port, 
                                 bcm_trunk_t hgtid, int modid, int *enable) 
{
    int rv = BCM_E_NONE;
    uint32 rval;
    int profile_index;
    _bcm_td_modport_map_profile_t profile;
    int entry_array_size;

    if (BCM_GPORT_IS_SET(ing_port)) {
        BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, ing_port, &ing_port));
    }
    if (!SOC_PORT_VALID(unit, ing_port)) {
        return BCM_E_PORT;
    }

    if (!SOC_MODID_ADDRESSABLE(unit, modid)) {
        return BCM_E_PARAM;
    }

    soc_mem_lock(unit, MODPORT_MAP_SWm);

    /* Get ingress port's current profile index */
    rv = READ_MODPORT_MAP_SELr(unit, ing_port, &rval);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        return rv;
    }
    profile_index = soc_reg_field_get(unit, MODPORT_MAP_SELr, rval,
            MODPORT_MAP_INDEX_UPPERf);

    /* Get current profile */
    profile.num_entries = MODPORT_MAP_PROFILE(unit, profile_index).num_entries;
    entry_array_size = profile.num_entries * sizeof(_bcm_td_modport_map_entry_t);
    profile.entry_array = sal_alloc(entry_array_size, "modport map profile entry array");
    if (NULL == profile.entry_array) {
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        return BCM_E_MEMORY;
    }
    sal_memcpy(profile.entry_array,
            MODPORT_MAP_PROFILE(unit, profile_index).entry_array,
            entry_array_size);

    /* Modify profile */
    *enable = SHR_BITGET(profile.entry_array[modid].higig_trunk_override, 
                        hgtid) ? 1 : 0;

    soc_mem_unlock(unit, MODPORT_MAP_SWm);
    sal_free(profile.entry_array);
    return rv;
}

/*
 * Function:
 *      bcm_td_modport_map_mode_set
 * Purpose:
 *      Set modport map mode.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      type - (IN) Configuration parameter type.
 *      arg  - (IN) Configuration paramter value.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td_modport_map_mode_set(int unit, bcm_switch_control_t type, int arg)
{
    switch (type) {
        case bcmSwitchFabricTrunkAutoIncludeDisable:
            MODPORT_MAP_INFO(unit).auto_include_disable = arg;
            break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td_modport_map_mode_get
 * Purpose:
 *      Get modport map mode.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      type - (IN) Configuration parameter type.
 *      arg  - (OUT) Configuration paramter value.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td_modport_map_mode_get(int unit, bcm_switch_control_t type, int *arg)
{
    switch (type) {
        case bcmSwitchFabricTrunkAutoIncludeDisable:
            *arg = MODPORT_MAP_INFO(unit).auto_include_disable;
            break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td_stk_modport_map_port_attach
 * Purpose:
 *      Initialize modport map profiles ptr on new port that added
 *      during flexport operation.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcm_td_stk_modport_map_port_attach(int unit, bcm_port_t port)
{
    _bcm_td_modport_map_profile_t profile;
    int profile_index;
    int rv;

    /* Get the profile_index of default profile */
    profile.num_entries = SOC_MODID_MAX(unit) + 1;
    profile.entry_array = sal_alloc(sizeof(_bcm_td_modport_map_entry_t) *
        profile.num_entries, "profile entry array");
    if (NULL == profile.entry_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(profile.entry_array, 0, sizeof(_bcm_td_modport_map_entry_t) *
            profile.num_entries);
    rv = _bcm_td_modport_map_profile_add(unit, &profile, &profile_index); 
    sal_free(profile.entry_array);
    if (BCM_FAILURE(rv)) {
       return rv;
    }

    /* Set the profile ptr to the port */
    BCM_IF_ERROR_RETURN(WRITE_MODPORT_MAP_SELr(unit, port, profile_index));
    MODPORT_MAP_PROFILE(unit, profile_index).ref_count++;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td_stk_modport_map_port_detach
 * Purpose:
 *      Detach port from modport map profiles.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcm_td_stk_modport_map_port_detach(int unit, bcm_port_t port)
{
    int profile_index;
    uint32 val;

    SOC_IF_ERROR_RETURN(READ_MODPORT_MAP_SELr(unit, port, &val));
    profile_index = soc_reg_field_get(unit, MODPORT_MAP_SELr, val,
                                      MODPORT_MAP_INDEX_UPPERf);

    MODPORT_MAP_PROFILE(unit, profile_index).ref_count--;

    /* Clear the profile ptr */
    BCM_IF_ERROR_RETURN(WRITE_MODPORT_MAP_SELr(unit, port, 0));

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *     bcm_td_modport_map_scache_size_get
 * Purpose:
 *     Get scache size needed to store modport map profile state.
 * Parameters:
 *     unit - Device unit number
 *     size - (OUT) Scache size needed.
 * Returns:
 *     None
 */
int
bcm_td_modport_map_scache_size_get(int unit, int *size)
{

#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        /* For each MODPORT_MAP_SWm entry, 8 bytes of internal
         * state need to be stored:
         * - First 2 bytes: bit 14 = dest_istrunk[0], bits 13-0 = dest[0].
         * - Next 2 bytes: bit 14 = dest_istrunk[1], bits 13-0 = dest[1].
         * - Next 4 bytes: 32-bit higig_trunk_override.
         */
        *size = (2 * sizeof(uint16) + sizeof(SHR_BITDCL)) 
                                * soc_mem_index_count(unit, MODPORT_MAP_SWm);
    } else
#endif /* BCM_KATANA2_SUPPORT */
    {
        /* For each MODPORT_MAP_SWm entry, 6 bytes of internal
         * state need to be stored:
         * - First byte: bit 7 = dest_istrunk[0], bits 6-0 = dest[0].
         * - Second byte: bit 7 = dest_istrunk[1], bits 6-0 = dest[1].
         * - Next 4 bytes: 32-bit higig_trunk_override.
         */
        *size = (2 * sizeof(uint8) + sizeof(SHR_BITDCL)) 
                                * soc_mem_index_count(unit, MODPORT_MAP_SWm);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_modport_map_sync
 * Purpose:
 *     Store modport map profile state to scache.
 * Parameters:
 *     unit - Device unit number
 *     scache_ptr - Scache pointer.
 * Returns:
 *     None
 */
int
bcm_td_modport_map_sync(int unit, uint8 **scache_ptr)
{
    int i, j, k;
    _bcm_td_modport_map_profile_t profile;
    int istrunk, dest;
    SHR_BITDCL higig_trunk_override;
    uint8 dest_byte;
#if defined(BCM_KATANA2_SUPPORT)
    uint16 kt2_dest_byte;
    int isenable;
#endif

    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
        profile = MODPORT_MAP_PROFILE(unit, i);
        for (j = 0; j < profile.num_entries; j++) {
            for (k = 0; k < 2; k++) {
                istrunk = profile.entry_array[j].dest_istrunk[k]; 
                dest = profile.entry_array[j].dest[k];
#if defined(BCM_KATANA2_SUPPORT)
                if (SOC_IS_KATANA2(unit)) {
                    /* DEST0 13:0
                     * if ISTRUNK0 == 0 then
                     *  13:8: Egress Physical Port; 7:0: Egress PP_PORT
                     * else if ISTRUNK0 == 1 then
                     *  3:0: Egress HiGig Trunk Group ID.
                     * note: KT2 physical port to pp_port is 1:1 mapped.
                     */
                    isenable = profile.entry_array[j].dest_enable[k];
                    kt2_dest_byte = ((isenable & 0x1) << 15) |
                                    ((istrunk & 0x1) << 14) |
                                    ((dest & 0x3f) << 8) | (dest & 0xff);
                    sal_memcpy(*scache_ptr, &kt2_dest_byte,
                               sizeof(kt2_dest_byte));
                    *scache_ptr += sizeof(kt2_dest_byte);
                } else
#endif /* BCM_KATANA2_SUPPORT */
                {
                    dest_byte = ((istrunk & 0x1) << 7) | (dest & 0x7f);
                    sal_memcpy(*scache_ptr, &dest_byte, sizeof(dest_byte));
                    *scache_ptr += sizeof(dest_byte);
                }
            }

            higig_trunk_override = profile.entry_array[j].higig_trunk_override[0];
            sal_memcpy(*scache_ptr, &higig_trunk_override,
                    sizeof(higig_trunk_override));
            *scache_ptr += sizeof(higig_trunk_override);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_modport_map_reinit
 * Purpose:
 *     Retrieve modport map profile state from hardware and scache.
 * Parameters:
 *     unit - Device unit number
 *     scache_ptr - Scache pointer.
 * Returns:
 *     None
 */
int
bcm_td_modport_map_reinit(int unit, uint8 **scache_ptr)
{
    int rv = BCM_E_NONE;
    bcm_port_t ing_port;
    uint32 val;
    int profile_index;
    int i, j, k;
    _bcm_td_modport_map_profile_t profile;
    int profile_buf_size;
    uint8 *profile_buf = NULL;
    int index_min, index_max;
    modport_map_sw_entry_t *modport_map_sw_entry;
    soc_field_t enable_field[2]  = {ENABLE0f, ENABLE1f};
    soc_field_t istrunk_field[2] = {ISTRUNK0f, ISTRUNK1f};
    soc_field_t dest_field[2]    = {DEST0f, DEST1f};
#if defined(BCM_KATANA2_SUPPORT)
    uint16 kt2_dest_byte;
#endif
    uint8 dest_byte;

    bcm_gport_t gport;
    bcm_trunk_t tid;
    bcm_trunk_chip_info_t chip_info;
    int hg_tid;
    bcm_pbmp_t all_pbmp;

    BCM_PBMP_CLEAR(all_pbmp);
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &all_pbmp);
    }
    if (SOC_IS_KATANA2(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &all_pbmp));
    }
#endif


    /* Recover each profile's reference count */
    PBMP_ITER(all_pbmp, ing_port) {
        SOC_IF_ERROR_RETURN(READ_MODPORT_MAP_SELr(unit, ing_port, &val));
        profile_index = soc_reg_field_get(unit, MODPORT_MAP_SELr, val,
                                      MODPORT_MAP_INDEX_UPPERf);
        MODPORT_MAP_PROFILE(unit, profile_index).ref_count++;
    }

    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
        profile = MODPORT_MAP_PROFILE(unit, i);
        if (profile.ref_count == 0) {
            /* Skip the scache region containing the state of this profile */
            if (*scache_ptr != NULL) {
#if defined(BCM_KATANA2_SUPPORT)
                if (SOC_IS_KATANA2(unit)) {
                    *scache_ptr += profile.num_entries *
                        (2 * sizeof(kt2_dest_byte) + sizeof(SHR_BITDCL)); 
                } else
#endif
                {
                    *scache_ptr += profile.num_entries *
                        (2 * sizeof(dest_byte) + sizeof(SHR_BITDCL)); 
                }
            }
            continue;
        }

        /* Read profile from MODPOR_MAP_SWm */

        profile_buf_size = profile.num_entries *
            soc_mem_entry_words(unit, MODPORT_MAP_SWm) * 4;
        profile_buf = soc_cm_salloc(unit, profile_buf_size,
                "modport map buffer"); 
        if (NULL == profile_buf) {
            return BCM_E_MEMORY;
        }

        index_min = i * profile.num_entries;
        index_max = index_min + profile.num_entries - 1;
        rv = soc_mem_read_range(unit, MODPORT_MAP_SWm, MEM_BLOCK_ANY,
                index_min, index_max, profile_buf);
        if (SOC_FAILURE(rv)) {
            soc_cm_sfree(unit, profile_buf);
            return rv;
        }

        /* Recover profile state */

        for (j = 0; j < profile.num_entries; j++) {
            modport_map_sw_entry = soc_mem_table_idx_to_pointer(unit,
                    MODPORT_MAP_SWm, modport_map_sw_entry_t *, profile_buf, j);

            for (k = 0; k < 2; k++) {
                profile.entry_array[j].dest_enable[k] =
                    soc_MODPORT_MAP_SWm_field32_get(unit, modport_map_sw_entry,
                            enable_field[k]);
                if (*scache_ptr != NULL) {
#if defined(BCM_KATANA2_SUPPORT)
                    if (SOC_IS_KATANA2(unit)) {
                        sal_memcpy(&kt2_dest_byte, *scache_ptr,
                                   sizeof(kt2_dest_byte));
                        *scache_ptr += sizeof(kt2_dest_byte);

                        profile.entry_array[j].dest_istrunk[k] =
                            (kt2_dest_byte >> 14) & 0x1; 
                        profile.entry_array[j].dest[k] = kt2_dest_byte & 0xff;
                    } else
#endif /* BCM_KATANA2_SUPPORT */
                    {
                        sal_memcpy(&dest_byte, *scache_ptr, sizeof(dest_byte));
                        *scache_ptr += sizeof(dest_byte);
                        profile.entry_array[j].dest_istrunk[k] =
                            (dest_byte >> 7) & 0x1; 
                        profile.entry_array[j].dest[k] = dest_byte & 0x7f;
                    }
                } else {
                    profile.entry_array[j].dest_istrunk[k] =
                        soc_MODPORT_MAP_SWm_field32_get(unit,
                            modport_map_sw_entry, istrunk_field[k]);
#if defined(BCM_KATANA2_SUPPORT)
                    if (SOC_IS_KATANA2(unit)) {
                        profile.entry_array[j].dest[k] =
                            (soc_MODPORT_MAP_SWm_field32_get(unit,
                                modport_map_sw_entry, dest_field[k])) & 0xff;
                    } else
#endif /* BCM_KATANA2_SUPPORT */
                    {
                        profile.entry_array[j].dest[k] =
                            soc_MODPORT_MAP_SWm_field32_get(unit,
                                modport_map_sw_entry, dest_field[k]);
                    }

                    /* Infer Higig trunk override info */
                    if (profile.entry_array[j].dest_enable[k] &&
                            !profile.entry_array[j].dest_istrunk[k]) {
                        SOC_GPORT_LOCAL_SET(gport, profile.entry_array[j].dest[k]);
                        rv = bcm_esw_trunk_find(unit, 0, gport, &tid);
                        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                            soc_cm_sfree(unit, profile_buf);
                            return rv;
                        }
                        if (BCM_SUCCESS(rv)) {
                            /* Port belongs to a Higig trunk group. */
                            rv = bcm_esw_trunk_chip_info_get(unit, &chip_info);
                            if (BCM_FAILURE(rv)) {
                                soc_cm_sfree(unit, profile_buf);
                                return rv;
                            }
                            if (tid >= chip_info.trunk_fabric_id_min &&
                                    tid <= chip_info.trunk_fabric_id_max) {
                                hg_tid = tid - chip_info.trunk_fabric_id_min;
                                SHR_BITSET(
                                    profile.entry_array[j].higig_trunk_override,
                                    hg_tid);
                            } else {
                                soc_cm_sfree(unit, profile_buf);
                                return BCM_E_INTERNAL;
                            }
                        } 
                    }
                }
            }

            if (*scache_ptr != NULL) {
                sal_memcpy(profile.entry_array[j].higig_trunk_override, 
                                            *scache_ptr, sizeof(SHR_BITDCL));
                *scache_ptr += sizeof(SHR_BITDCL);
            }

            if (soc_mem_field_valid(unit, MODPORT_MAP_SWm, VOQ_COS_VALIDf)) {
                profile.entry_array[j].voq_cos_valid =
                    soc_MODPORT_MAP_SWm_field32_get(unit, modport_map_sw_entry,
                            VOQ_COS_VALIDf);
            }
            if (soc_mem_field_valid(unit, MODPORT_MAP_SWm, VOQ_GRP_IDf)) {
                profile.entry_array[j].voq_group_id =
                    soc_MODPORT_MAP_SWm_field32_get(unit, modport_map_sw_entry,
                            VOQ_GRP_IDf);
            }
        }

        soc_cm_sfree(unit, profile_buf);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_modport_map_enable_scache_size_get
 * Purpose:
 *     Get scache size needed to store modport map profile enable field.
 * Parameters:
 *     unit - Device unit number
 *     size - (OUT) Scache size needed.
 * Returns:
 *     None
 */
int
bcm_td_modport_map_enable_scache_size_get(int unit, int *size)
{

    /* For each MODPORT_MAP_SWm entry, 1 bytes of internal
     * state need to be stored:
     * One byte - bit 1 = enable[1], bit 0 = enable[0].
     */
    *size = (1 * sizeof(uint8)) *
            soc_mem_index_count(unit, MODPORT_MAP_SWm);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_modport_map_enable_sync
 * Purpose:
 *     Store modport map profile enable field to scache.
 * Parameters:
 *     unit - Device unit number
 *     scache_ptr - Scache pointer.
 * Returns:
 *     None
 */
int
bcm_td_modport_map_enable_sync(int unit, uint8 **scache_ptr)
{
    int i, j;
    _bcm_td_modport_map_profile_t profile;
    uint8 enable_byte;

    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
        profile = MODPORT_MAP_PROFILE(unit, i);
        for (j = 0; j < profile.num_entries; j++) {
            enable_byte = ((profile.entry_array[j].dest_enable[1] & 0x1) << 1) |
                            (profile.entry_array[j].dest_enable[0] & 0x1);
            sal_memcpy(*scache_ptr, &enable_byte, sizeof(enable_byte));
            *scache_ptr += sizeof(enable_byte);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_modport_map_enable_reinit
 * Purpose:
 *     Retrieve modport map enable field from hardware and scache.
 * Parameters:
 *     unit - Device unit number
 *     scache_ptr - Scache pointer.
 * Returns:
 *     None
 */
int
bcm_td_modport_map_enable_reinit(int unit, uint8 **scache_ptr)
{
    int i, j;
    _bcm_td_modport_map_profile_t profile;
    uint8 enable_byte;

    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
        profile = MODPORT_MAP_PROFILE(unit, i);
        for (j = 0; j < profile.num_entries; j++) {
            sal_memcpy(&enable_byte, *scache_ptr, sizeof(enable_byte));
            *scache_ptr += sizeof(enable_byte);
            profile.entry_array[j].dest_enable[0] = (enable_byte & 0x1);
            profile.entry_array[j].dest_enable[1] = ((enable_byte >> 1) & 0x1);
        }
    }

    return BCM_E_NONE;
}

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
/*
 * Function:
 *     bcm_td_linkflap_trunk_map_scache_size_get
 * Purpose:
 *     Get scache size needed to store port to trunk mapping
 * Parameters:
 *     unit - Device unit number
 *     size - (OUT) Scache size needed.
 * Returns:
 *     None
 */
int
bcm_td_linkflap_trunk_map_scache_size_get(int unit, int *size)
{
    *size = (sizeof(int)) * SOC_MAX_NUM_PORTS;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_linkflap_trunk_map_sync
 * Purpose:
 *     Store linkflap port to trunk mapping.
 * Parameters:
 *     unit - Device unit number
 *     scache_ptr - Scache pointer.
 * Returns:
 *     None
 */
int
bcm_td_linkflap_trunk_map_sync(int unit, uint8 **scache_ptr)
{
    bcm_port_t p;
    int trunk;
    PBMP_PORT_ITER(unit, p) {
        trunk = _BCM_PORT_TRUNK_LINKFLAP_GET(unit, p);
        sal_memcpy(*scache_ptr, &trunk, sizeof(int));
        *scache_ptr += sizeof(int);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_linkflap_trunk_map_reinit
 * Purpose:
 *     Retrieve linkflap port to trunk mapping
 * Parameters:
 *     unit - Device unit number
 *     scache_ptr - Scache pointer.
 * Returns:
 *     None
 */
int
bcm_td_linkflap_trunk_map_reinit(int unit, uint8 **scache_ptr)
{
    bcm_port_t p;
    int trunk;
    PBMP_PORT_ITER(unit, p) {
        sal_memcpy(&trunk, *scache_ptr, sizeof(int));
        *scache_ptr += sizeof(int);
        _BCM_PORT_TRUNK_LINKFLAP_SET(unit, p, trunk);
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *     bcm_td_stk_trunk_override_hi_scache_size_get
 * Purpose:
 *     Get scache size needed to store modport map profile state.
 * Parameters:
 *     unit - Device unit number
 *     size - (OUT) Scache size needed.
 * Returns:
 *     None
 */
int
bcm_td_stk_trunk_override_hi_scache_size_get(int unit, int *size)
{
    uint32 higig_trunk_override_hi_size;

    higig_trunk_override_hi_size = SHR_BITALLOCSIZE(_BCM_STK_HGT_GROUP_MAX)
                                    - sizeof(SHR_BITDCL);
    *size = higig_trunk_override_hi_size * 
                                    soc_mem_index_count(unit, MODPORT_MAP_SWm);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_stk_trunk_override_hi_sync
 * Purpose:
 *     Store stk trunk override high bitmap to scache.
 * Parameters:
 *     unit - Device unit number
 *     scache_ptr - Scache pointer.
 * Returns:
 *     None
 */
int
bcm_td_stk_trunk_override_hi_sync(int unit, uint8 **scache_ptr)
{
    int i, j;
    uint32 higig_trunk_override_hi_size;
    _bcm_td_modport_map_profile_t profile;
    SHR_BITDCL *higig_trunk_override_hi;

    higig_trunk_override_hi_size = SHR_BITALLOCSIZE(_BCM_STK_HGT_GROUP_MAX)
                                    - sizeof(SHR_BITDCL);
    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
        profile = MODPORT_MAP_PROFILE(unit, i);
        for (j = 0; j < profile.num_entries; j++) {
            higig_trunk_override_hi = 
                                &profile.entry_array[j].higig_trunk_override[1];
            sal_memcpy(*scache_ptr, 
                        higig_trunk_override_hi, higig_trunk_override_hi_size);
            *scache_ptr += higig_trunk_override_hi_size;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_stk_trunk_override_hi_reinit
 * Purpose:
 *     Retrieve stk trunk override high bitmap scache.
 * Parameters:
 *     unit - Device unit number
 *     scache_ptr - Scache pointer.
 * Returns:
 *     None
 */
int
bcm_td_stk_trunk_override_hi_reinit(int unit, uint8 **scache_ptr)
{
    int i, j;
    uint32 higig_trunk_override_hi_size;
    _bcm_td_modport_map_profile_t profile;

    higig_trunk_override_hi_size = SHR_BITALLOCSIZE(_BCM_STK_HGT_GROUP_MAX)
                                    - sizeof(SHR_BITDCL);
    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
        profile = MODPORT_MAP_PROFILE(unit, i);
        if (profile.ref_count == 0) {
            *scache_ptr += profile.num_entries * higig_trunk_override_hi_size; 
            continue;
        }

        for (j = 0; j < profile.num_entries; j++) {
            sal_memcpy(&profile.entry_array[j].higig_trunk_override[1], 
                                *scache_ptr, higig_trunk_override_hi_size);
            *scache_ptr += higig_trunk_override_hi_size;
        }
    }

    return BCM_E_NONE;
}


#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     bcm_td_modport_map_sw_dump
 * Purpose:
 *     Displays modport map software structure information.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_td_modport_map_sw_dump(int unit)
{
    int num_entries;
    int i, j, word;
    int entry_index;
    _bcm_td_modport_map_profile_t profile;
    int enable, istrunk, dest;

    LOG_CLI((BSL_META_U(unit,
                        "  Stack Modport Higig Trunk Auto Include Disable = %d\n"),
             MODPORT_MAP_INFO(unit).auto_include_disable));

    LOG_CLI((BSL_META_U(unit,
                        "  Stack Modport Profile\n")));

    num_entries = 0;
    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
        num_entries += MODPORT_MAP_PROFILE(unit, i).num_entries;
    }
    LOG_CLI((BSL_META_U(unit,
                        "      Number of entries: %d\n"), num_entries));

    LOG_CLI((BSL_META_U(unit,
                        "      Index RefCount EntriesPerSet - HIGIG PORT or TRUNK\n")));
    entry_index = 0;
    for (i = 0; i < MODPORT_MAP_INFO(unit).num_profiles; i++) {
        profile = MODPORT_MAP_PROFILE(unit, i);
        if (profile.ref_count == 0) {
            entry_index += profile.num_entries;
            continue;
        }

        for (j = 0; j < profile.num_entries; j++) {
            LOG_CLI((BSL_META_U(unit,
                                "     %5d %8d %13d    "),
                     entry_index++, profile.ref_count, profile.num_entries));

            enable = profile.entry_array[j].dest_enable[0]; 
            istrunk = profile.entry_array[j].dest_istrunk[0]; 
            dest = profile.entry_array[j].dest[0]; 
            LOG_CLI((BSL_META_U(unit,
                                "enable0=%d, istrunk0=%d, dest0=%d, "),
                     enable, istrunk, dest));

            enable = profile.entry_array[j].dest_enable[1]; 
            istrunk = profile.entry_array[j].dest_istrunk[1]; 
            dest = profile.entry_array[j].dest[1]; 
            LOG_CLI((BSL_META_U(unit,
                                "enable1=%d, istrunk1=%d, dest1=%d, "),
                     enable, istrunk, dest));

            LOG_CLI((BSL_META_U(unit,
                                "higig_trunk_override=0x")));
            for (word = _SHR_BITDCLSIZE(_BCM_STK_HGT_GROUP_MAX) - 1;
                word >= 0; word--) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%08x%s"),
                             profile.entry_array[j].higig_trunk_override[word],
                             word ? "_" : "\n"));
            }
        }
    }
}

#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif /* BCM_TRIDENT_SUPPORT */
