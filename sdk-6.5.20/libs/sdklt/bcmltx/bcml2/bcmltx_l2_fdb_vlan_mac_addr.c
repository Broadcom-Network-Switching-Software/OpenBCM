/*! \file bcmltx_l2_fdb_vlan_mac_addr.c
 *
 * L2_FDB_VLAN.MAC_ADDR Transform Handler
 *
 * This file contains field transform information for
 * L2_FDB_VLAN.MAC_ADDR.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcml2/bcmltx_l2_fdb_vlan_mac_addr.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_L2

#define LT_FLD_IDX_MAC_ADDR     0

#define PT_FLD_IDX_MAC_ADDR     0

#define MC_MAC_BIT_POS        40

/******************************************************************************
* Public functions
 */
/*
 * \brief L2_FDB_VLAN.MAC_ADDR forward transform.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  in            L2_FDB_VLAN.MAC_ADDR.
 * \param [out] out           L2X.L2::MAC_ADDR,
 * \param [in]  arg           Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_l2_fdb_vlan_mac_addr_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg)
{

    SHR_FUNC_ENTER(unit);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "\t bcmltx_l2_fdb_vlan_mac_addr_transform\n")));
    if (in == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (in->count == 0) {
        /* First call in LT traverse case. */
        out->count = 0;
        out->field[out->count]->id	 = arg->rfield[PT_FLD_IDX_MAC_ADDR];
        out->field[out->count]->data = 0;
        out->count++;
        SHR_EXIT();
    }

    out->count = 0;
    out->field[out->count]->id   = arg->rfield[PT_FLD_IDX_MAC_ADDR];
    out->field[out->count]->data = in->field[0]->data;
    out->count++;

    if ((in->field[0]->data >> MC_MAC_BIT_POS) & 1) {
        ((bcmltx_l2_mac_type_t *)arg->comp_data->user_data)->is_mc = TRUE;
    } else {
        ((bcmltx_l2_mac_type_t *)arg->comp_data->user_data)->is_mc = FALSE;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief L2_FDB_VLAN.MAC_ADDR reverse transform.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  in            L2_FDB_VLAN.MAC_ADDR.
 * \param [out] out           L2X.L2::MAC_ADDR,
 * \param [in]  arg           Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_l2_fdb_vlan_mac_addr_rev_transform(int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "\t bcmltx_l2_fdb_vlan_mac_addr_rev_transform\n")));

    if (in == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (in->count == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->count = 0;
    out->field[out->count]->id   = arg->rfield[LT_FLD_IDX_MAC_ADDR];
    out->field[out->count]->data = in->field[0]->data;
    out->count++;

    if ((in->field[0]->data >> MC_MAC_BIT_POS) & 1) {
        ((bcmltx_l2_mac_type_t *)arg->comp_data->user_data)->is_mc = TRUE;
    } else {
        ((bcmltx_l2_mac_type_t *)arg->comp_data->user_data)->is_mc = FALSE;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}
