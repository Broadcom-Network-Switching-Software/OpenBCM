/*! \file bcmcth_oam_bfd_auth.c
 *
 * BCMCTH OAM BFD APIs for authentication LTs.
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
 * Simple password IMM event handlers
 */
static int
bfd_auth_sp_field_get(int unit, const bcmltd_field_t *field,
                      bcmcth_oam_bfd_auth_sp_t *entry)
{
    uint32_t fid;
    uint64_t fval;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    fid  = field->id;
    fval = field->data;
    idx  = field->idx;

    switch (fid) {
        case OAM_BFD_AUTH_SIMPLE_PASSWORDt_PASSWORD_LENf:
            entry->password_len = fval;
            break;

        case OAM_BFD_AUTH_SIMPLE_PASSWORDt_PASSWORDf:
            entry->password[idx] = fval;
            break;

        case OAM_BFD_AUTH_SIMPLE_PASSWORDt_OPERATIONAL_STATEf:
            entry->oper_state = fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_auth_sp_stage_fields_parse(int unit,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               bcmcth_oam_bfd_auth_sp_t *entry)
{
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    entry->id = key->data;

    field = data;
    while (field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(bfd_auth_sp_field_get(unit, field, entry));
        field = field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bfd_auth_sp_stage(int unit,
                  bcmltd_sid_t sid,
                  uint32_t trans_id,
                  bcmimm_entry_event_t event,
                  const bcmltd_field_t *key,
                  const bcmltd_field_t *data,
                   void *context,
                  bcmltd_fields_t *output_fields)
{
    bcmcth_oam_bfd_auth_sp_t entry;
    bcmltd_common_oam_bfd_auth_state_t_t oper_state;

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_auth_sp_stage_fields_parse(unit, key, data, &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_oam_bfd_auth_sp_insert(unit, &entry, &oper_state));
            break;

        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_auth_sp_stage_fields_parse(unit, key, data, &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_oam_bfd_auth_sp_update(unit, &entry, &oper_state));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_oam_bfd_auth_sp_delete(unit, key->data));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        output_fields->field[0]->id = OAM_BFD_AUTH_SIMPLE_PASSWORDt_OPERATIONAL_STATEf;
        output_fields->field[0]->idx = 0;
        output_fields->field[0]->data = oper_state;
        output_fields->count = 1;
    }

 exit:
    SHR_FUNC_EXIT();

}

/* OAM_BFD_AUTH_SIMPLE_PASSWORD  IMM event callback structure. */
static bcmimm_lt_cb_t bfd_auth_sp_hdl = {
    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bfd_auth_sp_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};

/*******************************************************************************
 * SHA1 IMM event handlers
 */
static int
bfd_auth_sha1_field_get(int unit, const bcmltd_field_t *field,
                        bcmcth_oam_bfd_auth_sha1_t *entry)
{
    uint32_t fid;
    uint64_t fval;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    fid  = field->id;
    fval = field->data;
    idx  = field->idx;

    switch (fid) {
        case OAM_BFD_AUTH_SHA1t_SHA1_KEYf:
            entry->sha1_key[idx] = fval;
            break;

        case OAM_BFD_AUTH_SHA1t_OPERATIONAL_STATEf:
            entry->oper_state = fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bfd_auth_sha1_stage_fields_parse(int unit,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 bcmcth_oam_bfd_auth_sha1_t *entry)
{
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    entry->id = key->data;

    field = data;
    while (field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(bfd_auth_sha1_field_get(unit, field, entry));
        field = field->next;
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_auth_sha1_stage(int unit,
                    bcmltd_sid_t sid,
                    uint32_t trans_id,
                    bcmimm_entry_event_t event,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data,
                    void *context,
                    bcmltd_fields_t *output_fields)
{
    bcmcth_oam_bfd_auth_sha1_t entry;
    bcmltd_common_oam_bfd_auth_state_t_t oper_state;

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_auth_sha1_stage_fields_parse(unit, key, data, &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_oam_bfd_auth_sha1_insert(unit, &entry, &oper_state));
            break;

        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_auth_sha1_stage_fields_parse(unit, key, data, &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_oam_bfd_auth_sha1_update(unit, &entry, &oper_state));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_oam_bfd_auth_sha1_delete(unit, key->data));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        output_fields->field[0]->id = OAM_BFD_AUTH_SHA1t_OPERATIONAL_STATEf;
        output_fields->field[0]->idx = 0;
        output_fields->field[0]->data = oper_state;
        output_fields->count = 1;
    }

 exit:
    SHR_FUNC_EXIT();

}

/* OAM_BFD_AUTH_SHA1 IMM event callback structure. */
static bcmimm_lt_cb_t bfd_auth_sha1_hdl = {
    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bfd_auth_sha1_stage,

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
int
bcmcth_oam_bfd_auth_sp_entry_get(int unit, uint32_t id,
                                 bcmcth_oam_bfd_auth_sp_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    const bcmltd_field_t *field;
    size_t num_fields;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    /* Allocate memory for key. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_fields_alloc(unit, 1, &in));
    in.field[0]->id = OAM_BFD_AUTH_SIMPLE_PASSWORDt_OAM_BFD_AUTH_SIMPLE_PASSWORD_IDf;
    in.field[0]->data = id;

    /* Allocate memory for data. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_num_data_fields_get(unit, OAM_BFD_AUTH_SIMPLE_PASSWORDt,
                                 &num_fields));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_fields_alloc(unit, num_fields, &out));

    /* Lookup IMM for the entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, OAM_BFD_AUTH_SIMPLE_PASSWORDt, &in, &out));

    sal_memset(entry, 0, sizeof(*entry));

    /* Populate the data fields. */
    for (idx = 0; idx < out.count; idx++) {
        field = out.field[idx];
        SHR_IF_ERR_VERBOSE_EXIT(bfd_auth_sp_field_get(unit, field, entry));
    }

    /* Populate the key fields. */
    entry->id = id;

exit:
    bcmcth_oam_bfd_fields_free(unit, 1, &in);
    bcmcth_oam_bfd_fields_free(unit, num_fields, &out);
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_auth_sha1_entry_get(int unit, uint32_t id,
                                   bcmcth_oam_bfd_auth_sha1_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    const bcmltd_field_t *field;
    size_t num_fields;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    /* Allocate memory for key. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_fields_alloc(unit, 1, &in));
    in.field[0]->id = OAM_BFD_AUTH_SHA1t_OAM_BFD_AUTH_SHA1_IDf;
    in.field[0]->data = id;

    /* Allocate memory for data. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_num_data_fields_get(unit, OAM_BFD_AUTH_SHA1t,
                                            &num_fields));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_fields_alloc(unit, num_fields, &out));

    /* Lookup IMM for the entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, OAM_BFD_AUTH_SHA1t, &in, &out));

    sal_memset(entry, 0, sizeof(*entry));

    /* Populate the data fields. */
    for (idx = 0; idx < out.count; idx++) {
        field = out.field[idx];
        SHR_IF_ERR_VERBOSE_EXIT(bfd_auth_sha1_field_get(unit, field, entry));
    }

    /* Populate the key fields. */
    entry->id = id;

exit:
    bcmcth_oam_bfd_fields_free(unit, 1, &in);
    bcmcth_oam_bfd_fields_free(unit, num_fields, &out);
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_auth_imm_register(int unit, bool warm)
{
    bcmcth_oam_bfd_imm_reg_t imm_reg[] = {
        {
            OAM_BFD_AUTH_SIMPLE_PASSWORDt,
            NULL,
            &bfd_auth_sp_hdl
        },
        {
            OAM_BFD_AUTH_SHA1t,
            NULL,
            &bfd_auth_sha1_hdl
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
