/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */

#include <shared/dbx/dbx_pre_compiled_xml_parser_internal.h>
#if defined(BCM_DNX_SUPPORT)
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#endif

#define _INTERNAL_AUTO_GENERATED_INCLUDES
#define _INTERNAL_AUTO_GENERATED_TOG_GET_CALL_PER_IMAGE

_INTERNAL_AUTO_GENERATED_INCLUDES
#if defined(BCM_DNX_SUPPORT)
#include <src/bcm/dnx/algo/lif_mngr/auto_generated/standard_1_dbx_pre_compiled_lif_mdb_xml_parser.h>
#endif
_INTERNAL_AUTO_GENERATED_INCLUDES

/* *INDENT-OFF* */

#if !defined(NO_FILEIO)
void*
dbx_pre_compiled_lif_phases_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags)
{
    int i;
#if defined(BCM_DNX_SUPPORT)
    uint8 is_image = 0;
    xml_node curTop = NULL;
#endif

    char db_name[RHNAME_MAX_SIZE] = { 0 };
    char base_db_name[RHNAME_MAX_SIZE] = { 0 };
    char _filepath[RHFILE_MAX_SIZE] = { 0 };

    
    if (flags & CONF_OPEN_PER_DEVICE)
    {
        if (dbx_file_get_db(unit, db_name, base_db_name) != _SHR_E_NONE)
        {
            return NULL;
        }
        if (!ISEMPTY(db_name))
        {
            sal_snprintf(_filepath, RHFILE_MAX_SIZE - 1, "%s/internal/%s", db_name, filepath);
        }
    }
    else
    {
        sal_snprintf(_filepath, RHFILE_MAX_SIZE - 1, "%s", filepath);
    }

#if defined(BCM_DNX_SUPPORT)
    if (is_image == 0)
        is_image = 0;
#endif
    for(i = 0; i < 4; i++)
    {
        _INTERNAL_AUTO_GENERATED_TOG_GET_CALL_PER_IMAGE
#if defined(BCM_DNX_SUPPORT)
        dnx_pp_prgm_current_image_check(unit, "standard_1", &is_image);
        if(is_image)
        {
            curTop = dbx_xml_top_get_internal(_filepath, topname, flags, pre_compiled_lif_phases_standard_1_xml_to_buf);
            if(curTop)
            {
                return curTop;
            }
        }
#endif
        _INTERNAL_AUTO_GENERATED_TOG_GET_CALL_PER_IMAGE

        
#if defined(BCM_DNX_SUPPORT)
        curTop = dbx_file_get_xml_top(unit, _filepath, topname, (flags & ~(CONF_OPEN_PER_DEVICE | CONF_OPEN_ALTER_LOC)) | CONF_OPEN_NO_ERROR_REPORT);
        if(curTop)
        {
            return curTop;
        }
#endif

        if (flags & CONF_OPEN_PER_DEVICE)
        {
            sal_memset(_filepath, 0, RHFILE_MAX_SIZE);
            if(i == 0)
            {
                sal_snprintf(_filepath, RHFILE_MAX_SIZE - 1, "%s/%s", db_name, filepath);
            }
            else if(i == 1)
            {
                sal_snprintf(_filepath, RHFILE_MAX_SIZE - 1, "%s/internal/%s", base_db_name, filepath);
            }
            else if(i == 2)
            {
                sal_snprintf(_filepath, RHFILE_MAX_SIZE - 1, "%s/%s", base_db_name, filepath);
            }
        }
        else
        {
            break;
        }
    }

    
    return dbx_file_get_xml_top(unit, filepath, topname, flags);
}
#else
void*
dbx_pre_compiled_lif_phases_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags)
{
    return NULL;
}
#endif

