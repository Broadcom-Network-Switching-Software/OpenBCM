/*! \file bcmltx_port_vlan_xlate_ipri_mapped.c
 *
 * PORT_VLAN_XLATE.IPRI_MAPPED Transform Handler
 *
 * This file contains field transform information for PORT_VLAN_XLATE.IPRI_MAPPED.
 *
 * Element 'N' of PORT_VLAN_XLATE.IPRI_MAPPED array corresponds to the field
 * arg->rfield[0](forward transform).
 * PORT_VLAN_XLATE.IPRI_MAPPED index should be
 * between 0 and arg->rfields - 1 (forward transform).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmport/bcmltx_port_vlan_xlate_ipri_mapped.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_PORT

#define MAX_PRIORITY 7
#define MAX_PRI_CNT  8

/*
 * \brief PORT_VLAN_XLATE.IPRI_MAPPED array forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_VLAN_XLATE.IPRI_MAPPED field array.
 * \param [out] out             PORT_TAB.PRI_MAPPING fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_port_vlan_xlate_ipri_mapped_transform(int unit,
                                             const bcmltd_fields_t *in,
                                             bcmltd_fields_t *out,
                                             const bcmltd_transform_arg_t *arg)
{
    size_t i; /* logical in field index */
    uint32_t pri_idx;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_port_vlan_xlate_ipri_mapped_transform\n")));
    if (in->count <= 0) {
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }

    if (in->count > MAX_PRI_CNT) {
        rv = SHR_E_PARAM;
        SHR_ERR_EXIT(rv);
    }

    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = 0;
    for (i = 0; i < in->count; i++) {
        if (in->field[i]->data > MAX_PRIORITY) {
            rv = SHR_E_PARAM;
            SHR_ERR_EXIT(rv);
        }
        pri_idx = in->field[i]->idx;
        out->field[out->count]->data |= (in->field[i]->data) << (pri_idx * 3);
    }
    out->count++;

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief PORT_VLAN_XLATE.IPRI_MAPPED array reverse transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_TAB.PRI_MAPPING fields.
 * \param [out] out             PORT_VLAN_XLATE.IPRI_MAPPED field array.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_port_vlan_xlate_ipri_mapped_rev_transform(int unit,
                                                 const bcmltd_fields_t *in,
                                                 bcmltd_fields_t *out,
                                                 const bcmltd_transform_arg_t *arg)
{
    size_t idx;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_port_vlan_xlate_ipri_mapped_rev_transform\n")));

    if (in->count <= 0) {
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }

    for (idx = out->count = 0; idx < MAX_PRI_CNT; idx++) {
        out->field[out->count]->id = arg->rfield[0];
        out->field[out->count]->idx = idx;
        out->field[out->count]->data = (in->field[0]->data >> (idx * 3)) & 0x7;
        out->count++;
    }

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}
