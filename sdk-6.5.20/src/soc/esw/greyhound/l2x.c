/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XGS GH L2 Table Manipulation API routines.
 *
 * A low-level L2 shadow table is optionally kept in soc->arlShadow by
 * using a callback to get all inserts/deletes from the l2xmsg task. It
 * can be disabled by setting the l2xmsg_avl property to 0.
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#include <soc/ptable.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/greyhound.h>

#ifdef BCM_GREYHOUND_SUPPORT

#define _SOC_GH_L2_TABLE_DMA_CHUNK_SIZE 1024

/*
 * Function:
 *    _soc_gh_l2_entry_limit_update
 * Purpose:
 *    Update the mac limits per l2 entry
 * Parameters:
 *    unit         - unit number
 *    l2_entry     - l2 entry
 *    ptm_tbl      - port or trumk mac limit table
 *    ptm_entry_dw - entry length of port or trumk mac limit table
 *    vvm_tbl      - vlan or vfi mac limit table
 *    vvm_entry_dw - entry length of vlan or vfi mac limit table
 *    s_count      - system mac limit count
 * Returns:
 *    SOC_E_XXX.
 */
STATIC int
_soc_gh_l2_entry_limit_update(
    int unit,
    uint32 *l2_entry,
    uint32 *ptm_tbl,
    uint32 ptm_entry_dw,
    uint32 *vvm_tbl,
    uint32 vvm_entry_dw,
    uint32 *s_count)
{
    uint32 dest_type, key_type;
    uint32 is_valid, is_limit_counted = 0;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int32 v_index = -1;  /* vlan index */
    int32 p_index = -1;  /* port index */
    uint32 p_count = 0;  /* port or trunk mac count */
    uint32 v_count = 0;  /* vlan or vfi mac count */
    uint32 *v_entry;     /* vlan or vfi count entry */
    uint32 *p_entry;     /* port or trunk count entry */

    mem = L2Xm;

    /* check if entry is valid */
    is_valid = soc_mem_field32_get(unit, mem, l2_entry, VALIDf);
    if (!is_valid) {
        /* not valid entry skip it */
        return SOC_E_NONE;
    }

    /* check if entry is limit counted */
    if (soc_mem_field_valid(unit, mem, LIMIT_COUNTEDf)) {
        is_limit_counted = soc_mem_field32_get(unit, mem, l2_entry,
                                               LIMIT_COUNTEDf);
        if (!is_limit_counted) {
            /* entry not limit counted skip it */
            return SOC_E_NONE;
        }
    }

    /*
    * Get the key type of the entries. Process entries with only key_type
    * GH_L2_HASH_KEY_TYPE_BRIDGE
    */
    key_type = soc_mem_field32_get(unit, mem, l2_entry, KEY_TYPEf);

    if (key_type != GH_L2_HASH_KEY_TYPE_BRIDGE) {
        return SOC_E_NONE;
    }

    dest_type = soc_mem_field32_get(unit, mem, l2_entry, L2__DEST_TYPEf);

    switch (dest_type) {
        case 0: /* mod port */
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm,
                                             MEM_BLOCK_ANY, 0, &entry));
            if (soc_mem_field32_get(unit, PORT_TABm, &entry, MY_MODIDf) ==
                soc_mem_field32_get(unit, mem, l2_entry, L2__MODULE_IDf)) {
                p_index = soc_mem_field32_get(unit, mem, l2_entry, PORT_NUMf) +
                          soc_mem_index_count(unit, TRUNK_GROUPm);
            }
            break;
        case 1: /* trunk */
            p_index = soc_mem_field32_get(unit, mem, l2_entry, L2__TGIDf);
            break;
        default:
            /* if VFI based key then break else continue? */
            break;
    }

    /*
    * based on key_type get vlan or vfi
    * to index into VLAN_OR_VFI_MAC_COUNTm
    */
    v_index = soc_mem_field32_get(unit, mem, l2_entry, L2__VLAN_IDf);

    /*
    * read and update vlan or vfi mac count
    * in buffer also update the sys mac count.
    */
    v_count = 0;
    if (v_index >= 0) {
        v_entry = (vvm_tbl + (v_index * vvm_entry_dw));
        v_count = soc_mem_field32_get(unit, VLAN_OR_VFI_MAC_COUNTm,
                                      v_entry, COUNTf);
        *s_count = *s_count + 1;
        v_count++;
        soc_mem_field32_set(unit, VLAN_OR_VFI_MAC_COUNTm,
                            v_entry, COUNTf, v_count);
    }

    /* read and update port or trunk mac count in buffer. */
    p_count = 0;
    if (p_index >= 0) {
        p_entry = ptm_tbl + (p_index * ptm_entry_dw);
        p_count = soc_mem_field32_get(unit, PORT_OR_TRUNK_MAC_COUNTm,
                                      p_entry, COUNTf);
        p_count++;
        soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_COUNTm,
                            p_entry, COUNTf, p_count);
    }

    return SOC_E_NONE;
}


