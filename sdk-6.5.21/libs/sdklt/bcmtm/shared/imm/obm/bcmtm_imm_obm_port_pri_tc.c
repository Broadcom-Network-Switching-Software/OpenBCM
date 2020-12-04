/*! \file bcmtm_imm_obm_port_pri_tc.c
 *
 * In-memory call back function for OBM mapping  internal prriority to
 * traffic class for all ports.
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
#include <bcmtm/bcmtm_utils.h>
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
 * \brief  OBM internal priority to traffic class physical table
 * configuration based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port number.
 * \param [in] pkt_type Packet type.
 * \param [in] pkt_pri Packet priority.
 * \param [in] tc Traffic class.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_port_pkt_pri_tc_pt_set(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmtm_lport_t lport,
                                 uint8_t pkt_type,
                                 uint8_t pkt_pri,
                                 uint8_t tc)
{
    bcmtm_drv_t *drv;
    bcmtm_obm_drv_t obm_drv;

    SHR_FUNC_ENTER(unit);
    sal_memset(&obm_drv, 0, sizeof(bcmtm_obm_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->obm_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->obm_drv_get(unit, &obm_drv));
    /* update physical table. */
    if (obm_drv.pkt_pri_tc_set) {
        SHR_IF_ERR_EXIT
            (obm_drv.pkt_pri_tc_set(unit, ltid, lport, pkt_type, pkt_pri, tc));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Resets the physical table to default values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] pkt_type Packet type.
 * \param [in] pkt_pri Packet priority.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_port_pkt_pri_tc_reset(int unit,
                                bcmltd_sid_t ltid,
                                bcmtm_lport_t lport,
                                uint8_t pkt_type,
                                uint8_t pkt_pri)
{
    int mport;

    SHR_FUNC_ENTER(unit);
    if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
         (bcmtm_obm_port_pkt_pri_tc_pt_set(unit, ltid, lport,
                                           pkt_type, pkt_pri, 0));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  OBM port internal priority traffic class IMM lookup.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] pkt_type Packet type.
 * \param [in] pkt_pri Packet priority.
 * \param [out] tc Traffic class.
 * \param [out] opcode Operational status code.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_port_pkt_pri_tc_imm_lkup(int unit,
                                   bcmltd_sid_t ltid,
                                   bcmtm_lport_t lport,
                                   uint8_t pkt_type,
                                   uint8_t pkt_pri,
                                   uint8_t *tc,
                                   uint8_t *opcode)
{

    bcmltd_fields_t out, in;
    bcmltd_fid_t fid;
    size_t num_fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    *tc = 0;
    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 3, &in));
    in.count = 0;

    /* TM_OBM_PORT_PKT_PRI_TC_MAPt_PORT_IDf */
    fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRI_TYPEf */
    fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRI_TYPEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, pkt_type));

    /* TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRIf */
    fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRIf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, pkt_pri));

    if (SHR_FAILURE(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        *opcode = ENTRY_INVALID;
    } else {
        fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_TRAFFIC_CLASSf;
        if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))){
            *tc = (uint8_t)fval;
        }
        fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_OPERATIONAL_STATEf;
        SHR_IF_ERR_EXIT
            (bcmtm_fval_get_from_field_array(unit, fid, 0, *(out.field), &fval));
        *opcode = (uint8_t)fval;
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();

}

/*!
 * \brief  OBM port internal priority traffic class IMM update.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] pkt_type Packet type.
 * \param [in] pkt_pri Packet priority.
 * \param [in] opcode Operational state code.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_port_pkt_pri_tc_imm_update(int unit,
                                     bcmltd_sid_t ltid,
                                     bcmtm_lport_t lport,
                                     uint8_t pkt_type,
                                     uint8_t pkt_pri,
                                     uint8_t opcode)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 4, &in));

    in.count = 0;

    /* TM_OBM_PORT_PKT_PRI_TC_MAPt_PORT_IDf */
    fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRI_TYPEf */
    fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRI_TYPEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, pkt_type));

    /* TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRIf */
    fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRIf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, pkt_pri));

    fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, opcode));
    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));

exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the physical table with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] pkt_type Priority based on packet type.
 * \param [in] pkt_pri Packet Priority of packet based on packet type.
 * \param [in] tc Traffic class.
 * \param [in] action Update/delete action.
 * \param [out] opcode Operational status of the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_port_pkt_pri_tc_set(int unit,
                              bcmltd_sid_t ltid,
                              bcmtm_lport_t lport,
                              uint8_t pkt_type,
                              uint8_t pkt_pri,
                              uint8_t tc,
                              uint8_t action,
                              uint8_t *opcode)
{
    int mport, hg3 = 0;

    SHR_FUNC_ENTER(unit);

    if (!bcmtm_obm_port_validation(unit, lport)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (action) {
        case ACTION_INSERT:
            /*
             * TM local port information.
             * Returns error if local port to physical port mapping is not available.
             * Updates operational state.
             */
            if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
                *opcode = PORT_INFO_UNAVAIL;
                SHR_EXIT();
            }
            /* Fall through */
        case ACTION_PORT_UPDATE_INT:
            if (SHR_SUCCESS(bcmtm_port_is_higig3(unit, lport, &hg3)) &&
                (((pkt_type == OBM_PRIORITY_TYPE_VLAN) && hg3) ||
                 ((pkt_type == OBM_PRIORITY_TYPE_HIGIG3) && !hg3))) {
                    *opcode = PORT_ENCAP_MISMATCH;
                    SHR_EXIT();
            }
            SHR_IF_ERR_EXIT
                (bcmtm_obm_port_pkt_pri_tc_pt_set(unit, ltid, lport,
                                                  pkt_type, pkt_pri, tc));
            /* opcode success */
            *opcode = VALID;
            break;

        case ACTION_PORT_DELETE_INT:
            (bcmtm_obm_port_pkt_pri_tc_reset(unit, ltid, lport,
                                                pkt_type, pkt_pri));
            *opcode = PORT_INFO_UNAVAIL;
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_OBM_PORT_PKT_PRI_TC_MAP logical table callback function for staging.
 *
 * Handle TM_OBM_PORT_PKT_PRI_TC_MAP table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid This is the logical table ID.
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
bcmtm_port_pkt_pri_tc_stage(int unit,
        bcmltd_sid_t ltid,
        uint32_t trans_id,
        bcmimm_entry_event_t event_reason,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    bcmtm_lport_t lport;
    bcmltd_fid_t fid;
    uint8_t pkt_pri, pkt_type, tc = 0, opcode;
    uint8_t action;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* TM_OBM_PORT_PKT_PRI_TC_MAPt_PORT_IDf */
    fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t)fval;

    if (!bcmtm_obm_port_validation(unit, lport)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRI_TYPEf */
    fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRI_TYPEf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    pkt_type = (uint8_t) fval;

    /* TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRIf */
    fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRIf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    pkt_pri = (uint8_t) fval;

    /*
     * DSCP and HIGIG3 supports 64 packet priorities. All other packet types
     * support 8 packet priorities.
     */
    if (pkt_pri > 7 &&
        ((pkt_type != OBM_PRIORITY_TYPE_DSCP) &&
         (pkt_type != OBM_PRIORITY_TYPE_HIGIG3))) {
        SHR_ERR_EXIT(SHR_E_PARAM);;
    }
    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_INTERNAL);
            output_fields->count = 0;
            /* TM_OBM_PORT_PKT_PRI_TC_MAPt_TRAFFIC_CLASSf */
            if (data) {
                fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_TRAFFIC_CLASSf;
                SHR_IF_ERR_EXIT
                    (bcmtm_fval_get_from_field_array(unit, fid, 0, data, &fval));
                tc = (uint8_t) fval;
            }
            action = ACTION_INSERT;
            SHR_IF_ERR_EXIT
                (bcmtm_obm_port_pkt_pri_tc_set(unit, ltid, lport, pkt_type,
                                               pkt_pri, tc, action, &opcode));
            fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields, fid, 0, opcode));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_obm_port_pkt_pri_tc_reset(unit, ltid, lport,
                                                 pkt_type, pkt_pri));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_obm_port_pkt_pri_tc_internal_update(int unit,
                                          bcmtm_lport_t lport,
                                          uint8_t action)
{
    uint64_t pkttype_min, pkttype_max, pkttype;
    uint64_t pri_min, pri_max, pri;
    uint8_t tc = 0, opcode;

    bcmltd_sid_t ltid = TM_OBM_PORT_PKT_PRI_TC_MAPt;
    bcmltd_fid_t fid;
    SHR_FUNC_ENTER(unit);

    /* Get number of pkytype supported. */
    fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRI_TYPEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, ltid, fid,
                                     &pkttype_min, &pkttype_max));

    fid = TM_OBM_PORT_PKT_PRI_TC_MAPt_PKT_PRIf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, ltid, fid, &pri_min, &pri_max));

    for (pkttype = pkttype_min; pkttype < pkttype_max; pkttype++) {

        for (pri = pri_min; pri < pri_max; pri++) {
            if (pri > 7 && pkttype != OBM_PRIORITY_TYPE_DSCP) {
                break;
            }
            SHR_IF_ERR_EXIT
                (bcmtm_obm_port_pkt_pri_tc_imm_lkup(unit, ltid, lport, pkttype,
                                                    pri, &tc, &opcode));
            if (opcode != ENTRY_INVALID) {
                SHR_IF_ERR_EXIT
                    (bcmtm_obm_port_pkt_pri_tc_set(unit, ltid, lport, pkttype,
                                                   pri, tc, action, &opcode));
                SHR_IF_ERR_EXIT
                    (bcmtm_obm_port_pkt_pri_tc_imm_update(unit, ltid, lport,
                                                          pkttype, pri, opcode));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_obm_port_pkt_pri_tc_imm_register(int unit)
{
    /*! Inmemory callback functions for TM_OBM_PORT_PKT_PRI_TC_MAP LT. */
    bcmimm_lt_cb_t bcmtm_obm_port_pkt_pri_tcimm_cb = {
        /*! Staging function. */
        .stage = bcmtm_port_pkt_pri_tc_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_OBM_PORT_PKT_PRI_TC_MAPt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for internal priority to TC mapping. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_OBM_PORT_PKT_PRI_TC_MAPt,
                             &bcmtm_obm_port_pkt_pri_tcimm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
