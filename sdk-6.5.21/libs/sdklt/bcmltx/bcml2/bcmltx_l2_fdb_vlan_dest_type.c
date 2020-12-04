/*! \file bcmltx_l2_fdb_vlan_dest_type.c
 *
 * L2_FDB_VLAN.DEST_TYPE Transform Handler
 *
 * This file contains field transform information for
 * L2_FDB_VLAN.DEST_TYPE.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcml2/bcmltx_l2_fdb_vlan_dest_type.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_L2

#define LT_KEY_FLD_IDX_MAC_ADDR  0
#define LT_KEY_FLD_COUNT          (LT_KEY_FLD_IDX_MAC_ADDR + 1)

#define LT_FLD_IDX_DEST_TYPE      0
#define LT_FLD_COUNT              (LT_FLD_IDX_DEST_TYPE + 1)

#define PT_FLD_IDX_DEST_TYPE      0
#define PT_FLD_COUNT              (PT_FLD_IDX_DEST_TYPE + 1)

typedef enum lt_dest_type_s {
    LT_PORT      = 0,
    LT_L2_MC_GRP = 1,
    LT_TRUNK     = 2
} lt_dest_type_t;

/******************************************************************************
* Public functions
 */
/*
 * \brief L2_FDB_VLAN destination forward transform.
 *
 * \param [in]  unit Unit number.
 * \param [in]  in_key   L2_FDB_VLAN.MAC_ADDR.
 * \param [in]  in   L2_FDB_VLAN.DEST_TYPE.
 * \param [out] out  L2X.T/TGID/PORT_NUM/L2MC_PTR fields.
 * \param [in]  arg  Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_l2_fdb_vlan_dest_type_transform(int unit,
                                       const bcmltd_fields_t *in_key,
                                       const bcmltd_fields_t *in,
                                       bcmltd_fields_t *out,
                                       const bcmltd_transform_arg_t *arg)
{
    uint64_t lt_mac_addr = 0;
    uint64_t lt_dest_type = 0;
    uint64_t pt_dest_type = 0;
    bool is_mc = FALSE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_l2_fdb_vlan_dest_type_transform\n")));

    /* Sanity check for component argument */
    if (arg == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Sanity check for in key fields count */
    if ((in_key->count < LT_KEY_FLD_COUNT) || (in_key->count != arg->kfields)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Sanity check for in fields count */
    if ((in->count < LT_FLD_COUNT) || (in->count != arg->fields)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Sanity check for out fields count */
    if (out->count < arg->rfields) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    lt_mac_addr = in_key->field[LT_KEY_FLD_IDX_MAC_ADDR]->data;
    lt_dest_type = in->field[LT_FLD_IDX_DEST_TYPE]->data;

    if ((lt_mac_addr >> MC_MAC_BIT_POS) & 1) {
        is_mc = TRUE;
    }

    if (is_mc == TRUE) {
        if (lt_dest_type != LT_L2_MC_GRP) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        if (lt_dest_type == LT_L2_MC_GRP) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    out->count = 0;

    if (lt_dest_type == LT_PORT) {
        pt_dest_type = 0;
    } else if (lt_dest_type == LT_TRUNK) {
        pt_dest_type = 1;
    } else {
        pt_dest_type = 0;
    }

    out->field[out->count]->id   = arg->rfield[PT_FLD_IDX_DEST_TYPE];
    out->field[out->count]->idx  = 0;
    out->field[out->count]->data = pt_dest_type;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief L2_FDB_VLAN destination reverse transform.
 *
 * \param [in]  unit Unit number.
 * \param [in]  in   L2X.MAC_ADDR.
 * \param [in]  in   L2X.DEST_TYPE.
 * \param [out] out  L2_FDB_VLAN.DEST_TYPE.
 * \param [in]  arg  Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_l2_fdb_vlan_dest_type_rev_transform(int unit,
                                           const bcmltd_fields_t *in_key,
                                           const bcmltd_fields_t *in,
                                           bcmltd_fields_t *out,
                                           const bcmltd_transform_arg_t *arg)
{
    uint64_t lt_mac_addr = 0;
    uint64_t lt_dest_type = 0;
    uint64_t pt_dest_type = 0;
    bool is_mc = FALSE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                        "\t bcmltx_l2_fdb_vlan_dest_type_rev_transform\n")));

    /* Sanity check for component argument */
    if (arg == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Sanity check for in key fields count */
    if ((in_key->count < LT_KEY_FLD_COUNT) || (in_key->count != arg->kfields)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Sanity check for in fields count */
    if ((in->count < PT_FLD_COUNT) || (in->count != arg->fields)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Sanity check for out fields count */
    if (out->count < arg->rfields) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    lt_mac_addr = in_key->field[LT_KEY_FLD_IDX_MAC_ADDR]->data;
    pt_dest_type = in->field[PT_FLD_IDX_DEST_TYPE]->data;

    if ((lt_mac_addr >> MC_MAC_BIT_POS) & 1) {
        is_mc = TRUE;
    }

    out->count = 0;

    if (pt_dest_type == 1) {
        lt_dest_type = LT_TRUNK;
    } else if (pt_dest_type == 0) {
        if (is_mc == TRUE) {
            lt_dest_type = LT_L2_MC_GRP;
        } else {
            lt_dest_type = LT_PORT;
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->field[out->count]->id   = arg->rfield[LT_FLD_IDX_DEST_TYPE];
    out->field[out->count]->idx  = 0;
    out->field[out->count]->data = lt_dest_type;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}
