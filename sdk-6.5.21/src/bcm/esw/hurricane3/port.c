/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/defs.h>
#include <assert.h>
#include <sal/core/libc.h>

#ifdef BCM_HURRICANE3_SUPPORT

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
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw_dispatch.h>

#include <bcm_int/common/lock.h>

#define _HR3_ENTRIES_PER_PORFILE (64)

STATIC soc_profile_mem_t *_bcm_hr3_egr_mask_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_hr3_sys_cfg_profile[BCM_MAX_NUM_UNITS];

int
bcm_hr3_port_reinit(int unit)
{
    soc_profile_mem_t *profile;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 profile_index;
    bcm_module_t modid;
    bcm_port_t port;
    system_config_table_entry_t *sys_cfg_entry;
    int tpid_enable, tpid_index;
    int is_local;

    /* EGR_MASK profile */
    profile = _bcm_hr3_egr_mask_profile[unit];
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

    /* SYSTEM_CONFIG_TABLE profile */
    profile = _bcm_hr3_sys_cfg_profile[unit];
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
            sys_cfg_entry =
                SOC_PROFILE_MEM_ENTRY(unit, profile,
                                      system_config_table_entry_t *,
                                      profile_index + port);
            tpid_enable =
                soc_SYSTEM_CONFIG_TABLEm_field32_get(unit, sys_cfg_entry,
                                                     OUTER_TPID_ENABLEf);
            for (tpid_index = 0; tpid_index < 4; tpid_index++) {
                if (tpid_enable & (1 << tpid_index)) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_fb2_outer_tpid_tab_ref_count_add(unit,
                                                              tpid_index, 1));
                }
            }
        }
    }

    return BCM_E_NONE;
}

int
bcm_hr3_port_init(int unit)
{
    soc_mem_t mem;
    int entry_words;
    union {
#ifdef BCM_GREYHOUND2_SUPPORT
        /*
         * GH2 support max port_num=66 (HR3's max port_num=64).
         *  - change the array size below from 64 to 128 for related profile
         *    memory init.
         */
        egr_mask_entry_t egr_mask[128];
        system_config_table_entry_t sys_cfg[128];
#else
        egr_mask_entry_t egr_mask[_HR3_ENTRIES_PER_PORFILE];
        system_config_table_entry_t sys_cfg[_HR3_ENTRIES_PER_PORFILE];
#endif
        uint32 w[1];
    } entry;
    void *entries[1];
    int modid, port, index;
    uint32 profile_index;
    uint16 tpid;
    bcm_vlan_action_set_t action;

    /* Create profile for EGR_MASK table */
    if (_bcm_hr3_egr_mask_profile[unit] == NULL) {
        _bcm_hr3_egr_mask_profile[unit] =
            sal_alloc(sizeof(soc_profile_mem_t), "EGR_MASK profile");
        if (_bcm_hr3_egr_mask_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_hr3_egr_mask_profile[unit]);
    }
    mem = EGR_MASKm;
    entry_words = sizeof(egr_mask_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                _bcm_hr3_egr_mask_profile[unit]));

    /* Create profile for SYSTEM_CONFIG_TABLE table */
    if (_bcm_hr3_sys_cfg_profile[unit] == NULL) {
        _bcm_hr3_sys_cfg_profile[unit] =
            sal_alloc(sizeof(soc_profile_mem_t),
                      "SYSTEM_CONFIG_TABLE profile");
        if (_bcm_hr3_sys_cfg_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_hr3_sys_cfg_profile[unit]);
    }
    mem = SYSTEM_CONFIG_TABLEm;
    entry_words = sizeof(system_config_table_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                _bcm_hr3_sys_cfg_profile[unit]));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        return bcm_hr3_port_reinit(unit);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    entries[0] = &entry;

    /* Add default entries for EGR_MASK profile */
    sal_memset(entry.egr_mask, 0, sizeof(entry.egr_mask));
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_hr3_egr_mask_profile[unit], entries,
                             SOC_PORT_ADDR_MAX(unit) + 1, &profile_index));
    for (modid = 1; modid <= SOC_MODID_MAX(unit); modid++) {
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, _bcm_hr3_egr_mask_profile[unit],
                                       profile_index,
                                       SOC_PORT_ADDR_MAX(unit) + 1));
    }
    /* EGR_MASK_MODBASE should be 0 for all modid which should match with the
     * allocated default profile index */

    /* Add default entries for SYSTEM_CONFIG_TABLE profile */
    tpid = _bcm_fb2_outer_tpid_default_get(unit);
    BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_lkup(unit, tpid, &index));

    sal_memset(entry.sys_cfg, 0, sizeof(entry.sys_cfg));
    for (port = 0; port <= SOC_PORT_ADDR_MAX(unit); port++) {
        soc_mem_field32_set(unit, SYSTEM_CONFIG_TABLEm, &entry.sys_cfg[port],
                OUTER_TPID_ENABLEf, 1 << index);

        if (soc_feature(unit, soc_feature_inner_tpid_enable)) {
            soc_mem_field32_set(unit, SYSTEM_CONFIG_TABLEm,
                    &entry.sys_cfg[port], INNER_TPID_ENABLEf, 1);
        }
    }

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_hr3_sys_cfg_profile[unit], entries,
                             SOC_PORT_ADDR_MAX(unit) + 1, &profile_index));
    for (modid = 1; modid <= SOC_MODID_MAX(unit); modid++) {
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, _bcm_hr3_sys_cfg_profile[unit],
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

    if (soc_feature(unit, soc_feature_wh2)){
        /*
         * The config "pbmp_gport_stack" specifies the hl bitmap only in
         * soc_info_config. The rest settings on IP/EP and PGW are done here.
         */

        PBMP_PORT_ITER(unit, port) {
            if (IS_HL_PORT(unit, port)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_port_higig2_mode_set(unit, port, 1));
                SOC_HG2_ENABLED_PORT_ADD(unit, port);

                bcm_vlan_action_set_t_init(&action);
                BCM_IF_ERROR_RETURN
                    (_bcm_trx_vlan_port_egress_default_action_get(unit,
                                                                  port, &action));
                /* Backward compatible defaults */
                action.ot_outer = bcmVlanActionDelete;
                action.dt_outer = bcmVlanActionDelete;
                BCM_IF_ERROR_RETURN
                    (_bcm_trx_vlan_port_egress_default_action_set(unit,
                                                                  port, &action));
            }
        }
    }
    return BCM_E_NONE;
}

