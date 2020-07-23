/*! \file bcma_bcmbd_encode.c
 *
 * Encode field values for register/memory modification.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_field.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/bcmbd/bcma_bcmbd.h>

int
bcma_bcmbd_encode_field(const bcmdrd_symbol_t *symbol, const char **fnames,
                        const char *field, const char *value,
                        uint32_t *and_masks, uint32_t *or_masks)
{
#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
    int wsize;
    bcmdrd_sym_field_info_t finfo;
    char vstr[8 * BCMDRD_MAX_PT_WSIZE + 32];
    uint32_t val[BCMDRD_MAX_PT_WSIZE];


    BCMDRD_SYM_FIELDS_ITER_BEGIN(symbol->fields, finfo, fnames) {

        if (sal_strcasecmp(finfo.name, field)) {
            continue;
        }

        sal_strlcpy(vstr, value, sizeof(vstr));

        sal_memset(val, 0, sizeof(val));
        if (symbol->flags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
            wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index));
            bcmdrd_field_be_set(and_masks, wsize, finfo.minbit, finfo.maxbit, val);
        } else {
            bcmdrd_field_set(and_masks, finfo.minbit, finfo.maxbit, val);
        }

        if (bcma_cli_parse_array_uint32(vstr, BCMDRD_MAX_PT_WSIZE, val) < 0) {
            return -1;
        }

        if (symbol->flags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
            wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index));
            bcmdrd_field_be_set(or_masks, wsize, finfo.minbit, finfo.maxbit, val);
        } else {
            bcmdrd_field_set(or_masks, finfo.minbit, finfo.maxbit, val);
        }

        return 0;
    } BCMDRD_SYM_FIELDS_ITER_END();

#endif /* BCMDRD_CONFIG_INCLUDE_FIELD_INFO */

    return -1;
}

int
bcma_bcmbd_encode_fields_from_tokens(const bcmdrd_symbol_t *symbol,
                                     const char **fnames,
                                     int ctoks_num,
                                     const bcma_cli_tokens_t *ctoks,
                                     int max,
                                     uint32_t *and_masks,
                                     uint32_t *or_masks)
{
    const bcma_cli_tokens_t *ctok = ctoks;
    uint32_t data;
    int idx;

    /* Initialize masks */
    sal_memset(and_masks, ~0, max * sizeof(*and_masks));
    sal_memset(or_masks, 0, max * sizeof(*or_masks));

    if (ctok->argc == 2 && sal_strcmp(ctok->argv[0], "all") == 0) {
        /* All 32-bit data words are assigned the same value */
        if (bcma_cli_parse_uint32(ctok->argv[1], &data) < 0) {
            return bcma_cli_parse_error("field", ctok->str);
        }
        sal_memset(and_masks, 0, max * sizeof(*and_masks));
        for (idx = 0; idx < max; idx++) {
            or_masks[idx] = data;
        }
    } else if (ctok->argc > 0 && bcma_cli_parse_is_int(ctok->argv[0])) {
        /* All tokens are treated as 32-bit data words */
        for (idx = 0; ctok->argc; idx++, ctok++) {
            if (bcma_cli_parse_uint32(ctok->argv[0], &or_masks[idx]) < 0) {
                return bcma_cli_parse_error("field", ctok->str);
            }
            and_masks[idx] = 0;
        }
    } else {
        /* All tokens are treated as field=value */
        for (idx = 0; ctok->argc && idx < ctoks_num; idx++, ctok++) {
            if (bcma_bcmbd_encode_field(symbol, fnames,
                                        ctok->argv[0], ctok->argv[1],
                                        and_masks, or_masks)) {
                return bcma_cli_parse_error("field", ctok->str);
            }
        }
    }

    return 0;
}
