/*! \file bcmcth_oam_bfd_event.c
 *
 * BCMCTH OAM BFD APIs for event LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmissu/issu_api.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_oam_imm.h>
#include <bcmcth/bcmcth_oam_drv.h>
#include <bcmcth/bcmcth_oam_util.h>

#include <bcmcth/bcmcth_oam_bfd.h>
#include <bcmcth/bcmcth_oam_bfd_uc.h>
#include <bcmcth/generated/oam_bfd_ha.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_OAM

typedef struct bcmcth_oam_bfd_event_report_s {
    /* Persistent event report information stored in HA memory. */
    bcmcth_oam_bfd_event_report_ha_t *ha;

    /* OAM_BFD_EVENT fields. */
    bcmltd_fields_t event_fields;

    /* OAM_BFD_EVENT_STATUS fields. */
    bcmltd_fields_t status_fields;
} bcmcth_oam_bfd_event_report_t;

static
bcmcth_oam_bfd_event_report_t *g_event_report[BCMDRD_CONFIG_MAX_UNITS] = {0};

/*******************************************************************************
 * Private Functions
 */
static int
bfd_event_control_prepopulate(int unit, bcmltd_sid_t sid)
{
    bcmltd_fields_t fields = {0};
    size_t num_fields;
    int idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_num_data_fields_get(unit, OAM_BFD_EVENT_CONTROLt,
                                            &num_fields));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_fields_alloc(unit, num_fields, &fields));

    /* Disable all the events. */
    for (idx = 0; idx < OAM_BFD_EVENT_CONTROLt_FIELD_COUNT; idx++) {
        fields.field[idx]->id = idx;
        fields.field[idx]->data = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_insert(unit, OAM_BFD_EVENT_CONTROLt, &fields));

exit:
    bcmcth_oam_bfd_fields_free(unit, num_fields, &fields);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * IMM event handlers.
 */
static int
bfd_event_control_field_get(int unit,
                            const bcmltd_field_t *field,
                            bcmcth_oam_bfd_event_control_t *entry)
{
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case OAM_BFD_EVENT_CONTROLt_LOCAL_STATE_ADMIN_DOWNf:
            entry->local_state_admin_down = fval;
            break;

        case OAM_BFD_EVENT_CONTROLt_LOCAL_STATE_DOWNf:
            entry->local_state_down = fval;
            break;

        case OAM_BFD_EVENT_CONTROLt_LOCAL_STATE_INITf:
            entry->local_state_init = fval;
            break;

        case OAM_BFD_EVENT_CONTROLt_LOCAL_STATE_UPf:
            entry->local_state_up = fval;
            break;

        case OAM_BFD_EVENT_CONTROLt_REMOTE_STATE_MODE_CHANGEf:
            entry->remote_state_mode_change = fval;
            break;

        case OAM_BFD_EVENT_CONTROLt_REMOTE_DISCRIMINATOR_CHANGEf:
            entry->remote_discr_change = fval;
            break;

        case OAM_BFD_EVENT_CONTROLt_REMOTE_PARAMETER_CHANGEf:
            entry->remote_param_change = fval;
            break;

        case OAM_BFD_EVENT_CONTROLt_REMOTE_POLL_BIT_SETf:
            entry->remote_poll_bit_set = fval;
            break;

        case OAM_BFD_EVENT_CONTROLt_REMOTE_FINAL_BIT_SETf:
            entry->remote_final_bit_set = fval;
            break;

        case OAM_BFD_EVENT_CONTROLt_MISCONNECTIVITY_DEFECTf:
            entry->misconn_defect = fval;
            break;

        case OAM_BFD_EVENT_CONTROLt_MISCONNECTIVITY_DEFECT_CLEARf:
            entry->misconn_clear = fval;
            break;

        case OAM_BFD_EVENT_CONTROLt_UNEXPECTED_MEG_DEFECTf:
            entry->unexpected_meg_defect = fval;
            break;

        case OAM_BFD_EVENT_CONTROLt_UNEXPECTED_MEG_DEFECT_CLEARf:
            entry->unexpected_meg_defect_clear = fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bfd_event_control_entry_get(int unit, bcmcth_oam_bfd_event_control_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    const bcmltd_field_t *field;
    size_t num_fields;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_num_data_fields_get(unit, OAM_BFD_EVENT_CONTROLt,
                                            &num_fields));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_fields_alloc(unit, num_fields, &out));

    in.count = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, OAM_BFD_EVENT_CONTROLt, &in, &out));

    sal_memset(entry, 0, sizeof(*entry));

    for (idx = 0; idx < out.count; idx++) {
        field = out.field[idx];
        SHR_IF_ERR_VERBOSE_EXIT
            (bfd_event_control_field_get(unit, field, entry));
    }

exit:
    bcmcth_oam_bfd_fields_free(unit, num_fields, &out);
    SHR_FUNC_EXIT();
}

