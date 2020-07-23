/*! \file ha_str_id.c
 *
 * HA API implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include "ha_internal.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_SHR_HA

/*******************************************************************************
 * Public functions
 */
void shr_ha_id_check(const char *blk_id)
{
    const char *err_id = NULL;

    if (blk_id == NULL) {
        return;
    } else if (*blk_id == '\0') {
        err_id = "<empty>";
    } else if (sal_isupper(*blk_id) ||
               sal_strchr(blk_id, '.') ||
               sal_strchr(blk_id, ' ') ||
               sal_strchr(blk_id, '_')) {
        err_id = blk_id;
    }
    if (err_id) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Invalid ID string: %s\n"), err_id));
    }
}


int shr_ha_str_to_id(const char *str,
                     shr_ha_blk_usage_t usage,
                     size_t len,
                     char *id_str)
{
    int idx = 0;
    size_t out_idx;
    bool is_cap = true;
    char c;

    if (!str || ! id_str || len == 0) {
        return SHR_E_PARAM;
    }
    switch (usage) {
    case SHR_HA_BLK_USAGE_LT:
        id_str[0] = 'l';
        break;
    case SHR_HA_BLK_USAGE_TYPE:
        id_str[0] = 't';
        break;
    case SHR_HA_BLK_USAGE_OTHER:
        id_str[0] = 'o';
        break;
    default:
        return SHR_E_PARAM;
    }
    out_idx = 1;    /* We already wrote the first character */
    while (str[idx] != '\0' && out_idx < len-1) {
        c = (char)sal_tolower(str[idx++]);
        /* Consider '_' and space as delimiters. */
        if (c == '_' || c == ' ') {
            is_cap = true;
            continue;
        }
        /* Ignore non alpha numeric characters */
        if (!sal_isalpha(c) && !sal_isdigit(c)) {
            continue;
        }

        if (is_cap) {
            c = (char)sal_toupper(c);
            is_cap = false;
        }
        id_str[out_idx++] = c;
    }
    id_str[out_idx] = '\0'; /* NULL terminate */
    return SHR_E_NONE;
}
