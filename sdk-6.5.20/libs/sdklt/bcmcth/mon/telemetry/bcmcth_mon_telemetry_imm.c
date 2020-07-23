/*! \file bcmcth_mon_telmetry_imm.c
 *
 * BCMCTH Monitor Telemetry IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmcth/bcmcth_mon_drv.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmdrd_config.h>
#include <bcmcth/bcmcth_mon_util.h>
#include <bcmcth/bcmcth_mon_telemetry_util.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/*******************************************************************************
 * Local definitions
 */
static shr_famm_hdl_t telemetry_fld_array_hdl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */

static int
mon_telemetry_fid_type_get(int unit,bcmlrd_sid_t sid, bcmlrd_fid_t fid,
                          bcmltd_field_data_tag_t *d_type)
{

    const bcmlrd_table_rep_t *tbl = NULL;
    bcmlrd_field_def_t field_def;
    const bcmlrd_map_t *map = NULL;
    SHR_FUNC_ENTER(unit);

    tbl = bcmltd_table_get(sid);
    if (tbl == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_VERBOSE_EXIT(bcmlrd_map_get(unit, sid, &map));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_def_get(unit, fid,tbl, map,  &field_def));
    *d_type = field_def.dtag;
exit:
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_min_max_default_get(int unit,
                                  bcmlrd_sid_t ltid,
                                  bcmlrd_fid_t fid,
                                  uint64_t *min_value,
                                  uint64_t *max_value,
                                  uint64_t *def_value)
{

    const bcmlrd_field_data_t *field_data = NULL;
    bcmltd_field_data_tag_t d_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_get(unit, ltid, fid, &field_data));
    SHR_IF_ERR_EXIT
        (mon_telemetry_fid_type_get(unit, ltid, fid, &d_type));

    SHR_NULL_CHECK(min_value, SHR_E_PARAM);
    SHR_NULL_CHECK(max_value, SHR_E_PARAM);
    SHR_NULL_CHECK(def_value, SHR_E_PARAM);


    switch (d_type) {
        case BCMLT_FIELD_DATA_TAG_BOOL:
            *min_value = (uint64_t)field_data->min->is_true;
            *max_value = (uint64_t)field_data->max->is_true;
            *def_value = (uint64_t)field_data->def->is_true;
            break;
        case BCMLT_FIELD_DATA_TAG_U8:
            *min_value = (uint64_t)field_data->min->u8;
            *max_value = (uint64_t)field_data->max->u8;
            *def_value = (uint64_t)field_data->def->u8;
            break;
        case BCMLT_FIELD_DATA_TAG_U16:
            *min_value = (uint64_t)field_data->min->u16;
            *max_value = (uint64_t)field_data->max->u16;
            *def_value = (uint64_t)field_data->def->u16;
            break;
        case BCMLT_FIELD_DATA_TAG_U32:
            *min_value = (uint64_t)field_data->min->u32;
            *max_value = (uint64_t)field_data->max->u32;
            *def_value = (uint64_t)field_data->def->u32;
            break;
        case BCMLT_FIELD_DATA_TAG_U64:
            *min_value = (uint64_t)field_data->min->u64;
            *max_value = (uint64_t)field_data->max->u64;
            *def_value = (uint64_t)field_data->def->u64;
            break;
        default:
            *min_value = 0;
            *max_value = 0;
            *def_value = 0;
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_control_lt_fields_def_values_get(
    int unit,
    bcmcth_mon_telemetry_control_t *entry)
{
    uint64_t min_value, max_value, def_value;
    SHR_FUNC_ENTER(unit);

    /* Initialize required control fields to default values */
    SHR_IF_ERR_VERBOSE_EXIT(mon_telemetry_min_max_default_get(unit,
                MON_TELEMETRY_CONTROLt,
                MON_TELEMETRY_CONTROLt_MAX_EXPORT_LENGTHf,
                &min_value, &max_value, &def_value));
    entry->max_export_len = (uint32_t)def_value;

    SHR_IF_ERR_VERBOSE_EXIT(mon_telemetry_min_max_default_get(unit,
                MON_TELEMETRY_CONTROLt,
                MON_TELEMETRY_CONTROLt_MAX_NUM_PORTSf,
                &min_value, &max_value, &def_value));
    entry->max_num_ports = (uint32_t)def_value;

exit:
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_instance_lt_fields_def_values_get(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry)
{
    uint64_t min_value, max_value, def_value;
    int i;
    SHR_FUNC_ENTER(unit);

    /* Initialize required instance fields to default values */
    SHR_IF_ERR_VERBOSE_EXIT(mon_telemetry_min_max_default_get(unit,
                MON_TELEMETRY_INSTANCEt,
                MON_TELEMETRY_INSTANCEt_MON_COLLECTOR_IDf,
                &min_value, &max_value, &def_value));
    entry->collector_id = (uint32_t)def_value;

    SHR_IF_ERR_VERBOSE_EXIT(mon_telemetry_min_max_default_get(unit,
                MON_TELEMETRY_INSTANCEt,
                MON_TELEMETRY_INSTANCEt_MON_EXPORT_PROFILE_IDf,
                &min_value, &max_value, &def_value));
    entry->export_profile_id = (uint32_t)def_value;

    for (i = 0; i < BCMMON_TELEMETRY_OBJECTS_MAX; i++) {
        SHR_IF_ERR_VERBOSE_EXIT(mon_telemetry_min_max_default_get(unit,
                    MON_TELEMETRY_INSTANCEt,
                    MON_TELEMETRY_INSTANCEt_MON_TELEMETRY_OBJECT_IDf,
                    &min_value, &max_value, &def_value));
        entry->object_id[i] = (uint32_t)def_value;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_control_st_lt_update(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_telemetry_control_t *entry,
    bcmltd_fields_t *out)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_telemetry_control_op(unit, trans_id,
         sid, BCMCTH_MON_TELEMETRY_CONTROL_ST_OP_SET, entry, out));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_control_st_lt_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_telemetry_control_t *entry,
    bcmltd_fields_t *out)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_telemetry_control_op(unit, trans_id, sid,
         BCMCTH_MON_TELEMETRY_CONTROL_ST_OP_DEL, entry, out));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_instance_object_lt_update(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_telemetry_instance_op(unit, trans_id, sid,
        BCMCTH_MON_TELEMETRY_INSTANCE_OBJ_OP_SET, entry, output_fields));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_instance_object_lt_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_telemetry_instance_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_telemetry_instance_op(unit, trans_id, sid,
        BCMCTH_MON_TELEMETRY_INSTANCE_OBJ_OP_DEL, entry, NULL));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_instance_collector_lt_update(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *output_fields)
{
    uint64_t min_value, max_value, def_value;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(mon_telemetry_min_max_default_get(unit,
                MON_TELEMETRY_INSTANCEt,
                MON_TELEMETRY_INSTANCEt_MON_COLLECTOR_IDf,
                &min_value, &max_value, &def_value));
    if (entry->collector_id != def_value) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_telemetry_instance_op(unit, trans_id, sid,
                                 BCMCTH_MON_TELEMETRY_INSTANCE_COLLECTOR_OP_SET,
                                 entry, output_fields));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_instance_collector_lt_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_telemetry_instance_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_telemetry_instance_op(unit, trans_id, sid,
                             BCMCTH_MON_TELEMETRY_INSTANCE_COLLECTOR_OP_DEL,
                             entry, NULL));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_instance_export_profile_lt_update(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *output_fields)
{
    uint64_t min_value, max_value, def_value;
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT(mon_telemetry_min_max_default_get(unit,
                MON_TELEMETRY_INSTANCEt,
                MON_TELEMETRY_INSTANCEt_MON_EXPORT_PROFILE_IDf,
                &min_value, &max_value, &def_value));
    if (entry->export_profile_id != def_value) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_telemetry_instance_op(unit, trans_id, sid,
                            BCMCTH_MON_TELEMETRY_INSTANCE_EXPORT_PROFILE_OP_SET,
                            entry, output_fields));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_instance_export_profile_lt_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_telemetry_instance_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_telemetry_instance_op(unit, trans_id, sid,
                           BCMCTH_MON_TELEMETRY_INSTANCE_EXPORT_PROFILE_OP_DEL,
                           entry, NULL));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_instance_stats_lt_update(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_telemetry_instance_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_telemetry_instance_op(unit, trans_id, sid,
            BCMCTH_MON_TELEMETRY_INSTANCE_STATS_OP_SET,
            entry, NULL));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_instance_stats_lt_get(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_telemetry_instance_op(unit, trans_id, sid,
            BCMCTH_MON_TELEMETRY_INSTANCE_STATS_OP_LOOKUP,
            entry, output_fields));