/**************************************************
 * @function soc_gh_l2_entry_limit_count_update
 *
 *
 * @purpose Read L2 table from hardware, calculate
 *          and restore port/trunk and vlan/vfi
 *          mac counts.
 *
 * @param  unit   @b{(input)}  unit number
 *
 * @returns BCM_E_NONE
 * @returns BCM_E_FAIL
 *
 * @comments This rountine gets called for an
 *           L2 table SER event, if any of the
 *           mac limits(system, port, vlan) are
 *           enabled on the device.
 *
 * @end
 */
int
soc_gh_l2_entry_limit_count_update(int unit)
{
    uint32 rval;
    uint32 *l2_entry;    /* l2 entry */
    uint32 s_count = 0;  /* system mac count */

    /* l2 table */
    int index_min, index_max;
    int entry_dw, entry_sz;
    int chnk_idx, chnk_idx_max, table_chnk_sz, idx;
    int chunk_size = _SOC_GH_L2_TABLE_DMA_CHUNK_SIZE;

    /* port or trunk mac count table */
    int ptm_index_min, ptm_index_max;
    int ptm_table_sz, ptm_entry_dw, ptm_entry_sz;

    /* vlan or vfi mac count table */
    int vvm_index_min, vvm_index_max;
    int vvm_table_sz, vvm_entry_dw, vvm_entry_sz;

    uint32 *l2_buf = NULL;
    uint32 *ptm_buf = NULL;
    uint32 *vvm_buf = NULL;

    soc_mem_t mem;
    int rv;
#ifdef BCM_HURRICANE3_SUPPORT
    uint32 l2_learn;
#endif /* BCM_HURRICANE3_SUPPORT */

    /* If MAC learn limit not enabled do nothing */
    SOC_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));
    if (!soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr,
                           rval, ENABLEf)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "\nMAC limits not enabled.\n")));
        return SOC_E_NONE;
    }

    /*
     * Mac limits are enabled.
     * 1. Read (DMA) L2 table to recalculate mac count
     * 2. Iterate through all the entries.
     * 3. if the entry is limit_counted
     * Check if KEY_TYPE is BRIDGE, VLAN or VFI
     * based on DEST_TYPE determine if Trunk or ModPort
     * Get port index into port_or_trunk_mac_count table
     * Get vlan/vfi index into vlan_or_vfi_mac_count table
     * update the port, vlan/vfi, system mac count.
     * Write(slam) port_or_trunk_mac count and
     * vlan_or_vfi_mac_count tables.
     */

    /*
     * Allocate memory for port/trunk mac count table to store
     * the updated count.
     */
    mem = PORT_OR_TRUNK_MAC_COUNTm;
    ptm_entry_dw = soc_mem_entry_words(unit, mem);
    ptm_entry_sz = ptm_entry_dw * sizeof(uint32);
    ptm_index_min = soc_mem_index_min(unit, mem);
    ptm_index_max = soc_mem_index_max(unit, mem);
    ptm_table_sz = (ptm_index_max - ptm_index_min + 1) * ptm_entry_sz;

    ptm_buf = soc_cm_salloc(unit, ptm_table_sz, "ptm_tmp");
    if (ptm_buf == NULL) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "soc_gh_l2_entry_limit_count_update: "
                              "Memory allocation failed for %s\n"),
                  SOC_MEM_NAME(unit, mem)));
        return SOC_E_MEMORY;
    }

    sal_memset(ptm_buf, 0, ptm_table_sz);

    /*
     * Allocate memory for vlan/vfi mac count table to store
     * the updated count.
     */
    mem = VLAN_OR_VFI_MAC_COUNTm;
    vvm_entry_dw = soc_mem_entry_words(unit, mem);
    vvm_entry_sz = vvm_entry_dw * sizeof(uint32);
    vvm_index_min = soc_mem_index_min(unit, mem);
    vvm_index_max = soc_mem_index_max(unit, mem);
    vvm_table_sz = (vvm_index_max - vvm_index_min + 1) * vvm_entry_sz;

    vvm_buf = soc_cm_salloc(unit, vvm_table_sz, "vvm_tmp");
    if (vvm_buf == NULL) {
        soc_cm_sfree(unit, ptm_buf);
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "soc_gh_l2_entry_limit_count_update: "
                              "Memory allocation failed for %s\n"),
                  SOC_MEM_NAME(unit, mem)));
        return SOC_E_MEMORY;
    }

    sal_memset(vvm_buf, 0, vvm_table_sz);

    /* Create a copy L2Xm table for calculating mac count */
    mem = L2Xm;
    entry_dw = soc_mem_entry_words(unit, mem);
    entry_sz = entry_dw * sizeof(uint32);
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);
    table_chnk_sz = chunk_size * entry_sz;

    l2_buf = soc_cm_salloc(unit, table_chnk_sz, "l2x_tmp");
    if (l2_buf == NULL) {
        soc_cm_sfree(unit, ptm_buf);
        soc_cm_sfree(unit, vvm_buf);
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "soc_gh_l2_entry_limit_count_update: "
                              "Memory allocation failed for %s\n"),
                  SOC_MEM_NAME(unit, mem)));
        return SOC_E_MEMORY;
    }

    SOC_L2X_MEM_LOCK(unit);
    for (chnk_idx = index_min; chnk_idx <= index_max; chnk_idx += chunk_size) {
        sal_memset(l2_buf, 0, table_chnk_sz);

        chnk_idx_max = ((chnk_idx + chunk_size) <= index_max) ?
                       (chnk_idx + chunk_size - 1) : index_max;

         /* DMA L2 Table */
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, chnk_idx,
                                chnk_idx_max, l2_buf);
        if (SOC_FAILURE(rv)) {
            soc_cm_sfree(unit, ptm_buf);
            soc_cm_sfree(unit, vvm_buf);
            soc_cm_sfree(unit, l2_buf);
            SOC_L2X_MEM_UNLOCK(unit);
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "DMA failed: %s, mac limts not synced!\n"),
                      soc_errmsg(rv)));
            return SOC_E_FAIL;
        }

        /* Iter through all l2 entries in the chunk */
        for (idx = 0; idx <= (chnk_idx_max - chnk_idx); idx++) {
            l2_entry = (l2_buf + (idx * entry_dw));

            rv = _soc_gh_l2_entry_limit_update(unit, l2_entry,
                                               ptm_buf, ptm_entry_dw,
                                               vvm_buf, vvm_entry_dw,
                                               &s_count);
            if (SOC_FAILURE(rv)) {
                soc_cm_sfree(unit, ptm_buf);
                soc_cm_sfree(unit, vvm_buf);
                soc_cm_sfree(unit, l2_buf);
                SOC_L2X_MEM_UNLOCK(unit);
                LOG_ERROR(BSL_LS_SOC_L2,
                          (BSL_META_U(unit,
                                      "L2 MAC count update failed: %s, "
                                      "for L2 Entry\n"),
                          soc_errmsg(rv)));
                return SOC_E_FAIL;
            }
        } /* End for index */
    } /* End for chnk_idx */

    /* Free memory */
    soc_cm_sfree(unit, l2_buf);

