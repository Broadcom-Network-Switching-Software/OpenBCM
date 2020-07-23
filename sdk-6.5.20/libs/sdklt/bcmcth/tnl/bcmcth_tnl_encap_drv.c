/*! \file bcmcth_tnl_encap_drv.c
 *
 * BCMCTH Tunnel Driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmptm/bcmptm.h>
#include <bcmbd/bcmbd.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_tnl_internal.h>
#include <bcmcth/bcmcth_tnl_drv.h>
#include <bcmissu/issu_api.h>
#include <bcmcth/generated/tnl_ha.h>

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCMCTH_TNL

/*! Device specific attach function type for tunnel encap. */
typedef bcmcth_tnl_encap_drv_t *(*bcmcth_tnl_encap_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_cth_tnl_encap_drv_get },
static struct {
    bcmcth_tnl_encap_drv_get_f drv_get;
} tnl_encap_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/*! TNL fields array handler. */
static shr_famm_hdl_t   tnl_fld_array_hdl[BCMDRD_CONFIG_MAX_UNITS];

static bcmcth_tnl_encap_drv_t   *tnl_encap_drv[BCMDRD_CONFIG_MAX_UNITS];
bcmcth_tnl_encap_sw_state_t bcmcth_tnl_encap_sw_state[BCMDRD_CONFIG_MAX_UNITS];
bcmcth_tnl_encap_sw_state_t bcmcth_tnl_encap_sw_state_ha[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
/*
 * Retrieve a field from the input field list.
 */
static int
tnl_entry_field_get(int unit,
                    const bcmltd_field_t *list,
                    uint32_t fid,
                    uint32_t *data)
{
    bool  found = false;
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    field = list;
    while (field) {
        if (field->id == fid) {
            *data = field->data;
            found = true;
            break;
        }
        field = field->next;
    }

    if (found == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Convert an LT field into a field in the MPLS entry structure.
 */
static int
tnl_mpls_lt_fid_to_mpls_entry_data(int unit,
                                   uint32_t fid,
                                   uint32_t idx,
                                   uint64_t data,
                                   tnl_mpls_encap_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);

    switch (fid) {
    case TNL_MPLS_ENCAPt_TNL_MPLS_ENCAP_IDf:
        entry->sw_encap_id = data;
        break;
    case TNL_MPLS_ENCAPt_ENCAP_INDEXf:
        entry->encap_id = data;
        break;
    case TNL_MPLS_ENCAPt_MAX_LABELSf:
        entry->max_labels = data;
        break;
    case TNL_MPLS_ENCAPt_NUM_LABELSf:
        entry->num_labels = data;
        break;
    case TNL_MPLS_ENCAPt_LABELf:
        entry->label[idx] = data;
        break;
    case TNL_MPLS_ENCAPt_LABEL_TTLf:
        entry->ttl[idx] = data;
        break;
    case TNL_MPLS_ENCAPt_MPLS_TNL_EXP_MODEf:
        entry->exp_mode[idx] = data;
        break;
    case TNL_MPLS_ENCAPt_PHB_EGR_MPLS_IDf:
        entry->phb_id[idx] = data;
        break;
    case TNL_MPLS_ENCAPt_LABEL_EXPf:
        entry->exp[idx] = data;
        break;
    case TNL_MPLS_ENCAPt_PRIf:
        entry->pri[idx] = data;
        break;
    case TNL_MPLS_ENCAPt_CFIf:
        entry->cfi[idx] = data;
        break;
    case TNL_MPLS_ENCAPt_ECN_CNG_TO_MPLS_EXP_IDf:
        entry->cng_to_exp[idx] = data;
        break;
    case TNL_MPLS_ENCAPt_ECN_CNG_TO_MPLS_EXP_PRIORITYf:
        entry->cng_to_exp_pri[idx] = data;
        break;
    case TNL_MPLS_ENCAPt_ECN_IP_TO_MPLS_EXP_IDf:
        entry->ecn_to_exp[idx] = data;
        break;
    case TNL_MPLS_ENCAPt_ECN_IP_TO_MPLS_EXP_PRIORITYf:
        entry->ecn_to_exp_pri[idx] = data;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
        break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Populate the MPLS entry structure as follows.
 * - initialize with default values.
 * - update with IMM fields from IMM DB in case the entry is not new.
 * - update based on current user input.
 */
static int
tnl_mpls_encap_entry_parse(int unit,
                           bool new,
                           bcmltd_fields_t *imm_data,
                           const bcmltd_field_t *list,
                           tnl_mpls_encap_entry_t *entry)
{
    size_t          num_fid = 0, count = 0;
    uint32_t        i = 0, fid = 0, idx = 0;
    uint32_t        table_sz = 0;
    uint64_t        data = 0;
    bcmlrd_fid_t   *fid_list = NULL;
    bcmlrd_field_def_t field_def;
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, TNL_MPLS_ENCAPt, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "bcmcthtnlMplsEncapFldList");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    /* Get the list of valid fids corresponding to TNL_MPLS_ENCAP for this unit. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_list_get(unit,
                               TNL_MPLS_ENCAPt,
                               num_fid,
                               fid_list,
                               &count));

    /* Initialize the structure with default values. */
    for (i = 0; i < count; i++) {
        fid = fid_list[i];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        TNL_MPLS_ENCAPt,
                                        fid,
                                        &field_def));
        data = BCMCTH_TNL_FIELD_DEF(field_def);
        for (idx = 0; idx < BCMCTH_TNL_MPLS_MAX_LABELS; idx++) {
            SHR_IF_ERR_VERBOSE_EXIT(
                tnl_mpls_lt_fid_to_mpls_entry_data(unit, fid, idx, data, entry));
        }
    }

    /* Update with IMM fields if entry is not new (UPDATE case). */
    if (!new && imm_data && imm_data->count != 0) {
        for (i = 0; i < imm_data->count; i++) {
            fid = imm_data->field[i]->id;
            data = imm_data->field[i]->data;
            idx = imm_data->field[i]->idx;

            SHR_IF_ERR_VERBOSE_EXIT(
                tnl_mpls_lt_fid_to_mpls_entry_data(unit, fid, idx, data, entry));
        }
    }

    /* Update with current user input. */
    field = list;
    while (field) {
        fid = field->id;
        data = field->data;
        idx = field->idx;

        SHR_IF_ERR_VERBOSE_EXIT(
            tnl_mpls_lt_fid_to_mpls_entry_data(unit, fid, idx, data, entry));
        field = field->next;
    }

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*
 * Get value of a field from IMM fields list.
 */
static int
tnl_imm_entry_field_get(int unit,
                        bcmltd_fields_t *list,
                        uint32_t fid,
                        uint32_t *data)
{
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);
    for (i = 0; i < list->count; i++) {
        if (list->field[i]->id == fid) {
            *data = list->field[i]->data;
            break;
        }
    }

    if (i == list->count) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to check if the current hardware index can be re-used
 * for an MPLS entry that is being updated.
 */
static int
tnl_mpls_validate_curr_index(int unit,
                             tnl_mpls_encap_entry_t *entry,
                             int old_max_labels,
                             uint32_t mpls_tnl_id,
                             bool *move)
{
    int         count = 0;
    uint32_t    sub_id = 0, max_label_per_tnl = 0;

    SHR_FUNC_ENTER(unit);

    max_label_per_tnl = BCMCTH_TNL_MAX_MPLS_LABELS(unit);
    sub_id = (mpls_tnl_id % max_label_per_tnl);

    /*
     * If the current hardware index cannot hold
     * the new max labels specified, entry has to be moved.
     */
    if ((sub_id + entry->max_labels) > max_label_per_tnl) {
        *move = true;
        SHR_EXIT();
    }

    /*
     * If the current hardware index is not valid w.r.t
     * the number of MPLS labels, entry has to be moved.
     */
    switch (entry->max_labels) {
    case 1:
    case 2:
        break;
    case 3:
    case 4:
        /* Bits 0 and 1 should be 0 for a chain of 3 to 4 labels. */
        if (sub_id & 0x3) {
            *move = true;
            SHR_EXIT();
        }
        break;
    case 5:
    case 6:
    case 7:
    case 8:
        /* Bits 0, 1 and 2 should be 0 for a chain of 5 to 8 labels. */
        if (sub_id & 0x7) {
            *move = true;
            SHR_EXIT();
        }
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Count the number of entries used from the current index.
     * If this value is more than the old max_labels value, then the following
     * entries are being used by another tunnel. Entry has to be moved.
     */
    SHR_BITCOUNT_RANGE(
        BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
        count,
        mpls_tnl_id,
        entry->max_labels);

    if (count > old_max_labels) {
        *move = true;
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a free index for MPLS tunnel
 *
 * Allocate an entry from the base tunnel and return the
 * index as MPLS tunnel ID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  max_labels      Maximum number of labels that
 *                              this MPLS tunnel can have.
 * \param [out] mpls_tnl_id     Return value of allocated MPLS tunnel ID.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_FULL       Tunnel hardware table full.
 */
static int
tnl_mpls_encap_alloc_free_index(int unit,
                                int max_labels,
                                uint32_t *mpls_tnl_id)
{
    int         num_tnl = 0, count = 0;
    int         max_label_per_tnl = 0;
    int         start_idx, width, next_idx = 0;
    int         idx = 0, sub_idx = 0, base_idx = 0;
    bool        found = false;

    SHR_FUNC_ENTER(unit);

    width = BCMCTH_TNL_MPLS_WIDTH(unit);
    num_tnl = BCMCTH_TNL_NUM_IPV4(unit);
    start_idx = BCMCTH_TNL_MPLS_START_IDX(unit);
    max_label_per_tnl = BCMCTH_TNL_MAX_MPLS_LABELS(unit);

    /* Get an unused base tnl index from the base tnl bitmap. */
    for (idx = start_idx; idx < num_tnl; (idx = idx + width)) {
        SHR_BITCOUNT_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap,
                           count,
                           idx,
                           width);
        if (count == 0) {
            /* Tunnel entry is free, use for MPLS. */
            *mpls_tnl_id = (((uint64_t)idx * max_label_per_tnl) / width);
            break;
        } else {
            /*
             * If this index is used, check if it is an MPLS tunnel and if there
             * are free entries.
             */
            if (BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type[idx] ==
                TNL_TYPE_MPLS) {
                /*
                 * Mpls tunnel, check if the sub entry is free and can hold
                 * max labels.
                 */
                base_idx = ((idx * max_label_per_tnl) / width);
                SHR_BITCOUNT_RANGE(
                    BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
                    count,
                    base_idx,
                    max_label_per_tnl);
                if (count < max_label_per_tnl) {
                    /*
                     * There is some space here.
                     * Start from the first offset to look for free space.
                     */
                    for (sub_idx = 0; sub_idx < max_label_per_tnl; sub_idx++) {
                        next_idx = (base_idx + sub_idx);
                        SHR_BITCOUNT_RANGE(
                            BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
                            count,
                            next_idx,
                            max_labels);
                        if ((count != 0) ||
                            ((sub_idx + max_labels) > max_label_per_tnl)) {
                            /* Not enough space or goes beyond one entry. */
                            continue;
                        } else {
                            if (max_labels > 2) {
                                /*
                                 * To represent more than two labels, last two
                                 * bits must be free. 3 and 4 label entries can
                                 * start at offsets 0 or 4.
                                 */
                                if (sub_idx & 0x3) {
                                    continue;
                                }

                                if (max_labels > 7) {
                                    /*
                                     * To accommodate more than four labels, last three
                                     * bits must be free. 5, 6, 7 and 8 label entries can
                                     * start only at offset 0.
                                     */
                                    if (sub_idx & 0x7) {
                                        continue;
                                    }
                                }
                            }
                            /* Found a suitable index. */
                            found = true;
                            *mpls_tnl_id = next_idx;
                            break;
                        }
                    }
                }
            }
        }
        /* Break away from the loop if index is found. */
        if (found == true) {
            break;
        }
    }

    if (idx == num_tnl) {
        SHR_ERR_EXIT(SHR_E_FULL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a free index for MPLS tunnel.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  max_labels      Maximum number of labels that
 *                              this MPLS tunnel can have.
 * \param [out] mpls_tnl_id     MPLS tunnel ID allocated by this function.
 * \param [out] tnl_id          Base tunnel ID corresponding to mpls_tnl_id.
 * \param [out] sub_id          Offset within the base tunnel for mpls_tnl_id.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_FULL       Tunnel hardware table full.
 */
static int
tnl_mpls_encap_get_free_index(int unit,
                              int max_labels,
                              uint32_t *mpls_tnl_id,
                              uint32_t *tnl_id,
                              uint32_t *sub_id)
{
    int         max_label_per_tnl = 1;

    SHR_FUNC_ENTER(unit);

    max_label_per_tnl = BCMCTH_TNL_MAX_MPLS_LABELS(unit);

    /*
     * Get a free index from scanning the used bitmap.
     */
    SHR_IF_ERR_VERBOSE_EXIT(
        tnl_mpls_encap_alloc_free_index(unit,
                                        max_labels,
                                        mpls_tnl_id));
    if (*mpls_tnl_id == 0) {
        SHR_ERR_EXIT(SHR_E_FULL);
    }

    *tnl_id = (*mpls_tnl_id / max_label_per_tnl);
    *sub_id = (*mpls_tnl_id % max_label_per_tnl);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a free index for IP tunnel.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [iout  tnl_id          Allocate tnl_id.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_FULL       Tunnel hardware table full.
 */
static int
tnl_ip_encap_get_free_index(int unit,
                            bcmltd_sid_t sid,
                            uint32_t *tnl_id)
{
    int         num_tnl = 0, count = 0;
    int         start_idx, width;
    int         idx = 0;

    SHR_FUNC_ENTER(unit);

    num_tnl = BCMCTH_TNL_NUM_IPV4(unit);
    switch (sid) {
    case TNL_IPV4_ENCAPt:
        width = BCMCTH_TNL_IPV4_WIDTH(unit);
        start_idx = BCMCTH_TNL_IPV4_START_IDX(unit);
        break;
    case TNL_IPV6_ENCAPt:
        width = BCMCTH_TNL_IPV6_WIDTH(unit);
        start_idx = BCMCTH_TNL_IPV6_START_IDX(unit);
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Get a free index.
     * Single-wide for IPv4 and double-wide for IPv6.
     */
    for (idx = start_idx; idx < num_tnl; (idx = idx + width)) {
        SHR_BITCOUNT_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap,
                           count,
                           idx,
                           width);
        if (count == 0) {
            *tnl_id = (idx / width);
            break;
        }
    }

    if (idx == num_tnl) {
        SHR_ERR_EXIT(SHR_E_FULL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Read an entry from Tunnel hardware table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          The operation arguments.
 * \param [in]  req_ltid        Logical table ID.
 * \param [in]  hw_sid          Hardware table ID.
 * \param [in]  tnl_id          Tunnel table hardware index to read from.
 * \param [out] entry_buf       Pointer to read buffer.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_NOT_FOUND  Hardware table not found.
 * \retval SHR_E_PARAM      Index is wrong..
 */
static int
tnl_encap_hw_entry_read(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t req_ltid,
                        bcmdrd_sid_t hw_sid,
                        uint32_t tnl_id,
                        uint8_t *entry_buf)
{
    uint32_t    rsp_flags;
    bcmltd_sid_t        rsp_ltid;
    bcmdrd_sym_info_t   sinfo;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);

    pt_dyn_info.index = tnl_id;
    pt_dyn_info.tbl_inst = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_info_get(unit, hw_sid, &sinfo));
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    hw_sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL,
                                    BCMPTM_OP_READ,
                                    0,
                                    sinfo.entry_wsize,
                                    req_ltid,
                                    op_arg->trans_id,
                                    NULL,
                                    NULL,
                                    (uint32_t *)entry_buf,
                                    &rsp_ltid,
                                    &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write an entry into Tunnel hardware table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          The operation arguments.
 * \param [in]  req_ltid        Logical table ID.
 * \param [in]  hw_sid          Hardware table ID.
 * \param [in]  tnl_id          Tunnel table hardware index to read from.
 * \param [in]  entry_buf       Pointer to write buffer.
 *
 * \retval SHR_E_NONE       OK.
 * \retval SHR_E_NOT_FOUND  Hardware table not found.
 * \retval SHR_E_PARAM      Index is wrong..
 */
static int
tnl_encap_hw_entry_write(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t req_ltid,
                         bcmdrd_sid_t hw_sid,
                         uint32_t tnl_id,
                         uint8_t *entry_buf)
{
    uint32_t    rsp_flags;
    bcmltd_sid_t        rsp_ltid;
    bcmdrd_sym_info_t   sinfo;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);

    pt_dyn_info.index = tnl_id;
    pt_dyn_info.tbl_inst = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_info_get(unit, hw_sid, &sinfo));
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    hw_sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL,
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)entry_buf,
                                    0,
                                    req_ltid,
                                    op_arg->trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear MPLS tunnel entry.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  hw_sid          Hardware table ID.
 * \param [in]  mpls_tnl_id     MPLS tunnel ID.
 * \param [in]  entry_buf       Pointer to tunnel entry buffer.
 *
 * \retval SHR_E_NONE       OK.
 */
static int
tnl_mpls_encap_hw_entry_clear(int unit,
                              uint32_t hw_sid,
                              uint64_t mpls_tnl_id,
                              int max_labels,
                              uint8_t *entry_buf)
{
    uint32_t    base_idx = 0, sub_id = 0;
    int         max_label_per_tnl = 0, count = 0;
    bool        entry_remove = false;

    SHR_FUNC_ENTER(unit);

    max_label_per_tnl = BCMCTH_TNL_MAX_MPLS_LABELS(unit);
    sub_id = (mpls_tnl_id % max_label_per_tnl);

    /* Get the offset into the first entry of this tunnel. */
    base_idx = ((mpls_tnl_id / max_label_per_tnl) * max_label_per_tnl);

    /*
     * Count the number of bits in this MPLS tunnel hardware entry.
     */
    SHR_BITCOUNT_RANGE(
        BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
        count,
        base_idx,
        max_label_per_tnl);

    /*
     * Last MPLS entry being removed from this tunnel entry,
     * Clear tunnel type.
     */
    if (count <= max_labels) {
        entry_remove = true;
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        BCMCTH_TNL_MPLS_HW_ENTRY_CLEAR(unit)(unit,
                                             hw_sid,
                                             sub_id,
                                             max_labels,
                                             entry_remove,
                                             entry_buf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Prepare the Tunnel hardware entry from LT fields.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  hw_sid          Hardware table ID.
 * \param [in]  tnl_type        Tunnel entry type.
 * \param [in]  sub_id          Offset within the base tunnel.
 * \param [in]  entry           MPLS entry s/w state.
 * \param [in]  in              Input parameter list.
 * \param [out] entry_buf       Pointer to populated hardware entry buffer.
 *
 * \retval SHR_E_NONE       OK.
 */
static int
tnl_encap_hw_entry_prepare(int unit,
                           uint32_t hw_sid,
                           int tnl_type,
                           uint32_t sub_id,
                           tnl_mpls_encap_entry_t *entry,
                           const bcmltd_field_t *data,
                           uint8_t *entry_buf)
{
    SHR_FUNC_ENTER(unit);

    /* Convert LT entry to HW entry. */
    switch (tnl_type) {
    case TNL_TYPE_IPV4:
        SHR_IF_ERR_VERBOSE_EXIT(BCMCTH_TNL_IPV4_LT_TO_HW_ENTRY(unit)(unit,
                                                                     hw_sid,
                                                                     data,
                                                                     entry_buf));
        break;
    case TNL_TYPE_IPV6:
        SHR_IF_ERR_VERBOSE_EXIT(BCMCTH_TNL_IPV6_LT_TO_HW_ENTRY(unit)(unit,
                                                                     hw_sid,
                                                                     data,
                                                                     entry_buf));
        break;
    case TNL_TYPE_MPLS:
        SHR_IF_ERR_VERBOSE_EXIT(BCMCTH_TNL_MPLS_LT_TO_HW_ENTRY(unit)(unit,
                                                                     hw_sid,
                                                                     sub_id,
                                                                     entry,
                                                                     entry_buf));
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Prepare the Tunnel hardware entry from LT fields.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  hw_sid          Hardware table ID.
 * \param [in]  tnl_type        Tunnel entry type.
 * \param [in]  sub_id          Offset within the base tunnel.
 * \param [in]  entry           MPLS entry s/w state.
 * \param [in]  in              Input parameter list.
 * \param [out] entry_buf       Pointer to populated hardware entry buffer.
 *
 * \retval SHR_E_NONE       OK.
 */
static int
tnl_encap_hw_entry_parse(int unit,
                         uint32_t hw_sid,
                         int tnl_type,
                         uint32_t sub_id,
                         tnl_mpls_encap_entry_t *entry,
                         uint8_t *entry_buf,
                         bcmltd_fields_t *data)
{
    SHR_FUNC_ENTER(unit);

    /* Convert HW entry to LT fields. */
    switch (tnl_type) {
    case TNL_TYPE_IPV4:
        SHR_IF_ERR_VERBOSE_EXIT(BCMCTH_TNL_IPV4_HW_TO_LT_ENTRY(unit)(unit,
                                                                     hw_sid,
                                                                     entry_buf,
                                                                     data));
        break;
    case TNL_TYPE_IPV6:
        SHR_IF_ERR_VERBOSE_EXIT(BCMCTH_TNL_IPV6_HW_TO_LT_ENTRY(unit)(unit,
                                                                     hw_sid,
                                                                     entry_buf,
                                                                     data));
        break;
    case TNL_TYPE_MPLS:
        SHR_IF_ERR_VERBOSE_EXIT(BCMCTH_TNL_MPLS_HW_TO_LT_ENTRY(unit)(unit,
                                                                     hw_sid,
                                                                     sub_id,
                                                                     entry,
                                                                     entry_buf,
                                                                     data));
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
  Public functions
 */
int
bcmcth_tnl_encap_entry_insert(int unit,
                              bcmltd_sid_t sid,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              bcmltd_fields_t *out)
{
    int         idx = 0;
    uint8_t     tnl_type = 0;
    uint32_t    hw_sid = 0, ret_fid = 0;
    uint32_t    tnl_id  = 0, mpls_tnl_id = 0;
    uint8_t     *entry_buf = NULL;
    uint32_t    entry_sz = 0;
    uint32_t    sub_id = 0;
    tnl_mpls_encap_entry_t   entry = {0};

    SHR_FUNC_ENTER(unit);

    switch (sid) {
    case TNL_IPV4_ENCAPt:
        ret_fid = TNL_IPV4_ENCAPt_ENCAP_INDEXf;
        tnl_type = TNL_TYPE_IPV4;
        hw_sid = BCMCTH_TNL_IPV4_HW_SID(unit);
        break;
    case TNL_IPV6_ENCAPt:
        ret_fid = TNL_IPV6_ENCAPt_ENCAP_INDEXf;
        tnl_type = TNL_TYPE_IPV6;
        hw_sid = BCMCTH_TNL_IPV6_HW_SID(unit);
        break;
    case TNL_MPLS_ENCAPt:
        ret_fid = TNL_MPLS_ENCAPt_ENCAP_INDEXf;
        tnl_type = TNL_TYPE_MPLS;
        hw_sid = BCMCTH_TNL_MPLS_HW_SID(unit);
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((sid == TNL_IPV4_ENCAPt) || (sid == TNL_IPV6_ENCAPt)) {
        /*
         * Get a free index.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (tnl_ip_encap_get_free_index(unit,
                                         sid,
                                         &tnl_id));
    } else {
        sal_memset(&entry, 0, sizeof(entry));

        /* Parse and populate mpls_entry. */
        SHR_IF_ERR_VERBOSE_EXIT(
            tnl_mpls_encap_entry_parse(unit,
                                       true,
                                       NULL,
                                       data,
                                       &entry));

        /* NUM_LABELS should never be greater than MAX_LABELS. */
        if (entry.num_labels > entry.max_labels) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /*
         * Get a free index.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (tnl_mpls_encap_get_free_index(unit,
                                           entry.max_labels,
                                           &mpls_tnl_id,
                                           &tnl_id,
                                           &sub_id));
    }

    /* Read the entry from hardware. */
    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, hw_sid));
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthtnlEncapHwEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_encap_hw_entry_read(unit,
                                 op_arg,
                                 sid,
                                 hw_sid,
                                 tnl_id,
                                 entry_buf));

    /* Prepare the hw entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_encap_hw_entry_prepare(unit,
                                    hw_sid,
                                    tnl_type,
                                    sub_id,
                                    &entry,
                                    data,
                                    entry_buf));

    /* Write back the entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_encap_hw_entry_write(unit,
                                  op_arg,
                                  sid,
                                  hw_sid,
                                  tnl_id,
                                  entry_buf));

    /* Mark the used tnl id in the bitmap and set tnl type. */
    switch (sid) {
    case TNL_IPV4_ENCAPt:
        SHR_BITSET(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap, tnl_id);
        BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type[tnl_id] = TNL_TYPE_IPV4;
        break;
    case TNL_IPV6_ENCAPt:
        SHR_BITSET_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap,
                         (tnl_id * BCMCTH_TNL_IPV6_WIDTH(unit)),
                         BCMCTH_TNL_IPV6_WIDTH(unit));
        /* Save the tunnel type of this base tunnel. */
        for (idx = 0; idx < BCMCTH_TNL_IPV6_WIDTH(unit); idx++) {
            BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type
                [((tnl_id * BCMCTH_TNL_IPV6_WIDTH(unit)) + idx)] = TNL_TYPE_IPV6;
        }
        break;
    case TNL_MPLS_ENCAPt:
        /* Mark the base tnl index as used in base tnl bitmap. */
        SHR_BITSET_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap,
                         (tnl_id * BCMCTH_TNL_MPLS_WIDTH(unit)),
                         BCMCTH_TNL_MPLS_WIDTH(unit));
        /* Mark the mpls index as used in the mpls tnl bitmap. */
        SHR_BITSET_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
                         mpls_tnl_id, entry.max_labels);

        /* Save the tunnel type of this base tunnel. */
        for (idx = 0; idx < BCMCTH_TNL_MPLS_WIDTH(unit); idx++) {
            BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type
                [((tnl_id * BCMCTH_TNL_MPLS_WIDTH(unit)) + idx)] = TNL_TYPE_MPLS;
        }
        break;
    }

    /* Return the allocated hardware index in the RO field, ENCAP_ID. */
    if (out) {
        out->field[0]->id = ret_fid;
        out->field[0]->idx = 0;
        if (sid == TNL_MPLS_ENCAPt) {
            out->field[0]->data = mpls_tnl_id;
        } else {
            out->field[0]->data = tnl_id;
        }
        out->count = 1;
    }

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_encap_entry_update(int unit,
                              bcmltd_sid_t sid,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              bcmltd_fields_t *out)
{
    int         rv = 0, idx = 0, max_label_per_tnl = 0, count = 0;
    bool        move = false, clear_excess = false;
    uint32_t    old_max_labels = 0, old_num_labels = 0;
    uint8_t     tnl_type = 0, *entry_buf = NULL;
    uint32_t    key_fid = 0, tnl_sw_id = 0, hw_sid = 0;
    uint32_t    tnl_id  = 0, mpls_tnl_id = 0, ret_fid = 0;
    uint32_t    base_idx = 0;
    uint32_t    entry_sz = 0, old_tnl_id = 0, old_mpls_tnl_id = 0;
    uint32_t    sub_id = 0, start_id = 0, num_entries = 0;
    shr_famm_hdl_t   hdl = NULL;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    tnl_mpls_encap_entry_t   entry = {0};

    SHR_FUNC_ENTER(unit);

    switch (sid) {
    case TNL_IPV4_ENCAPt:
        key_fid = TNL_IPV4_ENCAPt_TNL_IPV4_ENCAP_IDf;
        ret_fid = TNL_IPV4_ENCAPt_ENCAP_INDEXf;
        tnl_type = TNL_TYPE_IPV4;
        hw_sid = BCMCTH_TNL_IPV4_HW_SID(unit);
        break;
    case TNL_IPV6_ENCAPt:
        key_fid = TNL_IPV6_ENCAPt_TNL_IPV6_ENCAP_IDf;
        ret_fid = TNL_IPV6_ENCAPt_ENCAP_INDEXf;
        tnl_type = TNL_TYPE_IPV6;
        hw_sid = BCMCTH_TNL_IPV6_HW_SID(unit);
        break;
    case TNL_MPLS_ENCAPt:
        key_fid = TNL_MPLS_ENCAPt_TNL_MPLS_ENCAP_IDf;
        ret_fid = TNL_MPLS_ENCAPt_ENCAP_INDEXf;
        tnl_type = TNL_TYPE_MPLS;
        hw_sid = BCMCTH_TNL_MPLS_HW_SID(unit);
        max_label_per_tnl = BCMCTH_TNL_MAX_MPLS_LABELS(unit);
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    hdl = tnl_fld_array_hdl[unit];

    /* Get the value of the key field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_entry_field_get(unit, key, key_fid, &tnl_sw_id));

    /*
     * Do an IMM lookup to fetch the corresponding LT entry from
     * IMM database.
     */
    in_flds.count = BCMCTH_TNL_NUM_KEY_FIELDS(unit);
    in_flds.field = shr_famm_alloc(hdl, BCMCTH_TNL_NUM_KEY_FIELDS(unit));

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    out_flds.count = BCMCTH_TNL_NUM_DATA_FIELDS(unit);
    out_flds.field = shr_famm_alloc(hdl, BCMCTH_TNL_NUM_DATA_FIELDS(unit));

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    in_flds.field[0]->id = key_fid;
    in_flds.field[0]->data =  tnl_sw_id;

    if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) == SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * Get the previously allocated hardware index from the LT fields.
     * Use the same index for IPv4 and IPv6.
     * For MPLS, check if the index can be re-used. If not, allocate a new index.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_imm_entry_field_get(unit,
                                 &out_flds,
                                 ret_fid,
                                 &tnl_id));
    if (sid == TNL_MPLS_ENCAPt) {

        sal_memset(&entry, 0, sizeof(entry));
        mpls_tnl_id = tnl_id;
        tnl_id = (mpls_tnl_id / max_label_per_tnl);
        sub_id = (mpls_tnl_id % max_label_per_tnl);
        /* Get the previous MAX_LABELS value. */
        rv = tnl_imm_entry_field_get(unit,
                                     &out_flds,
                                     TNL_MPLS_ENCAPt_MAX_LABELSf,
                                     &old_max_labels);
        if (rv != SHR_E_NONE) {
            old_max_labels = 1;
        }

        /* Get the previous NUM_LABELS value. */
        rv = tnl_imm_entry_field_get(unit,
                                     &out_flds,
                                     TNL_MPLS_ENCAPt_NUM_LABELSf,
                                     &old_num_labels);
        if (rv != SHR_E_NONE) {
            old_num_labels = 0;
        }

        /*
         * Parse and populate MPLS entry from IMM fields and
         * current user input.
         */
        SHR_IF_ERR_VERBOSE_EXIT(
            tnl_mpls_encap_entry_parse(unit,
                                       false,
                                       &out_flds,
                                       data,
                                       &entry));

        /* NUM_LABELS should never be greater than MAX_LABELS. */
        if (entry.num_labels > entry.max_labels) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Check if MAX_LABELS value has increased. */
        if (entry.max_labels > old_max_labels) {
            /* Check if current hardware index can be used. */
            SHR_IF_ERR_VERBOSE_EXIT(
                tnl_mpls_validate_curr_index(unit,
                                             &entry,
                                             old_max_labels,
                                             mpls_tnl_id,
                                             &move));
            if (move == true) {
                /* If curent index cannot be used, allocate a new index. */
                old_mpls_tnl_id = mpls_tnl_id;
                SHR_IF_ERR_VERBOSE_EXIT
                    (tnl_mpls_encap_get_free_index(unit,
                                                   entry.max_labels,
                                                   &mpls_tnl_id,
                                                   &tnl_id,
                                                   &sub_id));
            }
        } else {
            /*
             * If MAX_LABELS has decreased from previoud value,
             * the surplus entries can be freed.
             */
            if (entry.max_labels < old_max_labels) {
                clear_excess = true;
            }
        }
    }

    /* Read the entry from hardware. */
    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, hw_sid));
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthtnlEncapHwEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_encap_hw_entry_read(unit,
                                 op_arg,
                                 sid,
                                 hw_sid,
                                 tnl_id,
                                 entry_buf));

    /* Prepare the hw entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_encap_hw_entry_prepare(unit,
                                    hw_sid,
                                    tnl_type,
                                    sub_id,
                                    &entry,
                                    data,
                                    entry_buf));

    /* Write back the entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_encap_hw_entry_write(unit,
                                  op_arg,
                                  sid,
                                  hw_sid,
                                  tnl_id,
                                  entry_buf));

    /* Delete entries from previous index. */
    if (move == true) {
        sal_memset(entry_buf, 0, entry_sz);

        old_tnl_id = (old_mpls_tnl_id / max_label_per_tnl);
        /* For MPLS, clear the fields specific to this entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (tnl_encap_hw_entry_read(unit,
                                     op_arg,
                                     sid,
                                     hw_sid,
                                     old_tnl_id,
                                     entry_buf));

        /* Clear previously allocated hardware index. */
        SHR_IF_ERR_VERBOSE_EXIT
            (tnl_mpls_encap_hw_entry_clear(unit,
                                           hw_sid,
                                           old_mpls_tnl_id,
                                           old_max_labels,
                                           entry_buf));

        /* Write back to the hardware table. */
        SHR_IF_ERR_VERBOSE_EXIT
            (tnl_encap_hw_entry_write(unit,
                                      op_arg,
                                      sid,
                                      hw_sid,
                                      old_tnl_id,
                                      entry_buf));

        /* Mark the old mpls index as free in the mpls tnl bitmap. */
        SHR_BITCLR_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
                         old_mpls_tnl_id, old_max_labels);

        /* Check if all mpls entries in the base tnl have been deleted. */
        base_idx = (old_tnl_id * max_label_per_tnl);
        SHR_BITCOUNT_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
                           count,
                           base_idx,
                           max_label_per_tnl);
        if (!count) {
            /*
             * No more mpls entries in this base tnl.
             * Clear the bit in the base tnl bitmap.
             */
            SHR_BITCLR_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap,
                             (old_tnl_id * BCMCTH_TNL_MPLS_WIDTH(unit)),
                             BCMCTH_TNL_MPLS_WIDTH(unit));
            for (idx = 0; idx < BCMCTH_TNL_MPLS_WIDTH(unit); idx++) {
                BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type
                    [((old_tnl_id * BCMCTH_TNL_MPLS_WIDTH(unit)) + idx)] = TNL_TYPE_NONE;
            }
        }
    }

    /* Clear surplus entries from the current index. */
    if (clear_excess == true) {
        start_id = (mpls_tnl_id + entry.max_labels);
        num_entries = (old_max_labels - entry.max_labels);

        sal_memset(entry_buf, 0, entry_sz);

        /* Read the entry from hardware. */
        SHR_IF_ERR_VERBOSE_EXIT
            (tnl_encap_hw_entry_read(unit,
                                     op_arg,
                                     sid,
                                     hw_sid,
                                     tnl_id,
                                     entry_buf));

        /* Clear extra entries. */
        SHR_IF_ERR_VERBOSE_EXIT
            (tnl_mpls_encap_hw_entry_clear(unit,
                                           hw_sid,
                                           start_id,
                                           num_entries,
                                           entry_buf));

        /* Write back the entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (tnl_encap_hw_entry_write(unit,
                                      op_arg,
                                      sid,
                                      hw_sid,
                                      tnl_id,
                                      entry_buf));

        /* Mark the mpls index as free in the mpls tnl bitmap. */
        SHR_BITCLR_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
                         start_id,
                         num_entries);
    }

    /* Mark the used tnl id in the bitmap and set tnl type. */
    switch (sid) {
    case TNL_IPV4_ENCAPt:
        SHR_BITSET(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap, tnl_id);
        BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type[tnl_id] = TNL_TYPE_IPV4;
        break;
    case TNL_IPV6_ENCAPt:
        SHR_BITSET_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap,
                         (tnl_id * BCMCTH_TNL_IPV6_WIDTH(unit)),
                         BCMCTH_TNL_IPV6_WIDTH(unit));
        /* Save the tunnel type of this base tunnel. */
        for (idx = 0; idx < BCMCTH_TNL_IPV6_WIDTH(unit); idx++) {
            BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type
                [((tnl_id * BCMCTH_TNL_IPV6_WIDTH(unit)) + idx)] = TNL_TYPE_IPV6;
        }
        break;
    case TNL_MPLS_ENCAPt:
        /* Mark the base tnl index as used in base tnl bitmap. */
        SHR_BITSET_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap,
                         (tnl_id * BCMCTH_TNL_MPLS_WIDTH(unit)),
                         BCMCTH_TNL_MPLS_WIDTH(unit));
        /* Mark the mpls index as used in the mpls tnl bitmap. */
        SHR_BITSET_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
                         mpls_tnl_id, entry.max_labels);

        /* Save the tunnel type of this base tunnel. */
        for (idx = 0; idx < BCMCTH_TNL_MPLS_WIDTH(unit); idx++) {
            BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type
                [((tnl_id * BCMCTH_TNL_MPLS_WIDTH(unit)) + idx)] = TNL_TYPE_MPLS;
        }
        break;
    }

    /* Return back harwdare index. */
    if (out) {
        out->field[0]->id = ret_fid;
        out->field[0]->idx = 0;
        if (sid == TNL_MPLS_ENCAPt) {
            out->field[0]->data = mpls_tnl_id;
        } else {
            out->field[0]->data = tnl_id;
        }
        out->count = 1;
    }

exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, BCMCTH_TNL_NUM_KEY_FIELDS(unit));
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, BCMCTH_TNL_NUM_DATA_FIELDS(unit));
    }
    SHR_FREE(entry_buf);

    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_encap_entry_delete(int unit,
                              bcmltd_sid_t sid,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              bcmltd_fields_t *out)
{
    int         idx = 0, max_label_per_tnl = 0, count = 0, rv = 0;
    uint8_t     *entry_buf = NULL;
    uint32_t    key_fid = 0, tnl_sw_id = 0, hw_sid = 0;
    uint32_t    tnl_id  = 0, mpls_tnl_id = 0, ret_fid = 0;
    uint32_t    base_idx = 0, max_labels = 0;
    uint32_t    entry_sz = 0;
    shr_famm_hdl_t   hdl = NULL;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};

    SHR_FUNC_ENTER(unit);

    switch (sid) {
    case TNL_IPV4_ENCAPt:
        key_fid = TNL_IPV4_ENCAPt_TNL_IPV4_ENCAP_IDf;
        ret_fid = TNL_IPV4_ENCAPt_ENCAP_INDEXf;
        hw_sid = BCMCTH_TNL_IPV4_HW_SID(unit);
        break;
    case TNL_IPV6_ENCAPt:
        key_fid = TNL_IPV6_ENCAPt_TNL_IPV6_ENCAP_IDf;
        ret_fid = TNL_IPV6_ENCAPt_ENCAP_INDEXf;
        hw_sid = BCMCTH_TNL_IPV6_HW_SID(unit);
        break;
    case TNL_MPLS_ENCAPt:
        key_fid = TNL_MPLS_ENCAPt_TNL_MPLS_ENCAP_IDf;
        ret_fid = TNL_MPLS_ENCAPt_ENCAP_INDEXf;
        hw_sid = BCMCTH_TNL_MPLS_HW_SID(unit);
        max_label_per_tnl = BCMCTH_TNL_MAX_MPLS_LABELS(unit);
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    hdl = tnl_fld_array_hdl[unit];

    /* Get the value coresponding to the key field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_entry_field_get(unit, key, key_fid, &tnl_sw_id));

    /* Fetch the corrsponding LT entry from the IMM database. */
    in_flds.count = BCMCTH_TNL_NUM_KEY_FIELDS(unit);
    in_flds.field = shr_famm_alloc(hdl, BCMCTH_TNL_NUM_KEY_FIELDS(unit));

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    out_flds.count = BCMCTH_TNL_NUM_DATA_FIELDS(unit);
    out_flds.field = shr_famm_alloc(hdl, BCMCTH_TNL_NUM_DATA_FIELDS(unit));

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    in_flds.field[0]->id = key_fid;
    in_flds.field[0]->data =  tnl_sw_id;

    if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) == SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get the hardware index allocated to this LT entry from the IMM fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_imm_entry_field_get(unit,
                                 &out_flds,
                                 ret_fid,
                                 &tnl_id));
    if (sid == TNL_MPLS_ENCAPt) {
        mpls_tnl_id = tnl_id;
        tnl_id = (mpls_tnl_id / max_label_per_tnl);

        rv = tnl_imm_entry_field_get(unit,
                                     &out_flds,
                                     TNL_MPLS_ENCAPt_MAX_LABELSf,
                                     &max_labels);
        if (rv != SHR_E_NONE) {
            max_labels = 1;
        }
    }

    /* Allocate memory for writing to the hardware table. */
    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, hw_sid));
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthtnlEncapHwEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

    /* Clear the entry data.*/
    sal_memset(entry_buf, 0, entry_sz);

    if (sid == TNL_MPLS_ENCAPt) {
        /* For MPLS, clear the fields specific to this entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (tnl_encap_hw_entry_read(unit,
                                     op_arg,
                                     sid,
                                     hw_sid,
                                     tnl_id,
                                     entry_buf));

        SHR_IF_ERR_VERBOSE_EXIT
            (tnl_mpls_encap_hw_entry_clear(unit,
                                           hw_sid,
                                           mpls_tnl_id,
                                           max_labels,
                                           entry_buf));
    }

    /* Write back to the hardware table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_encap_hw_entry_write(unit,
                                  op_arg,
                                  sid,
                                  hw_sid,
                                  tnl_id,
                                  entry_buf));

    /* Clear the bits from the bitmap and clear tnl type. */
    switch (sid) {
    case TNL_IPV4_ENCAPt:
        SHR_BITCLR(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap, tnl_id);
        BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type[tnl_id] = TNL_TYPE_NONE;
        break;
    case TNL_IPV6_ENCAPt:
        SHR_BITCLR_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap,
                         (tnl_id * BCMCTH_TNL_IPV6_WIDTH(unit)),
                         BCMCTH_TNL_IPV6_WIDTH(unit));
        /* Clear tunnel type of all base tunnel entries used by this tunnel. */
        for (idx = 0; idx < BCMCTH_TNL_IPV6_WIDTH(unit); idx++) {
            BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type
                [((tnl_id * BCMCTH_TNL_IPV6_WIDTH(unit)) + idx)] = TNL_TYPE_NONE;
        }
        break;
    case TNL_MPLS_ENCAPt:
        /* Mark the mpls index as free in the mpls tnl bitmap. */
        SHR_BITCLR_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
                         mpls_tnl_id, max_labels);

        /* Check if all mpls entries in the base tnl have been deleted. */
        base_idx = (tnl_id * max_label_per_tnl);
        SHR_BITCOUNT_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
                           count,
                           base_idx,
                           max_label_per_tnl);
        if (!count) {
            /*
             * No more mpls entries in this base tnl.
             * Clear the bit in the base tnl bitmap.
             */
            SHR_BITCLR_RANGE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap,
                             (tnl_id * BCMCTH_TNL_MPLS_WIDTH(unit)),
                             BCMCTH_TNL_MPLS_WIDTH(unit));
            for (idx = 0; idx < BCMCTH_TNL_MPLS_WIDTH(unit); idx++) {
                BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type
                    [((tnl_id * BCMCTH_TNL_MPLS_WIDTH(unit)) + idx)] = TNL_TYPE_NONE;
            }
        }
        break;
    }

exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, BCMCTH_TNL_NUM_KEY_FIELDS(unit));
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, BCMCTH_TNL_NUM_DATA_FIELDS(unit));
    }
    SHR_FREE(entry_buf);

    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_encap_entry_lookup(int unit,
                              bcmltd_sid_t sid,
                              const bcmltd_op_arg_t *op_arg,
                              void *context,
                              bcmimm_lookup_type_t lkup_type,
                              const bcmltd_fields_t *key,
                              bcmltd_fields_t *out)
{
    int         max_label_per_tnl = 0;
    uint8_t     tnl_type = 0, *entry_buf = NULL;
    uint32_t    hw_sid = 0;
    uint32_t    tnl_id  = 0, ret_fid = 0;
    uint32_t    entry_sz = 0;
    uint32_t    sub_id = 0;
    tnl_mpls_encap_entry_t   entry = {0};

    SHR_FUNC_ENTER(unit);

    /* Return if not getting from HW or not supported. */
    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW) ||
        !(BCMCTH_TNL_IPV4_HW_TO_LT_ENTRY(unit))) {
        SHR_EXIT();
    }

    switch (sid) {
    case TNL_IPV4_ENCAPt:
        ret_fid = TNL_IPV4_ENCAPt_ENCAP_INDEXf;
        tnl_type = TNL_TYPE_IPV4;
        hw_sid = BCMCTH_TNL_IPV4_HW_SID(unit);
        break;
    case TNL_IPV6_ENCAPt:
        ret_fid = TNL_IPV6_ENCAPt_ENCAP_INDEXf;
        tnl_type = TNL_TYPE_IPV6;
        hw_sid = BCMCTH_TNL_IPV6_HW_SID(unit);
        break;
    case TNL_MPLS_ENCAPt:
        ret_fid = TNL_MPLS_ENCAPt_ENCAP_INDEXf;
        tnl_type = TNL_TYPE_MPLS;
        hw_sid = BCMCTH_TNL_MPLS_HW_SID(unit);
        max_label_per_tnl = BCMCTH_TNL_MAX_MPLS_LABELS(unit);
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Get the previously allocated hardware index from the LT fields.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_imm_entry_field_get(unit,
                                 out,
                                 ret_fid,
                                 &tnl_id));
    if (tnl_type == TNL_TYPE_MPLS) {
        sub_id = tnl_id % max_label_per_tnl;
        tnl_id = tnl_id / max_label_per_tnl;
        /*
         * Parse and populate MPLS entry from IMM fields and
         * current user input.
         */
        SHR_IF_ERR_VERBOSE_EXIT(
            tnl_mpls_encap_entry_parse(unit,
                                       false,
                                       out,
                                       NULL,
                                       &entry));
    }

    /* Read the entry from hardware. */
    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, hw_sid));
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthtnlEncapHwEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_encap_hw_entry_read(unit,
                                 op_arg,
                                 sid,
                                 hw_sid,
                                 tnl_id,
                                 entry_buf));

    /* Parse the hw entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_encap_hw_entry_parse(unit,
                                  hw_sid,
                                  tnl_type,
                                  sub_id,
                                  &entry,
                                  entry_buf,
                                  out));

exit:
    SHR_FREE(entry_buf);

    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmcth_tnl_sw_state_cleanup
 *
 * \brief Free Tunnel SW structures.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmcth_tnl_sw_state_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap != NULL) {
        SHR_FREE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap);
    }

    if (BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap != NULL) {
        SHR_FREE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap);
    }

    if (BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type != NULL) {
        SHR_FREE(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type);
    }

    if (tnl_fld_array_hdl[unit]) {
        shr_famm_hdl_delete(tnl_fld_array_hdl[unit]);
        tnl_fld_array_hdl[unit] = 0;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmcth_tnl_sw_state_init
 *
 * \brief Initialise Tunnel SW structures.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  warm            Cold/Warm boot.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmcth_tnl_sw_state_init(int unit, bool warm)
{
    int num_ip_tnl, num_mpls_tnl;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    bcmcth_tnl_base_tnl_state_t *base_tnl_state = NULL;
    bcmcth_tnl_mpls_tnl_state_t *mpls_tnl_state = NULL;
    bcmcth_tnl_base_tnl_type_t *base_tnl_type = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (shr_famm_hdl_init(BCMCTH_TNL_NUM_DATA_FIELDS(unit),
                           &tnl_fld_array_hdl[unit]));

    num_ip_tnl = BCMCTH_TNL_NUM_IPV4(unit);
    num_mpls_tnl = BCMCTH_TNL_NUM_MPLS(unit);

    /* SW state for maintaing IPv4/v6/MPLS database in the Encap Table */
    ha_req_size = sizeof(bcmcth_tnl_base_tnl_state_t) +
                  SHR_BITALLOCSIZE(num_ip_tnl);
    ha_alloc_size = ha_req_size;
    /* Base IPv4/v6 tunnel bitmap */
    base_tnl_state =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_TNL_COMP_ID,
                         BCMCTH_TNL_BASE_TNL_BMAP_SUB_COMP_ID,
                         "bcmcthTnlBaseTnlState",
                         &ha_alloc_size);

    SHR_NULL_CHECK(base_tnl_state, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                             SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(base_tnl_state, 0, ha_alloc_size);
        base_tnl_state ->array_size = SHRi_BITDCLSIZE(num_ip_tnl);
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_TNL_COMP_ID,
                                        BCMCTH_TNL_BASE_TNL_BMAP_SUB_COMP_ID, 0,
                                        ha_req_size, 1,
                                        BCMCTH_TNL_BASE_TNL_STATE_T_ID));
    }
    BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_base_tnl_bitmap =
                                                base_tnl_state->base_tnl_bitmap;
    ha_alloc_size = ha_req_size - sizeof(bcmcth_tnl_base_tnl_state_t);

    /* SW state for maintaing temp IPv4/v6/MPLS database in the Encap Table */
    /* Base IPv4/v6 tunnel bitmap */
    SHR_ALLOC(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap, ha_alloc_size,
              "bcmcthtnlBaseTnlBmap");
    SHR_NULL_CHECK(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap, SHR_E_MEMORY);
    sal_memset(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap, 0,
               ha_alloc_size);

    /* Entry 0 is unused and reserved */
    SHR_BITSET(BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_base_tnl_bitmap, 0);

    /* Syncing the temp space to committed SW state. */
    sal_memcpy(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap,
               BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_base_tnl_bitmap, ha_alloc_size);

    /* MPLS tunnel bitmap */
    ha_req_size = sizeof(bcmcth_tnl_mpls_tnl_state_t) +
                  SHR_BITALLOCSIZE(num_mpls_tnl);
    ha_alloc_size = ha_req_size;
    mpls_tnl_state =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_TNL_COMP_ID,
                         BCMCTH_TNL_MPLS_TNL_BMAP_SUB_COMP_ID,
                         "bcmcthTnlMplsTnlState",
                         &ha_alloc_size);

    SHR_NULL_CHECK(mpls_tnl_state, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                            SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(mpls_tnl_state, 0, ha_alloc_size);
        mpls_tnl_state->array_size = SHRi_BITDCLSIZE(num_mpls_tnl);
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_TNL_COMP_ID,
                                        BCMCTH_TNL_MPLS_TNL_BMAP_SUB_COMP_ID, 0,
                                        ha_req_size, 1,
                                        BCMCTH_TNL_MPLS_TNL_STATE_T_ID));
    }
    BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_mpls_tnl_bitmap =
                                                mpls_tnl_state->mpls_tnl_bitmap;
    ha_alloc_size = ha_req_size - sizeof(bcmcth_tnl_mpls_tnl_state_t);

    /* MPLS tunnel temp bitmap */
    SHR_ALLOC(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap, ha_alloc_size,
              "bcmcthtnlMplsTnlBmap");
    SHR_NULL_CHECK(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap, SHR_E_MEMORY);
    sal_memset(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap, 0,
               ha_alloc_size);

    /* Syncing the temp space to committed SW state. */
    sal_memcpy(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
               BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_mpls_tnl_bitmap, ha_alloc_size);

    /* SW state for maintaing IPv4/v6/MPLS tunnel types in the Encap Table */
    ha_req_size = sizeof(bcmcth_tnl_base_tnl_type_t) +
                  (num_ip_tnl * sizeof(uint8_t));
    ha_alloc_size = ha_req_size;
    /* Base IPv4/v6 tunnel bitmap */
    base_tnl_type =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_TNL_COMP_ID,
                         BCMCTH_TNL_BASE_TNL_TYPE_SUB_COMP_ID,
                         "bcmcthTnlBaseTnlType",
                         &ha_alloc_size);

    SHR_NULL_CHECK(base_tnl_type, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                            SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(base_tnl_type, 0, ha_alloc_size);
        base_tnl_type->array_size = num_ip_tnl;
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_TNL_COMP_ID,
                                        BCMCTH_TNL_BASE_TNL_TYPE_SUB_COMP_ID, 0,
                                        ha_req_size, 1,
                                        BCMCTH_TNL_BASE_TNL_TYPE_T_ID));
    }
    BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_base_tnl_type =
                                                   base_tnl_type->base_tnl_type;
    ha_alloc_size = ha_req_size - sizeof(bcmcth_tnl_base_tnl_type_t);

    /* SW state for maintaing temp IPv4/v6/MPLS database in the Encap Table */
    SHR_ALLOC(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type, ha_alloc_size,
              "bcmcthtnlBaseTnlType");
    SHR_NULL_CHECK(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type, SHR_E_MEMORY);
    sal_memset(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type, 0,
               ha_alloc_size);

    /* Entry 0 is unused and reserved */
    BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_base_tnl_type[0] = TNL_TYPE_NONE;

    /* Syncing the temp space to committed SW state. */
    sal_memcpy(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type,
               BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_base_tnl_type, ha_alloc_size);

