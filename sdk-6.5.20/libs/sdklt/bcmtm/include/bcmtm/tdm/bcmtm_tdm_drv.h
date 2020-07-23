/*! \file bcmtm_tdm_drv.h
 *
 * TDM algorithm drivers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_TDM_DRV_H
#define BCMTM_TDM_DRV_H


#ifdef _TDM_STANDALONE
    #include <bcmtm_tdm_defines.h>
#else
    #include <bcmtm/tdm/bcmtm_tdm_defines.h>
#endif


/***********************************************************************
 * Typedefs.
 */

/*!
 * \brief TDM driver function for resource initialization.
 */
typedef int (*bcmtm_tdm_drv_init_f)(tdm_mod_t **tdm);

/*!
 * \brief TDM driver function for user configuration.
 */
typedef int (*bcmtm_tdm_drv_cfg_f)(tdm_mod_t *tdm, tdm_cfg_t *cfg);

/*!
 * \brief TDM driver function for calendar generation.
 */
typedef int (*bcmtm_tdm_drv_run_f)(tdm_mod_t *tdm);

/*!
 * \brief TDM driver function for resource cleanup.
 */
typedef void (*bcmtm_tdm_drv_free_f)(tdm_mod_t *tdm);

/*!
 * \brief TDM driver function for initialization.
 *
 * This structure is used to define TDM core algorithm drivers.
 */
typedef struct bcmtm_tdm_drv_s {
    /*! TDM init driver. */
    bcmtm_tdm_drv_init_f tdm_init;

    /*! TDM config driver. */
    bcmtm_tdm_drv_cfg_f tdm_cfg;

    /*! TDM run driver. */
    bcmtm_tdm_drv_run_f tdm_run;

    /*! TDM free driver. */
    bcmtm_tdm_drv_free_f tdm_free;
} bcmtm_tdm_drv_t;


/***********************************************************************
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
extern int
bcmtm_tdm_drv_init(tdm_mod_t **tdm);

/*!
 * \brief Config TDM user data.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] tdm Pointer of TDM config structure.
 *
 * \retval TDM_PASS TDM config completed successfully.
 * \retval TDM_FAIL TDM config failed.
 */
extern int
bcmtm_tdm_drv_cfg(tdm_mod_t *tdm, tdm_cfg_t *cfg);

/*!
 * \brief Free TDM resource.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \return Nothing.
 */
extern void
bcmtm_tdm_drv_free(tdm_mod_t *tdm);

/*!
 * \brief Run TDM algorithm to generate TDM calendars.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS TDM calendar generation completed successfully.
 * \retval TDM_FAIL TDM calendar generation failed.
 */
extern int
bcmtm_tdm_drv_run(tdm_mod_t *tdm);

#endif /* BCMTM_TDM_DRV_H */
