/*! \file bcmdrd_sym_field_filter.c
 *
 * Default field-view filtering function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>

int
bcmdrd_symbol_field_filter(const bcmdrd_symbol_t *symbol, const char **fnames,
                           const char *encoding, void *cookie)
{
#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
    uint32_t *data = (uint32_t *)cookie;
    uint32_t val[BCMDRD_MAX_PT_WSIZE];
    bcmdrd_sym_field_info_t finfo;
    char tstr[128];
    char *keyfield, *keyvals;
    char *ptr;
    int wsize;
    int kval = -1;

    /* Do not filter if no (or unknown) encoding */
    if (encoding == NULL || *encoding != '{') {
        return 0;
    }

    /* Do not filter if encoding cannot be parsed */
    sal_strlcpy(tstr, encoding, sizeof(tstr));
    ptr = tstr;
    if ((ptr = sal_strchr(ptr, ':')) == NULL) {
        return 0;
    }
    *ptr++ = 0;
    keyfield = ptr;
    if ((ptr = sal_strchr(ptr, ':')) == NULL) {
        return 0;
    }
    *ptr++ = 0;
    keyvals = ptr;

    /* Only show default view if no context */
    if (data == NULL) {
        return (sal_strstr(keyvals, "-1") == NULL) ? 1 : 0;
    }

    
    if ((ptr = sal_strchr(keyfield, '.')) != NULL) {
        return 0;
    }

    /* Look for <keyfield> in data entry */
    BCMDRD_SYM_FIELDS_ITER_BEGIN(symbol->fields, finfo, fnames) {

        if (finfo.name && sal_strcmp(finfo.name, keyfield) == 0) {
            /* Get normalized field value */
            sal_memset(val, 0, sizeof(val));
            if (symbol->flags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
                wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index));
                bcmdrd_field_be_get(data, wsize, finfo.minbit, finfo.maxbit, val);
            } else {
                bcmdrd_field_get(data, finfo.minbit, finfo.maxbit, val);
            }
            kval = val[0];
            break;
        }

    } BCMDRD_SYM_FIELDS_ITER_END();

    /* Check if current key matches any <keyval> in encoding */
    ptr = keyvals;
    while (ptr) {
        if (sal_atoi(ptr) == kval) {
            return 0;
        }
        if ((ptr = sal_strchr(ptr, '|')) != NULL) {
            ptr++;
        }
    }
#endif /* BCMDRD_CONFIG_INCLUDE_FIELD_INFO */

    /* No match - filter this field */
    return 1;
}
