/*! \file bcmsec_imm_sa_status.c
 *
 * In-memory handler function for SA IMM status LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmsec/bcmsec_utils.h>
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>
#include <bcmsec/bcmsec_utils_internal.h>
#include <bcmsec/bcmsec_event.h>

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_EVENT
/*******************************************************************************
 * Private functions
 */
/*******************************************************************************
 * Public functions
 */

/*!
 * \brief  Verify if an SA is IPsec.
 *
 * \param [in] unit  Unit number
 * \param [in] sc_id Secure channel id
 * \param [in] an Association number
 * \param [in] ipsec Ipsec or Macsec SA LT
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmsec_sa_ipsec_get (int unit, int blk_id, int sa_index,
                     bool encrypt, bool *ipsec)
{
    bcmltd_fields_t out, in;
    bcmltd_sid_t ltid;
    bcmltd_fid_t fid;
    size_t num_fids;
    int rv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    ltid = encrypt ? SEC_ENCRYPT_IPSEC_SA_POLICYt :
                     SEC_DECRYPT_IPSEC_SA_POLICYt;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fids));
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_alloc(unit, num_fids, &out));
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_alloc(unit, 3, &in));
    in.count = 0;

    fid = encrypt ? SEC_ENCRYPT_IPSEC_SA_POLICYt_SEC_BLOCK_IDf :
                    SEC_DECRYPT_IPSEC_SA_POLICYt_SEC_BLOCK_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, blk_id));

    fid = encrypt ?
            SEC_ENCRYPT_IPSEC_SA_POLICYt_SEC_ENCRYPT_IPSEC_SA_POLICY_IDf :
            SEC_DECRYPT_IPSEC_SA_POLICYt_SEC_DECRYPT_IPSEC_SA_POLICY_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, sa_index));

    rv = bcmimm_entry_lookup(unit, ltid, &in, &out);

    if (SHR_SUCCESS(rv)) {
        *ipsec = TRUE;
    } else {
        *ipsec = FALSE;
    }

exit:
    bcmsec_field_list_free(&in);
    bcmsec_field_list_free(&out);
    SHR_FUNC_EXIT();
}

static int
bcmsec_sa_encrypt_status_imm_update (int unit,
                        bcmsec_sa_status_imm_update_t *sa_status,
                        bool ipsec)
{
    bcmltd_sid_t ltid;
    bcmltd_fields_t in;
    bcmltd_fid_t fid;
    int rv, val;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));


    ltid = ipsec ? SEC_ENCRYPT_IPSEC_SA_POLICY_STATUSt :
                   SEC_ENCRYPT_MACSEC_SA_POLICY_STATUSt;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_alloc(unit, 4, &in));

    in.count = 0;
    fid = ipsec ? SEC_ENCRYPT_IPSEC_SA_POLICY_STATUSt_SEC_BLOCK_IDf :
                  SEC_ENCRYPT_MACSEC_SA_POLICY_STATUSt_SEC_BLOCK_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, sa_status->blk_id));
    fid = ipsec ?
            SEC_ENCRYPT_IPSEC_SA_POLICY_STATUSt_SEC_ENCRYPT_IPSEC_SA_POLICY_IDf :
            SEC_ENCRYPT_MACSEC_SA_POLICY_STATUSt_SEC_ENCRYPT_MACSEC_SA_POLICY_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, sa_status->sa_index));
    fid = ipsec ?
            SEC_ENCRYPT_IPSEC_SA_POLICY_STATUSt_STATEf :
            SEC_ENCRYPT_MACSEC_SA_POLICY_STATUSt_STATEf;
    val = sa_status->sa_status_type;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, val));

    rv = bcmimm_entry_update(unit, 0, ltid, &in);
    if (rv == SHR_E_NOT_FOUND || rv == SHR_E_EMPTY) {
        SHR_IF_ERR_EXIT(bcmimm_entry_insert(unit, ltid, &in));
    }
exit:
    bcmsec_field_list_free(&in);
    SHR_FUNC_EXIT();
}

static int
bcmsec_sa_decrypt_status_imm_update (int unit,
                        bcmsec_sa_status_imm_update_t *sa_status,
                        bool ipsec)
{
    bcmltd_sid_t ltid;
    bcmltd_fields_t in;
    bcmltd_fid_t fid;
    int rv, val;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));


    ltid = ipsec ? SEC_DECRYPT_IPSEC_SA_POLICY_STATUSt :
                   SEC_DECRYPT_MACSEC_SA_POLICY_STATUSt;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_alloc(unit, 4, &in));

    in.count = 0;
    fid = ipsec ?
            SEC_DECRYPT_IPSEC_SA_POLICY_STATUSt_SEC_BLOCK_IDf :
            SEC_DECRYPT_MACSEC_SA_POLICY_STATUSt_SEC_BLOCK_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, sa_status->blk_id));
    fid = ipsec ?
            SEC_DECRYPT_IPSEC_SA_POLICY_STATUSt_SEC_DECRYPT_IPSEC_SA_POLICY_IDf :
            SEC_DECRYPT_MACSEC_SA_POLICY_STATUSt_SEC_DECRYPT_MACSEC_SA_POLICY_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, sa_status->sa_index));
    fid = ipsec ?
            SEC_DECRYPT_IPSEC_SA_POLICY_STATUSt_STATEf :
            SEC_DECRYPT_MACSEC_SA_POLICY_STATUSt_STATEf;
    val = sa_status->sa_status_type;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, val));

    rv = bcmimm_entry_update(unit, 0, ltid, &in);
    if (rv == SHR_E_NOT_FOUND || rv == SHR_E_EMPTY) {
        SHR_IF_ERR_EXIT(bcmimm_entry_insert(unit, ltid, &in));
    }
exit:
    bcmsec_field_list_free(&in);
    SHR_FUNC_EXIT();
}

int
bcmsec_sa_status_imm_update (int unit,
                        bcmsec_sa_status_imm_update_t *sa_status)
{
    bool encrypt, ipsec = 0;

    SHR_FUNC_ENTER(unit);
    encrypt = (sa_status->dir == SA_EXPIRY_ENCRYPT) ? 1 : 0;

    /*
     * Derive IPsec information from IMM if information is not available.
     */
    if (sa_status->ipsec == -1) {
        SHR_IF_ERR_EXIT
            (bcmsec_sa_ipsec_get(unit, sa_status->blk_id,
                                 sa_status->sa_index, encrypt, &ipsec));
    } else {
        ipsec = sa_status->ipsec;
    }

    if (encrypt) {
        SHR_IF_ERR_EXIT
            (bcmsec_sa_encrypt_status_imm_update(unit, sa_status, ipsec));
    } else {
        SHR_IF_ERR_EXIT
            (bcmsec_sa_decrypt_status_imm_update(unit, sa_status, ipsec));
    }
exit:
    SHR_FUNC_EXIT();
}
