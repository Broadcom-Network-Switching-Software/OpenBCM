/*! \file dev_misc.h
 *
 * Device misc configuation header file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_DEV_MISC_INT_H
#define BCMI_LTSW_DEV_MISC_INT_H

#include <shr/shr_types.h>

/*! Symbol field indicator. */
#define BCMINT_LTSW_DEV_MISC_FLD_SYM        (1 << 0)

/*! Array field indicator. */
#define BCMINT_LTSW_DEV_MISC_FLD_ARRAY      (1 << 1)

/*! LT field data struture. */
typedef struct bcmint_ltsw_dev_misc_field_t {
    uint32_t    flags;

    const char  *name;

    uint32_t    index;

    union {
        uint64_t scalar;

        const char *str;
    } value;
} bcmint_ltsw_dev_misc_field_t;

/*! LT entry data struture. */
typedef struct bcmint_ltsw_dev_misc_entry_s {
    uint32_t    field_count;

    bcmint_ltsw_dev_misc_field_t   *field_data;
} bcmint_ltsw_dev_misc_entry_t;

/*! Logical table data struture. */
typedef struct bcmint_ltsw_dev_misc_lt_s {
    const char *name;

    uint32_t    flags;

    uint32_t    entry_count;

    bcmint_ltsw_dev_misc_entry_t   *entry_data;
} bcmint_ltsw_dev_misc_lt_t;

/*! Device misc configuration data struture. */
typedef struct bcmint_ltsw_dev_misc_data_s {
    uint32_t    lt_count;

    bcmint_ltsw_dev_misc_lt_t   *lt_data;
} bcmint_ltsw_dev_misc_data_t;

#endif /* BCMI_LTSW_DEV_MISC_INT_H */
