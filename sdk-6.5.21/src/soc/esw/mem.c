/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC ESW Memory (Table) Utilities
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/drv.h>
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/esw/flow_db.h>
#endif /* BCM_TRIDENT3_SUPPORT */

#if defined(BCM_ESW_SUPPORT)

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
/* Global memory for faster indirect mem access
 * using BSC_DT_SBUS_ACCESS_CONTROL
 */
uint32 indirect_mem_data_w[SOC_MAX_NUM_DEVICES][SOC_MAX_MEM_WORDS];
uint32 indirect_mem_data_r[SOC_MAX_NUM_DEVICES][SOC_MAX_MEM_WORDS];
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
/*
 * Function:
 *    soc_mem_view_phy_mem_get
 * Purpose:
 *    Convert a given mem view to physical memory
 * Parameters:  unit - device
 *              view (IN) - mem view
 *              mem (OUT) - physical memory
 *
 * Returns:
 *    SOC_E_NONE - conversion succeeded
 *    SOC_E_XXX - other error
 * Notes:
 *      This internally calls flow db utility functions
 *      to convert given mem view to underlying physical
 *      memory
 */
int
soc_mem_view_phy_mem_get(int unit,
              soc_mem_t view,
              soc_mem_t *mem)
{
    int rv = SOC_E_NONE;
    soc_flow_db_mem_view_info_t view_info;

    if (!mem) return SOC_E_PARAM;

    sal_memset(&view_info, 0x0, sizeof(view_info));

    rv = soc_flow_db_mem_view_info_get(unit, view, &view_info);
    if (rv != SOC_E_NONE) {
        if (rv == SOC_E_PARAM) {
#if defined(BCM_ESW_SUPPORT)
            LOG_CLI((BSL_META("mem view %d is invalid\n"),
                        view));
#endif
            assert(0);
        }
        return rv;
    }
    *mem = view_info.mem;
    return SOC_E_NONE;
}

/*
 * Function:
 *    soc_mem_view_split_field_info_get
 * Purpose:
 *    For a given field of a given memory view,
 *    get the split field information: user field width and split position,
 *    the two hardware field info.
 * Parameters:  unit - device
 *              view (IN) - mem view
 *              field (IN) - field
 *              sf_info (OUT) - field information
 * Returns:
 *    SOC_E_NONE - if the field information is found
 *    SOC_E_XXX - if the field is not found
 * Notes:
 *      This internally calls flow db utility functions
 *      to find information for a given field
 */
int soc_mem_view_split_field_info_get(int unit,
        soc_mem_t view,
        soc_field_t field,
        soc_mem_view_split_field_info_t *sf_info)   /* split field info */
{
    soc_flow_db_mem_view_split_field_info_t view_sf_info;
    int i;

    if (!sf_info) return SOC_E_PARAM;

    sal_memset(&view_sf_info, 0x0, sizeof(view_sf_info));
    SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_split_field_info_get(unit, 
           view, field, &view_sf_info));

    sf_info->num_fld = view_sf_info.num_fld;
    sf_info->width  = view_sf_info.width;
    for (i = 0; i < sf_info->num_fld; i++) {
        sf_info->fld[i].field.field = view_sf_info.field[i].field_id; 
        sf_info->fld[i].field.len   = view_sf_info.field[i].width;
        sf_info->fld[i].field.bp    = view_sf_info.field[i].offset;
        sf_info->fld[i].field.flags = view_sf_info.field[i].flags;
        sf_info->fld[i].v_offset    = view_sf_info.field[i].v_offset;
    }
    return SOC_E_NONE;
}
         
/*
 * Function:
 *    soc_mem_view_fieldinfo_get
 * Purpose:
 *    For a given field of a given memory view,
 *    get the information about the field - like - 
 *    width, offset, flags
 * Parameters:  unit - device
 *              view (IN) - mem view
 *              field (IN) - field
 *              fieldinfo (OUT) - field information
 * Returns:
 *    SOC_E_NONE - if the field information is found
 *    SOC_E_XXX - if the field is not found
 * Notes:
 *      This internally calls flow db utility functions
 *      to find information for a given field
 */
