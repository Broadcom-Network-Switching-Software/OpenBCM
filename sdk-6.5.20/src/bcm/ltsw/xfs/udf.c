/*! \file udf.c
 *
 * UDF Driver for XFS chips.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/udf_int.h>
#include <bcm_int/ltsw/xfs/udf.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcm_int/control.h>
/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_UDF

bcmint_udf_abstr_pkt_fmt_ltinfo_t
      *ltsw_udf_abstr_pkt_fmt_ltinfo[BCM_MAX_NUM_UNITS]
                                    [bcmUdfAbstractPktFormatLastCount];

bcmint_udf_lt_tbls_info_t *udf_tbls_info[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

/*
 * \brief ltsw_udf_lt_entry_commit
 *
 * Function to perform commit operation for a given LT table.
 *
 * Params:
 *    template     - (IN/OUT) Reference to entry handle.
 *    operation    - (IN)     Commit operation.
 *    priority     - (IN)     Commit priority.
 *    table_name   - (IN)     LT table name.
 *    key_name     - (IN)     LT key name.
 *    key_val      - (IN)     LT key val.
 *    field_name   - (IN)     LT field name.
 *    field_val    - (IN)     field value.
 *    free         - (IN)     If set to 1, free the template.
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *    !BCM_E_NONE      - Failure
 */
static int
ltsw_udf_lt_entry_commit(int unit,
                         bcmlt_entry_handle_t *template,
                         bcmlt_opcode_t operation,
                         bcmlt_priority_level_t priority,
                         char *table_name,
                         char *key_name,
                         bool key_symbol,
                         uint64_t key_val,
                         char *key_str,
                         char *field_name,
                         bool symbol,
                         uint64_t field_val,
                         char *field_str_val,
                         int free_template)
{
    int dunit;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    if (*template == BCMLT_INVALID_HDL) {
        /* Entry handle allocate for given template */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_allocate(dunit, table_name, template));
    }

    /* Add given key to the template. */
    if (key_symbol == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_symbol_add(*template, key_name, key_str));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(*template, key_name, key_val));
    }

    if (field_name != NULL) {
        /* Add given field value to the given field_name. */
        if (symbol == TRUE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(*template, field_name,
                                               field_str_val));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(*template, field_name, field_val));
        }
    }

    /* Perform the operation */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_lt_entry_commit(unit, *template, operation, priority));

