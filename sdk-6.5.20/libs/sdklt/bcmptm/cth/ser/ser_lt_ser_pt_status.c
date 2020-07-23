/*! \file ser_lt_ser_pt_status.c
 *
 * Functions used to update data to SER_PT_STATUS IMM LT
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_ser_cth.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <shr/shr_debug.h>
#include <bcmptm/bcmptm_ser_cth.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>

#include "ser_lt_ser_pt_status.h"
#include "ser_lt_ser_pt_control.h"
#include "ser_lts.h"

/******************************************************************************
 * Local definitions
 */

#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be
 * corresponding to SER_PT_STATUSt logical table
 * for entry validation.
 */
static int
bcmptm_ser_pt_status_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                             bcmimm_entry_event_t event_reason,
                                             const bcmltd_field_t *key,
                                             const bcmltd_field_t *data,
                                             void *context);

static int
bcmptm_ser_pt_status_imm_callback_lookup(int unit,
                                         bcmltd_sid_t sid,
                                         const bcmltd_op_arg_t *op_arg,
                                         void *context,
                                         bcmimm_lookup_type_t lkup_type,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out);

static bcmimm_lt_cb_t bcmptm_ser_pt_status_callback = {

    /*! Validate function. */
    .validate = bcmptm_ser_pt_status_imm_validation_callback,

    /*! Staging function. */
    .stage = NULL,

    /*! Extended lookup function. */
    .op_lookup = bcmptm_ser_pt_status_imm_callback_lookup,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/******************************************************************************
 * Private Functions
 */

/* Map SER recovery type from DRD to SER. */
static bcmptm_ser_recovery_type_t
bcmptm_ser_resp_type_map(bcmdrd_ser_resp_t resp_type)
{
    bcmptm_ser_recovery_type_t ser_recovery;

    if (resp_type == BCMDRD_SER_RESP_NONE) {
        ser_recovery = SER_RECOVERY_NO_OPERATION;
    } else if (resp_type == BCMDRD_SER_RESP_ENTRY_CLEAR) {
        ser_recovery = SER_RECOVERY_ENTRY_CLEAR;
    } else {
        ser_recovery = SER_RECOVERY_CACHE_RESTORE;
    }
    return ser_recovery;
}

/* Adjust recovery type of some PTs. */
static void
bcmptm_ser_pt_recovery_type_adjust(int unit, bcmdrd_sid_t pt_id, bool single_bit,
                                   bcmptm_ser_check_type_t ser_check_type,
                                   bcmptm_ser_recovery_type_t *ser_resp_type)
{
    bcmdrd_ser_resp_t resp_type_mapped;
    bcmdrd_sid_t pt_id_map;
    int pt_id_ecc_index, rv, index_map = 0;
    bool cache_en;
    bcmptm_ptcache_ctr_info_t ctr_info;
    bcmdrd_ser_resp_t resp_type;
    bcmdrd_pt_ser_info_t info;
    bcmptm_ser_recovery_type_t ser_resp_type_for_1bit;
    bcmdrd_pt_ser_en_ctrl_t en_ctrl;

    ser_resp_type_for_1bit = *ser_resp_type;

    rv = bcmdrd_pt_ser_info_get(unit, pt_id, &info);
    if (SHR_FAILURE(rv)) {
        info.resp = BCMDRD_SER_RESP_NONE;
    }
    resp_type = info.resp;
    resp_type_mapped = resp_type;

    if (ser_check_type == SER_ECC_CHECK) {
        /* Single bit error is not corrected. */
        if (single_bit == 1) {
            if (resp_type == BCMDRD_SER_RESP_WRITE_CACHE_RESTORE) {
                resp_type_mapped = BCMDRD_SER_RESP_NONE;
            } else if (resp_type == BCMDRD_SER_RESP_ENTRY_CLEAR) {
                (void)bcmptm_ptcache_ctr_info_get(unit, 1, pt_id, &ctr_info);
                if (ctr_info.pt_is_ctr) {
                    resp_type_mapped = BCMDRD_SER_RESP_NONE;
                }
            }
        }
    } else if (ser_check_type == SER_PARITY_CHECK) {
        /* Only supports single bit error. */
        if (single_bit == 0) {
            resp_type_mapped = BCMDRD_SER_RESP_NONE;
        }
    }
    /*
     * If PT owns ECC view, PT can be corrected,
     * so reponse type should be cache_restore.
     */
    pt_id_map = pt_id;
    (void)bcmptm_ser_mem_ecc_info_get(unit, pt_id, 0, &pt_id_map,
                                     &pt_id_ecc_index, NULL);
    if (pt_id_map == pt_id) {
        /*
         * If PT owns cached view, PT can be corrected by cached view,
         * so reponse type should be cache_restore.
         */
        (void)bcmptm_ser_sid_remap(unit, pt_id, &pt_id_map, &index_map);
    }
    (void)bcmptm_pt_attr_is_cacheable(unit, pt_id_map, &cache_en);
    if (cache_en == 0 && ser_check_type == SER_ECC_CHECK) {
        if (resp_type != BCMDRD_SER_RESP_ENTRY_CLEAR &&
            resp_type != BCMDRD_SER_RESP_NONE) {
            if (single_bit) {
                resp_type_mapped = BCMDRD_SER_RESP_NONE;
            } else {
                resp_type_mapped = BCMDRD_SER_RESP_ENTRY_CLEAR;
            }
        }
    }
    *ser_resp_type = bcmptm_ser_resp_type_map(resp_type_mapped);

    /*
     * Take L2_USER_ENTRYm for example, for double bit error,
     * recovery type gotten from DRD is NONE, so we need to remap it to Cache_Restore.
     */
    if (cache_en == 1 && single_bit == 0 && bcmdrd_pt_attr_is_cam(unit, pt_id)) {
        rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_EC,
                                       pt_id, &en_ctrl);
        if (SHR_SUCCESS(rv) &&
            (ser_resp_type_for_1bit == SER_RECOVERY_CACHE_RESTORE)) {
            *ser_resp_type = SER_RECOVERY_CACHE_RESTORE;
        }
    }
}