int soc_mem_view_fieldinfo_get(int unit,
        soc_mem_t view,
        soc_field_t field,
        soc_field_info_t *fieldinfo)
{
    int rv = SOC_E_NONE;
    soc_flow_db_mem_view_field_info_t view_field_info;

    if (!fieldinfo) return SOC_E_PARAM;

    sal_memset(&view_field_info, 0x0, sizeof(view_field_info));

    rv = soc_flow_db_mem_view_field_info_get(unit, view, field, &view_field_info);
    if (rv != SOC_E_NONE) {
        if (rv == SOC_E_NOT_FOUND) {
#if defined(BCM_ESW_SUPPORT)
            /* this print statement can be enhanced to
               find the physical memory
               and field (could be logical) */
            LOG_CLI((BSL_META("mem view %d field 0x%x is invalid\n"),
                        view, field));
#endif
            assert(0);
        }
        return rv;
    }

    fieldinfo->field = view_field_info.field_id; 
    fieldinfo->len = view_field_info.width;
    fieldinfo->bp = view_field_info.offset;
    fieldinfo->flags = view_field_info.flags;
    return SOC_E_NONE;
}

/*
 * Function:
 *    soc_mem_view_field_valid
 * Purpose:
 *    For a given field of a given memory view,
 *    find out if this is a valid field
 * Parameters:  unit - device
 *              view (IN) - mem view
 *              field (IN) - field
 * Returns:
 *    0 - not a valid field
 *    1 - valid field
 * Notes:
 *      This internally calls flow db utility functions
 *      to find information for a given field
 */
int
soc_mem_view_field_valid(int unit,
              soc_mem_t view,
              soc_field_t field)
{
    return soc_flow_db_mem_view_field_is_valid(unit, view, field);
}
#endif

int
soc_ft_mem_is_agg_indirect_access(int unit, soc_mem_t mem)
{
    /* Check if indirect access feature is enabled for device */
    if (!soc_feature(unit,
         soc_feature_flex_flowtracker_indirect_memory_access)) {
        return FALSE;
    }

    /* No indirect access for BCMSIM or PCID SIM */
    if (SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM) {
        return FALSE;
    }

    if (mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_0m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_1m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_2m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_3m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_3m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_4m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_5m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_6m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_7m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_8m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_9m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_10m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_11m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_12m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_13m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_14m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_15m ||
        mem == BSC_DT_FLEX_AFT_SESSION_DATA_BANK_0m ||
        mem == BSC_DT_FLEX_AFT_SESSION_DATA_BANK_1m ||
        mem == BSC_DT_FLEX_AFT_SESSION_DATA_BANK_2m ||
        mem == BSC_DT_FLEX_AFT_SESSION_DATA_BANK_3m ||
        mem == BSC_DG_AIFT_GROUP_TABLEm ||
        mem == BSC_DG_AMFT_GROUP_TABLEm ||
        mem == BSC_DG_AEFT_GROUP_TABLEm) {
        return TRUE;
    }
    return FALSE;

}


#ifdef BCM_FLOWTRACKER_V2_SUPPORT
/*
 * Function:
 *   soc_ft_mem_is_indirect_access
 * Purpose:
 *   This function validates if provided
 *   memory is enabled for indirect access or not.
 * Parameters:
 *   unit(IN) - device identifier
 *   mem (IN) - memory to check
 * Returns:
 *   FALSE - Indirect Memory access is not allowed.
 *   TRUE  - Indirect Memory access is allowed.
 */