exit:
    if (SHR_FUNC_ERR()) {
        if (base_tnl_state != NULL) {
            bcmissu_struct_info_clear(unit,
                                      BCMCTH_TNL_BASE_TNL_STATE_T_ID,
                                      BCMMGMT_TNL_COMP_ID,
                                      BCMCTH_TNL_BASE_TNL_BMAP_SUB_COMP_ID);
            shr_ha_mem_free(unit, base_tnl_state);
        }

        if (mpls_tnl_state != NULL) {
            bcmissu_struct_info_clear(unit,
                                      BCMCTH_TNL_MPLS_TNL_STATE_T_ID,
                                      BCMMGMT_TNL_COMP_ID,
                                      BCMCTH_TNL_MPLS_TNL_BMAP_SUB_COMP_ID);
            shr_ha_mem_free(unit, mpls_tnl_state);
        }

        if (base_tnl_type != NULL) {
            bcmissu_struct_info_clear(unit,
                                      BCMCTH_TNL_BASE_TNL_TYPE_T_ID,
                                      BCMMGMT_TNL_COMP_ID,
                                      BCMCTH_TNL_BASE_TNL_TYPE_SUB_COMP_ID);
            shr_ha_mem_free(unit, base_tnl_type);
        }

        bcmcth_tnl_sw_state_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmcth_tnl_transaction_commit
 *
 * \brief Commit SW states for the tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  lt_sid          Logical table ID.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  context         Context pointer.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmcth_tnl_transaction_commit(int unit,
                              bcmltd_sid_t lt_sid,
                              uint32_t trans_id,
                              void *context)
{
    int         num_ip_tnl, num_mpls_tnl;
    uint32_t    alloc_sz = 0;

    SHR_FUNC_ENTER(unit);

    num_ip_tnl = BCMCTH_TNL_NUM_IPV4(unit);
    num_mpls_tnl = BCMCTH_TNL_NUM_MPLS(unit);

    alloc_sz =  SHR_BITALLOCSIZE(num_mpls_tnl);
    sal_memcpy(BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_mpls_tnl_bitmap,
               BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
               alloc_sz);

    alloc_sz =  SHR_BITALLOCSIZE(num_ip_tnl);
    sal_memcpy(BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_base_tnl_bitmap,
               BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap,
               alloc_sz);

    alloc_sz = (num_ip_tnl * sizeof(uint8_t));
    sal_memcpy(BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_base_tnl_type,
               BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type,
               alloc_sz);

    SHR_FUNC_EXIT();
}

/*!
 * \fn bcmcth_tnl_transaction_abort
 *
 * \brief Abort SW states for the tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  lt_sid          Logical table ID.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  context         Context pointer.
 *
 * \retval SHR_E_NONE  OK
 */
void
bcmcth_tnl_transaction_abort(int unit,
                             bcmltd_sid_t lt_sid,
                             uint32_t trans_id,
                             void *context)
{
    uint32_t    alloc_sz = 0;
    int         num_ip_tnl, num_mpls_tnl;

    num_ip_tnl = BCMCTH_TNL_NUM_IPV4(unit);
    num_mpls_tnl = BCMCTH_TNL_NUM_MPLS(unit);

    alloc_sz =  SHR_BITALLOCSIZE(num_mpls_tnl);
    sal_memcpy(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_mpls_tnl_bitmap,
               BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_mpls_tnl_bitmap,
               alloc_sz);

    alloc_sz =  SHR_BITALLOCSIZE(num_ip_tnl);
    sal_memcpy(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_bitmap,
               BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_base_tnl_bitmap,
               alloc_sz);

    alloc_sz = (num_ip_tnl * sizeof(uint8_t));
    sal_memcpy(BCMCTH_TNL_ENCAP_SW_STATE(unit).bcmcth_tnl_base_tnl_type,
               BCMCTH_TNL_ENCAP_SW_STATE_HA(unit).bcmcth_tnl_base_tnl_type,
               alloc_sz);
}

int
bcmcth_tnl_encap_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    tnl_encap_drv[unit] = tnl_encap_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_tnl_encap_drv_get(int unit, bcmcth_tnl_encap_drv_t **drv)
{

    SHR_FUNC_ENTER(unit);

    if (tnl_encap_drv[unit] != NULL) {
        *drv = tnl_encap_drv[unit];
    } else {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}
