/*! \file bcm56780_a0_sec_sa_policy.c
 *
 * File containing SA policy related defines and internal function for
 * bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmsec/generated/bcmsec_ha.h>
#include <bcmsec/bcmsec_types.h>
#include <bcmsec/bcmsec_pt_internal.h>
#include <bcmsec/chip/bcm56780_a0_sec_sa_policy.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmsec/bcmsec_event.h>


#define BSL_LOG_MODULE  BSL_LS_BCMSEC_SA
/*******************************************************************************
 * Local definitions
 */

/*******************************************************************************
 * Private functions
 */

static int
bcm56780_a0_sa_policy_macsec_encrypt_set (int unit, bcmltd_sid_t ltid,
                                   bcmsec_pt_info_t *pt_dyn_info,
                                   bcmltd_field_t *in)
{
    int size;
    bcmdrd_sid_t sid = ESEC_SA_TABLEm;
    bcmdrd_fid_t fid;
    uint32_t *fval = NULL, *val = NULL;
    uint8_t *ptr;
    uint32_t idx, data;
    bcmsec_sa_status_imm_update_t sa_status;

    SHR_FUNC_ENTER(unit);

    sa_status.sa_index = pt_dyn_info->index;
    sa_status.blk_id = pt_dyn_info->tbl_inst;
    sa_status.sa_status_type = SA_INVALID;
    sa_status.dir = SA_EXPIRY_ENCRYPT;

    size = BCMSEC_MAX_ENTRY_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(fval, size, "bcmsecSaPolicyPtEntry");
    SHR_NULL_CHECK(fval, SHR_E_MEMORY);
    sal_memset(fval, 0, size);

    size = BCMSEC_MAX_FIELD_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(val, size, "bcmsecSaPolicyPtField");
    SHR_NULL_CHECK(val, SHR_E_MEMORY);
    sal_memset(val, 0, size);

    if (in && (pt_dyn_info->flags & BCMSEC_PT_DYN_FLAGS_UPDATE)) {
        SHR_IF_ERR_EXIT
            (bcmsec_pt_indexed_read(unit, sid, ltid,
                                    pt_dyn_info, fval));
    }

    while (in) {
        data = (uint32_t)in->data;
        val[0] = data;
        idx = in->idx;
        switch (in->id) {
            case SEC_ENCRYPT_MACSEC_SA_POLICYt_STATEf:
                if (data > 1) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                fid = VALIDf;
                sa_status.sa_status_type = val[0] ? 1 : 0;
                break;
            case SEC_ENCRYPT_MACSEC_SA_POLICYt_NEW_ACTIVE_ANf:
                fid = SECTAG_AN_IN_ONE_SA_PER_SCf;
                break;
            case SEC_ENCRYPT_MACSEC_SA_POLICYt_INITIAL_PKT_NUMBERf:
                fid = NEXTPNf;
                break;
            case SEC_ENCRYPT_MACSEC_SA_POLICYt_SHORT_SC_IDENTIFIERf:
                fid = SSCIf;
                break;
            case SEC_ENCRYPT_MACSEC_SA_POLICYt_XPN_SALTf:
                fid = SALTf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid, fval, val));
                ptr = (uint8_t *) val;
                ptr[idx] = data;
                break;
            case SEC_ENCRYPT_MACSEC_SA_POLICYt_KEYf:
                fid = SAKf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid, fval, val));
                ptr = (uint8_t *) val;
                ptr[idx] = data;
                break;
            default:
                in = in->next;
                continue;
        }
        SHR_IF_ERR_EXIT
            (bcmsec_field_set(unit, sid, fid, fval, val));
        in = in->next;
    }
    sa_status.ipsec = 0;
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, sid, ltid, pt_dyn_info, fval));
    SHR_IF_ERR_EXIT
        (bcmsec_sa_status_imm_update(unit, &sa_status));
