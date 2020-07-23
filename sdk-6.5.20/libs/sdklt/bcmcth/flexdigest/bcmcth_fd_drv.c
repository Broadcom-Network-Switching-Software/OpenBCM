/*! \file bcmcth_fd_drv.c
 *
 * BCMCTH Flex Digest Driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <shr/shr_ha.h>


#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_conf.h>

#include <bcmcth/bcmcth_flex_digest_drv.h>

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCMCTH_FLEXDIGEST

#define NUM_BINS_PER_SET   16
#define NUM_SETS           3
#define NUM_BINS (NUM_BINS_PER_SET * NUM_SETS)
#define NUM_TCAM_BUF 2
#define NUM_SRAM_BUF 4

#define BCMCTH_FD_ALLOC(_ptr, _sz, _str)                   \
            do                                         \
            {                                          \
                SHR_ALLOC(_ptr, _sz, _str);            \
                SHR_NULL_CHECK(_ptr, SHR_E_MEMORY);    \
                sal_memset(_ptr, 0, _sz);              \
            } while (0)


#define LFIELD_VALUE_GET(_data, _fid, _val) \
    do { \
        bcmltd_field_t *_tmp_data; \
        _tmp_data = (bcmltd_field_t *)(_data); \
        _val = 0; \
        while (_tmp_data) { \
            if (_tmp_data->id == _fid) { \
                _val = _tmp_data->data; \
                break; \
            } \
            _tmp_data = _tmp_data->next; \
        } \
    } while (0)

/* link all bcmltd_fields */
#define LFIELDS_LINK(_flds) \
    do { \
        uint32_t _ix; \
        for (_ix = 0; _ix < (_flds).count; _ix++) { \
            if (_ix == (_flds).count - 1) { \
                (_flds).field[_ix]->next = NULL; \
            } else { \
                (_flds).field[_ix]->next = (_flds).field[_ix + 1]; \
            } \
        } \
    } while (0)

/*! Device specific attach function type. */
typedef bcmcth_flex_digest_drv_t *(*bcmcth_flex_digest_drv_get_f)(int unit);

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_flex_digest_drv_get },
static struct {
    bcmcth_flex_digest_drv_get_f drv_get;
} flex_digest_drv_get[] = {
    { 0 },
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }
};

typedef struct fd_fields_table_s {
    uint32_t *field;
    int size;
} fd_fields_table_t;

