/*! \file bcma_bcmpc_encode.c
 *
 * Encode field values for register/memory modification.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bcmpc/bcma_bcmpc.h>

int
bcma_bcmpc_encode_field(const phymod_symbol_t *symbol, const char **fnames,
                        const char *field, const char *value,
                        uint32_t *and_masks, uint32_t *or_masks)
{
#if PHYMOD_CONFIG_INCLUDE_FIELD_INFO == 1
    int v, len;
    phymod_field_info_t finfo;
    char vstr[8 * BCMDRD_MAX_PT_WSIZE + 32];
    uint32_t val[BCMDRD_MAX_PT_WSIZE];


    PHYMOD_SYMBOL_FIELDS_ITER_BEGIN(symbol->fields, finfo, fnames) {

        if (sal_strcasecmp(finfo.name, field)) {
            continue;
        }

        if (!bcma_cli_parse_is_int(value)) {
            return -1;
        }

        sal_strlcpy(vstr, value, sizeof(vstr));

        sal_memset(val, 0, sizeof(val));
        phymod_field_set(and_masks, finfo.minbit, finfo.maxbit, val);

        /*
         * If the field value starts with 0x the accept values
         * spanning multiple words, e.g. 0x112233445566.
         */
        v = 0;
        if (vstr[0] == '0' && (vstr[1] == 'x' || vstr[1] == 'X')) {
            while ((len = sal_strlen(vstr)) > 10) {
                len -= 8;
                val[v++] = sal_strtoul(&vstr[len], NULL, 16);
                vstr[len] = 0;
            }
        }
        if (bcma_cli_parse_uint32(vstr, &val[v]) < 0) {
            return -1;
        }
        phymod_field_set(or_masks, finfo.minbit, finfo.maxbit, val);

        return 0;
    } PHYMOD_SYMBOL_FIELDS_ITER_END();

#endif /* PHYMOD_CONFIG_INCLUDE_FIELD_INFO */

    return -1;
}

int
bcma_bcmpc_encode_fields_from_tokens(const phymod_symbol_t *symbol,
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
            return bcma_bcmpc_parse_error("field", ctok->str);
        }
        sal_memset(and_masks, 0, max * sizeof(*and_masks));
        for (idx = 0; idx < max; idx++) {
            or_masks[idx] = data;
        }
    } else if (ctok->argc > 0 && bcma_cli_parse_is_int(ctok->argv[0])) {
        /* All tokens are treated as 32-bit data words */
        for (idx = 0; ctok->argc; idx++, ctok++) {
            if (bcma_cli_parse_uint32(ctok->argv[0], &or_masks[idx]) < 0) {
                return bcma_bcmpc_parse_error("field", ctok->str);
            }
            and_masks[idx] = 0;
        }
    } else {
        /* All tokens are treated as field=value */
        for (idx = 0; ctok->argc && idx < ctoks_num; idx++, ctok++) {
            if (bcma_bcmpc_encode_field(symbol, fnames,
                                        ctok->argv[0], ctok->argv[1],
                                        and_masks, or_masks)) {
                return bcma_bcmpc_parse_error("field", ctok->str);
            }
        }
    }

    return 0;
}
