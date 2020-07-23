/*! \file cci_imm.c
 *
 * Interface functions for CCI imm callbacks.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <shr/shr_fmm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_cci.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmissu/issu_api.h>
#include "cci_cache.h"
#include "cci_col.h"
#include "cci_internal.h"

/* Number of ports to collect counters from if using simulator */
#define NUM_SIM_PORTS   6

/*!
 * Helper function to set the configuration data.
 */

static int
cci_config_get(int unit,
               cci_context_t *con,
               uint32_t id,
               uint32_t elem,
               uint64_t data,
               cci_pol_cmd_t *cmd,
               cci_config_t *config)
{
    bcmlrd_client_field_info_t *f_info;
    uint32_t f;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_PARAM);

    f_info = con->config.f_info;
    for (f = 0; f < con->config.num_fid; f++) {
        if (f_info[f].id == id) {
            break;
        }
    }
    if (f == con->config.num_fid) {
        LOG_ERROR(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                  "Uknown config id [%d] \n"), id));
        SHR_EXIT();
    }
    id = f;

    if (!sal_strcmp(f_info[id].name, "COLLECTION_ENABLE")) {
        config->col_enable = data;
        *cmd = MSG_CMD_POL_CONFIG_COL_EN;
    } else if (!sal_strcmp(f_info[id].name, "PORT_ID")) {
        /*
         * Not all PORT_ID will be iterated.
         * Keep the valid_pbmp to identify if bit value is valid or not.
         */
        BCMDRD_PBMP_PORT_ADD(config->valid_pbmp, elem);
        if (data) {
            BCMDRD_PBMP_PORT_ADD(config->pbmp, elem);
        } else {
            BCMDRD_PBMP_PORT_REMOVE(config->pbmp, elem);
        }
        *cmd = MSG_CMD_POL_CONFIG_PORT;
    } else if (!sal_strcmp(f_info[id].name, "INTERVAL")) {
        config->interval = data;
        *cmd = MSG_CMD_POL_CONFIG_INTERVAL;
    } else if (!sal_strcmp(f_info[id].name, "MULTIPLIER_PORT")) {
        config->multiplier[CCI_CTR_TYPE_PORT] = data;
        *cmd = MSG_CMD_POL_CONFIG_MULTIPLIER_PORT;
    } else if (!sal_strcmp(f_info[id].name, "MULTIPLIER_IPIPE")) {
        config->multiplier[CCI_CTR_TYPE_IPIPE] = data;
        *cmd = MSG_CMD_POL_CONFIG_MULTIPLIER_IPIPE;
    } else if (!sal_strcmp(f_info[id].name, "MULTIPLIER_TM")) {
        config->multiplier[CCI_CTR_TYPE_TM] = data;
        *cmd = MSG_CMD_POL_CONFIG_MULTIPLIER_TM;
    } else if (!sal_strcmp(f_info[id].name, "MULTIPLIER_EPIPE")) {
         config->multiplier[CCI_CTR_TYPE_EPIPE] = data;
        *cmd = MSG_CMD_POL_CONFIG_MULTIPLIER_EPIPE;
    } else if (!sal_strcmp(f_info[id].name, "MULTIPLIER_EVICT")) {
        config->multiplier[CCI_CTR_TYPE_EVICT] = data;
        *cmd = MSG_CMD_POL_CONFIG_MULTIPLIER_EVICT;
    } else if (!sal_strcmp(f_info[id].name, "MULTIPLIER_SEC")) {
        config->multiplier[CCI_CTR_TYPE_SEC] = data;
        *cmd = MSG_CMD_POL_CONFIG_MULTIPLIER_SEC;
    } else if (!sal_strcmp(f_info[id].name, "EVICTION_THRESHOLD")) {
        config->eviction_threshold = data;
        *cmd = MSG_CMD_EVICTION_THRESHOLD;
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                  "Uknown config field [%s] \n"), f_info[id].name));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Configuration resource cleanup function
 */