#ifdef BCM_HURRICANE3_SUPPORT
    if (soc_feature(unit, soc_feature_l2_overflow_bucket)) {
        /* check if l2 overflow table is enabled */
        rv = READ_L2_LEARN_CONTROLr(unit, &l2_learn);
        if (SOC_FAILURE(rv)) {
            soc_cm_sfree(unit, ptm_buf);
            soc_cm_sfree(unit, vvm_buf);
            SOC_L2X_MEM_UNLOCK(unit);
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "Register read failed: %s, "
                                  "for L2 Learn Control\n"),
                      soc_errmsg(rv)));
            return SOC_E_FAIL;
        }
        if (soc_reg_field_get(unit, L2_LEARN_CONTROLr,
                              l2_learn, OVERFLOW_BUCKET_ENABLEf)) {
            /* L2 overflow table is enabled */
            mem = L2_ENTRY_OVERFLOWm;
            entry_dw = soc_mem_entry_words(unit, mem);
            entry_sz = entry_dw * sizeof(uint32);
            index_min = soc_mem_index_min(unit, mem);
            index_max = soc_mem_index_max(unit, mem);
            table_chnk_sz = (index_max + 1) * entry_sz;

            l2_buf = soc_cm_salloc(unit, table_chnk_sz, "l2x_overflow");
            if (l2_buf == NULL) {
                soc_cm_sfree(unit, ptm_buf);
                soc_cm_sfree(unit, vvm_buf);
                SOC_L2X_MEM_UNLOCK(unit);
                LOG_ERROR(BSL_LS_SOC_L2,
                          (BSL_META_U(unit,
                                      "soc_gh_l2_entry_limit_count_update: "
                                      "Memory allocation failed for %s\n"),
                          SOC_MEM_NAME(unit, mem)));
                return SOC_E_MEMORY;
            }

            sal_memset(l2_buf, 0, table_chnk_sz);

            /* DMA L2 overflow Table */
            rv =  soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                     index_min, index_max, l2_buf);
            if (SOC_FAILURE(rv)) {
                soc_cm_sfree(unit, ptm_buf);
                soc_cm_sfree(unit, vvm_buf);
                soc_cm_sfree(unit, l2_buf);
                SOC_L2X_MEM_UNLOCK(unit);
                LOG_ERROR(BSL_LS_SOC_L2,
                          (BSL_META_U(unit,
                                      "DMA failed: %s, "
                                      "mac limts not synced!\n"),
                          soc_errmsg(rv)));
                return SOC_E_FAIL;
            }

            /* Iter through all l2 overflow entries */
            for (idx = 0; idx <= index_max; idx++) {
                l2_entry = (l2_buf + (idx * entry_dw));

                rv = _soc_gh_l2_entry_limit_update(unit, l2_entry,
                                                   ptm_buf, ptm_entry_dw,
                                                   vvm_buf, vvm_entry_dw,
                                                   &s_count);
                if (SOC_FAILURE(rv)) {
                    soc_cm_sfree(unit, ptm_buf);
                    soc_cm_sfree(unit, vvm_buf);
                    soc_cm_sfree(unit, l2_buf);
                    SOC_L2X_MEM_UNLOCK(unit);
                    LOG_ERROR(BSL_LS_SOC_L2,
                              (BSL_META_U(unit,
                                          "L2 MAC count update failed: %s, "
                                          "for L2 Overflow Entry\n"),
                              soc_errmsg(rv)));
                    return SOC_E_FAIL;
                }
            }

            /* Free memory */
            soc_cm_sfree(unit, l2_buf);
        }
    }
