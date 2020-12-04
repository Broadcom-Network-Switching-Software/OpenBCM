/*! \file field_types.c
 * Purpose: Scan and print different field types
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/types.h>
#include <sal/appl/sal.h>
#include <sal/appl/field_types.h>

static char* sal_field_type_names[] = SAL_FIELD_TYPE_STRINGS;

sal_field_type_e
sal_field_scan_str(char *source, sal_field_type_e in_field_type, void *value, int value_size)
{
    sal_field_type_e out_field_type = SAL_FIELD_TYPE_NONE;

    return out_field_type;
}

void
sal_field_format_str(char *target, int target_size, sal_field_type_e out_field_type, void *value, int value_size)
{

}

/*
 * Return field type name
 */
char*
sal_field_type_str(sal_field_type_e field_type)
{
    if(field_type >= SAL_FIELD_TYPE_MAX)
        field_type = SAL_FIELD_TYPE_NONE;

    return sal_field_type_names[field_type];
}

/*
 * Return field type by name
 */
sal_field_type_e
sal_field_type_by_name(char *field_type_name)
{
    sal_field_type_e i_type;
    for(i_type = SAL_FIELD_TYPE_NONE; i_type < SAL_FIELD_TYPE_MAX; i_type++)
    {
        if(!sal_strcasecmp(sal_field_type_names[i_type], field_type_name))
            break;
    }

    return i_type;
}
