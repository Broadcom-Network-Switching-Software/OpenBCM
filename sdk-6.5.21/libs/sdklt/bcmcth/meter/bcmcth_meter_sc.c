/*! \file bcmcth_meter_sc.c
 *
 * This file defines Storm Control Meter
 * custom handler functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmcth/bcmcth_meter_sysm.h>
#include <bcmcth/bcmcth_meter_sc_util.h>

#include <bcmcth/bcmcth_meter_sc.h>

/*******************************************************************************
 * Local definitions and macros
 */

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*! Device specific attach function type for METER_ING_FP_TEMPLATE. */
typedef bcmcth_meter_sc_drv_t *(*bcmcth_meter_sc_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
{ _bc##_cth_meter_sc_drv_get },
static struct {
    bcmcth_meter_sc_drv_get_f drv_get;
} meter_sc_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/*! Storm control meter driver. */
static bcmcth_meter_sc_drv_t *meter_sc_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Initialize storm control meter entry.
 *
 * Initialize meter entry with default values from map file.
 *
 * \param [in] unit Unit number.
 * \param [out] out Pointer to output param structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
static int
meter_sc_init_params (int unit,
                      bcmcth_meter_sc_entry_t *out)
{
    bcmlrd_fid_t *fid_list = NULL;
    bcmlrd_field_def_t field_def;
    bcmlrd_sid_t sid;
    size_t num_fid = 0, count = 0;
    uint32_t i = 0;
    uint32_t table_sz = 0;

    SHR_FUNC_ENTER(unit);

    sid = METER_STORM_CONTROLt;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "bcmcthMeterFldList");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit, sid, num_fid, fid_list, &count));

    for (i = 0; i < count; i++) {
        sal_memset(&field_def, 0, sizeof(field_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit, sid, fid_list[i], &field_def));
        switch (fid_list[i]) {
        case METER_STORM_CONTROLt_METER_RATE_KBPSf:
            out->meter_rate_kbps = METER_SC_FIELD_DEF(field_def);
            break;
        case METER_STORM_CONTROLt_BURST_SIZE_KBITSf:
            out->burst_size_kbits = METER_SC_FIELD_DEF(field_def);
            break;
        case METER_STORM_CONTROLt_METER_RATE_PPSf:
            out->meter_rate_pps = METER_SC_FIELD_DEF(field_def);
            break;
        case METER_STORM_CONTROLt_BURST_SIZE_PKTSf:
            out->burst_size_pkts = METER_SC_FIELD_DEF(field_def);
            break;
        default:
            break;
        }
    }
exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get storm control meter config parameters.
 *
 * Parse user input, retrieve Meter LT params and validate
 * the values.
 *
 * \param [in] unit Unit number.
 * \param [in] field_count Number of fields.
 * \param [in] byte_mode Byte mode or packet mode.
 * \param [in] flist Pointer to input LT field list.
 * \param [out] out Pointer to output param structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
