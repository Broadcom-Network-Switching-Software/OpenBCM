/*! \file bcmltx_field_demux_acc.c
 *
 * Field demux/mux handler
 *
 * Built-in Transform implementation to split (demux) or recombine
 * (mux) API fields when they do not map directly to PT fields.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmltx/bcmltx_field_demux_util.h>
#include <bcmltx/bcmltx_field_demux_acc.h>

/*! BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*!
 * \brief Get accumulated transform magic number.
 *
 * Get magic number for accumulated transform in bcmltd generic arg.
 *
 * \param [in]  arg           LTD generic arg.
 *
 * \retval magic              Magic number.
 */
static uint32_t
bcmltx_field_mux_acc_transform_magic(const bcmltd_generic_arg_t *arg)
{
    uint32_t magic = 0;
    if (arg != NULL && arg->user_data != NULL) {
        bcmltd_rev_subfield_transform_info_t *revinfo =
            (bcmltd_rev_subfield_transform_info_t *)arg->user_data;
        magic = revinfo->magic;
    }

    return magic;
}

/*!
 * \brief Check accumulated transform magic number.
 *
 * Check magic number for accumulated transform in bcmltd generic arg.
 *
 * \param [in]  arg           LTD generic arg.
 *
 * \retval SHR_E_NONE         No error.
 * \retval SHR_E_INTERNAL     Error.
 */
static int
bcmltx_field_mux_acc_transform_check_magic(const bcmltd_generic_arg_t *arg)
{
    int rv = SHR_E_NONE;
    uint32_t magic = bcmltx_field_mux_acc_transform_magic(arg);

    if (magic != BCMLTD_REV_SUBFIELD_TRANSFORM_INFO_MAGIC) {
        rv = SHR_E_INTERNAL;
    }

    return rv;
}

/*!
 * \brief Reset user data.
 *
 * Reset user data(count and value).
 *
 * \param [in]  arg           LTD generic arg.
 *
 * \retval SHR_E_NONE         No error.
 * \retval SHR_E_INTERNAL     Error.
 */
static int
bcmltx_field_mux_acc_transform_reset_user_data(const bcmltd_generic_arg_t *arg)
{
    uint32_t i;
    int rv;
    rv = bcmltx_field_mux_acc_transform_check_magic(arg);

    if (SHR_SUCCESS(rv)) {
        bcmltd_rev_subfield_transform_info_t *revinfo =
            (bcmltd_rev_subfield_transform_info_t *)arg->user_data;
        *revinfo->count = 0;
        for (i = 0; i < revinfo->values; i++) {
            revinfo->value[i] = 0;
        }
    }

    return rv;
}

int
bcmltx_field_mux_acc_transform_clear_state(const bcmltd_generic_arg_t *arg)
{
    uint32_t magic = bcmltx_field_mux_acc_transform_magic(arg);

    if (magic == BCMLTD_REV_SUBFIELD_TRANSFORM_INFO_MAGIC) {
        bcmltd_rev_subfield_transform_info_t *revinfo =
            (bcmltd_rev_subfield_transform_info_t *)arg->user_data;
        /* Clear reverse subfield transform state counter and values. */
        *revinfo->count = 0;
        sal_memset(revinfo->value,
                   0,
                   sizeof(revinfo->value[0])*revinfo->values);
    }

    return 0;
}

int
bcmltx_field_mux_acc_transform_update_value(const bcmltd_generic_arg_t *arg,
                                            const uint32_t index,
                                            const uint64_t value)
{
    int rv;

    rv = bcmltx_field_mux_acc_transform_check_magic(arg);
    if (SHR_SUCCESS(rv)) {
        bcmltd_rev_subfield_transform_info_t *revinfo =
            (bcmltd_rev_subfield_transform_info_t *)arg->user_data;
        revinfo->value[index] |= value;
        (*revinfo->count)++;
    }

    return rv;
}

int
bcmltx_field_mux_acc_transform_get_value(const bcmltd_generic_arg_t *arg,
                                         const uint32_t index,
                                         uint64_t *value)
{
    int rv;

    rv = bcmltx_field_mux_acc_transform_check_magic(arg);
    if (SHR_SUCCESS(rv)) {
        bcmltd_rev_subfield_transform_info_t *revinfo =
            (bcmltd_rev_subfield_transform_info_t *)arg->user_data;
        *value = revinfo->value[index];
    }

    return rv;
}

