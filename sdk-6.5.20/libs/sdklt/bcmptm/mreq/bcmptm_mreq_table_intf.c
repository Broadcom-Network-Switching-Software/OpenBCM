/*! \file bcmptm_mreq_table_intf.c
 *
 * This file contains interfaces for the table information handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>

#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm_table_intf.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MREQ

/* Maximum number of PT key or data entries for a single PT operation */
#define PT_ENTRY_LIST_MAX  4

/* Size (in words) of one PT entry for BCMPTM_OP_GET_TABLE_INFO op */
#define PT_ENTRY_WSIZE     1

/*
 * Key and Data entry buffers for PTM table info operation.
 *
 * These buffers are used to set the key and data entries in
 * the request and response arguments for the BCMPTM_OP_GET_TABLE_INFO op.
 *
 * There is no information provided or returned
 * in the key and data entries of the request and response
 * arguments for the BCMPTM_OP_GET_TABLE_INFO op.
 * However, since the lower level PTM routines
 * may copy information from the request/response key and data buffers
 * for any PTM op, we need to pass different memory buffers.
 *
 * It is sufficient to set the buffer size to 1 word for
 * the key and data entries.
 */
static uint32_t table_info_req_entry_key[PT_ENTRY_LIST_MAX][PT_ENTRY_WSIZE];
static uint32_t table_info_req_entry_data[PT_ENTRY_LIST_MAX][PT_ENTRY_WSIZE];
static uint32_t table_info_rsp_entry_key[PT_ENTRY_LIST_MAX][PT_ENTRY_WSIZE];
static uint32_t table_info_rsp_entry_data[PT_ENTRY_LIST_MAX][PT_ENTRY_WSIZE];

static uint32_t *table_info_req_entry_key_list[PT_ENTRY_LIST_MAX] = {
    table_info_req_entry_key[0],
    table_info_req_entry_key[1],
    table_info_req_entry_key[2],
    table_info_req_entry_key[3]
};

static uint32_t *table_info_req_entry_data_list[PT_ENTRY_LIST_MAX] = {
    table_info_req_entry_data[0],
    table_info_req_entry_data[1],
    table_info_req_entry_data[2],
    table_info_req_entry_data[3]
};

static uint32_t *table_info_rsp_entry_key_list[PT_ENTRY_LIST_MAX] = {
    table_info_rsp_entry_key[0],
    table_info_rsp_entry_key[1],
    table_info_rsp_entry_key[2],
    table_info_rsp_entry_key[3]
};

static uint32_t *table_info_rsp_entry_data_list[PT_ENTRY_LIST_MAX] = {
    table_info_rsp_entry_data[0],
    table_info_rsp_entry_data[1],
    table_info_rsp_entry_data[2],
    table_info_rsp_entry_data[3]
};

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Initialize a PT keyed request information structure.
 *
 * Initialize a PT keyed request information structure for the
 * PTM BCMPTM_OP_GET_TABLE_INFO op.
 *
 * \param [in,out] mreq_info PT keyed request information.
 */
static void
table_info_keyed_mreq_info_init(bcmptm_keyed_lt_mreq_info_t *mreq_info)
{
    sal_memset(mreq_info, 0, sizeof(*mreq_info));

    /* Use dummy buffers to set key and data entries */
    mreq_info->entry_key_words = table_info_req_entry_key_list;
    mreq_info->entry_data_words = table_info_req_entry_data_list;
    mreq_info->pdd_l_dtyp = NULL;
    mreq_info->entry_id = 0;
    mreq_info->entry_pri = 0;
    mreq_info->same_key = FALSE;
    mreq_info->rsp_entry_key_buf_wsize = PT_ENTRY_WSIZE;
    mreq_info->rsp_entry_data_buf_wsize = PT_ENTRY_WSIZE;
    mreq_info->entry_attrs = NULL;
}

/*!
 * \brief Initialize a PT keyed response information structure.
 *
 * Initialize a PT keyed response information structure for the
 * PTM BCMPTM_OP_GET_TABLE_INFO op.
 *
 * \param [in,out] mrsp_info PT keyed response information.
 * \param [in] table_info Structure returning table information.
 */
static void
table_info_keyed_mrsp_info_init(bcmptm_keyed_lt_mrsp_info_t *mrsp_info,
                                bcmptm_table_info_t *table_info)
{
    sal_memset(mrsp_info, 0, sizeof(*mrsp_info));
    if (table_info != NULL) {
        sal_memset(table_info, 0, sizeof(*table_info));
    }

    /* Use dummy buffers to set key and data entries */
    mrsp_info->rsp_entry_key_words = table_info_rsp_entry_key_list;
    mrsp_info->rsp_entry_data_words = table_info_rsp_entry_data_list;
    mrsp_info->rsp_pdd_l_dtyp = NULL;
    mrsp_info->rsp_entry_pipe = 0;
    mrsp_info->rsp_entry_pri = 0;
    mrsp_info->pt_op_info = NULL;
    mrsp_info->rsp_misc = (void *)table_info;
}

/*******************************************************************************
 * Public functions
 */

int
bcmptm_mreq_keyed_table_entry_limit_get(int unit,
                        uint32_t trans_id,
                        bcmltd_sid_t sid,
                        const bcmltd_table_entry_limit_arg_t *table_arg,
                        bcmltd_table_entry_limit_t *table_data,
                        const bcmltd_generic_arg_t *arg)
{
    uint64_t req_flags;
    uint32_t rsp_flags;
    bcmltd_sid_t rsp_ltid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmptm_keyed_lt_mreq_info_t mreq_info;
    bcmptm_keyed_lt_mrsp_info_t mrsp_info;
    bcmptm_table_info_t table_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(table_data, 0, sizeof(*table_data));

    /*
     * Initialize arguments.
     *
     * Some parameters are not relevant for this PTM opcode,
     * but they to be set to a default value to avoid triggering
     * error check condition in the calling lower level routines.
     */

    req_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = -1;

    /* Set up request info */
    table_info_keyed_mreq_info_init(&mreq_info);

    /* Set up response info */
    table_info_keyed_mrsp_info_init(&mrsp_info, &table_info);
    table_info.entry_limit = table_arg->entry_maximum; /* LTM default */

    /* Get table information */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_mreq_keyed_lt(unit,
                              req_flags,
                              sid,
                              &pt_dyn_info,
                              NULL,
                              BCMPTM_OP_GET_TABLE_INFO,
                              &mreq_info,
                              trans_id,
                              &mrsp_info,
                              &rsp_ltid,
                              &rsp_flags));

    table_data->entry_limit = table_info.entry_limit;

exit:
    SHR_FUNC_EXIT();
}