/* FLEX_DIGEST_LKUP LT fields derived from FLEX_DIGEST_LKUP_PRESEL LT */
static fd_fields_table_t *fd_derived_fields[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

static bcmcth_flex_digest_drv_t *fd_drv[BCMDRD_CONFIG_MAX_UNITS];

static int fd_field_update (int unit, bcmltd_field_t *new_fld,
                             bcmltd_fields_t * ext_fld);
static int fd_derived_fields_get (int unit, bcmltd_sid_t lsid, int *size,
                                 uint32_t *buf);
static int fd_lkup_lentry_process (int unit, bcmltd_sid_t sid,
                  uint32_t trans_id, bcmimm_entry_event_t entry_event,
                  const bcmltd_field_t *key, const bcmltd_field_t *data,
                  bcmcth_flex_digest_variant_field_id_t *desc,
                  bcmltd_fields_t *output_fields);

/*******************************************************************************
* Public functions
 */
/*
 * Flex Digest driver initialization routine.
 */
int
bcmcth_flex_digest_drv_init(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;
    int size;
    int tbl_size;
    uint32_t *buf = NULL;
    fd_fields_table_t *fld_tbl;
    bcmlrd_variant_t variant;
    bcmcth_flex_digest_variant_field_id_t *desc;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    variant = bcmlrd_variant_get(unit);

    /* Perform device-specific software setup */
    fd_drv[unit] = flex_digest_drv_get[variant].drv_get(unit);

    if (fd_drv[unit] == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcmcth_flex_digest_drv_variant_field_get(unit,&desc));
    SHR_IF_ERR_EXIT(bcmcth_flex_digest_imm_register(unit, (void *)desc));

    if (fd_drv[unit] && fd_drv[unit]->drv_init) {
        SHR_IF_ERR_EXIT(fd_drv[unit]->drv_init(unit, warm));
    } else {
        SHR_EXIT();
    }

    /* first find the size of FLEX_DIGEST_LKUPt */
    tbl_size = 0;
    SHR_IF_ERR_EXIT(fd_derived_fields_get(unit, desc->lkup_sid, &tbl_size, NULL));
    size = (sizeof(uint32_t) * tbl_size) + sizeof(fd_fields_table_t);
    SHR_ALLOC(buf, size, "bcmCthFlexDigestDevDescBuf");
    SHR_NULL_CHECK(buf, SHR_E_MEMORY);

    fld_tbl        = (fd_fields_table_t *)buf;
    fld_tbl->field = (uint32_t *)(fld_tbl + 1);
    fld_tbl->size  = tbl_size;
    SHR_IF_ERR_EXIT(fd_derived_fields_get(unit, desc->lkup_sid, &fld_tbl->size,
         fld_tbl->field));
    fd_derived_fields[unit] = fld_tbl;
    SHR_FUNC_EXIT();

exit:
    if (buf != NULL) {
        SHR_FREE(buf);
    }
    SHR_FUNC_EXIT();
}

/*
 * Flex Digest driver cleanup routine.
 */
int
bcmcth_flex_digest_drv_cleanup(int unit)
{
    bcmdrd_dev_type_t dev_type;
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (fd_derived_fields[unit] != NULL) {;
        SHR_FREE(fd_derived_fields[unit]);
        fd_derived_fields[unit] = NULL;
    }

    /* Perform device-specific software cleanup */
    variant = bcmlrd_variant_get(unit);
    fd_drv[unit] = flex_digest_drv_get[variant].drv_get(unit);

    if (fd_drv[unit] && fd_drv[unit]->drv_cleanup) {
        SHR_IF_ERR_EXIT(fd_drv[unit]->drv_cleanup(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* Private functions
 */

/*
 * Allocate a single chunk of buffer for number of bcmltd_field_t fields and
 * populate the bcmltd_fields_t out field. Buffer should be freed with the
 * buffer pointer out->field.
 */
static int fd_field_array_alloc(int unit, size_t num_fids,
               bcmltd_fields_t *out)
{
    bcmltd_field_t *fld_buf;
    uint32_t i;

    SHR_FUNC_ENTER(unit);
    out->count = num_fids;
    out->field = NULL;

    BCMCTH_FD_ALLOC(out->field,
            num_fids * (sizeof(bcmltd_field_t *) + sizeof (bcmltd_field_t)),
             "bcmCthLbhLtFieldArray");
    fld_buf = (bcmltd_field_t *)(out->field + num_fids);
    for (i = 0; i < num_fids; i++) {
        out->field[i] = fld_buf + i;
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Retrieve number of the hardware information entries associated with the
 * given logical table ID.
 */
static int
fd_hentry_info_get(int unit,
    bcmltd_sid_t lsid,
    int *size,
    bcmcth_flex_digest_hentry_info_t *info)
{
    if (fd_drv[unit] && fd_drv[unit]->hentry_info_get) {
        return fd_drv[unit]->hentry_info_get(unit, lsid, size, info);
    }

    return SHR_E_UNAVAIL;
}

/*
 * Program one hardware memory entry buffer with a link list of logical
 *  fields(in parameter).
 */
static int
fd_entry_set(int unit,
    bcmltd_sid_t lsid,
    bcmcth_flex_digest_entry_info_t *info,
    bcmltd_field_t *in)
{
    if (fd_drv[unit] && fd_drv[unit]->entry_set) {
        return fd_drv[unit]->entry_set(unit, lsid, info, in);
    }

    return SHR_E_UNAVAIL;
}

/*
 * Validate a link list of logical fields.
 */
static int
fd_entry_validate(int unit,
    bcmltd_sid_t lsid,
    bcmcth_flex_digest_entry_info_t *info,
    bcmltd_field_t *in)
{
    if (fd_drv[unit] && fd_drv[unit]->entry_validate) {
        return fd_drv[unit]->entry_validate(unit, lsid, info, in);
    }

    return SHR_E_UNAVAIL;
}

static int
fd_derived_fields_get(int unit,
    bcmltd_sid_t lsid,
    int *size,
    uint32_t *buf)
{
    if (fd_drv[unit] && fd_drv[unit]->derived_fields_get) {
        return fd_drv[unit]->derived_fields_get(unit, lsid, size, buf);
    }

    return SHR_E_UNAVAIL;
}

int
bcmcth_flex_digest_drv_variant_field_get(int unit,
    bcmcth_flex_digest_variant_field_id_t **desc)
{
    if (fd_drv[unit] && fd_drv[unit]->variant_id_get) {
        return fd_drv[unit]->variant_id_get(unit, desc);
    }

    return SHR_E_UNAVAIL;
}

/*
 * Validate the logical fields for FLEX_DIGEST_LKUP logical table.
 */
static int fd_lkup_entry_validate (int unit,
    bcmimm_entry_event_t entry_event,
    bcmltd_field_t *key,
    bcmltd_field_t *data,
    bcmcth_flex_digest_variant_field_id_t *desc)
{
    bcmcth_flex_digest_entry_info_t info;
    int rv;
    uint32_t fid;
    uint64_t val64;
    bcmltd_fields_t in;
    bcmltd_field_t in_fld;
    bcmltd_field_t in_fld1;
    bcmltd_field_t *in_ptr_arr[2];
    size_t num_fids;
    bcmltd_fields_t out_flds;
    bcmltd_field_t *fld_data;

    SHR_FUNC_ENTER(unit);

    out_flds.field = NULL;
    if (!(entry_event == BCMIMM_ENTRY_INSERT ||
          entry_event == BCMIMM_ENTRY_UPDATE)) {
        SHR_EXIT();
    }

    fid = desc->lkup_id;  /*FLEX_DIGEST_LKUPt_FLEX_DIGEST_LKUP_IDf; */
    LFIELD_VALUE_GET(key, fid, val64);
    in_fld.id     = fid;
    in_fld.data   = val64;

    fid = desc->group_pri_id; /* FLEX_DIGEST_LKUPt_GROUP_PRIORITYf; */
    LFIELD_VALUE_GET(key, fid, val64);
    in_fld1.id     = fid;
    in_fld1.data   = val64;

    if (entry_event == BCMIMM_ENTRY_UPDATE) {
        in.count      = 2;
        in.field      = in_ptr_arr;
        in_ptr_arr[0] = &in_fld;
        in_ptr_arr[1] = &in_fld1;

        /* the function counts array as one field */
        SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, desc->lkup_sid, &num_fids));
        num_fids += 64; /* plus 4x16 array elements */

        SHR_IF_ERR_EXIT(fd_field_array_alloc(unit, num_fids, &out_flds));
        SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit, desc->lkup_sid, &in, &out_flds));

        /* update the existing fields with the new ones */
        SHR_IF_ERR_EXIT(fd_field_update (unit, (bcmltd_field_t *)data,
                             &out_flds));

        /* link all bcmltd_fields */
        LFIELDS_LINK(out_flds);
        if (out_flds.count) {
            fld_data = out_flds.field[0];
        } else {
            fld_data = NULL;
        }
    } else {
        fld_data = (bcmltd_field_t *)data;
    }

    if (fld_data != NULL) {
        info.type    = BCMCTH_FLEX_DIGEST_ENTRY_TYPE_KEY;
        info.tbl     = NULL;
        info.size    = 0;
        rv = fd_entry_validate(unit, desc->lkup_sid, &info, fld_data);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (out_flds.field != NULL) {
        SHR_FREE(out_flds.field);
    }
    SHR_FUNC_EXIT();
}

/*
 * Update the existing field list(parameter ext_fld) with the new list
 * of the logical fields(parameter new_fld).
 */
static int fd_field_update (int unit, bcmltd_field_t *new_fld,
                             bcmltd_fields_t * ext_fld)
{
    uint32_t i;

    /* update the existing fields with the new ones */
    while (new_fld) {
        for (i = 0; i < ext_fld->count; i++) {
            if ((new_fld->id == ext_fld->field[i]->id) &&
                (new_fld->idx == ext_fld->field[i]->idx)) {
                ext_fld->field[i]->data = new_fld->data;
                break;
            }
        }
        /* not found, new field to add */
        if (i == ext_fld->count) {
            ext_fld->field[ext_fld->count]->id  = new_fld->id;
            ext_fld->field[ext_fld->count]->idx = new_fld->idx;
            ext_fld->field[ext_fld->count]->data = new_fld->data;
            ext_fld->field[ext_fld->count]->next = NULL;
            ext_fld->count++;
        }
        new_fld = new_fld->next;
    }
    return SHR_E_NONE;
}

/*
 * Traverse the FLEX_DIGEST_LKUP table to find entries containing fields derived
 * from FLEX_DIGEST_LKUP_PRESEL table and rewrite to memory to sync up the
 * presel format. Should be called from presel function whenever a field is
 * added.
 */

static int fd_lkup_lentry_traverse (int unit,
                  uint32_t trans_id,
                  bcmcth_flex_digest_variant_field_id_t *desc)
{
    bcmltd_field_t in_fld;
    bcmltd_field_t in_fld1;
    bcmltd_field_t *in_ptr_arr[2];
    bcmltd_fields_t in;
    uint32_t ix, iy;
    uint32_t jx;
    int jy;
    fd_fields_table_t *tbl;
    bcmltd_fields_t out_flds;
    bcmltd_sid_t l_sid;
    size_t num_fids;
    int rv;
    int found;
    uint32_t max_id;
    uint32_t max_group;
    uint64_t def_val_max[1];
    uint32_t num;

    SHR_FUNC_ENTER(unit);

    tbl = fd_derived_fields[unit];
    out_flds.field = NULL;

    /* FLEX_DIGEST_LKUPt */
    l_sid = desc->lkup_sid;

    SHR_IF_ERR_EXIT(bcmlrd_field_max_get(unit, l_sid, desc->lkup_id,
                              1, def_val_max, &num));
    max_id = (uint32_t)def_val_max[0];

    SHR_IF_ERR_EXIT(bcmlrd_field_max_get(unit, l_sid, desc->group_pri_id,
                              1, def_val_max, &num));
    max_group = (uint32_t)def_val_max[0];

    /* the function counts array as one field */
    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, l_sid, &num_fids));
    num_fids += 64; /* plus 4x16 array elements */

    SHR_IF_ERR_EXIT(fd_field_array_alloc(unit, num_fids, &out_flds));
    in_fld.id     = desc->lkup_id; /*FLEX_DIGEST_LKUPt_FLEX_DIGEST_LKUP_IDf; */
    in_fld1.id    = desc->group_pri_id; /* FLEX_DIGEST_LKUPt_GROUP_PRIORITYf; */
    in_fld.next = &in_fld1;
    in_fld1.next = NULL;

    for (ix = 0; ix < (max_group + 1); ix++) {
        for (iy = 0; iy < (max_id + 1); iy++) {
            in_fld.data    = iy;
            in_fld1.data   = ix;

            in.count      = 2;
            in.field      = in_ptr_arr;
            in_ptr_arr[0] = &in_fld;
            in_ptr_arr[1] = &in_fld1;

            out_flds.count = num_fids;
            rv = bcmimm_entry_lookup(unit, l_sid, &in, &out_flds);
            if (rv == SHR_E_NOT_FOUND) {
                continue;
            } else {
                SHR_IF_ERR_EXIT(rv);
            }
            /* check if entry contains presel derived fields */
            found = FALSE;
            for (jx = 0; jx < out_flds.count; jx++) {
                for (jy = 0; jy < tbl->size; jy++) {
                    if (tbl->field[jy] == out_flds.field[jx]->id) {
                        found = TRUE;
                        break;
                    }
                }
                if (found == TRUE) {
                    break;
                }
            }

            if (found == TRUE) {
                SHR_IF_ERR_EXIT(fd_lkup_lentry_process (unit, l_sid, trans_id,
                    BCMIMM_ENTRY_UPDATE, &in_fld, NULL, desc, NULL));
            }
        }
    }
exit:
    if (out_flds.field != NULL) {
        SHR_FREE(out_flds.field);
    }
    SHR_FUNC_EXIT();

}

/*
 * Process the logical table FLEX_DIGEST_LKUP_PRESEL entry and write to
 * the memory.
 */
static int fd_presel_lentry_process (int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  bcmimm_entry_event_t entry_event,
                  const bcmltd_field_t *key,
                  const bcmltd_field_t *data,
                  bcmcth_flex_digest_variant_field_id_t *desc,
                  bcmltd_fields_t *output_fields)
{
    uint32_t entry[BCMPTM_MAX_PT_FIELD_WORDS];
    int rv;
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    bcmdrd_sid_t pt_id;
    bcmcth_flex_digest_hentry_info_t hinfo;
    bcmcth_flex_digest_entry_info_t info;
    int size;
    uint32_t *entry_tbl[1];
    bcmltd_field_t *in_ptr_arr[1];
    bcmltd_fields_t in;
    bcmltd_field_t in_fld;
    size_t num_fids;
    bcmltd_fields_t out_flds;
    bcmltd_field_t *fld_data;
    bcmptm_op_type_t req_op;

    SHR_FUNC_ENTER(unit);

    out_flds.field = NULL;
    sal_memset(entry, 0, sizeof(entry));
    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));

    pt_info.index = 0;

    if (entry_event == BCMIMM_ENTRY_UPDATE) {
        in.count      = 0;
        in.field      = in_ptr_arr;
        in_ptr_arr[0] = &in_fld;
        in_fld.id     = 0;
        in_fld.data   = 0;

        SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, sid, &num_fids));
        SHR_IF_ERR_EXIT(fd_field_array_alloc(unit, num_fids, &out_flds));
        SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit, sid, &in, &out_flds));

        /* update the existing fields with the new ones */
        SHR_IF_ERR_EXIT(fd_field_update (unit, (bcmltd_field_t *)data,
                             &out_flds));

        /* link all bcmltd_fields */
        LFIELDS_LINK(out_flds);
        if (out_flds.count) {
            fld_data = out_flds.field[0];
        } else {
            fld_data = NULL;
        }
    } else {
        fld_data = (bcmltd_field_t *)data;
    }

    info.type = BCMCTH_FLEX_DIGEST_ENTRY_TYPE_DATA;
    info.tbl  = entry_tbl;
    entry_tbl[0] = entry;
    info.size    = 1;
    rv = fd_entry_set(unit, sid, &info, fld_data);
    SHR_IF_ERR_EXIT(rv);

    size = 1;
    rv = fd_hentry_info_get(unit, sid, &size, &hinfo);
    SHR_IF_ERR_EXIT(rv);

    pt_id = hinfo.sid;
    if (entry_event == BCMIMM_ENTRY_INSERT) {
        req_op = BCMPTM_OP_WRITE;
    } else if (entry_event == BCMIMM_ENTRY_UPDATE) {
        req_op = BCMPTM_OP_WRITE;
    } else if (entry_event == BCMIMM_ENTRY_DELETE) {
        req_op = BCMPTM_OP_WRITE;
    } else {
         SHR_EXIT();
    }

    if (entry_event == BCMIMM_ENTRY_UPDATE ||
        entry_event == BCMIMM_ENTRY_INSERT) {
        /* turn on the atomic mode, multiple tcam entries may be updated */
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_mreq_atomic_trans_enable(unit));
    }

    SHR_IF_ERR_EXIT(bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_id,
                                    &pt_info,
                                    NULL,
                                    NULL,
                                    req_op,
                                    (uint32_t *)&entry,
                                    0,
                                    sid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

    if (entry_event == BCMIMM_ENTRY_UPDATE ||
        entry_event == BCMIMM_ENTRY_INSERT) {
        SHR_IF_ERR_EXIT(fd_lkup_lentry_traverse (unit, trans_id, desc));
    }

exit:
    if (out_flds.field != NULL) {
        SHR_FREE(out_flds.field);
    }
    SHR_FUNC_EXIT();
}

