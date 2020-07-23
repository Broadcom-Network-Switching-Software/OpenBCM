/*! \file bcmltx_std_table_validator.c
 *
 * Built-in Field Validation functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>

#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmltx/bcmltx_std_table_validator.h>
#include <bcmltx/bcmltx_util.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

int
bcmltx_lta_val_port_bitmap(int unit,
                           bcmlt_opcode_t opcode,
                           const bcmltd_fields_t *in,
                           const bcmltd_field_val_arg_t *arg)
{
    int rv;
    uint32_t port, port_num, fid, aix;
    uint32_t field_val[BCMDRD_MAX_PT_WSIZE];
    bcmltx_field_list_t *field_ptr;
    const bcmlrd_table_rep_t *tbl;

    if ((arg->fields != 3) ||
        (arg->comp_data == NULL)) {
        /* Not supported yet */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Inconsistent LTA information\n")));
        return FALSE;
    }

    if (arg->field[0] == BCMLTX_FAVC_PORT_LOGICAL) {
        port_num = arg->field[1];
        fid = arg->field[2];
        for (port = 0; port < port_num; port++) {
            if ((port % BCM_FIELD_SIZE_BITS) == 0) {
                /* New field index needed */
                aix = port / BCM_FIELD_SIZE_BITS;
                field_ptr = bcmltx_lta_find_field(in, fid, aix);
                if (field_ptr == NULL) {
                    /* Missing array element in list */
                    tbl = bcmlrd_table_get(arg->comp_data->sid);
                    if ((tbl != NULL) &&
                        (fid < tbl->fields)) {
                        /* Field validation failure */
                        LOG_VERBOSE(BSL_LOG_MODULE,
                                    (BSL_META_U(unit,
                                                "LTM %s: Missing field value"
                                                " for field %s[%d]\n"),
                                     tbl->name,
                                     tbl->field[fid].name,
                                     aix));
                    }
                    return SHR_E_PARAM;
                }
                sal_memset(field_val, 0, sizeof(field_val));
                rv = bcmltx_field_data_to_uint32_array(field_ptr,
                                                       field_val);
                if (SHR_E_NONE != rv) {
                    return rv;
                }
            }
            if (SHR_BITGET(field_val, (port % BCM_FIELD_SIZE_BITS))) {
            }
        }
    } else {
        /* Not supported yet */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Port Space %d not supported in pbm check\n"),
                                arg->field[0]));
        return FALSE;
    }

    return TRUE;
}
