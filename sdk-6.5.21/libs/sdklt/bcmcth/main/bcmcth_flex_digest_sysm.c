/*! \file bcmcth_flex_digest_sysm.c
 *
 *  Flex Digest component interfaces to the system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmcth/bcmcth_flex_digest_sysm.h>
#include <bcmcth/bcmcth_flex_digest_drv.h>
#include <bcmcth/bcmcth_flex_digest.h>
/*******************************************************************************
 * Defines
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_FLEXDIGEST

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */

/*******************************************************************************
 * Private Functions
 */
/* Phase: shutdown */
static shr_sysm_rv_t
cth_fd_sysm_shutdown(shr_sysm_categories_t inst_cat,
                     int unit,
                     void *comp_data,
                     bool graceful,
                     uint32_t *blocking_component)
{
    int idx;
    uint32_t comp_dep[] = {
        BCMMGMT_IMM_FE_COMP_ID,
        BCMMGMT_LTM_COMP_ID
    };

    for (idx = 0; idx < COUNTOF(comp_dep); idx++) {
        if (!shr_sysm_is_comp_complete(comp_dep[idx], inst_cat, unit)) {
            *blocking_component = comp_dep[idx];
            return SHR_SYSM_RV_BLOCKED;
        }
    }

    if (SHR_FAILURE(bcmcth_flex_digest_detach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/* Phase: comp_config */
static shr_sysm_rv_t
cth_fd_sysm_comp_config(shr_sysm_categories_t inst_cat,
                        int unit,
                        void *comp_data,
                        bool warm,
                        uint32_t *blocking_component)
{
    int idx;
    uint32_t comp_dep[] = {
        BCMMGMT_PTM_COMP_ID,
        BCMMGMT_DI_COMP_ID,
        BCMMGMT_SER_COMP_ID
    };

    for (idx = 0; idx < COUNTOF(comp_dep); idx++) {
        if (!shr_sysm_is_comp_complete(comp_dep[idx], inst_cat, unit)) {
            *blocking_component = comp_dep[idx];
            return SHR_SYSM_RV_BLOCKED;
        }
    }

    if (SHR_FAILURE(bcmcth_flex_digest_attach(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }


    return SHR_SYSM_RV_DONE;
}

/* Phase: stop */
static shr_sysm_rv_t
cth_fd_sysm_stop(shr_sysm_categories_t inst_cat,
                 int unit,
                 void *comp_data,
                 bool warm,
                 uint32_t *blocking_component)
{

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCTH Flex Digest System manager init struct.
 */
static shr_sysm_cb_func_set_t cth_fd_sysm_handler = {

    /*! Component configuration. */
    .comp_config = cth_fd_sysm_comp_config,

    /*! Shutdown. */
    .shutdown    = cth_fd_sysm_shutdown,

    /*! Stop. */
    .stop    = cth_fd_sysm_stop
};

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_flex_digest_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT(
        shr_sysm_register(BCMMGMT_FLEX_DIGEST_COMP_ID,
                          SHR_SYSM_CAT_UNIT,
                          &cth_fd_sysm_handler,
                          NULL));
exit:
    SHR_FUNC_EXIT();
}
