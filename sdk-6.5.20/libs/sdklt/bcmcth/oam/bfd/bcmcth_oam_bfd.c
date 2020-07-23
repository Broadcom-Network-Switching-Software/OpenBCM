/*! \file bcmcth_oam_bfd.c
 *
 * BCMCTH OAM BFD LT APIs.
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
 * Public Functions
 */
int
bcmcth_oam_bfd_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_uc_alloc(unit, warm));
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_event_report_init(unit, warm));

 exit:
    SHR_FUNC_EXIT();
}


void
bcmcth_oam_bfd_deinit(int unit)
{
    bcmcth_oam_bfd_uc_free(unit);
    bcmcth_oam_bfd_event_report_deinit(unit);
}

int
bcmcth_oam_bfd_imm_register(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Register the various components. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_control_imm_register(unit, warm));
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_auth_imm_register(unit, warm));
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_event_imm_register(unit, warm));
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_endpoint_imm_register(unit, warm));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_imm_cb_register(int unit, bool warm, int num_sids,
                               bcmcth_oam_bfd_imm_reg_t *imm_reg)
{
    int i;

    SHR_FUNC_ENTER(unit);

    /*
     * Callback registration for BFD LTs
     */
    for (i = 0; i < num_sids; i++) {
        if (bcmcth_oam_imm_mapped(unit, imm_reg[i].sid)) {

            if ((warm == false) && imm_reg[i].prepopulate != NULL) {
                /* Prepopulate update_only tables. */
                imm_reg[i].prepopulate(unit, imm_reg[i].sid);
            }

            SHR_IF_ERR_EXIT
                (bcmimm_lt_event_reg(unit, imm_reg[i].sid,
                                     imm_reg[i].cb, NULL));
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Memory allocation related functions.
 */
/* Get the number of fields in a BFD LT to allocate memory. */
int
bcmcth_oam_bfd_num_data_fields_get(int unit, bcmltd_sid_t sid,
                                   size_t *num_fields)
{
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get the number of fields. */
    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, sid, num_fields));

    /* Increase the field count for tables with arrays. */
    switch (sid) {
        case OAM_BFD_CONTROLt:
        case OAM_BFD_IPV4_ENDPOINTt:
        case OAM_BFD_IPV4_ENDPOINT_STATSt:
        case OAM_BFD_IPV4_ENDPOINT_STATUSt:
        case OAM_BFD_IPV6_ENDPOINTt:
        case OAM_BFD_IPV6_ENDPOINT_STATSt:
        case OAM_BFD_IPV6_ENDPOINT_STATUSt:
        case OAM_BFD_TNL_IPV4_ENDPOINTt:
        case OAM_BFD_TNL_IPV4_ENDPOINT_STATSt:
        case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt:
        case OAM_BFD_TNL_IPV6_ENDPOINTt:
        case OAM_BFD_TNL_IPV6_ENDPOINT_STATSt:
        case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt:
        case OAM_BFD_TNL_MPLS_ENDPOINT_STATSt:
        case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt:
        case OAM_BFD_EVENT_CONTROLt:
        case OAM_BFD_EVENT_STATUSt:
        case OAM_BFD_EVENTt:
        case OAM_BFD_STATSt:
            /* Do nothing. */
            break;


        case OAM_BFD_AUTH_SIMPLE_PASSWORDt:
            *num_fields += SIMPLE_PASSWORD_MAX_LENGTH;
            break;

        case OAM_BFD_AUTH_SHA1t:
            *num_fields +=  SHA1_KEY_LENGTH;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt:
            bfd_drv = bcmcth_oam_bfd_drv_get(unit);
            SHR_NULL_CHECK(bfd_drv, SHR_E_NOT_FOUND);

            *num_fields += bfd_drv->max_mpls_labels +
                           (MEP_IDENTIFIER_MAX_LENGTH * 2);
            break;

        default:
            /* Not a BFD table. */
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_fields_free(int unit, size_t num_fields, bcmltd_fields_t *fields)
{
    size_t idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fields, SHR_E_PARAM);

    if (fields->field != NULL) {
        for (idx = 0; idx < num_fields; idx++) {
            if (fields->field[idx] != NULL) {
                shr_fmm_free(fields->field[idx]);
            }
        }
        SHR_FREE(fields->field);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_fields_alloc(int unit, size_t num_fields,
                            bcmltd_fields_t *fields)
{
    size_t idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fields, SHR_E_PARAM);

    /* Allocate fields buffers */
    SHR_ALLOC(fields->field,
              sizeof(bcmltd_field_t *) * num_fields,
              "bcmcthOamBfdFields");

    SHR_NULL_CHECK(fields->field, SHR_E_MEMORY);

    sal_memset(fields->field, 0, sizeof(bcmltd_field_t *) * num_fields);

    fields->count = num_fields;

    for (idx = 0; idx < num_fields; idx++) {
        fields->field[idx] = shr_fmm_alloc();
        SHR_NULL_CHECK(fields->field[idx], SHR_E_MEMORY);
        sal_memset(fields->field[idx], 0, sizeof(bcmltd_field_t));
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmcth_oam_bfd_fields_free(unit, num_fields, fields);
    }
    SHR_FUNC_EXIT();
}
