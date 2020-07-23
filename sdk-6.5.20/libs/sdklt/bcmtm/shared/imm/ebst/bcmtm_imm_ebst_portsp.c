/*! \file bcmtm_imm_ebst_portsp.c
 *
 * BCMTM EBST port service pool in-memory interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/ebst/bcmtm_ebst_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/******************************************************************************
 * Private functions
 */
/*!
 * \brief parsing EBST port serive pool configuration.
 *
 * \param [in] unit Logical unit number.
 * \param [in] data Field list data from application.
 * \param [out] portsp_cfg Port service pool configuration.
 */
static void
bcmtm_ebst_portsp_parse(int unit,
                        const bcmltd_field_t *data,
                        bcmtm_ebst_portsp_cfg_t *portsp_cfg)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    bcmtm_ebst_portsp_map_t *portsp_map = &(portsp_cfg->portsp_map);

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case TM_EBST_PORT_SERVICE_POOLt_UC_BASE_INDEXf:
                portsp_map->uc_sp.base_id = fval;
                break;
            case TM_EBST_PORT_SERVICE_POOLt_UC_NUM_ENTRIESf:
                portsp_map->uc_sp.num_entry = fval;
                break;
            case TM_EBST_PORT_SERVICE_POOLt_MC_BASE_INDEXf:
                portsp_map->mc_sp.base_id = fval;
                break;
            case TM_EBST_PORT_SERVICE_POOLt_MC_NUM_ENTRIESf:
                portsp_map->mc_sp.num_entry = fval;
                break;
            default:
                break;
        }
        gen_field = gen_field->next;
    }
}

/*!
 * \brief Get the EBST mappings for TM_EBST_DATA table for queues.
 *
 * This returns the queue map for queue types used for populating data table.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port ID.
 * \param [in] spid Service pool ID. .
 * \param [out] portsp_map Port service pool mapping.
 *
 * \retval SHR_E_INIT EBST driver function not initialized.
 * \retval SHR_E_NOT_FOUND Mapping not found for the port and service pool ID.
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_ebst_portsp_map_get(int unit,
                          bcmtm_lport_t lport,
                          uint8_t spid,
                          bcmtm_ebst_portsp_map_t **portsp_map)
{

    bcmtm_ebst_dev_info_t *ebst_info;
    bcmtm_ebst_drv_t ebst_drv;
    bcmtm_drv_t *drv;
    uint16_t map_idx;

    SHR_FUNC_ENTER(unit);

    bcmtm_ebst_dev_info_get(unit, &ebst_info);
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->ebst_drv_get, SHR_E_INIT);

    SHR_IF_ERR_EXIT(drv->ebst_drv_get(unit, &ebst_drv));
    SHR_IF_ERR_EXIT
        (ebst_drv.ebst_portsp_index_get(unit, lport, spid, &map_idx));

    SHR_NULL_CHECK(ebst_info->map_info.portsp_map, SHR_E_NOT_FOUND);
    *portsp_map = &(ebst_info->map_info.portsp_map[map_idx]);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_EBST_PORT_SERVICE_POOLt imm lookup and parsing the data.
 *
 * Gets the value programmed by application from IMM and parse the data to
 * populate q_cfg.
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] spid Service pool ID.
 * \param [in] portsp_cfg Port service pool configuration.
 *
 * \retval SHR_E_PARAM Error in parameters.
 * \retval SHR_E_NOT_FOUND Field not found in the list.
 * \retval SHR_E_MEMORY Error in allocation of field list.
 * \retval SHR_E_NONE No error.
 */
