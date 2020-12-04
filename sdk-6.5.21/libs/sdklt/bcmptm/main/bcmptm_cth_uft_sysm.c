/*! \file bcmptm_cth_uft_sysm.c
 *
 * Interface functions to the system manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include "bcmptm/bcmptm_internal.h"
#include "bcmptm/bcmptm_chip_internal.h"
#include "bcmptm/bcmptm_itx.h"
#include "bcmptm/bcmptm_uft_internal.h"
#include "bcmptm/bcmptm_cth_uft_sysm.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
static bcmmgmt_sysm_comp_id_t init_comp_list[] = {
    BCMMGMT_BD_COMP_ID,
    BCMMGMT_CFG_COMP_ID,
    BCMMGMT_DRD_COMP_ID,
    BCMMGMT_LTD_COMP_ID,
    BCMMGMT_IMM_FE_COMP_ID,
    BCMMGMT_EVM_COMP_ID,
    BCMMGMT_PTM_COMP_ID,
    BCMMGMT_DI_COMP_ID,
    BCMMGMT_UFT_BE_COMP_ID,
    BCMMGMT_MAX_COMP_ID,
}; /* end of list indicator */
static bcmmgmt_sysm_comp_id_t cleanup_comp_list[] = {
    BCMMGMT_TRM_COMP_ID,
    BCMMGMT_LTM_COMP_ID,
    BCMMGMT_MAX_COMP_ID,
}; /* end of list indicator */


/*******************************************************************************
 * Private Functions
 */
static bool
is_blocked(shr_sysm_categories_t instance_category, int unit,
           bcmmgmt_sysm_comp_id_t *comp_list,
           uint32_t *blocking_component)
{
    int i;
    for (i = 0; ; i++) {
        if (comp_list[i] == BCMMGMT_MAX_COMP_ID) {
            break;
        }
        if (!shr_sysm_is_comp_complete(comp_list[i], instance_category, unit)) {
            *blocking_component = comp_list[i];
            return TRUE;
        }
    }
    return FALSE;
}

static shr_sysm_rv_t
uft_my_init(int unit, bool warm)
{
    int tmp_rv = 0;

    tmp_rv = bcmptm_uft_init(unit, warm);
    if (SHR_FAILURE(tmp_rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "bcmptm_uft_init FAILED(rv = %0d)\n"),
             tmp_rv));
        return SHR_SYSM_RV_ERROR;
    }

    /* Success */
    return SHR_SYSM_RV_DONE;
}

/* Phase: comp_config
 */
static shr_sysm_rv_t
unit_cth_uft_comp_config(shr_sysm_categories_t instance_category,
                         int unit,
                         void *comp_data,
                         bool warm,
                         uint32_t *blocking_component)
{
    if (is_blocked(instance_category, unit, init_comp_list,
                   blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }
    if (SHR_FAILURE(bcmptm_uft_em_tile_event_hdl_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return uft_my_init(unit, warm);
}

/* Phase: stop
 */
static shr_sysm_rv_t
unit_cth_uft_stop(shr_sysm_categories_t instance_category,
                  int unit,
                  void *comp_data,
                  bool graceful,
                  uint32_t *blocking_component)
{
    if (is_blocked(instance_category, unit, cleanup_comp_list,
                   blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }
    if (SHR_FAILURE(bcmptm_uft_em_tile_event_hdl_stop(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmptm_itx_stop(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/* Phase: shutdown */
static shr_sysm_rv_t
unit_cth_uft_shutdown(shr_sysm_categories_t instance_category,
                      int unit,
                      void *comp_data,
                      bool graceful,
                      uint32_t *blocking_component)
{
    if (is_blocked(instance_category, unit, cleanup_comp_list,
                   blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmptm_uft_cleanup(unit, graceful))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief Systerm init for CTH-UFT.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
static shr_sysm_rv_t
sysm_cth_uft_init(shr_sysm_categories_t instance_category,
                  int unit,
                  void *comp_data,
                  bool warm,
                  uint32_t *blocking_component)
{
    if (bcmptm_uft_fld_hdl_init() != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief Systerm shutdow for CTH-UFT.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
static shr_sysm_rv_t
sysm_cth_uft_shutdown(shr_sysm_categories_t instance_category,
                      int unit,
                      void *comp_data,
                      bool graceful,
                      uint32_t *blocking_component)
{
    bcmptm_uft_fld_hdl_cleanup();

    return SHR_SYSM_RV_DONE;
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_cth_uft_start(void)
{
    shr_sysm_cb_func_set_t funcs;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Register unit-independent functions. */
    sal_memset(&funcs, 0, sizeof(funcs));
    funcs.init = sysm_cth_uft_init;
    funcs.shutdown = sysm_cth_uft_shutdown;
    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_UFT_COMP_ID,
                           SHR_SYSM_CAT_GENERIC,
                           &funcs,
                           NULL));

    /* Register unit-specific functions. */
    sal_memset(&funcs, 0, sizeof(funcs));
    funcs.comp_config = unit_cth_uft_comp_config;
    funcs.stop = unit_cth_uft_stop;
    funcs.shutdown = unit_cth_uft_shutdown;
    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_UFT_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &funcs,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
