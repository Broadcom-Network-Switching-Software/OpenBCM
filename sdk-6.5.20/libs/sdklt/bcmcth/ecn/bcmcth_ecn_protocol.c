/*! \file bcmcth_ecn_protocol.c
 *
 * BCMCTH ECN Protocol table handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_assert.h>

#include <shr/shr_debug.h>
#include <shr/shr_ha.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmcth/bcmcth_ecn_drv.h>
#include <bcmcth/bcmcth_ecn_protocol.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_ECN

/******************************************************************************
 * Local definitions
 */

/* Device-specific protocol information */
static bcmcth_ecn_proto_info_t ecn_proto_info[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief ECN protocol PT read.
 *
 * Read ECN protocol entry into physical table.
 *
 * \param [in] unit         Unit number.
 * \param [in] op_arg       Operation arguments.
 * \param [in] ip_protocol  Internet protocol.
 * \param [Out] responsive   Responsiveness of the protocol.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
ecn_protocol_pt_read(int unit, const bcmltd_op_arg_t *op_arg,
                      uint32_t ip_protocol,
                      uint32_t *responsive)
{
    bcmcth_ecn_proto_info_t *pi = &ecn_proto_info[unit];
    uint32_t responsive_bitmap[2];
    uint32_t rsp_flags = 0, table_index = 0;
    uint32_t entry_sz = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t rsp_ltid, req_ltid;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint8_t *entry_buf = NULL;
    int pos = 0;

    SHR_FUNC_ENTER(unit);

    req_ltid = ECN_PROTOCOLt;
    sid = pi->protocol_match_sid;

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthEcnHwEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

    /* Derive hardware table index */
    assert(pi->num_proto_per_entry);
    table_index = (ip_protocol / pi->num_proto_per_entry);

    /* Initialize the memory to 0. */
    sal_memset(entry_buf, 0, entry_sz);
    sal_memset(responsive_bitmap, 0, 2 * sizeof(uint32_t));

    /* Field ID. */
    fid = pi->responsive_fid;

    /* Set PT table entry */
    pt_dyn_info.index = table_index;
    pt_dyn_info.tbl_inst = -1;

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, op_arg->attrib, sid, &pt_dyn_info,
                                    NULL, NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    entry_sz/4,
                                    req_ltid,
                                    op_arg->trans_id,
                                    NULL, NULL,
                                    (uint32_t *)entry_buf,
                                    &rsp_ltid,
                                    &rsp_flags));

    /* Read hardware information. */
    bcmdrd_pt_field_get(unit, sid, (uint32_t *)entry_buf, fid,
                        responsive_bitmap);

    /* Get position information to set corresponding bit */
    pos = (ip_protocol % pi->num_proto_per_entry);

    if (SHR_BITGET(responsive_bitmap, pos)) {
        *responsive = 1;
    } else {
        *responsive = 0;
    }

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief ECN protocol PT write.
 *
 * Write ECN protocol entry into physical table.
 *
 * \param [in] unit         Unit number.
 * \param [in] op_arg       Operation arguments.
 * \param [in] ip_protocol  Internet protocol.
 * \param [in] responsive   Responsiveness of the protocol.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
