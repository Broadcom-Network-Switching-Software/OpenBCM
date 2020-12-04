/*! \file bcmcth_ldh_imm.c
 *
 * BCMCTH LDH IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd_config.h>
#include <bcmimm/bcmimm_int_comp.h>

#include <bcmcth/bcmcth_ldh_drv.h>

#include "bcmcth_ldh_internal.h"

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_LDH

/*******************************************************************************
 * Local definitions
 */

/*! Number of monitor stats fields. */
#define LDH_STATS_FID_LEN       MON_LDH_STATSt_FIELD_COUNT

/*! Number of monitor control fields. */
#define LDH_MON_FID_LEN         MON_LDH_CONTROLt_FIELD_COUNT

/*! Logical table ID for monitor stats handler. */
#define LDH_STATS_IMM_SID       MON_LDH_STATSt

/*! Logical table ID for monitor control handler. */
#define LDH_MON_CTRL_IMM_SID    MON_LDH_CONTROLt

/*! Define a structure of parameter of fields. */
typedef struct fld_param_s {
    /*! Input parameter of fields. */
    bcmltd_fields_t in;

    /*! Output parameter of fields. */
    bcmltd_fields_t out;
} fld_param_t;

/*! Define a structure of LDH IMM handle. */
typedef struct ldh_imm_hdl_s {
    /*! LDH STATS LT fields array handler. */
    shr_famm_hdl_t  stats_hdl;

    /*! Parameter of LDH STATS LT set. */
    fld_param_t     stats_set;

    /*! Parameter of LDH STATS LT get. */
    fld_param_t     stats_get;

    /*! Parameter of LDH CONTROL LT get. */
    fld_param_t     ctrl_get;
} ldh_imm_hdl_t;

/*! Field ID to opcode mapping. */
static const ldh_ctrl_fid2op_t ldh_ctrl_fid2op[] = {
    { MON_LDH_CONTROLt_MON_LDH_INSTANCEf,         LDH_MON_ID },
    { MON_LDH_CONTROLt_MONITORf,                  LDH_MON_ENABLE },
    { MON_LDH_CONTROLt_COUNT_MODEf,               LDH_MON_COUNT_MODE },
    { MON_LDH_CONTROLt_COLLECTION_MODEf,          LDH_MON_COLL_MODE },
    { MON_LDH_CONTROLt_TIME_STEPf,                LDH_MON_TIME_STEP },
    { MON_LDH_CONTROLt_CTR_EGR_FLEX_POOL_NUMBERf, LDH_MON_POOL_ID },
    { MON_LDH_CONTROLt_CTR_EGR_FLEX_OFFSET_MODEf, LDH_MON_OFFSET_MODE },
    { MON_LDH_CONTROLt_OFFSETf,                   LDH_MON_OFFSET_IDX },
    { MON_LDH_CONTROLt_COUNTER_INCREMENTf,        LDH_MON_CTR_INCR },
    { MON_LDH_CONTROLt_MAX_HISTOGRAM_GROUPf,      LDH_MON_GROUP_MAX }
};

/*! Handle of IMM fileds. */
static ldh_imm_hdl_t *ldh_imm_hdl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Transform opcode to field id.
 */
static int
ldh_ctrl_op2fid_transform(uint32_t opcode)
{
    uint32_t i;
    size_t size;

    size = COUNTOF(ldh_ctrl_fid2op);

    for (i = 0; i < size; i++) {
        if (opcode == ldh_ctrl_fid2op[i].opcode) {
            return ldh_ctrl_fid2op[i].fid;
        }
    }

    return -1;
}

/*!
 * \brief imm Parse opcode to string.
 *
 * \param [in] entry Array of entry.
 * \param [in] num Number of element.
 */
static void
ldh_mon_ctrl_imm_opcode_info(uint32_t *entry, int num)
{
    int i;
    char *s[] = {
        "LDH_MON_ID",
        "LDH_MON_COUNT_MODE",
        "LDH_MON_COLL_MODE",
        "LDH_MON_POOL_ID",
        "LDH_MON_OFFSET_MODE",
        "LDH_MON_OFFSET_IDX",
        "LDH_MON_CTR_INCR",
        "LDH_MON_GROUP_MAX",
        "LDH_MON_TIME_STEP",
        "LDH_MON_ENABLE"
    };

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META("The sequence of opcode as follows\n")));

    for (i = 0; i < num; i++) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("opcode: %s data: %d\n"),
                     s[i], entry[i]));
    }
}

