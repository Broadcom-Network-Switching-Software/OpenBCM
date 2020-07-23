/*! \file bcmdrd_sym_field.c
 *
 * BCMDRD symbol field routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_symbols.h>

static int
field_desc_size(uint32_t fdw)
{
    if (BCMDRD_SYM_FIELD_EXT2(fdw)) {
        return 3;
    }
    if (BCMDRD_SYM_FIELD_EXT(fdw)) {
        return 2;
    }
    return 1;
}

static void
field_info_get(uint32_t *fp, bcmdrd_sym_field_info_t *finfo, int *fd_sz,
               const char **fnames)
{
    int width;
    int num_words;

    /* Get size of field encoding */
    num_words = field_desc_size(*fp);

    if (finfo) {
        sal_memset(finfo, 0, sizeof(*finfo));
        switch (num_words) {
        case 3:
            /* Triple Word */
            finfo->fid = BCMDRD_SYM_FIELD_EXT2_ID_GET(*fp);
            finfo->minbit = BCMDRD_SYM_FIELD_EXT2_MIN_GET(*fp);
            width = BCMDRD_SYM_FIELD_EXT2_WIDTH_GET(*fp);
            finfo->view = BCMDRD_SYM_FIELD_EXT2_VIEW_GET(*fp);
            if (BCMDRD_SYM_FIELD_EXT2_ATTR_IS_COUNTER(*fp)) {
                finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_COUNTER;
            }
            if (BCMDRD_SYM_FIELD_EXT2_ATTR_IS_KEY(*fp)) {
                finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_KEY;
            }
            if (BCMDRD_SYM_FIELD_EXT2_ATTR_IS_MASK(*fp)) {
                finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_MASK;
            }
            break;
        case 2:
            /* Double Word */
            finfo->fid = BCMDRD_SYM_FIELD_EXT_ID_GET(*fp);
            finfo->minbit = BCMDRD_SYM_FIELD_EXT_MIN_GET(*fp);
            width = BCMDRD_SYM_FIELD_EXT_WIDTH_GET(*fp);
            finfo->view = -1;
            if (BCMDRD_SYM_FIELD_EXT_ATTR_IS_COUNTER(*fp)) {
                finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_COUNTER;
            }
            if (BCMDRD_SYM_FIELD_EXT_ATTR_IS_KEY(*fp)) {
                finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_KEY;
            }
            if (BCMDRD_SYM_FIELD_EXT_ATTR_IS_MASK(*fp)) {
                finfo->flags |= BCMDRD_SYMBOL_FIELD_FLAG_MASK;
            }
            break;
        default:
            /* Assume Single Word */
            finfo->fid = BCMDRD_SYM_FIELD_ID_GET(*fp);
            finfo->minbit = BCMDRD_SYM_FIELD_MIN_GET(*fp);
            width = BCMDRD_SYM_FIELD_WIDTH_GET(*fp);
            finfo->view = -1;
            break;
        }

        finfo->maxbit = finfo->minbit + width - 1;
        finfo->name = NULL;

        if (fnames) {
            finfo->name = fnames[finfo->fid];
        }
    }

    if (fd_sz) {
        *fd_sz = num_words;
    }
}

static uint32_t *
field_desc_get(uint32_t *fp, bcmdrd_fid_t fid)
{
    int num_words;

    while (fp) {
        /* Get size of field encoding */
        num_words = field_desc_size(*fp);

        /* Check field ID */
        switch (num_words) {
        case 3:
            /* Triple Word */
            if (fid == BCMDRD_SYM_FIELD_EXT2_ID_GET(*fp)) {
                return fp;
            }
            break;
        case 2:
            /* Double Word */
            if (fid == BCMDRD_SYM_FIELD_EXT_ID_GET(*fp)) {
                return fp;
            }
            break;
        default:
            /* Assume Single Word */
            if (fid == BCMDRD_SYM_FIELD_ID_GET(*fp)) {
                return fp;
            }
            break;
        }

        if (BCMDRD_SYM_FIELD_LAST(*fp)) {
            break;
        }

        /* Move to next field descriptor */
        fp += num_words;
    }

    return NULL;
}

