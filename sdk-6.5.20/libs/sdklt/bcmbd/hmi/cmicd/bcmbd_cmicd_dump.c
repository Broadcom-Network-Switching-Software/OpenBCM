/*! \file bcmbd_cmicd_dump.c
 *
 * Dump register/memory access data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_cmicd_dump.h>

static bcmbd_dump_addr_f bcmbd_dump_addr_func[BCMDRD_CONFIG_MAX_UNITS];

static void
dump_addr(int unit, shr_pb_t *pb, uint32_t sym_flags,
          uint32_t adext, uint32_t addr)
{
    int rv = SHR_E_FAIL;
    char *addr_begin = "addr: ";
    char *addr_end = "";
    bcmbd_dump_addr_f dump_addr_func = bcmbd_dump_addr_func[unit];

    if (dump_addr_func) {
        rv = dump_addr_func(pb, unit, sym_flags, adext, addr);
        if (SHR_SUCCESS(rv)) {
            addr_begin = " [";
            addr_end = "]";
        }
    }
    shr_pb_printf(pb, "%s0x%04"PRIx32"%08"PRIx32"%s",
                  addr_begin, adext, addr, addr_end);
}

void
bcmbd_cmicd_dump_data(shr_pb_t *pb,
                      uint32_t *data, size_t wsize)
{
    unsigned int idx;

    for (idx = 0; idx < wsize; idx++) {
        shr_pb_printf(pb, " 0x%08"PRIx32, data[idx]);
    }
}

void
bcmbd_cmicd_dump_reg(shr_pb_t *pb, int unit,
                     uint32_t adext, uint32_t addr,
                     uint32_t *data, size_t wsize)
{
    dump_addr(unit, pb, BCMDRD_SYMBOL_FLAG_REGISTER, adext, addr);
    shr_pb_printf(pb, " data:");
    bcmbd_cmicd_dump_data(pb, data, wsize);
}

void
bcmbd_cmicd_dump_mem(shr_pb_t *pb, int unit,
                     uint32_t adext, uint32_t addr,
                     uint32_t *data, size_t wsize)
{
    dump_addr(unit, pb, BCMDRD_SYMBOL_FLAG_MEMORY, adext, addr);
    shr_pb_printf(pb, " data:");
    bcmbd_cmicd_dump_data(pb, data, wsize);
}

int
bcmbd_cmicd_dump_addr_func_set(int unit, bcmbd_dump_addr_f dump_addr_func)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    bcmbd_dump_addr_func[unit] = dump_addr_func;

    return SHR_E_NONE;
}
