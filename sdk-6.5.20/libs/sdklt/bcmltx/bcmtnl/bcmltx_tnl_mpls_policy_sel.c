/*! \file bcmltx_port_vlan_xlate_icfi_mapped.c
 *
 * PORT_VLAN_XLATE.ICFI_MAPPED Transform Handler
 *
 * This file contains field transform information for PORT_VLAN_XLATE.ICFI_MAPPED.
 *
 * Element 'N' of PORT_VLAN_XLATE.ICFI_MAPPED array corresponds to the field
 * arg->rfield[N](forward transform), arg->field[N](reverse transform).
 * PORT_VLAN_XLATE.ICFI_MAPPED index should be
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
#include <bcmltx/bcmtnl/bcmltx_tnl_mpls_policy_sel.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_TNL

/*
 * \brief TNL_DEFAULT_POLICY.POLICY_CMD_SELECT/POLICY_OBJ_SELECT array forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_VLAN_XLATE.ICFI_MAPPED field array.
 * \param [out] out             PORT_TAB.ICFI_0_MAPPING/ICFI_1_MAPPING fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_tnl_mpls_policy_sel_transform(int unit,
									 const bcmltd_fields_t *in,
									 bcmltd_fields_t *out,
									 const bcmltd_transform_arg_t *arg)
{
    size_t i; /* logical in field index */
    uint32_t cfi_idx;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_tnl_mpls_policy_sel_transform\n")));
    out->count = 0;
    for (i = 0; i < in->count; i++) {
        cfi_idx = in->field[i]->idx;
        out->field[out->count]->id = arg->rfield[cfi_idx];
        out->field[out->count]->data = in->field[i]->data;
        out->count++;
    }

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TNL_DEFAULT_POLICY.POLICY_CMD_SELECT/POLICY_OBJ_SELECT array reverse transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_TAB/ICFI_0_MAPPING/ICFI_1_MAPPING fields.
 * \param [out] out             PORT_VLAN_XLATE.ICFI_MAPPED field array.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_tnl_mpls_policy_sel_rev_transform(int unit,
										 const bcmltd_fields_t *in,
										 bcmltd_fields_t *out,
										 const bcmltd_transform_arg_t *arg)
{
    size_t idx;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_tnl_mpls_policy_sel_rev_transform\n")));

    for (idx = out->count = 0; idx < in->count; idx++) {
        if (idx < arg->fields) {
            out->field[out->count]->id = arg->rfield[0];
            out->field[out->count]->idx = idx;
            out->field[out->count]->data = in->field[idx]->data;
            out->count++;
        } else {
            rv = SHR_E_FAIL;
            break;
        }
    }

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}
