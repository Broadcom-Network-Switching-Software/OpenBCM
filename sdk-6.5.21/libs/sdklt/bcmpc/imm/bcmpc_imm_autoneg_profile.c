/*! \file bcmpc_imm_autoneg_profile.c
 *
 * PC_AUTONEG_PROFILEt IMM handler.
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

#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_cfg_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_AUTONEG_PROFILEt

/*! The data structure for PC_AUTONEG_PROFILEt entry. */
typedef struct pc_autoneg_prof_entry_s {
    int key;
    bcmpc_autoneg_prof_t data;
} pc_autoneg_prof_entry_t;

/*!
 * The data structure is used for mapping PC_AUTONEG_PROFILEt field IDs to
 * PC-specific enum values, e.g. \ref bcmpc_an_protocol_t and
 * \ref bcmpc_an_adv_ability_t.
 */
typedef struct pc_autoneg_ability_map_s {

    /*! LT field ID. */
    uint32_t fid;

    /*! PC enum value. */
    int pc_ability;

} pc_autoneg_ability_map_t;


/*******************************************************************************
 * Private Functions
 */

 /*!
  * \brief Initialize pc_autoneg_prof_entry_t data buffer.
  *
  * \param [in] entry entry buffer.
  *
  * \return Nothing.
  */
static void
pc_autoneg_prof_entry_t_init(pc_autoneg_prof_entry_t *entry)
{
    entry->key = BCMPC_INVALID_PROF_ID;
    bcmpc_autoneg_prof_t_init(&entry->data);
}

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
pc_autoneg_prof_entry_set(int unit, uint32_t fid, uint64_t fval,
                          pc_autoneg_prof_entry_t *entry)
{
    bcmpc_autoneg_prof_t *prof = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_AUTONEG_PROFILEt_PC_AUTONEG_PROFILE_IDf:
            entry->key = fval;
            break;
        case PC_AUTONEG_PROFILEt_AN_PARALLEL_DETECTf:
            break;
        case PC_AUTONEG_PROFILEt_AUTONEG_MODEf:
             prof->autoneg_mode = fval;
            break;
        case PC_AUTONEG_PROFILEt_ADVERT_SPEEDf:
             prof->an_advert_speed = fval;
            break;
        case PC_AUTONEG_PROFILEt_MEDIUM_TYPEf:
             prof->phy_medium = fval;
            break;
        case PC_AUTONEG_PROFILEt_FEC_MODEf:
             prof->fec_mode = fval;
            break;
        case PC_AUTONEG_PROFILEt_PAUSE_TYPEf:
             prof->pause_type = fval;
            break;
        case PC_AUTONEG_PROFILEt_LONG_CHf:
             prof->long_ch = fval;
            break;
        case PC_AUTONEG_PROFILEt_NUM_LANESf:
             prof->num_lanes = fval;
            break;
        case PC_AUTONEG_PROFILEt_LINK_TRAINING_OFFf:
             prof->link_training_off = fval;
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
 * Parse the input LT fields and save the data to pc_autoneg_prof_entry_t.
 *
 * \param [in] unit Unit number.
 * \param [in] key_flds IMM input key field array.
 * \param [in] data_flds IMM input data field array.
 * \param [out] entry Entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
static int
pc_lt_autoneg_prof_flds_parse(int unit,
                              const bcmltd_field_t *key_flds,
                              const bcmltd_field_t *data_flds,
                              pc_autoneg_prof_entry_t *entry)
{
    const bcmltd_field_t *fld;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key fields. */
    fld = key_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_autoneg_prof_entry_set(unit, fid, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_autoneg_prof_entry_set(unit, fid, fval, entry));

        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * IMM event handler
 */

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_autoneg_profile_stage(int unit,
                         bcmltd_sid_t sid,
                         uint32_t trans_id,
                         bcmimm_entry_event_t event,
                         const bcmltd_field_t *key_flds,
                         const bcmltd_field_t *data_flds,
                         void *context,
                         bcmltd_fields_t *output_fields)
{
    pc_autoneg_prof_entry_t entry, committed;
    bool port_based_cfg_enable = FALSE;

    SHR_FUNC_ENTER(unit);

    pc_autoneg_prof_entry_t_init(&entry);

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_enable));

    if (!port_based_cfg_enable) {
        SHR_EXIT();
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_autoneg_prof_flds_parse(unit, key_flds, data_flds,
                                               &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_autoneg_prof_internal_update(unit, entry.key,
                                                    &entry.data));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_autoneg_prof_flds_parse(unit, key_flds, NULL, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_db_imm_entry_lookup(unit,
                                           IMM_SID,
                                           P(&entry.key), P(&entry.data)));
            sal_memcpy(&committed, &entry, sizeof(committed));
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_autoneg_prof_flds_parse(unit, NULL, data_flds, &entry));
            if (sal_memcmp(&entry, &committed, sizeof(committed)) == 0) {
                /* Do nothing if the config is not changed. */
                SHR_EXIT();
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_autoneg_prof_internal_update(unit, entry.key,
                                                    &entry.data));
            break;
        case BCMIMM_ENTRY_DELETE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
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
 * Convert bcmltd_fields_t array \c flds to an integer profile id \c key and a
 * bcmpc_autoneg_prof_t \c data.
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
pc_autoneg_profile_entry_parse(int unit, const bcmltd_fields_t *flds,
                               size_t key_size, void *key,
                               size_t data_size, void *data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    pc_autoneg_prof_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_autoneg_prof_entry_t_init(&entry);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_autoneg_prof_entry_set(unit, fid, fval, &entry));
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
 * Convert an integer profile id \c key and a bcmpc_autoneg_prof_t \c data to
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
pc_autoneg_profile_entry_fill(int unit,
                              size_t key_size, void *key,
                              size_t data_size, void *data,
                              bcmltd_fields_t *flds)
{
    uint32_t *prof_id = key;
    bcmpc_autoneg_prof_t *prof = data;
    uint32_t fcnt = 0;

    SHR_FUNC_ENTER(unit);

    if ((key && key_size != sizeof(*prof_id)) ||
        (data && data_size != sizeof(*prof))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_AUTONEG_PROFILEt_PC_AUTONEG_PROFILE_IDf, 0,
             *prof_id, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_AUTONEG_PROFILEt_AUTONEG_MODEf, 0,
            prof->autoneg_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_AUTONEG_PROFILEt_ADVERT_SPEEDf, 0,
            prof->an_advert_speed, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_AUTONEG_PROFILEt_MEDIUM_TYPEf, 0,
            prof->phy_medium, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_AUTONEG_PROFILEt_FEC_MODEf, 0,
            prof->fec_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_AUTONEG_PROFILEt_PAUSE_TYPEf, 0,
            prof->pause_type, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_AUTONEG_PROFILEt_LONG_CHf, 0,
            prof->long_ch, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_AUTONEG_PROFILEt_NUM_LANESf, 0,
            prof->num_lanes, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_AUTONEG_PROFILEt_LINK_TRAINING_OFFf, 0,
            prof->link_training_off, fcnt);
    }

    flds->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_autoneg_profile_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_autoneg_profile_entry_parse;
    db_hdl.entry_fill = pc_autoneg_profile_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.stage = pc_autoneg_profile_stage;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}
