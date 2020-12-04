/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_TRIDENT_SUPPORT)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/hash.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw_dispatch.h>
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/flow.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/subport.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */
#include <bcm_int/common/lock.h>

STATIC soc_profile_mem_t *_bcm_td_egr_mask_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_td_sys_cfg_profile[BCM_MAX_NUM_UNITS];

int
bcm_td_port_reinit(int unit)
{
    soc_profile_mem_t *profile;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 profile_index;
    bcm_module_t modid;
    bcm_port_t port;
    system_config_table_entry_t *sys_cfg_entry;
    int tpid_enable, tpid_index;
    int is_local;

#ifdef BCM_TOMAHAWK3_SUPPORT
    /* Tomahawk 3 doesn't support stacking, so it's only going
     * to have its local ports
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        profile = _bcm_td_egr_mask_profile[unit];
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, profile, 0,
                                 160));
    } else
#endif
    {
        /* EGR_MASK profile */
        if (SOC_MEM_IS_VALID(unit, EGR_MASK_MODBASEm)) {
            profile = _bcm_td_egr_mask_profile[unit];
            for (modid = 0; modid <= SOC_MODID_MAX(unit); modid++) {
                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, EGR_MASK_MODBASEm, MEM_BLOCK_ALL, modid,
                                  entry));
                profile_index = soc_mem_field32_get(unit, EGR_MASK_MODBASEm, entry,
                                                    BASEf);
                SOC_IF_ERROR_RETURN
                    (soc_profile_mem_reference(unit, profile, profile_index,
                                               SOC_PORT_ADDR_MAX(unit) + 1));
            }
        }
    }

    /* SYSTEM_CONFIG_TABLE profile */
    if (SOC_MEM_IS_VALID(unit, SYSTEM_CONFIG_TABLE_MODBASEm)) {
        profile = _bcm_td_sys_cfg_profile[unit];
        for (modid = 0; modid <= SOC_MODID_MAX(unit); modid++) {
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, SYSTEM_CONFIG_TABLE_MODBASEm, MEM_BLOCK_ALL,
                              modid, entry));
            profile_index = soc_mem_field32_get(unit, SYSTEM_CONFIG_TABLE_MODBASEm,
                                                entry, BASEf);
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_reference(unit, profile, profile_index,
                                           SOC_PORT_ADDR_MAX(unit) + 1));

            BCM_IF_ERROR_RETURN
                (_bcm_esw_modid_is_local(unit, modid, &is_local));
            if (is_local) {
                continue;
            }

            /* Increment outer tpid reference count for non-local ports.
             * The local ports' outer tpid reference count is taken care
             * of by _bcm_fb2_outer_tpid_init.
             */
            for (port = 0; port <= SOC_PORT_ADDR_MAX(unit); port++) {
                sys_cfg_entry = SOC_PROFILE_MEM_ENTRY(unit, profile,
                        system_config_table_entry_t *, profile_index + port);
                tpid_enable = soc_SYSTEM_CONFIG_TABLEm_field32_get(unit,
                        sys_cfg_entry, OUTER_TPID_ENABLEf);
                for (tpid_index = 0; tpid_index < 4; tpid_index++) {
                    if (tpid_enable & (1 << tpid_index)) {
                        BCM_IF_ERROR_RETURN
                            (_bcm_fb2_outer_tpid_tab_ref_count_add(unit,
                                                             tpid_index, 1));
                    }
                }
            }
        }
    }

    return BCM_E_NONE;
}

union {
#ifdef BCM_TOMAHAWK_SUPPORT
    /* Note: This code applies to TH as well so the numbers are 
     *       bumped up from 128 to 256
     */
    egr_mask_entry_t egr_mask[256];
    system_config_table_entry_t sys_cfg[256];
#else
    egr_mask_entry_t egr_mask[128];
    system_config_table_entry_t sys_cfg[128];
#endif
    uint32 w[1];
} entry;

int
bcm_td_port_init(int unit)
{
    soc_mem_t mem;
    int entry_words;
    void *entries[1];
    int modid, port, index;
    uint32 profile_index;
    uint16 tpid;
    bcm_pbmp_t bmp;
#if defined(BCM_KATANA2_SUPPORT)
    int speed;
    soc_pbmp_t my_pbmp_xport_xe;
#endif

    /* Create profile for EGR_MASK table */
    if (_bcm_td_egr_mask_profile[unit] == NULL) {
        _bcm_td_egr_mask_profile[unit] =
            sal_alloc(sizeof(soc_profile_mem_t), "EGR_MASK profile");
        if (_bcm_td_egr_mask_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_td_egr_mask_profile[unit]);
    }
    mem = EGR_MASKm;
    entry_words = sizeof(egr_mask_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                _bcm_td_egr_mask_profile[unit]));

    /* Create profile for SYSTEM_CONFIG_TABLE table */
    if (SOC_MEM_IS_VALID(unit, SYSTEM_CONFIG_TABLEm)) {
        if (_bcm_td_sys_cfg_profile[unit] == NULL) {
            _bcm_td_sys_cfg_profile[unit] =
                sal_alloc(sizeof(soc_profile_mem_t),
                          "SYSTEM_CONFIG_TABLE profile");
            if (_bcm_td_sys_cfg_profile[unit] == NULL) {
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(_bcm_td_sys_cfg_profile[unit]);
        }
        mem = SYSTEM_CONFIG_TABLEm;
        entry_words = sizeof(system_config_table_entry_t) / sizeof(uint32);
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                    _bcm_td_sys_cfg_profile[unit]));
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        return bcm_td_port_reinit(unit);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    entries[0] = &entry;

    /* Add default entries for EGR_MASK profile */
    sal_memset(entry.egr_mask, 0, sizeof(entry.egr_mask));

    /* TH3 doesn't support stacking, so egr_mask memory only covers its ports */
    if (SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, _bcm_td_egr_mask_profile[unit], entries,
                                 160, &profile_index));
    } else {
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, _bcm_td_egr_mask_profile[unit], entries,
                                 SOC_PORT_ADDR_MAX(unit) + 1, &profile_index));
        for (modid = 1; modid <= SOC_MODID_MAX(unit); modid++) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_reference(unit, _bcm_td_egr_mask_profile[unit],
                                       profile_index,
                                       SOC_PORT_ADDR_MAX(unit) + 1));
    }

    }
    /* EGR_MASK_MODBASE should be 0 for all modid which should match with the
     * allocated default profile index */

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_port_init(unit);
    }
#endif

    if (SOC_MEM_IS_VALID(unit, SYSTEM_CONFIG_TABLEm)) {
        /* Add default entries for SYSTEM_CONFIG_TABLE profile */
        tpid = _bcm_fb2_outer_tpid_default_get(unit);
        BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_lkup(unit, tpid, &index));

        sal_memset(entry.sys_cfg, 0, sizeof(entry.sys_cfg));
        for (port = 0; port <= SOC_PORT_ADDR_MAX(unit); port++) {
            soc_mem_field32_set(unit, SYSTEM_CONFIG_TABLEm, &entry.sys_cfg[port],
                    OUTER_TPID_ENABLEf, 1 << index);

            /* INNER_TPID_ENABLE => {
             * MAXBIT => 4, MINBIT => 4,
             * DESC => "Indicates if INNER TPID comparison is allowed.
             * This is needed for Transparent Bridging."
             * } */

            if (soc_feature(unit, soc_feature_inner_tpid_enable)) {
                soc_mem_field32_set(unit, SYSTEM_CONFIG_TABLEm,
                        &entry.sys_cfg[port], INNER_TPID_ENABLEf, 1);
            }
        }
    
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, _bcm_td_sys_cfg_profile[unit], entries,
                                 SOC_PORT_ADDR_MAX(unit) + 1, &profile_index));
        for (modid = 1; modid <= SOC_MODID_MAX(unit); modid++) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_reference(unit, _bcm_td_sys_cfg_profile[unit],
                                           profile_index,
                                           SOC_PORT_ADDR_MAX(unit) + 1));
        }
        /* SYSTEM_CONFIG_TABLE_MODBASE should be 0 for all modid which should
         * match with the allocated default profile index */

        /* Update the TPID index reference count for all (module, port) except
         * for local (module, port). The TPID index reference count for
         * local (module, port) will be updated during port module init,
         * when default outer TPID will be set for each local port.
         */
        BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_tab_ref_count_add(unit, index,
                    (SOC_MODID_MAX(unit) + 1 - NUM_MODID(unit)) *
                    (SOC_PORT_ADDR_MAX(unit) + 1)));

    }
    if (SOC_IS_KATANA(unit) || SOC_IS_TRIUMPH3(unit)) {
        SOC_PBMP_CLEAR(bmp);
        SOC_PBMP_ASSIGN(bmp, PBMP_ALL(unit));

        PBMP_ITER(bmp, port) {
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, PORT_TABm, port, TRUST_DSCP_PTRf,
                                        port));
            if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, STORM_CONTROL_PTRf)) {
                BCM_IF_ERROR_RETURN
                    (soc_mem_field32_modify(unit, PORT_TABm, port,
                            STORM_CONTROL_PTRf, port));
            }
        }
    }

#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit) && 
        SOC_MEM_FIELD_VALID(unit, PORT_TABm, STORM_CONTROL_PTRf)) {
        
        SOC_PBMP_CLEAR(bmp);
        for (port = 0; port<(SOC_INFO(unit).lb_port) + 1; port++) {
            SOC_PBMP_PORT_ADD(bmp,port);
        }
        _bcm_kt2_subport_pbmp_update(unit, &bmp);

        PBMP_ITER(bmp, port) {
            BCM_IF_ERROR_RETURN(
                soc_mem_field32_modify(unit, PORT_TABm, port,
                                       STORM_CONTROL_PTRf, port));
        }
        /* programming PORT_TABLE.TRUST_DSCP_PTR is
         * handled in _bcm_common_init for KT2*/
    }
    if (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) {
        SOC_PBMP_CLEAR(my_pbmp_xport_xe);
        my_pbmp_xport_xe = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_XE,
                                                         my_pbmp_xport_xe);
        SOC_PBMP_CLEAR(bmp);
        SOC_PBMP_ASSIGN(bmp, PBMP_ALL(unit));
        PBMP_ITER(bmp, port) {
            if (SOC_PBMP_MEMBER(my_pbmp_xport_xe, port)) {
                SOC_IF_ERROR_RETURN(soc_phyctrl_speed_get(unit, port, &speed));
                if (speed == 21000) {
                    /* 21G HG ports support IEEE encapsulation to speed upto 20G */
                    speed = 20000;
                } else if (speed >= 10000) {
                    speed = 10000;
                } else {
                    speed = 1000;
                }
                /* Update speed for the new xe port */
                BCM_IF_ERROR_RETURN(bcm_esw_port_speed_set(unit, port, speed));
            }
        }
    }
#endif


    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td_port_deinit
 * Description:
 *      Deallocate resources of port module
 * Parameters:
 *      unit         - (IN) Device number
 * Return Value:
 *      BCM_E_XXX
 * Note:
 */
int
bcm_td_port_deinit(int unit)
{
    /* Cleanup profile for EGR_MASK table */
    if (_bcm_td_egr_mask_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                                _bcm_td_egr_mask_profile[unit]));
        sal_free(_bcm_td_egr_mask_profile[unit]);
        _bcm_td_egr_mask_profile[unit] = NULL;
    }

    /* Cleanup profile for SYSTEM_CONFIG_TABLE table */
    if (_bcm_td_sys_cfg_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                                _bcm_td_sys_cfg_profile[unit]));
        sal_free(_bcm_td_sys_cfg_profile[unit]);
        _bcm_td_sys_cfg_profile[unit] = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td_port_ing_pri_cng_set
 * Description:
 *      Set packet priority and cfi to internal priority and color mapping
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      untagged     - (IN) For untagged packet (ignore pkt_pri, cfi argumnet)
 *      pkt_pri      - (IN) Packet priority (802.1p cos)
 *      cfi          - (IN) Packet CFI
 *      int_pri      - (IN) Internal priority
 *      color        - (IN) Color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      When both pkt_pri and cfi are -1, the setting is for untagged packet
 */
