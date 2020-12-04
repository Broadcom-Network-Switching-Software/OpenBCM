/*! \file bcmcth_tnl_l3_intf.c
 *
 * This file contains L3 egress tunnel interface
 * software state related intialization and
 * cleanup routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bsl/bsl.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmcth/bcmcth_tnl_internal.h>

#include <bcmcth/bcmcth_tnl_l3_intf_table_commit.h>
#include <bcmcth/bcmcth_tnl_l3_intf.h>
#include <bcmissu/issu_api.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_TNL

/*******************************************************************************
 * Local definitions
 */
static bcmcth_tnl_l3_intf_state_t  tnl_l3_egr_intf_state[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Get tunnel interface state pointer.
 *
 * Get tunnel interface state pointer for this unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [out] warm          Returned state pointer.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_tnl_l3_intf_state_get(int unit, bcmcth_tnl_l3_intf_state_t **state)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(state, SHR_E_PARAM);

    *state = &tnl_l3_egr_intf_state[unit];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3 sw resources init.
 *
 * Initialize L3 SW data structures for this unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  warm          Cold/Warm boot.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_tnl_l3_intf_sw_state_init(int unit,
                                 bool warm)
{
    uint32_t    tbl_sz = 0;
    uint32_t    ha_alloc_sz = 0, ha_req_sz = 0;
    bcmlrd_field_def_t  field_def;
    bcmcth_tnl_l3_intf_info_t * l3_intf_info = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&field_def, 0, sizeof(field_def));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_def_get(unit,
                                    L3_EIFt,
                                    L3_EIFt_L3_EIF_IDf,
                                    &field_def));

    tbl_sz = (BCMCTH_TNL_FIELD_MAX(field_def) + 1);
    if (tbl_sz == 0) {
        SHR_EXIT();
    }

    /* Allocate backup HA memory for L3 tnl interface sw state. */
    ha_req_sz = sizeof(bcmcth_tnl_l3_intf_info_t) +
                (tbl_sz * sizeof(bcmcth_tnl_l3_intf_t));
    ha_alloc_sz = ha_req_sz;
    l3_intf_info =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_TNL_COMP_ID,
                         BCMCTH_TNL_L3_INTF_SUB_COMP_ID,
                         "bcmcthTnlL3IntfInfo",
                         &ha_alloc_sz);

    SHR_NULL_CHECK(l3_intf_info, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_sz < ha_req_sz) ?
                            SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(l3_intf_info, 0, ha_alloc_sz);
        l3_intf_info->array_size = tbl_sz;
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_TNL_COMP_ID,
                                        BCMCTH_TNL_L3_INTF_SUB_COMP_ID, 0,
                                        ha_req_sz, 1,
                                        BCMCTH_TNL_L3_INTF_INFO_T_ID));
    }
    tnl_l3_egr_intf_state[unit].intf_ha = l3_intf_info->l3_intf_array;
    ha_alloc_sz = ha_req_sz - sizeof(bcmcth_tnl_l3_intf_info_t);

    /* Allocate memory for L3 tnl interface sw state. */
    SHR_ALLOC(tnl_l3_egr_intf_state[unit].intf,
              ha_alloc_sz,
              "bcmcthtnlL3EgrIntf");
    SHR_NULL_CHECK(tnl_l3_egr_intf_state[unit].intf, SHR_E_MEMORY);
    sal_memset(tnl_l3_egr_intf_state[unit].intf, 0, ha_alloc_sz);


    tnl_l3_egr_intf_state[unit].tbl_sz = ha_alloc_sz;

    /* Copy backup sw state to active sw state. */
    sal_memcpy(tnl_l3_egr_intf_state[unit].intf,
               tnl_l3_egr_intf_state[unit].intf_ha,
               ha_alloc_sz);

exit:
    if (SHR_FUNC_ERR()) {
        if (tnl_l3_egr_intf_state[unit].intf != NULL) {
            SHR_FREE(tnl_l3_egr_intf_state[unit].intf);
        }
        if (l3_intf_info != NULL) {
            bcmissu_struct_info_clear(unit,
                                      BCMCTH_TNL_L3_INTF_INFO_T_ID,
                                      BCMMGMT_TNL_COMP_ID,
                                      BCMCTH_TNL_L3_INTF_SUB_COMP_ID);
            shr_ha_mem_free(unit, l3_intf_info);
        }

        tnl_l3_egr_intf_state[unit].tbl_sz = 0;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief L3 sw resources cleanup.
 *
 * Cleanup L3 SW data structures allocated for this unit.
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_tnl_l3_intf_sw_state_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (tnl_l3_egr_intf_state[unit].intf != NULL) {
        SHR_FREE(tnl_l3_egr_intf_state[unit].intf);
    }

    tnl_l3_egr_intf_state[unit].tbl_sz = 0;

    SHR_FUNC_EXIT();
}

/*!
 * \brief L3 tunnel encap id prepare
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_tnl_l3_intf_table_prepare(int unit,
                                 uint32_t trans_id,
                                 const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief L3 tunnel encap id commit
 *
 * Discard the backup state and copy current state
 * to backup state as the current
 * transaction is getting committed.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_tnl_l3_intf_table_commit(int unit,
                                uint32_t trans_id,
                                const bcmltd_generic_arg_t *arg)
{
    uint32_t                alloc_sz = 0;
    bcmcth_tnl_l3_intf_t        *bkp_ptr = NULL;
    bcmcth_tnl_l3_intf_t        *active_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    alloc_sz = tnl_l3_egr_intf_state[unit].tbl_sz;
    bkp_ptr = tnl_l3_egr_intf_state[unit].intf_ha;
    active_ptr = tnl_l3_egr_intf_state[unit].intf;
    if ((bkp_ptr == NULL) || (active_ptr == NULL)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Copy active sw state to backup sw state. */
    sal_memcpy(bkp_ptr, active_ptr, alloc_sz);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3 tunnel encap id abort
 *
 * Rollback the current state to backup state
 * as the current transaction is getting aborted.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_tnl_l3_intf_table_abort(int unit,
                               uint32_t trans_id,
                               const bcmltd_generic_arg_t *arg)
{
    uint32_t                alloc_sz = 0;
    bcmcth_tnl_l3_intf_t        *bkp_ptr = NULL;
    bcmcth_tnl_l3_intf_t        *active_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    alloc_sz = tnl_l3_egr_intf_state[unit].tbl_sz;
    bkp_ptr = tnl_l3_egr_intf_state[unit].intf_ha;
    active_ptr = tnl_l3_egr_intf_state[unit].intf;
    if ((bkp_ptr == NULL) || (active_ptr == NULL)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Copy backup sw state to active sw state. */
    sal_memcpy(active_ptr, bkp_ptr, alloc_sz);

exit:
    SHR_FUNC_EXIT();
}
