/*! \file bcmcth_oam_bfd_control.c
 *
 * BCMCTH OAM BFD APIs for global LTs.
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
#include <bcmlrd/bcmlrd_client.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_oam_imm.h>
#include <bcmcth/bcmcth_oam_drv.h>
#include <bcmcth/bcmcth_oam_bfd.h>
#include <bcmcth/bcmcth_oam_bfd_uc.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_OAM

/*******************************************************************************
 * Private Functions
 */
/* Pre-populate the BFD stats table. */
static int
bfd_stats_prepopulate(int unit, bcmltd_sid_t sid)
{
    bcmltd_fields_t fields = {0};
    size_t num_fields;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_num_data_fields_get(unit, OAM_BFD_STATSt, &num_fields));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_fields_alloc(unit, num_fields, &fields));

    /* Fill up zeros for all the stats */
    for (idx = 0; idx < num_fields; idx++) {
        fields.field[idx]->id = idx;
        fields.field[idx]->data = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_insert(unit, OAM_BFD_STATSt, &fields));

exit:
    bcmcth_oam_bfd_fields_free(unit, num_fields, &fields);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * BFD Control handlers
 */
static int
bfd_control_field_get(int unit,
                      const bcmltd_field_t *field,
                      bcmcth_oam_bfd_control_t *entry)
{
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case OAM_BFD_CONTROLt_BFDf:
            entry->bfd = fval;
            break;

        case OAM_BFD_CONTROLt_MAX_ENDPOINTSf:
            entry->max_endpoints = fval;
            break;

        case OAM_BFD_CONTROLt_MAX_ENDPOINTS_OPERf:
            entry->oper.max_endpoints = fval;
            break;

        case OAM_BFD_CONTROLt_MAX_AUTH_SHA1_KEYSf:
            entry->max_auth_sha1_keys = fval;
            break;

        case OAM_BFD_CONTROLt_MAX_AUTH_SHA1_KEYS_OPERf:
            entry->oper.max_auth_sha1_keys = fval;
            break;

        case OAM_BFD_CONTROLt_MAX_AUTH_SIMPLE_PASSWORD_KEYSf:
            entry->max_auth_sp_keys = fval;
            break;

        case OAM_BFD_CONTROLt_MAX_AUTH_SIMPLE_PASSWORD_KEYS_OPERf:
            entry->oper.max_auth_sp_keys = fval;
            break;

        case OAM_BFD_CONTROLt_MAX_PKT_SIZEf:
            entry->max_pkt_size = fval;
            break;

        case OAM_BFD_CONTROLt_MAX_PKT_SIZE_OPERf:
            entry->oper.max_pkt_size = fval;
            break;

        case OAM_BFD_CONTROLt_INTERNAL_LOCAL_DISCRIMINATORf:
            entry->internal_local_discr = fval;
            break;

        case OAM_BFD_CONTROLt_INTERNAL_LOCAL_DISCRIMINATOR_OPERf:
            entry->oper.internal_local_discr = fval;
            break;

        case OAM_BFD_CONTROLt_STATIC_REMOTE_DISCRIMINATORf:
            entry->static_remote_discr = fval;
            break;

        case OAM_BFD_CONTROLt_STATIC_REMOTE_DISCRIMINATOR_OPERf:
            entry->oper.static_remote_discr = fval;
            break;

        case OAM_BFD_CONTROLt_LOCAL_STATE_DOWN_EVENT_ON_ENDPOINT_CREATIONf:
            entry->local_state_down_event_on_endpoint_creation = fval;
            break;

        case OAM_BFD_CONTROLt_CONTROL_PLANE_INDEPENDENCEf:
            entry->control_plane_independence = fval;
            break;

        case OAM_BFD_CONTROLt_TXf:
            entry->tx = fval;
            break;

        case OAM_BFD_CONTROLt_OPERATIONAL_STATEf:
            entry->oper.state = fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_BITSET(entry->fbmp, fid);

exit:
    SHR_FUNC_EXIT();
}