int
bcm_td_port_ing_pri_cng_set(int unit, bcm_port_t port, int untagged,
                            int pkt_pri, int cfi,
                            int int_pri, bcm_color_t color)
{
    port_tab_entry_t         pent;
    ing_pri_cng_map_entry_t  map[16];
    ing_untagged_phb_entry_t phb;
    void                     *entries[2];
    uint32                   profile_index, old_profile_index;
    int                      index;
    int                      pkt_pri_cur, pkt_pri_min, pkt_pri_max;
    int                      cfi_cur, cfi_min, cfi_max;
    int                      rv = BCM_E_NONE;

    if (pkt_pri < 0) {
        pkt_pri_min = 0;
        pkt_pri_max = 7;
    } else {
        pkt_pri_min = pkt_pri;
        pkt_pri_max = pkt_pri;
    }

    if (cfi < 0) {
        cfi_min = 0;
        cfi_max = 1;
    } else {
        cfi_min = cfi;
        cfi_max = cfi;
    }
    
    /* Lock the port table */
    soc_mem_lock(unit, PORT_TABm); 

    /* Get profile index from port table. */
    rv = soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY,
                        SOC_PORT_MOD_OFFSET(unit, port), &pent);
    if (BCM_FAILURE(rv)) {
        goto cleanup_bcm_td_port_ing_pri_cng_set;
    }

    old_profile_index =
        soc_mem_field32_get(unit, PORT_TABm, &pent, TRUST_DOT1P_PTRf) * 16;

    entries[0] = map;
    entries[1] = &phb;

    rv = _bcm_ing_pri_cng_map_entry_get(unit, old_profile_index, 16, entries);
    if (BCM_FAILURE(rv)) {
        goto cleanup_bcm_td_port_ing_pri_cng_set;
    }

    if (untagged) {
        if (int_pri >= 0) {
            soc_mem_field32_set(unit, ING_UNTAGGED_PHBm, &phb, PRIf, int_pri);
        }
        if (color >= 0) {
            soc_mem_field32_set(unit, ING_UNTAGGED_PHBm, &phb, CNGf,
                                _BCM_COLOR_ENCODING(unit, color));
        }
    } else {
        for (pkt_pri_cur = pkt_pri_min; pkt_pri_cur <= pkt_pri_max;
             pkt_pri_cur++) {
            for (cfi_cur = cfi_min; cfi_cur <= cfi_max; cfi_cur++) {
                index = (pkt_pri_cur << 1) | cfi_cur;
                if (int_pri >= 0) {
                    soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &map[index],
                                        PRIf, int_pri);
                }
                if (color >= 0) {
                    soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &map[index],
                                        CNGf,
                                        _BCM_COLOR_ENCODING(unit, color));
                }
            }
        }
    }

    rv = _bcm_ing_pri_cng_map_entry_add(unit, entries, 16, &profile_index);
    if (BCM_FAILURE(rv)) {
        goto cleanup_bcm_td_port_ing_pri_cng_set;
    }

    if (profile_index != old_profile_index) {
        soc_mem_field32_set(unit, PORT_TABm, &pent, TRUST_DOT1P_PTRf,
                            profile_index / 16);
        rv = soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL,
                           SOC_PORT_MOD_OFFSET(unit, port), &pent);
        if (BCM_FAILURE(rv)) {
            goto cleanup_bcm_td_port_ing_pri_cng_set;
        }
    }

    if (old_profile_index != 0) {
        rv = _bcm_ing_pri_cng_map_entry_delete(unit, old_profile_index);
        if (BCM_FAILURE(rv)) {
            goto cleanup_bcm_td_port_ing_pri_cng_set;
        }
    }
cleanup_bcm_td_port_ing_pri_cng_set:
    /* Release port table lock */
    soc_mem_unlock(unit, PORT_TABm);
    return rv;
}

/*
 * Function:
 *      bcm_td_port_ing_pri_cng_get
 * Description:
 *      Get packet priority and cfi to internal priority and color mapping
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      untagged     - (IN) For untagged packet (ignore pkt_pri, cfi argumnet)
 *      pkt_pri      - (IN) Packet priority (802.1p cos)
 *      cfi          - (IN) Packet CFI
 *      int_pri      - (OUT) Internal priority
 *      color        - (OUT) Color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      When both pkt_pri and cfi are -1, the setting is for untagged packet
 */
int
bcm_td_port_ing_pri_cng_get(int unit, bcm_port_t port, int untagged,
                            int pkt_pri, int cfi,
                            int *int_pri, bcm_color_t *color)
{
    port_tab_entry_t         pent;
    ing_pri_cng_map_entry_t  map[16];
    ing_untagged_phb_entry_t phb;
    void                     *entries[2];
    uint32                   profile_index;
    int                      index;
    int                      hw_color;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY,
                                     SOC_PORT_MOD_OFFSET(unit, port), &pent));

    profile_index =
        soc_mem_field32_get(unit, PORT_TABm, &pent, TRUST_DOT1P_PTRf) * 16;

    entries[0] = map;
    entries[1] = &phb;

    BCM_IF_ERROR_RETURN
        (_bcm_ing_pri_cng_map_entry_get(unit, profile_index, 16, entries));
    if (untagged) {
        if (int_pri != NULL) {
            *int_pri = soc_mem_field32_get(unit, ING_UNTAGGED_PHBm, &phb,
                                           PRIf);
        }
        if (color != NULL) {
            hw_color = soc_mem_field32_get(unit, ING_UNTAGGED_PHBm, &phb,
                                           CNGf);
            *color = _BCM_COLOR_DECODING(unit, hw_color);
        }
    } else {
        index = (pkt_pri << 1) | cfi;
        if (int_pri != NULL) {
            *int_pri = soc_mem_field32_get(unit, ING_PRI_CNG_MAPm, &map[index],
                                           PRIf);
        }
        if (color != NULL) {
            hw_color = soc_mem_field32_get(unit, ING_PRI_CNG_MAPm, &map[index],
                                           CNGf);
            *color = _BCM_COLOR_DECODING(unit, hw_color);
        }
    }

    return BCM_E_NONE;
}

int
bcm_td_port_egress_set(int unit, bcm_port_t port, int modid, bcm_pbmp_t pbmp)
{
    soc_profile_mem_t *profile;
    egr_mask_modbase_entry_t base_entry;
    egr_mask_entry_t *mask_entries;
    void *entries[1];
    uint32 old_index, index;
    bcm_pbmp_t mask_pbmp;
    bcm_module_t modid_min, modid_max, cur_modid;
    bcm_port_t port_min, port_max, cur_port;
    bcm_trunk_t tid;
    int id, rv;
    bcm_pbmp_t pbmp_all, temp_pbmp_all;
    bcm_module_t my_modid;
    int modid_is_local;
    int egr_mask_modbase_index;
    bcm_module_t hw_mod;
    bcm_port_t hw_port;
    int mod_port_map_done = 0;

    profile = _bcm_td_egr_mask_profile[unit];

    BCM_PBMP_CLEAR(temp_pbmp_all);
    BCM_PBMP_ASSIGN(temp_pbmp_all, PBMP_PORT_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update (unit, &temp_pbmp_all);
    }
    if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &temp_pbmp_all));
    }
#endif

    BCM_PBMP_NEGATE(mask_pbmp, pbmp);
    BCM_PBMP_ASSIGN(pbmp_all,PBMP_CMIC(unit));  /* CPU port */
    BCM_PBMP_OR(pbmp_all,temp_pbmp_all);
    BCM_PBMP_AND(mask_pbmp, pbmp_all);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, port, &modid_min, &port_min, &tid,
                                    &id));

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        } else
#endif

#ifdef BCM_KATANA2_SUPPORT
        if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
        } else
#endif
        if (-1 != id || -1 != tid) {
            return BCM_E_PORT;
        }
        modid_max = modid_min;
        port_max = port_min;
        /* For devices with multiple module IDs, _bcm_esw_gport_resolve
         * had already performed module and port mapping.
         */
        mod_port_map_done = 1;
    } else {
        if (modid < -1 || modid > SOC_MODID_MAX(unit)) {
            return BCM_E_PARAM;
        } else if (modid == -1) {
            modid_min = 0;
            modid_max = SOC_MODID_MAX(unit);
        } else {
            modid_min = modid;
            modid_max = modid;
        }
        if (port < -1 || port > SOC_PORT_ADDR_MAX(unit)) {
            return BCM_E_PARAM;
        } else if (port == -1) {
            port_min = 0;
            port_max = SOC_PORT_ADDR_MAX(unit);
        } else {
            port_min = port;
            port_max = port;
        }
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    mask_entries = sal_alloc(sizeof(egr_mask_entry_t) * 256,
                             "port egress mask entries");
    if (mask_entries == NULL)
    {
        return BCM_E_MEMORY;
    }
    entries[0] = mask_entries;

    if (SOC_MEM_IS_VALID(unit, EGR_MASK_MODBASEm)) {
        soc_mem_lock(unit, EGR_MASK_MODBASEm);
    }

    rv = BCM_E_NONE;
    for (cur_modid = modid_min; cur_modid <= modid_max; cur_modid++) {

        rv = _bcm_esw_modid_is_local(unit, cur_modid, &modid_is_local);
        if (BCM_FAILURE(rv)) {
            break;
        }
        if (modid_is_local) {
            egr_mask_modbase_index = my_modid;
        } else {
            egr_mask_modbase_index = cur_modid;
        }

        old_index = 0;
        if (SOC_MEM_IS_VALID(unit, EGR_MASK_MODBASEm)) {
            rv = soc_mem_read(unit, EGR_MASK_MODBASEm, MEM_BLOCK_ALL,
                    egr_mask_modbase_index, &base_entry);
            if (BCM_FAILURE(rv)) {
                break;
            }
            old_index = soc_mem_field32_get(unit, EGR_MASK_MODBASEm, &base_entry,
                                        BASEf);
        }
        if (SOC_IS_TOMAHAWK3(unit)) {
            rv = soc_profile_mem_get(unit, profile, old_index,
                                 160, entries);
        } else {
            rv = soc_profile_mem_get(unit, profile, old_index,
                                 SOC_PORT_ADDR_MAX(unit) + 1, entries);
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
        for (cur_port = port_min; cur_port <= port_max; cur_port++) {
            if (modid_is_local && (NUM_MODID(unit) > 1) && !mod_port_map_done) {
                if (cur_port <= SOC_MODPORT_MAX(unit)) {
                    rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                            cur_modid, cur_port, &hw_mod, &hw_port);
                    if (BCM_FAILURE(rv)) {
                        break;
                    }
                } else {
                    break;
                }
            } else {
                hw_port = cur_port;
            }

            soc_mem_pbmp_field_set(unit, EGR_MASKm,
                    &mask_entries[hw_port], EGRESS_MASKf, &mask_pbmp);
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
        if (SOC_IS_TOMAHAWK3(unit)) {
            rv = soc_profile_mem_delete(unit, profile, old_index);

            rv = soc_profile_mem_add(unit, profile, entries,
                                 160, &index);
            if (BCM_FAILURE(rv)) {
                break;
            }
            sal_free(mask_entries);
            return rv;
        } else {
            rv = soc_profile_mem_add(unit, profile, entries,
                                 SOC_PORT_ADDR_MAX(unit) + 1, &index);
        }

        if (BCM_FAILURE(rv)) {
            break;
        }
        if (SOC_MEM_IS_VALID(unit, EGR_MASK_MODBASEm)) {
            rv = soc_mem_field32_modify(unit, EGR_MASK_MODBASEm,
                egr_mask_modbase_index, BASEf, index);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        rv = soc_profile_mem_delete(unit, profile, old_index);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    if (SOC_MEM_IS_VALID(unit, EGR_MASK_MODBASEm)) {
        soc_mem_unlock(unit, EGR_MASK_MODBASEm);
    }

    sal_free(mask_entries);
    return rv;
}

int
bcm_td_port_egress_get(int unit, bcm_port_t port, int modid, bcm_pbmp_t *pbmp)
{
    soc_profile_mem_t *profile;
    egr_mask_modbase_entry_t base_entry;
    egr_mask_entry_t *mask_entries;
    void *entries[1];
    uint32 index;
    bcm_pbmp_t mask_pbmp, temp_pbmp_all;
    bcm_module_t local_modid;
    bcm_port_t local_port;
    bcm_trunk_t tid;
    int id, rv;

    profile = _bcm_td_egr_mask_profile[unit];

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, port, &local_modid, &local_port,
                                    &tid, &id));

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        } else
#endif

#ifdef BCM_KATANA2_SUPPORT
        if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
        } else