exit:
    if (free_template) {
        (void) bcmlt_entry_free(*template);
        *template = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_cont_set(
    int unit,
    bcmlt_entry_handle_t udf_tmpl,
    bcmint_udf_abstr_pkt_fmt_ltinfo_t *lt_info,
    bcm_udf_cbmp_t chunk_bmap,
    char *cont_byte_fid,
    uint64_t *cont_arr,
    uint32_t cont_cnt)
{
    int c, rv;
    uint32_t act_cnt = 0;
    SHR_FUNC_ENTER(unit);

    rv = (bcmlt_entry_field_array_get(udf_tmpl,
                                     cont_byte_fid,
                                     0, cont_arr, cont_cnt, &act_cnt));
    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (SHR_SUCCESS(rv) && cont_cnt != act_cnt) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    for (c = 0; c < BCMINT_UDF_MAX_CHUNKS(unit); c++) {
        if (BCM_UDF_CBMP_CHUNK_TEST(chunk_bmap, c)) {
            if (cont_arr[c] == 1) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
            }
            cont_arr[c] = 1;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
         (bcmlt_entry_field_array_add(udf_tmpl,
                                      cont_byte_fid,
                                      0, cont_arr, cont_cnt));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_cont_alloc(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info,
    bcmint_udf_abstr_pkt_fmt_ltinfo_t *lt_info,
    uint64_t **cont_1_byte,
    uint32_t *num_1_byte,
    uint64_t **cont_2_byte,
    uint32_t *num_2_byte,
    uint64_t **cont_4_byte,
    uint32_t *num_4_byte)
{
    int c;
    uint32_t c_1b_cnt = 0;
    uint32_t c_2b_cnt = 0;
    uint32_t c_4b_cnt = 0;
    SHR_FUNC_ENTER(unit);

    for (c = 0; c < BCMINT_UDF_MAX_CHUNKS(unit); c++) {
        if (BCM_UDF_CBMP_CHUNK_TEST(info->one_byte_chunk_bmap, c)) {
            if ((lt_info->num_1b_chunks == 0) ||
                (c >= lt_info->num_1b_chunks)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            c_1b_cnt += 1;
        }

        if (BCM_UDF_CBMP_CHUNK_TEST(info->two_byte_chunk_bmap, c)) {
            if ((lt_info->num_2b_chunks == 0) ||
                (c >= lt_info->num_2b_chunks)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            c_2b_cnt += 1;
        }

        if (BCM_UDF_CBMP_CHUNK_TEST(info->four_byte_chunk_bmap, c)) {
            if ((lt_info->num_4b_chunks == 0) ||
                (c >= lt_info->num_4b_chunks)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            c_4b_cnt += 1;
        }
    }

    /* Validate whether the given chunks exceeds supported available chunks */
    if ((c_1b_cnt + c_2b_cnt + c_4b_cnt) > lt_info->num_avail_chunks) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (c_1b_cnt) {
        BCMINT_UDF_MEMALLOC(*cont_1_byte,
                         (sizeof(uint64_t) * lt_info->num_1b_chunks),
                         "Alloc Cont_1_byte");
        *num_1_byte = lt_info->num_1b_chunks;
    }

    if (c_2b_cnt) {
        BCMINT_UDF_MEMALLOC(*cont_2_byte,
                         (sizeof(uint64_t) * lt_info->num_2b_chunks),
                         "Alloc Cont_2_byte");
        *num_2_byte = lt_info->num_2b_chunks;
    }

    if (c_4b_cnt) {
        BCMINT_UDF_MEMALLOC(*cont_4_byte,
                         (sizeof(uint64_t) * lt_info->num_4b_chunks),
                         "Alloc Cont_4_byte");
        *num_4_byte = lt_info->num_4b_chunks;
    }

exit:
    SHR_FUNC_EXIT();
}


/******************************************************************************
 * Public functions
 */
int
xfs_ltsw_udf_init(int unit)
{
    bcmint_udf_control_t *udf_ctrl = NULL;
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    bcmint_udf_abstr_pkt_fmt_ltinfo_t *lt_info;
    bcmlt_entry_handle_t udf_info_tmpl = BCMLT_INVALID_HDL;
    uint64_t byte64_1 = 0;
    uint64_t byte64_2 = 0;
    SHR_FUNC_ENTER(unit);

    udf_ctrl = UDF_CONTROL(unit);

    /* check the range of UDF object IDs */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit,
                                       udf_tbls_info[unit]->udf_info_sid,
                                       udf_tbls_info[unit]->udf_info_key_fid,
                                       &byte64_1, &byte64_2));

    udf_ctrl->info.min_obj_id = byte64_1;
    udf_ctrl->info.max_obj_id = byte64_2;
    /* update maximum number of chunks */
    udf_ctrl->info.max_chunks = 32;
    for (pkt_fmt = 0; pkt_fmt < bcmUdfAbstractPktFormatLastCount; pkt_fmt++) {
         lt_info = ltsw_udf_abstr_pkt_fmt_ltinfo[unit][pkt_fmt];
         if (lt_info == NULL) {
             continue;
         }

         /* Allocate and Read UDF Policy Info template */
         SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_entry_commit(unit, &udf_info_tmpl,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      udf_tbls_info[unit]->udf_info_sid,
                                      udf_tbls_info[unit]->udf_info_key_fid,
                                      1,
                                      0,
                                      lt_info->pkt_header,
                                      NULL, 0, 0, NULL,
                                      0));
         SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(udf_info_tmpl,
                                   udf_tbls_info[unit]->udf_num_cont_1b_fid,
                                   &byte64_1));
         lt_info->num_1b_chunks = byte64_1;

         SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(udf_info_tmpl,
                                   udf_tbls_info[unit]->udf_num_cont_2b_fid,
                                   &byte64_1));
         lt_info->num_2b_chunks = byte64_1;

         SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(udf_info_tmpl,
                                   udf_tbls_info[unit]->udf_num_cont_4b_fid,
                                   &byte64_1));
         lt_info->num_4b_chunks = byte64_1;

         SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(udf_info_tmpl,
                                   udf_tbls_info[unit]->udf_max_cont_fid,
                                   &byte64_1));
         lt_info->num_avail_chunks = byte64_1;

         lt_info->avail_1b_cbmap = ((1 << lt_info->num_1b_chunks) - 1);
         lt_info->avail_2b_cbmap = ((1 << lt_info->num_2b_chunks) - 1);
         lt_info->avail_4b_cbmap = ((1 << lt_info->num_4b_chunks) - 1);
    }
