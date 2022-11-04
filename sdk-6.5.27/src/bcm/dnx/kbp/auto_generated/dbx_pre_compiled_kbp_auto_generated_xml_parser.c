/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */

#include <shared/dbx/dbx_pre_compiled_xml_parser_internal.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

#define _INTERNAL_AUTO_GENERATED_INCLUDES
#define _INTERNAL_AUTO_GENERATED_TOG_GET_CALL_PER_IMAGE

_INTERNAL_AUTO_GENERATED_INCLUDES
#include <bcm_int/dnx/kbp/auto_generated/standard_1_dbx_pre_compiled_kbp_xml_parser.h>
_INTERNAL_AUTO_GENERATED_INCLUDES

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
    uint8 is_image = 0;
    xml_node curTop = NULL;

    if (is_image == 0)
        is_image = 0;
    for(i = 0; i < 1; i++)
    {
        _INTERNAL_AUTO_GENERATED_TOG_GET_CALL_PER_IMAGE
        dnx_pp_prgm_current_image_check(unit, "standard_1", &is_image);
        if(is_image)
        {
            curTop = dbx_xml_top_get_internal(filepath, topname, flags, pre_compiled_kbp_standard_1_xml_to_buf);
            if(curTop)
            {
                return curTop;
            }
        }
        _INTERNAL_AUTO_GENERATED_TOG_GET_CALL_PER_IMAGE

        
        curTop = dbx_file_get_xml_top(unit, filepath, topname, (flags & ~(CONF_OPEN_PER_DEVICE | CONF_OPEN_ALTER_LOC)) | CONF_OPEN_NO_ERROR_REPORT);
        if(curTop)
        {
            return curTop;
        }


    }

    
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
#endif