static uint32_t *
field_desc_get_by_name(uint32_t *fp, const char **fnames, const char *fname)
{
    int num_words;
    bcmdrd_fid_t fid;

    while (fp) {
        /* Get size of field encoding */
        num_words = field_desc_size(*fp);

        /* Check field ID */
        switch (num_words) {
        case 3:
            /* Triple Word */
            fid = BCMDRD_SYM_FIELD_EXT2_ID_GET(*fp);
            if (sal_strcasecmp(fname, fnames[fid]) == 0) {
                return fp;
            }
            break;
        case 2:
            /* Double Word */
            fid = BCMDRD_SYM_FIELD_EXT_ID_GET(*fp);
            if (sal_strcasecmp(fname, fnames[fid]) == 0) {
                return fp;
            }
            break;
        default:
            /* Assume Single Word */
            fid = BCMDRD_SYM_FIELD_ID_GET(*fp);
            if (sal_strcasecmp(fname, fnames[fid]) == 0) {
                return fp;
            }
            break;
        }

        if (BCMDRD_SYM_FIELD_LAST(*fp)) {
            break;
        }

        /* Move to next field descriptor */
        fp += num_words;
    }

    return NULL;
}

int
bcmdrd_sym_fid_list_get(const bcmdrd_symbols_t *symbols,
                        bcmdrd_sid_t sid, size_t list_max,
                        bcmdrd_fid_t *fid_list, size_t *num_fid)
{
    const bcmdrd_symbol_t *symbol;
    size_t idx;
    bcmdrd_fid_t fid;
    uint32_t *fp;
    int num_words;

    if (symbols == NULL) {
        return -1;
    }
    if (sid >= symbols->size) {
        return -1;
    }
    symbol = bcmdrd_sym_info_get(symbols, sid, NULL);
    if (symbol == NULL) {
        return -1;
    }

    fp = symbol->fields;
    if (fp == NULL) {
        *num_fid = 0;
        return 0;
    }

    idx = 0;

    while (fp) {
        /* Get size of field encoding */
        num_words = field_desc_size(*fp);

        /* Check field ID */
        switch (num_words) {
        case 3:
            /* Triple Word */
            fid = BCMDRD_SYM_FIELD_EXT2_ID_GET(*fp);
            break;
        case 2:
            /* Double Word */
            fid = BCMDRD_SYM_FIELD_EXT_ID_GET(*fp);
            break;
        default:
            /* Assume Single Word */
            fid = BCMDRD_SYM_FIELD_ID_GET(*fp);
            break;
        }
        if (idx < list_max) {
            if (fid_list == NULL) {
                return -1;
            }
            fid_list[idx] = fid;
        }
        idx++;
        if (BCMDRD_SYM_FIELD_LAST(*fp)) {
            break;
        }
        /* Move to next field descriptor */
        fp += num_words;
    }
    *num_fid = idx;

    return 0;
}

const bcmdrd_symbol_t *
bcmdrd_sym_field_info_get(const bcmdrd_symbols_t *symbols,
                          bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                          bcmdrd_sym_field_info_t *finfo)
{
    uint32_t *fp;
    const bcmdrd_symbol_t *symbol;

    if (symbols == NULL) {
        return NULL;
    }
    if (sid >= symbols->size) {
        return NULL;
    }
    symbol = &symbols->symbols[sid];
    fp = field_desc_get(symbol->fields, fid);
    if (fp == NULL) {
        return NULL;
    }
    field_info_get(fp, finfo, NULL, symbols->field_names);
    return symbol;
}

const bcmdrd_symbol_t *
bcmdrd_sym_field_info_get_by_name(const bcmdrd_symbols_t *symbols,
                                  bcmdrd_sid_t sid, const char *fname,
                                  bcmdrd_sym_field_info_t *finfo)
{
    uint32_t *fp;
    const bcmdrd_symbol_t *symbol;

    if (symbols == NULL) {
        return NULL;
    }
    if (sid >= symbols->size) {
        return NULL;
    }
    symbol = &symbols->symbols[sid];
    fp = field_desc_get_by_name(symbol->fields, symbols->field_names, fname);
    if (fp == NULL) {
        return NULL;
    }
    field_info_get(fp, finfo, NULL, symbols->field_names);
    return symbol;
}


uint32_t *
bcmdrd_sym_field_info_decode(uint32_t *fp, const char **fnames,
                             bcmdrd_sym_field_info_t *finfo)
{
    int fd_sz = 1;

    if (!fp) {
        return NULL;
    }

    field_info_get(fp, finfo, &fd_sz, fnames);

    if (BCMDRD_SYM_FIELD_LAST(*fp)) {
        return NULL;
    }

    /* Return pointer to next field descriptor */
    return fp + fd_sz;
}

uint32_t
bcmdrd_sym_field_info_count(uint32_t *fp)
{
    int count = 0;

    while (fp) {
        fp = bcmdrd_sym_field_info_decode(fp, NULL, NULL);
        count++;
    }
    return count;
}