int
soc_ft_mem_is_indirect_access(int unit, soc_mem_t mem)
{
    /* Check if indirect access feature is enabled for device */
    if (!soc_feature(unit,
                soc_feature_flex_flowtracker_indirect_memory_access)) {
        return FALSE;
    }

    /* No indirect access for BCMSIM or PCID SIM */
    if (SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM) {
        return FALSE;
    }

    /* Check for valid memories */
    if (mem == BSC_DT_FLEX_SESSION_DATA_SINGLEm ||
        mem == BSC_DT_FLEX_SESSION_DATA_DOUBLEm ||
        mem == BSC_DG_GROUP_TIMESTAMP_PROFILEm ||
        mem == BSC_DG_GROUP_LOAD16_PROFILEm ||
        mem == BSC_DG_GROUP_LOAD8_PROFILEm ||
        mem == BSC_DG_GROUP_ALU16_PROFILE_0m ||
        mem == BSC_DG_GROUP_ALU16_PROFILE_1m ||
        mem == BSC_DG_GROUP_ALU16_PROFILE_2m ||
        mem == BSC_DG_GROUP_ALU16_PROFILE_3m ||
        mem == BSC_DG_GROUP_ALU16_PROFILE_4m ||
        mem == BSC_DG_GROUP_ALU16_PROFILE_5m ||
        mem == BSC_DG_GROUP_ALU16_PROFILE_6m ||
        mem == BSC_DG_GROUP_ALU16_PROFILE_7m ||
        mem == BSC_DG_GROUP_ALU32_PROFILE_0m ||
        mem == BSC_DG_GROUP_ALU32_PROFILE_1m ||
        mem == BSC_DG_GROUP_ALU32_PROFILE_2m ||
        mem == BSC_DG_GROUP_ALU32_PROFILE_3m ||
        mem == BSC_DG_GROUP_ALU32_PROFILE_4m ||
        mem == BSC_DG_GROUP_ALU32_PROFILE_5m ||
        mem == BSC_DG_GROUP_ALU32_PROFILE_6m ||
        mem == BSC_DG_GROUP_ALU32_PROFILE_7m ||
        mem == BSC_DG_GROUP_ALU32_PROFILE_8m ||
        mem == BSC_DG_GROUP_ALU32_PROFILE_9m ||
        mem == BSC_DG_GROUP_ALU32_PROFILE_10m ||
        mem == BSC_DG_GROUP_ALU32_PROFILE_11m ||
        mem == BSC_DG_GROUP_TABLEm ||
        mem == BSD_POLICY_ACTION_PROFILE_0m ||
        mem == BSD_POLICY_ACTION_PROFILE_1m ||
        mem == BSD_POLICY_ACTION_PROFILE_2m ||
        mem == BSC_DT_PDE_PROFILE_0m ||
        mem == BSC_DT_PDE_PROFILE_1m ||
        mem == BSC_DT_PDE_PROFILE_2m ||
        mem == BSC_DT_EXPORT_PDE_PROFILE_0m ||
        mem == BSC_DT_EXPORT_PDE_PROFILE_1m ||
        mem == BSD_FT_SLAVE_1m ||
        mem == BSD_FT_SLAVE_2m ||
        mem == BSC_DT_FLEX_AFT_SESSION_DATA_BANK_0m ||
        mem == BSC_DT_FLEX_AFT_SESSION_DATA_BANK_1m ||
        mem == BSC_DT_FLEX_AFT_SESSION_DATA_BANK_2m ||
        mem == BSC_DT_FLEX_AFT_SESSION_DATA_BANK_3m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_0m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_1m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_2m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_3m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_3m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_4m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_5m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_6m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_7m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_8m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_9m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_10m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_11m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_12m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_13m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_14m ||
        mem == BSC_DG_AGG_GROUP_ALU32_PROFILE_15m ||
        mem == BSC_DG_AIFT_GROUP_TABLEm ||
        mem == BSC_DG_AMFT_GROUP_TABLEm ||
        mem == BSC_DG_AEFT_GROUP_TABLEm ||
        mem == BSC_DT_EXPORT_PDE_PROFILE_2m) {

        return TRUE;
    }

    return FALSE;
}