static void
cci_config_cleanup(int unit,
                   cci_context_t   *con,
                   config_inmem_t *inmem)
{
    size_t num_fld;
    bcmltd_fields_t *in;
    bcmltd_fields_t *out;
    uint32_t i;

    in = &inmem->fields[0];
    out = &inmem->fields[1];
    num_fld = inmem->count_fld;
    if (in) {
        for (i = 0; i < num_fld; i++) {
            if (in->field[i]) {
                shr_fmm_free(in->field[i]);
            }
        }
        SHR_FREE(in->field);
    }
    if (out) {
        for (i = 0; i < num_fld; i++) {
            if (out->field[i]) {
                shr_fmm_free(out->field[i]);
            }
        }
        SHR_FREE(out->field);
    }

    if (inmem->f_info) {
        SHR_FREE(inmem->f_info);
    }

    con->init_config = 0;

}

/*!
 * In-memory logical table validate callback function.
 */
static int
cci_lt_validate_cb(int unit,
                bcmltd_sid_t sid,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context)
{
    cci_context_t   *con = (cci_context_t *)context;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_PARAM);

    if ((event_reason == BCMIMM_ENTRY_DELETE) ||
         (event_reason == BCMIMM_ENTRY_INSERT)) {
        /* Table entry is created during init. Table should have only one entry */
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * Event state control logical table validate callback function.
 */
static int
cci_lt_sync_state_control_validate_cb(int unit,
                                       bcmltd_sid_t sid,
                                       bcmimm_entry_event_t event_reason,
                                       const bcmltd_field_t *key_fields,
                                       const bcmltd_field_t *data_fields,
                                       void *context)
{
    cci_context_t   *con = (cci_context_t *)context;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_PARAM);

    if ((event_reason == BCMIMM_ENTRY_DELETE) ||
         (event_reason == BCMIMM_ENTRY_INSERT)) {
        /* Table entry is created during init. Table should have only one entry */
        /* Update only table */
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * In-memory logical table event callback function.
 */
static int
cci_lt_stage_cb(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key_fields,
                const bcmltd_field_t *data_fields,
                void *context,
                bcmltd_fields_t *output_fields)
{
    cci_context_t   *con = (cci_context_t *)context;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_PARAM);

    if (output_fields) {
        output_fields->count = 0;
    }
    if ((event_reason == BCMIMM_ENTRY_DELETE) ||
         (event_reason == BCMIMM_ENTRY_INSERT)) {
        /* Configuration table contains only one entry */
        SHR_ERR_EXIT(SHR_E_FAIL);
    } else if (event_reason == BCMIMM_ENTRY_LOOKUP) {
        SHR_EXIT();
    } else {
        cci_handle_t hpol;
        uint32_t id = 0;
        cci_config_t config;
        cci_pol_cmd_t cmd;
        /* Get polling collection context */
        SHR_IF_ERR_EXIT(
            bcmptm_cci_poll_handle_get(unit, con->hcol, &hpol));

        while (data_fields) {
            sal_memset(&config, 0, sizeof(config));
            id = data_fields->id;
            /* Get all the fields for same id */
            while ((data_fields) && (id == data_fields->id)) {
               SHR_IF_ERR_EXIT(
                   cci_config_get(unit, con, id,
                                  data_fields->idx,
                                  data_fields->data,
                                  &cmd, &config));
               data_fields = data_fields->next;
            }

            SHR_IF_ERR_EXIT(
                bcmptm_cci_col_poll_config(unit, hpol, cmd, &config));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * In memory event sync state
 * table record update
 */
static int
cci_event_sync_state_update(int unit,
                            cci_context_t   *con,
                            cci_ctr_type_id type,
                            cci_sync_event_state_t state)
{
    size_t num_fid;
    bcmlrd_client_field_info_t *f_info;
    bcmltd_fields_t *in;
    uint32_t i, id;
    config_inmem_t *in_state;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_INIT);

    in_state = &con->state;
    in = &in_state->fields[0];
    num_fid = in_state->num_fid;
    f_info = in_state->f_info;

    id = 0;
    for (i = 0; i < num_fid; i++) {
        in->field[id]->idx = 0;
        in->field[id]->id = f_info[i].id;
        if (f_info[i].key) {
            in->field[id]->data = type;
        } else {
            in->field[id]->data = state;
        }
        id += f_info[i].elements;
    }
    in->count = id;
    SHR_IF_ERR_EXIT(bcmimm_entry_update(unit, TRUE, in_state->sid, in));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * In-memory event state control logical table event callback function.
 */
static int
cci_lt_sync_state_control_stage_cb(int unit,
                                    bcmltd_sid_t sid,
                                    uint32_t trans_id,
                                    bcmimm_entry_event_t event_reason,
                                    const bcmltd_field_t *key_fields,
                                    const bcmltd_field_t *data_fields,
                                    void *context,
                                    bcmltd_fields_t *output_fields)
{
    cci_context_t   *con = (cci_context_t *)context;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(con, SHR_E_INIT);
    SHR_NULL_CHECK(key_fields, SHR_E_PARAM);
    SHR_NULL_CHECK(data_fields, SHR_E_PARAM);

    if (output_fields) {
        output_fields->count = 0;
    }
    if ((event_reason == BCMIMM_ENTRY_DELETE) ||
       (event_reason == BCMIMM_ENTRY_INSERT)) {
        /* Configuration table contains only one entry */
        SHR_ERR_EXIT(SHR_E_FAIL);
    } else if (event_reason == BCMIMM_ENTRY_LOOKUP) {
        SHR_EXIT();
    } else {
        cci_handle_t hpol;
        cci_ctr_type_id type;
        cci_sync_event_state_t state;
        type = (cci_ctr_type_id)key_fields->data;
        state = (cci_sync_event_state_t)data_fields->data;
        /* Get polling collection context */
        SHR_IF_ERR_EXIT
            (bcmptm_cci_poll_handle_get(unit, con->hcol, &hpol));
        /* Send message to sync in case it is OFF */
        if ((con->sync_state[type] == OFF) && (state == START)) {
            /* Update the sync state to START */
            SHR_IF_ERR_EXIT
                (cci_event_sync_state_update(unit, con, type, START));
            con->sync_state[type] = START;
            SHR_IF_ERR_EXIT
                (bcmptm_cci_stat_sync_start(unit, type));
        } else if ((con->sync_state[type] == COMPLETE) && (state == OFF)) {
            SHR_IF_ERR_EXIT
                (cci_event_sync_state_update(unit, con, type, OFF));
            con->sync_state[type] = OFF;
        } else if ((con->sync_state[type] == START) && (state == OFF)) {
            SHR_ERR_EXIT(SHR_E_BUSY);
        } else if ((con->sync_state[type] == COMPLETE) && (state == START)) {
            SHR_ERR_EXIT(SHR_E_BUSY);
        }

    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Configuration resource cleanup function
 */
void
bcmptm_cci_imm_config_cleanup(int unit,
                              cci_context_t   *con)
{
    cci_config_cleanup(unit, con, &con->config);
    cci_config_cleanup(unit, con, &con->state);
    cci_config_cleanup(unit, con, &con->state_ctrl);
    con->init_config = 0;
}


/*!
 * Configuration resource initialization function
 */
int
bcmptm_cci_imm_config_init(int unit,
                           cci_context_t   *con,
                           const char *table_name,
                           config_inmem_t *inmem,
                           bcmimm_lt_stage_cb *stage_cb,
                           bcmimm_lt_validate_cb *validate_cb)
{
    bcmlrd_table_attrib_t t_attrib;
    size_t num_fid;
    bcmltd_fields_t *in;
    bcmltd_fields_t *out;
    bcmlrd_client_field_info_t *f_info;
    bcmimm_lt_cb_t cb;
    uint32_t count = 0;
    uint32_t i;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_INIT);

    /* Get all table attributes. */
    rv = bcmlrd_table_attributes_get(unit, table_name, &t_attrib);
    if ((rv == SHR_E_NOT_FOUND) || (rv == SHR_E_UNAVAIL)) {
        /* Table may not be avaialble, use default */
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    num_fid = t_attrib.number_of_fields;

    /* Get the field definition. */
    SHR_ALLOC(inmem->f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "bcmptmCciControl");
    SHR_NULL_CHECK(inmem->f_info, SHR_E_MEMORY);

    sal_memset(inmem->f_info, 0,
              sizeof(bcmlrd_client_field_info_t) * num_fid);

    SHR_IF_ERR_EXIT(
        bcmlrd_table_fields_def_get(unit, table_name, num_fid,
                                    inmem->f_info, &num_fid));

    /* Find number of field count */
    f_info = inmem->f_info;
    for (i = 0; i < num_fid; i++) {
        count += f_info[i].elements;
    }

    /* Allocate the in and out fields */
    in = &inmem->fields[0];
    out = &inmem->fields[1];
    SHR_ALLOC(in->field, sizeof(bcmltd_field_t *) * count,
              "bcmptmCciControl");
    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);
    sal_memset(in->field, 0, sizeof(bcmltd_field_t *) * count);
    SHR_ALLOC(out->field, sizeof(bcmltd_field_t *) * count,
              "bcmptmCciControl");
    SHR_NULL_CHECK(out->field, SHR_E_MEMORY);
    sal_memset(out->field, 0, sizeof(bcmltd_field_t *) * count);
    for (i = 0; i < count; i++) {
        in->field[i] = shr_fmm_alloc();
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        sal_memset(in->field[i], 0, sizeof(bcmltd_field_t));
        out->field[i] = shr_fmm_alloc();
        SHR_NULL_CHECK(out->field[i], SHR_E_MEMORY);
        sal_memset(out->field[i], 0, sizeof(bcmltd_field_t));
    }

    /* Register the call back notificaton */
    sal_memset(&cb, 0, sizeof(cb));
    if ((stage_cb != NULL) || (validate_cb != NULL)) {
        cb.stage = stage_cb;
        cb.validate = validate_cb;

        SHR_IF_ERR_EXIT(
            bcmimm_lt_event_reg(unit, t_attrib.id, &cb, con));
    }

    inmem->num_fid = num_fid;
    inmem->count_fld = count;
    inmem->sid = t_attrib.id;

exit:
    if (SHR_FUNC_ERR()) {
        cci_config_cleanup(unit, con, inmem);
    }
    SHR_FUNC_EXIT();
}

/*!
 * In memory Configuration table record insert
 */
static int
cci_config_insert(int unit,
                  cci_context_t   *con)
{
    size_t num_fid;
    bcmlrd_client_field_info_t *f_info;
    bcmltd_fields_t *in;
    uint32_t i, id;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_INIT);

    in = &con->config.fields[0];
    num_fid = con->config.num_fid;
    f_info = con->config.f_info;

    /*
     * Logical and physical ports are mapped during initialization
     * Disable all the ports during init by default
     * Application will enable the ports on which counters need to be collected
     */
    id = 0;
    for (i = 0; i < num_fid; i++) {
        if (!sal_strcmp(f_info[i].name, "PORT_ID")) {
            uint32_t elem;
            for (elem = 0; elem < f_info[i].elements; elem++) {
                in->field[elem + id]->id = f_info[i].id;
                in->field[elem + id]->idx = elem;
                in->field[elem + id]->data = 0;
            }
            id += f_info[i].elements;
        }
    }
    in->count = id;
    rv = bcmimm_entry_insert(unit, con->config.sid, in);
    if ((rv == SHR_E_NOT_FOUND) || (rv == SHR_E_UNAVAIL)) {
        /* Table may not be avaialble, use default */
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * In memory event sync state table record insert
 */
static int
cci_event_sync_state_insert(int unit,
                            cci_context_t   *con)
{
    size_t num_fid;
    bcmlrd_client_field_info_t *f_info;
    bcmltd_fields_t *in;
    uint32_t i, id, key;
    cci_sync_event_state_t sync = OFF;
    config_inmem_t *state;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_INIT);

    state = &con->state;
    in = &state->fields[0];
    num_fid = state->num_fid;
    f_info = state->f_info;

    for (key = 0; key <=  CCI_CTR_TYPE_NUM; key++) {
        id = 0;
        for (i = 0; i < num_fid; i++) {
            in->field[id]->idx = 0;
            in->field[id]->id = f_info[i].id;
            if (f_info[i].key) {
                in->field[id]->data = key;
            } else {
                 in->field[id]->data = sync;
            }
            id += f_info[i].elements;
        }
        in->count = id;
        rv = bcmimm_entry_insert(unit, state->sid, in);
        if ((rv == SHR_E_NOT_FOUND) || (rv == SHR_E_UNAVAIL)) {
            /* Table may not be avaialble, use default */
            SHR_EXIT();
        } else {
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * In memory event sync state control
 * table record insert
 */
static int
cci_event_sync_state_ctrl_insert(int unit,
                                 cci_context_t   *con)
{
    size_t num_fid;
    bcmlrd_client_field_info_t *f_info;
    bcmltd_fields_t *in;
    uint32_t i, id, key;
    cci_sync_event_state_t sync = OFF;
    config_inmem_t *state_ctrl;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_INIT);

    state_ctrl = &con->state_ctrl;
    in = &state_ctrl->fields[0];
    num_fid = state_ctrl->num_fid;
    f_info = state_ctrl->f_info;

    for (key = 0; key <= CCI_CTR_TYPE_NUM; key++) {
        id = 0;
        for (i = 0; i < num_fid; i++) {
            in->field[id]->idx = 0;
            in->field[id]->id = f_info[i].id;
            if (f_info[i].key) {
                in->field[id]->data = key;
            } else {
                 in->field[id]->data = sync;
            }
            id += f_info[i].elements;
        }
        in->count = id;
        rv = bcmimm_entry_insert(unit, state_ctrl->sid, in);
        if ((rv == SHR_E_NOT_FOUND) || (rv == SHR_E_UNAVAIL)) {
            /* Table may not be avaialble, use default */
            SHR_EXIT();
        } else {
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 *  Get CCI Configuration information
 */
int
bcmptm_cci_config_get(int unit, cci_config_t *config)
{
    cci_context_t   *con;
    uint32_t i, j;
    uint32_t id = 0;
    size_t num_fid;
    bcmlrd_client_field_info_t *f_info;
    bcmltd_fields_t *in;
    bcmltd_fields_t *out;
    cci_pol_cmd_t cmd;

    SHR_FUNC_ENTER(unit);
    con = (cci_context_t *)bcmptm_cci_context_get(unit);
    SHR_NULL_CHECK(con, SHR_E_INIT);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    sal_memset(config, 0, sizeof(cci_config_t));
    config->nthreads = 1;

    if (bcmptm_pt_cci_ctr_col_dma_supported(unit)) {
        config->dma = 1;
    } else {
        config->dma = 0;
    }

    if (con->init_config) {
        in = &con->config.fields[0];
        out = &con->config.fields[1];
        num_fid = con->config.num_fid;
        f_info = con->config.f_info;
        in->count = 1;
        for (i = 0; i < num_fid; i++) {
            /* Look up for key value */
            /* Set the default field values */
            for (j = 0; j < f_info[i].elements; j++) {
                uint64_t data = 0;
                /* max data size is 64 bits */
                switch (f_info[i].width) {
                case 1:
                   data = f_info[i].def.is_true;
                break;
                case 8:
                   data = f_info[i].def.u8;
                break;
                case 16:
                   data = f_info[i].def.u16;
                break;
                case 32:
                   data = f_info[i].def.u32;
                break;
                case 64:
                   data = f_info[i].def.u64;
                break;
                default:
                   data = f_info[i].def.u32;
                }
                SHR_IF_ERR_EXIT(
                    cci_config_get(unit, con, f_info[i].id, j, data,
                                   &cmd, config));
           }
        }

        out->count =  con->config.count_fld;
        /* look up fields and fill information */
        SHR_IF_ERR_EXIT(
            bcmimm_entry_lookup(unit, con->config.sid, in , out));

        for (i = 0; i < out->count; i++) {
            id = out->field[i]->id;
            SHR_IF_ERR_EXIT(
                cci_config_get(unit, con, id,
                               out->field[i]->idx,
                               out->field[i]->data,
                               &cmd, config));
        }
    } else {
        /* Configurtion table doesn't exist hardcode */
        bcmdrd_pbmp_t pbmp;
        uint32_t port;
        config->col_enable = 1;
        config->interval = 1000000;
        config->multiplier[CCI_CTR_TYPE_PORT] = 1;
        config->multiplier[CCI_CTR_TYPE_IPIPE] = 1;
        config->multiplier[CCI_CTR_TYPE_TM] = 1;
        config->multiplier[CCI_CTR_TYPE_EPIPE] = 1;
        config->multiplier[CCI_CTR_TYPE_EVICT] = 1;
        if (bcmdrd_feature_is_sim(unit)) {
            port = NUM_SIM_PORTS;
        } else {
            port = 135;
        }
        BCMDRD_PBMP_CLEAR(pbmp);
        for (i = 1; i < port ; i++) {
            BCMDRD_PBMP_PORT_ADD(pbmp, i);
        }
    }
exit:
    SHR_FUNC_EXIT();

}

/*
 * Start sync the counter stats in cache.
 */
int
bcmptm_cci_stat_sync_start(int unit, int type)
{
    cci_context_t   *con;
    col_thread_msg_t msg;
    cci_handle_t hpol;

    SHR_FUNC_ENTER(unit);

    con = (cci_context_t *)bcmptm_cci_context_get(unit);
    SHR_NULL_CHECK(con, SHR_E_INIT);

    /* Get polling collection context */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_poll_handle_get(unit, con->hcol, &hpol));

    msg.cmd = MSG_CMD_POL_STAT_SYNC;
    msg.data[0] = type;

    SHR_IF_ERR_EXIT
        (bcmptm_cci_col_poll_msg(unit, &msg, hpol));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Complete sync the counter stats in cache.
 */
int
bcmptm_cci_stat_sync_complete(int unit, int type)
{
    cci_context_t   *con;

    SHR_FUNC_ENTER(unit);

    con = (cci_context_t *)bcmptm_cci_context_get(unit);
    SHR_NULL_CHECK(con, SHR_E_INIT);

    /* Update the sync state to START */
    SHR_IF_ERR_EXIT
        (cci_event_sync_state_update(unit, con, type, COMPLETE));
    con->sync_state[type] = COMPLETE;
exit:
    SHR_FUNC_EXIT();
}


/*
 * Initialize imm
 */

int bcmptm_cci_imm_init(int unit,
                        cci_context_t   *con)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_cci_imm_config_init(unit, con, CTR_CFG_LT_NM, &con->config,
             cci_lt_stage_cb, cci_lt_validate_cb));

    SHR_IF_ERR_EXIT
        (bcmptm_cci_imm_config_init(unit, con, CTR_SYNC_STATE_LT_NM, &con->state,
             NULL, NULL));

    SHR_IF_ERR_EXIT
        (bcmptm_cci_imm_config_init(unit, con, CTR_SYNC_STATE_CTRL_LT_NM, &con->state_ctrl,
         cci_lt_sync_state_control_stage_cb,
         cci_lt_sync_state_control_validate_cb));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Insert entires in imm
 */

int
bcmptm_cci_imm_insert(int unit,
                      cci_context_t   *con)
{
   SHR_FUNC_ENTER(unit);

   SHR_IF_ERR_EXIT
       (cci_config_insert(unit, con));
   SHR_IF_ERR_EXIT
       (cci_event_sync_state_insert(unit, con));
   SHR_IF_ERR_EXIT
       (cci_event_sync_state_ctrl_insert(unit, con));

exit:
    SHR_FUNC_EXIT();
}