static int
bfd_event_control_stage_fields_parse(int unit,
                                     const bcmltd_field_t *data,
                                     bcmcth_oam_bfd_event_control_t *entry)
{
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    /* Get the values already in IMM. */
    SHR_IF_ERR_VERBOSE_EXIT(bfd_event_control_entry_get(unit, entry));

    /* Overwrite the modified values. */
    field = data;
    while (field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bfd_event_control_field_get(unit, field, entry));
        field = field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bfd_event_control_stage(int unit,
                        bcmltd_sid_t sid,
                        uint32_t trans_id,
                        bcmimm_entry_event_t event,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        void *context,
                        bcmltd_fields_t *output_fields)
{
    bcmcth_oam_bfd_event_control_t entry;

    SHR_FUNC_ENTER(unit);

    if (event != BCMIMM_ENTRY_UPDATE) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_event_control_stage_fields_parse(unit, data, &entry));

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_event_control_update(unit, &entry));

exit:
    SHR_FUNC_EXIT();
}

/* OAM_BFD_EVENT_CONTROL IMM event callback structure. */
static bcmimm_lt_cb_t bfd_event_control_hdl = {
    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bfd_event_control_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};

/*******************************************************************************
 * Public Functions
 */

void
bcmcth_oam_bfd_event_report(int unit,
                            bcmltd_common_oam_bfd_endpoint_type_t_t ep_type,
                            uint32_t endpoint_id,
                            int num_events,
                            bcmltd_common_oam_bfd_endpoint_event_t_t *events)
{
    bcmcth_oam_bfd_event_report_t *event_report = g_event_report[unit];
    bcmltd_fields_t *fields;
    uint32_t fid;
    uint32_t last_idx;
    int rv;
    int i;

    if (num_events < 1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid number of events %d \n"),
                   num_events));
        return;
    }

    switch (ep_type) {
        case BCMLTD_COMMON_OAM_BFD_ENDPOINT_TYPE_T_T_IPV4:
            fid = OAM_BFD_EVENTt_OAM_BFD_IPV4_ENDPOINT_IDf;
            break;

        case BCMLTD_COMMON_OAM_BFD_ENDPOINT_TYPE_T_T_IPV6:
            fid = OAM_BFD_EVENTt_OAM_BFD_IPV6_ENDPOINT_IDf;
            break;

        case BCMLTD_COMMON_OAM_BFD_ENDPOINT_TYPE_T_T_TNL_IPV4:
            fid = OAM_BFD_EVENTt_OAM_BFD_TNL_IPV4_ENDPOINT_IDf;
            break;

        case BCMLTD_COMMON_OAM_BFD_ENDPOINT_TYPE_T_T_TNL_IPV6:
            fid = OAM_BFD_EVENTt_OAM_BFD_TNL_IPV6_ENDPOINT_IDf;
            break;

        case BCMLTD_COMMON_OAM_BFD_ENDPOINT_TYPE_T_T_TNL_MPLS:
            fid = OAM_BFD_EVENTt_OAM_BFD_TNL_MPLS_ENDPOINT_IDf;
            break;

        default:
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Unknown endpoint type %d \n"),
                       ep_type));
            return;
    }

    if (event_report->ha->max_idx == 0) {
        /*
         * First event being reported, get the depth of the OAM_BFD_EVENT table.
         */
        rv = bcmlrd_map_table_attr_get(unit, OAM_BFD_EVENTt,
                                       BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT,
                                       &(event_report->ha->max_idx));
        if (rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "bcmlrd_map_table_attr_get returned %d \n"),
                       rv));
        }
    }

    fields = &(event_report->event_fields);

    /* Clear off the previus entry. */
    for (i = 0; i < OAM_BFD_EVENTt_FIELD_COUNT; i++) {
        fields->field[0]->id = i;
        fields->field[0]->data = 0;
    }
    for (i = 0; i < num_events; i++) {
        /* Program the index at which to write the event. */
        fields->field[0]->id = OAM_BFD_EVENTt_OAM_BFD_EVENT_IDf;
        fields->field[0]->data = event_report->ha->next_idx;

        /* Endpoint Type. */
        fields->field[1]->id = OAM_BFD_EVENTt_ENDPOINT_TYPEf;
        fields->field[1]->data = ep_type;

        /* Endpoint Id. */
        fields->field[2]->id = fid;
        fields->field[2]->data = endpoint_id;

        /* Event. */
        fields->field[3]->id = OAM_BFD_EVENTt_EVENTf;
        fields->field[3]->data = events[i];

        if (event_report->ha->wraparound == true) {
            rv = bcmimm_entry_update(unit, true, OAM_BFD_EVENTt, fields);
            if (rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "bcmimm_entry_update returned %d \n"),
                           rv));
                return;
            }
        } else {
            rv = bcmimm_entry_insert(unit, OAM_BFD_EVENTt, fields);
            if (rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "bcmimm_entry_insert returned %d \n"),
                           rv));
                return;
            }
        }

        last_idx = event_report->ha->next_idx;
        event_report->ha->next_idx = (event_report->ha->next_idx + 1) % event_report->ha->max_idx;
        if (event_report->ha->next_idx == 0) {
            event_report->ha->wraparound = true;
        }
    }

    /* Program the last writen index to OAM_BFD_EVENT_STATUS table. */
    fields = &(event_report->status_fields);
    fields->field[0]->id = OAM_BFD_EVENT_STATUSt_OAM_BFD_EVENT_IDf;
    fields->field[0]->data = last_idx;

    if (event_report->ha->status_create == false) {
        /*
         * First event being reported, insert into OAM_BFD_EVENT_STATUS table.
         */
        rv = bcmimm_entry_insert(unit, OAM_BFD_EVENT_STATUSt, fields);
        if (rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "bcmimm_entry_insert returned %d \n"),
                       rv));
            return;
        }
        event_report->ha->status_create = true;
    } else {
        rv = bcmimm_entry_update(unit, true, OAM_BFD_EVENT_STATUSt, fields);
        if (rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "bcmimm_entry_update returned %d \n"),
                       rv));
            return;
        }
    }
}

