/*! \file bcmdi_flexcode.c
 *
 * BCMDI FlexCode Loader
 *
 * This module contains the implementation of FlexCode loader functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>
#include <sal/sal_time.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmdi/bcmdi.h>
#include <bcmdi/bcmdi_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMDI_FLEXCODE

static bool flc_ptm_commit_notify_flag = FALSE;
static bcmdi_flc_debug_switch_t flc_dbg_switch[BCMDRD_CONFIG_MAX_UNITS] = {{0}};

/*!
 *
 * \brief FLC PTM commit notify handler
 *
 * Update the value of global variable flc_ptm_commit_notify_flag if PTM commit
 * notify has been received. Variable flc_ptm_commit_notify_flag must to be set
 * to FALSE before using this handler
 */
static void
flc_ptm_notify(uint32_t trans_id, shr_error_t status, void *context)
{
    if (SHR_FAILURE(status)) {
        LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META("Failure reported in PTM notify callback\n")));
        return;
    }
    flc_ptm_commit_notify_flag = TRUE;
}

static int
flc_auth_start(int unit, const bcmdi_flc_file_auth_t *auth) {
    uint8_t data_bytes[DI_MAX_ENTRY_BYTES] = {0};
    uint8_t *dst = NULL;

    SHR_FUNC_ENTER(unit);

    if (auth == NULL) {
        SHR_EXIT();
    }

    if (bcmdrd_feature_is_real_hw(unit) == FALSE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Not real HW. Skip Flexcode auth start.\n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmdi_entry_load_to_host(unit, data_bytes, auth->valid.data,
                                  auth->valid.size));

    dst = (uint8_t *) (auth->valid.addr);
    sal_memcpy(dst, data_bytes, auth->valid.size);
    LOG_VERBOSE(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Flexcode authentication started\n")));
exit:
    SHR_FUNC_EXIT();
}

static int
flc_auth_load(int unit, const bcmdi_flc_file_auth_t *auth) {
    uint8_t data_bytes[DI_MAX_ENTRY_BYTES] = {0};
    uint8_t *dst = NULL;

    SHR_FUNC_ENTER(unit);

    if (auth == NULL) {
        SHR_EXIT();
    }

    if (bcmdrd_feature_is_real_hw(unit) == FALSE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Not real HW. Skip Flexcode auth loading.\n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmdi_entry_load_to_host(unit, data_bytes, auth->aeal.data,
                                  auth->aeal.size));

    dst = (uint8_t *) (auth->aeal.addr);
    sal_memcpy(dst, data_bytes, auth->aeal.size);
    LOG_VERBOSE(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Loading AEAL done (%d B)\n"),
                         auth->aeal.size));

    SHR_IF_ERR_EXIT
        (bcmdi_entry_load_to_host(unit, data_bytes, auth->signature.data,
                                  auth->signature.size));

    dst = (uint8_t *) (auth->signature.addr);
    sal_memcpy(dst, data_bytes, auth->signature.size);
    LOG_VERBOSE(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Loading Flexcode signature done (%d B)\n"),
                         auth->signature.size));

exit:
    SHR_FUNC_EXIT();
}

/*!
 *
 * \brief Load flexcode to device using PTM modelled path
 *
 * Commit transcation when a predefined number of memory writes
 * is reached and increase transcation ID.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors
 */