static int
meter_sc_parse_validate_params (int unit,
                                int field_count,
                                uint32_t byte_mode,
                                bcmltd_field_t **flist,
                                bcmcth_meter_sc_entry_t *out)
{
    int             i = 0;
    uint32_t        fid = 0;
    uint64_t        fval = 0;
    bool            config_err = false;
    bcmlrd_sid_t    sid;
    bcmlrd_field_def_t field_def;

    SHR_FUNC_ENTER(unit);

    sid = METER_STORM_CONTROLt;

    for (i = 0; i < field_count; i++) {
        if (flist[i] == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        fid = flist[i]->id;
        fval = flist[i]->data;

        sal_memset(&field_def, 0, sizeof(field_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit, sid, fid, &field_def));

        switch (fid) {
        case METER_STORM_CONTROLt_PORT_IDf:
            out->port_id = fval;
            break;
        case METER_STORM_CONTROLt_METER_STORM_CONTROL_IDf:
            out->meter_id = fval;
            break;
        case METER_STORM_CONTROLt_METER_RATE_KBPSf:
            if (byte_mode == METER_SC_PACKET_MODE) {
                config_err = true;
            }
            if (fval < METER_SC_FIELD_MIN(field_def) ||
                fval > METER_SC_FIELD_MAX(field_def)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            out->meter_rate_kbps = fval;
            break;
        case METER_STORM_CONTROLt_BURST_SIZE_KBITSf:
            if (byte_mode == METER_SC_PACKET_MODE) {
                config_err = true;
            }
            if (fval < METER_SC_FIELD_MIN(field_def) ||
                fval > METER_SC_FIELD_MAX(field_def)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            out->burst_size_kbits = fval;
            break;
        case METER_STORM_CONTROLt_METER_RATE_PPSf:
            if (byte_mode == METER_SC_BYTE_MODE) {
                config_err = true;
            }
            if (fval < METER_SC_FIELD_MIN(field_def) ||
                fval > METER_SC_FIELD_MAX(field_def)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            out->meter_rate_pps = fval;
            break;
        case METER_STORM_CONTROLt_BURST_SIZE_PKTSf:
            if (byte_mode == METER_SC_BYTE_MODE) {
                config_err = true;
            }
            if (fval < METER_SC_FIELD_MIN(field_def) ||
                fval > METER_SC_FIELD_MAX(field_def)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            out->burst_size_pkts = fval;
            break;
        case METER_STORM_CONTROLt_METER_RATE_KBPS_OPERf:
        case METER_STORM_CONTROLt_BURST_SIZE_KBITS_OPERf:
        case METER_STORM_CONTROLt_METER_RATE_PPS_OPERf:
        case METER_STORM_CONTROLt_BURST_SIZE_PKTS_OPERf:
            SHR_ERR_EXIT(SHR_E_ACCESS);
            break;

        default:
            break;
        }
    }

    /*
     * If there is no configuration error ie. (mode = byte and meter rate in kbps or
     * mode = packet and meter rate in pps), clear the unused fields that were
     * initialized to default.
     * If configuration error is present ie (mode = packet and meter rate in kbps or
     * mode = byte and meter rate in pps), then retain the defult values of the
     * unspecified fields.
     */
    if (!config_err) {
        if (byte_mode == METER_SC_BYTE_MODE) {
            out->meter_rate_pps = 0;
            out->burst_size_pkts = 0;
        } else {
            out->meter_rate_kbps = 0;
            out->burst_size_kbits = 0;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup and populate storm control meter parameters.
 *
 * Read H/w tables and return Meter LT params.
 *
 * \param [in] unit Unit number.
 * \param [in] in Meter structure pointer.
 * \param [out] foutlist Pointer to output param structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
static int
meter_sc_populate_params (int unit,
                          bcmcth_meter_sc_entry_t *in,
                          bcmltd_field_t *foutlist)
{
    size_t      num_fid = 0, count = 0;
    uint32_t    i = 0, fid = 0;
    uint32_t    table_sz = 0;
    bcmlrd_fid_t   *fid_list = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, METER_STORM_CONTROLt, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "bcmcthMeterFldList");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit,
                               METER_STORM_CONTROLt,
                               num_fid,
                               fid_list,
                               &count));

    for (i = 0; i < count; i++) {
        fid = fid_list[i];
        foutlist[i].id = fid;
        switch (i) {
        case METER_STORM_CONTROLt_PORT_IDf:
            foutlist[i].data = in->port_id;
            break;
        case METER_STORM_CONTROLt_METER_STORM_CONTROL_IDf:
            foutlist[i].data = in->meter_id;
            break;
        case METER_STORM_CONTROLt_METER_RATE_KBPSf:
            foutlist[i].data = in->meter_rate_kbps;
            break;
        case METER_STORM_CONTROLt_BURST_SIZE_KBITSf:
            foutlist[i].data = in->burst_size_kbits;
            break;
        case METER_STORM_CONTROLt_BURST_SIZE_KBITS_OPERf:
            foutlist[i].data = in->burst_size_kbits_oper;
            break;
        case METER_STORM_CONTROLt_METER_RATE_KBPS_OPERf:
            foutlist[i].data = in->meter_rate_kbps_oper;
            break;
        case METER_STORM_CONTROLt_METER_RATE_PPSf:
            foutlist[i].data = in->meter_rate_pps;
            break;
        case METER_STORM_CONTROLt_BURST_SIZE_PKTSf:
            foutlist[i].data = in->burst_size_pkts;
            break;
        case METER_STORM_CONTROLt_BURST_SIZE_PKTS_OPERf:
            foutlist[i].data = in->burst_size_pkts_oper;
            break;
        case METER_STORM_CONTROLt_METER_RATE_PPS_OPERf:
            foutlist[i].data = in->meter_rate_pps_oper;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
        }
    }
exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get meter field value.
 *
 * Parse user input and get the field corresponding to
 * a particular fid.
 *
 * \param [in] unit Unit number.
 * \param [in] field_count Number of fields.
 * \param [in] flist Pointer to input LT field list.
 * \param [in] fid Lt field id
 * \param [out] rval Pointer to return field value
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
static int
meter_sc_field_value_get (int unit, int field_count,
                          bcmltd_field_t **flist, uint32_t fid,
                          uint32_t *rval)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < field_count; i++) {
        if (flist[i] == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (flist[i]->id == fid) {
            *rval = flist[i]->data;
            break;
        }
    }
    if (i == field_count) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get Meter operating mode.
 *
 * Read the hardware register and get the meter mode
 * of this port.
 *
 * \param [in] unit         Unit number.
 * \param [in] op_arg       Operation arguments.
 * \param [in] port_id      Port number.
 * \param [out] byte_mode   Byte mode to be returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware read fails.
 */
static int
meter_sc_config_bytemode_get(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             uint32_t port_id,
                             uint32_t *byte_mode)
{
    bcmcth_meter_sc_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_sc_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    if (drv->bytemode_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->bytemode_get(unit, op_arg, port_id, byte_mode));
    } else {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert the storm control meter.
 *
 * Insert storm control meter entry into h/w table.
 *
 * \param [in] unit         Unit number.
 * \param [in] op_arg       Operation arguments.
 * \param [in] sc_entry     Storm control meter config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
static int
meter_sc_entry_insert(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmcth_meter_sc_entry_t *sc_entry)
{
    bcmcth_meter_sc_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_sc_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    if (drv->ins) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->ins(unit, op_arg, sc_entry));
    } else {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup storm control meter.
 *
 * Read storm control meter entry from h/w and return
 * the data.
 *
 * \param [in] unit         Unit number.
 * \param [in] op_arg       Operation arguments.
 * \param [in] port_id      Port number.
 * \param [in] meter_idx    Meter offset.
 * \param [out] ptr         Storm control meter pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
static int
meter_sc_entry_lookup(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      uint32_t port_id, uint32_t meter_idx,
                      bcmcth_meter_sc_entry_t **ptr)
{
    bcmcth_meter_sc_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_sc_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    if (drv->lkp) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->lkp(unit, op_arg, port_id, meter_idx, ptr));
    } else {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete storm control meter.
 *
 * Delete storm control meter entry from h/w table.
 *
 * \param [in] unit         Unit number.
 * \param [in] op_arg       Operation arguments.
 * \param [in] sc_entry     Storm control meter config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
static int
meter_sc_entry_delete(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmcth_meter_sc_entry_t *sc_entry)
{
    bcmcth_meter_sc_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_sc_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    if (drv->del) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->del(unit, op_arg, sc_entry));
    } else {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get storm control meter attributes.
 *
 * Delete storm control meter entry from h/w table.
 *
 * \param [in] unit         Unit number.
 * \param [in] trans_id     LT transaction ID.
 * \param [in] sc_entry     Storm control meter config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
static int
meter_sc_attrib_get(int unit,
                    bcmcth_meter_sc_attrib_t **attr)
{
    bcmcth_meter_sc_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_sc_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    if (drv->attrib_get) {
        SHR_IF_ERR_EXIT
            (drv->attrib_get(unit, attr));
    } else {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

static bcmcth_meter_sc_entry_t *
meter_sc_entry_get(int unit, int port, int idx)
{
    bcmcth_meter_sc_sw_state_t *sc_sw_state;
    bcmcth_meter_sc_attrib_t *attr;
    uint32_t meter_idx;

    if (SHR_FAILURE(bcmcth_meter_sc_sw_state_get(unit, &sc_sw_state))) {
        return NULL;
    }

    if (SHR_FAILURE(meter_sc_attrib_get(unit, &attr))) {
        return NULL;
    }

    meter_idx = (attr->meters_per_port * port) + idx;

    return &sc_sw_state->meter_entry[meter_idx];
}

static uint32_t
meter_sc_num_meters(int unit)
{
    bcmcth_meter_sc_attrib_t *attr;

    if (SHR_FAILURE(meter_sc_attrib_get(unit, &attr))) {
        return 0;
    }

    return attr->max_ports * attr->meters_per_port;
}

/*******************************************************************************
 * Public functions
 */

int
bcmcth_meter_sc_validate(int unit,
                         bcmlt_opcode_t opcode,
                         const bcmltd_fields_t *in,
                         const bcmltd_generic_arg_t *arg)
{
    int field_count;
    uint32_t port_id = 0, meter_idx = 0;
    uint32_t fid = 0;
    bcmltd_field_t **flist = NULL;
    bcmcth_meter_sc_attrib_t *attr = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(opcode);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(in->field, SHR_E_PARAM);

    field_count = in->count;
    flist = in->field;

    if (field_count == 0 || flist == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (meter_sc_attrib_get(unit, &attr));

    fid = METER_STORM_CONTROLt_PORT_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_field_value_get(unit, field_count, flist, fid, &port_id));

    fid = METER_STORM_CONTROLt_METER_STORM_CONTROL_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_field_value_get(unit, field_count, flist, fid, &meter_idx));

    if (port_id >= attr->max_ports ||
        meter_idx >= attr->meters_per_port) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_sc_insert(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg)
{
    int field_count;
    uint32_t port_id = 0, meter_idx = 0;
    uint32_t fid = 0, byte_mode = 0;
    bcmltd_field_t **flist = NULL;
    bcmcth_meter_sc_entry_t new_entry;
    bcmcth_meter_sc_entry_t *sc_entry;
    bcmcth_meter_sc_attrib_t *attr = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(in->field, SHR_E_PARAM);

    field_count = in->count;
    flist = in->field;

    if (field_count == 0 || flist == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (meter_sc_attrib_get(unit, &attr));

    fid = METER_STORM_CONTROLt_PORT_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_field_value_get(unit, field_count, flist, fid, &port_id));

    fid = METER_STORM_CONTROLt_METER_STORM_CONTROL_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_field_value_get(unit, field_count, flist, fid, &meter_idx));

    /* Retrieve the pointer to this meter entry from the database */
    sc_entry = meter_sc_entry_get(unit, port_id, meter_idx);
    SHR_NULL_CHECK(sc_entry, SHR_E_FAIL);

    /* If meter is already in use, return ERROR */
    if (sc_entry->meter_in_use) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    /* Initialize params */
    sal_memset(&new_entry, 0, sizeof(new_entry));
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_init_params(unit, &new_entry));

    /* Get meter operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_config_bytemode_get(unit, op_arg, port_id, &byte_mode));

    /* Parse and validate params */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_parse_validate_params(unit, field_count, byte_mode,
                                        flist, &new_entry));

    /* Insert the entry into H/W table */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_entry_insert(unit, op_arg, &new_entry));

    /* Mark the meter idx as used */
    sal_memcpy(sc_entry, &new_entry, sizeof(*sc_entry));
    sc_entry->meter_in_use = true;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_sc_lookup(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg)
{
    int field_count;
    uint32_t port_id = 0, meter_idx = 0;
    uint32_t fid = 0;
    bcmltd_field_t **flist = NULL;
    bcmltd_field_t *foutlist = NULL;
    bcmcth_meter_sc_entry_t *sc_entry = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(in->field, SHR_E_PARAM);

    field_count = in->count;
    flist = in->field;

    if (flist == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    fid = METER_STORM_CONTROLt_PORT_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_field_value_get(unit, field_count, flist, fid, &port_id));

    fid = METER_STORM_CONTROLt_METER_STORM_CONTROL_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_field_value_get(unit, field_count, flist, fid, &meter_idx));

    /* Retrieve the pointer to this meter entry from the database */
    sc_entry = meter_sc_entry_get(unit, port_id, meter_idx);
    SHR_NULL_CHECK(sc_entry, SHR_E_FAIL);

    if (!sc_entry->meter_in_use) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Read hw parameters and retrieve operational values */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_entry_lookup(unit, op_arg, port_id, meter_idx,
                               &sc_entry));

    field_count = out->count;
    foutlist = out->field[0];

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_populate_params(unit,
                                  sc_entry,
                                  foutlist));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_sc_delete(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg)
{
    int             field_count;
    uint32_t        port_id = 0, meter_idx = 0;
    uint32_t        fid = 0;
    bcmltd_field_t  **flist = NULL;
    bcmcth_meter_sc_entry_t *sc_entry = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(in->field, SHR_E_PARAM);

    field_count = in->count;
    flist = in->field;

    if ((field_count == 0) || (flist == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    fid = METER_STORM_CONTROLt_PORT_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_field_value_get(unit, field_count, flist, fid, &port_id));

    fid = METER_STORM_CONTROLt_METER_STORM_CONTROL_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_field_value_get(unit, field_count, flist, fid, &meter_idx));

    /* Retrieve the pointer to this meter entry from the database */
    sc_entry = meter_sc_entry_get(unit, port_id, meter_idx);
    SHR_NULL_CHECK(sc_entry, SHR_E_FAIL);

    if (!sc_entry->meter_in_use) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_entry_delete(unit, op_arg, sc_entry));

    sal_memset(sc_entry, 0, sizeof(bcmcth_meter_sc_entry_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_sc_update(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg)
{
    int field_count;
    uint32_t port_id = 0, meter_idx = 0;
    uint32_t fid, byte_mode = 0;
    bcmltd_field_t **flist = NULL;
    bcmcth_meter_sc_entry_t *sc_entry = NULL;
    bcmcth_meter_sc_entry_t new_entry;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(in->field, SHR_E_PARAM);

    field_count = in->count;
    flist = in->field;

    if (field_count == 0 || flist == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    fid = METER_STORM_CONTROLt_PORT_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_field_value_get(unit, field_count, flist, fid, &port_id));

    fid = METER_STORM_CONTROLt_METER_STORM_CONTROL_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_field_value_get(unit, field_count, flist, fid, &meter_idx));

    /* Retrieve the pointer to this meter entry from the database */
    sc_entry = meter_sc_entry_get(unit, port_id, meter_idx);
    SHR_NULL_CHECK(sc_entry, SHR_E_FAIL);

    if (!sc_entry->meter_in_use) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    sal_memcpy(&new_entry, sc_entry, sizeof(new_entry));

    /* Get meter operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_config_bytemode_get(unit, op_arg, port_id, &byte_mode));

    /* Parse and validate params */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_parse_validate_params(unit, field_count, byte_mode,
                                        flist, &new_entry));

    /* If no change in config, return */
    if (sal_memcmp(&new_entry, sc_entry, sizeof(new_entry)) == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_entry_insert(unit, op_arg, &new_entry));

    sal_memcpy(sc_entry, &new_entry, sizeof(*sc_entry));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_sc_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Perform device-specific software setup */
    meter_sc_drv[unit] = meter_sc_drv_get[dev_type].drv_get(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_sc_drv_cleanup(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    meter_sc_drv[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_sc_init(int unit,
                     bool warm)
{
    uint32_t num_meters = 0;
    uint32_t ha_req_size, ha_alloc_size;
    bcmcth_meter_sc_sw_state_t *sc_sw_state;
    bcmcth_meter_sc_attrib_t *attr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_sc_sw_state_get(unit, &sc_sw_state));

    SHR_IF_ERR_EXIT
        (meter_sc_attrib_get(unit, &attr));

    num_meters = (attr->max_ports * attr->meters_per_port);
    ha_req_size = (num_meters * sizeof(bcmcth_meter_sc_entry_t));
    ha_alloc_size = ha_req_size;

    sc_sw_state->bkp_entry =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_METER_COMP_ID,
                         METER_SC_BKP_SUB_COMP_ID,
                         "bcmcthMeterScSwState",
                         &ha_alloc_size);
    SHR_NULL_CHECK(sc_sw_state->bkp_entry, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(sc_sw_state->bkp_entry, 0, ha_alloc_size);

        bcmissu_struct_info_report(unit, BCMMGMT_METER_COMP_ID,
                                   METER_SC_BKP_SUB_COMP_ID, 0,
                                   sizeof(bcmcth_meter_sc_entry_t),
                                   num_meters,
                                   BCMCTH_METER_SC_ENTRY_T_ID);
    }

    SHR_ALLOC(sc_sw_state->meter_entry,
              ha_alloc_size,
              "bcmcthMeterScHwState");
    SHR_NULL_CHECK(sc_sw_state->meter_entry, SHR_E_MEMORY);
    sal_memset(sc_sw_state->meter_entry, 0, ha_alloc_size);

    sal_memcpy(sc_sw_state->meter_entry,
               sc_sw_state->bkp_entry,
               ha_alloc_size);

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(sc_sw_state->meter_entry);
        if (sc_sw_state->bkp_entry != NULL) {
            shr_ha_mem_free(unit, sc_sw_state->bkp_entry);
            sc_sw_state->bkp_entry = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_sc_cleanup(int unit)
{
    bcmcth_meter_sc_drv_t *drv = NULL;
    bcmcth_meter_sc_sw_state_t *sc_sw_state;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_sc_drv_get(unit, &drv));

    if (drv == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmcth_meter_sc_sw_state_get(unit, &sc_sw_state));

    SHR_FREE(sc_sw_state->meter_entry);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_sc_first(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out,
                      const bcmltd_generic_arg_t *arg)
{
    uint32_t port_id = 0, meter_idx = 0;
    uint32_t port_min = 0, meter_min = 0;
    bool found = false;
    bcmltd_field_t *foutlist = NULL;
    bcmlrd_field_def_t field_def;
    bcmcth_meter_sc_attrib_t *attr;
    bcmcth_meter_sc_entry_t *sc_entry = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_EXIT
        (meter_sc_attrib_get(unit, &attr));

    sal_memset(&field_def, 0, sizeof(field_def));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_def_get(
                unit,
                METER_STORM_CONTROLt,
                METER_STORM_CONTROLt_PORT_IDf,
                &field_def));
    port_min = METER_SC_FIELD_MIN(field_def);

    sal_memset(&field_def, 0, sizeof(field_def));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_def_get(
                unit,
                METER_STORM_CONTROLt,
                METER_STORM_CONTROLt_METER_STORM_CONTROL_IDf,
                &field_def));
    meter_min = METER_SC_FIELD_MIN(field_def);

    for (port_id = port_min; port_id < attr->max_ports; port_id++) {
        for (meter_idx = meter_min;
             meter_idx < attr->meters_per_port;
             meter_idx++) {
            sc_entry = meter_sc_entry_get(unit, port_id, meter_idx);
            if (sc_entry && sc_entry->meter_in_use) {
                found = true;
                break;
            }
        }
        if (found == true) {
            break;
        }
    }

    if (!found) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * read hw parameters and retrieve operational values.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_entry_lookup(unit, op_arg, port_id, meter_idx,
                               &sc_entry));

    foutlist = out->field[0];

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_populate_params(unit, sc_entry, foutlist));

exit:
    SHR_FUNC_EXIT();

}

int
bcmcth_meter_sc_next(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     const bcmltd_fields_t *in,
                     bcmltd_fields_t *out,
                     const bcmltd_generic_arg_t *arg)
{
    int field_count, i, j;
    uint32_t port_id = 0, meter_id = 0;
    uint32_t fid = 0;
    bool found = false;
    bcmltd_field_t **flist = NULL;
    bcmltd_field_t *foutlist = NULL;
    bcmcth_meter_sc_attrib_t *attr;
    bcmcth_meter_sc_entry_t *sc_entry = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(in->field, SHR_E_PARAM);

    field_count = in->count;
    flist = in->field;

    if (flist == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (meter_sc_attrib_get(unit, &attr));

    fid = METER_STORM_CONTROLt_PORT_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_field_value_get(unit, field_count, flist, fid, &port_id));

    fid = METER_STORM_CONTROLt_METER_STORM_CONTROL_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_field_value_get(unit, field_count, flist, fid, &meter_id));

    meter_id = (meter_id + 1);
    if (meter_id == attr->meters_per_port) {
        meter_id = 0;
        port_id = (port_id + 1);
    }

    if (port_id >= attr->max_ports) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    for (i = port_id; i < attr->max_ports; i++) {
        for (j = meter_id; j < attr->meters_per_port; j++) {
            sc_entry = meter_sc_entry_get(unit, i, j);
            if (sc_entry && sc_entry->meter_in_use) {
                found = true;
                port_id = i;
                meter_id = j;
                break;
            }
        }
        if (found == true) {
            break;
        }
        meter_id = 0;
    }

    if (!found) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Read hw parameters and retrieve operational values */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_entry_lookup(unit, op_arg, port_id, meter_id,
                               &sc_entry));

    field_count = out->count;
    foutlist = out->field[0];

    SHR_IF_ERR_VERBOSE_EXIT
        (meter_sc_populate_params(unit,
                                  sc_entry,
                                  foutlist));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_sc_commit(int unit,
                       uint32_t trans_id,
                       const bcmltd_generic_arg_t *arg)
{
    uint32_t table_size;
    bcmcth_meter_sc_attrib_t *attr;
    bcmcth_meter_sc_sw_state_t *sc_sw_state;
    bcmcth_meter_sc_entry_t *bkp_ptr = NULL;
    bcmcth_meter_sc_entry_t *active_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_sc_sw_state_get(unit, &sc_sw_state));

    SHR_IF_ERR_EXIT
        (meter_sc_attrib_get(unit, &attr));

    bkp_ptr = sc_sw_state->bkp_entry;
    active_ptr = sc_sw_state->meter_entry;
    if ((bkp_ptr == NULL) || (active_ptr == NULL)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    table_size = (meter_sc_num_meters(unit) * sizeof(bcmcth_meter_sc_entry_t));

    /* Copy active sw state to backup sw state. */
    sal_memcpy(bkp_ptr, active_ptr, table_size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_sc_abort(int unit,
                      uint32_t trans_id,
                      const bcmltd_generic_arg_t *arg)
{
    uint32_t table_size;
    bcmcth_meter_sc_sw_state_t *sc_sw_state;
    bcmcth_meter_sc_entry_t *bkp_ptr = NULL;
    bcmcth_meter_sc_entry_t *active_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_sc_sw_state_get(unit, &sc_sw_state));

    bkp_ptr = sc_sw_state->bkp_entry;
    active_ptr = sc_sw_state->meter_entry;
    if ((bkp_ptr == NULL) || (active_ptr == NULL)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    table_size = (meter_sc_num_meters(unit) * sizeof(bcmcth_meter_sc_entry_t));

    /* Copy backup sw state to active sw state. */
    sal_memcpy(active_ptr, bkp_ptr, table_size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_sc_drv_get(int unit,
                        bcmcth_meter_sc_drv_t **drv)
{
    SHR_FUNC_ENTER(unit);

    if (meter_sc_drv[unit] != NULL) {
        *drv = meter_sc_drv[unit];
    } else {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}