int
bcm_hr3_port_egress_set(int unit, bcm_port_t port, int modid, bcm_pbmp_t pbmp)
{
    soc_profile_mem_t *profile;
    egr_mask_modbase_entry_t base_entry;
    egr_mask_entry_t *mask_entries = NULL;
    int alloc_size = 0;
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

    profile = _bcm_hr3_egr_mask_profile[unit];

    BCM_PBMP_CLEAR(temp_pbmp_all);
    BCM_PBMP_ASSIGN(temp_pbmp_all, PBMP_PORT_ALL(unit));

    BCM_PBMP_NEGATE(mask_pbmp, pbmp);
    BCM_PBMP_ASSIGN(pbmp_all,PBMP_CMIC(unit));  /* CPU port */
    BCM_PBMP_OR(pbmp_all,temp_pbmp_all);
    BCM_PBMP_AND(mask_pbmp, pbmp_all);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, port, &modid_min, &port_min, &tid,
                                    &id));

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

    alloc_size = sizeof(egr_mask_entry_t) * (SOC_PORT_ADDR_MAX(unit) + 1);
    mask_entries = sal_alloc(alloc_size, "EGR_MASK entry buffer");
    sal_memset(mask_entries, 0, alloc_size);
    if (mask_entries == NULL) {
        return BCM_E_MEMORY;
    }
    entries[0] = mask_entries;

    soc_mem_lock(unit, EGR_MASK_MODBASEm);

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

        rv = soc_mem_read(unit, EGR_MASK_MODBASEm, MEM_BLOCK_ALL,
                egr_mask_modbase_index, &base_entry);
        if (BCM_FAILURE(rv)) {
            break;
        }
        old_index = soc_mem_field32_get(unit, EGR_MASK_MODBASEm, &base_entry,
                                        BASEf);
        rv = soc_profile_mem_get(unit, profile, old_index,
                                 SOC_PORT_ADDR_MAX(unit) + 1, entries);
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
        rv = soc_profile_mem_add(unit, profile, entries,
                                 SOC_PORT_ADDR_MAX(unit) + 1, &index);
        if (BCM_FAILURE(rv)) {
            break;
        }
        rv = soc_mem_field32_modify(unit, EGR_MASK_MODBASEm,
            egr_mask_modbase_index, BASEf, index);
        if (BCM_FAILURE(rv)) {
            break;
        }
        rv = soc_profile_mem_delete(unit, profile, old_index);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    soc_mem_unlock(unit, EGR_MASK_MODBASEm);

    sal_free(mask_entries);

    return rv;
}