/*
 * Process the logical table FLEX_DIGEST_HASH_SALT entry and write to
 * the memory.
 */
static int fd_hash_salt_lentry_process (int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  bcmimm_entry_event_t entry_event,
                  const bcmltd_field_t *key,
                  const bcmltd_field_t *data,
                  void *context,
                  bcmltd_fields_t *output_fields)
{
    uint32_t *entry;
    int rv;
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    bcmdrd_sid_t pt_id;
    bcmcth_flex_digest_hentry_info_t *hinfo = NULL;
    bcmcth_flex_digest_entry_info_t info;
    int size;
    uint32_t **entry_tbl = NULL;
    bcmltd_field_t *in_ptr_arr[1];
    bcmltd_fields_t in;
    bcmltd_field_t in_fld;
    size_t num_fids;
    bcmltd_fields_t out_flds;
    bcmltd_field_t *fld_data;
    bcmptm_op_type_t req_op;
    SHR_BITDCL updat_flag[SHRi_BITDCLSIZE(NUM_BINS)];
    int i;
    bcmltd_field_t *in_data;

    SHR_FUNC_ENTER(unit);

    out_flds.field = NULL;
    in_data = (bcmltd_field_t *)data;
    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    sal_memset(updat_flag, 0, sizeof(updat_flag));

    pt_info.index = 0;

    /* memory cleared within the alloc */
    size = NUM_BINS * (sizeof(uint32_t *) + sizeof(uint32_t));
    BCMCTH_FD_ALLOC(entry_tbl, size,  "bcmCthFlexDigestHashSaltTmpBuf");
    entry = (uint32_t *)(entry_tbl + NUM_BINS);

    for (i = 0; i < NUM_BINS; i++) {
        entry_tbl[i] = entry + i;
    }

    size = NUM_BINS * sizeof(bcmcth_flex_digest_hentry_info_t);
    BCMCTH_FD_ALLOC(hinfo, size,  "bcmCthFlexDigestHashSaltTmpBuf");

    if (entry_event == BCMIMM_ENTRY_UPDATE) {
        in.count      = 0;
        in.field      = in_ptr_arr;
        in_ptr_arr[0] = &in_fld;
        in_fld.id     = 0;
        in_fld.data   = 0;

        SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, sid, &num_fids));
        num_fids += NUM_BINS; /* plus 3x16 array elements */
        SHR_IF_ERR_EXIT(fd_field_array_alloc(unit, num_fids, &out_flds));
        SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit, sid, &in, &out_flds));

        /* update the existing fields with the new ones */
        SHR_IF_ERR_EXIT(fd_field_update (unit, (bcmltd_field_t *)data,
                             &out_flds));

        /* link all bcmltd_fields */
        LFIELDS_LINK(out_flds);
        if (out_flds.count) {
            fld_data = out_flds.field[0];
        } else {
            fld_data = NULL;
        }
    } else {
        fld_data = (bcmltd_field_t *)data;
    }

    info.type = BCMCTH_FLEX_DIGEST_ENTRY_TYPE_DATA;
    info.tbl  = entry_tbl;
    info.size = NUM_BINS;
    rv = fd_entry_set(unit, sid, &info, fld_data);
    SHR_IF_ERR_EXIT(rv);

    size = NUM_BINS;
    rv = fd_hentry_info_get(unit, sid, &size, hinfo);
    SHR_IF_ERR_EXIT(rv);

    /* initialize to update all fields by default */
    for (i = 0; i < NUM_BINS; i++) {
        SHR_BITSET(updat_flag, i);
    }
    if (entry_event == BCMIMM_ENTRY_INSERT) {
        req_op = BCMPTM_OP_WRITE;
    } else if (entry_event == BCMIMM_ENTRY_UPDATE) {
        req_op = BCMPTM_OP_WRITE;

        for (i = 0; i < NUM_BINS; i++) {
            SHR_BITCLR(updat_flag, i);
        }

        /* only update the requested fields */
        while (in_data) {
            if (in_data->id == FLEX_DIGEST_HASH_SALTt_BIN_Af) {
                SHR_BITSET(updat_flag, in_data->idx);
            } else if (in_data->id == FLEX_DIGEST_HASH_SALTt_BIN_Bf) {
                SHR_BITSET(updat_flag, NUM_BINS_PER_SET + in_data->idx);
            } else if (in_data->id == FLEX_DIGEST_HASH_SALTt_BIN_Cf) {
                SHR_BITSET(updat_flag, 2 * NUM_BINS_PER_SET + in_data->idx);
            }
            in_data = in_data->next;
        }
    } else if (entry_event == BCMIMM_ENTRY_DELETE) {
        req_op = BCMPTM_OP_WRITE;
    } else {
         SHR_EXIT();
    }

    for (i = 0; i < NUM_BINS; i++) {
        if (!SHR_BITGET(updat_flag, i)) {
            continue;
        }
        pt_id = hinfo[i].sid;
        SHR_IF_ERR_EXIT(bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_id,
                                    &pt_info,
                                    NULL,
                                    NULL,
                                    req_op,
                                    entry_tbl[i],
                                    0,
                                    sid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));
    }

