/*! \file bcmltx_field_demux_wide.c
 *
 * Field demux/mux handler for wide scalar fields.
 *
 * Built-in Transform implementation to split (demux) or recombine
 * (mux) API fields when they do not map directly to PT fields.
 *
 * This is for wide fields (array of uint64_t) placed in a single memory.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmltx/bcmltx_field_demux_util.h>
#include <bcmltx/bcmltx_field_demux_wide.h>

/*! BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL


/*
 * Split up the subfields of the given LT field into separate
 * pieces. Each subfield piece corresponds to a portion of the
 * destination PT field.
 */
int
bcmltx_field_demux_wide_transform(int unit,
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
    uint32_t greatest_dst_idx;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_field_demux_acc_transform \n")));

    BCMLTX_VALIDATE_ERR(arg->fields != 1);

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
        greatest_dst_idx = 0;

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

            if (c->dst_idx > greatest_dst_idx) {
                greatest_dst_idx = c->dst_idx;
            }
            if (SHR_FAILURE(rv)) {
                SHR_FREE(chunks);
                SHR_IF_ERR_EXIT(rv);
            }
        }
        dst_fcount += greatest_dst_idx + 1;
        SHR_FREE(chunks);
    }

    SHR_EXIT();

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_field_mux_wide_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    const bcmltd_field_list_t *src_list;
    uint32_t slist_ix;
    uint32_t dst_fcount = 0;
    uint32_t ix;
    uint64_t *out_lta_data = NULL;
    int rv;
    uint32_t greatest_dst_idx;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_field_mux_acc_transform \n")));

    /* src(logical)  : out(rfield) */
    src_list = arg->rfield_list;

    BCMLTX_VALIDATE_ERR(out->count < 1)
    BCMLTX_VALIDATE_ERR(arg->rfields != 1)

    for (slist_ix = 0; slist_ix < src_list->field_num; slist_ix++) {
        uint32_t nchunks = 0;
        uint32_t size = 0;
        bcmltx_chunk_t *chunks = NULL;
        const bcmltd_field_desc_t *src_field;
        uint32_t sfield_ix, sfield_num;

        src_field = &(src_list->field_array[slist_ix]);

        /* Alloc memory for temporary out field buffer. */
        sfield_num = out->count;
        size = sizeof(uint64_t) * (sfield_num);
        SHR_ALLOC(out_lta_data, size, "bcmltxLTAOut");
        SHR_NULL_CHECK(out_lta_data, SHR_E_MEMORY);
        sal_memset(out_lta_data, 0, size);

        /* Split the range into 64bit chunks. */
        rv = bcmltx_chunk_split(unit, src_field->minbit, src_field->maxbit,
                                &chunks, &nchunks);
        if (SHR_FAILURE(rv)) {
            SHR_FREE(out_lta_data);
            SHR_IF_ERR_EXIT(rv);
        }
        greatest_dst_idx = 0;

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
            out_lta_data[c->src_idx] |= lta_data;

            if (c->dst_idx > greatest_dst_idx) {
                greatest_dst_idx = c->dst_idx;
            }
        }
        SHR_FREE(chunks);

        /* Copy stored out_lta_data into out->field(LT) buffer. */
        for (sfield_ix = 0; sfield_ix < sfield_num ; sfield_ix++) {
            rv = bcmltx_lta_field_data_set(unit,
                                           out,
                                           sfield_ix,
                                           src_field->field_id,
                                           sfield_ix,
                                           out_lta_data[sfield_ix]);

            if (SHR_FAILURE(rv)) {
                SHR_FREE(out_lta_data);
                SHR_IF_ERR_EXIT(rv);
            }
        }

        dst_fcount += greatest_dst_idx + 1;
        SHR_FREE(out_lta_data);
    }

    SHR_EXIT();
 exit:

    SHR_FUNC_EXIT();
}