#endif
        if (-1 != id || -1 != tid) {
            return BCM_E_PORT;
        }
    } else {
        if (modid < 0 || modid > SOC_MODID_MAX(unit) ||
            port < 0 || port > SOC_PORT_ADDR_MAX(unit)) {
            return BCM_E_PARAM;
        } else {
            PORT_DUALMODID_VALID(unit, port);
            BCM_IF_ERROR_RETURN(_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                        modid, port, &local_modid, &local_port));
        }
    }

    index = 0;
    if (SOC_MEM_IS_VALID(unit, EGR_MASK_MODBASEm)) {
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, EGR_MASK_MODBASEm, MEM_BLOCK_ALL, local_modid,
                      &base_entry));
        index = soc_mem_field32_get(unit, EGR_MASK_MODBASEm, &base_entry, BASEf);
    }
    mask_entries = sal_alloc(sizeof(egr_mask_entry_t) * 256,
                             "port egress mask entries");
    if (mask_entries == NULL)
    {
        return BCM_E_MEMORY;
    }
    entries[0] = mask_entries;
    if (SOC_IS_TOMAHAWK3(unit)) {
        rv = soc_profile_mem_get(unit, profile, index, 160,
                             entries);
    } else {
        rv = soc_profile_mem_get(unit, profile, index, SOC_PORT_ADDR_MAX(unit) + 1,
                             entries);
    }
    if (BCM_FAILURE(rv))
    {
        sal_free(mask_entries);
        return rv;
    }
    soc_mem_pbmp_field_get(unit, EGR_MASKm, &mask_entries[local_port],
                           EGRESS_MASKf, &mask_pbmp);

    BCM_PBMP_CLEAR(temp_pbmp_all);
    BCM_PBMP_ASSIGN(temp_pbmp_all, PBMP_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update (unit, &temp_pbmp_all);
    }
    if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
        rv = _bcm_kt2_flexio_pbmp_update(unit, &temp_pbmp_all);
        if (BCM_FAILURE(rv))
        {
            sal_free(mask_entries);
            return rv;
        }
    }
#endif

    BCM_PBMP_NEGATE(*pbmp, mask_pbmp);
    BCM_PBMP_AND(*pbmp, temp_pbmp_all);
    BCM_PBMP_REMOVE(*pbmp, PBMP_LB(unit));
    BCM_PBMP_REMOVE(*pbmp, PBMP_RDB_ALL(unit));
    BCM_PBMP_REMOVE(*pbmp, PBMP_MACSEC_ALL(unit));
    BCM_PBMP_REMOVE(*pbmp, PBMP_FAE_ALL(unit));

    sal_free(mask_entries);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td_mod_port_tpid_enable_write
 * Description:
 *      Write the Tag Protocol ID enables for a (module, port).
 * Parameters:
 *      unit - Device number
 *      mod  - Module ID
 *      port - Port number
 *      tpid_enable - Tag Protocol ID enables
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_td_mod_port_tpid_enable_write(int unit, bcm_module_t mod,
        bcm_port_t port, int tpid_enable)
{
    int rv = BCM_E_NONE;
    system_config_table_modbase_entry_t modbase_entry;
    system_config_table_entry_t *entry_array;
    void *entries;
    uint32 old_profile_index, new_profile_index;
    int i;
    bcm_gport_t gport;

    if (soc_mem_is_valid(unit, SYSTEM_CONFIG_TABLEm)) {
        SOC_IF_ERROR_RETURN
        (READ_SYSTEM_CONFIG_TABLE_MODBASEm(unit, MEM_BLOCK_ANY, mod,
                                           &modbase_entry));
        old_profile_index = soc_SYSTEM_CONFIG_TABLE_MODBASEm_field32_get(unit,
                &modbase_entry, BASEf);

        entry_array = sal_alloc(sizeof(system_config_table_entry_t) *
                (SOC_PORT_ADDR_MAX(unit) + 1), "system config table entry array");
        if (entry_array == NULL) {
            return BCM_E_MEMORY;
        }

        for (i = 0; i <= SOC_PORT_ADDR_MAX(unit); i++) {
           sal_memcpy(&entry_array[i],
                   SOC_PROFILE_MEM_ENTRY(unit, _bcm_td_sys_cfg_profile[unit],
                       void *, old_profile_index + i),
                   sizeof(system_config_table_entry_t));
        }

        soc_SYSTEM_CONFIG_TABLEm_field32_set(unit, &entry_array[port],
                OUTER_TPID_ENABLEf, tpid_enable);

        entries = entry_array;
        rv = soc_profile_mem_add(unit, _bcm_td_sys_cfg_profile[unit], &entries,
                                 SOC_PORT_ADDR_MAX(unit) + 1, &new_profile_index);
        if (BCM_FAILURE(rv)) {
            sal_free(entry_array);
            return rv;
        }

        soc_SYSTEM_CONFIG_TABLE_MODBASEm_field32_set(unit, &modbase_entry,
                BASEf, new_profile_index);
        rv = WRITE_SYSTEM_CONFIG_TABLE_MODBASEm(unit, MEM_BLOCK_ALL, mod,
                                               &modbase_entry);
        if (BCM_FAILURE(rv)) {
            sal_free(entry_array);
            return rv;
        }

        rv = soc_profile_mem_delete(unit, _bcm_td_sys_cfg_profile[unit],
                                    old_profile_index);
        sal_free(entry_array);
    } else {
        if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, OUTER_TPID_ENABLEf)) {
            gport = _bcm_esw_port_gport_get_from_modport(unit, mod,
                                                        port);
            rv = _bcm_trx_source_trunk_map_set(unit, gport, OUTER_TPID_ENABLEf,
                                              tpid_enable);
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td_mod_port_tpid_enable_read
 * Description:
 *      Get the Tag Protocol ID enables for a (module, port).
 * Parameters:
 *      unit - Device number
 *      mod  - Module ID
 *      port - Port number
 *      tpid_enable - (OUT) Tag Protocol ID enables
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_td_mod_port_tpid_enable_read(int unit, bcm_module_t mod, bcm_port_t port,
        int *tpid_enable)
{
    system_config_table_modbase_entry_t modbase_entry;
    system_config_table_entry_t sys_cfg_entry;
    int base;
    bcm_gport_t gport;

    if (soc_mem_is_valid(unit, SYSTEM_CONFIG_TABLEm)) {
        SOC_IF_ERROR_RETURN
            (READ_SYSTEM_CONFIG_TABLE_MODBASEm(unit, MEM_BLOCK_ANY, mod,
                                               &modbase_entry));
        base = soc_SYSTEM_CONFIG_TABLE_MODBASEm_field32_get(unit, &modbase_entry,
                BASEf);

        SOC_IF_ERROR_RETURN
            (READ_SYSTEM_CONFIG_TABLEm(unit, MEM_BLOCK_ANY, base + port,
                                       &sys_cfg_entry));
        *tpid_enable = soc_SYSTEM_CONFIG_TABLEm_field32_get(unit, &sys_cfg_entry,
                OUTER_TPID_ENABLEf);
    } else {
        gport = _bcm_esw_port_gport_get_from_modport(unit, mod,
                                                    port);
        BCM_IF_ERROR_RETURN(
            _bcm_trx_source_trunk_map_get(unit, gport, OUTER_TPID_ENABLEf,
                                           (uint32 *)tpid_enable));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td_mod_port_tpid_set
 * Description:
 *      Set the Tag Protocol ID for a (module, port).
 * Parameters:
 *      unit - Device number
 *      mod  - Module ID
 *      port - Port number
 *      tpid - Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_td_mod_port_tpid_set(int unit, bcm_module_t mod, bcm_port_t port,
        uint16 tpid)
{
    int rv = BCM_E_NONE;
    int tpid_enable, tpid_index;

    _bcm_fb2_outer_tpid_tab_lock(unit);

    rv = _bcm_td_mod_port_tpid_enable_read(unit, mod, port, &tpid_enable);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return rv;
    }

    tpid_index = 0;
    while (tpid_enable) {
        if (tpid_enable & 1) {
            rv = _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
            if (BCM_FAILURE(rv)) {
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                return rv;
            }
        }
        tpid_enable = tpid_enable >> 1;
        tpid_index++;
    }

    rv = _bcm_fb2_outer_tpid_entry_add(unit, tpid, &tpid_index);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return rv;
    }

    tpid_enable = 1 << tpid_index;
    rv = _bcm_td_mod_port_tpid_enable_write(unit, mod, port, tpid_enable);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
    }

    _bcm_fb2_outer_tpid_tab_unlock(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_td_mod_port_tpid_get
 * Description:
 *      Retrieve the Tag Protocol ID for a (module, port).
 * Parameters:
 *      unit - Device number
 *      mod  - Module ID 
 *      port - Port number
 *      tpid - (OUT) Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_td_mod_port_tpid_get(int unit, bcm_module_t mod, bcm_port_t port,
        uint16 *tpid)
{
    int tpid_enable, tpid_index;

    BCM_IF_ERROR_RETURN
        (_bcm_td_mod_port_tpid_enable_read(unit, mod, port, &tpid_enable));

    tpid_index = 0;
    while (tpid_enable) {
        if (tpid_enable & 1) {
            return _bcm_fb2_outer_tpid_entry_get(unit, tpid, tpid_index);
        }
        tpid_enable = tpid_enable >> 1;
        tpid_index++;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_td_mod_port_tpid_add
 * Description:
 *      Add TPID for a (module, port).
 * Parameters:
 *      unit - (IN) Device number
 *      mod  - (IN) Module ID
 *      port - (IN) Port number
 *      tpid - (IN) Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_td_mod_port_tpid_add(int unit, bcm_module_t mod, bcm_port_t port, 
                      uint16 tpid)
{
    int rv = BCM_E_NONE;
    int remove_tpid = FALSE;
    int tpid_enable, index;

    _bcm_fb2_outer_tpid_tab_lock(unit);

    rv = _bcm_td_mod_port_tpid_enable_read(unit, mod, port, &tpid_enable);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return rv;
    }

    rv = _bcm_fb2_outer_tpid_lkup(unit, tpid, &index);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return rv;
    }

    if (rv == BCM_E_NOT_FOUND || !(tpid_enable & (1 << index))) {
        rv = _bcm_fb2_outer_tpid_entry_add(unit, tpid, &index);
        if (BCM_FAILURE(rv)) {
            _bcm_fb2_outer_tpid_tab_unlock(unit);
            return rv;
        }
        remove_tpid = TRUE;
    }

    tpid_enable |= (1 << index);
    rv = _bcm_td_mod_port_tpid_enable_write(unit, mod, port, tpid_enable);
    if (BCM_FAILURE(rv)) {
        if (remove_tpid) {
            _bcm_fb2_outer_tpid_entry_delete(unit, index);
        }
    }

    _bcm_fb2_outer_tpid_tab_unlock(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_td_mod_port_tpid_delete
 * Description:
 *      Delete TPID for a (module, port).
 * Parameters:
 *      unit - (IN) Device number
 *      mod  - (IN) Module ID
 *      port - (IN) Port number
 *      tpid - (IN) Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_td_mod_port_tpid_delete(int unit, bcm_module_t mod, bcm_port_t port,
        uint16 tpid)
{
    int rv = BCM_E_NONE;
    int index, tpid_enable;

    _bcm_fb2_outer_tpid_tab_lock(unit);

    rv = _bcm_fb2_outer_tpid_lkup(unit, tpid, &index);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return rv;
    } 

    rv = _bcm_td_mod_port_tpid_enable_read(unit, mod, port, &tpid_enable);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return rv;
    }

    if (tpid_enable & (1 << index)) {
        tpid_enable &= ~(1 << index);
        rv = _bcm_td_mod_port_tpid_enable_write(unit, mod, port, tpid_enable);
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return rv;
    }

    rv = _bcm_fb2_outer_tpid_entry_delete(unit, index);
    _bcm_fb2_outer_tpid_tab_unlock(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_td_mod_port_tpid_delete_all
 * Description:
 *      Delete all TPID for a (module, port).
 * Parameters:
 *      unit - (IN) Device number
 *      mod  - (IN) Module ID
 *      port - (IN) Port number
 * Return Value:
 *      BCM_E_XXX
 */
int 
_bcm_td_mod_port_tpid_delete_all(int unit, bcm_module_t mod, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    int tpid_enable, tpid_index;

    _bcm_fb2_outer_tpid_tab_lock(unit);

    rv = _bcm_td_mod_port_tpid_enable_read(unit, mod, port, &tpid_enable);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return rv;
    }

    rv = _bcm_td_mod_port_tpid_enable_write(unit, mod, port, 0);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return rv;
    }

    tpid_index = 0;
    while (tpid_enable) {
        if (tpid_enable & 1) {
            rv = _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
            if (BCM_FAILURE(rv)) {
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                return rv;
            }
        }
        tpid_enable = tpid_enable >> 1;
        tpid_index++;
    }

    _bcm_fb2_outer_tpid_tab_unlock(unit);
    return rv;
}

/*
 * Function:
 *      bcm_td_port_niv_type_set
 * Description:
 *      Set NIV port type.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      value - (IN) NIV port type 
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_td_port_niv_type_set(int unit, bcm_port_t port, int value)
{
    int mod_port_index;
    int pt_vt_key_type_value;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                DISCARD_IF_VNTAG_NOT_PRESENTf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                DISCARD_IF_VNTAG_PRESENTf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                VNTAG_ACTIONS_IF_PRESENTf, VNTAG_NOOP));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                VNTAG_ACTIONS_IF_NOT_PRESENTf, VNTAG_NOOP));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                TX_DEST_PORT_ENABLEf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                NIV_VIF_LOOKUP_ENABLEf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                NIV_RPF_CHECK_ENABLEf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                NIV_UPLINK_PORTf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                VT_ENABLEf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                DISABLE_VLAN_CHECKSf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_pt_vtkey_type_value_get(unit,
                VLXLT_HASH_KEY_TYPE_IVID, &pt_vt_key_type_value));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                _BCM_CPU_TABS_ETHER, VT_KEY_TYPE_2f,
                pt_vt_key_type_value));

    BCM_IF_ERROR_RETURN(_bcm_esw_egr_port_tab_set(unit, port,
                VNTAG_ACTIONS_IF_PRESENTf, VNTAG_NOOP));
    BCM_IF_ERROR_RETURN(_bcm_esw_egr_port_tab_set(unit, port,
                NIV_PRUNE_ENABLEf, 0));
    BCM_IF_ERROR_RETURN(_bcm_esw_egr_port_tab_set(unit, port,
                NIV_UPLINK_PORTf, 0));

    BCM_IF_ERROR_RETURN(
        _bcm_esw_src_mod_port_table_index_get_from_port(unit, port,
                                                        &mod_port_index));
    SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, EGR_GPP_ATTRIBUTESm,
                mod_port_index, SRC_IS_NIV_UPLINK_PORTf, 0));

    switch (value) {
        case BCM_PORT_NIV_TYPE_NONE:
            /* Everything is set to defaults above */
            break;

        case BCM_PORT_NIV_TYPE_SWITCH:
            BCM_IF_ERROR_RETURN(_bcm_esw_pt_vtkey_type_value_get(unit,
                        VLXLT_HASH_KEY_TYPE_VIF, &pt_vt_key_type_value));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, VT_KEY_TYPE_2f,
                        pt_vt_key_type_value));
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (soc_mem_field_valid(unit, PORT_TABm, VT_PORT_TYPE_SELECT_2f) ||
                soc_mem_field_valid(unit, LPORT_TABm, VT_PORT_TYPE_SELECT_2f)) {
                BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                    _BCM_CPU_TABS_ETHER, VT_PORT_TYPE_SELECT_2f, 1));
            } else if (soc_mem_field_valid(unit, PORT_TABm, VT_PORT_TYPE_SELECTf)) {
                BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                    _BCM_CPU_TABS_ETHER, VT_PORT_TYPE_SELECTf, 1));
            } else