/* Get a BFD control entry with default values for all the fields. */
int
bcmcth_oam_bfd_control_defaults_get(int unit,
                                    bcmcth_oam_bfd_control_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t fld_num;
    size_t actual_fld_num;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_num_data_fields_get(unit, OAM_BFD_CONTROLt,
                                            &fld_num));

    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * fld_num,
              "bcmcthOamBfdControlFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
       (bcmlrd_table_fields_def_get(unit, "OAM_BFD_CONTROL",
                                    fld_num, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        switch (fields_info[idx].id) {
            case OAM_BFD_CONTROLt_BFDf:
                entry->bfd = fields_info[idx].def.is_true;
                break;

            case OAM_BFD_CONTROLt_MAX_ENDPOINTSf:
                entry->max_endpoints = fields_info[idx].def.u32;
                break;

            case OAM_BFD_CONTROLt_MAX_AUTH_SHA1_KEYSf:
                entry->max_auth_sha1_keys = fields_info[idx].def.u32;
                break;

            case OAM_BFD_CONTROLt_MAX_AUTH_SIMPLE_PASSWORD_KEYSf:
                entry->max_auth_sp_keys = fields_info[idx].def.u32;
                break;

            case OAM_BFD_CONTROLt_MAX_PKT_SIZEf:
                entry->max_pkt_size = fields_info[idx].def.u16;
                break;

            case OAM_BFD_CONTROLt_INTERNAL_LOCAL_DISCRIMINATORf:
                entry->internal_local_discr = fields_info[idx].def.is_true;
                break;

            case OAM_BFD_CONTROLt_STATIC_REMOTE_DISCRIMINATORf:
                entry->static_remote_discr = fields_info[idx].def.is_true;
                break;

            case OAM_BFD_CONTROLt_LOCAL_STATE_DOWN_EVENT_ON_ENDPOINT_CREATIONf:
                entry->local_state_down_event_on_endpoint_creation =
                                                   fields_info[idx].def.is_true;
                break;

            case OAM_BFD_CONTROLt_CONTROL_PLANE_INDEPENDENCEf:
                entry->control_plane_independence = fields_info[idx].def.is_true;
                break;

            case OAM_BFD_CONTROLt_TXf:
                entry->tx = fields_info[idx].def.is_true;
                break;

            case OAM_BFD_CONTROLt_MAX_ENDPOINTS_OPERf:
            case OAM_BFD_CONTROLt_MAX_AUTH_SHA1_KEYS_OPERf:
            case OAM_BFD_CONTROLt_MAX_AUTH_SIMPLE_PASSWORD_KEYS_OPERf:
            case OAM_BFD_CONTROLt_OPERATIONAL_STATEf:
            case OAM_BFD_CONTROLt_MAX_PKT_SIZE_OPERf:
            case OAM_BFD_CONTROLt_STATIC_REMOTE_DISCRIMINATOR_OPERf:
            case OAM_BFD_CONTROLt_INTERNAL_LOCAL_DISCRIMINATOR_OPERf:
                /* Don't get the defaults for the read-only fields. */
                break;

            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

/* Get the OAM_BFD_CONTROL entry currently in IMM. */
int
bcmcth_oam_bfd_control_entry_get(int unit, bcmcth_oam_bfd_control_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    const bcmltd_field_t *field;
    size_t num_fields;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_num_data_fields_get(unit, OAM_BFD_CONTROLt,
                                            &num_fields));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_fields_alloc(unit, num_fields, &out));

    in.count = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, OAM_BFD_CONTROLt, &in, &out));

    sal_memset(entry, 0, sizeof(*entry));

    /* Get the defaults for all the fields. Some of these may get overwritten
     * with the values from IMM.
     */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_control_defaults_get(unit, entry));

    for (idx = 0; idx < out.count; idx++) {
        field = out.field[idx];
        SHR_IF_ERR_VERBOSE_EXIT(bfd_control_field_get(unit, field, entry));
    }

exit:
    bcmcth_oam_bfd_fields_free(unit, num_fields, &out);
    SHR_FUNC_EXIT();
}

/*
 * Convert the user passed fields in the stage callback handler to a structure.
 */