/* Adjust recovery type of some PTs. */
static void
bcmptm_ser_pt_check_type_get(int unit, bcmdrd_sid_t sid, int enable,
                             bcmptm_ser_check_type_t *ser_check_type)
{
    int ecc_checking, rv;
    bool is_tcam;
    bcmdrd_sid_t sid_tcam_only = INVALIDm;
    int inject_err_to_key_fld = 0;
    bcmdrd_pt_ser_en_ctrl_t ctrl_info;

    is_tcam = bcmdrd_pt_attr_is_cam(unit, sid);
    if (is_tcam) {
        bcmptm_ser_tcam_remap(unit, sid, &sid_tcam_only, &inject_err_to_key_fld);
        rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_TCAM,
                                       sid_tcam_only, &ctrl_info);
        if (SHR_SUCCESS(rv)) {
            enable = 1;
        }
    }
    /* Update SER_PT_STATUSt */
    if (enable) {
        (void)bcmptm_ser_mem_ecc_info_get(unit, sid, 0, NULL,
                                          NULL, &ecc_checking);
        *ser_check_type = ecc_checking ? SER_ECC_CHECK : SER_PARITY_CHECK;
    } else {
        *ser_check_type = SER_NO_CHECK;
    }
}

/* Adjust part of data fields of SER_PT_STATUS. */
static int
bcmptm_ser_pt_status_adjust(int unit, bcmdrd_sid_t sid, bcmltd_fields_t *out)
{
    uint32_t index_count = 0, enable;
    int rv, inst_num, copy_num;
    bcmptm_ser_check_type_t ser_check_type;
    bcmptm_ser_recovery_type_t resp_type_1bit = 0, resp_type_2bit = 0;
    size_t i;
    bcmltd_field_t *lt_fld;
    bcmltd_fields_t lt_flds;

    SHR_FUNC_ENTER(unit);

    lt_flds.field = bcmptm_ser_imm_flds_allocate(unit, 7);
    if (lt_flds.field == NULL) {
        return SHR_E_MEMORY;
    }
    lt_flds.count = 7;

    index_count = bcmptm_scor_pt_index_count(unit, sid);
    bcmptm_ser_imm_fld_set(lt_flds, 0, SER_PT_STATUSt_PT_INDEX_NUMf, 0, index_count);

    rv = bcmptm_ser_tbl_inst_num_get(unit, sid, &inst_num, &copy_num);
    SHR_IF_ERR_EXIT(rv);

    bcmptm_ser_imm_fld_set(lt_flds, 1, SER_PT_STATUSt_PT_INST_NUMf,
                           0, (uint32_t)inst_num);
    bcmptm_ser_imm_fld_set(lt_flds, 2, SER_PT_STATUSt_PT_COPY_NUMf,
                           0, (uint32_t)copy_num);

    enable = bcmptm_ser_pt_control_field_val_get(unit, sid,
                                                 SER_PT_CONTROLt_ECC_PARITY_CHECKf);

    bcmptm_ser_pt_check_type_get(unit, sid, (int)enable, &ser_check_type);
    bcmptm_ser_imm_fld_set(lt_flds, 3, SER_PT_STATUSt_SER_CHECK_TYPEf, 0,
                           (uint32_t)ser_check_type);

    bcmptm_ser_pt_recovery_type_adjust(unit, sid, 1, ser_check_type,
                                       &resp_type_1bit);
    bcmptm_ser_imm_fld_set(lt_flds, 4,
                           SER_PT_STATUSt_SER_RECOVERY_TYPE_FOR_SINGLE_BITf,
                           0, (uint32_t)resp_type_1bit);

    resp_type_2bit = resp_type_1bit;
    bcmptm_ser_pt_recovery_type_adjust(unit, sid, 0, ser_check_type,
                                       &resp_type_2bit);
    bcmptm_ser_imm_fld_set(lt_flds, 5,
                           SER_PT_STATUSt_SER_RECOVERY_TYPE_FOR_DOUBLE_BITf,
                           0, (uint32_t)resp_type_2bit);
    /* Key */
    bcmptm_ser_imm_fld_set(lt_flds, 6, SER_PT_STATUSt_PT_IDf, 0, sid);

    rv = bcmimm_entry_update(unit, true, SER_PT_STATUSt, &lt_flds);
    SHR_IF_ERR_EXIT(rv);

    if (out == NULL) {
        /* exit */
        SHR_EXIT();
    }
    for (i = 0; i < out->count; i++) {
        lt_fld = out->field[i];
        switch (lt_fld->id) {
            case SER_PT_STATUSt_PT_INDEX_NUMf:
                lt_fld->data = index_count;
                break;
            case SER_PT_STATUSt_PT_INST_NUMf:
                lt_fld->data = inst_num;
                break;
            case SER_PT_STATUSt_PT_COPY_NUMf:
                lt_fld->data = copy_num;
                break;
            case SER_PT_STATUSt_SER_CHECK_TYPEf:
                lt_fld->data = ser_check_type;
                break;
            case SER_PT_STATUSt_SER_RECOVERY_TYPE_FOR_SINGLE_BITf:
                lt_fld->data = resp_type_1bit;
                break;
            case SER_PT_STATUSt_SER_RECOVERY_TYPE_FOR_DOUBLE_BITf:
                lt_fld->data = resp_type_2bit;
                break;
            default:
                break;
        }
    }

exit:
    bcmptm_ser_imm_flds_free(unit, lt_flds.field, 7);

    SHR_FUNC_EXIT();
}

