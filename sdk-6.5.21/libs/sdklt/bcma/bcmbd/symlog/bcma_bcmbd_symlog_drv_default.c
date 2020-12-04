/*! \file bcma_bcmbd_symlog_drv_default.c
 *
 * SDKLT symlog (symbol log) driver.
 *
 * This driver relays on BCMLTM APIs, it would store the last write of each
 * symbol into logical table DEVICE_OP_DSH_INFO.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <bcmbd/bcmbd.h>

#include <bcmltm/bcmltm_device_op_dsh_info.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/bcmbd/bcma_bcmbd_symlog.h>


/* Debug log target definition. */
#define BSL_LOG_MODULE BSL_LS_APPL_SYMLOG

/* Cookie for CMIC/IPROC register searching. */
typedef struct symlog_iter_cookie_s {
    uint32_t addr;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t dyn_info;
} symlog_iter_cookie_t;

/* Tracked operations. */
#define SYMLOG_TRACKED_OPS (1 << BCMLT_PT_OPCODE_FIFO_POP) | \
                           (1 << BCMLT_PT_OPCODE_FIFO_PUSH) | \
                           (1 << BCMLT_PT_OPCODE_SET) | \
                           (1 << BCMLT_PT_OPCODE_MODIFY) | \
                           (1 << BCMLT_PT_OPCODE_INSERT) | \
                           (1 << BCMLT_PT_OPCODE_DELETE)

static uint32_t symlog_tracked_ops = SYMLOG_TRACKED_OPS;


/*******************************************************************************
 * Private functions
 */

static bool
symlog_op_tracked(bcmlt_pt_opcode_t op)
{
    if (symlog_tracked_ops & (1 << op)) {
        return true;
    }
    return false;
}

static int
symlog_cmic_addr_decode(const bcmdrd_symbol_t *symbol, bcmdrd_sid_t sid,
                        void *data)
{
    symlog_iter_cookie_t *cookie = data;
    uint32_t addr = cookie->addr;
    int minidx, maxidx, sz, idx;

    minidx = BCMDRD_SYMBOL_INDEX_MIN_GET(symbol->index);
    maxidx = BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index);
    sz = BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index);

    idx = (addr - symbol->addr) / sz;
    if (idx < minidx || idx > maxidx) {
         /* Continue for the next symbol */
        return 0;
    }

    /* The symbol is found if we get here. */
    cookie->sid = sid;
    cookie->dyn_info.index = idx;
    cookie->dyn_info.tbl_inst = 0;

    /* Stop the symbol iteration. */
    return -1;
}


/*******************************************************************************
 * Driver functions
 */

static int
symlog_schan_log_set(int unit, uint32_t adext, uint32_t addr,
                     uint32_t sym_flags, bcmlt_pt_opcode_t op, int st)
{
    int rv;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t dyn_info;

    if (!symlog_op_tracked(op)) {
        return 0;
    }

    sym_flags = sym_flags & (~BCMDRD_SYMBOL_FLAG_OVERLAY);
    rv = bcmbd_pt_addr_decode(unit, adext, addr, sym_flags, &sid, &dyn_info);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Unable to decode the address 0x%08"PRIx32" to "
                                "PT info (error code %d)\n"),
                     addr, rv));
        return -1;
    }

    rv = bcmltm_device_op_dsh_info_set(unit, sid, op, dyn_info.tbl_inst,
                                       dyn_info.index, st);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Unable to log the symbol operation "
                                "(error code %d)\n"),
                     rv));
        return -1;
    }

    return 0;
}

static int
symlog_cmic_log_set(int unit, uint32_t addr, uint32_t sym_flags,
                    bcmlt_pt_opcode_t op, int st)
{
    int rv;
    bcmdrd_symbols_iter_t iter;
    symlog_iter_cookie_t cookie;
    const bcmdrd_symbols_t *symbols;

    if (!symlog_op_tracked(op)) {
        return 0;
    }

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (!symbols) {
        return -1;
    }

    sal_memset(&cookie, 0, sizeof(cookie));
    cookie.addr = addr;

    sal_memset(&iter, 0, sizeof(iter));
    iter.name = "*";
    iter.pflags = sym_flags & (~BCMDRD_SYMBOL_FLAG_OVERLAY);
    iter.aflags = BCMDRD_SYMBOL_FLAG_OVERLAY;
    iter.symbols = symbols;
    iter.function = symlog_cmic_addr_decode;
    iter.vptr = &cookie;

    if (bcmdrd_symbols_iter(&iter) >= 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Unable to decode the address 0x%08"PRIx32" to "
                                "PT info\n"),
                     addr));
        return -1;
    }

    rv = bcmltm_device_op_dsh_info_set(unit, cookie.sid, op,
                                       cookie.dyn_info.tbl_inst,
                                       cookie.dyn_info.index, st);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Unable to log the symbol operation "
                                "(error code %d)\n"),
                     rv));
        return -1;
    }

    return 0;
}

bcma_bcmbd_symlog_drv_t bcma_bcmbd_symlog_drv_default = {
    .schan_log_set = symlog_schan_log_set,
    .cmic_log_set = symlog_cmic_log_set
};

