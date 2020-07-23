/*! \file bcma_bcmbd_convert.c
 *
 * Utility functions for name strings and flags convertion.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/bcmbd/bcma_bcmbd.h>

int
bcma_bcmbd_info_block_type_get(int unit, int idx, uint32_t info)
{
    int bt;

    if (idx >= BCMA_BCMBD_MAX_BLKTYPES_PER_SYM) {
        return -1;
    }

    bt = BCMDRD_SYM_INFO_BLKTYPE(info, idx);
    return (bt > 0) ? bt : -1;
}

int
bcma_bcmbd_info_block_type_valid(int unit, int blktype, uint32_t info)
{
    int bt, idx = 0;

    while ((bt = BCMDRD_SYM_INFO_BLKTYPE(info, idx++)) > 0) {
        if (blktype == bt) {
            return 1;
        }
    };
    return 0;
}

int
bcma_bcmbd_tok2flags(int unit, const bcma_cli_tokens_t *ctok,
                     uint32_t *pflags, uint32_t *aflags)
{
    int idx;

    *pflags = 0;
    *aflags = 0;

    /*
     * The ctok stores the parsed result of flags=<v1>,<v2>,...<vn>
     * in argument order of flags, <v1>, <v2>, ...<vn>.
     * The flags conversion starts from argument index 1 to parsing
     * <v1>, <v2>, ...<vn>
     */
    for (idx = 1; idx < ctok->argc; idx++) {
        uint32_t flag = 0;
        char *s = ctok->argv[idx];
        int not_set = 0;

        /* Make sure this flag is 1 or 0 ?*/
        if (s[0] == '!') {
            /* Flag must be zero */
            not_set = 1;
            s++;
        }

        if (bcma_cli_parse_uint32(s, &flag) < 0) {
            flag = bcma_bcmbd_symflag_name2type(s);
            if (flag == 0) {
                return bcma_cli_parse_error(ctok->argv[0], s);
            }
        }

        /*
         * Add the result to the correct flag
         */
        if (not_set) {
            *aflags |= flag;
        } else {
            *pflags |= flag;
        }
    }
    return 0;
}

int
bcma_bcmbd_tok2fieldflags(int unit, const bcma_cli_tokens_t *ctok,
                          uint32_t *pflags, uint32_t *aflags)
{
    int idx;

    if (ctok == NULL || pflags == NULL || aflags == NULL) {
        return 0;
    }

    *pflags = 0;
    *aflags = 0;

    /*
     * The ctok stores the parsed result of ff=<v1>,<v2>,...<vn>
     * in argument order of ff, <v1>, <v2>, ...<vn>.
     * The field flags conversion starts from argument index 1 to parsing
     * <v1>, <v2>, ...<vn>
     */
    for (idx = 1; idx < ctok->argc; idx++) {
        uint32_t flag = 0;
        char *s = ctok->argv[idx];
        int not_set = 0;

        /* Make sure this flag is 1 or 0 ?*/
        if (s[0] == '!') {
            /* Flag must be zero */
            not_set = 1;
            s++;
        }

        if (bcma_cli_parse_uint32(s, &flag) < 0) {
            flag = bcma_bcmbd_fieldflag_name2type(s);
            if (flag == 0) {
                return bcma_cli_parse_error(ctok->argv[0], s);
            }
        }

        /*
         * Add the result to the correct flag
         */
        if (not_set) {
            *aflags |= flag;
        } else {
            *pflags |= flag;
        }
    }
    return 0;
}
