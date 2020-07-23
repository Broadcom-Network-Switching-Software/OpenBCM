/*! \file bcmsec_imm_sa_policy_decrypt.c
 *
 * In-memory call back function for SA decrypt policy IMM LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>
#include <bcmsec/generated/bcmsec_ha.h>
#include <bcmsec/bcmsec_drv.h>
#include <bcmsec/bcmsec_utils.h>
#include <bcmsec/bcmsec_pt_internal.h>
#include <bcmsec/bcmsec_utils_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_SA

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief  Verify if an entry exists at the given index.
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
bcmsec_sa_policy_resource_verify (int unit, int blk_id, uint32_t sa_index,
                                  bool ipsec)
{
    bcmltd_fields_t out, in;
    bcmltd_sid_t ltid;
    bcmltd_fid_t fid;
    size_t num_fids;
    int rv;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    ltid = ipsec ? SEC_DECRYPT_IPSEC_SA_POLICYt :
                   SEC_DECRYPT_MACSEC_SA_POLICYt;

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fids));
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_alloc(unit, num_fids, &out));
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_alloc(unit, 3, &in));
    in.count = 0;

    fid = ipsec ? SEC_DECRYPT_IPSEC_SA_POLICYt_SEC_BLOCK_IDf :
                  SEC_DECRYPT_MACSEC_SA_POLICYt_SEC_BLOCK_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, blk_id));

    fid = ipsec ?
            SEC_DECRYPT_IPSEC_SA_POLICYt_SEC_DECRYPT_IPSEC_SA_POLICY_IDf:
            SEC_DECRYPT_MACSEC_SA_POLICYt_SEC_DECRYPT_MACSEC_SA_POLICY_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, sa_index));

    rv = bcmimm_entry_lookup(unit, ltid, &in, &out);

    if (SHR_SUCCESS(rv)) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    bcmsec_field_list_free(&in);
    bcmsec_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief SEC_DECRYPT_MACSEC_SA_POLICY logical table staging function.
 *
 * Handle SEC_DECRYPT_MACSEC_SA_POLICY IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] data. This is a linked list of extra added data fields (mostly
 * read only fields).
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmsec_sa_policy_decrypt_stage(int unit,
                       bcmltd_sid_t ltid,
                       uint32_t trans_id,
                       bcmimm_entry_event_t event_reason,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       void *context,
                       bcmltd_fields_t *output_fields)
{
    uint32_t blk_id;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    int index, rv;
    bcmltd_field_t *entry_data;
    bcmsec_pt_info_t pt_dyn_info = {0};
    uint64_t flags;
    bool ipsec;

    SHR_FUNC_ENTER(unit);

    ipsec = (ltid == SEC_DECRYPT_MACSEC_SA_POLICYt) ? 0 : 1;

    fid = ipsec ?
            SEC_DECRYPT_IPSEC_SA_POLICYt_SEC_BLOCK_IDf:
            SEC_DECRYPT_MACSEC_SA_POLICYt_SEC_BLOCK_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_fval_get_from_field_array(unit, fid, 0, key, &fval));
    blk_id = (uint32_t)fval;

    fid = ipsec ?
            SEC_DECRYPT_IPSEC_SA_POLICYt_SEC_DECRYPT_IPSEC_SA_POLICY_IDf:
            SEC_DECRYPT_MACSEC_SA_POLICYt_SEC_DECRYPT_MACSEC_SA_POLICY_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_fval_get_from_field_array(unit, fid, 0, key, &fval));
    index = (uint32_t)fval;

    rv = bcmsec_sa_policy_resource_verify(unit, blk_id, index,
                                          ipsec ? 0 : 1);
    SHR_IF_ERR_EXIT(rv);

    flags = 0;
    BCMSEC_PT_DYN_INFO(pt_dyn_info, index, blk_id, flags);

    entry_data = (bcmltd_field_t *)data;
    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            flags =  BCMSEC_PT_DYN_FLAGS_UPDATE;
            BCMSEC_PT_DYN_INFO(pt_dyn_info, index, blk_id, flags);
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmsec_sa_policy_pt_set(unit, ltid, &pt_dyn_info,
                                         false, entry_data));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief SA Policy encrypt table imm lookup handler.
 *
 * This populates fields manipulated by hardware and software not having a
 * updated copy in in-memory.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] trans_id Transaction ID.
 * \param [in] context Is a pointer that was given during registration.
 * \param [in] lkup_type Lookup type.
 * \param [in] in This is a linked list of the in fields in the
 * modified entry.
 * \param [out] out This is a linked list of the out fields in the
 * modified entry.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmsec_sa_policy_decrypt_lookup(int unit,
                          bcmltd_sid_t ltid,
                          uint32_t trans_id,
                          void *context,
                          bcmimm_lookup_type_t lkup_type,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out)
{
    int blk_id, index;
    bcmltd_fid_t fid;
    uint64_t fval, flags;
    const bcmltd_fields_t *key_fields;
    sa_policy_info_t sa_policy;
    bcmsec_pt_info_t pt_dyn_info = {0};
    bool ipsec;

    SHR_FUNC_ENTER(unit);

    ipsec = (ltid == SEC_DECRYPT_MACSEC_SA_POLICYt) ? 0 : 1;
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    fid = ipsec ?
            SEC_DECRYPT_IPSEC_SA_POLICYt_SEC_BLOCK_IDf :
            SEC_DECRYPT_MACSEC_SA_POLICYt_SEC_BLOCK_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_get(unit, (bcmltd_fields_t *)key_fields,
                              fid, 0, &fval));
    blk_id = fval;

    fid = ipsec ?
            SEC_DECRYPT_IPSEC_SA_POLICYt_SEC_DECRYPT_IPSEC_SA_POLICY_IDf:
            SEC_DECRYPT_MACSEC_SA_POLICYt_SEC_DECRYPT_MACSEC_SA_POLICY_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_get(unit, (bcmltd_fields_t *)key_fields,
                              fid, 0, &fval));
    index = (uint32_t)fval;

    flags = 0;
    BCMSEC_PT_DYN_INFO(pt_dyn_info, index, blk_id, flags);

    /* Get the read-only information from the device. */
    SHR_IF_ERR_EXIT
        (bcmsec_sa_policy_pt_get(unit, ltid, &pt_dyn_info,
                                 false, &sa_policy));

    fid = ipsec ?
            SEC_DECRYPT_IPSEC_SA_POLICYt_NEXT_PKT_NUMBERf :
            SEC_DECRYPT_MACSEC_SA_POLICYt_NEXT_PKT_NUMBERf;
    fval = (uint64_t)sa_policy.next_packet_number;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, out, fid, 0, fval));

    fid = ipsec ?
            SEC_DECRYPT_IPSEC_SA_POLICYt_IN_USEf :
            SEC_DECRYPT_MACSEC_SA_POLICYt_IN_USEf;
    fval = (uint64_t)sa_policy.in_use;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, out, fid, 0, fval));

    fid = ipsec ?
            SEC_DECRYPT_IPSEC_SA_POLICYt_IN_USE_START_TIMEf :
            SEC_DECRYPT_MACSEC_SA_POLICYt_IN_USE_START_TIMEf;
    fval = (uint64_t)sa_policy.start_time;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, out, fid, 0, fval));

    fid = ipsec ?
            SEC_DECRYPT_IPSEC_SA_POLICYt_IN_USE_STOP_TIMEf :
            SEC_DECRYPT_MACSEC_SA_POLICYt_IN_USE_STOP_TIMEf;
    fval = (uint64_t)sa_policy.stop_time;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, out, fid, 0, fval));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmsec_sa_policy_decrypt_imm_register(int unit)
{
    /*!
     * Inmemory callback functions for SEC_DECRYPT_MACSEC_SA_POLICY
     * and SEC_DECRYPT_MACSEC_SA_POLICY logical tables.
     */
    bcmimm_lt_cb_t bcmsec_sa_policy_imm_cb = {
        /*! Staging function. */
        .stage = bcmsec_sa_policy_decrypt_stage,

        /*! Lookup function. */
        .lookup = bcmsec_sa_policy_decrypt_lookup,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* SEC_DECRYPT_MACSEC_SA_POLICYt */
    rv = bcmlrd_map_get(unit, SEC_DECRYPT_MACSEC_SA_POLICYt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for decrypt port settings. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, SEC_DECRYPT_MACSEC_SA_POLICYt,
                             &bcmsec_sa_policy_imm_cb, NULL));

    /* SEC_DECRYPT_IPSEC_SA_POLICYt */
    rv = bcmlrd_map_get(unit, SEC_DECRYPT_IPSEC_SA_POLICYt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_ERR_EXIT(SHR_E_NONE);
    }
    /*! Registers callback functions for decrypt port settings. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, SEC_DECRYPT_IPSEC_SA_POLICYt,
                             &bcmsec_sa_policy_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