int
bcm_hr3_port_egress_get(int unit, bcm_port_t port, int modid, bcm_pbmp_t *pbmp)
{
    soc_profile_mem_t *profile;
    egr_mask_modbase_entry_t base_entry;
    egr_mask_entry_t *mask_entries = NULL;
    int alloc_size = 0;
    int rv = BCM_E_NONE;
    void *entries[1];
    uint32 index;
    bcm_pbmp_t mask_pbmp, temp_pbmp_all;
    bcm_module_t local_modid;
    bcm_port_t local_port;
    bcm_trunk_t tid;
    int id;

    profile = _bcm_hr3_egr_mask_profile[unit];

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, port, &local_modid, &local_port,
                                    &tid, &id));

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

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, EGR_MASK_MODBASEm, MEM_BLOCK_ALL, local_modid,
                      &base_entry));
    index = soc_mem_field32_get(unit, EGR_MASK_MODBASEm, &base_entry, BASEf);
    alloc_size = sizeof(egr_mask_entry_t) * (SOC_PORT_ADDR_MAX(unit) + 1);
    mask_entries = sal_alloc(alloc_size, "EGR_MASK entry buffer");
    if (mask_entries == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(mask_entries, 0, alloc_size);
    entries[0] = mask_entries;
    rv = soc_profile_mem_get(unit, 
            profile, index, SOC_PORT_ADDR_MAX(unit) + 1, entries);

    if (BCM_FAILURE(rv)) {
        sal_free(mask_entries);
        return rv;
    }

    soc_mem_pbmp_field_get(unit, EGR_MASKm, &mask_entries[local_port],
                           EGRESS_MASKf, &mask_pbmp);
    sal_free(mask_entries);

    BCM_PBMP_CLEAR(temp_pbmp_all);
    BCM_PBMP_ASSIGN(temp_pbmp_all, PBMP_ALL(unit));

    BCM_PBMP_NEGATE(*pbmp, mask_pbmp);
    BCM_PBMP_AND(*pbmp, temp_pbmp_all);
    BCM_PBMP_REMOVE(*pbmp, PBMP_LB(unit));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_hr3_mod_port_tpid_enable_set
 * Description:
 *      Write the Tag Protocol ID enables for a (module, port).
 * Parameters:
 *      unit - Device number
 *      mod  - Module ID
 *      port - Port number
 *      is_outer - TRUE: outer, FALSE : inner
 *      tpid_enable - Tag Protocol ID enables
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
_bcm_hr3_mod_port_tpid_enable_set(int unit, bcm_module_t mod,
        bcm_port_t port, int is_outer, int tpid_enable)
{
    int rv = BCM_E_NONE;
    system_config_table_modbase_entry_t modbase_entry;
    system_config_table_entry_t *entry_array;
    void *entries;
    uint32 old_profile_index, new_profile_index;
    int i;

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
               SOC_PROFILE_MEM_ENTRY(unit, _bcm_hr3_sys_cfg_profile[unit],
                   void *, old_profile_index + i),
               sizeof(system_config_table_entry_t)); 
    }

    if (TRUE == is_outer) {
        soc_SYSTEM_CONFIG_TABLEm_field32_set(unit, &entry_array[port],
                                             OUTER_TPID_ENABLEf, tpid_enable);
    } else {
        soc_SYSTEM_CONFIG_TABLEm_field32_set(unit, &entry_array[port],
                                             INNER_TPID_ENABLEf, tpid_enable);
    }

    entries = entry_array;
    rv = soc_profile_mem_add(unit, _bcm_hr3_sys_cfg_profile[unit], &entries,
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

    rv = soc_profile_mem_delete(unit, _bcm_hr3_sys_cfg_profile[unit],
                                old_profile_index); 
    sal_free(entry_array);
    return rv;
}