exit:
    SHR_FUNC_EXIT();
}

static void
mon_telemetry_instance_oper_state_update(
    int unit,
    bcmltd_fields_t *out)
{
    /* There are no invalid oper states set, update state to valid */
    if (out && (!out->count)) {
        out->field[out->count]->id =
            MON_TELEMETRY_INSTANCEt_INSTANCE_OPERATIONAL_STATEf;
        out->field[out->count]->data =
            BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_VALID;
        out->field[out->count]->idx = 0;
        out->count++;
    }

}

/*******************************************************************************
 * IMM event handler
 */

/*!
 * \brief imm MON_TELEMETRY_CONTROLt notification input fields parsing.
 *
 * Parse imm MON_TELEMETRY_CONTROLt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] data IMM input data field array.
 * \param [out] telemetry_control telemetry control info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
int
mon_telemetry_control_lt_fields_parse(
    int unit,
    const bcmltd_field_t *data,
    bcmcth_mon_telemetry_control_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));
    SHR_IF_ERR_VERBOSE_EXIT(mon_telemetry_control_lt_fields_def_values_get(
                            unit, entry));

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_TELEMETRY_CONTROLt_STREAMING_TELEMETRYf:
                entry->streaming_telemetry = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_TELEMETRY_CONTROLt_MAX_EXPORT_LENGTHf:
                entry->max_export_len = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_TELEMETRY_CONTROLt_MAX_NUM_PORTSf:
                entry->max_num_ports = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_TELEMETRY_CONTROLt_MAX_EXPORT_LENGTH_OPERf:
                entry->max_export_len_oper = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_TELEMETRY_CONTROLt_MAX_NUM_PORTS_OPERf:
                entry->max_num_ports_oper = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_TELEMETRY_CONTROLt_OPERATIONAL_STATEf:
                entry->oper_state = fval;
                SHR_BITSET(entry->fbmp, fid);
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
 * \brief imm MON_TELEMETRY_OBJECTt notification input fields parsing.
 *
 * Parse imm MON_TELEMETRY_OBJECTt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] telemetry_object telemetry object info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
int
mon_telemetry_object_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    bcmcth_mon_telemetry_object_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_TELEMETRY_OBJECTt_MON_TELEMETRY_OBJECT_IDf:
                entry->object_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        idx = gen_field->idx;

        switch (fid) {
            case MON_TELEMETRY_OBJECTt_TELEMETRY_STATf:
                entry->telemetry_stat[idx] = fval;
                SHR_BITSET(entry->fbmp, fid);
                SHR_BITSET(entry->fbmp_tel_stats, idx);
                break;
            case MON_TELEMETRY_OBJECTt_PORT_IDf:
                entry->port_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_TELEMETRY_OBJECTt_PORT_NAME_LENf:
                entry->port_name_len = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_TELEMETRY_OBJECTt_PORT_NAMEf:
                entry->port_name[idx] = fval;
                SHR_BITSET(entry->fbmp, fid);
                SHR_BITSET(entry->fbmp_port_name, idx);
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
 * \brief imm MON_TELEMETRY_INSTANCEt notification input fields parsing.
 *
 * Parse imm MON_TELEMETRY_INSTANCEt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] telemetry_instance telemetry instance info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
int
mon_telemetry_instance_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    bcmcth_mon_telemetry_instance_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    SHR_IF_ERR_VERBOSE_EXIT(mon_telemetry_instance_lt_fields_def_values_get(
                            unit, entry));
    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_TELEMETRY_INSTANCEt_MON_TELEMETRY_INSTANCE_IDf:
                entry->instance_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        idx = gen_field->idx;

        switch (fid) {
            case MON_TELEMETRY_INSTANCEt_NUM_OBJECTSf:
                entry->num_objects = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_TELEMETRY_INSTANCEt_MON_TELEMETRY_OBJECT_IDf:
                entry->object_id[idx] = fval;
                SHR_BITSET(entry->fbmp, fid);
                SHR_BITSET(entry->fbmp_tel_objs, idx);
                break;
            case MON_TELEMETRY_INSTANCEt_CORE_INSTANCEf:
                entry->core_instance = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_TELEMETRY_INSTANCEt_MON_COLLECTOR_IDf:
                entry->collector_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_TELEMETRY_INSTANCEt_MON_EXPORT_PROFILE_IDf:
                entry->export_profile_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_TELEMETRY_INSTANCEt_INSTANCE_OPERATIONAL_STATEf:
                entry->oper_state = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_TELEMETRY_INSTANCEt_TX_PKTSf:
                entry->tx_pkts = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_TELEMETRY_INSTANCEt_TX_PKT_FAILSf:
                entry->tx_pkt_fails = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_telemetry_instance_oper_state_from_entry(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    if (entry == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (entry->oper_state) {
        output_fields->field[output_fields->count]->id =
            MON_TELEMETRY_INSTANCEt_INSTANCE_OPERATIONAL_STATEf;
        output_fields->field[output_fields->count]->data =
            entry->oper_state;
        output_fields->count++;

    }
exit:
    SHR_FUNC_EXIT();
}

static void
bcmcth_mon_telemetry_instance_copy_old_entry(
    int unit,
    bcmcth_mon_telemetry_instance_t *old_entry,
    bcmcth_mon_telemetry_instance_t *new_entry)
{

    if (SHR_BITGET(old_entry->fbmp,
                   MON_TELEMETRY_INSTANCEt_CORE_INSTANCEf) &&
       !SHR_BITGET(new_entry->fbmp,
                   MON_TELEMETRY_INSTANCEt_CORE_INSTANCEf)) {
        new_entry->core_instance = old_entry->core_instance;
    }
    if (SHR_BITGET(old_entry->fbmp,
                   MON_TELEMETRY_INSTANCEt_TX_PKTSf) &&
       !SHR_BITGET(new_entry->fbmp,
                   MON_TELEMETRY_INSTANCEt_TX_PKTSf)) {
        new_entry->tx_pkts = old_entry->tx_pkts;
    }
    if (SHR_BITGET(old_entry->fbmp,
                   MON_TELEMETRY_INSTANCEt_TX_PKT_FAILSf) &&
       !SHR_BITGET(new_entry->fbmp,
                   MON_TELEMETRY_INSTANCEt_TX_PKT_FAILSf)) {
        new_entry->tx_pkt_fails = old_entry->tx_pkt_fails;
    }
}

static int
bcmcth_mon_telemetry_control_oper_state_from_entry(
    int unit,
    bcmcth_mon_telemetry_control_t *entry,
    bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    if (entry == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (entry->oper_state) {
        output_fields->field[output_fields->count]->id =
            MON_TELEMETRY_CONTROLt_OPERATIONAL_STATEf;
        output_fields->field[output_fields->count]->data =
            entry->oper_state;
        output_fields->count++;

    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_telemetry_control_config_from_lookup(
    int unit,
    bcmltd_sid_t sid,
    bcmcth_mon_telemetry_control_t *entry,
    bcmltd_fields_t *out_flds)
{
    bcmltd_fields_t     in_flds = {0};
    bcmltd_field_t *data = NULL;
    SHR_FUNC_ENTER(unit);

    in_flds.count = 0;

    sal_memset(entry, 0, sizeof(bcmcth_mon_telemetry_control_t));
    /*
     * Do an IMM lookup to retrieve the control entry
     * data.
     */
    SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit, sid, &in_flds, out_flds));
    if (out_flds->count) {
        data = out_flds->field[0];
        SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_data_from_outfields(unit,
                    data,
                    out_flds));
        SHR_IF_ERR_EXIT(mon_telemetry_control_lt_fields_parse(unit,
                    data,
                    entry));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_telemetry_instance_config_from_lookup(
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_field_t *key,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *out_flds)
{
    shr_famm_hdl_t      hdl = NULL;
    bcmltd_fields_t     in_flds = {0};
    bcmltd_field_t *data = NULL;
    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(bcmcth_mon_telemetry_instance_t));
    hdl = telemetry_fld_array_hdl[unit];

    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(hdl, 3);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }


    in_flds.field[0]->id = key->id;
    in_flds.field[0]->data = key->data;
    /*
     * Do an IMM lookup to retrieve the instance
     * data.
     */
    SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit, sid, &in_flds, out_flds));
    if (out_flds->count) {
        data = out_flds->field[0];
        SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_data_from_outfields(unit,
                    data,
                    out_flds));
        SHR_IF_ERR_EXIT(mon_telemetry_instance_lt_fields_parse(unit,
                    key,
                    data,
                    entry));
    }
exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 3);
    }
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_telemetry_instance_config_get(
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_field_t *key,
    bcmcth_mon_telemetry_instance_t *entry)
{
    shr_famm_hdl_t      hdl = NULL;
    uint32_t    instance_out_count = 0;
    bcmltd_fields_t     out_flds = {0};
    SHR_FUNC_ENTER(unit);

    hdl = telemetry_fld_array_hdl[unit];
    SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_mon_telemetry_instance_out_fields_count(unit,
                &instance_out_count));
    out_flds.count = instance_out_count;
    out_flds.field = shr_famm_alloc(hdl, out_flds.count);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_instance_config_from_lookup(
                unit, sid,
                key,
                entry, &out_flds));

exit:
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, instance_out_count);
    }
    SHR_FUNC_EXIT();
}

static int
mon_telemetry_instance_lt_fields_lookup(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *output_fields)
{
    bcmcth_mon_telemetry_instance_t entry;
    shr_famm_hdl_t      hdl = NULL;
    bcmltd_fields_t     out_flds = {0};
    bcmltd_field_t *data = NULL;
    uint32_t    i;

    SHR_FUNC_ENTER(unit);

    hdl = telemetry_fld_array_hdl[unit];
    out_flds.field = shr_famm_alloc(hdl, 2);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    sal_memset(&entry, 0, sizeof(entry));

    data = output_fields->field[0];
    SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_data_from_outfields(unit,
                data,
                output_fields));
    SHR_IF_ERR_EXIT(mon_telemetry_instance_lt_fields_parse(unit,
                in->field[0],
                data,
                &entry));

    SHR_IF_ERR_EXIT(mon_telemetry_instance_stats_lt_get(unit, trans_id, sid,
                &entry, &out_flds));

    if (out_flds.field[0]->id !=
           MON_TELEMETRY_INSTANCEt_INSTANCE_OPERATIONAL_STATEf) {
        for (i = 0; i < output_fields->count; i++) {

            if (output_fields->field[i]->id ==
                    MON_TELEMETRY_INSTANCEt_TX_PKTSf) {
                output_fields->field[i]->data = out_flds.field[0]->data;
            }
            if (output_fields->field[i]->id ==
                    MON_TELEMETRY_INSTANCEt_TX_PKT_FAILSf) {
                output_fields->field[i]->data = out_flds.field[1]->data;
            }
        }
    }