/* IMM validation callback routine. */
static int
bcmptm_ser_pt_status_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                             bcmimm_entry_event_t event_reason,
                                             const bcmltd_field_t *key,
                                             const bcmltd_field_t *data,
                                             void *context)
{
    if (!bcmptm_ser_checking_enable(unit)) {
        return SHR_E_DISABLED;
    }
    if (event_reason != BCMIMM_ENTRY_LOOKUP) {
        return SHR_E_UNAVAIL;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Lookup an entry from the LT SER_PT_STATUS.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [in]  op_arg          The operation arguments.
 * \param [in]  context         Pointer to retrieve some context.
 * \param [in]  lkup_type       Lookup type.
 * \param [in]  In              Input key fields list.
 * \param [in/out] out          Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
bcmptm_ser_pt_status_imm_callback_lookup(int unit,
                                         bcmltd_sid_t sid,
                                         const bcmltd_op_arg_t *op_arg,
                                         void *context,
                                         bcmimm_lookup_type_t lkup_type,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out)
{
    const bcmltd_fields_t *key = NULL;
    bcmdrd_sid_t pt_sid = INVALIDm;
    int rv;
    bcmltd_field_t *lt_fld;
    bcmltd_fields_t *lt_fld_list = NULL;
    size_t i;

    SHR_FUNC_ENTER(unit);

    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;
    for (i = 0; i < key->count; i++) {
        lt_fld = key->field[i];
        if (lt_fld->id == SER_PT_CONTROLt_PT_IDf) {
            pt_sid = lt_fld->data;
            break;
        }
    }
    if (bcmdrd_pt_is_reg(unit, pt_sid) == 0 &&
        bcmdrd_pt_is_mem(unit, pt_sid) == 0) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    lt_fld_list = out;
    rv = bcmptm_ser_pt_status_adjust(unit, pt_sid, lt_fld_list);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
int
bcmptm_ser_pt_status_imm_insert(int unit, bcmdrd_sid_t sid)
{
    int rv = SHR_E_NONE;
    size_t field_num;
    bcmltd_fields_t lt_flds;

    field_num = 1;
    lt_flds.count = field_num;
    lt_flds.field = bcmptm_ser_imm_flds_allocate(unit, field_num);
    if (lt_flds.field == NULL) {
        return SHR_E_MEMORY;
    }
    /* Key */
    bcmptm_ser_imm_fld_set(lt_flds, 0, SER_PT_STATUSt_PT_IDf, 0, sid);

    rv = bcmimm_entry_insert(unit, SER_PT_STATUSt, &lt_flds);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to insert IMM SER_PT_STATUS\n")));
    }
    bcmptm_ser_imm_flds_free(unit, lt_flds.field, field_num);

    return rv;
}

int
bcmptm_ser_pt_status_imm_update(int unit, bcmdrd_sid_t pt_id,
                                uint32_t fid_lt, uint32_t data)
{
    bcmltd_fields_t lt_flds;
    int rv = SHR_E_NONE;

    lt_flds.field = bcmptm_ser_imm_flds_allocate(unit, 2);
    if (lt_flds.field == NULL) {
        return SHR_E_MEMORY;
    }
    bcmptm_ser_imm_fld_set(lt_flds, 0, SER_PT_STATUSt_PT_IDf, 0, pt_id);
    bcmptm_ser_imm_fld_set(lt_flds, 1, fid_lt, 0, data);
    lt_flds.count = 2;

    rv = bcmimm_entry_update(unit, true, SER_PT_STATUSt, &lt_flds);

    bcmptm_ser_imm_flds_free(unit, lt_flds.field, 2);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to update IMM SER_PT_STATUS of [%s]\n"),
                   bcmdrd_pt_sid_to_name(unit, pt_id)));
    }
    return rv;
}