static int
ldh_lt_fields_buff_free(int unit, bcmltd_fields_t *in)
{
    uint32_t i;
    SHR_FUNC_ENTER(unit);

    if(!in) {
        SHR_EXIT();
    }
    if (in->field) {
        for (i = 0; i < in->count; i++) {
            if (in->field[i]) {
                SHR_FREE(in->field[i]);
            }
        }
        SHR_FREE(in->field);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ldh_lt_fields_buff_alloc(int unit, bcmltd_fields_t *in)
{
    uint32_t idx = 0;
    size_t cnt;

    SHR_FUNC_ENTER(unit);

    cnt = in->count;

    SHR_ALLOC(in->field, cnt * sizeof(bcmltd_field_t *),
              "bcmcthLdhFieldsArr");
    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);
    sal_memset(in->field, 0, cnt * sizeof(bcmltd_field_t *));

    for (idx = 0; idx < cnt; idx++) {
        SHR_ALLOC(in->field[idx], sizeof(bcmltd_field_t),
                  "bcmcthLdhFields");
        SHR_NULL_CHECK(in->field[idx], SHR_E_MEMORY);
        sal_memset(in->field[idx], 0, sizeof(bcmltd_field_t));
    }

exit:
    if(SHR_FUNC_ERR()) {
        ldh_lt_fields_buff_free(unit, in);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief imm MON_LDH_STATSt notification input fields parsing.
 *
 * Parse imm MON_LDH_STATSt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
ldh_stats_update_process(int unit,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    ldh_stats_key_t st_key;

    SHR_FUNC_ENTER(unit);

    sal_memset(&st_key, 0, sizeof(ldh_stats_key_t));

    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_LDH_STATSt_MON_LDH_INSTANCEf:
                st_key.mon_id = fval;
                break;
            case MON_LDH_STATSt_MON_LDH_QUEUE_INSTANCEf:
                st_key.q_id = fval;
                break;
            case MON_LDH_STATSt_MON_LDH_BUCKET_INSTANCEf:
                st_key.bkt_id = fval;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    gen_field = data;

    if (gen_field == NULL) {
        SHR_IF_ERR_EXIT
            (bcmcth_ldh_stats_update(unit,
                                     LDH_STATS,
                                     st_key,
                                     0));
    }

    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_LDH_STATSt_BYTEf:
                SHR_IF_ERR_EXIT
                    (bcmcth_ldh_stats_update(unit,
                                             LDH_STATS_BYTES,
                                             st_key,
                                             fval));
                break;
            case MON_LDH_STATSt_PKTf:
                SHR_IF_ERR_EXIT
                    (bcmcth_ldh_stats_update(unit,
                                             LDH_STATS_PACKETS,
                                             st_key,
                                             fval));
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                      the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                      modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                      The callback can use this pointer to retrieve some
 *                      context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ldh_stats_stage(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                bcmimm_entry_event_t event,
                const bcmltd_field_t *key,
                const bcmltd_field_t *data,
                void *context,
                bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (ldh_stats_update_process(unit, key, data));
            break;
        case BCMIMM_ENTRY_DELETE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (output_fields) {
        output_fields->count = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init all uninitialized LT fields value as SW default value.
 *
 * This initialization will set default value to all uninitialized fields.
 *
 * \param [in] unit Unit number.
 * \param [out] entry MON_AGM_CONTROL LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to set default value to \c entry.
 */
static int
ldh_mon_ctrl_lt_default_init(int unit, uint32_t *entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_default_get(unit,
                                  MON_LDH_CONTROLt,
                                  MON_LDH_CONTROLt_MONITORf,
                                  1, &def_val, &num));
    idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_MONITORf].opcode;
    entry[idx] = def_val;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_default_get(unit,
                                  MON_LDH_CONTROLt,
                                  MON_LDH_CONTROLt_COUNT_MODEf,
                                  1, &def_val, &num));
    idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_COUNT_MODEf].opcode;
    entry[idx] = def_val;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_default_get(unit,
                                  MON_LDH_CONTROLt,
                                  MON_LDH_CONTROLt_COLLECTION_MODEf,
                                  1, &def_val, &num));
    idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_COLLECTION_MODEf].opcode;
    entry[idx] = def_val;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_default_get(unit,
                                  MON_LDH_CONTROLt,
                                  MON_LDH_CONTROLt_TIME_STEPf,
                                  1, &def_val, &num));
    idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_TIME_STEPf].opcode;
    entry[idx] = def_val;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_default_get(unit,
                                  MON_LDH_CONTROLt,
                                  MON_LDH_CONTROLt_CTR_EGR_FLEX_OFFSET_MODEf,
                                  1, &def_val, &num));
    idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_CTR_EGR_FLEX_OFFSET_MODEf].opcode;
    entry[idx] = def_val;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_default_get(unit,
                                  MON_LDH_CONTROLt,
                                  MON_LDH_CONTROLt_COUNTER_INCREMENTf,
                                  1, &def_val, &num));
    idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_COUNTER_INCREMENTf].opcode;
    entry[idx] = def_val;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_default_get(unit,
                                  MON_LDH_CONTROLt,
                                  MON_LDH_CONTROLt_MAX_HISTOGRAM_GROUPf,
                                  1, &def_val, &num));
    idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_MAX_HISTOGRAM_GROUPf].opcode;
    entry[idx] = def_val;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief imm MON_LDH_CONTROLt notification input fields parsing.
 *
 * Parse imm MON_LDH_CONTROLt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] entry info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
