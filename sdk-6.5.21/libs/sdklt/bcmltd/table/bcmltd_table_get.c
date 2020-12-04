/*! \file bcmltd_table_get.c
 *
 * Return the table representation of the given table SID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmltd/bcmltd_internal.h>
#include <bcmltd/bcmltd_table.h>

/*******************************************************************************
 * Public functions
 */

const bcmltd_table_rep_t *
bcmltd_table_get(bcmltd_sid_t sid)
{
    const bcmltd_table_rep_t *tbl = NULL;

    if (sid < bcmltd_table_conf->tables) {
        tbl = bcmltd_table_conf->table[sid];
    }

    return tbl;
}