exit:
    if (entry_tbl != NULL) {
        SHR_FREE(entry_tbl);
    }
    if (hinfo != NULL) {
        SHR_FREE(hinfo);
    }
    if (out_flds.field != NULL) {
        SHR_FREE(out_flds.field);
    }
    SHR_FUNC_EXIT();
}
/*
 * Process the FLEX_DIGEST_LKUP logical table entry and write to the memory.
 */
static int fd_lkup_lentry_process (int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  bcmimm_entry_event_t entry_event,
                  const bcmltd_field_t *key,
                  const bcmltd_field_t *data,
                  bcmcth_flex_digest_variant_field_id_t *desc,
                  bcmltd_fields_t *output_fields)
{
    uint32_t *key_entry;
    uint32_t *data_entry0;
    uint32_t *data_entry1;
    uint32_t *ret_key_entry;
    uint32_t *ret_data_entry0;
    uint32_t *ret_data_entry1;
    int rv;
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint32_t    **ekw;
    uint32_t    **edw;
    uint32_t    **rsp_ekw;
    uint32_t    **rsp_edw;
    uint32_t    wsize = 0;
    uint64_t    req_flags = 0;
    uint32_t    fid;
    bcmptm_rm_hash_entry_attrs_t hash_attrs;
    bcmptm_keyed_lt_mreq_info_t lt_mreq_info;
    bcmptm_keyed_lt_mrsp_info_t lt_mrsp_info;
    bcmltd_fields_t out;
    bcmltd_fields_t out_flds;
    bcmltd_sid_t l_sid;
    bcmltd_field_t in_fld;
    bcmltd_field_t in_fld1;
    bcmltd_field_t *in_ptr_arr[2];
    bcmltd_fields_t in;
    uint64_t val64;
    uint32_t *entry_tbl[2];
    size_t num_fids;
    bcmltd_field_t *entry_data;
    bcmptm_rm_tcam_entry_attrs_t *tcam_attrs;
    int size;
    void *buf_ptr = NULL;
    bcmptm_op_type_t req_op;
    bcmcth_flex_digest_entry_info_t info;
    uint32_t id_max;
    uint64_t def_val_max[1];
    uint32_t num;
    bcmptm_pt_op_info_t pt_op_info;

    SHR_FUNC_ENTER(unit);

    out_flds.field = NULL;
    out.field      = NULL;

    size  = NUM_TCAM_BUF * BCMPTM_MAX_PT_FIELD_WORDS * sizeof(uint32_t);
    size += NUM_SRAM_BUF * BCMPTM_MAX_PT_FIELD_WORDS * sizeof(uint32_t);
    size += NUM_TCAM_BUF * sizeof(uint32_t *);
    size += NUM_SRAM_BUF * sizeof(uint32_t *);
    size += sizeof(bcmptm_rm_tcam_entry_attrs_t);

    /* memory cleared within the alloc */
    BCMCTH_FD_ALLOC(buf_ptr, size,  "bcmCthLbhTmpTcamBuf");

    /* partition the allocated buf */
    key_entry     = (uint32_t *)buf_ptr;
    ret_key_entry = key_entry + BCMPTM_MAX_PT_FIELD_WORDS;

    data_entry0     = ret_key_entry   + BCMPTM_MAX_PT_FIELD_WORDS;
    data_entry1     = data_entry0     + BCMPTM_MAX_PT_FIELD_WORDS;
    ret_data_entry0 = data_entry1     + BCMPTM_MAX_PT_FIELD_WORDS;
    ret_data_entry1 = ret_data_entry0 + BCMPTM_MAX_PT_FIELD_WORDS;

    ekw           = (uint32_t **)(ret_data_entry1 + BCMPTM_MAX_PT_FIELD_WORDS);
    rsp_ekw       = ekw + 1;
    edw           = rsp_ekw + 1;
    rsp_edw       = edw + 2;
    tcam_attrs    = (bcmptm_rm_tcam_entry_attrs_t *)(rsp_edw + 2);

    (void)sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    (void)sal_memset(&hash_attrs, 0, sizeof(bcmptm_rm_hash_entry_attrs_t));
    (void)sal_memset(&lt_mreq_info, 0, sizeof(bcmptm_keyed_lt_mreq_info_t));
    (void)sal_memset(&lt_mrsp_info, 0, sizeof(bcmptm_keyed_lt_mrsp_info_t));
    (void)sal_memset(tcam_attrs,   0, sizeof(bcmptm_rm_tcam_entry_attrs_t));
    (void)sal_memset(&pt_op_info,   0, sizeof(bcmptm_pt_op_info_t));

    fid = desc->lkup_id; /* FLEX_DIGEST_LKUPt_FLEX_DIGEST_LKUP_IDf; */
    LFIELD_VALUE_GET(key, fid, val64);
    lt_mreq_info.entry_id = (uint32_t)val64;
    in_fld.id     = fid;
    in_fld.data   = val64;

    SHR_IF_ERR_EXIT(bcmlrd_field_max_get(unit, desc->lkup_sid, fid,
                              1, def_val_max, &num));
    id_max = (uint32_t)def_val_max[0];

    fid = desc->group_pri_id; /*FLEX_DIGEST_LKUPt_GROUP_PRIORITYf; */
    LFIELD_VALUE_GET(key, fid, val64);
    tcam_attrs->slice_id = (uint8_t)val64;
    lt_mreq_info.entry_id += ((uint32_t)val64) * (id_max + 1);
    in_fld1.id     = fid;
    in_fld1.data   = val64;

    if (entry_event == BCMIMM_ENTRY_UPDATE) {
        in.count      = 2;
        in.field      = in_ptr_arr;
        in_ptr_arr[0] = &in_fld;
        in_ptr_arr[1] = &in_fld1;
        l_sid = desc->lkup_sid;  /* FLEX_DIGEST_LKUPt; */

        /* the function counts array as one field */
        SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, l_sid, &num_fids));
        num_fids += 64; /* plus 4x16 array elements */

        SHR_IF_ERR_EXIT(fd_field_array_alloc(unit, num_fids, &out_flds));
        SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit, l_sid, &in, &out_flds));

        /* update the existing fields with the new ones */
        SHR_IF_ERR_EXIT(fd_field_update (unit, (bcmltd_field_t *)data,
                             &out_flds));

        /* link all bcmltd_fields */
        LFIELDS_LINK(out_flds);
        if (out_flds.count) {
            entry_data = out_flds.field[0];
        } else {
            entry_data = NULL;
        }
    } else {
        entry_data = (bcmltd_field_t *)data;
    }

    if ((entry_event != BCMIMM_ENTRY_DELETE) && (entry_data != NULL)) {
        info.type    = BCMCTH_FLEX_DIGEST_ENTRY_TYPE_KEY;
        info.tbl     = entry_tbl;
        entry_tbl[0] = key_entry;
        info.size    = 1;
        rv = fd_entry_set(unit, sid, &info, entry_data);
        SHR_IF_ERR_EXIT(rv);

        entry_tbl[0] = (uint32_t *)data_entry0;
        entry_tbl[1] = (uint32_t *)data_entry1;

        info.type    = BCMCTH_FLEX_DIGEST_ENTRY_TYPE_DATA;
        info.tbl     = entry_tbl;
        info.size    = 2;
        rv = fd_entry_set(unit, sid, &info, entry_data);
        SHR_IF_ERR_EXIT(rv);
    }

    rsp_ekw[0] = (uint32_t *)ret_key_entry;
    ekw[0] = (uint32_t *)key_entry;
    edw[0] = (uint32_t *)data_entry0;
    edw[1] = (uint32_t *)data_entry1;
    rsp_edw[0] = (uint32_t *)ret_data_entry0;
    rsp_edw[1] = (uint32_t *)ret_data_entry1;
    lt_mreq_info.entry_attrs = &hash_attrs;
    lt_mreq_info.entry_key_words  = ekw;
    lt_mreq_info.entry_data_words = edw;
    lt_mreq_info.pdd_l_dtyp = NULL;
    lt_mreq_info.same_key = 0;

    val64 = 0;
    fid = desc->entry_pri_id; /* FLEX_DIGEST_LKUPt_ENTRY_PRIORITYf; */
    LFIELD_VALUE_GET(entry_data, fid, val64);
    lt_mreq_info.entry_pri = (uint32_t)val64;

    wsize = BCMPTM_MAX_PT_FIELD_WORDS;
    lt_mreq_info.rsp_entry_key_buf_wsize  = wsize;
    lt_mreq_info.rsp_entry_data_buf_wsize = wsize;

    lt_mrsp_info.rsp_entry_key_words  = rsp_ekw;
    lt_mrsp_info.rsp_entry_data_words = rsp_edw;
    lt_mrsp_info.rsp_pdd_l_dtyp = NULL;
    lt_mrsp_info.pt_op_info = &pt_op_info;

    tcam_attrs->per_slice_entry_mgmt = TRUE;
    lt_mreq_info.entry_attrs = (void *)tcam_attrs;

    pt_info.tbl_inst = -1;
    if (entry_event == BCMIMM_ENTRY_INSERT) {
        req_op = BCMPTM_OP_INSERT;
    } else if (entry_event == BCMIMM_ENTRY_UPDATE) {
        req_op = BCMPTM_OP_INSERT;
    } else if (entry_event == BCMIMM_ENTRY_DELETE) {
        req_op = BCMPTM_OP_DELETE;
    } else {  /* BCMIMM_ENTRY_LOOKUP */
        req_op = BCMPTM_OP_LOOKUP;
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_ltm_mreq_keyed_lt(unit,
                                                     req_flags,
                                                     sid,
                                                     (void *)&pt_info,
                                                     NULL,
                                                     req_op,
                                                     &lt_mreq_info,
                                                     trans_id,
                                                     &lt_mrsp_info,
                                                     &rsp_ltid,
                                                     &rsp_flags));
