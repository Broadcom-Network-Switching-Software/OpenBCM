/*! \file rm_alpm_traverse.h
 *
 * Main routines for RM ALPM Traverse
 *
 * This file contains routines traverse ALPM resouces,
 * which are internal to ALPM resource manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_ALPM_TRAVERSE_H
#define RM_ALPM_TRAVERSE_H


/*******************************************************************************
   Includes
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>

#include "rm_alpm.h"


/*******************************************************************************
   Function prototypes
 */
/*!
 * \brief Create (or refresh) snapshot and get first entry in it
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 * \param [in] max Max levels
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_get_first(int u, int m, bcmltd_sid_t ltid, bool snapshot_en, alpm_arg_t *arg, int max_levels);


/*!
 * \brief Get next entry in the snapshot based on given key.
 *
 * Destroy the snapshot if not found.
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 * \param [in] max Max levels
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_rm_alpm_get_next(int u, int m, bcmltd_sid_t ltid, bool snapshot_en, alpm_arg_t *arg, int max_levels);

extern void
bcmptm_rm_alpm_traverse_cache_invalidate(int u, int m,
                                         bcmltd_sid_t req_ltid,
                                         alpm_arg_t *arg, bool del);

extern int
bcmptm_rm_alpm_traverse_done(int u, int m,
                             bcmltd_sid_t req_ltid,
                             bool snapshot_en, alpm_arg_t *arg);

extern int
bcmptm_rm_alpm_traverse_init(int u, int m);

/*!
 * \brief Cleanup traverse snapshot info.
 *
 * Destroy the snapshot if not found.
 * \param [in] u Device u.
 *
 * \return nothing
 */
extern void
bcmptm_rm_alpm_traverse_cleanup(int u, int m);

#endif /* RM_ALPM_TRAVERSE_H */

