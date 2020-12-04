/*! \file bcma_cli_parse_cmp.c
 *
 * Compare a CLI command against a string.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli_parse.h>

int
bcma_cli_parse_abbr_cmp(const char *ref, const char *str, const char term)
{
    const char *tp, *ts;

    tp = ref;
    ts = str;

    /* Validate the input parameters */
    if (!tp && !ts) {
        return TRUE;
    } else if (!tp || !ts) {
        return FALSE;
    }
    if (!*tp && !*ts) {
        return TRUE;
    } else if (!*tp || !*ts) {
        return FALSE;
    }

    while (*tp) {
        /* Check for Upper case match only */
        if (sal_isupper(*tp) || sal_isdigit(*tp)) {
            if (*tp == sal_toupper(*ts)) {
                ts++;
            } else {
                break;
            }
        }
        tp++;
    }

    return (!*tp && ((*ts == term) || !*ts));
}

int
bcma_cli_parse_cmp(const char *ref, const char *str, const char term)
{
    const char *tp, *ts;

    tp = ref;
    ts = str;

    /* Validate the input parameters */
    if (!tp && !ts) {
        return TRUE;
    } else if (!tp || !ts) {
        return FALSE;
    }
    if (!*tp && !*ts) {
        return TRUE;
    } else if (!*tp || !*ts) {
        return FALSE;
    }

    /* Check for complete string match */
    while ((*ts != term) && (*tp) &&
           (sal_toupper(*ts) == sal_toupper(*tp))) {
        ts++;
        tp++;
    }

    /* We may terminate either on "term" character or end of string */
    if (!*tp && ((*ts == term) || !*ts)) {
        return TRUE;
    }

    /*
     * If we failed above, check if we matched up to a point where
     * no required characters are required (i.e. "abc" will match "ABcd"
     * but not "ABcdE". This (so far) appears to always do what is
     * expected.
     */

    if (*ts == term) {
        /* Ok - check pattern */
        while (*tp && !sal_isupper(*tp) && !sal_isdigit(*tp)) {
            tp++;
        }
        if (!*tp) {
            /* Matched */
            return TRUE;
        }
    }

    /* Check for Upper case match only */
    return bcma_cli_parse_abbr_cmp(ref, str, term);
}
