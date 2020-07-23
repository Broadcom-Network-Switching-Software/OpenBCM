/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ipmc.c
 * Purpose:     firebolt6 multicast replication support methods.
 */


#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_FIREBOLT6_SUPPORT) && defined(INCLUDE_L3)


#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/firebolt.h>

#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/multicast.h>
#if defined(BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/xgs5.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */

#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/multicast.h>
#include <bcm_int/common/multicast.h>
#endif

#if defined(BCM_FIREBOLT6_SUPPORT)
#include <bcm_int/esw/firebolt6.h>
#include <soc/firebolt6.h>
#endif




bcm_error_t
bcmi_fb6_repl_head_icc_move(int unit, int old_head_index,
                            int new_head_index)
{
    int rv = BCM_E_NONE;
    uint32 icc_row_idx, icc_clm_idx;
    int bit = 0;
    int count_width = 3;
    uint32 initial_copy_count_entry[SOC_MAX_MEM_WORDS];
    SHR_BITDCL copy_count_buf[32];
    int copy_count = 0;
    soc_mem_t repl_group_icc_table;

    /* Head ICC table is positioned as 3 bits for each head index. */
    repl_group_icc_table = MMU_REPL_HEAD_ICC_SC0m;

    icc_row_idx = old_head_index/32;
    icc_clm_idx = old_head_index % 32;

    /* Update the read entry now. */
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, repl_group_icc_table, MEM_BLOCK_ANY,
         icc_row_idx, &initial_copy_count_entry));

    soc_mem_field_get(unit, repl_group_icc_table,
        (uint32 *)&initial_copy_count_entry, INITIAL_COPY_COUNTf,
        copy_count_buf);

    /* For each head index there are 3 bits. */
    icc_clm_idx = (icc_clm_idx) * count_width;

    copy_count = 0;
    /* Get the 3 bit icc value. */
    for (bit = 0; bit < count_width; bit++) {
        if (SHR_BITGET(copy_count_buf, icc_clm_idx + bit)) {
            copy_count |= (1 << bit);
        }
    }

    /* Update the new head index. */
    icc_row_idx = new_head_index/32;
    icc_clm_idx = new_head_index % 32;

    /* Update the read entry now. */
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, repl_group_icc_table, MEM_BLOCK_ANY,
         icc_row_idx, &initial_copy_count_entry));

    soc_mem_field_get(unit, repl_group_icc_table,
        (uint32 *)&initial_copy_count_entry, INITIAL_COPY_COUNTf,
        copy_count_buf);

    /* For each head index there are 3 bits. */
    icc_clm_idx = (icc_clm_idx) * count_width;
    for (bit = 0; bit < count_width; bit++) {
        if (copy_count & (1 << bit)) {
            SHR_BITSET(copy_count_buf, icc_clm_idx + bit);
        } else {
            SHR_BITCLR(copy_count_buf, icc_clm_idx + bit);
        }
    }

    soc_mem_field_set(unit, repl_group_icc_table,
        (uint32 *)&initial_copy_count_entry, INITIAL_COPY_COUNTf,
        copy_count_buf);

    rv = soc_mem_write(unit, repl_group_icc_table, MEM_BLOCK_ALL,
        icc_row_idx, &initial_copy_count_entry);

    return rv;

}

#endif /* BCM_FIREBOLT6_SUPPORT && INCLUDE_L3 */

