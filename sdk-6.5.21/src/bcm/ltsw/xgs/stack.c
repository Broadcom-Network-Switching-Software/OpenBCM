/*! \file stack.c
 *
 * STACK Driver for XGS family chips.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/xgs/stack.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_STK

/******************************************************************************
 * Private functions
 */

 /*!
  * \brief Set my modual id on port table.
  *
  * \param [in] unit Unit Number.
  * \param [in] my_modid My Module ID Number.
  *
  * \retval SHR_E_NONE No errors.
  * \retval !SHR_E_NONE Failure.
  */
static int
ltsw_xgs_stack_set(int unit, int my_modid)
{
    bcmlt_entry_handle_t lte_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_info_t lte_info;
    uint64_t value;
    int dunit;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, PORTs, &lte_hdl));

    value = my_modid;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(lte_hdl, MY_MODIDs, value));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(lte_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_info_get(lte_hdl, &lte_info));
    if (SHR_FAILURE(lte_info.status)) {
        SHR_ERR_EXIT(lte_info.status);
    }

exit:
    if (lte_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(lte_hdl);
        lte_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

 /******************************************************************************
  * Public functions
 */

int
xgs_ltsw_stack_set(int unit, int my_modid)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (ltsw_xgs_stack_set(unit, my_modid));
exit:
    SHR_FUNC_EXIT();
}

