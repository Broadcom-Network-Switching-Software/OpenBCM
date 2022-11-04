/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */
#if defined(BCM_DNX_SUPPORT)
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#endif /* (BCM_DNX_SUPPORT) */
#include <src/bcm/dnx/field/map/auto_generated/dnx2_dbx_pre_compiled_fwd_app_type_auto_generated_xml_parser.h>

void *
dbx_pre_compiled_fwd_app_types_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags)
{
    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_application_v1))
    {
        return dbx_pre_compiled_dnx2_fwd_app_types_top_get(unit, filepath, topname, flags);
    }
    return NULL;
}
