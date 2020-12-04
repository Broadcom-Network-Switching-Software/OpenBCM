/*! \file bcma_bcmdrd_port_bitmap.c
 *
 * Create bit range string for a given port bitmap.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcma/bcmdrd/bcma_bcmdrd.h>

int
bcma_bcmdrd_port_bitmap(char *buf, int size, const bcmdrd_pbmp_t *pbmp,
                       const bcmdrd_pbmp_t *mask)
{
    int idx, pbm_no, len;
    const char *cp;
    uint32_t pbm;

    /* Start with an empty string */
    buf[0] = 0;
    len = 0;

    if (pbmp  == NULL || mask == NULL) {
        return -1;
    }

    pbm_no = 0;
    for (idx = (BCMDRD_PBMP_WORD_MAX - 1); idx >= 0; idx--) {
        pbm = BCMDRD_PBMP_WORD_GET(*mask, idx);
        if (pbm || pbm_no) {
            pbm_no++;
        }
        if (pbm_no == 0) {
            continue;
        }
        if (pbm_no == 1) {
            cp = "0x";
        } else {
            cp = "_";
        }
        if ((size - len) > 0) {
            len += sal_snprintf(&buf[len], size - len, "%s", cp);
        }
        pbm = BCMDRD_PBMP_WORD_GET(*pbmp, idx);
        if ((size - len) > 0) {
            len += sal_snprintf(&buf[len], size - len, "%08"PRIx32"", pbm);
        }
    }

    /* Ensure string is properly terminated */
    buf[size - 1] = 0;

    return 0;
}