exit:
    if (val) {
        SHR_FREE(val);
    }
    if (fval) {
        SHR_FREE(fval);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_sa_policy_ipsec_encrypt_set (int unit, bcmltd_sid_t ltid,
                                   bcmsec_pt_info_t *pt_dyn_info,
                                   bcmltd_field_t *in)
{
    int size;
    bcmdrd_sid_t sid = ESEC_SA_TABLEm;
    bcmdrd_fid_t fid;
    uint32_t *fval = NULL, *val = NULL;
    uint8_t *ptr;
    uint32_t idx, data;
    bcmsec_sa_status_imm_update_t sa_status;

    SHR_FUNC_ENTER(unit);

    sa_status.sa_index = pt_dyn_info->index;
    sa_status.blk_id = pt_dyn_info->tbl_inst;
    sa_status.sa_status_type = SA_INVALID;
    sa_status.dir = SA_EXPIRY_ENCRYPT;

    size = BCMSEC_MAX_ENTRY_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(fval, size, "bcmsecSaPolicyPtEntry");
    SHR_NULL_CHECK(fval, SHR_E_MEMORY);
    sal_memset(fval, 0, size);

    size = BCMSEC_MAX_FIELD_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(val, size, "bcmsecSaPolicyPtField");
    SHR_NULL_CHECK(val, SHR_E_MEMORY);
    sal_memset(val, 0, size);

    if (in && (pt_dyn_info->flags & BCMSEC_PT_DYN_FLAGS_UPDATE)) {
        SHR_IF_ERR_EXIT
            (bcmsec_pt_indexed_read(unit, sid, ltid,
                                    pt_dyn_info, fval));
    }

    while (in) {
        data = (uint32_t)in->data;
        val[0] = data;
        idx = in->idx;
        switch (in->id) {
            case SEC_ENCRYPT_IPSEC_SA_POLICYt_STATEf:
                if (data > 1) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                fid = VALIDf;
                sa_status.sa_status_type = val[0] ? 1 : 0;
                break;
            case SEC_ENCRYPT_IPSEC_SA_POLICYt_INITIAL_PKT_NUMBERf:
                fid = NEXTPNf;
                break;
            case SEC_ENCRYPT_IPSEC_SA_POLICYt_SECURITY_PARAMETER_INDEXf:
                fid = SSCIf;
                break;
            case SEC_ENCRYPT_IPSEC_SA_POLICYt_XPN_SALTf:
                fid = SALTf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid, fval, val));
                ptr = (uint8_t *) val;
                ptr[idx] = data;
                break;
            case SEC_ENCRYPT_IPSEC_SA_POLICYt_KEYf:
                fid = SAKf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid, fval, val));
                ptr = (uint8_t *) val;
                ptr[idx] = data;
                break;
            default:
                in = in->next;
                continue;
        }
        SHR_IF_ERR_EXIT
            (bcmsec_field_set(unit, sid, fid, fval, val));
        in = in->next;
    }
    sa_status.ipsec = 1;
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, sid, ltid, pt_dyn_info, fval));
    SHR_IF_ERR_EXIT
        (bcmsec_sa_status_imm_update(unit, &sa_status));
exit:
    if (val) {
        SHR_FREE(val);
    }
    if (fval) {
        SHR_FREE(fval);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_sa_policy_macsec_decrypt_set (int unit, bcmltd_sid_t ltid,
                                   bcmsec_pt_info_t *pt_dyn_info,
                                   bcmltd_field_t *in)
{
    int size;
    bcmdrd_sid_t sid = ISEC_SA_TABLEm;
    bcmdrd_fid_t fid;
    uint32_t *fval = NULL, *val = NULL;
    uint8_t *ptr;
    uint32_t idx, data;
    bcmsec_sa_status_imm_update_t sa_status;

    SHR_FUNC_ENTER(unit);

    sa_status.sa_index = pt_dyn_info->index;
    sa_status.blk_id = pt_dyn_info->tbl_inst;
    sa_status.sa_status_type = SA_INVALID;
    sa_status.dir = SA_EXPIRY_DECRYPT;

    size = BCMSEC_MAX_ENTRY_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(fval, size, "bcmsecSaPolicyPtEntry");
    SHR_NULL_CHECK(fval, SHR_E_MEMORY);
    sal_memset(fval, 0, size);

    size = BCMSEC_MAX_FIELD_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(val, size, "bcmsecSaPolicyPtField");
    SHR_NULL_CHECK(val, SHR_E_MEMORY);
    sal_memset(val, 0, size);

    if (in && (pt_dyn_info->flags & BCMSEC_PT_DYN_FLAGS_UPDATE)) {
        SHR_IF_ERR_EXIT
            (bcmsec_pt_indexed_read(unit, sid, ltid,
                                    pt_dyn_info, fval));
    }

    while (in) {
        data = (uint32_t)in->data;
        val[0] = data;
        idx = in->idx;
        switch (in->id) {
            case SEC_DECRYPT_MACSEC_SA_POLICYt_STATEf:
                if (data > 1) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                fid = VALIDf;
                sa_status.sa_status_type = val[0] ? 1 : 0;
                break;
            case SEC_DECRYPT_MACSEC_SA_POLICYt_INITIAL_PKT_NUMBERf:
                fid = NEXTPNf;
                break;
            case SEC_DECRYPT_MACSEC_SA_POLICYt_SHORT_SC_IDENTIFIERf:
                fid = SSCIf;
                break;
            case SEC_DECRYPT_MACSEC_SA_POLICYt_XPN_SALTf:
                fid = SALTf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid, fval, val));
                ptr = (uint8_t *) val;
                ptr[idx] = data;
                break;
            case SEC_DECRYPT_MACSEC_SA_POLICYt_KEYf:
                fid = SAKf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid, fval, val));
                ptr = (uint8_t *) val;
                ptr[idx] = data;
                break;
            case SEC_DECRYPT_MACSEC_SA_POLICYt_NEXT_PKT_NUMBERf:
            case SEC_DECRYPT_MACSEC_SA_POLICYt_IN_USEf:
            case SEC_DECRYPT_MACSEC_SA_POLICYt_IN_USE_START_TIMEf:
            case SEC_DECRYPT_MACSEC_SA_POLICYt_IN_USE_STOP_TIMEf:
                in = in->next;
                continue;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (bcmsec_field_set(unit, sid, fid, fval, val));
        in = in->next;
    }
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, sid, ltid, pt_dyn_info, fval));
    sa_status.ipsec = 0;
    SHR_IF_ERR_EXIT
        (bcmsec_sa_status_imm_update(unit, &sa_status));
