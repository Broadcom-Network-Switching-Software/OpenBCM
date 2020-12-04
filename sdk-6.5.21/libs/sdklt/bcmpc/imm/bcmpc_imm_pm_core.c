/*! \file bcmpc_imm_pm_core.c
 *
 * PC_PM_COREt IMM handler.
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
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>


#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_cfg_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_PM_COREt

/*! The data structure for PC_PC_COREt entry. */
typedef struct pc_pm_core_entry_s {
    bcmpc_pm_core_t key;
    bcmpc_pm_core_cfg_t data;
} pc_pm_core_entry_t;


/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Fill PM core structure according to the given LT field value.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] fval Field value.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_pm_core_entry_set(int unit, uint32_t fid, uint32_t fidx, uint64_t fval,
                     pc_pm_core_entry_t *entry)
{
    bcmpc_pm_core_t *pm_core = &entry->key;
    bcmpc_pm_core_cfg_t *ccfg = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_PM_COREt_PC_PM_IDf:
            pm_core->pm_id = fval;
            break;
        case PC_PM_COREt_CORE_INDEXf:
            pm_core->core_idx = fval;
            break;
        case PC_PM_COREt_TX_LANE_MAP_AUTOf:
            ccfg->tx_lane_map_auto = fval;
            BCMPC_LT_FIELD_SET(ccfg->fbmp,
                               BCMPC_PM_CORE_LT_FLD_TX_LANE_MAP_AUTO);
            break;
        case PC_PM_COREt_TX_LANE_MAPf:
            ccfg->tx_lane_map = fval;
            BCMPC_LT_FIELD_SET(ccfg->fbmp,
                               BCMPC_PM_CORE_LT_FLD_TX_LANE_MAP);
            break;
        case PC_PM_COREt_RX_LANE_MAP_AUTOf:
            ccfg->rx_lane_map_auto = fval;
            BCMPC_LT_FIELD_SET(ccfg->fbmp,
                               BCMPC_PM_CORE_LT_FLD_RX_LANE_MAP_AUTO);
            break;
        case PC_PM_COREt_RX_LANE_MAPf:
            ccfg->rx_lane_map = fval;
            BCMPC_LT_FIELD_SET(ccfg->fbmp,
                               BCMPC_PM_CORE_LT_FLD_RX_LANE_MAP);
            break;
        case PC_PM_COREt_TX_POLARITY_FLIP_AUTOf:
            ccfg->tx_polarity_flip_auto = fval;
            BCMPC_LT_FIELD_SET(ccfg->fbmp,
                               BCMPC_PM_CORE_LT_FLD_TX_POLARITY_FLIP_AUTO);
            break;
        case PC_PM_COREt_TX_POLARITY_FLIPf:
            ccfg->tx_polarity_flip = fval;
            BCMPC_LT_FIELD_SET(ccfg->fbmp,
                               BCMPC_PM_CORE_LT_FLD_TX_POLARITY_FLIP);
            break;
        case PC_PM_COREt_RX_POLARITY_FLIP_AUTOf:
            ccfg->rx_polarity_flip_auto = fval;
            BCMPC_LT_FIELD_SET(ccfg->fbmp,
                               BCMPC_PM_CORE_LT_FLD_RX_POLARITY_FLIP_AUTO);
            break;
        case PC_PM_COREt_RX_POLARITY_FLIPf:
            ccfg->rx_polarity_flip = fval;
            BCMPC_LT_FIELD_SET(ccfg->fbmp,
                               BCMPC_PM_CORE_LT_FLD_RX_POLARITY_FLIP);
            break;
        case PC_PM_COREt_PMD_COM_CLKf:
            ccfg->pmd_com_clk = fval;
            BCMPC_LT_FIELD_SET(ccfg->fbmp,
                               BCMPC_PM_CORE_LT_FLD_PMD_CLOCK);
            break;
        case PC_PM_COREt_OPERATIONAL_STATEf:
            ccfg->oper_state = fval;
            break;
        case PC_PM_COREt_TX_LANE_MAP_OPERf:
            ccfg->tx_lane_map_oper = fval;
            break;
        case PC_PM_COREt_RX_LANE_MAP_OPERf:
            ccfg->rx_lane_map_oper = fval;
            break;
        case PC_PM_COREt_TX_POLARITY_FLIP_OPERf:
            ccfg->tx_polarity_flip_oper = fval;
            break;
        case PC_PM_COREt_RX_POLARITY_FLIP_OPERf:
            ccfg->rx_polarity_flip_oper = fval;
            break;
        case PC_PM_COREt_PM_MODEf:
            ccfg->pm_mode = fval;
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
 * Parse the input LT fields and save the data to pc_pm_core_entry_t.
 *
 * \param [in] unit Unit number.
 * \param [in] key_flds IMM input key field array.
 * \param [in] data_flds IMM input data field array.
 * \param [out] entry PM Core config data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
static int
pc_lt_pm_core_fields_parse(int unit,
                           const bcmltd_field_t *key_flds,
                           const bcmltd_field_t *data_flds,
                           pc_pm_core_entry_t *entry)
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
            (pc_pm_core_entry_set(unit, fid, 0, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fidx = fld->idx;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_pm_core_entry_set(unit, fid, fidx, fval, entry));

        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize all fields value as SW default value.
 *
 * This initialization is only used for Insert operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry PC_PM_CORE LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
pc_pm_core_lt_default_values_init(int unit, pc_pm_core_entry_t *lt_entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    bcmpc_pm_core_cfg_t *ccfg = NULL;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    ccfg = &(lt_entry->data);

    if (!BCMPC_LT_FIELD_GET(ccfg->fbmp,
                            BCMPC_PM_CORE_LT_FLD_TX_LANE_MAP_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PM_COREt,
                                     PC_PM_COREt_TX_LANE_MAP_AUTOf,
                                     1, &def_val, &num));
        ccfg->tx_lane_map_auto = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(ccfg->fbmp,
                            BCMPC_PM_CORE_LT_FLD_RX_LANE_MAP_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PM_COREt,
                                     PC_PM_COREt_RX_LANE_MAP_AUTOf,
                                     1, &def_val, &num));
        ccfg->rx_lane_map_auto = (uint8_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(ccfg->fbmp,
                            BCMPC_PM_CORE_LT_FLD_TX_POLARITY_FLIP_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PM_COREt,
                                     PC_PM_COREt_TX_POLARITY_FLIP_AUTOf,
                                     1, &def_val, &num));
        ccfg->tx_polarity_flip_auto = (uint8_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(ccfg->fbmp,
                            BCMPC_PM_CORE_LT_FLD_RX_POLARITY_FLIP_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PM_COREt,
                                     PC_PM_COREt_RX_POLARITY_FLIP_AUTOf,
                                     1, &def_val, &num));
        ccfg->rx_polarity_flip_auto = (uint8_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(ccfg->fbmp, BCMPC_PM_CORE_LT_FLD_PM_MODE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_PM_COREt, PC_PM_COREt_PM_MODEf,
                                     1, &def_val, &num));
        ccfg->pm_mode = (uint8_t)def_val;
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
 * Convert bcmltd_fields_t array \c flds to a bcmpc_pm_core_t \c key and a
 * bcmpc_pm_core_cfg_t \c data.
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
pc_pm_core_entry_parse(int unit, const bcmltd_fields_t *flds,
                       size_t key_size, void *key,
                       size_t data_size, void *data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    bcmpc_pm_core_t *pm_core = key;
    bcmpc_pm_core_cfg_t *ccfg = data;
    pc_pm_core_entry_t entry;

    SHR_FUNC_ENTER(unit);

    if ((key && key_size != sizeof(*pm_core)) ||
        (data && data_size != sizeof(*ccfg))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    sal_memset(&entry.key, 0, sizeof(entry.key));
    bcmpc_pm_core_cfg_t_init(&entry.data);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fval = flds->field[i]->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_pm_core_entry_set(unit, fid, 0, fval, &entry));
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
 * Convert a bcmpc_pm_core_t \c key and a bcmpc_pm_core_cfg_t \c data to
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
pc_pm_core_entry_fill(int unit,
                      size_t key_size, void *key,
                      size_t data_size, void *data,
                      bcmltd_fields_t *flds)
{
    size_t fcnt = 0;
    bcmpc_pm_core_t *pm_core = key;
    bcmpc_pm_core_cfg_t *ccfg = data;

    SHR_FUNC_ENTER(unit);

    if ((key && key_size != sizeof(*pm_core)) ||
        (data && data_size != sizeof(*ccfg))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_PC_PM_IDf, 0, pm_core->pm_id, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_CORE_INDEXf, 0, pm_core->core_idx, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_TX_LANE_MAP_AUTOf, 0,
             ccfg->tx_lane_map_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_TX_LANE_MAPf, 0, ccfg->tx_lane_map, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_TX_LANE_MAP_OPERf, 0,
             ccfg->tx_lane_map_oper, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_RX_LANE_MAP_AUTOf, 0,
             ccfg->rx_lane_map_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_RX_LANE_MAPf, 0, ccfg->rx_lane_map, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_RX_LANE_MAP_OPERf, 0,
             ccfg->rx_lane_map_oper, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_TX_POLARITY_FLIP_AUTOf, 0,
             ccfg->tx_polarity_flip_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_TX_POLARITY_FLIPf, 0,
             ccfg->tx_polarity_flip, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_TX_POLARITY_FLIP_OPERf, 0,
             ccfg->tx_polarity_flip_oper, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_RX_POLARITY_FLIP_AUTOf, 0,
             ccfg->rx_polarity_flip_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_RX_POLARITY_FLIPf, 0, ccfg->rx_polarity_flip,
             fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_RX_POLARITY_FLIP_OPERf, 0,
             ccfg->rx_polarity_flip_oper, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_PMD_COM_CLKf, 0, ccfg->pmd_com_clk, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_PM_MODEf, 0, ccfg->pm_mode, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_PM_COREt_OPERATIONAL_STATEf, 0, ccfg->oper_state, fcnt);
    }

    flds->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the PC data structure to the LT fields.
 *
 * This is the fill callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
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
pc_pm_core_oper_flds_fill(int unit, bcmpc_pm_core_cfg_t *data,
                          bcmltd_fields_t *flds)
{
    uint32_t i = 0;
    bcmpc_pm_core_cfg_t *config = data;

    SHR_FUNC_ENTER(unit);

    if (!data) {
        SHR_EXIT();
    }

    /* update the opeartional fields in the LT. */
    for (i = 0; i < flds->count; i++) {
        switch (flds->field[i]->id) {
            case PC_PM_COREt_RX_LANE_MAP_OPERf:
                flds->field[i]->data = config->rx_lane_map_oper;
                break;
            case PC_PM_COREt_TX_LANE_MAP_OPERf:
                flds->field[i]->data = config->tx_lane_map_oper;
                break;
            case PC_PM_COREt_RX_POLARITY_FLIP_OPERf:
                flds->field[i]->data = config->rx_polarity_flip_oper;
                break;
            case PC_PM_COREt_TX_POLARITY_FLIP_OPERf:
                flds->field[i]->data = config->tx_polarity_flip_oper;
                break;
            case PC_PM_COREt_OPERATIONAL_STATEf:
                flds->field[i]->data = config->oper_state;
                break;
            default:
                break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function for PC_PM_CORE LT lookup.
 *
 *  The callback function will be called during normal lookup,
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
pc_pm_core_lookup(int unit, bcmltd_sid_t sid, uint32_t trans_id,
                  void *context, bcmimm_lookup_type_t lkup_type,
                  const bcmltd_fields_t *in, bcmltd_fields_t *out)
{

    pc_pm_core_entry_t entry;
    const bcmltd_field_t *key_flds = NULL;
    bool port_based_cfg_enable = FALSE;
    bcmpc_pm_core_cfg_t ccfg;
    bool is_active = false;

    SHR_FUNC_ENTER(unit);

    bcmpc_pm_core_cfg_t_init(&entry.data);

    /* key fields are in the "out" parameter for traverse operations */
    key_flds = (lkup_type == BCMIMM_TRAVERSE)? *(out->field): *(in->field);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_enable));

    if (!port_based_cfg_enable) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_pm_core_fields_parse(unit, NULL, key_flds, &entry));

    /*
     * Retrieve hardware settings for PM to update the operational values.
     */
    sal_memset(&ccfg, 0, sizeof(ccfg));

    SHR_IF_ERR_EXIT
        (bcmpc_pm_core_is_active(unit, entry.key, &is_active));

    if (is_active) {
        SHR_IF_ERR_EXIT
            (bcmpc_pm_hw_core_config_get(unit, entry.key, &ccfg));
            ccfg.oper_state = BCMPC_VALID;
    } else {
        ccfg.oper_state = BCMPC_INACTIVE_PM;
    }

    /* Update the read-only operational values in the PC_PM_CORE LT. */
    SHR_IF_ERR_EXIT
        (pc_pm_core_oper_flds_fill(unit, &ccfg, out));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * IMM event handler
 */

/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_pm_core_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_pm_core_entry_parse;
    db_hdl.entry_fill = pc_pm_core_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.lookup = pc_pm_core_lookup;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function is invoked during the pre_config stage
 * to pre-populate all the PC_PM_CORE LT entries
 * with null entries.
 */
int
bcmpc_pm_core_prepopulate_all(int unit)
{
    uint64_t pm_id_min = 0, pm_id_max = 0, pm_id = 0;
    uint32_t num;
    pc_pm_core_entry_t entry;

    SHR_FUNC_ENTER(unit);

    /* Get the min and max values for the keys. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, PC_PM_COREt, PC_PM_COREt_PC_PM_IDf,
                              1, &pm_id_min, &num));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, PC_PM_COREt, PC_PM_COREt_PC_PM_IDf,
                              1, &pm_id_max, &num));

    sal_memset(&entry, 0, sizeof(entry));

    /* Get the software defaults from the map table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_pm_core_lt_default_values_init(unit, &entry));

    entry.data.oper_state = BCMPC_PORT_UNKNOWN;

    entry.key.core_idx = 0;
    for (pm_id = pm_id_min; pm_id <= pm_id_max; pm_id++) {
         entry.key.pm_id = pm_id;
         SHR_IF_ERR_EXIT
             (bcmpc_db_imm_entry_insert(unit, IMM_SID,
                                        sizeof(bcmpc_pm_core_t), &entry.key,
                                        sizeof(bcmpc_pm_core_cfg_t),
                                        &entry.data));
    }

exit:
    SHR_FUNC_EXIT();
}