exit:
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, 2);
    }
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
 *                        the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [out] output_fields This output parameter can be used by the component
 * to add fields into the entry. Typically this should be used for read-only
 * type fields that otherwise can not be set by the application.
 * The component must set the count field of the \c output_fields parameter to
 * indicate the actual number of fields that were set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
mon_telemetry_control_stage(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{

    bcmcth_mon_telemetry_control_t entry, old_entry;
    shr_famm_hdl_t      hdl = NULL;
    bcmltd_fields_t     out_flds = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mon_telemetry_control_lt_fields_parse(unit, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
        if (entry.oper_state !=
            BCMLTD_COMMON_TELEMETRY_CONTROL_STATE_T_T_VALID) {
            output_fields->count = 1;
            output_fields->field[0]->id =
                MON_TELEMETRY_CONTROLt_OPERATIONAL_STATEf;
            output_fields->field[0]->data = entry.oper_state;
            output_fields->field[0]->idx = 0;
        }
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            if (SHR_BITGET(entry.fbmp,
                           MON_TELEMETRY_CONTROLt_STREAMING_TELEMETRYf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_control_st_lt_update(unit, trans_id,
                                                        sid, &entry,
                                                        output_fields));
            }
            break;
        case BCMIMM_ENTRY_UPDATE:
            hdl = telemetry_fld_array_hdl[unit];

            out_flds.count = MON_TELEMETRY_CONTROLt_FIELD_COUNT;
            out_flds.field = shr_famm_alloc(hdl, out_flds.count);

            if (out_flds.field == NULL) {
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
            SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_control_config_from_lookup(
                        unit, sid,
                        &old_entry, &out_flds));

            SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_control_oper_state_from_entry(
                        unit,
                        &old_entry, output_fields));
            if (SHR_BITGET(entry.fbmp,
                           MON_TELEMETRY_CONTROLt_STREAMING_TELEMETRYf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_control_st_lt_update(unit, trans_id,
                                                        sid, &entry,
                                                        output_fields));
            }
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (mon_telemetry_control_st_lt_delete(unit, trans_id,
                                                    sid, &entry,
                                                    output_fields));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, MON_TELEMETRY_CONTROLt_FIELD_COUNT);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Telemetry instance entries update
 *
 * Update all instance entries using this object ID
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  object_id     Telemetry object ID.
 * \param [in]  event         This is the reason for the entry event.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
mon_telemetry_object_update(int unit,
                            uint32_t trans_id,
                            bcmcth_mon_telemetry_object_t *cur_entry,
                            bcmimm_entry_event_t event)
{
    uint32_t    i = 0, k = 0, sid = 0;
    uint32_t    num_keys = 0, key = 0;
    uint32_t    min = 0, max = 0;
    uint32_t    fid = 0;
    uint32_t    object_fid = 0;
    bool        match = false;
    bcmlrd_field_def_t  def;
    shr_famm_hdl_t      hdl = NULL;
    bcmltd_fields_t     in_flds = {0}, out_flds = {0};
    uint64_t fval;
    bcmcth_mon_telemetry_instance_t entry;
    bcmltd_field_t *data = NULL;
    uint32_t    instance_out_count = 0;

    SHR_FUNC_ENTER(unit);

    sid = MON_TELEMETRY_INSTANCEt;
    num_keys = 1;
    key = MON_TELEMETRY_INSTANCEt_MON_TELEMETRY_INSTANCE_IDf;
    object_fid = MON_TELEMETRY_INSTANCEt_MON_TELEMETRY_OBJECT_IDf;
    hdl = telemetry_fld_array_hdl[unit];

    /*
     * Do an IMM lookup to retrieve the instance
     * entries using this telemetry object ID.
     */
    in_flds.count = num_keys;
    in_flds.field = shr_famm_alloc(hdl, 3);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_mon_telemetry_instance_out_fields_count(unit,
                                                       &instance_out_count));
    out_flds.count = instance_out_count;
    out_flds.field = shr_famm_alloc(hdl, out_flds.count);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmlrd_table_field_def_get(unit, sid, key, &def));
    min = TELEMETRY_FIELD_MIN(def);
    max = TELEMETRY_FIELD_MAX(def);

    for (i = min; i <= max; i++) {

        in_flds.count = 1;
        in_flds.field[0]->id = key;
        in_flds.field[0]->data = i;
        out_flds.count = instance_out_count;

        if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) !=
                SHR_E_NONE) {
            continue;
        }
        match = false;
        /* Iterate over table fields for object ID. */
        for (k = 0; k < out_flds.count; k++) {
            fid = out_flds.field[k]->id;
            fval = out_flds.field[k]->data;

            if (fid == object_fid) {
                /* object ID is different. */
                if (fval == cur_entry->object_id) {
                    match = true;
                    break;
                }
            }
        }

        /*
         * If the Instance entry does not contain the
         * object of interest, continue onto the next entry.
         */

        if (match == false) {
            continue;
        } else {

            data = out_flds.field[0];
            SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_data_from_outfields(unit,
                        data,
                        &out_flds));
            in_flds.count = 1;
            SHR_IF_ERR_EXIT(mon_telemetry_instance_lt_fields_parse(unit,
                        in_flds.field[0],
                        data,
                        &entry));
            if (event == BCMIMM_ENTRY_DELETE) {
                SHR_IF_ERR_EXIT
                    (bcmcth_mon_st_instance_handle_object_delete(unit, &entry,
                                                                 cur_entry));
            } else {
                SHR_IF_ERR_EXIT
                    (bcmcth_mon_st_instance_handle_object_update(unit, &entry,
                                                                 cur_entry));
            }

        }

    }

exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 3);
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, instance_out_count);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key   This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data  This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [out] output_fields This output parameter can be used by the component
 * to add fields into the entry. Typically this should be used for read-only
 * type fields that otherwise can not be set by the application.
 * The component must set the count field of the \c output_fields parameter to
 * indicate the actual number of fields that were set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
mon_telemetry_object_stage(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{

    bcmcth_mon_telemetry_object_t entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mon_telemetry_object_lt_fields_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (mon_telemetry_object_update(unit,
                                             trans_id,
                                             &entry,
                                             event));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
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
 * \param [in] key   This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data  This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [out] output_fields This output parameter can be used by the component
 * to add fields into the entry. Typically this should be used for read-only
 * type fields that otherwise can not be set by the application.
 * The component must set the count field of the \c output_fields parameter to
 * indicate the actual number of fields that were set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
mon_telemetry_instance_stage(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{

    bcmcth_mon_telemetry_instance_t entry, old_entry;
    uint32_t    object_bmp_size = 0;
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);
    int i = 0;
    bool match_found = FALSE;

    SHR_FUNC_ENTER(unit);

    object_bmp_size = SHR_BITALLOCSIZE(BCMMON_TELEMETRY_OBJECTS_MAX);

    SHR_IF_ERR_VERBOSE_EXIT
        (mon_telemetry_instance_lt_fields_parse(unit, key, data, &entry));

    if (SHR_BITGET(entry.fbmp,
                MON_TELEMETRY_INSTANCEt_CORE_INSTANCEf) &&
            (entry.core_instance != BCMMON_TELEMETRY_INVALID_CORE)) {
        for (i = 0; i < MAX_TELEMETRY_BTE_CORES; i++) {
            if (telemetry_info->ha_mem->st_initialized[i] &&
                    (entry.core_instance ==
                     telemetry_info->ha_mem->st_uc_num[i])) {
                match_found = TRUE;
            }
        }
        if ((match_found == FALSE) && output_fields) {
            output_fields->count = 1;
            output_fields->field[0]->id =
                MON_TELEMETRY_INSTANCEt_INSTANCE_OPERATIONAL_STATEf;
            output_fields->field[0]->data =
                BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_CONTROL_ENTRY_ERROR;
            output_fields->field[0]->idx = 0;
            SHR_EXIT();
        }
    }

    if (output_fields) {
        output_fields->count = 0;
        if (entry.oper_state !=
            BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_VALID) {
            output_fields->count = 1;
            output_fields->field[0]->id =
                MON_TELEMETRY_INSTANCEt_INSTANCE_OPERATIONAL_STATEf;
            output_fields->field[0]->data = entry.oper_state;
            output_fields->field[0]->idx = 0;
        }
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            sal_memset(telemetry_info->ha_mem->missing_obj_bmp, 0,
                       object_bmp_size);
            if (SHR_BITGET(entry.fbmp,
                           MON_TELEMETRY_INSTANCEt_MON_TELEMETRY_OBJECT_IDf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_instance_object_lt_update(unit, trans_id,
                    sid, &entry, output_fields));
            }
            if (SHR_BITGET(entry.fbmp,
                           MON_TELEMETRY_INSTANCEt_MON_COLLECTOR_IDf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_instance_collector_lt_update(unit, trans_id,
                    sid, &entry, output_fields));
            }
            if (SHR_BITGET(entry.fbmp,
                           MON_TELEMETRY_INSTANCEt_MON_EXPORT_PROFILE_IDf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_instance_export_profile_lt_update(unit,
                    trans_id,
                    sid, &entry, output_fields));
            }
            if (SHR_BITGET(entry.fbmp,
                           MON_TELEMETRY_INSTANCEt_TX_PKTSf) ||
                SHR_BITGET(entry.fbmp,
                           MON_TELEMETRY_INSTANCEt_TX_PKT_FAILSf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_instance_stats_lt_update(unit,
                                                            trans_id,
                                                            sid, &entry));
            }
            mon_telemetry_instance_oper_state_update(unit, output_fields);
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_instance_config_get(
                        unit, sid,
                        key,
                        &old_entry));

            SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_instance_oper_state_from_entry(
                        unit,
                        &old_entry, output_fields));
            bcmcth_mon_telemetry_instance_copy_old_entry(
                        unit,
                        &old_entry, &entry);
            if (SHR_BITGET(entry.fbmp,
                        MON_TELEMETRY_INSTANCEt_MON_TELEMETRY_OBJECT_IDf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_instance_object_lt_delete(unit, trans_id,
                                                             sid, &entry));
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_instance_object_lt_update(unit, trans_id,
                                                   sid, &entry, output_fields));
            }
            if (SHR_BITGET(entry.fbmp,
                        MON_TELEMETRY_INSTANCEt_MON_COLLECTOR_IDf)) {
                if (SHR_BITGET(old_entry.fbmp,
                            MON_TELEMETRY_INSTANCEt_MON_COLLECTOR_IDf) &&
                    (old_entry.collector_id != entry.collector_id)) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (mon_telemetry_instance_collector_lt_delete(unit,
                                                                    trans_id,
                                                                    sid,
                                                                    &old_entry));
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_instance_collector_lt_update(unit,
                                                                trans_id,
                                                                sid, &entry,
                                                                output_fields));
            }
            if (SHR_BITGET(entry.fbmp,
                        MON_TELEMETRY_INSTANCEt_MON_EXPORT_PROFILE_IDf)) {
                if (SHR_BITGET(old_entry.fbmp,
                            MON_TELEMETRY_INSTANCEt_MON_EXPORT_PROFILE_IDf) &&
                    (old_entry.export_profile_id != entry.export_profile_id)) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (mon_telemetry_instance_export_profile_lt_delete(unit,
                                                                  trans_id,
                                                                  sid, &old_entry));
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_instance_export_profile_lt_update(unit,
                                                   trans_id,
                                                   sid, &entry, output_fields));
            }
            if ((SHR_BITGET(entry.fbmp,
                           MON_TELEMETRY_INSTANCEt_TX_PKTSf) &&
                (old_entry.tx_pkts != entry.tx_pkts)) ||
                (SHR_BITGET(entry.fbmp,
                           MON_TELEMETRY_INSTANCEt_TX_PKT_FAILSf) &&
                (old_entry.tx_pkt_fails != entry.tx_pkt_fails))) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_instance_stats_lt_update(unit,
                                                            trans_id,
                                                            sid, &entry));
            }
            mon_telemetry_instance_oper_state_update(unit, output_fields);
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_instance_config_get(
                        unit, sid,
                        key,
                        &old_entry));
            if ((old_entry.core_instance != BCMMON_TELEMETRY_INVALID_CORE)) {
                entry.core_instance = old_entry.core_instance;
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_instance_object_lt_delete(unit, trans_id,
                                                             sid, &entry));
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_instance_collector_lt_delete(unit, trans_id,
                                                                sid, &entry));
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_telemetry_instance_export_profile_lt_delete(unit,
                                                                     trans_id,
                                                                     sid,
                                                                     &entry));
            }
            sal_memset(telemetry_info->ha_mem->missing_obj_bmp, 0,
                       object_bmp_size);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup callback function of IMM event handler (bcmimm_lt_cb_t).
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
mon_telemetry_instance_lookup(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    void *context,
    bcmimm_lookup_type_t lkup_type,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *output_fields)
{

    SHR_FUNC_ENTER(unit);
    if (lkup_type == BCMIMM_LOOKUP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mon_telemetry_instance_lt_fields_lookup(unit, trans_id, sid,
                                                     in,
                                                     output_fields));
    }
exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief MON_TELEMETRY_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_TELEMETRY_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t telemetry_control_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = mon_telemetry_control_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief MON_TELEMETRY_OBJECT In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_TELEMETRY_OBJECT logical table entry commit stages.
 */
static bcmimm_lt_cb_t telemetry_object_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = mon_telemetry_object_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief MON_TELEMETRY_INSTANCE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_TELEMETRY_INSTANCE logical table entry commit stages.
 */
static bcmimm_lt_cb_t telemetry_instance_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = mon_telemetry_instance_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function. */
    .lookup =  mon_telemetry_instance_lookup
};

/*******************************************************************************
 * Public Functions
 */

int
bcmcth_mon_telemetry_register(int unit)
{
    uint32_t    instance_out_count = 0;
    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for Telemetry LTs here.
     */
    if (bcmcth_mon_imm_mapped(unit, MON_TELEMETRY_CONTROLt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_TELEMETRY_CONTROLt,
                                 &telemetry_control_hdl, NULL));
    }
    if (bcmcth_mon_imm_mapped(unit, MON_TELEMETRY_OBJECTt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_TELEMETRY_OBJECTt,
                                 &telemetry_object_hdl, NULL));
    }
    if (bcmcth_mon_imm_mapped(unit, MON_TELEMETRY_INSTANCEt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_TELEMETRY_INSTANCEt,
                                 &telemetry_instance_hdl, NULL));
        SHR_IF_ERR_VERBOSE_EXIT(
                bcmcth_mon_telemetry_instance_out_fields_count(unit,
                    &instance_out_count));
        SHR_IF_ERR_EXIT
            (shr_famm_hdl_init(instance_out_count,
                               &telemetry_fld_array_hdl[unit]));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_telemetry_fld_arr_hdl_get(int unit,
                                        shr_famm_hdl_t *hdl)
{
    SHR_FUNC_ENTER(unit);

    *hdl = telemetry_fld_array_hdl[unit];

    SHR_FUNC_EXIT();
}