static int
bcmtm_ebst_portsp_lkup(int unit,
                       bcmltd_sid_t ltid,
                       bcmtm_lport_t lport,
                       uint8_t spid,
                       bcmtm_ebst_portsp_cfg_t *portsp_cfg)
{
    bcmltd_fields_t out, in;
    bcmltd_fid_t fid;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    fid = TM_EBST_PORT_SERVICE_POOLt_MC_TM_EBST_DATA_IDf;
    num_fid = TM_EBST_UC_Qt_FIELD_COUNT +
            2 * (bcmlrd_field_idx_count_get(unit, ltid, fid));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in));
    in.count = 0;

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    /* TM_EBST_PORT_SERVICE_POOLt_PORT_IDf */
    fid = TM_EBST_PORT_SERVICE_POOLt_PORT_IDf;
    SHR_IF_ERR_EXIT(bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_EBST_PORT_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf */
    fid = TM_EBST_PORT_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf;
    SHR_IF_ERR_EXIT(bcmtm_field_list_set(unit, &in, fid, 0, lport));

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        bcmtm_ebst_portsp_parse(unit, *(out.field), portsp_cfg);
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_EBST_PORT_SERVICE_POOL imm lookup handler.
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
bcmtm_ebst_portsp_imm_lkup_cb(int unit,
                              bcmltd_sid_t ltid,
                              uint32_t trans_id,
                              void *context,
                              bcmimm_lookup_type_t lkup_type,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out)
{
    bcmltd_fid_t fid;
    uint64_t fval;
    bcmtm_lport_t lport;
    uint8_t spid, idx;
    const bcmltd_fields_t *key_fields;
    bcmtm_ebst_dev_info_t *ebst_info;
    bcmtm_ebst_portsp_map_t *portsp_map;

    SHR_FUNC_ENTER(unit);

    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    /* TM_EBST_PORT_SERVICE_POOLt_PORT_IDf */
    fid = TM_EBST_PORT_SERVICE_POOLt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, (bcmltd_fields_t *) key_fields,
                              fid, 0, &fval));
    lport = (bcmtm_lport_t)fval;

    /* TM_EBST_PORT_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf */
    fid = TM_EBST_PORT_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, (bcmltd_fields_t *) key_fields,
                              fid, 0, &fval));
    spid = (uint8_t)fval;

    bcmtm_ebst_dev_info_get(unit, &ebst_info);
    if (SHR_SUCCESS(bcmtm_ebst_portsp_map_get(unit, lport, spid, &portsp_map))) {

        for (idx = 0; idx < MAX_BUFFER_POOL; idx++) {
            fid = TM_EBST_PORT_SERVICE_POOLt_UC_TM_EBST_DATA_IDf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, out, fid, idx,
                                      portsp_map->uc_sp.write_ptr[idx]));

            fid = TM_EBST_PORT_SERVICE_POOLt_MC_TM_EBST_DATA_IDf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, out, fid, idx,
                                      portsp_map->mc_sp.write_ptr[idx]));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EBST port service pool table change callback function for staging.
 *
 * Handle EBST port service pool IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid This is the logical table ID.
 * \param [in] trans_id Transaction ID.
 * \param [in] event_reason This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] output_fields Read only fields update to imm.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL EBST fails to handle LT change events.
 */