exit:
    SHR_FREE(buf_ptr);
    if (out.field != NULL) {
        SHR_FREE(out.field);
    }
    if (out_flds.field != NULL) {
        SHR_FREE(out_flds.field);
    }
    SHR_FUNC_EXIT();
}

/*!
  * \brief API to process the logical table entry.
  *
  * \param [in] unit Unit number.
  * \param [in] sid This is the logical table ID.
  * \param [in] trans_id is the transaction ID associated with this operation.
  * \param [in] entry_event indicates the operation type for the entry
  * \param [in] key_fields This is a linked list of the key fields.
  * \param [in] data_field This is a linked list of the data fields.
  * \param [in] context Is a pointer that was given during registration.
  * \param [out] output_fields.
  *
  * \retval SHR_E_NONE Success.
  * \retval !SHR_E_NONE Failure.
  */
int
bcmcth_flex_digest_lentry_process(int unit,
                     bcmltd_sid_t sid,
                     uint32_t trans_id,
                     bcmimm_entry_event_t entry_event,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     void *context,
                     bcmltd_fields_t *output_fields)
{
    bcmcth_flex_digest_variant_field_id_t *desc;

    SHR_FUNC_ENTER(unit);

    desc = (bcmcth_flex_digest_variant_field_id_t *)context;

    /* FLEX_DIGEST_LKUP_PRESELt */
    if (sid == desc->presel_sid) {
        SHR_IF_ERR_EXIT(fd_presel_lentry_process (unit, sid, trans_id,
              entry_event, key, data, desc, output_fields));
    } else if (sid == desc->lkup_sid) {
        SHR_IF_ERR_EXIT(fd_lkup_lentry_process (unit, sid, trans_id,
              entry_event, key, data, desc, output_fields));
    } else if (sid == FLEX_DIGEST_HASH_SALTt) {
        SHR_IF_ERR_EXIT(fd_hash_salt_lentry_process (unit, sid, trans_id,
              entry_event, key, data, context, output_fields));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
  * \brief API to validate the logical table entry.
  *
  * \param [in] unit Unit number.
  * \param [in] sid This is the logical table ID.
  * \param [in] entry_event indicates the operation type for the entry
  * \param [in] key_fields This is a linked list of the key fields
  * \param [in] data_field This is a linked list of the data fields
  * \param [in] context Is a pointer that was given during registration.
  *
  * \retval SHR_E_NONE Success.
  * \retval !SHR_E_NONE Failure.
  */
int
bcmcth_flex_digest_lentry_validate(int unit,
                     bcmltd_sid_t sid,
                     bcmimm_entry_event_t entry_event,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     void *context)
{
    int rv = SHR_E_NONE;
    bcmcth_flex_digest_variant_field_id_t *desc;

    SHR_FUNC_ENTER(unit);

    desc = (bcmcth_flex_digest_variant_field_id_t *)context;

    if (sid == desc->lkup_sid) {
        rv = fd_lkup_entry_validate(unit, entry_event,
                (bcmltd_field_t *)key,
                (bcmltd_field_t *)data,
                desc);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}