#endif /* BCM_TRIUMPH3_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                            _BCM_CPU_TABS_ETHER, VT_KEY_TYPE_USE_GLPf, 1));
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, VT_ENABLEf, 1));
            break;

        case BCM_PORT_NIV_TYPE_UPLINK:
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, DISCARD_IF_VNTAG_NOT_PRESENTf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, NIV_VIF_LOOKUP_ENABLEf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, NIV_UPLINK_PORTf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, DISABLE_VLAN_CHECKSf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_egr_port_tab_set(unit, port,
                        NIV_UPLINK_PORTf, 1));
            SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
                        EGR_GPP_ATTRIBUTESm, mod_port_index,
                        SRC_IS_NIV_UPLINK_PORTf, 1));
            break;

        case BCM_PORT_NIV_TYPE_DOWNLINK_ACCESS:
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, DISCARD_IF_VNTAG_PRESENTf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, VNTAG_ACTIONS_IF_NOT_PRESENTf,
                        VNTAG_ADD));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, TX_DEST_PORT_ENABLEf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, DISABLE_VLAN_CHECKSf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_egr_port_tab_set(unit, port,
                        VNTAG_ACTIONS_IF_PRESENTf, VNTAG_DELETE));
            BCM_IF_ERROR_RETURN(_bcm_esw_egr_port_tab_set(unit, port,
                        NIV_PRUNE_ENABLEf, 1));
            break;

        case BCM_PORT_NIV_TYPE_DOWNLINK_TRANSIT:
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, DISCARD_IF_VNTAG_NOT_PRESENTf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, TX_DEST_PORT_ENABLEf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, NIV_RPF_CHECK_ENABLEf, 1));
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                        _BCM_CPU_TABS_ETHER, DISABLE_VLAN_CHECKSf, 1));
            break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td_port_niv_type_get
 * Description:
 *      Get NIV port type.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      value - (OUT) NIV port type 
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_td_port_niv_type_get(int unit, bcm_port_t port, int *value)
{
    int rv = BCM_E_NONE;
    int vt_key_type, vt_enable, uplink, rpf_enable, prune_enable;
    int vt_key_type_value;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit,
                port, VT_KEY_TYPE_2f, &vt_key_type_value));
    BCM_IF_ERROR_RETURN(_bcm_esw_pt_vtkey_type_get(unit,
                vt_key_type_value, &vt_key_type));

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit,
                port, VT_ENABLEf, &vt_enable));

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit,
                port, NIV_UPLINK_PORTf, &uplink));

    BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit,
                port, NIV_RPF_CHECK_ENABLEf, &rpf_enable));

    BCM_IF_ERROR_RETURN(_bcm_esw_egr_port_tab_get(unit,
                port, NIV_PRUNE_ENABLEf, &prune_enable));

    if (vt_enable && (vt_key_type == VLXLT_HASH_KEY_TYPE_VIF)) {
        *value = BCM_PORT_NIV_TYPE_SWITCH;
    } else if (uplink) {
        *value = BCM_PORT_NIV_TYPE_UPLINK;
    } else if (rpf_enable) {
        *value = BCM_PORT_NIV_TYPE_DOWNLINK_TRANSIT;
    } else if (prune_enable) {
        *value = BCM_PORT_NIV_TYPE_DOWNLINK_ACCESS;
    } else {
        *value = BCM_PORT_NIV_TYPE_NONE;
    }

    return rv;
}