static int
bfd_control_stage_fields_parse(int unit,
                               bcmimm_entry_event_t event,
                               const bcmltd_field_t *data,
                               bcmcth_oam_bfd_control_t *entry)
{
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    if ((event != BCMIMM_ENTRY_INSERT) && (event != BCMIMM_ENTRY_UPDATE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (event == BCMIMM_ENTRY_INSERT) {
        /*
         * Get the defaults for all the fields. Some of these may get
         * overwritten with the values from user.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_oam_bfd_control_defaults_get(unit, entry));
    } else {
        /* Get the current view of IMM if it is an update operation. */
        SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_control_entry_get(unit, entry));
    }


    /*
     * Clear off the fbmp, so that it will be set with the user programmed
     * fields.
     */
    sal_memset(&(entry->fbmp), 0, sizeof(entry->fbmp));

    field = data;
    while (field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(bfd_control_field_get(unit, field, entry));
        field = field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static void
bfd_control_stage_out_fields_populate(int unit,
                                      bcmcth_oam_bfd_control_oper_t *oper,
                                      bcmltd_fields_t *output_fields)
{
    int count = 0;

    output_fields->field[count]->id = OAM_BFD_CONTROLt_MAX_ENDPOINTS_OPERf;
    output_fields->field[count]->data = oper->max_endpoints;
    count++;

    output_fields->field[count]->id = OAM_BFD_CONTROLt_MAX_AUTH_SHA1_KEYS_OPERf;
    output_fields->field[count]->data = oper->max_auth_sha1_keys;
    count++;

    output_fields->field[count]->id =
        OAM_BFD_CONTROLt_MAX_AUTH_SIMPLE_PASSWORD_KEYS_OPERf;
    output_fields->field[count]->data = oper->max_auth_sp_keys;
    count++;

    output_fields->field[count]->id = OAM_BFD_CONTROLt_MAX_PKT_SIZE_OPERf;
    output_fields->field[count]->data = oper->max_pkt_size;
    count++;

    output_fields->field[count]->id =
        OAM_BFD_CONTROLt_INTERNAL_LOCAL_DISCRIMINATOR_OPERf;
    output_fields->field[count]->data = oper->internal_local_discr;
    count++;

    output_fields->field[count]->id =
        OAM_BFD_CONTROLt_STATIC_REMOTE_DISCRIMINATOR_OPERf;
    output_fields->field[count]->data = oper->static_remote_discr;
    count++;

    output_fields->field[count]->id = OAM_BFD_CONTROLt_OPERATIONAL_STATEf;
    output_fields->field[count]->data = oper->state;
    count++;

    output_fields->count = count;
}

static int
bfd_control_stage(int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  bcmimm_entry_event_t event,
                  const bcmltd_field_t *key,
                  const bcmltd_field_t *data,
                  void *context,
                  bcmltd_fields_t *output_fields)
{
    bcmcth_oam_bfd_control_t old_entry, new_entry;
    bcmcth_oam_bfd_control_oper_t oper;

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_control_stage_fields_parse(unit, BCMIMM_ENTRY_INSERT,
                                                data, &new_entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_oam_bfd_control_insert(unit, &new_entry, &oper));
            break;

        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_oam_bfd_control_entry_get(unit, &old_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_control_stage_fields_parse(unit, BCMIMM_ENTRY_UPDATE,
                                                data, &new_entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_oam_bfd_control_update(unit, &new_entry,
                                               &old_entry, &oper));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_control_delete(unit));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        /* Convert the read-only output fields to IMM format. */
        bfd_control_stage_out_fields_populate(unit, &oper, output_fields);
    }

exit:
    SHR_FUNC_EXIT();
}

/* OAM_BFD_CONTROL IMM event callback structure. */
static bcmimm_lt_cb_t bfd_control_hdl = {
    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bfd_control_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};

static int
bfd_stats_stage_fields_parse(int unit, const bcmltd_field_t *data,
                             bcmcth_oam_bfd_stats_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case OAM_BFD_STATSt_RX_PKT_VERSION_ERRORf:
                entry->rx_pkt_version_error = fval;
                break;

            case OAM_BFD_STATSt_RX_PKT_LENGTH_ERRORf:
                entry->rx_pkt_length_error = fval;
                break;

            case OAM_BFD_STATSt_RX_PKT_ZERO_DETECT_MULTIPLIERf:
                entry->rx_pkt_zero_detect_multiplier = fval;
                break;

            case OAM_BFD_STATSt_RX_PKT_ZERO_MY_DISCRIMINATORf:
                entry->rx_pkt_zero_my_discr = fval;
                break;

            case OAM_BFD_STATSt_RX_PKT_P_AND_F_BITS_SETf:
                entry->rx_pkt_p_and_f_bits_set = fval;
                break;

            case OAM_BFD_STATSt_RX_PKT_M_BIT_SETf:
                entry->rx_pkt_m_bit_set = fval;
                break;

            case OAM_BFD_STATSt_RX_PKT_AUTH_MISMATCHf:
                entry->rx_pkt_auth_mismatch = fval;
                break;

            case OAM_BFD_STATSt_RX_PKT_AUTH_SIMPLE_PASSWORD_ERRORf:
                entry->rx_pkt_sp_auth_error = fval;
                break;

            case OAM_BFD_STATSt_RX_PKT_AUTH_SHA1_ERRORf:
                entry->rx_pkt_sha1_auth_error = fval;
                break;

            case OAM_BFD_STATSt_RX_PKT_ENDPOINT_NOT_FOUNDf:
                entry->rx_pkt_endpoint_not_found = fval;
                break;

            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_BITSET(entry->fbmp, fid);
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static void
bfd_stats_populate(bcmcth_oam_bfd_stats_t *entry,
                   bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case OAM_BFD_STATSt_RX_PKT_VERSION_ERRORf:
                *data = entry->rx_pkt_version_error;
                break;

            case OAM_BFD_STATSt_RX_PKT_LENGTH_ERRORf:
                *data = entry->rx_pkt_length_error;
                break;

            case OAM_BFD_STATSt_RX_PKT_ZERO_DETECT_MULTIPLIERf:
                *data = entry->rx_pkt_zero_detect_multiplier;
                break;

            case OAM_BFD_STATSt_RX_PKT_ZERO_MY_DISCRIMINATORf:
                *data = entry->rx_pkt_zero_my_discr;
                break;

            case OAM_BFD_STATSt_RX_PKT_P_AND_F_BITS_SETf:
                *data = entry->rx_pkt_p_and_f_bits_set;
                break;

            case OAM_BFD_STATSt_RX_PKT_M_BIT_SETf:
                *data = entry->rx_pkt_m_bit_set;
                break;

            case OAM_BFD_STATSt_RX_PKT_AUTH_MISMATCHf:
                *data = entry->rx_pkt_auth_mismatch;
                break;

            case OAM_BFD_STATSt_RX_PKT_AUTH_SIMPLE_PASSWORD_ERRORf:
                *data = entry->rx_pkt_sp_auth_error;
                break;

            case OAM_BFD_STATSt_RX_PKT_AUTH_SHA1_ERRORf:
                *data = entry->rx_pkt_sha1_auth_error;
                break;

            case OAM_BFD_STATSt_RX_PKT_ENDPOINT_NOT_FOUNDf:
                *data = entry->rx_pkt_endpoint_not_found;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }
}

static int
bfd_stats_stage(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                bcmimm_entry_event_t event,
                const bcmltd_field_t *key,
                const bcmltd_field_t *data,
                void *context,
                bcmltd_fields_t *output_fields)
{
    bcmcth_oam_bfd_stats_t entry;

    SHR_FUNC_ENTER(unit);

    if (event != BCMIMM_ENTRY_UPDATE) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (output_fields) {
        output_fields->count = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT(bfd_stats_stage_fields_parse(unit, data, &entry));

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_stats_update(unit, &entry));

exit:
    SHR_FUNC_EXIT();
}


static int
bfd_stats_lookup(int unit,
                 bcmltd_sid_t sid,
                 uint32_t trans_id,
                 void *context,
                 bcmimm_lookup_type_t lkup_type,
                 const bcmltd_fields_t *in,
                 bcmltd_fields_t *out)
{
    bcmcth_oam_bfd_stats_t entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_stats_lookup(unit, &entry));

    bfd_stats_populate(&entry, out);

exit:
    SHR_FUNC_EXIT();
}

/* OAM_BFD_STATS IMM event callback structure. */
static bcmimm_lt_cb_t bfd_stats_hdl = {
    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bfd_stats_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = bfd_stats_lookup
};

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_oam_bfd_control_imm_register(int unit, bool warm)
{
    bcmcth_oam_bfd_imm_reg_t imm_reg[] = {
        {
            OAM_BFD_CONTROLt,
            NULL,
            &bfd_control_hdl
        },
        {
            OAM_BFD_STATSt,
            bfd_stats_prepopulate,
            &bfd_stats_hdl
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