int
bcmltx_field_mux_acc_transform_is_copy_needed(const bcmltd_generic_arg_t *arg,
                                              int *need_copy)
{
    int rv;
    rv = bcmltx_field_mux_acc_transform_check_magic(arg);

    if (SHR_SUCCESS(rv)) {
        bcmltd_rev_subfield_transform_info_t *revinfo =
            (bcmltd_rev_subfield_transform_info_t *)arg->user_data;
        if (*revinfo->count == revinfo->num) {
            *need_copy = 1;
        } else {
            *need_copy = 0;
        }
    }

    return rv;
}

int
bcmltx_field_demux_acc_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg)
{
    uint32_t ifield_ix, ifield_num;
    const bcmltd_field_list_t *src_list;
    const bcmltd_field_list_t *dst_list;
    uint32_t src_field_id;
    uint32_t dst_fcount = 0;
    uint32_t slist_ix;
    int rv;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_field_demux_acc_transform \n")));

    BCMLTX_VALIDATE_ERR(arg->fields != 1);

    /* Clear the mux transform state (if any). This is needed when a
       transaction has a sequence of operations. */
    rv = bcmltx_field_mux_acc_transform_clear_state(arg->comp_data);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(rv);
    }

    /* src(logical)  : in (field)  */
    /* dst(physical) : out(rfield) */
    src_list = arg->field_list;
    dst_list = arg->rfield_list;

    src_field_id = arg->field[0];

    ifield_num = in->count;
    if (ifield_num == 0) {
        SHR_EXIT();
    }

    /* Check LTA input list matching field */
    for (ifield_ix = 0; ifield_ix < ifield_num; ifield_ix++) {
        BCMLTX_VALIDATE_ERR(in->field[ifield_ix]->id != src_field_id);
    }

    /* Extract data from in->field and copy data into out->field.
     * The example of wide field data copy from in->field to out->field.
     * #1. IPV6_ADDR[39:0]:   MEMDB_EFTA20_MEM7.DATA[127:88];
     * #2. IPV6_ADDR[127:40]: MEMDB_EFTA20_MEM6.DATA[87:0];
     * This function will be called twice.
     * 1st time for map entry #1.
     * 2nd time for map entry #2.
     *
     * #1
     * in->field                          out->field
     *     +-----+--------------------+       +----+--------------------+
     *     |i_ix |63   40|39         0|       |idx |63   40|39         0|
     *     +-----+-------+------------+       +----+-------+------------+
     *     | 0   | data2 |    data1   |       | 0  |       |    data1   |
     *     +-----+-------+------------+       +----+-------+------------+
     *     | 1   | data4 |    data3   |
     *     +-----+-------+------------+
     * #2
     * in->field                          out->field
     *     +-----+--------------------+       +----+----------+---------+
     *     |i_ix |63   40|39         0|       |idx |63      24|23      0|
     *     +-----+-------+------------+       +----+----------+---------+
     *     | 0   | data2 |    data1   |       | 0  |   data3  |  data2  |
     *     +-----+-------+------------+       +----+----------+---------+
     *     | 1   | data4 |    data3   |       | 1  |          |  data4  |
     *     +-----+-------+------------+       +----+----------+---------+
     */
    for (slist_ix = 0; slist_ix < src_list->field_num; slist_ix++) {
        uint32_t nchunks = 0;
        bcmltx_chunk_t *chunks = NULL;
        const bcmltd_field_desc_t *src_field;
        const bcmltd_field_desc_t *dst_field;
        uint32_t ix;

        src_field = &(src_list->field_array[slist_ix]);
        dst_field = &(dst_list->field_array[slist_ix]);

        /* Split the range into 64bit chunks. */
        SHR_IF_ERR_EXIT(
            bcmltx_chunk_split(unit, src_field->minbit, src_field->maxbit,
                               &chunks, &nchunks));

        /* For each chunk, extract data from in->field(LT)
         * and copy it to out->field(PT). */
        for (ix = 0; ix < nchunks; ix++) {
            bcmltd_field_t *lta_field;
            uint64_t lta_data;
            uint32_t out_index;
            bcmltx_chunk_t *c = &chunks[ix];

            /* Extract data from in->field. */
            lta_field = in->field[c->src_idx];
            lta_data = ((lta_field->data >> c->minbit) & c->bitmask);
            lta_data <<= c->dst_offset;

            /* Write data to out->field. */
            out_index = dst_fcount + c->dst_idx;
            rv = bcmltx_lta_field_data_set(unit,
                                           out,
                                           out_index,
                                           dst_field->field_id,
                                           c->dst_idx,
                                           lta_data);
            if (SHR_FAILURE(rv)) {
                SHR_FREE(chunks);
                SHR_IF_ERR_EXIT(rv);
            }
        }
        dst_fcount++;
        SHR_FREE(chunks);
    }

    SHR_EXIT();

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_field_mux_acc_transform(int unit,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_transform_arg_t *arg)
{
    const bcmltd_field_list_t *src_list;
    uint32_t slist_ix;
    uint32_t dst_fcount = 0;
    uint32_t ix;
    uint32_t sfield_num;
    int copy = 0;
    int rv;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_field_mux_acc_transform \n")));

    rv = bcmltx_field_mux_acc_transform_check_magic(arg->comp_data);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* src(logical)  : out(rfield) */
    src_list = arg->rfield_list;

    BCMLTX_VALIDATE_ERR(out->count < 1)
    BCMLTX_VALIDATE_ERR(arg->rfields != 1)

    /* Save out count and reset it to 0.
     * out->count should be set only when
     * data is copied to out->field buffer. */
    sfield_num = out->count;
    out->count = 0;

    for (slist_ix = 0; slist_ix < src_list->field_num; slist_ix++) {
        uint32_t nchunks = 0;
        bcmltx_chunk_t *chunks = NULL;
        const bcmltd_field_desc_t *src_field;

        src_field = &(src_list->field_array[slist_ix]);

        /* Split the range into 64bit chunks. */
        SHR_IF_ERR_EXIT(
            bcmltx_chunk_split(unit, src_field->minbit, src_field->maxbit,
                               &chunks, &nchunks));

        /* For each chunk, extract data from in->field(PT)
         * and copy it to out->field(LT). */
        for (ix = 0; ix < nchunks; ix++) {
            bcmltd_field_t *lta_field;
            uint64_t lta_data;
            uint32_t in_index;
            bcmltx_chunk_t *c = &chunks[ix];

            /* Extract data from in->field. */
            in_index = dst_fcount + c->dst_idx;
            lta_field = in->field[in_index];
            lta_data = ((lta_field->data >> c->dst_offset) & c->bitmask);
            lta_data <<= c->src_offset;

            /* Store data into comp_data. */
            rv = bcmltx_field_mux_acc_transform_update_value(arg->comp_data,
                                                             c->src_idx, lta_data);
            if (SHR_FAILURE(rv)) {
                SHR_FREE(chunks);
                SHR_IF_ERR_EXIT(rv);
            }
        }
        SHR_FREE(chunks);

        /* Check if all chunks are copied into comp_data. */
        SHR_IF_ERR_EXIT(
            bcmltx_field_mux_acc_transform_is_copy_needed(arg->comp_data,
                                                                  &copy));
        if (copy) {
            /* Copy stored comp_data into out->field(LT) buffer. */
            uint32_t sfield_ix;

            for (sfield_ix = 0; sfield_ix < sfield_num ; sfield_ix++) {
                uint64_t lta_data;

                SHR_IF_ERR_EXIT(
                    bcmltx_field_mux_acc_transform_get_value(arg->comp_data,
                                                             sfield_ix, &lta_data));

                SHR_IF_ERR_EXIT(
                    bcmltx_lta_field_data_set(unit,
                                              out,
                                              sfield_ix,
                                              src_field->field_id,
                                              sfield_ix,
                                              lta_data));
            }
            out->count = sfield_ix;

            SHR_IF_ERR_EXIT(
                bcmltx_field_mux_acc_transform_reset_user_data(arg->comp_data));
        }

        dst_fcount++;
    }

    SHR_EXIT();
 exit:
    SHR_FUNC_EXIT();
}