/*
 * Function:
 *  soc_indirect_mem_write
 * Purpose:
 *  This function writes into provided memory indirectly
 *  by writing into BSC_DT_SBUS_ACCESS_CONTROLm.
 * Parameters:
 *  unit(IN) - device identifier
 *  mem (IN) - memory to write
 *  array_index (IN) - array index
 *  copyno (IN) - copy number
 *  index (IN) - index to write
 *  entry_data (IN) - data to write
 * Returns:
 *  SOC_E_NONE - indirect mem write is success.
 *  SOC_E_XXX  - indirect mem write failed.
 * Notes:
 *  Flow tracker memories are shared across multiple
 *  FT stages in IP, MMU & EP. To prevent schan read
 *  write error this indirect method is used to write
 *  into those memories.
 */
int
soc_indirect_mem_write(int unit, soc_mem_t mem,
                       unsigned array_index,
                       int copyno, int index,
                       void *entry_data)
{
    uint8 at = 0, done_bit = 0;
    uint32 flags = SOC_MEM_NO_FLAGS, maddr = 0;
    int rv = SOC_E_NONE, blk = 0, min_polls = 10;
    soc_timeout_t to = {0};
    void *entry_data_ptr = NULL;
    sal_usecs_t timeout_usec = SOC_CONTROL(unit)->ftIndirectMemAccessTimeout;
    uint32 converted_entry_data[SOC_MAX_MEM_WORDS] = {0};
    uint32 cache_entry_data[SOC_MAX_MEM_WORDS] = {0};
    bsc_dt_sbus_access_control_entry_t dt_sbus_access_entry_w;
    bsc_dt_sbus_access_control_entry_t dt_sbus_access_entry_r;
    bsc_dt_agg_sbus_access_control_entry_t dt_agg_sbus_access_entry_w;
    bsc_dt_agg_sbus_access_control_entry_t dt_agg_sbus_access_entry_r;
    soc_mem_t acc_mem = BSC_DT_SBUS_ACCESS_CONTROLm;
    uint32 *dt_entry_w = NULL;
    uint32 *dt_entry_r = NULL;

    /* Initialize sbus access entry variable */
    sal_memset(&dt_sbus_access_entry_w, 0, sizeof(dt_sbus_access_entry_w));
    sal_memset(&dt_sbus_access_entry_r, 0, sizeof(dt_sbus_access_entry_r));
    sal_memset(&dt_agg_sbus_access_entry_w, 0, sizeof(dt_agg_sbus_access_entry_w));
    sal_memset(&dt_agg_sbus_access_entry_r, 0, sizeof(dt_agg_sbus_access_entry_r));

    sal_memset(indirect_mem_data_w[unit], 0, SOC_MAX_MEM_WORDS * 4);

    dt_entry_r = (uint32*)&dt_sbus_access_entry_r;
    dt_entry_w = (uint32*)&dt_sbus_access_entry_w;

    if (soc_ft_mem_is_agg_indirect_access(unit, mem)) {
        acc_mem = BSC_DT_AGG_SBUS_ACCESS_CONTROLm;
        dt_entry_r = (uint32*)&dt_agg_sbus_access_entry_r;
        dt_entry_w = (uint32*)&dt_agg_sbus_access_entry_w;

    }

    /* Write sanity check */
    rv = _soc_mem_write_sanity_check(unit, flags, mem, index);
    SOC_IF_ERROR_RETURN(rv);

    /* Convert data in hw format if needed */
    entry_data_ptr = soc_mem_write_tcam_to_hw_format(unit, mem, entry_data,
                                                     cache_entry_data,
                                                     converted_entry_data);

    /* Copy incoming data in indirect data buffer */
    sal_memcpy(indirect_mem_data_w[unit], entry_data_ptr,
                    (soc_mem_entry_words(unit, mem) * 4));

    /* Set data to write */
    soc_mem_field_set(unit, acc_mem,
                                              dt_entry_w,
                                              DATAf, indirect_mem_data_w[unit]);
    /* Set write command - opcode 1 */
    soc_mem_field32_set(unit, acc_mem,
                                                dt_entry_w,
                                                OPCODEf,
                                                SOC_INDIRECT_MEM_WRITE_OPCODE);
    /* Validate entry */
    soc_mem_field32_set(unit, acc_mem,
                                                dt_entry_w,
                                                VALIDf, 1);

    timeout_usec = soc_property_get(unit, "indirect_access_timeout_usec", timeout_usec);

    /* Write entry in hardware */
    SOC_MEM_BLOCK_ITER(unit, mem, blk) {

        /* Get address to write first */
        maddr = soc_mem_addr_get(unit, mem, array_index, blk, index, &at);

        /* Set address where data has to be written */
        soc_mem_field32_set(unit, acc_mem,
                                                    dt_entry_w,
                                                    ADDRESSf, maddr);
        /* Write entry in indirect access control memory */
        rv = soc_mem_write(unit, acc_mem,  MEM_BLOCK_ALL,
                                               0, dt_entry_w);
        SOC_IF_ERROR_RETURN(rv);

        /* Now Poll for Done Bit in memory to ascertain if
         * indirect access succeded */
        soc_timeout_init(&to, timeout_usec, min_polls);
        done_bit = 0;
        do {
            rv = soc_mem_read(unit, acc_mem, MEM_BLOCK_ALL,
                                             0, dt_entry_r);
            if (SOC_FAILURE(rv)) {
                break;
            }
            done_bit = soc_mem_field32_get(unit, acc_mem,
                                                             dt_entry_r,
                                                                  DONEf);
            if (done_bit) {
                break;
            }

        } while (!soc_timeout_check(&to));

        /* return timeout if done bit is not set */
        if (SOC_SUCCESS(rv) && !done_bit) {
            rv = SOC_E_TIMEOUT;
        }

        /* return if fail otherwise go for another block */
        if (SOC_FAILURE(rv)) {
            cli_out("Indirect Access Write failed for mem=%d,"
                    " index=%d, acc_mem=%d error=%d (%s)\n", mem, index,
                    acc_mem, rv, soc_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

/*
 * Function:
 *  soc_indirect_mem_read
 * Purpose:
 *  This function reads from provided memory indirectly
 *  by writing into BSC_DT_SBUS_ACCESS_CONTROLm.
 * Parameters:
 *  unit(IN) - device identifier
 *  mem (IN) - memory to read
 *  array_index (IN) - array index
 *  copyno (IN) - copy number
 *  index (IN) - index to write
 *  entry_data (OUT) - read data
 * Returns:
 *  SOC_E_NONE - indirect mem read is success.
 *  SOC_E_XXX  - indirect mem read failed.
 * Notes:
 *  Flow tracker memories are shared across multiple
 *  FT stages in IP, MMU & EP. To prevent schan read
 *  write error this indirect method is used to read
 *  into those memories.
 */
int
soc_indirect_mem_read(int unit, soc_mem_t mem,
                      unsigned array_index,
                      int copyno, int index,
                      void *entry_data)
{
    uint8 at = 0, done_bit = 0;
    uint32 maddr = 0;
    soc_timeout_t  to = {0};
    int rv = SOC_E_NONE, blk = 0, min_polls = 10;
    sal_usecs_t timeout_usec = SOC_CONTROL(unit)->ftIndirectMemAccessTimeout;
    bsc_dt_sbus_access_control_entry_t dt_sbus_access_entry_w;
    bsc_dt_sbus_access_control_entry_t dt_sbus_access_entry_r;
    bsc_dt_agg_sbus_access_control_entry_t dt_agg_sbus_access_entry_w;
    bsc_dt_agg_sbus_access_control_entry_t dt_agg_sbus_access_entry_r;
    soc_mem_t acc_mem = BSC_DT_SBUS_ACCESS_CONTROLm;
    uint32 *dt_entry_w = NULL;
    uint32 *dt_entry_r = NULL;

    /* Initialize sbus access entry variable */
    sal_memset(&dt_sbus_access_entry_w, 0, sizeof(dt_sbus_access_entry_w));
    sal_memset(&dt_sbus_access_entry_r, 0, sizeof(dt_sbus_access_entry_r));
    sal_memset(&dt_agg_sbus_access_entry_w, 0, sizeof(dt_agg_sbus_access_entry_w));
    sal_memset(&dt_agg_sbus_access_entry_r, 0, sizeof(dt_agg_sbus_access_entry_r));

    sal_memset(indirect_mem_data_r[unit], 0, SOC_MAX_MEM_WORDS * 4);

    dt_entry_r = (uint32*)&dt_sbus_access_entry_r;
    dt_entry_w = (uint32*)&dt_sbus_access_entry_w;

    if (soc_ft_mem_is_agg_indirect_access(unit, mem)) {
        acc_mem = BSC_DT_AGG_SBUS_ACCESS_CONTROLm;
        dt_entry_r = (uint32*)&dt_agg_sbus_access_entry_r;
        dt_entry_w = (uint32*)&dt_agg_sbus_access_entry_w;

    }

    /* Set read command - opcode 0 */
    soc_mem_field32_set(unit, acc_mem,
                                                dt_entry_w,
                                                OPCODEf,
                                                SOC_INDIRECT_MEM_READ_OPCODE);
    /* Validate entry */
    soc_mem_field32_set(unit, acc_mem,
                                                dt_entry_w,
                                                VALIDf, 1);

    timeout_usec = soc_property_get(unit, "indirect_access_timeout_usec", timeout_usec);

    /* Read entry from hardware through indirect access */
    SOC_MEM_BLOCK_ITER(unit, mem, blk) {

        /* Get address to read first */
        maddr = soc_mem_addr_get(unit, mem, array_index, blk, index, &at);

        /* Set address from where data has to be read */
        soc_mem_field32_set(unit, acc_mem,
                                                    dt_entry_w,
                                                    ADDRESSf, maddr);
        /* Write entry in indirect access control memory */
        rv = soc_mem_write(unit, acc_mem,  MEM_BLOCK_ALL,
                                               0, dt_entry_w);
        SOC_IF_ERROR_RETURN(rv);

        /* Now poll for done bit in memory to ascertain if
         * indirect access succeded */
        soc_timeout_init(&to, timeout_usec, min_polls);
        done_bit = 0;
        do {
            rv = soc_mem_read(unit, acc_mem, MEM_BLOCK_ALL,
                                                  0, dt_entry_r);
            if (SOC_FAILURE(rv)) {
                break;
            }
            done_bit = soc_mem_field32_get(unit, acc_mem,
                                                dt_entry_r, DONEf);
            if (done_bit) {
                soc_mem_field_get(unit, acc_mem,
                                                    dt_entry_r,
                                   DATAf, indirect_mem_data_r[unit]);

                /* Copy entry data from indirect mem data buffer */
                sal_memcpy(entry_data, indirect_mem_data_r[unit],
                                (soc_mem_entry_words(unit, mem) * 4));
                break;
            }

        } while (!soc_timeout_check(&to));

        /* return timeout if done bit is not set */
        if (SOC_SUCCESS(rv) && !done_bit) {
            rv = SOC_E_TIMEOUT;
        }

        /* return if fail otherwise go for another block */
        if (SOC_FAILURE(rv)) {
            cli_out("Indirect Access Read failed for mem=%d,"
                    " index=%d, acc_mem=%d error=%d (%s)\n", mem, index,
                    acc_mem, rv, soc_errmsg(rv));
            return rv;
        }
    }

    return rv;
}
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
#endif /* defined(BCM_ESW_SUPPORT) */