#endif /* BCM_HURRICANE3_SUPPORT */

    /* Write the tables to hardware */
    mem = PORT_OR_TRUNK_MAC_COUNTm;
    rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ANY,
                             ptm_index_min, ptm_index_max, (void *)ptm_buf);
    if (SOC_FAILURE(rv)) {
        soc_cm_sfree(unit, ptm_buf);
        soc_cm_sfree(unit, vvm_buf);
        SOC_L2X_MEM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "PORT_OR_TRUNK_MAC_COUNT write failed: "
                              "%s, mac limts not synced!\n"),
                   soc_errmsg(rv)));
        return SOC_E_FAIL;
    }

    mem = VLAN_OR_VFI_MAC_COUNTm;
    rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ANY,
                             vvm_index_min, vvm_index_max, (void *)vvm_buf);
    if (SOC_FAILURE(rv)) {
        soc_cm_sfree(unit, ptm_buf);
        soc_cm_sfree(unit, vvm_buf);
        SOC_L2X_MEM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "VLAN_OR_VFI_MAC_COUNT write failed: "
                              "%s, mac limts not synced!\n"),
                   soc_errmsg(rv)));
        return SOC_E_FAIL;
    }

    /* Update system count */
    soc_reg_field_set(unit, SYS_MAC_COUNTr, &rval, COUNTf, s_count);
    rv = WRITE_SYS_MAC_COUNTr(unit, rval);

    /* Free memory */
    soc_cm_sfree(unit, ptm_buf);
    soc_cm_sfree(unit, vvm_buf);

    SOC_L2X_MEM_UNLOCK(unit);

    return rv;
}

#endif /* BCM_GREYHOUND_SUPPORT */
