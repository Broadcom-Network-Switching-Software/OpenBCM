/*! \file bcma_bcmbd_fieldflags.c
 *
 * BCMBD command field flag utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcma/bcmbd/bcma_bcmbd.h>

typedef struct field_flags_map_s {
    const char *name;
    uint32_t value;
} field_flags_map_t;

static field_flags_map_t field_flags_table[] =
    {
        { "counter",           BCMDRD_SYMBOL_FIELD_FLAG_COUNTER           },
        { "key",               BCMDRD_SYMBOL_FIELD_FLAG_KEY               },
        { "mask",              BCMDRD_SYMBOL_FIELD_FLAG_MASK              },
        { NULL, 0 }
    };

const char *
bcma_bcmbd_fieldflag_type2name(uint32_t flag)
{
    field_flags_map_t *f = field_flags_table;

    for (; f->name; f++) {
        if (f->value == flag) {
            return f->name;
        }
    }
    return NULL;
}

uint32_t
bcma_bcmbd_fieldflag_name2type(const char *name)
{
    field_flags_map_t *fft = field_flags_table;

    for (; fft->name; fft++) {
        if (sal_strcmp(fft->name, name) == 0) {
            return fft->value;
        }
    }
    return 0;
}
