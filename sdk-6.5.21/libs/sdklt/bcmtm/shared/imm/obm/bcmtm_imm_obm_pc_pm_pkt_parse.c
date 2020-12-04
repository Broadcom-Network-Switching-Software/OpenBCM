/*! \file bcmtm_imm_obm_pc_pm_pkt_parse.c
 *
 * In-memory call back function for obm policy table.
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
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmtm/obm/bcmtm_obm_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief  Populates obm control control configuration using user defined values.
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
bcmtm_obm_pc_pm_pkt_parse_cfg_fill(int unit,
                           const bcmltd_field_t *data,
                           bcmtm_obm_pc_pm_pkt_parse_cfg_t *obm_pc_pm_pkt_parse_cfg)
{
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;
    int idx = 0;
    while (cur_data) {
          idx = cur_data->idx;
          switch (cur_data->id) {
            case TM_OBM_PC_PM_PKT_PARSEt_INNER_TPIDf:
                 obm_pc_pm_pkt_parse_cfg->inner_tpid = cur_data->data;
                 break;
            case TM_OBM_PC_PM_PKT_PARSEt_OUTER_TPIDf:
                 obm_pc_pm_pkt_parse_cfg->outer_tpid[idx] = cur_data->data;
                 break;
            case TM_OBM_PC_PM_PKT_PARSEt_ETAG_PARSEf:
                 obm_pc_pm_pkt_parse_cfg->etag_parsing = cur_data->data;
                 break;
            case TM_OBM_PC_PM_PKT_PARSEt_ETAG_ETHERTYPEf:
                 obm_pc_pm_pkt_parse_cfg->etag_ethertype = cur_data->data;
                 break;
            case TM_OBM_PC_PM_PKT_PARSEt_VNTAG_PARSEf:
                 obm_pc_pm_pkt_parse_cfg->vntag_parsing = cur_data->data;
                 break;
            case TM_OBM_PC_PM_PKT_PARSEt_VNTAG_ETHERTYPEf:
                 obm_pc_pm_pkt_parse_cfg->vntag_ethertype = cur_data->data;
                 break;
            case TM_OBM_PC_PM_PKT_PARSEt_HIGIG3_ETHERTYPEf:
                 obm_pc_pm_pkt_parse_cfg->gsh_ethertype = cur_data->data;
                 break;
            case TM_OBM_PC_PM_PKT_PARSEt_HIGIG3_ETHERTYPE_MASKf:
                 obm_pc_pm_pkt_parse_cfg->gsh_ethertype_mask = cur_data->data;
                 break;
            case TM_OBM_PC_PM_PKT_PARSEt_USER_DEFINED_PROTOCOL_MATCHf:
                 obm_pc_pm_pkt_parse_cfg->user_protocol_matching[idx] = cur_data->data;
                 break;
            case TM_OBM_PC_PM_PKT_PARSEt_USER_DEFINED_PROTOCOL_ETHERTYPEf:
                 obm_pc_pm_pkt_parse_cfg->ethertype[idx] = cur_data->data;
                 break;
            case TM_OBM_PC_PM_PKT_PARSEt_USER_DEFINED_PROTOCOL_ETHERTYPE_MASKf:
                 obm_pc_pm_pkt_parse_cfg->ethertype_mask[idx] = cur_data->data;
                 break;
            case TM_OBM_PC_PM_PKT_PARSEt_USER_DEFINED_PROTOCOL_DST_MACf:
                 obm_pc_pm_pkt_parse_cfg->mac_addr[idx] = cur_data->data;
                 break;
            case TM_OBM_PC_PM_PKT_PARSEt_USER_DEFINED_PROTOCOL_DST_MAC_MASKf:
                 obm_pc_pm_pkt_parse_cfg->mac_addr_mask[idx] = cur_data->data;
                 break;
            case TM_OBM_PC_PM_PKT_PARSEt_USER_DEFINED_PROTOCOL_TRAFFIC_CLASSf:
                 obm_pc_pm_pkt_parse_cfg->traffic_class = cur_data->data;
                 break;
            default:
                 break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief  OBM port control physical table configuration
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
bcmtm_obm_pc_pm_pkt_parse_pt_set(int unit,
        bcmltd_sid_t ltid,
        uint8_t pmid,
        bcmtm_obm_pc_pm_pkt_parse_cfg_t *obm_pc_pm_pkt_parse_cfg)
{
    bcmtm_drv_t *drv;
    bcmtm_obm_drv_t obm_drv;

    SHR_FUNC_ENTER(unit);
    sal_memset(&obm_drv, 0, sizeof(bcmtm_obm_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->obm_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->obm_drv_get(unit, &obm_drv));
    /* update physical table. */
    if (obm_drv.policy_set) {
        SHR_IF_ERR_EXIT
            (obm_drv.policy_set(unit, ltid, pmid, obm_pc_pm_pkt_parse_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Resets the physical table to default values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] key  This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_pc_pm_pkt_parse_reset(int unit,
                        bcmltd_sid_t ltid,
                        const bcmltd_field_t *key,
                        bcmtm_obm_pc_pm_pkt_parse_cfg_t *obm_pc_pm_pkt_parse_cfg)
{
    uint8_t pmid;
    bcmltd_fid_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* TM_OBM_PC_PM_PKT_PARSEt_PC_PM_IDf */
    fid = TM_OBM_PC_PM_PKT_PARSEt_PC_PM_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    pmid = (uint8_t)fval - 1   ;
    SHR_IF_ERR_EXIT
         (bcmtm_obm_pc_pm_pkt_parse_pt_set(unit, ltid, pmid, obm_pc_pm_pkt_parse_cfg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the physical table with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] key  This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] output_fields This is linked list of extra added data fields
 * (mostly read only fields.)
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_pc_pm_pkt_parse_set(int unit,
                      bcmltd_sid_t ltid,
                      const bcmltd_field_t *key,
                      bcmtm_obm_pc_pm_pkt_parse_cfg_t *obm_pc_pm_pkt_parse_cfg,
                      bcmltd_fields_t *output_fields)
{
    bcmltd_fid_t fid;
    uint64_t fval;
    uint8_t pmid;
    SHR_FUNC_ENTER(unit);

    /* TM_OBM_PC_PM_PKT_PARSEt_PC_PM_IDf */
    fid = TM_OBM_PC_PM_PKT_PARSEt_PC_PM_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    pmid = (uint8_t)fval - 1;
    SHR_IF_ERR_EXIT
        (bcmtm_obm_pc_pm_pkt_parse_pt_set(unit, ltid, pmid, obm_pc_pm_pkt_parse_cfg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_OBM_PC_PM_PKT_PARSE logical table callback function for staging.
 *
 * Handle TM_OBM_PC_PM_PKT_PARSE table change events.
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
bcmtm_obm_pc_pm_pkt_parse_stage(int unit,
        bcmltd_sid_t ltid,
        uint32_t trans_id,
        bcmimm_entry_event_t event_reason,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    bcmltd_fields_t out, in;
    bcmltd_field_t *cur_key = (bcmltd_field_t *)key;
    size_t num_fid;
    bcmtm_obm_pc_pm_pkt_parse_cfg_t obm_pc_pm_pkt_parse_cfg;
    int idx_count = 0;
    SHR_FUNC_ENTER(unit);

    sal_memset(&obm_pc_pm_pkt_parse_cfg, 0, sizeof(bcmtm_obm_pc_pm_pkt_parse_cfg_t));
    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&in, 0, sizeof(bcmltd_fields_t));
            sal_memset(&out, 0, sizeof(bcmltd_fields_t));
            SHR_IF_ERR_EXIT
                (bcmlrd_field_count_get(unit, ltid, &num_fid));
            idx_count = 0;
            idx_count = bcmlrd_field_idx_count_get(unit, TM_OBM_PC_PM_PKT_PARSEt,
                                  TM_OBM_PC_PM_PKT_PARSEt_USER_DEFINED_PROTOCOL_MATCHf);
            idx_count += bcmlrd_field_idx_count_get(unit, TM_OBM_PC_PM_PKT_PARSEt,
                                  TM_OBM_PC_PM_PKT_PARSEt_USER_DEFINED_PROTOCOL_ETHERTYPEf);
            idx_count += bcmlrd_field_idx_count_get(unit, TM_OBM_PC_PM_PKT_PARSEt,
                                  TM_OBM_PC_PM_PKT_PARSEt_USER_DEFINED_PROTOCOL_ETHERTYPE_MASKf);
            idx_count += bcmlrd_field_idx_count_get(unit, TM_OBM_PC_PM_PKT_PARSEt,
                                  TM_OBM_PC_PM_PKT_PARSEt_USER_DEFINED_PROTOCOL_DST_MACf);
            idx_count += bcmlrd_field_idx_count_get(unit, TM_OBM_PC_PM_PKT_PARSEt,
                                  TM_OBM_PC_PM_PKT_PARSEt_USER_DEFINED_PROTOCOL_DST_MAC_MASKf);
            idx_count += bcmlrd_field_idx_count_get(unit, TM_OBM_PC_PM_PKT_PARSEt,
                                  TM_OBM_PC_PM_PKT_PARSEt_USER_DEFINED_PROTOCOL_TRAFFIC_CLASSf);
            idx_count += bcmlrd_field_idx_count_get(unit, TM_OBM_PC_PM_PKT_PARSEt,
                                  TM_OBM_PC_PM_PKT_PARSEt_OUTER_TPIDf);
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_alloc(unit, (num_fid + idx_count - 7) , &out));
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_alloc(unit, 1, &in));
            in.count = 0;
            while (cur_key) {
                  SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, &in, cur_key->id,
                                          0, cur_key->data));
                  cur_key = cur_key->next;
            }
            SHR_IF_ERR_EXIT
                (bcmimm_entry_lookup(unit, ltid, &in, &out));
            bcmtm_obm_pc_pm_pkt_parse_cfg_fill(unit, *(out.field), &obm_pc_pm_pkt_parse_cfg);
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_INTERNAL);
            output_fields->count = 0 ;
            bcmtm_obm_pc_pm_pkt_parse_cfg_fill(unit, data, &obm_pc_pm_pkt_parse_cfg);
            SHR_IF_ERR_EXIT
                (bcmtm_obm_pc_pm_pkt_parse_set(unit, ltid, key,
                                       &obm_pc_pm_pkt_parse_cfg, output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_obm_pc_pm_pkt_parse_reset(unit, ltid, key, &obm_pc_pm_pkt_parse_cfg));
            break;
        default:
            break;
    }
exit:
    if (BCMIMM_ENTRY_UPDATE == event_reason)
    {
        bcmtm_field_list_free(&in);
        bcmtm_field_list_free(&out);
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_obm_pc_pm_pkt_parse_imm_register(int unit)
{
    /*! Inmemory callback functions for TM_OBM_PORT_CONTROL LT. */
    bcmimm_lt_cb_t bcmtm_obm_pc_pm_pkt_parse_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_obm_pc_pm_pkt_parse_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_OBM_PC_PM_PKT_PARSEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for obm flow control. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_OBM_PC_PM_PKT_PARSEt,
                             &bcmtm_obm_pc_pm_pkt_parse_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