int
bcmcth_oam_bfd_event_report_init(int unit, bool warm)
{
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    bcmcth_oam_bfd_event_report_t *event_report;
    bool event_alloc = false;
    bool event_status_alloc = false;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(g_event_report[unit],
              sizeof(bcmcth_oam_bfd_event_report_t),
              "bcmcthOAmBfdEventReport");
    event_report = g_event_report[unit];
    sal_memset(event_report, 0, sizeof(bcmcth_oam_bfd_event_report_t));

    ha_req_size = sizeof(bcmcth_oam_bfd_event_report_ha_t);
    ha_alloc_size = ha_req_size;

    event_report->ha = shr_ha_mem_alloc(unit, BCMMGMT_OAM_COMP_ID,
                                        BCMOAM_BFD_EVENT_REPORT_SUB_COMP_ID,
                                        "oamBfdEvRep",
                                        &ha_alloc_size);
    SHR_NULL_CHECK(g_event_report[unit], SHR_E_MEMORY);
    event_report = g_event_report[unit];

    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(event_report->ha, 0, ha_alloc_size);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_MON_COMP_ID,
                                        BCMOAM_BFD_CTRL_SUB_COMP_ID, 0,
                                        ha_req_size, 1,
                                        BCMCTH_OAM_BFD_EVENT_REPORT_HA_T_ID));
    }

    /*
     * Allocate memory for the OAM_BFD_EVENT OAM_BFD_EVENT_STATUS fields to
     * avoid allocation on every event report.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_fields_alloc(unit, OAM_BFD_EVENTt_FIELD_COUNT,
                                     &(event_report->event_fields)));
    event_alloc = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_fields_alloc(unit, OAM_BFD_EVENT_STATUSt_FIELD_COUNT,
                                     &(event_report->status_fields)));
    event_status_alloc = true;

 exit:
    if (SHR_FUNC_ERR()) {
        if (g_event_report[unit] != NULL) {
            if (g_event_report[unit]->ha != NULL) {
                shr_ha_mem_free(unit, g_event_report[unit]->ha);
                g_event_report[unit]->ha = NULL;
            }
            if (event_alloc == true) {
                bcmcth_oam_bfd_fields_free(unit, OAM_BFD_EVENTt_FIELD_COUNT,
                                           &(event_report->event_fields));
            }
            /* coverity[dead_error_line : FALSE] */
            if (event_status_alloc == true) {
                bcmcth_oam_bfd_fields_free(unit,
                                           OAM_BFD_EVENT_STATUSt_FIELD_COUNT,
                                           &(event_report->status_fields));
            }
            SHR_FREE(g_event_report[unit]);
        }
    }
    SHR_FUNC_EXIT();
}

void
bcmcth_oam_bfd_event_report_deinit(int unit)
{
    bcmcth_oam_bfd_event_report_t *event_report = g_event_report[unit];
    int rv;

    if (event_report == NULL) {
        return;
    }

    rv = bcmcth_oam_bfd_fields_free(unit, OAM_BFD_EVENTt_FIELD_COUNT,
                                    &(event_report->event_fields));
    if (rv != SHR_E_NONE) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "bcmcth_oam_bfd_fields_free returned %d \n"),
                   rv));
        return;
    }

    rv = bcmcth_oam_bfd_fields_free(unit, OAM_BFD_EVENT_STATUSt_FIELD_COUNT,
                                    &(event_report->status_fields));
    if (rv != SHR_E_NONE) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "bcmcth_oam_bfd_fields_free returned %d \n"),
                   rv));
        return;
    }

    SHR_FREE(g_event_report[unit]);
}

int
bcmcth_oam_bfd_event_imm_register(int unit, bool warm)
{
    bcmcth_oam_bfd_imm_reg_t imm_reg[] = {
        {
            OAM_BFD_EVENT_CONTROLt,
            bfd_event_control_prepopulate,
            &bfd_event_control_hdl
        }
    };

    int num_sids;

    SHR_FUNC_ENTER(unit);

    num_sids = sizeof(imm_reg) / sizeof(bcmcth_oam_bfd_imm_reg_t);

    /* Register the IMM callbacks. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_imm_cb_register(unit, warm, num_sids, imm_reg));

 exit:
    SHR_FUNC_EXIT();
}
