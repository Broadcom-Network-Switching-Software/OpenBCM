/*! \file bcmbd_cmicd_sym_addr.c
 *
 * Generate a symbol regsiter/memory address.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_pt.h>

#include <bcmbd/bcmbd_cmicd.h>
#include <bcmbd/bcmbd_cmicd_dump.h>

/*!
 * Perform symbolic decoding of a register/memory address.
 *
 * \param [in,out] pb Print buffer.
 * \param [in] unit Unit number.
 * \param [in] sym_flags Symbol flags from DRD.
 * \param [in] adext Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 *
 * \retval SHR_E_NONE Address successfuly decoded.
 * \retval SHR_E_FAIL Error decoding address.
 */
static int
bcmbd_cmicd_sym_addr(shr_pb_t *pb, int unit, uint32_t sym_flags,
                     uint32_t adext, uint32_t addr)
{
    int rv;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t dyn_info;

    rv = bcmbd_cmicd_addr_decode(unit, adext, addr, sym_flags, &sid, &dyn_info);
    if (SHR_SUCCESS(rv)) {
        shr_pb_printf(pb, "%s", bcmdrd_pt_sid_to_name(unit, sid));
        if (bcmdrd_pt_index_max(unit, sid) > 0) {
            shr_pb_printf(pb, "[%d]", dyn_info.index);
        }
        if (bcmdrd_pt_is_port_reg(unit, sid)) {
            shr_pb_printf(pb, ".%d", dyn_info.tbl_inst);
        } else if (bcmdrd_pt_num_tbl_inst(unit, sid) > 1) {
            shr_pb_printf(pb, "{%d}", dyn_info.tbl_inst);
        } else {
            /*
             * Number of table instances = 1
             * Check whether DUPLICATE access type SID is accessed in per-pipe
             */
            int dup_acctype, sym_acctype;
            int acc = (adext >> 8) & 0x1f;
            int pi_type = BCMDRD_PIPE_INFO_TYPE_AT_UNIQUE_GLOBAL;
            dup_acctype = bcmdrd_dev_pipe_info(unit, NULL, pi_type);
            sym_acctype = bcmdrd_pt_acctype_get(unit, sid);
            if (sym_acctype == dup_acctype && acc < dup_acctype) {
                shr_pb_printf(pb, "{%d}", acc);
            }
        }
    }
    return rv;
}

int
bcmbd_cmicd_sym_addr_enable(int unit)
{
    return bcmbd_cmicd_dump_addr_func_set(unit, bcmbd_cmicd_sym_addr);
}