static int
bcmtm_ebst_portsp_imm_stage_cb(int unit,
                               bcmltd_sid_t ltid,
                               uint32_t trans_id,
                               bcmimm_entry_event_t event_reason,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               void *context,
                               bcmltd_fields_t *output_fields)
{
    bcmtm_ebst_portsp_cfg_t portsp_cfg;
    bcmltd_fid_t fid;
    uint64_t fval;
    bcmtm_lport_t lport;
    uint8_t spid;
    bcmtm_ebst_dev_info_t *ebst_info;
    bcmtm_ebst_portsp_map_t *portsp_map;

    SHR_FUNC_ENTER(unit);

    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    sal_memset(&portsp_cfg, 0, sizeof(bcmtm_ebst_portsp_cfg_t));
    if (output_fields) {
        output_fields->count = 0;
    }
    /* TM_EBST_PORT_SERVICE_POOLt_PORT_IDf */
    fid = TM_EBST_PORT_SERVICE_POOLt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t)fval;

    /* TM_EBST_PORT_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf */
    fid = TM_EBST_PORT_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    spid = (uint8_t)fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            /* get the configuration from IMM. */
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_portsp_lkup(unit, ltid, lport, spid, &portsp_cfg));
            /* Fall through */
        case BCMIMM_ENTRY_INSERT:
            bcmtm_ebst_portsp_parse(unit, data, &portsp_cfg);
            break;
        case BCMIMM_ENTRY_DELETE:
            break;
        default:
            break;
    }
    if (SHR_SUCCESS(bcmtm_ebst_portsp_map_get(unit, lport, spid, &portsp_map))) {
        *portsp_map = portsp_cfg.portsp_map;
        sal_memset(portsp_map->uc_sp.write_ptr, -1, sizeof(int)*MAX_BUFFER_POOL);
        sal_memset(portsp_map->mc_sp.write_ptr, -1, sizeof(int)*MAX_BUFFER_POOL);
        portsp_map->uc_sp.max_id =
                portsp_map->uc_sp.base_id + portsp_map->uc_sp.num_entry - 1;
        portsp_map->mc_sp.max_id =
                portsp_map->mc_sp.base_id + portsp_map->mc_sp.num_entry - 1;
    }
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */
int
bcmtm_ebst_portsp_map_populate(int unit, bcmltd_sid_t ltid)
{
    const bcmlrd_map_t *map = NULL;
    bcmltd_fid_t fid;
    bcmltd_fields_t in, out;
    bcmtm_lport_t lport = 0;
    uint8_t spid = 0;
    bcmtm_ebst_portsp_cfg_t portsp_cfg;
    bcmtm_ebst_portsp_map_t *portsp_map;
    int rv;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, ltid, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, 2, &in));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, TM_EBST_PORT_SERVICE_POOLt_FIELD_COUNT, &out));

    in.count = 0;
    rv = bcmimm_entry_get_first(unit, ltid, &out);
    while (SHR_SUCCESS(rv)) {
        fid = TM_EBST_PORT_SERVICE_POOLt_PORT_IDf;
        SHR_IF_ERR_EXIT(bcmtm_field_list_get(unit, &out, fid, 0, &fval));
        SHR_IF_ERR_EXIT(bcmtm_field_list_set(unit, &in, fid, 0, fval));
        lport = (bcmtm_lport_t)fval;

        fid = TM_EBST_PORT_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf;
        SHR_IF_ERR_EXIT(bcmtm_field_list_get(unit, &out, fid, 0, &fval));
        SHR_IF_ERR_EXIT(bcmtm_field_list_set(unit, &in, fid, 0, fval));
        spid = (uint8_t)fval;

        sal_memset(&portsp_cfg, 0, sizeof(bcmtm_ebst_portsp_cfg_t));
        bcmtm_ebst_portsp_parse(unit, *(out.field), &portsp_cfg);

        if (SHR_SUCCESS(bcmtm_ebst_portsp_map_get(unit, lport, spid, &portsp_map))) {
            *portsp_map = portsp_cfg.portsp_map;
            sal_memset(portsp_map->uc_sp.write_ptr, -1, sizeof(int)*MAX_BUFFER_POOL);
            sal_memset(portsp_map->mc_sp.write_ptr, -1, sizeof(int)*MAX_BUFFER_POOL);
            portsp_map->uc_sp.max_id =
                portsp_map->uc_sp.base_id + portsp_map->uc_sp.num_entry - 1;
            portsp_map->mc_sp.max_id =
                portsp_map->mc_sp.base_id + portsp_map->mc_sp.num_entry - 1;
        }
        out.count = TM_EBST_PORT_SERVICE_POOLt_FIELD_COUNT;
        rv = bcmimm_entry_get_next(unit, ltid, &in, &out);
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

int
bcmtm_imm_ebst_portsp_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    /*
     * This structure contains callback function for TM_EBST_PORT_SERVICE_POOL
     */
    static bcmimm_lt_cb_t bcmtm_ebst_portsp_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_ebst_portsp_imm_stage_cb,
        .lookup = bcmtm_ebst_portsp_imm_lkup_cb
    };

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_EBST_PORT_SERVICE_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_EBST_PORT_SERVICE_POOLt,
                             &bcmtm_ebst_portsp_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