#ifdef INCLUDE_L3
/*
 * Function:
 *      _bcm_td_dvp_group_get 
 * Purpose:
 *      Get VP group gport from DVP 
 * Parameters:
 *      unit        - (IN) Device number.
 *      gport       - (IN) Gport number of DVP 
 *      dvp         - (IN) Extracted VP from Gport 
 *      vpg_gport   - (OUT) vp group gport 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_td_dvp_group_get(int unit, bcm_gport_t gport, int dvp, bcm_gport_t *vpg_gport)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_dvp_group_pruning)) {
        int vpg;
        egr_dvp_attribute_entry_t dvp_entry;

        SOC_IF_ERROR_RETURN
            (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, dvp, &dvp_entry));

        vpg = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &dvp_entry,
                VP_GROUP_PTRf);

        if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            BCM_GPORT_VLAN_PORT_ID_SET(*vpg_gport, vpg);
        }else if (BCM_GPORT_IS_NIV_PORT(gport)) {
            BCM_GPORT_NIV_PORT_ID_SET(*vpg_gport, vpg);
        }else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
            BCM_GPORT_EXTENDER_PORT_ID_SET(*vpg_gport, vpg);
        }else if (BCM_GPORT_IS_VXLAN_PORT(gport)) {
            BCM_GPORT_VXLAN_PORT_ID_SET(*vpg_gport, vpg);
        }else if (BCM_GPORT_IS_MIM_PORT(gport)) {
            BCM_GPORT_MIM_PORT_ID_SET(*vpg_gport, vpg);
        }else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
            BCM_GPORT_MPLS_PORT_ID_SET(*vpg_gport, vpg);
        }else {
            return BCM_E_PARAM;
        }
        rv = BCM_E_NONE;
    }
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_td_dvp_group_set 
 * Purpose:
 *      Set VP group for DVP 
 * Parameters:
 *      unit        - (IN) Device number.
 *      gport       - (IN) Gport number of DVP 
 *      dvp         - (IN) Extracted VP from Gport 
 *      vpg_gport   - (IN) vp group gport 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_td_dvp_group_set(int unit, bcm_gport_t gport, int dvp, bcm_gport_t vpg_gport)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_dvp_group_pruning)) {
        int vpg;
        egr_dvp_attribute_entry_t dvp_entry;

        if ((BCM_GPORT_IS_VLAN_PORT(gport) && 
                    !BCM_GPORT_IS_VLAN_PORT(vpg_gport)) ||
                (BCM_GPORT_IS_NIV_PORT(gport) &&
                 !BCM_GPORT_IS_NIV_PORT(vpg_gport)) ||
                (BCM_GPORT_IS_EXTENDER_PORT(gport) &&
                 !BCM_GPORT_IS_EXTENDER_PORT(vpg_gport)) ||
                (BCM_GPORT_IS_VXLAN_PORT(gport) &&
                 !BCM_GPORT_IS_VXLAN_PORT(vpg_gport)) ||
                (BCM_GPORT_IS_MIM_PORT(gport) &&
                 !BCM_GPORT_IS_MIM_PORT(vpg_gport)) ||
                (BCM_GPORT_IS_MPLS_PORT(gport) &&
                 !BCM_GPORT_IS_MPLS_PORT(vpg_gport))) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_VLAN_PORT(vpg_gport)) {
            vpg = BCM_GPORT_VLAN_PORT_ID_GET(vpg_gport);
            if (!_bcm_vp_used_get(unit, vpg, _bcmVpTypeVlan)) {
                return BCM_E_NOT_FOUND;
            }
        }else if (BCM_GPORT_IS_NIV_PORT(vpg_gport)) {
            vpg = BCM_GPORT_NIV_PORT_ID_GET(vpg_gport);
            if(!_bcm_vp_used_get(unit, vpg, _bcmVpTypeNiv)) {
                return BCM_E_NOT_FOUND;
            }
        }else if (BCM_GPORT_IS_EXTENDER_PORT(vpg_gport)) {
            vpg = BCM_GPORT_EXTENDER_PORT_ID_GET(vpg_gport);
            if(!_bcm_vp_used_get(unit, vpg, _bcmVpTypeExtender)) {
                return BCM_E_NOT_FOUND;
            }
        }else if (BCM_GPORT_IS_VXLAN_PORT(vpg_gport)) {
            vpg = BCM_GPORT_VXLAN_PORT_ID_GET(vpg_gport);
            if (!_bcm_vp_used_get(unit, vpg, _bcmVpTypeVxlan)) {
                return BCM_E_PARAM;
            }
        }else if (BCM_GPORT_IS_MIM_PORT(vpg_gport)) {
            vpg = BCM_GPORT_MIM_PORT_ID_GET(vpg_gport);
            if (!_bcm_vp_used_get(unit, vpg, _bcmVpTypeMim)) {
                return BCM_E_PARAM;
            }
        }else if (BCM_GPORT_IS_MPLS_PORT(vpg_gport)) {
            vpg = BCM_GPORT_MPLS_PORT_ID_GET(vpg_gport);
            if (!_bcm_vp_used_get(unit, vpg, _bcmVpTypeMpls)) {
                return BCM_E_PARAM;
            }
        }else {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN
            (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, dvp, &dvp_entry));
        soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &dvp_entry,
                VP_GROUP_PTRf, vpg);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ALL, dvp, &dvp_entry));
        rv = BCM_E_NONE;
    }
#endif 
    return rv;
}

/*
 * Function:
 *      _bcm_td_port_vif_match_multi_get
 * Purpose:
 *      Get all the matches for an existing vif
 * Parameters:
 *      unit        - (IN)  Device number.
 *      gport       - (IN)  Gport number
 *      size        - (IN)  Number of elements in match array
 *      match_array - (OUT) Match array
 *      count       - (OUT) Match count
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_td_port_vif_match_multi_get(int unit, bcm_gport_t gport, int size,
                                 bcm_port_match_info_t *match_array,
                                 int *count)
{
    int                    rv = BCM_E_NONE;
    int                    vp, tmp_vp;
    uint8                  *vt_buf = NULL;
    int                    index, index_min, index_max;
    void                   *vent = NULL;
    int                    action;
    bcm_gport_t            match_gport = 0;
    int                    key_type_value, key_type;
    int                    cnt = 0;
    bcm_module_t           mod_out, mod_in;
    bcm_port_t             port_out, port_in;
    soc_mem_t              vx_mem = VLAN_XLATEm;

    if (soc_mem_is_valid(unit, VLAN_XLATE_1_DOUBLEm)) {
        vx_mem = VLAN_XLATE_1_DOUBLEm;
    }

    /* Parameter checks */
    if ((size < 0) || (count == NULL)) {
        return BCM_E_PARAM;
    }

    if ((size > 0) && (match_array == NULL)) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_NIV_PORT(gport)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
        if (!(_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv))) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_NOT_FOUND;
    }

    vt_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, vx_mem),
                           "VLAN_XLATE buffer");

    if (NULL == vt_buf) {
        return BCM_E_MEMORY;
    }

    index_min = soc_mem_index_min(unit, vx_mem);
    index_max = soc_mem_index_max(unit, vx_mem);

    soc_mem_lock(unit, vx_mem);
    if ((rv = soc_mem_read_range(unit, vx_mem, MEM_BLOCK_ANY,
                                 index_min, index_max, vt_buf)) < 0) {
        goto cleanup;
    }

    for (index = index_min; index <= index_max; index++) {
        vent = soc_mem_table_idx_to_pointer(unit, vx_mem,
                                            void *,
                                            vt_buf, index);
        if (soc_feature(unit, soc_feature_base_valid)) {
            if (soc_mem_field32_get(unit, vx_mem, vent, BASE_VALID_0f) != 3 ||
                    soc_mem_field32_get(unit, vx_mem, vent, BASE_VALID_1f) != 7) {
                continue;
            }
        } else {
            if (soc_mem_field32_get(unit, vx_mem, vent, VALIDf) == 0) {
                continue;
            }
        }
        action = soc_mem_field32_get(unit, vx_mem, vent, VIF__MPLS_ACTIONf);
        if (action != 1) {
            continue; /* Entry is not for an SVP */
        }
        tmp_vp = soc_mem_field32_get(unit, vx_mem, vent, VIF__SOURCE_VPf);
        if (vp != tmp_vp) {
            continue;
        }

        if ((size > 0) && (size > cnt)) {
            key_type_value = (int)soc_mem_field32_get(unit, vx_mem, vent,
                                                              KEY_TYPEf);
            rv = _bcm_esw_vlan_xlate_key_type_get(unit, key_type_value,
                                                  &key_type);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            sal_memset(match_array, 0, sizeof(bcm_port_match_info_t));
            match_array->match_vlan = BCM_VLAN_INVALID;
            switch (key_type) {
                case VLXLT_HASH_KEY_TYPE_VIF_VLAN:
                    if (match_array->match_vlan == BCM_VLAN_INVALID) {
                        match_array->match_vlan =
                            soc_mem_field32_get(unit, vx_mem, vent, VIF__VLANf);
                    }
                case VLXLT_HASH_KEY_TYPE_VIF:
                    if (match_array->match_vlan == BCM_VLAN_INVALID) {
                        match_array->match_vlan = BCM_VLAN_NONE;
                    }

                    if (soc_mem_field32_get(unit, vx_mem, vent, VIF__Tf)) {
                        BCM_GPORT_TRUNK_SET(
                            match_gport,
                            soc_mem_field32_get(unit, vx_mem, vent,
                                                        VIF__TGIDf));
                    } else {
                        mod_in = soc_mem_field32_get(unit, vx_mem, vent,
                                                             VIF__MODULE_IDf);
                        port_in = soc_mem_field32_get(unit, vx_mem, vent,
                                                              VIF__PORT_NUMf);
                        rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                                     mod_in, port_in,
                                                     &mod_out, &port_out);
                        BCM_GPORT_MODPORT_SET(match_gport, mod_out, port_out);
                    }

                    if (BCM_GPORT_IS_NIV_PORT(gport)) {
                        if (match_array->match_vlan == BCM_VLAN_NONE) {
                            match_array->match =
                                BCM_PORT_MATCH_PORT_NIV_PORT_VIF;
                        } else {
                            match_array->match =
                                BCM_PORT_MATCH_PORT_NIV_PORT_VIF_VLAN;
                        }
                        match_array->niv_port_vif =
                            soc_mem_field32_get(unit, vx_mem, vent,
                                                        VIF__SRC_VIFf);
                    }

                    match_array->port = match_gport;
                    match_array++;
                    break;
                default:
                    rv = BCM_E_PARAM;
                    goto cleanup;
            }
        }
        cnt++;
    }
    *count = cnt;
cleanup:
    if (vt_buf) {
        soc_cm_sfree(unit, vt_buf);
    }
    soc_mem_unlock(unit, vx_mem);
    return rv;
}

/*
 * Function:
 *      bcm_td_vp_control_set
 * Purpose:
 *      Set per virtual port controls.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN) VP gport number
 *      type - (IN) Control type
 *      value - (IN) Value to be set
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td_vp_control_set(int unit, bcm_gport_t gport,
                      bcm_port_control_t type, int value)
{
    int rv;
    int vp = -1;
#ifdef BCM_TRIDENT2_SUPPORT
    bcm_niv_egress_t niv_egress;
    int              count;
    bcm_gport_t port = BCM_GPORT_INVALID;
    bcm_extender_egress_t extender_egress;
    _bcm_vp_info_t vp_info;
#endif
    soc_field_t      field;
    bcm_niv_port_t niv_port;
    bcm_vlan_port_t vlan_port;
    bcm_extender_port_t ext_port;
    source_vp_entry_t svp_entry;
    egr_dvp_attribute_entry_t dvp_entry;
    int cml = 0;

    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
            return BCM_E_NOT_FOUND;
        }
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vlan_port_id = gport;
        BCM_IF_ERROR_RETURN(
            bcm_esw_vlan_port_find(unit, &vlan_port));
#ifdef BCM_TRIDENT2_SUPPORT
        port = vlan_port.port;
#endif
    } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
            return BCM_E_NOT_FOUND;
        }
        bcm_niv_port_t_init(&niv_port);
        niv_port.niv_port_id = gport;
        BCM_IF_ERROR_RETURN(
            bcm_esw_niv_port_get(unit, &niv_port));
#ifdef BCM_TRIDENT2_SUPPORT
        if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
            port = BCM_GPORT_INVALID;
            bcm_niv_egress_t_init(&niv_egress);
            rv = bcm_trident_niv_egress_get(unit, niv_port.niv_port_id,
                                            1, &niv_egress, &count);
            if (BCM_SUCCESS(rv)) {
                if (!(niv_egress.flags & BCM_NIV_EGRESS_MULTICAST)) {
                    port = niv_egress.port;
                } 
            }
        } else {
            port = niv_port.port;
        }
#endif
    } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
            return BCM_E_NOT_FOUND;
        }
        bcm_extender_port_t_init(&ext_port);
        ext_port.extender_port_id = gport;
        BCM_IF_ERROR_RETURN(
            bcm_esw_extender_port_get(unit, &ext_port));
#ifdef BCM_TRIDENT2_SUPPORT
        if (ext_port.flags & BCM_EXTENDER_PORT_MATCH_NONE) {
            bcm_extender_egress_t_init(&extender_egress);
            BCM_IF_ERROR_RETURN(bcm_esw_extender_egress_get_all(unit,
                        ext_port.extender_port_id, 1, &extender_egress, &count));
            if (count == 0) {
                /* No Extender egress object has been added to VP yet. */
                return BCM_E_CONFIG;
            }
            if (extender_egress.flags & BCM_EXTENDER_EGRESS_MULTICAST) {
                return BCM_E_PARAM;
            }
            port = extender_egress.port;
        } else {
            port = ext_port.port;
        }
#endif
    } else if (BCM_GPORT_IS_TRUNK(gport)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_tid_to_vp_lag_vp(unit, 
                            BCM_GPORT_TRUNK_GET(gport), &vp));
#ifdef BCM_TRIDENT2_SUPPORT
        port = BCM_GPORT_INVALID;
#endif        
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
	} else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            return BCM_E_PARAM;
        }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
        port = BCM_GPORT_INVALID;
    } else if (BCM_GPORT_IS_VXLAN_PORT(gport)) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
            return BCM_E_PARAM;
        }
        port = BCM_GPORT_INVALID;
    } else if (BCM_GPORT_IS_MIM_PORT(gport)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
            return BCM_E_PARAM;
        }
        port = BCM_GPORT_INVALID;
    } else if (BCM_GPORT_IS_L2GRE_PORT(gport)) {
        vp = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
            return BCM_E_PARAM;
        }
        port = BCM_GPORT_INVALID;
#ifdef BCM_TRIDENT3_SUPPORT
    } else if (BCM_GPORT_IS_FLOW_PORT(gport)) {
        vp = BCM_GPORT_FLOW_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
            return BCM_E_PARAM;
        }
        port = BCM_GPORT_INVALID;
#endif
#endif
#ifdef BCM_HGPROXY_COE_SUPPORT
    } else if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&

        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
#endif
    } else { 
        return BCM_E_PARAM;
    }

    rv = BCM_E_UNAVAIL;

    switch (type) {
        case bcmPortControlBridge:
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, DISABLE_VP_PRUNINGf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                        DISABLE_VP_PRUNINGf, value ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                rv = BCM_E_NONE;
            }

            field = DISABLE_VP_PRUNINGf;
            if (BCM_GPORT_IS_NIV_PORT(gport)) {
                if (SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm,
                        COMMON__DISABLE_VP_PRUNINGf)) {
                    field = COMMON__DISABLE_VP_PRUNINGf;
                }
            }
#ifdef BCM_TRIDENT2_SUPPORT
            else if (BCM_GPORT_IS_MPLS_PORT(gport) ||
                    BCM_GPORT_IS_MIM_PORT(gport)) {
                if (SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm,
                        COMMON__DISABLE_VP_PRUNINGf)) {
                    field = COMMON__DISABLE_VP_PRUNINGf;
                }
            }
            else if (BCM_GPORT_IS_VXLAN_PORT(gport)) {
                if (SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm,
                        VXLAN__DISABLE_VP_PRUNINGf)) {
                    field = VXLAN__DISABLE_VP_PRUNINGf;
                }
            } else if (BCM_GPORT_IS_L2GRE_PORT(gport)) {
                if (SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm,
                        VXLAN__DISABLE_VP_PRUNINGf)) {
                    field = L2GRE__DISABLE_VP_PRUNINGf;
                }
            } else if (BCM_GPORT_IS_TRILL_PORT(gport)) {
                if (SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm,
                        VXLAN__DISABLE_VP_PRUNINGf)) {
                    field = TRILL__DISABLE_VP_PRUNINGf;
                }
            }
