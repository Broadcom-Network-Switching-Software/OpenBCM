/*! \file bcmltx_port_vlan_xlate_ing_miss_action.c
 *
 * PORT_VLAN_XLATE.ING_MISS_ACTION Transform Handler
 *
 * This file contains field transform information for PORT_VLAN_XLATE.ING_MISS_ACTION.
 *
 * Element 'N' of PORT_VLAN_XLATE.ING_MISS_ACTION array corresponds to the field
 * arg->rfield[N](forward transform), arg->field[N](reverse transform).
 * PORT_VLAN_XLATE.ING_MISS_ACTION index should be
 * between 0 and arg->rfields - 1 (forward transform)
 * between 0 and arg->fields - 1 (reverse transform).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmport/bcmltx_port_vlan_xlate_ing_miss_action.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_PORT

#define LT_FLD_IDX_ING_MISS_ACTION    0

#define PT_FLD_IDX_MISS_DROP_KEY_1    0
#define PT_FLD_IDX_MISS_DROP_KEY_2    1
#define PT_FLD_IDX_MISS_DROP          2

typedef enum lt_ing_miss_action_e {
    NO_DROP = 0,
    FIRST_LOOKUP_MISS_DROP = 1,
    SECOND_LOOKUP_MISS_DROP = 2,
    BOTH_LOOKUPS_MISS_DROP = 3,
    ANY_LOOKUP_MISS_DROP = 4
} lt_ing_miss_action_t;

/*
 * \brief PORT_VLAN_XLATE.ING_MISS_ACTION array forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_VLAN_XLATE.ING_MISS_ACTION field array.
 * \param [out] out             PORT_TAB.VT_MISS_DROP_KEY_1
 *                                       VT_MISS_DROP_KEY_2
 *                                       VT_MISS_DROP fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_port_vlan_xlate_ing_miss_action_transform(int unit,
                                                 const bcmltd_fields_t *in,
                                                 bcmltd_fields_t *out,
                                                 const bcmltd_transform_arg_t *arg)
{
    int rv = SHR_E_NONE;
    lt_ing_miss_action_t   ing_miss_action = 0;
    uint64_t  vt_miss_drop_key_1_val = 0 , vt_miss_drop_key_2_val = 0;
    uint64_t  vt_miss_drop_val = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_port_vlan_xlate_ing_miss_action_transform\n")));
    if (in->count == 0) {
        goto exit;
    }

    ing_miss_action = in->field[0]->data;
    switch (ing_miss_action) {
    case NO_DROP:
        vt_miss_drop_val = 0;
        vt_miss_drop_key_1_val = 0;
        vt_miss_drop_key_2_val = 0;
        break;
    case FIRST_LOOKUP_MISS_DROP:
        vt_miss_drop_val = 0;
        vt_miss_drop_key_1_val = 1;
        vt_miss_drop_key_2_val = 0;
        break;
    case SECOND_LOOKUP_MISS_DROP:
        vt_miss_drop_val = 0;
        vt_miss_drop_key_1_val = 0;
        vt_miss_drop_key_2_val = 1;
        break;
    case BOTH_LOOKUPS_MISS_DROP:
        vt_miss_drop_val = 1;
        vt_miss_drop_key_1_val = 0;
        vt_miss_drop_key_2_val = 0;
        break;
    case ANY_LOOKUP_MISS_DROP:
        vt_miss_drop_val = 0;
        vt_miss_drop_key_1_val = 1;
        vt_miss_drop_key_2_val = 1;
        break;

    }

    out->count = 0;
    out->field[out->count]->id = arg->rfield[PT_FLD_IDX_MISS_DROP];
    out->field[out->count]->data = vt_miss_drop_val;
    out->count++;
    out->field[out->count]->id = arg->rfield[PT_FLD_IDX_MISS_DROP_KEY_1];
    out->field[out->count]->data = vt_miss_drop_key_1_val;
    out->count++;
    out->field[out->count]->id = arg->rfield[PT_FLD_IDX_MISS_DROP_KEY_2];
    out->field[out->count]->data = vt_miss_drop_key_2_val;
    out->count++;

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief PORT_VLAN_XLATE.ING_MISS_ACTION array reverse transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_TAB.VT_MISS_DROP_KEY_1
 *                                       VT_MISS_DROP_KEY_2
 *                                       VT_MISS_DROP fields.
 * \param [out] out             PORT_VLAN_XLATE.ING_MISS_ACTION field array.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_port_vlan_xlate_ing_miss_action_rev_transform(int unit,
                                                     const bcmltd_fields_t *in,
                                                     bcmltd_fields_t *out,
                                                     const bcmltd_transform_arg_t *arg)
{
    size_t idx;
    int rv = SHR_E_NONE;
    uint64_t  vt_miss_drop_key_1_val = 0 , vt_miss_drop_key_2_val = 0;
    uint64_t  vt_miss_drop_val = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_port_vlan_xlate_ing_miss_action_rev_transform\n")));
    if (in->count == 0) {
        goto exit;
    }

    for (idx = 0; idx < in->count; idx++) {
        if (in->field[idx]->id == arg->field[PT_FLD_IDX_MISS_DROP]) {
            vt_miss_drop_val = in->field[idx]->data;
        } else if (in->field[idx]->id == arg->field[PT_FLD_IDX_MISS_DROP_KEY_1]) {
            vt_miss_drop_key_1_val = in->field[idx]->data;
        }  else if (in->field[idx]->id == arg->field[PT_FLD_IDX_MISS_DROP_KEY_2]) {
            vt_miss_drop_key_2_val = in->field[idx]->data;
        } else {
            rv = SHR_E_FAIL;
            break;
        }
    }

    out->count = 0;
    out->field[out->count]->id = arg->rfield[LT_FLD_IDX_ING_MISS_ACTION];
    out->field[out->count]->idx = 0;
    if (vt_miss_drop_val       == 0 &&
        vt_miss_drop_key_1_val == 0 &&
        vt_miss_drop_key_2_val == 0) {
        out->field[out->count]->data = NO_DROP;
    } else if (vt_miss_drop_val       == 0 &&
               vt_miss_drop_key_1_val == 1 &&
               vt_miss_drop_key_2_val == 0) {
        out->field[out->count]->data = FIRST_LOOKUP_MISS_DROP;
    } else if (vt_miss_drop_val       == 0 &&
               vt_miss_drop_key_1_val == 0 &&
               vt_miss_drop_key_2_val == 1) {
        out->field[out->count]->data = SECOND_LOOKUP_MISS_DROP;
    } else if (vt_miss_drop_val       == 1 &&
               vt_miss_drop_key_1_val == 0 &&
               vt_miss_drop_key_2_val == 0) {
        out->field[out->count]->data = BOTH_LOOKUPS_MISS_DROP;
    } else if (vt_miss_drop_val       == 0 &&
               vt_miss_drop_key_1_val == 1 &&
               vt_miss_drop_key_2_val == 1) {
        out->field[out->count]->data = ANY_LOOKUP_MISS_DROP;
    } else {
        rv = SHR_E_INTERNAL;
    }
    out->count++;
    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}
