/*! \file bcmtm_imm_ebst_sp.c
 *
 * BCMTM EBST interface to in-memory table for queues.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/ebst/bcmtm_ebst_internal.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/******************************************************************************
 * Private functions
 */
/*!
 * \brief EBST service pool mapping for populating data LT.
 *
 * \param [in] unit Logical unit number.
 * \param [in] spid Service pool ID.
 * \param [in] sp_cfg Service pool configuration.
 *
 * \retval SHR_E_NONE No error. Returns corresponding
 * error code if there is any error.
 */
static int
bcmtm_ebst_sp_map_get(int unit,
                      uint16_t spid,
                      bcmtm_ebst_entity_map_t **sp_map)
{
    bcmtm_drv_t *drv;
    bcmtm_ebst_dev_info_t *ebst_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    SHR_NULL_CHECK(ebst_info->map_info.sp_map, SHR_E_NOT_FOUND);
    *sp_map = &(ebst_info->map_info.sp_map[spid]);
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Parse TM_EBST_SERVICE_POOL logical table configurations.
 *
 * \param [in] unit Logical unit number.
 * \param [in] data Data field list.
 * \param [in] sp_cfg LT service pool configurations.
 */
static void
bcmtm_ebst_sp_parse(int unit,
                    const bcmltd_field_t *data,
                    bcmtm_ebst_sp_cfg_t *sp_cfg)
{
    const bcmltd_field_t *cur_data = data;
    uint64_t fval;

    while (cur_data) {
        fval = cur_data->data;
        switch (cur_data->id) {
            case TM_EBST_SERVICE_POOLt_MONITORf:
                sp_cfg->enable = fval;
                break;
            case TM_EBST_SERVICE_POOLt_BASE_INDEXf:
                sp_cfg->sp_map.base_id = fval;
                break;
            case TM_EBST_SERVICE_POOLt_NUM_ENTRIESf:
                sp_cfg->sp_map.num_entry = fval;
                break;
            case TM_EBST_SERVICE_POOLt_OPERATIONAL_STATEf:
                sp_cfg->opcode = fval;
                break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief TM_EBST_SERVICE_POOL in-memory lookup.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] sp_id Unicast queue ID.
 * \param [out] queue_cfg Queue configuration from in-memory lookup.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_ebst_sp_imm_lkup(int unit,
                        bcmltd_sid_t ltid,
                        uint16_t sp_id,
                        bcmtm_ebst_sp_cfg_t *sp_cfg)
{
    bcmltd_fields_t out, in;
    bcmltd_fid_t fid;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fid));
    fid = TM_EBST_SERVICE_POOLt_TM_EBST_DATA_IDf;

    num_fid += bcmlrd_field_idx_count_get(unit, ltid, fid);
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));
    in.count = 0;

    /*! TM_EBST_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf */
    fid = TM_EBST_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, sp_id));

    if (SHR_FAILURE(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        sp_cfg->opcode = ENTRY_INVALID;
    } else {
        bcmtm_ebst_sp_parse(unit, *(out.field), sp_cfg);
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_EBST_SERVICE_POOL imm stage lookup handler.
 *
 * This populates fields manipulated by hardware and software not having a
 * updated copy in in-memory.
 *
 * \param [in] unit Logical unit number.
 * \param [in] sid Logical table ID.
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
bcmtm_ebst_sp_imm_stage_lookup(int unit,
                               bcmltd_sid_t ltid,
                               uint32_t trans_id,
                               void *context,
                               bcmimm_lookup_type_t lkup_type,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out)
{
    bcmltd_fid_t fid;
    uint64_t fval;
    uint16_t spid, idx;
    const bcmltd_fields_t *key_fields;
    bcmtm_ebst_entity_map_t *sp_map;
    int buffer_pool;

    SHR_FUNC_ENTER(unit);

    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    /* TM_EBST_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf */
    fid = TM_EBST_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0,
                                         *(key_fields->field), &fval));
    spid = (uint16_t)fval;

    if (SHR_SUCCESS(bcmtm_ebst_sp_map_get(unit, spid, &sp_map))) {
        fid = TM_EBST_SERVICE_POOLt_TM_EBST_DATA_IDf;
        buffer_pool = bcmlrd_field_idx_count_get(unit, ltid, fid);
        for (idx = 0; idx < buffer_pool ; idx++) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, out, fid, idx,
                                      sp_map->write_ptr[idx]));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EBST Service pool IMM table change callback function for staging.
 *
 * Handle EBST Service pool IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid This is the logical table ID.
 * \param [in] trans_id This is the logical table transaction ID.
 * \param [in] event_reason This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] output_fields Read only fields update to imm.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL BST fails to handle LT change events.
 */
