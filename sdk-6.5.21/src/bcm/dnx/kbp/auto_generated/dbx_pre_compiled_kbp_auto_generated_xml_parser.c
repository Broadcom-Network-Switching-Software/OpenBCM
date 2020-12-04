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
#include <bcm_int/dnx/kbp/auto_generated/standard_1_dbx_pre_compiled_kbp_xml_parser.h>
/** Auto-generated includes */
#endif /* (BCM_DNX_SUPPORT) */

/* *INDENT-OFF* */

#if !defined(NO_FILEIO)
void*
dbx_pre_compiled_kbp_top_get(
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


    for(i = 0; i < 1; i++)
    {
        /** Auto-generated top_get call per image.*/

#if defined(BCM_DNX_SUPPORT)
        dnx_pp_prgm_current_image_check(unit, "standard_1", &is_image);
        if(is_image)
        {
            curTop = dbx_xml_top_get_internal(filepath, topname, flags, pre_compiled_kbp_standard_1_xml_to_buf);
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
        curTop = dbx_file_get_xml_top(unit, filepath, topname, (flags & ~(CONF_OPEN_PER_DEVICE | CONF_OPEN_ALTER_LOC)) | CONF_OPEN_NO_ERROR_REPORT);
        if(curTop)
        {
            return curTop;
        }
#endif /* (BCM_DNX_SUPPORT) */



    }
    /*
     * We can't find non-fast-prototype.
     */
    return dbx_file_get_xml_top(unit, filepath, topname, flags);
}
#else
void*
dbx_pre_compiled_kbp_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags)
{
    return NULL;
}
#endif  /* !(NO_FILEIO) */
/* *INDENT-ON* */

