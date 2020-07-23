/*! \file bcmdi_devicecode.c
 *
 * BCMDI DeviceCode Loader
 *
 * This module contains the implementation of DeviceCode loader functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_time.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmdi/bcmdi.h>
#include <bcmdi/bcmdi_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMDI_DEVICECODE

/*!
 *
 * \brief Load DeviceCode to device using PTM modelled path.
 *
 * Commit transcation when a predefined number of memory writes
 * is reached and increase transcation ID.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors
 */
static int
dvc_hw_write(int unit)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t rsp_entry_wsize;
    int i, j;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    const bcmdi_dvc_file_info_t *file_info = NULL;
    const bcmdi_dvc_file_entry_t *entry = NULL;
    const uint8_t *entry_vals = NULL;
    uint8_t src_bytes[DI_MAX_ENTRY_BYTES] = {0};
    uint8_t entry_bytes[DI_MAX_ENTRY_BYTES] = {0};
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdi_devicecode_file_info_get(unit, &file_info));
    SHR_IF_ERR_EXIT
        (bcmdi_devicecode_entry_list_get(unit, &entry));
    SHR_IF_ERR_EXIT
        (bcmdi_devicecode_entry_vals_list_get(unit, &entry_vals));

    if (file_info == NULL) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "No DeviceCode available for this device.\n")));
        SHR_EXIT();
    }

    req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(BCMLT_ENT_ATTR_GET_FROM_HW);

    /* Only use ireq API for initial implementation */
    for (i = 0; i < (int) file_info->entry_num; i++) {
        uint8_t *data = (uint8_t *) &entry_vals[entry[i].entry_data_index];

        if (DI_ENTRY_CHK_VALID_IDX(entry[i].flags) &&
            (!bcmdrd_pt_is_valid(unit, entry[i].sid) ||
             !bcmdrd_pt_index_valid(unit, entry[i].sid, entry[i].instance,
                                    entry[i].index))) {
            LOG_DEBUG(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Skip DeviceCode (sid %d/ inst %d/ idx %d)\n"),
                                 entry[i].sid,
                                 entry[i].instance,
                                 entry[i].index));
            /* Skip DeviceCode loading for invalid reg/mem instance or index */
            continue;
        }

        pt_info.index = entry[i].index;
        pt_info.tbl_inst = entry[i].instance;
        rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, entry[i].sid);

        if (DI_ENTRY_NEED_MASK(entry[i].flags)) {
            uint8_t *mask;
            SHR_IF_ERR_EXIT
                (bcmptm_ireq_read(unit,
                                  req_flags,
                                  entry[i].sid,
                                  &pt_info,
                                  NULL,
                                  rsp_entry_wsize,
                                  0,
                                  (uint32_t *) entry_bytes,
                                  &rsp_ltid,
                                  &rsp_flags));

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
            (bcmptm_ireq_write(unit,
                               BCMPTM_REQ_FLAGS_NO_FLAGS,
                               entry[i].sid,
                               &pt_info,
                               NULL,
                               (uint32_t *) entry_bytes,
                               0,
                               &rsp_ltid,
                               &rsp_flags));

        if (i == (int)(file_info->entry_num - 1))
            break;
    }

    bcmdi_devicecode_status_update(unit, file_info, !SHR_FUNC_ERR());

exit:
    SHR_FUNC_EXIT();

}

/*******************************************************************************
 * Public Functions
 */
int
bcmdi_devicecode_load(int unit, bool warm)
{
    sal_usecs_t proc_start, proc_time;

    SHR_FUNC_ENTER(unit);

    if (warm) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Skip DeviceCode load (warm-boot)\n")));
        goto exit;
    }

    proc_start = sal_time_usecs();
    SHR_IF_ERR_EXIT
        (dvc_hw_write(unit));
    proc_time = SAL_USECS_SUB(sal_time_usecs(), proc_start);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Done DeviceCode loading in %d.%06d seconds\n"),
                (int) proc_time / SECOND_USEC,
                (int) proc_time % SECOND_USEC));

exit:
    SHR_FUNC_EXIT();

}