exit:
    if (val) {
        SHR_FREE(val);
    }
    if (fval) {
        SHR_FREE(fval);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_sa_policy_ipsec_decrypt_set (int unit, bcmltd_sid_t ltid,
                                   bcmsec_pt_info_t *pt_dyn_info,
                                   bcmltd_field_t *in)
{
    int size;
    bcmdrd_sid_t sid = ISEC_SA_TABLEm;
    bcmdrd_fid_t fid;
    uint32_t *fval = NULL, *val = NULL;
    uint8_t *ptr;
    uint32_t idx, data;
    bcmsec_sa_status_imm_update_t sa_status;

    SHR_FUNC_ENTER(unit);

    sa_status.sa_index = pt_dyn_info->index;
    sa_status.blk_id = pt_dyn_info->tbl_inst;
    sa_status.sa_status_type = SA_INVALID;
    sa_status.dir = SA_EXPIRY_DECRYPT;

    size = BCMSEC_MAX_ENTRY_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(fval, size, "bcmsecSaPolicyPtEntry");
    SHR_NULL_CHECK(fval, SHR_E_MEMORY);
    sal_memset(fval, 0, size);

    size = BCMSEC_MAX_FIELD_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(val, size, "bcmsecSaPolicyPtField");
    SHR_NULL_CHECK(val, SHR_E_MEMORY);
    sal_memset(val, 0, size);

    if (in && (pt_dyn_info->flags & BCMSEC_PT_DYN_FLAGS_UPDATE)) {
        SHR_IF_ERR_EXIT
            (bcmsec_pt_indexed_read(unit, sid, ltid,
                                    pt_dyn_info, fval));
    }

    while (in) {
        data = (uint32_t)in->data;
        val[0] = data;
        idx = in->idx;
        switch (in->id) {
            case SEC_DECRYPT_IPSEC_SA_POLICYt_STATEf:
                if (data > 1) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                fid = VALIDf;
                sa_status.sa_status_type = val[0] ? 1 : 0;
                break;
            case SEC_DECRYPT_IPSEC_SA_POLICYt_INITIAL_PKT_NUMBERf:
                fid = NEXTPNf;
                break;
            case SEC_DECRYPT_IPSEC_SA_POLICYt_XPN_SALTf:
                fid = SALTf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid, fval, val));
                ptr = (uint8_t *) val;
                ptr[idx] = data;
                break;
            case SEC_DECRYPT_IPSEC_SA_POLICYt_KEYf:
                fid = SAKf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid, fval, val));
                ptr = (uint8_t *) val;
                ptr[idx] = data;
                break;
            case SEC_DECRYPT_IPSEC_SA_POLICYt_NEXT_PKT_NUMBERf:
            case SEC_DECRYPT_IPSEC_SA_POLICYt_IN_USEf:
            case SEC_DECRYPT_IPSEC_SA_POLICYt_IN_USE_START_TIMEf:
            case SEC_DECRYPT_IPSEC_SA_POLICYt_IN_USE_STOP_TIMEf:
                in = in->next;
                continue;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (bcmsec_field_set(unit, sid, fid, fval, val));
        in = in->next;
    }
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, sid, ltid, pt_dyn_info, fval));
    sa_status.ipsec = 1;
    SHR_IF_ERR_EXIT
        (bcmsec_sa_status_imm_update(unit, &sa_status));