ecn_protocol_pt_write(int unit, const bcmltd_op_arg_t *op_arg,
                      uint32_t ip_protocol,
                      uint32_t responsive)
{
    bcmcth_ecn_proto_info_t *pi = &ecn_proto_info[unit];
    uint32_t responsive_bitmap[2];
    uint32_t rsp_flags = 0, table_index = 0;
    uint32_t entry_sz = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t rsp_ltid, req_ltid;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint8_t *entry_buf = NULL;
    int pos = 0;

    SHR_FUNC_ENTER(unit);

    req_ltid = ECN_PROTOCOLt;
    sid = pi->protocol_match_sid;

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthEcnHwEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

    /* Derive hardware table index */
    assert(pi->num_proto_per_entry);
    table_index = (ip_protocol / pi->num_proto_per_entry);

    /* Initialize the memory to 0. */
    sal_memset(entry_buf, 0, entry_sz);
    sal_memset(responsive_bitmap, 0, 2 * sizeof(uint32_t));

    /* Field ID. */
    fid = pi->responsive_fid;

    /* Set PT table entry */
    pt_dyn_info.index = table_index;
    pt_dyn_info.tbl_inst = -1;

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, op_arg->attrib, sid, &pt_dyn_info,
                                    NULL, NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    entry_sz/4,
                                    req_ltid,
                                    op_arg->trans_id,
                                    NULL, NULL,
                                    (uint32_t *)entry_buf,
                                    &rsp_ltid,
                                    &rsp_flags));

    /* Read hardware information. */
    bcmdrd_pt_field_get(unit, sid, (uint32_t *)entry_buf, fid,
                        responsive_bitmap);

    /* Get position information to set corresponding bit */
    pos = (ip_protocol % pi->num_proto_per_entry);

    /* Use 64-bit macros to set bitmap. */
    if (!responsive) {
        SHR_BITCLR(responsive_bitmap, pos);
    } else {
        SHR_BITSET(responsive_bitmap, pos);
    }

    /* Set the updated value in pt table entry */
    bcmdrd_pt_field_set(unit, sid, (uint32_t *)entry_buf, fid,
                        responsive_bitmap);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, op_arg->attrib, sid, &pt_dyn_info,
                                    NULL, NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)entry_buf,
                                    0,
                                    req_ltid,
                                    op_arg->trans_id,
                                    NULL, NULL, NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

static int
ecn_protocol_lt_parse(int unit,
                      const bcmltd_field_t *list,
                      uint32_t fid,
                      bool *found,
                      uint32_t *val)
{
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    *found = false;
    /* Parse field list */
    field = list;
    while (field) {
        if (fid == field->id) {
            *found = true;
            *val = field->data;
            break;
        }

        field = field->next;
    }

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmcth_ecn_proto_info_init(int unit)
{
    /* Get device-specific data */
    return bcmcth_ecn_proto_info_get(unit, &ecn_proto_info[unit]);
}

int
bcmcth_ecn_protocol_insert(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmimm_entry_event_t event,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data)
{
    bool     found = false;
    uint32_t ip_proto = 0;
    uint32_t responsive = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ecn_protocol_lt_parse(unit,
                               key,
                               ECN_PROTOCOLt_IP_PROTOf,
                               &found,
                               &ip_proto));
    if (found == false) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (ecn_protocol_lt_parse(unit,
                               data,
                               ECN_PROTOCOLt_RESPONSIVEf,
                               &found,
                               &responsive));
    if (found == false) {
        if (event == BCMIMM_ENTRY_UPDATE) {
            /* No change, exiting. */
            SHR_EXIT();
        } else {
            responsive = 0;
        }
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Setting responsive state %d for protocol %d\n"),
              ip_proto, responsive));

    SHR_IF_ERR_EXIT
        (ecn_protocol_pt_write(unit, op_arg, ip_proto, responsive));


    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ecn_protocol_delete(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_field_t *key)
{
    bool     found = false;
    uint32_t ip_proto = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ecn_protocol_lt_parse(unit,
                               key,
                               ECN_PROTOCOLt_IP_PROTOf,
                               &found,
                               &ip_proto));

    SHR_IF_ERR_EXIT
        (ecn_protocol_pt_write(unit, op_arg, ip_proto, 0));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ecn_protocol_lookup(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *key,
                           bcmltd_fields_t *data)
{
    uint32_t ip_proto = 0;
    uint32_t responsive = 0;
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < key->count; i++) {
        if (key->field[i]->id == ECN_PROTOCOLt_IP_PROTOf) {
            ip_proto = key->field[i]->data;
            break;
        }
    }
    if (i == key->count) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Getting responsive state for protocol %d\n"),
              ip_proto));

    SHR_IF_ERR_EXIT
        (ecn_protocol_pt_read(unit, op_arg, ip_proto, &responsive));

    for (i = 0; i < data->count; i++) {
        if (data->field[i]->id == ECN_PROTOCOLt_RESPONSIVEf) {
            data->field[i]->data = responsive;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}
