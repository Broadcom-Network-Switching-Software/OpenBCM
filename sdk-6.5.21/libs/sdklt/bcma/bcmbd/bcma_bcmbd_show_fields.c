/*! \file bcma_bcmbd_show_fields.c
 *
 * Display fields of a register/memory.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_pb.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_sym_dump.h>

#include <bcma/bcmbd/bcma_bcmbd.h>

int
bcma_bcmbd_dump_fields(shr_pb_t *pb, int unit, const bcmdrd_symbol_t *symbol,
                       uint32_t *data, uint32_t oflags,
                       bcmdrd_symbol_custom_filter_cb_f ccb,
                       void *ccb_cookie)
{
#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
    uint32_t flags = BCMDRD_SYM_DUMP_EXT_HEX;
    const bcmdrd_symbols_t *symbols = bcmdrd_dev_symbols_get(unit, 0);

    if (symbols == NULL) {
        return 0;
    }

    if (oflags & BCMA_BCMBD_CMD_IDF_NONZERO) {
        flags |= BCMDRD_SYM_DUMP_SKIP_ZEROS;
    }
    if (oflags & BCMA_BCMBD_CMD_IDF_COMPACT) {
        flags |= BCMDRD_SYM_DUMP_SINGLE_LINE;
    }

    bcmdrd_sym_dump_fields(pb, symbol, symbols->field_names,
                           data, flags,
                           bcmdrd_symbol_field_filter, data,
                           ccb, ccb_cookie);
#endif /* BCMDRD_CONFIG_INCLUDE_FIELD_INFO */
    return 0;
}

int
bcma_bcmbd_show_fields(int unit, const bcmdrd_symbol_t *symbol,
                       uint32_t *data, uint32_t oflags,
                       bcmdrd_symbol_custom_filter_cb_f ccb,
                       void *ccb_cookie)
{
#if BCMDRD_CONFIG_INCLUDE_FIELD_INFO == 1
    shr_pb_t *pb;

    pb = shr_pb_create();
    bcma_bcmbd_dump_fields(pb, unit, symbol, data, oflags, ccb, ccb_cookie);
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);
#endif /* BCMDRD_CONFIG_INCLUDE_FIELD_INFO */
    return 0;
}