exit:
    if (udf_info_tmpl != BCMLT_INVALID_HDL) {
       (void) bcmlt_entry_free(udf_info_tmpl);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_udf_detach(int unit)
{
    int rv;
    int warm = 0;
    int dunit;
    uint64_t udf_id = 0;
    bcmlt_entry_handle_t udf_tmpl = BCMLT_INVALID_HDL;
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    bcmint_udf_abstr_pkt_fmt_ltinfo_t *lt_info;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    warm = bcmi_warmboot_get(unit);
    if (!warm) {
        /* Entry handle allocate for given template */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  udf_tbls_info[unit]->udf_sid,
                                  &udf_tmpl));

        /*
         * Traverse UDF policy table and retrieve the UDF objects created
         * the retrieved LT entries.
         */
        while ((rv = bcmlt_entry_commit(udf_tmpl,
                                        BCMLT_OPCODE_TRAVERSE,
                                        BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
            /* Retrieve the UDF Policy index */
            SHR_IF_ERR_VERBOSE_EXIT
                 (bcmlt_entry_field_get(udf_tmpl,
                                        udf_tbls_info[unit]->udf_key_fid,
                                        &udf_id));
            /* Destroy UDF objects */
            SHR_IF_ERR_VERBOSE_EXIT
                 (xfs_ltsw_udf_destroy(unit, (bcm_udf_id_t)udf_id));
        }
    }

    for (pkt_fmt = 0; pkt_fmt < bcmUdfAbstractPktFormatLastCount; pkt_fmt++) {
         lt_info = ltsw_udf_abstr_pkt_fmt_ltinfo[unit][pkt_fmt];
         if (lt_info != NULL) {
             BCMINT_UDF_MEMFREE(lt_info->pkt_header);
             BCMINT_UDF_MEMFREE(lt_info);
             ltsw_udf_abstr_pkt_fmt_ltinfo[unit][pkt_fmt] = NULL;
         }
    }
exit:
    if (udf_tmpl != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(udf_tmpl);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_udf_multi_chunk_create(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info)
{
    uint64_t *cont_1_byte = NULL;
    uint64_t *cont_2_byte = NULL;
    uint64_t *cont_4_byte = NULL;
    uint32_t num_1_byte = 0;
    uint32_t num_2_byte = 0;
    uint32_t num_4_byte = 0;
    uint64_t udf_id_64 = udf_id;
    bcmint_udf_abstr_pkt_fmt_ltinfo_t *lt_info = NULL;
    bcmlt_entry_handle_t udf_tmpl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    lt_info = ltsw_udf_abstr_pkt_fmt_ltinfo[unit][info->abstract_pkt_format];
    if (lt_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* Granularity is 8 bits. */
    if ((info->offset % 8) != 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* LT container alloc */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_cont_alloc(unit, udf_id, info,
                                lt_info,
                                &cont_1_byte, &num_1_byte,
                                &cont_2_byte, &num_2_byte,
                                &cont_4_byte, &num_4_byte));

    /* Allocate and Read UDF Policy Info template */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_entry_commit(unit, &udf_tmpl,
                                  BCMLT_OPCODE_INSERT,
                                  BCMLT_PRIORITY_NORMAL,
                                  udf_tbls_info[unit]->udf_sid,
                                  udf_tbls_info[unit]->udf_key_fid,
                                  0, udf_id_64, NULL,
                                  NULL, 0, 0, NULL,
                                  0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(udf_tmpl,
                                      udf_tbls_info[unit]->udf_anchor_fid,
                                      lt_info->pkt_header));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(udf_tmpl,
                               udf_tbls_info[unit]->udf_offset_fid,
                               ((info->offset)/8)));
    if (num_1_byte) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_cont_set(unit, udf_tmpl, lt_info,
                                  info->one_byte_chunk_bmap,
                                  udf_tbls_info[unit]->udf_cont_1_byte_fid,
                                  cont_1_byte, num_1_byte));
    }
    if (num_2_byte) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_cont_set(unit, udf_tmpl, lt_info,
                                  info->two_byte_chunk_bmap,
                                  udf_tbls_info[unit]->udf_cont_2_byte_fid,
                                  cont_2_byte, num_2_byte));
    }
    if (num_4_byte) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_cont_set(unit, udf_tmpl, lt_info,
                                  info->four_byte_chunk_bmap,
                                  udf_tbls_info[unit]->udf_cont_4_byte_fid,
                                  cont_4_byte, num_4_byte));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, udf_tmpl,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (SHR_FAILURE(_func_rv) &&
        (udf_tmpl != BCMLT_INVALID_HDL)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, udf_tmpl,
                                  BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL));
    }

    if (udf_tmpl != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(udf_tmpl);
    }

    if (cont_1_byte) {
        BCMINT_UDF_MEMFREE(cont_1_byte);
    }
    if (cont_2_byte) {
        BCMINT_UDF_MEMFREE(cont_2_byte);
    }
    if (cont_4_byte) {
        BCMINT_UDF_MEMFREE(cont_4_byte);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_udf_multi_chunk_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info,
    int *flags)
{
    int c;
    int width = 0;
    uint64_t offset = 0;
    uint64_t udf_id_64 = 0;
    uint32_t act_1b_cnt = 0;
    uint32_t act_2b_cnt = 0;
    uint32_t act_4b_cnt = 0;
    const char *pkt_hdr = NULL;
    uint64_t *cont_1b_arr = NULL;
    uint64_t *cont_2b_arr = NULL;
    uint64_t *cont_4b_arr = NULL;
    int first_1b_cont = -1;
    int first_2b_cont = -1;
    int first_4b_cont = -1;
    bcm_udf_abstract_pkt_format_t fmt;
    bcmint_udf_abstr_pkt_fmt_ltinfo_t *lt_info = NULL;
    bcmlt_entry_handle_t udf_tmpl = BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    if (info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    udf_id_64 = udf_id;
    /* UDF Policy object delete operation */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_entry_commit(unit, &udf_tmpl,
                                  BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL,
                                  udf_tbls_info[unit]->udf_sid,
                                  udf_tbls_info[unit]->udf_key_fid,
                                  0, udf_id_64, NULL,
                                  NULL,
                                  0, 0, NULL,
                                  0));

    /* retrieve the UDF anchor */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(udf_tmpl,
                                      udf_tbls_info[unit]->udf_anchor_fid,
                                      &pkt_hdr));

    /* retrieve the offset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(udf_tmpl,
                               udf_tbls_info[unit]->udf_offset_fid,
                               &offset));
    info->offset = offset * 8;

    /* Update abstract packet format */
    for (fmt = 0; fmt < bcmUdfAbstractPktFormatLastCount; fmt++) {
         lt_info = ltsw_udf_abstr_pkt_fmt_ltinfo[unit][fmt];
         if ((lt_info != NULL) &&
             (sal_strcmp(pkt_hdr, lt_info->pkt_header) == 0)) {
             break;
         }
    }

    if (fmt == bcmUdfAbstractPktFormatLastCount) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    info->abstract_pkt_format = fmt;

    /* Update 1/2/4 byte chunk bitmaps */
    if (lt_info->num_1b_chunks) {
        BCMINT_UDF_MEMALLOC(cont_1b_arr,
                         (sizeof(uint64_t) * lt_info->num_1b_chunks),
                         "Alloc Cont_1_byte");
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(
                        udf_tmpl,
                        udf_tbls_info[unit]->udf_cont_1_byte_fid,
                        0, cont_1b_arr, lt_info->num_1b_chunks,
                        &act_1b_cnt));
        if (act_1b_cnt != lt_info->num_1b_chunks) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
    }

    if (lt_info->num_2b_chunks) {
        BCMINT_UDF_MEMALLOC(cont_2b_arr,
                         (sizeof(uint64_t) * lt_info->num_2b_chunks),
                         "Alloc Cont_2_byte");
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(
                        udf_tmpl,
                        udf_tbls_info[unit]->udf_cont_2_byte_fid,
                        0, cont_2b_arr, lt_info->num_2b_chunks,
                        &act_2b_cnt));
        if (act_2b_cnt != lt_info->num_2b_chunks) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
    }

    if (lt_info->num_4b_chunks) {
        BCMINT_UDF_MEMALLOC(cont_4b_arr,
                         (sizeof(uint64_t) * lt_info->num_4b_chunks),
                         "Alloc Cont_4_byte");
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(
                        udf_tmpl,
                        udf_tbls_info[unit]->udf_cont_4_byte_fid,
                        0, cont_4b_arr, lt_info->num_4b_chunks,
                        &act_4b_cnt));
        if (act_4b_cnt != lt_info->num_4b_chunks) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
    }

    for (c = 0; c < act_1b_cnt; c++) {
        if ((cont_1b_arr != NULL) && (cont_1b_arr[c] == 1)) {
            BCM_UDF_CBMP_CHUNK_ADD(info->one_byte_chunk_bmap, c);
            width += 1;
            if (first_1b_cont == -1) {
                first_1b_cont = c;
            }
        }
    }

    for (c = 0; c < act_2b_cnt; c++) {
        if ((cont_2b_arr != NULL) && (cont_2b_arr[c] == 1)) {
            BCM_UDF_CBMP_CHUNK_ADD(info->two_byte_chunk_bmap, c);
            width += 2;
            if (first_2b_cont == -1) {
                first_2b_cont = c;
            }
        }
    }

    for (c = 0; c < act_4b_cnt; c++) {
        if ((cont_4b_arr != NULL) && (cont_4b_arr[c] == 1)) {
            BCM_UDF_CBMP_CHUNK_ADD(info->four_byte_chunk_bmap, c);
            width += 4;
            if (first_4b_cont == -1) {
                first_4b_cont = c;
            }
        }
    }

    /* update width */
    info->width = width;

    /* update flags */
    if (flags) {
        *flags = lt_info->flags;
    }