static int
flc_hw_write(int unit, char *type, uint32_t trans_id) {

    bcmbd_pt_dyn_info_t pt_info;
    uint32_t rsp_entry_wsize;
    int i, j;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint32_t count = 0;
    const bcmdi_flc_file_info_t *file_info = NULL;
    const bcmdi_flc_file_entry_t *entry = NULL;
    const uint8_t *entry_vals = NULL;
    const bcmdi_flc_file_auth_t *file_auth = NULL;
    uint8_t src_bytes[DI_MAX_ENTRY_BYTES] = {0};
    uint8_t entry_bytes[DI_MAX_ENTRY_BYTES] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdi_flexcode_file_info_get(unit, type, &file_info));
    if (file_info == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Flexcode is absent for configured variant!\n")));
        SHR_FUNC_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmdi_flexcode_entry_list_get(unit, type, &entry));
    SHR_NULL_CHECK(entry, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (bcmdi_flexcode_entry_vals_list_get(unit, type, &entry_vals));
    SHR_NULL_CHECK(entry_vals, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (bcmdi_flexcode_file_auth_get(unit, &file_auth));

    for (i = 0; i < (int) file_info->entry_num; i++) {
        uint8_t *data = (uint8_t *) &entry_vals[entry[i].entry_data_index];

        pt_info.index = entry[i].index;
        pt_info.tbl_inst = entry[i].instance;
        rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, entry[i].sid);

        if (!bcmdrd_pt_index_valid(unit, entry[i].sid,
                                   pt_info.tbl_inst, pt_info.index)) {
            continue;
        }
        if (DI_ENTRY_NEED_MASK(entry[i].flags)) {
            uint8_t *mask;
            SHR_IF_ERR_EXIT
                (bcmptm_mreq_indexed_lt(unit, BCMPTM_REQ_FLAGS_NO_FLAGS,
                                        entry[i].sid, &pt_info, NULL,
                                        NULL, BCMPTM_OP_READ,
                                        NULL,
                                        rsp_entry_wsize, 0,
                                        trans_id, NULL, NULL,
                                        (uint32_t *) &entry_bytes,
                                        &rsp_ltid, &rsp_flags));
            SHR_IF_ERR_EXIT
                (bcmdi_entry_load_from_host(unit, src_bytes, entry_bytes,
                                            entry[i].entry_data_size));
            mask = data + entry[i].entry_data_size;
            for (j = 0; j < entry[i].entry_data_size; j++, data++, mask++) {
                src_bytes[j] = (src_bytes[j] & ~(*mask)) | (*data & *mask);
            }
            data = src_bytes;
        }
        SHR_IF_ERR_EXIT
            (bcmdi_entry_load_to_host(unit, entry_bytes, data,
                                      entry[i].entry_data_size));

        SHR_IF_ERR_EXIT
            (bcmptm_mreq_indexed_lt(unit, 0, entry[i].sid, &pt_info, NULL,
                                    NULL, BCMPTM_OP_WRITE,
                                    (uint32_t *) entry_bytes,
                                    rsp_entry_wsize, 0,
                                    trans_id, NULL, NULL,
                                    NULL, &rsp_ltid, &rsp_flags));

        if (i == (int)(file_info->entry_num - 1))
            break;

        if (++count == FLC_PTM_COMMIT_THRESHOLD &&
            trans_id == BCMPTM_DIRECT_TRANS_ID) {
            /* Commit every FLC_PTM_COMMIT_THRESHOLD times of write */
            count = 0;
            SHR_IF_ERR_EXIT(
                bcmptm_mreq_indexed_lt(unit, BCMPTM_REQ_FLAGS_COMMIT,
                                        0, NULL, NULL,
                                        NULL, BCMPTM_OP_NOP,
                                        NULL, 0, 0,
                                        trans_id, NULL, NULL,
                                        NULL, &rsp_ltid, &rsp_flags));
            /* Use same trans_id for every transaction. */
        }
    }

    /* Fake commit and authentication only needed for SDKLT initialization */
    if (trans_id == BCMPTM_DIRECT_TRANS_ID) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Done last PTM commit. Wait for PTM notify.\n")));
        flc_ptm_commit_notify_flag = FALSE;
        SHR_IF_ERR_EXIT
               (bcmptm_mreq_indexed_lt(unit, BCMPTM_REQ_FLAGS_COMMIT,
                                       0, NULL, NULL,
                                       NULL, BCMPTM_OP_NOP,
                                       NULL, 0, 0,
                                       trans_id, flc_ptm_notify, NULL,
                                       NULL, &rsp_ltid, &rsp_flags));

        flc_auth_load(unit, file_auth);

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Wait PTM notify for FlexCode programming\n")));
        while (flc_ptm_commit_notify_flag != TRUE) {
            sal_usleep(FLC_PTM_NOTIFY_POLL_PERIOD_US);
            
        }

        flc_auth_start(unit, file_auth);
    }

    bcmdi_flexcode_status_update(unit, file_info, !SHR_FUNC_ERR());

exit:
    SHR_FUNC_EXIT();

}

/*******************************************************************************
 * Public Functions
 */
int
bcmdi_flexcode_load(int unit, char *type, uint32_t trans_id, bool warm)
{
    sal_usecs_t proc_start, proc_time;

    SHR_FUNC_ENTER(unit);

    if (warm) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Skip Flexcode load (warm-boot)\n")));
        goto exit;
    }

    proc_start = sal_time_usecs();
    if (flc_dbg_switch[unit].pre_verify) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Check overwrite to RESETVAL...\n")));
        SHR_IF_ERR_EXIT
            (bcmdi_flexcode_hw_verify(unit, type, BCMDI_FLC_VERIFY_MODE_RESETVAL));
    }

    SHR_IF_ERR_EXIT
        (flc_hw_write(unit, type, trans_id));

    if (flc_dbg_switch[unit].post_verify) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Check device data with DATABASE...\n")));
        SHR_IF_ERR_EXIT
            (bcmdi_flexcode_hw_verify(unit, type, BCMDI_FLC_VERIFY_MODE_DATABASE));
    }
    proc_time = SAL_USECS_SUB(sal_time_usecs(), proc_start);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Done Flexcode loading in %d.%06d seconds\n"),
                (int) proc_time / SECOND_USEC,
                (int) proc_time % SECOND_USEC));

exit:
    SHR_FUNC_EXIT();

}

