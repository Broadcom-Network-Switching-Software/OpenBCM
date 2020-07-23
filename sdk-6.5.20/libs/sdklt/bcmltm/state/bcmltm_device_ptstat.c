/*! \file bcmltm_device_ptstat.c
 *
 * Functions to update the PT tainted state.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmltd/chip/bcmltd_id.h>

#include <bcmimm/bcmimm_int_comp.h>

#include <bcmlrd/bcmlrd_map.h>

#include <bcmltm/bcmltm_state_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTM_STATE

/*******************************************************************************
 * Local definitions
 */

/*
 * Number of fields.
 *
 * Assume that "DEVICE_OP_PT_INFO" and "DEVICE_OP_DSH_INFO" have the same
 * number of fields.
 */
#define PTSTAT_FIELD_COUNT DEVICE_OP_PT_INFOt_FIELD_COUNT

/* Operations which would taint the PTs. */
#define PTSTAT_TAINTED_OPS (1 << BCMLT_PT_OPCODE_FIFO_POP) | \
                           (1 << BCMLT_PT_OPCODE_FIFO_PUSH) | \
                           (1 << BCMLT_PT_OPCODE_SET) | \
                           (1 << BCMLT_PT_OPCODE_MODIFY) | \
                           (1 << BCMLT_PT_OPCODE_INSERT) | \
                           (1 << BCMLT_PT_OPCODE_DELETE)
static uint32_t ptstat_tainted_ops = PTSTAT_TAINTED_OPS;

/* Tables which are used to track PT direct accesses. */
static uint32_t ptstat_track_tab[] = { DEVICE_OP_PT_INFOt,
                                       DEVICE_OP_DSH_INFOt };


/*******************************************************************************
 * Private functions
 */

/*
 * \brief Check if there is any write log saved in the tracking table.
 *
 * \param [in] unit Unit number.
 * \param [in] famm famm buffer for IMM table traverse.
 * \param [in] sid IMM table to traverse.
 *
 * retval true The device is tainted from \c sid data.
 * retval false The device is clean from \c sid data.
 */
static bool
ptstat_dev_tainted_check(int unit, uint32_t sid)
{
    int rv;
    uint32_t fcnt, idx;
    bcmltd_fields_t in, out;
    bcmltd_field_t *in_flds[PTSTAT_FIELD_COUNT], *out_flds[PTSTAT_FIELD_COUNT];
    bcmltd_field_t in_buf[PTSTAT_FIELD_COUNT], out_buf[PTSTAT_FIELD_COUNT];

    sal_memset(&in, 0, sizeof(in));
    sal_memset(&out, 0, sizeof(out));
    sal_memset(in_flds, 0, sizeof(in_flds));
    sal_memset(out_flds, 0, sizeof(out_flds));
    sal_memset(in_buf, 0, sizeof(in_flds));
    sal_memset(out_buf, 0, sizeof(out_flds));
    fcnt = COUNTOF(in_flds);
    in.count = fcnt;
    in.field = in_flds;
    out.count = fcnt;
    out.field = out_flds;
    for (idx = 0; idx < fcnt; idx++) {
        in_flds[idx] = &in_buf[idx];
        out_flds[idx] = &out_buf[idx];
    }

    rv = bcmimm_entry_get_first(unit, sid, &out);
    while (SHR_SUCCESS(rv)) {
        int pt_id = -1, op = -1, st = -1;

        for (idx = 0; idx < out.count; idx++) {
            /*
             * Assume that the field IDs of DEVICE_OP_PT_INFOt and
             * DEVICE_OP_DSH_INFOt are identical.
             */
            switch (out.field[idx]->id) {
            case DEVICE_OP_PT_INFOt_PT_IDf:
                pt_id = out.field[idx]->data;
                break;
            case DEVICE_OP_PT_INFOt_PT_OPf:
                op = out.field[idx]->data;
                break;
            case DEVICE_OP_PT_INFOt_PT_OP_STATUSf:
                st = out.field[idx]->data;
                break;
            default:
                break;
            }
        }

        if (pt_id != -1 && st == 0 &&
            ((1 << op) & ptstat_tainted_ops) > 0) {
            return true;
        }

        /* Fill key field for getting the next entry */
        in.count = 1;
        in.field[0]->id = DEVICE_OP_PT_INFOt_PT_IDf;
        in.field[0]->data = pt_id;
        out.count = fcnt;
        rv = bcmimm_entry_get_next(unit, sid, &in, &out);
    }

    return false;
}


/*******************************************************************************
 * Public functions
 */

int
bcmltm_dev_ptstat_init(int unit, bool warm)
{
    int idx;

    SHR_FUNC_ENTER(unit);

    if (!warm || !bcmdrd_dev_exists(unit)) {
        SHR_EXIT();
    }

    for (idx = 0; idx < COUNTOF(ptstat_track_tab); idx++) {
        if (ptstat_dev_tainted_check(unit, ptstat_track_tab[idx])) {
            bcmdrd_dev_tainted_set(unit, true);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_dev_ptstat_update(int unit, uint32_t ptid,
                         bcmlt_pt_opcode_t pt_op, bool op_failure)
{
    if ((op_failure == 0) &&
        (ptstat_tainted_ops & (1 << pt_op))) {
        bcmdrd_dev_tainted_set(unit, true);
    }

    return SHR_E_NONE;
}