ldh_mon_ctrl_lt_field_parse(int unit,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            uint32_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    if (key->id == MON_LDH_CONTROLt_MON_LDH_INSTANCEf) {
        idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_MON_LDH_INSTANCEf].opcode;
        entry[idx] = key->data;
    }

    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_LDH_CONTROLt_MONITORf:
                idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_MONITORf].opcode;
                entry[idx] = fval;
                break;
            case MON_LDH_CONTROLt_COUNT_MODEf:
                idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_COUNT_MODEf].opcode;
                entry[idx] = fval;
                break;
            case MON_LDH_CONTROLt_COLLECTION_MODEf:
                idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_COLLECTION_MODEf].opcode;
                entry[idx] = fval;
                break;
            case MON_LDH_CONTROLt_TIME_STEPf:
                idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_TIME_STEPf].opcode;
                entry[idx] = fval;
                break;
            case MON_LDH_CONTROLt_CTR_EGR_FLEX_POOL_NUMBERf:
                idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_CTR_EGR_FLEX_POOL_NUMBERf].opcode;
                entry[idx] = fval;
                break;
            case MON_LDH_CONTROLt_CTR_EGR_FLEX_OFFSET_MODEf:
                idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_CTR_EGR_FLEX_OFFSET_MODEf].opcode;
                entry[idx] = fval;
                break;
            case MON_LDH_CONTROLt_OFFSETf:
                idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_OFFSETf].opcode;
                entry[idx] = fval;
                break;
            case MON_LDH_CONTROLt_COUNTER_INCREMENTf:
                idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_COUNTER_INCREMENTf].opcode;
                entry[idx] = fval;
                break;
            case MON_LDH_CONTROLt_MAX_HISTOGRAM_GROUPf:
                idx = ldh_ctrl_fid2op[MON_LDH_CONTROLt_MAX_HISTOGRAM_GROUPf].opcode;
                entry[idx] = fval;
                break;

            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                      the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                      modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                      The callback can use this pointer to retrieve some
 *                      context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ldh_mon_control_stage(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      bcmimm_entry_event_t event,
                      const bcmltd_field_t *key,
                      const bcmltd_field_t *data,
                      void *context,
                      bcmltd_fields_t *output_fields)
{
    uint32_t entry[64];
    size_t size;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, INVALID_DATA, sizeof(entry));
    size = COUNTOF(ldh_ctrl_fid2op);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            if (event == BCMIMM_ENTRY_INSERT) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ldh_mon_ctrl_lt_default_init(unit, entry));
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (ldh_mon_ctrl_lt_field_parse(unit, key, data, entry));

            ldh_mon_ctrl_imm_opcode_info(entry, size);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ldh_mon_ctrl_update(unit,
                                            entry,
                                            size));
            break;
        case BCMIMM_ENTRY_DELETE:
            if (key->id == MON_LDH_CONTROLt_MON_LDH_INSTANCEf) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_ldh_mon_ctrl_op(unit,
                                            LDH_MON_ENABLE,
                                            key->data,
                                            FALSE));
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (output_fields) {
        output_fields->count = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LDH STATS In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to LDH_STATS logical table entry commit stages.
 */
static bcmimm_lt_cb_t ldh_stats_imm_cb = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = ldh_stats_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_LDH_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t ldh_mon_ctrl_imm_cb = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = ldh_mon_control_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

static int
ldh_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, LDH_STATS_IMM_SID, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 LDH_STATS_IMM_SID,
                                 &ldh_stats_imm_cb,
                                 NULL));
    }

    rv = bcmlrd_map_get(unit, LDH_MON_CTRL_IMM_SID, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 LDH_MON_CTRL_IMM_SID,
                                 &ldh_mon_ctrl_imm_cb,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

static void
ldh_imm_cleanup(int unit)
{
    if (ldh_imm_hdl[unit]) {
        if (ldh_imm_hdl[unit]->stats_hdl) {
            shr_famm_free(ldh_imm_hdl[unit]->stats_hdl,
                          ldh_imm_hdl[unit]->stats_set.in.field,
                          ldh_imm_hdl[unit]->stats_set.in.count);
            shr_famm_hdl_delete(ldh_imm_hdl[unit]->stats_hdl);
            ldh_imm_hdl[unit]->stats_hdl = 0;
        }

        ldh_lt_fields_buff_free(unit, &ldh_imm_hdl[unit]->stats_get.in);
        ldh_lt_fields_buff_free(unit, &ldh_imm_hdl[unit]->stats_get.out);
        ldh_lt_fields_buff_free(unit, &ldh_imm_hdl[unit]->ctrl_get.in);
        ldh_lt_fields_buff_free(unit, &ldh_imm_hdl[unit]->ctrl_get.out);

        sal_free(ldh_imm_hdl[unit]);
        ldh_imm_hdl[unit] = NULL;
    }
}

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_ldh_imm_init(int unit)
{
    ldh_imm_hdl_t *ldhimm = NULL;
    size_t num;

    SHR_FUNC_ENTER(unit);

    if (ldh_imm_hdl[unit] != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    ldhimm = sal_alloc(sizeof(ldh_imm_hdl_t), "bcmcthLdhImm");
    if (ldhimm == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(ldhimm, 0, sizeof(ldh_imm_hdl_t));

    if (ldhimm->stats_hdl == NULL) {
        SHR_IF_ERR_EXIT
            (shr_famm_hdl_init(LDH_STATS_FID_LEN,
                               &ldhimm->stats_hdl));
    }

    ldhimm->stats_set.in.count = LDH_STATS_FID_LEN;
    ldhimm->stats_set.in.field = shr_famm_alloc(ldhimm->stats_hdl,
                                                LDH_STATS_FID_LEN);
    if (ldhimm->stats_set.in.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, MON_LDH_STATSt, &num));

    ldhimm->stats_get.in.count = 3;
    ldhimm->stats_get.in.field = NULL;

    ldhimm->stats_get.out.count = num;
    ldhimm->stats_get.out.field = NULL;

    SHR_IF_ERR_EXIT
        (ldh_lt_fields_buff_alloc(unit, &ldhimm->stats_get.in));
    SHR_IF_ERR_EXIT
        (ldh_lt_fields_buff_alloc(unit, &ldhimm->stats_get.out));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, MON_LDH_CONTROLt, &num));

    ldhimm->ctrl_get.in.count = 1;
    ldhimm->ctrl_get.in.field = NULL;

    ldhimm->ctrl_get.out.count = num;
    ldhimm->ctrl_get.out.field = NULL;

    SHR_IF_ERR_EXIT
        (ldh_lt_fields_buff_alloc(unit, &ldhimm->ctrl_get.in));
    SHR_IF_ERR_EXIT
        (ldh_lt_fields_buff_alloc(unit, &ldhimm->ctrl_get.out));

    ldh_imm_hdl[unit] = ldhimm;

    SHR_IF_ERR_EXIT(ldh_imm_register(unit));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "FAILED to init LDH imm.\n")));
        if (ldhimm) {
            ldh_imm_hdl[unit] = ldhimm;
            ldh_imm_cleanup(unit);
            ldhimm = NULL;
        }
        ldh_imm_hdl[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

void
bcmcth_ldh_imm_cleanup(int unit)
{
    ldh_imm_cleanup(unit);
}

int
bcmcth_ldh_stats_imm_set(int unit,
                         uint32_t mon_id,
                         uint32_t queue_id,
                         uint32_t bkt_id,
                         uint32_t pkt,
                         uint32_t byte,
                         uint32_t type)
{
    ldh_imm_hdl_t *ldhimm;

    SHR_FUNC_ENTER(unit);

    ldhimm = ldh_imm_hdl[unit];
    if (ldhimm == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    ldhimm->stats_set.in.field[0]->id = MON_LDH_STATSt_MON_LDH_INSTANCEf;
    ldhimm->stats_set.in.field[0]->data = mon_id;
    ldhimm->stats_set.in.field[1]->id = MON_LDH_STATSt_MON_LDH_QUEUE_INSTANCEf;
    ldhimm->stats_set.in.field[1]->data = queue_id;
    ldhimm->stats_set.in.field[2]->id = MON_LDH_STATSt_MON_LDH_BUCKET_INSTANCEf;
    ldhimm->stats_set.in.field[2]->data = bkt_id;
    ldhimm->stats_set.in.field[3]->id = MON_LDH_STATSt_BYTEf;
    ldhimm->stats_set.in.field[3]->data = byte;
    ldhimm->stats_set.in.field[4]->id = MON_LDH_STATSt_PKTf;
    ldhimm->stats_set.in.field[4]->data = pkt;

    switch (type) {
        case LDH_LT_INSERT:
            SHR_IF_ERR_EXIT
                (bcmimm_entry_insert(unit,
                                     MON_LDH_STATSt,
                                     &ldhimm->stats_set.in));
            break;
        case LDH_LT_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmimm_entry_update(unit,
                                     true,
                                     MON_LDH_STATSt,
                                     &ldhimm->stats_set.in));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_stats_imm_get(int unit,
                         uint32_t mon_id,
                         uint32_t queue_id,
                         uint32_t bkt_id,
                         uint64_t *pkt,
                         uint64_t *byte)
{
    uint32_t i;
    size_t num_fields;
    ldh_imm_hdl_t *ldhimm;

    SHR_FUNC_ENTER(unit);

    ldhimm = ldh_imm_hdl[unit];
    if (ldhimm == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    num_fields = ldhimm->stats_get.out.count;
    ldhimm->stats_get.in.field[0]->id = MON_LDH_CONTROLt_MON_LDH_INSTANCEf;
    ldhimm->stats_get.in.field[0]->data = mon_id;

    ldhimm->stats_get.in.field[0]->id = MON_LDH_STATSt_MON_LDH_INSTANCEf;
    ldhimm->stats_get.in.field[0]->data = mon_id;
    ldhimm->stats_get.in.field[1]->id = MON_LDH_STATSt_MON_LDH_QUEUE_INSTANCEf;
    ldhimm->stats_get.in.field[1]->data = queue_id;
    ldhimm->stats_get.in.field[2]->id = MON_LDH_STATSt_MON_LDH_BUCKET_INSTANCEf;
    ldhimm->stats_get.in.field[2]->data = bkt_id;

    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit,
                             MON_LDH_STATSt,
                             &ldhimm->stats_get.in,
                             &ldhimm->stats_get.out));

    for (i = 0; i < num_fields; i++) {
        if (ldhimm->stats_get.out.field[i]->id == MON_LDH_STATSt_BYTEf) {
            *byte = ldhimm->stats_get.out.field[i]->data;
        }
        if (ldhimm->stats_get.out.field[i]->id == MON_LDH_STATSt_PKTf) {
            *pkt = ldhimm->stats_get.out.field[i]->data;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_control_imm_get(int unit,
                           uint32_t mon_id,
                           uint32_t opcode,
                           uint32_t *val)
{
    uint32_t i;
    int fid;
    size_t num_fields;
    ldh_imm_hdl_t *ldhimm;

    SHR_FUNC_ENTER(unit);

    fid = ldh_ctrl_op2fid_transform(opcode);
    if (fid < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ldhimm = ldh_imm_hdl[unit];
    if (ldhimm == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    num_fields = ldhimm->ctrl_get.out.count;
    ldhimm->ctrl_get.in.field[0]->id = MON_LDH_CONTROLt_MON_LDH_INSTANCEf;
    ldhimm->ctrl_get.in.field[0]->data = mon_id;

    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit,
                             MON_LDH_CONTROLt,
                             &ldhimm->ctrl_get.in,
                             &ldhimm->ctrl_get.out));

    for (i = 0; i < num_fields; i++) {
        if ((uint32_t)fid == ldhimm->ctrl_get.out.field[i]->id) {
            *val = ldhimm->ctrl_get.out.field[i]->data;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}