exit:
    if (val) {
        SHR_FREE(val);
    }
    if (fval) {
        SHR_FREE(fval);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_sa_policy_encrypt_get (int unit, bcmltd_sid_t ltid,
                           bcmsec_pt_info_t *pt_dyn_info,
                           sa_policy_info_t *sa_policy)
{
    int size;
    bcmdrd_sid_t sid = ESEC_SA_TABLEm;
    bcmdrd_fid_t fid;
    uint32_t *fval = NULL, *val = NULL;

    SHR_FUNC_ENTER(unit);

    size = BCMSEC_MAX_ENTRY_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(fval, size, "bcmsecSaPolicyPtEntry");
    SHR_NULL_CHECK(fval, SHR_E_MEMORY);
    sal_memset(fval, 0, size);

    size = BCMSEC_MAX_FIELD_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(val, size, "bcmsecSaPolicyPtField");
    SHR_NULL_CHECK(val, SHR_E_MEMORY);
    sal_memset(val, 0, size);

    pt_dyn_info->flags |= BCMLT_ENT_ATTR_GET_FROM_HW;
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_read(unit, sid, ltid,
                                pt_dyn_info, fval));

    fid = NEXTPNf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_get(unit, sid, fid, fval, val));
    sa_policy->next_packet_number = ((uint64_t)val[1] << 32) | val[0];

    fid = STATUSf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_get(unit, sid, fid, fval, val));
    sa_policy->in_use = val[0];

    fid = STARTEDTIMEf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_get(unit, sid, fid, fval, val));
    sa_policy->start_time = val[0];

    fid = STOPPEDTIMEf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_get(unit, sid, fid, fval, val));
    sa_policy->stop_time = val[0];

exit:
    if (val) {
        SHR_FREE(val);
    }
    if (fval) {
        SHR_FREE(fval);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_sa_policy_decrypt_get (int unit, bcmltd_sid_t ltid,
                           bcmsec_pt_info_t *pt_dyn_info,
                           sa_policy_info_t *sa_policy)
{
    int size;
    bcmdrd_sid_t sid = ISEC_SA_TABLEm;
    bcmdrd_fid_t fid;
    uint32_t *fval = NULL, *val = NULL;

    SHR_FUNC_ENTER(unit);

    size = BCMSEC_MAX_ENTRY_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(fval, size, "bcmsecSaPolicyPtEntry");
    SHR_NULL_CHECK(fval, SHR_E_MEMORY);
    sal_memset(fval, 0, size);

    size = BCMSEC_MAX_FIELD_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(val, size, "bcmsecSaPolicyPtField");
    SHR_NULL_CHECK(val, SHR_E_MEMORY);
    sal_memset(val, 0, size);

    pt_dyn_info->flags |= BCMLT_ENT_ATTR_GET_FROM_HW;
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_read(unit, sid, ltid,
                                pt_dyn_info, fval));

    fid = NEXTPNf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_get(unit, sid, fid, fval, val));
    sa_policy->next_packet_number = ((uint64_t)val[1] << 32) | val[0];

    fid = STATUSf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_get(unit, sid, fid, fval, val));
    sa_policy->in_use = val[0];

    fid = STARTEDTIMEf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_get(unit, sid, fid, fval, val));
    sa_policy->start_time = val[0];

    fid = STOPPEDTIMEf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_get(unit, sid, fid, fval, val));
    sa_policy->stop_time = val[0];

exit:
    if (val) {
        SHR_FREE(val);
    }
    if (fval) {
        SHR_FREE(fval);
    }
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public functions
 */

int
bcm56780_a0_sa_policy_set (int unit, bcmltd_sid_t ltid,
                           bcmsec_pt_info_t *pt_dyn_info,
                           bcmltd_field_t *in, int encrypt)
{
    if ((ltid == SEC_ENCRYPT_MACSEC_SA_POLICYt) ||
        (ltid == SEC_DECRYPT_MACSEC_SA_POLICYt)) {
        if (encrypt) {
            return bcm56780_a0_sa_policy_macsec_encrypt_set(unit, ltid, pt_dyn_info, in);
        } else {
            return bcm56780_a0_sa_policy_macsec_decrypt_set(unit, ltid, pt_dyn_info, in);
        }
    } else {
        if (encrypt) {
            return bcm56780_a0_sa_policy_ipsec_encrypt_set(unit, ltid, pt_dyn_info, in);
        } else {
            return bcm56780_a0_sa_policy_ipsec_decrypt_set(unit, ltid, pt_dyn_info, in);
        }
    }
    return SHR_E_INTERNAL;
}

int
bcm56780_a0_sa_policy_get (int unit, bcmltd_sid_t ltid,
                           bcmsec_pt_info_t *pt_dyn_info,
                           sa_policy_info_t *sa_policy,
                           int encrypt)
{
    if (encrypt) {
        return bcm56780_a0_sa_policy_encrypt_get(unit, ltid, pt_dyn_info, sa_policy);
    } else {
        return bcm56780_a0_sa_policy_decrypt_get(unit, ltid, pt_dyn_info, sa_policy);
    }
    return SHR_E_INTERNAL;
}