int
bcmptm_ser_pt_status_callback_register(int unit)
{
    SHR_FUNC_ENTER(unit);
    /* To register callback for SER LTs here. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit,
                             SER_PT_STATUSt,
                             &bcmptm_ser_pt_status_callback,
                             NULL));
exit:
    SHR_FUNC_EXIT();
}

uint32_t
bcmptm_ser_pt_status_field_val_get(int unit, bcmdrd_sid_t pt_id, uint32_t fid_lt)
{
    bcmltd_fields_t lt_flds_in, lt_flds_out;
    int i = 0, data_field_num = SER_PT_STATUSt_FIELD_COUNT - 1;
    uint32_t field_val = (uint32_t)-1;

    SHR_FUNC_ENTER(unit);

    lt_flds_out.field = NULL;
    lt_flds_in.field = bcmptm_ser_imm_flds_allocate(unit, 1);
    if (lt_flds_in.field == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }
    /* Adjust PT status */
    SHR_IF_ERR_EXIT
        (bcmptm_ser_pt_status_adjust(unit, pt_id, NULL));

    lt_flds_in.count = 1;
    bcmptm_ser_imm_fld_set(lt_flds_in, 0, SER_PT_STATUSt_PT_IDf, 0, pt_id);

    lt_flds_out.field = bcmptm_ser_imm_flds_allocate(unit, data_field_num);
    if (lt_flds_out.field == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_MEMORY);
    }
    lt_flds_out.count = data_field_num;

    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit, SER_PT_STATUSt, &lt_flds_in, &lt_flds_out));

    while (i < data_field_num) {
        if (lt_flds_out.field[i]->id == fid_lt) {
            field_val = (uint32_t)lt_flds_out.field[i]->data;
            break;
        }
        i++;
    }

exit:
    if (lt_flds_out.field != NULL) {
        bcmptm_ser_imm_flds_free(unit, lt_flds_out.field, data_field_num);
    }
    if (lt_flds_in.field != NULL) {
        bcmptm_ser_imm_flds_free(unit, lt_flds_in.field, 1);
    }
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to lookup IMM SER_PT_STATUS of [%s]\n"),
                   bcmdrd_pt_sid_to_name(unit, pt_id)));
    }
    return field_val;
}