#endif /* BCM_TRIDENT2_SUPPORT */

            if (SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm, field)) {
                SOC_IF_ERROR_RETURN
                    (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
                soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &dvp_entry,
                        field, value ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (WRITE_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ALL, vp, &dvp_entry));
                rv = BCM_E_NONE;
            } 
            break;

        case bcmPortControlIP4:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, IPV4L3_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                        IPV4L3_ENABLEf, value ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

        case bcmPortControlIP6:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, IPV6L3_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                        IPV6L3_ENABLEf, value ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

        case bcmPortControlIP4Mcast:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, IPMCV4_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                        IPMCV4_ENABLEf, value ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

        case bcmPortControlIP6Mcast:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, IPMCV6_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                        IPMCV6_ENABLEf, value ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

        case bcmPortControlIP4McastL2:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, IPMCV4_L2_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                        IPMCV4_L2_ENABLEf, value ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

        case bcmPortControlIP6McastL2:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, IPMCV6_L2_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                        IPMCV6_L2_ENABLEf, value ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

        case bcmPortControlL3Ingress:
#ifdef BCM_TRIDENT2_SUPPORT
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_gport_validate(unit, port, &port));
            if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm, L3_IIFf)) {
                if ((value >= 1) && (value < soc_mem_index_count(unit, L3_IIFm))) {
                    rv = _bcm_trx_source_trunk_map_set(unit, port, L3_IIFf, value);
                    BCM_IF_ERROR_RETURN(rv);
                    if (soc_mem_field_valid(unit, PORT_TABm, PORT_OPERATIONf)) {
                        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                PORT_OPERATIONf, 0x2);
                    }
                } else {
                    if (soc_mem_field_valid(unit, PORT_TABm, PORT_OPERATIONf)) {
                        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                PORT_OPERATIONf, 0x0);
                    }
                }
            } 
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

#ifdef BCM_TRIDENT2_SUPPORT
        case bcmPortControlFcoeRouteEnable:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, FCOE_ROUTE_ENABLEf)) {
                    SOC_IF_ERROR_RETURN(
                        READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                    soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                                               FCOE_ROUTE_ENABLEf, 
                                               (value) ? 1 : 0);
                    SOC_IF_ERROR_RETURN(
                        WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                    rv = BCM_E_NONE;
                }     
                if (SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm, 
                                        FCOE_ROUTE_ENABLEf)) {
                    SOC_IF_ERROR_RETURN(
                        READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, 
                                                &dvp_entry));
                    soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &dvp_entry,
                                                       FCOE_ROUTE_ENABLEf, 
                                                       (value) ? 1 : 0);
                    SOC_IF_ERROR_RETURN(
                        WRITE_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ALL, vp, 
                                                 &dvp_entry));
                    rv = BCM_E_NONE;
                }
            }
            break;

        case bcmPortControlFcoeVftEnable:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, FCOE_VFT_ENABLEf)) {
                    SOC_IF_ERROR_RETURN(
                        READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                    soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                                               FCOE_VFT_ENABLEf, 
                                               (value) ? 1 : 0);
                    SOC_IF_ERROR_RETURN(
                        WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                    rv = BCM_E_NONE;
                }     
            }
            break;

        case bcmPortControlFcoeZoneCheckEnable:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, 
                                        FCOE_ZONE_CHECK_ENABLEf)) {
                    SOC_IF_ERROR_RETURN(
                        READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                    soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                                               FCOE_ZONE_CHECK_ENABLEf, 
                                               (value) ? 1 : 0);
                    SOC_IF_ERROR_RETURN(
                        WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                    rv = BCM_E_NONE;
                }     
            }
            break;

        case bcmPortControlFcoeSourceBindCheckEnable:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, 
                                        FCOE_SRC_BIND_CHECK_ENABLEf)) {
                    SOC_IF_ERROR_RETURN(
                        READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                    soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                                               FCOE_SRC_BIND_CHECK_ENABLEf, 
                                               (value) ? 1 : 0);
                    SOC_IF_ERROR_RETURN(
                        WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                    rv = BCM_E_NONE;
                }     
            }
            break;

        case bcmPortControlFcoeFpmaPrefixCheckEnable:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm,
                                        FCOE_SRC_FPMA_PREFIX_CHECK_ENABLEf)) {
                    SOC_IF_ERROR_RETURN(
                        READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                    soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                                            FCOE_SRC_FPMA_PREFIX_CHECK_ENABLEf, 
                                            (value) ? 1 : 0);
                    SOC_IF_ERROR_RETURN(
                        WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                    rv = BCM_E_NONE;
                }     
            }
            break;

        case bcmPortControlFcoeDoNotLearn:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, FCOE_DO_NOT_LEARNf)) {
                    SOC_IF_ERROR_RETURN(
                        READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                    soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                                               FCOE_DO_NOT_LEARNf, 
                                               (value) ? 1 : 0);
                    SOC_IF_ERROR_RETURN(
                        WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                    rv = BCM_E_NONE;
                }     
            }
            break;

        case bcmPortControlFcoeNetworkPort:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, FCOE_NETWORK_PORTf)) {
                    SOC_IF_ERROR_RETURN(
                        READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                    soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                                               FCOE_NETWORK_PORTf, 
                                               (value) ? 1 : 0);
                    SOC_IF_ERROR_RETURN(
                        WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                    rv = BCM_E_NONE;
                }     
            }
            break;

#endif /* BCM_TRIDENT2_SUPPORT */

        case bcmPortControlMcastGroupRemap:
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, NETWORK_PORTf)) {
                rv = soc_mem_field32_modify(unit, SOURCE_VPm, vp,
                        NETWORK_PORTf, value ? 1 : 0);
            }
            break;

        case bcmPortControlVlanVpGroupIngress:
            if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
                int field_width;
                int index = vp;
                soc_mem_t mem = SOURCE_VPm;
                soc_field_t field = VLAN_MEMBERSHIP_PROFILEf;
#ifdef BCM_HGPROXY_COE_SUPPORT
                if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                    (soc_feature(unit, soc_feature_channelized_switching)))  &&
                    _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
                    int portid = -1;
                    int modid = -1;
                    int trunk = -1;
                    int id = -1;
                    mem = SOURCE_TRUNK_MAP_TABLEm;

                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_gport_resolve(unit, gport, &modid, &portid,
                                               &trunk, &id));
                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_src_mod_port_table_index_get(
                            unit, modid, portid, &index));
                }
#endif
                field_width = soc_mem_field_length(unit, mem, field);
                if ((value >= (1 << field_width)) ||
                    (value < 0)) {
                    return BCM_E_PARAM;
                }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
                    rv = bcm_td2p_ing_vp_group_unmanaged_set(unit,TRUE);
                } else
#endif
                {
                    rv = bcm_td_ing_vp_group_unmanaged_set(unit,TRUE);
                }
                if (rv == BCM_E_NONE) {
                    SOC_IF_ERROR_RETURN(
                        soc_mem_field32_modify(unit, mem, index,
                                               field, value));
                }
            }
            break;

        case bcmPortControlVlanVpGroupEgress:
            if (soc_feature(unit,soc_feature_vp_group_egress_vlan_membership)) {
                int field_width;
                int index = vp;
                soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
                soc_field_t field;
#if defined(BCM_KATANA2_SUPPORT)
                if(SOC_IS_SABER2(unit)) {
                    field = VLAN_MEMBERSHIP_PROFILEf;
                } else 
#endif
                {
                    field = COMMON__VLAN_MEMBERSHIP_PROFILEf;
                }
#ifdef BCM_HGPROXY_COE_SUPPORT
                if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                    (soc_feature(unit, soc_feature_channelized_switching)))  &&
                    _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
                    int portid = -1;
                    int modid = -1;
                    int trunk = -1;
                    int id = -1;
                    mem = EGR_GPP_ATTRIBUTESm;
                    field = VLAN_MEMBERSHIP_PROFILEf;

                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_gport_resolve(unit, gport, &modid, &portid,
                                               &trunk, &id));
                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_src_mod_port_table_index_get(
                            unit, modid, portid, &index));
                }
#endif
                field_width = soc_mem_field_length(unit, mem, field);
                if ((value >= (1 << field_width)) ||
                    (value < 0)) {
                    return BCM_E_PARAM;
                }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
                    rv = bcm_td2p_egr_vp_group_unmanaged_set(unit,TRUE);
                } else
#endif
                {
                    rv = bcm_td_egr_vp_group_unmanaged_set(unit,TRUE);
                }

                if (rv == BCM_E_NONE) {
                    if (mem == EGR_DVP_ATTRIBUTEm) {
                        SOC_IF_ERROR_RETURN(
                            READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY,
                                                    index, &dvp_entry));
                        BCM_IF_ERROR_RETURN(
                            _td_egr_dvp_attribute_field_name_get(
                                unit, &dvp_entry,
                                VLAN_MEMBERSHIP_PROFILEf, &field));
                        soc_EGR_DVP_ATTRIBUTEm_field32_set(
                            unit, &dvp_entry, field, value);
                        SOC_IF_ERROR_RETURN(
                            WRITE_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ALL,
                                                     index, &dvp_entry));
                    } else {
                        SOC_IF_ERROR_RETURN(
                            soc_mem_field32_modify(unit, mem, index,
                                                   field, value));
                    }
                }
            }
            break;

        case bcmPortControlL2Learn:
            if (!(value & BCM_PORT_LEARN_FWD)) {
               cml |= (1 << 0);
            }
            if (value & BCM_PORT_LEARN_CPU) {
               cml |= (1 << 1);
            }
            if (value & BCM_PORT_LEARN_PENDING) {
               cml |= (1 << 2);
            }
            if (value & BCM_PORT_LEARN_ARL) {
               cml |= (1 << 3);
            }

            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, CML_FLAGS_NEWf)) {
                SOC_IF_ERROR_RETURN(
                    READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                    CML_FLAGS_NEWf, cml);
                SOC_IF_ERROR_RETURN(
                    WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                rv = BCM_E_NONE;
            }
            break;

        case bcmPortControlL2Move:
            if (!(value & BCM_PORT_LEARN_FWD)) {
               cml |= (1 << 0);
            }
            if (value & BCM_PORT_LEARN_CPU) {
               cml |= (1 << 1);
            }
            if (value & BCM_PORT_LEARN_PENDING) {
               cml |= (1 << 2);
            }
            if (value & BCM_PORT_LEARN_ARL) {
               cml |= (1 << 3);
            }

            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, CML_FLAGS_MOVEf)) {
                SOC_IF_ERROR_RETURN(
                    READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                    CML_FLAGS_MOVEf, cml);
                SOC_IF_ERROR_RETURN(
                    WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
                rv = BCM_E_NONE;
            }
            break;

        case bcmPortControlDoNotCheckVlan:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, DISABLE_VLAN_CHECKSf)) {
                SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, SOURCE_VPm, vp,
                       DISABLE_VLAN_CHECKSf, value ? 1 : 0));
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;
        case bcmPortControlDoNotCheckVlanEgress:
#if defined(BCM_TRIDENT2_SUPPORT)
            BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));

            field = COMMON__DISABLE_VLAN_CHECKf;
            if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
                if (BCM_GPORT_IS_TRILL_PORT(gport)) {
                    field = TRILL__DISABLE_VLAN_CHECKf;
                } else if (BCM_GPORT_IS_VXLAN_PORT(gport)) {
                    field = VXLAN__DISABLE_VLAN_CHECKf;
                } else if (BCM_GPORT_IS_L2GRE_PORT(gport)) {
                    field = L2GRE__DISABLE_VLAN_CHECKf;
#if defined(BCM_TRIDENT3_SUPPORT)
                } else if (BCM_GPORT_IS_FLOW_PORT(gport)) {
                    uint32 data_type;

                    SOC_IF_ERROR_RETURN(READ_EGR_DVP_ATTRIBUTEm(unit,
                          MEM_BLOCK_ANY, vp, &dvp_entry));
                    data_type = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                                 &dvp_entry, DATA_TYPEf);
                    if (data_type == _BCM_FLOW_VXLAN_EGRESS_DEST_VP_TYPE) {
                        field = VXLAN__DISABLE_VLAN_CHECKf;
                    } else if (data_type ==
                                   _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE) {
                        field = L2GRE__DISABLE_VLAN_CHECKf;
                    } else if (data_type >
                                _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE) {
                        return BCM_E_UNAVAIL;
                    }
#endif
                }
            }
            if (SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm, field)) {
                SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
                                    EGR_DVP_ATTRIBUTEm, vp, field,
                                    value ? 1 : 0));
                rv = BCM_E_NONE;
            }
#endif
            break;
        case bcmPortControlDvpGroupKnockoutEnable:
#if defined(BCM_KATANA2_SUPPORT)
            if (soc_feature(unit, soc_feature_dvp_group_pruning)) {
                SOC_IF_ERROR_RETURN
                    (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
                soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &dvp_entry,
                        ENABLE_VP_GROUP_PRUNINGf, value ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (WRITE_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ALL, vp, &dvp_entry));
                rv = BCM_E_NONE;
            }
#endif
            break;
        case bcmPortControlDvpGroupId:
#if defined(BCM_KATANA2_SUPPORT)
            if (soc_feature(unit, soc_feature_dvp_group_pruning)) {
                /* coverity[negative_returns: FALSE] */
                rv = _bcm_td_dvp_group_set(unit, gport, vp, value);
            }
#endif
            break;
#if defined(BCM_KATANA2_SUPPORT)
        case bcmPortControlOamLookupWithDvp:
            if (SOC_IS_KATANA2(unit)) {
                rv = _bcm_kt2_port_control_oam_loopkup_with_dvp_set(unit,
                        gport, value);
            }
            break;
#endif
        case bcmPortControlTpidSrc:
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, TPID_SOURCEf)) {
                switch(value) {
                    case bcmPortTpidSrcVp:
                        SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, SOURCE_VPm, vp,
                                            TPID_SOURCEf, 0));
                        rv = BCM_E_NONE;
                        break;
                    case bcmPortTpidSrcVpn:
                        SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, SOURCE_VPm, vp,
                                            TPID_SOURCEf, 1));
                        rv = BCM_E_NONE;
                        break;
                    case bcmPortTpidSrcPort:
                        SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, SOURCE_VPm, vp,
                                            TPID_SOURCEf, 2));
                        rv = BCM_E_NONE;
                        break;
                    case bcmPortTpidSrcTrillPayload:
                        SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, SOURCE_VPm, vp,
                                            TPID_SOURCEf, 3));
                        rv = BCM_E_NONE;
                        break;
                    default:
                        rv = BCM_E_PARAM;
                        break;
                }
            }
            break;
        default:
            break;
    }

    return rv;
}

/*
 * Function:
 *      bcm_td_vp_control_get
 * Purpose:
 *      Get per virtual port controls.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN) VP gport number
 *      type - (IN) Control type
 *      value - (OUT) Current value 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td_vp_control_get(int unit, bcm_gport_t gport,
                       bcm_port_control_t type, int *value)
{
    int rv;
    int vp = -1;
#ifdef BCM_TRIDENT2_SUPPORT
    bcm_niv_egress_t niv_egress;
    int              count;
    bcm_gport_t port = BCM_GPORT_INVALID;
    soc_field_t      field;
    bcm_extender_egress_t extender_egress;
    _bcm_vp_info_t vp_info;
#endif
    bcm_niv_port_t niv_port;
    bcm_vlan_port_t vlan_port;
    bcm_extender_port_t ext_port;
    source_vp_entry_t svp_entry;
    uint32 flags=0, val=0;
    egr_dvp_attribute_entry_t dvp_entry;

    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
            return BCM_E_NOT_FOUND;
        }
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vlan_port_id = gport;
        BCM_IF_ERROR_RETURN(
            bcm_esw_vlan_port_find(unit, &vlan_port));
#ifdef BCM_TRIDENT2_SUPPORT
        port = vlan_port.port;
#endif
    } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
            return BCM_E_NOT_FOUND;
        }
        bcm_niv_port_t_init(&niv_port);
        niv_port.niv_port_id = gport;
        BCM_IF_ERROR_RETURN(
            bcm_esw_niv_port_get(unit, &niv_port));
#ifdef BCM_TRIDENT2_SUPPORT
        if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
            port = BCM_GPORT_INVALID;
            bcm_niv_egress_t_init(&niv_egress);
            rv = bcm_trident_niv_egress_get(unit, niv_port.niv_port_id,
                                            1, &niv_egress, &count);
            if (BCM_SUCCESS(rv)) {
                if (!(niv_egress.flags & BCM_NIV_EGRESS_MULTICAST)) {
                    port = niv_egress.port;
                } 
            }
        } else {
            port = niv_port.port;
        }
#endif
    } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
            return BCM_E_NOT_FOUND;
        }
        bcm_extender_port_t_init(&ext_port);
        ext_port.extender_port_id = gport;
        BCM_IF_ERROR_RETURN(
            bcm_esw_extender_port_get(unit, &ext_port));
#ifdef BCM_TRIDENT2_SUPPORT
                if (ext_port.flags & BCM_EXTENDER_PORT_MATCH_NONE) {
                    bcm_extender_egress_t_init(&extender_egress);
                    BCM_IF_ERROR_RETURN(bcm_esw_extender_egress_get_all(unit,
                                ext_port.extender_port_id, 1, &extender_egress, &count));
                    if (count == 0) {
                        /* No Extender egress object has been added to VP yet. */
                        return BCM_E_CONFIG;
                    }
                    if (extender_egress.flags & BCM_EXTENDER_EGRESS_MULTICAST) {
                        return BCM_E_PARAM;
                    }
                    port = extender_egress.port;
                } else {
                    port = ext_port.port;
                }
#endif

    } else if (BCM_GPORT_IS_TRUNK(gport)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_tid_to_vp_lag_vp(unit, 
                            BCM_GPORT_TRUNK_GET(gport), &vp));
#ifdef BCM_TRIDENT2_SUPPORT
        port = BCM_GPORT_INVALID;
#endif 
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
	} else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            return BCM_E_PARAM;
        }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
        port = BCM_GPORT_INVALID;
    } else if (BCM_GPORT_IS_VXLAN_PORT(gport)) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
            return BCM_E_PARAM;
        }
        port = BCM_GPORT_INVALID;
    } else if (BCM_GPORT_IS_MIM_PORT(gport)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
            return BCM_E_PARAM;
        }
        port = BCM_GPORT_INVALID;
    } else if (BCM_GPORT_IS_L2GRE_PORT(gport)) {
        vp = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
            return BCM_E_PARAM;
        }
        port = BCM_GPORT_INVALID;
#ifdef BCM_TRIDENT3_SUPPORT
    } else if (BCM_GPORT_IS_FLOW_PORT(gport)) {
        vp = BCM_GPORT_FLOW_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
            return BCM_E_PARAM;
        }
        port = BCM_GPORT_INVALID;
#endif
#endif
#ifdef BCM_HGPROXY_COE_SUPPORT
    } else if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
#endif
    } else {
        return BCM_E_PARAM;
    }

    rv = BCM_E_UNAVAIL;

    switch (type) {
        case bcmPortControlBridge:
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, DISABLE_VP_PRUNINGf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                        DISABLE_VP_PRUNINGf);
                rv = BCM_E_NONE;
            } 
            break;

        case bcmPortControlIP4:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, IPV4L3_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                    IPV4L3_ENABLEf);
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

        case bcmPortControlIP6:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, IPV6L3_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                    IPV6L3_ENABLEf);
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

        case bcmPortControlIP4Mcast:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, IPMCV4_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                    IPMCV4_ENABLEf);
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

        case bcmPortControlIP6Mcast:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, IPMCV6_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                    IPMCV6_ENABLEf);
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

        case bcmPortControlIP4McastL2:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, IPMCV4_L2_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                    IPMCV4_L2_ENABLEf);
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

        case bcmPortControlIP6McastL2:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, IPMCV6_L2_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                    IPMCV6_L2_ENABLEf);
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

        case bcmPortControlL3Ingress:
#ifdef BCM_TRIDENT2_SUPPORT
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_gport_validate(unit, port, &port));
            if (soc_mem_field_valid(unit, PORT_TABm, PORT_OPERATIONf)) {
                rv = _bcm_esw_port_tab_get(unit, port, PORT_OPERATIONf, value);
                BCM_IF_ERROR_RETURN(rv);
                if ((*value == 0x2) && 
                    (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm, L3_IIFf)) ) {
                    rv = _bcm_trx_source_trunk_map_get(unit, port, L3_IIFf, (uint32 *)value);
                } 
            } 
#endif /* BCM_TRIDENT2_SUPPORT */
            break;

#ifdef BCM_TRIDENT2_SUPPORT
        case bcmPortControlFcoeRouteEnable:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, FCOE_ROUTE_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                    FCOE_ROUTE_ENABLEf);
                rv = BCM_E_NONE;
                }     
            }
            break;

        case bcmPortControlFcoeVftEnable:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, FCOE_VFT_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                    FCOE_VFT_ENABLEf);
                rv = BCM_E_NONE;
                }     
            }
            break;

        case bcmPortControlFcoeZoneCheckEnable:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, 
                                        FCOE_ZONE_CHECK_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                    FCOE_ZONE_CHECK_ENABLEf);
                rv = BCM_E_NONE;
                }     
            }
            break;

        case bcmPortControlFcoeSourceBindCheckEnable:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, 
                                        FCOE_SRC_BIND_CHECK_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(
                                                unit, &svp_entry,
                                                FCOE_SRC_BIND_CHECK_ENABLEf);
                rv = BCM_E_NONE;
                }     
            }
            break;

        case bcmPortControlFcoeFpmaPrefixCheckEnable:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, 
                                        FCOE_SRC_FPMA_PREFIX_CHECK_ENABLEf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(
                                            unit, &svp_entry,
                                            FCOE_SRC_FPMA_PREFIX_CHECK_ENABLEf);
                rv = BCM_E_NONE;
                }     
            }
            break;

        case bcmPortControlFcoeDoNotLearn:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, FCOE_DO_NOT_LEARNf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                    FCOE_DO_NOT_LEARNf);
                rv = BCM_E_NONE;
                }     
            }
            break;

        case bcmPortControlFcoeNetworkPort:
            if (soc_feature(unit, soc_feature_fcoe)) {
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, FCOE_NETWORK_PORTf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                    FCOE_NETWORK_PORTf);
                rv = BCM_E_NONE;
                }     
            }
            break;

#endif /* BCM_TRIDENT2_SUPPORT */


        case bcmPortControlMcastGroupRemap:
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, NETWORK_PORTf)) {
                SOC_IF_ERROR_RETURN
                    (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                    NETWORK_PORTf);
                rv = BCM_E_NONE;
            }
            break;

        case bcmPortControlVlanVpGroupIngress:
            if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
                int index = vp;
                soc_mem_t mem = SOURCE_VPm;
                soc_field_t field = VLAN_MEMBERSHIP_PROFILEf;
                uint32 buf[SOC_MAX_MEM_FIELD_WORDS];

#ifdef BCM_HGPROXY_COE_SUPPORT
                if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                    (soc_feature(unit, soc_feature_channelized_switching)))  &&
                    _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
                    int portid = -1;
                    int modid = -1;
                    int trunk = -1;
                    int id = -1;
                    mem = SOURCE_TRUNK_MAP_TABLEm;

                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_gport_resolve(unit, gport, &modid, &portid,
                                               &trunk, &id));
                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_src_mod_port_table_index_get(
                            unit, modid, portid, &index));
                }
