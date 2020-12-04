/*! \file bcmsec_imm_mgmt.c
 *
 * In-memory call back function for management related IMM LTs.
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
#include <bcmsec/bcmsec_drv.h>
#include <bcmsec/bcmsec_utils_internal.h>
#include <bcmltd/chip/bcmltd_sec_constants.h>

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_MGMT
/*******************************************************************************
 * Private functions
 */

/*!
 * \brief  Decrypt management physical table configuration
 * based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] lport Logical port number.
 * \param [in] ltid Logical table ID.
 * \param [in] Obm port control configuration to be programmed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmsec_decrypt_mgmt_pt_set(int unit,
        bcmltd_sid_t ltid,
        bcmsec_decrypt_mgmt_cfg_t *decrypt_mgmt_cfg)
{
    bcmsec_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmsec_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->decrypt_mgmt_set, SHR_E_UNAVAIL);

    /* update physical table. */
    if (drv->decrypt_mgmt_set) {
        SHR_IF_ERR_EXIT
            (drv->decrypt_mgmt_set(unit, ltid, decrypt_mgmt_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief  Populates decrypt management configuration using user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] obm_pc_pm_pkt_parse_cfg  Obm policy configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static void
bcmsec_decrypt_mgmt_cfg_fill(int unit,
                           const bcmltd_field_t *data,
                           bcmsec_decrypt_mgmt_cfg_t *decrypt_mgmt_cfg_t)
{
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;
    int idx = 0;
    while (cur_data) {
          idx = cur_data->idx;
          switch (cur_data->id) {
            case SEC_DECRYPT_MGMTt_DST_MACf:
                 decrypt_mgmt_cfg_t->dst_mac[idx] = cur_data->data;
                 break;
            case SEC_DECRYPT_MGMTt_ETHERTYPEf:
                 decrypt_mgmt_cfg_t->ethertype[idx] = cur_data->data;
                 break;
            case SEC_DECRYPT_MGMTt_DST_MAC_RANGE_LOWf:
                 decrypt_mgmt_cfg_t->dst_mac_range_low = cur_data->data;
                 break;
            case SEC_DECRYPT_MGMTt_DST_MAC_RANGE_HIGHf:
                 decrypt_mgmt_cfg_t->dst_mac_range_high = cur_data->data;
                 break;
            case SEC_DECRYPT_MGMTt_DST_MAC_GROUP_0f:
                 decrypt_mgmt_cfg_t->dst_mac_group_0 = cur_data->data;
                 break;
            case SEC_DECRYPT_MGMTt_ETHERTYPE_GROUP_0f:
                 decrypt_mgmt_cfg_t->ethertype_group_0 = cur_data->data;
                 break;
            case SEC_DECRYPT_MGMTt_DST_MAC_GROUP_1f:
                 decrypt_mgmt_cfg_t->dst_mac_group_1 = cur_data->data;
                 break;
            case SEC_DECRYPT_MGMTt_ETHERTYPE_GROUP_1f:
                 decrypt_mgmt_cfg_t->ethertype_group_1 = cur_data->data;
                 break;
            default:
                 break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief SEC_PM_CONTROL logical table callback function for staging.
 *
 * Handle SEC_PM_CONTROL IMM table change events.
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
bcmsec_decrypt_mgmt_stage(int unit,
                    bcmltd_sid_t ltid,
                    uint32_t trans_id,
                    bcmimm_entry_event_t event_reason,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data,
                    void *context,
                    bcmltd_fields_t *output_fields)
{
    int blk_id;
    bcmltd_fields_t out, in;
    bcmltd_field_t *cur_key = (bcmltd_field_t *)key;
    bcmsec_decrypt_mgmt_cfg_t decrypt_mgmt_cfg;
    size_t num_fid = 0;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    uint32_t idx_count;

    SHR_FUNC_ENTER(unit);

    /* SEC_DECRYPT_MGMTt_SEC_BLOCK_IDf */
    fid = SEC_DECRYPT_MGMTt_SEC_BLOCK_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_fval_get_from_field_array(unit, fid, 0, key, &fval));
    blk_id = fval;

    sal_memset(&decrypt_mgmt_cfg, 0, sizeof(bcmsec_decrypt_mgmt_cfg_t));
    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&in, 0, sizeof(bcmltd_fields_t));
            sal_memset(&out, 0, sizeof(bcmltd_fields_t));
            SHR_IF_ERR_EXIT
                (bcmlrd_field_count_get(unit, ltid, &num_fid));
            idx_count = 0;
            idx_count = bcmlrd_field_idx_count_get(unit, SEC_DECRYPT_MGMTt,
                                  SEC_DECRYPT_MGMTt_DST_MACf);
            idx_count += bcmlrd_field_idx_count_get(unit, SEC_DECRYPT_MGMTt,
                                  SEC_DECRYPT_MGMTt_ETHERTYPEf);
            SHR_IF_ERR_EXIT
                (bcmsec_field_list_alloc(unit, (num_fid + idx_count - 2) , &out));
            SHR_IF_ERR_EXIT
                (bcmsec_field_list_alloc(unit, 1, &in));
            in.count = 0;
            while (cur_key) {
                  SHR_IF_ERR_EXIT
                    (bcmsec_field_list_set(unit, &in, cur_key->id,
                                          0, cur_key->data));
                  cur_key = cur_key->next;
            }
            SHR_IF_ERR_EXIT
                (bcmimm_entry_lookup(unit, ltid, &in, &out));
            bcmsec_decrypt_mgmt_cfg_fill(unit, *(out.field), &decrypt_mgmt_cfg);
        case BCMIMM_ENTRY_INSERT:
            output_fields->count = 0;
            bcmsec_decrypt_mgmt_cfg_fill(unit, data, &decrypt_mgmt_cfg);
            decrypt_mgmt_cfg.blk_id = blk_id;
            SHR_IF_ERR_EXIT
                (bcmsec_decrypt_mgmt_pt_set(unit, ltid, &decrypt_mgmt_cfg));
            break;
        case BCMIMM_ENTRY_DELETE:
            decrypt_mgmt_cfg.blk_id = blk_id;
            SHR_IF_ERR_EXIT
                (bcmsec_decrypt_mgmt_pt_set(unit, ltid, &decrypt_mgmt_cfg));
            break;
        default:
            break;
    }
exit:
    if ((BCMIMM_ENTRY_UPDATE == event_reason) && (num_fid != 0)) {
        bcmsec_field_list_free(&in);
        bcmsec_field_list_free(&out);
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmsec_decrypt_mgmt_imm_register(int unit)
{
    /*! Inmemory callback functions for SEC_DECRYPT_MGMTt logical table. */
    bcmimm_lt_cb_t bcmsec_decrypt_mgmt_imm_cb = {
        /*! Staging function. */
        .stage = bcmsec_decrypt_mgmt_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, SEC_DECRYPT_MGMTt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for cut-through settings. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, SEC_DECRYPT_MGMTt,
                             &bcmsec_decrypt_mgmt_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
