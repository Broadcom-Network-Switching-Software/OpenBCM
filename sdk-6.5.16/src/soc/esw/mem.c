/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
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

#endif /* defined(BCM_ESW_SUPPORT) */