#endif
                SOC_IF_ERROR_RETURN(
                    soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, buf));
                *value = soc_mem_field32_get(unit, mem, buf, field);

                rv = BCM_E_NONE;
            }
            break;

        case bcmPortControlVlanVpGroupEgress:
            if (soc_feature(unit,soc_feature_vp_group_egress_vlan_membership)) {
                int index = vp;
                soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
                soc_field_t field = COMMON__VLAN_MEMBERSHIP_PROFILEf;
                uint32 buf[SOC_MAX_MEM_FIELD_WORDS];

#ifdef BCM_HGPROXY_COE_SUPPORT
                if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                    (soc_feature(unit, soc_feature_channelized_switching)))  &&
                    _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
                    int portid = -1;
                    int modid = -1;
                    int trunk = -1;
                    int id = -1;
                    mem = EGR_GPP_ATTRIBUTESm;
                    field = VLAN_MEMBERSHIP_PROFILEf;

                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_gport_resolve(unit, gport, &modid, &portid,
                                               &trunk, &id));
                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_src_mod_port_table_index_get(
                            unit, modid, portid, &index));
                }
#endif
                if (mem == EGR_DVP_ATTRIBUTEm) {
                    SOC_IF_ERROR_RETURN(
                        READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY,
                                                index, &dvp_entry));
                    BCM_IF_ERROR_RETURN(
                        _td_egr_dvp_attribute_field_name_get(
                            unit, &dvp_entry,
                            VLAN_MEMBERSHIP_PROFILEf, &field));
                    *value = soc_EGR_DVP_ATTRIBUTEm_field32_get(
                                 unit, &dvp_entry, field);
                } else {
                    SOC_IF_ERROR_RETURN(
                        soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, buf));
                    *value = soc_mem_field32_get(unit, mem, buf, field);
                }
                rv = BCM_E_NONE;
            }
            break;

        case bcmPortControlL2Learn:
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, CML_FLAGS_NEWf)) {
                SOC_IF_ERROR_RETURN(
                    READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                val = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                 CML_FLAGS_NEWf);

                if (!(val & (1 << 0))) {
                   flags |= BCM_PORT_LEARN_FWD;
                }
                if (val & (1 << 1)) {
                   flags |= BCM_PORT_LEARN_CPU;
                }
                if (val & (1 << 2)) {
                   flags |= BCM_PORT_LEARN_PENDING;
                }
                if (val & (1 << 3)) {
                   flags |= BCM_PORT_LEARN_ARL;
                }

                *value = flags;
                rv = BCM_E_NONE;
            }
            break;

        case bcmPortControlL2Move:
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, CML_FLAGS_MOVEf)) {
                SOC_IF_ERROR_RETURN(
                    READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                val = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                 CML_FLAGS_MOVEf);
                if (!(val & (1 << 0))) {
                   flags |= BCM_PORT_LEARN_FWD;
                }
                if (val & (1 << 1)) {
                   flags |= BCM_PORT_LEARN_CPU;
                }
                if (val & (1 << 2)) {
                   flags |= BCM_PORT_LEARN_PENDING;
                }
                if (val & (1 << 3)) {
                   flags |= BCM_PORT_LEARN_ARL;
                }

                *value = flags;
                rv = BCM_E_NONE;
            }
            break;
        case bcmPortControlDoNotCheckVlan:
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, DISABLE_VLAN_CHECKSf)) {
                SOC_IF_ERROR_RETURN(
                    READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(
                    unit, &svp_entry, DISABLE_VLAN_CHECKSf);
                rv = BCM_E_NONE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            break;
        case bcmPortControlDoNotCheckVlanEgress:
#if defined(BCM_TRIDENT2_SUPPORT)
            BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
            field = COMMON__DISABLE_VLAN_CHECKf;
            if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
                if (BCM_GPORT_IS_TRILL_PORT(gport)) {
                    field = TRILL__DISABLE_VLAN_CHECKf;
                } else if (BCM_GPORT_IS_VXLAN_PORT(gport)) {
                    field = VXLAN__DISABLE_VLAN_CHECKf;
                } else if (BCM_GPORT_IS_L2GRE_PORT(gport)) {
                    field = L2GRE__DISABLE_VLAN_CHECKf;
#if defined(BCM_TRIDENT3_SUPPORT)
                } else if (BCM_GPORT_IS_FLOW_PORT(gport)) {
                    uint32 data_type;

                    SOC_IF_ERROR_RETURN(READ_EGR_DVP_ATTRIBUTEm(unit,
                          MEM_BLOCK_ANY, vp, &dvp_entry));
                    data_type = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm,
                                 &dvp_entry, DATA_TYPEf);
                    if (data_type == _BCM_FLOW_VXLAN_EGRESS_DEST_VP_TYPE) {
                        field = VXLAN__DISABLE_VLAN_CHECKf;
                    } else if (data_type ==
                                    _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE) {
                        field = L2GRE__DISABLE_VLAN_CHECKf;
                    } else if (data_type >
                                     _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE) {
                        return BCM_E_UNAVAIL;
                    }
#endif
                }
            }

            if (SOC_MEM_FIELD_VALID(unit, EGR_DVP_ATTRIBUTEm, field)) {
                SOC_IF_ERROR_RETURN(READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY,
                                    vp, &dvp_entry));
                *value = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &dvp_entry,
                                                            field);
                rv = BCM_E_NONE;
            }
#endif
            break;
        case bcmPortControlDvpGroupKnockoutEnable:
#if defined(BCM_KATANA2_SUPPORT)
            if (soc_feature(unit, soc_feature_dvp_group_pruning)) {
                SOC_IF_ERROR_RETURN
                    (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
                *value = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &dvp_entry,
                        ENABLE_VP_GROUP_PRUNINGf);
                
                rv = BCM_E_NONE;
            }
#endif
            break;
        case bcmPortControlDvpGroupId:
#if defined(BCM_KATANA2_SUPPORT)
            if (soc_feature(unit, soc_feature_dvp_group_pruning)) {
                /* coverity[negative_returns: FALSE] */
                rv = _bcm_td_dvp_group_get(unit, gport, vp, value);
            }
#endif
            break;
#if defined(BCM_KATANA2_SUPPORT)
        case bcmPortControlOamLookupWithDvp:
            if (SOC_IS_KATANA2(unit)) {
                rv = _bcm_kt2_port_control_oam_loopkup_with_dvp_get(unit, gport,
                        value);
            }
            break;
#endif
        case bcmPortControlTpidSrc:
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_VPm, TPID_SOURCEf)) {
                SOC_IF_ERROR_RETURN(
                    READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
                *value = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                                                    TPID_SOURCEf);
                switch(*value) {
                    case 0:
                        *value = bcmPortTpidSrcVp;
                        break;
                    case 1:
                        *value = bcmPortTpidSrcVpn;
                        break;
                    case 2:
                        *value = bcmPortTpidSrcPort;
                        break;
                    case 3:
                        *value = bcmPortTpidSrcTrillPayload;
                        break;
                    default:
                        return BCM_E_INTERNAL;
                }
                rv = BCM_E_NONE;
            }
            break;
        default:
            break;
    }

    return rv;
}

/*
 * Function:
 *      bcm_td_vp_force_vlan_set
 * Purpose:
 *      configure private vlan for virtual port.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN) VP gport number
 *      value - (OUT) Current value
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td_vp_force_vlan_set(int unit, bcm_gport_t gport,
                       bcm_vlan_t vlan, uint32 flags)
{
    int vp;
    source_vp_entry_t svp_entry;
    ing_dvp_table_entry_t dvp_entry;
    soc_mem_t mem;
    int rv;
    int port_type = -1;

    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_PARAM;
    }

    if (flags & BCM_PORT_FORCE_VLAN_UNTAG) {
        return BCM_E_PARAM;
    }

    mem = SOURCE_VPm;
    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, vp, &svp_entry);
    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return (rv);
    }

    soc_mem_field32_set(unit, mem, &svp_entry, EXP_PVLAN_VIDf,vlan);
    soc_mem_field32_set(unit, mem, &svp_entry, PVLAN_ENABLEf,
          flags & BCM_PORT_FORCE_VLAN_ENABLE? 1: 0);
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vp, &svp_entry);
    soc_mem_unlock(unit, mem);
    SOC_IF_ERROR_RETURN(rv);

    if ((flags & BCM_PORT_FORCE_VLAN_PORT_TYPE_MASK) == 
                         BCM_PORT_FORCE_VLAN_PROMISCUOUS_PORT) {
        port_type = 0;
    } else if ((flags & BCM_PORT_FORCE_VLAN_PORT_TYPE_MASK) ==
                         BCM_PORT_FORCE_VLAN_ISOLATED_PORT) {
        port_type = 1;
    } else if ((flags & BCM_PORT_FORCE_VLAN_PORT_TYPE_MASK) ==
                         BCM_PORT_FORCE_VLAN_COMMUNITY_PORT) {
        port_type = 2;
    } 

    if (port_type != -1) { 
        rv = soc_mem_field32_modify(unit, VLAN_TABm, vlan, 
                    SRC_PVLAN_PORT_TYPEf,port_type);
        SOC_IF_ERROR_RETURN(rv);
    }

    mem = ING_DVP_TABLEm;
    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, vp, &dvp_entry);
    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return (rv);
    }

    soc_mem_field32_set(unit, mem, &dvp_entry, PVLAN_VIDf,vlan);
    if (port_type != -1) { 
        soc_mem_field32_set(unit, mem, &dvp_entry, DST_PVLAN_PORT_TYPEf,
                        port_type);
    }
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vp, &dvp_entry);
    soc_mem_unlock(unit, mem);
    return rv;
}

/*
 * Function:
 *      bcm_td_vp_force_vlan_get
 * Purpose:
 *      get private vlan information for a given virtual port.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN) VP gport number
 *      value - (OUT) Current value
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td_vp_force_vlan_get(int unit, bcm_gport_t gport,
                       bcm_vlan_t *vlan, uint32 *flags)
{
    int vp;
    source_vp_entry_t svp_entry;
    ing_dvp_table_entry_t dvp_entry;
    int rv;
    int port_type;

    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_PARAM;
    }

    *flags = 0;
    rv = soc_mem_read(unit, SOURCE_VPm, MEM_BLOCK_ANY, vp, &svp_entry);
    SOC_IF_ERROR_RETURN(rv);

    *vlan = soc_mem_field32_get(unit, SOURCE_VPm, &svp_entry, EXP_PVLAN_VIDf);
    if (soc_mem_field32_get(unit, SOURCE_VPm, &svp_entry, PVLAN_ENABLEf)) {
        *flags = BCM_PORT_FORCE_VLAN_ENABLE;
    }

    rv = soc_mem_read(unit, ING_DVP_TABLEm, MEM_BLOCK_ANY, vp, &dvp_entry);
    SOC_IF_ERROR_RETURN(rv);

    port_type = soc_mem_field32_get(unit, ING_DVP_TABLEm, &dvp_entry, 
                         DST_PVLAN_PORT_TYPEf);
    if (port_type == 0) {
        *flags |= BCM_PORT_FORCE_VLAN_PROMISCUOUS_PORT;
    } else if (port_type == 1) {
        *flags |= BCM_PORT_FORCE_VLAN_ISOLATED_PORT;
    } else if (port_type == 2) {
        *flags |= BCM_PORT_FORCE_VLAN_COMMUNITY_PORT;
    }
    return rv;
}
#endif /* INCLUDE_L3 */
/*
 * Function:
 *     bcm_td_port_drain_cells
 * Purpose:
 *     To drain cells associated to the port.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) Port
 * Returns:
 *     BCM_E_XXX
 */
int bcm_td_port_drain_cells(int unit, int port)
{
    mac_driver_t *macd;
    int rv = BCM_E_NONE;

    PORT_LOCK(unit);
    rv = soc_mac_probe(unit, port, &macd);

    if (BCM_SUCCESS(rv)) {
        rv = MAC_CONTROL_SET(macd, unit, port, SOC_MAC_CONTROL_EGRESS_DRAIN, 1);
    }
    PORT_UNLOCK(unit);
    return rv;
}

#endif /* BCM_TRIDENT_SUPPORT */
