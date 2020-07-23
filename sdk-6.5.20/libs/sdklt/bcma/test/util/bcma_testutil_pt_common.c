/*! \file bcma_testutil_pt_common.c
 *
 * Physical table common utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_assert.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_pt_common.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

int
bcma_testutil_pt_mask_get(int unit, bcmdrd_sid_t sid, int index,
                          uint32_t *mask, int wsize, bool skip_eccp)
{
    bcmdrd_fid_t *fid_list = NULL;
    size_t num_fid, idx;
    int maxbit, minbit, n, w, b;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    if (bcmdrd_pt_is_valid(unit, sid) == false) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if ((uint32_t)wsize < bcmdrd_pt_entry_wsize(unit, sid)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    sal_memset(mask, 0, wsize * sizeof(uint32_t));

    /* general rule : traverse each field and fetch its width */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_fid_list_get(unit, sid, 0, NULL, &num_fid));

    SHR_ALLOC(fid_list, num_fid * sizeof(bcmdrd_fid_t), "bcmaTestFidList");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);

    sal_memset(fid_list, 0, num_fid * sizeof(bcmdrd_fid_t));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_fid_list_get(unit, sid, num_fid, fid_list, &num_fid));

    for (idx = 0; idx < num_fid; idx++) {
        if (skip_eccp == TRUE) {
            bcmdrd_sym_field_info_t finfo;

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_info_get(unit, sid, fid_list[idx], &finfo));

            if ((sal_strstr(finfo.name, "ECC") != NULL) ||
                (sal_strstr(finfo.name, "PARITY") != NULL)) {
                continue;
            }
        }

        maxbit = bcmdrd_pt_field_maxbit(unit, sid, fid_list[idx]);
        minbit = bcmdrd_pt_field_minbit(unit, sid, fid_list[idx]);

        assert(maxbit < (wsize << 5));

        for (n = minbit; n <= maxbit; n++) {
            w = n / 32;
            b = n % 32;

            mask[w] |= (1 << b);
        }
    }

    /* override it this sid needs special handling in chip-specific function */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcma_testutil_drv_pt_mask_override
            (unit, sid, index, mask, wsize), SHR_E_UNAVAIL);

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

void
bcma_testutil_pt_data_dump(const uint32_t *data, int wsize)
{
    int i;

    cli_out("\n..................................................");
    for (i = 0; i< wsize; i++) {
        if (i % 4 == 0) {
            cli_out("\n");
        }
        cli_out("0x%08"PRIx32" ", data[i]);
    }
    cli_out("\n..................................................\n");
}

int
bcma_testutil_pt_data_comp(const uint32_t *data0, const uint32_t *data1,
                           const uint32_t *mask, int wsize)
{
    int i;

    for (i = 0; i < wsize; i++) {
        if (((data0[i] ^ data1[i]) & mask[i]) != 0) {
            return -1;
        }
    }

    return 0;
}

int
bcma_testutil_pt_field_set(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                           int idx, int tbl_inst, uint32_t field_value)
{
    bcmbd_pt_dyn_info_t dyn_info;
    uint32_t pt_data[BCMDRD_MAX_PT_WSIZE], wsize;

    SHR_FUNC_ENTER(unit);

    sal_memset(pt_data, 0, sizeof(pt_data));

    dyn_info.index = idx;
    dyn_info.tbl_inst = tbl_inst;

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    SHR_IF_ERR_EXIT
        (bcmbd_pt_read(unit, sid, &dyn_info, NULL, pt_data, wsize));

    bcmdrd_pt_field_set(unit, sid, pt_data, fid, &field_value);

    SHR_IF_ERR_EXIT
        (bcmbd_pt_write(unit, sid, &dyn_info, NULL, pt_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_pt_field_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                           int idx, int tbl_inst, uint32_t *field_value)
{
    bcmbd_pt_dyn_info_t dyn_info;
    uint32_t pt_data[BCMDRD_MAX_PT_WSIZE], wsize;

    SHR_FUNC_ENTER(unit);

    sal_memset(pt_data, 0, sizeof(pt_data));

    dyn_info.index = idx;
    dyn_info.tbl_inst = tbl_inst;

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    SHR_IF_ERR_EXIT
        (bcmbd_pt_read(unit, sid, &dyn_info, NULL, pt_data, wsize));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, pt_data, fid, field_value));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_pt_clear_all(int unit, bcmdrd_sid_t sid)
{
    bcmbd_pt_dyn_info_t dyn_info;
    uint32_t pt_data[BCMDRD_MAX_PT_WSIZE], pipe_map = 0;
    int inst_num, tbl_inst, start, end, idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &pipe_map));

    sal_memset(pt_data, 0, sizeof(pt_data));

    inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);
    start = bcmdrd_pt_index_min(unit, sid);
    end = bcmdrd_pt_index_max(unit, sid);

    for (tbl_inst = 0; tbl_inst < inst_num; tbl_inst++) {
        if (!(pipe_map & (1 << tbl_inst))) {
            /* Skip invalid table instance. */
            continue;
        }

        for (idx = start; idx <= end; idx++) {
            dyn_info.index = idx;
            dyn_info.tbl_inst = tbl_inst;

            SHR_IF_ERR_EXIT
                (bcmbd_pt_write(unit, sid, &dyn_info, NULL, pt_data));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_pt_inst_adjust(int unit, bcmdrd_sid_t sid, int *inst_start,
                             int *inst_end)
{
    int inst_max = bcmdrd_pt_num_tbl_inst(unit, sid) - 1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(inst_start, SHR_E_PARAM);
    SHR_NULL_CHECK(inst_end, SHR_E_PARAM);

    if (*inst_start < 0) {
        *inst_start = 0;
    }

    if (*inst_end < 0 || *inst_end > inst_max) {
        *inst_end = inst_max;
    }

    if (*inst_start > *inst_end) {
        *inst_start = 0;
        *inst_end = inst_max;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_pt_ser_ctrl(int unit, int sid, bool en)
{
    bcmdrd_pt_ser_en_ctrl_t ctrl_info;
    int ctrl_type = BCMDRD_SER_EN_TYPE_EC, rv;
    uint32_t field_data;

    SHR_FUNC_ENTER(unit);

    if (bcmdrd_pt_attr_is_cam(unit, sid)) {
        ctrl_type = BCMDRD_SER_EN_TYPE_TCAM;
    }

    rv = bcmdrd_pt_ser_en_ctrl_get(unit, ctrl_type, sid, &ctrl_info);
    if (SHR_SUCCESS(rv)) {
        field_data = en ? 1 : 0;

        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_field_set(unit, ctrl_info.sid, ctrl_info.fid, 0,
                                        0, field_data));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_pt_valid_index_range_get(int unit, bcmdrd_sid_t sid, int tbl_inst,
                                       int idx_min, int idx_max, int *idx_start,
                                       int *idx_end)
{
    int is, ie;

    /* Get first valid index. */
    is = idx_min;
    while (is <= idx_max) {
        if (bcmdrd_pt_index_valid(unit, sid, tbl_inst, is)) {
            break;
        }

        is++;
    }

    /* No valid index found. */
    if (is > idx_max) {
        return SHR_E_FAIL;
    }

    /* Get last valid index. */
    ie = is + 1;
    while (ie <= idx_max) {
        if (!bcmdrd_pt_index_valid(unit, sid, tbl_inst, ie)) {
            break;
        }

        ie++;
    };

    *idx_start = is;
    *idx_end = ie - 1;

    return SHR_E_NONE;
}

