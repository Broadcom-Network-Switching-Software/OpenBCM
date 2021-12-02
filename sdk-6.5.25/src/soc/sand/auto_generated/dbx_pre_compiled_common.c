/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */

#include <shared/dbx/dbx_pre_compiled_xml_parser_internal.h>
#if defined(BCM_DNX2_SUPPORT)
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#endif

#define _INTERNAL_AUTO_GENERATED_INCLUDES
#define _INTERNAL_AUTO_GENERATED_TOG_GET_CALL_PER_IMAGE

_INTERNAL_AUTO_GENERATED_INCLUDES
#if defined(BCM_DNX2_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/sand/auto_generated/common_dbx_pre_compiled.h>
#endif
_INTERNAL_AUTO_GENERATED_INCLUDES

/* *INDENT-OFF* */

#if !defined(NO_FILEIO)
void*
dbx_pre_compiled_common_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags)
{
    int i;

#if defined(BCM_DNX2_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    xml_node curTop = NULL;
#endif




    for(i = 0; i < 1; i++)
    {
        _INTERNAL_AUTO_GENERATED_TOG_GET_CALL_PER_IMAGE

#if defined(BCM_DNX2_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNXF_SUPPORT)
        curTop = dbx_xml_top_get_internal(filepath, topname, flags, pre_compiled_common_common_xml_to_buf);
        if(curTop)
        {
            return curTop;
        }
#endif

        _INTERNAL_AUTO_GENERATED_TOG_GET_CALL_PER_IMAGE


        

#if defined(BCM_DNX2_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNXF_SUPPORT)
        curTop = dbx_file_get_xml_top(unit, filepath, topname, (flags & ~(CONF_OPEN_PER_DEVICE | CONF_OPEN_ALTER_LOC)) | CONF_OPEN_NO_ERROR_REPORT);
        if(curTop)
        {
            return curTop;
        }
#endif



    }

    
    return dbx_file_get_xml_top(unit, filepath, topname, flags);
}
#else
void*
dbx_pre_compiled_common_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags)
{
    return NULL;
}
#endif

