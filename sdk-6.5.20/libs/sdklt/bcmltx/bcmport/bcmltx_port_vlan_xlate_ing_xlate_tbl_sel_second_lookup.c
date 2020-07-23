/*! \file bcmltx_port_vlan_xlate_ing_xlate_tbl_sel_second_lookup.c
 *
 * PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_SECOND_LOOKUP Transform Handler
 *
 * This file contains field transform information for
 * PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_SECOND_LOOKUP.
 *
 * PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_SECOND_LOOKUP corresponds to the field
 * arg->rfield[0](forward transform), arg->field[0](reverse transform).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmport/bcmltx_port_vlan_xlate_ing_xlate_tbl_sel_second_lookup.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_PORT

#define LT_FLD_IDX_SEL_SECOND_LOOKUP    0

#define PT_FLD_IDX_PORT_TYPE    0
#define PT_FLD_IDX_KEY_TYPE     1

typedef  enum lt_ing_xlate_tbl_sel_e {
    VLAN_ING_XLATE_IVID_OVID = 0,
    VLAN_ING_XLATE_IVID_OVID_MODPORT = 1,
    VLAN_ING_XLATE_IVID_OVID_TRUNK = 2,
    VLAN_ING_XLATE_IVID_OVID_PORT_GRP = 3,
    VLAN_ING_XLATE_OTAG = 4,
    VLAN_ING_XLATE_OTAG_MODPORT = 5,
    VLAN_ING_XLATE_OTAG_TRUNK = 6,
    VLAN_ING_XLATE_OTAG_PORT_GRP = 7,
    VLAN_ING_XLATE_ITAG = 8,
    VLAN_ING_XLATE_ITAG_MODPORT = 9,
    VLAN_ING_XLATE_ITAG_TRUNK = 10,
    VLAN_ING_XLATE_ITAG_PORT_GRP = 11,
    VLAN_ING_XLATE_OVID = 12,
    VLAN_ING_XLATE_OVID_MODPORT = 13,
    VLAN_ING_XLATE_OVID_TRUNK = 14,
    VLAN_ING_XLATE_OVID_PORT_GRP = 15,
    VLAN_ING_XLATE_IVID = 16,
    VLAN_ING_XLATE_IVID_MODPORT = 17,
    VLAN_ING_XLATE_IVID_TRUNK = 18,
    VLAN_ING_XLATE_IVID_PORT_GRP = 19,
    VLAN_ING_XLATE_CFI_PRI = 20,
    VLAN_ING_XLATE_CFI_PRI_MODPORT = 21,
    VLAN_ING_XLATE_CFI_PRI_TRUNK = 22,
    VLAN_ING_XLATE_CFI_PRI_PORT_GRP = 23
} lt_ing_xlate_tbl_sel_t;

typedef enum key_type_e {
    IVID_OVID = 0,
    OTAG = 1,
    ITAG = 2,
    OVID = 4,
    IVID = 5,
    CFI_PRI = 6
} key_type_t;

typedef enum port_type_e {
    ANY_TYPE = 0,
    MODPORT = 1,
    TRUNK = 1,
    SVP = 2,
    PORT_GRP = 3
} port_type_t;

typedef struct ing_xlate_tbl_sel_info_s {
    lt_ing_xlate_tbl_sel_t   tbl_sel;
    key_type_t               key_type;
    port_type_t              port_type;
} ing_xlate_tbl_sel_info_t;

static ing_xlate_tbl_sel_info_t ing_xlate_tbl_sel_info[] = {
    {VLAN_ING_XLATE_IVID_OVID,              IVID_OVID,        ANY_TYPE},
    {VLAN_ING_XLATE_IVID_OVID_MODPORT,      IVID_OVID,        MODPORT},
    {VLAN_ING_XLATE_IVID_OVID_TRUNK,        IVID_OVID,        TRUNK},
    {VLAN_ING_XLATE_IVID_OVID_PORT_GRP,     IVID_OVID,        PORT_GRP},
    {VLAN_ING_XLATE_OTAG,                   OTAG,             ANY_TYPE},
    {VLAN_ING_XLATE_OTAG_MODPORT,           OTAG,             MODPORT},
    {VLAN_ING_XLATE_OTAG_TRUNK,             OTAG,             TRUNK},
    {VLAN_ING_XLATE_OTAG_PORT_GRP,          OTAG,             PORT_GRP},
    {VLAN_ING_XLATE_ITAG,                   ITAG,             ANY_TYPE},
    {VLAN_ING_XLATE_ITAG_MODPORT,           ITAG,             MODPORT},
    {VLAN_ING_XLATE_ITAG_TRUNK,             ITAG,             TRUNK},
    {VLAN_ING_XLATE_ITAG_PORT_GRP,          ITAG,             PORT_GRP},
    {VLAN_ING_XLATE_OVID,                   OVID,             ANY_TYPE},
    {VLAN_ING_XLATE_OVID_MODPORT,           OVID,             MODPORT},
    {VLAN_ING_XLATE_OVID_TRUNK,             OVID,             TRUNK},
    {VLAN_ING_XLATE_OVID_PORT_GRP,          OVID,             PORT_GRP},
    {VLAN_ING_XLATE_IVID,                   IVID,             ANY_TYPE},
    {VLAN_ING_XLATE_IVID_MODPORT,           IVID,             MODPORT},
    {VLAN_ING_XLATE_IVID_TRUNK,             IVID,             TRUNK},
    {VLAN_ING_XLATE_IVID_PORT_GRP,          IVID,             PORT_GRP},
    {VLAN_ING_XLATE_CFI_PRI,                CFI_PRI,          ANY_TYPE},
    {VLAN_ING_XLATE_CFI_PRI_MODPORT,        CFI_PRI,          MODPORT},
    {VLAN_ING_XLATE_CFI_PRI_TRUNK,          CFI_PRI,          TRUNK},
    {VLAN_ING_XLATE_CFI_PRI_PORT_GRP,       CFI_PRI,          PORT_GRP}
};

/*
 * \brief PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_SECOND_LOOKUP forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_SECOND_LOOKUP field.
 * \param [out] out             PORT_TAB.VT_PORT_TYPE_SELECT_2/VT_KEY_TYPE_2 fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_port_vlan_xlate_ing_xlate_tbl_sel_second_lookup_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    size_t i;
    int rv = SHR_E_NONE;
    uint64_t key_type_val  = 0;
    uint64_t port_type_val = 0;
    int found = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_port_vlan_xlate_ing_xlate_tbl_sel_second_lookup_transform\n")));

    if (in->count == 0) {
        goto exit;
    }

    for (i = 0;
         i < (sizeof(ing_xlate_tbl_sel_info) / sizeof(ing_xlate_tbl_sel_info[0]));
         i++) {
        if (in->field[0]->data == (uint64_t)ing_xlate_tbl_sel_info[i].tbl_sel) {
            key_type_val  = (uint64_t)ing_xlate_tbl_sel_info[i].key_type;
            port_type_val = (uint64_t)ing_xlate_tbl_sel_info[i].port_type;
            found = 1;
            break;
        }
    }

    if (!found) {
        rv = SHR_E_PARAM;
        SHR_ERR_EXIT(rv);
    }

    out->count = 0;
    out->field[out->count]->id   = arg->rfield[PT_FLD_IDX_KEY_TYPE];
    out->field[out->count]->data = key_type_val;
    out->count++;
    out->field[out->count]->id   = arg->rfield[PT_FLD_IDX_PORT_TYPE];
    out->field[out->count]->data = port_type_val;
    out->count++;

 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_SECOND_LOOKUP reverse transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_TAB/VT_PORT_TYPE_SELECT_2/VT_KEY_TYPE_2fields.
 * \param [out] out             PORT_VLAN_XLATE.ING_XLATE_TBL_SEL_SECOND_LOOKUP field.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_port_vlan_xlate_ing_xlate_tbl_sel_second_lookup_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    size_t i;
    int rv = SHR_E_NONE;
    uint64_t key_type_val  = 0;
    uint64_t port_type_val = 0;
    int found = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_port_vlan_xlate_ing_xlate_tbl_sel_second_lookup_rev_transform\n")));

    if (in->count == 0) {
        goto exit;
    }

    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id == arg->field[PT_FLD_IDX_KEY_TYPE]) {
            key_type_val = in->field[i]->data;
        } else if (in->field[i]->id == arg->field[PT_FLD_IDX_PORT_TYPE]) {
            port_type_val = in->field[i]->data;
        } else {
            rv = SHR_E_FAIL;
            SHR_ERR_EXIT(rv);
        }
    }

    out->count = 0;
    for (i = 0;
         i < (sizeof(ing_xlate_tbl_sel_info) / sizeof(ing_xlate_tbl_sel_info[0]));
         i++) {
        if (key_type_val  == (uint64_t)ing_xlate_tbl_sel_info[i].key_type &&
            port_type_val == (uint64_t)ing_xlate_tbl_sel_info[i].port_type) {
            out->field[out->count]->id   = arg->rfield[LT_FLD_IDX_SEL_SECOND_LOOKUP];
            out->field[out->count]->data = (uint64_t)ing_xlate_tbl_sel_info[i].tbl_sel;
            out->count++;
            found = 1;
            break;
        }
    }

    if (!found) {
        rv = SHR_E_PARAM;
        SHR_ERR_EXIT(rv);
    }

 exit:
    SHR_FUNC_EXIT();
}