exit:
    if (udf_tmpl != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(udf_tmpl);
    }

    if (cont_1b_arr != NULL) {
        BCMINT_UDF_MEMFREE(cont_1b_arr);
    }
    if (cont_2b_arr != NULL) {
        BCMINT_UDF_MEMFREE(cont_2b_arr);
    }
    if (cont_4b_arr != NULL) {
        BCMINT_UDF_MEMFREE(cont_4b_arr);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_udf_destroy(
    int unit,
    bcm_udf_id_t udf_id)
{
    uint64_t udf_id_64 = udf_id;
    bcmlt_entry_handle_t udf_tmpl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* UDF Policy object delete operation */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_entry_commit(unit, &udf_tmpl,
                                  BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL,
                                  udf_tbls_info[unit]->udf_sid,
                                  udf_tbls_info[unit]->udf_key_fid,
                                  0, udf_id_64, NULL,
                                  NULL,
                                  0, 0, NULL,
                                  1));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_udf_is_object_exist(
    int unit,
    bcm_udf_id_t udf_id)
{
    uint64_t udf_id_64 = udf_id;
    bcmlt_entry_handle_t udf_tmpl = BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    /* Allocate and Read UDF Policy Info template */
    SHR_IF_ERR_EXIT
        (ltsw_udf_lt_entry_commit(unit, &udf_tmpl,
                                  BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL,
                                  udf_tbls_info[unit]->udf_sid,
                                  udf_tbls_info[unit]->udf_key_fid,
                                  0, udf_id_64, NULL,
                                  NULL,
                                  0, 0, NULL,
                                  1));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_udf_object_list_get(
    int unit,
    int max,
    bcm_udf_id_t *udf_obj_list,
    int *actual)
{
    int rv;
    int dunit;
    int ent_cnt = 0;
    int obj_cnt = 0;
    uint64_t udf_id = 0;
    bcmlt_entry_handle_t udf_tmpl = BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Entry handle allocate for given template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, udf_tbls_info[unit]->udf_sid, &udf_tmpl));

    /*
     * Traverse UDF policy table and retrieve the UDF objects created
     * the retrieved LT entries.
     */
    while ((rv = bcmlt_entry_commit(udf_tmpl,
                                    BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        /* Increment traversed UDF entry count. */
        ent_cnt++;
        if (max < ent_cnt) {
            break;
        }

        /* Retrieve the UDF Policy index */
        SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_field_get(udf_tmpl,
                                    udf_tbls_info[unit]->udf_key_fid,
                                    &udf_id));

        udf_obj_list[obj_cnt++] = udf_id;
    }

    if ((obj_cnt == 0) && SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    *actual = obj_cnt;

exit:
    if (udf_tmpl != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(udf_tmpl);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_udf_multi_pkt_fmt_info_get(
    int unit,
    bcm_udf_abstract_pkt_format_t pkt_fmt,
    bcm_udf_multi_abstract_pkt_format_info_t *info)
{
    int c;
    bcmint_udf_abstr_pkt_fmt_ltinfo_t *lt_info = NULL;
    SHR_FUNC_ENTER(unit);

    lt_info = ltsw_udf_abstr_pkt_fmt_ltinfo[unit][pkt_fmt];
    if (lt_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    info->base_offset = 0;
    info->num_chunks_max = lt_info->num_avail_chunks;

    for (c = 0; c < BCMINT_UDF_MAX_CHUNKS(unit); c++) {
        if (lt_info->avail_4b_cbmap & (1 << c)) {
            BCM_UDF_CBMP_CHUNK_ADD(info->chunk_4b_bmap_avail, c);
        }
        if (lt_info->avail_2b_cbmap & (1 << c)) {
            BCM_UDF_CBMP_CHUNK_ADD(info->chunk_2b_bmap_avail, c);
        }
        if (lt_info->avail_1b_cbmap & (1 << c)) {
            BCM_UDF_CBMP_CHUNK_ADD(info->chunk_1b_bmap_avail, c);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_udf_abstr_pkt_fmt_object_list_get(
    int unit,
    bcm_udf_abstract_pkt_format_t pkt_fmt,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual)
{
    int dunit;
    int obj_cnt = 0;
    uint64_t udf_id = 0;
    const char *pkt_hdr = NULL;
    bcmint_udf_abstr_pkt_fmt_ltinfo_t *lt_info = NULL;
    bcmlt_entry_handle_t udf_tmpl = BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    if ((udf_id_list == NULL) || (actual == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_info = ltsw_udf_abstr_pkt_fmt_ltinfo[unit][pkt_fmt];
    if (lt_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Entry handle allocate for given template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, udf_tbls_info[unit]->udf_sid, &udf_tmpl));

    *actual = 0;
    /*
     * Traverse UDF policy table and retrieve the UDF objects created
     * the retrieved LT entries.
     */
    while ((bcmlt_entry_commit(udf_tmpl,
                               BCMLT_OPCODE_TRAVERSE,
                               BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        /* Retrieve the UDF Policy index */
        SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_field_get(udf_tmpl,
                                    udf_tbls_info[unit]->udf_key_fid,
                                    &udf_id));
        SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_field_symbol_get(udf_tmpl,
                                      udf_tbls_info[unit]->udf_anchor_fid,
                                      &pkt_hdr));
        if (sal_strcmp(pkt_hdr, lt_info->pkt_header) == 0) {
            obj_cnt++;
            if (max < obj_cnt) {
                break;
            }
            udf_id_list[*actual] = udf_id;
            *actual += 1;
        }
    }

exit:
    if (udf_tmpl != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(udf_tmpl);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_udf_abstr_pkt_fmt_list_get(
    int unit,
    int max,
    bcm_udf_abstract_pkt_format_t *pkt_fmt_list,
    int *actual)
{
    int cnt = 0;
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    bcmint_udf_abstr_pkt_fmt_ltinfo_t *lt_info = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pkt_fmt_list, SHR_E_PARAM);
    SHR_NULL_CHECK(actual, SHR_E_PARAM);

    for (pkt_fmt = 0; pkt_fmt < bcmUdfAbstractPktFormatLastCount; pkt_fmt++) {
         lt_info = ltsw_udf_abstr_pkt_fmt_ltinfo[unit][pkt_fmt];
         if (lt_info == NULL) {
             continue;
         }

         pkt_fmt_list[cnt++] = pkt_fmt;
         if (cnt > max) {
             break;
         }
   }

   *actual = cnt;
exit:
    SHR_FUNC_EXIT();
}