int
bcmdi_flexcode_hw_verify(int unit, char *type, int mode) {

    bcmbd_pt_dyn_info_t pt_info;
    uint32_t rsp_entry_wsize;
    int i, res = 0;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint32_t trans_id = BCMPTM_DIRECT_TRANS_ID;
    const bcmdi_flc_file_info_t *file_info = NULL;
    const bcmdi_flc_file_entry_t *entry = NULL;
    const uint8_t *entry_vals = NULL;
    uint8_t buf1_bytes[DI_MAX_ENTRY_BYTES] = {0};
    uint8_t buf2_bytes[DI_MAX_ENTRY_BYTES] = {0};
    uint8_t *entry_data = NULL;
    uint8_t *entry_mask = NULL;
    uint8_t *dev_data = NULL;
    const bcmdrd_symbols_t *symbols = bcmdrd_dev_symbols_get(unit, 0);
    const bcmdrd_symbol_t *symbol;
    char *mode_str;
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdi_flexcode_file_info_get(unit, type, &file_info));
    SHR_IF_ERR_EXIT
        (bcmdi_flexcode_entry_list_get(unit, type, &entry));
    SHR_IF_ERR_EXIT
        (bcmdi_flexcode_entry_vals_list_get(unit, type, &entry_vals));

    req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(BCMLT_ENT_ATTR_GET_FROM_HW);

    for (i = 0; i < (int) file_info->entry_num; i++) {
        entry_data = (uint8_t *) &entry_vals[entry[i].entry_data_index];
        entry_mask = entry_data + entry[i].entry_data_size;

        /* Skip verify for TCAM memories */
        if (bcmdrd_pt_attr_is_cam(unit, entry[i].sid)) continue;

        pt_info.index = entry[i].index;
        pt_info.tbl_inst = entry[i].instance;
        rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, entry[i].sid);
        SHR_IF_ERR_EXIT
            (bcmptm_mreq_indexed_lt(unit, req_flags,
                                    entry[i].sid, &pt_info, NULL,
                                    NULL, BCMPTM_OP_READ,
                                    NULL,
                                    rsp_entry_wsize, 0,
                                    trans_id, NULL, NULL,
                                    (uint32_t *)&buf1_bytes,
                                    &rsp_ltid, &rsp_flags));

        switch (mode) {
            case BCMDI_FLC_VERIFY_MODE_DATABASE:
                mode_str = "DATABASE";
                /* Need to convert since database is in LE */
                SHR_IF_ERR_EXIT
                    (bcmdi_entry_load_from_host(unit, buf2_bytes,
                                                buf1_bytes,
                                                entry[i].entry_data_size));
                dev_data = buf2_bytes;
                break;

            case BCMDI_FLC_VERIFY_MODE_RESETVAL:
                mode_str = "RESETVAL";
                SHR_NULL_CHECK(symbols, SHR_E_INIT);
                /* Don't convert since resetval is in host endianness */
                symbol = bcmdrd_sym_info_get(symbols, entry[i].sid, NULL);
                if (symbol == NULL) {
                    LOG_WARN(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                    "Symbol for %s (%d) is not defined\n"),
                                    bcmdrd_pt_sid_to_name(unit, entry[i].sid),
                                    entry[i].sid));
                    continue;
                } else if (symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER) {
                    entry_data = (uint8_t *) bcmdrd_sym_reg_resetval_get(symbols, symbol);
                } else if (symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
                    entry_data = buf2_bytes; /* all zeros in this mode */
                }
                dev_data = buf1_bytes;
                break;

            default:
                LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "Verify Mode %d is not defined\n"), mode));
                SHR_EXIT();
        }

        if (DI_ENTRY_NEED_MASK(entry[i].flags)) {
            res = bcmdrd_sym_mem_cmp_mask(dev_data,
                                          entry_data,
                                          entry_mask,
                                          entry[i].entry_data_size);
        } else {
            res = sal_memcmp(dev_data,
                             entry_data,
                             entry[i].entry_data_size);
        }

        if (res != 0) {
            LOG_WARN(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s Mismatch on %s [%d]\n"),
                         mode_str,
                         bcmdrd_pt_sid_to_name(unit, entry[i].sid),
                         entry[i].index));
        }
    }

    SHR_IF_ERR_EXIT
        (bcmptm_mreq_indexed_lt(unit, BCMPTM_REQ_FLAGS_COMMIT, 0, NULL, NULL,
                                NULL, BCMPTM_OP_NOP, NULL, 0, 0, trans_id,
                                NULL, NULL, NULL, NULL, &rsp_flags));
exit:
    SHR_FUNC_EXIT();

}

int
bcmdi_flexcode_debug_switch_set(int unit, bcmdi_flc_debug_switch_t dbg_switch)
{
    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    flc_dbg_switch[unit] = dbg_switch;

exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_flexcode_debug_switch_get(int unit, bcmdi_flc_debug_switch_t *dbg_switch)
{
    SHR_FUNC_ENTER(unit);

    if (BCMDRD_UNIT_VALID(unit)) {
        *dbg_switch = flc_dbg_switch[unit];
        SHR_EXIT();
    }

    SHR_ERR_EXIT(SHR_E_UNIT);

exit:
    SHR_FUNC_EXIT();
}

