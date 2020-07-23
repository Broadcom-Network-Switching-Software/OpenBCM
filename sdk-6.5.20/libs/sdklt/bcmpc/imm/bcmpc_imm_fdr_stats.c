/*! \file bcmpc_imm_fdr_stats.c
 *
 * PC_FDR_STATSt IMM handler.
 *
 * There are two handlers in this file.
 * 1. IMM event handler, \ref bcmimm_lt_cb_t.
 *    The event handler is used to notify PC for the various LT operations.
 *
 * 2. IMM-based DB schema handler, \ref bcmpc_db_imm_schema_handler_t.
 *    The schema handler is used to translate IMM (or LT) field array to/from PC
 *    specific data type.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_table.h>

#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_cfg_internal.h>

int bcmpc_imm_lt_register(int unit);

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_FDR_STATSt

/*! The data structure for PC_PORT_INFOt entry. */
typedef struct pc_fdr_port_stats_entry_s {
    bcmpc_lport_t key;
    bcmpc_fdr_port_stats_t data;
} pc_fdr_port_stats_entry_t;

/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Fill PC structure according to the given LT field value.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] fval Field value.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_port_fdr_stats_entry_set(int unit, uint32_t fid, uint32_t fidx,
                            uint64_t fval, pc_fdr_port_stats_entry_t *entry)
{
    bcmpc_fdr_port_stats_t *stats = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_FDR_STATSt_PORT_IDf:
            entry->key = fval;
            break;
        case PC_FDR_STATSt_FDR_START_TIMEf:
            stats->start_time = fval;
            break;
        case PC_FDR_STATSt_FDR_END_TIMEf:
            stats->end_time = fval;
            break;
        case PC_FDR_STATSt_UNCORRECTABLE_CODE_WORDSf:
            stats->num_uncorr_cws = fval;
            break;
        case PC_FDR_STATSt_SYMBOL_ERRORSf:
            stats->num_symerrs = fval;
            break;
        case PC_FDR_STATSt_CODE_WORDSf:
            stats->num_cws = fval;
            break;
        case PC_FDR_STATSt_CODE_WORD_S0_ERRORSf:
            stats->s0_errs = fval;
            break;
        case PC_FDR_STATSt_CODE_WORD_S1_ERRORSf:
            stats->s1_errs = fval;
            break;
        case PC_FDR_STATSt_CODE_WORD_S2_ERRORSf:
            stats->s2_errs = fval;
            break;
        case PC_FDR_STATSt_CODE_WORD_S3_ERRORSf:
            stats->s3_errs = fval;
            break;
        case PC_FDR_STATSt_CODE_WORD_S4_ERRORSf:
            stats->s4_errs = fval;
            break;
        case PC_FDR_STATSt_CODE_WORD_S5_ERRORSf:
            stats->s5_errs = fval;
            break;
        case PC_FDR_STATSt_CODE_WORD_S6_ERRORSf:
            stats->s6_errs = fval;
            break;
        case PC_FDR_STATSt_CODE_WORD_S7_ERRORSf:
            stats->s7_errs = fval;
            break;
        case PC_FDR_STATSt_CODE_WORD_S8_ERRORSf:
            stats->s8_errs = fval;
            break;
        case PC_FDR_STATSt_OPERATIONAL_STATEf:
            stats->oper_state = fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief IMM input LT fields parsing.
 *
 * Parse the input LT fields and save the data to pc_port_info_entry_t.
 *
 * \param [in] unit Unit number.
 * \param [in] key_flds IMM input key field array.
 * \param [in] data_flds IMM input data field array.
 * \param [out] entry Port info data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
static int
pc_lt_port_fdr_stats_fields_parse(int unit,
                                  const bcmltd_field_t *key_flds,
                                  const bcmltd_field_t *data_flds,
                                  pc_fdr_port_stats_entry_t *entry)
{
    const bcmltd_field_t *fld;
    uint32_t fid, fidx;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key fields. */
    fld = key_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_fdr_stats_entry_set(unit, fid, 0, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fidx = fld->idx;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_fdr_stats_entry_set(unit, fid, fidx, fval, entry));

        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * DB schema handler
 */

/*!
 * \brief Convert the LT fields to the PC data structure.
 *
 * This is the parse callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert bcmltd_fields_t array \c flds to a bcmpc_lport_t \c key and a
 * bcmpc_port_oper_status_t \c data.
 *
 * \param [in] unit Unit number.
 * \param [in] flds LT field array.
 * \param [in] key_size Key size.
 * \param [out] key Key value from \c flds.
 * \param [in] data_size Data size.
 * \param [out] data Data value from \c flds.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_imm_port_fdr_stats_entry_parse(int unit, const bcmltd_fields_t *flds,
                                  size_t key_size, void *key,
                                  size_t data_size, void *data)
{
    size_t i;
    uint32_t fid, fidx;
    uint64_t fval;
    pc_fdr_port_stats_entry_t entry;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fidx = flds->field[i]->idx;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_port_fdr_stats_entry_set(unit, fid, fidx, fval, &entry));
    }

    if (key) {
        sal_memcpy(key, &entry.key, key_size);
    }

    if (data) {
        sal_memcpy(data, &entry.data, data_size);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the PC data structure to the LT fields.
 *
 * This is the fill callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert a bcmpc_lport_t \c key and a bcmpc_port_cfg_t \c data to
 * bcmltd_fields_t array \c flds.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 * \param [out] flds LT field array.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_imm_port_fdr_stats_entry_fill(int unit,
                                 size_t key_size, void *key,
                                 size_t data_size, void *data,
                                 bcmltd_fields_t *flds)
{
    size_t fcnt = 0;
    bcmpc_lport_t *lport = key;
    bcmpc_fdr_port_stats_t *stats = data;
    bool is_unmapped = false;

    SHR_FUNC_ENTER(unit);

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_FDR_STATSt_PORT_IDf, 0,
             *lport, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_FDR_START_TIMEf,
             0, stats->start_time, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_FDR_END_TIMEf,
             0, stats->end_time, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_UNCORRECTABLE_CODE_WORDSf,
             0, stats->num_uncorr_cws, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_CODE_WORDSf,
             0, stats->num_cws, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_SYMBOL_ERRORSf,
             0, stats->num_symerrs, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_CODE_WORD_S0_ERRORSf,
             0, stats->s0_errs, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_CODE_WORD_S1_ERRORSf,
             0, stats->s1_errs, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_CODE_WORD_S2_ERRORSf,
             0, stats->s2_errs, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_CODE_WORD_S3_ERRORSf,
             0, stats->s3_errs, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_CODE_WORD_S4_ERRORSf,
             0, stats->s4_errs, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_CODE_WORD_S5_ERRORSf,
             0, stats->s5_errs, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_CODE_WORD_S6_ERRORSf,
             0, stats->s6_errs, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_CODE_WORD_S7_ERRORSf,
             0, stats->s7_errs, fcnt);
        SHR_IF_ERR_EXIT
            (bcmlrd_field_is_unmapped(unit, IMM_SID,
                                      PC_FDR_STATSt_CODE_WORD_S8_ERRORSf,
                                      &is_unmapped));
        if (!is_unmapped) {
            BCMPC_LT_FIELD_VAL_ADD
                (flds,
                 PC_FDR_STATSt_CODE_WORD_S8_ERRORSf,
                 0, stats->s8_errs, fcnt);
        }
        BCMPC_LT_FIELD_VAL_ADD
            (flds,
             PC_FDR_STATSt_OPERATIONAL_STATEf,
             0, stats->oper_state, fcnt);
    }

    flds->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the PC FDR data structure to the LT fields.
 *
 * This is the fill callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert an integer port id \c key and a bcmpc_fdr_port_stats_t \c data to
 * bcmltd_fields_t array \c flds.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 * \param [out] flds LT field array.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_fdr_port_stats_oper_flds_fill(int unit,
                                 bcmpc_fdr_port_stats_t *data,
                                 bcmltd_fields_t *flds)
{
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);

    if (!data) {
        SHR_EXIT();
    }

    /* update the fdr statistics fields in the LT. */
    for (i = 0; i < flds->count; i++) {
        switch (flds->field[i]->id) {
            case PC_FDR_STATSt_FDR_START_TIMEf:
                flds->field[i]->data = data->start_time;
                break;
            case PC_FDR_STATSt_FDR_END_TIMEf:
                flds->field[i]->data = data->end_time;
                break;
            case PC_FDR_STATSt_UNCORRECTABLE_CODE_WORDSf:
                flds->field[i]->data = data->num_uncorr_cws;
                break;
            case PC_FDR_STATSt_CODE_WORDSf:
                flds->field[i]->data = data->num_cws;
                break;
            case PC_FDR_STATSt_SYMBOL_ERRORSf:
                flds->field[i]->data = data->num_symerrs;
                break;
            case PC_FDR_STATSt_CODE_WORD_S0_ERRORSf:
                flds->field[i]->data = data->s0_errs;
                break;
            case PC_FDR_STATSt_CODE_WORD_S1_ERRORSf:
                flds->field[i]->data = data->s1_errs;
                break;
            case PC_FDR_STATSt_CODE_WORD_S2_ERRORSf:
                flds->field[i]->data = data->s2_errs;
                break;
            case PC_FDR_STATSt_CODE_WORD_S3_ERRORSf:
                flds->field[i]->data = data->s3_errs;
                break;
            case PC_FDR_STATSt_CODE_WORD_S4_ERRORSf:
                flds->field[i]->data = data->s4_errs;
                break;
            case PC_FDR_STATSt_CODE_WORD_S5_ERRORSf:
                flds->field[i]->data = data->s5_errs;
                break;
            case PC_FDR_STATSt_CODE_WORD_S6_ERRORSf:
                flds->field[i]->data = data->s6_errs;
                break;
            case PC_FDR_STATSt_CODE_WORD_S7_ERRORSf:
                flds->field[i]->data = data->s7_errs;
                break;
            case PC_FDR_STATSt_CODE_WORD_S8_ERRORSf:
                flds->field[i]->data = data->s8_errs;
                break;
            case PC_FDR_STATSt_OPERATIONAL_STATEf:
                flds->field[i]->data = data->oper_state;
                break;
            default:
                break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * IMM event handler
 */

/*!
 * \brief Callback function for FDR CONTROL LT lookup.
 *
 * The callback function will be called during normal lookup,
 * traverse or when required to read the content from hardware.
 * The purpose of this call is to allow the component to overwrite
 * read-only operational fields which are dynamic in nature.
 *
 * The lookup operations defined by \ref bcmimm_lookup_type_t can be classified
 * into two operation categories, lookup and traverse.
 *
 * For the lookup category the key fields will be found in the \c in parameter
 * while for the traverse category the key fields will be found in the \c out
 * parameter.
 *
 * In all the operations the \c out field will contain all the data fields and
 * their associated values as maintained by the IMM or default values
 * (for fields that were not yet set).
 * The component may only change the data portions of the data fields.
 * For improved performance the fields will be sorted based on
 * their field ID. Array fields will be sorted based on their index.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in Is a structure which contains the key fields but may include
 * also other fields. The component should only focus on the key fields.
 * \param [in,out] out this structure contains all the fields of the
 * table, so that the component should not delete or add any field to this
 * structure. The data values of the field were set by the IMM so the component
 * may only overwrite the data section of the fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_fdr_control_lookup(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context,
                      bcmimm_lookup_type_t lkup_type,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out)
{

    pc_fdr_port_stats_entry_t entry;
    bcmpc_fdr_port_stats_t fdr_stats;
    const bcmltd_field_t *key_flds = NULL;
    bool port_based_cfg_enable = FALSE;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;

    SHR_FUNC_ENTER(unit);


    /* key fields are in the "out" parameter for traverse operations */
    key_flds = (lkup_type == BCMIMM_TRAVERSE)? *(out->field): *(in->field);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_enable));

    if (!port_based_cfg_enable) {
        SHR_EXIT();
    }

    sal_memset(&entry, 0, sizeof(pc_fdr_port_stats_entry_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_port_fdr_stats_fields_parse(unit, NULL, key_flds, &entry));

    lport = entry.key;
    pport = bcmpc_lport_to_pport(unit, entry.key);

    /*
     * Defensive check, pport should be valid to retrieve the operational
     * values from the hardware.
     */
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_EXIT();
    }

    sal_memset(&fdr_stats, 0, sizeof(fdr_stats));
    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_fdr_stats_get(unit, lport, pport, &fdr_stats));

    SHR_IF_ERR_EXIT
        (pc_fdr_port_stats_oper_flds_fill(unit,
                                          (&fdr_stats), out));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_port_fdr_stats_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_imm_port_fdr_stats_entry_parse;
    db_hdl.entry_fill = pc_imm_port_fdr_stats_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.lookup = pc_fdr_control_lookup;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}
