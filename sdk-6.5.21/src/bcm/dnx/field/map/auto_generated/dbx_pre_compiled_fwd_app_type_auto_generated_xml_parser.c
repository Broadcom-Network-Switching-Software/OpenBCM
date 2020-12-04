/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */

#include <shared/dbx/dbx_pre_compiled_xml_parser_internal.h>
#if defined(BCM_DNX_SUPPORT)
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
/** Auto-generated includes */
#include <src/bcm/dnx/field/map/auto_generated/standard_1_dbx_pre_compiled_fwd_app_type_xml_parser.h>
/** Auto-generated includes */
#endif /* (BCM_DNX_SUPPORT) */

/* *INDENT-OFF* */

#if !defined(NO_FILEIO)
void*
dbx_pre_compiled_fwd_app_types_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags)
{
    int i;

#if defined(BCM_DNX_SUPPORT)
    uint8 is_image = 0;
    xml_node curTop = NULL;
#endif /* (BCM_DNX_SUPPORT) */


    char db_name[RHNAME_MAX_SIZE] = { 0 };
    char base_db_name[RHNAME_MAX_SIZE] = { 0 };
    char _filepath[RHFILE_MAX_SIZE] = { 0 };

    /** PER_DEVICE flag means that specific file is under device db */
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

    for(i = 0; i < 4; i++)
    {
        /** Auto-generated top_get call per image.*/

#if defined(BCM_DNX_SUPPORT)
        dnx_pp_prgm_current_image_check(unit, "standard_1", &is_image);
        if(is_image)
        {
            curTop = dbx_xml_top_get_internal(_filepath, topname, flags, pre_compiled_fwd_app_types_standard_1_xml_to_buf);
            if(curTop)
            {
                return curTop;
            }
        }
#endif /* (BCM_DNX_SUPPORT) */

        /** Auto-generated top_get call per image.*/

        /*
         * We can't find non-fast-prototype updated file path.
         */

#if defined(BCM_DNX_SUPPORT)
        curTop = dbx_file_get_xml_top(unit, _filepath, topname, (flags & ~(CONF_OPEN_PER_DEVICE | CONF_OPEN_ALTER_LOC)) | CONF_OPEN_NO_ERROR_REPORT);
        if(curTop)
        {
            return curTop;
        }
#endif /* (BCM_DNX_SUPPORT) */



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
    /*
     * We can't find non-fast-prototype.
     */
    return dbx_file_get_xml_top(unit, filepath, topname, flags);
}
#else
void*
dbx_pre_compiled_fwd_app_types_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags)
{
    return NULL;
}
#endif  /* !(NO_FILEIO) */
/* *INDENT-ON* */

