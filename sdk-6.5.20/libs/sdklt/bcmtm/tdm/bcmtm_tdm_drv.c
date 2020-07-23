/*! \file bcmtm_tdm_drv.c
 *
 * TDM driver functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _TDM_STANDALONE
    #include <bcmtm_tdm_top.h>
    #include <tdm_dv_shim.h>
#else
    #include <bcmtm/tdm/bcmtm_tdm_top.h>
#endif


/********************************************************************
 * Internal functions.
 */
/*!
 * \brief Initialize TDM resource and driver functions.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS TDM initialization sequence completed successfully.
 * \retval TDM_FAIL TDM initialization sequence failed.
 */
int
bcmtm_tdm_drv_init(tdm_mod_t **tdm)
{
    int drv_idx;
    tdm_mod_t *tdmp = NULL;

    if (tdm == NULL ) {
        return TDM_FAIL;
    }

    tdmp = TDM_ALLOC(sizeof(tdm_mod_t), "bcmtmTdmConstructor");
    if (!tdmp) {
        TDM_ERROR0("Failed to allocate tdm resource.\n");
        return TDM_FAIL;
    } else {
        TDM_MEMSET(tdmp, 0, sizeof(tdm_mod_t));
        for (drv_idx = 0; drv_idx < TDM_DRV_SIZE; drv_idx++) {
            tdmp->core_drv[drv_idx] = &bcmtm_tdm_core_null;
        }
        (*tdm) = tdmp;
    }

    return TDM_PASS;
}

/*!
 * \brief Config TDM user data.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] tdm Pointer of TDM config structure.
 *
 * \retval TDM_PASS TDM config completed successfully.
 * \retval TDM_FAIL TDM config failed.
 */
int
bcmtm_tdm_drv_cfg(tdm_mod_t *tdm, tdm_cfg_t *cfg)
{
    if (tdm != NULL && cfg != NULL) {
        TDM_MEMCPY(&(tdm->user_data.cfg), cfg, sizeof(tdm_cfg_t));
        return TDM_PASS;
    }

    return TDM_FAIL;
}

/*!
 * \brief Free TDM resource.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_drv_free(tdm_mod_t *tdm)
{
    if (tdm != NULL) {
        TDM_FREE(tdm);
    }
}

/*!
 * \brief Run TDM algorithm to generate TDM calendars.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS TDM calendar generation completed successfully.
 * \retval TDM_FAIL TDM calendar generation failed.
 */
int
bcmtm_tdm_drv_run(tdm_mod_t *tdm)
{
    return ((tdm != NULL) ? (tdm->core_drv[TDM_DRV_RUN](tdm)) : TDM_FAIL);
}
