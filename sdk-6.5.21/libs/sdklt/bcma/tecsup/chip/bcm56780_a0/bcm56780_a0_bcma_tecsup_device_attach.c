/*! \file bcm56780_a0_bcma_tecsup_device_attach.c
 *
 * Chip stub for techsupport.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/tecsup/bcma_tecsup_drv.h>

static int
bcm56780_a0_bcma_tecsup_src(const bcmdrd_symbol_t *s, bcmdrd_sid_t sid,
                            void *vptr)
{
    uint32_t *src = vptr;

    *src = s->addr;

    return 0;
}

/*!
 * \brief Get the high precision timer source.
 */
static uint32_t
bcm56780_a0_bcma_tecsup_src_get(int unit, uint32_t sel)
{
    const bcmdrd_symbols_t *symbols;
    bcmdrd_symbols_iter_t iter;
    uint32_t src;
    char *blk, *type, *ctrl, hdl[128];

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (!symbols) {
        return 0;
    }

    switch (sel & 0xf) {
        case 0x0:
            type = "SCHAN";
            break;
        case 0x1:
            type = "SCHAN_FIFO";
            break;
        default:
            type = "FIFODMA";
            break;
    }

    switch ((sel >> 4) & 0xf) {
        case 0x1:
            blk = "CMIC";
            break;
        case 0x3:
            blk = "CMIC_TOP";
            break;
        case 0x4:
            blk = "TOP_SYNCE";
            break;
        case 0xc:
            blk = "CMIC_IPROC";
            break;
        default:
            blk = "CMIC_COMMON_POOL";
            break;
    }

    switch ((sel >> 28) & 0xf) {
        case 0x0:
            ctrl = "MEMm";
            break;
        case 0x1:
            ctrl = "MESSAGEr";
            break;
        default:
            ctrl = "CTRLr";
            break;
    }

    sal_snprintf(hdl, sizeof(hdl), "%s_%s_%s%d_%s",
                 blk, type, "CH", (sel >> 20) & 0xf, ctrl);

    sal_memset(&iter, 0, sizeof(iter));
    iter.name = (const char *)hdl;
    iter.matching_mode = BCMDRD_SYMBOLS_ITER_MODE_EXACT;
    iter.symbols = symbols;
    iter.function = bcm56780_a0_bcma_tecsup_src;
    iter.vptr = &src;

    if (bcmdrd_symbols_iter(&iter) == 0) {
        return 0;
    }

    return src + (((sel >> 12) & 0xff) << ((sel >> 24) & 0xf));
}

int
bcm56780_a0_bcma_tecsup_dev_data_get(int unit, uint32_t sel,
                                     bcma_tecsup_dev_data_t *data)
{
    if (!data) {
        return SHR_E_PARAM;
    }

    data->src = bcm56780_a0_bcma_tecsup_src_get(unit, sel);
    return SHR_E_NONE;
}