static int
bcmtm_ebst_sp_imm_stage_cb(int unit,
                           bcmltd_sid_t ltid,
                           uint32_t trans_id,
                           bcmimm_entry_event_t event_reason,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           void *context,
                           bcmltd_fields_t *output_fields)
{
    bcmtm_ebst_sp_cfg_t sp_cfg;
    bcmltd_fid_t fid;
    uint64_t fval;
    uint16_t spid;
    bcmtm_ebst_entity_map_t *sp_map;
    bcmtm_ebst_dev_info_t *ebst_info;

    SHR_FUNC_ENTER(unit);
    sal_memset(&sp_cfg, 0, sizeof(bcmtm_ebst_sp_cfg_t));

    /* TM_EBST_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf */
    fid = TM_EBST_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    spid = (uint16_t)fval;
    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_sp_imm_lkup(unit, ltid, spid, &sp_cfg));
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_INTERNAL);
            output_fields->count = 0;
            bcmtm_ebst_sp_parse(unit, data, &sp_cfg);

            /* update the OPERATIONAL_STATE */
            sp_cfg.opcode = (ebst_info->monitor > 0) ? 1: 0;
            fid = TM_EBST_SERVICE_POOLt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields, fid, 0,
                                      sp_cfg.opcode));
            break;
        case BCMIMM_ENTRY_DELETE:
            break;
        default:
            break;
    }

    if (SHR_SUCCESS(bcmtm_ebst_sp_map_get(unit, spid, &sp_map))) {
        *sp_map = sp_cfg.sp_map;
        sal_memset(sp_map->write_ptr, -1, sizeof(int) * MAX_BUFFER_POOL);
        sp_map->max_id = sp_map->base_id + sp_map->num_entry - 1;
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_imm_ebst_sp_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    /* This structure contains callback functions that will be corresponding
     * to TM_EBST_SERVICE_POOL logical table entry commit stages.
     */
    static bcmimm_lt_cb_t bcmtm_ebst_sp_imm_cb = {
        .stage = bcmtm_ebst_sp_imm_stage_cb,
        .lookup = bcmtm_ebst_sp_imm_stage_lookup
    };
    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_EBST_SERVICE_POOLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, TM_EBST_SERVICE_POOLt,
                                 &bcmtm_ebst_sp_imm_cb, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_ebst_sp_map_populate(int unit, bcmltd_sid_t ltid)
{
    const bcmlrd_map_t *map = NULL;
    bcmltd_fid_t fid;
    bcmltd_fields_t in, out;
    uint8_t spid = 0;
    bcmtm_ebst_sp_cfg_t sp_cfg;
    bcmtm_ebst_entity_map_t *sp_map;
    int rv;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, ltid, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, 1, &in));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, TM_EBST_SERVICE_POOLt_FIELD_COUNT, &out));

    in.count = 0;
    rv = bcmimm_entry_get_first(unit, ltid, &out);
    while (SHR_SUCCESS(rv)) {

        fid = TM_EBST_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf;
        SHR_IF_ERR_EXIT(bcmtm_field_list_get(unit, &out, fid, 0, &fval));
        SHR_IF_ERR_EXIT(bcmtm_field_list_set(unit, &in, fid, 0, fval));
        spid = (uint8_t)fval;

        sal_memset(&sp_cfg, 0, sizeof(bcmtm_ebst_sp_cfg_t));
        bcmtm_ebst_sp_parse(unit, *(out.field), &sp_cfg);

        if (SHR_SUCCESS(bcmtm_ebst_sp_map_get(unit, spid, &sp_map))) {
            *sp_map = sp_cfg.sp_map;
            sal_memset(sp_map->write_ptr, -1, sizeof(int)*MAX_BUFFER_POOL);
            sp_map->max_id = sp_map->base_id + sp_map->num_entry - 1;
        }
        out.count = TM_EBST_SERVICE_POOLt_FIELD_COUNT;
        rv = bcmimm_entry_get_next(unit, ltid, &in, &out);
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}
