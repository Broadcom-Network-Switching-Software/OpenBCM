/*! \file bcmcth_port_system.c
 *
 * This file contains Port system
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
#include <bcmcth/bcmcth_port_util.h>

#include <bcmcth/bcmcth_port_system_table_commit.h>
#include <bcmcth/bcmcth_port_system.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_PORT

/*******************************************************************************
 * Local definitions
 */
static bcmcth_port_system_state_t  port_system_state[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Get port system state pointer.
 *
 * Get port system state pointer for the unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [out] state         Returned state pointer.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_port_system_state_get(int unit, bcmcth_port_system_state_t **state)
{
    (*state) = &(port_system_state[unit]);
    return SHR_E_NONE;
}

/*!
 * \brief Port system sw resources init.
 *
 * Initialize Port system SW data structures for this unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  warm          Cold/Warm boot.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_port_system_sw_state_init(int unit,
                                 bool warm)
{
    uint32_t    tbl_sz = 0;
    uint32_t    ha_alloc_sz = 0, ha_req_sz = 0;
    uint32_t    size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_size_get(unit, PORT_SYSTEMt, &size));

    tbl_sz = size;
    if (tbl_sz == 0) {
        SHR_EXIT();
    }

    /* Allocate backup HA memory for port system sw state. */
    ha_req_sz = (tbl_sz * sizeof(bcmcth_port_system_t));
    ha_alloc_sz = ha_req_sz;
    port_system_state[unit].info_ha =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_PORT_COMP_ID,
                         BCMPORT_SYSTEM_SUB_COMP_ID,
                         "bcmcthPortSystemState",
                         &ha_alloc_sz);

    SHR_NULL_CHECK(port_system_state[unit].info_ha, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_sz < ha_req_sz) ?
                            SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(port_system_state[unit].info_ha, 0, ha_alloc_sz);
    }

    /* Allocate memory for port system sw state. */
    SHR_ALLOC(port_system_state[unit].info,
              ha_alloc_sz,
              "bcmcthtnlL3EgrIntf");
    SHR_NULL_CHECK(port_system_state[unit].info, SHR_E_MEMORY);
    sal_memset(port_system_state[unit].info, 0, ha_alloc_sz);


    port_system_state[unit].tbl_sz = ha_alloc_sz;

    /* Copy backup sw state to active sw state. */
    sal_memcpy(port_system_state[unit].info,
               port_system_state[unit].info_ha,
               ha_alloc_sz);

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(port_system_state[unit].info);
        port_system_state[unit].info = NULL;

        if (port_system_state[unit].info_ha != NULL) {
            shr_ha_mem_free(unit, port_system_state[unit].info_ha);
            port_system_state[unit].info_ha = NULL;
        }

        port_system_state[unit].tbl_sz = 0;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Port system sw resources cleanup.
 *
 * Cleanup Port system SW data structures allocated for this unit.
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_port_system_sw_state_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_FREE(port_system_state[unit].info);
    port_system_state[unit].info = NULL;

    port_system_state[unit].tbl_sz = 0;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Port system id prepare
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_port_system_table_prepare(int unit,
                                 uint32_t trans_id,
                                 const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Port system id commit
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
bcmcth_port_system_table_commit(int unit,
                                uint32_t trans_id,
                                const bcmltd_generic_arg_t *arg)
{
    uint32_t                alloc_sz = 0;
    bcmcth_port_system_t        *bkp_ptr = NULL;
    bcmcth_port_system_t        *active_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    alloc_sz = port_system_state[unit].tbl_sz;
    bkp_ptr = port_system_state[unit].info_ha;
    active_ptr = port_system_state[unit].info;
    if ((bkp_ptr == NULL) || (active_ptr == NULL)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Copy active sw state to backup sw state. */
    sal_memcpy(bkp_ptr, active_ptr, alloc_sz);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Port system id abort
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
bcmcth_port_system_table_abort(int unit,
                               uint32_t trans_id,
                               const bcmltd_generic_arg_t *arg)
{
    uint32_t                alloc_sz = 0;
    bcmcth_port_system_t        *bkp_ptr = NULL;
    bcmcth_port_system_t        *active_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    alloc_sz = port_system_state[unit].tbl_sz;
    bkp_ptr = port_system_state[unit].info_ha;
    active_ptr = port_system_state[unit].info;
    if ((bkp_ptr == NULL) || (active_ptr == NULL)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Copy backup sw state to active sw state. */
    sal_memcpy(active_ptr, bkp_ptr, alloc_sz);

exit:
    SHR_FUNC_EXIT();
}