/*
 * Function:
 *      _bcm_hr3_mod_port_tpid_enable_get
 * Description:
 *      Get the Tag Protocol ID enables for a (module, port).
 * Parameters:
 *      unit - Device number
 *      mod  - Module ID
 *      port - Port number
 *      is_outer - TRUE: outer, FALSE : inner
 *      tpid_enable - (OUT) Tag Protocol ID enables
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
_bcm_hr3_mod_port_tpid_enable_get(int unit, bcm_module_t mod, bcm_port_t port,
         int is_outer, int *tpid_enable)
{
    system_config_table_modbase_entry_t modbase_entry;
    system_config_table_entry_t sys_cfg_entry;
    int base;

    SOC_IF_ERROR_RETURN
        (READ_SYSTEM_CONFIG_TABLE_MODBASEm(unit, MEM_BLOCK_ANY, mod,
                                           &modbase_entry));
    base = soc_SYSTEM_CONFIG_TABLE_MODBASEm_field32_get(unit, &modbase_entry,
                                                        BASEf);

    SOC_IF_ERROR_RETURN
        (READ_SYSTEM_CONFIG_TABLEm(unit, MEM_BLOCK_ANY, base + port,
                                   &sys_cfg_entry));

    if (TRUE == is_outer) {
        *tpid_enable = soc_SYSTEM_CONFIG_TABLEm_field32_get(unit, &sys_cfg_entry,
                                                            OUTER_TPID_ENABLEf);
    } else {
        *tpid_enable = soc_SYSTEM_CONFIG_TABLEm_field32_get(unit, &sys_cfg_entry,
                                                            INNER_TPID_ENABLEf);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_hr3_mod_port_tpid_enable_write
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
_bcm_hr3_mod_port_tpid_enable_write(int unit, bcm_module_t mod,
        bcm_port_t port, int tpid_enable)
{
    return _bcm_hr3_mod_port_tpid_enable_set(unit, mod, port, TRUE, tpid_enable);
}

/*
 * Function:
 *      _bcm_hr3_mod_port_tpid_enable_read
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
_bcm_hr3_mod_port_tpid_enable_read(int unit, bcm_module_t mod, bcm_port_t port,
         int *tpid_enable)
{
    return _bcm_hr3_mod_port_tpid_enable_get(unit, mod, port, TRUE, tpid_enable);
}

/*
 * Function:
 *      _bcm_hr3_mod_port_inner_tpid_enable_write
 * Description:
 *      Write the Inner Tag Protocol ID enables for a (module, port).
 * Parameters:
 *      unit - Device number
 *      mod  - Module ID
 *      port - Port number
 *      tpid_enable - Inner Tag Protocol ID enables
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_hr3_mod_port_inner_tpid_enable_write(int unit, bcm_module_t mod,
        bcm_port_t port, int tpid_enable)
{
    return _bcm_hr3_mod_port_tpid_enable_set(unit, mod, port, FALSE, tpid_enable);
}

/*
 * Function:
 *      _bcm_hr3_mod_port_inner_tpid_enable_read
 * Description:
 *      Get the Inner Tag Protocol ID enables for a (module, port).
 * Parameters:
 *      unit - Device number
 *      mod  - Module ID
 *      port - Port number
 *      tpid_enable - (OUT) Inner Tag Protocol ID enables
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_hr3_mod_port_inner_tpid_enable_read(int unit, bcm_module_t mod, bcm_port_t port,
         int *tpid_enable)
{
    return _bcm_hr3_mod_port_tpid_enable_get(unit, mod, port, FALSE, tpid_enable);
}

/*
 * Function:
 *      _bcm_hr3_mod_port_tpid_set
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
_bcm_hr3_mod_port_tpid_set(int unit, bcm_module_t mod, bcm_port_t port,
        uint16 tpid)
{
    int rv = BCM_E_NONE;
    int tpid_enable, tpid_index;

    _bcm_fb2_outer_tpid_tab_lock(unit);

    rv = _bcm_hr3_mod_port_tpid_enable_read(unit, mod, port, &tpid_enable);
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
    rv = _bcm_hr3_mod_port_tpid_enable_write(unit, mod, port, tpid_enable);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
    }

    _bcm_fb2_outer_tpid_tab_unlock(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_hr3_mod_port_tpid_get
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
_bcm_hr3_mod_port_tpid_get(int unit, bcm_module_t mod, bcm_port_t port,
        uint16 *tpid)
{
    int tpid_enable, tpid_index;

    BCM_IF_ERROR_RETURN
        (_bcm_hr3_mod_port_tpid_enable_read(unit, mod, port, &tpid_enable));

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
 *      _bcm_hr3_mod_port_tpid_add
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
_bcm_hr3_mod_port_tpid_add(int unit, bcm_module_t mod, bcm_port_t port, 
                      uint16 tpid)
{
    int rv = BCM_E_NONE;
    int remove_tpid = FALSE;
    int tpid_enable, index;

    _bcm_fb2_outer_tpid_tab_lock(unit);

    rv = _bcm_hr3_mod_port_tpid_enable_read(unit, mod, port, &tpid_enable);
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
    rv = _bcm_hr3_mod_port_tpid_enable_write(unit, mod, port, tpid_enable);
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
 *      _bcm_hr3_mod_port_tpid_delete
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
_bcm_hr3_mod_port_tpid_delete(int unit, bcm_module_t mod, bcm_port_t port,
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

    rv = _bcm_hr3_mod_port_tpid_enable_read(unit, mod, port, &tpid_enable);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return rv;
    }

    if (tpid_enable & (1 << index)) {
        tpid_enable &= ~(1 << index);
        rv = _bcm_hr3_mod_port_tpid_enable_write(unit, mod, port, tpid_enable);
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
 *      _bcm_hr3_mod_port_tpid_delete_all
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
_bcm_hr3_mod_port_tpid_delete_all(int unit, bcm_module_t mod, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    int tpid_enable, tpid_index;

    _bcm_fb2_outer_tpid_tab_lock(unit);

    rv = _bcm_hr3_mod_port_tpid_enable_read(unit, mod, port, &tpid_enable);
    if (BCM_FAILURE(rv)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        return rv;
    }

    rv = _bcm_hr3_mod_port_tpid_enable_write(unit, mod, port, 0);
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
 *      bcmi_wh2_port_encap_get
 * Purpose:
 *      Get the port encapsulation mode
 *      For WH2, only support
 *      BCM_PORT_ENCAP_IEEE and BCM_PORT_ENCAP_HIGIG2_LITE
 * Parameters:
 *      unit - unit #
 *      port - port #
 *      mode (OUT) - One of BCM_PORT_ENCAP_xxx (see port.h)
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_wh2_port_encap_get(int unit, bcm_port_t port, int *mode)
{
    uint32 rval, higig2;

    BCM_IF_ERROR_RETURN(READ_PGW_GE_CONFIGr(unit, port, &rval));
    higig2 = soc_reg_field_get(unit, PGW_GE_CONFIGr, rval, HIGIG2_MODEf);
    if (higig2) {
        *mode = BCM_PORT_ENCAP_HIGIG2_LITE;
    } else {
        *mode = BCM_PORT_ENCAP_IEEE;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_wh2_port_encap_set
 * Purpose:
 *      Set the port encapsulation mode
 *      For WH2, only support
 *      BCM_PORT_ENCAP_IEEE and BCM_PORT_ENCAP_HIGIG2_LITE
 * Parameters:
 *      unit - unit #
 *      port - port #
 *      mode - One of BCM_PORT_ENCAP_xxx (see port.h)
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_wh2_port_encap_set(int unit, bcm_port_t port, int mode)
{
    int     to_higig;

    /* no encap change has been returned in bcmi_esw_port_encap_set */
    /* error check */
    if (mode == BCM_PORT_ENCAP_IEEE) {
        if (!IS_HL_PORT(unit, port)) {
            return BCM_E_UNAVAIL;
        }
    } else if (mode == BCM_PORT_ENCAP_HIGIG2_LITE) {
        if (!IS_E_PORT(unit, port)) {
            return BCM_E_UNAVAIL;
        }
    } else {
        return BCM_E_UNAVAIL;
    }

    to_higig = (mode != BCM_PORT_ENCAP_IEEE);

    BCM_IF_ERROR_RETURN(
        _bcm_port_encap_stport_set(unit, port, mode));
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_higig2_mode_set(unit, port, to_higig));
    if (mode == BCM_PORT_ENCAP_HIGIG2_LITE) {
        SOC_HG2_ENABLED_PORT_ADD(unit, port);
    } else {
        SOC_HG2_ENABLED_PORT_REMOVE(unit, port);
    }
    soc_dport_map_update(unit);

    return BCM_E_NONE;
}

#endif /* BCM_HURRICANE3_SUPPORT */
